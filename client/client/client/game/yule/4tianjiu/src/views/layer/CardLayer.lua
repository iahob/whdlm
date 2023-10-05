local CardLayer = class("CardLayer", function(scene)
	local cardLayer = display.newLayer()
	return cardLayer
end)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")
local cmd = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.CMD_Game")
local CardNode = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.views.layer.CardNode")

--手牌位置
-- local handCardPos = {cc.p(537, 650), cc.p(184, 420), cc.p(517, 160), cc.p(920, 420)};
local handCardW = 80;
local myHandCardW = 95;

--手牌位置
local handCardPos = {   
						{cc.p(537, 650), cc.p(617, 650), cc.p(697, 650), cc.p(777, 650)},
						{cc.p(184, 420), cc.p(264, 420), cc.p(344, 420), cc.p(424, 420)},
						{cc.p(517, 160), cc.p(612, 160), cc.p(707, 160), cc.p(802, 160)},
					    {cc.p(920, 420), cc.p(1000, 420), cc.p(1080, 420), cc.p(1160, 420)}
					}

--结束手牌位置
local handCardResultPos = {   
						{cc.p(582, 688), cc.p(582, 612), cc.p(697, 650), cc.p(777, 650)},
						{cc.p(229, 458), cc.p(229, 382), cc.p(344, 420), cc.p(424, 420)},
						{cc.p(572, 204), cc.p(572, 116), cc.p(707, 160), cc.p(802, 160)},
					    {cc.p(965, 458), cc.p(965, 382), cc.p(1080, 420), cc.p(1160, 420)}
					}

function CardLayer:ctor(viewParent)
	self.m_parent = viewParent;

	--注册touch事件、是否设置吞噬
	ExternalFun.registerTouchEvent(self, true)


	self.cardArr = {};             --牌组
	self.selecting = false;        --是否选牌
	self.startPos = 0;             --起始位置
	self.endPos   = 0;             --结束位置
	self.cardWd   = 105;           --卡牌宽
	self.cardHg   = 160;           --卡牌高
	self.cardLayerH = 200;         --触摸高度
	self.isTouchEnable = false;    -- 设置自己是否可以触摸
	
	--手牌
	self.nodeHandCard = self:createHandCardNode();
	self.node_hand = {{}, {}, {}, {}};
end

--加入牌组
function CardLayer:addCard(card)
	table.insert(self.cardArr, card);

end

function CardLayer:onResetData()
	self:stopAllActions()
	for i = 1, cmd.GAME_PLAYER do
		self.nodeHandCard[i]:removeAllChildren()
	end
	self.node_hand = {{}, {}, {}, {}};
	self.cardArr = {};             --牌组
	self.isTouchEnable = false;     -- 设置自己是否可以触摸

	self:onResetSelfCards();
end

function CardLayer:onRemoveHandCard(viewId)
	self.nodeHandCard[viewId]:removeAllChildren();
end

--初始化手牌NODE
function CardLayer:createHandCardNode()
	local nodeCard = {};
	for i = 1, cmd.GAME_PLAYER do
		nodeCard[i] = cc.Node:create():addTo(self, 5);
	end
	return nodeCard;
end

--获取玩家手牌
function CardLayer:getHandCard()
	return self.node_hand;
end

--创建手牌
function CardLayer:createHandCard(viewId, cbCardCount)
	--先清除牌
	self:onRemoveHandCard(viewId);

	self.isTouchEnable = true;
	for i=1,cmd.handCardNum do 
		local CardNode = CardNode:create(handCardPos[viewId][i]);
		CardNode:setPosition(cc.p(handCardPos[viewId][i].x, handCardPos[viewId][i].y));
		self.nodeHandCard[viewId]:addChild(CardNode);
    	
    	self.node_hand[viewId][i] = CardNode;
    	
    	if i <= self.m_parent.cardNumLimit then
    		CardNode:setVisible(true);
    		CardNode:showCardBack(true);
    	else
    		CardNode:setVisible(false);
    	end

    	-- local offsetPos = 0;
    	if viewId == cmd.MY_VIEWID then
    		CardNode:setScale(1.0);
    		self:addCard(CardNode);
    		--自己亮牌
    		if self.m_parent.myCardData[i] > 0 or self.m_parent._scene.m_bLookOn then
    			CardNode:setSelected(false, false);
	    		
    			if self.m_parent._scene.m_bLookOn then
	    			CardNode:showCardBack(true);
    			else
    				CardNode:setCardValue(self.m_parent.myCardData[i]);
	    			CardNode:showCardBack(false);
    			end
    		else
    			CardNode:setVisible(false);
    		end
    	else
    		CardNode:setScale(0.85);
    	end 
    	
    	
    	--庄家亮牌
    	local viewIdBanker = self.m_parent._scene:SwitchViewChairID(self.m_parent._scene.wBankerUser);
    	if viewIdBanker == viewId then
    		if i <= self.m_parent.cbBankerOpenCardCount and self.m_parent.cbBankerHandCard[i] and self.m_parent.cbBankerHandCard[i] > 0 then
    			CardNode:setCardValue(self.m_parent.cbBankerHandCard[i]);
    			if self.m_parent._scene.m_bLookOn then
	    			CardNode:showCardBack(true);
    			else
	    			CardNode:showCardBack(false);
    			end
    		end
    	end
	end
end

--组牌完成
function CardLayer:showZupaiResult(viewId)
	if viewId == cmd.MY_VIEWID then
		self.isTouchEnable = false;
	end
	for i=1,cmd.handCardNum do
		local CardNode = self.node_hand[viewId][i];
		if CardNode then
			CardNode:setSelected(false, false);

			if i <= self.m_parent.cardNumLimit then
	    		CardNode:setVisible(true);
	    		CardNode:showCardBack(true);
	    	else
	    		CardNode:setVisible(false);
	    	end

	    	--重新计算位置
	    	-- if not self.m_parent.bDaPaiJiu then  --小牌九
	    	-- 	CardNode:setCardValue(self.m_parent.myCardData[i]);
	    	-- 	CardNode:setPosition(handCardPos[viewId][i]);
	    	-- else
	    		if i <= 2 then
					CardNode:setCardLeftShow(self.m_parent.myCardData[i]);
				else
					CardNode:setCardValue(self.m_parent.myCardData[i]);
				end
				CardNode:setPosition(handCardResultPos[viewId][i]);
	    	-- end

	    	--自己亮牌
	    	if viewId == cmd.MY_VIEWID then
	    		if self.m_parent.myCardData[i] > 0 or self.m_parent._scene.m_bLookOn then
	    			CardNode:setVisible(true);
	    			if self.m_parent._scene.m_bLookOn then
		    			CardNode:showCardBack(true);
	    			else
		    			CardNode:showCardBack(false);
	    			end
	    		else
	    			CardNode:setVisible(false);
	    		end
	    	end 
		end
	end
end

--显示手牌
function CardLayer:showHandCard(viewId, cbHandCard)
	if viewId == cmd.MY_VIEWID then
		self.isTouchEnable = false;
	end
	for i=1,cmd.handCardNum do
		local CardNode = self.node_hand[viewId][i];
		if CardNode then
			CardNode:setSelected(false, false);

			if cbHandCard[i] > 0 then
				CardNode:setVisible(true);
				CardNode:showCardBack(false);
				--重新计算位置
				-- if not self.m_parent.bDaPaiJiu then  --小牌九
		  --   		CardNode:setCardValue(self.m_parent.myCardData[i]);
		  --   		CardNode:setPosition(handCardPos[viewId][i]);
		  --   	else
		    		if i <= 2 then
						CardNode:setCardLeftShow(cbHandCard[i]);
					else
						CardNode:setCardValue(cbHandCard[i]);
					end
					CardNode:setPosition(handCardResultPos[viewId][i]);
		    	-- end
			else
				CardNode:setVisible(false);
			end
		end
	end
end

--触摸事件处理
function CardLayer:onTouchBegan(touch, event)
	if self.isTouchEnable == false or self.m_parent._scene.m_bLookOn then
		return false
	end
	local len = #self.cardArr;

	if len == 0 then
		return;
	end

	-- local posStart = self:convertToWorldSpaceAR(cc.p(self.cardArr[1]:getPositionX() - self.cardWd / 2 -20, self.cardArr[1]:getPositionY()));
	local posStart = cc.p(self.cardArr[1]:getPositionX() - self.cardWd / 2 -20, self.cardArr[1]:getPositionY() - self.cardHg/2);
	local rect = cc.rect(posStart.x, posStart.y, (self.cardArr[len]:getPositionX() - self.cardArr[1]:getPositionX() + self.cardWd), self.cardLayerH);
	local touchPos = touch:getLocation();
	
	if cc.rectContainsPoint(rect, touchPos) then
		print("onTouchBegan");
		self.selecting = true;
		self.endPos = touchPos.x
		self.startPos = self.endPos;

		self:touchCards();
	else
		print("onTouchBegan1111");
		-- self:onResetSelfCards();
	end
	return true
end

function CardLayer:onTouchMoved(touch, event)
	if self.isTouchEnable == false or self.m_parent._scene.m_bLookOn then
		return false
	end
	if not self.selecting then
		return;
	end
	local len = #self.cardArr;
	if len == 0 then
		return;
	end

	-- local posStart = self:convertToWorldSpaceAR(cc.p(self.cardArr[1]:getPositionX() - self.cardWd / 2 -20, self.cardArr[1]:getPositionY()));
	local posStart = cc.p(self.cardArr[1]:getPositionX() - self.cardWd / 2 -20, self.cardArr[1]:getPositionY());
	local rect = cc.rect(posStart.x, posStart.y, (self.cardArr[len]:getPositionX() - self.cardArr[1]:getPositionX() + self.cardWd), self.cardLayerH);
	local touchPos = touch:getLocation();

	if cc.rectContainsPoint(rect, touchPos) then
		print("onTouchMoved");
		self.endPos = touchPos.x;
		self:touchCards();
	end

	return true
end

function CardLayer:onTouchEnded(touch, event)
	if self.isTouchEnable == false or self.m_parent._scene.m_bLookOn then
		return false
	end
	for i=1, #self.cardArr do
		local CardNode = self.cardArr[i];
		if CardNode then
			if CardNode:getTouched() then
				CardNode:setSelected(not CardNode:getSelected(), true);
			end
			CardNode:setTouched(false);
			if CardNode:getSelected() then
				self.m_parent.cbSelectedCardData[i] = CardNode;
			else
				self.m_parent.cbSelectedCardData[i] = nil;
			end
		end
	end
	self.selecting = false;

	--能否组牌
	self.m_parent:onIsCanZuPai();
end

--置灰所有选中的牌
function CardLayer:touchCards()
	local sPos = self.startPos;
	local ePos = self.endPos;
	if self.endPos < self.startPos then
		sPos = self.endPos;
		ePos = self.startPos;
	end
	-- local zeroPiontToWorld = self:convertToWorldSpaceAR(cc.p(0, 0));
	local zeroPiontToWorld = cc.p(0, 0);
	for i=1, #self.cardArr do
		local CardNode = self.node_hand[cmd.MY_VIEWID][i];
		if CardNode then
			if (zeroPiontToWorld.x + CardNode:getPositionX() - self.cardWd / 2 + 90) >= sPos and (zeroPiontToWorld.x + CardNode:getPositionX() - self.cardWd / 2) <= ePos then
				CardNode:setTouched(true);
			else
				CardNode:setTouched(false);
			end
		end
	end
end

--重置自己的牌面
function CardLayer:onResetSelfCards()
	self.m_parent.cbSelectedCardData = {nil, nil, nil, nil};
	for i=1, #self.cardArr do
		local CardNode = self.node_hand[cmd.MY_VIEWID][i];
		if CardNode then
			-- CardNode._touched = false;
			CardNode:setTouched(false);
			CardNode:setSelected(false, false);
			-- if CardNode:getSelected() then
				
			-- end
		end
	end
end

return CardLayer