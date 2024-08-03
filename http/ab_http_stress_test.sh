#!/bin/bash

# 默认参数
CONCURRENCY=1000
TOTAL_REQUESTS=1000000
URL="http://127.0.0.1:8000/hello"

# 使用 ab 进行压力测试
ab -n $TOTAL_REQUESTS -c $CONCURRENCY -k $URL

# 输出结果
echo "压力测试完成: 并发请求数 = $CONCURRENCY, 总请求数 = $TOTAL_REQUESTS, URL = $URL"
