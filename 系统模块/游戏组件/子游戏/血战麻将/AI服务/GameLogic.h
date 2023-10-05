#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//�߼�����

#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////
//��������

//������־
#define WIK_NULL					0x00								//û������
#define WIK_LEFT					0x01								//�������
#define WIK_CENTER					0x02								//�г�����
#define WIK_RIGHT					0x04								//�ҳ�����
#define WIK_PENG					0x08								//��������
#define WIK_GANG					0x10								//��������
#define WIK_LISTEN					0x20								//��������
#define WIK_CHI_HU					0x40								//�Ժ�����
#define WIK_ZI_MO					0x80								//����

//////////////////////////////////////////////////////////////////////////
//���ƶ���

//����
#define CHK_NULL					0x00										//�Ǻ�����
#define CHK_CHI_HU					0x01										//������

//����ӷ�
#define CHR_QIANG_GANG				0x00000001									//����
#define CHR_GANG_SHANG_PAO			0x00000002									//������
#define CHR_GANG_KAI				0x00000004									//���ϻ�
#define CHR_HAIDI_LAOYUE			0x00000008									//��������

//////////////////////////////////
#define CHR_PING_HU					0x00000010									//ƽ��	
#define CHR_ZIMO					0x00000020									//����
#define CHR_MEN_QING				0x00000040									//����
#define CHR_DUI_DUI_HU				0x00000080									//�ԶԺ�
#define CHR_QING_YI_SE				0x00000100									//��һɫ
#define CHR_DAI_YAO_JIU				0x00000200									//���۾�
#define CHR_QI_XIAO_DUI				0x00000400									//��С��
#define CHR_JIN_GOUGOU				0x00000800									//�𹳹���������
#define CHR_QING_DUI				0x00001000									//��ԣ���һɫ�ĶԶԺ���
#define CHR_LONG_QI_DUI				0x00002000									//���߶�
#define CHR_QING_QI_DUI				0x00004000									//���߶ԣ���һɫ����С�ԣ�
#define CHR_QING_YAO_JIU			0x00008000									//���۾�
#define CHR_JIANG_JIN_GOUGOU		0x00010000									//���𹳹�
#define CHR_QING_JIN_GOUGOU			0x00020000									//��𹳹�
#define CHR_TIAN_HU					0x00040000									//���
#define CHR_DI_HU					0x00080000									//�غ�
#define CHR_QING_LONG_QI_DUI		0x00100000									//�����߶�
#define CHR_SHIBA_LUOHAN			0x00200000									//ʮ���޺�
#define CHR_QING_SHIBA_LUOHAN		0x00400000									//��ʮ���޺�

//////////////////////////////////////////////////////////////////////////


//��������
struct tagKindItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbCardIndex[3];						//�˿�����
	BYTE							cbValidIndex[3];					//ʵ���˿�����
};

//���ƽ��
struct tagGangCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[4];						//�˿�����
};

//��������
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//�����˿�
	BYTE							cbWeaveKind[4];						//�������
	BYTE							cbCenterCard[4];					//�����˿�
	BYTE                            cbCardData[4][4];                   //ʵ���˿�
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
	CChiHuRight & operator = ( DWORD dwRight );

	//�����
	CChiHuRight & operator &= ( DWORD dwRight );
	//�����
	CChiHuRight & operator |= ( DWORD dwRight );

	//��
	CChiHuRight operator & ( DWORD dwRight );
	CChiHuRight operator & ( DWORD dwRight ) const;

	//��
	CChiHuRight operator | ( DWORD dwRight );
	CChiHuRight operator | ( DWORD dwRight ) const;

	//���ܺ���
public:
	//�Ƿ�ȨλΪ��
	bool IsEmpty();

	//����ȨλΪ��
	void SetEmpty();

	//��ȡȨλ��ֵ
	BYTE GetRightData( DWORD dwRight[], BYTE cbMaxCount);

	//����Ȩλ��ֵ
	bool SetRightData( const DWORD dwRight[], BYTE cbRightCount);

private:
	//���Ȩλ�Ƿ���ȷ
	bool IsValidRight( DWORD dwRight );
};


//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//�˿�����

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ƺ���
public:
	//�����˿�
	void RandCardData(BYTE cbCardData[], BYTE cbMaxCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);

	//��������
public:
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�˿���Ŀ
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//����˿�
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);
	//У����Ч������
	bool IsValidHuangSanZhang(BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT], BYTE cbChangeCardCount);
	//�Զ�ѡ�����ŵ���
	void AutoHuangSanZhang(const BYTE cbCardIndex[MAX_INDEX], BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT]);

	//�ȼ�����
public:
	//�����ȼ�
	BYTE GetUserActionRank(BYTE cbUserAction);

	//�����ж�
public:
	//�����ж�
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//�����ж�
public:
	//���Ʒ���
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult);
	//�Ժ�����
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight, BYTE cbHuaZhuCard);
	//�Ƿ�����
	bool IsTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuaZhuCard);
	//�Ƿ���
	bool IsHuaZhu(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuaZhuCard);

	//ת������
public:
	//�˿�ת��
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//�˿�ת��
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//�˿�ת��
	BYTE SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//�˿�ת��
	BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);

	//��������
protected:
	//����
	bool IsMenQing(const tagAnalyseItem *pAnalyseItem);
	//�ԶԺ�
	bool IsDuiDuiHu(const tagAnalyseItem *pAnalyseItem);
	//��һɫ��
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount, const BYTE cbCurrentCard);
	//���
	bool IsQingDui(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//��С��
	bool IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//���߶�
	bool IsLongQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//���߶�
	bool IsQingQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//�����߶�
	bool IsQingLongQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//���۾�
	bool IsDaiYaoJiu(const tagAnalyseItem *pAnalyseItem);
	//���۾�
	bool IsQingYaoJiu(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//�𹳹�
	bool IsJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//��𹳹�
	bool IsQingJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//���𹳹�
	bool IsJiangJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//ʮ���޺�
	bool IsShiBaLuoHan(const tagAnalyseItem *pAnalyseItem);
	//��ʮ���޺�
	bool IsQingShiBaLuoHan(const tagAnalyseItem *pAnalyseItem);

	//�ڲ�����
private:
	//�����˿�
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);
	//����,������ֵ����
	bool SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif
