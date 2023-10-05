local uiBase =
{
	zorder = 1,
	widget = nil,
	widgetName = nil,
	parent = nil,
	needMoveInAction = false,
}

function uiBase:ctor(o)
	local o = o or {}
	self.__index = self
	setmetatable(o, self)
	return o
end

function uiBase:show()
	if self.widget and not tolua.isnull(self.widget) then
		if self.needMoveInAction then
			self:moveIn()
		end
		self.widget:setVisible(true)
	end
end

function uiBase:hide()
	if self.widget and not tolua.isnull(self.widget) then
		self.widget:setVisible(false)
	end
end

function uiBase:onShow()
end

function uiBase:onHide()
	if type(self.onCloseCallback) == "function" then
        self.onCloseCallback()
    end
end

function uiBase:isActive()
	return self.widget and not tolua.isnull(self.widget)
end

function uiBase:destroy()
	if self.widget and not tolua.isnull(self.widget) then
		self.widget:removeFromParent()
		self:onHide()
	end
	self.widget = nil
end

function uiBase:onEnter()
end

function uiBase:onExit()
end

function uiBase:moveIn()
    local actionNode = self.widget
    actionNode:setAnchorPoint(cc.p(0.5, 0.5))
    actionNode:setPosition(display.width*0.5, display.height*0.5)

    actionNode:setScale(0.8)
    actionNode:setOpacity(0)

    local scale = cc.ScaleTo:create(0.2, 1)
    local fadeIn = cc.FadeIn:create(0.4)
    local callback = cc.CallFunc:create(function() self:onShow() end)
    local show = cc.Spawn:create(scale, fadeIn, callback)

    actionNode:stopAllActions()
    actionNode:runAction(show)
end

function uiBase:registerNodeEvent()
	local function onNodeEvent(event)
	    if event == "enter" then
	        self:onEnter()
	    elseif event == "exit" then
	        self:onExit()
	    end
	end
    if self.widget and not tolua.isnull(self.widget) then
    	self.widget:registerScriptHandler(onNodeEvent)
    end
end

return uiBase