



--
--
-- Author: zcc
-- Date: 2019-03-08 10:36:42
-- 房间信息
--
--


local RoomInfo = class("RoomInfo", function(scene)
	local RoomInfo = display.newLayer()
	return RoomInfo
end)

local ExternalFun  = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local TAG_BT_PLAY 	= 0x01
local TAG_BT_RECORD = 0x02
local TAG_BT_EXIT 	= 0x03

function RoomInfo:ctor(scene,data)
	this = self
	self._scene = scene
	self._data = data
	self.userItem = {}
	-- for i=1,10 do
	-- 	self.userItem[#self.userItem+1] = self._scene:GetMeUserItem()
	-- end
	self.userItem = clone(self._scene.m_GameUserItem)

	local rootLayer, csbNode = ExternalFun.loadRootCSB("ui/RoomInfo.csb", self)
	local bg = csbNode:getChildByName("bg")
	self.m_bgLayer = bg

	local funCallback = function(ref)
		this:onButtonCallback(ref:getTag(), ref)
	end

	local btn = bg:getChildByName("play_btn")
	btn:setTag(TAG_BT_PLAY)
	btn:addClickEventListener(funCallback)
	self.play_btn = btn

	local btn = bg:getChildByName("record_btn")
	btn:setTag(TAG_BT_RECORD)
	btn:addClickEventListener(funCallback)
	self.record_btn = btn

	local btn = bg:getChildByName("back_btn")
	btn:setTag(TAG_BT_EXIT)
	btn:addClickEventListener(funCallback)

	self.play_node = bg:getChildByName("play_node")
	self.record_node = bg:getChildByName("record_node")

	self.play_btn:setEnabled(false)
	self.record_btn:setEnabled(true)

	self.play_node:setVisible(true)
	self.record_node:setVisible(false)


	self:initPlayNode()
	self:initRecordNode()
end

function RoomInfo:initPlayNode()
	-- body

	local check_dice =  self.play_node:getChildByName("check_dice")
	local area_check =  self.play_node:getChildByName("area_check")
	local user_check =  self.play_node:getChildByName("user_check")

	local play_txt =  self.play_node:getChildByName("play_txt")
	local area_limit =  self.play_node:getChildByName("area_limit")
	local user_limit =  self.play_node:getChildByName("user_limit")


	local config = self._scene:getRoomConfig()
	
	if config.bIsDoubleSice then 
		play_txt:setString("二骰玩法")
	else
		play_txt:setString("三骰玩法")
	end

	area_limit:setString(config.lAreaLimitScore.."封顶")
	user_limit:setString(config.lUserMaxScore.."封顶")

    -- self.m_tabPrivateRoomConfig.bIsDoubleSice
    -- self.m_tabPrivateRoomConfig.lUserMaxScore = cmd_data.lUserMaxScore
    -- self.m_tabPrivateRoomConfig.lAreaLimitScore = cmd_data.lAreaLimitScore
end

function RoomInfo:initRecordNode( ... )
	-- body

 	self.cell_width = 133
 	self.cell_height = self.record_node:getContentSize().height

    self._listGoldRoomView = cc.TableView:create(self.record_node:getContentSize())
    self._listGoldRoomView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)    
    self._listGoldRoomView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN) 
    self._listGoldRoomView:setPosition(cc.p(0,0))
    self._listGoldRoomView:setDelegate()
    self._listGoldRoomView:addTo(self.record_node)
    self._listGoldRoomView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listGoldRoomView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listGoldRoomView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listGoldRoomView:reloadData()
end

function RoomInfo:cellSizeForTable(view, idx)
	return self.cell_width , self.cell_height
end

function RoomInfo:tableCellAtIndex(view, idx)
	local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local config = self._scene:getRoomConfig()
    if idx == 0 then 
    	self:createName(cell)
    elseif idx == 1  then
    	if config.bIsGoldPersonalRoom then 
    		-- self:createAllScore(cell)
    		self:createNumberBoard(cell,idx)
	    else
	    	self:createAllScore(cell)
	    end
    else
    	if config.bIsGoldPersonalRoom then 
	    	self:createNumberBoard(cell,idx)
	    else
	    	self:createNumberBoard(cell,idx-1)
	    end
    end

    return cell
end

function RoomInfo:numberOfCellsInTableView(view)
	local config = self._scene:getRoomConfig()
	if config.bIsGoldPersonalRoom then 
		if self._data.nUserPlayCount <= 0 then 
			return 0
		end
		return self._data.nUserPlayCount + 1
	else
		if self._data.nUserPlayCount <= 0 then 
			return 0
		end
		return self._data.nUserPlayCount + 2
	end
	return 0
end

function RoomInfo:createName(cell)
	-- body
	local height = self.cell_height

	local sp = cc.Sprite:create("roomInfo/box.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)

	local sp = cc.Sprite:create("roomInfo/line_1.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width-1,height-sp:getContentSize().height/2)

	local sp = cc.Sprite:create("roomInfo/line.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width/2,height)

	local sp = cc.Sprite:create("roomInfo/num.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)

	height = height - sp:getContentSize().height

	local config = self._scene:getRoomConfig()
	if config.bIsGoldPersonalRoom then 
		local sp = cc.Sprite:create("roomInfo/line_1.png")
		cell:addChild(sp)
		sp:setPosition(self.cell_width-1,height-sp:getContentSize().height/2)

		local name = self._scene:GetMeUserItem().szNickName
		local size = cc.size(self.cell_width,sp:getContentSize().height)
	    local clipnick = ClipText:createClipText(size, name, nil, 28)
	    clipnick:setAnchorPoint(cc.p(0.5,0.5))
	    clipnick:setTextColor(cc.c3b(135, 90, 67))
	    clipnick:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)
	    cell:addChild(clipnick)

			
		height = height - sp:getContentSize().height

	    local image = ccui.ImageView:create("roomInfo/line.png")
	    image:setAnchorPoint(cc.p(0.5, 0.5))
	    image:setPosition(self.cell_width/2,height)
	    image:setScale9Enabled(true)
	    image:setContentSize(cc.size(self.cell_width,1))
	    cell:addChild(image)
	else 	
		for i=1,#self.userItem do
			local sp = cc.Sprite:create("roomInfo/line_1.png")
			cell:addChild(sp)
			sp:setPosition(self.cell_width-1,height-sp:getContentSize().height/2)

			local name = self.userItem[i].szNickName
			local size = cc.size(self.cell_width,sp:getContentSize().height)
		    local clipnick = ClipText:createClipText(size, name, nil, 28)
		    clipnick:setAnchorPoint(cc.p(0.5,0.5))
		    clipnick:setTextColor(cc.c3b(135, 90, 67))
		    clipnick:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)
		    cell:addChild(clipnick)

				
			height = height - sp:getContentSize().height

		    local image = ccui.ImageView:create("roomInfo/line.png")
		    image:setAnchorPoint(cc.p(0.5, 0.5))
		    image:setPosition(self.cell_width/2,height)
		    image:setScale9Enabled(true)
		    image:setContentSize(cc.size(self.cell_width,1))
		    cell:addChild(image)
		end
	end


end

function RoomInfo:createAllScore(cell)
	local height = self.cell_height

	local sp = cc.Sprite:create("roomInfo/box.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)

	local size = cc.size(self.cell_width,sp:getContentSize().height)
    local clipnick = ClipText:createClipText(size, "总分", nil, 28)
    clipnick:setAnchorPoint(cc.p(0.5,0.5))
    -- clipnick:setTextColor(cc.c3b(135, 90, 67))
    clipnick:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)
    cell:addChild(clipnick)

    height = height - sp:getContentSize().height

	local sp = cc.Sprite:create("roomInfo/line_1.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width-1,self.cell_height-sp:getContentSize().height/2)





	local config = self._scene:getRoomConfig()
	if config.bIsGoldPersonalRoom then
		local sp = cc.Sprite:create("roomInfo/line_1.png")
		cell:addChild(sp)
		sp:setPosition(self.cell_width-1,height-sp:getContentSize().height/2) 
		local score = 0 
		for k,v in pairs(self._data.lScore[1]) do
			score = score + v
		end
		local txtscore = nil
	    if score >= 0 then
	        txtscore = cc.LabelAtlas:create(math.abs(score),"roomInfo/win.png",19,25,string.byte("/"))
	    else
	        txtscore = cc.LabelAtlas:create("/"..math.abs(score),"roomInfo/lose.png",19,25,string.byte("/"))
	    end
	    txtscore:setAnchorPoint(0.5, 0.5)
	    txtscore:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)
	    cell:addChild(txtscore)

		height = height - sp:getContentSize().height

	    local image = ccui.ImageView:create("roomInfo/line.png")
	    image:setAnchorPoint(cc.p(0.5, 0.5))
	    image:setPosition(self.cell_width/2,height)
	    image:setScale9Enabled(true)
	    image:setContentSize(cc.size(self.cell_width,1))
	    cell:addChild(image)
	else 
		for i=1,#self.userItem do

			local sp = cc.Sprite:create("roomInfo/line_1.png")
			cell:addChild(sp)
			sp:setPosition(self.cell_width-1,height-sp:getContentSize().height/2)

			local user = self.userItem[i]

			local score = 0 
			for k,v in pairs(self._data.lScore[user.wChairID+1]) do
				score = score + v
			end
			local txtscore = nil
		    if score >= 0 then
		        txtscore = cc.LabelAtlas:create(math.abs(score),"roomInfo/win.png",19,25,string.byte("/"))
		    else
		        txtscore = cc.LabelAtlas:create("/"..math.abs(score),"roomInfo/lose.png",19,25,string.byte("/"))
		    end
		    txtscore:setAnchorPoint(0.5, 0.5)
		    txtscore:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)
		    cell:addChild(txtscore)

			height = height - sp:getContentSize().height

		    local image = ccui.ImageView:create("roomInfo/line.png")
		    image:setAnchorPoint(cc.p(0.5, 0.5))
		    image:setPosition(self.cell_width/2,height)
		    image:setScale9Enabled(true)
		    image:setContentSize(cc.size(self.cell_width,1))
		    cell:addChild(image)

		end

	end

end

function RoomInfo:createNumberBoard(cell,idx)
	local height = self.cell_height

	local sp = cc.Sprite:create("roomInfo/box.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)
	local box = sp

	local sp = cc.Sprite:create("roomInfo/line_1.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width-1,height-sp:getContentSize().height/2)


	local sp = cc.Sprite:create("roomInfo/ju.png")
	cell:addChild(sp)
	sp:setPosition(self.cell_width/2,height-box:getContentSize().height/2)

	--Create Text_1_0
	local text = ccui.Text:create()
	text:setFontName("fonts/round_body.ttf")
	text:setString(idx)
	text:setFontSize(22)
	text:setPosition(self.cell_width/2,height-box:getContentSize().height/2)
	cell:addChild(text)

	height = height - box:getContentSize().height






	local config = self._scene:getRoomConfig()
	if config.bIsGoldPersonalRoom then 

		local sp = cc.Sprite:create("roomInfo/line_1.png")
		cell:addChild(sp)
		sp:setPosition(self.cell_width-1,height-sp:getContentSize().height/2)

		local score = self._data.lScore[1][idx]
		local txtscore = nil
	    if score >= 0 then
	        txtscore = cc.LabelAtlas:create("/"..math.abs(score),"roomInfo/win.png",19,25,string.byte("/"))
	    else
	        txtscore = cc.LabelAtlas:create("/"..math.abs(score),"roomInfo/lose.png",19,25,string.byte("/"))
	    end
	    txtscore:setAnchorPoint(0.5, 0.5)
	    txtscore:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)
	    cell:addChild(txtscore)


		height = height - sp:getContentSize().height

	    local image = ccui.ImageView:create("roomInfo/line.png")
	    image:setAnchorPoint(cc.p(0.5, 0.5))
	    image:setPosition(self.cell_width/2,height)
	    image:setScale9Enabled(true)
	    image:setContentSize(cc.size(self.cell_width,1))
	    cell:addChild(image)
	else
		for i=1,#self.userItem do

			local sp = cc.Sprite:create("roomInfo/line_1.png")
			cell:addChild(sp)
			sp:setPosition(self.cell_width-1,height-sp:getContentSize().height/2)

			local score = self._data.lScore[self.userItem[i].wChairID+1][idx]
			local txtscore = nil
		    if score > 0 then
		        txtscore = cc.LabelAtlas:create("/"..math.abs(score),"roomInfo/win.png",19,25,string.byte("/"))
		    elseif score < 0 then
		        txtscore = cc.LabelAtlas:create("/"..math.abs(score),"roomInfo/lose.png",19,25,string.byte("/"))
		    else
		    	txtscore = cc.LabelAtlas:create(score,"roomInfo/win.png",19,25,string.byte("/"))
		    end
		    txtscore:setAnchorPoint(0.5, 0.5)
		    txtscore:setPosition(self.cell_width/2,height-sp:getContentSize().height/2)
		    cell:addChild(txtscore)


			height = height - sp:getContentSize().height

		    local image = ccui.ImageView:create("roomInfo/line.png")
		    image:setAnchorPoint(cc.p(0.5, 0.5))
		    image:setPosition(self.cell_width/2,height)
		    image:setScale9Enabled(true)
		    image:setContentSize(cc.size(self.cell_width,1))
		    cell:addChild(image)
		end
	end


end

function RoomInfo:onButtonCallback(tag, ref)
	if tag == TAG_BT_EXIT then
		print("离开")
		self:hideLayer()
	elseif tag == TAG_BT_RECORD then 
		self.play_node:setVisible(false)
		self.record_node:setVisible(true)

		self.play_btn:setEnabled(true)
		self.record_btn:setEnabled(false)

	elseif tag == TAG_BT_PLAY then 
		self.play_node:setVisible(true)
		self.record_node:setVisible(false)

		self.play_btn:setEnabled(false)
		self.record_btn:setEnabled(true)
	end
end


function RoomInfo:hideLayer()
	self:removeFromParent()
end

return RoomInfo