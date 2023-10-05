#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////

//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序
#define ST_CUSTOM					2									//自定排序
#define MAX_SEARCH_RESULT_COUNT		128

//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[4];					//扑克数目
	BYTE							cbCardData[4][MAX_COUNT];			//扑克数据
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
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
	BYTE							cbSearchCount;						//结果数目
	BYTE							cbCardCount[MAX_SEARCH_RESULT_COUNT];				//扑克数目
	BYTE							cbResultCard[MAX_SEARCH_RESULT_COUNT][MAX_COUNT];	//结果扑克
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];				//扑克数据

	//AI变量
public:
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];//所有扑克
	BYTE							m_cbUserCardCount[GAME_PLAYER];			//扑克数目
	WORD							m_wBankerUser;							//地主玩家

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

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
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResult & OutCardResult);
	//同牌搜索
	BYTE SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
		tagSearchCardResult *pSearchCardResult);
	//带牌类型搜索(三带一，四带一等)
	BYTE SearchTakeCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount,
		tagSearchCardResult *pSearchCardResult);
	//连牌搜索
	BYTE SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
		tagSearchCardResult *pSearchCardResult);
	//搜索飞机
	BYTE SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult);

	//内部函数
public:
	//分析扑克
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//分析分布
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing);

	//设置函数
public:
	//设置扑克
	VOID SetUserCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount) ;
	//设置底牌
	VOID SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount) ;
	//设置庄家
	VOID SetBanker(WORD wBanker) ;
	//删除扑克
	VOID RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) ;

	//辅助函数
public:
	//分析炸弹
	VOID GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount);
	//构造扑克
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//排序结果扑克
	VOID SortSearchCardResult(tagSearchCardResult &tmpSearchCardResult, bool bEnemyRemainFive);
	//排序接牌扑克
	VOID SortOutCardResult(tagOutCardResult OutCardResultArray[MAX_SEARCH_RESULT_COUNT], bool bEnemyRemainFive, BYTE cbResultCount);
	
	//出牌函数
public:
	//有牌权出牌
	VOID OutCardRight(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagSearchCardResult *pSearchCardResult);

	//叫分函数
public:
	//叫分判断
	BYTE LandScore(WORD wMeChairID) ;
};

//////////////////////////////////////////////////////////////////////////

#endif