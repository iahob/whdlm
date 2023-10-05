local CardLayer = class("CardLayer", function(scene)
	local cardLayer = display.newLayer()
	return cardLayer
end)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.sparrowgdex.src.models.GameLogic")
local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowgdex.src.models.CMD_Game")

--桌面麻将布局参数  布局格式  自己为1  右边为2 左边为3 对面为4
--手牌
local posHandCard = {cc.p(62, 74), cc.p(1180, 180), cc.p(200, 630), cc.p(950, 665)}
local anchorPointHandCard = {cc.p(0, 0), cc.p(0, 0), cc.p(1, 0), cc.p(0, 1)} --方向
local pramHandCard = {{88, 136, 88, 0}, {28, 75, 0, 31}, {28, 75, 0, -31},{43, 68, -43, 0}} --宽 高 x方向偏移量 y方向偏移量
local zorderCard = {1, -1, 1, -1}

--出牌
local posOutcard = {cc.p(400, 210), cc.p(1105, 212), cc.p(260, 600), cc.p(900, 550)}
local pramOutCard = {{44, 67, 44, 52}, {55, 47, -55, 32}, {55, 47, 55, -32},{44, 67, -44, -52}} --宽 高 x方向偏移量 y方向偏移量

local numPerLine = 10    --打出的牌，每行多少个

--碰牌，杠牌
local posActiveCard = {cc.p(1285, 62), cc.p(1180, 630), cc.p(200, 170), cc.p(340, 665)}
local anchorPointActiveCard = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}}--方向
local pramActiveCard = {{44, 67, -44, 0}, {55, 47, 0, -32}, {55, 47, 0, 32},{44, 67, 44, 0}} --宽 高 x方向偏移量 y方向偏移量
local offsetPer = 10  --每个碰杠牌之间间隔

--local cbCardData = {1, 5, 7, 6, 34, 12, 32, 25, 18, 19, 27, 22, 33, 33}

CardLayer.TAG_BUMPORBRIDGE = 1
CardLayer.TAG_CARD_FONT = 1
CardLayer.TAG_LISTEN_FLAG = 2

CardLayer.ENUM_CARD_NORMAL = nil
CardLayer.ENUM_CARD_POPUP = 1
CardLayer.ENUM_CARD_MOVING = 2
CardLayer.ENUM_CARD_OUT = 3

CardLayer.Z_ORDER_TOP = 50


--创建立着的手牌
function CardLayer:createStandCardSprite(viewId, value, isMagic)
	local resCsb = {}
	if self._scene.bIsVideo then
		resCsb = 	
		{
			cmd.RES_PATH.."card/Node_majong_my.csb",
			cmd.RES_PATH.."card/Node_majong_right_down.csb",
			cmd.RES_PATH.."card/Node_majong_left_down.csb",
			cmd.RES_PATH.."card/Node_majong_top_dowm.csb",
		}
	else
		resCsb = 	
		{
			cmd.RES_PATH.."card/Node_majong_my.csb",
			cmd.RES_PATH.."card/Node_majong_right.csb",
			cmd.RES_PATH.."card/Node_majong_left.csb",
			cmd.RES_PATH.."card/Node_majong_top.csb",
		}
	end

	local resValue = 
	{
		cmd.RES_PATH.."card/my_big/tile_me_up_",
		cmd.RES_PATH.."card/left_right/tile_leftRight_",
		cmd.RES_PATH.."card/left_right/tile_leftRight_",
		cmd.RES_PATH.."card/my_small/tile_meUp_",
	}

	local card =  cc.CSLoader:createNode(resCsb[viewId])
	card:setContentSize(cc.size(pramHandCard[viewId][1], pramHandCard[viewId][2]))
	--print("我的麻将尺寸", card:getContentSize().width, card:getContentSize().height)
	--print("创建立着的麻将,玩家位置", viewId)
	if nil ~= value then --其他玩家的牌不处理
		--获取数值
		--print("创建立着的麻将", value)
		local cardIndex = GameLogic.SwitchToCardIndex(value)
		local sprPath = resValue[viewId]
		if cardIndex < 10 then
			sprPath = sprPath..string.format("0%d", cardIndex)..".png"
		else
			sprPath = sprPath..string.format("%d", cardIndex)..".png"
		end
		local spriteValue = display.newSprite(sprPath)
		--获取精灵
		local sprCard = card:getChildByName("card_value")
		if nil ~= sprCard then
			sprCard:setSpriteFrame(spriteValue:getSpriteFrame())
		end

		local card_bg = card:getChildByName("card_bg")
		if nil ~= card_bg then
			if self._scene._scene.m_bLookOn then
				sprCard:setVisible(false)
				local spriteValue1 = display.newSprite(cmd.RES_PATH.."card/back_big.png")
				card_bg:setSpriteFrame(spriteValue1:getSpriteFrame())
			end
		end

		local sprFlag = card:getChildByName("flag")
		if nil ~=  sprFlag and nil ~= value and viewId == cmd.MY_VIEWID and not self._scene._scene.m_bLookOn then --只有自己的手牌才显示癞子
			sprFlag:setVisible(self:isMagicCard(value))
		end
	end
	return card
end

--创建非自己的碰杠牌和打出去的牌
function CardLayer:createOutOrActiveCardSprite(viewId, value, isMagic)
	--print("创建打出的手牌",viewId, value, isMagic)
	local resCsb = 
	{
		cmd.RES_PATH.."card/Node_majong_my_downsmall.csb",
		cmd.RES_PATH.."card/Node_majong_right_down.csb",
		cmd.RES_PATH.."card/Node_majong_left_down.csb",
		cmd.RES_PATH.."card/Node_majong_top_dowm.csb",
	}
	local resValue = 
	{
		cmd.RES_PATH.."card/my_small/tile_meUp_",
		cmd.RES_PATH.."card/left_right/tile_leftRight_",
		cmd.RES_PATH.."card/left_right/tile_leftRight_",
		cmd.RES_PATH.."card/my_small/tile_meUp_",
	}
	local card =  cc.CSLoader:createNode(resCsb[viewId])
	card:setContentSize(cc.size(pramOutCard[1], pramOutCard[2]))
	card:setAnchorPoint(cc.p(0.5, 0.5))
	--获取数值	
	local cardIndex = GameLogic.SwitchToCardIndex(value)
	local sprPath = resValue[viewId]
	if cardIndex < 10 then
		sprPath = sprPath..string.format("0%d", cardIndex)..".png"
	else
		sprPath = sprPath..string.format("%d", cardIndex)..".png"
	end
	local spriteValue = display.newSprite(sprPath)
	--获取精灵
	local sprCard = card:getChildByName("card_value")
	if nil ~= sprCard then
		sprCard:setSpriteFrame(spriteValue:getSpriteFrame())
	end
	local sprFlag = card:getChildByName("flag")
	if nil ~=  sprFlag then
		sprFlag:setVisible(self:isMagicCard(value))
	end
	return card
end
--创建我的碰杠牌（我的碰杠牌比较大）
function CardLayer:createMyActiveCardSprite(value, isMagic)
	local card =  cc.CSLoader:createNode(cmd.RES_PATH.."card/Node_majong_my_downnormal.csb")
	--获取数值
	local cardIndex = GameLogic.SwitchToCardIndex(value)
	local sprPath = cmd.RES_PATH.."card/my_normal/tile_me_"
	if cardIndex < 10 then
		sprPath = sprPath..string.format("0%d", cardIndex)..".png"
	else
		sprPath = sprPath..string.format("%d", cardIndex)..".png"
	end
	local spriteValue = display.newSprite(sprPath)
	--获取精灵
	local sprCard = card:getChildByName("card_value")
	if nil ~= sprCard then
		sprCard:setSpriteFrame(spriteValue:getSpriteFrame())
	end

	local sprFlag = card:getChildByName("flag")
	if nil ~=  sprFlag then
		sprFlag:setVisible(self:isMagicCard(value))
	end
	return card
end

--碰杠牌显示背面
function CardLayer:showActiveCardBack( viewid, card )
	if nil ~= card then
		--获取精灵
		local sprCard = card:getChildByName("card_value")
		if nil ~= sprCard then
			sprCard:setVisible(false)
		end

		local sprFlag = card:getChildByName("flag")
		if nil ~=  sprFlag then
			sprFlag:setVisible(false)
		end
		local sprPath = 
		{
			cmd.RES_PATH.."card/back_small.png",
			cmd.RES_PATH.."card/left_right_back.png",
			cmd.RES_PATH.."card/left_right_back.png",
			cmd.RES_PATH.."card/back_small.png",
		}
		local spriteBg = display.newSprite(sprPath[viewid])
		local sprCardBg = card:getChildByName("card_bg")
		if nil ~= sprCardBg then
			sprCardBg:setSpriteFrame(spriteBg:getSpriteFrame())
		end
		return card
	end
	return nil
end

--碰杠牌显示正面
function CardLayer:tileActiveCard(viewid)
	local node = self.nodeActiveCard[viewid]
	if nil ~= node then
		local allChild = node:getChildren()
		for k, v in pairs(allChild) do
			--获取精灵
			local sprCard = v:getChildByName("card_value")
			if nil ~= sprCard then
				sprCard:setVisible(true)
			end
			local sprFlag = v:getChildByName("flag")
			if nil ~=  sprFlag then
				local tag = v:getTag()
				local index = math.floor(tag/10) +1
				dump(self.cbActiveCardData[viewid], "获取碰杠牌数据", 10)
				print("获取碰杠牌数据", tag)
				if index < #self.cbActiveCardData[viewid] then
					local value = self.cbActiveCardData[viewid][index].cbCardValue[1]
					sprFlag:setVisible(self:isMagicCard(value))
				end
				--sprFlag:setVisible(true)
			end
			local sprPath = 
			{
				cmd.RES_PATH.."card/dowm_small.png",
				cmd.RES_PATH.."card/left_right_down.png",
				cmd.RES_PATH.."card/left_right_down.png",
				cmd.RES_PATH.."card/dowm_small.png",
			}
			local spriteBg = display.newSprite(sprPath[viewid])
			local sprCardBg = v:getChildByName("card_bg")
			if nil ~= sprCardBg then
				sprCardBg:setSpriteFrame(spriteBg:getSpriteFrame())
			end
		end
	end
end

--我的麻将倒下
function CardLayer:createMyTileCardSprite(value, isMagic)
	local card =  cc.CSLoader:createNode(cmd.RES_PATH.."card/Node_majong_my_downbig.csb")
	--获取数值
	local cardIndex = GameLogic.SwitchToCardIndex(value)
	local sprPath = cmd.RES_PATH.."card/my_big/tile_me_up_"
	if cardIndex < 10 then
		sprPath = sprPath..string.format("0%d", cardIndex)..".png"
	else
		sprPath = sprPath..string.format("%d", cardIndex)..".png"
	end
	local spriteValue = display.newSprite(sprPath)
	--获取精灵
	local sprCard = card:getChildByName("card_value")
	if nil ~= sprCard then
		sprCard:setSpriteFrame(spriteValue:getSpriteFrame())
	end

	local sprFlag = card:getChildByName("flag")
	if nil ~=  sprFlag then
		sprFlag:setVisible(self:isMagicCard(value))
	end
	return card
end

function CardLayer:onInitData()
	--body
	math.randomseed(os.time())
	self.cbCardData = {{}, {}, {}, {}}
	self.cbCardCount = {0, 0, 0, 0}
	self.cbOutcardCount = {0, 0, 0, 0}

	self.cbActiveCardData = {{}, {}, {}, {}}
	self.isTouchEnable = false -- 设置自己是否可以触摸
	self.nCurrentTouchCardTag = 0 --touchBegin选中的麻将，判断按下和离开是不是同一张麻将
	self.selectTag = 0 --选中的卡牌的tag
	self.isMoving = false --是否移动出牌
	self.beginPoint = nil
	self.isSelectCardModel = cmd.SELECT_CARD_NIL --选派模式（有多个杠时候，可以选择杠哪个）
	self.cbSelectData ={}  --可选数据

	self.cbMagicData ={}

	--玩家要听牌可以打的牌
	self.cbListenList = {}
	--玩家打完听牌可以胡的牌 14个
	self.cbHuCard = {}
	--玩家打完听牌可以胡的牌番数
	self.cbHuCardFan = {}
	--玩家打完听牌可以胡的牌剩余张数sort
	self.cbHuCardLeft = {}
end

function CardLayer:onResetData()
	--body
	self:stopAllActions()
	for i = 1, cmd.GAME_PLAYER do
		self.cbCardCount[i] = cmd.MAX_COUNT
		self.nodeOutcard[i]:removeAllChildren()
		self.nodeActiveCard[i]:removeAllChildren()
		self.nodeHandCard[i]:removeAllChildren()
		self:removeChildByTag(i *10086)
	end
	self.cbCardData = {{}, {}, {}, {}}
	
	self.cbCardCount = {0, 0, 0, 0}
	self.cbOutcardCount = {0, 0, 0, 0}


	self.cbActiveCardData = {{}, {}, {}, {}}
	self.isTouchEnable = false
	self.nCurrentTouchCardTag = 0
	self.selectTag = 0
	self.cbMagicData ={}
	self.isSelectCardModel = cmd.SELECT_CARD_NIL
	self.cbSelectData ={}  --可选数据
	self.isMoving = false --是否移动出牌
	self.beginPoint = nil

	--玩家要听牌可以打的牌
	self.cbListenList = {}
	--玩家打完听牌可以胡的牌 14个
	self.cbHuCard = {}
	--玩家打完听牌可以胡的牌番数
	self.cbHuCardFan = {}
	--玩家打完听牌可以胡的牌剩余张数
	self.cbHuCardLeft = {}
	-- 玩家出的牌
    self.showCards = {{},{},{},{}}
    self.markCards = {}
end

function CardLayer:ctor(scene)
	self._scene = scene
	self:onInitData()

	ExternalFun.registerTouchEvent(self, true)
	--桌牌
	--self.nodeTableCard = self:createTableCard()
	--手牌
	self.nodeHandCard = self:createHandCardNode()
	--出牌
	self.nodeOutcard = self:createOutCardNode()
	dump(self.nodeOutcard, "创建打出的手牌node")
	--碰或杠牌
	self.nodeActiveCard = self:createActiveCardNode()
    self.showCards = {{},{},{},{}} -- 桌子上展示出来的牌
    self.markCards = {}
end


--设置自己是否可以触摸
function CardLayer:setMyCardTouchEnabled(enable)
	if self._scene._scene.m_bLookOn then
		return
	end
	self.isTouchEnable = enable
end

--初始化手牌NODE
function CardLayer:createHandCardNode()
	local nodeCard = {}
	for i = 1, cmd.GAME_PLAYER do
		nodeCard[i] = cc.Node:create()
		:move(posHandCard[i])
		:setAnchorPoint(anchorPointHandCard[i])
		:addTo(self, 5)
	end
	nodeCard[1]:setLocalZOrder(10)
	--nodeCard[3]:setLocalZOrder(10)
	return nodeCard
end

--创建立着的手牌，传入卡牌数据，和viewID
function CardLayer:createHandCard(viewid, cbCardData, cbCardCount)
	for i=1,cbCardCount do
		--创建麻将
		local card = nil
		if nil ~= cbCardData then
			card = self:createStandCardSprite(viewid, cbCardData[i], false)
		else
			card = self:createStandCardSprite(viewid, nil, false)
		end
		
		--添加到麻将node
		local numActive = #self.cbActiveCardData[viewid]
		self.nodeHandCard[viewid]:addChild(card, (self.cbCardCount[viewid] +numActive*3)* zorderCard[viewid])
		--设置位置 基础位置加上偏移量乘已经绘制的卡牌个数(如果是一张牌，那么是游戏中发牌，麻将与手牌中间有间隔)
		card:setPosition(cc.p(self.cbCardCount[viewid] * pramHandCard[viewid][3], self.cbCardCount[viewid] * pramHandCard[viewid][4]))

		if (13 - numActive*3) == self.cbCardCount[viewid] then --如果是发牌给玩家，偏离一点点
			card:setPosition(self.cbCardCount[viewid] *pramHandCard[viewid][3] + pramHandCard[viewid][3]/2 , self.cbCardCount[viewid] * pramHandCard[viewid][4] + pramHandCard[viewid][4]/2 )
		end
		--print("添加麻将到node", viewid, self.cbCardCount[viewid] +1, card:getPositionX(), card:getPositionY())
		self.cbCardCount[viewid] = self.cbCardCount[viewid] + 1 --个数加1
		card:setTag(self.cbCardCount[viewid])

		--卡牌添加到当前卡牌数组
		if nil ~= cbCardData then
			self.cbCardData[viewid][self.cbCardCount[viewid]] = cbCardData[i]
		else
			self.cbCardData[viewid][self.cbCardCount[viewid]] = nil
		end
	end
end

--手牌排序
function CardLayer:sortHandCard(viewid)
	--print("排序索引", viewid)
	--先将卡牌从新紧密布局
	local index = 0; --实际索引
	for i=1, cmd.MAX_COUNT do
		local card = self.nodeHandCard[viewid]:getChildByTag(i)
		if nil ~=  card then
			card:setPosition(cc.p(index * pramHandCard[viewid][3], index * pramHandCard[viewid][4]))
			--print("麻将重排位置",index, card:getPositionX(), card:getPositionY())
			card:setLocalZOrder(index *zorderCard[viewid])
			index = index +1
			card:setTag(index) --s索引从1开始
		end
	end

	if viewid == cmd.MY_VIEWID or self._scene.bIsVideo then --癞子排在最左边
		table.sort(self.cbCardData[viewid], function (a, b)
				if self:isMagicCard(a) and self:isMagicCard(b) then
					return a < b
				elseif self:isMagicCard(a) then
					return true
				elseif self:isMagicCard(b) then
					return false
				else
					return a < b
				end
		end)
		local resValue = 
		{
			cmd.RES_PATH.."card/my_big/tile_me_up_",
			cmd.RES_PATH.."card/left_right/tile_leftRight_",
			cmd.RES_PATH.."card/left_right/tile_leftRight_",
			cmd.RES_PATH.."card/my_small/tile_meUp_",
		}
		--重新设置麻将纹理
		--print("排序，卡牌数目", self.cbCardCount[viewid])
		dump(self.cbCardData[viewid], "排序，卡牌数据")
		for i=1, self.cbCardCount[viewid] do
			local card = self.nodeHandCard[viewid]:getChildByTag(i)
			if nil ~=  card then
				local sprPath = resValue[viewid]
				--print("排序，卡牌数值", self.cbCardData[viewid][i])
				local cardIndex = GameLogic.SwitchToCardIndex(self.cbCardData[viewid][i])
				if cardIndex < 10 then
					sprPath = sprPath..string.format("0%d", cardIndex)..".png"
				else
					sprPath = sprPath..string.format("%d", cardIndex)..".png"
				end

				local spriteValue = display.newSprite(sprPath)
				--获取麻将值纹理
				local cardValue = card:getChildByName("card_value")
				if nil ~= cardValue then
					cardValue:setSpriteFrame(spriteValue:getSpriteFrame())
					cardValue:setVisible(true)
				end

				local card_bg = card:getChildByName("card_bg")
				if nil ~= card_bg then
					if self._scene._scene.m_bLookOn then
						cardValue:setVisible(false)
						local spriteValue1 = display.newSprite(cmd.RES_PATH.."card/back_big.png")
						card_bg:setSpriteFrame(spriteValue1:getSpriteFrame())
					end
				end

				--设置癞子
				local sprFlag = card:getChildByName("flag")
				if nil ~=  sprFlag and not self._scene._scene.m_bLookOn then
					sprFlag:setVisible(self:isMagicCard(self.cbCardData[viewid][i]))
				end

				--恢复麻将颜色
				card:setColor(cc.c3b(255,255,255))
			end
		end
	end
	-- 如果是碰最后一张偏移表示预出牌
	print("self.cbCardCount[viewid] === ",self.cbCardCount[viewid])
	if self.cbCardCount[viewid] % 3 == 2 then
		local card = self.nodeHandCard[viewid]:getChildByTag(self.cbCardCount[viewid])
		card:setPosition((self.cbCardCount[viewid] - 1) *pramHandCard[viewid][3] + pramHandCard[viewid][3]/2 , self.cbCardCount[viewid] * pramHandCard[viewid][4] + pramHandCard[viewid][4]/2 )
	end
end

--创建打出的手牌node
function CardLayer:createOutCardNode()
	local nodeCard = {}
	for i = 1, cmd.GAME_PLAYER do
		nodeCard[i] = cc.Node:create()
			:move(posOutcard[i])
			:addTo(self)
	end
	nodeCard[1]:setLocalZOrder(2)
	return nodeCard
end

--创建打出的手牌
function CardLayer:createOutCard(viewid, cbCardData, cbCardCount)
	print("创建打出的麻将", viewid,cbCardCount,cbCardData )
	local zorder = {-1, -1, 1, 1}  --排数的层级关系
	for i=1,cbCardCount do
		--获取已经打出去的麻将
		local numOutCard = self.cbOutcardCount[viewid]
		--计算本次应该打出的麻将的起始位置（正常只有一张，重连是全部的）
		local offsetX, offsetY = 0,0
		if cmd.MY_VIEWID == viewid or cmd.TOP_VIEWID ==  viewid then --我和对面
			offsetX = math.mod(numOutCard, numPerLine)
			offsetY = math.floor(numOutCard/numPerLine)
		elseif cmd.LEFT_VIEWID == viewid or cmd.RIGHT_VIEWID == viewid then
			offsetX = math.floor(numOutCard/numPerLine)
			offsetY = math.mod(numOutCard, numPerLine)
		end
		local pos  = cc.p(pramOutCard[viewid][3]* offsetX, pramOutCard[viewid][4]* offsetY)
		--创建麻将
		local card = self:createOutOrActiveCardSprite(viewid, cbCardData[i], false)
	    table.insert(self.showCards[viewid], {sp = card, value = cbCardData[i]})
		--添加到麻将node
		--print("创建打出的麻将", pos.x, pos.y,offsetX, offsetY)
		self.nodeOutcard[viewid]:addChild(card, offsetX* zorderCard[viewid] + offsetY* zorder[viewid])
		--设置位置 基础位置加上偏移量乘已经绘制的卡牌个数
		card:setPosition(pos)
		
		--数目更新，设置TAG
		self.cbOutcardCount[viewid] = self.cbOutcardCount[viewid] +1
		card:setTag(self.cbOutcardCount[viewid])
	end
end

--创建碰杠牌的node
function CardLayer:createActiveCardNode()
	local nodeCard = {}
	for i = 1, cmd.GAME_PLAYER do
		nodeCard[i] = cc.Node:create()
			:move(posActiveCard[i])
			:addTo(self)
	end
	nodeCard[1]:setLocalZOrder(2)
	nodeCard[3]:setLocalZOrder(6)

	return nodeCard
end

--创建碰杠牌
function CardLayer:createActiveCard(viewid, tagAvtiveCard, wProvideViewId) --传入tagAvtiveCard
	--先判断类型
	local jsonStr = cjson.encode(tagAvtiveCard)
    LogAsset:getInstance():logData(jsonStr, true)
    dump(tagAvtiveCard,"CardLayer:createActiveCard".."#"..viewid.."#"..wProvideViewId)
	if tagAvtiveCard.cbType == GameLogic.SHOW_MING_GANG then --明杠已经碰过，只添加一张
		--获取位置，只添加一张
		local index = 0
		dump(self.cbActiveCardData[viewid],"我的碰杠牌")
		for i=1,#self.cbActiveCardData[viewid] do
			--查找是否已经碰
			local activeInfo = self.cbActiveCardData[viewid][i]
			if activeInfo.cbCardValue[1] ==  tagAvtiveCard.cbCardValue[1] then --有碰
				index = i
				break
			end
		end
		print("有没有找到",index)
		assert(index ~= 0,"没找到之前的碰牌")
		--计算开始位置(每墩3个，杠牌有一张放在上面)
		local pos = cc.p((pramActiveCard[viewid][3]*3 +anchorPointActiveCard[viewid][1] *offsetPer)*(index -1), (pramActiveCard[viewid][4]*3 +anchorPointActiveCard[viewid][2] *10) *(index -1))
		 --local card = nil--self:createOutOrActiveCardSprite(viewid, tagAvtiveCard.cbCardValue[1], false)
		 local card = self:createOutOrActiveCardSprite(viewid, tagAvtiveCard.cbCardValue[1], false)
		
		if nil ~= card then
			self.nodeActiveCard[viewid]:addChild(card, 4 + index *3)
			card:setTag((index -1)* 10 + 4) --设置索引，方便明杠时候移除
			local psoCard = cc.p(pos.x + pramActiveCard[viewid][3], 
				pos.y + (anchorPointActiveCard[viewid][2] == 1 and pramActiveCard[viewid][2] *math.abs(anchorPointActiveCard[viewid][2])
				or pramActiveCard[viewid][4] *2 *math.abs(anchorPointActiveCard[viewid][2]) + pramActiveCard[viewid][2] *math.abs(anchorPointActiveCard[viewid][2])) 
			+ math.abs(anchorPointActiveCard[viewid][1]) *pramActiveCard[viewid][2]/5)

			if cmd.TOP_VIEWID == viewid then
				psoCard.y = psoCard.y -5
			end

			card:setPosition(psoCard)
		end
		self.cbActiveCardData[viewid][index] = clone(tagAvtiveCard)
	else -- 其余牌型重新绘制
		--获取之前已经存在的牌堆数
		local numActive = #self.cbActiveCardData[viewid]
		--计算开始位置(每墩3个，杠牌有一张放在上面)
		local pos = cc.p((pramActiveCard[viewid][3]*3 +anchorPointActiveCard[viewid][1] *offsetPer) *numActive , (pramActiveCard[viewid][4]*3 +anchorPointActiveCard[viewid][2] *offsetPer) *numActive)
		for i=1,tagAvtiveCard.cbCardNum do
			--创建卡牌
			local card = self:createOutOrActiveCardSprite(viewid, tagAvtiveCard.cbCardValue[1], false)
			-- if cmd.MY_VIEWID == viewid then
			-- 	card = self:createMyActiveCardSprite(tagAvtiveCard.cbCardValue[1], false)
			-- else
			-- 	card = self:createOutOrActiveCardSprite(viewid, tagAvtiveCard.cbCardValue[1], false)
			-- end
			self.nodeActiveCard[viewid]:addChild(card, -(i + numActive *3) *zorderCard[viewid])
			card:setTag(numActive* 10 + i) --设置索引，方便明杠时候移除
			--计算当前麻将的位置
			local psoCard = cc.p(pos.x + pramActiveCard[viewid][3]*(i -1), pos.y + pramActiveCard[viewid][4]*(i -1))

			--如果是第四张，放在上面
			if 4 == i then  --此处参数比较复杂，推荐画图分析
				card:setLocalZOrder(4+ numActive *3)
				psoCard = cc.p(pos.x + pramActiveCard[viewid][3], pos.y + (anchorPointActiveCard[viewid][2] == 1 and pramActiveCard[viewid][2] *math.abs(anchorPointActiveCard[viewid][2])
					or pramActiveCard[viewid][4] *2 *math.abs(anchorPointActiveCard[viewid][2]) + pramActiveCard[viewid][2] *math.abs(anchorPointActiveCard[viewid][2])) 
				+ math.abs(anchorPointActiveCard[viewid][1]) *pramActiveCard[viewid][2]/5)
			end
			if nil ~= card then
				card:setPosition(psoCard)
			end
			if tagAvtiveCard.cbType == GameLogic.SHOW_AN_GANG and (not (cmd.MY_VIEWID == viewid and 4 == i))then --自己第四张不盖着
				--显示背面
				self:showActiveCardBack( viewid, card )
			end
		end
		table.insert(self.cbActiveCardData[viewid], tagAvtiveCard)
	end
	--删除手上的麻将
	local deleteNum = 0
	if GameLogic.SHOW_MING_GANG ==  tagAvtiveCard.cbType then
		deleteNum = 1
	elseif GameLogic.SHOW_AN_GANG ==  tagAvtiveCard.cbType then
		deleteNum = 4
	elseif GameLogic.SHOW_FANG_GANG ==  tagAvtiveCard.cbType  then
		deleteNum = 3
	elseif GameLogic.SHOW_PENG ==  tagAvtiveCard.cbType  then
		deleteNum = 2
	end
	local haveDelete = 0
	if viewid == cmd.MY_VIEWID or self._scene.bIsVideo  then --自己的查找删除，别人随机删除
		for i = self.cbCardCount[viewid], 1, -1 do
			if self.cbCardData[viewid][i] == tagAvtiveCard.cbCardValue[1] then
				self:removeHandCard(viewid, i)
				haveDelete = haveDelete + 1
				print("玩家操作结果 应该删除， 已经删除 i", deleteNum, haveDelete, i)
				if deleteNum == haveDelete then
					break
				end
			end
		end
	else
		for i = deleteNum, 1, -1 do
			--local index = math.random(0, self.cbCardCount[viewid] -1)
			self:removeHandCard(viewid, i)
		end
	end

	--手牌重新排序
	--刷新麻将	
	self:sortHandCard(viewid)
	--如果是碰或明杠，提供玩家最后一张牌出牌删除(以后有吃一样)
	if GameLogic.SHOW_MING_GANG ==  tagAvtiveCard.cbType or GameLogic.SHOW_PENG == tagAvtiveCard.cbType then
		if nil ~= self.cbOutcardCount[wProvideViewId] then
			self.nodeOutcard[wProvideViewId]:removeChildByTag(self.cbOutcardCount[wProvideViewId])
			self.cbOutcardCount[wProvideViewId] = self.cbOutcardCount[wProvideViewId] -1
			table.remove(self.showCards[wProvideViewId]) --移除被操作的牌
		end
	end

	if viewid == cmd.MY_VIEWID	 then
		--设置可以操作
		self:setMyCardTouchEnabled(true)
	end
end

--创建碰杠牌 断线重连(断线重连没有删除牌的操作，没有重新排序手牌的操作)
function CardLayer:createActiveCardReEnter(viewid, tagAvtiveCard) --传入tagAvtiveCard
		--获取之前已经存在的牌堆数
		local numActive = #self.cbActiveCardData[viewid]
		--计算开始位置(每墩3个，杠牌有一张放在上面)
		local pos = cc.p((pramActiveCard[viewid][3]*3 +anchorPointActiveCard[viewid][1] *offsetPer) *numActive , (pramActiveCard[viewid][4]*3 +anchorPointActiveCard[viewid][2] *offsetPer) *numActive)
		for i=1,tagAvtiveCard.cbCardNum do
			--创建卡牌
			local card = self:createOutOrActiveCardSprite(viewid, tagAvtiveCard.cbCardValue[1], false)
			-- if cmd.MY_VIEWID == viewid then
			-- 	card = self:createMyActiveCardSprite(tagAvtiveCard.cbCardValue[1], false)
			-- else
			-- 	card = self:createOutOrActiveCardSprite(viewid, tagAvtiveCard.cbCardValue[1], false)
			-- end
			self.nodeActiveCard[viewid]:addChild(card, -(i + numActive *3) *zorderCard[viewid])
			card:setTag(numActive* 10 + i) --设置索引，方便明杠时候移除
			--计算当前麻将的位置
			local psoCard = cc.p(pos.x + pramActiveCard[viewid][3]*(i -1), pos.y + pramActiveCard[viewid][4]*(i -1))

			--如果是第四张，放在上面
			if 4 == i then  --此处参数比较复杂，推荐画图分析
				card:setLocalZOrder(4 + numActive *3)
				psoCard = cc.p(pos.x + pramActiveCard[viewid][3], pos.y + (anchorPointActiveCard[viewid][2] == 1 and pramActiveCard[viewid][2] *math.abs(anchorPointActiveCard[viewid][2])
					or pramActiveCard[viewid][4] *2 *math.abs(anchorPointActiveCard[viewid][2]) + pramActiveCard[viewid][2] *math.abs(anchorPointActiveCard[viewid][2])) 
				+ math.abs(anchorPointActiveCard[viewid][1]) *pramActiveCard[viewid][2]/5)
			end
			if nil ~= card then
				card:setPosition(psoCard)
			end
			-- if tagAvtiveCard.cbType == GameLogic.SHOW_MING_GANG and 4 == i then 
			-- 	--显示背面
			-- 	self:showActiveCardBack( viewid, card )
			-- end
			if tagAvtiveCard.cbType == GameLogic.SHOW_AN_GANG and (not (cmd.MY_VIEWID == viewid and 4 == i))then --自己第四张不盖着
				--显示背面
				self:showActiveCardBack( viewid, card )
			end
		end
		table.insert(self.cbActiveCardData[viewid], tagAvtiveCard)
end

--发牌动画(游戏开始)
function CardLayer:sendCardToPlayer(viewid, cbCardData, cbCardCount)
	print("发送扑克到用户", viewid, cbCardCount)
	local heap = nil
	local pos = posHandCard[viewid]
	if 4 == cbCardCount then
		heap = cc.CSLoader:createNode(cmd.RES_PATH.."card/Node_heap.csb")
		pos = cc.p(posHandCard[viewid].x + (self.cbCardCount[viewid] +2) *pramHandCard[viewid][3], posHandCard[viewid].y  + (self.cbCardCount[viewid] +2) *pramHandCard[viewid][4])
	else --一张牌
		heap = cc.Sprite:create(cmd.RES_PATH.."card/back_normal.png")
		pos = cc.p(posHandCard[viewid].x + self.cbCardCount[viewid] * pramHandCard[viewid][3], posHandCard[viewid].y  + self.cbCardCount[viewid] * pramHandCard[viewid][4])
	end
	self:addChild(heap, 0, viewid *10086)
	heap:setPosition(cc.p(667, 375))

	function callbackWithArgs(viewid, cbCardData, cbCardCount, cardSpr)
          local ret = function ()
          	self:createHandCard(viewid, cbCardData, cbCardCount)
          	cardSpr:removeFromParent()
          end
          return ret
    end
    local callFun = cc.CallFunc:create(callbackWithArgs(viewid, cbCardData, cbCardCount, heap))
	heap:runAction(cc.Sequence:create(cc.MoveTo:create(0.1, pos), cc.Hide:create(),callFun))
end

--出牌动画
function CardLayer:outCard(viewid, cbCardInfo)
	-- 移除遮罩
	self:removeCardMark()
	--计算要出的牌的位置
	print("卡牌界面出牌", viewid, cbCardInfo)
	local numOutCard = self.cbOutcardCount[viewid]
	local offsetX, offsetY = 0,0
	if cmd.MY_VIEWID == viewid or cmd.TOP_VIEWID ==  viewid then --我和对面
		offsetX = math.mod(numOutCard, numPerLine)
		offsetY = math.floor(numOutCard/numPerLine)
	elseif cmd.LEFT_VIEWID == viewid or cmd.RIGHT_VIEWID == viewid then
		offsetX = math.floor(numOutCard/numPerLine)
		offsetY = math.mod(numOutCard, numPerLine)
	end
	local pos  = cc.p(pramOutCard[viewid][3]* offsetX, pramOutCard[viewid][4]* offsetY)

	--获取目标地点相对于玩家手牌的相对位置
	local posWorld = self.nodeOutcard[viewid]:convertToWorldSpace(pos)
	local posNode = self.nodeHandCard[viewid]:convertToNodeSpace(posWorld)

	function callbackWithArgs(viewid, cbCardInfo, index)
		local ret = function ()
			--创建麻将，自己出牌传索引，别人出牌算牌值
			--如果是自己出牌，跟据当前选中的卡牌索引
			local cbCardData = {}
			if cmd.MY_VIEWID == viewid or self._scene.bIsVideo then  --回放显示牌
				cbCardData[1] = self.cbCardData[viewid][cbCardInfo]
			else
				cbCardData[1] = cbCardInfo
			end
	  		--添加到麻将node
			self:createOutCard(viewid, cbCardData, 1)
			--移除手牌(自己的话根据索引，别人的牌随机一张移除)
			self:removeHandCard(viewid, index)
			--刷新麻将	
			self:sortHandCard(viewid)
		end
		return ret
    end
    print("创建出牌回调",viewid, cbCardInfo)
    local index = 0
	if cmd.MY_VIEWID == viewid or self._scene.bIsVideo then
		index = cbCardInfo
	else
		index = self.cbCardCount[viewid]
	end
	print("出牌索引", index)
	local card = self.nodeHandCard[viewid]:getChildByTag(index)
    local callFun = cc.CallFunc:create(callbackWithArgs(viewid, cbCardInfo, index))
    if nil ~= card then
    	card:runAction(cc.Sequence:create(cc.MoveTo:create(0.3, posNode), callFun))
    end
end

--托管出牌动画
function CardLayer:outCardTrustee(cbCardInfo)
	--查找卡牌索引
	local index = 0
	for i = self.cbCardCount[cmd.MY_VIEWID], 1, -1 do
		if self.cbCardData[cmd.MY_VIEWID][i] == cbCardInfo then
			index = i
			break
		end
	end
	--打出麻将
	if 0 ~= index then
		self:outCard(cmd.MY_VIEWID, index)
		self:setMyCardTouchEnabled(false)
		self._scene.nodeTips:removeAllChildren()
		--清掉数据
		self.cbListenList = {}
		self.cbHuCard = {}
		self.cbHuCardFan = {}
		self.cbHuCardLeft = {}
	end
end


--回放函数---------------------------------------------------------------------------
--回放出牌
function CardLayer:outCardVedio(viewID, cbCardInfo)
	--查找卡牌索引
	local index = 0
	for i = self.cbCardCount[viewID], 1, -1 do
		if self.cbCardData[viewID][i] == cbCardInfo then
			index = i
			break
		end
	end
	--打出麻将
	if 0 ~= index then
		self:outCard(viewID, index)
		self:setMyCardTouchEnabled(false)
		self._scene.nodeTips:removeAllChildren()
		--清掉数据
		self.cbListenList = {}
		self.cbHuCard = {}
		self.cbHuCardFan = {}
		self.cbHuCardLeft = {}
	end
end
---回放函数--------------------------------------------------------------------------------------------------

--手牌移除
function CardLayer:removeHandCard(viewId, index)
	local sprCard = self.nodeHandCard[viewId]:getChildByTag(index)
	if nil ~= sprCard then
		sprCard:removeFromParent()
		self.cbCardCount[viewId] = self.cbCardCount[viewId] -1 
		table.remove(self.cbCardData[viewId], index)
	end
end

--显示玩家倒下的牌
function CardLayer:showUserTileMajong( viewID, cbCardData )
	--先移除之前的
	self.nodeHandCard[viewID]:removeAllChildren()
	for i=1,#cbCardData do
		local sprCard = nil
		if viewID == cmd.MY_VIEWID then
			sprCard = self:createMyTileCardSprite(cbCardData[i], false)
		else
			sprCard = self:createOutOrActiveCardSprite(viewID, cbCardData[i], false)
		end
		--设置位置
		self.nodeHandCard[viewID]:addChild(sprCard, i* zorderCard[viewID])
		sprCard:setPosition(cc.p((i -1) * pramHandCard[viewID][3], (i - 1) * pramHandCard[viewID][4]))
		print("显示玩家倒下的牌",i, sprCard:getPositionX(), sprCard:getPositionY())
		sprCard:setTag(i) 
	end
end

--判断麻将是否是癞子
function CardLayer:isMagicCard( cardValue )
	for i=1,#self.cbMagicData do
		if nil ~= self.cbMagicData[i] and 34 ~= self.cbMagicData[i] then
			--print("判断麻将是否是癞子", self.cbMagicData[i], GameLogic.SwitchToCardData(self.cbMagicData[i] +1))
			local magicValue = GameLogic.SwitchToCardData(self.cbMagicData[i] +1) --本地是1到34，服务器是0到33self.cbMagicData[i]
			if cardValue == magicValue then
				return true
			end
		end
	end
	return false
end

--获取麻将的胡牌信息
function CardLayer:getHuCardInfo(cardValue)
	local cbHuInfoList = {}
	for i=1,#self.cbListenList do
		if cardValue == self.cbListenList[i] then --找到牌可胡
			--查找胡牌信息 --此处应一一对应
			local huCard = self.cbHuCard[i]
			local huFan = self.cbHuCardFan[i]
			local huNum = self.cbHuCardLeft[i]
			print("获取麻将的胡牌信息", #huCard, huCard[1])
			for j=1,#huCard do
				local cbHuInfo = {}
				print("获取麻将的胡牌信息1", huCard[j])
				cbHuInfo.cbCardValue = huCard[j]
				cbHuInfo.cbNum = huNum[j]
				cbHuInfo.cbFan = huFan[j]
				table.insert(cbHuInfoList, cbHuInfo)
			end
		end
	end
	if 0 < #cbHuInfoList then
		self._scene:showHuTips(cbHuInfoList)
	else
		self._scene.nodeTips:removeAllChildren()
	end
end

--显示选牌模式
function CardLayer:showSelectCardModel( cbCardData , selectModel)
	self.isSelectCardModel = selectModel
	if cmd.SELECT_CARD_NIL == selectModel then
		return
	end
	self.cbSelectData = clone(cbCardData)

	--手牌设置，不可以操作手牌变暗
	local index = 0; --实际索引
	for i=1, cmd.MAX_COUNT do
		local card = self.nodeHandCard[cmd.MY_VIEWID]:getChildByTag(i)
		if nil ~= card then
			local isSelect = false
			for j=1,#cbCardData do
				if self.cbCardData[cmd.MY_VIEWID][i] == cbCardData[j] then
					isSelect = true
				end
			end
			if not isSelect then
				card:setColor(cc.c3b(127,127,127))
			end
		end
	end
end
--选牌模式下，判断卡牌是否可以选择
function CardLayer:isCardCanBeSelect( cbCardData )
	for i=1,#self.cbSelectData do
		if cbCardData == self.cbSelectData[i] then
			return true
		end
	end
	return false
end

--查找可以杠的牌
function CardLayer:findUserGangCard()
	local card = {}
	for i=1,#self.cbCardData[cmd.MY_VIEWID] do
		local cardNum = 0
		local cardValue = self.cbCardData[cmd.MY_VIEWID][i]
		for j=i,#self.cbCardData[cmd.MY_VIEWID] do
			if cardValue == self.cbCardData[cmd.MY_VIEWID][j] then
				cardNum = cardNum +1
			end
		end
		if 4 == cardNum then
			table.insert(card, cardValue)
		end
	end
	return card
end

--触摸事件处理
function CardLayer:onTouchBegan(touch, event)
	if self.isTouchEnable == false then
		return false
	end
	local pos = touch:getLocation()
	--转换到手牌node
	pos = self.nodeHandCard[cmd.MY_VIEWID]:convertToNodeSpace(pos)
	print("touch begin1!", pos.x, pos.y)
	for i = 1, self.cbCardCount[cmd.MY_VIEWID] do
		local card = self.nodeHandCard[cmd.MY_VIEWID]:getChildByTag(i)
		if nil ~= card then
			local cardRect = card:getBoundingBox()
			--自己算boundBoxing
			cardRect.x = cardRect.x - (pramHandCard[cmd.MY_VIEWID][1])/2
			cardRect.y = cardRect.y - (pramHandCard[cmd.MY_VIEWID][2])/2
			if cc.rectContainsPoint(cardRect, pos) then
				if cmd.SELECT_CARD_NIL ~= self.isSelectCardModel  and not self:isCardCanBeSelect(self.cbCardData[cmd.MY_VIEWID][i]) then
					return true
				end
				if cmd.SELECT_CARD_GANG == self.isSelectCardModel then --如果是杠牌选牌模式
					--发送杠牌
					local cbOperateCard = {self.cbCardData[cmd.MY_VIEWID][i], self.cbCardData[cmd.MY_VIEWID][i], self.cbCardData[cmd.MY_VIEWID][i]}
					self._scene._scene:sendOperateCard(GameLogic.WIK_GANG, cbOperateCard)
					--设置为非选牌模式
					self.isSelectCardModel = cmd.SELECT_CARD_NIL
				else
					self.beginPoint = pos
					self.nCurrentTouchCardTag = i
				end
				return true
			end
		end
	end
	return true
end

function CardLayer:onTouchMoved(touch, event)
	if self.isTouchEnable == false then
		return true
	end
	local pos = touch:getLocation()
	--print("touch move!", pos.x, pos.y)
	pos = self.nodeHandCard[cmd.MY_VIEWID]:convertToNodeSpace(pos)
	if self.beginPoint and math.pow(pos.x - self.beginPoint.x,2) + math.pow(pos.y - self.beginPoint.y,2)  < 15*15 then
		self.isMoving = false
	   	return true
	end
	if cmd.SELECT_CARD_NIL ~= self.isSelectCardModel then
		self.isMoving = false
		return  true
	end

	--移动
	if self.nCurrentTouchCardTag ~= 0 then
		if 0 ~= self.selectTag and self.nCurrentTouchCardTag ~= self.selectTag then
			--之前弹起的缩回去
			local card = self.nodeHandCard[cmd.MY_VIEWID]:getChildByTag(self.selectTag)
			--不再提示胡牌
			self._scene.nodeTips:removeAllChildren()
			if nil ~=  card then
				card:runAction(cc.MoveTo:create(0.1, cc.p(card:getPositionX(), 0)))
			end	
			self.selectTag = 0
		end

		--将牌补满(ui与值的对齐方式)
		self.isMoving = true
		local card = self.nodeHandCard[cmd.MY_VIEWID]:getChildByTag(self.nCurrentTouchCardTag)
		if nil ~= card then
			card:setPosition(pos)
			card:setLocalZOrder(CardLayer.Z_ORDER_TOP)

			--听牌提示
			--判断打出该牌是否可以听牌，如果听牌，提示听牌数据
			if 0 == self._scene.nodeTips:getChildrenCount() then
				self:getHuCardInfo(self.cbCardData[cmd.MY_VIEWID][self.nCurrentTouchCardTag])
			end
		end
	end

	return true
end

function CardLayer:onTouchEnded(touch, event)
	if self.isTouchEnable == false then
		return 
	end
	local pos = touch:getLocation()
	--转换到手牌node
	pos = self.nodeHandCard[cmd.MY_VIEWID]:convertToNodeSpace(pos)
	--如果是移动
	if self.isMoving then
		self.isMoving = false
		--判断移动位置，主要是Y轴
		if pos.y > pramHandCard[cmd.MY_VIEWID][2] then --移动生效，出牌
			--打出麻将
			self:outCard(cmd.MY_VIEWID, self.nCurrentTouchCardTag)
			--发消息
			self._scene._scene:sendOutCard(self.cbCardData[cmd.MY_VIEWID][self.nCurrentTouchCardTag])

			self:setMyCardTouchEnabled(false)
			self.selectTag = 0
			self.nCurrentTouchCardTag = 0

			--出牌隐藏操作按钮	
			self._scene:ShowGameBtn(GameLogic.WIK_NULL)
			--出牌之后不再提示胡牌
			self._scene.nodeTips:removeAllChildren()
			--清掉数据
			self.cbListenList = {}
			self.cbHuCard = {}
			self.cbHuCardFan = {}
			self.cbHuCardLeft = {}
		else --还原
				--之前移动牌的放回去
				local card = self.nodeHandCard[cmd.MY_VIEWID]:getChildByTag(self.nCurrentTouchCardTag)
				-- 移除遮罩
				self:removeCardMark()
				--不再提示胡牌
				self._scene.nodeTips:removeAllChildren()
				--计算横坐标
				local posx = (self.nCurrentTouchCardTag -1) * pramHandCard[cmd.MY_VIEWID][3]
				if nil ~=  card then
					local numActive = #self.cbActiveCardData[cmd.MY_VIEWID]
					card:setLocalZOrder((self.nCurrentTouchCardTag -1)* zorderCard[cmd.MY_VIEWID])

					if (14 - numActive*3) == self.nCurrentTouchCardTag then --如果是发牌给玩家，偏离一点点
						posx = posx + pramHandCard[cmd.MY_VIEWID][3]/2 
					end
					card:runAction(cc.MoveTo:create(0.1, cc.p(posx, 0)))
					self.nCurrentTouchCardTag = 0
					self.selectTag = 0
				end
		end
		return
	end
	local isTouchCard = false
	for i = 1, self.cbCardCount[cmd.MY_VIEWID] do
		local card = self.nodeHandCard[cmd.MY_VIEWID]:getChildByTag(i)
		if nil == card then
			break
		end
		local cardRect = card:getBoundingBox()
		--自己算boundBoxing（csb获取boundBoxing 不对）
		cardRect.x = cardRect.x - (pramHandCard[cmd.MY_VIEWID][1])/2
		cardRect.y = cardRect.y - (pramHandCard[cmd.MY_VIEWID][2])/2
		if cc.rectContainsPoint(cardRect, pos) then
			if cmd.SELECT_CARD_NIL ~= self.isSelectCardModel  and not self:isCardCanBeSelect(self.cbCardData[cmd.MY_VIEWID][i]) then
				return
			end
			if i == self.nCurrentTouchCardTag then --如果两次选中的是同一张,而且该麻将没有弹起
				isTouchCard = true
				--设置当前麻将弹起,设置为当前选中
				card:runAction(cc.MoveTo:create(0.1, cc.p(card:getPositionX(), card:getPositionY() + 20)))
				if i ~= self.selectTag then
					--之前弹起的缩回去
					local card = self.nodeHandCard[cmd.MY_VIEWID]:getChildByTag(self.selectTag)
					--不再提示胡牌
					self._scene.nodeTips:removeAllChildren()
					if nil ~=  card then
						card:runAction(cc.MoveTo:create(0.1, cc.p(card:getPositionX(), 0)))
					end
					self.selectTag = i
					if cmd.SELECT_CARD_NIL == self.isSelectCardModel then --如果是选牌模式,不提示胡牌
						--判断打出该牌是否可以听牌，如果听牌，提示听牌数据
						self:getHuCardInfo(self.cbCardData[cmd.MY_VIEWID][i])
					end
				else
					--打出麻将
					self:outCard(cmd.MY_VIEWID, i)
					--发消息
					self._scene._scene:sendOutCard(self.cbCardData[cmd.MY_VIEWID][i])

					self:setMyCardTouchEnabled(false)
					self.selectTag = 0
					self.nCurrentTouchCardTag = 0

					--出牌隐藏操作按钮	
					self._scene:ShowGameBtn(GameLogic.WIK_NULL)
					--出牌之后不再提示胡牌
					self._scene.nodeTips:removeAllChildren()
					--清掉数据
					self.cbListenList = {}
					self.cbHuCard = {}
					self.cbHuCardFan = {}
					self.cbHuCardLeft = {}
				end
			end
		end
	end
	print("touch", isTouchCard, self.selectTag)
	if not isTouchCard then --如果点击麻将外，之前弹起缩回
			--之前弹起的缩回去
			local card = self.nodeHandCard[cmd.MY_VIEWID]:getChildByTag(self.selectTag)
			-- 移除遮罩
			self:removeCardMark()
			if nil ~=  card then
				card:runAction(cc.MoveTo:create(0.1, cc.p(card:getPositionX(), 0)))
			end
			self.nCurrentTouchCardTag = 0
			self.selectTag = 0
			--不再提示胡牌
			self._scene.nodeTips:removeAllChildren()
	end
	
	if isTouchCard and self.selectTag > 0 then
		local outCardData = self.cbCardData[cmd.MY_VIEWID][self.selectTag]
		self:setCardMarkWithValue(outCardData)
	end
end

function CardLayer:removeCardMark()
	for i, spr in pairs(self.markCards) do
		local mark = spr:getChildByName("mark")
		if mark then
			mark:setVisible(false)
		end
	end
	self.markCards = {}
end


function CardLayer:setCardMarkWithValue(cardData)
	-- 先隐藏以显示的
	self:removeCardMark()

	local function showMark(spr)
		local mark = spr:getChildByName("mark")
		if mark then
			mark:setVisible(true)
		end
	end

	for i = 1,4 do
		local activelist = self.cbActiveCardData[i]
		if #activelist > 0 then
			for k, tab in ipairs(activelist) do
				if tab.cbCardValue[1] == cardData then
					for j=1 , tab.cbCardNum do
						local sp = self.nodeActiveCard[i]:getChildByTag((k-1)*10 + j)
				    	showMark(sp)
						table.insert(self.markCards, sp)
					end
				end
			end
		end
	end

	for i, list in ipairs(self.showCards) do
		for k,v in ipairs(list) do
			if v.value == cardData then
				showMark(v.sp)
				table.insert(self.markCards, v.sp)
			end
		end
	end
end
return CardLayer