local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowhz.src.models.CMD_Game")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local uiBase = appdf.req(appdf.EXTERNAL_SRC .. "uiBase")

local uiLocation = uiBase:ctor{
    zorder = 120,
}

function uiLocation:ctor(...)
    -- cc.FileUtils:getInstance():addSearchPath("game/yule/sparrowhz/res/")
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiLocation:init(parent, playerList, playerNum)
    local csbNode = cc.CSLoader:createNode("LocationLayer.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode
    cc.funAddChildListener(csbNode, "Panel_bg", function() self:onClickQuit() end)
    cc.funAddChildListener(csbNode, "Button_quit", function() self:onClickQuit() end)

    self.playerList = playerList
    self.playerNum = playerNum or 4
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

local pos = {{x=1007,y=666},{x=95,y=466},{x=95,y=236},{x=1240,y=466}}

local test_playerList = {
    {ipStr="172.16.3.63", szPlaceName="深圳市中建钢构大厦", dLongitude=114.3181514740, dLatitude=30.5278142506, viewid=1},
    {ipStr="172.16.3.63", szPlaceName="深圳市中建钢构大厦", dLongitude=112.9392814636, dLatitude=30.3892390220, viewid=2},
    {ipStr="172.16.3.63", szPlaceName="深圳市中建钢构大厦", dLongitude=115.3181514740, dLatitude=32.3892390220, viewid=3},
    {ipStr="172.16.3.63", szPlaceName="深圳市中建钢构大厦", dLongitude=110.3181514740, dLatitude=31.3892390220, viewid=4},
}

function uiLocation:refresh(playerList)
    self._playerNode = cc.find(self.widget, "panel_node")
    self._playerNode:retain()
    self._playerNode:removeFromParent(false)

    local playerList = playerList or test_playerList

    for i=1,self.playerNum do
        local playerNode = self._playerNode:clone()
        self.widget:addChild(playerNode, 2, 2)
        local playerData = playerList[i]
        self:refreshPlayer(playerNode, playerData, playerData.viewid)
    end

    for i=1,self.playerNum do
        for j=1,self.playerNum do
            if i<j then
                local playerData_A = playerList[i]
                local playerData_B = playerList[j]
                local distance = algorithm(playerData_A.dLongitude, playerData_A.dLatitude, playerData_B.dLongitude, playerData_B.dLatitude)
                self:drawLine(pos[playerData_A.viewid], pos[playerData_B.viewid], distance)
            end
        end
    end
end

function uiLocation:refreshPlayer(playerNode, playerData)
    local lab_ip = playerNode:getChildByName("lab_ip")
    local lab_addr = playerNode:getChildByName("lab_addr")
    lab_ip:setString(playerData.ipStr)
    lab_addr:setString(playerData.szPlaceName)
    playerNode:setPosition(pos[playerData.viewid])
end

function uiLocation:drawLine(src, dst, distance_km)
    local detx = math.abs(dst.x-src.x)
    local dety = math.abs(dst.y-src.y)
    local length = math.sqrt(detx*detx+dety*dety)
    local rot
    local offsetx,offsety = 0,0
    if detx == 0 then
        rot = 90
        offsetx = 10
        offsety = 10
    else
        rot = math.atan((dst.y-src.y)/(dst.x-src.x))
        rot = rot / math.pi * 180.0
        rot = rot * (-1)
        offsety = 10
    end

    local mid = cc.pMidpoint(src, dst)
    local sprite = cc.Sprite:createWithSpriteFrame(cc.SpriteFrame:create(cmd.RES_PATH.."localtion/line_long.png", cc.rect(0,0,length,3)))
    sprite:setAnchorPoint(0.5, 0.5)
    sprite:setPosition(mid)
    sprite:setRotation(rot)
    sprite:addTo(self.widget)

    local lab = ccui.Text:create(string.format("%.2fkm",distance_km), "fonts/round_body.ttf", 20)
    lab:setColor(cc.c4b(255,255,0,255))
    local pos = cc.pAdd(mid, cc.p(offsetx, offsety))
    lab:setPosition(pos)
    lab:setRotation(rot)
    self.widget:addChild(lab, 10)
end

function uiLocation:onExit()
    if self._playerNode then
        self._playerNode:release()
        self._playerNode = nil
    end
    if self._handler then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self._handler)
        self._handler = nil
    end
end

return uiLocation