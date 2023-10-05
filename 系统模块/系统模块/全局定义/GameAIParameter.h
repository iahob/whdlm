#ifndef AIPARAMETER_HEAD_FILE
#define AIPARAMETER_HEAD_FILE

#include "math.h"
#include <list>
using namespace std;

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//常量定义
#define	AI_TIME_SLOT				12								//时段定义
#define AI_RATE_FLOATED				20								//占比浮动
#define AI_CARRY_GRADE				4								//携带等级
#define AI_LEAVE_GRADE				3								//离开等级(最高优先,最优先,次优先)
#define AI_HANGING_GRADE			3								//挂机等级(和携带等级对应)
#define AI_MAX_ENTER_ONEHOUR		20								//一小时最大进入
#define AI_MAX_LEAVE_ONEHOUR		20								//一小时最大离开
#define AI_RANDOMENTER_TENMIN		4								//十分钟进入
#define AI_RANDOMLEAVE_TENMIN		4								//十分钟离开
#define MAX_AI_CHIP_COUNT			5								//最大筹码(携带相关)
#define TIME_DAY_TICKCOUNT			24 * 3600						//二十四小时
#define AI_TOP_VARIATIONTIMES		3								//持有金币/初始金币。比值最大的前3
#define AI_TOP_MAXCARRYSCORE		3								//游戏币最多的机器人前3
#define AI_TOP_MINCARRYSCORE		3								//游戏币最少的机器人前3
#define AI_TOP_MAXLOSTSCORE			5								//游戏输掉金币数量前5
#define AI_INI_SWHTABLE_RATE_MIN	2								//初始换桌概率最小
#define AI_INI_SWHTABLE_RATE_MAX	9								//初始换桌概率最大
#define AI_SWHTABLE_RATE_ADD		4								//换桌概率增加
#define AI_SWHTABLE_RATE_SUB		3								//换桌概率减少
#define AI_FREE_SAMETABLE_SECMIN	5 * 60							//同桌空闲最小
#define AI_FREE_SAMETABLE_SECMAX	10 * 60							//同桌空闲最大

//携带等级
#define AI_CARRY_GRADE_NONE			0x0000							//无等级
#define AI_CARRY_GRADE_A			0x0001							//等级A
#define AI_CARRY_GRADE_B			0x0002							//等级B	
#define AI_CARRY_GRADE_C			0x0004							//等级C
#define AI_CARRY_GRADE_D			0x0008							//等级D

//离开等级
#define AI_LEAVE_GRADE_NONE			0x0010							//无等级次优先
#define AI_LEAVE_GRADE_SECONDARY	0x0020							//最优先
#define AI_LEAVE_GRADE_PRIORITY		0x0040							//最高优先

//掩码
#define AI_CARRY_GRADE_MASK			0x000F							//携带掩码
#define AI_LEAVE_GRADE_MASK			0x00F0							//离开掩码
#define AI_SHIELD_LEAVE_GRADE_MASK	0xFF0F							//屏蔽离开掩码

//基准
#define AI_CARRYGRADE_STANDARD		2								//携带基准
#define AI_LEAVEGRADE_STANDARD		2								//离开基准

//算法
#define DETECTIONPROBABILITY(a)		(((rand() % 100) < a) ? true : false)
#define RANDVAL(a, b)				(a >= b ? b : ((rand() % (b - a + 1) + a)))

//求fStandardVal的N次为fVal
static BYTE sqrtEx(float fStandardVal, float fVal)
{
	BYTE cbRandSqrt = 0;
	if (fStandardVal != 0 && fVal == 1)
	{
		return cbRandSqrt;
	}

	while (true)											
	{			
		if (cbRandSqrt > AI_CARRY_GRADE)
		{
			break;
		}

		if (pow(fStandardVal, (float)cbRandSqrt) == fVal)
		{														
			return cbRandSqrt;
		}														

		cbRandSqrt++;
	}

	//返回INVALID_BYTE
	return 0xFF;
}

//比较类
class SortTop
{
public:
	DWORD					dwUserID;
	SCORE					lSortVal;

	SortTop(DWORD dwUserID, SCORE lSortVal)
	{
		this->dwUserID = dwUserID;
		this->lSortVal = lSortVal;
	}

	SortTop() {}
	~SortTop() {}

public:
	bool operator < (SortTop b)
	{
		return this->lSortVal < b.lSortVal;
	}

	bool operator >(SortTop b)
	{
		return this->lSortVal > b.lSortVal;
	}
};

typedef list<SortTop>::iterator ITSORTTOP;

//////////////////////////////////////////////////////////////////////////
//结构定义
//时间配置
struct tagAITimeInfo
{
	WORD					wHour;								//时间小时
	WORD					wMinute;							//时间分钟
};

//房间AI配置
struct tagRoomAIParameter
{
	DWORD					dwAICountBase;								//A I 基数
	tagAITimeInfo			AITimeSlot[AI_TIME_SLOT];					//时段配置
	WORD					wMinAICountRate[AI_TIME_SLOT];				//数目占比
	WORD					wMaxAICountRate[AI_TIME_SLOT];				//数目占比
	WORD					wMinAIInoutRate[AI_TIME_SLOT];				//进出占比
	WORD					wMaxAIInoutRate[AI_TIME_SLOT];				//进出占比
	LONG					lAICarryScoreMinMultiple[AI_CARRY_GRADE];	//携带倍数
	LONG					lAICarryScoreMaxMultiple[AI_CARRY_GRADE];	//携带倍数
	WORD					wAIScoreLevelRate[AI_CARRY_GRADE];			//携带占比
	WORD					wAIScoreFloatMinRate[AI_CARRY_GRADE];		//浮动占比
	WORD					wAIScoreFloatMaxRate[AI_CARRY_GRADE];		//浮动占比
	WORD					wAILeaveRate[AI_LEAVE_GRADE];				//离开概率
	WORD					wAIHangingMinRate;							//挂机比例
	WORD					wAIHangingMaxRate;							//挂机比例
	WORD					wHangingEnterMinTime[AI_HANGING_GRADE];		//进入时间(分)
	WORD					wHangingEnterMaxTime[AI_HANGING_GRADE];		//进入时间(分)
	WORD					wSetHangingRate[AI_HANGING_GRADE];			//筛选概率
	WORD					wMinuteChangeStatus;						//时间切换

	//默认配置
	tagRoomAIParameter()
	{
		//AI数目基数
		dwAICountBase = 0;

		//时间段配置						//每时段人数范围比例				//每时段进出比例
		AITimeSlot[0].wHour = 0;			wMinAICountRate[0] = 80;			wMinAIInoutRate[0] = 20;
		AITimeSlot[0].wMinute = 0;			wMaxAICountRate[0] = 100;			wMaxAIInoutRate[0] = 60;
		AITimeSlot[1].wHour = 2;			wMinAICountRate[1] = 72;			wMinAIInoutRate[1] = 20;
		AITimeSlot[1].wMinute = 0;			wMaxAICountRate[1] = 90;			wMaxAIInoutRate[1] = 40;
		AITimeSlot[2].wHour = 4;			wMinAICountRate[2] = 64;			wMinAIInoutRate[2] = 25;
		AITimeSlot[2].wMinute = 0;			wMaxAICountRate[2] = 80;			wMaxAIInoutRate[2] = 50;
		AITimeSlot[3].wHour = 6;			wMinAICountRate[3] = 48;			wMinAIInoutRate[3] = 33;
		AITimeSlot[3].wMinute = 0;			wMaxAICountRate[3] = 60;			wMaxAIInoutRate[3] = 50;
		AITimeSlot[4].wHour = 8;			wMinAICountRate[4] = 58;			wMinAIInoutRate[4] = 33;
		AITimeSlot[4].wMinute = 0;			wMaxAICountRate[4] = 70;			wMaxAIInoutRate[4] = 50;
		AITimeSlot[5].wHour = 10;			wMinAICountRate[5] = 64;			wMinAIInoutRate[5] = 12;
		AITimeSlot[5].wMinute = 0;			wMaxAICountRate[5] = 80;			wMaxAIInoutRate[5] = 25;
		AITimeSlot[6].wHour = 12;			wMinAICountRate[6] = 72;			wMinAIInoutRate[6] = 12;
		AITimeSlot[6].wMinute = 0;			wMaxAICountRate[6] = 90;			wMaxAIInoutRate[6] = 25;
		AITimeSlot[7].wHour = 14;			wMinAICountRate[7] = 72;			wMinAIInoutRate[7] = 12;
		AITimeSlot[7].wMinute = 0;			wMaxAICountRate[7] = 90;			wMaxAIInoutRate[7] = 25;
		AITimeSlot[8].wHour = 16;			wMinAICountRate[8] = 80;			wMinAIInoutRate[8] = 10;
		AITimeSlot[8].wMinute = 0;			wMaxAICountRate[8] = 100;			wMaxAIInoutRate[8] = 20;
		AITimeSlot[9].wHour = 18;			wMinAICountRate[9] = 96;			wMinAIInoutRate[9] = 10;
		AITimeSlot[9].wMinute = 0;			wMaxAICountRate[9] = 120;			wMaxAIInoutRate[9] = 20;
		AITimeSlot[10].wHour = 20;			wMinAICountRate[10] = 96;			wMinAIInoutRate[10] = 10;
		AITimeSlot[10].wMinute = 0;			wMaxAICountRate[10] = 120;			wMaxAIInoutRate[10] = 20;
		AITimeSlot[11].wHour = 22;			wMinAICountRate[11] = 96;			wMinAIInoutRate[11] = 10;
		AITimeSlot[11].wMinute = 0;			wMaxAICountRate[11] = 120;			wMaxAIInoutRate[11] = 20;

		//进入携带金币为游戏配置最小筹码倍数,第二筹码倍数以及人数占比
		lAICarryScoreMinMultiple[0] = 200;
		lAICarryScoreMaxMultiple[0] = 250;
		wAIScoreLevelRate[0] = 14;
		wAIScoreFloatMinRate[0] = 80;
		wAIScoreFloatMaxRate[0] = 120;
		//进入携带金币为游戏配置第二个筹码倍数,第三筹码倍数以及人数占比
		lAICarryScoreMinMultiple[1] = 251;
		lAICarryScoreMaxMultiple[1] = 300;
		wAIScoreLevelRate[1] = 43;
		wAIScoreFloatMinRate[1] = 80;
		wAIScoreFloatMaxRate[1] = 120;
		//进入携带金币为游戏配置第三个筹码倍数,第四筹码倍数以及人数占比
		lAICarryScoreMinMultiple[2] = 301;
		lAICarryScoreMaxMultiple[2] = 500;
		wAIScoreLevelRate[2] = 36;
		wAIScoreFloatMinRate[2] = 80;
		wAIScoreFloatMaxRate[2] = 120;
		//进入携带金币为游戏配置第四个筹码倍数,最大筹码倍数以及人数占比
		lAICarryScoreMinMultiple[3] = 501;
		lAICarryScoreMaxMultiple[3] = 1000;
		wAIScoreLevelRate[3] = 7;
		wAIScoreFloatMinRate[3] = 80;
		wAIScoreFloatMaxRate[3] = 120;

		//机器离开人数设定
		wAILeaveRate[0] = 80;
		wAILeaveRate[1] = 90;
		wAILeaveRate[2] = 100;

		//机器人挂机
		wAIHangingMinRate = 20;
		wAIHangingMaxRate = 30;

		//筛选概率
		wSetHangingRate[0] = 15;
		wHangingEnterMinTime[0] = 3;
		wHangingEnterMaxTime[0] = 10;
		wSetHangingRate[1] = 40;
		wHangingEnterMinTime[1] = 30;
		wHangingEnterMaxTime[1] = 60;
		wSetHangingRate[2] = 80;
		wHangingEnterMinTime[2] = 180;
		wHangingEnterMaxTime[2] = 180;

		//间隔时间切换挂机状态
		wMinuteChangeStatus = 15;
	}
};

struct tagAIEnterInfo
{
	DWORD					dwAIEntetTime;						//进入时间
	SCORE					dAIEnterScore;						//进入金币
};

//获取开启服务端时间TimeSlotIndex
static BYTE GetServerTimeSlotIndex(tagRoomAIParameter RoomAIParameter)
{
	BYTE cbTimeSlotIndex = 0xFF;

	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	
	//精确到分钟
	WORD wHour = SystemTime.wHour;
	WORD wMinute = SystemTime.wMinute;
	WORD wTotalMinute = wHour * 60 + wMinute;

	//0点~22点
	for (BYTE i = 0; i < AI_TIME_SLOT - 1; i++)
	{
		WORD wFirstLoadMinute = RoomAIParameter.AITimeSlot[i].wHour * 60 + RoomAIParameter.AITimeSlot[i].wMinute;
		WORD wSecLoadMinute = RoomAIParameter.AITimeSlot[i + 1].wHour * 60 + RoomAIParameter.AITimeSlot[i + 1].wMinute;

		if (wTotalMinute >= wFirstLoadMinute && wTotalMinute < wSecLoadMinute)
		{
			cbTimeSlotIndex = i;
			break;
		}
	}

	//22点~0点
	if (!(cbTimeSlotIndex >= 0 && cbTimeSlotIndex < AI_TIME_SLOT))
	{
		WORD wFirstLoadMinute = RoomAIParameter.AITimeSlot[11].wHour * 60 + RoomAIParameter.AITimeSlot[11].wMinute;
		WORD wSecLoadMinute = 24 * 60;

		if (wTotalMinute >= wFirstLoadMinute && wTotalMinute < wSecLoadMinute)
		{
			cbTimeSlotIndex = 11;
		}
	}

	ASSERT(cbTimeSlotIndex >= 0 && cbTimeSlotIndex < AI_TIME_SLOT);

	return cbTimeSlotIndex;
}

//////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif