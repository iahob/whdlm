local cmd = {}

cmd.RES                     =   "game/yule/rbbattle/res/"

--游戏版本
cmd.VERSION 					= appdf.VersionValue(7,0,1)
--游戏标识
cmd.KIND_ID						= 144  -- 144

cmd.GAME_NAME                   = "红黑大战"                            --游戏名字

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
cmd.GS_PLACE_JETTON             = cmd.GAME_STATUS_PLAY               --下注状态
cmd.GS_GAME_END                 = cmd.GAME_STATUS_PLAY+1             --结束状态


cmd.MAX_TCP_BUFFER              = 16000
cmd.WEEK                        = 7                                   -- 一周7天 

-- 扑克数目
cmd.CARD_COUNT                  = 52                                  --扑克数目
--区域索引
cmd.ID_RED_AR                   = 0                                 --红
cmd.ID_BLACK_AR                 = 1                                 --黑
cmd.ID_POKER_TYPE_AR            = 2                                 --牌型区
cmd.MAX_INDEX                   = 3                                 --最大索引

--区域索引
cmd.RED_INDEX                   = 1                                 --red索引
cmd.BLACK_INDEX                 = 2                                 --black索引
cmd.POKER_TYPE_INDEX            = 3                                 --poker type索引


cmd.AREA_COUNT                  = 3                                 -- 区域数目
cmd.CONTROL_AREA                = 3                                 -- 区域数目

-- 赔率定义
cmd.RATE_PAIR                   = 1                                 --对子赔率
cmd.RATE_SHUN                   = 2                                 --顺子赔率
cmd.RATE_JING                   = 3                                 --金花赔率
cmd.RATE_SHUN_JING              = 5                                 --顺金赔率
cmd.RATE_BAO_ZI                 = 10                                --豹子赔率


----游戏音乐音效常量
--背景音乐
cmd.SOUND_GameSceneBGM = "RedBlack_GameBGM.mp3";
--弹出红VS黑动效音效
cmd.EFFECT_RedVsBlack = "RedVsBlack.mp3";
--开牌音效
cmd.EFFECT_OpenCard1 = "miCard1.mp3";
cmd.EFFECT_OpenCard2 = "miCard2.mp3";
cmd.EFFECT_OpenCard3 = "miCard3.mp3";
--爆池动效音效
cmd.EFFECT_BaoChi = "baochi.mp3";
--金币区域移动动效 音效
cmd.EFFECT_AreaGoldMoveing = "goldmove.mp3";
--开始下注音效
cmd.EFFECT_StartJetton = "startJetton.mp3";
--停止下注音效
cmd.EFFECT_StopJetton = "stopJetton.mp3";
--开始下注敲钟音效
cmd.EFFECT_StartJettonAlert = "alert.mp3";
--倒计时音效
cmd.EFFECT_Countdown = "countdown.mp3";
--下注筹码音效
cmd.EFFECT_JettonGold = "movechips.mp3";
--获胜音效
cmd.EFFECT_Bigwin = "movechips.mp3";


--机器人配置
cmd.tagCustomAndroid = 
{
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
    {k = "lRobotAreaLimit", t = "score", l = {2}},
    
    --存取款
    --游戏币下限
    {k = "lRobotScoreMin", t = "score"},
    --游戏币上限
    {k = "lRobotScoreMax", t = "score"},
    --取款最小值(非庄)
    {k = "lRobotBankGetMin", t = "score"},
    --取款最大值(非庄)
    {k = "lRobotBankGetMax", t = "score"},
    --存款百分比
    {k = "lRobotBankStoMul", t = "score"},
}


--记录信息
cmd.tagServerGameRecord = 
{   
    {k = "bWinRed", t = "bool"},                    --红方胜利
    {k = "bWinBlack", t = "bool"},                  --黑方胜利
    {k = "bWinPoker", t = "bool"},                  --牌型区域胜利
    {k = "cbPokerType", t = "byte"},                --1单张，2对子（2到8），3对子（9到a），4顺子，5金花，6顺金，7豹子
}

--下注信息
cmd.tagUserBet = 
{   
    {k = "szNickName", t = "string", s = 32},                    --用户昵称
    {k = "dwUserGameID", t = "dword"},                           --用户ID
    {k = "lUserStartScore", t = "score"},                        --用户金币
    {k = "lUserWinLost", t = "score"},                           --用户金币
    {k = "lUserBet", t = "score", l = {cmd.AREA_COUNT}},         --用户下注
}

--服务器命令结构

--*********************      服务器命令结构       ************--
cmd.SUB_S_GAME_FREE                 = 99                --游戏空闲
cmd.SUB_S_GAME_START                = 100               --游戏开始
cmd.SUB_S_PLACE_JETTON              = 101               --用户下注
cmd.SUB_S_GAME_END                  = 102               --游戏结束
cmd.SUB_S_UP_DESK                   = 103               --用户申请上桌
cmd.SUB_S_HISTORY_RECORD            = 104               --游戏路单记录
cmd.SUB_S_QURARY_LOTTORY_INFO       = 105               --查询开奖信息
cmd.SUB_S_SEND_RECORD               = 106               --游戏记录,没用到
cmd.SUB_S_PLACE_JETTON_FAIL         = 107               --下注失败
cmd.SUB_S_DOWN_DESK                 = 108               --用户下桌，通常是三轮不下注或者输光了或者强退游戏发送该消息
cmd.SUB_S_SEND_REJETTON             = 109               --发送玩家是否满足重复投以及上轮投注情况
cmd.SUB_S_QURARY_ROOM_PLAYERS_INFO  = 110               --查询房间中玩家的信息
cmd.SUB_S_CANCEL_BANKER             = 111               --取消申请
cmd.SUB_S_APPLY_BANKER              = 112               --申请庄家
cmd.SUB_S_CHANGE_BANKER             = 113               --切换庄家


-- --用户下注、控制端
-- cmd.CMD_S_SendUserBetInfo = 
-- {
--     {k = "lUserStartScore",    t = "score", l = {200} },           --起始分数
--     {k = "lUserJettonScore",   t = "score", l = {200,200,200}},                --个人总注
-- }

--玩家信息
cmd.CMD_S_SendRoomUserInfo = 
{
    {k = "nPlayerCount",     t = "int"},                          --房间中玩家个数
    {k = "wChairID",         t = "word",  l = {20}},              --玩家
    {k = "lBetRank20Score",  t = "score", l = {20}},              --赌豪前二十人的身家几何
    {k = "lBetRank20Jetton", t = "score", l = {20}},              --赌豪前二十人近二十轮下注总金额
    {k = "nWinCount",        t = "int",   l = {20}},              --赌豪前二十人的近二十轮的胜利次数，玩法不足二十次也包括在内
}

--发送玩家上局下注情况
cmd.CMD_S_SendBetInfo = 
{
    {k = "iResult",          t = "int"},                               --0成功，1是没有上轮数据，2是金币不够。
    {k = "lUserJettonScore", t = "score", l = {cmd.AREA_COUNT}},       --个人总注
}

--申请庄家
cmd.CMD_S_ApplyBanker = 
{
    {k = "wApplyUser",       t = "word"},           --申请上庄玩家
}

--取消申请
cmd.CMD_S_CancelBanker = 
{
    {k = "bAuto",             t = "bool"},           --是否自动取消申请
    {k = "wCancelUser",       t = "word"},           --取消申请上庄玩家
    {k = "bIsAllowApplyBanker",   t = "bool"},           --是否已申请下庄
}

--切换庄家
cmd.CMD_S_ChangeBanker = 
{
    {k = "wBankerUser",       t = "word"},             --当庄玩家
    {k = "lBankerScore",       t = "score"},           --庄家金币
}

--玩家下桌子
cmd.CMD_S_GameDown = 
{
    {k = "wChairID",       t = "word"},           --下桌子玩家
    {k = "wDeskChairID",   t = "word"},           --下桌子椅子號
}

--下注失败结构
cmd.CMD_S_PlaceJettonFail = 
{
    {k = "wPlaceUser",        t = "word"},           --下注玩家
    {k = "lJettonArea",       t = "byte"},           --下注区域
    {k = "lPlaceScore",       t = "score"},          --当前下注
    {k = "cbReason",          t = "byte"},           --下注失败原因
}

--游戏状态 free
cmd.CMD_S_StatusFree = 
{
    {k = "bIsAllowApplyBanker",t = "bool"},                          --玩家是否能够申请上庄
    -- 全局信息
    {k = "cbTimeLeave",        t = "byte"},                          --剩余时间
    {k = "lCurrentPoolScore",  t = "score"},                         --奖池分数
    {k = "cbDeskChairID",      t = "byte",  l = {6}},                --做桌玩家id
    {k = "lBet",               t = "score",  l = {5}},                --配置筹码

    -- 庄家信息
    {k = "wBankerUser",        t = "word"},                          --当前庄家
    {k = "cbBankerTime",       t = "word"},                          --庄家局数
    {k = "lBankerWinScore",    t = "score"},                         --庄家成绩
    {k = "lBankerScore",       t = "score"},                         --庄家分数
    -- 控制信息
    {k = "lApplyBankerCondition",  t = "score"},                     --申请条件
    {k = "lLimitScore",        t = "score"},                         --上桌限制分数
    {k = "lUserMaxScore",      t = "score"},                         --玩家单局下注最大金币数
    {k = "lAreaLimitScore",    t = "score", l = {2}},                --区域限制[0]-红黑区域限制，[1]-牌型区域限制
    -- 房间信息
    {k = "szGameRoomName",     t = "string",  s = cmd.SERVER_LEN},   --房间名称
    {k = "CustomAndroid",      t = "table", d = cmd.tagCustomAndroid},--机器人配置
}

--游戏状态 play/jetton
cmd.CMD_S_StatusPlay = 
{
    {k = "bIsAllowApplyBanker",t = "bool"},                            --玩家是否能够申请上庄

    {k = "bIsJettonStatus",    t = "bool"},                            --当前游戏处于押注状态或者开牌结算状态 
    {k = "lBet",               t = "score",  l = {5}},                --配置筹码                 
    --全局下注                      
    {k = "lAllJettonScore",    t = "score",   l = {cmd.AREA_COUNT}},   --全体总注   
    --玩家下注        
    {k = "lUserJettonScore",   t = "score",   l = {cmd.AREA_COUNT}},   --个人总注
    --玩家积分              
    {k = "lUserMaxScore",      t = "score"},                           --最大下注
    --控制信息    
    {k = "lApplyBankerCondition",   t = "score"},                      --申请条件  
    {k = "lLimitScore",        t = "score"},                           --上桌限制分数                
    {k = "lAreaLimitScore",    t = "score",   l = {2}},                --区域限制
    --扑克信息
    {k = "cbTableCardArray",   t = "byte",    l = {3,3}},              --桌面扑克
    {k = "lStorageStart",      t = "score"},                           --库存数值  
    --结束信息
    {k = "lCurrentPoolScore",  t = "score"},                           --奖池分数                  
    {k = "lEndUserScore",      t = "score"},                           --玩家成绩  
    {k = "lEndUserReturnScore",t = "score"},                           --返回积分      
    {k = "lEndRevenue",        t = "score"},                           --游戏税收      
                  
    {k = "cbDeskChairID",      t = "byte",    l = {6}},                --做桌玩家id
    -- 庄家信息
    {k = "wBankerUser",        t = "word"},                            --当前庄家
    {k = "cbBankerTime",       t = "word"},                            --庄家局数
    {k = "lBankerWinScore",    t = "score"},                           --庄家赢分
    {k = "lBankerScore",       t = "score"},                           --庄家分数
    --结束信息
    {k = "lEndBankerScore",       t = "score"},                        --庄家成绩
    --全局信息  
    {k = "cbTimeLeave",        t = "byte"},                            --剩余时间  
    {k = "cbGameStatus",       t = "byte"},                            --游戏状态   
    -- 房间信息
    {k = "szGameRoomName",     t = "string",  s = cmd.SERVER_LEN},     --房间名称
    {k = "CustomAndroid",      t = "table",   d = cmd.tagCustomAndroid},--机器人配置
--gameend gamestart 注释
}

--游戏空闲
cmd.CMD_S_GameFree = 
{
    {k = "cbTimeLeave",         t = "byte"},           --剩余时间
    {k = "lStorageStart",       t = "score"},          --库存数值
    {k = "lPoolScore",          t = "score"},          --彩池数值
    {k = "wCurrentBanker",      t = "word"},           --当前庄家
    {k = "nBankerTime",         t = "int"},            --做庄次数
    {k = "nListUserCount",      t = "int"},            --玩家列表数量、没用到
}

--游戏开始
cmd.CMD_S_GameStart = 
{
    {k = "wBankerUser",         t = "word"},           --当前庄家
    {k = "nBankerTime",         t = "int"},            --做庄次数
    {k = "lBankerScore",        t = "score"},          --庄家金币
    {k = "lStorageStart",       t = "score"},          --库存数值
    {k = "lCurrentPoolScore",   t = "score"},          --奖池分数
    {k = "lUserMaxScore",       t = "score"},          --我的金币
    {k = "cbTimeLeave",         t = "byte"},           --剩余时间
    {k = "nChipRobotCount",     t = "int"},            --人数上限 (下注机器人)
    {k = "nAndriodApplyCount",  t = "int"},            --机器人列表人数
}

--游戏路单等数据记录
cmd.CMD_S_GameHistoryRecord = 
{
    {k = "cbRBHistory",         t = "byte", l = {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6}},  --牌路走势记录,1为红方胜，2为黑方胜
    {k = "cbRB_WL_History",     t = "byte", l = {20}},          --近二十局红黑胜负走势记录,1为红方胜，2为黑方胜
    {k = "cbPokerType",         t = "byte", l = {20}},          --近二十轮1单张，2对子（2到8），3对子（9到a），4顺子，5金花，6顺金，7豹子
}

--用户下注
cmd.CMD_S_PlaceJetton = 
{
    {k = "wChairID",            t = "word"},           --用户位置
    {k = "cbJettonArea",        t = "byte"},           --筹码区域
    {k = "lJettonScore",        t = "score"},          --加注数目
    {k = "bIsAndroid",          t = "bool"},           --是否机器人
}

--用户上桌
cmd.CMD_S_UpDesk = 
{
    {k = "wChairID",            t = "word"},         --用户位置ID
    {k = "bUpDeskOk",           t = "bool"},         --上桌是否成功
    {k = "wDeskChairID",        t = "word"},         --桌子椅子號
    {k = "cbFailReason",        t = "byte"},         --上桌失败原因,1没有空余位置/ 2财富不满足坐桌条件
}

--开奖信息查询
cmd.CMD_S_WinLottoryInfo = 
{   
    {k = "lpoolScore",          t = "score"},                         --奖池
    {k = "cbOpenType",          t = "byte"},                          --开奖牌型
    {k = "lTotalReward",        t = "score"},                         --总开奖金额
    {k = "cbWinName",           t = "string", l={100}, s = 32},       --中奖人的昵称
    {k = "nWinScore",           t = "int",    l = {100}},             --中奖人的分的金额
    {k = "nOpenTime",           t = "int",    l = {6}},               --开奖时间,年月日时分秒
    {k = "dwUserID",            t = "dword",  l = {100}},             --
    {k = "dwCustomID",          t = "dword",  l = {100}},             --
    {k = "wFaceID",             t = "word",  l = {100}},              --
    {k = "cbGender",            t = "byte",  l = {100}},              --
}

--游戏结束
cmd.CMD_S_GameEnd = 
{
    {k = "bIsOpenReward",       t = "bool"},               --本轮是否开出奖池大奖
    -- 下局信息
    {k = "cbTimeLeave",         t = "byte"},               --剩余时间
    -- 扑克信息
    {k = "cbTableCardArray",    t = "byte",   l = {3,3}},  --桌面扑克
    {k = "lCurrentPoolScore",   t = "score"},              --奖池分数
    -- 玩家成绩
    {k = "lUserScore",          t = "score"},              --玩家成绩
    {k = "lUserReturnScore",    t = "score"},              --返回积分

    {k = "lUserScoreT",         t = "score"},              --玩家成绩，没有除去抽水和税收的值，用来给客户端显示筹码动画需要
    {k = "bWinRed",             t = "bool"},               --红方胜利
    {k = "bWinBlack",           t = "bool"},               --黑方胜利
    {k = "bWinPoker",           t = "bool"},               --牌型区域胜利
    {k = "cbPokerType",         t = "byte",   l = {2}},    --1单张，2对子(2到8)，3对子（9到a），4顺子，5金花，6顺金，7豹子
    {k = "lBetDeskScore",       t = "score",  l = {6}},    --桌上玩家输赢score
    {k = "lDeskScoreT",         t = "score",  l = {6}},    --桌上玩家成绩，没有除去抽水和税收的值，用来给客户端显示筹码动画需要

    -- 庄家信息
    {k = "wCurrentBanker",        t = "word"},                            --当前庄家
    {k = "lBankerScore",          t = "score"},                           --庄家成绩
    {k = "lBankerTotallScore",    t = "score"},                           --庄家总分数
    {k = "nBankerTime",           t = "int"},                             --做庄次数
}


--**********************    客户端命令结构        ************

cmd.SUB_C_PLACE_JETTON                  = 1                --用户下注
cmd.SUB_C_APPLY_DESK                    = 2                --用户申请上桌
cmd.SUB_C_QURARY_LOTTORY_INFO           = 3                --用户查询开奖信息
cmd.SUB_C_CLICK_REJETTON                = 4                --用户勾选重复投
cmd.SUB_C_QURARY_RECORD                 = 5                --用户请求记录信息
cmd.SUB_C_QURARY_ROOM_USERS_INFO        = 6                --查询房间中用户的信息
cmd.SUB_C_APPLY_BANKER                  = 7                --申请庄家
cmd.SUB_C_CANCEL_BANKER                 = 8                --取消申请
---------------------------------------------------------------------------------------


--用户下注
cmd.CMD_C_PlaceJetton = 
{
    {k = "wUserChairID",            t = "word"},         --用户位置id
    {k = "cbJettonArea",            t = "byte"},         --筹码区域
    {k = "lJettonScore",            t = "score"},        --加注数目
}

--用户申请上桌
cmd.CMD_C_ApplyDesk = 
{   
    {k = "wUserChairID",            t = "word"},         --用户chairid
    {k = "bApply",                  t = "bool"},         --申请上桌
    {k = "wDeskChairID",            t = "word"},         --桌号
}

--用户查询开奖信息
cmd.CMD_C_QuaryLottoryInfo = 
{   
    {k = "wUserChairID",            t = "word"},          --用户id
}

--玩家点击重复下注
cmd.CMD_C_UserReJetton = 
{   
    {k = "wUserChairID",            t = "word"},          --用户id
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