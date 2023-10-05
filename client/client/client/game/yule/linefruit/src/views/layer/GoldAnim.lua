local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local GoldAnim = class("GoldAnim", function (  )
    local colorLayer = cc.LayerColor:create(cc.c4b(0,0,0,0))
    return colorLayer
end)

--构造
function GoldAnim:ctor(times)
    -- 注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    -- 加载csb资源
    self._csbNode = ExternalFun.loadCSB("GoldAnim/GoldAnim.csb", self)
    self._csbNode:setPosition(appdf.WIDTH / 2, appdf.HEIGHT / 2)

    -- 背景
    self.m_spBg = self._csbNode:getChildByName("sprite_bg_2")

    print("times",times)
    if times then
        if times < 50 and times > 0 then
            self.m_spBg:setSpriteFrame("GoldAnim/sprite_zfl.png")
            self:play()
            self:runAction(cc.Sequence:create(cc.DelayTime:create(1),
            cc.CallFunc:create(function() self:showLayer(false) end)))
        elseif times < 100 and times >= 50 then
            self.m_spBg:setSpriteFrame("GoldAnim/sprite_zjl.png")
            self:play()
            self:runAction(cc.Sequence:create(cc.DelayTime:create(1),
                cc.CallFunc:create(function() self:play() end),
                cc.DelayTime:create(1),
                cc.CallFunc:create(function() self:showLayer(false) end)))
        elseif times >= 100 then
            self.m_spBg:setSpriteFrame("GoldAnim/sprite_zdjl.png")
            self:play()
            self:runAction(cc.Sequence:create(cc.DelayTime:create(1),
                cc.CallFunc:create(function() self:play() end),
                cc.DelayTime:create(1),
                cc.CallFunc:create(function() self:play() end),
                cc.DelayTime:create(1),
                cc.CallFunc:create(function() self:showLayer(false) end)))
        end
    end
end

function GoldAnim:showLayer( var )
    self:setVisible(var)
end

function GoldAnim:setGold( var )
    self._csbNode:getChildByName("Art_score"):setString(var)
end

function GoldAnim:play( var )
    local animTimeline = cc.CSLoader:createTimeline("GoldAnim/GoldAnim.csb")
    animinfo = animTimeline:getAnimationInfo("play")
    animTimeline:gotoFrameAndPlay(animinfo.startIndex,animinfo.endIndex,animinfo.startIndex,false)
    self._csbNode:runAction(animTimeline)
end

return GoldAnim