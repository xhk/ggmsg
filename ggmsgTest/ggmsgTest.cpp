// ggmsgTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "../ggmsg/ChannelMgr.h"
#include <tchar.h>
#include <thread>

int OnPositiveConnect(int nServiceID, int nConnectID)
{
	std::cout << "OnPositiveConnect " << nServiceID << " " << nConnectID <<"\n";
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
	std::cout << "OnPassiveConnect " << nSeviceID << " " << nConnectID << "\n";
	return 0;
}

int OnPassiveDisConnect(int nSeviceID, int nConnectID)
{
	std::cout << "OnPassiveDisConnect " << nSeviceID << " " << nConnectID << "\n";
	return 0;
}

int OnReceiveMsg(int nServiceID, int nConnectID, const void *pMsg, int nMsgLen)
{
	std::cout << "from Service:" << nServiceID << " ConnectID:" << nConnectID << ", msg content:" << (char*)pMsg << "\n";
	return 0;
}

int main(int argc, TCHAR *argv[])
{
	int nServiceID = _ttoi(argv[1]);
	short sPort = _ttoi(argv[2]);
    std::cout << "Hello World!\n"; 
	ChannelMgr node;
	node.Start(nServiceID, sPort, OnPassiveConnect, OnPassiveDisConnect, OnReceiveMsg);

	if (argc > 3) {
		std::string host = argv[3];
		short port = _ttoi(argv[4]);
		
		node.Connect(host, port, OnPositiveConnect, OnPositiveDisConnect, OnReceiveMsg);
		char buf[32] = { 0 };
		getchar();
		for (int i=0;i<1000;++i)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			sprintf_s(buf, "msg %d", i + 1);
			node.SendToAllService(buf, strlen(buf) + 1);
		}
	}

	getchar();
	node.Stop();
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
