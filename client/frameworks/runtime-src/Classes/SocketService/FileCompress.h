#pragma once
#include "HuffmanTree.h"


const unsigned char g_cbRecvByteMap[312] =
{
	0xFA, 0x2E, 0x56, 0xD0, 0x42, 2, 0x3D, 0xC4, 0x4D,
	1, 0xAE, 0xBB, 0xD3, 0xC7, 0xF6, 0x44, 0xCE, 0x92,
	0x19, 0x84, 0xE8, 0x45, 0xE4, 0xBC, 0xEC, 0x64, 0xCF,
	0x50, 0x77, 0x3F, 0x41, 0x68, 0xC9, 0xE3, 0xAA, 0xB1,
	0xB3, 0x6E, 0x4F, 0x1E, 0x66, 0x52, 0x7F, 0x70, 0x5A,
	0x30, 0xB9, 0x13, 0x16, 0x12, 0xB8, 0x6F, 0x4E, 0x93,
	0x29, 0xDE, 0x54, 0xC6, 0xBF, 0xBD, 0x11, 0x21, 0x9D,
	0x1C, 0x39, 0x46, 0xD8, 7, 0xFD, 0x43, 0x7C, 0x3B,
	0x89, 0x9B, 0x47, 0xDA, 0x5D, 0xEE, 0x61, 0xE9, 0xD1,
	0x74, 0x27, 0xBA, 0xDF, 0xD2, 0x2F, 0x97, 0xEA, 0xCB,
	0xAB, 0x40, 3, 0x14, 0xE5, 0xE1, 0x33, 0xEB, 0x62,
	0x83, 0x90, 0x63, 0x8C, 0x65, 0xAF, 0xB, 0x6C, 0xA3,
	0xF5, 0xF9, 0xE0, 0xE7, 0xD4, 0x7A, 0x3E, 0x60, 0x91,
	0x37, 0xAC, 0x2A, 0xC8, 0xF8, 0x34, 0x76, 0, 0xA8,
	0xC3, 0xB4, 0x71, 0x8E, 0x87, 0x78, 0xE6, 0xC0, 0x10,
	0x58, 0xD9, 0x17, 0x4A, 0x7B, 0x81, 0x38, 0x35, 0xF4,
	0xD, 0x5F, 4, 0x4B, 0x9C, 0x72, 0xA1, 0x1D, 0x6B, 0x7D,
	0x98, 0xE2, 0xA9, 0xCD, 0xA, 0xF2, 0x2C, 0x32, 0x73,
	0xD5, 0xFE, 0xDC, 0x79, 0xF, 0xDD, 0x18, 0x36, 0x1B,
	0xCC, 0xA7, 0xB2, 0x69, 0x9A, 0x28, 0x55, 0x5B, 0x8D,
	0xC5, 0x3A, 0x82, 0xF1, 0xED, 0x96, 0xB0, 0x9E, 5,
	0xF7, 0xE, 0x25, 0x86, 0x31, 0xC1, 0x67, 0x3C, 0xA4,
	0x2D, 0xB7, 0x88, 0xCA, 0x1A, 9, 0xB6, 0x49, 0xF3,
	0x59, 0x94, 0x95, 0x51, 0x53, 0x20, 0x15, 0x24, 0x5E,
	0x4C, 0x6A, 8, 0x80, 0x2B, 0xA0, 0x99, 0xFF, 0xA6,
	0xBE, 0x7E, 0xF0, 0xC2, 0xD6, 0x5C, 0xFC, 0xFB, 0xA5,
	0x8B, 0xD7, 0x9F, 0x26, 0x6D, 0xEF, 0x85, 0x75, 0x8A,
	0x57, 0xC, 0xAD, 0x1F, 0xB5, 0x8F, 0x48, 0xA2, 0x22,
	6, 0x23, 0xDB, 0xE8, 0x69, 0xAA, 0, 0x5D, 0x54, 0xAA,
	0, 0x5D, 0x54, 0xAA, 0, 0x5D, 0x54, 0xAA, 0, 0x5D,
	0x54, 0xAA, 0, 0x5D, 0x54, 0xAA, 0, 0x5D, 0x54, 0xAA,
	0, 0x5D, 0x54, 0xAA, 0, 0x5D, 0x54, 0xAA, 0, 0x5D,
	0x54, 0xAA, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0,
};
/*
const unsigned char m_cbRecvByteMap[256] =
{
0xDD,0x4E,0xC5,0x22,0x6B,0x35,0x2C,0xAA,0x34,0x82,0x56,0xE8,0x86,0x36,0xD6,0x69,
0xEB,0xBD,0x3C,0xF7,0x53,0x92,0x20,0x71,0xF6,0x75,0xF4,0xE4,0x2,0x6A,0xE0,0x50,
0x9F,0x99,0x55,0x6F,0x58,0x14,0x89,0x16,0x9A,0xA7,0xCE,0x31,0x32,0xB8,0xFB,0x9C,
0x5B,0x9E,0x9B,0xF3,0x73,0x66,0xB6,0xE7,0xA5,0xEC,0x70,0xC3,0x64,0xA9,0xA,0x9,
0x65,0xCB,0x60,0xB2,0x2B,0x48,0x8E,0x10,0x5C,0x3,0x6,0xD4,0x40,0xAD,0xB0,0xD8,
0x94,0xB1,0xF0,0xF8,0xDF,0xCF,0x2D,0x1C,0x42,0x76,0xBB,0x49,0xC2,0xFA,0x59,0x1D,
0xB4,0x8B,0xA3,0xB,0x43,0x98,0xC,0xFF,0x44,0xAB,0x72,0x5,0x7A,0xF1,0x8,0xA4,
0x25,0x6D,0x79,0x28,0x3D,0xC4,0x7,0x78,0xEE,0x88,0x7F,0x5A,0x74,0x13,0x5D,0xAF,
0x84,0xBA,0x33,0x51,0x7E,0x18,0x1F,0xFD,0x83,0xAC,0xBE,0xC0,0x4,0xD9,0xD3,0xD2,
0x39,0xEA,0x37,0x21,0x68,0x12,0x46,0x6C,0x81,0x96,0xE,0x3F,0x4A,0x1E,0x1A,0xF9,
0x1,0x6E,0xF5,0xDC,0xE9,0x27,0xD0,0x15,0xC6,0xDB,0x3B,0x5F,0x8D,0xBC,0x2F,0xC8,
0xD5,0xC1,0x11,0x8C,0x87,0x30,0x9D,0x0,0x57,0xDE,0xA8,0x91,0x77,0x7B,0xB7,0xC9,
0x29,0x2E,0xB5,0x8A,0xED,0x90,0xCA,0x47,0x2A,0x38,0xAE,0xD7,0x4C,0xEF,0x19,0x80,
0xFE,0xD,0x62,0x3E,0xE2,0xE6,0xF,0x24,0x67,0x54,0x23,0x61,0xA6,0xCC,0x4D,0xE3,
0x3A,0xB3,0x7C,0x17,0x97,0xB9,0xD1,0x52,0xBF,0xA0,0xC7,0x8F,0xE5,0x5E,0x41,0x4F,
0x85,0x1B,0xDA,0xE1,0x4B,0xA1,0x45,0xFC,0x95,0xCD,0xF2,0x7D,0x63,0xA2,0x93,0x26
};
*/

struct CharInfor
{
	CharInfor(int count = 0)
		:_count(count)
	{}

	CharInfor operator+(const CharInfor& info)
	{
		return CharInfor(_count + info._count);
	}

	bool operator> (const CharInfor& info)
	{
		return  _count > info._count;
	}

	bool operator< (const CharInfor& info)
	{
		return   _count < info._count;
	}

	bool operator !=(const CharInfor& info)
	{
		return  _count != info._count;
	}

	int				_count;			//字符出现的次数
	unsigned char	_ch;			//出现的字符
};


struct tagUnCompress
{
	int _size;
	CharInfor _info[256];
};
class FileCompress
{
public:
	FileCompress()
	{
		Reset();
	}
public:
	unsigned char* UncompressFile(unsigned char* pData, int nDataSize, int &nRetCount)
	{
		unsigned char * pBuffer = nullptr;

		if (readInfor(pData, nDataSize))
		{
			nRetCount = 0;
			pBuffer = nullptr;
		}
		else
		{
			int nBufferSize = _size + 1;
			nRetCount = _size;
			//创建文件缓存区数据
			pBuffer = new unsigned char[nBufferSize];
			//初始化缓存区
			memset(pBuffer, 0, nBufferSize);

			//创建哈夫曼数
			CharInfor invalid;
			HuffmanTree<CharInfor> huffmanTree(_info, 256, invalid);


			//根节点的权值为字符出现的总和
			int nCharCount = huffmanTree.GetRoot()->_weight._count;


			HuffmanTreeNode<CharInfor>* root = huffmanTree.GetRoot();


			int nCharIndex = 0;
			int nCurrentUnCount = 0;
			int nPos = 0;
			if (_size)
			{
				HuffmanTreeNode<CharInfor>* cur = root;

				while (nCharCount && nCurrentUnCount <= _size)
				{
					unsigned char cbOneByte = pData[nCharIndex];
					nPos = 7;
					do
					{
						//如果不是叶子结点就找叶子节点，0向左走，1向右走
						if ((1 << nPos) & cbOneByte)
							cur = cur->_right;
						else
							cur = cur->_left;

						//到叶子节点说明找到字符，放入解压文件
						if (cur->_left == NULL && cur->_right == NULL)
						{
							pBuffer[nCurrentUnCount++] = cur->_ch;
							nCharCount--;
							cur = root;
						}

						if (nCharCount < 1)
							break;
						nPos--;
					} while (nPos);

					nCharIndex++;
				};
			}
		}

		return pBuffer;
	}
protected:
	bool readInfor(unsigned char* pData, int nDataSize)
	{
		int result = -1;

		if (pData)
		{
			if (nDataSize >= 8)
			{
				//当前文件内容大小
				int nFileSize = nDataSize - 4;
				//4个字节为组合数
				int nConfigFileSize = *(int*)&pData[nDataSize - 4];
				//配置文件内容大小
				int nConfigDataSize = 8 * nConfigFileSize | 4;
				//验证配置文件内容
				if (nConfigDataSize < nDataSize)
				{
					//创建配置文件缓存区
					char * pBlockData = new  char[nConfigDataSize];

					//去掉末尾的大小 其中加的4个字节为文件大小
					memcpy(pBlockData, &pData[nFileSize - nConfigDataSize], nConfigDataSize);

					//解密配置数据
					if (nConfigDataSize >= 1)
					{
						int nIndex = 0;
						char* pTmpData = pBlockData;
						do
						{
							//对单个字节进行解密
							char cbOneByte = g_cbRecvByteMap[(unsigned char)(*pTmpData + nIndex)] + nIndex;
							//索引递减
							--nIndex;
							//把解密完成的字符赋值保存
							*pTmpData++ = cbOneByte;

						} while (nIndex != -4 - 8 * nConfigFileSize);

					}
					tagUnCompress * pObj = (tagUnCompress*)pBlockData;
					//获取文件大小
					_size = pObj->_size;

					if (_size < 1)
					{
						result = -1;
					}
					else
					{
						//初始化压缩配置信息
						if (nConfigFileSize >= 1)
						{

							CharInfor * charInfor = pObj->_info;
							while (nConfigFileSize)
							{
								_info[charInfor->_ch] = *charInfor;
								--nConfigFileSize;
								charInfor++;
							}

							result = 0;
						}
					}
				}
			}
		}

		return result;
	}

	void Reset()
	{
		for (size_t i = 0; i<256; i++)
		{
			_info[i]._ch = i;
			_info[i]._count = 0;
		}

		_size = 0;
	}

protected:
	//字符统计信息
	CharInfor	_info[256];
	//当前解压缩文件原始大小
	int			_size;
};
