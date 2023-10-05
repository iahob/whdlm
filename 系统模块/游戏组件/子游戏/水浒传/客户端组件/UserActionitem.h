#ifndef USER_ACTION_ITEM_HEAD_FILE
#define USER_ACTION_ITEM_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//�ƶ�����
enum enmActionKind
{
	AK_NULL					= 0,							//
	AK_One_GunDong,											//��������
	AK_One_YaXian,											//Ѻ��
	AK_One_KaiJian,											//����
	AK_Two_WaitBet,											//�ȴ���ע
	AK_Two_KaiJian,											//����
	AK_Three_GunDong,										//�ȴ���ע
	AK_Three_KaiJian,										//����
	AK_Three_KaiJianEnd,									//����
	AK_Three_End,											//����
	AK_GameConclude,										//����
	AK_Enter,												//����
};

//////////////////////////////////////////////////////////////////////////

//��Ҷ���
struct tagUserAction
{
	enmActionKind					enActionKind;						//��������
	BOOL							bHandleStatus;						//����״̬

	//���캯��
	tagUserAction()
	{
		enActionKind = AK_NULL;
		ResetAction();
	}
	//��������
	virtual ~tagUserAction() {}
	//���ú���
	virtual VOID ResetAction()
	{
		bHandleStatus = FALSE;
	}
};

//Ѻ�߿���
struct tagActionOneGunDong: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	SCORE						lScore;

	//���캯��
	tagActionOneGunDong()
	{
		enActionKind = AK_One_GunDong;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
		lScore = 0;
	}
};

//Ѻ�߿���
struct tagActionOneYaXian: public tagUserAction
{
	SCORE						lScore;
	SCORE						lXianScore;
	int							nZhongJiangXian;
	CPoint						ptXian[ITEM_X_COUNT];

	//���캯��
	tagActionOneYaXian()
	{
		enActionKind = AK_One_YaXian;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		lScore = 0;
		lXianScore = 0;
		nZhongJiangXian = 0;
		ZeroMemory(ptXian, sizeof(ptXian));
	}
};

//����
struct tagActionOneKaiJian: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	SCORE						lScore;
	SCORE						lQuanPanScore;
	BYTE						cbGameMode;
	bool						bZhongJiang[ITEM_Y_COUNT][ITEM_X_COUNT];

	//���캯��
	tagActionOneKaiJian()
	{
		enActionKind = AK_One_KaiJian;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
		lScore = 0;
		lQuanPanScore = 0;
		cbGameMode = 0;
		ZeroMemory(bZhongJiang, sizeof(bZhongJiang));
	}
};

//�ȴ���ע
struct tagActionTwoWaitBet: public tagUserAction
{
	bool						bOne;
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbUser[3];
	BYTE						cbExp;
	SCORE						lScore;								//��ǰ����

	//���캯��
	tagActionTwoWaitBet()
	{
		enActionKind = AK_Two_WaitBet;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		bOne = true;
		nCurIndex = 0;
		nMaxIndex = 0;
		ZeroMemory(cbUser, sizeof(cbUser));
		cbExp = 0;
		lScore = 0;
	}
};

//����
struct tagActionTwoKaiJian: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbUser[3];
	BYTE						cbSice[2];
	SCORE						lScore;

	//���캯��
	tagActionTwoKaiJian()
	{
		enActionKind = AK_Two_KaiJian;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
		lScore = 0;
		ZeroMemory(cbUser, sizeof(cbUser));
		ZeroMemory(cbSice, sizeof(cbSice));
	}
};

//����
struct tagActionThreeGunDong: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbBounsGameCount;							//С��������
	BYTE						cbItem[4];
	SCORE						lScore;

	//���캯��
	tagActionThreeGunDong()
	{
		enActionKind = AK_Three_GunDong;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
		lScore = 0;
		cbBounsGameCount = 0;
		ZeroMemory(cbItem, sizeof(cbItem));
	}
};

//����
struct tagActionThreeKaiJian: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbIndex;
	BYTE						nTime;
	SCORE						lScore;

	//���캯��
	tagActionThreeKaiJian()
	{
		enActionKind = AK_Three_KaiJian;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
		lScore = 0;
		nTime = 0;
		cbIndex = 0xff;
	}
};


//����
struct tagActionThreeKaiJianEnd: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbIndex;
	SCORE						lScore;

	//���캯��
	tagActionThreeKaiJianEnd()
	{
		enActionKind = AK_Three_KaiJianEnd;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
		lScore = 0;
		cbIndex = 0xff;
	}
};

//����
struct tagActionThreeEnd: public tagUserAction
{
	//���캯��
	tagActionThreeEnd()
	{
		enActionKind = AK_Three_End;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
	}
};

//����
struct tagActionGameConclude: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;

	//���캯��
	tagActionGameConclude()
	{
		enActionKind = AK_GameConclude;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
	}
};

//����
struct tagActionEnter: public tagUserAction
{
	BYTE				cbEnterMode;

	//���캯��
	tagActionEnter()
	{
		enActionKind = AK_Enter;
		ResetAction();
	}
	//���ú���
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		cbEnterMode = 1;
	}
};

#endif