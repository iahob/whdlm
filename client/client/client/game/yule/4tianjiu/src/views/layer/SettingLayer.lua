local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local SettingLayer = class("SettingLayer", cc.Layer)

SettingLayer.BT_EFFECT = 1
SettingLayer.BT_MUSIC = 2
SettingLayer.BT_CLOSE = 3
SettingLayer.BT_MASK = 4

--构造
function SettingLayer:ctor( verstr )
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self.SetLayer = ExternalFun.loadCSB("setting/SetLayer.csb", self)
    self.SetLayer:setPosition(0,0)
    self._csbNode = self.SetLayer;
    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender)
        end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(SettingLayer.BT_MASK);
    btn:addTouchEventListener(cbtlistener);

    local btn_exit = self._csbNode:getChildByName("btn_exit");
    btn_exit:setTag(SettingLayer.BT_CLOSE);
    btn_exit:addTouchEventListener(cbtlistener);

    --背景
    local sp_bg = self._csbNode:getChildByName("bg")
    self.m_spBg = sp_bg

    --音效
    self.m_btnEffect = self._csbNode:getChildByName("btn_sound")
    self.m_btnEffect:setTag(SettingLayer.BT_EFFECT)
    self.m_btnEffect:addTouchEventListener(cbtlistener)

    --音乐
    self.m_btnMusic = self._csbNode:getChildByName("btn_music")
    self.m_btnMusic:setTag(SettingLayer.BT_MUSIC)
    self.m_btnMusic:addTouchEventListener(cbtlistener)
    --按钮纹理
    if GlobalUserItem.bVoiceAble == true then 
        self.m_btnMusic:loadTextureNormal("setting/btn_on.png")
    else
        self.m_btnMusic:loadTextureNormal("setting/btn_off.png")
    end
    if GlobalUserItem.bSoundAble == true then 
        self.m_btnEffect:loadTextureNormal("setting/btn_on.png")
    else
        self.m_btnEffect:loadTextureNormal("setting/btn_off.png")
    end

    -- --游戏版本
    -- self.m_TextVer = self._csbNode:getChildByName("Text_version")
    -- self.m_TextVer:setString(verstr)
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
            ExternalFun.playBackgroudAudio("bg_game.mp3")
            sender:loadTextureNormal("setting/btn_on.png")
        else
            AudioEngine.stopMusic()
            sender:loadTextureNormal("setting/btn_off.png")
        end
    elseif SettingLayer.BT_EFFECT == tag then   --音效
        local effect = not GlobalUserItem.bSoundAble
        GlobalUserItem.setSoundAble(effect)
        if GlobalUserItem.bSoundAble == true then 
            sender:loadTextureNormal("setting/btn_on.png")
        else
            sender:loadTextureNormal("setting/btn_off.png")
        end
    elseif SettingLayer.BT_MASK == tag or SettingLayer.BT_CLOSE == tag then
        self:removeFromParent();
    end
end

--触摸回调
function SettingLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        local fadeout = cc.FadeOut:create(0.5)
        local call = cc.CallFunc:create(function( )
            self:removeFromParent()
        end)
        self._csbNode:runAction(cc.Sequence:create(fadeout, call))
    end
    -- return not self:isVisible()
end

function SettingLayer:onTouchEnded(touch, event)
    
end

return SettingLayer