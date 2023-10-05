local GameLogic = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.GameLogic")
local cardTypeBig = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.cardTypeBig")
local cardTypeSmall = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.cardTypeSmall")

local cache = cc.SpriteFrameCache:getInstance()
cache:addSpriteFrames("brnn_cards.plist")

local CardSpriteVer2 = class("CardSpriteVer2", function()
    return cc.CSLoader:createNode("Node_card_front.csb")
end)

function CardSpriteVer2:ctor()
    self.m_cardData = 0
    self.m_cardValue = 0
    self.m_cardColor = 0
end

--创建卡牌
function CardSpriteVer2:createCard( cbCardData )
    local sp = CardSpriteVer2.new()
    sp:setScale(76/127,103/181)

    sp.m_cardData = cbCardData;
    sp.m_cardValue = GameLogic.GetCardValue(cbCardData)
    sp.m_cardColor = GameLogic.GetCardColor(cbCardData)
    sp:updateSprite()
    sp:showCardBack(true)

    return sp
end

--设置卡牌数值
function CardSpriteVer2:setCardValue( cbCardData )
    self.m_cardData = cbCardData;
    self.m_cardValue = GameLogic.GetCardValue(cbCardData)
    self.m_cardColor = GameLogic.GetCardColor(cbCardData)

    self:updateSprite()
end

--更新纹理资源
function CardSpriteVer2:updateSprite(  )
    local m_cardData = self.m_cardData
    local m_cardValue = self.m_cardValue --1-13
    local m_cardColor = self.m_cardColor --0-3 方块 梅花 红桃 黑桃

    --原先为什么要设置tag?
    -- self:setTag(m_cardData)

    local cache = cc.SpriteFrameCache:getInstance()

    --大小王
    if m_cardValue == 0x0E or m_cardValue == 0x0F then
        self:getChildByName("Sprite_num"):setSpriteFrame(cache:getSpriteFrame(string.format("%d.png", m_cardValue) ))
        self:getChildByName("Sprite_magic_s"):setVisible(false)
        self:getChildByName("Sprite_magic_large"):setSpriteFrame(cache:getSpriteFrame(string.format("role_4_%s.png", m_cardValue)))
    else
        local cardSmallData = cardTypeSmall[m_cardValue]
        local cardBigData = cardTypeBig[m_cardValue]

        if cardSmallData and cardBigData then
            self:getChildByName("Sprite_magic_s"):setVisible(true)
            if m_cardColor ==  0 or m_cardColor == 2 then
                self:getChildByName("Sprite_num"):setSpriteFrame(cache:getSpriteFrame(cardBigData.cardNumRed))
            else
                self:getChildByName("Sprite_num"):setSpriteFrame(cache:getSpriteFrame(cardBigData.cardNumBlack))
            end
            self:getChildByName("Sprite_magic_s"):setSpriteFrame(cache:getSpriteFrame(cardSmallData["cardColorSmall_"..m_cardColor]))
            self:getChildByName("Sprite_magic_large"):setSpriteFrame(cache:getSpriteFrame(cardBigData["cardColorBig_"..m_cardColor]))
        else
            -- print("数据异常========", m_cardColor, m_cardValue)
        end
    end
end

--显示扑克背面
function CardSpriteVer2:showCardBack( var )
    self:getChildByName("Sprite_back"):setVisible(var)
end

return CardSpriteVer2