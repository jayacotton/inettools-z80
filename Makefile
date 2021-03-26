CFLAGS	= +cpm --list --c-code-in-asm  -Wall -pragma-include:zpragma.inc -DFRAM
# this is a windows specific dir for cygwin use
DESTDIR = /cygdrive/c/users/lbmgm/HostFileBdos/c/
# this is a linux/unix path
#DESTDIR = ~/HostFileBdos/c/
CP = cp

all: telnetd ifconfig telnet ping pingnoti dig wget myget ntp date https mac uptime today timezone wizreset digit

digit: digit.o sysface.o libfram.o spi.o
	zcc  +cpm -create-app -odigit digit.o sysface.o libfram.o spi.o
	$(CP) DIGIT.COM $(DESTDIR)digit.com

digit.o: digit.c
	zcc $(CFLAGS) -c digit.c

wizreset: wizreset.o w5500.o spi.o wizchip_conf.o
	zcc  +cpm -create-app -owizreset wizreset.o w5500.o spi.o wizchip_conf.o
	$(CP) WIZRESET.COM $(DESTDIR)wizreset.com

wizreset.o: wizreset.c
	zcc $(CFLAGS) -c wizreset.c

telnetd: telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o 
	zcc  +cpm -create-app -otelnetd telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o snaplib.c libfram.o
	$(CP) TELNETD.COM $(DESTDIR)telnetd.com

telnet_server.o: telnet_server.c
	zcc $(CFLAGS) -c telnet_server.c

w5500.o: w5500.c w5500.h wizchip_conf.h
	zcc  $(CFLAGS) -c  w5500.c 

dhcp.o: dhcp.c dhcp.h
	zcc  $(CFLAGS)   -c  dhcp.c 

spi.o: spi.c spi.h
	zcc  $(CFLAGS)  -c  spi.c 

socket.o: socket.c socket.h
	zcc  $(CFLAGS)   -c  socket.c 

ethernet.o: ethernet.c ethernet.h
	zcc  $(CFLAGS)   -c  ethernet.c 

dns.o: dns.c dns.h
	zcc  $(CFLAGS)   -c  dns.c 

wizchip_conf.o: wizchip_conf.c wizchip_conf.h
	zcc  $(CFLAGS)  -c  wizchip_conf.c 

ifconfig: addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libfram.o
	zcc  +cpm -m -create-app -oifconfig -Wunused addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libfram.o
	$(CP) IFCONFIG.COM $(DESTDIR)ifconfig.com

addrprint.o: addrprint.c
	zcc $(CFLAGS) -c addrprint.c

dig: dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libfram.o
	zcc  +cpm -create-app -odig dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o libfram.o
	$(CP) DIG.COM $(DESTDIR)dig.com

dnsprint.o: dnsprint.c
	zcc $(CFLAGS) -c dnsprint.c

ping: ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o libfram.o
	zcc  +cpm -create-app -oping ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm libfram.o
	$(CP) PING.COM $(DESTDIR)ping.com

ping.o: ping.c
	zcc $(CFLAGS) -c ping.c

time.o: time.c
	zcc $(CFLAGS) -c  time.c 

telnet: telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o
	zcc +cpm -create-app -otelnet telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o
	$(CP) TELNET.COM $(DESTDIR)telnet.com

telnet_client.o: telnet_client.c
	zcc $(CFLAGS) -c telnet_client.c

myget: get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o
	zcc +cpm -create-app -omyget get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o
	cp MYGET.COM $(DESTDIR)myget.com

get.o: get.c
	zcc $(CFLAGS) -c get.c

ntp: ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o libfram.o
	zcc +cpm -create-app -ontp ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o libfram.o
	$(CP) NTP.COM $(DESTDIR)ntp.com

ntp.o: ntp.c
	zcc $(CFLAGS) -c ntp.c

sysface.o: sysface.c
	zcc  $(CFLAGS) -c  sysface.c 

date: date.o sysface.o libfram.o spi.o
	zcc  +cpm -create-app -odate date.o sysface.o libfram.o spi.o
	$(CP) DATE.COM $(DESTDIR)date.com

date.o: date.c
	zcc $(CFLAGS) -c date.c

wget: htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o libfram.o
	zcc +cpm -create-app -owget htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o -lm libfram.o
	$(CP) WGET.COM $(DESTDIR)wget.com

htget.o: htget.c
	zcc $(CFLAGS) -c htget.c

https: httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o
	zcc  +cpm -create-app -ohttps httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o libfram.o
	$(CP) HTTPS.COM $(DESTDIR)https.com

httpServer.o: httpServer.c
	zcc $(CFLAGS) -c httpServer.c

httpParser.o: httpParser.c
	zcc  $(CFLAGS)  -c  httpParser.c 

pingnoti: pingn.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o libfram.o
	zcc  +cpm -create-app -opingnoti pingn.o -DNOTIMER w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm libfram.o
	$(CP) PINGNOTI.COM $(DESTDIR)pingnoti.com

pingn.o: ping.c
	zcc $(CFLAGS) -DNOTIMER -c ping.c -o pingn.o

mac: mac.o libfram.o spi.o snaplib.o
	zcc  +cpm -create-app -m -omac mac.o libfram.o spi.o snaplib.o
	$(CP) MAC.COM $(DESTDIR)mac.com

snaplib.o:
	zcc  $(CFLAGS) -c snaplib.c

mac.o:
	zcc  $(CFLAGS) -c mac.c

libfram.o:
	zcc  $(CFLAGS) -c libfram.c

uptime: uptime.o sysface.o libfram.o spi.o
	zcc +cpm -create-app -ouptime uptime.o sysface.o libfram.o spi.o
	$(CP) UPTIME.COM $(DESTDIR)uptime.com

uptime.o: uptime.c
	zcc $(CFLAGS) -c uptime.c

today: today.o sysface.o libfram.o spi.o
	zcc  +cpm -create-app -otoday today.o sysface.o libfram.o spi.o
	$(CP) TODAY.COM $(DESTDIR)today.com

today.o: today.c
	zcc $(CFLAGS) -c today.c

timezone: timezone.o sysface.o libfram.o spi.o
	zcc +cpm -create-app -otimezone timezone.o sysface.o libfram.o spi.o
	$(CP) TIMEZONE.COM $(DESTDIR)timezone.com

timezone.o: timezone.c
	zcc $(CFLAGS) -c timezone.c

clean:
	$(RM) *.o *.err *.lis *.def *.lst *.sym *.exe *.COM  driver ifconfig dnsprnt ping dig ftp telnet wget myget get ctc ntp date https clock pingnoti telnetd mac timezone today uptime test
	rm -rf html irc latex email

install:
	cp ./*.COM /srv/www/htdocs/. 

ship:
	rm -rf irc latex email *.o *.err *.lis *.def *.lst *.sym *.exe driver dnsprnt get ctc ntp myget 
	rm -rf test ctc ntp wget telnet ping dig ifconfig date https timezone today uptime test 

documents:
	doxygen 
