local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local module_pre = "game.yule.animalbattle.src"
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameViewLayer = appdf.req(module_pre .. ".views.layer.GameViewLayer")
local GameServer_CMD = appdf.req(appdf.HEADER_SRC.."CMD_GameServer")
local GameFrame = appdf.req(appdf.GAME_SRC.."yule.animalbattle.src.models.GameFrame")
local SpineObj = appdf.req("client.src.external.SpineObj")

--cc.FileUtils:getInstance():addSearchPath(device.writablePath.."game/yule/animalbattle/res")


--时间标识
local IDI_FREE			=		99									--空闲时间
local IDI_PLACE_JETTON	=		100									--下注时间
local IDI_DISPATCH_CARD	=		301									--发牌时间
local IDI_OPEN_CARD		=		302								    --发牌时间
local IDI_ANDROID_BET	=		1000	

--assignArray(destTable)清空array
--assignArray(destTable,srcTable)将srcTable的内容复制到destTable
--assignArray(destTable,n,value) 使destTable[i]=value,i=1,2...n
local function assignArray(destTable,...) --只改变table的array部分
	local var={...}
	if var[1]==nil then
		for i=1,#destTable do
			destTable[i]=nil
		end
	elseif type(var[1])=="table" then
		for i=1,#var[1] do
			destTable[i]=var[1][i]
		end
		destTable[#var[1]+1]=nil
	elseif type(var[1])=="number" then
		for i=1,var[1] do
			destTable[i]=var[2]
		end
		destTable[var[1]+1]=nil
	end
end

--onreset

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

--创建网络消息包
local function create_netdata( keyTable ) 
	local len=getLenthOfNetData(keyTable)
	return CCmd_Data:create(len);
end

-- create_netdata(CMD_C_ContinueJetton)
-- assert(false)

function GameLayer:getGameKind()
    return cmd.KIND_ID
end

function GameLayer:ctor( frameEngine,scene )  
	local cmddata = create_netdata(cmd.CMD_C_ContinueJetton)
	GameLayer.super.ctor(self, frameEngine, scene)

	self:initData()
	self._roomRule = self._gameFrame._dwServerRule
	self.originalFPS=cc.Director:getInstance():getAnimationInterval()
	cc.Director:getInstance():setAnimationInterval(1/30)
end

function GameLayer:initData()
	self.tabMyBets={}
	self.tabTotalBets={}
	self.m_nCumulativeScore=0 --总得分
	self.m_nAllPlayBet=0  --此次下注总和
	self.m_nCurrentNote=nil
	self.m_nApplyBanker = {}
	self.m_myIsBet = false
	self.m_otherIsBet = false
	self.m_lastTimeBet = false
	self.isOnLastPlaceJetton = false
	--self.bAllowOpeningAni=true
end

function GameLayer:CreateView()
	self._dataModle = GameFrame:create()
	ExternalFun.playBackgroudAudio("GAME_FREE.mp3")
	local this=self
    return GameViewLayer:create(this)
        :addTo(this)


end

function GameLayer:clearBets()
	self.m_nAllPlayBet=0
	for i=1,cmd.AREA_COUNT do
		self.tabMyBets[i]=0
		self.tabTotalBets[i]=0
	end
	self._gameView:updateMyBets(self.tabMyBets)    
	self._gameView:updateTotalBets(self.tabTotalBets)
end

function GameLayer:SetGameStatus(status)
	self.m_gameStatu=status
	-- self.m_gameStatu =status
end

--退出桌子
function GameLayer:onExitTable()
	-- local MeItem = self:GetMeUserItem()
	-- if MeItem and MeItem.cbUserStatus > yl.US_FREE then
	-- 	self:showPopWait()
	-- 	self:runAction(cc.Sequence:create(
	-- 		cc.CallFunc:create(
	-- 			function ()
	-- 				self._gameFrame:StandUp(1)
	-- 			end
	-- 			),
	-- 		cc.DelayTime:create(3),
	-- 		cc.CallFunc:create(
	-- 			function ()
	-- 				self:onExitRoom()
	-- 			end
	-- 			)
	-- 		)
	-- 	)
	-- 	return
	-- end
	self:onExitRoom()
end

--离开房间
function GameLayer:onExitRoom()
	-- removebackgroundcallback() 
	-- if type(self._scene.onBackgroundCallBack)=="function" then
	-- 	setbackgroundcallback(self._scene.onBackgroundCallBack)
	-- end

	self._gameFrame:onCloseSocket()
	self:KillGameClock()
	print("self.originalFPS: ",self.originalFPS)
	cc.Director:getInstance():setAnimationInterval(self.originalFPS)
	self._scene:onKeyBack()
end

local ithMsg=0
function GameLayer:onEventGameMessage(sub,dataBuffer)
	ithMsg=ithMsg+1
	print("ithMsg: ",ithMsg)
	print("onEventGameMessage sub == " , sub)
	if sub== cmd.SUB_S_GAME_FREE then	--空闲时间
		return self:OnSubGameFree(dataBuffer)
	elseif sub==cmd.SUB_S_GAME_START then	--游戏开始
		return self:OnSubGameStart(dataBuffer)
	elseif sub==cmd.SUB_S_PLACE_JETTON then	--游戏结束
		return self:OnSubPlaceJetton(dataBuffer)
	elseif sub==cmd.SUB_S_GAME_END then	--玩家下注
		return self:OnSubGameEnd(dataBuffer)
	elseif sub==cmd.SUB_S_PLACE_JETTON_FAIL then--下注失败
		return self:OnSubPlaceJettonFail(dataBuffer)
	elseif sub==cmd.SUB_S_CEAN_JETTON then	--清除下注
		return self:OnSubClearJetton(dataBuffer)
	elseif sub==cmd.SUB_S_CONTINU_JETTON then	--更新下注
		return self:OnSubContinueJetton(dataBuffer)
	elseif sub==cmd.SUB_S_SEND_RECORD	then  --游戏记录
		return self:OnSubGameRecord(dataBuffer)
	elseif sub==cmd.SUB_S_CHANGE_BANKER then --切换庄家
		return self:OnSubGhangeBanker(dataBuffer)
	elseif sub==cmd.SUB_S_APPLY_BANKER then --申请庄家
		return self:OnSubApplyBanker(dataBuffer)
	elseif sub==cmd.SUB_S_CANCEL_BANKER then 
		return self:OnSubCancelBanker(dataBuffer)
	elseif sub==cmd.SUB_S_SYSTIP then 
		local p = ExternalFun.read_netdata({{t="string",k="tips",s=512}}, dataBuffer)
		return showToast(self,p.tips,1)
	end
end

--系统消息
function GameLayer:onSystemMessage( wType,szString )
    return showToast(self,szString,1)
end

function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)
	
	if cbGameStatus==cmd.GAME_STATUS_FREE then
		local pStatusFree = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
		-- dump(pStatusFree,"pStatusFree",12)
		
		--设置状态
		self:SetGameStatus(cmd.GAME_STATUS_FREE)
		self._gameView:SetGameStatus(cmd.GAME_STATUS_FREE)
		self._gameView:updateTimeTextImg()
		self._gameView:updateAsset(self:GetMeUserItem().lScore)
		self._gameView:updateCurrentScore(0)
		self._gameView:enable_NoteNum_Clear_ContinueBtn(false)

		self:SetGameClock(self:GetMeChairID(),IDI_FREE,pStatusFree.cbTimeLeave)
				
	
		--玩家信息
		self.m_lMeMaxScore=pStatusFree.lUserMaxScore
		self.m_lAreaLimitScore=pStatusFree.lAreaLimitScore
		self.m_lChipArray = pStatusFree.lChipArray
		self._gameView:updateChipArray(self.m_lChipArray)
		self.m_nCurrentNote = self.m_lChipArray[1][1]
		self._gameView:updateCountDown(cbGameStatus,pStatusFree.cbTimeLeave)

		local bankerTab = {}
		for k,v in pairs(self:getUserList()) do
			if v.wChairID == pStatusFree.wBankerUser then 
				bankerTab = v
			end
		end

		if  pStatusFree.wBankerUser == 65535 then
			bankerTab.szNickName = "系统坐庄"
			bankerTab.lScore = 100000000
		end
		self.m_bankerTab = bankerTab
		self.m_wBankerUser = pStatusFree.wBankerUser

		self.lApplyBankerCondition = pStatusFree.lApplyBankerCondition
		self.bEnableSysBanker = pStatusFree.bEnableSysBanker
		self._gameView:updateApplyBanker(bankerTab,pStatusFree.wBankerUser)
		self._gameView:updateGameBroken(cmd.GAME_STATUS_FREE)
	elseif cbGameStatus==cmd.GS_PLACE_JETTON or cbGameStatus==cmd.GS_GAME_END then
		local pStatusPlay = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer)
		-- dump(pStatusPlay,"pStatusPlay",12)
		self.m_lChipArray = pStatusPlay.lChipArray
		self:SetGameStatus(cbGameStatus)
		if cbGameStatus==cmd.GS_PLACE_JETTON then
			self._gameView:SetGameStatus(cmd.GS_PLACE_JETTON)
			self._gameView:enableAllBtns(true)
			self._gameView:updateCurrentScore(0)
			-- self._gameView:disableNoteNumBtns(1)
			self._gameView:updateGameBroken(cmd.GS_PLACE_JETTON)

			local tabBets = {}
			for i=1,cmd.AREA_COUNT-1 do
				tabBets[i]=pStatusPlay.lAllJettonScore[1][i+1]
			end 
			self._gameView:updateGamePlayerBets(tabBets,false)
			self._gameView:updateTotalBets(tabBets)
			
		elseif cbGameStatus==cmd.GS_GAME_END then
			self._gameView:SetGameStatus(cmd.GS_GAME_END)
			self._gameView:enableAllBtns(false)
			self._gameView:updateGameBroken(cmd.GS_GAME_END)
		end

		local nTimerID = (pStatusPlay.cbGameStatus==cmd.GS_GAME_END) and IDI_OPEN_CARD or IDI_PLACE_JETTON
		self:SetGameClock(self:GetMeChairID(), nTimerID, pStatusPlay.cbTimeLeave)
		self._gameView:updateAsset(self:GetMeUserItem().lScore-self.m_nAllPlayBet) 
		self._gameView:updateTimeTextImg()

		--玩家积分
		self.m_lMeMaxScore=pStatusPlay.lUserMaxScore
		self.m_lAreaLimitScore=pStatusPlay.lAreaLimitScore

		
		self._gameView:updateChipArray(self.m_lChipArray)
		self.m_nCurrentNote = self.m_lChipArray[1][1]

		self._gameView:updateCountDown(cbGameStatus,pStatusPlay.cbTimeLeave)
		

		local bankerTab = {}
		for k,v in pairs(self:getUserList()) do
			if v.wChairID == pStatusPlay.wBankerUser then 
				bankerTab = v
			end
		end

		if  pStatusPlay.wBankerUser == 65535 then
			bankerTab.szNickName = "系统坐庄"
			bankerTab.lScore = 100000000
		end
		self.m_bankerTab = bankerTab
		self.m_wBankerUser = pStatusPlay.wBankerUser
		
		self.lApplyBankerCondition = pStatusPlay.lApplyBankerCondition
		self.bEnableSysBanker = pStatusPlay.bEnableSysBanker
		self._gameView:updateApplyBanker(bankerTab,pStatusPlay.wBankerUser)
	end

	self:dismissPopWait()
end

function GameLayer:OnSubGameFree(dataBuffer)

	self.m_myIsBet = false
	self.m_otherIsBet = false
	local pGameFree = ExternalFun.read_netdata(cmd.CMD_S_GameFree, dataBuffer)

	--设置时间
	self:SetGameClock(self:GetMeChairID(),IDI_FREE,pGameFree.cbTimeLeave)
	self.m_nAllPlayBet=0

	self:SetGameStatus(cmd.GAME_STATUS_FREE)
	self._gameView:SetGameStatus(cmd.GAME_STATUS_FREE)
	self._gameView:updateTimeTextImg()
	self._gameView:enableBetBtns(true)
	self._gameView:enable_NoteNum_Clear_ContinueBtn(false)

	self._gameView:updateAsset(self:GetMeUserItem().lScore)
	
	-- self._gameView:updateStorage(pGameFree.lBonus) --彩池数字

	assignArray(self.tabMyBets,cmd.AREA_COUNT-1,0)     --清空个人下注
	assignArray(self.tabTotalBets,cmd.AREA_COUNT-1,0)   --清空总下注
	self._gameView:updateMyBets(self.tabMyBets)    
	self._gameView:updateTotalBets(self.tabTotalBets)
	self._gameView:updateCurrentScore(0)
	self._gameView:updateCountDown(cmd.GAME_STATUS_FREE,pGameFree.cbTimeLeave)
	self._gameView:removeChip()
	self._gameView:removeGameWait()
	self._gameView:initTotalScore()
	self._gameView:updateBankerMoney()
	self.m_cbGameStatus = cmd.GAME_STATUS_FREE
end

function GameLayer:OnSubGameStart(dataBuffer)
	self.m_myIsBet = false
	self.m_otherIsBet = false
	local pGameStart = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
	--设置状态
	self:SetGameStatus(cmd.GS_PLACE_JETTON)
	self._gameView:SetGameStatus(cmd.GS_PLACE_JETTON)
	self._gameView:updateTimeTextImg()

	self._gameView:enableAllBtns(true) 
	local lUserScore = self:GetMeUserItem().lScore
	local startIndex=nil
	for i=1,5 do
		-- local noteNum=math.pow(10,i+1)
		local noteNum=self.m_lChipArray[1][i]
		if (lUserScore-self.m_nAllPlayBet)/noteNum<1 then
			startIndex=i
			break
		end
	end

	if nil~=startIndex then
		self._gameView:disableNoteNumBtns(startIndex)
	end

	self._gameView:updateAsset(lUserScore)
	-- self._gameView:updateStorage( pGameStart.lBonus ) --更新彩池数字
	self._gameView:updateCurrentScore(0)
	--玩家信息
	self.m_lMeMaxScore=pGameStart.lUserMaxScore;  

	--设置时间
	self:SetGameClock(self:GetMeChairID(),IDI_PLACE_JETTON,pGameStart.cbTimeLeave);
	self._gameView:updateCountDown(cmd.GS_PLACE_JETTON,pGameStart.cbTimeLeave)
	self._gameView:removeChip()
	self._gameView:removeGameWait()
	self._gameView:initTotalScore()
	self._gameView:updateBankerMoney()
	local spine = SpineObj:create("skeleton/jetton/skeleton.json", "skeleton/jetton/skeleton.atlas", false)
	spine:setAnimation(0, "animation", false)
	spine:registerSpineEventHandler(function (event) spine:destroy() end, 2)
	spine:setPosition(667,375)
	self:addChild(spine)

	self._gameView.continueBtn:setEnabled(self.m_lastTimeBet)


	ExternalFun.playSoundEffect("kaishixiazhu.mp3")
end

function GameLayer:OnSubGameEnd(dataBuffer)
	local pGameEnd = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer)
	dump(pGameEnd,"pGameEnd",12)
	self:SetGameStatus(cmd.GS_GAME_END)
	self._gameView:SetGameStatus(cmd.GS_GAME_END)
	self._gameView:updateTimeTextImg()
	self._gameView:removeFirstOpeningAni()

	if self.m_nAllPlayBet > 0 then 
		self.m_lastTimeBet = true
	else
		self.m_lastTimeBet = false
	end

	self.m_nAllPlayBet=0
	self._gameView:enableAllBtns(false) 
	self.isOnLastPlaceJetton = false
	self.m_GameEndTime = pGameEnd.cbTimeLeave;

	--设置时间
	self:SetGameClock(self:GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd.cbTimeLeave)
	self.m_nCumulativeScore = self.m_nCumulativeScore+pGameEnd.lUserScore

	self._gameView:GameOver(pGameEnd.cbTableCardArray[1], pGameEnd.lUserScore, self.m_nCumulativeScore,pGameEnd.cbShaYuAddMulti,pGameEnd.lOtherUserScore)
	self._gameView:updateCountDown(cmd.GS_GAME_END,pGameEnd.cbTimeLeave)
	self.m_cbGameStatus = cmd.GAME_STATUS_FREE

	ExternalFun.playSoundEffect("tingzhixiazhu.mp3")
end

function GameLayer:getPlayerList(  )
    return self._gameFrame._UserList
end


function GameLayer:OnSubPlaceJetton(dataBuffer)  --done
	if self.m_gameStatu~=cmd.GS_PLACE_JETTON then
		return
	end

	local pPlaceJetton = ExternalFun.read_netdata(cmd.CMD_S_PlaceJetton, dataBuffer)
	local index=pPlaceJetton.cbJettonArea 
	dbg_assert(pPlaceJetton.lJettonScore)

	self.tabTotalBets[index]=self.tabTotalBets[index] or 0
	self.tabTotalBets[index]=self.tabTotalBets[index]+pPlaceJetton.lJettonScore
	self._gameView:updateTotalBet(index,self.tabTotalBets)
	if pPlaceJetton.wChairID==self:GetMeChairID() then
		self.tabMyBets[index]=self.tabMyBets[index] or 0
		self.tabMyBets[index]=self.tabMyBets[index]+pPlaceJetton.lJettonScore
		self._gameView:updateMyBets(self.tabMyBets)
		self._gameView:updateGamePlayerBet(index,pPlaceJetton.lJettonScore,true)
		self.m_nAllPlayBet=self.m_nAllPlayBet+self.m_nCurrentNote
		self._gameView:updateAsset(self:GetMeUserItem().lScore-self.m_nAllPlayBet)
		self.m_cbGameStatus = cmd.GS_PLACE_JETTON
		local startIndex=nil
		for i=1,5 do
			local noteNum=self.m_lChipArray[1][i]
			if (self:GetMeUserItem().lScore-self.m_nAllPlayBet)/noteNum<1 then
				startIndex=i
				break
			end
		end
		if not self.isOnLastPlaceJetton then
			self._gameView.continueBtn:setEnabled(true)
		end

		if nil~=startIndex then
			self._gameView:disableNoteNumBtns(startIndex)
		end
	else
		self._gameView:updateGamePlayerBet(index,pPlaceJetton.lJettonScore,false)
	end
end

--下注失败
function GameLayer:OnSubPlaceJettonFail(dataBuffer) --done
	if self.m_gameStatu~=cmd.GS_PLACE_JETTON then
		return
	end
	
	local pPlaceJettonFail = ExternalFun.read_netdata(cmd.CMD_S_PlaceJettonFail, dataBuffer)
end

function GameLayer:OnSubContinueJetton(dataBuffer) --done
	--showToast(self._gameView,"收到续投消息",3)
	if self.m_gameStatu~=cmd.GS_PLACE_JETTON then
		--showToast(self._gameView,"非下注时间，不处理",1)
		return
	end

	local  pLastJetton= ExternalFun.read_netdata(cmd.CMD_S_ContiueJetton, dataBuffer)
	local tabTotalBets = clone(self.tabTotalBets)
	for i=1,cmd.AREA_COUNT-1 do
		self.tabTotalBets[i]=pLastJetton.lAllJettonScore[1][i+1]
	end
	self._gameView:updateTotalBets(self.tabTotalBets)

	if self:GetMeChairID() == pLastJetton.wChairID then
		--showToast(self._gameView,"收到自己续投消息",1)
		local temptest=0
		for i=1,cmd.AREA_COUNT-1 do
			self.tabMyBets[i]=pLastJetton.lUserJettonScore[1][i+1]
			temptest=temptest+self.tabMyBets[i]
			self._gameView:updateGamePlayerBet(i,self.tabMyBets[i]-tabTotalBets[i],true)
		end 
		self._gameView:updateMyBets(self.tabMyBets)
		-- showToast(self._gameView,"收到自己续投消息,上盘总下注为"..temptest,1)
		self.m_nAllPlayBet=0
		for i=1,cmd.AREA_COUNT-1 do
			self.m_nAllPlayBet=self.m_nAllPlayBet+self.tabMyBets[i]
		end
		self._gameView:updateAsset(self:GetMeUserItem().lScore-self.m_nAllPlayBet)

		local startIndex=nil
		for i=1,5 do
			-- local noteNum=math.pow(10,i+1)
			local noteNum=self.m_lChipArray[1][i]
			if (self:GetMeUserItem().lScore-self.m_nAllPlayBet)/noteNum<1 then
				startIndex=i
				break
			end
		end

		if nil~=startIndex then
			self._gameView:disableNoteNumBtns(startIndex)
		end
	else
		local tabBets = 0
		for i=1,cmd.AREA_COUNT-1 do
			tabBets=pLastJetton.lUserJettonScore[1][i+1]
			self._gameView:updateGamePlayerBet(i,tabBets-tabTotalBets[i],false)	
		end 
		-- showToast(self._gameView,"收到自己续投消息,上盘总下注为"..temptest,1)
		
	end

end


function GameLayer:OnSubClearJetton(dataBuffer) --清除下注 --done
	if self.m_gameStatu~=cmd.GS_PLACE_JETTON then
		return
	end

	local  pCleanJetton=ExternalFun.read_netdata(cmd.CMD_S_CeanJetton, dataBuffer)
	for i=1,cmd.AREA_COUNT-1 do
		self.tabTotalBets[i]=pCleanJetton.lAllCPlaceScore[1][i+1]
	end

	self._gameView:updateTotalBets(self.tabTotalBets)

	if self:GetMeChairID()==pCleanJetton.wChairID then
		for i=1,cmd.AREA_COUNT-1 do
			self.m_nAllPlayBet=self.m_nAllPlayBet-(self.tabMyBets[i] or 0)
		end
		--dbg_assert(0==self.m_nAllPlayBet)
		self._gameView:updateAsset(self:GetMeUserItem().lScore-self.m_nAllPlayBet)
		assignArray(self.tabMyBets,cmd.AREA_COUNT-1,0)
		self._gameView:updateMyBets(self.tabMyBets)
	
		dbg_assert(0==self.m_nAllPlayBet)
		-- print("self:GetMeUserItem().lScore: ",self:GetMeUserItem().lScore)
		-- print("self.m_nAllPlayBet: ",self.m_nAllPlayBet)
		-- assert(false)
		local endIndex=nil
		for i=5,1,-1 do
			-- local noteNum=math.pow(10,i+1)
			local noteNum=self.m_lChipArray[1][i]
			if (self:GetMeUserItem().lScore-self.m_nAllPlayBet)>=noteNum then
				endIndex=i
				break
			end
		end

		if nil~=endIndex then
			self._gameView:enableNoteNumBtns(endIndex)
		end

	end
end


--银行消息
function GameLayer:onSocketInsureEvent( sub,dataBuffer )
    self:dismissPopWait()
    if sub == GameServer_CMD.SUB_GR_USER_INSURE_SUCCESS then
        local cmd_table = ExternalFun.read_netdata(GameServer_CMD.CMD_GR_S_UserInsureSuccess, dataBuffer)
        self.bank_success = cmd_table
        -- self:GetMeUserItem().lScore = cmd_table.lUserScore
        GlobalUserItem.tabAccountInfo.lUserScore  = cmd_table.lUserScore
    	GlobalUserItem.tabAccountInfo.lUserInsure = cmd_table.lUserInsure
        self._gameView:updateAsset(GlobalUserItem.tabAccountInfo.lUserScore-self.m_nAllPlayBet)
        self._gameView:onBankSuccess()
    elseif sub == GameServer_CMD.SUB_GR_USER_INSURE_FAILURE then
        local cmd_table = ExternalFun.read_netdata(GameServer_CMD.CMD_GR_S_UserInsureFailure, dataBuffer)
        self.bank_fail = cmd_table

        self._gameView:onBankFailure()
    elseif sub == GameServer_CMD.SUB_GR_USER_INSURE_INFO then --银行资料
        local cmdtable = ExternalFun.read_netdata(GameServer_CMD.CMD_GR_S_UserInsureInfo, dataBuffer)
        dump(cmdtable, "cmdtable", 6)

        self._gameView:onGetBankInfo(cmdtable)
    else
        print("unknow gamemessage sub is ==>"..sub)
    end
end

function GameLayer:sendNetData( cmddata )
    return self._gameFrame:sendSocketData(cmddata)
end

--申请取款
function GameLayer:sendTakeScore(lScore, szPassword )
    local cmddata = ExternalFun.create_netdata(GameServer_CMD.CMD_GR_C_TakeScoreRequest)
    cmddata:setcmdinfo(GameServer_CMD.MDM_GR_INSURE, GameServer_CMD.SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushbyte(GameServer_CMD.SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushscore(lScore)
    cmddata:pushstring(md5(szPassword),yl.LEN_PASSWORD)
    cmddata:pushstring(GlobalUserItem.szDynamicPass,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end

--申请存款
function GameLayer:sendSaveScore(lScore)
    --操作记录
    local buffer = ExternalFun.create_netdata(GameServer_CMD.CMD_GR_C_SaveScoreRequest)
    buffer:setcmdinfo(GameServer_CMD.MDM_GR_INSURE,GameServer_CMD.SUB_GR_SAVE_SCORE_REQUEST)
    buffer:pushbyte(GameServer_CMD.SUB_GR_SAVE_SCORE_REQUEST)
    buffer:pushscore(lScore)
    buffer:pushstring(GlobalUserItem.szDynamicPass,yl.LEN_PASSWORD)
    self:sendNetData(buffer)
end

--请求银行信息
function GameLayer:sendRequestBankInfo()
    local cmddata = CCmd_Data:create(67)
    cmddata:setcmdinfo(GameServer_CMD.MDM_GR_INSURE,GameServer_CMD.SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushbyte(GameServer_CMD.SUB_GR_QUERY_INSURE_INFO)
    -- cmddata:pushstring(md5(GlobalUserItem.tabAccountInfo.szPassword),yl.LEN_PASSWORD)
    cmddata:pushstring(GlobalUserItem.md5Passwd,yl.LEN_PASSWORD)

    self:sendNetData(cmddata)
end


--清除按钮消息
function GameLayer:OnCleanJetton() --done

	if self.m_gameStatu~=cmd.GS_PLACE_JETTON then
		return
	end

	--构造数据
	local cmddata = create_netdata(cmd.CMD_C_CleanMeJetton)
	cmddata:pushword(self:GetMeChairID())  

	local ret=self:SendData(cmd.SUB_C_CLEAN_JETTON,cmddata) 

end

--加注按钮消息
function GameLayer:OnPlaceJetton(sender) --done

	if self.m_gameStatu~=cmd.GS_PLACE_JETTON then
		return
	end
 
 	local cbJettonArea=sender.m_kind
	--合法判断
	dbg_assert(cbJettonArea>=1 and cbJettonArea<cmd.AREA_COUNT)
	if (not (cbJettonArea>=1 and cbJettonArea<cmd.AREA_COUNT)) then  return  end
	--状态判断
	if (self.m_gameStatu~=cmd.GS_PLACE_JETTON) then
		
		return 
	end

	if self.m_nCurrentNote==nil or self.m_nCurrentNote<0 then
		return
	end
	local index = #self.m_lChipArray[1]
	local currentNote = self.m_nCurrentNote
	while self.m_nCurrentNote >= self.m_lChipArray[1][1] do  --100为最小注额
		if self:GetMeUserItem().lScore -self.m_nAllPlayBet < self.m_nCurrentNote then
			self.m_nCurrentNote = self.m_lChipArray[1][index]
			for i=1,#self._gameView.noteNumEffs do
				if index == i then 
					self._gameView.noteNumEffs[i]:setVisible(true)
				else
					self._gameView.noteNumEffs[i]:setVisible(false)
				end
			end
		else
			break
		end
		index = index - 1
	end

	print("self.m_nCurrentNote == ",self.m_nCurrentNote)

	local cmddata = create_netdata(cmd.CMD_C_PlaceJetton)

	cmddata:pushbyte(cbJettonArea)   
	cmddata:pushscore(self.m_nCurrentNote)
	local ret=self:SendData(cmd.SUB_C_PLACE_JETTON,cmddata) 

	ExternalFun.playClickEffect( )
	dbg_assert(ret and ret~=0)
	print("ret: ",ret)
end


--续投
function GameLayer:OnLastPlaceJetton( ) --done
	if self.m_gameStatu~=cmd.GS_PLACE_JETTON then
		--showToast(self._gameView,"非下注时间，点击无效",1)
		return
	end
	 
 	local cmddata = create_netdata(cmd.CMD_C_ContinueJetton)
	cmddata:pushword(self:GetMeChairID())
	self.isOnLastPlaceJetton = true
	--发送消息
	local ret=self:SendData(cmd.SUB_C_CONTINUE_JETTON,cmddata)

end

--切换下注大小按钮消息
function GameLayer:OnNoteSwitch(sender,index)
	if self.m_gameStatu~=cmd.GS_PLACE_JETTON then
		return
	end

	-- print
	self.m_nCurrentNote=sender.m_noteNum

	for i=1,#self._gameView.noteNumEffs do
		if index == i then 
			self._gameView.noteNumEffs[i]:setVisible(true)
		else
			self._gameView.noteNumEffs[i]:setVisible(false)
		end
	end


end

-- 重置框架
function GameLayer:OnResetGameEngine()
    self._dataModle:removeAllUser()
    self._dataModle:initUserList(self:getUserList())
    -- self._gameView:cleanGameData()
end

function GameLayer:getUserList(  )
    return self._gameFrame._UserList
    -- dump(self._gameFrame._UserList,"草泥马",12)
    -- assert(false)
    -- return self._dataModle:getChairUserList()
end

--刷新所有玩家信息
function GameLayer:resetUser()
	-- --先清理玩家信息
 --    for i = 1, Game_CMD.GAME_PLAYER do
 --        local wViewChairId = self:SwitchViewChairID(i-1)
 --        self._gameView:OnUpdateUserExit(wViewChairId)
 --    end

    -- --初始化已有玩家
    -- self._dataModle:removeAllUser()
    -- for i = 1, Game_CMD.GAME_PLAYER do
    --     local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
    --     if userItem then
    --    		self._dataModle:addUser(userItem)
    --     end
   	-- end
end

--用户进入
function GameLayer:onEventUserEnter(wTableID, wChairID, useritem)
	--缓存用户
	self._dataModle:addUser(useritem)
	-- self._gameView:refreshUserList()
	-- self._gameView:onGetUserScore(useritem)
end

--用户状态
function GameLayer:onEventUserStatus(useritem, newstatus, oldstatus)
	if newstatus.cbUserStatus == yl.US_FREE or newstatus == yl.US_NULL then
		self._dataModle:removeUser(useritem)
		-- self._gameView:refreshUserList()
	else
		self._dataModle:updateUser(useritem)
		-- self._gameView:refreshUserList()
	end
end

--用户分数
function GameLayer:onEventUserScore(useritem)
	self._dataModle:updateUser(useritem)

	-- self._gameView:onGetUserScore(useritem)
end

function GameLayer:OnSubGameRecord(dataBuffer)
	-- body
	local tab = {7,0,1,2,3,4,5,6,8,9,10,11}

	local len = dataBuffer:getlen()
	local  pGameRecord=ExternalFun.read_netdata(cmd.CMD_S_SendRecord, dataBuffer)
	dump(pGameRecord,"pGameRecord",12)
	self._gameView.turnTableRecords = {}
	for i=1,#pGameRecord.GameRecordArrary[1] do
		if pGameRecord.GameRecordArrary[1][i] < 255 then
			self._gameView:AddTurnTableRecord(tab[pGameRecord.GameRecordArrary[1][i]+1])
		end
	end
	self._gameView:updateShowTurnTableRecord()
end

--申请上庄 c
function GameLayer:sendApplyBanker(pBool)
	
	if pBool then 
		if self.lApplyBankerCondition > self:GetMeUserItem().lScore then 
			return showToast(self,"上庄金币需要的达到"..self.lApplyBankerCondition,1,cc.c4b(0,255,0,255))
		end

	 	local cmddata = ExternalFun.create_netdata()
		self:SendData(cmd.SUB_C_APPLY_BANKER,cmddata)
	else
		if (self.m_nBankerUserID == self:GetMeUserItem().wChairID) and (self.m_gameStatu ~= cmd.GAME_STATUS_FREE) then 
			return showToast(self,"游戏过程中不能下庄",1)
		end

	 	local cmddata = ExternalFun.create_netdata()
		self:SendData(cmd.SUB_C_CANCEL_BANKER,cmddata)
	end

end

-- 切换庄家 s
function GameLayer:OnSubGhangeBanker(dataBuffer )
	local  pChangeBanker=ExternalFun.read_netdata(cmd.CMD_S_ChangeBanker, dataBuffer)
	-- dump("切换庄家",pChangeBanker,6)
	local userList = self:getUserList()
	self.m_nBankerUserID = pChangeBanker.wBankerUser
	self.m_wBankerUser = pChangeBanker.wBankerUser

	if 65535 == pChangeBanker.wBankerUser then
		self.m_nApplyBanker = {} 
		local tab = {}
		tab.szNickName = "系统坐庄"
		tab.lScore = 100000000
		self._gameView:updateApplyBanker(tab,pChangeBanker.wBankerUser)
	else

		dump(self.m_nApplyBanker,"self.m_nApplyBanker  1 ",12)

		for i = #self.m_nApplyBanker,1,-1 do 
			if self.m_nApplyBanker[i] == pChangeBanker.wBankerUser then
				table.remove(self.m_nApplyBanker,i)
				-- if pChangeBanker.wBankerUser == self.
			end
		end
		dump(self.m_nApplyBanker,"self.m_nApplyBanker  2 ",12)
		print()
		print("pChangeBanker.wBankerUser = ",pChangeBanker.wBankerUser)

		for k,v in pairs(userList) do
		 	if v.wChairID == pChangeBanker.wBankerUser then
		 		self._gameView:updateApplyBanker(v,pChangeBanker.wBankerUser)
		 	end
		end
	end

	ExternalFun.playSoundEffect("zhuangjianv.mp3")


	self._gameView:updateBankerCount()

end

-- 申请上庄 s
function GameLayer:OnSubApplyBanker(dataBuffer)
	local  pApplyBanker=ExternalFun.read_netdata(cmd.CMD_S_ApplyBanker, dataBuffer)
	dump(pApplyBanker,"CMD_S_ApplyBanker",12)
	self.m_nApplyBanker[#self.m_nApplyBanker+1] = pApplyBanker.wApplyUser
	self._gameView:updateBankerCount()
	

	for k,v in pairs(self:getUserList()) do
	 	if v.wChairID == self.m_wBankerUser  then
	 		self._gameView:updateApplyBanker(v,self.m_wBankerUser )
	 	end
	end

	if self.m_wBankerUser == 65535 then
		for k,v in pairs(self:getUserList()) do
		 	if v.wChairID == pApplyBanker.wApplyUser  then
		 		self._gameView:updateApplyBanker(nil,self.m_wBankerUser )
		 	end
		end
	end
	
	if pApplyBanker.wApplyUser == self:GetMeChairID() then 


		self._gameView:updateXzBtnRes("banker/q_banker_1.png","banker/q_banker_2.png",true)
		return showToast(self,"申请上庄成功！",1,cc.c4b(0,255,0,255))
	end
end

-- 取消上庄 s
function GameLayer:OnSubCancelBanker( dataBuffer )
	local  pCancelBanker=ExternalFun.read_netdata(cmd.CMD_S_CancelBanker, dataBuffer)
	dump(pCancelBanker,'pCancelBanker',12)

	for i = #self.m_nApplyBanker,1,-1 do 
		if self.m_nApplyBanker[i] == pCancelBanker.wCanceUser then
			table.remove(self.m_nApplyBanker,i)
		end
	end
	self._gameView:updateBankerCount()

	for k,v in pairs(self:getUserList()) do
	 	if v.wChairID == self.m_wBankerUser  then
	 		self._gameView:updateApplyBanker(v,self.m_wBankerUser )
	 	end
	end

	if self.m_wBankerUser == 65535 then
		for k,v in pairs(self:getUserList()) do
		 	if v.wChairID == pCancelBanker.wCanceUser then
		 		self._gameView:updateApplyBanker(nil,self.m_wBankerUser )
		 	end
		end
	end

	if pCancelBanker.wCanceUser == self:GetMeChairID() then 


		self._gameView:updateXzBtnRes("banker/x_banker_1.png","banker/x_banker_2.png",false)
		return showToast(self,"取消上庄成功！",1,cc.c4b(0,255,0,255))
	end

	
end

function GameLayer:getBankerCount()
	-- body
	return #self.m_nApplyBanker
end

function GameLayer:getChipArray()
	-- body
	return self.m_lChipArray
end

function GameLayer:getMyBets()
	local num = 0
	for k, v in pairs(self.tabMyBets) do
		if v > 0 then
			num = num + v
		end
	end
	return num
end

return GameLayer