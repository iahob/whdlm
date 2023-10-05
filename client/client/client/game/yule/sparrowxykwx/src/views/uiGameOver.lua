local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.CMD_Game")
local CardLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.layer.CardLayer")
local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")

local uiGameOver = env.uiBase:ctor{
    zorder = yl.MAX_INT
}

local test_dataList = {
    {
        cardData = {0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35},
        piao = 2,
        gang = 10,
        fan = 12,
        totalScore = 100,
        nick = "test0",
        uid = "10023",
        isHuPai = true,
        outCardIdx = 5,
    },
    {
        cardData = {0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35},
        piao = 2,
        gang = 10,
        fan = 12,
        totalScore = 100,
        nick = "test0",
        uid = "10023",
        outCardIdx = 6,
    },
    {
        cardData = {0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35},
        piao = 2,
        gang = 10,
        fan = 12,
        totalScore = -100,
        nick = "test0",
        uid = "10023",
        outCardIdx = 7,
        isSelf =
        true,
    },
    maPai = {0x37,0x37,0x37},
}

local test_useritem = {
    dwUserID = 7431,
    dwCustomID = 7423,
    wFaceID = 0,
}


function uiGameOver:ctor(parent, callback)
    self:init(parent, callback)
    self:registerNodeEvent()
    return self
end

function uiGameOver:init(parent, callback)
    local csbNode = cc.CSLoader:createNode(cmd.RES_PATH .. "GameResultLayer.csb")
    parent:addChild(csbNode, 99999999, 1)
    self.widget = csbNode
    self.callback = callback
    cc.funAddChildListener(csbNode, "Button_quit", function() self:onClickQuit() end)
    cc.funAddChildListener(csbNode, "Button_show", function() self:onClickShare() end)
    cc.funAddChildListener(csbNode, "Button_show_0", function() self:onClickAgain() end)
end

function uiGameOver:onClickQuit()
    print("uiGameOver:onClickQuit")
    if type(self.callback) == "function" then
        self.callback("quit")
    end
    self:destroy()
end

function uiGameOver:onClickShare()
    print("uiGameOver:onClickShare")
    if type(self.callback) == "function" then
        self.callback("share")
    end
    -- self:destroy()
end

function uiGameOver:onClickAgain()
    print("uiGameOver:onClickAgain")
    if type(self.callback) == "function" then
        self.callback("continue")
    end
    self:destroy()
end

--只需要负责ui刷新,不要管数据逻辑
function uiGameOver:refresh(dataList)
    dump(dataList, "#uiGameOver:refresh==")
    dataList = dataList or test_dataList
    --创建马牌
    local node_ma = cc.find(self.widget, "Node_ma")

    local maValues = dataList.maPai
    for idx,cardVaule in ipairs(maValues) do
        local card = CardLayer:createOutOrActiveCardSprite(1, cardVaule, false)
        card:setPosition((idx-1)*42+70, 50)
        card:setScale(0.95)
        node_ma:addChild(card)
    end
    local iswin = true
    --手牌
    for i,playerData in ipairs(dataList) do
        local playerNode = cc.find(self.widget, "Node_player_" .. (i-1))
        self:refreshOne(playerNode, playerData, playerData.useritem or test_useritem)

        if playerData.isSelf then
            if playerData.totalScore < 0 then
                iswin = false
            end
        end
    end

    if not iswin then
        local tittle = cc.find(self.widget, "Sprite_30")
        tittle:setTexture(cmd.RES_PATH.."single_round/kwx_img_lose.png")
    end

    for i=#dataList+1,3 do
        local playerNode = cc.find(self.widget, "Node_player_" .. (i-1))
        playerNode:setVisible(false)
    end
end

function uiGameOver:refreshOne(playerNode, playerData, useritem)
    for idx,cardVaule in ipairs(playerData.cardData) do
        local card = CardLayer:createOutOrActiveCardSprite(1, cardVaule, false)
        local x = (idx-1)*44+125
        if idx >= playerData.outCardIdx then
            x = x + 25
        end
        card:setPosition(x, 40)
        playerNode:addChild(card)
    end
    local text_name = cc.find(playerNode, "Text_name")
    local text_score = cc.find(playerNode, "AtlasLabel_score")
    local text_piao = cc.find(playerNode, "AtlasLabel_piao")
    local text_gang = cc.find(playerNode, "AtlasLabel_gang")
    local text_fan = cc.find(playerNode, "AtlasLabel_fan")
    local spStatus = cc.find(playerNode, "Sprite_status")
    local spFace = cc.find(playerNode, "Sprite_face")
    local text_hu = cc.find(playerNode, "Text_hu")

    spStatus:setLocalZOrder(2)

    local totalScore = playerData.totalScore
    local piao = playerData.piao
    local gang = playerData.gang
    local fan = playerData.fan
    local totalScoreStr = ""
    if totalScore and totalScore ~= 0 then
        totalScore = "/" .. math.abs(totalScore)
    end

    if playerData.isZiMo then
        spStatus:setTexture("single_round/kwx_img_zimo.png")
    elseif playerData.isHuPai then
        spStatus:setTexture("single_round/kwx_img_jiepao.png")
    elseif playerData.isFangPao then
        spStatus:setTexture("single_round/kwx_img_fangpao.png")
    else
        spStatus:setVisible(false)
    end

    if playerData.isHuPai then
        text_score:setProperty(totalScore, "single_round/kwx_lab_score_l.png", 23, 31, "/")
        text_piao:setProperty(piao, "single_round/kwx_lab_score_l.png", 23, 31, "/")
        text_gang:setProperty(gang, "single_round/kwx_lab_score_l.png", 23, 31, "/")
        text_fan:setProperty(fan, "single_round/kwx_lab_score_l.png", 23, 31, "/")

        text_hu:setString(playerData.cardTypeStr or "")
    else
        text_score:setString(totalScore)
        text_piao:setString(piao)
        text_gang:setString(gang)
        text_fan:setString(fan)
        text_hu:setVisible(false)
    end

    text_name:setString(useritem.szNickName)

    -- local head = HeadSprite:createClipHead(useritem, 92, "plaza/plaza_sp_headbg.png")
    local head = HeadSprite:createClipHead(useritem, 92, "faces/kwx_face_mask.png")
    head:setPosition(46, 46)
    spFace:addChild(head)
end

function uiGameOver:onExit()
    if type(self.callback) == "function" then
        self.callback("onExit")
    end
end

return uiGameOver