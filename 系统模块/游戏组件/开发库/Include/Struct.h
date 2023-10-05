#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE


#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//游戏列表

//游戏类型
struct tagGameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct tagGameKind
{
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wGameID;							//模块索引
	WORD							wRecommend;							//推荐游戏
	WORD							wGameFlag;							//游戏标志
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//在线人数
	DWORD							dwSetCount;							//在线人数
	DWORD							dwFullCount;						//满员人数
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//Lua游戏种类
struct tagLuaGameKind
{
	WORD							wKindID;							//游戏ID
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	WORD							wTypeID;							//挂接索引
	WORD							wSortID;							//排序索引
	TCHAR							szModuleName[LEN_PROCESS];			//进程名字
	DWORD							dwClientVersion;					//客户端版本号
	DWORD							dwResVersion;						//推荐游戏
	BYTE							cbNullity;							//禁用标志 0 不禁用 1 禁用
};

//H5游戏种类
struct tagH5GameKind
{
	WORD							wKindID;							//游戏ID
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	WORD							wTypeID;							//挂接索引
	WORD							wSortID;							//排序索引
	TCHAR							szModuleName[LEN_PROCESS];			//进程名字
	DWORD							dwClientVersion;					//客户端版本号
	DWORD							dwResVersion;						//推荐游戏
	BYTE							cbNullity;							//禁用标志 0 不禁用 1 禁用
};

//U3D游戏种类
struct tagU3DGameKind
{
	WORD							wKindID;							//游戏ID
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	WORD							wTypeID;							//挂接索引
	WORD							wSortID;							//排序索引
	TCHAR							szModuleName[LEN_PROCESS];			//进程名字
	DWORD							dwClientVersion;					//客户端版本号
	DWORD							dwResVersion;						//推荐游戏
	BYTE							cbNullity;							//禁用标志 0 不禁用 1 禁用
};

//游戏节点
struct tagGameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//定制类型
struct tagGamePage
{
	WORD							wPageID;							//页面索引
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_PAGE];			//显示名称
};

//游戏房间
struct tagGameServer
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wServerID;							//房间索引
	WORD                            wServerKind;                        //房间类型
	WORD							wServerType;						//房间类型
	WORD							wServerLevel;						//房间等级
	WORD							wServerPort;						//房间端口
	BYTE							cbPlayMode;							//0积分模式1金币模式
	SCORE							lCellScore;							//单元积分
	BYTE							cbEnterMember;						//进入会员
	SCORE							lEnterScore;						//进入积分
	DWORD							dwServerRule;						//房间规则
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//AI数
	DWORD							dwFullCount;						//满员人数
	TCHAR							szServerAddr[32];					//房间名称
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	//私人房添加
	DWORD							dwSurportType;						//支持类型
	WORD							wTableCount;						//桌子数目

	//配置属性
	DWORD							dwSetPlayerCount;					//设置人数
	TCHAR							szGameInfomation[LEN_SERVER];		//游戏说明

	//大联盟添加
	bool							bGroupPersonalMaxChair;				//大联盟百人游戏
};

//代理列表
struct tagAgentGameKind
{
	WORD							wKindID;							//类型索引
	WORD							wSortID;							//排序索引
};

//比赛轮数
struct tagMatchRounds
{
	BYTE							cbPlayCount;						//游戏局数
	WORD							wPromotion;							//晋级人数
//	BOOL							bResetFlag;							//复位or累计
};

//定时赛
struct tagLockTimeMatch
{
	//时间配置
	SYSTEMTIME						MatchStartTime;						//开始日期
	SYSTEMTIME						SignupStartTime;					//报名开始
	SYSTEMTIME						SignupEndTime;						//报名截止	

	//报名参数
	BYTE							cbMatchMode;						//开赛模式
	WORD							wMaxSignupUser;						//报名人数
	WORD							wStartUserCount;					//开赛人数	
	
	//分数信息
	LONGLONG						lInitalBase;						//初始底分

	//比赛分数
	LONGLONG						lMatchInitScore;					//初始积分
	tagMatchRounds					hMatchRounds[MAX_MATCH_ROUNDS];		//比赛轮数

	//局数信息
	BYTE							cbTotalRounds;						//总轮数
	BYTE							cbTotalPlayCount;					//总局数
};

//DWORD							wEndDelayTime;						//延迟时间
//SYSTEMTIME					MatchEndTime;						//结束日期
//LONGLONG						lMatchCullScore;					//淘汰积分
//比赛局数
//WORD							wMinPlayCount;						//最少局数
//WORD							wMaxPlayCount;						//最大局数
//复活配置
//BYTE							cbReviveEnabled;					//启用标识
//BYTE							cbSafeCardEnabled;					//启用标识
//BYTE							cbReviveTimes;						//复活次数
//LONGLONG						lReviveFee;							//复活费用
//LONGLONG						lSafeCardFee;						//保险费用

//即时赛
struct tagImmediateMatch
{
	//人数信息
	WORD							wStartUserCount;					//开赛人数	
	WORD							wAndroidUserCount;					//机器数量
	WORD							wAndroidDelaySignupTime;			//延迟报名

	//分数信息
	LONGLONG						lInitalBase;						//初始底分

	LONGLONG						lInitalScore;						//初始积分
	tagMatchRounds					hMatchRounds[MAX_MATCH_ROUNDS];		//比赛轮数

	//局数信息
	BYTE							cbTotalRounds;						//总轮数
	BYTE							cbTotalPlayCount;					//总局数

	
	//AI基本配置
	BYTE							cbInitAndroidMin;					//初始AI最小值		
	BYTE							cbInitAndroidMax;					//初始AI最大值
	BOOL							bAndroidDebug;						//AI调试
	BYTE							cbNo1Ratio1;						//高出第一名百分比
	BYTE							cbNo1Ratio2;						//高出第一名百分比
	BYTE							cbNo1Ratio3;						//高出第一名百分比
	
	//AI加入
	WORD							wAndroidFullTime;					//AI填满时间
	BYTE							cbTotalSection;						//总阶段数
	BYTE							cbSectionCount[MAX_MATCH_SECTION];	//每个阶段AI数
};	

//奖励信息
struct tagMatchRewardInfo
{
	WORD							wRankID;							//奖励名次
	SCORE							lRewardGold;						//奖励金币
	SCORE							lRewardIngot;						//奖励元宝	
	DWORD							dwRewardTicket;						//奖励奖券
};

//比赛信息
struct tagGameMatch
{
	//基本信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识	
	BYTE							cbMatchType;						//比赛类型
	TCHAR							szMatchName[32];					//比赛名称

	//报名信息
	BYTE							cbFeeType;							//费用类型
	BYTE							cbDeductArea;						//缴费区域
	LONGLONG						lSignupFee;							//报名费用	
	BYTE							cbSignupMode;						//报名方式
	BYTE							cbJoinCondition;					//参赛条件
	BYTE							cbMemberOrder;						//会员等级
	LONG							lExperience;						//玩家经验

	//排名方式
	BYTE							cbRankingMode;						//排名方式	
	WORD							wCountInnings;						//统计局数
	BYTE							cbFilterGradesMode;					//筛选方式
	
	//奖励信息
	WORD							wRewardCount;						//奖励人数
	tagMatchRewardInfo				MatchRewardInfo[3];					//比赛奖励

	//比赛规则
	BYTE							cbMatchRule[512];					//比赛规则
};

//报名数量
struct tagMatchSignUpCount
{
	LONGLONG						lMatchNo;							//比赛编号				
	DWORD							dwMatchCount;						//比赛人数
	WORD							wServerID;							//房间标识
};

//聊天信息
struct tagChatServer
{
	WORD							wChatID;							//广场标识
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
};

//俱乐部信息
struct tagClubServer
{
	WORD							wClubID;							//广场标识
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
};

//视频配置
struct tagAVServerOption
{
	WORD							wAVServerPort;						//视频端口
	DWORD							dwAVServerAddr;						//视频地址
};

//在线信息
struct tagOnLineInfoKind
{
	WORD							wKindID;							//类型标识
	DWORD							dwOnLineCount;						//在线人数
};

//在线统计
struct tagOnLineInfoKindEx
{
	WORD							wKindID;							//类型标识
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//AI数
	DWORD							dwSetCount;							//设置人数
};

//在线信息
struct tagOnLineInfoServer
{
	WORD							wServerID;							//房间标识
	DWORD							dwOnLineCount;						//在线人数
};

//奖项子项
struct tagLotteryItem
{
	BYTE							cbItemIndex;						//奖项索引
	BYTE							cbItemType;							//奖励类型
	SCORE							lItemQuota;							//奖励额度
};
//////////////////////////////////////////////////////////////////////////////////
//临时定义(GameAIParameter.h已有定义)
#define AI_CARRY_GRADE				4											//携带等级

//机器参数
struct tagAndroidParameter
{
	//属性变量	
	DWORD							dwAndroidCount;								//机器数目
	DWORD							dwSwitchMinInnings;							//换桌局数(默认不可配)
	DWORD							dwSwitchMaxInnings;							//换桌局数(默认不可配)
	DWORD							dwServiceMode;								//服务模式(默认不可配)
	DWORD							dwAIHangingMinRate;							//挂机比例
	DWORD							dwAIHangingMaxRate;							//挂机比例
	DWORD							dwAICarryScoreMinMultiple[AI_CARRY_GRADE];	//携带倍数
	DWORD							dwAICarryScoreMaxMultiple[AI_CARRY_GRADE];	//携带倍数
	DWORD							dwAIScoreLevelRate[AI_CARRY_GRADE];			//携带占比
	DWORD							dwAIScoreFloatMinRate[AI_CARRY_GRADE];		//浮动占比
	DWORD							dwAIScoreFloatMaxRate[AI_CARRY_GRADE];		//浮动占比
};

//////////////////////////////////////////////////////////////////////////////////
//用户信息

//桌子状态
struct tagTableStatus
{
	BYTE							cbTableLock;						//锁定标志
	BYTE							cbPlayStatus;						//游戏标志
	LONG							lCellScore;							//单元积分
};

//用户状态
struct tagUserStatus
{
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子位置
	BYTE							cbUserStatus;						//用户状态
};

//用户属性
struct tagUserAttrib
{
	BYTE							cbCompanion;						//用户关系
};

//用户积分
struct tagUserScore
{
	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行
	SCORE							lIngot;								//用户元宝
	DOUBLE							dBeans;								//用户游戏豆

	//输赢信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//全局信息
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
};

//用户积分
struct tagMobileUserScore
{
	//积分信息
	SCORE							lScore;								//用户分数
	DOUBLE							dBeans;								//用户游戏豆

	//输赢信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//全局信息
	DWORD							dwExperience;						//用户经验
};

//道具节点
struct tagPropertyTypeItem
{
	DWORD							dwTypeID;							//类型标识
	DWORD							dwSortID;							//排序标识
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//道具节点
struct tagPropertyRelatItem
{
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwTypeID;							//类型标识
};

//道具信息
struct tagPropertyItem
{
	//道具信息
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwPropertyKind;						//功能类型

	BYTE							cbUseArea;							//使用范围
	BYTE							cbServiceArea;						//对象范围
	BYTE							cbRecommend;						//推荐标识

	BYTE							cbExchangeType;						//兑换类型
	DWORD							dwExchangeRatio;					//钻石兑换道具比率

	//获得财富
	SCORE							lSendLoveLiness;					//获得魅力（赠送方）
	SCORE							lRecvLoveLiness;					//获得魅力（接收方）
	SCORE							lUseResultsGold;					//获得金币

	//持续效果
	DWORD							dwUseResultsValidTime;				//有效时间
	DWORD							dwUseResultsValidTimeScoreMultiple;	//有效倍率

	//礼物拆包
	DWORD							dwUseResultsGiftPackage;			//礼物拆包

	DWORD							dwSortID;							 //排序标识
	TCHAR							szName[PROPERTY_LEN_NAME];			 //道具名称
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//使用信息
	BYTE							cbPlatformKind;						 //平台编号0:全部 1:LUA,2:H5,3:U3D
};

//子道具信息
struct tagPropertySubItem
{
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwOwnerPropertyID;					//道具标识
	DWORD                           dwPropertyCount;                    //道具数目
	DWORD							dwSortID;							//排序标识
};

//道具礼包
struct tagGiftPropertyInfo
{
	WORD                            wPropertyCount;                     //道具数目
	WORD							wPropertyID;						//道具标识
};

//道具使用
struct tagUsePropertyInfo
{
	WORD                            wPropertyCount;                     //道具数目
	WORD                            dwValidNum;						    //有效数字
	DWORD                           dwEffectTime;                       //生效时间
};


//用户道具
struct tagUserProperty
{
	WORD                            wPropertyUseMark;                   //道具标示
	tagUsePropertyInfo              PropertyInfo[MAX_PT_MARK];			//使用信息   
};

//道具包裹
struct tagPropertyPackage
{
	WORD                            wTrumpetCount;                     //小喇叭数
	WORD                            wTyphonCount;                      //大喇叭数
};

//时间信息
struct tagTimeInfo
{
	DWORD						dwEnterTableTimer;						//进出桌子时间
	DWORD						dwLeaveTableTimer;						//离开桌子时间
	DWORD						dwStartGameTimer;						//开始游戏时间
	DWORD						dwEndGameTimer;							//离开游戏时间
};

//////////////////////////////////////////////////////////////////////////
//好友信息

struct tagNearUserInfo
{
	//用户属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	DWORD							dwFaceID;							//头像 I D
	DWORD							dwCustomID;							//头像 I D
	BYTE							cbGender;							//用户性别
	WORD							wMemberOrder;						//会员等级
	WORD							wGrowLevel;							//用户等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//经度纬度
	DOUBLE							dLongitude;							//坐标经度
	DOUBLE							dLatitude;							//坐标纬度
	DWORD							dwDistance;							//目标距离	
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//地名

	//用户地址
	DWORD							dwClientAddr;						//用户地址
};

//用户信息
struct tagFriendUserInfo
{
	//用户属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	DWORD							dwFaceID;							//头像 I D
	DWORD							dwCustomID;							//头像 I D
	BYTE							cbGender;							//用户性别
	WORD							wMemberOrder;						//会员等级
	WORD							wGrowLevel;							//用户等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实姓名

	//附属资料
	TCHAR							szQQ[LEN_QQ];						//用户QQ
	TCHAR							szEMail[LEN_EMAIL];					//用户Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//座机号码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
	TCHAR							szPostalCode[LEN_POSTALCODE];		//邮政编码

	//用户状态
	BYTE							cbMainStatus;						//用户状态
	BYTE							cbGameStatus;						//游戏状态
	WORD							wKindID;							//游戏标识
	WORD							wServerID;							//房间标识
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子位置
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	DWORD							dwClientAddr;						//用户地址

	//经度纬度
	BYTE							cbCoordinate;						//开启标识
	DOUBLE							dLongitude;							//坐标经度
	DOUBLE							dLatitude;							//坐标纬度
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//地名
};

//用户分组
struct tagClientGroupInfo
{
	BYTE							cbGroupIndex;						//分组索引
	TCHAR							szGroupName[GROUP_LEN_NAME];		//分组名称		
};

//好友信息
struct tagServerFriendInfo
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbMainStatus;						//用户状态
	BYTE							cbGameStatus;						//用户状态
	BYTE							cbGroupID;							//组别标识	
};

//好友信息
struct tagClientFriendInfo
{
	//用户属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	DWORD							dwFaceID;							//头像 I D
	DWORD							dwCustomID;							//头像 I D
	BYTE							cbGender;							//用户性别
	WORD							wMemberOrder;						//用户等级
	WORD							wGrowLevel;							//用户等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实姓名
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	BYTE							cbGroupID;							//组别标识	

	//在线信息
	BYTE							cbMainStatus;						//用户状态
	BYTE							cbGameStatus;						//游戏状态
	WORD							wKindID;							//游戏标识
	WORD							wServerID;							//房间标识
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子位置
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//用户资料
struct tagUserIndividual
{
	DWORD							dwUserID;							//用户标识
	//附属资料
	TCHAR							szQQ[LEN_QQ];						//用户QQ
	TCHAR							szEMail[LEN_EMAIL];					//用户Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//座机号码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码	
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
	TCHAR							szPostalCode[LEN_POSTALCODE];		//邮政编码
};

//备注信息
struct tagUserRemarksInfo
{
	DWORD							dwFriendUserID;						//好友标识
	TCHAR							szRemarksInfo[LEN_REMARKS];			//备注信息
};

//财富信息
struct tagUserWealthInfo
{
	DWORD							dwUserID;							//用户标识
	LONGLONG						lUserGold;							//用户金币	
};

//////////////////////////////////////////////////////////////////////////

//用户信息
struct tagUserInfo
{
	//基本属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwGroupID;							//社团 I D
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szQQ[LEN_QQ];						//QQ号码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wLastTableID;					    //游戏桌子
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行
	SCORE							lIngot;								//用户元宝
	DOUBLE							dBeans;								//游戏豆
	//私人房添加
	SCORE							lDiamond;							//钻石数量
	SCORE							lHistoryKindIDWin;					//该游戏历史总输赢

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数	
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//代理信息
	DWORD							dwAgentID;							//代理 I D

	//时间信息
	tagTimeInfo						TimerInfo;							//时间信息	
};

//用户信息
struct tagUserInfoHead
{
	//用户属性
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGroupID;							//社团 I D

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户属性
	bool							bIsAndroid;							//机器标识
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行
	SCORE							lIngot;								//用户元宝
	DOUBLE							dBeans;								//用户游戏豆
	SCORE							lFirstScore;						//进入房间积分
	SCORE							lHistoryKindIDWin;					//该游戏历史总输赢

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//代理信息
	DWORD							dwAgentID;							//代理 I D
};

//头像信息
struct tagCustomFaceInfo
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息
};
//头像信息96*96
struct tagCustomFaceInfo96
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwCustomFace[BIGFACE_CX*BIGFACE_CY];//图片信息
};

//用户信息
struct tagUserRemoteInfo
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	WORD							wFaceID;							//头像索引

	//等级信息
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//位置信息
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	TCHAR							szGameServer[LEN_SERVER];			//房间位置

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wLastTableID;					    //游戏桌子
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态
};

//////////////////////////////////////////////////////////////////////////////////
//任务参数
struct tagTaskParameter
{
	//基本信息
	WORD							wTaskID;							//任务标识
	WORD							wTaskType;							//任务类型
	WORD							wTaskObject;						//任务目标

	WORD							wKindID;							//类型标识

	WORD							wServerID;							//房间标识
	WORD							wTaskProgress;						//任务进度
	BYTE							cbTaskStatus;						//任务状态
	WORD							wTimeType;							//时间类型
	
	//奖励信息
	
	DWORD							dwAwardType;						//奖励类型
	TCHAR							szAwardValue[100];					//奖励值

	//描述信息
	TCHAR							szTaskName[LEN_TASK_NAME];			//任务名称	TCHAR							szTaskDescribe[320];				//任务描述
	TCHAR							szTaskDescribe[32];					//任务描述
};

//任务详情
struct tagTaskConfigInfo
{
	//基本信息
	WORD							wTaskID;							//任务标识
	WORD							wTaskType;							//任务类型
	WORD							wUserType;							//任务类型
	WORD							wTaskObject;						//任务目标
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	WORD							wTimeType;							//时间类型
};

//任务状态
struct tagTaskStatus
{
	WORD							wTaskID;							//任务标识
	WORD							wTaskProgress;						//任务进度
	BYTE							cbTaskStatus;						//任务状态
};

//低保参数
struct tagBaseEnsureParameter
{
	SCORE							lScoreCondition;					//游戏币条件
	SCORE							lScoreAmount;						//游戏币数量
	BYTE							cbTakeTimes;						//领取次数	
};

//推广信息
struct tagUserSpreadInfo
{
	DWORD							dwSpreadCount;						//推广人数
	SCORE							lSpreadReward;						//推广奖励
};

//等级配置
struct tagGrowLevelConfig
{
	WORD							wLevelID;							//等级 I D
	DWORD							dwExperience;						//相应经验
};

//等级参数
struct tagGrowLevelParameter
{
	WORD							wCurrLevelID;						//当前等级
	DWORD							dwExperience;						//当前经验
	DWORD							dwUpgradeExperience;				//下级经验
	SCORE							lUpgradeRewardGold;					//升级奖励
	SCORE							lUpgradeRewardIngot;				//升级奖励
};

//会员参数
struct tagMemberParameter
{
	BYTE							cbMemberOrder;						//会员标识
	TCHAR							szMemberName[16];					//会员名称
	SCORE							lMemberPrice;						//会员价格
	SCORE							lPresentScore;						//赠送游戏币
};

//会员参数
struct tagMemberParameterNew
{
	BYTE							cbMemberOrder;						//会员标识
	TCHAR							szMemberName[16];					//会员名称
	DWORD                           dwMemberRight;                      //会员权限
	DWORD                           dwMemberTask;                       //会员任务
	DWORD                           dwMemberShop;                       //会员商城
	DWORD                           dwMemberInsure;                     //会员银行
	DWORD                           dwMemberDayPresent;                 //会员送金
	DWORD                           dwMemberDayGiftID;                  //会员礼包
};

//实名参数
struct tagAuthRealParameter
{
	DWORD							dwAuthentDisable;					//验证开启
	SCORE							dwAuthRealAward;					//奖励金币
};
//////////////////////////////////////////////////////////////////////////////////

//广场子项
struct tagGamePlaza
{
	WORD							wPlazaID;							//广场标识
	TCHAR							szServerAddr[32];					//服务地址
	TCHAR							szServerName[32];					//服务器名
};

//级别子项
struct tagLevelItem
{
	LONG							lLevelScore;						//级别积分
	TCHAR							szLevelName[16];					//级别描述
};

//会员子项
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//等级标识
	TCHAR							szMemberName[16];					//等级名字
};

//管理子项
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//等级标识
	TCHAR							szMasterName[16];					//等级名字
};

//列表子项
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//列表宽度
	BYTE							cbDataDescribe;						//字段类型
	TCHAR							szColumnName[16];					//列表名字
};

//等级子项
struct tagServerLevelItem
{
	DWORD							dwSortID;							//排列标识
	BYTE							cbServerLevelIndex;					//房间等级索引
	TCHAR							szServerLevelName[16];				//房间等级名字
};

//游戏聊天子项
struct tagGameChatItem
{
	DWORD							dwSortID;							//排列标识
	TCHAR							szGameChatString[MAX_CHATSTRINGLEN];//聊天字符串
	WORD							wChatTypeIndex;						//聊天类型索引
	BYTE							cbChatChance;						//聊天概率
};

//地址信息
struct tagAddressInfo
{
	TCHAR							szAddress[32];						//服务地址
};

//数据信息
struct tagDataBaseParameter
{
	WORD							wDataBasePort;						//数据库端口
	TCHAR							szDataBaseAddr[32];					//数据库地址
	TCHAR							szDataBaseUser[32];					//数据库用户
	TCHAR							szDataBasePass[32];					//数据库密码
	TCHAR							szDataBaseName[32];					//数据库名字
};

//房间配置
struct tagServerOptionInfo
{
	//挂接属性
	WORD							wKindID;							//挂接类型
	WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识

	//税收配置
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//房间配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间属性
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//用户信息
struct tagMobileUserInfoHead
{
	//用户属性
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwUserID;							//用户 I D

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户属性
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lIngot;								//用户元宝
	DOUBLE							dBeans;								//用户游戏豆

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwExperience;						//用户经验
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//代理信息
	DWORD							dwAgentID;							//代理 I D
	DWORD							dwClientAddr;						//用户地址
};
//////////////////////////////////////////////////////////////////////////////////

//更新信息
struct tagUpdateInfo
{
	WORD							wStructSize;						//结构大小
	WORD							wUpdateCount;						//更新数目
	char							szDownLoadAddress[MAX_PATH];		//下载地址
};

//更新子项
struct tagUpdateItem
{
	WORD							wStructSize;						//结构大小
	char							szFileCheckSum[33];					//文件校验和
	char							szFileName[128];					//文件名称
};

//私人房间配置
struct tagPersonalTableParameter
{
	BYTE								cbGameMode;									//游戏模式 0:局数 1：时间 2：圈数 3：积分
	LONGLONG							lIniScore;									//初始游积分

	WORD								wJoinGamePeopleCount;						//参加游戏的最大人数
	SCORE								lCellScore;									//游戏底分

	DWORD								dwPlayTurnCount; 							//私人放进行游戏的最大局数
	DWORD								dwPlayTimeLimit;							//私人房进行游戏的最大时间 单位秒
	BYTE								cbIsJoinGame;								//是否参与游戏

	DWORD								dwTimeAfterBeginCount;						//一局开始多长时间后解散桌子 单位秒
	DWORD								dwTimeOffLineCount;							//掉线多长时间后解散桌子  单位秒
	DWORD								dwTimeNotBeginGame;							//多长时间未开始游戏解散桌子	 单位秒
	DWORD								dwTimeAfterCreateRoom;						//私人房创建多长时间后无人坐桌解散桌子

	WORD								wBeginFreeTime;								//免费时段开始
	WORD								wEndFreeTime;								//免费时段结束
};


//私人房间配置
struct tagPersonalTableFeeList
{
	BYTE							cbPlayMode;							//0 积分 1 金币
	DWORD							dwDrawTimeLimit;					//时间限制
	DWORD							dwDrawCountLimit;					//局数限制
	LONGLONG						lIniScore;							//初始分数
	LONGLONG						lFeeScore;							//创建费用
	WORD							wAAPayFee;							//AA费用
	BYTE							cbGameMode;							//游戏模式
};

//私人定制配置
struct tagGetPersonalRule
{
	BYTE							cbPersonalRule[2][1024];			//(0 积分 1 金币)私人定制配置
};
//密码状态
enum enPasswordState
{
	enUnShare = 0,							//未分享状态
	enShare = 1,							//已分享状态
	enUsed = 2,								//已使用状态
};
//房间密码
struct tagRoomPassword
{
	enPasswordState		enPS;				//密码状态
	DWORD				dwRoomPass;			//房间密码
	DWORD				dwUserID;			//用户ID
};

//私人房间信息
struct tagPersonalTableInfo
{
	DWORD							dwServerID;							//房间I D
	DWORD							dwKindID;							//房间类型
	DWORD							dwTableID;							//桌子I D
	DWORD							dwUserID;							//桌主I D
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	SCORE							lCellScore;							//房间底分
	DWORD							dwPersonalRoomID;					//约战房间ID
	DWORD							dwRoomTax;							//单独一个私人房间的税率，千分比
	WORD							wJoinGamePeopleCount;				//参与游戏的人数
	BYTE							cbPassword;							//是否密码设置
	tagRoomPassword					hRoomPassword[MAX_PASSWORD_COUNT];  //房间密码
	WORD							wBeginFreeTime;						//免费时段开始
	WORD							wEndFreeTime;						//免费时段结束
	WORD							wCurSitPlayer;						//当前坐下的玩家
	BYTE							cbPlayMode;							//0 积分模式	1 金币模式
	SCORE							lEnterScoreLimit;					//进入房间上限
	SCORE							lLeaveScoreLimit;					//离开下线
	BYTE							cbJoinMode;							//是否允许陌生人加入	0 不允许	1 允许
	BYTE							cbGameRule[RULE_LEN];				//游戏规则 弟 0 位标识 是否设置规则 0 代表设置 1 代表未设置
	DWORD							dwGroupID;							//俱乐部id， 非俱乐部模式 填 0
	bool							bGameStart;							//游戏开始
	TCHAR							szNickName[LEN_NICKNAME];			//桌主姓名
	TCHAR							szCreateTime[32];					//创建时间
	BYTE							cbPayMode;							//支付模式
	DWORD							dwPlayerUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//房间里面的玩家
	DWORD							dwBankerUserID;						//百人游戏庄家USERID(无效为0)
	SCORE							lMaxChairGameMinChip;				//百人游戏最小下注
	DWORD							dwConfigID;							//玩法id
	TCHAR							szConfigName[13];					//玩法名称
	//构造函数
	tagPersonalTableInfo()
	{
		ZeroMemory(this, sizeof(tagPersonalTableInfo));
	}
};


//私人房间信息
struct tagPersonalTableInfoEx
{
	DWORD							dwServerID;							//房间I D
	DWORD							dwKindID;							//房间类型
	DWORD							dwTableID;							//桌子I D
	DWORD							dwUserID;							//桌主I D
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	SCORE							lCellScore;							//房间底分
	DWORD							dwPersonalRoomID;					//约战房间ID
	DWORD							dwRoomTax;							//单独一个私人房间的税率，千分比
	WORD							wJoinGamePeopleCount;				//参与游戏的人数
	BYTE							cbPassword;							//是否密码设置
	tagRoomPassword					hRoomPassword[MAX_PASSWORD_COUNT];  //房间密码
	WORD							wBeginFreeTime;						//免费时段开始
	WORD							wEndFreeTime;						//免费时段结束
	WORD							wCurSitPlayer;						//当前坐下的玩家
	BYTE							cbPlayMode;							//0 积分模式	1 金币模式
	SCORE							lEnterScoreLimit;					//进入房间上限
	SCORE							lLeaveScoreLimit;					//离开下线
	BYTE							cbJoinMode;							//是否允许陌生人加入	0 不允许	1 允许
	BYTE							cbGameRule[RULE_LEN];				//游戏规则 弟 0 位标识 是否设置规则 0 代表设置 1 代表未设置
	DWORD							dwGroupID;							//俱乐部id， 非俱乐部模式 填 0
	bool							bGameStart;							//游戏开始
	TCHAR							szNickName[LEN_NICKNAME];			//桌主姓名
	TCHAR							szCreateTime[32];					//创建时间
	BYTE							cbPayMode;							//支付模式
	DWORD							dwJoinUserID[PERSONAL_ROOM_CHAIR];	//约战房间内的玩家
	DWORD							dwFaceID[PERSONAL_ROOM_CHAIR];		//玩家头像
	DWORD							dwCustomID[PERSONAL_ROOM_CHAIR];						//玩家自定义头像
	TCHAR							szJoinUserNickName[PERSONAL_ROOM_CHAIR][LEN_NICKNAME];	//玩家昵称

	//构造函数
	tagPersonalTableInfoEx()
	{
		ZeroMemory(this, sizeof(tagPersonalTableInfoEx));
	}
};


//私人房配置选项
struct tagPersonalRoomOption
{
	BYTE							cbPlayMode;							//0 金币模式	1 积分模式
	BYTE							cbIsJoinGame;						// 0 表示不参与游戏  1 标识参与游戏	
	BYTE							cbMinPeople;						//房间最小人数	
	BYTE							cbMaxPeople;						//房间最大人数	
	SCORE							lMinCellScore;						//私人房的最大底分
	SCORE							lMaxCellScore;						//私人房的最大底分

	SCORE							lPersonalRoomTax;					//私人房税收

	WORD							wCanCreateCount;					//玩家能够创建的私人房的最大数目
	DWORD							dwPlayTurnCount; 					//私人放进行游戏的最大局数
	DWORD							dwPlayTimeLimit;					//私人房进行游戏的最大时间 单位秒

	DWORD							dwTimeAfterBeginCount;				//一局开始多长时间后解散桌子 单位秒
	DWORD							dwTimeOffLineCount;					//掉线多长时间后解散桌子  单位秒
	DWORD							dwTimeNotBeginGame;					//多长时间未开始游戏解散桌子	 单位秒
	DWORD							dwTimeAfterCreateRoom;				//私人房创建多长时间后无人坐桌解散桌子
	WORD							wBeginFreeTime;						//免费时段开始
	WORD							wEndFreeTime;						//免费时段结束
	SCORE							lMinEnterScore;						//私人房的最大底分
	SCORE							lMaxEnterScore;						//私人房的最大底分
	DWORD							dwTimeRespondDismiss;				//掉线玩家响应解散请求时间
};



//私人房配置选项
const int FEE_OPTION_COUNT = 5;
struct CreateRoomPayFee
{
	BYTE							cbGameMode;							//游戏模式
	DWORD							dwPlayTurnLimit;					//房间局数限制
	DWORD							dwPlayTimeLimit;					//房间时间限制
	DWORD							dwPayFee;							//创建房间消息费用
	WORD							wAAPayFee;							//AA制费用
	DWORD							dwIniScore;							//房间初始积分
	BYTE							cbPlayMode;							//0 积分  1 金币
	CreateRoomPayFee()
	{
		cbGameMode = 0;
		dwPlayTurnLimit = 0;
		dwPlayTimeLimit = 0;
		dwPayFee = 0;
		wAAPayFee = 0;
		dwIniScore = 0;
		cbPlayMode = 0;
	}
};

struct tagPersonalRoomOptionRightAndFee
{
	byte							cbCreateRight;						//创建私人房间的权限
	int								nCellScore[CELLSCORE_COUNT];
	CreateRoomPayFee				stCreateRoomPayFee[FEE_OPTION_COUNT];
};

const int MAX_CREATE_PERSONAL_ROOM = 20;
const int MAX_CREATE_SHOW_ROOM = 20;

struct tagRoomCreateInfo
{
	DWORD					dwPersonalRoomID;						//约战房间ID
	WORD					wBeginFreeTime;							//开始免费时段
	WORD					wEndFreeTime;							//结束免费时段
	WORD					wCurSitCount;							//当前坐下的玩家数量
};

//房主创建房间信息
struct tagHostCreatRoomInfo
{
	DWORD					dwUserID;
	DWORD					dwKindID;
	tagRoomCreateInfo		hRoomCreateInfo[MAX_CREATE_PERSONAL_ROOM];

	tagHostCreatRoomInfo()
	{
		memset(&hRoomCreateInfo, 0, sizeof(tagRoomCreateInfo) * MAX_CREATE_PERSONAL_ROOM);
	}
};


//约战房间玩家信息
struct tagPersonalUserScoreInfo
{
	DWORD							dwUserID;							//玩家ID
	TCHAR							szUserNicname[LEN_NICKNAME];		//用户昵称

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lTaxCount;							//税收总数

	tagPersonalUserScoreInfo()
	{
		memset(this, 0, sizeof(tagPersonalUserScoreInfo));
	}

};

//用于请求单个约战房间信息
struct tagPersonalRoomInfo
{
	DWORD					dwPersonalRoomID;							//约战房间ID
	DWORD					dwGameKindID;								//游戏类型

	DWORD					dwPlayTurnCount; 							//私人放进行游戏的最大局数
	DWORD					dwPlayTimeLimit;							//私人房进行游戏的最大时间 单位秒

	BYTE 					cbIsDisssumRoom;							//房间状态 0 表示未开始 1 表示 游戏中 2 已解散
	SYSTEMTIME				sysCreateTime;								//私人房间创建时间
	SYSTEMTIME				sysDissumeTime;								//私人房间结束时



	BYTE					cbPayMode;									//支付模式
	BYTE					cbNeedRoomCard;								//需要消耗的房卡数量
	BYTE					cbPlayerCount;								//参与游戏的人数
	WORD					wBeginFreeTime;								//开始免费时段
	WORD					wEndFreeTime;								//结束免费时段
	WORD					wCurSitCount;								//当前坐下的玩家数量
	BYTE					cbGameMode;									//游戏模式 0:局数 1：时间 2：圈数 3：积分
	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];//私人房间所有玩家信息 
	tagPersonalRoomInfo()
	{
		dwPlayTurnCount = 0;
		dwPlayTimeLimit = 0;
		cbIsDisssumRoom = 0;
		cbPayMode = 0;
		cbNeedRoomCard = 0;
		cbPlayerCount = 0;
		dwPersonalRoomID = 0;
		wBeginFreeTime = 0;
		wEndFreeTime = 0;
		wCurSitCount = 0;
		dwGameKindID = 0;
		cbGameMode = 0;
	}
};

//用于请求单个私人房间信息
struct tagQueryPersonalRoomUserScore
{
	DWORD			dwPersonalRoomID;					//约战房间ID
	TCHAR			szUserNicname[LEN_NICKNAME];		//用户昵称
	DWORD			dwPlayTurnCount; 					//私人放进行游戏的最大局数
	DWORD			dwPlayTimeLimit;					//私人房进行游戏的最大时间 单位秒
	SYSTEMTIME		sysCreateTime;						//私人房间创建时间
	SYSTEMTIME		sysDissumeTime;						//私人房间结束时

	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];//私人房间所有玩家信息 
	tagQueryPersonalRoomUserScore()
	{
		dwPlayTurnCount = 0;
		dwPlayTimeLimit = 0;
		dwPersonalRoomID = 0;
	}
};

struct tagPersonalGeneralRule
{
	int nTurnCount;				//约战局数
	int nPlayerCount;			//约战人数
	int nFee;					//约战费用
	int nIniScore;				//初始积分
	int nExOne;					//扩展配置1
	int nExTwo;					//扩展配置2
	int nExThree;				//扩展配置3
	tagPersonalGeneralRule()
	{
		memset(this, 0, sizeof(tagPersonalGeneralRule));
	}
};


//配置结构
struct tagPersonalRule
{
	byte	cbSpecialRule;											//是否有特殊规则
	tagPersonalGeneralRule personalRule[PERSONAL_OPTION_COUNT];		//配置参数
	int nCellScore[CELLSCORE_COUNT];														//初始积分
	tagPersonalRule()
	{
		memset(nCellScore, 0, sizeof(nCellScore));
	}
};

//共享库存规则
struct tagShareStorageRule
{
	BYTE							cbShareStorageRule;								//配置标识 

	//库存配置
	SCORE							lConfigSysStorage;								//配置系统库存
	SCORE							lConfigPlayerStorage;							//配置玩家库存
	SCORE							lConfigParameterK;								//配置调节系数(百分比)
	SCORE							lConfigSysStorResetRate;						//配置系统库存重置比例

	//暗抽配置
	SCORE							lConfigAnChouRate;								//配置暗抽比例

	//大奖配置
	SCORE							lConfigMosgoldRate;								//配置大奖抽取比例
	SCORE							lConfigMosgoldDispatchRate;						//配置派奖概率
	SCORE							lConfigMosgoldStorageRate;						//配置大奖库存比例	
};

//////////////////////////////////////////////////////////////////////////////////

enum enUserLogonMode
{
	//登陆方式
	ULMByAccount,
	ULMByGameID,
	ULMByVisitor,
	ULMBySina,
	ULMByTencent,
	ULMByRenRen,
};

//系统消息
struct tag_GR_SystemMessage
{
	DWORD                           dwMessageID;                        //消息ID
	BYTE							cbMessageType;						//消息类型
	BYTE                            cbAllRoom;                          //全体房间
	DWORD							dwTimeRate;						    //时间频率
	__time64_t						tStartTime;							//开始时间
	__time64_t                      tConcludeTime;                      //结束时间
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//系统消息
};

//系统消息
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //发送时间
	tag_GR_SystemMessage            SystemMessage;                     //系统消息
};
/////////////////////////////////////////////////////////////////////////////////////////////////
//俱乐部定义

//服务器子项
struct tagServiceItem
{
	WORD							wServiceID;							//逻辑标识
	WORD							wServicePort;						//服务端口
	TCHAR							szServiceAddr[32];					//服务地址

};

//网络信息
struct tagIMSocketInfo
{
	WORD							wAccessID;							//网络标识
	DWORD							dwTokenID;							//令牌标识
};

//群组约战配置
struct tagIMGroupOption
{
	DWORD							dwStationID;						//站点标识
	BYTE							cbPayTypeMask;						//支付掩码
	BYTE							cbCurrencyKindMask;					//货币掩码
};

//群组财富
struct tagIMGroupWealth
{
	LONGLONG						lIngot;								//剩余钻石
	LONGLONG						lRoomCard;							//剩余房卡
	LONGLONG						lScore;								//勋章
};

//群组属性
struct tagIMGroupProperty
{
	//基础信息
	DWORD							dwGroupID;							//群组标识	
	DWORD							dwCreaterID;						//群主标识
	DWORD							dwCreaterGameID;					//群主标识	
	WORD							wGroupLevel;						//群组级别
	TCHAR							szGroupName[LEN_GROUP_NAME];		//群组名称
	TCHAR							szCreaterNickName[LEN_ACCOUNTS];	//群主昵称

	//成员信息
	WORD							wMemberCount;						//成员数量
	WORD							wMaxMemberCount;					//成员数量

	//创建信息
	SYSTEMTIME						CreateDateTime;						//创建时间
	TCHAR							szGroupIntroduce[LEN_GROUP_INTRODUCE];//群组介绍	
	BYTE							cbGroupStatus;						//群组状态0已删除，1生效，2冻结
};

//群组规则
struct tagIMGroupBattleRule
{
	BYTE							cbPayType;							//支付类型
	BYTE							cbCurrencyKind;						//货币类型
	BYTE							cbBattleMask;						//约战掩码
	LONGLONG						lManagerRight;						//管理员权限
	LONGLONG						lCaptainRight;						//队长权限
	BYTE							cbMedalMode;						//勋章模式0关闭，1开启
	BYTE							cbMedalShow;						//勋章栏显示0不显示，1显示
	BYTE							cbCurMedalShow;						//当前勋章显示0不显示，1显示
	BYTE							cbMedalRevenueShow;					//勋章损耗显示0不显示，1显示
	BYTE							cbMedalClear;						//勋章清零0不清零，1零点清零
	BYTE							cbInMode;							//进入规则0无需审核，1审核
	BYTE							cbOutMode;							//退出规则0无需审核，1审核
	BYTE							cbFreeTableLimit;					//空座限制
	BYTE							cbBigWinnerShow;					//0不显示，1显示
	BYTE							cbGroupIDShow;						//0不显示，1显示
};

//群组附加信息
struct tagIMGroupExtraInfo
{
	DWORD							dwGroupID;							//亲友圈ID
	DWORD							dwUserID;							//玩家ID
	INT								nMemberRight;						//玩家身份
	LONGLONG						lInvitationCode;					//邀请码
	BYTE							cbGroupIDShow;						//亲友圈ID显示，0不显示，1显示
	//上级信息
	TCHAR							szQQ[LEN_NICKNAME];					//用户QQ
	TCHAR							szWX[LEN_NICKNAME];					//用户WX
	TCHAR							szPhone[12];						//用户手机号
};

//群组子项
struct tagIMGroupItem
{
	tagIMGroupWealth				GroupWealth;						//群组财富
	tagIMGroupProperty				GroupProperty;						//群组属性
	tagIMGroupBattleRule			GroupBattleRule;					//群组规则
};

//配置信息子项
struct tagIMCommonItem
{
	TCHAR							szOptionName[32];//配置名称	
	INT								nOptionValue;	//配置数值
};

//用户信息
struct tagIMUserInfo
{
	//基本属性	
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D	
	BYTE							cbGender;							//用户性别
	BYTE							cbUserStatus;						//用户状态
	WCHAR							szNickName[LEN_ACCOUNTS];			//玩家昵称	

	//自定头像
	DWORD							dwCustomID;							//自定标识	
};

//上下分结果
struct tagIMUserScore
{
	//基本属性	
	DWORD							dwUserID;							//用户ID
	BYTE							cbSuccess;							//0失败，1成功
	SCORE							lCurScore;							//当前分
};

//删除成员
struct tagIMDeleteMember
{
	DWORD							dwMemberID;							//成员ID
	BYTE							cbSuccess;							//0失败，1成功
};

//群组成员
struct tagIMGroupMember
{
	DWORD							dwMemberID;							//成员标识	
	DWORD							dwBattleCount;						//约战次数
	SYSTEMTIME						JoinDateTime;						//加入时间
	LONGLONG						lScore;								//勋章
	INT								nMemberRight;						//权限
	DWORD							dwCaptainID;						//队长ID
	BYTE							cbStatus;							//0离线，1在线
	DWORD							dwFavoriteConfig;					//收藏玩法
	BYTE							cbManagerRightFlag;					//是否单独设置管理员权限
	LONGLONG						lMemberManagerRight;				//成员管理员权限
	INT								nAwardPoint;						//队长点位
	LONGLONG						lInvitationCode;					//邀请码

	TCHAR							szQQ[LEN_NICKNAME];					//用户QQ
	TCHAR							szWX[LEN_NICKNAME];					//用户WX
	TCHAR							szPhone[12];						//用户手机号
};

//群组成员游戏信息
struct tagIMMemberDayRecord
{
	DWORD							dwMemberID;							//成员标识	
	INT								nPlayCount;							//游戏轮数
	INT								nBigWinnerCount;					//大赢家次数
	LONGLONG						lScore;								//当日最后勋章
	LONGLONG						lWinScore;							//当日输赢分
	LONGLONG						lRevenue;							//当日贡献税收
	BYTE							cbFlag;								//点赞
};

//队长以及下属游戏统计
struct tagTeamerDayRecord
{
	DWORD							dwUserID;							//成员标识
	DWORD							dwGameID;
	TCHAR							szName[LEN_NICKNAME];
	INT								nPlayCount;							//游戏轮数
	INT								nBigWinnerCount;					//大赢家次数
	LONGLONG						lRevenue;							//当日产生税收
	LONGLONG						lContribution;						//给上级的贡献值
	BYTE							cbFlag;								//点赞
};

//玩家单日详情信息
struct tagIMMemberDayDetail
{
	DWORD							dwRoomID;							//房间标识	
	BYTE							cbPlayMode;							//房间类型
	TCHAR							szServerName[32];					//房间名称
	INT								nDrawCount;							//局数
	DWORD							dwBigWinnerID;						//大赢家ID
	TCHAR							szBigWinnerName[32];				//大赢家昵称
	LONGLONG						lScore;								//得分
	SYSTEMTIME						EndTime;							//游戏结束时间
};

//房间单局详情信息
struct tagIMRoomTurnDetail
{
	INT			nTurnIndex;
	SYSTEMTIME	StartTime;
	SCORE	lTurnScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];
	bool	bFlag[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];
};

//俱乐部单日详情信息
struct tagIMGroupDayDetail
{
	DWORD							dwRoomID;							//房间标识	
	BYTE							cbPlayMode;							//房间类型
	TCHAR							szServerName[32];					//房间名称
	INT								nDrawCount;							//局数
	DWORD							dwBigWinnerID;						//大赢家ID
	TCHAR							szBigWinnerName[32];				//大赢家昵称
	LONGLONG						lDiamond;							//钻石消耗
	LONGLONG						lRoomRevenue;						//房间产生的贡献
	SYSTEMTIME						EndTime;							//游戏结束时间
};

//群组成员
struct tagIMGroupMemberUser
{
	tagIMUserInfo					UserInfo;							//用户信息
	tagIMGroupMember				MemberInfo;							//成员信息	
};

//申请消息
struct tagIMApplyMessage
{
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageStatus;					//消息状态
	BYTE							cbMessageType;						//消息类型
	//申请信息
	DWORD							dwApplyerID;						//用户标识
	DWORD							dwApplyerGameID;					//游戏标识
	DWORD							dwApplyerCustomID;					//头像标识
	TCHAR							szApplyerNickName[LEN_NICKNAME];	//用户昵称
	SYSTEMTIME						ApplyDateTime;						//申请时间

	//群组信息
	DWORD							dwGroupID;							//群组标识	
	DWORD							dwCreaterID;						//关注标识
	TCHAR							szGroupName[LEN_GROUP_NAME];		//群组名称	
	//DWORD							dwOperatorID;						//审核者ID
	//TCHAR							szOperatorName[LEN_NICKNAME];		//审核者昵称	
};

//积分提取记录
struct tagIMAwardBoxRecord
{
	SCORE							lBeforeScore;						//提取前
	SCORE							lScore;								//提取额度
	SCORE							lAfterScore;						//提取后
	SYSTEMTIME						DateTime;							//提取时间
};

//俱乐部赠送记录
struct tagIMPresentItem
{
	DWORD							dwGroupID;							//俱乐部标识
	DWORD							dwPresentID;						//操作玩家
	DWORD							dwPresentGameID;					//操作玩家游戏ID
	TCHAR							szPresentNickName[LEN_ACCOUNTS];	//操作玩家昵称
	BYTE							cbPresentUserRight;					//操作玩家身份1圈主，2管理员
	DWORD							dwReceiveID;						//接收玩家
	DWORD							dwReceiveGameID;					//接收玩家游戏ID
	TCHAR							szReceiveNickName[LEN_ACCOUNTS];	//接收玩家昵称
	LONGLONG						lChangeScore;						//分数变化值
	LONGLONG						lCurScore;							//改变后分数
	BYTE							cbType;								//类型0：赠送，1：消耗
	SYSTEMTIME						CollectDateTime;					//时间
};

//俱乐部扣费记录
struct tagIMRevenueItem
{
	DWORD							dwGroupID;							//俱乐部标识
	TCHAR							szKindName[LEN_ACCOUNTS];			//游戏名称
	LONGLONG						lScore;								//分数
	LONGLONG						lRevenue;							//税收
	SYSTEMTIME						CollectDateTime;					//时间
};

//私人房间信息
struct tagClubTableInfo
{
	DWORD							dwServerID;							//房间I D
	DWORD							dwKindID;							//房间类型
	WORD							wTableID;							//桌子I D
	DWORD							dwUserID;							//桌主I D
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	SCORE							lCellScore;							//房间底分
	DWORD							dwPersonalRoomID;					//约战房间ID
	WORD							wJoinGamePeopleCount;				//参与游戏的人数
	WORD							wCurSitPlayer;						//当前坐下的玩家
	BYTE							cbPlayMode;							//0 积分模式	1 金币模式
	SCORE							lEnterScoreLimit;					//进入房间上限
	SCORE							lLeaveScoreLimit;					//离开下线
	BYTE							cbJoinMode;							//是否允许陌生人加入	0 不允许	1 允许
	DWORD							dwGroupID;							//俱乐部id， 非俱乐部模式 填 0
	bool							bGameStart;							//游戏开始
	TCHAR							szNickName[LEN_NICKNAME];			//桌主姓名
	TCHAR							szCreateTime[32];					//创建时间
	BYTE							cbPayMode;							//支付模式
	BYTE							cbGameRule[RULE_LEN];				//游戏规则 弟 0 位标识 是否设置规则 0 代表设置 1 代表未设置
	DWORD							dwPlayerID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//玩家ID
	DWORD							dwBankerUserID;						//百人游戏庄家USERID(无效为0)
	SCORE							lMaxChairGameMinChip;				//百人游戏最小下注
	DWORD							dwConfigID;							//玩法ID
	TCHAR							szConfigName[13];					//玩法名称
	//构造函数
	tagClubTableInfo()
	{
		ZeroMemory(this, sizeof(tagClubTableInfo));
	}

};

//俱乐部玩法信息
struct tagGroupConfigInfo
{
	DWORD							dwConfigID;								//玩法ID
	DWORD							dwGroupID;								//俱乐部id
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
	BYTE							cbJoinMode;								//加入模式
	BYTE							cbTaxMode;								//税收模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家
	DWORD							dwTaxCount;								//税收数值	
	BYTE							cbLessScoreDismiss;						//分数不够解散
	LONGLONG						lTimeSecond;							//时间戳
	BYTE							cbGroupPersonalMaxChair;				//大联盟百人游戏(创建玩法就是创建桌子)

	//构造函数
	tagGroupConfigInfo()
	{
		ZeroMemory(this, sizeof(tagGroupConfigInfo));
	}

};

//俱乐部约战信息
struct tagClubSettleBattleInfo
{
	DWORD							dwUserID;							//用户I D
	DWORD							dwBattleCount;						//约战次数
	DWORD							dwSettleCount;						//结算次数
};

//俱乐部跑马灯信息
struct tagClubRollMsg
{
	DWORD							dwUserID;							//用户I D
	BYTE							cbRollMsgFlag;						//跑马灯状态
	TCHAR							szRollMsg[33];						//跑马灯内容
	INT								nRollMsgTime;						//播报间隔
};

//////////////////////////////////////////////////////////////////////////
//游戏礼包


//签到礼包
struct tagSignInReward
{
	DWORD								nPackageID;							//礼包ID
	TCHAR								szPackageName[32];					//礼包名称
	BYTE								cbType;								//礼包类型（0每日签到 1累计签到）
	DWORD								nNeedDay;							//需要时间
	DWORD								nSortID;							//礼包排序
	DWORD								nProbability;						//礼包概率
	int									nPlatformKindID;					//平台类别
};

//礼包详情
struct tagPackageInfo
{
	DWORD								nPackageID;							//礼包ID
	DWORD								dwGoodsCount;						//物品数目
	TCHAR								szPackageInfo[64];					//礼包详情
	TCHAR								szResourceURL[50];					//图片地址
};

//礼包内容
struct tagRewardInfo
{
	DWORD								dwGoodsCount;						//物品数目
	TCHAR								szPackageInfo[64];					//礼包详情
	TCHAR								szResourceURL[50];					//图片地址
};

//共享库存状态
typedef enum
{
	_NULL_STATUS_ = 0,								//无状态
	_ADD_SUBITEM_ = 11,								//添加子项(预留)
	_MODIFY_SUBITEM_ = 22,							//修改子项	
	_REMOVE_SUBITEM_ = 33,							//移除子项(预留)
	_CLIENTQUERY_SUBITEM_ = 44,						//客户端查询子项	
	_SERVERCUR_SUBITEM_ = 55,						//服务端当前子项
	_MODIFY_SUBITEM_RES_SUCCEED_ = 66,				//修改子项结果成功
	_MODIFY_SUBITEM_RES_FAILED_ = 77,				//修改子项结果失败
	_CLIENTQUERY_SUBITEM_RES_SUCCEED_ = 88,			//客户端查询子项成功	
	_CLIENTQUERY_SUBITEM_RES_FAILED_ = 99,			//客户端查询子项失败	
}ENUM_SHARESTORAGE_STATUS;		

//修改子项类型
typedef enum
{
	_NULL_SUBITEM_ = 0,				//无子项
	_MODIFY_SUBSTORAGE_ = 11,		//修改库存子项
	_MODIFY_SUBANCHOU_ = 22,		//修改暗抽子项
	_MODIFY_SUBMOSGOLD_ = 33,		//修改大奖子项
	_MODIFY_SUBTEMPSTORAGE_ = 44,	//修改临时库存子项
}ENUM_MODIFY_SUBITEM_TYPE;

//配置库存子项
struct tagConfigSubStorageItem
{
	//库存配置
	SCORE							lConfigSysStorage;								//配置系统库存
	SCORE							lConfigPlayerStorage;							//配置玩家库存
	SCORE							lConfigParameterK;								//配置调节系数(百分比)
	SCORE							lConfigSysStorResetRate;						//配置系统库存重置比例

	//暗抽配置
	SCORE							lConfigAnChouRate;								//配置暗抽比例

	//大奖配置
	SCORE							lConfigMosgoldRate;								//配置大奖抽取比例
	SCORE							lConfigMosgoldDispatchRate;						//配置派奖概率
	SCORE							lConfigMosgoldStorageRate;						//配置大奖库存比例

	//临时库存配置
	SCORE							lConfigTempSysStorage;							//配置临时系统库存
	SCORE							lConfigTempPlayerStorage;						//配置临时玩家库存
	SCORE							lConfigTempParameterK;							//配置临时调节系数(百分比)
	SCORE							lConfigTempSysStorResetRate;					//配置临时系统库存重置比例
};

//当前库存子项
struct tagCurSubStorageItem
{
	//基本配置
	SCORE							lCurStatisticSysWin;							//当前统计(包括通用库存，房间牌桌调试中系统输赢和暗抽大奖)
	SCORE							lCurStatisticService;							//当前服务比例总数
	SCORE							lCurSysStorage;									//当前系统库存
	SCORE							lCurPlayerStorage;								//当前玩家库存
	SCORE							lCurParameterK;									//当前调节系数(百分比):
	SCORE							lCurSysStorResetRate;							//当前系统库存重置比例
	SCORE							lCurWinChance;									//当前中奖率
	SCORE							lCurResetCount;									//当前库存重置次数
	SCORE							lCurSysWin;										//当前通用控制系统总输赢

	//暗抽配置
	SCORE							lCurAnChouRate;									//当前暗抽比例
	SCORE							lCurAnChouTotal;								//当前暗抽总数

	//大奖配置
	SCORE							lCurMosgoldRate;								//当前大奖抽取比例
	SCORE							lCurMosgoldDispatchRate;						//当前派奖概率
	SCORE							lCurMosgoldStorageRate;							//当前大奖库存比例

	//临时库存配置
	SCORE							lCurTempSysStorage;								//当前临时系统库存
	SCORE							lCurTempPlayerStorage;							//当前临时玩家库存
	SCORE							lCurTempParameterK;								//当前临时调节系数(百分比)
	SCORE							lCurTempSysStorResetRate;						//当前临时系统库存重置比例
	SCORE							lCurTempWinChance;								//当前临时中奖率
};

//共享库存子项
struct tagShareStorageItem
{
	ULONGLONG						ulItemID;										//子项ID(dwGroupID和wKindID组合)
	DWORD							dwGroupID;										//亲友圈标识
	WORD							wKindID;										//类型标识
	BYTE							cbPlayMode;										//0积分1金币
	ENUM_MODIFY_SUBITEM_TYPE		emModifySubItemType;							//修改子项类型
	tagConfigSubStorageItem			ConfigSubStorageItemInfo;						//配置库存子项信息
	tagCurSubStorageItem			CurSubStorageItemInfo;							//当前库存子项信息
};

//共享服务信息
struct tagShareServerInfo
{
	WORD							wServerID;										//房间标识
	WORD							wKindID;										//类型标识
	BYTE							cbPlayMode;										//0积分1金币
};

//共享配置子项
struct tagShareConfig
{
	WORD							wShareStorageItemCount;							//共享库存子项数目
	WORD							wLoadServerCount;								//加载房间数目
	tagShareServerInfo				tagServerInfoArray[MAX_SERVER];					//房间数组
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;							//当前状态
};

//////////////////////////////////////////////////////////
//大联盟AI
struct tagAIUserInfo
{
	WORD		wServerID;	//房间标识
	DWORD		dwUserID;	//AI标识
	byte		cbStatus;	//0 未分配	1 已分配 2 分配成功 3 退出房间
	SCORE		lScore;		//AI当前分数
	time_t		tTime;		//AI进入俱乐部时间
	time_t		tEnterTime;	//AI进入俱乐部时间
	byte		cbIsLeave;	//0 不离开 1 离开 2 退出
};


//离开联盟权重配置
struct tagWeightOutRatio
{
	DWORD dwTime;		//当天进入大联盟时长
	DWORD dwOutRatio;	//进入离开概率
};

//离开联盟权重配置集合
struct WeightOutOption
{
	tagWeightOutRatio WeightOutRatio[10];
};

//进入游戏权重配置
struct tagWeightInRatio
{
	DWORD dwTime;		//当天进入大联盟时长
	DWORD dwInRatio;	//进入离开概率
};
//////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif