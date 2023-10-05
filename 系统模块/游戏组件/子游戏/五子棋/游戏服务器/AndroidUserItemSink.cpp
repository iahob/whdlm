#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////
//辅助时间
#define TIME_LESS					2									//最少时间
//游戏时间
#define IDI_ANDROID_ITEM_SINK       100
#define IDI_START_GAME			(IDI_ANDROID_ITEM_SINK+0)			//开始时间
#define IDI_USER_ADD_SCORE		(IDI_ANDROID_ITEM_SINK+2)			//下注时间
#define IDI_HUIQI				(IDI_ANDROID_ITEM_SINK+1)			//悔棋时间
#define	IDI_QIUHE				(IDI_ANDROID_ITEM_SINK+3)			//求和
//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	XPos	=	100;
	YPos	=	100;
	initGame();
	//接口变量
	m_pIAndroidUserItem=NULL;

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool  CAndroidUserItemSink::RepositionSink()
{
	initGame();
	return true;
}

//时间消息
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始定时器
		{
			//发送准备
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
			m_pIAndroidUserItem->SendUserReady(NULL,0);

			return true;
		}
	case IDI_USER_ADD_SCORE:	//加注定时
		{
			//删除定时器
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
			
			SetChess();
			return true;
		}
	case IDI_HUIQI:				//悔棋应答
		{
			//BYTE aa =(BYTE)(2.0 * rand()/(RAND_MAX + 1.0));
			//发送消息
			CMD_C_RegretAnswer ReqretAnswer;
			ReqretAnswer.cbApprove = 0;
			m_pIAndroidUserItem->SendSocketData(SUB_C_REGRET_ANSWER,&ReqretAnswer,sizeof(ReqretAnswer));
			return true;
		}
	case IDI_QIUHE:				//求和应答
		{
			//BYTE aa =(BYTE)(2.0 * rand()/(RAND_MAX + 1.0));
			//发送消息
			CMD_C_PeaceAnswer PeaceAnswer;
			PeaceAnswer.cbApprove=1;
			m_pIAndroidUserItem->SendSocketData(SUB_C_PEACE_ANSWER,&PeaceAnswer,sizeof(PeaceAnswer));
			return true;
		}
	}

	return false;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{		
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}		
	case SUB_S_PLACE_CHESS:		//放置棋子
		{
			return OnSubPlaceChess(pData,wDataSize);
		}
	case SUB_S_REGRET_REQ:		//悔棋请求
		{
			return OnSubRegretReq(pData,wDataSize);
		}
	case SUB_S_REGRET_FAILE:	//悔棋失败
		{
			return OnSubRegretFaile(pData,wDataSize);
		}
	case SUB_S_REGRET_RESULT:	//悔棋结果
		{
			return OnSubRegretResult(pData,wDataSize);
		}
	case SUB_S_PEACE_REQ:		//和棋请求
		{
			return OnSubPeaceReq(pData,wDataSize);
		}
	case SUB_S_PEACE_ANSWER:	//和棋应答
		{
			return OnSubPeaceAnser(pData,wDataSize);
		}
	case SUB_S_BLACK_TRADE:		//交换对家
		{
			return OnSubBlackTrade(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_CHESS_MANUAL:	//游戏棋谱
		{
			return OnSubChessManual(pData,wDataSize);
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//开始时间
			UINT nElapse=rand()%(5)+TIME_LESS;
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

			return true;
		}
	case GAME_STATUS_PLAY:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//消息处理
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//设置变量
			m_wBlackUser=pStatusPlay->wBlackUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_bRestrict=pStatusPlay->cbRestrict?true:false;
			m_bTradeUser=pStatusPlay->cbTradeUser?true:false;
			m_bDoubleChess=pStatusPlay->cbDoubleChess?true:false;

			return true;
		}
		default:
			return true;
	}

	ASSERT(FALSE);

	return false;
}

//用户进入
void  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
void  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
void  CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}
//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//游戏变量
	m_wBlackUser=pGameStart->wBlackUser;
	m_wCurrentUser=pGameStart->wBlackUser;
	m_bRestrict=pGameStart->cbRestrict?true:false;
	m_bTradeUser=pGameStart->cbTradeUser?true:false;
	m_bDoubleChess=pGameStart->cbDoubleChess?true:false;

	//设置变量
	m_GameLogic.InitGameLogic();

	//开始时间
	//设置随机数时间种子                
	srand((unsigned int)time(NULL));

	if (m_wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		XPos		=	7;
		YPos		=	8;
		board[XPos][YPos]	=	1;
		UINT nElapse=1 + (UINT)(1.0 * rand()/(RAND_MAX + 1.0));
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
	}

	return true;
}
//用户放棋
bool CAndroidUserItemSink::OnSubPlaceChess(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceChess));
	if (wDataSize!=sizeof(CMD_S_PlaceChess)) return false;

	//消息处理
	CMD_S_PlaceChess * pPlaceChess=(CMD_S_PlaceChess *)pBuffer;

	//游戏变量
	m_wCurrentUser	=	pPlaceChess->wCurrentUser;
	BYTE	xx		=	pPlaceChess->cbXPos;
	BYTE	yy		=	pPlaceChess->cbYPos;

	board[xx][yy]	=	0;		//设定为玩家棋子数
	for(int i=0; i<572; i++)
	{
		if(ptab[xx][yy][i])
		{
			win[0][i]++;
			ctab[xx][yy][i] = false;
			win[1][i] = 7;
		}
	}
	
	if (m_wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		
		int  num = 0;
		for (int i=0; i<15; i++)
		{
			
			for (int j=0; j<15; j++)
			{
				
				if (board[i][j] != 2)
				{
					
					num++;
				}
				
			}
			//MessageBox(NULL,"111","111",NULL);
		}
		
			//int a = GetCount();
		//CString SS;
		//SS.Format(TEXT("%d"),num);
		//MessageBox(NULL,SS,"222",NULL);
		if (num >= 100)
		{
			//MessageBox(NULL,"222","222",NULL);
			m_pIAndroidUserItem->SendSocketData(SUB_C_PEACE_REQ);
			return true;
		}
		ComTurn();
	}

	return true;
}
//悔棋请求
bool CAndroidUserItemSink::OnSubRegretReq(const void * pBuffer, WORD wDataSize)
{
	UINT nElapse=1 + (UINT)(2.0 * rand()/(RAND_MAX + 1.0));
	m_pIAndroidUserItem->SetGameTimer(IDI_HUIQI,nElapse);

	return true;
}
//悔棋失败
bool CAndroidUserItemSink::OnSubRegretFaile(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RegretFaile));
	if (wDataSize!=sizeof(CMD_S_RegretFaile)) return false;

	//消息处理
	CMD_S_RegretFaile * pRegretFaile=(CMD_S_RegretFaile *)pBuffer;

	//次数限制
	if (pRegretFaile->cbFaileReason==FR_COUNT_LIMIT)
	{
		return true;
	}

	//对家反对
	if (pRegretFaile->cbFaileReason==FR_PLAYER_OPPOSE)
	{
		return true;
	}
	return true;
}
//悔棋结果
bool CAndroidUserItemSink::OnSubRegretResult(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RegretResult));
	if (wDataSize!=sizeof(CMD_S_RegretResult)) return false;

	//变量定义
	CMD_S_RegretResult * pRegretResult=(CMD_S_RegretResult *)pBuffer;

	//修改棋谱
	m_GameLogic.RegretChess(pRegretResult->wRegretCount);

	return true;
}
//和棋请求
bool CAndroidUserItemSink::OnSubPeaceReq(const void * pBuffer, WORD wDataSize)
{
	UINT nElapse=1 + (UINT)(2.0 * rand()/(RAND_MAX + 1.0));
	m_pIAndroidUserItem->SetGameTimer(IDI_QIUHE,nElapse);
	return true;
}
//和棋应答
bool CAndroidUserItemSink::OnSubPeaceAnser(const void * pBuffer, WORD wDataSize)
{
	return true;
}
//交换对家
bool CAndroidUserItemSink::OnSubBlackTrade(const void * pBuffer, WORD wDataSize)
{
	//对换用户
	m_wCurrentUser=m_wBlackUser;
	m_wBlackUser=(m_wBlackUser+1)%GAME_PLAYER;

	return true;
}
//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;

	initGame();
	XPos	=	100;
	YPos	=	100;

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);

	//设置定时器
	//开始时间
	UINT nElapse=rand()%(5)+TIME_LESS;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	return true;
}
//游戏棋谱
bool CAndroidUserItemSink::OnSubChessManual(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagChessManual)==0);
	if (wDataSize%sizeof(tagChessManual)!=0) return false;

	//放置棋子
	WORD wManualCount=wDataSize/sizeof(tagChessManual);
	for (WORD wIndex=0;wIndex<wManualCount;wIndex++) 
	{
		tagChessManual * pChessManual=(((tagChessManual *)pBuffer)+wIndex);
		m_GameLogic.PlaceChess(pChessManual->cbXPos,pChessManual->cbYPos,pChessManual->cbColor);
	}
	return true;
}
//放置棋子
void CAndroidUserItemSink::SetChess()
{
	//发送消息
	CMD_C_PlaceChess PlaceChess;
	PlaceChess.cbXPos	=	XPos;
	PlaceChess.cbYPos	=	YPos;

	m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_CHESS,&PlaceChess,sizeof(PlaceChess));
}
//设置放棋计时器
void CAndroidUserItemSink::SetPlaceTimer()
{
	UINT nElapse	=	1 + (UINT)(3.0 * rand()/(RAND_MAX + 1.0));
	m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
}

//初始函数
void CAndroidUserItemSink::initGame()
{
	int i,j,k;
	int count=0;
 
	//设定玩家与计算机在各个获胜组合中的棋子数
	for(i=0; i<572; i++)
	{
		win[0][i] = 0;
		win[1][i] = 0;
	}
 
	//初始化棋盘状态
	for(i=0; i<15; i++) 
	{
		for(j=0; j<15; j++)
		{
			board[i][j] = 2;		//设定为空格
		}
	}
	//设定水平方向的获胜组合
	for(i=0; i<15; i++)    
	{
		for(j=0; j<11; j++)
		{
			for(k=0; k<5; k++)
			{
				ptab[i][j+k][count] = true;
				ctab[i][j+k][count] = true;
			}
			count++;
		}
	}
   
   //设定垂直方向的获胜组合
	for(i=0; i<15; i++)  
	{
		for(j=0; j<11; j++)
		{
			for(k=0; k<5; k++)
			{
				ptab[j+k][i][count] = true;
				ctab[j+k][i][count] = true;
			}
			count++;
		}
	}
    //设定正对角线方向的获胜组合
    for(i=0; i<11; i++) 
	{
		for(j=0; j<11; j++)
		{
			for(k=0; k<5; k++)
			{
				ptab[j+k][i+k][count] = true;
				ctab[j+k][i+k][count] = true;
			}
			count++;
		}
	}
     //设定反对角线上的获胜组合
	for(i=0; i<11; i++)   
	{
		for(j=14; j>=4; j--)
		{
			for(k=0; k<5; k++)
			{
				ptab[j-k][i+k][count] = true;
				ctab[j-k][i+k][count] = true;
			}
			count++;
		}
	}
}
//机器人下棋位置
void CAndroidUserItemSink::ComTurn()
{
	int grades[2][15][15];
	int m,n,i,max=0;
	int u,v;

	for(m=0; m<15; m++) 
	{
		for(n=0; n<15; n++)
		{
			grades[0][m][n] = 0;
			grades[1][m][n] = 0;

			if(board[m][n] == 2)//空格
			{
				for(i=0; i<572; i++)
				{
					//计算玩家在空位置上的获胜分数
					if(ptab[m][n][i] && win[0][i] != 7)
					{
						switch(win[0][i])
						{
						case 0:
							grades[0][m][n] += 1;
							break;
						case 1:
							grades[0][m][n] += 200;
							break;
						case 2:
							grades[0][m][n] += 400;
							break;
						case 3:
							grades[0][m][n] += 2000;
							break;
						case 4:
							grades[0][m][n] += 10000;
							break;
						}
					}				

					//计算计算机在空位置上的获胜分数
					if(ctab[m][n][i] && win[1][i] != 7)
					{
						switch(win[1][i])
						{
						case 0:
							grades[1][m][n] += 1;
							break;
						case 1:
							grades[1][m][n] += 220;
							break;
						case 2:
							grades[1][m][n] += 420;
							break;
						case 3:
							grades[1][m][n] += 2100;
							break;
						case 4:
							grades[1][m][n] += 20000;
							break;
						}
					}
				}
			}

			if(max == 0)
			{
				u = m;
				v = n;
			}

			if(grades[0][m][n] > max)
			{
				max = grades[0][m][n];
				u = m;
				v = n;
			}
			else if(grades[0][m][n] == max)
			{
				if(grades[1][m][n] > grades[1][u][v])
				{
					u = m;
					v = n;
				}
			}

			if(grades[1][m][n] > max)
			{
				max = grades[1][m][n];
				u = m;
				v = n;
			}
			else if(grades[1][m][n] == max)
			{
				if(grades[0][m][n] > grades[0][u][v])
				{
					u = m;
					v = n;
				}
			}
		}
	}

	board[u][v] = 1;      //设定为计算机的棋子数

	//去掉对方的胜利组合
	for(i=0; i<572; i++)
	{
		if(ctab[u][v][i])
		{
			win[1][i]++;
			ptab[u][v][i] = false;
			win[0][i] = 7;
		}
	}

	//发送消息
	XPos	=	u;
	YPos	=	v;
	SetPlaceTimer();
}
//棋盘上有多少棋子
int  CAndroidUserItemSink::GetCount()
{
	int  num = 0;
	for (int i=0; i<15; i++)
	{
		for (int j=0; i<15; j++)
		{
			if (board[i][j] != 2)
			{
				num++;
			}
		}
	}
	return num;
}