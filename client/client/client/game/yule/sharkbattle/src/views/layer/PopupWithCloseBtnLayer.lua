local module_pre = "game.yule.sharkbattle.src"

local PopupLayer = appdf.req(module_pre .. ".views.layer.PopupLayer")

--游戏说明
local PopupWithCloseBtnLayer = class("PopupWithCloseBtnLayer",PopupLayer)

function PopupWithCloseBtnLayer:ctor(popImgFile)
	PopupWithCloseBtnLayer.super.ctor(self)
	self.popImg=self.popImg or display.newSprite(popImgFile)
	assert(self.popImg)
	self.popImg:setPosition(display.center) 
	self.closeBtn=self.closeBtn or ccui.Button:create("close.png","closedown.png")
	self:addChild(self.popImg)
	self.popImg:addChild(self.closeBtn)
	local imgSize=self.popImg:getContentSize()
	local btnSize=self.closeBtn:getContentSize()
	self.closeBtn:setPosition(imgSize.width-btnSize.width/2,imgSize.height-btnSize.height/2)
	local this=self
	self.closeBtn:addClickEventListener(function() this:removeFromParent() end)
end

return PopupWithCloseBtnLayer