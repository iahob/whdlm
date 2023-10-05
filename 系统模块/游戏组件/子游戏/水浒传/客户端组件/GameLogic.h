#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
//////////////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const CPoint				m_ptXian[ITEM_COUNT][ITEM_X_COUNT];			//�˿�����
	static const BYTE				m_cbItemType[24];

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ƺ���
public:
	//�������
	LONGLONG GetRandMax(BYTE cbIndex, LONGLONG lZhongJiang[], int nCount);
	//�������
	LONGLONG LLRand(LONGLONG lMaxCount);
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[][ITEM_X_COUNT], double dControl);
	//ȡ���˿�
	VOID GetCardData(BYTE cbCardBuffer[ITEM_COUNT][ITEM_X_COUNT], double dControl);
	//�����˿�
	VOID RandCardListEx(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//ȡ���˿�
	int RandCardListThree(BYTE &cbIndex, BYTE cbCardBuffer[4], double dControl);


	//�߼�����
public:
	//ȡ���н�
	BYTE GetZhongJiangInfo(BYTE cbItemInfo[][ITEM_X_COUNT], CPoint ptZhongJiang[][ITEM_X_COUNT]);
	//ȡ���н�
	BYTE GetZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT]);
	//ȡ���н�
	int GetZhongJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//ȡ���н�
	int GetZhongJiangTime(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT]);
	//ȫ�̽�
	int GetQuanPanJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//����С����
	BYTE GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//ȡ���н�
	LONGLONG GetThreeZhongJiangInfo(BYTE cbItemInfo[], BYTE& cbZhongPos);
	//ȡ���н�
	bool GetThreeZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[]);

	//�ڲ�����
public:
	//�����н�
	BYTE GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT]);
	//�����н�
	int GetZhongJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT]);
	//����С����
	BYTE GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT]);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif