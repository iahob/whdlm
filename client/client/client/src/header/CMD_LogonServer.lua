--
-- Author: zhong
-- Date: 2016-08-04 09:51:55
--

--存放logonserver部分结构体定义
--消息定义见 yl.lua
local login = {}

login.CMD_GP_AccountBind_Exists = 
{
	--目标用户
	{k = "szAccounts", t = "tchar", len = yl.LEN_ACCOUNTS},
	--用户密码
	{k = "szPassword", t = "tchar", len = yl.LEN_PASSWORD},
	--机器序列
	{k = "szMachineID", t = "tchar", len = yl.LEN_MACHINE_ID},
}

----------------------------------------------
--/服务命令
login.MDM_GP_USER_SERVICE				= 3									--用户服务

--账号服务
login.SUB_GP_MODIFY_MACHINE				= 100								--修改机器
login.SUB_GP_MODIFY_LOGON_PASS			= 101								--修改密码
login.SUB_GP_MODIFY_INSURE_PASS			= 102								--修改密码
login.SUB_GP_MODIFY_UNDER_WRITE			= 103								--修改签名

--修改头像
login.SUB_GP_USER_FACE_INFO				= 120								--头像信息
login.SUB_GP_SYSTEM_FACE_INFO			= 122								--系统头像
login.SUB_GP_CUSTOM_FACE_INFO			= 123								--自定头像

--个人资料
login.SUB_GP_USER_INDIVIDUAL			= 140								--个人资料
login.SUB_GP_QUERY_INDIVIDUAL			= 141								--查询信息
login.SUB_GP_MODIFY_INDIVIDUAL			= 152								--修改资料
login.SUB_GP_INDIVIDUAL_RESULT			= 153								--完善资料
login.SUB_GP_REAL_AUTH_QUERY			= 154								--认证请求
login.SUB_GP_REAL_AUTH_RESULT			= 155								--认证结果
login.SUB_GP_BIND_SPREADER 				= 156 								--绑定推广

--银行服务
login.SUB_GP_USER_ENABLE_INSURE			= 160								--开通银行
login.SUB_GP_USER_SAVE_SCORE			= 161								--存款操作
login.SUB_GP_USER_TAKE_SCORE			= 162								--取款操作
login.SUB_GP_USER_TRANSFER_SCORE		= 163								--转账操作
login.SUB_GP_USER_INSURE_INFO			= 164								--银行资料
login.SUB_GP_QUERY_INSURE_INFO			= 165								--查询银行
login.SUB_GP_USER_INSURE_SUCCESS		= 166								--银行成功
login.SUB_GP_USER_INSURE_FAILURE		= 167								--银行失败
login.SUB_GP_QUERY_USER_INFO_REQUEST	= 168								--查询用户
login.SUB_GP_QUERY_USER_INFO_RESULT		= 169								--用户信息
login.SUB_GP_USER_INSURE_ENABLE_RESULT 	= 170								--开通结果	

--签到服务
-- login.SUB_GP_CHECKIN_QUERY				= 220								--查询签到
-- login.SUB_GP_CHECKIN_INFO				= 221								--签到信息
-- login.SUB_GP_CHECKIN_DONE				= 222								--执行签到
-- login.SUB_GP_CHECKIN_RESULT				= 223								--签到结果

--任务服务
login.SUB_GP_TASK_LOAD					= 240								--任务加载
login.SUB_GP_TASK_TAKE					= 241								--任务领取
login.SUB_GP_TASK_REWARD				= 242								--任务奖励
login.SUB_GP_TASK_GIVEUP				= 243								--任务放弃
login.SUB_GP_TASK_INFO					= 250								--任务信息
login.SUB_GP_TASK_LIST					= 251								--任务信息
login.SUB_GP_TASK_RESULT				= 252								--任务结果
login.SUB_GP_TASK_GIVEUP_RESULT			= 253								--放弃结果

--低保服务
login.SUB_GP_BASEENSURE_LOAD			= 260								--加载低保
login.SUB_GP_BASEENSURE_TAKE			= 261								--领取低保
login.SUB_GP_BASEENSURE_PARAMETER		= 262								--低保参数
login.SUB_GP_BASEENSURE_RESULT			= 263								--低保结果

--推广服务
login.SUB_GP_SPREAD_QUERY				= 280								--推广奖励
login.SUB_GP_SPREAD_INFO				= 281								--奖励参数

--等级服务
login.SUB_GP_GROWLEVEL_QUERY			= 300								--查询等级
login.SUB_GP_GROWLEVEL_PARAMETER		= 301								--等级参数
login.SUB_GP_GROWLEVEL_UPGRADE			= 302								--等级升级

--兑换服务
login.SUB_GP_EXCHANGE_QUERY				= 320								--兑换参数
login.SUB_GP_EXCHANGE_PARAMETER			= 321								--兑换参数
login.SUB_GP_PURCHASE_MEMBER			= 322								--购买会员
login.SUB_GP_PURCHASE_RESULT			= 323								--购买结果
login.SUB_GP_EXCHANGE_SCORE_BYINGOT		= 324								--兑换游戏币
login.SUB_GP_EXCHANGE_SCORE_BYBEANS		= 325								--兑换游戏币
login.SUB_GP_EXCHANGE_RESULT			= 326								--兑换结果

--会员服务
login.SUB_GP_MEMBER_PARAMETER			= 340								--会员参数
login.SUB_GP_MEMBER_QUERY_INFO			= 341								--会员查询
login.SUB_GP_MEMBER_DAY_PRESENT			= 342								--会员送金
login.SUB_GP_MEMBER_DAY_GIFT			= 343								--会员礼包
login.SUB_GP_MEMBER_PARAMETER_RESULT	= 350								--参数结果
login.SUB_GP_MEMBER_QUERY_INFO_RESULT	= 351								--查询结果
login.SUB_GP_MEMBER_DAY_PRESENT_RESULT	= 352								--送金结果
login.SUB_GP_MEMBER_DAY_GIFT_RESULT		= 353								--礼包结果

--抽奖服务
login.SUB_GP_LOTTERY_CONFIG_REQ			= 360								--请求配置
login.SUB_GP_LOTTERY_CONFIG				= 361								--抽奖配置
login.SUB_GP_LOTTERY_USER_INFO			= 362								--抽奖信息
login.SUB_GP_LOTTERY_START				= 363								--开始抽奖
login.SUB_GP_LOTTERY_RESULT				= 364								--抽奖结果

--游戏服务
login.SUB_GP_QUERY_USER_GAME_DATA		= 370								--查询数据	

--帐号绑定
login.SUB_GP_ACCOUNT_BINDING			= 380								--帐号绑定
-- 推广绑定结果
login.SUB_GP_BIND_SPREADER_RESULT 		= 382

--操作结果
login.SUB_GP_OPERATE_SUCCESS			= 500								--操作成功
login.SUB_GP_OPERATE_FAILURE			= 501								--操作失败
login.SUB_GP_GET_USER_WEALTH			= 504
login.SUB_GP_GET_USER_WEALTH_RESULT		= 505 

--获取财富

--签到
login.SUB_GP_CHECKIN_QUERY			= 222								--查询签到
login.SUB_GP_CHECKIN_INFO			= 221								--签到信息
login.SUB_GP_CHECKIN_DONE			= 220								--签到
login.SUB_GP_CHECKIN_GETCYCREWARD	= 224								--领取
login.SUB_GP_CHECKIN_RESULT			= 223								--签到结果
login.SUB_GP_CHECKIN_INFO_END		= 225								--信息结束

-----------------------------
--用户服务

--用户头像
login.CMD_GP_UserFaceInfo = 
{
	--头像标识
	{k = "wFaceID", t = "word"},
	--自定标识
	{k = "dwCustomID", t = "dword"}
}

--系统头像 修改头像
login.CMD_GP_SystemFaceInfo = 
{
	--头像标识
	{k = "wFaceID", t = "word"},
	--用户id
	{k = "dwUserID", t = "dword"},
	--用户密码
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},
	--机器序列
	{k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},
}

--自定头像 修改头像
login.CMD_GP_CustomFaceInfo = 
{
	--用户id
	{k = "dwUserID", t = "dword"},
	--用户密码
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},
	--机器序列
	{k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},
	--图片信息
	--{k = "dwCustomFace", t = "ptr", s = yl.FACE_CX * yl.FACE_CY * 4}
}

-- 绑定机器
login.CMD_GP_ModifyMachine = 
{
	-- 绑定标志
	{ t = "byte", k = "cbBind" },
	-- 用户标识
	{ t = "dword", k = "dwUserID"},
	--用户密码
	{ t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
	--机器序列
	{ t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},
}

-- 个人资料
login.CMD_GP_UserIndividual = 
{
	-- 用户uid
	{ t = "dword", k = "dwUserID"},
}

-- 查询信息
login.CMD_GP_QueryIndividual = 
{
	-- 用户uid
	{ t = "dword", k = "dwUserID"},
	-- 用户密码
	{ t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
}

-- 实名认证
login.CMD_GP_RealAuth = 
{
	-- 用户uid
	{ t = "dword", k = "dwUserID"},
	-- 用户密码
	{ t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
	-- 真实名字
	{ t = "tchar", k = "szCompellation", s = yl.LEN_COMPELLATION},
	-- 证件号码
	{ t = "tchar", k = "szPassPortID", s = yl.LEN_PASS_PORT_ID},
}

-- 认证结果
login.CMD_GP_IndividuaResult = 
{
	-- 成功标识
	{t = "bool", k = "bSuccessed"},
	-- 当前钻石
	{t = "score", k = "lCurrDiamond"},
	-- 提示内容
	{t = "string", k = "szNotifyContent"}
}

-- 绑定推广
login.CMD_GP_BindSpreader = 
{
	-- 用户uid
	{ t = "dword", k = "dwUserID"},
	-- 用户密码
	{ t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
	-- 推广ID
	{ t = "dword", k = "dwSpreadID"},
}

-- 绑定结果
login.CMD_GP_BindSpreaderResult = 
{
	-- 奖励钻石
	{ t = "dword", k = "dwRewardDiamond"},
	-- 用户钻石
	{ t = "dword", k = "dwDiamond"},
	-- 成功消息
	{ t = "string", k = "szDescribeString", s = 128},
}

-- CMD_GP_UserIndividual
-- 携带信息
login.DTP_GP_UI_ACCOUNTS			 = 1									--用户账号	
login.DTP_GP_UI_NICKNAME			 = 2									--用户昵称
login.DTP_GP_UI_USER_NOTE			 = 3									--用户说明
login.DTP_GP_UI_UNDER_WRITE		 	 = 4									--个性签名(修改)
login.DTP_GP_UI_QQ				 	 = 5									--Q Q 号码
login.DTP_GP_UI_EMAIL				 = 6									--电子邮件
login.DTP_GP_UI_SEAT_PHONE			 = 7									--固定电话
login.DTP_GP_UI_MOBILE_PHONE		 = 8									--移动电话
login.DTP_GP_UI_COMPELLATION		 = 9									--真实名字
login.DTP_GP_UI_DWELLING_PLACE		 = 10									--联系地址
login.DTP_GP_UI_PASSPORTID    		 = 11									--身份标识
login.DTP_GP_UI_SPREADER			 = 12									--推广标识

-----------------------------
--银行服务

--银行资料
login.CMD_GP_UserInsureInfo = 
{
	{k = "cbEnjoinTransfer", t = "byte"},								--转账开关
	{k = "wRevenueTake", t = "word"},									--税收比例
	{k = "wRevenueTransfer", t = "word"},								--税收比例
	{k = "wRevenueTransferMember", t = "word"},							--税收比例
	{k = "wServerID", t = "word"},										--房间标识
	{k = "lUserScore", t = "score"},									--用户游戏币
	{k = "lUserInsure", t = "score"},									--银行游戏币
	{k = "lTransferPrerequisite", t = "score"},							--转账条件
	{k = "dwUserRight", t = "dword"},									--开放转账
}

-- 开通银行
login.CMD_GP_UserEnableInsure = 
{
	{t = "dword", k = "dwUserID"},										-- 用户id
	{t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},				-- 登陆密码
	{t = "tchar", k = "szInsurePass", s = yl.LEN_PASSWORD},				-- 用户密码
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},			-- 机器标识
}

-- 存入金币
login.CMD_GP_UserSaveScore = 
{
	{t = "dword", k = "dwUserID"},										-- 用户id
	{t = "score", k = "lSaveScore"},									-- 存入金币
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},			-- 机器标识
}

-- 提取金币
login.CMD_GP_UserTakeScore = 
{
	{t = "dword", k = "dwUserID"},										-- 用户id
	{t = "score", k = "lTakeScore"},									-- 提取金币
	{t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},				-- 用户密码
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},			-- 机器标识
}

-- 转账金币
login.CMD_GP_UserTransferScore = 
{
	{t = "dword", k = "dwUserID"},										-- 用户id
	{t = "score", k = "lTransferScore"},								-- 转账金币
	{t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},				-- 用户密码
	{t = "dword", k = "dwGameID"},										-- 目标GameID
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},			-- 机器标识
	{t = "tchar", k = "szTransRemark", s = yl.LEN_TRANS_REMARK},		-- 转账备注
}

-- 银行成功
login.CMD_GP_UserInsureSuccess = 
{
	{t = "dword", k = "dwUserID"},										-- 用户id
	{t = "score", k = "lUserScore"},									-- 用户金币
	{t = "score", k = "lUserInsure"},									-- 银行金币
	{t = "tchar", k = "szDescribeString", s = 128},						-- 描述消息
}

-- 银行失败
login.CMD_GP_UserInsureFailure = 
{
	{t = "int", k = "lResultCode"},	 									-- 错误代码
	{t = "tchar", k = "szDescribeString", s = 128},						-- 描述消息
}

-- 提取结果
login.CMD_GP_UserTakeResult = 
{
	{t = "dword", k = "dwUserID"},										-- 用户id
	{t = "score", k = "lUserScore"},									-- 用户金币
	{t = "score", k = "lUserInsure"},									-- 银行金币
}

--查询银行
login.CMD_GP_QueryInsureInfo = 
{
	--用户id
	{k = "dwUserID", t = "dword"},
	--用户密码
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},
}

--银行-用户信息
login.CMD_GP_UserTransferUserInfo = 
{
	{k = "dwTargetGameID", t = "dword"},								--目标用户
	{k = "szAccounts", t = "string", s = 32},							--目标用户
}

-- 开通结果
login.CMD_GP_UserInsureEnableResult = 
{
	{t = "byte", k = "cbInsureEnabled"},								-- 使能标识
	{t = "tchar", k = "szDescribeString", s = 128},						-- 描述消息
}


-- 修改银行密码
login.CMD_GP_ModifyInsurePass = 
{
	{t = "dword", k = "dwUserID"},										-- 用户ID
	{t = "tchar", k = "szDesPassword", s = yl.LEN_PASSWORD},			-- 修改密码
	{t = "tchar", k = "szScrPassword", s = yl.LEN_PASSWORD},			-- 原始密码
}
-----------------------------
--任务服务

--任务结果
login.CMD_GP_TaskResult =
{
	--结果信息
	{k = "bSuccessed", t = "bool"},										--成功标识
	{k = "wCommandID", t = "word"},										--命令标识

	--财富信息
	{k = "lCurrScore", t = "score"},									--当前游戏币
	{k = "lCurrIngot", t = "score"},									--当前元宝

	--提示信息
	{k = "szNotifyContent", t = "string"},								--提示内容
}

-----------------------------
--等级服务

-- 查询等级
login.CMD_GP_GrowLevelQueryInfo = 
{
	{k = "dwUserID", t = "dword"},										--用户标识
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},				--用户密码

	-- 附加信息
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},			-- 机器标识
}


-----------------------------------------签到----------------------------------------
login.CMD_GP_CheckInQueryInfo = 
{
	{k = "dwUserID", t = "dword"},										--用户标识
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},				--用户密码
	{k = "nPlatformKindID", t = "int"},										-- 平台			
}

--签到信息
login.CMD_GP_CheckInInfo = 
{
	{k = "wSeriesDate", t = "word"},										--连续日期
	{k = "bTodayChecked", t = "bool"},										--签到标识
	{k = "nGotPackageID", t = "dword", l = {16}},							--已领礼包
	{k = "nPackCount", t = "dword"},										--礼包总数
}

--礼包信息
login.tagSignInReward = 
{
	{k = "nPackageID", t = "dword"},										-- 礼包ID			
	{k = "szPackageName",t = "string", s = 32},								-- 礼包名称	
	{k = "cbType",t = "byte"},												-- 礼包类型（0每日签到 1累计签到）
	{k = "nNeedDay", t = "dword"},											--需要时间
	{k = "nSortID", t = "dword"},											--礼包排序
	{k = "nProbability", t = "dword"},										--礼包概率
	{k = "nPlatformKindID", t = "int"},										--平台ID
	{k = "dwGoodsCount", t = "dword"},										--礼包总数
	{k = "szPackInfo", t = "string",  s = 64},								-- 礼包内容	
	{k = "szImgUrl", t = "string",  s = 50},								-- 礼包下载地址											-- 平台					
}

--签到
login.CMD_GP_CheckInDone = 
{
	{k = "dwUserID", t = "dword"},											--用户标识
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},					--用户密码
	{k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},				-- 机器标识
	{k = "nPlatformKindID", t = "int"},										-- 平台			
}

--领取礼包
login.CMD_GP_CheckInGetCycReward = 
{
	{k = "dwUserID", t = "dword"},											--用户标识
	{k = "nPackageID", t = "dword"},										-- 礼包ID			
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},					--用户密码
	--{k = "nPlatformKindID", t = "int"},										-- 平台			
}

--签到结果(免费和领取都是这个接口)
login.CMD_GP_CheckInResult = 
{
	{k = "bSuccessed", t = "bool"},											--成功标识
	{k = "nPackageID", t = "dword"},										-- 礼包ID	
	{k = "cbType", t = "byte"},												-- 礼包类型（0每日签到 1累计签到）
	{k = "lScore", t = "score"},											-- 金币	
	{k = "lDiamond", t = "score"},											-- 钻石	
	{k = "nAwardsTickets", t = "int"},										-- 奖券			
	{k = "szDescribeString", t = "string", s = 128},						-- 描述消息
}


----------------------------------------------

----------------------------------------------
--道具命令
login.MDM_GP_PROPERTY						=	6	

--道具信息
login.SUB_GP_QUERY_PROPERTY					=	1							--道具查询
login.SUB_GP_PROPERTY_BUY					=	2							--购买道具
login.SUB_GP_PROPERTY_USE					=	3							--道具使用
login.SUB_GP_QUERY_SINGLE 					= 	9							--单个查询

login.SUB_GP_QUERY_PROPERTY_RESULT			=	101							--道具查询
login.SUB_GP_PROPERTY_BUY_RESULT			=	102							--购买道具
login.SUB_GP_QUERY_BACKPACKET_RESULT		=	104							--背包查询
login.SUB_GP_QUERY_SINGLE_RESULT 			= 	109 						--单个查询
login.SUB_GP_QUERY_PROPERTY_RESULT_FINISH	=	201							--道具查询

login.SUB_GP_PROPERTY_FAILURE				=	404							--道具失败

-- 道具购买
login.CMD_GP_PropertyBuy = 
{
	{k = "dwUserID", t = "dword"},										-- 用户ID
	{k = "dwPropID", t = "dword"},										-- 道具ID
	{k = "cbConsumeType", t = "byte"},									-- 钻石数目
	{k = "dwPropertyCount", t = "dword"},								-- 道具数目
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},				-- 用户密码
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},			-- 机器标识
}


--道具成功
login.CMD_GP_S_PropertySuccess = --CMD_GP_PropertyBuyResult
{
	--购买信息
	{k = "dwUserID", t = "dword"},										-- 用户ID
	{k = "dwPropID", t = "dword"},										-- 道具ID
	{k = "dwPropCount", t = "dword"},									-- 道具数量	
	{k = "lDiamond", t = "score"},										-- 钻石
	{k = "lScore", t = "score"},										-- 金币
	{k = "szNotifyContent", t = "string"},								-- 提示内容	
}

-- 查询单个
login.CMD_GP_PropertyQuerySingle = 
{
	{k = "dwUserID", t = "dword"},										--用户ID
	{k = "dwPropertyID", t = "dword"},									--道具ID
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD}				--用户密码
}

-- 查询结果
login.CMD_GP_PropertyQuerySingleResult = 
{
	{k = "dwUserID", t = "dword"},										--用户ID
	{k = "dwPropertyID", t = "dword"},									--道具ID
	{k = "dwItemCount", t = "dword"},									--道具数目
}

--道具失败
login.CMD_GP_PropertyFailure = 
{
	{k = "lErrorCode", t = "score"},									--错误代码
	{k = "szDescribeString", t = "string", s = 256},					--描述信息				
}

--获取赠送
login.CMD_GP_C_GetSendPresent = 
{
	{k = "dwUserID", t = "dword"},										--赠送者
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD}				--用户密码
}
----------------------------------------------

----------------------------------------------
--道具列表
login.MDM_GP_PROPERTY_LIST						=	2				--道具列表


login.SUB_GP_QUERY_PROPERTY_LIST_RESULT			=	112				--道具列表完成

-- 道具描述
login.tagPropertyInfo = 
{
	-- 道具信息
	-- 道具标识
	{t = "dword", k = "dwPropID"},
	-- 功能类型
	{t = "dword", k = "wKind"},


	-- 发布范围
	{t = "byte", k = "wUseArea"},
	-- 服务范围
	{t = "byte", k = "wServiceArea"},
	-- 推荐标识
	{t = "byte", k = "wRecommend"},

		-- 兑换货币类型(0钻石， 1金币)
	{t = "byte", k = "bExchangeType"},
	-- 兑换比率
	{t = "dword", k = "dwExchangeRatio"},

	-- 赠送魅力
	{t = "score", k = "lSendLoveLiness"},
	-- 接受魅力
	{t = "score", k = "lRecvLoveLiness"},
	-- 获得游戏币
	{t = "score", k = "lUseResultsGold"},

	-- 持续效果
	{t = "dword", k = "dwUseResultsVilidTime"},
	-- 有效倍率
	{t = "dword", k = "dwUseResultsVilidTimeScoreMultiple"},

	-- 礼物拆包
	{t = "dword", k = "dwUseResultsGiftPackage"},

	-- 排序标识
	{t = "dword", k = "dwSortID"},
	-- 道具名称
	{t = "string", k = "szName", s = 32},
	-- 使用信息
	{t = "string", k = "szRegulationsInfo", s = 128},
	{t = "byte", k = "cbPlatformKind"},
}


--/登陆命令
login.MDM_MB_LOGON				 		= 100									--广场登录

--登录模式
login.SUB_MB_LOGON_OTHERPLATFORM		= 4										--其他登录

--登录结果
login.SUB_MB_LOGON_SUCCESS				= 100									--登录成功
login.SUB_MB_LOGON_FAILURE				= 101									--登录失败
login.SUB_MB_MATCH_SIGNUPINFO			= 102									--报名信息

--升级提示
login.SUB_MB_UPDATE_NOTIFY				= 200									--升级提示

-- CMD_GP_LogonSuccess
-- 携带信息
login.DTP_GP_GROUP_INFO 				= 1 									-- 社团信息
login.DTP_GP_MEMBER_INFO 				= 2 									-- 会员信息
login.DTP_GP_UNDER_WRITE 				= 3 									-- 个性签名
login.DTP_GP_STATION_URL 				= 4 									-- 主页信息

------
-- 消息结构
------
-- 其他登陆
login.CMD_MB_LogonOtherPlatform = 
{
	-- 系统信息
	{t = "word", k = "wModuleID"},								-- 模块标识
	{t = "dword", k = "dwPlazaVersion"},						-- 广场版本
	{t = "byte", k = "cbDeviceType"},							-- 设备类型

	-- 登陆信息
	{t = "byte", k = "cbGender"},								-- 用户性别
	{t = "byte", k = "cbPlatformID"},							-- 平台编号
	{t = "tchar", k = "szUserUin", s = yl.LEN_USER_UIN},		-- 用户Uin
	{t = "tchar", k = "szNickName", s = yl.LEN_NICKNAME},		-- 用户昵称
	{t = "tchar", k = "szCompellation", s = yl.LEN_COMPELLATION},-- 真实名字

	-- 连接信息
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},	-- 机器标识
	{t = "tchar", k = "szMobilePhone", s = yl.LEN_MOBILE_PHONE},-- 电话号码
	{t = "tchar", k = "szDeviceToken", s = yl.LEN_DEVICE_TOKEN},-- 绑定标志
	{t = "tchar", k = "strFaceUrl", s = yl.LEN_FACE_URL},		-- 头像链接

	{t = "byte", k = "cbClientType"},							-- 平台类型（h5 1 U3D 2  lua 3）
}

-- 登陆成功
login.CMD_MB_LogonSuccess =
{
	{t = "word", k = "wFaceID"},								-- 头像标识
	{t = "byte", k = "cbGender"},								-- 用户性别
	{t = "dword", k = "dwCustomID"},							-- 自定头像
	{t = "dword", k = "dwUserID"},								-- 用户 I D
	{t = "dword", k = "dwGameID"},								-- 游戏 I D
	--{t = "dword", k = "dwExperience"},							-- 经验数值
	--{t = "score", k = "dwLoveLiness"},							-- 用户魅力
	{t = "string", k = "szAccount", s = yl.LEN_ACCOUNTS},		-- 用户帐号
	{t = "string", k = "szNickName", s = yl.LEN_NICKNAME},		-- 用户昵称
	{t = "string", k = "szDynamicPass", s = yl.LEN_PASSWORD},	-- 动态密码

	-- 财富信息
	{t = "score", k = "lUserScore"},							-- 用户游戏币
	--{t = "score", k = "lUserIngot"},							-- 用户元宝
	{t = "score", k = "lUserInsure"},							-- 用户银行
	--{t = "double", k = "dUserBeans"},							-- 用户游戏豆
	{t = "score", k = "lDiamond"},								-- 用户钻石

	-- 扩展信息
	{t = "byte", k = "cbInsureEnabled"},						-- 使能标识
	{t = "byte", k = "cbIsAgent"},								-- 代理标识
	{t = "byte", k = "cbMoorMachine"},							-- 锁定机器

	-- 约战信息
	--{t = "score", k = "lRoomCard"},								-- 用户房卡
	{t = "dword", k = "dwLockServerID"}, 						-- 锁定房间
	{t = "dword", k = "dwLockKindID"},							-- 锁定游戏

	{t = "int", k = "nAwardsTickets"},							-- 奖券
}

-- 登陆失败
login.CMD_MB_LogonFailure = 
{
	{t = "int", k = "lResultCode"},								-- 错误代码
	{t = "string", k = "szDescribeString"}, 					-- 描述信息
}
----------------------------------------------

----------------------------------------------
--/列表命令
login.MDM_MB_SERVER_LIST				= 101									--列表信息

--列表信息
login.SUB_MB_LIST_KIND					= 100									--种类列表
login.SUB_MB_LIST_SERVER				= 101									--房间列表
login.SUB_MB_LIST_MATCH					= 102								--比赛列表	
login.SUB_MB_LIST_LUA_GAME					= 103   							--游戏列表
login.SUB_MB_LIST_FINISH				= 200									--列表完成
login.SUB_MB_AGENT_KIND				= 400									--代理列表


login.SUB_MB_GET_ONLINE					= 300									--获取在线
login.SUB_MB_KIND_ONLINE				= 301									--类型在线
login.SUB_MB_SERVER_ONLINE			= 302									--房间在线
----------------------------------------------

login.CMD_MB_GetOnline = 
{
	{t = "word", k = "wServerCount"},																	-- 房间数目
	{t = "word", k = "wOnlineServerID", s = 1024}, 											-- 房间标识
}

login.tagOnlineInfo = 
{
	{t = "word", k = "wID"}, 																-- 类型标识
	{t = "dword", k = "dwOnlineCount" }, 											-- 在线人数
	{t = "dword", k = "dwAndroidCount"}, 										-- 机器人数
	{t = "dword", k = "dwSetCount"}, 												-- 在线人数
}


login.tagGameItem = 
{
	{t = "word", k = "wKindID"}, 										-- 游戏ID
	{t = "string", k = "szKindName", s = 32},						-- 游戏名字				
	{t = "word", k = "wTypeID"}, 										
	{t = "word", k = "wSortID"}, 										
	{t = "string", k = "szModuleName", s = 32},					--游戏路径
	{t = "dword", k = "dwClientVersion"}, 								
	{t = "dword", k = "dwResVersion"}, 
	{t = "byte", k = "cbNullity"}, 

}


login.CMD_MB_KindOnline = 
{
	{t = "word", k = "wKindCount"},																	-- 类型数目
	--login.tagOnlineInfo  个数按wKindCount
}


login.CMD_MB_ServerOnline = 
{
	{t = "word", k = "wServerCount"},																	-- 类型数目
	--login.tagOnlineInfo  个数按wServerCount
}


login.MDM_MB_PERSONAL_SERVICE            = 200                                   --获取锁房信息


login.SUB_MB_PERSONAL_GET_OFFLINE_INFO    = 231			--获取

login.SUB_MB_PERSONAL_GET_OFFLINE_INFO_RESULT     = 232	--返回

login.CMD_GP_GetOffLineInfoResult = 
{
	{t = "dword", k = "dwServerId"},				--
}


return login