#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE

#pragma pack(1)

//平台定义
#include "..\..\全局定义\Platform.h"

//////////////////////////////////////////////////////////////////////////////////
//注册命令

#define MDM_CS_REGISTER				1									//服务注册

//服务注册
#define SUB_CS_C_REGISTER_PLAZA		100									//注册广场
#define SUB_CS_C_REGISTER_SERVER	101									//注册房间
#define SUB_CS_C_REGISTER_MATCH		102									//注册比赛
#define SUB_CS_C_REGISTER_CHAT		103									//注册聊天	
#define SUB_CS_C_REGISTER_PERSONAL		104								//注册约战房间限制	
#define SUB_CS_C_REGISTER_GROUP		105									//注册俱乐部
#define SUB_CS_C_REGISTER_AI		106									//AI注册

//注册结果
#define SUB_CS_S_REGISTER_FAILURE	200									//注册失败
#define SUB_CS_S_REGISTER_SUCCESS	201									//注册成功
//////////////////////////////////////////////////////////////////////////////////

//注册广场
struct CMD_CS_C_RegisterPlaza
{
	TCHAR							szServerAddr[32];					//服务地址
	TCHAR							szServerName[LEN_SERVER];			//服务器名
};

//注册游戏
struct CMD_CS_C_RegisterServer
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wServerID;							//房间索引
	WORD                            wServerKind;                        //房间类别
	WORD                            wServerType;                        //房间类型		
	WORD							wServerLevel;						//房间等级
	WORD							wServerPort;						//房间端口
	SCORE							lCellScore;							//单元积分
	SCORE							lEnterScore;						//进入积分
	BYTE							cbEnterMember;						//进入会员
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwFullCount;						//满员人数
	WORD							wTableCount;						//桌子数目
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerAddr[32];					//服务地址
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	DWORD							dwSetPlayerCount;					//设置人数
	TCHAR							szGameInfomation[LEN_SERVER];		//游戏说明
	BYTE							cbPlayMode;							//0 积分模式	1	金币模式
	bool							bGroupPersonalMaxChair;				//大联盟百人游戏
};

//注册失败
struct CMD_CS_S_RegisterFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//注册比赛
struct CMD_CS_C_RegisterMatch
{
	//基本信息
	WORD							wServerID;							//房间标识
	//基本信息
	DWORD							dwMatchID;							//比赛标识	
	LONGLONG						lMatchNo;							//比赛场次
	BYTE							cbMatchStatus;						//比赛状态
	BYTE							cbMatchType;						//比赛类型
	WCHAR							szMatchName[32];					//比赛名称

	//报名信息
	BYTE							cbFeeType;							//费用类型
	BYTE							cbDeductArea;						//缴费区域
	LONGLONG						lSignupFee;							//报名费用	
	BYTE							cbSignupMode;						//报名方式
	BYTE							cbJoinCondition;					//参赛条件
	BYTE							cbMemberOrder;						//会员等级
	DWORD							dwExperience;						//玩家经验
	DWORD							dwFromMatchID;						//比赛标识		
	BYTE							cbFilterType;						//筛选方式
	WORD							wMaxRankID;							//最大名次
	SYSTEMTIME						MatchEndDate;						//结束日期
	SYSTEMTIME						MatchStartDate;						//起始日期	

	//排名方式
	BYTE							cbRankingMode;						//排名方式	
	WORD							wCountInnings;						//统计局数
	BYTE							cbFilterGradesMode;					//筛选方式

	//分组设置
	BYTE							cbDistributeRule;					//分组规则
	WORD							wMinDistributeUser;					//最少人数
	WORD							wDistributeTimeSpace;				//分组间隔	
	WORD							wMinPartakeGameUser;				//最少人数
	WORD							wMaxPartakeGameUser;				//最多人数

	//比赛规则
	BYTE							cbMatchRule[512];					//比赛规则

	//奖励信息
	WORD							wRewardCount;						//奖励人数
	tagMatchRewardInfo				MatchRewardInfo[3];					//比赛奖励
};

//约战房创建房间限制
struct CMD_CS_S_RegisterPersonal
{
	DWORD dwKindID;
	BYTE  cbPlayMode;
	DWORD dwMaxCreate;
	bool  bSupportGroup;
};


//注册俱乐部
struct CMD_CS_C_RegisterGroup
{
	TCHAR							szServerAddr[32];					//服务地址	
	WORD							wServicePort;						//端口地址
};
//////////////////////////////////////////////////////////////////////////////////
//服务信息

#define MDM_CS_SERVICE_INFO			2									//服务信息

//房间信息
#define SUB_CS_C_SERVER_ONLINE		1									//房间人数
#define SUB_CS_C_SERVER_MODIFY		2									//房间修改
#define SUB_CS_C_SHARE_STORAGE		3									//共享库存
#define SUB_CS_C_GAMESERVER_WEB_SHARE_STORAGE	4						//共享库存

//广场命令
#define SUB_CS_S_PLAZA_INFO			100									//广场信息
#define SUB_CS_S_PLAZA_INSERT		101									//广场列表
#define SUB_CS_S_PLAZA_REMOVE		103									//广场删除
#define SUB_CS_S_PLAZA_FINISH		104									//广场完成

//房间命令
#define SUB_CS_S_SERVER_INFO		110									//房间信息
#define SUB_CS_S_SERVER_ONLINE		111									//房间人数
#define SUB_CS_S_SERVER_INSERT		112									//房间列表
#define SUB_CS_S_SERVER_MODIFY		113									//房间修改
#define SUB_CS_S_SERVER_REMOVE		114									//房间删除
#define SUB_CS_S_SERVER_FINISH		115									//房间完成
#define SUB_CS_S_SHARE_STORAGE		116									//共享库存
#define SUB_CS_S_WEB_SHARE_STORAGE	117									//共享库存

//比赛命令
#define SUB_CS_S_MATCH_INSERT		120									//比赛列表
#define SUB_CS_S_MATCH_OPTION       121									//比赛配置
#define SUB_CS_S_SIGNUP_COUNT		122									//报名人数

//聊天命令
#define SUB_CS_S_CHAT_INSERT		130									//聊天服务
#define SUB_SS_S_ONLINE				140									//用户上线
#define SUB_SS_S_OFFLINE			141									//用户离线
#define SUB_SS_S_STATUS				142									//用户状态
#define SUB_SS_S_GAMESTATUS			143									//游戏状态
#define SUB_SS_S_SYNCHRFRIEND		144									//同步好友	
#define SUB_SS_S_SYNCHARFINISH		145									//同步完成
#define SUB_SS_S_MODIFYFRIEND		146									//修改好友
#define SUB_SS_S_REMOVEGROUP		147									//移除分组

//////////////////////////////////////////////////////////////////////////////////

//房间数据
struct CMD_CS_C_ServerModify
{
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wNodeID;							//节点索引
	WORD							wServerPort;						//房间端口
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//AI数
	DWORD							dwFullCount;						//满员人数
	DWORD							dwSetCount;							//设置人数
	TCHAR							szServerAddr[32];					//服务地址
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szGameInfomation[LEN_SERVER];		//游戏说明
};

//房间人数
struct CMD_CS_C_ServerOnLine
{
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//AI数
	DWORD							dwSetCount;							//设置人数
};

//共享库存
struct CMD_CS_C_ShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;
	tagShareStorageItem				ShareStorageItem;
};

//比赛人数
struct CMD_CS_C_MatchSignUpCount
{
	LONGLONG						lMatchNo;							//比赛编号				
	DWORD							dwMatchCount;						//比赛人数
	WORD							wServerID;							//房间标识
};
//////////////////////////////////////////////////////////////////////////////////

//广场删除
struct CMD_CS_S_PlazaRemove
{
	DWORD							dwPlazaID;							//广场标识
};

//房间人数
struct CMD_CS_S_ServerOnLine
{
	WORD							wServerID;							//房间标识
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//AI数
	DWORD							dwSetCount;							//设置人数
};

//房间修改
struct CMD_CS_S_ServerModify
{
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wNodeID;							//节点索引
	WORD							wServerID;							//房间标识
	WORD							wServerPort;						//房间端口
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//AI数
	DWORD							dwFullCount;						//满员人数
	TCHAR							szServerAddr[32];					//服务地址
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	DWORD							dwSetPlayerCount;					//设置人数
	TCHAR							szGameInfomation[LEN_SERVER];		//游戏说明

};

//共享库存
struct CMD_CS_S_ShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;
	tagShareStorageItem				ShareStorageItem;
};

//共享库存
struct CMD_CS_S_WebShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;
	tagShareStorageItem				ShareStorageItem;
};

//房间删除
struct CMD_CS_S_ServerRemove
{
	WORD							wServerID;							//房间标识
};

//////////////////////////////////////////////////////////////////////////////////
//用户汇总

#define MDM_CS_USER_COLLECT			3									//用户汇总

//用户状态
#define SUB_CS_C_USER_ENTER			1									//用户进入
#define SUB_CS_C_USER_LEAVE			2									//用户离开
#define SUB_CS_C_USER_FINISH		3									//用户完成
#define SUB_CS_C_USER_STATUS		4									//用户状态

//用户状态
#define SUB_CS_S_COLLECT_REQUEST	100									//汇总请求

#define SUB_CS_S_USER_GAMESTATE		101									//用户状态

#define SUB_CS_S_PERSONAL_INFO			102									//约战信息
#define SUB_CS_S_PERSONAL_INFO_UPDATE	103									//更新
#define SUB_CS_S_DELETE_PERSONAL		104									//删除约战房间
//////////////////////////////////////////////////////////////////////////////////

//用户进入
struct CMD_CS_C_UserEnter
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	//辅助信息
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//详细信息
	tagUserInfo						userInfo;							//用户信息
};

//用户离开
struct CMD_CS_C_UserLeave
{
	DWORD							dwUserID;							//用户标识
};

//用户状态
struct CMD_CS_C_UserStatus
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	BYTE							cbUserStatus;						//用户状态
	WORD							wKindID;							//游戏标识
	WORD							wServerID;							//房间标识
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子位置
};

//用户状态
struct CMD_CS_S_UserGameStatus
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbGameStatus;						//用户状态
	WORD							wKindID;							//游戏标识
	WORD							wServerID;							//房间标识
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子位置
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//////////////////////////////////////////////////////////////////////////////////
//远程服务

#define MDM_CS_REMOTE_SERVICE		4									//远程服务

//查找服务
#define SUB_CS_C_SEARCH_DATABASE	1									//数据查找
#define SUB_CS_C_SEARCH_CORRESPOND	2									//协调查找
#define SUB_CS_C_SEARCH_ALLCORRESPOND	3								//协调查找

//查找结果
#define SUB_CS_S_SEARCH_DATABASE	100									//数据查找
#define SUB_CS_S_SEARCH_CORRESPOND	101									//协调查找
#define SUB_CS_S_SEARCH_ALLCORRESPOND	102								//协调查找

//////////////////////////////////////////////////////////////////////////////////

//协调查找
struct CMD_CS_C_SearchCorrespond
{
	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址

	//查找数据
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
};

//协调查找
struct CMD_CS_S_SearchCorrespond
{
	//连接信息
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址

	//数据信息
	WORD							wUserCount;							//用户数目
	tagUserRemoteInfo				UserRemoteInfo[16];					//用户信息
};


//协调查找
struct CMD_CS_C_AllSearchCorrespond
{
	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址

	DWORD							dwCount;
	DWORD							dwGameID[1];						//游戏标识
};

//协调查找
struct CMD_CS_S_SearchAllCorrespond
{
	//连接信息
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址

	//数据信息
	DWORD							dwCount;
	tagUserRemoteInfo				UserRemoteInfo[1];
};


//////////////////////////////////////////////////////////////////////////////////
//管理服务

#define MDM_CS_MANAGER_SERVICE		5									//管理服务

#define SUB_CS_C_SYSTEM_MESSAGE 	1									//系统消息
#define SUB_CS_C_PROPERTY_TRUMPET 	2									//喇叭消息

#define SUB_CS_S_SYSTEM_MESSAGE 	100									//系统消息
#define SUB_CS_S_PROPERTY_TRUMPET  	200									//喇叭消息
#define SUB_CS_S_PLATFORM_PARAMETER 300									//平台参数	


//发送喇叭
struct CMD_CS_S_SendTrumpet
{
	WORD                           wPropertyIndex;                      //道具索引 
	DWORD                          dwSendUserID;                         //用户 I D
	DWORD                          TrumpetColor;                        //喇叭颜色
	TCHAR                          szSendNickName[32];				    //玩家昵称 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //喇叭内容
};
//////////////////////////////////////////////////////////////////////////////////
//机器服务

#define MDM_CS_ANDROID_SERVICE	    6									//机器服务

#define SUB_CS_C_ADDPARAMETER		100									//添加参数
#define SUB_CS_C_MODIFYPARAMETER    101									//修改参数
#define SUB_CS_C_DELETEPARAMETER	102									//删除参数	

#define SUB_CS_S_ADDPARAMETER		200									//添加参数
#define SUB_CS_S_MODIFYPARAMETER    201									//修改参数
#define SUB_CS_S_DELETEPARAMETER	202									//删除参数	

//////////////////////////////////////////////////////////////////////////////////
//添加参数
struct CMD_CS_C_AddParameter
{
	WORD							wServerID;							//房间标识
	tagAndroidParameter				AndroidParameter;					//机器参数
};

//修改参数
struct CMD_CS_C_ModifyParameter
{
	WORD							wServerID;							//房间标识
	tagAndroidParameter				AndroidParameter;					//机器参数
};

//删除参数
struct CMD_CS_C_DeleteParameter
{
	WORD							wServerID;							//房间标识
};


//添加参数
struct CMD_CS_S_AddParameter
{	
	tagAndroidParameter				AndroidParameter;					//机器参数
};

//修改参数
struct CMD_CS_S_ModifyParameter
{
	tagAndroidParameter				AndroidParameter;					//机器参数
};

//删除参数
struct CMD_CS_S_DeleteParameter
{
	WORD							wServerID;							//房间标识
};

//++++++++++++++++++++++++++++++约战房间
//私人房间命令
#define SUB_CS_C_SEARCH_TABLE					131						//查找桌子
#define SUB_CS_S_SEARCH_TABLE_RESULT			132						//查找结果
#define SUB_CS_C_QUERY_GAME_SERVER				133						//查找房间
#define SUB_CS_S_QUERY_GAME_SERVER_RESULT		134						//查找结果
#define SUB_CS_C_CREATE_TABLE					135						//创建桌子
#define SUB_CS_S_CREATE_TABLE_RESULT			136						//创建结果
#define SUB_CS_C_DISMISS_TABLE					137						//解散桌子
#define SUB_CS_C_DISMISS_TABLE_RESULT			138						//解散桌子
#define SUB_CS_S_INSERT_CREATE_RECORD			139						//插入创建记录
#define SUB_CS_S_QUERY_PERSONAL_ROOM_LIST		140						//请求私人房间列表
#define SUB_CS_C_QUERY_PERSONAL_ROOM_LIST_RESULT	141					//请求私人房间结果 
#define SUB_CS_C_DISSUME_SEARCH_TABLE			142						//请求解散私人房间 
#define SUB_CS_S_DISSUME_SEARCH_TABLE_RESULT	143						//查找结果
#define SUB_CS_S_QUERY_ROOM_PASSWORD			144						//查找分享密码
#define SUB_CS_S_QUERY_ROOM_PASSWORD_RESULT		145						//查找分享密码结果
#define SUB_CS_C_UPDATE_TABLE_COUNT				146						//更新人数
#define SUB_CS_C_QUERY_GOLD_ROOM_LIST			147						//金币约战房间列表
#define SUB_CS_C_QUERY_GOLD_ROOM_LIST_RESULT	148						//金币约战房间列表结果
#define SUB_CS_C_QUERY_GOLD_ROOM_LIST_FINISH	149						//金币约战房间列表结果
#define SUB_CS_C_CURRENCE_GROUP_FEE				150						//亲友圈房间消耗
#define SUB_CS_C_UPDATE_BANKER_USERID			151						//更新大联盟百人游戏庄家USERID

//俱乐部相关
#define MDM_CS_CLUB_SERVICE_INFO				8						//俱乐部服务信息

#define SUB_CS_C_QUERY_CLUB_ROOM_LIST			200						//俱乐部房间
#define SUB_CS_C_QUERY_CLUB_ROOM_LIST_RESULT	201						//俱乐部房间结果
#define SUB_CS_C_QUERY_CLUB_ROOM_LIST_FINISH	202						//俱乐部房间完成
#define SUB_CS_C_CLUB_USER_STATUS				203						//俱乐部用户状态变化
#define SUB_CS_C_CLUB_CREATE_TABLE				204						//俱乐部房间创建
#define SUB_CS_C_CLUB_DISMISS_TABLE				205						//俱乐部房间解散
#define SUB_CS_C_CLUB_CURRENCE_FEE				206						//房间消耗
#define SUB_CS_C_CLUB_CREATE_RULE				207						//俱乐部玩法创建
#define SUB_CS_C_CLUB_DISMISS_RULE				208						//俱乐部玩法删除
#define SUB_CS_C_UPDATE_GROUP_SCORE				209						//更新亲友圈币
#define SUB_CS_C_CHECK_NEW_USER					210						//检查新玩家
#define SUB_CS_C_QUERY_ROOM_BATTLE_INFO			211						//查询桌子对局信息
#define SUB_CS_C_QUERY_ROOM_BATTLE_INFO_RES		212						//查询桌子对局信息结果
#define SUB_CS_S_CLUB_SERVER_CLOSE				213						//服务器关闭
#define SUB_CS_C_CLUB_DELETE_CONFIG				214						//俱乐部玩法删
#define SUB_CS_C_CLUB_TABLE_CHANGEBANKER		215						//俱乐部更换庄家
#define SUB_CS_C_CREATE_GROUP_SUCCESS			216						//俱乐部创建成功

//获取私人房间列表
struct CMD_MB_QeuryPersonalRoomList
{
	DWORD							dwUserID;							//用户ID
};

struct CMD_MB_SC_QeuryPersonalRoomList
{
	DWORD							dwUserID;							//用户ID
	DWORD							dwSocketID;						//网络ID
};


//玩家请求房间成绩
struct CMD_GR_QUERY_USER_ROOM_SCORE
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwKindID;								//房间类型
};

//++++++++++++++++++++++++++++++约战房间
//查找桌子
struct CMD_CS_C_SearchTable
{
	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwKindID;							//游戏类型标识
	DWORD							dwPersonalRoomID;					//约战房间ID	
};

//查找结果
struct CMD_CS_S_SearchTableResult
{
	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址

	DWORD							dwServerID;							//房间I D
	DWORD							dwTableID;							//桌子I D
	DWORD							dwKindID;							//房间类型
	DWORD							dwGroupID;							//俱乐部ID
};

//查找房间密码结果
struct CMD_CS_S_QueryRoomPasswordResult
{
	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址

	DWORD							dwRoomPassword;						//房间密码	
};

//查询桌子
struct CMD_CS_C_QueryGameServer
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwKindID;							//游戏标识
	BYTE							cbIsJoinGame;						//是否参与游戏
	BYTE							cbPlayMode;							//0 积分模式 1 金币模式
	DWORD							dwGroupID;							//亲友圈ID
	DWORD							dwConfigID;							//玩法ID

	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
};

//查询结果
struct CMD_CS_S_QueryGameServerResult
{
	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址

	//房间信息
	DWORD							dwServerID;							//查询结果
	bool							bCanCreateRoom;						//是否可以创建房间

	//错误描述
	TCHAR							szErrDescrybe[MAX_PATH];			//错误描述
};

//创建桌子
struct CMD_CS_C_CreateTable
{
	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szClientAddr[16];					//创建房间的IP地址
	BYTE							cbPayMode;							//支付模式
	BYTE							cbGameMode;							//游戏模式
	LONG							lNeedRoomCard;						//理论上支付房卡数量	
	//桌子信息
	tagPersonalTableInfo			PersonalTable;						//桌子信息
};

//创建结果
struct CMD_CS_S_CreateTableResult
{
	//连接参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szClientAddr[16];					//创建房间的IP地址
	BYTE							cbPayMode;							//支付模式
	BYTE							cbGameMode;							//游戏模式
	LONG							lNeedRoomCard;						//理论上支付房卡数量
	//桌子信息
	tagPersonalTableInfo			PersonalTable;						//桌子信息
	////桌子信息
	//DWORD							dwTableID;							//桌子I D						
	//TCHAR							szServerID[7];						//房间编号
};
//更新桌子
struct CMD_CS_C_UpdateTablePlayerCount
{
	DWORD							dwSocketID;							//网络ID
	WORD							wServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	WORD							wSitCount;							//坐下人数
	DWORD							dwUserID;							//操作用户
	DWORD							wChairID;							//椅子位置
	bool							bSit;								//坐或起立
	bool							bGameStart;							//游戏开始

	CMD_CS_C_UpdateTablePlayerCount()
	{
		dwSocketID	= -1;
		wServerID	= 0;
		wTableID	= 0;
		wSitCount	= 0;
		dwUserID = INVALID_DWORD;
		wChairID = INVALID_CHAIR;
		bSit = false;
		bGameStart = false;
	}
};

//更新联盟百人游戏庄家USERID
struct CMD_CS_C_UpdateGroupMaxChairBankerUserID
{
	WORD							wServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	WORD							wBankerChairID;						//庄家位置
	DWORD							dwBankerUserID;						//庄家USERID

	CMD_CS_C_UpdateGroupMaxChairBankerUserID()
	{
		wServerID = 0;
		wTableID = 0;
		wBankerChairID = INVALID_CHAIR;
		dwBankerUserID = 0;
	}
};

//解散桌子
struct CMD_CS_C_DismissTable
{
	DWORD							dwSocketID;						//网络ID
	DWORD							dwServerID;							//房间I D
	DWORD							dwTableID;							//桌子I D
	CMD_CS_C_DismissTable()
	{
		dwSocketID = -1;
	}
};

//房主强制解散游戏
struct CMD_GR_HostDissumeGame
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwTableID;							//桌子 I D
};

struct CMD_CS_C_DismissTableResult
{
	DWORD								dwSocketID;									//网络ID
	tagPersonalTableInfo				PersonalTableInfo;							
	tagPersonalUserScoreInfo			PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];
};

//私人房间列表
struct CMD_CS_C_HostCreatRoomInfo
{
	DWORD								wSocketID;							//网络标识
	int									nCreatRoomCount;					//房间数目
	tagHostCreatRoomInfo				HostCreatRoomInfo;					//房间信息
};

struct CMD_CS_C_GoldPersonalRoomInfo
{
	DWORD							dwRoomID;							//房间I D
	DWORD							dwKindID;							//房间类型
	DWORD							dwServerID;							//房间I D
	DWORD							dwTableID;							//桌子I D
	SCORE							lCellScore;							//房间底分
	DWORD							dwRoomTax;							//单独一个私人房间的税率，千分比
	WORD							wPlayerCount;						//参与游戏最大人数
	WORD							wPlayingUser;						//参与游戏的人数
	WORD							wBeginFreeTime;						//免费时段开始
	WORD							wEndFreeTime;						//免费时段结束
	SCORE							lEnterScoreLimit;					//进入房间上限
	SCORE							lLeaveScoreLimit;					//离开房间下限
	BYTE							cbGameRule[RULE_LEN];				//游戏规则 弟 0 位标识 是否设置规则 0 代表设置 1 代表未设置
	TCHAR							szCreateTime[32];					//创建时间
};

//亲友圈房间消耗
struct CMD_GR_CurrenceGroupFee
{
	DWORD					dwGroupID;							//亲友圈ID
	SCORE					lFeeDiamond;						//消耗钻石
	SCORE					lDiamond;							//剩余钻石
};

//桌子创建记录
struct DBR_GR_InsertCreateRecord
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwServerID;							//房间标识
	LONGLONG						lCellScore;							//设置底分
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	DWORD							dwPersonalRoomID;					//约战房间ID
	TCHAR							szPassword[LEN_PASSWORD];			//密码设置
	DWORD							dwRoomTax;							//单独一个私人房间的税率，千分比
	WORD							wJoinGamePeopleCount;				//参与游戏的人数
	TCHAR							szClientAddr[16];					//创建房间的IP地址
	BYTE							cbPayMode;							//支付模式
	LONG							lNeedRoomCard;						//理论上需要消耗的房卡
	BYTE							cbGameMode;							//游戏模式
	BYTE							cbPlayMode;							//0 积分场 1 金币场
	LONGLONG						lGroupID;							//俱乐部ID
	LONGLONG						lMinEnterScore;
};

//请求金币房间列表
struct CMD_CS_C_QueryGoldPersonalRoomList
{
	DWORD							dwSocketID;						//网络标识
	WORD							wKindID;						//游戏类型					
	BYTE							cbPlayMode;						//0 积分场 1 金币场
	DWORD							dwGroupID;						//俱乐部id， 非俱乐部模式 填 0
};

//请求金币房间列表
struct CMD_CS_C_QueryGoldPersonalRoomListResult
{
	DWORD							dwSocketID;						//网络标识	
};
//////////////////////////////////////////////////////////////////////////////////
//俱乐部相关

//查询房间
struct CMD_CS_C_QueryClubRoom
{
	DWORD							dwSocketID;						//网络标识	
	DWORD							dwGroupID;						//俱乐部ID
};

//查询结果
struct CMD_CS_S_QueryResult
{
	DWORD							dwSocketID;						//网络标识
	DWORD							dwGroupID;						//俱乐部ID
	WORD							wTableCount;					//查询数目
};
//查询完毕
struct CMD_CS_S_QueryFinish
{
	DWORD							dwSocketID;						//网络标识	
};

//桌子状态变化
struct CMD_CS_S_UpdateTable
{
	DWORD                           dwKindID;							//类型ID
	DWORD							dwServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	DWORD							dwPersonalRoomID;					//约战房间ID
	DWORD							dwGroupID;							//俱乐部ID
	DWORD							dwUserID;							//操作用户
	DWORD							wChairID;							//椅子位置
	bool							bSit;								//坐或起立
	bool							bGameStart;							//游戏开始
};

//桌子庄家变化
struct CMD_CS_S_TableChangeBanker
{
	DWORD                           dwKindID;							//类型ID
	DWORD							dwServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	DWORD							dwPersonalRoomID;					//约战房间ID
	DWORD							dwGroupID;							//俱乐部ID
	DWORD							wBankerChairID;						//庄家位置
	DWORD							dwBankerUserID;						//百人游戏庄家USERID(无效为0)
};

//结算桌子
struct CMD_CS_S_DismissClubTable
{
	DWORD                           dwKindID;							//类型ID
	DWORD							dwServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	DWORD							dwPersonalRoomID;					//约战房间ID
	DWORD							dwGroupID;							//俱乐部ID
	bool							bDeleteServer;						//true删除所有server房间

};

//圈友分数变化
struct CMD_CS_S_UpdateClubScore
{
	DWORD							dwServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	DWORD							dwPersonalRoomID;					//约战房间ID
	DWORD							dwGroupID;							//俱乐部ID
	DWORD							dwUserID;							//操作用户
	DWORD							wChairID;							//椅子位置
	SCORE							lScore;								//分数
};

//检查新玩家
struct CMD_CS_S_CheckNewUser
{
	DWORD							dwUserID;							//玩家I D
	TCHAR							szClientIP[15];						//IP
};

//查询桌子对战信息
struct CMD_CS_C_QueryTableInfo
{
	DWORD							dwSocketID;							//网络标识	
	DWORD							dwGroupID;							//群组标识
	DWORD							dwServerID;							//服务器ID
	DWORD							dwTableID;							//桌子ID
};

//查询桌子对战信息结果
struct CMD_CS_S_QueryTableInfoResult
{
	DWORD							dwSocketID;							//网络标识	
	DWORD							dwGroupID;							//群组标识
	DWORD							dwRoomID;							//服务器ID
	DWORD							dwAllCount;							//总局数
	DWORD							dwPlayCount;						//当前局数
	DWORD							dwUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//玩家ID
	SCORE							lGameScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//游戏得分

};

//俱乐部玩法删除
struct CMD_CS_C_ClubDeleteConfig
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwGroupID;							//群组ID
	DWORD							dwConfigID;							//玩法ID
};

//俱乐部创建成功
struct CMD_CS_C_CreateGroupSuccess
{
	DWORD							dwGroupID;							//俱乐部ID
};

//////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++++网站服务
#define MDM_CS_WEB_SERVICE						9							//网站服务

//调度命令
#define SUB_CS_C_UPDATE_PARAMTER				101						//更新配置
#define SUB_CS_C_UPDATE_USER_SCORE				102						//更新财富
#define SUB_CS_C_UPDATE_MAIL					103						//推送邮件
#define SUB_CS_C_WEB_SHARE_STORAGE				106						//共享库存

//网站命令 AI 相关
#define SUB_CS_C_UPDATE_AI_PARAMTER				201						//更新AI加载配置
#define SUB_CS_C_UPDATE_AI_WEIGHT_OUT			202						//更新AI离开权重配置
#define SUB_CS_C_UPDATE_AI_WEIGHT_IN			203						//更新AI进入房间配置
#define SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL			204						//更新AI人数权重
#define SUB_CS_C_DELETE_AI						205						//删除AI

//返回网站
#define SUB_CS_S_UPDATE_SUCCESS					111						//更新成功
#define SUB_CS_S_UPDATE_FAILURE					112						//更新失败

//转发命令
#define SUB_CS_S_UPDATE_PARAMTER				101						//更新配置
#define SUB_CS_S_UPDATE_USER_SCORE				102						//更新财富
#define SUB_CS_S_UPDATE_MAIL					103						//推送邮件
#define SUB_CS_S_GROUP_MAX_MEMBER				104						//群组最大人数

//AI操作结果
#define SUB_SW_S_AI_OPERATE_RESULT				105						//AI 操作结果

//
#define SUB_CS_S_WEB_SHARE_STORAGE_RESULT		113						//共享库存结果

//更新配置
struct CMD_CS_C_UpdateParamter
{
	WORD							wUpdateType;						//更新类型
};

//更新分数
struct CMD_CS_C_UpdateUserScore
{
	DWORD							dwUserID;							//用户 I D
};

//推送邮件
struct CMD_CS_C_UpdateMail
{
	WORD							wUserCount;							//目标数量
};

//更新群组最大人数
struct CMD_CS_C_GroupMaxMember
{
	DWORD							dwGroupID;							//群组ID，0是对所有群
	WORD							wMaxCount;
};

//共享库存
struct CMD_CS_C_WebShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;				//共享库存状态
	tagShareStorageItem				ShareStorageItem;					//共享库存子项
};

//共享库存
struct CMD_CS_C_GameServerWebShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;				//共享库存状态
	tagShareStorageItem				ShareStorageItem;					//共享库存子项
	TCHAR							szResultDescripe[512];				//结果描述
};

//AI 参数更新
struct CMD_CS_C_UpdateAIParameter
{
	BYTE							cbStatus;					//0 删除 1 增加 2 修改
	DWORD							dwGroupID;					//俱乐部ID
	SCORE							lDistirbuteTotalScore;		//分配总分数
	DWORD							dwAICount;					//AI总数
	WORD							wKindID;					//游戏标识
	DWORD							dwPlayingAIRateMin;			//游戏中的AI
	DWORD							dwPlayingAIRateMax;			//等待中的AI
	SCORE							lDistirbuteSingleScore;		//单个AI分配分数
	SCORE							lDistirbuteScoreMinRatio;	//单个AI分配分数浮动比例
	SCORE							lDistirbuteScoreMaxRatio;	//单个AI分配分数浮动比例
};


//AI 参数更新
struct CMD_CS_C_UpdateAIParameterToAI
{
	DWORD dwSocketID;											//网络标识
	BYTE							cbStatus;					//0 删除 1 增加 2 修改
	DWORD							dwGroupID;					//俱乐部ID
	SCORE							lDistirbuteTotalScore;		//分配总分数
	DWORD							dwAICount;					//AI总数
	WORD							wKindID;					//游戏标识
	DWORD							dwPlayingAIRateMin;			//游戏中的AI
	DWORD							dwPlayingAIRateMax;			//等待中的AI
	SCORE							lDistirbuteSingleScore;		//单个AI分配分数
	SCORE							lDistirbuteScoreMinRatio;	//单个AI分配分数浮动比例
	SCORE							lDistirbuteScoreMaxRatio;	//单个AI分配分数浮动比例
};



//AI 离开权限更新
struct CMD_CS_C_UpdateAIWeightOut
{
	int nIndex;
	tagWeightOutRatio WeightOutRatio;
};

//AI 离开权限更新
struct CMD_CS_C_UpdateAIWeightOutToAI
{
	DWORD dwSocketID;						//网络标识
	CMD_CS_C_UpdateAIWeightOut UpdateAIWeightOut;
};


//AI 进入房间权限更新
struct CMD_CS_C_UpdateAIWeightIn
{
	int nIndex;
	tagWeightInRatio WeightInRatio;
};

//AI 进入房间权限更新
struct CMD_CS_C_UpdateAIWeightInToAI
{
	
	DWORD dwSocketID;						//网络标识
	CMD_CS_C_UpdateAIWeightIn UpdateAIWeightIn;
};

//不同时间段 AI 总数
struct CMD_CS_C_UpdateAITimeTotal
{
	int nIndex;
	DWORD dwTime;
	DWORD dwTimeChange;
};

//不同时间段 AI 总数
struct CMD_CS_C_UpdateAITimeTotalToAI
{
	DWORD dwSocketID;						//网络标识
	CMD_CS_C_UpdateAITimeTotal UpdateAITimeTotal;
};


//操作结果
struct CMD_WS_S_AIOperateResult
{
	DWORD							dwSocketID;						//网络标识
	WORD							wOperateID;							//操作标识
	BYTE							cbResult;							//操作结果
	TCHAR							szDescripe[260];					//登录密码
};

//共享库存结果
struct CMD_CS_S_WebShareStorageResult
{
	bool							bGameServerRegister;				//注册标识
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;				//共享库存状态
	tagShareStorageItem				ShareStorageItem;					//共享库存子项
	TCHAR							szResultDescripe[512];				//结果描述
};

struct tagDeleteAI
{
	WORD  wKindID;
	DWORD dwUserID; //一次最多删除20个AI
	byte  cbIsAI;	//0 真实玩家 1 机器人
};

//删除俱乐部中的玩家
struct CMD_CS_C_DeleteAI
{
	byte  cbOperate;	//0 下线 1 退出俱乐部
	DWORD dwGroupID;
	tagDeleteAI DeleteAI[20];
};

//不同时间段 AI 总数
struct CMD_CS_C_DeleteAIParameterToAI
{
	DWORD dwSocketID;						//网络标识
	CMD_CS_C_DeleteAI DeleteAI;				//删除AI
};

#pragma pack()

#endif