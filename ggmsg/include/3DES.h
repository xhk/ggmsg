//3DES.h : 头文件
//

#define ENCRYPT 0
#define DECRYPT 1

//#ifdef _WINDOWS
//#ifndef _WINDOWS_
//#include "windows.h"
//#endif
//#endif

typedef bool (*PSubKey)[16][48];

//3DES Class
class C3DES
{
public:
	bool DoDES(char *Out, char *In, long datalen, const char *Key, int keylen, bool Type);

private:
	const static char Table_IP[64];			// 初始置换
	const static char Table_InverseIP[64];	// 未置换
	const static char Table_E[48];			// 扩展置换
	const static char Table_PC1[56];		// 密钥初始置换 
	const static char Table_Moveleft[16];	// 左移运算
	const static char Table_PC2[48];		// 密钥压缩置换 
	const static char Box_S[8][4][16];		// S盒置换
	const static char Table_P[32];			// P盒置换

	bool SubKey[2][16][48];		//两个密钥的16轮子密钥 
	char Tmp[256];
	char deskey[16];

	// 通用置换函数
	void Transform(bool *Out, bool *In, const char *Table, int len);
	// 异或运算 
	void Xor(bool *InA, const bool *InB, int len);
	// 循环左移 
	void MoveLeft(bool *In, int len, int loop);
	// 字节转换成位 
	void Byte2Bit(bool *Out, const char *In, int bits);
	// 位转换字节 
	void Bit2Byte(char *Out, const bool *In, int bits);
	// S盒置换
	void funS(bool Out[32], const bool In[48]);
	// F函数 
	void funF(bool In[32], const bool Ki[48]);
	// 生成子密钥 
	void MakeSubKey(PSubKey pSubKey, const char Key[8]);
	// 生成密钥 
	void MakeKey(const char* Key, int len);
	// 一重DES加/解密 
	void DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type);
};
