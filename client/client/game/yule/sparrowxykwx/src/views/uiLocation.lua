local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local uiLocation = env.uiBase:ctor{
    zorder = 120,
}

function uiLocation:ctor(...)
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiLocation:init(parent, playerList, playerNum)
    local csbNode = cc.CSLoader:createNode("LocationLayer.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode
    cc.funAddChildListener(csbNode, "Panel_1", function() self:onClickQuit() end)
    cc.funAddChildListener(csbNode, "Button_1", function() self:onClickQuit() end)

    self.playerList = playerList
    self.playerNum = playerNum
    self._handler = cc.EventListenerCustom:create(yl.RY_NEARUSER_NOTIFY, function(...) self:onUserEnterNotify(...) end)
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithFixedPriority(self._handler, 1)

    for k,playerData in ipairs(playerList) do
        FriendMgr:getInstance():sendQueryUserLocation(playerData.uid)
    end
end

function uiLocation:onClickQuit()
    self:destroy()
end

local function algorithm(longitude1, latitude1, longitude2, latitude2)
    local function rad(d)
        return d*math.pi/180
    end

    local function round(n)
        return math.floor(n+0.5)
    end

    local a = rad(latitude1) - rad(latitude2)
    local b = rad(longitude1) - rad(longitude2)
    local s = 2 * math.asin(math.sqrt(math.pow(math.sin(a / 2), 2) +
        math.cos(latitude1) * math.cos(latitude2) * math.pow(math.sin(b / 2), 2)))

    s = s * 6371.393

    s = round(s * 10000) / 10000
    return s
end

--只需要负责ui刷新,不要管数据逻辑
function uiLocation:refresh(playerList)
    local line_left = cc.find(self.widget, "line_left")
    local line_right = cc.find(self.widget, "line_right")
    local line_top = cc.find(self.widget, "line_top")
    local sp_location_left = cc.find(self.widget, "sp_location_left")
    local sp_location_right = cc.find(self.widget, "sp_location_right")
    local sp_location_self = cc.find(self.widget, "sp_location_self")

    local playerDataLeft,playerDataRight,playerDataSelf
    for _,playerData in ipairs(playerList) do
        if playerData.viewid == 1 then
            playerDataSelf = playerData
        elseif playerData.viewid == 2 then
            playerDataRight = playerData
        elseif playerData.viewid == 3 then
            playerDataLeft = playerData
        end
    end

    line_left:setVisible(playerDataLeft~=nil)
    sp_location_left:setVisible(playerDataLeft~=nil)
    line_right:setVisible(playerDataRight~=nil)
    sp_location_right:setVisible(playerDataRight~=nil)

    line_top:setVisible(playerDataLeft~=nil and playerDataRight~=nil)

    --距离
    if playerDataLeft then
        local distance_left = algorithm(playerDataLeft.dLongitude, playerDataLeft.dLatitude, playerDataSelf.dLongitude, playerDataSelf.dLatitude)
        local distanceStr = string.format("%.2fkm", distance_left)
        line_left:getChildByName("text"):setString(distanceStr)

        sp_location_left:getChildByName("text_ip"):setString(playerDataLeft.ipStr)
        sp_location_left:getChildByName("text_addr"):setString(playerDataLeft.szPlaceName)
    end

    if playerDataRight then
        local distance_right = algorithm(playerDataRight.dLongitude, playerDataRight.dLatitude, playerDataSelf.dLongitude, playerDataSelf.dLatitude)
        local distanceStr = string.format("%.2fkm", distance_right)
        line_right:getChildByName("text"):setString(distanceStr)

        sp_location_right:getChildByName("text_ip"):setString(playerDataRight.ipStr)
        sp_location_right:getChildByName("text_addr"):setString(playerDataRight.szPlaceName)
    end

    if playerDataLeft and playerDataRight then
        local distance_top = algorithm(playerDataRight.dLongitude, playerDataRight.dLatitude, playerDataSelf.dLongitude, playerDataSelf.dLatitude)
        local distanceStr = string.format("%.2fkm", distance_top)
        line_top:getChildByName("text"):setString(distanceStr)
    end

    sp_location_self:getChildByName("text_ip"):setString(playerDataSelf.ipStr)
    sp_location_self:getChildByName("text_addr"):setString(playerDataSelf.szPlaceName)
end

function uiLocation:onUserEnterNotify(event)
    local nearuser = event.msg
    -- dump(nearuser, "#uiLocation:onUserEnterNotify==")

    for k,playerData in ipairs(self.playerList) do
        if playerData.uid == nearuser.dwUserID then

            playerData.ip = nearuser.dwClientAddr
            playerData.szPlaceName = nearuser.szPlaceName
            playerData.dLatitude = nearuser.dLatitude   --纬度
            playerData.dLongitude = nearuser.dLongitude --经度

            if playerData.szPlaceName == "" then
                playerData.szPlaceName = "未知"
            end

            local ipTable = ExternalFun.long2ip(nearuser.dwClientAddr)
            local ipStr = string.format("%s.%s.%s.%s", ipTable.m, ipTable.p, ipTable.s, ipTable.b)
            playerData.ipStr = ipStr
        end
    end

    -- 地点ip都收到了就刷新
    local playerNum = 0
    for k,v in pairs(self.playerList) do
        if v.viewid and v.ip then
            playerNum = playerNum + 1
        end
    end

    if playerNum >= self.playerNum then
        self:refresh(self.playerList)
    end
end

function uiLocation:onExit()
    if nil ~= self._handler then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self._handler)
        self._handler = nil
    end
end

return uiLocation