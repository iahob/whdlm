--
-- Author: zhong
-- Date: 2016-11-21 18:39:13
--
--设置界面
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")

local SettingLayer = class("SettingLayer", cc.Layer)

SettingLayer.BT_EFFECT = 1
SettingLayer.BT_MUSIC = 2
SettingLayer.BT_CLOSE = 3
SettingLayer.BT_MASK = 4
--构造
function SettingLayer:ctor( parent )
    local this = self
    self.m_parent = parent
    --注册触摸事件
    ExternalFun.registerTouchEvent(this, true)

    --加载csb资源
    local csbNode = ExternalFun.loadCSB("setting/SettingLayer.csb", self)
    local cbtlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender,eventType)
    end

    local sp_bg = csbNode:getChildByName("setting_bg")
    self.m_spBg = sp_bg

    local btn = csbNode:getChildByName("mask")
    btn:setTag(SettingLayer.BT_MASK)
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            this:removeFromParent()
        end
    end)

    --关闭按钮
    local btn = sp_bg:getChildByName("close_btn")
    btn:setTag(SettingLayer.BT_CLOSE)
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            this:removeFromParent()
        end
    end)

    --switch
    --音效
    self.m_btnEffect = sp_bg:getChildByName("check_effect")
    self.m_btnEffect:setTag(SettingLayer.BT_EFFECT)
    self.m_btnEffect:addEventListener(cbtlistener)
    self.m_btnEffect:setSelected(GlobalUserItem.bSoundAble)

    --音乐
    self.m_btnMusic = sp_bg:getChildByName("check_bg")
    self.m_btnMusic:setTag(SettingLayer.BT_MUSIC)
    self.m_btnMusic:addEventListener(cbtlistener)
    self.m_btnMusic:setSelected(GlobalUserItem.bVoiceAble)
    
    -- 版本号
    -- local mgr = self.m_parent:getParentNode():getParentNode():getApp():getVersionMgr()
    -- local verstr = mgr:getResVersion(200) or "0"
    -- verstr = "游戏版本:" .. appdf.BASE_C_VERSION .. "." .. verstr
    -- local txt_ver = sp_bg:getChildByName("txt_ver")
    -- txt_ver:setString(verstr)
end

--
function SettingLayer:showLayer( var )
    self:setVisible(var)
end

function SettingLayer:onSelectedEvent( tag, sender )
    if SettingLayer.BT_MUSIC == tag then
        local music = not GlobalUserItem.bVoiceAble
        GlobalUserItem.setVoiceAble(music)
        if GlobalUserItem.bVoiceAble == true then
            ExternalFun.playBackgroudAudio("background.mp3")
        end
    elseif SettingLayer.BT_EFFECT == tag then
        local effect = not GlobalUserItem.bSoundAble
        GlobalUserItem.setSoundAble(effect)
    end
end

function SettingLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function SettingLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:removeFromParent()
    end
end

return SettingLayer