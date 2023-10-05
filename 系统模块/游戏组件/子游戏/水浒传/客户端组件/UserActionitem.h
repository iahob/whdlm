#ifndef USER_ACTION_ITEM_HEAD_FILE
#define USER_ACTION_ITEM_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//移动类型
enum enmActionKind
{
	AK_NULL					= 0,							//
	AK_One_GunDong,											//滚动开牌
	AK_One_YaXian,											//押线
	AK_One_KaiJian,											//开奖
	AK_Two_WaitBet,											//等待下注
	AK_Two_KaiJian,											//开奖
	AK_Three_GunDong,										//等待下注
	AK_Three_KaiJian,										//开奖
	AK_Three_KaiJianEnd,									//开奖
	AK_Three_End,											//开奖
	AK_GameConclude,										//开奖
	AK_Enter,												//开奖
};

//////////////////////////////////////////////////////////////////////////

//玩家动作
struct tagUserAction
{
	enmActionKind					enActionKind;						//动作类型
	BOOL							bHandleStatus;						//处理状态

	//构造函数
	tagUserAction()
	{
		enActionKind = AK_NULL;
		ResetAction();
	}
	//析构函数
	virtual ~tagUserAction() {}
	//重置函数
	virtual VOID ResetAction()
	{
		bHandleStatus = FALSE;
	}
};

//押线开牌
struct tagActionOneGunDong: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	SCORE						lScore;

	//构造函数
	tagActionOneGunDong()
	{
		enActionKind = AK_One_GunDong;
		ResetAction();
	}
	//重置函数
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
		lScore = 0;
	}
};

//押线开牌
struct tagActionOneYaXian: public tagUserAction
{
	SCORE						lScore;
	SCORE						lXianScore;
	int							nZhongJiangXian;
	CPoint						ptXian[ITEM_X_COUNT];

	//构造函数
	tagActionOneYaXian()
	{
		enActionKind = AK_One_YaXian;
		ResetAction();
	}
	//重置函数
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		lScore = 0;
		lXianScore = 0;
		nZhongJiangXian = 0;
		ZeroMemory(ptXian, sizeof(ptXian));
	}
};

//开奖
struct tagActionOneKaiJian: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	SCORE						lScore;
	SCORE						lQuanPanScore;
	BYTE						cbGameMode;
	bool						bZhongJiang[ITEM_Y_COUNT][ITEM_X_COUNT];

	//构造函数
	tagActionOneKaiJian()
	{
		enActionKind = AK_One_KaiJian;
		ResetAction();
	}
	//重置函数
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

//等待下注
struct tagActionTwoWaitBet: public tagUserAction
{
	bool						bOne;
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbUser[3];
	BYTE						cbExp;
	SCORE						lScore;								//当前积分

	//构造函数
	tagActionTwoWaitBet()
	{
		enActionKind = AK_Two_WaitBet;
		ResetAction();
	}
	//重置函数
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

//开奖
struct tagActionTwoKaiJian: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbUser[3];
	BYTE						cbSice[2];
	SCORE						lScore;

	//构造函数
	tagActionTwoKaiJian()
	{
		enActionKind = AK_Two_KaiJian;
		ResetAction();
	}
	//重置函数
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

//开奖
struct tagActionThreeGunDong: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbBounsGameCount;							//小玛丽次数
	BYTE						cbItem[4];
	SCORE						lScore;

	//构造函数
	tagActionThreeGunDong()
	{
		enActionKind = AK_Three_GunDong;
		ResetAction();
	}
	//重置函数
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

//开奖
struct tagActionThreeKaiJian: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbIndex;
	BYTE						nTime;
	SCORE						lScore;

	//构造函数
	tagActionThreeKaiJian()
	{
		enActionKind = AK_Three_KaiJian;
		ResetAction();
	}
	//重置函数
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


//开奖
struct tagActionThreeKaiJianEnd: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;
	BYTE						cbIndex;
	SCORE						lScore;

	//构造函数
	tagActionThreeKaiJianEnd()
	{
		enActionKind = AK_Three_KaiJianEnd;
		ResetAction();
	}
	//重置函数
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
		lScore = 0;
		cbIndex = 0xff;
	}
};

//开奖
struct tagActionThreeEnd: public tagUserAction
{
	//构造函数
	tagActionThreeEnd()
	{
		enActionKind = AK_Three_End;
		ResetAction();
	}
	//重置函数
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
	}
};

//开奖
struct tagActionGameConclude: public tagUserAction
{
	int							nCurIndex;
	int							nMaxIndex;

	//构造函数
	tagActionGameConclude()
	{
		enActionKind = AK_GameConclude;
		ResetAction();
	}
	//重置函数
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		nCurIndex = 0;
		nMaxIndex = 0;
	}
};

//开奖
struct tagActionEnter: public tagUserAction
{
	BYTE				cbEnterMode;

	//构造函数
	tagActionEnter()
	{
		enActionKind = AK_Enter;
		ResetAction();
	}
	//重置函数
	VOID ResetAction()
	{
		bHandleStatus = FALSE;
		cbEnterMode = 1;
	}
};

#endif