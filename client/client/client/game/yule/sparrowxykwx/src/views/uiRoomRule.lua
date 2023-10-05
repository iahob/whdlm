local uiRoomRule = env.uiBase:ctor{
}

function uiRoomRule:ctor(...)
    self:init(...)
    return self
end

function uiRoomRule:init(parent)
    local csbNode = cc.CSLoader:createNode("Node_room_rule.csb")
    parent:addChild(csbNode)
    self.widget = csbNode
end

function uiRoomRule:refresh(data)
    local bmf_1 = cc.find(self.widget, "bmf_1")
    local bmf_2 = cc.find(self.widget, "bmf_2")
    bmf_1:setString(string.format("玩法: %s", data.channel))
    bmf_2:setString(string.format("%s%s",data.maMode, data.maNum))
end

return uiRoomRule