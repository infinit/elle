FROM ubuntu:16.04 AS trusty-ci

RUN apt-get update && apt-get install -y git python3.5 python3-pip && rm -rf /var/lib/apt/lists/*

FROM trusty-ci AS trusty-check

ADD requirements.txt .
RUN pip3 install -r requirements.txt
