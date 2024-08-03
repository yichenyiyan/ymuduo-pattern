4核 伪6G内存 压力测试结果如下：

yichen@yichen-Ubuntu:/mnt/hgfs/Share/TMP/mymuduo/http$ ls
ab_http_stress_test.sh  HttpContext.cc  HttpRequest.h    HttpResponse.h  http_server2     HttpServer.cc  Server.log
Favicon.h               HttpContext.h   HttpResponse.cc  http_server     http-server2.cc  HttpServer.h
yichen@yichen-Ubuntu:/mnt/hgfs/Share/TMP/mymuduo/http$ ab -n 1000000 -c 1000 -k http://127.0.0.1:8000/hello
This is ApacheBench, Version 2.3 <$Revision: 1879490 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 100000 requests
Completed 200000 requests
Completed 300000 requests
Completed 400000 requests
Completed 500000 requests
Completed 600000 requests
Completed 700000 requests
Completed 800000 requests
Completed 900000 requests
Completed 1000000 requests
Finished 1000000 requests


Server Software:        yichenMuduo
Server Hostname:        127.0.0.1
Server Port:            8000

Document Path:          /hello
Document Length:        38 bytes

Concurrency Level:      1000
Time taken for tests:   920.902 seconds
Complete requests:      1000000
Failed requests:        0
Keep-Alive requests:    1000000
Total transferred:      148000000 bytes
HTML transferred:       38000000 bytes
Requests per second:    1085.89 [#/sec] (mean)
Time per request:       920.902 [ms] (mean)
Time per request:       0.921 [ms] (mean, across all concurrent requests)
Transfer rate:          156.95 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   1.0      0      52
Processing:     9  920 349.8    906   18225
Waiting:        8  920 349.8    906   18225
Total:          9  920 350.4    906   18239

Percentage of the requests served within a certain time (ms)
  50%    906
  66%    954
  75%    985
  80%   1005
  90%   1060
  95%   1110
  98%   1188
  99%   1293
 100%  18239 (longest request)
