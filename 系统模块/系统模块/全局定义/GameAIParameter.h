#ifndef AIPARAMETER_HEAD_FILE
#define AIPARAMETER_HEAD_FILE

#include "math.h"
#include <list>
using namespace std;

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//��������
#define	AI_TIME_SLOT				12								//ʱ�ζ���
#define AI_RATE_FLOATED				20								//ռ�ȸ���
#define AI_CARRY_GRADE				4								//Я���ȼ�
#define AI_LEAVE_GRADE				3								//�뿪�ȼ�(�������,������,������)
#define AI_HANGING_GRADE			3								//�һ��ȼ�(��Я���ȼ���Ӧ)
#define AI_MAX_ENTER_ONEHOUR		20								//һСʱ������
#define AI_MAX_LEAVE_ONEHOUR		20								//һСʱ����뿪
#define AI_RANDOMENTER_TENMIN		4								//ʮ���ӽ���
#define AI_RANDOMLEAVE_TENMIN		4								//ʮ�����뿪
#define MAX_AI_CHIP_COUNT			5								//������(Я�����)
#define TIME_DAY_TICKCOUNT			24 * 3600						//��ʮ��Сʱ
#define AI_TOP_VARIATIONTIMES		3								//���н��/��ʼ��ҡ���ֵ����ǰ3
#define AI_TOP_MAXCARRYSCORE		3								//��Ϸ�����Ļ�����ǰ3
#define AI_TOP_MINCARRYSCORE		3								//��Ϸ�����ٵĻ�����ǰ3
#define AI_TOP_MAXLOSTSCORE			5								//��Ϸ����������ǰ5
#define AI_INI_SWHTABLE_RATE_MIN	2								//��ʼ����������С
#define AI_INI_SWHTABLE_RATE_MAX	9								//��ʼ�����������
#define AI_SWHTABLE_RATE_ADD		4								//������������
#define AI_SWHTABLE_RATE_SUB		3								//�������ʼ���
#define AI_FREE_SAMETABLE_SECMIN	5 * 60							//ͬ��������С
#define AI_FREE_SAMETABLE_SECMAX	10 * 60							//ͬ���������

//Я���ȼ�
#define AI_CARRY_GRADE_NONE			0x0000							//�޵ȼ�
#define AI_CARRY_GRADE_A			0x0001							//�ȼ�A
#define AI_CARRY_GRADE_B			0x0002							//�ȼ�B	
#define AI_CARRY_GRADE_C			0x0004							//�ȼ�C
#define AI_CARRY_GRADE_D			0x0008							//�ȼ�D

//�뿪�ȼ�
#define AI_LEAVE_GRADE_NONE			0x0010							//�޵ȼ�������
#define AI_LEAVE_GRADE_SECONDARY	0x0020							//������
#define AI_LEAVE_GRADE_PRIORITY		0x0040							//�������

//����
#define AI_CARRY_GRADE_MASK			0x000F							//Я������
#define AI_LEAVE_GRADE_MASK			0x00F0							//�뿪����
#define AI_SHIELD_LEAVE_GRADE_MASK	0xFF0F							//�����뿪����

//��׼
#define AI_CARRYGRADE_STANDARD		2								//Я����׼
#define AI_LEAVEGRADE_STANDARD		2								//�뿪��׼

//�㷨
#define DETECTIONPROBABILITY(a)		(((rand() % 100) < a) ? true : false)
#define RANDVAL(a, b)				(a >= b ? b : ((rand() % (b - a + 1) + a)))

//��fStandardVal��N��ΪfVal
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

	//����INVALID_BYTE
	return 0xFF;
}

//�Ƚ���
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
//�ṹ����
//ʱ������
struct tagAITimeInfo
{
	WORD					wHour;								//ʱ��Сʱ
	WORD					wMinute;							//ʱ�����
};

//����AI����
struct tagRoomAIParameter
{
	DWORD					dwAICountBase;								//A I ����
	tagAITimeInfo			AITimeSlot[AI_TIME_SLOT];					//ʱ������
	WORD					wMinAICountRate[AI_TIME_SLOT];				//��Ŀռ��
	WORD					wMaxAICountRate[AI_TIME_SLOT];				//��Ŀռ��
	WORD					wMinAIInoutRate[AI_TIME_SLOT];				//����ռ��
	WORD					wMaxAIInoutRate[AI_TIME_SLOT];				//����ռ��
	LONG					lAICarryScoreMinMultiple[AI_CARRY_GRADE];	//Я������
	LONG					lAICarryScoreMaxMultiple[AI_CARRY_GRADE];	//Я������
	WORD					wAIScoreLevelRate[AI_CARRY_GRADE];			//Я��ռ��
	WORD					wAIScoreFloatMinRate[AI_CARRY_GRADE];		//����ռ��
	WORD					wAIScoreFloatMaxRate[AI_CARRY_GRADE];		//����ռ��
	WORD					wAILeaveRate[AI_LEAVE_GRADE];				//�뿪����
	WORD					wAIHangingMinRate;							//�һ�����
	WORD					wAIHangingMaxRate;							//�һ�����
	WORD					wHangingEnterMinTime[AI_HANGING_GRADE];		//����ʱ��(��)
	WORD					wHangingEnterMaxTime[AI_HANGING_GRADE];		//����ʱ��(��)
	WORD					wSetHangingRate[AI_HANGING_GRADE];			//ɸѡ����
	WORD					wMinuteChangeStatus;						//ʱ���л�

	//Ĭ������
	tagRoomAIParameter()
	{
		//AI��Ŀ����
		dwAICountBase = 0;

		//ʱ�������						//ÿʱ��������Χ����				//ÿʱ�ν�������
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

		//����Я�����Ϊ��Ϸ������С���뱶��,�ڶ����뱶���Լ�����ռ��
		lAICarryScoreMinMultiple[0] = 200;
		lAICarryScoreMaxMultiple[0] = 250;
		wAIScoreLevelRate[0] = 14;
		wAIScoreFloatMinRate[0] = 80;
		wAIScoreFloatMaxRate[0] = 120;
		//����Я�����Ϊ��Ϸ���õڶ������뱶��,�������뱶���Լ�����ռ��
		lAICarryScoreMinMultiple[1] = 251;
		lAICarryScoreMaxMultiple[1] = 300;
		wAIScoreLevelRate[1] = 43;
		wAIScoreFloatMinRate[1] = 80;
		wAIScoreFloatMaxRate[1] = 120;
		//����Я�����Ϊ��Ϸ���õ��������뱶��,���ĳ��뱶���Լ�����ռ��
		lAICarryScoreMinMultiple[2] = 301;
		lAICarryScoreMaxMultiple[2] = 500;
		wAIScoreLevelRate[2] = 36;
		wAIScoreFloatMinRate[2] = 80;
		wAIScoreFloatMaxRate[2] = 120;
		//����Я�����Ϊ��Ϸ���õ��ĸ����뱶��,�����뱶���Լ�����ռ��
		lAICarryScoreMinMultiple[3] = 501;
		lAICarryScoreMaxMultiple[3] = 1000;
		wAIScoreLevelRate[3] = 7;
		wAIScoreFloatMinRate[3] = 80;
		wAIScoreFloatMaxRate[3] = 120;

		//�����뿪�����趨
		wAILeaveRate[0] = 80;
		wAILeaveRate[1] = 90;
		wAILeaveRate[2] = 100;

		//�����˹һ�
		wAIHangingMinRate = 20;
		wAIHangingMaxRate = 30;

		//ɸѡ����
		wSetHangingRate[0] = 15;
		wHangingEnterMinTime[0] = 3;
		wHangingEnterMaxTime[0] = 10;
		wSetHangingRate[1] = 40;
		wHangingEnterMinTime[1] = 30;
		wHangingEnterMaxTime[1] = 60;
		wSetHangingRate[2] = 80;
		wHangingEnterMinTime[2] = 180;
		wHangingEnterMaxTime[2] = 180;

		//���ʱ���л��һ�״̬
		wMinuteChangeStatus = 15;
	}
};

struct tagAIEnterInfo
{
	DWORD					dwAIEntetTime;						//����ʱ��
	SCORE					dAIEnterScore;						//������
};

//��ȡ���������ʱ��TimeSlotIndex
static BYTE GetServerTimeSlotIndex(tagRoomAIParameter RoomAIParameter)
{
	BYTE cbTimeSlotIndex = 0xFF;

	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	
	//��ȷ������
	WORD wHour = SystemTime.wHour;
	WORD wMinute = SystemTime.wMinute;
	WORD wTotalMinute = wHour * 60 + wMinute;

	//0��~22��
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

	//22��~0��
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