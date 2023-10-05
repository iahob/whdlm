#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE
#pragma once
#pragma pack(1)
#include "Stdafx.h"

#pragma warning(disable : 4244)

//////////////////////////////////////////////////////////////////////////

//用于财神的转换，如果有牌可以代替财神本身牌使用，则设为该牌索引，否则设为MAX_INDEX. 注:如果替换牌是序数牌,将出错.
#define	INDEX_REPLACE_CARD					MAX_INDEX

//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR				0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////

#define WIK_GANERAL					0x00								//普通操作
#define WIK_MING_GANG				0x01								//明杠（碰后再杠）
#define WIK_FANG_GANG				0x02								//放杠
#define WIK_AN_GANG					0x03								//暗杠

//动作标志
#define WIK_NULL					0x00								//没有类型
#define WIK_LEFT					0x01								//左吃类型
#define WIK_CENTER				0x02								//中吃类型
#define WIK_RIGHT				0x04								//右吃类型
#define WIK_PENG					0x08								//碰牌类型
#define WIK_GANG				0x10								//杠牌类型
#define WIK_LISTEN				0x20								//吃牌类型
#define WIK_CHI_HU				0x40								//吃胡类型

//////////////////////////////////////////////////////////////////////////
//胡牌定义
//	1番
#define CHR_ZI_MO						0x00000001									//自摸
#define CHR_PING_HU					0x00000002									//平和
//	2番
#define CHR_PENG_PENG				0x00000004									//碰碰和
#define CHR_SHI_GUI_YI_MING		0x00000008									//明四归一
#define CHR_QIANG_GANG			0x00000010									//抢杠和
#define CHR_GANG_KAI					0x00000020									//杠上开花
#define CHR_HAI_DI						0x00000040									//海底捞月
#define CHR_LIANG_DAO				0x00000080									//亮倒
#define CHR_KA_5_XING				0x00000100									//卡五星
#define CHR_GANG_PAO				0x00000200									//杠上炮
//	4番
#define CHR_QI_DUI						0x00000400									//七对
#define CHR_XIAO_SAN_YUAN		0x00000800									//小三元
#define CHR_QUAN_QIU_REN		0x00001000									//手抓一
#define CHR_QING_YI_SE				0x00002000									//清一色
#define CHR_SHI_GUI_YI_AN			0x00004000									//暗四归一
//	8番
#define CHR_LONG_QI_DUI			0x00008000									//龙七对
#define CHR_DA_SAN_YUAN			0x00010000									//大三元

//	128番
#define CHR_HAOHUA_LONG_QI	0x00020000									//豪华龙七对

//////////////////////////////////////////////////////////////////////////

//类型子项
struct tagKindItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbValidIndex[3];					//实际扑克索引
};

//组合子项
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
	BYTE							cbCardData[4];						//
};

//杠牌结果
struct tagGangCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_WEAVE];				//扑克数据
};

//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼扑克
	bool                            bMagicEye;                          //牌眼是否是王霸
	BYTE							cbWeaveKind[MAX_WEAVE];				//组合类型
	BYTE							cbCenterCard[MAX_WEAVE];			//中心扑克
	BYTE                            cbCardData[MAX_WEAVE][4];           //实际扑克
};
#pragma pack()
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//扑克数据
	BYTE							m_cbMagicIndex;						//钻牌索引
	BOOL							m_bOpenSiGuiYi;						//是否开启四归一玩法
	static const BYTE				m_cbCardDebugDataArray[MAX_REPERTORY];	//扑克数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//控制函数
public:
	//混乱扑克
	void RandCardData(BYTE cbCardData[], BYTE cbMaxCount);
	//混乱扑克
	void RandCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbRandData[]);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//设置财神
	void SetMagicIndex( BYTE cbMagicIndex ) { m_cbMagicIndex = cbMagicIndex; }
	//财神判断
	bool IsMagicCard( BYTE cbCardData );
	//花牌判断
	bool IsHuaCard( BYTE cbCardData );
	//设置四归一
	void SetSiGuiYi(BOOL bOpenSiGuiYi){m_bOpenSiGuiYi = bOpenSiGuiYi;}
	//辅助函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//扑克数目
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//组合扑克
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);

	//等级函数
public:
	//动作等级
	BYTE GetUserActionRank(BYTE cbUserAction);
	//胡牌等级
	WORD GetChiHuActionRank(DWORD & ChiHuRight);

	//动作判断
public:
	//吃牌判断
	BYTE EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌判断
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌判断
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//动作判断
public:
	//杠牌分析
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult);
	//杠牌分析
	BYTE AnalyseGangCardEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbProvideCard, tagGangCardResult & GangCardResult);
	//吃胡分析
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, DWORD &ChiHuRight);
	//听牌分析
	BYTE AnalyseTingCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//杠牌分析
	BYTE GangCardType(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//获取胡牌数据
	BYTE GetHuCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[]);
	//获取辅助数据，打哪些牌能听，以及能胡哪些牌
	BYTE GetTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount, BYTE cbOutCardData[], BYTE cbHuCardCount[], BYTE cbHuCardData[][HEAP_FULL_COUNT], BYTE cbHuFan[][HEAP_FULL_COUNT]);

	//转换函数
public:
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//扑克转换
	BYTE SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//扑克转换
	BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);
	//七小对牌
	bool IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[],const  BYTE cbWeaveCount,const BYTE cbCurrentCard);

	//胡法分析
protected:
	//单钓将
	bool IsDanDiaoJiang( const tagAnalyseItem *pAnalyseItem, BYTE cbCurrentCard );
	//坎张
	bool IsKanZhang( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard );
	//边张
	bool IsBianZhang( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard );
	//无字
	bool IsWuZi( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//缺一门
	bool IsQueYiMen( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//明杠
	bool IsMingGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//幺九刻
	bool IsYaoJiuKe( const tagAnalyseItem *pAnalyseItem );
	//老少副
	bool IsLaoShaoFu( const tagAnalyseItem *pAnalyseItem );
	//连六
	bool IsLianLu( const tagAnalyseItem *pAnalyseItem );
	//喜相逢
	bool IsXiXiangFeng( const tagAnalyseItem *pAnalyseItem );
	//一般高
	bool IsYiBanGao( const tagAnalyseItem *pAnalyseItem );
	//断幺
	bool IsDuanYao( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//暗杠
	bool IsAnGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//双暗刻
	bool IsShuangAnKe( const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//双同刻
	bool IsShuangTongKe( const tagAnalyseItem *pAnalyseItem );
	//平胡
	bool IsPingHu( const tagAnalyseItem *pAnalyseItem );
	//门前清
	bool IsMenQianQing( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//全求人
	bool IsQuanQiuRen( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//箭刻
	bool IsJianKe( const tagAnalyseItem *pAnalyseItem );
	//双明杠
	bool IsShuangMingGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//不求人
	bool IsBuQiuRen( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//全带幺
	bool IsQuanDaiYao( const tagAnalyseItem *pAnalyseItem );
	//双箭刻
	bool IsShuangJianKe( const tagAnalyseItem *pAnalyseItem );
	//双暗杠
	bool IsShuangAnGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//五门齐
	bool IsWuMenQi( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//三色三步高
	bool IsSanSeBu( const tagAnalyseItem *pAnalyseItem );
	//混一色
	bool IsHunYiSe( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard );
	//碰碰和
	bool IsPengPeng( const tagAnalyseItem *pAnalyseItem );
	//三色三节高
	bool IsSanSeJie( const tagAnalyseItem *pAnalyseItem );
	//三色三同顺
	bool IsSanSeTong( const tagAnalyseItem *pAnalyseItem );
	//花龙
	bool IsHuaLong( const tagAnalyseItem *pAnalyseItem );
	//三风刻
	bool IsSanFengKe( const tagAnalyseItem *pAnalyseItem );
	//小于五
	bool IsXiaoWu( const tagAnalyseItem *pAnalyseItem );
	//大于五
	bool IsDaWu( const tagAnalyseItem *pAnalyseItem );
	//四暗刻
	bool IsSiAnKe( const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//一色三步高
	bool IsYiSeBu(const tagAnalyseItem & AnalyseItem);
	//全带5
	bool IsQuanDaiWu(const tagAnalyseItem & AnalyseItem);
	//三同刻
	bool IsSanTongKe(const tagAnalyseItem & AnalyseItem);
	//三暗刻
	bool IsSanAnKe(const tagAnalyseItem & AnalyseItem,const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//大四喜
	bool IsDaSiXi(const tagAnalyseItem & AnalyseItem);
	//大三元
	bool IsDaSanYuan(const tagAnalyseItem & AnalyseItem);
	//四杠
	bool IsSiGang(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount);
	//清幺九
	bool IsQingYaoJiu(const tagAnalyseItem &AnalyseItem);
	//小四喜
	bool IsXiaoSiXi(const tagAnalyseItem & AnalyseItem);
	// 小三元
	bool IsXiaoShanYuan(const tagAnalyseItem & AnalyseItem);
	//字一色牌
	bool IsZiYiSe(const tagAnalyseItem & AnalyseItem);
	// 一色双龙会
	bool IsYiSeHuangHuiLong(const tagAnalyseItem & AnalyseItem);
	//一色四同顺
	bool IsYiSeSiTongShun(const tagAnalyseItem &AnalyseItem);
	//一色四节高
	bool IsYiSeSiJieGao(const tagAnalyseItem &AnalyseItem);
	//一色四步高
	bool IsYiSeSiBuGao(const tagAnalyseItem &AnalyseItem);
	//三杠
	bool IsSanGang(const tagWeaveItem WeaveItem[],const  BYTE cbWeaveCount);
	//混幺九
	bool IsHunYaoJiu(const tagAnalyseItem &AnalyseItem);
	//清一色牌
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount,const BYTE cbCurrentCard);	
	//全双
	bool IsQuanShuang(const tagAnalyseItem &AnalyseItem);
	//一色三同顺
	bool IsYiSeSanTongShun(const tagAnalyseItem &AnalyseItem);
	//一色三节高
	bool IsYiSeSanJieGao(const tagAnalyseItem &AnalyseItem);
	//全大
	bool IsQuanDa(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	//全中
	bool IsQuanZhong(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	//全小
	bool IsQuanXiao(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	//推不倒
	bool IsTuiBuDao( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//组合龙
	bool IsZuHeLong( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//全不靠
	bool IsQuanBuKao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//三色双龙会
	bool IsSanSeShuangLong( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//清龙
	bool IsQingLong( const tagAnalyseItem & AnalyseItem );
	//七星不靠
	bool IsQiXingBuKao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//十三幺
	bool IsShiSanYao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//九莲宝灯
	bool IsJiuLianBaoDeng( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//绿一色
	bool IsLuYiSe( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//四归一
	bool IsMingSiGuiYi( const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard,bool bQingYiSe);
	//四归一
	bool IsAnSiGuiYi( const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard,bool bQingYiSe);
	//连七对
	bool IsLianQiDui( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard );
	//卡5星
	bool IsKaWuXing( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard );

	//内部函数
public:
	//分析扑克
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);
	//排序,根据牌值排序
	bool SortCardList( BYTE cbCardData[MAX_COUNT], BYTE cbCardCount );
};

//////////////////////////////////////////////////////////////////////////

#endif
