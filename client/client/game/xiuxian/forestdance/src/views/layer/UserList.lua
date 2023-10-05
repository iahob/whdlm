--
-- Author: Tang
-- Date: 2016-10-13 16:08:36
--
local UserList = class("UserList", function(module,scene)

	return display.newLayer()
end)

local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.EXTERNAL_SRC .. "PopupInfoHead"
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"
local g_var = ExternalFun.req_var
local Tag = 
{
	tag_btnClose    = 1
}
function UserList:ctor(module,scene)

	self._dataModule = module
	self._scene = scene
	self:initCsb()
	self:InitData()
end

function UserList:initCsb()

  local bg =  ccui.ImageView:create()
  bg:setContentSize(cc.size(yl.WIDTH, yl.HEIGHT))
  bg:setScale9Enabled(true)
  bg:setAnchorPoint(cc.p(0.5,0.5))
  bg:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  bg:setTouchEnabled(true)
  self:addChild(bg)

  bg:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
          self:runAction(cc.MoveTo:create(0.2,cc.p(0,-yl.HEIGHT)))
        end
  end)

  --加载CSB
  local csbnode = cc.CSLoader:createNode("game_res/UserList.csb");
  csbnode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  self._rootNode = csbnode
  self:addChild(csbnode)

  --初始化按钮事件
  local btn = csbnode:getChildByName("btn_close")
  btn:setTag(Tag.tag_btnClose)
  btn:addTouchEventListener(handler(self, self.OnEvent))

  self:setCurUserInfo()
end

function UserList:InitData()
 	local tablePanle = self._rootNode:getChildByName("table_panel")
	self.m_pTableView = cc.TableView:create(cc.size(560, 290))
	self.m_pTableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
	self.m_pTableView:setPosition(cc.p(0,0))
	self.m_pTableView:setDelegate()
	self.m_pTableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self.m_pTableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self.m_pTableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	tablePanle:addChild(self.m_pTableView)
end

function UserList:OnEvent(sender,eventType)
	local tag = sender:getTag()
	if eventType == ccui.TouchEventType.ended then
		if tag == Tag.tag_btnClose then
			self:runAction(cc.MoveTo:create(0.2,cc.p(0,-yl.HEIGHT)))
		end
	end
end

function UserList:popLayer()
    self:stopAllActions()
    self:setPosition(cc.p(0,-yl.HEIGHT))
	self:runAction(cc.MoveTo:create(0.2,cc.p(0,0)))
end

function UserList:setCurUserInfo(userInfo)
  local useritem = self._scene:GetMeUserItem()
  local default = {nick=useritem.szNickName,score=useritem.lScore,win="0",item=useritem}
  if nil ~= userInfo then
  	default.nick = userInfo.nick 
  	default.score= userInfo.score
  	default.win  = userInfo.win
  	default.item= userInfo.item 
  end

  local head = g_var(PopupInfoHead):createClipHead(default.item, 79)
  self._rootNode:removeChildByTag(10)
  head:setPosition(cc.p(-233,183))
  head:setTag(10)
  head:enableInfoPop(true, cc.p(yl.WIDTH/2-150,yl.HEIGHT/2-150),cc.p(0.5,0.5))
  self._rootNode:addChild(head)

  --庄家昵称
  if not self._curUserNick then
  	self._curUserNick = g_var(ClipText):createClipText(cc.size(120, 22),default.nick,"fonts/round_body.ttf",22)
  	self._curUserNick:setAnchorPoint(cc.p(0.0,0.5))
  	self._curUserNick:setPosition(cc.p(-117,198))
  	self._rootNode:addChild(self._curUserNick)
  else
  	self._curUserNick:setString(default.nick)
  end

  --庄家分数
  if not self._curUserScore then
  	self._curUserScore = g_var(ClipText):createClipText(cc.size(120, 22),default.score,"fonts/round_body.ttf",22)
  	self._curUserScore:setAnchorPoint(cc.p(0.0,0.5))
  	self._curUserScore:setPosition(cc.p(75,198))
  	self._rootNode:addChild(self._curUserScore)
  else
  	self._curUserScore:setString(default.score)
  end

  --输赢详细
  if not self._curUserWin then
  	self._curUserWin = g_var(ClipText):createClipText(cc.size(120, 22),default.win,"fonts/round_body.ttf",22)
  	self._curUserWin:setAnchorPoint(cc.p(0.0,0.5))
  	self._curUserWin:setPosition(cc.p(-117,159))
  	self._rootNode:addChild(self._curUserWin)
  else
  	self._curUserWin:setString(default.win)
  end
end

function UserList:reloadData()
	self.m_pTableView:reloadData()
end

--------------------------------------------------------------------tableview
function UserList.cellSizeForTable( view, idx )
	return 560,90
end

function UserList:numberOfCellsInTableView( view )

	if 0 == #self._dataModule:getUserList() then
		return 0
	else
		return #self._dataModule:getUserList()
	end
end

function UserList:tableCellAtIndex( view, idx )
	--print("idx is =================================================================== >" .. idx)
	local cell = view:dequeueCell()
	local userlist = self._dataModule:DeepCopy(self._dataModule.m_UserList)
	self._dataModule:reverse(userlist)
	
	local useritem = userlist[idx+1]
	if nil == cell then
		cell = cc.TableViewCell:new()
		--cell背景
		local csbnode = cc.CSLoader:createNode("game_res/UserNode.csb");
  		csbnode:setPosition(280, 45)
  		cell:addChild(csbnode)
  		cell._csb = csbnode

  		--玩家头像
		local headBG = csbnode:getChildByName("head_bg")
		local head = g_var(PopupInfoHead):createClipHead(useritem, 47)
		head:setPosition(cc.p(headBG:getContentSize().width/2,headBG:getContentSize().height/2))
		head:setTag(1) 
		local popPos = head:convertToWorldSpace(cc.p(head:getAnchorPointInPoints().x,head:getAnchorPointInPoints().y))
		
		
  		head:enableInfoPop(true, cc.p(yl.WIDTH/2-150,yl.HEIGHT/2-150),cc.p(0.5,0.5))
		headBG:addChild(head)

		--玩家昵称
		local nick =  g_var(ClipText):createClipText(cc.size(150, 20),useritem.szNickName)
		nick:setTag(2)
		nick:setAnchorPoint(cc.p(0.0,0.5))
		nick:setPosition(cc.p(-198,0))
		csbnode:addChild(nick)

		--玩家游戏币
		local coin =  g_var(ClipText):createClipText(cc.size(150, 20),ExternalFun.numberThousands(useritem.lScore))  
		coin:setAnchorPoint(cc.p(0.0,0.5))
		coin:setTag(3)
		coin:setPosition(35, 0)
		csbnode:addChild(coin)
	else
		cell:removeAllChildren()
		--cell背景
		local csbnode = cc.CSLoader:createNode("game_res/UserNode.csb");
  		csbnode:setPosition(280, 45)
  		cell:addChild(csbnode)
  		cell._csb = csbnode

  		--玩家头像
		local headBG = csbnode:getChildByName("head_bg")
		local head = g_var(PopupInfoHead):createClipHead(useritem, 47)
		head:setPosition(cc.p(headBG:getContentSize().width/2,headBG:getContentSize().height/2))
		head:setTag(1)
  		head:enableInfoPop(true, cc.p(yl.WIDTH/2-150,yl.HEIGHT/2-150),cc.p(0.5,0.5))
		headBG:addChild(head)

		--玩家昵称
		local nick =  g_var(ClipText):createClipText(cc.size(150, 20),useritem.szNickName)
		nick:setTag(2)
		nick:setAnchorPoint(cc.p(0.0,0.5))
		nick:setPosition(cc.p(-198,0))
		csbnode:addChild(nick)

		--玩家游戏币
		local coin =  g_var(ClipText):createClipText(cc.size(150, 20),ExternalFun.numberThousands(useritem.lScore))  
		coin:setAnchorPoint(cc.p(0.0,0.5))
		coin:setTag(3)
		coin:setPosition(35, 0)
		csbnode:addChild(coin)

	end
	return cell
end

-----------------------------------------------------------------------------
return UserList