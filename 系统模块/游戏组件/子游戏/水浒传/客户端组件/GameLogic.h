#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
//////////////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const CPoint				m_ptXian[ITEM_COUNT][ITEM_X_COUNT];			//扑克数据
	static const BYTE				m_cbItemType[24];

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//控制函数
public:
	//随机函数
	LONGLONG GetRandMax(BYTE cbIndex, LONGLONG lZhongJiang[], int nCount);
	//随机函数
	LONGLONG LLRand(LONGLONG lMaxCount);
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[][ITEM_X_COUNT], double dControl);
	//取得扑克
	VOID GetCardData(BYTE cbCardBuffer[ITEM_COUNT][ITEM_X_COUNT], double dControl);
	//混乱扑克
	VOID RandCardListEx(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//取得扑克
	int RandCardListThree(BYTE &cbIndex, BYTE cbCardBuffer[4], double dControl);


	//逻辑函数
public:
	//取得中奖
	BYTE GetZhongJiangInfo(BYTE cbItemInfo[][ITEM_X_COUNT], CPoint ptZhongJiang[][ITEM_X_COUNT]);
	//取得中奖
	BYTE GetZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT]);
	//取得中奖
	int GetZhongJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//取得中奖
	int GetZhongJiangTime(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT]);
	//全盘奖
	int GetQuanPanJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//进入小玛丽
	BYTE GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//取得中奖
	LONGLONG GetThreeZhongJiangInfo(BYTE cbItemInfo[], BYTE& cbZhongPos);
	//取得中奖
	bool GetThreeZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[]);

	//内部函数
public:
	//单线中奖
	BYTE GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT]);
	//单线中奖
	int GetZhongJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT]);
	//进入小玛丽
	BYTE GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT]);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif