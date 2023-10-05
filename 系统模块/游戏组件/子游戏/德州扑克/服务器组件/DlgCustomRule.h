#pragma once

#pragma warning(disable : 4244)
////////////////////////////////////////////////////////////////////////////////////////////

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

	SCORE									lCellSclore;			//äע����
	SCORE									lMinBetScore;			//���Я�����
	SCORE									lMaxBetScore;			//���Я�����

	BYTE									cbStartTime;			//��ʱʱ��
	BYTE									cbBetTime;				//��עʱ��
	BYTE									cbEndTime;				//����ʱ��
};

class CDlgCustomRule : public CDialog
{
	DECLARE_MESSAGE_MAP()
	//��������
public:
	//���캯��
	CDlgCustomRule();
	//��������
	virtual ~CDlgCustomRule();

	//���غ���
protected:
	//��ʼ������
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

	//���ñ���
protected:
	tagCustomRule					m_CustomRule;						//���ýṹ
};
////////////////////////////////////////////////////////////////////////////////////////////