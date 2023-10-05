local HelpLayer = class("HelpLayer", cc.Node)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
HelpLayer.MASK = 1
HelpLayer.EXIT = 2
function HelpLayer:ctor()
    self:loadResource()
end

function HelpLayer:loadResource()
	self.csbNode = ExternalFun.loadCSB("rule.csb", self)

	local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    local btn = self.csbNode:getChildByName("mask")
    btn:setTag(HelpLayer.MASK)
    btn:addTouchEventListener(btnEvent)
end

function HelpLayer:onButtonClickedEvent(tag, ref)
	if tag == HelpLayer.MASK or tag == HelpLayer.EXIT then
		self:removeFromParent()
	end
end



return HelpLayer