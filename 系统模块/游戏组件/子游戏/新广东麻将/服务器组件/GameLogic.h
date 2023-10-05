#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
//////////////////////////////////////////////////////////////////////////

//���ڲ����ת����������ƿ��Դ����������ʹ�ã�����Ϊ����������������ΪMAX_INDEX. ע:����滻����������,������.
#define	INDEX_REPLACE_CARD			MAX_INDEX

//////////////////////////////////////////////////////////////////////////
//�߼�����

#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#define WIK_GANERAL					0x00								//��ͨ����
#define WIK_MING_GANG				0x01								//���ܣ������ٸܣ�
#define WIK_FANG_GANG				0x02								//�Ÿ�
#define WIK_AN_GANG					0x03								//����

//������־
#define WIK_NULL					0x00								//û������
#define WIK_LEFT					0x01								//�������
#define WIK_CENTER					0x02								//�г�����
#define WIK_RIGHT					0x04								//�ҳ�����
#define WIK_PENG					0x08								//��������
#define WIK_GANG					0x10								//��������
#define WIK_LISTEN					0x20								//��������
#define WIK_CHI_HU					0x40								//�Ժ�����
#define WIK_FANG_PAO				0x80								//����

//////////////////////////////////////////////////////////////////////////
//���ƶ���
#define CHK_MASK_SMALL				0x000000FF							//С������
#define CHK_MASK_BIG				0xFFFFFF00							//�������

//С������
#define CHR_PING_HU					0x00000001							//ƽ��
#define CHR_QIANG_GANG_HU			0x00000002							//���ܺ�

//�������
#define CHR_PENG_PENG				0x00000200							//������
#define CHR_QI_DUI					0x00000400							//�߶�
#define CHR_TIAN_HU					0x00000800							//���
#define CHR_DI_HU					0x00001000							//�غ�
#define CHR_QING_YI_SE				0x00002000							//��һɫ
#define CHR_ZI_YI_SE				0x00004000							//��һɫ
#define CHR_HUN_YI_SE				0x00008000							//��һɫ
#define CHR_XIAO_SAN_YUAN			0x00010000							//С��Ԫ
#define CHR_DA_SAN_YUAN				0x00020000							//����Ԫ
#define CHR_XIAO_SI_XI				0x00040000							//С��ϲ	
#define CHR_DA_SI_XI				0x00080000							//����ϲ
#define CHR_SI_AN_KE				0x00100000							//�İ���
#define CHR_LUO_HAN					0x00200000							//ʮ���޺�	
#define CHR_SHI_SAN_YAO				0x00400000							//ʮ����
//δʹ��

#define CHR_GANG_SHANG_HUA			0x00000004                          //���ϻ�
#define CHR_GANG_SHANG_PAO			0x00000008                          //������
#define CHR_SHI_SAN_LAN				0x04000000							//ʮ����


//////////////////////////////////////////////////////////////////////////////////

//��������
struct tagKindItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbValidIndex[3];					//ʵ���˿�����
	BYTE							cbMagicCount;						//��������
};

//���ƽ��
struct tagGangCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[MAX_WEAVE];				//�˿�����
};


//��������
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//�����˿�
	bool							bMagicEye;                          //�����Ƿ�������
	BYTE							cbWeaveKind[MAX_WEAVE];				//�������
	BYTE						    cbCenterCard[MAX_WEAVE];			//�����˿�
	BYTE							cbCardData[MAX_WEAVE][4];			//ʵ���˿�
};


//////////////////////////////////////////////////////////////////////////


#define MASK_CHI_HU_RIGHT			0x0fffffff

/*
//	Ȩλ�ࡣ
//  ע�⣬�ڲ�����λʱ���ֻ��������Ȩλ.����
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)������������޶���ġ�
//  ֻ�ܵ�������:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{	
	//��̬����
private:
	static bool						m_bInit;
	static DWORD					m_dwRightMask[MAX_RIGHT_COUNT];

	//Ȩλ����
private:
	DWORD							m_dwRight[MAX_RIGHT_COUNT];

public:
	//���캯��
	CChiHuRight();

	//���������
public:
	//��ֵ��
	CChiHuRight & operator = (DWORD dwRight);

	//�����
	CChiHuRight & operator &= (DWORD dwRight);
	//�����
	CChiHuRight & operator |= (DWORD dwRight);

	//��
	CChiHuRight operator & (DWORD dwRight);
	CChiHuRight operator & (DWORD dwRight) const;

	//��
	CChiHuRight operator | (DWORD dwRight);
	CChiHuRight operator | (DWORD dwRight) const;

	//���
	bool operator == (DWORD dwRight) const;
	bool operator == (const CChiHuRight chr) const;

	//�����
	bool operator != (DWORD dwRight) const;
	bool operator != (const CChiHuRight chr) const;

	//���ܺ���
public:
	//�Ƿ�ȨλΪ��
	bool IsEmpty();

	//����ȨλΪ��
	void SetEmpty();

	//��ȡȨλ��ֵ
	BYTE GetRightData(DWORD dwRight[], BYTE cbMaxCount);

	//����Ȩλ��ֵ
	bool SetRightData(const DWORD dwRight[], BYTE cbRightCount);

private:
	//���Ȩλ�Ƿ���ȷ
	bool IsValidRight(DWORD dwRight);
};

//////////////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE					m_cbCardDataArray[MAX_REPERTORY];			//�˿�����
	BYTE								m_cbMagicIndex[2];							//��������
	BOOL								m_bHuQiDui;									//�ܷ���߶�
	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

public:
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	VOID RandCardList(BYTE cbCardData[], BYTE cbCardBuffer[], BYTE cbBufferCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//����,������ֵ����
	bool SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//���ò���
	void SetMagicIndex( BYTE cbMagicIndex[] ); 
	//�����ж�
	bool IsMagicCard(BYTE cbCardData);
	//��ȡ�������
	BYTE GetMajicCount(const BYTE cbCardIndex[MAX_INDEX]);
	//�����ж�
	bool IsHuaCard(BYTE cbCardData);
	//�����ж�
	BYTE IsHuaCard(BYTE cbCardIndex[MAX_INDEX]);
	//�����Զ������
	VOID SetUserRule(BOOL bHuQiDui);

	//�ȼ�����
public:
	//�����ȼ�
	BYTE GetUserActionRank(BYTE cbUserAction);
	//���Ƶȼ�
	WORD GetChiHuActionRank(const CChiHuRight & ChiHuRight);

	//�Զ�����
	BYTE AutomatismOutCard(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbEnjoinOutCard[MAX_COUNT], BYTE cbEnjoinOutCardCount);

	//�����ж�
public:
	//�����ж�
	BYTE EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

public://�����ж�
	//���Ʒ���
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbProvideCard, tagGangCardResult & GangCardResult);
	//�Ժ�����
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight);
	//���Ʒ���
	BYTE AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//��ȡ�������ݣ����ƺ����
	BYTE GetHuCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[]);
	//��ȡ��������,����Щ������
	BYTE GetTingData(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE& cbOutCardCount, BYTE cbOutCardData[]);
	//��ȡ�������ݣ�����Щ���������Լ��ܺ���Щ��
	BYTE GetTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount,BYTE cbOutCardData[],BYTE cbHuCardCount[],BYTE cbHuCardData[][HEAP_FULL_COUNT],BYTE cbHuFan[][HEAP_FULL_COUNT]);

public://ת������
	//�˿�ת��
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//�˿�ת��
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//�˿�ת��
	BYTE SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//�˿�ת��
	BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);

public://��������
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�˿���Ŀ
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);	
	//����˿�
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);

private://�ڲ�����
	bool AddKindItem(tagKindItem &TempKindItem, tagKindItem KindItem[], BYTE &cbKindItemCount, bool &bMagicThree);
	//�����˿�
	bool AnalyseCard(const BYTE cbCardIndex1[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount, CAnalyseItemArray & AnalyseItemArray);

protected://��������

	//ƽ��
	bool IsPingHu(const tagAnalyseItem *pAnalyseItem);
	//����
	bool IsJiHu(const tagAnalyseItem *pAnalyseItem);
	//��һɫ
	bool IsQingYiSe(const tagAnalyseItem * pAnalyseItem);	
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//��һɫ
	bool IsHunYiSe(const tagAnalyseItem * pAnalyseItem);	
	bool IsHunYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//��һɫ��
	bool IsZiYiSe(tagAnalyseItem * pAnalyseItem);
	bool IsZiYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//����Ԫ
	bool IsDaSanYuan(tagAnalyseItem * pAnalyseItem);
	//С��Ԫ
	bool IsXiaoSanYuan(const BYTE cbCardIndex[MAX_INDEX],tagAnalyseItem * pAnalyseItem);
	//����ϲ
	bool IsDaSiXi(tagAnalyseItem * pAnalyseItem);
	//С��ϲ
	bool IsXiaoSiXi(tagAnalyseItem * pAnalyseItem);
	//ʮ����
	bool IsShiSanYao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//������
	bool IsPengPeng(const tagAnalyseItem *pAnalyseItem);
	//�߶�ϵ��
	bool IsQiDui(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount);
	//ʮ����ϵ��
	bool IsShiSanLan(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount);

};

//////////////////////////////////////////////////////////////////////////////////

#endif