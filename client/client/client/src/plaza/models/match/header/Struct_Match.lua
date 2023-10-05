
--
-- Author: zhouweixiang
-- Date: 2017-02-09 19:48:30
--
local match_struct = {}

match_struct.MAX_MATCH_ROUNDS = 10

match_struct.MAX_MATCH_SECTION = 10

----------比赛房间结构体-------
-- 时间定义
match_struct.SYSTEMTIME = 
{
    {t = "word", k = "wYear"},
    {t = "word", k = "wMonth"},
    {t = "word", k = "wDayOfWeek"},
    {t = "word", k = "wDay"},
    {t = "word", k = "wHour"},
    {t = "word", k = "wMinute"},
    {t = "word", k = "wSecond"},
    {t = "word", k = "wMilliseconds"},
}


match_struct.tagMatchRounds =
{
	{t = "byte", k = "cbPlayCount"},						 			--游戏局数
	{t = "word", k = "wPromotion"},					 					--晋级人数
	--{t = "score", k = "lInitalScore"},									--初始积分
	--{t = "int", k = "bResetFlag"},										--复位或累计
}

--定时赛
match_struct.tagLockTimeMatch = 
{
	--时间配置
	{t = "table", k = "MatchStartTime", d = match_struct.SYSTEMTIME},			
	{t = "table", k = "SignupStartTime", d = match_struct.SYSTEMTIME},			--开始日期
	{t = "table", k = "SignupEndTime", d = match_struct.SYSTEMTIME},			--结束日期

	--报名参数
	{t = "byte", k = "cbMatchMode"},								--开赛模式
	{t = "word", k = "wMaxSignupUser"},							--报名人数
	{t = "word", k = "wStartUserCount"},								--开赛人数	

	--分数信息
	{t = "score", k = "lInitalBase"},									--初始基数

	{t = "score", k = "lInitalScore"},									--初始积分 --add


	 {t="table",k="hMatchRounds",d=match_struct.tagMatchRounds,l={match_struct.MAX_MATCH_ROUNDS}},								


	{t = "byte", k = "cbTotalRounds"}, --总轮数
	{t = "byte", k = "cbTotalPlayCount"},	--总局数


}


--即时赛
match_struct.tagImmediateMatch = 
{
	--人数信息
	{t = "word", k = "wStartUserCount"},								--开赛人数	
	{t = "word", k = "wAndroidUserCount"},								--机器数量
	{t = "word", k = "wAndroidDelaySignupTime"},						--延迟报名

	--分数信息
	{t = "score", k = "lInitalBase"},									--初始基数
	{t = "score", k = "lInitalScore"},									--初始积分 --add
	

	{t="table",k="hMatchRounds",d=match_struct.tagMatchRounds,l={match_struct.MAX_MATCH_ROUNDS}},

	{t = "byte", k = "cbTotalRounds"}, --总轮数
	{t = "byte", k = "cbTotalPlayCount"},	--总局数

	{t = "byte", k = "cbInitAndroidMin"},
	{t = "byte", k = "cbInitAndroidMax"},
	{t = "int", k = "bAndroidCheat"},
	{t = "byte", k = "cbNo1Ratio1"},
	{t = "byte", k = "cbNo1Ratio2"},
	{t = "byte", k = "cbNo1Ratio3"},

	{t = "word", k = "wAndroidFullTime"},	
	{t = "byte", k = "cbTotalSection"},								 
	{t = "byte", k = "cbSectionCount",l={match_struct.MAX_MATCH_SECTION}},
	
	--局数信息
	--{t = "word", k = "wPlayCount"},										--游戏局数
	-- {t = "byte", k = "cbSwitchTableCount"},								--换桌局数(0为不换桌)
	-- {t = "word", k = "wPrecedeTimer"},									--优先坐下
}	

--奖励信息
match_struct.tagMatchRewardInfo = 
{
	{t = "word", k = "wRankID"},										--奖励名次
	{t = "score", k = "lRewardGold"},									--奖励游戏币
	{t = "score", k = "lRewardIngot"},									--奖励元宝	
	{t = "dword", k = "dwTicket"},							--奖励奖券
}


--比赛信息
match_struct.tagGameMatch = 
{
	--基本信息
	{t = "word", k = "wServerID"},										--房间标识
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
	{t = "int", k = "lExperience"},									--玩家经验

	--排名方式
	{t = "byte", k = "cbRankingMode"},									--排名方式		
	{t = "word", k = "wCountInnings"},									--统计局数
	{t = "byte", k = "cbFilterGradesMode"},								--筛选方式

	--奖励信息
	{t = "word", k = "wRewardCount"},									--奖励人数
	{t = "table", k = "MatchRewardInfo", d = match_struct.tagMatchRewardInfo, l = {3}},	--比赛奖励

	--比赛规则
	--BYTE							cbMatchRule[512];					--比赛规则

	--{t = "table", k ="cbMatchRule",d=match_struct.tagImmediateMatch},
}
--

return match_struct