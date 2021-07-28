FROM alpine:3.13 as builder
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories \
    && apk update \
    && apk upgrade \
    && apk add g++ make boost-dev
WORKDIR /src
COPY ./* /src/
RUN make clean && make

FROM alpine:3.13
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories \
    && apk update \
    && apk upgrade \
    && apk add libgcc libstdc++ \
    && rm -f /var/cache/apk/*
WORKDIR /http
COPY --from=builder /src/toyhttpd /http/toyhttpd
EXPOSE 12000
VOLUME [ "/http/htdocs", "/http/default" ]
CMD [ "/http/toyhttpd" ]