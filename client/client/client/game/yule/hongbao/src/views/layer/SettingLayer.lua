--
-- Author: luo
-- Date: 2016年12月30日 15:18:32
--
--设置界面
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local SettingLayer = class("SettingLayer", cc.Layer)

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "SLIDER_BG",            -- 背景滑动
    "SLIDER_EFFECT",        -- 音效滑动
    "BTN_SLIENCE_OPEN",     -- 开启静音
    "BTN_SLIENCE_CLOSE",    -- 关闭静音
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
--构造
function SettingLayer:ctor( verstr )
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("setting/Layer.csb", self)
    self._csbNode:setPosition(0,0)
    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            self:OnButtonClickedEvent(sender:getTag(),sender)
        end
    end


    --关闭按钮
    local btn = self._csbNode:getChildByName("Button_2")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            self:removeFromParent()
        end
    end)

    -- 滑动条事件
    local sliderFunC = function( sender, eventType )
        self:onSliderEvent( sender,eventType )
    end
    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    -- 音乐滑动
    local slider_bg = self._csbNode:getChildByName("Slider_1")
    slider_bg:setTag(TAG_ENUM.SLIDER_BG)
    slider_bg:addEventListener(sliderFunC)

    self.m_sliderBg = slider_bg
    if GlobalUserItem.nMusic < 5 then
        self.m_sliderBg:setPercent(5)
    elseif GlobalUserItem.nMusic > 95 then
        self.m_sliderBg:setPercent(95)
    else
         self.m_sliderBg:setPercent(GlobalUserItem.nMusic)
    end
    local bMusic = slider_bg:getPercent() == 5

    -- 音效滑动
    local slider_effect = self._csbNode:getChildByName("Slider_1_0")
    slider_effect:setTag(TAG_ENUM.SLIDER_EFFECT)
    slider_effect:addEventListener(sliderFunC)

    self.m_sliderEffect = slider_effect 
    if GlobalUserItem.nSound < 5 then
        self.m_sliderEffect:setPercent(5)
    elseif GlobalUserItem.nSound > 95 then
        self.m_sliderEffect:setPercent(95)
    else
         self.m_sliderEffect:setPercent(GlobalUserItem.nSound)
    end
    local bEffect = slider_effect:getPercent() == 5

    -- 静音
    btn = self._csbNode:getChildByName("Button_mute")
    btn:addTouchEventListener( touchFunC )
    self.m_btnSlience = btn
    self:updateSlience(bMusic and bEffect)

end

-- 滑动条
function SettingLayer:onSliderEvent( sender, event )
    local bSlience = true
    local bChange = false
    local tag = sender:getTag()
    local changePer = sender:getPercent()
    if changePer <= 5 then
        changePer = 0
        sender:setPercent(5) 
    elseif changePer > 95 then
        changePer = 100
        sender:setPercent(95) 
    end

    if event == ccui.SliderEventType.percentChanged then
        if TAG_ENUM.SLIDER_BG == tag then
            bChange = changePer ~= GlobalUserItem.nMusic
            GlobalUserItem.nMusic = changePer
        elseif TAG_ENUM.SLIDER_EFFECT == tag then
            bChange = changePer ~= GlobalUserItem.nSound
            GlobalUserItem.nSound = changePer
        end
    end

    if 0 ~= GlobalUserItem.nMusic or 0 ~= GlobalUserItem.nSound then
        bSlience = false
    end
    if bChange then
        self:updateSlience(bSlience)
        if GlobalUserItem.nMusic == 0 then
            GlobalUserItem.setVoiceAble(false)
            AudioEngine.stopMusic()
        else
            GlobalUserItem.bVoiceAble = true
            --背景音乐
            AudioEngine.stopMusic()
            AudioEngine.playMusic("sound/BGM.mp3",true)
        end

        if GlobalUserItem.nSound == 0 then
            GlobalUserItem.setSoundAble(false)
        else
            GlobalUserItem.bSoundAble = true
        end
        self:updateVolum()
    end
    if GlobalUserItem.nSound == 0 and GlobalUserItem.nMusic == 0 then
            self:updateSlience(true)
    end

end

-- 更新音量
function SettingLayer:updateVolum()
    local tmp = GlobalUserItem.nMusic 
    if tmp >100 then
        tmp = 100
    elseif tmp < 0 then
        tmp = 0
    end
    AudioEngine.setMusicVolume(tmp/100.0) 

    tmp = GlobalUserItem.nSound 
    if tmp >100 then
        tmp = 100
    elseif tmp < 0 then
        tmp = 0
    end
    AudioEngine.setEffectsVolume(tmp/100.0) 
end

-- 更新开关
function SettingLayer:updateSlience( bSlience )
    local normal = "setting/music_close.png"
    local disable = "setting/music_close.png"
    local press = "setting/music_close.png"
    -- 静音
    if bSlience then
        -- 已开启静音, 关闭
        normal = "setting/music_open.png"
        disable = "setting/music_open.png"
        press = "setting/music_open.png"
        self.m_btnSlience:setTag(TAG_ENUM.BTN_SLIENCE_OPEN)
    else
        -- 已关闭静音, 开启 
        self.m_btnSlience:setTag(TAG_ENUM.BTN_SLIENCE_CLOSE)
    end
    self.m_btnSlience:loadTextureDisabled(disable)
    self.m_btnSlience:loadTextureNormal(normal)
    self.m_btnSlience:loadTexturePressed(press) 
end
--
function SettingLayer:showLayer( var )
    self:setVisible(var)
end

-- 按键监听
function SettingLayer:onButtonClickedEvent( tag,sender )
    if TAG_ENUM.BTN_SLIENCE_OPEN == tag then
        -- 关闭静音
        self:updateSlience(false)
        GlobalUserItem.setVoiceAble(true)
        GlobalUserItem.setSoundAble(true)

        if GlobalUserItem.nMusic < 5 then
            self.m_sliderBg:setPercent(5)
        elseif GlobalUserItem.nMusic > 95 then
            self.m_sliderBg:setPercent(95)
        else
             self.m_sliderBg:setPercent(GlobalUserItem.nMusic)
        end

        if GlobalUserItem.nSound < 5 then
            self.m_sliderEffect:setPercent(5)
        elseif GlobalUserItem.nSound > 95 then
            self.m_sliderEffect:setPercent(95)
        else
             self.m_sliderEffect:setPercent(GlobalUserItem.nSound)
        end
       
        
        self:updateVolum()

        GlobalUserItem.setVoiceAble(true)
        GlobalUserItem.setSoundAble(true)
    elseif TAG_ENUM.BTN_SLIENCE_CLOSE == tag then
        -- 开启静音
        self:updateSlience(true)
        GlobalUserItem.setVoiceAble(false)
        GlobalUserItem.setSoundAble(false)

        if GlobalUserItem.nMusic < 5 then
            self.m_sliderBg:setPercent(5)
        elseif GlobalUserItem.nMusic > 95 then
            self.m_sliderBg:setPercent(95)
        else
             self.m_sliderBg:setPercent(GlobalUserItem.nMusic)
        end

        if GlobalUserItem.nSound < 5 then
            self.m_sliderEffect:setPercent(5)
        elseif GlobalUserItem.nSound > 95 then
            self.m_sliderEffect:setPercent(95)
        else
             self.m_sliderEffect:setPercent(GlobalUserItem.nSound)
        end
        self:updateVolum()

        GlobalUserItem.setVoiceAble(false)
        GlobalUserItem.setSoundAble(false)
    end
end
--触摸回调
function SettingLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function SettingLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self._csbNode:getChildByName("settingBG")
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:removeFromParent()
    end
end

return SettingLayer