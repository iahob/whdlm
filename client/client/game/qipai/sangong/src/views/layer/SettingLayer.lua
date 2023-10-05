local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local SettingLayer = class("SettingLayer", cc.Layer)

SettingLayer.BT_EFFECT = 1
SettingLayer.BT_MUSIC = 2
SettingLayer.BT_CLOSE = 3
SettingLayer.TAG_MASK = 4

function SettingLayer:ctor(verstr)
    --加载csb资源
     --注册触摸事件
    local csbNode = ExternalFun.loadCSB("set/LayerSetting.csb", self)

    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:onBtnClick(sender:getTag(), sender)
        end
    end

    local mask = csbNode:getChildByName("shadaw")  
    mask:setTouchEnabled(true)
    mask:setTag(SettingLayer.TAG_MASK)
    mask:addTouchEventListener(btnEvent)

    --关闭按钮
    local btn = csbNode:getChildByName("btnClose")
    btn:setTag(SettingLayer.BT_CLOSE)
    btn:addTouchEventListener(btnEvent)

    self.m_btnEffect = csbNode:getChildByName("effect_btn")
    self.m_btnEffect:setTag(SettingLayer.BT_EFFECT)
    self.m_btnEffect:addTouchEventListener(btnEvent)
    --音乐
    self.m_btnMusic = csbNode:getChildByName("music_btn")
    self.m_btnMusic:setTag(SettingLayer.BT_MUSIC)
    self.m_btnMusic:addTouchEventListener(btnEvent)
    self:refreshBtnState()

    self:refreshBtnState()
end

function SettingLayer:onBtnClick( tag, sender )
    if SettingLayer.BT_CLOSE == tag or SettingLayer.TAG_MASK == tag then
        ExternalFun.playClickEffect()
        self:hideLayer()
    elseif SettingLayer.BT_MUSIC == tag then
        local music = not GlobalUserItem.bVoiceAble
        GlobalUserItem.setVoiceAble(music)
        self:refreshBtnState()
        if GlobalUserItem.bVoiceAble == true then
            ExternalFun.playBackgroudAudio("BACK_GROUND.mp3")
        else
            AudioEngine.stopMusic()
        end
    elseif SettingLayer.BT_EFFECT == tag then
        local effect = not GlobalUserItem.bSoundAble
        GlobalUserItem.setSoundAble(effect)
        self:refreshBtnState()
    end
end

function SettingLayer:refreshBtnState()
    --按钮纹理
    if GlobalUserItem.bVoiceAble == true then 
        self.m_btnMusic:loadTextureNormal("set/btn_off.png")
        self.m_btnMusic:loadTexturePressed("set/btn_on.png")
    else
        self.m_btnMusic:loadTextureNormal("set/btn_on.png")
        self.m_btnMusic:loadTexturePressed("set/btn_off.png")
    end
    if GlobalUserItem.bSoundAble == true then 
        self.m_btnEffect:loadTextureNormal("set/btn_off.png")
        self.m_btnEffect:loadTexturePressed("set/btn_on.png")
    else
        self.m_btnEffect:loadTextureNormal("set/btn_on.png")
        self.m_btnEffect:loadTexturePressed("set/btn_off.png")
    end
end

function SettingLayer:showLayer()
    self:setVisible(true)
end

function SettingLayer:hideLayer()
    self:setVisible(false)
end

return SettingLayer