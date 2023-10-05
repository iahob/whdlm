#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//排序类型
#define ST_ORDER					1									//大小排序
#define ST_COUNT					2									//数目排序

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[8];					//扑克数目
	BYTE							cbCardData[8][MAX_CARD_COUNT];		//扑克数据
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_CARD_COUNT];		//结果扑克
};

//分布信息
struct tagDistributing
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbDistributing[15][6];				//分布信息
};

//搜索结果
struct tagSearchCardResult
{
	BYTE							cbSearchCount;						//种类数目
	BYTE							cbCardCount[MAX_CARD_COUNT];		//扑克数目
	BYTE							cbResultCard[MAX_CARD_COUNT][MAX_CARD_COUNT];//扑克列表
};

//////////////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//扑克数据
	bool									m_b2Biggest;										//是否2最大
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();
	VOID	 Set2Biggest(bool bFlag){m_b2Biggest = bFlag;}

	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }


	//控制函数
public:
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//排列扑克
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType,bool b2Biggest=TRUE);
	//删除扑克
	bool RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//排列扑克
	VOID SortOutCardList(BYTE cbCardData[], BYTE cbCardCount);


	//逻辑函数
public:
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData,bool b2Biggest=TRUE);
	//对比扑克
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//搜索出牌
	BYTE SearchOutCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult );
	//搜索指定同牌 (搜索一对或者三条)
	BYTE SearchSameCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult );
	//搜索单张牌
	BYTE SearchSingleCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagSearchCardResult *pSearchCardResult );
	//搜索炸弹
	BYTE SearchBombCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult );
	//搜索连牌
	BYTE SearchLineCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount, tagSearchCardResult *pSearchCardResult );
	//搜索带牌
	BYTE SearchTakeCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult );
	//搜索飞机带翅膀
	BYTE SearchThreeLineTakeNTwo( const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE cbLineCount ,tagSearchCardResult *pSearchCardResult );
	//搜索飞机带1
	BYTE SearchThreeLineTakeOne( const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE cbLineCount ,tagSearchCardResult *pSearchCardResult );
	//搜索510K
	BYTE Search510K(const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE mode,BYTE color,tagSearchCardResult *pSearchCardResult);
	//是否连牌
	bool IsLineCardType(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount);


	//内部函数
private:
	//构造扑克
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//分析扑克
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//分析分布
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing);
};

//////////////////////////////////////////////////////////////////////////////////

#endif