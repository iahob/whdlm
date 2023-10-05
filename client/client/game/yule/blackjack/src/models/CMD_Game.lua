--[[
for j = 1, 5 do
            self.userCard[i].card[j] = display.newSprite(cmd.RES.."game/poker/card_back.png")
                    :move(ptCard[i].x + (i==cmd.MY_VIEWID and mySpacingS or otherSpacingS)*(j - 1), ptCard[i].y)
                    :setVisible(false)
                    :addTo(self.userCard[i].area)
            if i == cmd.MY_VIEWID then
                self.userCard[i].card[j]:setScale(0.8)
            else
                self.userCard[i].card[j]:setScale(0.45)
            end
        end
]]


local cmd =  {}

cmd.RES 						= "game/yule/blackjack/res/"
--游戏版本
cmd.VERSION 					= appdf.VersionValue(6,7,0,1)
--游戏标识
cmd.KIND_ID						= 241
	
--游戏人数
cmd.GAME_PLAYER					= 6

--视图位置
cmd.MY_VIEWID					= 1

--空闲状态
cmd.GAME_STATUS_FREE	       = 0
--下注状态
cmd.GAME_ADD_SCORE			   = 100
--游戏状态
cmd.GAME_STATUS_GAME           = 101

--***************服务端命令结构**************--
--游戏开始
cmd.SUB_S_GAME_START			= 100
--游戏结束
cmd.SUB_S_GAME_END              = 101
--发牌消息
cmd.SUB_S_SEND_CARD             = 102
--分牌
cmd.SUB_S_SPLIT_CARD            = 103                                 
--停牌
cmd.SUB_S_STOP_CARD				= 104
--加倍
cmd.SUB_S_DOUBLE_SCORE			= 105
--//保险
cmd.SUB_S_INSURE                =106  
--//下注                               
cmd.SUB_S_ADD_SCORE             =107  
--//要牌                               
cmd.SUB_S_GET_CARD              =108   
--//发送明牌                             
cmd.SUB_S_CHEAT_CARD            =109   
--//游戏记录
cmd.SUB_S_RECORD                =118                                 

                            




cmd.SUB_S_ROOMCARD_RECORD           =123                                 --房卡记录



--***************客户端命令结构**************--
--//用户加注
cmd.SUB_C_ADD_SCORE                 =1   
--//要牌//要牌                                
cmd.SUB_C_GET_CARD                  =2   
--//加倍                                
cmd.SUB_C_DOUBLE_SCORE              =3                                   
--//保险
cmd.SUB_C_INSURE                    =4                                   
--//分牌
cmd.SUB_C_SPLIT_CARD                =5                                  
--//放弃跟注 
cmd.SUB_C_STOP_CARD                 =6                                   

--公共加密原文长度
cmd.AESENCRYPTION_LENGTH        = 16
--最大牌数
cmd.MAX_COUNT                   = 5

------------------------------------------------消息结构体----------------------------------
--机器人存款取款
cmd.tagCustomAndroid =
{
    {k = "lRobotScoreMin", t = "score"},                      
    {k = "lRobotScoreMax", t = "score"},                          
    {k = "lRobotBankGet", t = "score"},                            
    {k = "lRobotBankGetBanker", t = "score"},                         
    {k = "lRobotBankStoMul", t = "score"},                             
}

cmd.CMD_S_RECORD = 
{
    {t = "int",     k = "iCount",       l={cmd.GAME_PLAYER}},   --牌数量
    {t = "score",   k = "lDetailScore", l={32,32,32,32,32,32}},                    --单局结算分
    {t = "score",   k = "lAllScore",    l = {cmd.GAME_PLAYER}},                                        --总结算分
}  

-- --游戏状态
cmd.CMD_S_StatusFree =
{
    --时间定义
    {k="cbTimeAddScore" , t = "byte"},      --下注时间
    {k="cbTimeOpertion" , t = "byte"},      --操作时间

    {k = "cbBankerMode" , t = "byte"},      --庄家模式  
    {k = "lBaseJeton", t = "score"},                          --单元筹码
    {k = "wPlayerCount", t = "word"},                         --房间人数配置
    --基础积分
    {k = "lRoomStorageStart", t = "score"},                          --房间起始库存
    {k = "lRoomStorageCurrent", t = "score"},                        --房间当前库存
    {k = "CustomAndroid", t = "table", d = cmd.tagCustomAndroid},   --机器人配置
    {k = "cbPriMode" , t = "byte"},      --约战模式
    
}

-- --下注状态
cmd.CMD_S_StatusAddScore =
{
    --时间定义
    {k="cbTimeAddScore" , t = "byte"},      --下注时间
    {k="cbTimeOpertion" , t = "byte"},      --操作时间
    {k="cbTimeRemain" , t = "byte"},      --剩余操作时间

    {k = "cbBankerMode" , t = "byte"},      --庄家模式  
    {k = "lBaseJeton", t = "score"},                          --单元筹码
    {k = "wPlayerCount", t = "word"},                         --房间人数配置

    {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}},  --玩家状态
    --加注信息
    {k = "lMaxCellScore", t = "score"},                                --单元上限
    --状态信息
    {k = "wBankerUser", t = "word"},                                    --庄家用户

    {k = "lTableScore", t = "score", l = {cmd.GAME_PLAYER}},            --下注数目  
    {k = "lRoomStorageStart", t = "score"},                                --房间起始库存 
    {k = "lRoomStorageCurrent", t = "score"},                                --房间当前库存 
    {k = "CustomAndroid", t = "table", d = cmd.tagCustomAndroid},   --机器人配置 
    {k = "cbPriMode" , t = "byte"},      --约战模式
    
};

-- --游戏状态
cmd.CMD_S_StatusGame =
{
     --时间定义
    {k="cbTimeAddScore" , t = "byte"},      --下注时间
    {k="cbTimeOpertion" , t = "byte"},      --操作时间
    {k="cbTimeRemain" , t = "byte"},      --剩余操作时间

    {k = "cbBankerMode" , t = "byte"},      --庄家模式  
    {k = "lBaseJeton", t = "score"},                          --单元筹码
    {k = "wPlayerCount", t = "word"},                         --房间人数配置
--游戏状态
    {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}},  --玩家状态

    --加注信息
    {k = "wBankerUser", t = "word"},                                    --庄家用户
    {k = "wCurrentUser", t = "word"},                                    --当前玩家
    {k = "lTableScore", t = "score", l = {cmd.GAME_PLAYER}},            --下注数目  wCurrentUser


    --状态信息
    {k = "cbCardCount", t = "byte",l={cmd.GAME_PLAYER*2}},   --牌数量
    {k = "cbHandCardData", t = "byte",l={11,11,11,11,11,11,11,11,11,11,11,11}},       --牌数据

    {k = "bStopCard", t = "bool",l={cmd.GAME_PLAYER*2}},   --牌数量
    {k = "bDoubleCard", t = "bool",l={cmd.GAME_PLAYER*2}},   --牌数量
    {k = "bInsureCard", t = "bool",l={cmd.GAME_PLAYER*2}},   --牌数量
    
    {k = "lRoomStorageStart", t = "score"},                                --房间起始库存 
    {k = "lRoomStorageCurrent", t = "score"},                                --房间当前库存 
    {k = "CustomAndroid", t = "table", d = cmd.tagCustomAndroid},   --机器人配置
    {k = "cbPriMode" , t = "byte"},      --约战模式
  

    
};

--游戏开始
cmd.CMD_S_GameStart = 
{
    {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}},  --玩家状态
    --下注信息
    {k = "lCellScore", t = "score"},                                    --单元下注
    {k = "lMaxCellScore", t = "score"},                                --单元上限 没用的字段

    --状态信息
    {k = "wBankerUser", t = "word"},                                    --庄家用户
};
--下注
cmd.CMD_S_AddScore=
{
    {t = "word",k="wAddScoreUser"},                        --下注玩家
    {k = "lAddScore", t = "score"},                                --下多少
}

--腰牌
cmd.CMD_S_GetCard=
{
    {t = "word",k = "wGetCardUser"},                        --下注玩家
    {t = "byte",k = "cbCardData"},                                --下多少
    {t = "bool",k = "bSysGet"},                                --下多少
}

--发牌
cmd.CMD_S_SendCard=
{
    {k = "cbHandCardData", t = "byte",l={2,2,2,2,2,2}},   --牌
    {t = "bool",k = "bWin"},                                --是否赢派
    {k = "wCurrentUser", t = "word"},                                    --当前玩家
}

--分牌
cmd.CMD_S_SplitCard=
{
    {t = "word",k = "wSplitUser"},                                --分牌玩家
    {t = "byte",k = "bInsured"},                                --之前是否下了保险
    {k = "lAddScore", t = "score"},                                --下多少
    {k = "cbCardData", t = "byte",l={2}},   --牌
}

--停牌
cmd.CMD_S_StopCard=
{
    {t = "word",k = "wStopCardUser"},                        --停牌玩家
    {k = "wCurrentUser", t = "word"},                                    --当前玩家
}

--加倍
cmd.CMD_S_DoubleScore=
{
    {t = "word",k = "wDoubleScoreUser"},                                --加倍玩家
    {t = "byte",k = "cbCardData"},                                --之前是否下了保险
    {k = "lAddScore", t = "score"},                                --下多少
}

--保险
cmd.CMD_S_Insure=
{
    {t = "word",k = "wInsureUser"},                                --保险玩家
    {k = "lInsureScore", t = "double"},                                --保险金
}





--游戏结束
cmd.CMD_S_GameEnd =
{
    {k = "lGameTax", t = "score", l = {cmd.GAME_PLAYER}},                                       --游戏税收
    {k = "lGameScore", t = "score", l = {cmd.GAME_PLAYER}},              --游戏税收
    {k = "cbCardData", t = "byte", l={11,11,11,11,11,11,11,11,11,11,11,11}},                     --用户扑克
    
};








--****************定时器标识******************--
--开始定时器
cmd.IDI_START_GAME   			= 200
--加注定时器
cmd.IDI_USER_ADD_SCORE			= 201
--选比牌用户定时器
cmd.IDI_USER_OPERTION_CARD		= 202
--*****************时间标识*****************--
--开始定时器
cmd.TIME_START_GAME				= 20
--加注定时器
cmd.TIME_USER_ADD_SCORE			= 25
--操作定时器
cmd.TIME_USER_OPERATION		= 25


-- 语音动画
cmd.VOICE_ANIMATION_KEY = "voice_ani_key"

return cmd