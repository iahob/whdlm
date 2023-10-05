--[[--
游戏命令
]]

local cmd = cmd or {}

--游戏标识
cmd.KIND_ID                 = 200
cmd.VERSION 					= appdf.VersionValue(7,0,1)             --游戏版本
--游戏人数
cmd.PLAYER_COUNT            = 3
--非法视图
cmd.INVALID_VIEWID          = 0
--左边玩家视图
cmd.LEFT_VIEWID             = 1
--自己玩家视图
cmd.MY_VIEWID               = 2
--右边玩家视图
cmd.RIGHT_VIEWID            = 3

--最大数目
cmd.MAX_COUNT               = 20
--全牌数目
cmd.FULL_COUNT              = 54
--常规数目
cmd.NORMAL_COUNT            = 17
--派发数目
cmd.DISPATCH_COUNT          = 51
--房卡结算统计最大局数
cmd.MAX_RECORD_COUNT        = 30

--游戏状态
cmd.GAME_SCENE_FREE     	= 0  			--等待开始
cmd.GAME_SCENE_CALL 		= 100  			--叫分状态
cmd.GAME_SCENE_PLAY 		= 101  			--游戏进行
cmd.GAME_SCENE_END          = 255           --游戏结束

-- 倒计时
cmd.TAG_COUNTDOWN_READY     = 1
cmd.TAG_COUNTDOWN_CALLSCORE = 2
cmd.TAG_COUNTDOWN_OUTCARD   = 3
-- 游戏倒计时
cmd.COUNTDOWN_READY         = 30            -- 准备倒计时
cmd.COUNTDOWN_CALLSCORE     = 20            -- 叫分倒计时
cmd.COUNTDOWN_OUTCARD       = 20            -- 出牌倒计时
cmd.COUNTDOWN_HANDOUTTIME   = 30            -- 首出倒计时

-- 游戏胜利方
cmd.kDefault                = -1
cmd.kLanderWin              = 0
cmd.kLanderLose             = 1
cmd.kFarmerWin              = 2
cmd.kFarmerLose             = 3

-- 春天标记
cmd.kFlagDefault            = 0
cmd.kFlagChunTian           = 1
cmd.kFlagFanChunTian        = 2
---------------------------------------------------------------------------------------
--服务器命令结构
cmd.SUB_S_GAME_START        = 100           --游戏开始
cmd.SUB_S_CALL_SCORE        = 101           --用户叫分
cmd.SUB_S_BANKER_INFO       = 102           --庄家信息
cmd.SUB_S_OUT_CARD          = 103           --用户出牌
cmd.SUB_S_PASS_CARD         = 104           --用户放弃
cmd.SUB_S_GAME_CONCLUDE     = 105           --游戏结束
cmd.SUB_S_TRUSTEE           = 108           --游戏托管
cmd.SUB_S_ROOMCARD_RECORD   = 109           -- 房卡记录

------
--服务端消息结构
------

--空闲状态
cmd.CMD_S_StatusFree = 
{
    --游戏属性
    {k = "lCellScore", t = "int"},                              --基础积分

    --时间信息
    {k = "cbTimeOutCard", t = "byte"},                          --出牌时间
    {k = "cbTimeCallScore", t = "byte"},                        --叫分时间
    {k = "cbTimeStartGame", t = "byte"},                        --开始时间
    {k = "cbTimeHeadOutCard", t = "byte"},                      --首出时间

    --历史积分
    {k = "lTurnScore", t = "score", l = {cmd.PLAYER_COUNT}},    --积分信息
    {k = "lCollectScore", t = "score", l = {cmd.PLAYER_COUNT}}, --积分信息

    --游戏模式
    {k = "cbPlayMode", t = "byte"},                                    --游戏模式，0为积分房卡，1为金币房卡
    {k = "remainCardCount",  t = "table", d = cmd.REMAINCARDCOUNT},    --剩余扑克
}

cmd.REMAINCARDCOUNT = {
    --下标为0开始依次为大王，小王，2，A,K,Q,J............3, 剩余张数， 0为无剩余
    {k = "cbRemainCount", t = "byte", l = {15}},                          
}

cmd.USERACHIEVEMENT = {
    {k = "wFaceID", t = "word"},                          
    {k = "dwCustomID", t = "dword"},                          
    {k = "dwGameID", t = "dword"},                          
    {k = "szNickName", t = "string", s = 32},

    {k = "cbBombCount", t = "byte"},                          
    {k = "cbPlaneCount", t = "byte"},                          
    {k = "cbMissileCount", t = "byte"},                          
    {k = "cbChunTianCount", t = "byte"},                          
    {k = "cbFanChunTianCount", t = "byte"},                          
    
}

--托管
cmd.CMD_S_TRUSTEE = {
    {k = "wTrustee", t = "word"},                          
    {k = "bTrustee", t = "byte"},                          
}

--叫分状态
cmd.CMD_S_StatusCall = 
{
    --时间信息
    {k = "cbTimeOutCard", t = "byte"},                          --出牌时间
    {k = "cbTimeCallScore", t = "byte"},                        --叫分时间
    {k = "cbTimeStartGame", t = "byte"},                        --开始时间
    {k = "cbTimeHeadOutCard", t = "byte"},                      --首出时间

    --游戏信息
    {k = "lCellScore", t = "int"},                              --单元积分
    {k = "wCurrentUser", t = "word"},                           --当前玩家
    {k = "cbBankerScore", t = "byte"},                          --庄家叫分
    {k = "cbScoreInfo", t = "byte", l = {cmd.PLAYER_COUNT}},    --叫分信息
    {k = "cbHandCardData", t = "byte", l = {cmd.NORMAL_COUNT}}, --手上扑克
    {k = "cbUserTrustee", t = "byte", l = {cmd.PLAYER_COUNT}},  --托管标识

    --历史积分
    {k = "lTurnScore", t = "score", l = {cmd.PLAYER_COUNT}},    --积分信息
    {k = "lCollectScore", t = "score", l = {cmd.PLAYER_COUNT}}, --积分信息

    -- 倍数
    {k = "lScoreTimes", t = "int"},

        --游戏模式
    {k = "cbPlayMode", t = "byte"},                          --游戏模式，0为积分房卡，1为金币房卡
    {k = "remainCardCount",  t = "table", d = cmd.REMAINCARDCOUNT},    --剩余扑克
    {k = "cbTimeRemain",  t = "byte"},   
}

--游戏状态
cmd.CMD_S_StatusPlay = 
{
    --时间信息
    {k = "cbTimeOutCard", t = "byte"},                          --出牌时间
    {k = "cbTimeCallScore", t = "byte"},                        --叫分时间
    {k = "cbTimeStartGame", t = "byte"},                        --开始时间
    {k = "cbTimeHeadOutCard", t = "byte"},                      --首出时间

    --游戏变量
    {k = "lCellScore", t = "int"},                              --单元积分
    {k = "cbBombCount", t = "byte"},                            --炸弹次数
    {k = "wBankerUser", t = "word"},                            --庄家用户
    {k = "wCurrentUser", t = "word"},                           --当前庄家
    {k = "cbBankerScore", t = "byte"},                          --庄家叫分

    --出牌信息
    {k = "wTurnWiner", t = "word"},                             --胜利玩家
    {k = "cbTurnCardCount", t = "byte"},                        --出牌数目
    {k = "cbTurnCardData", t = "byte", l = {cmd.MAX_COUNT}},    --出牌数据

    --扑克信息
    {k = "cbBankerCard", t = "byte", l = {3}},                  --游戏底牌
    {k = "cbHandCardData", t = "byte", l = {cmd.MAX_COUNT}},    --手上扑克
    {k = "cbHandCardCount", t = "byte", l = {cmd.PLAYER_COUNT}},--扑克数目

    --历史积分
    {k = "lTurnScore", t = "score", l = {cmd.PLAYER_COUNT}},    --积分信息
    {k = "lCollectScore", t = "score", l = {cmd.PLAYER_COUNT}}, --积分信息
    {k = "cbUserTrustee", t = "byte", l = {cmd.PLAYER_COUNT}},  --托管标识

    -- 倍数
    {k = "lScoreTimes", t = "int"},

        --游戏模式
    {k = "cbPlayMode", t = "byte"},                          --游戏模式，0为积分房卡，1为金币房卡
    {k = "remainCardCount",  t = "table", d = cmd.REMAINCARDCOUNT},    --剩余扑克
    {k = "cbTimeRemain",  t = "byte"},                       --短线重连剩余秒数
}

--发送扑克/游戏开始
cmd.CMD_S_GameStart = 
{
    {k = "lCellScore", t = "int"},                              --单元积分
    {k = "wStartUser", t = "word"},                             --开始玩家
    {k = "wCurrentUser", t = "word"},                           --当前玩家
    {k = "cbValidCardData", t = "byte"},                        --明牌扑克
    {k = "cbValidCardIndex", t = "byte"},                       --明牌位置
    {k = "cbCardData", t = "byte", l = {cmd.NORMAL_COUNT}},     --扑克列表
}

--用户叫分
cmd.CMD_S_CallScore = 
{
    {k = "wCurrentUser", t = "word"},                           --当前玩家
    {k = "wCallScoreUser", t = "word"},                         --叫分玩家
    {k = "cbCurrentScore", t = "byte"},                         --当前叫分
    {k = "cbUserCallScore", t = "byte"},                        --上次叫分
}

--庄家信息
cmd.CMD_S_BankerInfo = 
{
    {k = "wBankerUser", t = "word"},                            --庄家玩家
    {k = "wCurrentUser", t = "word"},                           --当前玩家
    {k = "cbBankerScore", t = "byte"},                          --庄家叫分
    {k = "cbBankerCard", t = "byte", l = {3}},                  --庄家扑克

    -- 倍数
    {k = "lScoreTimes", t = "int"},
}

--用户出牌
cmd.CMD_S_OutCard = 
{
    {k = "cbCardCount", t = "byte"},                            --出牌数目
    {k = "wCurrentUser", t = "word"},                           --当前玩家
    {k = "wOutCardUser", t = "word"},                           --出牌玩家
    {k = "lScoreTimes", t = "int"},                             --倍数
    {k = "remainCardCount",  t = "table", d = cmd.REMAINCARDCOUNT},    --剩余扑克
    {k = "cbCardData", t = "byte", l = {cmd.MAX_COUNT}},        --扑克列表
}

--放弃出牌
cmd.CMD_S_PassCard = 
{
    {k = "cbTurnOver", t = "byte"},                             --一轮结束
    {k = "wCurrentUser", t = "word"},                           --当前玩家
    {k = "wPassCardUser", t = "word"},                          --放弃玩家
}

--游戏结束
cmd.CMD_S_GameConclude = 
{
    --积分变量
    {k = "lCellScore", t = "int"},                              --单元积分
    {k = "lGameScore", t = "score", l = {3}},                   --游戏积分

    --春天标识
    {k = "bChunTian", t = "byte"},                              --春天
    {k = "bFanChunTian", t = "byte"},                           --反春天

    --炸弹信息
    {k = "cbBombCount", t = "byte"},                            --炸弹个数
    {k = "cbEachBombCount", t = "byte", l = {cmd.PLAYER_COUNT}},--炸弹个数

    --游戏信息
    {k = "cbBankerScore", t = "byte"},                          --叫分数目
    {k = "cbCardCount", t = "byte", l = {cmd.PLAYER_COUNT}},    --扑克数目
    {k = "cbHandCardData", t = "byte", l = {cmd.FULL_COUNT}},   --扑克列表

    -- 倍数
    {k = "lScoreTimes", t = "int"},
    {k = "remainCardCount",  t = "table", d = cmd.REMAINCARDCOUNT},    --剩余扑克

    {k = "cbMaxTimes", t = "byte"},                            --是否封顶

}

-- 房卡记录
cmd.CMD_S_RoomCardRecord = 
{
    {t = "word", k = "nCount"},                                 -- 局数
    {t = "score", k = "lDetailScore", l = {cmd.MAX_RECORD_COUNT, cmd.MAX_RECORD_COUNT, cmd.MAX_RECORD_COUNT}}, -- 单局结算分
}

--客户端命令结构
cmd.SUB_C_CALL_SCORE        = 1             --用户叫分
cmd.SUB_C_OUT_CARD          = 2             --用户出牌
cmd.SUB_C_PASS_CARD         = 3             --用户放弃
cmd.SUB_C_TRUSTEE           = 4             --取消托管
cmd.SUB_C_REQUEST_RCRecord  = 5             -- 查询房卡记录

------
--客户端消息结构
------

--用户叫分
cmd.CMD_C_CallScore = 
{
    {k = "cbCallScore", t = "byte"},                            --叫分数目
}

--用户出牌
cmd.CMD_C_OutCard = 
{
    {k = "cbCardCount", t = "byte"},                            --出牌数目
    {k = "cbCardData", t = "byte", l = {cmd.MAX_COUNT}},        --扑克数据
}

cmd.CMD_C_TRUSTEE = 
{
    {k = "bTrustee", t = "byte"},                              --取消托管
}

return cmd