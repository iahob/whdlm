
local CardSprite = class("CardSprite", cc.Sprite);

--服务器返回的牌值对应的Sprite
local ServerCardValueSprite = 
{
	--方块 A-K
	[1] = "Card101",[2] = "Card102",[3] = "Card103",[4] = "Card104",[5] = "Card105",
	[6] = "Card106",[7] = "Card107",[8] = "Card108",[9] = "Card109",[10] = "Card110",
	[11] = "Card111",[12] = "Card112",[13] = "Card113",
	--梅花 A-K
	[17] = "Card201",[18] = "Card202",[19] = "Card203",[20] = "Card204",[21] = "Card205",
	[22] = "Card206",[23] = "Card207",[24] = "Card208",[25] = "Card209",[26] = "Card210",
	[27] = "Card211",[28] = "Card212",[29] = "Card213",
	--红桃 A-K
	[33] = "Card301", [34] = "Card302", [35] = "Card303", [36] = "Card304", [37] = "Card305", 
	[38] = "Card306", [39] = "Card307", [40] = "Card308", [41] = "Card309", [42] = "Card310", 
	[43] = "Card311", [44] = "Card312", [45] = "Card313",
	--黑桃 A-K
	[49] = "Card401", [50] = "Card402", [51] = "Card403", [52] = "Card404", [53] = "Card405",
	[54] = "Card406", [55] = "Card407", [56] = "Card408", [57] = "Card409", [58] = "Card410",
	[59] = "Card411", [60] = "Card412", [61] = "Card413"
}

function CardSprite:ctor()
	self.cardValue = 0;
end

--创建卡牌
function CardSprite:createCard()
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
	local cardStr = "card/" .. ServerCardValueSprite[self.cardValue] .. ".png" or "card/Card001.png";
	return cardStr;
end

--更新纹理资源
function CardSprite:updateSprite()
	local cardStr = "card/Card001.png";

	if ServerCardValueSprite[self.cardValue] then
		cardStr = "card/" .. ServerCardValueSprite[self.cardValue] .. ".png";	
	end
	if not self._card then
		local cardSize = self:getContentSize();
		self._card = cc.Sprite:create(cardStr);
	    self._card:setPosition(cardSize.width * 0.5, cardSize.height * 0.5);
	    -- m_spBack:setVisible(false);
	    self:addChild(self._card);
	    self._card:setLocalZOrder(1);
	else
		self._card:setTexture(cardStr);
	end
	-- self:setTexture(cardStr);
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
    local m_spBack = cc.Sprite:create("card/Card001.png");
    m_spBack:setPosition(cardSize.width * 0.5, cardSize.height * 0.5);
    -- m_spBack:setVisible(false);
    self:addChild(m_spBack);
    m_spBack:setLocalZOrder(2);

    self.m_spBack = m_spBack;
end

--翻牌效果
function CardSprite:openCardAnimation()
	-- 翻牌
	if not self._card or not self.m_spBack then
		return;
	end

	self._card:setFlipX(true);

	local backSeq = cc.Sequence:create(cc.DelayTime:create(0.15),cc.Hide:create(),cc.DelayTime:create(0.15),cc.Hide:create());
	local backScale = cc.ScaleTo:create(0.15, -1, 1);
	local backSpawn = cc.Spawn:create(backSeq,backScale);
	self.m_spBack:runAction(backSpawn);

	local frontSeq = cc.Sequence:create(cc.DelayTime:create(0.15),cc.Show:create(),cc.DelayTime:create(0.15),cc.Show:create());
	local frontScale = cc.ScaleTo:create(0.15, -1, 1);
	local frontSpawn = cc.Spawn:create(frontSeq,frontScale);
	self._card:runAction(frontSpawn);

end

return CardSprite;