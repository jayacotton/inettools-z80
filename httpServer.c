#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "socket.h"
#include "wizchip_conf.h"

#include "httpServer.h"
#include "httpParser.h"
#include "httpUtil.h"
#include "ethernet.h"
#include "time.h"
#include "ctc.h"
#include "trace.h"

#ifdef	_USE_SDCARD_
#include "ff.h"
#else
#include <stdio.h>
#include <cpm.h>
#endif
wiz_NetInfo gWIZNETINFO;
uint8_t run_user_applications;

#define DATA_BUF_SIZE	256	
#define MAX_HTTPSOCK 6
uint8_t socknumlist[] = { 2, 3, 4, 5, 6, 7 };

uint8_t buffer[256];
unsigned char mac[6] = { 0x98, 0x76, 0xb6, 0x11, 0x00, 0xc4 };

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static uint8_t HTTPSock_Num[_WIZCHIP_SOCK_NUM_] = { 0, };

static st_http_request *http_request;		/**< Pointer to received HTTP request */
static st_http_request *parsed_http_request;	/**< Pointer to parsed HTTP request */
static uint8_t *http_response;			/**< Pointer to HTTP response */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
uint8_t *pHTTP_TX;
uint8_t *pHTTP_RX;

volatile uint32_t httpServer_tick_1s = 0;
st_http_socket HTTPSock_Status[_WIZCHIP_SOCK_NUM_] = { {STATE_HTTP_IDLE,}, };

FILE *file;
uint8_t filename[14];
uint8_t fr;

/*****************************************************************************
 * Private functions
 ****************************************************************************/
void httpServer_Sockinit (uint8_t cnt, uint8_t * socklist);
static uint8_t getHTTPSocketNum (uint8_t seqnum);
static int8_t getHTTPSequenceNum (uint8_t socket);
static int8_t http_disconnect (uint8_t sn);

static void http_process_handler (uint8_t s,
				  st_http_request * p_http_request);
static void send_http_response_header (uint8_t s, uint8_t content_type,
				       uint32_t body_len,
				       uint16_t http_status);
static void send_http_response_body (uint8_t s, uint8_t * uri_name,
				     uint8_t * buf, uint32_t start_addr,
				     uint32_t file_len);
static void send_http_response_cgi (uint8_t s, uint8_t * buf,
				    uint8_t * http_body, uint16_t file_len);

/*****************************************************************************
 * Public functions
 ****************************************************************************/
// Callback functions definition: MCU Reset / WDT Reset
void
default_mcu_reset (void)
{;
}

void
default_wdt_reset (void)
{;
}

void (*HTTPServer_ReStart) (void) = default_mcu_reset;
void (*HTTPServer_WDT_Reset) (void) = default_wdt_reset;

void
httpServer_Sockinit (uint8_t cnt, uint8_t * socklist)
{
  uint8_t i;

  for (i = 0; i < cnt; i++)
    {
      // Mapping the H/W socket numbers to the sequential index numbers
      HTTPSock_Num[i] = socklist[i];
    }
}

void
main ()
{
  int i;

  TRACE ("");
#ifdef NOTNOW
  i = ctc_vet ();		/* turns out we don't care */
  if (i == CPM)
    printf ("Running on CPM\n");
  else
    printf ("Running on ZSDOS\n");

  ctc_init ();			/* set up and start ctc timer */
  time_init ();			/* setup timer code */
#endif


  if (Ethernet_begin (mac) == 0)
    {
      if (Ethernet_hardwareStatus () == EthernetNoHardware)
	printf ("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus () == LinkOFF)
	printf ("Plug in the cable\n");
      exit (0);
    }
  TRACE ("");
  Ethernet_localIP (gWIZNETINFO.ip);
  Ethernet_localDNS (gWIZNETINFO.dns);
  TRACE ("");
  httpServer_init (TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);
  while (1)
    {
      httpServer_run (0);
      if (bdos (CPM_ICON, 0))
	exit (0);
    }
}

static uint8_t
getHTTPSocketNum (uint8_t seqnum)
{
  // Return the 'H/W socket number' corresponding to the index number
  return HTTPSock_Num[seqnum];
}

static int8_t
getHTTPSequenceNum (uint8_t socket)
{
  uint8_t i;

  for (i = 0; i < _WIZCHIP_SOCK_NUM_; i++)
    if (HTTPSock_Num[i] == socket)
      return i;

  return -1;
}


void
httpServer_init (uint8_t * tx_buf, uint8_t * rx_buf, uint8_t cnt,
		 uint8_t * socklist)
{
  // User's shared buffer
  pHTTP_TX = tx_buf;
  pHTTP_RX = rx_buf;

  // H/W Socket number mapping
  httpServer_Sockinit (cnt, socklist);
}


/* Register the call back functions for HTTP Server */
void
reg_httpServer_cbfunc (void (*mcu_reset) (void), void (*wdt_reset) (void))
{
  // Callback: HW Reset and WDT reset function for each MCU platforms
  if (mcu_reset)
    HTTPServer_ReStart = mcu_reset;
  if (wdt_reset)
    HTTPServer_WDT_Reset = wdt_reset;
}


void
httpServer_run (uint8_t seqnum)
{
  uint8_t s;			// socket number
  int16_t len;
  //uint32_t gettime = 0; // 20150828 ## Eric removed

  uint8_t ret = 0;

  TRACE ("");
  http_request = (st_http_request *) pHTTP_RX;	// Structure of HTTP Request
  parsed_http_request = (st_http_request *) pHTTP_TX;

  // Get the H/W socket number
  s = getHTTPSocketNum (seqnum);

  /* HTTP Service Start */
  switch (getSn_SR (s))
    {
    case SOCK_ESTABLISHED:
      TRACE ("");
      // Interrupt clear
      if (getSn_IR (s) & Sn_IR_CON)
	{
	  setSn_IR (s, Sn_IR_CON);
	}

      // HTTP Process states
      switch (HTTPSock_Status[seqnum].sock_status)
	{

	case STATE_HTTP_IDLE:
	  TRACE ("");
	  if ((len = getSn_RX_RSR (s)) > 0)
	    {
	      if (len > DATA_BUF_SIZE)
		len = DATA_BUF_SIZE;

	      TRACE ("");
	      if ((len = recv (s, (uint8_t *) http_request, len)) < 0)
		break;		// Exception handler

	      TRACE ("");
	      //////////////////////////////////////////////////////////////////////////////
	      // Todo; User defined custom command handler (userHandler.c)
	      //ret = custom_command_handler ((uint8_t *) http_request);
	      //////////////////////////////////////////////////////////////////////////////
	      ret = 0;
	      if (ret > 0)	// Custom command handler
		{
		  // Todo: Users can change this parts for custom function added
		  //if(ret == COMMAND_SUCCESS)            send(s, (uint8_t *)"CMDOK", 5);
		  //else if(ret == COMMAND_ERROR) send(s, (uint8_t *)"CMDERROR", 8);

		  HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE;
		}
	      else		// HTTP process handler
		{
		  *(((uint8_t *) http_request) + len) = '\0';	// End of string (EOS) marker
		  TRACE ("");
		  parse_http_request (parsed_http_request,
				      (uint8_t *) http_request);

		  // HTTP 'response' handler; includes send_http_response_header / body function
		  http_process_handler (s, parsed_http_request);

		  if (HTTPSock_Status[seqnum].file_len > 0)
		    HTTPSock_Status[seqnum].sock_status =
		      STATE_HTTP_RES_INPROC;
		  else
		    HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE;	// Send the 'HTTP response' end
		}
	    }
	  break;


	case STATE_HTTP_RES_INPROC:
	  /* Repeat: Send the remain parts of HTTP responses */
	  // Repeatedly send remaining data to client
	  TRACE ("");
	  send_http_response_body (s, 0, http_response, 0, 0);

	  if (HTTPSock_Status[seqnum].file_len == 0)
	    HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE;
	  break;

	case STATE_HTTP_RES_DONE:
	  // Socket file info structure re-initialize
	  TRACE ("");
	  HTTPSock_Status[seqnum].file_len = 0;
	  HTTPSock_Status[seqnum].file_offset = 0;
	  HTTPSock_Status[seqnum].file_start = 0;
	  HTTPSock_Status[seqnum].sock_status = STATE_HTTP_IDLE;

	  fclose (file);
#ifdef _USE_WATCHDOG_
	  HTTPServer_WDT_Reset ();
#endif
	  http_disconnect (s);
	  break;

	default:
	  TRACE ("");
	  break;
	}
      break;

    case SOCK_CLOSE_WAIT:
      TRACE ("");
      // Socket file info structure re-initialize
      HTTPSock_Status[seqnum].file_len = 0;
      HTTPSock_Status[seqnum].file_offset = 0;
      HTTPSock_Status[seqnum].file_start = 0;
      HTTPSock_Status[seqnum].sock_status = STATE_HTTP_IDLE;

      http_disconnect (s);
      break;

    case SOCK_INIT:
      TRACE ("");
      listen (s);
      break;

    case SOCK_LISTEN:
      TRACE ("");
      break;

    case SOCK_SYNSENT:
      TRACE ("");
      //case SOCK_SYNSENT_M:
    case SOCK_SYNRECV:
      TRACE ("");
      //case SOCK_SYNRECV_M:
      break;

    case SOCK_CLOSED:
      TRACE ("");
      if (socket (s, Sn_MR_TCP, HTTP_SERVER_PORT, 0x00) == s)	/* Reinitialize the socket */
	{
	}
      break;
    default:
      TRACE ("");
      break;
    }				// end of switch

#ifdef _USE_WATCHDOG_
  HTTPServer_WDT_Reset ();
#endif
}

////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////
static void
send_http_response_header (uint8_t s, uint8_t content_type, uint32_t body_len,
			   uint16_t http_status)
{
  switch (http_status)
    {
    case STATUS_OK:		// HTTP/1.1 200 OK
      if ((content_type != PTYPE_CGI) && (content_type != PTYPE_XML))
	// CGI/XML type request does not respond HTTP header
	{
	  make_http_response_head ((char *) http_response, content_type,
				   body_len);
	}
      else
	{
	  // CGI/XML type request does not respond HTTP header to client
	  http_status = 0;
	}
      break;
    case STATUS_BAD_REQ:	// HTTP/1.1 400 OK
      memcpy (http_response, ERROR_REQUEST_PAGE, sizeof (ERROR_REQUEST_PAGE));
      break;
    case STATUS_NOT_FOUND:	// HTTP/1.1 404 Not Found
      memcpy (http_response, ERROR_HTML_PAGE, sizeof (ERROR_HTML_PAGE));
      break;
    default:
      break;
    }

  // Send the HTTP Response 'header'
  if (http_status)
    {
      send (s, http_response, strlen ((char *) http_response));
    }
}

static void
send_http_response_body (uint8_t s, uint8_t * uri_name, uint8_t * buf,
			 uint32_t start_addr, uint32_t file_len)
{
  int8_t get_seqnum;
  uint32_t send_len;
  uint8_t p;
  uint8_t flag_datasend_end = 0;

  uint32_t addr = 0;

  if ((get_seqnum = getHTTPSequenceNum (s)) == -1)
    return;			// exception handling; invalid number

  // Send the HTTP Response 'body'; requested file
  if (!HTTPSock_Status[get_seqnum].file_len)	// ### Send HTTP response body: First part ###
    {
      // Send process end
      send_len = file_len;
    }
  else				// ### Send HTTP response body: Remained parts ###
    {
      memset (filename, 0, 14);
      strcat (filename, strupr (HTTPSock_Status[get_seqnum].file_name));
      p = strlen (filename);
      if (p)
	{
	  filename[p - 1] = '\0';
	}
      printf ("name1 %s\n", filename);
      if ((file = fopen ((const char *) filename, "r")))
	{
	  fsetpos (file, HTTPSock_Status[get_seqnum].file_offset);
	}
      else
	{
	  send_len = 0;
	}

    }

  send_len = DATA_BUF_SIZE - 1;
	memset(buffer,0,DATA_BUF_SIZE);
  printf ("send_len = %ld\n", send_len);
  fr = fread (buffer, send_len, 1, file);
  if (fr == 0)
    {
      send_len = 0;
    }
  else
    {
      send_len = fr;
//      buffer[send_len + 1] = '\0';
    }

  // Requested content send to HTTP client

  if (send_len)
    send (s, buf, send_len);
  else
    flag_datasend_end = 1;

  if (flag_datasend_end)
    {
      HTTPSock_Status[get_seqnum].file_start = 0;
      HTTPSock_Status[get_seqnum].file_len = 0;
      HTTPSock_Status[get_seqnum].file_offset = 0;
      flag_datasend_end = 0;
    }
  else
    {
      HTTPSock_Status[get_seqnum].file_offset += send_len;
    }
  fclose (file);
  file = 0;
}

static void
send_http_response_cgi (uint8_t s, uint8_t * buf, uint8_t * http_body,
			uint16_t file_len)
{
  uint16_t send_len = 0;

  send_len =
    sprintf ((char *) buf, "%s%d\r\n\r\n%s", RES_CGIHEAD_OK, file_len,
	     http_body);

  send (s, buf, send_len);
}


static int8_t
http_disconnect (uint8_t sn)
{
  setSn_CR (sn, Sn_CR_DISCON);
  /* wait to process the command... */
  while (getSn_CR (sn));

  return SOCK_OK;
}


static void
http_process_handler (uint8_t s, st_http_request * p_http_request)
{
  uint8_t *uri_name;
  uint32_t content_addr = 0;
  uint32_t file_len = 0;
  uint8_t p;
  uint8_t post_name[32] = { 0x00, };	// POST method request file name

  uint8_t uri_buf[MAX_URI_SIZE] = { 0x00, };

  uint16_t http_status;
  int8_t get_seqnum;
  uint8_t content_found;

  if ((get_seqnum = getHTTPSequenceNum (s)) == -1)
    return;			// exception handling; invalid number

  http_status = 0;
  http_response = pHTTP_RX;
  file_len = 0;

  // HTTP Method Analyze
  switch (p_http_request->METHOD)
    {
    case METHOD_ERR:
      http_status = STATUS_BAD_REQ;
      send_http_response_header (s, 0, 0, http_status);
      break;

    case METHOD_HEAD:
    case METHOD_GET:
      if (!get_http_uri_name (p_http_request->URI, uri_buf))
	{
	  send_http_response_header (s, p_http_request->TYPE, 0,
				     STATUS_NOT_FOUND);
	  break;
	}
      uri_name = uri_buf;
      if (!strcmp ((char *) uri_name, "/"))
	strcpy ((char *) uri_name, INITIAL_WEBPAGE);	// If URI is "/", respond by index.html
      if (!strcmp ((char *) uri_name, "m"))
	strcpy ((char *) uri_name, M_INITIAL_WEBPAGE);
      if (!strcmp ((char *) uri_name, "mobile"))
	strcpy ((char *) uri_name, MOBILE_INITIAL_WEBPAGE);
      find_http_uri_type (&p_http_request->TYPE, uri_name);	// Checking requested file types (HTML, TEXT, GIF, JPEG and Etc. are included)

      if (p_http_request->TYPE == PTYPE_CGI)
	{
	  content_found =
	    http_get_cgi_handler (uri_name, pHTTP_TX, &file_len);
	  if (content_found
	      && (file_len <= (2048 - (strlen (RES_CGIHEAD_OK) + 8))))
	    {
	      send_http_response_cgi (s, http_response, pHTTP_TX,
				      (uint16_t) file_len);
	    }
	  else
	    {
	      send_http_response_header (s, PTYPE_CGI, 0, STATUS_NOT_FOUND);
	    }
	}
      else
	{			// If No CGI request, Try to find The requested web content in storage (e.g., 'SD card' or 'Data flash')
	  memset (filename, 0, 14);
	  //strcat(filename,"C:");
	  strcat (filename, strupr (uri_name));
	  p = strlen (filename);
	  if (p)
	    {
	      filename[p - 1] = '\0';
	    }
	  printf ("name2 %s\n", filename);
	  if ((file = fopen (filename, "r")))
	    {
	      content_found = 1;	// file open succeed
	      printf ("file open\n");
//fixme
	      file_len = 3200;
	      printf ("file len %ld\n", file_len);
//		fclose(file);
//		file = 0;
	    }
	  else
	    {
	      content_found = 0;	// file open failed
	    }
	  if (!content_found)
	    {
	      http_status = STATUS_NOT_FOUND;
	    }
	  else
	    {
	      http_status = STATUS_OK;
	    }

	  // Send HTTP header
	  if (http_status)
	    {
	      send_http_response_header (s, p_http_request->TYPE, file_len,
					 http_status);
	    }

	  // Send HTTP body (content)
	  if (http_status == STATUS_OK)
	    {
	      send_http_response_body (s, uri_name, http_response,
				       content_addr, file_len);
	    }
	}
      break;

    case METHOD_POST:
      mid ((char *) p_http_request->URI, "/", " HTTP", (char *) post_name);
      uri_name = post_name;
      find_http_uri_type (&p_http_request->TYPE, uri_name);	//Check file type (HTML, TEXT, GIF, JPEG are included)

      if (p_http_request->TYPE == PTYPE_CGI)	// HTTP POST Method; CGI Process
	{
	  content_found =
	    http_post_cgi_handler (post_name, p_http_request, http_response,
				   &file_len);
	  if (content_found
	      && (file_len <= (2048 - (strlen (RES_CGIHEAD_OK) + 8))))
	    {
	      send_http_response_cgi (s, pHTTP_TX, http_response,
				      (uint16_t) file_len);

	      // Reset the H/W for apply to the change configuration information
	      if (content_found == HTTP_RESET)
		HTTPServer_ReStart ();
	    }
	  else
	    {
	      send_http_response_header (s, PTYPE_CGI, 0, STATUS_NOT_FOUND);
	    }
	}
      else			// HTTP POST Method; Content not found
	{
	  send_http_response_header (s, 0, 0, STATUS_NOT_FOUND);
	}
      break;

    default:
      http_status = STATUS_BAD_REQ;
      send_http_response_header (s, 0, 0, http_status);
      break;
    }
}

void
httpServer_time_handler (void)
{
  httpServer_tick_1s++;
}

uint32_t
get_httpServer_timecount (void)
{
  return httpServer_tick_1s;
}

uint8_t
http_get_cgi_handler (uint8_t * name, uint8_t * buf, uint32_t * file_len)
{
}

uint8_t
http_post_cgi_handler (uint8_t * uri_name,
		       st_http_request * p_http_request,
		       uint8_t * buf, uint32_t * file_len)
{
}
