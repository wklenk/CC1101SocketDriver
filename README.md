**This software is Alpha-State, still under development and of limited use at the moment.**

* Update 2015-10-25: Fixed issue with SPI communication no longer working unter Raspbian "Jessie". https://github.com/wklenk/CC1101SocketDriver/issues/1
* Update 2014-01-28: Added a RawDataFrame for unstructured data received (alternative to RFBeeDataFrame)
* Update 2014-01-07: Should be able to receive up to 255 bytes now in CC1101 "variable length mode". Was only able to test it up to 184 bytes due to performance issues of the RFBee.
* Update 2014-01-01: Able to receive up to 60 byte payloads from RFBee and output on a TCP socket.

#CC1101SocketDriver

SPI Protocol Driver for TI CC1101 based RF communication modules.

This is the right place for you, if you ...

* want to wire a CC1101 based communication module to a Raspberry Pi
* want to write applications using a high-level programming language like Java or Python.
* have some experience in using TCP network sockets in this programming language
* want to do RF communication with other CC1101 based devices like [RFBee (Arduino)](https://github.com/Seeed-Studio/RFBee)
* want to build sensor networks
* just want to play arround with RF communication

Check out my blog at http://wolfgangklenk.wordpress.com

##How to compile?
Just copy the source files (.cpp and .hpp) into a Raspberry pi and compile the stuff using g++.

`g++ *.cpp`

This will generate an executable file a.out.
Just run this file, but note that you need to execute it as "root" as the
access to the GPIO file requires root permissions.

`sudo ./a.out`

