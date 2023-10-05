local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.sharkbattle.src"
local PopupWithCloseBtnLayer = appdf.req(module_pre .. ".views.layer.PopupWithCloseBtnLayer")


local SettingLayer = class("SettingLayer",PopupWithCloseBtnLayer)

SettingLayer.CBT_SILENCE 	= 1
SettingLayer.CBT_SOUND   	= 2

function SettingLayer:ctor(popBgFile)
 	SettingLayer.super.ctor(self,popBgFile)

 	local cbtlistener = function (sender,eventType)
    	self:onSelectedEvent(sender:getTag(),sender,eventType)
    end

 	self.effectAudio = ccui.CheckBox:create("soundoff.png","","soundon.png","","")
		:move(730,415)
		:setSelected(GlobalUserItem.bSoundAble)
		:addTo(self)
		:setTag(self.CBT_SOUND)
	self.effectAudio:addEventListener(cbtlistener)

	self.bgAudio = ccui.CheckBox:create("soundoff.png","","soundon.png","","")
		:move(730,305)
		:setSelected(GlobalUserItem.bVoiceAble)
		:addTo(self)
		:setTag(self.CBT_SILENCE)
	self.bgAudio:addEventListener(cbtlistener)

end


function SettingLayer:onSelectedEvent(tag,sender,eventType)
	if tag == SettingLayer.CBT_SILENCE then
		GlobalUserItem.setVoiceAble(eventType == 0)
		--背景音乐
        ExternalFun.playPlazzBackgroudAudio()
	elseif tag == SettingLayer.CBT_SOUND then
		GlobalUserItem.setSoundAble(eventType == 0)
	end
end

return SettingLayer