local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local BankerListLayer = class("BankerListLayer", cc.Layer)
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

BankerListLayer.BT_MASK = 1;
BankerListLayer.BT_APPLY_ON = 2;
BankerListLayer.BT_CANCEL_APPLY = 3;
BankerListLayer.BT_APPLY_DOWN = 4;

--构造
function BankerListLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("bankerlist/ApplyListLayer.csb", self)
    self._csbNode:setPosition(0,0);

    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel");
    btn:setTag(BankerListLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    local close_btn = self._csbNode:getChildByName("close_btn");
    close_btn:setTag(BankerListLayer.BT_MASK);
    close_btn:addTouchEventListener(btnEvent);

    --申请上庄
    self.btn_apply_on = self._csbNode:getChildByName("btn_apply_on");
    self.btn_apply_on:setTag(BankerListLayer.BT_APPLY_ON);
    self.btn_apply_on:addTouchEventListener(btnEvent);

    --取消申请
    self.btn_cancel_apply = self._csbNode:getChildByName("btn_cancel_apply");
    self.btn_cancel_apply:setTag(BankerListLayer.BT_CANCEL_APPLY);
    self.btn_cancel_apply:addTouchEventListener(btnEvent);

    --申请下庄
    self.btn_apply_off = self._csbNode:getChildByName("btn_apply_off");
    self.btn_apply_off:setTag(BankerListLayer.BT_APPLY_DOWN);
    self.btn_apply_off:addTouchEventListener(btnEvent);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    --
    local banker_limit = self._csbNode:getChildByName("banker_limit");
   
    local panel = self._csbNode:getChildByName("content");

    self.m_tabList = {};
    -- self._listView = cc.TableView:create(cc.size(600,280))
    self._listView = cc.TableView:create(panel:getContentSize())
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_BOTTOMUP)  
    self._listView:setPosition(panel:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self._csbNode)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    -- self._listView:reloadData()
end

function BankerListLayer:freshApplyBankerLimit(lApplyBankerCondition)
    local roleGoldNum = self.m_parent:formatScoreText(lApplyBankerCondition);
    local banker_limit = self._csbNode:getChildByName("banker_limit");
    banker_limit:setString("携带金币数大于" .. roleGoldNum .. "才可申请上庄。");
end

function BankerListLayer:refreshList(userlist, lApplyBankerCondition)
    -- dump(userlist, "-------------------userlist-----------", 6)

    self:setVisible(true);
    self.m_tabList = userlist;
    self._listView:reloadData();

    if nil == self.m_parent or nil == self.m_parent.getApplyState then
        self.btn_apply_on:setEnabled(false);
        self.btn_cancel_apply:setEnabled(false);
        self.btn_apply_off:setEnabled(false);
        return
    end

    --获取当前申请状态
    local state = self.m_parent:getApplyState();

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

function BankerListLayer:refreshBtnState()
    if nil == self.m_parent or nil == self.m_parent.getApplyState then
        ExternalFun.enableBtn(self.btn_apply_on, false);
        ExternalFun.enableBtn(self.btn_cancel_apply, false);
        ExternalFun.enableBtn(self.btn_apply_off, false);
        return
    end

    --获取当前申请状态
    local state = self.m_parent:getApplyState();
    if state == self.m_parent._apply_state.kApplyedState then
        --已申请状态，下庄限制
        -- ExternalFun.enableBtn(self.btn_apply_off, self.m_parent:getCancelable());
        ExternalFun.enableBtn(self.btn_apply_off, not self.m_parent.bIsClickCancelBanker);
    end
end

-- 子视图大小
function BankerListLayer:cellSizeForTable(view, idx)
    return view:getViewSize().width, 48;
end

function BankerListLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    -- local useritem = self.m_tabList[idx+1].m_userItem;
    -- idx = #self.m_tabList - idx - 1;
    if self.m_tabList[idx + 1].m_userItem == nil then
        return;
    end

    local roleItem = self.m_tabList[idx + 1].m_userItem;
    if nil == roleItem then
        return cell 
    end

    local csNode = ExternalFun.loadCSB("bankerlist/bankerListItem.csb", cell)
    csNode:setPosition(view:getViewSize().width * 0.5, 0)
    csNode:setName("csbNode") 

    if roleItem then
        local index = #self.m_tabList - idx;

        -- local role_item = csNode:getChildByName("role_item");

        local role_rank = csNode:getChildByName("role_rank");
        role_rank:setString(index);


        local role_name = csNode:getChildByName("role_name");
        local name = string.EllipsisByConfig(roleItem.szNickName,360,string.getConfig("fonts/round_body.ttf" , 36));
        role_name:setString(name);
        
        -- local role_gold = role_item:getChildByName("role_gold");
        local gold_num = csNode:getChildByName("gold_num");
        
        local roleGoldNum = self.m_parent:formatScoreText(roleItem.lScore);
        gold_num:setString(roleGoldNum);
        
        return cell
    end
    return cell
end

function BankerListLayer:tableCellTouched(view, cell)
    
end

-- 子视图数目
function BankerListLayer:numberOfCellsInTableView(view)
    if nil == self.m_tabList then
        return 0
    else
        return #self.m_tabList;
    end
end

function BankerListLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function BankerListLayer:onButtonClickedEvent(tag, sender)
    if BankerListLayer.BT_MASK == tag then
        -- self.m_parent.m_applyListLayer = nil;
        -- self:removeFromParent();
        self:setVisible(false);
    elseif BankerListLayer.BT_APPLY_ON == tag then
        self.m_parent:applyBanker(self.m_parent._apply_state.kApplyState);
    elseif BankerListLayer.BT_CANCEL_APPLY == tag then
        self.m_parent:applyBanker(self.m_parent._apply_state.kCancelState);
    elseif BankerListLayer.BT_APPLY_DOWN == tag then
        self.m_parent:applyBanker(self.m_parent._apply_state.kApplyedState);
    end
end

--触摸回调
function BankerListLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        -- self.m_parent.m_applyListLayer = nil;
        -- self:removeFromParent();
        self:setVisible(false);
    end
end

function BankerListLayer:onTouchEnded(touch, event)

end

return BankerListLayer