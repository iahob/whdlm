-- 
-- 
--	单局结算 
-- 
-- 


local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")

local ResultLayer = class("resultLayer",function ()
	-- body
	return cc.Layer:create()
end)


function ResultLayer:ctor(scene)
	-- body
	self._scene = scene
	local csbnode = cc.CSLoader:createNode("ui/ResultInfo.csb");
	self:addChild(csbnode)


	local panel = csbnode:getChildByName("Panel")

	self.close = panel:getChildByName("close")
	self.yes = panel:getChildByName("btnyes")

	self.item = {}
	for i=1,11 do
		local item = panel:getChildByName("Panel_"..i)
		item:setVisible(false)
		self.item[#self.item+1] = item
	end

	self.sp = {}
	for i=1,3 do
		local sp = panel:getChildByName("sp_"..i)
		sp:setVisible(false)
		self.sp[#self.sp+1] = sp
	end


	self.close:addClickEventListener(function ()
		self:hide()
	end)

	self.yes:addClickEventListener(function ()
		self:hide()
	end)
	self:setData()


	self._scene.isGameEnd = false

    for k,v in pairs(self._scene.m_GameUserItem) do
        if v then
			self._scene.gameView:updataDeskChair(v,self._scene.gameUserChair)
        end
    end

end

function ResultLayer:hide()
	-- body
	self._scene.gameEndData = {}
	-- print("GlobalUserItem.bPrivateRoom = ",GlobalUserItem.bPrivateRoom)
	if GlobalUserItem.bPrivateRoom and self._scene.isPriOver then 
		local la = self._scene.gameView:getChildByName("private_end_layer")
		if la then la:setVisible(true) end
	end
	self:removeFromParent()
end

function ResultLayer:setData()
	-- body

 	local cbSice = clone(self._scene.gameEndData.cbSice[1])
 	for i=1,#cbSice do
 		local index = cbSice[i]
 		if index < 255 then
 			self.sp[i]:setVisible(true)
 			self.sp[i]:setTexture("dice/x_sp_"..self._scene:getDiceIndex().."_"..index..".png")
 		else
 			self.sp[i]:setVisible(false)
 		end
 	end

 	local index = 2
 	for i=1,#self._scene.gameEndData.lUserScore[1] do
 		local useritem = self._scene:getUserInfoByChairID((i-1))
 		if useritem then 
 			local lUserScore = self._scene.gameEndData.lUserScore[1][i]
 			local item = nil
 			if useritem.wChairID == self._scene.countBankerId then 
 				item = self.item[1]

 				local sp = cc.Sprite:create("public/banker_icon.png")
				item:addChild(sp,2)
				sp:setPosition(16, 73)

 			else
 				item = self.item[index]
 				index = index+1
 			end
 			item:setVisible(true)

			local clippingfile = "avatar/avatar_down.png"
	 		local av = HeadSprite:createClipHead( useritem, 90, clippingfile )
	    	item:addChild(av,1)
		    av:setPosition(cc.p(46,46.7))

		    local sp = cc.Sprite:create("avatar/avatar.png")
		   	item:addChild(sp,1)
		    sp:setPosition(cc.p(46,46.7))

			local text = ccui.Text:create()
			text:setFontSize(26)
			text:setString(useritem.szNickName)
			text:setAnchorPoint(0.0, 0.5)
			text:setPosition(108.4, 60)
			text:setTextColor({r = 78, g = 50, b = 50})
			item:addChild(text)

			local path = "public/winnum.png"
			if lUserScore < 0 then path = "public/losenum.png" end
			local AtlasLabel_1 = ccui.TextAtlas:create(tostring(lUserScore),path,19,25,"0")
			AtlasLabel_1:setAnchorPoint(0.0000, 0.5000)
			AtlasLabel_1:setPosition(108, 28.3529)
			item:addChild(AtlasLabel_1)

			if lUserScore < 0 then 
				local sp = cc.Sprite:create("public/lose.png")
				item:addChild(sp)
				sp:setPosition(108, 28.3529)
				AtlasLabel_1:setPosition(115, 28.3529)
			end
 		end
 	end


end

return ResultLayer