local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local cmd = appdf.req(appdf.GAME_SRC.."yule.dzshowhand.src.models.CMD_Game")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.dzshowhand.src.views.layer.GameViewLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

-- 初始化界面
function GameLayer:ctor(frameEngine,scene)
    GameLayer.super.ctor(self, frameEngine, scene)
end

function GameLayer:getParentNode()--
    return self._scene
end
--创建场景
function GameLayer:CreateView()
    return GameViewLayer:create(self):addTo(self)
end

-- 初始化游戏数据
function GameLayer:OnInitGameEngine()
    --游戏变量 
    self.m_wDUser       = 65535      --D玩家
    self.m_wCurrentUser = 65535     --当前玩家
    self.m_cbBigBlind   = 65535     --大盲玩家
    self.m_cbSmallBlind = 65535     --小盲玩家
    self.m_lCenterChip    = 0       --底池分数
    self.m_lCellScore     = 0       --盲注分数
    self.m_lTurnMaxScore  = 0       --最大下注
    self.m_lTurnLessScore = 0       --最小下注
    self.m_lAddLessScore  = 0       --加最小注
    self.m_lAnteScore     = 0       --固定底注
    self.m_lMinBetScore   = 0       --最低携带金币
    self.m_lMaxBetScore   = 0       --最高携带金币
    self.m_lAllBetScore   = 0       --累计带入限制
    self.m_cbBalanceCount = 0       --平衡次数
    self.m_bCanStandUp    = false   --起立标记 
    self.m_cbMaxPlayCount = 9
    self.m_nGameEndDelay = 2

    self.m_allInConclude = false

  

    --时间
    self.m_startTime = 0
    self.m_betTime   = 0

    self.m_cbCenterCardData = {0,0,0,0,0}            --公牌数据
    self.m_lUserAddScore    = {0,0,0,0,0,0,0,0,0}    --用户总下注
    self.m_lTableScore      = {0,0,0,0,0,0,0,0,0}    --当前轮玩家下注额
    self.m_lTakeScore       = {0,0,0,0,0,0,0,0,0}    --当前玩家携带注额
    self.m_cbPlayStatus     = {0,0,0,0,0,0,0,0,0}    --玩家游戏状态
    self.m_cbHandCardData   = {} --用户扑克
    self.m_tabUserItem      = {}

    self.bPriGameEnd = false

    --约战
    self.m_userRecord = {}   --用户记录
    self._wchair = self:GetMeChairID()
    GameLayer.super.OnInitGameEngine(self)
    self:KillGameClock()

    self.m_playingUser = {}
end


-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode("share/shareLayer.csb"):addTo(self)
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
            local btn = shareLayer:getChildByName("bg"):getChildByName("Button_friend")
            btn:setTag(TAG_WXSHARE)
            btn:addTouchEventListener( touchFunC )
            -- 朋友圈按钮
            btn = shareLayer:getChildByName("bg"):getChildByName("Button_friendGroup")
            btn:setTag(TAG_CYCLESHARE)
            btn:addTouchEventListener( touchFunC )
            -- 屏蔽层
            local panel = shareLayer:getChildByName("Panel_2")
            panel:setTag(MASK_PANEL)
            panel:addTouchEventListener( touchFunC )
        else
            print("no image")
        end
    end
end

-- 重置游戏数据
function GameLayer:OnResetGameEngine()
    GameLayer.super.OnResetGameEngine(self)
    self:OnInitGameEngine()
end

function GameLayer:resetData()
    self.m_lCenterChip      = 0       --底池分数
    self.m_lTurnMaxScore    = 0       --最大下注
    self.m_lTurnLessScore   = 0       --最小下注
    self.m_lAddLessScore    = 0       --加最小注
    self.m_cbBalanceCount   = 0
    self.m_cbHandCardData   = {}
    self.m_cbCenterCardData = {0,0,0,0,0}
    self.m_lUserAddScore    = {0,0,0,0,0,0,0,0,0}    --用户总下注
    self.m_lTableScore      = {0,0,0,0,0,0,0,0,0}    --当前轮玩家下注额
end

--相对索引 return 1-9
function GameLayer:getUserIndex( wChairID )
    local viewIndex = cmd.InvalidIndex
    local MyChair = self:GetMeChairID() 

    if not (MyChair>=0 and MyChair<9 ) then
          MyChair = self._wchair 
    else
          self._wchair  = MyChair
    end

    viewIndex = math.mod((wChairID - MyChair+cmd.GAME_PLAYER),cmd.GAME_PLAYER) --顺时针
    return viewIndex+1
end

--换位操作
function GameLayer:onChangeDesk()
    self._gameFrame:QueryChangeDesk()
end

function GameLayer:onUserChat(chatinfo,sendchair)
    if chatinfo and sendchair then
        local viewid = self:getUserIndex(sendchair) 
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:ShowUserChat(viewid, chatinfo.szChatString)
        end
    end
end

function GameLayer:onUserExpression(expression,sendchair)
print("GameLayer:onUserExpression",expression,sendchair)
    if expression and sendchair then
        local viewid = self:getUserIndex(sendchair) 
        print("2222",viewid, expression.wItemIndex)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:ShowUserExpression(viewid, expression.wItemIndex)
        end
    end
end

--相对索引座位 return 0-8
function GameLayer:getUserChair( viewIndex )
    local MyChair = self:GetMeUserItem().wChairID
    print("我的椅子号",MyChair)
    return  math.mod(cmd.GAME_PLAYER-(viewIndex-1)+MyChair,cmd.GAME_PLAYER)
end

function GameLayer:getSitUserInfoByChairID(chairId)
    --dump(self.m_tabUserItem, "the user item is =============>", 6)
    local useritem
    for k,v in pairs(self.m_tabUserItem) do 
        if v.wChairID == chairId then
           useritem = v
           break
        end
    end

    return useritem
end


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

function GameLayer:getParentNode( )
    return self._scene
end

--系统消息
function GameLayer:onSystemMessage( wType,szString )
    if wType == 501 or wType == 515 then
        local msg = szString or "你的游戏币不足，无法继续游戏"
        local query = QueryDialog:create(msg, function(ok)
                if not (GlobalUserItem.dwCurRoomGroupID ~= 0 and GlobalUserItem.bPrivateGoldRoom) then
                    self:onExitTable()
                end
            end):setCanTouchOutside(false)
                :addTo(self)
    end
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

------继承函数
function GameLayer:getVideoLayerConfig()
    return 
    {
        pos = cc.p(667, 220),
        wChairCount = self._gameFrame._wChairCount,
        maskOpacity = 50
    }
end

--显示开始游戏按钮
function GameLayer:onEventShowBegin(dwWaitTime)
    print("*************************显示开始游戏按钮************************")
    self._gameView:showSitBtn(dwWaitTime ~= 0 and true or false,dwWaitTime)
end

-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)

    local myUserItem = self:GetMeUserItem()
    self.m_bLookOn = false
    if myUserItem.cbUserStatus == yl.US_LOOKON then
        self.m_bLookOn = true
    end
    print("self.m_bLookOn",self.m_bLookOn)

    print("场景数据:" .. cbGameStatus)
    if self.m_bOnGame then
        return
    end

    --print(GlobalUserItem.tabAccountInfo.dwLockServerID,"GlobalUserItem.tabAccountInfo.dwLockServerID")
    print("GlobalUserItem.nCurRoomIndex" , GlobalUserItem.nCurRoomIndex)
    self.m_bOnGame = true
    self.m_cbGameStatus = cbGameStatus
    self._gameView:onResetView()
    self:OnInitGameEngine()

    self.m_nowRoomStatus = cbGameStatus

    local MyChair = self:GetMeChairID() 

    if MyChair>=0 and MyChair<9 then
        self._wchair  = MyChair
    end

    self.m_tabUserItem[self._wchair+1] = self:GetMeUserItem()
    self._gameView:showUserInfo(self:GetMeUserItem(),self.m_lTakeScore[self._wchair+1])

    if self._wchair == yl.INVALID_CHAIR or  not (self._wchair>=0 and self._wchair<9 ) then
        showToast(self, "自己的椅子号不存在！", 2)
        return 
    end

        --初始化已有玩家   
    local tableId = self._gameFrame:GetTableID()
    for i = 1, cmd.GAME_PLAYER do
        if i~= 1 then
            self._gameView:deleteUserHg(i)
        end
        --if i ~= self._wchair+1 then
            self._gameFrame:QueryUserInfo(tableId, i-1)
        --end
    end
    dump(self:GetMeUserItem() , "我的信息！！！！！！！！")

    self._gameView:showChangeBtn( false )
    if cbGameStatus == cmd.GS_FREE  then                        --空闲状态
        self:onEventGameSceneFree(dataBuffer)
    elseif cbGameStatus == cmd.GS_PLAY then                
        self:onEventGameSceneStatus(dataBuffer)
    elseif cbGameStatus == cmd.GS_END then
        self:onEventGameSceneGameEnd(dataBuffer)    
    end
    --
    self._gameView:checkAdress()
    
    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
        self._gameView._priView:initPriRoomInfo()
        self._gameView:showLeftSecond();
        self._gameView:updatePriSecond()
        self._gameView:onEventGetRecord()  --主动请求战绩

    end
   
    self._gameView:showSitBtn(self.m_bLookOn)
    self._gameView:setSeatEvent()
    self:dismissPopWait()

end

function GameLayer:onEventGameSceneFree( dataBuffer )
   local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
  dump(cmd_data , "111")

   --游戏变量
    self.m_lCellScore = cmd_data.lCellScore --盲注
    self.m_lAnteScore = cmd_data.lAnteScore --ante
    self.m_lMinBetScore = cmd_data.lMinBetScore --最低携带
    self.m_lMaxBetScore = cmd_data.lMaxBetScore --最高携带
    self.m_lAllBetScore = cmd_data.lAllBetScore --累计带入
    self.m_lTakeScore = clone(cmd_data.lUserMaxScore[1])
    self.m_cbMaxPlayCount = cmd_data.cbMaxPlayerCount


    self.playMode = cmd_data.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.playMode == 1 or false

   --时间
    self.m_startTime = cmd_data.cbStartTime
    self.m_betTime   = cmd_data.cbBetTime

    if PriRoom and GlobalUserItem.bPrivateRoom then
        self.m_cbPriTime = cmd_data.cbTime
        self.m_cbLeftSecond = cmd_data.nLeftSecond
    end

    
    self._gameView:showReadyBtn(true)
    self._gameView:showChangeBtn( not (PriRoom and GlobalUserItem.bPrivateRoom) )
    self._gameView._rootNode:getChildByName("ready_1"):setVisible(false)

   
    if self.m_lTakeScore[self._wchair+1] <=0 then
        --self._gameView:setTakeChip()
        self._gameView:onEventGetEnableScore()
    end

    for i=1,cmd.GAME_PLAYER do
        if self.m_tabUserItem[i] then
            self._gameView:showUserInfo(self.m_tabUserItem[i],self.m_lTakeScore[i])
        end
    end

    if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom or GlobalUserItem.wCurRoomGroupID ~= 0 then
        self._gameView:setProgressTimer(self.m_startTime,cmd.MY_VIEW_INDEX)
    end
 
    --self:checkIsGayOtherSit()
end

function GameLayer:onEventGameSceneStatus( dataBuffer )
    print("the data len ==========",dataBuffer:getlen())
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer)
dump(cmd_data , "123456789")
    --时间
    self.m_startTime = cmd_data.cbStartTime
    self.m_betTime   = cmd_data.cbBetTime

    --游戏变量
    self.m_lCellScore = cmd_data.lCellScore --盲注
    self.m_lAnteScore = cmd_data.lAnteScore --ante
    self.m_lMinBetScore = cmd_data.lMinBetScore --最低携带
    self.m_lMaxBetScore = cmd_data.lMaxBetScore --最高携带
    self.m_lAllBetScore = cmd_data.lAllBetScore --累计带入

    self.playMode = cmd_data.cbPlayMode
    GlobalUserItem.bPrivateGoldRoom = self.playMode == 1 or false
	
	self.m_wDUser = cmd_data.wDUser
    self.m_wCurrentUser = cmd_data.wCurrentUser
    self.m_cbBigBlind = cmd_data.wMaxChipInUser
    self.m_cbSmallBlind = cmd_data.wMinChipInUser
    self.m_lTurnMaxScore = cmd_data.lTurnMaxScore
    self.m_lTurnLessScore = cmd_data.lTurnLessScore
    self.m_lAddLessScore = cmd_data.lAddLessScore
	self.m_lTableScore = clone(cmd_data.lTableScore[1])
	self.m_lUserAddScore = clone(cmd_data.lTotalScore[1])
    self.m_lTakeScore = clone(cmd_data.lUserMaxScore[1])
    self.m_cbPlayStatus = clone(cmd_data.cbPlayStatus[1])

    self.m_cbMaxPlayCount = cmd_data.cbMaxPlayerCount

    --动态加入
    --[[if (self.m_cbPlayStatus[self:GetMeChairID()+1] == 0 and 
        cmd_data.cbPlayDynamicJoin[self:GetMeChairID()+1] == 1) or
         cmd_data.bStandUp[self:GetMeChairID()+1] == 1 then 
       self._gameView:showSitStatus(0,cmd.MY_VIEW_INDEX)
       self._gameView:showSitAnimation()
    end]]

    if PriRoom and GlobalUserItem.bPrivateRoom then
        self.m_cbPriTime = cmd_data.cbTime
        self.m_cbLeftSecond = cmd_data.nLeftSecond
    end

	local lTotal = 0
    for i=1,cmd.GAME_PLAYER do
       if self.m_cbPlayStatus[i] == 1 then 
        
          --显示桌面筹码
          if self.m_lTableScore[i] > 0 then
              self._gameView:showAddChipScore(self:getUserIndex(i-1),true)
              self._gameView:setChipScore(self:getUserIndex(i-1),self.m_lTableScore[i])
          end
          
          --刷新携带
          self._gameView:updateUserInfo(self:getUserIndex(i-1),self.m_lTakeScore[i])
       end
       lTotal = lTotal + self.m_lUserAddScore[i]
    end    

    --刷新携带
    for i=1,cmd.GAME_PLAYER do
        if self.m_lTakeScore[i] > 0 then
            self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),cc.CallFunc:create(function()
                 --self._gameView:showSitStatus(1,self:getUserIndex(i-1))
                 self.m_lTakeScore[i] = self.m_lTakeScore[i] -  self.m_lUserAddScore[i]
                
                 self._gameView:showUserInfo(self.m_tabUserItem[i],self.m_lTakeScore[i])
            end)))
        end
    end

    --显示底池
    self._gameView:showDeskChipScore(true)
    self._gameView:updateDeskChip(lTotal)

    --显示D玩家
    self._gameView:showDUser(self.m_wDUser)

    --扑克数据
    self.m_cbHandCardData = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}
    self.m_cbHandCardData[self._wchair+1] = clone(cmd_data.cbHandCardData[1])
    self.m_cbCenterCardData = clone(cmd_data.cbCenterCardData[1])

    --2张底牌
    local actions = {}
    actions.round = 1
    actions.interval = 0.0
    actions.perCount = 2    --发2张底牌
    actions.views = {}
    for i=cmd.GAME_PLAYER,1,-1 do
        if self.m_cbPlayStatus[i] == 1  then
            local viewIndex = self:getUserIndex(i-1)
            table.insert(actions.views, viewIndex)
        end
    end
    self._gameView:sendCard(actions,false)
    --dump(actions,"游戏场景2张底牌")re

    
    --第1次下注平衡后就开始发给三张公牌
    --第2次下注平衡后就开始发第四张公牌
    --第3次下注平衡后就开始发第五张公牌
    --第4次下注平衡后就结束游戏
    --开始发公牌
    self.m_cbBalanceCount = cmd_data.cbBalanceCount
    if cmd_data.cbBalanceCount > 0 then
        local actions = {}
        actions.round = 2
        actions.interval = 0.1
        actions.perCount = cmd_data.cbBalanceCount + 2
        actions.perCount = actions.perCount>5 and 5 or actions.perCount
        dump(m_cbCenterCardData, "the send data", 6)
        self._gameView:sendCard(actions,false) 
        dump(actions,"游戏场景5张底牌")
    end

    --显示我的牌型
    if 0 ~= cmd_data.cbCardType then
        self._gameView:showMyCardType(cmd_data.cbCardType)
    end

    --判断是否平衡
    local bBalance = true
    local lBalance = -1
    for i=1,cmd.GAME_PLAYER do
       if self.m_cbPlayStatus[i] == 1 then 
           if lBalance > 0 and lBalance ~= self.m_lTableScore[i] then
               bBalance = false
               break
           end
           lBalance = self.m_lTableScore[i]
       end
    end
    print("bBalance" , bBalance)
    print("bBalance" , bBalance)

     --操作玩家是自己
    local userItem = self:GetMeUserItem()
    if self.m_wCurrentUser == self._wchair then
        local operates = {bPre = false ,datas={}} 
        if bBalance and lBalance > 0 then
            --assert(self._wchair == self.m_cbBigBlind) --大盲玩家是自己
            --加注,过牌,弃牌
            operates.datas = {
                                {id=1,status=true},
                                {id=4,status=true},
                                {id=3,status=true}
                             }
        end

        if bBalance and (lBalance==0) then
               --加注,过牌,弃牌
               operates.datas = {              
                                    {id=1,status=true},
                                    {id=4,status=true},
                                    {id=3,status=true}
                                }
        end

        if not bBalance then
           --加注,跟注,弃牌
           --加注,跟注,弃牌
           operates.datas = {              
                                {id=1,status=true},
                                {id=(self.m_lTurnLessScore >= self.m_lTakeScore[self._wchair+1] - self.m_lUserAddScore[self._wchair+1] and 5 or 2),status=true},
                                {id=3,status=true}
                            }
        end
        self._gameView:updateControl(operates,true)
    else

        if self.m_cbPlayStatus[self._wchair+1] == 1 then 
            --预操作按钮
            local operates = {bPre = true ,datas={
                                                    --跟任何注,过牌,弃牌
                                                    {id=1,status=true},
                                                    {id=2,status=false},
                                                    {id=3,status=true}
                                                 }
                             } 

            self._gameView:updateControl(operates,true)
        end
        
    end

    if self.m_cbPlayStatus[self._wchair+1]~=1  then
        self.m_cbGameStatus = cmd.GS_FREE
        self._gameView:showChangeBtn( not (PriRoom and GlobalUserItem.bPrivateRoom))
        self._gameView:showReadyBtn(false)
        --self._gameView.btn_change1:setPositionX(0)
    end
    


    --self:checkIsGayOtherSit()
    --倒计时
    self._gameView:setProgressTimer(cmd_data.cbTimeLeave,self:getUserIndex(self.m_wCurrentUser))
   
end

function GameLayer:onEventGameSceneGameEnd( dataBuffer )

    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusEnd, dataBuffer)

    --时间
    self.m_startTime = cmd_data.cbStartTime
    self.m_betTime   = cmd_data.cbBetTime

    --游戏变量
    self.m_lCellScore = cmd_data.lCellScore --盲注
    self.m_lAnteScore = cmd_data.lAnteScore --ante
    self.m_lMinBetScore = cmd_data.lMinBetScore --最低携带
    self.m_lMaxBetScore = cmd_data.lMaxBetScore --最高携带
    self.m_lAllBetScore = cmd_data.lAllBetScore --累计带入
    
    self.m_wDUser = cmd_data.wDUser
    self.m_wCurrentUser = cmd_data.wCurrentUser
    self.m_cbBigBlind = cmd_data.wMaxChipInUser
    self.m_cbSmallBlind = cmd_data.wMinChipInUser
    self.m_lTurnMaxScore = cmd_data.lTurnMaxScore
    self.m_lTurnLessScore = cmd_data.lTurnLessScore
    self.m_lAddLessScore = cmd_data.lAddLessScore
    self.m_lTableScore = clone(cmd_data.lTableScore[1])
    self.m_lUserAddScore = clone(cmd_data.lTotalScore[1])
    self.m_lTakeScore = clone(cmd_data.lUserMaxScore[1])
    self.m_cbPlayStatus = clone(cmd_data.cbPlayStatus[1])

    --动态加入
    --[[if (self.m_cbPlayStatus[self:GetMeChairID()+1] == 0 and 
        cmd_data.cbPlayDynamicJoin[self:GetMeChairID()+1] == 1) or
         cmd_data.bStandUp[self:GetMeChairID()+1] == 1 then 
       self._gameView:showSitStatus(0,cmd.MY_VIEW_INDEX)
       self._gameView:showSitAnimation()
    end]]

     if PriRoom and GlobalUserItem.bPrivateRoom then
        self.m_cbPriTime = cmd_data.cbTime
        self.m_cbLeftSecond = cmd_data.nLeftSecond
    end

    local lTotal = 0
    for i=1,cmd.GAME_PLAYER do
       if self.m_cbPlayStatus[i] == 1 then 
        
          --显示桌面筹码
          if self.m_lTableScore[i] > 0 then
              self._gameView:showAddChipScore(self:getUserIndex(i-1),true)
              self._gameView:setChipScore(self:getUserIndex(i-1),self.m_lTableScore[i])
          end
          
          --刷新携带
          self._gameView:updateUserInfo(self:getUserIndex(i-1),self.m_lTakeScore[i])
       end
       lTotal = lTotal + self.m_lUserAddScore[i]
    end    

    --刷新携带
    for i=1,cmd.GAME_PLAYER do
        if self.m_lTakeScore[i] > 0 then
            self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),cc.CallFunc:create(function()
                 --self._gameView:showSitStatus(1,self:getUserIndex(i-1))
                 
                 self._gameView:showUserInfo(self.m_tabUserItem[i],self.m_lTakeScore[i] - self.m_lUserAddScore[i])
            end)))
        end
    end

    --显示底池
    self._gameView:showDeskChipScore(true)
    self._gameView:updateDeskChip(lTotal)

    --显示D玩家
    self._gameView:showDUser(self.m_wDUser)

     --扑克数据
    self.m_cbHandCardData = clone(cmd_data.cbCardData)
    self.m_cbCenterCardData = clone(cmd_data.cbCenterCardData[1])

    --2张底牌
    local actions = {}
    actions.round = 1
    actions.interval = 0.0
    actions.perCount = 2    --发2张底牌
    actions.views = {}
    for i=1,cmd.GAME_PLAYER do
        if self.m_cbPlayStatus[i] == 1 then
            local viewIndex = self:getUserIndex(i-1)
            table.insert(actions.views, viewIndex)
        end
    end
    self._gameView:sendCard(actions,false)
    dump(actions,"结束场景2张底牌")

    --公牌
    self.m_cbBalanceCount = cmd_data.cbBalanceCount
    if cmd_data.cbBalanceCount > 0 then
        local actions = {}
        actions.round = 2
        actions.interval = 0.1
        actions.perCount = cmd_data.cbBalanceCount + 2
        --dump(m_cbCenterCardData, "the send data", 6)
        self._gameView:sendCard(actions,true) 
        dump(actions,"结束场景5张底牌")
    end
		
   --结束
   self._gameView:showGameConclude(cmd_data)
end


--游戏协议
function GameLayer:onEventGameMessage(sub,dataBuffer)
    if nil == self._gameView then
        return
    end
    print("onEventGameMessage ======== >"..sub)
    local MyChair = self:GetMeChairID() 
    if not (self._wchair>=0 and self._wchair<9)  then
          self._wchair  = MyChair
    end
    if sub == cmd.SUB_S_GAME_START  then                     --游戏开始
        self.m_cbGameStatus = cmd.GS_PLAY
        self.m_nowRoomStatus = cmd.GS_PLAY
        self:onSubGameStart(dataBuffer)     
        
    elseif sub == cmd.SUB_S_ADD_SCORE then                   --游戏加注
        self:onSubAddScore(dataBuffer)   
    elseif sub == cmd.SUB_S_GIVE_UP then                     --玩家放弃
        self:onSubGiveUp(dataBuffer)     
    elseif sub == cmd.SUB_S_SEND_CARD then                   --发牌消息
        self:onSubSendCard(dataBuffer)   
    elseif sub == cmd.SUB_S_GAME_CONCLUDE then               --游戏结束
        
        self:onSubGameConclude(dataBuffer)  

        self._gameView:onEventGetRecord()  --主动请求战绩
    elseif sub == cmd.SUB_S_GAME_FREE then                   --游戏结束
        self.m_cbGameStatus = cmd.GS_FREE
        self.m_nowRoomStatus = cmd.GS_FREE
        self:onSubGameFree(dataBuffer)  

        
    elseif sub == cmd.SUB_S_SET_CHIP then                    --设置携带
        self:onSubSetChip(dataBuffer)
    elseif sub == cmd.SUB_S_SHOW_CARD then                   --玩家亮牌
        self:onSubShowCard(dataBuffer)  
    elseif sub == cmd.SUB_S_STAND_UP then                    --玩家起立
        self:onSubStandUp(dataBuffer)   
    elseif sub == cmd.SUB_S_RECORD then                      --房卡流水
        self:onSubPriRecord(dataBuffer)
    elseif sub == cmd.SUB_S_POOL_INFO then                      --游戏边池
        self:onSubPoolInfo(dataBuffer)  
    elseif sub == cmd.SUB_S_REST_SCORE then                     --获取可以买入的筹码
        self:onSubGetEnbleScore(dataBuffer)                    --获取可以买入的筹码
    elseif sub == cmd.SUB_S_LAST_RECORD then  --牌局回顾
        self:onSubLastRecord(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_OVER then
        self.m_cbGameStatus = cmd.GS_FREE
        self.m_nowRoomStatus = cmd.GS_FREE
        self:onSubGameOver(dataBuffer)
    else
        --assert("unKnown message")                                                                                                                                                                                                                    --todo                                                                                                         --todo                                                                                                           --todo                                                                                                           --todo                                                                                                       --todo                                                                                                       
    end
    
end

--游戏空闲
function GameLayer:onSubGameFree( dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameFree, dataBuffer)
    --显示progressTimer
    self._gameView:setProgressTimer(self.m_startTime,cmd.MY_VIEW_INDEX)


    --显示准备
    self._gameView:showReadyBtn(true)
    self._gameView:showChangeBtn( not (PriRoom and GlobalUserItem.bPrivateRoom))

    --携带筹码
    -- if cmd_data.bAutoStart then
    --     self.m_lTakeScore = clone(cmd_data.lUserMaxScore[1])
    
    --     --刷新携带
    --     for i=1,cmd.GAME_PLAYER do
    --         if self.m_lTakeScore[i] > 0 then
    --             self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),cc.CallFunc:create(function()
    --                  --self._gameView:showSitStatus(1,self:getUserIndex(i-1))
                     
    --                  self._gameView:showUserInfo(self.m_tabUserItem[i],self.m_lTakeScore[i])
    --             end)))
    --         end
    --     end
    -- end
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
    dump(cmd_data, "游戏开始", 6)
    self.m_bOnGame = false
    self._gameView:showSitBtn(self.m_bLookOn)
    --游戏变量
    self.m_wDUser = cmd_data.wDUser
    self.m_wCurrentUser = cmd_data.wCurrentUser
    self.m_cbBigBlind = cmd_data.wMaxChipInUser
    self.m_cbSmallBlind = cmd_data.wMinChipInUser
    self.m_lTurnMaxScore = cmd_data.lTurnMaxScore
    self.m_lTurnLessScore = cmd_data.lTurnLessScore
    self.m_lAddLessScore = cmd_data.lAddLessScore
    self.m_lTakeScore = clone(cmd_data.lUserMaxScore[1])

    self.m_nGameEndDelay = 2

    self._gameView:showChangeBtn(false)

    if PriRoom and GlobalUserItem.bPrivateRoom then
        self.m_cbLeftSecond = self.m_cbLeftSecond-1
        self._gameView:updatePriSecond()
    end

    --扑克数据
    self.m_cbHandCardData = clone(cmd_data.cbCardData)

    --玩家状态
    self.m_cbPlayStatus = clone(cmd_data.cbPlayStatus[1]) 

    --默认大小盲(such as 大盲10 小盲5) + ante
    --self.m_lTableScore[self.m_cbBigBlind+1] = self.m_lCellScore
    --self.m_lTableScore[self.m_cbSmallBlind+1] = self.m_lCellScore/2

    --下注额
    --self.m_lUserAddScore[self.m_cbBigBlind+1] = self.m_lTableScore[self.m_cbBigBlind+1]
    --self.m_lUserAddScore[self.m_cbSmallBlind+1] = self.m_lTableScore[self.m_cbSmallBlind+1]

    local lTotal = 0
    for i=1,cmd.GAME_PLAYER do
       if self.m_cbPlayStatus[i] == 1 then 
          --if self.m_lAnteScore > 0 then
            --固定底注
             self.m_lTableScore[i] = cmd_data.lUserTableScore[1][i] --self.m_lTableScore[i] + self.m_lAnteScore
             self.m_lUserAddScore[i] = cmd_data.lUserTableScore[1][i]--self.m_lUserAddScore[i] + self.m_lAnteScore
          --end 

          --显示桌面筹码
          --self._gameView:showAddChipScore(self:getUserIndex(i-1),true)
          self._gameView:setChipScore(self:getUserIndex(i-1),self.m_lTableScore[i],true)

          --刷新携带
          self.m_lTakeScore[i] = self.m_lTakeScore[i] - self.m_lUserAddScore[i]
          self._gameView:updateUserInfo(self:getUserIndex(i-1),self.m_lTakeScore[i])

          self._gameView:showReadyFlag(self:getUserIndex(i-1),false)
       end
       lTotal = lTotal + self.m_lUserAddScore[i]

    end       

    --显示底池
    --self._gameView:showDeskChipScore(true)
    --self._gameView:updateDeskChip(lTotal)

    --显示D玩家
    self._gameView:showDUser(self.m_wDUser)

    --发送2张底牌
    local actions = {}
    actions.round = 1
    actions.interval = 0.1
    actions.perCount = 2    --发2张底牌
    actions.views = {}
    for i=cmd.GAME_PLAYER,1,-1 do
        if self.m_cbPlayStatus[i] == 1 then
            local viewIndex = self:getUserIndex(i-1)
            table.insert(actions.views, viewIndex)
        end
    end
    self._gameView:sendCard(actions,true,true)
    dump(actions,"游戏开始2张底牌")

    --操作玩家是自己
    local userItem = self:GetMeUserItem()
    if self.m_wCurrentUser == self._wchair then
        local operates = {bPre = false ,datas={
                                                --加注,跟注,弃牌
                                                {id=1,status=true},
                                                {id=2,status=true},
                                                {id=3,status=true}
                                              }
                         } 

        self._gameView:updateControl(operates)
    else
        --预操作按钮
        local operates = {bPre = true ,datas={
                                                --跟任何注,过牌,弃牌
                                                {id=1,status=true},
                                                {id=2,status=false},
                                                {id=3,status=true}
                                             }
                         } 

        self._gameView:updateControl(operates)  
    end

    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount +1
            self._gameView._priView:onRefreshInfo()
        end
    end

    --倒计时
    self._gameView:setProgressTimer(self.m_betTime,self:getUserIndex(self.m_wCurrentUser))
end

--加注
function GameLayer:onSubAddScore(dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_AddScore, dataBuffer)

    dump(cmd_data, "加注消息", 6)

    

    --游戏变量
    self.m_wCurrentUser = cmd_data.wCurrentUser
    self.m_lTurnLessScore = cmd_data.lTurnLessScore
    self.m_lTurnMaxScore = cmd_data.lTurnMaxScore
    self.m_lAddLessScore = cmd_data.lAddLessScore
    self.m_lAddScoreCount = cmd_data.lAddScoreCount

    --累计金币
    self.m_lTableScore[cmd_data.wAddScoreUser+1] = self.m_lTableScore[cmd_data.wAddScoreUser+1] + cmd_data.lAddScoreCount
    self.m_lUserAddScore[cmd_data.wAddScoreUser+1] = self.m_lUserAddScore[cmd_data.wAddScoreUser+1] + cmd_data.lAddScoreCount

    --用户携带
    --assert(self.m_lTakeScore[cmd_data.wAddScoreUser+1] - cmd_data.lAddScoreCount >= 0)
    if self.m_lTakeScore[cmd_data.wAddScoreUser+1] - cmd_data.lAddScoreCount >=0 then
        self.m_lTakeScore[cmd_data.wAddScoreUser+1] = self.m_lTakeScore[cmd_data.wAddScoreUser+1] - cmd_data.lAddScoreCount
        --刷新携带
        self._gameView:updateUserInfo(self:getUserIndex(cmd_data.wAddScoreUser),self.m_lTakeScore[cmd_data.wAddScoreUser+1])
    end

    --显示加注
    --self._gameView:showAddChipScore(self:getUserIndex(cmd_data.wAddScoreUser),true)
    self._gameView:setChipScore(self:getUserIndex(cmd_data.wAddScoreUser),self.m_lTableScore[cmd_data.wAddScoreUser+1],true)
	

	
    --判断操作类型 1加注 2跟注 3弃牌 4梭哈 5过牌
    local nOperate = 65535
    if cmd_data.lAddScoreCount == 0 then 
        --过牌
        if self.m_cbPlayStatus[cmd_data.wAddScoreUser+1] ~= 0 then
           nOperate = cmd.CHIP_PASS
           self._gameView:playSound("check.wav",cmd_data.wAddScoreUser)
        end
    elseif self.m_lTakeScore[cmd_data.wAddScoreUser+1] == 0 then 
        -- 梭哈
        nOperate = cmd.CHIP_ALLIN
        self._gameView:playSound("allin.wav",cmd_data.wAddScoreUser)
    else
        --找到下注玩家的上一个玩家
        local cbPreUsr = cmd_data.wAddScoreUser
        for i=1,cmd.GAME_PLAYER-1 do
           cbPreUsr = math.mod(cmd.GAME_PLAYER-(1-cbPreUsr),cmd.GAME_PLAYER)
           if self.m_cbPlayStatus[cbPreUsr+1] == 1 then 
              break
           end
        end

        --平衡
        if self.m_lTableScore[cbPreUsr+1] == self.m_lTableScore[cmd_data.wAddScoreUser+1] then  
            --操作了跟注
            nOperate = cmd.CHIP_FOLLOW  
            self._gameView:playSound("follow.wav",cmd_data.wAddScoreUser)
        else
            nOperate = cmd.CHIP_ADD
            self._gameView:playSound("raise.wav",cmd_data.wAddScoreUser)
        end  
    end

    --操作显示
    self._gameView:showOperateFlag(self:getUserIndex(cmd_data.wAddScoreUser),nOperate)
	
    --当前玩家是自己
    local userItem = self:GetMeUserItem()
    print("self.m_wCurrentUser",self.m_wCurrentUser )
    print("self._wchair",self._wchair)
    if self.m_wCurrentUser == self._wchair then
        --上家下注筹码大于自己剩余携带只能梭哈
        local operates = {bPre=false,datas={}}
        if self.m_lTableScore[cmd_data.wAddScoreUser+1] >= self.m_lTableScore[self._wchair+1] + self.m_lTakeScore[self._wchair+1] then 
            --加注(false),梭哈,弃牌
            operates.datas = {
                                {id=1,status=false},
                                {id=5,status=true},
                                {id=3,status=true}
                             }
            self._gameView:updateControl(operates)
            if  self.m_wCurrentUser ~= yl.INVALID_CHAIR then
            --倒计时
                self._gameView:setProgressTimer(self.m_betTime,self:getUserIndex(self.m_wCurrentUser))
            end
            return 
        end

        local bBalance = true  --平衡标识
        local lBalance = -1     
        for i=1,cmd.GAME_PLAYER do
            if self.m_cbPlayStatus[i] == 1 then 
                if lBalance >= 0 and lBalance ~= self.m_lTableScore[i] then
                    bBalance = false
                    break
                end
                lBalance = self.m_lTableScore[i]
            end
        end

        print("add score....",cmd_data.lAddScoreCount)
        print("bBalance",bBalance)
        dump(self.m_lTableScore, "table score", 6)

        --发公牌之前大盲最后确定是否过牌或者上家过牌
        if cmd_data.lAddScoreCount == 0 then
            if bBalance then
                 --加注,过牌,弃牌
                operates.datas = {
                                    {id=1,status=true},
                                    {id=4,status=true},
                                    {id=3,status=true}
                                 }
            else
               --加注,跟注,弃牌
                operates.datas = {
                                     {id=1,status=true},
                                     {id=2,status=true},
                                     {id=3,status=true}
                                 }                                 
            end
            
        else
            if bBalance == true  then 
               if cmd_data.lTurnLessScore == 0 then -- self.m_cbBigBlind == self._wchair then 
                   --加注,过牌,弃牌
                   operates.datas = {
                                       {id=1,status=true},
                                       {id=4,status=true},
                                       {id=3,status=true}
                                    } 
               end
            else   
                --加注,跟注,弃牌
                operates.datas = {
                                     {id=1,status=true},
                                     {id=2,status=true},
                                     {id=3,status=true}
                                 }     
            end    
        end
        
        self._gameView:updateControl(operates)

    elseif self.m_wCurrentUser ~= yl.INVALID_CHAIR then--显示预操作按钮
        if self.m_cbPlayStatus[self._wchair+1] == 1 then 
           local operates = {bPre = true ,datas={
                                                --跟任何注,过牌,弃牌
                                                {id=1,status=true},
                                                {id=2,status=true},
                                                {id=3,status=true}
                                             } 
                         }                    
             self._gameView:updateControl(operates)  
        end
    end
    if  self.m_wCurrentUser ~= yl.INVALID_CHAIR then
        --倒计时
        self._gameView:setProgressTimer(self.m_betTime,self:getUserIndex(self.m_wCurrentUser))
    end
end

--用户放弃
function GameLayer:onSubGiveUp( dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GiveUp, dataBuffer)
    --设置游戏状态
    self.m_cbPlayStatus[cmd_data.wGiveUpUser+1] = 0
    if cmd_data.wGiveUpUser == self._wchair then 
        self.m_bCanStandUp = true
        self.m_cbGameStatus = cmd.GS_FREE
    end

    self._gameView:playSound("giveup.wav",cmd_data.wGiveUpUser)

    local viewId = self:getUserIndex(cmd_data.wGiveUpUser)
    --显示放弃
    local nOperate = cmd.CHIP_GIVEUP
    self._gameView:showOperateFlag(viewId,nOperate)

    self._gameView:showPlayerScoreChange(cmd_data.lLostScore,viewId)
    --头像变灰
    self._gameView:showGiveUpStatus(viewId)
end

--发牌
function GameLayer:onSubSendCard( dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_SendCard, dataBuffer)
    dump(cmd_data , "11111发牌")
    local sendCardCount = cmd_data.cbSendCardCount - self._gameView:getCenterCardCount()

    if sendCardCount == 5 then
        self.m_nGameEndDelay = 6
    elseif sendCardCount == 2 then
        self.m_nGameEndDelay = 4
    end

    --dump(cmd_data, "cmd data", 6)
    self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ( ... )
            local userItem = self:GetMeUserItem()
            self.m_wCurrentUser = cmd_data.wCurrentUser
            self.m_lTurnLessScore = cmd_data.lTurnLessScore
            self.m_lTurnMaxScore = cmd_data.lTurnMaxScore
            self.m_lAddLessScore = cmd_data.lAddLessScore
            self.m_cbCenterCardData = clone(cmd_data.cbCenterCardData[1])

            self.m_cbBalanceCount = self.m_cbBalanceCount + 1

        
            for i=1,cmd.GAME_PLAYER do
               if self.m_cbPlayStatus[i] == 1 then 
                   
                   self._gameView:showUserInfo(self.m_tabUserItem[i],self.m_lTakeScore[i])
               end
            end

            --开始发牌
            local actions = {}
            actions.round = 2
            actions.interval = 0.1
            actions.perCount = cmd_data.cbSendCardCount - self._gameView:getCenterCardCount()  

            --dump(m_cbCenterCardData, "the send data", 6)
            self._gameView:sendCard(actions,true)
            dump(actions,"发牌消息")


            --显示自己牌型
            if self.m_cbPlayStatus[self._wchair+1] == 1 then 
                self._gameView:showMyCardType(cmd_data.cbCardType)
            end

            --操作玩家是自己
            if self.m_wCurrentUser == self._wchair then
                local operates = {bPre = false ,datas={
                                                        --加注,过牌,弃牌
                                                        {id=1,status=true},
                                                        {id=4,status=true},
                                                        {id=3,status=true}
                                                      } 
                                  }                    
                self._gameView:updateControl(operates)     
            else 
                --预操作按钮
                local operates = {bPre = true ,datas={
                                                        --跟任何注,过牌,弃牌
                                                        {id=1,status=true},
                                                        {id=2,status=true},
                                                        {id=3,status=true}
                                                     }
                                  }                    
                if sendCardCount == 5 or sendCardCount == 2 then
                        operates = nil
                end
                self._gameView:updateControl(operates)
            end

            


             --倒计时
            self._gameView:setProgressTimer(self.m_betTime,self:getUserIndex(self.m_wCurrentUser))
    end)))
    
end

--游戏结算
function GameLayer:onSubGameConclude( dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameConclude, dataBuffer)
    --dump(cmd_data,"==============游戏结束")
    self._gameView:removeTimer()
    
    print("cbTotalEnd==================结算,"..cmd_data.cbTotalEnd)
    --self.m_lTakeScore = clone(cmd_data.lUserMaxScore[1])

    for i=1 , 9 do
        if self.m_cbPlayStatus[i] == 1 and self.m_lTakeScore[i] == 0 then
            self.m_allInConclude = true
        end
    end

    if cmd_data.cbTotalEnd ==1 and self.m_allInConclude then
        
            --显示最大组合牌
        self._gameView:showComplexCard(cmd_data , true)

    end

    
    
    self:runAction(cc.Sequence:create(cc.DelayTime:create(self.m_nGameEndDelay),cc.CallFunc:create(function ( ... )
            self._gameView:updateControl()
            self._gameView:showGameConclude(cmd_data)

    end)))



end



function GameLayer:onSubLastRecord( dataBuffer )
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_LastRecord, dataBuffer)
    dump(cmd_data,"===================")
    self._gameView:showLastTurn(cmd_data)
end




--设置携带
function GameLayer:onSubSetChip( dataBuffer )
    print("设置筹码")
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_SetChip, dataBuffer)
    dump(cmd_data , "设置筹码")
    local wChipUser = cmd_data.wChipUser
    local wUserIndex = self:getUserIndex(wChipUser)
    print("wUserIndex",wUserIndex)

    self.m_lTakeScore[wChipUser+1] = cmd_data.lChipScore
    local meUserItem = self:GetMeUserItem()

    if wUserIndex == cmd.MY_VIEW_INDEX and meUserItem.cbUserStatus < yl.US_READY then 
        self._gameView:showReadyBtn(true)
        self._gameView:showChangeBtn( not (PriRoom and GlobalUserItem.bPrivateRoom))
    end
    self._gameView:showUserInfo(self.m_tabUserItem[wChipUser+1],self.m_lTakeScore[wChipUser+1])
    --self._gameReord.lTakeScore[1][wChipUser+1] = cmd_data.lChipScore
    --self._gameView:refreshRecord()

    self._gameView:onEventGetRecord()  --主动请求战绩

    --self:checkIsGayOtherSit()

end

function GameLayer:checkIsGayOtherSit()
    local playerNum = 0
    for i=1,cmd.GAME_PLAYER do
            if self.m_lTakeScore[i] ~= 0 then
                playerNum = playerNum + 1
            end
    end
    if playerNum == self.m_cbMaxPlayCount then
        for i=1,cmd.GAME_PLAYER do
                if self.m_lTakeScore[i] == 0 then
                        self._gameView:setSitStatus(self:getUserIndex(i-1),false)
                end
        end
    end
end

--用户亮牌
function GameLayer:onSubShowCard(dataBuffer)
    
end

--用户起立
function GameLayer:onSubStandUp(dataBuffer)
    print("用户起立")
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StandUp, dataBuffer)
    self.m_cbPlayStatus[cmd_data.wStandUpUser+1] = 0
    if cmd_data.wStandUpUser == self:GetChairID() then 
       self.m_bCanStandUp = false
       self._gameView:showReadyBtn(false)
    end

    self._gameView:showSitStatus(0,self:getUserIndex(cmd_data.wStandUpUser))
end

--房卡流水记录
function GameLayer:onSubPriRecord(dataBuffer)
   print("流水记录")
   local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_Record, dataBuffer) 
   dump(cmd_data, "流水记录", 6)

    -- if cmd_data.nCount~=0 or not self._gameReord then
        self._gameReord = clone(cmd_data)
    -- end
    if GlobalUserItem.bPrivateRoom then
        self._gameView:refreshRecord()
    end
   
end

--游戏结束
function GameLayer:onSubGameOver(dataBuffer)

        self._gameView:setProgressTimer(self.m_startTime,cmd.MY_VIEW_INDEX)
        
        self._gameView:showReadyBtn(true)
        self._gameView:showChangeBtn( not (PriRoom and GlobalUserItem.bPrivateRoom))

end


--游戏奖池（第一个为底池，其余为边池，要判断数据是否为0）
function GameLayer:onSubPoolInfo(dataBuffer)
    print("奖池信息")
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_PoolInfo, dataBuffer) 
    dump(cmd_data, "奖池信息", 6)

    self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ( ... )

            local lTotal = 0
            local isBet = false
            --上一轮下注额显示
            for i=1,cmd.GAME_PLAYER do
                self._gameView:showAddChipScore(i,false)
                if self.m_lTableScore[i] > 0 then
                    self._gameView:showGoldToTable(self:getUserIndex(i-1))
                    isBet = true
                end
                lTotal = lTotal + self.m_lUserAddScore[i]

            end
            if isBet then
                self._gameView:playSound("bet_s.wav")
            end

                --新一轮
        self.m_lTableScore = {0,0,0,0,0,0,0,0,0}    --当前轮玩家下注额

        --显示底池
        self._gameView:showDeskChipScore(true)
        self._gameView:updateDeskChip(lTotal)

        self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ( ... )

                self._gameView:updateDeskChip(cmd_data.lPoolScore[1][1])
                for i=2,cmd_data.cbPoolCount do
                    if cmd_data.lPoolScore[1][i] > 0 then
                        self._gameView:showTableToPool(i,cmd_data.lPoolScore[1][i])
                    end
                end

        end)))

    end)))

end

function GameLayer:onSubGetEnbleScore(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_REST_SCORE, dataBuffer) 
    local lScore = cmd_data.lScore
    --进入买入界面
    print("onSubGetEnbleScore------------------------>", lScore)
    if (lScore + self.m_lTakeScore[self._wchair+1] )< self.m_lMinBetScore then
        showToast(self, "已达购买上限且余额不足，无法开始游戏！", 2)
    else
        self._gameView:setTakeChip(0, lScore)
    end

end

-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end

function GameLayer:cacheUser(UserItem)
	self.m_tabUserItem[UserItem.wChairID+1] = clone(UserItem)
end

function GameLayer:removeUser(UserItem)
    if self.bPriGameEnd then
        return
    end
	for k,v in pairs(self.m_tabUserItem) do
		if UserItem.dwUserID == v.dwUserID then
                                self._gameView:removeUserFromView(k)
                                --table.remove(self.m_tabUserItem,k)
			self.m_tabUserItem[k] = nil

			--break	
		end
	end
end
--用户状态
function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)
    print("用户状态")
    dump(useritem)
    --print("change user " .. useritem.wChairID .. "; nick " .. useritem.szNickName)
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then

        if (oldstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            return
        end
        
	self:removeUser(useritem)

    else
        if (newstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            return
        end

        self.m_tabUserItem[useritem.wChairID+1] = clone(useritem)
        --self._gameView:onEventGetRecord()
    
        self._gameView:showUserInfo(useritem,self.m_lTakeScore[useritem.wChairID+1])

        self._gameView:showReadyFlag(self:getUserIndex(useritem.wChairID),newstatus.cbUserStatus == yl.US_READY)
        print("+++++++++++++++++++++++++++++++++++",useritem.wChairID , self:GetMeChairID(),newstatus.cbUserStatus)
        if useritem.wChairID == self:GetMeChairID() and newstatus.cbUserStatus == yl.US_READY then
          self._gameView:showReadyBtn(false)
        end

    end    
    self._gameView:checkAdress()
end

--用户进入
function GameLayer:onEventUserEnter(tableid,chairid,useritem)
    --print("用户进入")

  --刷新用户信息
    if tableid ~= self:GetMeUserItem().wTableID and not GlobalUserItem.bVideo then
        return
    end
	--self:cacheUser(useritem)
    self.m_tabUserItem[useritem.wChairID+1] = clone(useritem)
    
    self._gameView:showUserInfo(useritem,self.m_lTakeScore[useritem.wChairID+1])
   
    self._gameView:showReadyFlag(self:getUserIndex(chairid),useritem.cbUserStatus == yl.US_READY)

    self._gameView:playSound("enter.wav")
    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        self._gameView:onEventGetRecord()
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end
    self._gameView:checkAdress()
end

--用户分数
function GameLayer:onEventUserScore( item )
    if item.wTableID ~= self:GetMeUserItem().wTableID then
       return
    end
    -- if item.wChairID == self:GetMeChairID() then
    --     print("用户分数 玩家分数变化",item.lScore)
    --     if (item.lScore + self.m_lTakeScore[self._wchair+1] )< self.m_lMinBetScore then
    --         showToast(self, "已达购买上限且余额不足，无法开始游戏！", 2)
    --     else
    --         self._gameView.m_lUserTakeChip = item.lScore - self.m_lTakeScore[self._wchair+1]
    --     end
    -- end
    --self._gameView:updateScore(item)
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

--约战房处理
function GameLayer:onGetSitUserNum()
    return self._gameView:getUserNum()
end

function GameLayer:getGameRule(buffer)
    dump(buffer,"buffer")
    local strTab = {}
    local mangzhu = ExternalFun.bytesToInt(buffer, 24, 31);
    -- local mangzhu = buffer[24]
    table.insert(strTab, "盲注:"..mangzhu.."/"..mangzhu*2)
    -- local qianzhu = buffer[28]
    local qianzhu = ExternalFun.bytesToInt(buffer, 28, 35);
    table.insert( strTab, "前注:"..qianzhu)
    return strTab
end

function GameLayer:onPlayingUserInfo(userInfo)

    self.m_playingUser = {}
    for i=1,16 do
        self.m_playingUser[i] = userInfo.dwUserID[1][i]
    end
end
--是否参与游戏
function GameLayer:isGaming()

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

    --         self:onExitTable()
        return false
    else
        return true
    end
end
return GameLayer