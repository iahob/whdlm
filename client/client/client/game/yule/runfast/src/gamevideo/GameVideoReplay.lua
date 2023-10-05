--
-- 录像处理

-- 游戏录像处理
local VideoReplayModel = appdf.req(GameVideo.MODULE.PLAZAMODULE .."VideoReplayModel")
local GameVideoReplay = class("GameVideoReplay", VideoReplayModel)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local module_pre = "game.yule.runfast.src"
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
	content = "跑得快牌局回放,跑得快游戏精彩刺激，一起来玩吧！",
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
				{ k = "lBaseScore",t = "score"},                                    --基础积分
	                		{k = "cbPlayMode", t = "byte"},                          --游戏模式，0为积分房卡，1为金币房卡
				{k = "wPlayerCount",t="word"},--游戏人数
				{k = "wPlayerCountRule",t="word"},  --游戏规则 2-6人为0，其余为人数值
				{k = "wBankerUser",t="word"},   --庄家用户
				{k = "wBeforWiner",t="word"},   --上局赢家
				{k = "wCurrentUser",t="word"},  --当前玩家
				{k = "cbCardData",t="byte",l={16}},--扑克列表
				{k = "bSixTheenCard",t="bool"}, --游戏模式 (16 or 15)
				{k = "bNoForceClose",t="bool"}, --不可强关
				{k = "bNoRemoveBomb",t="bool"}, --炸弹不可拆
				{k = "bSpadeBanker",t="bool"},  --黑桃3庄
				{k = "cbSpadeFirstOutCard", t = "byte"},--黑桃3庄时首出的牌
 				{k = "cbBankerFirstOutCard", t = "byte"},--赢家庄时，第一局首出的牌
				{k = "bFirstOutSpade",t="bool"},--首出必带黑桃3
				{k = "bForceOutCard", t = "bool"},--有大必出
	 			{k = "bEchelonConclude", t = "bool"},--梯度结算
				{k = "szNickName", t = "string", s = 32},                   --用户昵称
				{k = "wChairID",t="word"}, --用户椅子号
				{k = "lScore",t="score"}    --用户分数
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
			cmdtable.msgsub = 105
			return cmdtable
		elseif cmd.SUB_S_AUTOMATISM == sub then
			local cmdtable = ExternalFun.read_netdata(cmd.CMD_S_RoomCardRecord, pBuffer)
			cmdtable.msgsub = 116
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
	elseif 105 == sub then
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
	--assert(#msgTable.startinfo == cmd.GAME_PLAYER)

	for i = 1, cmd.GAME_PLAYER   do

        		if i ~= cmd.MY_VIEWID then
            		gameLayer._gameView.nodePlayer[i]:setVisible(false)
        		end
    	end
	
	local myCard = {}
	for k,v in pairs(msgTable.startinfo) do
		-- 视图转换

		gameLayer.wPlayerCount = v.wPlayerCount

		local viewId = gameLayer:SwitchViewChairID(v.wChairID)
		print("转换视图 ==> ", viewId)
		-- 发牌
		local cards = {}--clone(v.cbCardData[1])

		for i=1,gameLayer.cbUserMaxCardCount do
			cards[i] = v.cbCardData[1][i]
		end

		GameLogic:SortCardList(cards)
		gameLayer._userCards[viewId] = clone(cards)
		gameLayer._gameView._cardLayer.allPlayerCard[viewId] = clone(cards)

		if viewId == cmd.MY_VIEWID then
			myCard = clone(cards)
		else
			gameLayer._gameView:OnUpdateUser(viewId,gameLayer.m_tabUserItem[v.wChairID+1])
		end
		
		-- if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
		-- 	if 1 == msgTable.nGameRound then
		-- 		PriRoom:getInstance().m_tabPriData.lIniScore = v.lScore
		-- 	end
		-- end
		gameLayer.cbUserMaxCardCount = v.bSixTheenCard and 16 or 15
		gameLayer.bNoForceClose = v.bNoForceClose --不可强关
		gameLayer.lBaseScore = v.lBaseScore
		gameLayer.bSpadeBanker   = v.bSpadeBanker
		gameLayer.bForceOutCard = v.bForceOutCard
		gameLayer.bEchelonConclude = v.bEchelonConclude

		if gameLayer.bSpadeBanker then
			if v.cbSpadeFirstOutCard ~= yl.INVALID_BYTE then
				gameLayer.cbFirstOutCard = v.cbSpadeFirstOutCard 
				gameLayer.bFirstOutSpade = true
			else
				gameLayer.bFirstOutSpade = false
			end
		else
			if v.cbBankerFirstOutCard ~= yl.INVALID_BYTE then
				gameLayer.cbFirstOutCard = v.cbBankerFirstOutCard 
				gameLayer.bFirstOutSpade = true
			else
				gameLayer.bFirstOutSpade = false
			end
		end

		
		gameLayer.wCurrentUser = v.wCurrentUser

		gameLayer.cbPlayMode = v.cbPlayMode
    		GlobalUserItem.bPrivateGoldRoom = gameLayer.cbPlayMode == 1  or false

	end

	for i = 1, gameLayer.wPlayerCount do
		msg.userCards[i] = clone(msgTable.startinfo[i].cbCardData[1])
	end

	gameLayer._gameView._cardLayer:showSendCardAni(myCard,{gameLayer.cbUserMaxCardCount,gameLayer.cbUserMaxCardCount,gameLayer.cbUserMaxCardCount,gameLayer.cbUserMaxCardCount},true)
	
	print("缓存 索引==>" .. GameVideo:getInstance().m_nMessageIdx)
	GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	
	--gameLayer._gameView:setGameMultiple(gameLayer.lBaseScore)

	--约战房刷新信息
	--if msgTable.bIsPrivateRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind)  then
		--print("回放约战房")
		--PriRoom:getInstance().m_tabPriData.dwPlayCount = msgTable.nGameRound
		--gameLayer._gameView._priView:onRefreshInfo()
	--end
	
	return 35, true
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
		self:cacheUserCards(msg, gameLayer,bForward)
		GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	end
	
	-- 视图转换
	local viewId = gameLayer:SwitchViewChairID(msgTable.wOutCardUser)
	gameLayer.wCurrentUser = msgTable.wCurrentUser
	
	local outCards = {}
	local serverOut = msgTable.bCardData[1]
	for i = 1, msgTable.bCardCount do
		table.insert(outCards, serverOut[i])
	end
	

	if gameLayer.bNewTurn and gameLayer.wPlayerCount == 3 then

		for i=1,3 do
			if i ~= viewId then
				gameLayer._gameView._cardLayer:removeOutCard(i)
			end
		end
	end

	gameLayer.bNewTurn = false

	GameLogic:SortCardList(outCards)
	GameLogic:RemoveCard(outCards, gameLayer._userCards[viewId])
	gameLayer._gameView._cardLayer.allPlayerCard[viewId] =clone(gameLayer._userCards[viewId])

	gameLayer._gameView._cardLayer:gameOutCard(viewId, outCards)
	
	
	if gameLayer.wCurrentUser ~= yl.INVALID_CHAIR then
		gameLayer._gameView._cardLayer:removeOutCard(gameLayer:SwitchViewChairID(gameLayer.wCurrentUser))
	end

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
		self:cacheUserCards(msg, gameLayer,bForward)
		GameVideo:getInstance().m_tabStepVideoMessage[GameVideo:getInstance().m_nMessageIdx] = msg
	end
	
	
	gameLayer.bNewTurn = msgTable.bNewTurn
	gameLayer.wCurrentUser = msgTable.wCurrentUser

	gameLayer._gameView:gamePassCard(gameLayer:SwitchViewChairID(msgTable.wPassUser))


	if gameLayer.bNewTurn then
		gameLayer.cbTurnCardData = {}
	end

	gameLayer._gameView._cardLayer:removeOutCard(gameLayer:SwitchViewChairID(msgTable.wCurrentUser))
	

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
		self:cacheUserCards(msg, gameLayer,bForward)
		
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
	for i = 1, gameLayer.wPlayerCount do
		local viewId = gameLayer:SwitchViewChairID(i - 1)
		viewCardData[viewId] = cardData[i]
		viewCardCount[viewId] = msgTable.bCardCount[1][i]
		--gameLayer._gameView:gameOutCard(viewId, viewCardData[viewId], true, true)
		if viewId ~= cmd.MY_VIEWID then
			if gameLayer.wPlayerCount == 2 and i<=2 then
				gameLayer._gameView._cardLayer:sortOtherPlayerHandCard(viewId,#viewCardData[viewId],viewCardData[viewId])
			else
				gameLayer._gameView._cardLayer:gameOutCard(viewId, viewCardData[viewId], true, true)
			end
		end
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
		
		result[i].userItem = gameLayer.m_tabUserItem[i]
		result[i].count = msgTable.bCardCount[1][i]
		result[i].boom = msgTable.bBombCount[1][i]
	end
	--gameLayer._gameView._resultLayer:setResult(result, bMeWin)
	gameLayer._gameView._resultLayer:setResult(result, bMeWin,gameLayer._MyChairID)

	gameLayer._gameView:gameEnded()

	gameLayer.bNewTurn = true
	gameLayer.wTimeOutCount = 0
	gameLayer.bTrustee = {}
    	gameLayer.cbCardData = {}
    	gameLayer.cbTurnCardData = {}
	
	return 50 , false
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
	
	 
	local count = {0,0,0,0}
	for i=1,gameLayer.wPlayerCount do

		local viewid = gameLayer:SwitchViewChairID(i-1)
		count[viewid] = #msg.userCards[i]
		gameLayer._gameView._cardLayer.allPlayerCard[viewid] = clone(msg.userCards[i])

		gameLayer._userCards[viewid] =  clone(msg.userCards[i])
	end

	local myChairId = gameLayer:GetMeChairID()

	gameLayer._gameView._cardLayer:showSendCardAni(gameLayer._userCards[cmd.MY_VIEWID],count)

	
end

-- 存储玩家牌
function GameVideoReplay:cacheUserCards(msg, gameLayer, isForward)
	-- 三个玩家牌
	for i = 1,gameLayer.wPlayerCount  do
		-- 视图转换
		local viewId = gameLayer:SwitchViewChairID(i-1)
		msg.userCards[i] = gameLayer._userCards[viewId]
	end
	if isForward then
		gameLayer:stopAllActions()
		gameLayer._gameView:stopAllActions()
		gameLayer._gameView:onResetData()

		local count = {0,0,0,0}
		for i=1,gameLayer.wPlayerCount do

			local viewid = gameLayer:SwitchViewChairID(i-1)
			count[viewid] = #msg.userCards[i]
			gameLayer._gameView._cardLayer.allPlayerCard[viewid] = clone(msg.userCards[i])

			gameLayer._userCards[viewid] =  clone(msg.userCards[i])
		end
		print("==================6666666666666")

		gameLayer._gameView._cardLayer:showSendCardAni(gameLayer._userCards[cmd.MY_VIEWID],count)

	end
end

return GameVideoReplay
