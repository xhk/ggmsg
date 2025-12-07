#pragma once
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <ctime>
#include <atomic>
#include "Timer.h"
#include "define.h"


using boost::asio::ip::tcp;



class ChannelMgr;
class  Channel
	: public std::enable_shared_from_this<Channel>
{
public:
	Channel(ChannelMgr *pChannelMgr, tcp::socket socket, boost::asio::io_context *pIoContext, int nChannalType);
	
	~Channel();

	int GetServiceID() { return m_nServiceID; }
	int GetConnectID() { return m_nConnectID; }

	void Start();

	bool IsMe(const std::string & strHost, short sPort) {
		return m_strRemoteIp == strHost && m_uRemotePort == sPort;
	}
	

	// ��������������
	void DoReqShakeHand();


	void SendMsg(const void *pData, size_t nDataLen);

	void write(char *data, std::size_t length);

	std::string RemoteIp() {
		return m_strRemoteIp;
	}
	std::string remote_ip() { return socket_.remote_endpoint().address().to_string(); }
	unsigned short remote_port() { return m_uRemotePort; }
	unsigned int SendTimes() { return m_nSendTimes; }
	unsigned int RecvTimes() { return m_nRecvTimes; }
	unsigned int SendBytes() { return m_nSendBytes; }
	unsigned int RecvBytes() { return m_nRecvBytes; }
	time_t CreateTime() { return m_tCreateTime; }

	// ͨ������
	enum ChannalType{
		positive,
		passive
	};

private:


	void DoReadHead();
	void DoReadBody(const NetHead & head);
	void do_write();

	void do_close();

	void PumpHeartBeat();
	void HeartBeat();


	void OnReceivePacket(const void *pPacket, int nLength);

	void OnRecvShakeHandReq(const void *pPacket, int nLength);
	void OnRecvShakeHandRsp(const void *pPacket, int nLength);

	boost::asio::io_context *m_pIoContext;
	ChannelMgr *m_pChannelMgr;
	tcp::socket socket_;

	char *m_pRecvBuf;
	int m_nRecvBufLen;

	time_t m_tCreateTime; // �ػ�����ʱ��

	unsigned int m_nSendTimes;
	unsigned int m_nRecvTimes;
	unsigned int m_nSendBytes;
	unsigned int m_nRecvBytes;
	unsigned int m_nLastActiveTime; // �ϴλʱ��

	struct DataEle {
		char *pData;
		std::size_t nLen;
	};

	std::string m_strRemoteIp;
	unsigned short m_uRemotePort;
	std::queue<DataEle> m_dataQueue;
	bool m_bSending;

	int m_nServiceID;
	int m_nConnectID;
	char m_chSelfEncryptKey[32];
	char m_chPeerEncryptKey[32];

	static std::atomic_uint m_nConnectIDSerial;

	ChannalType m_channalType;

	time_t_timer m_timerHeartBeat; // ��������ʱ��
	
};
