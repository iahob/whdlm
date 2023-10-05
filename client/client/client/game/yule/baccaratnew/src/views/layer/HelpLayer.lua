local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local HelpLayer = class("HelpLayer", cc.Layer)

HelpLayer.BT_MASK = 1
HelpLayer.BT_ClOSE = 1

--构造
function HelpLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("help/HelpLayer.csb", self);
    csbNode:setPosition(0,0);
    self._csbNode = csbNode;

    --回调方法
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(HelpLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    --遮罩
    local btn_close = self._csbNode:getChildByName("btn_close");
    btn_close:setTag(HelpLayer.BT_ClOSE);
    btn_close:addTouchEventListener(btnEvent);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    --左
    local content = self._csbNode:getChildByName("panel");


    --用户列表
    local m_tableView = cc.TableView:create(content:getContentSize())
    -- local m_tableView = cc.TableView:create(cc.size(1000,500))
    m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    -- m_tableView:setPosition(content:getPosition())
    m_tableView:setPosition(content:getPosition())
    m_tableView:setDelegate()
    m_tableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    m_tableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self:addChild(m_tableView)
    self.m_tableView = m_tableView;
    -- content:removeFromParent()

    local userlist = {1};
    self:refreshList( userlist );
end

function HelpLayer:refreshList( userlist )

    self:setVisible(true)
    self.m_userlist = userlist
    self.m_tableView:reloadData()
end

--tableview
function HelpLayer.cellSizeForTable( view, idx )
    -- return g_var(UserItem).getSize()
    return 820,1800;
end

function HelpLayer:numberOfCellsInTableView( view )
    if nil == self.m_userlist then
        return 0
    else
        return #self.m_userlist
    end
end

function HelpLayer:tableCellAtIndex( view, idx )
    local cell = view:dequeueCell()
    
    if nil == self.m_userlist then
        return cell
    end

    local useritem = self.m_userlist[idx+1]
    local item = nil

    if nil == cell then
        cell = cc.TableViewCell:new()
        -- item = g_var(UserItem):create()
        -- item:setPosition(view:getViewSize().width * 0.5, 0)
        -- item:setName("user_item_view")

        item = display.newSprite("help/rule_info.png");
        item:setPosition(view:getViewSize().width * 0.5, 900)
        item:setName("user_item_view")

        cell:addChild(item)
    else
        item = cell:getChildByName("user_item_view")
    end

    if nil ~= useritem and nil ~= item then
        -- item:refresh(useritem, false, idx / #self.m_userlist)
    end

    return cell
end

function HelpLayer:onExit()
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:removeEventListener(self.listener)
end

function HelpLayer:onEnterTransitionFinish()
    self:registerTouch()
end

function HelpLayer:registerTouch()
    -- local function onTouchBegan( touch, event )
    --     return self:isVisible()
    -- end

    -- local function onTouchEnded( touch, event )
    --     local pos = touch:getLocation();
    --     local m_spBg = self.m_spBg
    --     pos = m_spBg:convertToNodeSpace(pos)
    --     local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    --     if false == cc.rectContainsPoint(rec, pos) then
    --         self:setVisible(false)
    --     end        
    -- end

    -- local listener = cc.EventListenerTouchOneByOne:create();
    -- listener:setSwallowTouches(true)
    -- self.listener = listener;
    -- listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN );
    -- listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED );
    -- local eventDispatcher = self:getEventDispatcher();
    -- eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self);
end

function HelpLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function HelpLayer:onButtonClickedEvent(tag, sender)
    
    if HelpLayer.BT_MASK == tag then
        self:removeFromParent();
    elseif HelpLayer.BT_ClOSE == tag then
        self:removeFromParent();
    end
end

--触摸回调
function HelpLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        -- local fadeout = cc.FadeOut:create(0.5)
        -- local call = cc.CallFunc:create(function( )
        --     self:removeFromParent()
        -- end)
        -- self._csbNode:runAction(cc.Sequence:create(fadeout, call))

        self:removeFromParent();
    end
end

function HelpLayer:onTouchEnded(touch, event)

end

return HelpLayer