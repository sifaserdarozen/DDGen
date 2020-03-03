# DDGen [![Build Status](https://travis-ci.org/sifaserdarozen/DDGen.png)](https://travis-ci.org/sifaserdarozen/DDGen) [![Coverity Status](https://img.shields.io/coverity/scan/8972.svg)](https://scan.coverity.com/projects/sifaserdarozen-ddgen) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/ce80d5adf8dd406eaa83e7ba32eb2cc7)](https://www.codacy.com/app/sifa-serdar-ozen/DDGen?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sifaserdarozen/DDGen&amp;utm_campaign=Badge_Grade)
Pseudo rtp packet generator

### What is it?
DDGen may be used to generate pseuto rtp packets to test VoIP systems. It can geneate traffic in two modes;
* Passive mode (generated VoIP traffic is sample of a switch mirror)
* Active mode (generated VoIP trafic is destined to a specific IP)

It can be build locally from source code, or one of prebuild docker images can be used.
- Latest image: sifaserdarozen/ddgen:latest
- Stable image: sifaserdarozen/ddgen:0.2

### How to get a local build
In a Linux flavor, be sure to install *cmake*, *make* and *g++* packages. Then go into repository and fire
```
./configure
./make
```
Unit tests can be executed by
```
./bin/ddgen_utests
```

### How to run
As DDGen uses Linux row sockets, it will need root privileges for some of its modes. The most simple usage will be generating passive g711 traffic as;
```
./bin/ddgen --mirror
```
This will lead an execution with default parameters in passive mode. In this mode generated traffic is written to a pcap file. See following sections for detailed usage and options.

### Passive Mode (mirror traffic generator)
Usual case in passive mode is saving generated traffic as pcap file.
```
./bin/ddgen --mirror
```
By default there are 10 simultaneous calls each lasting about 60 seconds. As calls end, new ones are created. These default values can be overriden through `---nc` & `--dc` options. If you would like to have 5 simultaneous calls with 150 seconds duration usage will be;
```
./bin/ddgen --nc 5 --dc 150 --mirror
```
By default, similation duration is 600 seconds. It can be overriden through `--ds` option. To run a simulation of one hour, we can use;
```
./bin/ddgen --ds 3600 --mirror
```
By default, generated stream ip s will start through 172.24.201.54. This can be overriden with `--start` option.
```
./bin/ddgen --nc 5 --dc 150 --mirror --start 192.168.10.1
```
Default mode for `--mirror` option is to write generated traffic as pcap file. If somehow you want to send generated traffic to a socket (for example 192.168.126.1:28008) use `--socket`. Since this will need inpersonation (faking about ip of the packet), it needs sudo priviledges.
```
sudo ./bin/ddgen --nc 10 --dc 60 --mirror --socket 192.168.126.1 28008
```

### Active mode (DDGen as drlink traffic generator)
By default active traffic is forwarded to a pair of drlink sockets. In order to generate drlink traffic with default number of calls and send generated streams to addresses 192.168.126.1:28008 and 192.168.126.1:28009, we can use `-drlink` option. Since this will need inpersonation (faking about source ip of the packet), it needs sudo priviledges.
```
sudo ./bin/ddgen --drlink 192.168.126.1 28008 192.168.126.1 28009
```
As in passive case, number and duration of streams and overall simulation time can be set through `--nc`, `-dc` and `--ds` options.
```
/bin/ddgen --nc 100 --dc 60 --ds 3600 --drlink 192.168.126.1 28008 192.168.126.1 28009
```
Default starting IP of  generated stream will be 172.24.201.54. This can be overriden with `--start` option.
```
sudo ./bin/ddgen --drlink 192.168.126.1 28008 192.168.126.1 28009 --start 192.168.10.1
```
Active traffic is send to specified target ip & ports by default. If you want to save active traffic as pcap, use `--pcap` flag. since it does not require socket operations, this can be performed in non priviledged mode.
```
./bin/ddgen --nc 10 --dc 60 --drlink 192.168.126.1 28008 192.168.126.1 28009 --pcap
```

### Use of secure web interface
http://localhost:8080/healthz
http://localhost:8080/readyz
https://localhost:443/healthz
https://localhost:443/readyz
There is a default web interface serving at port 8080. This interface is used for kubernetes health and readiness checks.
In order to use secure port 443 use `--secure` option. Besides, there should be ddgen-cert.pem certificate file prepared in working directory and executable should be run with priviledged mode for non standard port ranges;
```
sudo ./bin/ddgen --mirror --secure
```


### To generate and run the container image
In order to generate [docker](https://www.docker.com/) container `docker/Dockerfile` file can be used. Used image is [Alpine Linux](https://alpinelinux.org/).
```
docker build -t ddgen -f docker/Dockerfile .
```
There is also prebuild ddgen container that can be obtained from docker hub.

Image can be used as usual with providing arguments.

#### Mirror mode from container
To generate mirrored traffic of 10 simultaneous calls with average duration of 60 seconds and send the generated traffic to 192.168.126.1:28000.
```
docker run ddgen /ddgen/ddgen --nc 10 --dc 60 --mirror --socket 192.168.126.1 28000
```

#### Active mode from container
Running of container is much similar
```
docker run ddgen /ddgen/ddgen --nc 10 --dc 60 --drlink 192.168.126.1 28008 192.168.126.1 28009
```
### use of multiple containers simultaneously ###
If it is a desire to run multiple ddgen containers in parallel, a start ip address for endpoints may be provided.
In this case generating traffic for that container will start from that ip and increment till simulation ends.
This enables multiple generators simulate a range of ip traffic in parallel. To start with 172.24.201.54;
```
docker run ddgen /ddgen/ddgen --nc 10 --dc 6 --socket 192.168.126.1 28008 --start 172.24.201.54 --mirror
```

### Use of container manually ###
in order to use container image but trigger manually, get access to shell
```
docker run -ti --entrypoint=/bin/sh ddgen
```
and start simulation from console as;
```
/ddgen # ./ddgen/ddgen --mirror --nc 10

```
### To run with kubernetes
Kubernetes may be used to run published ddgen images. To deploy two clusters of ddgen one with http and the other with https health check;
```
kubectl apply -f kubernetes/deploy.yaml
kubectl apply -f kubernetes/deploy-secure.yaml
```

#### Amazon S3 object storage  support for pcap files.

DDGen supports [Amazon S3](https://aws.amazon.com/s3) when generates pcap. This is the default mode for passive mode, or `--pcap` flagged active mode. After simulation ends, geberated pcap file will be send to S3 bucket.
First install the necessary dependencies
```
sudo apt-get install libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev
```
Then configure with dynamodb support
```
./configure.sh --use-s3
./make
```
Requesting write of metadata to DynamoDB can be done as;
```
./bin/ddgen --mirror --useS3
```
This will use global S3 address, and assumes aws credentials are already reachable through environment variables, home folder file or any other means described in aws sdk.

#### DynamoDB support

DDGen supports [DynamodB](https://aws.amazon.com/dynamodb/). If it is desired to forward generated stream metadata to AWS DynamoDB, corresponding aws sdk should be build.
First install the necessary dependencies
```
sudo apt-get install libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev
```
Then configure with dynamodb support
```
./configure.sh --use-dynamodb
./make
```
Requesting write of metadata to DynamoDB can be done as;
```
./bin/ddgen --mirror --useDb
```
This will use global DynamoDB address, and assumes aws credentials are already reachable through environment variables, home folder file or any other means described in aws sdk.


### Use of DynamoDb and S3 with credentials as environment file ###
```
docker run --env-file docker/aws/env.list ddgen /ddgen/ddgen --nc 2 --dc 10 --ds 30 --mirror --useDb --useS3
```
Here env.list should be a file as
```
AWS_DEFAULT_REGION=us-east-2
AWS_ACCESS_KEY_ID=SomeKeyId
AWS_SECRET_ACCESS_KEY=SomeSecretAccessKey
```

### Use of cloud formation
There are cloud formation templates that can be used to deploy a test stack in AWS.
Be sure to enter the credentials either as environment variables or as file.
Then fill the parameters file `./cloudformations/parameters.yaml` due to your wish
Then create the stack as,
```
aws cloudformation create-stack --stack-name ddgen --template-body file://./cloudformation/template.yaml --parameters file://./cloudformation/parameters.yaml --region=us-east-2
```
As usual, genereated stacks can be updated or delated through usual aws commands
```
aws cloudformation update-stack --stack-name ddgen --template-body file://./cloudformation/template.yaml --parameters file://./cloudformation/parameters.yaml --region=us-east-2
```
```
aws cloudformation describe-stack-events --stack-name ddgen
```
```
aws cloudformation delete-stack --stack-name ddgen
```

### Use of local DynamoDB ###
amazon provides a local db that can be spined up with
```
docker run -p 8000:8000 amazon/dynamodb-local -sharedDb
```
For local DynamoDB credentials are required but actually not used. Simple set of environment variables would be fine;
```
export AWS_DEFAULT_REGION=us-east-2
export AWS_ACCESS_KEY_ID=SomeKeyId
export AWS_SECRET_ACCESS_KEY=SomeSecretAccessKey
```
In order to use local (or any specific DynamoDB), there is `--dbPath` option
```
./bin/ddgen --mirror --nc 10 --dbPath http://localhost:8000
```

### local DynamoDB inspection ###

Tables can be inspected by:
```
aws --region us-east-2 dynamodb list-tables --endpoint-url http://sifa.3amlabs.net:8000
aws --region us-east-2 dynamodb describe-table --table DDGenTest --endpoint-url http://localhost:8000
```
And table content can be observed with:
```
aws dynamodb scan --region us-east-2 --table-name DDGenTest --endpoint-url http://localhost:8000 | grep Duration | wc
aws dynamodb scan --region us-east-2 --table-name DDGenTest --endpoint-url http://localhost:8000
```

Sample table
```
{
    "Streams": { "L": [ { "M": { "DestinationIp": { "S": "172.24.101.55" },
                                 "Tones": { "L": [ { "M": { "Amplitude": { "N": "0.749523" }, "Frequency": { "N": "1.019567" }, "Phase": { "N": "-2.940207" } } } ] },
                                 "SourceIp": { "S": "172.24.101.54" },
                                 "Ssrc": { "N": "4273662652" },
                                 "SequenceNumber": { "N": "7580" },
                                 "Payload": { "N": "8" },
                                 "SourcePort": { "N": "32514" },
                                 "DestinationPort": { "N": "32514" },
                                 "Timestamp": { "N": "2634425776" } } },
                        { "M": { "DestinationIp": { "S": "172.24.101.54" },
                                 "Tones": { "L": [ { "M": { "Amplitude": { "N": "0.420526" }, "Frequency": { "N": "2.111433" }, "Phase": { "N": "1.093815" } } } ]  },
                                 "SourceIp": { "S": "172.24.101.55" },
                                 "Ssrc": { "N": "3846789339" },
                                 "SequenceNumber": { "N": "57978" },
                                 "Payload": { "N": "8" },
                                 "SourcePort": { "N": "32514" },
                                 "DestinationPort": { "N": "32514" },
                                 "Timestamp": { "N": "495317806" } } } ] },
    "Name": { "S": "172.24.101.54:32514_to_172.24.101.55:32514_at_20200218_155557_532435" },
    "Duration": { "N": "100" }
}
```

### Profiling
Small documentation about possible profiling options for ddgen may be seen through [profiling a c++ application](https://sifaserdarozen.wordpress.com/2016/01/01/a-small-survey-for-profiling-a-c-application)
* poor mans profiler : see /profilers/pmp.sh.readme
* gprof              : see /prifilers/gprof.readme, code should be compiled with "make gprof=yes" to enable -pg flag
* perf               : see /profilers/perf.readme
* callgrind          : see /profilers/callgrind.readme

### Thanks to used 3rd Parties
Catch for unit tests: [Catch](https://github.com/philsquared/Catch)
Http library used to provide kubernetes readiness / liveliness probes : [libhttp](http://www.libhttp.org)
DynamoDB and S3 access : [AWS c++ SDK](https://github.com/aws/aws-sdk-cpp)
Online markdown editor used to create this page: [MEditor](https://pandao.github.io/editor.md/en.html)

### License
Please check individual licanse information of used 3rd parties. Besides, there is a great deal of g722 taken from ITU reference implementation that should be treated as ITU wishes.
All remaining part is MIT.
