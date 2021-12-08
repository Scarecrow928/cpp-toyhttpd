FROM alpine:3.15 AS builder
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories \
    && apk update \
    && apk upgrade \
    && apk add make cmake boost-dev gcc g++
WORKDIR /src
COPY . /src
RUN cmake . && cmake --build . -j4

FROM alpine:3.15
EXPOSE 12000
VOLUME [ "/var/www/html" ]
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories \
    && apk update \
    && apk upgrade \
    && apk add libgcc libstdc++ \
    && rm -f /var/cache/apk/*
COPY --from=builder /src/myhttpd /usr/bin/
COPY --from=builder /src/lib* /usr/lib/
CMD [ "myhttpd" ]