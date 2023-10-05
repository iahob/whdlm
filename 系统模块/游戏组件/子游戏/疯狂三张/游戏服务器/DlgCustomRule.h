#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////////////

//���ýṹ
struct tagCustomRule
{
	SCORE									lRoomStorageStart;			//�����ʼֵ
	SCORE									lRoomStorageDeduct;			//˥��ֵ
	SCORE									lRoomStorageMax1;			//���ⶥֵ1
	SCORE									lRoomStorageMul1;			//Ӯ�ְٷֱ�1
	SCORE									lRoomStorageMax2;			//���ⶥֵ1
	SCORE									lRoomStorageMul2;			//Ӯ�ְٷֱ�1

	//AI���ȡ��
	SCORE									lRobotScoreMin;	
	SCORE									lRobotScoreMax;
	SCORE	                                lRobotBankGet; 
	SCORE									lRobotBankGetBanker; 
	SCORE									lRobotBankStoMul; 

	int										nButtonValue1[4];
	int										nButtonValue2[4];
	int										nButtonValue3[4];

	int										nMinAllinTurnCount;         //��Сȫѹ����
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
	bool FillDataToDebug();
	//��������
	bool FillDebugToData();

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