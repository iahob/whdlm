


local OperateLayer = class("OperateLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)

local ExternalFun  = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ShrimpConfig = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.ShrimpConfig")

local baoziTag = 10001
local nuodanTag = 10003
local nuochuanTag = 10004
local lianchuanTag = 10002

function OperateLayer:ctor(scene,config)
	-- body
	self.showId = 0
	self._scene = scene
	self.config = config
	self.btnCount = 0

	self.minMoney = 0
	self.maxMoney = 0
	local function onNodeEvent(event)
        if event == "enter" then
            -- self:onEnter()
        elseif event == "exit" then
            self:onExit()
        end
    end
    self:registerScriptHandler(onNodeEvent)

	self:initUI()
end

function OperateLayer:onExit( )
	-- body
	if self.btnScheadule then 
    	cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.btnScheadule)
    	self.btnScheadule = nil
    end
end

function OperateLayer:initUI()
	-- body

	local rootLayer, csbNode = ExternalFun.loadRootCSB("ui/OperateLayer.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode

   	self.room = self.m_csbNode:getChildByName("OperateLayer")
   	self.checkBtn = {}
   	for i=1,6 do
   		local btn = self.room:getChildByName("Button_"..i)
   		btn.id = i
   		btn.isCheck = false
   		btn:addTouchEventListener(handler(self,self.onCheckBtn))
   		self.checkBtn[#self.checkBtn+1] = btn
   	end

   	self.checkSp = {}
   	for i=1,6 do
   		local sp = self.room:getChildByName("Sprite_"..i)
   		sp.id = i
   		-- sp:setVisible(false)
   		self.checkSp[#self.checkSp+1] = sp
   	end

   	local baozi = self.room:getChildByName("baozi")
   	local nuodan = self.room:getChildByName("nuodan")
   	local nuochuan = self.room:getChildByName("nuochuan")
   	local lianchuan = self.room:getChildByName("lianchuan")

	baozi:setTag(baoziTag)
	nuodan:setTag(nuodanTag)
	nuochuan:setTag(nuochuanTag)
	lianchuan:setTag(lianchuanTag)

	baozi:setVisible(false)
	nuodan:setVisible(false)
	nuochuan:setVisible(false)
	lianchuan:setVisible(false)

   	self.baoZisp = baozi:getChildByName("sprite_1")
   	self.baoZisp:setVisible(false)

   	self.lianChuanSp_1 = lianchuan:getChildByName("sprite_1")  
   	self.lianChuanSp_2 = lianchuan:getChildByName("sprite_2")  
   	self.lianChuanSp_1:setVisible(false)
	self.lianChuanSp_2:setVisible(false)

   	self.nuoDanSp_1 = nuodan:getChildByName("sprite_1")  
   	self.nuoDanSp_2 = nuodan:getChildByName("sprite_2")  
   	self.nuoDanSp_1:setVisible(false)
	self.nuoDanSp_2:setVisible(false)

   	self.nuoChuanSp_1 = nuochuan:getChildByName("sprite_1")  
   	self.nuoChuanSp_2 = nuochuan:getChildByName("sprite_2")  
   	self.nuoChuanSp_3 = nuochuan:getChildByName("sprite_3")  
   	self.nuoChuanSp_4 = nuochuan:getChildByName("sprite_4")
   	self.nuoChuanSp_1:setVisible(false)
	self.nuoChuanSp_2:setVisible(false)
   	self.nuoChuanSp_3:setVisible(false)
	self.nuoChuanSp_4:setVisible(false)
	self.nuoChuanSp = {}
	self.nuoChuanSp[#self.nuoChuanSp+1] = self.nuoChuanSp_1
	self.nuoChuanSp[#self.nuoChuanSp+1] = self.nuoChuanSp_2
	self.nuoChuanSp[#self.nuoChuanSp+1] = self.nuoChuanSp_3
	self.nuoChuanSp[#self.nuoChuanSp+1] = self.nuoChuanSp_4


	self.addBtn = self.room:getChildByName("panel"):getChildByName("add")
	self.reduceBtn = self.room:getChildByName("panel"):getChildByName("reduce")

	self.addBtn:addClickEventListener(handler(self,self.onAddBtn))
	self.reduceBtn:addClickEventListener(handler(self,self.onReduceBtn))


	local slider = self.room:getChildByName("panel"):getChildByName("slider")
	self.sliderText = self.room:getChildByName("panel"):getChildByName("Text_2")
	self.sliderName = self.room:getChildByName("panel"):getChildByName("Sprite_1")

	-- Slider:create()
	-- slider:setMaximumAllowedValue(90)
	-- slider:setMinimumAllowedValue(10)

	self.cancel = self.room:getChildByName("panel"):getChildByName("cancel")
	self.yes = self.room:getChildByName("panel"):getChildByName("yes")

	self.yes:addClickEventListener(handler(self,self.onYes))
	self.cancel:addClickEventListener(handler(self,self.onHide))
	slider:onEvent(handler(self,self.sliderCallback))
	self.slider = slider

	if self.maxMoney <= 0 then 
		slider:setPercent(5)
		slider:setEnabled(false)
	end
	

	self.room:onTouch(function (event)
		-- body
		if event.name == "ended" then
			if self.showId == 4 then 
				for i=1,#self.checkBtn do
					self.checkBtn[i].isCheck = false
				end
				for i=1,#self.checkSp do
					self.checkSp[i]:removeAllChildren()
				end
				for i=1,#self.nuoChuanSp do
					self.nuoChuanSp[i].id = 0
					self.nuoChuanSp[i]:setVisible(false)
					self.btnCount = 0
				end

				self.minMoney = 0
				self.maxMoney = 0 
				self:setSliderPercent()

			end
		end

	end)
	-- self.myText = self.room:getChildByName("Text_9_1")
	-- self.allText = self.room:getChildByName("Text_9_2")

	-- self.myText:setString("0")
	-- self.allText:setString("0")
end

function OperateLayer:onHide()
	-- body
	if self.funcCall then self.funcCall() end
	self:removeFromParent()
end

function OperateLayer:onYes()
	-- body	
	if self.showId == 1 then 
		if self.baoZisp:isVisible() then
			local index = self.config[self.baoZisp.id]
			self._scene:sendGameBetInfo(5,index,self.minMoney,0)
			ExternalFun.playSoundEffect("betBaozi.mp3")
		else

		end
	end

	if self.showId == 2 then 

		if self.lianChuanSp_1:isVisible() and self.lianChuanSp_2:isVisible() then
			local index = ShrimpConfig.getChuanIndex(self.config[self.lianChuanSp_1.id],self.config[self.lianChuanSp_2.id])
			self._scene:sendGameBetInfo(2,index,self.minMoney,0)
			ExternalFun.playSoundEffect("betChuan.mp3")
		else

		end
	end

	if self.showId == 3 then 
		if self.nuoDanSp_1:isVisible() and self.nuoDanSp_2:isVisible() then
			local index1 = self.config[self.nuoDanSp_1.id]
			local index2 = self.config[self.nuoDanSp_2.id]
			self._scene:sendGameBetInfo(3,index2,self.minMoney,index1)
			ExternalFun.playSoundEffect("betNuoChuan.mp3")
		else

		end
	end

	if self.showId == 4 then
		if self.nuoChuanSp_1:isVisible() and self.nuoChuanSp_2:isVisible() 
			and self.nuoChuanSp_3:isVisible() and self.nuoChuanSp_4:isVisible() then
			ExternalFun.playSoundEffect("betNuoChuan.mp3")
			local index1 = ShrimpConfig.getChuanIndex(self.config[self.nuoChuanSp_1.id],self.config[self.nuoChuanSp_2.id])
			local index2 = ShrimpConfig.getChuanIndex(self.config[self.nuoChuanSp_3.id],self.config[self.nuoChuanSp_4.id])
			self._scene:sendGameBetInfo(4,index2,self.minMoney,index1)
		else

		end
	end

	self:onHide()
end

function OperateLayer:setExitCall(funcCall)
	-- body
	self.funcCall = funcCall
end

function OperateLayer:show(id,data)
	-- room

	local panel = self.room:getChildByTag(10000+id)
	panel:setVisible(true)
	self.showId = id
	self.data = data

	dump(self.data,"show",12)

	self.sliderName:setTexture("public/text_"..id..".png")

	self.slider:setPercent(5)
	self.sliderText:setString("0/0")

end

function OperateLayer:onCheckBtn(ref, tType) 
	-- self:buttonSound()

	if self.showId == 1 then 
		self:updataBaoZi(ref, tType)
	end

	if self.showId == 2 then 
		self:updataLianChuan(ref, tType)
	end

	if self.showId == 3 then 
		self:updataNuoDan(ref, tType)
	end

	if self.showId == 4 then
		self:uodataNuoChuan(ref, tType)
	end
end

function OperateLayer:removeCheckSpChildren()
	-- body
	for i=1,#self.checkSp do
		self.checkSp[i]:removeAllChildren()
	end

end

function OperateLayer:updataBaoZi( ref, tType )
	-- body
	if tType == ccui.TouchEventType.ended then
		self:removeCheckSpChildren()
		local sp = cc.Sprite:create("button/tab_hook.png")
		self.checkSp[ref.id]:addChild(sp)
		sp:setPosition(20,20)

		if self.baoZisp.id == ref.id then
			local isShow = self.baoZisp:isVisible()
			sp:setVisible(not isShow)
			self.baoZisp:setVisible(not isShow)
		else
			self.baoZisp:setVisible(true)
		end
		
		self.baoZisp:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[ref.id]..".png")
		self.baoZisp.id = ref.id

		if self.baoZisp:isVisible() then 
			local index = self.config[ref.id] + 1
			self.maxMoney = self.data[index] < self.data.l_UserLimit and self.data[index] or self.data.l_UserLimit
		else
			self.maxMoney = 0
		end

		self:setSliderPercent()
	end
end

function OperateLayer:updataLianChuan(ref, tType )
	-- body
	if tType == ccui.TouchEventType.ended then
		local btn = ref
		if self.checkSp[ref.id]:getChildByName("check") then 
			self.checkSp[ref.id]:removeChildByName("check")
		else
			local sp = cc.Sprite:create("button/tab_hook.png")
			self.checkSp[ref.id]:addChild(sp)
			sp:setPosition(20,20)
			sp:setName("check")
		end
		
		local tab = {}
		for i=1,#self.checkSp do
			if self.checkSp[i]:getChildByName("check") then 
				tab[#tab+1] = self.checkSp[i]
			end
		end
		local index = #tab


		if index == 0 then
			self.lianChuanSp_1:setVisible(false)
			self.lianChuanSp_2:setVisible(false)
		elseif index == 1 then 
			if self.lianChuanSp_1:isVisible() and self.lianChuanSp_2:isVisible() then 
				if self.lianChuanSp_1.id == ref.id then
					self.lianChuanSp_1.id = self.lianChuanSp_2.id
					self.lianChuanSp_1:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[self.lianChuanSp_2.id]..".png")
				else
					if self.checkSp[btn.id]:getChildByName("check") then
						self.lianChuanSp_1:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[ref.id]..".png")
					end
				end
			else
				self.lianChuanSp_1.id = ref.id
				if self.checkSp[btn.id]:getChildByName("check") then
					self.lianChuanSp_1:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
				end
			end

			self.lianChuanSp_1:setVisible(true)
			self.lianChuanSp_2:setVisible(false)

		elseif index == 2 then 
			self.lianChuanSp_1:setVisible(true)
			self.lianChuanSp_2:setVisible(true)
			self.lianChuanSp_2.id = btn.id
			self.lianChuanSp_2:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
		elseif index == 3 then
			for i=1,#tab do
				if tab[i].id == self.lianChuanSp_2.id then 
					tab[i]:removeAllChildren()
				end
			end
			self.lianChuanSp_2.id = btn.id
			self.lianChuanSp_2:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")

		else
			self.checkSp[btn.id]:removeAllChildren()
		end

		if self.lianChuanSp_1:isVisible() and self.lianChuanSp_2:isVisible() then 
			local id1 = self.lianChuanSp_1.id
			local id2 = self.lianChuanSp_2.id
			local index = ShrimpConfig.getChuanIndex(self.config[id1],self.config[id2]) + 1
			print("index == ",index)
			self.maxMoney = self.data[index] < self.data.l_UserLimit and self.data[index] or self.data.l_UserLimit
		else
			self.maxMoney = 0
		end
		self:setSliderPercent()

	end
end

function OperateLayer:updataNuoDan(ref, tType)
	-- body
	if tType == ccui.TouchEventType.ended then
		local btn = ref
		if self.checkSp[btn.id]:getChildByName("check") then 
			self.checkSp[btn.id]:removeChildByName("check")
		else
			local sp = cc.Sprite:create("button/tab_hook.png")
			self.checkSp[btn.id]:addChild(sp)
			sp:setPosition(20,20)
			sp:setName("check")
		end

		local tab = {}
		for i=1,#self.checkSp do
			if self.checkSp[i]:getChildByName("check") then 
				tab[#tab+1] = self.checkSp[i]
			end
		end
		local index = #tab
		if index == 0 then
			self.nuoDanSp_1:setVisible(false)
			self.nuoDanSp_2:setVisible(false)
		elseif index == 1 then 
			if self.nuoDanSp_1:isVisible() and self.nuoDanSp_2:isVisible() then 
				if self.nuoDanSp_1.id == btn.id then
					self.nuoDanSp_1.id = self.nuoDanSp_2.id
					self.nuoDanSp_1:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[self.nuoDanSp_2.id]..".png")
				else
					if self.checkSp[btn.id]:getChildByName("check") then
						self.nuoDanSp_1:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
					end
				end
			else
				self.nuoDanSp_1.id = btn.id
				if self.checkSp[btn.id]:getChildByName("check") then
					self.nuoDanSp_1:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
				end
			end

			self.nuoDanSp_1:setVisible(true)
			self.nuoDanSp_2:setVisible(false)

		elseif index == 2 then 
			self.nuoDanSp_1:setVisible(true)
			self.nuoDanSp_2:setVisible(true)
			self.nuoDanSp_2.id = btn.id

			self.nuoDanSp_2:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
		elseif index == 3 then
			-- self.nuoDanSp_2:setVisible(false)
			for i=1,#tab do
				if tab[i].id == self.nuoDanSp_2.id then 
					tab[i]:removeAllChildren()
				end
			end
			self.nuoDanSp_2.id = btn.id
			self.nuoDanSp_2:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
		else
			self.checkSp[btn.id]:removeAllChildren()
		end

		if self.nuoDanSp_1:isVisible() then 
			local id = self.nuoDanSp_1.id
			local index = self.config[id] + 1
			print("index == ",index)
			self.maxMoney = self.data[index] < self.data.l_UserLimit and self.data[index] or self.data.l_UserLimit
		else
			self.maxMoney = 0
		end

		self:setSliderPercent()
	end
end

function OperateLayer:addRedHook( index )
	self.checkSp[index]:removeChildByName("red")
	local sp = cc.Sprite:create("button/tab_hook.png")
	self.checkSp[index]:addChild(sp,1)
	sp:setPosition(20,20)
	sp:setName("red")

	local green = self.checkSp[index]:getChildByName("green")
	if green then 
		sp:setPositionX(15)
		green:setPosition(25,20)
	end
end

function OperateLayer:addGreenHook( index )
	-- body
	self.checkSp[index]:removeChildByName("green")
	local sp = cc.Sprite:create("button/tab_green.png")
	self.checkSp[index]:addChild(sp,2)
	sp:setName("green")
	sp:setPosition(20,20)

	local red = self.checkSp[index]:getChildByName("red")
	if red then 
		red:setPositionX(15)
		sp:setPosition(25,20)
	end
end

function OperateLayer:remveCheckHook(color,index)
	-- body
	self.checkSp[index]:removeChildByName(color)

	local red = self.checkSp[index]:getChildByName("red")
	local green = self.checkSp[index]:getChildByName("green")

	if redSp and green then 
		red:setPositionX(15)
		green:setPosition(25,14)
	elseif red and not green then
		red:setPosition(20,14)
	elseif not red and green then
		green:setPosition(20,14)
	end
end

function OperateLayer:uodataNuoChuan(ref, tType)
	-- body

	if tType == ccui.TouchEventType.ended then
		self.btnCount = self.btnCount + 1
		local btn = ref
		local index = self.btnCount%4
		local show_1 = self.nuoChuanSp_1:isVisible()
		local show_2 = self.nuoChuanSp_2:isVisible()
		local show_3 = self.nuoChuanSp_3:isVisible()
		local show_4 = self.nuoChuanSp_4:isVisible()
		local redSp = self.checkSp[btn.id]:getChildByName("red")
		local greenSp = self.checkSp[btn.id]:getChildByName("green")

		if redSp and greenSp then 
			self.btnCount = self.btnCount - 1
			return
		end

		if index == 1 then 
			if btn.id == self.nuoChuanSp_2.id then 
				self.btnCount = self.btnCount - 1 
				return 
			end
			if not show_1 then 
				self:addRedHook(btn.id)
				self.nuoChuanSp_1.id = btn.id
				self.nuoChuanSp_1:setVisible(true)
				self.nuoChuanSp_1:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
			else
				self:remveCheckHook("red",self.nuoChuanSp_1.id)
				self:addRedHook(btn.id)
				self.nuoChuanSp_1.id = btn.id
				self.nuoChuanSp_1:setVisible(true)
				self.nuoChuanSp_1:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
			end
		elseif index == 2 then 

			if btn.id == self.nuoChuanSp_1.id then 
				self.btnCount = self.btnCount - 1 
				return 
			end

			if not show_2 then 
				self:addRedHook(btn.id)
				self.nuoChuanSp_2.id = btn.id
				self.nuoChuanSp_2:setVisible(true)
				self.nuoChuanSp_2:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
			else
				self:remveCheckHook("red",self.nuoChuanSp_2.id)
				self:addRedHook(btn.id)
				self.nuoChuanSp_2.id = btn.id
				self.nuoChuanSp_2:setVisible(true)
				self.nuoChuanSp_2:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
			end
		elseif index == 3 then
			if btn.id == self.nuoChuanSp_4.id then 
				self.btnCount = self.btnCount - 1 
				return 
			end

			if not show_3 then 
				self:addGreenHook(btn.id)
				self.nuoChuanSp_3.id = btn.id
				self.nuoChuanSp_3:setVisible(true)
				self.nuoChuanSp_3:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
			else
				self:remveCheckHook("green",self.nuoChuanSp_3.id)
				self:addGreenHook(btn.id)
				self.nuoChuanSp_3.id = btn.id
				self.nuoChuanSp_3:setVisible(true)
				self.nuoChuanSp_3:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
			end
		elseif index == 0 then
			if btn.id == self.nuoChuanSp_3.id then 
				self.btnCount = self.btnCount - 1 
				return 
			end

			if not show_4 then 
				self:addGreenHook(btn.id)
				self.nuoChuanSp_4.id = btn.id
				self.nuoChuanSp_4:setVisible(true)
				self.nuoChuanSp_4:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
			else
				self:remveCheckHook("green",self.nuoChuanSp_4.id)
				self:addGreenHook(btn.id)
				self.nuoChuanSp_4.id = btn.id
				self.nuoChuanSp_4:setVisible(true)
				self.nuoChuanSp_4:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..self.config[btn.id]..".png")
			end
		end
	end

	if self.nuoChuanSp_1:isVisible() and self.nuoChuanSp_2:isVisible() then 
		local id1 = self.nuoChuanSp_1.id
		local id2 = self.nuoChuanSp_2.id
		local index = ShrimpConfig.getChuanIndex(self.config[id1],self.config[id2]) + 1
		print("index == ",index)
		self.maxMoney = self.data[index] < self.data.l_UserLimit and self.data[index] or self.data.l_UserLimit
	else
		self.maxMoney = 0
	end
	self:setSliderPercent()
end

function OperateLayer:sliderCallback(event)
	-- body
	local slider = event.target
	if event.name == "ON_PERCENTAGE_CHANGED" then

		local percent = slider:getPercent()
		local index = percent - 5
		if percent <= 5 then 
			slider:setPercent(5)
			index = 0
		end

		if percent >= 95 then 
			slider:setPercent(95)
			index = 90
		end

		self.minMoney = math.ceil((self.maxMoney/10)*index/90)*10
		self.sliderText:setString(self.minMoney .."/"..self.maxMoney)
	end
end

function OperateLayer:setSliderPercent()
	if self.minMoney > self.maxMoney then 
		self.minMoney = self.maxMoney
	end

	local index = math.ceil(self.minMoney/self.maxMoney*100)
	
	if index <= 5 then index = 5 end
	if index >= 95 then index = 95 end
	
	if self.maxMoney <= 0 then 
		self.slider:setPercent(5)
		self.sliderText:setString("0/0")
	else
		self.slider:setPercent(index)
		self.sliderText:setString(self.minMoney .."/"..self.maxMoney)
	end

	if self.maxMoney <= 0 then
		self.slider:setEnabled(false)
	else
		self.slider:setEnabled(true)
	end

	-- self.myText:setString(self.maxMoney.."")
	-- self.allText:setString(self._scene.lAreaLimitScore.."")
end

function OperateLayer:onAddBtn()
	-- body
	if self.maxMoney <= 0 then 
		return
	end

	self.minMoney = self.minMoney + 10
	if self.minMoney > self.maxMoney then 
		self.minMoney = self.maxMoney
	end

	local percent = self.minMoney / self.maxMoney
	local index = self.slider:getPercent()
	index = index + percent
	self.slider:setPercent(index)
	if index <= 5 then 
		index = 0 
		self.slider:setPercent(5)
	end
	if index >= 95 then 
		index = 90
		self.slider:setPercent(95) 
	end
	

	self.sliderText:setString(self.minMoney .."/"..self.maxMoney)


end

function OperateLayer:onReduceBtn( ... )
	if self.maxMoney <= 0 then 
		return
	end

	self.minMoney = self.minMoney - 10
	if self.minMoney > self.maxMoney then 
		self.minMoney = self.maxMoney
	end	

	if self.minMoney < 0 then 
		self.minMoney = 0
	end

	local  percent = self.minMoney/self.maxMoney
	local index = self.slider:getPercent()
	index = index - percent
	self.slider:setPercent(index)
	if index <= 5 then 
		index = 0 
		self.slider:setPercent(5)
	end
	if index >= 95 then 
		index = 90
		self.slider:setPercent(95)
	end
	

	self.sliderText:setString(self.minMoney .."/"..self.maxMoney)
end

return OperateLayer