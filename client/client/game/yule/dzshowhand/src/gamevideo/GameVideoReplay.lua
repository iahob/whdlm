--
-- 录像处理

-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.yule.dzshowhand.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")


function GameVideoReplay:ctor( scene )
	GameVideoReplay.super.ctor(self)
end

-- 回放邀请信息
function GameVideoReplay:getShareMessage( tabParam )
	local playbackcode = tabParam.dwPlayBackCode or 0
	local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
	return
	{
	title = "回放码:" .. playbackcode,
	content = "德州扑克牌局回放,德州扑克游戏精彩刺激，一起来玩吧！",
	url = url,
	imagepath = "",
	isImage = "false",
	}
end

-- 获取单局结算界面
function GameVideoReplay:getSingleGameSettlement( tabParam )
	-- todo
	return nil
end

-- 开始回放
function GameVideoReplay:onStartReplay(gameLayer)
	-- 隐藏设置
end

-- 消息读取
function GameVideoReplay:onReadMessage( pBuffer )
	local datalen = pBuffer:getlen()
	print("data len ",datalen)
	
	local curlen = pBuffer:getcurlen()
	if curlen < datalen then
		local sub = pBuffer:readword()
		print("sub is =======",sub)
		if cmd.SUB_S_GAME_START == sub then
			print("回放->游戏开始")
			local t = --游戏开始
			{
			{k = "szNickName", t = "string", s = 32},                   --用户昵称
			{k = "wChairID",t="word"}, --用户椅子号
			{k = "wCurrentUser",t="word"},  --当前玩家
			{k="wDUser",t="word"},--D玩家
			{k="wMaxChipInUser",t="word"},--大盲注玩家
			{k="cbDynamicJoinCount",t="byte"},--动态加入玩家数量
			{k="wDynamicJoinUser",t="word",l={cmd.GAME_PLAYER}},--动态加入罚盲玩家

			{k="wMinChipInUser",t="word"},--小盲注玩家	
			{k="lTurnMaxScore",t="score"},				--最大下注
			{k="lTurnLessScore",t="score"},				--最小下注
			{k="lAddLessScore",t="score"},				--最大下注

			{k="lUserMaxScore",t="score",l={cmd.GAME_PLAYER}},--玩家分数
			{k="cbCardData",t="byte",l={cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT,cmd.MAX_COUNT}},--用户扑克
			{k="cbPlayStatus",t="byte",l={cmd.GAME_PLAYER}},--用户状态
			{k="bStandUp",t="bool",l={cmd.GAME_PLAYER}}--玩家站起

			{k="lCellScore",t="score"},					--盲注分数
			{k="lAnteScore",t="score"},					--固定底注
			{k="lMinBetScore",t="score"},				--最低携带金币
			{k="lMaxBetScore",t="score"},				--最高携带金币
			{k="lAllBetScore",t="score"},					--累计带入限制

			{k="cbTime",t="byte"},					--开房时长0.5小时倍数
			{k="cbMaxPlayerCount",t="byte"},				--最大人数

			}
			local cmdtable = {}
			cmdtable.msgsub = 101
			cmdtable.startinfo = {}
			
			local buffer = ExternalFun.read_netdata(t, pBuffer)
			table.insert(cmdtable.startinfo, buffer)
			
			dump(buffer, "game start info")
			
			if buffer.wPlayerCount>=2 then
				for i=1,buffer.wPlayerCount-1 do
					buffer = ExternalFun.read_netdata(t, pBuffer)
					table.insert(cmdtable.startinfo, buffer)
				end
			end
			
			return cmdtable
		elseif cmd.SUB_S_OUT_CARD == sub then
			local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_OutCard, pBuffer)
			cmdtable.msgsub = 102
			return cmdtable
		elseif cmd.SUB_S_PASS_CARD == sub then
			local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_PassCard, pBuffer)
			cmdtable.msgsub = 103
			return cmdtable
		elseif cmd.SUB_S_GAME_END == sub then
			local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, pBuffer)
			cmdtable.msgsub = 104
			return cmdtable
		elseif cmd.SUB_S_AUTOMATISM == sub then
			local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_UserAutomatism, pBuffer)
			cmdtable.msgsub = 109
			return cmdtable
		end
	end
	return nil
end

-- 约战总结算
function GameVideoReplay:onGetPrivateEndMessage(videoDetailInfo,userList)
	local cmdtable = {}
	cmdtable.msgsub = 2000
	
	local scorelist = {}
	for k,v in pairs(userList) do
		scorelist[v.wChairID + 1] = v.dwTotalScore
	end
	cmdtable.lScore = {}
	cmdtable.lScore[1] = scorelist
	
	return cmdtable
end

-- 消息处理
function GameVideoReplay:onHandleMessage( gameLayer, msgTable, bForward, bBackward )
	gameLayer._gameView.bIsVideo = true
	bForward = bForward or false
	bBackward = bBackward or false
	local sub = msgTable.msgsub
	if nil == gameLayer or nil == gameLayer._gameView then
		return nil, false, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_VIEW_ERROR
	end
	
	if 101 == sub then
		-- 游戏开始
		print("回放 开始")
		return self:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
	elseif 102 == sub then
		--用户出牌
		print("回放 用户出牌")
		return self:onGetVideoOutCard(gameLayer, msgTable, bForward, bBackward)
	elseif 103 == sub then
		--pass
		print("回放 用户PASS")
		return self:onGetVideoPassCard(gameLayer, msgTable, bForward, bBackward)
	elseif 104 == sub then
		print("回放 游戏结束")
		return self:onGetVideoGameEnd(gameLayer, msgTable, bForward, bBackward)
	elseif 109 == sub then
		print("回放 用户托管")
		return self:onGetVideoTrustTee(gameLayer, msgTable, bForward, bBackward)
	end
	return 10
end

-- 回放场景
function GameVideoReplay:onGetVideoScene(gameLayer, msgTable, bForward, bBackward)
	return 5, true
end
-- 游戏开始
function GameVideoReplay:onGetVideoSendCard(gameLayer, msgTable, bForward, bBackward)
	
	--清理桌面
	gameLayer._gameView:onResetData()
	gameLayer._gameView:setGameMultiple(1)
	
	if bBackward then
		print("快退处理 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
		local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
		if not self:backwardCheck(msg, msgTable) then
			return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
		end
	end
	
	local msg = {}
	msg.msgsub = msgTable.msgsub
	msg.userCards = {}
	gameLayer._userCards = {}
	--判断游戏模式
	gameLayer.cbUserMaxCardCount = msgTable.startinfo[1].bSixTheenCard and 16 or 15
	
	--扑克数据
	assert(#msgTable.startinfo == cmd.GAME_PLAYER)
	for i = 1, cmd.GAME_PLAYER do
		msg.userCards[i] = clone(msgTable.startinfo[i].cbCardData[1])
	end
	
	for k,v in pairs(msgTable.startinfo) do
		-- 视图转换
		local viewId = gameLayer:SwitchViewChairID(v.wChairID)
		print("转换视图 ==> ", viewId)
		-- 发牌
		local cards = clone(v.cbCardData[1])
		GameLogic:SortCardList(cards)
		gameLayer._userCards[viewId] = clone(cards)
		gameLayer._gameView:GameVideoSendCard(cards,{gameLayer.cbUserMaxCardCount,gameLayer.cbUserMaxCardCount,gameLayer.cbUserMaxCardCount},viewId)
		
		if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
			if 1 == msgTable.nGameRound then
				PriRoom:getInstance().m_tabPriData.lIniScore = v.lScore
			end
		end
	end
	
	print("缓存 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
	GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	
	--约战房刷新信息
	if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind)  then
		print("回放约战房")
		PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
		gameLayer._gameView._priView:onRefreshInfo()
	end
	
	return 20, true
end

function GameVideoReplay:onGetVideoOutCard(gameLayer, msgTable, bForward, bBackward)
	if bBackward then
		print("快退处理")
		local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
		if not self:backwardCheck(msg, msgTable) then
			return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
		end
		-- 清理桌面
		gameLayer._gameView:onResetData()
		
		-- 更新玩家牌
		self:backwardUpdateUserCards(msg, gameLayer)
	else
		--存储数据
		local msg = {}
		msg.msgsub = msgTable.msgsub
		msg.userCards = {}
		-- 三个玩家牌
		self:cacheUserCards(msg, gameLayer)
		GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	end
	
	-- 视图转换
	local viewId = gameLayer:SwitchViewChairID(msgTable.wOutCardUser)
	
	local outCards = {}
	local serverOut = msgTable.bCardData[1]
	for i = 1, msgTable.bCardCount do
		table.insert(outCards, serverOut[i])
	end
	GameLogic:SortCardList(outCards)
	gameLayer._gameView:gameOutCard(viewId, outCards)
	GameLogic:RemoveCard(outCards, gameLayer._userCards[viewId])
	gameLayer._gameView:removeOutCard(gameLayer:SwitchViewChairID(msgTable.wCurrentUser))
	
	-- 随机速率
	local speed = math.random(5, 20)
	return speed, true
end

function GameVideoReplay:onGetVideoPassCard(gameLayer, msgTable, bForward, bBackward)
	if bBackward then
		print("快退处理")
		local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
		if not self:backwardCheck(msg, msgTable) then
			return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
		end
		
		-- 清理桌面
		gameLayer._gameView:onResetData()
		
		-- 更新玩家牌
		self:backwardUpdateUserCards(msg, gameLayer)
	else
		-- 存储数据
		local msg = {}
		msg.msgsub = msgTable.msgsub
		msg.userCards = {}
		-- 三个玩家牌
		self:cacheUserCards(msg, gameLayer)
		GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	end
	
	gameLayer._gameView:gamePassCard(gameLayer:SwitchViewChairID(msgTable.wPassUser))
	gameLayer.bNewTurn = msgTable.bNewTurn
	if gameLayer.bNewTurn == 1 then
		gameLayer.cbTurnCardData = {}
	end

	gameLayer._gameView:removeOutCard(gameLayer:SwitchViewChairID(msgTable.wCurrentUser))
	
	-- 随机速率
	local speed = math.random(5, 20)
	return speed, true
end

function GameVideoReplay:onGetVideoGameEnd(gameLayer, msgTable, bForward, bBackward)
	if bBackward then
		print("快退处理")
		local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
		if not self:backwardCheck(msg, msgTable) then
			return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
		end
		
		-- 清理桌面
		gameLayer._gameView:onResetData()
		
		-- 更新玩家牌
		self:backwardUpdateUserCards(msg, gameLayer)
	else
		-- 存储数据
		local msg = {}
		msg.msgsub = msgTable.msgsub
		msg.userCards = {}
		-- 三个玩家牌
		self:cacheUserCards(msg, gameLayer)
		
		GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	end
	
	--剩余牌显示
	local cardData = {}			--分发
	local index = 1
	for i = 1, 3 do
		cardData[i] = {}
		for j = 1, msgTable.bCardCount[1][i] do
			cardData[i][j] = msgTable.bCardData[1][index]
			index = index + 1
		end
	end
	local viewCardData = {}		--转换
	local viewCardCount = {}
	for i = 1, 3 do
		local viewId = gameLayer:SwitchViewChairID(i - 1)
		viewCardData[viewId] = cardData[i]
		viewCardCount[viewId] = msgTable.bCardCount[1][i]
		gameLayer._gameView:gameOutCard(viewId, viewCardData[viewId], true, true)
	end

	--结算框
	local result = {}
	local myChairId = gameLayer:GetMeChairID()
	local bMeWin = false
	for i = 1, 3 do
		result[i] = {}
		result[i].lScore = msgTable.lGameScore[1][i]
		if i - 1 == myChairId and result[i].lScore > 0 then
			bMeWin = true
		end
		local item = gameLayer:getUserInfoByChairID(i-1)
		result[i].userItem = item
	end
	--gameLayer._gameView._resultLayer:setResult(result, bMeWin)

	gameLayer.bNewTurn = true
	gameLayer.wTimeOutCount = 0
	gameLayer.bTrustee = {}
    gameLayer.cbCardData = {}
    gameLayer.cbTurnCardData = {}
	gameLayer._gameView:gameEnded()
	
	return 50 , true
end

function GameVideoReplay:onGetVideoTrustTee(gameLayer, msgTable, bForward, bBackward)
	if bBackward then
		print("快退处理")
		local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
		if not self:backwardCheck(msg, msgTable) then
			return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
		end
	else
		GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	end
	return 10 , true
end

-- 回放数据记录
function GameVideoReplay:onGetVideoGameRecord(gameLayer, msgTable, bForward, bBackward)
	local bNoDelay = (bBackward or bForward)
	if bBackward then
		print("快退处理")
		local msg = GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx]
		if not self:backwardCheck(msg, msgTable) then
			return nil, true, "回放处理异常 CODE" .. GameVideo.ERRORCODE.CODE_CACHE_ERROR
		end
	else
		-- 存储数据
		local msg = {}
		msg.msgsub = msgTable.msgsub
		
		GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	end
	
	gameLayer.m_userRecord = {}
	local nInningsCount = msgTable.nCount
end

function GameVideoReplay:onGetPrivateRoundEnd( gameLayer, msgTable, bForward, bBackward )
	
	local scorelist = msgTable.lScore[1]
	dump(scorelist, "the score list")
	gameLayer._gameView._priView:onPriGameEnd(msgTable)
	
	return 20, false
end

-- 回退数据检查
function GameVideoReplay:backwardCheck( msg, msgTab )
	if type(msg) ~= "table" then
		print("回退消息包异常")
		return false, "回退消息包异常"
	end
	-- 消息不一致
	if msg.msgsub ~= msgTab.msgsub then
		print("回退消息包异常, 消息不一致")
		return false, "回退消息包异常"
	end
	
	-- 玩家牌
	local usercards = msg.userCards
	if type(usercards) ~= "table" then
		print("用户牌错误")
		return false, "用户牌错误"
	end
	return true
end

-- 回退更新牌
function GameVideoReplay:backwardUpdateUserCards( msg, gameLayer )
	
	--刷新手牌
	local cardNode = gameLayer._gameView._handCardLayer.nodeCard[cmd.MY_VIEWID] 
	for j = 1,16 do
		local card = cardNode:getChildByTag(j)
		card:setVisible(false)
	end
	for i = 1,#msg.userCards[cmd.MY_VIEWID] do
		table.insert(gameLayer._gameView._handCardLayer.cbCardData, msg.userCards[i])
	end
	
	for i = 1,cmd.GAME_PLAYER  do
		local viewId = gameLayer:SwitchViewChairID(i-1)
		gameLayer._userCards[viewId] =  clone(msg.userCards[i])
		
		--玩家剩余牌数
		for i = 1, 3 do
			gameLayer._gameView.nRemainCard[i] = #msg.userCards[viewId]
			local remainCard = gameLayer._gameView.nodePlayer[i]:getChildByName("Text_remainCard")
			if remainCard then
				remainCard:setString(""..gameLayer._gameView.nRemainCard[i])
				remainCard:setVisible(true)
			end
		end
		
		gameLayer._gameView._handCardLayer:setVideoHandCard(msg.userCards[i],#msg.userCards[i],viewId)
	end
end

-- 存储玩家牌
function GameVideoReplay:cacheUserCards(msg, gameLayer, isForward)
	-- 三个玩家牌
	for i = 1,cmd.GAME_PLAYER  do
		-- 视图转换
		local viewId = gameLayer:SwitchViewChairID(i-1)
		msg.userCards[i] = clone(gameLayer._userCards[viewId])
	end
end

return GameVideoReplay
