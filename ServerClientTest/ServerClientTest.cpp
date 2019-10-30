// ServerClientTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include "../ggmsg/ggmsg.h"

std::vector<int> clientConnectIDList;

int OnPositiveConnect(int nServiceID, int nConnectID)
{
	std::cout << "OnPositiveConnect " << nServiceID << " " << nConnectID << "\n";
	return 0;
}

// 被动连接通知
// nSeviceID 发起连接的服务ID
int OnPassiveConnect(int nSeviceID, int nConnectID)
{
	clientConnectIDList.push_back(nConnectID);

	std::cout << "OnPassiveConnect " << nSeviceID << " " << nConnectID << "\n";
	return 0;
}

int OnReceiveMsg(int nServiceID, int nConnectID, const void *pMsg, int nMsgLen)
{
	std::cout << "from Service:" << nServiceID << " ConnectID:" << nConnectID << ", msg content:" << (char*)pMsg << "\n";
	return 0;
}

int main(int argc, char *argv[])
{	
	auto c = ggmsg_Create();
	if (!strcmp(argv[1], ("server"))) {
		std::cout << "Server Start!\n";

		ggmsg_Start(c, 9, 9009, OnPassiveConnect, OnReceiveMsg);
		char msg[] = "hello client";
		int nMsgLen = strlen(msg) + 1;
		for (int i = 0; i < 100; ++i) {
			std::this_thread::sleep_for(std::chrono::seconds(2));
			for (int cid : clientConnectIDList) {
				ggmsg_SendToConnect(c, cid, msg, nMsgLen);
			}
		}
	}
	else {
		std::cout << "Client Start!\n";

		ggmsg_Connect(c, "127.0.0.1", 9009, OnPositiveConnect, OnReceiveMsg);
		char msg[] = "hello server";
		int nMsgLen = strlen(msg) + 1;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		for (int i = 0; i < 100; ++i) {
			std::this_thread::sleep_for(std::chrono::seconds(2));
			ggmsg_SendToService(c, 9, msg, nMsgLen);
		}
	}

	getchar();
	ggmsg_Stop(c);
	ggmsg_Destory(c);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
