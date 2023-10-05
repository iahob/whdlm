//藏宝库IT社区（28xin.com）
#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////

Cltladistance::Cltladistance()
{

}

Cltladistance::~Cltladistance()
{

}

inline double Cltladistance::GetRadian(double d)
{
	return d * PI / 180.0;
}

inline DWORD Cltladistance::GetRound(double number)
{
	return (DWORD)((number > 0.0) ? floor(number + 0.5) : ceil(number - 0.5));
}

inline double Cltladistance::GetDistance(double lat1, double lng1, double lat2, double lng2)
{
	double radLat1 = GetRadian(lat1);
	double radLat2 = GetRadian(lat2);
	double a = radLat1 - radLat2;
	double b = GetRadian(lng1) - GetRadian(lng2);

	double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));

	dst = dst * EARTH_RADIUS;
	dst= GetRound(dst * 10000000) / 10000;
	return dst;
}

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CServerUserItem::CServerUserItem(enUserItemKind enUserKind):m_enUserKind(enUserKind)
{
}

//析构函数
CServerUserItem::~CServerUserItem()
{
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CLocalUserItem::CLocalUserItem():CServerUserItem(enLocalKind)
{
	//设置变量
	m_dwSocketID=0L;
	m_dwLogonTime=0;
	m_wServerID = FRIEND_INVALID_SERVERID;
	m_wTableID = FRIEND_INVALID_TABLEID;

	//辅助变量
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));

	return;
}

//析构函数
CLocalUserItem::~CLocalUserItem()
{
}

//设置状态
VOID  CLocalUserItem::SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID)
{
	//设置变量
	m_UserInfo.cbGameStatus = cbGameStatus;
	m_wServerID = wServerID;
	m_wTableID = wTableID;
}

//对比帐号
bool CLocalUserItem::ContrastNickName(LPCTSTR pszNickName)
{
	//效验参数
	//ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//长度对比
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//字符对比
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//对比密码
bool CLocalUserItem::ContrastLogonPass(LPCTSTR pszPassword)
{
	//效验参数
	//ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//长度对比
	INT nTargetLen=lstrlen(pszPassword);
	INT nSourceLen=lstrlen(m_szLogonPass);

	//密码对比
	if (nTargetLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszPassword,nTargetLen,m_szLogonPass,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//重置数据
VOID CLocalUserItem::ResetUserItem()
{
	//设置变量
	m_dwSocketID=0L;

	//辅助变量
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));

	return;
}

//修改密码
VOID CLocalUserItem::ModifyLogonPassword(LPCTSTR pszPassword)
{
	//拷贝字符
	lstrcpyn(m_szLogonPass,pszPassword,CountArray(m_szLogonPass));

	return;
}

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CUserFriendGroup::CUserFriendGroup()
{
	//变量定义
	m_dwOwnerUserID=NULL;
	m_wFriendCount=0;	
	m_wGroupContent=0;
	m_pFriendInfo=NULL;
}

//析构函数
CUserFriendGroup::~CUserFriendGroup()
{
	//释放资源
	if(m_pFriendInfo!=NULL)	SafeDeleteArray(m_pFriendInfo);
}

//初始化分组
bool CUserFriendGroup::InitFriendGroup(WORD wGroupContent)
{
	//设置变量
	m_wGroupContent = wGroupContent;

	//分配容量
	m_pFriendInfo = AllocateContent(m_wGroupContent);

	return m_pFriendInfo!=NULL;
}

//添加好友
bool CUserFriendGroup::AppendFriendInfo(tagServerFriendInfo & FriendInfo)
{
	//指针校验
	//ASSERT(m_pFriendInfo!=NULL);
	if(m_pFriendInfo==NULL) return false;

	//拷贝数据
	CopyMemory((m_pFriendInfo+m_wFriendCount++),&FriendInfo,sizeof(tagServerFriendInfo));

	//扩展容量
	if(m_wFriendCount==m_wGroupContent) ExtendGroupContent(m_wFriendCount/10);

	return true;
}

//移除好友
bool CUserFriendGroup::RemoveFriendInfo(DWORD dwUserID)
{
	//指针校验
	//ASSERT(m_pFriendInfo!=NULL);
	if(m_pFriendInfo==NULL) return false;

	//变量定义
	tagServerFriendInfo * pFriendInfo=SearchFriendItem(dwUserID);

	//移动内存
	if(pFriendInfo!=NULL) 
	{
		--m_wFriendCount;
		if(pFriendInfo!=(tagServerFriendInfo *)(m_pFriendInfo+m_wGroupContent-1))
		{
			pFriendInfo = pFriendInfo++;
		}		
	}

	return true;
}

//查找好友
tagServerFriendInfo * CUserFriendGroup::SearchFriendItem(DWORD dwUserID)
{
	//变量定义
	tagServerFriendInfo * pFriendInfo=NULL;

	//查找好友
	for(WORD wIndex=0;wIndex<m_wFriendCount;++wIndex)
	{
		pFriendInfo = (tagServerFriendInfo *)(m_pFriendInfo+wIndex);
		if(pFriendInfo->dwUserID==dwUserID) return pFriendInfo;
	}

	return NULL;
}

//枚举好友
tagServerFriendInfo * CUserFriendGroup::EnumFriendItem(WORD wEnumIndex)
{
	//参数校验
	if(wEnumIndex>=m_wFriendCount) return NULL;

	return (tagServerFriendInfo *)(m_pFriendInfo+wEnumIndex);
}

//分配容量
tagServerFriendInfo * CUserFriendGroup::AllocateContent(WORD wGroupContent)
{
	//变量定义
	tagServerFriendInfo * pFriendInfo=NULL;

	//分配类型
	try
	{
		pFriendInfo = new tagServerFriendInfo[wGroupContent];
		//ASSERT(pFriendInfo!=NULL);
		if(pFriendInfo==NULL) throw(TEXT("系统内存资源不足,无法分配内存!"));

		//初始化内存
		ZeroMemory(pFriendInfo,wGroupContent*sizeof(tagServerFriendInfo));
	}
	catch(...)
	{
		SafeDeleteArray(pFriendInfo);
		pFriendInfo=NULL;
	}

	return pFriendInfo;
}

//扩展容量
bool CUserFriendGroup::ExtendGroupContent(WORD wExtendCount)
{
	//设置变量
	tagServerFriendInfo * pFriendInfo = AllocateContent(m_wGroupContent+wExtendCount);
	if(pFriendInfo==NULL) return false;

	try
	{
		//移动数据
		MoveMemory(pFriendInfo,m_pFriendInfo,m_wGroupContent*sizeof(tagServerFriendInfo));

		//释放内存
		SafeDeleteArray(m_pFriendInfo);

		//设置变量
		m_pFriendInfo=pFriendInfo;
		m_wGroupContent += wExtendCount;
	}
	catch(...)
	{
		return false;
	}

	return true;
}

//重置分组
VOID CUserFriendGroup::ResetFriendGroup()
{
	//设置变量	
	m_wFriendCount=0;
	m_dwOwnerUserID=0;
	if(m_pFriendInfo!=NULL)
	{
		ZeroMemory(m_pFriendInfo,m_wGroupContent*sizeof(tagServerFriendInfo));
	}
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserManager::CServerUserManager()
{
	//设置质数
	m_UserIDMap.InitHashTable(PRIME_PLATFORM_USER);

	return;
}

//析构函数
CServerUserManager::~CServerUserManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemStore[i];
		SafeDelete(pServerUserItem);
	}

	//释放用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemArray[i];
		SafeDelete(pServerUserItem);
	}

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//枚举用户
CServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//查找用户
CServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//删除用户
bool CServerUserManager::DeleteUserItem()
{
	//存储对象
	m_UserItemStore.Append(m_UserItemArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//删除用户
bool CServerUserManager::DeleteUserItem(DWORD dwUserID)
{
	//变量定义
	CServerUserItem * pTempUserItem=NULL;

	//寻找对象
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//获取用户
		pTempUserItem=m_UserItemArray[i];
		if (pTempUserItem->GetUserID()!=dwUserID) continue;

		//重置对象
		pTempUserItem->ResetUserItem();

		//删除对象
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//错误断言
	//ASSERT(FALSE);

	return false;
}

//插入用户
bool CServerUserManager::InsertLocalUserItem(tagInsertLocalUserInfo & InsertLocalUserInfo,tagFriendUserInfo & UserInfo,LPCTSTR pszPassword)
{
	//变量定义
	CLocalUserItem * pServerUserItem=NULL;

	//获取指针
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=(CLocalUserItem *)m_UserItemStore[nItemPostion];
		m_UserItemStore.RemoveAt(nItemPostion);
	}
	else
	{
		try
		{
			pServerUserItem=new CLocalUserItem;
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return false;
		}
	}

	//属性变量
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));

	//辅助变量
	lstrcpyn(pServerUserItem->m_szLogonPass,pszPassword,CountArray(pServerUserItem->m_szLogonPass));

	//插入用户
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	//设置变量
	pServerUserItem->SetLogonTime(InsertLocalUserInfo.dwLogonTime);
	pServerUserItem->SetSocketID(InsertLocalUserInfo.dwSocketID);

	return true;
}

DWORD CServerUserManager::GetNearUserItem(DWORD dwUserID,CNearUserManager & NearUserManager)
{
	DWORD dwCount =GetUserItemCount();
	DWORD dwCurrCount = 0;
	CLocalUserItem *pUserItem = (CLocalUserItem *)SearchUserItem(dwUserID);
	tagNearUserInfo NearUserInfo;
	for (WORD i = 0;i<dwCount;i++)
	{

		CLocalUserItem * pServerUserItem = (CLocalUserItem *)	EnumUserItem(i);
		tagFriendUserInfo *pUserInfo = &(pServerUserItem->m_UserInfo);
		if (pUserInfo->cbCoordinate ==1 && pUserItem->m_UserInfo.cbCoordinate == 1)
		{
			dwCurrCount++;
			ZeroMemory(&NearUserInfo,sizeof(NearUserInfo));
			NearUserInfo.dwUserID = pUserInfo->dwUserID;
			NearUserInfo.dwGameID = pUserInfo->dwGameID;
			NearUserInfo.dwFaceID = pUserInfo->dwFaceID;
			NearUserInfo.dwCustomID = pUserInfo->dwCustomID;
			NearUserInfo.cbGender = pUserInfo->cbGender;
			NearUserInfo.wMemberOrder = pUserInfo->wMemberOrder;
			NearUserInfo.wGrowLevel = pUserInfo->wGrowLevel;
			NearUserInfo.dLongitude = pUserInfo->dLongitude;
			NearUserInfo.dLatitude = pUserInfo->dLatitude;
			NearUserInfo.dwClientAddr = pUserInfo->dwClientAddr;
			lstrcpyn(NearUserInfo.szNickName,pUserInfo->szNickName,CountArray(NearUserInfo.szNickName));
			lstrcpyn(NearUserInfo.szUnderWrite,pUserInfo->szUnderWrite,CountArray(NearUserInfo.szUnderWrite));
			NearUserInfo.dwDistance =(DWORD)(m_ltladistance.GetDistance(NearUserInfo.dLatitude,NearUserInfo.dLongitude,pUserItem->m_UserInfo.dLatitude,pUserItem->m_UserInfo.dLongitude));
			NearUserManager.InsertNearUserItem(NearUserInfo);
		}
		
	}

	return dwCurrCount;
}

DWORD CServerUserManager::QueryNearUserItem(DWORD dwUserID,DWORD dwNearUserID,CNearUserManager & NearUserManager)
{
	DWORD dwCount = 0;
	CLocalUserItem *pUserItem = (CLocalUserItem *)SearchUserItem(dwUserID);
	CLocalUserItem *pNearUserItem = (CLocalUserItem *)SearchUserItem(dwNearUserID);
	if (pNearUserItem != NULL)
	{
		tagFriendUserInfo *pUserInfo = &(pNearUserItem->m_UserInfo);
		{
			tagNearUserInfo NearUserInfo;
			ZeroMemory(&NearUserInfo,sizeof(NearUserInfo));
			NearUserInfo.dwUserID = pUserInfo->dwUserID;
			NearUserInfo.dwGameID = pUserInfo->dwGameID;
			NearUserInfo.dwFaceID = pUserInfo->dwFaceID;
			NearUserInfo.dwCustomID = pUserInfo->dwCustomID;
			NearUserInfo.cbGender = pUserInfo->cbGender;
			NearUserInfo.wMemberOrder = pUserInfo->wMemberOrder;
			NearUserInfo.wGrowLevel = pUserInfo->wGrowLevel;
			NearUserInfo.dLongitude = pUserInfo->dLongitude;
			NearUserInfo.dLatitude = pUserInfo->dLatitude;
			NearUserInfo.dwClientAddr = pUserInfo->dwClientAddr;
			lstrcpyn(NearUserInfo.szNickName,pUserInfo->szNickName,CountArray(NearUserInfo.szNickName));
			lstrcpyn(NearUserInfo.szUnderWrite,pUserInfo->szUnderWrite,CountArray(NearUserInfo.szUnderWrite));
			lstrcpyn(NearUserInfo.szPlaceName, pUserInfo->szPlaceName, CountArray(NearUserInfo.szPlaceName));
			if (pUserItem->m_UserInfo.cbCoordinate == 1 && pUserInfo->cbCoordinate ==1)
			{
				NearUserInfo.dwDistance =(DWORD)(m_ltladistance.GetDistance(NearUserInfo.dLatitude,NearUserInfo.dLongitude,pUserItem->m_UserInfo.dLatitude,pUserItem->m_UserInfo.dLongitude));
			}
			NearUserManager.InsertNearUserItem(NearUserInfo);
			dwCount=NearUserManager.GetUserItemCount();
		}

	}

	return dwCount;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CFriendGroupManager::CFriendGroupManager()
{
	//设置质数
	m_UserIDMap.InitHashTable(PRIME_PLATFORM_USER);
}

//析构函数
CFriendGroupManager::~CFriendGroupManager()
{	
	//释放用户
	for (INT_PTR i=0;i<m_UserFriendGroupArray.GetCount();i++)
	{
		CUserFriendGroup * pUserFriendGroup = m_UserFriendGroupArray[i];
		SafeDelete(pUserFriendGroup);
	}

	//释放用户
	for (INT_PTR i=0;i<m_UserFriendGroupStore.GetCount();i++)
	{
		CUserFriendGroup * pUserFriendGroup = m_UserFriendGroupStore[i];
		SafeDelete(pUserFriendGroup);
	}

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserFriendGroupStore.RemoveAll();
	m_UserFriendGroupArray.RemoveAll();
}

//枚举用户
CUserFriendGroup * CFriendGroupManager::EnumGroupItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserFriendGroupArray.GetCount()) return NULL;
	return m_UserFriendGroupArray[wEnumIndex];
}

//查找用户
CUserFriendGroup * CFriendGroupManager::SearchGroupItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//删除用户
bool CFriendGroupManager::DeleteFriendGroup()
{
	//存储对象
	m_UserFriendGroupStore.Append(m_UserFriendGroupArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserFriendGroupArray.RemoveAll();

	return true;
}

//删除用户
bool CFriendGroupManager::DeleteFriendGroup(DWORD dwUserID)
{
	//变量定义
	CUserFriendGroup * pTempFriendGroup=NULL;

	//寻找对象
	for (INT_PTR i=0;i<m_UserFriendGroupArray.GetCount();i++)
	{
		//获取用户
		pTempFriendGroup=m_UserFriendGroupArray[i];
		if (pTempFriendGroup==NULL || pTempFriendGroup->GetOwnerUserID()!=dwUserID) continue;

		//重置对象
		pTempFriendGroup->ResetFriendGroup();

		//删除对象
		m_UserFriendGroupArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserFriendGroupStore.Add(pTempFriendGroup);

		return true;
	}

	//错误断言
	//ASSERT(FALSE);

	return false;
}

//插入好友
bool CFriendGroupManager::InsertFriendGroup(DWORD dwUserID,CUserFriendGroup * pUserFriendGroup)
{
	//设置标识
	pUserFriendGroup->SetOwnerUserID(dwUserID);

	//插入用户
	m_UserFriendGroupArray.Add(pUserFriendGroup);
	m_UserIDMap[dwUserID]=pUserFriendGroup;

	return true;
}

//获取分组
CUserFriendGroup * CFriendGroupManager::ActiveFriendGroup(WORD wFriendCount)
{
	//变量定义
	CUserFriendGroup * pUserFriendGroup=NULL;
	WORD wGroupContent = __min(wFriendCount+wFriendCount/10,MAX_FRIEND_COUNT);
	wGroupContent = __max(MIN_FRIEND_CONTENT,wGroupContent);

	//获取指针
	if (m_UserFriendGroupStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserFriendGroupStore.GetCount()-1;
		for(INT_PTR nIndex=nItemPostion;nIndex>=0;--nIndex)
		{
			pUserFriendGroup=(CUserFriendGroup *)m_UserFriendGroupStore[nIndex];
			if(pUserFriendGroup->GetGroupContent()>=wGroupContent)
			{
				pUserFriendGroup->ResetFriendGroup();
				m_UserFriendGroupStore.RemoveAt(nItemPostion);
				break;
			}
		}
	}

	if(pUserFriendGroup==NULL || pUserFriendGroup->GetGroupContent()<wGroupContent)
	{
		try
		{
			pUserFriendGroup=new CUserFriendGroup();
			if(pUserFriendGroup!=NULL) pUserFriendGroup->InitFriendGroup(wGroupContent);
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return false;
		}
	}

	return pUserFriendGroup;
}
//////////////////////////////////////////////////////////////////////////

CNearUserManager::CNearUserManager()
{

}

CNearUserManager::~CNearUserManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		tagNearUserInfo * pServerUserItem = m_UserItemArray[i];
		SafeDelete(pServerUserItem);
	}

	//删除数据
	m_UserItemArray.RemoveAll();
}

tagNearUserInfo * CNearUserManager::EnumUserItem(DWORD dwEnumIndex)
{
	if (dwEnumIndex>=(DWORD)m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[dwEnumIndex];
}

bool CNearUserManager::InsertNearUserItem(tagNearUserInfo & NearUserItem)
{
	//变量定义
	tagNearUserInfo * pNearUserItem=NULL;

	try
	{
		pNearUserItem=new tagNearUserInfo;
	}
	catch (...)
	{
		//ASSERT(FALSE);
		return false;
	}

	//属性变量
	CopyMemory(pNearUserItem,&NearUserItem,sizeof(NearUserItem));

	//插入用户
	DWORD dwCount =GetUserItemCount();
	bool bInsert = false;
	for (WORD i = 0;i<dwCount;i++)
	{
		tagNearUserInfo * pNextNearUserInfo =	EnumUserItem(i);
		if (pNextNearUserInfo->dwDistance>pNearUserItem->dwDistance)
		{
			m_UserItemArray.InsertAt(i,pNearUserItem);
			bInsert =true;
			break;
		}
	}
	if (!bInsert)
	{
		m_UserItemArray.Add(pNearUserItem);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//比赛

//构造函数
CMatchOptionItem::CMatchOptionItem()
{
	//设置变量
	ZeroMemory(&m_GameMatchOption, sizeof(m_GameMatchOption));

	return;
}

//构造函数
CMatchServerItem::CMatchServerItem()
{
	//设置变量
	ZeroMemory(&m_GameMatch, sizeof(m_GameMatch));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMatchServerManager::CMatchServerManager()
{
	//设置质数
	m_MatchOptionItemMap.InitHashTable(PRIME_SERVER);
	m_MatchServerItemMap.InitHashTable(PRIME_SERVER);

	return;
}

//析构函数
CMatchServerManager::~CMatchServerManager()
{
	//变量定义
	WORD wKey = 0;
	WORD wServerID = 0;

	//删除房间
	CMatchServerItem *pGameServerItem = NULL;
	POSITION Position = m_MatchServerItemMap.GetStartPosition();
	while (Position != NULL)
	{
		m_MatchServerItemMap.GetNextAssoc(Position, wKey, pGameServerItem);
		SafeDelete(pGameServerItem);
	}
	for (INT_PTR i = 0; i < m_MatchServerItemBuffer.GetCount(); i++)
	{
		pGameServerItem = m_MatchServerItemBuffer[i];
		SafeDelete(pGameServerItem);
	}
	m_MatchServerItemMap.RemoveAll();
	m_MatchServerItemBuffer.RemoveAll();

	//删除配置
	CMatchOptionItem *pMatchOptionItem = NULL;
	Position = m_MatchOptionItemMap.GetStartPosition();
	while (Position != NULL)
	{
		m_MatchOptionItemMap.GetNextAssoc(Position, wServerID, pMatchOptionItem);
		SafeDelete(pMatchOptionItem);
	}
	for (INT_PTR i = 0; i < m_MatchOptionItemBuffer.GetCount(); i++)
	{
		pMatchOptionItem = m_MatchOptionItemBuffer[i];
		SafeDelete(pMatchOptionItem);
	}
	m_MatchOptionItemMap.RemoveAll();
	m_MatchOptionItemBuffer.RemoveAll();

	return;
}

//重置列表
VOID CMatchServerManager::ResetMatchData()
{
	//废弃列表
	DisuseKernelItem();
	DisuseServerItem();

	//清理列表
	CleanKernelItem();
	CleanServerItem();

	return;
}

//清理内核
VOID CMatchServerManager::CleanKernelItem()
{
	//变量定义
	POSITION PositionPage = m_MatchOptionItemMap.GetStartPosition();

	//清理定制
	while (PositionPage != NULL)
	{
		//索引房间
		WORD wServerID = 0;
		CMatchOptionItem *pGamePageItem = NULL;
		m_MatchOptionItemMap.GetNextAssoc(PositionPage, wServerID, pGamePageItem);

		//删除判断
		if (pGamePageItem->m_bDisuse == true)
		{
			m_MatchOptionItemMap.RemoveKey(wServerID);
			m_MatchOptionItemBuffer.Add(pGamePageItem);
		}
	}

	return;
}

//清理房间
VOID CMatchServerManager::CleanServerItem()
{
	//变量定义
	POSITION PositionServer = m_MatchServerItemMap.GetStartPosition();

	//删除房间
	while (PositionServer != NULL)
	{
		//索引房间
		WORD wServerID = 0;
		CMatchServerItem *pGameServerItem = NULL;
		m_MatchServerItemMap.GetNextAssoc(PositionServer, wServerID, pGameServerItem);

		//删除判断
		if (pGameServerItem->m_bDisuse == true)
		{
			//删除数据
			m_MatchServerItemMap.RemoveKey(wServerID);
			m_MatchServerItemBuffer.Add(pGameServerItem);
		}
	}

	return;
}

//废弃内核
VOID CMatchServerManager::DisuseKernelItem()
{
	//变量定义	
	WORD wKey = 0;

	//废弃定制
	CMatchOptionItem *pGamePageItem = NULL;
	POSITION Position = m_MatchOptionItemMap.GetStartPosition();
	while (Position != NULL)
	{
		m_MatchOptionItemMap.GetNextAssoc(Position, wKey, pGamePageItem);
		pGamePageItem->m_bDisuse = true;
	}

	return;
}

//废弃房间
VOID CMatchServerManager::DisuseServerItem()
{
	//变量定义
	WORD wKey = 0;
	CMatchServerItem *pGameServerItem = NULL;

	//废弃房间
	POSITION Position = m_MatchServerItemMap.GetStartPosition();
	while (Position != NULL)
	{
		m_MatchServerItemMap.GetNextAssoc(Position, wKey, pGameServerItem);
		pGameServerItem->m_bDisuse = true;
	}

	return;
}

////统计人数
//DWORD CMatchServerManager::CollectOnlineInfo(bool bAndroid)
//{
//	//变量定义
//	DWORD dwOnLineCount = 0L;
//	POSITION PositionKind = m_KindItemMap.GetStartPosition();
//
//	//统计人数
//	while (PositionKind != NULL)
//	{
//		//索引类型
//		WORD wKindID = 0;
//		CGameKindItem *pGameKindItem = NULL;
//		m_KindItemMap.GetNextAssoc(PositionKind, wKindID, pGameKindItem);
//
//		//统计人数
//		if (pGameKindItem != NULL)
//		{
//			if (bAndroid == false)
//			{
//				dwOnLineCount += pGameKindItem->m_GameKind.dwOnLineCount;
//			}
//			else
//			{
//				dwOnLineCount += pGameKindItem->m_GameKind.dwAndroidCount;
//			}
//		}
//	}
//
//	return dwOnLineCount;
//}

////类型在线
//DWORD CMatchServerManager::CollectOnlineInfo(WORD wKindID, bool bAndroid)
//{
//	//变量定义
//	DWORD dwOnLineCount = 0L;
//	POSITION PositionServer = m_ServerItemMap.GetStartPosition();
//
//	//枚举房间
//	while (PositionServer != NULL)
//	{
//		//索引房间
//		WORD wServerID = 0;
//		CGameServerItem *pGameServerItem = NULL;
//		m_ServerItemMap.GetNextAssoc(PositionServer, wServerID, pGameServerItem);
//
//		//统计人数
//		if (pGameServerItem->m_GameServer.wKindID == wKindID)
//		{
//			if (bAndroid == false)
//			{
//				dwOnLineCount += pGameServerItem->m_GameServer.dwOnLineCount;
//			}
//			else
//			{
//				dwOnLineCount += pGameServerItem->m_GameServer.dwAndroidCount;
//			}
//		}
//	}
//
//	return dwOnLineCount;
//}
////统计设置人数
//DWORD CMatchServerManager::CollectSetPlayer(WORD wKindID)
//{
//	bool bAllGameServerItem = wKindID == INVALID_WORD;
//	//变量定义
//	DWORD dwOnLineCount = 0L;
//	POSITION PositionServer = m_ServerItemMap.GetStartPosition();
//
//	//枚举房间
//	while (PositionServer != NULL)
//	{
//		//索引房间
//		WORD wServerID = 0;
//		CGameServerItem *pGameServerItem = NULL;
//		m_ServerItemMap.GetNextAssoc(PositionServer, wServerID, pGameServerItem);
//
//		//统计人数
//		if (bAllGameServerItem)
//		{
//			dwOnLineCount += pGameServerItem->m_GameServer.dwSetPlayerCount;
//		}
//		else if (pGameServerItem->m_GameServer.wKindID == wKindID)
//		{
//			dwOnLineCount += pGameServerItem->m_GameServer.dwSetPlayerCount;
//		}
//	}
//
//	return dwOnLineCount;
//}

//插入配置
bool CMatchServerManager::InsertMatchOption(tagMatchSignUpCount *pMatchOption)
{
	//效验参数
	//ASSERT(pMatchOption != NULL);
	if (pMatchOption == NULL)
	{
		return false;
	}

	//获取子项
	CMatchOptionItem *pMatchOptionItem = NULL;
	if (m_MatchOptionItemMap.Lookup(pMatchOption->wServerID, pMatchOptionItem) == FALSE)
	{
		//创建对象
		try
		{
			INT_PTR nStroeCount = m_MatchOptionItemBuffer.GetCount();
			if (nStroeCount > 0)
			{
				pMatchOptionItem = m_MatchOptionItemBuffer[nStroeCount - 1];
				m_MatchOptionItemBuffer.RemoveAt(nStroeCount - 1);
			}
			else
			{
				pMatchOptionItem = new CMatchOptionItem;
				if (pMatchOptionItem == NULL)
				{
					return false;
				}
			}
		}
		catch (...)
		{
			return false;
		}

		//设置变量
		ZeroMemory(pMatchOptionItem, sizeof(CMatchOptionItem));
	}

	//设置数据
	pMatchOptionItem->m_bDisuse = false;
	CopyMemory(&pMatchOptionItem->m_GameMatchOption, pMatchOption, sizeof(tagMatchSignUpCount));

	//设置索引
	m_MatchOptionItemMap[pMatchOption->wServerID] = pMatchOptionItem;

	return true;
}

//插入房间
bool CMatchServerManager::InsertMatchServer(tagGameMatch *pGameServer)
{
	//效验参数
	//ASSERT(pGameServer != NULL);
	if (pGameServer == NULL)
	{
		return false;
	}

	//查找房间
	WORD wKindID = 0;
	DWORD dwOnLineCount = 0L;
	DWORD dwAndroidCount = 0L;
	DWORD dwMaxPlayer = 0L;
	CMatchServerItem *pGameServerItem = NULL;

	//获取子项
	if (m_MatchServerItemMap.Lookup(pGameServer->wServerID, pGameServerItem) == FALSE)
	{
		//创建对象
		try
		{
			INT_PTR nStroeCount = m_MatchServerItemBuffer.GetCount();
			if (nStroeCount > 0)
			{
				pGameServerItem = m_MatchServerItemBuffer[nStroeCount - 1];
				m_MatchServerItemBuffer.RemoveAt(nStroeCount - 1);
			}
			else
			{
				pGameServerItem = new CMatchServerItem;
				if (pGameServerItem == NULL)
				{
					return false;
				}
			}
		}
		catch (...)
		{
			return false;
		}

		//设置变量
		ZeroMemory(pGameServerItem, sizeof(CMatchServerItem));
	}

	//设置数据
	pGameServerItem->m_bDisuse = false;
	CopyMemory(&pGameServerItem->m_GameMatch, pGameServer, sizeof(tagGameMatch));

	

	//设置索引
	m_MatchServerItemMap[pGameServer->wServerID] = pGameServerItem;

	return true;
}

//删除房间
bool CMatchServerManager::DeleteMatchServer(WORD wServerID)
{
	////查找房间
	//CGameServerItem *pGameServerItem = NULL;
	//if (m_ServerItemMap.Lookup(wServerID, pGameServerItem) == FALSE)
	//{
	//	return false;
	//}

	////删除数据
	//m_ServerItemMap.RemoveKey(wServerID);
	//m_ServerItemBuffer.Add(pGameServerItem);

	////设置人数
	//CGameKindItem *pGameKindItem = NULL;
	//if (m_KindItemMap.Lookup(pGameServerItem->m_GameServer.wKindID, pGameKindItem) == TRUE)
	//{
	//	tagGameServer *pGameServer = &pGameServerItem->m_GameServer;
	//	pGameKindItem->m_GameKind.dwOnLineCount = __max(pGameKindItem->m_GameKind.dwOnLineCount - pGameServer->dwOnLineCount, 0);
	//	pGameKindItem->m_GameKind.dwFullCount = __max(pGameKindItem->m_GameKind.dwFullCount - pGameServer->dwFullCount, 0);
	//	pGameKindItem->m_GameKind.dwAndroidCount = __max(pGameKindItem->m_GameKind.dwAndroidCount - pGameServer->dwAndroidCount, 0);
	//}

	return false;
}

//删除配置
bool CMatchServerManager::DeleteMatchOption(WORD wPageID)
{
	//查找类型
	CMatchOptionItem *pMatchItem = NULL;
	if (m_MatchOptionItemMap.Lookup(wPageID, pMatchItem) == FALSE)
	{
		return false;
	}

	//删除数据
	m_MatchOptionItemMap.RemoveKey(wPageID);
	m_MatchOptionItemBuffer.Add(pMatchItem);

	//设置变量
	ZeroMemory(pMatchItem, sizeof(CMatchOptionItem));

	return true;
}

//枚举房间
CMatchServerItem *CMatchServerManager::EmunMatchServerItem(POSITION &Position)
{
	//变量定义
	WORD wKey = 0;
	CMatchServerItem *pGameServerItem = NULL;

	//获取对象
	if (Position == NULL)
	{
		Position = m_MatchServerItemMap.GetStartPosition();
	}
	if (Position != NULL)
	{
		m_MatchServerItemMap.GetNextAssoc(Position, wKey, pGameServerItem);
	}

	return pGameServerItem;
}

//枚举配置
CMatchOptionItem *CMatchServerManager::EmunMatchOptionItem(POSITION &Position)
{
	//变量定义
	WORD wKey = 0;
	CMatchOptionItem *pMatchOptionItem = NULL;

	//获取对象
	if (Position == NULL)
	{
		Position = m_MatchOptionItemMap.GetStartPosition();
	}
	if (Position != NULL)
	{
		m_MatchOptionItemMap.GetNextAssoc(Position, wKey, pMatchOptionItem);
	}

	return pMatchOptionItem;
}

//查找房间
CMatchServerItem *CMatchServerManager::SearchMatchServer(WORD wServerID)
{
	CMatchServerItem *pGameServerItem = NULL;
	m_MatchServerItemMap.Lookup(wServerID, pGameServerItem);
	return pGameServerItem;
}

//查找配置
CMatchOptionItem *CMatchServerManager::SearchMatchOption(WORD wServerID)
{
	CMatchOptionItem *pMatchOptionItem = NULL;
	m_MatchOptionItemMap.Lookup(wServerID, pMatchOptionItem);
	return pMatchOptionItem;
}


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CPersonalRoomManager::CPersonalRoomManager()
{
	//设置质数
	m_PersonalTableMap.InitHashTable(10000003);

	return;
}

//析构函数
CPersonalRoomManager::~CPersonalRoomManager()
{
	m_PersonalTableMap.RemoveAll();
	return;
}

//枚举用户
tagPersonalTableInfoEx * CPersonalRoomManager::EnumPersonalTableItem(DWORD wKindID, BYTE cbPlayMode, int nPos)
{
	CPersonalTableInfoEx * pCPersonalTableInfoEx = NULL;
	tagPersonalTableInfoEx * pPersonalTableInfoEx = NULL;
	m_PersonalTableMap.Lookup(wKindID, pCPersonalTableInfoEx);
	if (!pCPersonalTableInfoEx)
	{
		return NULL;
	}
	return pCPersonalTableInfoEx->GetAt(nPos);
}

//获取该类型房间数量
int CPersonalRoomManager::GetPersonalTableCount(DWORD wKindID)
{
	CPersonalTableInfoEx * pCPersonalTableInfoEx = NULL;
	tagPersonalTableInfoEx * pPersonalTableInfoEx = NULL;
	m_PersonalTableMap.Lookup(wKindID, pCPersonalTableInfoEx);
	if (!pCPersonalTableInfoEx)
	{
		return 0;
	}
	return pCPersonalTableInfoEx->GetCount();
}


//删除约战房间
bool CPersonalRoomManager::DeletePersonalTableItem(DWORD wKindID, DWORD wServerID, DWORD dwRoomID)
{
	CPersonalTableInfoEx * pCPersonalTableInfoEx = NULL;
	tagPersonalTableInfoEx * pPersonalTableInfoEx = NULL;
	m_PersonalTableMap.Lookup(wKindID, pCPersonalTableInfoEx);
	if (!pCPersonalTableInfoEx)
	{
		return false;
	}
	if (wKindID != 0 && dwRoomID != 0)
	{
		//遍历房间
		for (int i = 0; i < pCPersonalTableInfoEx->GetCount(); i++)
		{
			pPersonalTableInfoEx = pCPersonalTableInfoEx->GetAt(i);
			if (pPersonalTableInfoEx->dwPersonalRoomID == dwRoomID)
			{
				pPersonalTableInfoEx = pCPersonalTableInfoEx->GetAt(i);
				//删除房间
				pCPersonalTableInfoEx->RemoveAt(i);

				delete[] pPersonalTableInfoEx;
				break;
			}
		}
	}
	else
	{
		//遍历房间
		for (int i = pCPersonalTableInfoEx->GetCount() - 1; i >= 0; i--)
		{
			pPersonalTableInfoEx = pCPersonalTableInfoEx->GetAt(i);
			if (pPersonalTableInfoEx->dwServerID == wServerID)
			{
				pPersonalTableInfoEx = pCPersonalTableInfoEx->GetAt(i);
				//删除房间
				pCPersonalTableInfoEx->RemoveAt(i);

				delete[] pPersonalTableInfoEx;
			}
		}
	}
	
	return true;
}

//插入约战房间
bool CPersonalRoomManager::InsertPersonalTableItem(tagPersonalTableInfoEx * pPersonalTableInfoEx)
{
	//效验参数
	//ASSERT(pPersonalTableInfoEx != NULL);
	if (pPersonalTableInfoEx == NULL)
	{
		return false;
	}

	
	CPersonalTableInfoEx * pCPersonalTableInfoEx = NULL;
	if (m_PersonalTableMap.Lookup(pPersonalTableInfoEx->dwKindID, pCPersonalTableInfoEx) == FALSE)
	{
		//创建对象
		try
		{
			pCPersonalTableInfoEx = new CPersonalTableInfoEx;
			if (pCPersonalTableInfoEx == NULL)
			{
				return false;
			}

		}
		catch (...)
		{
			return false;
		}

		//设置变量
		ZeroMemory(pCPersonalTableInfoEx, sizeof(CPersonalTableInfoEx));

	
	}

	tagPersonalTableInfoEx * pTempPersonalTableInfoEx = new tagPersonalTableInfoEx();
	if (pTempPersonalTableInfoEx == NULL)
	{
		return false;
	}
	memcpy(pTempPersonalTableInfoEx, pPersonalTableInfoEx, sizeof(tagPersonalTableInfoEx));

	//桌子列表
	if (pCPersonalTableInfoEx)
	{
		pCPersonalTableInfoEx->Add(pTempPersonalTableInfoEx);
	}

	//设置索引
	m_PersonalTableMap[pPersonalTableInfoEx->dwKindID] = pCPersonalTableInfoEx;

	return true;
}