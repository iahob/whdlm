local cmd =  {}

cmd.RES_PATH 				= "game/yule/sparrowgdex/res/"
--游戏版本
cmd.VERSION 					= appdf.VersionValue(6,7,0,1)
--游戏标识
cmd.KIND_ID						= 391
--游戏人数
cmd.GAME_PLAYER					= 4
--视图位置(1~4)
cmd.MY_VIEWID					= 1
cmd.RIGHT_VIEWID				= 2
cmd.LEFT_VIEWID					= 3
cmd.TOP_VIEWID					= 4
--最大牌数
cmd.MAX_COUNT 					= 14
--最大库存
cmd.MAX_REPERTORY 				= 136
--最大组合
cmd.MAX_WEAVE 					= 4


cmd.MAX_INDEX					= 34									--最大索引

cmd.MAX_HUA_INDEX			   	= 0									--花牌索引
cmd.MAX_HUA_COUNT			    = 8									--花牌个数
cmd.MAX_MA_COUNT			    = 8									--最大抓码数
--扑克定义
cmd.HEAP_FULL_COUNT		        = 34								--堆立全牌
cmd.MAX_RIGHT_COUNT		        = 1									--最大权位DWORD个数	最大胡牌类型数（可以同时胡几种牌型）
				
cmd.MAX_MAGIC_COUNT 			= 2									--最大财神数
cmd.MAX_RECORD_COUNT			=32				      				--房卡结算统计最大局数

-- 语音动画
cmd.VOICE_ANIMATION_KEY = "voice_ani_key"


--********************       定时器标识         ***************--
cmd.IDI_START_GAME				= 201						--开始定时器
cmd.IDI_OUT_CARD				= 202						--出牌定时器
cmd.IDI_OPERATE_CARD			= 203						--操作定时器

--*******************        时间标识         *****************--
--快速出牌时间
cmd.TIME_OUT_CARD_FAST			= 10
--出牌时间
cmd.TIME_OUT_CARD				= 30
--准备时间
cmd.TIME_START_GAME 			= 30


--******************         游戏状态             ************--
--空闲状态
cmd.GAME_SCENE_FREE				= 0
--游戏状态
cmd.GAME_SCENE_PLAY				= 100
--结算状态
cmd.GAME_SCENE_GAMEEND			= 101
--******************         选牌模式             ************--
--没有选牌
cmd.SELECT_CARD_NIL				= 0
cmd.SELECT_CARD_GANG			= 1

cmd.CMD_S_RECORD =
{
	{k = "nCount", t = "int"},
	{k = "cbHuCount", t = "byte", l = {cmd.GAME_PLAYER}},--胡牌次数
	{k = "cbMaCount", t = "byte", l = {cmd.GAME_PLAYER}},--中码个数
	{k = "cbAnGang", t = "byte", l = {cmd.GAME_PLAYER}},--暗杠次数
	{k = "cbMingGang", t = "byte", l = {cmd.GAME_PLAYER}},--明杠次数
	{k = "lAllScore", t = "score", l = {cmd.GAME_PLAYER}},--总结算分
	{k = "lDetailScore", t = "score", l = {cmd.MAX_RECORD_COUNT, cmd.MAX_RECORD_COUNT, cmd.MAX_RECORD_COUNT, cmd.MAX_RECORD_COUNT,}},--单局结算分
}


--组合子项`
cmd.tagWeaveItem = 
{
	{k = "cbWeaveKind", t = "byte"},							--组合类型
	{k = "cbCenterCard", t = "byte"},							--中心扑克
	{k = "cbParam", t = "byte"},								--类型标志
	{k = "wProvideUser", t = "word"},							--供应用户
	{k = "cbCardData", t = "byte", l = {4}},					--麻将数据
}

--空闲状态
cmd.CMD_S_StatusFree = 
{
	--基础积分
	{k = "cbHeapCount", t = "byte", l = {cmd.GAME_PLAYER}},		--堆牌数
	{k = "lCellScore", t = "int"},								--基础积分
	--时间信息
 	{k = "cbTimeOutCard", t = "byte"},							--出牌时间
 	{k = "cbTimeOperateCard", t = "byte"},						--操作时间
 	{k = "cbTimeStartGame", t = "byte"},						--开始时间
 	{k = "cbTimeWaitEnd", t = "byte"},							--结算等待时间
	--历史积分
	{k = "lTurnScore", t = "score", l = {cmd.GAME_PLAYER}},		--积分信息
	{k = "lCollectScore", t = "score", l = {cmd.GAME_PLAYER}},	--积分信息


	--规则		
	{k = "cbPlayerCount", t = "byte"},							--玩家人数
	{k = "cbMaCount", t = "byte"},								--码数
	{k = "cbMagicMode", t = "byte"},							--翻鬼模式
	{k = "bQiangGangHu", t = "bool"},							--抢杠胡
	{k = "bHuQiDui", t = "bool"},								--胡七对
	{k = "bHaveZiCard", t = "bool"},							--带字牌
	{k = "bNoMagicDouble", t = "bool"},							--无鬼加倍
	{k = "cbAllCardCount", t = "byte"},							--牌数量
	{t = "byte",k="cbPlayMode"},             --游戏模式
}
--游戏状态
cmd.CMD_S_StatusPlay = 
{
	
	{k = "cbHeapCount", t = "byte", l = {cmd.GAME_PLAYER}},		--堆牌数

	--时间信息
	{k = "cbTimeOutCard", t = "byte"},							--出牌时间
	{k = "cbTimeOperateCard", t = "byte"},						--叫分时间
	{k = "cbTimeStartGame", t = "byte"},						--开始时间
	{k = "cbTimeWaitEnd", t = "byte"},							--结算等待时间
	--游戏变量
	{k = "lCellScore", t = "score"},							--单元积分
	{k = "wBankerUser", t = "word"},							--庄家用户
	{k = "wCurrentUser", t = "word"},							--当前用户
	{k = "cbMagicIndex", t = "byte", l = {cmd.MAX_MAGIC_COUNT}},--财神索引

	{k = "cbPlayerCount", t = "byte"},
	{k = "cbMaCount", t = "byte"},
	{k = "cbMagicMode", t = "byte"},							--翻鬼模式
	{k = "bQiangGangHu", t = "bool"},							--抢杠胡
	{k = "bHuQiDui", t = "bool"},								--胡七对
	{k = "bHaveZiCard", t = "bool"},							--带字牌
	{k = "bNoMagicDouble", t = "bool"},							--无鬼加倍

	{k = "cbActionCard", t = "byte"},							--动作扑克
	{k = "cbActionMask", t = "byte"},							--动作掩码
	{k = "wProvideUser", t = "word"},							--供牌玩家
	{k = "cbAllCardCount", t = "byte"},							--牌数量
	{k = "cbLeftCardCount", t = "byte"},						--剩余数目
	{k = "bTrustee", t = "bool", l = {cmd.GAME_PLAYER}},		--是否托管

	{k = "wOutCardUser", t = "word"},							--出牌用户
	{k = "cbOutCardData", t = "byte"},							--出牌扑克
	{k = "cbDiscardCount", t = "byte", l = {cmd.GAME_PLAYER}},	--丢弃数目
	{k = "cbDiscardCard", t = "byte", l = {60,60,60,60}},       --丢弃记录

	{k = "cbCardCount", t = "byte", l = {cmd.GAME_PLAYER}},		--扑克数目
	{k = "cbCardData", t = "byte", l = {cmd.MAX_COUNT}},		--扑克列表
	{k = "cbSendCardData", t = "byte"},							--发送扑克

	{k = "cbWeaveItemCount", t = "byte", l = {cmd.GAME_PLAYER}},--组合数目
	{k = "WeaveItemArray", t = "table", d = cmd.tagWeaveItem, l = {4, 4, 4, 4}},--组合扑克

	{k = "wHeapHead", t = "word"},								--堆立头部
	{k = "wHeapTail", t = "word"},								--堆立尾部
	{k = "cbHeapCardInfo", t = "byte", l = {2, 2, 2, 2}},		--牌堆信息

	--历史积分
	{k = "lTurnScore", t = "score", l = {cmd.GAME_PLAYER}},		--积分信息
	{k = "lCollectScore", t = "score", l = {cmd.GAME_PLAYER}},	--积分信息

	{t = "byte",k="cbPlayMode"},             --游戏模式
	{t = "byte",k="cbTrusteeTime"},             --游戏模式
}

--*********************      服务器命令结构       ************--
cmd.SUB_S_GAME_START			= 100								--游戏开始
cmd.SUB_S_OUT_CARD				= 101								--用户出牌
cmd.SUB_S_SEND_CARD				= 102								--发送扑克
cmd.SUB_S_OPERATE_NOTIFY		= 103								--操作提示
cmd.SUB_S_OPERATE_RESULT		= 104								--操作命令
cmd.SUB_S_LISTEN_CARD			= 105--不需要						--用户听牌
cmd.SUB_S_TRUSTEE				= 106								--用户托管
cmd.SUB_S_REPLACE_CARD			= 107--不需要						--用户补牌
cmd.SUB_S_GAME_CONCLUDE			= 108								--游戏结束
cmd.SUB_S_SET_BASESCORE			= 109								--设置基数
cmd.SUB_S_TING_DATA				= 110								--听牌数据
cmd.SUB_S_HU_CARD				= 111								--听牌胡牌数据
cmd.SUB_S_RECORD				= 112								--房卡结算记录

--发送扑克
cmd.CMD_S_GameStart = 
{
	{k = "wBankerUser", t = "word"},							--庄家用户
	{k = "wSiceCount", t = "word"},								--筛子点数
	{k = "wHeapHead", t = "word"},								--牌堆头部
	{k = "wHeapTail", t = "word"},								--牌堆尾部
	{k = "cbMagicIndex", t = "byte", l = {cmd.MAX_MAGIC_COUNT}},--财神索引
	{k = "cbHeapCardInfo", t = "byte", l = {2,2,2,2}},          --堆立信息
	{k = "cbUserAction", t = "byte"},							--用户动作
	{k = "cbCardData", t = "byte", l = {cmd.MAX_COUNT}},		--麻将列表
	{k = "cbFanCardData", t = "byte"},							--用户动作
}

--用户出牌
cmd.CMD_S_OutCard = 
{
	{k = "wOutCardUser", t = "word"},							--出牌用户
	{k = "cbOutCardData", t = "byte"},							--出牌扑克
	{k = "bSysOut", t = "bool"},								--托管系统出牌
}
--发送扑克
cmd.CMD_S_SendCard = 
{
	{k = "cbCardData", t = "byte"},								--扑克数据
	{k = "cbActionMask", t = "byte"},							--动作掩码
	{k = "wCurrentUser", t = "word"},							--当前用户
	{k = "wSendCardUser", t = "word"},							--发牌用户
	{k = "wReplaceUser", t = "word"},							--补牌用户
	{k = "bTail", t = "bool"},									--末尾发牌
}
--操作提示
cmd.CMD_S_OperateNotify = 
{
	{k = "cbActionMask", t = "byte"},							--动作掩码
	{k = "cbActionCard", t = "byte"},							--动作扑克
	{k = "wProvideUser", t = "word"},							--供应用户
}
--操作命令
cmd.CMD_S_OperateResult = 
{
	{k = "wOperateUser", t = "word"},							--操作用户
	{k = "cbActionMask", t = "byte"},							--动作掩码
	{k = "wProvideUser", t = "word"},							--供应用户
	{k = "cbOperateCode", t = "byte"},							--操作代码
	{k = "cbOperateCard", t = "byte", l = {3}},					--操作扑克
}
--提示听牌
cmd.CMD_S_TING_DATA = 
{
	{k = "cbOutCardCount", t = "byte"},
	{k = "cbOutCardData", t = "byte", l = {14}},
	--看情况开启
	{k = "cbHuCardCount", t = "byte", l = {14}},
	{k = "cbHuCardData", t = "byte", l = {34, 34, 34, 34, 34, 34, 34,
											34, 34, 34, 34, 34, 34, 34}},
	{k = "cbHuCardRemainingCount", t = "byte", l = {34, 34, 34, 34, 34, 34, 34,
											34, 34, 34, 34, 34, 34, 34}},
	{k = "cbHuFan", t = "byte", l = {34, 34, 34, 34, 34, 34, 34,
											34, 34, 34, 34, 34, 34, 34}},
}

--胡牌数据
cmd.CMD_S_HU_DATA = 
{
	{k = "wListenUser", t = "word"},							--听牌用户
	{k = "cbHuCardCount", t = "byte"},
	{k = "cbHuCardData", t = "byte", l = {34}},
}

--听牌操作命令
cmd.CMD_S_ListenCard =
{
	{k = "wListenUser", t = "word"},							--听牌用户
	{k = "bListen", t = "bool"},							--是否听牌
}
--游戏结束
cmd.CMD_S_GameConclude = 
{
	{k = "lCellScore", t = "int"},								--单元积分
	{k = "lGameScore", t = "score", l = {cmd.GAME_PLAYER}},		--游戏积分
	{k = "lRevenue", t = "score", l = {cmd.GAME_PLAYER}},		--税收积分
	{k = "lGangScore", t = "score", l = {cmd.GAME_PLAYER}},		--本局杠输赢分
	{k = "lHuScore", t = "score", l = {cmd.GAME_PLAYER}},		--本局胡输赢分
	{k = "lMaScore", t = "score", l = {cmd.GAME_PLAYER}},		--本局码输赢分

	{k = "wFleeUser", t = "word"},								--逃跑玩家
	{k = "wProvideUser", t = "word"},							--供应用户
	{k = "cbProvideCard", t = "byte"},							--供应扑克
	{k = "cbSendCardData", t = "byte"},							--最后发牌
	{k = "cbChiHuKind", t = "dword", l = {cmd.GAME_PLAYER}},	--胡牌类型
	{k = "dwChiHuRight", t = "dword", l = {cmd.MAX_RIGHT_COUNT, cmd.MAX_RIGHT_COUNT, cmd.MAX_RIGHT_COUNT, cmd.MAX_RIGHT_COUNT}},		--胡牌类型

	{k = "cbCardCount", t = "byte", l = {cmd.GAME_PLAYER}},		--扑克数目
	{k = "cbHandCardData", t = "byte", l = {cmd.MAX_COUNT, cmd.MAX_COUNT, cmd.MAX_COUNT, cmd.MAX_COUNT}},	--扑克列表
	{k = "cbMaCount", t = "byte"},								--码数
	{k = "cbMaData", t = "byte", l = {cmd.MAX_MA_COUNT}},		--码数据
	{k = "cbZhongMaCount", t = "byte"},							--中码个数
	{k = "cbZhongMaData", t = "byte", l = {cmd.MAX_MA_COUNT}},	--中码数据
	{k = "bNoMagicCard", t = "int"},
	{k = "cbMagicIndex", t = "byte", l = {cmd.MAX_MAGIC_COUNT}},--财神索引
	{k = "cbWeaveItemCount", t = "byte", l = {cmd.GAME_PLAYER}},--组合数目
	{k = "WeaveItemArray", t = "table", d = cmd.tagWeaveItem, l = {cmd.MAX_WEAVE, cmd.MAX_WEAVE, cmd.MAX_WEAVE, cmd.MAX_WEAVE}},--组合扑克							--是否无鬼加倍
}
--用户托管
cmd.CMD_S_Trustee = 
{
	{k = "bTrustee", t = "bool"},								--是否托管
	{k = "wChairID", t = "word"}								--托管用户
}
--补牌命令
cmd.CMD_S_ReplaceCard = 
{
	{k = "wReplaceUser", t = "word"},							--补牌用户
	{k = "cbReplaceCard", t = "byte"}							--补牌扑克
}
--游戏记录
cmd.CMD_S_Record = 
{
	{k = "nCount", t = "int"},									--
	{k = "cbHuCount", t = "byte", l = {cmd.GAME_PLAYER}},		--
	{k = "cbMaCount", t = "byte", l = {cmd.GAME_PLAYER}},		--
	{k = "cbAnGang", t = "byte", l = {cmd.GAME_PLAYER}},		--
	{k = "cbMingGang", t = "byte", l = {cmd.GAME_PLAYER}},		--
	{k = "lAllScore", t = "score", l = {cmd.GAME_PLAYER}},		--
	{k = "lDetailScore", t = "score", l = {32, 32, 32, 32}},	--
}


--**********************    客户端命令结构        ************--
cmd.SUB_C_OUT_CARD				= 1									--出牌命令
cmd.SUB_C_OPERATE_CARD			= 2									--操作扑克
cmd.SUB_C_LISTEN_CARD			= 3									--用户听牌
cmd.SUB_C_TRUSTEE				= 4									--用户托管
cmd.SUB_C_REPLACE_CARD			= 5									--用户补牌
cmd.SUB_C_SEND_CARD             = 6									--发送扑克
-- 请求流水
cmd.SUB_C_REQUEST_RCRecord                 = 7

--出牌命令
cmd.CMD_C_OutCard = 
{
	{k = "cbCardData", t = "byte"}								--扑克数据
}
--操作命令
cmd.CMD_C_OperateCard = 
{
	{k = "cbOperateCode", t = "byte"},							--操作代码
	{k = "cbOperateCard", t = "byte", l = {3}}					--操作扑克
}
--用户听牌
cmd.CMD_C_ListenCard = 
{
	{k = "bListenCard", t = "bool"}								--是否听牌
}
--用户托管
cmd.CMD_C_Trustee = 
{
	{k = "bTrustee", t = "bool"}								--是否托管
}
--补牌命令
cmd.CMD_C_ReplaceCard = 
{
	{k = "cbCardData", t = "byte"}								--扑克数据
}
--发送扑克
cmd.CMD_C_SendCard = 
{
	{k = "cbControlGameCount", t = "byte"},						--控制次数
	{k = "cbCardCount", t = "byte"},							--扑克数目
	{k = "wBankerUser", t = "word"},							--控制庄家
	{k = "cbCardData", t = "byte", l = {cmd.MAX_REPERTORY}}		--扑克数据
}

--组合子项`
cmd.tagAvtiveCard = 
{
	cbCardNum = 0,
	cbType = 0,
	cbCardValue = {},
}

--胡牌信息`
cmd.tagHuInfo = 
{
	cbCardValue = 0,
	cbNum = 0,
	cbFan = 0,
}
return cmd