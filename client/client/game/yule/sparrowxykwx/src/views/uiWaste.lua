--荒庄
local uiWaste = env.uiBase:ctor{
    zorder = 1000,
}

function uiWaste:ctor(...)
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiWaste:init(parent)
    local csbNode = cc.CSLoader:createNode("Layer_waste.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode

    cc.funAddChildListener(csbNode, "Image_1/Button_1", function() self:onClickQuit() end)
    -- self:refresh(dataList)
end

local dataList_test = {
    {"nick_a", 30},
    {"nick_b", -30},
    {"nick_c", -30},
}

function uiWaste:refresh(dataList)
    local dataList = dataList or dataList_test
    for k,data in ipairs(dataList) do
        local node = cc.find(self.widget, "Image_1/player_" .. k)
        self:refreshOne(node, data)
    end

    for k=#dataList+1,3 do
        local node = cc.find(self.widget, "Image_1/player_" .. k)
        node:setVisible(false)
    end
end

function uiWaste:refreshOne(node, data)
    local lab_nick = cc.find(node, "Text_nick")
    local lab_gang = cc.find(node, "Text_gang")
    local name,score = unpack(data)
    lab_nick:setString(name)
    lab_gang:setString("杠分:" .. score)
end

function uiWaste:onClickQuit()
    print("uiWaste:onClickQuit==>", self.callback)
    if type(self.callback) == "function" then
        self.callback()
    end
    self:destroy()
end

function uiWaste:setCallbackAfterDestroy(callback)
    self.callback = callback
end

function uiWaste:onExit()
end

return uiWaste