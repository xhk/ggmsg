#include "stdafx.h"

#include "ggmsg.h"
#include "ChannelMgr.h"


class MyListener : public ChannelListener 
{
public:
	MyListener() {
		m_fnOnPassiveConnect = nullptr;
		m_fnOnPassiveDisConnect = nullptr;
		m_fnOnPositiveConnect = nullptr;
		m_fnOnPositiveDisConnect = nullptr;
		m_fnOnReceiveMsg = nullptr;
	}
	virtual void OnPositiveConnect(int nServiceID, int nConnectID) {
		if (m_fnOnPositiveConnect) {
			m_fnOnPositiveConnect(nServiceID, nConnectID);
		}
	};
	virtual void OnPositiveDisConnect(int nServiceID, int nConnectID) {
		if (m_fnOnPositiveDisConnect) {
			m_fnOnPositiveDisConnect(nServiceID, nConnectID);
		}
	};
	virtual void OnPassiveConnect(int nSeviceID, int nConnectID) {
		if (m_fnOnPassiveConnect)
			m_fnOnPassiveConnect(nSeviceID, nConnectID);
	};
	virtual void OnPassiveDisConnect(int nServiceID, int nConnectID) {
		if (m_fnOnPassiveDisConnect)
			m_fnOnPassiveDisConnect(nServiceID, nConnectID);
	};
	virtual void OnReceiveMsg(int nServiceID, int nConnectID, const char* pData, int nDataLen) {
		if (m_fnOnReceiveMsg)
			m_fnOnReceiveMsg(nServiceID, nConnectID, pData, nDataLen);
	};

	// 成员变量的类型声明
	FnOnPassiveConnect m_fnOnPassiveConnect;
	FnOnPassiveDisConnect m_fnOnPassiveDisConnect;
	FnOnPositiveConnect m_fnOnPositiveConnect;
	FnOnPositiveDisConnect m_fnOnPositiveDisConnect;
	FnOnReceiveMsg m_fnOnReceiveMsg;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


__int64 ggmsg_Create()
{
	auto l = new MyListener();
	return (__int64) new ChannelMgr(l);
}

void ggmsg_Destory(__int64 c) {
	auto pMgr = (ChannelMgr*)c;
	if (pMgr) {
		auto l = (MyListener*)pMgr->Listener();
		if (l) {
			delete l;
		}
		delete pMgr;
	}
}

void ggmsg_Start(__int64 c, int nServiceID, short port,
	FnOnPassiveConnect fnOnPassiveConnect,
	FnOnPassiveDisConnect fnOnPassiveDisConnect,
	FnOnReceiveMsg fnOnReceiveMsg)
{
	auto pMgr = (ChannelMgr*)c;
	if (!pMgr) {
		return;
	}

	auto l = (MyListener*)pMgr->Listener();
	l->m_fnOnPassiveConnect = fnOnPassiveConnect;
	l->m_fnOnPassiveDisConnect = fnOnPassiveDisConnect;
	l->m_fnOnReceiveMsg = fnOnReceiveMsg;

	pMgr->Start(nServiceID, port);
}

// 主动发起连接
int ggmsg_Connect(
	__int64 c,
	const char *szHost, short sPort,
	FnOnPositiveConnect fnOnPositiveConnect,
	FnOnPositiveDisConnect fnOnPositiveDisConnect,
	FnOnReceiveMsg fnOnReceiveMsg)
{
	auto pMgr = (ChannelMgr*)c;
	if (!pMgr) {
		return -1;
	}

	auto l = (MyListener*)pMgr->Listener();
	l->m_fnOnPositiveConnect = fnOnPositiveConnect;
	l->m_fnOnPositiveDisConnect = fnOnPositiveDisConnect;
	l->m_fnOnReceiveMsg = fnOnReceiveMsg;
	if (!pMgr->Connect(szHost, sPort)) {
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

	if (!pMgr->SendToService(nServiceID, (char*)pData, nDataLen)) {
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

	if (!pMgr->SendToConnect(nConnectID, (char*)pData, nDataLen)) {
		return -1;
	}

	return 0;
}


#ifdef __cplusplus
}
#endif // __cplusplus