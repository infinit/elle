FROM ubuntu:17.10

RUN apt-get install openssl

WORKDIR /app

COPY linux64/src/sample /app/bin/sample
COPY linux64/lib /app/lib

ENTRYPOINT ["/app/bin/sample"]
