local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local HelpLayer = class("HelpLayer", cc.Layer)

HelpLayer.BT_MASK = 1
HelpLayer.BT_CLOSE = 2

--构造
function HelpLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("help/HelpLayer.csb", self);
    csbNode:setPosition(0,0);
    self._csbNode = csbNode;

    --回调方法
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(HelpLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    --左
    local content = self._csbNode:getChildByName("panel");

    --音效
    self.btn_close = self._csbNode:getChildByName("btn_close")
    self.btn_close:setTag(HelpLayer.BT_CLOSE)
    self.btn_close:addTouchEventListener(btnEvent)
end

function HelpLayer:onExit()
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:removeEventListener(self.listener)
end

function HelpLayer:onEnterTransitionFinish()
    self:registerTouch()
end

function HelpLayer:registerTouch()
    -- local function onTouchBegan( touch, event )
    --     return self:isVisible()
    -- end

    -- local function onTouchEnded( touch, event )
    --     local pos = touch:getLocation();
    --     local m_spBg = self.m_spBg
    --     pos = m_spBg:convertToNodeSpace(pos)
    --     local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    --     if false == cc.rectContainsPoint(rec, pos) then
    --         self:setVisible(false)
    --     end        
    -- end

    -- local listener = cc.EventListenerTouchOneByOne:create();
    -- listener:setSwallowTouches(true)
    -- self.listener = listener;
    -- listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN );
    -- listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED );
    -- local eventDispatcher = self:getEventDispatcher();
    -- eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self);
end

function HelpLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function HelpLayer:onButtonClickedEvent(tag, sender)
    
    if HelpLayer.BT_MASK == tag or HelpLayer.BT_CLOSE == tag then
        self:removeFromParent();
    end
end

--触摸回调
function HelpLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
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

function HelpLayer:onTouchEnded(touch, event)

end

return HelpLayer