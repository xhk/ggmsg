# ggmsg
分布式消息中间件

基于boost asio实现的分布式网络消息中间件。

通信的两端对等，无客户端、服务端之分。

通信都是双方相互的，A与B建立连通后，可以进行A问B答、A推送到B、B问A答、B推送到A；不分模式，只是发送消息（数据），由于socket本身全双工，所以一个socket就可以满足大多数情况了。不像MQ之类的消息中间件，有服务端、客户端之分；还分多种通信模式。

ggmsg 采用两端直连模式，类似于nanomsg，没有broker，性能更优，没有中间商赚差价，你懂的。严格上来讲不算是“中间”件。

![3个对等节点示意图](https://github.com/xhk/ggmsg/blob/master/doc/node_3.png?raw=true)


# 性能测试
- 在AMD Ryzen 7 5800U 基准频率1.9GHz 的机器上 100万次 1kb的消息发送 0.7~0.8s完成, jggmsg在1~1.9s。
- 在Intel Core i7-6700 基准频率3.41GHz 的机器上 100万次 1kb的消息发送 2.5~2.6s完成。

# 编译

## Windows
- `mkdir build`
- `cd build`
- `cmake .. -DCMAKE_TOOLCHAIN_FILE=d:\github.com\vcpkg\scripts\buildsystems\vcpkg.cmake`
- `cmake --build .`

# java 

- `swig -c++ -java -package jggmsg -outdir java_src -o jggmsg.cpp jggmsg.i`
- `cd java_src`
- `javac *.java -d ./`
- `jar cf jggmsg.jar jggmsg`

# charpe

- `swig -c++ -csharp -namespace nggmsg -outdir csharp_src -o nggmsg.cpp nggmsg.i`

