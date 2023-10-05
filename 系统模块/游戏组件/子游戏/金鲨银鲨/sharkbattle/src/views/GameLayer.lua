local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")
local GameLayer = class("GameLayer", GameModel)

local module_pre = "game.yule.sharkbattle.src"
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameViewLayer = appdf.req(module_pre .. ".views.layer.GameViewLayer")
local GameServer_CMD = appdf.req(appdf.HEADER_SRC.."CMD_GameServer")

--cc.FileUtils:getInstance():addSearchPath(device.writablePath.."game/yule/sharkbattle/res")

local INT_MAX  =       2147483647

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


function GameLayer:ctor( frameEngine,scene )     
	--游戏变量
	print("gamelayer ctor begin")
	self.gameData={}
	self.gameData.lCellScore = 0
	self.gameData.lPlayScore = 0
	self.gameData.lPlayChip = 0
	self.gameData.nTurnTableRecord = nil
	self.gameData.lHasInChip = false
	self.gameData.allPlayBet=0 --自己单局下注的总大小

	--下注变量
	self.gameData.nUnitNote = cmd.MIN_UNITNOTE
	self.gameData.nCurrentNote = cmd.MIN_UNITNOTE
	self.gameData.lPlayBet={}
	self.gameData.lPlayAllBet={} --所有玩家单局对某动物的下注之和
	self.gameData.lPlayLastBet={}
	self.gameData.nAnimalMultiple={}
	assignArray(self.gameData.lPlayBet,cmd.ANIMAL_MAX,0)
	assignArray(self.gameData.lPlayAllBet,cmd.ANIMAL_MAX,0)
	assignArray(self.gameData.lPlayLastBet,cmd.ANIMAL_MAX,0)
	assignArray(self.gameData.nAnimalMultiple,cmd.ANIMAL_MAX,0) --倍率
	self.gameData.lPlayWin = 0
	self.gameData.lMyGameEndScore=0
	self.gameData.lServiceScore=0
	--设置变量
	self.gameData.lAreaLimitScore = 0
	self.gameData.lPlayLimitScore = 0
	print("before GameLayer.super.ctor")
    GameLayer.super.ctor(self, frameEngine, scene)
    print("gamelayer ctor end")
    self:InitTurnTableResultId()

    self.originalFPS=cc.Director:getInstance():getAnimationInterval()
    cc.Director:getInstance():setAnimationInterval(1/30)
    --cc.Director:getInstance():setDisplayStats(true)

    self.betNums={0,0,0,0,0,0,0,0,0,0,0,0} --下注数量
    self.dChipArray = {}
end

function GameLayer:CreateView()
	print("GameLayer:CreateView() begin")
	local this=self
    return GameViewLayer:create(this)
        :addTo(this)
end

--请求银行信息
function  GameLayer:sendRequestBankInfo()
    local cmddata = CCmd_Data:create(67)
    cmddata:setcmdinfo(GameServer_CMD.MDM_GR_INSURE,GameServer_CMD.SUB_GR_QUERY_INSURE_INFO)
    cmddata:pushbyte(GameServer_CMD.SUB_GR_QUERY_INSURE_INFO)
    local str = md5(GlobalUserItem.szPassword) or "d1fd5495e7b727081497cfce780b6456"
    cmddata:pushstring(str,yl.LEN_PASSWORD)
    self:sendNetData(cmddata)
end

function GameLayer:getGameKind()
    return cmd.KIND_ID
end

----------------------------------------------------------------------------------
--定时器
--空闲时间
local IDI_FREE		=			101									--空闲时间
local IDI_BET		=				102									--下注时间
local IDI_END		=				103									--结束时间
local IDI_DISPATCH_CARD	=		104									--剩余时间
local BET_NUMBER		=			100                             --初始下注大小

----------------------------------------------------------------------------------

--重置函数
 function GameLayer:OnResetGameEngine()
 
	--游戏变量
	self.gameData.lCellScore = 0 

	self.gameData.lPlayShowPrizes=0 --彩金
	self.gameData.lPlayScore = 0 --得分
	self.gameData.lGameScore=0 --总分

	self.gameData.lPlayChip = 0
	self.gameData.nTurnTableRecord = nil
	self.gameData.lHasInChip = false

	--下注变量
	self.gameData.nCurrentNote = 0
	self.gameData.nUnitNote = 0
	assignArray(self.gameData.lPlayBet,cmd.ANIMAL_MAX,0)
	assignArray(self.gameData.lPlayAllBet,cmd.ANIMAL_MAX,0)
	assignArray(self.gameData.lPlayLastBet,cmd.ANIMAL_MAX,0)
	self.gameData.lPlayWin = 0

	--设置变量
	self.gameData.lAreaLimitScore = 0
	self.gameData.lPlayLimitScore = 0

    self._gameView:reSetForNewGame()
    GameLayer.super.OnResetGameEngine(self)

	return true
end

--时钟删除
function GameLayer:OnEventGameClockKill( wChairID)
	return true
end

--时钟信息
function GameLayer:OnEventGameClockInfo( wChairID,  nElapse,  wClockID)

end

function GameLayer:getPlayerList(  )
    return self._gameFrame._UserList
end

--退出桌子
function GameLayer:onExitTable()
	local MeItem = self:GetMeUserItem()
	if MeItem and MeItem.cbUserStatus > yl.US_FREE then
		self:showPopWait()
		self:runAction(cc.Sequence:create(
			cc.CallFunc:create(
				function ()
					self._gameFrame:StandUp(1)
				end
				),
			cc.DelayTime:create(3),
			cc.CallFunc:create(
				function ()
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
	self._gameFrame:onCloseSocket()
	self:KillGameClock()
	cc.Director:getInstance():setAnimationInterval(self.originalFPS)
	self._scene:onKeyBack()
end

function GameLayer:InitTurnTableResultId()
	self.turnTableResultId={
	cmd.ANIMAL_SLIVER_SHARK,cmd.ANIMAL_EAGLE,cmd.ANIMAL_EAGLE,cmd.ANIMAL_EAGLE,
		cmd.ANIMAL_GOLD_SHARK, cmd.ANIMAL_LION, cmd.ANIMAL_LION, cmd.ANIMAL_LION, 
		cmd.ANIMAL_SLIVER_SHARK,cmd.ANIMAL_PANDA,cmd.ANIMAL_PANDA,cmd.ANIMAL_GOLD_SHARK,
		cmd.ANIMAL_MONKEY,cmd.ANIMAL_MONKEY,cmd.ANIMAL_SLIVER_SHARK,cmd.ANIMAL_RABBIT,
		cmd.ANIMAL_RABBIT,cmd.ANIMAL_RABBIT,cmd.ANIMAL_GOLD_SHARK,cmd.ANIMAL_SWALLOW,
		cmd.ANIMAL_SWALLOW,cmd.ANIMAL_SWALLOW,cmd.ANIMAL_SLIVER_SHARK,cmd.ANIMAL_PIGEON,
		cmd.ANIMAL_PIGEON,cmd.ANIMAL_GOLD_SHARK,cmd.ANIMAL_PEACOCK,cmd.ANIMAL_PEACOCK
	}
	assert(#self.turnTableResultId==28)
end

function GameLayer:TurnTableAnimal(turnTableIndex) --0-27，转盘左上角为索引开始处
	print("turnTableIndex+1: ",turnTableIndex+1)
	
	print(self.turnTableResultId[turnTableIndex+1])
	--local turnTableIndex=turnTableIndex%28 --temp
	assert(self.turnTableResultId[turnTableIndex+1])
	return self.turnTableResultId[turnTableIndex+1] --位置turnTableIndex+1上对应的结果种类
end

local ithMsg=0
--游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)
	ithMsg=ithMsg+1
	print("ithMsg: ",ithMsg)
	if sub== cmd.SUB_S_GAME_FREE then	--空闲时间
		print("SUB_S_GAME_FREE")
		return self:OnSubGameFree(dataBuffer)
	elseif sub==cmd.SUB_S_GAME_START then	--游戏开始
		print("SUB_S_GAME_START")
		return self:OnSubGameStart(dataBuffer)
	elseif sub==cmd.SUB_S_GAME_END then	--游戏结束
		print("SUB_S_GAME_END")
		return self:OnSubGameEnd(dataBuffer)
	elseif sub==cmd.SUB_S_PLAY_BET then	--玩家下注
		print("SUB_S_PLAY_BET")
		return self:OnSubPlayBet(dataBuffer)
	elseif sub==cmd.SUB_S_PLAY_BET_FAIL then--下注失败
		print("SUB_S_PLAY_BET_FAIL")
		return self:OnSubPlayBetFail(dataBuffer)
	elseif sub==cmd.SUB_S_BET_CLEAR then	--清除下注
		print("SUB_S_BET_CLEAR")
		return self:OnSubBetClear(dataBuffer)
	end
end

--场景消息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)
 	ithMsg=ithMsg+1
	print("ithMsg: ",ithMsg)
	ExternalFun.playBackgroudAudio( "GAME_FREE.wav" )

	if cbGameStatus==cmd.GAME_SCENE_FREE then
		print(" cbGameStatus==cmd.GAME_SCENE_FREE")

			--消息处理
		local pStatusFree = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
		
			--设置状态
		self.m_cbGameStatus=cmd.GAME_SCENE_FREE
		self._gameView:SetGameStatus(cmd.GAME_SCENE_FREE)
		dump(pStatusFree,"GAME_SCENE_FREE")

		--设置自己位置
		--self._gameView:SetMineChairID(self:SwitchViewChairID(self:GetMeChairID()))

		--设置时间
		self:SetGameClock(self:GetMeChairID(), IDI_FREE, pStatusFree.cbTimeLeave)
		--设置变量
		self.gameData.lCellScore = pStatusFree.lCellScore
		self.gameData.lAreaLimitScore = pStatusFree.lAreaLimitScore
		self.gameData.lPlayLimitScore = pStatusFree.lPlayLimitScore
		self.gameData.lPlayChip = 0

		--设置游戏记录
		for i=1,cmd.RECORD_COUNT_MAX do
			print("pStatusFree.nTurnTableRecord[1]"..i,pStatusFree.nTurnTableRecord[1][i])
			if (INT_MAX~=pStatusFree.nTurnTableRecord[1][i] ) then
				self._gameView:AddTurnTableRecord( self:TurnTableAnimal(pStatusFree.nTurnTableRecord[1][i]) )
			end
            -- 波波：暂时注释
--			if (INT_MAX~=pStatusFree.nTurnTableRecordExt[1][i] ) then
--				self._gameView:AddTurnTableRecord( self:TurnTableAnimal(pStatusFree.nTurnTableRecordExt[1][i]) )
--			end
		end

		self._gameView:SetShowTurnTableRecord(true) 
		 
		--设置单位注
		local array = pStatusFree.dChipArray[1]
		self.dChipArray = array
		self.gameData.nUnitNote = self.dChipArray[1] --BET_NUMBER
		self.gameData.nCurrentNote = self.dChipArray[1] --BET_NUMBER
		self._gameView:SetCurrentNote(self.gameData.nCurrentNote)
		self._gameView:SetAnimalMultiple(self.gameData.nAnimalMultiple) --设置倍率
		self._gameView:enableAllBtns(false)
	elseif cbGameStatus==cmd.GAME_SCENE_BET then
		print("cbGameStatus==cmd.GAME_SCENE_BET")
	
		local pStatusPlay = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer)
		-- appdf.printTable(pStatusPlay)
		dump(pStatusPlay,"GAME_SCENE_BET")

		--设置状态
		self.m_cbGameStatus=cmd.GAME_SCENE_BET 
		self._gameView:SetGameStatus(cmd.GAME_SCENE_BET)

		--设置自己位置
		--self._gameView:SetMineChairID(self:SwitchViewChairID(self:GetMeChairID()))

		--设置时间
		assert(pStatusPlay.cbTimeLeave)
		self:SetGameClock(self:GetMeChairID(), IDI_BET, pStatusPlay.cbTimeLeave)
		--设置变量
		self.gameData.lCellScore = pStatusPlay.lCellScoreend
		self.gameData.lAreaLimitScore = pStatusPlay.lAreaLimitScore
		self.gameData.lPlayLimitScore = pStatusPlay.lPlayLimitScore
		--设置游戏记录
		for i=1,cmd.RECORD_COUNT_MAX do
			print("pStatusPlay.nTurnTableRecord[1]"..i,pStatusPlay.nTurnTableRecord[1][i])
			if ( INT_MAX~= pStatusPlay.nTurnTableRecord[1][i] ) then
				self._gameView:AddTurnTableRecord(self:TurnTableAnimal(pStatusPlay.nTurnTableRecord[1][i]) )
			end
            -- 波波：暂时注释
--			if ( INT_MAX~= pStatusPlay.nTurnTableRecordExt[1][i] ) then
--				self._gameView:AddTurnTableRecord(self:TurnTableAnimal(pStatusPlay.nTurnTableRecordExt[1][i]) )
--			end
		end

		self._gameView:SetShowTurnTableRecord(true)

		self._gameView:SetAnimalMultiple( pStatusPlay.nAnimalMultiple[1] )
		--self._gameView:SetPlayAllBet( pStatusPlay.lAllBet[1] )
		self._gameView:SetPlayBet( pStatusPlay.lPlayBet[1] )
		self._gameView:SetAllPlayBet(pStatusPlay.lAllBet[1])
		 
		--设置单位注
		self.dChipArray = pStatusPlay.dChipArray[1]
		self.gameData.nUnitNote = self.dChipArray[1] --BET_NUMBER
		self.gameData.nCurrentNote = self.dChipArray[1] --BET_NUMBER
		self._gameView:SetCurrentNote(self.gameData.nCurrentNote)
		self._gameView:enableAllBtns(true)
	elseif cbGameStatus==cmd.GAME_SCENE_END then
		print("cbGameStatus==cmd.GAME_SCENE_END")

		local pStatusEnd=ExternalFun.read_netdata(cmd.CMD_S_StatusEnd, dataBuffer)
		
		-- appdf.printTable(pStatusEnd)
		dump(pStatusEnd,"GAME_SCENE_END")

		--设置状态
		self.m_cbGameStatus=cmd.GAME_SCENE_END 
		--这里设置界面为空闲状态， 以显示时间，GAME_SCENE_END消息结构体不包含转盘结果，所以不能播放结算动画
		self._gameView:SetGameStatus(cmd.GAME_SCENE_FREE) --PC端是设置为cmd.GAME_SCENE_BET

		--设置自己位置
		--self._gameView:SetMineChairID(self:SwitchViewChairID(self:GetMeChairID()))
			
		--设置时间
		--assert(pStatusEnd.cbTimeLeave)
		self:SetGameClock(self:GetMeChairID(), IDI_END, pStatusEnd.cbTimeLeave)
		--设置变量
		self.gameData.lCellScore = pStatusEnd.lCellScore
		self.gameData.lAreaLimitScore = pStatusEnd.lAreaLimitScore
		self.gameData.lPlayLimitScore = pStatusEnd.lPlayLimitScore

		--设置游戏记录
		for i=1,cmd.RECORD_COUNT_MAX do
			print("pStatusEnd.nTurnTableRecord[1]"..i,pStatusEnd.nTurnTableRecord[1][i])
			if ( INT_MAX~=pStatusEnd.nTurnTableRecord[1][i] ) then
				self._gameView:AddTurnTableRecord( self:TurnTableAnimal(pStatusEnd.nTurnTableRecord[1][i]) )
			end
            -- 波波：暂时注释
--			if ( INT_MAX~=pStatusEnd.nTurnTableRecordExt[1][i] ) then
--				self._gameView:AddTurnTableRecord( self:TurnTableAnimal(pStatusEnd.nTurnTableRecordExt[1][i]) )
--			end
		end

		self._gameView:SetShowTurnTableRecord(true)

		--设置全局下注
		self._gameView:SetAnimalMultiple( pStatusEnd.nAnimalMultiple[1] )
		self._gameView:SetPlayBet( pStatusEnd.lPlayBet[1] )
		self._gameView:SetAllPlayBet(pStatusEnd.lAllBet[1])

		--设置单位注
		self.dChipArray = pStatusEnd.dChipArray[1]
		self.gameData.nUnitNote = self.dChipArray[1] --BET_NUMBER
		self.gameData.nCurrentNote = self.dChipArray[1] --BET_NUMBER
		self._gameView:SetCurrentNote(self.gameData.nCurrentNote)
		self._gameView:enableAllBtns(false)
	end
	
	self:dismissPopWait()
end


--设置事件
function GameLayer:OnGameOptionChange()
	
end

--resetGame --被回调

--空闲时间
function GameLayer:OnSubGameFree(dataBuffer)
	self.m_cbGameStatus=cmd.GAME_SCENE_FREE
	 ExternalFun.playBackgroudAudio( "GAME_FREE.wav" )

	local pGameFree = ExternalFun.read_netdata(cmd.CMD_S_GameFree, dataBuffer)

	self.gameData.allPlayBet=0
	--设置时间
	assert(pGameFree.cbTimeLeave)
	self:SetGameClock(self:GetMeChairID(),IDI_FREE,pGameFree.cbTimeLeave)

	--设置状态
	self._gameView:SetGameStatus(cmd.GAME_SCENE_FREE)

	self._gameView:SetPlayShowWin(0) --设置显示得分
	self._gameView:SetPlayShowGameScore(0) 
	self._gameView:UpdateShowTotalBet(0)
	--设置倍数
	assignArray(self.gameData.nAnimalMultiple,cmd.ANIMAL_MAX,0)
	self._gameView:SetAnimalMultiple(self.gameData.nAnimalMultiple)

	--保存上一次
	for i=1,cmd.ANIMAL_MAX do
		if self.gameData.lPlayBet[i] > 0 then
			assignArray(self.gameData.lPlayLastBet,self.gameData.lPlayBet)
			break
		end
	end
	
	--玩家下注清空
	assignArray(self.gameData.lPlayBet,cmd.ANIMAL_MAX,0)
	assignArray(self.gameData.lPlayAllBet,cmd.ANIMAL_MAX,0)
	--self._gameView:CleanUserJetton()
	--设置记录
	-- if ( self.gameData.nTurnTableRecord ~= nil ) then
	-- 	print("self.gameData.nTurnTableRecord: ",self.gameData.nTurnTableRecord)
	-- 	self._gameView:AddTurnTableRecord( self:TurnTableAnimal(self.gameData.nTurnTableRecord) )
	-- end
	-- self._gameView:SetShowTurnTableRecord(true)

	--self._gameView:EndWin()
	self._gameView:SetPlayBet(self.gameData.lPlayBet) --设置下注大小
	self._gameView:SetAllPlayBet(self.gameData.lPlayAllBet)
	self._gameView:enableAllBtns(false)
end

--游戏开始
function GameLayer:OnSubGameStart(dataBuffer)
	self.betNums={0,0,0,0,0,0,0,0,0,0,0,0} --下注数量

	self._gameView.goonBtn:setEnabled(true)
 	self._gameView:updatePlayShowWin()
	self._gameView:UpdateShowTotalScore(GlobalUserItem.tabAccountInfo.lUserScore)
	--消息处理
	local pGameStart = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)

	self.m_cbGameStatus=cmd.GAME_SCENE_BET
	self._gameView:SetGameStatus(cmd.GAME_SCENE_BET)

	--设置时间	
	self:SetGameClock(self:GetMeChairID(), IDI_BET, pGameStart.cbTimeLeave)

	--设置倍数
	self._gameView:SetAnimalMultiple(pGameStart.nAnimalMultiple[1])


	--玩家下注清空
	assignArray(self.gameData.lPlayBet,cmd.ANIMAL_MAX,0)
	assignArray(self.gameData.lPlayAllBet,cmd.ANIMAL_MAX,0)

	--设置界面
	self._gameView:SetPlayBet(self.gameData.lPlayBet)
	self._gameView:SetAllPlayBet(self.gameData.lPlayAllBet)
	self._gameView:enableAllBtns(true)
end

--游戏结算
function GameLayer:OnSubGameEnd(dataBuffer)
	 ExternalFun.playBackgroudAudio( "GAME_START.wav" )

	local pGameEnd = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer)
	dump(pGameEnd,"pGameEnd")
	local wMeChair = self:GetMeChairID()
	print("pGameEnd.lPlayShowPrizes: ",pGameEnd.lPlayShowPrizes)
	--设置状态
	self.m_cbGameStatus=cmd.GAME_SCENE_END
	self._gameView:SetGameStatus(cmd.GAME_SCENE_END)

	--设置时间
	self:SetGameClock(wMeChair, IDI_END, pGameEnd.cbTimeLeave)

	--结束动画
	assert(pGameEnd.nTurnTableTarget[1][1]<28)
	
	self._gameView:GameOver(pGameEnd.bTurnTwoTime, pGameEnd.nTurnTableTarget[1], pGameEnd.nPrizesMultiple, pGameEnd.lPlayWin[1], pGameEnd.lPlayPrizes,pGameEnd.lGameScore)

	--保存记录
	self.gameData.nTurnTableRecord = pGameEnd.nTurnTableTarget[1][1]

	self._gameView:SetPlayShowPrizes(pGameEnd.lPlayShowPrizes) 	
	self._gameView:SetPlayShowGameScore(pGameEnd.lGameScore) 

	self.gameData.lMyGameEndScore = pGameEnd.lGameScore
	self.gameData.lServiceScore = pGameEnd.lServiceScore
	--设置时间
	--self:SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd.cbTimeLeave)
	self._gameView:enableAllBtns(false)
end


function GameLayer:OnSubPlayBet(dataBuffer) --点击续投也是收到这个消息
	local pPlayBet = ExternalFun.read_netdata(cmd.CMD_S_PlayBet, dataBuffer)
	if pPlayBet.wChairID ~=self:GetMeChairID() then
		local index=pPlayBet.nAnimalIndex+1
		self.gameData.lPlayAllBet[index]=self.gameData.lPlayAllBet[index]+pPlayBet.lBetChip
		self._gameView:SetAllPlayBet(self.gameData.lPlayAllBet[index],index)
	end
end


--下注失败
function GameLayer:OnSubPlayBetFail(dataBuffer)

	local pPlayBetFail = ExternalFun.read_netdata(cmd.CMD_S_PlayBetFail, dataBuffer)
	--自己才处理
	if  self:GetMeChairID() == pPlayBetFail.wChairID   then
		
		--设置变量
		local animaxIndex=pPlayBetFail.nAnimalIndex+1
		local temp1=self.gameData.lPlayBet[animaxIndex]
		local temp2=self.gameData.lPlayAllBet[animaxIndex]
		self.gameData.lPlayBet[animaxIndex] =temp1- pPlayBetFail.lBetChip
		assert(self.gameData.lPlayBet[animaxIndex] >=0)
		if self.gameData.lPlayBet[animaxIndex]<0 then
			self.gameData.lPlayBet[animaxIndex]=temp1
		end
		self.gameData.lPlayAllBet[animaxIndex] =temp2- pPlayBetFail.lBetChip
		self.gameData.allPlayBet=self.gameData.allPlayBet- pPlayBetFail.lBetChip
		--设置界面
		self._gameView:SetPlayBet(self.gameData.lPlayBet[animaxIndex], animaxIndex)
		self._gameView:SetAllPlayBet(self.gameData.lPlayAllBet[animaxIndex], animaxIndex)
		self._gameView:UpdateShowTotalScore(GlobalUserItem.tabAccountInfo.lUserScore-self.gameData.allPlayBet)
	end
end

--对其他玩家betClear消息进行处理
function GameLayer:OnSubBetClear(dataBuffer)
	local pBetClear = ExternalFun.read_netdata(cmd.CMD_S_BetClear, dataBuffer)
	if pBetClear.wChairID ~=self:GetMeChairID() then
		for i=1,cmd.ANIMAL_MAX do
			if pBetClear.lPlayBet[i] > 0 then
				self.gameData.lPlayAllBet[i] =self.gameData.lPlayAllBet[i] - pBetClear.lPlayBet[i]
				self._gameView:SetAllPlayBet(self.gameData.lPlayAllBet[i],i)
			end
		end
	end
end

--下注按钮消息
function GameLayer:OnMessagePlayBet(sender)
	print("GameLayer:OnMessagePlayBet() begin\n")

	if self._gameView:GetGameStatus()~=cmd.GAME_SCENE_BET then return end

	--判断信息
	if ( sender.m_kind < 0  or  sender.m_kind>=cmd.ANIMAL_MAX ) then
	
		return 
	end

	if(self.gameData.nCurrentNote < 0) then  return  end

	local meItem = self:GetMeUserItem()
	--appdf.printTable(meItem)
	if meItem then
		if self.gameData.nCurrentNote>meItem.lScore then  return end
		local lUserBet=0
		for i=1,cmd.ANIMAL_MAX do
			lUserBet=lUserBet+self.gameData.lPlayBet[i]
		end
		if lUserBet+self.gameData.nCurrentNote>meItem.lScore then  return end
	end

	if ( self.m_cbGameStatus ~= cmd.GAME_SCENE_BET  ) then  return end

	--定义变量
	local nAnimalIndex = sender.m_kind+1

	--判断超额
	if ( (self.gameData.lAreaLimitScore ~= 0  and  
		self.gameData.lPlayAllBet[nAnimalIndex] + self.gameData.nCurrentNote > self.gameData.lAreaLimitScore)
		 or  (self.gameData.lPlayLimitScore ~= 0  and  self.gameData.lPlayBet[nAnimalIndex] + self.gameData.nCurrentNote > self.gameData.lPlayLimitScore) )
		then
		
		 return 
	end

	ExternalFun.playClickEffect()

	--设置变量
	self.gameData.allPlayBet=self.gameData.allPlayBet+self.gameData.nCurrentNote
	self.gameData.lPlayBet[nAnimalIndex] =self.gameData.lPlayBet[nAnimalIndex] + self.gameData.nCurrentNote
	self.gameData.lPlayAllBet[nAnimalIndex] =self.gameData.lPlayAllBet[nAnimalIndex] + self.gameData.nCurrentNote

	self._gameView:UpdateShowTotalScore(GlobalUserItem.tabAccountInfo.lUserScore-self.gameData.allPlayBet)
	self._gameView:UpdateShowTotalBet(self.gameData.allPlayBet)
	
	--设置界面
	self._gameView:SetPlayBet(self.gameData.lPlayBet[nAnimalIndex], nAnimalIndex)
	self._gameView:SetAllPlayBet(self.gameData.lPlayAllBet[nAnimalIndex], nAnimalIndex)

	--发送消息
	local cmddata = ExternalFun.create_netdata(cmd.CMD_C_PlayBet)
	cmddata:pushint(sender.m_kind)  
	cmddata:pushscore(self.gameData.nCurrentNote)
	self:SendData(cmd.SUB_C_PLAY_BET,cmddata)  
end

--续压按钮消息
 function GameLayer:OnMessageOperationRenewal()
 	if self._gameView:GetGameStatus()~=cmd.GAME_SCENE_BET then return end

 	local bFirst=false
 	for i=1,cmd.ANIMAL_MAX do
 		if self.gameData.lPlayLastBet[i]>0 then
 			bFirst=true
 			break
 		end
 	end

	--第一局
	if bFirst==false then
		showToast(self._gameView,"之前无下注，无法续投！",1)
		return 
	end
	
	--判断是否够下注
	local temp=0
	for i=1,cmd.ANIMAL_MAX do
		assert(self.gameData.lPlayLastBet[i]>=0)
		temp=temp+self.gameData.lPlayLastBet[i]
	end
	if temp>GlobalUserItem.tabAccountInfo.lUserScore then
		showToast(self._gameView,"您的金币不足，无法续压上轮下注！",1)
		return
	end

	--先清空
	local cmddata = CCmd_Data:create()
	self:SendData(cmd.SUB_C_BET_CLEAR,cmddata)

	for i=1,cmd.ANIMAL_MAX do
		self.gameData.lPlayAllBet[i] =self.gameData.lPlayAllBet[i]- self.gameData.lPlayBet[i]
		self.gameData.lPlayBet[i] = 0
	end
	self.gameData.allPlayBet=0

	--self.testBets={}
	local function sendBet( num , betIdx)
		local leftNum = num
		while(true)
		do
			local bet = 0
			for k = 6, 1, -1 do
				if leftNum >= self.dChipArray[k] then
					bet = self.dChipArray[k]
					break
				end
			end

			local cmddata = CCmd_Data:create(12)
			cmddata:pushint(betIdx)
			cmddata:pushscore(bet)
			self:SendData(cmd.SUB_C_PLAY_BET,cmddata)
			leftNum = leftNum - bet
			if leftNum <= 0 then
				return
			end
		end
	end
	--重新下注
	for i=1,cmd.ANIMAL_MAX do
		self.gameData.lPlayBet[i] = self.gameData.lPlayLastBet[i]
		self.gameData.lPlayAllBet[i] =self.gameData.lPlayAllBet[i] +self.gameData.lPlayLastBet[i]
		self.gameData.allPlayBet=self.gameData.allPlayBet+self.gameData.lPlayLastBet[i]
		--发送消息
		if self.gameData.lPlayBet[i] > 0 then
			local leftNum = self.gameData.lPlayBet[i]
			sendBet(self.gameData.lPlayBet[i], i - 1)
			-- local cmddata = CCmd_Data:create(12)
			-- cmddata:pushint(i-1)
			-- cmddata:pushscore(bet)
			-- self:SendData(cmd.SUB_C_PLAY_BET,cmddata) 
		end
		--self.testBets[i]=self.gameData.lPlayBet[i]
	end

	self._gameView.goonBtn:setEnabled(false)
	--设置界面
	self._gameView:SetPlayBet(self.gameData.lPlayBet)
	--showToast(self,"续押大小："..self.gameData.allPlayBet,1)
	self._gameView:SetAllPlayBet(self.gameData.lPlayAllBet)
	self._gameView:UpdateShowTotalScore(GlobalUserItem.tabAccountInfo.lUserScore-self.gameData.allPlayBet)
	self._gameView:UpdateShowTotalBet(self.gameData.allPlayBet)
end


--取消按钮消息
function GameLayer:OnMessageOperationCancel()
	if self._gameView:GetGameStatus()~=cmd.GAME_SCENE_BET then return end
	--发送消息
	local cmddata = CCmd_Data:create()
	self:SendData(cmd.SUB_C_BET_CLEAR,cmddata) 

	--清空变量
	for i=1,cmd.ANIMAL_MAX do

	 	self.gameData.lPlayAllBet[i] =self.gameData.lPlayAllBet[i]- self.gameData.lPlayBet[i]
	 	self.gameData.allPlayBet=self.gameData.allPlayBet- self.gameData.lPlayBet[i]
		self.gameData.lPlayBet[i] = 0
		--设置界面
		self._gameView:SetPlayBet(self.gameData.lPlayBet[i],i) 
		self._gameView:SetAllPlayBet(self.gameData.lPlayAllBet[i],i)
	end
	print("self.gameData.allPlayBet: ",self.gameData.allPlayBet)
	assert(self.gameData.allPlayBet==0)
	self._gameView:UpdateShowTotalScore(GlobalUserItem.tabAccountInfo.lUserScore-self.gameData.allPlayBet)
	self._gameView:UpdateShowTotalBet(self.gameData.allPlayBet)
end

--切换下注大小按钮消息
function GameLayer:OnMessageOperationSwitch(sender)
	if self._gameView:GetGameStatus()~=cmd.GAME_SCENE_BET then return end

	local idx = 1
	for k ,v in ipairs(self.dChipArray) do
		if v == self.gameData.nUnitNote then
			idx = k
		end
	end

	if sender.m_plusNote==true then
		idx = idx + 1
		-- self.gameData.nUnitNote= math.min(self.gameData.nUnitNote*10,cmd.MAX_UNITNOTE)
	else
		idx = idx - 1
		-- self.gameData.nUnitNote= math.max(self.gameData.nUnitNote/10,cmd.MIN_UNITNOTE)
	end
	if idx == 0 then
		idx = 6
	elseif idx > 6 then
		idx = 1
	end
	self.gameData.nUnitNote= self.dChipArray[idx]
	self.gameData.nCurrentNote=self.gameData.nUnitNote
	--设置界面
	self._gameView:SetCurrentNote(self.gameData.nCurrentNote)

end


--银行消息
function GameLayer:onSocketInsureEvent( sub,dataBuffer )
    self:dismissPopWait()
    if sub == GameServer_CMD.SUB_GR_USER_INSURE_SUCCESS then
        local cmd_table = ExternalFun.read_netdata(GameServer_CMD.CMD_GR_S_UserInsureSuccess, dataBuffer)
        self.bank_success = cmd_table
        GlobalUserItem.tabAccountInfo.lUserScore = cmd_table.lUserScore
    	GlobalUserItem.tabAccountInfo.lUserInsure = cmd_table.lUserInsure
        self._gameView:UpdateShowTotalScore(GlobalUserItem.tabAccountInfo.lUserScore-self.gameData.allPlayBet)
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


--用户分数
function GameLayer:onEventUserScore( item )
    if item.wTableID ~= self:GetMeUserItem().wTableID then
       return
    end

    if item.wChairID == self:GetMeChairID() then
    	GlobalUserItem.tabAccountInfo.lUserScore = item.lScore
		self._gameView:UpdateShowTotalScore(GlobalUserItem.tabAccountInfo.lUserScore)
    end
end

return GameLayer