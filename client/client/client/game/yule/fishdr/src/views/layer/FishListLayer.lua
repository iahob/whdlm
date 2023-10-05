local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local FishListLayer = class("FishListLayer", cc.Layer)
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

FishListLayer.BT_MASK = 1;
FishListLayer.BT_CLOSE = 2;

--构造
function FishListLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("csb/FishListLayer.csb", self);
    self._csbNode:setPosition(0,0);

   
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender);
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(FishListLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    local btn_close = self._csbNode:getChildByName("btn_close");
    btn_close:setTag(FishListLayer.BT_CLOSE);
    btn_close:addTouchEventListener(btnEvent);

    local panel = self._csbNode:getChildByName("panel");
    self.m_fTwo = panel:getContentSize().height / 2;

    self.m_tabList = {2,3,4,5,6,7,8,9,10,12,15,18,20,25,30,40,150,200,300,400,1000}
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

    self:updateList();
end

function FishListLayer:rewardPoolFresh(rewardData)
    self.rewardData = rewardData;

    self:updateList();
end

function FishListLayer:updateList()
    self.m_nListCount =  math.ceil(21/6);

    if 0 < self.m_nListCount then
        self._listView:reloadData();
    end
end

-- 子视图大小
function FishListLayer:cellSizeForTable(view, idx)
    -- return 180, 130;
    return view:getViewSize().width, 140;
end

function FishListLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell();
    if not cell then        
        cell = cc.TableViewCell:new();
    else
        cell:removeAllChildren();
    end

    idx = self.m_nListCount - (idx + 1);
    idx = idx * 6 + 1;
    -- CMD_MB_S_QueryVideoInfoResult
    -- 第一个cell
    local iteminfo = self.m_tabList[idx];
    if nil == iteminfo then
        return cell;
    end

    if nil ~= iteminfo then

        for i = 1, 6 do
            local index = idx + i - 1;

            if index <= 21 then
                local csb = self:createRecordItem(index, cell);
                csb:setPosition(cc.p(90 + 180*(i -1), 65));
            end
        end

        
    end

    return cell;
end

function FishListLayer:createRecordItem(idx, parent)

    local csb = ExternalFun.loadCSB("csb/FishListItem.csb", parent)

    if nil ~= parent then
        -- 房号
        local fish_icon = csb:getChildByName("fish_icon");
        fish_icon:setTexture("fishlist/fish_" .. idx ..".png");

        local multiple_num = csb:getChildByName("multiple_num");
        multiple_num:setString(self.m_tabList[idx]);

        local multiple_max = csb:getChildByName("multiple_max");

        if idx == #self.m_tabList then
            multiple_num:setVisible(false);
            multiple_max:setVisible(true);
        else
            multiple_num:setVisible(true);
            multiple_max:setVisible(false);
        end
        
        local multiple_icon = csb:getChildByName("multiple_icon");
    end

    return csb;
end

-- 子视图数目
function FishListLayer:numberOfCellsInTableView(view)
    return self.m_nListCount;
    -- return 10;
end

function FishListLayer:showLayer( var )
    self:setVisible(var);
end

--按钮回调方法
function FishListLayer:onButtonClickedEvent(tag, sender)
    
    if FishListLayer.BT_MASK == tag then
        self:removeFromParent();
    elseif FishListLayer.BT_CLOSE == tag then
        self:removeFromParent();
    end
end

--触摸回调
function FishListLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation();
    local m_spBg = self.m_spBg;
    pos = m_spBg:convertToNodeSpace(pos);
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height);
    if false == cc.rectContainsPoint(rec, pos) then
       self:removeFromParent();
    end
end

function FishListLayer:onTouchEnded(touch, event)

end

return FishListLayer