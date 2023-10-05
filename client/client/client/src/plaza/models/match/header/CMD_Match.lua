--
-- Author: zhouweixiang
-- Date: 2017-02-08 20:21:14
--
--比赛头文件

local match_struct = appdf.req(appdf.CLIENT_SRC .. "plaza.models.match.header.Struct_Match")

local match_CMD = {}

local match_game = {}

match_game.MDM_GR_MATCH = 10						--比赛命令

match_game.SUB_GR_MATCH_INFO = 1					--比赛信息	
match_game.SUB_GR_MATCH_RULE = 2					--比赛规则
match_game.SUB_GR_MATCH_FEE = 3						--报名费用
match_game.SUB_GR_MATCH_NUM	= 4						--等待人数
match_game.SUB_GR_LEAVE_MATCH = 5					--退出比赛
match_game.SUB_GR_MATCH_SAFECARD = 6				--保险提醒
match_game.SUB_GR_MATCH_BUY_SAFECARD = 7			--购买保险
match_game.SUB_GR_MATCH_WAIT_TIP = 8				--等待提示
match_game.SUB_GR_MATCH_RESULT = 9					--比赛结果
match_game.SUB_GR_MATCH_STATUS = 10					--比赛状态
match_game.SUB_GR_MATCH_USTATUS	= 11				--比赛状态	
match_game.SUB_GR_MATCH_DESC = 12					--比赛描述
match_game.SUB_GR_MATCH_GOLDUPDATE = 13				--游戏币更新
match_game.SUB_GR_MATCH_ELIMINATE = 14				--比赛淘汰
match_game.SUB_GR_MATCH_REVIVE = 15					--复活提醒
match_game.SUB_GR_MATCH_USER_REVIVE	= 16			--用户复活
match_game.SUB_GR_MATCH_PROMOTION = 17				--晋级消息

match_game.MAX_MATCH_DESC = 4						--最多描述 

match_game.CMD_GR_Match_Rule = 
{
	--基本信息
	{t = "dword", k = "dwMatchID"},										--比赛标识
	{t = "byte", k = "cbMatchType"},									--比赛类型
	{t = "string", k = "szMatchName", s =  32},							--比赛名称

	--报名信息
	{t = "byte", k = "cbFeeType"},										--费用类型
	{t = "byte", k = "cbDeductArea"},									--缴费区域
	{t = "score", k = "lSignupFee"},									--报名费用
	{t = "byte", k = "cbSignupMode"},									--报名方式
	{t = "byte", k = "cbJoinCondition"},								--参赛条件
	{t = "byte", k = "cbMemberOrder"},									--会员等级
	{t = "dword", k = "lExperience"},									--玩家经验 

	--排名方式
	{t = "byte", k = "cbRankingMode"},									--排名方式		
	{t = "word", k = "wCountInnings"},									--统计局数
	{t = "byte", k = "cbFilterGradesMode"},								--筛选方式

	--比赛规则
	--BYTE							cbMatchRule[512];					--比赛规则

	--奖励信息
	{t = "word", k = "wRewardCount"}, 									--奖励人数
	{t = "table", k = "MatchRewardInfo", d = match_struct.tagMatchRewardInfo, l = {3}},	--比赛奖励
}

--复活提醒
match_game.CMD_GR_Match_Revive = 
{
	--比赛信息
	{t = "dword", k = "dwMatchID"},										--比赛标识
	{t = "score", k = "lMatchNO"},										--比赛场次

	--复活信息
	{t = "score", k = "lReviveFee"},									--复活费用
	{t = "byte", k = "cbReviveTimes"},									--复活次数
	{t = "byte", k = "cbReviveTimesed"},								--复活次数
	{t = "string", k = "szNotifyContent", s = 128},					--提示内容
}

--保险提醒
match_game.CMD_GR_Match_SafeCard = 
{
	--比赛信息
	{t = "dword", k = "dwMatchID"},										--比赛标识
	{t = "score", k = "lMatchNO"},										--比赛场次

	--保险费用
	{t = "score", k = "lSafeCardFee"},									--保险费用
	{t = "string", k = "szNotifyContent", s = 128},						--提示内容
}

--购买保险
match_game.CMD_GR_Match_BuySafeCard = 
{
	{t = "bool", k = "bBuySafeCard"},									--购买标识
}

--费用提醒
match_game.CMD_GR_Match_Fee = 
{
	--比赛信息
	{t = "dword", k = "dwMatchID"},										--比赛标识
	{t = "score", k = "lMatchNO"},										--比赛场次

	--报名信息
	{t = "byte", k = "cbSignupMode"},									--报名方式
	{t = "byte", k = "cbFeeType"},										--费用类型
	{t = "byte", k = "cbDeductArea"},									--扣费区域
	{t = "score", k = "lSignupFee"},									--报名费用
	{t = "string", k = "szNotifyContent"},						--提示内容
}

--比赛人数
match_game.CMD_GR_Match_Num = 
{
	{t = "dword", k = "dwWaitting"},									--等待人数
	{t = "dword", k = "dwTotal"},										--开赛人数
	{t = "dword", k = "dwSeconds"},									--剩余时间
};

--赛事信息
match_game.CMD_GR_Match_Info = 
{
	{t = "byte", k = "cbTotalRounds"},								--总轮数
	{t = "byte", k = "cbCurRounds"},								--当前轮数
	{t = "byte", k = "cbTotalPlayCount"},							--当轮总局数
	{t = "byte", k = "cbCurPlayCount"},								--当前局数
	{t = "byte", k = "cbCurMatchRank"},								--当前排名
	{t = "byte", k = "cbStartUserCount"},							--开赛人数
	{t = "byte", k = "cbPromotion"},								--晋级人数
}

--提示信息
match_game.CMD_GR_Match_Wait_Tip = 
{
	{t = "score", k = "lScore"},										--当前积分
	{t = "word", k = "wRank"},											--当前名次
	{t = "word", k = "wCurTableRank"},									--本桌名次
	{t = "word", k = "wUserCount"},										--当前人数
	{t = "word", k = "wCurGameCount"},									--当前局数
	{t = "word", k = "wGameCount"},										--总共局数
	{t = "word", k = "wPlayingTable"},									--游戏桌数
	{t = "string", k = "szMatchName"},	 								--比赛名称
}

--比赛结果
match_game.CMD_GR_MatchResult = 
{	
	--奖励信息
	{t = "score", k = "lGold"},											--游戏币奖励
	{t = "score", k = "lIngot"},										--元宝奖励
	{t = "dword", k = "dwTicket"},									--奖励奖券
	{t = "string", k = "szNickName", s = yl.LEN_ACCOUNTS},				--玩家昵称

	--比赛信息
	{t = "word", k = "wMatchRank"},										--比赛名次
	{t = "string", k = "szMatchName", s = 64},							--比赛名称
	{t = "table", k = "MatchEndTime", d = match_struct.SYSTEMTIME},		--结束时间	
}


--比赛描述
match_game.CMD_GR_MatchDesc = 
{
	{t = "string", k = "szTitle1", s = 16},								--信息标题
	{t = "string", k = "szTitle2", s = 16},								--信息标题
	{t = "string", k = "szTitle3", s = 16},								--信息标题
	{t = "string", k = "szTitle4", s = 16},								--信息标题
	
	{t = "string", k = "szDescribe1", s = 64},							--描述内容
	{t = "string", k = "szDescribe2", s = 64},							--描述内容
	{t = "string", k = "szDescribe3", s = 64},							--描述内容
	{t = "string", k = "szDescribe4", s = 64},							--描述内容
	
	{t = "dword", k = "crTitleColor"},									--标题颜色
	--COLORREF						crTitleColor;						//标题颜色
	{t = "dword", k = "crDescribeColor"},								--描述颜色
}

--游戏币更新
match_game.CMD_GR_MatchGoldUpdate =
{
	{t = "score", k = "lCurrGold"},										--当前游戏币
}

--用户复活
match_game.CMD_GR_Match_UserRevive =
{
	{t = "bool", k = "bUseSafeCard"},									--使用保险卡
}

--系统消息
match_game.CMD_CM_SystemMessage = 
{
	{t = "word", k = "wType"},											--消息类型
	{t = "word", k = "wLength"},										--消息长度
	{t = "string", k = "szString"},										--消息内容
}

match_CMD.game = match_game

local match_define = {}

--比赛用户状态
match_define.MUS_NULL = 0x00 							--没有状态
match_define.MUS_OUT = 0x01 							--淘汰状态
match_define.MUS_SIGNUPED = 0x02 						--报名状态
match_define.MUS_PLAYING = 0x03							--比赛进行

--比赛状态
match_define.MS_FREE = 0x00								--空闲状态
match_define.MS_WAITPLAY = 0x01							--等待开始
match_define.MS_MATCHING = 0x02							--比赛进行
match_define.MS_WAITEND	= 0x04							--等待结束
match_define.MS_MATCHEND = 0x08							--比赛结束

--缴费区域
match_define.DEDUCT_AREA_SERVER	= 0						--房间缴费
match_define.DEDUCT_AREA_WEBSITE = 1					--网站缴费

--费用类型
match_define.FEE_TYPE_GOLD = 0							--缴纳游戏币
match_define.FEE_TYPE_MEDAL = 1							--缴纳奖牌	

--参赛条件
match_define.MATCH_JOINCD_MEMBER_ORDER = 0x01			--会员等级	
match_define.MATCH_JOINCD_EXPERIENCE = 0x02				--经验等级

--筛选方式
match_define.FILTER_TYPE_SINGLE_TURN = 0				--单轮排名
match_define.FILTER_TYPE_TOTAL_RANKING = 1				--总排名次

--报名模式
match_define.SIGNUP_MODE_SIGNUP_FEE	= 0x01				--报名模式
match_define.SIGNUP_MODE_MATCH_USER	= 0x02				--报名模式

--排名方式
match_define.RANKING_MODE_TOTAL_GRADES = 0				--排名方式
match_define.RANKING_MODE_SPECIAL_GRADES = 1			--排名方式 

--筛选成绩
match_define.FILTER_GRADES_MODE_BEST = 0				--最优成绩
match_define.FILTER_GRADES_MODE_AVERAGE	= 1				--平均成绩
match_define.FILTER_GRADES_MODE_AVERAGEEX = 2			--平均成绩

--开赛模式
match_define.MATCH_MODE_NORMAL = 0x00					--常规开赛
match_define.MATCH_MODE_ROUND = 0x01					--循环开赛

--比赛类型
match_define.MATCH_TYPE_LOCKTIME = 0x00					--定时类型
match_define.MATCH_TYPE_IMMEDIATE = 0x01				--即时类型

 
match_define.LEN_MATCH_LOCKTIME = 101					--定时赛附加信息长度
match_define.LEN_MATCH_IMMEDIATE = 76 				--即时赛附加信息长度

match_define.SYSMESSAGE_WAIT_RESULT = 0x0011			--定时赛等待比赛结果
match_define.SMT_MATCHRESULT  = 0x1000					--排名未进前三结果

match_define.SMT_CLOSE_GAME  = 0x0200					--退出游戏
match_define.SMT_CLOSE_ROOM  = 0x0100					--退出房间

match_define.MDM_CM_SYSTEM							= 1000			--系统命令

match_define.SUB_CM_SYSTEM_MESSAGE					= 1				--系统消息
match_define.SUB_CM_ACTION_MESSAGE					= 2				--动作消息

match_define.MDM_GF_FRAME						= 100
match_define.SUB_GF_SYSTEM_MESSAGE				= 200			--系统消息
match_define.SUB_GF_ACTION_MESSAGE				= 201			--动作消息

local tabModule = {}
-- 模块定义
tabModule.MATCHMODULE                = "client.src.plaza.models.match."              -- 比赛模块
tabModule.MATCHHEADER                = "client.src.plaza.models.match.header."       -- 比赛header
tabModule.PLAZAMODULE              	 = "client.src.plaza.models.match.plaza.src."    -- 比赛大厅模块
tabModule.GAMEMODULE              	 = "client.src.plaza.models.match.game."         -- 比赛游戏模块
match_define.tabModule = tabModule

-- 场景/层 tag定义
local tabLayTag = {}
tabLayTag.MATCH_TYPELIST  = 3000							--比赛，约战类型选择
tabLayTag.MATCH_ROOMLIST  = 3001							--比赛房列表
tabLayTag.MATCH_INFOLAYER = 3002							--比赛信息
tabLayTag.LAYER_FRIENDLIST = 3003							--好友分享
tabLayTag.MATCH_LAYEREND  = 3010							--比赛界面结束

match_define.tabLayTag = tabLayTag

-- 游戏服务器登陆操作定义
local tabLoginAction = {}
match_define.tabLoginAction = tabLoginAction

--比赛游戏状态

match_CMD.define = match_define

return match_CMD

