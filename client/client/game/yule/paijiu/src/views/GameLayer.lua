local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")

local GameLayer = class("GameLayer", GameModel)

local cmd = appdf.req(appdf.GAME_SRC.."yule.paijiu.src.models.CMD_Game")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.paijiu.src.views.layer.GameViewLayer")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.paijiu.src.models.GameLogic")


local IDI_STARTGAME=201
local IDI_CALLBANKER=202
local IDI_SETCHIP=203
local IDI_RANGECARD=204

function GameLayer:ctor(frameEngine, scene)
	self.name="GameLayer"
	GameLayer.super.ctor(self, frameEngine, scene)
	self.myChairId=self:GetMeChairID()
	self.m_cbGameStatus=0
	self.m_cbGameStatus2 = 0;
	print("self.myChairId:",self.myChairId)

	self.SwitchViewChairID =self.SwitchViewChairIDBeforeSceneMsg
end

function GameLayer:CreateView()
	dump(GameViewLayer,"GameViewLayer",6)
    return GameViewLayer:create(self):addTo(self)
end

function GameLayer:OnResetGameEngine()
	GameLayer.super.OnResetGameEngine(self)
	self.m_cbGameStatus=0

	self.m_cbGameStatus2 = 0;
	self.bEnd=false
	self.allCardCount=32
	self.SwitchViewChairID =self.SwitchViewChairIDBeforeSceneMsg
	self._gameView:resetData()
	
end

function GameLayer:getGameKind()
    return cmd.KIND_ID
end

function GameLayer:onGetSitUserNum()
    return table.nums(self._gameView.tabHeadSp)
end

function GameLayer:getUserInfoByChairID( chairid )
    local viewId = self:SwitchViewChairID(chairid)
    return self._gameView.tabUserItemCache[viewId]
end


function GameLayer:OnEventGameClockInfo(chairId,time,clockId)
	if chairId~=self:GetMeChairID() then
		return
	end
	if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
		return
	end

	if not self.bTrustee and time<=4 then
		ExternalFun.playSoundEffect("GAME_WARN.WAV")
	end

	if clockId==IDI_STARTGAME then
		if self.bTrustee and time<=math.max(self.cbTimeStartGame-1,1) then
			self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_READY)
		elseif time<=1  then 
			self._gameFrame:setEnterAntiCheatRoom(false)
            self:onExitTable()
		end
	elseif clockId==IDI_CALLBANKER then
		-- if (self.bTrustee and time<=math.max(self.cbTimeCallBanker-2,1) ) or  time<=1 then 
		-- 	self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_NOROB)
		-- end
	elseif clockId==IDI_SETCHIP then
		-- if self.bTrustee and time<=math.max(self.cbTimeSetChip-2,1)  or time<=1 then

		-- 	local index = ((self._gameView.bIsMaxBet == false) and 1 or #self._gameView.chipBtns)
		-- 	if not #self._gameView.chipBtns then
		-- 		index = 1
		-- 	end

		-- 	self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_CHIP + index)
		-- end
	elseif clockId==IDI_RANGECARD then
		-- if self.bTrustee and time<=math.max(self.cbTimeRangeCard-2,1) or time<=1 then
		-- 	if self.cardNumLimit>2 then
		-- 		self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_NOTIFY)
		-- 	end
		-- 	self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_ZUPAI)
		-- end

		-- if self.bTrustee and time<=math.max(self.cbTimeRangeCard-2,1) then
		-- 	if self.cardNumLimit>2 then
		-- 		self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_NOTIFY)
		-- 	end
		-- 	self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_ZUPAI)
		-- end
	end
end

function GameLayer:setTrustee(bTrustee)
	-- self.bTrustee=bTrustee~=nil and bTrustee or not self.bTrustee
	-- -- self._gameView.trustNode:setVisible(self.bTrustee)
	-- self._gameView.trustNode:setVisible(false)
end

--断线重连取消托管
function GameLayer:setHideTrustee(bTrustee)
	self.bTrustee=bTrustee;
	
	self._gameView.trustNode:setVisible(self.bTrustee)
end

function GameLayer:SetOpenCardClock(passedTime)
	if not GlobalUserItem.bVideo then
		self:SetGameClock(self:GetMeChairID(),IDI_RANGECARD,math.max(1,math.ceil(self.cbTimeRangeCard-passedTime)))
	end
end

function GameLayer:SetGameClock(chair,id,time)
	if GlobalUserItem.bVideo then
		return 
	end
	 GameLayer.super.SetGameClock(self,chair,id,time)
	 -- 设置计时器
    local viewid = self:GetClockViewID()
    if viewid and viewid ~= yl.INVALID_CHAIR then
       	self._gameView:createHeadAnim(viewid)
       	self._gameView:setShowParticle(viewid,time,true)
       	local progress = self._gameView.m_TimeProgress[viewid]  
    	for i=1,2 do
    		if progress[i] then
	            progress[i]:setPercentage(100)
	            progress[i]:setVisible(true)
	            progress[i]:runAction(cc.Sequence:create(cc.ProgressTo:create(time, 0), cc.CallFunc:create(function()
	                progress[i]:setVisible(false)
	            end)))
	        end
        end
    end

end

function GameLayer:KillGameClock(notView)
	if GlobalUserItem.bVideo then
		return 
	end

    local viewid = self:GetClockViewID()
    if viewid and viewid ~= yl.INVALID_CHAIR then
        local progress = self._gameView.m_TimeProgress[viewid] or {}
        for i=1,2 do
    		if progress[i] then
	            progress[i]:stopAllActions()
	            progress[i]:setVisible(false)
	        end
        end

        self._gameView:setShowParticle(viewid,0,false)
    end
    
    GameLayer.super.KillGameClock(self,notView)

end

function GameLayer:SwitchViewChairIDBeforeSceneMsg(chairId)
	-- if self.myChairId==yl.INVALID_CHAIR then --密码房
		self.myChairId=self:GetMeChairID()
	-- end
	--onEventUserStatus调用到此处时仍可能self.myChairId==yl.INVALID_CHAIR
	--print("SwitchViewChairID self:GetMeChairID():",self:GetMeChairID())

	local viewid=cmd.MY_VIEWID +(chairId-self.myChairId)
	return viewid>0 and viewid or viewid+cmd.GAME_PLAYER
end


function GameLayer:SwitchViewChairIDAfterSceneMsg(chairId) --需求：四人约战房应为每边坐一个玩家。
	if self.myChairId==yl.INVALID_CHAIR then --密码房
		self.myChairId=self:GetMeChairID()
	end
	if self.wPlayerCount and chairId>=self.wPlayerCount then return yl.INVALID_CHAIR end
	--onEventUserStatus调用到此处时仍可能self.myChairId==yl.INVALID_CHAIR
	--print("SwitchViewChairID self:GetMeChairID():",self:GetMeChairID())
	
	local viewid=cmd.MY_VIEWID +(chairId-self.myChairId)
	local v=viewid>0 and viewid or viewid+(self.wPlayerCount or cmd.GAME_PLAYER)
	print("chairId,self.myChairId",chairId,self:GetMeChairID())
	print("viewid:",v)
	print("self.wPlayerCount:",self.wPlayerCount)
	assert(self.wPlayerCount)
	assert(v>=1 and v<=self.wPlayerCount)
	return viewid>0 and viewid or viewid+(self.wPlayerCount or cmd.GAME_PLAYER)
end


--创建网络消息包
local function getLenthOfNetData( keyTable ) 
	local len = 0;
	for i=1,#keyTable do
		local keys = keyTable[i];
		local keyType = string.lower(keys["t"]);

		--todo 数组长度计算
		local keyLen = 0;
		if "byte" == keyType or "bool" == keyType then
			keyLen = 1;
		elseif "score" == keyType or "double" == keyType then
			keyLen = 8;
		elseif "word" == keyType or "short" == keyType then
			keyLen = 2;
		elseif "dword" == keyType or "int" == keyType or "float" == keyType then
			keyLen = 4;
		elseif "string" == keyType then
			keyLen = keys["s"];
		elseif "tchar" == keyType then
			keyLen = keys["s"] * 2
		elseif "ptr" == keyType then
			keyLen = keys["s"]
		elseif "table"==keyType then
			keyLen=getLenthOfNetData(keys["d"])
			print(keyTable["k"],"table len:",keyLen)
		else
			print("error keytype ==> ", keyType);
		end

		local multi=0
		local lenTable=keys["l"]
		if lenTable then
			for i=1,#lenTable do
				multi=multi+lenTable[i]
			end
		else
			multi=1
		end

		len = len + keyLen*multi
	end
	print("net len ==> ", len)
	return len
end

function GameLayer:onEventGameScene(cbGameStatus, dataBuffer)
	self.chipMsgTime=nil
	self.cardMsgTime=nil
	self.endMsgTime=nil
	self.bReceivedGameMsg=false
	self._gameView:endRobAnimation()
	ExternalFun.playBackgroudAudio("background.mp3")
	if self.m_bOnGame then
        return
    end

    local function assignTime(data)
    	self.cbTimeStartGame=data.cbTimeStartGame~=0 and data.cbTimeStartGame or 30
		self.cbTimeCallBanker=data.cbTimeCallBanker~=0 and data.cbTimeCallBanker or 10
		self.cbTimeSetChip=data.cbTimeSetChip~=0 and data.cbTimeSetChip or 10
		self.cbTimeRangeCard=data.cbTimeRangeCard~=0 and data.cbTimeRangeCard or 20
    end 

    self.m_cbGameStatus = cbGameStatus

    self.m_cbGameStatus2 = cbGameStatus;

    --自动托管
    self._gameView:resetAutoSet();

    self.m_bOnGame = true
    local data
    if cbGameStatus == cmd.GS_WK_FREE then
    	print("dataBuffer:getlen:",dataBuffer:getlen())
    	print("cmd.CMD_S_StatusFree len:", getLenthOfNetData(cmd.CMD_S_StatusFree))

    	data=ExternalFun.read_netdata(cmd.CMD_S_StatusFree,dataBuffer)
    	dump(data,"cmd.CMD_S_StatusFree:",6)
    	assignTime(data)

		self:onGameSceneFree(data)
	elseif cbGameStatus == cmd.GS_WK_ROBBANKER then
		print("GS_WK_ROBBANKER len:",dataBuffer:getlen())	

		 data=ExternalFun.read_netdata(cmd.CMD_S_StatusRob,dataBuffer)
		 assignTime(data)

		self:onGameSceneRobBanker(data)
	elseif cbGameStatus == cmd.GS_WK_CHIP then

		 data=ExternalFun.read_netdata(cmd.CMD_S_StatusChip,dataBuffer)
		 assignTime(data)

		self:onGameSceneChip(data)
	elseif cbGameStatus == cmd.GS_WK_PLAYING then
		 data=ExternalFun.read_netdata(cmd.CMD_S_StatusOpen,dataBuffer)
		 assignTime(data)

		self:onGameScenePlaying(data)
	end

	GlobalUserItem.bPrivateGoldRoom = data.cbPlayMode == 1

	--自动托管
	self._gameView.btn_trust:setVisible(false)
	--断线重连取消托管
	self:setHideTrustee(false)
	self._gameView:isShowAutoTrust();
	-- self._gameView:isShowAutoTrust(false);

	--托管模式、最大最小下注
	self._gameView.cbTrusteeMode = data.cbTrusteeMode;
    self._gameView:changeMaxOrMinBet();

	self._gameView:setGameInfo(data.bFourCards)

	 -- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            self._gameView._priView:onRefreshInfo()
        end
    end

    if GlobalUserItem.bPrivateGoldRoom then
        if self._gameView._priView then
            self._gameView._priView:onRefreshInfo()
        end
        if self._gameView.tabHeadSp[cmd.MY_VIEWID] then
        	self._gameView.tabHeadSp[cmd.MY_VIEWID]:setRoomerState(false)
        end
        
    end

	self:dismissPopWait()
end


function GameLayer:resetUserItem()
	print(debug.traceback())
	cmd.globalData.wPlayerCount=self.wPlayerCount
	--if self.wPlayerCount==4 then
		self._gameView:initPos()
	--end
	for oldViewId=1,cmd.GAME_PLAYER do
		for j=1,self.wPlayerCount do
			if self:SwitchViewChairID(j-1)==oldViewId then
				local newViewId=self:SwitchViewChairIDAfterSceneMsg(j-1)
				self._gameView:resetUserItem(oldViewId,newViewId)
			end
		end
	end
	self.SwitchViewChairID=self.SwitchViewChairIDAfterSceneMsg

    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        if nil ~= userItem then
            local wViewChairId = self:SwitchViewChairID(i-1)
            self._gameView:OnUpdateUser(wViewChairId, userItem)
        end
    end
end

--回放玩家信息显示
function GameLayer:resetVideoUserItem()
	print(debug.traceback())
	cmd.globalData.wPlayerCount=self.wPlayerCount
	--if self.wPlayerCount==4 then
		self._gameView:initPos()
	--end

	for oldViewId=1,cmd.GAME_PLAYER do
		if self._gameView.tempTabUserItem[oldViewId] then
			self._gameView:resetUserItem(oldViewId,oldViewId)
		end
	end
end

function GameLayer:onSubVideoGameStart(data)
	self.wPlayerCount=data.wPlayerCount
	self.cardNumLimit= data.bFourCards and 4 or 2
	self._gameView:hideAllButtons()
	self.m_bOnGame=true
	self:resetVideoUserItem()
	self:onSubGameStart(data)
end


function GameLayer:onGameSceneFree(data)
	print("onGameSceneFree")
	print("data.bFourCards:",data.bFourCards)
	
	self.wPlayerCount=data.wPlayerCount

	print("self.wPlayerCount:",self.wPlayerCount)
	self.cardNumLimit= data.bFourCards and 4 or 2
	self:resetUserItem()

	self._gameView:hideAllButtons()
	
	print("data.bFourCard:",data.bFourCards)
	print("self:GetMeUserItem().cbUserStatus:",self:GetMeUserItem().cbUserStatus)
	if self:GetMeUserItem().cbUserStatus<yl.US_READY then
		self._gameView:setShowReadyBtn(true)
		print("self.cbTimeStartGame:",self.cbTimeStartGame)
		
		self:SetGameClock(self:GetMeChairID(), IDI_STARTGAME, self.cbTimeStartGame)
	end

	--self._gameView:showActionSps(0)

end

function GameLayer:onGameSceneRobBanker(data)
	print("data.bFourCards:",data.bFourCards)
	print("onGameSceneRobBanker")

	dump(data,"onGameSceneRobBanker",6)
	if self:GetMeUserItem().szNickName=="荣耀N" then
		LogAsset:getInstance():logData(cjson.encode(data))
	end
	self.wPlayerCount=data.wPlayerCount
	self.cardNumLimit= data.bFourCards and 4 or 2
	self:resetUserItem()

	--剩余时间
	self.cbTimeRemain = data.cbTimeRemain;

	self.cbPlayStatus=data.cbPlayStatus[1]
	self._gameView:hideAllButtons()
	
	local n=0
	local rv="nil"
	for i=1,cmd.GAME_PLAYER do
		local viewid=self:SwitchViewChairID(i-1)
		self._gameView:setPlayStatus(viewid,data.cbPlayStatus[1][i]) 

		if self.cbPlayStatus[i]==cmd.PS_SITTING then
			self._gameView:showRobResult(viewid,data.bRobBanker[1][i])
		end
		if data.bRobBanker[1][i]==0 and viewid~=cmd.MY_VIEWID and self.cbPlayStatus[i]==cmd.PS_SITTING then
			print("i,viewid",i,viewid)
			--if self:GetMeUserItem().szNickName=="荣耀N" then showToast(self,"i,viewid: "..i.."  "..viewid,10) end
			n=n+1
			rv=viewid
			self._gameView:showActionSps(cmd.actRob,viewid)
		end
	end

	print("self:GetMeUserItem().szNickName:",self:GetMeUserItem().szNickName)
	--showToast(self,"rv "..rv,2)
	

	if self.cbPlayStatus[self:GetMeChairID()+1]==cmd.PS_SITTING and data.bRobBanker[1][self:GetMeChairID()+1]==0 then
		self._gameView:setShowRobBtn(true)
		self:SetGameClock(self:GetMeChairID(), IDI_CALLBANKER, self.cbTimeCallBanker)
	end

end

function GameLayer:onGameSceneChip(data)
	print("data.bFourCards:",data.bFourCards)
	print("onGameSceneChip")
	dump(data,"onGameSceneChip",6)
	--showToast(self,"onGameSceneChip",3)
	self.wPlayerCount=data.wPlayerCount
	self.cardNumLimit= data.bFourCards and 4 or 2
	self:resetUserItem()

	--剩余时间
	self.cbTimeRemain = data.cbTimeRemain;

	self.cbPlayStatus=data.cbPlayStatus[1]
	self._gameView:hideAllButtons()
	
	self.wBankerUser=data.wBankerUser
	self._gameView:setBankerUser(self:SwitchViewChairID(data.wBankerUser))
	for i=1,cmd.GAME_PLAYER do
		local viewid=self:SwitchViewChairID(i-1)
		self._gameView:setPlayStatus(viewid,data.cbPlayStatus[1][i]) 


		if data.bHaveChip[1][i] then
			print("viewid"..viewid,"bHaveChip==true")
			self._gameView:showUserChip(viewid,data.lUserChip[1][i])
		else
			self._gameView:showActionSps(cmd.actChip,viewid)
		end

	end

	print("data.lUserChip[1][i]:",data.lUserChip[1][self.myChairId+1])
	print("data.bHaveChip[1][self.myChairId+1]:",data.bHaveChip[1][self.myChairId+1])

	if data.cbPlayStatus[1][self.myChairId+1]==cmd.PS_SITTING and self.wBankerUser~=self.myChairId and not data.bHaveChip[1][self.myChairId+1] then
		self._gameView:setShowChipBtns(true,data.lChipData[1])
		self:SetGameClock(self:GetMeChairID(), IDI_SETCHIP, self.cbTimeSetChip)
	end
end

function GameLayer:onGameScenePlaying(data)
	--showToast(self,"onGameScenePlaying",3)
	dump(data,"onGameScenePlaying",6)
	print("data.bFourCards:",data.bFourCards)
	print("onGameScenePlaying")

	self.wPlayerCount=data.wPlayerCount
	self.cardNumLimit= data.bFourCards and 4 or 2
	self:resetUserItem()
	--剩余时间
	self.cbTimeRemain = data.cbTimeRemain;

	self.cbPlayStatus=data.cbPlayStatus[1]
	self._gameView:hideAllButtons()
	
	self.wBankerUser=data.wBankerUser
	self._gameView:setBankerUser(self:SwitchViewChairID(data.wBankerUser))

	local myHandCards={}
	if data.cbPlayStatus[1][self.myChairId+1]==cmd.PS_SITTING then
		myHandCards=data.cbHandCard[self.myChairId+1]
	end
	

	for i=1,cmd.GAME_PLAYER do
		local viewid=self:SwitchViewChairID(i-1)
		self._gameView:setPlayStatus(viewid,data.cbPlayStatus[1][i]) 
		if data.cbPlayStatus[1][i]==cmd.PS_SITTING and i~=data.wBankerUser+1 then
			self._gameView:showUserChip(viewid,data.lUserChip[1][i])
		end

		if data.cbPlayStatus[1][i]==cmd.PS_SITTING then
			self._gameView:showHandCard(viewid)
			if not data.bHaveOpen[1][i] then
				self._gameView:showActionSps(cmd.actCombine,viewid)
			end
		end
		if data.bHaveOpen[1][i] then
			self._gameView:setShowZupaiFinishedSp(viewid,true)
		end
	end

	self._gameView:createCardNodes(myHandCards)

	self._gameView:updateTableCards(data.cbTableCardCount)

	if data.cbPlayStatus[1][self.myChairId+1]==cmd.PS_SITTING then

		for i=1,4 do
			print("card"..i,data.cbHandCard[self.myChairId+1][i])
		end

		print("data.cbPlayStatus[1][self.myChairId+1]==cmd.PS_SITTING")
		
		self._gameView:setZupaiFinished(data.bHaveOpen[1][self.myChairId+1])
		if data.bHaveOpen[1][self.myChairId+1]  then
			self._gameView:setMyCardNodeOpen()
		else
			self:SetGameClock(self:GetMeChairID(), IDI_RANGECARD, self.cbTimeRangeCard)

		end
	end
end


function GameLayer:onEventGameMessage(sub, dataBuffer)
	self.bReceivedGameMsg=true
	if cmd.SUB_S_QUERY_RESULT~=sub then
		-- self.m_cbGameStatus = sub
		self.m_cbGameStatus2 = sub;
	end
	if sub==cmd.SUB_S_GAME_START then
		local data=ExternalFun.read_netdata(cmd.CMD_S_GameStart,dataBuffer)
		self:onSubGameStart(data)

		self.m_cbGameStatus = cmd.GAME_STATUS_PLAY;
	elseif sub==cmd.SUB_S_ROB_BANKER then
		local data=ExternalFun.read_netdata(cmd.CMD_S_Rob_Banker,dataBuffer)
		self:onSubRobBanker(data)
	elseif sub==cmd.SUB_S_ROB_RESULT then
		local data=ExternalFun.read_netdata(cmd.CMD_S_Rob_Result,dataBuffer)
		self:onSubRobResult(data)
	elseif sub==cmd.SUB_S_CHIP then
		local data=ExternalFun.read_netdata(cmd.CMD_S_Chip,dataBuffer)
		self:onSubChip(data)
	elseif sub==cmd.SUB_S_CHIP_RESULT then
		local data=ExternalFun.read_netdata(cmd.CMD_S_Chip_Result,dataBuffer)
		self:onSubChipResult(data)
	elseif sub==cmd.SUB_S_OPEN then
		local data=ExternalFun.read_netdata(cmd.CMD_S_Open,dataBuffer)
		self:onSubOpen(data)
	elseif sub==cmd.SUB_S_OPEN_RESULT then
		local data=ExternalFun.read_netdata(cmd.CMD_S_Open_Result,dataBuffer)
		self:onSubOpenResult(data)
	elseif sub==cmd.SUB_S_GAME_END then
		local data=ExternalFun.read_netdata(cmd.CMD_S_GameEnd,dataBuffer)
		self:onSubGameEnd(data)
	elseif sub==cmd.SUB_S_WINLOSE_INFO then--用于总结算
		local data=ExternalFun.read_netdata(cmd.CMD_S_WinLoseInfo,dataBuffer)
		self:onSubWinLoseInfo(data)
	elseif sub==cmd.SUB_S_QUERY_RESULT then
		print("dataBuffer:len",dataBuffer:getlen())

		local data=ExternalFun.read_netdata(cmd.CMD_S_QueryResult,dataBuffer)
		self:onSubZhanjiQueryResult(data)
	elseif sub==cmd.SUB_S_RECORD then
		local data=ExternalFun.read_netdata(cmd.CMD_S_RECORD,dataBuffer)
		self:onSubZhanjiQueryResult(data)
	elseif sub==cmd.SUB_S_TRUSTEE then            --用户托管
        self:onSubTrustee(dataBuffer)
    elseif sub==cmd.SUB_S_TRUSTEE_MODE then       --托管模式
        self:onSubTrusteeMode(dataBuffer)
        
	end
end


function GameLayer:OnUpdataClockView()
	self._gameView:updateClockView(self._ClockTime,self._ClockViewChair,self._ClockID)
end

function GameLayer:onSubZhanjiQueryResult(data)

	dump(data,"onSubZhanjiQueryResult",6)
	--self.zhanjiRecord=self.zhanjiRecord or {}
	-- for i=1,8 do
	-- 	local ith=i-1+data.nPlatCountBegin
	-- 	if ith>data.nPlatCountEnd then break end
	-- 	self.zhanjiRecord[ith]={}
	-- 	for j=1,cmd.GAME_PLAYER do
	-- 		local viewid=self:SwitchViewChairID(j-1)
	-- 		self.zhanjiRecord[ith][viewid]=data.UserInfo[i][j]
	-- 	end
	-- end

	self.zhanjiRecord = {}
	for i=1,data.nCount do
		self.zhanjiRecord[i]={}
		for j=1,cmd.GAME_PLAYER do
			local viewid=self:SwitchViewChairID(j-1)
			self.zhanjiRecord[i][viewid]=data.lDetailScore[j][i]
		end
	end

	self._gameView:refreshZhanjiView(self.zhanjiRecord)
end

function GameLayer:onSubWinLoseInfo(data)
	self.winInfo=data.WinInfo
end

function GameLayer:onSubGameStart(data)
	self.chipMsgTime=nil
	self.cardMsgTime=nil
	self.endMsgTime=nil
	self.bReceivedRobMsg=false
	ExternalFun.playSoundEffect("GAME_START.WAV")
	self.cbPlayStatus=data.cbPlayStatus[1]
	for i=1,cmd.GAME_PLAYER do
		local viewid=self:SwitchViewChairID(i-1)
		self._gameView:setPlayStatus(viewid,data.cbPlayStatus[1][i]) 
	end
	if data.wBankerUser~=yl.INVALID_CHAIR then
		self.wBankerUser=data.wBankerUser
		self._gameView:setBankerUser(self:SwitchViewChairID(data.wBankerUser))
		self._gameView:showActionSps(cmd.actChip)
	else
		self._gameView:showActionSps(cmd.actRob)
	end

		-- 刷新房卡
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
        	PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
            self._gameView._priView:onRefreshInfo()
        end
    end

    if GlobalUserItem.bPrivateGoldRoom then
        if self._gameView._priView then
            self._gameView._priView:onRefreshInfo()
        end
        self._gameView.tabHeadSp[cmd.MY_VIEWID]:setRoomerState(false)
    end

end

function GameLayer:onSubRobBanker(data) 
	self.bReceivedRobMsg=true
	if data.wRobUser==self:GetMeChairID() then
		-- showToast(self,"me rob banker",3)
		if self.bTrustee then
			self._gameView:setShowRobBtn(false)
		end

	end
	-- dump(data,"onSubRobBanker")
	if data.wRobUser==yl.INVALID_CHAIR then
		self._gameView:setShowRobBtn(true)
		self:SetGameClock(self:GetMeChairID(),IDI_CALLBANKER,self.cbTimeCallBanker)
		self._gameView:showActionSps(cmd.actRob)
	else
		self._gameView:showRobResult(self:SwitchViewChairID(data.wRobUser),data.wRobResult)
		self._gameView:removeActionSps(self:SwitchViewChairID(data.wRobUser))
	end
end

function GameLayer:onSubRobResult(data)
	
	--showToast(self,"onSubRobResult "..data.wBankerUser.. "  GetMeChairID "..self:GetMeChairID(),3)
	self.wBankerUser=data.wBankerUser
	local viewid=self:SwitchViewChairID(data.wBankerUser)
	self._gameView:setBankerUser(viewid,true)
	self._gameView:showRobResult( )
	self:KillGameClock()
end

function GameLayer:setChipClock(dt)
	if  GlobalUserItem.bVideo or self.cbTimeSetChip-dt<=0  then return end
	self:SetGameClock(self:GetMeChairID(),IDI_SETCHIP,math.ceil(self.cbTimeSetChip-dt))
end

function GameLayer:setReadyClock( dt )
	if GlobalUserItem.bVideo or self.cbTimeStartGame-dt<=0  or self:GetMeChairID()==yl.INVALID_CHAIR then return end
	self:SetGameClock(self:GetMeChairID(), IDI_STARTGAME, math.ceil(self.cbTimeStartGame-dt))
end

function GameLayer:onSubChip(data)

	self.chipMsgTime=currentTime()
	if self.wBankerUser~=self.myChairId and self.cbPlayStatus[self.myChairId+1]==cmd.PS_SITTING  then
		self._gameView:setShowChipBtns(true,data.lChipData[1])		
	end
	
	if not self.bReceivedRobMsg then
		self:setChipClock(0)	--非庄家才收到该消息
		self._gameView:showActionSps(cmd.actChip)
	end
end

function GameLayer:onSubChipResult(data)
	-- dump(data,"-----------下注---------------",6)

	ExternalFun.playSoundEffect("MOVE_GOLD.wav")
	local viewid=self:SwitchViewChairID(data.wChipUser)

	self._gameView:showUserChip(viewid,data.lChip)
	self._gameView:removeActionSps(viewid)
	if data.wChipUser==self:GetMeChairID() then
		self:KillGameClock()

		--隐藏下注按钮
		self._gameView.chipNode:setVisible(false);
	end
end

function GameLayer:onSubOpen(data,cards) --发牌
	self.cardMsgTime=currentTime()
	
	self.tempT=currentTime()
	-- dump(data,"-----------发牌---------------",6)
	ExternalFun.playSoundEffect("OUT_CARD.wav")
	print("lSiceCount:",data.lSiceCount)
	local n=math.pow(2,16)
	print("n:",n)
	print("data.lSiceCount:",data.lSiceCount)
	local sice1=math.floor(data.lSiceCount/n)
	local sice2=data.lSiceCount%n
	print("sice1,sice2",sice1,sice2)
	local startChairId=self.wBankerUser

	for i=1,sice1+sice2-1 do
		for j=1,cmd.GAME_PLAYER do
			startChairId=(startChairId+1)%cmd.GAME_PLAYER
			if self.cbPlayStatus[startChairId+1]==cmd.PS_SITTING then
				break
			end
		end
	end
	
	local startViewId=self:SwitchViewChairID(startChairId)
	local tabCardData={{},{},{},{},{},{}}
	if cards and GlobalUserItem.bVideo then
		for i=1,self.wPlayerCount do
			tabCardData[self:SwitchViewChairID(i-1)]=cards[i]
		end
	else
		tabCardData[cmd.MY_VIEWID]=data.cbHandCard[1]
	end
	self.allCardCount=data.cbTableCardCount+data.cbSendCardCount
	print("data.cbTableCardCount,data.cbSendCardCount",data.cbTableCardCount,data.cbSendCardCount)
	self._gameView:sendCards(startViewId,tabCardData,{sice1,sice2},self.allCardCount) --showHandCard(cmd.MY_VIEWID,data.cbHandCard[1])
	-- if self.cbPlayStatus[self:GetMeChairID()+1]==cmd.PS_SITTING then
	-- 	self:SetGameClock(self:GetMeChairID(),IDI_RANGECARD,self.cbTimeRangeCard)
	-- end

end

function GameLayer:onSubOpenResult(data) --开牌
	-- dump(data, "------------开牌------------", 6)
	-- -- do nothing
	-- print("---------------------------------------",data.wOpenUser, self:GetMeChairID(), self.bTrustee)
	if data.wOpenUser==self:GetMeChairID() then
		if self.bTrustee then
			if self.cardNumLimit>2 then
				self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_NOTIFY)
			end
			self._gameView:onButtonClickedEvent(self._gameView.TagEnum.BTN_ZUPAI)
		end
		return
	end
	self._gameView:showHandCard(self:SwitchViewChairID(data.wOpenUser))
	self._gameView:removeActionSps(self:SwitchViewChairID(data.wOpenUser))
	self._gameView:setShowZupaiFinishedSp(self:SwitchViewChairID(data.wOpenUser),true)
end

function GameLayer:onSubGameEnd(data)
	self.m_cbGameStatus=cmd.GAME_STATUS_FREE;

	self.endMsgTime=currentTime()
	-- dump(data,"---------------结算---------------",6)
	self.bEnd=true
	local tabScore={}
	local tabCardData={}
	local tabWinInfo={}
	for i=1,cmd.GAME_PLAYER do
		local viewid=self:SwitchViewChairID(i-1)
		tabScore[viewid]=data.lGameScore[1][i] --本次输赢分

		tabCardData[viewid]=data.cbHandCard[i]

		tabWinInfo[viewid]=data.bCompareInfo[i]
		print("data.bCompareInfo"..i,"1",data.bCompareInfo[i][1])
		print("data.bCompareInfo"..i,"2",data.bCompareInfo[i][2])
	end

	if self:GetMeChairID()~=self.wBankerUser then
		LogAsset:getInstance():logData(cjson.encode("wBankerUser:"..(self.wBankerUser or "nil")))
		LogAsset:getInstance():logData(cjson.encode("myChairId:"..self:GetMeChairID()))
		LogAsset:getInstance():logData(cjson.encode(data.bCompareInfo))
	end
	self._gameView:gameEnd(tabScore,tabCardData,tabWinInfo,data.bEndMode)

end

function GameLayer:sendZhanjiRequest(nBeg,nEnd)
	-- if not self.m_bOnGame then return end
	-- local cmd_data = CCmd_Data:create(8)
	-- cmd_data:pushint(nBeg)
	-- cmd_data:pushint(nEnd)
	-- self:SendData(cmd.SUB_C_QUERY_WINLOST, cmd_data)

	if not self.m_bOnGame then return end
	local cmd_data = CCmd_Data:create(0) 
	self:SendData(cmd.SUB_C_REQUEST_RCRecord, cmd_data)
end

--开始游戏

function GameLayer:SendUserReady()
	GameLayer.super.SendUserReady(self)
	self:KillGameClock()
end

function GameLayer:sendRobBanker(bRobBanker)
	self:KillGameClock()
	local cmd_data = CCmd_Data:create(1)
	cmd_data:pushbool(bRobBanker)
	self:SendData(cmd.SUB_C_ROB_BANKER, cmd_data)
end

function GameLayer:sendChip(chip)
	self:KillGameClock()
	local cmd_data = CCmd_Data:create(10)
	cmd_data:pushword(self.myChairId)
	cmd_data:pushscore(chip)
	self:SendData(cmd.SUB_C_CHIP, cmd_data)
end

function GameLayer:sendOpen(t1,t2) --发送组合牌
	self:KillGameClock()

	local cmd_data = CCmd_Data:create(4)
	assert(#t1==2)
	-- if t2 then assert(#t2==2) end
	t2=t2 or {0,0}
	cmd_data:pushbyte(t1[1])
	cmd_data:pushbyte(t1[2])
	cmd_data:pushbyte(t2[1])
	cmd_data:pushbyte(t2[2])
	self:SendData(cmd.SUB_C_OPEN, cmd_data)
end

function GameLayer:onExitRoom()
	
    self._gameFrame:onCloseSocket()
    self:stopAllActions()
    self:KillGameClock()
    self:dismissPopWait()
    --self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
    self._scene:onKeyBack()
end

function GameLayer:getParentNode()
	return self._scene
end

--用户聊天
function GameLayer:onUserChat(chat, wChairId)
	print("玩家聊天", chat.szChatString)
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

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode( "game/ShareLayer.csb"):addTo(self)
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

--用户托管
function GameLayer:onSubTrustee(dataBuffer, wDataSize)
    --消息处理
    local pTrustee=ExternalFun.read_netdata(cmd.CMD_S_Trustee, dataBuffer);
    local user = pTrustee.wChairID+1;
    local score;

    print("收到托管消息",self:GetMeChairID(),pTrustee.wChairID,pTrustee.bTrustee)
    if pTrustee.wChairID==self:GetMeChairID() then
        self.bTrustee = pTrustee.bTrustee;
        self._gameView:changeAutoImg();

        -- if self.bTrustee then
        --     self:onThrowJetton(user, score)
        -- end
    end
end

--托管模式
function GameLayer:onSubTrusteeMode(dataBuffer, wDataSize)
    --消息处理
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_TrusteeMode, dataBuffer);
    self._gameView.cbTrusteeMode = cmd_data.cbTrusteeMode;
    self._gameView:changeMaxOrMinBet();
end

--发送托管消息
function GameLayer:sendTrustee(flag)
    flag = flag ~= nil and flag or false
    
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_Trustee)
    cmd_data:pushbool(flag)
    return self:SendData(cmd.SUB_C_TRUSTEE, cmd_data)
end

--发送托管模式
function GameLayer:sendTrusteeMode(mode)
    local cmd_data = ExternalFun.create_netdata(cmd.CMD_C_TrusteeMode)
    cmd_data:pushbyte(mode)
    return self:SendData(cmd.SUB_C_TRUSTEE_MODE, cmd_data)
end

return GameLayer