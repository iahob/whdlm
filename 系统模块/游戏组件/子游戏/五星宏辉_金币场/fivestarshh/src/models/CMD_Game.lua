local cmd = {}

cmd.RES                     =   "game/yule/fivestarshh/res/"

--游戏版本
cmd.VERSION 					= appdf.VersionValue(7,0,1)
--游戏标识
cmd.KIND_ID						= 147  -- 147

cmd.GAME_NAME                   = "五星宏辉"                            --游戏名字

--游戏人数
cmd.GAME_PLAYER					= 200

--房间名长度
cmd.SERVER_LEN					= 32

--游戏记录长度
cmd.RECORD_LEN					= 5

--视图位置
cmd.MY_VIEWID					= 2

-- 状态定义

cmd.GAME_STATUS_FREE            = 0                                  --空闲状态
cmd.GAME_STATUS_PLAY            = 100                                --游戏状态

cmd.GAME_SCENE_FREE             = cmd.GAME_STATUS_FREE               --等待开始
cmd.GAME_SCENE_BET              = cmd.GAME_STATUS_PLAY               --下注状态
cmd.GAME_SCENE_END              = cmd.GAME_STATUS_PLAY+1             --结束状态

-- 扑克数目
-- cmd.CARD_COUNT                  = 52                                  --扑克数目

-- 游戏定义
cmd.LU_DAN_COUNT_BIG                  = 100                                  --路单最大局数
cmd.LU_DAN_COUNT_SMALL                  = 48                                  --小路单局数
cmd.MAX_CARD_COUNT                = 54*8                                  --牌堆数量

--区域索引
cmd.AREA_HEI                    = 0                                 --黑桃
cmd.AREA_HONG                   = 1                                 --红桃
cmd.AREA_MEI                    = 2                                 --梅花
cmd.AREA_FANG                   = 3                                 --方块
cmd.AREA_KING                   = 4                                 --皇冠
cmd.AREA_MAX                    = 5                                 --最大区域

--花色
cmd.TYPE_1                    = 1                                 --黑桃
cmd.TYPE_2                    = 2                                 --红桃
cmd.TYPE_3                    = 3                                 --梅花
cmd.TYPE_4                    = 4                                 --方块
cmd.TYPE_5                    = 5                                 --小王
cmd.TYPE_6                    = 6                                 --大王

-- 区域倍数
cmd.MULTIPLE_HEI                    = 3.8                                 --黑桃
cmd.MULTIPLE_HONG                   = 3.8                                 --红桃
cmd.MULTIPLE_MEI                    = 4.0                                 --梅花
cmd.MULTIPLE_FANG                   = 4.0                                 --方片
cmd.MULTIPLE_KING                   = 20.0                                --王


----游戏音乐音效常量
--背景音乐
cmd.SOUND_GameSceneBGM = "bg.mp3";
--切换筹码音效
cmd.EFFECT_ExchangeChip = "exchangechip.mp3";
--开牌音效
cmd.EFFECT_OpenCard1 = "turncard.mp3";
--翻到人音效
cmd.EFFECT_OpenCard2 = "turncard1.wav";
--飞花音效
cmd.EFFECT_FlyFlower = "flyflower.wav";
--开始下注音效
cmd.EFFECT_StartJetton = "startJetton.mp3";
--停止下注音效
cmd.EFFECT_StopJetton = "stopJetton.mp3";
--倒计时音效
cmd.EFFECT_Countdown = "countdown.wav";
--下注音效
cmd.EFFECT_Jetton = "betbtn.mp3";
--获胜音效
cmd.EFFECT_Bigwin = "endresult.wav";

--牌值对应图
cmd.ServerCardValueSprite = 
{
    --方块 A-K
    [1] = "card_101",[2] = "card_102",[3] = "card_103",[4] = "card_104",[5] = "card_105",
    [6] = "card_106",[7] = "card_107",[8] = "card_108",[9] = "card_109",[10] = "card_110",
    [11] = "card_111",[12] = "card_112",[13] = "card_113",
    --梅花 A-K
    [17] = "card_201",[18] = "card_202",[19] = "card_203",[20] = "card_204",[21] = "card_205",
    [22] = "card_206",[23] = "card_207",[24] = "card_208",[25] = "card_209",[26] = "card_210",
    [27] = "card_211",[28] = "card_212",[29] = "card_213",
    --红桃 A-K
    [33] = "card_101", [34] = "card_102", [35] = "card_103", [36] = "card_104", [37] = "card_105", 
    [38] = "card_106", [39] = "card_107", [40] = "card_108", [41] = "card_109", [42] = "card_110", 
    [43] = "card_111", [44] = "card_112", [45] = "card_113",
    --黑桃 A-K
    [49] = "card_201", [50] = "card_202", [51] = "card_203", [52] = "card_204", [53] = "card_205",
    [54] = "card_206", [55] = "card_207", [56] = "card_208", [57] = "card_209", [58] = "card_210",
    [59] = "card_211", [60] = "card_212", [61] = "card_213",

    --大小王
    [78] = "card_401", [79] = "card_301",

}

--机器人配置
cmd.tagCustomAndroid = 
{
    --下注
    --下注筹码个数
    {k = "lRobotMinBetTime", t = "score"},
    --下注筹码个数
    {k = "lRobotMaxBetTime", t = "score"},
    --下注筹码金额
    {k = "lRobotMinJetton", t = "score"},
    --下注筹码金额
    {k = "lRobotMaxJetton", t = "score"},
    --下注机器人数
    {k = "lRobotBetMinCount", t = "score"},
    --下注机器人数
    {k = "lRobotBetMaxCount", t = "score"},
    --区域限制
    {k = "lRobotAreaLimit", t = "score"},
    
    --存取款
    --游戏币下限
    {k = "lRobotScoreMin", t = "score"},
    --游戏币上限
    {k = "lRobotScoreMax", t = "score"},
    --取款最小值(非庄)
    {k = "lRobotBankGetMin", t = "score"},
    --取款最大值(非庄)
    {k = "lRobotBankGetMax", t = "score"},
    --取款最小值(非庄)
    {k = "lRobotBankGetBankerMin", t = "score"},
    --取款最大值(非庄)
    {k = "lRobotBankGetBankerMax", t = "score"},
    --存款百分比
    {k = "lRobotBankStoMul", t = "score"},

    --是否做庄
    {k = "nEnableRobotBanker", t = "bool"},
    --坐庄次数
    {k = "lRobotBankerCountMin", t = "score"},
    --坐庄次数
    {k = "lRobotBankerCountMax", t = "score"},
    --列表人数
    {k = "lRobotListMinCount", t = "score"},
    --列表人数
    {k = "lRobotListMaxCount", t = "score"},
    --最多申请个数
    {k = "lRobotApplyBanker", t = "score"},
    --空盘重申
    {k = "lRobotWaitBanker", t = "score"},

    --区域几率
    {k = "nAreaChance", t = "int", l = {cmd.AREA_MAX}},
}

--服务器命令结构

--*********************      服务器命令结构       ************--
cmd.SUB_S_GAME_FREE                 = 100               --游戏空闲
cmd.SUB_S_GAME_START                = 101               --游戏开始
cmd.SUB_S_PLACE_JETTON              = 102               --用户下注
cmd.SUB_S_GAME_END                  = 103               --游戏结束
cmd.SUB_S_APPLY_BANKER              = 104               --申请庄家
cmd.SUB_S_CHANGE_BANKER             = 105               --切换庄家
cmd.SUB_S_CANCEL_BANKER             = 106               --取消申请
cmd.SUB_S_PLACE_JETTON_FAIL         = 107               --下注失败
cmd.SUB_S_HORN_INFO                 = 108               --大奖广播

--游戏状态 free
cmd.CMD_S_StatusFree = 
{
    -- 全局信息
    {k = "cbTimeLeave",        t = "byte"},                          --剩余时间

    -- 庄家信息
    {k = "wBankerUser",        t = "word"},                          --当前庄家
    {k = "lBankerScore",       t = "score"},                         --庄家分数
    {k = "lBankerWinScore",    t = "score"},                         --庄家成绩
    {k = "wBankerTime",        t = "word"},                          --庄家局数
    {k = "lApplyBankerCondition",  t = "score"},                     --申请庄家条件

    -- 控制信息
    {k = "lAreaLimitScore",    t = "score"},                          --区域限制
    {k = "lUserLimitScore",    t = "score"},                          --个人限制
    {k = "lChipCount",         t = "score",  l = {5}},                --注额

    --大路单信息100
    {k = "cbLuDanBigCount",       t = "byte"},                           --大路单数量
    {k = "cbLuDanBigData",        t = "byte", l = {cmd.LU_DAN_COUNT_BIG}},   --大路单扑克

    --小路单信息48
    {k = "cbLuDanSmallCount",       t = "byte"},                           --小路单数量
    {k = "cbLuDanSmallData",        t = "byte", l = {cmd.LU_DAN_COUNT_SMALL}},   --小路单扑克

    --统计信息
    {k = "nGameCount",         t = "BYTE"},                            --游戏局数
    {k = "nOpenColorCount",    t = "int", l = {cmd.AREA_MAX}},        --花色统计
}

--游戏状态 play/jetton
cmd.CMD_S_StatusPlay = 
{
    --游戏信息  
    {k = "cbTimeLeave",        t = "byte"},                            --剩余时间  
    {k = "cbGameStatus",       t = "byte"},                            --游戏状态   
     -- 庄家信息
    {k = "wBankerUser",        t = "word"},                            --当前庄家
    {k = "lBankerScore",       t = "score"},                           --庄家分数
    {k = "lBankerWinScore",    t = "score"},                           --庄家赢分
    {k = "wBankerTime",        t = "word"},                             --庄家局数
    {k = "lApplyBankerCondition",   t = "score"},                      --申请庄家条件  
    --控制信息                 
    {k = "lAreaLimitScore",    t = "score"},                --区域限制
    {k = "lUserLimitScore",    t = "score"},                           --个人限制
    --注额信息
    {k = "lChipCount",         t = "score",  l = {5}},                 --注额

    --大路单信息100
    {k = "cbLuDanBigCount",       t = "byte"},                           --大路单数量
    {k = "cbLuDanBigData",        t = "byte", l = {cmd.LU_DAN_COUNT_BIG}},   --大路单扑克

    --小路单信息48
    {k = "cbLuDanSmallCount",       t = "byte"},                           --小路单数量
    {k = "cbLuDanSmallData",        t = "byte", l = {cmd.LU_DAN_COUNT_SMALL}},   --小路单扑克

    --统计信息
    {k = "nGameCount",         t = "byte"},                             --游戏局数
    {k = "nOpenColorCount",    t = "int", l = {cmd.AREA_MAX}},         --花色统计

    --扑克信息
    {k = "cbCurrentOpenCard",   t = "byte"},                           --开奖扑克

    --下注数
    {k = "lAllBet",             t = "score", l = {cmd.AREA_MAX}},      --总下注
    {k = "lPlayBet",            t = "score", l = {cmd.AREA_MAX}},      --玩家下注

    --用户信息
    {k = "lUserAllWinLose",     t = "score"},                           --总输赢       
    --玩家成绩       
    {k = "lGameScore",          t = "score"},                           --玩家成绩  
    {k = "lRevenue",t = "score"},                                       --游戏税收      
    
}

--游戏空闲
cmd.CMD_S_GameFree = 
{
    --全局信息
    {k = "cbTimeLeave",          t = "byte"},                            --剩余时间
    --统计信息
    {k = "nGameCount",           t = "byte"},                             --游戏局数
    {k = "nOpenColorCount",      t = "int", l = {cmd.AREA_MAX}},         --花色统计
    
    --大路单信息100
    {k = "cbLuDanBigCount",       t = "byte"},                           --大路单数量
    {k = "cbLuDanBigData",        t = "byte", l = {cmd.LU_DAN_COUNT_BIG}},   --大路单扑克

    --小路单信息48
    {k = "cbLuDanSmallCount",       t = "byte"},                           --小路单数量
    {k = "cbLuDanSmallData",        t = "byte", l = {cmd.LU_DAN_COUNT_SMALL}},   --小路单扑克

    --用户信息
    {k = "lUserAllWinLose",      t = "score"},                           --总输赢    
    
}

--游戏开始
cmd.CMD_S_GameStart = 
{
    {k = "cbTimeLeave",          t = "byte"},                            --剩余时间
    {k = "wBankerUser",          t = "word"},                            --庄家位置
    {k = "lBankerScore",         t = "score"},                           --庄家金币
    --AI使用
    {k = "nChipRobotCount",      t = "int"},                             --人数上限 (下注AI)
    {k = "nListUserCount",       t = "int"},                             --列表人数
    {k = "nAndriodCount",        t = "int"},                             --AI列表人数
}

--用户下注
cmd.CMD_S_PlaceBet = 
{
    {k = "wChairID",             t = "word"},                            --剩余时间
    {k = "cbArea",               t = "byte"},                            --庄家位置
    {k = "lBetScore",            t = "score"},                           --庄家金币
}

--申请庄家
cmd.CMD_S_ApplyBanker = 
{
    {k = "wApplyUser",           t = "word"},                             --申请上庄玩家
}

--取消申请
cmd.CMD_S_CancelBanker = 
{
    {k = "wCancelUser",          t = "word"},                             --取消申请上庄玩家
    {k = "bAuto",                t = "bool"},                             --是否自动取消申请
    {k = "bIsAllowApplyBanker",  t = "bool"},                             --是否已申请下庄
}

--切换庄家
cmd.CMD_S_ChangeBanker = 
{
    {k = "wBankerUser",          t = "word"},                             --当庄玩家
    {k = "lBankerScore",         t = "score"},                            --庄家金币
}

--游戏结束
cmd.CMD_S_GameEnd = 
{
    -- 全局信息
    {k = "cbTimeLeave",          t = "byte"},                             --剩余时间
    -- 扑克信息
    {k = "cbCurrentOpenCard",    t = "byte"},                             --开奖扑克
    -- 庄家信息
    {k = "lBankerScore",         t = "score"},                            --庄家本局成绩
    {k = "lBankerWinScore",      t = "score"},                            --庄家总成绩
    {k = "nBankerTime",          t = "int"},                              --做庄次数

    -- 玩家成绩
    {k = "lGameScore",           t = "score",l = {200}},                            --玩家成绩
    {k = "lRevenue",             t = "score"},                            --游戏税收
}

--广播信息
cmd.CMD_S_HORN = 
{
    -- 全局信息
    {k = "lScore",                t = "score"},                            --播报额度
    {k = "lTimes",                t = "score"},                            --倍数
    {k = "cbCount",               t = "byte"},                             --
}

--广播玩家
cmd.HORNINFO = 
{
    -- 全局信息
    {k = "lScore",                t = "score"},                             --赢分
    {k = "szNickName",            t = "string", s = 32},                    --名称
    {k = "bAI",                   t = "bool"},                              --AI
}

--下注失败
cmd.CMD_S_PlaceBetFail = 
{
    {k = "wChairID",              t = "word"},                              --下注玩家
    {k = "cbBetArea",             t = "byte"},                              --下注区域
    {k = "lPlaceScore",           t = "score"},                             --当前下注
    {k = "cbReason",              t = "byte"},                              --失败原因
}

--**********************    客户端命令结构        ************

cmd.SUB_C_PLACE_JETTON                  = 1                --用户下注
cmd.SUB_C_APPLY_BANKER                  = 2                --申请庄家
cmd.SUB_C_CANCEL_BANKER                 = 3                --取消申请
cmd.SUB_C_CLEAR_JETTON                  = 4                --用户下注
---------------------------------------------------------------------------------------

--用户下注
cmd.CMD_C_PlaceBet = 
{
    {k = "cbBetArea",            t = "byte"},         --筹码区域
    {k = "lBetScore",            t = "score"},        --加注数目
}
cmd.CMD_C_ClearBet = 
{
    {k = "wChairID",              t = "word"},         --筹码区域
  
}

--申请列表
function cmd.getEmptyApplyInfo(  )
    return
    {
        --用户信息
        m_userItem = {},
        --是否当前庄家
        m_bCurrent = false,
        --编号
        m_llIdx = 0,
        --是否超级抢庄
        m_bRob = false
    }
end

print("********************************************************load cmd");
return cmd