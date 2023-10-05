--
-- Author: Tang
-- Date: 2016-10-13 16:08:13
--
local BankerList = class("BankerList", function(module,scene)
	return display.newLayer()
end)
local Tag = 
{
	tag_applyBanker = 0,
	tag_btnClose    = 1
}
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.EXTERNAL_SRC .. "PopupInfoHead"
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"
local g_var = ExternalFun.req_var
local module_pre = "game.xiuxian.forestdance.src"
--local module_pre = "game.battle.forestdance.src"

local cmd = module_pre .. ".models.cmd_game"

function BankerList:ctor(module,scene)
	self._dataModule = module
	self._scene = scene
	self:initCsb()
	self:InitData()
end

function BankerList:initCsb()
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
  local csbnode = cc.CSLoader:createNode("game_res/BankerList.csb");
  csbnode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  self._rootNode = csbnode
  self:addChild(csbnode)

  --初始化按钮事件
  local btn = csbnode:getChildByName("btn_close")
  btn:setTag(Tag.tag_btnClose)
  btn:addTouchEventListener(handler(self, self.OnEvent))

  btn = csbnode:getChildByName("btn_apply")
  btn:setTag(Tag.tag_applyBanker)
  btn:addTouchEventListener(handler(self, self.OnEvent))
  btn:setEnabled(false)

  --更新按钮状态
  self:updateControl()
  --设置庄家信息
  self:setCurBankerInfo()
end

function BankerList:InitData()
 	local tablePanle = self._rootNode:getChildByName("table_panel")
	self.m_pTableView = cc.TableView:create(cc.size(560, 220))
	self.m_pTableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
	self.m_pTableView:setPosition(cc.p(0,0))
	self.m_pTableView:setDelegate()
	self.m_pTableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self.m_pTableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self.m_pTableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	tablePanle:addChild(self.m_pTableView)
end

function BankerList:updateControl()
	local applyBtn = self._rootNode:getChildByName("btn_apply")
--	local userItem = self._scene:GetMeUserItem()
--	if userItem.lScore  < self._scene.m_lApplyBankerScore  then
--		applyBtn:setEnabled(false)
--	else
--		if self._scene._gameStatus ~= g_var(cmd).IDI_GAME_FREE and userItem.wChairID == self._scene.m_wBankerUser  then
			applyBtn:setEnabled(false)
--			return
--		end
--		applyBtn:setEnabled(true)
--	end
end

function BankerList:OnEvent(sender,eventType)
	local tag = sender:getTag()
	if eventType == ccui.TouchEventType.ended then
		if tag == Tag.tag_btnClose then
			self:runAction(cc.MoveTo:create(0.2,cc.p(0,-yl.HEIGHT)))
		elseif tag == Tag.tag_applyBanker then
			if self._scene._applyStatus == g_var(cmd).applyNone then
				return
			end

			local userItem = self._scene:GetMeUserItem()
			local  dataBuffer = CCmd_Data:create(3)
			local applyAction = (self._scene._applyStatus == g_var(cmd).unApply) and true or false
          	dataBuffer:pushbool(applyAction)
          	dataBuffer:pushword(userItem.wChairID)

          	self._scene:SendData(g_var(cmd).SUB_C_BANKER_OPERATE,dataBuffer)
          	self._scene._applyStatus = g_var(cmd).applyNone
		end
	end
end

function BankerList:popLayer()
	self:runAction(cc.MoveTo:create(0.2,cc.p(0,0)))
end

function BankerList:setApplyStatus()
	local btn = self._rootNode:getChildByName("btn_apply")
	
	local applyStatus = self._scene._applyStatus
	if applyStatus == g_var(cmd).applyed  then
		btn:loadTextures("anniu20.png","anniu21.png","",UI_TEX_TYPE_PLIST)
	elseif applyStatus == g_var(cmd).unApply then
		btn:loadTextures("anniu16.png","anniu17.png","",UI_TEX_TYPE_PLIST)		
	end
end

function BankerList:setCurBankerInfo(bankerInfo)
  local default = {nick="系统坐庄",score="0",win="0"}
  if nil ~= bankerInfo then
  	default.nick = bankerInfo.nick
  	default.score= bankerInfo.score
  	default.win  = bankerInfo.win
  	default.item = bankerInfo.item
  end

  local head = g_var(PopupInfoHead):createClipHead(default.item, 79)
  self._rootNode:removeChildByTag(10)
  head:setPosition(cc.p(-233,183))
  head:setTag(10)
  head:enableInfoPop(true, cc.p(yl.WIDTH/2-150,yl.HEIGHT/2-150),cc.p(0.5,0.5))
  self._rootNode:addChild(head)

  --庄家昵称
  if not self._curBankerNick then
  	self._curBankerNick = g_var(ClipText):createClipText(cc.size(120, 22),default.nick,"fonts/round_body.ttf",22)
  	self._curBankerNick:setAnchorPoint(cc.p(0.0,0.5))
  	self._curBankerNick:setPosition(cc.p(-117,198))
  	self._rootNode:addChild(self._curBankerNick)
  else
  	self._curBankerNick:setString(default.nick)
  end

  --庄家分数
  if not self._curBankerScore then
  	self._curBankerScore = g_var(ClipText):createClipText(cc.size(120, 22),default.score,"fonts/round_body.ttf",22)
  	self._curBankerScore:setAnchorPoint(cc.p(0.0,0.5))
  	self._curBankerScore:setPosition(cc.p(75,198))
  	self._rootNode:addChild(self._curBankerScore)
  else
  	self._curBankerScore:setString(default.score)
  end

  --输赢详细
  if not self._curBankerWin then
  	self._curBankerWin = g_var(ClipText):createClipText(cc.size(120, 22),default.win,"fonts/round_body.ttf",22)
  	self._curBankerWin:setAnchorPoint(cc.p(0.0,0.5))
  	self._curBankerWin:setPosition(cc.p(-117,159))
  	self._rootNode:addChild(self._curBankerWin)
  else
  	self._curBankerWin:setString(default.win)
  end
end

function BankerList:reloadData()
	local item =  self._dataModule:getUserByChair(self._scene:getUserList(),self._scene.m_wBankerUser)
	if item then
		--设置当前庄家信息
		local bankerInfo = {}
		bankerInfo.nick = item.szNickName
	  	bankerInfo.score= item.lScore
	  	bankerInfo.win  = self._scene.m_lBankerGrade
	  	bankerInfo.item = item
		self:setCurBankerInfo(bankerInfo)
	else
		--系统坐庄
		self:setCurBankerInfo()		
	end

	--申请庄家按钮
	--self:setApplyStatus()
	self:updateControl()

	--重载数据
	self.m_pTableView:reloadData()
end

--------------------------------------------------------------------tableview
function BankerList.cellSizeForTable( view, idx )
	return 560,90
end

function BankerList:numberOfCellsInTableView( view )

	--print("#self._dataModule:getBankList() is =========================>"..#self._dataModule:getBankList())
	if 0 == #self._dataModule:getBankList() then
		return 0
	else
		return #self._dataModule:getBankList()
	end
end

function BankerList:tableCellAtIndex( view, idx )
	--print("idx is =================================================================== >" .. idx)
	local cell = view:dequeueCell()
	local list = self._dataModule:DeepCopy(self._dataModule.m_BankList)
	self._dataModule:reverse(list)
	
	local useritem = list[idx+1]
	assert(useritem)
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
return BankerList