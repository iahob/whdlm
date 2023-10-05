local cmd = {}

cmd.RES                     =   "game/yule/attlhp/res/"

--游戏版本
cmd.VERSION 					= appdf.VersionValue(7,0,1)
--游戏标识
cmd.KIND_ID						= 532  

cmd.GAME_NAME                   = "ATT连环炮"                            --游戏名字
--房间名长度
cmd.SERVER_LEN					= 32

--游戏记录长度
cmd.RECORD_LEN					= 5

cmd.MAX_CARD_COUNT              = 5

-- 状态定义

cmd.GAME_SCENE_FREE            = 0                                  --空闲状态
cmd.GAME_STATUS_PLAY            = 1             --游戏状态
cmd.GS_GAME_LUCKYTIME           = 2            --游戏状态

-- 扑克数目
-- cmd.CARD_COUNT                  = 52                                  --扑克数目

-- 牌型定义
cmd.CT_5K       =0
cmd.CT_RS       =1
cmd.CT_SF       =2
cmd.CT_4K       =3
cmd.CT_FH       =4
cmd.CT_FL       =5
cmd.CT_ST       =6
cmd.CT_3K       =7
cmd.CT_2P       =8
cmd.CT_1P       =9
cmd.CT_INVALID  =11

-- 最大猜大小次数
cmd.MAX_GUESS_COUNT = 6

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

--服务器命令结构

--*********************      服务器命令结构       ************--
cmd.SUB_S_GAME_START                 = 100               --游戏开始
cmd.SUB_S_GAME_CONCLUDE              = 101               --游戏结算
cmd.SUB_S_CURRENT_BET                = 102               --压注
cmd.SUB_S_GUESS                      = 103               --猜大小
cmd.SUB_S_SWITCH_CARD                = 104               --转换扑克
cmd.SUB_S_LUCKYTIME                  = 105               --luckytime
cmd.SUB_S_CONTINUE_GUESS             = 106               --续押
cmd.SUB_S_GAME_RECORD                = 107               --查看记录
cmd.SUB_S_GAME_AUTO                  = 108               --自动游戏
cmd.SUB_S_GAME_GUSS_INDEX            = 109               --猜大小的牌

-- BONUS结构
cmd.BONUS = 
{
    {k = "lBonus5K", t = "int"},
    {k = "lBonusRS", t = "int"},
    {k = "lBonusSF", t = "int"},
    {k = "lBonus4K", t = "int"},
}

-- 
cmd.SWITCH_CARD = 
{
    {k = "bSwitchFlag", t = "bool",l = {5}},
    {k = "cbSwitchCount", t = "byte"},
}

-- 压注倍率
cmd.BET_RADIO = 
{
    {k = "lHighRadio", t = "score"},
    {k = "lMidRadio", t = "score"},
    {k = "lLowRadio", t = "score"},
}

-- 压大小
cmd.GUESSINFO = 
{
    {k = "wLuckyCardIndex", t = "word"},
    {k = "wGuessCount", t = "word"},
    {k = "bBig", t = "bool"},
}

-- 压大小记录
cmd.GUESSRECORD = 
{
    {k = "bIsAlreadyGuess", t = "bool"}, --已经猜
    {k = "bGuessRight", t = "bool"},     --猜对
    {k = "bValid", t = "bool"},          --是否有效
}

-- 记录
cmd.RECORD_INFO = 
{
    {k = "l5KBet", t = "score"},
    {k = "lRSBet", t = "score"},
    {k = "lSFBet", t = "score"},
    {k = "l4KBet", t = "score"},
    {k = "lFHBet", t = "score"},
    {k = "lFLBet", t = "score"},
    {k = "lSTBet", t = "score"},
    {k = "l3KBet", t = "score"},
    {k = "l2PBet", t = "score"},
    {k = "l1PBet", t = "score"},
    {k = "lInvalidBet", t = "score"},
    {k = "lPlayTotal", t = "score"},
}
-- 自动标识
cmd.AUTO_FLAG = 
{
    {k = "bFirstReversalFinish", t = "bool"},   -- 第一次翻牌完成标识
    {k = "bAlreadySwitchCard", t = "bool"},     -- 已激活转换牌
}


--游戏状态 free
cmd.CMD_S_StatusGameFree = 
{
    {k = "bonus",        t = "table", d = cmd.BONUS},                          --奖池
    {k = "lExchangeRadio",        t = "int"},                          --兑换比例
    {k = "lbureauCount",       t = "int"},                         --游戏局数
    {k = "recordInfo",    t = "table", d = cmd.RECORD_INFO},                         --游戏局数
    {k = "wTableID",       t = "word"},                         --桌子ID
    {k = "szServerName",       t = "string", s =cmd.SERVER_LEN}, 
    {k = "lBetItem",       t = "score", l = {10}},  -- 筹码配置
    {k = "lBet",       t = "int"},
}

--游戏状态 play/jetton
cmd.CMD_S_StatusGamePlay = 
{
    
    {k = "bonus",        t = "table", d = cmd.BONUS},                          --奖池
    {k = "lbureauCount",       t = "int"},                         --游戏局数
    {k = "lExchangeRadio",        t = "int"},                          --兑换比例
    {k = "lBet",        t = "int"},                          --压注
    {k = "balreadySwitch",        t = "bool"},                          --是否转换
    {k = "bSwitchFlag",        t = "bool", l = {cmd.MAX_CARD_COUNT}},                          --转换标识
    {k = "bAuto",        t = "bool"},                          --压注
    {k = "cbFirstCardData",        t = "byte", l = {cmd.MAX_CARD_COUNT}},                          --初始扑克列表
    {k = "cbSecondCardData",        t = "byte", l = {cmd.MAX_CARD_COUNT}},                          --第二次扑克列表
    {k = "cbSwitchCardData",        t = "byte", l = {cmd.MAX_CARD_COUNT}},                          --经过切换的扑克列表


    {k = "recordInfo",    t = "table", d = cmd.RECORD_INFO},                         --游戏局数
    {k = "wTableID",       t = "word"},                         --桌子ID
    {k = "szServerName",       t = "string", s =cmd.SERVER_LEN},          
    {k = "lBetItem",       t = "score", l = {10}},  -- 筹码配置
}

--
cmd.CMD_S_StatusLuckyTime = 
{
    {k = "bonus",              t = "table", d = cmd.BONUS},                          --奖池
    {k = "lbureauCount",       t = "int"},                         --游戏局数
    {k = "lExchangeRadio",        t = "int"},                          --兑换比例
    {k = "lBet",               t = "int"},                          --压注
    {k = "bLuckyTimePause",        t = "bool"},                          --暂停标识
    {k = "guessRecord",        t = "table", d = cmd.GUESSRECORD, l = {6}},   --猜大小记录
    {k = "cbGuessCardResultRecord",  t = "byte", l = {6}},                  --猜大小牌记录
    {k = "wCurrentGuessCount",        t = "word"},                          --猜大小局数
    {k = "cbLuckyCardData",        t = "byte", l = {13}},                          --LuckyCard
    {k = "cbHandCardData",        t = "byte", l = {cmd.MAX_CARD_COUNT}},           --手牌扑克

    
    {k = "recordInfo",    t = "table", d = cmd.RECORD_INFO},                         --游戏局数
    {k = "wTableID",       t = "word"},                         --桌子ID
    {k = "szServerName",       t = "string", s =cmd.SERVER_LEN},    
    {k = "lBetItem",       t = "score", l = {10}},  -- 筹码配置
}

--游戏开始
cmd.CMD_S_GameStart = 
{
    {k = "cbFirstCardArray",        t = "byte", l = {cmd.MAX_CARD_COUNT}},           --初始扑克列表
    {k = "cbSecondCardArray",        t = "byte", l = {cmd.MAX_CARD_COUNT}},           --初始扑克列表
}

--游戏结算
cmd.CMD_S_GameConclude = 
{
    {k = "lUserScore",             t = "score"},                            --玩家成绩
    {k = "lbureauCount",               t = "int"},                            --游戏局数
    {k = "bonus",              t = "table", d = cmd.BONUS},                          --奖池
    {k = "bGuess",               t = "bool"},                            --是否猜大小
    {k = "recordInfo",    t = "table", d = cmd.RECORD_INFO},     -- 记录信息
    {k = "lExchangeRadio",               t = "int"},                            --兑换比例
    {k = "llastBet",               t = "int"},                            --上次下注的筹码

}

--压大小
cmd.CMD_S_Guess = 
{
    {k = "guessRecord",        t = "table", d = cmd.GUESSRECORD, l = {6}},   --猜大小记录
    {k = "cbGuessCardResultRecord",  t = "byte", l = {6}},                  --猜大小牌记录
    {k = "wCurrentGuessCount",  t = "word"},                             --当前猜大小局数
}

--压注
cmd.CMD_S_CurrentBet = 
{
    {k = "lBet",         t = "score"},                           
}

--转换
cmd.CMD_S_SwitchCard = 
{
    -- 全局信息
    {k = "switchCard",          t = "table", d = cmd.SWITCH_CARD},
    {k = "bGuess",               t = "bool"}, 
}

--luckytime
cmd.CMD_S_LuckyTime = 
{
    {k = "cbLuckyCardData",               t = "byte", l = {13}},                             --
    {k = "cbGuessCardResultRecord",               t = "byte", l = {13}},                             --
}


--记录
cmd.CMD_S_GameRecord = 
{
    {k = "bShowRecord",                   t = "bool"},                          
}

--自动游戏
cmd.CMD_S_GameAuto = 
{
    {k = "bAuto",              t = "bool"},
    -- {k = "switchCard",          t = "table", d = cmd.SWITCH_CARD},
}

cmd.CMD_S_CLICK_Guess = 
{
    {k = "cbIndex", t = "byte"} -- 牌值
}

--**********************    客户端命令结构        ************

cmd.SUB_C_LUCKYTIME                    = 20                --luckytime
cmd.SUB_C_CURRENT_BET                  = 21                --压注
cmd.SUB_C_SWITCH_CARD                  = 22                --转换扑克
cmd.SUB_C_GAME_END                     = 23                --游戏结束
cmd.SUB_C_GUESS                        = 24                --caidax
cmd.SUB_C_CONTINUE_GUESS               = 25                --续押
cmd.SUB_C_GAME_RECORD                  = 26                --查看记录
cmd.SUB_C_GAME_AUTO                    = 27                --自动游戏
cmd.SUB_C_GAME_CLICK_GUESS             = 28                --
---------------------------------------------------------------------------------------

--猜大小
cmd.CMD_C_Guess = 
{
    {k = "guessInfo",            t = "table", d = cmd.GUESSINFO},         --猜测信息
    {k = "cbKeyCardData",            t = "byte"},        --目标扑克
}
--压注
cmd.CMD_C_CurrentBet = 
{
    {k = "lBet",            t = "int"},       
}
--记录
cmd.CMD_C_GameRecord = 
{
    {k = "bShowRecord",            t = "bool"},       
}
--zidongyouxi
cmd.CMD_C_GameAuto = 
{
    {k = "bAuto",            t = "bool"},  
    -- {k = "switchCard",       t = "table", d = cmd.SWITCH_CARD},  
}
--结算
cmd.CMD_C_GameEnd = 
{
    {k = "wChairID",            t = "word"},  
    {k = "lBet",          t = "int"},     
    {k = "cbHandCardData",          t = "byte", l = {cmd.MAX_CARD_COUNT}},     
}
--转换
cmd.CMD_C_SwitchCard = 
{
    {k = "switchCard",           t = "table", d = cmd.SWITCH_CARD},  
    {k = "bSwitchCard",          t = "bool"},
    {k = "cbSwitchCardData",     t = "byte", l = {cmd.MAX_CARD_COUNT}},     
}
return cmd