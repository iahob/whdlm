#ifndef PLATFORM_FRAME_HEAD_FILE
#define PLATFORM_FRAME_HEAD_FILE

#pragma once

//ϵͳ�ļ�
#include "Stdafx.h"
#include "Resource.h"
#include "ClientDebug.h"
#include "AIConfigDebug.h"

//�����ļ�
#include "MissionList.h"
#include "MissionLogon.h"
#include "MissionAndroid.h"
#include "ServerListData.h"
//////////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IPlatformFrame INTERFACE_VERSION(1,1)
static const GUID IID_IPlatformFrame = { 0x99c5d22b, 0xc1ee, 0x4af2, 0xb9, 0x7, 0xf2, 0x81, 0x53, 0xc6, 0x62, 0xd8 };
#else
#define VER_IPlatformFrame INTERFACE_VERSION(1,1)
static const GUID IID_IPlatformFrame = { 0xcb259add, 0x7322, 0x4380, 0x82, 0xbc, 0xef, 0x97, 0xd1, 0x4, 0xf4, 0x7a };
#endif

class CDlgServerManager;
//�����ӿ�
interface IPlatformFrame : public IUnknownEx
{
};

//����״̬
enum enServiceStatus
{
	ServiceStatus_Unknow,			//δ֪״̬
	ServiceStatus_Entering,			//����״̬
	ServiceStatus_Validate,			//��֤״̬
	ServiceStatus_RecvInfo,			//��ȡ״̬
	ServiceStatus_ServiceIng,		//����״̬
	ServiceStatus_NetworkDown,		//�ж�״̬
};
//ƽ̨���
class CPlatformFrame :public CSkinDialog, public IPlatformFrame, public ITCPSocketSink, public IServerListDataSink, IAndroidOperateCenter, public IUserManagerSink, public IChatControlSink, public IClientDebugCallback, public IAIConfigDebugCallback
{
	//��������
protected:
	bool							m_bLogonSuccess;					//��¼�ɹ�
	int								m_nSelectItem;						//ѡ������

	CClientUserItemArray			m_MenuUserItemArray;				//�û�����

	//��Դ����
protected:
	CBrush							m_brBackGround;						//������ˢ

	//��Ϸ�б�
protected:
	CServerListData					m_ServerListData;					//��Ϸ����

	//�ؼ�����
protected:
	CSkinComboBox					m_cbGameList;						//��Ϸ�б�
	CSkinComboBox					m_cbServerList;						//�����б�
	CSkinComboBox					m_cbEnterHour;						//����ʱ��
	CSkinComboBox					m_cbEnterMinute;					//����ʱ��
	CSkinComboBox					m_cbEnterSecond;					//����ʱ��
	CSkinComboBox					m_cbLeaveHour;						//�뿪ʱ��
	CSkinComboBox					m_cbLeaveMinute;					//�뿪ʱ��
	CSkinComboBox					m_cbLeaveSecond;					//�뿪ʱ��

	//�������
protected:
	CTCPSocketHelper				m_TCPSocketModule;					//��������	
	CPlazaUserManagerHelper			m_PlazaUserManagerModule;			//�û�����

	//�ؼ�����
protected:
	CSkinEdit						m_edtAndroidCount;					//������Ŀ
	CSkinEdit						m_edtTableScoreMin;					//Я������
	CSkinEdit						m_edtTableScoreMax;					//Я������
	CSkinEdit						m_edtEnterIntervalMin;				//������
	CSkinEdit						m_edtEnterIntervalMax;				//������
	CSkinEdit						m_edtLeaveIntervalMin;				//�뿪���
	CSkinEdit						m_edtLeaveIntervalMax;				//�뿪���
	CSkinEdit						m_edtSwitchTableMin;				//��������
	CSkinEdit						m_edtSwitchTableMax;				//��������
	CSkinEdit						m_edtCountMember0;					//��ͨ��Ա
	CSkinEdit						m_edtCountMember1;					//һ����Ա
	CSkinEdit						m_edtCountMember2;					//������Ա
	CSkinEdit						m_edtCountMember3;					//������Ա
	CSkinEdit						m_edtCountMember4;					//�ļ���Ա
	CSkinEdit						m_edtCountMember5;					//�弶��Ա

	CSkinEdit						m_edtInputChat;						//��������
	//��ť�ؼ�
protected:
	CSkinButton						m_btEnterRoom;						//���뷿��
	CSkinButton						m_btExitRoom;						//�˳�����
	CSkinButton						m_btRoomManager;					//�������
	CSkinButton						m_btGameControl;					//��Ϸ����
	CSkinButton						m_btSystemMessage;					//ϵͳ��Ϣ
	CSkinButton						m_btSendChat;						//������Ϣ
	CSkinButton						m_btAIAdvanced;						//AI�߼����԰�ť
	CSkinButton						m_btQuery;							//��ѯ��ť
	CSkinButton						m_btAddItem;						//�������
	CSkinButton						m_btModifyItem;						//�޸�����
	CSkinButton						m_btDeleteItem;						//ɾ������

	//�ؼ�����
protected:
	CSkinListCtrl					m_ParemeterList;					//�����б�
	CSystemTrayIcon					m_SystemTrayIcon;					//����ͼ��

	//�������
protected:
	CMissionList					m_MissionList;						//�б�����
	CMissionLogon					m_MissionLogon;						//��¼����
	CMissionAndroid					m_MissionAndroid;					//��������
	CMissionManager					m_MissionManager;					//�������
	
	//�û�����
protected:
	DWORD							m_dwUserRight;						//�û�Ȩ��
	DWORD							m_dwMasterRight;					//����Ȩ��
	IClientUserItem *				m_pIMySelfUserItem;					//�Լ�ָ��
	IClientUserItem *				m_pISelectedUserItem;				//ѡ��ָ��
	//��������
protected:	
	WORD							m_wServerType;						//��������
	DWORD							m_dwServerRule;						//�������
	tagGameKind						m_GameKind;							//������Ϣ
	tagGameServer					m_GameServer;						//������Ϣ
	enServiceStatus					m_ServiceStatus;					//����״̬	

	// �������
public:
	HINSTANCE						m_hDebugInst;
	IClientDebug *					m_pIClientDebug;					//���ƽӿ�
	HINSTANCE						m_hAIConfigDebugInst;				//AI����ʵ��
	IAIConfigDebug *				m_pIAIConfigDebug;					//AI���Խӿ�
	CRichEditMessage				m_ChatMessage;						//������Ϣ
	CWndChatControl					m_ChatControl;						//�������
	CUserListControl				m_UserListControl;					//�û��б�
	
	CDlgServerManager *				m_pDlgServerManager;				//�������
	//�������
private:
	DWORD							m_dwChatTime;						//����ʱ��
	CString							m_strChatString;					//������Ϣ
	//��̬����
protected:
	static CPlatformFrame *			m_pPlatformFrame;					//���ָ��

	//��������
public:
	//���캯��
	CPlatformFrame();
	//��������
	virtual ~CPlatformFrame();

	//�����б�����
	VOID	CleanListData();
	//���غ���
public:
	//��ʼ�ؼ�
	//virtual VOID InitControlUI();
	//��Ϣ����
	//virtual VOID Notify(TNotifyUI &  msg);
	//Ƥ������
	//virtual LPCTSTR GetSkinFile() { return TEXT("ServerCtrl.xml"); }
	//���غ���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//״̬֪ͨ
public:
	//���֪ͨ
	virtual VOID OnGameItemFinish();
	//���֪ͨ
	virtual VOID OnGameKindFinish(WORD wKindID);
	//����֪ͨ
	virtual VOID OnGameItemUpdateFinish();
	
	//�û��˵�
protected:
	//��ʾ�˵�
	virtual VOID TrackUserItemMenu(IClientUserItem * pIClientUserItem);
	
	//�Ҽ��б�
	VOID OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//����ӿ�
protected:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wSocketID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//����֪ͨ
public:
	//����֪ͨ
	virtual VOID OnGameItemInsert(CGameListItem * pGameListItem);
	//����֪ͨ
	virtual VOID OnGameItemUpdate(CGameListItem * pGameListItem);
	//ɾ��֪ͨ
	virtual VOID OnGameItemDelete(CGameListItem * pGameListItem);

	//�����ӿ�
public:
	//�ؼ�ʹ��
	virtual VOID OnEventEnableControls(BOOL bEnabled);
	//��������
	virtual VOID OnEventAndroidParenter(WORD wSubCommdID,WORD wParameterCount,tagAndroidParameter * pAndroidParameter);

	//��������
private:
	//��ʼ���ؼ�
	VOID InitControls();
	//����޸�
	VOID AddModifyParameter(bool bModify);
	//ѡ������
	VOID SelectItem(int nIndex);
	//��������
	VOID SetItemToParameterList(int nItemIndex,tagAndroidParameter * pAndroidParameter);
	//�����ؼ�
	VOID RectifyControl(INT nWidth, INT nHeight);
	//����״̬
	VOID SetServiceStatus(enServiceStatus ServiceStatus);
	//��Ϣ����
protected:
	//�ر���Ϣ
	VOID OnClose();	
	//���ٴ���
	VOID OnDestroy();
	//������Ϣ
	BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);
	//λ����Ϣ
	VOID OnGetMinMaxInfo(MINMAXINFO * lpMMI);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonDblClk(UINT nFlags, CPoint Point);
	//�ı���Ϣ
	VOID OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//ѡ��ı�
	VOID OnCbnComboGameSelchange();
	//��������
	VOID OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	//ɾ������
	VOID OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	//���̰���
	VOID OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult);

	//�ؼ��ӿ�
public:
	//��Ϣ�ӿ�
	IStringMessage * GetStringMessage() { return QUERY_OBJECT_INTERFACE(m_ChatMessage, IStringMessage); }
	//�Զ���Ϣ
protected:
	//�¼���Ϣ
	LRESULT OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam);

	//�¼�����
protected:
	//�������
	VOID OnBnClickedBtAdd();
	//�༭����
	VOID OnBnClickedBtModify();
	//ɾ������
	VOID OnBnClickedBtDelete();
	//��ѯ����
	VOID OnBnClickedBtQuery();
	//���뷿��
	VOID OnBnClickedBtEnterRoom();
	//�˳�����
	VOID OnBnClickedBtExitRoom();
	//��Ϸ����
	VOID OnBnClickedGameDebug();
	//AI�߼�����
	VOID OnBnClickedAIAdvanced();
	//������Ϣ
	VOID OnBnclickedSendChat();
	//ϵͳ��Ϣ
	VOID OnBnClickedSystemMsg();
	//�������
	VOID OnBnClickedRoomManager();

	//�û��ӿ�
public:
	//�û�����
	virtual VOID OnUserItemAcitve(IClientUserItem * pIClientUserItem);
	//�û�ɾ��
	virtual VOID OnUserItemDelete(IClientUserItem * pIClientUserItem);
	//�û�����
	virtual VOID OnUserFaceUpdate(IClientUserItem * pIClientUserItem);
	//�û�����
	virtual VOID OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserScore & LastScore);
	//�û�����
	virtual VOID OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserStatus & LastStatus);
	//�û�����
	virtual VOID OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserAttrib & UserAttrib);
	//�����¼�
protected:
	//�û�����
	bool OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��¼����
	bool OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//���ô���
	bool OnSocketMainConfig(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//״̬����
	//bool OnSocketMainStatus(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////���д���
	//bool OnSocketMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////������
	//bool OnSocketMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////�һ�����
	//bool OnSocketMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////���ߴ���
	//bool OnSocketMainProperty(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������
	bool OnSocketMainManager(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////ϵͳ����
	//bool OnSocketMainSystem(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////��������
	//bool OnSocketMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////ǩ������
	//bool OnSocketMainCheckIn(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////��Ա����
	//bool OnSocketMainMember(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////�ͱ�����
	//bool OnSocketMainBaseEnsure(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����¼�
protected:
	//�û�����
	bool OnSocketSubUserEnter(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnSocketSubUserScore(VOID * pData, WORD wDataSize);
	//�û�״̬
	bool OnSocketSubUserStatus(VOID * pData, WORD wDataSize);
	////�û���Ϸ����
	//bool OnSocketSubUserItemGameData(VOID * pData, WORD wDataSize);
	//�һ��¼�
	bool OnSocketSubAndroidHangingNotify(VOID * pData, WORD wDataSize);
	//�ȼ��¼�
	bool OnSocketSubAndroidGradeNotify(VOID * pData, WORD wDataSize);
	//��¼���
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//��¼�ɹ�
	bool OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);
	//������ʾ
	//bool OnSocketSubUpdateNotify(VOID * pData, WORD wDataSize);
	////����ʧ��
	//bool OnSocketSubRequestFailure(VOID * pData, WORD wDataSize);
	////�û�����
	//bool OnSocketSubUserChat(VOID * pData, WORD wDataSize);
	////�û�����
	//bool OnSocketSubExpression(VOID * pData, WORD wDataSize);
	////�û�˽��
	//bool OnSocketSubWisperUserChat(VOID * pData, WORD wDataSize);
	////˽�ı���
	//bool OnSocketSubWisperExpression(VOID * pData, WORD wDataSize);
	//ϵͳ��Ϣ
	bool OnSocketSubSystemMessage(VOID * pData, WORD wDataSize);
	////������Ϣ
	//bool OnSocketSubActionMessage(VOID * pData, WORD wDataSize);
	////�������
	//bool OnSocketSubUserInvite(VOID * pData, WORD wDataSize);
	////���߳ɹ�
	//bool OnSocketSubPropertySuccess(VOID * pData, WORD wDataSize);
	////����ʧ��
	//bool OnSocketSubPropertyFailure(VOID * pData, WORD wDataSize);
	////����ЧӦ
	//bool OnSocketSubPropertyEffect(VOID * pData, WORD wDataSize);
	////������Ϣ
	//bool OnSocketPropertyMessage(VOID * pData, WORD wDataSize);
	////��������
	//bool OnSocketPropertyTrumpet(VOID * pData, WORD wDataSize);
	////�ȴ�����
	//bool OnSocketSubWaitDistribute(VOID *pData, WORD wDataSize);
	////����Ч��
	//bool OnSocketSubUserPropBuff(VOID *pData, WORD wDataSize);
	////��ұ���
	//bool OnSocketSubUserBackpack(VOID *pData, WORD wDataSize);
	////����ʹ��
	//bool OnSocketSubPropertyUse(VOID *pData, WORD wDataSize);
	////��ѯ����
	//bool OnSocketSubQuerySendPresent(VOID *pData, WORD wDataSize);
	////���͵���
	//bool OnSocketSubPropertyPresent(VOID *pData, WORD wDataSize);
	////��ȡ����
	//bool OnSocketSubGetSendPresent(VOID *pData, WORD wDataSize);
	////�������
	//bool OnSocketSubUserTrumpet(VOID *pData, WORD wDataSize);
		
	//�ڲ�����
public:
	// ����Ч��
	bool EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex);
	//��������
	bool SendUserChatPacket(DWORD dwTargetUserID, LPCTSTR pszChatString, COLORREF crColor);

	// �û�ѡ��
	virtual VOID OnChangeChatTarget(IClientUserItem * pIClientUserItem);
	// ������Ϣ
	virtual bool OnDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);
	//AI���Ե�����Ϣ
	virtual bool OnAIConfigDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);
	// ������Ϣ
	bool  OnSocketSubUserDebug(const void * pData, WORD wDataSize);
	//AI����������Ϣ
	bool  OnSocketSubUserAIConfigDebug(const void * pData, WORD wDataSize);
	// ���͵�¼
	bool SendLogonPacket();
	// ��ʾ��ť
	VOID ShowHideButton(bool bShow);

	//�û�����
	VOID OnEventUserEnter(IClientUserItem * pIClientUserItem);
	//�û��뿪
	VOID OnEventUserLeave(IClientUserItem * pIClientUserItem);
	//���ܺ���
public:
	//��ȡʵ��
	static CPlatformFrame * GetInstance() { return m_pPlatformFrame; }
	//�ͷ�ʵ��
	static void ReleaseInstance();

	DECLARE_MESSAGE_MAP()			
};

//////////////////////////////////////////////////////////////////////////////////

#endif