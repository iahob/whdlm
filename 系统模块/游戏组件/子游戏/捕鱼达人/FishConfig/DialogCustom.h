#pragma once
#include "Resource.h"
#include "DialogDexter.h"

// 声明变量
extern tagCustomRule g_CustomRule;						// 全局配置

// 倍数配置
class CDialogCustom1 : public CDialogDexter
{
	// 函数信息
public:
	// 构造函数
	CDialogCustom1() : CDialogDexter(IDD_DIALOG_1) {};

	// 绑定控件
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		// 倍数配置
		DDX_Text(pDX, IDC_EDIT_MUL_1, g_CustomRule.nMultipleValue[0]);
		DDX_Text(pDX, IDC_EDIT_MUL_2, g_CustomRule.nMultipleValue[1]);
		DDX_Text(pDX, IDC_EDIT_MUL_3, g_CustomRule.nMultipleValue[2]);
		DDX_Text(pDX, IDC_EDIT_MUL_4, g_CustomRule.nMultipleValue[3]);
		DDX_Text(pDX, IDC_EDIT_MUL_5, g_CustomRule.nMultipleValue[4]);
		DDX_Text(pDX, IDC_EDIT_MUL_6, g_CustomRule.nMultipleValue[5]);
		DDX_Text(pDX, IDC_EDIT_MUL_8, g_CustomRule.nMultipleValue[6]);
		DDX_Text(pDX, IDC_EDIT_MUL_9, g_CustomRule.nMultipleValue[7]);
		DDX_Text(pDX, IDC_EDIT_MUL_10, g_CustomRule.nMultipleValue[8]);
		//DDX_Text(pDX, IDC_EDIT_MUL_11, g_CustomRule.nMultipleValue[9]);
		DDX_Text(pDX, IDC_EDIT_MUL_7, g_CustomRule.nScoreRatio);
	}
};


// 调试配置
class CDialogCustom2 : public CDialogDexter
{
	// 函数信息
public:
	// 构造函数
	CDialogCustom2() : CDialogDexter(IDD_DIALOG_2) {};
	//初始化窗口
	virtual BOOL OnInitDialog()
	{
		((CEdit*)GetDlgItem(IDC_EDIT_63))->EnableWindow(FALSE);
		return __super::OnInitDialog();

	}	

	// 绑定控件
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		// 调试配置	
		DDX_Text(pDX, IDC_EDIT_PARAMETER_K, g_CustomRule.nParameterK);
		DDX_Text(pDX, IDC_EDIT_SYSTEM_STORAGE, g_CustomRule.lSystemStorage);
		DDX_Text(pDX, IDC_EDIT_USER_STORAGE, g_CustomRule.lUserStorage);

		DDX_Check(pDX, IDC_CHECK_TIME, g_CustomRule.nFixTime);
		DDX_Text(pDX, IDC_EDIT_63, g_CustomRule.nTimeWrite);
		DDV_MinMaxInt(pDX,g_CustomRule.nTimeWrite,1,1440);

	}

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckTime();

	afx_msg void OnEnChangeEditSystemStorage();
	afx_msg void OnEnChangeEditUserStorage();
	afx_msg void OnEnChangeEditParameterK();
};


// 场景配置
class CDialogCustom3 : public CDialogDexter
{
	// 函数信息
public:
	// 构造函数
	CDialogCustom3() : CDialogDexter(IDD_DIALOG_3) {};

	// 绑定控件
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		// 场景配置
		DDX_Text(pDX, IDC_EDIT_1, g_CustomRule.nCreateCount);
		DDX_Text(pDX, IDC_EDIT_2, g_CustomRule.nSceneTime);
		DDX_Text(pDX, IDC_EDIT_3, g_CustomRule.nBulletVelocity);
		DDX_Text(pDX, IDC_EDIT_4, g_CustomRule.nBulletCoolingTime);

		DDX_Text(pDX, IDC_EDIT_5, g_CustomRule.nExplosionProportion);
		DDX_Text(pDX, IDC_EDIT_21, g_CustomRule.nExplosionStart[0]);
		DDX_Text(pDX, IDC_EDIT_6, g_CustomRule.nExplosionStart[1]);
		DDX_Text(pDX, IDC_EDIT_7, g_CustomRule.lExplosionCondition);
		DDX_Text(pDX, IDC_EDIT_8, g_CustomRule.lExplosionObtainMax24);

		DDX_CBIndex( pDX, IDC_COMBO1, (int &)g_CustomRule.nExplosionConditionType );

		DDX_Text(pDX, IDC_EDIT_9,  g_CustomRule.nAndroidMultiple[0]);
		DDX_Text(pDX, IDC_EDIT_10, g_CustomRule.lAndroidScore[0]);
		DDX_Text(pDX, IDC_EDIT_11, g_CustomRule.nAndroidMultiple[1]);
		DDX_Text(pDX, IDC_EDIT_12, g_CustomRule.lAndroidScore[1]);	
		DDX_Text(pDX, IDC_EDIT_13, g_CustomRule.nAndroidMultiple[2]);
		DDX_Text(pDX, IDC_EDIT_14, g_CustomRule.lAndroidScore[2]);
		DDX_Text(pDX, IDC_EDIT_15, g_CustomRule.nAndroidMultiple[3]);
		DDX_Text(pDX, IDC_EDIT_16, g_CustomRule.lAndroidScore[3]);
		DDX_Text(pDX, IDC_EDIT_17, g_CustomRule.nAndroidMultiple[4]);
		DDX_Text(pDX, IDC_EDIT_18, g_CustomRule.lAndroidScore[4]);

		DDX_Text(pDX, IDC_EDIT_19, g_CustomRule.nMaxTipCount);
		DDX_Text(pDX, IDC_EDIT_63, g_CustomRule.nMaxBigFishCount);
	
		DDX_Text(pDX, IDC_EDIT_20, g_CustomRule.lAndroidScoreMax);

		DDV_MinMaxInt(pDX,g_CustomRule.nMaxTipCount,1,10);

	}
};

// 活动配置
class CDialogCustom4 : public CDialogDexter
{
	// 函数信息
public:
	// 构造函数
	CDialogCustom4() : CDialogDexter(IDD_DIALOG_4) {};

	// 绑定控件
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		// 活动配置
		DDX_Text(pDX, IDC_EDIT_ACT_1, g_CustomRule.nLaserTime);
		DDX_Text(pDX, IDC_EDIT_ACT_2, g_CustomRule.nLaserChance);
		DDX_Text(pDX, IDC_EDIT_ACT_4, g_CustomRule.nSpeedTime);
		DDX_Text(pDX, IDC_EDIT_ACT_5, g_CustomRule.nSpeedChance);;
		DDX_Text(pDX, IDC_EDIT_ACT_8, g_CustomRule.nGiftScore[0]);
		DDX_Text(pDX, IDC_EDIT_ACT_9, g_CustomRule.nGiftChance[0]);
		DDX_Text(pDX, IDC_EDIT_ACT_10, g_CustomRule.nGiftScore[1]);
		DDX_Text(pDX, IDC_EDIT_ACT_11, g_CustomRule.nGiftChance[1]);
		DDX_Text(pDX, IDC_EDIT_ACT_12, g_CustomRule.nGiftScore[2]);
		DDX_Text(pDX, IDC_EDIT_ACT_13, g_CustomRule.nGiftChance[2]);
		DDX_Text(pDX, IDC_EDIT_ACT_14, g_CustomRule.nGiftScore[3]);
		DDX_Text(pDX, IDC_EDIT_ACT_15, g_CustomRule.nGiftChance[3]);
		DDX_Text(pDX, IDC_EDIT_ACT_16, g_CustomRule.nGiftScore[4]);
		DDX_Text(pDX, IDC_EDIT_ACT_17, g_CustomRule.nGiftChance[4]);
		DDX_Text(pDX, IDC_EDIT_ACT_18, g_CustomRule.nNullChance);
		DDX_Text(pDX, IDC_EDIT_ACT_19, g_CustomRule.nSupplyCondition[0]);
		DDX_Text(pDX, IDC_EDIT_ACT_20, g_CustomRule.nSupplyCondition[1]);
		DDX_Text(pDX, IDC_EDIT_ACT_7, g_CustomRule.nSpeedScore);
	}

	//初始化窗口
	virtual BOOL OnInitDialog()
	{
		((CEdit*)GetDlgItem(IDC_EDIT_ACT_7))->LimitText(1);
		return __super::OnInitDialog();

	}
};

// 鱼配置
class CDialogCustom5 : public CDialogDexter
{
	// 函数信息
public:
	// 构造函数
	CDialogCustom5() : CDialogDexter(IDD_DIALOG_5) {};

	// 绑定控件
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		// 鱼配置
		DDX_Text(pDX, IDC_EDIT_FIS_1, g_CustomRule.nCatchFishMultiple[0][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_2, g_CustomRule.nCatchFishMultiple[0][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_3, g_CustomRule.nCatchFishMultiple[1][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_4, g_CustomRule.nCatchFishMultiple[1][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_5, g_CustomRule.nCatchFishMultiple[2][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_6, g_CustomRule.nCatchFishMultiple[2][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_7, g_CustomRule.nCatchFishMultiple[3][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_8, g_CustomRule.nCatchFishMultiple[3][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_9, g_CustomRule.nCatchFishMultiple[4][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_10, g_CustomRule.nCatchFishMultiple[4][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_11, g_CustomRule.nCatchFishMultiple[5][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_12, g_CustomRule.nCatchFishMultiple[5][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_13, g_CustomRule.nCatchFishMultiple[6][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_14, g_CustomRule.nCatchFishMultiple[6][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_15, g_CustomRule.nCatchFishMultiple[7][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_16, g_CustomRule.nCatchFishMultiple[7][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_17, g_CustomRule.nCatchFishMultiple[8][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_18, g_CustomRule.nCatchFishMultiple[8][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_19, g_CustomRule.nCatchFishMultiple[9][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_20, g_CustomRule.nCatchFishMultiple[9][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_21, g_CustomRule.nCatchFishMultiple[10][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_22, g_CustomRule.nCatchFishMultiple[10][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_23, g_CustomRule.nCatchFishMultiple[11][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_24, g_CustomRule.nCatchFishMultiple[11][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_25, g_CustomRule.nCatchFishMultiple[12][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_26, g_CustomRule.nCatchFishMultiple[12][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_27, g_CustomRule.nCatchFishMultiple[13][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_28, g_CustomRule.nCatchFishMultiple[13][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_29, g_CustomRule.nCatchFishMultiple[14][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_30, g_CustomRule.nCatchFishMultiple[14][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_31, g_CustomRule.nCatchFishMultiple[15][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_32, g_CustomRule.nCatchFishMultiple[15][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_33, g_CustomRule.nCatchFishMultiple[16][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_34, g_CustomRule.nCatchFishMultiple[16][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_35, g_CustomRule.nCatchFishMultiple[17][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_36, g_CustomRule.nCatchFishMultiple[17][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_37, g_CustomRule.nCatchFishMultiple[18][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_38, g_CustomRule.nCatchFishMultiple[18][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_39, g_CustomRule.nCatchFishMultiple[19][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_40, g_CustomRule.nCatchFishMultiple[19][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_41, g_CustomRule.nCatchFishMultiple[20][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_42, g_CustomRule.nCatchFishMultiple[20][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_64, g_CustomRule.nCatchFishMultiple[21][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_65, g_CustomRule.nCatchFishMultiple[21][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_66, g_CustomRule.nCatchFishMultiple[22][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_67, g_CustomRule.nCatchFishMultiple[22][1]);

		DDX_Text(pDX, IDC_EDIT_FIS_68, g_CustomRule.nCatchFishMultiple[23][0]);
		DDX_Text(pDX, IDC_EDIT_FIS_69, g_CustomRule.nCatchFishMultiple[23][1]);

		// 捕鱼难度系数
		DDX_Text(pDX, IDC_EDIT_FIS_43, g_CustomRule.nCatchFishDifficult[0]);

		DDX_Text(pDX, IDC_EDIT_FIS_44, g_CustomRule.nCatchFishDifficult[1]);

		DDX_Text(pDX, IDC_EDIT_FIS_45, g_CustomRule.nCatchFishDifficult[2]);

		DDX_Text(pDX, IDC_EDIT_FIS_46, g_CustomRule.nCatchFishDifficult[3]);

		DDX_Text(pDX, IDC_EDIT_FIS_47, g_CustomRule.nCatchFishDifficult[4]);

		DDX_Text(pDX, IDC_EDIT_FIS_48, g_CustomRule.nCatchFishDifficult[5]);

		DDX_Text(pDX, IDC_EDIT_FIS_49, g_CustomRule.nCatchFishDifficult[6]);

		DDX_Text(pDX, IDC_EDIT_FIS_50, g_CustomRule.nCatchFishDifficult[7]);

		DDX_Text(pDX, IDC_EDIT_FIS_51, g_CustomRule.nCatchFishDifficult[8]);

		DDX_Text(pDX, IDC_EDIT_FIS_52, g_CustomRule.nCatchFishDifficult[9]);

		DDX_Text(pDX, IDC_EDIT_FIS_53, g_CustomRule.nCatchFishDifficult[10]);

		DDX_Text(pDX, IDC_EDIT_FIS_54, g_CustomRule.nCatchFishDifficult[11]);

		DDX_Text(pDX, IDC_EDIT_FIS_55, g_CustomRule.nCatchFishDifficult[12]);

		DDX_Text(pDX, IDC_EDIT_FIS_56, g_CustomRule.nCatchFishDifficult[13]);

		DDX_Text(pDX, IDC_EDIT_FIS_57, g_CustomRule.nCatchFishDifficult[14]);

		DDX_Text(pDX, IDC_EDIT_FIS_58, g_CustomRule.nCatchFishDifficult[15]);

		DDX_Text(pDX, IDC_EDIT_FIS_59, g_CustomRule.nCatchFishDifficult[16]);

		DDX_Text(pDX, IDC_EDIT_FIS_60, g_CustomRule.nCatchFishDifficult[17]);

		DDX_Text(pDX, IDC_EDIT_FIS_61, g_CustomRule.nCatchFishDifficult[18]);

		DDX_Text(pDX, IDC_EDIT_FIS_62, g_CustomRule.nCatchFishDifficult[19]);

		DDX_Text(pDX, IDC_EDIT_FIS_63, g_CustomRule.nCatchFishDifficult[20]);

		DDX_Text(pDX, IDC_EDIT_FIS_70, g_CustomRule.nCatchFishDifficult[21]);

		DDX_Text(pDX, IDC_EDIT_FIS_71, g_CustomRule.nCatchFishDifficult[22]);

		DDX_Text(pDX, IDC_EDIT_FIS_72, g_CustomRule.nCatchFishDifficult[23]);
	}
};

// 鱼配置
class CDialogCustom6 : public CDialogDexter
{
	// 函数信息
public:
	// 构造函数
	CDialogCustom6() : CDialogDexter(IDD_DIALOG_6) {};

	// 绑定控件
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		// 鱼配置
		DDX_Text(pDX, IDC_EDIT1, g_CustomRule.nAwardMinMultiple);
		DDX_Check(pDX, IDC_CHECK1, g_CustomRule.nAwardChatBox);
	}
};
// 元宝配置
class CDialogCustomYuanBao : public CDialogDexter
{
	// 函数信息
public:
	// 构造函数
	CDialogCustomYuanBao() : CDialogDexter(IDD_DIALOG_YUANBAO) {};

	// 绑定控件
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		DDX_Text(pDX, IDC_GOLD_EDIT1, g_CustomRule.nBullionsAriseCount[0]);
		DDX_Text(pDX, IDC_GOLD_EDIT2, g_CustomRule.nBullionsAriseCount[1]);

		DDX_Text(pDX, IDC_GOLD_EDIT3, g_CustomRule.nBullionsAriseTime[0]);
		DDX_Text(pDX, IDC_GOLD_EDIT4, g_CustomRule.nBullionsAriseTime[1]);

		DDX_Text(pDX, IDC_GOLD_EDIT5, g_CustomRule.nBullionsAriseValue[0]);
		DDX_Text(pDX, IDC_GOLD_EDIT6, g_CustomRule.nBullionsAriseValue[1]);

		DDX_CBIndex(pDX, IDC_GOLD_COMBO, (int &)g_CustomRule.nBullionsConditionType);
		DDX_Text(pDX, IDC_GOLD_EDIT7, g_CustomRule.nBullionsCondition);

		DDX_Text(pDX, IDC_GOLD_EDIT8, g_CustomRule.nBullionsObtainMax24);
	}
};