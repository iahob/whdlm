local cmd =  {}

-------------公共宏定义-------------
cmd.RES 						= "game/qipai/sangong/res/"
cmd.GAME 					    = "sangong/"
cmd.VERSION 					= appdf.VersionValue(7,0,1)             --游戏版本
cmd.KIND_ID						= 33                                    --游戏标识
cmd.GAME_PLAYER					= 8                                     --游戏人数
cmd.GAME_NAME					= "三公"                                --游戏名字
cmd.MAX_CARD_COUNT				= 3                                     --最大手牌数目
cmd.MAX_JETTON_AREA				= 6                                     --下注区域
cmd.MAX_TIMES				    = 9                                     --最大赔率
cmd.MAX_CARD				    = 52                                    --最大扑克数

cmd.EM_ROBBANKER_MAX            = 4                                     --最大抢庄倍数
cmd.EM_JETTON_MAX               = 4                                     --最大下注筹码数
cmd.CT_MAX                      = 13                                     --最大牌型数



-------------结束原因-------------
cmd.GER_NO_PLAYER				= 0x10                                  --没有玩家

cmd.MY_VIEWID					= 3                                     --视图位置

-------------游戏状态-------------
cmd.GAME_STATUS_FREE			= 0                                     --空闲状态
cmd.GAME_STATUS_PLAY			= 100                                   --游戏状态

cmd.EM_STATUS_FREE 				= cmd.GAME_STATUS_FREE					--等待开始
cmd.EM_STATUS_ROBBANKER			= cmd.GAME_STATUS_PLAY				    --叫庄
cmd.EM_STATUS_JETTON			= cmd.GAME_STATUS_PLAY+1				--下注状态
cmd.EM_STATUS_OPEN_CARD         = cmd.GAME_STATUS_PLAY+2                --摊牌
cmd.EM_STATUS_GAME_END          = cmd.GAME_STATUS_PLAY+3                --结算

cmd.IDC_BT_AGREED				= 200								    --同意按钮
cmd.IDC_BT_DISAGREED			= 201								    --取消按钮
cmd.IDC_BT_OK					= 202								    --确定按钮
cmd.IDC_BT_OKADD				= 203								    --确定输入
cmd.IDC_BT_CLOSE				= 204								    --关闭按钮
cmd.IDC_CALL_SCORE_ONE			= 205								    --1 号按钮
cmd.IDC_CALL_SCORE_TWO			= 206								    --2 号按钮
cmd.IDC_CALL_SCORE_THREE		= 207								    --3 号按钮


--玩家操作类型
cmd.EM_OPERATE_NULL             = 0                                   --
cmd.EM_OPERATE_READY            = 1                                   --准备
cmd.EM_OPERATE_GIVEUP           = 2                                   --放弃
cmd.EM_OPERATE_BANKER           = 3                                   --选庄
cmd.EM_OPERATE_JETTON           = 4                                   --下注
cmd.EM_OPERATE_ARRANGECARD      = 5                                   --搓牌
cmd.EM_OPERATE_OPEN             = 6                                   --摊牌
cmd.EM_OPERATE_MAX              = 7                                   --

--抢庄类型
cmd.EM_OP_BANKER_NULL           = 0                                   --
--定庄
cmd.EM_OP_BANKER_BWZ            = 1                                   --霸王庄
cmd.EM_OP_BANKER_LLZZ           = 2                                   --轮流坐庄
cmd.EM_OP_BANKER_PDZZ           = 3                                   --牌大坐庄

--抢庄
cmd.EM_OP_BANKER_MPQZ           = 4                                   --名牌抢庄
cmd.EM_OP_BANKER_ZYQZ           = 5                                   --自由抢庄

--通比
cmd.EM_OP_BANKER_TB             = 6                                   --通比
cmd.EM_OP_BANKER_MAX            = 7                                   --


cmd.SERVER_LEN					= 32 

cmd.EM_CT_TIMES_NULL            =  0                                   --无翻倍玩法
cmd.EM_CT_TIMES_TY              =  1                                   --通用翻倍玩法
cmd.EM_CT_TIMES_PZ              =  2                                   --配置翻倍玩法


-------------服务端命令结构-------------
cmd.SUB_S_GAME_CONFIG			= 100									--游戏配置(AI使用) 游戏开始
cmd.SUB_S_STATUS_CHANGE			= 101									--状态变更
cmd.SUB_S_SEND_CARD			    = 102									--发牌
cmd.SUB_S_OPERATE_BANKER		= 103									--抢庄操作回复
cmd.SUB_S_BANKER_RESULT		    = 104									--抢庄操作结果
cmd.SUB_S_JETTON_NOITFY			= 105									--下注通知
cmd.SUB_S_JETTON				= 106									--下注回复
cmd.SUB_S_OPEN_CARD				= 107									--摊牌回复
cmd.SUB_S_COMPARE_CARD			= 108									--比牌结果
cmd.SUB_S_GAME_END		        = 109									--结算
cmd.SUB_S_GAME_START            = 110                                   --游戏开始
cmd.SUB_S_GAME_RECORD           = 111                                   --游戏记录
cmd.SUB_S_ROOMCARD_RECORD       = 112                                   --房卡记录

--总结算分
cmd.CMD_S_RECORD =
{
    {t = "int", k = "nCount"},
    {t = "score", k = "lDetailScore", l = {30, 30, 30, 30, 30, 30, 30, 30}},    --积分信息（最大局数30）
    {t = "byte", k = "cbPlayStatus", l = {cmd.GAME_PLAYER}},              --用户状态
}


cmd.stCardInfo =
{
    {t = "byte", k = "cbCards", l = {cmd.MAX_CARD_COUNT}},
    {t = "byte", k = "cbCardType"},                                       --游戏状态
}



--空闲状态
cmd.CMD_S_StatusFree =
{
    {t = "byte", k = "cbStatus"},                                       --游戏状态

	--规则
	{t = "byte", k = "cbFreeTime"},					                    --空闲时间
	{t = "byte", k = "cbOperateBankerTime"},					        --叫庄时间
	{t = "byte", k = "cbJettonTime"},					                --下注时间
    {t = "byte", k = "cbOpenCardTime"},                                 --开牌时间
    {t = "byte", k = "cbGameEndTime"},                                  --结算时间

    {t = "byte", k = "cbRobBankerType"},                                --抢庄模式
    {t = "byte", k = "cbRobBankerRatio", l = {cmd.EM_ROBBANKER_MAX}},   --抢庄倍数
    {t = "score", k = "lJettonButton", l = {cmd.EM_JETTON_MAX}},       --下注筹码配置
    {t = "score", k = "lCardTypeTime", l = {cmd.CT_MAX}},               --当前牌型倍数

    {t = "byte", k = "cbCardTypeTimesRule"},                            --牌型倍数规则
    {t = "score", k = "dCellScore"},                                   --当前房间底分
    {t = "byte", k = "cbHeadIndex", l = {cmd.GAME_PLAYER}},            --当前玩家牌数据

    {t = "byte", k = "cbPlayMode"},                                     --房间模式 0 积分 1 金币
    {t = "bool", k = "bSupportTuizhu"},                                 --是否支持推注
}

--游戏状态
cmd.CMD_S_StatusPlay =
{
    {t = "table", k = "statusFree", d = cmd.CMD_S_StatusFree},           --状态和规则

	--状态信息
    {t = "byte", k = "cbStatusLeftTime"},                                 --状态剩余时间
    {t = "word", k = "wBankerChairID"},                                   --叫庄用户 无效值为INVALID_CHAIR(0xFF)
    {t = "score", k = "dJettonScore", l = {cmd.GAME_PLAYER}},            --下注分
    {t = "byte", k = "cbOperateType", l = {cmd.GAME_PLAYER}},             --操作类型
    {t = "table", k = "stUserCardInfo", d = cmd.stCardInfo, l = {cmd.GAME_PLAYER}}, --当前玩家牌数据
    {t = "word", k = "wRobBankerRatio", l = {cmd.GAME_PLAYER}},           --抢庄倍数
    {t = "bool", k = "bUserPlaying", l = {cmd.GAME_PLAYER}},              --true:游戏中 false：未参与游戏
    {t = "score", k = "dUserWinloseScore", l = {cmd.GAME_PLAYER}},                 --游戏得分
    {t = "score", k = "dMinScore"},                                     --最小下注
    {t = "score", k = "dMaxScore"},                                     --最大下注
    {t = "bool", k = "bCanTuizhu"},                                      --是否可推注
}


--游戏开始
cmd.CMD_S_GameStart =
{
    {t = "bool", k = "bUserPlaying", l = {cmd.GAME_PLAYER}},            --true:游戏中 false：未参与游戏
    {t = "bool", k = "bCanTuizhu", l = {cmd.GAME_PLAYER}},              --true:可推注 false：不可推注       
}


--状态改变
cmd.CMD_S_StatusChange =
{
    {t = "byte", k = "cbStatus"},                                       --状态标识
    {t = "byte", k = "cbTimeLeft"},                                     --状态剩余时间
    {t = "bool", k = "bStatusStart"},                                   --true-状态开始  false-状态结束
}

--发牌
cmd.CMD_S_SendCard =
{
    {t = "byte", k = "cbCards",  l = {3,3,3,3,3,3,3,3}},                --手牌，注意：如果是暗牌则填充为0
    {t = "byte", k = "cbCurCount"},                                     --当前显示数量
}


--抢庄回复
cmd.CMD_S_OperateBanker =
{
    {t = "word", k = "wChairID"},                                       --椅子ID
    {t = "word", k = "wRobBankerRatio"},                                --抢庄倍数
}

--抢庄结果
cmd.CMD_S_OperateBankerResult =
{
    {t = "word", k = "wBankerChairID"},                                 --当前庄家
}


--下注通知
cmd.CMD_S_JettonNotify =
{
    {t = "score", k = "dMinScore"},                                     --最小下注
    {t = "score", k = "dMaxScore"},                                     --最大下注
}


--下注回复
cmd.CMD_S_Jetton =
{
    {t = "word", k = "wChairID"},                                        --椅子号
    {t = "score", k = "dScore"},                                        --分数
}

--搓牌回复
cmd.CMD_S_ArrangeCard =
{
    {t = "word", k = "wChairID"},                                        --椅子号
}

--开牌回复
cmd.CMD_S_OpenCard =
{
    {t = "word", k = "wChairID"},                                        --椅子号
    {t = "table", k = "stCards", d = cmd.stCardInfo},                    --当前玩家牌数据
}

--比牌结果
cmd.CMD_S_CompareResult =
{
    {t = "table", k = "stUserCardInfo", d = cmd.stCardInfo, l = {cmd.GAME_PLAYER}}, --当前玩家牌数据
    {t = "byte", k = "bWinType", l = {cmd.GAME_PLAYER}},                            --比牌结果
}

--游戏结束
cmd.CMD_S_GameEnd =
{
    {t = "word", k = "wBankerChairID"},                                             --庄家椅子号
    {t = "table", k = "stUserCardInfo", d = cmd.stCardInfo, l = {cmd.GAME_PLAYER}}, --当前玩家牌数据
    {t = "score", k = "dUserWinloseScore", l = {cmd.GAME_PLAYER}},                 --游戏得分
    {t = "score", k = "dUserReveneue", l = {cmd.GAME_PLAYER}},				        --游戏税收
    
    {t = "score", k = "dUserScoreInfo", l = {8,8,8,8,8,8,8,8}},					--玩家得分数据(通比模式使用)
    {t = "byte", k = "bWinType", l = {cmd.GAME_PLAYER}},                       --游戏税收
    {t = "bool", k = "bDismiss"},                                         --true-为中途解散  false-正常结算
}


--托管
cmd.CMD_S_Trustee = 
{
    {t = "word", k = "wTrusteeUser"},
    {t = "bool", k = "bTrustee"},
}

--托管模式
cmd.CMD_S_ChipMode = {
    {t = "byte", k = "cbTrusteeMode"},
}
-------------客户端命令结构-------------
cmd.SUB_C_OPERATE_BANKER		= 1									    --用户叫庄
cmd.SUB_C_ADD_SCORE				= 2									    --用户加注
cmd.SUB_C_OPEN_CARD			    = 3                                     --请求摊牌

cmd.SUB_C_AMDIN_COMMAND			= 5									    --系统控制
cmd.SUB_C_TRUSTEE               = 6                                     --托管
cmd.SUB_C_CHIPMODE              = 7                                     --最大最小托管下注

--用户叫庄
cmd.CMD_C_OperateBanker =
{
    {t = "word", k = "wRobBankerRatio"},					            --抢庄倍数，0不抢
}

--用户加注
cmd.CMD_C_Jetton =
{
    {t = "score", k = "lScore"},				                        --加注数目
}
--用户托管
cmd.CMD_C_Trustee=
{
    {t="bool",k="bTrustee"},                            --是否托管  
    -- {t="byte",k="cbTrusteeMode"},                       --托管模式 0下注最小额度，1下注最大额度
}

--用户托管
cmd.CMD_C_ChipMode=
{
    {t="byte",k="cbTrusteeMode"},                       --托管模式 0下注最小额度，1下注最大额度
}

cmd.RQ_SET_WIN_AREA		        = 1
cmd.RQ_RESET_CONTROL	        = 2

cmd.RQ_PRINT_SYN		        = 3
cmd.CMD_C_AdminReq =
{
    {t = "byte", k = "cbReqType"},
    {t = "byte", k = "cbExtendData", l = {20}},				            --附加数据
}

--请求回复
cmd.ACK_SET_WIN_AREA             = 1
cmd.ACK_PRINT_SYN                = 2
cmd.ACK_RESET_CONTROL            = 3 
cmd.CR_ACCEPT                    = 2				                    --接受
cmd.CR_REFUSAL                   = 3				                    --拒绝
cmd.CMD_S_CommandResult =
{
    {t = "byte", k = "cbAckType"},				                        --回复类型
    {t = "byte", k = "cbResult"},
    {t = "byte", k = "cbExtendData", l = {20}},				            --附加数据
}

--控制区域信息
cmd.tagControlInfo =
{
    {t = "int", k = "nAreaWin"},			                            --控制区域
}

--服务器控制返回
cmd.S_CR_FAILURE				  = 0		                            --失败
cmd.S_CR_UPDATE_SUCCES			  = 1		                            --更新成功
cmd.S_CR_SET_SUCCESS			  = 2		                            --设置成功
cmd.S_CR_CANCEL_SUCCESS		      = 3		                            --取消成功
cmd.CMD_S_ControlReturns =
{
    {t = "byte", k = "cbReturnsType"},				                    --回复类型
    {t = "byte", k = "cbControlArea"},				                    --控制区域
    {t = "byte", k = "cbControlTimes"},				                    --控制次数
}

--客户端控制申请
cmd.C_CA_UPDATE					  = 1		                            --更新
cmd.C_CA_SET				      = 2		                            --设置
cmd.C_CA_CANCELS			      = 3		                            --取消
cmd.CMD_C_ControlApplication =
{
    {t = "byte", k = "cbControlAppType"},				                --申请类型
    {t = "byte", k = "cbControlArea"},				                    --控制区域
    {t = "byte", k = "cbControlTimes"},				                    --控制次数
}

-------------定时器标识-------------
cmd.IDI_START_GAME   			= 200                                   --开始定时器
cmd.IDI_USER_ADD_SCORE			= 201                                   --加注定时器
cmd.IDI_USER_COMPARE_CARD		= 202                                   --选比牌用户定时器
cmd.IDI_DISABLE					= 203                                   --过滤定时器

cmd.IDI_CALL_BANKER 			= 204                                   --叫庄定时器
cmd.IDI_OPEN_CARD 			    = 205                                   --开牌定时器

-------------时间标识-------------
cmd.TIME_START_GAME				= 30                                    --开始定时器
cmd.TIME_USER_ADD_SCORE			= 20                                    --加注定时器
cmd.TIME_USER_COMPARE_CARD		= 25                                    --比牌定时器

cmd.VOICE_ANIMATION_KEY = "voice_ani_key"                               -- 语音动画

return cmd