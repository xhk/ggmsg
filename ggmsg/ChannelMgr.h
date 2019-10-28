#pragma once
// 从boost echo的例子中修改而来

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>

#include "Channel.h"
#include <functional>

#include "ggmsg.h"

using boost::asio::ip::tcp;

#ifdef __NET_MSG_LIB__
#define __net_msg_interface__ __declspec(dllexport)
#else
#define __net_msg_interface__ __declspec(dllimport)

#endif // __NET_MSG_LIB__



class __net_msg_interface__ ChannelMgr
{
public:
	ChannelMgr()
		:acceptor_(nullptr)
	{
		m_nServiceID = -1;
	}

	~ChannelMgr() {
		if (acceptor_) {
			delete acceptor_;
		}
	}

	// 向指定服务发送数据
	bool SendToService(int nServiceID, const void *pData, int nDataLen);

	// 向所有服务发送数据
	bool SendToAllService(const void *pData, int nDataLen);

	// 向指定连接发送数据
	bool SendToConnect(int nConnectID, const void *pData, int nDataLen);
	
	void Start(int nServiceID, short port, 
		FnOnPassiveConnect fnOnPassiveConnect,
		FnOnReceiveMsg fnOnReceiveMsg);
	void Stop();

	// 主动发起连接
	bool Connect(const std::string & strHost, short sPort,
		FnOnPositiveConnect fnOnPositiveConnect,
		FnOnReceiveMsg fnOnReceiveMsg);

	typedef std::vector<std::shared_ptr<Channel> > ChannelList;
	void GetChannelList(ChannelList &Channel_list);

	int GetServiceID() {
		return m_nServiceID;
	}

	void AddService(std::shared_ptr<Channel> s);

	// 移除会话
	void DeleteService(std::shared_ptr<Channel> s);

private:
	void do_accept();
	bool InternalConnect(const std::string & strHost, short sPort);
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
	FnOnPositiveConnect m_fnOnPositiveConnect;
	FnOnPassiveConnect m_fnOnPassiveConnect;
	FnOnReceiveMsg m_fnOnReceiveMsg;


	friend Channel;
};
