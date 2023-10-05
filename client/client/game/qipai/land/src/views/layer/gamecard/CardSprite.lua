--
-- Author: zhong
-- Date: 2016-06-27 11:36:40
--
local CardSprite = class("CardSprite", cc.Sprite);

--纹理宽高
-- local CARD_WIDTH = 158
-- local CARD_HEIGHT = 208
local CARD_WIDTH = 137
local CARD_HEIGHT = 185
local BACK_Z_ORDER = 2

------
--set/get
function CardSprite:setDispatched( var )
	self.m_bDispatched = var;
end

function CardSprite:getDispatched(  )
	if nil ~= self.m_bDispatched then
		return self.m_bDispatched;
	end
	return false;
end

function CardSprite:getCardData()
	return self.m_cardData
end

--拖动选择
function CardSprite:setCardDragSelect( var )
	self.m_bDragSelect = var
end

function CardSprite:getCardDragSelect()
	return self.m_bDragSelect
end

--弹出
function CardSprite:setCardShoot( var )
	self.m_bShoot = var
end

function CardSprite:getCardShoot()
	return self.m_bShoot
end
------

function CardSprite:ctor()
	self.m_cardData = 0
	self.m_cardValue = 0
	self.m_cardColor = 0
	self.m_bDispatched = false
	self.m_bDragSelect = false
	self.m_bShoot = false
	self.m_nCardWidth = 0
	self.m_nCardHeight = 0

	self.m_spLandFlag = nil
	self.m_strLandFlagFrameName = ""
	self.m_bIsLandCard = false
end

--创建卡牌
function CardSprite:createCard( cbCardData, tagParam )
	local sp = CardSprite.new();
	tagParam = tagParam or {}
	sp.m_nCardWidth = tagParam._width or CARD_WIDTH
	sp.m_nCardHeight = tagParam._height or CARD_HEIGHT
	sp.m_strCardFile = tagParam._file or "game/card.png"
	sp.m_strLandFlagFrameName = tagParam._landFlagFrameName or "land_cardlander_flag.png"

	local tex = cc.Director:getInstance():getTextureCache():getTextureForKey(sp.m_strCardFile);
	-- 避免纹理释放
	if nil == tex then
		local tab = {}
		tab.msg = "卡牌纹理异常"
		tab.texfile = sp.m_strCardFile
		local cachemsg = cjson.encode(tab) or ""
		local targetPlatform = cc.Application:getInstance():getTargetPlatform()
		if cc.PLATFORM_OS_WINDOWS == targetPlatform then
			LogAsset:getInstance():logData(cachemsg,true)
		else
			buglyReportLuaException(cachemsg, debug.traceback())
		end
		tex = cc.Director:getInstance():getTextureCache():addImage(sp.m_strCardFile)
	end
	if nil ~= sp and nil ~= tex and sp:initWithTexture(tex, tex:getContentSize()) then
		sp.m_cardData = cbCardData;
		sp.m_cardValue = yl.POKER_VALUE[cbCardData] --math.mod(cbCardData, 16)--bit:_and(cbCardData, 0x0F)
		sp.m_cardColor = yl.CARD_COLOR[cbCardData] --math.floor(cbCardData / 16)--bit:_rshift(bit:_and(cbCardData, 0xF0), 4)

		sp:updateSprite();
		--扑克背面
		sp:createBack();

		return sp;
	end
	return nil;
end

--设置卡牌数值
function CardSprite:setCardValue( cbCardData )
	self.m_cardData = cbCardData;
	self.m_cardValue = yl.POKER_VALUE[cbCardData]  --math.mod(cbCardData, 16) --bit:_and(cbCardData, 0x0F)
	self.m_cardColor = yl.CARD_COLOR[cbCardData]  --math.floor(cbCardData / 16) --bit:_rshift(bit:_and(cbCardData, 0xF0), 4)

	self:updateSprite();
end

--更新纹理资源
function CardSprite:updateSprite(  )
	local m_cardData = self.m_cardData
	local m_cardValue = self.m_cardValue or 0
	local m_cardColor = self.m_cardColor or 0
	local c_width = self.m_nCardWidth
	local c_height = self.m_nCardHeight

	self:setTag(m_cardData);

	local rect = cc.rect((m_cardValue - 1) * c_width, m_cardColor * c_height, c_width, c_height);
	if 0 ~= m_cardData then
		rect = cc.rect((m_cardValue - 1) * c_width, m_cardColor * c_height, c_width, c_height);
		if 0x4F == m_cardData then
			rect = cc.rect(0, 4 * c_height, c_width, c_height);
		elseif 0x4E == m_cardData then
			rect = cc.rect(c_width, 4 * c_height, c_width, c_height);
		end
	else
		--使用背面纹理区域
		rect = cc.rect(2 * c_width, 4 * c_height, c_width, c_height);
	end
	self:setTextureRect(rect);
end

--显示扑克背面
function CardSprite:showCardBack( var )
	if nil ~= self.m_spBack then
		self.m_spBack:setVisible(var);
	end	
end

--扑克选择效果
function CardSprite:showSelectEffect(bSelect)
	local c_width = self.m_nCardWidth
	local c_height = self.m_nCardHeight
	if nil == self.m_pMask then
		self.m_pMask = cc.Sprite:create("game/card.png")
		if nil ~= self.m_pMask then
			self.m_pMask:setColor(cc.BLACK)
			self.m_pMask:setOpacity(100)
			self.m_pMask:setTextureRect(cc.rect(2 * c_width, 4 * c_height, c_width, c_height))
			self.m_pMask:setPosition(c_width * 0.5, c_height * 0.5)
			self:addChild(self.m_pMask)
		end
	end

	if nil ~= self.m_pMask then
		self.m_pMask:setVisible(bSelect)
	end	
end

-- 地主标识
function CardSprite:showLandFlag( bLand )
	if bLand then
		if nil == self.m_spLandFlag then
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(self.m_strLandFlagFrameName)
			if nil ~= frame then
				self.m_spLandFlag = cc.Sprite:createWithSpriteFrame(frame)
				self.m_spLandFlag:setAnchorPoint(cc.p(1.0, 1.0))
				self.m_spLandFlag:setPosition(cc.p(144, 185))
				self:addChild(self.m_spLandFlag)
			end
		end
	end
	if nil ~= self.m_spLandFlag then
		self.m_spLandFlag:setVisible(bLand)
	end
	self.m_bIsLandCard = bLand
end

--创建背面
function CardSprite:createBack( )
	local c_width = self.m_nCardWidth
	local c_height = self.m_nCardHeight

	local tex = cc.Director:getInstance():getTextureCache():getTextureForKey(self.m_strCardFile);
	--纹理区域
	local rect = cc.rect(2 * c_width, 4 * c_height, c_width, c_height);

	local cardSize = self:getContentSize();
    local m_spBack = cc.Sprite:createWithTexture(tex, rect);
    m_spBack:setPosition(cardSize.width * 0.5, cardSize.height * 0.5);
    m_spBack:setVisible(false);
    self:addChild(m_spBack);
    m_spBack:setLocalZOrder(BACK_Z_ORDER);
    self.m_spBack = m_spBack;
end

return CardSprite;