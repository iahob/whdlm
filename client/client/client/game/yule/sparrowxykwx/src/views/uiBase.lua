local uiBase =
{
	zorder = 1,
	widget = nil,
	widgetName = nil,
	parent = nil,
}

function uiBase:ctor(o)
	local o = o or {}
	self.__index = self
	setmetatable(o, self)
	return o
end

function uiBase:show()
	if self.widget and not tolua.isnull(self.widget) then
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