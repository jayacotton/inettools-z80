# inettools-z80
Inet tools for z80-rc2014 tcp/ip stack code.

This code is based on software for the W5500 chip.  The code includes the following programs:

ifconfig:  this is the poor mans version,  It has no options and shows your ip address and a few other parameters.

ping:  this is about a simple a ping as you can get.  Round trip time values work when RC2014/RomWBW timer support is present.  The time values are in 20ms intervals.  

pingnoti: this is ping without a timer.  

telnet:  This is a terminal program to talking to a host machine.

telnetd:  This is an attempt to allow CP/M to work via telnet console.   HIGHLY BROKEN at this time.

dig: This is a really simple version of dig, it reports a single ip address for remote host.

wget:  This will allow cp/m users to load programs onto there CF card etc using http protocol.

ntp: This program will get the date and time from an ntp server and set the RTC chip.  This requires an RTC/RC2014 board.

date: This reads the date and time from the RTC.  This requires an RTC/RC2014 board.

mac:  This is a tool that collects and stores the mac address for your wiznet card.

In addition to these programs there are a few test programs, best to ignore them for now.

Things that need work. 
The http server code is flat busted just now, and needs a lot of time.   The
DNS and DHCP timeout code needs work.  Telnetd is really broken just now.

All of this code runs on the RC2014 WIZNET V1 https://github.com/markt4311/MT011 board from Mark T.  
You can find info on this and many other exciting
retro computing boards at google group RC2014-Z80.  You can find h/w at tindie.com search for rc2014, or www.rc2014.co.uk.
Using the RomWbW code see https://github.com/wwarthen/RomWBW for more info on this.

The code is built using the z88dk tool chain, see z88dk.org for more info on the tool chain.
All of this work has been conducted on linux ubuntu v18.

This code is really preliminary in nature, it works in an ideal environment, but falls on its face when any kind of
out of band condition turns up.  Most often with the DHCP server gets tired of the FeatherWing asking questions.

Also the code it very large, most of the programs are 30 to 45 kb, which is huge on a z80.  I have made a preliminary
pass at reduceing the size of the code.  z88dk does not have (currently) dead code removal, so I used cppcheck and cscope
to track down dead functions and commented them out of the code pool.  Planning to be more sophisticated later.

To build the code you need z88dk installed, you will also need doxygen to produce the documentation files.

A note:  MYGET.COM needs a configuration file on the cp/m disk called MYGET.CFG.  It has the ip address of the
http host you are using for uploading (downloading) ... loading your code onto the cp/m disk.  
To make that work, you need to install an http server on your host machine, and populate the source directory with
files to transfer...  

Make commands are:  make clean, make all (or target name), make install, make documents, make ship
make install puts files into /var/www/html.  This is the default publishing
directory for the html server.


