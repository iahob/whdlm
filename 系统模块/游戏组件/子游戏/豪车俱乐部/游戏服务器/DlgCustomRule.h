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
	LONGLONG							cbFreeTime;						//����ʱ��
	LONGLONG							cbBetTime;						//��עʱ��
	LONGLONG							cbEndTime;						//����ʱ��

	//��ע����
	LONGLONG						lAreaLimitScore;				//��������
	LONGLONG						lUserLimitScore;				//��������
	//���
	LONGLONG						StorageStart;					//��ʼ���
	LONGLONG						StorageDeduct;					//���˥��
	LONGLONG						StorageMax1;					//���ⶥ1
	LONGLONG						StorageMul1;					//���ⶥ�����Ӯ�ָ���
	LONGLONG						StorageMax2;					//���ⶥ2
	LONGLONG						StorageMul2;					//���ⶥ�����Ӯ�ָ���

	LONGLONG						BetScore[7];

	//���캯��
	tagCustomGeneral()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		lApplyBankerCondition = 50000;
		lBankerTime = 10;
		lBankerTimeAdd = 10;
		lBankerScoreMAX = 150000;
		lBankerTimeExtra = 10;
		nEnableSysBanker = TRUE;

		cbFreeTime = 3;
		cbBetTime = 10;
		cbEndTime = 20;

		lAreaLimitScore = 500000;
		lUserLimitScore = 100000;

		StorageStart = 500000;
		StorageDeduct = 0;
		StorageMax1 = 1500000;
		StorageMul1 = 50;
		StorageMax2 = 3000000;
		StorageMul2 = 80;

		BetScore[0] = 10;
		BetScore[1] = 50;
		BetScore[2] = 100;
		BetScore[3] = 500;
		BetScore[4] = 1000;
		BetScore[5] = 5000;
		BetScore[6] = 10000;

	}
};


struct tagCustomConfig
{
	tagCustomGeneral				CustomGeneral;					//ͨ�ù���
	tagCustomAndroid				CustomAndroid;					//������

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
	
	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomRule : public CDialog
{
	//�ؼ�����
protected:
	CDlgCustomGeneral				m_DlgCustomGeneral;					//ͨ������
	CDlgCustomAndroid				m_DlgCustomAndroid;					//��������

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
	bool CheckDataMinMax(LONGLONG valueMax);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR,LRESULT *pResult);
	//���������������ʱ ɾ������������ѡ��
	void DeleteAndroid();
	//��������
	bool CheckData();
	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////
#endif