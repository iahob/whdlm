local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local HelpLayer = class("HelpLayer", function (  )
    local colorLayer = cc.LayerColor:create(cc.c4b(0,0,0,100))
    return colorLayer
end)

HelpLayer.BT_RULE = 1
HelpLayer.BT_BOUNS = 2
HelpLayer.BT_CLOSE = 3

--构造
function HelpLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("HelpLayer/HelpLayer.csb", self)
    self._csbNode:setPosition(appdf.WIDTH/2,appdf.HEIGHT/2 )  
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
    local btn = self._csbNode:getChildByName("Button_close")
    btn:setTag(HelpLayer.BT_CLOSE)
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            self:removeFromParent()
        end
    end)

    --玩法
    self.m_btnRule = self._csbNode:getChildByName("Button_rule")
    self.m_btnRule:setTag(HelpLayer.BT_RULE)
    self.m_btnRule:addTouchEventListener(cbtlistener)

    --奖励
    self.m_btnBouns = self._csbNode:getChildByName("Button_bouns")
    self.m_btnBouns:setTag(HelpLayer.BT_BOUNS)
    self.m_btnBouns:addTouchEventListener(cbtlistener)

    self.scrollView = self._csbNode:getChildByName("ScrollView")
    self.scrollView:setScrollBarWidth(0)

    self.m_imRule = self.scrollView:getChildByName("Image_rule")
    self.m_imBouns = self.scrollView:getChildByName("Image_bouns")

    self.m_btnRule:loadTextureNormal("HelpLayer/btn_rule_1.png")
end

function HelpLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function HelpLayer:OnButtonClickedEvent(tag, sender)
    self.scrollView:jumpToTop()
    if HelpLayer.BT_RULE == tag then
        sender:loadTextureNormal("HelpLayer/btn_rule_1.png")
        self.m_btnBouns:loadTextureNormal("HelpLayer/btn_bouns_0.png")
        self.m_imRule:setVisible(true)
        self.m_imBouns:setVisible(false)
    elseif HelpLayer.BT_BOUNS == tag then
        sender:loadTextureNormal("HelpLayer/btn_bouns_1.png")
        self.m_btnRule:loadTextureNormal("HelpLayer/btn_rule_0.png")
        self.m_imRule:setVisible(false)
        self.m_imBouns:setVisible(true)
    end
end

--触摸回调
function HelpLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
--        self:removeFromParent()
        self:showLayer(false)
    end
    return self:isVisible()
end

return HelpLayer