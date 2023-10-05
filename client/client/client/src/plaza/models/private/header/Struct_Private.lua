--
-- Author: zhong
-- Date: 2016-12-03 13:01:43
--

local private_define = appdf.req(appdf.CLIENT_SRC .. "plaza.models.private.header.Define_Private")
local private_struct = {}

-- 时间定义
private_struct.SYSTEMTIME = 
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

-- 私人房间配置
private_struct.tagPersonalTableParameter = 
{
    -- 游戏模式
    {t = "byte", k = "cbPlayMode"},    --0 积分 1 金币
    -- 时间限制
    {t = "dword", k = "dwDrawTimeLimit"},
    -- 局数限制
    {t = "dword", k = "dwDrawCountLimit"},
    -- 初始分数
    {t = "score", k = "lIniScore"},
    -- 创建费用
    {t = "score", k = "lFeeScore"},
    -- AA费用
    {t = "word", k = "wAAPayFee"},
    -- 游戏模式
    {t = "byte", k = "cbGameMode"},
}

-- 私人房间玩家信息
private_struct.tagPersonalUserScoreInfo = 
{
    -- 玩家ID
    {t = "dword", k = "dwUserID"},
    -- 玩家昵称
    {t = "string", k = "szUserNicname", s = yl.LEN_NICKNAME},

    -- 积分信息
    -- 用户分数
    {t = "score", k = "lScore"},
    -- 用户成绩
    {t = "score", k = "lGrade"},
    -- 税收总数
    {t = "score", k = "lTaxCount"},
}

-- 用于请求单个私人房间信息
private_struct.tagPersonalRoomInfo = 
{
    --{t = "string", k = "szRoomID", s = private_define.ROOM_ID_LEN},
    {t = "dword", k = "dwPersonalRoomID"},
    -- 游戏类型
    {t = "dword", k = "dwGameKindID"},
    -- 消耗类型(0 游戏豆; 1 房卡)
    --{t = "byte", k = "cbCardOrBean"},
    -- 消耗游戏豆 或 房卡的数量
    --{t = "score", k = "lFeeCardOrBeanCount"},
    -- 私人放进行游戏的最大局数
    {t = "dword", k = "dwPlayTurnCount"},
    -- 私人房进行游戏的最大时间 单位秒
    {t = "dword", k = "dwPlayTimeLimit"},
    -- 是否解散房间(房间状态 0 表示未开始 1 表示 游戏中 2 已解散)
    {t = "byte", k = "cbIsDisssumRoom"},
    -- 私人房间创建时间
    {t = "table", k = "sysCreateTime", d = private_struct.SYSTEMTIME},
    -- 私人房间结束时间
    {t = "table", k = "sysDissumeTime", d = private_struct.SYSTEMTIME},
    -- 税收总数
    --{t = "score", k = "lTaxCount"},
    -- 支付模式
    {t = "byte", k = "cbPayMode"},
    -- 消耗数量
    {t = "byte", k = "lNeedRoomCard"},
    -- 参与人数
    {t = "byte", k = "lPlayerCount"},
    -- 免费开始时段
    {t = "word", k = "wBeginFeeTime"},
    -- 免费结束时段
    {t = "word", k = "wEndFeeTime"},
    -- 当前坐下玩家数量
    {t = "word", k = "wCurSitCount"},
    -- y游戏模式（圈/局）
    {t = "byte", k = "cbGameMode"},
    -- 私人房间所有玩家信息
    {t = "table", k = "PersonalUserScoreInfo", d = private_struct.tagPersonalUserScoreInfo, l = {private_define.PERSONAL_ROOM_CHAIR}}
}

-- 用于请求单个私人房间信息
private_struct.tagQueryPersonalRoomUserScore = 
{
    --{t = "string", k = "szRoomID", s = private_define.ROOM_ID_LEN},
    {t = "dword", k = "dwPersonalRoomID"},
    -- 房主昵称
    {t = "string", k = "szUserNicname", s = yl.LEN_NICKNAME},
    -- 私人放进行游戏的最大局数
    {t = "dword", k = "dwPlayTurnCount"},
    -- 私人房进行游戏的最大时间 单位秒
    {t = "dword", k = "dwPlayTimeLimit"},
    -- 私人房间创建时间
    {t = "table", k = "sysCreateTime", d = private_struct.SYSTEMTIME},
    -- 私人房间结束时间
    {t = "table", k = "sysDissumeTime", d = private_struct.SYSTEMTIME},

    -- 私人房间所有玩家信息
    {t = "table", k = "PersonalUserScoreInfo", d = private_struct.tagPersonalUserScoreInfo, l = {private_define.PERSONAL_ROOM_CHAIR}}
}

-- 私人房配置选项
private_struct.tagPersonalRoomOption = 
{
    -- 游戏模式
    {t = "byte", k = "cbPlayMode"},    --0 积分 1 金币
    --[[-- 消耗类型(0 游戏豆; 1 房卡)
    {t = "byte", k = "cbCardOrBean"},
    -- 消耗数量
    {t = "score", k = "lFeeCardOrBeanCount"},]]
    -- 是否参与 (0 不参与; 1 参与)
    {t = "byte", k = "cbIsJoinGame"},
    -- 房间最小人数
    {t = "byte", k = "cbMinPeople"},
    -- 房间最大人数
    {t = "byte", k = "cbMaxPeople"},
    -- 私人房的最小底分
    {t = "score", k = "lMinCellScore"},
    -- 私人房的最大底分
    {t = "score", k = "lMaxCellScore"},
    -- 私人房税收
    {t = "score", k = "lPersonalRoomTax"},

    -- 玩家能够创建的私人房的最大数目
    {t = "word", k = "wCanCreateCount"},
    -- 私人房进行游戏的最大局数
    {t = "dword", k = "dwPlayTurnCount"},
    -- 私人房进行游戏的最大时间 单位秒
    {t = "dword", k = "dwPlayTimeLimit"},

    -- 一局开始多长时间后解散桌子 单位秒
    {t = "dword", k = "dwTimeAfterBeginCount"},
    -- 掉线多长时间后解散桌子 单位秒
    {t = "dword", k = "dwTimeOffLineCount"},
    -- 多长时间未开始游戏解散桌子 单位秒
    {t = "dword", k = "dwTimeNotBeginGame"},
    -- 私人房创建多长时间后无人坐桌解散桌子
    {t = "dword", k = "dwTimeAfterCreateRoom"},

    -- 免费开始时段
    {t = "word", k = "wBeginFeeTime"},
    -- 免费结束时段
    {t = "word", k = "wEndFeeTime"},
    -- 私人房的最小进入分
    {t = "score", k = "lMinEnterScore"},
    -- 私人房的最大进入分
    {t = "score", k = "lMaxEnterScore"},

    {t = "dword", k = "dwTimeLast"},
}

-- 底分配置
private_struct.tagPersonalCellScore = 
{
    {t = "int", k = "nCellScore", l = {private_define.CELL_SCORE_NUM}}
}

private_struct.tagPersonalRule = 
{
    {t = "byte", k = "cbPersonalRule", l = {private_define.LEN_PERSONAL_RULE}}
}

private_struct.tagPersonalGeneralRule =
{
    {t = "int", k = "nTurnCount"},
    {t = "int", k = "nPlayerCount"},
    {t = "int", k = "nFee"},
    {t = "int", k = "nIniScore"},
    {t = "int", k = "nExOne"},
    {t = "int", k = "nExTwo"},
    {t = "int", k = "nExThree"},
}

private_struct.tagPersonalRuleNormal =
{
    {t = "byte", k = "cbSpecialRule"},
    {t = "table", k = "tagPersonalGeneralRule ", d = private_struct.tagPersonalGeneralRule, l = {5}},
    {t = "table", k = "tagPersonalCellScore ", d = private_struct.tagPersonalCellScore},
}

-- 用于请求单个金币私人房间信息
private_struct.tagGoldPersonalRoomInfo = 
{
    --房间id
    {t = "dword", k = "dwPersonalRoomID"},
    -- 游戏类型
    {t = "dword", k = "dwGameKindID"},
    --ID
    {t = "dword", k = "dwServerID"},
    -- 桌子ID
    {t = "dword", k = "dwTableID"},
    -- 底分
    {t = "score", k = "lCellScore"},
    --单独一个私人房间的税率，千分比
    {t = "dword", k = "dwRoomTax"},
    -- 参与游戏最大人数
    {t = "word", k = "wPlayerCount"},
    -- 参与人数
    {t = "word", k = "lPlayerCount"},
    -- 免费开始时段
    {t = "word", k = "wBeginFeeTime"},
    -- 免费结束时段
    {t = "word", k = "wEndFeeTime"},
    -- 进入房间上限
    {t = "score", k = "lEnterScoreLimit"},
    -- 离开房间下限
    {t = "score", k = "lLeaveScoreLimit"},
    -- 私人房间所有玩家信息
    --{t = "byte", k = "cbGameRule", l = {100}},
}


return private_struct