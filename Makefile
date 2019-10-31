driver:
	zcc +cpm -O3 --list --c-code-in-asm -c w5500.c 
	zcc +cpm -O3 --list --c-code-in-asm -c main.c 
	zcc +cpm -O3 --list --c-code-in-asm -c dhcp.c 
	zcc +cpm -O3 --list --c-code-in-asm -c spi.c 
	zcc +cpm -O3 --list --c-code-in-asm -c socket.c 
	zcc +cpm -O3 --list --c-code-in-asm -c addrprint.c 
	zcc +cpm -O3 --list --c-code-in-asm -c ethernet.c 
	zcc +cpm -O3 --list --c-code-in-asm -c dnsprint.c 
	zcc +cpm -O3 --list --c-code-in-asm -c dns.c 
	zcc +cpm -O3 --list --c-code-in-asm -c ping.c 
	zcc +cpm -O3 --list --c-code-in-asm -c wizchip_conf.c 
	zcc +cpm -O3 --list --c-code-in-asm -c ftp.c 
	zcc +cpm -O3 --list --c-code-in-asm -c telnet_client.c 
	zcc +cpm -O3 --list -DNOTIME --c-code-in-asm -c htget.c 
	zcc +cpm -O3 --list --c-code-in-asm -c get.c 
	zcc +cpm -O3 --list --c-code-in-asm -c time.c 
	zcc +cpm -O3 --list --c-code-in-asm -c ntp.c 
	zcc +cpm -O3 --list --c-code-in-asm -c ntplib.c 
	zcc +cpm -O3 --list --c-code-in-asm -c date.c 
	zcc +cpm -O3 --list --c-code-in-asm -c httpServer.c 
	zcc +cpm -O3 --list --c-code-in-asm -c httpParser.c 
	zcc +cpm -create-app -oifconfig addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -odig dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -oping ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm
	zcc +cpm -create-app -opingnoti ping.c -DNOTIMER w5500.o dhcp.o spi.o socket.o ethernet.o dns.o time.o -lm
	zcc +cpm -create-app -otelnet telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -omyget get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -ontp ntp.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -odate date.o 
	zcc +cpm -create-app -ohttps httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -owget htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o time.o -lm

clean:
	$(RM) *.o *.err *.lis *.def *.lst *.sym *.exe *.COM  driver ifconfig dnsprnt ping dig ftp telnet wget myget get ctc ntp date https clock
	rm -rf html irc latex email

install:
	sudo cp ./*.COM /var/www/html/. 

ship:
	rm -rf irc latex email *.o *.err *.lis *.def *.lst *.sym *.exe driver dnsprnt get ctc ntp myget test ctc ntp wget telnet ping dig ifconfig date https

documents:
	doxygen 
