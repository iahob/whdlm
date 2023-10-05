#pragma once


////////////////////////////////////////////////////////////////////////////////////////////

//���ýṹ
struct tagCustomRule
{
	SCORE									lStorageStartTable;			//���ӿ����ʼֵ
	SCORE									lStorageDeductRoom;			//ȫ��˥��ֵ
	SCORE									lStorageMax1Room;			//ȫ�ֿ��ⶥֵ1
	SCORE									lStorageMul1Room;			//ȫ��Ӯ�ְٷֱ�1
	SCORE									lStorageMax2Room;			//ȫ�ֿ��ⶥֵ2
	SCORE									lStorageMul2Room;			//ȫ��Ӯ�ְٷֱ�2
	WORD									wGameTwo;					//�ȱ�����
	WORD									wGameTwoDeduct;				//�ȱ�����
	WORD									wGameThree;					//С��������
	WORD									wGameThreeDeduct;			//С��������
	BYTE									cbBetCount;					//��ע����
	SCORE									lBetScore[8];				//��ע��С
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
	bool FillDataToControl();
	//��������
	bool FillControlToData();

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