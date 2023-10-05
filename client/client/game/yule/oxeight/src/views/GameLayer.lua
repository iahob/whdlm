local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")

local GameLayer = class("GameLayer", GameModel)
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local cmd = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.CMD_Game")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.GameLogic")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.views.layer.GameViewLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
-- 初始化界面
function GameLayer:ctor(frameEngine,scene)
    GameLayer.super.ctor(self, frameEngine, scene)

end

function GameLayer:getParentNode()
    return self._scene
end
--创建场景
function GameLayer:CreateView()
    return GameViewLayer:create(self):addTo(self)
end

-- 初始化游戏数据
function GameLayer:OnInitGameEngine()
    --游戏规则 2-6人为0，其余为人数值
    self.wPlayerCountRule = yl.INVALID_WORD                         
    self.cbPlayStatus = {0, 0, 0, 0,0,0,0,0} --游戏状态
    self.lGameEndScore = {0,0,0,0,0,0,0,0} --游戏结束分数
    self.lFreeConif = {0, 0, 0, 0} --积分
    self.lPercentConfig = {0, 0, 0, 0} --百分比
    self.lCardType = {}         --牌型
    self.cbCardData = {}
    self.wBankerUser = yl.INVALID_CHAIR
    self.cbDynamicJoin = 0
    self.m_tabPrivateRoomConfig = {}
    self.m_lAddScoreList = {}
    self.m_bStartGame = false
    self.isPriOver = false
    self.bAddScore = false
    self.m_lMaxTurnScore = 0
    self.isCanCuo = false
    self.m_maxBet = 0
    self.isFirstTurn = false
    self.cbLastCardData = nil

    self.m_tabUserItem = {}
    self.m_callBanker ={0,0,0,0,0,0,0,0}
    self.isPlayMessage = false

    --约战
    self.m_userRecord = {}   --用户记录

    self.m_bLookOn = false

    self.m_playingUser = {}

        --房卡模式
    self.cbPlayMode = 0  --默认积分模式

    self._MyChairID = self:GetMeChairID()

    GameLayer.super.OnInitGameEngine(self)
end

-- 重置游戏数据
function GameLayer:OnResetGameEngine()

    GameLayer.super.OnResetGameEngine(self)
    self._gameView:onResetView()

    self.m_lMaxTurnScore = 0

    --print("重置游戏数据")
    --游戏规则 2-6人为0，其余为人数值
    self.wPlayerCountRule = yl.INVALID_WORD        
    self.cbPlayStatus = {0, 0, 0, 0 ,0 ,0,0,0} --游戏状态
    self.lFreeConif = {0, 0, 0, 0} --积分
    self.lPercentConfig = {0, 0, 0, 0} --百分比
    self.lCardType = {}
    self.m_lAddScoreList = {}
    self.m_enterMessage = cmd.GS_TK_FREE
    self.cbLastCardData = nil

    --self.cbCardData = {}
    self.wBankerUser = yl.INVALID_CHAIR
    self.cbDynamicJoin = 0
    self.m_tabPrivateRoomConfig = {}
    self.m_bStartGame = false
    self.isPriOver = false
    self.bAddScore = false
    self.m_bLookOn = false

    self.m_SendCardStatus = 0  -- 发牌状态，配牌器使用
    self.bDelaySendFour = false

    for i=1,cmd.GAME_PLAYER do
         if self._gameView.m_tabUserHead[i] then
                
             self._gameView:setReadyVisible(i, false)
             self._gameView.m_tabUserHead[i]:removeFromParent()
             self._gameView.m_tabUserHead[i]= nil
         end
    end

end

-- 椅子号转视图位置,注意椅子号从0~nChairCount-1,返回的视图位置从1~nChairCount
function GameLayer:SwitchViewChairID(chair)
    local viewid = yl.INVALID_CHAIR
    local nChairCount = cmd.GAME_PLAYER
    local meChairID = self:GetMeChairID()
    if meChairID == yl.INVALID_CHAIR then
        meChairID = self._MyChairID
    else
        self._MyChairID = meChairID 
    end
    if chair ~= yl.INVALID_CHAIR and chair < nChairCount then
        local cha = meChairID - chair
                if math.abs(cha) == 0 then
                    viewid = 3
                elseif math.abs(cha) == 1 then
                    viewid = cha > 0 and 2 or 4
                elseif math.abs(cha) == 2 then
                    viewid = cha > 0 and 1 or 5
                elseif math.abs(cha) == 3 then
                    viewid = cha > 0 and 8 or 6
                elseif math.abs(cha) == 4 then
                    viewid = 7
                elseif math.abs(cha) == 5 then
                    viewid = cha > 0 and 6 or 8
                elseif math.abs(cha) == 6 then
                    viewid = cha > 0 and 5 or 1
                elseif math.abs(cha) == 7 then
                    viewid = cha > 0 and 4 or 2

                end
    end
    return viewid
end

--将视图id转换为普通id
function GameLayer:SwitchChairID( viewid )
    if viewid < 1 or viewid >cmd.GAME_PLAYER then
        error("this is error viewid")
    end
    for i=1,cmd.GAME_PLAYER do
        if self:SwitchViewChairID(i-1) == viewid then
            return i
        end
    end
end

------继承函数
function GameLayer:getVideoLayerConfig()
    return 
    {
        pos = cc.p(667, 220),
        wChairCount = self._gameFrame._wChairCount,
        maskOpacity = 50
    }
end

--是否正在玩
function GameLayer:isPlayerPlaying(viewId)
    if viewId < 1 or viewId > cmd.GAME_PLAYER then
        --print("view chair id error!")
        return false
    end
    for i = 1, cmd.GAME_PLAYER do
        if self:SwitchViewChairID(i - 1) == viewId then
            if self.cbPlayStatus[i] == 1 then
                return true
            end
        end
    end

    return false
end

--获取gamekind
function GameLayer:getGameKind()
    return cmd.KIND_ID
end

-- 时钟处理
function GameLayer:OnEventGameClockInfo(chair,time,clockId)

    self._gameView:logicClockInfo(chair,time,clockId)
   
end

function GameLayer:KillGameClockOnStart()
    self._gameView._csbNode:getChildByName("clock_node"):setVisible(false)
    self:KillGameClock()
end


--用户聊天
function GameLayer:onUserChat(chat, wChairId)
    --print("玩家聊天", chat.szChatString)
    self._gameView:onUserChat(chat, self:SwitchViewChairID(wChairId))
end

--用户表情
function GameLayer:onUserExpression(expression, wChairId)
    self._gameView:onUserExpression(expression, self:SwitchViewChairID(wChairId))
end

-- 语音播放开始
function GameLayer:onUserVoiceStart( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        --print("语音播放开始 viewid",viewid)
        self._gameView:ShowUserVoice(viewid, true)
    end
end


--显示开始游戏按钮
function GameLayer:onEventShowBegin(dwWaitTime)
    self._gameView:showSitBtn(dwWaitTime ~= 0 and true or false,dwWaitTime)
end


function GameLayer:onPlayingUserInfo(userInfo)

    self.m_playingUser = {}
    for i=1,16 do
        self.m_playingUser[i] = userInfo.dwUserID[1][i]
    end
end


-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end

-- 语音播放结束
function GameLayer:onUserVoiceEnded( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        --print("语音播放结束,viewid",viewid)
        self._gameView:ShowUserVoice(viewid, false)
    end
end


--退出桌子
function GameLayer:onExitTable()
   
    self:KillGameClock()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE  then
        self:showPopWait()
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(
                function () 
                    self._gameFrame:StandUp(1)
                end
                ),
            cc.DelayTime:create(10),
            cc.CallFunc:create(
                function ()
                    --print("delay leave")
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
    print("======================离开房间==================")
    self._scene:onKeyBack()
end

--退出
function GameLayer:onExit()
    print("======================退出==================")
    self:KillGameClock()
    self:dismissPopWait()
    GameLayer.super.onExit(self)
end

--坐下人数
function GameLayer:onGetSitUserNum()
    local num = 0
    for i = 1, cmd.GAME_PLAYER do
        if nil ~= self.m_tabUserItem[i] then
            num = num + 1
        end
    end
    return num
end

--[[function GameLayer:getUserInfoByChairID(chairId)
    local viewId = self:SwitchViewChairID(chairId)
    return self.m_tabUserItem[viewId]
end]]

function GameLayer:onGetNoticeReady()
    --print("牛牛 系统通知准备")
    if nil ~= self._gameView and nil ~= self._gameView.btStart then
        --self._gameView.btStart:setVisible(true)
    end
end

--系统消息
function GameLayer:onSystemMessage( wType,szString )
    --print("处理游戏币不足")
    if self.m_bStartGame or GlobalUserItem.dwCurRoomGroupID ~= 0 then
        local msg = szString or ""
        self.m_querydialog = QueryDialog:create(msg,function()
            if not self.m_bClubRoom then 
                    self:onExitTable()
            end 
        end,nil,1)
        self.m_querydialog:setCanTouchOutside(false)
        self.m_querydialog:addTo(self)
    else
        self.m_bPriScoreLow = true
        self.m_szScoreMsg = szString
    end
end

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode(cmd.RES_PATH.."game/ShareLayer.csb"):addTo(self)
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


function GameLayer:addPrivateGameLayer( layer )
    if nil == layer then
        return
    end
    self._gameView:addChild(layer, 2)
end

-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus, dataBuffer)
    print("场景信息==============================================")

    local myUserItem = self:GetMeUserItem()
    if myUserItem.cbUserStatus ~= yl.US_LOOKON then 
        self._gameView:OnUpdateUser(cmd.MY_VIEWID, self:GetMeUserItem(),true)

        self.m_tabUserItem[self:GetMeChairID()+1] = clone(self:GetMeUserItem())
    else
        self.m_bLookOn = true
    end

    print("************是否旁观",self.m_bLookOn)
    --初始化已有玩家   
    local tableId = self._gameFrame:GetTableID()
    for i = 1, cmd.GAME_PLAYER do
        --if i ~= self:GetMeChairID()+1 then
            -- self._gameFrame:QueryUserInfo(tableId, i-1)
        --end
        local item = self._gameFrame:getTableUserItem(tableId, i-1)
        if item then
            self._gameView:OnUpdateUser(self:SwitchViewChairID(i - 1), item,true)
            self.m_tabUserItem[item.wChairID+1] = clone(item)
        end
    end

    -- self._gameView:setStartBtnShow(false)
    self._gameView:setBtnchangeShow(true,false)
   
    self.m_cbGameStatus = cbGameStatus
    self._gameView:setClockBgVisible(false)
    self._gameView:resetAutoSet()
    self.m_enterMessage = cbGameStatus

	if cbGameStatus == cmd.GS_TK_FREE	then				--空闲状态
        print("空闲状态")
                self:onSceneFree(dataBuffer)
	elseif cbGameStatus == cmd.GS_TK_CALL	then			--叫分状态
        print("叫分状态")
                self:onSceneCall(dataBuffer)
	elseif cbGameStatus == cmd.GS_TK_SCORE	then			--下注状态
        print("下注状态")
                self:onSceneScore(dataBuffer)
    elseif cbGameStatus == cmd.GS_TK_PLAYING  then           --游戏状态
        print("游戏状态")
                self:onScenePlaying(dataBuffer)
    elseif cbGameStatus == cmd.GS_TK_WAITEND  then           --游戏状态
        print("等待结束状态")
        self.m_cbGameStatus = cmd.GS_TK_PLAYING
                self:onScenePlaying(dataBuffer)
	end
    self:dismissPopWait()

    if self.cbPlayMode == 1 then
        for i = 1, cmd.GAME_PLAYER do
            if self._gameView.m_tabUserHead[i] then
                self._gameView.m_tabUserHead[i]:showRoomHolder(false)
            end
        end
    end
    if self._gameView._listView then
        self._gameView._listView:reloadData()
    end
    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
        self:onRecord()
    end

    if GlobalUserItem.dwCurRoomGroupID ~= 0 and GlobalUserItem.bPrivateGoldRoom then 
        self.m_bClubRoom = true
    end


    self._gameView:showSitBtn(self.m_bLookOn)


    --self._gameView:setBankerWaitIcon(cmd.MY_VIEWID,false,true,true)
    --self._gameView:showCuoLayer()
end

--空闲场景
function GameLayer:onSceneFree(dataBuffer)
    --print("onSceneFree")
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer);
    
    dump(cmd_table,"=============空闲场景",6)
    
    cmd.TIME_USER_CALL_BANKER       = cmd_table.nTimeCallBanker
    --开始定时器
    cmd.TIME_USER_START_GAME        = cmd_table.nTimeStart
    --加注定时器
    cmd.TIME_USER_ADD_SCORE         = cmd_table.nTimeAddScore
    --摊牌定时器
    cmd.TIME_USER_OPEN_CARD         = cmd_table.nTimeOpenCard

    self._cardTypeTimes = cmd_table.wCardTypeTimes[1]
    self.lCellScore = cmd_table.lCellScore
    local lRoomStorageStart = cmd_table.lRoomStorageStart
    local lRoomStorageCurrent = cmd_table.lRoomStorageCurrent

    local lTurnScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lTurnScore[i] = cmd_table.lTurnScore[1][i]
    end

    local lCollectScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lCollectScore[i] = cmd_table.lCollectScore[1][i]
    end
        --游戏模式
    self.cbPlayMode = cmd_table.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1  or false

    -- 反作弊标识
    local bIsAllowAvertCheat = cmd_table.bIsAllowAvertCheat
    --游戏牌型
    self.m_tabPrivateRoomConfig.cardType = cmd_table.ctConfig
    --发牌模式
    self.m_tabPrivateRoomConfig.sendCardType = cmd_table.stConfig
    -- 坐庄模式
    self.m_tabPrivateRoomConfig.bankGameType = cmd_table.bgtConfig
    --下注模式
    self.m_tabPrivateRoomConfig.betType = cmd_table.btConfig
    
    self.m_tabPrivateRoomConfig.gtType = cmd_table.gtConfig

    -- 房卡积分模式
    self._gameView:showRoomRule(self.m_tabPrivateRoomConfig)

    self.wPlayerCountRule = cmd_table.wPlayerCountRule

    self.isCanCuo = cmd_table.cbAdmitRevCard == 1 and true  or false 

    -- self._gameView.btAuto:setVisible(false)
    
    if not GlobalUserItem.isAntiCheat() then
        local useritem = self:GetMeUserItem()

        if useritem.cbUserStatus == yl.US_SIT and not self.m_bLookOn then
            -- self._gameView:setStartBtnShow(true)
            print("777777777777777777")
            self._gameView:setBtnchangeShow(true,true)
        end

        if useritem.cbUserStatus > yl.US_SIT then
            return
        end
        
        -- 私人房无倒计时
        if not GlobalUserItem.bPrivateRoom or (GlobalUserItem.bPrivateRoom and PriRoom:getInstance().m_tabPriData.dwPlayCount ~= 0) or (GlobalUserItem.bPrivateRoom and self.cbPlayMode == 1) then
            print("999999999999999999")
            -- 设置倒计时
            self:KillGameClock()
            self._gameView.nAutoStatus = cmd.AUTO_PLAY_START

            self:SetGameClock(self:GetMeChairID(), cmd.IDI_START_GAME, cmd.TIME_USER_START_GAME)
            self._gameView:setClockBgVisible(true)
            -- self._gameView.btAuto:setVisible(true)
        else
            self._gameView:setClockBgVisible(false)
        end
    end
end
--叫庄场景
function GameLayer:onSceneCall(dataBuffer)

    --移除所有子类
    self._gameView.nodeCallMul:removeAllChildren()

    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusCall, dataBuffer);
    --dump(cmd_table)
    dump(cmd_table,"=============叫庄场景",6)

    cmd.TIME_USER_CALL_BANKER       = cmd_table.nTimeCallBanker
    --开始定时器
    cmd.TIME_USER_START_GAME        = cmd_table.nTimeStart
    --加注定时器
    cmd.TIME_USER_ADD_SCORE         = cmd_table.nTimeAddScore
    --摊牌定时器
    cmd.TIME_USER_OPEN_CARD         = cmd_table.nTimeOpenCard

    self._cardTypeTimes = cmd_table.wCardTypeTimes[1]
    self.lCellScore = cmd_table.lCellScore
    local wCallBanker = cmd_table.wCallBanker
    self.cbDynamicJoin = cmd_table.cbDynamicJoin
    self.isPlayMessage = true
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~动态加入：", self.cbDynamicJoin)
    for i = 1, cmd.GAME_PLAYER do
        --游戏状态
        self.cbPlayStatus[i] = cmd_table.cbPlayStatus[1][i]
    end
    local lRoomStorageStart = cmd_table.lRoomStorageStart
    local lRoomStorageCurrent = cmd_table.lRoomStorageCurrent

    local lTurnScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lTurnScore[i] = cmd_table.lTurnScore[1][i]
    end
    local lCollectScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lCollectScore[i] = cmd_table.lCollectScore[1][i]
    end

        --游戏模式
    self.cbPlayMode = cmd_table.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1 or false

    -- 反作弊标识
    local bIsAllowAvertCheat = cmd_table.bIsAllowAvertCheat
    --叫庄状态
    local cbCallBankerStatus = {}
    for i = 1, cmd.GAME_PLAYER do
        cbCallBankerStatus[i] = cmd_table.cbCallBankerStatus[1][i]
    end
    --叫庄倍数
    local cbCallBankerTimes = {}
    for i = 1, cmd.GAME_PLAYER do
        cbCallBankerTimes[i] = cmd_table.cbCallBankerTimes[1][i]
        if self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER then
            cbCallBankerTimes[i] = 1
        end
    end
    self._cbCallBankerTimes = cbCallBankerTimes

    --游戏牌型
    self.m_tabPrivateRoomConfig.cardType = cmd_table.ctConfig
    --发牌模式
    self.m_tabPrivateRoomConfig.sendCardType = cmd_table.stConfig
    -- 坐庄模式
    self.m_tabPrivateRoomConfig.bankGameType = cmd_table.bgtConfig
    -- 下注模式
    self.m_tabPrivateRoomConfig.betType = cmd_table.btConfig

    self.isCanCuo = cmd_table.cbAdmitRevCard == 1 and true  or false 

    self.m_tabPrivateRoomConfig.gtType = cmd_table.gtConfig

    self._gameView:showRoomRule(self.m_tabPrivateRoomConfig)

    if self.m_bLookOn then
            self._gameView:setClockBgVisible(true)
            self:KillGameClock()
            self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd_table.cbTimeRemain) 
            self._gameView:setBtnchangeShow(false,false)
            return
    end

    self._gameView.nAutoStatus = cmd.AUTO_PLAY_FREE

    if cmd_table.wNewTurnChairId ~= yl.INVALID_CHAIR then

        self.wBankerUser = cmd_table.wNewTurnChairId

        self._gameView:setBankerUser(self:SwitchViewChairID(self.wBankerUser))


        if cmd_table.wNewTurnChairId == self:GetMeChairID() then

           if self.cbDynamicJoin == 0 then 
                    self._gameView:setClockBgVisible(true)
                    self:KillGameClock()
                    self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd_table.cbTimeRemain) 

                    self._gameView.nAutoStatus = cmd.AUTO_PLAY_CALL
                    self._gameView.bLastCall =self._gameView.TAG_ENUM.BT_MUL1
            else
                    self._gameView:showWaitOtherTips()
                    self._gameView:setBtnchangeShow(true,false)
            end

            self._gameView:gameCallBanker(cmd.MY_VIEWID, bFirstTimes,cmd_table.cbMaxCallBankerTimes,true)
        else
            self._gameView.nAutoStatus = cmd.AUTO_PLAY_FREE
            --self._gameView:setBankerWaitIcon(self:SwitchViewChairID(self.wBankerUser),false,true,true)
            if self.cbDynamicJoin ~= 0 then 
                self._gameView:showWaitOtherTips()
                self._gameView:setBtnchangeShow(true,false)
            end
            -- self._gameView.btAuto:setVisible(false)
            self._gameView:setStartBtnShow(false)
        end
         


    else
            for i = 1, cmd.GAME_PLAYER do
                local wViewChairId = self:SwitchViewChairID(i - 1)
                if self.cbPlayStatus[i] == 1 then --当前玩家没有叫庄
                    if cbCallBankerStatus[i] == 0 then
                            if wViewChairId == cmd.MY_VIEWID then 
                                    if self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER then --自由抢庄
                                        self._gameView.nodeCall:setVisible(true)
                                    else
                                        self.cbCardData  = {} 
                                        self._gameView:gameCallBanker(cmd.MY_VIEWID, bFirstTimes,cmd_table.cbMaxCallBankerTimes)
                                        
                                    end
                                    if self.cbDynamicJoin == 0 then 
                                            self._gameView:setClockBgVisible(true)
                                            self:KillGameClock()
                                            self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd_table.cbTimeRemain) 

                                            self._gameView.nAutoStatus = cmd.AUTO_PLAY_CALL
                                    else
                                           self._gameView:showWaitOtherTips()
                                        self._gameView:setBtnchangeShow(true,false)
                                    end
                                 
                            end
                    else
                         self._gameView:setCallMultiple(wViewChairId,cbCallBankerTimes[i],false)  

                         if self.cbDynamicJoin == 0 then 
                                self._gameView:setClockBgVisible(false)
                                self:KillGameClock()
                                self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd_table.cbTimeRemain) 

                                self._gameView.nAutoStatus = cmd.AUTO_PLAY_CALL
                             
                        end

                    end
                else
                    if wViewChairId == cmd.MY_VIEWID then 
                        self._gameView:showWaitOtherTips()
                        self._gameView:setBtnchangeShow(true,false)
                    end
                end
            end     
    end 

    if self.cbDynamicJoin == 0 then 
        -- self._gameView.btAuto:setVisible(true)
    else
        -- self._gameView.btAuto:setVisible(false)
    end
    
    self._gameView:setStartBtnShow(false)
end
--叫分场景 
function GameLayer:onSceneScore(dataBuffer)
    --print("onSceneScore")
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusScore, dataBuffer);
        dump(cmd_table,"=============下注场景",6)
    self.lCellScore = cmd_table.lCellScore
    self._cardTypeTimes = cmd_table.wCardTypeTimes[1]

    cmd.TIME_USER_CALL_BANKER       = cmd_table.nTimeCallBanker
    --开始定时器
    cmd.TIME_USER_START_GAME        = cmd_table.nTimeStart
    --加注定时器
    cmd.TIME_USER_ADD_SCORE         = cmd_table.nTimeAddScore
    --摊牌定时器
    cmd.TIME_USER_OPEN_CARD         = cmd_table.nTimeOpenCard

    for i = 1, cmd.GAME_PLAYER do
        --游戏状态
        self.cbPlayStatus[i] = cmd_table.cbPlayStatus[1][i]
        --积分配置
        self.lFreeConif[i] = cmd_table.lFreeConfig[1][i]
        --百分比配置
        self.lPercentConfig[i] = cmd_table.lPercentConfig[1][i]
    end


        --游戏模式
    self.cbPlayMode = cmd_table.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1 or false

    self.cbDynamicJoin = cmd_table.cbDynamicJoin
    --print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~动态加入：", self.cbDynamicJoin)
    self.m_lMaxTurnScore = cmd_table.lTurnMaxScore

    self.isCanCuo = cmd_table.cbAdmitRevCard == 1 and true  or false 
    self.wBankerUser = cmd_table.wBankerUser
        -- 反作弊标识
    local bIsAllowAvertCheat = cmd_table.bIsAllowAvertCheat

    --游戏牌型
    self.m_tabPrivateRoomConfig.cardType = cmd_table.ctConfig
    --发牌模式
    self.m_tabPrivateRoomConfig.sendCardType = cmd_table.stConfig
    -- 坐庄模式
    self.m_tabPrivateRoomConfig.bankGameType = cmd_table.bgtConfig
    -- 下注模式
    self.m_tabPrivateRoomConfig.betType = cmd_table.btConfig

    self.m_tabPrivateRoomConfig.gtType = cmd_table.gtConfig

    self._gameView:showRoomRule(self.m_tabPrivateRoomConfig)

    self._gameView.nodeCall:setVisible(false)
    --已下注分数
    local lTableScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lTableScore[i] = cmd_table.lTableScore[1][i]
        if self.cbPlayStatus[i] == 1 then
            local wViewChairId = self:SwitchViewChairID(i - 1)
            self._gameView:setUserTableScore(wViewChairId, lTableScore[i])

            -- 显示下注中头像状态
            if self:SwitchViewChairID(self.wBankerUser) ~= wViewChairId and lTableScore[i] == 0 then
                if self._gameView.m_tabUserHead[wViewChairId] then
                    print("************显示下注中头像状态",wViewChairId)
                    self._gameView.m_tabUserHead[wViewChairId]:setHeadStatus(cmd.US_Bet)
                end
            end

            local viewId = self:SwitchViewChairID(i-1)
            if (viewId ~= cmd.MY_VIEWID) then 
                    if i-1 ~= self.wBankerUser  and lTableScore[i] ==0 then
                                local bNormal 
                                if self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_BETFIRST then
                                    bNormal = true
                                elseif self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
                                    bNormal = false  
                                end
                        

                                --self._gameView:setBankerWaitIcon(viewId,true,bNormal)
                        
                    end
            end

        end
    end

    --庄家
    
    local lRoomStorageStart = cmd_table.lRoomStorageStart
    local lRoomStorageCurrent = cmd_table.lRoomStorageCurrent
    local lTurnScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lTurnScore[i] = cmd_table.lTurnScore[1][i]
    end
    local lCollectScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lCollectScore[i] = cmd_table.lCollectScore[1][i]
    end


	    --叫庄倍数
    local cbCallBankerTimes = {}
    for i = 1, cmd.GAME_PLAYER do
        cbCallBankerTimes[i] = cmd_table.cbCallBankerTimes[1][i]
        if self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER then
            cbCallBankerTimes[i] = 1
        end
    end
    self._cbCallBankerTimes = cbCallBankerTimes
    self._gameView:setCallMultiple(self:SwitchViewChairID(self.wBankerUser),cbCallBankerTimes[self.wBankerUser+1],true)
    --庄家信息
    self._gameView:setBankerUser(self:SwitchViewChairID(self.wBankerUser), false)
    self._gameView:setBankerMultiple(self:SwitchViewChairID(self.wBankerUser))


    self._gameView:resetEffect()
    
    --牌值
    for i = 1, cmd.GAME_PLAYER do
        self.cbCardData[i] = {}
        for j = 1, cmd.MAX_CARDCOUNT do
            self.cbCardData[i][j] = cmd_table.cbCardData[i][j]
        end

        if cmd_table.lPlayerBetBtEx[1][i] >0 then
            local viewid = self:SwitchViewChairID(i-1)
            if self._gameView.m_tabUserHead[viewid] and viewid ~= cmd.MY_VIEWID then
                self._gameView.m_tabUserHead[viewid]:showBetBtEx(true)
            end
        end

    end


    --显示牌并开自己的牌
    for i = 1, cmd.GAME_PLAYER do
        if self.cbPlayStatus[i] == 1 then
            local wViewChairId = self:SwitchViewChairID(i - 1)
            for j = 1, cmd.MAX_CARDCOUNT do

                if self.cbCardData[i][j] ~= 0 then
                    --print("wViewChairId",wViewChairId)
                    local card
                    if wViewChairId == 4 then --右手边
                        card = self._gameView.nodeCard[wViewChairId][j+1]
                    else
                        card = self._gameView.nodeCard[wViewChairId][j]
                    end
                    card:setVisible(true)
                    if wViewChairId == cmd.MY_VIEWID and not self.m_bLookOn  then          --是自己则打开牌
                        local value = GameLogic:getCardValue(self.cbCardData[i][j])
                        local color = GameLogic:getCardColor(self.cbCardData[i][j])
                        self._gameView:setCardTextureRect(wViewChairId, j, value, color)
                    end
                end

            end
        end
    end

    if self.m_bLookOn then
            self._gameView:setClockBgVisible(true)
            self:KillGameClock()
            self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_TIME_USER_ADD_SCORE, cmd_table.cbTimeRemain) 
            -- self._gameView.btAuto:setVisible(false)
            self._gameView:setStartBtnShow(false)
            return
    end


    self._gameView.nAutoStatus = cmd.AUTO_PLAY_FREE
    -- 积分房卡配置的下注
    -- 下注类型
    if self:SwitchViewChairID(self.wBankerUser) ~= cmd.MY_VIEWID and lTableScore[self._MyChairID+1] == 0 then 
        print("my score is ===",lTableScore[self._MyChairID+1])
        if self.m_tabPrivateRoomConfig.betType == cmd.BETTYPE_CONFIG.BT_FREE_ then --自由配置额度
        --print("自由配置额度")
            self._gameView:setScoreJetton(self.lFreeConif,cmd_table.lPlayerBetBtEx[1][self:GetMeChairID()+1])
        elseif self.m_tabPrivateRoomConfig.betType == cmd.BETTYPE_CONFIG.BT_PENCENT_ then --百分比配置额度
            --print("百分比配置额度")
            local Jettons = {}
            for i=1,#self.lPercentConfig do
                local percent = self.lPercentConfig[i]/100
                local jetton = math.floor(self.m_lMaxTurnScore * percent)
                table.insert(Jettons, jetton)
                
            end
            self._gameView:setScoreJetton(Jettons,cmd_table.lPlayerBetBtEx[1][self:GetMeChairID()+1])
            dump(Jettons,"Jettons")
        else
            error("self.m_tabPrivateRoomConfig.betType error")
        end
        

        self._gameView:showChipBtn(self:SwitchViewChairID(self.wBankerUser))
        self._gameView.nAutoStatus = cmd.AUTO_PLAY_BET
    end





    print("self.cbDynamicJoin111",self.cbDynamicJoin)
    if self.cbDynamicJoin == 0 then 
        self:KillGameClock()
        self:SetGameClock(self.wBankerUser, cmd.IDI_TIME_USER_ADD_SCORE, cmd_table.cbTimeRemain)
        if lTableScore[self._MyChairID+1] == 0 and self:SwitchViewChairID(self.wBankerUser) ~= cmd.MY_VIEWID then
            self._gameView:setClockBgVisible(true)
        else
            self._gameView:setClockBgVisible(false)
        end
        -- self._gameView.btAuto:setVisible(true)
    else
        self._gameView:showWaitOtherTips()
        self._gameView:setBtnchangeShow(true,false)

        -- self._gameView.btAuto:setVisible(false)
        -- self._gameView:setStartBtnShow(false)
    end
    
end
--游戏场景
function GameLayer:onScenePlaying(dataBuffer)
    --print("onScenePlaying")
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer);
     dump(cmd_table,"=============游戏场景",6)
    self.lCellScore = cmd_table.lCellScore
    self._cardTypeTimes = cmd_table.wCardTypeTimes[1]

    cmd.TIME_USER_CALL_BANKER       = cmd_table.nTimeCallBanker
    --开始定时器
    cmd.TIME_USER_START_GAME        = cmd_table.nTimeStart
    --加注定时器
    cmd.TIME_USER_ADD_SCORE         = cmd_table.nTimeAddScore
    --摊牌定时器
    cmd.TIME_USER_OPEN_CARD         = cmd_table.nTimeOpenCard

        -- 反作弊标识
    local bIsAllowAvertCheat = cmd_table.bIsAllowAvertCheat
    local bDelayFreeDynamicJoin = cmd_table.bDelayFreeDynamicJoin

    --游戏模式
    self.cbPlayMode = cmd_table.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1 or false


    --游戏牌型
    self.m_tabPrivateRoomConfig.cardType = cmd_table.ctConfig
    --发牌模式
    self.m_tabPrivateRoomConfig.sendCardType = cmd_table.stConfig
    -- 坐庄模式
    self.m_tabPrivateRoomConfig.bankGameType = cmd_table.bgtConfig
    -- 下注模式
    self.m_tabPrivateRoomConfig.betType = cmd_table.btConfig

    self.m_tabPrivateRoomConfig.gtType = cmd_table.gtConfig

    self._gameView:showRoomRule(self.m_tabPrivateRoomConfig)

    self.isCanCuo = cmd_table.cbAdmitRevCard == 1 and true  or false 

    for i = 1, cmd.GAME_PLAYER do
        --游戏状态
        self.cbPlayStatus[i] = cmd_table.cbPlayStatus[1][i]
        --积分配置
        self.lFreeConif[i] = cmd_table.lFreeConfig[1][i]
        --百分比配置
        self.lPercentConfig[i] = cmd_table.lPercentConfig[1][i]
    end
    self.cbDynamicJoin = cmd_table.cbDynamicJoin
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~动态加入：", self.cbDynamicJoin,bDelayFreeDynamicJoin)
    self.m_lMaxTurnScore = cmd_table.lTurnMaxScore
    local lTableScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lTableScore[i] = cmd_table.lTableScore[1][i]
        if self.cbPlayStatus[i] == 1 and lTableScore[i] ~= 0 then
            local wViewChairId = self:SwitchViewChairID(i - 1)
            self._gameView:gameAddScore(wViewChairId, lTableScore[i])
        end
    end
    self.wBankerUser = cmd_table.wBankerUser
    local lRoomStorageStart = cmd_table.lRoomStorageStart
    local lRoomStorageCurrent = cmd_table.lRoomStorageCurrent

    --local handCard = {0x1b,0x06,0x1a,0x0c,0x31}
    for i = 1, cmd.GAME_PLAYER do
        self.cbCardData[i] = {}
        for j = 1, #cmd_table.cbHandCardData[1] do
            self.cbCardData[i][j] = cmd_table.cbHandCardData[i][j]
            --if i== self:GetMeChairID() +1 then
                --self.cbCardData[i][j]=handCard[j]
           -- end
        end
    end


    local bOxCard = {}
    for i = 1, cmd.GAME_PLAYER do
        bOxCard[i] = cmd_table.bOpenCard[1][i]
        local wViewChairId = self:SwitchViewChairID(i - 1)
        if self.cbPlayStatus[i] == 1 and bDelayFreeDynamicJoin == false then
            if true == bOxCard[i]  and cmd.MY_VIEWID == wViewChairId and not self.m_bLookOn then
                self._gameView:onButtonConfirm(wViewChairId)
            end
            if not bOxCard[i] and cmd.MY_VIEWID == wViewChairId and not self.m_bLookOn then 
                print("************************还没开牌")
                self._gameView:setCombineCard(self.cbCardData[i])
                self._gameView:setSpecialInfo(cmd_table.bSpecialCard[1][i],cmd_table.cbOriginalCardType[1][i])
                self._gameView:showCalculate(true,true)
                --self._gameView:gameScenePlaying()
            end
            print("还没开牌 wViewChairId, bOxCard[i]",wViewChairId, bOxCard[i])
            self._gameView:setOpenCardVisible(wViewChairId, true, bOxCard[i])
        end
    end

    local lTurnScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lTurnScore[i] = cmd_table.lTurnScore[i]
    end
    local lCollectScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lCollectScore[i] =  cmd_table.lCollectScore[i]
    end


    --显示牌并开自己的牌
    for i = 1, cmd.GAME_PLAYER do
        if self.cbPlayStatus[i] == 1 then
            local wViewChairId = self:SwitchViewChairID(i - 1)
            for j = 1, 5 do
                local card = self._gameView.nodeCard[wViewChairId][j]
                card:setVisible(true)
                if (wViewChairId == cmd.MY_VIEWID or bDelayFreeDynamicJoin) and not self.m_bLookOn then          --是自己则打开牌
                    local value = GameLogic:getCardValue(self.cbCardData[i][j])
                    local color = GameLogic:getCardColor(self.cbCardData[i][j])
                    self._gameView:setCardTextureRect(wViewChairId, j, value, color)
                    if bDelayFreeDynamicJoin then
                        self._gameView:showPlayerCardType(wViewChairId, cmd_table.cbCombineCardType[1][i])
                        self._gameView:runWinLoseAnimate(wViewChairId, cmd_table.lTurnScore[1][i])
                    end
                end
            end
        end
    end

    --叫庄倍数
    local cbCallBankerTimes = {}
    for i = 1, cmd.GAME_PLAYER do
        cbCallBankerTimes[i] = cmd_table.cbCallBankerTimes[1][i]
        if self.wBankerUser + 1 == i then
            if cmd_table.cbCallBankerTimes[1][i] == 0 then
                cbCallBankerTimes[i] = 1
            end
        end
        -- if self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER then
        --     cbCallBankerTimes[i] = 1
        -- end
    end
    self._cbCallBankerTimes = cbCallBankerTimes
    self._gameView:setCallMultiple(self:SwitchViewChairID(self.wBankerUser),cbCallBankerTimes[self.wBankerUser+1],true)

    local clockId = self.wBankerUser

    if self.wBankerUser  ~= yl.INVALID_CHAIR then
        self._gameView:setBankerUser(self:SwitchViewChairID(self.wBankerUser))
        -- self._gameView.m_tabUserHead[self:SwitchViewChairID(self.wBankerUser)]:showBank(true)
    else
        clockId = self:GetMeChairID()
    end

    if bDelayFreeDynamicJoin then
        -- self.cbDynamicJoin = 0
        -- self:runAction(cc.Sequence:create(cc.DelayTime:create(6),cc.CallFunc:create(function ()
            -- self._gameView.btAuto:setVisible(true)
            -- self._gameView.btStart:setVisible(true)
            -- self._gameView:setBtnchangeShow(true)

        -- end)))
        return
    end

    if self.m_bLookOn then
        self._gameView:setClockBgVisible(true)
        self:KillGameClock()
        self:SetGameClock(clockId, cmd.IDI_TIME_OPEN_CARD, cmd_table.cbTimeRemain)
            -- self._gameView.btAuto:setVisible(false)
            self._gameView:setStartBtnShow(false)
            return
    end
    
    print("self.cbDynamicJoin222",self.cbDynamicJoin)
    if self.cbDynamicJoin == 0 then 
        self._gameView:setClockBgVisible(true)
        self:KillGameClock()
        self:SetGameClock(clockId, cmd.IDI_TIME_OPEN_CARD, cmd_table.cbTimeRemain)
        self._gameView.nAutoStatus = cmd.AUTO_PLAY_OPENCARD

        self:stopAllActions()
        -- self._gameView.btAuto:setVisible(true)
        self:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
            
        if self._gameView.bIsAuto and self:isPlayerPlaying(cmd.MY_VIEWID)  then
            self._gameView:autoPlay(self._gameView.nAutoStatus)
        end

        end)))
    else
        self._gameView:showWaitOtherTips()
        self._gameView:setBtnchangeShow(true,false)
        -- self._gameView.btAuto:setVisible(false)
        -- self._gameView:setStartBtnShow(false)
    end
end

-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)
    print("onEventGameMessage===================="..sub)
	if sub == cmd.SUB_S_CALL_BANKER then           --叫庄


        self.m_cbGameStatus = cmd.GS_TK_CALL
        print("游戏模式===>",self.m_tabPrivateRoomConfig.bankGameType,self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_ROB or self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER)
        local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_CallBanker, dataBuffer)
        
        self._gameView:setStartBtnShow(false)


        if self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_ROB or self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER then
            self:runAction(cc.Sequence:create(
                cc.CallFunc:create(function()self._gameView:runStartAction(callback)  self:KillGameClockOnStart() end),
                cc.DelayTime:create(2),
                cc.CallFunc:create(function()self:onSubCallBanker(cmd_table)end)))
            self.bDelaySendFour = true
        else
            self:onSubCallBanker(cmd_table) 
        end 

    elseif sub == cmd.SUB_S_CALL_BANKERINFO then
        self:onSubCallBankerInfo(dataBuffer) 
                             --
	elseif sub == cmd.SUB_S_GAME_START then        --游戏开始

            self.m_cbGameStatus = cmd.GS_TK_CALL 
            local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
            
            self._gameView:setStartBtnShow(false)

            if not (self.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_ROB) and
                not (self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER) then
                self:runAction(cc.Sequence:create(cc.CallFunc:create(function()self._gameView:runStartAction(callback) self:KillGameClockOnStart() end),
                    cc.DelayTime:create(2),
                    cc.CallFunc:create(function()self:onSubGameStart(cmd_table)end)))
            else
                self:onSubGameStart(cmd_table)
            end


	elseif sub == cmd.SUB_S_ADD_SCORE then         --加注结果
                        self.m_cbGameStatus = cmd.GS_TK_SCORE
	           self:onSubAddScore(dataBuffer)
	elseif sub == cmd.SUB_S_SEND_CARD then         --发牌消息

            self.m_SendCardStatus = cmd.SEND_COMPLETE

                self.m_cbGameStatus = cmd.GS_TK_PLAYING
                local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_SendCard, dataBuffer)
                if  self.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_TONGBI then
                    self:runAction(cc.Sequence:create(cc.CallFunc:create(function()self._gameView:runStartAction(callback) self:KillGameClockOnStart() end),
                    cc.DelayTime:create(2),
                    cc.CallFunc:create(function()self:onSubSendCard(cmd_table)end)))
                else
                    -- self:onSubSendCard(cmd_table)
                    self:runAction(cc.Sequence:create(
                    cc.DelayTime:create(1),
                    cc.CallFunc:create(function()self:onSubSendCard(cmd_table)end)))
                end


	elseif sub == cmd.SUB_S_OPEN_CARD then         --用户摊牌
                        self.m_cbGameStatus = cmd.GS_TK_PLAYING
                self:onSubOpenCard(dataBuffer)
	elseif sub == cmd.SUB_S_SEND_FOUR_CARD then         --发送4张扑克
                self.m_SendCardStatus = cmd.SEND_FOUR
                self.m_cbGameStatus = cmd.GS_TK_PLAYING
                local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_SendFourCard, dataBuffer);
                -- if self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_ROB or self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER then
                    print("发送4张扑克1111111111111111111111,self.bDelaySendFour",self.bDelaySendFour)
                if self.bDelaySendFour then   
                    self:runAction(cc.Sequence:create(
                        cc.DelayTime:create(1),
                        cc.CallFunc:create(function() print("发送4张扑克1111111111111111111111") self:onSubSendFourCard(cmd_table) end)))
                    
                else
                    self:onSubSendFourCard(cmd_table) 
                end

	elseif sub == cmd.SUB_S_PLAYER_EXIT then       --用户强退
                        self.m_cbGameStatus = cmd.GS_TK_PLAYING
	           self:onSubPlayerExit(dataBuffer)
	elseif sub == cmd.SUB_S_GAME_END then          --游戏结束
	           self:onSubGameEnd(dataBuffer)
            elseif sub == cmd.SUB_S_RECORD then            --游戏记录

                        self:onSubGameRecord(dataBuffer)
            elseif sub == cmd.SUB_S_ROOMCARD_RECORD then
                        self:onSubPriGameRecord(dataBuffer)  
            elseif sub == cmd.SUB_S_GAME_STATUS then

                    local gameStatus = dataBuffer:readbyte()
                    if gameStatus == 1 then 
                        self._gameView:onResetView()
                        if PriRoom and GlobalUserItem.bPrivateRoom then
                            if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
                                PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
                                self._gameView._priView:onRefreshInfo()
                            end
                        end
                        if not self.m_bLookOn then 
                            self._gameView:showSitBtn(false)
                        end
                        self._gameView:setStartBtnShow(false)
                        self:KillGameClock()
                        self._gameView:setClockBgVisible(false)
                    end
    elseif sub == cmd.SUB_S_CONFIG_DEBUG then
        local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_ConfigDebug, dataBuffer);
        dump(cmd_table,"调试信息",11)
        self._gameView:SuperCtrlUpdateCurStatus(cmd_table)
    elseif sub == cmd.SUB_S_GET_POOL_CARD then
        local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_PoolCard, dataBuffer);
        dump(cmd_table,"牌池")
        self._gameView:SuperCtrlOpenCardNode(cmd_table.bCards[1])

    elseif sub == cmd.SUB_S_REQUEST_CARDTYPE then
        local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_RequestCardType, dataBuffer);
        dump(cmd_table,"牌型请求")
        self._gameView:SuperCtrlUpdate(cmd_table.cbGameCards[1], true)

    elseif sub == cmd.SUB_S_ERROR_NOTIFY then
        local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_ErrorNotify, dataBuffer);
        showToast(self,cmd_table.msg,1)

    elseif sub == cmd.SUB_S_GAMEFREE_NOTIFY then   -- 服务器通知可以显示准备按钮了
        self.m_cbGameStatus = cmd.GS_TK_FREE
        local function showStartBtn()
            self._gameView:cleanFlyScore()
            if not GlobalUserItem.bPrivateRoom then
                self._gameView.btChangedesk:setEnabled(true)
            end
            if self:GetMeUserItem().cbUserStatus == yl.US_READY then
                return
            end
            self._gameView:setClockBgVisible(true)
            self:SetGameClock(self:GetMeChairID(), cmd.IDI_START_GAME, cmd.TIME_USER_START_GAME)
            if self.m_bLookOn then
                return
            end
            self._gameView:setBtnchangeShow(true,true)
            self.cbDynamicJoin = 0
            self:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                    if self._gameView.bIsAuto and not self.m_bLookOn then --and self:isPlayerPlaying(cmd.MY_VIEWID) then
                        self._gameView:autoPlay(self._gameView.nAutoStatus)
                    end
            end)))
        end

        if self._gameView.bActComplete == false and self.cbDynamicJoin == 0 then
            print("服务器通知可以显示准备按钮了2222",self._gameView.bActComplete)
            self._gameView.btStart:runAction(cc.RepeatForever:create(
                cc.Sequence:create(
                    cc.DelayTime:create(1),
                    cc.CallFunc:create(function()
                        if self._gameView.bActComplete == true then
                            self._gameView.btStart:stopAllActions()
                            showStartBtn()
                        end
                    end)))
                )
        else
            print("服务器通知可以显示准备按钮了1111111")
            showStartBtn()
        end
	else
        --print("unknow gamemessage sub is"..sub)
        --error("unknow gamemessage sub")
	end
end

--用户叫庄
function GameLayer:onSubCallBanker(cmd_table)
    -- print("dataBuffer len",dataBuffer:getlen())
    local cmd_table = cmd_table --ExternalFun.read_netdata(cmd.CMD_S_CallBanker, dataBuffer)
    --dump(cmd_table)
    print("用户叫庄",cmd_table.wNewTurnChairId)


    local isAuto = true
    self.isFirstTurn = false

    if self.m_bLookOn then
            self._gameView:setClockBgVisible(true)
            self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd.TIME_USER_CALL_BANKER)
            return
    end

    if self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER then --自由抢庄
        self._gameView.nodeCall:setVisible(true)
        -- self._gameView:setStartBtnShow(false)
        self._gameView:setBtnchangeShow(false,false)
    else
        -- self.cbCardData  = {}

       
        if cmd_table.wNewTurnChairId ~= yl.INVALID_CHAIR then

            self.wBankerUser = cmd_table.wNewTurnChairId
            self._gameView:setBankerUser(self:SwitchViewChairID(self.wBankerUser))


            if cmd_table.wNewTurnChairId == self:GetMeChairID() then
                self._gameView:gameCallBanker(cmd.MY_VIEWID, bFirstTimes,cmd_table.cbMaxCallBankerTimes,true)
                self._gameView.bLastCall =GameViewLayer.TAG_ENUM.BT_MUL1
            else
                isAuto = false
                self._gameView.nAutoStatus = cmd.AUTO_PLAY_FREE
                --self._gameView:setBankerWaitIcon(self:SwitchViewChairID(self.wBankerUser),false,true,true)
                
            end

            self.isFirstTurn = true

        else
            self._gameView:gameCallBanker(cmd.MY_VIEWID, bFirstTimes,cmd_table.cbMaxCallBankerTimes)
        end
                                          
     end
    
    if self.cbDynamicJoin == 0 and self.cbPlayStatus[self:GetMeChairID()+1] == 1 then 
        self:KillGameClock()
        -- self:stopAllActions()
        -- self._gameView.btAuto:setVisible(true)

        if isAuto then
            self._gameView:setClockBgVisible(true)
            self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd.TIME_USER_CALL_BANKER)
            self._gameView.nAutoStatus = cmd.AUTO_PLAY_CALL
            self:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                
                if self._gameView.bIsAuto and self:isPlayerPlaying(cmd.MY_VIEWID) then
                    self._gameView:autoPlay(self._gameView.nAutoStatus)
                end

            end)))
        else
            self._gameView:setClockBgVisible(false)
            self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd.TIME_USER_CALL_BANKER)
        end
    else
        self:KillGameClock()
        self._gameView:showWaitOtherTips()
        self._gameView:setBtnchangeShow(true,false)
        -- self._gameView.btAuto:setVisible(false)               
    end

end

local bEffect = true
function GameLayer:onSubCallBankerInfo(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_CallBankerInfo, dataBuffer)
    -- self.cbCardData  = {} 
    dump(cmd_table, "call banker info======================")

     --叫庄状态
    local cbCallBankerStatus = {}
    for i = 1, cmd.GAME_PLAYER do
        cbCallBankerStatus[i] = cmd_table.cbCallBankerStatus[1][i]
    end

    --叫庄倍数
    local cbCallBankerTimes = {}
    for i = 1, cmd.GAME_PLAYER do
        cbCallBankerTimes[i] = cmd_table.cbCallBankerTimes[1][i]
        self.m_callBanker[i]=cmd_table.cbCallBankerTimes[1][i]
    end

    for i=1,cmd.GAME_PLAYER do 
        if 1 == cbCallBankerStatus[i] then
           local wViewChairId = self:SwitchViewChairID(i - 1)
           if wViewChairId == cmd.MY_VIEWID then 
               self._gameView:setClockBgVisible(false)
               self._gameView:showCallBankerMul(0)
           end
           --显示倍数
            if not (cbCallBankerTimes[i] == 0 and self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_ROB and PriRoom:getInstance().m_tabPriData.dwPlayCount == 1) then
                --print("setCallMultiple===========================1,"..wViewChairId..","..cbCallBankerTimes[i])
                self._gameView:setCallMultiple(wViewChairId,cbCallBankerTimes[i],false)
            end

            local item = self:getUserInfoByChairID(cmd_table.wCallBankerChairId)
            if cmd_table.wCallBankerChairId == i-1 and item then
                if cbCallBankerTimes[i] > 0 then
                    if not bEffect then
                        return
                    end
                    self._gameView:playEffect("rober_bank_",item.cbGender)
                    bEffect = false
                    local act = cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function ( ... )
                            bEffect = true
                        end))
                    act:setTag(78)
                    self._gameView:runAction(act)
                else
                    self._gameView:playEffect("buqiang_",item.cbGender)
                end
            end
        end
    end

end
--游戏开始
function GameLayer:onSubGameStart(cmd_table)
    -- local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer);
    --dump(cmd_table)
    print("游戏开始")

    -- self._gameView:playEffect("gameStart.mp3")
    self.m_lMaxTurnScore = cmd_table.lTurnMaxScore
    self.wBankerUser = cmd_table.wBankerUser

    dump(cmd_table, "the game start info")
    --print("m_lMaxTurnScore is ",self.m_lMaxTurnScore)

    self._gameView.nodeCall:setVisible(false)

    --assert(false)

    -- 玩家状态
    for i = 1, cmd.GAME_PLAYER do
        --游戏状态
        self.cbPlayStatus[i] = cmd_table.cbPlayerStatus[1][i]
        --积分配置
        self.lFreeConif[i] = cmd_table.lFreeConfig[1][i]
        --百分比配置
        self.lPercentConfig[i] = cmd_table.lPercentConfig[1][i]
    end

    local maxCall = 0
    local maxIndex ={} 
    for i=1,cmd.GAME_PLAYER do
            if self.m_callBanker[i]>maxCall then
                maxCall = self.m_callBanker[i]
                maxIndex = {}
                table.insert(maxIndex,i)

            elseif self.m_callBanker[i] == maxCall then
                table.insert(maxIndex,i)
            end
            
    end
    local delaytime = 3
            self:KillGameClock()
            self._gameView:setClockVisible(false)  -- 抢庄完成动画时间内隐藏倒计时
    if #maxIndex >1 and (self.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_ROB or self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER) then
            self._gameView:beginRobAnimation(maxIndex,self.wBankerUser)
            -- self._gameView:runAction(cc.Sequence:create(cc.DelayTime:create(delaytime),cc.CallFunc:create(function ( )
            --     --显示庄家标识
            --     self._gameView:setBankerUser(self:SwitchViewChairID(self.wBankerUser))
            --     --只显示庄家倍数
            --     self._gameView:setBankerMultiple(self:SwitchViewChairID(self.wBankerUser))
            --     for i = 1, cmd.GAME_PLAYER do
                    
            --         if cmd_table.lPlayerBetBtEx[1][i] >0 then
            --             local viewid = self:SwitchViewChairID(i-1)
            --             if self._gameView.m_tabUserHead[viewid] and viewid ~= cmd.MY_VIEWID then
            --                 self._gameView.m_tabUserHead[viewid]:showBetBtEx(true)
            --             end
            --         end

            --      end

            -- end)))
    else

        --显示庄家标识
        self._gameView:setBankerUser(self:SwitchViewChairID(self.wBankerUser), true)
        --只显示庄家倍数
        self._gameView:setBankerMultiple(self:SwitchViewChairID(self.wBankerUser))

          -- for i = 1, cmd.GAME_PLAYER do
                    
          --           if cmd_table.lPlayerBetBtEx[1][i] >0 then
          --               local viewid = self:SwitchViewChairID(i-1)
          --               if self._gameView.m_tabUserHead[viewid] and viewid ~= cmd.MY_VIEWID then
          --                   self._gameView.m_tabUserHead[viewid]:showBetBtEx(true)
          --               end
          --           end

          -- end

    end

    --庄家等待下注Tips
    -- for i=1,cmd.GAME_PLAYER do
    --    if 1 == self.cbPlayStatus[i]  then
    --        local viewId = self:SwitchViewChairID(i-1)
    --        if (viewId ~= cmd.MY_VIEWID) then 
    --             if i-1 ~= self.wBankerUser  then
    --                 local bNormal 
    --                 if self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_BETFIRST then
    --                     bNormal = true
    --                 elseif self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
    --                     bNormal = false  
    --                 end
                    

    --                 --self._gameView:setBankerWaitIcon(viewId,true,bNormal)
                   
    --             end
    --        end

    --    end
    -- end

    self._gameView:removeChildByName("banker_xuanBei")

    --发牌模式
    self.m_tabPrivateRoomConfig.sendCardType = cmd_table.stConfig
    -- 坐庄模式
    self.m_tabPrivateRoomConfig.bankGameType = cmd_table.bgtConfig
    -- 下注模式
    self.m_tabPrivateRoomConfig.betType = cmd_table.btConfig
    -- 下注类型
    if self.m_tabPrivateRoomConfig.betType == cmd.BETTYPE_CONFIG.BT_FREE_ then --自由配置额度
        --print("自由配置额度")
        self._gameView:setScoreJetton(self.lFreeConif,cmd_table.lPlayerBetBtEx[1][self:GetMeChairID()+1])
    elseif self.m_tabPrivateRoomConfig.betType == cmd.BETTYPE_CONFIG.BT_PENCENT_ then --百分比配置额度
        --print("百分比配置额度")
        local Jettons = {}
        for i=1,#self.lPercentConfig do
            local percent = self.lPercentConfig[i]/100
            local jetton = math.floor(self.m_lMaxTurnScore * percent)
            table.insert(Jettons, jetton)
            
        end
        self._gameView:setScoreJetton(Jettons,cmd_table.lPlayerBetBtEx[1][self:GetMeChairID()+1])
    else
        error("self.m_tabPrivateRoomConfig.betType error")
    end
    local setJetton = function ()
        if self:SwitchViewChairID(self.wBankerUser) ~=  cmd.MY_VIEWID and not self.m_bLookOn and 1 == self.cbPlayStatus[self:GetMeChairID()+1] then
             
            self._gameView:showChipBtn(self:SwitchViewChairID(self.wBankerUser))
            -- 显示下注按钮后再设置状态，否则会报错
            self._gameView.nAutoStatus = cmd.AUTO_PLAY_BET

        end

        self._gameView:setClockBgVisible(false)
        -- if self.cbDynamicJoin == 0 and not self.m_bLookOn then 
        if self.cbDynamicJoin == 0 then 
            self:SetGameClock(self.wBankerUser, cmd.IDI_TIME_USER_ADD_SCORE, cmd.TIME_USER_ADD_SCORE)
            if self.wBankerUser ~=  self:GetMeChairID() then
                self._gameView:setClockBgVisible(true)
            end
        end
        -- 显示下注中头像状态
        for i = 1, cmd.GAME_PLAYER do
            local viewid = self:SwitchViewChairID(i-1)
            if self:SwitchViewChairID(self.wBankerUser) ~= viewid then
                if self._gameView.m_tabUserHead[viewid] and self.cbPlayStatus[i] == 1 then
                    self._gameView.m_tabUserHead[viewid]:setHeadStatus(cmd.US_Bet)
                end
            end
        end
    end

    -- 庄家动画完成之后再显示下注按钮
    delaytime = delaytime + 1
    if #maxIndex >1 and (self.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_ROB or self.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_FREEBANKER) then
        self:runAction(cc.Sequence:create(cc.DelayTime:create(delaytime),cc.CallFunc:create(function( ... )
            setJetton()
        end)))
    else
        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.5),cc.CallFunc:create(function( ... )
            setJetton()
        end)))
    end
    self._gameView:resetEffect()

    -- self._gameView:setClockBgVisible(false)

    -- if self.cbDynamicJoin == 0 and not self.m_bLookOn then 
    --     self:SetGameClock(self.wBankerUser, cmd.IDI_TIME_USER_ADD_SCORE, cmd.TIME_USER_ADD_SCORE)
    --     if self.wBankerUser ~=  self:GetMeChairID() then
    --         self._gameView:setClockBgVisible(true)
    --     end
    -- end

    --发牌
    for i = 1, cmd.GAME_PLAYER do
        self.cbCardData[i] = {}
        for j = 1, 5 do
            self.cbCardData[i][j] = cmd_table.cbCardData[i][j]
        end

        if cmd_table.lPlayerBetBtEx[1][i] >0 then
            local viewid = self:SwitchViewChairID(i-1)
            if self._gameView.m_tabUserHead[viewid] and viewid ~= cmd.MY_VIEWID then
                self._gameView.m_tabUserHead[viewid]:showBetBtEx(true)
            end
        end

    end


    local sendCount = 0
    if self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
        sendCount = 4
        
    end
    if not (self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR and  
        self.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_ROB) or self.isFirstTurn  then
        self._gameView:gameSendCard(self:SwitchViewChairID(self.wBankerUser),sendCount)
    end

    if self.m_bLookOn then
            return
    end

        --打开自己的牌
    --dump(self._gameView.nodeCard[cmd.MY_VIEWID])
    for i = 1, cmd.MAX_CARDCOUNT do
        local index = self:GetMeChairID() + 1
        local data = self.cbCardData[index][i]
        local value = GameLogic:getCardValue(data)
        local color = GameLogic:getCardColor(data)
        local card = self._gameView.nodeCard[cmd.MY_VIEWID][i]
        --self._gameView:setCardTextureRect(cmd.MY_VIEWID, i, value, color)
    end


    if self.cbPlayStatus[self._MyChairID + 1] == 1 then
        -- self._gameView.btAuto:setVisible(true)
    else
        self:KillGameClock()
        self._gameView:showWaitOtherTips()
        self._gameView:setBtnchangeShow(true,false)
        -- self._gameView.btAuto:setVisible(false)    

    end

    delaytime = delaytime + 1
    self:runAction(cc.Sequence:create(cc.DelayTime:create(delaytime),cc.CallFunc:create(function ()
            
        if self._gameView.bIsAuto and self:isPlayerPlaying(cmd.MY_VIEWID) then
            self._gameView:autoPlay(self._gameView.nAutoStatus)
        end

        end)))
    
end


function GameLayer:getGameRule(buffer)

        -- buffer:readbyte()
        -- buffer:readbyte()
        -- buffer:readbyte()

        local strTable ={}
        --游戏模式(玩法)
        local gameMode =  buffer[4]--buffer:readbyte()

        local szMode = "经典模式"
        if gameMode == 23 then
            szMode = "疯狂加倍"
        else
            szMode = "经典模式"   
        end
        table.insert(strTable,szMode)

        --规则
        local sendMode = buffer[5]--buffer:readbyte()

        local szSendMode=""
        if sendMode == 32 then
            szSendMode = "发四等五"
        elseif sendMode == 33 then
            szSendMode = "下注发牌"   
        end
        table.insert(strTable,szSendMode)

        local jokerMode = buffer[6]--buffer:readbyte() --有无大小王
       
        --庄家玩法
        local bankerMode = buffer[7]
        local szbankerMode=""
        if bankerMode == 52 then
            szbankerMode = "霸王庄"
        elseif bankerMode == 53 then
            szbankerMode = "明牌抢庄"  
        elseif bankerMode == 54 then
            szbankerMode = "牛牛上庄"
        elseif bankerMode == 55 then 
            szbankerMode = "无牛下庄"
        elseif bankerMode == 56 then 
            szbankerMode = "自由抢庄"
        elseif bankerMode == 57 then 
            szbankerMode = "通比玩法"
        end
        table.insert(strTable,szbankerMode)
    return strTable
end


--用户下注
function GameLayer:onSubAddScore(dataBuffer)
    print("用户下注")

    self.bAddScore = true
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_AddScore, dataBuffer);
    local delayTime = 0.1

    if self.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_ROB then
        delayTime = 2
    
    end
            
        --self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ( )
             local wAddScoreUser = cmd_table.wAddScoreUser
            local lAddScoreCount = cmd_table.lAddScoreCount

            local userViewId = self:SwitchViewChairID(wAddScoreUser)
            --dump(cmd_table,"用户下注=========",6)
            self._gameView:gameAddScore(userViewId, lAddScoreCount,true)
            --self._gameView:setBankerWaitIcon(userViewId,false)
            
        --end)))
    
  
   self._gameView:playEffect("ADD_SCORE.WAV")
end

--发牌消息
function GameLayer:onSubSendCard(cmd_table)

    print("发牌消息")
    
    dump(cmd_table)
    for i = 1, cmd.GAME_PLAYER do
        local viewid = self:SwitchViewChairID(i-1)
        self.cbCardData[i] = {}
        for j = 1, 5 do
            self.cbCardData[i][j] = cmd_table.cbCardData[i][j]
        end

        if  viewid == cmd.MY_VIEWID then  --原始数据
            print("自己的chairid",i-1)
            self._gameView:setCombineCard(self.cbCardData[i])
            --if self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
                self.m_SpecialType = cmd_table.bSpecialCard[1][i]
                self.m_initType = cmd_table.cbOriginalCardType[1][i]
            --else
                --self._gameView:setSpecialInfo(cmd_table.bSpecialCard[1][i],cmd_table.cbOriginalCardType[1][i])
            --end

        end

        if self.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_TONGBI then
                
                if self.cbCardData[i][1] ~= 0 then
                    self.cbPlayStatus[i] = 1
                else
                    self.cbPlayStatus[i] = 0
                end
        end
    end

    self._gameView:SuperCtrlUpdate()       
    
    --打开自己的牌
    if self:GetMeChairID() == 65535 then
        return
    end
    for i = 1, cmd.MAX_CARDCOUNT do
        local index = self:GetMeChairID() + 1
        local data = self.cbCardData[index][i]
        local value = GameLogic:getCardValue(data)
        local color = GameLogic:getCardColor(data)
        local card = self._gameView.nodeCard[cmd.MY_VIEWID][i]
        --if self.m_tabPrivateRoomConfig.sendCardType ~= cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
             --self._gameView:setCardTextureRect(cmd.MY_VIEWID, i, value, color)
        --end
    end


    self._gameView.nAutoStatus = cmd.AUTO_PLAY_OPENCARD

    local sendCount = 5
    if self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR  then
        sendCount = 1
        if self.m_enterMessage == cmd.GS_TK_PLAYING and self.cbDynamicJoin ~= 0 then
            sendCount = 5
        end
       -- self._gameView.nAutoStatus = cmd.AUTO_PLAY_OPENCARD
    end

    if  self.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_TONGBI then
        sendCount = 5 
        for i=1,cmd.GAME_PLAYER do
            local viewid = self:SwitchViewChairID(i-1)

            if self.cbPlayStatus[i] == 1 then
                    self._gameView:gameAddScore(viewid, self.lCellScore,true)
            end

        end
        self._gameView:playEffect("ADD_SCORE.WAV")

    end



    local beginViewId = self:SwitchViewChairID(self.wBankerUser)

    if self.wBankerUser  == yl.INVALID_CHAIR then

        beginViewId = 3  --通比玩法从自己开始
    end
    self._gameView:gameSendCard(beginViewId,sendCount)

    self._gameView:setClockBgVisible(false)

         --庄家等待下注Tips
    for i=1,cmd.GAME_PLAYER do 
        --self._gameView:setBankerWaitIcon(i,false)

            local viewid = self:SwitchViewChairID(i-1)
            if self._gameView.m_tabUserHead[viewid] and (sendCount == 5 or  sendCount == 1) then
                self._gameView.m_tabUserHead[viewid]:showBetBtEx(false)
            end

    end

    print("================sendcard1")
    if self.m_bLookOn then
        self:KillGameClock()
        self._gameView:setClockBgVisible(true)
        self:SetGameClock(self:GetMeChairID(), cmd.IDI_TIME_OPEN_CARD, cmd.TIME_USER_OPEN_CARD)
            return
    end

    print("================sendcard2")
    if self.cbPlayStatus[self._MyChairID + 1] == 1 then
        -- self._gameView.btAuto:setVisible(true)
    end
        -- self:stopAllActions()
        self:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
            
            if self._gameView.bIsAuto and self:isPlayerPlaying(cmd.MY_VIEWID)  then
                self._gameView:autoPlay(self._gameView.nAutoStatus)
            end

        end)))


    if self.cbDynamicJoin == 0 and self.cbPlayStatus[self:GetMeChairID()+1] == 1 then 
        self:KillGameClock()
        self._gameView:setClockBgVisible(true)
        self:SetGameClock(self:GetMeChairID(), cmd.IDI_TIME_OPEN_CARD, cmd.TIME_USER_OPEN_CARD)
    else
        self:KillGameClock()
        self._gameView:showWaitOtherTips()
        self._gameView:setBtnchangeShow(true,false)
        -- self._gameView.btAuto:setVisible(false)
    end


    if self:isPlayerPlaying(cmd.MY_VIEWID) and (sendCount == 5 or  sendCount == 1) then
        --出现计分器
        if not self.isCanCuo then
            local delaytime = 0.3
            if self.wBankerUser ~= yl.INVALID_CHAIR and self:GetMeChairID() ~= yl.INVALID_CHAIR then
                delaytime = 0.3 + math.abs(self.wBankerUser - self:GetMeChairID())*0.8
            end

            self:runAction(cc.Sequence:create(cc.DelayTime:create(delaytime),cc.CallFunc:create(function ()

                self._gameView:setSpecialInfo(self.m_SpecialType,self.m_initType)
                self._gameView:showCalculate(true,true)
                

            end)))
            
        end
    end



       -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end


end

--用户摊牌
function GameLayer:onSubOpenCard(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_Open_Card, dataBuffer);
    --dump(cmd_table)
    --print("用户摊牌 wPlayerID",wPlayerID)
    local wPlayerID = cmd_table.wOpenChairID
    local bOpen = cmd_table.bOpenCard
    local wViewChairId = self:SwitchViewChairID(wPlayerID)


    if wViewChairId == cmd.MY_VIEWID and not self.m_bLookOn then
        self._gameView:onButtonConfirm(wViewChairId)
        self._gameView:setClockBgVisible(false)
    end
    self._gameView:stopCardAni(wViewChairId)
    self._gameView:setOpenCardVisible(wViewChairId,true,bOpen)

    -- self._gameView:playEffect("SEND_CARD.wav")

    if self._gameView.superCtrlLayer then
        if self._gameView.superCtrlLayer.pUserItem.wChairID == wPlayerID then
            self.m_SendCardStatus = cmd.OPEN_CARD
            self._gameView:SuperCtrlUpdate()
        end
    end

    -- 清除搓牌中头像状态--设为完成
    for i=1,cmd.GAME_PLAYER do
        local viewid = self:SwitchViewChairID(i-1)
        if self._gameView.m_tabUserHead[viewid] and viewid == wViewChairId  then
            if self.cbPlayStatus[i] == 1 then
                self._gameView.m_tabUserHead[viewid]:setHeadStatus(cmd.US_CuoWan)
            end
        end
    end
end




--发送4张扑克
function GameLayer:onSubSendFourCard(cmd_table)
    print("发送4张扑克")
    -- local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_SendFourCard, dataBuffer);
    dump(cmd_table)
    
    local delayTime = 0.1
    -- if self.isPlayMessage then
    --     self.isPlayMessage = false
    --     delayTime = 1
    -- end 
    self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ( ... )
            for i = 1, cmd.GAME_PLAYER do
                self.cbCardData[i] = {}
                for j = 1, 5 do
                    self.cbCardData[i][j] = cmd_table.cbCardData[i][j]
                end

                if self:SwitchViewChairID(i-1) == cmd.MY_VIEWID then  --原始数据
                    self._gameView:setCombineCard(self.cbCardData[i])
                end

                if nil ~= self._gameView.m_tabUserHead[self:SwitchViewChairID(i-1)] and self.cbCardData[i][1] ~= 0 then
                    self.cbPlayStatus[i] = 1
                else
                    self.cbPlayStatus[i] = 0
                end

            end

            ---if self.cbPlayStatus[self._MyChairID+1] == 1 and not self.m_bLookOn then 

                self._gameView:gameSendCard(cmd.MY_VIEWID,4)
            --end

            self._gameView:SuperCtrlUpdate() 

            if self.m_bLookOn then
                self:KillGameClock()
                --self:SetGameClock(self:GetMeChairID(), cmd.IDI_TIME_OPEN_CARD, cmd.TIME_USER_OPEN_CARD)
                self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd.TIME_USER_CALL_BANKER)
                    return
            end

            --打开自己的牌
            for i = 1, 4 do
                local index = self._MyChairID + 1
                local data = self.cbCardData[index][i]
                local value = GameLogic:getCardValue(data)
                local color = GameLogic:getCardColor(data)
                local card = self._gameView.nodeCard[cmd.MY_VIEWID][i]
                self._gameView:setCardTextureRect(cmd.MY_VIEWID, i, value, color)
               
            end

           
            if self.cbDynamicJoin == 0 and self.cbPlayStatus[self:GetMeChairID()+1] == 1 then  
                self:KillGameClock()
                --self:SetGameClock(self:GetMeChairID(), cmd.IDI_TIME_OPEN_CARD, cmd.TIME_USER_OPEN_CARD)
                self:SetGameClock(cmd.MY_VIEWID, cmd.IDI_CALL_BANKER, cmd.TIME_USER_CALL_BANKER)
            else
                self:KillGameClock()
                self._gameView:showWaitOtherTips()
                self._gameView:setBtnchangeShow(true,false)
                -- self._gameView.btAuto:setVisible(false)  
            end
    end)))
    

     --庄家等待下注Tips
    --for i=1,cmd.GAME_PLAYER do 
        --self._gameView:setBankerWaitIcon(i,false)
    --end
end


--用户强退
function GameLayer:onSubPlayerExit(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_PlayerExit, dataBuffer);
    --dump(cmd_table)
    local wPlayerID = cmd_table.wPlayerID
    local wViewChairId = self:SwitchViewChairID(wPlayerID)
    --self._gameView.bCanMoveCard = false
    -- self._gameView.nodePlayer[wViewChairId]:setVisible(false)
    --self._gameView.btOpenCard:setVisible(false)
    --self._gameView.btPrompt:setVisible(false)
    --self._gameView.spriteCalculate:setVisible(false)
    --for i = 1, 5 do
        -- self._gameView.cardFrame[i]:setVisible(false)
        -- self._gameView.cardFrame[i]:setSelected(false)
    --end
    self._gameView:setOpenCardVisible(wViewChairId, false)
    -- 播动画时，播完再删除
    print("=====用户强退",self._gameView.bActComplete )
    if self._gameView.bActComplete == false then
        self._gameView.nodePlayer[wViewChairId]:runAction(cc.RepeatForever:create(
            cc.Sequence:create(
                cc.DelayTime:create(1),
                cc.CallFunc:create(function()
                    if self._gameView.bActComplete == true then
                        self._gameView.nodePlayer[wViewChairId]:stopAllActions()
                        self._gameView.nodePlayer[wViewChairId]:setVisible(false)
                        self.cbPlayStatus[wPlayerID + 1] = 0
                    end
                end)))
            )
    else
        self._gameView.nodePlayer[wViewChairId]:setVisible(false)
        self.cbPlayStatus[wPlayerID + 1] = 0
    end
end

--游戏结束
function GameLayer:onSubGameEnd(dataBuffer)
    local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer);
   dump(cmd_table,"===================onSubGameEnd")
    self.m_bStartGame = false
    self.isPlayMessage = false
    self.cbDynamicJoin = 0
    self._gameView:onButtonConfirm(cmd.MY_VIEWID)
    self._gameView.bActComplete = false

    self:saveUserInfos()
    self:KillGameClockOnStart()

    self._gameView.nAutoStatus = cmd.AUTO_PLAY_START

    self.cbLastCardData = clone(cmd_table.cbLastCardData[1])
    -- dump(cmd_table.cbCardData,"结束牌型")

     --牌值
    for i = 1, cmd.GAME_PLAYER do
        self.cbCardData[i] = {}
        for j = 1, cmd.MAX_CARDCOUNT do
            self.cbCardData[i][j] = cmd_table.cbCardData[i][j]
        end
    end

    local lGameTax = {}
    for i = 1, cmd.GAME_PLAYER do
        lGameTax[i] = cmd_table.lGameTax[1][i]
    end

    self.lCardType = {0,0,0,0,0,0,0,0}
    local sartDelayTime = 0
    for i = 1, cmd.GAME_PLAYER do
        self.lGameEndScore[i] = cmd_table.lGameScore[1][i]
        print(i,cmd_table.lGameScore[1][i])
        if self.cbPlayStatus[i] == 1 then
            local wViewChairId = self:SwitchViewChairID(i - 1)

            self.lCardType[i] = cmd_table.cbCardType[1][i]
            sartDelayTime = sartDelayTime + 1
        end
        --self.cbPlayStatus[i] = 0
    end

    local cbDelayOverGame = cmd_table.cbDelayOverGame

    -- 清除搓牌中头像状态
    for i=1,cmd.GAME_PLAYER do
        local viewid = self:SwitchViewChairID(i-1)
        if self._gameView.m_tabUserHead[viewid]  then
            self._gameView.m_tabUserHead[viewid]:setHeadStatus(cmd.US_Normal)
        end
    end

    --开牌动画
    local actionList = {}

    local function insertAnim(i)
        local viewid = self:SwitchViewChairID(i - 1)
        if (self.cbCardData and #self.cbCardData>0) then
            if self.cbCardData[i][1] > 0 then
                self._gameView:stopCardAni(viewid)
                table.insert(actionList,cc.DelayTime:create(0.65))
                table.insert(actionList,cc.CallFunc:create(function() print("viewid",viewid)self._gameView:openCardAnimate(viewid,nil,true) end))
            end
        end
    end    
    for i = self.wBankerUser + 2, cmd.GAME_PLAYER do
        insertAnim(i)
    end
    for i = 1 , self.wBankerUser + 1 do
        if i > 8 then break end
        insertAnim(i)
    end
    local lCardType = clone(self.lCardType)
    dump(self.lGameEndScore,"self.lGameEndScore")
    table.insert(actionList,cc.DelayTime:create(0.5))
    table.insert(actionList,cc.CallFunc:create(function() self._gameView:gameEnd(self.lGameEndScore,lCardType) end))
    self:runAction(cc.Sequence:create(actionList))
    --dump(self.cbCardData)



        self._gameView:closeCuoLayer()
        self.m_lAddScoreList = {}

        if self.m_bLookOn then
                return
        end
   
        -- 设置倒计时
        if self.cbDynamicJoin == 0 then 
            self:KillGameClock()
            -- self._gameView:setClockBgVisible(true)
            
            -- self:runAction(cc.Sequence:create(cc.DelayTime:create(6),cc.CallFunc:create(function ()
                -- self:SetGameClock(self:GetMeChairID(), cmd.IDI_START_GAME, cmd.TIME_USER_START_GAME)
            -- end)))
        end

        



   -- self:runAction(cc.Sequence:create(cc.DelayTime:create(10),cc.CallFunc:create(function ()
            
   --          if self._gameView.bIsAuto then --and self:isPlayerPlaying(cmd.MY_VIEWID) then
   --              self._gameView:autoPlay(self._gameView.nAutoStatus)
   --          end

   --      end)))


    self:onRecord()

    self._gameView:SuperCtrlUpdate()
end
--游戏记录
function GameLayer:onSubGameRecord(dataBuffer)
    --print("游戏记录")
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_RECORD, dataBuffer)
     --dump(cmd_data,"====================1111")
    self.m_userRecord.wincount = {}
    self.m_userRecord.losecount = {}
    self.m_userRecord.totalcount = cmd_data.nCount
    for i=1,cmd.GAME_PLAYER do
        self.m_userRecord.wincount[i] = cmd_data.lUserWinCount[1][i]
        self.m_userRecord.losecount[i] = cmd_data.lUserLostCount[1][i]
    end
    
end

--游戏记录（房卡）
function GameLayer:onSubPriGameRecord(dataBuffer)
    print("房卡流水记录")
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_RoomCardRecord, dataBuffer)
    -- dump(cmd_data, "cmd_data")

    --全部成绩
    self.m_userPriRecord = {}
    local scoreList = {}
    local userList = {}
    local nInningsCount = cmd_data.nCount

    for i = 1, cmd.GAME_PLAYER do
        --游戏状态
        --self.cbPlayStatus[i] = cmd_data.cbPlayerStatus[1][i]

         local item = self:getUserInfoByChairID(i-1)
                    table.insert(userList,item)
    end
    --dump(self.cbPlayStatus,"========================222222222")

    for i=1,nInningsCount do
        self.m_userPriRecord[i] ={}
        for j=1,cmd.GAME_PLAYER do 
            --if  self.cbPlayStatus[j] > 0 then  
                self.m_userPriRecord[i][j]=cmd_data.lDetailScore[j][i]
            --end
        end
    end
    --dump(self.m_userPriRecord,"=========================1111111111111")
    
    -- local userIndex = 1   --用户自己列索引    
    -- for j = 1, nInningsCount do
    --     local DetailScore = {}
    --     local recordIndex = 1
    --     for i=1, cmd.GAME_PLAYER do
    --         if self.cbPlayStatus[i] > 0 then  
                
    --            DetailScore[i] = cmd_data.lDetailScore[i][j]
    --            if 1 == j then
    --                local item = self:getUserInfoByChairID(i-1)
    --                table.insert(userList,item)
    --            end
    --         end
            
    --     end
    --     table.insert(scoreList, DetailScore)
    -- end
    -- local data = {_data=scoreList,_index=userIndex}
    -- dump(data,"===================111")
     self._gameView:updataScoreList()
     --self._gameView:updateDetailNick(userList)

  
end


--用户状态
function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)

    if self.isPriOver then
        return 
    end
    --print("change user " .. useritem.wChairID .. "; nick " .. useritem.szNickName .. "; newstatus " ..newstatus.cbUserStatus .."; oldstatus " ..oldstatus.cbUserStatus )
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then

        --if (oldstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            --return
        --end
        -- print("退出change user " .. useritem.wChairID .. "; nick " .. useritem.szNickName,self.cbPlayStatus[useritem.wChairID + 1])
        
            for i=1, cmd.GAME_PLAYER do
                if self.m_tabUserItem[i] and self.m_tabUserItem[i].dwUserID == useritem.dwUserID then
        print("退出change user " .. useritem.wChairID .. "; nick " .. useritem.szNickName,self.cbPlayStatus[useritem.wChairID + 1])
                    if self._gameView.bActComplete == false and self.cbPlayStatus[self.m_tabUserItem[i].wChairID + 1] == 1 then
                        local act = cc.RepeatForever:create(
                            cc.Sequence:create(
                                cc.DelayTime:create(1),
                                cc.CallFunc:create(function()
                                    if self._gameView.bActComplete == true then
                                        self:stopActionByTag(useritem.dwUserID)
                                        self.m_tabUserItem[i] = nil
                                        self._gameView:OnUpdateUserExit(useritem)
                                    end
                                end)))
                        act:setTag(useritem.dwUserID)

                        self:runAction(act)
                    else
                        self.m_tabUserItem[i] = nil
                        self._gameView:OnUpdateUserExit(useritem)
                    end
                end
            end
            -- self._gameView:OnUpdateUserExit(useritem)
           
            
    else
        --print("改变状态")
        if (newstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            --print("tableID is error")
            return
        end


        if newstatus.cbUserStatus == yl.US_LOOKON then
                return
        end


        self.m_tabUserItem[useritem.wChairID+1] = clone(useritem)
        local viewid = self:SwitchViewChairID(useritem.wChairID)
        
        --print("更新用户资料 wChairID,viewid,szNickName",useritem.wChairID,viewid,useritem.szNickName)
        self._gameView:OnUpdateUser(viewid,useritem)
        if newstatus.cbUserStatus == yl.US_READY then
            self._gameView:setReadyVisible(viewid, true)

            if viewid == cmd.MY_VIEWID then
                self._gameView:setClockBgVisible(false)
            end
        end

        -- print("******************************************")
        -- print(newstatus.cbUserStatus, oldstatus.cbUserStatus,cmd.US_Off )
        -- print("******************************************")
        if newstatus.cbUserStatus ~= oldstatus.cbUserStatus then
            if newstatus.cbUserStatus == yl.US_OFFLINE then
                self._gameView:setUserHeadStatus(viewid, cmd.US_Off)
            elseif oldstatus.cbUserStatus == yl.US_OFFLINE then
                self._gameView:setUserHeadStatus(viewid, cmd.US_Normal)
            end
        end
    end    
end

--用户进入
function GameLayer:onEventUserEnter(tableid,chairid,useritem)
    self._gameView:showAdressLayer(false)

    print("the table id is ================>"..tableid.." chairid==>"..chairid)

    --dump(useritem,"the item is ")
    if self.isPriOver then
        return 
    end

    if useritem.cbUserStatus == yl.US_LOOKON then
            return
    end

  --刷新用户信息
    if (tableid ~= self:GetMeUserItem().wTableID and not GlobalUserItem.bVideo) then

            for i=1, cmd.GAME_PLAYER do
                if self.m_tabUserItem[i] and self.m_tabUserItem[i].dwUserID == useritem.dwUserID then
                        self.m_tabUserItem[i] = nil
                end
            end
            self._gameView:OnUpdateUserExit(useritem)
        return
    end

    if useritem then
        self.m_tabUserItem[useritem.wChairID+1] = clone(useritem)
    end

    local wViewChairId = self:SwitchViewChairID(chairid)
    self._gameView:OnUpdateUser(wViewChairId, useritem, true)
    if useritem.cbUserStatus == yl.US_READY then
        self._gameView:setReadyVisible(wViewChairId, true)
    end

    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end
end

--用户分数
function GameLayer:onEventUserScore( item )
    if item.wTableID ~= self:GetMeUserItem().wTableID then
       return
    end

    local viewid = self:SwitchViewChairID(item.wChairID)
    if self._gameView.m_tabUserHead[viewid] then
        self._gameView.m_tabUserHead[viewid].m_userItem.lScore = item.lScore
        self._gameView:updateScore(viewid)
    end
end

--换位操作
function GameLayer:onChangeDesk()
    print("onChangeDesk")
    self._gameFrame:QueryChangeDesk()
end

------------------------------------------------------------------------

------------------------------------------------------------------------
--开始游戏
function GameLayer:onStartGame()
    if true == self.m_bPriScoreLow then
        local msg = self.m_szScoreMsg or ""
        self.m_querydialog = QueryDialog:create(msg,function()
            self:onExitTable()
        end,nil,1)
        self.m_querydialog:setCanTouchOutside(false)
        self.m_querydialog:addTo(self)
    else
       
        --self:KillGameClock()
        self._gameView:setClockBgVisible(false)
        self._gameView:onResetView()
        self._gameFrame:SendUserReady()

        self.m_bStartGame = true
    end

end

function GameLayer:getPlayNum()
    local num = 0
    for i = 1, cmd.GAME_PLAYER do
        if self.cbPlayStatus[i] == 1 then
            num = num + 1
        end
    end

    return num
end

function GameLayer:sendCardFinish()
    if self.cbDynamicJoin == 1 or GlobalUserItem.bVideo then 
        return
    end 

    --self._gameView:setClockBgVisible(true)
    --self:KillGameClock()
    if not self.bAddScore then 
        --self:SetGameClock(self:GetMeChairID(), cmd.IDI_TIME_USER_ADD_SCORE, cmd.TIME_USER_ADD_SCORE)
        return
    end
   -- self:SetGameClock(self:GetMeChairID(), cmd.IDI_TIME_OPEN_CARD, cmd.TIME_USER_OPEN_CARD)
end

--开单张牌
function GameLayer:openOneCard(viewid,index,bEnd)--, bEnded)
    local chairId = self:SwitchChairID(viewid)


    local temp = index

    if GlobalUserItem.bVideo and viewid == 4 and self.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then

             if temp ~= 1 then
                 temp = temp - 1
             else
                 temp = 5
             end

    end

    if not self.cbCardData[chairId] then
        return  false
    end

    if self.cbCardData[chairId][temp] == 0 then
        return false
    end



    local data = self.cbCardData[chairId][temp]
 
    local value = GameLogic:getCardValue(data)
    local color = GameLogic:getCardColor(data)
    local card = self._gameView.nodeCard[viewid][index]
    self._gameView:setCardTextureRect(viewid, index, value, color)

    if bEnd and (index == cmd.MAX_CARDCOUNT) then
        print("******************弹出第五张牌")
        self._gameView:resetCardByType(clone(self.cbCardData),clone(self.lCardType),viewid)
    end

    return true
end



function GameLayer:getMeCardLogicValue(num)
    local index = self:GetMeChairID() + 1
    local value = GameLogic:getCardLogicValue(self.cbCardData[index][num])
    local str = string.format("index:%d, num:%d, self.cbCardData[index][num]:%d, return:%d", index, num, self.cbCardData[index][num], value)
    --print(str)
    return value
end

function GameLayer:getMeCardValue( index )
    local chairID = self:GetMeChairID() + 1
    if index == nil then
       return self.cbCardData[chairID]
    end
   
    local value = self.cbCardData[chairID][index]
    return value 
end

function GameLayer:getOxCard(cbCardData)
    return GameLogic:getOxCard(cbCardData)
end

function GameLayer:getPrivateRoomConfig()
    return self.m_tabPrivateRoomConfig
end
--约战记录
function GameLayer:getDetailScore()
    return self.m_userRecord
end

--********************   发送消息     *********************--
--叫庄
function GameLayer:onBanker(cbBanker,mul)

    local dataBuffer = CCmd_Data:create(2)
    dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME,cmd.SUB_C_CALL_BANKER)
    dataBuffer:pushbyte(cbBanker)
    --dump({cbBanker},"onBanker=============")
    dataBuffer:pushbyte(mul)
    local result = self._gameFrame:sendSocketData(dataBuffer)
    print("game cmd =====",game_cmd.MDM_GF_GAME)
    return result
end

function GameLayer:onRecord()
        local cmd_data = CCmd_Data:create()
    
       return self:SendData(cmd.SUB_C_RECORD, cmd_data)
end

function GameLayer:onAddScore(lScore)
    --print("牛牛 发送下注 lScore",lScore)
    if lScore == nil then
        error("send lScore is nil !")
    end
    if self:SwitchViewChairID(self.wBankerUser) == cmd.MY_VIEWID then
        --print("牛牛: 自己庄家不下注")
        return
    end
    local dataBuffer = CCmd_Data:create(8)
    dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_ADD_SCORE)
    dataBuffer:pushscore(lScore)
    return self._gameFrame:sendSocketData(dataBuffer)
end

--发送开牌消息
function GameLayer:onOpenCard(data)

    local dataBuffer = CCmd_Data:create(5)
    dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_OPEN_CARD)
    for i=1,#data do
       dataBuffer:pushbyte(data[i])
    end

    return self._gameFrame:sendSocketData(dataBuffer)
end

--发送执行调试
function GameLayer:sendDebugConfig(data)

    local dataBuffer = CCmd_Data:create()
    dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_CONFIG_DEBUG)
    for i=1,#data do
        if i == 1 or i == #data then
            dataBuffer:pushword(data[i])
        else
            dataBuffer:pushbyte(data[i])
        end
    end

    return self._gameFrame:sendSocketData(dataBuffer)
end

--请求牌型数据
function GameLayer:sendRequestCardType(chair, cardType)

    local dataBuffer = CCmd_Data:create()
    dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_REQUEST_CARDTYPE)

    dataBuffer:pushword(chair)
    dataBuffer:pushbyte(cardType)

    return self._gameFrame:sendSocketData(dataBuffer)
end


--请求剩余牌
function GameLayer:sendRequestCardPool()
    print("请求剩余牌")
    local dataBuffer = CCmd_Data:create(0)
    dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_GET_POOL_CARD)

    return self._gameFrame:sendSocketData(dataBuffer)
end

--请求取消调试
function GameLayer:sendCancelDebug(curDebugChairID)

    local dataBuffer = CCmd_Data:create()
    dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_CONFIG_CANCEL)

    dataBuffer:pushword(curDebugChairID)

    for i=1,8 do
        dataBuffer:pushbyte(0)
    end

    dataBuffer:pushword(0)

    return self._gameFrame:sendSocketData(dataBuffer)
end

--请求当前调试状态
function GameLayer:requestCurDebugConfig(curDebugChairID)

    local dataBuffer = CCmd_Data:create(12)
    dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_REQUEST_CURCONFIG)

    dataBuffer:pushword(curDebugChairID)

    for i=1,8 do
        dataBuffer:pushbyte(0)
    end

    dataBuffer:pushword(0)

    return self._gameFrame:sendSocketData(dataBuffer)
end

---------------------------------------------------------------

function GameLayer:getChairCount()
    return table.nums(self.m_tabUserItem)
end

--获取用户数据
function GameLayer:getUserInfoByChairID(wchairid)
    for k,v in pairs(self.m_tabUserItem) do
        if v.wChairID == wchairid then
            return v
        end
    end
    return nil
end

--获取用户数据
function GameLayer:getUserInfoByUserID(dwUserID)
    for k,v in pairs(self.m_tabUserItem) do
        if v.dwUserID == dwUserID then
            return v
        end
    end
    return nil
end

--获取用户数据
function GameLayer:getUserInfoByGameID(dwGameID)
    for k,v in pairs(self.m_tabUserItem) do
        if v.dwGameID == dwGameID then
            return v
        end
    end
    return nil
end

function GameLayer:saveUserInfos(tabUserItem)
    self._SaveInfo = clone(tabUserItem)
end
function GameLayer:getSaveUserInfos(wchairid)
    for k,v in pairs(self.m_tabUserItem) do
        if v.wChairID == wchairid then
            return v
        end
    end
    return self.m_tabUserItem[wchairid+1]
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
        
        local tip = (self.m_cbGameStatus > 0 and self.cbPlayStatus[self._MyChairID + 1] == 1) and "现在退出游戏会由系统托管代打哟，输了可不能怪他哦！" or "您要退出游戏么？"
       
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