#include "StdAfx.h"
#include "AndroidUserManager.h"

//////////////////////////////////////////////////////////////////////////////////

//常量定义
#define ANDROID_LOGON_COUNT			32									//登录数目
#define ADNDROID_PLAY_DRAW          10                                  //游戏局数  
#define ANDROID_UNLOAD_TIME			30*60								//不加载时间
#define ANDROID_UNIN_TIME			10*60								//不进入时间

//时间标识
#define IDI_ANDROID_IN				(IDI_REBOT_MODULE_START+0)			//进入时间
#define IDI_ANDROID_OUT				(IDI_REBOT_MODULE_START+1)			//退出时间
#define IDI_ANDROID_PULSE			(IDI_REBOT_MODULE_START+2)			//脉冲时间
#define IDI_ANDROID_DYNAMIC			(IDI_REBOT_MODULE_START+3)			//动态时间

//机器时间
#ifdef _DEBUG		
#define TIME_ANDROID_INOUT		60L									//进出时间
#define TIME_ANDROID_PULSE		1L									//脉冲时间
#define TIME_ANDROID_DYNAMIC	3L									//动态时间
#else
#define TIME_ANDROID_INOUT		600L								//进出时间
#define TIME_ANDROID_PULSE		1L									//脉冲时间
#define TIME_ANDROID_DYNAMIC	3L									//动态时间
#endif

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserManager::CAndroidUserManager()
{
	//系统参数
	m_pGameParameter = NULL;
	m_pGameServiceAttrib = NULL;
	m_pGameServiceOption = NULL;
	m_pGameMatchOption = NULL;
	m_pGameChatInfo = NULL;

	//组件接口
	m_pITimerEngine = NULL;
	m_pIServerUserManager = NULL;
	m_pIGameServiceManager = NULL;
	m_pIGameServiceSustomTime = NULL;
	m_pITCPNetworkEngineEvent = NULL;
	m_pIMainServiceFrame = NULL;
	m_ppITableFrame = NULL;

	//配置变量	
	m_dwMinSitInterval = 0;
	m_dwMaxSitInterval = 0;
	m_bServiceContinue = false;
	m_wAutoAndroidCount = ANDROID_LOGON_COUNT;

	//库存变量
	m_wStockCount = 0;
	ZeroMemory(&m_AndroidParameter, sizeof(m_AndroidParameter));

	//设置索引
	m_AndroidUserItemMap.InitHashTable(PRIME_ANDROID_USER);
	m_AndroidUserItemMap.RemoveAll();

	return;
}

//析构函数
CAndroidUserManager::~CAndroidUserManager()
{
	//清理断言
	//ASSERT(m_ppITableFrame == NULL);
	//ASSERT(m_AndroidUserItemMap.GetCount() == 0L);
	//ASSERT(m_AndroidUserItemArray.GetCount() == 0L);
	//ASSERT(m_AndroidUserItemBuffer.GetCount() == 0L);
	//ASSERT(m_AccountsInfoArray.GetCount() == 0L);
	//ASSERT(m_AccountsInfoArrayBuffer.GetCount() == 0L);
	//ASSERT(m_LeaveInfoArray.GetCount() == 0L);

	return;
}

//接口查询
VOID * CAndroidUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserManager, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserManager, Guid, dwQueryVer);
	return NULL;
}

//启动服务
bool CAndroidUserManager::StartService()
{
	//时间间隔
	DWORD dwTimeCell = TIME_CELL;
	DWORD dwElapse = TIME_ANDROID_PULSE * 1000L;

	//调整时间
	if (m_pIGameServiceSustomTime != NULL)
	{
		dwTimeCell = m_pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell = TIME_CELL;

		dwElapse = m_pIGameServiceSustomTime->GetAndroidTimerPulse();
		if (dwElapse<dwTimeCell) dwElapse = dwTimeCell;
	}

	//启动时间
	m_pITimerEngine->SetTimer(IDI_ANDROID_PULSE, dwElapse, TIMES_INFINITY, 0);

	return true;
}

//停止服务
bool CAndroidUserManager::ConcludeService()
{
	//释放资源
	SafeDeleteArray(m_ppITableFrame);

	//删除存储
	for (INT_PTR i = 0; i<m_AndroidUserItemArray.GetCount(); i++)
	{
		SafeRelease(m_AndroidUserItemArray[i]);
	}

	for (INT_PTR i = 0; i<m_AndroidUserItemBuffer.GetCount(); i++)
	{
		SafeRelease(m_AndroidUserItemBuffer[i]);
	}

	for (INT_PTR i = 0; i<m_AccountsInfoArray.GetCount(); i++)
	{
		SafeDelete(m_AccountsInfoArray[i]);
	}

	for (INT_PTR i = 0; i<m_AccountsInfoArrayBuffer.GetCount(); i++)
	{
		SafeDelete(m_AccountsInfoArrayBuffer[i]);
	}

	for (INT_PTR i = 0; i<m_LeaveInfoArray.GetCount(); i++)
	{
		SafeDelete(m_LeaveInfoArray[i]);
	}

	//清理数组
	m_AndroidUserItemMap.RemoveAll();
	m_AndroidUserItemArray.RemoveAll();
	m_AndroidUserItemBuffer.RemoveAll();
	m_AccountsInfoArray.RemoveAll();
	m_AccountsInfoArrayBuffer.RemoveAll();
	m_LeaveInfoArray.RemoveAll();

	//库存用户
	m_wAutoAndroidCount = ANDROID_LOGON_COUNT;
	m_wStockCount = 0;
	ZeroMemory(&m_AndroidParameter, sizeof(m_AndroidParameter));

	return true;
}

//配置组件
bool CAndroidUserManager::InitAndroidUser(tagAndroidUserParameter & AndroidUserParameter)
{
	//服务配置
	m_bServiceContinue = AndroidUserParameter.bServiceContinue;
	m_dwMinSitInterval = AndroidUserParameter.dwMinSitInterval;
	m_dwMaxSitInterval = AndroidUserParameter.dwMaxSitInterval;

	//设置变量
	m_pGameParameter = AndroidUserParameter.pGameParameter;
	m_pGameServiceAttrib = AndroidUserParameter.pGameServiceAttrib;
	m_pGameServiceOption = AndroidUserParameter.pGameServiceOption;
	m_pGameMatchOption = AndroidUserParameter.pGameMatchOption;

	//组件接口
	m_pITimerEngine = AndroidUserParameter.pITimerEngine;
	m_pIServerUserManager = AndroidUserParameter.pIServerUserManager;
	m_pIGameServiceManager = AndroidUserParameter.pIGameServiceManager;
	m_pIGameServiceSustomTime = AndroidUserParameter.pIGameServiceSustomTime;
	m_pITCPNetworkEngineEvent = AndroidUserParameter.pITCPNetworkEngineEvent;

	//创建桌子
	if (m_ppITableFrame == NULL)
	{
		m_ppITableFrame = new ITableFrame*[m_pGameServiceOption->wTableCount];
		ZeroMemory(m_ppITableFrame, sizeof(ITableFrame*)* m_pGameServiceOption->wTableCount);
	}

	//屏蔽读取AI配置
	return true;

	//读取AI配置
	//工作目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\AIConfig\\GameAIParameter%d.ini"), szDirectory, AndroidUserParameter.pGameServiceOption->wServerID);
	BYTE cbReadAIParameter = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("cbReadAIParameter"), 0, szFileName);

	//读取
	if (cbReadAIParameter == TRUE)
	{
		//AI数目基数
		m_RoomAIParameter.dwAICountBase = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("dwAICountBase"), 0, szFileName);

		//时间段配置(12个时段)，每时段人数范围比例及每时段进出比例
		for (WORD i = 0; i < AI_TIME_SLOT; i++)
		{
			//变量定义
			CString strAITimeSlotHour;
			strAITimeSlotHour.Format(TEXT("AITimeSlot%d_wHour"), i);

			CString strAITimeSlotMinute;
			strAITimeSlotMinute.Format(TEXT("AITimeSlot%d_wMinute"), i);

			CString strMinAICountRate;
			strMinAICountRate.Format(TEXT("wMinAICountRate%d"), i);

			CString strMaxAICountRate;
			strMaxAICountRate.Format(TEXT("wMaxAICountRate%d"), i);

			CString strMinAIInoutRate;
			strMinAIInoutRate.Format(TEXT("wMinAIInoutRate%d"), i);

			CString strMaxAIInoutRate;
			strMaxAIInoutRate.Format(TEXT("wMaxAIInoutRate%d"), i);

			m_RoomAIParameter.AITimeSlot[i].wHour = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAITimeSlotHour, 0, szFileName);
			m_RoomAIParameter.AITimeSlot[i].wMinute = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAITimeSlotMinute, 0, szFileName);
			m_RoomAIParameter.wMinAICountRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strMinAICountRate, 0, szFileName);
			m_RoomAIParameter.wMaxAICountRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strMaxAICountRate, 0, szFileName);

			m_RoomAIParameter.wMinAIInoutRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strMinAIInoutRate, 0, szFileName);
			m_RoomAIParameter.wMaxAIInoutRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strMaxAIInoutRate, 0, szFileName);
		}

		//进入携带金币为游戏配置第N个筹码倍数以及人数占比
		for (WORD i = 0; i < AI_CARRY_GRADE; i++)
		{
			//变量定义
			CString strAICarryScoreMinMultiple;
			strAICarryScoreMinMultiple.Format(TEXT("lAICarryScoreMinMultiple%d"), i);

			CString strAICarryScoreMaxMultiple;
			strAICarryScoreMaxMultiple.Format(TEXT("lAICarryScoreMaxMultiple%d"), i);

			CString strAIScoreLevelRate;
			strAIScoreLevelRate.Format(TEXT("wAIScoreLevelRate%d"), i);

			CString strAIScoreFloatMinRate;
			strAIScoreFloatMinRate.Format(TEXT("wAIScoreFloatMinRate%d"), i);

			CString strAIScoreFloatMaxRate;
			strAIScoreFloatMaxRate.Format(TEXT("wAIScoreFloatMaxRate%d"), i);

			m_RoomAIParameter.lAICarryScoreMinMultiple[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAICarryScoreMinMultiple, 0, szFileName);
			m_RoomAIParameter.lAICarryScoreMaxMultiple[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAICarryScoreMaxMultiple, 0, szFileName);
			m_RoomAIParameter.wAIScoreLevelRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAIScoreLevelRate, 0, szFileName);
			m_RoomAIParameter.wAIScoreFloatMinRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAIScoreFloatMinRate, 0, szFileName);
			m_RoomAIParameter.wAIScoreFloatMaxRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAIScoreFloatMaxRate, 0, szFileName);
		}

		//机器离开等级概率
		for (WORD i = 0; i < AI_LEAVE_GRADE; i++)
		{
			//变量定义
			CString strAILeaveRate;
			strAILeaveRate.Format(TEXT("wAILeaveRate%d"), i);

			m_RoomAIParameter.wAILeaveRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAILeaveRate, 0, szFileName);
		}

		//机器人挂机
		m_RoomAIParameter.wAIHangingMinRate = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("wAIHangingMinRate"), 0, szFileName);
		m_RoomAIParameter.wAIHangingMaxRate = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("wAIHangingMaxRate"), 0, szFileName);

		//筛选概率
		for (WORD i = 0; i < AI_HANGING_GRADE; i++)
		{
			//变量定义
			CString strSetHangingRate;
			strSetHangingRate.Format(TEXT("wSetHangingRate%d"), i);

			CString strHangingEnterMinTime;
			strHangingEnterMinTime.Format(TEXT("wHangingEnterMinTime%d"), i);

			CString strHangingEnterMaxTime;
			strHangingEnterMaxTime.Format(TEXT("wHangingEnterMaxTime%d"), i);

			m_RoomAIParameter.wSetHangingRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strSetHangingRate, 0, szFileName);
			m_RoomAIParameter.wHangingEnterMinTime[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strHangingEnterMinTime, 0, szFileName);
			m_RoomAIParameter.wHangingEnterMaxTime[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strHangingEnterMaxTime, 0, szFileName);
		}

		//间隔时间切换挂机状态
		m_RoomAIParameter.wMinuteChangeStatus = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("wMinuteChangeStatus"), 0, szFileName);
	}

	return true;
}

//移除参数
bool CAndroidUserManager::RemoveAndroidParameter()
{
	//关闭定时器
	m_pITimerEngine->KillTimer(IDI_ANDROID_IN);
	m_pITimerEngine->KillTimer(IDI_ANDROID_OUT);

	//库存数目
	//ASSERT(m_wStockCount == m_AccountsInfoArray.GetCount());

	//解锁空闲
	for (WORD wItem = 0; wItem < m_AccountsInfoArray.GetCount(); wItem++)
	{
		DWORD dwUserID = m_AccountsInfoArray[wItem]->dwUserID;

		//查找AI
		CAndroidUserItem * pAndroidUserItem = NULL;
		//ASSERT(m_AndroidUserItemMap.Lookup(dwUserID, pAndroidUserItem) == FALSE);

		if (m_AndroidUserItemMap.Lookup(dwUserID, pAndroidUserItem) == FALSE && m_pIMainServiceFrame != NULL)
		{
			//解锁AI
			m_pIMainServiceFrame->UnLockAndroidUser(dwUserID);
		}
	}

	//删掉库存
	m_wStockCount = 0;
	m_RoomAIParameter.dwAICountBase = 0;
	m_wAutoAndroidCount = 0;
	ZeroMemory(&m_AndroidParameter, sizeof(m_AndroidParameter));

	m_AccountsInfoArray.RemoveAll();
	m_LeaveInfoArray.RemoveAll();

	//变量定义
	WORD wAndroidUserItemCount = m_AndroidUserItemArray.GetCount();
	CAndroidUserItemArray UserLeaveArray;
	UserLeaveArray.RemoveAll();

	for (INT_PTR i = 0; i < wAndroidUserItemCount; i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//状态判断
		if (pAndroidUserItem == NULL)
		{
			continue;
		}

		//未登录成功就删除批次
		if (pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			pAndroidUserItem->m_bWaitLeave = true;

			//设置AI动作
			if (m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
			{
				pAndroidUserItem->SetAndroidAction(ANDROID_WAITSTANDUP, true);
			}

			continue;
		}

		//变量定义
		IServerUserItem * pIServerUserItem = pAndroidUserItem->m_pIServerUserItem;
		if (pIServerUserItem != NULL)
		{
			//服务状态
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			if (!(cbUserStatus == US_FREE || cbUserStatus == US_SIT))
			{
				pAndroidUserItem->m_bWaitLeave = true;

				//设置AI动作
				if (m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
				{
					pAndroidUserItem->SetAndroidAction(ANDROID_WAITSTANDUP, true);
				}

				continue;
			}

			UserLeaveArray.Add(pAndroidUserItem);
		}
	}

	for (INT_PTR i = 0; i < UserLeaveArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = UserLeaveArray[i];

		if (pAndroidUserItem)
		{
			//删除用户
			WORD wRoundID = pAndroidUserItem->m_wRoundID;
			WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
			DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), false);
		}
	}

	UserLeaveArray.RemoveAll();

	return true;
}

//设置参数
bool CAndroidUserManager::AddAndroidParameter(tagAndroidParameter AndroidParameter)
{
	//数目判断
	if (AndroidParameter.dwAndroidCount > m_AndroidParameter.dwAndroidCount)
	{
		//拷贝数据
		CopyMemory(&m_AndroidParameter, &AndroidParameter, sizeof(tagAndroidParameter));

		//调整参数
		if (m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
		{
			m_AndroidParameter.dwSwitchMinInnings = 0;
			m_AndroidParameter.dwSwitchMaxInnings = 0;
		}

		//初始默认一半人数
		m_RoomAIParameter.dwAICountBase = m_AndroidParameter.dwAndroidCount;
		m_wAutoAndroidCount = (WORD)(m_RoomAIParameter.dwAICountBase / 2);

		//拷贝配置
		m_RoomAIParameter.wAIHangingMinRate = (WORD)m_AndroidParameter.dwAIHangingMinRate;
		m_RoomAIParameter.wAIHangingMaxRate = (WORD)m_AndroidParameter.dwAIHangingMaxRate;

		for (WORD i = 0; i < AI_CARRY_GRADE; i++)
		{
			m_RoomAIParameter.lAICarryScoreMinMultiple[i] = (LONG)m_AndroidParameter.dwAICarryScoreMinMultiple[i];
			m_RoomAIParameter.lAICarryScoreMaxMultiple[i] = (LONG)m_AndroidParameter.dwAICarryScoreMaxMultiple[i];
			m_RoomAIParameter.wAIScoreLevelRate[i] = (WORD)m_AndroidParameter.dwAIScoreLevelRate[i];
			m_RoomAIParameter.wAIScoreFloatMinRate[i] = (WORD)m_AndroidParameter.dwAIScoreFloatMinRate[i];
			m_RoomAIParameter.wAIScoreFloatMaxRate[i] = (WORD)m_AndroidParameter.dwAIScoreFloatMaxRate[i];
		}
	}
	//修改其他配置
	else if (AndroidParameter.dwAndroidCount == m_AndroidParameter.dwAndroidCount)
	{
		//拷贝数据
		CopyMemory(&m_AndroidParameter, &AndroidParameter, sizeof(tagAndroidParameter));

		//调整参数
		if (m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
		{
			m_AndroidParameter.dwSwitchMinInnings = 0;
			m_AndroidParameter.dwSwitchMaxInnings = 0;
		}

		//拷贝配置
		m_RoomAIParameter.wAIHangingMinRate = (WORD)m_AndroidParameter.dwAIHangingMinRate;
		m_RoomAIParameter.wAIHangingMaxRate = (WORD)m_AndroidParameter.dwAIHangingMaxRate;

		for (WORD i = 0; i < AI_CARRY_GRADE; i++)
		{
			m_RoomAIParameter.lAICarryScoreMinMultiple[i] = (LONG)m_AndroidParameter.dwAICarryScoreMinMultiple[i];
			m_RoomAIParameter.lAICarryScoreMaxMultiple[i] = (LONG)m_AndroidParameter.dwAICarryScoreMaxMultiple[i];
			m_RoomAIParameter.wAIScoreLevelRate[i] = (WORD)m_AndroidParameter.dwAIScoreLevelRate[i];
			m_RoomAIParameter.wAIScoreFloatMinRate[i] = (WORD)m_AndroidParameter.dwAIScoreFloatMinRate[i];
			m_RoomAIParameter.wAIScoreFloatMaxRate[i] = (WORD)m_AndroidParameter.dwAIScoreFloatMaxRate[i];
		}
	}

	return true;
}

//插入机器
bool CAndroidUserManager::InsertAndroidInfo(tagAndroidAccountsInfo AndroidAccountsInfo[], WORD wAndroidCount, tagGameChatInfo * pGameChatInfo)
{
	//记录库存	
	CAndroidUserItem * pAndroidUserItem = NULL;
	WORD wStockCount = m_wStockCount;
	bool bAndroidExist = false;

	//拷贝数值
	m_pGameChatInfo = pGameChatInfo;

	//添加新机器
	for (WORD wIndex = 0; wIndex<wAndroidCount; wIndex++)
	{
		//设置变量
		bAndroidExist = false;
		//从库存中查找
		for (WORD wItem = 0; wItem<wStockCount; wItem++)
		{
			if (m_AccountsInfoArray[wItem]->dwUserID == AndroidAccountsInfo[wIndex].dwUserID)
			{
				bAndroidExist = true;
				break;
			}
		}

		//存在判断
		if (bAndroidExist == true) continue;

		//从映射中查找
		if (m_AndroidUserItemMap.Lookup(AndroidAccountsInfo[wIndex].dwUserID, pAndroidUserItem) == TRUE)
		{
			continue;
		}

		//添加机器
		ActiveAndroidUserInfo(AndroidAccountsInfo[wIndex]);
	}

	//初始进入
	if (GetAndroidCount() == 0)
	{
		InitialAndroidIn();
	}

	//开启定时器
	DWORD dwAndroidInElapse = RANDVAL(1, TIME_ANDROID_INOUT);
	m_pITimerEngine->KillTimer(IDI_ANDROID_IN);
	m_pITimerEngine->SetTimer(IDI_ANDROID_IN, dwAndroidInElapse * 1000L, 1, 0);

	DWORD dwAndroidOutElapse = RANDVAL(1, TIME_ANDROID_INOUT);
	m_pITimerEngine->KillTimer(IDI_ANDROID_OUT);
	m_pITimerEngine->SetTimer(IDI_ANDROID_OUT, dwAndroidOutElapse * 1000L, 1, 0);

	m_pITimerEngine->KillTimer(IDI_ANDROID_DYNAMIC);
	m_pITimerEngine->SetTimer(IDI_ANDROID_DYNAMIC, TIME_ANDROID_DYNAMIC * 1000L, TIMES_INFINITY, 0);

	return true;
}

//绑定桌子
bool CAndroidUserManager::BindTableFrame(ITableFrame * pTableFrame, WORD wTableID)
{
	//数目校验
	//ASSERT(pTableFrame != NULL && wTableID >= 0 && wTableID < m_pGameServiceOption->wTableCount);
	if (!(pTableFrame != NULL && wTableID >= 0 && wTableID < m_pGameServiceOption->wTableCount))
	{
		return false;
	}

	//接口校验
	//ASSERT(m_ppITableFrame != NULL);
	if (!m_ppITableFrame)
	{
		return false;
	}

	//绑定桌子
	m_ppITableFrame[wTableID] = pTableFrame;

	return true;
}

//删除机器
bool CAndroidUserManager::DeleteAndroidUserItem(DWORD dwAndroidID, bool bStockRetrieve)
{
	//获取对象
	WORD wIndex = LOWORD(dwAndroidID);
	CAndroidUserItem * pAndroidUserItem = GetAndroidUserItem(wIndex);

	//对象效验
	////ASSERT((pAndroidUserItem!=NULL)&&(pAndroidUserItem->m_wRoundID==HIWORD(dwAndroidID)));
	if ((pAndroidUserItem == NULL) || (pAndroidUserItem->m_wRoundID != HIWORD(dwAndroidID))) return false;

	//关闭事件
	try
	{
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwAndroidID, 0, 0L);
	}
	catch (...)
	{
		//错误断言
		//ASSERT(FALSE);
	}

	//删除对象
	DWORD dwUserID = pAndroidUserItem->GetUserID();
	FreeAndroidUserItem(dwUserID, wIndex);

	//解锁AI
	if (m_pIMainServiceFrame != NULL)
	{
		m_pIMainServiceFrame->UnLockAndroidUser(dwUserID);
	}

	return true;
}

//查找机器
IAndroidUserItem * CAndroidUserManager::SearchAndroidUserItem(DWORD dwUserID, DWORD dwContextID)
{
	//查找机器
	CAndroidUserItem * pAndroidUserItem = NULL;
	m_AndroidUserItemMap.Lookup(dwUserID, pAndroidUserItem);

	//机器判断
	if (pAndroidUserItem != NULL)
	{
		WORD wRoundID = pAndroidUserItem->m_wRoundID;
		WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
		if (MAKELONG(wAndroidIndex, wRoundID) == dwContextID) return pAndroidUserItem;
	}

	return NULL;
}

//创建机器
IAndroidUserItem * CAndroidUserManager::CreateAndroidUserItem(tagAndroidAccountsInfo & AndroidAccountsInfo)
{
	//效验参数
	//ASSERT(AndroidAccountsInfo.dwUserID != 0L);
	if (AndroidAccountsInfo.dwUserID == 0L) return NULL;

	//创建对象
	CAndroidUserItem * pAndroidUserItem = ActiveAndroidUserItem(AndroidAccountsInfo);
	if (pAndroidUserItem == NULL) return NULL;

	//属性变量
	WORD wRoundID = pAndroidUserItem->m_wRoundID;
	WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;

	//连接模拟
	try
	{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(MAKELONG(wAndroidIndex, wRoundID), 0L) == false)
		{
			throw 0;
		}
	}
	catch (...)
	{
		//错误断言
		//ASSERT(FALSE);

		//释放用户
		FreeAndroidUserItem(AndroidAccountsInfo.dwUserID, wAndroidIndex);

		return NULL;
	}

	//变量定义
	CMD_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(LogonUserID));

	//版本信息
	LogonUserID.dwPlazaVersion = VERSION_PLAZA;
	LogonUserID.dwFrameVersion = VERSION_FRAME;
	LogonUserID.dwProcessVersion = m_pGameServiceAttrib->dwClientVersion;

	//用户信息
	LogonUserID.dwUserID = AndroidAccountsInfo.dwUserID;
	LogonUserID.wKindID = m_pGameServiceOption->wKindID;

	//构造数据
	TCP_Command Command;
	Command.wMainCmdID = MDM_GR_LOGON;
	Command.wSubCmdID = SUB_GR_LOGON_USERID;

	//消息处理
	try
	{
		//如果是大联盟AI
		if (AndroidAccountsInfo.dwGroupID != 0)
		{
			Command.wSubCmdID = SUB_GR_LOGON_AI;

			CMD_GR_LogonAIUser LogonAI;
			//版本信息
			LogonAI.dwPlazaVersion = VERSION_PLAZA;
			LogonAI.dwFrameVersion = VERSION_FRAME;
			LogonAI.dwProcessVersion = m_pGameServiceAttrib->dwClientVersion;

			//用户信息
			LogonAI.dwUserID = AndroidAccountsInfo.dwUserID;
			lstrcpyn(LogonAI.szPassword, TEXT(" "), CountArray(LogonUserID.szPassword));
			LogonAI.wKindID = m_pGameServiceOption->wKindID;
			LogonAI.dwGroupID =	AndroidAccountsInfo.dwGroupID;
			LogonAI.wTableID = AndroidAccountsInfo.wTableID;

			//发送数据
			if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(MAKELONG(wAndroidIndex, wRoundID), Command, &LogonAI, sizeof(LogonAI)) == false)
			{
				throw 0;
			}
		}
		else
		{
			//发送数据
			if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(MAKELONG(wAndroidIndex, wRoundID), Command, &LogonUserID, sizeof(LogonUserID)) == false)
			{
				throw 0;
			}
		}
	}
	catch (...)
	{
		//错误断言
		//ASSERT(FALSE);

		//删除机器
		DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), false);

		return NULL;
	}

	return pAndroidUserItem;
}

//枚举机器
IAndroidUserItem * CAndroidUserManager::EnumAndroidUserItem(WORD wEnumIndex)
{
	if (wEnumIndex >= m_AndroidUserItemArray.GetCount())
	{
		return NULL;
	}

	return m_AndroidUserItemArray[wEnumIndex];
}

//设置初始携带
void CAndroidUserManager::SetAndroidInitialCarryScore(DWORD dwUserID, DWORD dwContextID, SCORE lInitialCarryScore)
{
	//校验用户
	IAndroidUserItem * pIAndroidUserItem = SearchAndroidUserItem(dwUserID, dwContextID);

	if (pIAndroidUserItem != NULL && pIAndroidUserItem->GetUserID() == dwUserID)
	{
		pIAndroidUserItem->SetUserItemInitialCarryScore(dwUserID, lInitialCarryScore);
	}
}

//设置游戏变化
void CAndroidUserManager::SetAndroidGameVariationScore(DWORD dwUserID, DWORD dwContextID, SCORE lGameVariationScore)
{
	//校验用户
	IAndroidUserItem * pIAndroidUserItem = SearchAndroidUserItem(dwUserID, dwContextID);

	if (pIAndroidUserItem != NULL && pIAndroidUserItem->GetUserID() == dwUserID)
	{
		pIAndroidUserItem->SetUserItemGameVariationScore(dwUserID, lGameVariationScore);
	}
}

//设置接口
VOID CAndroidUserManager::SetMainServiceFrame(IMainServiceFrame *pIMainServiceFrame)
{
	m_pIMainServiceFrame = pIMainServiceFrame;

	return;
}

//获取用户
IServerUserItem * CAndroidUserManager::GetUserItem(WORD wTableID, WORD wChairID)
{
	//数目校验
	//ASSERT(m_ppITableFrame != NULL && wTableID >= 0 && wTableID < m_pGameServiceOption->wTableCount && wChairID >= 0 && wChairID < m_pGameServiceAttrib->wChairCount);
	if (!(m_ppITableFrame != NULL && wTableID >= 0 && wTableID < m_pGameServiceOption->wTableCount && wChairID >= 0
		&& wChairID < m_pGameServiceAttrib->wChairCount))
	{
		return NULL;
	}

	//桌子校验
	ITableFrame	*pITableFrame = m_ppITableFrame[wTableID];
	//ASSERT(pITableFrame != NULL);
	if (!pITableFrame)
	{
		return NULL;
	}

	return pITableFrame->GetTableUserItem(wChairID);
}

//脉冲事件
bool CAndroidUserManager::OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//进入处理
	if (dwTimerID == IDI_ANDROID_IN)
	{
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			return true;
		}

		//关闭定时器
		m_pITimerEngine->KillTimer(dwTimerID);

		//变量定义
		bool bAllowAndroidAttend = CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
		bool bAllowAndroidSimulate = CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);

		//登录处理
		if ((bAllowAndroidAttend == true) || (bAllowAndroidSimulate == true))
		{
			//变量定义
			WORD wAIInCount = 0;
			WORD wMaxAIRandEnterTenMin = RANDVAL(3, AI_RANDOMENTER_TENMIN);

			while (true)
			{
				//类型判断
				if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
				{
					break;
				}

				if (!EfficacyAndroidInOut(true))
				{
					break;
				}

				if (m_AccountsInfoArray.GetCount() == 0L)
				{
					break;
				}

				if (wAIInCount >= wMaxAIRandEnterTenMin)
				{
					break;
				}

				//创建机器
				if (CreateAndroidUserItem(*m_AccountsInfoArray[0]) != NULL)
				{
					//删除库存
					m_wStockCount--;
					tagAndroidAccountsInfo *pAccountsInfo = m_AccountsInfoArray[0];
					ZeroMemory(pAccountsInfo, sizeof(tagAndroidAccountsInfo));
					m_AccountsInfoArray.RemoveAt(0);
					m_AccountsInfoArrayBuffer.Add(pAccountsInfo);

					wAIInCount++;
				}
			}

			DWORD dwAndroidInElapse = RANDVAL(1, TIME_ANDROID_INOUT);
			m_pITimerEngine->SetTimer(IDI_ANDROID_IN, dwAndroidInElapse * 1000L, 1, 0);
		}

		return true;
	}

	//退出处理
	if (dwTimerID == IDI_ANDROID_OUT)
	{
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			return true;
		}

		//关闭定时器
		m_pITimerEngine->KillTimer(dwTimerID);

		//变量定义
		bool bAllowAndroidAttend = CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
		bool bAllowAndroidSimulate = CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);

		//登录处理
		if ((bAllowAndroidAttend == true) || (bAllowAndroidSimulate == true))
		{
			//变量定义
			DWORD dwUserID = 0L;
			POSITION Position = m_AndroidUserItemMap.GetStartPosition();

			//变量定义
			WORD wAIOutCount = 0;
			WORD wMaxAIRandLeaveTenMin = RANDVAL(3, AI_RANDOMLEAVE_TENMIN);

			//退出处理		
			while (Position != NULL)
			{
				//获取对象
				CAndroidUserItem * pAndroidUserItem = NULL;
				m_AndroidUserItemMap.GetNextAssoc(Position, dwUserID, pAndroidUserItem);

				//状态判断
				if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL
					|| m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
				{
					continue;
				}

				if (!EfficacyAndroidInOut(false))
				{
					break;
				}

				if (wAIOutCount >= wMaxAIRandLeaveTenMin)
				{
					break;
				}

				//判断离开等级
				WORD wLeaveGradeIndex = pAndroidUserItem->GetLeaveGradeIndex();

				//校验概率
				if (!DETECTIONPROBABILITY(m_RoomAIParameter.wAILeaveRate[wLeaveGradeIndex]))
				{
					continue;
				}

				//变量定义
				IServerUserItem * pIServerUserItem = pAndroidUserItem->m_pIServerUserItem;
				if (pIServerUserItem != NULL)
				{
					//服务状态
					BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

					//过滤状态
					if (cbUserStatus == US_PLAYING)
					{
						continue;
					}

					if (!(cbUserStatus == US_FREE || cbUserStatus == US_SIT))
					{
						pAndroidUserItem->m_bWaitLeave = true;
						wAIOutCount++;

						//设置AI动作
						if (m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
						{
							pAndroidUserItem->SetAndroidAction(ANDROID_WAITSTANDUP, true);
						}

						continue;
					}

					//删除用户
					WORD wRoundID = pAndroidUserItem->m_wRoundID;
					WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
					DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), false);

					wAIOutCount++;
				}
			}

			DWORD dwAndroidOutElapse = RANDVAL(1, TIME_ANDROID_INOUT);
			m_pITimerEngine->SetTimer(IDI_ANDROID_OUT, dwAndroidOutElapse * 1000L, 1, 0);
		}

		return true;
	}

	//用户脉冲
	if (dwTimerID == IDI_ANDROID_PULSE)
	{
		//变量定义
		DWORD dwUserID = 0L;
		CAndroidUserItem * pAndroidUserItem = NULL;
		POSITION Position = m_AndroidUserItemMap.GetStartPosition();

		//用户处理
		while (Position != NULL)
		{
			//获取对象
			m_AndroidUserItemMap.GetNextAssoc(Position, dwUserID, pAndroidUserItem);

			//时间处理
			try
			{
				if (pAndroidUserItem->m_pIServerUserItem != NULL) pAndroidUserItem->OnTimerPulse(dwTimerID, dwBindParameter);
			}
			catch (...)
			{
				//ASSERT(FALSE);
				DeleteAndroidUserItem(MAKELONG(pAndroidUserItem->m_wAndroidIndex, pAndroidUserItem->m_wRoundID), true);
			}
		}

		return true;
	}

	//动态时间
	if (dwTimerID == IDI_ANDROID_DYNAMIC)
	{
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			return true;
		}

		//动态补充挂机
		DynamicRectifyUserItemArrayHanging();

		//分析离开
		AnalyseAndroidItemArrayLeave();

		//效验机器人等待离开
		EfficacyAndroidWaitLeave();

		return true;
	}

	return false;
}

//挂机机器数目
WORD CAndroidUserManager::GetAndroidHangingCount()
{
	//变量定义
	WORD wAndroidHangingCount = 0;

	//
	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//状态判断
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		if (pAndroidUserItem->IsAiHanging())
		{
			wAndroidHangingCount++;
		}
	}

	return wAndroidHangingCount;
}

//加载判断
bool CAndroidUserManager::GetAndroidLoadInfo(DWORD & dwLoadCount)
{
	//数目判断
	if (GetAndroidCount() > m_AndroidParameter.dwAndroidCount)
	{
		return false;
	}

	if (m_AndroidParameter.dwAndroidCount < ((DWORD)(m_wStockCount + GetAndroidCount())))
	{
		return false;
	}

	//加载数目
	dwLoadCount = m_AndroidParameter.dwAndroidCount - (m_wStockCount + GetAndroidCount());

	//
	if (dwLoadCount > 0 && GetAndroidCount() == 0)
	{
		//获取开启服务端时间TimeSlotIndex
		BYTE cbTimeSlotIndex = GetServerTimeSlotIndex(m_RoomAIParameter);

		//非法校验
		if (!(cbTimeSlotIndex >= 0 && cbTimeSlotIndex < AI_TIME_SLOT))
		{
			dwLoadCount = 0;

			return false;
		}

		WORD wAICountRate = RANDVAL(m_RoomAIParameter.wMinAICountRate[cbTimeSlotIndex], m_RoomAIParameter.wMaxAICountRate[cbTimeSlotIndex]);

		SCORE lKeyLoadCount = ((SCORE)(dwLoadCount * wAICountRate)) / ((SCORE)100);
		dwLoadCount = (WORD)ceil(lKeyLoadCount);

		m_wAutoAndroidCount = (WORD)(dwLoadCount / 2);
	}

	return dwLoadCount > 0;
}

//用户状况
WORD CAndroidUserManager::GetAndroidUserInfo(tagAndroidUserInfo & AndroidUserInfo, DWORD dwServiceMode)
{
	//设置变量
	ZeroMemory(&AndroidUserInfo, sizeof(AndroidUserInfo));

	//变量定义
	POSITION Position = m_AndroidUserItemMap.GetStartPosition();

	//枚举对象
	while (Position != NULL)
	{
		//获取对象
		DWORD dwUserID = 0L;
		CAndroidUserItem * pAndroidUserItem = NULL;
		m_AndroidUserItemMap.GetNextAssoc(Position, dwUserID, pAndroidUserItem);

		//效验参数
		//ASSERT((dwUserID != 0L) && (pAndroidUserItem != NULL));
		if ((dwUserID == 0L) || (pAndroidUserItem == NULL))
		{
			break;
		}

		//绑定判断
		if (pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		//变量定义
		IServerUserItem * pIServerUserItem = pAndroidUserItem->m_pIServerUserItem;
		tagAndroidParameter * pAndroidParameter = &m_AndroidParameter;

		//离开判断
		if (pAndroidUserItem->m_bWaitLeave)
		{
			continue;
		}

		//模式判断
		if ((pAndroidParameter->dwServiceMode&dwServiceMode) == 0L)
		{
			continue;
		}

		//休息时间
		DWORD dwCurTime = (DWORD)time(NULL);
		if ((pAndroidUserItem->m_dwStandupTickCount + pAndroidUserItem->m_dwReposeTickCount)>dwCurTime) continue;

		//状态判断
		switch (pIServerUserItem->GetUserStatus())
		{
		case US_FREE:
		{
			//ASSERT(AndroidUserInfo.wFreeUserCount<CountArray(AndroidUserInfo.pIAndroidUserFree));
			AndroidUserInfo.pIAndroidUserFree[AndroidUserInfo.wFreeUserCount++] = pAndroidUserItem;
			break;
		}
		case US_SIT:
		case US_READY:
		{
			//ASSERT(AndroidUserInfo.wSitdownUserCount<CountArray(AndroidUserInfo.pIAndroidUserSitdown));
			AndroidUserInfo.pIAndroidUserSitdown[AndroidUserInfo.wSitdownUserCount++] = pAndroidUserItem;
			break;
		}
		case US_PLAYING:
		case US_OFFLINE:
		{
			//ASSERT(AndroidUserInfo.wPlayUserCount<CountArray(AndroidUserInfo.pIAndroidUserPlay));
			AndroidUserInfo.pIAndroidUserPlay[AndroidUserInfo.wPlayUserCount++] = pAndroidUserItem;
			break;
		}
		}
	};

	return AndroidUserInfo.wFreeUserCount + AndroidUserInfo.wPlayUserCount + AndroidUserInfo.wSitdownUserCount;
}

//获取相应等级数量
WORD CAndroidUserManager::GetAndroidGradeCount(WORD wAndroidGrade)
{
	//变量定义
	WORD wAndroidGradeCount = 0;

	//
	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//状态判断
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		if (pAndroidUserItem->JudgeAndroidGrade(wAndroidGrade))
		{
			wAndroidGradeCount++;
		}
	}

	return wAndroidGradeCount;
}

//发送数据
bool CAndroidUserManager::SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据
	for (INT_PTR i = 0; i<m_AndroidUserItemArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//状态判断
		if (pAndroidUserItem->m_pIServerUserItem == NULL) continue;
		if (pAndroidUserItem->m_AndroidAccountsInfo.dwUserID == 0L) continue;

		//消息处理
		try
		{
			if (pAndroidUserItem->OnSocketRead(wMainCmdID, wSubCmdID, pData, wDataSize) == false)
			{
				throw 0;
			}
		}
		catch (...)
		{
			//错误断言
			//ASSERT(FALSE);

			//断开用户
			WORD wRoundID = pAndroidUserItem->m_wRoundID;
			WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
			DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), true);
		}
	}

	return true;
}

//发送数据
bool CAndroidUserManager::SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//获取对象
	WORD wIndex = LOWORD(dwAndroidID);
	CAndroidUserItem * pAndroidUserItem = GetAndroidUserItem(wIndex);

	//对象效验
	////ASSERT((pAndroidUserItem!=NULL)&&(pAndroidUserItem->m_wRoundID==HIWORD(dwAndroidID)));
	if ((pAndroidUserItem == NULL) || (pAndroidUserItem->m_wRoundID != HIWORD(dwAndroidID))) return false;

	//消息处理
	try
	{
		if (pAndroidUserItem->OnSocketRead(wMainCmdID, wSubCmdID, pData, wDataSize) == false)
		{
			//ASSERT(FALSE);
			throw 0;
		}
	}
	catch (...)
	{
		//ASSERT(FALSE);
		DeleteAndroidUserItem(dwAndroidID, true);
	}

	return true;
}

//发送日志数据
void CAndroidUserManager::SendLogData(tagLogUserInfo LogUserInfo)
{
	m_pIMainServiceFrame->SendLogData(LogUserInfo);
}

//发送日志数据
void CAndroidUserManager::SendLogData(TCHAR * pszLogInfo)
{
	m_pIMainServiceFrame->SendLogData(pszLogInfo);
}
//发送数据
bool CAndroidUserManager::SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//构造数据
	TCP_Command Command;
	Command.wSubCmdID = wSubCmdID;
	Command.wMainCmdID = wMainCmdID;

	//消息处理
	try
	{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwAndroidID, Command, pData, wDataSize) == false)
		{
			throw 0;
		}
	}
	catch (...)
	{
		CString str;
		str.Format(TEXT("SendDataToServer: MainID=%d,SubID=%d"), Command.wMainCmdID, Command.wSubCmdID);
		CTraceService::TraceString(str, TraceLevel_Exception);

		//ASSERT(FALSE);
		DeleteAndroidUserItem(dwAndroidID, true);
	}

	return true;
}

//获取对象
CAndroidUserItem * CAndroidUserManager::GetAndroidUserItem(WORD wIndex)
{
	//效验索引
	//ASSERT(wIndex >= INDEX_ANDROID);
	if (wIndex<INDEX_ANDROID) return NULL;

	//效验索引
	WORD wKeyIndex = INVALID_WORD;
	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		if (pAndroidUserItem == NULL)
		{
			continue;
		}

		if (pAndroidUserItem->m_wAndroidIndex == wIndex)
		{
			wKeyIndex = i;
			break;
		}
	}

	////ASSERT(wKeyIndex != INVALID_WORD);
	if (wKeyIndex == INVALID_WORD)
	{
		return NULL;
	}

	//获取对象
	CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[wKeyIndex];

	return pAndroidUserItem;
}

//释放对象
VOID CAndroidUserManager::FreeAndroidUserItem(DWORD dwUserID, WORD wIndex)
{
	//效验参数
	//ASSERT(dwUserID != 0);
	if (dwUserID == 0) return;

	//变量定义
	CAndroidUserItem * pAndroidUserItem = NULL;
	m_AndroidUserItemMap.Lookup(dwUserID, pAndroidUserItem);

	//对象判断
	if (pAndroidUserItem == NULL)
	{
		//ASSERT(FALSE);
		return;
	}

	//复位对象
	pAndroidUserItem->RepositUserItem();

	//设置索引
	m_AndroidUserItemMap.RemoveKey(dwUserID);
	m_AndroidUserItemBuffer.Add(pAndroidUserItem);

	//删除索引
	WORD wRemoveIndex = INVALID_WORD;
	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		if (pAndroidUserItem == NULL)
		{
			continue;
		}

		if (pAndroidUserItem->m_wAndroidIndex == wIndex)
		{
			wRemoveIndex = i;
			break;
		}
	}

	//ASSERT(wRemoveIndex != INVALID_WORD);
	m_AndroidUserItemArray.RemoveAt(wRemoveIndex);

	//删除存储
	for (WORD wItem = 0; wItem<m_wStockCount; wItem++)
	{
		if (m_AccountsInfoArray[wItem]->dwUserID == dwUserID)
		{
			tagAndroidAccountsInfo *pAndroidAccountsInfo = m_AccountsInfoArray[wItem];
			ZeroMemory(pAndroidAccountsInfo, sizeof(tagAndroidAccountsInfo));

			m_AccountsInfoArray.RemoveAt(wItem);
			m_AccountsInfoArrayBuffer.Add(pAndroidAccountsInfo);
			m_wStockCount--;
			break;
		}
	}

	//离开信息
	tagAndroidLeaveInfo * pAndroidLeaveInfo = new tagAndroidLeaveInfo;
	ZeroMemory(pAndroidLeaveInfo, sizeof(tagAndroidLeaveInfo));

	pAndroidLeaveInfo->dwUserID = dwUserID;
	pAndroidLeaveInfo->dwLeaveTickCount = (DWORD)time(NULL);

	m_LeaveInfoArray.Add(pAndroidLeaveInfo);

	return;
}

//分析离开
VOID CAndroidUserManager::AnalyseAndroidItemArrayLeave()
{
	//列表定义
	list<SortTop> list_AIVariationTimes;
	list<SortTop> list_AICarryScoreAsc;
	list<SortTop> list_AICarryScoreDesc;
	list<SortTop> list_AIGameVariationScore;

	list_AIVariationTimes.clear();
	list_AICarryScoreAsc.clear();
	list_AICarryScoreDesc.clear();
	list_AIGameVariationScore.clear();

	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//状态判断
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		//插入列表
		list_AIVariationTimes.push_back(SortTop(pAndroidUserItem->GetUserID(), pAndroidUserItem->GetScoreVariationTimes()));
		list_AICarryScoreAsc.push_back(SortTop(pAndroidUserItem->GetUserID(), pAndroidUserItem->GetMeUserItem()->GetUserScore()));
		list_AICarryScoreDesc.push_back(SortTop(pAndroidUserItem->GetUserID(), pAndroidUserItem->GetMeUserItem()->GetUserScore()));
		list_AIGameVariationScore.push_back(SortTop(pAndroidUserItem->GetUserID(), pAndroidUserItem->m_AndroidAccountsInfo.lGameVariationScore));
	}

	//升序列表
	list_AIVariationTimes.sort();
	list_AICarryScoreAsc.sort();
	list_AICarryScoreDesc.sort();
	list_AIGameVariationScore.sort();

	//反转
	list_AIVariationTimes.reverse();
	list_AICarryScoreAsc.reverse();

	//当前时间
	DWORD dwCurTickCount = (DWORD)time(NULL);

	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//状态判断
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		//校验时间
		//在该房间停留超过24小时
		if (dwCurTickCount - pAndroidUserItem->GetLogonFinishTickCount() > TIME_DAY_TICKCOUNT)
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_PRIORITY);

			continue;
		}

		//持有金币/初始金币。比值最大的前3
		if (AnalyseLeaveGradeListValid(pAndroidUserItem->GetUserID(), list_AIVariationTimes, AI_TOP_VARIATIONTIMES))
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_SECONDARY);

			continue;
		}

		//游戏币最多的机器人前3
		if (AnalyseLeaveGradeListValid(pAndroidUserItem->GetUserID(), list_AICarryScoreAsc, AI_TOP_MAXCARRYSCORE))
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_SECONDARY);

			continue;
		}

		//游戏币最少的机器人前3
		if (AnalyseLeaveGradeListValid(pAndroidUserItem->GetUserID(), list_AICarryScoreDesc, AI_TOP_MINCARRYSCORE))
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_SECONDARY);

			continue;
		}

		//游戏输掉金币数量前5
		if (AnalyseLeaveGradeListValid(pAndroidUserItem->GetUserID(), list_AIGameVariationScore, AI_TOP_MAXLOSTSCORE))
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_SECONDARY);

			continue;
		}

		//默认无等级次优先
		pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_NONE);
	}

	return;
}

//分析离开等级列表
bool CAndroidUserManager::AnalyseLeaveGradeListValid(DWORD dwUserID, list<SortTop> tempList, int nTopCount)
{
	//循环索引
	int nCirculateIndex = 0;
	for (ITSORTTOP it = tempList.begin(); it != tempList.end(); it++, nCirculateIndex++)
	{
		if (nCirculateIndex >= nTopCount)
		{
			break;
		}

		if ((*it).dwUserID == dwUserID)
		{
			return true;
		}
	}

	return false;
}

//动态补充挂机
VOID CAndroidUserManager::DynamicRectifyUserItemArrayHanging()
{
	//效验类型
	if (!(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR))
	{
		return;
	}

	//挂机比例
	WORD wAIHangingRate = RANDVAL(m_RoomAIParameter.wAIHangingMinRate, m_RoomAIParameter.wAIHangingMaxRate);

	//机器数目
	WORD wAndroidCount = GetAndroidCount();
	WORD wCurAndroidHangingCount = GetAndroidHangingCount();
	SCORE lKeyAndroidHangingCount = ((SCORE)(wAndroidCount * wAIHangingRate)) / ((SCORE)100);
	WORD wKeyAndroidHangingCount = (WORD)ceil(lKeyAndroidHangingCount);

	//补充部分就是来自于（A * 15 % +B * 40 % +C * 80 % ）的其中随机抽取
	if (wCurAndroidHangingCount < wKeyAndroidHangingCount)
	{
		for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
		{
			//获取对象
			CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

			//状态判断
			if (pAndroidUserItem == NULL || pAndroidUserItem->IsAiHanging() || pAndroidUserItem->m_pIServerUserItem == NULL)
			{
				continue;
			}

			//变量定义
			bool bAndroidOccupy = (pAndroidUserItem->m_wAndroidAction&ANDROID_OCCUPYSEAT) != 0;
			bool bAndroidBeBanker = (pAndroidUserItem->m_wAndroidAction&ANDROID_BEBANKER) != 0;

			//过滤AI占位及AI庄家
			if (!(!bAndroidOccupy && !bAndroidBeBanker))
			{
				continue;
			}

			//携带等级索引
			BYTE cbCarryGradeIndex = pAndroidUserItem->GetCarryGradeIndex();
			if (!(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1))
			{
				continue;
			}

			//跳出判断
			if (wCurAndroidHangingCount >= wKeyAndroidHangingCount)
			{
				break;
			}

			//校验概率
			if (DETECTIONPROBABILITY(m_RoomAIParameter.wSetHangingRate[cbCarryGradeIndex]))
			{
				wCurAndroidHangingCount++;

				pAndroidUserItem->SetAiHanging(true);
			}
		}

		//不满足
		if (wCurAndroidHangingCount < wKeyAndroidHangingCount)
		{
			WORD wDValHangingCount = wKeyAndroidHangingCount - wCurAndroidHangingCount;
			WORD wIndex = 0;

			for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
			{
				//获取对象
				CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

				//状态判断
				if (pAndroidUserItem == NULL || pAndroidUserItem->IsAiHanging() || pAndroidUserItem->m_pIServerUserItem == NULL)
				{
					continue;
				}

				//变量定义
				bool bAndroidOccupy = (pAndroidUserItem->m_wAndroidAction&ANDROID_OCCUPYSEAT) != 0;
				bool bAndroidBeBanker = (pAndroidUserItem->m_wAndroidAction&ANDROID_BEBANKER) != 0;

				//过滤AI占位及AI庄家
				if (!(!bAndroidOccupy && !bAndroidBeBanker))
				{
					continue;
				}

				//携带等级索引
				BYTE cbCarryGradeIndex = pAndroidUserItem->GetCarryGradeIndex();
				if (!(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1))
				{
					continue;
				}

				//跳出判断
				if (wIndex >= wDValHangingCount)
				{
					break;
				}

				wIndex++;
				pAndroidUserItem->SetAiHanging(true);
			}
		}
	}
	//修改挂机概率
	else if (wCurAndroidHangingCount > wKeyAndroidHangingCount)
	{
		WORD wDValHangingCount = wCurAndroidHangingCount - wKeyAndroidHangingCount;
		WORD wIndex = 0;

		//设置非挂机状态
		for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
		{
			//获取对象
			CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

			//状态判断
			if (pAndroidUserItem == NULL || !pAndroidUserItem->IsAiHanging() || pAndroidUserItem->m_pIServerUserItem == NULL)
			{
				continue;
			}

			//携带等级索引
			BYTE cbCarryGradeIndex = pAndroidUserItem->GetCarryGradeIndex();
			if (!(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1))
			{
				continue;
			}

			//跳出判断
			if (wIndex >= wDValHangingCount)
			{
				break;
			}

			wIndex++;
			pAndroidUserItem->SetAiHanging(false);
		}
	}
}

//初始进入
VOID CAndroidUserManager::InitialAndroidIn()
{
	//校验自动数目
	if (m_wAutoAndroidCount <= 0)
	{
		return;
	}

	//变量定义
	bool bAllowAndroidAttend = CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
	bool bAllowAndroidSimulate = CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);

	//登录处理
	if ((bAllowAndroidAttend == true) || (bAllowAndroidSimulate == true))
	{
		//寻找机器
		for (WORD i = 0; i<m_wAutoAndroidCount; i++)
		{
			//模拟判断
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
				&& (m_AndroidParameter.dwServiceMode&ANDROID_SIMULATE) != 0
				&& (m_AndroidParameter.dwServiceMode&ANDROID_PASSIVITY) == 0
				&& (m_AndroidParameter.dwServiceMode&ANDROID_INITIATIVE) == 0)
			{
				continue;
			}

			//数组判空
			if (m_AccountsInfoArray.IsEmpty())
			{
				break;
			}

			//创建机器
			IAndroidUserItem * pAndroidUserItem = CreateAndroidUserItem(*m_AccountsInfoArray[0]);
			if (pAndroidUserItem != NULL)
			{
				//设置初始进入
				CAndroidUserItem * pKeyAndroidUserItem = (CAndroidUserItem *)pAndroidUserItem;
				pKeyAndroidUserItem->m_bInitialIn = true;

				//删除库存
				m_wStockCount--;
				tagAndroidAccountsInfo *pAccountsInfo = m_AccountsInfoArray[0];
				ZeroMemory(pAccountsInfo, sizeof(tagAndroidAccountsInfo));

				m_AccountsInfoArray.RemoveAt(0);
				m_AccountsInfoArrayBuffer.Add(pAccountsInfo);
			}
		}
	}
}

//效验机器人进出
bool CAndroidUserManager::EfficacyAndroidInOut(bool bAndroidIn)
{
	//获取开启服务端时间TimeSlotIndex
	BYTE cbTimeSlotIndex = GetServerTimeSlotIndex(m_RoomAIParameter);

	//非法校验
	if (!(cbTimeSlotIndex >= 0 && cbTimeSlotIndex < AI_TIME_SLOT))
	{
		CString strMessage;
		strMessage.Format(TEXT("EfficacyAndroidInOut: cbTimeSlotIndex=%d"), cbTimeSlotIndex);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}

	SCORE lKeyLoadCount = ((SCORE)(m_AndroidParameter.dwAndroidCount * m_RoomAIParameter.wMinAICountRate[cbTimeSlotIndex])) / ((SCORE)100);
	WORD wMinAICount = (WORD)ceil(lKeyLoadCount);

	lKeyLoadCount = ((SCORE)(m_AndroidParameter.dwAndroidCount * m_RoomAIParameter.wMaxAICountRate[cbTimeSlotIndex])) / ((SCORE)100);
	WORD wMaxAICount = (WORD)ceil(lKeyLoadCount);

	//范围效验
	if (bAndroidIn && GetAndroidCount() + 1 > wMaxAICount)
	{
		return false;
	}

	//范围效验
	if (!bAndroidIn && GetAndroidCount() - 1 < wMinAICount)
	{
		return false;
	}

	//当前TickCount
	DWORD dwCurTickCount = (DWORD)time(NULL);

	SCORE lAIInout = ((SCORE)(m_AndroidParameter.dwAndroidCount * m_RoomAIParameter.wMinAIInoutRate[cbTimeSlotIndex])) / ((SCORE)100);
	WORD wMinAIInout = (WORD)ceil(lAIInout);

	lAIInout = ((SCORE)(m_AndroidParameter.dwAndroidCount * m_RoomAIParameter.wMaxAIInoutRate[cbTimeSlotIndex])) / ((SCORE)100);
	WORD wMaxAIInout = (WORD)ceil(lAIInout);

	WORD wMaxAIInCount = RANDVAL(wMinAIInout, wMaxAIInout);
	WORD wMaxAIOutCount = RANDVAL(wMinAIInout, wMaxAIInout);

	wMaxAIInCount = min(wMaxAIInCount, AI_MAX_ENTER_ONEHOUR);
	wMaxAIOutCount = min(wMaxAIOutCount, AI_MAX_LEAVE_ONEHOUR);

	//一小时进入
	if (bAndroidIn)
	{
		//判断下线
		if (GetAndroidCount() + 1 > wMaxAICount)
		{
			WORD wOneHourAndroidIn = 0;

			for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
			{
				//获取对象
				CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

				//状态判断
				if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL || pAndroidUserItem->m_bInitialIn == true)
				{
					continue;
				}

				if (pAndroidUserItem->GetLogonFinishTickCount() + 3600 > dwCurTickCount)
				{
					wOneHourAndroidIn++;
				}
			}

			if (wOneHourAndroidIn + 1 > wMaxAIInCount)
			{
				return false;
			}
		}
	}

	//一小时离开
	if (!bAndroidIn)
	{
		WORD wOneHourAndroidOut = 0;

		for (INT_PTR i = 0; i < m_LeaveInfoArray.GetCount(); i++)
		{
			//获取对象
			tagAndroidLeaveInfo * pAndroidLeaveInfo = m_LeaveInfoArray[i];

			if (pAndroidLeaveInfo && pAndroidLeaveInfo->dwLeaveTickCount + 3600 > dwCurTickCount)
			{
				wOneHourAndroidOut++;
			}
		}

		if (wOneHourAndroidOut + 1 > wMaxAIOutCount)
		{
			return false;
		}
	}

	return true;
}

//效验机器人等待离开
VOID CAndroidUserManager::EfficacyAndroidWaitLeave()
{
	//变量定义
	CAndroidUserItemArray waitLeaveArray;
	waitLeaveArray.RemoveAll();

	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//状态判断
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL || pAndroidUserItem->m_bWaitLeave == false)
		{
			continue;
		}

		waitLeaveArray.Add(pAndroidUserItem);
	}

	for (INT_PTR i = 0; i < waitLeaveArray.GetCount(); i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem = waitLeaveArray[i];

		if (pAndroidUserItem)
		{
			//删除用户
			WORD wRoundID = pAndroidUserItem->m_wRoundID;
			WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
			DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), false);
		}
	}

	waitLeaveArray.RemoveAll();

	if (m_RoomAIParameter.dwAICountBase == 0)
	{
		//关闭定时器
		m_pITimerEngine->KillTimer(IDI_ANDROID_DYNAMIC);
	}
}

//唯一CurrentIndex
WORD CAndroidUserManager::GetUniqueCurrentIndex()
{
	//变量定义
	WORD wCurrentIndex = INVALID_WORD;

	for (WORD wIndex = m_AndroidUserItemMap.GetCount(); wIndex < MAX_ANDROID; wIndex++)
	{
		bool bIndexValid = true;

		//变量定义
		DWORD dwUserID = 0L;
		POSITION Position = m_AndroidUserItemMap.GetStartPosition();

		//退出处理		
		while (Position != NULL)
		{
			//获取对象
			CAndroidUserItem * pAndroidUserItem = NULL;
			m_AndroidUserItemMap.GetNextAssoc(Position, dwUserID, pAndroidUserItem);

			//状态判断
			if (pAndroidUserItem == NULL)
			{
				continue;
			}

			//校验索引
			if (pAndroidUserItem->m_wAndroidIndex == wIndex + INDEX_ANDROID)
			{
				bIndexValid = false;
				break;
			}
		}

		if (bIndexValid)
		{
			wCurrentIndex = wIndex;
			break;
		}
	}

	return wCurrentIndex;
}

//激活对象
tagAndroidAccountsInfo * CAndroidUserManager::ActiveAndroidUserInfo(tagAndroidAccountsInfo & AccountsInfo)
{
	//变量定义
	tagAndroidAccountsInfo * pAndroidAccountsInfo = NULL;
	INT_PTR nFreeItemCount = m_AccountsInfoArrayBuffer.GetCount();

	//获取对象
	if (nFreeItemCount>0)
	{
		//获取对象
		INT_PTR nItemPostion = nFreeItemCount - 1;
		pAndroidAccountsInfo = m_AccountsInfoArrayBuffer[nItemPostion];

		//删除数组
		m_AccountsInfoArrayBuffer.RemoveAt(nItemPostion);
	}

	//创建对象
	if (pAndroidAccountsInfo == NULL)
	{
		try
		{
			pAndroidAccountsInfo = new tagAndroidAccountsInfo;
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return NULL;
		}

		if (pAndroidAccountsInfo == NULL)
		{
			//ASSERT(FALSE);
			return NULL;
		}
	}

	//对象赋值
	CopyMemory(pAndroidAccountsInfo, &AccountsInfo, sizeof(tagAndroidAccountsInfo));
	//设置索引
	m_AccountsInfoArray.Add(pAndroidAccountsInfo);
	//增加库存
	m_wStockCount++;

	return pAndroidAccountsInfo;
}

//激活对象
CAndroidUserItem * CAndroidUserManager::ActiveAndroidUserItem(tagAndroidAccountsInfo & AccountsInfo)
{
	//变量定义
	CAndroidUserItem * pAndroidUserItem = NULL;
	INT_PTR nFreeItemCount = m_AndroidUserItemBuffer.GetCount();

	//获取对象
	if (nFreeItemCount>0)
	{
		//获取对象
		INT_PTR nItemPostion = nFreeItemCount - 1;
		pAndroidUserItem = m_AndroidUserItemBuffer[nItemPostion];

		//删除数组
		m_AndroidUserItemBuffer.RemoveAt(nItemPostion);
	}

	//创建对象
	if (pAndroidUserItem == NULL)
	{
		//数目判断
		if (m_AndroidUserItemArray.GetCount() >= MAX_ANDROID)
		{
			//ASSERT(FALSE);
			return NULL;
		}

		try
		{
			//变量定义
			IAndroidUserItemSink * pIAndroidUserItemSink = NULL;

			//创建陪玩
			if (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule) == true)
			{
				pIAndroidUserItemSink = (IAndroidUserItemSink *)m_pIGameServiceManager->CreateAndroidUserItemSink(IID_IAndroidUserItemSink, VER_IAndroidUserItemSink);
			}

			//创建对象
			try
			{
				pAndroidUserItem = new CAndroidUserItem;
			}
			catch (...)
			{
				//错误断言
				//ASSERT(FALSE);

				//删除对象
				SafeRelease(pIAndroidUserItemSink);
			}

			//错误判断
			if (pAndroidUserItem == NULL)
			{
				//错误断言
				//ASSERT(FALSE);

				//删除对象
				SafeRelease(pIAndroidUserItemSink);

				return NULL;
			}

			//设置用户
			if ((pIAndroidUserItemSink != NULL) && (pIAndroidUserItemSink->Initialization(pAndroidUserItem) == false))
			{
				//错误断言
				//ASSERT(FALSE);

				//删除对象
				SafeRelease(pAndroidUserItem);
				SafeRelease(pIAndroidUserItemSink);

				return NULL;
			}

			//设置唯一CurrentIndex
			WORD wCurrentIndex = GetUniqueCurrentIndex();
			//ASSERT(wCurrentIndex != INVALID_WORD);

			m_AndroidUserItemArray.Add(pAndroidUserItem);

			//配置变量
			pAndroidUserItem->m_dwMinSitInterval = m_dwMinSitInterval;
			pAndroidUserItem->m_dwMaxSitInterval = m_dwMaxSitInterval;

			//设置接口
			pAndroidUserItem->m_wAndroidIndex = wCurrentIndex + INDEX_ANDROID;
			pAndroidUserItem->m_pIServerUserManager = m_pIServerUserManager;
			pAndroidUserItem->m_pIAndroidUserItemSink = pIAndroidUserItemSink;
			pAndroidUserItem->m_pIAndroidUserManager = QUERY_ME_INTERFACE(IAndroidUserManager);
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return NULL;
		}
	}
	else
	{
		//设置唯一CurrentIndex
		WORD wCurrentIndex = GetUniqueCurrentIndex();
		//ASSERT(wCurrentIndex != INVALID_WORD);

		pAndroidUserItem->m_wAndroidIndex = wCurrentIndex + INDEX_ANDROID;

		m_AndroidUserItemArray.Add(pAndroidUserItem);
	}

	//设置变量
	pAndroidUserItem->m_AndroidAccountsInfo = AccountsInfo;

	//设置索引
	m_AndroidUserItemMap[AccountsInfo.dwUserID] = pAndroidUserItem;

	return pAndroidUserItem;
}

//强制起立
VOID CAndroidUserManager::ForceAndroidUserItemStandup(CAndroidUserItem * pAndroidUserItem)
{
	//校验参数
	//ASSERT(pAndroidUserItem != NULL);
	if (pAndroidUserItem == NULL) return;

	//属性变量
	WORD wRoundID = pAndroidUserItem->m_wRoundID;
	WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;

	//构造结构
	CMD_GR_UserStandUp UserStandUp;
	UserStandUp.cbForceLeave = true;
	UserStandUp.wTableID = pAndroidUserItem->m_pIServerUserItem->GetTableID();
	UserStandUp.wChairID = pAndroidUserItem->m_pIServerUserItem->GetChairID();

	//发送数据
	SendDataToServer(MAKELONG(wAndroidIndex, wRoundID), MDM_GR_USER, SUB_GR_USER_STANDUP, &UserStandUp, sizeof(UserStandUp));

	return;
}


//////////////////////////////////////////////////////////////////////////////////
