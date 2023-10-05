#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FREE					105									//空闲时间
#define IDI_USER_POINT				106									//下注时间
#define IDI_GAME_END				107									//开牌时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
VOID *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_FREE:
		{	
			m_pIAndroidUserItem->KillGameTimer(IDI_FREE);
			if(rand()%10==0)m_pIAndroidUserItem->SendSocketData(SUB_C_LEAVE);

			return true;
		}

	case IDI_USER_POINT:
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_POINT);
			//LONGLONG lMinValue = GetPrivateProfileInt(TEXT("Android"), TEXT("MinValue"),100, TEXT("FiveStar.ini"));
			//LONGLONG lMaxValue = GetPrivateProfileInt(TEXT("Android"), TEXT("MaxValue"),100000, TEXT("FiveStar.ini"));
			const LONGLONG BetScore[5] = {1000,10000,100000,1000000,10000000};
			// 发送下注请求
			CMD_C_PlaceBet Point;
			ZeroMemory(&Point, sizeof(Point));
			Point.cbBetArea = rand() % 5;
			Point.lBetScore = BetScore[rand()%5]; //(lMinValue + rand() % (lMaxValue - lMinValue)) / 10;
			//for(int i=enAreaBlack;i<enAreaCount;i++)
			//{
			//	// 去个位数
			//	Point.lAreaPoint[i] = (lMinValue + rand()%(lMaxValue-lMinValue))/10;
			//	Point.lAreaPoint[i] *= 10;
			//}
			//CString aa;
			//aa.Format(TEXT("机器人下注区域%d下注数量%I64d"), Point.cbBetArea, Point.lBetScore);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			m_pIAndroidUserItem->SendSocketData(SUB_C_SET_POINT,&Point,sizeof(Point));
			//继续下注
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_POINT, 2);
			return true;
		}
	case IDI_GAME_END:

		return true;
	}
	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{

	switch (wSubCmdID)
	{
	case SUB_S_USERPOINT:
		{
			return true;
		}
	case SUB_S_GAME_FREE:		//游戏空闲
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_GameFree));
			if (wDataSize!=sizeof(CMD_S_GameFree)) return false;
			//不下注
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_POINT);
			//消息处理
			CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pData;

			//设置时间
			m_pIAndroidUserItem->SetGameTimer(IDI_FREE,pGameFree->cbTimeLeave);

			return true;
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_GameStart));
			if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

			srand(time(NULL));

			//消息处理
			CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

			m_pIAndroidUserItem->SetGameTimer(IDI_USER_POINT,2);

			return true;
		}
	case SUB_S_GAME_END:		//游戏结束
		{
		    m_pIAndroidUserItem->KillGameTimer(IDI_USER_POINT);
			ASSERT(wDataSize == sizeof(CMD_S_GameEnd));
			if (wDataSize != sizeof(CMD_S_GameEnd))	return true;
			CMD_S_GameEnd *pEnd = (CMD_S_GameEnd *)pData;
			//m_pIAndroidUserItem->SetGameTimer(IDI_GAME_END,pEnd->cbTimeEnd);
			return true;
		}
	default:
	{
		return true;
	}
	}

	//错误断言
	ASSERT(FALSE);
	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case  GAME_STATUS_FREE:			//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//消息处理
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			return true;
		}
	case GAME_STATUS_PLAY:		//游戏状态
	case GAME_SCENE_END:		//结束状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			return true;
		}
	}

	return true;
}
//用户进入
VOID  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
VOID  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
VOID  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}
//消息处理----------------------------------------------------

//////////////////////////////////////////////////////////////////////////


