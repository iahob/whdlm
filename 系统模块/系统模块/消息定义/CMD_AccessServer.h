#ifndef CMD_ACCESS_HEAD_FILE
#define CMD_ACCESS_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//成员身份定义
#define GP_MEMBER					0x00								//普通成员
#define GP_CREATOR					0x01								//圈主
#define GP_MANAGER					0x02								//管理员
#define GP_CAPTAIN					0x04								//队长
#define GP_TEAMER					0x08								//队员

//管理员权限定义
#define MR_1						0x01									//冻结亲友圈
#define MR_2						0x02									//进出设置
#define MR_3						0x04									//设置公告
#define MR_4						0x08									//设置跑马灯
#define MR_5						0x10									//用户设置
#define MR_6						0x20									//勋章模式设置
#define MR_7						0x40									//玩法管理
#define MR_8						0x80									//点赞标记
#define MR_9						0x100									//队长权限
#define MR_10						0x200									//添加队长
#define MR_11						0x400									//解除队长
#define MR_12						0x800									//解除队员
#define MR_13						0x1000									//调配队员
#define MR_14						0x2000									//大赢家设置
//队长权限定义
#define CR_1						0x01									//勋章开关
#define CR_2						0x02									//勋章显示
#define CR_3						0x04									//勋章修改
#define CR_4						0x08									//点赞标记
#define CR_5						0x10									//解除队员
#define CR_6						0x20									//添加队员
#define CR_7						0x40									//邀请队员
#define CR_8						0x80									//发送消息

//约战服务
#define MDM_GA_BATTLE_SERVICE		1									//约战服务

//桌子操作
#define SUB_GA_QUERY_TABLE			110									//查询桌子
#define SUB_GA_CREATE_TABLE			111									//创建桌子
#define SUB_GA_DISMISS_TABLE		112									//解散桌子
#define SUB_GA_GET_TABLE_USERLIST	113									//获取列表
#define SUB_GA_SEARCH_SERVER_TABLE	114									//桌号查询
#define SUB_GA_QUERY_GAME_SERVER	115									//获取房间列表
//操作结果
#define SUB_GA_TABLE_ITEM			200									//桌子信息
#define SUB_GA_TABLE_LIST			201									//桌子列表
#define SUB_GA_OPERATE_FAILED		202									//操作失败
#define SUB_GA_BATTLE_RECORD		203									//约战记录
#define SUB_GA_TABLE_PARAMLIST		204									//桌子参数
#define SUB_GA_TABLE_USERLIST		205									//用户列表
#define SUB_GA_DISMISS_RESULT		206									//解散结果
#define SUB_GA_TABLE_LIST_FINISH	207									//列表完成

//消息推送
#define SUB_GA_USER_TABLE_STATUS	300									//用户坐下
#define SUB_GA_TABLE_START			301									//桌子开始
#define SUB_GA_TABLE_PARAM			302									//桌子参数
#define SUB_GA_TABLE_CREATE			303									//桌子创建
#define SUB_GA_TABLE_DISMISS		304									//桌子解散
#define SUB_GA_TABLE_CHANGEBANKER	305									//切换庄家

//////////////////////////////////////////////////////////////////////////////////
//查询服务
struct CMD_GA_QueryGameServer
{
	DWORD							dwGroupID;							//亲友圈标识
	WORD							wQueryBeginIndex;					//查询起始
	WORD							wQueryCount;						//
};
//查询桌子
struct CMD_GA_QueryTable
{
	WORD							wKindID;							//类型标识
	DWORD							dwGroupID;							//亲友圈标识
};

//创建桌子
struct CMD_GA_CreateTable
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwStationID;						//站点标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//群组信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwGroupCreaterID;					//群主标识

	//创建信息
	WORD							wKindID;							//类型标识	
	WORD							wPayType;							//支付类型
	WORD							wPlayerCount;						//游戏人数
	BYTE							cbSettleKind;						//结算方式
	BYTE							cbVideoMode;						//视频模式
	BYTE							cbCurrencyKind;						//货币类型
	BYTE							cbForbidSameIP;						//禁止同IP	
	WORD							wPlayCount;							//游戏局数
	DWORD							dwPlayTime;							//游戏时长
	LONGLONG						lBaseScore;							//游戏底分

	//客户信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//解散桌子
struct CMD_GA_DismissTable
{
	DWORD							dwGroupID;							//群组标识

	WORD							wKindID;							//类型标识
	DWORD							dwPersonalRoomID;					//桌子编号
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码	
};

//获取列表
struct CMD_GA_GetTableUserList
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwPersonalRoomID;					//房间号
};

//桌子列表
struct CMD_GA_TableList
{
	DWORD							dwGroupID;							//群组标识
	WORD							wTableCount;						//桌子数目
};

//操作失败
struct CMD_GA_OperateFailed
{
	WORD							wErrorCode;							//失败编码
	TCHAR							szErrorMessage[128];				//错误信息
};

//用户坐下
struct CMD_GA_UserTableStatus
{
	DWORD                           dwKindID;							//类型ID
	DWORD							dwServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	DWORD							dwPersonalRoomID;					//房间号
	DWORD							dwGroupID;							//亲友圈ID
	DWORD							dwUserID;							//用户标识
	WORD							wChairID;							//用户椅子
	bool							bSit;								//坐下起立
};

//桌子参数
struct CMD_GA_TableParam
{
	DWORD							dwMappedNum;						//映射编号
	DWORD							dwStartTime;						//开始时间
	WORD							wFinishCount;						//完成局数
};

//桌子解散
struct CMD_GA_TableDismiss
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMappedNum;						//映射编号
};

//解散结果
struct CMD_GA_DismissResult
{
	BYTE							cbResultCode;						//结果代码
	TCHAR							szDescribeString[256];				//描述信息
};

//用户列表
struct CMD_GA_TableUserList
{
	WORD							wUserCount;							//用户数量
	DWORD							dwUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];	//用户I D
};

//桌子开始
struct CMD_GA_TableStart
{
	DWORD                           dwKindID;							//类型ID
	DWORD							dwServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	DWORD							dwGroupID;							//亲友圈ID
	DWORD							dwPersonalRoomID;					//房间号
	bool							bStart;								//是否开始
};

//切换庄家
struct CMD_GA_TableChangeBanker
{
	DWORD                           dwKindID;							//类型ID
	DWORD							dwServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	DWORD							dwGroupID;							//亲友圈ID
	DWORD							dwPersonalRoomID;					//房间号
	DWORD							dwBankerUserID;						//百人游戏庄家USERID(无效为0)
};

//////////////////////////////////////////////////////////////////////////////////

//逻辑服务
#define MDM_GA_LOGIC_SERVICE		2									//逻辑服务

//请求命令
#define SUB_GA_LOGON_SERVER			1									//登录服务
#define SUB_GA_SEARCH_GROUP			2									//搜索群组
#define SUB_GA_CREATE_GROUP			3									//创建群组
#define SUB_GA_UPDATE_GROUP			4									//更新群组
#define SUB_GA_DELETE_GROUP			5									//删除群组
#define SUB_GA_DELETE_MEMBER		7									//删除成员		
#define SUB_GA_APPLY_REQUEST		8									//申请请求
#define SUB_GA_APPLY_RESPOND		9									//申请响应
#define SUB_GA_APPLY_DELETE			10									//申请删除
#define SUB_GA_APPLY_REPEAL			11									//申请撤销
#define SUB_GA_SETTLE_BATTLE		12									//约战结算
#define SUB_GA_QUERY_BATTLE			13									//查询约战
#define SUB_GA_QUERY_TOTAL_BATTLE	14									//约战总数	

#define SUB_GA_PRESENT_SCORE		15									//赠送勋章
#define SUB_GA_QUERY_PRESENT		16									//查询赠送
#define SUB_GA_QUERY_REVENUE		17									//查询扣费
#define SUB_GA_APPEND_CONFIG		18									//添加玩法
#define SUB_GA_MODIFY_CONFIG		19									//修改玩法
#define SUB_GA_DELETE_CONFIG		20									//删除玩法
#define SUB_GA_MODIFY_RIGHT			21									//修改权限
#define SUB_GA_QUERY_CREATE			22									//查询创建
#define SUB_GA_QUERY_MEDALMODE		23									//查询勋章模式
#define SUB_GA_QUERY_DAYRECORD		24									//查询日统计
#define SUB_GA_SET_ROLLMSG			25									//设置跑马灯
#define SUB_GA_EDIT_NOTICE			26									//编辑公告
#define SUB_GA_SET_GROUP_STATUS		27									//设置亲友圈状态
#define SUB_GA_SET_INOUT			28									//进出设置
#define SUB_GA_QUERY_RIGHT			29									//查询权限
#define SUB_GA_RIGHT_MANAGEMENT		30									//权限管理
#define SUB_GA_SET_MEDAL			31									//勋章模式设置
#define SUB_GA_SET_FREE_TABLE		32									//空桌设置
#define SUB_GA_SET_BIGWINNER		33									//大赢家显示设置
#define SUB_GA_QUERY_USER_DETAIL	34									//查询玩家一天详情
#define SUB_GA_QUERY_GROUP_DETAIL	35									//查询亲友圈一天详情
#define SUB_GA_SET_FLAG				36									//设置结算标记
#define SUB_GA_DISMISS_CLUB_TABLE	37									//解散亲友圈桌子
#define SUB_GA_SET_FAVORITE_CONFIG	38									//设置收藏玩法
#define SUB_GA_MODIFY_GROUP_NAME	39									//修改亲友圈名称
#define SUB_GA_QUERY_ROOM_DETAIL	40									//查询房间对局详情
#define SUB_GA_QUERY_CAPTAIN_LIST	41									//查询队长列表
#define SUB_GA_ADD_CAPTAIN_TEAMER	42									//调配队员
#define SUB_GA_QUERY_TEAM_RECORD	43									//查询小队日统计
#define SUB_GA_QUERY_TEAMER_RECORD	44									//查询小队成员日统计
#define SUB_GA_SET_PRIVACY			45									//隐私设置
#define SUB_GA_SET_CAPTAIN_POINT	46									//设置队长点位
#define SUB_GA_OPEN_BOX				47									//打开积分保险箱
#define SUB_GA_GET_AWARDSCORE		48									//提取积分
#define SUB_GA_QUERY_INVITATION_CODE 49									//查询邀请码
#define SUB_GA_BLIND_INVITATION_CODE 50									//绑定推广码
#define SUB_GA_QUERY_SCORE			 51									//圈主/管理员/队长查询可用分数
#define SUB_GA_QUERY_ROOM_BATTLE_INFO	52								//查询桌子对局信息
#define SUB_GA_MODIFY_INFO			53									//修改信息
//应答操作
#define SUB_GA_GROUP_OPTION			100									//群组配置
#define SUB_GA_APPLY_MESSAGE		101									//申请消息
#define SUB_GA_LOGON_FAILURE		102									//登录失败
#define SUB_GA_SEARCH_RESULT		103									//搜索结果
#define SUB_GA_WEALTH_UPDATE		104									//财富更新
#define SUB_GA_APPLY_DEL_RESULT		105									//删除结果
#define SUB_GA_QUERY_BATTLE_RESULT	106									//约战结果
#define SUB_GA_SETTLE_RESULT		107									//结算结果
#define SUB_GA_TOTAL_RESULT			108									//结算结果

#define SUB_GR_PRESENT_SUCCESS		109									//赠送成功
#define SUB_GA_QUERY_PRESENT_LIST	110									//赠送列表
#define SUB_GA_QUERY_REVENUE_LIST	111									//扣费列表
#define SUB_GA_QUERY_CREATE_RESULT	112									//查询创建结果
#define SUB_GA_QUERY_DAYRECORD_LIST	113									//日记录列表
#define SUB_GA_USER_DAY_DETAIL_LIST	116									//玩家日详情列表
#define SUB_GA_GROUP_DAY_DETAIL_LIST 117								//亲友圈日详情列表
#define SUB_GA_FAVORITE_CONFIG		118									//收藏玩法
#define SUB_GA_ROOM_DETAIL_INFO		119									//亲友圈房间详情
#define SUB_GA_CAPTAIN_LIST			120									//队长列表
#define SUB_GA_TEAM_DAY_RECORD		121									//小队日统计
#define SUB_GA_TEAMER_DAY_RECORD	122									//小队成员日统计
#define SUB_GA_CAPTAIN_POINT		123									//队长点位
#define SUB_GA_BOX_INFO				124									//保险箱信息
#define SUB_GA_GETBOXSCORE_RESULT	125									//提取结果
#define SUB_GA_INVITATION_CODE		126									//邀请码信息
#define SUB_GA_BLIND_CODE_RES		127									//绑定成功
#define SUB_GA_QUERY_SCORE_RES		128									//查询分数结果
#define SUB_GA_QUERY_TABLE_RES		129									//查询对局信息结果
//操作结果
#define SUB_GA_OPERATE_SUCCESS		200									//操作成功
#define SUB_GA_OPERATE_FAILURE		201									//操作失败	

//系统消息
#define SUB_GA_SYSTEM_MESSAGE		300									//系统消息

//////////////////////////////////////////////////////////////////////////////////
//登录服务
struct SUB_GA_LogonServer
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//搜索群组
struct SUB_GA_SearchGroup
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwGroupID;							//群组标识
};

//创建群组
struct SUB_GA_CreateGroup
{
	//规则信息
	BYTE							cbPayType;							//支付类型	
	BYTE							cbCurrencyKind;						//货币类型	
	BYTE							cbBattleMask;						//约战掩码

	//用户信息
	DWORD							dwCreateID;							//群主标识
	DWORD							dwStationID;						//站点标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//群组信息
	TCHAR							szGroupName[LEN_GROUP_NAME];		//群组名称
	TCHAR							szGroupIntroduce[LEN_GROUP_INTRODUCE];//群组介绍
};

//更新群组
struct SUB_GA_UpdateGroup
{
	//群组信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwStationID;						//站点标识
	BYTE							cbUpdateKind;						//更新类型

	//用户信息
	DWORD							dwCreateID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//删除群组
struct SUB_GA_DeleteGroup
{
	//群组信息
	DWORD							dwGroupID;							//群组标识
	DWORD							dwStationID;						//站点标识

	//用户信息
	DWORD							dwCreateID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//设置群组状态
struct SUB_GA_SetGroupStatus
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwCreateID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	BYTE							cbStatus;							//状态值1生效，2删除，3冻结
};

//删除成员
struct SUB_GA_DeleteMember
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码	
	INT								nCount;								//删除个数后接DWORD
};

//申请请求
struct SUB_GA_ApplyRequest
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwApplyerID;						//用户标识		
	BYTE							cbApplyType;						//申请类型0加入，1退出
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	LONGLONG						lInvitationCode;					//邀请码，申请类型为0时有效
};

//申请响应
struct SUB_GA_ApplyRespond
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
struct SUB_GA_ApplyDelete
{
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//申请撤销
struct SUB_GA_ApplyRepeal
{
	DWORD							dwMessageID;						//消息标识	

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//约战结算
struct SUB_GA_SettleBattle
{	
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//成员标识
	DWORD							dwSettleCount;						//结算次数

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//查询约战
struct SUB_GA_QueryBattle
{
	DWORD							dwGroupID;							//群组标识

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//添加玩法
struct tagConfig
{
	DWORD							dwConfigID;								//玩法ID
	DWORD							dwCreateID;								//创建者ID
	DWORD							dwGroupID;								//亲友圈id
	WORD							wKindID;								//类型ID
	LONGLONG						lCellScore;							    //底分设置
	DWORD							dwDrawCountLimit;						//局数限制
	DWORD							dwDrawTimeLimit;						//时间限制
	WORD							wJoinGamePeopleCount;					//参与游戏的人数
	DWORD							dwRoomTax;								//单独一个私人房间的税率，千分比
	BYTE							cbPassword;								//密码设置 0:无密码 1:需要密码
	BYTE							cbPayMode;								//支付方式									
	BYTE							cbGameRule[RULE_LEN];					//游戏规则 第0位标识 是否设置规则 0 代表未设置 1 代表设置
	BYTE							cbGameMode;								//游戏模式
	LONGLONG						lEnterScore;							//进入房间的分数下限
	LONGLONG						lLeaveScore;							//离开房间分数下限
	BYTE							cbJoinMode;								//玩家参与方式	0 不允许陌生人参与	1 允许陌生人参与	
};

//添加玩法
struct SUB_GA_AppendConfig
{
	DWORD							dwCreateID;								//创建者ID
	DWORD							dwGroupID;								//亲友圈id
	TCHAR							szConfigName[13];						//玩法名称
	WORD							wKindID;								//类型ID
	BYTE							cbPlayMode;								//结算模式0积分，1勋章
	LONGLONG						lCellScore;							    //底分设置
	DWORD							dwDrawCountLimit;						//局数限制
	DWORD							dwDrawTimeLimit;						//时间限制
	WORD							wJoinGamePeopleCount;					//参与游戏的人数
	DWORD							dwRoomTax;
	BYTE							cbPassword;								//密码设置 0:无密码 1:需要密码
	BYTE							cbPayMode;								//支付方式									
	BYTE							cbGameRule[RULE_LEN];					//游戏规则 第0位标识 是否设置规则 0 代表未设置 1 代表设置
	BYTE							cbGameMode;								//游戏模式
	LONGLONG						lEnterScore;							//进入房间的分数下限
	LONGLONG						lLeaveScore;							//离开房间分数下限
	BYTE							cbJoinMode;
	BYTE							cbTaxMode;								//税收模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家
	DWORD							dwTaxCount;								//税收数值
	BYTE							cbLessScoreDismiss;						//分数不够解散
};

//修改玩法
struct SUB_GA_ModifyConfig
{
	DWORD							dwUserID;								//用户标识
	DWORD							dwGroupID;								//群组ID
	DWORD							dwConfigID;								//玩法ID
	TCHAR							szConfigName[13];						//玩法名称
	BYTE							cbPlayMode;								//结算模式0积分，1勋章
	LONGLONG						lCellScore;							    //底分设置
	DWORD							dwDrawCountLimit;						//局数限制
	DWORD							dwDrawTimeLimit;						//时间限制
	WORD							wJoinGamePeopleCount;					//参与游戏的人数
	BYTE							cbPassword;								//密码设置 0:无密码 1:需要密码
	BYTE							cbPayMode;								//支付方式									
	BYTE							cbGameRule[RULE_LEN];					//游戏规则 第0位标识 是否设置规则 0 代表未设置 1 代表设置
	BYTE							cbGameMode;								//游戏模式
	LONGLONG						lEnterScore;							//进入房间的分数下限
	LONGLONG						lLeaveScore;							//离开房间分数下限
	BYTE							cbTaxMode;								//服务模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家
	DWORD							dwTaxCount;								//服务比例数值
	BYTE							cbLessScoreDismiss;						//分数不够解散
};

//删除玩法
struct SUB_GA_DeleteConfig
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGroupID;							//群组ID
	DWORD							dwConfigID;							//玩法ID
};

//修改权限
struct SUB_GA_ModifyRight
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

//申请消息
struct SUB_GA_ApplyMessage
{
	BYTE							cbMessageType;						//消息类型
	WORD							wMessageCount;						//消息数量
};

//登录失败
struct SUB_GA_S_LogonFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//搜索结果
struct SUB_GA_S_SearchResult
{
	WORD							wGroupCount;						//群组数量
};

//用户财富
struct SUB_GA_WealthUpdate
{
	LONGLONG						lUserIngot;							//用户钻石
	LONGLONG						lUserRoomCard;						//用户房卡
};

//删除结果
struct SUB_GA_ApplyDelResult
{
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型
	bool							bDelResult;							//删除结果
	TCHAR							szErrorDescribe[128];				//错误消息
};

//撤销通知
struct SUB_GA_ApplyRepResult
{
	DWORD							dwMessageID;						//消息标识	
	bool							bRepResult;							//撤销结果
	TCHAR							szErrorDescribe[128];				//错误消息
};

//操作成功
struct SUB_GA_OperateSuccess
{
	LONG							lOperateCode;						//操作代码
	TCHAR							szOperateNotice[128];				//操作提示
};

//操作失败
struct SUB_GA_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//系统消息
struct SUB_GA_SystemMessage
{
	WORD							wType;								//消息类型		
	TCHAR							szString[128];						//消息内容
};

//约战结果
struct SUB_GA_BattleResult
{
	DWORD							dwCreateCount;						//创建数目
	LONGLONG						lConsumeIngot;						//消耗数目
	DWORD							dwRecordCount;						//记录数目
};
//赠送勋章
struct SUB_GA_PresentScore
{
	DWORD							dwGroupID;							//亲友圈ID
	DWORD							dwPresentUserID;					//赠送玩家ID
	LONGLONG						lPresentScore;						//赠送数目
	BYTE							cbType;								//0赠送，1消耗
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	INT								nCount;								//操作个数
};

//查询赠送记录
struct SUB_GA_QueryPresent
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwQueryUserID;						//用户标识
};

//请求创建
struct SUB_GA_QueryCreate
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwConfigID;							//玩法标识
	DWORD							dwUserID;							//用户标识
};

//查询单日统计
struct SUB_GA_QueryDayRecord
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwQueryUserID;						//用户标识
	DWORD							dwDateID;							//日期ID --20190715
};

//查询亲友圈日详情
struct SUB_GA_QueryGroupDetail
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwQueryUserID;						//查询玩家
	DWORD							dwDateID;							//日期ID --20190715
};

//查询玩家日详情
struct SUB_GA_QueryUserDayDetail
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwQueryUserID;						//查询玩家
	DWORD							dwTargetUserID;						//目标玩家
	DWORD							dwDateID;							//日期ID --20190715
};

//查询勋章模式
struct SUB_GA_QueryMedalMode
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
};

//设置跑马灯
struct SUB_GA_SetRollMsg
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbFlag;								//0关闭，1开启
	TCHAR							szRollMsg[33];						//消息内容最多32个字
};

//亲友圈公告
struct SUB_GA_SetNotice
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szNotice[109];						//消息内容最多108个字
};

//进出设置
struct SUB_GA_SetInOut
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbInMode;							//进入设置0不需审核，1需审核
	BYTE							cbInType;							//进入方式0邀请码加入（隐藏ID），1亲友圈ID加入
	BYTE							cbOutMode;							//退出设置0不需审核，1需审核
};

//权限管理
struct SUB_GA_RightManagement
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwTargetID;							//目标用户ID
	BYTE							cbManagerRight[30];					//管理员权限
	BYTE							cbCaptainRight[30];					//队长权限
};

//权限查询
struct SUB_GA_QueryRight
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
};

//勋章设置
struct SUB_GA_SetMedal
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
struct SUB_GA_SetFreeTable
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbFreeTableLimit;					//允许空闲数量
};

//大赢家设置
struct SUB_GA_SetBigWinner
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbShow;								//0不显示，1显示
};

//设置点赞标记
struct SUB_GA_SetFlag
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwOperateUserID;					//操作玩家
	DWORD							dwTargetUserID;						//目标玩家
	DWORD							dwDateID;							//日期ID --20190715
	BYTE							cbFlag;								//标记，0取消，1生效
};

//申请解散桌子
struct SUB_GA_ApplyDismissClubTable
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//操作玩家
	DWORD							dwRoomID;							//房间ID
};

//设置收藏玩法
struct SUB_GA_SetFavoriteConfig
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//操作玩家
	DWORD							dwConfigID;							//收藏玩法
};

//修改名称
struct SUB_GA_ModifyGroupName
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//操作玩家
	TCHAR							szGroupName[LEN_GROUP_NAME];		//群组名称
};

//查询房间对局详情
struct SUB_GA_QueryRoomDetail
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwRoomID;							//房间ID
	DWORD							dwDateID;							//日期标识
	DWORD							dwUserID;							//查询玩家
	SYSTEMTIME						EndTime;							//房间结束时间
};

//查询队长列表
struct SUB_GA_QueryCaptain
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//查询玩家
	DWORD							dwCaptainID;						//队长ID,为0时，查询亲友圈直属队长，不为0，则查询该队长的下属队长
	DWORD							dwDateID;							//日期ID
};

//添加队员
struct SUB_GA_AddTeamer
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//操作ID
	DWORD							dwCaptainID;						//队长ID
	//DWORD							dwMemberID;							//成员ID
	BYTE							cbMode;								//0移除，1添加
	INT								nCount;								//数量，后接成员ID
};

//查询小队单日统计
struct SUB_GA_QueryTeamDayRecord
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwQueryUserID;						//用户标识
	DWORD							dwDateID;							//日期ID --20190715
};

//查询小队成员单日统计
struct SUB_GA_QueryTeamerDayRecord
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwQueryUserID;						//用户标识
	DWORD							dwCaptainID;						//队长ID
	DWORD							dwDateID;							//日期ID --20190715
};

//隐私设置
struct SUB_GA_SetPrivacy
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbShowGroupID;						//0隐藏ID，1显示ID
};

//设置队长点位
struct SUB_GA_SetCaptainPoint
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwCaptainID;						//队长ID
	WORD							wPoint;								//点位
};

//打开积分保险箱
struct SUB_GA_OpenBox
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

//提取积分
struct SUB_GA_GetAwardScore
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	SCORE							lScore;								//提取分数
};

//查询邀请码
struct SUB_GA_QueryInvatationCode
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
};

//绑定邀请码
struct SUB_GA_BlindInvatationCode
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	LONGLONG						lInvitationCode;					//推广码
};

//查询可用分
struct SUB_GA_QueryScore
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
};

//查询桌子信息
struct SUB_GA_QueryTableBattleInfo
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwServerID;							//服务器ID
	DWORD							dwTableID;							//桌子ID
};

//查询桌子信息结果
struct SUB_GA_QueryTableBattleInfoResult
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwRoomID;							//房间ID
	DWORD							dwAllCount;							//总局数
	DWORD							dwPlayCount;						//当前局数
	DWORD							dwUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//玩家ID
	SCORE							lGameScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//游戏得分
};

//修改信息
struct SUB_GA_Modify_Info
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
	BYTE							cbFlag[3];							//修改标签，1有效
	TCHAR							szQQ[LEN_NICKNAME];					//用户QQ
	TCHAR							szWX[LEN_NICKNAME];					//用户WX
	TCHAR							szPhone[12];						//用户手机号
};
//////////////////////////////////////////////////////////////////////////////////

//群组服务
#define MDM_GA_GROUP_SERVICE		3									//群组服务

//请求命令
#define SUB_GA_ENTER_GROUP			1									//进入群组
#define SUB_GA_LEAVE_GROUP			2									//离开群组

//群组命令
#define SUB_GA_GROUP_ITEM			100									//群组信息
#define SUB_GA_GROUP_PROPERTY		101									//群组属性
#define SUB_GA_GROUP_MEMBER			102									//群组成员
#define SUB_GA_GROUP_UPDATE			103									//群组更新
#define SUB_GA_GROUP_DELETE			104									//群组移除
#define SUB_GA_MEMBER_INSERT		105									//添加成员
#define SUB_GA_MEMBER_DELETE		106									//删除成员
#define SUB_GA_MEMBER_UPDATE		107									//更新成员
#define SUB_GA_LOGON_PROPERTY		108									//群组属性
#define SUB_GA_LOGON_SUCCESS		109									//登录成功
#define SUB_GA_QUERY_PROPERTY		110									//群组查询
#define SUB_GA_QUERY_PROPERTY_END	111									//群组查询结束
#define SUB_AG_GROUP_EXTRA_INFO		112									//群组附加信息
//玩法
#define SUB_GA_GROUP_CONFIG			120									//群组玩法
#define SUB_GA_CONFIG_APPEND		121									//玩法添加
#define SUB_GA_CONFIG_MODIFY		122									//玩法修改
#define SUB_GA_CONFIG_DELETE		123									//玩法删除
#define SUB_GA_RIGHT_CHANGE			124									//权限改变
#define SUB_GA_ROLL_MSG				125									//跑马灯
#define SUB_GA_GROUP_NOTICE			126									//公告
#define SUB_GA_GROUP_STATUS			127									//群组状态
#define SUB_GA_INOUT_MODE			128									//进出设置
#define SUB_GA_FREE_TABLE_LIMIT		129									//空桌设置
#define SUB_GA_BIGWINNER_SHOW_MODE	130									//大赢家设置
#define SUB_GA_MANAGER_RIGHT		131									//管理员权限设置
#define SUB_GA_MEDAL_MODE			132									//勋章模式
#define SUB_GA_MEMBER_STATUS		133									//成员状态
#define SUB_GA_SET_FLAG_RESULT		134									//设置点赞标记
#define SUB_GA_SET_CLEAR_MEDAL		135									//勋章清零
#define SUB_GA_GROUP_NAME			136									//群组名称
#define SUB_GA_ADD_TEAMER			137									//调配队员
#define SUB_GA_GROUP_PRIVACY		138									//俱乐部隐私
#define SUB_GA_GROUP_COMMON_INFO	139									//公共配置
//进入结果
#define SUB_GA_ENTER_SUCCESS		200									//进入成功
#define SUB_GA_ENTER_FAILURE		201									//进入失败

//////////////////////////////////////////////////////////////////////////////////

//进入群组
struct SUB_GA_EnterGroup
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwGroupID;							//群组标识
};

//离开群组
struct SUB_GA_LeaveGroup
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwGroupID;							//群组标识
};
//查询群组
struct SUB_GA_QueryGroup
{
	DWORD							dwUserID;							//用户标识
	WORD							wGroupCount;						//群组数量
};
//群组属性
struct SUB_GA_GroupItem
{
	tagIMGroupItem					GroupItem;							//群组属性
};
//群组属性
struct SUB_GA_GroupProperty
{
	tagIMGroupProperty				GroupProperty;						//群组属性
};

//群组成员
struct SUB_GA_GroupMember
{
	DWORD							dwGroupID;							//群组标识
};

//群组更新
struct SUB_GA_GroupUpdate
{	
	DWORD							dwGroupID;							//群组标识
	BYTE							cbUpdateKind;						//更新类型
};

//群组删除
struct SUB_GA_GroupDelete
{
	DWORD							dwGroupID;							//群组标识
};

//添加成员
struct SUB_GA_MemberInsert
{
	DWORD							dwGroupID;							//群组标识
	WORD							wMemberCount;						//成员数量
	tagIMGroupMemberUser			MemberUser;							//成员用户	
};

//删除成员
struct SUB_GA_MemberDelete
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwMemberID;							//成员标识
	WORD							wMemberCount;						//成员数量
};

//更新成员
struct SUB_GA_MemberUpdate
{
	DWORD							dwGroupID;							//群组标识
	tagIMGroupMember				GroupMember;						//成员信息
};

//群组玩法
struct SUB_GA_GroupConfig
{
	DWORD							dwGroupID;							//群组标识
	INT								nCount;								//玩法个数
};

//玩法添加
struct SUB_GA_ConfigAppend
{
	DWORD							dwGroupID;							//群组标识
	tagGroupConfigInfo				ConfigInfo;							//玩法信息
};

//玩法修改
struct SUB_GA_ConfigModify
{
	DWORD							dwGroupID;							//群组标识
	tagGroupConfigInfo				ConfigInfo;							//玩法信息
};

//玩法删除
struct SUB_GA_ConfigDelete
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwConfigID;							//玩法标识
};

//权限改变
struct SUB_GA_RightChange
{
	DWORD							dwGroupID;							//类型标识
	DWORD							dwMemberID;							//成员标识
	INT								nMemberRight;						//成员权限
};

//进入失败
struct SUB_GA_EnterFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//查询创建结果
struct SUB_GA_QueryCreateResult
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwConfigID;							//玩法标识
	INT								nMaxNullTableCount;					//最大空桌数量
	bool							bCanCreate;							//结果
	DWORD							dwRoomID;							//如果不能创建，则返回一个RoomID
};

//跑马灯
struct SUB_GA_RollMsg
{
	DWORD							dwGroupID;							//群组标识
	BYTE							cbFlag;								//0开启，1关闭
	INT								nTime;								//播放间隔单位秒
	TCHAR							szRollMsg[33];						//内容
	BYTE							cbMsgType;							//1圈主消息，2队长消息,3自己为队长时的消息
};

//公告
struct SUB_GA_GroupNotice
{
	DWORD							dwGroupID;							//群组标识
	TCHAR							szNotice[109];						//内容
};

//管理员权限
struct SUB_GA_ManagerRight
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwTargetID;							//目标玩家ID
	LONGLONG						lManagerRight;						//管理员权限
	LONGLONG						lCaptainRight;						//队长权限
	//BYTE							cbManagerRight[30];					//管理员权限
	//BYTE							cbCaptainRight[30];					//队长权限
};

//成员状态
struct SUB_GA_MemberStatus
{
	DWORD							dwUserID;							//玩家标识
	BYTE							cbStatus;							//0离线，1在线

};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif