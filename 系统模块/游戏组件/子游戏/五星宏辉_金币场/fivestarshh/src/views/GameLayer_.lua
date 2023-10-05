local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local module_pre = "game.yule.fivestarshh.src";
require("cocos.init")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
-- local cmd = module_pre .. ".models.CMD_Game"
-- local cmd = appdf.req(appdf.GAME_SRC.."yule.fivestarshh.src.models.CMD_Game")
local cmd = appdf.req(module_pre..".models.CMD_Game")
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local GameLogic = module_pre .. ".models.GameLogic";
local GameViewLayer = appdf.req(module_pre .. ".views.layer.GameViewLayer")
local QueryDialog   = require("app.views.layer.other.QueryDialog")
local g_var = ExternalFun.req_var
local GameFrame = appdf.req(module_pre .. ".models.GameFrame")

--下注失败提示
local JettonFailResult = 
{
    [0] = "当前不是下注阶段",
    [1] = "当前下注超过个人携带",
    [2] = "当前下注超过个人下注限制",
    [3] = "下注总金额超过区域下注限制",
    [4] = "下注总金额超过区域赔付限制",
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

    --倒计时重置开始
    self._gameView:resertShowCutTimeLabel();
end

------网络发送
--玩家下注
function GameLayer:sendPlaceJetton(areaIndex, jettonMoney)
    local cmddata = ExternalFun.create_netdata(cmd.CMD_C_PlaceBet);
    -- cmddata:pushword(self:GetMeChairID());
    cmddata:pushbyte(areaIndex);
    cmddata:pushscore(jettonMoney);

    self:SendData(cmd.SUB_C_PLACE_JETTON, cmddata);
end

function GameLayer:sendClearJetton()
    local cmddata = ExternalFun.create_netdata(cmd.CMD_C_ClearBet);
     cmddata:pushword(self:GetMeChairID());
    self:SendData(cmd.SUB_C_CLEAR_JETTON, cmddata);
end


--路单信息
function GameLayer:historyRecord(cbLuDanSmallCount, cbLuDanSmallData)
    self._gameView:onTrendInfoFresh();
    self._gameView:showRecordResult(cbLuDanSmallCount, cbLuDanSmallData);
end

--申请庄家
function GameLayer:sendApplyBanker()
    local cmddata = CCmd_Data:create(0);
    self:SendData(cmd.SUB_C_APPLY_BANKER, cmddata);
end

--取消申请、申请下庄
function GameLayer:sendCancelApply()
print("-----------取消申请、申请下庄--------------")
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

        --1.根据剩余时间，初始化定时器
        self:SetGameClock(currentGameStatus, cmd_data.cbTimeLeave)

        --2.显示牌背面
        self._gameView:showCardBack();
        --3.火动画
        self._gameView:PlayCloseCardAnim(false);

        --4.路单
        self._gameView.cbLuDanBigCount = cmd_data.cbLuDanBigCount;
        self._gameView.cbLuDanBigData = cmd_data.cbLuDanBigData[1];
        self._gameView.cbLuDanSmallCount = cmd_data.cbLuDanSmallCount;
        self._gameView.cbLuDanSmallData = cmd_data.cbLuDanSmallData[1];
        --4.路单
        self:historyRecord(cmd_data.cbLuDanSmallCount, cmd_data.cbLuDanSmallData[1]);

        --5.玩家总输赢
        self._gameView.lUserAllWinLose = 0;
        self._gameView:freshUserAllWinLose();

        
    elseif cbGameStatus == cmd.GAME_SCENE_BET  or cbGameStatus == cmd.GAME_SCENE_END then  --下注状态、结束状态
        cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer);
        dump(cmd_data,"下注状态、结束状态",6);

        --6.初始化区域个人、总的已下注UI
        self._gameView:InitUserAndAllJettonLabel(cmd_data.lAllBet[1], cmd_data.lPlayBet[1]);
        --1.根据剩余时间，初始化定时器
        self:SetGameClock(currentGameStatus, cmd_data.cbTimeLeave)

        --2.显示牌背面
        self._gameView:showCardBack();
        --3.火动画
        self._gameView:PlayCloseCardAnim(false);

        --4.路单
        self._gameView.cbLuDanBigCount = cmd_data.cbLuDanBigCount;
        self._gameView.cbLuDanBigData = cmd_data.cbLuDanBigData[1];
        self._gameView.cbLuDanSmallCount = cmd_data.cbLuDanSmallCount;
        self._gameView.cbLuDanSmallData = cmd_data.cbLuDanSmallData[1];
    
        --7.是否有下注
        for i=1,5 do
            -- 1 红区 、2 黑区、 3 牌型区
            local userAreaTotalJettonScore = cmd_data.lPlayBet[1][i];
            if userAreaTotalJettonScore > 0 then
                self.m_cbGameStatus=cmd.GAME_SCENE_BET;
            end
        end
        --若处于下注
        currentGameStatus = cmd_data.cbGameStatus;

        --下注、结算阶段
        if currentGameStatus == cmd.GAME_SCENE_BET then
            --8.启用游戏按钮
            self._gameView:SwitchButtonEnabled(true);
            --4.路单
            self:historyRecord(cmd_data.cbLuDanSmallCount, cmd_data.cbLuDanSmallData[1]);

            --5.玩家总输赢
            -- self._gameView.lUserAllWinLose = cmd_data.lUserAllWinLose;
            --改为显示本局输赢
            self._gameView.lUserAllWinLose = 0;
            self._gameView:freshUserAllWinLose();
        elseif currentGameStatus == cmd.GAME_SCENE_END then
            --8.关闭游戏按钮
            self._gameView:SwitchButtonEnabled(false);
            --4.路单
            self:historyRecord(cmd_data.cbLuDanSmallCount, cmd_data.cbLuDanSmallData[1]);

            --玩家本局输赢、已减税收
            self._gameView.m_PlayerWinScore = cmd_data.lGameScore;

            --5.玩家总输赢
            -- self._gameView.lUserAllWinLose = cmd_data.lUserAllWinLose - self._gameView.m_PlayerWinScore;
            --改为显示本局输赢
            self._gameView.lUserAllWinLose = 0;
            self._gameView:freshUserAllWinLose();

            --剩余倒计时
            local time1 = 0.0;
            local time2 = 0.0;
            local time3 = 0.0;
            local time4 = 0.0;
            local residueTime = cmd_data.cbTimeLeave - 2;

            if residueTime >= 2 then
                time4 = 2.0;
            end
            if residueTime >= 4 then
                time3 = 2.0;
            end
            if residueTime >= 6 then
                time2 = 2.0;
            end
            if residueTime >= 7 then
                time1 = 1.0;
            end

            self:stopAllActions();
            self:runAction(cc.Sequence:create(
                cc.CallFunc:create( function()
                    --1.火动画
                    self._gameView:PlayCloseCardAnim(true);
                end ),
                cc.DelayTime:create(time1),
                cc.CallFunc:create( function()
                    --2.播放开牌动画
                    if residueTime >= 6 then
                        -- 闪烁动画
                        self._gameView:PlayOpenCardPreAnim(cmd_data.cbCurrentOpenCard, true);
                    else
                        self._gameView:PlayOpenCardPreAnim(cmd_data.cbCurrentOpenCard, true);
                    end
                    
                end ),
                cc.DelayTime:create(time2),
                cc.CallFunc:create( function()
                    --3.路单
                    if residueTime >= 4 then
                        --飞花前
                        self._gameView:PlayFlyFlowerLightAnim(cmd_data.cbCurrentOpenCard);
                    else
                        --路单刷新
                        self:historyRecord(cmd_data.cbLuDanSmallCount, cmd_data.cbLuDanSmallData[1]);
                    end
                    
                end ),
                cc.DelayTime:create(time3),
                cc.CallFunc:create( function()
                    --4.显示玩家得分动画、已减税收
                    self._gameView.m_PlayerWinScore = cmd_data.lGameScore;
                    -- self._gameView.m_PlayerWinScore = 1800000;
                    if self._gameView.m_PlayerWinScore > 0 then
                        self._gameView:PlayPlayerWinAnim();
                    end
                end ),
                cc.DelayTime:create(time4),
                cc.CallFunc:create( function()
                    --5.总输赢
                    -- if residueTime >= 0 then
                    --     --飞金币
                    --     if self._gameView.m_PlayerWinScore > 0 then
                    --         self._gameView:flyCurGameGold(self._gameView.m_PlayerWinScore);
                    --     end
                    -- else
                    --     --玩家总输赢
                    --     self._gameView.lUserAllWinLose = cmd_data.lUserAllWinLose;
                    --     self._gameView:freshUserAllWinLose();
                    -- end
                    --玩家总输赢
                    -- self._gameView.lUserAllWinLose = cmd_data.lUserAllWinLose;
                    --改为显示本局总输赢
                    self._gameView.lUserAllWinLose = self._gameView.m_PlayerWinScore;
                    self._gameView:freshUserAllWinLose();

                    --手动刷新玩家金币
                    if self._gameView.m_PlayerWinScore > 0 then
                        --玩家分+赢分+下注分
                        local areaIndex = self._gameView:getAreaIndexByCard(cmd_table.cbCurrentOpenCard);

                        local playerJettonTotal = 0;
                        for i = 1, 5 do
                            local index = self._gameView:getIndexByViewId(i);
                            playerJettonTotal = playerJettonTotal + self._gameView.m_CurrentJettonCount[index-1];
                        end 
                        local currentUserMoney = self._gameView.m_PlayerMoney + self._gameView.m_PlayerWinScore + playerJettonTotal;
                        --self._gameView:UpdateUserMoney(currentUserMoney);
                    end
                    
                end)
            ));

            --游戏币不足
            if cmd_data.cbTimeLeave - 2 >= 2 then
                self:runAction(cc.Sequence:create(
                    cc.DelayTime:create(cmd_data.cbTimeLeave - 2),
                    cc.CallFunc:create( function()
                        if self._gameView.m_PlayerMoney + self._gameView.lUserAllWinLose < GlobalUserItem.GetRoomInfo().lEnterScore then
                            showToast(self, "游戏币不足!", 2);
                        end
                    end )
                ));
            end
            
        else
            -- --若不处于下注状态，则显示等待开局倒计时UI
            -- currentGameStatus = cmd.GAME_SCENE_END;
            -- self._gameView:ShowWaitStartGame(cmd_data.cbTimeLeave);
        end
    else
    end

    --1.庄家相关
    self._dataModle:removeApplyUser(cmd_data.wBankerUser);
    --抢庄条件
    self._gameView:onGetApplyBankerCondition(cmd_data.lApplyBankerCondition);
    -- --是否显示庄家
    -- self._gameView:onIsShowApplyBanker(cmd_data.bIsAllowApplyBanker);
    --当前庄家
    self._gameView.m_wBankerUser = cmd_data.wBankerUser;
    --庄家赢分
    self._gameView.m_lBankerWinScore = cmd_data.lBankerWinScore;
    --当前庄家分数
    self._gameView.m_lBankerScore = cmd_data.lBankerScore;
    --庄家局数
    self._gameView.cbBankerTime = cmd_data.wBankerTime;
    --刷新庄家信息
    self._gameView:resetBankerInfo();

    --2.设置游戏状态
    self._gameView:SetCurrentGameStatus(currentGameStatus);

    --3.初始化 玩家单局下注限制、区域下注限制
    self._gameView:InitUserMaxJetton(cmd_data.lUserLimitScore, cmd_data.lAreaLimitScore);

    --控制信息
    --4.下注配置
    self._gameView.lChipCount = cmd_data.lChipCount[1];
    self._gameView:onExchangeBet(true);

    --5.局数、花色
    self._gameView.nGameCount = cmd_data.nGameCount;
    self._gameView.nOpenColorCount = cmd_data.nOpenColorCount[1];
    self._gameView:freshExchangeRecordShow();
    --6.轮数、局数
    self._gameView:freshCicleAndGame();

    --7.大奖广播隐藏
    self._gameView:hideTipsBottom();

    --8.播放背景音乐
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
        -- self.m_cbGameStatus = cmd.GAME_SCENE_FREE;
		self:onSubGameEnd(dataBuffer);
    elseif sub == cmd.SUB_S_PLACE_JETTON_FAIL then  --下注失败
        -- print("下注失败");
        self:onSubJettonFail(dataBuffer);
    elseif sub == cmd.SUB_S_HORN_INFO then  --大奖广播
        -- print("大奖广播");
        self:onSubHornInfo(dataBuffer);
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
    dump(cmd_table,"游戏空闲",6);

    --0.重置本局数据
    self._gameView:ReSetGameData();
    self._gameView:SetCurrentGameStatus(cmd.GAME_SCENE_FREE);

    --1.初始化定时器
    self:SetGameClock(cmd.GAME_SCENE_FREE, cmd_table.cbTimeLeave)

    --申请按钮状态更新
    -- self._gameView:refreshApplyBtnState();
    --2.初始化个人信息
    self._gameView:initSelfInfo();

    --3.休息一下动画
    self._gameView:PlayAnimByGameState(1);

    --4.局数、花色
    self._gameView.nGameCount = cmd_table.nGameCount;
    self._gameView.nOpenColorCount = cmd_table.nOpenColorCount[1];
    self._gameView:freshExchangeRecordShow()
    --5.轮数、局数
    self._gameView:freshCicleAndGame();

    --6.路单
    self._gameView.cbLuDanBigCount = cmd_table.cbLuDanBigCount;
    self._gameView.cbLuDanBigData = cmd_table.cbLuDanBigData[1];

    self._gameView.cbLuDanSmallCount = cmd_table.cbLuDanSmallCount;
    self._gameView.cbLuDanSmallData = cmd_table.cbLuDanSmallData[1];

    --7.换牌
    self._gameView:randomShowCardBack();
    
    self:runAction(cc.Sequence:create(
        cc.CallFunc:create( function()
            --1.显示牌背面
            self._gameView:showCardBack();
            --2.火动画
            self._gameView:PlayCloseCardAnim(false);
        end ),
        cc.DelayTime:create(1.0),
        cc.CallFunc:create( function()
            --3.路单
            self:historyRecord(cmd_table.cbLuDanSmallCount, cmd_table.cbLuDanSmallData[1]);
        end),
        cc.DelayTime:create(2.0),
        cc.CallFunc:create( function()
            -- --4.换牌
            -- self._gameView:randomShowCardBack();

            --5.总输赢、显示本局输赢、重置
            self._gameView.lUserAllWinLose = 0;
            self._gameView:freshUserAllWinLose();
        end)
    ));
    
    --7.总输赢
    -- self._gameView.lUserAllWinLose = cmd_table.lUserAllWinLose;
    -- self._gameView:freshUserAllWinLose();
    
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameStart,dataBuffer);
    dump(cmd_table,"游戏开始",6)
    
    --播放开始音效
    local currentGameStatus = cmd.GAME_SCENE_BET;

    --0.重置本局数据
    self._gameView:ReSetGameData();
    --1.设置定时器
    self:SetGameClock(currentGameStatus, cmd_table.cbTimeLeave)
    --2.设置当前游戏状态
    self._gameView:SetCurrentGameStatus(currentGameStatus)
    --开始下注动画
    self._gameView:PlayAnimByGameState(2);
    --4.启用游戏按钮
    self._gameView:SwitchButtonEnabled(true)
    --5.当前庄家
    self._gameView.m_wBankerUser = cmd_table.wBankerUser;
    --当前庄家分数
    self._gameView.m_lBankerScore = cmd_table.lBankerScore;
    --刷新庄家分数、名字
    self._gameView:resetBankerInfo()

    --系统坐庄不显示分数
    if self._gameView.m_wBankerUser ~= yl.INVALID_CHAIR then
        self._gameView:resetBankerScore();
    end
    --申请按钮状态更新
    self._gameView:refreshApplyBtnState();

end

--用户下注
function GameLayer:OnSubPlateJetton(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_PlaceBet, dataBuffer);
    dump(cmd_table,"用户下注",6);

    local isSelf = self:GetMeChairID() == cmd_table.wChairID;

    --自己下注改变状态
    if isSelf then
        self.m_cbGameStatus = cmd.GAME_SCENE_BET;
    end
    self._gameView:PlayerJetton(cmd_table, isSelf);
end

--游戏结束
function GameLayer:onSubGameEnd(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameEnd,dataBuffer);
    dump(cmd_table,"游戏结束",6);
    
    self._gameView.isEndGame = true;

    local currentGameStatus = cmd.GAME_SCENE_END;

    -- local isOpenReward = cmd_table.bIsOpenReward;

    --5.庄家赢分
    self._gameView.m_lBankerWinScore = cmd_table.lBankerScore;
    self._gameView.cbBankerTime = cmd_table.nBankerTime;

    --1.设置定时器
    self:SetGameClock(currentGameStatus, cmd_table.cbTimeLeave)

    --2.设置游戏状态
    self._gameView:SetCurrentGameStatus(currentGameStatus);
    --3.禁用游戏按钮
    self._gameView:SwitchButtonEnabled(false);
    --4.玩家本轮赢分、已减税收
     local chairId = self:GetMeChairID();
     local score = cmd_table.lGameScore[1][chairId];
    self._gameView.m_PlayerWinScore = cmd_table.lGameScore[1][chairId];
   
    self:stopAllActions();
    self:runAction(cc.Sequence:create(
        cc.CallFunc:create( function()
            --1.停止下注动画
            self._gameView:PlayAnimByGameState(3);
            --2.火动画
            self._gameView:PlayCloseCardAnim(true);
        end ),
        cc.DelayTime:create(1.0),
        cc.CallFunc:create( function()
            --3.播放开牌动画
            self._gameView:PlayOpenCardPreAnim(cmd_table.cbCurrentOpenCard, true);
        end ),
        cc.DelayTime:create(2.0),
        cc.CallFunc:create( function()
            --4.飞花前
            self._gameView:PlayFlyFlowerLightAnim(cmd_table.cbCurrentOpenCard);
            
        end ),
        cc.DelayTime:create(2.0),
        cc.CallFunc:create( function()
            --5.结算弹窗
            -- self._gameView.m_PlayerWinScore = 1800000;
            if self._gameView.m_PlayerWinScore > 0 then
                self._gameView:PlayPlayerWinAnim();
                -- self._gameView:PlayGameEndAnim();
            end
        end ),
        cc.DelayTime:create(2.0),
        cc.CallFunc:create( function()
            --6.飞金币
            -- self._gameView:flyCurGameGold(self._gameView.m_PlayerWinScore);
            --玩家总输赢
            -- self._gameView.lUserAllWinLose = self._gameView.lUserAllWinLose + self._gameView.m_PlayerWinScore;
            --改为显示本局输赢
            self._gameView.lUserAllWinLose = self._gameView.m_PlayerWinScore;
            self._gameView:freshUserAllWinLose();

            --手动刷新玩家金币
            if self._gameView.m_PlayerWinScore > 0 then
                --玩家分+赢分+下注分
                local areaIndex = self._gameView:getAreaIndexByCard(cmd_table.cbCurrentOpenCard);
                local playerJettonTotal = 0;
                for i = 1, 5 do
                    local index = self._gameView:getIndexByViewId(i);
                    playerJettonTotal = playerJettonTotal + self._gameView.m_CurrentJettonCount[index-1];
                end 
                local currentUserMoney = self._gameView.m_PlayerMoney + self._gameView.m_PlayerWinScore + playerJettonTotal;
                --self._gameView:UpdateUserMoney(currentUserMoney);
            end
        end )
    ));

    if self._gameView.m_PlayerWinScore < 0 then
        --游戏币不足
        self:runAction(cc.Sequence:create(
            cc.DelayTime:create(cmd_table.cbTimeLeave - 2),
            cc.CallFunc:create( function()
                -- if self._gameView.m_PlayerMoney < GlobalUserItem.GetRoomInfo().lEnterScore then
                    -- showToast(self, "游戏币不足!", 2);
                -- end
            end )
        ));
    end

end

--申请庄家
function GameLayer:onApplyBanker(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_ApplyBanker,dataBuffer);
    dump(cmd_table,"--------申请庄家-----",6)

    self._dataModle:addApplyUser(cmd_table.wApplyUser, false);
    self._gameView:onApplyBanker(cmd_table);
end

--取消申请
function GameLayer:onCancelBanker(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_CancelBanker,dataBuffer);
    dump(cmd_table,"--------取消申请-----",6)

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

--下注失败
function GameLayer:onSubJettonFail(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_PlaceBetFail, dataBuffer);
    -- dump(cmd_table,"--------下注失败-----",6)
    local myChairId = self:GetMeChairID();

    local wChairID = cmd_table.wChairID;
    local cbBetArea = cmd_table.cbBetArea;
    local lPlaceScore = cmd_table.lPlaceScore;
    local cbReason = cmd_table.cbReason;
    
    --自己下注
    if cmd_table.wChairID == myChairId then
        if JettonFailResult[cbReason] then
            showToast(self, "下注失败: " .. JettonFailResult[cbReason], 2);
        else
            showToast(self, "下注失败", 2);
        end
    end
end

--大奖广播
function GameLayer:onSubHornInfo(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_HORN, dataBuffer);
    -- dump(cmd_table,"--------大奖广播-----",6)

    --播报额度、倍数、数量
    local lScore = cmd_table.lScore;
    local lTimes = cmd_table.lTimes;
    local cbCount = cmd_table.cbCount;

    --大奖广播
    self._gameView:isShowTipsBottom(true);
    for i=1, cbCount do
        local cmd_table = ExternalFun.read_netdata(cmd.HORNINFO, dataBuffer)
        if nil ~= cmd_table then
            --广播数组
            -- table.insert(self._gameView.m_infoList, cmd_table);
            --广播
            self._gameView:ShowAwardTip(cmd_table, cbCount);
        end
    end
    -- error("0")
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
    -- if self._gameView.m_wBankerUser == item.wChairID then
    --     --庄家游戏币
    --     -- self._gameView.m_lBankerScore = item.lScore;
    --     -- self._gameView:resetBankerScore();

    --     --自己分数
    --     -- self._gameView:UpdateUserMoney(item.lScore);
    -- end
end

---------------------------------------------------------------------------------------
return GameLayer