local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local RecordLayer = class("RecordLayer", cc.Layer)

--构造
function RecordLayer:ctor(record)
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("record/RecordLayer.csb", self)
    -- self._csbNode:setPosition(0,0);

    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          -- self:onButtonClickedEvent(sender:getTag(), sender)
          self:removeFromParent()
      end
    end

    local close_btn = self._csbNode:getChildByName("Button_15");
    close_btn:addTouchEventListener(btnEvent);

    self.m_spBg = self._csbNode:getChildByName("bg_2")

    local recordInfoKey = {
        [1] = "l5KBet",      
        [2] = "lRSBet",      
        [3] = "lSFBet",      
        [4] = "l4KBet",      
        [5] = "lFHBet" ,     
        [6] = "lFLBet",      
        [7] = "lSTBet",      
        [8] = "l3KBet",      
        [9] = "l2PBet" ,     
        [10] = "l1PBet",    
        [11] = "lInvalidBet", 
        [12] = "lPlayTotal",  
    }
    for i = 1, 12 do
        local txt = self._csbNode:getChildByName("AtlasLabel_"..i)
        txt:setString(record[recordInfoKey[i]])
    end

end


--触摸回调
function RecordLayer:onTouchBegan(touch, event)
    return true
end

function RecordLayer:onTouchEnded(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:removeFromParent();
    end
end

return RecordLayer