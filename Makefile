#CFLAGS	= +cpm --list --c-code-in-asm  -Wall -pragma-include:zpragma.inc 
#LINKOP	= +cpm -Cl -v -create-app -m -pragma-include:zpragma.inc -DAMALLOC2
CFLAGS	= +cpm -Wall -pragma-include:zpragma.inc 
LINKOP	= +cpm -create-app -pragma-include:zpragma.inc -DAMALLOC2
# configure the inet storage option
STORE = -DFRAM
#STORE = -DDISK 
#STORE = -DNVRAM 
# this is a windows specific dir for cygwin use
DESTDIR = ~/projects/cpnet-z80/contrib/HostFileBdos/c/
DESTDIR1 = ~/projects/cpnet-z80/contrib/HostFileBdos/c/
# this is a linux/unix path
#DESTDIR = ~/HostFileBdos/c/
CP = cp

all: telnetd ifconfig telnet ping pingnoti dig wget myget ntp date https mac uptime today timezone wizreset digit 

digit: digit.o sysface.o libfram.o spi.o
	zcc  $(LINKOP) -odigit digit.o sysface.o libfram.o spi.o
	$(CP) DIGIT.COM $(DESTDIR)digit.com

digit.o: digit.c
	zcc $(CFLAGS) $(STORE) -c digit.c

wizreset: wizreset.o w5500.o spi.o wizchip_conf.o
	zcc  $(LINKOP) -owizreset wizreset.o w5500.o spi.o wizchip_conf.o
	$(CP) WIZRESET.COM $(DESTDIR)wizreset.com

wizreset.o: wizreset.c
	zcc $(CFLAGS) $(STORE) -c wizreset.c

telnetd: telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o libhost.o
	zcc  $(LINKOP) -otelnetd telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o snaplib.c libfram.o libhost.o
	$(CP) TELNETD.COM $(DESTDIR)telnetd.com

telnet_server.o: telnet_server.c
	zcc $(CFLAGS) $(STORE) -c telnet_server.c

w5500.o: w5500.c w5500.h wizchip_conf.h
	zcc  $(CFLAGS) $(STORE) -c  w5500.c 

dhcp.o: dhcp.c dhcp.h
	zcc  $(CFLAGS) $(STORE)   -c  dhcp.c 

spi.o: spi.c spi.h
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
	$(CP) IFCONFIG.COM $(DESTDIR)ifconfig.com

libinet.o: libinet.c
	zcc $(CFLAGS) $(STORE) -c libinet.c

addrprint.o: addrprint.c
	zcc $(CFLAGS) $(STORE) -c addrprint.c

dig: dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libfram.o libhost.o
	zcc  $(LINKOP) -odig dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libfram.o libhost.o
	$(CP) DIG.COM $(DESTDIR)dig.com

dnsprint.o: dnsprint.c
	zcc $(CFLAGS) $(STORE) -c dnsprint.c

ping: ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o libfram.o libhost.o
	zcc  $(LINKOP) -oping ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm libfram.o libhost.o 
	$(CP) PING.COM $(DESTDIR)ping.com

ping.o: ping.c
	zcc $(CFLAGS) $(STORE) -c ping.c

time.o: time.c
	zcc $(CFLAGS) $(STORE) -c  time.c 

telnet: telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o libhost.o
	zcc $(LINKOP) -otelnet telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o libhost.o
	$(CP) TELNET.COM $(DESTDIR)telnet.com

telnet_client.o: telnet_client.c
	zcc $(CFLAGS) $(STORE) -c telnet_client.c

myget: get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o libhost.o
	zcc $(LINKOP) -omyget get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o libhost.o
	cp MYGET.COM $(DESTDIR)myget.com

get.o: get.c
	zcc $(CFLAGS) $(STORE) -c get.c

ntp: ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o libfram.o libhost.o
	zcc $(LINKOP) -ontp ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o libfram.o libhost.o
	$(CP) NTP.COM $(DESTDIR)ntp.com

ntp.o: ntp.c
	zcc $(CFLAGS) $(STORE) -c ntp.c

sysface.o: sysface.c
	zcc  $(CFLAGS) $(STORE) -c  sysface.c 

date: date.o sysface.o libfram.o spi.o
	zcc  $(LINKOP) -odate date.o sysface.o libfram.o spi.o
	$(CP) DATE.COM $(DESTDIR)date.com

date.o: date.c
	zcc $(CFLAGS) $(STORE) -c date.c

wget: htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o libfram.o libhost.o
	zcc $(LINKOP) -owget htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o -lm libfram.o libhost.o
	$(CP) WGET.COM $(DESTDIR)wget.com

htget.o: htget.c
	zcc $(CFLAGS) $(STORE) -c htget.c

https: httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o libhost.o
	zcc  $(LINKOP) -ohttps httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o libhost.o
	$(CP) HTTPS.COM $(DESTDIR)https.com

httpServer.o: httpServer.c
	zcc $(CFLAGS) $(STORE) -c httpServer.c

httpParser.o: httpParser.c
	zcc  $(CFLAGS) $(STORE)  -c  httpParser.c 

pingnoti: pingn.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o libfram.o libhost.o
	zcc  $(LINKOP) -opingnoti pingn.o -DNOTIMER w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm libfram.o libhost.o
	$(CP) PINGNOTI.COM $(DESTDIR)pingnoti.com

pingn.o: ping.c
	zcc $(CFLAGS) $(STORE) -DNOTIMER -c ping.c -o pingn.o

mac: mac.o libfram.o spi.o snaplib.o
	zcc  $(LINKOP) -omac mac.o libfram.o spi.o snaplib.o
	$(CP) MAC.COM $(DESTDIR)mac.com

snaplib.o:
	zcc  $(CFLAGS) $(STORE) -c snaplib.c

mac.o:
	zcc  $(CFLAGS) $(STORE) -c mac.c

libhost.o: libhost.c
	zcc $(CFLAGS) $(STORE) -c libhost.c

uptime: uptime.o sysface.o libfram.o spi.o
	zcc $(LINKOP) -ouptime uptime.o sysface.o libfram.o spi.o
	$(CP) UPTIME.COM $(DESTDIR)uptime.com

uptime.o: uptime.c
	zcc $(CFLAGS) $(STORE) -c uptime.c

today: today.o sysface.o libfram.o spi.o
	zcc  $(LINKOP) -otoday today.o sysface.o libfram.o spi.o
	$(CP) TODAY.COM $(DESTDIR)today.com

today.o: today.c
	zcc $(CFLAGS) $(STORE) -c today.c

timezone: timezone.o sysface.o libfram.o spi.o
	zcc $(LINKOP) -otimezone timezone.o sysface.o libfram.o spi.o
	$(CP) TIMEZONE.COM $(DESTDIR)timezone.com

timezone.o: timezone.c
	zcc $(CFLAGS) $(STORE) -c timezone.c

clean:
	$(RM) *.o *.err *.lis *.def *.lst *.sym *.exe *.COM  driver ifconfig dnsprnt ping dig ftp telnet wget myget get ctc ntp date https clock pingnoti telnetd mac timezone today uptime test *.map
	rm -rf html irc latex email

install:
	cp ./*.COM $(DESTDIR1)/. 

ship:
	rm -rf irc latex email *.o *.err *.lis *.def *.lst *.sym *.exe driver dnsprnt get ctc ntp myget 
	rm -rf test ctc ntp wget telnet ping dig ifconfig date https timezone today uptime test 

documents:
	doxygen 
