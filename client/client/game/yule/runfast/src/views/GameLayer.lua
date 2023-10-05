local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")

local GameLayer = class("GameLayer", GameModel)

local cmd = appdf.req(appdf.GAME_SRC.."yule.runfast.src.models.CMD_Game")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.runfast.src.models.GameLogic")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.runfast.src.views.layer.GameViewLayer")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

function GameLayer:ctor(frameEngine, scene)
    GameLayer.super.ctor(self, frameEngine, scene)
end

function GameLayer:CreateView()
    return GameViewLayer:create(self):addTo(self)
end

function GameLayer:OnInitGameEngine()
    GameLayer.super.OnInitGameEngine(self)
    self.wBankerUser = yl.INVALID_CHAIR
    self.wCurrentUser = yl.INVALID_CHAIR
    self.bTrustee = {}
    self.bNewTurn = false
    self.cbCardData = {}
    self.cbTurnCardData = {}
    self.nowTipsCardData = {}
	self.bBtnOutNoneEnabled = nil
	self.wTimeOutCount = 0

	--游戏模式(针对房卡模式)
	self.bSixTheenCard = false  --正常15张扑克
 	self.bNoForceClose = true --不可强关
	self.bNoRemoveBomb = false --炸弹不可拆
    	self.bFirstOutSpade = false --首出必带黑桃3
    	self.cbFirstOutCard = 0x33 --首出牌值
    	self.bSpadeBanker   = true
	self.cbUserMaxCardCount = 15 --默认15张扑克
	self.BeforWinner        = 65535
	self.isPriEnd = false
	self.wPlayerCount = 0
	self.bMyAutoStatus = false
	
	self.m_tabUserItem = {}
	self._MyChairID = self:GetMeChairID()

    self.m_playingUser = {}



end

function GameLayer:OnResetGameEngine()
    GameLayer.super.OnResetGameEngine(self)

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
	 self:KillGameClock()
     self._scene:onKeyBack()
 
end

------继承函数
function GameLayer:getVideoLayerConfig()
    return 
    {
        pos = cc.p(667, 370),
        wChairCount = 3,
        maskOpacity = 50
    }
end

function GameLayer:getUserInfoByChairID( chairid )
    local viewId = self:SwitchViewChairID(chairid)
	local item = self._gameView.m_pUserItems[viewId]
    return item
end


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
                if self.wPlayerCount == 2 then
                	if cha ==0 then
                		viewid = 2
                	elseif math.abs(cha) == 1 then
                		viewid = 1
                	end
                else
                	if cha == 0 then
                		viewid = 2
                	elseif math.abs(cha) == 1 then
                		viewid = cha > 0 and 1 or 3
                	elseif math.abs(cha) == 2 then
                		viewid = cha > 0 and 3 or 1

                	end
                end
    end
    return viewid
end

-- 计时器响应
function GameLayer:OnEventGameClockInfo(chair,time,clockId)
   
	if GlobalUserItem.bVideo  then 
	    	return
	end

    
    
    	if clockId == cmd.IDI_OUT_CARD then --出牌时间
	    	if time <= 0 and chair == self:GetMeChairID() and not self.m_bLookOn then 
	    		--self._gameView:showAutoLayer(true)
	    	end

            if time<=1 then
                if GlobalUserItem.bPrivateGoldRoom or not GlobalUserItem.bPrivateRoom then
                    self._gameView:setGameBtnStatus(false)
                end
            end
	elseif clockId == cmd.IDI_START_GAME then --开始时间
		if time <= 0 and chair == self:GetMeChairID() then
			--self._gameFrame:setEnterAntiCheatRoom(false)--退出防作弊，如果有的话
			if not GlobalUserItem.bPrivateRoom or  GlobalUserItem.bPrivateGoldRoom then
				self:onExitTable()
			end
		end
	end

	if time == 3 then
	    self:PlaySound(GameViewLayer.RES_PATH.."sound/GAME_WARN.WAV")
	end
end

--获取gamekind
function GameLayer:getGameKind()
    return cmd.KIND_ID
end


function GameLayer:onEventUserScore( item )
    if item.wTableID ~= self:GetMeUserItem().wTableID then
       return
    end

    self._gameView:updateUserScore(self:SwitchViewChairID(item.wChairID),item.lScore)
end


--用户聊天
function GameLayer:onUserChat(chat, wChairId)
    self._gameView:userChat(self:SwitchViewChairID(wChairId), chat.szChatString)
end

--用户表情
function GameLayer:onUserExpression(expression, wChairId)
    self._gameView:userExpression(self:SwitchViewChairID(wChairId), expression.wItemIndex)
end

-- 语音播放开始
function GameLayer:onUserVoiceStart( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        --print("语音播放开始 viewid",viewid)
        self._gameView:ShowUserVoice(viewid, true)
    end
end



-- 语音播放结束
function GameLayer:onUserVoiceEnded( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        --print("语音播放结束,viewid",viewid)
        self._gameView:ShowUserVoice(viewid, false)
    end
end

-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end


-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode(cmd.RES_PATH.."rungame_res/ShareLayer.csb"):addTo(self)
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




--用户状态
function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)

    --print("change user " .. useritem.wChairID .. "; nick " .. useritem.szNickName .. "; newstatus " ..newstatus.cbUserStatus .."; oldstatus " ..oldstatus.cbUserStatus )
    dump(useritem,"GameLayer:onEventUserStatus ")
    if newstatus.cbUserStatus == yl.US_FREE or newstatus.cbUserStatus == yl.US_NULL then

 	if self.isPriEnd then
 		return
 	end
        
            for i=1, cmd.GAME_PLAYER do
                if self.m_tabUserItem[i] and self.m_tabUserItem[i].dwUserID == useritem.dwUserID then
                        self.m_tabUserItem[i] = nil
                end
            end

            self._gameView:OnUpdateUserExit(useritem)
           
            
    else
        --print("改变状态")
        if (newstatus.wTableID ~= self:GetMeUserItem().wTableID) then
            --print("tableID is error")
            return
        end
        self.m_tabUserItem[useritem.wChairID+1] = clone(useritem)
        local viewid = self:SwitchViewChairID(useritem.wChairID)
        
        self._gameView:OnUpdateUser(viewid,useritem)
 
    end    
end

--用户进入
function GameLayer:onEventUserEnter(tableid,chairid,useritem)

    --print("the table id is ================>"..tableid.." chairid==>"..chairid)

    dump(useritem,"GameLayer:onEventUserEnter ")

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
    self._gameView:OnUpdateUser(wViewChairId, useritem)

    -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end
end

function GameLayer:onPlayingUserInfo(userInfo)
    self.m_playingUser = {}
    for i=1,16 do
        self.m_playingUser[i] = userInfo.dwUserID[1][i]
    end
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


-- 场景消息
function GameLayer:onEventGameScene(cbGameStatus, dataBuffer)

    local myUserItem = self:GetMeUserItem()
    self.m_bLookOn = false
    if myUserItem.cbUserStatus == yl.US_LOOKON then
        self.m_bLookOn = true
    end

    self.nowTipsCardData = {}
	for i=1,cmd.GAME_PLAYER +1 do
		if self._gameView.nodePlayer[i] then
			self._gameView.nodePlayer[i]:setVisible(false)
			self._gameView._csbNode:getChildByName("ready"..i):setVisible(false)
		end
		
	end

	self._gameView:onResetData()

    if not self.m_bLookOn then
	   self._gameView:OnUpdateUser(cmd.MY_VIEWID, self:GetMeUserItem())
        self.m_tabUserItem[self:GetMeChairID()+1] = self:GetMeUserItem()
    end
    	
    
    	--初始化已有玩家   
    	local tableId = self._gameFrame:GetTableID()
    	for i = 1, cmd.GAME_PLAYER do
            print("场景消息 发送获取已在的玩家！！")
            self._gameFrame:QueryUserInfo(tableId, i-1)
    	end
    	self.m_cbGameStatus = cbGameStatus
	if cbGameStatus == cmd.GAME_SCENE_FREE or cbGameStatus == cmd.GAME_SCENE_GAMEEND then
		print("空闲状态")
		local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusFree,dataBuffer)

		dump(cmd_table, "free scene data ===", 6)
		
		for i = 1, 3 do
			local viewId = self:SwitchViewChairID(i - 1)
			self.bTrustee[viewId] = cmd_table.bAutoStatus[1][i]
		end

		--游戏模式
		self.cbUserMaxCardCount = cmd_table.bSixTheenCard and 16 or 15
		self.bNoForceClose = cmd_table.bNoForceClose --不可强关
	    	self.bNoRemoveBomb = false --cmd_table.bNoRemoveBomb --炸弹不可拆
	    	self.bFirstOutSpade = false --cmd_table.bFirstOutSpade
		self.bSpadeBanker   = cmd_table.bSpadeBanker
		self.bForceOutCard = cmd_table.bForceOutCard
		self.bEchelonConclude = cmd_table.bEchelonConclude
		self.wServerLevel = cmd_table.wServerLevel
		self.wPlayerCount = cmd_table.wPlayerCount
		self.lBaseScore = cmd_table.lBaseScore
		self.bMyAutoStatus = false

		--游戏模式
    		self.cbPlayMode = cmd_table.cbPlayMode
    		GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1  or false

	    	--print("bFirstOutSpade is ===============",self.bFirstOutSpade)
	  
		
		self._gameView.btnStart:setVisible(true)
		
		
		if not GlobalUserItem.bPrivateRoom  or (GlobalUserItem.bPrivateRoom and PriRoom:getInstance().m_tabPriData.dwPlayCount == 0) then
			
			if self.bEchelonConclude then
				self._gameView:showTDtip()
			end

		end

		
        self:SetGameClock(self:GetMeChairID(), cmd.IDI_START_GAME, cmd.TIME_START_GAME)
        
		print("空闲场景消息111")
		
	elseif cbGameStatus == cmd.GAME_SCENE_PLAY then
		

		local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay,dataBuffer)
		dump(cmd_table, "the play status data =====", 6)
		--error("message",0)
		
		print("游戏状态")

		self.lBaseScore = cmd_table.lBaseScore
		
		local wLastOutUser = cmd_table.wLastOutUser
		
		local bCardData = clone(cmd_table.bCardData[1])

		local bCardCount = clone(cmd_table.bCardCount[1])
		--bCardData = {0x31,0x01,0x3b,0x1b,0x0b,0x39,0x29,0x09,0x19,0x08,0x27,0x17,0x03}
		--bCardCount[self:GetMeChairID() + 1] = 13

		local bBombCount = clone(cmd_table.bBombCount[1])
		local bTurnCardCount = cmd_table.bTurnCardCount
		local bTurnCardData = clone(cmd_table.bTurnCardData[1])
		local bAutoStatus = {false,false,false} --clone(cmd_table.bAutoStatus[1])
		local wRoomType = cmd_table.wRoomTyp
		self.bMyAutoStatus = false

		--游戏模式
    		self.cbPlayMode = cmd_table.cbPlayMode
    		GlobalUserItem.bPrivateGoldRoom = self.cbPlayMode == 1  or false


		--游戏模式
		self.BeforWinner = cmd_table.wBeforWiner
		self.cbUserMaxCardCount = cmd_table.bSixTheenCard and 16 or 15
		self.bNoForceClose = cmd_table.bNoForceClose --不可强关
	    	self.bNoRemoveBomb = false --cmd_table.bNoRemoveBomb --炸弹不可拆
	    	--self.bFirstOutSpade = false --cmd_table.bFirstOutSpade
	    	self.bSpadeBanker   = cmd_table.bSpadeBanker
	    	self.bForceOutCard = cmd_table.bForceOutCard
		self.bEchelonConclude = cmd_table.bEchelonConclude
		self.wServerLevel = cmd_table.wServerLevel
		self.wPlayerCount = cmd_table.wPlayerCount

		self.wBankerUser = cmd_table.wBankerUser
		self.wCurrentUser = cmd_table.wCurrentUser

		if self.bSpadeBanker then
			if cmd_table.cbSpadeFirstOutCard ~= yl.INVALID_BYTE then
				self.cbFirstOutCard = cmd_table.cbSpadeFirstOutCard 
				self.bFirstOutSpade = true
			else
				self.bFirstOutSpade = false
			end
		else
			if cmd_table.cbBankerFirstOutCard ~= yl.INVALID_BYTE then
				self.cbFirstOutCard = cmd_table.cbBankerFirstOutCard 
				self.bFirstOutSpade = true
			else
				self.bFirstOutSpade = false
			end
		end


		local myChairId = self:GetMeChairID()
		self.cbCardData = {}
		for i = 1, bCardCount[myChairId + 1] do
			self.cbCardData[i] =  bCardData[i]
		end
        
		local viewCardCount = {0,0,0}
		for i = 1, self.wPlayerCount do
			local viewId = self:SwitchViewChairID(i - 1)
			viewCardCount[viewId] = bCardCount[i]

			self.bTrustee[viewId] = bAutoStatus[i]
            if viewId == cmd.MY_VIEWID then
                self._gameView:showAutoLayer(self.bTrustee[viewId])
            end
			--炸弹
			--self._gameView:setBombNum(viewId, bBombCount[i])
		end

		GameLogic:SortCardList(self.cbCardData)
		self._gameView:gameSendCard(self.cbCardData, viewCardCount,false)


		self._gameView:showCardRecord(self.cbCardData , cmd_table.bRemainCardCount[1])

		self.cbTurnCardData = {}
		for i = 1, bTurnCardCount do
			self.cbTurnCardData[i] = bTurnCardData[i]
		end
		--self.cbTurnCardData = {0x11,0x34,0x24,0x14,0x13}

		local time = cmd_table.cbTimeRemain--cmd.TIME_OUT_CARD
		--if self.wCurrentUser >= 0 and self.wCurrentUser < 3 then
			--self._gameView:setFirstPlayer(self:SwitchViewChairID(wCurrentUser))
		--end
		if bTurnCardCount == 0 then
			self.bNewTurn = true
		end
		
		local lastOutView = self:SwitchViewChairID(wLastOutUser)
		if self.wCurrentUser == myChairId then
			--游戏按钮
			local resultOutCard = self:updateGameBtn()
            if resultOutCard.cbCardCount == 0  then
                self._gameView:autoPlay(true)
                self._gameView._cardLayer:setMyCardTouchEnabled(false)
            elseif self.cbTurnCardData and #self.cbTurnCardData~=0 then
                local cardType = GameLogic:GetCardType(self.cbTurnCardData)
                if cardType==cmd.CT_SINGLE or 
                    cardType==cmd.CT_SINGLE_LINE or 
                    cardType==cmd.CT_DOUBLE_LINE or
                    cardType==cmd.CT_THREE_LINE  or
                    cardType==cmd.CT_BOMB then
                
                    self._gameView._cardLayer:setNotSelectCardEffect(self.cbTurnCardData)
                end
                self._gameView._cardLayer:detectionPopupCard()
            end
		end

        

        


		if lastOutView <= 4 and wLastOutUser ~= self.wCurrentUser  then
			self._gameView._cardLayer:gameOutCard(lastOutView, self.cbTurnCardData, true)
		end

		self:SetGameClock(self.wCurrentUser, cmd.IDI_OUT_CARD, time)

	else
		print("\ndefault\n")
		return false
	end

    if cbGameStatus == cmd.GAME_SCENE_GAMEEND then
        --self.m_cbGameStatus = 0
        --self:onRecord()
        --self._gameView:switchBtnChangeOrTrust()
        --self:onSubGameEnd(dataBuffer)
    end

	self._gameView:showAutoLayer(false)
		    -- 刷新局数
	if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
	       self._gameView._priView:onRefreshInfo()
        end
        if not GlobalUserItem.bPrivateGoldRoom then
            self._gameView.btnTrustee:setVisible(false)
        end
	end
    self._gameView._csbNode:getChildByName("Button_roomInfo"):setVisible(PriRoom and GlobalUserItem.bPrivateRoom)
    self._gameView:switchBtnChangeOrTrust()

	self:onRecord()
	self:dismissPopWait()

    self._gameView.btnSit:setVisible(self.m_bLookOn and self:getSitUserCount()<self.wPlayerCount )

	--self._gameView:ShowUserVoice(2, true) GlobalUserItem.bPrivateGoldRoom
	--self._gameView:showBaoDan(true,1)
	--self._gameView._cardLayer:testxxxx(4)

	return true
end

-- 游戏消息
function GameLayer:onEventGameMessage(sub, dataBuffer)
    -- body
	if sub == cmd.SUB_S_GAME_START then			--游戏开始
		self.m_cbGameStatus = 100
        self._gameView:switchBtnChangeOrTrust()
		return self:onSubGameStart(dataBuffer)
	elseif sub == cmd.SUB_S_OUT_CARD then		--用户出牌
		return self:onSubOutCard(dataBuffer)
	elseif sub == cmd.SUB_S_PASS_CARD then		--放弃出牌
		return self:onSubPassCard(dataBuffer)
	elseif sub == cmd.SUB_S_GAME_END then		--游戏结束
		self.m_cbGameStatus = 0
		self:onRecord()
        self._gameView:switchBtnChangeOrTrust()
		return self:onSubGameEnd(dataBuffer)
   	elseif sub == cmd.SUB_S_AUTOMATISM then		--托管
		return self:onSubUserAutomatism(dataBuffer)
	elseif  sub == cmd.SUB_S_ROOMCARD_RECORD then	--流水记录
		return self:onSubRoomCardRecord(dataBuffer)
	else
		print("\ndefault\n")
		--return false
	end



	return true
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)
	print("onSubGameStart", "tqr")
	local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
	dump(cmd_table,"============")
	local wBankerUser = cmd_table.wBankerUser
	self.wCurrentUser = cmd_table.wCurrentUser
	self.BeforWinner = cmd_table.wBeforWiner
	self.bSpadeBanker   = cmd_table.bSpadeBanker
	self.bForceOutCard = cmd_table.bForceOutCard
	self.bEchelonConclude = cmd_table.bEchelonConclude

	if self.bSpadeBanker then
		if cmd_table.cbSpadeFirstOutCard ~= yl.INVALID_BYTE then
			self.cbFirstOutCard = cmd_table.cbSpadeFirstOutCard 
			self.bFirstOutSpade = true
		else
			self.bFirstOutSpade = false
		end
	else
		if cmd_table.cbBankerFirstOutCard ~= yl.INVALID_BYTE then
			self.cbFirstOutCard = cmd_table.cbBankerFirstOutCard 
			self.bFirstOutSpade = true
		else
			self.bFirstOutSpade = false
		end
	end

	dump({self.bFirstOutSpade,self.cbFirstOutCard},"=====================111")

	self.bNewTurn = true

	local viewBankerUser = self:SwitchViewChairID(wBankerUser)
	local cbCardCount = {self.cbUserMaxCardCount, self.cbUserMaxCardCount, self.cbUserMaxCardCount,self.cbUserMaxCardCount}
	-- if cmd_table.wCurrentUser == yl.INVALID_CHAIR then --有数值为3的炸弹
	-- 	cbCardCount[viewBankerUser] = self.cbUserMaxCardCount - 4
	-- end
	
	for i = 1, cbCardCount[cmd.MY_VIEWID] do
		self.cbCardData[i] = cmd_table.cbCardData[1][i]
	end
    

	GameLogic:SortCardList(self.cbCardData)
	self._gameView:gameSendCard(self.cbCardData, cbCardCount,true)

	--self._gameView:setFirstPlayer(viewBankerUser)
	-- if wBankerUser == self:GetMeChairID() then
	-- 	self._gameView:setGameBtnStatus(true, false, false)
	-- end
	self:SetGameClock(self.wCurrentUser, cmd.IDI_OUT_CARD, cmd.TIME_OUT_CARD)
    self:PlaySound(GameViewLayer.RES_PATH.."sound/GAME_START.WAV")
    self._gameView:showCardRecord(self.cbCardData  ,cmd_table.bRemainCardCount[1])

	-- 刷新局数
	if PriRoom and GlobalUserItem.bPrivateRoom then
	        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
	        	PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
	            self._gameView._priView:onRefreshInfo()
	        end
	end
end

--用户出牌
function GameLayer:onSubOutCard(dataBuffer)
	print("onSubOutCard", "tqr")

	local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_OutCard, dataBuffer)
	dump(cmd_table,"================onSubOutCard")
    self.nowTipsCardData = {}
	local bCardCount = cmd_table.bCardCount
	self.wCurrentUser = cmd_table.wCurrentUser
	local wOutCardUser = cmd_table.wOutCardUser
	local viewId = self:SwitchViewChairID(wOutCardUser)

    self._gameView._cardLayer:cleanNotSelectEffect()

	if self.bNewTurn  then

		for i=1,3 do
			if i ~= viewId then
				self._gameView._cardLayer:removeOutCard(i)
			end
		end
	end

	self.bNewTurn = false
	self.bFirstOutSpade = false
	local outCardData = {}
	for i = 1, bCardCount do
		outCardData[i] = cmd_table.bCardData[1][i]
	end
	--保存
	self.cbTurnCardData = clone(outCardData)

	
	self._gameView._cardLayer:gameOutCard(viewId, outCardData)

	if viewId == cmd.MY_VIEWID then
		GameLogic:RemoveCard(outCardData, self.cbCardData)
        self._gameView:setGameBtnStatus(false)
	end

	local time = cmd.TIME_OUT_CARD
	print("111111111111111111self.bMyAutoStatus",self.bMyAutoStatus)
	if self.wCurrentUser == self:GetMeChairID() and not self.bMyAutoStatus then
		--游戏按钮
		local resultOutCard = self:updateGameBtn()

		if resultOutCard.cbCardCount == 0  then
			self._gameView:autoPlay(true)
			self._gameView._cardLayer:setMyCardTouchEnabled(false)
        elseif self.cbTurnCardData and #self.cbTurnCardData~=0 then
            local cardType = GameLogic:GetCardType(self.cbTurnCardData)
            if cardType==cmd.CT_SINGLE or 
                cardType==cmd.CT_SINGLE_LINE or 
                cardType==cmd.CT_DOUBLE_LINE or
                cardType==cmd.CT_THREE_LINE  or
                cardType==cmd.CT_BOMB then

                    self._gameView._cardLayer:setNotSelectCardEffect(self.cbTurnCardData)
            end

            self._gameView._cardLayer:detectionPopupCard()
		end
		
	end

    

	if self.wCurrentUser ~= yl.INVALID_CHAIR then
		self._gameView._cardLayer:removeOutCard(self:SwitchViewChairID(self.wCurrentUser))
	end
	
    if viewId ~= cmd.MY_VIEWID then
	   self._gameView:showCardRecord(nil , cmd_table.bRemainCardCount[1])
    end

    	self:PlaySound(GameViewLayer.RES_PATH.."sound/OUT_CARD.wav")

	self:SetGameClock(self.wCurrentUser, cmd.IDI_OUT_CARD, time)
end

--放弃出牌
function GameLayer:onSubPassCard(dataBuffer)
	local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_PassCard, dataBuffer)
	print("onSubPassCard===========================")
    self.nowTipsCardData = {}
	self.bNewTurn = cmd_table.bNewTurn
	local wPassUser = cmd_table.wPassUser
	self.wCurrentUser = cmd_table.wCurrentUser
	self.bFirstOutSpade = false
    self._gameView._cardLayer:cleanNotSelectEffect()
	-- if true then
	-- 	return 
	-- end

	--print("self.bNewTurn, bSaMeGui, wPassUser, wCurrentUser/n", self.bNewTurn, bSaMeGui, wPassUser, self.wCurrentUser)

	self._gameView:gamePassCard(self:SwitchViewChairID(wPassUser))

	if self.bNewTurn  then
		self.cbTurnCardData = {}

	end

	local dtime = 0.01
	if  self.wCurrentUser == self:GetMeChairID() and self.bNewTurn then
		dtime = 0.5
	end 
	if self.wCurrentUser == yl.INVALID_CHAIR then
		return
	end
	--print(self.wCurrentUser.."=============================777")

	self:runAction(cc.Sequence:create(cc.DelayTime:create(dtime),cc.CallFunc:create(function ( ... )
		self._gameView._cardLayer:removeOutCard(self:SwitchViewChairID(self.wCurrentUser))
			local time = cmd.TIME_OUT_CARD
		if self.wCurrentUser == self:GetMeChairID() and not self.bMyAutoStatus then
			--游戏按钮
			local resultOutCard = self:updateGameBtn()

			if resultOutCard.cbCardCount == 0  then
				self._gameView:autoPlay(true)
				self._gameView._cardLayer:setMyCardTouchEnabled(false)
            elseif self.cbTurnCardData and #self.cbTurnCardData~=0 then
                local cardType = GameLogic:GetCardType(self.cbTurnCardData)
                if cardType==cmd.CT_SINGLE or 
                    cardType==cmd.CT_SINGLE_LINE or 
                    cardType==cmd.CT_DOUBLE_LINE or
                    cardType==cmd.CT_THREE_LINE  or
                    cardType==cmd.CT_BOMB then
                
                    self._gameView._cardLayer:setNotSelectCardEffect(self.cbTurnCardData)
                end
                self._gameView._cardLayer:detectionPopupCard()
			end

			
		end

		self:SetGameClock(self.wCurrentUser, cmd.IDI_OUT_CARD, time)

	end)) )
	
end

--游戏结束
function GameLayer:onSubGameEnd(dataBuffer)
	print("onSubGameEnd", "tqr")
	local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer)
	dump(cmd_table,"================onSubGameEnd")
	self._gameView:showAutoLayer(false)
    self.nowTipsCardData = {}
	--剩余牌显示
	local cardData = {}			--分发
	local index = 1
	for i = 1, 3 do
		cardData[i] = {}
		for j = 1, cmd_table.bCardCount[1][i] do
			cardData[i][j] = cmd_table.bCardData[1][index]
			index = index + 1
		end
	end
	local viewCardData = {}		--转换
	local viewCardCount = {}
	for i = 1, self.wPlayerCount do
		local viewId = self:SwitchViewChairID(i - 1)
		viewCardData[viewId] = cardData[i]
		viewCardCount[viewId] = cmd_table.bCardCount[1][i]
		if viewId ~= cmd.MY_VIEWID then
			if self.wPlayerCount == 2 and viewId ==4  then
				print("==============gameend========")
				self._gameView._cardLayer:sortOtherPlayerHandCard(viewId,#viewCardData[viewId],viewCardData[viewId])
			else
				self._gameView._cardLayer:gameOutCard(viewId, viewCardData[viewId], true, true)
			end
		end
	end

	--结算框
	local result = {}
	local myTableId = self:GetMeTableID()
	local myChairId = self:GetMeChairID()
	local bMeWin = false
	for i = 1, 3 do
		result[i] = {}
		result[i].lScore = cmd_table.lGameScore[1][i]
		if i - 1 == myChairId and result[i].lScore >= 0 then
			bMeWin = true
		end
		result[i].userItem = self.m_tabUserItem[i]
		result[i].count = cmd_table.bCardCount[1][i]
		result[i].boom = cmd_table.bBombCount[1][i]
	end
	self:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ( ... )
		

		self._gameView._resultLayer:setResult(result, bMeWin,self._MyChairID)

		self._gameView:gameEnded()

		self:PlaySound(GameViewLayer.RES_PATH.."sound/GAME_END.WAV")

		if bMeWin then
			self:PlaySound(GameViewLayer.RES_PATH.."sound/GAME_WIN.wav")
		else
			self:PlaySound(GameViewLayer.RES_PATH.."sound/GAME_LOST.wav")
		end

		self:SetGameClock(self:GetMeChairID(), cmd.IDI_START_GAME, cmd.TIME_START_GAME)

	end)))
	self._gameView:setHandNum(false)
	self._gameView:setGameBtnStatus(false)

	
	self._gameView:showBaoDan(false,1)
    self._gameView:showBaoDan(false,3)
	

	self.bNewTurn = true
	self.wTimeOutCount = 0
	self.bTrustee = {}
    	self.cbCardData = {}
    	self.cbTurnCardData = {}
	
end




function GameLayer:getGameRule(buffer)


        local strTable ={}
        --游戏模式(玩法)
        local playerNum =  buffer[2]--buffer:readbyte()


        table.insert(strTable,playerNum.."人")

        --规则
        local cardMode ={"15张牌","16张牌"}
        table.insert(strTable,cardMode[buffer[3]+1])

        local firstMode = {"赢家首出","黑桃3首出"}
        table.insert(strTable,firstMode[buffer[6]+1])
        local outMode = {"非有大必出","有大必出"}
        table.insert(strTable,outMode[buffer[8]+1])
        local resultMode = {"非梯度结算","梯度结算"}
        table.insert(strTable,resultMode[buffer[9]+1])
    return strTable
end


--托管
function GameLayer:onSubUserAutomatism(dataBuffer)
	print("onSubUserAutomatism", "tqr")
	local cmd_table = ExternalFun.read_netdata(cmd.CMD_S_UserAutomatism, dataBuffer)
	local viewId = self:SwitchViewChairID(cmd_table.wChairID)
	self.bTrustee[viewId] = cmd_table.bTrusee
	print("收到托管消息",viewId,self.bTrustee[viewId])
    if viewId == cmd.MY_VIEWID then
        self._gameView:showAutoLayer(cmd_table.bTrusee)
    end
end

function GameLayer:onSubRoomCardRecord(dataBuffer)
	print("房卡流水记录")
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_RoomCardRecord, dataBuffer)
    dump(cmd_data, "cmd_data 获得了房卡记录")

    --全部成绩
    self.m_userPriRecord ={}
    self.m_userPriRecord.count = cmd_data.nCount
    self.m_userPriRecord.detail = {{},{},{}}
    self.m_userPriRecord.bomboCount = cmd_data.dwBomboCount[1]
	
    for i = 1, cmd_data.nCount do
    	for j=1,cmd.GAME_PLAYER do 
        		self.m_userPriRecord.detail[j][i] = cmd_data.lDetailScore[j][i]
        	end
    end
	
		
end

--提示出牌
function GameLayer:promptCard()
	local bNextLastOne = false 
	if self._gameView._cardLayer.cbCardCount[cmd.MY_VIEWID+1] == 1 then  --如果下家只剩下最后一张牌,自己必须出最大牌
		print("cmd.MY_VIEWID + 1 === ",cmd.MY_VIEWID+1)
		bNextLastOne = true
	end

    local lastCardData = ( #self.nowTipsCardData>0) and self.nowTipsCardData or self.cbTurnCardData
    dump(lastCardData,"lastCardData")
	local resultOutCard = GameLogic:SearchOutCard(self.cbCardData, lastCardData,bNextLastOne,self.bFirstOutSpade,self.cbFirstOutCard)
    
    if resultOutCard.cbCardCount == 0 and ( #self.nowTipsCardData>0) then
        resultOutCard = GameLogic:SearchOutCard(self.cbCardData, self.cbTurnCardData,bNextLastOne,self.bFirstOutSpade,self.cbFirstOutCard)
    end
	
    if resultOutCard.cbCardCount == 0 then
		return nil
	else
        
		return resultOutCard.cbResultCard
	end
end

function GameLayer:updateGameBtn()
	--游戏按钮
	local resultOutCard = GameLogic:SearchOutCard(self.cbCardData, self.cbTurnCardData)		--不出按钮可否点击
	dump(resultOutCard,"===========可以出的牌",12)
	if resultOutCard.cbCardCount == 0 then
		self.bBtnOutNoneEnabled = true
	else
		if self.bForceOutCard then
			self.bBtnOutNoneEnabled = false
		else
			self.bBtnOutNoneEnabled = true
		end
	end

	if self.bNewTurn then
		self.bBtnOutNoneEnabled = false
	end

	local bBtnOutCardEnabled = nil		--出牌按钮可否点击
	local popupCardData = self._gameView._cardLayer:getPopupCard()

	if self:detectionOutCard(popupCardData) then
		bBtnOutCardEnabled = true
	else
		bBtnOutCardEnabled = false
	end
	self._gameView._cardLayer:setMyCardTouchEnabled(true)

	if resultOutCard.cbCardCount > 0 then
		self._gameView:setGameBtnStatus(true, self.bBtnOutNoneEnabled, bBtnOutCardEnabled)
	end

    dump(resultOutCard,"判断是否最后一手牌，resultOutCard：")
    print("我的手牌数量",self._gameView._cardLayer.cbCardCount[cmd.MY_VIEWID])
    if  not self.m_bLookOn then 
        local cardData = self:promptCard()
        if cardData and #cardData == #(self._gameView._cardLayer.cbCardData) then
            self._gameView._cardLayer:popupCard(cardData)
            self._gameView:setGameBtnStatus(false, nil, false)
            self:sendOutCard(cardData)
            resultOutCard.cbCardCount=99
        end
    end
    --

	return resultOutCard
end

function GameLayer:detectionOutCard(popupCardData)
	local bOk = GameLogic:CompareCard(popupCardData, self.cbTurnCardData,self._gameView._cardLayer.cbCardData)
	dump(self.cbTurnCardData,"=======================1")
	dump(popupCardData,"=======================2")
	--dump({bOk},"=====================3")
	if not popupCardData then
		return false
	end
	--三带一牌型只能最后一手牌才可以出
	GameLogic:SortCardList(popupCardData)

	if #popupCardData == #self.cbCardData and self.bNewTurn then
		local cardType = GameLogic:GetCardType(popupCardData,true)
		if cardType == cmd.CT_THREE_LINE  
		or cardType ==  cmd.CT_THREE_LINE_TAKE_SINGLE 
		or cardType ==  cmd.CT_TWO_THREE_LINE_TAKE_TWO_SINGLE 
		or cardType ==  cmd.CT_TWO_THREE_LINE_TAKE_ONE_DOUBLE
		or cardType ==  cmd.CT_THREE_THREE_LINE_TAKE_THREE_SINGLE then
			return true
		end
	end
	-- if bOk and (GameLogic:GetCardType(popupCardData) == cmd.CT_THREE_LINE_TAKE_SINGLE ) and (#self.cbCardData > 4) then
	-- 	bOk = false
	-- 	return bOk
	-- end

	if self.bNoRemoveBomb then --如果是炸弹不可拆
		if bOk then
			--分析扑克
			local analyseResult = GameLogic:AnalyseCardData(self.cbCardData)
			if analyseResult.bFourCount >= 1 then
				for i=1,analyseResult.bFourCount do
					local BombLogicValue = analyseResult.bFourLogicValue[i]
					for j=1,#popupCardData do
						local oneCardValue = GameLogic:GetCardLogicValue(popupCardData[j]) 
						if (oneCardValue == BombLogicValue)  then
							if GameLogic:GetCardType(popupCardData)==cmd.CT_BOMB then
								bOk = true
							else
								bOk = false	
							end
							--dump({bOk},"=====================4")
							return bOk 
						end
					end
				end
			end
		end
	end
	if self._gameView._cardLayer.cbCardCount[cmd.MY_VIEWID+1] == 1 then  --如果下一玩家只剩下最后一张扑克 
		if (#popupCardData == 1) then
			if true == bOk then
				local temp = clone(self.cbCardData)
				GameLogic:SortCardList(temp)
				if  GameLogic:GetCardLogicValue(popupCardData[1]) < GameLogic:GetCardLogicValue(temp[1]) then 
					bOk = false
					--dump({bOk},"=====================5")
					return bOk
				end
			end
		end
	end	

	-- --首出黑桃三
	 --local bFirstOutSpade = self.bFirstOutSpade
	--  if (not bFirstOutSpade) and (self.BeforWinner==65535 and not self.bSpadeBanker) then 
	-- 	bFirstOutSpade = true
	-- end

	-- if bFirstOutSpade then --如果四个三,不必首出
	-- 	local temp = clone(self.cbCardData)
	-- 	if GameLogic:RemoveCard({0x33,0x23,0x13,0x03},temp) then 
	-- 	 	bFirstOutSpade = false
	-- 	 	self.bFirstOutSpade = false	
	-- 	end 
	-- end

	if self.bFirstOutSpade and (#self.cbTurnCardData == 0)  then
		local bHaveBlackThree = false
		for i = #self.cbCardData, 1, -1 do
			if self.cbCardData[i] == self.cbFirstOutCard then
				bHaveBlackThree = true
				break
			end
		end
		if bHaveBlackThree then
			local bOutBlackThree = false
			for i = 1, #popupCardData do
				if popupCardData[i] == self.cbFirstOutCard then
					bOutBlackThree = true
					break
				end
			end
			if not bOutBlackThree then
				bOk = false
			end
		end
	end

	--dump({bOk},"=====================6")

	return bOk
end

--****************************    发送消息      ********************************--
--发送开始消息
function GameLayer:sendStart()
	
    --self._gameView:onResetData()
	return self._gameFrame:SendUserReady()
end


function GameLayer:onRecord()
    print("请求房卡记录")
        local cmd_data = CCmd_Data:create()
    
       return self:SendData(cmd.SUB_C_REQUEST_RCRecord, cmd_data)
end

--发送出牌消息
function GameLayer:sendOutCard(cbCardData)
	local cbCardCount = #cbCardData
	local wDataSize = 1 + cbCardCount
	local cmd_data = CCmd_Data:create(wDataSize)
	cmd_data:pushbyte(cbCardCount)
	for i = 1, cbCardCount do
		cmd_data:pushbyte(cbCardData[i])
	end
	dump(cbCardData,"==============sendOutCard")
	return self:SendData(cmd.SUB_C_OUT_CART, cmd_data)
end

--发送过牌消息
function GameLayer:sendPassCard()
	local cmd_data = CCmd_Data:create(0)
	return self:SendData(cmd.SUB_C_PASS_CARD, cmd_data)
end

--发送托管消息
function GameLayer:sendAutomatism(flag)

	if flag == false and self.wCurrentUser == self:GetMeChairID() then
		
		--游戏按钮
		local resultOutCard = self:updateGameBtn()

		if resultOutCard.cbCardCount == 0  then
			self._gameView:autoPlay(true)
			self._gameView._cardLayer:setMyCardTouchEnabled(false)
        elseif self.cbTurnCardData and #self.cbTurnCardData~=0 then
            local cardType = GameLogic:GetCardType(self.cbTurnCardData)
            if cardType==cmd.CT_SINGLE or 
                cardType==cmd.CT_SINGLE_LINE or 
                cardType==cmd.CT_DOUBLE_LINE or
                cardType==cmd.CT_THREE_LINE  or
                cardType==cmd.CT_BOMB then
                
                self._gameView._cardLayer:setNotSelectCardEffect(self.cbTurnCardData)
            end
            self._gameView._cardLayer:detectionPopupCard()
		end
	end

	local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_Automatism)
	cmd_data:pushbool(flag)
	return self:SendData(cmd.SUB_C_AUTOMATISM, cmd_data)
end

function GameLayer:getSitUserCount()
    local num = 0
    for i=1 , 3 do
        if self._gameView.nodePlayer[i]:isVisible() then
            num = num+1
        end
    end
    return num
end



--流水记录条数
function GameLayer:getPriRecordCount()
	local count = self.m_userPriRecord.count or 0
	
	return count
end

function GameLayer:getPriRecord()
	return self.m_userPriRecord
end
return GameLayer