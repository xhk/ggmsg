#include "stdafx.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <random>
#include <boost/asio.hpp>

#include "3DES.h"
//#include "Diagnosis.h"


std::atomic_uint Channel::m_nConnectIDSerial;

Channel::Channel(ChannelMgr *pChannelMgr, tcp::socket socket, boost::asio::io_context *pIoContext, int nChannalType)
	: socket_(std::move(socket))
	, m_timerHeartBeat(*pIoContext)
{
	m_pIoContext = pIoContext;
	m_pChannelMgr = pChannelMgr;
	m_nLastActiveTime = std::time(0);
	m_tCreateTime = std::time(0);
	m_nSendTimes = 0;
	m_nRecvTimes = 0;
	m_nSendBytes = 0;
	m_nRecvBytes = 0;

	m_bSending = false;

	m_channalType = (ChannalType)nChannalType;
	m_nConnectID = ++m_nConnectIDSerial;
	m_nRecvBufLen = 1024;
	m_pRecvBuf = new char[m_nRecvBufLen];
	
	m_strRemoteIp = remote_ip();
	m_uRemotePort = socket_.remote_endpoint().port();
}

Channel::~Channel()
{
	delete[]m_pRecvBuf;

}


void Channel::do_close()
{
	m_pChannelMgr->DeleteService(shared_from_this());
	try {
		// if the close is also disconnect, shutdown will be throw exception in linux.
		// so catch the exception here.
		socket_.shutdown(tcp::socket::shutdown_both);
		socket_.close();
	}
	catch (boost::system::system_error& err) {
		// do nothing
	}
	

	if (m_channalType == ChannalType::positive) {
		m_pChannelMgr->OnPositiveDisConnect(m_nServiceID, m_nConnectID);

		if (m_pChannelMgr->working_) {
			m_pChannelMgr->InternalConnect(m_strRemoteIp, m_uRemotePort);
		}
	}
	else {
		m_pChannelMgr->OnPassiveDisConnect(m_nServiceID, m_nConnectID);
	}
}

void Channel::Start()
{
	m_uRemotePort = socket_.remote_endpoint().port();
	m_strRemoteIp = socket_.remote_endpoint().address().to_string();
	DoReadHead();

	
}

void Channel::PumpHeartBeat() {
	auto self(shared_from_this());
	m_timerHeartBeat.expires_after(boost::asio::chrono::seconds(30));
	m_timerHeartBeat.async_wait([self, this](const boost::system::error_code& ec) {
		if (ec) {
			return;
		}

		std::time_t tNow = std::time(nullptr);
		if (tNow - m_nLastActiveTime > 30) {
			HeartBeat();
		}

		PumpHeartBeat();
	});
}

void Channel::HeartBeat()
{
	const int nPackageLen = sizeof(NetHead) + sizeof(HeartBeatReq);
	char *pData = new char[nPackageLen];

	NetHead *pHead = (NetHead*)pData;
	HeartBeatReq *pBody = (HeartBeatReq*)(pData + sizeof(NetHead));

	pHead->nMagic = 0;
	pHead->nHeadSize = sizeof(NetHead);
	pHead->nBodySize = sizeof(HeartBeatReq);
	pHead->nBeforeCompressLen = 0;
	pHead->nBeforeEncryptLen = sizeof(HeartBeatReq);
	pHead->nCompressMethod = 0;
	pHead->nEncryptMethod = ggemNone;
	pHead->nMsgType = ggmtHeartBeat;
	pHead->nSerialNo = 0;

	pBody->nServiceID = m_nServiceID;

	SendMsg(pData, nPackageLen);
	delete[]pData;
}

void Channel::DoReadHead()
{
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(m_pRecvBuf, m_nRecvBufLen),
		[this, self](boost::system::error_code ec, std::size_t length) {
		if (!ec) {
			m_nLastActiveTime = std::time(0);
			NetHead *pHead = (NetHead *)m_pRecvBuf;
			if (pHead->nHeadSize + pHead->nBodySize > m_nRecvBufLen) {
				// 	ʱ�佫չ��buf
				char *pNewBuf = new char[pHead->nHeadSize + pHead->nBodySize];
				memcpy(pNewBuf, m_pRecvBuf, m_nRecvBufLen);
				delete[]m_pRecvBuf;
				m_pRecvBuf = pNewBuf;
				m_nRecvBufLen = pHead->nHeadSize + pHead->nBodySize;
			}

			if (pHead->nHeadSize + pHead->nBodySize <= length) {
				//  	һ�δ���
				OnReceivePacket(m_pRecvBuf, pHead->nHeadSize + pHead->nBodySize);
				DoReadHead();
			}
			else {
				//  	��������
				DoReadBody(*pHead);
			}
		}
		else {
			// 	�ӵ�Ͽ
			do_close();
		}
	});
}

void Channel::DoReadBody(const NetHead & head)
{
	auto self(shared_from_this());
	//  	δ���յ���ֽ�� = һ���ĵ�ͷ��Ŀ�� + ��Ŀ - һ���յ���ֽ��
	int nLeftBytes = head.nHeadSize + head.nBodySize - sizeof(NetHead);
	socket_.async_read_some(boost::asio::buffer(m_pRecvBuf + sizeof(NetHead), nLeftBytes),
		[this, self, head](boost::system::error_code ec, std::size_t length) {
		if (!ec) {
			m_nLastActiveTime = std::time(0);
			if (head.nHeadSize + head.nBodySize <= length + sizeof(NetHead)) {
				// 	һ�δ���
				OnReceivePacket(m_pRecvBuf, head.nHeadSize + head.nBodySize);
				DoReadHead();
			}
			else {
				//  	����δ�յ���, ���˳�����
				DoReadBody(head);
			}
		}
		else {
			// 	�ӵ�Ͽ
			do_close();
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
		break;
	}
	case ggmtHeartBeat: {
		break;
	}
	case ggmtMsg: {
		// 	��ӡ���ֽ���:
		char *pBody = (char*)pPacket + pHead->nHeadSize;
		m_pChannelMgr->OnReceiveMsg(m_nServiceID, m_nConnectID, pBody, pHead->nBodySize);
		break;
	}
	default: {
		break;
	}
	}
}

void Channel::DoReqShakeHand() {
	const int nPackageLen = sizeof(NetHead) + sizeof(ShakeHandReq);
	char *pData = new char[nPackageLen];

	NetHead *pHead = (NetHead*)pData;
	ShakeHandReq *pBody = (ShakeHandReq*)(pData + sizeof(NetHead));

	pHead->nMagic = 0;
	pHead->nHeadSize = sizeof(NetHead);
	pHead->nBodySize = sizeof(ShakeHandReq);
	pHead->nBeforeCompressLen = 0;
	pHead->nBeforeEncryptLen = sizeof(ShakeHandReq);
	pHead->nCompressMethod = 0;
	pHead->nEncryptMethod = ggemNone;
	pHead->nMsgType = ggmtShakeHand;
	pHead->nSerialNo = 0;

	pBody->nServiceID = m_pChannelMgr->GetServiceID();
	std::default_random_engine random(std::time(0));
	std::uniform_int_distribution<int> dis(0, 255);
	for (int i = 0; i < 32; i++) {
		pBody->chEncryptKey[i] = (char)dis(random);
	}
	
	memset(pBody->chDesc, 0, sizeof(pBody->chDesc));
	strcpy(pBody->chDesc, "connect");

	SendMsg(pData, nPackageLen);
	delete[]pData;
}

void Channel::OnRecvShakeHandReq(const void *pPacket, int nLength) {
	auto pHead = (NetHead*)pPacket;
	ShakeHandReq *pShakeHandReq = (ShakeHandReq*)((char*)pPacket + pHead->nHeadSize);

	m_nServiceID = pShakeHandReq->nServiceID;

	const int nPackageLen = sizeof(NetHead) + sizeof(ShakeHandRsp);
	char *pData = new char[nPackageLen];

	NetHead *pRspHead = (NetHead*)pData;
	ShakeHandRsp *pBody = (ShakeHandRsp*)(pData + sizeof(NetHead));

	pRspHead->nMagic = 0;
	pRspHead->nHeadSize = sizeof(NetHead);
	pRspHead->nBodySize = sizeof(ShakeHandRsp);
	pRspHead->nBeforeCompressLen = 0;
	pRspHead->nBeforeEncryptLen = sizeof(ShakeHandRsp);
	pRspHead->nCompressMethod = 0;
	pRspHead->nEncryptMethod = ggemNone;
	pRspHead->nMsgType = ggmtShakeHand;
	pRspHead->nSerialNo = 0;

	pBody->nServiceID = m_pChannelMgr->GetServiceID();
	pBody->nResult = 0;
	std::default_random_engine random(std::time(0));
	std::uniform_int_distribution<int> dis(0, 255);
	for (int i = 0; i < 32; i++) {
		pBody->chEncryptKey[i] = (char)dis(random);
	}
	memset(pBody->chInfo, 0, sizeof(pBody->chInfo));
	strcpy(pBody->chInfo, "connect ok");

	SendMsg(pData, nPackageLen);
	delete[]pData;

	// 	���ͻ���Ӧ
	m_pChannelMgr->OnPassiveConnect(m_nServiceID, m_nConnectID);
}

void Channel::OnRecvShakeHandRsp(const void *pPacket, int nLength) {
	auto pHead = (NetHead*)pPacket;
	ShakeHandRsp *pShakeHandRsp = (ShakeHandRsp*)((char*)pPacket + pHead->nHeadSize);

	m_nServiceID = pShakeHandRsp->nServiceID;

	// 	���ͻ���Ӧ
	m_pChannelMgr->OnPositiveConnect(m_nServiceID, m_nConnectID);
}

void Channel::write(char *data, std::size_t length) {
	auto self(shared_from_this());
	char *pData = new char[length];
	memcpy(pData, data, length);

	boost::asio::post(*m_pIoContext, [self, pData, length]() {
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
		[this, self, pData](boost::system::error_code ec, std::size_t length) {
		if (!ec) {
			m_nSendBytes += length;
			m_nSendTimes++;

			m_bSending = false;
			if (!m_dataQueue.empty()) {
				do_write();
			}
		}
		else {
			do_close();
		}
		delete[]pData;
	});
}

void Channel::SendMsg(const void *pData, size_t nDataLen) {
	if (!pData || nDataLen == 0) {
		return;
	}

	// 	���Ƿ��Ѿ���Ͽ
	if (!socket_.is_open()) {
		return;
	}

	auto self(shared_from_this());

	// 	���ͷ+��
	const int nPackageLen = sizeof(NetHead) + nDataLen;
	char *pSendData = new char[nPackageLen];

	NetHead *pHead = (NetHead*)pSendData;
	memcpy(pSendData + sizeof(NetHead), pData, nDataLen);

	pHead->nMagic = 0;
	pHead->nHeadSize = sizeof(NetHead);
	pHead->nBodySize = nDataLen;
	pHead->nBeforeCompressLen = 0;
	pHead->nBeforeEncryptLen = nDataLen;
	pHead->nCompressMethod = 0;
	pHead->nEncryptMethod = ggemNone;
	pHead->nMsgType = ggmtMsg;

	boost::asio::post(*m_pIoContext, [self, pSendData, nPackageLen]() {
		DataEle de = { pSendData,nPackageLen };
		self->m_dataQueue.push(de);
		self->do_write();
	});
}