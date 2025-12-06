// 3DES.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "3DES.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ��ʼ�û� 
const char C3DES::Table_IP[64] = 
{
	58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};

// ĩ�û� 
const char C3DES::Table_InverseIP[64] = 
{
	40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25
};

// ��չ�û� 
const char C3DES::Table_E[48] = 
{
	32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
	 8,	 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
	16,	17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};

// ��Կ��ʼ�û� 
const char C3DES::Table_PC1[56] = 
{
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};

// �������� 
const char C3DES::Table_Moveleft[16] = 
{
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

// ��Կѹ���û� 
const char C3DES::Table_PC2[48] = 
{
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

// S�� 
const char C3DES::Box_S[8][4][16] = 
{
	// S1
	14, 4, 13, 1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
	// S2
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
	// S3
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
	// S4
	7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
	// S5
	2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
	// S6
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
	// S7
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
	// S8
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};

// P���û� 
const char C3DES::Table_P[32] = 
{
	16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
	2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25
};

// ͨ���û����� 
void C3DES::Transform(bool *Out, bool *In, const char *Table, int len)
{
	for(int i = 0; i < len; ++ i)
	Tmp[i] = In[ Table[i] - 1 ];
	memcpy(Out, Tmp, len);
}

// ������� 
void C3DES::Xor(bool *InA, const bool *InB, int len)
{
	for(int i = 0; i < len; ++ i)
	InA[i] ^= InB[i];
}

// ѭ������ 
void C3DES::MoveLeft(bool *In, int len, int loop)
{
	memcpy(Tmp, In, loop);
	memcpy(In, In + loop, len - loop);
	memcpy(In + len - loop, Tmp, loop);
}

// �ֽ�ת����λ 
void C3DES::Byte2Bit(bool *Out, const char *In, int bits)
{
	for(int i = 0; i < bits; ++ i)
	Out[i] = (In[i >> 3] >> (i & 7)) & 1;
}

// λת���ֽ� 
void C3DES::Bit2Byte(char *Out, const bool *In, int bits)
{
	memset(Out, 0, bits >> 3);
	for(int i = 0; i < bits; ++ i)
	Out[i >> 3] |= In[i] << (i & 7);
}

// S���û� 
void C3DES::funS(bool Out[32], const bool In[48])
{
	for(char i = 0, j, k; i < 8; ++ i, In += 6, Out += 4)
	{
		j = (In[0] << 1) + In[5];
		k = (In[1] << 3) + (In[2] << 2) + (In[3] << 1) + In[4];
		Byte2Bit(Out, &Box_S[i][j][k], 4);
	}
}

// F���� 
void C3DES::funF(bool In[32], const bool Ki[48])
{
	static bool MR[48];
	Transform(MR, In, Table_E, 48);
	Xor(MR, Ki, 48);
	funS(In, MR);
	Transform(In, In, Table_P, 32);
}

// ��������Կ 
void C3DES::MakeSubKey(PSubKey pSubKey, const char Key[8])
{
	static bool K[64], *KL = &K[0], *KR = &K[28];
	Byte2Bit(K, Key, 64);
	Transform(K, K, Table_PC1, 56);
	for(int i = 0; i < 16; ++ i)
	{
		MoveLeft(KL, 28, Table_Moveleft[i]);
		MoveLeft(KR, 28, Table_Moveleft[i]);
		Transform((*pSubKey)[i], K, Table_PC2, 48);
	}
}

// ������Կ 
void C3DES::MakeKey(const char* Key, int len)
{
	memset(deskey, 0, 16);
	memcpy(deskey, Key, len > 16 ? 16 : len);
	MakeSubKey(&SubKey[1], &deskey[8]);
}

// һ��DES��/���� 
void C3DES::DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type)
{
	static bool M[64], tmp[32], *Li = &M[0], *Ri = &M[32];
	Byte2Bit(M, In, 64);
	Transform(M, M, Table_IP, 64);
	if( Type == ENCRYPT )
	{
		// ����
		for(int i = 0; i < 16; ++ i)
		{
			memcpy(tmp, Ri, 32);
			funF(Ri, (*pSubKey)[i]);
			Xor(Ri, Li, 32);
			memcpy(Li, tmp, 32);
		}
	}
	else
	{
		// ����
		for(int i = 15; i >= 0; -- i)
		{
			memcpy(tmp, Li, 32);
			funF(Li, (*pSubKey)[i]);
			Xor(Li, Ri, 32);
			memcpy(Ri, tmp, 32);
		}
	}
	Transform(M, M, Table_InverseIP, 64);
	Bit2Byte(Out, M, 64);
}

// 3DES��/���� 
bool C3DES::DoDES(char *Out, char *In, long datalen, const char *Key, int keylen, bool Type)
{
	if(!( Out && In && Key && (datalen=(datalen+7)&0xfffffff8)))
		return false;
	MakeKey(Key, keylen);

	// 3��DES ����:��(key0)-��(key1)-��(key0) ����:��(key0)-��(key1)-��(key0) 
	for(long i = 0, j = datalen >> 3; i < j; ++ i, Out += 8, In += 8)
	{
		DES(Out, In,  &SubKey[0], Type);
		DES(Out, Out, &SubKey[1], !Type);
		DES(Out, Out, &SubKey[0], Type);
	}
	return true;
}