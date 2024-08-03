#!/bin/bash

# 设置目标地址和端口
TARGET="127.0.0.1"
PORT="8000"
MESSAGE="yichen"
NUM_CONNECTIONS=1000

# 向本地回环 IP 的 8000 端口发起 1000 个连接，并发送字符 "yichen"
for i in $(seq 1 $NUM_CONNECTIONS); do
    # 使用 exec 通过文件描述符进行连接，并发送消息
    (exec 3<>/dev/tcp/$TARGET/$PORT && echo "$MESSAGE" >&3) &
done

# 等待所有后台进程完成
wait

echo "Finished sending $NUM_CONNECTIONS connections with message '$MESSAGE' to $TARGET:$PORT."
