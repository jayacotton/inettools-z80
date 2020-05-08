CFLAGS	= --list --c-code-in-asm 
#CFLAGS	= -O3 --list --c-code-in-asm

all: telnetd ifconfig telnet ping pingnoti dig wget myget ntp date https mac uptime today timezone

telnetd: telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -otelnetd telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o snaplib.c

telnet_server.o:telnet_server.c telnetd.h
	zcc +cpm $(CFLAGS) -DDEBUG -c telnet_server.c 

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

ifconfig: addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -oifconfig -Wunused addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o

addrprint.o: addrprint.c 
	zcc +cpm $(CFLAGS)  -c  addrprint.c 

dig: dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -odig dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o

dnsprint.o: dnsprint.c
	zcc +cpm $(CFLAGS)  -c  dnsprint.c 

ping: ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o 
	zcc +cpm -create-app -oping ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm

ping.o: ping.c
	zcc +cpm $(CFLAGS)  -c  ping.c 

time.o: time.c
	zcc +cpm $(CFLAGS) -c  time.c 

telnet: telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -otelnet telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 

telnet_client.o:
	zcc +cpm $(CFLAGS) -c  telnet_client.c 

myget: get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -omyget get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 

get.o:
	zcc +cpm $(CFLAGS) -c  get.c 

ntp: ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o
	zcc +cpm -create-app -ontp ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o sysface.o

ntp.o: ntp.c 
	zcc +cpm $(CFLAGS) -DUNIXEPOCH -DPDT -c  ntp.c 

sysface.o: sysface.c
	zcc +cpm $(CFLAGS) -c  sysface.c 

date: date.o sysface.o
	zcc +cpm -create-app -odate date.o sysface.o

date.o: date.c
	zcc +cpm $(CFLAGS) -DPDT -c  date.c 

wget: htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o
	zcc +cpm -create-app -owget htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o -lm

htget.o: htget.c
	zcc +cpm $(CFLAGS) -c htget.c

https: httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -ohttps httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 

httpServer.o:
	zcc +cpm $(CFLAGS)  -c  httpServer.c 

httpParser.o:
	zcc +cpm $(CFLAGS)  -c  httpParser.c 

pingnoti: w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o
	zcc +cpm -create-app -opingnoti ping.c -DNOTIMER w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm

mac: mac.o
	zcc +cpm -create-app -omac mac.o

mac.o:
	zcc +cpm $(CFLAGS) -c mac.c

uptime: uptime.o sysface.o
	zcc +cpm -create-app -ouptime uptime.o sysface.o

uptime.o: uptime.c
	zcc +cpm $(CFLAGS) -c uptime.c

today: today.o sysface.o
	zcc +cpm -create-app -otoday today.o sysface.o

today.o: today.c
	zcc +cpm $(CFLAGS) -c today.c

timezone: timezone.o sysface.o
	zcc +cpm -create-app -otimezone timezone.o sysface.o

timezone.o: timezone.c
	zcc +cpm $(CFLAGS) -c timezone.c

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
