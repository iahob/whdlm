local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)
local cmd = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.CMD_Game")
local GameLogic = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.GameLogic")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.views.layer.GameViewLayer")
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

function GameLayer:onExit()
    if self.compareFun then
        -- 注销时钟
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.compareFun)
        self.compareFun = nil
    end
    self:KillGameClock()
    self:dismissPopWait()
    GameLayer.super.onExit(self)
end

function GameLayer:requestChangeDesk()
    self._gameFrame:QueryChangeDesk()
end


--别人退出桌子（GameModel做了头像清理，如果要清理其他东西在这里处理）
--强退后做托管处理，不清理头像
function GameLayer:onEventUserStatus(useritem, newstatus, oldstatus)
    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
    local MyChair = self:GetMeChairID()

    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end

    if newstatus.cbUserStatus == yl.US_LOOKON then
        return
    end

    -- 房卡模式游戏中需显示断线状态
    if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then    --积分场
        -- 旧的清除
        -- if self.m_cbGameStatus == cmd.GS_TK_FREE then
            if oldstatus.wTableID == MyTable then
                local viewid = self:SwitchViewChairID(oldstatus.wChairID)
                if viewid and viewid ~= yl.INVALID_CHAIR then
                    self._gameView:OnUpdateUser(viewid, nil)
                    if PriRoom then
                        PriRoom:getInstance():onEventUserState(viewid, useritem, true)
                        if self._gameView.m_RoomHolderFlag and useritem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then
                            self._gameView.m_RoomHolderFlag:setVisible(false)
                        end
                    end
                end
                if self.endShow then
                    self:onClearTable()
                end
            end
        -- end
        if useritem.cbUserStatus == yl.US_OFFLINE then
            if oldstatus.wTableID == MyTable then
                local viewid = self:SwitchViewChairID(oldstatus.wChairID)
                if viewid and viewid ~= yl.INVALID_CHAIR and viewid ~= cmd.MY_VIEWID then
                    self._gameView:OnUpdateUser(viewid, useritem)
                    if PriRoom then
                        PriRoom:getInstance():onEventUserState(viewid, useritem, true)
                    end
                end
                if self.endShow then
                    self:onClearTable()
                end
            end
            return
        end
    else
        -- 旧的清除
        -- if self.m_cbGameStatus == cmd.GS_TK_FREE then
            if newstatus.wChairID == yl.INVALID_CHAIR then
                if oldstatus.wTableID == MyTable then
                    local viewid = self:SwitchViewChairID(oldstatus.wChairID)
                    if viewid and viewid ~= yl.INVALID_CHAIR then
                        self._gameView:OnUpdateUser(viewid, nil)
                        if PriRoom then
                            PriRoom:getInstance():onEventUserState(viewid, useritem, true)
                        end
                    end
                    if self.endShow then
                        self:onClearTable()
                    end
                    if self._gameView.userWinLight then
                        local k = self:SwitchViewChairID(oldstatus.wChairID)
                        if self._gameView.userWinLight[k] then
                            -- 注销时钟
                            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._gameView.userWinLight[k])
                            self._gameView.userWinLight[k] = nil
                        end
                    end
                    self:onSaveUsers()
                end
            end
        -- end
    end

    -- 更新新状态
    if newstatus.wTableID == MyTable then
        local viewid = self:SwitchViewChairID(newstatus.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)
            self:onSaveUsers()
            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, false)
            end
        end
    end
end

--自己退出桌子
function GameLayer:onExitTable()
    local this = self
    self:KillGameClock()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        self:showPopWait()
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(
                function () 
                    this._gameFrame:StandUp(1)
                end
                ),
            cc.DelayTime:create(10),
            cc.CallFunc:create(
                function ()
                    print("delay leave")
                    this:onExitRoom()
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
--    self._gameFrame:onCloseSocket()
    self._scene:onKeyBack() --_scene是ClientScnen
end

--系统消息
function GameLayer:onSystemMessage( wType,szString )
--    if wType == 501 or wType == 515 then
        GlobalUserItem.bWaitQuit = true
        self:onSubNoticeAddScore(szString)
--    end
end

function GameLayer:onSubNoticeAddScore(szString)
    --设置准备按钮不可见
    self.m_bNoScore = true

    if self.m_bStartGame then
        local msg = szString or "你的游戏币不足，无法继续游戏"
        self.m_querydialog = QueryDialog:create(msg,function()
            self:onExitTable()
        end,nil,1)
        self.m_querydialog:setCanTouchOutside(false)
        self.m_querydialog:addTo(self)
    else
        self.m_bNoScore = true
        self.m_szScoreMsg = szString
    end
end

function GameLayer:SwitchViewChairID(chair)
    local viewid = yl.INVALID_CHAIR
    local nChairCount = cmd.GAME_PLAYER
    local nChairID = self:GetMeChairID()
    if chair ~= yl.INVALID_CHAIR and chair < nChairCount then
        viewid = math.mod(chair + math.floor(nChairCount * 4/3) - nChairID, nChairCount) + 1
    end
    return viewid
end

function GameLayer:onGetSitUserNum()
    local userNum = 0
     for i=1, cmd.GAME_PLAYER do
         
     end
    return userNum
    --return table.nums(self._gameView.nodePlayer)
end

function GameLayer:getUserInfoByChairID(chairId)
    return self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), chairId)
end

--获取gamekind
function GameLayer:getGameKind()
    return cmd.KIND_ID
end

-- 初始化界面
function GameLayer:ctor(frameEngine,scene)
    GameLayer.super.ctor(self,frameEngine,scene)
end

-- 创建场景
function GameLayer:CreateView()
    --cc.FileUtils:getInstance():addSearchPath(device.writablePath..cmd.RES,true)
    return GameViewLayer:create(self)
        :addTo(self)
end

function GameLayer:getParentNode( )
    return self._scene
end
--设置私有房的层级
function GameLayer:addPrivateGameLayer( layer )
    if nil == layer then
        return
    end
        self._gameView:addChild(layer, 9)
end

-- 初始化游戏数据
function GameLayer:OnInitGameEngine()
    GameLayer.super.OnInitGameEngine(self)

    self.m_wCurrentUser = yl.INVALID_CHAIR              --当前用户
    self.m_wBankerUser = yl.INVALID_CHAIR               --庄家用户

    self.m_cbPlayStatus = {0,0,0,0,0,0,0,0}                     --游戏状态
    self.m_lTableScore = {0,0,0,0,0,0,0,0}                      --下注数目
    self.cbPlayStatus = {0,0,0,0,0,0,0,0}                       --有可能与游戏状态是同一个变量

    self.m_lMaxCellScore = 0                            --单元上限
    self.m_lCellScore = 0                               --单元下注

    self.m_lCurrentTimes = 0                            --当前倍数
    self.m_lUserMaxScore = 0                            --最大分数

    self.m_bLookCard  = {false,false,false,false,false,false,false,false}        --看牌动作

    self.m_wLostUser = yl.INVALID_CHAIR                 --比牌失败
    self.m_wWinnerUser = yl.INVALID_CHAIR               --胜利用户 

    self.m_lAllTableScore = 0

    self.m_bNoScore = false   --游戏币不足
    self.m_bStartGame = false


    self.isPriOver = false
    -- 提示
    self.m_szScoreMsg = ""
    
    self.chUserAESKey = 
    {
        0x32, 0x43, 0xF6, 0xA8,
        0x88, 0x5A, 0x30, 0x8D,
        0x31, 0x31, 0x98, 0xA2,
        0xE0, 0x37, 0x07, 0x34
    }
    self.nVoiceFollow = {0, 0, 0, 0, 0, 0, 0, 0}

    self.userPlaying = {}                     --当局玩家

    self.cbCardData = {}
    self.sendCardCount = 0
    self.trust = false

    self.isRobBanker = false
    self.callBankerUser = {}  
    self.notCallBankerUser = {}     

    self.trustConfig = {}
    self.trustConfig.BankerType = 1
    self.trustConfig.BankerRatio = 1  --1为最小倍数
    self.trustConfig.BetsType = 1
    self.trustConfig.BetsConfig = 1   --7为最小条件

    --流水
    self.m_userPriRecord = {}

    --房卡模式
    self.cbPlayMode = 0  --默认积分模式
    self.bSupportTuizhu = false   --是否支持推注

    --是否是旁观
    self.m_bLookOn = false


    self.m_playingUser = {}
end

-- 重置游戏数据
function GameLayer:OnResetGameEngine()
    GameLayer.super.OnResetGameEngine(self)

    self._gameView:OnResetView()

    self.m_wCurrentUser = yl.INVALID_CHAIR              --当前用户
    self.m_wBankerUser = yl.INVALID_CHAIR               --庄家用户
    self.m_cbPlayStatus = {0,0,0,0,0,0,0,0}                     --游戏状态
    self.m_lTableScore = {0,0,0,0,0,0,0,0}                      --下注数目
    self.cbCanTuiZhu = {0,0,0,0,0,0,0,0}                --是否可推注
    self.m_lMaxCellScore = 0                            --单元上限
    self.m_lCellScore = 0                               --单元下注
    self.m_lCurrentTimes = 0                            --当前倍数
    self.m_lUserMaxScore = 0                            --最大分数
    self.m_bLookCard  = {false,false,false,false,false,false,false,false}        --看牌动作
    self.m_wLostUser = yl.INVALID_CHAIR                 --比牌失败
    self.m_wWinnerUser = yl.INVALID_CHAIR               --胜利用户 
    self.m_lAllTableScore = 0

    self.userPlaying = {}                     --当局玩家

    self.cbCardData = {}
    self.sendCardCount = 0

    self.trust = false

    self.isRobBanker = false
    self.callBankerUser = {} 
    self.notCallBankerUser = {}    

    self.trustConfig = {}
    self.trustConfig.BankerType = 1
    self.trustConfig.BankerRatio = 1
    self.trustConfig.BetsType = 1
    self.trustConfig.BetsConfig = 1

    --流水
    self.m_userPriRecord = {}
    --房卡模式
    self.cbPlayMode = 0  --默认积分模式
    self.bSupportTuizhu = false   --是否支持推注

    --是否是旁观
    self.m_bLookOn = false

    
    self.m_playingUser = {}
end

-- 设置计时器
function GameLayer:SetGameClock(chair,id,time)
    GameLayer.super.SetGameClock(self,chair,id,time)
end

-- 关闭计时器
-- function GameLayer:KillGameClock(notView)
--     local viewid = self:GetClockViewID()
--     if viewid and viewid ~= yl.INVALID_CHAIR then
--         local progress = self._gameView.m_TimeProgress[viewid]
--         if progress ~= nil then
--             progress:stopAllActions()
--             progress:setVisible(false)
--         end
--     end
    
--     if GameLayer.super.KillGameClock then
--         GameLayer.super.KillGameClock(self,notView)
--     end
-- end

--获得当前正在玩的玩家数量
function GameLayer:getPlayingNum()
    local num = 0
    for i = 1, cmd.GAME_PLAYER do
        if self.m_cbPlayStatus[i] == 1 then
            num = num + 1
        end
    end
    return num
end

--获取房间的玩家
function GameLayer:getSaveUserInfos(wchairid)
    for k,v in pairs(self.onSaveUser) do
        if v.wChairID == wchairid then
            return v
        end
    end
    return self.onSaveUser[wchairid +1]
end

-- 时钟处理
function GameLayer:OnEventGameClockInfo(chair,time,clockid)

    print("OnEventGameClockInfo----------->", clockid, time)

    if self.m_bLookOn then
        return
    end

     --房卡不托管
    if GlobalUserItem.bPrivateRoom then
        if time <= 0 then
            if not GlobalUserItem.bPrivateGoldRoom then
                return true
            end
        end
    end 

    if time == 3 then
        self._gameView:showGamCountdownWarning()
    end

    if time < 4 then
        self:PlaySound(cmd.RES.."sound_res/daoshu.mp3")
    end
    if clockid == cmd.EM_STATUS_FREE then

        -- if (self.trust and time<=math.max(self.cbTimeCallBanker-2,1)) then
        --     return self._gameView:OnButtonClickedEvent(self._gameView.BT_READY)
        -- end

        self._gameView:OnUpdataClockViewByStatus(cmd.EM_STATUS_FREE, time)

        if time <= 0 then
            self:onExitTable()
            return true
        end
    elseif clockid == cmd.EM_STATUS_ROBBANKER then
        -- if (self.trust and time<=math.max(self.cbTimeCallBanker-2,1)) then
        --     return self._gameView:OnButtonClickedEvent(self._gameView.BT_READY)
        -- end
        self._gameView:OnUpdataClockViewByStatus(cmd.EM_STATUS_ROBBANKER, time)
        if time <= 0 and not self.isRobBanker then
            --自动托管
            self._gameView:ShowTrust(true)
            return true
        end
    elseif clockid == cmd.EM_STATUS_JETTON then
        -- if (self.trust and time<=math.max(self.cbTimeCallBanker-2,1) and false) then 
        --     self._gameView:OnButtonClickedEvent(self._gameView.BT_CALLBANKER)
        -- end
        self._gameView:OnUpdataClockViewByStatus(cmd.EM_STATUS_JETTON, time)
        if time <= 0 then
            --自动托管
            self._gameView:ShowTrust(true)
            return true
        end
    elseif clockid == cmd.EM_STATUS_OPEN_CARD then
        self._gameView:OnUpdataClockViewByStatus(cmd.EM_STATUS_OPEN_CARD, time)
        if time <= 0 then
            --自动托管
            self._gameView:ShowTrust(true)
            return true
        end
    elseif clockid == cmd.EM_STATUS_GAME_END then
        self._gameView:OnUpdataClockViewByStatus(cmd.EM_STATUS_GAME_END, time)
    end
end

function GameLayer:resetPlayerState()
    self.m_cbPlayStatus = {0,0,0,0,0,0,0,0}                     --游戏状态
    self.cbPlayStatus = {0,0,0,0,0,0,0,0}  
    self.cbCanTuiZhu = {0,0,0,0,0,0,0,0}  
end

-- 场景信息（断线重连走这里）
function GameLayer:onEventGameScene(cbGameStatus, dataBuffer)
    --换桌清除托管界面
    self.trust = false
    self._gameView:ShowTrust(false)

    self._gameView.JETTON_MODE = 0       --默认最小额度下注

    local myUserItem = self:GetMeUserItem()
    if myUserItem.cbUserStatus == yl.US_LOOKON then 
        self.m_bLookOn = true
    end


    -- self._gameView:showJettonNum()
    -- self._gameView:changeAutoImg()

    self:resetPlayerState()               --防止换桌以后，换桌按钮不显示
    --换桌后清理桌面
    self._gameView:onClearTips()
    self._gameView:cleanDesk()
    
    self:onSaveUsers()
    -- 初始化已有玩家
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i - 1)
        local wViewChairId = self:SwitchViewChairID(i - 1)
        self._gameView:OnUpdateUser(wViewChairId, userItem)
    end
    self.m_cbGameStatus = cbGameStatus
    self.m_wCurrentUser = self:GetMeChairID()

    if cbGameStatus == cmd.GAME_STATUS_FREE then                                 --空闲状态
        self:onEventGameSceneFree(dataBuffer)
    elseif cbGameStatus == cmd.GAME_STATUS_PLAY then                             --游戏状态
        self:onEventGameScenePlay(dataBuffer)
        self.bIsHaveShowEnd = false
    end

    self._gameView:showSitBtn(self.m_bLookOn)


    if GlobalUserItem.bPrivateRoom then
        self._gameView.btn_changeDesk:setEnabled(false);
        if not GlobalUserItem.bPrivateGoldRoom then
            self._gameView.btnTrust:setVisible(false)
        end
    end

    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end
end

function GameLayer:onEventGameSceneFree(dataBuffer)
    -- 空闲状态
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
    dump(cmd_table, "onEventGameSceneFree", 7)

    --记录各阶段操作时间
    self.cbStatus = cmd_table.cbStatus
    self.cbFreeTime = cmd_table.cbFreeTime
    self.cbOperateBankerTime = cmd_table.cbOperateBankerTime
    self.cbJettonTime = cmd_table.cbJettonTime
    self.cbOpenCardTime = cmd_table.cbOpenCardTime
    self.cbCompareCardTime = cmd_table.cbCompareCardTime
    self.cbGameEndTime = cmd_table.cbGameEndTime

    self.cbRobBankerType = cmd_table.cbRobBankerType
    self.cbRobBankerRatio = cmd_table.cbRobBankerRatio
    self.lJettonButton = cmd_table.lJettonButton
    self.lCardTypeTime = cmd_table.lCardTypeTime
    self.cbCardTypeTimesRule = cmd_table.cbCardTypeTimesRule
    self.dCellScore = cmd_table.dCellScore

        --游戏模式
    self.cbPlayMode = cmd_table.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1  or false
    self.bSupportTuizhu = cmd_table.bSupportTuizhu

    --判断是否是通比模式
    if self.cbRobBankerType == cmd.EM_OP_BANKER_TB  then
        self._gameView:showGameTypeTB()
    end


    --显示牌型倍数
    self._gameView:showGameTypeTimesRule(self.cbCardTypeTimesRule, self.lCardTypeTime[1], self.dCellScore)

    --显示准备提示和准备换桌按钮
    self._gameView:showGamStatusAndButton(self.cbStatus)

    --初始化抢庄node
    self._gameView:initNodeGetBanker(self.cbRobBankerType, self.cbRobBankerRatio[1])

    --初始化下注node
    self._gameView:initNodeBets(self.lJettonButton[1])

    self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_FREE, self.cbFreeTime)

    self._gameView:initRoomInfo()
end

function GameLayer:onEventGameScenePlay(dataBuffer)
    -- 游戏状态
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer)
    dump(cmd_table, "onEventGameScenePlay", 7)
    
    --通用属性
    --记录各阶段操作时间
    self.cbStatus = cmd_table.statusFree.cbStatus
    self.cbFreeTime = cmd_table.statusFree.cbFreeTime
    self.cbOperateBankerTime = cmd_table.statusFree.cbOperateBankerTime
    self.cbJettonTime = cmd_table.statusFree.cbJettonTime
    self.cbOpenCardTime = cmd_table.statusFree.cbOpenCardTime
    self.cbCompareCardTime = cmd_table.statusFree.cbCompareCardTime
    self.cbGameEndTime = cmd_table.statusFree.cbGameEndTime

    self.cbRobBankerType = cmd_table.statusFree.cbRobBankerType
    self.cbRobBankerRatio = cmd_table.statusFree.cbRobBankerRatio
    self.lJettonButton = cmd_table.statusFree.lJettonButton
    self.lCardTypeTime = cmd_table.statusFree.lCardTypeTime
    self.cbCardTypeTimesRule = cmd_table.statusFree.cbCardTypeTimesRule
    self.dCellScore = cmd_table.statusFree.dCellScore

    --游戏模式
    self.cbPlayMode = cmd_table.statusFree.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1  or false
    self.bSupportTuizhu = cmd_table.statusFree.bSupportTuizhu


    --判断是否是通比模式
    if self.cbRobBankerType == cmd.EM_OP_BANKER_TB  then
        self._gameView:showGameTypeTB()
    end

    --显示牌型倍数
    self._gameView:showGameTypeTimesRule(self.cbCardTypeTimesRule, self.lCardTypeTime[1], self.dCellScore)

    --初始化抢庄node
    self._gameView:initNodeGetBanker(self.cbRobBankerType, self.cbRobBankerRatio[1])

    --初始化下注node
    self._gameView:initNodeBets(self.lJettonButton[1])

    self._gameView:initRoomInfo()


    --设置玩家游戏状态
    for i=1,cmd.GAME_PLAYER do
        if cmd_table.bUserPlaying[1][i] then
             self.m_cbPlayStatus[i] = 1 
        end
    end
    dump(self.m_cbPlayStatus, "玩家游戏状态====》", 6)

    self.cbCanTuiZhu[self:GetMeChairID() + 1] = cmd_table.bCanTuizhu

    --获取当前状态时间, 我在游戏状态才显示
    if 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] then
        self:SetGameClock(self:GetMeChairID(), self.cbStatus, self.cbFreeTime)
    else  --如果我不在游戏状态，显示换桌按钮
        self._gameView:showuserWaiting(true)
    end
   

    --显示庄家
    self.wBankerUser = cmd_table.wBankerChairID
    if self.wBankerUser ~= yl.INVALID_CHAIR then

        --设置抢庄倍数
        self._gameView:showBankerRatio(cmd_table.wRobBankerRatio[1])

        --设置庄家
        self._gameView:showBanker(cmd_table.wBankerChairID, false)
    end  


    --根据状态差异显示状态信息
    if self.cbStatus == cmd.EM_STATUS_GAME_END then
        --显示玩家手牌，牌型
        for i = 1, cmd.GAME_PLAYER do
            print("EM_STATUS_GAME_END  m_cbPlayStatus-------->", i , self.m_cbPlayStatus[i])
            if self.onSaveUser[i] and self.onSaveUser[i].wChairID ~= yl.INVALID_CHAIR and 1 == self.m_cbPlayStatus[i] then
                local viewid = self:SwitchViewChairID(i -1)
                self.cbCardData[i] = cmd_table.stUserCardInfo[1][i].cbCards[1]
                self._gameView:SetUserCard(viewid,  cmd_table.stUserCardInfo[1][i], false, 3, 3)

                --牌型
                self._gameView:SetUserCardType(viewid, cmd_table.stUserCardInfo[1][i].cbCardType)

                 --显示下注
                self._gameView:showCallInScore(viewid, cmd_table.dJettonScore[1][i], false)
            end
        end

        --浮动显示输赢分数
        self._gameView:showUserWinLoseScore(self.wBankerUser, cmd_table.dUserWinloseScore[1])
    elseif self.cbStatus == cmd.EM_STATUS_OPEN_CARD then
        --显示玩家手牌,下注
        for i = 1, cmd.GAME_PLAYER do
            if self.onSaveUser[i] and self.onSaveUser[i].wChairID ~= yl.INVALID_CHAIR and 1 == self.m_cbPlayStatus[i] then
                local viewid = self:SwitchViewChairID(i -1)
                self.cbCardData[i] = cmd_table.stUserCardInfo[1][i].cbCards[1]

                --此时第三章牌为盖着的，如果我没操作，第三章牌盖着，显示搓牌和开牌按钮
                local showNum = 3
                if 0 == cmd_table.cbOperateType[1][i] and viewid == cmd.MY_VIEWID then
                    showNum = 2

                    --根据不同状态显示界面
                    self._gameView:showGamStatusAndButton(self.cbStatus)
                end

                if self.m_bLookOn then
                    showNum = 0
                end

                self._gameView:SetUserCard(viewid,  cmd_table.stUserCardInfo[1][i], false, 3, showNum)

                 --显示下注
                self._gameView:showCallInScore(viewid, cmd_table.dJettonScore[1][i], false)
            end
        end
    elseif self.cbStatus == cmd.EM_STATUS_JETTON then

        --如果我不是庄家 并且我尚未下注
        if self.wBankerUser ~= self:GetMeChairID() and 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] and 0 == cmd_table.dJettonScore[1][self:GetMeChairID() + 1] then 
            --设置下注s
            self._gameView:setJettonButton(cmd_table.dMinScore, cmd_table.dMaxScore)
        end

           --显示玩家手牌,下注
        for i = 1, cmd.GAME_PLAYER do
            if self.onSaveUser[i] and self.onSaveUser[i].wChairID ~= yl.INVALID_CHAIR and 1 == self.m_cbPlayStatus[i] then
                local viewid = self:SwitchViewChairID(i -1)

                self.cbCardData[i] = cmd_table.stUserCardInfo[1][i].cbCards[1]
                -- self._gameView:SetUserCard(viewid,  cmd_table.stUserCardInfo[1][i], 3)
                if self.cbRobBankerType == cmd.EM_OP_BANKER_MPQZ and not self.m_bLookOn then   --名牌抢庄发两，
                    self._gameView:SetUserCard(viewid,  cmd_table.stUserCardInfo[1][i], false, 2)
                else
                    -- self._gameView:SetUserCard(viewid,  cmd_table.stUserCardInfo[1][i], false, 2, 0)
                end

                --如果是自己而且没有操作，根据状态显示操作信息
                if viewid == cmd.MY_VIEWID and 0 == cmd_table.cbOperateType[1][i] then
                    --根据不同状态显示界面
                    self._gameView:showGamStatusAndButton(self.cbStatus)
                end

                 --显示下注
                self._gameView:showCallInScore(viewid, cmd_table.dJettonScore[1][i], false)
            end
        end
    elseif self.cbStatus == cmd.EM_STATUS_ROBBANKER then
        --明牌抢庄会先发两张牌
        if self.cbRobBankerType == cmd.EM_OP_BANKER_MPQZ then
            --显示玩家手牌
            for i = 1, cmd.GAME_PLAYER do
                if self.onSaveUser[i] and self.onSaveUser[i].wChairID ~= yl.INVALID_CHAIR and 1 == self.m_cbPlayStatus[i] then
                    local viewid = self:SwitchViewChairID(i -1)
                    self.cbCardData[i] = cmd_table.stUserCardInfo[1][i].cbCards[1]

                    if not self.m_bLookOn then
                        self._gameView:SetUserCard(viewid,  cmd_table.stUserCardInfo[1][i], false, 2)
                    else
                        self._gameView:SetUserCard(viewid,  cmd_table.stUserCardInfo[1][i], false, 2, 0)
                    end

                    --如果是自己而且没有操作，根据状态显示操作信息
                    if viewid == cmd.MY_VIEWID and 0 == cmd_table.cbOperateType[1][i] then
                        --根据不同状态显示界面
                        self._gameView:showGamStatusAndButton(self.cbStatus)
                    end
                end
            end
        else
             --根据不同状态显示界面
            if 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] then
                self._gameView:showGamStatusAndButton(self.cbStatus)
            end
        end

    end
end

--显示开始游戏按钮
function GameLayer:onEventShowBegin(dwWaitTime)
    print("显示开始按钮",dwWaitTime)
    self._gameView:showSitBtn(dwWaitTime~=0 and true or false,dwWaitTime)
end

-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)
    if sub == cmd.SUB_S_GAME_START then 
        self.m_cbGameStatus = cmd.GAME_STATUS_PLAY
		self:onSubReadyBegin(dataBuffer)
	elseif sub == cmd.SUB_S_STATUS_CHANGE then 
		self:onSubStatueChange(dataBuffer)
    elseif sub == cmd.SUB_S_OPERATE_BANKER then 
		self:onSubCallBanker(dataBuffer)
    elseif sub == cmd.SUB_S_BANKER_RESULT then 
        self:onSubCallBankerResult(dataBuffer)
    elseif sub == cmd.SUB_S_JETTON_NOITFY then 
        self:onSubAddScoreNotify(dataBuffer)
    elseif sub == cmd.SUB_S_JETTON then 
        self:onSubAddScore(dataBuffer)
    elseif sub == cmd.SUB_S_SEND_CARD then 
        self:onSubSendCard(dataBuffer)
    elseif sub == cmd.SUB_S_OPEN_CARD then 
        self:onSubOpenCard(dataBuffer)
    elseif sub == cmd.SUB_S_COMPARE_CARD then 
		self:onSubCompareCard(dataBuffer)
	elseif sub == cmd.SUB_S_GAME_END then 
		self:onSubGameEnd(dataBuffer)
    elseif sub == cmd.SUB_S_ROOMCARD_RECORD then 
        self:onSubGameRecord(dataBuffer)
  
    elseif sub==cmd.SUB_S_TRUSTEE then                       --用户托管
        self:OnSubTrustee(dataBuffer)
    elseif sub==cmd.SUB_S_CHIPMODE then                       --用户托管
        self:OnSubTrusteeScore(dataBuffer)
    else
    	print("unknow gamemessage sub is"..sub)
    end
end

function GameLayer:changDeskIsVisible()
    local MyChair =self:GetMeChairID()+1
    self._gameView:setBtChangeDeskEnabled(self.cbPlayStatus[MyChair]~=1  ,self.cbPlayStatus[MyChair]~=1  )
end

--开始游戏
function GameLayer:onSubReadyBegin(dataBuffer)
    --游戏开始
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
    --设置玩家游戏状态
    for i=1,cmd.GAME_PLAYER do
        if cmd_table.bUserPlaying[1][i] then
             self.m_cbPlayStatus[i] = 1 

             if i  == self:GetMeChairID() + 1 then
                --设置换桌按钮不可用
                local btn = self._gameView.btnBg:getChildByName("btn_changeDesk")
                btn:setEnabled(false)
             end
        end
        if cmd_table.bCanTuizhu[1][i] then
            self.m_cbPlayStatus[i] = 1
        end
    end

    --显示开始动画
    self:PlaySound(cmd.RES.."sound_res/game_start.mp3")
    self._gameView:showGamBeginOrCopareBegin(1)

        -- 刷新局数
    if PriRoom and GlobalUserItem.bPrivateRoom then
        local curcount = PriRoom:getInstance().m_tabPriData.dwPlayCount
        PriRoom:getInstance().m_tabPriData.dwPlayCount = curcount + 1
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end

    --开始按钮不可见
    self._gameView.btStart:setVisible(false)

    --准备按钮不可见
    self._gameView.btReady:setVisible(false)

    self.isRobBanker = false

end

--状态变更
function GameLayer:onSubStatueChange(dataBuffer)
    --游戏开始
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusChange, dataBuffer)

    print("游戏状态变化--------->cbStatus, bStatusStart",cmd_table.cbStatus, cmd_table.bStatusStart)

    --根据状态变化改变设置操作显示
    self.cbStatus = cmd_table.cbStatus

    if cmd_table.cbStatus == cmd.EM_STATUS_ROBBANKER then
        --显示叫庄
        if cmd_table.bStatusStart and 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1]  then
            -- self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_ROBBANKER, cmd_table.cbTimeLeft)
        else
            --隐藏倒计时
            self._gameView:OnUpdataClockViewByStatus(0, 0)

            --隐藏抢庄倍数显示
            self._gameView:hideGetBankerRaito()
        end
    elseif cmd_table.cbStatus == cmd.EM_STATUS_JETTON then
              --显示下注
        if cmd_table.bStatusStart then

            --如果我是庄家
            if self.wBankerUser == self:GetMeChairID() and 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] then 
                --设置提示等待其他玩家下注
                self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_JETTON, self.cbJettonTime)
            end
        else
            --隐藏倒计时
            -- self._gameView:OnUpdataClockViewByStatus(0, 0)
        end   
    elseif cmd_table.cbStatus == cmd.EM_STATUS_OPEN_CARD then
        --显示开牌提示
        if cmd_table.bStatusStart and 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] then
            -- self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_OPEN_CARD, cmd_table.cbTimeLeft)
        else
            --隐藏倒计时
            -- self._gameView:OnUpdataClockViewByStatus(0, 0)
        end  
    elseif cmd_table.cbStatus == cmd.EM_STATUS_GAME_END then
        --显示结束
        if cmd_table.bStatusStart then
            -- self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_GAME_END, cmd_table.cbTimeLeft)
        else
            self.cbStatus = cmd.EM_STATUS_FREE
            self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_FREE, self.cbFreeTime)
            self.m_cbGameStatus = cmd.EM_STATUS_FREE

            self._gameView:showGamStatusAndButton(self.cbStatus)
        end  
    end
    if cmd_table.bStatusStart and 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] then  --如果我在游戏状态，显示状态变化和按钮
        self._gameView:showGamStatusAndButton(self.cbStatus)
    end
end

--用户叫庄
function GameLayer:onSubCallBanker(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_OperateBanker, dataBuffer)

    --设置玩家抢庄倍数
    self._gameView:showGetBankerRaito(cmd_table.wChairID, cmd_table.wRobBankerRatio)

    --设置抢庄动画
    if cmd_table.wRobBankerRatio > 0 then
        -- self._gameView:showCallBanker(cmd_table.wChairID, true)
        --存储抢庄玩家id
        table.insert(self.callBankerUser, cmd_table.wChairID)

        self:PlaySound(cmd.RES.."sound_res/qiangzhuang_nv.mp3")
    else
        table.insert(self.notCallBankerUser, cmd_table.wChairID)
    end
end

--用户叫庄结果，显示庄家按钮
function GameLayer:onSubCallBankerResult(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_OperateBankerResult, dataBuffer)
    dump(cmd_table, "CMD_S_OperateBankerResult", 7)

    self.wBankerUser = cmd_table.wBankerChairID

    local delayTime = 0
    if self.cbRobBankerType <= cmd.EM_OP_BANKER_PDZZ then
        delayTime = 2.0
    elseif self.cbRobBankerType <= cmd.EM_OP_BANKER_ZYQZ then
        delayTime = 2.0
        --先做抢庄动画
        if 1 == #self.callBankerUser then
            delayTime = 0
        else
            self._gameView:showCallBanker(self.callBankerUser, self.notCallBankerUser)
        end
    end

    --隐藏倒计时提示
    self._gameView:OnUpdataClockViewByStatus(0, 0)
    self:KillGameClock()


    self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime), cc.CallFunc:create(function ()
        self.callBankerUser = {}
        self.notCallBankerUser = {}
        --设置庄家
        self._gameView:showBanker(cmd_table.wBankerChairID, true)
    end)))
end


--判断玩家托管是否满足最大加注
function GameLayer:isCanMaxScoreTrust(type)
    --获取我的手牌
    local cardList = self.cbCardData[self:GetMeChairID() +1]
    dump(cardList, "isCanMaxScoreTrust", 7)

    local cardValue1 = yl.POKER_VALUE[cardList[1]] 
    local cardValue2 = yl.POKER_VALUE[cardList[2]] 

    if cardValue1 == cardValue2 and 3 == cardValue1 then  --豹子
        return true
    elseif cardValue1 == cardValue2 and type < 6 then --炸弹
        return true
    elseif cardValue1 >=11 and  cardValue2 >= 11 and type < 6 then --三公
        return true
    elseif math.max(cardValue1, cardValue2) >= 11 and  math.min(cardValue1, cardValue2) >= 9 and type < 4 then --公九
        return true
    elseif math.max(cardValue1, cardValue2) >= 11 and  math.min(cardValue1, cardValue2) >= 8 and type < 3 then --公九
        return true
    elseif math.max(cardValue1, cardValue2) >= 11 and  math.min(cardValue1, cardValue2) >= 7 and type < 2 then --公九
        return true
    else
        return false
    end
end

function GameLayer:addScoreTrust()
    if self.wBankerUser == self:GetMeChairID() then
        return
    end

    if 1 == self.trustConfig.BetsType then
        self:onAddScore(self.lJettonButton[1][1])
    elseif 2 == self.trustConfig.BetsType then
        self:onAddScore(self.lJettonButton[1][4])
    else
        if self:isCanMaxScoreTrust(self.trustConfig.BetsConfig) then
            self:onAddScore(self.lJettonButton[1][4])
        else
            self:onAddScore(self.lJettonButton[1][1])
        end
    end
end

--用户下注提示
function GameLayer:onSubAddScoreNotify(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_JettonNotify, dataBuffer)

    local delayTime = 0
    if self.cbRobBankerType == cmd.EM_OP_BANKER_MPQZ or self.cbRobBankerType == cmd.EM_OP_BANKER_ZYQZ then
        delayTime = 4.0
    end

    self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime), cc.CallFunc:create(function ()
        --设置下注
        self._gameView:setJettonButton(cmd_table.dMinScore, cmd_table.dMaxScore)

        --不是庄家且在游戏中，显示下注倒计时
        print("onSubAddScoreNotify", self.m_cbPlayStatus[self:GetMeChairID() + 1], self:GetMeChairID())
        if 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] then
            self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_JETTON, self.cbJettonTime)

            if self.trust and 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] then
                self:runAction(cc.Sequence:create(cc.DelayTime:create(2.0), cc.CallFunc:create(function ()
                    --显示下注
                    self:addScoreTrust()
                end))) 
            end
        end
    end)))
end

--用户下注显示
function GameLayer:onSubAddScore(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_Jetton, dataBuffer)

    --显示下注金额下注
    self._gameView:showJettonScore(cmd_table.wChairID, cmd_table.dScore)
end

--发牌
function GameLayer:onSubSendCard(dataBuffer)
    print("onSubSendCard!!!!!!!")
    self:KillGameClock()

    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_SendCard, dataBuffer)

    self.cbCardData = cmd_table.cbCards

    --当前牌张数
    self.sendCardCount = cmd_table.cbCurCount
  
    if 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] and 3 == self.sendCardCount then
        self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_OPEN_CARD, self.cbOpenCardTime)
    end


    local delayTime = 0.5
    --明牌抢庄延时发牌
    if self.cbRobBankerType == cmd.EM_OP_BANKER_MPQZ then
        delayTime = 3
    end

   --延时发牌，动画3秒
    self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime), cc.CallFunc:create(function ()
        --显示叫庄
        self._gameView:sendCardBegin()
    end))) 
end


--开牌
function GameLayer:onSubOpenCard(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_OpenCard, dataBuffer)
    -- self.m_cbGameStatus = cmd.GS_TK_PLAYING

    -- dump(self.onSaveUser[i], "onSubOpenCard", 7)


    local wChairID = cmd_table.wChairID
    if self.onSaveUser[wChairID + 1] and self.onSaveUser[wChairID + 1].wChairID ~= yl.INVALID_CHAIR then
        local viewid = self:SwitchViewChairID(wChairID)
        self.cbCardData[wChairID + 1] = cmd_table.stCards.cbCards[1]

        --只处理自己的开牌消息
        if viewid == cmd.MY_VIEWID then
            --延时一秒处理开牌
            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5), cc.CallFunc:create(function ()
                self._gameView:SetUserCard(viewid, cmd_table.stCards, true, 3)
                self._gameView:SetUserCardType(viewid, cmd_table.stCards.cbCardType)
                --隐藏倒计时
                self._gameView:OnUpdataClockViewByStatus(0, 0)
            end)))
        end
        
        dump(cmd_table.stCards, "onSubOpenCard----------->".. viewid, 7)
    end

    if wChairID == self:GetMeChairID() then
        --隐藏操作界面
        --搓牌
        local btn = self._gameView.m_csbNode:getChildByName("btn_check")
        btn:setVisible(false)
        --看牌
        local btn = self._gameView.m_csbNode:getChildByName("btn_show")
        btn:setVisible(false)
    end
end

--深度拷贝  
function GameLayer:deepcopy(object)      
    local lookup_table = {}  
    local function _copy(object)  
        if type(object) ~= "table" then  
            return object  
        elseif lookup_table[object] then  
  
            return lookup_table[object]  
        end  -- if          
        local new_table = {}  
        lookup_table[object] = new_table  
  
  
        for index, value in pairs(object) do  
            new_table[_copy(index)] = _copy(value)  
        end   
        return setmetatable(new_table, getmetatable(object))      
    end       
    return _copy(object)  
end    

--更新人物信息
function GameLayer:onSaveUsers()
    self.onSaveUser = { }
    for i = 1, cmd.GAME_PLAYER do
        self.onSaveUser[i] = self:deepcopy(self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i - 1))
    end
end

--比牌信息
function GameLayer:onSubCompareCard(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_CompareResult, dataBuffer)

    --显示牌型
    local cardInfo = cmd_table.stUserCardInfo
    for i=1,cmd.GAME_PLAYER do
        local viewid = self:SwitchViewChairID(i - 1)
        if self.onSaveUser[i] and self.onSaveUser[i].wChairID ~= yl.INVALID_CHAIR then
            local info = cmd_table.stUserCardInfo[1][i]
            self._gameView:SetUserCardType(viewid, info.cbCardType)
        end
    end
end

--游戏结束
function GameLayer:onSubGameEnd(dataBuffer)
    self._gameView:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function()  

        -- if GlobalUserItem.bPrivateGoldRoom and self._gameView._priView then
        --     self._gameView._priView:onRefreshInfo(true)
        -- end
    end)))

    self.userGold = {}
    self.endShow = false
    self.endNum = 1

    if 1 == self.m_cbPlayStatus[self:GetMeChairID() + 1] then
        self:SetGameClock(self:GetMeChairID(), cmd.EM_STATUS_GAME_END, self.cbGameEndTime)
    end
   
    if self._gameView.CuoLayer and self._gameView.CuoLayer.eventDispatcher then
        --解绑
        self._gameView.CuoLayer.eventDispatcher:removeEventListener(self._gameView._cuoCardLayer.listener2)
        self._gameView.CuoLayer.eventDispatcher:removeEventListener(self._gameView._cuoCardLayer.listener3)
        self._gameView.CuoLayer.eventDispatcher = nil
    end
    self._gameView:closeCuoLayer()
    -- self:KillGameClock()


    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer)
    dump(cmd_table,"游戏结算----》", 7)

    --判断是不是中途解散 所有数据是0
    print("----------------", cmd_table.bDismiss)
    if true == cmd_table.bDismiss then
        return
    end

    --显示玩家输赢
    local bankerChairID = cmd_table.wBankerChairID

    --浮动显示输赢分数
    self._gameView:showUserWinLoseScore(bankerChairID, cmd_table.dUserWinloseScore[1])

    --显示牌型
    local cardInfo = cmd_table.stUserCardInfo
    for i=1,cmd.GAME_PLAYER do
        local viewid = self:SwitchViewChairID(i - 1)
        if self.onSaveUser[i] and self.onSaveUser[i].wChairID ~= yl.INVALID_CHAIR and 1 == self.m_cbPlayStatus[i] then
            local info = cmd_table.stUserCardInfo[1][i]
            self._gameView:SetUserCard(viewid, cmd_table.stUserCardInfo[1][i], true, 3)
            self._gameView:SetUserCardType(viewid, info.cbCardType)
        end
    end

    --金币动画
    local bankerViewID = self:SwitchViewChairID(bankerChairID)
    self._gameView:showGoldAction(cmd_table.dUserWinloseScore[1], bankerViewID)

    --判断通杀通赔
    local bIsTongSha = true
    local bIsTongPei = true
    for i=1, cmd.GAME_PLAYER do
        if cmd_table.dUserWinloseScore[1][i] > 0 and i ~= bankerChairID + 1 then
            bIsTongSha = false
        end
        if cmd_table.dUserWinloseScore[1][i] < 0 and i ~= bankerChairID + 1 then
            bIsTongPei = false
        end
    end

    print("GameEnd----通杀通赔", bIsTongSha, bIsTongPei)
    --通杀通赔,需要人数大于2
    if (bIsTongSha or bIsTongPei) and self:getPlayingNum() > 2 then
        self:runAction(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function ()
            --显示通杀通赔
            if bIsTongSha then
                self:PlaySound(cmd.RES.."sound_res/banker_win_all.mp3")
                self._gameView:showGamResultAction(1)
            elseif bIsTongPei then
                self:PlaySound(cmd.RES.."sound_res/banker_lose_all.mp3")
                self._gameView:showGamResultAction(2)
            end
        end))) 
    end

    

    -- if GlobalUserItem.bPrivateGoldRoom then
    --     if self.cbPlayStatus and self.cbPlayStatus[self.m_wCurrentUser + 1] then
    --         if self.cbPlayStatus[self.m_wCurrentUser + 1] == 0 then
    --             self.m_cbGameStatus = cmd.GS_TK_FREE
    --             self._gameView.d:setVisible(true)
    --             self.cbPlayStatus[self.m_wCurrentUser + 1] = 1
    --             -- self.cbPlayStatus = nil

    --             if self._gameView._priView then
    --                 self._gameView._priView:onRefreshInfo(true)
    --             end

    --             return
    --         end
    --     end
    -- end
end

function GameLayer:onSubGameRecord(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_RECORD, dataBuffer)
    -- dump(cmd_table,"========================222222222")
    --全部成绩
    self.m_userPriRecord = {}
    local scoreList = {}
    local userList = {}
    local nInningsCount = cmd_table.nCount

    for i = 1, cmd.GAME_PLAYER do
        --游戏状态
         local item = self:getUserInfoByChairID(i-1)
                    table.insert(userList,item)
    end
    --dump(self.cbPlayStatus,"========================222222222")

    for i=1,nInningsCount do
        self.m_userPriRecord[i] ={}
        for j=1,cmd.GAME_PLAYER do 
             self.m_userPriRecord[i][j]=cmd_table.lDetailScore[j][i]
        end
    end
    -- dump(self.m_userPriRecord,"=========================1111111111111")
    self._gameView:updataScoreList()
end




function GameLayer:OnFlushCardFinish()
    local nodeCard = self._gameView.userCard[self:SwitchViewChairID(self.m_wLostUser)]
    self._gameView:StopCompareCard()
    if myChair == self.m_wWinnerUser then
        self:PlaySound(cmd.RES .. "sound_res/COMPARE_WIN.wav")
    elseif myChair == self.m_wLostUser then
        self:PlaySound(cmd.RES .. "sound_res/COMPARE_LOSE.wav")
    end
    self.compareFun = cc.Director:getInstance():getScheduler():scheduleScriptFunc( function()
        self:compare()
    end , 0, false)
end


--更新秘钥
function GameLayer:onSubUpdateAesKey(dataBuffer)
    for i = 1, 16 do
        self.chUserAESKey[i] = dataBuffer:readbyte()
    end
end

--发送开始
function GameLayer:onGameBegin(bBegin)
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_GameBegin)
    cmd_data:pushbool(bBegin)
    self:SendData(cmd.SUB_C_GAME_BEGIN, cmd_data)
end

-- 发送准备
function GameLayer:sendReady()
    self:KillGameClock()
    self._gameFrame:SendUserReady()
    self.m_bStartGame = true
end


--发送抢庄
function GameLayer:onGetBanker(ratio)
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_OperateBanker)
    cmd_data:pushword(ratio)
    self:SendData(cmd.SUB_C_OPERATE_BANKER, cmd_data)

    print("***********************btCallBanker**false********************")

    --隐藏加注界面
    self.isRobBanker = true
    self._gameView.nodeGetBanker:setVisible(false)

    --隐藏倒计时提示
    -- self._gameView:OnUpdataClockViewByStatus(0, 0)
    -- self:KillGameClock()
end

--发送下注
function GameLayer:onAddScore(score)
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_Jetton)
    cmd_data:pushscore(score)
    self:SendData(cmd.SUB_C_ADD_SCORE, cmd_data)

    --隐藏下注界面
    self._gameView.nodeBets:setVisible(false)

    --隐藏倒计时提示
    self._gameView:OnUpdataClockViewByStatus(0, 0)

    self:KillGameClock()
    self:PlaySound(cmd.RES .. "sound_res/CHOOSE_SCORE.wav")
end

--搓牌
function GameLayer:onCuoCard()
    local i = self:GetMeChairID()
    if self.onSaveUser[i + 1] and self.onSaveUser[i + 1].wChairID ~= yl.INVALID_CHAIR then
        self._gameView:SetUserCuoCard(self.cbCardData[i + 1][3])
    end
end

--发送开牌
function GameLayer:onOpenCard()
    print("###############onOpenCard#############")
    local cmd_data = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_OPEN_CARD, cmd_data)
    self:KillGameClock()

    --搓牌
    local btn = self._gameView.m_csbNode:getChildByName("btn_check")
    btn:setVisible(false)
    --看牌
    local btn = self._gameView.m_csbNode:getChildByName("btn_show")
    btn:setVisible(false)
end


--换位操作
function GameLayer:onChangeDesk()
    self._gameFrame:QueryChangeDesk()
end

function GameLayer:onUserChat(chatinfo,sendchair)
    if chatinfo and sendchair then
        local viewid = self:SwitchViewChairID(sendchair)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:ShowUserChat(viewid, chatinfo.szChatString)
        end
    end
end

function GameLayer:onUserExpression(expression,sendchair)
    if expression and sendchair then
        local viewid = self:SwitchViewChairID(sendchair)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:ShowUserExpression(viewid, expression.wItemIndex)
        end
    end
end

-- 语音播放开始
function GameLayer:onUserVoiceStart( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self._gameView:ShowUserVoice(viewid, true)
    end
end

-- 语音播放结束
function GameLayer:onUserVoiceEnded( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self._gameView:ShowUserVoice(viewid, false)
    end
end

-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end

--获得加密Key
function GameLayer:getAesKey()
    --将数组转成字符串
    local strInput = ""
    for i = 1, #self.chUserAESKey do
        strInput = strInput .. string.format("%d,", self.chUserAESKey[i])
     end
     --加密
    local result = AesCipher(strInput)
    --将字符串转成数组
    local resultKey = {}
    local k = 1
    local num = 0
    for i = 1, string.len(result) do
        if string.sub(result, i, i) ~= ',' then
            local bt = string.byte(result, i) - string.byte("0")
            num = num*10 + bt
        else
            resultKey[k] = num
            k = k + 1
            num = 0
        end
    end

    return resultKey
end

function GameLayer:onThrowJetton(i,score)
    local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i - 1)
    local wViewChairId = self:SwitchViewChairID(i - 1)
    self._gameView:OnUpdateUser(wViewChairId, userItem)
    self.m_lAllTableScore = self.m_lAllTableScore + score
    self.m_lTableScore[i] = score
    -- 用户下注
    self._gameView:SetUserTableScore(wViewChairId, self.m_lTableScore[i])
    -- 移动筹码
--    self._gameView:PlayerJetton(wViewChairId, self.m_lTableScore[i])
end

-------------------------------------------------------新大厅增加函数--------------------------------------------
--游戏回放（房卡）
function GameLayer:onSubRecord(dataBuffer)
    print("onSubRecord----------------------", dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_RECORD, dataBuffer)
    local scoreList = {}
    local nInningsCount = PriRoom:getInstance().m_tabPriData.dwPlayCount--cmd_data.nCount[1][self:GetMeChairID() + 1]
    for j = 1, nInningsCount do
        local DetailScore = {}
        for i = 1, cmd.GAME_PLAYER do
            DetailScore[i] = cmd_data.lDetailScore[i][j]
        end
        table.insert(scoreList, DetailScore)
    end
    self._gameView:updataScoreList(scoreList)
end

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode("privateroom/game/ShareLayer.csb"):addTo(self)
            local TAG_WXSHARE = 101
            local TAG_CYCLESHARE = 102
            local MASK_PANEL = 103
            -- 按钮事件
            local touchFunC = function(ref, tType)
                if tType == ccui.TouchEventType.ended then
                    local tag = ref:getTag()
                    local target = nil
                    if TAG_WXSHARE == tag then
                        target = yl.ThirdParty.WECHAT
                    elseif TAG_CYCLESHARE == tag then
                        target = yl.ThirdParty.WECHAT_CIRCLE
                    elseif MASK_PANEL == tag then
                        
                    end
                    if nil ~= target then
                        MultiPlatform:getInstance():shareToTarget(
                            target, 
                            function(isok)

                            end,
                            "截图分享", 
                            "分享我的游戏截图", 
                            yl.HTTP_URL, 
                            imagepath, 
                            "true"
                        )
                    end
                    shareLayer:removeFromParent()
                end
            end
            -- 微信按钮
            local btn = shareLayer:getChildByName("btn_wxshare")
            btn:setTag(TAG_WXSHARE)
            btn:addTouchEventListener( touchFunC )
            -- 朋友圈按钮
            btn = shareLayer:getChildByName("btn_cycleshare")
            btn:setTag(TAG_CYCLESHARE)
            btn:addTouchEventListener( touchFunC )
            -- 屏蔽层
            local panel = shareLayer:getChildByName("share_panel")
            panel:setTag(MASK_PANEL)
            panel:addTouchEventListener( touchFunC )
        else
            print("no image")
        end
    end
end

--回调聊天记录
function GameLayer:addChatRecordWithGame(cmdtable, bBrow)
    self._gameView.m_GameChat:addChatRecordWith(cmdtable, bBrow)
end

--检查能否开始游戏
function GameLayer:checkReady()
    if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom and self:GetMeUserItem().dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID and PriRoom:getInstance().m_tabPriData.dwPlayCount == 0 then
        self:onSaveUsers()
        local checkFlag = true
        for i = 1, cmd.GAME_PLAYER do
            if self.onSaveUser[i] and self.onSaveUser[i].wChairID ~= yl.INVALID_CHAIR then
                if self.onSaveUser[i].dwUserID ~= PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID and self.onSaveUser[i].cbUserStatus ~= yl.US_READY then
                    checkFlag = false
                end
            end
        end
        self._gameView.btReady:setVisible(false)
        self._gameView.btGameBegin:setVisible(true)
        if checkFlag and #self.onSaveUser > 1 then
            self._gameView.btGameBegin:setEnabled(true)
            self._gameView.btGameBegin:loadTextures(cmd.GAME .. "btn_gamebegin_0.png", cmd.GAME .. "btn_gamebegin_1.png", cmd.GAME .. "btn_gamebegin_0.png", ccui.TextureResType.plistType)
        else
            self._gameView.btGameBegin:setEnabled(false)
            self._gameView.btGameBegin:loadTextures(cmd.GAME .. "btn_gamebegin_2.png", cmd.GAME .. "btn_gamebegin_2.png", cmd.GAME .. "btn_gamebegin_2.png", ccui.TextureResType.plistType)
        end
        if self.m_cbGameStatus ~= cmd.GS_TK_FREE then
            self._gameView.btGameBegin:setVisible(false)
        end
    end
    if GlobalUserItem.bPrivateGoldRoom then
--        self._gameView.m_RoomHolderFlag:setVisible(false)
--        if self.m_cbGameStatus == cmd.GS_TK_FREE then
--            self._gameView.btGameBegin:setVisible(false)
--            self._gameView.btReady:setVisible(true)
--        end
--        if self.m_bStartGame then
--            self._gameView.btGameBegin:setVisible(false)
--            self._gameView.btReady:setVisible(false)
--        end
    end
end

function GameLayer:getGameRule(buffer)

        local strTable ={}

        --庄家
        local bankerMode =  buffer[4]
        local bankerType = {"霸王庄", "轮流坐庄","牌大坐庄","明牌抢庄","自由抢庄","通比模式"}
        local szbankerMode= bankerType[bankerMode]
        table.insert(strTable,szbankerMode)

        --翻倍规则
        local betRule = buffer[5]
        local betType = {"无翻倍", "通用倍数","几点几倍"}
        local szbetMode= betType[betRule + 1]
        table.insert(strTable,szbetMode)

         --最大抢庄倍数
        local maxBankerRatio = ExternalFun.bytesToInt(buffer, 6, 13);
        local szMaxBankerRatio = "最大抢庄" .. maxBankerRatio .. "倍"
        if bankerMode == 4 then  --通比模式显示
            table.insert(strTable,szMaxBankerRatio)
        end

        --封顶分数
        local maxScore = ExternalFun.bytesToInt(buffer, 14, 21);
        local szmaxScore = "封顶倍数" .. maxScore 
        if bankerMode == 6 then  --通比模式显示
            table.insert(strTable,szmaxScore)
        end
    return strTable
end

--用户托管
function GameLayer:OnSubTrustee(dataBuffer, wDataSize)
    --消息处理
    local pTrustee=ExternalFun.read_netdata(cmd.CMD_S_Trustee, dataBuffer)
    

    if pTrustee.wTrusteeUser==self:GetMeChairID() then
        self.trust = pTrustee.bTrustee
        self._gameView:changeAutoImg()
    end
end

--托管模式
function GameLayer:OnSubTrusteeScore(dataBuffer, wDataSize)
    --消息处理
    local cmd_table=ExternalFun.read_netdata(cmd.CMD_S_ChipMode, dataBuffer)
    self._gameView.JETTON_MODE = cmd_table.cbTrusteeMode
    self._gameView:showJettonNum()
end

--发送托管消息
function GameLayer:sendTrustee(flag,mode)
    flag = flag ~= nil and flag or false
    self.trust = flag
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_Trustee)
    cmd_data:pushbool(flag)
    -- cmd_data:pushbyte(mode)
    return self:SendData(cmd.SUB_C_TRUSTEE, cmd_data)
end

function GameLayer:sendTrusteeJetton(score)
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_ChipMode)
    cmd_data:pushbyte(score)
    -- cmd_data:pushbyte(mode)
    return self:SendData(cmd.SUB_C_CHIPMODE, cmd_data)
end

function GameLayer:getVideoLayerConfig()
    return 
    {
        pos = cc.p(667, 90),
        wChairCount = self._gameFrame._wChairCount,
        maskOpacity = 10
    }
end


function GameLayer:onPlayingUserInfo(userInfo)

    self.m_playingUser = {}
    for i=1,16 do
        self.m_playingUser[i] = userInfo.dwUserID[1][i]
    end
end

--退出询问
function GameLayer:onQueryExitGame()

    local bNoGame = false --没有参与游戏


    for i=1,#self.m_playingUser do
        if self.m_playingUser[i] == GlobalUserItem.tabAccountInfo.dwUserID  then
            bNoGame = false
            break
        elseif i == #self.m_playingUser then
            bNoGame = true
        end
    end

    if self.m_bLookOn  or bNoGame then

            self:onExitTable()
            return
    end


    if PriRoom and true == GlobalUserItem.bPrivateRoom and (false == GlobalUserItem.bPrivateGoldRoom or GlobalUserItem.dwCurRoomGroupID ~= 0) then  --金币房卡可以直接退出
        PriRoom:getInstance():queryQuitGame(self.m_cbGameStatus)
    else

        if self._queryDialog then
           return
        end
        
        local tip = (self.m_cbGameStatus > 0 and self.cbPlayStatus[self:GetMeChairID() + 1] == 1) and "现在退出游戏会由系统托管代打哟，输了可不能怪他哦！" or "您要退出游戏么？"
       
        self._queryDialog = QueryDialog:create(tip, function(ok)
            if ok == true then
                --退出防作弊
                self._gameFrame:setEnterAntiCheatRoom(false)
                if  self.m_cbGameStatus > 0 then

                    self._gameFrame:onCloseSocket()
                    self:dismissPopWait()
                    self:stopAllActions()
                    self:KillGameClock()
                    self._scene:onKeyBack()
                    return
                end
                
                self:onExitTable()
            end
            self._queryDialog = nil
        end):setCanTouchOutside(false)
            :addTo(self)
    end
end

return GameLayer