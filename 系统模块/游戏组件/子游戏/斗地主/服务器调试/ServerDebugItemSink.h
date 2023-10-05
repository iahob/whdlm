#pragma once
#include "../���������/ServerDebug.h"

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//��Ϣ����
struct tagDebugInfo
{
	DWORD							dwDebugID;							//����ID
	WORD							wTableID;							//���Ӻ�
	int								nCardLevel;							//���Ƶȼ�
	BYTE							cbCardChance;						//���Ƹ���
	int								nStartTime;							//��ʼʱ��
	int								nEndTime;							//����ʱ��
	int								nRunCount;							//ִ�д���
	SCORE							lSystemScore;						//���Ե÷�
	bool							bWaitScore;							//�ȴ�����
	bool							bWaitDelete;						//�ȴ�ɾ��
	CTime							tStartTime;							//��ʼʱ��
	CTime							tEndTime;							//����ʱ��
};

//��Ϣ����
struct tagHistoryDebugInfo
{
	DWORD							dwDebugID;						//����ID
	WORD							wTableID;							//���Ӻ�
	int								nCardLevel;							//���Ƶȼ�
	BYTE							cbCardChance;						//���Ƹ���
	int								nDebugTime[3];					//��������
	int								nStartTime;							//��ʼʱ��
	int								nEndTime;							//����ʱ��
	SCORE							lSystemScore;						//���Ե÷�
};

//��Ϣ����
struct tagDebugCardInfo
{
	BYTE							cbCardData[4];						//�����˿�
	BYTE							cbCardCount;						//��������
	BYTE							cbDebugCount;						//��������
	int								nCardLevel;							//���Ƶȼ�
	tagDebugCardInfo()
	{
		ZeroMemory(this, sizeof(tagDebugCardInfo));
	}
	tagDebugCardInfo(const tagDebugCardInfo &TRight)
	{
		CopyMemory(this, &TRight, sizeof(tagDebugCardInfo));
	}
	void operator= (const tagDebugCardInfo &TRight)
	{
		CopyMemory(this, &TRight, sizeof(tagDebugCardInfo));
	}
};


typedef CWHArray< tagDebugInfo >				CDebugInfo;
typedef CWHArray< tagHistoryDebugInfo >		CHistoryDebugInfo;
typedef CWHArray< tagDebugCardInfo >			CDebugCard;

class CServerDebugItemSink : public IServerDebug
{
	DWORD				m_dwDebugID;
	CDebugInfo			m_ArrayDebug;
	CHistoryDebugInfo	m_ArrayHistoryDebug;
	CDebugCard			m_ArrayDebugCard;

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//����������
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame);
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem);

	//���Ƶ���
	virtual bool __cdecl GetDebugInfo(ITableFrame * pITableFrame, BYTE cbBankerCard[3], BYTE cbHandCardCount[GAME_PLAYER], BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT]);

	//���·���
	virtual void __cdecl UpdateDebugInfo(ITableFrame * pITableFrame);
	virtual bool __cdecl UpdateDebugInfo(ITableFrame * pITableFrame, tagScoreInfo ScoreInfoArray[GAME_PLAYER]);

	//��Ϣ����
private:
	//���ӵ���
	bool AddDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, CMD_C_AddDebug *pAddDebug);
	//�޸ĵ���
	bool ModifyDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, CMD_C_ModifyDebug *pModifyDebug);
	//ɾ������
	bool DeleteDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, DWORD dwDebugID);

	//�ڲ�����
private:
	//�������
	int GetDebugInfoIndex(DWORD dwDebugID);
	//��ȡ����
	int GetDebugIndex(ITableFrame * pITableFrame, bool bUpdateScore);
	//�������
	int GetInsertIndex(tagDebugInfo &DebugInfo);
	//У�������Ϣ
	bool CheckDebugInfo(tagDebugInfo &DebugInfo, int nExcludeIndex = -1);
	//����֪ͨ
	bool SendMesasage(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, TCHAR * pText, ...);
	// ������Ϣ
	bool SendDebugInfo(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, tagDebugInfo &DebugInfo);
	// ������Ϣ
	bool SendHistoryDebugInfo(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, tagDebugInfo &DebugInfo);
	//���Ӻ���
	int AddGoodCard(tagDebugCardInfo &DebugCardInfo);
	//��С�����
	int GetMinRandCount(int nCardLevel);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�
	bool RandCardList(BYTE cbSrcCardData[], BYTE cbSrcCardCount, BYTE cbNewCardData[], BYTE cbNewCardCount);
	//���ʣ�µ���
	bool RandDebugCardList(BYTE cbSrcCardData[], BYTE cbSrcCardCount, BYTE cbNewCardData[], BYTE cbNewCardCount, int nRandCount);
};
