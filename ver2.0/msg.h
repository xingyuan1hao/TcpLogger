/*<FH>*************************************************************************
* 文件名称:msg.h
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


#ifndef MSG_H
#define MSG_H

#include <cstddef>
#include <cstring>

struct Msg {
    typedef unsigned char byte;

    unsigned int len;
    byte* data;

    inline void initMsg(const void* data = nullptr, unsigned int len = 0) {
        if (data == NULL || len == 0) {
            this->len  = 0;
            this->data = NULL;
            return;
        }

        this->len  = len;
        this->data = new byte[len];
        memcpy(this->data, data, len);
    }

    inline void moveMsg(Msg& msg) {
        this->len   = msg.len;
        this->data  = msg.data;
        msg.data = NULL;
    }

    explicit Msg(const void* data = NULL, size_t len = 0) {
        initMsg(data, len);
    }

    Msg(const Msg& msg) {
        initMsg(msg.data, msg.len);
    }

    Msg(Msg&& msg) noexcept {
        moveMsg(msg);
    }

    Msg& operator=(const Msg& msg) {
        if (&msg == this)
            return *this;

        delete[] this->data;
        initMsg(msg.data, msg.len);
        return *this;
    }

    Msg& operator=(Msg&& msg) noexcept {
        if (&msg == this)
            return *this;

        delete[] this->data;
        moveMsg(msg);
        return *this;
    }

    ~Msg() {
        delete[] data;
    }
};

#endif //MSG_H
