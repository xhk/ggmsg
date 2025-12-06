// stdafx.h: 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件
//

#pragma once

#ifdef _WINDOWS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>

#endif

// C标准库头文件
#include <cstring>    // 用于memcpy、memset等函数
#include <cstdarg>    // 用于va_list等可变参数相关函数
#include <cstdio>     // 用于sprintf等函数

// 在此处引用程序需要的其他标头
#define __NET_MSG_LIB__
#define __GGMSG_LIB__