/* read and set the time zone field in the nvram 

NOTE:  at this time no checking is done on the time zone
information.  You type it we store it.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysface.h"

long zone_off;

typedef struct tz
{
  long offset;
  char *name;
  char *text;
} TZ;

extern struct tz BigData[];
extern void GetZone ();
extern void GetZoneT ();
char *text1;
char *text2;

void
main (int argc, char *argv[])
{
  if (argc <= 1)
    {
	text1 = GetTZtxt();
      GetZoneT ();
      printf ("Your offset is UTC %ld seconds %s %s\n", zone_off, text1,
	      text2);
    }
  else
    {
      text1 = argv[1];
      GetZoneT ();
      printf ("Your offset is UTC %ld seconds %s %s\n", zone_off, text1,
	      text2);
      SetTZ (zone_off);
	SetTZtxt(text1);
    }
}

void
GetZone ()
{
  int i;
  for (i = 0; BigData[i].text; i++)
    {
      if (zone_off == BigData[i].offset)
	{
	  text1 = BigData[i].name;
	  text2 = BigData[i].text;
	  return;
	}
    }
  text1 = text2 = NULL;
}

void
GetZoneT ()
{
  int i;
  char *p;
  p = strchr (text1, '\n');
  if (p)
    *p = '\0';
  p = strchr (text1, '\r');
  if (p)
    *p = '\0';
  for (i = 0; BigData[i].name; i++)
    {
      if (strstr (BigData[i].name, text1))
	{
	  text2 = BigData[i].text;
	  zone_off = BigData[i].offset;
	  return;
	}
    }
  zone_off = text1 = text2 = NULL;
}

struct tz BigData[] = {
  {0, "GMT", "GREENWICH MEAN TIME"},
  {0, "WET", "WESTERN EUROPEAN TIME"},
  {0, "WT", "WESTERN SAHARA STANDARD TIME"},
  {10800, "ADT", "ARABIA DAYLIGHT TIME"},
  {-10800, "ADT", "ATLANTIC DAYLIGHT TIME"},
  {-10800, "AMST", "AMAZON SUMMER TIME"},
  {-10800, "ART", "ARGENTINA TIME"},
  {10800, "AST", "ARAB STANDARD TIME"},
  {-10800, "BRT", "BRASILIA TIME"},
  {-10800, "CLST", "CHILE SUMMER TIME"},
  {10800, "EAT", "EAST AFRICA TIME"},
  {10800, "EEDT", "EASTERN EUROPE DAYLIGHT TIME"},
  {10800, "EEST", "EASTERN EUROPE SUMMER TIME"},
  {-10800, "FKST", "FALKLAND ISLANDS SUMMER TIME"},
  {-10800, "GFT", "FRENCH GUIANA TIME"},
  {10800, "IDT", "ISRAEL DAYLIGHT TIME"},
  {10800, "MSK", "MOSCOW STANDARD TIME"},
  {-10800, "PYST", "PARAGUAY SUMMER TIME"},
  {-10800, "SRT", "SURINAME TIME"},
  {10800, "TRT", "TURKEY TIME OR TURKISH TIME"},
  {-10800, "UYT", "URUGUAY STANDARD TIME"},
  {12600, "IRST", "IRAN STANDARD TIME"},
  {-14400, "AMT", "AMAZON TIME"},
  {14400, "AMT", "ARMENIA TIME"},
  {-14400, "AST", "ATLANTIC STANDARD TIME"},
  {14400, "AZT", "AZERBAIJAN TIME"},
  {-14400, "BOT", "BOLIVIA TIME"},
  {-14400, "CDT", "CUBA DAYLIGHT TIME"},
  {-14400, "CLT", "CHILE STANDARD TIME"},
  {-14400, "EDT", "EASTERN DAYLIGHT TIME"},
  {-14400, "FKT", "FALKLAND ISLANDS TIME"},
  {14400, "GET", "GEORGIA STANDARD TIME"},
  {14400, "GST", "GULF STANDARD TIME"},
  {-14400, "GYT", "GUYANA TIME"},
  {14400, "MUT", "MAURITIUS TIME"},
  {-14400, "PYT", "PARAGUAY TIME"},
  {14400, "RET", "REUNION TIME"},
  {14400, "SAMT", "SAMARA TIME"},
  {14400, "SCT", "SEYCHELLES TIME"},
  {-14400, "VET", "VENEZUELAN STANDARD TIME"},
  {14400, "VOLT", "VOLGOGRAD TIME"},
  {16200, "AFT", "AFGHANISTAN TIME"},
  {16200, "IRDT", "IRAN DAYLIGHT TIME"},
  {-18000, "ACT", "ACRE TIME"},
  {18000, "AMST", "ARMENIA SUMMER TIME"},
  {18000, "AQTT", "AQTOBE TIME (Kazakhstan)"},
  {18000, "AZST", "AZERBAIJAN SUMMER TIME"},
  {-18000, "CDT", "CENTRAL DAYLIGHT TIME"},
  {-18000, "COT", "COLUMBIA TIME"},
  {-18000, "CST", "CUBA STANDARD TIME"},
  {-18000, "EASST", "EASTERN ISLAND SUMMER TIME"},
  {-18000, "ECT", "ECUADOR TIME"},
  {-18000, "EST", "EASTERN STANDARD TIME"},
  {18000, "MVT", "MALDIVES TIME"},
  {18000, "ORAT", "ORAL TIME"},
  {-18000, "PET", "PERU TIME"},
  {18000, "PKT", "PAKISTAN STANDARD TIME"},
  {18000, "TFT", "FRENCH SOUTHERN AND ANTARCTIC TERRITORIES TIME"},
  {18000, "TJT", "TAJIKISTAN TIME"},
  {18000, "TMT", "TURKMENISTAN TIME"},
  {18000, "UZT", "UZBEKISTAN TIME"},
  {18000, "YEKT", "YEKATERINBURG TIME"},
  {19800, "IST", "INDIA STANDARD TIME"},
  {20700, "NPT", "NEPAL TIME"},
  {21600, "ALMT", "ALMA-ATA TIME"},
  {21600, "BDT", "BANGLADESH TIME (also see BST)"},
  {21600, "BST", "BANGLADESH STANDARD TIME (also see BDT)"},
  {21600, "BTT", "BHUTAN TIME"},
  {-21600, "CST", "CENTRAL STANDARD TIME"},
  {-21600, "EAST", "EASTERN ISLAND STANDARD TIME"},
  {-21600, "GALT", "GALAPAGOS TIME"},
  {21600, "KGT", "KYRGYZSTAN TIME"},
  {-21600, "MDT", "MOUNTAIN DAYLIGHT TIME"},
  {21600, "NOVT", "NOVOSIBIRSK TIME"},
  {21600, "OMST", "OMSK STANDARD TIME"},
  {21600, "QYZT", "QYZYLORDA TIME (Kazakhstan)"},
  {23400, "MMT", "MYANMAR TIME"},
  {25200, "CXT", "CHRISTMAS ISLAND TIME"},
  {25200, "HOVT", "HOVD TIME (Mongolia)"},
  {25200, "ICT", "INDOCHINA TIME"},
  {25200, "KRAT", "KRASNOYARSK TIME"},
  {-25200, "MST", "MOUNTAIN STANDARD TIME"},
  {-25200, "PDT", "PACIFIC DAYLIGHT TIME"},
  {25200, "WIB", "WESTERN INDONESIAN TIME"},
  {-25200, "YDT", "YUKON DAYLIGHT TIME"},
  {-28800, "AKDT", "ALASKA DAYLIGHT TIME"},
  {28800, "AWST", "AUSTRALIAN WESTERN STANDARD TIME"},
  {28800, "BNT", "BRUNEI TIME"},
  {28800, "CHOT", "CHOIBALSAN TIME (Mongolia)"},
  {28800, "CST", "CHINA STANDARD TIME"},
  {28800, "CT", "CHINA TIME (Macau)"},
  {28800, "HKT", "HONG KONG TIME"},
  {28800, "IRKT", "IRKUTSK TIME"},
  {28800, "KRAST", "KRASNOYARSK SUMMER TIME"},
  {28800, "MYT", "MALAYSIA TIME"},
  {28800, "PHT", "PHILIPPINE TIME"},
  {-28800, "PST", "PACIFIC STANDARD TIME"},
  {-28800, "PST", "PITCAIRN TIME"},
  {28800, "SGT", "SINGAPORE TIME"},
  {28800, "ULAT", "ULAANBAATAR TIME"},
  {28800, "WITA", "CENTRAL INDONESIAN TIME"},
  {28800, "WST", "WESTERN STANDARD TIME"},
  {-28800, "YST", "YUKON STANDARD TIME"},
  {-30600, "MART", "MARQUESAS TIME"},
  {30600, "PST", "PYONGYANG TIME (North Korea)"},
  {31500, "CWST", "CENTRAL WESTERN AUSTRALIA TIME (Eucla)"},
  {-32400, "AKST", "ALASKA TIME"},
  {32400, "AWDT", "AUSTRALIAN WESTERN DAYLIGHT TIME"},
  {32400, "CHOST", "CHOIBALSAN SUMMER TIME (Mongolia)"},
  {-32400, "GAMT", "GAMBIER TIME"},
  {-32400, "HADT", "HAWAII-ALEUTIAN DAYLIGHT TIME"},
  {32400, "IRKST", "IRKUTSK SUMMER TIME"},
  {32400, "JST", "JAPAN STANDARD TIME"},
  {32400, "KST", "KOREA STANDARD TIME"},
  {32400, "SBT", "SOLOMON ISLANDS TIME"},
  {32400, "TLT", "EAST TIMOR TIME (Timor-Leste Time)"},
  {32400, "WDT", "WESTERN DAYLIGHT TIME"},
  {32400, "WIT", "EASTERN INDONESIAN TIME"},
  {32400, "YAKT", "YAKUTSK TIME"},
  {34200, "ACST", "AUSTRALIAN CENTRAL STANDARD TIME"},
  {34200, "CST", "CENTRAL STANDARD TIME"},
  {36000, "AEST", "AUSTRALIAN EASTERN STANDARD TIME"},
  {36000, "CHsT", "CHAMORRO STANDARD TIME"},
  {36000, "CHST", "CHAMORRO STANDARD TIME"},
  {36000, "CHUT", "CHUUK TIME"},
  {-36000, "CKT", "COOK ISLAND TIME"},
  {36000, "EST", "EASTERN STANDARD TIME"},
  {-36000, "HAST", "HAWAII-ALEUTIAN STANDARD TIME"},
  {-36000, "HST", "HAWAII STANDARD TIME"},
  {36000, "PGT", "PAPUA NEW GUINEA TIME"},
  {-36000, "SDT", "SAMOA DAYLIGHT TIME"},
  {-36000, "TAHT", "TAHITI TIME"},
  {36000, "TRUT", "TRUK TIME (Micronesia)"},
  {36000, "VLAT", "VLADIVOSTOK TIME"},
  {36000, "YAP", "YAP TIME (Micronesia)"},
  {3600, "CET", "CENTRAL EUROPEAN TIME"},
  {-3600, "CVT", "CAPE VERDE TIME"},
  {3600, "IST", "IRISH SUMMER TIME"},
  {3600, "WAT", "WEST AFRICA TIME"},
  {3600, "WEST", "WESTERN EUROPEAN SUMMER TIME"},
  {3600, "WST", "WESTERN SAHARA SUMMER TIME"},
  {37800, "ACDT", "AUSTRALIAN CENTRAL DAYLIGHT TIME"},
  {37800, "CDT", "CENTRAL DAYLIGHT TIME"},
  {37800, "LHST", "LORD HOWE STANDARD TIME"},
  {39600, "AEDT", "AUSTRALIAN EASTERN DAYLIGHT TIME"},
  {39600, "ANAT", "ANADYR TIME (Russia)"},
  {39600, "EDT", "EASTERN DAYLIGHT TIME"},
  {39600, "KOST", "KOSRAE TIME (Micronesia)"},
  {39600, "LHDT", "LORD HOWE DAYLIGHT TIME"},
  {39600, "MAGT", "MAGADAN TIME"},
  {39600, "NCT", "NEW CALEDONIA TIME"},
  {39600, "NFT", "NORFOLK TIME"},
  {-39600, "NUT", "NIUE TIME"},
  {39600, "PONT", "PONAPE TIME (Micronesia)"},
  {39600, "SAKT", "SAKHALIN TIME"},
  {39600, "SRET", "SREDNEKOLYMSK TIME"},
  {-39600, "SST", "SAMOA STANDARD TIME"},
  {39600, "VUT", "VANUATU TIME"},
  {43200, "FJT", "FIJI TIME"},
  {43200, "GILT", "GILBERT ISLAND TIME"},
  {43200, "MHT", "MARSHALL ISLANDS TIME"},
  {43200, "NRT", "NAURU TIME"},
  {43200, "NZST", "NEW ZEALAND STANDARD TIME"},
  {43200, "PETT", "KAMCHATKA TIME"},
  {43200, "TVT", "TUVALU TIME"},
  {43200, "WAKT", "WAKE ISLAND TIME (Northern Pacific Ocean)"},
  {43200, "WFT", "WALLIS AND FUTUNA TIME"},
  {45900, "CHAST", "CHATHAM ISLAND STANDARD TIME"},
  {46800, "FJST", "FIJI SUMMER TIME"},
  {46800, "NZDT", "NEW ZEALAND DAYLIGHT TIME"},
  {46800, "PHOT", "PHOENIX ISLAND TIME"},
  {46800, "TKT", "TOKELAU TIME"},
  {46800, "TOT", "TONGA TIME"},
  {46800, "WST", "WESTERN SAMOA TIME (standard time)"},
  {49500, "CHADT", "CHATHAM ISLAND DAYLIGHT TIME"},
  {50400, "LINT", "LINE ISLANDS TIME"},
  {50400, "WST",
   "WESTERN SAMOA TIME (*also used during daylight saving time)"},
  {-5400, "NDT", "NEWFOUNDLAND DAYLIGHT SAVING TIME"},
  {-7200, "BRST", "BRASILIA SUMMER TIME"},
  {7200, "CAT", "CENTRAL AFRICA TIME"},
  {7200, "CEST", "CENTRAL EUROPEAN SUMMER TIME"},
  {7200, "EET", "EASTERN EUROPEAN TIME"},
  {-7200, "FNT", "FERNANDO de NORONHA"},
  {7200, "IST", "ISRAEL STANDARD TIME"},
  {7200, "SAST", "SOUTH AFRICA STANDARD TIME"},
  {-7200, "UYST", "URUGUAY SUMMER TIME"},
  {7200, "WAST", "WEST AFRICA SUMMER TIME"},
  {-9000, "NST", "NEWFOUNDLAND STANDARD TIME"},
  {0, 0, 0}
};
