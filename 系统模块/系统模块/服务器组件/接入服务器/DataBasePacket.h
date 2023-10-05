#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//请求数据包

//系统命令
#define DBR_GR_LOAD_GROUP_OPTION	1									//加载配置
#define DBR_GR_LOAD_FORBID_WORDS	2									//禁止词汇

//群组命令
#define DBR_GR_LOGON_SERVER			10									//用户登录
#define DBR_GR_CREATE_GROUP			11									//创建群组
#define DBR_GR_UPDATE_GROUP			12									//更新群组
#define DBR_GR_DELETE_GROUP			13									//删除群组
#define DBR_GR_INSERT_MEMBER		14									//添加成员
#define DBR_GR_DELETE_MEMBER		15									//删除成员
#define DBR_GR_SETTLE_BATTLE		16									//约战结算
#define DBR_GR_QUERY_BATTLE			17									//查询约战
#define DBR_GR_QUERY_TOTAL_BATTLE	18									//约战总数
#define DBR_GR_APPEND_CONFIG		19									//创建玩法
#define DBR_GR_MODIFY_CONFIG		20									//修改玩法
#define DBR_GR_DELETE_CONFIG		21									//删除玩法
#define DBR_GR_MODIFY_RIGHT			22									//修改权限
#define DBR_GR_PRESENT_SCORE		23									//赠送勋章
#define DBR_GR_QUERY_PRESENT		24									//赠送记录
#define DBR_GR_QUERY_REVENUE		25									//扣费记录
#define DBR_GR_QUERY_DAYRECORD		26									//日记录
#define DBR_GR_QUERY_CREATE			27									//查询创建限制
#define DBR_GR_QUERY_MEDAL_MODE		28									//查询勋章模式
#define DBR_GR_SET_GROUP_STATUS		29									//设置大联盟状态
//申请命令
#define DBR_GR_APPLY_REQUEST		30									//申请请求
#define DBR_GR_APPLY_RESPOND		31									//申请响应
#define DBR_GR_APPLY_DELETE			32									//申请删除

#define DBR_GR_SET_ROLLMSG			33									//设置跑马灯
#define DBR_GR_SET_NOTICE			34									//设置公告
#define DBR_GR_SET_INOUT			35									//进出设置
#define DBR_GR_RIGHT_MANAGEMENT		36									//权限管理
#define DBR_GR_RIGHT_QUERY			37									//权限查询
#define DBR_GR_SET_MEDAL			38									//勋章设置
#define DBR_GR_SET_FREETABLE_LIMIT	39									//空桌限制
#define DBR_GR_SET_BIGWINNER_SHOW	40									//大赢家显示
#define DBR_GR_QUERY_USERDAY_DETAIL	41									//玩家玩家日详情
#define DBR_GR_QUERY_GROUPDAY_DETAIL 42									//玩家大联盟日详情
#define DBR_GR_SET_FLAG				43									//设置点赞标志
#define DBR_GR_MEDAL_CLEAR			44									//勋章清零
#define DBR_GR_SET_FAVORITE_CONFIG	45									//设置收藏玩法
#define DBR_GR_MODIFY_GROUP_NAME	46									//修改大联盟名称
#define DBR_GR_QUERY_ROOM_DETAIL	47									//查询房间对局详情

/////////////////小队相关
#define DBR_GR_QUERY_CAPTAIN_LIST	48									//查询队长
#define DBR_GR_ADD_CAPTAIN_TEAMER	49									//调配队员
#define DBR_GR_QUERY_TEAM_RECORD	50									//小队日统计
#define DBR_GR_QUERY_TEAMER_RECORD	51									//小队成员日统计
#define DBR_GR_SET_PRIVACY			52									//隐私设置
#define DBR_GR_SET_CAPTAIN_POINT	53									//队长点位设置
#define DBR_GR_OPEN_BOX				54									//打开积分保险箱
#define DBR_GR_GET_AWARD_SCORE		55									//提取积分
#define DBR_GR_QUERY_CODE			56									//查询邀请码
#define DBR_GR_BLIND_CODE			57									//绑定邀请码
#define DBR_GR_CHECK_NEW_USER		58									//检查新玩家
#define DBR_GR_QUERY_SCORE			59									//查询可用分

////////////////////////////
#define DBR_GR_UPDATE_ONLINE_STATUS			60							//更新俱乐部在线状态

#define DBR_GR_MODIFY_INFO			61									//修改资料

//////////////////////////////////////////////////////////////////////////

//用户登录
struct DBR_GR_LogonServer
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//创建群组
struct DBR_GR_CreateGroup
{
	//规则信息
	BYTE							cbPayType;							//支付类型	
	BYTE							cbCurrencyKind;						//货币类型	

	//用户信息
	DWORD							dwCreateID;							//群主标识	
	DWORD							dwStationID;						//站点标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//群组信息
	TCHAR							szGroupName[LEN_GROUP_NAME];		//群组名称
	TCHAR							szGroupIntroduce[LEN_GROUP_INTRODUCE];//群组介绍	
};

//更新群组
struct DBR_GR_UpdateGroup
{
	//群组信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwStationID;						//站点标识
	BYTE							cbUpdateKind;						//更新类型

	//用户信息
	DWORD							dwCreateID;							//群主标识	
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//删除群组
struct DBR_GR_DeleteGroup
{
	//群组信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwStationID;						//站点标识

	//用户信息
	DWORD							dwCreateID;							//群主标识	
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//设置群组状态
struct DBR_GR_SetGroupStatus
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwCreateID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	BYTE							cbStatus;							//状态值1生效，2删除，3冻结
};

//添加成员
struct DBR_GR_InsertMember
{
	//群组信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//成员标识	

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//删除成员
struct DBR_GR_DeleteMember
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	INT								nCount;
};

//申请请求
struct DBR_GR_ApplyRequest
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwApplyerID;						//用户标识		
	BYTE							cbApplyType;						//申请类型
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	LONGLONG						lInvitationCode;					//邀请码，申请类型为0时有效
};

//申请响应
struct DBR_GR_ApplyRespond
{
	DWORD							dwMessageID;						//消息标识

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//申请信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwApplyerID;						//用户标识	
	BYTE							cbApplyStatus;						//申请状态
};

//申请删除
struct DBR_GR_ApplyDelete
{
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//约战结算
struct DBR_GR_SettleBattle
{		
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//成员标识
	DWORD							dwSettleCount;						//结算次数

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//查询约战
struct DBR_GR_QueryBattle
{
	DWORD							dwGroupID;							//群组标识

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//查询约战
struct DBR_GR_QueryBattleResult
{
	DWORD							dwCreateCount;						//创建数目
	LONGLONG						lConsumeIngot;						//消耗数目
	DWORD							dwRecordCount;						//记录数目
};


//创建玩法
struct DBR_GR_AppendConfig
{
	DWORD							dwCreateID;								//创建者ID
	DWORD							dwGroupID;								//大联盟id
	tagGroupConfigInfo				configInfo;								//玩法信息
};
//修改玩法
struct DBR_GR_ModifyConfig
{
	DWORD							dwUserID;								//用户标识
	DWORD							dwGroupID;								//群组ID
	tagGroupConfigInfo				configInfo;								//玩法信息

};
//删除玩法
struct DBR_GR_DeleteConfig
{
	DWORD							dwUserID;								//玩家ID
	DWORD							dwGroupID;								//群组ID
	DWORD							dwConfigID;								//玩法ID
	bool							bGroupPersonalMaxChair;					//大联盟百人游戏
	bool							bAttemperEngineConclude;				//
};

//修改权限
struct DBR_GR_ModifyRight
{
	//群组信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//成员标识

	//权限信息
	INT								nRight;								//修改权限
	BYTE							cbMode;								//0增加权限，1删除权限

	//用户信息
	DWORD							dwCreateID;							//操作用户标识(只能是擂主)
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码	
};

//赠送勋章
struct DBR_GR_PresentScore
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwPresentUserID;					//赠送玩家ID
	LONGLONG						lPresentScore;						//赠送数目
	BYTE							cbType;								//0赠送，1消耗
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	INT								nCount;

};

//查询赠送记录
struct DBR_GR_QueryPresent
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwQueryUserID;						//查询玩家ID
};

//查询扣费
struct DBR_GR_QueryRevenue
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwQueryUserID;						//查询玩家ID
};

//查询日记录
struct DBR_GR_QueryDayRecord
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwQueryUserID;						//查询玩家ID
	DWORD							dwDateID;							//日期ID
};

//查询玩家日详情
struct DBR_GR_QueryUserDayDetail
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwQueryUserID;						//查询玩家
	DWORD							dwTargetUserID;						//目标玩家
	DWORD							dwDateID;							//日期ID --20190715
};

//查询大联盟日详情
struct DBR_GR_QueryGroupDayDetail
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwQueryUserID;						//查询玩家
	DWORD							dwDateID;							//日期ID --20190715
};

//查询创建限制
struct DBR_GR_QueryCreate
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwConfigID;							//玩法ID
	DWORD							dwUserID;							//玩家ID
};

//查询勋章模式
struct DBR_GR_QueryMedalMode
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwUserID;							//玩家ID
};

//跑马灯消息
struct DBR_GR_SetRollMsg
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbFlag;								//0关闭，1开启
	TCHAR							szRollMsg[33];						//消息内容最多32个字
};

//公告消息
struct DBR_GR_SetNotice
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szNotice[109];						//消息内容最多108个字
};

//进出设置
struct DBR_GR_SetInOut
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbInMode;							//进入设置0不需审核，1需审核
	BYTE							cbInType;							//进入方式0邀请码加入（隐藏ID），1大联盟ID加入
	BYTE							cbOutMode;							//退出设置0不需审核，1需审核
};

//权限管理
struct DBR_GR_RightManagement
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwTargetID;							//目标用户ID
	LONGLONG						lManagerRight;						//管理员权限
	LONGLONG						lCaptainRight;						//队长权限
};

//权限查询
struct DBR_GR_QueryRight
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
};

//勋章设置
struct DBR_GR_SetMedal
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbMedalMode;						//0关闭，1开启
	BYTE							cbMedalShow;						//0不显示，1显示
	BYTE							cbCurMedalShow;						//0所有人可见，1管理员可见
	BYTE							cbMedalRevenueShow;					//0所有人可见，1管理员可见
	BYTE							cbMedalClear;						//0不清零，1零点清零
};

//空桌设置
struct DBR_GR_SetFreeTableLimit
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbCount;							//数量
};

//大赢家设置
struct DBR_GR_SetBigWinner
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbShow;								//0所有可见，1管理员可见
};

//设置点赞标记
struct DBR_GR_SetFlag
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwOperateUserID;					//操作玩家
	DWORD							dwTargetUserID;						//目标玩家
	DWORD							dwDateID;							//日期ID --20190715
	BYTE							cbFlag;								//标记，0取消，1生效
};

//设置收藏玩法
struct DBR_GR_SetFavoriteConfig
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//操作玩家
	DWORD							dwConfigID;							//收藏玩法
};

//修改大联盟名称
struct DBR_GR_ModifyGroupName
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//操作玩家
	TCHAR							szGroupName[LEN_GROUP_NAME];		//群组名称
};

//勋章清零
struct DBR_GR_ClearMedal
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwDateID;
};

//查询房间详情
struct DBR_GR_QueryRoomDetail
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwRoomID;							//房间ID
	DWORD							dwDateID;							//日期标识
	DWORD							dwUserID;							//查询玩家
	SYSTEMTIME						EndTime;							//房间结束时间
};

//查询队长列表
struct DBR_GR_QueryCaptain
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//查询玩家
	DWORD							dwCaptainID;						//队长ID,为0时，查询大联盟直属队长，不为0，则查询该队长的下属队长
	DWORD							dwDateID;
};
//添加队员
struct DBR_GR_AddTeamer
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//操作ID
	DWORD							dwCaptainID;						//队长ID
	//DWORD							dwMemberID;							//成员ID
	BYTE							cbMode;								//0移除，1添加
	INT								nCount;								//数量，后接成员ID
};

//查询小队日记录
struct DBR_GR_QueryTeamDayRecord
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwQueryUserID;						//查询玩家ID
	DWORD							dwDateID;							//日期ID
};

//查询小队成员日记录
struct DBR_GR_QueryTeamerDayRecord
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwQueryUserID;						//查询玩家ID
	DWORD							dwCaptainID;						//队长ID
	DWORD							dwDateID;							//日期ID
};

//隐私设置
struct DBR_GR_SetPrivacy
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbShowGroupID;						//0隐藏ID，1显示ID
};

//设置队长点位
struct DBR_GR_SetCaptainPoint
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwCaptainID;						//队长ID
	WORD							wPoint;								//点位
};

//打开积分保险箱
struct DBR_GR_OpenBox
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

//提取积分
struct DBR_GR_GetAwardScore
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	SCORE							lScore;								//提取分数
};

//查询邀请码
struct DBR_GR_QueryCode
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
};

//绑定邀请码
struct DBR_GR_BlindCode
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	LONGLONG						lCode;								//推广码
};

//查询可用分
struct DBR_GR_QueryScore
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
};

//检查新玩家
struct DBR_GR_CheckNewUser
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szClientIP[15];						//玩家ip
};

//更新在线状态
struct DBR_GR_UpdateOnLineStatus
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	byte							cbStatus;							//0 离开 1 进入
};

//修改信息
struct DBR_GR_Modify_Info
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbFlag[3];							//修改标签，1有效
	TCHAR							szQQ[LEN_NICKNAME];					//用户QQ
	TCHAR							szWX[LEN_NICKNAME];					//用户WX
	TCHAR							szPhone[12];						//用户手机号
};
//////////////////////////////////////////////////////////////////////////

//输出信息

//系统命令
#define DBO_GR_GROUP_OPTION			100									//群组配置
#define DBO_GR_FIRBID_WORDS			101									//禁止词汇

//群组命令
#define DBO_GR_LOGON_SUCCESS		110									//登录成功
#define DBO_GR_LOGON_FAILURE		111									//登录失败
#define DBO_GR_CREATE_GROUP			112									//创建群组
#define DBO_GR_UPDATE_GROUP			113									//更新群组
#define DBO_GR_DELETE_GROUP			114									//删除群组
#define DBO_GR_INSERT_MEMBER		115									//添加成员
#define DBO_GR_DELETE_MEMBER		116									//删除成员
#define DBO_GR_BATTLE_RESULT		117									//查询约战
#define DBO_GR_SETTLE_RESULT		118									//结算结果
#define DBO_GR_TOTAL_RESULT			119									//总数结果
//申请命令
#define DBO_GR_WEALTH_UPDATE		120									//财富更新
#define DBO_GR_APPLY_MESSAGE		121									//申请消息
#define DBO_GR_APPLY_DEL_RESULT		122									//删除结果
#define DBO_GR_APPLY_MESSAGE_EX		123									//申请消息

#define DBO_GR_MODIFY_RIGHT			130									//修改权限	
#define DBO_GR_PRESENT_SUCCESS		131									//赠送结果
#define DBO_GR_QUERY_PRESENT_LIST	132									//赠送记录列表
#define DBO_GR_QUERY_REVENUE_LIST	133									//扣费记录列表
#define DBO_GR_APPEND_CONFIG		134									//创建玩法
#define DBO_GR_MODIFY_CONFIG		135									//修改玩法
#define DBO_GR_DELETE_CONFIG		136									//删除玩法
#define DBO_GR_DAY_RECORD_LIST		137									//日记录
#define DBO_GR_QUERY_CREATE_RESULT	138									//空桌查询
#define DBO_GR_MEDAL_MODE			139									//勋章模式
#define DBO_GR_GROUP_STATUS			140									//群组状态
#define DBO_GR_INOUT_MODE			141									//进出设置
#define DBO_GR_MANAGER_RIGHT		142									//管理员权限
#define DBO_GR_FREE_TABLE_LIMIT		143									//空桌设置
#define DBO_GR_BIGWINNER_SHOW		144									//大赢家显示设置
#define DBO_GR_SET_MANAGER_RIGHT	145									//设置管理员权限
#define DBO_GR_USER_DAY_DETAIL_LIST	146									//玩家日详情
#define DBO_GR_GROUP_DAY_DETAIL_LIST 147								//大联盟日详情
#define DBO_GR_SET_FLAG				148									//点赞设置
#define DBO_GR_CLEAR_MEDAL			149									//清零勋章
#define DBO_GR_FAVORITE_CONFIG		150									//收藏玩法
#define DBO_GR_ROLL_MSG				151									//跑马灯
#define DBO_GR_GROUP_NAME			152									//大联盟名称
#define DBO_GR_ROOM_DETAIL			153									//房间详情
#define DBO_GR_CAPTAIN_LIST			154									//队长列表
#define DBO_GR_ADD_TEAMER			155									//调配队员结果
#define DBO_GR_TEAM_DAY_RECORD_LIST	156									//小队日记录
#define DBO_GR_TEAMER_DAY_LIST		157									//小队成员日记录
#define DBO_GR_GROUP_PRIVACY		158									//隐私设置
#define DBO_GR_CAPTAIN_POINT		159									//点位设置成功
#define DBO_GR_BOX_INFO				160									//保险箱信息
#define DBO_GR_GET_AWARDSCORE_RES	161									//奖励分提取结果
#define DBO_GR_INVITATION_CODE		162									//邀请码信息
#define DBO_GR_BLIND_CODE_RES		163									//绑定结果
#define DBO_GR_QUERY_SCORE_RES		164									//查询分数结果
#define DBO_GR_MODIFY_INFO			165									//修改信息
//操作失败
#define DBO_GR_OPERATE_SUCCESS		200									//操作成功
#define DBO_GR_OPERATE_FAILURE		201									//操作失败	
//////////////////////////////////////////////////////////////////////////////////
//登录成功
struct DBO_GR_LogonSuccess
{
	//基本属性	
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D	
	BYTE							cbGender;							//用户性别
	DWORD							dwStationID;						//站点标识
	TCHAR							szNickName[LEN_ACCOUNTS];			//玩家昵称
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//自定头像
	DWORD							dwCustomID;							//自定标识

	//网络信息
	tagIMSocketInfo					SocketInfo;							//网络信息

	//群组列表
	WORD							wGroupCount;						//群组数量	
};

//登录失败
struct DBO_GR_LogonFailure
{
	//用户信息	
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//创建群组
struct DBO_GR_CreateGroup
{
	tagIMGroupItem					GroupItem;							//群组属性
	tagIMGroupMember				GroupMember;						//群组成员
};

//更新群组
struct DBO_GR_UpdateGroup
{
	//群组信息
	DWORD							dwGroupID;							//群组标识	
	BYTE							cbUpdateKind;						//更新类型
};

//删除群组
struct DBO_GR_DeleteGroup
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwGroupID;							//群组标识
};

//群组状态
struct DBO_GR_GroupStatus
{
	DWORD							dwGroupID;							//群组标识
	BYTE							cbStatus;							//群组状态
};

//进出设置
struct DBO_GR_InOutMode
{
	DWORD							dwGroupID;							//群组标识
	BYTE							cbInMode;							//进入设置0不需审核，1需审核
	BYTE							cbInType;							//进入方式0邀请码加入（隐藏ID），1大联盟ID加入
	BYTE							cbOutMode;							//退出设置0不需审核，1需审核
};

//设置跑马灯
struct DBO_GR_RollMsg
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbFlag;								//0关闭，1开启
	TCHAR							szRollMsg[33];						//消息内容最多32个字
	INT								nTime;								//滚动间隔
	BYTE							cbType;								//1亲友圈消息，2队长消息
};

//大联盟名称
struct DBO_GR_GroupName
{
	DWORD							dwGroupID;							//群组标识
	TCHAR							szGroupName[GROUP_LEN_NAME];		//名称
};

//勋章设置结果
struct DBO_GR_SetMedalResult
{
	DWORD							dwGroupID;							//群组标识
	BYTE							cbMedalMode;						//0关闭，1开启
	BYTE							cbMedalShow;						//0不显示，1显示
	BYTE							cbCurMedalShow;						//0所有人可见，1管理员可见
	BYTE							cbMedalRevenueShow;					//0所有人可见，1管理员可见
	BYTE							cbMedalClear;						//0不清零，1零点清零
};

//空桌设置结果
struct DBO_GR_FreeTableLimit
{
	DWORD							dwGroupID;							//群组标识
	BYTE							cbCount;							//数量
};


//大赢家设置结果
struct DBO_GR_BigWinnerShow
{
	DWORD							dwGroupID;							//群组标识
	BYTE							cbShowMode;							//0所有可见，1管理员可见
};

//添加成员
struct DBO_GR_InsertMember
{
	DWORD							dwGroupID;							//群组标识
	tagIMGroupMemberUser			MemberUser;							//成员用户
};

//删除成员
struct DBO_GR_DeleteMember
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//操作玩家
	BYTE							cbType;								//0申请退出，1被删除
	INT								nCount;								//数量，后接tagIMDeleteMember
};

//财富更新
struct DBO_GR_WealthUpdate
{
	DWORD							dwUserID;							//用户标识
	LONGLONG						lUserIngot;							//用户钻石
	LONGLONG						lUserRoomCard;						//用户房卡
	LONGLONG						lUserMedal;							//用户勋章
};

//申请消息
struct DBO_GR_ApplyMessage
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbMessageType;						//消息类型
	WORD							wMessageCount;						//消息数量	
};

//删除结果
struct DBO_GR_ApplyDelResult
{
	DWORD							dwCreaterID;						//群主I D
	DWORD							dwUserID;							//用户标识
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型
	bool							bDelResult;							//删除结果
	TCHAR							szErrorDescribe[128];				//错误消息
};

//操作成功
struct DBO_GR_OperateSuccess
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	LONG							lOperateCode;						//操作代码
	TCHAR							szOperateNotice[128];				//操作提示
};

//操作失败
struct DBO_GR_OperateFailure
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//添加修改权限
struct DBO_GR_ModifyRight
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwGroupID;							//群组标识
	INT								nRight;								//修改权限
	BYTE							cbMode;								//0增加权限，1删除权限

	DWORD							dwMemberID;							//成员标识
	INT								nMemberRight;						//修改后权限


};

//移除队长
struct DBO_GR_CancelCaptain
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//成员标识
	INT								nMemberRight;						//修改后权限
};

//管理员权限
struct DBO_GR_ManagerRight
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwTargetID;							//目标用户ID
	LONGLONG						lManagerRight;
	LONGLONG						lCaptainRight;
};

//上下分结果
struct DBO_GR_PresentSuccess
{
	//用户信息
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwPresentID;						//赠送玩家ID
	BYTE							cbType;								//类型0发放，1赠送
	INT								nCount;								//成功数量
};

//赠送记录列表
struct DBO_GR_Present_List
{
	DWORD							dwGroupID;							//大联盟ID
	INT								nCount;								//记录数量
	bool							bListEnd;							//结束
};

//扣费记录列表
struct DBO_GR_QueryRevenue_List
{
	DWORD							dwGroupID;							//大联盟ID
	INT								nCount;								//记录数量
	bool							bListEnd;							//结束
};

//日记录列表
struct DBO_GR_QueryDayRecord_List
{
	DWORD							dwGroupID;							//大联盟ID
	INT								nCount;								//记录数量
	bool							bListEnd;							//结束
};

//玩家日详情列表
struct DBO_GR_QueryUserDayDetail_List
{
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwUserID;							//玩家ID
	DWORD							dwGameID;							//玩家GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			//玩家昵称
	INT								nPlayCount;							//对局次数
	INT								nBigWinnerCount;					//大赢家次数
	SCORE							lCurScore;							//当前分数
	SCORE							lGameScore;							//输赢分数
	INT								nCount;								//后接数据条数
	bool							bListEnd;							//结束
};

//大联盟日详情列表
struct DBO_GR_QueryGroupDayDetail_List
{
	DWORD							dwGroupID;							//大联盟ID
	INT								nCount;								//记录数量
	INT								nDiamond;							//总消耗钻石
	LONGLONG						lAllRoomRevenue;					//总贡献
	bool							bListEnd;							//结束
};

//创建玩法
struct DBO_GR_CreateConfig
{
	DWORD							dwGroupID;
	SCORE							lDiamond;
	tagGroupConfigInfo				ConfigInfo;
};

//修改玩法
struct DBO_GR_ModifyConfig
{
	DWORD							dwGroupID;
	tagGroupConfigInfo				ConfigInfo;
};

//删除玩法
struct DBO_GR_DeleteConfig
{
	DWORD							dwGroupID;
	DWORD							dwConfigID;
};

//查询空桌限制
struct DBO_GR_QueryCreateResult
{
	DWORD							dwGroupID;
	DWORD							dwConfigID;
	DWORD							dwUserID;
	INT								nMaxNullTableCount;
};

//查询勋章模式
struct DBO_GR_MedalMode
{
	DWORD							dwGroupID;
	BYTE							cbMedalMode;
};

//房间详情
struct DBO_GR_QueryRoomDetail_List
{
	DWORD							dwRoomID;							//房间ID
	TCHAR							szKindName[LEN_ACCOUNTS];			//游戏名称
	SCORE							lMinTableScore;						//最低入场分
	INT								nDrawCount;							//总局数
	BYTE							cbPlayerCount;						//玩家人数
	TCHAR							szNickName[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT][LEN_NICKNAME];		//玩家昵称
	SCORE							lAllScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];						//以下列表总分
	INT								nListCount;
	bool							bListEnd;							//结束
};

//队长列表
struct DBO_GR_Captain_List
{
	DWORD							dwGroupID;							//大联盟ID
	INT								nCaptainCount;						//下属队长数量
	INT								nPlayCount;							//总局数
	INT								nBigWinnerCount;					//大赢家总数
	SCORE							lRevenueCount;						//产生税收累计
	SCORE							lContributionCount;					//贡献累计
	INT								nCount;								//数量
	bool							bListEnd;							//结束
};

//调配队员
struct DBO_GR_Add_Teamer
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//操作ID
	DWORD							dwCaptainID;						//队长ID
	//DWORD							dwMemberID;							//成员ID
	BYTE							cbMode;								//0移除，1添加
	INT								nCount;								//数量，后接tagIMAddTeamer
};

//调配结果
struct tagIMAddTeamer
{
	DWORD							dwUserID;							//用户ID
	BYTE							cbSuccess;							//0失败，1成功
};

//小队日统计列表
struct DBO_GR_TeamDayRecord_List
{
	DWORD							dwGroupID;							//大联盟ID
	INT								nCount;								//记录数量
	bool							bListEnd;							//结束
};

//小队成员日统计列表
struct DBO_GR_TeamerDayRecord_List
{
	DWORD							dwGroupID;							//大联盟ID
	INT								nCount;								//记录数量
	bool							bListEnd;							//结束
};

//隐私设置结果
struct DBO_GR_Privacy
{
	DWORD							dwGroupID;							//大联盟ID
	BYTE							cbShowGroupID;						//0隐藏ID,1显示ID
};

//点位设置结果
struct DBO_GR_CaptainPoint
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwCaptainID;						//队长ID
	WORD							wPoint;								//点位
};

//打开保险结果
struct DBO_GR_BoxInfo
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//玩家ID
	WORD							wPoint;								//点位
	SCORE							lAwardScore;						//奖励分
	SCORE							lTodayGetScore;						//今日已提取分
	INT								nCount;								//提取记录数量
	bool							bListEnd;
};

//提取积分结果
struct DBO_GR_GetAwardScoreResult
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//玩家ID
	WORD							wAwardPoint;						//点位
	SCORE							lCurAwardScore;						//当前奖励分
	SCORE							lGetScore;							//提取分
};

//邀请码信息
struct DBO_GR_InvitationCode
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//玩家ID
	LONGLONG						lInvitationCode;					//邀请码
};

//绑定成功
struct DBO_GR_BlindCodeRes
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//玩家ID
	DWORD							dwCaptainID;						//队长ID
};

//查询分数结果
struct DBO_GR_QueryScoreResult
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//玩家ID
	LONGLONG						lScore;								//可用分
};

//修改资料结果
struct DBO_GR_ModifyInfo
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwGroupID;							//群组标识
	INT								nMemberRight;						//玩家权限

	BYTE							cbFlag[3];							//修改标签，1有效
	TCHAR							szQQ[LEN_NICKNAME];					//用户QQ
	TCHAR							szWX[LEN_NICKNAME];					//用户WX
	TCHAR							szPhone[12];						//用户手机号

};
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//请求数据包

//系统命令
#define DBR_GR_LOAD_GROUP_LIST		301									//加载群组
#define DBR_GR_LOAD_GROUP_ITEM		302									//查找群组
#define DBR_GR_LOAD_GROUP_MEMBER	303									//群组成员
#define DBR_GR_LOAD_GROUP_CONFIG	304									//群组玩法
#define DBR_GR_LOAD_SINGLE_GROUP	305									//加载单个俱乐部参数

//成员命令
#define DBR_GR_UPDATE_MEMBER		310									//更新成员

//////////////////////////////////////////////////////////////////////////
//输出信息

//群组列表
#define DBO_GR_GROUP_LIST			400									//群组属性
#define DBO_GR_GROUP_LIST_FINISH	401									//列表完成

//群组子项
#define DBO_GR_GROUP_ITEM			410									//群组属性
#define DBO_GR_GROUP_ITEM_FINISH	411									//子项完成	

//群组成员
#define DBO_GR_GROUP_MEMBER			420									//群组成员
#define DBO_GR_UPDATE_MEMBER		421									//更新成员

//群组玩法
#define DBO_GR_GROUP_CONFIG			422									//群组玩法
//群组公共配置
#define DBO_GR_COMMON_INFO			423									//公共配置
//////////////////////////////////////////////////////////////////////////////////
//群组信息

//加载群组列表
struct DBR_GR_LoadGroupList
{
	DWORD							dwGroupStartID;						//群组标识
	DWORD							dwGroupEndID;						//群组标识
};

//加载群组信息
struct DBR_GR_LoadGroupItem
{
	DWORD							dwGroupID;							//群组标识
};

//群组成员
struct DBR_GR_LoadGroupMember
{
	DWORD							dwGroupID;							//群组标识
};

//群组成员
struct DBR_GR_LoadSingleGroup
{
	DWORD							dwGroupID;							//群组标识
};



//群组玩法
struct DBR_GR_LoadGroupConfig
{
	DWORD							dwGroupID;							//群组标识
};

//更新成员
struct DBR_GR_UpdateMember
{
	//成员信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//成员标识

	//更新类型							
	BYTE							cbUpdateKind;						//更新类型
	WORD							wIncBattleCount;					//开房次数
};

//////////////////////////////////////////////////////////////////////////////////
//群组成员
struct DBO_GR_GroupMember
{
	DWORD							dwGroupID;							//群组标识
};

//更新成员
struct DBO_GR_UpdateMember
{
	DWORD							dwGroupID;							//群组标识
	tagIMGroupMember				GroupMember;						//群组成员
};

//群组玩法
struct DBO_GR_GroupConfig
{
	DWORD							dwGroupID;							//群组标识
};

//赠送成功
struct SUB_GR_PresentSuccess
{
	//用户信息
	DWORD							dwGroupID;							//大联盟ID
	DWORD							dwPresentID;						//赠送玩家ID
	DWORD							dwReceiveID;						//用户标识
	SCORE							lScore;								//勋章
	BYTE							cbType;								//类型0发放，1赠送
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()
#endif