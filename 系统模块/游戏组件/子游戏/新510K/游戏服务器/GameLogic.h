#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define ST_ORDER					1									//��С����
#define ST_COUNT					2									//��Ŀ����

//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[8];					//�˿���Ŀ
	BYTE							cbCardData[8][MAX_CARD_COUNT];		//�˿�����
};

//���ƽ��
struct tagOutCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_CARD_COUNT];		//����˿�
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
	BYTE							cbSearchCount;						//������Ŀ
	BYTE							cbCardCount[MAX_CARD_COUNT];		//�˿���Ŀ
	BYTE							cbResultCard[MAX_CARD_COUNT][MAX_CARD_COUNT];//�˿��б�
};

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//�˿�����
	bool									m_b2Biggest;										//�Ƿ�2���
	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();
	VOID	 Set2Biggest(bool bFlag){m_b2Biggest = bFlag;}

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
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType,bool b2Biggest=TRUE);
	//ɾ���˿�
	bool RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�
	VOID SortOutCardList(BYTE cbCardData[], BYTE cbCardCount);


	//�߼�����
public:
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData,bool b2Biggest=TRUE);
	//�Ա��˿�
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//��������
	BYTE SearchOutCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult );
	//����ָ��ͬ�� (����һ�Ի�������)
	BYTE SearchSameCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult );
	//����������
	BYTE SearchSingleCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagSearchCardResult *pSearchCardResult );
	//����ը��
	BYTE SearchBombCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult );
	//��������
	BYTE SearchLineCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount, tagSearchCardResult *pSearchCardResult );
	//��������
	BYTE SearchTakeCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult );
	//�����ɻ������
	BYTE SearchThreeLineTakeNTwo( const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE cbLineCount ,tagSearchCardResult *pSearchCardResult );
	//�����ɻ���1
	BYTE SearchThreeLineTakeOne( const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE cbLineCount ,tagSearchCardResult *pSearchCardResult );
	//����510K
	BYTE Search510K(const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE mode,BYTE color,tagSearchCardResult *pSearchCardResult);
	//�Ƿ�����
	bool IsLineCardType(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount);


	//�ڲ�����
private:
	//�����˿�
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//�����˿�
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//�����ֲ�
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing);
};

//////////////////////////////////////////////////////////////////////////////////

#endif