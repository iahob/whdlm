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
cmd.VERSION =  appdf.VersionValue(7,0,1)
--游戏标识print
cmd.KIND_ID = 145
--游戏人数
cmd.GAME_PLAYER = 200
--游戏名字
cmd.GAME_NAME = "百人龙虎斗"

--其他定义
cmd.MAX_CHIP_COUNT            = 6                                  --筹码个数
cmd.MAX_SIMPLE_CHART_COUNT    = 20                                 --简洁状态最近的20局胜数结果
cmd.MAX_ADVANCED_CHART_COUNT  = 100                                --满足100局清空

--房间名长度
cmd.SERVER_LEN      = 32
cmd.GAME_STATUS_PLAY = 100

--游戏状态定义
cmd.GAME_SCENE_FREE   = 0                                   --空闲状态
cmd.GAME_SCENE_JETTON = cmd.GAME_STATUS_PLAY 				--下注状态
cmd.GAME_SCENE_END 	= cmd.GAME_STATUS_PLAY+1 			    --结束状态

--区域倍数
cmd.AREA_LONG_times      = 1							    --龙倍数
cmd.AREA_PING_times      = 8 								--平倍数
cmd.MULTIPLE_HU_times    = 1 								--虎倍数

--区域索引
cmd.AREA_LONG      = 0                              --龙索引
cmd.AREA_PING      = 1                              --平索引
cmd.AREA_HU        = 2                              --虎索引
cmd.AREA_ALL       = 3                              --合计索引

--计算结果
cmd.RESULT_WIN     = 1                              --赢
cmd.RESULT_LOSE    = -1                             --输

--客户端控制
cmd.CONTROLAREA = {
	-- AREA_INVALID = 3,
	LONG_ = 0,
	PING = 1,
	HU = 2
}

--占位
cmd.SEAT_LEFT1_INDEX			= 0                 --左一
cmd.SEAT_LEFT2_INDEX			= 1                 --左二
cmd.SEAT_LEFT3_INDEX			= 2                 --左三
cmd.SEAT_RIGHT1_INDEX			= 3                 --右一
cmd.SEAT_RIGHT2_INDEX			= 4                 --右二
cmd.SEAT_RIGHT3_INDEX			= 5                 --右三
cmd.MAX_OCCUPY_SEAT_COUNT		= 6                 --最大占位个数


cmd.CLIENT_CONTROL = {
	{k = "cbExcuteTimes", t = "byte"},						--执行次数
	{k = "longhuping", t = "table", d = cmd.CONTROLAREA} --龙虎平
}

--记录信息
cmd.tagServerGameRecord = {
	{k = "cbLong", t = "byte"},	      --龙点
	{k = "cbHu", t = "byte"}          --虎点
}

--下注信息
cmd.tagUserBet = {
	{k = "szNickName", t = "word", l = {32}},	       --用户昵称
	{k = "dwUserGameID", t = "dword"},                 --用户ID
	{k = "lUserStartScore", t = "score"},              --用户金币
	{k = "lUserWinLost", t = "score"},                 --用户金币
	{k = "lUserBet", t = "score", l = {cmd.AREA_ALL}}  --用户下注
}

--下注信息数组
cmd.tagCustomAndroid = {
	--坐庄
	{k = "nEnableRobotBanker", t = "bool"},       --是否做庄

	{k = "lRobotBankerCountMin", t = "score"},	      --坐庄次数
	{k = "lRobotBankerCountMax", t = "score"},        --坐庄次数
	{k = "lRobotListMinCount", t = "score"},         --列表人数
	{k = "lRobotListMaxCount", t = "score"},         --列表人数
	{k = "lRobotApplyBanker", t = "score"},       --最多申请个数
	{k = "lRobotWaitBanker", t = "score"},       --空盘重申
	
	--下注
	{k = "lRobotMinBetTime", t = "score"},	      --下注筹码个数
	{k = "lRobotMaxBetTime", t = "score"},        --下注筹码个数
	{k = "lRobotMinJetton", t = "score"},         --下注筹码金额
	{k = "lRobotMaxJetton", t = "score"},         --下注筹码金额
	{k = "lRobotBetMinCount", t = "score"},       --下注机器人数
	{k = "lRobotBetMaxCount", t = "score"},       --下注机器人数
	{k = "lRobotAreaLimit", t = "score"},         --区域限制

	--存取款
	{k = "lRobotScoreMin", t = "score"},          --金币下限
	{k = "lRobotScoreMax", t = "score"},          --金币上限
	{k = "lRobotBankGetMin", t = "score"},        --取款最小值
	{k = "lRobotBankGetMax", t = "score"},        --取款最大值
	{k = "lRobotBankStoMul", t = "score"},        --存款百分比
}

--空闲状态
cmd.CMD_S_StatusFree = {
	{k = "cbTimeLeave", t = "byte"},	      		       		    --剩余时间
	{k = "lUserMaxScore", t = "score"},                				--玩家最大下注金币
	{k = "lAreaLimitScore", t = "score"},              				--区域限制
	{k = "lChipArray", t = "score", l = {cmd.MAX_CHIP_COUNT}},      --筹码配置
	--上庄
	{k = "bEnableBanker", t = "bool"},                				--激活坐庄功能标志(若为false，有关变量可以忽略)
	{k = "wBankerUser", t = "word"},                				--当前庄家
	{k = "cbBankerTime", t = "word"},                				--庄家局数
	{k = "lBankerWinScore", t = "score"},                			--庄家赢分
	{k = "lBankerScore", t = "score"},                			    --庄家携带分数
	{k = "bEnableSysBanker", t = "bool"},                			--允许系统做庄标志
	{k = "lApplyBankerCondition", t = "score"},                		--申请条件
	--占位
	{k = "lOccupySeatFree", t = "score"}, 							--免费站位金币条件
	{k = "wOccupySeatChairIDArray", t = "word", l = {cmd.MAX_OCCUPY_SEAT_COUNT}},      --站位椅子ID

	{k = "CustomAndroid", t = "table", d = cmd.tagCustomAndroid}    --机器人配置
}

--游戏状态
cmd.CMD_S_StatusPlay = {
	{k = "cbTimeLeave", t = "byte"},        							--剩余时间
	{k = "cbGameStatus", t = "byte"},        							--游戏状态
	{k = "lUserMaxScore", t = "score"},         						--玩家最大下注
	{k = "lAreaLimitScore", t = "score"},         						--区域限制
	{k = "lChipArray", t = "score", l = {cmd.MAX_CHIP_COUNT}},          --筹码配置

	--桌面扑克
	{k = "cbTableCardArray", t = "byte", l = {2}},         				--桌面扑克

	--下注
	{k = "lAreaInAllScore", t = "score", l = {cmd.AREA_ALL}},	        --每个区域的总分
	{k = "lUserInAllScore", t = "score", l = {cmd.AREA_ALL}},           --每个玩家每个区域的下注
	
	--庄家信息
	{k = "bEnableBanker", t = "bool"},          						--激活坐庄功能标志(若为false，有关变量可以忽略)
	{k = "wBankerUser", t = "word"},          						    --当前庄家
	{k = "wBankerTime", t = "word"},          						    --庄家局数
	{k = "lBankerScore", t = "score"},          						--庄家携带分数
	{k = "bEnableSysBanker", t = "bool"},          						--允许系统做庄
	{k = "lApplyBankerCondition", t = "score"},          				--申请条件

	--占位
	{k = "lOccupySeatFree", t = "score"}, 							--免费站位金币条件
	{k = "wOccupySeatChairIDArray", t = "word", l = {cmd.MAX_OCCUPY_SEAT_COUNT}},      --站位椅子ID


	--结束信息
	{k = "lEndUserScore", t = "score"},          									--玩家成绩
	{k = "lBankerWinScore", t = "score"},                               			--庄家成绩
	{k = "lUserAllScore", t = "score", l = {cmd.GAME_PLAYER * cmd.AREA_ALL}},       --每个玩家每个区域的下注
	{k = "lUserWinScore", t = "score", l = {cmd.GAME_PLAYER}},      				--玩家成绩
	{k = "lUserAreaWinScore", t = "score", l = {cmd.GAME_PLAYER * cmd.AREA_ALL}},   --每个玩家每个区域的得分

	{k = "lOtherUserInAllScore", t = "score", l = {cmd.AREA_ALL}},	        --除去庄家和占座玩家和自己每个区域的下注
	{k = "lOtherUserAreaWinScore", t = "score", l = {cmd.AREA_ALL}},           --除去庄家和占座玩家和自己每个区域的得分(不扣税)
	{k = "lUserRevenue", t = "score", l = {cmd.GAME_PLAYER}},	        --除去庄家和占座玩家和自己每个区域的下注
	
	--机器人配置
	{k = "CustomAndroid", t = "table", d = cmd.tagCustomAndroid},       --机器人配置
}

--命令定义-服务器
cmd.SUB_S_GAME_FREE             = 99					--游戏空闲
cmd.SUB_S_GAME_START            = 100				    --游戏开始
cmd.SUB_S_PLACE_JETTON          = 101                   --用户下注
cmd.SUB_S_PLACE_JETTON_FAIL     = 102                   --下注失败
cmd.SUB_S_CONTINU_JETTON        = 103                   --续投
cmd.SUB_S_CONTINUE_JETTON_FAIL  = 104                   --续投失败
cmd.SUB_S_GAME_END              = 105                   --游戏结束

cmd.SUB_S_SEND_SIMPLE_CHART     = 106                   --简洁路单
cmd.SUB_S_SEND_ADVANCED_CHART   = 107                   --高级路单

cmd.SUB_S_OCCUPYSEAT            = 108                   --占位结果
cmd.SUB_S_OCCUPYSEAT_FAIL       = 109                   --占位失败
cmd.SUB_S_UPDATE_OCCUPYSEAT     = 110                   --更新占位
cmd.SUB_S_OCCUPYSEAT_STANDUP    = 111                   --占位起立
cmd.SUB_S_APPLY_BANKER          = 112                   --申请庄家
cmd.SUB_S_CANCEL_BANKER         = 113                   --取消申请
cmd.SUB_S_CHANGE_BANKER         = 114                   --切换庄家
cmd.SUB_S_BANKER_QUIT_RESULT    = 115                   --庄家在下装或者结算下庄
cmd.SUB_S_SYSTIP                = 116                   --系统提示
cmd.SUB_S_STATISTICS_RECORD     = 117					--统计玩家最近20局记录


--控制端相关
cmd.SUB_S_CONTROL_TEXT          = 1                   --控制提示
cmd.SUB_S_ROOM_DIFFICULTY_RES   = 2                   --房间难度结果
cmd.SUB_S_TEMP_CONFIG           = 3                   --临时配置

cmd.SUB_S_REFRESH_ROOMCTRL      = 4                   --更新房间控制列表
cmd.SUB_S_REFRESH_ROOM_STORAGE  = 5                   --更新房间用户列表
cmd.SUB_S_CONTROL_REFRESH_RULE  = 6                   --刷新配置

cmd.SUB_S_REFRESH_ROOM_YESTERDAYRES   = 7             --刷新昨日数据结果
cmd.SUB_S_ADMIN_COMMDN          = 8                   --系统控制
cmd.SUB_S_SEND_USER_BET_INFO    = 9                   --发送下注
cmd.SUB_S_QUERY_TABLE_ADVANCE_RES    = 9              --查询收放表结果

--失败结构
cmd.CMD_S_PlaceJettonFail = {
	{k = "wPlaceUser", t = "word"},        							--下注玩家
	{k = "lJettonArea", t = "byte"},        					    --下注区域
	{k = "lPlaceScore", t = "score"},        					    --当前下注
}

--续投
cmd.CMD_S_ContiueJetton = {
	{k = "lAreaInAllScore", t = "score", l = {cmd.AREA_ALL}},       --每个区域的总下注
	{k = "lCurUserInAllScore", t = "score", l = {cmd.AREA_ALL}},    --当前玩家每个区域的下注
	{k = "wPlaceUser", t = "word"},                                 --续投玩家
}

--发送下注
cmd.CMD_S_SendUserBetInfo = {
	{k = "lUserStartScore", t = "score", l = {cmd.GAME_PLAYER}},         			--起始分数
	-- {k = "lUserJettonScore", t = "dword", l = {}},        					    --个人总注????????
}

--申请庄家
cmd.CMD_S_ApplyBanker = {
	{k = "wApplyUser", t = "word"},         						--申请玩家
}

--系统提示
cmd.CMD_S_SysTip = {
	{k = "szTip", t = "dword", l = {512}},         					--系统提示
}

--取消申请
cmd.CMD_S_CancelBanker = {
	{k = "wCancelUser", t = "word"},         						--取消玩家
}

--切换庄家
cmd.CMD_S_ChangeBanker = {
	{k = "wBankerUser", t = "word"},         						--当前庄家
	{k = "lBankerScore", t = "score"},         						--庄家金币
}

--游戏空闲
cmd.CMD_S_GameFree = {
	{k = "cbTimeLeave", t = "byte"},        						--剩余时间
	{k = "nListUserCount", t = "dword"},        					--列表人数
}

--游戏开始
cmd.CMD_S_GameStart = {
	{k = "wBankerUser", t = "word"},        						--庄家位置
	{k = "lBankerScore", t = "score"},        						--庄家携带金币
	{k = "lUserMaxScore", t = "score"},        						--玩家最大下注金币
	{k = "cbTimeLeave", t = "byte"},        						--剩余时间
	{k = "nChipRobotCount", t = "dword"},        					--人数上限(下注机器人)
}

--用户下注
cmd.CMD_S_PlaceJetton = {
	{k = "wChairID", t = "word"},        								--用户位置
	{k = "cbJettonArea", t = "byte"},        				    		--筹码区域
	{k = "lJettonScore", t = "score"},        							--加注数目
	{k = "cbAndroid", t = "byte"},        					   		 	--机器人
}

--游戏结束
cmd.CMD_S_GameEnd = {
	{k = "cbTimeLeave", t = "byte"},        							--剩余时间

	{k = "cbTableCardArray", t = "byte", l = {2}},        		    	--桌面扑克
	
	{k = "wBankerTime", t = "word"},        		    	            --做庄次数

	{k = "lUserScore", t = "score"},        					    	--玩家成绩
	{k = "lBankerWinScore", t = "score"},        					    --庄家成绩
	{k = "lUserInAllScore", t = "score", l = {cmd.GAME_PLAYER * cmd.AREA_ALL}},  		--每个玩家每个区域的下注   
	
	{k = "lUserWinScore", t = "score", l = {cmd.GAME_PLAYER}},                           --玩家成绩
	{k = "lUserAreaWinScore", t = "score", l = {cmd.GAME_PLAYER * cmd.AREA_ALL}},       	--每个玩家每个区域的得分
	-- {k = "lUserRevenue", t = "score", l = {cmd.GAME_PLAYER}}       	    --玩家税收
	{k = "lOtherUserInAllScore", t = "score", l = {cmd.AREA_ALL}},	        --除去庄家和占座玩家和自己每个区域的下注
	{k = "lOtherUserAreaWinScore", t = "score", l = {cmd.AREA_ALL}},        --除去庄家和占座玩家和自己每个区域的得分(不扣税)
	{k = "lUserRevenue", t = "score", l = {cmd.GAME_PLAYER}},	            --除去庄家和占座玩家和自己每个区域的下注
}

--简洁路单
cmd.CMD_S_SimpleChart = {
	{k = "ServerGameRecord", t = "table", d = cmd.tagServerGameRecord},   
	{k = "cbGameStatus", t = "byte"},        							--游戏状态
}

--高级路单
cmd.CMD_S_AdvancedChart = {
	{k = "ServerGameRecord", t = "table", d = cmd.tagServerGameRecord},  
	{k = "cbGameStatus", t = "byte"},        							--游戏状态   
}

--占位结果
cmd.CMD_S_OccupySeat = {
	{k = "wOccupySeatChairID", t = "word"},        			                        --申请占位玩家ID	
	{k = "cbOccupySeatIndex", t = "byte"},        					                --占位索引
	{k = "wOccupySeatChairIDArray", t = "word", l = {cmd.MAX_OCCUPY_SEAT_COUNT}},   --占位椅子ID    					 
}

--占位失败
cmd.CMD_S_OccupySeat_Fail = {
	{k = "bSeatBusy", t = "bool"},        				                            --座位有人标识
	{k = "wAlreadyOccupySeatChairID", t = "word"},        	                        --已申请占位玩家ID				 
	{k = "cbAlreadyOccupySeatIndex", t = "byte"},        					        --已占位索引  
	{k = "wOccupySeatChairIDArray", t = "word", l = {cmd.MAX_OCCUPY_SEAT_COUNT}},   --占位椅子ID     					
}

--更新占位
cmd.CMD_S_UpdateOccupySeat = {
	{k = "wOccupySeatChairIDArray", t = "word", l = {cmd.MAX_OCCUPY_SEAT_COUNT}},       --占位椅子ID	
	{k = "wQuitOccupySeatChairID", t = "word"},        					                --申请退出占位玩家
}

--占位起立
cmd.CMD_S_OccupySeatStandUp = {
	{k = "wOccupySeatChairIDArray", t = "word", l = {cmd.MAX_OCCUPY_SEAT_COUNT}},       --占位椅子ID	
	{k = "wStandUpOccupySeatChairID", t = "word"},        					            --退出占位玩家
}
--控制服务端信息
cmd.CMD_S_ADMIN_STORAGE_INFO = {
	{k = "lCurrentStorage", t = "score"},        				
	{k = "lCurrentDeduct", t = "score"},        					 
	{k = "lMaxStorage", t = "score", l = {2}},        					 
	{k = "wStorageMul", t = "word", l = {2}},        					
}

cmd.CMD_C_UpdateStorage = {
	{k = "lStorageStart", t = "score"},        --库存数值   					
	{k = "lStorageDeduct", t = "score"},       --库存数值   							
} 

cmd.CMD_C_ModifyStorage = {
	{k = "lMaxStorage", t = "score", l = {2}}, --库存上限    					 
	{k = "wStorageMul", t = "word", l = {2}},  --吐分概率
}


--请求回复
cmd.CMD_S_CommandResult = {
	{k = "cbAnswerType", t = "byte"}, --回复类型    					 
	ANSWER_SET_WIN_AREA = 111,
	ANSWER_CANCEL_CONTROL = 112,
	{k = "cbResult", t = "byte"},    

	CR_ACCEPT = 2,                             --接受
	CR_REFUSAL = 3,                            --拒绝
}

cmd.CMD_C_AdminReq = {
	{k = "cbReqType", t = "byte"},    
	RQ_SET_WIN_AREA = 11,
	RQ_CANCEL_CONTROL = 21,
	{k = "clientControl", t = "table", d = cmd.clientControl}       --客户端控制
}

--命令定义-客户端
cmd.SUB_C_PLACE_JETTON		= 1                --用户下注
cmd.SUB_C_CONTINUE_JETTON   = 2                --续投
cmd.SUB_C_OCCUPYSEAT		= 3                --占位
cmd.SUB_C_QUIT_OCCUPYSEAT   = 4                --退出占位
cmd.SUB_C_APPLY_BANKER		= 5                --申请庄家
cmd.SUB_C_CANCEL_BANKER     = 6                --取消申请
cmd.SUB_C_REQUEST_STATISTICS_RECORD = 7		   --请求 玩家列表最近20局统计数据

--控制端相关
cmd.SUB_C_REFRESH_ROOM_STORAGESTATUS		= 8               --更新库存
cmd.SUB_C_REFRESH_ROOM_YESTERDAY   			= 9               --刷新昨日数据
cmd.SUB_C_ROOM_DIFFICULTY					= 10              --房间难度控制
cmd.SUB_C_REFRESH_RULE   					= 11              --刷新配置
cmd.SUB_C_MODIFY_RULE						= 12              --修改配置
cmd.SUB_C_MODIFY_REPLACE_RULE    			= 13              --修改并替换配置
cmd.SUB_C_REFRESH_ROOMCTRL     				= 14              --刷新房间控制列表
cmd.SUB_C_AMDIN_COMMAND     				= 15              --系统控制
cmd.SUB_C_QUERY_TABLE_ADVANCE     			= 16              --查询收放表(收放页面)

--用户下注
cmd.CMD_C_PlaceJetton = {
	{k = "cbJettonArea", t = "byte"}, 			--筹码区域    					 
	{k = "lJettonScore", t = "score"},  		--加注数目
}

--占位
cmd.CMD_C_OccupySeat = {
	{k = "wOccupySeatChairID", t = "word"}, 	    --占位玩家    					 
	{k = "cbOccupySeatIndex", t = "byte"},  		--占位索引
}

--房间难度控制
-- cmd.CMD_C_RoomDifficulty = {
	
-- }

--统计玩家最近20局记录
cmd.CMD_S_StatisticsRecord = {
	{k = "wChairID", t = "word"},        							--用户椅子ID
	{k = "lRecordInAllScore", t = "score"},							--最近20局总下注
	{k = "lWinCount", t = "score"}									--最近20局胜局(0分不累加胜局)
}


cmd.numTextFormat = function (lScore)
	-- body
    if lScore >= 100000000 then
        lScore = string.format("%.2f", lScore  / 100000000)
        return lScore,false,true
    elseif lScore >= 10000 then
        lScore = string.format("%.2f", lScore  / 10000)
        return lScore,true,false
    else
        return lScore,false,false
    end

end

return cmd