


local BetInfoLayer = class("BetInfoLayer",function ()
	return cc.Layer:create()
end)

local ShrimpConfig = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.ShrimpConfig")
local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")

function BetInfoLayer:ctor(scene,path)
	-- body
	self.path = path or "ui/BetInfo.csb"
	self._scene = scene
	self.csbnode = cc.CSLoader:createNode(self.path);
	self:addChild(self.csbnode)

	self.closeBtn = self.csbnode:getChildByName("close")
	self.listview = self.csbnode:getChildByName("listview")

	self.closeBtn:addClickEventListener(function ()
		self:removeFromParent()
	end)

	-- self.yes:addClickEventListener(function ()
	-- 	self:removeFromParent()
	-- end)
end

local font1 = {"鱼","虾","蟹","狮子","鸡","老虎","葫芦"}
local font2 = {"鱼","虾","蟹","狮子","鸡","老虎","葫芦"}
function BetInfoLayer:setData(data)
	-- body
	for k,v in pairs(data) do
		dump(v,"setData",12)
		local allAreaInfo  = self:getAllAreaInfo(v.lAllAreaInfo)
		local allBaoZiInfo = self:getAllBaoZiInfo(v.lAllBaoZiInfo)
		local allChuanInfo = self:getAllChuanInfo(v.lAllChuanInfo)
		local nuo_dan_info = self:getAllNuoDanInfo(v.nuo_dan_info)
		local nuo_chuan_info = self:getAllNuoChuanInfo(v.nuo_chuan_info)

		local height = 40
		local w = math.ceil((#allAreaInfo)/4)
		height = height + w * 80
		
		w = math.ceil((#allBaoZiInfo)/4)
		height = height + w * 80 

		w = math.ceil((#allChuanInfo)/3)
		height = height + w * 80 

		w = math.ceil((#nuo_dan_info)/2)
		height = height + w * 80

		w = #nuo_chuan_info
		height = height + w * 80


		local item = nil
		if self.path == "ui/BetInfo.csb" then 
			item = ccui.ImageView:create()
			item:loadTexture("public/betinfo_di1.png",0)
			item:setScale9Enabled(true)
			item:setContentSize(cc.size(self.listview:getContentSize().width,height))
			self.listview:addChild(item)
		else
			item = ccui.Layout:create()
			item:setContentSize(cc.size(self.listview:getContentSize().width,height))
			self.listview:addChild(item)
		end


 		local av = HeadSprite:createClipHead( v.useritem, 90, "avatar/avatar_down.png" )
    	item:addChild(av)
	    av:setPosition(cc.p(75,height - 45))

	    av = cc.Sprite:create("avatar/avatar.png")
	    item:addChild(av)
	    av:setPosition(cc.p(75,height - 45))

		local text = ccui.Text:create()
		text:setFontSize(22)
		text:setString(v.useritem.szNickName)
		text:setPosition(75, height - 105)
		text:setTextColor({r = 78, g = 50, b = 50})
		item:addChild(text)


		local x = 150
		local y = 20
		for i=1,#allAreaInfo do
			allAreaInfo[i]:setAnchorPoint(0,1)
			allAreaInfo[i]:setPosition( x +  (math.ceil((i-1)%4))*180,height - math.floor((i-1)/4)*80 - y)
			item:addChild(allAreaInfo[i])
		end
		height = height - math.ceil(#allAreaInfo/4)*80

		for i=1,#allBaoZiInfo do
			allBaoZiInfo[i]:setAnchorPoint(0,1)
			allBaoZiInfo[i]:setPosition( x + (math.ceil((i-1)%4))*180,height - math.floor((i-1)/4)*80 - y)
			item:addChild(allBaoZiInfo[i])
		end
		height = height - math.ceil(#allBaoZiInfo/4)*80

		for i=1,#allChuanInfo do
			allChuanInfo[i]:setPosition( x + (math.ceil((i-1)%3))*255,height - math.floor((i-1)/3)*80 - y)
			item:addChild(allChuanInfo[i])
		end
		height = height - math.ceil(#allChuanInfo/3)*80

		for i=1,#nuo_dan_info do
			nuo_dan_info[i]:setPosition(x + (math.ceil((i-1)%2))*300,height - math.floor((i-1)/2)*80 - y)
			item:addChild(nuo_dan_info[i])
		end
		height = height - math.ceil(#nuo_dan_info/2)*80

		for i=1,#nuo_chuan_info do
			nuo_chuan_info[i]:setPosition(x ,height - (i-1)*80 - y)
			item:addChild(nuo_chuan_info[i])
		end

	end
end

function BetInfoLayer:getAllAreaInfo(lAllAreaInfo)
	
	local font = {}
    if self._scene.bIsDoubleSice then 
    	font = {"单压","单压","单压","单压","单压","单压"} -- two
    else
    	font = {"单压","单压","单压","单压","单压","单压"} -- three
    end
    local tab = {}

	for i=1,#lAllAreaInfo do
		if lAllAreaInfo[i] > 0 then 
			local image = ccui.ImageView:create()
			image:loadTexture("public/betinfo_di2.png",0)
			image:setScale9Enabled(true)
			image:setContentSize({width = 180, height = 80})

			local text = ccui.Text:create()
			text:setFontSize(24)
			text:setString(font[i])
			text:setPosition(55.5, 54)
			text:setTextColor({r = 87, g = 0, b = 0})
			image:addChild(text)

			local al = ccui.TextAtlas:create(lAllAreaInfo[i],"public/winnum.png",19,25,"0")
			al:setAnchorPoint(0.5, 0.5)
			al:setPosition(54.5, 23)
			image:addChild(al)

			local sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..(i-1)..".png")
			sp:setPosition(141.5, 40)
			image:addChild(sp)

			tab[#tab+1] = image
		end
	end

	return tab
end

function BetInfoLayer:getAllBaoZiInfo(lAllBaoZiInfo)
	
    local tab = {}

	for i=1,#lAllBaoZiInfo do

		if lAllBaoZiInfo[i] > 0 then 

			local image = ccui.ImageView:create()
			image:loadTexture("public/betinfo_di2.png",0)
			image:setScale9Enabled(true)
			image:setAnchorPoint(0.5,1)
			image:setContentSize({width = 180, height = 80})

			local text = ccui.Text:create()
			text:setFontSize(24)
			text:setString("豹子")
			text:setPosition(55.5, 54)
			text:setTextColor({r = 87, g = 0, b = 0})
			image:addChild(text)

			local al = ccui.TextAtlas:create(lAllBaoZiInfo[i],"public/winnum.png",19,25,"0")
			al:setAnchorPoint(0.5, 0.5)
			al:setPosition(54.5, 23)
			image:addChild(al)

			local sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..(i-1)..".png")
			sp:setPosition(141.5, 40)
			image:addChild(sp)

			tab[#tab+1] = image
		end
	end

	return tab
end

function BetInfoLayer:getAllChuanInfo(lAllChuanInfo)
	
    local tab = {}

	for i=1,#lAllChuanInfo do

		if lAllChuanInfo[i] > 0 then 

			local image = ccui.ImageView:create()
			image:loadTexture("public/betinfo_di2.png",0)
			image:setScale9Enabled(true)
			image:setAnchorPoint(0,1)
			image:setContentSize({width = 255, height = 80})

			local text = ccui.Text:create()
			text:setFontSize(24)
			text:setString("连串")
			text:setPosition(55.5, 54)
			text:setTextColor({r = 87, g = 0, b = 0})
			image:addChild(text)

			local al = ccui.TextAtlas:create(lAllChuanInfo[i],"public/winnum.png",19,25,"0")
			al:setAnchorPoint(0.5, 0.5)
			al:setPosition(54.5, 23)
			image:addChild(al)

			local one , two = ShrimpConfig.geiIndexForChuan(i-1)

			local sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..one..".png")
			sp:setPosition(141.5, 40)
			image:addChild(sp)

			sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..two..".png")
			sp:setPosition(213.5, 40)
			image:addChild(sp)

			tab[#tab+1] = image
		end
	end

	return tab
end

function BetInfoLayer:getAllNuoDanInfo(nuo_dan_info)
	
    local tab = {}
	for i=1,#nuo_dan_info do
		if nuo_dan_info[i].score > 0 then 
			local data  = nuo_dan_info[i]
			local image = ccui.ImageView:create()
			image:loadTexture("public/betinfo_di2.png",0)
			image:setScale9Enabled(true)
			image:setAnchorPoint(0,1)
			image:setContentSize({width = 300, height = 80})

			local text = ccui.Text:create()
			text:setFontSize(24)
			text:setString("挪单")
			text:setPosition(55.5, 54)
			text:setTextColor({r = 87, g = 0, b = 0})
			image:addChild(text)	

			local al = ccui.TextAtlas:create(nuo_dan_info[i].score,"public/winnum.png",19,25,"0")
			al:setAnchorPoint(0.5, 0.5)
			al:setPosition(54.5, 23)
			image:addChild(al)

			-- local one , two = ShrimpConfig.geiIndexForChuan(i-1)               "score"     = 0

			local sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..data.old_place..".png")
			sp:setPosition(141.5, 40)
			image:addChild(sp)

			sp = cc.Sprite:create("public/arrow.png")
			sp:setPosition(200,40)
			image:addChild(sp)

			sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..data.new_place..".png")
			sp:setPosition(260, 40)
			image:addChild(sp)

			tab[#tab+1] = image
		end
	end

	return tab
end

function BetInfoLayer:getAllNuoChuanInfo(nuo_dan_info)
	
    local tab = {}

	for i=1,#nuo_dan_info do

		if nuo_dan_info[i].score > 0 then 
			local data  = nuo_dan_info[i]
			local image = ccui.ImageView:create()
			image:loadTexture("public/betinfo_di2.png",0)
			image:setAnchorPoint(0,1)
			image:setScale9Enabled(true)
			image:setContentSize({width = 450, height = 80})

			local text = ccui.Text:create()
			text:setFontSize(24)
			text:setString("挪串")
			text:setPosition(55.5, 54)
			text:setTextColor({r = 87, g = 0, b = 0})
			image:addChild(text)

			local al = ccui.TextAtlas:create(data.score,"public/winnum.png",19,25,"0")
			al:setAnchorPoint(0.5, 0.5)
			al:setPosition(54.5, 23)
			image:addChild(al)

			local one , two = ShrimpConfig.geiIndexForChuan(data.old_place)

			local sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..one..".png")
			sp:setPosition(142, 40)
			image:addChild(sp)

			sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..two..".png")
			sp:setPosition(215, 40)
			image:addChild(sp)

			sp = cc.Sprite:create("public/arrow.png")
			sp:setPosition(275,40)
			image:addChild(sp)

			one , two = ShrimpConfig.geiIndexForChuan(data.new_place)

			sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..one..".png")
			sp:setPosition(335, 40)
			image:addChild(sp)

			sp = cc.Sprite:create("dice/x_sp_"..self._scene:getDiceIndex().."_"..two..".png")
			sp:setPosition(408, 40)
			image:addChild(sp)

			tab[#tab+1] = image
		end
	end

	return tab
end

function BetInfoLayer:setSice(data)
	-- body
	local index = 0
	local node = self.csbnode:getChildByName("dice")
	if node and data then
		for i=1,#data[1] do
			local path = "dice/x_sp_"..self._scene:getDiceIndex().."_"..data[1][i]..".png"
			local sp = cc.Sprite:create(path)
			if sp then
				node:addChild(sp)
				sp:setPosition(36+index*69,38)
				index = index + 1
			end
		end
	end
end

function BetInfoLayer:setCount(data)
	-- body
	local Text = self.csbnode:getChildByName("Text")
	if Text  then
		Text:setString("第"..data.."局")
	end
end

return BetInfoLayer