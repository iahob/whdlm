#include "StdAfx.h"
#include "LockTimeMatch.h"
#include "..\游戏服务器\DataBasePacket.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//比赛状态
//enum emMatchStatus
//{
//	MatchStatus_Free=0,
//	MatchStatus_WaitPlay,
//	MatchStatus_Playing,
//	MatchStatus_WaitEnd,
//	MatchStatus_End
//};
//比赛状态
enum emMatchStatus
{
	MatchStatus_Null = 0,									//无状态
	MatchStatus_Signup,										//报名中
	MatchStatus_EndSignup,									//报名结束
	MatchStatus_Wait,										//等待桌子
	MatchStatus_Playing										//比赛进行中
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//常量定义
#define INVALID_VALUE				0xFFFF								//无效值

//时钟定义
#define IDI_SWITCH_STATUS			(IDI_MATCH_MODULE_START+1)					//切换状态
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+2)					//分配用户
#define IDI_CHECK_START_SIGNUP		(IDI_MATCH_MODULE_START+3)					//开始报名
#define IDI_CHECK_END_SIGNUP		(IDI_MATCH_MODULE_START+4)					//开始截止
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+5)					//开始时钟
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+6)					//结束时钟

//时钟定义
#define IDI_LOCK_DELETE_OVER_MATCH		(IDI_MATCH_MODULE_START+7)				//删除结束的比赛组
#define IDI_LOCK_CHECK_OVER_MATCH		(IDI_MATCH_MODULE_START+8)				//检查一场比赛 
#define IDI_LOCK_CHECK_MATCH			(IDI_MATCH_MODULE_START+9)				//巡查比赛 
#define IDI_LOCK_WAIT_TABLE				(IDI_MATCH_MODULE_START+10)				//等待配桌 
#define IDI_LOCK_SWITCH_TABLE			(IDI_MATCH_MODULE_START+11)				//换桌等待 
#define IDI_LOCK_MATCH_FINISH			(IDI_MATCH_MODULE_START+12)				//比赛完成
#define IDI_SWITCH_TABLE_MATCH			(IDI_MATCH_MODULE_START+13)				//比赛换桌
#define IDI_MATCH_FINISH_MATCH			(IDI_MATCH_MODULE_START+14)				//比赛完成

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CLockTimeMatch::CLockTimeMatch()
{
	//OutputDebugString(TEXT("ptdt *** 构造函数 CLockTimeMatch"));

	//状态变量
	//m_MatchStatus=MatchStatus_Free;
	m_cbMatchStatus = MatchStatus_Signup;
	//比赛配置
	m_pMatchOption=NULL;
	m_pLockTimeMatch=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//设置变量
	ZeroMemory(&m_MatchStartTime, sizeof(m_MatchStartTime));
	m_cbCurMatchRounds = 1;
	m_cbTotalPlayerCount = 0;

	//内核接口
	m_ppITableFrame=NULL;
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//服务接口
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;

	//移除元素
	m_LockTimeTableArray.RemoveAll();	
	m_LockAndroidArray.RemoveAll();
}

CLockTimeMatch::~CLockTimeMatch(void)
{
	//OutputDebugString(TEXT("ptdt *** 析构函数 ~CLockTimeMatch"));

	m_cbMatchStatus = MatchStatus_Null;
	//释放资源
	SafeDeleteArray(m_ppITableFrame);

	//关闭定时器
	m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);
	m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);
	m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);		
	m_pITimerEngine->KillTimer(IDI_LOCK_CHECK_MATCH);

	//移除元素
	//m_DistributeManage.RemoveAll();
	m_LockAndroidArray.RemoveAll();

	//释放桌子
	for (int i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		SafeDelete(m_LockTimeTableArray[i]);
	}
	m_LockTimeTableArray.RemoveAll();
}

//接口查询
VOID* CLockTimeMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	//OutputDebugString(TEXT("ptdt *** QueryInterface"));

	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//启动通知
void CLockTimeMatch::OnStartService()
{
	//OutputDebugString(TEXT("ptdt *** OnStartService"));

	//计算场次
	m_pMatchOption->lMatchNo=CalcMatchNo();

	////设置状态
	//if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
	//{
	//	if (m_pMatchOption->cbMatchStatus==MS_FREE) m_MatchStatus=MatchStatus_Free;
	//	if (m_pMatchOption->cbMatchStatus==MS_MATCHEND) m_MatchStatus=MatchStatus_End;
	//	if (m_pMatchOption->cbMatchStatus==MS_MATCHING) m_MatchStatus=MatchStatus_Playing;	
	//}

	//设置状态
	m_TimeLastMatch=CTime::GetCurrentTime();	
	//记录时间
	CTime::GetCurrentTime().GetAsSystemTime(m_MatchStartTime);

	//更新状态
//	UpdateMatchStatus();

	//切换时钟
//	SwitchMatchTimer();		 

	//报名开始
	//if (IsMatchSignupStart()==false)
	//{
	//	//开始检测	
	//	m_pITimerEngine->SetTimer(IDI_CHECK_START_SIGNUP,1000L,TIMES_INFINITY,NULL);
	//}	

	////报名截止
	//if (IsMatchSignupEnd()==false && m_MatchStatus<MatchStatus_Playing)
	//{
	//	m_pITimerEngine->SetTimer(IDI_CHECK_END_SIGNUP,1000L,TIMES_INFINITY,NULL);	
	//}


	//获取时间
	CTime CurTime = CTime::GetCurrentTime();
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	//报名开始
//	CTime SignupEndTime(m_pLockTimeMatch->SignupEndTime);		//报名截止
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);		//比赛开始
	
	//变量定义
	DWORD dwTimeInterval = 1;
	m_cbTotalPlayerCount = 0;

	//开始报名
	if (CurTime < SignupStartTime)
	{
		CTimeSpan TimeSpan = SignupStartTime - CurTime;
		dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();
				
		m_pITimerEngine->SetTimer(IDI_CHECK_START_SIGNUP, dwTimeInterval * 1000, TIMES_INFINITY, NULL);
	}
	else
	{
		m_cbMatchStatus = MatchStatus_Signup;
	}
	
	////开始截止
	//if (CurTime < SignupEndTime)
	//{
	//	CTimeSpan TimeSpan = SignupEndTime - CurTime;
	//	dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();

	//	m_pITimerEngine->SetTimer(IDI_CHECK_END_SIGNUP, dwTimeInterval*1000, TIMES_INFINITY, NULL);
	//}
	//else
	//{
	//	AfxMessageBox(TEXT("比赛报名已截止，服务器不能启动!"));
	//	return;
	//}
	//开始时钟
	if (CurTime < MatchStartTime)
	{
		CTimeSpan TimeSpan = MatchStartTime - CurTime;
		dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();

		m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH, dwTimeInterval * 1000, TIMES_INFINITY, NULL);
	}	
}

//绑定桌子
bool CLockTimeMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	//OutputDebugString(TEXT("ptdt *** BindTableFrame"));

	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		//ASSERT(false);
		return false;
	}

	//创建钩子
	CTableFrameHook * pTableFrameHook=new CTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//设置接口
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;
	
	return true;
}

//初始化接口
bool CLockTimeMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	//OutputDebugString(TEXT("ptdt *** InitMatchInterface"));

	//服务配置
	m_pMatchOption=MatchManagerParameter.pGameMatchOption;
	m_pGameServiceOption=MatchManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=MatchManagerParameter.pGameServiceAttrib;

	//内核组件
	m_pITimerEngine=MatchManagerParameter.pITimerEngine;
	m_pIDataBaseEngine=MatchManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=MatchManagerParameter.pTCPNetworkEngine;

	//服务组件		
	m_pIGameServiceFrame=MatchManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=MatchManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=MatchManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=MatchManagerParameter.pIServerUserItemSink;

	//分组设置
	m_DistributeManage.SetDistributeRule(m_pMatchOption->cbDistributeRule);

	//比赛规则
	m_pLockTimeMatch=(tagLockTimeMatch *)m_pMatchOption->cbMatchRule;

	//调整间隔
	if (m_pMatchOption->wDistributeTimeSpace==0)
	{
		m_pMatchOption->wDistributeTimeSpace=5;
	}

	//调整人数
	if (m_pMatchOption->wMaxPartakeGameUser<2) 
	{
		m_pMatchOption->wMaxPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//调整人数
	if (m_pMatchOption->wMaxPartakeGameUser>m_pGameServiceAttrib->wChairCount)
	{
		m_pMatchOption->wMaxPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//调整人数
	if (m_pMatchOption->wMinPartakeGameUser<2) 
	{
		m_pMatchOption->wMinPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//调整人数
	if (m_pMatchOption->wMinPartakeGameUser>m_pMatchOption->wMaxPartakeGameUser)
	{
		m_pMatchOption->wMinPartakeGameUser=m_pMatchOption->wMaxPartakeGameUser;
	}

	//分组人数
	if (m_pMatchOption->wMinDistributeUser<m_pMatchOption->wMinPartakeGameUser)
	{
		m_pMatchOption->wMinDistributeUser=m_pMatchOption->wMinPartakeGameUser;
	}

	//创建桌子
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}

	return true;
}

//时间事件
bool CLockTimeMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdt *** OnEventTimer dwTimerID = %d"), dwTimerID);
	//OutputDebugString(szInfo);
	
	switch(dwTimerID)
	{
	case IDI_LOCK_CHECK_MATCH:
		{
			CaptureMatchTimer();
			return true;
		}
	case IDI_CHECK_START_SIGNUP:			//开始报名
		{
			//测试代码			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("IDI_CHECK_START_SIGNUP:开始报名"));
			CTraceService::TraceString(szMessage, TraceLevel_Debug);
			//关闭定时器
			m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);
			//报名开始												
			m_cbMatchStatus = MatchStatus_Signup;

			//变量定义
			DBR_GR_SignupStart SignupStart;

			//构造结构
			SignupStart.dwMatchID=m_pMatchOption->dwMatchID;
			SignupStart.lMatchNo=m_pMatchOption->lMatchNo;
			SignupStart.cbMatchType=m_pMatchOption->cbMatchType;
			SignupStart.wServerID=m_pGameServiceOption->wServerID;
			SignupStart.cbSignupMode=m_pMatchOption->cbSignupMode;

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(SignupStart.dwMatchID,DBR_GR_MATCH_SIGNUP_START,0,&SignupStart,sizeof(SignupStart));
			
			return true;
		}
	case IDI_CHECK_START_MATCH:				//开始比赛
		{
			//测试代码			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("IDI_CHECK_START_MATCH:开始比赛"));
			CTraceService::TraceString(szMessage, TraceLevel_Debug);
			//关闭定时器
			m_pITimerEngine->KillTimer(IDI_CHECK_START_MATCH);
			
			//测试代码						
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("当前A： %d 个人参加比赛。"), m_LockOnMatchUserMap.GetCount());
			CTraceService::TraceString(szMessage, TraceLevel_Debug);

			//凑齐整桌
			if (m_LockOnMatchUserMap.GetCount() % 3 == 1)
			{				
				if (m_LockAndroidArray.GetCount() > 0)
				{	
					IServerUserItem *pLoopUserItem = m_LockOnMatchUserMap[m_LockAndroidArray[0]];
					if (pLoopUserItem)
					{
						DBR_GR_MatchUnSignup MatchUnSignup;
						MatchUnSignup.dwReason = UNSIGNUP_REASON_SYSTEM;
						MatchUnSignup.dwUserID = pLoopUserItem->GetUserID();
						//构造结构
						MatchUnSignup.dwInoutIndex = pLoopUserItem->GetInoutIndex();
						MatchUnSignup.dwClientAddr = pLoopUserItem->GetClientAddr();
						MatchUnSignup.dwMatchID = m_pMatchOption->dwMatchID;
						MatchUnSignup.lMatchNo = m_pMatchOption->lMatchNo;
						lstrcpyn(MatchUnSignup.szMachineID, pLoopUserItem->GetMachineID(), CountArray(MatchUnSignup.szMachineID));
						//投递数据
						m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID, DBR_GR_MATCH_UNSIGNUP, 0, &MatchUnSignup, sizeof(MatchUnSignup));
					}
					
					//删除用户
					m_LockOnMatchUserMap.RemoveKey(m_LockAndroidArray[0]);
					m_LockAndroidArray.RemoveAt(0);
				}
			}
			else if (m_LockOnMatchUserMap.GetCount() % 3 == 2)
			{
				if (m_LockAndroidArray.GetCount() > 1)
				{
					IServerUserItem *pLoopUserItem = m_LockOnMatchUserMap[m_LockAndroidArray[0]];
					if (pLoopUserItem)
					{
						DBR_GR_MatchUnSignup MatchUnSignup;
						MatchUnSignup.dwReason = UNSIGNUP_REASON_SYSTEM;
						MatchUnSignup.dwUserID = pLoopUserItem->GetUserID();
						//构造结构
						MatchUnSignup.dwInoutIndex = pLoopUserItem->GetInoutIndex();
						MatchUnSignup.dwClientAddr = pLoopUserItem->GetClientAddr();
						MatchUnSignup.dwMatchID = m_pMatchOption->dwMatchID;
						MatchUnSignup.lMatchNo = m_pMatchOption->lMatchNo;
						lstrcpyn(MatchUnSignup.szMachineID, pLoopUserItem->GetMachineID(), CountArray(MatchUnSignup.szMachineID));
						//投递数据
						m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID, DBR_GR_MATCH_UNSIGNUP, 0, &MatchUnSignup, sizeof(MatchUnSignup));
					}
					pLoopUserItem = m_LockOnMatchUserMap[m_LockAndroidArray[1]];
					if (pLoopUserItem)
					{
						DBR_GR_MatchUnSignup MatchUnSignup;
						MatchUnSignup.dwReason = 0;
						MatchUnSignup.dwUserID = pLoopUserItem->GetUserID();
						//构造结构
						MatchUnSignup.dwInoutIndex = pLoopUserItem->GetInoutIndex();
						MatchUnSignup.dwClientAddr = pLoopUserItem->GetClientAddr();
						MatchUnSignup.dwMatchID = m_pMatchOption->dwMatchID;
						MatchUnSignup.lMatchNo = m_pMatchOption->lMatchNo;
						lstrcpyn(MatchUnSignup.szMachineID, pLoopUserItem->GetMachineID(), CountArray(MatchUnSignup.szMachineID));
						//投递数据
						m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID, DBR_GR_MATCH_UNSIGNUP, 0, &MatchUnSignup, sizeof(MatchUnSignup));
					}

					m_LockOnMatchUserMap.RemoveKey(m_LockAndroidArray[0]);
					m_LockOnMatchUserMap.RemoveKey(m_LockAndroidArray[1]);

					m_LockAndroidArray.RemoveAll();
				}

			}			
			//测试代码						
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("当前B: %d 个人参加比赛。"), m_LockOnMatchUserMap.GetCount());
			CTraceService::TraceString(szMessage, TraceLevel_Debug);

			//玩家是否符合参赛人数			
			if (m_LockOnMatchUserMap.GetCount() < m_pLockTimeMatch->wStartUserCount)
			{
				//发送结束消息
				TCHAR szMessage[128] = TEXT("对不起比赛人数达不到参赛要求，比赛解散，请您明天再来参加！");
				m_pIGameServiceFrame->SendGameMessage(szMessage, SMT_CLOSE_ROOM /* SMT_CHAT | SMT_EJECT | SMT_GLOBAL | SMT_CLOSE_GAME*/);

				//比赛结束
				DBR_GR_MatchCancel MatchCancel;
				ZeroMemory(&MatchCancel, sizeof(MatchCancel));

				//构造数据
				MatchCancel.bMatchFinish = FALSE;
				MatchCancel.dwMatchID = m_pMatchOption->dwMatchID;
				MatchCancel.lMatchNo = m_pMatchOption->lMatchNo;
				MatchCancel.wServerID = m_pGameServiceOption->wServerID;
				
				//投递请求
				m_pIDataBaseEngine->PostDataBaseRequest(MatchCancel.dwMatchID, DBR_GR_MATCH_CANCEL, 0, &MatchCancel, sizeof(MatchCancel));

				return true;
			}

			//变量定义
			m_cbMatchStatus = MatchStatus_Playing;	
			BYTE cbMatchStatus = MS_WAITPLAY;
			m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));

			//第一次记录
			m_cbTotalPlayerCount = m_LockOnMatchUserMap.GetCount();

			//构造人数			
			//CMD_GR_Match_Num MatchNum;
			//MatchNum.dwWaitting = m_LockOnMatchUserMap.GetCount();
			//MatchNum.dwTotal = m_cbTotalPlayerCount;
			////发送人数
			//m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, MDM_GR_MATCH, SUB_GR_MATCH_NUM, &MatchNum, sizeof(MatchNum));

			////变量定义
			//DBR_GR_MatchStart MatchStart;

			////构造结构
			//MatchStart.dwMatchID=m_pMatchOption->dwMatchID;
			//MatchStart.lMatchNo=m_pMatchOption->lMatchNo;
			//MatchStart.cbMatchType=m_pMatchOption->cbMatchType;
			//MatchStart.wServerID=m_pGameServiceOption->wServerID;

			////投递请求
			//m_pIDataBaseEngine->PostDataBaseRequest(MatchStart.dwMatchID,DBR_GR_MATCH_START,0,&MatchStart,sizeof(MatchStart));

			////发送状态
			//BYTE cbMatchStatus=MS_MATCHING;
			//m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));	
	
			SendMatchInfo();
			EfficacyStartMatch();

			return true;
		}	
	case IDI_LOCK_SWITCH_TABLE:
		{
			//测试代码			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("IDI_SWITCH_TABLE"));
			CTraceService::TraceString(szMessage, TraceLevel_Debug);

			//关闭定时器
			m_pITimerEngine->KillTimer(IDI_LOCK_SWITCH_TABLE);

			//全部起立
			PerformAllUserStandUp();

			//if (m_pLockTimeMatch->bAndroidDebug)
			//{
			//	//AI调试
			//	m_pMatchSink->OnEventMatchAndroidDebug(this, dwPromotion, m_DebugAndroidArray[0], m_DebugAndroidArray[1], m_DebugAndroidArray[2]);
			//}
			//else
			//{
			
			//排名晋级			
			MatchRoundsOver((DWORD)dwBindParameter);			
			//}

			return true;
		}
	case IDI_LOCK_MATCH_FINISH:
		{
			//测试代码			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("IDI_MATCH_FINISH"));
			CTraceService::TraceString(szMessage, TraceLevel_Debug);

			//if (m_pLockTimeMatch->bAndroidDebug)
			//{
			//	//AI调试
			//	m_pMatchSink->OnEventMatchAndroidDebug(this, 0, m_DebugAndroidArray[0], m_DebugAndroidArray[1], m_DebugAndroidArray[2]);
			//}
			//else
			//{				

				//最后结束
				MatchOver();

				m_cbCurMatchRounds = 1;
			//}


			return true;
		}
		/*
		case IDI_CHECK_END_MATCH:				//结束比赛
		{
			//变量定义
			BYTE cbMatchStatus=m_MatchStatus;

			//更新状态
			UpdateMatchStatus();

			//比赛结束
			if (m_MatchStatus==MatchStatus_WaitEnd)
			{
				if (cbMatchStatus!=MatchStatus_WaitEnd)
				{
					//发送状态
					BYTE cbMatchStatus=MS_WAITEND;
					m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));
					
					//获取时间
					CTime CurTime=CTime::GetCurrentTime();
					CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

					//比赛结束
					bool bMatchOver=CurTime>=MatchEndTime;

					//变量定义					
					WORD wUserIndex=0;
					TCHAR szMessage[128];
					IServerUserItem *pIServerUserItem = NULL;

					do 
					{
						pIServerUserItem=m_pIServerUserManager->EnumUserItem(wUserIndex++);
						if (pIServerUserItem==NULL) break;

						//比赛中用户
						if (pIServerUserItem->GetUserStatus()==US_PLAYING)
						{							
							if (bMatchOver==false)
							{
								// _sntprintf_s(szMessage,CountArray(szMessage),TEXT("今天的比赛结束时间已到，请尽快在 %d 分钟内结束本局游戏。"),m_pLockTimeMatch->wEndDelayTime/60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);
							}
							else
							{
								//_sntprintf_s(szMessage, CountArray(szMessage), TEXT("本场比赛结束时间已到，请尽快在 %d 分钟内结束当前比赛。否则系统将会自动解散本回合并不计入成绩。"), m_pLockTimeMatch->wEndDelayTime / 60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);
							}
						}
						else
						{
							if (bMatchOver==false)
							{
								_sntprintf_s(szMessage, CountArray(szMessage), TEXT("今天的比赛到此结束，请您明天再来参加！"));
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);
							}
							else
							{
								//发送结束消息
								//_sntprintf_s(szMessage, CountArray(szMessage), TEXT("本场比赛结束时间已到，感谢您的参与；请在 %d 分钟后前往相关页面查询您的本场比赛成绩。"), m_pLockTimeMatch->wEndDelayTime / 60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);
							}
						}

					} while (true);
				}
			}

			//比赛结束
			if (m_MatchStatus==MatchStatus_End)
			{
				//关闭定时器
				m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
				m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);

				//发送状态
				BYTE cbMatchStatus=MS_MATCHEND;
				m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

				//解散游戏
				for (INT i=0; i<m_pGameServiceOption->wTableCount; i++)
				{
					//获取对象
					ITableFrame * pITableFrame=m_ppITableFrame[i];					
					//ASSERT(pITableFrame!=NULL);
					if (pITableFrame==NULL) continue;

					//解散游戏
					if (pITableFrame->IsGameStarted()) 
					{
						pITableFrame->DismissGame();
					}

					//执行起立
					PerformAllUserStandUp(pITableFrame);
				}

				//移除所有
				m_DistributeManage.RemoveAll();

				//获取时间
				CTime CurTime=CTime::GetCurrentTime();
				CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

				//比赛结束
				if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND || CurTime>=MatchEndTime)
				{
					//比赛结束
					DBR_GR_MatchOver MatchOver;
					ZeroMemory(&MatchOver,sizeof(MatchOver));

					//构造数据				
					MatchOver.cbMatchType=MATCH_TYPE_LOCKTIME;
					MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
					MatchOver.lMatchNo=m_pMatchOption->lMatchNo;					
					//MatchOver.wPlayCount=m_pLockTimeMatch->wMinPlayCount;
					MatchOver.wServerID=m_pGameServiceOption->wServerID;
					MatchOver.bMatchFinish=(CurTime>=MatchEndTime)?TRUE:FALSE;

					//常规开赛
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
					{
						MatchOver.MatchEndTime=m_pLockTimeMatch->MatchEndTime;
						MatchOver.MatchStartTime=m_pLockTimeMatch->MatchStartTime;
					}	

					//循环开赛
					//if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
					//{
					//	//变量定义
					//	SYSTEMTIME SystemTimeCurr;
					//	CurTime.GetAsSystemTime(SystemTimeCurr);

					//	//设置时间
					//	SystemTimeCurr.wHour=m_pLockTimeMatch->MatchStartTime.wHour;
					//	SystemTimeCurr.wMinute=m_pLockTimeMatch->MatchStartTime.wMinute;
					//	SystemTimeCurr.wSecond=m_pLockTimeMatch->MatchStartTime.wSecond;

					//	//开始时间
					//	MatchOver.MatchStartTime=SystemTimeCurr;

     //                   //设置时间
					//	SystemTimeCurr.wHour=m_pLockTimeMatch->MatchEndTime.wHour;
					//	SystemTimeCurr.wMinute=m_pLockTimeMatch->MatchEndTime.wMinute;
					//	SystemTimeCurr.wSecond=m_pLockTimeMatch->MatchEndTime.wSecond;

					//	//结束时间
					//	MatchOver.MatchEndTime=SystemTimeCurr;						
					//}									

					//投递请求
					m_pIDataBaseEngine->PostDataBaseRequest(MatchOver.dwMatchID,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
					
					//切换时钟
					if (CurTime<MatchEndTime) SwitchMatchTimer();					
						
					return true;
				}

				//发送结束消息
				TCHAR szMessage[128]=TEXT("今天的比赛到此结束，请您明天再来参加！");
				m_pIGameServiceFrame->SendGameMessage(szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);

				//切换时钟
				SwitchMatchTimer();				
			}

			return true;
		}*/
	}
	return true;
}

//数据库事件
bool CLockTimeMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdt *** OnEventDataBase wRequestID = %d"), wRequestID);
	//OutputDebugString(szInfo);

	switch(wRequestID)
	{
	case DBO_GR_MATCH_SIGNUP_RESULT:		//报名结果
		{
			//参数效验
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			
			
			//报名失败
			if (pMatchSignupResult->bResultCode==false)
			{
				//提示消息不为空
				if (pMatchSignupResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

					TCHAR szMessage[128];
					_sntprintf_s(szMessage, CountArray(szMessage), TEXT("玩家:%d 登录失败,【%s】"), pIServerUserItem->GetUserID(), pMatchSignupResult->szDescribeString);
					CTraceService::TraceString(szMessage, TraceLevel_Warning);
				}

				return true;
			}

			//测试代码
			if (pIServerUserItem->IsAndroidUser() == false)
			{
				TCHAR szMessage[128];
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("玩家:%d 登录完成"), pIServerUserItem->GetUserID());
				CTraceService::TraceString(szMessage, TraceLevel_Warning);
			}

			//发送分数
			SendMatchUserInitScore(pIServerUserItem);

			//设置状态
			pIServerUserItem->SetUserMatchStatus(MUS_SIGNUPED);

			//报名状态		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));		

			////使用保险卡
			//if (m_pLockTimeMatch->cbReviveEnabled && m_pLockTimeMatch->cbSafeCardEnabled && pIServerUserItem->IsAndroidUser()==false)
			//{
			//	SendSafeCardNotify(pIServerUserItem);
			//}
			//else
			{
				//设置用户
				m_LockOnMatchUserMap[pIServerUserItem->GetUserID()] = pIServerUserItem;

				if (pIServerUserItem->IsAndroidUser() == true)
				{
					m_LockAndroidArray.Add(pIServerUserItem->GetUserID());
				}
				
				TCHAR szMessage[128];
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("报名结果:用户ID:%d,已报名人数：%d 人."), pIServerUserItem->GetUserID(), m_LockOnMatchUserMap.GetCount());
				CTraceService::TraceString(szMessage, TraceLevel_Exception);
				
				//SendSignupSuccessNotify(pIServerUserItem);				
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("恭喜您，报名成功，请等待系统为您分配座位！"), SMT_CHAT);
				
                //距离比赛开始剩余时间s
                //获取时间
                CTime CurTime = CTime::GetCurrentTime();
       
                CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);		//比赛开始

                //变量定义
                CTimeSpan TimeSpan = MatchStartTime - CurTime;
                DWORD dwTimeLeft = (DWORD)TimeSpan.GetTotalSeconds();

                SendMatchInfo(pIServerUserItem, dwTimeLeft);
			}

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//发送数据
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//退赛结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;

			//系统取消
			if (pMatchSignupResult->dwReason==UNSIGNUP_REASON_SYSTEM) return true;

			//退费失败
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("退赛结果:用户ID:%d,已报名人数：%d 人."), pIServerUserItem->GetUserID(), m_LockOnMatchUserMap.GetCount());
			CTraceService::TraceString(szMessage, TraceLevel_Exception);
			
			if (pIServerUserItem->IsAndroidUser() == true)
			{
				for (int index = 0; index < m_LockAndroidArray.GetCount(); index++)
				{
					if (pIServerUserItem->GetUserID() == m_LockAndroidArray[index])
					{
						m_LockAndroidArray.RemoveAt(index);
						break;
					}
				}
			}
			//删除用户
			m_LockOnMatchUserMap.RemoveKey(pIServerUserItem->GetUserID());			
			
			SendMatchInfo(NULL);
			//获取成绩
			tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();

			//设置成绩
			pUserScore->dwWinCount=0L;
			pUserScore->dwLostCount=0L;
			pUserScore->dwFleeCount=0L;
			pUserScore->dwDrawCount=0L;
			pUserScore->lIntegralCount=0L;
			pUserScore->lScore-=m_pLockTimeMatch->lMatchInitScore;

			//发送分数
			SendMatchUserScore(pIServerUserItem);

			//设置状态
			pIServerUserItem->SetUserMatchStatus(MUS_NULL);

			//报名状态		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));
						
			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//发送数据
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_ROUNDS_RANK_LIST:
		{
			//参数校验
			//ASSERT(wDataSize <= sizeof(DBO_GR_MatchRankList));
			if (wDataSize > sizeof(DBO_GR_MatchRankList)) return false;

			//提取数据
			DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;
			RankingPromotion(pMatchRankList);
			
			break;				
		}
	case DBO_GR_MATCH_RANK_LIST:			//比赛排行
		{
			//参数校验
			//ASSERT(wDataSize<=sizeof(DBO_GR_MatchRankList));
			if(wDataSize>sizeof(DBO_GR_MatchRankList)) return false;

			//提取数据
			DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;

			//变量定义
			IServerUserItem * pIRewardUserItem = NULL;
			tagMatchRankInfo * pMatchRankInfo = NULL;

			//发放奖励
			for (WORD wIndex = 0; wIndex<pMatchRankList->wUserCount; wIndex++)
			{
				//查找玩家
				pMatchRankInfo = &pMatchRankList->MatchRankInfo[wIndex];
				pIRewardUserItem = m_pIServerUserManager->SearchUserItem(pMatchRankInfo->dwUserID);
				if (pIRewardUserItem == NULL || pIRewardUserItem->IsAndroidUser() == true) continue;
				if (wIndex == 0)
				{
					TCHAR szStr[128];
					if (pMatchRankInfo->lRewardGold>0 && pMatchRankInfo->lRewardIngot > 0)
						_sntprintf_s(szStr, CountArray(szStr), TEXT("%d 金币,%d 钻石"), pMatchRankInfo->lRewardGold, pMatchRankInfo->lRewardIngot);
					else if (pMatchRankInfo->lRewardGold > 0)
						_sntprintf_s(szStr, CountArray(szStr), TEXT("%d 金币"), pMatchRankInfo->lRewardGold);
					else
						_sntprintf_s(szStr, CountArray(szStr), TEXT("%d 钻石"), pMatchRankInfo->lRewardIngot);

					TCHAR szString[128];
					_sntprintf_s(szString, CountArray(szString), TEXT("恭喜【%s】在【%s】的比赛中获得第一名，赢得【%s】大奖！"), pIRewardUserItem->GetNickName(), m_pMatchOption->szMatchName, szStr);

					m_pIGameServiceFrame->SendSystemMessage(szString);
				}
				//写入奖励
				WriteUserAward(pIRewardUserItem, pMatchRankInfo);
			}

			//遍历桌子
			for (WORD i = 0; i < m_LockTimeTableArray.GetCount(); i++)
			{
				//桌子信息
				tagLockTimeTableInfo * pTableFrameInfo = m_LockTimeTableArray[i];
				if (pTableFrameInfo == NULL) continue;

				//桌子指针
				ITableFrame* pTableFrame = pTableFrameInfo->pTableFrame;
				if (pTableFrame == NULL) continue;

				//强制解散游戏
				if (pTableFrame->IsGameStarted() == true)
				{
					pTableFrame->DismissGame();
				}

				pTableFrame->SendGameLogData(NULL, TEXT("比赛结束所有玩家起立 TableID = %d"), pTableFrame->GetTableID());

				//執行起立
				PerformAllUserStandUp(pTableFrame);
			}
			
			//发送状态
			BYTE cbMatchStatus = MS_FREE;
			SendGroupData(MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
					
			//断开协调
			if (m_pIGameServiceFrame!=NULL && pMatchRankList->bMatchFinish) 
			{
				m_pIGameServiceFrame->DisconnectCorrespond();
			}

			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//奖励结果
		{
			//参数效验
			//ASSERT(wDataSize==sizeof(DBO_GR_MatchRewardResult));
			if(wDataSize!=sizeof(DBO_GR_MatchRewardResult)) return false;

			//提取数据
			DBO_GR_MatchRewardResult * pMatchRewardResult = (DBO_GR_MatchRewardResult*)pData;

			//查找玩家
			IServerUserItem *  pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRewardResult->dwUserID);
			if (pIRewardUserItem==NULL) return true;

			//奖励成功
			if(pMatchRewardResult->bResultCode==true)
			{
				//更新用户信息
				tagUserInfo * pUserInfo=pIRewardUserItem->GetUserInfo();

				//发送分数
				SendMatchUserScore(pIRewardUserItem);

				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;

				//发送数据
				m_pIGameServiceFrame->SendData(pIRewardUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_CANCEL_RESULT:		//取消结果
		{
			//参数效验
			//ASSERT(wDataSize==sizeof(DBO_GR_MatchCancelResult));
			if(wDataSize!=sizeof(DBO_GR_MatchCancelResult)) return false;

			//提取数据
			DBO_GR_MatchCancelResult * pMatchCancelResult = (DBO_GR_MatchCancelResult*)pData;

			//取消成功
			if (pMatchCancelResult->bSuccessed==true)
			{
				//发送结束消息
				TCHAR szMessage[128]=TEXT("因报名人数不足，取消本场比赛。已报名的玩家稍后将会收到系统退还的报名费用，请注意查收。");
				m_pIGameServiceFrame->SendRoomMessage(szMessage,/*SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME|*/SMT_CLOSE_ROOM);	
			}

			//参赛玩家退赛
			POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
			IServerUserItem *pITempUserItem = NULL;
			DWORD dwUserID = 0;
			WORD wQuitUserCount = 0;
			WORD wMatchUserCount = (WORD)m_LockOnMatchUserMap.GetCount();
			while (pos != NULL)
			{
				m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pITempUserItem);
				if (pITempUserItem != NULL)
				{
					pITempUserItem->SetUserMatchStatus(MUS_NULL);
					OnEventUserQuitMatch(pITempUserItem, UNSIGNUP_REASON_PLAYER);

					//比赛结束后代打玩家未返回，则踢出房间
					bool bAndroidUser = pITempUserItem->IsAndroidUser();
					bool bTrusteeUser = pITempUserItem->IsTrusteeUser();
					if (bAndroidUser == false && bTrusteeUser == true)
					{
						//离开房间
						if (pITempUserItem->GetBindIndex() == INVALID_WORD)
						{
							pITempUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
						}
					}
				}
			}

			//获取时间
			CTime CurTime=CTime::GetCurrentTime();
	//		CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

			//断开协调
			if (m_pIGameServiceFrame!=NULL /*&& pMatchCancelResult->bMatchFinish*/) 
			{
				m_pIGameServiceFrame->DisconnectCorrespond(); 
			}

			//移除所有
//			m_DistributeManage.RemoveAll();

			break;
		}
	}
	return true;
}

//比赛事件
bool CLockTimeMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdt *** OnEventSocketMatch wRequestID = %d"), wSubCmdID);
	//OutputDebugString(szInfo);

	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_FEE:	//比赛费用
		{
			//参数效验
			//ASSERT(wDataSize==sizeof(BYTE));
			if(wDataSize!=sizeof(BYTE)) return false;

			//报名已截止
			if (IsMatchSignupEnd() == true && pIServerUserItem->GetUserMatchStatus() == MUS_NULL)
			{
				//发送消息
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("报名已经截止，请您移驾至其他比赛房间参与比赛！"), SMT_EJECT);

				return true;
			}

			//提取数据
			BYTE cbSignupMode=*(BYTE*)pData;
			if ((cbSignupMode&m_pMatchOption->cbSignupMode)==0)return false;

			//测试代码
			if (pIServerUserItem->IsAndroidUser() == false)
			{
				TCHAR szMessage[128];
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("玩家:%d 登录"), pIServerUserItem->GetUserID());
				CTraceService::TraceString(szMessage, TraceLevel_Warning);
			}

			//变量定义
			DBR_GR_MatchSignup MatchSignup;
			ZeroMemory(&MatchSignup,sizeof(MatchSignup));

			//构造结构
			MatchSignup.cbSignupMode=cbSignupMode;			
			MatchSignup.lMatchNo=m_pMatchOption->lMatchNo;
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();			
			MatchSignup.lInitScore=m_pLockTimeMatch->lMatchInitScore;			
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();			
			MatchSignup.wMaxSignupUser=m_pLockTimeMatch->wMaxSignupUser;
            
			//拷贝机器码
			lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_SIGNUP,dwSocketID,&MatchSignup,sizeof(MatchSignup));

			return true;
		}
	case SUB_GR_LEAVE_MATCH:	//退出比赛
		{
			//参数效验
			//ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;
		
			//测试代码
		
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("玩家:%d 退出比赛"), pIServerUserItem->GetUserID());
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
			

			OnEventUserQuitMatch(pIServerUserItem,UNSIGNUP_REASON_PLAYER,0,dwSocketID);

			return true;
		}
	//case SUB_GR_MATCH_BUY_SAFECARD:	//购买保险
	//	{
	//		//参数效验
	//		//ASSERT(wDataSize==sizeof(CMD_GR_Match_BuySafeCard));
	//		if(wDataSize!=sizeof(CMD_GR_Match_BuySafeCard)) return false;

	//		//提取数据
	//		CMD_GR_Match_BuySafeCard * pMatchBuySafeCard =(CMD_GR_Match_BuySafeCard *)pData;

	//		//购买保险
	//		if (pMatchBuySafeCard->bBuySafeCard)
	//		{
	//			//构造结构
	//			DBR_GR_MatchBuySafeCard MatchBuySafeCard;

	//			//设置变量
	//			MatchBuySafeCard.dwUserID=pIServerUserItem->GetUserID();
	//			MatchBuySafeCard.dwMatchID=m_pMatchOption->dwMatchID;
	//			MatchBuySafeCard.lMatchNo=m_pMatchOption->lMatchNo;
	//			//MatchBuySafeCard.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

	//			//投递数据
	//			m_pIDataBaseEngine->PostDataBaseRequest(MatchBuySafeCard.dwUserID,DBR_GR_MATCH_BUY_SAFECARD,dwSocketID,&MatchBuySafeCard,sizeof(MatchBuySafeCard));
	//		}
	//		else
	//		{
	//			//消息通知
	//			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("系统正在努力为您分配座位，请等待！"),SMT_CHAT);

	//			//加入分组
	//			InsertDistribute(pIServerUserItem);
	//		}

	//		return true;
	//	}
	//case SUB_GR_MATCH_USER_REVIVE:	//玩家复活
	//	{
	//		//参数效验
	//		//ASSERT(pIServerUserItem!=NULL);
	//		if(pIServerUserItem==NULL) return false;

	//		////分数判断
	//		//if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()>m_pLockTimeMatch->lMatchCullScore)
	//		//{
	//		//	//消息通知
	//		//	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("您的分数未达到淘汰分数线，不需要复活！"),SMT_CHAT);

	//		//	return true;
	//		//}			

	//		//构造结构
	//		DBR_GR_MatchUserRevive MatchUserRevive;

	//		//比赛信息			
	//		MatchUserRevive.dwMatchID=m_pMatchOption->dwMatchID;
	//		MatchUserRevive.lMatchNo=m_pMatchOption->lMatchNo;
	//		MatchUserRevive.dwUserID=pIServerUserItem->GetUserID();

	//		////复活配置						
	//		//MatchUserRevive.lReviveFee=m_pLockTimeMatch->lReviveFee;
	//		//MatchUserRevive.lInitScore=m_pLockTimeMatch->lMatchInitScore;
	//		//MatchUserRevive.lCullScore=m_pLockTimeMatch->lMatchCullScore;						
	//		//MatchUserRevive.cbReviveTimes=m_pLockTimeMatch->cbReviveTimes;			

	//		//投递数据
	//		m_pIDataBaseEngine->PostDataBaseRequest(MatchUserRevive.dwUserID,DBR_GR_MATCH_USER_REVIVE,dwSocketID,&MatchUserRevive,sizeof(MatchUserRevive));

	//		return true;
	//	}
	}

	return true;
}

//用户积分
bool CLockTimeMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem,BYTE cbReason)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserItemScore"));

	//效验参数
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//构造积分
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	//构造积分
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//发送数据
	m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//变量定义
	CMD_GR_MobileUserScore MobileUserScore;
	ZeroMemory(&MobileUserScore,sizeof(MobileUserScore));

	//构造数据
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	MobileUserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//构造积分
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();
	MobileUserScore.UserScore.dBeans=pUserInfo->dBeans;

	//发送数据
	m_pIGameServiceFrame->SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));

	//即时写分
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//变量定义
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//用户信息
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//提取积分
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//比赛信息
		WriteGameScore.dwMatchID=m_pMatchOption->dwMatchID;
		WriteGameScore.lMatchNo=m_pMatchOption->lMatchNo;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore),TRUE);

		//视频回放
		/*	if(CServerRule::IsSuportPlayBack(m_pGameServiceOption->dwServerRule))
			{
			DBR_GR_WriteVideoPlayer WriteVideoPlayer;
			ZeroMemory(&WriteVideoPlayer, sizeof(DBR_GR_WriteVideoPlayer));

			memcpy(WriteVideoPlayer.szVideoID,pIServerUserItem->GetVideoNumber(),sizeof(WriteVideoPlayer.szVideoID) );
			if(WriteVideoPlayer.szVideoID[0] != '\0')
			{
			WriteVideoPlayer.dwUserID	= WriteGameScore.dwUserID;
			WriteVideoPlayer.lScore		= WriteGameScore.VariationInfo.lScore;
			WriteVideoPlayer.wChairID	= pIServerUserItem->GetChairID();
			WriteVideoPlayer.wKindID	= m_pGameServiceAttrib->wKindID;

			m_pIDataBaseEngine->PostDataBaseRequest(WriteVideoPlayer.dwUserID,DBR_GR_WRITE_VIDEO_PLAYER_INFO,0L,&WriteVideoPlayer,sizeof(WriteVideoPlayer), TRUE);

			pIServerUserItem->SetVideoNumber("");
			}
			}*/
	}

	return true;
}

//用户数据
bool CLockTimeMatch::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserItemGameData"));

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemGameData(pIServerUserItem,cbReason);
	}

	return true;
}

//用户状态
bool CLockTimeMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserItemStatus"));

	//清除数据
	if(pIServerUserItem->GetUserStatus()==US_NULL) pIServerUserItem->SetMatchData(NULL);

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wLastTableID,wLastChairID);
	}

	return true;
}

//用户权限
bool CLockTimeMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserItemRight"));

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//用户登录
bool CLockTimeMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserLogon"));

	//构造结构
	CMD_GR_Match_Rule MatchRule;

	//基本信息
	MatchRule.dwMatchID=m_pMatchOption->dwMatchID;
	MatchRule.cbMatchType=m_pMatchOption->cbMatchType;
	lstrcpyn(MatchRule.szMatchName,m_pMatchOption->szMatchName,CountArray(MatchRule.szMatchName));	

	//报名信息
	MatchRule.cbFeeType=m_pMatchOption->cbFeeType;
	MatchRule.lSignupFee=m_pMatchOption->lSignupFee;	
	MatchRule.dwExperience=m_pMatchOption->dwExperience;	
	MatchRule.cbDeductArea=m_pMatchOption->cbDeductArea;
	MatchRule.cbSignupMode=m_pMatchOption->cbSignupMode;
	MatchRule.cbMemberOrder=m_pMatchOption->cbMemberOrder;
	MatchRule.cbJoinCondition=m_pMatchOption->cbJoinCondition;	

	//排名方式
	MatchRule.cbRankingMode=m_pMatchOption->cbRankingMode;
	MatchRule.wCountInnings=m_pMatchOption->wCountInnings;
	MatchRule.cbFilterGradesMode=m_pMatchOption->cbFilterGradesMode;

	//比赛规则
	CopyMemory(MatchRule.cbMatchRule,m_pMatchOption->cbMatchRule,sizeof(MatchRule.cbMatchRule));

	//比赛奖励
	MatchRule.wRewardCount=__min(m_pMatchOption->wRewardCount,CountArray(MatchRule.MatchRewardInfo));
	CopyMemory(MatchRule.MatchRewardInfo,m_pMatchOption->MatchRewardInfo,MatchRule.wRewardCount* sizeof(MatchRule.MatchRewardInfo[0]));

	//计算大小
	WORD wDataSize=sizeof(MatchRule)-sizeof(MatchRule.MatchRewardInfo);
	wDataSize+=MatchRule.wRewardCount*sizeof(MatchRule.MatchRewardInfo[0]);

	//发送属性
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_RULE,&MatchRule,wDataSize);

	return true;
}

//用户登出
bool CLockTimeMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	return true;
}

//登录完成
bool CLockTimeMatch::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserLogonFinish"));
	//发送比赛信息
	return SendMatchInfo(pIServerUserItem);

	//发送状态
	//if (m_MatchStatus>=MatchStatus_Free && pIServerUserItem->IsAndroidUser()==false)
	//{
	//	//比赛状态		
	//	BYTE cbMatchStatus=MS_FREE;		
	//	if (m_MatchStatus==MatchStatus_End) cbMatchStatus=MS_MATCHEND;
	//	if (m_MatchStatus==MatchStatus_WaitEnd) cbMatchStatus=MS_WAITEND;
	//	if (m_MatchStatus==MatchStatus_Playing) cbMatchStatus=MS_MATCHING;		
	//	if (m_MatchStatus==MatchStatus_WaitPlay) cbMatchStatus=MS_WAITPLAY;
	//	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

	//	//淘汰判断
	//	if (pIServerUserItem->GetUserMatchStatus()==MUS_SIGNUPED)
	//	{
	//		//淘汰判断
	//		/*if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()<m_pLockTimeMatch->lMatchCullScore)
	//		{
	//			pIServerUserItem->SetUserMatchStatus(MUS_OUT);
	//		}*/
	//	}		

	//	//报名状态		
	//	BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
	//	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));		

	//	//开赛倒计时
	//	if (m_MatchStatus==MatchStatus_WaitPlay)
	//	{
	//		//加入分组
	//		if (cbUserMatchStatus==MUS_SIGNUPED) InsertDistribute(pIServerUserItem);

	//		//构造人数
	//		CMD_GR_Match_Num MatchNum;
	//		MatchNum.dwWaitting=m_DistributeManage.GetCount();
	//		MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;
	//		
	//		//发送人数
	//		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	//	}
	//}
 //
	return true;
}

//进入事件
bool CLockTimeMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//OutputDebugString(TEXT("ptdt *** OnEventEnterMatch"));

	//判断时间
	TCHAR szDescribe[256];
	WORD wMessageType;
	if (VerifyMatchTime(szDescribe,CountArray(szDescribe),wMessageType)==false)
	{
		m_pIGameServiceFrame->SendRoomMessage(dwSocketID,szDescribe,wMessageType|SMT_GLOBAL,dwUserIP==0);
		return true;
	}

	//手机用户
	if (bIsMobile == true)
	{
		//处理消息
		CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
		pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
		pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

		//变量定义
		DBR_GR_LogonMobile LogonMobile;
		ZeroMemory(&LogonMobile,sizeof(LogonMobile));

		//构造数据
		LogonMobile.dwUserID=pLogonMobile->dwUserID;
		LogonMobile.dwClientAddr=dwUserIP;		
		LogonMobile.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonMobile.lMatchNo=m_pMatchOption->lMatchNo;
		LogonMobile.cbMatchType=m_pMatchOption->cbMatchType;
		LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
		LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;		
		lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
		lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));
	}
	else
	{
		//处理消息
		CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
		pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
		pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

		//变量定义
		DBR_GR_LogonUserID LogonUserID;
		ZeroMemory(&LogonUserID,sizeof(LogonUserID));

		//构造数据
		LogonUserID.dwClientAddr=dwUserIP;
		LogonUserID.dwUserID=pLogonUserID->dwUserID;
		LogonUserID.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonUserID.lMatchNo=m_pMatchOption->lMatchNo;	
		LogonUserID.cbMatchType=m_pMatchOption->cbMatchType;
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}

	return true;
}

//用户参赛
bool CLockTimeMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserJoinMatch"));

	//参数校验
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	//判断状态
	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		return false;
	}

	if (pIServerUserItem->IsAndroidUser() == true && m_LockAndroidArray.GetCount() >= 2)
	{
		return true;
	}

	//报名未开始
	if (IsMatchSignupStart()==false)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("报名尚未开始，请稍后再来！"),SMT_EJECT);

		return true;
	}

	//报名已截止
	if (IsMatchSignupEnd()==true /*&& pIServerUserItem->GetUserMatchStatus()==MUS_NULL*/)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("报名已经截止，请您移驾至其他比赛房间参与比赛！"),SMT_EJECT);

		return true;
	}

	//判断状态
	if (pIServerUserItem->GetUserMatchStatus()!=MUS_SIGNUPED)
	{
		//扣除报名费
		if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
		{			
			//真人扣报名费
			if(m_pMatchOption->lSignupFee>0 && pIServerUserItem->IsAndroidUser()==false)
			{
				SendDeductFeeNotify(pIServerUserItem);

				return true;
			}			
		}		

		//自动报名
		if (m_pMatchOption->lSignupFee==0 || pIServerUserItem->IsAndroidUser()==true)
		{
			//报名方式
			BYTE cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;

			return OnEventSocketMatch(SUB_GR_MATCH_FEE,&cbSignupMode,sizeof(cbSignupMode),pIServerUserItem,dwSocketID);
		}
	}

	SendMatchInfo(NULL);
	
	//发送成绩
	SendMatchUserScore(pIServerUserItem);

	//发送消息
	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("系统正在努力为您分配座位，请稍后。。。"),SMT_CHAT);

	return true;
}

//用户退赛
bool CLockTimeMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserQuitMatch"));

	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	////判断比赛状态
	//if (pIServerUserItem->GetUserMatchStatus()==MUS_SIGNUPED)
	//{
	//	return true;
	//}

	//正在游戏状态
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//已进行过比赛
	if (pIServerUserItem->GetUserPlayCount()>0)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("抱歉,您已参加过本场比赛，不能退赛！"),SMT_CHAT);

		return true;
	}
	
	//删除用户
	m_LockOnMatchUserMap.RemoveKey(pIServerUserItem->GetUserID());
		
	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("用户退赛:用户ID:%d,参赛人数：%d 人,退赛原因:%d,是否AI:%d"), pIServerUserItem->GetUserID(), m_LockOnMatchUserMap.GetCount(), cbReason, pIServerUserItem->IsAndroidUser());
	CTraceService::TraceString(szMessage, TraceLevel_Exception);

	//SendMatchInfo(NULL);	
	////设置状态
	//pIServerUserItem->SetUserMatchStatus(MUS_NULL);
	
	//退还报名费
	//if(m_pMatchOption->lSignupFee>0 && !pIServerUserItem->IsAndroidUser())
	{
		//变量定义
		DBR_GR_MatchUnSignup MatchUnSignup;
		MatchUnSignup.dwReason=cbReason;
		MatchUnSignup.dwUserID=pIServerUserItem->GetUserID();

		//构造结构
		MatchUnSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
		MatchUnSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
		MatchUnSignup.dwMatchID=m_pMatchOption->dwMatchID;
		MatchUnSignup.lMatchNo=m_pMatchOption->lMatchNo;
		lstrcpyn(MatchUnSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchUnSignup.szMachineID));

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchUnSignup,sizeof(MatchUnSignup));
	}	
	//else
	//{
	//	//删除用户
	//	bool bSuccess = (m_LockOnMatchUserMap.RemoveKey(pIServerUserItem->GetUserID()) == TRUE);
	//	if (bSuccess == false)return false;

	//	SendMatchInfo(NULL);
	//
	//	//设置状态
	//	pIServerUserItem->SetUserMatchStatus(MUS_NULL);

	//	//报名状态		
	//	BYTE cbUserMatchStatus = pIServerUserItem->GetUserMatchStatus();
	//	m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_USTATUS, &cbUserMatchStatus, sizeof(cbUserMatchStatus));

	//	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT | SMT_EJECT);		
	//}
	
	//TCHAR szMessage[128];
	//_sntprintf_s(szMessage, CountArray(szMessage), TEXT("用户退赛:用户ID:%d,已报名人数：%d 人."), pIServerUserItem->GetUserID(), m_LockOnMatchUserMap.GetCount());
	//CTraceService::TraceString(szMessage, TraceLevel_Exception);
	////移除用户
	//m_LockOnMatchUserMap.RemoveKey(pIServerUserItem->GetUserID());
	//移除分组
//	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	//移除同桌信息
//	m_DistributeManage.RemoveUserSameTableInfo(pIServerUserItem->GetUserID());

	return true;
}

//游戏开始
bool CLockTimeMatch::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	//OutputDebugString(TEXT("ptdt *** OnEventGameStart"));

	//设置底分
	pITableFrame->SetCellScore((LONG)m_pLockTimeMatch->lInitalBase);
	
	//获取接口
	tagLockTimeTableInfo *pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return false;

	//设置状态
	pTableFrameInfo->bMatchTaskFinish = false;
	pTableFrameInfo->bSwtichTaskFinish = false;
	pTableFrameInfo->bWaitForNextInnings = false;

	//已完成局数加1
	if (pTableFrameInfo->cbCurPlayCount >= pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount)
	{
		//轮数+1 局数为1
		pTableFrameInfo->cbCurRounds++;
		pTableFrameInfo->cbCurPlayCount = 1;
		 
		m_cbCurMatchRounds = pTableFrameInfo->cbCurRounds + 1;
	}
	else
	{
		pTableFrameInfo->cbCurPlayCount++;
	}

	//发送比赛信息
	SendTableMatchInfo(pITableFrame, INVALID_CHAIR);

	//构造数据包
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage, sizeof(SystemMessage));

	TCHAR szMsg[256];
	_sntprintf_s(szMsg, CountArray(szMsg), TEXT("定时赛桌子:%d,第%d轮第%d局开始（共%d轮）。"), pTableFrameInfo->wTableID, pTableFrameInfo->cbCurRounds + 1, pTableFrameInfo->cbCurPlayCount, pTableFrameInfo->cbTotalRounds);
	lstrcpyn(SystemMessage.szString, szMsg, CountArray(SystemMessage.szString));
	SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
	SystemMessage.wType = SMT_CHAT;
	
	CTraceService::TraceString(szMsg, TraceLevel_Exception);

	//发送数据
	WORD wSendSize = sizeof(SystemMessage)-sizeof(SystemMessage.szString) + SystemMessage.wLength*sizeof(TCHAR);
	pITableFrame->SendTableData(INVALID_CHAIR, SUB_GF_SYSTEM_MESSAGE, (VOID*)&SystemMessage, wSendSize, MDM_GF_FRAME);

	return true;
}

//游戏结束
bool CLockTimeMatch::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//OutputDebugString(TEXT("ptdt *** OnEventGameEnd"));

	//ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return false;

	//获取桌子信息
	tagLockTimeTableInfo * pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return false;
	//变量定义
	TCHAR szMessage[128] = TEXT("");

	//完成局数	
	if (pTableFrameInfo->cbCurRounds >= pTableFrameInfo->cbTotalRounds - 1
		&& pTableFrameInfo->cbCurPlayCount >= pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount)
	{
		//等待提醒
		SendWaitMessage(pITableFrame);

		pTableFrameInfo->bMatchTaskFinish = true;
		//比赛完成
		if (CheckMatchTaskFinish() == true)
		{			
			m_pITimerEngine->SetTimer(IDI_LOCK_MATCH_FINISH, 5000, TIMES_INFINITY, 0);

			//构造消息
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("恭喜您,您已玩满%d轮游戏,系统将进行排名,请您耐心等待！桌子ID:%d"), pTableFrameInfo->cbTotalRounds, pTableFrameInfo->wTableID);
		}
		else
		{
			//构造消息
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("恭喜您,您已玩满%d轮游戏,等其他桌完成后系统将进行排名,请您耐心等待！桌子ID:%d"), pTableFrameInfo->cbTotalRounds, pTableFrameInfo->wTableID);
		}		

		//设置变量
		pTableFrameInfo->cbCurRounds = 0;
	//	pTableFrameInfo->cbTotalRounds = 0;
		pTableFrameInfo->cbCurPlayCount = 0;
		memset(&pTableFrameInfo->hMatchRounds, 0, MAX_MATCH_ROUNDS * sizeof(tagMatchRounds));
	}
	else
	{
		//换桌判断		
		if (pTableFrameInfo->cbCurPlayCount >= pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount)
		{
			//设置状态
			pTableFrameInfo->bSwtichTaskFinish = true;

			//插入空闲列表
			for (WORD i = 0; i < pITableFrame->GetChairCount(); i++)
			{
				IServerUserItem *pIServerUserItem = pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem != NULL)
				{
					InsertFreeGroup(pIServerUserItem);
				}
			}

			//检查换桌
			if (CheckSwitchTaskFinish() == true)
			{				
				m_pITimerEngine->SetTimer(IDI_LOCK_SWITCH_TABLE, 5000, TIMES_INFINITY, pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].wPromotion);
			}

			//等待提醒
			SendWaitMessage(pITableFrame);

			//构造消息
			lstrcpyn(szMessage, TEXT("本轮已结束，请耐心等待其他桌完成，稍后为您配桌..."), CountArray(szMessage));
		}
		else
		{
			//设置定时器		
			PostMatchTimer(IDI_LOCK_CONTINUE_GAME, 2, (WPARAM)pITableFrame);
			//构造消息
			lstrcpyn(szMessage, TEXT("本局已结束，请稍候..."), CountArray(szMessage));
		}
	}

	//发送消息
	pITableFrame->SendGameMessage(szMessage, SMT_CHAT | SMT_TABLE_ROLL);

	CTraceService::TraceString(szMessage, TraceLevel_Warning);
	return true;
}

//玩家返赛
bool CLockTimeMatch::OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserReturnMatch"));

	//发送比赛信息
	SendTableUserMatchInfo(pITableFrame,pIServerUserItem->GetChairID());

	return true;
}

//用户坐下
bool CLockTimeMatch::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::OnActionUserSitDown"));

	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("用户坐下:用户ID:%d,是否AI:%d,桌子:%d"), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), wTableID);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);
	return true; 
}

//用户起立
bool CLockTimeMatch::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::OnActionUserStandUp"));

	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("用户起立:用户ID:%d,是否AI:%d,桌子:%d"), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), wTableID);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);
	return true;
}

 //用户同意
bool CLockTimeMatch::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::OnActionUserOnReady"));

	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("用户同意:用户ID:%d,是否AI:%d,桌子:%d"), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), wTableID);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);
	return true; 
}

//参赛权限
bool CLockTimeMatch::VerifyUserEnterMatch(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::VerifyUserEnterMatch"));

	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//机器不受限制
	if (pIServerUserItem->IsAndroidUser()==true) return true;

	//变量定义
	WORD wMessageType;
	TCHAR szMessage[128]=TEXT("");

	//校验时间
	if (VerifyMatchTime(szMessage,CountArray(szMessage),wMessageType)==false)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,wMessageType);

		return false;
	}
	
	//最大局数
	//if (m_pLockTimeMatch->wMaxPlayCount>0 && (pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount)==m_pLockTimeMatch->wMaxPlayCount)
	{
		//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("您已完成 %d 局比赛，您将不能继续游戏，请等待比赛结束来查看排名！"),m_pLockTimeMatch->wMaxPlayCount);

		//发送消息
		if(pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);
		}
		else
		{
			m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
		}

		return false;
	}

	return true;
}

//截止报名
bool CLockTimeMatch::IsMatchSignupEnd()
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::IsMatchSignupEnd"));

	//获取时间
	CTime CurTime=CTime::GetCurrentTime();
	CTime SignupEndTime(m_pLockTimeMatch->MatchStartTime/*SignupEndTime*/);

	//常规开赛
//	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
//	{
		//比较时间
		CTimeSpan TimeSpan=CurTime-SignupEndTime;
		
		return TimeSpan.GetTotalSeconds()>=0;	
//	}
	//else
	//{
	//	//计算时间
	//	int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

	//	//判断时间
	//	return nCurrSeconds>=(SignupEndTime.GetHour()*3600+SignupEndTime.GetMinute()*60+SignupEndTime.GetSecond());
	//}		
}

//开始报名
bool CLockTimeMatch::IsMatchSignupStart()
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::IsMatchSignupStart"));

	//获取时间
	CTime CurTime=CTime::GetCurrentTime();
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	

//	//常规开赛
//	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
//	{
		//比较时间
		CTimeSpan TimeSpan = CurTime-SignupStartTime;

		return TimeSpan.GetTotalSeconds()>=0;	
//	}
	//else
	//{
	//	//计算时间
	//	int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

	//	//判断时间
	//	return nCurrSeconds>=(SignupStartTime.GetHour()*3600+SignupStartTime.GetMinute()*60+SignupStartTime.GetSecond());
	//}	
}

//开始间隔
DWORD CLockTimeMatch::GetMatchStartInterval()
{
	//获取时间
//	CTime CurTime=CTime::GetCurrentTime();
//	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);
////	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
//
//	//变量定义
//	DWORD dwTimeInterval=1;
//	DWORD dwCurrStamp,dwStartStamp,dwEndStamp;
//
//	//比赛未开始
//	if(CurTime<MatchStartTime)
//	{
//		CTimeSpan TimeSpan = MatchStartTime-CurTime;
//		dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();
//	}
//
//	//比赛已结束
////	if(CurTime>=MatchEndTime) dwTimeInterval=0;
//
//	//计算时间
//	dwCurrStamp = CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();
//	dwStartStamp = m_pLockTimeMatch->MatchStartTime.wHour*3600+m_pLockTimeMatch->MatchStartTime.wMinute*60+m_pLockTimeMatch->MatchStartTime.wSecond;
////	dwEndStamp = m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond;
//
//	//比赛未开始
//	if(dwCurrStamp<dwStartStamp) 
//	{
//		dwTimeInterval = dwStartStamp-dwCurrStamp;
//	}
//
//	//比赛已结束
//	if(dwCurrStamp>=dwEndStamp)
//	{
//		dwTimeInterval = 24*3600-(dwCurrStamp-dwStartStamp);		
//	}

//	return dwTimeInterval;
	return 0;
}

//執行起立
//VOID CLockTimeMatch::PerformAllUserStandUp(ITableFrame *pITableFrame)
//{
//	//参数校验
//	//ASSERT(pITableFrame!=NULL);
//	if (pITableFrame==NULL) return;
//
//	//处理起立
//	for (int i=0;i<m_pGameServiceAttrib->wChairCount;i++)
//	{
//		//获取用户
//		IServerUserItem * pUserServerItem=pITableFrame->GetTableUserItem(i);
//		if (pUserServerItem&&pUserServerItem->GetTableID()!=INVALID_TABLE)
//		{
//			pITableFrame->PerformStandUpAction(pUserServerItem);
//		}
//	}	
//}

//计算场次
LONGLONG CLockTimeMatch::CalcMatchNo()
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::CalcMatchNo"));

	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	//时间分量
	WORD wDay = SystemTime.wDay;
	WORD wMonth = SystemTime.wMonth;
	WORD wYear = SystemTime.wYear - 1980;
	DWORD dwSecond = SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;

	//记录变量
	static DWORD wMatchCount = 0;
	static DWORD wLastDay = SystemTime.wDay;

	//更新记录
	if (SystemTime.wDay > wLastDay)
	{
		wMatchCount = 1;
		wLastDay = SystemTime.wDay;
	}
	else
	{
		wMatchCount++;
	}

	return (LONGLONG(wYear) << 44 | LONGLONG(wMonth) << 40 | LONGLONG(wDay) << 34 | LONGLONG(dwSecond) << 16 | LONGLONG(wMatchCount));

	//循环开赛
	//if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
	//{
	//	//变量定义
	//	SYSTEMTIME SystemStartTime=m_pLockTimeMatch->MatchStartTime;

	//	//设置变量
	//	SystemStartTime.wHour=0;
	//	SystemStartTime.wMinute=0;
	//	SystemStartTime.wSecond=0;
	//	SystemStartTime.wMilliseconds=0;

	//	//获取时间
	//	CTime CurTime=CTime::GetCurrentTime();
	//	CTime MatchStartTime(SystemStartTime);

	//	//变量定义
	//	CTimeSpan TimeSpan=CurTime-MatchStartTime;

	//	return TimeSpan.GetDays()+1;
	//}

	return 1;
}
/*
//分配用户
bool CLockTimeMatch::PerformDistribute()
{
	//状态判断
	if (m_MatchStatus!=MatchStatus_Playing) return true;

	//人数校验
	if (m_DistributeManage.GetCount()<m_pMatchOption->wMinDistributeUser) return false;

	//分配用户
	while(true)
	{
		//变量定义
		CDistributeInfoArray DistributeInfoArray;

        //获取用户
		WORD wRandCount = __max(m_pMatchOption->wMaxPartakeGameUser-m_pMatchOption->wMinPartakeGameUser+1,1);
		WORD wChairCount = m_pMatchOption->wMinPartakeGameUser+rand()%wRandCount;
		WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
		if (wDistributeCount < wChairCount) break;

		//寻找位置
		ITableFrame * pICurrTableFrame=NULL;
		for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
		{
			//获取对象
			//ASSERT(m_ppITableFrame[i]!=NULL);
			ITableFrame * pITableFrame=m_ppITableFrame[i];

			//状态判断
			if (pITableFrame->GetNullChairCount()==pITableFrame->GetChairCount())
			{
				pICurrTableFrame=pITableFrame;
				break;
			}
		}

		//桌子判断
		if (pICurrTableFrame==NULL) break;

		//玩家坐下
		bool bSitSuccess=true;
		INT_PTR nSitFailedIndex=INVALID_CHAIR;
		for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			//变量定义
			WORD wChairID=INVALID_CHAIR;

			//椅子搜索
			for (WORD i=0;i<pICurrTableFrame->GetChairCount();i++)
			{
				if (pICurrTableFrame->GetTableUserItem(i)==NULL)
				{
					wChairID = i;
					break;
				}
			}

			//分配用户
			if (wChairID!=INVALID_CHAIR)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

				//用户起立
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=pIServerUserItem->GetTableID();
					ITableFrame * pITableFrame = m_ppITableFrame[wTableID];
					pITableFrame->PerformStandUpAction(pIServerUserItem);
				}

				//用户坐下
				if(pICurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					nSitFailedIndex=nIndex;
					break;
				}
			}
		}

		//坐下结果
		for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			if (bSitSuccess==true || nSitFailedIndex==nIndex)
			{
				m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[nIndex].pPertainNode);
			}
			else
			{
				//用户起立
				if (DistributeInfoArray[nIndex].pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=DistributeInfoArray[nIndex].pIServerUserItem->GetTableID();
					m_ppITableFrame[wTableID]->PerformStandUpAction(DistributeInfoArray[nIndex].pIServerUserItem);
				}
			}
		}			
	}

	return true;
}
*/
//更新状态
VOID CLockTimeMatch::UpdateMatchStatus()
{
/*	//变量定义
	CTime CurTime=CTime::GetCurrentTime();
//	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);	

	//比赛未开始
	BYTE MatchStatus=m_MatchStatus;

	//等待开始比赛
	if (MatchStatus==MatchStatus_Free && CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
	   m_pLockTimeMatch->MatchStartTime.wHour*3600+m_pLockTimeMatch->MatchStartTime.wMinute*60+m_pLockTimeMatch->MatchStartTime.wSecond)
	{
		MatchStatus=MatchStatus_WaitPlay;
	}

	//比赛进行中
	if (MatchStatus==MatchStatus_WaitPlay && m_DistributeManage.GetCount()>=m_pLockTimeMatch->wStartUserCount)
	{
		MatchStatus=MatchStatus_Playing;
	}

	//等待结束
	//if (MatchStatus==MatchStatus_Playing && CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
	//   m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond)
	//{
	//	MatchStatus=MatchStatus_WaitEnd;
	//}	

	//结束判断
	if (MatchStatus==MatchStatus_WaitEnd)  
	{
		//变量定义
		bool bContinueWait=false;

		//解散游戏
		//for (INT i=0; i<m_pGameServiceOption->wTableCount; i++)
		//{
		//	//获取对象
		//	ITableFrame * pITableFrame=m_ppITableFrame[i];								
		//	if (pITableFrame==NULL) continue;

		//	//解散游戏
		//	if (pITableFrame->IsGameStarted())
		//	{
		//		bContinueWait=true;
		//		break;
		//	}
		//}

		//无需等待
		if (bContinueWait==false)
		{
			MatchStatus=MatchStatus_End;
		}

		////强制结束
		//if ((DWORD)(CurTime.GetHour() * 3600 + CurTime.GetMinute() * 60 + CurTime.GetSecond()) >=
		//	(DWORD)(m_pLockTimeMatch->MatchEndTime.wHour * 3600 + m_pLockTimeMatch->MatchEndTime.wMinute * 60 + m_pLockTimeMatch->MatchEndTime.wSecond + m_pLockTimeMatch->wEndDelayTime))
		//{
		//	MatchStatus = MatchStatus_End;
		//}
	}

	//设置状态
	m_MatchStatus=MatchStatus; 	*/
}

//切换时钟
VOID CLockTimeMatch::SwitchMatchTimer()
{
	////比赛未开始
	//if (m_MatchStatus==MatchStatus_Free)
	//{
	//	//开始检测	
	//	m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,2000L,TIMES_INFINITY,NULL);
	//}

	////等待开始
	//if (m_MatchStatus==MatchStatus_WaitPlay)
	//{
	//	//结束检测
	//	m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,2000L,TIMES_INFINITY,NULL);
	//}

	////比赛进行中
	//if (m_MatchStatus==MatchStatus_Playing)
	//{
	//	//结束检测
	//	m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,NULL);

	//	//分组检测
	//	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pMatchOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	//}

	////等待结束
	//if (m_MatchStatus==MatchStatus_WaitEnd)
	//{
	//	//结束检测
	//	m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,NULL);
	//}

	////比赛已结束
	//if (m_MatchStatus==MatchStatus_End)
	//{
	//	//切换状态
	//	m_pITimerEngine->SetTimer(IDI_SWITCH_STATUS,2000,TIMES_INFINITY,NULL);
	//}
}

//插入用户
bool CLockTimeMatch::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//效验参数
	////ASSERT(pIServerUserItem!=NULL);
	//if (pIServerUserItem==NULL) return false;

	////比赛状态
 //	if(m_MatchStatus!=MatchStatus_WaitPlay && m_MatchStatus!=MatchStatus_Playing) return false;

	////状态判断
	////ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	//if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	////参赛校验
	//if(VerifyUserEnterMatch(pIServerUserItem)==false) return false;

	////存在判断
	//if (m_DistributeManage.SearchNode(pIServerUserItem)!=NULL) return true;

	////变量定义
	//tagDistributeInfo DistributeInfo;
	//ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	////设置变量
	//DistributeInfo.pPertainNode=NULL;	
	//DistributeInfo.dwInsertStamp=(DWORD)time(NULL);
	//DistributeInfo.pIServerUserItem=pIServerUserItem;   

	////插入节点
	//if (m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	//{
	//	return pIServerUserItem->IsAndroidUser()?false:true; 
	//}

	////等待开始
	//if (m_MatchStatus==MatchStatus_WaitPlay)
	//{
	//	//构造人数
	//	CMD_GR_Match_Num MatchNum;
	//	MatchNum.dwWaitting=m_DistributeManage.GetCount();
	//	MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;

	//	//发送人数
	//	m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	//}

	return true;
}


//清除玩家
void CLockTimeMatch::ClearSameTableUser(DWORD dwUserID)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::ClearSameTableUser"));

	//变量定义
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		pSameTableInfo->wPlayerCount=0;
		ZeroMemory(pSameTableInfo->wPlayerIDSet,sizeof(pSameTableInfo->wPlayerIDSet));
	}
}

//插入用户
void CLockTimeMatch::InsertSameTableUser(DWORD dwUserID,DWORD dwTableUserID)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::InsertSameTableUser"));

	//变量定义
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		//添加用户
		pSameTableInfo->wPlayerIDSet[pSameTableInfo->wPlayerCount++] = dwTableUserID;
	}

	return;
}

//事件校验
bool CLockTimeMatch::VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::VerifyMatchTime"));

	//变量定义
	CTime CurTime=CTime::GetCurrentTime();
//	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	
	CTime SignupEndTime(m_pLockTimeMatch->MatchStartTime/*SignupEndTime*/);

	//变量定义
//	bool bMatchRound=m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND;
	int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

	//设置类型
	wMessageType=SMT_EJECT|SMT_CHAT;

	////比赛结束
	//if (CurTime>=MatchEndTime)
	//{
	//	wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
	//	_sntprintf_s(pszMessage, wMaxCount, wMaxCount, TEXT("比赛已经结束,请您移驾至其他比赛房间参与比赛吧！"));
	//	return false;
	//}

	//比赛已结束
	/*if (nCurrSeconds >= MatchEndTime.GetHour()*3600+MatchEndTime.GetMinute()*60+MatchEndTime.GetSecond())
	{
		wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
		_sntprintf_s(pszMessage, wMaxCount, wMaxCount, TEXT("今天的比赛已经结束,请您于明天%d时%d分%d秒前来参加比赛！"), MatchStartTime.GetHour(), MatchStartTime.GetMinute(), MatchStartTime.GetSecond());
		return false;
	}*/

	//报名未开始
	if (CurTime<SignupStartTime)
	{
		wMessageType |= SMT_CLOSE_ROOM;
		_sntprintf_s(pszMessage, wMaxCount, wMaxCount, TEXT("比赛报名尚未开始,本场比赛报名时段为：%d-%d-%d %d:%d:%d至%d-%d-%d %d:%d:%d"),
			SignupStartTime.GetYear(),SignupStartTime.GetMonth(),SignupStartTime.GetDay(),SignupStartTime.GetHour(),SignupStartTime.GetMinute(),SignupStartTime.GetSecond(),
			SignupEndTime.GetYear(),SignupEndTime.GetMonth(),SignupEndTime.GetDay(),SignupEndTime.GetHour(),SignupEndTime.GetMinute(),SignupEndTime.GetSecond());
		return false;
	}


	//报名未开始
	if (/*bMatchRound &&*/ nCurrSeconds<(SignupStartTime.GetHour()*3600+SignupStartTime.GetMinute()*60+SignupStartTime.GetSecond()))
	{
		wMessageType |= SMT_CLOSE_ROOM;
		_sntprintf_s(pszMessage, wMaxCount, wMaxCount, TEXT("比赛报名尚未开始,请您于今天%d时%d分%d秒前来报名参加比赛！"), SignupStartTime.GetHour(), SignupStartTime.GetMinute(), SignupStartTime.GetSecond());
		return false;
	}	

	return true;
}

//发送用户分数
bool CLockTimeMatch::SendMatchUserScore(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendMatchUserScore"));

	//发送数据
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));

	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//构造积分
	UserScore.UserScore.dBeans=pUserInfo->dBeans;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.lScore=pUserInfo->lScore;

	return m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}

//发送分数
bool CLockTimeMatch::SendMatchUserInitScore(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendMatchUserInitScore"));

	//获取成绩
	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();

	//设置成绩
	pUserScore->dwWinCount=0L;
	pUserScore->dwLostCount=0L;
	pUserScore->dwFleeCount=0L;
	pUserScore->dwDrawCount=0L;
	pUserScore->lIntegralCount=0L;
	pUserScore->lScore=(LONGLONG)m_pLockTimeMatch->lMatchInitScore;

	//发送分数
	return SendMatchUserScore(pIServerUserItem);
}

//复活提醒
bool CLockTimeMatch::SendReviveNotify(IServerUserItem * pIServerUserItem, BYTE cbReviveTimesed, bool bUseSafeCard)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendReviveNotify"));

	if (bUseSafeCard==false)
	{
		//构造结构 
		CMD_GR_Match_Revive MatchRevive;

		//比赛信息
		MatchRevive.dwMatchID=m_pMatchOption->dwMatchID;
		MatchRevive.lMatchNO=m_pMatchOption->lMatchNo;

		////复活信息
		//MatchRevive.lReviveFee=m_pLockTimeMatch->lReviveFee;
		//MatchRevive.cbReviveTimes=m_pLockTimeMatch->cbReviveTimes;
		//MatchRevive.cbReviveTimesed=cbReviveTimesed;

		//构造消息
		//_sntprintf_s(MatchRevive.szNotifyContent,CountArray(MatchRevive.szNotifyContent),TEXT("您的比赛币低于 %d，将不能继续游戏。 您只需支付 %I64d 金币便可复活继续比赛，您确定要复活吗？"),m_pLockTimeMatch->lMatchCullScore,m_pLockTimeMatch->lReviveFee);

        //发送数据
		WORD wDataSize=sizeof(MatchRevive)-sizeof(MatchRevive.szNotifyContent);
		wDataSize+=CountStringBuffer(MatchRevive.szNotifyContent);
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_REVIVE,&MatchRevive,wDataSize);
	}
	else
	{
		//变量定义
		TCHAR szMessage[128];
		//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("您的比赛币低于 %d，将不能继续游戏。由于您购买了保险卡，系统将免费为您复活一次！"),m_pLockTimeMatch->lMatchCullScore);

		//消息通知
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT);
	}

	return true;
}

//保险提醒
bool CLockTimeMatch::SendSafeCardNotify(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendSafeCardNotify"));
	//构造结构
	//CMD_GR_Match_SafeCard MatchSafeCard;

	////设置变量
	//MatchSafeCard.dwMatchID=m_pMatchOption->dwMatchID;
	//MatchSafeCard.lMatchNO=m_pMatchOption->lMatchNo;
	////MatchSafeCard.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

	////构造消息
	//_sntprintf_s(MatchSafeCard.szNotifyContent,CountArray(MatchSafeCard.szNotifyContent),TEXT("恭喜您报名成功！本场比赛出售保险卡, 被淘汰可免费复活一次，售价 %I64d 金币，您是否要购买？"),MatchSafeCard.lSafeCardFee);

	////发送数据
	//WORD wDataSize=sizeof(MatchSafeCard)-sizeof(MatchSafeCard.szNotifyContent);
	//wDataSize+=CountStringBuffer(MatchSafeCard.szNotifyContent);
	//m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_SAFECARD,&MatchSafeCard,wDataSize);

	return true;
}

//报名提醒
bool CLockTimeMatch::SendDeductFeeNotify(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendDeductFeeNotify"));

	//构造结构
	CMD_GR_Match_Fee MatchFee;

	//设置变量
	MatchFee.cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;
	MatchFee.dwMatchID=m_pMatchOption->dwMatchID;
	MatchFee.lMatchNO=m_pMatchOption->lMatchNo;
	MatchFee.lSignupFee=m_pMatchOption->lSignupFee;
	MatchFee.cbFeeType=m_pMatchOption->cbFeeType;
	MatchFee.cbDeductArea=m_pMatchOption->cbDeductArea;	

	ZeroMemory(MatchFee.szNotifyContent,sizeof(MatchFee.szNotifyContent));

	//构造提示			
	if (m_pMatchOption->cbDeductArea==DEDUCT_AREA_SERVER && MatchFee.lSignupFee>0)
	{
		LPCTSTR pszFeeType[]={TEXT("游戏币"),TEXT("钻石")};
		_sntprintf_s(MatchFee.szNotifyContent,CountArray(MatchFee.szNotifyContent),TEXT("参赛将扣除报名费 %I64d %s，确认要参赛吗？"),m_pMatchOption->lSignupFee,pszFeeType[m_pMatchOption->cbFeeType-FEE_TYPE_GOLD]);
	}

	//发送消息
	WORD wDataSize=sizeof(MatchFee)-sizeof(MatchFee.szNotifyContent);
	wDataSize+=CountStringBuffer(MatchFee.szNotifyContent);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_FEE,&MatchFee,wDataSize);

	return true;
}

//淘汰提醒
bool CLockTimeMatch::SendEliminateNotify(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendEliminateNotify"));

	//变量定义
	TCHAR szMessage[128]=TEXT("");
	//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("由于您的比赛币低于 %d，您将不能继续游戏！"),m_pLockTimeMatch->lMatchCullScore);

	//发送消息
	m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);

	//构造结构
	DBR_GR_MatchEliminate MatchEliminate;
	MatchEliminate.cbMatchType=MATCH_TYPE_LOCKTIME;
	MatchEliminate.dwMatchID=m_pMatchOption->dwMatchID;
	MatchEliminate.lMatchNo=m_pMatchOption->lMatchNo;
	MatchEliminate.dwUserID=pIServerUserItem->GetUserID();

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(MatchEliminate.dwUserID,DBR_GR_MATCH_ELIMINATE,0,&MatchEliminate,sizeof(MatchEliminate));

	//发送数据
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_ELIMINATE,NULL,0);

	return true;
}

//报名成功
bool CLockTimeMatch::SendSignupSuccessNotify(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendSignupSuccessNotify"));

	//报名成功
	//if (m_MatchStatus!=MatchStatus_Free)
	//{
	//	//消息通知
	//	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("恭喜您，报名成功，请等待系统为您分配座位！"),SMT_CHAT);

	//	//加入分组
	//	InsertDistribute(pIServerUserItem);					
	//}
	//else
	//{
	//	//消息通知
	//	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("恭喜您，报名成功，请届时前来参加比赛！"),SMT_CHAT);
	//}

	return true;
}

//移除玩家
bool CLockTimeMatch::RemoveDistribute(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::RemoveDistribute"));

	//验证参数
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//移除用户
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}


//发送比赛信息
void CLockTimeMatch::SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendTableUserMatchInfo"));

	//验证参数
	//ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	tagLockTimeTableInfo *pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return;

	//构造数据
	CMD_GR_Match_Info MatchInfo;
	ZeroMemory(&MatchInfo,sizeof(MatchInfo));

	//设置标题
//	_sntprintf_s(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("积分方式：累计积分"));
//	_sntprintf_s(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("游戏基数：%d"), m_pLockTimeMatch->lMatchInitScore);
//	_sntprintf_s(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("局制名称：定时开赛"));

	MatchInfo.cbCurRounds = pTableFrameInfo->cbCurRounds + 1;
	MatchInfo.cbCurPlayCount = pTableFrameInfo->cbCurPlayCount;
	MatchInfo.cbTotalRounds = pTableFrameInfo->cbTotalRounds;
	MatchInfo.cbTotalPlayCount = pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount;
	MatchInfo.cbPromotion = (BYTE)pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].wPromotion;
	MatchInfo.cbStartUserCount = m_cbTotalPlayerCount;

	for(WORD i=0; i<pITableFrame->GetChairCount(); i++)
	{
		//获取用户
		if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
		IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem==NULL) continue;

		//设置局数
		//MatchInfo.wGameCount=(WORD)(pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount)+1;

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			pITableFrame->SendTableData(i,SUB_GR_MATCH_INFO,&MatchInfo,sizeof(MatchInfo),MDM_GF_FRAME);
		}

		TCHAR szMessage[128];
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("SUB_GR_MATCH_INFO:UserID:%d,ChairID:%d,CurRounds:%d,TotalRounds:%d,TotalPlayCount:%d,Promotion:%d"),
			pIServerUserItem->GetUserID(), i, MatchInfo.cbCurRounds, MatchInfo.cbTotalRounds, MatchInfo.cbTotalPlayCount, MatchInfo.cbPromotion);
		CTraceService::TraceString(szMessage, TraceLevel_Warning);
	}

	return;
}

//写入奖励
bool CLockTimeMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::WriteUserAward"));

	//参数效验
	//ASSERT(pIServerUserItem != NULL && pMatchRankInfo != NULL);
	if (pIServerUserItem == NULL || pMatchRankInfo == NULL) return false;

	//写入判断
	bool bWriteReward = (pMatchRankInfo->lRewardGold + pMatchRankInfo->lRewardIngot + pMatchRankInfo->dwRewardTicket) > 0;

	//写入奖励
	if (bWriteReward == true)
	{
		//变量定义
		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult, sizeof(MatchResult));

		//比赛奖励
		CTime::GetCurrentTime().GetAsSystemTime(MatchResult.MatchEndTime);
		MatchResult.wMatchRank = pMatchRankInfo->wRankID;
		MatchResult.lGold = pMatchRankInfo->lRewardGold;
		MatchResult.lIngot = pMatchRankInfo->lRewardIngot;
		MatchResult.dwRewardTicket = pMatchRankInfo->dwRewardTicket;
		lstrcpyn(MatchResult.szNickName, pIServerUserItem->GetNickName(), CountArray(MatchResult.szNickName));
		lstrcpyn(MatchResult.szMatchName, m_pMatchOption->szMatchName, CountArray(MatchResult.szMatchName));

		//发送数据
		if (pIServerUserItem->IsAndroidUser() == false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("获奖玩家:%d,名次: %d"), pIServerUserItem->GetUserID(), pMatchRankInfo->wRankID);
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
			//构造消息
			//TCHAR szMessage[256]=TEXT("\0");
			//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("点击查看比赛排行：http://service.foxuc.com/GameMatch.aspx?PlazaStationID=%d&TypeID=2&MatchID=%d"),
			//pIServerUserItem->GetPlazaStationID(),m_pMatchOption->dwMatchID);

			////发送消息
			//m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);	
		}
	}
	else
	{
		//构造消息
		//TCHAR szMessage[256];
		//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("您的比赛已结束，恭喜您获得第%d名，欢迎您参加其他场次的比赛！"),pMatchRankInfo->wRankID);

		////发送消息
		//SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME|SMT_MATCHRESULT);
		//变量定义

		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult, sizeof(MatchResult));

		//比赛奖励
		CTime::GetCurrentTime().GetAsSystemTime(MatchResult.MatchEndTime);
		MatchResult.wMatchRank = pMatchRankInfo->wRankID;
		lstrcpyn(MatchResult.szNickName, pIServerUserItem->GetNickName(), CountArray(MatchResult.szNickName));
		lstrcpyn(MatchResult.szMatchName, m_pMatchOption->szMatchName, CountArray(MatchResult.szMatchName));

		//发送数据
		if (pIServerUserItem->IsAndroidUser() == false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));

			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("未获奖玩家:%d,名次: %d"), pIServerUserItem->GetUserID(), pMatchRankInfo->wRankID);
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
		}
		return true;
	}

	//写入记录
	if (m_pIDataBaseEngine != NULL)
	{
		//变量定义
		DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward, sizeof(MatchReward));

		//构造结构
		MatchReward.dwUserID = pMatchRankInfo->dwUserID;
		MatchReward.lRewardGold = pMatchRankInfo->lRewardGold;
		MatchReward.lRewardIngot = pMatchRankInfo->lRewardIngot;
		MatchReward.dwRewardTicket = pMatchRankInfo->dwRewardTicket;
		MatchReward.dwClientAddr = pIServerUserItem ? pIServerUserItem->GetClientAddr() : 0;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID, DBR_GR_MATCH_REWARD, 0, &MatchReward, sizeof(MatchReward));
	}

	//参数效验
	////ASSERT(pMatchRankInfo!=NULL);
	//if(pMatchRankInfo==NULL) return false;

	////写入记录
	//if (m_pIDataBaseEngine!=NULL)
	//{
	//	//变量定义
	//	DBR_GR_MatchReward MatchReward;
	//	ZeroMemory(&MatchReward,sizeof(MatchReward));
	//	
	//	//构造结构
	//	MatchReward.dwUserID=pMatchRankInfo->dwUserID;			
	//	MatchReward.lRewardGold=pMatchRankInfo->lRewardGold;
	//	MatchReward.lRewardIngot=pMatchRankInfo->lRewardIngot;
	//	MatchReward.dwRewardExperience=pMatchRankInfo->dwRewardExperience;
	//	MatchReward.dwClientAddr=pIServerUserItem?pIServerUserItem->GetClientAddr():0;

	//	//投递数据
	//	m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID,DBR_GR_MATCH_REWARD,0,&MatchReward,sizeof(MatchReward));
	//}

	////在线玩家
	//if (pIServerUserItem!=NULL)
	//{
	//	//变量定义
	//	CMD_GR_MatchResult MatchResult;
	//	ZeroMemory(&MatchResult,sizeof(MatchResult));

	//	//比赛奖励
	//	GetSystemTime(&MatchResult.MatchEndTime);
	//	MatchResult.wMatchRank=pMatchRankInfo->wRankID;
	//	MatchResult.lGold=pMatchRankInfo->lRewardGold;		
	//	MatchResult.lIngot=pMatchRankInfo->lRewardIngot;	
	//	MatchResult.dwExperience=pMatchRankInfo->dwRewardExperience;
	//	lstrcpyn(MatchResult.szNickName,pIServerUserItem->GetNickName(),CountArray(MatchResult.szNickName));
	//	lstrcpyn(MatchResult.szMatchName,m_pMatchOption->szMatchName,CountArray(MatchResult.szMatchName));

	//	//发送数据
	//	if (pIServerUserItem->IsAndroidUser()==false)
	//	{
	//		m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
	//		
	//	}
	//}		

	return true;
}

//获取接口
tagLockTimeTableInfo* CLockTimeMatch::GetTableInterface(ITableFrame* pITableFrame)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::GetTableInterface"));

	for (int i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->pTableFrame == pITableFrame) return m_LockTimeTableArray[i];
	}

	return NULL;
}

//每轮结束
bool CLockTimeMatch::MatchRoundsOver(int wPromotion)
{
	//比赛结束
	DBR_GR_MatchRoundsOver MatchRoundsOver;
	ZeroMemory(&MatchRoundsOver, sizeof(MatchRoundsOver));

	//构造数据				
	MatchRoundsOver.bMatchFinish = TRUE;
	MatchRoundsOver.dwMatchID = m_pMatchOption->dwMatchID;
	MatchRoundsOver.lMatchNo = m_pMatchOption->lMatchNo;
	MatchRoundsOver.cbMatchType = MATCH_TYPE_IMMEDIATE;
	MatchRoundsOver.wPlayCount = m_pLockTimeMatch->cbTotalPlayCount;
	MatchRoundsOver.wServerID = m_pGameServiceOption->wServerID;
	MatchRoundsOver.cbMatchRounds = m_cbCurMatchRounds;
	MatchRoundsOver.wPromotion = wPromotion;

	//获取时间
	MatchRoundsOver.MatchStartTime = m_MatchStartTime;

	CTime::GetCurrentTime().GetAsSystemTime(MatchRoundsOver.MatchEndTime);

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(MatchRoundsOver.dwMatchID, DBR_GR_MATCH_ROUNDS_OVER, 0, &MatchRoundsOver, sizeof(MatchRoundsOver));
	
	return true;
}

//比赛结束
VOID CLockTimeMatch::MatchOver()
{
	//删除定时器
	AllKillMatchTimer();
	m_pITimerEngine->KillTimer(IDI_LOCK_CHECK_MATCH);
	m_pITimerEngine->KillTimer(IDI_LOCK_WAIT_TABLE);
	m_pITimerEngine->KillTimer(IDI_LOCK_SWITCH_TABLE);
	m_pITimerEngine->KillTimer(IDI_LOCK_MATCH_FINISH);

	//设置比赛状态
	if (m_cbMatchStatus == MatchStatus_Playing)
	{
		m_cbMatchStatus = MatchStatus_Null;
		
		//比赛结束		
		DBR_GR_MatchOver MatchOver;
		ZeroMemory(&MatchOver, sizeof(MatchOver));

		//构造数据				
		MatchOver.bMatchFinish = TRUE;
		MatchOver.dwMatchID = m_pMatchOption->dwMatchID;
		MatchOver.lMatchNo = m_pMatchOption->lMatchNo;
		MatchOver.cbMatchType = MATCH_TYPE_IMMEDIATE;
		MatchOver.wPlayCount = m_pLockTimeMatch->cbTotalPlayCount;
		MatchOver.wServerID = m_pGameServiceOption->wServerID;
		MatchOver.cbMatchRounds = m_cbCurMatchRounds;

		//获取时间
		MatchOver.MatchStartTime = m_MatchStartTime;
		CTime::GetCurrentTime().GetAsSystemTime(MatchOver.MatchEndTime);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(MatchOver.dwMatchID, DBR_GR_MATCH_OVER, 0, &MatchOver, sizeof(MatchOver));		
	}
}
//插入分组
VOID CLockTimeMatch::InsertFreeGroup(IServerUserItem * pUserServerItem)
{
	//参数校验
	//ASSERT(pUserServerItem != NULL);
	if (pUserServerItem == NULL) return;

	for (int i = 0; i < m_LockFreeUserArray.GetCount(); i++)
	{
		if (m_LockFreeUserArray[i] != NULL && m_LockFreeUserArray[i]->GetUserID() == pUserServerItem->GetUserID()) return;
	}

	m_LockFreeUserArray.Add(pUserServerItem);
}

//本桌继续游戏
VOID CLockTimeMatch::ContinueGame(ITableFrame * pITableFrame)
{
	//参数校验
	//ASSERT(pITableFrame != NULL);
	if (pITableFrame == NULL) return;

	//获取接口
	tagLockTimeTableInfo * pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return;

	//查找用户
	for (int i = 0; i < pITableFrame->GetChairCount(); i++)
	{
		//查找用户
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//处理起立
		if (pITableFrame->PerformStandUpAction(pIServerUserItem) == false)
		{
			//ASSERT(FALSE);
			CTraceService::TraceString(TEXT("继续游戏_起立失败"), TraceLevel_Exception);

			continue;
		}

		//处理坐下
		if (pITableFrame->PerformSitDownAction(i, pIServerUserItem) == false)
		{
			//ASSERT(FALSE);
			CTraceService::TraceString(TEXT("继续游戏_坐下失败"), TraceLevel_Exception);

			continue;
		}
	}

	//设置定时器
	PostMatchTimer(IDI_LOCK_CHECK_TABLE_START, 10, (WPARAM)pITableFrame);
}

//检查桌子
VOID CLockTimeMatch::CheckTableStart(ITableFrame *pITableFrame)
{
	//校验参数
	//ASSERT(pITableFrame != NULL);
	if (pITableFrame == NULL) return;

	if (pITableFrame&&pITableFrame->IsGameStarted() == false)
	{
		for (int i = 0; i < pITableFrame->GetChairCount(); i++)
		{
			IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem && pIServerUserItem->GetTableID() != INVALID_TABLE)
			{
				if (pIServerUserItem->IsClientReady() == false || pIServerUserItem->IsTrusteeUser() == true)
				{
					//测试代码				
					TCHAR szMessage[128];
					_sntprintf_s(szMessage, CountArray(szMessage), TEXT("检查桌子:玩家【%s】,桌子:%d,椅子:%d"),  pIServerUserItem->GetNickName(), pITableFrame->GetTableID(), i);
					CTraceService::TraceString(szMessage, TraceLevel_Exception);

					pITableFrame->ImitateGameOption(pIServerUserItem);
				}
			}
		}
	}
}

//发送提示
VOID CLockTimeMatch::SendWaitMessage(ITableFrame *pTableFrame, WORD wChairID/*=INVALID_CHAIR*/)
{
	//状态校验
	if (m_cbMatchStatus == MatchStatus_Null || m_cbMatchStatus == MatchStatus_Signup)
	{
		return;
	}

	//获取信息
	tagLockTimeTableInfo * pITableFameInfo = GetTableInterface(pTableFrame);
	if (pITableFameInfo == NULL) return;

	//变量定义
	WORD wPlayingTable = 0;
	for (WORD i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->pTableFrame->GetGameStatus() >= GAME_STATUS_PLAY)
		{
			wPlayingTable++;
		}
	}

	//变量定义
	CMD_GR_Match_Wait_Tip WaitTip;
	ZeroMemory(&WaitTip, sizeof(WaitTip));
	WaitTip.wPlayingTable = wPlayingTable;
	WaitTip.wUserCount = (WORD)m_LockOnMatchUserMap.GetCount();
	WaitTip.cbTotalRounds = pITableFameInfo->cbTotalRounds;
	WaitTip.cbCurRounds = pITableFameInfo->cbCurRounds + 1;
	WaitTip.cbCurPlayCount = pITableFameInfo->cbCurPlayCount;

	lstrcpyn(WaitTip.szMatchName, m_pMatchOption->szMatchName, LEN_SERVER);

	//发送比赛信息给本桌的玩家
	for (WORD i = 0; i < pTableFrame->GetChairCount(); i++)
	{
		if (wChairID != INVALID_CHAIR && wChairID != i) continue;
		IServerUserItem *pTableUserItem = pTableFrame->GetTableUserItem(i);
		if (pTableUserItem == NULL) continue;
		WaitTip.wCurTableRank = GetUserRank(pTableUserItem, pTableFrame);
		if (WaitTip.wCurTableRank == INVALID_WORD) continue;


		WaitTip.wRank = GetUserRank(pTableUserItem);
		WaitTip.lScore = pTableUserItem->GetUserScore();

		m_pIGameServiceFrame->SendData(pTableUserItem, MDM_GF_FRAME, SUB_GR_MATCH_WAIT_TIP, &WaitTip, sizeof(WaitTip));
	}

	//发送比赛信息给其他桌的玩家
	for (WORD i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (pTableFrame->GetTableID() == m_LockTimeTableArray[i]->pTableFrame->GetTableID())continue;
		if (m_LockTimeTableArray[i]->pTableFrame->GetGameStatus() >= GAME_STATUS_PLAY) continue;
		for (WORD j = 0; j < m_LockTimeTableArray[i]->pTableFrame->GetChairCount(); j++)
		{
			IServerUserItem *pTableUserItem = m_LockTimeTableArray[i]->pTableFrame->GetTableUserItem(j);
			if (pTableUserItem == NULL) continue;
			WaitTip.wCurTableRank = GetUserRank(pTableUserItem, m_LockTimeTableArray[i]->pTableFrame);
			if (WaitTip.wCurTableRank == INVALID_WORD) continue;

			WaitTip.wRank = GetUserRank(pTableUserItem);
			WaitTip.lScore = pTableUserItem->GetUserScore();

			m_pIGameServiceFrame->SendData(pTableUserItem, MDM_GF_FRAME, SUB_GR_MATCH_WAIT_TIP, &WaitTip, sizeof(WaitTip));
		}
	}
}
//检查结束
bool CLockTimeMatch::CheckMatchTaskFinish()
{
	//测试代码
	int  iFinishCount = 0;
	for (int ii = 0; ii < m_LockTimeTableArray.GetCount(); ii++)
	{
		if (m_LockTimeTableArray[ii]->bMatchTaskFinish)
			iFinishCount++;
	}
	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("总桌数:%d,已完成数:%d"),  m_LockTimeTableArray.GetCount(), iFinishCount);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);

	for (int i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->bMatchTaskFinish == false)
		{
			//测试代码			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("当前比赛:未完成的桌数:%d"), m_LockTimeTableArray[i]->wTableID);
			CTraceService::TraceString(szMessage, TraceLevel_Exception);
			return false;
		}
	}

	return true;
}

//检查换桌
bool CLockTimeMatch::CheckSwitchTaskFinish()
{
	//测试代码
	int  iFinishCount = 0;
	for (int ii = 0; ii < m_LockTimeTableArray.GetCount(); ii++)
	{
		if (m_LockTimeTableArray[ii]->bSwtichTaskFinish)
			iFinishCount++;
	}
	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("检查换桌:总桌数:%d,已完成数:%d"),  m_LockTimeTableArray.GetCount(), iFinishCount);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);

	for (int i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->bSwtichTaskFinish == false)
		{
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("检查换桌:未完成的桌数:%d"), m_LockTimeTableArray[i]->wTableID);
			CTraceService::TraceString(szMessage, TraceLevel_Exception);

			return false;
		}
	}

	return true;
}

//AI调试
VOID CLockTimeMatch::MatchAndroidDebug(WORD wPromotion)
{
	if (wPromotion == 0)
	{
		//最后结束
//		MatchOver();

		m_cbCurMatchRounds = 1;
	}
	else
	{
		//排名晋级
		MatchRoundsOver(wPromotion);
	}
}

//排名晋级
VOID CLockTimeMatch::RankingPromotion(VOID * pData)
{
	//提取数据
	DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;


	WORD wPromotion = m_pLockTimeMatch->hMatchRounds[m_cbCurMatchRounds - 1].wPromotion;

	//发放奖励
	for (int iIndex = 0; iIndex < pMatchRankList->wUserCount; iIndex++)
	{
		//变量定义
		IServerUserItem * pIRewardUserItem = NULL;
		tagMatchRankInfo * pMatchRankInfo = NULL;
		pMatchRankInfo = &pMatchRankList->MatchRankInfo[iIndex];
		for (int i = 0; i < m_LockFreeUserArray.GetCount(); i++)
		{
			if (m_LockFreeUserArray[i] != NULL && (m_LockFreeUserArray[i]->GetUserID() == pMatchRankInfo->dwUserID))
			{
				pIRewardUserItem = m_LockFreeUserArray[i];
				break;
			}
		}

		if (pIRewardUserItem == NULL || pIRewardUserItem->IsAndroidUser() == true) continue;
		pIRewardUserItem->SetUserMatchRank((BYTE)pMatchRankInfo->wRankID);

		if (iIndex < wPromotion)
		{
			//晋级的先不给奖励
			m_pIGameServiceFrame->SendData(pIRewardUserItem, MDM_GR_MATCH, SUB_GR_MATCH_PROMOTION, &pMatchRankInfo->wRankID, sizeof(WORD));

			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("晋级玩家:%d,名次: %d"), pIRewardUserItem->GetUserID(), pMatchRankInfo->wRankID);
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
		}
		else
		{
			//写入未晋级人的奖励
			WriteUserAward(pIRewardUserItem, pMatchRankInfo);
		}
	}
	//删除未晋级玩家
	for (int iIndex = 0; iIndex < pMatchRankList->wUserCount; iIndex++)
	{
		if (iIndex < wPromotion)
			continue;

		//变量定义
		IServerUserItem * pIRewardUserItem = NULL;
		tagMatchRankInfo * pMatchRankInfo = NULL;
		pMatchRankInfo = &pMatchRankList->MatchRankInfo[iIndex];
		for (int i = 0; i < m_LockFreeUserArray.GetCount(); i++)
		{
			if (m_LockFreeUserArray[i] != NULL && (m_LockFreeUserArray[i]->GetUserID() == pMatchRankInfo->dwUserID))
			{
				m_LockFreeUserArray[i]->SetUserMatchStatus(MUS_NULL);
			//	if (m_LockFreeUserArray[i]->IsAndroidUser() == true)
			//		m_pMatchSink->PromotionUserLogout(m_LockFreeUserArray[i]);

				m_LockFreeUserArray.RemoveAt(i);
				break;
			}
		}
	}

	//回收部分桌子
	int iFreeTable = wPromotion / m_pGameServiceAttrib->wChairCount;
	m_LockTimeTableArray.RemoveAt(iFreeTable, m_LockTimeTableArray.GetCount() - iFreeTable);

	//分配桌子
	PerformDistributeTable();
}
//分配桌子
VOID CLockTimeMatch::PerformDistributeTable()
{
	//检查人数
	if (CheckMatchUser()) return;

	//变量定义
	CLockTimeTableMgrArray	FreeTableArray;

	//获取空桌
	for (INT_PTR i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->pTableFrame->IsGameStarted() == false && m_LockTimeTableArray[i]->bMatchTaskFinish == false)
		{
			//获取数量
			INT_PTR cbTimer = m_LockMatchTimerArray.GetCount();
			bool cbIsInsert = true;
			for (int j = 0; j < cbTimer; j++)
			{
				//判断一下有没有卡赛的桌子在等待
				if ((m_LockMatchTimerArray[j]->dwTimerID == IDI_LOCK_CHECK_TABLE_START || m_LockMatchTimerArray[j]->dwTimerID == IDI_LOCK_CONTINUE_GAME)
					&& (ITableFrame*)(m_LockMatchTimerArray[j]->wParam) == m_LockTimeTableArray[i]->pTableFrame)
				{
					cbIsInsert = false;
					break;
				}
			}

			//插入判断
			if (cbIsInsert == true)
			{
				FreeTableArray.Add(m_LockTimeTableArray[i]);
			}
		}
	}

	for (INT_PTR i = 0; i<FreeTableArray.GetCount(); i++)
	{
		DWORD nCurUserCount = (DWORD)m_LockFreeUserArray.GetCount();
		if (nCurUserCount / FreeTableArray[i]->pTableFrame->GetChairCount()>0)
		{
			//全部起立
			PerformAllUserStandUp(FreeTableArray[i]->pTableFrame);

			//处理坐下
			for (WORD wChairID = 0; wChairID < FreeTableArray[i]->pTableFrame->GetChairCount(); wChairID++)
			{
				//用户插入该桌
				WORD wRandID = 0;
				////处理AI调试，优先把调试AI坐下
				//if (i == 0 && m_pImmediateMatch->bAndroidDebug)
				//{
				//	for (int iCount = 0; iCount < (int)m_LockFreeUserArray.GetCount(); iCount++)
				//	{
				//		//获取用户
				//		IServerUserItem *pIServerUserItem = m_LockFreeUserArray[iCount];
				//		if (pIServerUserItem == NULL) continue;

				//		if (m_DebugAndroidArray[wChairID] == pIServerUserItem->GetUserID())
				//		{
				//			wRandID = iCount;
				//			break;
				//		}
				//	}
				//}
				//else
				//{
					wRandID = rand() % (WORD)m_LockFreeUserArray.GetCount();
				//}

				//获取用户
				IServerUserItem *pIServerUserItem = m_LockFreeUserArray[wRandID];
				if (pIServerUserItem == NULL) continue;

				//先起立再坐下 让该桌的人全部起立
				if (pIServerUserItem->GetTableID() != INVALID_TABLE)
				{
					for (int m = 0; m < m_LockTimeTableArray.GetCount(); m++)
					{
						if (pIServerUserItem->GetTableID() == m_LockTimeTableArray[m]->wTableID)
						{
							m_LockTimeTableArray[m]->pTableFrame->PerformStandUpAction(pIServerUserItem);
							break;
						}
					}
				}

				if (FreeTableArray[i]->pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem))
				{
					TCHAR szMessage[128];
					_sntprintf_s(szMessage, CountArray(szMessage), TEXT("SitDownAction:玩家ID:%d,桌子ID:%d椅子ID:%d"), pIServerUserItem->GetUserID(), FreeTableArray[i]->pTableFrame->GetTableID(), wChairID);
					CTraceService::TraceString(szMessage, TraceLevel_Warning);

					m_LockFreeUserArray.RemoveAt(wRandID);
				}
				else
				{
					CTraceService::TraceString(TEXT("坐下失败？"), TraceLevel_Exception);
				}
			}

			//10秒后做一个检查 发现该桌还没有开始游戏 说明已经卡掉了
			PostMatchTimer(IDI_LOCK_CHECK_TABLE_START, 10, (WPARAM)FreeTableArray[i]->pTableFrame);
		}
	}

	return;
}

//執行起立
VOID CLockTimeMatch::PerformAllUserStandUp()
{
	//遍历桌子
	for (WORD i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		//桌子信息
		tagLockTimeTableInfo * pTableFrameInfo = m_LockTimeTableArray[i];
		if (pTableFrameInfo == NULL) continue;

		//桌子指针
		ITableFrame* pTableFrame = pTableFrameInfo->pTableFrame;
		if (pTableFrame == NULL) continue;

		//强制解散游戏
		if (pTableFrame->IsGameStarted() == true)
		{
			pTableFrame->DismissGame();
		}

		//執行起立
		PerformAllUserStandUp(pTableFrame);
	}
	return;
}

//執行起立
VOID CLockTimeMatch::PerformAllUserStandUp(ITableFrame *pITableFrame)
{
	//参数校验
	//ASSERT(pITableFrame != NULL);
	if (pITableFrame == NULL) return;

	//处理起立
	for (int i = 0; i < pITableFrame->GetChairCount(); i++)
	{
		//获取用户
		IServerUserItem * pUserServerItem = pITableFrame->GetTableUserItem(i);
		if (pUserServerItem&&pUserServerItem->GetTableID() != INVALID_TABLE)
		{
			if (pITableFrame->PerformStandUpAction(pUserServerItem))
			{
				pITableFrame->SendGameLogData(NULL, TEXT("定时赛结束所有玩家起立 TableID = %d  userid = %d 起立成功"), pITableFrame->GetTableID(), pUserServerItem->GetUserID());
			}
			else
			{
				pITableFrame->SendGameLogData(NULL, TEXT("定时赛结束所有玩家起立 TableID = %d  userid = %d 起立失败"), pITableFrame->GetTableID(), pUserServerItem->GetUserID());
			}
		}
	}
}
//捕获比赛专用定时器
VOID CLockTimeMatch::CaptureMatchTimer()
{
	for (int i = 0; i < m_LockMatchTimerArray.GetCount(); i++)
	{
		if (--m_LockMatchTimerArray[i]->iElapse <= 0)
		{
			if (m_LockMatchTimerArray[i]->dwTimerID == IDI_LOCK_CONTINUE_GAME)
			{
				//继续游戏
				ContinueGame((ITableFrame*)m_LockMatchTimerArray[i]->wParam);
			}			
			else if (m_LockMatchTimerArray[i]->dwTimerID == IDI_LOCK_CHECK_TABLE_START)
			{
				//检查桌子
				CheckTableStart((ITableFrame*)m_LockMatchTimerArray[i]->wParam);
			}
		/*	else if (m_LockMatchTimerArray[i]->dwTimerID == IDI_LOCK_ANDROID_JOIN)
			{
				m_cbCurSections = m_LockMatchTimerArray[i]->wParam;
				if (m_cbCurSections <= m_pImmediateMatch->cbTotalSection)
				{
					if (KillMatchTimer(i)) i--;

					PostMatchTimer(IDI_LOCK_ANDROID_JOIN, m_pImmediateMatch->wAndroidFullTime * 60 / m_pImmediateMatch->cbTotalSection, m_cbCurSections + 1);
					continue;;
				}
				else
					m_cbCurSections = 0;
			}*/
			//删除定时器
			if (KillMatchTimer(i)) i--;
			else
			{
				//ASSERT(false);
			}
		}
	}
}
//投递比赛专用定时器
VOID CLockTimeMatch::PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam, LPARAM lParam)
{
	tagLockMatchTimer* pMatchTimer = new tagLockMatchTimer;
	pMatchTimer->dwTimerID = dwTimerID;
	pMatchTimer->iElapse = iElapse;
	pMatchTimer->wParam = wParam;
	pMatchTimer->lParam = lParam;
	m_LockMatchTimerArray.Add(pMatchTimer);
}

//杀死比赛专用定时器
bool CLockTimeMatch::KillMatchTimer(DWORD dwTimerID, WPARAM wParam)
{
	for (int i = 0; i < m_LockMatchTimerArray.GetCount(); i++)
	{
		if (m_LockMatchTimerArray[i]->dwTimerID == dwTimerID&&m_LockMatchTimerArray[i]->wParam == wParam)
		{
			tagLockMatchTimer* pMatchTimer = m_LockMatchTimerArray[i];
			m_LockMatchTimerArray.RemoveAt(i);
			SafeDelete(pMatchTimer);
			return true;
		}
	}
	return false;
}

//杀死比赛专用定时器
bool CLockTimeMatch::KillMatchTimer(INT_PTR dwIndexID)
{
	//ASSERT(dwIndexID >= 0 && dwIndexID < m_LockMatchTimerArray.GetCount());
	if (dwIndexID >= 0 && dwIndexID < m_LockMatchTimerArray.GetCount())
	{
		tagLockMatchTimer* pMatchTimer = m_LockMatchTimerArray[dwIndexID];
		m_LockMatchTimerArray.RemoveAt(dwIndexID);
		SafeDelete(pMatchTimer);
		return true;
	}
	return false;
}
//全部杀死比赛专用定时器
VOID CLockTimeMatch::AllKillMatchTimer()
{
	//测试代码			
	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("杀死比赛专用定时器"));
	CTraceService::TraceString(szMessage, TraceLevel_Debug);

	for (int i = 0; i < m_LockMatchTimerArray.GetCount();/*i++*/)
	{
		tagLockMatchTimer* pMatchTimer = m_LockMatchTimerArray[i];
		m_LockMatchTimerArray.RemoveAt(i);
		SafeDelete(pMatchTimer);
	}
}

//获取名次
WORD CLockTimeMatch::GetUserRank(IServerUserItem * pUserServerItem, ITableFrame *pITableFrame)
{
	//参数效验
	//ASSERT(pUserServerItem != NULL);
	if (pUserServerItem == NULL) return INVALID_WORD;

	//定义变量
	LONGLONG lUserScore = pUserServerItem->GetUserScore();
	WORD wRank = 1;
	DWORD dwUserID = 0;
	IServerUserItem *pLoopUserItem = NULL;

	if (pITableFrame == NULL)
	{
		//遍历容器
		POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
		while (pos != NULL)
		{
			m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pLoopUserItem);
			if (pLoopUserItem != pUserServerItem && pLoopUserItem->GetUserScore() > lUserScore)
				wRank++;
		}
	}
	else
	{
		//ASSERT(pUserServerItem->GetTableID() == pITableFrame->GetTableID());
		if (pUserServerItem->GetTableID() != pITableFrame->GetTableID()) return INVALID_WORD;

		for (WORD i = 0; i<pITableFrame->GetChairCount(); i++)
		{
			pLoopUserItem = pITableFrame->GetTableUserItem(i);
			if (pLoopUserItem == NULL) continue;

			if (pLoopUserItem != pUserServerItem && pLoopUserItem->GetUserScore()>lUserScore)
				wRank++;
		}
	}

	return wRank;
}

//为首轮插入空桌子
void CLockTimeMatch::InsertNullTable()
{
	for (int i = 0; i < m_pGameServiceOption->wTableCount; i++)
	{
		//超过就退出
		WORD nCurTableCount = (WORD)m_LockTimeTableArray.GetCount();
		if (nCurTableCount >= m_LockOnMatchUserMap.GetCount() / m_pGameServiceAttrib->wChairCount)break;

		//初始化比赛桌
		if (m_ppITableFrame[i]->GetNullChairCount() == m_pGameServiceAttrib->wChairCount)
		{
			tagLockTimeTableInfo * pTableFrameInfo = new tagLockTimeTableInfo;
			ZeroMemory(pTableFrameInfo, sizeof(tagLockTimeTableInfo));
			pTableFrameInfo->wTableID = i;
			pTableFrameInfo->pTableFrame = m_ppITableFrame[i];
			pTableFrameInfo->lBaseScore = m_pLockTimeMatch->lInitalBase;
			pTableFrameInfo->cbTotalRounds = m_pLockTimeMatch->cbTotalRounds;
			memcpy(&pTableFrameInfo->hMatchRounds[0], &m_pLockTimeMatch->hMatchRounds[0], sizeof(tagMatchRounds)*MAX_MATCH_ROUNDS);

			m_LockTimeTableArray.Add(pTableFrameInfo);
		}
	}
}
//比赛开始
VOID CLockTimeMatch::EfficacyStartMatch()
{
	//插入桌子
	InsertNullTable();

	//没有桌子的情况
	if (m_LockTimeTableArray.GetCount() < m_LockOnMatchUserMap.GetCount() / m_pGameServiceAttrib->wChairCount)
	{
		if (m_LockOnMatchUserMap.GetCount() < m_pGameServiceAttrib->wChairCount)
		{
			//构造数据包
			CMD_CM_SystemMessage Message;
			ZeroMemory(&Message, sizeof(Message));

			Message.wType = SMT_EJECT | SMT_CHAT | SMT_CLOSE_GAME;
			lstrcpyn(Message.szString, TEXT("抱歉，由于某用户强行退出比赛，该场比赛提前结束。"), CountArray(Message.szString));

			Message.wLength = CountStringBuffer(Message.szString);
			//发送数据
			WORD wSendSize = sizeof(Message)-sizeof(Message.szString) + Message.wLength*sizeof(TCHAR);
			SendGroupData(MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &Message, wSendSize);
			m_cbMatchStatus = MatchStatus_Signup;
			return;
		}

	//	m_cbMatchStatus = MatchStatus_Wait;
		//3秒后继续调用该函数直到有桌子
	//	SetMatchGameTimer(IDI_LOCK_WAIT_TABLE, 3000, 1, NULL);

		//构造数据包
		CMD_CM_SystemMessage Message;
		ZeroMemory(&Message, sizeof(Message));

		Message.wType = SMT_CHAT;
		_sntprintf_s(Message.szString, CountArray(Message.szString),TEXT("抱歉，由于过多用户桌子数量不够！还差几张桌子开赛,请等待。。。"));
		Message.wLength = CountStringBuffer(Message.szString);

		//发送数据
		WORD wSendSize = sizeof(Message)-sizeof(Message.szString) + Message.wLength*sizeof(TCHAR);
		SendGroupData(MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &Message, wSendSize);

		return;
	}

	//预赛安排座位
	DWORD dwUserID = 0L;
	IServerUserItem *pIServerUserItem = NULL;
	POSITION pos = m_LockOnMatchUserMap.GetStartPosition();

	WORD wChairID = 0;
	WORD wAndroidDebug = 0;
	while (pos != NULL)
	{
		//获取用户
		m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pIServerUserItem);
		//ASSERT(pIServerUserItem != NULL);
		if (pIServerUserItem == NULL) continue;

		InsertFreeGroup(pIServerUserItem);

		//调试AI
	/*	if (m_pImmediateMatch->bAndroidDebug == TRUE && wAndroidDebug < m_wChairCount && pIServerUserItem->IsAndroidUser() == true)
		{
			m_DebugAndroidArray.Add(pIServerUserItem->GetUserID());

			wAndroidDebug++;
		}*/

		//设置用户比赛中
		pIServerUserItem->SetUserMatchStatus(MUS_PLAYING);
		pIServerUserItem->SetMatchData(this);

		//发送状态
		BYTE cbUserMatchStatus = pIServerUserItem->GetUserMatchStatus();
		m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_USTATUS, &cbUserMatchStatus, sizeof(cbUserMatchStatus));
	}

	//桌子自动拉人
	PerformDistributeTable();

	//设置状态	
	m_cbMatchStatus = MatchStatus_Playing;

	//记录时间
	CTime::GetCurrentTime().GetAsSystemTime(m_MatchStartTime);

	//开始通知
	OnEventMatchStart();

	//设置定时器
	m_pITimerEngine->SetTimer(IDI_LOCK_CHECK_MATCH, 1000, TIMES_INFINITY, 0);	
}

//发送数据
bool CLockTimeMatch::SendMatchInfo(IServerUserItem * pIServerUserItem, DWORD Seconds)
{
	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting = (DWORD)m_LockOnMatchUserMap.GetCount();
	MatchNum.dwTotal = m_cbTotalPlayerCount;
    MatchNum.dwSeconds = Seconds;
	
	if (pIServerUserItem)m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_NUM, &MatchNum, sizeof(MatchNum));

	m_pIGameServiceFrame->SendMatchMessage(m_pMatchOption->lMatchNo, m_LockOnMatchUserMap.GetCount());

	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("更新人数,当前比赛:%d 人."), m_LockOnMatchUserMap.GetCount());
	CTraceService::TraceString(szMessage, TraceLevel_Exception);
    
    if(pIServerUserItem == NULL || (pIServerUserItem != NULL && Seconds != 0))
    {
        return true;
    }
	//发送比赛信息
	m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_NUM, &MatchNum, sizeof(MatchNum));
	//	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_DESC, &m_MatchDesc, sizeof(m_MatchDesc));

	//发送报名状态
	POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem = NULL;
	DWORD dwUserID = 0;
	while (pos != NULL)
	{
		m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pUserItem);
		if (pUserItem && pIServerUserItem == pUserItem)
		{
			BYTE cbUserMatchStatus = pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_USTATUS, &cbUserMatchStatus, sizeof(cbUserMatchStatus));

			return true;
		}
	}

	tagUserInfo *pUserScore = pIServerUserItem->GetUserInfo();
	pUserScore->dwWinCount = pUserScore->dwLostCount = pUserScore->dwFleeCount = pUserScore->dwDrawCount = pUserScore->cbGender = 0;
	if ((LONGLONG)m_pLockTimeMatch->lMatchInitScore != 0)
	{
		pUserScore->lScore = (LONG)m_pLockTimeMatch->lMatchInitScore;
	}

	return SendGroupUserScore(pIServerUserItem);
}

//发送用户分数
bool CLockTimeMatch::SendGroupUserScore(IServerUserItem * pIServerUserItem)
{
	//发送数据
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore, sizeof(UserScore));

	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID = pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
	//UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	//UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	//UserScore.UserScore.lIntegralCount =pUserInfo->lIntegralCount;

	//构造积分
	UserScore.UserScore.lScore = pUserInfo->lScore;
	//UserScore.UserScore.lIngot=pUserInfo->lIngot;
	//UserScore.UserScore.dBeans=pUserInfo->dBeans;

	POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem = NULL;
	DWORD dwUserID = 0;
	while (pos != NULL)
	{
		m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pUserItem);
		if (pUserItem)
			m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_USER, SUB_GR_USER_SCORE, &UserScore, sizeof(UserScore));
	}
	return true;	
}

//发送数据到一组用户
bool CLockTimeMatch::SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem = NULL;
	DWORD dwUserID = 0;
	while (pos != NULL)
	{
		m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pUserItem);
		if (pUserItem)
			m_pIGameServiceFrame->SendData(pUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
	}
	return true;
}

//报名人满，开始比赛
bool CLockTimeMatch::OnEventMatchStart()
{
	//变量定义
	DBR_GR_MatchStart MatchStart;

	//构造结构
	MatchStart.dwMatchID = m_pMatchOption->dwMatchID;
	MatchStart.lMatchNo = m_pMatchOption->lMatchNo;
	MatchStart.cbMatchType = m_pMatchOption->cbMatchType;
	MatchStart.wServerID = m_pGameServiceOption->wServerID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(MatchStart.dwMatchID, DBR_GR_MATCH_START, 0, &MatchStart, sizeof(MatchStart));
	
	SendMatchInfo(NULL);
	BYTE cbMatchStatus = MS_MATCHING;
	SendGroupData(MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
	
	return true;
}

//检测参赛人数
bool CLockTimeMatch::CheckMatchUser()
{
	//参赛用户不够 椅子数量就要终止比赛
	DWORD nCurUserCount = (DWORD)m_LockOnMatchUserMap.GetCount();
	if (nCurUserCount<m_pGameServiceAttrib->wChairCount)
	{
		/*//ASSERT(FALSE);
		CTraceService::TraceString(TEXT("检查人数..."),TraceLevel_Exception);
		return false;*/
		//测试代码
		TCHAR szMessage[128];
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("检测参赛人数:当前玩家数:%d,椅子数: %d"), nCurUserCount, m_pGameServiceAttrib->wChairCount);
		CTraceService::TraceString(szMessage, TraceLevel_Warning);

		//构造数据包
		CString szString;
		szString.Format(TEXT("抱歉，由于比赛用户强行退出比赛，人数不足 %d 人，提前结算。欢迎您继续报名参加比赛！"), m_pGameServiceAttrib->wChairCount);
		SendGroupMessage(szString);

		MatchOver();

		return true;
	}
	return false;
}

//发送一组信息
VOID CLockTimeMatch::SendGroupMessage(LPCTSTR pStrMessage)
{
	//构造数据包
	CMD_CM_SystemMessage Message;
	ZeroMemory(&Message, sizeof(Message));

	Message.wType = SMT_EJECT | SMT_CHAT;
	lstrcpyn(Message.szString, pStrMessage, CountArray(Message.szString));
	Message.wLength = CountStringBuffer(Message.szString);

	//发送数据
	WORD wSendSize = sizeof(Message)-sizeof(Message.szString) + Message.wLength*sizeof(TCHAR);
	SendGroupData(MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &Message, wSendSize);
}

//发送比赛信息
VOID CLockTimeMatch::SendTableMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//验证参数
	//ASSERT(pITableFrame != NULL);
	if (pITableFrame == NULL) return;

	tagLockTimeTableInfo *pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return;

	//变量定义
	WORD wChairCount = pITableFrame->GetChairCount();
	TCHAR szMsg[256] = TEXT("");

	if (m_cbMatchStatus == MatchStatus_Playing)
	{
		//构造数据
		CMD_GR_Match_Info MatchInfo;
		ZeroMemory(&MatchInfo, sizeof(MatchInfo));

		MatchInfo.cbCurRounds = pTableFrameInfo->cbCurRounds + 1;
		MatchInfo.cbCurPlayCount = pTableFrameInfo->cbCurPlayCount;
		MatchInfo.cbTotalRounds = pTableFrameInfo->cbTotalRounds;
		MatchInfo.cbTotalPlayCount = pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount;
		MatchInfo.cbPromotion = (BYTE)pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].wPromotion;
		MatchInfo.cbStartUserCount = m_cbTotalPlayerCount;


		//wChairID为INVALID_CHAIR发给本桌所有玩家，否则只发给指定玩家
		for (WORD i = 0; i < wChairCount; i++)
		{
			//获取用户
			if (wChairID != INVALID_CHAIR && wChairID != i) continue;
			IServerUserItem *pIServerUserItem = pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;
			MatchInfo.cbCurMatchRank = pIServerUserItem->GetUserMatchRank();

			//发送数据
			pITableFrame->SendTableData(i, SUB_GR_MATCH_INFO, &MatchInfo, sizeof(MatchInfo), MDM_GF_FRAME);

			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("SUB_GR_MATCH_INFO:UserID:%d,ChairID:%d,CurRounds:%d,TotalRounds:%d,TotalPlayCount:%d,Promotion:%d"),
				pIServerUserItem->GetUserID(), i, MatchInfo.cbCurRounds, MatchInfo.cbTotalRounds, MatchInfo.cbTotalPlayCount, MatchInfo.cbPromotion);
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
		}

		//发送提示
		if ((wChairID != INVALID_CHAIR) && (pTableFrameInfo->bWaitForNextInnings == true))
		{
			SendWaitMessage(pITableFrame, wChairID);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
