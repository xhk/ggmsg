#pragma once

// Function pointer type definitions
typedef int(*FnOnPositiveConnect)(int nServiceID, int nConnectID);
typedef int(*FnOnPositiveDisConnect)(int nServiceID, int nConnectID);
typedef int(*FnOnPassiveConnect)(int nSeviceID, int nConnectID);
typedef int(*FnOnPassiveDisConnect)(int nSeviceID, int nConnectID);
typedef int(*FnOnReceiveMsg)(int nServiceID, int nConnectID, const void *pMsg, int nDataLen);

#ifdef _WINDOWS
#ifdef __GGMSG_LIB__
#define __ggmsg_interface__ __declspec(dllexport)
#else
#define __ggmsg_interface__ __declspec(dllimport)
#endif
#else
#define __ggmsg_interface__
typedef long long __int64;
#endif

#ifdef __cplusplus
extern "C" {
#endif

__int64 __ggmsg_interface__ ggmsg_Create();
void __ggmsg_interface__ ggmsg_Destory(__int64 c);
void __ggmsg_interface__ ggmsg_Start(__int64 c, int nServiceID, short port, FnOnPassiveConnect fnOnPassiveConnect, FnOnPassiveDisConnect fnOnPassiveDisConnect, FnOnReceiveMsg fnOnReceiveMsg);
int __ggmsg_interface__ ggmsg_Connect(__int64 c, const char *szHost, short sPort, FnOnPositiveConnect fnOnPositiveConnect, FnOnPositiveDisConnect fnOnPositiveDisConnect, FnOnReceiveMsg fnOnReceiveMsg);
void __ggmsg_interface__ ggmsg_Stop(__int64 c);
int __ggmsg_interface__ ggmsg_SendToService(__int64 c, int nServiceID, const void *pData, int nDataLen);
int __ggmsg_interface__ ggmsg_SendToConnect(__int64 c, int nConnectID, const void *pData, int nDataLen);

#ifdef __cplusplus
}
#endif
