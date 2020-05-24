CFLAGS	= -list --c-code-in-asm  -Wall -pragma-include:zpragma.inc 
#CFLAGS	= -O3 --list --c-code-in-asm
DESTDIR = ~/HostFileBdos/c/
CP = cp

all: telnetd ifconfig telnet ping pingnoti dig wget myget ntp date https mac uptime today timezone wizreset digit

digit: digit.c sysface.o
	zcc +cpm -create-app -$(CFLAGS) -odigit digit.c sysface.o
	$(CP) DIGIT.COM $(DESTDIR)digit.com

wizreset: wizreset.c w5500.o spi.o wizchip_conf.o
	zcc +cpm -create-app -$(CFLAGS) -owizreset wizreset.c w5500.o spi.o wizchip_conf.o
	$(CP) WIZRESET.COM $(DESTDIR)wizreset.com

telnetd: telnet_server.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -$(CFLAGS) -otelnetd telnet_server.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o snaplib.c
	$(CP) TELNETD.COM $(DESTDIR)telnetd.com

w5500.o: w5500.c w5500.h wizchip_conf.h
	zcc +cpm $(CFLAGS) -c  w5500.c 

dhcp.o: dhcp.c dhcp.h
	zcc +cpm $(CFLAGS)   -c  dhcp.c 

spi.o: spi.c spi.h
	zcc +cpm $(CFLAGS)  -c  spi.c 

socket.o: socket.c socket.h
	zcc +cpm $(CFLAGS)   -c  socket.c 

ethernet.o: ethernet.c ethernet.h
	zcc +cpm $(CFLAGS)   -c  ethernet.c 

dns.o: dns.c dns.h
	zcc +cpm $(CFLAGS)   -c  dns.c 

wizchip_conf.o: wizchip_conf.c wizchip_conf.h
	zcc +cpm $(CFLAGS)  -c  wizchip_conf.c 

ifconfig: addrprint.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -m -create-app -oifconfig -Wunused -$(CFLAGS) addrprint.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	$(CP) IFCONFIG.COM $(DESTDIR)ifconfig.com

dig: dnsprint.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -$(CFLAGS) -odig dnsprint.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	$(CP) DIG.COM $(DESTDIR)dig.com

ping: ping.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o 
	zcc +cpm -create-app -$(CFLAGS) -oping ping.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm
	$(CP) PING.COM $(DESTDIR)ping.com

time.o: time.c
	zcc +cpm $(CFLAGS) -c  time.c 

telnet: telnet_client.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -$(CFLAGS) -otelnet telnet_client.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	$(CP) TELNET.COM $(DESTDIR)telnet.com

myget: get.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -$(CFLAGS) -omyget get.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	cp MYGET.COM $(DESTDIR)myget.com

ntp: ntp.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o
	zcc +cpm -create-app -$(CFLAGS) -ontp ntp.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o
	$(CP) NTP.COM $(DESTDIR)ntp.com

sysface.o: sysface.c
	zcc +cpm $(CFLAGS) -c  sysface.c 

date: date.c sysface.o
	zcc +cpm -create-app -$(CFLAGS) -odate date.c sysface.o
	$(CP) DATE.COM $(DESTDIR)date.com

wget: htget.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o
	zcc +cpm -create-app -$(CFLAGS) -owget htget.c w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o -lm
	$(CP) WGET.COM $(DESTDIR)wget.com

https: httpServer.c httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -$(CFLAGS) -ohttps httpServer.c httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	$(CP) HTTPS.COM $(DESTDIR)https.com
httpParser.o:
	zcc +cpm $(CFLAGS)  -c  httpParser.c 

pingnoti: w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o
	zcc +cpm -create-app -$(CFLAGS) -opingnoti ping.c -DNOTIMER w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm
	$(CP) PINGNOTI.COM $(DESTDIR)pingnoti.com

mac: mac.o
	zcc +cpm -create-app -omac mac.o
	$(CP) MAC.COM $(DESTDIR)mac.com

mac.o:
	zcc +cpm $(CFLAGS) -c mac.c

uptime: uptime.c sysface.o
	zcc +cpm -create-app -$(CFLAGS) -ouptime uptime.c sysface.o
	$(CP) UPTIME.COM $(DESTDIR)uptime.com

today: today.c sysface.o
	zcc +cpm -create-app -$(CFLAGS) -otoday today.c sysface.o
	$(CP) TODAY.COM $(DESTDIR)today.com

timezone: timezone.c sysface.o
	zcc +cpm -create-app -$(CFLAGS) -otimezone timezone.c sysface.o
	$(CP) TIMEZONE.COM $(DESTDIR)timezone.com
clean:
	$(RM) *.o *.err *.lis *.def *.lst *.sym *.exe *.COM  driver ifconfig dnsprnt ping dig ftp telnet wget myget get ctc ntp date https clock pingnoti telnetd mac timezone today uptime test
	rm -rf html irc latex email

install:
	sudo cp ./*.COM /var/www/html/. 

ship:
	rm -rf irc latex email *.o *.err *.lis *.def *.lst *.sym *.exe driver dnsprnt get ctc ntp myget 
	rm -rf test ctc ntp wget telnet ping dig ifconfig date https timezone today uptime test 

documents:
	doxygen 
