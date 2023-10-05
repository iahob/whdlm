local cmd =  {}
cmd.RES_PATH                    =   "game/yule/runfast/res/"
--游戏版本
cmd.VERSION 					= appdf.VersionValue(6,7,0,1)
cmd.TXT	  					    = "跑得快房卡版"
--游戏标识
cmd.KIND_ID						= 240
--游戏人数
cmd.GAME_PLAYER					= 3
--视图位置(1~3)
cmd.MY_VIEWID					= 2

--正常牌数
cmd.NORMAL_CARD_COUNT			= 16
--牌间距
cmd.CARD_SPACING 				= 30
--牌宽
cmd.CARD_WIDTH 					= 143
--牌高
cmd.CARD_HEIGHT 				= 194

--********************        扑克类型        *************--
--错误类型
cmd.CT_ERROR					= 0
--单牌类型
cmd.CT_SINGLE					= 1
--单连类型
cmd.CT_SINGLE_LINE				= 2
--对连类型
cmd.CT_DOUBLE_LINE				= 3
--三连类型
cmd.CT_THREE_LINE				= 4--最后能出
--三带一单
cmd.CT_THREE_LINE_TAKE_SINGLE	= 5--最后能出
--三带二单
cmd.CT_THREE_TAKE_TWO_SINGLE    = 6
--三带一对
cmd.CT_THREE_LINE_TAKE_DOUBLE	= 7

--2连3同带2单
cmd.CT_TWO_THREE_LINE_TAKE_TWO_SINGLE = 8 --最后能出
--2连3同带1对
cmd.CT_TWO_THREE_LINE_TAKE_ONE_DOUBLE = 9--最后能出
--2连3同带4单
cmd.CT_TWO_THREE_LINE_TAKE_FOUR_SINGLE= 10
--2连3同带2对
cmd.CT_TWO_THREE_LINE_TAKE_TWO_DOUBLE = 11
--2连3同带2单1对
cmd.CT_TWO_THREE_LINE_TAKE_OTHER      = 12

--3连3同带3张
cmd.CT_THREE_THREE_LINE_TAKE_THREE_SINGLE     = 13 --最后能出
--3连3同带6张

cmd.CT_THREE_THREE_LINE_TAKE_SIX_SINGLE     = 14

--4连3同，带4张
cmd.CT_FOUR_THREE_LINE_TAKE_FOUR_SINGLE     = 15  --4连3同，首出时可直接出完


--炸弹类型
cmd.CT_BOMB						= 16
cmd.CT_FOUR_TAKE_THREE				  = 17

-- 语音动画
cmd.VOICE_ANIMATION_KEY = "voice_ani_key"


--********************       定时器标识         ***************--
--出牌定时器
cmd.IDI_OUT_CARD				= 200
--开始定时器
cmd.IDI_START_GAME				= 202

--*******************        时间标识         *****************--
--快速出牌时间
cmd.TIME_OUT_CARD_FAST			= 10
--出牌时间
cmd.TIME_OUT_CARD				= 30
--准备时间
cmd.TIME_START_GAME 			= 30


--******************         游戏状态             ************--
--等待开始
cmd.GAME_SCENE_FREE				= 0
--叫庄状态
cmd.GAME_SCENE_PLAY				= 100

--叫庄状态
cmd.GAME_SCENE_GAMEEND				= 101

--*********************      服务器命令结构       ************--
--游戏开始
cmd.SUB_S_GAME_START			= 101

--玩家出牌
cmd.SUB_S_OUT_CARD				= 102

--玩家过牌
cmd.SUB_S_PASS_CARD				= 103

--游戏结束
cmd.SUB_S_GAME_END				= 104

--玩家托管
cmd.SUB_S_AUTOMATISM			= 105

--房卡记录
cmd.SUB_S_ROOMCARD_RECORD		= 116									

--空闲状态
cmd.CMD_S_StatusFree = 
{
	--基础积分
	{k = "lBaseScore", t = "score"},
	--历史积分
	{k = "lTurnScore", t = "score", l = {cmd.GAME_PLAYER}},
	{k = "lCollectScore", t = "score", l = {cmd.GAME_PLAYER}},
	--托管状态
 	{k = "bAutoStatus", t = "bool", l = {cmd.GAME_PLAYER}},

 	{k = "wPlayerCount", t = "word"}, --参与人数
 	{k = "bSixTheenCard" , t = "bool"},--游戏模式 16 or 15
 	{k = "bNoForceClose", t = "bool"},--不可强关
 	{k = "bNoRemoveBomb", t = "bool"},--炸弹不可拆
 	{k = "bSpadeBanker", t = "bool"},--黑桃3庄
 	{k = "bFirstOutSpade", t = "bool"},--首出必带黑桃3
 	 {k = "bForceOutCard", t = "bool"},--有大必出
 	{k = "bEchelonConclude", t = "bool"},--梯度结算
 	{k = "cbPlayMode", t = "byte"}, --游戏模式
 	{k = "wServerLevel", t = "word"},--房间等级
 	{k = "bRemainCardCount", t = "byte", l = {13}},

 	{k = "cbTimeOutCard", t = "byte"}, 	--出牌时间
 	{k = "cbTimeGameEnd", t = "byte"}, 	--结算时间
 	{k = "cbTimeReady", t = "byte"}, 	--准备时间

}
--游戏状态
cmd.CMD_S_StatusPlay = 
{
	--基础积分
	{k = "lBaseScore", t = "score"},
	--庄家用户
	{k = "wBankerUser", t = "word"},
	--上局赢者
	{k = "wBeforWiner", t = "word"}, 
	--出牌的人
	{k = "wLastOutUser", t = "word"},
	--当前玩家
	{k = "wCurrentUser", t = "word"},
	--手上扑克
	{k = "bCardData", t = "byte", l = {16}},
	-- --扑克数目
	{k = "bCardCount", t = "byte", l = {cmd.GAME_PLAYER}},
	-- --炸弹数目
	{k = "bBombCount", t = "byte", l = {cmd.GAME_PLAYER}},
	-- --出牌数目
	{k = "bTurnCardCount", t = "byte"},
	-- --出牌列表
	{k = "bTurnCardData", t = "byte", l = {16}},
	-- --总局得分
	{k = "lAllTurnScore", t = "score", l = {cmd.GAME_PLAYER}},
	--上局得分
	{k = "lLastTurnScore", t = "score", l = {cmd.GAME_PLAYER}},
	--托管状态
	{k = "bAutoStatus", t = "bool", l = {cmd.GAME_PLAYER}},
	--房间类型
	{k = "wRoomType", t = "word"},
	--历史积分
	{k = "lTurnScore", t = "score", l = {cmd.GAME_PLAYER}},
	{k = "lCollectScore", t = "score", l = {cmd.GAME_PLAYER}},
	
	{k = "wPlayerCount", t = "word"}, --参与人数
 	{k = "bSixTheenCard" , t = "bool"},--游戏模式 16 or 15
 	{k = "bNoForceClose", t = "bool"},--不可强关
 	{k = "bNoRemoveBomb", t = "bool"},--炸弹不可拆
 	{k = "bSpadeBanker", t = "bool"},--黑桃3庄
 	
 	{k = "cbSpadeFirstOutCard", t = "byte"},--黑桃3庄时首出的牌
 	{k = "cbBankerFirstOutCard", t = "byte"},--赢家庄时，第一局首出的牌

 	{k = "bFirstOutSpade", t = "bool"},--首出必带黑桃3
 	{k = "bForceOutCard", t = "bool"},--有大必出
 	{k = "bEchelonConclude", t = "bool"},--梯度结算
 	{k = "cbPlayMode", t = "byte"}, --游戏模式
 	{k = "wServerLevel", t = "word"},--房间等级
 	{k = "bRemainCardCount", t = "byte", l = {13}},
 	{k = "cbTimeRemain", t = "byte"}, --重连剩余时间
}
--发送扑克
cmd.CMD_S_GameStart = 
{
	--庄家用户
	{k = "wBankerUser", t = "word"},
	--上局赢者
	{k = "wBeforWiner", t = "word"}, 
	--当前玩家
	{k = "wCurrentUser", t = "word"},
	--扑克列表
	{k = "cbCardData", t = "byte", l = {16}},
	
	--{k = "wPlayerCount", t = "word"}, --参与人数
 	{k = "bSixTheenCard" , t = "bool"},--游戏模式 16 or 15
 	{k = "bNoForceClose", t = "bool"},--不可强关
 	{k = "bNoRemoveBomb", t = "bool"},--炸弹不可拆
 	{k = "bSpadeBanker", t = "bool"},--黑桃3庄

 	{k = "cbSpadeFirstOutCard", t = "byte"},--黑桃3庄时首出的牌
 	{k = "cbBankerFirstOutCard", t = "byte"},--赢家庄时，第一局首出的牌


 	{k = "bFirstOutSpade", t = "bool"},--首出必带黑桃3
 	{k = "bForceOutCard", t = "bool"},--有大必出
 	{k = "bEchelonConclude", t = "bool"},--梯度结算
 	{k = "bRemainCardCount", t = "byte", l = {13}},
}
--用户出牌
cmd.CMD_S_OutCard = 
{
	--扑克数目
	{k = "bCardCount", t = "byte"},
	--当前玩家
	{k = "wCurrentUser", t = "word"},
	--出牌玩家
	{k = "wOutCardUser", t = "word"},
	{k = "bRemainCardCount", t = "byte", l = {13}},
	 --扑克列表
	{k = "bCardData", t = "byte", l = {16}}
}
--放弃出牌
cmd.CMD_S_PassCard = 
{
	--一轮开始
	{k = "bNewTurn", t = "bool"},
	--放弃玩家
	{k = "wPassUser", t = "word"},
	--当前玩家
	{k = "wCurrentUser", t = "word"}
}
--游戏结束
cmd.CMD_S_GameEnd = 
{
	--游戏税收
	{k = "lGameTax", t = "score"},
	--游戏积分
	{k = "lGameScore", t = "score", l = {cmd.GAME_PLAYER}},
	--扑克数目
	{k = "bCardCount", t = "byte", l = {cmd.GAME_PLAYER}},
	--扑克列表
	{k = "bCardData", t = "byte", l = {48}},

	{k = "bBombCount", t = "byte", l = {cmd.GAME_PLAYER}},--炸弹数目
	{k = "lBombScore", t = "score", l = {cmd.GAME_PLAYER}},--炸弹分

	{k = "bRemainCardCount", t = "byte", l = {13}},
}

cmd.CMD_S_RoomCardRecord = 
{
    {k = "nCount", t = "word"},                                            --局数
    {k = "lDetailScore", t = "score", l = {30, 30, 30}},      			  --积分信息
    {k = "cbPlayStatus", t = "byte", l = {cmd.GAME_PLAYER}}, --玩家状态
    {k = "dwBomboCount", t = "dword", l = {cmd.GAME_PLAYER}}, --炸弹数量
}

--玩家托管事件
cmd.CMD_S_UserAutomatism = 
{
	--椅子号
	{k = "wChairID", t = "word"},
	--托管
	{k = "bTrusee", t = "bool"}
}


--**********************    客户端命令结构        ************--

cmd.SUB_C_OUT_CART				= 2			--用户出牌
cmd.SUB_C_PASS_CARD				= 3			--放弃出牌
cmd.SUB_C_AUTOMATISM				= 4			--托管消息
cmd.SUB_C_REQUEST_RCRecord			= 5			--房卡流水

--用户托管
cmd.CMD_C_Automatism = 
{
	{k = "bAutomatism", t = "bool"}
}
--出牌数据包
cmd.CMD_C_OutCard = 
{
	--出牌数目
	{k = "bCardCount", t = "byte"},
	--扑克列表
	{k = "bCardData", t = "byte", l = {16}}
}

return cmd