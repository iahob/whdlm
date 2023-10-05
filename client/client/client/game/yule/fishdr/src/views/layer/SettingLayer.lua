local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun");
local SettingLayer = class("SettingLayer", cc.Layer);

SettingLayer.SLIDER_EFFECT = 1;
SettingLayer.SLIDER_MUSIC = 2;
SettingLayer.BT_CLOSE = 3;
SettingLayer.BT_MASK = 4;
SettingLayer.BT_EFFECT_OFF = 5;
SettingLayer.BT_EFFECT_ON = 6;

--构造
function SettingLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("csb/SetLayer.csb", self);
    csbNode:setPosition(0,0);
    self._csbNode = csbNode;
    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender);
        end
    end

    --遮罩
    local btn = csbNode:getChildByName("Panel_1");
    btn:setTag(SettingLayer.BT_MASK);
    btn:addTouchEventListener(cbtlistener);

    --背景
    local sp_bg = csbNode:getChildByName("bg");
    self.m_spBg = sp_bg;

    --关闭btn
    local btn_exit = csbNode:getChildByName("btn_exit");
    btn_exit:setTag(SettingLayer.BT_CLOSE);
    btn_exit:addTouchEventListener(cbtlistener);

    -- 滑动条事件
    local sliderFunC = function(sender, eventType)
        self:onSliderEvent(sender,eventType);
    end

    -- 音乐滑动
    local slider_music = csbNode:getChildByName("slider_music");
    slider_music:setTag(SettingLayer.SLIDER_MUSIC);
    slider_music:addEventListener(sliderFunC);
    slider_music:setPercent(GlobalUserItem.nMusic);
    self.slider_music = slider_music;
    local bMusic = slider_music:getPercent() == 0;

    -- 音效滑动
    local slider_sound = csbNode:getChildByName("slider_sound");
    slider_sound:setTag(SettingLayer.SLIDER_EFFECT);
    slider_sound:addEventListener(sliderFunC);
    slider_sound:setPercent(GlobalUserItem.nSound);
    self.slider_sound = slider_sound;
    local bEffect = slider_sound:getPercent() == 0;

    -- 特效
    local node_off = csbNode:getChildByName("node_off");
    self.node_off = node_off;
    local btn_off = node_off:getChildByName("btn_off");
    btn_off:setTag(SettingLayer.BT_EFFECT_OFF);
    btn_off:addTouchEventListener(cbtlistener);
    local icon_off = node_off:getChildByName("icon_off");

    local node_on = csbNode:getChildByName("node_on");
    self.node_on = node_on;
    local btn_on = node_on:getChildByName("btn_on");
    btn_on:setTag(SettingLayer.BT_EFFECT_ON);
    btn_on:addTouchEventListener(cbtlistener);
    local icon_on = node_on:getChildByName("icon_on");
end

function SettingLayer:showLayer(var)
    self:setVisible(var);
end

-- 滑动条
function SettingLayer:onSliderEvent(sender, event)
    local bChange = false;
    local tag = sender:getTag();
    local changePer = sender:getPercent();
    if event == ccui.SliderEventType.percentChanged then
        if SettingLayer.SLIDER_MUSIC == tag then
            bChange = changePer ~= GlobalUserItem.nMusic;
            GlobalUserItem.nMusic = changePer;
        elseif SettingLayer.SLIDER_EFFECT == tag then
            bChange = changePer ~= GlobalUserItem.nSound;
            GlobalUserItem.nSound = changePer;
        end
    end

    if bChange then
        if GlobalUserItem.nMusic == 0 then
            GlobalUserItem.setVoiceAble(false);
        else
            GlobalUserItem.bVoiceAble = true;
            --背景音乐
            GlobalUserItem.setVoiceAble(true);
        end

        if GlobalUserItem.nSound == 0 then
            GlobalUserItem.setSoundAble(false);
        else
            GlobalUserItem.bSoundAble = true;
        end
        self:updateVolum()
    end
end

-- 更新音量
function SettingLayer:updateVolum()
    local tmp = GlobalUserItem.nMusic;
    if tmp >100 then
        tmp = 100;
    elseif tmp < 0 then
        tmp = 0;
    end
    AudioEngine.setMusicVolume(tmp/100.0);

    tmp = GlobalUserItem.nSound;
    if tmp >100 then
        tmp = 100;
    elseif tmp < 0 then
        tmp = 0;
    end
    AudioEngine.setEffectsVolume(tmp/100.0);
end


--按钮回调方法
function SettingLayer:OnButtonClickedEvent( tag, sender )
    if SettingLayer.BT_EFFECT_OFF == tag then
        self.node_off:setVisible(false);
        self.node_on:setVisible(true);
    elseif SettingLayer.BT_EFFECT_ON == tag then
        self.node_off:setVisible(true);
        self.node_on:setVisible(false);
    elseif SettingLayer.BT_CLOSE == tag then
        self:removeFromParent();
    elseif SettingLayer.BT_MASK == tag then
        self:removeFromParent();
    end
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

function SettingLayer:onTouchEnded(touch, event)
    
end

return SettingLayer