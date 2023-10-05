local GameLogic = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.GameLogic")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local CardSprite = class("CardSprite", cc.Sprite)

local CARD_WIDTH = 1157/13
local CARD_HEIGHT = 575/5
local CARD_SCALE = 1.1

function CardSprite:ctor()
	self.cardData = 0
	self.cardValue = 0
	self.cardColor = 4

	local cardSize = self:getContentSize()
    self.effect = cc.Sprite:create("animation/pai/1.png")
    self.effect:setPosition(cardSize.width*0.5, cardSize.height*0.5+13)
    self.effect:addTo(self,-1)

	local animation = cc.Animation:create()
    for i = 1, 13 do
        -- local buf = string.format(format, i)
        animation:addSpriteFrameWithFile("animation/pai/"..i..".png")
        -- animation:setScale(1.2)
    end
    animation:setDelayPerUnit(0.1)
    animation:setRestoreOriginalFrame(false)
    self.effect:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    self.effect:setVisible(false)
end

--创建卡牌
function CardSprite:createCard()
	local card = CardSprite.new()
	card.cardFile = "card/card.png"

	local tex = cc.Director:getInstance():getTextureCache():getTextureForKey(card.cardFile)
	if nil == tex then
		tex = cc.Director:getInstance():getTextureCache():addImage(card.cardFile)
	end

	if card then
		card:upDateSprite()
		card:createBack()
		return card
	end

	return nil
end

function CardSprite:setCardValue(value)
	local cardType = GameLogic.GetCardColor(value)

	if cardType == 0x00 then
		--方块
		self.cardColor = 0
	elseif cardType == 0x10 then
		--梅花
		self.cardColor = 1
	elseif cardType == 0x20 then
		--红桃
		self.cardColor = 2
	elseif cardType == 0x30 then
		--黑桃
		self.cardColor = 3
	elseif cardType == 0x30 then
		--大小王以及牌背面
		self.cardColor = 4
	end 
	self.cardData = value
	self.cardValue = GameLogic.GetCardValue(self.cardData)
	self:upDateSprite()
end

function CardSprite:upDateSprite()
	self:createCardFront()
end

function CardSprite:showCardBack(isShow)
	if nil ~= self.m_spBack then
		self.m_spBack:setVisible(isShow);
	end
end

function CardSprite:createBack()
	local tex = cc.Director:getInstance():getTextureCache():getTextureForKey(self.cardFile)
	local rect = cc.rect(0*CARD_WIDTH, 4*CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT)
	local cardSize = self:getContentSize()
	local m_spBack = cc.Sprite:createWithTexture(tex, rect)
    m_spBack:setPosition(cardSize.width * 0.5, cardSize.height * 0.5);
    m_spBack:setScale(CARD_SCALE)
    self:addChild(m_spBack);
    self.m_spBack = m_spBack;

    local spineSp = sp.SkeletonAnimation:create("animation/fire/skeleton.json", "animation/fire/skeleton.atlas", 1)
    spineSp:setPosition(self.m_spBack:getContentSize().width/2,self.m_spBack:getContentSize().height/2)
    spineSp:setAnimation(0, "animation", true)
    spineSp:addTo(self.m_spBack)
    spineSp:setScale(0.9)

end

function CardSprite:createCardFront()
	local cardSize = self:getContentSize()
	local tex = cc.Director:getInstance():getTextureCache():getTextureForKey(self.cardFile)
	if nil == tex then
		tex = cc.Director:getInstance():getTextureCache():addImage(self.cardFile)
	end

	local rect = cc.rect((self.cardValue - 1) * CARD_WIDTH, self.cardColor * CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
	if 0 ~= self.cardData then
		rect = cc.rect((self.cardValue - 1) * CARD_WIDTH, self.cardColor * CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
	else
		rect = cc.rect(0 * CARD_WIDTH, 4 * CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
		-- if self.m_spBack then
		-- 	self.m_spBack:initWithTexture(tex, rect)
		-- else
		-- 	self:createBack()
		-- end
		-- return
	end

	if self._card then
		self._card:initWithTexture(tex, rect)
		return
	end

	local m_cardFront = cc.Sprite:createWithTexture(tex, rect)
	m_cardFront:setPosition(cardSize.width*0.5, cardSize.height*0.5)
	m_cardFront:setScale(CARD_SCALE)
	m_cardFront:addTo(self)
	self._card = m_cardFront
end

function CardSprite:openCardAnimation(index,isHuWin)
	if not self._card or not self.m_spBack then
		return
	end
	self._card:setFlipX(true)
	local backSeq = cc.Sequence:create(cc.DelayTime:create(0.1),cc.Hide:create(),cc.DelayTime:create(0.1),cc.Hide:create());
	local backScale = cc.ScaleTo:create(0.1, -0.7, 0.7);
	local backSpawn = cc.Spawn:create(backSeq,backScale);
	self.m_spBack:runAction(backSpawn);

	local frontSeq = cc.Sequence:create(cc.DelayTime:create(0.1),cc.Show:create(),cc.DelayTime:create(0.1),cc.Show:create());
	local frontScale = cc.Sequence:create(cc.ScaleTo:create(0.1, -1, 1)) 
	local frontSpawn = cc.Spawn:create(frontSeq,frontScale);
	self._card:runAction(cc.Sequence:create (frontSpawn, cc.CallFunc:create(function()
		local str = ""
		if index == 1 then
			str = string.format("lhb_p_%d.mp3", self.cardValue)
		elseif index == 2 then 
			if isHuWin then 
				str = string.format("sound-woman-win%d.mp3", self.cardValue)
			else
				str = string.format("sound-woman-lose%d.mp3", self.cardValue)
			end
		end
		ExternalFun.playSoundEffect(str)
	end)));
	ExternalFun.playSoundEffect("sound-fanpai.mp3")
end

return CardSprite	