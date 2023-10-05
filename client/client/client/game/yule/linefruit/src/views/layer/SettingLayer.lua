local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local SettingLayer = class("SettingLayer", cc.Layer)

SettingLayer.BT_EFFECT = 1
SettingLayer.BT_MUSIC = 2
SettingLayer.BT_CLOSE = 3

--构造
function SettingLayer:ctor( verstr )
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("SetLayer/SetLayer.csb", self)
    self._csbNode:setPosition(0,0)
    self._csbNode = self._csbNode:getChildByName("Node")
    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender)
        end
    end
    --背景
    local sp_bg = self._csbNode:getChildByName("sprite_bg")
    self.m_spBg = sp_bg

    --关闭按钮
--    local btn = self._csbNode:getChildByName("Button_close")
--    btn:setTag(SettingLayer.BT_CLOSE)
--    btn:addTouchEventListener(function (ref, eventType)
--        if eventType == ccui.TouchEventType.ended then
--            ExternalFun.playClickEffect()
--            self:removeFromParent()
--        end
--    end)

    --音效
    self.m_btnEffect = self._csbNode:getChildByName("Button_sound")
    self.m_btnEffect:setTag(SettingLayer.BT_EFFECT)
    self.m_btnEffect:addTouchEventListener(cbtlistener)

    --音乐
    self.m_btnMusic = self._csbNode:getChildByName("Button_music")
    self.m_btnMusic:setTag(SettingLayer.BT_MUSIC)
    self.m_btnMusic:addTouchEventListener(cbtlistener)
    --按钮纹理
    if GlobalUserItem.bVoiceAble == true then 
        self.m_btnMusic:loadTextureNormal("SetLayer/btn_set_open.png")
    else
        self.m_btnMusic:loadTextureNormal("SetLayer/btn_set_close.png")
    end
    if GlobalUserItem.bSoundAble == true then 
        self.m_btnEffect:loadTextureNormal("SetLayer/btn_set_open.png")
    else
        self.m_btnEffect:loadTextureNormal("SetLayer/btn_set_close.png")
    end

    --游戏版本
    self.m_TextVer = self._csbNode:getChildByName("Text_version")
    self.m_TextVer:setString(verstr)

    self:setScale(720/750)
end

function SettingLayer:showLayer( var )
    self:setVisible(var)
end

--按钮回调方法
function SettingLayer:OnButtonClickedEvent( tag, sender )
    if SettingLayer.BT_MUSIC == tag then    --音乐
        local music = not GlobalUserItem.bVoiceAble
        GlobalUserItem.setVoiceAble(music)
        if GlobalUserItem.bVoiceAble == true then 
            ExternalFun.playBackgroudAudio("bg.mp3")
            sender:loadTextureNormal("SetLayer/btn_set_open.png")
        else
            AudioEngine.stopMusic()
            sender:loadTextureNormal("SetLayer/btn_set_close.png")
        end
    elseif SettingLayer.BT_EFFECT == tag then   --音效
        local effect = not GlobalUserItem.bSoundAble
        GlobalUserItem.setSoundAble(effect)
        if GlobalUserItem.bSoundAble == true then 
            sender:loadTextureNormal("SetLayer/btn_set_open.png")
        else
            sender:loadTextureNormal("SetLayer/btn_set_close.png")
        end
    end
end

--触摸回调
function SettingLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:showLayer(false)
    end
    return not self:isVisible()
end

function SettingLayer:onTouchEnded(touch, event)
    self:removeFromParent()
end

return SettingLayer