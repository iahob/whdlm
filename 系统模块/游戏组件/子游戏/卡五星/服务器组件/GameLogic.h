#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE
#pragma once
#pragma pack(1)
#include "Stdafx.h"

#pragma warning(disable : 4244)

//////////////////////////////////////////////////////////////////////////

//���ڲ����ת����������ƿ��Դ����������ʹ�ã�����Ϊ����������������ΪMAX_INDEX. ע:����滻����������,������.
#define	INDEX_REPLACE_CARD					MAX_INDEX

//////////////////////////////////////////////////////////////////////////
//�߼�����

#define	MASK_COLOR				0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////

#define WIK_GANERAL					0x00								//��ͨ����
#define WIK_MING_GANG				0x01								//���ܣ������ٸܣ�
#define WIK_FANG_GANG				0x02								//�Ÿ�
#define WIK_AN_GANG					0x03								//����

//������־
#define WIK_NULL					0x00								//û������
#define WIK_LEFT					0x01								//�������
#define WIK_CENTER				0x02								//�г�����
#define WIK_RIGHT				0x04								//�ҳ�����
#define WIK_PENG					0x08								//��������
#define WIK_GANG				0x10								//��������
#define WIK_LISTEN				0x20								//��������
#define WIK_CHI_HU				0x40								//�Ժ�����

//////////////////////////////////////////////////////////////////////////
//���ƶ���
//	1��
#define CHR_ZI_MO						0x00000001									//����
#define CHR_PING_HU					0x00000002									//ƽ��
//	2��
#define CHR_PENG_PENG				0x00000004									//������
#define CHR_SHI_GUI_YI_MING		0x00000008									//���Ĺ�һ
#define CHR_QIANG_GANG			0x00000010									//���ܺ�
#define CHR_GANG_KAI					0x00000020									//���Ͽ���
#define CHR_HAI_DI						0x00000040									//��������
#define CHR_LIANG_DAO				0x00000080									//����
#define CHR_KA_5_XING				0x00000100									//������
#define CHR_GANG_PAO				0x00000200									//������
//	4��
#define CHR_QI_DUI						0x00000400									//�߶�
#define CHR_XIAO_SAN_YUAN		0x00000800									//С��Ԫ
#define CHR_QUAN_QIU_REN		0x00001000									//��ץһ
#define CHR_QING_YI_SE				0x00002000									//��һɫ
#define CHR_SHI_GUI_YI_AN			0x00004000									//���Ĺ�һ
//	8��
#define CHR_LONG_QI_DUI			0x00008000									//���߶�
#define CHR_DA_SAN_YUAN			0x00010000									//����Ԫ

//	128��
#define CHR_HAOHUA_LONG_QI	0x00020000									//�������߶�

//////////////////////////////////////////////////////////////////////////

//��������
struct tagKindItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbValidIndex[3];					//ʵ���˿�����
};

//�������
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbPublicCard;						//������־
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbCardData[4];						//
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
	bool                            bMagicEye;                          //�����Ƿ�������
	BYTE							cbWeaveKind[MAX_WEAVE];				//�������
	BYTE							cbCenterCard[MAX_WEAVE];			//�����˿�
	BYTE                            cbCardData[MAX_WEAVE][4];           //ʵ���˿�
};
#pragma pack()
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//�˿�����
	BYTE							m_cbMagicIndex;						//��������
	BOOL							m_bOpenSiGuiYi;						//�Ƿ����Ĺ�һ�淨
	static const BYTE				m_cbCardDebugDataArray[MAX_REPERTORY];	//�˿�����

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
	//�����˿�
	void RandCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbRandData[]);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//���ò���
	void SetMagicIndex( BYTE cbMagicIndex ) { m_cbMagicIndex = cbMagicIndex; }
	//�����ж�
	bool IsMagicCard( BYTE cbCardData );
	//�����ж�
	bool IsHuaCard( BYTE cbCardData );
	//�����Ĺ�һ
	void SetSiGuiYi(BOOL bOpenSiGuiYi){m_bOpenSiGuiYi = bOpenSiGuiYi;}
	//��������
public:
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�˿���Ŀ
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//����˿�
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);

	//�ȼ�����
public:
	//�����ȼ�
	BYTE GetUserActionRank(BYTE cbUserAction);
	//���Ƶȼ�
	WORD GetChiHuActionRank(DWORD & ChiHuRight);

	//�����ж�
public:
	//�����ж�
	BYTE EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//�����ж�
public:
	//���Ʒ���
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult);
	//���Ʒ���
	BYTE AnalyseGangCardEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbProvideCard, tagGangCardResult & GangCardResult);
	//�Ժ�����
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, DWORD &ChiHuRight);
	//���Ʒ���
	BYTE AnalyseTingCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//���Ʒ���
	BYTE GangCardType(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//��ȡ��������
	BYTE GetHuCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[]);
	//��ȡ�������ݣ�����Щ���������Լ��ܺ���Щ��
	BYTE GetTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount, BYTE cbOutCardData[], BYTE cbHuCardCount[], BYTE cbHuCardData[][HEAP_FULL_COUNT], BYTE cbHuFan[][HEAP_FULL_COUNT]);

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
	//��С����
	bool IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[],const  BYTE cbWeaveCount,const BYTE cbCurrentCard);

	//��������
protected:
	//������
	bool IsDanDiaoJiang( const tagAnalyseItem *pAnalyseItem, BYTE cbCurrentCard );
	//����
	bool IsKanZhang( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard );
	//����
	bool IsBianZhang( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard );
	//����
	bool IsWuZi( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//ȱһ��
	bool IsQueYiMen( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//����
	bool IsMingGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//�۾ſ�
	bool IsYaoJiuKe( const tagAnalyseItem *pAnalyseItem );
	//���ٸ�
	bool IsLaoShaoFu( const tagAnalyseItem *pAnalyseItem );
	//����
	bool IsLianLu( const tagAnalyseItem *pAnalyseItem );
	//ϲ���
	bool IsXiXiangFeng( const tagAnalyseItem *pAnalyseItem );
	//һ���
	bool IsYiBanGao( const tagAnalyseItem *pAnalyseItem );
	//����
	bool IsDuanYao( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//����
	bool IsAnGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//˫����
	bool IsShuangAnKe( const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//˫ͬ��
	bool IsShuangTongKe( const tagAnalyseItem *pAnalyseItem );
	//ƽ��
	bool IsPingHu( const tagAnalyseItem *pAnalyseItem );
	//��ǰ��
	bool IsMenQianQing( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//ȫ����
	bool IsQuanQiuRen( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//����
	bool IsJianKe( const tagAnalyseItem *pAnalyseItem );
	//˫����
	bool IsShuangMingGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//������
	bool IsBuQiuRen( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//ȫ����
	bool IsQuanDaiYao( const tagAnalyseItem *pAnalyseItem );
	//˫����
	bool IsShuangJianKe( const tagAnalyseItem *pAnalyseItem );
	//˫����
	bool IsShuangAnGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//������
	bool IsWuMenQi( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//��ɫ������
	bool IsSanSeBu( const tagAnalyseItem *pAnalyseItem );
	//��һɫ
	bool IsHunYiSe( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//������
	bool IsPengPeng( const tagAnalyseItem *pAnalyseItem );
	//��ɫ���ڸ�
	bool IsSanSeJie( const tagAnalyseItem *pAnalyseItem );
	//��ɫ��ͬ˳
	bool IsSanSeTong( const tagAnalyseItem *pAnalyseItem );
	//����
	bool IsHuaLong( const tagAnalyseItem *pAnalyseItem );
	//�����
	bool IsSanFengKe( const tagAnalyseItem *pAnalyseItem );
	//С����
	bool IsXiaoWu( const tagAnalyseItem *pAnalyseItem );
	//������
	bool IsDaWu( const tagAnalyseItem *pAnalyseItem );
	//�İ���
	bool IsSiAnKe( const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//һɫ������
	bool IsYiSeBu(const tagAnalyseItem & AnalyseItem);
	//ȫ��5
	bool IsQuanDaiWu(const tagAnalyseItem & AnalyseItem);
	//��ͬ��
	bool IsSanTongKe(const tagAnalyseItem & AnalyseItem);
	//������
	bool IsSanAnKe(const tagAnalyseItem & AnalyseItem,const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//����ϲ
	bool IsDaSiXi(const tagAnalyseItem & AnalyseItem);
	//����Ԫ
	bool IsDaSanYuan(const tagAnalyseItem & AnalyseItem);
	//�ĸ�
	bool IsSiGang(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount);
	//���۾�
	bool IsQingYaoJiu(const tagAnalyseItem &AnalyseItem);
	//С��ϲ
	bool IsXiaoSiXi(const tagAnalyseItem & AnalyseItem);
	// С��Ԫ
	bool IsXiaoShanYuan(const tagAnalyseItem & AnalyseItem);
	//��һɫ��
	bool IsZiYiSe(const tagAnalyseItem & AnalyseItem);
	// һɫ˫����
	bool IsYiSeHuangHuiLong(const tagAnalyseItem & AnalyseItem);
	//һɫ��ͬ˳
	bool IsYiSeSiTongShun(const tagAnalyseItem &AnalyseItem);
	//һɫ�Ľڸ�
	bool IsYiSeSiJieGao(const tagAnalyseItem &AnalyseItem);
	//һɫ�Ĳ���
	bool IsYiSeSiBuGao(const tagAnalyseItem &AnalyseItem);
	//����
	bool IsSanGang(const tagWeaveItem WeaveItem[],const  BYTE cbWeaveCount);
	//���۾�
	bool IsHunYaoJiu(const tagAnalyseItem &AnalyseItem);
	//��һɫ��
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount,const BYTE cbCurrentCard);	
	//ȫ˫
	bool IsQuanShuang(const tagAnalyseItem &AnalyseItem);
	//һɫ��ͬ˳
	bool IsYiSeSanTongShun(const tagAnalyseItem &AnalyseItem);
	//һɫ���ڸ�
	bool IsYiSeSanJieGao(const tagAnalyseItem &AnalyseItem);
	//ȫ��
	bool IsQuanDa(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	//ȫ��
	bool IsQuanZhong(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	//ȫС
	bool IsQuanXiao(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	//�Ʋ���
	bool IsTuiBuDao( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//�����
	bool IsZuHeLong( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//ȫ����
	bool IsQuanBuKao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//��ɫ˫����
	bool IsSanSeShuangLong( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//����
	bool IsQingLong( const tagAnalyseItem & AnalyseItem );
	//���ǲ���
	bool IsQiXingBuKao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//ʮ����
	bool IsShiSanYao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//��������
	bool IsJiuLianBaoDeng( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//��һɫ
	bool IsLuYiSe( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//�Ĺ�һ
	bool IsMingSiGuiYi( const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard,bool bQingYiSe);
	//�Ĺ�һ
	bool IsAnSiGuiYi( const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard,bool bQingYiSe);
	//���߶�
	bool IsLianQiDui( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//��5��
	bool IsKaWuXing( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard );

	//�ڲ�����
public:
	//�����˿�
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);
	//����,������ֵ����
	bool SortCardList( BYTE cbCardData[MAX_COUNT], BYTE cbCardCount );
};

//////////////////////////////////////////////////////////////////////////

#endif
