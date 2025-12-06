#pragma once
// ��boost echo���������޸Ķ���

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>

#include "ChannelNode.h"
#include "Channel.h"

using boost::asio::ip::tcp;


class ChannelMgr : public ChannelNode
{
public:
	ChannelMgr(ChannelListener *pListener)
		:acceptor_(nullptr)
	{
		m_nServiceID = -1;
		m_pListener = pListener;
		working_ = false;
	}

	~ChannelMgr() {
		if (acceptor_) {
			delete acceptor_;
		}
	}

	ChannelListener* Listener() { return m_pListener; }

	// ��ָ������������
	bool SendToService(int nServiceID, const char *pData, int nDataLen);

	// �����з���������
	bool SendToAllService(const char *pData, int nDataLen);

	// ��ָ�����ӷ�������
	bool SendToConnect(int nConnectID, const char *pData, int nDataLen);
	
	void Start(int nServiceID, short port);
	void Stop();

	// ������������
	bool Connect(const char * strHost, short sPort);

	typedef std::vector<std::shared_ptr<Channel> > ChannelList;
	void GetChannelList(ChannelList &Channel_list);

	int GetServiceID() {
		return m_nServiceID;
	}

	void AddService(std::shared_ptr<Channel> s);

	// �Ƴ��Ự
	void DeleteService(std::shared_ptr<Channel> s);

protected:
	virtual void OnPositiveConnect(int nServiceID, int nConnectID);
	virtual void OnPositiveDisConnect(int nServiceID, int nConnectID);
	virtual void OnPassiveConnect(int nSeviceID, int nConnectID);
	virtual void OnPassiveDisConnect(int nSeviceID, int nConnectID);
	virtual void OnReceiveMsg(int nServiceID, int nConnectID, const void* pMsg, int nMsgLen);

private:
	void do_accept();
	bool InternalConnect(const std::string strHost, short sPort);
	void DoConnect(tcp::socket *pConnectSocket, tcp::resolver::results_type *pEndPoint, time_t_timer *pTimer);
	
	void StartIoThread();

	int m_nServiceID;
	boost::asio::io_context m_ioContext;
	tcp::acceptor *acceptor_;
	std::mutex Channels_lock_;
	std::unordered_map<int, std::shared_ptr<Channel> > Channels_;
	bool working_;
	std::thread io_thread_;

	std::unordered_map<int, std::shared_ptr<Channel> > m_services;
	/*FnOnPositiveConnect m_fnOnPositiveConnect;
	FnOnPositiveDisConnect m_fnOnPositiveDisConnect;
	FnOnPassiveConnect m_fnOnPassiveConnect;
	FnOnPassiveDisConnect m_fnOnPassiveDisConnect;
	FnOnReceiveMsg m_fnOnReceiveMsg;*/

	ChannelListener* m_pListener;
		friend Channel;
};