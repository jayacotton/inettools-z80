driver:
	zcc +cpm -O3 -vn -s -m --list -c w5500.c 
	zcc +cpm -O3 -vn -s -m --list -c snaplib.c 
	zcc +cpm -O3 -vn -s -m --list -c main.c 
	zcc +cpm -O3 -vn -s -m --list -c test.c 
	zcc +cpm -O3 -vn -s -m --list -c dhcp.c 
	zcc +cpm -O3 -vn -s -m --list -c spi.c 
	zcc +cpm -O3 -vn -s -m --list -c socket.c 
	zcc +cpm -O3 -vn -s -m --list -c addrprint.c 
	zcc +cpm -O3 -vn -s -m --list -c ethernet.c 
	zcc +cpm -O3 -vn -s -m --list -c dnsprint.c 
	zcc +cpm -O3 -vn -s -m --list -c dns.c 
	zcc +cpm -O3 -vn -s -m --list -c ping.c 
	zcc +cpm -O3 -vn -s -m --list -c wizchip_conf.c 
	zcc +cpm -O3 -vn -s -m --list -c ftp.c 
	zcc +cpm -O3 -vn -s -m --list -c telnet_client.c 
	zcc +cpm -O3 -vn -s -m --list -DNOTIME -c htget.c 
	zcc +cpm -O3 -vn -s -m --list -c get.c 
	zcc +cpm -O3 -vn -s -m --list -c time.c 
	zcc +cpm -O3 -vn -s -m --list -c ntp.c 
	zcc +cpm -O3 -vn -s -m --list -c rtci2c.c 
	zcc +cpm -O3 -vn -s -m --list -c ntplib.c 
	zcc +cpm -O3 -vn -s -m --list -c date.c 
	zcc +cpm -O3 -vn -s -m --list -c httpServer.c 
	zcc +cpm -O3 -vn -s -m --list -c httpParser.c 
	zcc +cpm -O3 -vn -s -m --list -c loader.c 
	zcc +cpm -create-app -oifconfig addrprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -odig dnsprint.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -oping ping.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o
	zcc +cpm -create-app -otelnet telnet_client.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -omyget get.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -ontp ntp.o rtci2c.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o snaplib.o
	zcc +cpm -create-app -odate date.o rtci2c.o  spi.o
	zcc +cpm -create-app -ohttps httpServer.o httpParser.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o 
	zcc +cpm -create-app -owget -DLOADER htget.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o ctcdriver.c time.o loader.o snaplib.o
	zcc +cpm -create-app -DCLOCK -DLOADER -cclock ctcdriver.c time.o ntplib.o w5500.o dhcp.o spi.o socket.o ethernet.o dns.o wizchip_conf.o loader.o snaplib.o
	zcc +cpm -create-app -DTEST -DLOADER -octc ctcdriver.c time.o loader.o snaplib.o
	zcc +cpm -create-app -otest test.o w5500.o spi.o
	macro ctc_int_handler
	asm8080 ctc_int_handler.asm -l

clean:
	$(RM) *.o *.err *.lis *.def *.lst *.sym *.exe *.COM  driver ifconfig dnsprnt ping dig ftp telnet wget myget get ctc ntp date https clock
	rm -rf html irc latex email

install:
	sudo cp ./*.COM /var/www/html/. 

ship:
	rm -rf irc latex email *.o *.err *.lis *.def *.lst *.sym *.exe driver dnsprnt get ctc ntp myget test ctc ntp wget telnet ping dig ifconfig date https

documents:
	doxygen 
