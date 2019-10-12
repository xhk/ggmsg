#include "stdafx.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

Channel::~Channel()
{
	
}


void Channel::do_close()
{
	

	m_pChannelMgr->earse(shared_from_this());
	socket_.shutdown(tcp::socket::shutdown_both);
	socket_.close();
}

void Channel::Start()
{
	m_uRemotePort = socket_.remote_endpoint().port();
	DoReadHead();

	
}

void Channel::PumpHeartBeat() {
	auto self(shared_from_this());
	m_timerHeartBeat.expires_from_now(30);
	m_timerHeartBeat.async_wait([self, this](const boost::system::error_code& ec) {
		if (ec) {
			return;
		}

		std::time_t tNow = std::time_t(0);
		if (tNow - m_nLastActiveTime > 30) {
			HeartBeat();
		}

		PumpHeartBeat();
	});
}

void Channel::HeartBeat()
{
	const int nPackageLen = sizeof(NetHead) + sizeof(HeartBeatReq);
	char data[nPackageLen] = { 0 };
	auto pHead = (NetHead*)data;
	pHead->nHeadSize = sizeof(NetHead);
	pHead->nBodySize = sizeof(ShakeHandReq);
	pHead->nMsgType = ggmtHeartBeat;

	auto pReq = (HeartBeatReq*)(data + pHead->nHeadSize);
	strcpy_s(pReq->chInfo, "alive");

	write(data, nPackageLen);
}

void Channel::DoReqShakeHand()
{
	const int nPackageLen = sizeof(NetHead) + sizeof(ShakeHandReq);
	char data[nPackageLen] = { 0 };
	auto pHead = (NetHead*)data;
	pHead->nHeadSize = sizeof(NetHead);
	pHead->nBodySize = sizeof(ShakeHandReq);
	pHead->nMsgType = ggmtShakeHand;
	
	auto pReq = (ShakeHandReq*)(data + pHead->nHeadSize);
	pReq->nServiceID = m_pChannelMgr->GetServiceID();

	write(data, nPackageLen);
	DoReadHead();
}

void Channel::OnRecvShakeHandRsp(const void *pPacket, int nLength)
{
	auto pHead = (NetHead*)pPacket;
	auto pRsp = (ShakeHandRsp*)((char*)pPacket + pHead->nHeadSize);
	if (pRsp->nResult == 0) {
		m_nServiceID = pRsp->nServiceID;
	}

	m_pChannelMgr->AddService(shared_from_this());
}

void Channel::OnRecvShakeHandReq(const void *pPacket, int nLength)
{
	auto pReqHead = (NetHead*)pPacket;
	auto pReq = (ShakeHandReq*)((char*)pPacket + pReqHead->nHeadSize);
	m_nServiceID = pReq->nServiceID;

	const int nPackageLen = sizeof(NetHead) + sizeof(ShakeHandRsp);
	char buf[nPackageLen] = { 0 };
	auto pRspHead = (NetHead*)(buf);
	pRspHead->nHeadSize = sizeof(NetHead);
	pRspHead->nBodySize = sizeof(ShakeHandRsp);
	pRspHead->nMsgType = pReqHead->nMsgType;
	auto pRsp = (ShakeHandRsp*)(buf + pRspHead->nHeadSize);
	pRsp->nServiceID = m_pChannelMgr->GetServiceID();

	write(buf, nPackageLen);
}

void Channel::DoReadHead() 
{
	auto self(shared_from_this());
	boost::asio::async_read(socket_, boost::asio::buffer(data_, sizeof(NetHead)), 
		[this, self](boost::system::error_code ec, std::size_t length)
	{
		if (ec) {
			do_close();
		}
		else {
			//std::cout << "read head completed \n";
			auto pNetHead = (NetHead*)data_;
			DoReadBody(*pNetHead);
		}
	});
}

void Channel::DoReadBody(const NetHead & head)
{
	auto self(shared_from_this());
	boost::asio::async_read(socket_, boost::asio::buffer(data_ + head.nHeadSize, head.nBodySize),
		[this, self](boost::system::error_code ec, std::size_t length)
	{
		if (ec) {
			do_close();
		}
		else {
			auto pNetHead = (NetHead*)data_;
			OnReceivePacket(data_, pNetHead->nHeadSize + pNetHead->nBodySize);
			DoReadHead();
		}
	});
}

void Channel::do_read()
{
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(data_, max_length),
		[this, self](boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			m_nRecvTimes++;
			m_nRecvBytes += length;
			/*try {
				OnRecvBinarayData(data_, length);
			}
			catch (...) {
				TRACE("packet exception\n");
				do_close();
				return;
			}*/

			do_read();
		}
		else
		{
			do_close();
		}
	});
}

void Channel::SendMsg(const void *pMsg, std::size_t nDataLen)
{
	auto self(shared_from_this());
	int nPackageLen = sizeof(NetHead) + nDataLen;
	char *pData = new char[nPackageLen];
	auto pHead = (NetHead*)pData;
	pHead->nHeadSize = sizeof(NetHead);
	pHead->nBodySize = nDataLen;
	pHead->nMsgType = ggmtMsg;

	memcpy(pData + pHead->nHeadSize, pMsg, nDataLen);

	m_pIoContext->post([self, pData, nPackageLen]() {
		DataEle de = { pData,nPackageLen };
		self->m_dataQueue.push(de);
		self->do_write();
	});
}

void Channel::write(char *data, std::size_t length) {
	auto self(shared_from_this());
	char *pData = new char[length];
	memcpy(pData, data, length);

	m_pIoContext->post([self, pData, length]() {
		DataEle de = { pData,length };
		self->m_dataQueue.push(de);
		self->do_write();
	});
}

void Channel::do_write() {
	if ( m_bSending || m_dataQueue.empty()) {
		return;
	}

	auto self(shared_from_this());

	m_bSending = true;
	auto d = m_dataQueue.front();
	m_dataQueue.pop();
	auto pData = d.pData;
	boost::asio::async_write(socket_, boost::asio::buffer(pData, d.nLen),
		[this, self, pData](boost::system::error_code ec, std::size_t length)
	{
		delete[]pData;
		m_bSending = false;
		if (ec)
		{
			//do_close();
			//std::string msg = ec.message();
			// 这里不用do_close()的，这里如果调用do_close，会触发do_read返回错误
			// 从而再次调用do_close引发异常
		}
		else {
			m_nSendBytes += length;
			m_nSendTimes++;

			do_write();
		}

	});
}
	

void Channel::OnReceivePacket(const void *pPacket, int nLength)
{
	//std::cout << "OnReceivePacket\n";
	auto pHead = (NetHead*)(pPacket);
	switch (pHead->nMsgType)
	{
	case ggmtShakeHand: {
		if (m_channalType == ChannalType::positive) {
			OnRecvShakeHandRsp(pPacket, nLength);
		}
		else if(m_channalType == ChannalType::passive)
		{
			OnRecvShakeHandReq(pPacket, nLength);
		}
	}
	break;
	case ggmtMsg: {
		if (m_pChannelMgr->m_handler) {
			m_pChannelMgr->m_handler(m_nServiceID, (char *)pPacket + pHead->nHeadSize, pHead->nBodySize);
		}
	}break;
	default:
		break;
	}
}
