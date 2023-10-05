local BankerLayer = class("BankerLayer", cc.Layer)
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local GAME_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")

local TAG_ENUM = {}
TAG_ENUM.BT_EXIT   = 1
TAG_ENUM.BT_MASK   = 2
TAG_ENUM.BT_APPLY  = 3
TAG_ENUM.BT_CANCEL = 4
TAG_ENUM.BT_REFUSE = 5

local ZORDERLAYER = {
	USERLIST = 10,            --用户列表
	TITLE    = 11,            --上庄条件
}

function BankerLayer:ctor(parent)
	self._gameView = parent
	self.userList = self._gameView:getDataMgr():getApplyBankerUserList()

	self:loadResource()
end

function BankerLayer:loadResource()
	local rootLayer, csbNode = ExternalFun.loadRootCSB("banker.csb", self) 
	self.csbNode = csbNode

	local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --退出
	btn = csbNode:getChildByName("Button_exit")
	btn:setTag(TAG_ENUM.BT_EXIT)
	btn:addTouchEventListener(btnEvent)
	--遮罩
	btn = csbNode:getChildByName("mask")
	btn:setTag(TAG_ENUM.BT_MASK)
	btn:addTouchEventListener(btnEvent)
	--申请上庄
	btn = csbNode:getChildByName("Button_apply")
	btn:setTag(TAG_ENUM.BT_APPLY)
	btn:addTouchEventListener(btnEvent)
	--取消申请
	btn = csbNode:getChildByName("Button_cancel")
	btn:setTag(TAG_ENUM.BT_CANCEL)
	btn:addTouchEventListener(btnEvent)
	--主动下庄
	btn = csbNode:getChildByName("Button_refuse")
	btn:setTag(TAG_ENUM.BT_REFUSE)
	btn:addTouchEventListener(btnEvent)

	self.applyList = cc.TableView:create(cc.size(704, 324))
    self.applyList:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self.applyList:setPosition(cc.p(313.08,252.30))
    self.applyList:setDelegate()
    self.applyList:addTo(self.csbNode)
    self.applyList:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self.applyList:registerScriptHandler(self.tableCellTouched, cc.TABLECELL_TOUCHED)
    self.applyList:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    self.applyList:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self.applyList:reloadData()
	self.applyList:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
    local title = csbNode:getChildByName("Text_bankerCondition")
    -- title:setZOrder(ZORDERLAYER.TITLE)
    -- csbNode:getChildByName("title_bg"):setZOrder(ZORDERLAYER.TITLE)

    local score = self._gameView.lApplyBankerCondition
    local text = ""
    if score >= 10000 then
    	text = string.format("大于%d万才可以上庄", score/10000)
    else
    	text = string.format("大于%d才可以上庄", score)
    end

	title:setString(text)

	-- dump(self._gameView:getDataMgr():getApplyBankerUserList(),"self.userList",12)
	-- for k,v in pairs(self.userList) do
	-- 	if self.userList.
	-- end
end

function BankerLayer:tableCellAtIndex(view, idx)
	local cell = view:dequeueCell()
	local item = nil
	local num = #self.userList - idx 

	if nil == cell then
		cell = cc.TableViewCell:new()
		local userBar = self:createUser(self.userList[idx+1], num)
        userBar:setAnchorPoint(0, 0)
        userBar:setPosition(cc.p(400,50))
        cell:addChild(userBar)
	else
		cell:removeAllChildren()
        local userBar = self:createUser(self.userList[idx+1], num)
        userBar:setAnchorPoint(0, 0)
        userBar:setPosition(cc.p(400,50))
        cell:addChild(userBar)
	end
	return cell
end

function BankerLayer.tableCellTouched(view, cell)
end

function BankerLayer:cellSizeForTable()
	return 707, 113
end

function BankerLayer:numberOfCellsInTableView()
	if nil == self.userList then
		return 0
	else
		return #self.userList
	end
end

function BankerLayer:createUser(userItem, index)
	local ly = ccui.Layout:create()
	ly:setContentSize(cc.size(707,113))

	local csbNode = ExternalFun.loadCSB("userItem.csb", ly)

	local tmpBt = csbNode:getChildByName("Button_head")
	local pos = cc.p(-262.78,0)
	-- local head = PopupInfoHead:createNormal(userItem, 75):addTo(csbNode):setPosition(pos)
	local head = PopupInfoHead:createClipHead(userItem, 105,"head_frame/avatar_size.png")
	csbNode:addChild(head)
	head:setPosition(pos)
	head:setScale(0.8)

	csbNode:getChildByName("Button_head"):setScale(0.8)

	csbNode:getChildByName("Text_name"):setString(userItem.szNickName) 

	csbNode:getChildByName("AtlasLabel_gold"):setString(ExternalFun.formatScoreText(userItem.lScore)) 
	-- csbNode:getChildByName("index"):setString(index.."/") 
	return ly
end

--申请上庄成功
function BankerLayer:onApplyBanker()
	self.csbNode:getChildByTag(TAG_ENUM.BT_APPLY):setVisible(false)
	self.csbNode:getChildByTag(TAG_ENUM.BT_CANCEL):setVisible(true)
	self.csbNode:getChildByTag(TAG_ENUM.BT_REFUSE):setVisible(false)
end

--申请下庄成功
function BankerLayer:onCancelBanker()
	self.csbNode:getChildByTag(TAG_ENUM.BT_APPLY):setVisible(true)
	self.csbNode:getChildByTag(TAG_ENUM.BT_CANCEL):setVisible(false)
	self.csbNode:getChildByTag(TAG_ENUM.BT_REFUSE):setVisible(false)
end

function BankerLayer:showMeBankerBet()
	self.csbNode:getChildByTag(TAG_ENUM.BT_APPLY):setVisible(false)
	self.csbNode:getChildByTag(TAG_ENUM.BT_CANCEL):setVisible(false)
	self.csbNode:getChildByTag(TAG_ENUM.BT_REFUSE):setVisible(true)
end

function BankerLayer:onGameMessageBankerQuit()
	if self._gameView:isMeIsBanker() then
		self.csbNode:getChildByTag(TAG_ENUM.BT_REFUSE):setEnabled(false)
	end
end

--切换庄家
function BankerLayer:changeBanker()
	--自己坐庄
	if self._gameView:isMeIsBanker() then
		self.csbNode:getChildByTag(TAG_ENUM.BT_APPLY):setVisible(false)
		self.csbNode:getChildByTag(TAG_ENUM.BT_CANCEL):setVisible(false)
		self.csbNode:getChildByTag(TAG_ENUM.BT_REFUSE):setVisible(true)
	else
		self.csbNode:getChildByTag(TAG_ENUM.BT_APPLY):setVisible(true)
		self.csbNode:getChildByTag(TAG_ENUM.BT_CANCEL):setVisible(false)
		self.csbNode:getChildByTag(TAG_ENUM.BT_REFUSE):setVisible(false)
	end
end

function BankerLayer:resetBtnState()
	self.csbNode:getChildByTag(TAG_ENUM.BT_REFUSE):setEnabled(true)
end

--更新庄家列表
function BankerLayer:updateBankerList()
	self.userList = self._gameView:getDataMgr():getApplyBankerUserList()
	self.applyList:reloadData()




end

function BankerLayer:onButtonClickedEvent(tag, ref)
	if tag == TAG_ENUM.BT_EXIT or tag == TAG_ENUM.BT_MASK then
		self:setVisible(false)
	elseif tag == TAG_ENUM.BT_APPLY then
		if self._gameView.my_gold < self._gameView.lApplyBankerCondition then
			-- showToast(self, "你的金币不足以申请庄家，申请失败！", 1)
			return
		end
		self._gameView:sendApplyBanker()
	elseif tag == TAG_ENUM.BT_CANCEL or tag == TAG_ENUM.BT_REFUSE then
		if self._gameView:isMeChair(self._gameView.wBankerUser) then
			if self._gameView.gameState ~= GAME_CMD.GAME_SCENE_FREE then
				self.csbNode:getChildByTag(TAG_ENUM.BT_REFUSE):setEnabled(false)
			end
		end
		self._gameView:sendCancelApply()
	end
end



return BankerLayer