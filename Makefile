include Make.config
LINKOP	= +cpm -create-app -pragma-include:zpragma.inc -DAMALLOC2 
# this is a windows specific dir for cygwin use
#DESTDIR = ~/HostFileBdos/c/
DESTDIR = /cygdrive/c/users/lbmgm/HostFileBdos/c/
DESTDIR1 = /cygdrive/c/xampp/htdocs/
SUM = sum
CP = cp

all: telnetd ifconfig telnet ping pingnoti dig wget htlist myget ntp date https mac uptime today timezone wizreset digit 

digit: digit.o sysface.o libinet.o spi.o
	zcc  $(LINKOP) -odigit digit.o sysface.o libinet.o spi.o
	$(SUM) DIGIT.COM

digit.o: digit.c
	zcc $(CFLAGS) $(STORE) -c digit.c

wizreset: wizreset.o w5500.o spi.o wizchip_conf.o
	zcc  $(LINKOP) -owizreset wizreset.o w5500.o spi.o wizchip_conf.o
	$(SUM) WIZRESET.COM

wizreset.o: wizreset.c
	zcc $(CFLAGS) $(STORE) -c wizreset.c

telnetd: telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o
	zcc  $(LINKOP) -otelnetd telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o snaplib.c libinet.o libhost.o
	$(SUM) TELNETD.COM

telnet_server.o: telnet_server.c
	zcc $(CFLAGS) $(STORE) -c telnet_server.c

w5500.o: w5500.c w5500.h wizchip_conf.h
	zcc  $(CFLAGS) $(STORE) -c  w5500.c 

dhcp.o: dhcp.c dhcp.h
	zcc  $(CFLAGS) $(STORE)   -c  dhcp.c 

spi.o: spi.c spi.h
#	zcc  +cpm -pragma-include:zpragma.inc  $(STORE) -c  spi.c 
	zcc  $(CFLAGS)  $(STORE) -c  spi.c 

socket.o: socket.c socket.h
	zcc  $(CFLAGS)  $(STORE)  -c  socket.c 

ethernet.o: ethernet.c ethernet.h
	zcc  $(CFLAGS)  $(STORE)  -c  ethernet.c 

dns.o: dns.c dns.h
	zcc  $(CFLAGS) $(STORE) -DHOSTS -c  dns.c 

wizchip_conf.o: wizchip_conf.c wizchip_conf.h
	zcc  $(CFLAGS) $(STORE)  -c  wizchip_conf.c 

ifconfig: addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libhost.o libinet.o 
	zcc  $(LINKOP) -oifconfig -Wunused addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libhost.o libinet.o
	$(SUM) IFCONFIG.COM

libinet.o: libinet.c
	zcc $(CFLAGS) $(STORE) -c libinet.c

addrprint.o: addrprint.c
	zcc $(CFLAGS) $(STORE) -c addrprint.c

dig: dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libinet.o libhost.o
	zcc  $(LINKOP) -odig dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libinet.o libhost.o
	$(SUM) DIG.COM

dnsprint.o: dnsprint.c
	zcc $(CFLAGS) $(STORE) -c dnsprint.c

ping: ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o libinet.o libhost.o
	zcc  $(LINKOP) -oping ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm libinet.o libhost.o 
	$(SUM) PING.COM

ping.o: ping.c
	zcc $(CFLAGS) $(STORE) -c ping.c

time.o: time.c
	zcc $(CFLAGS) $(STORE) -c  time.c 

telnet: telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o sysface.o
	zcc $(LINKOP) -otelnet telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o sysface.o
	$(SUM) TELNET.COM

telnet_client.o: telnet_client.c
	zcc $(CFLAGS) $(STORE) -c telnet_client.c

myget: get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o
	zcc $(LINKOP) -omyget get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o
	$(SUM) MYGET.COM

htlist: htlist.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o
	zcc $(LINKOP) -ohtlist htlist.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o
	$(SUM) HTLIST.COM

htlist.o: htlist.c
	zcc $(CFLAGS) $(STORE) -c htlist.c

get.o: get.c
	zcc $(CFLAGS) $(STORE) -c get.c

ntp: ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o libinet.o libhost.o
	zcc $(LINKOP) -ontp ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o libinet.o libhost.o
	$(SUM) NTP.COM

ntp.o: ntp.c
	zcc $(CFLAGS) $(STORE) -c ntp.c

sysface.o: sysface.c
	zcc  $(CFLAGS) $(STORE) -c  sysface.c 

date: date.o sysface.o libinet.o spi.o
	zcc  $(LINKOP) -odate date.o sysface.o libinet.o spi.o
	$(SUM) DATE.COM

date.o: date.c
	zcc $(CFLAGS) $(STORE) -c date.c

wget: ethernet.o htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o libinet.o libhost.o
	zcc $(LINKOP) -owget htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o -lm libinet.o libhost.o
	$(SUM) WGET.COM

htget.o: htget.c
	zcc $(CFLAGS) $(STORE) -c htget.c

https: httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o
	zcc  $(LINKOP) -ohttps httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libinet.o libhost.o
	$(SUM) HTTPS.COM

httpServer.o: httpServer.c
	zcc $(CFLAGS) $(STORE) -c httpServer.c

httpParser.o: httpParser.c
	zcc  $(CFLAGS) $(STORE)  -c  httpParser.c 

pingnoti: pingn.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o libinet.o libhost.o
	zcc  $(LINKOP) -opingnoti pingn.o -DNOTIMER w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm libinet.o libhost.o
	$(SUM) PINGNOTI.COM

pingn.o: ping.c
	zcc $(CFLAGS) $(STORE) -DNOTIMER -c ping.c -o pingn.o

mac: mac.o libinet.o spi.o snaplib.o
	zcc  $(LINKOP) -omac mac.o libinet.o spi.o snaplib.o
	$(SUM) MAC.COM

snaplib.o:
	zcc  $(CFLAGS) $(STORE) -c snaplib.c

mac.o:
	zcc  $(CFLAGS) $(STORE) -c mac.c

libhost.o: libhost.c
	zcc $(CFLAGS) $(STORE) -c libhost.c

uptime: uptime.o sysface.o libinet.o spi.o
	zcc $(LINKOP) -ouptime uptime.o sysface.o libinet.o spi.o
	$(SUM) UPTIME.COM

uptime.o: uptime.c
	zcc $(CFLAGS) $(STORE) -c uptime.c

today: today.o sysface.o libinet.o spi.o
	zcc  $(LINKOP) -otoday today.o sysface.o libinet.o spi.o
	$(SUM) TODAY.COM

today.o: today.c
	zcc $(CFLAGS) $(STORE) -c today.c

timezone: timezone.o sysface.o libinet.o spi.o
	zcc $(LINKOP) -otimezone timezone.o sysface.o libinet.o spi.o
	$(SUM) TIMEZONE.COM

timezone.o: timezone.c
	zcc $(CFLAGS) $(STORE) -c timezone.c

clean:
	$(RM) *.o *.err *.lis *.def *.lst *.sym *.exe *.COM  driver ifconfig dnsprnt ping dig ftp telnet wget myget get ctc ntp date https clock pingnoti telnetd mac timezone today uptime test *.map
	rm -rf html irc latex email

install:
	cp ./*.COM $(DESTDIR1)/. 

check:
	$(SUM) *.COM

ship:
	rm -rf irc latex email *.o *.err *.lis *.def *.lst *.sym *.exe driver dnsprnt get ctc ntp myget 
	rm -rf test ctc ntp wget telnet ping dig ifconfig date https timezone today uptime test 

documents:
	doxygen 
