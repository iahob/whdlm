#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ýṹ
struct tagCustomRule
{
	//ͨ������
	int					nScoreRatio;			//��������
	BYTE				cbPlayTime;				//����ʱ��
	BYTE				cbBagCount;				//�������
	LONGLONG			lScoreRangeMin;			//�����С
	LONGLONG			lScoreRangeMax;			//������
	DOUBLE				dBagTimes;				//�⸶�������һλС��
	BYTE				cbThunderPosition;		//�׺�λ�ã�0С�����һλ��1С����ڶ�λ

	//AI����
	BYTE				cbMinBagListCount;		//�����б������������ֵʱ��ʼ���ף�
	BYTE				cbMaxRealBagListCount;	//�����б����������������������ֵʱ�����ף�
	BYTE				cbSetBagCount[2];		//���׸���
	BYTE				cbSetBagTime[2];		//���׼��
	LONGLONG			lSetBagScore[2];		//���׺����С
	BYTE				cbStartGetBagTime;		//�������ʼʱ��
	BYTE				cbGetBagTime[2];		//��������
	BYTE				cbMaxGetBagCount;		//������������
	BYTE				cbLastNoGetBagCount;	//������������

	//�������
	LONGLONG			lSystemStorage;			//ϵͳ���
	LONGLONG			lUserStorage;			//��ҿ��
	int					nParameterK;			//��ǰ����K(�ٷֱ�)

};

//////////////////////////////////////////////////////////////////////////////////

//���ô���
class CDlgCustomRule : public CDialog
{
	//���ñ���
protected:
	tagCustomRule					m_CustomRule;						//���ýṹ

	//��������
public:
	//���캯��
	CDlgCustomRule();
	//��������
	virtual ~CDlgCustomRule();

	//���غ���
protected:
	//���ú���
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//���ܺ���
public:
	//���¿ؼ�
	bool FillDataToControl();
	//��������
	bool FillControlToData();
	//�����н���
	WORD CalcWinChance(LONGLONG lSystemStorage, LONGLONG lUserStorage, LONGLONG nParameterK);

	//��Ϣ����
public:
	//ϵͳ���仯
	afx_msg void OnEnChangeEditSystemStorage();
	//��ҿ��仯
	afx_msg void OnEnChangeEditUserStorage();
	//����ϵ���仯
	afx_msg void OnEnChangeEditParameterK();


	//���ú���
public:
	//��ȡ����
	bool GetCustomRule(tagCustomRule & CustomRule);
	//��������
	bool SetCustomRule(tagCustomRule & CustomRule);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif