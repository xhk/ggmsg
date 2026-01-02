// ServerClientTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <random>

#include "../ggmsg/include/ggmsg.h"
#include "../ggmsg/include/ChannelNode.h"

#include "3DES.h"

class MyListener : public ChannelListener
{
public:
	virtual void OnPositiveConnect(int nServiceID, int nConnectID) override
	{
		std::cout << "OnPositiveConnect " << nServiceID << " " << nConnectID << "\n";
	}

	virtual void OnPositiveDisConnect(int nServiceID, int nConnectID) override
	{
		std::cout << "OnPositiveDisConnect " << nServiceID << " " << nConnectID << "\n";
	}

	virtual void OnPassiveConnect(int nSeviceID, int nConnectID) {
		std::cout << "OnPassiveConnect " << nSeviceID << " " << nConnectID << "\n";
		
	};
	virtual void OnPassiveDisConnect(int nServiceID, int nConnectID) {
		std::cout << "OnPassiveDisConnect " << nServiceID << " " << nConnectID << "\n";
		
	};

	virtual void OnReceiveMsg(int nServiceID, int nConnectID, const char *pMsg, int nMsgLen) override
	{
		std::cout << "from Service:" << nServiceID << " ConnectID:" << nConnectID << ", msg content:" << (char*)pMsg << "\n";
	}
};

MyListener listener;

void TestClient() {
	auto node = ChannelNode::Create(&listener);
	node->Connect( "127.0.0.1", 10021);
	/*for(int i=0;i<100;++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));
		node->SendToService(9, "hello server", 12);
	}*/
}

int main(int argc, char *argv[])
{	
	TestClient();
		
	getchar();

	return 0;
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
