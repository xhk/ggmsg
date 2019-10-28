// ggmsg.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "ChannelMgr.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


__int64 ggmsg_Create()
{
	return (__int64) new ChannelMgr();
}

void ggmsg_Destory(__int64 c) {
	auto pMgr = (ChannelMgr*)c;
	if (pMgr) {
		delete pMgr;
	}
}

void ggmsg_Start(__int64 c, int nServiceID, short port,
	FnOnPassiveConnect fnOnPassiveConnect,
	FnOnReceiveMsg fnOnReceiveMsg)
{
	auto pMgr = (ChannelMgr*)c;
	if (!pMgr) {
		return;
	}

	pMgr->Start(nServiceID, port, fnOnPassiveConnect, fnOnReceiveMsg);
}

// 主动发起连接
int ggmsg_Connect(
	__int64 c,
	const char *szHost, short sPort,
	FnOnPositiveConnect fnOnPositiveConnect,
	FnOnReceiveMsg fnOnReceiveMsg)
{
	auto pMgr = (ChannelMgr*)c;
	if (!pMgr) {
		return -1;
	}

	if (!pMgr->Connect(szHost, sPort, fnOnPositiveConnect, fnOnReceiveMsg)) {
		return -1;
	}

	return 0;
}

void ggmsg_Stop(__int64 c) {
	auto pMgr = (ChannelMgr*)c;
	if (!pMgr) {
		return;
	}

	pMgr->Stop();
}


// 向指定服务发送数据
int ggmsg_SendToService(__int64 c, int nServiceID, const void *pData, int nDataLen)
{
	auto pMgr = (ChannelMgr*)c;
	if (!pMgr) {
		return -1;
	}

	if (!pMgr->SendToService(nServiceID, pData, nDataLen)) {
		return -1;
	}

	return 0;
}

// 向指定连接发送数据
int ggmsg_SendToConnect(__int64 c, int nConnectID, const void *pData, int nDataLen)
{
	auto pMgr = (ChannelMgr*)c;
	if (!pMgr) {
		return -1;
	}

	if (!pMgr->SendToConnect(nConnectID, pData, nDataLen)) {
		return -1;
	}

	return 0;
}


#ifdef __cplusplus
}
#endif // __cplusplus