﻿/*<FH>*************************************************************************
* 文件名称:log.h
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


#ifndef LOG_H
#define LOG_H


#include <cstdio>
#define LOG(fmt, ...)   do{printf(fmt, ##__VA_ARGS__); printf("\n");} while(0)

#endif //LOG_H
