--
--
--庄家申请列表
local module_pre = "game.yule.baccaratnew.src"

local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var;
local BankerItem = module_pre .. ".views.layer.userlist.BankerItem"

local ApplyListLayer = class("ApplyListLayer", cc.Layer)
ApplyListLayer.BT_CLOSE = 1
ApplyListLayer.BT_APPLY_ON = 2
ApplyListLayer.BT_CANCEL_APPLY = 3
ApplyListLayer.BT_APPLY_DOWN = 4

function ApplyListLayer:ctor( viewParent)
	--注册事件
	local function onLayoutEvent( event )
		if event == "exit" then
			self:onExit()
        elseif event == "enterTransitionFinish" then
        	self:onEnterTransitionFinish()
        end
	end
	self:registerScriptHandler(onLayoutEvent)

	--
	self.m_parent = viewParent

	--用户列表
	self.m_userlist = {}

	--加载csb资源
	local csbNode = ExternalFun.loadCSB("bankerlist/ApplyListLayer.csb", self)

	local sp_bg = csbNode:getChildByName("sp_userlist_bg")
	self.m_spBg = sp_bg
	local content = csbNode:getChildByName("content")

	--用户列表
	local m_tableView = cc.TableView:create(content:getContentSize())
	m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
	m_tableView:setPosition(content:getPosition())
	m_tableView:setDelegate()
	m_tableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	m_tableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	csbNode:addChild(m_tableView)
	self.m_tableView = m_tableView;
	content:removeFromParent()

	--关闭按钮
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender);
		end
	end
	local btn = csbNode:getChildByName("close_btn")
	btn:setTag(ApplyListLayer.BT_CLOSE)
	btn:addTouchEventListener(btnEvent);

	--申请上庄
    self.btn_apply_on = csbNode:getChildByName("btn_apply_on");
    self.btn_apply_on:setTag(ApplyListLayer.BT_APPLY_ON);
    self.btn_apply_on:addTouchEventListener(btnEvent);

    --取消申请
    self.btn_cancel_apply = csbNode:getChildByName("btn_cancel_apply");
    self.btn_cancel_apply:setTag(ApplyListLayer.BT_CANCEL_APPLY);
    self.btn_cancel_apply:addTouchEventListener(btnEvent);

    --申请下庄
    self.btn_apply_off = csbNode:getChildByName("btn_apply_off");
    self.btn_apply_off:setTag(ApplyListLayer.BT_APPLY_DOWN);
    self.btn_apply_off:addTouchEventListener(btnEvent);

	--上庄条件限制
	local banker_limit = csbNode:getChildByName("banker_limit");
	banker_limit:setString(self.m_parent.m_llCondition);

	content:removeFromParent()
end

function ApplyListLayer:refreshList( userlist )
	self:setVisible(true)
	self.m_userlist = userlist
	self.m_tableView:reloadData()

	if nil == self.m_parent or nil == self.m_parent.getApplyState then
        self.btn_apply_on:setEnabled(false);
        self.btn_cancel_apply:setEnabled(false);
        self.btn_apply_off:setEnabled(false);
        return
    end

	-------------------------------------
	--获取当前申请状态
	local state = self.m_parent:getApplyState()	
	local str1 = nil
	local str2 = nil

	ExternalFun.enableBtn(self.m_btnApply, false)
	--未申请状态则申请、申请状态则取消申请、已申请则取消申请
	if state == self.m_parent._apply_state.kCancelState then  -- 取消申请
        self.btn_apply_on:setVisible(true);
        self.btn_cancel_apply:setVisible(false);
        self.btn_apply_off:setVisible(false);
        --申请条件限制
        ExternalFun.enableBtn(self.btn_apply_on, self.m_parent:getApplyable());
    elseif state == self.m_parent._apply_state.kApplyState then  --
        self.btn_apply_on:setVisible(false);
        self.btn_cancel_apply:setVisible(true);
        self.btn_apply_off:setVisible(false); 
        ExternalFun.enableBtn(self.btn_cancel_apply, true);
    elseif state == self.m_parent._apply_state.kApplyedState then
        self.btn_apply_on:setVisible(false);
        self.btn_cancel_apply:setVisible(false);
        self.btn_apply_off:setVisible(true);
        --取消上庄限制
        -- ExternalFun.enableBtn(self.btn_apply_off, self.m_parent:getCancelable());
        ExternalFun.enableBtn(self.btn_apply_off, not self.m_parent.bIsClickCancelBanker);
    end
end

function ApplyListLayer:refreshBtnState(  )
	if nil == self.m_parent or nil == self.m_parent.getApplyState then
        ExternalFun.enableBtn(self.btn_apply_on, false);
        ExternalFun.enableBtn(self.btn_cancel_apply, false);
        ExternalFun.enableBtn(self.btn_apply_off, false);
        return
    end

	--获取当前申请状态
	local state = self.m_parent:getApplyState()
	if state == self.m_parent._apply_state.kApplyedState then
		--已申请状态，下庄限制
		ExternalFun.enableBtn(self.btn_apply_off, self.m_parent:getCancelable())
	end
end

--tableview
function ApplyListLayer.cellSizeForTable( view, idx )
	return g_var(BankerItem).getSize()
end

function ApplyListLayer:numberOfCellsInTableView( view )
	if nil == self.m_userlist then
		return 0
	else
		return #self.m_userlist
	end
end

function ApplyListLayer:tableCellAtIndex( view, idx )
	local cell = view:dequeueCell()
	
	if nil == self.m_userlist then
		return cell
	end

	local useritem = self.m_userlist[idx+1].m_userItem
	local var_bRob = self.m_userlist[idx+1].m_bRob
	local item = nil

	if nil == cell then
		cell = cc.TableViewCell:new()
		item = g_var(BankerItem):create(self.m_parent)
		item:setPosition(view:getViewSize().width * 0.5, 0)
		item:setName("user_item_view")
		cell:addChild(item)
	else
		item = cell:getChildByName("user_item_view")
	end

	if nil ~= useritem and nil ~= item then
		local index = #self.m_userlist - idx;
		item:refresh(useritem,false, idx / #self.m_userlist, index)
	end

	return cell
end
--

function ApplyListLayer:onButtonClickedEvent( tag, sender )
	ExternalFun.playClickEffect()
	if ApplyListLayer.BT_CLOSE == tag then
		self:setVisible(false)
	elseif ApplyListLayer.BT_APPLY_ON == tag then
        self.m_parent:applyBanker(self.m_parent._apply_state.kApplyState);
    elseif ApplyListLayer.BT_CANCEL_APPLY == tag then
        self.m_parent:applyBanker(self.m_parent._apply_state.kCancelState);
    elseif ApplyListLayer.BT_APPLY_DOWN == tag then
        self.m_parent:applyBanker(self.m_parent._apply_state.kApplyedState);
    end
end

function ApplyListLayer:onApplyClickedEvent( tag,sender )
	ExternalFun.playClickEffect()
	if nil ~= self.m_parent then
		self.m_parent:applyBanker(self.m_parent.m_enApplyState)
	end
end

function ApplyListLayer:onExit()
	local eventDispatcher = self:getEventDispatcher()
	eventDispatcher:removeEventListener(self.listener)
end

function ApplyListLayer:onEnterTransitionFinish()
	self:registerTouch()
end

function ApplyListLayer:registerTouch()
	local function onTouchBegan( touch, event )
		return self:isVisible()
	end

	local function onTouchEnded( touch, event )
		local pos = touch:getLocation();
		local m_spBg = self.m_spBg
        pos = m_spBg:convertToNodeSpace(pos)
        local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
        if false == cc.rectContainsPoint(rec, pos) then
            self:setVisible(false)
        end        
	end

	local listener = cc.EventListenerTouchOneByOne:create();
	listener:setSwallowTouches(true)
	self.listener = listener;
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN );
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED );
    local eventDispatcher = self:getEventDispatcher();
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self);
end
return ApplyListLayer