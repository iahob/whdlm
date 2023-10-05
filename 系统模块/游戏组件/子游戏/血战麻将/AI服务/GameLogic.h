#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//动作定义

//动作标志
#define WIK_NULL					0x00								//没有类型
#define WIK_LEFT					0x01								//左吃类型
#define WIK_CENTER					0x02								//中吃类型
#define WIK_RIGHT					0x04								//右吃类型
#define WIK_PENG					0x08								//碰牌类型
#define WIK_GANG					0x10								//杠牌类型
#define WIK_LISTEN					0x20								//吃牌类型
#define WIK_CHI_HU					0x40								//吃胡类型
#define WIK_ZI_MO					0x80								//自摸

//////////////////////////////////////////////////////////////////////////
//胡牌定义

//胡牌
#define CHK_NULL					0x00										//非胡类型
#define CHK_CHI_HU					0x01										//胡类型

//另外加番
#define CHR_QIANG_GANG				0x00000001									//抢杠
#define CHR_GANG_SHANG_PAO			0x00000002									//杠上炮
#define CHR_GANG_KAI				0x00000004									//杠上花
#define CHR_HAIDI_LAOYUE			0x00000008									//海底捞月

//////////////////////////////////
#define CHR_PING_HU					0x00000010									//平胡	
#define CHR_ZIMO					0x00000020									//自摸
#define CHR_MEN_QING				0x00000040									//门清
#define CHR_DUI_DUI_HU				0x00000080									//对对胡
#define CHR_QING_YI_SE				0x00000100									//清一色
#define CHR_DAI_YAO_JIU				0x00000200									//带幺九
#define CHR_QI_XIAO_DUI				0x00000400									//七小对
#define CHR_JIN_GOUGOU				0x00000800									//金钩钩（单吊）
#define CHR_QING_DUI				0x00001000									//清对（清一色的对对胡）
#define CHR_LONG_QI_DUI				0x00002000									//龙七对
#define CHR_QING_QI_DUI				0x00004000									//清七对（清一色的七小对）
#define CHR_QING_YAO_JIU			0x00008000									//清幺九
#define CHR_JIANG_JIN_GOUGOU		0x00010000									//将金钩钩
#define CHR_QING_JIN_GOUGOU			0x00020000									//清金钩钩
#define CHR_TIAN_HU					0x00040000									//天胡
#define CHR_DI_HU					0x00080000									//地胡
#define CHR_QING_LONG_QI_DUI		0x00100000									//清龙七对
#define CHR_SHIBA_LUOHAN			0x00200000									//十八罗汉
#define CHR_QING_SHIBA_LUOHAN		0x00400000									//清十八罗汉

//////////////////////////////////////////////////////////////////////////


//类型子项
struct tagKindItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbCardIndex[3];						//扑克索引
	BYTE							cbValidIndex[3];					//实际扑克索引
};

//杠牌结果
struct tagGangCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[4];						//扑克数据
};

//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼扑克
	BYTE							cbWeaveKind[4];						//组合类型
	BYTE							cbCenterCard[4];					//中心扑克
	BYTE                            cbCardData[4][4];                   //实际扑克
};

//////////////////////////////////////////////////////////////////////////

#define MASK_CHI_HU_RIGHT			0x0fffffff

/*
//	权位类。
//  注意，在操作仅位时最好只操作单个权位.例如
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)，这样结果是无定义的。
//  只能单个操作:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{	
	//静态变量
private:
	static bool						m_bInit;
	static DWORD					m_dwRightMask[MAX_RIGHT_COUNT];

	//权位变量
private:
	DWORD							m_dwRight[MAX_RIGHT_COUNT];

public:
	//构造函数
	CChiHuRight();

	//运算符重载
public:
	//赋值符
	CChiHuRight & operator = ( DWORD dwRight );

	//与等于
	CChiHuRight & operator &= ( DWORD dwRight );
	//或等于
	CChiHuRight & operator |= ( DWORD dwRight );

	//与
	CChiHuRight operator & ( DWORD dwRight );
	CChiHuRight operator & ( DWORD dwRight ) const;

	//或
	CChiHuRight operator | ( DWORD dwRight );
	CChiHuRight operator | ( DWORD dwRight ) const;

	//功能函数
public:
	//是否权位为空
	bool IsEmpty();

	//设置权位为空
	void SetEmpty();

	//获取权位数值
	BYTE GetRightData( DWORD dwRight[], BYTE cbMaxCount);

	//设置权位数值
	bool SetRightData( const DWORD dwRight[], BYTE cbRightCount);

private:
	//检查权位是否正确
	bool IsValidRight( DWORD dwRight );
};


//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//扑克数据

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
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);

	//辅助函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//扑克数目
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//组合扑克
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);
	//校验有效换三张
	bool IsValidHuangSanZhang(BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT], BYTE cbChangeCardCount);
	//自动选择换三张的牌
	void AutoHuangSanZhang(const BYTE cbCardIndex[MAX_INDEX], BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT]);

	//等级函数
public:
	//动作等级
	BYTE GetUserActionRank(BYTE cbUserAction);

	//动作判断
public:
	//碰牌判断
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌判断
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//动作判断
public:
	//杠牌分析
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult);
	//吃胡分析
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight, BYTE cbHuaZhuCard);
	//是否听牌
	bool IsTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuaZhuCard);
	//是否花猪
	bool IsHuaZhu(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuaZhuCard);

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

	//胡法分析
protected:
	//门清
	bool IsMenQing(const tagAnalyseItem *pAnalyseItem);
	//对对胡
	bool IsDuiDuiHu(const tagAnalyseItem *pAnalyseItem);
	//清一色牌
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount, const BYTE cbCurrentCard);
	//清对
	bool IsQingDui(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//七小对
	bool IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//龙七对
	bool IsLongQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//清七对
	bool IsQingQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//清龙七对
	bool IsQingLongQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//带幺九
	bool IsDaiYaoJiu(const tagAnalyseItem *pAnalyseItem);
	//清幺九
	bool IsQingYaoJiu(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//金钩钩
	bool IsJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//清金钩钩
	bool IsQingJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//将金钩钩
	bool IsJiangJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard);
	//十八罗汉
	bool IsShiBaLuoHan(const tagAnalyseItem *pAnalyseItem);
	//清十八罗汉
	bool IsQingShiBaLuoHan(const tagAnalyseItem *pAnalyseItem);

	//内部函数
private:
	//分析扑克
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);
	//排序,根据牌值排序
	bool SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif
