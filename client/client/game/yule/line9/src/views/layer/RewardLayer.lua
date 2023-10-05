local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local RewardLayer = class("RewardLayer", cc.Layer)

RewardLayer.BT_CLOSE = 1

--构造
function RewardLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("RewardLayer/RewardLayer.csb", self)
    self._csbNode:setPosition(0,0)  

    self._csbNode = self._csbNode:getChildByName("Node")

    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender)
        end
    end
    --背景
    local sp_bg = self._csbNode:getChildByName("Sprite_bg_2")
    self.m_spBg = sp_bg

    --关闭按钮
    local btn = self._csbNode:getChildByName("Button_close")
    btn:setTag(RewardLayer.BT_CLOSE)
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            self:showLayer(false)
--            self:removeFromParent()
        end
    end)

    --背景光
    local spriteBg = self._csbNode:getChildByName("Sprite_bg")
    local animBg = self._csbNode:getChildByName("Anim_bg")
    local animNode = cc.CSLoader:createNode("./animation/rewardback.csb")
    animBg:addChild(animNode)
    local animTimeline = cc.CSLoader:createTimeline("./animation/rewardback.csb")
    local animinfo = animTimeline:getAnimationInfo("play")
    animTimeline:gotoFrameAndPlay(animinfo.startIndex,animinfo.endIndex,animinfo.startIndex,true)
    spriteBg:setVisible(false)
    animNode:runAction(animTimeline)

    --灯光
    local spriteLight = self._csbNode:getChildByName("Sprite_light")
    animNode = cc.CSLoader:createNode("./animation/rewardlight.csb")
    self._csbNode:addChild(animNode)
    animNode:setPosition(spriteLight:getPosition())
    animTimeline = cc.CSLoader:createTimeline("./animation/rewardlight.csb")
    animinfo = animTimeline:getAnimationInfo("play")
    animTimeline:gotoFrameAndPlay(animinfo.startIndex,animinfo.endIndex,animinfo.startIndex,true)
    spriteLight:setVisible(false)
    animNode:runAction(animTimeline)
end

function RewardLayer:updateInfo(nodeCountry, nodeCountryNum, m_winScore, nGiveAwayCount)
    local nodeCountryNum = clone(nodeCountryNum)
    local nodeCountry = {}
    for i = 1, 14 do
        if nodeCountryNum[i-1][1] > 0 then
            table.insert(nodeCountry,i-1)
        end
    end
--    dump(nodeCountryNum,"nodeCountryNum")
--    dump(nodeCountry,"nodeCountry")

    local node = self._csbNode:getChildByName(string.format("Node_%d", #nodeCountry))
    for i = 1, #nodeCountry do
        node:getChildByName(string.format("Sprite_country_%d", i)):setSpriteFrame(string.format("game/im_country_%d.png", nodeCountry[i]))
        node:getChildByName(string.format("Sprite_country_%d", i)):getChildByName("Text_1"):setString("x " .. nodeCountryNum[nodeCountry[i]][1])
    end

    if nGiveAwayCount == 0 and m_winScore > 0 then
        local artGold = self._csbNode:getChildByName("Art_gold")
        local score = m_winScore
        if score >= 10000 then
            if score >= 1000000 then
                score = math.floor(score / 10000)
            else
                local a,b = math.modf(score / 10000)
                if b < 0.1 then
                    score = a
                else
                    score = string.format("%.1f",score / 10000)
                end
            end
            artGold:getChildByName("Sprite_wan"):setPosition(string.len(score) * 45 + 10, -1)
            artGold:getChildByName("Sprite_wan"):setVisible(true)
        else
            artGold:getChildByName("Sprite_wan"):setVisible(false)
        end
        artGold:setString(score)
        artGold:setVisible(true)
        ExternalFun.playSoundEffect("GoldMove.wav")
    elseif nGiveAwayCount > 0 and m_winScore == 0 then
        self._csbNode:getChildByName("Sprite_bar"):getChildByName("Text_1"):setString("免费 " .. nGiveAwayCount .. " 次拉靶")
        self._csbNode:getChildByName("Sprite_bar"):setVisible(true)
        ExternalFun.playSoundEffect("jiangli.wav")
    else
        local nodeTiao = self._csbNode:getChildByName("Sprite_tiao")
        local artGold = nodeTiao:getChildByName("Art_gold")
        local score = m_winScore
        if score >= 10000 then
            if score >= 1000000 then
                score = math.floor(score / 10000)
            else
                local a,b = math.modf(score / 10000)
                if b < 0.1 then
                    score = a
                else
                    score = string.format("%.1f",score / 10000)
                end
            end
            artGold:getChildByName("Sprite_wan"):setPosition(string.len(score) * 45 + 10, -1)
            artGold:getChildByName("Sprite_wan"):setVisible(true)
        else
            artGold:getChildByName("Sprite_wan"):setVisible(false)
        end
        artGold:setString(score)

        nodeTiao:getChildByName("Sprite_bar"):getChildByName("Text_1"):setString("免费 " .. nGiveAwayCount .. " 次拉靶")
        nodeTiao:getChildByName("Sprite_bar"):setVisible(true)

        nodeTiao:setVisible(true)
        ExternalFun.playSoundEffect("GoldBigMove.wav")
    end

    node:setVisible(true)
end

function RewardLayer:showLayer( var )
    self:setVisible(var)
end

--触摸回调
function RewardLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
--        self:removeFromParent()
        self:showLayer(false)
    end
    return self:isVisible()
end

return RewardLayer