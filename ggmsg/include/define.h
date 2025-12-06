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
	ggmtHeartBeat, // ������Ϣ

	ggmtMsg = 101 // ����ͨ����
};

enum ggmsgEncryptMethod{
	ggemNone,
	ggem3des,
};

struct ShakeHandReq
{
	int nServiceID; // ���󷽵�ServiceID
	char chEncryptKey[32];
	char chDesc[1024];
};

struct ShakeHandRsp
{
	int nServiceID; // ��Ӧ����ServiceID
	int nResult;
	char chEncryptKey[32];
	char chInfo[1024];
};

// ������Ϣ�޻�Ӧ
// δ���ɿ��Ǵ�һЩ���õ���Ϣ������ͳ����Ϣ
struct HeartBeatReq
{
	int nServiceID; // ���󷽵�ServiceID
	char chInfo[1024];
};

#pragma pack(pop)