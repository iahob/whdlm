--
-- 设置界面(二级弹窗)
-- local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
-- local SettingLayer = class("SettingLayer", TransitionLayer)
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun");
local SettingLayer = class("SettingLayer", cc.Layer);

local module_pre = "game.yule.fivestarshh.src";
local cmd = appdf.req(module_pre..".models.CMD_Game")

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
function SettingLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("setting/SettingLayer.csb", self)
    self.m_rootLayer = rootLayer

    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 滑动条事件
    local sliderFunC = function( sender, eventType )
        self:onSliderEvent( sender,eventType )
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    self.m_spBg = spbg

    -- 关闭
    local btn = csbNode:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )

    -- 音乐滑动
    local slider_music = csbNode:getChildByName("slider_music")
    slider_music:setTag(TAG_ENUM.SLIDER_BG)
    slider_music:addEventListener(sliderFunC)

    self.slider_music = slider_music
    if GlobalUserItem.nMusic < 5 then
        self.slider_music:setPercent(5)
    elseif GlobalUserItem.nMusic > 95 then
        self.slider_music:setPercent(95)
    else
         self.slider_music:setPercent(GlobalUserItem.nMusic)
    end
    local bMusic = slider_music:getPercent() == 5

    -- 音效滑动
    local slider_effect = csbNode:getChildByName("slider_effect")
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
    local btn_slience_on = csbNode:getChildByName("btn_slience_on")
    btn_slience_on:setTag(TAG_ENUM.BTN_SLIENCE_OPEN)
    btn_slience_on:addTouchEventListener( touchFunC )
    self.btn_slience_on = btn_slience_on;

    local btn_slience_off = csbNode:getChildByName("btn_slience_off")
    btn_slience_off:setTag(TAG_ENUM.BTN_SLIENCE_CLOSE)
    btn_slience_off:addTouchEventListener( touchFunC )

    self.btn_slience_off = btn_slience_off;

    if bMusic and bEffect then
        self.btn_slience_on:setVisible(true)
        self.btn_slience_off:setVisible(false)
    else
        self.btn_slience_on:setVisible(false)
        self.btn_slience_off:setVisible(true)
    end

    -- 注册事件监听、二级界面
    -- self:registerEventListenr()
    -- 加载动画
    -- self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function SettingLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:removeFromParent();
    elseif TAG_ENUM.BTN_SLIENCE_OPEN == tag then
        -- 关闭静音
        self.btn_slience_on:setVisible(false)
        self.btn_slience_off:setVisible(true)

        GlobalUserItem.setVoiceAble(true)
        GlobalUserItem.setSoundAble(true)

        if GlobalUserItem.nMusic < 5 then
            self.slider_music:setPercent(5)
        elseif GlobalUserItem.nMusic > 95 then
            self.slider_music:setPercent(95)
        else
             self.slider_music:setPercent(GlobalUserItem.nMusic)
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
        self.btn_slience_on:setVisible(true)
        self.btn_slience_off:setVisible(false)

        GlobalUserItem.setVoiceAble(false)
        GlobalUserItem.setSoundAble(false)

        if GlobalUserItem.nMusic < 5 then
            self.slider_music:setPercent(5)
        elseif GlobalUserItem.nMusic > 95 then
            self.slider_music:setPercent(95)
        else
             self.slider_music:setPercent(GlobalUserItem.nMusic)
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
        self.btn_slience_on:setVisible(bSlience)
        self.btn_slience_off:setVisible(not bSlience)

        if GlobalUserItem.nMusic == 0 then
            GlobalUserItem.setVoiceAble(false)
        else
            GlobalUserItem.bVoiceAble = true
            --背景音乐
            -- ExternalFun.playPlazzBackgroudAudio( )
            ExternalFun.playBackgroudAudio(cmd.SOUND_GameSceneBGM);
        end

        if GlobalUserItem.nSound == 0 then
            GlobalUserItem.setSoundAble(false)
        else
            GlobalUserItem.bSoundAble = true
        end
        self:updateVolum()
    end
    if GlobalUserItem.nSound == 0 and GlobalUserItem.nMusic == 0 then
        self.btn_slience_on:setVisible(true)
        self.btn_slience_off:setVisible(false)
    end

end

function SettingLayer:onExit()
    cc.UserDefault:getInstance():setBoolForKey("vocieable",GlobalUserItem.bVoiceAble)
    cc.UserDefault:getInstance():setBoolForKey("soundable",GlobalUserItem.bSoundAble)
    GlobalUserItem.setMusicVolume(GlobalUserItem.nMusic)
    GlobalUserItem.setEffectsVolume(GlobalUserItem.nSound)
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

--触摸回调
function SettingLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation();
    local m_spBg = self.m_spBg;
    pos = m_spBg:convertToNodeSpace(pos);
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height);
    if false == cc.rectContainsPoint(rec, pos) then
        local fadeout = cc.FadeOut:create(0.5);
        local call = cc.CallFunc:create(function( )
            self:removeFromParent();
        end)
        self._csbNode:runAction(cc.Sequence:create(fadeout, call));
    end
end

return SettingLayer