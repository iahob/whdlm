#pragma once

#include "../DexterSDK/Include/Dexter.h"

// 赠送信息
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

// 输赢分数
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

// 日志信息
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


// 客户端消息
enum
{
	SUBC_C_STOCK_INQUIRY = 5018,						// 库存查询		
	SUBC_C_STOCK_SET,									// 库存设置

	SUBC_C_DIFFICULTY_SET,								// 难度设置
	SUBC_C_DIFFICULTY_DELETE,							// 难度删除
	SUBC_C_DIFFICULTY_INQUIRY,							// 难度查询
	SUBC_C_DIFFICULTY_CLEAR,							// 难度清空

	SUBC_C_GIFT_SET,									// 赠送设置
	SUBC_C_GIFT_DELETE,									// 赠送删除
	SUBC_C_GIFT_INQUIRY,								// 赠送查询
	SUBC_C_GIFT_CLEAR,									// 赠送清空

	SUBC_C_WIN_LOSE_UPDATE,								// 输赢更新
	SUBC_C_LOG_UPDATE,									// 日志更新
};

// 库存查询
struct CMDC_C_StockInquiry
{
	bool				bActiveInquiry;
};

// 库存设置
struct CMDC_C_StockSet
{
	longlong			lGameStock;
	longlong			lGameJackpot;
};

// 难度设置
struct CMDC_C_DifficultySet
{
	uint				nDifficultyID;
	double				dDifficultyValue;
};

// 难度删除
struct CMDC_C_DifficultyDelete
{
	uint				nDifficultyID;
};

// 难度查询
struct CMDC_C_DifficultyInquiry
{
};

// 难度清空
struct CMDC_C_DifficultyClear
{
};

// 赠送设置
struct CMDC_C_GiftSet
{
	uint				nGiftID;
	uint				nGiftType;
	uint				nGiftCount; 
};

// 赠送删除
struct CMDC_C_GiftDelete
{
	uint				nGiftID;
};

// 赠送查询
struct CMDC_C_GiftInquiry
{
};

// 赠送清空
struct CMDC_C_GiftClear
{
};

// 输赢更新
struct CMDC_C_WinLoseUpdate
{
};

// 日志更新
struct CMDC_C_LogUpdate
{
};


// 服务器消息
enum
{
	SUBC_S_TEXT_MESSAGE	= 5818,					// 提示信息
	SUBC_S_STOCK_INQUIRY,						// 库存查询		
	SUBC_S_DIFFICULTY_INQUIRY,					// 难度查询
	SUBC_S_GIFT_INQUIRY,						// 赠送查询

	SUBC_S_WIN_LOSE_UPDATE,						// 输赢更新
	SUBC_S_LOG_UPDATE,							// 日志更新

	SUBC_S_LOG_CUSTOM = 9825,					// 自定义消息
};

// 提示信息
struct CMDC_S_TextMessage
{
	tchar				szMessage[128];
};

// 库存查询
struct CMDC_S_StockInquiry
{
	longlong			lGameStock;
	longlong			lGameJackpot;
	bool				bMessageBox;
};

// 难度查询
struct CMDC_S_DifficultyInquiry
{
	uint				nDifficultyID;
	double				dDifficultyValue;
};

// 赠送查询
struct CMDC_S_GiftInquiry
{
	uint				nGiftID;
	uint				nGiftType;
	uint				nGiftCount; 
};


// 输赢更新
struct CMDC_S_WinLoseUpdate
{
	uint				nPlayID;
	tchar				szPlayName[32];
	longlong			lPlayScore;
};

// 日志更新
struct CMDC_S_LogUpdate
{
	tchar				szPlayName[32];
	tchar				szMessage[200];
	__time64_t			TimeModify;
};
