all: telnetd ifconfig telnet ping pingnoti dig wget myget ntp date https mac

telnetd: telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -otelnetd telnet_server.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o snaplib.c

telnet_server.o:telnet_server.c
	zcc +cpm -O3 --list --c-code-in-asm -DDEBUG -c  telnet_server.c 

w5500.o: w5500.c
	zcc +cpm -O3 --list --c-code-in-asm -c  w5500.c 

dhcp.o: dhcp.c
	zcc +cpm -O3 --list --c-code-in-asm   -c  dhcp.c 

spi.o: spi.c
	zcc +cpm -O3 --list --c-code-in-asm  -c  spi.c 

socket.o: socket.c
	zcc +cpm -O3 --list --c-code-in-asm   -c  socket.c 

ethernet.o: ethernet.c
	zcc +cpm -O3 --list --c-code-in-asm   -c  ethernet.c 

dns.o: dns.c
	zcc +cpm -O3 --list --c-code-in-asm   -c  dns.c 

wizchip_conf.o: wizchip_conf.c
	zcc +cpm -O3 --list --c-code-in-asm  -c  wizchip_conf.c 

ifconfig: addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -oifconfig -Wunused addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o

addrprint.o: addrprint.c
	zcc +cpm -O3 --list --c-code-in-asm  -c  addrprint.c 

dig: dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -odig dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o

dnsprint.o: dnsprint.c
	zcc +cpm -O3 --list --c-code-in-asm  -c  dnsprint.c 

ping: ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o 
	zcc +cpm -create-app -oping ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm

ping.o: ping.c
	zcc +cpm -O3 --list --c-code-in-asm  -c  ping.c 

time.o: time.c
	zcc +cpm -O3 --list --c-code-in-asm -c  time.c 

telnet: telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -otelnet telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 

telnet_client.o:
	zcc +cpm -O3 --list --c-code-in-asm -c  telnet_client.c 

myget: get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -omyget get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 

get.o:
	zcc +cpm -O3 --list --c-code-in-asm -c  get.c 

ntp: ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -ontp ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 

ntp.o: ntp.c
	zcc +cpm -O3 --list --c-code-in-asm -DPDT -c  ntp.c 

date: date.o
	zcc +cpm -create-app -odate date.o 

date.o: 
	zcc +cpm -O3 --list --c-code-in-asm -DPDT -c  date.c 

wget: htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o
	zcc +cpm -create-app -owget htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o -lm

htget.o: htget.c
	zcc +cpm -O3 --list --c-code-in-asm -c htget.c

https: httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o
	zcc +cpm -create-app -ohttps httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 

httpServer.o:
	zcc +cpm -O3 --list --c-code-in-asm  -c  httpServer.c 

httpParser.o:
	zcc +cpm -O3 --list --c-code-in-asm  -c  httpParser.c 

pingnoti: w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o
	zcc +cpm -create-app -opingnoti ping.c -DNOTIMER w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm

mac: mac.o
	zcc +cpm -create-app -omac mac.o

mac.o:
	zcc +cpm -O3 --list --c-code-in-asm -c mac.c

clean:
	$(RM) *.o *.err *.lis *.def *.lst *.sym *.exe *.COM  driver ifconfig dnsprnt ping dig ftp telnet wget myget get ctc ntp date https clock pingnoti telnetd
	rm -rf html irc latex email

install:
	sudo cp ./*.COM /var/www/html/. 

ship:
	rm -rf irc latex email *.o *.err *.lis *.def *.lst *.sym *.exe driver dnsprnt get ctc ntp myget test ctc ntp wget telnet ping dig ifconfig date https

documents:
	doxygen 
