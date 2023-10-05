//�ر���IT������28xin.com��
#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

//���ݿ⹳��
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;
	//�洢����
protected:
	BYTE							m_cbBuffer[GP_SOCKET_PACKET];			//��������

	//�������
protected:
	CDataBaseAide					m_IMGroupDBAide;					//ƽ̨���ݿ�	
	CDataBaseHelper					m_IMGroupDBModule;					//ƽ̨���ݿ�

	//�������
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//ϵͳ����
protected:
	//�û���¼
	bool OnRequestLogonServer(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnRequestCreateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnRequestUpdateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ��Ⱥ��
	bool OnRequestDeleteGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ӳ�Ա
	bool OnRequestInsertMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ����Ա
	bool OnRequestDeleteMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestApplyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ӧ
	bool OnRequestApplyRespond(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ɾ��
	bool OnRequestApplyDelete(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Լս����
	bool OnRequestSettleBattle(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestLoadGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestLoadForbidWords(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯԼս
	bool OnRequestQueryBattle(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Լս����
	bool OnRequestQueryTotalBattle(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//����淨
	bool OnRequestAppendConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸��淨
	bool OnRequestModifyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ���淨
	bool OnRequestDeleteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�Ȩ��
	bool OnRequestModifyRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ͷ���
	bool OnRequestPresentScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ���ͼ�¼
	bool OnRequestQueryPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�۷Ѽ�¼
	bool OnRequestQueryRevenue(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�ռ�¼
	bool OnRequestQueryDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ���������
	bool OnRequestQueryDayDetail(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ������������
	bool OnRequestGroupDayDetail(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ��������
	bool OnRequestQueryCreate(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯѫ��ģʽ
	bool OnRequestQueryMedalMode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����Ⱥ��״̬
	bool OnRequestSetGroupStatus(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestSetInOut(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���������
	bool OnRequestSetRollMSg(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ȩ�޹���
	bool OnRequestRightManagement(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ȩ�޲�ѯ
	bool OnRequestRightQuery(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ѫ������
	bool OnRequestSetMedal(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestSetFreeTableLimit(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ӯ������
	bool OnRequestSetBigWinner(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestSetFlag(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ѫ��
	bool OnRequestClearMedal(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ղ��淨
	bool OnRequestSetFavoriteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�����
	bool OnRequestModifyGroupName(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����Ծ�����
	bool OnRequestRoomDetail(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�ӳ��б�
	bool OnRequestCaptainList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�ӳ��б�
	bool OnRequestAddTeamer(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯС����ͳ��
	bool OnRequestTeamDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯС�ӳ�Ա��ͳ��
	bool OnRequestTeamerDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��˽����
	bool OnRequestSetPrivacy(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��λ����
	bool OnRequestSetCaptainPoint(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�򿪱�����
	bool OnRequestOpenBox(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ȡ����
	bool OnRequestGetAwardScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ������
	bool OnRequestInvitationCode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestBlindCode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestCheckNewUser(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ���÷�
	bool OnRequestQueryScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���¾��ֲ�����״̬
	bool OnRequestUpdateOnlineStatus(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ص������ֲ���Ϣ
	bool OnRequestLoadSingleGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸���Ϣ
	bool OnRequestModifyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ�鹦��
protected:
	//���³�Ա
	bool OnRequestUpdateMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ���б�
	bool OnRequestLoadGroupList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ������
	bool OnRequestLoadGroupItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ���Ա
	bool OnRequestLoadGroupMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ⱥ���淨
	bool OnRequestLoadGroupConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
protected:		
	//��ȡȺ��
	WORD ExtractGroupID(LPCTSTR pszGroupIDArray, CDWordArray & GroupIDArray);
	//����쳣
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif