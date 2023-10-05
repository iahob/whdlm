#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once
#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//���Ͷ���
#define	CT_5K						0                                    //����
#define	CT_RS						1									 //ͬ����˳
#define	CT_SF						2									 //ͬ��˳
#define	CT_4K						3									 //����
#define	CT_FH						4									 //��«
#define	CT_FL						5									 //ͬ��
#define	CT_ST						6									 //˳��
#define	CT_3K						7									 //����
#define	CT_2P						8									 //����
#define	CT_1P						9									 //һ��
#define	CT_INVALID					11									 //��Ч����

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//�˿���Ŀ
#define FULL_CARD_COUNT				54									//ȫ���˿���Ŀ
#define LUCKYTIME_CARDDATA_COUNT	13									//ȫ���˿���Ŀ

//////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[4];					//������һ�ԣ���������������Ŀ
	BYTE							cbCardData[4][MAX_CARD_COUNT];		//�˿�����
};

//ö������
typedef struct 
{
	BYTE							cbEnumCardData[MAX_CARD_COUNT];		//������
	BYTE							cbCardType;							//����
}ENUMCARDTYPE;

//��Ϸ�߼�
class CGameLogic
{
	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR;}
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//���Ժ���
public:
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	void RandLuckyCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�
	void AnalysebCardData(BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult);
	//��ȡ����
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount = MAX_CARD_COUNT);
	//�Ƿ�������
	bool IsContainKingCard(BYTE cbCardData[], BYTE cbCardCount = MAX_CARD_COUNT);
	//�������Ƶ�����
	BYTE CalcularKingCard(BYTE cbCardData[], BYTE cbCardCount = MAX_CARD_COUNT);
	//�Ƿ�ͬ��
	bool IsCommonFlower(BYTE cbCardData[], BYTE cbCardCount = MAX_CARD_COUNT);
	//�Ƿ�˳��
	bool IsFlush(BYTE cbCardData[], BYTE cbCardCount = MAX_CARD_COUNT);
	//�Ƿ�ͬ����˳��(10,j,q,k,a,�����һ������)
	bool IsCommonFlowerBigFlush(BYTE cbCardData[], BYTE cbCardCount = MAX_CARD_COUNT);
	//�Ƿ�ͬ��˳
	bool IsCommonFlowerFlush(BYTE cbCardData[], BYTE cbCardCount = MAX_CARD_COUNT);
	//�����˿�,�����߼�ֵ�ͻ�ɫ,�����˿�
	BYTE ConstructCard(BYTE cbLogicValue, BYTE cbCardColor);
	//���������˿�
	VOID AnalyseHeldCard(BYTE cbCardData[], BYTE cbCardCount, bool bHeldFlag[MAX_CARD_COUNT], bool bMark[MAX_CARD_CT]);
	//�����´�С
	VOID AnalyseGuess(BYTE cbKeyCardData, bool bBig, GUESSRECORD &guessrecord);
	//ö������10�����г�ȡ5�ŵ����п���
	INT EnumCardDataCount(BYTE cbFirstCardData[], BYTE cbSecondCardData[]);
	//���
	VOID GetCombine(BYTE cbElementsArray[], int setLg, int nSelectCount);
	//���ö������
	VOID GetEnumType(BYTE cbElementsArray[], bool *flags, int length);
	//����ö���˿�����
	bool AnalyseEnumCard(BYTE cbFirstCardData[], BYTE cbSecondCardData[], BYTE cbEnumCardData[]);
	//���ö��������
	VOID GetEnumCardData(ENUMCARDTYPE *pEnumCardType, INT nSize);

	//��������
private:
	static  BYTE				m_cbCardData[FULL_CARD_COUNT];					     //�˿˶���

	static  BYTE				m_cbLuckyTimeCardData[LUCKYTIME_CARDDATA_COUNT];     //�˿˶���
};

//////////////////////////////////////////////////////////////////////////

#endif
