--
-- Author: Tang
-- Date: 2016-12-08 15:41:53
--
local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local module_pre = "game.qipai.thirteenzj.src"
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(module_pre..".models.cmd_game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
local GameViewLayer = appdf.req(module_pre .. ".views.layer.GameViewLayer")
local QueryDialog   = appdf.req("app.views.layer.other.QueryDialog")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

function GameLayer:ctor( frameEngine,scene )        
    GameLayer.super.ctor(self, frameEngine, scene)
    self:OnInitGameEngine()
    self._roomRule = self._gameFrame._dwServerRule
    self.m_bLeaveGame = false  
    self._clockTimeLeave = 0
    self._endCallBack = nil
    self._wBankerUser = yl.INVALID_CHAIR

    self._bUserGameStatus = {false,false,false,false}

    self._cbHandCard = {}  --13张自己手牌数据
    self._sortedCard = {}  --选牌结束后的扑克列表
    self._scoreTimes = {}  --每段比分
    self._scoreEnd = {} --结算分数
    self._selfscoreEnd = 0  --自己的结算分数
    self._bSpecialType = {} --是否特殊牌型
    self._cbautoCard = {}   --自动出牌
    self._bDetailAll = {}   --单蹦
    self._cbAllDetaiUser = yl.INVALID_CHAIR --通蹦玩家
    self._bHaveBanker = 0   --是否霸王庄模式
    self._cbMaCardData = 0   -- --0,0x31,0x3D,0x3A  无码牌 A K 10
    self._nAllWinTimes = 0      --额外打枪模式

    self._lScoreCell = 1

    self._finishSelect = {}
    --约战需要
    self.m_tabUserItem = {}
    self.m_bjoinGame = false    --当前是否参与游戏
    self.m_bPriEnd = false      --是否有约战结算
    self.m_isGameEnd = false

    self.m_bIsQuick = true

    self._bIsEndLayer = false --是否已显示结算界面

    self._enterMessage = cmd.GS_WK_FREE

    self._wchair = self:GetMeUserItem().wChairID
    self.m_tabUserResult = {}

    self.m_tonghuaFlag = 0

    self.m_curPlayCount = 0
    self.lScoreTimesPlus ={}

    self.m_bLookOn = false
    self.m_playingUser = {}

     --游戏记录
    self.m_recordList = {}    --游戏记录
    if false == GlobalUserItem.bVideo then
        self._gameFrame:QueryUserInfo(self:GetMeUserItem().wTableID,yl.INVALID_CHAIR)
    end
end

--获取gamekind
function GameLayer:getGameKind()
    return cmd.KIND_ID
end

--创建场景
function GameLayer:CreateView()
     self._gameView = GameViewLayer:create(self)
     self:addChild(self._gameView)
     return self._gameView
end

function GameLayer:getParentNode( )
    return self._scene
end

function GameLayer:getFrame( )
    return self._gameFrame
end

function GameLayer:logData(msg)
    if nil ~= self._scene.logData then
        self._scene:logData(msg)
    end
end

function GameLayer:reSetData()
    self._cbautoCard = {}   --自动出牌
end

--相对索引 return 1-6
function GameLayer:getUserIndex( wChairID )
    local viewIndex = cmd.InvalidIndex
    local MyChair = self:GetMeUserItem().wChairID --self._wchair
    if MyChair ~=  yl.INVALID_CHAIR then
        self._wchair = MyChair
    else
        MyChair = self._wchair
    end
    viewIndex = math.mod((wChairID-MyChair+cmd.GAME_PLAYER),cmd.GAME_PLAYER)
    return  viewIndex+1
end

--相对索引座位 return 0-5
function GameLayer:getUserChair( viewIndex )
    local MyChair = self._wchair --self:GetMeUserItem().wChairID
    return  math.mod(viewIndex-1+MyChair,cmd.GAME_PLAYER)
end

function GameLayer:getSitUserInfoByChairID(wchairid)
    for k,v in pairs(self.m_tabUserItem) do
        --local usergamestatus = self._bUserGameStatus[chairId] or false
        if v.wChairID == wchairid then
            return v
        end
    end
    return nil
end

---------------------------------------------------------------------------------------
------继承函数
function GameLayer:onEnterTransitionFinish()
    GameLayer.super.onEnterTransitionFinish(self)
end

function GameLayer:onExit()
    self:KillGameClock()
    self:dismissPopWait()
    GameLayer.super.onExit(self)
    print("GameLayer onExit")
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
                    self._gameFrame:StandUp(1)
                end
                ),
            cc.DelayTime:create(10),
            cc.CallFunc:create(
                function ()
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
    self._scene:onKeyBack()
end

function GameLayer:onLeavetRoom()
    self._gameFrame:onCloseSocket()
    self:stopAllActions()
    self:KillGameClock()
    self:dismissPopWait()
    self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
end

function GameLayer:OnEventGameClockInfo(chair,clocktime,clockID)
    if nil ~= self._gameView  and self._gameView.UpdataClockTime then
        self._gameView:UpdataClockTime(clockID,clocktime)
    end
end

-- 设置计时器
function GameLayer:SetGameClock(chair,id,time)
    self:KillGameClock()
    GameLayer.super.SetGameClock(self,chair,id,time)
end

function GameLayer:OnResetGameEngine()
    print("OnResetGameEngine")
    self.m_bOnGame = false
    self:reSetData() 
    GameLayer.super.OnResetGameEngine(self)
    self:stopAllActions()
    self._gameView:gameClean()
end

--比牌完成
function GameLayer:sendCompleteCompare()
   local cmddata = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_COMPLETE_COMPARE, cmddata)
    if self._endCallBack ~= nil then
       --self._endCallBack()
    end
    print("比牌完成11111")
end

-- 发送托管
function GameLayer:sendRobot( value )
    local cmddata = CCmd_Data:create(1)
    cmddata:pushbyte(value)
    self:SendData(cmd.SUB_C_TRUSTEE,cmddata)
end

--系统消息
function GameLayer:onSystemMessage( wType,szString )
    if wType == 501 or wType == 51 then
        print("十三水游戏币不足消息", szString)
        local msg = szString or "你的游戏币不足，无法继续游戏"
        local query = QueryDialog:create(msg, function(ok)
                if ok == true then
                    if not self.m_bClubRoom then 
                        self:onExitTable()
                    end 
                end
            end):setCanTouchOutside(false)
                :addTo(self)
    end
end

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode(cmd.RES_PATH.."game_res/ShareLayer.csb"):addTo(self)
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




-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)
    print("场景数据:" .. cbGameStatus)
    
    for i=1,cmd.GAME_PLAYER do
        local userbg = self._gameView._rootNode:getChildByName(string.format("user_%d", i))
        self._gameView._rootNode:getChildByName("txt_ready_"..i):setVisible(false)
        if userbg and i ~= cmd.ME_VIEW_CHAIR then
           userbg:setVisible(false)
        end
    end

    dump(self:GetMeUserItem(),"self:GetMeUserItem()")
    local myUserItem = self:GetMeUserItem()
    if myUserItem.cbUserStatus ~= yl.US_LOOKON then 
        self._gameView:showUserInfo(self:GetMeUserItem())
        self.m_tabUserItem[self:GetMeChairID()+1]=clone(self:GetMeUserItem())
        self.m_bLookOn = false
    else
        self.m_bLookOn = true
    end

    if self.m_bOnGame then
        if PriRoom and GlobalUserItem.bPrivateRoom then
            if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
                PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
                self._gameView._priView:onRefreshInfo()
            end
        end
        -- return
    end

    self.m_bOnGame = true
    self._gameView:gameClean()
    self.m_cbGameStatus = cbGameStatus
    

    --初始化已有玩家   
    local tableId = self._gameFrame:GetTableID()
    for i = 1, cmd.GAME_PLAYER do

        -- if i ~= self:GetMeChairID()+1 then
            self._gameFrame:QueryUserInfo(tableId, i-1)
        -- end

    end
    self.m_tabUserResult[self:GetMeChairID()+1]=clone(self:GetMeUserItem())
    
    -- print("--------------8888888888888--------------")
    -- for i = 1, cmd.GAME_PLAYER do
    --     local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
    --     dump(userItem,"---------userItem-----场景----1111-----",6)
    --     if nil ~= userItem then
    --         self._gameView:showUserInfo(userItem)
    --         if PriRoom then
    --             PriRoom:getInstance():onEventUserState(wViewChairId, userItem, false)
    --         end
    --     end
    -- end

    if cbGameStatus == cmd.GS_WK_FREE  then                        --空闲状态
        self:onEventGameSceneFree(dataBuffer)
    elseif cbGameStatus == cmd.GS_WK_PLAYING then                
        self:onEventGameSceneStatus(dataBuffer)
    end

    self._enterMessage = cbGameStatus
    self._gameView:setRoomInfo()
    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end
    if GlobalUserItem.bPrivateGoldRoom then
        for i=1,cmd.GAME_PLAYER do
            self._gameView._rootNode:getChildByName(string.format("user_%d",i)):getChildByName("icon_fangzhu"):setVisible(false)
        end
    end

    self:dismissPopWait()

    if GlobalUserItem.dwCurRoomGroupID ~= 0 and GlobalUserItem.bPrivateGoldRoom then 
        self.m_bClubRoom = true
    end
    
    local nChairCount = 0
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        if nil ~= userItem then
            nChairCount = nChairCount +1
        end
    end
    if self.m_bLookOn then
        if nChairCount >= cmd.GAME_PLAYER then
            return;
        end
        -- self._gameView:showSitBtn(true)
    end
    self._gameView:showSitBtn(self.m_bLookOn)
end

function GameLayer:onEventGameSceneFree( dataBuffer )
    print("空闲场景")
    local  free_data = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
    dump(free_data, "the free data is =========== > ", 6)

    self.m_tTimeStartGame = free_data.cbTimeStartGame or 30
    self.m_tTimeShowCard  = free_data.cbTimeRangeCard  or 60
    --self.m_tTimeShowCard  = 10
    self._cbPlayCount = free_data.cbPlayCount

    self.m_tonghuaFlag = free_data.cbTongHuaMode

    self._lScoreCell = free_data.lBaseScore

    self.m_bIsQuick = free_data.cbQuickMode

        --游戏模式
    self.cbPlayMode = free_data.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1 or false
    self.m_isGameEnd = false
    self._bIsEndLayer = true
    --显示准备
    self._gameView:showReadyBtn(true)
    self._gameView:setBaseScore(free_data.lBaseScore)
    -- if GlobalUserItem.bPrivateRoom == true and PriRoom  and not GlobalUserItem.bPrivateGoldRoom then
              
    -- else
        self:SetGameClock(self:GetMeUserItem().wChairID,cmd.Clock_free, self.m_tTimeStartGame)
    --end
    
    self._bHaveBanker = free_data.bHaveBanker --霸王庄模式
    print("self._bHaveBanker ..",self._bHaveBanker)
    self._cbMaCardData = free_data.cbMaCardData
    self._nAllWinTimes = free_data.nAllWinTimes --额外打枪
end

function GameLayer:onEventGameSceneStatus( dataBuffer )
    print("游戏场景")
    local status = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer)
    dump(status, "the status data is ==============", 6)
    
    self.m_tTimeStartGame = status.cbTimeStartGame or 30
    self.m_tTimeShowCard  = status.cbTimeRangeCard  or 60
    self.m_tTimeShowCard  =  status.cbTimeLeave or 30
    
    self._timeShow = status.cbTimeRangeCard  or 60

    self._lScoreCell = status.lBaseScore

    self.m_bIsQuick = status.cbQuickMode

    --变量赋值
    self._cbHandCard = status.bHandCardData[1]
    self._bUserGameStatus = status.bGameStatus[1]

    self.m_tonghuaFlag = status.cbTongHuaMode

    self._bHaveBanker = status.bHaveBanker --霸王庄模式
    self._cbMaCardData = status.cbMaCardData
    self._nAllWinTimes = status.nAllWinTimes --额外打枪
    self.m_isGameEnd = false
    self._bIsEndLayer = false
    --游戏模式
    self.cbPlayMode = status.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1 or false

    --清空视图
    self._gameView:gameClean()
    self._gameView:setBaseScore(status.lBaseScore)
    self._gameView:showReadyBtn(false)

    self._wBankerUser = status.wBanker
    self._cbPlayCount = status.cbPlayCount
    self.lScoreTimesPlus = status.lScoreTimesPlus

    --扑克信息
    local dispatchInfo = {}
    dispatchInfo.playerIndex = {}
    dispatchInfo.playerCount = status.cbPlayCount
    dispatchInfo.cardCount   = cmd.HAND_CARD_COUNT
    for i=1,#self._bUserGameStatus do
        local bGameStatus = self._bUserGameStatus[i]
        if true == bGameStatus then
          local viewIndex = self:getUserIndex(i-1)
          table.insert(dispatchInfo.playerIndex, viewIndex)
        end
     end 

    self.m_wAllKillUser =  status.wAllKillUser
    self.m_bAllWin = clone(status.bAllWin)

    --完成分段
    self._finishSelect = status.bFinishSegment[1]
   
    local bPopSelectView = false
    if true == self._bUserGameStatus[self:GetMeUserItem().wChairID+1] and not self._finishSelect[self:GetMeUserItem().wChairID+1] then 
        bPopSelectView = true
    else   
         self:SetGameClock(self:GetMeUserItem().wChairID,cmd.Clock_status, self.m_tTimeShowCard)  
    end
    --是否游戏中
    self.m_bjoinGame = self._bUserGameStatus[self:GetMeUserItem().wChairID+1]

    self.m_cbGameStatus = self.m_bjoinGame and cmd.GS_WK_PLAYING or cmd.GS_WK_FREE

    local myChair = self:GetMeUserItem().wChairID
    local lScoreTimes = {}
    if myChair == 0 then
        lScoreTimes = status.lScoreTimes1
    elseif myChair == 1 then
        lScoreTimes = status.lScoreTimes2
    elseif myChair == 2 then
        lScoreTimes = status.lScoreTimes3
    elseif myChair == 3 then
        lScoreTimes = status.lScoreTimes4
    end



    self._gameView:dispatchCard(dispatchInfo,false,bPopSelectView)
     --设置分段数据bPopSelectView
    local bcompare = true
    local viewUser = {}
    self.m_curPlayCount = 0
    for i=1,cmd.GAME_PLAYER do
        local datas = {}
        local chair = {1,4,3,2}  --客户要求，0号椅子逆时针转
        if chair[i] == 1 then
            datas = clone(status.bSegmentCard1)
        elseif chair[i] == 2 then
            datas = clone(status.bSegmentCard2)
        elseif chair[i] == 3 then
            datas = clone(status.bSegmentCard3)
        elseif chair[i] == 4 then
            datas = clone(status.bSegmentCard4)
        end
        local userIndex = self:getUserIndex(chair[i]-1)
        self._bSpecialType[userIndex] = 0
        if  self._bUserGameStatus[chair[i]] then
            self.m_curPlayCount = self.m_curPlayCount +1
            if true == self._finishSelect[chair[i]] then
                local cardInfo = {}
                cardInfo.Front = {datas[1][1],datas[1][2],datas[1][3]}
                cardInfo.Mid   = {datas[2][1],datas[2][2],datas[2][3],datas[2][4],datas[2][5]}
                cardInfo.Tail  = {datas[3][1],datas[3][2],datas[3][3],datas[3][4],datas[3][5]}
                self._sortedCard[userIndex] = cardInfo

                local cardData = {}
                for k,v in pairs(cardInfo.Front) do
                    table.insert(cardData, v)
                end
                for k,v in pairs(cardInfo.Mid) do
                    table.insert(cardData, v)
                end
                for k,v in pairs(cardInfo.Tail) do
                    table.insert(cardData, v)
                end

                self:runAction(cc.Sequence:create(cc.DelayTime:create(0.2), cc.CallFunc:create(function()
                    self._gameView:showSelectedCard(userIndex, cardData)
                    end)))
            else
                print("选牌未完成", userIndex)
                self._gameView:showSelectStatus(userIndex,cmd.GS_WK_SETCHIP - cmd.GS_WK_CALL_BANKER)    --显示选牌中
                self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),cc.CallFunc:create(function()
                    self._gameView:showSelectAction(userIndex)
                end)))
                
                bcompare = false
            end
            self._scoreTimes[userIndex] = lScoreTimes[chair[i]]
            self._bSpecialType[userIndex] = status.bSpecialType[1][chair[i]]
            if status.bSpecialType[1][chair[i]] == 0 then
                table.insert(viewUser,userIndex) 
            end
        end
    end
    --需要比牌
    if bcompare == true then
        print("需要比牌", bcom)
        self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3), cc.CallFunc:create(function()
                self._gameView:compareCard(viewUser,status)
            end)))
    end
end



function GameLayer:getGameRule(buffer)

        -- buffer:readbyte()
        -- buffer:readbyte()
        -- buffer:readbyte()

        local strTable ={}
        --游戏模式(玩法)
        local gameMode =  buffer[3]--buffer:readbyte()
        local gun = buffer[4]
        local ma = buffer[5]
        local maTxt = {"无码牌","马牌A" ,"马牌10","马牌5"}
        --dump({gameMode,gun,ma},"============================")
        
        table.insert(strTable,gameMode == 0 and "经典通比(无庄家)" or "庄家比模式(霸王庄)")
        --table.insert(strTable,gun == 1 and "打枪加分+1" or "打枪加分+2")
        table.insert(strTable,maTxt[ma+1])

    return strTable
end


function GameLayer:onEventGameMessage(sub,dataBuffer)
    if nil == self._gameView then
        return
    end

    if sub == cmd.SUB_S_SHOW_CARD then                       --玩家摊牌
        self:OnSubShowCard(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_START  then                     --游戏开始
        self:onSubGameStart(dataBuffer)
    elseif sub == cmd.SUB_S_PLAYER_EXIT then                     --用户强退
        self:OnSubPlayerExit(dataBuffer)  
    elseif sub == cmd.SUB_S_SEND_CARD_EX then                    --开始发牌
         self.m_cbGameStatus = cmd.GS_WK_PLAYING
                
        self:OnSubDispatchCard(dataBuffer)
    elseif sub == cmd.SUB_S_COMPARE_CARD then                    --比较扑克
        self:OnSubCompareCard(dataBuffer) 
    elseif sub == cmd.SUB_S_GAME_END then                        --游戏结束
        self.m_cbGameStatus = cmd.GS_WK_FREE
        self:OnSubGameEnd(dataBuffer)   
    elseif sub == cmd.SUB_S_TRUSTEE then                         --玩家托管
        self:OnSubTrustTee(dataBuffer)                                     
    elseif sub == cmd.SUB_S_MOBILE_PUTCARD then                 --扑克分析(最优牌型和特殊牌型)
        self._cbautoCard = ExternalFun.read_netdata(cmd.CMD_S_MobilePutCard, dataBuffer)
       -- dump(self._cbautoCard, "手机托管发牌", 10)
        if true == self._bUserGameStatus[self:GetMeUserItem().wChairID+1] and not self._finishSelect[self:GetMeUserItem().wChairID+1] then 
            self._gameView:SpecialCardInfo()   
        end

    elseif sub == cmd.SUB_S_RECORD then                            --游戏记录
        self.m_recordList =  ExternalFun.read_netdata(cmd.CMD_S_Record, dataBuffer)
		--local len = dataBuffer:getlen() 
        --dump(self.m_recordList, "游戏记录", 10)        
    elseif sub == cmd.SUB_S_GAME_STATUS then                            --动态开始
        print("---------动态开始------3333333333333--------------")
        local gameStatus = dataBuffer:readbyte()
        if gameStatus == 1 then 
            self._gameView:gameClean()
            if PriRoom and GlobalUserItem.bPrivateRoom then
                if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
                    PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
                    self._gameView._priView:onRefreshInfo()
                end
            end
            self._gameView:showSitBtn(false)
        end      
    end
    print("onEventGameMessage ======== >"..sub)
end

-- 游戏开始
function GameLayer:onSubGameStart(dataBuffer)
    print("游戏开始")
    self._gameView:gameClean()

    self._gameView:showReadyBtn(false)
    for i=1,cmd.GAME_PLAYER do
        self._gameView:showReady(i,false)
    end
end

--显示开始游戏按钮
function GameLayer:onEventShowBegin(dwWaitTime)
    print("----------显示开始游戏按钮-------------")
    -- self._gameView:showSitBtn(true, dwWaitTime)
    self._gameView:showSitBtn(dwWaitTime~=0 and true or false,dwWaitTime)
end

function GameLayer:onPlayingUserInfo(userInfo)
    self.m_playingUser = {}
    for i=1,16 do
        self.m_playingUser[i] = userInfo.dwUserID[1][i]
    end
end

function GameLayer:OnSubTrustTee(dataBuffer)
     local trustTee = ExternalFun.read_netdata(cmd.CMD_S_Trustee, dataBuffer)
     local item = self:GetMeUserItem()
     if trustTee.wChairID == item.wChairID then
         return
     end

     local userIndex = self:getUserIndex(trustTee.wChairID)
     local bTrustee  = trustTee.bTrustee
     self._gameView:trustTeeDeal(userIndex,bTrustee)
    self._gameView:showReadyBtn(false)
end

function GameLayer:OnSubDispatchCard( dataBuffer )
    print("开始发牌")
    local send_data = ExternalFun.read_netdata(cmd.CMD_S_SendCard, dataBuffer)
    --dump(send_data, "the send card data is ", 6)
    self._gameView:gameClean()
    self._gameView.btn_game_start:setVisible(false)

    self._gameView:showReadyBtn(false)
    for i=1,cmd.GAME_PLAYER do
        self._gameView:showReady(i-1,false)
    end
    self._enterMessage = cmd.GS_WK_FREE
    self._finishSelect = {}
    self.m_tabUserResult = clone(self.m_tabUserItem)
    self.m_isGameEnd = false
    self._bIsEndLayer = false

    self._cbHandCard = send_data.bCardData[1]
    self._bUserGameStatus = send_data.bGameStatus[1]
    self.m_bjoinGame = self._bUserGameStatus[self:GetMeUserItem().wChairID+1]
    self._wBankerUser = send_data.wBanker
    if self._wBankerUser ~= yl.INVALID_CHAIR then
        local userIndex = self:getUserIndex(self._wBankerUser)
        self._gameView:showBankerIcon(userIndex)
    end
    print("当前庄家", self._wBankerUser)

    self.m_curPlayCount = send_data.cbPlayCount
    --发牌信息
    local dispatchInfo = {}
    dispatchInfo.playerIndex = {}
    dispatchInfo.playerCount = send_data.cbPlayCount
    dispatchInfo.cardCount   = cmd.HAND_CARD_COUNT
    for i=1,#self._bUserGameStatus do
        local bGameStatus = self._bUserGameStatus[i]
        if bGameStatus then
          local viewIndex = self:getUserIndex(i-1)
          table.insert(dispatchInfo.playerIndex, viewIndex)
        end
     end 

    self._gameView:dispatchCard(dispatchInfo,true,true)


    for i=1,cmd.GAME_PLAYER do
       self._gameView:showReady(i, false)
    end

    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
            self._gameView._priView:onRefreshInfo()
        end
    end
end


function GameLayer:OnSubShowCard(dataBuffer)
    print("玩家摊牌")
   local showcardBuff = ExternalFun.read_netdata(cmd.CMD_S_ShowCard, dataBuffer)
   --dump(showcardBuff, "ShowCard data is =============== > ", 6)
   local user = showcardBuff.wCurrentUser

   if user == self:GetMeUserItem().wChairID  then   --过滤自己
          if self._gameView._selectLayer then
              self._gameView._selectLayer:removeFromParent()
              self._gameView._selectLayer = nil
          else
            if not self.m_bLookOn then
                return
            end
          end
      --return
   end

   local userindex = self:getUserIndex(user)
   
   --玩家摊牌数据
   local cardInfo = {}
   cardInfo.Front = showcardBuff.bFrontCard[1]
   cardInfo.Mid   = showcardBuff.bMidCard[1]
   cardInfo.Tail  = showcardBuff.bBackCard[1]
   self._sortedCard[userindex] = cardInfo
   local cardData = {}
   for k,v in pairs(cardInfo.Front) do
       table.insert(cardData, v)
   end
   for k,v in pairs(cardInfo.Mid) do
       table.insert(cardData, v)
   end
   for k,v in pairs(cardInfo.Tail) do
       table.insert(cardData, v)
   end
   self._gameView:showSelectedCard(userindex, cardData)
end

function GameLayer:OnSubCompareCard(dataBuffer) 
    print("开始比牌")
    --清空自动出牌数据
    self._cbautoCard = {}
    local compareCardBuff = ExternalFun.read_netdata(cmd.CMD_S_CompareCard, dataBuffer)
    self._wBankerUser = compareCardBuff.wBanker

    self._finishSelect = {}
    dump(compareCardBuff,"=====================1")
    if self._wBankerUser ~= yl.INVALID_CHAIR then
        local userIndex = self:getUserIndex(self._wBankerUser)
        self._gameView:showBankerIcon(userIndex)
    end

    self.m_wAllKillUser =  compareCardBuff.wAllKillUser
    self.m_bAllWin = clone(compareCardBuff.bAllWin)

    --比牌数据
    for i=1,cmd.GAME_PLAYER do
        local datas = {}
        if i == 1 then
            datas = clone(compareCardBuff.bSegmentCard1)
        elseif i == 2 then
            datas = clone(compareCardBuff.bSegmentCard2)
        elseif i == 3 then
            datas = clone(compareCardBuff.bSegmentCard3)
        elseif i == 4 then
            datas = clone(compareCardBuff.bSegmentCard4)
        end
        local userIndex = self:getUserIndex(i-1)
        if self._bUserGameStatus[i] and cmd.ME_VIEW_CHAIR ~= userIndex then
            --玩家摊牌数据
           local cardInfo = {}
           cardInfo.Front = {datas[1][1],datas[1][2],datas[1][3]}
           cardInfo.Mid   = {datas[2][1],datas[2][2],datas[2][3],datas[2][4],datas[2][5]}
           cardInfo.Tail  = {datas[3][1],datas[3][2],datas[3][3],datas[3][4],datas[3][5]}
           self._sortedCard[userIndex] = cardInfo

           local cardData = {}
           for k,v in pairs(cardInfo.Front) do
               table.insert(cardData, v)
           end
           for k,v in pairs(cardInfo.Mid) do
               table.insert(cardData, v)
           end
           for k,v in pairs(cardInfo.Tail) do
               table.insert(cardData, v)
           end

           self:runAction(cc.Sequence:create(cc.DelayTime:create(0.1), cc.CallFunc:create(function()
                    self._gameView:showSelectedCard(userIndex, cardData)
                    end)))
           

            --设置玩家手牌数据  
           self._gameView:setSelectCardData(userIndex)
        end
    end

    self.lScoreTimesPlus = compareCardBuff.lScoreTimesPlus
    
    local myChair = self:GetMeUserItem().wChairID
    local lScoreTimes = {}
    if myChair == 0 then
        lScoreTimes = compareCardBuff.lScoreTimes1
    elseif myChair == 1 then
        lScoreTimes = compareCardBuff.lScoreTimes2
    elseif myChair == 2 then
        lScoreTimes = compareCardBuff.lScoreTimes3
    elseif myChair == 3 then
        lScoreTimes = compareCardBuff.lScoreTimes4
    end
    --每段比分
    local viewUser = {}

    for i=1,4 do
        local chair = {1,4,3,2} --客户要求，0号椅子逆时针转
        local userIndex = self:getUserIndex(chair[i]-1)
        if true == self._bUserGameStatus[chair[i]]  then
            self._scoreTimes[userIndex] = lScoreTimes[chair[i]]
            --是否特殊牌型
            self._bSpecialType[userIndex] = compareCardBuff.bSpecialType[1][chair[i]]
            if compareCardBuff.bSpecialType[1][chair[i]] == 0 then
                table.insert(viewUser,userIndex) 
            end
        else
            self._bSpecialType[userIndex] = 0
        end
    end
    --比牌动画
    --self._gameView:compareviewUserCard(viewUser,compareCardBuff)
    dump(viewUser,"-------比牌动画--viewUser----------",6)
    self:runAction(cc.Sequence:create(cc.DelayTime:create(0.12), cc.CallFunc:create(function()
                    self._gameView:compareCard(viewUser,compareCardBuff)
                    end)))
end

function GameLayer:OnSubPlayerExit(dataBuffer)
    print("用户强退====================")
    local PlayerExit = ExternalFun.read_netdata(cmd.CMD_S_PlayerExit, dataBuffer)

    --玩家iD
    local exitUser = PlayerExit.wPlayerID
    --self._bUserGameStatus[exitUser+1] = false
    self._gameView._exitScore = PlayerExit.lScore[1]
    --self._gameView:showExitScore()
end

function GameLayer:OnSubGameEnd(dataBuffer) 
   print("游戏结束")

    local endBuffer = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer)
    local time  = (self.m_tTimeStartGame == 0) and 30 or self.m_tTimeStartGame
    
    self._scoreEnd = endBuffer.lGameScore[1]
    dump(self._scoreEnd, "结束显示分数", 10)
    self._selfscoreEnd = endBuffer.lScoreTimes[1][self._wchair+1]

    --if not self._bUserGameStatus[self:GetMeChairID()+1] then 
         --self:runAction(cc.Sequence:create(cc.DelayTime:create(5),cc.CallFunc:create(function()

        self:SetGameClock(self._wchair,cmd.Clock_end, time)
        self._gameView:setClockVisible(true)
        if not self.m_bLookOn then 
            self._gameView:showReadyBtn(true)       
        end
            
         --end)))
    --else
        --self._gameView:setClockVisible(false)   
    --end 


    self.m_isGameEnd = true
    self._cardData = clone(endBuffer.cbHandCard)
    --self._endCallBack = function()
        self._gameView:showEndScore(self._scoreEnd, endBuffer.bEndMode)
    --end

    --if endBuffer.bEndMode ~= 0 then
       --self._endCallBack()
    --end
   -- self:onRecord()
end


function GameLayer:onRecord()
        local cmd_data = CCmd_Data:create()
    
       return self:SendData(cmd.SUB_C_RECORD, cmd_data)
end

--用户状态
function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then

        if self.m_bPriEnd then
            return 
        end 
        if (oldstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            return
        end

        if PriRoom and GlobalUserItem.bPrivateRoom and PriRoom:getInstance().m_tabPriData.dwPlayCount >0 and not GlobalUserItem.bPrivateGoldRoom then
            return
        end

        self._gameView:deleteUserInfo(useritem)

    else
        print(" 状态刷新 ",newstatus.wTableID ~= self:GetMeUserItem().wTableID)
        if (newstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            return
        end

        if newstatus.cbUserStatus == yl.US_LOOKON then
            return
        end

        if useritem then
            self.m_tabUserItem[useritem.wChairID+1] = clone(useritem)
            self.m_tabUserResult[useritem.wChairID+1] = clone(useritem)
        end

        --刷新用户信息
        if useritem == self:GetMeUserItem() then
            if PriRoom and GlobalUserItem.bPrivateRoom then
                if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == useritem.dwUserID then
                    self._gameView:showFangzhuIcon(1)
                end
                self._gameView:updateScore(useritem)
               
            end
             -- return
        end
        self._gameView:showUserInfo(useritem)
        print(" 状态刷新 cbUserStatus",newstatus.cbUserStatus)
        if newstatus.cbUserStatus == yl.US_READY then
            self._gameView:showReady(useritem.wChairID, true)
        end
    end    
end

--用户进入
function GameLayer:onEventUserEnter(tableid,chairid,useritem)
    if useritem.cbUserStatus == yl.US_LOOKON then
        return
    end
  --刷新用户信息
    if tableid ~= self:GetMeUserItem().wTableID and not GlobalUserItem.bVideo then
        return
    end

    if useritem then
        self.m_tabUserItem[useritem.wChairID+1] = clone(useritem)
        self.m_tabUserResult[useritem.wChairID+1] = clone(useritem)
    end

    if useritem == self:GetMeUserItem() then
        if PriRoom and GlobalUserItem.bPrivateRoom then
            if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == useritem.dwUserID then
                self._gameView:showFangzhuIcon(1)
            end
            self._gameView:updateScore(useritem)
        end
        if not GlobalUserItem.bVideo then
            return
        end
    end

    self._gameView:showUserInfo(useritem)
    if useritem.cbUserStatus == yl.US_READY then
        self._gameView:showReady(useritem.wChairID, true)
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
    self._gameView:updateScore(item)
end


--用户表情
function GameLayer:onUserExpression(expression, wChairId)
    print("用户表情=================")
    self._gameView:onUserExpression(expression, self:getUserIndex(wChairId))
end

-- 文本聊天
function GameLayer:onUserChat(chatdata, chairid)
    local viewid = self:getUserIndex(chairid)
    self._gameView:onUserChat(chatdata, viewid)    
end

-- 语音播放开始
function GameLayer:onUserVoiceStart( useritem, filepath )
    local viewid = self:getUserIndex(useritem.wChairID)
    self._gameView:onUserVoiceStart(viewid)
end

-- 语音播放结束
function GameLayer:onUserVoiceEnded( useritem, filepath )
    local viewid = self:getUserIndex(useritem.wChairID)
    self._gameView:onUserVoiceEnded(viewid)
end


function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end

--约战房处理
function GameLayer:onGetSitUserNum()
    return self._gameView:getUserNum()
end

--获取庄模式
function GameLayer:getBankerMode()
    return self._bHaveBanker
end

function GameLayer:getMaCode()
   return self._cbMaCardData
end

--获取额外打枪
function GameLayer:getGunNum() 
    return self._nAllWinTimes
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

--私人房强制解散处理
function GameLayer:dismissResult()
   self._gameView:dismissResult()
end

---------------------------------------------------------------------------------------
------继承函数
function GameLayer:getVideoLayerConfig()
    return 
    {
        pos = cc.p(667, 320),
        wChairCount = self._gameFrame._wChairCount,
        maskOpacity = 30
    }
end

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
        local tip = self.m_cbGameStatus > 0 and "现在退出游戏会由系统托管代打哟，输了可不能怪他哦！" or "您要退出游戏么？"
       
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