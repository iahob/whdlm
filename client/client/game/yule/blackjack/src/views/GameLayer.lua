
local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")

local GameLayer = class("GameLayer", GameModel)

local cmd = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.models.CMD_Game")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.models.GameLogic")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.views.layer.GameViewLayer")
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

function GameLayer:onExit()

    self:KillGameClock()
    self:dismissPopWait()
    GameLayer.super.onExit(self)
end

--退出桌子
function GameLayer:onExitTable()
    if self.m_querydialog then
        return
    end
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


function GameLayer:onSystemMessage( wType,szString )

        local msg = szString or ""
        self.m_querydialog = QueryDialog:create(msg,function()

            if not self.m_bClubRoom then 
                    self:onExitTable()
            end 

        end,nil,1)
        self.m_querydialog:setCanTouchOutside(false)
        self.m_querydialog:addTo(self)

end

-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end


function GameLayer:SwitchViewChairID(chair)
    local viewid = yl.INVALID_CHAIR
    local nChairCount = 6
    --print("椅子数目", nChairCount)
    if not self.nMyChairID then
        self.nMyChairID = self:GetMeChairID()
    end
    
    --print("wode",self.nMyChairID)
    if chair ~= yl.INVALID_CHAIR and chair < nChairCount then
        if chair>self.nMyChairID then
            viewid = chair - self.nMyChairID + 1
        elseif chair<self.nMyChairID then
            viewid = nChairCount - (self.nMyChairID - chair -1)
        else
            viewid = 1
        end
    end

    return viewid
end

function GameLayer:onGetSitUserNum()
    local userNum = 0
    for i=1,table.nums(self._gameView.nodePlayer) do
        if self._gameView.nodePlayer[i]:isVisible() then
            userNum = userNum + 1
        end
    end
    return userNum
    --return table.nums(self._gameView.nodePlayer)
end

function GameLayer:getUserInfoByChairID(chairId)
    
    --return  self.m_GameUserItem[chairId]--self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), chairId)
    return self.m_GameUserItem and self.m_GameUserItem[chairId] or nil
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

------继承函数
function GameLayer:getVideoLayerConfig()
    return 
    {
        pos = cc.p(667, 50),
        wChairCount = self._gameFrame._wChairCount,
        maskOpacity = 50
    }
end

-- 初始化游戏数据
function GameLayer:OnInitGameEngine()

    GameLayer.super.OnInitGameEngine(self)

    self.m_wCurrentUser = yl.INVALID_CHAIR              --当前用户
    self.m_wBankerUser = yl.INVALID_CHAIR               --庄家用户

    self.m_cbPlayStatus = {0,0,0,0,0,0}                     --游戏状态

 
    self.m_lCellScore = 0                               --单元下注

 
    self.m_bNoScore = false   --游戏币不足
    self.m_bStartGame = false

    self.m_GameUserItem = {}


    self.isPriOver = false
    -- 提示
    self.m_szScoreMsg = ""
    
   
    self.playerCount = 0

    self.bankerShowA = false -- 本局庄家是否明A
    self.myNowCard = {{} , {}}

    self.cardCount = {{0,0} , {0,0} , {0,0} , {0,0} , {0,0} , {0,0}}

    self.isSplitCard = {false , false , false , false , false , false}

    self.nowOperationCard = {1,1,1,1,1,1}

    self.cbInitTimeAddScore = 20
    self.cbInitTimeOpertion = 20

    self.m_currentUser = nil

    self.videoHandCard ={ }

    self.userAddScore = {}
    self.userInsureScore = {}

    self.nMyChairID = nil

    self.m_bLookOn = false

    self.m_playingUser = {}

   

    self.lastSendCard = 0

    

    self.isCuoCardStatus = false

    self.isDouble = false

   

end

-- 重置游戏数据
function GameLayer:OnResetGameEngine()
    GameLayer.super.OnResetGameEngine(self)

    self._gameView:OnResetView()

    self.m_wCurrentUser = yl.INVALID_CHAIR              --当前用户
    self.m_wBankerUser = yl.INVALID_CHAIR               --庄家用户
    self.m_cbPlayStatus = {0,0,0,0,0,0}                     --游戏状态
    self.m_lCellScore = 0                               --单元下注

 
   

    self.bankerShowA = false -- 本局庄家是否明A
    self.myNowCard = {{} , {}}
    self.cardCount = {{0,0} , {0,0} , {0,0} , {0,0} , {0,0} , {0,0}}
    self.isSplitCard = {false , false , false , false , false , false}
    self.nowOperationCard = {1,1,1,1,1,1}

    

    self.m_currentUser = nil

    self.videoHandCard = {  }

    self.userAddScore = {}
    self.userInsureScore = {}

    self.nMyChairID = nil

   

    self.lastSendCard = 0

    

    self.isCuoCardStatus = false

    self.isDouble = false
    
   
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
dump(useritem , "更新状态！！！")
    
    if newstatus.cbUserStatus == yl.US_LOOKON then
            return
    end

    -- 房卡模式游戏中需显示断线状态
    if GlobalUserItem.bPrivateRoom then
        -- 旧的清除
        if self.m_cbGameStatus == cmd.GAME_STATUS_FREE then
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
                --if self.endShow then
                --    self:onClearTable()
                --end
            end
        end
        if useritem.cbUserStatus == yl.US_OFFLINE then
            if oldstatus.wTableID == MyTable then
                local viewid = self:SwitchViewChairID(oldstatus.wChairID)
                if viewid and viewid ~= yl.INVALID_CHAIR and viewid ~= cmd.MY_VIEWID then
                    self._gameView:OnUpdateUser(viewid, useritem)
                    if PriRoom then
                        PriRoom:getInstance():onEventUserState(viewid, useritem, true)
                    end
                end
                --if self.endShow then
                --    self:onClearTable()
                --end
            end
            return
        end
    else
        -- 旧的清除
        if self.m_cbGameStatus == cmd.GAME_STATUS_FREE then
            if newstatus.wChairID == yl.INVALID_CHAIR then
                if oldstatus.wTableID == MyTable then
                    local viewid = self:SwitchViewChairID(oldstatus.wChairID)
                    if viewid and viewid ~= yl.INVALID_CHAIR then
                        self._gameView:OnUpdateUser(viewid, nil)
                        if PriRoom then
                            PriRoom:getInstance():onEventUserState(viewid, useritem, true)
                        end
                    end
                    --if self.endShow then
                    --    self:onClearTable()
                    --end
                end
            end
        end
    end

    -- 更新新状态
    if newstatus.wTableID == MyTable then
        local viewid = self:SwitchViewChairID(newstatus.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)
            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, false)
            end
        end
    end
end

-- 清理桌面玩家信息
function GameLayer:onClearTable()
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i - 1)
        local wViewChairId = self:SwitchViewChairID(i - 1)
        self._gameView:OnUpdateUser(wViewChairId, userItem)

        if not userItem then
            if self:SwitchViewChairID(self.m_wBankerUser) == wViewChairId then
                self._gameView.m_BankerFlag:setVisible(false)
            end
        end
    end
end

-- 设置计时器
function GameLayer:SetGameClock(chair,id,time)--m_TimeBg
    GameLayer.super.SetGameClock(self,chair,id,time)
    local viewid = chair
    --print("GetClockViewID",viewid)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        local progress = self._gameView.m_TimeProgress[viewid]
        local progressBg = self._gameView.m_TimeBg[viewid]
        if progress ~= nil and progressBg~=nil then
            progressBg:setVisible(true)
            progress:setPercentage(100)
            progress:setVisible(true)
            progress:runAction(cc.Sequence:create(cc.ProgressTo:create(time, 0), cc.CallFunc:create(function()
                progress:setVisible(false)
                progressBg:setVisible(false)
                self:OnEventGameClockInfo(viewid, id)
            end)))
        end
    end
end

-- 关闭计时器
function GameLayer:KillGameClock(notView)
    local viewid = notView or self:GetClockViewID()
    if viewid and viewid ~= yl.INVALID_CHAIR then
        local progress = self._gameView.m_TimeProgress[viewid]
        local progressBg = self._gameView.m_TimeBg[viewid]
        if progress ~= nil and progressBg~=nil then
            progress:stopAllActions()
            progress:setVisible(false)
            progressBg:setVisible(false)
        end
    end
    
    GameLayer.super.KillGameClock(self,notView)
end

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

-- 时钟处理
function GameLayer:OnEventGameClockInfo(chair,time,clockid)
    --print("定时器时间到了",chair,time,clockid ,self.isCuoCardStatus)
     --房卡不托管
    if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
        if time <= 1 then
            return true
        end
    end 

    if time <= 1 and self.m_bLookOn then 
        return 
    end

    if time < 5 then
        self:PlaySound(cmd.RES.."sound_res/GAME_WARN.wav")
    end
    if clockid == cmd.IDI_START_GAME then
        if time <= 1 then
                        self._gameFrame:setEnterAntiCheatRoom(false)--退出防作弊
                        if self.m_bClubRoom then
                                    if PriRoom:getInstance().m_tabPriData.dwPlayCount==0 then
                                        self:onExitTable()
                                    end
                        end

            return true
        end
    elseif clockid == cmd.IDI_USER_OPERTION_CARD then
        if time <=1 and chair == cmd.MY_VIEWID  then 
            
            
            return true
        end
    end
end

function GameLayer:getShareText()
    print(self.bankerMode ," GameLayer:getShareText()" )
    local str = "底分："..self.m_lCellScore.."   局数："..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit.."    "..(self.bankerMode==1 and "霸王庄" or "BJ上庄")
    return str
end

-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)
    --辅助读取int64
    local int64 = Integer64.new()
print("##########################  状态",cbGameStatus)

        local myUserItem = self:GetMeUserItem()
        if myUserItem.cbUserStatus ~= yl.US_LOOKON then 
            self.m_bLookOn = false
        else
            self.m_bLookOn = true
        end

    --初始化已有玩家
    self.m_cbPlayStatus = {0,0,0,0,0,0}

    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        local wViewChairId = self:SwitchViewChairID(i-1)
        print("onEventGameScene更新用户",wViewChairId)
        self._gameView:OnUpdateUser(wViewChairId, userItem)
        self.m_GameUserItem[i] = userItem
        
        print("onEventGameScene viewid is==============> ", wViewChairId)
    end

    self.m_cbGameStatus = cbGameStatus
    if GlobalUserItem.bPrivateRoom then
        self._gameView.m_AreaMenu:getChildByTag(46):setEnabled(false)
    end
    
	if cbGameStatus == cmd.GAME_STATUS_FREE	then				--空闲状态
        self:onSceneFree(dataBuffer)
    elseif cbGameStatus == cmd.GAME_ADD_SCORE then            --下注状态
        self:onSceneAddScore(dataBuffer)
	elseif cbGameStatus == cmd.GAME_STATUS_GAME	then			--游戏状态
        self:onSceneGame(dataBuffer)
	end

    -- 刷新房卡
    print("场景消息PriRoom GlobalUserItem.bPrivateRoom", PriRoom, GlobalUserItem.bPrivateRoom)
    if PriRoom and GlobalUserItem.bPrivateRoom then
        self._gameView._listView:reloadData()
        print("场景消息 self._gameView._priView self._gameView._priView.onRefreshInfo", self._gameView._priView, self._gameView._priView.onRefreshInfo)
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            print("场景消息 刷新房卡信息")
            self._gameView._priView:onRefreshInfo()
        end
    else
        self.lInitScore=GlobalUserItem.tabAccountInfo.lUserScore
        print("场景中更新筹码",GlobalUserItem.tabAccountInfo.lUserScore)
    end

        if GlobalUserItem.dwCurRoomGroupID ~= 0 and GlobalUserItem.bPrivateGoldRoom then 
                self.m_bClubRoom = true
        end

    self._gameView:showSitBtn(self.m_bLookOn)

    self:dismissPopWait()
end

function GameLayer:onSceneFree(dataBuffer)--空闲
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
    dump(cmd_data, "CMD_S_StatusFree", 10)

    self.cbInitTimeAddScore = cmd_data.cbTimeAddScore
    self.cbInitTimeOpertion = cmd_data.cbTimeOpertion

    self.m_priMode = cmd_data.cbPriMode
    GlobalUserItem.bPrivateGoldRoom = self.m_priMode == 1 or false

    self._gameView:setChipNum(cmd_data.lBaseJeton)

    if not GlobalUserItem.isAntiCheat() and not self.m_bLookOn then
        self._gameView.btReady:setVisible(self:GetMeUserItem().cbUserStatus == yl.US_SIT)
        -- 私人房无倒计时
        if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom then
            -- 设置倒计时
            self:SetGameClock(cmd.MY_VIEWID,cmd.IDI_START_GAME,cmd.TIME_START_GAME)
        end   
    end

    self.playerCount = cmd_data.wPlayerCount
    self.bankerMode = cmd_data.cbBankerMode
    self._gameView:updateRoomInfo(cmd_data.wPlayerCount , cmd_data.cbBankerMode)

end

function GameLayer:onSceneAddScore(dataBuffer)--下注

    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusAddScore, dataBuffer)
    dump(cmd_data, "CMD_S_StatusAddScore", 10)

    self.cbInitTimeAddScore = cmd_data.cbTimeAddScore
    self.cbInitTimeOpertion = cmd_data.cbTimeOpertion
    self.m_priMode = cmd_data.cbPriMode
    GlobalUserItem.bPrivateGoldRoom = self.m_priMode == 1 or false
    self.m_lCellScore = cmd_data.lBaseJeton

    self._gameView:setChipNum(self.m_lCellScore)

    self.m_wBankerUser = cmd_data.wBankerUser

    self._gameView:SetBanker(self:SwitchViewChairID(self.m_wBankerUser))

    

    for i=1 , 6 do
        local status = cmd_data.cbPlayStatus[1][i]
        local view = self:SwitchViewChairID(i - 1)
        self.m_cbPlayStatus[view] = status
    end

    if self.m_cbPlayStatus[cmd.MY_VIEWID]~=1 then
        self.m_cbGameStatus =cmd.GAME_STATUS_FREE
    end

        
    self._gameView.btReady:setVisible(false)

    for i=1 , cmd.GAME_PLAYER do
        if cmd_data.lTableScore[1][i]>0 then
            local view = self:SwitchViewChairID(i-1)
            local lab = self._gameView.m_addScore[view]
            lab:setVisible(true)
            lab:getChildByTag(10):setString(cmd_data.lTableScore[1][i])
            self.userAddScore[view] =  cmd_data.lTableScore[1][i]
        end
    end

    self.playerCount = cmd_data.wPlayerCount
    self.bankerMode = cmd_data.cbBankerMode
    self._gameView:updateRoomInfo(cmd_data.wPlayerCount , cmd_data.cbBankerMode)

    if self.m_wBankerUser ~= self:GetMeChairID() and cmd_data.lTableScore[1][self:GetMeChairID()+1]==0 and self.m_cbPlayStatus[cmd.MY_VIEWID]==1 and not self.m_bLookOn then
        self._gameView.nodeAddScore:setVisible(true)
    end

    for i = 1, cmd.GAME_PLAYER  do
        local view = self:SwitchViewChairID(i-1)
        if self.m_cbPlayStatus[view]==1 and i-1 ~= self.m_wBankerUser and cmd_data.lTableScore[1][i]==0 then
            self:SetGameClock(view,cmd.IDI_USER_ADD_SCORE,cmd_data.cbTimeRemain)
        end
    end

end

function GameLayer:onSceneGame(dataBuffer)--游戏
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusGame, dataBuffer)
    dump(cmd_data, "CMD_S_StatusGame", 10)

    self.cbInitTimeAddScore = cmd_data.cbTimeAddScore
    self.cbInitTimeOpertion = cmd_data.cbTimeOpertion
    self.m_priMode = cmd_data.cbPriMode
    GlobalUserItem.bPrivateGoldRoom = self.m_priMode == 1 or false
    self.m_lCellScore = cmd_data.lBaseJeton
    self.m_currentUser = cmd_data.wCurrentUser
    self._gameView:setChipNum(self.m_lCellScore)

    for i=1 , 6 do
        local status = cmd_data.cbPlayStatus[1][i]
        local view = self:SwitchViewChairID(i - 1)
        self.m_cbPlayStatus[view] = status
    end

    if self.m_cbPlayStatus[cmd.MY_VIEWID]~=1 then
        self.m_cbGameStatus =cmd.GAME_STATUS_FREE
    end


    self.m_wBankerUser = cmd_data.wBankerUser

    self._gameView:SetBanker(self:SwitchViewChairID(self.m_wBankerUser))

for i=1 , cmd.GAME_PLAYER do
        if cmd_data.lTableScore[1][i]>0 then
            local view = self:SwitchViewChairID(i-1)
            local lab = self._gameView.m_addScore[view]
            lab:setVisible(true)
            lab:getChildByTag(10):setString(cmd_data.lTableScore[1][i])
            self.userAddScore[view] = cmd_data.lTableScore[1][i]
        end
    end
        
    self._gameView.btReady:setVisible(false)

    self.playerCount = cmd_data.wPlayerCount
    self.bankerMode = cmd_data.cbBankerMode
    self._gameView:updateRoomInfo(cmd_data.wPlayerCount , cmd_data.cbBankerMode)

    self:SetGameClock(self:SwitchViewChairID(cmd_data.wCurrentUser),cmd.IDI_USER_OPERTION_CARD,cmd_data.cbTimeRemain)

    if  cmd_data.cbHandCardData[(self.m_wBankerUser+1)*2-1][2]~=0 and 
        GameLogic:getCardValue(cmd_data.cbHandCardData[(self.m_wBankerUser+1)*2-1][2]) == 1 then
        self.bankerShowA = true -- 本局庄家是否明A
    end

    for i=1 , cmd.GAME_PLAYER do
        local view = self:SwitchViewChairID(i-1)

        for j=1 , 2 do
            print(view , i-1 , j ,cmd_data.cbCardCount[1][i*2-1+(j-1)] )
            self.cardCount[view][j] = cmd_data.cbCardCount[1][i*2-1+(j-1)]
            if j==2 and self.cardCount[view][j]~=0 then
                self.isSplitCard[view] = true
                if cmd_data.bStopCard[1][i*2-1] then
                    self.nowOperationCard[view] = 2
                end
            end
        end
    end

    for i=1 , 2 do
        for j=1 , self.cardCount[cmd.MY_VIEWID][i] do
            table.insert(self.myNowCard[i] , cmd_data.cbHandCardData[(self:GetMeChairID()+1)*2-1+(i-1)][j]) 
        end
    end

    
    
    dump(self.myNowCard , "我的牌")
    dump(self.isSplitCard , "是否已分牌")
    dump(self.nowOperationCard , "当前操作的牌")
    dump(self.cardCount , "牌的数量")

    local delayCount = 1
    local bankerShowCard = cmd_data.cbHandCardData[(self.m_wBankerUser+1)*2-1][2]
    for i = 1, cmd.GAME_PLAYER do
        for j=1 , 2 do
            for k=1 , self.cardCount[i][j] do
                local cardIndex = nil 
                
                if i == cmd.MY_VIEWID then
                    cardIndex = GameLogic:getCardColor(self.myNowCard[j][k])*13 + GameLogic:getCardValue(self.myNowCard[j][k])
                elseif self:SwitchViewChairID(self.m_wBankerUser)==i and j==1 and k==2 then
                    cardIndex = GameLogic:getCardColor(bankerShowCard)*13 + GameLogic:getCardValue(bankerShowCard)
                end
                self._gameView:SendCard(i,k+(j-1)*5,cardIndex,delayCount*0.1)
                delayCount = delayCount + 1
            end
        end
    end   

    self:runAction(cc.Sequence:create(
        cc.DelayTime:create(delayCount*0.1 ),
        cc.CallFunc:create(
            function ()
                self._gameView:showMyCardType(self.myNowCard)
            end)))

    
--
    if cmd_data.wCurrentUser == self:GetMeChairID() and self.bankerShowA and self.cardCount[cmd.MY_VIEWID][1] + self.cardCount[cmd.MY_VIEWID][2]==2 and self.m_wBankerUser~=self:GetMeChairID()  then
        self._gameView:showOperationNode(true,nil,delayCount*0.1)
        self:PlaySound(cmd.RES.."sound_res/new/blackjack_baoxian.mp3")

    elseif cmd_data.wCurrentUser == self:GetMeChairID()  then

        self._gameView:showOperationNode(false,self:getStartOpertionRule(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]]),delayCount*0.1)
        
    end

    if self.isSplitCard[self:SwitchViewChairID(cmd_data.wCurrentUser)] then
        self:runAction(cc.Sequence:create(
            cc.DelayTime:create(delayCount*0.1),
            cc.CallFunc:create(
                function ()
                    self._gameView:splitCardAnim(self:SwitchViewChairID(cmd_data.wCurrentUser) , self.nowOperationCard[self:SwitchViewChairID(cmd_data.wCurrentUser)])
                end
                                )
            ))
    end

    

end

-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)


	if sub == cmd.SUB_S_GAME_START then   --开始下注
		self:onSubGameStart(dataBuffer)
    elseif sub == cmd.SUB_S_SEND_CARD then --开始发牌
        self:onSubSendCard(dataBuffer)
    elseif sub == cmd.SUB_S_SPLIT_CARD then--有人分牌
        self:onSubSplitCard(dataBuffer)
    elseif sub == cmd.SUB_S_STOP_CARD then--有人停牌
        self:onSubStopCard(dataBuffer)
    elseif sub == cmd.SUB_S_DOUBLE_SCORE then--有人加倍
        self:onSubDouble(dataBuffer)
    elseif sub == cmd.SUB_S_INSURE then--有人买保险
        self:onSubInsure(dataBuffer)
    elseif sub == cmd.SUB_S_ADD_SCORE then  --玩家下注了
        self:onSubAddScore(dataBuffer)
    elseif sub == cmd.SUB_S_GET_CARD then  --有人要牌
        self:onSubAddCard(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_END then 
        self:onSubGameEnd(dataBuffer)
    elseif sub == cmd.SUB_S_RECORD then                     --游戏记录
         self:onSubGameRecord(dataBuffer)

    else

    	print("unknow gamemessage sub is"..sub)
    end
        print("收到游戏消息！！====================",sub)
 
end

--开始发牌
function GameLayer:onSubSendCard(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_SendCard, dataBuffer)
    dump(cmd_data,"游戏发牌开始咯！！")


     --发牌
    self:PlaySound(cmd.RES.."sound_res/SEND_CARD_BEGIN.wav")
    local bankerChair = self.m_wBankerUser+1
    local delayCount = 1
    for index = 1 , 2 do
        for i = 1, cmd.GAME_PLAYER do
 
            if bankerChair>5 then
                bankerChair=0
            end
            local viewId = self:SwitchViewChairID(bankerChair)
            if self.m_cbPlayStatus[self:SwitchViewChairID(bankerChair)] == 1 then

                local cardIndex = GameLogic:getCardColor(cmd_data.cbHandCardData[bankerChair+1][index])*13 + GameLogic:getCardValue(cmd_data.cbHandCardData[bankerChair+1][index])
                
                self._gameView:SendCard(self:SwitchViewChairID(bankerChair),index,cardIndex,delayCount*0.3)
                delayCount = delayCount + 1
                self.cardCount[self:SwitchViewChairID(bankerChair)][1] = self.cardCount[self:SwitchViewChairID(bankerChair)][1] + 1

            end
            bankerChair = bankerChair + 1
        end
    end



    if GameLogic:getCardValue(cmd_data.cbHandCardData[self.m_wBankerUser+1][2]) == 1 then
        self.bankerShowA = true
    end
    self.m_currentUser = cmd_data.wCurrentUser
    self.myNowCard[1] = cmd_data.cbHandCardData[self:GetMeChairID()+1]

    if self.m_cbPlayStatus[cmd.MY_VIEWID] == 1 and not self.m_bLookOn then 

            self:runAction(cc.Sequence:create(
                cc.DelayTime:create(delayCount*0.1 ),
                cc.CallFunc:create(
                    function ()
                        self._gameView:showMyCardType(self.myNowCard)
                    end
                                    )
                    ))

            if cmd_data.wCurrentUser == self:GetMeChairID() then 
                if self.bankerShowA then
                    self._gameView:showOperationNode(true,nil,delayCount*0.1)
                    self:PlaySound(cmd.RES.."sound_res/new/blackjack_baoxian.mp3")
                else
                    self._gameView:showOperationNode(false,self:getStartOpertionRule(cmd_data.cbHandCardData[self:GetMeChairID()+1]),delayCount*0.1)
                end 
            end

            local allScore = 0 
            for i=1 , #self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]] do
                local num = GameLogic:getCardValue(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]][i])>=10 and 10 or GameLogic:getCardValue(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]][i])
                allScore = allScore + (num==1 and 11 or num)
            end
            
            self:playCardSound(allScore , 2)
            

            self:SetGameClock(self:SwitchViewChairID(cmd_data.wCurrentUser) ,cmd.IDI_USER_OPERTION_CARD,self.cbInitTimeOpertion)
    end


end

function GameLayer:getStartOpertionRule(card)
    local show = {false , true , true , true}
    local allScore = 0
    local Anum = 0
    for i=1 , #card do
        local num = GameLogic:getCardValue(card[i])>=10 and 10 or GameLogic:getCardValue(card[i])
        if num==1 then
            Anum = Anum + 1
        end
        allScore = allScore + (num==1 and 11 or num)
    end

    for i=1 , Anum do
        if allScore>21 then
            allScore = allScore - 10
        end
    end
        
    if  self:GetMeChairID()~=self.m_wBankerUser   and   --庄家不能分
        #card==2                                and   --牌2张的时候才可以分
        GameLogic:getCardValue(card[1]) == GameLogic:getCardValue(card[2]) and  --对子才可以分
        not self.isSplitCard[cmd.MY_VIEWID] then   --分了就不能分了
        show[1] = true  --拿到对子，可以分牌
    end

    

    if self:GetMeChairID()==self.m_wBankerUser and allScore<16 then--庄家点数小于16不能停牌，闲家无限制
        show[2] = false
    end


    if allScore>=21 or (self:GetMeChairID()==self.m_wBankerUser and allScore>=17) or #card>=5 or self.isDouble then --如果一发牌就 黑杰克或21点，不能要拍 不能加倍
        show[3] = false
    end

    if #card>2 or allScore>=21 or self:GetMeChairID()==self.m_wBankerUser or self.isSplitCard[cmd.MY_VIEWID] then
        show[4] = false
    end
    return show
end

--有人分牌
function GameLayer:onSubSplitCard(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_SplitCard, dataBuffer)
    dump(cmd_data,"有人分牌！！")
    local splitUserView = self:SwitchViewChairID(cmd_data.wSplitUser)
    self._gameView:clearCard(splitUserView)
   -- self.myNowCard = {{} , {}}
    self.isSplitCard[splitUserView] = true
    self.cardCount[splitUserView][1] = 2
    self.cardCount[splitUserView][2] = 2
    assert(false)
    local tempCard = {}
    if splitUserView==cmd.MY_VIEWID and self.m_cbPlayStatus[cmd.MY_VIEWID] == 1 and not self.m_bLookOn then
        for i=1 , 2 do
            table.insert(tempCard , self.myNowCard[1][i])
        end
        for i=1 ,2 do
            self.myNowCard[i] = {}
            table.insert(self.myNowCard[i] , tempCard[i])
            table.insert(self.myNowCard[i] , cmd_data.cbCardData[1][i])
        end
        dump(self.myNowCard , "分牌后我的牌")
        self._gameView:showMyCardType(self.myNowCard)
        self._gameView.userCard[cmd.MY_VIEWID].area:setPositionX(0)
    end
    if self.userAddScore[splitUserView] then
    self.userAddScore[splitUserView] = self.userAddScore[splitUserView] * 2

    local lab = self._gameView.m_addScore[splitUserView]
    lab:setVisible(true)
    lab:getChildByTag(10):setString(self.userAddScore[splitUserView])
end
    if self.userInsureScore[splitUserView] then
        self.userInsureScore[splitUserView] = self.userInsureScore[splitUserView] * 2
        local lab = self._gameView.m_insureScore[splitUserView]
        lab:setVisible(true)
        lab:getChildByTag(10):setString(self.userInsureScore[splitUserView])
    end
    
    local delayCount = 1
    for i=1 , 2 do
        for j=1 , 2 do
            local cardIndex = nil 
            local index = j + (i-1)*5
            if splitUserView==cmd.MY_VIEWID then
                cardIndex = GameLogic:getCardColor(self.myNowCard[i][j])*13 + GameLogic:getCardValue(self.myNowCard[i][j])
            end
            self._gameView:SendCard(splitUserView,index,cardIndex,delayCount*0.1)
            delayCount = delayCount + 1
        end
    end

    if splitUserView == cmd.MY_VIEWID then
        self._gameView:showOperationNode(false,self:getStartOpertionRule(self.myNowCard[1]),delayCount*0.1)
    end


    --local this = self
    --self:runAction(cc.Sequence:create(
    --    cc.DelayTime:create(delayCount*0.1 + 0.5),
    --    cc.CallFunc:create(
     --       function ()
                self._gameView:splitCardAnim(splitUserView , 1)
     --       end
     --                       )
     --       ))
    
end


--有人停牌
function GameLayer:onSubStopCard(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StopCard, dataBuffer)
    dump(cmd_data,"有人停牌！！".."wo de yi zi"..self:GetMeChairID())--
    local stopCardView = self:SwitchViewChairID(cmd_data.wStopCardUser)
    local this = self

    if self.isCuoCardStatus and cmd_data.wStopCardUser == self:GetMeChairID() and not self.m_bLookOn then
        self.isCuoCardStatus = false
        self._gameView:cuoActionEnd(false)
    end

    if cmd_data.wStopCardUser ~= yl.INVALID_TABLE and cmd_data.wStopCardUser == self:GetMeChairID()  then
        --print("隐藏操作")
        self._gameView.nodeOperation:setVisible(false)
        self._gameView.nodeInsure:setVisible(false)
    end
    self._gameView:showStopCard(stopCardView , self.nowOperationCard[stopCardView])
    self.m_currentUser = cmd_data.wCurrentUser
    if cmd_data.wStopCardUser == cmd_data.wCurrentUser then
        self.nowOperationCard[self:SwitchViewChairID(cmd_data.wStopCardUser)] = 2
        --print("停止动画")
        self._gameView:splitCardAnim(stopCardView , 2)
    else
        self._gameView:splitCardAnim(stopCardView , 0)
        
    end
--print("本局庄家是不是有A",self.bankerShowA)
dump(self.isSplitCard , "各位玩家分牌了没有")
    if cmd_data.wCurrentUser ~= yl.INVALID_TABLE and cmd_data.wCurrentUser == self:GetMeChairID() then
        --print("wo caozuo")
        if self.m_wBankerUser == self:GetMeChairID() then
            if self:GetMeChairID()~=cmd_data.wStopCardUser then
                self._gameView:showOperationNode(false,self:getStartOpertionRule(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]]),0)
            end
        else
            if self.bankerShowA and not self.isSplitCard[self:SwitchViewChairID(cmd_data.wCurrentUser)] then
                self._gameView:showOperationNode(true,nil,0)
                self:PlaySound(cmd.RES.."sound_res/new/blackjack_baoxian.mp3")
            else
                self._gameView:showOperationNode(false,self:getStartOpertionRule(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]]),0)
            end
        end
    end
    self:KillGameClock(stopCardView)
    if self.m_wBankerUser~=cmd_data.wCurrentUser  then
        self:SetGameClock(self:SwitchViewChairID(cmd_data.wCurrentUser) ,cmd.IDI_USER_OPERTION_CARD,self.cbInitTimeOpertion)
    end
    
    self:PlaySound(cmd.RES.."sound_res/new/blackjack_tingpai.mp3")
end

--有人加倍
function GameLayer:onSubDouble(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_DoubleScore, dataBuffer)
    dump(cmd_data,"有人加倍！！")
    self.lastSendCard = cmd_data.cbCardData
    local cardIndex = GameLogic:getCardColor(cmd_data.cbCardData)*13 + GameLogic:getCardValue(cmd_data.cbCardData)
    local doubleView = self:SwitchViewChairID(cmd_data.wDoubleScoreUser)

    if doubleView ~= cmd.MY_VIEWID then
        cardIndex = nil
    elseif not self.m_bLookOn then
        self.isDouble = true
        table.insert(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]] ,cmd_data.cbCardData )
        self._gameView.nodeCuoOrShow:setVisible(true)
        self._gameView:hideMyCardType(1)
        cardIndex = 0
        self.isCuoCardStatus = true
    end

    self.cardCount[doubleView][1] = self.cardCount[doubleView][1] + 1
 
    self._gameView:SendCard(doubleView,
                            (self.nowOperationCard[doubleView]-1) * 5 + self.cardCount[doubleView][self.nowOperationCard[doubleView]],
                            cardIndex,
                            0)
    dump(self.userAddScore , "加倍下注")

    if self.userAddScore[doubleView] then
    self.userAddScore[doubleView] = self.userAddScore[doubleView] * 2

    local lab = self._gameView.m_addScore[doubleView]
    lab:setVisible(true)
    lab:getChildByTag(10):setString(self.userAddScore[doubleView])
    end
    self:PlaySound(cmd.RES.."sound_res/new/blackjack_shuangbei.mp3")
    if self.userInsureScore[doubleView] then
        self.userInsureScore[doubleView] = self.userInsureScore[doubleView] * 2
        local lab = self._gameView.m_insureScore[doubleView]
        lab:setVisible(true)
        lab:getChildByTag(10):setString(self.userInsureScore[doubleView])
    end
    
end

--有人买保险
function GameLayer:onSubInsure(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_Insure, dataBuffer)
    dump(cmd_data,"有人买保险！！")
    local insureView = self:SwitchViewChairID(cmd_data.wInsureUser)

    self.userInsureScore[insureView] = cmd_data.lInsureScore

    local lab = self._gameView.m_insureScore[insureView]
    lab:setVisible(true)
    lab:getChildByTag(10):setString(self.userInsureScore[insureView])
end

--有人要牌
function GameLayer:onSubAddCard(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GetCard, dataBuffer)
    dump(cmd_data,"有人要牌！！")
    self.lastSendCard = cmd_data.cbCardData
    local cardIndex = GameLogic:getCardColor(cmd_data.cbCardData)*13 + GameLogic:getCardValue(cmd_data.cbCardData)
    local getCardView = self:SwitchViewChairID(cmd_data.wGetCardUser)

    if getCardView ~= cmd.MY_VIEWID then
        cardIndex = nil

    else
        table.insert(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]] ,cmd_data.cbCardData )
        if cmd_data.bSysGet==false and not self.m_bLookOn then
            self._gameView.nodeCuoOrShow:setVisible(true)
            self._gameView:hideMyCardType(self.nowOperationCard[cmd.MY_VIEWID])
            cardIndex = 0
            self.isCuoCardStatus = true
        else
            self._gameView.nodeOperation:setVisible(false)  
            self._gameView:showMyCardType(self.myNowCard)

        end
    end


    self.cardCount[getCardView][self.nowOperationCard[getCardView]] = self.cardCount[getCardView][self.nowOperationCard[getCardView]] + 1


    local index = (self.nowOperationCard[getCardView]-1) * 5 + self.cardCount[getCardView][self.nowOperationCard[getCardView]]


    self._gameView:SendCard(getCardView,
                            index,
                            cardIndex,
                            0)

    if self.isSplitCard[getCardView] then
        --local this = self
        --self:runAction(cc.Sequence:create(
        --cc.DelayTime:create(1.2),
        --cc.CallFunc:create(
        --    function ()
                self._gameView:splitCardAnim(getCardView , self.nowOperationCard[getCardView])
         --   end
         --                   )
         --   ))
    end
    self:PlaySound(cmd.RES.."sound_res/new/blackjack_yaopai.mp3")
    if cardIndex~=nil and self.m_currentUser == self:GetMeChairID() then 
        local allScore = 0 
        local ANum = 0
        for i=1 , #self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]] do
            local num = GameLogic:getCardValue(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]][i])>=10 and 10 or GameLogic:getCardValue(self.myNowCard[self.nowOperationCard[getCardView]][i])
            if num==1 then
                ANum = ANum + 1
            end
            allScore = allScore + (num==1 and 11 or num)
        end
        for k=1 , ANum do
            if allScore>21 then
                allScore = allScore - 10
            end
        end
        if cmd_data.bSysGet  then
            self:playCardSound(allScore , self.cardCount[getCardView][self.nowOperationCard[getCardView]])
        end
        --self._gameView:showOperationNode(false,self:getStartOpertionRule(self.myNowCard[self.nowOperationCard[getCardView]]),0)
    end
end

function GameLayer:playCardSound(score , cardNum)
    if score>21 then --爆了
        self:PlaySound(cmd.RES.."sound_res/new/blackjack_baobao.mp3")
    elseif cardNum==5 then --五龙
        self:PlaySound(cmd.RES.."sound_res/new/blackjack_wulong.mp3")
    elseif score==21 and cardNum==2 then --黑杰克
        self:PlaySound(cmd.RES.."sound_res/new/blackjack_heijieke.mp3")
    else--普通点数
        self:PlaySound(cmd.RES.."sound_res/new/blackjack_".. score ..".mp3")
    end
end

function GameLayer:cuoCardFinish()
    
    self._gameView.nodeCuoOrShow:setVisible(false)
    self._gameView:showMyCardType(self.myNowCard)
    if self.isCuoCardStatus then
        self.isCuoCardStatus = false
        self._gameView:showOperationNode(false,self:getStartOpertionRule(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]]),0)
    end

    local allScore = 0 
    local ANum = 0
    for i=1 , #self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]] do
        local num = GameLogic:getCardValue(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]][i])>=10 and 10 or GameLogic:getCardValue(self.myNowCard[self.nowOperationCard[cmd.MY_VIEWID]][i])
        if num==1 then
            ANum = ANum + 1
        end
        allScore = allScore + (num==1 and 11 or num)
    end
    for k=1 , ANum do
        if allScore>21 then
            allScore = allScore - 10
        end
    end
   
        self:playCardSound(allScore , self.cardCount[cmd.MY_VIEWID][self.nowOperationCard[cmd.MY_VIEWID]])
    
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

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
    dump(cmd_data,"游戏开始咯！！")
    local int64 = Integer64.new()
    
    self:OnResetGameEngine()
    self.m_bStartGame = true
    self._gameView.btReady:setVisible(false)

    self._gameView:showSitBtn(false)

    self.m_cbGameStatus = cmd.GAME_ADD_SCORE

    for i=1 , 6 do
        local status = cmd_data.cbPlayStatus[1][i]
        local view = self:SwitchViewChairID(i - 1)
        self.m_cbPlayStatus[view] = status
    end
    
    self.m_lCellScore = cmd_data.lCellScore

    self._gameView:setChipNum(self.m_lCellScore)

    self.m_wBankerUser = cmd_data.wBankerUser

    self._gameView:SetBanker(self:SwitchViewChairID(self.m_wBankerUser))

    
    for i = 1, cmd.GAME_PLAYER  do
        --跟新玩家
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        local wViewChairId = self:SwitchViewChairID(i-1)
        --print("游戏开始更新用户",wViewChairId)
        self._gameView:OnUpdateUser(wViewChairId, userItem)
        self.m_GameUserItem[i] = userItem
        
       -- print("onSubGameStart viewid is==============> ", wViewChairId,userItem)


        if self.m_cbPlayStatus[wViewChairId] ==1 and i-1 ~= self.m_wBankerUser then--闲家下分定时器
            --print("设置下分定时器",wViewChairId ,self.cbInitTimeAddScore)
            self:SetGameClock(wViewChairId,cmd.IDI_USER_ADD_SCORE,self.cbInitTimeAddScore)
        end
    end


    self:PlaySound(cmd.RES.."sound_res/new/blackjack_kaishixiazhu.mp3")
    
    

    if self.m_wBankerUser ~= self:GetMeChairID() and self.m_cbPlayStatus[cmd.MY_VIEWID] == 1 and not self.m_bLookOn then
        self._gameView.nodeAddScore:setVisible(true)
    end
    
    self:PlaySound(cmd.RES.."sound_res/GAME_START.wav")

    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
            self._gameView._priView:onRefreshInfo()
        end
    end

end

--用户叫分
function GameLayer:onSubAddScore(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_AddScore, dataBuffer)
    dump(cmd_data,"CMD_S_AddScore:",6)

    local MyChair = self:GetMeChairID()

    self:KillGameClock(self:SwitchViewChairID(cmd_data.wAddScoreUser))

    local lab = self._gameView.m_addScore[self:SwitchViewChairID(cmd_data.wAddScoreUser)]
    lab:setVisible(true)
    lab:getChildByTag(10):setString(cmd_data.lAddScore)

    
    self.userAddScore[self:SwitchViewChairID(cmd_data.wAddScoreUser)] = cmd_data.lAddScore
    dump(self.userAddScore , "下注")

    if MyChair == cmd_data.wAddScoreUser then
        self._gameView.nodeAddScore:setVisible(false)
    end
    
end


--游戏结束
function GameLayer:onSubGameEnd(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer)
    dump(cmd_data , "游戏结束")
    self:KillGameClock()
    self.m_bStartGame = false

    self.m_cbGameStatus = cmd.GAME_STATUS_FREE

    local MyChair = self:GetMeChairID() + 1

     --清理界面
    local lGameTax = cmd_data.lGameTax

    local winner 
    local lGameScore = {}
    for i = 1, cmd.GAME_PLAYER do
        lGameScore[i] = cmd_data.lGameScore[1][i]
        if lGameScore[i] > 0 then
            winner = i
        else
            winner = 0
        end
    end


    local endScore = {}
    for i=1 , 6 do
        local view = self:SwitchViewChairID(i-1)
        if self.m_cbPlayStatus[view]==1 then
            endScore[view] = cmd_data.lGameScore[1][i]

        end
    end
--blackjack_shule   blackjack_yingle
    
    if self.m_cbPlayStatus[cmd.MY_VIEWID]==1 then
        if  endScore[cmd.MY_VIEWID]>=0 then
            self:PlaySound(cmd.RES.."sound_res/new/blackjack_yingle.mp3")
        else
            self:PlaySound(cmd.RES.."sound_res/new/blackjack_shule.mp3")
        end
    end

    


    self._gameView.nodeAddScore:setVisible(false)
    self._gameView.nodeOperation:setVisible(false)
    self._gameView.nodeInsure:setVisible(false)
   self._gameView.nodeCuoOrShow:setVisible(false)

   
    self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
         cc.CallFunc:create(function()
            self._gameView.btReady:setVisible(true)
            self._gameView:showGameEnd(endScore , cmd_data.cbCardData)
         end),
         cc.DelayTime:create(2.5),
         cc.CallFunc:create(function()
            if not GlobalUserItem.bPrivateRoom then
                self:onClearTable()
            end
         end)))
    
    
    self._gameView.nodeOperation:setVisible(false)
    if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom then
        -- 设置倒计时
        self:SetGameClock(cmd.MY_VIEWID,cmd.IDI_START_GAME,cmd.TIME_START_GAME)

    end 
    

    --self.m_cbPlayStatus = {0, 0, 0, 0, 0,0}
    self:PlaySound(cmd.RES.."sound_res/GAME_END.wav")

    

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


--发送准备
function GameLayer:onStartGame(bReady)
    self:OnResetGameEngine()
    if bReady == true then
        self:KillGameClock()
        self:SendUserReady()
        self.m_bStartGame = true
        
    end
end




--换位操作
function GameLayer:onChangeDesk()
    self._gameFrame:QueryChangeDesk()
end




--发送加注消息
function GameLayer:onSendAddScore(score)
    self._gameView.nodeAddScore:setVisible(false)
    local  dataBuffer = CCmd_Data:create(8)
    local index = {1,2,5,10}--self.m_lCellScore
    dataBuffer:pushscore(index[score]*self.m_lCellScore)
    self:SendData(cmd.SUB_C_ADD_SCORE, dataBuffer)
end

--发送分牌消息
function GameLayer:onSendSplitCard()
    local  dataBuffer = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_SPLIT_CARD, dataBuffer)
end

--发送停牌消息
function GameLayer:onSendStopCard()
    local  dataBuffer = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_STOP_CARD, dataBuffer)
end

--发送加牌消息
function GameLayer:onSendAddCard()
    local  dataBuffer = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_GET_CARD, dataBuffer)
end

--发送双倍下注消息
function GameLayer:onSendDouble()
    local  dataBuffer = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_DOUBLE_SCORE, dataBuffer)
end
--发送保险消息
function GameLayer:onSendInsure(isInsure)
    if isInsure then
        local  dataBuffer = CCmd_Data:create(0)
        self:SendData(cmd.SUB_C_INSURE, dataBuffer)
        
    end
    self._gameView:showOperationNode(false,self:getStartOpertionRule(self.myNowCard[1]),0)
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



function GameLayer:getGameRule(buffer)

        local strTable ={}
        --游戏模式(玩法)
        local banker = buffer[4]
        local bankerStr = {"BJ上庄","霸王庄"}
        table.insert(strTable,bankerStr[banker+1])


    return strTable
end



-------------------------------------------------------新大厅增加函数--------------------------------------------
--游戏记录（房卡）
function GameLayer:onSubGameRecord(dataBuffer)
    print("游戏记录")
      print("dataBuffer:len",dataBuffer:getlen())
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_RECORD, dataBuffer)

    dump(cmd_data, "CMD_S_Record")
    --全部成绩

    self.m_userRecord = {}
    local scoreList = {}
    local nInningsCount = cmd_data.iCount
    local maxRound = 0

    for i=1 , cmd.GAME_PLAYER do
        local DetailScore = {}
        if nInningsCount[1][i]>maxRound then
            maxRound = nInningsCount[1][i]
        end
        for j = 1, nInningsCount[1][i] do
            DetailScore[j] = cmd_data.lDetailScore[i][j]
        end
        table.insert(scoreList, DetailScore)
    end

    --for j = 1, nInningsCount do
    --    local DetailScore = {}
    --    for i=1, cmd.GAME_PLAYER do
    --        DetailScore[i] = cmd_data.lDetailScore[i][j]
    --    end
    --    table.insert(scoreList, DetailScore)
    --end
    dump(scoreList , "ssssssss")

    self._gameView:updataScoreList(scoreList , maxRound)
    
end

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode(cmd.RES.."game/ShareLayer.csb"):addTo(self)
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


return GameLayer