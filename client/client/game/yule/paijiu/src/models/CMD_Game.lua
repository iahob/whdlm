local cmd={}

--------------------------------------------------------------------------
--公共宏定义
cmd.KIND_ID				=			14									--游戏 I D
cmd.GAME_NAME			=			"牌九"					    --游戏名字
--cmd.GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	--游戏类型
cmd.GAME_PLAYER			=			6									--游戏人数
cmd.globalData={}
cmd.globalData.wPlayerCount=cmd.GAME_PLAYER

cmd.VERSION_SERVER		=	    	appdf.VersionValue(7,0,1)				--程序版本
cmd.VERSION_CLIENT		=		    appdf.VersionValue(7,0,1)				--程序版本

cmd.HAND_CARD_COUNT		=			2									--手牌数目

cmd.SERVER_LEN			=			32									--房间长度

--用户状态
cmd.PS_NONE            =             0                                   --用户状态
cmd.PS_SITTING          =            1                                   --用户状态   
cmd.PS_DYNAMIC        =              2                                   --用户状态   第一次进入桌子时若游戏已开始，则为该状态

cmd.GER_NORMAL =0
cmd.GER_DISMISS=1
cmd.GER_USER_LEAVE=2

-- 语音动画
cmd.VOICE_ANIMATION_KEY = "voice_ani_key"

cmd.GAME_STATUS_FREE=0
cmd.GAME_STATUS_PLAY=100

--游戏状态
cmd.GS_WK_FREE		=		    	cmd.GAME_STATUS_FREE					--等待开始
cmd.GS_WK_ROBBANKER	=			    cmd.GAME_STATUS_PLAY+1					--玩家抢庄
cmd.GS_WK_CHIP		=		    	cmd.GAME_STATUS_PLAY+2					--玩家下注
cmd.GS_WK_PLAYING	=		    	cmd.GAME_STATUS_PLAY+4					--游戏进行


cmd.actRob=1
cmd.actChip=2
cmd.actCombine=3

--六人场
cmd.MY_VIEWID = 1
cmd.RB_VIEWID=2 --right bottom
cmd.RT_VIEWID=3 --right top
cmd.TOP_VIEWID=4 --my对面
cmd.LT_VIEWID=5 --left top
cmd.LB_VIEWID=6 --left bottom

--四人场
cmd.FOUR_RIGHT_VIEWID=2
cmd.FOUR_TOP_VIEWID=3
cmd.FOUR_LEFT_VIEWID=4

--结构体定义

--------------------------------------------------------------------------
--服务器命令结构
cmd.SUB_S_GAME_START	=			101									--游戏开始
cmd.SUB_S_ROB_BANKER	=			102									--玩家抢庄
cmd.SUB_S_ROB_RESULT	=			104									--抢庄结果
cmd.SUB_S_CHIP			=			105									--玩家下注
cmd.SUB_S_CHIP_RESULT	=			106									--下注结果
cmd.SUB_S_OPEN			=			109									--玩家开牌
cmd.SUB_S_OPEN_RESULT	=			110									--开牌结果
cmd.SUB_S_GAME_END		=			112									--游戏结束

cmd.SUB_S_WINLOSE_INFO 	=			115									--总结算
cmd.SUB_S_QUERY_RESULT	=			116									--战绩流水查询结果

cmd.SUB_S_TRUSTEE	    =			124									--玩家托管
cmd.SUB_S_TRUSTEE_MODE	=			125									--托管模式
--------------------------------------------------------------------------

--玩家托管
cmd.CMD_S_Trustee=
{
	{t="word",k="wChairID"},                       --椅子ID
	{t="bool",k="bTrustee"},                       --是否托管
}

--托管模式
cmd.CMD_S_TrusteeMode=
{
	{t="byte",k="cbTrusteeMode"},                  --托管模式 0-下注最小筹码 1-下注最大筹码
}

cmd.CMD_S_WinLoseInfo=
{
	{t="int",k="WinInfo",l={3,3,3,3,3,3}}
}

cmd.Video_Start=
{
	{t="string",k="szNickName",s=yl.LEN_NICKNAME},			--用户昵称		
	{t="word",k="wChairID"},							--椅子ID

	{t="word",k="wBankerUser"},						--庄家椅号
	{t="score",k="lBankMoney"},							--坐庄钱数
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--游戏状态

	{t="score",k="lScore"},								--积分
	{t="int",k="lCellScore"},							--底分

	{t="bool",k="bFourCards"},							----大牌九
	{t="word",k="wPlayerCount"},						--玩家人数
}

--游戏开始
cmd.CMD_S_GameStart=
{
	{t="word",k="wBankerUser"},						--庄家椅号
	{t="score",k="lBankMoney"},							--坐庄钱数
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--游戏状态
}

--玩家抢庄
cmd.CMD_S_Rob_Banker=
{
	{t="word",k="wRobUser"},							--抢庄玩家
	{t="score",k="lMaxBankMoney"},						--最大钱数
	{t="word",k="wRobResult"}, --0,1,2   0没操作，1放弃，2抢庄 
}

--抢庄结果
cmd.CMD_S_Rob_Result=
{
	{t="word",k="wBankerUser"},						--庄家椅号
	{t="score",k="lBankMoney"},							--坐庄钱数
}

--玩家下注
cmd.CMD_S_Chip	= 									--通知可以下注了
{
	{t="word",k="wBankerUser"},						--庄家椅号	
	{t="score",k="lMaxChipMoney"},						--最大下注
	{t="score",k="lChipData",l={5}}, 
}


--下注结果
cmd.CMD_S_Chip_Result=
{
	{t="word",k="wChipUser"},							--下注玩家
	{t="score",k="lChip"},								--本次加注
}

--玩家发牌
cmd.CMD_S_Open=
{
	{t="byte",k="cbHandCard",l={cmd.HAND_CARD_COUNT*2}},	--玩家手牌
	{t="score",k="lUserChip",l={cmd.GAME_PLAYER}},--玩家下注
	{t="score",k="lSiceCount"},							--色子信息
	{t="bool",k="bReShuffleCard"}, 
	{t="byte",k="cbTableCardCount"},--桌子牌数是当局发完牌后剩余的数目
	{t="byte",k="cbSendCardCount"} --发牌数目就是单局发的牌数
}

--玩家发牌、回放、加了一个玩家chiarid
cmd.Video_Open=
{
	{t="word",k="wChairID"},							--玩家
	{t="byte",k="cbHandCard",l={cmd.HAND_CARD_COUNT*2}},	--玩家手牌
	{t="score",k="lUserChip",l={cmd.GAME_PLAYER}},--玩家下注
	{t="score",k="lSiceCount"},							--色子信息
	{t="bool",k="bReShuffleCard"}, 
	{t="byte",k="cbTableCardCount"},--桌子牌数是当局发完牌后剩余的数目
	{t="byte",k="cbSendCardCount"} --发牌数目就是单局发的牌数
}

--开牌结果
cmd.CMD_S_Open_Result=
{
	{t="word",k="wOpenUser"},							--开牌玩家
}

--游戏结束
cmd.CMD_S_GameEnd=
{
	{t="score",k="lGameTax"},							--游戏税收
	{t="score",k="lGameScore",l={cmd.GAME_PLAYER}},			--游戏积分
	{t="byte",k="bEndMode"},							--结束方式

	{t="int",k="nUsedCardNum"},
	{t="word",k="wBankerUser"},
	{t="score",k="lBankMoney"},
	{t="byte",k="cbHandCard",l={4,4,4,4,4,4}},
	{t="bool",k="bCompareInfo",l={2,2,2,2,2,2}}
}

cmd.CMD_S_QueryResult=
{
	{t="score",k="UserInfo",l={cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER}},				--胜负信息
	{t="int",k="nPlatCountBegin"},						--局数
	{t="int",k="nPlatCountEnd"},							--局数
};


--断线重连
--空闲状态
cmd.CMD_S_StatusFree=
{
	--游戏变量
	{t="score",k="lBackChip"},							--游戏底注

	--历史成绩
	{t="score",k="lTurnScore",l={cmd.GAME_PLAYER}},			--上局成绩
	{t="score",k="lCollectScore",l={cmd.GAME_PLAYER}},			--总局成绩

	--房间信息
	{t="string",k="szServerName",s=cmd.SERVER_LEN},			--房间名称
	{t="bool",k="bFourCards"},
	{t="word",k="wPlayerCount"},

	{t="byte",k="cbTimeStartGame"},
	{t="byte",k="cbTimeCallBanker"},
	{t="byte",k="cbTimeSetChip"},
	{t="byte",k="cbTimeRangeCard"},
	{t="byte",k="cbTimeShowCard"},

    {t="score",k="lRobotScoreMin"},	
    {t="score",k="lRobotScoreMax"},	
    {t="score",k="lRobotBankGet"},	
    {t="score",k="lRobotBankGetBanker"},	
    {t="score",k="lRobotBankStoMul"},	

    --类型 0积分场， 1金币场
    {t = "byte", k = "cbPlayMode"},					                    --约战房游戏模式

    {t="byte",k="cbTrusteeMode"},                  --托管模式 0-下注最小筹码 1-下注最大筹码
}

--抢庄状态
cmd.CMD_S_StatusRob=
{
	--游戏变量
	{t="score",k="lBackChip"},							--游戏底注
	{t="word",k="wCurRobber"},							--抢庄玩家
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--用户状态

	--历史成绩
	{t="score",k="lTurnScore",l={cmd.GAME_PLAYER}},			--上局成绩
	{t="score",k="lCollectScore",l={cmd.GAME_PLAYER}},			--总局成绩

	--房间信息
	{t="string",k="szServerName",s=cmd.SERVER_LEN},			--房间名称
	{t="bool",k="bFourCards"},
	{t="word",k="wPlayerCount"},
	{t="byte",k="bRobBanker",l={cmd.GAME_PLAYER}}, --0表示没抢或者正在抢 1放弃 2抢

	{t="byte",k="cbTimeStartGame"},
	{t="byte",k="cbTimeCallBanker"},
	{t="byte",k="cbTimeSetChip"},
	{t="byte",k="cbTimeRangeCard"},
	{t="byte",k="cbTimeShowCard"},

    {t="score",k="lRobotScoreMin"},	
    {t="score",k="lRobotScoreMax"},	
    {t="score",k="lRobotBankGet"},	
    {t="score",k="lRobotBankGetBanker"},	
    {t="score",k="lRobotBankStoMul"},
    --类型 0积分场， 1金币场
    {t = "byte", k = "cbPlayMode"},					                    --约战房游戏模式

    {t = "byte", k = "cbTimeRemain"},					                 --重连剩余秒数

    {t="byte",k="cbTrusteeMode"},                  --托管模式 0-下注最小筹码 1-下注最大筹码
}

--下注状态
cmd.CMD_S_StatusChip=
{
	--游戏变量
	{t="score",k="lBackChip"},							--游戏底注
	{t="word",k="wBankerUser"},						--庄家位置
	{t="score",k="lBankerMoney"},						--坐庄金币
	{t="score",k="lChipLimit",l={2}},						--下注限制
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--用户状态

	--历史成绩
	{t="score",k="lTurnScore",l={cmd.GAME_PLAYER}},			--上局成绩
	{t="score",k="lCollectScore",l={cmd.GAME_PLAYER}},			--总局成绩

	--下注信息
	{t="bool",k="bHaveChip",l={cmd.GAME_PLAYER}},				--是否下注
	{t="score",k="lUserChip",l={cmd.GAME_PLAYER}},--玩家下注
	{t="score",k="lChipData",l={5}},

	--房间信息
	{t="string",k="szServerName",s=cmd.SERVER_LEN},			--房间名称
	{t="bool",k="bFourCards"},
	{t="word",k="wPlayerCount"},

	{t="byte",k="cbTimeStartGame"},
	{t="byte",k="cbTimeCallBanker"},
	{t="byte",k="cbTimeSetChip"},
	{t="byte",k="cbTimeRangeCard"},
	{t="byte",k="cbTimeShowCard"},

    {t="score",k="lRobotScoreMin"},	
    {t="score",k="lRobotScoreMax"},	
    {t="score",k="lRobotBankGet"},	
    {t="score",k="lRobotBankGetBanker"},	
    {t="score",k="lRobotBankStoMul"},
    --类型 0积分场， 1金币场
    {t = "byte", k = "cbPlayMode"},					                    --约战房游戏模式

    {t = "byte", k = "cbTimeRemain"},					                 --重连剩余秒数

    {t="byte",k="cbTrusteeMode"},                  --托管模式 0-下注最小筹码 1-下注最大筹码
}

--开牌状态
cmd.CMD_S_StatusOpen=
{
	--游戏变量
	{t="score",k="lBackChip"},							--游戏底注
	{t="word",k="wBankerUser"},						--庄家位置
	{t="score",k="lBankerMoney"},						--坐庄金币
	{t="score",k="lUserChip",l={cmd.GAME_PLAYER}},--玩家下注
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--用户状态

	--历史成绩
	{t="score",k="lTurnScore",l={cmd.GAME_PLAYER}},			--上局成绩
	{t="score",k="lCollectScore",l={cmd.GAME_PLAYER}},			--总局成绩

	--开牌信息
	{t="bool",k="bHaveOpen",l={cmd.GAME_PLAYER}},				--是否开牌
	{t="byte",k="cbHandCard",l={4,4,4,4,4,4}},--手牌信息
	{t="score",k="lSiceCount"},							--色子信息

	--房间信息
	{t="string",k="szServerName",s=cmd.SERVER_LEN},			--房间名称
	{t="bool",k="bFourCards"},
	{t="word",k="wPlayerCount"},
	{t="byte",k="cbTableCardCount"},

	{t="byte",k="cbTimeStartGame"},
	{t="byte",k="cbTimeCallBanker"},
	{t="byte",k="cbTimeSetChip"},
	{t="byte",k="cbTimeRangeCard"},
	{t="byte",k="cbTimeShowCard"},

    {t="score",k="lRobotScoreMin"},	
    {t="score",k="lRobotScoreMax"},	
    {t="score",k="lRobotBankGet"},	
    {t="score",k="lRobotBankGetBanker"},	
    {t="score",k="lRobotBankStoMul"},

    --类型 0积分场， 1金币场
    {t = "byte", k = "cbPlayMode"},					                    --约战房游戏模式

    {t = "byte", k = "cbTimeRemain"},					                 --重连剩余秒数

    {t="byte",k="cbTrusteeMode"},                  --托管模式 0-下注最小筹码 1-下注最大筹码
}


--------------------------------------------------------------------------
--客户端命令结构
cmd.SUB_C_ROB_BANKER	=			300									--玩家抢庄
cmd.SUB_C_CHIP			=			301									--玩家下注
cmd.SUB_C_OPEN			=			304									--玩家开牌
cmd.SUB_C_TRUSTEE		=			305									--玩家托管

cmd.SUB_C_QUERY_WINLOST	=			307									--查询信息
cmd.SUB_C_TRUSTEE_MODE	=			308									--托管模式
--------------------------------------------------------------------------

--玩家托管
cmd.CMD_C_Trustee=
{
	{t="bool",k="bTrustee"},                           --是否托管
}

--托管模式
cmd.CMD_C_TrusteeMode=
{
	{t="byte",k="cbTrusteeMode"},                      --托管模式 0-下注最小筹码 1-下注最大筹码
}

--玩家抢庄
cmd.CMD_C_Rob_Banker=
{
	{t="bool",k="bRobBanker"},							--是否抢庄
}

--玩家下注
cmd.CMD_C_Chip=
{
	{t="word",k="wUserID"},							--玩家椅号
	{t="score",k="lChip"},					--下注额度
}

cmd.CMD_C_Open=
{
	{t="byte",k="bCardData",l={4}}
}

cmd.CMD_C_QueryInfo=
{
	{t="int",k="nPlatCountBegin"},					--局数
	{t="int",k="nPlatCountEnd"}						--局数
};


-------------------------------------------
--增加金币房卡流水
cmd.MAX_RECORD_COUNT			=	30									--房卡结算统计最大局数
cmd.SUB_C_REQUEST_RCRecord		=	12									--请求房卡记录
cmd.SUB_S_RECORD				=	123									--游戏记录
cmd.CMD_S_RECORD =
{
    {t="int",k="nCount"},
    {t="score",k="lAllScore",l={cmd.GAME_PLAYER}},          --总结算分
    {t="score",k="lDetailScore",l={cmd.MAX_RECORD_COUNT,cmd.MAX_RECORD_COUNT,cmd.MAX_RECORD_COUNT,cmd.MAX_RECORD_COUNT,cmd.MAX_RECORD_COUNT,cmd.MAX_RECORD_COUNT}},       --单局结算分
};

return cmd
