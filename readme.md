# DDGen [![Build Status](https://travis-ci.org/sifaserdarozen/DDGen.png)](https://travis-ci.org/sifaserdarozen/DDGen)
Pseudo rtp packet generator

## Purpose

Tool for active (drlink) or passive (mirrored traffic) pseudo rtp packet generator to test VoIP systems.
Uses  row sockets which requires root privileges. 
By default drlink traffic is send to configured ip/port pairs, and mirrored traffic is written to a pcap file. However, these may be overridden though.

## Active Mode (drlink traffic generator) : ddgen --nc 10 [-i ip_of_capturer -p port_of_capturer ... ]

In order to generate 10 simultaneous calls each having random duration (uniform %10) of 60s and send to drlink media address 192.168.126.1:28008 and 192.168.126.1:28009
ddgen --nc 10 --dc 60 --drlink 192.168.126.1 28008 192.168.126.1 28009

(if omitted) default values are: 127.0.0.1 and 29000 29001
drlink data is send to drlink socket by default. If want to save as pcap, use --pcap flag
ddgen --nc 10 --dc 60 --pcap --drlink 192.168.126.1 28008 192.168.126.1 28009

## Passive Mode (mirror traffic generator) : ddgen --nc 10 --dc 60 --mirror

In order to save pair traffic as pcap file, which should be operating in non functional mirror mode.
If somehow want to send generated pair data to a socket (for example 192.168.126.1:28008) use;
ddgen --nc 10 --dc 60 --socket 192.168.126.1 28008 --mirror


## Build and Test
Compiled and tested in Debian Jessie. Probably will work in other Linux flavours with minor modifications.
- install make, g++ and nvidia-cuda-toolkit
        make
- unit tests may be fired by
        ./bin/ddgen_utests


## Profiling
Small documentation about possible profiling options for ddgen may be seen through https://sifaserdarozen.wordpress.com/2016/01/01/a-small-survey-for-profiling-a-c-application/
        poor mans profiler : see /profilers/pmp.sh.readme
        gprof              : see /prifilers/gprof.readme, code should be compiled with "make gprof=yes" to enable -pg flag
        perf               : see /profilers/perf.readme
        callgrind          : see /profilers/callgrind.readme
        
## Thanks to used 3rd Parties
Catch for unit tests https://github.com/philsquared/Catch

