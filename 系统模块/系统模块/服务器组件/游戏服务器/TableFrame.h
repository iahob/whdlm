#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef IServerUserItem * CTableUserItemArray[MAX_CHAIR];				//��Ϸ����
typedef CWHArray<IServerUserItem *> CLookonUserItemArray;				//�Թ�����
typedef CWHArray<tagGameScoreRecord *> CGameScoreRecordArray;			//��¼����
typedef CWHArray<tagGroupScoreRecord *> CGroupScoreRecordArray;			//��¼����
typedef CMap<DWORD, DWORD, LONGLONG, LONGLONG&> CMapPersonalTabelScoreInfo;
typedef CWHArray<DWORD> CUserSitSort;									//�����������

interface IMatchTableFrameNotify;

//ȡ������ԭ��
#define CANCELTABLE_REASON_PLAYER		0								//���ȡ��
#define CANCELTABLE_REASON_SYSTEM 		1								//ϵͳȡ��
#define CANCELTABLE_REASON_ENFOCE 		2							    //ǿ�ƽ�ɢ����
#define CANCELTABLE_REASON_ERROR 		3								//����ȡ��
#define CANCELTABLE_REASON_NOT_START	4
#define CANCELTABLE_REASON_OVER_TIME	5

//////////////////////////////////////////////////////////////////////////////////

//���ӿ��
class CTableFrame : public ITableFrame, public ICompilationSink
{
	//��Ϸ����
protected:
	WORD							m_wTableID;							//���Ӻ���
	WORD							m_wChairCount;						//������Ŀ
	BYTE							m_cbStartMode;						//��ʼģʽ
	WORD							m_wUserCount;						//�û���Ŀ
	SCORE							m_lBetValueArray[MAX_AI_CHIP_COUNT];//���ó���

	//״̬����
protected:
	bool							m_bGameStarted;						//��Ϸ��־
	bool							m_bDrawStarted;						//��Ϸ��־
	bool							m_bTableStarted;					//��Ϸ��־
	bool							m_bTableInitFinish;					//��ʼ��ʶ

	//״̬����
protected:
	bool							m_bAllowLookon[MAX_CHAIR];			//�Թ۱�־
	SCORE							m_lFrozenedScore[MAX_CHAIR];		//�������(����ģʽ1.�볡�� 2.�볡����)

	//��Ϸ����
protected:
	LONG							m_lCellScore;						//��Ԫ����
	BYTE							m_cbGameStatus;						//��Ϸ״̬

	//ʱ�����
protected:
	DWORD							m_dwDrawStartTime;					//��ʼʱ��
	SYSTEMTIME						m_SystemTimeStart;					//��ʼʱ��
	WORD                            m_wDrawCount;                       //��Ϸ����

	//��̬����
protected:
	DWORD							m_dwTableOwnerID;					//�����û�
	TCHAR							m_szEnterPassword[LEN_PASSWORD];	//��������-��ͨ��
	tagRoomPassword					m_hRoomPassword[MAX_PASSWORD_COUNT];//��������-Լս��
	//���߱���
protected:
	WORD							m_wOffLineCount[MAX_CHAIR];			//���ߴ���
	DWORD							m_dwOffLineTime[MAX_CHAIR];			//����ʱ��

	//�û�����
protected:
	CTableUserItemArray				m_TableUserItemArray;				//��Ϸ�û�
	CLookonUserItemArray			m_LookonUserItemArray;				//�Թ��û�

	CMD_SS_G_AI_Update_Table_Count m_AIUpdateTableCount;				//AI��������
	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�
	IMainServiceFrame *				m_pIMainServiceFrame;				//����ӿ�
	IAndroidUserManager *			m_pIAndroidUserManager;				//�����ӿ�


	//��չ�ӿ�
protected:
	ITableUserAction *				m_pITableUserAction;				//�����ӿ�
	ITableUserRequest *				m_pITableUserRequest;				//����ӿ�	
	ITableShareStorage *			m_pITableShareStorage;				//������ӿ�	

	//���ݽӿ�
protected:
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//�ں�����
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//��¼����

	//������Ϣ
protected:
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameMatchOption *			m_pGameMatchOption;					//��������	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//��Ϸ��¼
protected:
	CGameScoreRecordArray			m_GameScoreRecordActive;			//��Ϸ��¼
	static CGameScoreRecordArray	m_GameScoreRecordBuffer;			//��Ϸ��¼
	//�����˼�¼
protected:
	CGroupScoreRecordArray			m_GroupScoreRecordActive;			//��Ϸ��¼
	static CGroupScoreRecordArray	m_GroupScoreRecordBuffer;			//��Ϸ��¼
	//�����ӿ�
protected:
	ITableFrameHook	*				m_pITableFrameHook;					//�����ӿ�
	ITableUserAction *				m_pIMatchTableAction;				//�����ӿ�
	IPersonalTableFrameHook	*		m_pIPersonaTableFrameHook;			//�����ӿ�
	ITCPSocketService *				m_pAndroidITCPSocketService;		//AI����ӿ�

	//˽������
protected:
	IServerUserItem*				m_pTableOwnerUser;					//������Ϣ
	DWORD							m_dwDrawCountLimit;					//cbGameMode 0:�������� 1��ʱ������ 2��Ȧ������ 3����������
	DWORD							m_dwDrawTimeLimit;					//ʱ������
	CTime							m_ctTimeModeStart;					//ʱ��ģʽ��ʼʱ��
	DWORD							m_dwPersonalPlayCount;				//Լս����
	LONGLONG						m_lIniScore;						//��ʼ����
	DWORD							m_dwTimeBegin;						//��ʼʱ��
	DWORD							m_PlayerWaitTime;					//�ȴ����ʱ��
	DWORD							m_TableOwnerWaitTime;				//�ȴ�����ʱ��
	DWORD							m_GameStartOverTime;				//�ȴ���ʼʱ��
	bool							m_bWaitConclude;					//�ȴ�����
	DWORD							m_dwReplyCount;						//�ظ�����
	byte							m_cbRequestReply[MAX_CHAIR];		//�����
	byte							m_cbRequestUser;					//�����ɢ��������Ӻ�
	byte							m_cbIsRequestOver;					//�����ɢ�Ƿ����
	CMapPersonalTabelScoreInfo		m_MapPersonalTableScoreInfo;		//��ҷ���
	DWORD							m_dwPersonalRoomID;					//Լս����ID
	bool							m_bIsDissumGame;					//�Ƿ�ǿ�ƽ�ɢ��Ϸ
	byte							m_cbGanmeRule[RULE_LEN];
	bool							m_bIsAllRequstCancel;
	bool							m_bPersonalLock;					//˽������
	bool							m_bIsEnfoceDissum;					//�Ƿ�ǿ�ƽ�ɢ��Ϸ
	BYTE							m_cbLoopCount;						//��ǰȦ��
	DWORD							m_dwGroupID;						//������ID
	BYTE							m_cbGroupTaxMode;					//����ģʽ��1�̶�ģʽ������ң�2�̶�ģʽ��Ӯ�ң�3����ģʽ������ң�4����ģʽ��Ӯ��
	LONGLONG						m_lGroupTaxCount;					//���������ֵ
	BYTE							m_cbLessScoreDismiss;				//����������ɢ
	WORD							m_wAutoReadyTime;					//�Զ�׼��ʱ��
	DWORD							m_dwPreTurnReadyUser[MAX_CHAIR];	//��¼��һ�� ׼�������

	//Լս����ս����¼
public:
	tagPersonalUserScoreInfo		m_PersonalUserScoreInfo[MAX_CHAIR];		//Լս����ս����¼
	byte							m_cbSpecialInfo[SPECIAL_INFO_LEN];		//Լս�����ɢͳ����Ϣ
	int								m_nSpecialInfoLen;						//Լս����ͳ����Ϣ����
	tagPersonalTableParameter		m_PersonalTableParameter;				//Լս��������Ϣ
	tagPersonalRoomOption			m_PersonalRoomOption;					//Լս��������
	DWORD							m_dwRecordTableID;
	bool							m_bEscapeStatus[MAX_CHAIR];				//�û��Ƿ�Ϊ����״̬	
	int								m_nPersonalRoomPlayerCount;
	TCHAR							m_szRoomGUID[PERSONAL_ROOM_GUID];		// Լս����Ψһ��
	bool							m_bIsCancelBeforeOneTurn;				//�Ƿ�һ��δ�����ͱ���ɢ����
	DWORD							m_dwDealCancel;

public:
	BYTE							m_cbGameMode;							//0:����ģʽ 1��ʱ��ģʽ 2��Ȧ��ģʽ 3������ģʽ
	BYTE							m_cbPayMode;							//֧����ʽ 0 ��ʾ ����֧���� 1 ��ʾ AA�� ֧��
	DWORD							m_dwRoomHostAddr;						//����ip��ַ
	LONG							m_lPayRoomCardCount;					//֧����������
	//״̬
	SYSTEMTIME						m_sysStartTime;						//������ʼʱ��
	SYSTEMTIME						m_sysEndTime;						//��������ʱ��
	DWORD							m_dwPlayingUserID[MAX_CHAIR];		//��¼������Ϸ�����	//״̬
	BYTE							m_cbPlayingStatusLeave[MAX_CHAIR];	//��Ϸ��״̬�뿪	
	//Լս������
	long long						m_lEnterScoreLimite;				//Լս��ҷ��볡�޶�
	long long						m_lLeaveScoreLimite;				//Լս��ҷ��볡�޶�
	byte							m_cbCanLeave[MAX_CHAIR];			//�Ƿ��ܹ��뿪
	DWORD							m_dwInOutUser[MAX_CHAIR];			//Լս�����뿪����ң������õ�
	BYTE							m_cbChangeChairLeave;				//�����뿪
	BYTE							m_cbSetAutoStart;					//�����Զ���ʼ
	CUserSitSort					m_ArrayUserSitSort;					//�����������
	WORD							m_wAIAutoStartTime;					//AI�Զ���ʼʱ��

	//��¼��ǰ�п�ʼ��ť�����
	DWORD							m_dwStartCommandUser;				//�п�ʼ��ť���
	bool							m_cbIsAreadySend;					//�Ƿ��Ѿ����Ϳ�ʼ��ť
	byte							m_cbAICreate;						//�Ƿ�ΪAI����
	DWORD							m_dwGroupConfigID;					//�淨��ʶ
	//��������
public:
	//���캯��
	CTableFrame();
	//��������
	virtual ~CTableFrame();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���Խӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID() { return m_wTableID; }
	//��Ϸ����
	virtual WORD GetChairCount() { return m_wChairCount; }
	//��λ����Ŀ
	virtual WORD GetNullChairCount(){ return m_wChairCount - m_wUserCount; }

	//���ò���
public:
	//�Զ�����
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//��������
	virtual tagGameMatchOption * GetGameMatchOption() { return m_pGameMatchOption; };
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��������
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//���ýӿ�
public:
	//��ʼģʽ
	virtual BYTE GetStartMode() { return m_cbStartMode; }
	//��ʼģʽ
	virtual VOID SetStartMode(BYTE cbStartMode) { m_cbStartMode = cbStartMode; }

	//��Ԫ����
public:
	//��Ԫ����
	virtual LONG GetCellScore() { return m_lCellScore; }
	//��Ԫ����
	virtual VOID SetCellScore(LONG lCellScore);

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus = cbGameStatus; }

	//��Ϣ�ӿ�
public:
	//��Ϸ״̬
	virtual bool IsGameStarted() { return m_bGameStarted; }
	//��Ϸ״̬
	virtual bool IsDrawStarted() { return m_bDrawStarted; }
	//��Ϸ״̬
	virtual bool IsTableStarted() { return m_bTableStarted; }
	//����״̬
	virtual bool IsTableLocked();

	//Լս����������
	virtual VOID SetPersonalTablePassword(tagRoomPassword *pPassword){ memcpy(m_hRoomPassword, pPassword, sizeof(m_hRoomPassword)); }
	//���ƽӿ�
public:
	//��ʼ��Ϸ
	virtual bool StartGame();
	//��ɢ��Ϸ
	virtual bool DismissGame();
	//������Ϸ
	virtual bool ConcludeGame(BYTE cbGameStatus, BOOL bPersonalLoop = false);
	//��������
	virtual bool ConcludeTable();

	//�û��ӿ�
public:
	//Ѱ���û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChairID);
	//�Թ��û�
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex);

	//д�ֽӿ�
public:
	//д�����
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal = INVALID_DWORD, DWORD dwPlayGameTime = INVALID_DWORD);
	//д�����
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);

	//д��¼��
	virtual bool WriteTableVideoPlayer(DWORD dwUserID, CHAR *pVideoNumber);
	//д��¼��
	virtual bool WriteTableVideoData(CHAR *pVideoNumber, WORD wServerID, WORD wTableID, BYTE *pVideoData, WORD wSize);

	//����ӿ�
public:
	//����������
	virtual SCORE CalculateRevenue(WORD wChairID, SCORE lScore);
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);

	//ʱ��ӿ�
public:
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID);

	//��Ϸ�û�
public:
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME);

	//�Թ��û�
public:
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�����û�
public:
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��������
	virtual bool SendRoomData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//��������
	virtual bool SendRoomData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendRoomAIConfigData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//��������
	virtual bool SendRoomAIConfigData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//ϵͳ��Ϣ
public:
	//������Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);

public:
	//������־����
	virtual bool SendGameLogData(tagLogUserInfo LogUserInfo);
	//������־����
	virtual bool SendGameLogData(CString cstrLogInfo);
	//������־����
	virtual bool SendGameLogData(const TCHAR * szLogInfo, ...);
	//������־����
	virtual bool SendGameLogData(IServerUserItem * pIServerUserItem, const TCHAR * pszFormat, ...);
	//������־����
	virtual bool SendGameLogData(IServerUserItem * pIServerUserItem, const TCHAR szDescName[LEN_SERVER], const TCHAR * pszFormat);

	//��������
public:
	//��������
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative = false);
	//�Թ۶���
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem);
	//���¶���
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword = NULL, DWORD dwRoomPwd = 0);

	//���ܽӿ�
public:
	//���ͳ���
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//�����ӿ�
public:
	//���ýӿ�
	virtual bool SetTableFrameHook(IUnknownEx * pIUnknownEx);
	//��ȡ�ӿ�
	virtual IUnknownEx * GetTableFrameHook(){ return m_pITableFrameHook; }
	//α������
	virtual bool ImitateGameOption(IServerUserItem * pIServerUserItem);

	//����ӿ�
public:
	virtual bool SendChatMessage(IServerUserItem * pIServerUserItem, IServerUserItem * pITargetServerUserItem, DWORD dwChatColor, LPCTSTR lpszChatString, LPTSTR lpszDescribeString);
	//��ȡ��Ϣ
public:
	//��ȡ��Ϣ
	virtual LPCTSTR GetCompilation(){ return szCompilation; }

	//���ܺ���
public:
	//��Ϸ����
	virtual WORD GetDrawCount();
	//��ȡ��λ
	virtual WORD GetNullChairID();
	//�����λ
	WORD GetRandNullChairID();
	//�û���Ŀ
	WORD GetSitUserCount();
	//�Թ���Ŀ
	WORD GetLookonUserCount();
	//������Ŀ
	WORD GetOffLineUserCount();
	//�뿪�ж�
	BOOL JudePersonalStandUp(IServerUserItem * pIServerUserItem);
	//��ѯ����
	void GetBetValues(SCORE lBetValueArray[MAX_AI_CHIP_COUNT]);

	//���ܺ���
public:
	//�û�״��
	WORD GetTableUserInfo(tagTableUserInfo & TableUserInfo);
	//��������
	bool InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter);

	//�û��¼�
public:
	//�����¼�
	bool OnEventUserOffLine(IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//ϵͳ�¼�
public:
	//ʱ���¼�
	bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//��Ϸ�¼�
	bool OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//����¼�
	bool OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//��������
public:
	//����״̬
	bool SendTableStatus();
	//����ʧ��
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);

	//Ч�麯��
public:
	//��ʼЧ��
	bool EfficacyStartGame(WORD wReadyChairID);
	//��ַЧ��
	bool EfficacyIPAddress(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyScoreRule(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem);

public:
	//������
	bool CheckDistribute();
	//��Ϸ��¼
	void RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime = INVALID_DWORD);
	//�����˼�¼
	void RecordGroupScore(bool bDrawStarted, DWORD dwStartGameTime = INVALID_DWORD);
	//˽������
	virtual bool IsPersonalTableLocked() { return m_bPersonalLock; }
	//��������
	virtual VOID SetPersonalTableLocked(bool bLocked);
	//��ȡ����
	virtual bool GetPersonalScore(DWORD dwUserID, LONGLONG& lScore);
	//������Ϣ
	virtual tagPersonalTableParameter GetPersonalTableParameter();

	//˽������
public:
	//��������
	virtual VOID SetTableOwner(DWORD dwUserID) { m_dwTableOwnerID = dwUserID; };
	//��ȡ����
	virtual DWORD GetTableOwner() { return m_dwTableOwnerID; };
	//α������
	virtual DWORD GetRecordTableOwner();
	//��������
	virtual VOID SetPersonalTable(DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, LONGLONG lIniScore);
	//�������Ӳ���
	virtual VOID SetPersonalTableParameter(tagPersonalTableParameter  PersonalTableParameter, tagPersonalRoomOption PersonalRoomOption);
	//���Ӵ�����೤ʱ��δ��ʼ��Ϸ ��ɢ����
	virtual VOID SetTimerNotBeginAfterCreate();

	//�����ɢ
	virtual bool CancelTableRequest(DWORD dwUserID, WORD dwChairID);
	//��ɢ��
	virtual bool CancelTableRequestReply(DWORD dwUserID, BYTE cbAgree);
	//Լս����ID
	virtual void SetPersonalRoomID(DWORD wRoomID);
	//����ǿ�ƽ�ɢ����
	virtual void HostDissumeGame(BOOL bIsEnforce = false);

	//��ȡԼս����
	virtual DWORD GetPersonalRoomID();

	//������Ϸ����
	virtual VOID SetGameRule(byte * cbRule, int nLen);

	//��ȡ��Ϸ����
	virtual byte * GetGameRule();

	//��ȡ����ʱ��������Ϣ
	virtual void GetSpeicalInfo(byte * cbSpecialInfo, int nSpecialLen);

	//�������������ӵĸ���
	virtual VOID SetTableChairCount(WORD wChairCount);
	//���ó���
	virtual VOID SetBetValueArray(SCORE lBetValueArray[MAX_AI_CHIP_COUNT]);

	//���ô����˰�����Ϸׯ��USERID
	virtual VOID SetGroupMaxChairBankerUserID(WORD wBankerChairID, DWORD dwBankerUserID);

public:
	byte m_cbDataBaseMode;			//ʹ�õ����ݿ�ģʽ  0 Ϊ���� 1 Ϊ���

	//Լս�����Ƿ��ɢ
	virtual bool IsPersonalRoomDisumme();

	//�����ǽ�����ݿ⻹�ǻ������ݿ�,  0 Ϊ���ֿ� 1 Ϊ��ҿ�
	virtual void SetDataBaseMode(byte cbDataBaseMode);

	//��ȡ���ݿ�ģʽ,  0 ����1 Ϊ ���
	virtual byte GetDataBaseMode();

public:
	//����Լս��Ψһ���
	virtual void SetPersonalRoomGUID(TCHAR * szInfo, int nLen);
	//��ȡԼս��Ψһ���
	virtual TCHAR * GetPersonalRoomGUID();
	//����֧����ʽ
	virtual void SetPayRoomCardMode(byte cbPayMode, LONG lPayRoomCardCount,BYTE cbGameMode){ m_cbPayMode = cbPayMode; m_lPayRoomCardCount = lPayRoomCardCount; m_cbGameMode = cbGameMode; }
	//��ȡ֧����ʽ
	virtual BYTE GetPayRoomCardMode(){ return m_cbPayMode; }
	//���淿����ip��ַ
	virtual void SetHostAddr(DWORD dwAddr);
	//��ȡ������ip��ַ
	virtual DWORD  GetHostAddr();

	//��ȡ�����ֵ ��GetGameMode() ���� 0:����ֵ  1��ʱ��ֵ   2��Ȧ��ֵ   3������ֵ
	virtual DWORD  GetDrawCountLimit(){ return m_dwDrawCountLimit; }
	//���ô���ʱ��
	virtual void SetCreatePersonalTime(SYSTEMTIME tm);
	//����Ȧ��
	virtual void SetLoopCount(BYTE val){ m_cbLoopCount = val; }
	//��ȡȦ��
	virtual BYTE GetLoopCount() { return m_cbLoopCount; }
	//��ȡ��Ϸģʽ 0:����ģʽ 1��ʱ��ģʽ 2��Ȧ��ģʽ 3������ģʽ
	virtual BYTE	GetGameMode() { return m_cbGameMode; }

	//������Ϸ��״̬�뿪����
public:
	//������Ϸ��״̬�뿪���� cbCanLeave = 0 ��ʾ�����뿪  cbCanLeave = 1 ��ʾ���뿪 cbCanLeave = 2 ��ʾ�����뿪
	virtual void SetPlayingStatusLeave(WORD wChairID, byte cbCanLeave);
	//Լս���ģʽ���
public:
	virtual void SetEnterAndLeaveScore(LONGLONG lEnterScore, LONGLONG lLeaveScore);
	virtual void GetEnterScore(LONGLONG &lEnterScore);
	//���������
public:
	//���ô�����ID
	virtual void SetGroupInfo(DWORD dwGroupID, BYTE cbMode, DWORD dwCount,BYTE cbLessScoreDismiss);
	//���ô�����ID
	virtual DWORD GetGroupID();
	//��ȡ�����˷��������Ϣ
	virtual void GetGroupTaxInfo(BYTE &cbTaxMode, LONGLONG &dwTaxCount);
	//���������
public:
	//����������ı�
	virtual void ChangeCurSubStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagCurSubStorageItem & CurSubStorageItem);
	//��ȡ��ǰ����������
	virtual tagCurSubStorageItem* GetCurSubStorageItem();
	//��ȡ���ù���������
	virtual tagConfigSubStorageItem* GetConfigSubStorageItem();
	//У�鹲����
	bool EfficacyConfigSubStorage(tagShareStorageItem *ptagShareStorageItem, TCHAR szResultDescripe[512]);

public:
	//��������
	SCORE CalculateServiceFate();

public:
	//֧��״̬����
	byte m_cbPersonalPayStatus[MAX_CHAIR];
	//��ȡ���֧��״̬
	virtual byte GetUserPayStatus(WORD wChairID);
	//�������֧��״̬
	virtual byte SetUserPayStatus(WORD wChairID, byte cbStatus);

public:
	//�Զ���ʼ
	bool AutoStartGame();

	//�Թ�����
	virtual bool PerformLookonStandup(WORD wChairID, IServerUserItem * pIServerUserItem);

	//�ж�����Ƿ�������Ϸ
	virtual bool IsJoinGame(WORD wChairID);

	//�ж�����Ƿ�������Ϸ
	virtual void SetAICreate(byte cbAICreate);

	//�ж�����Ƿ�������Ϸ
	virtual void SetAISocketWorkTCP(ITCPSocketService *	pAndroidITCPSocketService);

	//�ж�����Ƿ�������Ϸ
	virtual void UpdateAITableChange(int nChange);

	//�ж�����Ƿ�������Ϸ
	virtual void SetGroupConfigID(DWORD  dwGroupConfigID);

	//��̬��ʼ�ж�
	bool EfficacyAutoStartGame(WORD wReadyChairID);


};


//////////////////////////////////////////////////////////////////////////////////

#endif