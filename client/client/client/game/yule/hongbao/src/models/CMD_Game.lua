--
-- Author: Tang
-- Date: 2016-10-11 17:21:32

--豪车俱乐部
--
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd  = {}

--游戏版本
cmd.VERSION  			= appdf.VersionValue(6,7,0,1)
--游戏标识
cmd.KIND_ID				= 148
--游戏人数
cmd.GAME_PLAYER   		= 100
--房间名长度
cmd.SERVER_LEN			= 32

--状态定义
cmd.GS_GAME_FREE		= 0
cmd.GS_GAME_PLAY     = 100   --下注状态


--红包信息
cmd.stRedBagInfo = 
{
	{k="wDispatchUser",t="word"}, 			--发包玩家
	{k = "lBagScore",t = "score"},
	{k = "cbThunderPoint",t = "byte"},
	{k="lBagKey",t="word"}, 
}

cmd.CMD_S_StatusPlay = 
{
	{k = "nRatio",t = "int"},		--金币比例
	{k = "cbPlayTime",t = "byte"},		--抢包时间
	{k = "cbPlayerCount",t = "byte"},		--抢包人数
	{k = "lScoreRangeMin",t = "score"},		--红包最小值
	{k = "lScoreRangeMax",t = "score"},		--红包最大值
	{k = "lDispatchCondition",t = "score"},		--发包要求
	{k = "dBagTimes",t = "double"},		--赔率
	{k = "cbThunderPosition",t = "byte"},    -- 雷号位置，0小数点第一位，1小数点第二位

	{k="wBankerUser",t="word"}, 			--发包玩家
	{k = "lScore",t = "score"},		--当前发包额度
	{k = "cbThunderPoint",t = "byte"},		--雷号
	{k = "cbLeftCount",t = "byte"},		--剩余可抢个数
	{k = "cbLeftTime",t = "byte"},		--剩余抢包时间
	{k="wGetUser",t="word", l = {10}}, 		--已抢包玩家

	{k = "lBagKey", t = "score"}, -- 当前红包
	{k = "dBagDetail",t="double"}, 		--自己抢到的分数
	{k = "dUserScore", t = "double"},  -- 自己得到的分数
	{k = "bThunderPoint",t="bool"}, 	--自己是否中雷
}

------------------------------------------------------------------------------------------------
--服务器命令结构

cmd.SUB_S_START					    =101										--游戏开始
cmd.SUB_S_DISPATCH_BAG				=102									--派发红包
cmd.SUB_S_CANCEL_BAG				=103									--取消红包
cmd.SUB_S_GET_BAG					=104									--抢红包
cmd.SUB_S_GET_BAG_FAIL				=105									--抢包失败
cmd.SUB_S_END				        =106									--抢包结束

--游戏开始
cmd.CMD_S_GameStart	=
{
	{k="wBankerUser",t="word"}, 			--发包玩家
	{k="lScore",t="score"},			--红包额度						
	{k="cbThunderPoint",t="byte"},			--雷号
	{k="lBagKey",t="score"}			--标识			
};

--发红包
cmd.CMD_S_DispatchBag	=
{
	{k="wDispatchUser",t="word"},				--发包玩家
	{k="lScore",t="score"},				--红包额度
	{k="lBagKey",t="score"},	--红包标识		
};

--取消红包
cmd.CMD_S_CancelBag =
{
	{k="lBagKey",t="score"}		--申请玩家					
};

--抢红包
cmd.CMD_S_GetBag =
{
	{k="wGetUser",t="word"},		--抢包玩家					
	{k="fBagScore",t="double"},		--红包分数					
	{k="fUserScore",t="double"},	--玩家输赢分数					
	{k="bThunderPoint",t="bool"},		--中雷					
	{k="cbLeftCount",t="byte"},		--剩余个数					
};

--抢包失败
cmd.CMD_S_GetBag_Fail = 
{	
	{k="wGetUser",t="word"},		--抢包玩家
	{k="cbReason",t="byte"}	--失败原因				
};

--抢包结算
cmd.CMD_S_GameEnd = 
{
	{k="fBankerScore",t="double"},		--庄家得分		
	{k="cbGetCount",t="byte"},	--抢包个数
					
	--庄家信息
	{k="dBagScore",t="double", l = {10}},		--抢包分数
	{k="bThunderPoint",t="bool", l = {10}},		--是否中雷
	{k="dUserScore",t="double", l = {10}},		--玩家得分

};

---------------------------------------------------------------------------------------------------------------
--客户端命令结构

cmd.SUB_C_DISPATCH_BAG				=1									--派发红包
cmd.SUB_C_GET_BAG				=2									--抢红包

--发红包
cmd.CMD_C_DispatchBag = 
{
	{k="lScore",t="score"}, 		--红包额度
	{k="lThunderPoint",t="score"}		--雷号		
};
--抢红包
cmd.CMD_C_GetBag = 
{
	{k="lBagKey",t="score"}, 		--红包标识	
};


return cmd