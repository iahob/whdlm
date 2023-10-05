#pragma once

#include "../DexterSDK/Include/Dexter.h"

// ������Ϣ
struct tagGiftInfo
{
	uint				nGiftType;
	uint				nGiftCount; 

	tagGiftInfo()
	{
		nGiftType = 0;
		nGiftCount = 0;
	}
	tagGiftInfo( tagGiftInfo & Right )
	{
		nGiftType = Right.nGiftType;
		nGiftCount = Right.nGiftCount;
	}
};

// ��Ӯ����
struct tagPersonalScore
{
	tchar				szPlayName[32];
	longlong			lPlayScore;
	__time64_t			TimeModify;

	tagPersonalScore()
	{
		ZeroMemory(szPlayName, sizeof(szPlayName));
		lPlayScore = 0;
		TimeModify = 0;
	}
	tagPersonalScore( tagPersonalScore & Right )
	{
		CopyMemory(szPlayName, Right.szPlayName, sizeof(szPlayName));
		lPlayScore = Right.lPlayScore;
		TimeModify = Right.TimeModify;
	}
};

// ��־��Ϣ
struct tagLogoInfo
{
	tchar				szPlayName[32];
	tchar				szMessage[200];
	__time64_t			TimeModify;

	tagLogoInfo()
	{
		ZeroMemory(szMessage, sizeof(szMessage));
		TimeModify = 0;
	}
	tagLogoInfo( tagLogoInfo & Right )
	{
		CopyMemory(szMessage, Right.szMessage, sizeof(szMessage));
		TimeModify = Right.TimeModify;
	}
};


// �ͻ�����Ϣ
enum
{
	SUBC_C_STOCK_INQUIRY = 5018,						// ����ѯ		
	SUBC_C_STOCK_SET,									// �������

	SUBC_C_DIFFICULTY_SET,								// �Ѷ�����
	SUBC_C_DIFFICULTY_DELETE,							// �Ѷ�ɾ��
	SUBC_C_DIFFICULTY_INQUIRY,							// �ѶȲ�ѯ
	SUBC_C_DIFFICULTY_CLEAR,							// �Ѷ����

	SUBC_C_GIFT_SET,									// ��������
	SUBC_C_GIFT_DELETE,									// ����ɾ��
	SUBC_C_GIFT_INQUIRY,								// ���Ͳ�ѯ
	SUBC_C_GIFT_CLEAR,									// �������

	SUBC_C_WIN_LOSE_UPDATE,								// ��Ӯ����
	SUBC_C_LOG_UPDATE,									// ��־����
};

// ����ѯ
struct CMDC_C_StockInquiry
{
	bool				bActiveInquiry;
};

// �������
struct CMDC_C_StockSet
{
	longlong			lGameStock;
	longlong			lGameJackpot;
};

// �Ѷ�����
struct CMDC_C_DifficultySet
{
	uint				nDifficultyID;
	double				dDifficultyValue;
};

// �Ѷ�ɾ��
struct CMDC_C_DifficultyDelete
{
	uint				nDifficultyID;
};

// �ѶȲ�ѯ
struct CMDC_C_DifficultyInquiry
{
};

// �Ѷ����
struct CMDC_C_DifficultyClear
{
};

// ��������
struct CMDC_C_GiftSet
{
	uint				nGiftID;
	uint				nGiftType;
	uint				nGiftCount; 
};

// ����ɾ��
struct CMDC_C_GiftDelete
{
	uint				nGiftID;
};

// ���Ͳ�ѯ
struct CMDC_C_GiftInquiry
{
};

// �������
struct CMDC_C_GiftClear
{
};

// ��Ӯ����
struct CMDC_C_WinLoseUpdate
{
};

// ��־����
struct CMDC_C_LogUpdate
{
};


// ��������Ϣ
enum
{
	SUBC_S_TEXT_MESSAGE	= 5818,					// ��ʾ��Ϣ
	SUBC_S_STOCK_INQUIRY,						// ����ѯ		
	SUBC_S_DIFFICULTY_INQUIRY,					// �ѶȲ�ѯ
	SUBC_S_GIFT_INQUIRY,						// ���Ͳ�ѯ

	SUBC_S_WIN_LOSE_UPDATE,						// ��Ӯ����
	SUBC_S_LOG_UPDATE,							// ��־����

	SUBC_S_LOG_CUSTOM = 9825,					// �Զ�����Ϣ
};

// ��ʾ��Ϣ
struct CMDC_S_TextMessage
{
	tchar				szMessage[128];
};

// ����ѯ
struct CMDC_S_StockInquiry
{
	longlong			lGameStock;
	longlong			lGameJackpot;
	bool				bMessageBox;
};

// �ѶȲ�ѯ
struct CMDC_S_DifficultyInquiry
{
	uint				nDifficultyID;
	double				dDifficultyValue;
};

// ���Ͳ�ѯ
struct CMDC_S_GiftInquiry
{
	uint				nGiftID;
	uint				nGiftType;
	uint				nGiftCount; 
};


// ��Ӯ����
struct CMDC_S_WinLoseUpdate
{
	uint				nPlayID;
	tchar				szPlayName[32];
	longlong			lPlayScore;
};

// ��־����
struct CMDC_S_LogUpdate
{
	tchar				szPlayName[32];
	tchar				szMessage[200];
	__time64_t			TimeModify;
};
