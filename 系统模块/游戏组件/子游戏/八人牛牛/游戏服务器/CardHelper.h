#ifndef __H_CONFIG_CARD_H__
#define __H_CONFIG_CARD_H__
#include <windows.h>
#include <map>
enum emUserDebugType
{
	EM_DEBUG_NULL = -1,
	EM_DEBUG_CARD_OX_0 = 0,		//无牛
	EM_DEBUG_CARD_OX_1,			//牛1
	EM_DEBUG_CARD_OX_2,			//牛2
	EM_DEBUG_CARD_OX_3,			//牛3
	EM_DEBUG_CARD_OX_4,			//牛4
	EM_DEBUG_CARD_OX_5,			//牛5
	EM_DEBUG_CARD_OX_6,			//牛6
	EM_DEBUG_CARD_OX_7,			//牛7
	EM_DEBUG_CARD_OX_8,			//牛8
	EM_DEBUG_CARD_OX_9,			//牛9
	EM_DEBUG_CARD_OX,			//牛牛
	EM_DEBUG_CARD_OX_SIHUA,		//四花牛
	EM_DEBUG_CARD_OX_WUHUA,		//五花牛
	EM_DEBUG_CARD_OX_SHUNZI,	//顺子
	EM_DEBUG_CARD_OX_TONGHUA,	//同花
	EM_DEBUG_CARD_OX_HULU,		//葫芦
	EM_DEBUG_CARD_OX_ZHADAN,	//炸弹
	EM_DEBUG_CARD_OX_TONGHUASHUN,//同花顺
	EM_DEBUG_CARD_OX_WUXIAO,	//五小牛
	EM_DEBUG_CARD_MAX,			//牌型

	EM_DEBUG_WIN,				//赢控制
	EM_DEBUG_LOSE,				//输控制
	EM_DEBUG_MAX,
};

enum emColorType
{
	COLOR_FANGKUAI = 0,
	COLOR_MEIHUA,
	COLOR_HONGTAO,
	COLOR_HEITAO,
	COLOR_MAX,
};
#define COLOR(X) ((X&0xF0)>>4)
#define VALUE(X) (X&0x0F)
#define POINT(X) (VALUE(X)>10?10:VALUE(X))
#define MAKE_CARD(xColor,xValue) ((xColor<<4) | xValue)

#define MAX_CONFIG_CARD_COUNT		5
#define MAX_CONFIG_POOL_SIZE		54
#define MAX_CONFIG_RECORED_SIZE		100000			//
#define MAX_CONFIG_CARD_INDEX	54
#define ELEMENT_OCCUPY_BIT	6
#define MAX_CARD_VALUE		0x10
#define CONFIG_FILE_NAME		"NiuniuPattern.ini"

extern BYTE gConfigCardPool[MAX_CONFIG_POOL_SIZE];

#define USE_SOLUTION_A

struct stResultInfo
{
	unsigned int nCards[MAX_CONFIG_RECORED_SIZE];
	unsigned int nCurCount;
	stResultInfo()
	{
		memset(nCards, 0, sizeof(nCards));
		nCurCount = 0;
	}
	void Add(unsigned int nCardId)
	{
		if (nCurCount >= MAX_CONFIG_RECORED_SIZE)
			return;

		//检查重复
		for (unsigned int i = 0; i < nCurCount; ++i)
		{
			if (nCards[i] == nCardId)
				return;
		}
		nCards[nCurCount++] = nCardId;
	}
	void Reset()
	{
		nCurCount = 0;
	}
};

class CCardHelper
{
	//配牌处理
	static bool				m_bInit;
	static stResultInfo		m_AllResult[EM_DEBUG_CARD_MAX];
	//
	bool			m_bCurValidCardIndex[MAX_CONFIG_POOL_SIZE];
	bool			m_bCurvalidCardType[EM_DEBUG_CARD_MAX];
	//方案2
	BYTE			m_cbPatternCards[EM_DEBUG_CARD_MAX][MAX_CONFIG_CARD_COUNT];

public:
	CCardHelper();
	virtual ~CCardHelper();

protected:
	//
	void SwitchUintToCards(unsigned int nIdx, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	unsigned int SwitchCardsToUint(BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//全组合
	void Combine(BYTE* cbPool, int nStart, int nEnd, BYTE cbMaxCount, BYTE* cbTmp, BYTE cbCurCount, BYTE cbCheckCache[MAX_CONFIG_CARD_INDEX], stResultInfo* stResult);

public:
	void RandPattern(BYTE* cbPool, BYTE cbPoolSize);
	void Read();
	void Write();

public:
	//
	unsigned int GetCardIndex(BYTE cbCard);
	BYTE GetCardByIndex(unsigned int nIdx);
	//初始化缓存
	void RefreshPattern(BYTE* cbPool, BYTE cbPoolSize);
	//从缓存冲获取
	bool GetTypeCards(emUserDebugType emType, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//
	bool HaveType(emUserDebugType emType);

public:
	//
	emUserDebugType GetType(BYTE cbCards[MAX_CONFIG_CARD_COUNT],BYTE* pGetPattern = nullptr);

	//方案2(待完善)
protected:
	//特定组合
	bool CombineSpecialType(BYTE* cbPool, int nStart, int nEnd, BYTE cbMaxCount, BYTE* cbTmp, BYTE cbCurCount, BYTE cbCheckCache[MAX_CONFIG_CARD_INDEX], emUserDebugType emType);
	//EM_DEBUG_CARD_OX_SIHUA:
	bool GetSiHua(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//EM_DEBUG_CARD_OX_WUHUA:
	bool GetWuHua(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//EM_DEBUG_CARD_OX_SHUNZI:
	bool GetShunZi(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//EM_DEBUG_CARD_OX_TONGHUA:
	bool GetTongHua(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//EM_DEBUG_CARD_OX_HULU:
	bool GetHuLu(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//EM_DEBUG_CARD_OX_ZHADAN:
	bool GetZhaDan(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//EM_DEBUG_CARD_OX_TONGHUASHUN:
	bool GetTongHuaShun(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//EM_DEBUG_CARD_OX_WUXIAO:
	bool GetWuXiao(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
	//
	bool GetNiu(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT],BYTE nNiuCount);
	//
	void ForamteCardPool(BYTE* cbPool, BYTE cbPoolSize, BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE], BYTE cbCardCountInfo[MAX_CARD_VALUE]);
	BYTE GetRandColorCard(BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE], BYTE cbCardCountInfo[MAX_CARD_VALUE], emColorType emFilterColor, BYTE cbMinCount, BYTE cbValue, BYTE cbFilterValue = INVALID_BYTE);
	BYTE GetRandValueCard(BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE], BYTE cbMinCount, emColorType emAssignColor, BYTE cbFilterCard[MAX_CONFIG_CARD_COUNT],BYTE cbFilterCount);
	//
	bool GetTypeCards(BYTE* cbPool, BYTE cbPoolSize, emUserDebugType emType, BYTE cbCards[MAX_CONFIG_CARD_COUNT]);
};

#endif