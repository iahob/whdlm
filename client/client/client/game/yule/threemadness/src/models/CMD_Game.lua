local cmd =  {}

cmd.RES 						= "game/yule/threemadness/res/"
--游戏版本
cmd.VERSION 					= appdf.VersionValue(6,7,0,1)
--游戏标识
cmd.KIND_ID						= 56
	
--游戏人数
cmd.GAME_PLAYER					= 5

--视图位置
cmd.MY_VIEWID					= 3

--空闲状态
cmd.GAME_STATUS_FREE			= 0
--游戏状态
cmd.GAME_STATUS_PLAY			= 100

--***************服务端命令结构**************--
--游戏开始
cmd.SUB_S_GAME_START			= 100
--加注结果
cmd.SUB_S_ADD_SCORE				= 101
--放弃跟注
cmd.SUB_S_GIVE_UP				= 102
--发牌消息
cmd.SUB_S_SEND_CARD				= 103
--游戏结束
cmd.SUB_S_GAME_END				= 104
--比牌跟注
cmd.SUB_S_COMPARE_CARD			= 105
--看牌跟注
cmd.SUB_S_LOOK_CARD				= 106
--用户强退
cmd.SUB_S_PLAYER_EXIT			= 107
--开牌消息
cmd.SUB_S_OPEN_CARD				= 108
--等待比牌
cmd.SUB_S_WAIT_COMPARE			= 109
--智能消息
cmd.SUB_S_ANDROID_CARD			= 110
--看牌消息
cmd.SUB_S_CHEAT_CARD			= 111

--游戏币不足
cmd.SUB_S_RC_TREASURE_DEFICIENCY	= 121

cmd.SUB_S_ROOMCARD_RECORD           =123                                 --房卡记录

cmd.SUB_S_TURN_COUNT               = 124                                --当前轮数

--***************客户端命令结构**************--
--用户加注
cmd.SUB_C_ADD_SCORE				= 1
--放弃消息
cmd.SUB_C_GIVE_UP				= 2
--比牌消息
cmd.SUB_C_COMPARE_CARD			= 3
--看牌消息
cmd.SUB_C_LOOK_CARD				= 4
--开牌消息
cmd.SUB_C_OPEN_CARD				= 5
--等待比牌
cmd.SUB_C_WAIT_COMPARE			= 6
--完成动画
cmd.SUB_C_FINISH_FLASH			= 7
--完成动画
cmd.SUB_C_ADD_SCORE_TIME		= 8
-- 请求流水
cmd.SUB_C_REQUEST_RCRecord                 = 16

--公共加密原文长度
cmd.AESENCRYPTION_LENGTH        = 16
--最大牌数
cmd.MAX_COUNT                   = 3

------------------------------------------------消息结构体----------------------------------
--机器人存款取款
cmd.tagCustomAndroid =
{
    {k = "lRobotScoreMin", t = "score"},                      
    {k = "lRobotScoreMax", t = "score"},                          
    {k = "lRobotBankGet", t = "score"},                            
    {k = "lRobotBankGetBanker", t = "score"},                         
    {k = "lRobotBankStoMul", t = "score"},                             
};

-- --游戏状态
cmd.CMD_S_StatusFree =
{
    --基础积分
    {k = "lCellScore", t = "score"},                                --基础积分
    {k = "lRoomStorageStart", t = "score"},                          --房间起始库存
    {k = "lRoomStorageCurrent", t = "score"},                        --房间当前库存
    {k = "CustomAndroid", t = "table", d = cmd.tagCustomAndroid},   --机器人配置
    {k = "szServerName", t = "string", s = 32},                     --房间名称

    {k = "wGamePlayerCountRule", t = "word"},                         --房间人数配置

    {t = "score",k="lInitScore"},                                 --初始分
    {t = "byte",k="cbMinLookCardTurnCount"},             --最小看牌轮数
    {t = "byte",k="cbMinCompareCardTurnCount"},      --最小比牌轮数

    {t="int",k="nButtonValue",l={4}},

    {t = "byte",k="cbPlayMode"},             --游戏模式
    {t = "byte",k="cbMaxTurnCount"},      --下注最大轮数
    {t = "byte",k="cbMinAllinTurnCount"}      --最小全压轮数
}

-- --游戏状态
cmd.CMD_S_StatusPlay =
{
    --加注信息
    {k = "lMaxCellScore", t = "score"},                                --单元上限
    {k = "lCellScore", t = "score"},                                    --单元下注
    {k = "lCurrentTimes", t = "score"},                                --当前倍数
    {k = "lUserMaxScore", t = "score"},                                --用户分数上限


    --状态信息
    {k = "wBankerUser", t = "word"},                                    --庄家用户
    {k = "wCurrentUser", t = "word"},                                   --当前用户
    {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}},            --游戏状态
    {k = "bMingZhu", t = "bool", l = {cmd.GAME_PLAYER}},                --看牌状态
    {k = "lTableScore", t = "score", l = {cmd.GAME_PLAYER}},            --下注数目  
    {k = "lRoomStorageStart", t = "score"},                                --房间起始库存 
    {k = "lRoomStorageCurrent", t = "score"},                                --房间当前库存 
    {k = "CustomAndroid", t = "table", d = cmd.tagCustomAndroid},   --机器人配置

    --扑克信息
    {k = "cbHandCardData", t = "byte", l = {cmd.MAX_COUNT}},            --扑克信息

    --状态信息
    {k = "bCompareState", t = "bool"},                                  --比牌状态
    {k = "szServerName", t = "string", s = 32},                           --房间名称

    {k = "wGamePlayerCountRule", t = "word"},                         --房间人数配置

    {t = "score",k="lInitScore"},                                --初始分
    {t = "byte",k="cbMinLookCardTurnCount"},             --最小看牌轮数
    {t = "byte",k="cbMinCompareCardTurnCount"},      --最小比牌轮数
    {t = "byte",k="cbCurTurnCount"},                         --当前轮数
    {t = "bool",k="bShowHand"},                                   --梭哈

    {t="int",k="nButtonValue",l={4}},

    {t = "byte",k="cbPlayMode"},             --游戏模式
    {t = "byte",k="cbMaxTurnCount"},      --下注最大轮数
    {t = "byte",k="cbMinAllinTurnCount"},      --最小全压轮数
    {k = "cbRemainingTime", t = "byte"},
};

--游戏开始
cmd.CMD_S_GameStart = 
{
    --下注信息
    {k = "lMaxCellScore", t = "score"},                                --单元上限
    {k = "lCellScore", t = "score"},                                    --单元下注
    {k = "lCurrentTimes", t = "score"},                                --当前倍数
    {k = "lUserMaxScore", t = "score"},                                --用户分数上限

    --状态信息
    {k = "wBankerUser", t = "word"},                                    --庄家用户
    {k = "wCurrentUser", t = "word"},                                   --当前用户
    {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}},            --游戏状态
};

cmd.CMD_S_TurnCount=
{
    {t = "byte",k="cbTurnCount"}                        --当前轮数 
}

--用户下注
cmd.CMD_S_AddScore = 
{
    {k = "wCurrentUser", t = "word"},                                   --当前用户
    {k = "wAddScoreUser", t = "word"},                                   --加注用户
    {k = "wCompareState", t = "byte"},                                   --比牌状态
    {k = "lAddScoreCount", t = "score"},                                --加注数目
    {k = "lCurrentTimes", t = "score"},                                    --当前倍数
    {k = "bShowHand",t="bool"},
};

--用户放弃
cmd.CMD_S_GiveUp =
{
    {k = "wGiveUpUser", t = "word"},                                   --放弃用户
};

--比牌数据包
cmd.CMD_S_CompareCard = 
{
    {k = "wCurrentUser", t = "word"},                                   --当前用户
    {k = "wCompareUser", t = "word", l = {2}},                          --比牌用户
    {k = "wLostUser", t = "word"},    
    {k="bShowHand",t="bool"}                                  --输牌用户
};

--看牌数据包
cmd.CMD_S_LookCard = 
{
    {k = "wLookCardUser", t = "word"},                                   --看牌用户
    {k = "cbCardData", t = "byte", l = {cmd.MAX_COUNT}},                 --用户扑克
};

--开牌数据包
cmd.CMD_S_OpenCard =
{
    {k = "wWinner", t = "word"},                                   --胜利用户
};

--游戏结束
cmd.CMD_S_GameEnd =
{
    {k = "lGameTax", t = "score"},                                       --游戏税收
    {k = "lGameScore", t = "score", l = {cmd.GAME_PLAYER}},              --游戏税收
    {k = "cbCardData", t = "byte", l = {3,3,3,3,3}},                     --用户扑克
    {k = "wCompareUser", t = "WORD", l = {4,4,4,4,4}},                   --比牌用户
    {k = "wEndState", t = "word"},                                       --结束状态
    {k = "bDelayOverGame", t = "bool"},                                  --延迟开始
    {k = "wServerType", t = "word"},                                      --房间类型
};


--用户退出
cmd.CMD_S_PlayerExit = 
{
    {k = "wPlayerID", t = "word"},                                      --退出用户
};

--等待比牌
cmd.CMD_S_WaitCompare = 
{
    {k = "wCompareUser", t = "word"},                                      --比牌用户
};




--房卡记录
cmd.CMD_S_RoomCardRecord = 
{
    {k = "nCount", t = "word"},                                      --局数
    {k = "lDetailScore", t = "score", l = {32, 32, 32, 32, 32}},        --单局结算分
    {k="cbCardTypeCount",t="byte",l={7,7,7,7,7}},
    {k="lAllScore",t="score",l={cmd.GAME_PLAYER}},
};
--****************定时器标识******************--
--开始定时器
cmd.IDI_START_GAME   			= 200
--加注定时器
cmd.IDI_USER_ADD_SCORE			= 201
--选比牌用户定时器
cmd.IDI_USER_COMPARE_CARD		= 202
--过滤定时器
cmd.IDI_DISABLE					= 203
--*****************时间标识*****************--
--开始定时器
cmd.TIME_START_GAME				= 30
--加注定时器
cmd.TIME_USER_ADD_SCORE			= 30
--比牌定时器
cmd.TIME_USER_COMPARE_CARD		= 30


-- 语音动画
cmd.VOICE_ANIMATION_KEY = "voice_ani_key"

return cmd