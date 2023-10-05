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

	//�����˴��ȡ��
	SCORE									lRobotScoreMin;
	SCORE									lRobotScoreMax;
	SCORE	                                lRobotBankGet;
	SCORE									lRobotBankGetBanker;
	SCORE									lRobotBankStoMul;

	//ʱ�䶨��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeCallBanker;					//��ׯʱ��
	BYTE							cbTimeSetChip;						//��עʱ��
	BYTE							cbTimeRangeCard;					//����ʱ��
	BYTE							cbTimeShowCard;						//����ʱ��

	bool							bHaveBanker;						//����ׯ
	BYTE							cbPlayerCount;						//�������
	BYTE							cbMaCard;							//����
	BYTE							cbTongHuaMode;						//0ͬ���ȱȵ�����1ͬ���ȱȻ�ɫ
	BYTE							cbQuickMode;						//���ٱ���0��ͨ��1����

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