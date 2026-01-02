// ServerClientTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <random>
#include <unordered_set>

#include "../ggmsg/include/ggmsg.h"
#include "../ggmsg/include/ChannelNode.h"
#include "3DES.h"

std::mutex connectIDLock;
std::unordered_set<int> clientConnectIDList;


class MyListener : public ChannelListener {
	virtual void OnPositiveConnect(int nServiceID, int nConnectID) {
		std::cout << "OnPositiveConnect " << nServiceID << " " << nConnectID << "\n";
		
	};
	virtual void OnPositiveDisConnect(int nServiceID, int nConnectID) {
		std::cout << "OnPositiveDisConnect " << nServiceID << " " << nConnectID << "\n";
		
	};
	virtual void OnPassiveConnect(int nSeviceID, int nConnectID) {
		std::cout << "OnPassiveConnect " << nSeviceID << " " << nConnectID << "\n";
		std::lock_guard<std::mutex> lock(connectIDLock);
		clientConnectIDList.insert(nConnectID);
	};
	virtual void OnPassiveDisConnect(int nServiceID, int nConnectID) {
		std::cout << "OnPassiveDisConnect " << nServiceID << " " << nConnectID << "\n";
		std::lock_guard<std::mutex> lock(connectIDLock);
		clientConnectIDList.erase(nConnectID);
	};
	virtual void OnReceiveMsg(int nServiceID, int nConnectID, const char* pData, int nDataLen) {
		std::cout << "from Client:" << nConnectID << " msg len:" << nDataLen << " msg content:" << pData << "\n";
	};
};

MyListener listener;
void TestServer() {
	auto node = ChannelNode::Create(&listener);
	node->Start(9, 10021);
	char* msg = "hello ,this is server msg";
	auto msg_len = strlen(msg)+1;
	while (getchar()) {
		std::lock_guard<std::mutex> lock(connectIDLock);
		for (auto it = clientConnectIDList.begin(); it != clientConnectIDList.end(); ++it) {
			node->SendToConnect(*it,msg, msg_len);
		}
	}
}

int main(int argc, char *argv[])
{	
	TestServer();
	
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
