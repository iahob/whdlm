local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")

local CardNode = class("CardNode", cc.Node);

function CardNode:ctor(startPos)
	ExternalFun.registerNodeEvent(self);

    -- 加载csb资源
    local csbNode = ExternalFun.loadCSB("card/CardRight.csb",self);
    self.m_csbNode = csbNode;

    self.card_value = csbNode:getChildByName("card_value");
    self.card_back = csbNode:getChildByName("card_back");

    self._touched = false;     --是否可点击
    self._selected = false;    --是否选中牌
    self.startPos = startPos;  --卡牌初始位置
    self.cardIsLeft = false;   --卡牌横放
    self.isMoveing = false;    --正在移动
end

--设置卡牌数值
function CardNode:setCardValue(cardValue)
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(cardValue ..".png");
	if nil ~= frame then
		self.card_value:setSpriteFrame(frame);
	end
end

--设置卡牌数值
function CardNode:setCardLeftShow(cardValue)
	self.cardIsLeft = true;
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("card_back_2.png");
	if nil ~= frame then
		self.card_back:setSpriteFrame(frame);
	end

	local frame1 = cc.SpriteFrameCache:getInstance():getSpriteFrame(cardValue .."_1.png");
	if nil ~= frame1 then
		self.card_value:setSpriteFrame(frame1);
	end
end

--是否显示牌背
function CardNode:showCardBack(isShowBack)
	self.card_value:setVisible(not isShowBack);
end

function CardNode:setTouched(isTouched)
	self._touched = isTouched;
	if not self.card_value or not self.card_value:isVisible() then
		return;
	end
	if isTouched then
		self.card_value:setColor(cc.c3b(200, 200, 200));
	else
		self.card_value:setColor(cc.c3b(255, 255, 255));
	end
end

function CardNode:getTouched()
	return self._touched;
end

function CardNode:setSelected(isSelected, isMove)
	self._selected = isSelected;
	--重置牌选中状态
	self:stopAllActions();
	if isSelected then
		self:setPosition(cc.p(self.startPos.x, self.startPos.y + 40));
	else
		self:setPosition(cc.p(self.startPos.x, self.startPos.y));
	end
	-- dump(self.startPos,"---------self.startPos---------",6)

	if isMove then
		if isSelected then
			self:runAction(cc.MoveTo:create(0.2, cc.p(self.startPos.x, self.startPos.y + 40)));
		else
			self:runAction(cc.MoveTo:create(0.2, cc.p(self.startPos.x, self.startPos.y)));
		end
	end
end

function CardNode:getSelected()
	return self._selected;
end

--翻牌效果
function CardNode:openCardAnimation()
	-- 翻牌
	if not self._card or not self.m_spBack then
		return;
	end

	self._card:setFlipX(true);

	local delayTime = 0.15;
	local backSeq = cc.Sequence:create(cc.DelayTime:create(delayTime),cc.Hide:create(),cc.DelayTime:create(delayTime),cc.Hide:create());
	local backScale = cc.ScaleTo:create(delayTime, -1, 1);
	local backSpawn = cc.Spawn:create(backSeq,backScale);
	self.m_spBack:runAction(backSpawn);

	local frontSeq = cc.Sequence:create(cc.DelayTime:create(delayTime),cc.Show:create(),cc.DelayTime:create(delayTime),cc.Show:create());
	local frontScale = cc.ScaleTo:create(delayTime, -1, 1);
	local frontSpawn = cc.Spawn:create(frontSeq,frontScale);
	self._card:runAction(frontSpawn);
end

return CardNode;