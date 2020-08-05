FROM alpine:3.11 AS build
RUN apk update && apk add --no-cache gcc g++ make cmake git zlib-dev openssl-dev curl-dev
WORKDIR /src/
COPY . /src
RUN ["chmod", "+x", "configure.sh"]
RUN ["./configure.sh", "--use-dynamodb", "--use-s3"]
RUN ["make"]

FROM alpine:3.11
RUN apk add --no-cache libstdc++ tini openssl-dev openssl libcurl
WORKDIR /ddgen
COPY --from=build /src/bin/. /ddgen/.
COPY ./docker/certificate /ddgen/.

#ENTRYPOINT ["/bin/sh","-c","trap : TERM INT; tail -f /dev/null"]
#ENTRYPOINT ["/bin/ddgen_utests"]
#ENTRYPOINT ["/bin/ddgen --nc 10 --dc 6 --socket 10.228.210.40 28008 --mirror"]

COPY docker/start.sh /ddgen/
COPY docker/cert-generate.sh /ddgen/

ENTRYPOINT ["tini", "--", "./start.sh"]
