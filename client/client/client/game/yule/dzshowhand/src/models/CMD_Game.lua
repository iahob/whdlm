
local cmd = {}
--公共宏定义
cmd.VERSION_SERVER		=	    	appdf.VersionValue(6,7,0,1)				    --程序版本
cmd.VERSION_CLIENT		=		    appdf.VersionValue(6,7,0,1)				    --程序版本

cmd.KIND_ID							= 3									--游戏 I D
cmd.GAME_PLAYER						= 9									--游戏人数
cmd.GAME_NAME						= "德州扑克"						--游戏名字

--游戏状态
cmd.GS_FREE							= 0									--空闲状态
cmd.GS_PLAY							= 100								--游戏状态
cmd.GS_END							= 101								--结算状态

--结束原因
cmd.GER_NO_PLAYER					= 0x10								--没有玩家

--数目定义
cmd.FULL_COUNT					    = 52								--全牌数目
cmd.MAX_COUNT						= 2									--最大数目
cmd.MAX_CENTER_COUNT				= 5									--最大数目
cmd.SCORE_ZERO						= 0

cmd.JETTON_COUNT					= 10
cmd.MY_VIEW_INDEX					= 1									--自己视图

--数值掩码
cmd.CARD_MASK_COLOR					= 0xF0								--花色掩码
cmd.CARD_MASK_VALUE					= 0x0F								--数值掩码

--下注类型 1加注 2跟注 3弃牌 4梭哈 5过牌
cmd.CHIP_ADD						= 1
cmd.CHIP_FOLLOW						= 2
cmd.CHIP_GIVEUP						= 3
cmd.CHIP_ALLIN						= 4
cmd.CHIP_PASS						= 5

cmd.INT_MAX							= 2147483647

-- 语音动画
cmd.VOICE_ANIMATION_KEY = "voice_ani_key"

cmd.RES_PATH 			= "game/yule/dzshowhand/res/"


--扑克类型
cmd.CT_SINGLE					= 1									--单牌类型
cmd.CT_ONE_LONG					= 2									--对子类型
cmd.CT_TWO_LONG					= 3									--两对类型
cmd.CT_THREE_TIAO				= 4									--三条类型
cmd.CT_SHUN_ZI					= 5									--顺子类型
cmd.CT_TONG_HUA					= 6									--同花类型
cmd.CT_HU_LU					= 7									--葫芦类型
cmd.CT_TIE_ZHI					= 8									--铁支类型
cmd.CT_TONG_HUA_SHUN			= 9									--同花顺型
cmd.CT_KING_TONG_HUA_SHUN		= 10								--皇家同花顺

cmd.CardType = 
{
 "高牌",
 "对子",
 "两对",
 "三条",
 "顺子",
 "同花",
 "葫芦",
 "四条",
 "同花顺",
 "皇家同花顺"
}

--------------------------------------------------------------------------
--服务器命令结构
cmd.SUB_S_GAME_START				= 100									--游戏开始
cmd.SUB_S_ADD_SCORE					= 101									--加注结果
cmd.SUB_S_GIVE_UP					= 102									--放弃跟注	
cmd.SUB_S_SEND_CARD					= 103									--发牌消息
cmd.SUB_S_GAME_CONCLUDE				= 104									--游戏结束
cmd.SUB_S_SIT_DOWN					= 105									--用户坐下
cmd.SUB_S_GAME_FREE					= 106									--游戏结束
cmd.SUB_S_SET_CHIP					= 107									--设置筹码
cmd.SUB_S_SHOW_CARD					= 108									--用户亮牌
cmd.SUB_S_STAND_UP					= 109									--用户站起

cmd.SUB_S_CHEAT_CARD				= 110									--特殊命令
cmd.SUB_S_SWITCH_RESULT				= 120									--换牌结果
cmd.SUB_S_EX_USER_RECORD			= 135									--用户记录
cmd.SUB_S_RECORD					= 136									--私人房记录

cmd.SUB_S_POOL_INFO					= 137									--池子信息

cmd.SUB_S_REST_SCORE				= 138									--可用加注额
cmd.SUB_S_LAST_RECORD				= 139									--上局信息
cmd.SUB_S_GAME_OVER					= 140									--游戏结束

--------------------------------------------------------------------------
--特殊命令
cmd.CMD_S_CheatCard = 
{
	{k="bFresh",t="bool"},
	{k="cbCenterCardData",t="byte",l={cmd.MAX_CENTER_COUNT}},--中心扑克
	{k="cbHandCardData",t="byte",l={cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT}} --手上扑克
}


--空闲状态
cmd.CMD_S_StatusFree = 
{
	--时间信息
	{k="cbStartTime",t="byte"},					--开时时间
	{k="cbBetTime",t="byte"},					--下注时间
	{k="cbEndTime",t="byte"},					--结算时间
	{k="lCellScore",t="score"},					--盲注分数
	{k="lAnteScore",t="score"},					--固定底注
	{k="lMinBetScore",t="score"},				--最低携带金币
	{k="lMaxBetScore",t="score"},				--最高携带金币
	{k="lAllBetScore",t="score"},				--累计带入限制
	{k="lUserMaxScore",t="score",l={cmd.GAME_PLAYER}}, --玩家分数	
	{k="cbTime",t="byte"},						--开房时长0.5小时倍数
	{k="nLeftSecond",t="int"},					--剩余时间
	{k="cbMaxPlayerCount",t="byte"},				--最大人数
	{k="cbPlayMode",t="byte"},				--最大人数
}

--游戏状态
cmd.CMD_S_StatusPlay =
{
	--时间信息
	{k="cbStartTime",t="byte"},					--开时时间
	{k="cbBetTime",t="byte"},					--下注时间
	{k="cbEndTime",t="byte"},					--结算时间
	{k="lCellScore",t="score"},					--盲注分数
	{k="lAnteScore",t="score"},					--固定底注
	{k="lMinBetScore",t="score"},				--最低携带金币
	{k="lMaxBetScore",t="score"},				--最高携带金币
	{k="lAllBetScore",t="score"},				--累计带入限制

	{k="lTurnMaxScore",t="score"},				--最大下注
	{k="lTurnLessScore",t="score"},				--最小下注
	{k="lCellMaxScore",t="score"},				--最大下注
	{k="lAddLessScore",t="score"},				--加最小注
	{k="lTableScore",t="score",l={cmd.GAME_PLAYER}},--下注数目
	{k="lTotalScore",t="score",l={cmd.GAME_PLAYER}},--累计下注
	{k="lUserMaxScore",t="score",l={cmd.GAME_PLAYER}},--玩家分数
	{k="lCenterScore",t="score"},				--中心筹码
	{k="cbTime",t="byte"},		 --开房时长0.5小时倍数
	{k="nLeftSecond",t="int"},	 --剩余时间

	--状态信息
	{k="bAutoChip",t="bool"},					--自动兑换
	{k="wDUser",t="word"},						--D玩家
	{k="wMaxChipInUser",t="word"},				--大盲注玩家	
	{k="wMinChipInUser",t="word"},				--小盲注玩家	
	{k="wCurrentUser",t="word"},				--当前玩家
	{k="cbPlayStatus",t="byte",l={cmd.GAME_PLAYER}},--游戏状态
	{k="cbPlayStatusRecord",t="byte",l={cmd.GAME_PLAYER}},--游戏状态
	{k="cbPlayDynamicJoin",t="byte",l={cmd.GAME_PLAYER}},--游戏状态
	{k="bStandUp",t="bool",l={cmd.GAME_PLAYER}},--玩家站起
	{k="cbBalanceCount",t="byte"},				--平衡次数

	--扑克信息
	{k="cbCenterCardData",t="byte",l={cmd.MAX_CENTER_COUNT}},--中心扑克
	{k="cbHandCardData",t="byte",l={cmd.MAX_COUNT}},--桌面扑克
	{k="cbMaxPlayerCount",t="byte"},				--最大人数

	{k="cbCardType",t="byte"},	--发牌后的牌型
	{k="cbPoolCount",t="byte"},	--底池个数
	{k="lPoolScore",t="score",l={cmd.GAME_PLAYER}},--奖池分数	
	{k="cbPlayMode",t="byte"},				--约战模式
	{k="cbTimeLeave",t="byte"},				--剩余操作时间
	{k="bGiveUp",t="bool",l={cmd.GAME_PLAYER}},--弃牌状态
}

--游戏状态
cmd.CMD_S_StatusEnd =
{
	--时间信息
	{k="cbStartTime",t="byte"},--开时时间
	{k="cbBetTime",t="byte"},--下注时间
	{k="cbEndTime",t="byte"},--结算时间

	--加注信息
	{k="lCellScore",t="score"},--盲注分数
	{k="lMinBetScore",t="score"},--最低携带金币
	{k="lMaxBetScore",t="score"},--最高携带金币

	{k="cbTime",t="byte"},		 --开房时长
	{k="nLeftSecond",t="int"},	 --剩余时间

	{k="lTableScore",t="score",l={cmd.GAME_PLAYER}},--下注数目
	{k="lTotalScore",t="score",l={cmd.GAME_PLAYER}},--累计下注
	{k="lUserMaxScore",t="score",l={cmd.GAME_PLAYER}},--玩家分数
	{k="lCenterScore",t="score"},--中心筹码

	--状态信息
	{k="bAutoChip",t="bool"},--自动兑换
	{k="wDUser",t="word"},--D玩家
	{k="wCurrentUser",t="word"},--当前玩家
	{k="cbPlayStatus",t="byte",l={cmd.GAME_PLAYER}},--游戏状态
	{k="cbPlayStatusRecord",t="byte",l={cmd.GAME_PLAYER}},--游戏状态
	{k="cbPlayDynamicJoin",t="byte",l={cmd.GAME_PLAYER}},--游戏状态
	{k="bStandUp",t="bool",l={cmd.GAME_PLAYER}},--玩家站起
	{k="cbBalanceCount",t="byte"},--平衡次数
						
	--扑克信息
	{k="cbCenterCardData",t="byte",l={cmd.MAX_CENTER_COUNT}},--扑克数目
	
	{k="cbTotalEnd",t="byte"},--强退标志
	{k="cbDelayTime",t="byte"},--延时时间
	{k="lGameTax",t="score",l={cmd.GAME_PLAYER}},--游戏税收
	{k="lGameScore",t="score",l={cmd.GAME_PLAYER}},--游戏得分
	--用户扑克
	{k="cbCardData",t="byte",l={cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT}},
	--最后扑克
	{k="cbLastCenterCardData",t="byte",l={cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT}},
	--最后牌型
	{k="cbLastCardType",t="byte",l={cmd.GAME_PLAYER}},

	--奖池个数
	{k="cbPoolCount",t="byte"},
	{k="lPoolScore",t="score",l={cmd.GAME_PLAYER}},--奖池分数	
	{k="cbPoolWinnerCount",t="byte",l={cmd.GAME_PLAYER}},--池子赢家个数	
	{k="wPoolWinner",t="word",l={cmd.GAME_PLAYER}},--池子赢家座位号
	{k="lUserMaxScore",t="score",l={cmd.GAME_PLAYER}},--本局结算后的总携带分
}


--设置筹码
cmd.CMD_S_SetChip = 
{
	{k="wChipUser",t="word"},	--当前用户
	{k="lChipScore",t="score"},	--筹码数目						
}

--约占记录
cmd.CMD_S_Record =
{
	{k="nCount",t="int"},
	{k="wMaxLoser",t="word"},
	{k="wMaxTaker",t="word"},
	{k="wMvp",t="word"},
	{k="lAllScore",t="score",l={cmd.GAME_PLAYER}},		--总结算
	{k="lTakeScore",t="score",l={cmd.GAME_PLAYER}},		--总带入分
}
 
--发送扑克
cmd.CMD_S_GameStart =
{

	{k="wCurrentUser",t="word"},--当前玩家
	{k="wDUser",t="word"},--D玩家
	{k="wMaxChipInUser",t="word"},--大盲注玩家	
	{k="cbDynamicJoinCount",t="byte"},--动态加入玩家数量
	{k="wDynamicJoinUser",t="word",l={cmd.GAME_PLAYER}},--动态加入罚盲玩家
	{k="wMinChipInUser",t="word"},--小盲注玩家	
	{k="lTurnMaxScore",t="score"},				--最大下注
	{k="lTurnLessScore",t="score"},				--最小下注
	{k="lAddLessScore",t="score"},				--最大下注
	{k="lUserMaxScore",t="score",l={cmd.GAME_PLAYER}},--玩家分数
	{k="cbCardData",t="byte",l={cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT}},--用户扑克
	{k="cbPlayStatus",t="byte",l={cmd.GAME_PLAYER}},--用户状态
	{k="bStandUp",t="bool",l={cmd.GAME_PLAYER}},--玩家站起
	{k="lUserTableScore",t="score",l={cmd.GAME_PLAYER}},--玩家已下注分
}


--用户下注
cmd.CMD_S_AddScore =
{
	{k="wCurrentUser",t="word"},--当前用户
	{k="wAddScoreUser",t="word"},--加注用户
	{k="lAddScoreCount",t="score"},--加注数目
	{k="lTurnLessScore",t="score"},--最少加注
	{k="lTurnMaxScore",t="score"},--最大下注
	{k="lAddLessScore",t="score"} --加最小注

}

--用户放弃
cmd.CMD_S_GiveUp =
{
	{k="wGiveUpUser",t="word"},--放弃用户
	{k="lLostScore",t="score"},--输掉金币	
	{k="lTableScore",t="score"}
}

--发牌数据包
cmd.CMD_S_SendCard =
{
	{k="cbPublic",t="byte"},--是否公牌
	{k="wCurrentUser",t="word"},--当前用户
	{k="lTurnLessScore",t="score"},--最少加注
	{k="lTurnMaxScore",t="score"},--最大下注
	{k="lAddLessScore",t="score"},--加最小注
	{k="cbSendCardCount",t="byte"},--发牌数目
	{k="cbCenterCardData",t="byte",l={cmd.MAX_CENTER_COUNT}}, --中心扑克	
	{k="cbCardType",t="byte"},	--发牌后的牌型
}


--游戏结束
cmd.CMD_S_GameConclude =
{
	{k="cbTotalEnd",t="byte"},--强退标志
	{k="cbDelayTime",t="byte"},--延时时间
	{k="lGameTax",t="score",l={cmd.GAME_PLAYER}},--游戏税收
	{k="lGameScore",t="score",l={cmd.GAME_PLAYER}},--游戏得分
	--用户扑克
	{k="cbCardData",t="byte",l={cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT}},
	--最后扑克
	{k="cbLastCenterCardData",t="byte",l={cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT}},
	--最后牌型
	{k="cbLastCardType",t="byte",l={cmd.GAME_PLAYER}},

	--奖池个数
	{k="cbPoolCount",t="byte"}, 
	{k="lPoolScore",t="score",l={cmd.GAME_PLAYER}},--奖池分数	
	{k="cbPoolWinnerCount",t="byte",l={cmd.GAME_PLAYER}},--池子赢家个数	
	{k="wPoolWinner",t="word",l={cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER}},--池子赢家座位号
	{k="lUserMaxScore",t="score",l={cmd.GAME_PLAYER}},--本局结算后的总携带分 
}

--游戏空闭
cmd.CMD_S_GameFree =
{
	{k="bAutoStart",t="bool"},--自动开始
	{k="lUserMaxScore",t="score",l={cmd.GAME_PLAYER}}--玩家分数	
}


--用户亮牌
cmd.CMD_S_ShowCard =
{
	{k="wShowUser",t="word"},--亮牌用户
	{k="cbCardData",t="byte",l={cmd.MAX_COUNT}}--用户扑克
		
}

--用户站起
cmd.CMD_S_StandUp =
{
	{k="wStandUpUser",t="word"} --站起用户
}


--奖池信息
cmd.CMD_S_PoolInfo =
{
	{k="cbPoolCount",t="byte"}, 
	{k="lPoolScore",t="score",l={cmd.GAME_PLAYER}},--奖池分数	
}


cmd.CMD_S_REST_SCORE = 
{
	{k="lScore", t="score"}												--可加注数目					
}

cmd.CMD_S_LastRecord = 
{
	{k = "szNickName", t = "string", l ={9}, s=32},--,32,32,32,32,32,32,32,32}},                   --用户昵称	
	{k="lScore",t="score",l={cmd.GAME_PLAYER}},--玩家分数	
	{k="cbPlayStatus",t="byte",l={cmd.GAME_PLAYER}},--游戏状态	
	{k="cbHandData",t="byte",l={cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT}},
	{k="cbCenterCardData",t="byte",l={cmd.MAX_CENTER_COUNT}},--扑克数目	
	{k="cbCardType",t="byte",l={cmd.GAME_PLAYER}},--牌型	
	{k="cbMaxCard",t="byte",l={cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT,cmd.MAX_CENTER_COUNT}},
	{k="wMinChipInUser",t="word"},--小盲
	{k="wMaxChipInUser",t="word"},--大盲
}



--------------------------------------------------------------------------
--客户端命令结构
cmd.SUB_C_ADD_SCORE					= 1									--用户加注
cmd.SUB_C_GIVE_UP					= 2									--放弃消息
cmd.SUB_C_GAME_END					= 3									--游戏结束
cmd.SUB_C_SET_CHIP					= 4									--设置筹码
cmd.SUB_C_SHOW_CARD					= 5									--用户亮牌.
cmd.SUB_C_STAND_UP					= 6									--用户站起

cmd.SUB_C_GAME_RECORD				= 7									--请求游戏记录
cmd.SUB_C_GET_REST_SCORE			= 8									--请求可补充分
cmd.SUB_C_LAST_RECORD			= 9									--牌局信息

--用户加注
cmd.CMD_C_AddScore = 
{
	{k="lScore",t="score"}												--加注数目					
}


--设置筹码
cmd.CMD_C_SetChip = 
{
	{k="bAutoAddChip",t="bool"},--自动兑换
	{k="lChipScore",t="score"}	--筹码数目						
}





return cmd
