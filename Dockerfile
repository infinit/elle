FROM alpine:latest AS drake-build

RUN apk add --no-cache gcc musl-dev python3 python3-dev py-pip
ADD . /root/
RUN pip3 install -r /root/requirements.txt
RUN /root/src/bin/drake /root --prefix=/usr //install

FROM alpine:latest

RUN apk add --no-cache python3
COPY --from=drake-build /usr/bin/drake /usr/bin/drake
COPY --from=drake-build /usr/lib/python3.6/site-packages/ /usr/lib/python3.6/site-packages/
ENTRYPOINT /usr/bin/drake
