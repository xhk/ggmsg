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
		
	}

	~ChannelMgr() {
		if (acceptor_) {
			delete acceptor_;
		}
	}

	// 向指定服务发送数据
	bool Send(int nServiceID, const void *pData, int nDataLen);
	bool SendToAll(const void *pData, int nDataLen);

	using DataHandler = std::function<void(int nServiceID, const void *pPacket, int nLength)>;
	void Start(int nServiceID, short port, DataHandler);
	void Stop();

	//bool Connect(int nServiceID);
	bool Connect(const std::string & strHost, short sPort);



	typedef std::vector<std::shared_ptr<Channel> > ChannelList;
	void GetChannelList(std::vector<std::shared_ptr<Channel> > &Channel_list);

	// 移除会话
	void earse(std::shared_ptr<Channel> s ){
		std::stringstream ip_port;
		ip_port << s->remote_ip() << ":" << s->remote_port();
		{
			std::lock_guard<std::mutex> lk(Channels_lock_);
			Channels_.erase(ip_port.str());
		}
	}

	int GetServiceID() {
		return m_nServiceID;
	}

	void AddService(std::shared_ptr<Channel> s) {
		std::lock_guard<std::mutex> lk(Channels_lock_);
		m_services.insert({ s->GetServiceID(), s });
	}

private:
	void do_accept();
	void DoConnect(tcp::socket *pConnectSocket, tcp::resolver::results_type *pEndPoint, time_t_timer *pTimer);
	
	int m_nServiceID;
	boost::asio::io_context m_ioContext;
	tcp::acceptor *acceptor_;
	std::mutex Channels_lock_;
	std::unordered_map<std::string, std::shared_ptr<Channel> > Channels_;
	bool working_;
	std::thread io_thread_;

	std::unordered_map<int, std::shared_ptr<Channel> > m_services;
	DataHandler m_handler;


	friend Channel;
};
