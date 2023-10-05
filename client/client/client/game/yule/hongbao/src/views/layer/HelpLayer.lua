--
-- Author: luo
-- Date: 2016年12月30日 15:18:32
--
--设置界面
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local HelpLayer = class("HelpLayer", cc.Layer)

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "SLIDER_BG",            -- 背景滑动
    "SLIDER_EFFECT",        -- 音效滑动
    "BTN_SLIENCE_OPEN",     -- 开启静音
    "BTN_SLIENCE_CLOSE",    -- 关闭静音
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
--构造
function HelpLayer:ctor( verstr )
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("help/helpLayer.csb", self)
    self._csbNode:setPosition(0,0)


    --关闭按钮
    local btn = self._csbNode:getChildByName("Button_1")
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            self:removeFromParent()
        end
    end)

end

--触摸回调
function HelpLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function HelpLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self._csbNode:getChildByName("bg_1")
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:removeFromParent()
    end
end

return HelpLayer