local UserListerLayer = class("UserListerLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local GAME_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")

UserListerLayer.BT_BG = 1
UserListerLayer.BT_CLOSE = 2

function UserListerLayer:ctor(userList,userInfo)
	-- self.userData = userList

	self.userData = {}
	for i,v in pairs(userList) do
		if v then 
			self.userData[#self.userData+1] = v
		end
	end

	table.sort(self.userData, function(a, b)
        return a.wChairID < b.wChairID
    end)

	dump(self.userData)
	
	self.userInfo = userInfo
	self:loadResource()
end

function UserListerLayer:loadResource()
	local rootLayer, csbNode = ExternalFun.loadRootCSB("userList.csb", self) 
	self.csbNode = csbNode

	local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
   end

   --退出
	local btn = csbNode:getChildByName("Button_exit")
	btn:setTag(UserListerLayer.BT_CLOSE)
	btn:addTouchEventListener(btnEvent)

	--遮罩
	local btn = csbNode:getChildByName("mask")
	btn:setTag(UserListerLayer.BT_BG)
	btn:addTouchEventListener(btnEvent)

	self.userList = cc.TableView:create(cc.size(907, 480))
	self.userList:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
	-- self.userList:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL) 

	self.userList:setAnchorPoint(cc.p(0,0))
	self.userList:setPosition(cc.p(213.50,117.00))
	self.userList:setDelegate()
	self.userList:addTo(self.csbNode)
	self.userList:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self.userList:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
	self.userList:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self.userList:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	self.userList:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
	self.userList:reloadData()
end

function UserListerLayer:onButtonClickedEvent(tag, ref)
	if tag == UserListerLayer.BT_CLOSE or tag == UserListerLayer.BT_BG then
		self:setVisible(false)
	end
end

function UserListerLayer:tableCellAtIndex(view, idx)
	local cell = view:dequeueCell()
    local item = nil
    local num = #self.userData - idx

    if nil == cell then
        cell = cc.TableViewCell:new()
        -- local userBar = self:createUser(self.userData[idx+1], num)
        -- userBar:setAnchorPoint(0, 0)
        -- userBar:setPosition(cc.p(497,50))
        -- cell:addChild(userBar)
    else
        cell:removeAllChildren()

    end

    local userBar = self:createUser(self.userData[idx+1], num)
    if not userBar then return cell end
    userBar:setAnchorPoint(0.5, 0.5)
    userBar:setPosition(cc.p(907/2,45))
    cell:addChild(userBar)

     return cell
end

function UserListerLayer:tableCellTouched(view, cell)

end

function UserListerLayer.cellSizeForTable()
	return 907, 90
end

function UserListerLayer:numberOfCellsInTableView()
		return #self.userData	
end

function UserListerLayer:refreshUserList(userList)
	self.userData = userList
	self.userList:reloadData()
end

function UserListerLayer:createUser(userItem, index)

	-- dump(self.userInfo,"self.userInfo",12)
	for i=1,#self.userInfo do
		local info = self.userInfo[i]
		if info.wChairID == userItem.wChairID then 

			local ly = ccui.Layout:create()
			ly:setContentSize(cc.size(907,80))

			local csbNode = ExternalFun.loadCSB("UserNode.csb", ly)

			local panel = csbNode:getChildByName("Panel_2")
			local avNode = panel:getChildByName("head")

			local head = PopupInfoHead:createNormal(userItem, 60)
			avNode:addChild(head)
			-- avNode:setPosition(231,40)

			panel:getChildByName("ranking"):setString(#self.userData+1-index) 
			panel:getChildByName("name"):setString(userItem.szNickName) 
			-- panel:getChildByName("goldNum"):setString(userItem.lScore) 

			
			panel:getChildByName("winNum"):setString(info.lWinCount) 


		   local lScore =  tostring(userItem.lScore)
		   if userItem.lScore >= 100000000 then
		        panel:getChildByName("Text_wan"):setVisible(false)
		        panel:getChildByName("Text_yi"):setVisible(true)
		        lScore = string.format("%.2f", userItem.lScore  / 100000000)
		    elseif userItem.lScore >= 10000 then
		        panel:getChildByName("Text_wan"):setVisible(true)
		        panel:getChildByName("Text_yi"):setVisible(false)
		        lScore = string.format("%.2f", userItem.lScore  / 10000)
		    else
		        panel:getChildByName("Text_wan"):setVisible(false)
		        panel:getChildByName("Text_yi"):setVisible(false)
		    end

		    local gold_panel = panel:getChildByName("goldNum")
		    lScore = string.gsub(lScore,'%.','/')
		    gold_panel:setString(lScore)
		    panel:getChildByName("Text_wan"):setPositionX(gold_panel:getPositionX() + gold_panel:getContentSize().width)
		    panel:getChildByName("Text_yi"):setPositionX(gold_panel:getPositionX() + gold_panel:getContentSize().width)


		    lScore,isShowWan,isShowYi =  GAME_CMD.numTextFormat(info.lRecordInAllScore)
		    lScore = string.gsub(lScore,'%.','/')
		    panel:getChildByName("betNum"):setString(lScore)

		    local bet_wan = panel:getChildByName("bet_wan")
		    local bet_yi = panel:getChildByName("bet_yi")

            bet_wan:setVisible(isShowWan)
            bet_yi:setVisible(isShowYi)

            local size = panel:getChildByName("betNum"):getContentSize()
            local x = panel:getChildByName("betNum"):getPositionX()
            bet_wan:setPositionX(x + size.width)
            bet_yi:setPositionX(x + size.width)

		    

			return ly
		end
	end

	return nil
end


return UserListerLayer	