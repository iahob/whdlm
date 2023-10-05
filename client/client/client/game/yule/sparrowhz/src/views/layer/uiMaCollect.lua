local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowhz.src.models.CMD_Game")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local uiBase = appdf.req(appdf.EXTERNAL_SRC .. "uiBase")

local uiMaCollect = uiBase:ctor{
    zorder = 120,
}

function uiMaCollect:ctor(...)
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiMaCollect:init(parent, callback)
    local csbNode = cc.CSLoader:createNode("MaLayer.csb")
    parent:addChild(csbNode, self.zorder, 1)
    csbNode:setVisible(false)
    self.widget = csbNode
    cc.funAddChildListener(csbNode, "Panel_1", function() self:onClickQuit() end)

    self._callback = callback

    local delay = cc.DelayTime:create(2)
    local hide = cc.Hide:create()
    local show = cc.Show:create()
    local delayHide = cc.DelayTime:create(1)
    local callfunc = cc.CallFunc:create(function() self:onAutoDestroy() end)
    self.widget:runAction(cc.Sequence:create(hide, delayHide, show, delay, callfunc))

    self.cardNode = cc.find(self.widget, "img_card_1")
    self.cardNode:retain()
    self.cardNode:removeFromParent(false)
end

function uiMaCollect:onClickQuit()
    if not self.widget then
        return
    end
    self.widget:stopAllActions()

    if type(self._callback) == "function" then
        self._callback()
    end
    self:destroy()
end

function uiMaCollect:onAutoDestroy()
    if type(self._callback) == "function" then
        self._callback()
    end
    self:destroy()
end

--只需要负责ui刷新,不要管数据逻辑
function uiMaCollect:refresh(cardList)
    local totalCardType = #cardList
    totalCardType = math.max(totalCardType-2, 0)
    local background = cc.find(self.widget, "Panel_1/Image_1")
    local gap = 3
    local cardWidth = 88
    local totalWidth = 226 + (totalCardType) * (cardWidth+gap)
    totalWidth = math.max(226, totalWidth)
    background:setContentSize(cc.size(totalWidth, 168))

    local startx = display.width*0.5 - totalWidth * 0.5 + cardWidth * 0.5 + 20
    local parent = self.widget

    for k,cardValue in ipairs(cardList) do
        local value = cardValue % 16
        local isShot = false
        if value == 1 or value == 5 or value == 9 then
            isShot = true
        end
        local node = parent:getChildByTag(k)
        if not node then
            node = self.cardNode:clone()
            parent:addChild(node, 10, k)
        end
        local x = startx + (k-1) * (cardWidth+gap)
        node:setPosition(x, 378)

        self:refreshOneCard(node, cardValue, isShot)
    end
end

function uiMaCollect:refreshOneCard(cardNode, cardValue, isShot)
    local img = cardNode:getChildByName("img")
    local nValue = cardValue % 16
    local nColor = math.floor(cardValue/16)
    local strFile = cmd.RES_PATH.."game/font_big/font_"..nColor.."_"..nValue..".png"
    img:loadTexture(strFile)
    local img_choose = cardNode:getChildByName("Image_18")
    img_choose:setVisible(isShot)
end

function uiMaCollect:onExit()
    self.cardNode:release()
end

return uiMaCollect