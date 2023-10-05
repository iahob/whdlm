local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.fivestarshh.src"
local cmd = appdf.req(module_pre..".models.CMD_Game")
local CardSprite = class("CardSprite", cc.Sprite);

function CardSprite:ctor()
	self.cardValue = 0;
	
	self:initCsbRes();
end

function CardSprite:initCsbRes()
	local rootLayer, csbNode = ExternalFun.loadRootCSB("game/CardNode.csb", self);

	self._csbNode = csbNode;

	self.bg = csbNode:getChildByName("bg");

	self.card_value = csbNode:getChildByName("card_value");
	self.card_type_small = csbNode:getChildByName("card_type_small");
	self.card_type_big = csbNode:getChildByName("card_type_big");
	self.card_king = csbNode:getChildByName("card_king");
end

--创建卡牌
function CardSprite:createCard(parent)
	self.m_parent = parent;

	local card = CardSprite.new();

	if card then
		-- card.cardValue = cardValue;
		card:updateSprite();
		--扑克背面
		card:createBack();

		return card;
	end
	return nil;
end

--设置卡牌数值
function CardSprite:setCardValue(cardValue)
	self.cardValue = cardValue;
	self:updateSprite();
end

--获取卡牌图标
function CardSprite:getCardIcon()
	local cardStr = "card/" .. cmd.ServerCardValueSprite[self.cardValue] .. ".png" or "card/card_001.png";
	return cardStr;
end

--更新纹理资源
function CardSprite:updateSprite()
	local cardStr = "card/card_001.png";

	if cmd.ServerCardValueSprite[self.cardValue] then
		cardStr = "card/" .. cmd.ServerCardValueSprite[self.cardValue] .. ".png";
		--大小王
		if self.cardValue == 78 or self.cardValue == 79 then
			self.card_king:setTexture(cardStr);
			self.card_king:setVisible(true);

			self.card_value:setVisible(false);
			self.card_type_small:setVisible(false);
			self.card_type_big:setVisible(false);
		else
			self.card_king:setVisible(false);

			self.card_value:setTexture(cardStr);
			--花色
			local cardColor = self.m_parent:getCardColorByCard(self.cardValue);
			local str1 = string.format("card/card_type_%d_1.png", cardColor);
			local str2 = string.format("card/card_type_%d_2.png", cardColor);
			self.card_type_small:setTexture(str2);
			self.card_type_big:setTexture(str1);

			self.card_value:setVisible(true);
			self.card_type_small:setVisible(true);
			self.card_type_big:setVisible(true);
		end
	end
end

--显示扑克背面
function CardSprite:showCardBack(isShow)
	if nil ~= self.m_spBack then
		self.m_spBack:setVisible(isShow);
	end	
end

--创建背面
function CardSprite:createBack()
	local cardSize = self:getContentSize();
    -- local m_spBack = cc.Sprite:create("card/card_001.png");
    local m_spBack = nil;
    local str = string.format("card_type_%d.png", 1);
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
    if nil ~= frame then
        m_spBack = cc.Sprite:createWithSpriteFrame(frame);
    end

    m_spBack:setPosition(cardSize.width * 0.5, cardSize.height * 0.5);
    -- m_spBack:setVisible(false);
    self:addChild(m_spBack);
    m_spBack:setLocalZOrder(2);

    self.m_spBack = m_spBack;

    self:randomShowCardBack();
end

--翻牌效果
function CardSprite:openCardAnimation()
	-- 翻牌
	if not self.card_value or not self.m_spBack then
		return;
	end

	local delayTime = 0.3;
	self.bg:setFlipX(true);

	local backSeq = cc.Sequence:create(cc.DelayTime:create(delayTime),cc.Hide:create(),cc.DelayTime:create(delayTime),cc.Hide:create());
	local backScale = cc.ScaleTo:create(delayTime, -1, 1);
	local backSpawn = cc.Spawn:create(backSeq,backScale);
	self.m_spBack:runAction(backSpawn);

	local frontSeq = cc.Sequence:create(cc.DelayTime:create(delayTime),cc.Show:create(),cc.DelayTime:create(delayTime),cc.Show:create());
	local frontScale = cc.ScaleTo:create(delayTime, -1, 1);
	local frontSpawn = cc.Spawn:create(frontSeq,frontScale);
	self.bg:runAction(frontSpawn);

	if self.cardValue == 78 or self.cardValue == 79 then
		self.card_king:setFlipX(true);
		local frontSeq = cc.Sequence:create(cc.DelayTime:create(delayTime),cc.Show:create(),cc.DelayTime:create(delayTime),cc.Show:create());
		local frontScale = cc.ScaleTo:create(delayTime, -1, 1);
		local frontSpawn = cc.Spawn:create(frontSeq,frontScale);
		self.card_king:runAction(frontSpawn);
	else
		self.card_value:setFlipX(true);
		self.card_type_small:setFlipX(true);
		self.card_type_big:setFlipX(true);
		local frontSeq = cc.Sequence:create(cc.DelayTime:create(delayTime),cc.Show:create(),cc.DelayTime:create(delayTime),cc.Show:create());
		local frontScale = cc.ScaleTo:create(delayTime, -1, 1);
		local frontSpawn = cc.Spawn:create(frontSeq,frontScale);
		self.card_value:runAction(frontSpawn);

		local frontSeq = cc.Sequence:create(cc.DelayTime:create(delayTime),cc.Show:create(),cc.DelayTime:create(delayTime),cc.Show:create());
		local frontScale = cc.ScaleTo:create(delayTime, -1, 1);
		local frontSpawn = cc.Spawn:create(frontSeq,frontScale);
		self.card_type_small:runAction(frontSpawn);

		local frontSeq = cc.Sequence:create(cc.DelayTime:create(delayTime),cc.Show:create(),cc.DelayTime:create(delayTime),cc.Show:create());
		local frontScale = cc.ScaleTo:create(delayTime, -1, 1);
		local frontSpawn = cc.Spawn:create(frontSeq,frontScale);
		self.card_type_big:runAction(frontSpawn);
	end
end

--随机显示背面图
function CardSprite:randomShowCardBack()
	local randomIndex = math.random(1,4);
	local str = string.format("card_type_%d.png", randomIndex);
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
    if nil ~= frame then
        self.m_spBack:setSpriteFrame(frame);
    end
end

return CardSprite;