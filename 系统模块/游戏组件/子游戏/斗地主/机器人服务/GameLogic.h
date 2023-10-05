#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////

//��������
#define ST_ORDER					0									//��С����
#define ST_COUNT					1									//��Ŀ����
#define ST_CUSTOM					2									//�Զ�����
#define MAX_SEARCH_RESULT_COUNT		128

//////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[4];					//�˿���Ŀ
	BYTE							cbCardData[4][MAX_COUNT];			//�˿�����
};

//���ƽ��
struct tagOutCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT];			//����˿�
};

//�ֲ���Ϣ
struct tagDistributing
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbDistributing[15][6];				//�ֲ���Ϣ
};

//�������
struct tagSearchCardResult
{
	BYTE							cbSearchCount;						//�����Ŀ
	BYTE							cbCardCount[MAX_SEARCH_RESULT_COUNT];				//�˿���Ŀ
	BYTE							cbResultCard[MAX_SEARCH_RESULT_COUNT][MAX_COUNT];	//����˿�
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];				//�˿�����

	//AI����
public:
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	BYTE							m_cbUserCardCount[GAME_PLAYER];			//�˿���Ŀ
	WORD							m_wBankerUser;							//�������

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//�߼�����
public:
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//��������
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResult & OutCardResult);
	//ͬ������
	BYTE SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
		tagSearchCardResult *pSearchCardResult);
	//������������(����һ���Ĵ�һ��)
	BYTE SearchTakeCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount,
		tagSearchCardResult *pSearchCardResult);
	//��������
	BYTE SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
		tagSearchCardResult *pSearchCardResult);
	//�����ɻ�
	BYTE SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult);

	//�ڲ�����
public:
	//�����˿�
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//�����ֲ�
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing);

	//���ú���
public:
	//�����˿�
	VOID SetUserCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount) ;
	//���õ���
	VOID SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount) ;
	//����ׯ��
	VOID SetBanker(WORD wBanker) ;
	//ɾ���˿�
	VOID RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) ;

	//��������
public:
	//����ը��
	VOID GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount);
	//�����˿�
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//�������˿�
	VOID SortSearchCardResult(tagSearchCardResult &tmpSearchCardResult, bool bEnemyRemainFive);
	//��������˿�
	VOID SortOutCardResult(tagOutCardResult OutCardResultArray[MAX_SEARCH_RESULT_COUNT], bool bEnemyRemainFive, BYTE cbResultCount);
	
	//���ƺ���
public:
	//����Ȩ����
	VOID OutCardRight(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagSearchCardResult *pSearchCardResult);

	//�зֺ���
public:
	//�з��ж�
	BYTE LandScore(WORD wMeChairID) ;
};

//////////////////////////////////////////////////////////////////////////

#endif