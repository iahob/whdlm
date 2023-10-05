local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local HelpLayer = class("HelpLayer", cc.Layer)

HelpLayer.BT_MASK = 1
HelpLayer.BT_CLOSE = 2

--构造
function HelpLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("rule/RuleLayer.csb", self);
    -- csbNode:setPosition(0,0);
    self._csbNode = csbNode;

    --回调方法
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
        ExternalFun.playClickEffect()
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    self.btn_close = self._csbNode:getChildByName("Button_1")
    self.btn_close:setTag(HelpLayer.BT_CLOSE)
    self.btn_close:addTouchEventListener(btnEvent)
end

function HelpLayer:onExit()
end


--按钮回调方法
function HelpLayer:onButtonClickedEvent(tag, sender)
    
    if HelpLayer.BT_MASK == tag or HelpLayer.BT_CLOSE == tag then
        self:removeFromParent();
    end
end

--触摸回调
function HelpLayer:onTouchBegan(touch, event)
    return true
end

function HelpLayer:onTouchEnded(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self._csbNode:getChildByName("bg_2")
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        -- local fadeout = cc.FadeOut:create(0.5)
        -- local call = cc.CallFunc:create(function( )
        --     self:removeFromParent()
        -- end)
        -- self._csbNode:runAction(cc.Sequence:create(fadeout, call))

        self:removeFromParent();
    end

end

return HelpLayer