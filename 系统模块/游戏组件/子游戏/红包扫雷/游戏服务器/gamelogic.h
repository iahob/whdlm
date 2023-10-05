#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��λ����
typedef enum
{
	THOUSAND_POS = 17,		//ǧλ����
	HUNDRED_POS,			//��λ����
	TEN_POS,				//ʮλ����
	ONE_POS,				//��λ����
	DECIMALFIRST_POS,		//С������һλ
	DECIMALSECOND_POS,		//С�����ڶ�λ
	INVALID_POS,			//��Чλ
}NUMPOS;

//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();
	
	//�����ɷ����
	void RandDispatchMoney(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE dDispatchArray[]);
	//������ɺ��
	DOUBLE RandDispatchMoney(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE dMinMoney = 0.01);
	//��ȡ��λ������
	BYTE GetNumPos(DOUBLE dRobMoney, NUMPOS numpos);
	//��ȡ����λ��λ��
	BYTE GetIntegerBitCount(DOUBLE dRobMoney);
	//�Ƿ������׵�
	bool IsHitThunderPoint(DOUBLE dRobMoney, WORD wThunderPoint,NUMPOS positon);
	// �������
	int RandomArea(int nAreaVaule[], int nAreaLen);

};

//////////////////////////////////////////////////////////////////////////

#endif
