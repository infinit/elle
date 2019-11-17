FROM ubuntu:18.10 AS cosmic-ci

RUN apt-get update && apt-get install -y ccache fuse git gnutls-bin python3.5 python3-pip valgrind && rm -rf /var/lib/apt/lists/*
RUN ln -s /usr/lib/x86_64-linux-gnu/libgnutls.so.30 /usr/lib/x86_64-linux-gnu/libgnutls.so
ADD drake/requirements.txt .
RUN pip3 install -r requirements.txt
