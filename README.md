# ProTrekkr

This a fork of a fork of ProTrekkr. 

Lots of code will be ripped out. Tears will be shed.

![protrekkr, yeah!](protrekkr.jpg)

Some ideas:

- reorganize folder structure
- reduce global variables 
- remove unneeded features
- samplerates other that 44.1 kHz
- lua API 
- Sporth plugin
- more internal effects
- Conversion to C99 (code isn't really c++ anyways)


This repo contains a fork of ProTrekkr, now with linux JACK Audio support.

see http://code.google.com/p/protrekkr/


To build under linux, simply do:
$ make -f makefile.linux

And then to run:
$ ./release/distrib/ptk_linux
