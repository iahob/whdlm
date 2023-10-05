local module_pre = "game.yule.animalbattle.src"
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local cmd = appdf.req(module_pre .. ".models.CMD_Game")

local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local BankLayer=appdf.req(module_pre .. ".views.layer.BankLayer")
local HelpLayer=appdf.req(module_pre .. ".views.layer.HelpLayer")
local PlayerlistLayer=appdf.req(module_pre .. ".views.layer.PlayerlistLayer")

local MenuLayer=class("MenuLayer",cc.Layer)
local popZorder=10
function MenuLayer:ctor(scene,menuX,menuY)
	

	self.colorLayer = cc.LayerColor:create(cc.c4b(0, 0, 0, 125))
		:setContentSize(display.width, display.height)
		:addTo(self)
	self.colorLayer:setTouchEnabled(true)
	self.colorLayer:registerScriptTouchHandler(function(eventType, x, y)
		return self:onTouch(eventType, x, y)
	end)

	self.bg_sp = cc.Sprite:create("menu_box.png")
	self:addChild(self.bg_sp)
	self.bg_sp:setPosition(1220,530)

	local introBtn=ccui.Button:create("introbtn.png","introbtn.png")
	local setBtn=ccui.Button:create("setbtn.png","setbtn.png")
	local bankBtn=ccui.Button:create("bankbtn.png","bankbtn.png")

	-- local h=70
	bankBtn:setPosition(111,225)
	introBtn:setPosition(111,140)
	setBtn:setPosition(111,60)

	self.bg_sp:addChild(bankBtn)
	self.bg_sp:addChild(introBtn)
	self.bg_sp:addChild(setBtn)

	introBtn:addClickEventListener(function() 
		ExternalFun.playSoundEffect("dianji.mp3")
		self:removeSelf()  
		scene:addChild(HelpLayer:create(scene),popZorder) 
	end)
	setBtn:addClickEventListener(function()  
		ExternalFun.playSoundEffect("dianji.mp3")
		self:removeSelf() 
		scene:addChild(SettingLayer:create(scene),popZorder)  
	end)
	-- playerBtn:addClickEventListener(function() self:removeSelf() scene:addChild(PlayerlistLayer:create(scene:getPlayerList()),popZorder) end )

	bankBtn:addClickEventListener(function() 
		ExternalFun.playSoundEffect("dianji.mp3")
		self:removeSelf() 
		if 0 == GlobalUserItem.cbInsureEnabled then
	   	 	showToast(scene, "初次使用，请先开通银行！", 1)
	    	return 
		end
		scene._bankLayer=BankLayer:create(scene) 
		scene:addChild(scene._bankLayer,popZorder)
	end )
    -- 是否开启银行服务
    local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
    bankBtn:setVisible(enableBank)



	-- ExternalFun.registerTouchEvent(self,true)
	-- print("bankBtn:setPosition: ",bankBtn:getPosition())
	-- print("setBtn:setPosition: ",setBtn:getPosition())
end

-- function MenuLayer:onTouchBegan()
-- 	return true
-- end

-- function MenuLayer:onTouchEnded()
-- 	self:removeFromParent()
-- end

function MenuLayer:onTouch(eventType, x, y)
	if eventType == "began" then
		return true
	end

	local pos = cc.p(x, y)
    local rectLayerBg = self.bg_sp:getBoundingBox()
    if not cc.rectContainsPoint(rectLayerBg, pos) then
    	self:hideLayer()
    end

    return true
end


function MenuLayer:hideLayer()
	ExternalFun.playSoundEffect("guanbi.mp3")
	self.colorLayer:setTouchEnabled(false)
	self:setVisible(false)
	self:removeFromParent()
end


return MenuLayer

