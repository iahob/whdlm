// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// ClientControl.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

int RandInt(int nMin,int nMax)
{
	int nDelta = nMax - nMin;
	if (nDelta <=0)
	{
		return nMax;
	}

	int nRand1 = ::rand();
	int nRand2 = ::rand();
	return ((nRand1<<15) + nRand2)%(nDelta+1) + nMin;
}

