# inettools-z80
Inet tools for z80-rc2014 tcp/ip stack code.

This code is based on software for the W5500 chip.  The code includes the following programs:

ifconfig:  this is the poor mans version,  It has no options and shows your ip address and a few other parameters.
ping:  this is about a simple a ping as you can get.  There are to time values for round trip times.
telnet:  This is a terminal program to talking to a host machine.
dig: This is a really simple version of dig, it reports a single ip address for remote host.
wget:  This will allow cp/m users to load programs onto there CF card etc using http protocol.
ntp: This program will get the date and time from an ntp server and set the RTC chip.
date: This reads the date and time from the RTC.

In addition to these programs there are a few test programs, best to ignore them for now.

Things that need work.  I am integrating the CTC timer into some of this code to allow dead connection timeouts
and also measuring connection performance.  Also, the http server code is flat busted just now, and needs a lot
of time.

All of this code runs on the RC2014 WIZNET V1 https://github.com/markt4311/MT011 board from Mark T.  
You can find info on this and many other exciting
retro computing boards at google group RC2014-Z80.  You can find h/w at tindie.com search for rc2014, or www.rc2014.co.uk.
Using the RomWbW code see https://github.com/wwarthen/RomWBW for more info on this.

The code is built using the z88dk tool chain, see z88dk.org for more info on the tool chain.

All of this work has been conducted on linux ubuntu v18.

This code is really preliminary in nature, it works in an ideal environment, but falls on its face when any kind of
out of band condition turns up.  Most often with the DHCP server gets tired of the FeatherWing asking questions.

Also the code it very large, most of the programs are 30 to 45 kb, which is huge on a z80.  I have not attempted to 
reduce the foot print of the code yet.

To build the code you need z88dk installed, you will also need doxygen to produce the documentation files and for
the CTC nacent timer code you need macro see my macro github project  and my 8080 assembler github project.

If you don't want to mess with the CTC code, just hack out the macro and asm8080 lines in the Makefile.

A note:  MYGET.COM needs a configureation file on the cp/m disk called MYGET.CFG.  It has the ip address of the
http host you are using for uploading (downloading) ... loading your code onto the cp/m disk.  

To make that work, you need to install an http server on your host machine, and populate the source directory with
files to transfer...  

Make commands are:  make clean, make, make install, make documents, make ship
