#ifndef GAME_DLG_PERSONAL_RULE_HEAD_FILE
#define GAME_DLG_PERSONAL_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//���ýṹ
struct tagOxSixXSpecial
{
	tagPersonalRule					comPersonalRule;

	BYTE							cbBankerModeEnable[MAX_BANKERMODE];	//6����ׯģʽ		TRUE�򼤻FALSE�򲻼���
	BYTE							cbGameModeEnable[MAX_GAMEMODE];		//��Ϸģʽ			TRUE�򼤻FALSE�򲻼���
	DWORD							cbBeBankerCon[MAX_BEBANKERCON];		//��ׯ���� INVALID_DWORD�򲻼��0����
	DWORD							cbUserbetTimes[MAX_USERBETTIMES];	//�м���ע���� INVALID_DWORD�򲻼��0����
	BYTE							cbAdvancedConfig[MAX_ADVANCECONFIG];//�߼�����			TRUE�򼤻FALSE�򲻼���
	BYTE							cbSpeCardType[MAX_SPECIAL_CARD_TYPE];//���ͼ��		TRUE�򼤻FALSE�򲻼���
	
	tagOxSixXSpecial()
	{
		ZeroMemory(&comPersonalRule, sizeof(comPersonalRule));

		ZeroMemory(cbBankerModeEnable, sizeof(cbBankerModeEnable));
		ZeroMemory(cbGameModeEnable, sizeof(cbGameModeEnable));
		ZeroMemory(cbBeBankerCon, sizeof(cbBeBankerCon));
		ZeroMemory(cbUserbetTimes, sizeof(cbUserbetTimes));
		ZeroMemory(cbAdvancedConfig, sizeof(cbAdvancedConfig));
		ZeroMemory(cbSpeCardType, sizeof(cbSpeCardType));
	}
};

//////////////////////////////////////////////////////////////////////////////////

//���ô���
class CDlgPersonalRule : public CDialog
{
	//���ñ���
protected:
	tagOxSixXSpecial					m_OxSixXSpecialRule;						//���ýṹ
	int									m_iyoldpos;									//������λ��

	//��������
public:
	//���캯��
	CDlgPersonalRule();
	//��������
	virtual ~CDlgPersonalRule();

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
	bool GetPersonalRule(tagOxSixXSpecial & OxSixXSpecialRule);
	//��������
	bool SetPersonalRule(tagOxSixXSpecial & OxSixXSpecialRule);

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
#pragma pack(pop)
//////////////////////////////////////////////////////////////////////////////////

#endif