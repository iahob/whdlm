#ifndef AI_MANAGER_HEAD_FILE
#define AI_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"



//////////////////////////////////////////////////////////////////////////////////


//һ�����͵�AI
typedef CMap<DWORD, DWORD, tagAIUserInfo *, tagAIUserInfo *> CKindAIUser;

//һ�����ֲ��е�����
typedef CMap<WORD, WORD, CKindAIUser *, CKindAIUser *> CGroupKindAI;

//���ֲ����
typedef CMap<DWORD, DWORD, CGroupKindAI *, CGroupKindAI *> CGroupAIUser;

//��Ϸ���Ͳ���
typedef CMap<WORD, WORD, tagGroupAIParameter * , tagGroupAIParameter * > CKindAIParameter;

//���ֲ�����
typedef CMap<DWORD, DWORD, CKindAIParameter *, CKindAIParameter * > CGroupAIParameter;

//���ֲ�����
typedef CMap<DWORD, DWORD, int, int> CGroupTable;

//���ֲ��淨����
typedef CWHArray<tagGroupConfigInfo> CArrayGroupGameRuleTable;

//��Ҫ���߻��˳����ֲ������
typedef CWHArray<tagDeleteGroupUser> CArrayDeleteGroupUser;



//////////////////////////////////////////////////////////////////////////////////
//��������
struct tagServerItem
{
	DWORD							m_dwSocketID;							//������
	WORD							m_wUserTable;						//�Ѿ�ʹ�õ���������
	CGroupTable						m_GroupTable;						//���ֲ�����
	tagGameServer					m_GameServer;						//��Ϸ����
	tagServerItem()
	{
		m_wUserTable = 0;
	}
};

typedef CMap<WORD, WORD, tagServerItem *, tagServerItem *> CMapServerID;

//////////////////////////////////////////////////////////////////////////////////

//AI����
class CAIManager
{
	//��������
public:
	//���캯��
	CAIManager();
	//��������
	virtual ~CAIManager();

//Ȩ������
public:
	TimeChange		m_TimeChange;		//AI�����仯
	WeightInOption	m_WeightInOption;	//������ϷȨ������
	WeightOutOption m_WeightOutOption;	//�뿪������Ȩ������
	DWORD			m_dwGroupSocketID;	//���������socket

public:
	CGroupAIParameter	m_mapGroupAIParameter;			//���ֲ�����
	CGroupAIUser		m_mapGroupAIUser;				//���ֲ�AI���
	CMapServerID		m_mapServerID;					//��Ϸ���伯��
	CGroupTable			m_mapGroupTable;				//���ֲ�����ͳ��
	CArrayGroupGameRuleTable m_ArrayGroupGameRuleTable; //�淨�����б�
	CArrayDeleteGroupUser m_ArrayDeleteGroupUser;		//ɾ�����ֲ����

	IDBCorrespondManager *	m_pIDBCorrespondManager;	//��������
	ITCPNetworkEngine *		m_pITCPNetworkEngine;		//��������

//����Ȩ��
public:
	//�����뿪������Ȩ������
	void SetWeightOutOption(WeightOutOption wghtOutOption);

	//���ý�����ϷȨ������
	void SetWeightInOption(WeightInOption wghtInOption);

	//�����뿪������Ȩ������
	void SetWeightOutOption(int nIndex, tagWeightOutRatio WeightOutRatio);

	//���ý�����ϷȨ������
	void SetWeightInOption(int nIndex, tagWeightInRatio WeightInRatio);


	//����AI����ʱ��任
	bool SetAITimeChange(TimeChange * pTimeChange);

	//����AI����ʱ��任
	bool SetAITimeChange(int nIndex, int Weight);

public:
	//������ֲ�����
	byte InsertGroupParameter(tagGroupAIParameter GroupAIParameter);

	//��ȡAI����
	tagGroupAIParameter * GetGroupParameter(DWORD dwGroupID, WORD wKindID);

	//�������ݿ�����
	void SetDBCorrespondManager(IDBCorrespondManager *	pIDBCorrespondManager, ITCPNetworkEngine *	pITCPNetworkEngine);

	//���ͼ���һ��������ϷAI
	void SendLoadOneKindAIAction(DWORD dwGroupID, WORD wKindID);

	//���ͼ�������AI
	void SendLoadAllAIAction();

	//���ͼ�������AI
	void InsertAIUser(DWORD dwGroupID, WORD wKindID, tagAIUserInfo * pAIUserInfo);

	//��AI����
	void SendAIToGameServer(CMD_SS_G_PersonalRoomCreateSuccess * pPersonalRoomCreateSuccess, DWORD dwSocketID);

	//��һ��AI����
	void SendOneAIToGameServer(tagAISitFailure * pAISitFailure, DWORD dwSocketID);

	//���ͼ�������AI
	void SendAIToGroupServer(DWORD dwGroupID, WORD wKindID, DWORD dwSocketID);

	//���뷿��
	bool InsertServerItem(DWORD dwSocketID, tagGameServer & GameServer);

	//����AI״̬
	bool UpdateAIStatusInfo(tagAIStatusInfo * pAIStatusInfo);

	//����AI����
	bool UpdateAIScore(tagAIStatusInfo * pAIStatusInfo);

	//��ȡ����Ȩ��
	DWORD GetAIWeightIn(DWORD dwTimeCount);

	//��ȡ�뿪Ȩ��
	DWORD GetAIWeightOut(DWORD dwTimeCount);

	//��ȡ��ǰʱ���AI��������
	DWORD GetTimeAICount(DWORD dwTimeCount);

	//ʱ�����
	DWORD GetAIWaitTimeCount(time_t tTime);

	//AI ������뿪
	DWORD AIInOrOut(DWORD dwGroupSocketID);

	//ͳ�ƴ����˵�������
	DWORD GetAICount();

	//��������
	DWORD CreateRoom(WORD wServerID, WORD wPlayMode, CMD_GR_AICreateTable CreateTable);

	//��ȡ����AI
	tagAIUserInfo * GetSingleAICount(DWORD dwGroupID, WORD wKindID);

	//��ȡ����AI
	bool DeleteServer(DWORD dwSocketID);

	//������������
	bool UpdateAITableChange(CMD_SS_G_AI_Update_Table_Count * pTableChange);

	//ͳ�Ƶ�ǰ���ֲ�ʹ�����ӵ�����
	bool GroupCreateRoomCount(DWORD dwGroupID, WORD wKindID);

	// �����淨
	bool InsertGroupGameRule(tagGroupConfigInfo GroupConfigInfo);

	// ɾ���淨
	bool DeleteGroupGameRule(tagGroupConfigInfo GroupConfigInfo);

	// �޸��淨
	bool ModifyGroupGameRule(tagGroupConfigInfo GroupConfigInfo);

	//��������Ƿ�Ϊ��
	bool CheckGroupRoomIsNull();

	//��������뿪��Ϊ
	bool SetUserLeave(CMD_CS_C_DeleteAI DeleteAI);

	//������뿪
	bool LetUserLeave();

	//�������״̬
	bool ResetUserStatus(WORD wServerID);

};

//////////////////////////////////////////////////////////////////////////////////

#endif