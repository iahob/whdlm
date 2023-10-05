local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.CMD_Game")
local uiHuTip = env.uiBase:ctor{
    zorder = 1000,
}

function uiHuTip:ctor(...)
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiHuTip:init(parent, cardLayer)
    local csbNode = cc.CSLoader:createNode("HuTipLayer.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.oneTipNode = cc.find(csbNode, "Image_whole_back/img_tip")
    self.oneTipNode:setTag(1)
    self.widget = csbNode
    self._cardLayer = cardLayer
end

local dataListTest = {
    {34,1,2},
    {34,1,2},
    {34,1,2},
    {34,1,2},
    {34,1,2},
    {34,1,2},
    {34,1,2},
}

function uiHuTip:refresh(dataList)
    dataList = dataList or dataListTest
    local totalCardType = #dataList
    local background = cc.find(self.widget, "Image_whole_back")
    local gap = 3
    background:setContentSize(cc.size(301+(totalCardType-1)*131, 153))

    local parentNode = self.oneTipNode:getParent()
    for i,data in ipairs(dataList) do
        local child = parentNode:getChildByTag(i)
        if not child then
            child = self.oneTipNode:clone()
            parentNode:addChild(child, 1, i)
        end
        local x = 180 + (i-1) * (131+gap)
        local y = 77
        child:setPosition(x,y)
        self:refreshOne(child, data)
    end
    for i=#dataList+1,10 do
        local child = parentNode:getChildByTag(i)
        if child then
            child:removeFromParent()
        end
    end
end

function uiHuTip:refreshOne(node, data)
    -- dump(data, "#uiHuTip:refreshOne")
    local cardValue,leftNum,multi = unpack(data)
    local AtlasLabel_num = cc.find(node, "AtlasLabel_num")
    local AtlasLabel_multi = cc.find(node, "AtlasLabel_multi")
    AtlasLabel_num:setString(leftNum)
    AtlasLabel_multi:setString(multi)
    local card = self._cardLayer:createOutOrActiveCardSprite(cmd.MY_VIEWID, cardValue, false)
    card:setPosition(29, 43)
    node:addChild(card)
end

function uiHuTip:onExit()
end

return uiHuTip