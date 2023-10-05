local CardLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.layer.CardLayer")

local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.CMD_Game")
local uiHuTipStatic = env.uiBase:ctor{
    zorder = 130,
}

function uiHuTipStatic:ctor(...)
    self:init(...)
    self:registerNodeEvent()
    return self
end

-- local pos = {cc.p(310,260), cc.p(1050-154,455+37), cc.p(270,455+37), cc.p(828, 681)}
local pos = {cc.p(310,260), cc.p(1016+130,625), cc.p(240,625), cc.p(828, 681)}
function uiHuTipStatic:init(parent, cardValues, viewid)
    local cardCount = #cardValues
    self.widget = parent:getChildByTag(viewid)
    if cardCount>3 then
        if not self.widget then
            self.widget = cc.CSLoader:createNode("LayerMoreThan3.csb")
            parent:addChild(self.widget, self.zorder, viewid)
        end
        self:refreshMoreThanThree(cardCount)
    else
        if not self.widget then
            self.widget = cc.CSLoader:createNode("LayerHuTipStatic.csb")
            parent:addChild(self.widget, self.zorder, viewid)
        end
        self:refresh(cardValues)
    end

    local img = self.widget:getChildByName("Image_1")
    if viewid == 2 then
        img:setAnchorPoint(cc.p(1,0))
    end

    img:setPosition(pos[viewid])
end

function uiHuTipStatic:refreshMoreThanThree(cardCount)
    cc.find(self.widget, "AtlasLabel_1"):setString(cardCount)
end

function uiHuTipStatic:setTipPosition(x,y)
    local img = self.widget:getChildByName("Image_1")
    img:setPosition(x,y)
end

function uiHuTipStatic:refresh(dataList)
    local totalCardType = #dataList
    local background = cc.find(self.widget, "Image_1")
    local gap = 3
    background:setContentSize(cc.size(130+(totalCardType-1)*47, 86))

    for idx,cardVaule in ipairs(dataList) do
        local card = CardLayer:createOutOrActiveCardSprite(1, cardVaule, false)
        local x = 88 + (idx-1) * (42+gap)
        local y = 42
        print(x,y)
        card:setPosition(x,y)
        background:addChild(card)
    end
end

function uiHuTipStatic:onExit()
end

return uiHuTipStatic