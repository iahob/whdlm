
--设置界面
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.paijiu.src.models.GameLogic")
local cmd = appdf.req(appdf.GAME_SRC.."yule.paijiu.src.models.CMD_Game")

local SettingLayer = class("SettingLayer", cc.Layer)

SettingLayer.BT_EFFECT = 1
SettingLayer.BT_MUSIC = 2
SettingLayer.BT_CLOSE = 3
--构造
function SettingLayer:ctor( parent )
    self:addChild(cc.LayerColor:create(cc.c4b(30, 30, 30, 200), 1334, 750),-1)
    self.m_parent = parent
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)

    --加载csb资源
    local csbNode = ExternalFun.loadCSB("setting/SettingLayer.csb", self)

    local cbtlistener = function (sender,eventType)
        self:onSelectedEvent(sender:getTag(),eventType)
    end

    local sp_bg = csbNode:getChildByName("setting_bg")
    self.m_spBg = sp_bg

    --关闭按钮
    local btn = csbNode:getChildByName("close_btn")
    btn:setTag(SettingLayer.BT_CLOSE)
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            self:removeFromParent()
        end
    end)

    --switch
    --音效
    self.m_btnEffect = csbNode:getChildByName("btn_effect")
    self.m_btnEffect:setTag(SettingLayer.BT_EFFECT)
    self.m_btnEffect:addTouchEventListener(cbtlistener)
    if GlobalUserItem.bSoundAble == true then   
        --设置纹理
        self.m_btnEffect:loadTextureNormal("setting/off.png")
        self.m_btnEffect:loadTexturePressed("setting/off.png")
    else
         --设置纹理
        self.m_btnEffect:loadTextureNormal("setting/on.png")
        self.m_btnEffect:loadTexturePressed("setting/on.png")
    end

    --音乐
    self.m_btnMusic = csbNode:getChildByName("btn_bg")
    self.m_btnMusic:setTag(SettingLayer.BT_MUSIC)
    self.m_btnMusic:addTouchEventListener(cbtlistener)
    if GlobalUserItem.bVoiceAble == true then
        --设置纹理
        self.m_btnMusic:loadTextureNormal("setting/off.png")
        self.m_btnMusic:loadTexturePressed("setting/off.png")
    else
         --设置纹理
        self.m_btnMusic:loadTextureNormal("setting/on.png")
        self.m_btnMusic:loadTexturePressed("setting/on.png")
    end

    -- 版本号
    local mgr = self.m_parent:getParent():getParentNode():getApp():getVersionMgr()
    local verstr = mgr:getResVersion(cmd.KIND_ID) or "0"
    verstr = "游戏版本:" .. appdf.BASE_C_VERSION .. "." .. verstr
    local txt_ver = csbNode:getChildByName("Text_version")
    txt_ver:setString(verstr)
    --txt_ver:setString("客服端测试版1.0")
end

--
function SettingLayer:showLayer( var )
    self:setVisible(var)
end

function SettingLayer:onSelectedEvent( tag, eventType )
    if SettingLayer.BT_MUSIC == tag then
        if eventType == ccui.TouchEventType.ended then
            local music = not GlobalUserItem.bVoiceAble
            print("按钮状态，音乐", music)
            GlobalUserItem.setVoiceAble(music)
         
            if GlobalUserItem.bVoiceAble == true then
                ExternalFun.playBackgroudAudio("background.mp3")
                --设置纹理
                self.m_btnMusic:loadTextureNormal("setting/off.png")
                self.m_btnMusic:loadTexturePressed("setting/off.png")
            else
                 --设置纹理
                self.m_btnMusic:loadTextureNormal("setting/on.png")
                self.m_btnMusic:loadTexturePressed("setting/on.png")
            end
        end
    elseif SettingLayer.BT_EFFECT == tag then
        if eventType == ccui.TouchEventType.ended then
            local effect = not GlobalUserItem.bSoundAble
            GlobalUserItem.setSoundAble(effect)
            if GlobalUserItem.bSoundAble == true then
                --设置纹理
                self.m_btnEffect:loadTextureNormal("setting/off.png")
                self.m_btnEffect:loadTexturePressed("setting/off.png")
            else
                 --设置纹理
                self.m_btnEffect:loadTextureNormal("setting/on.png")
                self.m_btnEffect:loadTexturePressed("setting/on.png")
            end
        end
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