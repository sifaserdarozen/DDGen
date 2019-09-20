# DDGen [![Build Status](https://travis-ci.org/sifaserdarozen/DDGen.png)](https://travis-ci.org/sifaserdarozen/DDGen) [![Coverity Status](https://img.shields.io/coverity/scan/8972.svg)](https://scan.coverity.com/projects/sifaserdarozen-ddgen) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/ce80d5adf8dd406eaa83e7ba32eb2cc7)](https://www.codacy.com/app/sifa-serdar-ozen/DDGen?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sifaserdarozen/DDGen&amp;utm_campaign=Badge_Grade)
Pseudo rtp packet generator

### What is it?
DDGen may be used to generate pseuto rtp packets to test VoIP systems. It can geneate traffic in two modes;
* Active mode (generated VoIP trafic is destined to a specific IP)
* Passive mode (generated VoIP traffic is mirrored)

### How to build
In a Linux flavor, be sure to install *make*, *g++* and *nvidia-cuda-toolkit* package. Then clone or dowload code.
```
make ddgen
```
will do the job. Unit tests may be fired by
```
./bin/ddgen_utests
```

### How to run
As DDGen uses Linux row sockets, it will need root privileges for some modes
```
sudo ./bin/ddgen --mirror
```
This will lead an execution with default parameters in passive mode. In this mode generated traffic is written to a pcap file. See following sections for detailed usage.


### Active mode (DDGen as drlink traffic generator)
In order to generate 10 simultaneous calls each having random duration (uniform %10) of 60s and send to drlink media address 192.168.126.1:28008 and 192.168.126.1:28009. 
Ip of generated packets may start from a specific ip by: --start option
```
sudo ./bin/ddgen --nc 10 --dc 60 --drlink 192.168.126.1 28008 192.168.126.1 28009 (--start 172.24.201.54)
```
Default values of ip and port are: 127.0.0.1 and 29000 29001

Active traffic is send to specified target ip & ports by default. If you want to save active traffic as pcap, use `--pcap` flag
```
./bin/ddgen --nc 10 --dc 60 --drlink 192.168.126.1 28008 192.168.126.1 28009 --pcap (--start 172.24.201.54)
```

### Passive Mode (mirror traffic generator)
Usual case in passive mode is saving generated traffic as pcap file.
Ip of generated packets may start from a specific ip by: --start option
```
./bin/ddgen --nc 10 --dc 60 --mirror (--pcap) (--start 172.24.201.54)
```

If somehow you want to send generated traffic to a socket (for example 192.168.126.1:28008) use;
```
sudo ./bin/ddgen --nc 10 --dc 60 --mirror --socket 192.168.126.1 28008 (--start 172.24.201.54)
```

MIRROR:
send to the socket:            sifa@sifa:~/DDGen$ docker run ddgen /bin/ddgen --nc 2 --dc 60 --mirror --socket 10.228.210.40 28008 (--start 172.24.201.54)
pcap                           sifa@sifa:~/DDGen$ docker run ddgen /bin/ddgen --nc 2 --dc 60 --mirror (--pcap) (--start 172.24.201.54)


ACTIVE
send to the socket:            sifa@sifa:~/DDGen$ docker run ddgen /bin/ddgen --nc 2 --dc 60 --drlink 10.228.212.32 28008 10.228.212.32 28009 (--start 172.24.201.54)
pcap                           sifa@sifa:~/DDGen$ docker run ddgen /bin/ddgen --nc 2 --dc 60 --drlink 10.228.212.32 28008 10.228.212.32 28009 --pcap (--start 172.24.201.54)


### To generate and run the contaner image
Install docker and generate image using docker/Dockerfile
```
docker build -t ddgen -f docker/Dockerfile .
```

Image can be used by providing arguments. 

#### Mirror mode from container
To generate mirrored traffic of 10 simultaneous calls with average duration of 60 seconds and send the generated traffic to 192.168.126.1:28000.
```
docker run ddgen /bin/ddgen --nc 10 --dc 60 --mirror --socket 192.168.126.1 28000 (--start 172.24.201.54)
```

To generate mirrored traffic of 10 simultaneous calls with average duration of 60 seconds and save as pcap
```
docker run ddgen /bin/ddgen --nc 10 --dc 60 --mirror (--pcap) (--start 172.24.201.54)
```

#### Active mode from container
In order to generate 10 simultaneous calls each having random duration of 60s and send to drlink media address 192.168.126.1:28008 and 192.168.126.1:28009. 
```
docker run ddgen /bin/ddgen --nc 10 --dc 60 --drlink 192.168.126.1 28008 192.168.126.1 28009 (--start 172.24.201.54)
```

Active traffic is send to specified target ip & ports by default. If you want to save active traffic as pcap, use `--pcap` flag
```
docker run ddgen /bin/ddgen --nc 10 --dc 60 --drlink 192.168.126.1 28008 192.168.126.1 28009 --pcap (--start 172.24.201.54)
```

If it is a desire to run multiple ddgen containers in parallel, a start ip address for endpoints may be provided.
In this case generating traffic for that container will start from that ip and increment till simulation ends.
This enables multiple generators simulate a range of ip traffic in parallel. To start with 172.24.201.54; 
```
docker run ddgen /bin/ddgen --nc 10 --dc 6 --socket 192.168.126.1 28008 --start 172.24.201.54 --mirror
```
 

### Profiling
Small documentation about possible profiling options for ddgen may be seen through https://sifaserdarozen.wordpress.com/2016/01/01/a-small-survey-for-profiling-a-c-application/
* poor mans profiler : see /profilers/pmp.sh.readme
* gprof              : see /prifilers/gprof.readme, code should be compiled with "make gprof=yes" to enable -pg flag
* perf               : see /profilers/perf.readme
* callgrind          : see /profilers/callgrind.readme

### Thanks to used 3rd Parties
Catch for unit tests https://github.com/philsquared/Catch

### License
Please check individual licanse information of used 3rd parties. Besides, there is a great deal of g722 taken from ITU reference implementation that should be trated as ITU wishes.
All remaining part is MIT.

