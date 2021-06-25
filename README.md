### 简介 
仿照[tinyhttpd](https://github.com/cbsheng/tinyhttpd)，用C++改写的静态http服务器

### 编译
#### 直接编译
- 安装`g++`和`make`
- `make` 或 `make debug`
- `export PORT=port number`，默认`12000`
- `./toyhttpd`
 
#### docker
- `docker build -t mytag .`
- `docker run -p 12000:12000 -v $(pwd)/htdocs:/http/htdocs -v $(pwd)/default:/http/default mytag`

### demo
[有反向代理](https://cn1.scarecrow928.com:10443/toyhttpd/)

[直连](http://us1.scarecrow928.com:12000)