// ggmsgTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "../ggmsg/ChannelNode.h"
#include <tchar.h>
#include <thread>
#include <time.h>

class MyChannelListener : public ChannelListener {
public:
	virtual void OnPositiveConnect(int nServiceID, int nConnectID)
	{
		std::cout << "OnPositiveConnect " << nServiceID << " " << nConnectID << "\n";

	}

	virtual void OnPositiveDisConnect(int nServiceID, int nConnectID)
	{
		std::cout << "OnPositiveDisConnect " << nServiceID << " " << nConnectID << "\n";
	
	}



	// 被动连接通知
	// nSeviceID 发起连接的服务ID
	virtual void OnPassiveConnect(int nSeviceID, int nConnectID)
	{
		std::cout << "OnPassiveConnect " << nSeviceID << " " << nConnectID << "\n";
		
	}

	virtual void OnPassiveDisConnect(int nSeviceID, int nConnectID)
	{
		std::cout << "OnPassiveDisConnect " << nSeviceID << " " << nConnectID << "\n";
	
	}

	virtual void OnReceiveMsg(int nServiceID, int nConnectID, const void* pMsg, int nMsgLen)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "from Service:" << nServiceID << " ConnectID:" << nConnectID << ", msg content:" << (char*)pMsg << "\n";
		
	}

};

enum Mode {
	Positive,
	Passive
};

void test_basic(Mode mode, int serviceId, std::string host, short port) {
	int nServiceID = serviceId;
	short sPort = port;
	std::cout << "test_basic!\n";
	MyChannelListener listener;

	ChannelNode* node = ChannelNode::Create(&listener);
	if (mode == Mode::Passive) {
		node->Start(nServiceID, sPort);
		char buf[32] = { 0 };
		for (int i = 0; i < 1000; ++i)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			sprintf_s(buf, "push msg %d", i + 1);
			node->SendToConnect(1, buf, strlen(buf) + 1);
		}
	}else {
		node->Connect(host.c_str(), port);
		char buf[32] = { 0 };
		// wait for connect
		std::this_thread::sleep_for(std::chrono::seconds(5));
		for (int i = 0; i < 1000; ++i)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			sprintf_s(buf, "msg %d", i + 1);
			node->SendToAllService(buf, strlen(buf) + 1);
		}
	}

	getchar();
	node->Stop();
	ChannelNode::Destory(node);
}


void test_pub(Mode mode, int serviceId, std::string host, short port) {
	
	if (mode == Mode::Passive) {
		ChannelListener listener;

		ChannelNode* node = ChannelNode::Create(&listener);
		node->Start(serviceId, port);
		std::this_thread::sleep_for(std::chrono::seconds(5));
		const int buf_size = 1024;
		char *buf = new char[buf_size];
		memset(buf, 'A', buf_size);
		// wait for client connect
		std::this_thread::sleep_for(std::chrono::seconds(10));
		std::cout << "start send\n";
		clock_t start = clock();
		for (int i = 0; i < 100*10000; ++i)
		{
			node->SendToConnect(1, buf, buf_size);
		}
		clock_t end = clock();
		std::cout << "send finish!\n";
		double duration = (double)(end - start) / CLOCKS_PER_SEC;
		std::cout << "total time:" << duration << "s\n";
		delete[]buf;
	}
	else {
		ChannelListener listener;

		ChannelNode* node = ChannelNode::Create(&listener);
		node->Connect(host.c_str(), port);
		
	}
	getchar();
}


int main(int argc, TCHAR *argv[])
{
	std::string strTestCase = argv[1];
	std::string strMode = argv[2];
	auto serviceId = atoi(argv[3]);
	auto host = argv[4];
	auto port = (short)atoi(argv[5]);
	Mode mode = Mode::Positive;
	if (strMode =="passive") {
		mode = Mode::Passive;
	}

	if (strTestCase == "basic") {
		test_basic(mode, serviceId, host, port);
	}else if(strTestCase == "pub") {
		test_pub(mode, serviceId, host, port);
	}
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
