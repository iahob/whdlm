#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//数位定制
typedef enum
{
	THOUSAND_POS = 17,		//千位数字
	HUNDRED_POS,			//百位数字
	TEN_POS,				//十位数字
	ONE_POS,				//个位数字
	DECIMALFIRST_POS,		//小数点后第一位
	DECIMALSECOND_POS,		//小数点后第二位
	INVALID_POS,			//无效位
}NUMPOS;

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();
	
	//乱序派发红包
	void RandDispatchMoney(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE dDispatchArray[]);
	//随机生成红包
	DOUBLE RandDispatchMoney(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE dMinMoney = 0.01);
	//获取数位上数字
	BYTE GetNumPos(DOUBLE dRobMoney, NUMPOS numpos);
	//获取整数位的位数
	BYTE GetIntegerBitCount(DOUBLE dRobMoney);
	//是否命中雷点
	bool IsHitThunderPoint(DOUBLE dRobMoney, WORD wThunderPoint,NUMPOS positon);
	// 随机区域
	int RandomArea(int nAreaVaule[], int nAreaLen);

};

//////////////////////////////////////////////////////////////////////////

#endif
