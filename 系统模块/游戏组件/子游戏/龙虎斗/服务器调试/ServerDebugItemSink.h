#pragma once
#include "../���������/ServerDebug.h"
#pragma warning(disable :4244)
//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//���п��ܵ���
typedef struct
{
    BYTE							cbCardArray[2];
}CARDARRAY;

class CServerDebugItemSink : public IServerDebug
{
	//���Ʊ���
protected:
	BYTE                            m_cbWinSideDebug;
	BYTE							m_cbExcuteTimes;					//ִ�д���

    CList<CARDARRAY, CARDARRAY>		m_cardArrayList;
    static const BYTE				m_cbCardData[4][13];					//�˿˶���

    //�������
protected:
    ITableFrame						*m_pITableFrame;

public:
    CServerDebugItemSink(void);
    virtual ~CServerDebugItemSink(void);

public:
    //����������
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption);

    //��Ҫ����
	virtual bool __cdecl NeedDebug(BYTE &cbWinSide);

	//���µ���
	void  DebugInfo(bool bCancelAreaCtrl, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption);

	// ��¼����
	VOID WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);

	//���ؿ�������
    virtual bool __cdecl DebugResult(BYTE	cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker);

    //���ӽӿڻص�
    void SetTableFrameSink(ITableFrame * pITableFrame)  { if(m_pITableFrame != pITableFrame) { m_pITableFrame = pITableFrame; } }

    //��ȡ��ֵ
    BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }

    //��ʼ���˿������ֿ���
    void InitCardArray();

    //�������
    SCORE CalculateScore(BYTE cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker);
};
