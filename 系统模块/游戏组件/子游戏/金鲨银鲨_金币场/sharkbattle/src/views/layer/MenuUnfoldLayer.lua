local module_pre = "game.yule.sharkbattle.src"
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local PopupWithCloseBtnLayer=appdf.req(module_pre .. ".views.layer.PopupWithCloseBtnLayer")
local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local BankLayer= appdf.req(module_pre .. ".views.layer.BankLayer")

local MenuUnfoldLayer=class("MenuPopLayer",cc.Layer)
local popZorder=1
function MenuUnfoldLayer:ctor(scene,menuX,menuY,menuHeight,menuBtn)
	self.menuBtn=menuBtn
	local shadow=cc.Sprite:create("menushadow.png")
	introBtn=ccui.Button:create("intro.png","introdown.png")
	setBtn=ccui.Button:create("setting.png","settingdown.png")
	bankBtn=ccui.Button:create("bankbtn.png","bankbtndown.png")

	
	local offy=(shadow:getContentSize()).height/2-menuHeight/2 
	shadow:setPosition(menuX, menuY-offy)

	introBtn:setPosition(menuX,menuY-menuHeight)
	setBtn:setPosition(menuX,menuY-2*menuHeight)
	bankBtn:setPosition(menuX,menuY-3*menuHeight)

	introBtn:addClickEventListener(function() self:removeSelf() scene:addChild(PopupWithCloseBtnLayer:create("introimg.png"),popZorder) end)
	setBtn:addClickEventListener(function() self:removeSelf() scene:addChild(SettingLayer:create("settingbg.jpg"),popZorder)  end)
	
	bankBtn:addClickEventListener(function() 
		self:removeSelf() 
		if 0 == GlobalUserItem.cbInsureEnabled then
	   	 	showToast(scene, "初次使用，请先开通银行！", 1)
	    	return 
		end
		scene._bankLayer=BankLayer:create(scene) 
		scene:addChild(scene._bankLayer,popZorder) 
	end )

	self:addChild(shadow)
	self:addChild(introBtn)
	self:addChild(setBtn)
	self:addChild(bankBtn)

	ExternalFun.registerTouchEvent(self,true)
end

function MenuUnfoldLayer:onExit()
	self.menuBtn:loadTextureNormal("menuunfold.png")
end

function MenuUnfoldLayer:onTouchBegan()
	return true
end

function MenuUnfoldLayer:onTouchEnded()
	self.menuBtn:loadTextureNormal("menuunfold.png")
	self:removeFromParent()
end

return MenuUnfoldLayer