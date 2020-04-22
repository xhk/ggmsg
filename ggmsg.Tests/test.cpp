#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include "../ggmsg/ggmsg.h"

std::mutex connectIDLock;
std::vector<int> clientConnectIDList;

int OnPositiveConnect(int nServiceID, int nConnectID)
{
	std::cout << "OnPositiveConnect " << nServiceID << " " << nConnectID << "\n";
	return 0;
}

int OnPositiveDisConnect(int nServiceID, int nConnectID)
{
	std::cout << "OnPositiveDisConnect " << nServiceID << " " << nConnectID << "\n";
	return 0;
}

// 被动连接通知
// nSeviceID 发起连接的服务ID
int OnPassiveConnect(int nSeviceID, int nConnectID)
{
	std::lock_guard<std::mutex> lk(connectIDLock);
	clientConnectIDList.push_back(nConnectID);

	std::cout << "OnPassiveConnect " << nSeviceID << " " << nConnectID << "\n";
	return 0;
}

int OnPassiveDisConnect(int nSeviceID, int nConnectID)
{
	std::lock_guard<std::mutex> lk(connectIDLock);
	auto it = clientConnectIDList.begin();
	for (; it != clientConnectIDList.end(); ++it) {
		clientConnectIDList.erase(it);
		break;
	}

	std::cout << "OnPassiveDisConnect " << nSeviceID << " " << nConnectID << "\n";
	return 0;
}

std::mutex outMutex;

int OnServerReceiveMsg(int nServiceID, int nConnectID, const void *pMsg, int nMsgLen)
{
	std::lock_guard<std::mutex> lk(outMutex);
	std::cout << "server from Service:" << nServiceID << " ConnectID:" << nConnectID << ", msg content:" << (char*)pMsg << "\n";
	return 0;
}

int OnReceiveMsg(int nServiceID, int nConnectID, const void *pMsg, int nMsgLen)
{
	std::lock_guard<std::mutex> lk(outMutex);
	std::cout << "from Service:" << nServiceID << " ConnectID:" << nConnectID << ", msg content:" << (char*)pMsg << "\n";
	return 0;
}

//TEST(TestCaseName, TestName) {
//	auto server = ggmsg_Create();
//	EXPECT_TRUE(server > 0);
//	ggmsg_Start(server, 9, 9009, OnPassiveConnect, OnPassiveDisConnect, OnServerReceiveMsg);
//	std::this_thread::sleep_for(std::chrono::seconds(2));
//
//	ggmsg_Stop(server);
//	ggmsg_Destory(server);
//
//	EXPECT_TRUE(true);
//}

TEST(ClientServer, OneLongConnectClient) {
	auto server = ggmsg_Create();
	auto client = ggmsg_Create();

	EXPECT_TRUE(server > 0);
	EXPECT_TRUE(client > 0);
	{
		ggmsg_Start(server, 9, 9009, OnPassiveConnect, OnPassiveDisConnect, OnServerReceiveMsg);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	{
		std::cout << "Client Start!\n";

		int nConnect = ggmsg_Connect(client, "127.0.0.1", 9009, OnPositiveConnect, OnPositiveDisConnect, OnReceiveMsg);
		EXPECT_EQ(nConnect, 0);
		char msg[] = "hello server";
		int nMsgLen = strlen(msg) + 1;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		for (int i = 0; i < 1000; ++i) {
			//std::this_thread::sleep_for(std::chrono::seconds(2));
			int nSend = ggmsg_SendToService(client, 9, msg, nMsgLen);
			EXPECT_EQ(nSend, 0);
		}
	}

	{
		char msg[] = "hello client";
		int nMsgLen = strlen(msg) + 1;
		/*for (int i = 0; i < 1000; ++i) {
			std::lock_guard<std::mutex> lk(connectIDLock);
			for (int cid : clientConnectIDList) {
				int nSend = ggmsg_SendToConnect(server, cid, msg, nMsgLen);
				EXPECT_EQ(nSend, 0);
			}
		}*/
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));
	ggmsg_Stop(server);
	ggmsg_Destory(server);
	ggmsg_Stop(client);
	ggmsg_Destory(client);
}