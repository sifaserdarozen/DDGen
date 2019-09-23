FROM alpine:3.8 AS build
RUN apk update && apk add gcc g++ make
WORKDIR /src/
COPY . /src
RUN make

FROM alpine:3.8
RUN apk add --no-cache libstdc++ tini openssl-dev
COPY --from=build /src/bin/. /bin/.
COPY *.pem /.

#ENTRYPOINT ["/bin/sh","-c","trap : TERM INT; tail -f /dev/null"]
#ENTRYPOINT ["/bin/ddgen_utests"]
#ENTRYPOINT ["/bin/ddgen --nc 10 --dc 6 --socket 10.228.210.40 28008 --mirror"]

ADD docker/start.sh /

ENTRYPOINT ["tini", "--", "/start.sh"]
