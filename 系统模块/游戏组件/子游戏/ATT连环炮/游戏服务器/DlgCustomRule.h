#pragma once


////////////////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4244)


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