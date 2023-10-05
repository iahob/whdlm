#ifndef PLATFORM_FRAME_HEAD_FILE
#define PLATFORM_FRAME_HEAD_FILE

#pragma once

//系统文件
#include "Stdafx.h"
#include "Resource.h"
#include "ClientDebug.h"
#include "AIConfigDebug.h"

//任务文件
#include "MissionList.h"
#include "MissionLogon.h"
#include "MissionAndroid.h"
#include "ServerListData.h"
//////////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IPlatformFrame INTERFACE_VERSION(1,1)
static const GUID IID_IPlatformFrame = { 0x99c5d22b, 0xc1ee, 0x4af2, 0xb9, 0x7, 0xf2, 0x81, 0x53, 0xc6, 0x62, 0xd8 };
#else
#define VER_IPlatformFrame INTERFACE_VERSION(1,1)
static const GUID IID_IPlatformFrame = { 0xcb259add, 0x7322, 0x4380, 0x82, 0xbc, 0xef, 0x97, 0xd1, 0x4, 0xf4, 0x7a };
#endif

class CDlgServerManager;
//大厅接口
interface IPlatformFrame : public IUnknownEx
{
};

//服务状态
enum enServiceStatus
{
	ServiceStatus_Unknow,			//未知状态
	ServiceStatus_Entering,			//进入状态
	ServiceStatus_Validate,			//验证状态
	ServiceStatus_RecvInfo,			//读取状态
	ServiceStatus_ServiceIng,		//服务状态
	ServiceStatus_NetworkDown,		//中断状态
};
//平台框架
class CPlatformFrame :public CSkinDialog, public IPlatformFrame, public ITCPSocketSink, public IServerListDataSink, IAndroidOperateCenter, public IUserManagerSink, public IChatControlSink, public IClientDebugCallback, public IAIConfigDebugCallback
{
	//变量定义
protected:
	bool							m_bLogonSuccess;					//登录成功
	int								m_nSelectItem;						//选择索引

	CClientUserItemArray			m_MenuUserItemArray;				//用户数组

	//资源变量
protected:
	CBrush							m_brBackGround;						//背景画刷

	//游戏列表
protected:
	CServerListData					m_ServerListData;					//游戏数据

	//控件变量
protected:
	CSkinComboBox					m_cbGameList;						//游戏列表
	CSkinComboBox					m_cbServerList;						//房间列表
	CSkinComboBox					m_cbEnterHour;						//进入时间
	CSkinComboBox					m_cbEnterMinute;					//进入时间
	CSkinComboBox					m_cbEnterSecond;					//进入时间
	CSkinComboBox					m_cbLeaveHour;						//离开时间
	CSkinComboBox					m_cbLeaveMinute;					//离开时间
	CSkinComboBox					m_cbLeaveSecond;					//离开时间

	//组件变量
protected:
	CTCPSocketHelper				m_TCPSocketModule;					//网络连接	
	CPlazaUserManagerHelper			m_PlazaUserManagerModule;			//用户管理

	//控件变量
protected:
	CSkinEdit						m_edtAndroidCount;					//机器数目
	CSkinEdit						m_edtTableScoreMin;					//携带分数
	CSkinEdit						m_edtTableScoreMax;					//携带分数
	CSkinEdit						m_edtEnterIntervalMin;				//进入间隔
	CSkinEdit						m_edtEnterIntervalMax;				//进入间隔
	CSkinEdit						m_edtLeaveIntervalMin;				//离开间隔
	CSkinEdit						m_edtLeaveIntervalMax;				//离开间隔
	CSkinEdit						m_edtSwitchTableMin;				//换桌局数
	CSkinEdit						m_edtSwitchTableMax;				//换桌局数
	CSkinEdit						m_edtCountMember0;					//普通会员
	CSkinEdit						m_edtCountMember1;					//一级会员
	CSkinEdit						m_edtCountMember2;					//二级会员
	CSkinEdit						m_edtCountMember3;					//三级会员
	CSkinEdit						m_edtCountMember4;					//四级会员
	CSkinEdit						m_edtCountMember5;					//五级会员

	CSkinEdit						m_edtInputChat;						//聊天输入
	//按钮控件
protected:
	CSkinButton						m_btEnterRoom;						//进入房间
	CSkinButton						m_btExitRoom;						//退出房间
	CSkinButton						m_btRoomManager;					//房间管理
	CSkinButton						m_btGameControl;					//游戏控制
	CSkinButton						m_btSystemMessage;					//系统消息
	CSkinButton						m_btSendChat;						//发送消息
	CSkinButton						m_btAIAdvanced;						//AI高级属性按钮
	CSkinButton						m_btQuery;							//查询按钮
	CSkinButton						m_btAddItem;						//添加子项
	CSkinButton						m_btModifyItem;						//修改子项
	CSkinButton						m_btDeleteItem;						//删除子项

	//控件变量
protected:
	CSkinListCtrl					m_ParemeterList;					//参数列表
	CSystemTrayIcon					m_SystemTrayIcon;					//任务图标

	//任务组件
protected:
	CMissionList					m_MissionList;						//列表任务
	CMissionLogon					m_MissionLogon;						//登录任务
	CMissionAndroid					m_MissionAndroid;					//机器任务
	CMissionManager					m_MissionManager;					//任务管理
	
	//用户属性
protected:
	DWORD							m_dwUserRight;						//用户权限
	DWORD							m_dwMasterRight;					//管理权限
	IClientUserItem *				m_pIMySelfUserItem;					//自己指针
	IClientUserItem *				m_pISelectedUserItem;				//选择指针
	//房间属性
protected:	
	WORD							m_wServerType;						//房间类型
	DWORD							m_dwServerRule;						//房间规则
	tagGameKind						m_GameKind;							//类型信息
	tagGameServer					m_GameServer;						//房间信息
	enServiceStatus					m_ServiceStatus;					//房间状态	

	// 调试组件
public:
	HINSTANCE						m_hDebugInst;
	IClientDebug *					m_pIClientDebug;					//控制接口
	HINSTANCE						m_hAIConfigDebugInst;				//AI属性实例
	IAIConfigDebug *				m_pIAIConfigDebug;					//AI属性接口
	CRichEditMessage				m_ChatMessage;						//聊天信息
	CWndChatControl					m_ChatControl;						//聊天控制
	CUserListControl				m_UserListControl;					//用户列表
	
	CDlgServerManager *				m_pDlgServerManager;				//管理对象
	//聊天变量
private:
	DWORD							m_dwChatTime;						//聊天时间
	CString							m_strChatString;					//发送信息
	//静态变量
protected:
	static CPlatformFrame *			m_pPlatformFrame;					//框架指针

	//函数定义
public:
	//构造函数
	CPlatformFrame();
	//析构函数
	virtual ~CPlatformFrame();

	//清理列表数据
	VOID	CleanListData();
	//重载函数
public:
	//初始控件
	//virtual VOID InitControlUI();
	//消息提醒
	//virtual VOID Notify(TNotifyUI &  msg);
	//皮肤名称
	//virtual LPCTSTR GetSkinFile() { return TEXT("ServerCtrl.xml"); }
	//重载函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//消息过虑
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//状态通知
public:
	//完成通知
	virtual VOID OnGameItemFinish();
	//完成通知
	virtual VOID OnGameKindFinish(WORD wKindID);
	//更新通知
	virtual VOID OnGameItemUpdateFinish();
	
	//用户菜单
protected:
	//显示菜单
	virtual VOID TrackUserItemMenu(IClientUserItem * pIClientUserItem);
	
	//右键列表
	VOID OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//网络接口
protected:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wSocketID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//更新通知
public:
	//插入通知
	virtual VOID OnGameItemInsert(CGameListItem * pGameListItem);
	//更新通知
	virtual VOID OnGameItemUpdate(CGameListItem * pGameListItem);
	//删除通知
	virtual VOID OnGameItemDelete(CGameListItem * pGameListItem);

	//机器接口
public:
	//控件使能
	virtual VOID OnEventEnableControls(BOOL bEnabled);
	//机器参数
	virtual VOID OnEventAndroidParenter(WORD wSubCommdID,WORD wParameterCount,tagAndroidParameter * pAndroidParameter);

	//辅助函数
private:
	//初始化控件
	VOID InitControls();
	//添加修改
	VOID AddModifyParameter(bool bModify);
	//选中子项
	VOID SelectItem(int nIndex);
	//设置子项
	VOID SetItemToParameterList(int nItemIndex,tagAndroidParameter * pAndroidParameter);
	//调整控件
	VOID RectifyControl(INT nWidth, INT nHeight);
	//设置状态
	VOID SetServiceStatus(enServiceStatus ServiceStatus);
	//消息函数
protected:
	//关闭消息
	VOID OnClose();	
	//销毁窗口
	VOID OnDestroy();
	//进程消息
	BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);
	//位置消息
	VOID OnGetMinMaxInfo(MINMAXINFO * lpMMI);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDblClk(UINT nFlags, CPoint Point);
	//改变消息
	VOID OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//选择改变
	VOID OnCbnComboGameSelchange();
	//单击子项
	VOID OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	//删除子项
	VOID OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	//键盘按下
	VOID OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult);

	//控件接口
public:
	//消息接口
	IStringMessage * GetStringMessage() { return QUERY_OBJECT_INTERFACE(m_ChatMessage, IStringMessage); }
	//自定消息
protected:
	//事件消息
	LRESULT OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam);

	//事件函数
protected:
	//添加子项
	VOID OnBnClickedBtAdd();
	//编辑子项
	VOID OnBnClickedBtModify();
	//删除子项
	VOID OnBnClickedBtDelete();
	//查询参数
	VOID OnBnClickedBtQuery();
	//进入房间
	VOID OnBnClickedBtEnterRoom();
	//退出房间
	VOID OnBnClickedBtExitRoom();
	//游戏调试
	VOID OnBnClickedGameDebug();
	//AI高级属性
	VOID OnBnClickedAIAdvanced();
	//发送消息
	VOID OnBnclickedSendChat();
	//系统消息
	VOID OnBnClickedSystemMsg();
	//房间管理
	VOID OnBnClickedRoomManager();

	//用户接口
public:
	//用户激活
	virtual VOID OnUserItemAcitve(IClientUserItem * pIClientUserItem);
	//用户删除
	virtual VOID OnUserItemDelete(IClientUserItem * pIClientUserItem);
	//用户更新
	virtual VOID OnUserFaceUpdate(IClientUserItem * pIClientUserItem);
	//用户更新
	virtual VOID OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserScore & LastScore);
	//用户更新
	virtual VOID OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserStatus & LastStatus);
	//用户更新
	virtual VOID OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserAttrib & UserAttrib);
	//网络事件
protected:
	//用户处理
	bool OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//登录处理
	bool OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//配置处理
	bool OnSocketMainConfig(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//状态处理
	//bool OnSocketMainStatus(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////银行处理
	//bool OnSocketMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////任务处理
	//bool OnSocketMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////兑换处理
	//bool OnSocketMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////道具处理
	//bool OnSocketMainProperty(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//管理处理
	bool OnSocketMainManager(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////系统处理
	//bool OnSocketMainSystem(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏处理
	bool OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////比赛处理
	//bool OnSocketMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////签到处理
	//bool OnSocketMainCheckIn(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////会员处理
	//bool OnSocketMainMember(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	////低保处理
	//bool OnSocketMainBaseEnsure(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:
	//用户进入
	bool OnSocketSubUserEnter(VOID * pData, WORD wDataSize);
	//用户积分
	bool OnSocketSubUserScore(VOID * pData, WORD wDataSize);
	//用户状态
	bool OnSocketSubUserStatus(VOID * pData, WORD wDataSize);
	////用户游戏数据
	//bool OnSocketSubUserItemGameData(VOID * pData, WORD wDataSize);
	//挂机事件
	bool OnSocketSubAndroidHangingNotify(VOID * pData, WORD wDataSize);
	//等级事件
	bool OnSocketSubAndroidGradeNotify(VOID * pData, WORD wDataSize);
	//登录完成
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//登录成功
	bool OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize);
	//登录失败
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);
	//更新提示
	//bool OnSocketSubUpdateNotify(VOID * pData, WORD wDataSize);
	////请求失败
	//bool OnSocketSubRequestFailure(VOID * pData, WORD wDataSize);
	////用户聊天
	//bool OnSocketSubUserChat(VOID * pData, WORD wDataSize);
	////用户表情
	//bool OnSocketSubExpression(VOID * pData, WORD wDataSize);
	////用户私聊
	//bool OnSocketSubWisperUserChat(VOID * pData, WORD wDataSize);
	////私聊表情
	//bool OnSocketSubWisperExpression(VOID * pData, WORD wDataSize);
	//系统消息
	bool OnSocketSubSystemMessage(VOID * pData, WORD wDataSize);
	////动作消息
	//bool OnSocketSubActionMessage(VOID * pData, WORD wDataSize);
	////邀请玩家
	//bool OnSocketSubUserInvite(VOID * pData, WORD wDataSize);
	////道具成功
	//bool OnSocketSubPropertySuccess(VOID * pData, WORD wDataSize);
	////道具失败
	//bool OnSocketSubPropertyFailure(VOID * pData, WORD wDataSize);
	////道具效应
	//bool OnSocketSubPropertyEffect(VOID * pData, WORD wDataSize);
	////道具消息
	//bool OnSocketPropertyMessage(VOID * pData, WORD wDataSize);
	////道具喇叭
	//bool OnSocketPropertyTrumpet(VOID * pData, WORD wDataSize);
	////等待分配
	//bool OnSocketSubWaitDistribute(VOID *pData, WORD wDataSize);
	////道具效果
	//bool OnSocketSubUserPropBuff(VOID *pData, WORD wDataSize);
	////玩家背包
	//bool OnSocketSubUserBackpack(VOID *pData, WORD wDataSize);
	////道具使用
	//bool OnSocketSubPropertyUse(VOID *pData, WORD wDataSize);
	////查询赠送
	//bool OnSocketSubQuerySendPresent(VOID *pData, WORD wDataSize);
	////赠送道具
	//bool OnSocketSubPropertyPresent(VOID *pData, WORD wDataSize);
	////获取赠送
	//bool OnSocketSubGetSendPresent(VOID *pData, WORD wDataSize);
	////玩家喇叭
	//bool OnSocketSubUserTrumpet(VOID *pData, WORD wDataSize);
		
	//内部函数
public:
	// 聊天效验
	bool EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex);
	//发送聊天
	bool SendUserChatPacket(DWORD dwTargetUserID, LPCTSTR pszChatString, COLORREF crColor);

	// 用户选择
	virtual VOID OnChangeChatTarget(IClientUserItem * pIClientUserItem);
	// 调试信息
	virtual bool OnDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);
	//AI属性调试信息
	virtual bool OnAIConfigDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);
	// 调试消息
	bool  OnSocketSubUserDebug(const void * pData, WORD wDataSize);
	//AI属性配置消息
	bool  OnSocketSubUserAIConfigDebug(const void * pData, WORD wDataSize);
	// 发送登录
	bool SendLogonPacket();
	// 显示按钮
	VOID ShowHideButton(bool bShow);

	//用户进入
	VOID OnEventUserEnter(IClientUserItem * pIClientUserItem);
	//用户离开
	VOID OnEventUserLeave(IClientUserItem * pIClientUserItem);
	//功能函数
public:
	//获取实例
	static CPlatformFrame * GetInstance() { return m_pPlatformFrame; }
	//释放实例
	static void ReleaseInstance();

	DECLARE_MESSAGE_MAP()			
};

//////////////////////////////////////////////////////////////////////////////////

#endif