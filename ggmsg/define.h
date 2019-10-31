#pragma once
#pragma pack(push,1)

struct NetHead 
{
	int nMagic;
	int nHeadSize;
	int nBodySize;
	int nBeforeCompressLen;
	int nBeforeEncryptLen;
	short nCompressMethod;
	short nEncryptMethod;
	int nMsgType;
	int nSerialNo;
};

enum ggmsgType
{
	ggmtShakeHand = 1,
	ggmtHeartBeat, // 心跳消息

	ggmtMsg = 101 // 正常通信了
};

enum ggmsgEncryptMethod{
	ggemNone,
	ggem3dec,
};

struct ShakeHandReq
{
	int nServiceID; // 请求方的ServiceID
	char chEncryptKey[32];
	char chDesc[1024];
};

struct ShakeHandRsp
{
	int nServiceID; // 回应方的ServiceID
	int nResult;
	char chEncryptKey[32];
	char chInfo[1024];
};

// 心跳消息无回应
// 未来可考虑带一些有用的信息，比如统计信息
struct HeartBeatReq
{
	int nServiceID; // 请求方的ServiceID
	char chInfo[1024];
};

#pragma pack(pop)