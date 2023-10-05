#include "StdAfx.h"
#include "WHBase64.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//

//��������
const wchar_t CHAR_63 = '+';
const wchar_t CHAR_64 = '/';
const wchar_t CHAR_PAD = '=';

//�ַ���
const wchar_t alph[] = 
{ 
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9',CHAR_63,CHAR_64
};

//���嶨��
union unBuffer
{
	unsigned char bytes[ 4 ];
	unsigned int block;
}; 

//��ȡ����
inline int GetDataLength(int nCodeLength)
{
	return nCodeLength-nCodeLength/4;
}

//��ȡ����
inline int GetCodeLength(int nDataLength)
{
	int len=nDataLength+nDataLength/3 + (int)(nDataLength%3!=0);
	if (len % 4) len += 4-len%4;
	return len;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CWHBase64::CWHBase64()
{
}

//��������
CWHBase64::~CWHBase64()
{
}

//���뺯��
int CWHBase64::Encode(const CHAR * pData, int nDataLen, TCHAR szOutBuffer[], int nBufferLen)
{
	int len = GetCodeLength(nDataLen);
	wchar_t* out = new wchar_t[len];	

	unBuffer buffer;
	const int mask = 0x3F;	

	for (int i = 0, j = 0, left = nDataLen; i < nDataLen; i += 3, j += 4, left -= 3)
	{
		buffer.bytes[2]=pData[i];

		if (left > 1)
		{
			buffer.bytes[ 1 ] = pData[ i + 1 ];
			if( left > 2 )
				buffer.bytes[ 0 ] = pData[ i + 2 ];
			else
				buffer.bytes[ 0 ] = 0;
		}
		else
		{
			buffer.bytes[ 1 ] = 0;
			buffer.bytes[ 0 ] = 0;
		}

		out[ j ] = alph[ ( buffer.block >> 18 ) & mask ];
		out[ j + 1 ] = alph[ ( buffer.block >> 12 ) & mask ];
		if( left > 1 )
		{
			out[ j + 2 ] = alph[ ( buffer.block >> 6 ) & mask ];
			if ( left > 2 )
				out[ j + 3 ] = alph[ buffer.block & mask ];
			else
				out[ j + 3 ] = CHAR_PAD;
		}
		else
		{
			out[ j + 2 ] = CHAR_PAD;
			out[ j + 3 ] = CHAR_PAD;
		}
	}

	//����ת��
	CW2CT szOutResult(out);
	CopyMemory(szOutBuffer, szOutResult,len*sizeof(TCHAR));	

	//���ý�����
	szOutBuffer[len] = 0;	

	//�ͷ���Դ
	SafeDeleteArray(out);

	return len;
}

// ���뺯��
int CWHBase64::Decode(const TCHAR * pszCode, int nCodeLength, TCHAR szOutBuffer[], int nBufferLen)
{
	unBuffer buffer;
	buffer.block = 0;

	//����ת��
	CT2CW pinCode(pszCode);
	
	//�����ڴ�
	wchar_t * pOutData = new wchar_t[nBufferLen];

	int j = 0;
	for (int i = 0; i < nCodeLength; i++)
	{
		int val = 0;
		int m = i % 4;
		wchar_t x = pinCode[i];		

		if( x >= 'A' && x <= 'Z' )
			val = x - 'A';
		else if( x >= 'a' && x <= 'z' )
			val = x - 'a' + 'Z' - 'A' + 1;
		else if( x >= '0' && x <= '9' )
			val = x - '0' + ( 'Z' - 'A' + 1 ) * 2;
		else if( x == CHAR_63 )
			val = 62;
		else if( x == CHAR_64 )
			val = 63;

		if( x != CHAR_PAD )
			buffer.block |= val << ( 3 - m ) * 6;
		else
			m--;

		if (m == 3 || x == CHAR_PAD)
		{
			pOutData[ j++ ] = buffer.bytes[2];
			if ( x != CHAR_PAD || m > 1 )
			{
				pOutData[ j++ ] = buffer.bytes[ 1 ];
				if( x != CHAR_PAD || m > 2 ) pOutData[ j++ ] = buffer.bytes[ 0 ];
			}

			buffer.block = 0;
		}

		if (x == CHAR_PAD) break;
		if (j == nBufferLen) break;
	}

	//���ý�����
	pOutData[j]=0;

	//����ת��
	CW2CT pszBuffer(pOutData);	
	CopyMemory(szOutBuffer, pszBuffer, nBufferLen*sizeof(TCHAR));	

	//�ͷ���Դ
	SafeDeleteArray(pOutData);

	return j;
}

//////////////////////////////////////////////////////////////////////////////////////////////