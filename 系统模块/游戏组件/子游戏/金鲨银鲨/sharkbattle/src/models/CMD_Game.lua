local cmd = {}

--游戏标识
cmd.KIND_ID = 127

--游戏人数
cmd.PLAYER_COUNT  = 100

--temp
local GAME_STATUS_FREE = 0
local GAME_STATUS_PLAY = 100

--游戏状态
cmd.GAME_SCENE_FREE	=	GAME_STATUS_FREE		--等待开始
cmd.GAME_SCENE_BET	=	GAME_STATUS_PLAY		--下注状态
cmd.GAME_SCENE_END	=	(GAME_STATUS_PLAY+1)	--开奖状态      --游戏结束


--转盘索引
cmd.TURAN_TABLE_MAX	=				28							--转盘索引
cmd.TURAN_TABLE_JUMP	=				30							--转盘跳转

--动物索引
cmd.ANIMAL_LION	        =				0							--狮子
cmd.ANIMAL_PANDA	    =				1							--熊猫
cmd.ANIMAL_MONKEY	    =				2							--猴子
cmd.ANIMAL_RABBIT	    =				3							--兔子
cmd.ANIMAL_EAGLE	    =				4							--老鹰
cmd.ANIMAL_PEACOCK      =				5							--孔雀
cmd.ANIMAL_PIGEON	    =				6							--鸽子
cmd.ANIMAL_SWALLOW      =				7							--燕子
cmd.ANIMAL_SLIVER_SHARK =				8							--银鲨
cmd.ANIMAL_GOLD_SHARK	=				9							--金鲨
cmd.ANIMAL_BIRD		    =				10							--飞禽
cmd.ANIMAL_BEAST		=				11							--走兽
cmd.ANIMAL_MAX		    =				12

--分类信息
cmd.ANIMAL_TYPE_NULL		=			0							--无
cmd.ANIMAL_TYPE_BEAST		=			1							--走兽
cmd.ANIMAL_TYPE_BIRD		=			2							--飞禽
cmd.ANIMAL_TYPE_GOLD		=			3							--黄金
cmd.ANIMAL_TYPE_SLIVER		=			4							--白银

--记录信息
cmd.KYE_GAME_INNING		    =			1							--游戏局数
cmd.KYE_REWARD_INNING		=			2							--中奖局数
cmd.KYE_TOTEL_REWARD		=			3							--游戏局数
cmd.KYE_MAX_REWARD			=			4							--中奖局数

--游戏记录
cmd.RECORD_COUNT_MAX		=			11							--最大数量

----------------------------------------------------------------------------------
--temp
-- local WM_USER=0

-- --控制消息
-- cmd. IDM_UPDATE_CHIP	=					(WM_USER+2000)				--更新筹码消息
-- cmd. IDM_EXCHANGE_CHIP	=				(WM_USER+200)				--兑换筹码消息

-- --下注按钮
-- cmd. IDM_PLAY_BET		=				(WM_USER+201)				--狮子

-- --控制按钮
-- cmd. IDM_OPERATION_RENEWAL=				(WM_USER+213)				--续压
-- cmd. IDM_OPERATION_CANCEL	=			(WM_USER+214)				--取消
-- cmd. IDM_OPERATION_SWITCH	=			(WM_USER+215)				--切换
-- cmd. IDM_OPERATION_BANK		=		    (WM_USER+216)				--切换
----------------------------------------------------------------------------------

cmd.MAX_UNITNOTE = 1000000
cmd.MIN_UNITNOTE = 100

cmd.RECORD_COUNT_MAX = 20
----------------------------------------------------------------------------------
--游戏状态
cmd.CMD_S_StatusFree=
{
	{t="byte",k="cbTimeLeave"},						    --剩余时间
	{t="score",k="lStartStorage"},						--起始库存
	
	{t="int",k="lCellScore"},							--底分
--	{t="score",k="						lPlayScore"},							--玩家分数
	{t="score",k="lStorageStart"},						--库存（彩池）

	{t="score",k="lAreaLimitScore"},					--区域限制
	{t="score",k="lPlayLimitScore"},					--玩家限制
	
	{t="int",k="nTurnTableRecord",l={cmd.RECORD_COUNT_MAX} },	--游戏记录
	{t="int",k="nTurnTableRecordExt",l={cmd.RECORD_COUNT_MAX} },
	{t="string",k="szGameRoomName",s=32},					--房间名称 
	{t="bool",k="bGenreEducate"},					--练习模式

	{t="score",k="dChipArray",l={6}},			--筹码配置
	--{t="score",k="tagCustomAndroid",l={12}},			--useless
}

--游戏状态
cmd.CMD_S_StatusPlay=
{
	{t="byte",k="cbTimeLeave"},						--剩余时间
	{t="score",k="lStartStorage"},						--起始库存

	{t="int",k="lCellScore"},							--底分
-- 	{t="score",k="						lPlayScore"},							--玩家分数
-- 	{t="score",k="						lPlayChip"},							--玩家筹码
	{t="score",k="lStorageStart"},						--库存（彩池）		

	{t="score",k="lAreaLimitScore"},					--区域限制
	{t="score",k="lPlayLimitScore"},					--玩家限制

	{t="int",k="nAnimalMultiple",l={cmd.ANIMAL_MAX}},		--动物倍数
	{t="score",k="lAllBet",l={cmd.ANIMAL_MAX}},				--总下注
	{t="score",k="lPlayBet",l={cmd.ANIMAL_MAX}},				--玩家下注

	{t="int",k="nTurnTableRecord",l={cmd.RECORD_COUNT_MAX} },	--游戏记录
	{t="int",k="nTurnTableRecordExt",l={cmd.RECORD_COUNT_MAX} },	--游戏记录
	{t="string",k="szGameRoomName",s=32},					--房间名称 
	{t="bool",k="bGenreEducate"},						--练习模式

	{t="score",k="dChipArray",l={6}},			--筹码配置
	--{t="score",k="tagCustomAndroid",l={12}},			--useless
}

--游戏状态
cmd.CMD_S_StatusEnd=
{
	{t="byte",k="cbTimeLeave"},						    --剩余时间
	{t="score",k="lStartStorage"},						--起始库存

	{t="int",k="lCellScore"},							--底分
-- 	{t="score",k="						lPlayScore"},							--玩家分数
-- 	{t="score",k="						lPlayChip"},							--玩家筹码
	{t="score",k="lStorageStart"},						--库存（彩池）

	{t="score",k="lAreaLimitScore"},					--区域限制
	{t="score",k="lPlayLimitScore"},					--玩家限制

	{t="int",k="nAnimalMultiple",l={cmd.ANIMAL_MAX}},	--动物倍数
	{t="score",k="lAllBet",l={cmd.ANIMAL_MAX}},			--总下注
	{t="score",k="lPlayBet",l={cmd.ANIMAL_MAX}},		--玩家下注

	{t="int",k="nTurnTableRecord",l={cmd.RECORD_COUNT_MAX}},	--游戏记录
	{t="int",k="nTurnTableRecordExt",l={cmd.RECORD_COUNT_MAX} },
	{t="string",k="szGameRoomName",s=32},					--房间名称 
	{t="bool",k="bGenreEducate"},						    --练习模式

	{t="score",k="dChipArray",l={6}},			            --筹码配置
	--{t="score",k="tagCustomAndroid",l={12}},			--useless?
}


----------------------------------------------------------------------------------
--服务器命令结构
cmd.SUB_S_GAME_FREE             = 101				--游戏空闲
cmd.SUB_S_GAME_START            = 102				--游戏开始
cmd.SUB_S_GAME_END              = 103				--游戏结束
cmd.SUB_S_PLAY_BET              = 104				--用户下注
cmd.SUB_S_PLAY_BET_FAIL         = 105				--用户下注失败
cmd.SUB_S_BET_CLEAR             = 106				--清除下注
cmd.SUB_S_AMDIN_COMMAND         = 107				--管理员命令
cmd.SUB_S_ADMIN_STORAGE_INFO    = 108				--库存命令
cmd.SUB_S_SEND_USER_BET_INFO    = 109               --发送下注

--游戏空闲
cmd.CMD_S_GameFree=
{
	{t="byte",k="cbTimeLeave"},						--剩余时间
}

--游戏开始
cmd.CMD_S_GameStart=
{
	{t="byte",k="cbTimeLeave"},						    --剩余时间

	{t="int",k="nAnimalMultiple",l={cmd.ANIMAL_MAX}},	--动物倍数
	{t="score",k="lStorageStart"},						--库存（彩池）
}

--游戏结束
cmd.CMD_S_GameEnd=
{
	{t="byte",k="cbTimeLeave"},						    --剩余时间
	{t="int",k="bTurnTwoTime"},						    --转2次
	{t="int",k="nTurnTableTarget",l={2}},				--转盘目标
	{t="int",k="nPrizesMultiple"},					    --彩金	
	{t="score",k="lPlayWin",l={2}},						--玩家输赢
	{t="score",k="lPlayPrizes"},						--玩家彩金

	{t="score",k="lPlayShowPrizes"},					--显示彩金
	{t="score",k="lGameScore"},							--玩家总分
	{t="score",k="lServiceScore"},						--服务费
}

--用户下注
cmd.CMD_S_PlayBet =
{
	{t="word",k="wChairID"},							--玩家位置
	{t="int",k="nAnimalIndex"},						    --下注动物
	{t="score",k="lBetChip"},							--筹码数量
	{t="bool",k="bAndroid"},							--机器标志
}

--发送下注
cmd.CMD_S_SendUserBetInfo=
{
	{t="score",k="lUserStartScore",l={cmd.GAME_PLAYER} },			--起始分数
	--longlong型超出发包的范围
	{t="int",k="lUserJettonScore",l={cmd.GAME_PLAYER,cmd.ANIMAL_MAX} } --个人总注
}

--用户下注失败
cmd.CMD_S_PlayBetFail=
{
	{t="word",k="wChairID"},							--玩家位置
	{t="int",k="nAnimalIndex"},						    --下注动物
	{t="score",k="lBetChip"}							--筹码数量
}

--清除下注
cmd.CMD_S_BetClear=
{
	{t="word",k="wChairID"},							--玩家位置
	{t="score",k="lPlayBet",l={cmd.ANIMAL_MAX}},		--玩家清除数量
}

--管理员消息
cmd.CMD_S_AdminCommand=
{
	{t="string",k="szInfo",s=32},					    --返回信息
}

--控制服务端信息
cmd.CMD_S_ADMIN_STORAGE_INFO=
{
	{t="score",k="lCurrentStorage"},					--当前库存
	{t="score",k="lCurrentDeduct"},						--当前衰减
	{t="score",k="lMaxStorage",l={2}},					--库存上限
	{t="word",k="wStorageMul",l={2}},					--系统输分概率
	{t="score",k="lUserTotalScore",l={cmd.GAME_PLAYER} },--玩家总输赢
}
----------------------------------------------------------------------------------
--客户端命令结构
cmd.SUB_C_EXCHANGE_CHIP     = 101						--兑换筹码
cmd.SUB_C_PLAY_BET          = 102						--下注消息
cmd.SUB_C_BET_CLEAR         = 103						--清除下注
cmd.SUB_C_AMDIN_COMMAND     = 104						--管理员命令
cmd.SUB_C_AMDIN_STORANGINFO = 105						--库存信息

--兑换筹码
cmd.CMD_C_Chip =
{
	{t="score",k="lChip"},								--筹码数量
}

--下注消息
cmd.CMD_C_PlayBet =
{
	{t="int",k="nAnimalIndex"},						--下注动物
	{t="score",k="lBetChip"},							--筹码数量
}

--清除下注


--管理员消息
-- cmd.CMD_C_AdminCommand
-- {
-- 	{t="byte",k="		cbReqType"},
-- 				cmd. RQ_MODIFICATION		1									--修改
-- 				cmd. RQ_CANCEL			2									--取消
-- 				cmd. RQ_CURRENT			3									--当前
-- 				cmd. RQ_UPDATA_STORAGE   4									--修改库存
-- 	{t="byte",k="		cbExtendData[20]"},												--附加数据
-- 	{t="score",k="	lStorage"},														--库存值
-- 	{t="word",k="		cbStorageDeduct"},												--衰减值
-- }

cmd.CMD_C_StorageInfo=
{
	{t="score",k="lMaxStorage",l={2}},						--库存上限
	{t="word",k="wStorageMul",l={2}},						--吐分概率
}


return cmd