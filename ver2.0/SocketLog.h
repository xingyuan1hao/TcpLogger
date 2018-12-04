/*<FH>*************************************************************************
* 文件名称:SocketLog.h
* 文件标识:
* 内容摘要:
* 其它说明:
* 当前版本: V1.0
* 作    者:
* 完成日期:
* 修改记录1:
*     修改日期:
*     版 本 号:
*     修 改 人:
*     修改内容:
**<FH>************************************************************************/


#ifndef SOCKETLOG_H
#define SOCKETLOG_H

#include <cstring>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include "lib/tcpacceptor.h"
#include "msg.h"

class SocketLog {
public:
    static SocketLog* getInstance();

    // post data to queue will be send automatic later
    void post(const void* buf, size_t len);
    void post(const char* str);
    void post(std::string str);

    // send data immediately with thread safe
    void send(const void* buf, size_t len);
    void send(const char* str);
    void send(std::string str);

    void disconnectAllStreams();

private:
    SocketLog();
    void startAcceptThread();
    void startSendThread();

    int port;
    TCPAcceptor* acceptor;
    bool inited;

    vector<TCPStream*> connectedStreams;
    std::mutex streamMutex;

    std::queue<Msg> msgQueue;
    std::mutex msgQueueMutex;
    std::condition_variable msgQueueCondition;
};

#endif //SOCKETLOG_H
