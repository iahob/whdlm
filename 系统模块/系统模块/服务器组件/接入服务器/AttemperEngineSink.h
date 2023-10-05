#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "ServerUserManager.h"
#include "ServerListManager.h"
#include "GroupInfoManager.h"
//#include "ClubServerDataInfo.h"
#include "WordFilter.h"
//////////////////////////////////////////////////////////////////////////

//绑定参数
struct tagBindParameter
{	
	//网络信息	
	DWORD							dwSocketID;							//网络标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwActiveTime;						//激活时间

	//服务信息
	WORD							wGroupID;							//群组标识
	WORD							wLogicID;							//接入标识
	DWORD							dwUserID;							//用户标识
};

//////////////////////////////////////////////////////////////////////////

//类型定义
typedef CWHArray<tagIMGroupOption>		CGroupOptionArray;			//群组配置
//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;
	//存储变量
protected:
	BYTE							m_cbBuffer[GP_SOCKET_PACKET];			//缓冲数据

	//状态变量
protected:
	bool							m_bNeedBattle;						//验证约战
	time_t							m_tMedalClearStart;
	//配置变量
protected:	
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//连接信息
	tagServiceItem					m_BattleItem;						//约战子项	
	bool							m_bNeedConnectBattle;				//链接约战
	//管理变量
protected:
	CServerUserManager				m_ServerUserManager;				//用户管理	
	CServerListManager				m_ServerListManager;				//列表管理
	CGroupInfoManager				m_GroupInfoManager;					//群组管理	
	CWordFilter						m_WordFilter;						//字词过滤
	CGroupOptionArray				m_GroupOptionArray;					//配置管理
	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_TCPSocketCorrespond;				//协调服务
	ITCPSocketService *				m_pITCPSocketBattle;				//约战服务	
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPSocketService *				m_pLogServerTCPSocketService;		//日志服务引擎
	ITCPSocketService *				m_pAITCPSocketService;				//AI服务引擎

	TCHAR							m_PrsAdd[32];
	WORD							m_wPrsPort;
	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release(){	return;	}
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//自定事件
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//协调连接
protected:
	//注册服务
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//列表事件
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);		
	//转发事件
	bool OnTCPSocketClubInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//网站事件
	bool OnTCPSocketWebInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//约战连接
protected:
	//注册服务
	bool OnTCPSocketBattleMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//列表事件
	bool OnTCPSocketBattleMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//约战服务
	bool OnTCPSocketBattleMainBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//AI服务
	bool OnTCPSocketAIMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:	
	//逻辑服务
	bool OnTCPNetworkMainLogicService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//群组服务
	bool OnTCPNetworkMainGroupService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//约战服务
	bool OnTCPNetworkMainBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//数据事件
protected:
	//登录成功
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//创建群组
	bool OnDBCreateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//更新群组
	bool OnDBUpdateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除群组
	bool OnDBDeleteGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//添加成员
	bool OnDBInsertMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除成员
	bool OnDBDeleteMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//申请消息
	bool OnDBApplyMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组配置
	bool OnDBGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//禁止词汇
	bool OnDBForbidWords(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//财富更新
	bool OnDBWealthUpdate(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除结果
	bool OnDBApplyDelResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作成功
	bool OnDBOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作失败
	bool OnDBOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载配置
	bool OnDBLoadGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//其他事件
protected:
	//群组列表
	bool OnDBGroupList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//列表完成
	bool OnDBGroupListFinish(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//公共配置
	bool OnDBGroupCommonInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组子项
	bool OnDBGroupItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//子项完成
	bool OnDBGroupItemFinish(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组成员
	bool OnDBGroupMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//更新成员
	bool OnDBUpdateMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组玩法
	bool OnDBGroupConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//约战结果
	bool OnDBBattleResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改权限
	bool OnDBModifyRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//赠送成功
	bool OnDBPresentSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//赠送记录
	bool OnDBPresentList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//扣费记录
	bool OnDBRevenueList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//创建玩法
	bool OnDBAppendConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改玩法
	bool OnDBModifyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除玩法
	bool OnDBDeleteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//日记录
	bool OnDBDayRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//日详情
	bool OnDBUserDayDetailList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//日详情
	bool OnDBGroupDayDetailList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//创建查询
	bool OnDBQueryCreateResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//勋章模式
	bool OnDBMedalMode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组状态
	bool OnDBGroupStatus(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//进出设置结果
	bool OnDBInOutMode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询管理员权限
	bool OnDBQueryManagerRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//空桌设置
	bool OnDBFreeTableLImit(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//大赢家设置
	bool OnDBBigWinnerShow(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//设置管理员权限
	bool OnDBSetManagerRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//点赞设置
	bool OnDBSetFlag(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//勋章清零
	bool OnDBClearMedal(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//收藏玩法
	bool OnDBFavoriteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//跑马灯
	bool OnDBRollMsg(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//大联盟名称
	bool OnDBGroupName(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//房间详情
	bool OnDBRoomDetailInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//队长列表
	bool OnDBCaptainList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//调配队员结果
	bool OnDBAddTeamer(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//小队日统计
	bool OnDBTeamDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//小队成员日统计
	bool OnDBTeamerDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//大联盟隐私
	bool OnDBGroupPrivacy(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//队长点位
	bool OnDBCaptainPoint(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//保险箱信息
	bool OnDBBoxInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//奖励分提取结果
	bool OnDBGetAwardScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//邀请码信息
	bool OnDBInvitationCode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//绑定成功
	bool OnDBBlindCode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询分数结果
	bool OnDBQueryScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改信息
	bool OnDBModifyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//数据发送
protected:
	//约战发送
	bool SendDataToBattle(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//群组发送
	bool SendDataToGroupMember(DWORD dwGroupID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, bool bVerifyGroupID = true);
	//群组管理员发送
	bool SendDataToGroupManager(DWORD dwGroupID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//群组队员发送
	bool SendDataToGroupTeamer(DWORD dwGroupID,DWORD dwCaptainID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送成员
	bool SendGroupMemberToUser(DWORD dwGroupID, DWORD dwSocketID);
	//发送玩法
	bool SendGroupConfigToUser(DWORD dwGroupID, DWORD dwSocketID);
	//更新附加信息
	bool UpdateGroupExtraInfo(DWORD dwGroupID, DWORD dwMemberID);
	//发送日志
	void SendLogData(const TCHAR * pszFormat, ...);
	//
	//辅助函数
protected:	
	//生成令牌
	DWORD BuildTokenID();	
	//绑定参数
	tagBindParameter * GetBindParameter(WORD wBindIndex);
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	//用户登录
	bool OnEventUserLogon(CLocalUserItem * pServerUserItem);
	//大联盟成员校验
	bool CheckClubUser(DWORD dwGroupID, DWORD dwUserID);
	//字符判断
	int CheckClubName(LPCWSTR pszGroupName);
};

//////////////////////////////////////////////////////////////////////////

#endif

