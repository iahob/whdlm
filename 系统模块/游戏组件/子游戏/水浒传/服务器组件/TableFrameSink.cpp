#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////////////

LONGLONG			CTableFrameSink::m_lStorageStartTable = 0;						//起始桌子库存
LONGLONG			CTableFrameSink::m_lStorageInputTable = 0;						//起始桌子库存
LONGLONG			CTableFrameSink::m_lCurStorage = 0;								//起始桌子库存
LONGLONG			CTableFrameSink::m_lRoomStockRecoverScore = 0;					//起始桌子库存
LONGLONG			CTableFrameSink::m_lStorageDeductRoom = 0;						//全局库存衰减
LONGLONG			CTableFrameSink::m_lStorageMax1Room = 0;						//全局库存封顶
LONGLONG			CTableFrameSink::m_lStorageMul1Room = 0;						//全局系统输钱比例
LONGLONG			CTableFrameSink::m_lStorageMax2Room = 0;						//全局库存封顶
LONGLONG			CTableFrameSink::m_lStorageMul2Room = 0;						//全局系统输钱比例
WORD				CTableFrameSink::m_wGameTwo = INVALID_WORD;						//比倍概率
WORD				CTableFrameSink::m_wGameTwoDeduct = INVALID_WORD;						//比倍概率
WORD				CTableFrameSink::m_wGameThree = INVALID_WORD;					//小玛丽概率
WORD				CTableFrameSink::m_wGameThreeDeduct = INVALID_WORD;					//小玛丽概率
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> CTableFrameSink::m_MapRoomUserInfo;	//玩家USERID映射玩家信息
CList<CString, CString&> CTableFrameSink::m_ListOperationRecord;					//操作控制记录


//////////////////////////////////////////////////////////////////////////////////
#define IDI_CHECK_TABLE				1					//检查桌子
#define IDI_USER_OUT_TIME			6					//玩家超时

//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_cbLine = 9;
	m_lBet = 0;
	m_cbGameMode = GM_NULL;
	m_cbTwoMode = 0xFF;
	m_cbCurBounsGameCount = 0;
	m_cbBounsGameCount = 0;
	m_lOneGameScore = 0;
	m_lBetScore = 0;
	m_lTwoGameScore = 0;
	m_lThreeGameScore = 0;
	m_bGameStart = false;

	//组件变量
	m_pITableFrame=NULL;
	m_pGameCustomRule=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//服务控制
	m_wSingleGameTwo = INVALID_WORD;
	m_wSingleGameThree = INVALID_WORD;
	m_hInst = NULL;
	m_pServerControl = NULL;
	m_hInst = LoadLibrary(TEXT("WaterMarginServerDebug.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerControl = static_cast<IServerControl*>(ServerControl());
		}
	}

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
	if( m_pServerControl )
	{
		delete m_pServerControl;
		m_pServerControl = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	m_cbGameMode = GM_NULL;
	m_cbTwoMode = 0xFF;
	m_cbCurBounsGameCount = 0;
	m_cbBounsGameCount = 0;
	m_lOneGameScore = 0;
	m_lBetScore = 0;
	m_lTwoGameScore = 0;
	m_lThreeGameScore = 0;	

	return;
}

//配置桌子
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//错误判断
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
		return NcaTextOut(__LINE__);
	}

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	ASSERT(m_pGameCustomRule != NULL);

	WORD wTableID = m_pITableFrame->GetTableID();

	//初始化库存
	LONGLONG lCurrentStorageTable = m_pGameCustomRule->lStorageStartTable;
	m_lCurStorage = m_pGameCustomRule->lStorageStartTable;
	m_lStorageStartTable = m_lCurStorage;
	m_lStorageInputTable = m_pGameCustomRule->lStorageStartTable;
	m_lStorageDeductRoom = /*m_pGameCustomRule->lStorageDeductRoom*/0;
	m_lStorageMax1Room = m_pGameCustomRule->lStorageMax1Room;
	m_lStorageMul1Room = m_pGameCustomRule->lStorageMul1Room;
	m_lStorageMax2Room = m_pGameCustomRule->lStorageMax2Room;
	m_lStorageMul2Room = m_pGameCustomRule->lStorageMul2Room;
	m_wGameTwo = m_pGameCustomRule->wGameTwo;
	m_wGameTwoDeduct = m_pGameCustomRule->wGameTwoDeduct;
	m_wGameThree = m_pGameCustomRule->wGameThree;
	m_wGameThreeDeduct = m_pGameCustomRule->wGameThreeDeduct;

// 	//校验数据
// 	if (m_lStorageDeductRoom < 0 || m_lStorageDeductRoom > 1000)
// 	{
// 		m_lStorageDeductRoom = 0;
// 	}
// 
// 	if (m_lStorageDeductRoom > 1000)
// 	{
// 		m_lStorageDeductRoom = 1000;
// 	}

	if (m_lStorageMul1Room < 0 || m_lStorageMul1Room > 1000) 
	{
		m_lStorageMul1Room = 100;
	}

	if (m_lStorageMul2Room < 0 || m_lStorageMul2Room > 1000) 
	{
		m_lStorageMul2Room = 80;
	}

	//int nCount = 100000;
	//int nGameCount = 200;
	//LONGLONG lWinScore[200] = {0};
	//LONGLONG lScore = 0;
	//int nWinCount[200] = {0};
	//int nSHZCount[200] = {0};
	//double dControl = 40;
	//double dControlTemp = 0.1;
	//double dWin[200] = {0};
	//for (int n = 0; n < nGameCount; n++)
	//{
	//	for (int i = 0; i < nCount; i++)
	//	{
	//		lWinScore[n] += m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet] * m_cbLine;
	//		m_GameLogic.RandCardList(m_cbItemInfo,dControl);
	//		lScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];
	//		lWinScore[n] -= lScore;
	//		if(lScore > 0)
	//		{
	//			nWinCount[n]++;
	//		}


	//		bool bFind = false;
	//		for (int ii = 0; ii < ITEM_Y_COUNT; ii++)
	//		{
	//			for (int j = 0; j < ITEM_X_COUNT; j++)
	//			{
	//				if(m_cbItemInfo[ii][j] == CT_SHUIHUZHUAN)
	//				{
	//					nSHZCount[n]++;
	//					bFind = true;
	//					break;
	//				}
	//			}

	//			if(bFind) break;
	//		}
	//	}
	//	dControl += dControlTemp;
	//	dWin[n] = nWinCount[n] / ((double)nCount);
	//}


	return true;
}

//消费能力
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//最少积分
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return 0;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_WIAT);	
	
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{				
			if(m_bGameStart)
			{
				//积分变量
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

				if(pIServerUserItem != NULL)
				{			
					//计算积分
					ScoreInfoArray[0].lScore=m_lBetScore+m_lOneGameScore+m_lTwoGameScore+m_lThreeGameScore;
					ScoreInfoArray[0].cbType=(ScoreInfoArray[0].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

					//计算税收
					ScoreInfoArray[0].lRevenue=m_pITableFrame->CalculateRevenue(0,ScoreInfoArray[0].lScore);
					if (ScoreInfoArray[0].lRevenue>0L) ScoreInfoArray[0].lScore-=ScoreInfoArray[0].lRevenue;

					//机器人不写库存
					if(!pIServerUserItem->IsAndroidUser())
					{
						//库存
						WORD wTableID = m_pITableFrame->GetTableID();

						m_lCurStorage -= ScoreInfoArray[0].lScore;

						if(ScoreInfoArray[0].lScore+pIServerUserItem->GetUserScore() < 0)
						{
							CString str;
							str.Format(TEXT("【%d桌】 玩家【%d】【%s】金币不足,携带金币:%I64d,成绩:%I64d"), m_pITableFrame->GetTableID() + 1,  pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(),pIServerUserItem->GetUserScore(),ScoreInfoArray[0].lScore );
							NcaTextOut(str, m_pGameServiceOption->szServerName);
							ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
						}
						if(m_lCurStorage < 0)
						{
							CString str;
							str.Format(TEXT("【%d桌】 当前库存 %I64d, 得分 %I64d， %s[%ld]， 下注 %I64d， 押线 %I64d， 比倍 %I64d， 小玛丽 %I64d"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, ScoreInfoArray[0].lScore, pIServerUserItem->GetNickName(),pIServerUserItem->GetGameID(), m_lBetScore, m_lOneGameScore, m_lTwoGameScore, m_lThreeGameScore);
							NcaTextOut(str, m_pGameServiceOption->szServerName);
						}
					}

					//写入积分
					m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
				}

				m_bGameStart = false;

				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);


				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN, ScoreInfoArray[0].lScore);

			}
			else
			{
				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			
			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			m_bGameStart = false;

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);

			//获取用户
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(0);

			ASSERT(pIServerUserItem != NULL);

			UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

			return true;
		}
	case GER_USER_LEAVE:	//用户强退
	case GER_NETWORK_ERROR:	//网络中断
		{
			if(m_bGameStart)
			{
				m_bGameStart = false;
				//积分变量
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

				if(pIServerUserItem != NULL)
				{
					//计算积分
					ScoreInfoArray[0].lScore=m_lBetScore+m_lOneGameScore+m_lTwoGameScore+m_lThreeGameScore;
					ScoreInfoArray[0].cbType=(ScoreInfoArray[0].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

					//计算税收
					ScoreInfoArray[0].lRevenue=m_pITableFrame->CalculateRevenue(0,ScoreInfoArray[0].lScore);
					if (ScoreInfoArray[0].lRevenue>0L) ScoreInfoArray[0].lScore-=ScoreInfoArray[0].lRevenue;

					//机器人不写库存
					if(!pIServerUserItem->IsAndroidUser())
					{
						//库存
						WORD wTableID = m_pITableFrame->GetTableID();
						m_lCurStorage -= ScoreInfoArray[0].lScore;
						if(m_lCurStorage < 0)
						{
							CString str;
							str.Format(TEXT("【%d桌】 当前库存 %I64d, 得分 %I64d， %s[%ld]， 下注 %I64d， 押线 %I64d， 比倍 %I64d， 小玛丽 %I64d"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, ScoreInfoArray[0].lScore, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), m_lBetScore, m_lOneGameScore, m_lTwoGameScore, m_lThreeGameScore);
							NcaTextOut(str, m_pGameServiceOption->szServerName);
						}
						if(ScoreInfoArray[0].lScore+pIServerUserItem->GetUserScore() < 0)
						{
							CString str;
							str.Format(TEXT("【%d桌】 玩家【%d】【%s】金币不足,携带金币:%I64d,成绩:%I64d"), m_pITableFrame->GetTableID() + 1,  pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(),pIServerUserItem->GetUserScore(),ScoreInfoArray[0].lScore );
							NcaTextOut(str, m_pGameServiceOption->szServerName);
							ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
						}
					}
					//写入积分
					m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
				}
				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP, ScoreInfoArray[0].lScore);
			}
			else
			{
				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
			}

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			StatusFree.lCellScore = m_pGameServiceOption->lCellScore;

			//下注值
			StatusFree.cbBetCount = m_pGameCustomRule->cbBetCount;
			CopyMemory(StatusFree.lBetScore, m_pGameCustomRule->lBetScore, sizeof(StatusFree.lBetScore));

			//更新房间用户信息
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				//变量定义
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));

				//查找当前桌子库存
				LONGLONG lCurrentStorageTable = m_lCurStorage;

				//拷贝数据
				StorageInfo.lCurrentStorageTable = lCurrentStorageTable;
				StorageInfo.lCurrentDeductRoom = m_lStorageDeductRoom;
				StorageInfo.lMaxStorageRoom[0] = m_lStorageMax1Room;
				StorageInfo.lMaxStorageRoom[1] = m_lStorageMax2Room;
				StorageInfo.wStorageMulRoom[0] = (WORD)m_lStorageMul1Room;
				StorageInfo.wStorageMulRoom[1] = (WORD)m_lStorageMul2Room;
				StorageInfo.wGameTwo = m_wGameTwo;
				StorageInfo.wGameTwoDeduct = m_wGameTwoDeduct;
				StorageInfo.wGameThree = m_wGameThree;
				StorageInfo.wGameThreeDeduct = m_wGameThreeDeduct;
				StorageInfo.lRoomStorageStart = m_lStorageStartTable;
				StorageInfo.lRoomStorageCurrent = m_lCurStorage;
				StorageInfo.lRoomStorageInput = m_lStorageInputTable;
				StorageInfo.lRoomStockRecoverScore = m_lRoomStockRecoverScore;


				//m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				//m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_TWO:	//比倍状态
		{
			//构造数据
			CMD_S_StatusTwo StatusTwo;
			ZeroMemory(&StatusTwo,sizeof(StatusTwo));

			StatusTwo.lCellScore = m_pGameServiceOption->lCellScore;
			StatusTwo.cbTwoMode = m_cbTwoMode;
			StatusTwo.lBet = m_lBet;
			StatusTwo.lOneGameScore = m_lOneGameScore;
			StatusTwo.lTwoGameScore = m_lTwoGameScore;
			CopyMemory(StatusTwo.cbItemInfo, m_cbItemInfo, sizeof(StatusTwo.cbItemInfo));	//开奖信息

			//下注值
			StatusTwo.cbBetCount = m_pGameCustomRule->cbBetCount;
			CopyMemory(StatusTwo.lBetScore, m_pGameCustomRule->lBetScore, sizeof(StatusTwo.lBetScore));

			//更新房间用户信息
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				//变量定义
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));

				//查找当前桌子库存
				LONGLONG lCurrentStorageTable = m_lCurStorage;

				//拷贝数据
				StorageInfo.lCurrentStorageTable = lCurrentStorageTable;
				StorageInfo.lCurrentDeductRoom = m_lStorageDeductRoom;
				StorageInfo.lMaxStorageRoom[0] = m_lStorageMax1Room;
				StorageInfo.lMaxStorageRoom[1] = m_lStorageMax2Room;
				StorageInfo.wStorageMulRoom[0] = (WORD)m_lStorageMul1Room;
				StorageInfo.wStorageMulRoom[1] = (WORD)m_lStorageMul2Room;
				StorageInfo.wGameTwo = m_wGameTwo;
				StorageInfo.wGameTwoDeduct = m_wGameTwoDeduct;
				StorageInfo.wGameThree = m_wGameThree;
				StorageInfo.wGameThreeDeduct = m_wGameThreeDeduct;
				StorageInfo.lRoomStorageStart = m_lStorageStartTable;
				StorageInfo.lRoomStorageCurrent = m_lCurStorage;
				StorageInfo.lRoomStorageInput = m_lStorageInputTable;
				StorageInfo.lRoomStockRecoverScore = m_lRoomStockRecoverScore;
				//m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				//m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusTwo,sizeof(StatusTwo));
		}
	}

	//错误断言
	ASSERT(FALSE);

	return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return NcaTextOut(__LINE__);
}

//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return NcaTextOut(__LINE__);
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return NcaTextOut(__LINE__);
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_ONE_START:					//押线开始
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_OneStart));
			if (wDataSize!=sizeof(CMD_C_OneStart)) return NcaTextOut(__LINE__, pIServerUserItem);

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_WIAT);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_WIAT) return NcaTextOut(__LINE__, pIServerUserItem);

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return NcaTextOut(__LINE__, pIServerUserItem);

			//变量定义
			CMD_C_OneStart * pOneStart=(CMD_C_OneStart *)pData;


			return OnUserOneStart(pIServerUserItem,pOneStart->lBet);
		}
	case SUB_C_ONE_END:					//结束押线
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_ONE || m_pITableFrame->GetGameStatus()==GAME_SCENE_WIAT);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_ONE && m_pITableFrame->GetGameStatus()!=GAME_SCENE_WIAT) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserGameEnd(pIServerUserItem);
		}
	case SUB_C_TWO_MODE:					//押线下注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_TwoMode));
			if (wDataSize!=sizeof(CMD_C_TwoMode)) return NcaTextOut(__LINE__, pIServerUserItem);

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_TWO);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_TWO) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//变量定义
			CMD_C_TwoMode * pTwoMode=(CMD_C_TwoMode *)pData;


			return OnUserTwoMode(pIServerUserItem,pTwoMode->cbOpenMode);
		}
 	case SUB_C_TWO_START:					//押线下注
 		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_TwoStart));
			if (wDataSize!=sizeof(CMD_C_TwoStart)) return NcaTextOut(__LINE__, pIServerUserItem);

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_TWO);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_TWO) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//变量定义
			CMD_C_TwoStart * pTwoStart=(CMD_C_TwoStart *)pData;


 			return OnUserTwoStart(pIServerUserItem, pTwoStart->cbOpenArea);
 		}
	case SUB_C_TWO_GIVEUP:					//放弃比倍
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_TWO);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_TWO) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserGameEnd(pIServerUserItem);
		}
	case SUB_C_THREE_START:					//放弃比倍
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_THREE);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_THREE) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserThreeStart(pIServerUserItem);
		}
	case SUB_C_THREE_END:					//结束玛丽
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_THREE);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_THREE) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserGameEnd(pIServerUserItem);
		}
	case SUB_C_UPDATE_TABLE_STORAGE:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorageTable));
			if (wDataSize != sizeof(CMD_C_UpdateStorageTable)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//变量定义
			CMD_C_UpdateStorageTable *pUpdateStorage=(CMD_C_UpdateStorageTable *)pData;

			WORD wTableID = m_pITableFrame->GetTableID();
			SCORE lStockScore = m_lCurStorage;
			m_lCurStorage = pUpdateStorage->lStorageTable;
			m_lStorageInputTable += pUpdateStorage->lStorageTable - lStockScore;

			//20条操作记录
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("操作时间: %04d/%02d/%02d-%02d:%02d:%02d, 控制账户[%s],修改当前库存为 %I64d,当前房间原库存为 %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), pUpdateStorage->lStorageTable, lStockScore);

			m_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			//变量定义
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

			//数组下标
			WORD wIndex = 0;
			while(posListRecord)
			{
				CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

				strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
				_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
				strRecord.ReleaseBuffer();
				OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			//刷新房间库存
			CMD_S_UpdateRoomStorage UpdateRoomStorage;
			ZeroMemory(&UpdateRoomStorage, sizeof(UpdateRoomStorage));
			UpdateRoomStorage.lRoomStorageStart = m_lStorageStartTable;
			UpdateRoomStorage.lRoomStorageCurrent = m_lCurStorage;
			UpdateRoomStorage.lRoomStorageInput = m_lStorageInputTable;
			UpdateRoomStorage.lRoomStockRecoverScore = m_lRoomStockRecoverScore;

			//发送数据
			m_pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
			//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
			//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));

			return true;
		}
	case SUB_C_MODIFY_ROOM_CONFIG:
		{
			ASSERT(wDataSize == sizeof(CMD_C_ModifyRoomConfig));
			if (wDataSize != sizeof(CMD_C_ModifyRoomConfig))
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}
			
			CMD_C_ModifyRoomConfig *pModifyRoomConfig = (CMD_C_ModifyRoomConfig *)pData;

			if(pModifyRoomConfig->wGameTwo > 100 && pModifyRoomConfig->wGameTwo != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameTwoDeduct >= 100 && pModifyRoomConfig->wGameTwoDeduct != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameThree > 100 && pModifyRoomConfig->wGameThree != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameThreeDeduct >= 100 && pModifyRoomConfig->wGameThreeDeduct != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);

			if(pModifyRoomConfig->wStorageMulRoom[0] > 100) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wStorageMulRoom[1] > 100) return NcaTextOut(__LINE__, pIServerUserItem);			

			//拷贝数据
			m_lStorageMax1Room = pModifyRoomConfig->lMaxStorageRoom[0];
			m_lStorageMax2Room = pModifyRoomConfig->lMaxStorageRoom[1];
			m_lStorageMul1Room = (SCORE)(pModifyRoomConfig->wStorageMulRoom[0]);
			m_lStorageMul2Room = (SCORE)(pModifyRoomConfig->wStorageMulRoom[1]);
			m_lStorageDeductRoom = /*pModifyRoomConfig->lStorageDeductRoom*/0;
			m_wGameTwo = pModifyRoomConfig->wGameTwo;
			m_wGameTwoDeduct = pModifyRoomConfig->wGameTwoDeduct;
			m_wGameThree = pModifyRoomConfig->wGameThree;
			m_wGameThreeDeduct = pModifyRoomConfig->wGameThreeDeduct;

			//20条操作记录
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("操作时间: %04d/%02d/%02d-%02d:%02d:%02d, 控制账户[%s], 库存上限值1为 %I64d,赢分概率1为 %I64d,上限值2为 %I64d,赢分概率2为 %I64d, %s, %s"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), m_lStorageMax1Room, m_lStorageMul1Room, m_lStorageMax2Room, m_lStorageMul2Room, GetControlDataString(2), GetControlDataString(3));

			m_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			//变量定义
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

			//数组下标
			WORD wIndex = 0;
			while(posListRecord)
			{
				CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

				strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
				_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
				strRecord.ReleaseBuffer();
				OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
			m_pITableFrame->SendRoomData(NULL, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
	case SUB_C_REQUEST_QUERY_USER:
		{
			ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
			if (wDataSize != sizeof(CMD_C_RequestQuery_User)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

			CMD_S_RequestQueryResult QueryResult;
			ZeroMemory(&QueryResult, sizeof(QueryResult));

			//查找已存在的用户
			ROOMUSERINFO roomUserInfo;
			ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));
			if (m_MapRoomUserInfo.Lookup(pQuery_User->dwGameID, roomUserInfo))
			{
				//拷贝用户信息数据
				QueryResult.bFind = true;
				CopyMemory(&QueryResult.userinfo, &roomUserInfo, sizeof(QueryResult.userinfo));
			}

			////发送数据
			//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));
			//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));
			//发送数据
			m_pITableFrame->SendRoomData(NULL, SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

			return true;
		}
	case SUB_C_USER_CONTROL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UserControl));
			if (wDataSize != sizeof(CMD_C_UserControl)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			CMD_C_UserControl *pUserControl = (CMD_C_UserControl *)pData;

			//遍历映射
			POSITION ptMapHead = m_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;

			//20条操作记录
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}
			
			//操作记录
			CString strOperationRecord;
			CString strControlType;

			if (pUserControl->userControlInfo.controlType >= CONTINUE_3 && pUserControl->userControlInfo.controlType <= CONTINUE_CANCEL &&
				(pUserControl->userControlInfo.cbZhongJiangJian <= 100 || pUserControl->userControlInfo.cbZhongJiangJian == 255) &&
				(pUserControl->userControlInfo.cbZhongJiang <= 100 || pUserControl->userControlInfo.cbZhongJiang == 255)) //激活控制
			{				
				//查找已存在的用户
				ROOMUSERINFO roomUserInfo;
				ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));
				if (m_MapRoomUserInfo.Lookup(pUserControl->dwGameID, roomUserInfo))
				{
					roomUserInfo.UserContorl.cbControlCount = pUserControl->userControlInfo.cbControlCount;
					roomUserInfo.UserContorl.controlType = pUserControl->userControlInfo.controlType;	
					roomUserInfo.UserContorl.cbControlData = pUserControl->userControlInfo.cbControlData;	
					roomUserInfo.UserContorl.cbZhongJiang = pUserControl->userControlInfo.cbZhongJiang;	
					roomUserInfo.UserContorl.cbZhongJiangJian = pUserControl->userControlInfo.cbZhongJiangJian;	

					if(pUserControl->userControlInfo.controlType == CONTINUE_CANCEL)
					{
						roomUserInfo.UserContorl.cbControlCount = 0;
					}
					
					m_MapRoomUserInfo.SetAt(pUserControl->dwGameID, roomUserInfo);
					GetControlTypeString(pUserControl->userControlInfo, strControlType);
					CTime time = CTime::GetCurrentTime();
					strOperationRecord.Format(TEXT("操作时间: %04d/%02d/%02d-%02d:%02d:%02d, 控制账户[%s], 控制玩家[%s],%s "),
						time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), roomUserInfo.szNickName, strControlType);

					//变量定义
					CMD_S_UserControl serverUserControl;
					ZeroMemory(&serverUserControl, sizeof(serverUserControl));
					serverUserControl.dwGameID = roomUserInfo.dwGameID;
					CopyMemory(serverUserControl.szNickName, roomUserInfo.szNickName, sizeof(serverUserControl.szNickName));
					serverUserControl.controlResult = CONTROL_SUCCEED;
					serverUserControl.UserControl = pUserControl->userControlInfo;
					serverUserControl.cbControlCount = pUserControl->userControlInfo.cbControlCount;

					////发送数据
					//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
					//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
					//发送数据
					m_pITableFrame->SendRoomData(NULL, SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
				}
				else
				{
					GetControlTypeString(pUserControl->userControlInfo, strControlType);
					CTime time = CTime::GetCurrentTime();
					strOperationRecord.Format(TEXT("操作时间: %04d/%02d/%02d-%02d:%02d:%02d, 控制账户[%s], 控制玩家[%s],%s,失败！"),
						time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), roomUserInfo.szNickName, strControlType);		
				}
			}
			else
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			m_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//数组下标
			try
			{
				CString strRecord;
				while(posListRecord)
				{
					strRecord = m_ListOperationRecord.GetNext(posListRecord);
					strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
					_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
					strRecord.ReleaseBuffer();
					OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
					wIndex++;
				}
			}
			catch(...)
			{
				DWORD w = GetLastError();
				ASSERT(NULL);
			}
			

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
		case SUB_C_REQUEST_UDPATE_ROOMUSERLIST:
			{
				//权限判断
				if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//遍历映射
				POSITION ptHead = m_MapRoomUserInfo.GetStartPosition();
				DWORD dwUserID = 0;
				ROOMUSERINFO KeyUserInfo;
				ZeroMemory(&KeyUserInfo, sizeof(KeyUserInfo));

				CMD_S_UpdateRoomUserList UpdateRoomUserList;

				while(ptHead)
				{
					m_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, KeyUserInfo);

					ZeroMemory(&UpdateRoomUserList, sizeof(UpdateRoomUserList));
					CopyMemory(&(UpdateRoomUserList.roomUserInfo), &KeyUserInfo, sizeof(KeyUserInfo));
					UpdateRoomUserList.dwUserID = dwUserID;

					////发送数据
					//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOMUSERLIST, &UpdateRoomUserList, sizeof(UpdateRoomUserList));
					//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOMUSERLIST, &UpdateRoomUserList, sizeof(UpdateRoomUserList));

					//发送数据
					m_pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_ROOMUSERLIST, &UpdateRoomUserList, sizeof(UpdateRoomUserList));
				}

				return true;
			}
		case SUB_C_REQUEST_UDPATE_ROOMSTORAGE:
			{
				//权限判断
				if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//刷新房间库存
				CMD_S_UpdateRoomStorage UpdateRoomStorage;
				ZeroMemory(&UpdateRoomStorage, sizeof(UpdateRoomStorage));
				UpdateRoomStorage.lRoomStorageStart = m_lStorageStartTable;
				UpdateRoomStorage.lRoomStorageCurrent = m_lCurStorage;
				UpdateRoomStorage.lRoomStorageInput = m_lStorageInputTable;
				UpdateRoomStorage.lRoomStockRecoverScore = m_lRoomStockRecoverScore;

				////发送数据
				//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
				//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
				//发送数据
				m_pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));

				return true;
			}
		case SUB_C_SINGLEMODE_CONFIRM:
			{	
				ASSERT(wDataSize == sizeof(CMD_C_SingleMode));
				if (wDataSize != sizeof(CMD_C_SingleMode)) 
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//权限判断
				if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				
				CMD_C_SingleMode *pSingleMode = (CMD_C_SingleMode *)pData;
				WORD wTableID = pSingleMode->wTableID;

				SCORE lStockScore = m_lCurStorage;
				m_lCurStorage = pSingleMode->lTableStorage;
				m_lStorageInputTable += pSingleMode->lTableStorage - lStockScore;

				//20条操作记录
				if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
				{
					m_ListOperationRecord.RemoveHead();
				}

				CString strOperationRecord;
				CTime time = CTime::GetCurrentTime();
				strOperationRecord.Format(TEXT("操作时间: %04d/%02d/%02d-%02d:%02d:%02d, 控制账户[%s],修改第[ %d ]号桌子库存为 %I64d,当前房间总库存为 %I64d"),
					time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), pSingleMode->wTableID + 1, pSingleMode->lTableStorage, m_lCurStorage);

				m_ListOperationRecord.AddTail(strOperationRecord);

				//写入日志
				strOperationRecord += TEXT("\n");
				WriteInfo(strOperationRecord);

				//变量定义
				CMD_S_Operation_Record OperationRecord;
				ZeroMemory(&OperationRecord, sizeof(OperationRecord));
				POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

				//数组下标
				WORD wIndex = 0;
				while(posListRecord)
				{
					CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

					strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
					_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
					strRecord.ReleaseBuffer();
					OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
					wIndex++;
				}

				ASSERT(wIndex <= MAX_OPERATION_RECORD);

				////发送数据
				//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
				//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

				//发送数据
				m_pITableFrame->SendRoomData(NULL, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

				return true;
			}
	}

	return NcaTextOut(__LINE__, pIServerUserItem);
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// 消息处理
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		return this->OnGameMessage(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN, pData, wDataSize, pIServerUserItem);
	}
	return false;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
	{
		m_cbGameMode = GM_NULL;
		OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
	}
	return true;
}

//用户重入
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true; 
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserEnter(wChairID);


		//更新房间用户信息
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
	}

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);

		//更新房间用户信息
		UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
	}

	return true;
}

//用户同意
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	return true;
}

//押线开始
bool CTableFrameSink::OnUserOneStart(IServerUserItem * pIServerUserItem, SCORE lBet)
{
	if(lBet < 0 || lBet >= m_pGameCustomRule->cbBetCount) return NcaTextOut(__LINE__, pIServerUserItem);

	ASSERT(pIServerUserItem->GetUserScore() >= m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[lBet] * m_cbLine);
	if(pIServerUserItem->GetUserScore() < m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[lBet] * m_cbLine) return NcaTextOut(__LINE__, pIServerUserItem);

	m_lBet = lBet;
	m_bGameStart = true;

	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_ONE);	

	WORD wTableID = m_pITableFrame->GetTableID();
	m_cbTwoMode = 0xFF;	

	m_wSingleGameTwo = m_wGameTwo;
	m_wSingleGameThree = m_wGameThree;

	if(m_lCurStorage > 0 && !pIServerUserItem->IsAndroidUser())
	{
		SCORE lRecoverScore = m_lCurStorage*m_lStorageDeductRoom/1000;
		m_lCurStorage -= lRecoverScore;
		m_lRoomStockRecoverScore += lRecoverScore;
	}

	if(!pIServerUserItem->IsAndroidUser())
	{
		CString str;
		str.Format(TEXT("【%d桌】 当前库存 %I64d  %s[%ld]"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, pIServerUserItem->GetNickName(),pIServerUserItem->GetGameID());
		NcaTextOut(str, m_pGameServiceOption->szServerName);
	}

	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//更新控制
	ROOMUSERINFO roomUserInfo;
	bool bWin = UpdateControl(roomUserInfo);	

	//混乱扑克
	double dControl = m_lStorageMulRoom;

	m_lBetScore = -m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet] * m_cbLine;

	bool bControl = false;
	if(bWin && m_pServerControl != NULL)
	{		
		if(m_pServerControl->ServerControl(roomUserInfo.UserContorl, m_cbItemInfo))
		{
			m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];
			bControl = true;
		}
	}
	if(!bControl)
	{
		m_GameLogic.RandCardList(m_cbItemInfo,dControl);
		m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];

		while(m_lCurStorage - m_lOneGameScore < 0 && m_lOneGameScore > 0 && !pIServerUserItem->IsAndroidUser())
		{
			dControl /= 1.2;
			m_GameLogic.RandCardList(m_cbItemInfo,dControl);
			m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];
		}
	}  

	if (m_lOneGameScore > 0)
	{
		m_cbBounsGameCount = m_GameLogic.GetEnterThree(m_cbItemInfo);
		if (m_cbBounsGameCount > 0)
		{
			m_pITableFrame->SetGameStatus(GAME_SCENE_THREE);
			m_cbGameMode = GM_THREE;
		}
		else
		{
			m_pITableFrame->SetGameStatus(GAME_SCENE_TWO);
			m_cbGameMode = GM_TWO;
		}
		//更新房间用户信息
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
	}
	else
	{
		m_cbGameMode = GM_NULL;
	}

	//构造变量
	CMD_S_GameStart GameStart;
	GameStart.lScore = m_lOneGameScore;
	GameStart.cbGameMode = m_cbGameMode;
	CopyMemory(GameStart.cbItemInfo, m_cbItemInfo, sizeof(GameStart.cbItemInfo));


	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));


	//if (m_cbGameMode == GM_NULL)
	//{
	//	OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
	//}

	return true;
}

//比倍开始
bool CTableFrameSink::OnUserTwoMode(IServerUserItem * pIServerUserItem, BYTE cbOpenMode)
{
	ASSERT(m_cbTwoMode == 0xFF);
	if(m_cbTwoMode != 0xFF) return NcaTextOut(__LINE__, pIServerUserItem);

	m_cbTwoMode = cbOpenMode;

	return true;
}

//比倍开始
bool CTableFrameSink::OnUserTwoStart(IServerUserItem * pIServerUserItem, BYTE cbOpenArea)
{
	ASSERT(m_cbTwoMode != 0xFF);
	if(m_cbTwoMode == 0xFF) 
	{
		CString str;
		str.Format(TEXT("【%d桌】 下注区域有错， %s[%ld]"), m_pITableFrame->GetTableID() + 1, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID());
		NcaTextOut(str, m_pGameServiceOption->szServerName);
		return NcaTextOut(__LINE__, pIServerUserItem);
	}

	SCORE lScore = m_lOneGameScore;
	if (m_cbTwoMode == 0)
	{
		lScore = m_lOneGameScore / 2;
	}
	else if (m_cbTwoMode == 2)
	{
		lScore = m_lOneGameScore * 2;
	}

	ASSERT(pIServerUserItem->GetUserScore() + m_lOneGameScore + m_lBetScore >= lScore);
	if(pIServerUserItem->GetUserScore() + m_lOneGameScore + m_lBetScore < lScore)
	{
		CString str;
		str.Format(TEXT("【%d桌】 下注分数不够, 比倍下注 %I64d， %s[%ld]， 下注 %I64d， 押线 %I64d， 比倍 %I64d， 分数 %I64d"), m_pITableFrame->GetTableID() + 1, lScore, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), m_lBetScore, m_lOneGameScore, m_lTwoGameScore, pIServerUserItem->GetUserScore());
		NcaTextOut(str, m_pGameServiceOption->szServerName);
		return NcaTextOut(__LINE__, pIServerUserItem);
	}
	m_lTwoGameScore -= lScore;

	WORD wTableID = m_pITableFrame->GetTableID();
	LONGLONG lStockScore = m_lCurStorage / __max(m_MapRoomUserInfo.GetCount(), 1) - m_lOneGameScore - m_lTwoGameScore - m_lBetScore;

	double dControl = (double)m_lStorageMulRoom;

	//查找已存在的用户	
	ROOMUSERINFO roomUserInfo;
	roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
	BYTE cbControlType = 0; //不控制
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), roomUserInfo))
	{	
		if(roomUserInfo.UserContorl.cbControlCount > 0)
		{
			if(roomUserInfo.UserContorl.cbZhongJiang != 0xFF)
			{
				if(rand() % 100 < roomUserInfo.UserContorl.cbZhongJiang)
				{
					//控制赢
					cbControlType = 1;
				}
				else
				{
					//控制输
					cbControlType = 2;
				}

				if(roomUserInfo.UserContorl.cbZhongJiangJian != 0xFF)
				{
					roomUserInfo.UserContorl.cbZhongJiang = roomUserInfo.UserContorl.cbZhongJiang * roomUserInfo.UserContorl.cbZhongJiangJian / 100;
				}
				else
				{
					roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
				}
				m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
			}	
			else if(m_wSingleGameTwo != INVALID_WORD)
			{
				//控制
				cbControlType = (rand() % 100 < m_wSingleGameTwo) ? 3 : 2;
				m_wSingleGameTwo = m_wGameTwoDeduct == INVALID_WORD ? INVALID_WORD : m_wSingleGameTwo * m_wGameTwoDeduct / 100;
			}
		}
		else
		{
			roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
			if(m_wSingleGameTwo != INVALID_WORD)
			{
				//控制
				cbControlType = (rand() % 100 < m_wSingleGameTwo) ? 3 : 2;
				m_wSingleGameTwo = m_wGameTwoDeduct == INVALID_WORD ? INVALID_WORD : m_wSingleGameTwo * m_wGameTwoDeduct / 100;
			}
		}
	}
	else
	{
		roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
		if(m_wSingleGameTwo != INVALID_WORD)
		{
			//控制
			cbControlType = (rand() % 100 < m_wSingleGameTwo) ? 3 : 2;
			m_wSingleGameTwo = m_wGameTwoDeduct == INVALID_WORD ? INVALID_WORD : m_wSingleGameTwo * m_wGameTwoDeduct / 100;
		}
	}

	SCORE lTwoScore = 0;
	BYTE cbOpenSize1 = 0;
	BYTE cbOpenSize2 = 0; 

	do 
	{
		cbOpenSize1 = rand() % 6 + 1;
		cbOpenSize2 = rand() % 6 + 1;

		lTwoScore = 0;

		if (cbOpenArea == 0 && cbOpenSize1 + cbOpenSize2 <= 6)		//小
		{
			lTwoScore = lScore * 2;
			if (cbOpenSize1 == cbOpenSize2)		//对子
			{
				lTwoScore = lScore * 4;
			}
		}
		else if (cbOpenArea == 2 && cbOpenSize1 + cbOpenSize2 > 7)	//大
		{
			lTwoScore = lScore * 2;
			if (cbOpenSize1 == cbOpenSize2)		//对子
			{
				lTwoScore = lScore * 4;
			}
		}
		else if (cbOpenArea == 1 && cbOpenSize1 + cbOpenSize2 == 7)		//和
		{
			lTwoScore = lScore * 6;
		}
		
		if(cbControlType == 2)		//控制输
		{
			if(lTwoScore == 0)
			{
				break;
			}
		}
		else if(cbControlType == 1 || cbControlType == 3) //控制赢
		{
			if(lTwoScore > 0)
			{
				if(lStockScore - lTwoScore < 0 && cbControlType == 3)
				{
					cbControlType = 0;
				}
				else
				{
					break;
				}
			}
		}

	} while (lTwoScore > 0 && lStockScore - lTwoScore < 0 && cbControlType == 0 || cbControlType != 0);
	
	m_lTwoGameScore += lTwoScore;

	CMD_S_GameTwoStart TwoStart;
	TwoStart.cbOpenSize[0] = cbOpenSize1;
	TwoStart.cbOpenSize[1] = cbOpenSize2;
	TwoStart.lScore = lTwoScore;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TWO_START,&TwoStart,sizeof(TwoStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TWO_START,&TwoStart,sizeof(TwoStart));

// 	if (lTwoScore <= 0)
// 	{
// 		m_cbGameMode = GM_NULL;
// 		OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
// 	}

	
	return true;
}

//小玛丽开始
bool CTableFrameSink::OnUserThreeStart(IServerUserItem * pIServerUserItem)
{
	WORD wTableID = m_pITableFrame->GetTableID();
	LONGLONG lStockScore = m_lCurStorage / m_pGameServiceOption->wTableCount - m_lOneGameScore - m_lBetScore;

	//查找已存在的用户	
	ROOMUSERINFO roomUserInfo;
	roomUserInfo.UserContorl.cbZhongJiang = 0xFF;

	BYTE cbControlType = 0; //不控制
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), roomUserInfo))
	{	
		if(roomUserInfo.UserContorl.cbControlCount > 0)
		{
			if(roomUserInfo.UserContorl.cbZhongJiang != 0xFF)
			{
				if(rand() % 100 < roomUserInfo.UserContorl.cbZhongJiang)
				{
					//控制赢
					cbControlType = 1;
				}
				else
				{
					//控制输
					cbControlType = 2;
				}

				if(roomUserInfo.UserContorl.cbZhongJiangJian != 0xFF)
				{
					roomUserInfo.UserContorl.cbZhongJiang = roomUserInfo.UserContorl.cbZhongJiang * roomUserInfo.UserContorl.cbZhongJiangJian / 100;
				}
				else
				{
					roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
				}
				m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
			}
			else
			{
				if(m_wSingleGameThree != INVALID_WORD)
				{
					//控制
					cbControlType = (rand() % 100 < m_wSingleGameThree) ? 1 : 2;
					m_wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD :m_wSingleGameThree * m_wGameThreeDeduct / 100;
				}
			}
		}
		else
		{
			roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
			if(m_wSingleGameThree != INVALID_WORD)
			{
				//控制
				cbControlType = (rand() % 100 < m_wSingleGameThree) ? 1 : 2;
				m_wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD :m_wSingleGameThree * m_wGameThreeDeduct / 100;
			}
		}
	}
	else
	{
		roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
		if(m_wSingleGameThree != INVALID_WORD)
		{
			//控制
			cbControlType = (rand() % 100 < m_wSingleGameThree) ? 1 : 2;
			m_wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD :m_wSingleGameThree * m_wGameThreeDeduct / 100;
		}
	}

	for (int i = 0; i < m_cbBounsGameCount; i++)
	{
		BYTE cbZhongJiang = roomUserInfo.UserContorl.cbZhongJiang;
		WORD wSingleGameThree = m_wSingleGameThree;

		CMD_S_GameThreeStart GameThreeStart;	
		GameThreeStart.cbBounsGameCount = m_cbBounsGameCount - i - 1;
		m_GameLogic.RandCardListEx(GameThreeStart.cbItem, CountArray(GameThreeStart.cbItem));

		BYTE cbPos;
		GameThreeStart.lScore = m_GameLogic.GetThreeZhongJiangInfo(GameThreeStart.cbItem, cbPos) * -m_lBetScore;
		while (GameThreeStart.lScore > 0 && lStockScore - GameThreeStart.lScore < 0)
		{
			m_GameLogic.RandCardListEx(GameThreeStart.cbItem, CountArray(GameThreeStart.cbItem));

			BYTE cbPos;
			GameThreeStart.lScore = m_GameLogic.GetThreeZhongJiangInfo(GameThreeStart.cbItem, cbPos) * -m_lBetScore;
		}
		lStockScore -= GameThreeStart.lScore;
		m_lThreeGameScore += GameThreeStart.lScore;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THREE_START,&GameThreeStart,sizeof(GameThreeStart));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THREE_START,&GameThreeStart,sizeof(GameThreeStart));

		BYTE cbIndex;
		int nTime;
		bool bWin;
		double dRandCount = 1;
		int nTimeCount = 0;
		int nZhongJiangCount = 0;
		do 
		{
			int nCount = 0;			
			do 
			{
				nTime = m_GameLogic.RandCardListThree(cbIndex, GameThreeStart.cbItem, m_lStorageMulRoom*10 / dRandCount);
				bWin = m_GameLogic.GetThreeZhongJiangInfo(cbIndex, GameThreeStart.cbItem);

				if(nTime > 0 && !bWin && cbControlType == 0)
				{
					if(++nTimeCount > 2)
					{
						ASSERT(NULL);
						BYTE cbItemType[4] = {5, 11, 17, 23};
						cbIndex = cbItemType[rand() % 4];
						nTime = 0;
						bWin = false;
						break;
					}
				}
				
				if(cbControlType == 2)		//控制输
				{
					if(!bWin && nTime == 0 || ++nCount >= 10000)
					{
						ASSERT(nCount < 10000);
						break;
					}
				}
				else if(cbControlType == 1) //控制赢
				{
					if(bWin && lStockScore - (nTime * -m_lBetScore) - m_lThreeGameScore >= 0 || ++nCount >= 10000)
					{
						nZhongJiangCount++;
						ASSERT(nCount < 10000);
						break;
					}
					else
					{
						dRandCount /= 1.2;
						if(dRandCount == 0)
						{
							dRandCount = 1;
						}
					}
				}
				else
				{
					dRandCount *= 2;
				}
			}
			while (nTime > 0 && bWin && lStockScore - nTime * -m_lBetScore < 0 && cbControlType == 0 || cbControlType != 0);

			if(cbZhongJiang != 0xFF)
			{
				if(rand() % 100 < cbZhongJiang)
				{
					//控制赢
					cbControlType = 1;
				}
				else
				{
					//控制输
					cbControlType = 2;
				}

				if(roomUserInfo.UserContorl.cbZhongJiangJian != 0xFF)
				{
					cbZhongJiang = cbZhongJiang * roomUserInfo.UserContorl.cbZhongJiangJian / 100;
				}
				else
				{
					cbZhongJiang = 0xFF;
				}
			}
			else
			{
				if(wSingleGameThree != INVALID_WORD)
				{
					//控制					
					wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD : wSingleGameThree / 2;
					cbControlType = ((rand() % 100) < wSingleGameThree) ? 1 : 2;

					//同一轮随机不超过4次
					if(nZhongJiangCount > (rand() % 5))
					{
						cbControlType = 2;
					}
				}
				else
				{
					cbControlType = 0;
				}
			}
			CMD_S_GameThreeKaiJiang GameThreeKaiJiang;
			GameThreeKaiJiang.cbIndex = cbIndex;
			if (bWin)
			{
				GameThreeKaiJiang.nTime = nTime;
				GameThreeKaiJiang.lScore = nTime * -m_lBetScore;
				lStockScore -= GameThreeKaiJiang.lScore;
				m_lThreeGameScore += GameThreeKaiJiang.lScore;
			}
			else
			{
				GameThreeKaiJiang.nTime = 0;
				GameThreeKaiJiang.lScore = 0;
			}

			
			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THREE_KAI_JIAN,&GameThreeKaiJiang,sizeof(GameThreeKaiJiang));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THREE_KAI_JIAN,&GameThreeKaiJiang,sizeof(GameThreeKaiJiang));

		} while (nTime != 0);
			

		if(roomUserInfo.UserContorl.cbZhongJiangJian != 0xFF)
		{
			roomUserInfo.UserContorl.cbZhongJiang = roomUserInfo.UserContorl.cbZhongJiang * roomUserInfo.UserContorl.cbZhongJiangJian / 100;
		}
		else
		{
			roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
			if(m_wSingleGameThree != INVALID_WORD)
			{
				//控制
				cbControlType = ((rand() % 100) < m_wSingleGameThree) ? 1 : 2;
				m_wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD : m_wSingleGameThree * m_wGameThreeDeduct / 100;
			}
			else
			{
				cbControlType = 0;
			}
		}
		dRandCount = 1;
	}

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THREE_END);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THREE_END);	
	return true;
}

//游戏结束
bool CTableFrameSink::OnUserGameEnd(IServerUserItem * pIServerUserItem)
{
	m_cbGameMode = GM_NULL;
	OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);

	return true;
}

void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

//写日志文件
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
	static CCriticalSection g_criSection;
	g_criSection.Lock();//给所有使用的资源上锁
	try
	{
		//设置语言区域
		char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
		setlocale(LC_CTYPE, "chs");

		CStdioFile myFile;
		CString strFileName;
		strFileName.Format(TEXT("水浒传[%s]控制.log"), m_pGameServiceOption->szServerName);
		BOOL bOpen = myFile.Open(strFileName, CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate);

		if (bOpen)
		{	
			myFile.SeekToEnd();
			myFile.WriteString(pszString);
			myFile.Flush();
			myFile.Close();
		}

		//还原区域设定
		setlocale(LC_CTYPE, old_locale);
		free(old_locale);
	}
	catch(...)
	{
		DWORD w = GetLastError();
		ASSERT(NULL);
	}
	g_criSection.Unlock(); //给使用的资源解锁
}

//更新房间用户信息
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction, LONGLONG lGameScore)
{
	if(pIServerUserItem->IsAndroidUser()) return;

	//变量定义
	ROOMUSERINFO roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(roomUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//用户坐下和重连
	if (userAction == USER_SITDOWN || userAction == USER_RECONNECT)
	{
		roomUserInfo.wChairID = pIServerUserItem->GetChairID();
		roomUserInfo.wTableID = pIServerUserItem->GetTableID();
	}
	else if (userAction == USER_STANDUP || userAction == USER_OFFLINE)
	{
		roomUserInfo.wChairID = INVALID_CHAIR;
		roomUserInfo.wTableID = INVALID_TABLE;
	}

	//查找已存在的用户
	ROOMUSERINFO lastroomUserInfo;
	ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), lastroomUserInfo))
	{
		roomUserInfo.lGameScore = lastroomUserInfo.lGameScore + lGameScore;
		roomUserInfo.UserContorl = lastroomUserInfo.UserContorl;
	}

	m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
}

//更新控制
bool CTableFrameSink::UpdateControl(ROOMUSERINFO &roomUserInfo)
{
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(0);

	m_lStorageMulRoom = 0;
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL)
	{		
		return NcaTextOut(__LINE__, pIServerUserItem);
	}
	
	//查找已存在的用户	
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), roomUserInfo))
	{		
		if(roomUserInfo.UserContorl.cbControlCount > 0)
		{
			roomUserInfo.UserContorl.cbControlCount--;
			m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
			if(roomUserInfo.UserContorl.controlType != CONTINUE_LOST)
			{
				if (m_lCurStorage > m_lStorageMax2Room)
				{
					m_lStorageMulRoom = m_lStorageMul2Room * 2;
				}
				else 
				{
					m_lStorageMulRoom = m_lStorageMul1Room * 2;
				}
				return true;
			}
			else
			{
				m_lStorageMulRoom = 0;
			}
			return NcaTextOut(__LINE__, pIServerUserItem);
		}
	}

	if (m_lCurStorage > m_lStorageMax2Room)
	{
		m_lStorageMulRoom = m_lStorageMul2Room;
	}
	else if (m_lCurStorage > 0)
	{
		if(m_lStorageMax1Room>m_lCurStorage && m_lStorageMax1Room!=0)	
			m_lStorageMulRoom = m_lStorageMul1Room*m_lCurStorage/m_lStorageMax1Room;
		else 
			m_lStorageMulRoom = m_lStorageMul1Room;
			
	}

	return NcaTextOut(__LINE__, pIServerUserItem);
}

//获取控制类型
void CTableFrameSink::GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr)
{
	
	switch(UserControl.controlType)
	{
	case CONTINUE_3:
		{
			controlTypestr.Format(TEXT("控制类型为 %d局 3连 %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_4:
		{
			controlTypestr.Format(TEXT("控制类型为  %d局4连 %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_5:
		{
			controlTypestr.Format(TEXT("控制类型为 %d局 5连 %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_ALL:
		{
			controlTypestr.Format(TEXT("控制类型为 %d局 全盘 %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_LOST:
		{
			controlTypestr.Format(TEXT("控制类型为 %d局不中"), UserControl.cbControlCount);
			break;
		}
	case CONTINUE_CANCEL:
		{
			controlTypestr = TEXT("控制类型为取消控制");
			break;
		}
	}
}



//获取控制类型
CString CTableFrameSink::GetControlDataString(USERCONTROL &UserControl)
{
	CString str;

	switch(UserControl.cbControlData)
	{
	case 0:
		{
			str = TEXT("斧头");
			break;
		}
	case 1:
		{
			str = TEXT("银枪");
			break;
		}
	case 2:
		{
			str = TEXT("大刀");
			break;
		}
	case 3:
		{
			str = TEXT("鲁");
			break;
		}
	case 4:
		{
			str = TEXT("林");
			break;
		}
	case 5:
		{
			str = TEXT("宋");
			break;
		}
	case 6:
		{
			str = TEXT("替天行道");
			break;
		}
	case 7:
		{
			str = TEXT("忠义堂");
			break;
		}
	case 8:
		{
			str = TEXT("水浒传");
			break;
		}
	case 9:
		{
			str = TEXT("武器");
			break;
		}
	case 10:
		{
			str = TEXT("人物");
			break;
		}
	}

	CString strTemp;
	if(UserControl.cbZhongJiang == 255)
	{
		if(UserControl.cbControlData == 8)
		{
			str += TEXT("，不控制小玛丽");
		}
		else
		{
			str += TEXT("，不控制比倍");
		}
	}
	else
	{
		if(UserControl.cbControlData == 8)
		{
			strTemp.Format(TEXT("，小玛丽中奖率%d"), UserControl.cbZhongJiang);
		}
		else
		{
			strTemp.Format(TEXT("，比倍中奖率%d"), UserControl.cbZhongJiang);
		}

		str += strTemp;
		if(UserControl.cbZhongJiangJian == 255)
		{
			str += TEXT("，第2局起随机中奖");
		}
		else
		{
			strTemp.Format(TEXT("，衰减比例%d"), UserControl.cbZhongJiangJian);
			str += strTemp;
		}
	}
	return str;
}

//获取控制类型
CString CTableFrameSink::GetControlDataString(BYTE cbGameMode)
{
	CString str;

	if(cbGameMode == 2)
	{
		if(m_wGameTwo == INVALID_WORD)
		{
			str = TEXT("不控制比倍");
		}
		else
		{
			if(m_wGameTwoDeduct == INVALID_WORD)
			{
				str.Format(TEXT("比倍中奖概率为 %d 第二局起不控制"), m_wGameTwo);
			}
			else
			{
				str.Format(TEXT("比倍中奖概率为 %d 衰减值为 %d"), m_wGameTwo, m_wGameTwoDeduct);
			}
		}
	}
	else if(cbGameMode == 3)
	{
		if(m_wGameThree == INVALID_WORD)
		{
			str = TEXT("不控制小玛丽");
		}
		else
		{
			if(m_wGameThreeDeduct == INVALID_WORD)
			{
				str.Format(TEXT("小玛丽中奖概率为 %d 第二局起不控制"), m_wGameThree);
			}
			else
			{
				str.Format(TEXT("小玛丽中奖概率为 %d 衰减值为 %d"), m_wGameThree, m_wGameThreeDeduct);
			}
		}
	}
	return str;
}
//////////////////////////////////////////////////////////////////////////////////
