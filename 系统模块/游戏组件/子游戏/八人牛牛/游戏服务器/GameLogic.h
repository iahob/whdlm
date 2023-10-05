#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

//��ֵ����
#define	LOGIC_MASK_COLOR				0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE				0x0F								//��ֵ����

//�˿�����
#define OX_THREE_SAME					102									//��������

//����ģʽ
#define CT_CLASSIC_OX_VALUE0			0									//��ţ
#define CT_CLASSIC_OX_VALUE1			1									//ţһ
#define CT_CLASSIC_OX_VALUE2			2									//ţ��
#define CT_CLASSIC_OX_VALUE3			3									//ţ��
#define CT_CLASSIC_OX_VALUE4			4									//ţ��
#define CT_CLASSIC_OX_VALUE5			5									//ţ��
#define CT_CLASSIC_OX_VALUE6			6									//ţ��
#define CT_CLASSIC_OX_VALUE7			7									//ţ��
#define CT_CLASSIC_OX_VALUE8			8									//ţ��
#define CT_CLASSIC_OX_VALUE9			9									//ţ��
#define CT_CLASSIC_OX_VALUENIUNIU		10									//ţţ
#define CT_CLASSIC_OX_VALUE_FOURFLOWER	11									//�Ļ�ţ
#define CT_CLASSIC_OX_VALUE_FIVEFLOWER	12									//�廨ţ
#define CT_CLASSIC_OX_VALUE_SHUNZI		13									//˳��
#define CT_CLASSIC_OX_VALUE_SAMEFLOWER	14									//ͬ��
#define CT_CLASSIC_OX_VALUE_HULU		15									//��«
#define CT_CLASSIC_OX_VALUE_BOMB		16									//ը��
#define CT_CLASSIC_OX_VALUE_TONGHUASHUN	17									//ͬ��˳
#define CT_CLASSIC_OX_VALUE_FIVESNIUNIU	18									//��Сţ

//�ӱ�ģʽ
#define CT_ADDTIMES_OX_VALUE0				0									//��ţ
#define CT_ADDTIMES_OX_VALUE1				1									//ţһ
#define CT_ADDTIMES_OX_VALUE2				2									//ţ��
#define CT_ADDTIMES_OX_VALUE3				3									//ţ��
#define CT_ADDTIMES_OX_VALUE4				4									//ţ��
#define CT_ADDTIMES_OX_VALUE5				5									//ţ��
#define CT_ADDTIMES_OX_VALUE6				6									//ţ��
#define CT_ADDTIMES_OX_VALUE7				7									//ţ��
#define CT_ADDTIMES_OX_VALUE8				8									//ţ��
#define CT_ADDTIMES_OX_VALUE9				9									//ţ��
#define CT_ADDTIMES_OX_VALUENIUNIU			10									//ţţ
#define CT_ADDTIMES_OX_VALUE_FOURFLOWER		11									//�Ļ�ţ
#define CT_ADDTIMES_OX_VALUE_FIVEFLOWER		12									//�廨ţ
#define CT_ADDTIMES_OX_VALUE_SHUNZI			13									//˳��
#define CT_ADDTIMES_OX_VALUE_SAMEFLOWER		14									//ͬ��
#define CT_ADDTIMES_OX_VALUE_HULU			15									//��«
#define CT_ADDTIMES_OX_VALUE_BOMB			16									//ը��
#define CT_ADDTIMES_OX_VALUE_TONGHUASHUN	17									//ͬ��˳
#define CT_ADDTIMES_OX_VALUE_FIVESNIUNIU	18									//��Сţ

#define MAX_POOL_SIZE	54
//////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[4];					//�˿���Ŀ
	BYTE							cbCardData[4][MAX_CARDCOUNT];			//�˿�����
	BYTE							cbKingCount;
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
private:
	BYTE							m_cbPool[MAX_POOL_SIZE];
	BYTE							m_cbPoolSize;

	//��������
public:
	static BYTE						m_cbCardListDataNoKing[52];					//�˿˶���
	static BYTE						m_cbCardListDataHaveKing[54];				//�˿˶���
	BYTE							m_cbCardTypeTimes[MAX_CARD_TYPE];			//���ͱ���
	BYTE							m_cbEnableCardType[MAX_SPECIAL_CARD_TYPE];	//���ͼ���

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig = CT_CLASSIC_);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }
	//��ȡ����
	BYTE GetTimes(BYTE cbCardData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig = CT_CLASSIC_, BYTE cbCombineCardType = INVALID_BYTE);
	//�������ͱ���
	void SetCardTypeTimes(BYTE cbCardTypeTimes[MAX_CARD_TYPE]);
	//���ü�����������
	void SetEnableCardType(BYTE cbEnableCardType[MAX_SPECIAL_CARD_TYPE]);

	//���Ժ���
public:
	//�����˿�(ͨ��ţţ����ʹ��)
	void SortNNCardList(BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�(�¼�����ʹ��)
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, bool bDescend = true);
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, bool bHaveKing = false);
	//���ܺ���
public:
	//�߼���ֵ(ͨ��ţţ����ʹ��)
	BYTE GetNNCardLogicValue(BYTE cbCardData);
	//�߼���ֵ(�¼�����ʹ��)
	BYTE GetCardLogicValue(BYTE cbCardData);
	//�Ա��˿�
	bool CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig = CT_CLASSIC_, BYTE cbFirstTypeEX = INVALID_BYTE, BYTE cbNextTypeEX = INVALID_BYTE);
	//��ȡţţ
	bool GetOxCard(BYTE cbCardData[], BYTE cbCardCount);
	//��������
public:
	//�����˿�
	bool AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//�ж���Сţ
	bool IsFiveSNiuNiu(BYTE cbCardData[], BYTE cbCardCount);
	//�ж�ͬ��˳
	bool IsTongHuaShun(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[]);
	//�ж�ͬ��
	bool IsTongHua(BYTE cbCardData[], BYTE cbCardCount);
	//�ж�˳��
	bool IsShunZi(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[]);
	//�ж�ը��
	bool IsBomb(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[]);
	//�жϺ�«
	bool IsHuLu(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[]);
	//�ж��廨ţ
	bool IsFiveFlowerNN(BYTE cbCardData[], BYTE cbCardCount);
	//�ж��Ļ�ţ
	bool IsFourFlowerNN(BYTE cbCardData[], BYTE cbCardCount);
	//�Ƿ�������
	bool IsContainKingCard(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��һ�����Ƶ�����
	BYTE GetKingCardIndex(BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�
	BYTE ConstructCard(BYTE cbCardLogicValue, BYTE cbCardColor);
	//��ȡ��������
	BYTE GetKingCount(BYTE cbCardData[], BYTE cbCardCount);
	//�Ƿ���A��˳��
	bool IsContainAShunZi(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[]);
	//�������ͣ���ȷ��ǰ��4���ƣ����һ���ƿɱ�Ź�������,�������һ���ƣ� INVALID_BYTEΪ�޷�����
	BYTE ConstructCardType(CList<BYTE, BYTE&> &cardlist, BYTE cbConstructCardData[MAX_CARDCOUNT], BYTE cbCardType, KING_CONFIG gtConfig);
	//ɾ��Ŀ������
	bool RemoveKeyCard(CList<BYTE, BYTE&> &cardlist, BYTE cbKeyCard);
	//��ȡ�������������
	void GetSpecialSortCard(BYTE cbCardType, BYTE cbHandCardData[MAX_CARDCOUNT], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig);
	//���ĵ���ģʽ�£�������������֪��������������������������
	BYTE GetMaxCardTypeSendFourRealPlayer(CList<BYTE, BYTE &> &cardlist, BYTE cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT], CARDTYPE_CONFIG ctConfig, ITableFrame *pITableFrame, BYTE cbPlayStatus[GAME_PLAYER], BYTE cbDynamicJoin[GAME_PLAYER], WORD wPlayerCount);
	//���ĵ���ģʽ�£����������֪��������������������������
	BYTE GetMaxCardTypeSingle(CList<BYTE, BYTE &> &cardlist, BYTE cbCardData[MAX_CARDCOUNT], CARDTYPE_CONFIG ctConfig);

public:
	void RandPool(bool bHaveKing);
	bool GetCards(BYTE* pCards,BYTE cbGetCount);
	BYTE GetRandCard();
	void AddCard(BYTE cbCard);
	void AddCards(BYTE* pCards, BYTE cbGetCount);
	void RemoveCard(BYTE cbCard);
	void RemoveCards(BYTE* pCards, BYTE cbGetCount);
	BYTE* GetPoolData(){ return m_cbPool; };
	BYTE GetPoolLeftCount(){ return m_cbPoolSize; }

};

//////////////////////////////////////////////////////////////////////////

#endif
