#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\��Ϸ������\GameLogic.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////

#define MAX_CALLBANKERTIMES				5
#define MAX_ADDSCORE					4

//ʱ������
struct tagSendFourCallBanker
{
	//0����ģʽ,1���ӱ�
	WORD					wCallBankerChanceArray[MAX_GAMEMODE][MAX_CARD_TYPE];						//��ׯ����
	BYTE					cbBankerTimesArray[MAX_CARD_TYPE][MAX_CALLBANKERTIMES];						//��ׯ����

	tagSendFourCallBanker()
	{
		wCallBankerChanceArray[0][0] = 1;
		cbBankerTimesArray[0][0] = 2;
		cbBankerTimesArray[0][1] = 1;
		cbBankerTimesArray[0][2] = 0;
		cbBankerTimesArray[0][3] = 0;
		cbBankerTimesArray[0][4] = 0;

		wCallBankerChanceArray[0][1] = 2;
		cbBankerTimesArray[1][0] = 4;
		cbBankerTimesArray[1][1] = 3;
		cbBankerTimesArray[1][2] = 2;
		cbBankerTimesArray[1][3] = 1;
		cbBankerTimesArray[1][4] = 0;

		wCallBankerChanceArray[0][2] = 2;
		cbBankerTimesArray[2][0] = 4;
		cbBankerTimesArray[2][1] = 3;
		cbBankerTimesArray[2][2] = 2;
		cbBankerTimesArray[2][3] = 1;
		cbBankerTimesArray[2][4] = 0;


		wCallBankerChanceArray[0][3] = 2;
		cbBankerTimesArray[3][0] = 4;
		cbBankerTimesArray[3][1] = 3;
		cbBankerTimesArray[3][2] = 2;
		cbBankerTimesArray[3][3] = 1;
		cbBankerTimesArray[3][4] = 0;

		wCallBankerChanceArray[0][4] = 2;
		cbBankerTimesArray[4][0] = 3;
		cbBankerTimesArray[4][1] = 4;
		cbBankerTimesArray[4][2] = 2;
		cbBankerTimesArray[4][3] = 1;
		cbBankerTimesArray[4][4] = 0;

		wCallBankerChanceArray[0][5] = 2;
		cbBankerTimesArray[5][0] = 3;
		cbBankerTimesArray[5][1] = 4;
		cbBankerTimesArray[5][2] = 2;
		cbBankerTimesArray[5][3] = 1;
		cbBankerTimesArray[5][4] = 0;

		wCallBankerChanceArray[0][6] = 2;
		cbBankerTimesArray[6][0] = 2;
		cbBankerTimesArray[6][1] = 4;
		cbBankerTimesArray[6][2] = 3;
		cbBankerTimesArray[6][3] = 1;
		cbBankerTimesArray[6][4] = 0;

		wCallBankerChanceArray[0][7] = 3;
		cbBankerTimesArray[7][0] = 1;
		cbBankerTimesArray[7][1] = 2;
		cbBankerTimesArray[7][2] = 4;
		cbBankerTimesArray[7][3] = 5;
		cbBankerTimesArray[7][4] = 3;

		wCallBankerChanceArray[0][8] = 3;
		cbBankerTimesArray[8][0] = 1;
		cbBankerTimesArray[8][1] = 2;
		cbBankerTimesArray[8][2] = 4;
		cbBankerTimesArray[8][3] = 5;
		cbBankerTimesArray[8][4] = 3;

		wCallBankerChanceArray[0][9] = 3;
		cbBankerTimesArray[9][0] = 1;
		cbBankerTimesArray[9][1] = 2;
		cbBankerTimesArray[9][2] = 5;
		cbBankerTimesArray[9][3] = 4;
		cbBankerTimesArray[9][4] = 3;

		wCallBankerChanceArray[0][10] = 5;
		cbBankerTimesArray[10][0] = 1;
		cbBankerTimesArray[10][1] = 2;
		cbBankerTimesArray[10][2] = 5;
		cbBankerTimesArray[10][3] = 4;
		cbBankerTimesArray[10][4] = 3;

		wCallBankerChanceArray[0][11] = 6;
		cbBankerTimesArray[11][0] = 0;
		cbBankerTimesArray[11][1] = 3;
		cbBankerTimesArray[11][2] = 4;
		cbBankerTimesArray[11][3] = 2;
		cbBankerTimesArray[11][4] = 1;

		wCallBankerChanceArray[0][12] = 6;
		cbBankerTimesArray[12][0] = 0;
		cbBankerTimesArray[12][1] = 3;
		cbBankerTimesArray[12][2] = 4;
		cbBankerTimesArray[12][3] = 2;
		cbBankerTimesArray[12][4] = 1;

		wCallBankerChanceArray[0][13] = 4;
		cbBankerTimesArray[13][0] = 1;
		cbBankerTimesArray[13][1] = 3;
		cbBankerTimesArray[13][2] = 5;
		cbBankerTimesArray[13][3] = 4;
		cbBankerTimesArray[13][4] = 2;

		wCallBankerChanceArray[0][14] = 4;
		cbBankerTimesArray[14][0] = 1;
		cbBankerTimesArray[14][1] = 3;
		cbBankerTimesArray[14][2] = 5;
		cbBankerTimesArray[14][3] = 4;
		cbBankerTimesArray[14][4] = 2;

		wCallBankerChanceArray[0][15] = 5;
		cbBankerTimesArray[15][0] = 1;
		cbBankerTimesArray[15][1] = 2;
		cbBankerTimesArray[15][2] = 5;
		cbBankerTimesArray[15][3] = 4;
		cbBankerTimesArray[15][4] = 3;

		wCallBankerChanceArray[0][16] = 7;
		cbBankerTimesArray[16][0] = 0;
		cbBankerTimesArray[16][1] = 0;
		cbBankerTimesArray[16][2] = 1;
		cbBankerTimesArray[16][3] = 2;
		cbBankerTimesArray[16][4] = 3;

		wCallBankerChanceArray[0][17] = 6;
		cbBankerTimesArray[17][0] = 1;
		cbBankerTimesArray[17][1] = 2;
		cbBankerTimesArray[17][2] = 3;
		cbBankerTimesArray[17][3] = 5;
		cbBankerTimesArray[17][4] = 4;

		wCallBankerChanceArray[0][18] = 4;
		cbBankerTimesArray[18][0] = 2;
		cbBankerTimesArray[18][1] = 5;
		cbBankerTimesArray[18][2] = 4;
		cbBankerTimesArray[18][3] = 3;
		cbBankerTimesArray[18][4] = 2;

		wCallBankerChanceArray[1][0] = 0;

		wCallBankerChanceArray[1][1] = 1;

		wCallBankerChanceArray[1][2] = 1;

		wCallBankerChanceArray[1][3] = 1;

		wCallBankerChanceArray[1][4] = 1;

		wCallBankerChanceArray[1][5] = 2;
	
		wCallBankerChanceArray[1][6] = 2;

		wCallBankerChanceArray[1][7] = 2;

		wCallBankerChanceArray[1][8] = 3;

		wCallBankerChanceArray[1][9] = 3;

		wCallBankerChanceArray[1][10] = 6;

		wCallBankerChanceArray[1][11] = 8;
	
		wCallBankerChanceArray[1][12] = 8;

		wCallBankerChanceArray[1][13] = 5;

		wCallBankerChanceArray[1][14] = 5;

		wCallBankerChanceArray[1][15] = 7;

		wCallBankerChanceArray[1][16] = 10;

		wCallBankerChanceArray[1][17] = 9;

		wCallBankerChanceArray[1][18] = 5;
	}
};

//ʱ������
struct tagSendFourAddScore
{
	BYTE					cbAddScoreArray[MAX_CARD_TYPE][MAX_ADDSCORE];					

	tagSendFourAddScore()
	{
		cbAddScoreArray[0][0] = 3;
		cbAddScoreArray[0][1] = 2;
		cbAddScoreArray[0][2] = 1;
		cbAddScoreArray[0][3] = 0;

		cbAddScoreArray[1][0] = 2;
		cbAddScoreArray[1][1] = 4;
		cbAddScoreArray[1][2] = 3;
		cbAddScoreArray[1][3] = 1;

		cbAddScoreArray[2][0] = 2;
		cbAddScoreArray[2][1] = 4;
		cbAddScoreArray[2][2] = 3;
		cbAddScoreArray[2][3] = 1;

		cbAddScoreArray[3][0] = 2;
		cbAddScoreArray[3][1] = 4;
		cbAddScoreArray[3][2] = 3;
		cbAddScoreArray[3][3] = 1;

		cbAddScoreArray[4][0] = 2;
		cbAddScoreArray[4][1] = 4;
		cbAddScoreArray[4][2] = 3;
		cbAddScoreArray[4][3] = 1;

		cbAddScoreArray[5][0] = 2;
		cbAddScoreArray[5][1] = 4;
		cbAddScoreArray[5][2] = 3;
		cbAddScoreArray[5][3] = 1;

		cbAddScoreArray[6][0] = 2;
		cbAddScoreArray[6][1] = 4;
		cbAddScoreArray[6][2] = 3;
		cbAddScoreArray[6][3] = 1;

		cbAddScoreArray[7][0] = 1;
		cbAddScoreArray[7][1] = 2;
		cbAddScoreArray[7][2] = 4;
		cbAddScoreArray[7][3] = 3;

		cbAddScoreArray[8][0] = 1;
		cbAddScoreArray[8][1] = 4;
		cbAddScoreArray[8][2] = 3;
		cbAddScoreArray[8][3] = 2;

		cbAddScoreArray[9][0] = 1;
		cbAddScoreArray[9][1] = 4;
		cbAddScoreArray[9][2] = 3;
		cbAddScoreArray[9][3] = 2;

		cbAddScoreArray[10][0] = 1;
		cbAddScoreArray[10][1] = 4;
		cbAddScoreArray[10][2] = 3;
		cbAddScoreArray[10][3] = 2;

		cbAddScoreArray[11][0] = 0;
		cbAddScoreArray[11][1] = 1;
		cbAddScoreArray[11][2] = 2;
		cbAddScoreArray[11][3] = 3;

		cbAddScoreArray[12][0] = 0;
		cbAddScoreArray[12][1] = 1;
		cbAddScoreArray[12][2] = 2;
		cbAddScoreArray[12][3] = 3;

		cbAddScoreArray[13][0] = 1;
		cbAddScoreArray[13][1] = 2;
		cbAddScoreArray[13][2] = 3;
		cbAddScoreArray[13][3] = 4;

		cbAddScoreArray[14][0] = 1;
		cbAddScoreArray[14][1] = 2;
		cbAddScoreArray[14][2] = 3;
		cbAddScoreArray[14][3] = 4;

		cbAddScoreArray[15][0] = 1;
		cbAddScoreArray[15][1] = 2;
		cbAddScoreArray[15][2] = 3;
		cbAddScoreArray[15][3] = 4;

		cbAddScoreArray[16][0] = 0;
		cbAddScoreArray[16][1] = 0;
		cbAddScoreArray[16][2] = 1;
		cbAddScoreArray[16][3] = 2;

		cbAddScoreArray[17][0] = 1;
		cbAddScoreArray[17][1] = 2;
		cbAddScoreArray[17][2] = 3;
		cbAddScoreArray[17][3] = 4;

		cbAddScoreArray[18][0] = 1;
		cbAddScoreArray[18][1] = 2;
		cbAddScoreArray[18][2] = 3;
		cbAddScoreArray[18][3] = 4;
	}
};

//��Ϸ�Ի���
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��ע��Ϣ
protected:
	LONGLONG						m_lTurnMaxScore;						//�����ע

	//�û��˿�
protected:
	BYTE							m_cbHandCardData[MAX_CARDCOUNT];		//�û�����	

	//��ǰ��4��
	BYTE							m_cbFourCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�

	//�ؼ�����
public:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;					//�û��ӿ�

	//AI��ȡ��	
	LONGLONG						m_lRobotScoreRange[2];					//���Χ
	LONGLONG						m_lRobotBankGetScore;					//�������
	LONGLONG						m_lRobotBankGetScoreBanker;				//������� (ׯ��)
	int								m_nRobotBankStorageMul;					//����

	BYTE                            m_cbDynamicJoin;						//��̬�����ʶ

	//��Ϸ����
protected:
	BANERGAMETYPE_CONFIG			m_bgtConfig;
	BETTYPE_CONFIG					m_btConfig;
	SENDCARDTYPE_CONFIG				m_stConfig;
	KING_CONFIG						m_gtConfig;
	CARDTYPE_CONFIG					m_ctConfig;

	//�������ö��(��Чֵ0)
	LONG							m_lFreeConfig[MAX_CONFIG];

	//�ٷֱ����ö��(��Чֵ0)
	LONG							m_lPercentConfig[MAX_CONFIG];
	WORD							m_wBgtRobNewTurnChairID;				//������ׯ�¿�һ����ׯ����ң���ЧΪINVALID_CHAIR�� ����Чʱ��ֻ��������ѡ���������Ҹ������ׯ�ң�
	LONGLONG						m_lCellScore;

	UINT							m_nChatMsgIndex;						//�����������

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���Խӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//�û�ǿ��
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//������Ϣ
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//�û�̯��
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//�û���ׯ
	bool OnSubCallBanker(const void * pBuffer, WORD wDataSize);
	//�û���ׯ��Ϣ
	bool OnSubCallBankerInfo(const void * pBuffer, WORD wDataSize);
	//�ɷ��˿�
	bool OnSubSendFourCard(const void * pBuffer, WORD wDataSize);

private:
	//��ȡ����
	void ReadConfigInformation(tagCustomAndroid *pCustomAndroid);
	//���в���
	void BankOperate(BYTE cbType);
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString);
	//��������
	void SendAIChatMessage();
	//���ĵ���ģʽ�£����������֪��������������������������
	BYTE GetMaxCardTypeSingle(WORD wChairID);
	//��ȡȨ������
	BYTE GetChanceIndex(BYTE cbChanceArray[], BYTE cbChanceCount);
	//ǰ��4��������
	bool IsFourCardTypeNiu(BYTE cbCardData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig);
};

//////////////////////////////////////////////////////////////////////////

#endif
