#include "stdafx.h"

#include "ChannelMgr.h"
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

void ChannelMgr::Start(int nServiceID, short port,
	FnOnPassiveConnect fnOnPassiveConnect,
	FnOnReceiveMsg fnOnReceiveMsg)
{
	m_nServiceID = nServiceID;

	working_ = true;
	acceptor_ = new tcp::acceptor(m_ioContext, tcp::endpoint(tcp::v4(), port));
	m_fnOnReceiveMsg = fnOnReceiveMsg;
	m_fnOnPassiveConnect = fnOnPassiveConnect;

	do_accept();
	
	if (!io_thread_.native_handle()) {
		StartIoThread();
	}
}

void ChannelMgr::StartIoThread()
{
	io_thread_ = std::thread([this]()
	{
		// 循环才可以让服务不间断
		// 否则客户端断开连接就有可能是io_context.run();跑出异常
		while (working_) {
			//try
			//{
			m_ioContext.run();
			//}
			//catch (std::exception& e)
			//{
			//	std::cerr << "Exception: " << e.what() << "\n";
			//	TRACE("exception: %s\n", e.what());
			//}
		}
	});
}

void ChannelMgr::Stop()
{
	if( working_ )
	{
		working_ = false;
		m_ioContext.stop();
		io_thread_.join();
	}
	
}

void ChannelMgr::GetChannelList(ChannelList &Channel_list)
{
	std::lock_guard<std::mutex> lk(Channels_lock_);
	for(auto & s : Channels_) {
		Channel_list.push_back(s.second);
	}
}

bool ChannelMgr::SendToService(int nServiceID, const void *pData, int nDataLen)
{
	std::shared_ptr<Channel> s;
	{
		std::lock_guard<std::mutex> lk(Channels_lock_);
		auto it = m_services.find(nServiceID);
		if (it == m_services.end()) {
			return false;
		}

		s = it->second;
	}
	
	s->SendMsg(pData, nDataLen);
	return true;
}

bool ChannelMgr::SendToAllService(const void *pData, int nDataLen)
{
	{
		std::lock_guard<std::mutex> lk(Channels_lock_);
		for (auto & s : m_services)
		{
			s.second->SendMsg(pData, nDataLen);
		}
	}

	return true;
}

bool ChannelMgr::SendToConnect(int nConnectID, const void *pData, int nDataLen)
{
	std::shared_ptr<Channel> s;
	{
		std::lock_guard<std::mutex> lk(Channels_lock_);
		auto it = Channels_.find(nConnectID);
		if (it == Channels_.end()) {
			return false;
		}

		s = it->second;
	}

	s->SendMsg(pData, nDataLen);
	return true;
}

void ChannelMgr::do_accept()
{
	acceptor_->async_accept(
		[this](boost::system::error_code ec, tcp::socket socket)
	{
		if (!ec)
		{
			std::stringstream ip_port;
			std::string ip = socket.remote_endpoint().address().to_string();
			short port = socket.remote_endpoint().port();
			ip_port << socket.remote_endpoint().address().to_string() << ":" << socket.remote_endpoint().port();

			auto s = std::make_shared<Channel>(this, std::move(socket), &m_ioContext, Channel::passive);
			s->Start();
		}

		do_accept();
	});
}

bool ChannelMgr::Connect(const std::string & strHost, short sPort,
	FnOnPositiveConnect fnOnPositiveConnect,
	FnOnReceiveMsg fnOnReceiveMsg
)
{
	m_fnOnPositiveConnect = fnOnPositiveConnect;
	m_fnOnReceiveMsg = fnOnReceiveMsg;

	return InternalConnect(strHost, sPort);
}

bool ChannelMgr::InternalConnect(const std::string & strHost, short sPort)
{
	

	tcp::resolver resolver(m_ioContext);
	auto endpoints = resolver.resolve(strHost, std::to_string(sPort));
	auto pConnectSocket = new tcp::socket(m_ioContext);
	auto pEndPoint = new tcp::resolver::results_type();
	*pEndPoint = endpoints;
	auto pTimer = new time_t_timer(m_ioContext);

	DoConnect(pConnectSocket, pEndPoint, pTimer);

	if (!io_thread_.native_handle()) {
		StartIoThread();
	}

	return true;
}

void ChannelMgr::DoConnect(tcp::socket *pConnectSocket, tcp::resolver::results_type *pEndPoint, time_t_timer *pTimer)
{
	boost::asio::async_connect(*pConnectSocket, *pEndPoint,
		[this, pConnectSocket, pEndPoint, pTimer](boost::system::error_code ec, tcp::endpoint) {
		if (ec) {
			// 再次重连
			// 当然这里可以对错误进行判断，是可以重连再重连
			pTimer->expires_from_now(5);
			pTimer->async_wait([this, pConnectSocket, pEndPoint, pTimer](const boost::system::error_code& ec) {
				DoConnect(pConnectSocket, pEndPoint, pTimer);
			});
		}
		else {
			// 检查一下是否已经有到此的连接
			//{
			//	auto strHost = pEndPoint->begin()->endpoint().address().to_string();
			//	auto sPort = pEndPoint->begin()->endpoint().port();
			//	std::lock_guard<std::mutex> lk(Channels_lock_);
			//	for (auto & c : Channels_) {
			//		if (c.second->IsMe(strHost, sPort)) {
			//			if (m_fnOnPositiveConnect) {
			//				m_fnOnPositiveConnect(c.second->GetServiceID(), c.second->GetConnectID());
			//			}
			//			return true;
			//		}
			//	}
			//}

			auto s = std::make_shared<Channel>(this, std::move(*pConnectSocket), &m_ioContext, Channel::positive);
			s->DoReqShakeHand();
			delete pConnectSocket;
			delete pEndPoint;
			delete pTimer;
		}
	});
}

void ChannelMgr::AddService(std::shared_ptr<Channel> s) {
	std::lock_guard<std::mutex> lk(Channels_lock_);
	Channels_.insert({ s->GetConnectID(), s });
	// 小于0的认为是纯客户端
	if (s->GetServiceID() >= 0) {
		m_services.insert({ s->GetServiceID(), s });
	}
}

// 移除会话
void ChannelMgr::DeleteService(std::shared_ptr<Channel> s) {
	{
		std::lock_guard<std::mutex> lk(Channels_lock_);
		Channels_.erase(s->GetConnectID());
		if (s->GetServiceID() >= 0) {
			if (m_services.find(s->GetServiceID()) != m_services.end())
			{
				m_services.erase(s->GetServiceID());
			}
		}
	}
}