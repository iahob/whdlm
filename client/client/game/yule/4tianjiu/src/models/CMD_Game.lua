local cmd={}
cmd.RES                     =   "game/yule/4tianjiu/res/"

--------------------------------------------------------------------------
--公共宏定义
cmd.VERSION_SERVER		=	    	appdf.VersionValue(7,0,1)				    --程序版本
cmd.VERSION_CLIENT		=		    appdf.VersionValue(7,0,1)				    --程序版本

cmd.KIND_ID				        =			59									--游戏 I D
cmd.GAME_NAME			        =			"4人牌九"					        --游戏名字
cmd.GAME_PLAYER			        =			4									--游戏人数

cmd.FULL_CARD_COUNT			    =			28									--全副扑克
cmd.SEL_ADDSCORE_CONFIG			=			3									--可选后台加注配置(底分N倍)
cmd.MAX_PERSONALROOM_TURNCOUNT	=			32									--私人房最大局数
cmd.HAND_CARD_COUNT			    =			2									--手牌数目
cmd.MY_VIEWID			        =			3									--自己视图位置
cmd.MAX_CARD_COUNT			    =			32									--扑克数目
cmd.handCardNum			        =			4									--手牌数
cmd.BET_COUNT			        =			3									--下注个数
cmd.MAX_SHOW_CARD_COUNT			=			3									--亮牌最大数目
cmd.BET_SCORE_COUNT			    =			6									--下注个数

cmd.actRob			            =			1									--抢庄中
cmd.actChip			            =			2									--下注中
cmd.actCombine			        =			3									--组牌中

cmd.SERVER_LEN			        =			32									--房间长度

--比较结果
cmd.COMPCARD_WIN_3		        =		    3				    --比较胜利(赢三道)
cmd.COMPCARD_WIN_2		        =		    2					--比较胜利(赢二道)
cmd.COMPCARD_WIN_1		        =		    1					--比较胜利(赢一道)
cmd.COMPCARD_EQUAL		        =		    0					--比较相同
cmd.COMPCARD_LOSE_1		        =		    -1					--比较失败(输一道)
cmd.COMPCARD_LOSE_2		        =		    -2					--比较失败(输二道)
cmd.COMPCARD_LOSE_3		        =		    -3					--比较失败(输三道)

-- cmd.GER_NORMAL =0
-- cmd.GER_DISMISS=1
-- cmd.GER_USER_LEAVE=2

-- -- 语音动画
-- cmd.VOICE_ANIMATION_KEY = "voice_ani_key"

--游戏状态
cmd.GAME_STATUS_FREE		=		    	0					--等待开始
cmd.GAME_STATUS_ROBBANKER	=			    101					--玩家抢庄
cmd.GAME_STATUS_CHIP		=		    	102					--玩家下注
cmd.GAME_SCENE_OPEN_CARD	=		    	104					--玩家开牌
cmd.GAME_GAME_END	        =		    	105					--玩家结束

--用户状态
cmd.PS_NONE                 =                0                  --无人
cmd.PS_SITTING              =                1                  --游戏中   
cmd.PS_DYNAMIC              =                2                  --动态加入

--倒计时
cmd.IDI_STARTGAME=201
cmd.IDI_CALLBANKER=202
cmd.IDI_SETCHIP=203
cmd.IDI_RANGECARD=204

--四人场
cmd.MY_VIEWID = 3
cmd.LEFT_VIEWID=2 
cmd.RIGHT_VIEWID=4 
cmd.TOP_VIEWID=1

-- 语音动画
cmd.VOICE_ANIMATION_KEY = "voice_ani_key"

----游戏音乐音效常量
--背景音乐
cmd.background = "bg_game.mp3";
--游戏开始
cmd.GAME_START = "GAME_START.WAV";
--摇骰子
cmd.DRAW_SICE = "shaizi.mp3";
--金币移动
cmd.MOVE_GOLD = "MOVE_GOLD.wav";
--玩家赢
cmd.GAME_WIN = "win.mp3";
--玩家输
cmd.GAME_LOST = "lose.mp3";
--游戏结束
cmd.GAME_END = "GAME_END.WAV";
--时钟
cmd.GAME_WARN = "GAME_WARN.WAV";
--发牌
cmd.SEND_CARD = "fapai.mp3";
--发牌
cmd.OUT_CARD = "fapai.mp3";
--扣牌
cmd.KOU_CARD = "koupai.mp3";
--选牌
cmd.XUAN_CARD = "xuanpai.mp3";
--选牌
cmd.MAN = "man/";
--选牌
cmd.WOMAN = "woman/";

--------------------------------------------------------------------------
--服务端返回协议
cmd.SUB_S_GAME_START	   =			101									--游戏开始
cmd.SUB_S_ROB_BANKER	   =			102									--玩家抢庄
cmd.SUB_S_ROB_RESULT	   =			103									--抢庄结果
cmd.SUB_S_CHIP_RESULT	   =			104									--下注结果
cmd.SUB_S_START_OPEN_CARD  =			105									--玩家开牌
cmd.SUB_S_OPEN_CARD	       =			106									--开牌结果
cmd.SUB_S_GAME_END		   =			107									--游戏结束
cmd.SUB_S_CHNAGE_BANKER    =			108									--庄家换庄
cmd.SUB_S_WINLOSE_INFO	   =			109									--胜负信息
cmd.SUB_S_TRUSTEE	       =			110									--玩家托管
cmd.SUB_S_GAME_FREE	       =			111									--空闲状态
--------------------------------------------------------------------------

--游戏配置
cmd.tagGameConfig =
{
	--房间类型
	{t="byte",k="cbPlayMode"},							        --约战游戏模式（0：积分场 1：金币场）
	--时间定义
	{t="byte",k="cbTimeStartGame"},							    --开始时间
	{t="byte",k="cbTimeRobBanker"},							    --抢庄时间
	{t="byte",k="cbTimeChip"},							        --下注时间
	{t="byte",k="cbTimeOpenCard"},							    --理牌时间
	{t="byte",k="cbTimeChangeBanker"},							--切庄时间
	--游戏配置
	{t="score",k="dBetScore",l={cmd.BET_SCORE_COUNT}},			--下注个数
	{t="bool",k="bJiaGuaPaiJiu"},							    --加锅牌九
	{t="bool",k="bDaPaiJiu"},							        --大小牌九


	{t="byte",k="cbBankerMode"},							    --庄家模式（0：轮庄 1：抢庄 2：霸王庄）
	{t="int",k="nChangeBankerCount"},							--换庄局数
	{t="byte",k="cbBankerOpenCardCount"},						--庄家亮牌数
	{t="byte",k="cbBetDaoShu"},							        --下注道数（2道 3道）
	{t="bool",k="bAutoOpenCard"},							    --自动开牌
	{t="bool",k="bSelectScore"},							    --每局选分
	{t="score",k="dGuoDiScore"},							    --锅底分数
	{t="score",k="dMinBetScore"},							    --最小下注
	{t="bool",k="bHeadAllBet"},							        --头局下满
	--牌型配置
	{t="bool",k="bZhaDan"},							            --炸弹
	{t="bool",k="bDiJiuNiangNiang"},					        --地九娘娘
	{t="bool",k="bGuiZi"},							            --鬼子
	{t="bool",k="bTianJiuWang"}							        --天九王
}
--断线重连
--空闲状态
cmd.CMD_S_StatusFree=
{
	{k = "GameConfig",t = "table", d = cmd.tagGameConfig},      --游戏配置

	--游戏变量
	{t="word",k="wBankerUser"},                                 --庄家位置
	{t="int",k="nBankerCount"},                                 --坐庄次数
	{t="score",k="dBankerMoney"},                               --坐庄金币（加锅模式为锅底的分，非加锅为身上的分）

	--托管变量
	{t="byte",k="cbTrusteeMode",l={cmd.GAME_PLAYER}},			--托管模式 （0：取消托管 1：下注最小筹码 2：下注最大筹码）
}

--抢庄状态
cmd.CMD_S_StatusRobBanker=
{
	{k = "GameConfig",t = "table", d = cmd.tagGameConfig},      --游戏配置

	{t="byte",k="cbTimeRemain"},							    --剩余时间
	--游戏变量
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--用户状态（0：无人 1：游戏中 2：动态加入）
	--抢庄信息
	{t="bool",k="bHaveRobBanker",l={cmd.GAME_PLAYER}},			--是否抢庄
	{t="bool",k="bRobBanker",l={cmd.GAME_PLAYER}},			    --抢庄信息
	--托管变量
	{t="byte",k="cbTrusteeMode",l={cmd.GAME_PLAYER}},			--托管模式（0：取消托管 1：下注最小筹码 2：下注最大筹码）
}

--下注状态
cmd.CMD_S_StatusChip=
{
	{k = "GameConfig",t = "table", d = cmd.tagGameConfig},      --游戏配置

	{t="byte",k="cbTimeRemain"},							    --剩余时间
	--游戏变量
	{t="word",k="wBankerUser"},							        --庄家位置
	{t="int",k="nBankerCount"},							        --坐庄次数
	{t="score",k="dBankerMoney"},							    --坐庄金币（加锅模式为锅底的分，非加锅为身上的分）
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--用户状态（0：无人 1：游戏中 2：动态加入）
	--抢庄信息
	{t="bool",k="bHaveChip",l={cmd.GAME_PLAYER}},			    --是否下注
	{t="score",k="dUserChip",l={cmd.BET_COUNT, cmd.BET_COUNT, cmd.BET_COUNT, cmd.BET_COUNT}},			    --玩家下注
	--托管变量
	{t="byte",k="cbTrusteeMode",l={cmd.GAME_PLAYER}},			--托管模式（0：取消托管 1：下注最小筹码 2：下注最大筹码）
}

--开牌状态
cmd.CMD_S_StatusOpenCard=
{
	{k = "GameConfig",t = "table", d = cmd.tagGameConfig},      --游戏配置

	{t="byte",k="cbTimeRemain"},							    --剩余时间
	--游戏变量
	{t="word",k="wBankerUser"},							        --庄家位置
	{t="int",k="nBankerCount"},							        --坐庄次数
	{t="score",k="dBankerMoney"},							    --坐庄金币（加锅模式为锅底的分，非加锅为身上的分）
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--用户状态（0：无人 1：游戏中 2：动态加入）
	
	--下注信息
	{t="score",k="dUserChip",l={cmd.BET_COUNT, cmd.BET_COUNT, cmd.BET_COUNT, cmd.BET_COUNT}},			--玩家下注
	
	--开牌信息
	{t="dword",k="nUsedCardNum"},							        --剩余牌值(使用一个32位的数字来表示32张牌的使用情况)
	{t="bool",k="bHaveOpenCard",l={cmd.GAME_PLAYER}},			--是否开牌
	{t="byte",k="cbHandCard",l={cmd.HAND_CARD_COUNT * 2}},		--手牌信息(如果开牌就是组牌后的信息)
	{t="byte",k="cbAutoHandCard",l={cmd.HAND_CARD_COUNT * 2}},	--最优手牌
	{t="byte",k="cbBankerHandCard",l={cmd.MAX_SHOW_CARD_COUNT}},--庄家手牌
	--托管变量
	{t="byte",k="cbTrusteeMode",l={cmd.GAME_PLAYER}},			--托管模式（0：取消托管 1：下注最小筹码 2：下注最大筹码）
}

--结算状态
cmd.CMD_S_StatusGameEnd=
{
	{k = "GameConfig",t = "table", d = cmd.tagGameConfig},      --游戏配置

	{t="byte",k="cbTimeRemain"},							    --剩余时间
	--游戏变量
	{t="word",k="wBankerUser"},							        --庄家位置
	{t="int",k="nBankerCount"},							        --坐庄次数
	{t="score",k="dBankerMoney"},							    --坐庄金币（加锅模式为锅底的分，非加锅为身上的分）
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--用户状态（0：无人 1：游戏中 2：动态加入）

	--下注信息
	{t="score",k="dUserChip",l={cmd.BET_COUNT, cmd.BET_COUNT, cmd.BET_COUNT, cmd.BET_COUNT}},			--玩家下注
	
	--开牌信息
	{t="dword",k="nUsedCardNum"},							    --剩余牌值(使用一个32位的数字来表示32张牌的使用情况)
	{t="byte",k="cbOpenCard",l={cmd.HAND_CARD_COUNT * 2,cmd.HAND_CARD_COUNT * 2,cmd.HAND_CARD_COUNT * 2,cmd.HAND_CARD_COUNT * 2}},			--开牌信息
	{t="bool",k="bCompareInfo",l={2,2,2,2}},			        --比较信息
	{t="int",k="nWinScoreInfo",l={cmd.GAME_PLAYER}},			--输赢信息
	--结束信息
	{t="bool",k="bChangeBanker"},							    --是否换庄（加锅模式上庄局数到了选择切锅）
	{t="score",k="dGameTax",l={cmd.GAME_PLAYER}},			    --游戏服务费
	{t="score",k="dGameScore",l={cmd.GAME_PLAYER}},		        --游戏积分(已减服务费)
	--托管变量
	{t="byte",k="cbTrusteeMode",l={cmd.GAME_PLAYER}},			--托管模式（0：取消托管 1：下注最小筹码 2：下注最大筹码）

}


--游戏开始
cmd.CMD_S_GameStart=
{
	{t="word",k="wBankerUser"},						            --庄家椅号
	{t="score",k="dBankerMoney"},							    --坐庄钱数（加锅模式为锅底的分，非加锅为身上的分）
	{t="byte",k="cbPlayStatus",l={cmd.GAME_PLAYER}},			--游戏状态（0：无人 1：游戏中 2：动态加入）
}

--玩家抢庄
cmd.CMD_S_RobBanker=
{
	{t="word",k="wRobUser"},							        --抢庄玩家
	{t="bool",k="bRobBanker"},						            --是否抢庄
}

--抢庄结果
cmd.CMD_S_RobBankerResult=
{
	{t="word",k="wBankerUser"},						            --庄家椅号
	{t="score",k="dBankerMoney"},							    --坐庄钱数（加锅模式为锅底的分，非加锅为身上的分）
}

--下注结果
cmd.CMD_S_ChipResult	= 									
{
	{t="word",k="wChipUser"},						            --下注玩家	
	{t="score",k="dChip",l={cmd.BET_COUNT}},                    --本次加注
}

--开始开牌
cmd.CMD_S_StartOpenCard=
{
	{t="dword",k="nUsedCardNum"},							     --剩余牌值(使用一个32位的数字来表示32张牌的使用情况)
	{t="byte",k="cbBankerHandCard",l={cmd.MAX_SHOW_CARD_COUNT}}, --庄家手牌
	{t="byte",k="cbHandCard",l={cmd.HAND_CARD_COUNT*2}},	     --玩家手牌
	{t="byte",k="cbAutoHandCard",l={cmd.HAND_CARD_COUNT * 2}},	 --最优手牌
	{t="word",k="wSiceCount"},							         --色子信息
}

--玩家开牌
cmd.CMD_S_Open_Result=
{
	{t="word",k="wOpenUser"},							         --开牌玩家
}

--游戏结束
cmd.CMD_S_GameEnd=
{
	{t="byte",k="cbEndMode"},							         --结束模式(0：常规结束 1：游戏解散 4：游戏解散（空闲时间）)
	{t="bool",k="bChangeBanker"},							     --是否换庄（加锅模式上庄局数到了选择切锅）
	{t="word",k="wBankerUser"},                                  --庄家位置
	{t="score",k="dBankerMoney"},                                  --坐庄钱数（加锅模式为锅底的分，非加锅为身上的分）
	{t="byte",k="cbOpenCard",l={cmd.HAND_CARD_COUNT * 2,cmd.HAND_CARD_COUNT * 2,cmd.HAND_CARD_COUNT * 2,cmd.HAND_CARD_COUNT * 2}},			--开牌信息
	{t="bool",k="bCompareInfo",l={2,2,2,2}},			         --比较信息
	{t="int",k="nWinScoreInfo",l={cmd.GAME_PLAYER}},			 --输赢信息
	{t="score",k="dGameTax",l={cmd.GAME_PLAYER}},			     --游戏服务费
	{t="score",k="dGameScore",l={cmd.GAME_PLAYER}},			     --游戏积分(已减服务费)
}

--庄家换庄
cmd.CMD_S_ChangeBanker=
{
	{t="word",k="wBankerUser"},                                  --庄家位置（等于自己时为继续当庄，等于其他玩家时为轮庄，无效为下庄）
	{t="score",k="dBankerMoney"},                                  --坐庄钱数（加锅模式为锅底的分，非加锅为身上的分）
}

--托管模式
cmd.CMD_S_Trustee=
{
	{t="word",k="wChairID"},                                      --椅子ID
	{t="byte",k="cbTrusteeMode"},                                 --托管模式 （0：取消托管 1：下注最小筹码 2：下注最大筹码）
}

--胜负信息
cmd.CMD_S_WinLoseInfo=
{
	{t="score",k="dTotalGameScore",l={cmd.GAME_PLAYER}},		  --总结算分
	{t="int",k="WinInfo",l={3,3,3,3}}                             --胜负次数（胜利 通杀 通赔）
}

----------------------------------------------
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

cmd.CMD_S_QueryResult=
{
	{t="score",k="UserInfo",l={cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER,cmd.GAME_PLAYER}},				--胜负信息
	{t="int",k="nPlatCountBegin"},						--局数
	{t="int",k="nPlatCountEnd"},							--局数
};
----------------------------------------------------

--------------------------------------------------------------------------
--客户端命令结构
cmd.SUB_C_ROB_BANKER	        =			100									--玩家抢庄
cmd.SUB_C_CHIP			        =			101									--玩家下注
cmd.SUB_C_OPEN_CARD			    =			102									--玩家开牌
cmd.SUB_C_SHOW_SCORE_END		=			103									--结算结束
cmd.SUB_C_CHNAGE_BANKER	        =			104									--庄家换庄
cmd.SUB_C_TRUSTEE	            =			105									--玩家托管
--------------------------------------------------------------------------

--玩家抢庄
cmd.CMD_C_RobBanker=
{
	{t="bool",k="bRobBanker"},							   --是否抢庄
}

--玩家下注
cmd.CMD_C_Chip=
{
	{t="word",k="wUserID"},							       --玩家椅号
	{t="score",k="dChip",l={cmd.BET_COUNT}},               --下注额度
}

--玩家开牌
cmd.CMD_C_OpenCard=
{
	{t="byte",k="cbCardData",l={cmd.HAND_CARD_COUNT*2}}    --牌型组合
}

--玩家托管
cmd.CMD_C_Trustee=
{
	{t="byte",k="cbTrusteeMode"},                          --托管模式 （0：取消托管 1：下注最小筹码 2：下注最大筹码）
}

--庄家换庄
cmd.CMD_C_ChangeBanker=
{
	{t="bool",k="bChangeBanker"},                          --是否换庄
}


cmd.CMD_C_QueryInfo=
{
	{t="int",k="nPlatCountBegin"},					       --局数
	{t="int",k="nPlatCountEnd"}						       --局数
};

-------------------------------------------
--增加金币房卡流水
cmd.MAX_RECORD_COUNT			=	30									--房卡结算统计最大局数
cmd.SUB_C_REQUEST_RCRecord		=	12									--请求房卡记录
cmd.SUB_S_RECORD				=	113									--游戏记录
cmd.CMD_S_RECORD =
{
    {t="int",k="nCount"},
    {t="score",k="lDetailScore",l={cmd.MAX_RECORD_COUNT,cmd.MAX_RECORD_COUNT,cmd.MAX_RECORD_COUNT,cmd.MAX_RECORD_COUNT}},--单局结算分
    {t="score",k="lAllScore",l={cmd.GAME_PLAYER}},          --总结算分
    {t="byte",k="cbWinCount",l={cmd.GAME_PLAYER}},          --赢局
    {t="byte",k="cbLoseCount",l={cmd.GAME_PLAYER}},         --输局
    {t="byte",k="cbEqualCount",l={cmd.GAME_PLAYER}},        --平局      
};

return cmd
