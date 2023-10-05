local cmd =  {}

cmd.RES 						= "game/yule/shrimp/res/"
--游戏版本
cmd.VERSION 					= appdf.VersionValue(6,7,0,1)
--游戏标识
cmd.KIND_ID						= 529
	
--游戏人数
cmd.GAME_PLAYER					= 11

--视图位置
cmd.MY_VIEWID					= 3

--空闲状态
cmd.GAME_STATUS_FREE			= 0
cmd.GAME_STATUS_PLAY			= 100
cmd.GAME_STATUS_END             = 101

--***************服务端命令结构**************--
--游戏空闲
cmd.SUB_S_GAME_FREE			    = 99
--游戏开始（摇骰子）
cmd.SUB_S_GAME_START			= 100
--用户下注
cmd.SUB_S_PLACE_JETTON			= 101
--游戏结束
cmd.SUB_S_GAME_END				= 102
--游戏记录
cmd.SUB_S_SEND_RECORD			= 103
--下注失败
cmd.SUB_S_PLACE_JETTON_FAIL		= 104
--取消申请
cmd.SUB_S_CANCEL_BANKER			= 105
--申请庄家
cmd.SUB_S_APPLY_BANKER			= 106
--切换庄家
cmd.SUB_S_CHANGE_BANKER		    = 107
--玩家位置
cmd.SUB_S_USER_CHAIR            = 108
--更新下注限制
cmd.SUB_S_BET_LIMIT             = 109
--用户本轮下注信息
cmd.SUB_S_USER_BET_INFO         = 110
--庄家摇骰子
cmd.SUB_S_BANKER_YAO            = 111
--庄家摇骰子
cmd.SUB_S_BANKER_OPEN           = 112
cmd.SUB_S_SCORE                 = 113

cmd.SUB_S_CONTROL               = 114
cmd.SUB_S_USER_RESULT           = 115 --查询用户结果
cmd.SUB_S_UPDATE                = 116 --更新库存
cmd.SUB_S_GAME_RECORD           = 117

--***************客户端命令结构**************--
--用户下注
cmd.SUB_C_PLACE_JETTON          = 1
--骰子记录
cmd.SUB_C_SEND_RECORD           = 2
--申请庄家
cmd.SUB_C_APPLY_BANKER          = 3
--取消申请
cmd.SUB_C_CANCEL_BANKER         = 4
--查询当前区域下注信息（用户点击挪单玩法按钮发送）
cmd.SUB_C_QURARY_ARERA_BET      = 5
--查询当前串下注信息（用户点击挪串玩法发送
cmd.SUB_C_QURARY_CHUAN_BET      = 6                                
--查询本轮玩家下注信息
cmd.SUB_C_QURARY_USER_BET       = 7
--查询当前压豹子下注信息（用户点击豹子玩法发送
cmd.SUB_C_QURARY_BAOZI_BET      = 8   
--用户点击连串玩法发送                              
cmd.SUB_C_QURARY_LCHUAN_BET     = 9
--庄家摇骰子
cmd.SUB_C_CANCEL_BANKER_YAO     = 10
--庄家摇骰子
cmd.SUB_C_CANCEL_BANKER_OPEN    = 11  
--游戏记录
cmd.SUB_C_GAME_RECORD           = 13

cmd.SUB_C_CONTROL               = 909           --客户端进行控制

cmd.SUB_C_UPDATE                = 14            --刷新库存
cmd.SUB_C_QUERY_USER            = 15            --查询用户

cmd.S_CR_FAILURE                = 20
cmd.S_CR_SET_SUCCESS            = 21
cmd.S_CR_CANCEL_SUCCESS         = 22




cmd.BETERROR = {
    [1] = "当前不是下注阶段",
    [2] = "当前下注超过用户积分限制",
    [3] = "下注总金额超过区域限制",
    [4] = "下注区域错误",
    [5] = "其它错误",
    [6] = "总下注分数超过庄家赔付总分",
    [7] = "下注总金额超过串玩法限制",
    [8] = "下注总金额超过豹子玩法限制",
    [9] = "超过挪串次数限制",
    [10] = "下注玩家逃跑",
    [11] = "用户积分不足"
}                  

------------------------------------------------消息结构体----------------------------------
--机器人存款取款
-- cmd.tagCustomAndroid =
-- {
--     {k = "lRobotScoreMin", t = "score"},                      
--     {k = "lRobotScoreMax", t = "score"},                          
--     {k = "lRobotBankGet", t = "score"},                            
--     {k = "lRobotBankGetBanker", t = "score"},                         
--     {k = "lRobotBankStoMul", t = "score"},                             
-- };

cmd.CustomAndroid = {
    --坐庄
    {k = "nEnableRobotBanker", t = "bool"},--是否做庄
    {k = "lRobotBankerCountMin", t = "score"},--坐庄次数
    {k = "lRobotBankerCountMax", t = "score"},--坐庄次数
    {k = "lRobotListMinCount", t = "score"},--列表人数
    {k = "lRobotListMaxCount", t = "score"},--列表人数
    {k = "lRobotApplyBanker", t = "score"},--最多申请个数
    {k = "lRobotWaitBanker", t = "score"}, --空盘重申

    --下注
    {k = "lRobotMinBetTime", t = "score"}, --下注筹码个数
    {k = "lRobotMaxBetTime", t = "score"}, --下注筹码个数
    {k = "lRobotMaxJetton", t = "score"},  --下注筹码金额
    {k = "lRobotMaxJetton", t = "score"},  --下注筹码金额
    {k = "lRobotBetMinCount", t = "score"},--下注机器人数
    {k = "lRobotBetMaxCount", t = "score"},--下注机器人数
    {k = "lRobotAreaLimit", t = "score"},  --区域限制

    --存取款
    {k = "lRobotScoreMin", t = "score"}, --金币下限
    {k = "lRobotScoreMax", t = "score"}, --金币上限
    {k = "lRobotBankGetMin", t = "score"},--取款最小值
    {k = "lRobotBankGetMax", t = "score"},--取款最大值
    {k = "lRobotBankStoMul", t = "score"},--存款百分比
}

cmd.CMD_S_banker_sice = 
{
    {k = "wUserChair",t = "word"}
}

-- --游戏状态
cmd.CMD_S_StatusFree =
{
    {k = "bGameCheatUser", t = "bool"},
    {k = "bIsPersonalRoom", t = "bool"},                                --是否为积分约战类型
    {k = "bIsGoldPersonalRoom", t = "bool"},
    {k = "bIsDoubleSice", t = "bool"},                                  --是否为双骰子玩法
    -- 全局信息
    {k = "cbTimeLeave", t = "byte"},                                    --剩余时间
    
    -- 庄家信息
    {k = "wBankerUser", t = "word"},                                    --当前庄家
    {k = "cbBankerTime", t = "word"},                                   --庄家局数
    {k = "lBankerWinScore", t = "score"},                               --庄家成绩
    {k = "lBankerScore", t = "score"},                                  --庄家分数 
   
    -- 控制信息
    {k = "lApplyBankerCondition", t = "score"},                         --申请条件
    {k = "lUserMaxScore", t = "score"},                                 --玩家单局下注最大金币数
    {k = "lAreaLimitScore", t = "score"},                               --区域限制
    
    --筹码
    {k = "lJettonNum1", t = "score"},                                   --房间人数配置
    {k = "lJettonNum2", t = "score"},                                   --房间人数配置
    {k = "lJettonNum3", t = "score"},                                   --房间人数配置
    {k = "lJettonNum4", t = "score"},                                   --房间人数配置
    {k = "lJettonNum5", t = "score"},                                   --房间人数配置

    {k = "cbLastSice", t = "byte",l = {3}},                               --上一局开骰子情况
    {k = "wDeskChairID", t = "word",l = {11}},                                   --做桌玩家id(从左下角第一个位置1开始顺时针到右下角位置11)，位置号对应玩家id不为255则说明有人入坐
   
    -- 房间信息
    {k = "szGameRoomName", t = "string",s = 32},                        --房间名称
    {k = "CustomAndroid", t = "table", d = cmd.CustomAndroid},       --机器人配置

};

-- --游戏状态

cmd.CMD_S_StatusJetton = 
{
    {k = "bGameCheatUser", t = "bool"},
    {t = "bool", k = "bIsPersonalRoom"},                 --是否为积分约战类型
    {k = "bIsGoldPersonalRoom", t = "bool"},
    {t = "bool", k = "bIsDoubleSice"},                     --是否为双骰子玩法
    --全局下注
    {t = "score", k = "lAreaJettonScore", l = {6}},         --各个区域总注
    {t = "score", k = "lChuanJettonScore", l = {15}},       --各个串总注
    {t = "score", k = "lBaoziJettonScore", l = {6}},        --各个豹子总注

    {t = "score", k = "lUserBetScore",l = {6}},              --各个区域自己下注

    {t = "score", k = "lUserMaxScore"},                       --最大下注
    --控制信息
    {t = "score", k ="lApplyBankerCondition"},               --申请条件
    {t = "score", k = "lAreaLimitScore"},                     --区域限制

    --庄家信息
    {t = "word", k = "wBankerUser"},                         --当前庄家
    {t = "word", k = "cbBankerTime"},                        --庄家局数
    {t = "score", k = "lBankerScore"},                        --庄家分数

    --全局信息
    {t = "int", k = "cbTimeLeave"},                         --剩余时间
    {t = "byte", k = "cbGameStatus"},                        --游戏状态

    --筹码
    {t = "score", k = "lJettonNum1"},
    {t = "score", k = "lJettonNum2"},
    {t = "score", k = "lJettonNum3"},
    {t = "score", k = "lJettonNum4"},
    {t = "score", k = "lJettonNum5"},

    {t = "byte", k = "cbLastSice", l = {3}},                       --上一局开骰子情况
    {t = "word", k = "wDeskChairID", l = {11}},           --做桌玩家id(从左下角第一个位置1开始顺时针到右下角位置11)，位置号对应玩家id不为255则说明有人入坐
    --房间信息
    {t = "string", k = "szGameRoomName",s = 32},          --房间名称
    {t = "table", k = "CustomAndroid", d = cmd.CustomAndroid},       --机器人配置
};


--游戏状态
cmd.CMD_S_StatusEnd = 
{
    {k = "bGameCheatUser", t = "bool"},
    {t = "bool", k = "bIsPersonalRoom"},                    --是否为积分约战类型
    {k = "bIsGoldPersonalRoom", t = "bool"},
    {t = "bool", k = "bIsDoubleSice"},                      --是否为双骰子玩法
    --全局下注
    {t = "score", k = "lAreaJettonScore", l = {6}},         --各个区域总注
    {t = "score", k = "lChuanJettonScore", l = {15}},       --各个串总注
    {t = "score", k = "lBaoziJettonScore", l = {6}},        --各个豹子总注
    
    {t = "score",k = "lUserMaxScore"},                      --最大下注
    --控制信息
    {t = "score",k = "lApplyBankerCondition"},              --申请条件
    {t = "score",k = "lAreaLimitScore"},                    --区域限制
    --骰子信息
    {t = "byte",k = "cbSice", l = {3}},--骰子信息，

    --结束信息
    {t = "score",k = "lEndUserScore"},                      --玩家成绩
    
    {t = "score",k = "lEndUserReturnScore"},                --返回积分
    {t = "score",k = "lEndRevenue"},--游戏税收

    --庄家信息
    {t = "word", k = "wBankerUser"},                        --当前庄家
    {t = "word", k = "cbBankerTime"},                       --庄家局数
    {t = "score",k = "lBankerWinScore"},                    --庄家赢分
    {t = "score",k = "lBankerScore"},                       --庄家分数

    --结束信息
    {t = "score",k = "lEndBankerScore"},                    --庄家成绩

    --全局信息
    {t = "byte", k = "cbTimeLeave"},                        --剩余时间
    {t = "byte", k = "cbGameStatus"},                       --游戏状态

    --筹码
    {t = "score",k = "lJettonNum1"},
    {t = "score",k = "lJettonNum2"},
    {t = "score",k = "lJettonNum3"},
    {t = "score",k = "lJettonNum4"},
    {t = "score",k = "lJettonNum5"},

    {t = "byte", k = "cbLastSice", l = {3}},                --上一局开骰子情况
    {t = "word", k = "wDeskChairID", l = {11}},
    {t = "string", k = "szGameRoomName",s = 32},          --房间名称
    {t = "table", k = "CustomAndroid", d = cmd.CustomAndroid},    --房间信息
};


--游戏空闲
cmd.CMD_S_GameFree = 
{
    {t = "byte", k = "cbTimeLeave"},                         --剩余时间
    {t = "word", k = "wCurrentBanker"},                      --当前庄家
    {t = "int",  k = "nBankerTime"},                         --做庄次数
    {t = "int",  k = "nListUserCount"},

};
--游戏开始
cmd.CMD_S_GameStart = 
{
    {t = "word", k = "wBankerUser"},                         --当前庄家
    {t = "int", k = "nBankerTime"},                         --做庄次数
    {t = "score", k = "lBankerScore"},                        --庄家金币
    {t = "score", k = "lUserMaxJettonScore"},                 --我的最大下注金币限制
    {t = "byte", k = "cbTimeLeave"},                         --剩余时间
    {t = "int", k = "nChipRobotCount"},                     --人数上限 (下注机器人)
    {t = "int", k = "nAndriodApplyCount"},                  --机器人列表人数
};


--游戏结束
cmd.CMD_S_GameEnd = 
{
    {t = "byte",k = "cbTimeLeave"},    --剩余时间
    {t = "byte",k = "cbSice",l={3}},   --骰子信息
    --玩家成绩
    {t = "score", k = "lUserScore",l={11}},   --玩家成绩
    {t = "score", k = "lUserReturnScore"}, --返回积分
    {t = "score", k = "lUserRevenue"}, --玩家税收
   
    --庄家信息
    {t = "word", k = "wCurrentBanker"},                    --当前庄家
    {t = "score", k = "lBankerScore"},                      --庄家成绩
    {t = "score", k = "lBankerTotallScore"},                --庄家总分数
    {t = "int", k = "nBankerTime"},                       --做庄次数

    {t = "byte",k = "cbLastSice", l = {3}},                      --上一局开骰子情况
};

--用户下注
cmd.CMD_S_PlaceJetton = 
{
    {t = "word", k = "wChairID"},                           --用户位置id
    {t = "byte", k = "cbJettonArea"},                       --筹码区域
    {t = "byte", k = "cbOldJettonArea"},                    --旧区域
    {t = "byte", k = "cbJettonType"},                       --押注类型(1-区域押注，2-串押注，3-挪单，4-挪串，5-豹子)
    {t = "score", k = "lJettonScore"},                       --加注数目
    {t = "bool", k = "bIsAndroid"},                         --是否机器人

};


--失败结构
cmd.CMD_S_PlaceJettonFail = 
{
    {t = "word", k =  "wPlaceUser"},                       --下注玩家
    {t = "byte", k =  "cbReason"},                         --下注失败原因
    {t = "byte", k =  "cbJettonArea"},                     --筹码区域
    {t = "byte", k =  "cbJettonType"},                     --押注类型
    {t = "score", k =  "lJettonScore"},                    --加注数目
};

--取消申请
cmd.CMD_S_CancelBanker =
{
    {t = "bool", k = "bAuto"},                              --是否自动取消申请
    {t = "word", k = "wCancelUser"},                        --取消玩家
    {t = "bool", k = "bIsClickCancelBanker"},               --是否游戏中点击取消当前庄家
};

--切换庄家
cmd.CMD_S_ChangeBanker =
{
    {t = "word", k = "wBankerUser"},                     --当庄玩家
    {t = "score", k = "lBankerScore"},                   --庄家金币
    {t = "word", k = "wUserChair", l = {11}},            --玩家位置
};

--申请庄家
cmd.CMD_S_ApplyBanker = 
{
    {t = "word", k = "wApplyUser"}                                             --申请玩家
};

--玩家位置
cmd.CMD_S_UserChair = 
{
    {t = "word", k = "wUserChair", l = {11}}                             --玩家位置
};

--记录信息
cmd.tagServerGameRecord =
{
    {t = "byte", k = "SICE" , l = {3}},                --双骰子玩法时，第三个固定为255，012345分别代表鱼、虾、蟹、狮子（鸡）、老虎、葫芦
};

--控制
cmd.tagControlInfo =
{
    {t = "byte",k = "cbControlType"},                                 --2控制骰子，1控制庄家
    {t = "byte",k = "cbSice" , l = {3}},                              --控制骰子
    {t = "byte",k = "cbBankerControl"},                               --控制庄家输、赢
};      

cmd.tagAdminReq = 
{
    {t = "byte",k = "m_cbExcuteTimes"},                                 --执行次数
    {t = "byte",k = "m_cbControlStyle"},                                --控制方式
    {t = "byte",k = "m_cbWinArea"},                                     --赢家区域
    {t = "byte",k = "m_cbPokerType"},                                   --控制赢的牌型
};


cmd.CMD_C_AdminReq = 
{
    {t = "byte",k = "cbReqType"},
    {t = "byte",k = "cbExtendData", l = {20}},          --附加数据
};

--用户下注
cmd.CMD_C_PlaceJetton =
{
    {t = "word",k = "wUserChairID"},                                   --用户位置id
    {t = "byte",k = "cbJettonType"},                                   --押注类型(1-区域押注，2-串押注，3-挪单，4-挪串，5-豹子)
    {t = "byte",k = "cbJettonArea"},                                   --押注区域,如果是串玩法则对应0~14个串（鱼虾，鱼蟹，鱼狮子（鸡），鱼老虎，鱼葫芦，虾蟹，虾狮子（鸡），虾老虎，虾葫芦，
    --蟹狮子（鸡），蟹老虎，蟹葫芦，狮子（鸡）老虎，狮子（鸡）葫芦，老虎葫芦）
      
    {t = "score", k = "lJettonScore"},                                   --加注数目
    {t = "byte",k = "cbOldJettonArea"},                                --挪单，挪串之前的玩家原下注区域或者串
};

--玩家各种玩法下注限制
cmd.CMD_S_Current_Limit = 
{
    {t = "score", k = "lAllAreaLimit", l = {6}},        --区域下注当前剩余下注额度限制
    {t = "score", k = "lAllChuanLimit", l = {15}},      --串下注当前剩余下注额度限制
    {t = "score", k = "lAllBaoZiLimit", l = {6}},       --豹子下注当前剩余下注额度限制
    {t = "score", k = "lAreaJettonScore", l = {6}},     --各个区域总注
    {t = "score", k = "lChuanJettonScore", l = {15}},   --各个串总注
    {t = "score", k = "l_UserLimit"},
}

cmd.S_NUO_INFO = 
{
    {t = "byte",k = "old_place"}, 
    {t = "byte",k = "new_place"},
    {t = "score",k = "score"}, 
}

cmd.CMD_S_Current_UserBetInfo = 
{
    {t = "word",k = "wChairId"},                        --玩家
    {t = "score", k = "lAllAreaInfo", l = {6,6,6,6,6,6,6,6,6,6,6} },          --区域下注信息
    {t = "score", k = "lAllChuanInfo", l = {15,15,15,15,15,15,15,15,15,15,15} }, --串下注信息
    {t = "score", k = "lAllBaoZiInfo", l = {6,6,6,6,6,6,6,6,6,6,6} },        --豹子下注信息
    {t = "table", k = "nuo_dan_info", d = cmd.S_NUO_INFO,l = {3,3,3,3,3,3,3,3,3,3,3}},  
    {t = "table", k = "nuo_chuan_info", d = cmd.S_NUO_INFO,l = {3,3,3,3,3,3,3,3,3,3,3}},  
--     {t = "score", k = "lAllNuoDanInfo", l = {6,6,6,6,6,6,6,6,6,6,6}},       --挪单信息
--     {t = "score", k = "lAllNuoChuanInfo", l = {15,15,15,15,15,15,15,15,15,15,15}},    --挪串信息
}


cmd.CMD_S_GAMERecord = 
{
    {t = "word",k = "wChairId"},
    {t = "byte",k = "cbBankerID"},
    {t = "byte", k = "cbSice",l = {3}},
    {t = "score",k = "lUserWinScore",l = {11}},
    {t = "score",k = "lAllAreaInfo",l = {6,6,6,6,6,6,6,6,6,6,6} },
    {t = "score",k = "lAllChuanInfo",l = {15,15,15,15,15,15,15,15,15,15,15} },
    {t = "score",k = "lAllBaoZiInfo",l = {6,6,6,6,6,6,6,6,6,6,6}},
    {t = "table", k = "nuo_dan_info", d = cmd.S_NUO_INFO,l = {3,3,3,3,3,3,3,3,3,3,3}},  
    {t = "table", k = "nuo_chuan_info", d = cmd.S_NUO_INFO,l = {3,3,3,3,3,3,3,3,3,3,3}},
    {t = "int", k = "n_UserID" , l = {11}},
}

cmd.CMD_S_Score =
{
    {t = "byte", k = "cbBankerID"},
    {t = "score",k = "m_userWinScore",l={64,64,64,64,64,64,64,64,64,64,64}},
}


cmd.CMD_C_ControlApplication = 
{
    {t = "int",k = "iTableId"},
    {t = "byte",k = "cbControlAppType"},
    {t = "byte",k = "cbCtrlType"},
    {t = "byte",k = "bIsBankerWin"},
    {t = "byte",k = "cbSice",l={3}},
    {t = "byte",k = "cbControlTimes"},
}

cmd.CMD_S_ControlReturns = 
{
    {t = "byte",k = "cbReturnsType"},     --回复类型
    {t = "byte",k = "cbCtrlType"},       --控制类型（1控制庄家输赢，2采用区域控制）
    {t = "byte",k = "bIsBankerWin"},     --庄家输赢
    {t = "byte",k = "cbSice",l = {3}},   --区域控制(骰子控制)
    {t = "byte",k = "cbControlTimes"},   --控制次数(默认1不可设置)
}

cmd.ROOMUSERINFO = 
{
    {t = "word",k = "wChairID"},
    {t = "word",k = "wTableID"},
    {t = "int",k = "dwGameID"},
    {t = "bool",k = "bAndroid"},
    {t = "byte", k = "cbUserStatus"},
    {t = "byte", k = "cbGameStatus"}, 
    {t = "string",k = "szNickName",s=yl.LEN_ACCOUNTS},
}

cmd.CMD_S_RequestQueryResult = 
{
    {t = "table", k = "userinfo", d = cmd.ROOMUSERINFO},  
    {t = "bool", k = "bFind"},
}

cmd.CMD_S_UpdateStorage = 
{
    {t = "byte",  k = "cbReqType"},             --请求类型
    {t = "score", k = "lStorageStart"},         --起始库存
    {t = "score", k = "lStorageDeduct"},        --库存衰减
    {t = "score", k = "lStorageCurrent"},       --当前库存
    {t = "score", k = "lStorageMax1"},          --库存上限1
    {t = "score", k = "lStorageMul1"},          --系统输分概率1
    {t = "score", k = "lStorageMax2"},          --库存上限2
    {t = "score", k = "lStorageMul2"},          --系统输分概率2
}

--****************定时器标识******************--
--开始定时器
cmd.IDI_START_GAME   			= 200
--加注定时器
cmd.IDI_USER_ADD_SCORE			= 201
--选比牌用户定时器
cmd.IDI_USER_COMPARE_CARD		= 202
--过滤定时器
cmd.IDI_DISABLE					= 203
--*****************时间标识*****************--
--开始定时器
cmd.TIME_START_GAME				= 20
--加注定时器
cmd.TIME_USER_ADD_SCORE			= 25
--比牌定时器
cmd.TIME_USER_COMPARE_CARD		= 25


-- 语音动画
cmd.VOICE_ANIMATION_KEY = "voice_ani_key"

return cmd