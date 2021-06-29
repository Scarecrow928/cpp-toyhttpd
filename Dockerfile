FROM alpine:3.13
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories \
    && apk update \
    && apk upgrade \
    && apk add g++ make
WORKDIR /src
COPY ./* /src/
RUN make clean && make

FROM alpine:3.13
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories \
    && apk update \
    && apk upgrade \
    && apk add libgcc libstdc++
WORKDIR /http
COPY --from=0 /src/toyhttpd /http/toyhttpd
EXPOSE 12000
VOLUME [ "/http/htdocs", "/http/default" ]
CMD [ "/http/toyhttpd" ]