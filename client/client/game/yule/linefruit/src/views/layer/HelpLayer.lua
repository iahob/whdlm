local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local HelpLayer = class("HelpLayer", cc.Layer)

HelpLayer.BT_LEFT = 1
HelpLayer.BT_RIGHT = 2
HelpLayer.BT_CLOSE = 3

--构造
function HelpLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("HelpLayer/HelpLayer.csb", self)
    self._csbNode:setPosition(0,0)  
    self._csbNode = self._csbNode:getChildByName("Node")

    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender)
        end
    end
    --背景
    local sp_bg = self._csbNode:getChildByName("sprite_bg")
    self.m_spBg = sp_bg

    --关闭按钮
--    local btn = self._csbNode:getChildByName("Button_close")
--    btn:setTag(HelpLayer.BT_CLOSE)
--    btn:addTouchEventListener(function (ref, eventType)
--        if eventType == ccui.TouchEventType.ended then
--            ExternalFun.playClickEffect()
--            self:removeFromParent()
--        end
--    end)

    --左
    self.m_btnLeft = self._csbNode:getChildByName("Button_left")
    self.m_btnLeft:setTag(HelpLayer.BT_LEFT)
    self.m_btnLeft:addTouchEventListener(cbtlistener)

    --右
    self.m_btnRight = self._csbNode:getChildByName("Button_right")
    self.m_btnRight:setTag(HelpLayer.BT_RIGHT)
    self.m_btnRight:addTouchEventListener(cbtlistener)

    self.m_image = {}
    self.m_image[1] = self._csbNode:getChildByName("Image_bouns")
    self.m_image[2] = self._csbNode:getChildByName("Image_rabbit")
    self.m_image[3] = self._csbNode:getChildByName("Image_rule")

    self.m_image[1]:setVisible(true)
    self.m_image[2]:setVisible(false)
    self.m_image[3]:setVisible(false)

    self.index = 1

    self:setScale(720/750)
end

function HelpLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function HelpLayer:OnButtonClickedEvent(tag, sender)
    self.m_image[1]:setVisible(false)
    self.m_image[2]:setVisible(false)
    self.m_image[3]:setVisible(false)
    if HelpLayer.BT_LEFT == tag then
        if self.index > 1 then
            self.index = self.index - 1
        end
        self.m_image[self.index]:setVisible(true)
    elseif HelpLayer.BT_RIGHT == tag then
        if self.index < 3 then
            self.index = self.index + 1
        end
        self.m_image[self.index]:setVisible(true)
    end
--    if self.index == 1 then
--        self.m_btnLeft:setVisible(false)
--        self.m_btnRight:setVisible(true)
--    elseif self.index == 3 then
--        self.m_btnRight:setVisible(false)
--        self.m_btnLeft:setVisible(true)
--    else
--        self.m_btnRight:setVisible(true)
--        self.m_btnLeft:setVisible(true)
--    end
end

--触摸回调
function HelpLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:showLayer(false)
    end
    return not self:isVisible()
end

function HelpLayer:onTouchEnded(touch, event)
    self:removeFromParent()
end

return HelpLayer