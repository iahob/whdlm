#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "ServerUserManager.h"
#include "ServerListManager.h"
#include "GroupInfoManager.h"
//#include "ClubServerDataInfo.h"
#include "WordFilter.h"
//////////////////////////////////////////////////////////////////////////

//�󶨲���
struct tagBindParameter
{	
	//������Ϣ	
	DWORD							dwSocketID;							//�����ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//������Ϣ
	WORD							wGroupID;							//Ⱥ���ʶ
	WORD							wLogicID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHArray<tagIMGroupOption>		CGroupOptionArray;			//Ⱥ������
//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;
	//�洢����
protected:
	BYTE							m_cbBuffer[GP_SOCKET_PACKET];			//��������

	//״̬����
protected:
	bool							m_bNeedBattle;						//��֤Լս
	time_t							m_tMedalClearStart;
	//���ñ���
protected:	
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//������Ϣ
	tagServiceItem					m_BattleItem;						//Լս����	
	bool							m_bNeedConnectBattle;				//����Լս
	//�������
protected:
	CServerUserManager				m_ServerUserManager;				//�û�����	
	CServerListManager				m_ServerListManager;				//�б����
	CGroupInfoManager				m_GroupInfoManager;					//Ⱥ�����	
	CWordFilter						m_WordFilter;						//�ִʹ���
	CGroupOptionArray				m_GroupOptionArray;					//���ù���
	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	ITCPSocketService *				m_TCPSocketCorrespond;				//Э������
	ITCPSocketService *				m_pITCPSocketBattle;				//Լս����	
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	ITCPSocketService *				m_pLogServerTCPSocketService;		//��־��������
	ITCPSocketService *				m_pAITCPSocketService;				//AI��������

	TCHAR							m_PrsAdd[32];
	WORD							m_wPrsPort;
	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release(){	return;	}
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//�Զ��¼�
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//Э������
protected:
	//ע�����
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�б��¼�
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);		
	//ת���¼�
	bool OnTCPSocketClubInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��վ�¼�
	bool OnTCPSocketWebInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//Լս����
protected:
	//ע�����
	bool OnTCPSocketBattleMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�б��¼�
	bool OnTCPSocketBattleMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//Լս����
	bool OnTCPSocketBattleMainBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//AI����
	bool OnTCPSocketAIMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����¼�
protected:	
	//�߼�����
	bool OnTCPNetworkMainLogicService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//Ⱥ�����
	bool OnTCPNetworkMainGroupService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//Լս����
	bool OnTCPNetworkMainBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����¼�
protected:
	//��¼�ɹ�
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnDBCreateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnDBUpdateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ��Ⱥ��
	bool OnDBDeleteGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ӳ�Ա
	bool OnDBInsertMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ����Ա
	bool OnDBDeleteMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBApplyMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ������
	bool OnDBGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ֹ�ʻ�
	bool OnDBForbidWords(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�Ƹ�����
	bool OnDBWealthUpdate(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ�����
	bool OnDBApplyDelResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����ɹ�
	bool OnDBOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBLoadGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����¼�
protected:
	//Ⱥ���б�
	bool OnDBGroupList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�б����
	bool OnDBGroupListFinish(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBGroupCommonInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ������
	bool OnDBGroupItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBGroupItemFinish(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ���Ա
	bool OnDBGroupMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���³�Ա
	bool OnDBUpdateMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ���淨
	bool OnDBGroupConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Լս���
	bool OnDBBattleResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�Ȩ��
	bool OnDBModifyRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ͳɹ�
	bool OnDBPresentSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ͼ�¼
	bool OnDBPresentList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�۷Ѽ�¼
	bool OnDBRevenueList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����淨
	bool OnDBAppendConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸��淨
	bool OnDBModifyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ���淨
	bool OnDBDeleteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ռ�¼
	bool OnDBDayRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������
	bool OnDBUserDayDetailList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������
	bool OnDBGroupDayDetailList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������ѯ
	bool OnDBQueryCreateResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ѫ��ģʽ
	bool OnDBMedalMode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ��״̬
	bool OnDBGroupStatus(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������ý��
	bool OnDBInOutMode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����ԱȨ��
	bool OnDBQueryManagerRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBFreeTableLImit(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ӯ������
	bool OnDBBigWinnerShow(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ù���ԱȨ��
	bool OnDBSetManagerRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBSetFlag(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ѫ������
	bool OnDBClearMedal(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ղ��淨
	bool OnDBFavoriteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����
	bool OnDBRollMsg(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����������
	bool OnDBGroupName(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBRoomDetailInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ӳ��б�
	bool OnDBCaptainList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����Ա���
	bool OnDBAddTeamer(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//С����ͳ��
	bool OnDBTeamDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//С�ӳ�Ա��ͳ��
	bool OnDBTeamerDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������˽
	bool OnDBGroupPrivacy(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ӳ���λ
	bool OnDBCaptainPoint(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������Ϣ
	bool OnDBBoxInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������ȡ���
	bool OnDBGetAwardScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������Ϣ
	bool OnDBInvitationCode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�󶨳ɹ�
	bool OnDBBlindCode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�������
	bool OnDBQueryScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸���Ϣ
	bool OnDBModifyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ݷ���
protected:
	//Լս����
	bool SendDataToBattle(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//Ⱥ�鷢��
	bool SendDataToGroupMember(DWORD dwGroupID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, bool bVerifyGroupID = true);
	//Ⱥ�����Ա����
	bool SendDataToGroupManager(DWORD dwGroupID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//Ⱥ���Ա����
	bool SendDataToGroupTeamer(DWORD dwGroupID,DWORD dwCaptainID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//���ͳ�Ա
	bool SendGroupMemberToUser(DWORD dwGroupID, DWORD dwSocketID);
	//�����淨
	bool SendGroupConfigToUser(DWORD dwGroupID, DWORD dwSocketID);
	//���¸�����Ϣ
	bool UpdateGroupExtraInfo(DWORD dwGroupID, DWORD dwMemberID);
	//������־
	void SendLogData(const TCHAR * pszFormat, ...);
	//
	//��������
protected:	
	//��������
	DWORD BuildTokenID();	
	//�󶨲���
	tagBindParameter * GetBindParameter(WORD wBindIndex);
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	//�û���¼
	bool OnEventUserLogon(CLocalUserItem * pServerUserItem);
	//�����˳�ԱУ��
	bool CheckClubUser(DWORD dwGroupID, DWORD dwUserID);
	//�ַ��ж�
	int CheckClubName(LPCWSTR pszGroupName);
};

//////////////////////////////////////////////////////////////////////////

#endif

