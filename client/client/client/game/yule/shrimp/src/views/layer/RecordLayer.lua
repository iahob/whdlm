--
-- Author: tom
-- Date: 2017-02-27 17:26:42
--
local RecordLayer = class("RecordLayer", function(scene)
	local RecordLayer = display.newLayer()
	return RecordLayer
end)

local cmd = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.models.CMD_Game")


function RecordLayer:onInitData()
end

function RecordLayer:onResetData()
end

local this
function RecordLayer:ctor(scene)
	this = self
	self._scene = scene

	self.colorLayer = cc.LayerColor:create(cc.c4b(0, 0, 0, 125))
		:setContentSize(display.width, display.height)
		:addTo(self)
	self.colorLayer:setTouchEnabled(true)
	self.colorLayer:registerScriptTouchHandler(function(eventType, x, y)
		return this:onTouch(eventType, x, y)
	end)
	
	local csbnode = cc.CSLoader:createNode("ui/GameRecord.csb");
	self:addChild(csbnode)

	self.sp = csbnode:getChildByName("Sprite_1")
	self.listView = csbnode:getChildByName("listview")

	-- self:initView()
end

function RecordLayer:onButtonCallback(tag, ref)

end

function RecordLayer:onTouch(eventType, x, y)
	if eventType == "began" then
		return true
	end

	local pos = cc.p(x, y)
    local rectLayerBg = self.sp:getBoundingBox()
    if not cc.rectContainsPoint(rectLayerBg, pos) then
    	self:hideLayer()
    end

    return true
end

function RecordLayer:showLayer()
	self.colorLayer:setTouchEnabled(true)
	self:setVisible(true)
end

function RecordLayer:hideLayer()
	self:removeFromParent()
end

function RecordLayer:initView(data)
	-- body
	for i= 1 ,#data do
		local tab = data[i]
		table.sort( tab, function( a,b )
			return a < b
		end )

		local layout = ccui.Layout:create()
		layout:setContentSize(cc.size(309, 80))

		local sp = cc.Sprite:create("record/di.png")
		layout:addChild(sp)
		sp:setPosition(154.5,40)

		local al = ccui.TextAtlas:create(tostring(i),"record/num.png",25,34,"0")
		al:setAnchorPoint(0.5000, 0.5000)
		al:setPosition(33, 40)
		layout:addChild(al)

		if tab[1] == tab[2] and tab[2] == tab[3] then 
			local path = "dice/x_sp_"..self._scene:getDiceIndex().."_"..tab[1]..".png"
			local sp = cc.Sprite:create(path)
			if sp then
				layout:addChild(sp)
				sp:setPosition(110, 40)
				local icon = cc.Sprite:create("record/3.png")
				sp:addChild(icon)
				icon:setPosition(67,61)
			end

		elseif tab[1] == tab[2] then 
			local path = "dice/x_sp_"..self._scene:getDiceIndex().."_"..tab[1]..".png"
			local sp = cc.Sprite:create(path)
			if sp then
				layout:addChild(sp)
				sp:setPosition(110, 40)
				local icon = cc.Sprite:create("record/2.png")
				sp:addChild(icon)
				icon:setPosition(67,61)
			end
			local path = "dice/x_sp_"..self._scene:getDiceIndex().."_"..tab[3]..".png"
			sp = cc.Sprite:create(path)
			if sp then
				layout:addChild(sp)
				sp:setPosition(185, 40)
			end

		elseif tab[2] == tab[3] then 
			local path = "dice/x_sp_"..self._scene:getDiceIndex().."_"..tab[2]..".png"
			local sp = cc.Sprite:create(path)
			if sp then
				layout:addChild(sp)
				sp:setPosition(110, 40)
				local icon = cc.Sprite:create("record/2.png")
				sp:addChild(icon)
				icon:setPosition(67,61)
			end

			local path = "dice/x_sp_"..self._scene:getDiceIndex().."_"..tab[1]..".png"
			sp = cc.Sprite:create(path)
			if sp then
				layout:addChild(sp)
				sp:setPosition(185, 40)
			end
		else
			for j=1,#tab do
				local path = "dice/x_sp_"..self._scene:getDiceIndex().."_"..tab[j]..".png"
				local sp = cc.Sprite:create(path)
				if sp then 
					layout:addChild(sp)
					sp:setPosition(110 + (j-1)*75, 40)
				end
			end
	 	end

	 	self.listView:addChild(layout)
	end

	self.listView:jumpToBottom()
end



return RecordLayer