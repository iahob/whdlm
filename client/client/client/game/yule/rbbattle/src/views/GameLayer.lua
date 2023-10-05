local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local module_pre = "game.yule.rbbattle.src";
require("cocos.init")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
-- local cmd = module_pre .. ".models.CMD_Game"
-- local cmd = appdf.req(appdf.GAME_SRC.."yule.rbbattle.src.models.CMD_Game")
local cmd = appdf.req(module_pre..".models.CMD_Game")
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local GameLogic = module_pre .. ".models.GameLogic";
local GameViewLayer = appdf.req(module_pre .. ".views.layer.GameViewLayer")
local bjlDefine = appdf.req(module_pre .. ".models.bjlGameDefine")
local QueryDialog   = require("app.views.layer.other.QueryDialog")
local g_var = ExternalFun.req_var
local GameFrame = appdf.req(module_pre .. ".models.GameFrame")


--下注失败提示
local JettonFailResult = 
{
    [0] = "当前不是下注阶段",
    [1] = "当前下注超过用户积分限制",
    [2] = "下注总金额超过区域限制",
    [3] = "黑方和红方不能同时下注",
    [4] = "其它错误",
    [5] = "总下注分数不能超过庄家总分"
}

--上桌失败提示
local UpDeskFailResult = 
{
    [1]        = "没有空余位置",
    [2]        = "财富不满足坐桌条件",
    [3]        = "您已经在座位上",
    [4]        = "其它错误",
    [5]        = "庄家不能申请坐桌"
}

--重复投失败提示
local ReJettonFailResult = 
{
    [1]        = "上一轮没有下注",
    [2]        = "您的金币不足"
}

function GameLayer:ctor(frameEngine,scene)
    ExternalFun.registerNodeEvent(self)
    self.m_bLeaveGame = false
    -- self.m_bOnGame = false
    self._dataModle = GameFrame:create() 
    GameLayer.super.ctor(self,frameEngine,scene)

    self.m_cbGameStatus=0
end

--创建场景
function GameLayer:CreateView()
    return GameViewLayer:create(self)
        :addTo(self)
end

function GameLayer:getParentNode( )
    return self._scene
end

function GameLayer:getFrame( )
    return self._gameFrame
end

function GameLayer:getUserList(  )
    return self._gameFrame._UserList
end

function GameLayer:sendNetData(cmddata)
    return self:getFrame():sendSocketData(cmddata)
end

function GameLayer:getDataMgr( )
    return self._dataModle
end

function GameLayer:logData(msg)
    if nil ~= self._scene.logData then
        self._scene:logData(msg)
    end
end

--申请取款
function GameLayer:sendTakeScore( lScore,szPassword )
    local cmddata = ExternalFun.create_netdata(g_var(game_cmd).CMD_GR_C_TakeScoreRequest)
    cmddata:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE, g_var(game_cmd).SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushbyte(g_var(game_cmd).SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushscore(lScore)
    cmddata:pushstring(md5(szPassword),yl.LEN_PASSWORD)
    cmddata:pushstring(GlobalUserItem.szDynamicPass,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end

--请求银行信息
function GameLayer:sendRequestBankInfo()
    local cmddata = CCmd_Data:create(67)
    print("g_var(game_cmd).MDM_GR_INSURE",g_var(game_cmd).MDM_GR_INSURE)
    print("g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO",g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    cmddata:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE,g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushbyte(g_var(game_cmd).SUB_GR_QUERY_INSURE_INFO)
    local str = md5(GlobalUserItem.szPassword) or "d1fd5495e7b727081497cfce780b6456"
    cmddata:pushstring(str,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end

--申请存款
function GameLayer:onSaveScore(lScore)
    --操作记录
    local buffer = ExternalFun.create_netdata(g_var(game_cmd).CMD_GR_C_SaveScoreRequest)
    buffer:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE,g_var(game_cmd).SUB_GR_SAVE_SCORE_REQUEST)
    buffer:pushbyte(g_var(game_cmd).SUB_GR_SAVE_SCORE_REQUEST)
    buffer:pushscore(lScore)
    buffer:pushstring(GlobalUserItem.szDynamicPass,yl.LEN_PASSWORD)
    self:sendNetData(buffer)
    --if not self.sendNetData(buffer) then
        --showToast(self,"发送存款失败！",2)
    --end
end

--银行消息
function GameLayer:onSocketInsureEvent( sub,dataBuffer )
    self:dismissPopWait()
    if sub == g_var(game_cmd).SUB_GR_USER_INSURE_SUCCESS then
        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureSuccess, dataBuffer)
        self.bank_success = cmd_table

        self._gameView:onBankSuccess()
        self:sendRequestBankInfo()
    elseif sub == g_var(game_cmd).SUB_GR_USER_INSURE_FAILURE then
        local cmd_table = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureFailure, dataBuffer)
        self.bank_fail = cmd_table

        self._gameView:onBankFailure()
    elseif sub == g_var(game_cmd).SUB_GR_USER_INSURE_INFO then --银行资料
        local cmdtable = ExternalFun.read_netdata(g_var(game_cmd).CMD_GR_S_UserInsureInfo, dataBuffer)
        self._gameView:onGetBankInfo(cmdtable)
    else
        print("unknow gamemessage sub is ==>"..sub)
    end
end


---------------------------------------------------------------------------------------
------继承函数

--获取gamekind
function GameLayer:getGameKind()
    return cmd.KIND_ID;
end

-- function GameLayer:onExit()
--     self:KillGameClock()
--     self:dismissPopWait()
--     GameLayer.super.onExit(self)
-- end

-- 重置游戏数据
function GameLayer:OnResetGameEngine()
    self._gameView.m_enApplyState = self._gameView._apply_state.kCancelState;
    self._dataModle:removeAllUser();
    self._gameView:refreshApplyList();
    self._gameView:refreshApplyBtnState();
end

--强行起立、退出(用户切换到后台断网处理)
function GameLayer:standUpAndQuit()
    -- self:sendCancelOccupy()
    GameLayer.super.standUpAndQuit(self)
end

--退出桌子
function GameLayer:onExitTable()
    self:KillGameClock()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        self:showPopWait()
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(
                function () 
                    -- self:sendCancelOccupy()
                    self._gameFrame:StandUp(1)
                end
                ),
            cc.DelayTime:create(10),
            cc.CallFunc:create(
                function ()
                    --强制离开游戏(针对长时间收不到服务器消息的情况)
                    print("delay leave")
                    self:onExitRoom()
                end
                )
            )
        )
        return
    end

   self:onExitRoom()
end

--离开房间
function GameLayer:onExitRoom()
    self:getFrame():onCloseSocket()

    self._scene:onKeyBack()    
end

-- 计时器响应
function GameLayer:OnEventGameClockInfo(chair,time,clockId)
    -- if nil ~= self._gameView and nil ~= self._gameView.updateClock then
    --     self._gameView:updateClock(clockId, time)
    -- end
end

-- 设置计时器
function GameLayer:SetGameClock(id,time)
    self:KillGameClock();
    local chairId = self:GetMeChairID();
    GameLayer.super.SetGameClock(self, chairId, id, time);
end

------网络发送
--玩家下注
function GameLayer:sendPlaceJetton(areaIndex, jettonMoney)
    local cmddata = ExternalFun.create_netdata(cmd.CMD_C_PlaceJetton);
    cmddata:pushword(self:GetMeChairID());
    cmddata:pushbyte(areaIndex);
    cmddata:pushscore(jettonMoney);

    self:SendData(cmd.SUB_C_PLACE_JETTON, cmddata);
end

--用户查询开奖信息
function GameLayer:sendQuraryLottoryInfo(  )
    -- print("-----------------用户查询开奖信息-----------------")
    local cmddata = ExternalFun.create_netdata(cmd.CMD_C_QuaryLottoryInfo);
    cmddata:pushword(self:GetMeChairID());

    self:SendData(cmd.SUB_C_QURARY_LOTTORY_INFO, cmddata);
end

--发送重复投消息
function GameLayer:SendReJettonMessage()
    -- print("-----------------发送重复投消息-----------------")
    local cmddata = ExternalFun.create_netdata(cmd.CMD_C_UserReJetton);
    cmddata:pushword(self:GetMeChairID());

    self:SendData(cmd.SUB_C_CLICK_REJETTON, cmddata);
end

--用户请求记录信息
function GameLayer:sendQuraryRecord(  )
    local cmddata = CCmd_Data:create(0);
    self:SendData(cmd.SUB_C_QURARY_RECORD, cmddata);
end

--查询房间中用户的信息
function GameLayer:sendQuraryRoomUsersInfo(  )
    local cmddata = CCmd_Data:create(0);
    self:SendData(cmd.SUB_C_QURARY_ROOM_USERS_INFO, cmddata);
end

--申请坐桌
function GameLayer:sendApplyDesk(chairId, isApply, deskIndex)
    local cmddata = ExternalFun.create_netdata(cmd.CMD_C_ApplyDesk);
    cmddata:pushword(chairId);
    cmddata:pushbool(isApply);
    cmddata:pushword(deskIndex);

    self:SendData(cmd.SUB_C_APPLY_DESK, cmddata);
end

--申请庄家
function GameLayer:sendApplyBanker()
    local cmddata = CCmd_Data:create(0);
    self:SendData(cmd.SUB_C_APPLY_BANKER, cmddata);
end

--取消申请、申请下庄
function GameLayer:sendCancelApply()
    local cmddata = CCmd_Data:create(0);
    self:SendData(cmd.SUB_C_CANCEL_BANKER, cmddata);
end
------网络接收

-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)
    print("场景数据:" .. cbGameStatus);
    -- if self.m_bOnGame then
    --     return
    -- end
    -- self.m_bOnGame = true

    --先清理玩家信息
    self._dataModle:removeAllUser();
    --初始化已有玩家
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1);
        if nil ~= userItem then
            --增加用户
            self._dataModle:addUser(userItem);
        end
    end

    self:KillGameClock();
    --0.重置本局数据
    self._gameView:ReSetGameData();

    --初始化个人信息
    self._gameView:initSelfInfo();

    local cmd_data = nil;
    local currentGameStatus = cmd.GAME_SCENE_FREE;
    self.m_cbGameStatus=0;
    --空闲场景
    if cbGameStatus == cmd.GAME_SCENE_FREE   then                        --空闲状态
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer);
        dump(cmd_data,"空闲状态",6);

        --配置筹码
        self._gameView:betServerSet(cmd_data.lBet[1]);

    elseif cbGameStatus == cmd.GS_PLACE_JETTON  or cbGameStatus == cmd.GS_GAME_END then  --下注状态、结束状态
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer);
        dump(cmd_data,"下注状态、结束状态",6);

        --配置筹码
        self._gameView:betServerSet(cmd_data.lBet[1]);
        
        if cmd_data.bIsJettonStatus then
            --是否有下注
            for i=1,3 do
                -- 1 红区 、2 黑区、 3 牌型区
                local userAreaTotalJettonScore = cmd_data.lUserJettonScore[1][i];
                if userAreaTotalJettonScore > 0 then
                    self.m_cbGameStatus=cmd.GS_PLACE_JETTON;
                end
            end
            --若处于下注
            currentGameStatus = cmd.GS_PLACE_JETTON;
            --1.显示左右三张牌牌背
            -- self._gameView:SetCardActive(true);
            self._gameView:openRewardCardCreate();

            --2.根据全体总注，初始化桌面已经下注的筹码
            self._gameView:InitChouma(cmd_data.lAllJettonScore[1]);
            --3.根据自己的下注，初始化自己下注UI
            self._gameView:InitUserJettonLabel(cmd_data.lUserJettonScore[1]);
            --4.根据剩余时间，初始化定时器
            self:SetGameClock(currentGameStatus, cmd_data.cbTimeLeave)
            --5.启用游戏按钮
            self._gameView:InitUserMaxJetton(cmd_data.lUserMaxScore, cmd_data.lAreaLimitScore[1]); --因为设置游戏按钮的可用需要用到个人下注限制，所以提前调用一次
            self._gameView:SwitchButtonEnabled(true);
        else
            --若不处于下注状态，则显示等待开局倒计时UI
            currentGameStatus = cmd.GS_GAME_END;
            self._gameView:ShowWaitStartGame(cmd_data.cbTimeLeave);
            --禁用游戏按钮
            self._gameView:SwitchButtonEnabled(false);
        end
    else
    end

    --从申请列表移除
    self._dataModle:removeApplyUser(cmd_data.wBankerUser);
    --抢庄条件
    self._gameView:onGetApplyBankerCondition(cmd_data.lApplyBankerCondition);
    --是否显示庄家
    self._gameView:onIsShowApplyBanker(cmd_data.bIsAllowApplyBanker);
    --当前庄家
    self._gameView.m_wBankerUser = cmd_data.wBankerUser;
    --庄家赢分
    self._gameView.m_lBankerWinScore = cmd_data.lBankerWinScore;
    --当前庄家分数
    self._gameView.m_lBankerScore = cmd_data.lBankerScore;
    --庄家局数
    self._gameView.cbBankerTime = cmd_data.cbBankerTime;
    --刷新庄家信息
    self._gameView:resetBankerInfo();

    --消息公共处理
    --1.初始化彩金池金额
    self._gameView:UpdateStorage(cmd_data.lCurrentPoolScore);
    --2.初始化桌子上6个座位的玩家信息
    self._gameView:InitTableUserChairID(cmd_data.cbDeskChairID[1]);
    self._gameView:InitTableUser();
    --3.初始化 玩家单局下注限制、区域下注限制
    self._gameView:InitUserMaxJetton(cmd_data.lUserMaxScore, cmd_data.lAreaLimitScore[1]);
    --4.设置游戏状态
    self._gameView:SetCurrentGameStatus(currentGameStatus);
    --5.请求游戏路单数据
    self:sendQuraryRecord();
    --6.播放背景音乐
    ExternalFun.playBackgroudAudio(cmd.SOUND_GameSceneBGM);
end

----------------------------------------------------
-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)  
    if self.m_bLeaveGame or nil == self._gameView then
        return
    end 
    -- print("游戏消息---sub", sub);
	if sub == cmd.SUB_S_GAME_FREE then   --游戏空闲
        -- print("游戏空闲");
        self.m_cbGameStatus = cmd.GAME_SCENE_FREE;
		self:onSubGameFree(dataBuffer);
	elseif sub == cmd.SUB_S_GAME_START then --游戏开始
        -- print("游戏开始");

		self:onSubGameStart(dataBuffer);
	elseif sub == cmd.SUB_S_PLACE_JETTON then --用户下注
        -- print("用户下注");
        
		self:OnSubPlateJetton(dataBuffer)
	elseif sub == cmd.SUB_S_GAME_END then --游戏结束
        -- print("游戏结束");
        self.m_cbGameStatus = cmd.GAME_SCENE_FREE;
		self:onSubGameEnd(dataBuffer);
	elseif sub == cmd.SUB_S_UP_DESK then  --用户申请上桌
        -- print("用户申请上桌");
		self:OnPlayerUpDeskCallBack(dataBuffer);
	elseif sub == cmd.SUB_S_HISTORY_RECORD then --游戏路单记录
        -- print("游戏路单记录");
		self:onSubHistoryRecord(dataBuffer);
	elseif sub == cmd.SUB_S_QURARY_LOTTORY_INFO then --查询开奖信息
        -- print("查询开奖信息");
		self:onQuraryLottoryInfo(dataBuffer);
    elseif sub == cmd.SUB_S_SEND_RECORD then   --游戏记录
        -- print("游戏记录");
        self:onSubSendRecord(dataBuffer);
    elseif sub == cmd.SUB_S_PLACE_JETTON_FAIL then  --下注失败
        -- print("下注失败");
        self:onSubJettonFail(dataBuffer);
    elseif sub == cmd.SUB_S_DOWN_DESK then  --用户下桌
        -- print("用户下桌");
        self:onSubDownDesk(dataBuffer);
    elseif sub == cmd.SUB_S_SEND_REJETTON then  --发送玩家是否满足重复投以及上轮投注情况
        -- print("发送玩家是否满足重复投以及上轮投注情况");
        self:onSubSendRejetton(dataBuffer);
    elseif sub == cmd.SUB_S_QURARY_ROOM_PLAYERS_INFO then  --查询房间中玩家的信息
        -- print("查询房间中玩家的信息");
        self:onSubQuraryRoomPlayersInfo(dataBuffer);
    elseif sub == cmd.SUB_S_CANCEL_BANKER then  --取消申请
        self:onCancelBanker(dataBuffer);
    elseif sub == cmd.SUB_S_APPLY_BANKER then  --申请庄家
        self:onApplyBanker(dataBuffer);
    elseif sub == cmd.SUB_S_CHANGE_BANKER then  --切换庄家
        self:onChangeBanker(dataBuffer);
	else
		-- print("unknow gamemessage sub is ==>"..sub)
	end
end

--游戏空闲
function GameLayer:onSubGameFree(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameFree, dataBuffer);
    -- dump(cmd_table,"游戏空闲",6);
    --0.重置本局数据
    self._gameView:ReSetGameData();
    self._gameView:SetCurrentGameStatus(cmd.GAME_SCENE_FREE);

    --申请按钮状态更新
    self._gameView:refreshApplyBtnState();
    --初始化个人信息
    self._gameView:initSelfInfo();
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameStart,dataBuffer);
    -- dump(cmd_table,"游戏开始",6)
    
    --播放开始音效
    local currentGameStatus = cmd.GS_PLACE_JETTON;

    --0.重置本局数据
    self._gameView:ReSetGameData();
    --1.设置定时器
    self:SetGameClock(currentGameStatus, cmd_table.cbTimeLeave)
    --2.设置当前游戏状态
    self._gameView:SetCurrentGameStatus(currentGameStatus)
    --3.播放开局动画/开始下注动画/发牌动画
    self._gameView:PlayGameStartAnim();
    -- self._gameView:openRewardCardCreate(true);
    --4.启用游戏按钮
    self._gameView:SwitchButtonEnabled(true)
    --5.处理重复投
    self:runAction(cc.Sequence:create(
        cc.DelayTime:create(4.0),
        cc.CallFunc:create( function()
            if self._gameView.m_IsReJetton and not self._gameView.m_IsReJettonBet then
                self:SendReJettonMessage();
            end
        end )
    ));


    --当前庄家
    self._gameView.m_wBankerUser = cmd_table.wBankerUser;
    --当前庄家分数
    self._gameView.m_lBankerScore = cmd_table.lBankerScore;
    --刷新庄家分数
    self._gameView:resetBankerScore();
    --申请按钮状态更新
    self._gameView:refreshApplyBtnState();

end

--用户下注
function GameLayer:OnSubPlateJetton(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_PlaceJetton, dataBuffer);
    -- dump(cmd_table,"用户下注",6);

    local isSelf = self:GetMeChairID() == cmd_table.wChairID;

    --自己下注改变状态
    if isSelf then
        self.m_cbGameStatus = cmd.GS_PLACE_JETTON;
    end

    local userTableId = self._gameView:GetTableUser(cmd_table.wChairID);
    self._gameView:PlayerJetton(cmd_table, isSelf, userTableId);
end

--游戏结束
function GameLayer:onSubGameEnd(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameEnd,dataBuffer);
    dump(cmd_table,"游戏结束",6);
    
    self._gameView.isEndGame = true;

    local currentGameStatus = cmd.GS_GAME_END;

    local isOpenReward = cmd_table.bIsOpenReward;

    local baociTime = 0;
    if isOpenReward then
        baociTime = 3.0;
    else
        baociTime = 0.0;
    end

    --庄家赢分
    self._gameView.m_lBankerWinScore = cmd_table.lBankerScore;
    self._gameView.cbBankerTime = cmd_table.nBankerTime;

    --1.设置游戏状态
    self._gameView:SetCurrentGameStatus(currentGameStatus);
    --2.禁用游戏按钮
    self._gameView:SwitchButtonEnabled(false);
    
    self:runAction(cc.Sequence:create(
        cc.CallFunc:create( function()
            --3.根据玩家的得分，生成筹码
            self._gameView:CreateChoumaObjForScore(cmd_table)
            --4.播放停止下注动画
            self._gameView:PlayStopJettonAnim();
        end ),
        cc.DelayTime:create(2.0),
        cc.CallFunc:create( function()
            --5.播放开牌动画
            -- self._gameView:openRewardCardCreate();
            self._gameView:cardOpenRewardAni(cmd_table.cbTableCardArray, cmd_table.cbPokerType[1]);
        end ),
        cc.DelayTime:create(5.5),
        cc.CallFunc:create( function()
            --6.显示赢的区域
            self._gameView:ShowWinArea(cmd_table.bWinRed, cmd_table.bWinBlack, cmd_table.bWinPoker)
            --7.更新路单数据
            self:sendQuraryRecord();
        end ),
        cc.DelayTime:create(2.0),
        cc.CallFunc:create( function()
            --8.显示筹码转移动画、筹码飞到玩家头像处动画
            self._gameView:ChoumaMoveingHandler(cmd_table);
        end ),
        cc.DelayTime:create(2.5),
        cc.CallFunc:create( function()
             --9.是否大奖 爆池
            if isOpenReward then
                ExternalFun.playSoundEffect(cmd.EFFECT_BaoChi);
                self._gameView:PlayBaoChiAnim(true);
            end
        end ),
        cc.DelayTime:create(baociTime),
        cc.CallFunc:create( function()
            --10.显示玩家得分
            self._gameView:ShowPlayerWinScore()
            --11.显示玩家得分动画
            local selfWinScore = cmd_table.lUserScore + cmd_table.lUserReturnScore
            if selfWinScore > 0 then
                self._gameView:PlayPlayerWinAnim(selfWinScore);
            end
            --12.更新彩金池金额
            self._gameView:UpdateStorage(cmd_table.lCurrentPoolScore);
        end )
    ));

end

--用户上桌结果
function GameLayer:OnPlayerUpDeskCallBack(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_UpDesk,dataBuffer);
    -- dump(cmd_table,"--------用户申请上桌-----",6);

    local isSelf = self:GetMeChairID() == cmd_table.wChairID;
    if cmd_table.bUpDeskOk then
        --上桌成功，更新数据/UI
        self._gameView:OnPlayerUpDeskCallBack(cmd_table.wChairID, cmd_table.wDeskChairID);

    elseif isSelf then
        --上桌失败，若是自己，则提示失败原因
        showToast(self, "上桌失败: " .. UpDeskFailResult[cmd_table.cbFailReason], 2);
    end
end

--游戏路单记录
function GameLayer:onSubHistoryRecord(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameHistoryRecord,dataBuffer);
    -- dump(cmd_table,"--------游戏路单记录-----",6)

    self._gameView:onTrendInfoFresh(cmd_table);

    if self._gameView.isEndGame then
        self._gameView:showResultMove(cmd_table);
    else
        self._gameView:showRecordResult(cmd_table);
        
    end
    
end

--查询开奖信息
function GameLayer:onQuraryLottoryInfo(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_WinLottoryInfo,dataBuffer);
    -- dump(cmd_table,"--------查询开奖信息-----",6)

    self._gameView:onRewardPoolFresh(cmd_table);

    -- self._gameView:refreshUserList();
end

--申请庄家
function GameLayer:onApplyBanker(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_ApplyBanker,dataBuffer);
    -- dump(cmd_table,"--------申请庄家-----",6)

    self._dataModle:addApplyUser(cmd_table.wApplyUser, false);
    self._gameView:onApplyBanker(cmd_table);
end

--取消申请
function GameLayer:onCancelBanker(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_CancelBanker,dataBuffer);
    -- dump(cmd_table,"--------取消申请-----",6)

    if cmd_table.bIsAllowApplyBanker then
        --是否已申请下庄
        self._gameView.bIsClickCancelBanker = true;
        showToast(self, "申请成功，等待游戏结束后主动下庄!", 2);
        self._gameView:refreshApplyBtnState();
        return;
    end

    --从申请列表移除
    self._dataModle:removeApplyUser(cmd_table.wCancelUser);

    self._gameView:onGetCancelBanker(cmd_table);

    --取消申请提示
    local bAuto = cmd_table.bAuto;
    local myChairId = self:GetMeChairID();
    local m_PlayerMoney = self._gameView.m_PlayerMoney;
    local m_llCondition = self._gameView.m_llCondition;
    
    if bAuto and cmd_table.wCancelUser == myChairId then  --自动取消且是自己
        self._gameView.bIsClickCancelBanker = false;

        if m_PlayerMoney < m_llCondition then
            showToast(self, "由于您的金币数（" .. m_PlayerMoney .. "）少于坐庄必须金币数（" .. m_llCondition .. "）,轮到他人上庄！", 2);
        else
            showToast(self, "做庄次数达到(" ..self._gameView.cbBankerTime ..")次,自动换庄!", 2);
        end
    elseif not bAuto and cmd_table.wCancelUser == myChairId and cmd_table.wCancelUser == self._gameView.m_wBankerUser then  --主动取消且是自己
        self._gameView.bIsClickCancelBanker = false;

        showToast(self, "下庄成功!", 2);
    end
end

--切换庄家
function GameLayer:onChangeBanker(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_ChangeBanker,dataBuffer);
    -- dump(cmd_table,"--------切换庄家-----",6)

    --从申请列表移除
    self._dataModle:removeApplyUser(cmd_table.wBankerUser);

    self._gameView:onChangeBanker(cmd_table);

    --申请列表更新
    self._gameView:refreshApplyList();
end

--游戏记录
function GameLayer:onSubSendRecord(dataBuffer)
    
end

--下注失败
function GameLayer:onSubJettonFail(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_PlaceJettonFail, dataBuffer);

    showToast(self, "下注失败: " .. JettonFailResult[cmd_table.cbReason], 2);
end

--用户下桌
function GameLayer:onSubDownDesk(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameDown, dataBuffer);
    -- dump(cmd_table,"----------onSubDownDesk-------",6)
    -- 下桌玩家、桌子号
    self._gameView:OnPlayerExitDesk(cmd_table.wChairID, cmd_table.wDeskChairID);
end

--发送玩家是否满足重复投以及上轮投注情况
function GameLayer:onSubSendRejetton(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_SendBetInfo, dataBuffer);

    if cmd_table.iResult == 0 then
        --重复投成功
        self._gameView:UserReJettonHandler(cmd_table.lUserJettonScore[1]);
        self._gameView.m_IsReJettonBet = true;
    else
        --重复投失败
        showToast(self, "重复投失败: " .. ReJettonFailResult[cmd_table.iResult], 2);
        --取消重复投
        self._gameView.m_IsReJetton = false;
        self._gameView.icon_select:setVisible(false);

    end
end

--查询房间中玩家的信息
function GameLayer:onSubQuraryRoomPlayersInfo(dataBuffer)
    local roomUsersInfo = ExternalFun.read_netdata(cmd.CMD_S_SendRoomUserInfo, dataBuffer);
    -- dump(roomUsersInfo,"--------onSubQuraryRoomPlayersInfo-----",6)
    -- 刷新玩家列表
    self._gameView:onPlayerListFresh(roomUsersInfo);

    self._gameView:refreshUserList();
end

function GameLayer:onEventUserEnter(wTableID,wChairID,useritem)
    print("add user " .. useritem.wChairID .. "; nick " .. useritem.szNickName);
    --缓存用户
    self._dataModle:addUser(useritem);
   
    -- assert(false)
   -- print("------------------缓存用户--------------------")
   -- dump(useritem, "--------------------", useritem);


    --刷新用户列表
    -- self._gameView:refreshUserList();
    --刷新桌上用户
    -- self._gameView:InitTableUser();
end

function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)
    print("change user " .. useritem.wChairID .. "; nick " .. useritem.szNickName);
    -- print("------------------缓存用户--------------------")
    -- dump(useritem, "--------useritem------------", 6);
    -- dump(newstatus, "------------newstatus--------", 6);
    
    -- if newstatus == yl.US_FREE or newstatus == yl.US_NULL then
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then
        print("删除")
        self._dataModle:removeUser(useritem);

        --刷新用户列表
        -- self._gameView:refreshUserList();
        --刷新桌上用户
        -- self._gameView:InitTableUser();
    else
        --刷新用户信息
        self._dataModle:updateUser(useritem);
        print("刷新用户信息")
    end
    -- assert(false)
  
end

function GameLayer:onEventUserScore(item)
    -- dump(item, "_----------------item---666--------", 6);
    self._dataModle:updateUser(item);
    self._gameView:onGetUserScore(item);

    --刷新用户列表
    -- self._gameView:refreshUserList();
    --刷新桌上用户
    -- self._gameView:InitTableUser();

    --庄家
    if self._gameView.m_wBankerUser == item.wChairID then
        --庄家游戏币
        self._gameView.m_lBankerScore = item.lScore;
        self._gameView:resetBankerScore();

        --自己分数
        -- self._gameView:UpdateUserMoney(item.lScore);
    end
end

---------------------------------------------------------------------------------------
return GameLayer