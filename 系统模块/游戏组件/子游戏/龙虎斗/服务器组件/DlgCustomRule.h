#pragma once
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
    LONGLONG						cbFreeTime;						//����ʱ��
    LONGLONG						cbBetTime;						//��עʱ��
    LONGLONG						cbEndTime;						//����ʱ��

    //��ע����
    LONGLONG						lAreaLimitScore;				//��������
    LONGLONG						lUserLimitScore;				//��������

    //��Ϣ
    TCHAR							szMessageItem1[64];				//��Ϣ1
    TCHAR							szMessageItem2[64];				//��Ϣ2
    TCHAR							szMessageItem3[64];				//��Ϣ3

    SCORE							lChipArray[MAX_CHIP_COUNT];		//��������
    LONGLONG						lOccupySeatFree;				//���ռλ�������

    SCORE							lSysCtrlSysStorage;				//ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lSysCtrlPlayerStorage;			//��ҿ��(���ҪӮ��Ǯ)
    SCORE							lSysCtrlParameterK;				//����ϵ��(�ٷֱ�):
    SCORE							lConfigAnChouRate;				//�������
    SCORE							lSysCtrlSysStorResetRate;		//ϵͳ������ñ���

    SCORE							lConfigMosgoldRate;				//�󽱳�ȡ����
    SCORE							lConfigMosgoldStorage;			//�󽱿��
    SCORE							lConfigMosgoldDispatchCon;		//�ɽ����
    SCORE							lConfigMosgoldDispatchRate;		//�ɽ�����
    SCORE							lConfigMosgoldStorageRate;		//�󽱿�����

    //���캯��
    tagCustomGeneral()
    {
        PrimaryDefaultCustomRule();
    }

    //��������
    void PrimaryDefaultCustomRule()
    {
        lApplyBankerCondition = 1000;
        lBankerTime = 10;
        lBankerTimeAdd = 10;
        lBankerScoreMAX = 10000;
        lBankerTimeExtra = 10;
        nEnableSysBanker = TRUE;

        cbFreeTime = 5;
        cbBetTime = 15;
        cbEndTime = 10;

        lAreaLimitScore = 10000;
        lUserLimitScore = 8000;

        CopyMemory(szMessageItem1, TEXT("�����Ǹ�������ɫ����"), sizeof(TEXT("�����Ǹ�������ɫ����")));
        CopyMemory(szMessageItem2, TEXT("������ӭ"), sizeof(TEXT("������ӭ")));
        CopyMemory(szMessageItem3, TEXT("��Ҿ�����Ϸ��"), sizeof(TEXT("��Ҿ�����Ϸ��")));


        SCORE lTempChipArray[MAX_CHIP_COUNT] = {1, 5, 10, 50, 100, 500};
        CopyMemory(lChipArray, lTempChipArray, sizeof(lTempChipArray));

        lOccupySeatFree = 100000;

        lSysCtrlSysStorage = 200000;
        lSysCtrlPlayerStorage = 250000;
        lSysCtrlParameterK = 10;
        lConfigAnChouRate = 2;
        lSysCtrlSysStorResetRate = 5;

        lConfigMosgoldRate = 3;
        lConfigMosgoldStorage = 0;
        lConfigMosgoldDispatchCon = 0;
        lConfigMosgoldDispatchRate = 8;
        lConfigMosgoldStorageRate = 20;
    }

    //�м�����
    void MidDefaultCustomRule()
    {
        lApplyBankerCondition = 10000;
        lBankerTime = 10;
        lBankerTimeAdd = 10;
        lBankerScoreMAX = 100000;
        lBankerTimeExtra = 10;
        nEnableSysBanker = TRUE;

        cbFreeTime = 5;
        cbBetTime = 15;
        cbEndTime = 10;

        lAreaLimitScore = 100000;
        lUserLimitScore = 80000;

        CopyMemory(szMessageItem1, TEXT("�����Ǹ�������ɫ����"), sizeof(TEXT("�����Ǹ�������ɫ����")));
        CopyMemory(szMessageItem2, TEXT("������ӭ"), sizeof(TEXT("������ӭ")));
        CopyMemory(szMessageItem3, TEXT("��Ҿ�����Ϸ��"), sizeof(TEXT("��Ҿ�����Ϸ��")));


        SCORE lTempChipArray[MAX_CHIP_COUNT] = {10, 50, 100, 500, 1000, 5000};
        CopyMemory(lChipArray, lTempChipArray, sizeof(lTempChipArray));

        lOccupySeatFree = 1000000;

        lSysCtrlSysStorage = 2000000;
        lSysCtrlPlayerStorage = 2500000;
        lSysCtrlParameterK = 10;
        lConfigAnChouRate = 2;
        lSysCtrlSysStorResetRate = 5;

        lConfigMosgoldRate = 3;
        lConfigMosgoldStorage = 0;
        lConfigMosgoldDispatchCon = 0;
        lConfigMosgoldDispatchRate = 8;
        lConfigMosgoldStorageRate = 20;
    }

    //�߼�����
    void SeniorDefaultCustomRule()
    {
        lApplyBankerCondition = 100000;
        lBankerTime = 10;
        lBankerTimeAdd = 10;
        lBankerScoreMAX = 1000000;
        lBankerTimeExtra = 10;
        nEnableSysBanker = TRUE;

        cbFreeTime = 5;
        cbBetTime = 15;
        cbEndTime = 10;

        lAreaLimitScore = 1000000;
        lUserLimitScore = 800000;

        CopyMemory(szMessageItem1, TEXT("�����Ǹ�������ɫ����"), sizeof(TEXT("�����Ǹ�������ɫ����")));
        CopyMemory(szMessageItem2, TEXT("������ӭ"), sizeof(TEXT("������ӭ")));
        CopyMemory(szMessageItem3, TEXT("��Ҿ�����Ϸ��"), sizeof(TEXT("��Ҿ�����Ϸ��")));


        SCORE lTempChipArray[MAX_CHIP_COUNT] = {100, 500, 1000, 5000, 10000, 50000};
        CopyMemory(lChipArray, lTempChipArray, sizeof(lTempChipArray));

        lOccupySeatFree = 10000000;

        lSysCtrlSysStorage = 20000000;
        lSysCtrlPlayerStorage = 25000000;
        lSysCtrlParameterK = 10;
        lConfigAnChouRate = 2;
        lSysCtrlSysStorResetRate = 5;

        lConfigMosgoldRate = 3;
        lConfigMosgoldStorage = 0;
        lConfigMosgoldDispatchCon = 0;
        lConfigMosgoldDispatchRate = 8;
        lConfigMosgoldStorageRate = 20;
    }
};

struct tagCustomConfig
{
    bool							bEnableBanker;					//��ׯ����
    tagCustomGeneral				CustomGeneral;					//ͨ�ù���
    tagCustomAndroid				CustomAndroid;					//������

    //���캯��
    tagCustomConfig()
    {
        PrimaryDefaultCustomRule();
    }

    void PrimaryDefaultCustomRule()
    {
        bEnableBanker = true;
        CustomGeneral.PrimaryDefaultCustomRule();
        CustomAndroid.PrimaryDefaultCustomRule();
    }

    void MidDefaultCustomRule()
    {
        bEnableBanker = true;
        CustomGeneral.MidDefaultCustomRule();
        CustomAndroid.MidDefaultCustomRule();
    }

    void SeniorDefaultCustomRule()
    {
        bEnableBanker = true;
        CustomGeneral.SeniorDefaultCustomRule();
        CustomAndroid.SeniorDefaultCustomRule();
    }
};

class CDlgCustomRule;
////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomGeneral : public CDialog
{
    //���ýṹ
public:
    tagCustomGeneral					m_CustomGeneral;						//�Զ�����
    CDlgCustomRule*						m_pDlgCustomRule;

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

public:
    //������ׯ���
    void EnableBankerFun(BOOL bEnable);
    //��ׯ����
    void CheckBankerCtrl(BOOL bEnable);
    //�Զ���ص�
    void SetDlgCustomRuleSink(CDlgCustomRule* pDialog) { m_pDlgCustomRule = pDialog; }
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedCheckCombebanker();
  
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

public:
    //������ׯ���
    void EnableBankerFun(BOOL bEnable);
  
};

////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomRule : public CDialog
{
    //�ؼ�����
public:
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
    bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize, EM_DEFAULT_CUSTOM_RULE emCurRule);
    //ɾ������������ѡ��
    void DeleteAndroid();
    //������ׯ���
    void EnableBankerFun(BOOL bEnable);
    afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);

    DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////