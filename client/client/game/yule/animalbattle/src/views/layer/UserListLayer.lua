local UserListerLayer = class("UserListerLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
UserListerLayer.BT_BG = 1
UserListerLayer.BT_CLOSE = 2

function UserListerLayer:ctor(userList)
	self.userData = userList
	self:loadResource()
end

function UserListerLayer:loadResource()
	

	self.colorLayer = cc.LayerColor:create(cc.c4b(0, 0, 0, 125))
		:setContentSize(display.width, display.height)
		:addTo(self)
	self.colorLayer:setTouchEnabled(true)
	self.colorLayer:registerScriptTouchHandler(function(eventType, x, y)
		return self:onTouch(eventType, x, y)
	end)

	local csbNode = ExternalFun.loadCSB("userList/Layer.csb", self)

	self.bg_sp = csbNode:getChildByName("Panel_1"):getChildByName("Sprite")

	local btn = csbNode:getChildByName("Panel_1"):getChildByName("Button")
	btn:addClickEventListener(function( )
		self:hideLayer()
	end)


	self.userList = cc.TableView:create(cc.size(700, 353))
	-- self.userList:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
	self.userList:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 

	self.userList:setAnchorPoint(cc.p(0,0))
	self.userList:setPosition(cc.p(7.5,9.5))
	self.userList:setDelegate()
	self.userList:addTo(self.bg_sp)
	self.userList:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self.userList:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
	self.userList:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self.userList:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	self.userList:reloadData()
end

function UserListerLayer:onTouch(eventType, x, y)
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

function UserListerLayer:tableCellAtIndex(view, idx)
	local cell = view:dequeueCell()
    local item = nil
    local num = #self.userData - idx

    if nil == cell then
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    local useritem = self.userData[idx+1]
    if useritem then
	    local csbNode = cc.CSLoader:createNode("useritem.csb")
	    cell:addChild(csbNode)

	    local node = csbNode:getChildByName("panel")
	    local name = node:getChildByName("txt")
		name:setString("")

	    local clipnick = ClipText:createClipText(cc.size(200,30), useritem.szNickName, nil, 30)
	    clipnick:setAnchorPoint(name:getAnchorPoint())
	    clipnick:setTextColor(cc.c3b(103, 91, 59))
	    clipnick:setPosition(name:getPosition())
	    node:addChild(clipnick)

		local head = node:getChildByName("head")
	    local av = PopupInfoHead:createClipHead( useritem, 80, "head_sp.png" )
	    head:addChild(av)
	    av:setPosition(head:getContentSize().width/2,head:getContentSize().height/2)

        local fontW = node:getChildByName("wan")
        local fontY = node:getChildByName("yi")
        local gold = node:getChildByName("asl")
        fontW:setVisible(false)
		fontY:setVisible(false)
		local font = nil
        local lScore = useritem.lScore
        if useritem.lScore > 100000000 then 
            lScore =  string.format("%.2f", lScore/100000000)
            font = font
        elseif useritem.lScore > 10000 then 
            lScore =  string.format("%.2f", lScore/10000)
            font = fontW
        end

        gold:setString(lScore)
        if font then 
        	font:setVisible(true)
	        font:setPositionX(gold:getPositionX()+gold:getContentSize().width+font:getContentSize().width/2)
       	end

	end

    return cell
end

function UserListerLayer:tableCellTouched(view, cell)

end

function UserListerLayer.cellSizeForTable()
	return 700, 89
end

function UserListerLayer:numberOfCellsInTableView()
		return #self.userData	
end

function UserListerLayer:refreshUserList(userList)
	self.userData = userList
	self.userList:reloadData()
end

function UserListerLayer:createUser(userItem, index)
	local ly = ccui.Layout:create()
	ly:setContentSize(cc.size(957,113))

	local csbNode = ExternalFun.loadCSB("userItem.csb", ly)

	local tmpBt = csbNode:getChildByName("Button_head") 
	local pos = cc.p(49.5,54)
	local head = PopupInfoHead:createNormal(userItem, 75):addTo(tmpBt):setPosition(pos)
   	head:enableInfoPop(false, cc.p(80, 80))
	
	csbNode:getChildByName("Text_name"):setString(userItem.szNickName) 
	csbNode:getChildByName("AtlasLabel_gold"):setString(userItem.lScore) 
	csbNode:getChildByName("index"):setString(index.."/") 
	return ly
end

function UserListerLayer:showLayer()
	self.colorLayer:setTouchEnabled(true)
	self:setVisible(true)
end

function UserListerLayer:hideLayer()
	ExternalFun.playSoundEffect("guanbi.mp3")
	self.colorLayer:setTouchEnabled(false)
	self:setVisible(false)
	self:removeFromParent()
end


return UserListerLayer	