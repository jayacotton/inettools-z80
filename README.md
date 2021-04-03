# inettools-z80
Inet tools for z80-rc2014 tcp/ip stack code.

Updates to the code base include FRAM support for storing all the parameters I was stashing on the
disk or in the NVRAM on the RTC chip.  At this point, you should be able to build and run the code
base without having an RTC board in your machine, but you will need an FRAM chip.

Future plan is the create a disk file version of the FRAM code, so all the parameters can be saved
on a disk file, as a last resort.


This code is based on software for the W5500 chip.  The code includes the following programs:

ifconfig:  ifconfig -s will read the addresses from the w5500 and store them in the FRAM.  ifconfig 
with no parameters will print the values from the FRAM.

This is the poor mans version,  It has no options and shows your ip address and a few other parameters.  
The most important part of this is that it uses DHCP to get your IP address from your router.  

ping:  this is about a simple a ping as you can get.  Round trip time values work when RC2014/RomWBW timer support is present.  The time values are in 20ms intervals.  
C>ping www.nasa.gov                                                                                                               
PING WWW.NASA.GOV (13.227.73.96)                                                
 52 bytes from 13.227.73.96: icmp_seq=4321 time=80 ms                           
 52 bytes from 13.227.73.96: icmp_seq=4322 time=80 ms                           
 52 bytes from 13.227.73.96: icmp_seq=4323 time=80 ms                           
 52 bytes from 13.227.73.96: icmp_seq=4324 time=80 ms                           
 52 bytes from 13.227.73.96: icmp_seq=4325 time=80 ms        

pingnoti: this is ping without a timer.  IF you don't have a CTC to drive
the interrupt timer, then you need this version.

C>pingnoti www.nasa.gov                                                                                                           
PING WWW.NASA.GOV (13.227.73.116)                                               
 52 bytes from 13.227.73.116: icmp_seq=4321                                     
 52 bytes from 13.227.73.116: icmp_seq=4322                                     
 52 bytes from 13.227.73.116: icmp_seq=4323                                     
 52 bytes from 13.227.73.116: icmp_seq=4324                                     
 52 bytes from 13.227.73.116: icmp_seq=4325                                     
                                            
telnet:  This is a terminal program to talking to a host machine.

C>telnet 192.168.0.120                                                                                                            
Connected...                                                                    
                                                                                
login: jay                                                         
Password:                                                                       
Last login: Tue May  5 16:44:59 PDT 2020 from 192.168.0.131 on pts/5            
Welcome to Ubuntu 19.04 (GNU/Linux 5.0.0-15-generic x86_64)                     
                                                                                
 * Documentation:  https://help.ubuntu.com                                      
 * Management:     https://landscape.canonical.com                              
 * Support:        https://ubuntu.com/advantage                                 
                                                                                
                                                                                
1 update can be installed immediately.                                          
0 of these updates are security updates.                                        
                                                                                
$

telnetd:  This is an attempt to allow CP/M to work via telnet console.   HIGHLY BROKEN at this time.

dig: This is a really simple version of dig, it reports a single ip address for remote host.

wget:  This will allow cp/m users to load programs onto there CF card etc using http protocol.

NOTE:  the time code uses (nvram or FRAM, and later a disk file) in the RTC to store operational data.  
Since there are 32 locations in nvram and I use 12, things are a bit tight.  Note: FRAM has 8k of space.

today:  This is code cribed from the z88dk/examples/today.c.  I modified it to work with
the rc2014 RTC and timer code.  I.e. get the UNIXEPOCH as adjusted for time zone and 
delta time from last ntp update.
today does this
C>today                                                                         
Today is Tuesday, the fifth day of May, two thousand and twenty. In             
fifteen seconds, the time will be sixteen minutes before ten PM,                
Daylight Savings Time. The moon is waxing gibbous.                              
                                                                    
timezone:  Tool for setting your time zone offset.  You will need to know your ICOA time zone code.
For me that is PDT in summer and PST in winter.  Not all locals have 'daylight savings' time.

To set,
C>timezone PDT                                                                  
Your offset is UTC -25200 seconds PDT PACIFIC DAYLIGHT TIME  

Now you can check your timezone value by useing timezone without a time value.
C>timezone                                                                      
Your offset is UTC -25200 seconds PDT PACIFIC DAYLIGHT TIME                     
                                                                 
uptime:  This reads the amount of time your RC2014 has been running since reboot.  Its
really a handy tool for testing the interrupt timer.  But then so it timer.

ntp: This program will get the date and time from an ntp server and set the RTC chip.  
This requires an RTC/RC2014 board.  Ntp's default nist server is time.google.com.
If you want a different one, then you must type in the server http address.

C>ntp                                                                                                                             
OR
C>ntp time.google.com                                                 

date: This reads the date and time from the RTC.  This requires an RTC/RC2014 board.
L>date
Sat May 16 22:56:50 2020

digit:  This runs a wall clock using ASCII art to draw digits and loops at about 4 hz.
you can exit with ctrl-c.  This does require that your terminal work with vt100 commands.
```
C>l:digit
 2222   2222         555555  9999         44  44  6666  
    22 22  22   ::   55     99  99   ::   44  44 66     
   22     22         55555   99999        444444 66666  
  22     22     ::       55     99   ::       44 66  66 
222222 222222        55555   9999             44  6666  
```
mac:  This is a tool that collects and stores the mac address for your wiznet card.
mac -r will print out the mac address as stored in the FRAM

wizreset:  THis will clear all the data out of your wiznet chip.  Its basically a hardware
reset.

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
You can also build this code on windows with cygwin installed.

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

If you have an FRAM chip on your MT011 board you can just do a build, but you may want to edit
Makefile and remove the -DFRAM flag.  I suspect that I may be the only user with FRAM for a while.

Make commands are:  make clean, make all (or target name), make install, make documents, make ship
make install puts files into /var/www/html.  This is the default publishing
directory for the html server.

Since you are looking at this project, you may be interested in this CPNET project.
https://github.com/durgadas311/cpnet-z80

