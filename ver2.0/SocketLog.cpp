﻿/*<FH>*************************************************************************
* 文件名称:SocketLog.cpp
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

#include <csignal>
#include "SocketLog.h"
#include "log.h"

#define LOGE(fmt, ...)      LOG(fmt, ##__VA_ARGS__)

//#define DEBUG_THIS_FILE
#ifdef DEBUG_THIS_FILE
    #define LOGD(fmt, ...)  LOG(fmt, ##__VA_ARGS__)
#else
    #define LOGD(fmt, ...)  ((void)0)
#endif

SocketLog* SocketLog::getInstance() {
    static auto socketLog = new SocketLog();
    return socketLog;
}

void SocketLog::post(const void* buf, size_t len) {
    if (!inited)
        return;

    msgQueueMutex.lock();
    msgQueue.push(Msg(buf, len));
    msgQueueMutex.unlock();
    msgQueueCondition.notify_one();
}

void SocketLog::post(const char* str) {
    post(str, strlen(str));
}

void SocketLog::post(std::string str) {
    post(str.c_str());
}

void SocketLog::send(const void* buf, size_t len) {
    std::lock_guard<std::mutex> lockStream(streamMutex);
    LOGD("SocketLog::send: len=%ld", len);

    if (!inited)
        return;

    int size = connectedStreams.size();
    if (size == 0) {
        LOGD("no stream connected now! will not send!");
    } else {
        LOGD("stream connected num=%ld", size);
    }

	vector<TCPStream*>::iterator iter;
    for(iter = connectedStreams.begin(); iter != connectedStreams.end();) 
	{
        auto& stream = *iter;

        LOGD("try to send to stream:%p, len=%ld", *iter, len);
        int ret = stream->send(buf, len);
        if (ret == -1) 
		{
            LOGE("send failed! delete(close) stream");
			delete stream;
            iter = connectedStreams.erase(iter);// 
            //break;
            //iter++;
        } 
		else 
		{
            LOGD("send success! send len=%ld", len);
            iter++;
        }
		
    }

	LOGD("brak\r\n");
}

void SocketLog::send(const char* str) {
    send(str, strlen(str));
}

void SocketLog::send(std::string str) {
    send(str.c_str());
}

void SocketLog::disconnectAllStreams() {
    std::lock_guard<std::mutex> lockStream(streamMutex);

    for(auto stream:connectedStreams) {
        stream->send("SocketLog say Bye!\n");
        delete stream;
    }
    connectedStreams.clear();
}

SocketLog::SocketLog() 
{
    signal(SIGPIPE, SIG_IGN);
	
    port = 5000;//监听端口
    acceptor = nullptr;
    inited = false;
	
    START:
    acceptor = new TCPAcceptor(port);

    if (acceptor->start() == 0) 
	{
        LOG("SocketLog start success! port:%d", port);
        startAcceptThread();
        startSendThread();
        inited = true;
    } 
	else 
	{
        LOGE("SocketLog start failed! check your port:%d", port);
        inited = false;
        delete acceptor;
        acceptor = nullptr;

        port++;
        goto START;
    }
}

void SocketLog::startSendThread() {
    new std::thread([this]{
        LOGD("sendThread running...");
        Msg msg;

        while (true) 
		{
            {
                std::unique_lock<std::mutex> lock(msgQueueMutex);
                LOGD("msgQueue.size=%ld, msgQueueCondition will %s", msgQueue.size(),
                     msgQueue.empty() ? "waiting..." : "not wait!");
                msgQueueCondition.wait(lock, [this] { return !msgQueue.empty(); });
                LOGD("msgQueueCondition wake! msgQueue.size=%ld", msgQueue.size());

                msg = std::move(msgQueue.front());
                msgQueue.pop();
            }

            LOGD("try to send data.len=%ld", msg.len);
            send(msg.data, msg.len);
        }
    });
}

void SocketLog::startAcceptThread() 
{
    new std::thread([this]{
        LOGD("acceptThread running...");

        while (true) 
		{
            LOGD("wait for accept...");
            TCPStream* stream = acceptor->accept();

            if (stream) 
			{
                LOGD("catch an accept:%p", stream);
                stream->send("Welcome to SocketLog!\n");
                streamMutex.lock();
                connectedStreams.push_back(stream);
                streamMutex.unlock();
            } 
			else 
			{
                LOGE("accept failed! will stop acceptThread!");
                inited = false;
                delete acceptor;
                acceptor = nullptr;
                return;
            }
        }
    });
}
