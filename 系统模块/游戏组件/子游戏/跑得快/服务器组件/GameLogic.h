#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//扑克类型
#define CT_ERROR							0									//错误类型
#define CT_SINGLE							1									//单牌类型
#define CT_SINGLE_LINE						2									//单连类型
#define CT_DOUBLE_LINE						3									//对连类型
#define CT_THREE_LINE						4									//三连类型(最后可出)(可以大于2个三张)
#define CT_THREE_TAKE_SINGLE				5									//三带一单(最后可出)(1个三张)
#define CT_THREE_TAKE_TWO_SINGLE			6									//三带二单(1个三张)
#define CT_THREE_TAKE_DOUBLE				7									//三带一对(1个三张)

#define CT_THREE_LINE_TAKE_N				8									//连同三张带N,N等于连数(最后可出)
#define CT_THREE_LINE_TAKE_2N				10									//连同三张带2N,N等于连数
#define CT_THREE_LINE_TAKE_MASS				11									//最后能出(MASS表示带牌，MASS不等于0，N,2N)
#define CT_FOUR_TAKE_THREE					13									//四带三

//比如三连三同张 带 0 最后能出 (CT_THREE_LINE)
//比如三连三同张 带 1 最后能出 (CT_THREE_LINE_TAKE_MASS)
//比如三连三同张 带 2 最后能出 (CT_THREE_LINE_TAKE_MASS)
//比如三连三同张 带 3 最后能出 (CT_THREE_LINE_TAKE_N)
//比如三连三同张 带 4 最后能出 (CT_THREE_LINE_TAKE_MASS)
//比如三连三同张 带 5 最后能出 (CT_THREE_LINE_TAKE_MASS)
//比如三连三同张 带 6 正常出   (CT_THREE_LINE_TAKE_2N)

#define CT_BOMB								14									//炸弹类型

//宏定义
#define MAX_COUNT					16									//最大数目

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//分析结构
struct tagAnalyseResult
{
	BYTE 							bFourCount;							//四张数目
	BYTE 							bThreeCount;						//三张数目
	BYTE 							bDoubleCount;						//两张数目
	BYTE							bSignedCount;						//单张数目
	BYTE 							bFourLogicVolue[4];					//四张列表
	BYTE 							bThreeLogicVolue[5];				//三张列表
	BYTE 							bDoubleLogicVolue[8];				//两张列表
	BYTE 							bSignedLogicVolue[16];				//单张列表
	BYTE							bFourCardData[MAX_COUNT];			//四张列表
	BYTE							bThreeCardData[MAX_COUNT];			//三张列表
	BYTE							bDoubleCardData[MAX_COUNT];			//两张列表
	BYTE							bSignedCardData[MAX_COUNT];			//单张数目
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_bCardListDataSixTeen[48];				//扑克数据(16张玩法)
	static const BYTE				m_bCardListDatafifTeen[45];				//扑克数据(15张玩法)

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取类型
	BYTE GetCardType(BYTE bCardData[], BYTE bCardCount);
	//获取数量
	BYTE GetCardCount(BYTE bCardData[],BYTE bCardCount, BYTE cbCardData);
	//获取数值
	BYTE GetCardValue(BYTE bCardData) { return bCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE bCardData) { return bCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE bCardData[], BYTE bCardCount);
	//混乱扑克
	void RandCardList(BYTE bCardBuffer[], BYTE bBufferCount);
	//删除扑克
	bool RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount);

	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE bCardData);

	//功能函数
public:
	//对比扑克
	bool CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount);
	//分析扑克
	void AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResult & AnalyseResult);
	//出牌判断
	bool SearchOutCard(BYTE bCardData[], BYTE bCardCount, BYTE bTurnCardData[], BYTE bTurnCardCount, BYTE bTurnOutType, bool bNoRemoveBomb, tagOutCardResult & OutCardResult);
	//出牌判断(搜索大于单牌的结果)
	bool SearchOutCard(BYTE cbCardData[], BYTE cbCardCount, bool bNoRemoveBomb, BYTE & cbResultCount);
};

//////////////////////////////////////////////////////////////////////////

#endif