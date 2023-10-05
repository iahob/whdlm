local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")
local scheduler = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "utils.scheduler")

local uiFinalReport = env.uiBase:ctor{
    zorder = 1001
}

function uiFinalReport:ctor(parent, callback)
    self:init(parent, callback)
    self:registerNodeEvent()
    return self
end

local dataList_test = {
    {
        nick = "t1",
        id = 1001,
        score = 2,
        zimoCount = 0,
        huCount = 10,
        paoCount = 10,
        angangCount = 2,
        cbMingGang = 2,
        isMostWin = true,
    },
    {
        nick = "t2",
        id = 1002,
        score = 1,
        zimoCount = 0,
        huCount = 10,
        paoCount = 10,
        angangCount = 2,
        cbMingGang = 2,
        isMostWin = true,
    },
    {
        nick = "t3",
        id = 1003,
        score = -1,
        zimoCount = 0,
        huCount = 10,
        paoCount = 10,
        angangCount = 2,
        cbMingGang = 2,
        isMostWin = false,
    },
    roomNum = 157988,
    round = 1,
    topRound = 5,
}

local useritem = {
    dwUserID = 7431,
    dwCustomID = 7423,
    wFaceID = 0,
}

function uiFinalReport:init(parent, callback)
    local csbNode = cc.CSLoader:createNode("PrivateGameEndLayer.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode
    self.callback = callback
    cc.funAddChildListener(csbNode, "Image_bg/btn_share", function() self:onClickShare() end)
    cc.funAddChildListener(csbNode, "Image_bg/btn_quit", function() self:onClickQuit() end)

    self.schId = scheduler.scheduleGlobal(function()
        if self:isActive() and not self.widget:isVisible() then
            print("==uiFinalReport too long to existed in invisible mode! it's time to close!!!")
            scheduler.unscheduleGlobal(self.schId)
            --self:onClickQuit()
        end
    end, 8)
end

function uiFinalReport:onClickQuit()
    print("uiFinalReport:onClickQuit")
    if type(self.callback) == "function" then
        self.callback("quit")
    end
    self:destroy()
end

function uiFinalReport:onClickShare()
    print("uiFinalReport:onClickShare")
    if type(self.callback) == "function" then
        self.callback("share")
    end
    self:destroy()
end

--只需要负责ui刷新,不要管数据逻辑
function uiFinalReport:refresh(dataList)
    -- local dataList = dataList or dataList_test

    local nodeLose = cc.CSLoader:createNode("final_result/player_lose.csb")
    local nodeWin = cc.CSLoader:createNode("final_result/player_win.csb")
    local img_bg_lose = nodeLose:getChildByName("img_bg")
    local img_bg_win = nodeWin:getChildByName("img_bg")

    local lab_room_num = cc.find(self.widget, "Image_bg/lab_room_num")
    local lab_round_count = cc.find(self.widget, "Image_bg/lab_round_count")
    local Sprite_17 = cc.find(self.widget, "Sprite_17")
    lab_room_num:setString(dataList.roomNum)
    lab_round_count:setString(string.format("%s:%s",dataList.round, dataList.topRound))

    for idx,data in ipairs(dataList) do
        local infoNode = cc.find(self.widget, "Image_bg/infoNode_" .. idx)
        if data.score > 0 then
            infoNode:addChild(img_bg_win:clone())
        else
            infoNode:addChild(img_bg_lose:clone())
        end
        self:refreshOne(infoNode, data, data.useritem)
    end
end

function uiFinalReport:refreshOne(playerNode, data, useritem)
    local totalScore = data.score
    if totalScore >= 0 then
        totalScore = "/" .. math.abs(totalScore)
    else
        totalScore = "." .. math.abs(totalScore)
    end

    cc.find(playerNode, "img_bg/text_name"):setString(data.nick)
    cc.find(playerNode, "img_bg/text_id"):setString(data.id)
    cc.find(playerNode, "img_bg/lab_zimo"):setString(data.zimoCount)
    cc.find(playerNode, "img_bg/lab_hu"):setString(data.huCount)
    cc.find(playerNode, "img_bg/lab_dianpao"):setString(data.paoCount)
    cc.find(playerNode, "img_bg/lab_angang"):setString(data.angangCount)
    cc.find(playerNode, "img_bg/lab_minggang"):setString(data.cbMingGang)
    cc.find(playerNode, "img_bg/lab_result"):setString(totalScore)

    local imgWinMost = cc.find(playerNode, "img_bg"):getChildByName("Image_1")
    if imgWinMost then
        imgWinMost:setVisible(data.isMostWin)
        -- imgWin:setLocalZOrder(6) --setZOrder没用,这里有bug,只能加一层
    end

    local panel = cc.find(playerNode, "img_bg/Panel")

    local head = HeadSprite:createClipHead(useritem, 92, "faces/kwx_face_mask.png")
    head:setPosition(42, 42)
    panel:addChild(head)
end

function uiFinalReport:onExit()
    if self.schId then
        scheduler.unscheduleGlobal(self.schId)
        self.schId = nil
    end
end

return uiFinalReport