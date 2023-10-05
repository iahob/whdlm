local ACT_TYPE = {
    image = 1,  --图片
    notice = 2, --公告
}

local uiActivity = env.uiBase:ctor{
    zorder = 1000,
}

function uiActivity:ctor(...)
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiActivity:init(parent)
    local csbNode = cc.CSLoader:createNode("LayerActivity.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode

    cc.funAddChildListener(csbNode, "Button_1", function() self:onClickQuit() end)
    cc.funAddChildListener(csbNode, "Sprite_4/Panel_1", function() self:onClickNotice() end)
    cc.funAddChildListener(csbNode, "Sprite_4/Panel_2", function() self:onClickAct() end)

    self.listView = cc.find(self.widget, "ListView_1")
    self.listView:setScrollBarEnabled(false)
    self.item = cc.find(self.widget, "Button_left_item")
    self.item:setTag(1)
    self.item:retain()
    self.item:setVisible(false)

    self:selectType(ACT_TYPE.notice)
    self:refreshActivitList()
end

function uiActivity:selectType(_type)
    cc.find(self.widget, "Sprite_4/Button_1"):setVisible(_type == ACT_TYPE.notice)
    cc.find(self.widget, "Sprite_4/Button_2"):setVisible(_type == ACT_TYPE.image)
    self._selectType = _type
end

function uiActivity:onClickQuit()
    self:destroy()
end

function uiActivity:onClickNotice()
    self:selectType(ACT_TYPE.notice)
end

function uiActivity:onClickAct()
    self:selectType(ACT_TYPE.image)
end

local dataList_test = {
    {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
    {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
    {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
    {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
    {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
    {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
    {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
}

function uiActivity:refreshActivitList(dataList_notice, dataList_act)
    if self._selectType == ACT_TYPE.notice then
        dataList = dataList_notice or dataList_test
    else
        dataList = dataList_act or dataList_test
    end

    local btn_callback = function(sender,touch_type)
        if self._curSelectIdx == sender:getTag() then
            sender:setHighlighted(true)
        end
        if touch_type == ccui.TouchEventType.ended then
            self:onClickItem(sender:getTag())
        end
    end

    self.listView:removeAllItems()
    for idx,data in ipairs(dataList) do
        local item = self.item:clone()
        item:setTag(idx)
        item:setVisible(true)
        item:addTouchEventListener(btn_callback)
        self:refreshActivityItem(item, data)
        self.listView:pushBackCustomItem(item)
    end
    self._curSelectIdx = 1
    self:updateSelect()
end

function uiActivity:refreshActivityItem(btn, data)
    btn:getChildByName("Text_1"):setString(data.tittle)
    btn:setHighlighted(isSelected)
end

function uiActivity:updateSelect()
    local items = self.listView:getItems()
    local items_count = table.getn(items)
    for i=1,items_count do
        local item = self.listView:getItem(i-1)
        item:setHighlighted(self._curSelectIdx == item:getTag())
    end
end

function uiActivity:onClickItem(whichOne)
    self._curSelectIdx = whichOne
    self:updateSelect()
end

function uiActivity:onExit()
    self.item:release()
end

return uiActivity