local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.line9.src"
local TrendLayerItem = appdf.req(module_pre .. ".views.layer.TrendLayerItem")
local TrendLayer = class("TrendLayer", function (  )
    local colorLayer = cc.LayerColor:create(cc.c4b(0,0,0,100))
    return colorLayer
end)

TrendLayer.BT_CLOSE = 1

--构造
function TrendLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("TrendLayer/TrendLayer.csb", self)
    self._csbNode:setPosition(appdf.WIDTH/2,appdf.HEIGHT/2 )  
    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender)
        end
    end

    --背景
    local sp_bg = self._csbNode:getChildByName("sprite_bg")
    self.m_spBg = sp_bg

    --关闭按钮
    local btn = self._csbNode:getChildByName("Button_close")
    btn:setTag(TrendLayer.BT_CLOSE)
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
--            self:removeFromParent()
            self:showLayer(false)
        end
    end)

    self.m_tabList = {}
    self._listView = cc.TableView:create(cc.size(860,430))
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_BOTTOMUP)  
    self._listView:setPosition(-430,-230)
    self._listView:setDelegate()
    self._listView:addTo(self._csbNode)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()
end

function TrendLayer:updateList(list)
    self.m_tabList = clone(list)
    if 0 < #self.m_tabList then
        self._listView:reloadData()
    end
end

-- 子视图大小
function TrendLayer:cellSizeForTable(view, idx)
    return 803, 140
end

function TrendLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then
        cell = TrendLayerItem:create()
        cell._csbNode:setPosition(430, 70)
    else
        --        cell:removeAllChildren()
    end
    local iteminfo = self.m_tabList[idx + 1]
    if iteminfo then
        cell._csbNode:getChildByName("item"):getChildByName("Sprite_line"):setSpriteFrame(string.format("game/im_%d_0.png", iteminfo.cbLineNumber + 1))
        cell._csbNode:getChildByName("item"):getChildByName("Sprite_country"):initWithFile(string.format("TrendLayer/country/country_%d.png", iteminfo.cbLineType))
        cell._csbNode:getChildByName("item"):getChildByName("Text_times"):setString("x " .. iteminfo.cbLineCount)
        cell._csbNode:getChildByName("item"):getChildByName("Text_name"):setString(iteminfo.szNickName)

        local score = iteminfo.lLineScore
        local node = cell._csbNode:getChildByName("item"):getChildByName("Art_gold")
        if score >= 10000 then
            if score >= 1000000 then
                score = math.floor(score / 10000)
            else
                local a,b = math.modf(score / 10000)
                if b < 0.1 then
                    score = a
                else
                    score = string.format("%.1f",score / 10000)
                end
            end
            node:getChildByName("Sprite_wan"):setPosition(string.len(score) * 19, 1)
            node:setPosition(180, -9)
            node:getChildByName("Sprite_wan"):setVisible(true)
        else
            node:setPosition(196, -9)
            node:getChildByName("Sprite_wan"):setVisible(false)
        end
        node:setString(score)

        local year = string.sub(iteminfo.szWinTime, 1, 4)
        local month = string.sub(iteminfo.szWinTime, 5, 6)
        local day = string.sub(iteminfo.szWinTime, 7, 8)
        local hour = string.sub(iteminfo.szWinTime, 9, 10)
        local minute = string.sub(iteminfo.szWinTime, 11, 12)
--        local second = string.sub(iteminfo.szWinTime, 13, 14)
        local date = year .. "/" .. month .. "/" .. day .. "  " .. hour .. ":" .. minute
--         .. ":" .. second
        cell._csbNode:getChildByName("item"):getChildByName("Text_time"):setString(date)
        return cell
    end
    return cell
end

-- 子视图数目
function TrendLayer:numberOfCellsInTableView(view)
    return #self.m_tabList
end

function TrendLayer:showLayer( var )
    self:setVisible(var)
end

--触摸回调
function TrendLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
--        self:removeFromParent()
        self:showLayer(false)
    end
    return self:isVisible()
end

return TrendLayer