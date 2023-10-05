local CardSprite = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.CardSpriteVer2")
local SchedulerPool = appdf.req(appdf.EXTERNAL_SRC .. "SchedulerPool")

local TAG_CARD_BEGIN = 10000

local ActionMgr = {}
ActionMgr.TAG_CARD_BEGIN = TAG_CARD_BEGIN

local topPosList = {cc.p(650, 750),cc.p(580, 610), cc.p(580, 610), cc.p(680, 610), cc.p(800, 610)}
-- local endPosList = {cc.p(600, 682),cc.p(243, 227), cc.p(467, 227), cc.p(714, 227), cc.p(948, 227)}
--copy from gameviewlayer's cardpoint
local endPosList = {cc.p(670, 678), cc.p(264, 227), cc.p(264+233, 227), cc.p(264+233*2, 227), cc.p(264+233*3,227)}

function ActionMgr:create()
    self.schedulerPool = SchedulerPool:create()
    return self
end

--发牌, 节点,目标位置
function ActionMgr:createSendCardAction(cardSprites, dstPositions, parent)
    local index = 1
    local function play()
        local toppos = topPosList[index]
        local endpos = endPosList[index]
        self:createFiveCardsSendAction(cardSprites[index], toppos, endpos, parent, index)
        index = index + 1

        if index > 5 then
            -- self:callbackSendFinish(parent)
            return false
        end
        return index <= 5
    end

    play()
    self.schedulerPool:loopCall(play, 0.5)
end

function ActionMgr:playOpenAction(parent, cardArry)
    -- self.schedulerPool:delayCall(function()
    --     self:openFiveCardWithOneHide(parent, 1, {0x38,0x38,0x38,0x38,0x38})
    --     self:openFiveCardWithOneHide(parent, 2, {0x38,0x38,0x38,0x38,0x38})
    --     self:openFiveCardWithOneHide(parent, 3, {0x38,0x38,0x38,0x38,0x38})
    --     self:openFiveCardWithOneHide(parent, 4, {0x38,0x38,0x38,0x38,0x38})
    --     self:openFiveCardWithOneHide(parent, 5, {0x38,0x38,0x38,0x38,0x38})
    -- end, 4)

    -- self.schedulerPool:delayCall(function()
    --     self:openFiveCard(parent, 1, {0x38,0x38,0x38,0x38,0x38})
    --     self:openFiveCard(parent, 2, {0x38,0x38,0x38,0x38,0x38})
    --     self:openFiveCard(parent, 3, {0x38,0x38,0x38,0x38,0x38})
    --     self:openFiveCard(parent, 4, {0x38,0x38,0x38,0x38,0x38})
    --     self:openFiveCard(parent, 5, {0x38,0x38,0x38,0x38,0x38})
    -- end, 6)

    -- self.schedulerPool:delayCall(function()
        self:openFiveCardWithOneHide(parent, 1, cardArry[1])
        self:openFiveCardWithOneHide(parent, 2, cardArry[2])
        self:openFiveCardWithOneHide(parent, 3, cardArry[3])
        self:openFiveCardWithOneHide(parent, 4, cardArry[4])
        self:openFiveCardWithOneHide(parent, 5, cardArry[5])
    -- end, 2)

    -- self.schedulerPool:delayCall(function()
    --     self:openFiveCard(parent, 1, cardArry[1])
    --     self:openFiveCard(parent, 2, cardArry[2])
    --     self:openFiveCard(parent, 3, cardArry[3])
    --     self:openFiveCard(parent, 4, cardArry[4])
    --     self:openFiveCard(parent, 5, cardArry[5])
    -- end, 3)
end
function ActionMgr:stopActions(cardSprites)
    for i=1,5 do
        -- local card = CardSprite:createCard(0)
        -- card:setVisible(true)
        -- card:setAnchorPoint(0, 0.5)
        -- card:setPosition(675-35+30, 464)
        -- card:setCardValue(0x3d)
        -- parent:addChild(card, i, TAG_CARD_BEGIN+i - whichOne*5)
        local card = cardSprites[i]
        card:setVisible(true)
        card:showCardBack(true)
        card:setAnchorPoint(0, 0.5)
        card:setPosition(675-35+30, 464)
        table.insert(cards, card)

        local idx = 5-i
        local seq = cc.Sequence:create(cc.DelayTime:create(idx*0.065),
            cc.EaseIn:create(bezier:clone(), 1),
            cc.CallFunc:create(function(sender)
                --5张到位
                print("******************发牌动画")
                if sender:getTag() == (TAG_CARD_BEGIN+1 - whichOne*5) then
                    for idx,card in pairs(cards) do
                        local unfold = cc.MoveTo:create(0.06, cc.p(endpos.x+(idx-1)*32, endpos.y))
                        card:runAction(cc.Sequence:create(unfold, fold, cc.DelayTime:create(3) ))
                    end
                end
            end)
        )
        card:runAction(seq)
    end
end

--发一个位置的5张牌
function ActionMgr:createFiveCardsSendAction(cardSprites, toppos, endpos, parent, whichOne)
    local startpos = cc.p(display.width+15, 464)
    local controlPoint_1 = cc.p(toppos.x-30, toppos.y+200)
    local controlPoint_2 = cc.p(endpos.x, endpos.y)
    local bezierConfig = {controlPoint_1, controlPoint_2, endpos}
    local bezier = cc.BezierTo:create(0.8, bezierConfig)

    local cards = {}
    for i=1,5 do
        -- local card = CardSprite:createCard(0)
        -- card:setVisible(true)
        -- card:setAnchorPoint(0, 0.5)
        -- card:setPosition(675-35+30, 464)
        -- card:setCardValue(0x3d)
        -- parent:addChild(card, i, TAG_CARD_BEGIN+i - whichOne*5)
        local card = cardSprites[i]
        card:setVisible(true)
        card:showCardBack(true)
        card:setAnchorPoint(0, 0.5)
        card:setPosition(675-35+30, 464)
        table.insert(cards, card)

        local idx = 5-i
        local seq = cc.Sequence:create(cc.DelayTime:create(idx*0.065),
            cc.EaseIn:create(bezier:clone(), 1),
            cc.CallFunc:create(function(sender)
                --5张到位
                print("******************发牌动画")
                if sender:getTag() == (TAG_CARD_BEGIN+1 - whichOne*5) then
                    for idx,card in pairs(cards) do
                        local unfold = cc.MoveTo:create(0.06, cc.p(endpos.x+(idx-1)*28, endpos.y))
                        card:runAction(cc.Sequence:create(unfold, fold, cc.DelayTime:create(3) ))
                    end
                end
            end)
        )
        card:runAction(seq)
    end
end

--开牌
function ActionMgr:openFiveCardWithOneHide(parent, whichOne, cardvalues)
    for i=1,5 do
        local toppos = topPosList[whichOne]
        local endpos = endPosList[whichOne]
        local card = parent:getChildByTag(TAG_CARD_BEGIN+i- whichOne*5)
        local fold = cc.MoveTo:create(0.3, cc.p(endpos.x+4*32-i*2, endpos.y) )
        local unfold = cc.MoveTo:create(0.2, cc.p(endpos.x+(i-1)*32, endpos.y))
        local delay = cc.DelayTime:create(0.06)
        local callback = function()
            card:setCardValue(cardvalues[i])
            card:showCardBack(i==4)
        end
        local callfunc = cc.CallFunc:create(callback)
        local seq = cc.Sequence:create(fold, delay, callfunc, delay, unfold)
        card:runAction(seq)
    end
end

--开牌
function ActionMgr:openFiveCard(parent, whichOne, cardvalues)
    for i=1,3 do
        local toppos = topPosList[whichOne]
        local endpos = endPosList[whichOne]
        local card = parent:getChildByTag(TAG_CARD_BEGIN+i- whichOne*5)
        local fold = cc.MoveTo:create(0.1, cc.p(endpos.x+(i-1)*32+32, endpos.y) )
        local unfold = cc.MoveTo:create(0.1, cc.p(endpos.x+(i-1)*32, endpos.y))
        local delay = cc.DelayTime:create(0.06)
        local callback = function()
            card:setCardValue(cardvalues[i])
            card:showCardBack(false)
        end
        local callfunc = cc.CallFunc:create(callback)
        -- local seq = cc.Sequence:create(fold, delay, callfunc, delay, unfold, cc.DelayTime:create(1), cc.RemoveSelf:create(true) )
        local seq = cc.Sequence:create(fold, delay, callfunc, delay, unfold)
        card:runAction(seq)
    end

    --第四张牌
    local card_4 = parent:getChildByTag(TAG_CARD_BEGIN+4- whichOne*5)

    --第五张牌
    local toppos = topPosList[whichOne]
    local endpos = endPosList[whichOne]
    local card = parent:getChildByTag(TAG_CARD_BEGIN+5- whichOne*5)
    local fold = cc.MoveTo:create(0.06, cc.p(endpos.x+3*32, endpos.y) )
    local unfold = cc.MoveTo:create(0.06, cc.p(endpos.x+4*32, endpos.y))
    local delay = cc.DelayTime:create(0.06)
    local callback = function()
        --打开第五张
        card:setCardValue(cardvalues[5])
        card:showCardBack(false)
        --打开第四张
        card_4:setCardValue(cardvalues[4])
        card_4:showCardBack(false)
    end
    local callfunc = cc.CallFunc:create(callback)
    -- local seq = cc.Sequence:create(fold, delay, callfunc, delay, unfold, cc.DelayTime:create(1), cc.RemoveSelf:create(true) )
    local seq = cc.Sequence:create(fold, delay, callfunc, delay, unfold)
    card:runAction(seq)

--[[
    for i=1,5 do
        local toppos = topPosList[whichOne]
        local endpos = endPosList[whichOne]
        local card = parent:getChildByTag(TAG_CARD_BEGIN+i- whichOne*5)
        local fold = cc.MoveTo:create(0.06, cc.p(endpos.x+4*32-i*2, endpos.y) )
        local unfold = cc.MoveTo:create(0.06, cc.p(endpos.x+(i-1)*32, endpos.y))
        local delay = cc.DelayTime:create(0.06)
        local callback = function()
            card:setCardValue(cardvalues[i])
            card:showCardBack(false)
        end
        local callfunc = cc.CallFunc:create(callback)
        -- local seq = cc.Sequence:create(fold, delay, callfunc, delay, unfold, cc.DelayTime:create(1), cc.RemoveSelf:create(true) )
        local seq = cc.Sequence:create(fold, delay, callfunc, delay, unfold)
        card:runAction(seq)
    end
    ]]
end

function ActionMgr:bettingAction(node, offsetx, oldx, oldy)
    node:stopAllActions()
    node:setPosition(oldx, oldy)
    local mv = cc.MoveBy:create(0.2, cc.p(offsetx, 0))
    local reverse = mv:reverse()
    local seq = cc.Sequence:create(mv, reverse)
    node:runAction(cc.EaseOut:create(seq, 1))
end

function ActionMgr:floatScoreAction(node, offsety)
    node:stopAllActions()
    local mv = cc.MoveBy:create(2, cc.p(0, offsety))
    local seq = cc.Sequence:create(mv, cc.RemoveSelf:create(true))
    node:runAction(cc.EaseOut:create(seq, 1))
end

function ActionMgr:destroy()
    self.schedulerPool:clearAll()
end

return ActionMgr