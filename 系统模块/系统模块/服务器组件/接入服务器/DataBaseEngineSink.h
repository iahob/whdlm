//藏宝库IT社区（28xin.com）
#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

//数据库钩子
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;
	//存储变量
protected:
	BYTE							m_cbBuffer[GP_SOCKET_PACKET];			//缓冲数据

	//组件变量
protected:
	CDataBaseAide					m_IMGroupDBAide;					//平台数据库	
	CDataBaseHelper					m_IMGroupDBModule;					//平台数据库

	//组件变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//系统功能
protected:
	//用户登录
	bool OnRequestLogonServer(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//创建群组
	bool OnRequestCreateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//更新群组
	bool OnRequestUpdateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除群组
	bool OnRequestDeleteGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//添加成员
	bool OnRequestInsertMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除成员
	bool OnRequestDeleteMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//申请请求
	bool OnRequestApplyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//申请响应
	bool OnRequestApplyRespond(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//申请删除
	bool OnRequestApplyDelete(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//约战结算
	bool OnRequestSettleBattle(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载配置
	bool OnRequestLoadGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载配置
	bool OnRequestLoadForbidWords(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询约战
	bool OnRequestQueryBattle(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//约战总数
	bool OnRequestQueryTotalBattle(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//添加玩法
	bool OnRequestAppendConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改玩法
	bool OnRequestModifyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除玩法
	bool OnRequestDeleteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改权限
	bool OnRequestModifyRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//赠送分数
	bool OnRequestPresentScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询赠送记录
	bool OnRequestQueryPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询扣费记录
	bool OnRequestQueryRevenue(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询日记录
	bool OnRequestQueryDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询玩家日详情
	bool OnRequestQueryDayDetail(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询大联盟日详情
	bool OnRequestGroupDayDetail(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询创建限制
	bool OnRequestQueryCreate(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询勋章模式
	bool OnRequestQueryMedalMode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//设置群组状态
	bool OnRequestSetGroupStatus(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//进出设置
	bool OnRequestSetInOut(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//设置跑马灯
	bool OnRequestSetRollMSg(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//权限管理
	bool OnRequestRightManagement(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//权限查询
	bool OnRequestRightQuery(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//勋章设置
	bool OnRequestSetMedal(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//空桌设置
	bool OnRequestSetFreeTableLimit(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//大赢家设置
	bool OnRequestSetBigWinner(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//点赞设置
	bool OnRequestSetFlag(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//清零勋章
	bool OnRequestClearMedal(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//收藏玩法
	bool OnRequestSetFavoriteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改名称
	bool OnRequestModifyGroupName(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询房间对局详情
	bool OnRequestRoomDetail(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询队长列表
	bool OnRequestCaptainList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询队长列表
	bool OnRequestAddTeamer(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询小队日统计
	bool OnRequestTeamDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询小队成员日统计
	bool OnRequestTeamerDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//隐私设置
	bool OnRequestSetPrivacy(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//点位设置
	bool OnRequestSetCaptainPoint(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//打开保险箱
	bool OnRequestOpenBox(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//提取积分
	bool OnRequestGetAwardScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询邀请码
	bool OnRequestInvitationCode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//绑定邀请码
	bool OnRequestBlindCode(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//检查新玩家
	bool OnRequestCheckNewUser(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询可用分
	bool OnRequestQueryScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//更新俱乐部在线状态
	bool OnRequestUpdateOnlineStatus(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载单个俱乐部信息
	bool OnRequestLoadSingleGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改信息
	bool OnRequestModifyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组功能
protected:
	//更新成员
	bool OnRequestUpdateMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组列表
	bool OnRequestLoadGroupList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组子项
	bool OnRequestLoadGroupItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组成员
	bool OnRequestLoadGroupMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//群组玩法
	bool OnRequestLoadGroupConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//辅助函数
protected:		
	//提取群组
	WORD ExtractGroupID(LPCTSTR pszGroupIDArray, CDWordArray & GroupIDArray);
	//输出异常
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif