#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "TableFrameSink.h"

//�Զ�������
struct tagCustomGeneral
{
	//��ׯ��Ϣ
	LONGLONG						lApplyBankerCondition;			//��������
	LONGLONG						lBankerTime;					//��ׯ����
	LONGLONG						lBankerTimeAdd;					//ׯ��������
	LONGLONG						lBankerScoreMAX;				//������ׯ�����
	LONGLONG						lBankerTimeExtra;				//������ׯ����
	BOOL							nEnableSysBanker;				//ϵͳ��ׯ
	
	//ʱ��
	LONGLONG						lFreeTime;						//����ʱ��
	LONGLONG						lBetTime;						//��עʱ��
	LONGLONG						lEndTime;						//����ʱ��

	//��ע����
	LONGLONG						lAreaLimitScore;				//��������
	LONGLONG						lUserLimitScore;				//��������
	LONGLONG						lEndGameMul;					//��ǰ���ưٷֱ�
	
	//��Ϣ
	TCHAR							szMessageItem1[64];				//��Ϣ1
	TCHAR							szMessageItem2[64];				//��Ϣ2
	TCHAR							szMessageItem3[64];				//��Ϣ3

	//���
	SCORE							lConfigSysStorage;				//ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							lConfigPlayerStorage;			//��ҿ��(���ҪӮ��Ǯ)
	SCORE							lConfigParameterK;				//����ϵ��(�ٷֱ�):
	
	//������ׯ
	SUPERBANKERCONFIG				superbankerConfig;				//��ׯ����
	//ռλ
	OCCUPYSEATCONFIG				occupyseatConfig;				//ռλ����

	LONGLONG						lChipArray[MAX_CHIP_COUNT];		//��������

	//���캯��
	tagCustomGeneral()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		lApplyBankerCondition = 10000000;
		lBankerTime = 10;
		lBankerTimeAdd = 10;
		lBankerScoreMAX = 100000000;
		lBankerTimeExtra = 10;
		nEnableSysBanker = TRUE;

		lFreeTime = 5;
		lBetTime = 10;
		lEndTime = 20;

		lAreaLimitScore = 100000000;
		lUserLimitScore = 10000000;
		lEndGameMul = 80;

		CopyMemory(szMessageItem1,TEXT("�����Ǹ�������ɫ����"),sizeof(TEXT("�����Ǹ�������ɫ����")));
		CopyMemory(szMessageItem2,TEXT("������ӭ"),sizeof(TEXT("������ӭ")));
		CopyMemory(szMessageItem3,TEXT("��Ҿ�����Ϸ��"),sizeof(TEXT("��Ҿ�����Ϸ��")));

		lConfigSysStorage = 400000;
		lConfigPlayerStorage = 280000;
		lConfigParameterK = 20;

		superbankerConfig.superbankerType = SUPERBANKER_CONSUMETYPE;
		superbankerConfig.enVipIndex = VIP_INVALID;
		superbankerConfig.lSuperBankerConsume = 100000;

		occupyseatConfig.occupyseatType = OCCUPYSEAT_FREETYPE;
		occupyseatConfig.enVipIndex = VIP_INVALID;
		occupyseatConfig.lOccupySeatConsume = 0;
		occupyseatConfig.lOccupySeatFree = 1000000;
		occupyseatConfig.lForceStandUpCondition = 500000;

		LONGLONG lTempChipArray[MAX_CHIP_COUNT] = { 100, 1000, 10000, 50000, 100000, 1000000, 5000000 };
		CopyMemory(lChipArray, lTempChipArray, sizeof(lTempChipArray));
	}
};


struct tagCustomConfig
{
	tagCustomGeneral				CustomGeneral;					//ͨ�ù���
	tagCustomAndroid				CustomAndroid;					//��ϷAI

	//���캯��
	tagCustomConfig()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		CustomGeneral.DefaultCustomRule();
		CustomAndroid.DefaultCustomRule();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomGeneral : public CDialog
{
	//���ýṹ
public:
	tagCustomGeneral					m_CustomGeneral;						//�Զ�����
	int									m_iyoldpos;								//������λ��

	//��������
public:
	//���캯��
	CDlgCustomGeneral();
	//��������
	virtual ~CDlgCustomGeneral();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnClickSuperBankerVIPConfig();
	afx_msg void OnClickSuperBankerConsumeConfig();

	afx_msg void OnClickOccupySeatVIPConfig();
	afx_msg void OnClickOccupySeatConsumeConfig();
	afx_msg void OnClickOccupySeatFreeConfig();
};
////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomAndroid : public CDialog
{
	//���ýṹ
public:
	tagCustomAndroid					m_CustomAndroid;						//�Զ�����

	//��������
public:
	//���캯��
	CDlgCustomAndroid();
	//��������
	virtual ~CDlgCustomAndroid();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	afx_msg void OnBNClickEnableRobotBanker();
	
	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomRule : public CDialog
{
	//�ؼ�����
protected:
	CDlgCustomGeneral				m_DlgCustomGeneral;					//ͨ������
	CDlgCustomAndroid				m_DlgCustomAndroid;					//��ϷAI����

	//���ñ���
protected:
	WORD							m_wCustomSize;						//���ô�С
	LPBYTE							m_pcbCustomRule;					//���û���

	//���ýṹ
protected:
	tagCustomConfig					m_CustomConfig;						//�Զ�����

	//��������
public:
	//���캯��
	CDlgCustomRule();
	//��������
	virtual ~CDlgCustomRule();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();
	//��������
	BOOL UpdateConfigData(BOOL bSaveAndValidate);

	//���ܺ���
public:
	//��������
	bool SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//��������
	bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//Ĭ������
	bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//������ϷAI����
	bool HideAndroidModule(bool bHide);

	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR,LRESULT *pResult);
	bool CheckDataMinMax(LONGLONG valueMax);

	VIPINDEX GetVipIndex(WORD wSelIndex);

	int GetCurSel(VIPINDEX vipIndex);

	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////
#endif