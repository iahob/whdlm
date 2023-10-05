local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local SettingLayer = class("SettingLayer", cc.Layer)

SettingLayer.BT_CLOSE = 1
SettingLayer.BT_BG = 2
SettingLayer.BT_MUSIC = 3
SettingLayer.BT_EFFECT = 4

function SettingLayer:ctor()
	self._volume = 0
	self._effect = 0
	self:loadResource()
end

function SettingLayer:loadResource()
	local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    local sliderFunc = function (sender, eventType)
    	self:onSliderEvent(sender, eventType)
    end

	local csbNode = ExternalFun.loadCSB("setting.csb", self)
	self.csbNode = csbNode

	--退出
	local btn = csbNode:getChildByName("Button_exit")
	btn:setTag(SettingLayer.BT_CLOSE)
	btn:addTouchEventListener(btnEvent)

	--遮罩
	local btn = csbNode:getChildByName("mask")
	btn:setTag(SettingLayer.BT_BG)
	btn:addTouchEventListener(btnEvent)

	--音乐
	local btn = csbNode:getChildByName("Slider_music")
	btn:setTag(SettingLayer.BT_MUSIC)
	btn:addEventListener(sliderFunc)
	btn:setPercent(GlobalUserItem.nMusic)

	--音效
	local btn = csbNode:getChildByName("Slider_effect")
	btn:setTag(SettingLayer.BT_EFFECT)
	btn:addEventListener(sliderFunc)
	btn:setPercent(GlobalUserItem.nMusic)
end

function SettingLayer:onButtonClickedEvent(tag, ref)
	if tag == SettingLayer.BT_CLOSE or tag == SettingLayer.BT_BG then
		self:removeFromParent()
	end
end

function SettingLayer:onSliderEvent(sender, event)
	local bSlience = true
    local bChange = false
    local tag = sender:getTag()
    local changePer = sender:getPercent()
    if event == ccui.SliderEventType.percentChanged then
        if SettingLayer.BT_MUSIC == tag then
            bChange = changePer ~= GlobalUserItem.nMusic
            GlobalUserItem.nMusic = changePer
        elseif SettingLayer.BT_EFFECT == tag then
            bChange = changePer ~= GlobalUserItem.nSound
            GlobalUserItem.nSound = changePer
        end
    end

    if 0 ~= GlobalUserItem.nMusic or 0 ~= GlobalUserItem.nSound then
        bSlience = false
    end
    if bChange then
        if GlobalUserItem.nMusic == 0 then
            GlobalUserItem.setVoiceAble(false)
        else
            GlobalUserItem.bVoiceAble = true
            --背景音乐
            ExternalFun.playBackgroudAudio( "bg.mp3" )
        end

        if GlobalUserItem.nSound == 0 then
            GlobalUserItem.setSoundAble(false)
        else
            GlobalUserItem.bSoundAble = true
        end
        self:updateVolum()
    end
end

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

return SettingLayer