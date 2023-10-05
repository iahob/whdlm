local cmd = {}

--[[
******
* 结构体描述
* {k = "key", t = "type", s = len, l = {}}
* k 表示字段名,对应C++结构体变量名
* t 表示字段类型,对应C++结构体变量类型
* s 针对string变量特有,描述长度
* l 针对数组特有,描述数组长度,以table形式,一维数组表示为{N},N表示数组长度,多维数组表示为{N,N},N表示数组长度
* d 针对table类型,即该字段为一个table类型
* ptr 针对数组,此时s必须为实际长度

** egg
* 取数据的时候,针对一维数组,假如有字段描述为 {k = "a", t = "byte", l = {3}}
* 则表示为 变量a为一个byte型数组,长度为3
* 取第一个值的方式为 a[1][1],第二个值a[1][2],依此类推

* 取数据的时候,针对二维数组,假如有字段描述为 {k = "a", t = "byte", l = {3,3}}
* 则表示为 变量a为一个byte型二维数组,长度都为3
* 则取第一个数组的第一个数据的方式为 a[1][1], 取第二个数组的第一个数据的方式为 a[2][1]
******
]]

--游戏版本
cmd.VERSION 					= appdf.VersionValue(6,7,0,1)
--游戏标识
cmd.KIND_ID						= 508
--游戏名字
cmd.GAME_NAME                   = "三九连线"                            
	
--游戏人数
cmd.GAME_PLAYER					= 100

--房间名长度
cmd.SERVER_LEN					= 32

--游戏记录长度
cmd.RECORD_LEN					= 4

cmd.AUTOPLAY                    = "autoplay"              --托管定时函数

--状态定义
cmd.GAME_SCENE_FREE             = 0                      --等待开始
cmd.GAME_SCENE_SLOW             = 100                    --转动减速
cmd.GAME_SCENE_PLAY             = 101                    -- 游戏进行


--服务器定义消息
cmd.IDM_START_GAME				= 200			--开始消息
cmd.IDM_STOP_GAME				= 201			--开始消息
cmd.IDM_TURN_OVER				= 202			--旋转结束
cmd.IDM_START_HOSTED			= 203			--开始托管
cmd.IDM_STOP_HOSTED				= 204			--结束托管
cmd.IDM_SCORE_LESS				= 205			--分数减少
cmd.IDM_SCORE_ADD				= 206			--分数增加
cmd.IDM_SCORE_MAX				= 207			--分数最大
cmd.IDM_LINE_CHECK				= 208			--线数操作

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
cmd.CMD_S_Status = 
{
    --任务进度
    {k = "nTaskProgress", t = "int",l={cmd.TASK_INDEX_MAX}},
    --任务目标
    {k = "nTaskTarget", t = "int",l={cmd.TASK_INDEX_MAX}},
    --任务数量
    {k = "nTaskCount", t = "int",l={cmd.TASK_INDEX_MAX}},
    --任务金币
    {k = "nTaskScore", t = "int",l={cmd.TASK_INDEX_MAX}},
    --3连倍数
    {k = "nThreeLineMultiple", t = "int",l={12}},
    --9连倍数
    {k = "nNineLineMultiple", t = "int",l={9}},
    --下注数组
    {k = "nBetScore",t = "int",l={10}},
    --路单信息
    {k = "nRoadMultipleIndex", t = "int",l={10}},
    --房间彩池
    {k = "nRoomLotteryPool", t = "score"},
    --三连中奖
    {k = "nThreeTotal",t = "score"},
    --九连中奖
    {k = "nNiceTotal",t = "score"},
    --中奖累计
    {k = "nGrandTotal",t = "score"},
    --最大中奖
    {k = "nWinMax",t = "score"}

}

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

--彩池数量
cmd.CMD_S_LotteryPool = 
{
    {k = "nRoomLotteryPool",t = "score"},                       --彩池数量
};

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
    

return cmd 