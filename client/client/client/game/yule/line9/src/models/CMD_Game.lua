local cmd = {}

cmd.RES 						= "game/yule/line9/res/"
cmd.GAME 					    = "ninelineking/"

--游戏版本
cmd.VERSION 					= appdf.VersionValue(7,0,1)
--游戏标识
cmd.KIND_ID						= 520
cmd.GAME_NAME					= "足球连线"                            --游戏名字
	
--游戏人数
cmd.GAME_PLAYER					= 1

--房间名长度
cmd.SERVER_LEN					= 32

--游戏记录长度
cmd.RECORD_LEN					= 4

cmd.AUTOPLAY                    = "autoplay"                            --托管定时函数

--状态定义
cmd.GAME_SCENE_FREE             = 0                                     --等待开始
cmd.GAME_SCENE_SLOW             = 100                                   --转动减速
cmd.GAME_SCENE_PLAY             = 101                                   -- 游戏进行


--线数定义
cmd.LINE_WIDTH_1				= 0
cmd.LINE_WIDTH_2				= 1
cmd.LINE_WIDTH_3				= 2
cmd.LINE_HEIGHT_4				= 3
cmd.LINE_HEIGHT_5				= 4
cmd.LINE_HEIGHT_6				= 5
cmd.LINE_OBLIQUE_7				= 6
cmd.LINE_OBLIQUE_8				= 7
cmd.LINE_INDEX_MAX				= 8


--3连倍数
cmd.MultipleThree_AAA =           0
cmd.MultipleThree_BBB =           1
cmd.MultipleThree_CCC =           2
cmd.MultipleThree_DDD =           3
cmd.MultipleThree_EEE =           4
cmd.MultipleThree_FFF =           5
cmd.MultipleThree_GGG =           6
cmd.MultipleThree_BCD =           7
cmd.MultipleThree_HHH =           8
cmd.MultipleThree_III =           9
cmd.MultipleThree_II =            10
cmd.MultipleThree_I =             11
cmd.MultipleThree_Max =           12                                  -- 3连倍数
cmd.MultipleThree_Invalid =       -1              -- 无效倍数 


--9连倍数

cmd.MultipleNine_A =              0
cmd.MultipleNine_B =              1
cmd.MultipleNine_C =              2
cmd.MultipleNine_D =              3
cmd.MultipleNine_E =              4
cmd.MultipleNine_F =              5
cmd.MultipleNine_G =              6
cmd.MultipleNine_H =              7
cmd.MultipleNine_I =              8
cmd.MultipleNine_Max =            9                   -- 9连倍数
cmd.MultipleNine_Invalid =        -1                  -- 无效倍数 

--线数详细
--cmd.LINE_WIDTH_1_1				0][0
--cmd.LINE_WIDTH_1_2				1][0
--cmd.LINE_WIDTH_1_3				2][0

--cmd.LINE_WIDTH_2_1				0][1
--cmd.LINE_WIDTH_2_2				1][1
--cmd.LINE_WIDTH_2_3				2][1

--cmd.LINE_WIDTH_3_1				0][2
--cmd.LINE_WIDTH_3_2				1][2
--cmd.LINE_WIDTH_3_3				2][2

--cmd.LINE_HEIGHT_4_1				0][0
--cmd.LINE_HEIGHT_4_2				0][1
--cmd.LINE_HEIGHT_4_3				0][2

--cmd.LINE_HEIGHT_5_1				1][0
--cmd.LINE_HEIGHT_5_2				1][1
--cmd.LINE_HEIGHT_5_3				1][2

--cmd.LINE_HEIGHT_6_1				2][0
--cmd.LINE_HEIGHT_6_2				2][1
--cmd.LINE_HEIGHT_6_3				2][2

--cmd.LINE_OBLIQUE_7_1			2][0
--cmd.LINE_OBLIQUE_7_2			1][1
--cmd.LINE_OBLIQUE_7_3			0][2

--cmd.LINE_OBLIQUE_8_1			2][2
--cmd.LINE_OBLIQUE_8_2			1][1
--cmd.LINE_OBLIQUE_8_3			0][0

--任务信息
cmd.TASK_INDEX_MAX	=			3

--转动定义
cmd.TURN_WIDTH_COUNT =			3									--转动宽数量
cmd.TURN_HEIGHT_COUNT = 		3									--转动高数量

--下注范围
cmd.BET_INDEX_MAX =				10

--路单信息
cmd.ROAD_MULTIPLE_MAX =			10

--游戏状态 
--cmd.CMD_S_Status = 
--{
--    --任务进度
--    {k = "nTaskProgress", t = "int",l={cmd.TASK_INDEX_MAX}},
--    --任务目标
--    {k = "nTaskTarget", t = "int",l={cmd.TASK_INDEX_MAX}},
--    --任务数量
--    {k = "nTaskCount", t = "int",l={cmd.TASK_INDEX_MAX}},
--    --任务金币
--    {k = "nTaskScore", t = "int",l={cmd.TASK_INDEX_MAX}},
--    --3连倍数
--    {k = "nThreeLineMultiple", t = "int",l={12}},
--    --9连倍数
--    {k = "nNineLineMultiple", t = "int",l={9}},
--    --下注数组
--    {k = "nBetScore",t = "int",l={10}},
--    --路单信息
--    {k = "nRoadMultipleIndex", t = "int",l={10}},
--    --房间彩池
--    {k = "nRoomLotteryPool", t = "score"},
--    --三连中奖
--    {k = "nThreeTotal",t = "score"},
--    --九连中奖
--    {k = "nNiceTotal",t = "score"},
--    --中奖累计
--    {k = "nGrandTotal",t = "score"},
--    --最大中奖
--    {k = "nWinMax",t = "score"}

--}

-----------------------------------------------------

cmd.SUB_S_ELEMENT_STOP      = 101   --元素信息
cmd.SUB_S_LOTTERY_POOL      = 102   --彩池数量
cmd.SUB_S_TASK_INFO         = 103   --任务信息
cmd.SUB_S_AWARD_INFO        = 104   --中奖信息

--元素停止
cmd.CMD_S_ElementStop = 
{
    {k = "lPlayWinScore",t = "score"},                          -- 玩家赢分数
    {k = "nPlayTaskScore",t = "int"},                           -- 任务分数
    {k = "nMultipleIndex",t = "int"},                           -- 中奖信息
    {k = "bMagnification",t = "bool"},                          -- 大倍分数
    {k = "bStopNew",t = "bool"},                                -- 立即停止
    {k = "bTaskComplete",t = "bool",l = {cmd.TASK_INDEX_MAX}},  -- 完成任务
    {k = "bPlayWinLine",t = "bool",l = {cmd.LINE_INDEX_MAX}},   -- 赢线数
    {k = "nElementIndex",t = "int",l = {cmd.TURN_HEIGHT_COUNT,cmd.TURN_HEIGHT_COUNT,cmd.TURN_HEIGHT_COUNT}},  -- 元素信息

};

----彩池数量
--cmd.CMD_S_LotteryPool = 
--{
--    {k = "nRoomLotteryPool",t = "score"},                       --彩池数量
--};

--任务信息
cmd.CMD_S_TaskInfo =
{
    {k = "nTaskProgress",t = "int",l = {cmd.TASK_INDEX_MAX}},   -- 任务进度
};

--中奖信息
cmd.CMD_S_AwardInfo = 
{
    {k = "szPlayName",t = "string",s = 40},                    --玩家名字
    {k = "nTableID",t = "int"},                                 --桌子ID
    {k = "nMultipleIndex",t = "int"},                           --中奖信息
    {k = "nAwardScore",t = "score"},                            --中奖分数
};

----------------------------------------------------------------------------------------
--客户端命令结构
cmd.SUB_C_START =                        101                     --开始消息
cmd.SUB_C_STOP =                         102                     --结束消息

--开始消息
cmd.CMD_C_Start = 
{
    {k = "nBetScore",t = "int"},                                --下注数量
    {k = "nBetLine",t = "bool",l = {cmd.LINE_INDEX_MAX}},       -- 下注线数
};

--停止消息
cmd.CMD_C_Stop = 
{
};







cmd.MAX_LINE_COUNT              = 9									    --最大线量
cmd.MAX_BET_COUNT               = 10									--最大下注
cmd.X_COUNT                     = 5									    --横
cmd.Y_COUNT                     = 3									    --列

--逻辑类型
cmd.CT_SPAIN					= 0									    --西班牙
cmd.CT_GERMANY					= 1									    --德国
cmd.CT_FRANCE					= 2									    --法国
cmd.CT_ENGLAND					= 3									    --英格兰
cmd.CT_SWITZERLAND				= 4									    --瑞士
cmd.CT_COLOMBIA					= 5									    --哥伦比亚
cmd.CT_AUSTRALIA				= 6									    --澳大利亚
cmd.CT_JAPAN					= 7									    --日本
cmd.CT_RUSSIA					= 8									    --俄罗斯
cmd.CT_KOREA					= 9									    --韩国
cmd.CT_ARGENTINA				= 10								    --阿根廷
cmd.CT_BRAZIL					= 11								    --巴西
cmd.CT_CHINA					= 12								    --中国
cmd.CT_WORLD_CUP				= 13								    --大力神杯

cmd.SUB_S_GAME_START			= 100									--结束信息
cmd.SUB_S_GAME_CONCLUDE			= 101									--结束游戏
cmd.SUB_S_GAME_LOTTERY			= 102									--房间彩金
cmd.SUB_S_GAME_HISTORY_RECORD	= 103									--大奖记录

--结束信息
cmd.CMD_S_GameStart = 
{	
    {t = "byte", k = "cbItemInfo", l = {5,5,5}},				        --开奖信息
    {t = "score", k = "lLineScore", l = {cmd.MAX_LINE_COUNT}},			--游戏积分
    {t = "int", k = "nGiveAwayCount"},			                        --赠送次数
}

--房间彩金
cmd.CMD_S_Lottery =
{	
    {t = "score", k = "lLotteryScore"},					                --房间彩金
}

--大奖记录
cmd.CMD_S_HistoryRecord =
{	
    {t = "string", k = "szWinTime", s = 15},			                --得奖时间（模式是20180301144130）
    {t = "dword", k = "dwGameID"},					                    --用户标识
    {t = "string", k = "szNickName", s = 32},			                --用户昵称
    {t = "byte", k = "cbLineType"},				                        --押线类型(就是连的是哪国的)
    {t = "byte", k = "cbLineCount"},				                    --押线类型数量
    {t = "score", k = "lLineScore"},					                --押线得分
    {t = "byte", k = "cbLineNumber"},				                    --中奖线型
}
-----------------------

cmd.GAME_STATUS_FREE			= 0                                     --空闲状态
cmd.GAME_STATUS_PLAY			= 100                                   --游戏状态

cmd.GAME_SCENE_FREE				= cmd.GAME_STATUS_FREE				    --空闲
cmd.GAME_SCENE_WIAT				= cmd.GAME_STATUS_PLAY				    --等待
cmd.GAME_SCENE_PLAY				= cmd.GAME_STATUS_PLAY+1				--游戏中

cmd.SUB_C_GAME_START			= 100									--押线开始
cmd.SUB_C_GIVE_AWAY_START		= 101									--押线开始
cmd.SUB_C_GAME_CONCLUDE			= 102									--游戏结束

--游戏开始
cmd.CMD_C_GameStart = 
{
    {t = "byte", k = "cbLineCount"},					                --押线数量
    {t = "byte", k = "cbBetIndex"},					                    --押注索引
}

--空闲状态
cmd.CMD_S_StatusFree = 
{
	--游戏属性
    {t = "score", k = "lCellScore"},					                --基础积分
	--下注值
    {t = "byte", k = "cbBetCount"},					                    --下注数量
    {t = "score", k = "lBetScore", l = {cmd.MAX_BET_COUNT}},			--下注大小

    {t = "score", k = "lLotteryScore"},					                --房间彩金
}

--游戏状态（场景消息，如果上次退出时还保留了次数你重进会直接 收到PLAY，断线重连也会收到这个消息）
cmd.CMD_S_StatusPlay = 
{
	--游戏属性
    {t = "score", k = "lCellScore"},					                --基础积分
	--下注值
    {t = "byte", k = "cbBetCount"},					                    --下注数量
    {t = "score", k = "lBetScore", l = {cmd.MAX_BET_COUNT}},			--下注大小

    {t = "byte", k = "cbLineCount"},					                --押线数量
    {t = "byte", k = "cbBetIndex"},					                    --押线索引
    {t = "int", k = "nGiveAwayCount"},			                        --赠送次数
    {t = "score", k = "lTotalGameScore"},					            --游戏总分

    {t = "score", k = "lLotteryScore"},					                --房间彩金
}

return cmd 