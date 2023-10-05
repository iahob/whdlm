local SuperControlLayer = class("SuperControlLayer", function ()
	local cardLayer = display.newLayer()
	return cardLayer
end)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local cmd = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.CMD_Game")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.GameLogic")


local RESULT_TAG = 10   -- 结算正负
local CARDTYPE_TAG = 100
local CHANGECARD_TAG = 200

local EDITBOX_ID_TAG = 21
local EDITBOX_JS_TAG = 22

local ResultCheckTag = 11
local CardTypeCheckTag = 12

--纹理宽高
local CARD_WIDTH = 118
local CARD_HEIGHT = 164.4

function SuperControlLayer:ctor(scene, userItem)
	dump(userItem,"SuperControlLayer")
	self._scene = scene

	self.pUserItem = userItem  -- 被调试玩家
	self.ctrUserItem = scene._scene:GetMeUserItem() -- 调试玩家
	dump(self.pUserItem,"被调试玩家",6)
	dump(self.ctrUserItem,"调试玩家",6)

	self.selectCardData = {0,0,0,0,0}

	self.bCheckWin = false

	self.sendCardType = scene._scene.m_tabPrivateRoomConfig.sendCardType -- 发牌模式

    local rootLayer, csbNode = ExternalFun.loadRootCSB("superControl/SuperControlLayer.csb",self)
    self._csbNode = csbNode

	self.cardTypeNode = self._csbNode:getChildByName("Node_CardType")

    self.resultNode = csbNode:getChildByName("Node_result")


	self.bResultDebug = true
	local swichCheck =  function(sender, eventtype)
    	if eventtype == ccui.TouchEventType.ended then
			self:swichCheck(sender:getTag())
    -- 		local tag = sender:getTag()
    -- 		if tag == ResultCheckTag then
				-- self.bResultDebug = true
    -- 			resultNodeCheck:setSelected(true)
    -- 			cardTypeCheck:setSelected(false)
    -- 		elseif tag == CardTypeCheckTag then
				-- self.bResultDebug = false
    -- 			resultNodeCheck:setSelected(false)
    -- 			cardTypeCheck:setSelected(true)
    -- 		end
    	end
	end
	local resultNodeCheck = self.resultNode:getChildByName("CheckBox_1"):setSelected(true)
	resultNodeCheck:setTag(ResultCheckTag)
	local cardTypeCheck = self.cardTypeNode:getChildByName("CheckBox_1_0"):setSelected(false)
	cardTypeCheck:setTag(CardTypeCheckTag)
	-- 调试选项互斥
	cardTypeCheck:addTouchEventListener(swichCheck)
	resultNodeCheck:addTouchEventListener(swichCheck)
	self.cardTypeCheck = cardTypeCheck
	self.resultNodeCheck = resultNodeCheck

    local cardNode = csbNode:getChildByName("Panel_Card"):setVisible(false)
    self.cardNode = cardNode
    cardNode:setLocalZOrder(10)
    cardNode:addTouchEventListener(function( sender, eventtype )
    	if eventtype == ccui.TouchEventType.ended then
    		cardNode:setVisible(false)
    	end
    end)

	local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    -- 调试ID输入框
    local temp = csbNode:getChildByName("edit1")
    temp:setVisible(false)
    local editbox = ccui.EditBox:create(cc.size(170, 44),"superControl/editBg.png")
                :setPosition(temp:getPosition())
                :setFontColor(cc.c3b(255, 212, 95))
                :setAnchorPoint(cc.p(0.5,0.5))  
                :setPlaceHolder("")
                :setPlaceholderFont("fonts/FZHTJW.TTF", 29)
                :setFont("fonts/FZHTJW.TTF", 24) 
                :setMaxLength(6)
                :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
                :addTo(csbNode)
    self.editboxID = editbox
    self.editboxID:setText(userItem.dwGameID)
    self.editboxID:setTag(EDITBOX_ID_TAG)
    self.editboxID:registerScriptEditBoxHandler(editHanlder)

    -- 调试昵称输入框
    local temp = csbNode:getChildByName("edit2")
    temp:setVisible(false)
    local editbox = ccui.EditBox:create(cc.size(170, 44),"superControl/editBg.png")
                :setPosition(temp:getPosition())
                :setFontColor(cc.c3b(255, 212, 95))
                :setAnchorPoint(cc.p(0.5,0.5))  
                :setPlaceHolder("")
                :setPlaceholderFont("fonts/FZHTJW.TTF", 29)
                :setFont("fonts/FZHTJW.TTF", 24) 
                -- :setMaxLength(6)
                -- :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
                :addTo(csbNode)
    self.editboxName = editbox
    self.editboxName:setText(userItem.szNickName)


    -- 局数输入框
    local temp = self.resultNode:getChildByName("editNum")
    temp:setVisible(false)
    local editbox = ccui.EditBox:create(cc.size(100, 44),"superControl/editBg.png")
                :setPosition(temp:getPosition())
                :setFontColor(cc.c3b(255, 212, 95))
                :setAnchorPoint(cc.p(0.5,0.5))  
                :setPlaceHolder("")
                :setPlaceholderFont("fonts/FZHTJW.TTF", 29)
                :setFont("fonts/FZHTJW.TTF", 24) 
                :setMaxLength(6)
                :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
                :addTo(self.resultNode)
    self.editboxNum = editbox
    self.editboxNum:setTag(EDITBOX_JS_TAG)
    self.editboxNum:registerScriptEditBoxHandler(editHanlder)

    -- 关闭按钮
    csbNode:getChildByName("Button_close"):addTouchEventListener(function(sender,eventtype)
			if eventtype == ccui.TouchEventType.ended then
				scene:removeSuperCtrl()
			end
		end)
    -- 取消调试
    csbNode:getChildByName("Button_Cancel"):addTouchEventListener(function(sender,eventtype)
			if eventtype == ccui.TouchEventType.ended then
				scene._scene:sendCancelDebug(self.pUserItem.wChairID)
			end
		end)
    -- 执行调试
    csbNode:getChildByName("Button_Do"):addTouchEventListener(function(sender,eventtype)
			if eventtype == ccui.TouchEventType.ended then
				local data = {}
				table.insert(data, self.pUserItem.wChairID)
				if self.bResultDebug then
					table.insert(data, 1)
				else
					table.insert(data, 2)
				end
				table.insert(data, self.bCheckWin and 2 or 1)
				local total = tonumber(self.editboxNum:getText()) or 0
				if self.bResultDebug and total <= 0 then
					showToast(self,"请输入正确局数",1)
					return
				end
				table.insert(data, total)
				for i = 1, 5 do
					table.insert(data, self.selectCardData[i])
				end
				table.insert(data, self.ctrUserItem.wChairID)
				dump(data)
				scene._scene:sendDebugConfig(data)
			end
		end)

    self:initResultNodeButton()
    self:initCardTypeNode()
    self:initCardNode()
    self:setDetailed()
end

function SuperControlLayer:onEditEvent(event,editbox)
	local tag = editbox:getTag()
	if tag == EDITBOX_ID_TAG then
		if event == "return" then
			local id = tonumber(editbox:getText())
			local item = self._scene._scene:getUserInfoByGameID(id)
			if item then
				dump(item,"onEditEvent")
				self.pUserItem = item
				self.editboxName:setText(item.szNickName)
				self:updateHandCard()
			else
				self.editboxName:setText("")
				showToast(self, "玩家不在该牌桌", 1)
			end
		end
	elseif tag == EDITBOX_JS_TAG then
		if event == "return" then
			local num = tonumber(editbox:getText()) or 0
			if num > 5 then
				editbox:setText("5")
			end
			self:swichCheck(ResultCheckTag)
		end
	end
end
function SuperControlLayer:swichCheck(tag)
	if tag == ResultCheckTag then
		self.bResultDebug = true
		self.resultNodeCheck:setSelected(true)
		self.cardTypeCheck:setSelected(false)
	elseif tag == CardTypeCheckTag then
		self.bResultDebug = false
		self.resultNodeCheck:setSelected(false)
		self.cardTypeCheck:setSelected(true)
	end
end
-- 调试状态说明更新
function SuperControlLayer:updateStatus(data)
	dump(data,"调试状态说明更新")
    -- 可点击牌型
	for i=0, 18 do
		local typeCheck = self.cardTypeNode:getChildByName("cardType_"..i)
		typeCheck:setEnabled(data.bValidDebugType[1][i + 1])
	end

	local text = self._csbNode:getChildByName("Text_39_0")
	local resultStr = self._csbNode:getChildByName("result")
	local cur_num = self._csbNode:getChildByName("curr_num")

	text:setVisible(true)
	resultStr:setVisible(true)
	cur_num:setVisible(true)
	self.control_name:setVisible(true)
	self.control_ID:setVisible(true)
	self.player_name:setVisible(true)
	self.Text_tips:setVisible(true)

	local debugUserItem = self._scene._scene:getUserInfoByChairID(data.stConfig.wDebugUserChair)
	local ctrlUserItem = self._scene._scene:getUserInfoByChairID(data.stConfig.wConfigUserChair)

	if data.stConfig.cbType == 1 and debugUserItem ~= nil then
		text:setString("调试结果结算为   ，局数")
		local str = {"输","赢"}
		resultStr:setString(str[data.stConfig.cbGameWin])
		cur_num:setString(string.format("第%d局(共%d局)",data.cbCurDebugCount, data.stConfig.cbGameCount ))
	elseif data.stConfig.cbType == 2 and debugUserItem ~= nil then
		text:setString("调试牌型结果为")
		resultStr:setString(GameLogic.CT_OX_CLASSIC_LIST[data.cbCurCardType + 1])
		cur_num:setString("")

		-- 自己牌型调试成功
		local sendCardStatus = self._scene._scene.m_SendCardStatus
		if self.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
			if data.stConfig.wDebugUserChair == data.stConfig.wConfigUserChair then
				if sendCardStatus == cmd.SEND_COMPLETE and self._scene._scene.m_cbGameStatus ~= cmd.GS_TK_FREE then
					self._scene._scene.cbCardData[data.stConfig.wDebugUserChair+ 1] = data.stConfig.cbGameCards[1]
            		self._scene:setCombineCard(data.stConfig.cbGameCards[1])

            		if self._scene._bShowCard == true then
		                local card = data.stConfig.cbGameCards[1][5]
		                local value = GameLogic:getCardValue(card)
		                local color = GameLogic:getCardColor(card)
		                self._scene:setCardTextureRect(cmd.MY_VIEWID, 5, value, color)
		                self._scene:updateCardType(data.cbCurCardType,true,true)
		            end
				end
			end
		else
			if data.stConfig.wDebugUserChair == data.stConfig.wConfigUserChair then
				if sendCardStatus == cmd.SEND_COMPLETE and self._scene._scene.m_cbGameStatus ~= cmd.GS_TK_FREE then
					self._scene._scene.cbCardData[data.stConfig.wDebugUserChair+ 1] = data.stConfig.cbGameCards[1]
	        		self._scene:setCombineCard(data.stConfig.cbGameCards[1])

	        		if self._scene._bShowCard == true then
	        			for i = 1, 5 do
			                local data = data.stConfig.cbGameCards[1][i]
			                local value = GameLogic:getCardValue(data)
			                local color = GameLogic:getCardColor(data)
			                self._scene:setCardTextureRect(cmd.MY_VIEWID, i, value, color)
			                self._scene:updateCardType(data.cbCurCardType,true,true)
			            end
		            end
				end
			end
		end
	else
		-- 当前没有调试
		text:setVisible(false)
		resultStr:setVisible(false)
		cur_num:setVisible(false)
		self.control_name:setVisible(false)
		self.control_ID:setVisible(false)
		self.player_name:setVisible(false)
		self.Text_tips:setVisible(false)
		return
	end
	

	self.control_name:setString(ctrlUserItem.szNickName)
	self.control_ID:setString(ctrlUserItem.dwGameID)
	self.player_name:setString(debugUserItem.szNickName)
end

-- 调试状态说明
function SuperControlLayer:setDetailed()

	self.Text_tips = self._csbNode:getChildByName("Text_39")
	local control_name = self._csbNode:getChildByName("control_name"):setVisible(false)
	local nick =  ClipText:createClipText(cc.size(80,25),self.ctrUserItem.szNickName,"fonts/FZHTJW.TTF",22)
	nick:setAnchorPoint(cc.p(0.5,0.5))
	nick:setTextColor(cc.c3b(255,194,76))
	nick:addTo(self._csbNode)
	nick:setPosition(control_name:getPosition())
	self.control_name = nick


	self.control_ID = self._csbNode:getChildByName("control_ID"):setString(self.ctrUserItem.dwGameID)


	local player_name = self._csbNode:getChildByName("player_name"):setVisible(false)
	local nick =  ClipText:createClipText(cc.size(80,25),self.pUserItem.szNickName,"fonts/FZHTJW.TTF",22)
	nick:setTextColor(cc.c3b(255,194,76))
	nick:setAnchorPoint(cc.p(0,0.5))
	nick:addTo(self._csbNode)
	nick:setPosition(player_name:getPosition())
	self.player_name = nick


end

function SuperControlLayer:initResultNodeButton()

	local checkWin = self.resultNode:getChildByName("CheckBox_Win"):setTag(RESULT_TAG + 1)
	local checkLose = self.resultNode:getChildByName("CheckBox_Lose"):setTag(RESULT_TAG + 2)
	local nodeSelect = self.resultNode:getChildByName("CheckBox_1")

	local function listenerFun( sender, type )
		local tag = sender:getTag() - RESULT_TAG
		self:swichCheck(ResultCheckTag)
		if tag == 1 then
			self.bCheckWin = true
			checkWin:setSelected(true)
			checkLose:setSelected(false)
		else
			self.bCheckWin = false
			checkWin:setSelected(false)
			checkLose:setSelected(true)
		end
	end

	checkWin:addTouchEventListener(listenerFun)
	checkLose:addTouchEventListener(listenerFun)
end


function SuperControlLayer:initCardTypeNode()
	local cardTypeNode = self._csbNode:getChildByName("Node_CardType")
	local nodeSelect = cardTypeNode:getChildByName("CheckBox_1_0")

	local function listenerFun( sender, eventtype )
		if eventtype == ccui.TouchEventType.ended then
			local tag = sender:getTag() - CARDTYPE_TAG
			self._scene._scene:sendRequestCardType(self.pUserItem.wChairID, tag)

			self:swichCheck(CardTypeCheckTag)    -- 切换选择box
			for i=0, 18 do
				local typeCheck = cardTypeNode:getChildByName("cardType_"..i)
				if tag == i then
					typeCheck:setSelected(true)
				else
					typeCheck:setSelected(false)
				end
			end
		end
	end

	for i=0, 18 do
		local typeCheck = cardTypeNode:getChildByName("cardType_"..i)
		typeCheck:setTag(CARDTYPE_TAG + i)
		typeCheck:addTouchEventListener(listenerFun)
	end

	for k = 1, 5 do
		local btn = cardTypeNode:getChildByName("btn_ChangeCard_"..k)
		btn:setTag(CHANGECARD_TAG + k)
		btn:addTouchEventListener(function(sender,eventtype)
			if eventtype == ccui.TouchEventType.ended then
				self._scene._scene:sendRequestCardPool()
				self.changeCardIdx = sender:getTag() - CHANGECARD_TAG
			end
		end)

		local card = cardTypeNode:getChildByName("Image_"..k)
		card:setTag(CHANGECARD_TAG + k)
		card:addTouchEventListener(function(sender,eventtype)
			if eventtype == ccui.TouchEventType.ended then
				local idx = sender:getTag() - CHANGECARD_TAG
				if cardTypeNode:getChildByName("btn_ChangeCard_"..k):isVisible() then
					self._scene._scene:sendRequestCardPool()
					self.changeCardIdx = sender:getTag() - CHANGECARD_TAG
				end
			end
		end)
		local sp = cc.Sprite:new()
		sp:addTo(card, 1, 1)
		sp:setPosition(59,82.2)

		local strCardFile = "game/card.png"
		local tex = cc.Director:getInstance():getTextureCache():getTextureForKey(strCardFile);
		if nil ~= card and nil ~= tex and sp:initWithTexture(tex, cc.rect(2*CARD_WIDTH,4*CARD_HEIGHT,CARD_WIDTH,CARD_HEIGHT)) then
			self:updateSprite(sp, 0)
		end
	end
	self:updateHandCard()
end

-- 手牌更新
function SuperControlLayer:updateHandCard(cards,bRequestType)
dump(self.pUserItem,"手牌更新")
	self._scene._scene:requestCurDebugConfig(self.pUserItem.wChairID)

	local cardData = cards ~= nil and cards or self._scene._scene.cbCardData[self.pUserItem.wChairID + 1]
	if cardData then
		for i = 1, 5 do
			if cardData[i] then
				self.selectCardData[i] = cardData[i]
			end
		end
	end

	local sendCardStatus = self._scene._scene.m_SendCardStatus
	-- print("===============self.pUserItem.wChairID",self.pUserItem.wChairID)
	-- dump(self.selectCardData,"SuperControlLayer")
	local showBtnNum = 0
	local showCardNum = 0
	if self.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
		showBtnNum = 1
		if self.pUserItem.dwGameID == self.ctrUserItem.dwGameID then
			if sendCardStatus == cmd.SEND_FOUR then
				showCardNum = 4
			else
				showCardNum = 5
				if sendCardStatus == cmd.SEND_COMPLETE then
					showBtnNum = 1
				else
					showBtnNum = 5
				end
			end
		else
			if sendCardStatus == cmd.SEND_FOUR then
				showCardNum = 4
				showBtnNum = 1
			else
				showCardNum = 0
				showBtnNum = 5
			end
		end
	else
		if self.pUserItem.dwGameID == self.ctrUserItem.dwGameID then
			showBtnNum = 5
			if sendCardStatus == 0 then
				showCardNum = 0
			else
				showCardNum = 5
			end
		else
			if sendCardStatus == cmd.SEND_FOUR then
				showCardNum = 4
				showBtnNum = 1
			elseif sendCardStatus == cmd.OPEN_CARD then
				showCardNum = 0
				showBtnNum = 5
			else
				showCardNum = 5
				showBtnNum = 5
			end
		end

	end
    if self._scene._scene.m_cbGameStatus == cmd.GS_TK_FREE then
		showCardNum = 0
		showBtnNum = 5
	end
	if bRequestType then
		showCardNum = 5
	end
	print(" ****************** 刷新手牌 showCardNum ,showBtnNum ",showCardNum ,showBtnNum)
	for k = 1, 5 do
		local btn = self.cardTypeNode:getChildByName("btn_ChangeCard_"..k)
		local card = self.cardTypeNode:getChildByName("Image_"..k)
		local sp = card:getChildByTag(1)

		if showBtnNum == 1 then
			if k == 5 then
				btn:setVisible(true)
			else
				btn:setVisible(false)
				-- self:updateSprite(sp, cardData[k])
			end
		elseif showBtnNum == 5 then
			btn:setVisible(true)
		elseif showBtnNum == 0 then
			btn:setVisible(false)
		end

		if showCardNum == 4 then
			if k == 5 then
				self:updateSprite(sp, 0)
				self.selectCardData[k] = 0
			else
				self:updateSprite(sp, self.selectCardData[k])
			end
		elseif showCardNum == 5 then
			self:updateSprite(sp, self.selectCardData[k])
		elseif showCardNum == 0 then
			self:updateSprite(sp, 0)
			self.selectCardData[k] = 0
		end
	end
end

function SuperControlLayer:initCardNode()

	local strCardFile = "game/card.png"

	local tex = cc.Director:getInstance():getTextureCache():getTextureForKey(strCardFile);
	local function cardTouchFun (sender,eventtype)
		if eventtype == ccui.TouchEventType.ended then
			local p = sender:getChildByTag(1)
			if p then
				p:setColor(cc.c3b(120,120,120))
				self.selectCardData[self.changeCardIdx] = sender:getTag()
				local card = self.cardTypeNode:getChildByName("Image_"..self.changeCardIdx)
				self:updateSprite(card:getChildByTag(1),sender:getTag())
				self.cardNode:setVisible(false)
				self:swichCheck(CardTypeCheckTag)    -- 切换选择box
			end
		end
	end

	for i = 1, 5 do
		for k = 1, 13 do
			if i == 5 and k > 2 then break end
			local image = self.cardNode:getChildByName("Image_"..i.."_"..k)
			local value = (4-i)*16 + k
			if i == 5 then
				value = 4*16 + 13 + k
			end
			image:setTag(value)
			image:addTouchEventListener(cardTouchFun)

			local sp = cc.Sprite:new()
			sp:addTo(image, 1, 1)
			sp:setPosition(59,82.2)

			if nil ~= image and nil ~= tex and sp:initWithTexture(tex, cc.rect(2*CARD_WIDTH,4*CARD_HEIGHT,CARD_WIDTH,CARD_HEIGHT)) then
				self:updateSprite(sp, value)
			end
		end
	end
end

function SuperControlLayer:updateCardNode(bCards)

	for i = 1, 5 do
		for k = 1, 13 do
			if i == 5 and k > 2 then break end
			local idx = (4 - i)*13 + k
			if i == 5 then
				idx = 52+ k
			end
			local image = self.cardNode:getChildByName("Image_"..i.."_"..k)
			local sp = image:getChildByTag(1)
			print("idx==========",idx)
			if bCards[idx] then
				image:setEnabled(true)
				sp:setColor(cc.c3b(255,255,255))
			else
				image:setEnabled(false)
				sp:setColor(cc.c3b(120,120,120))
			end

			local value = (4-i)*16 + k
			if i == 5 then
				value = 4*16 + 13 + k
			end
			for j = 1, 5 do
				if self.selectCardData[j] == value then
					image:setEnabled(false)
					sp:setColor(cc.c3b(120,120,120))
				end
			end
		end
	end
end

--更新纹理资源
function SuperControlLayer:updateSprite(card, cardData)

  if 0xffff == cardData then
    return
  end

  local m_cardValue = yl.POKER_VALUE[cardData] or 0
  local m_cardColor = yl.CARD_COLOR[cardData]  or 0
  
  local rect = cc.rect((m_cardValue - 1) * CARD_WIDTH, m_cardColor * CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
  if 0 ~= cardData then
    rect = cc.rect((m_cardValue - 1) * CARD_WIDTH, m_cardColor * CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
    if 0x4F == cardData then
      rect = cc.rect(CARD_WIDTH, 4 * CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
    elseif 0x4E == cardData then
      rect = cc.rect(0, 4 * CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
    end
  else
    --使用背面纹理区域
    rect = cc.rect(2 * CARD_WIDTH, 4 * CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
  end
  card:setTextureRect(rect);
end

function SuperControlLayer:showCardNode(bCards)
	self:updateCardNode(bCards)
	self.cardNode:setVisible(true)
end
return SuperControlLayer