#pragma once

#ifdef WIN32
#ifdef __NET_MSG_LIB__
#define __net_msg_interface__ __declspec(dllexport)
#else
#define __net_msg_interface__ __declspec(dllimport)
#endif // __NET_MSG_LIB__
#else
#define __net_msg_interface__
#endif
#include <string>

class ChannelListener 
{
public:
	virtual void OnPositiveConnect(int nServiceID, int nConnectID) {};
	virtual void OnPositiveDisConnect(int nServiceID, int nConnectID) {};
	virtual void OnPassiveConnect(int nSeviceID, int nConnectID) {};
	virtual void OnPassiveDisConnect(int nServiceID, int nConnectID) {};
	virtual void OnReceiveMsg(int nServiceID, int nConnectID, const char* pData, int nDataLen) {};
};

class __net_msg_interface__ ChannelNode
{
public:
	static ChannelNode* Create(ChannelListener *pListener);
	static void Destory(ChannelNode* pNode);
	virtual void Start(int nServiceID, short port) = 0;
	virtual void Stop() = 0;

	// ������������
	virtual bool Connect(const char *strHost, short sPort) = 0;


	virtual int GetServiceID() = 0;

	// ��ָ������������
	virtual bool SendToService(int nServiceID, const char* pData, int nDataLen) = 0;

	// �����з���������
	virtual bool SendToAllService(const char* pData, int nDataLen) = 0;

	// ��ָ�����ӷ�������
	virtual bool SendToConnect(int nConnectID, const char* pData, int nDataLen) = 0;
};