--设置界面
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local SettingLayer = class("SettingLayer", cc.Layer)

SettingLayer.BT_EFFECT = 1
SettingLayer.BT_MUSIC = 2
SettingLayer.BT_CLOSE = 3
--构造
function SettingLayer:ctor(verstr)
	--加载csb资源
	local csbNode = ExternalFun.loadCSB("SettingLayer.csb", self)
	self.csbNode = csbNode

	local function btnEvent( sender, eventType )
		self:onBtnClick(sender:getTag(), sender)
	end

    cc.funAddChildListener(csbNode, "setting_bg/close_btn", function() self:onclickClose() end)
    cc.funAddChildListener(csbNode, "layout_bg", function() self:onclickClose() end)
    cc.funAddChildListener(csbNode, "setting_bg/btn_showeffect", function() self:onclickGameEffect() end)
    cc.funAddTouchEventListener(csbNode, "setting_bg/sp_effect/btn", function(...) self:onTouchSlider(1, ...) end)
    local node = cc.funAddTouchEventListener(csbNode, "setting_bg/sp_effect_0/btn", function(...) self:onTouchSlider(2, ...) end)
    self.sildderWidth = node:getParent():getContentSize().width

    self:setGameEffectEnabled(false)

    --版本
    local m_txtversion = cc.find(csbNode, "setting_bg/txt_version")
    m_txtversion:setString(verstr)
end

function SettingLayer:onclickClose()
	ExternalFun.playClickEffect()
	self:removeFromParent()
end

local beginx
local oldBtnPosX
--0,1,2,3 began,moved,ended,canceld
function SettingLayer:onTouchSlider(setType, sender, etype)
	local percent

	if etype == 0 then
		beginx = sender:getTouchBeganPosition().x
		oldBtnPosX = sender:getPositionX()
	elseif etype == 1 then
		local offset = sender:getTouchMovePosition().x - beginx
		local newx = oldBtnPosX + offset
		newx = math.max(0, newx)
		newx = math.min(self.sildderWidth, newx)
		sender:setPositionX(newx)
		percent = (newx / self.sildderWidth) * 100

		self:setSliderPercent(setType, percent)
	elseif etype == 2 then
		local offset = sender:getTouchEndPosition().x - beginx
		local newx = oldBtnPosX + offset
		newx = math.max(0, newx)
		newx = math.min(self.sildderWidth, newx)
		sender:setPositionX(newx)
		percent = (newx / self.sildderWidth) * 100

		if setType == 1 then
			if percent <= 0 then
				GlobalUserItem.setSoundAble(false)
			else
				GlobalUserItem.setSoundAble(true)
				GlobalUserItem.setEffectsVolume(percent)
				AudioEngine.setEffectsVolume(percent/100.00)
			end
		else
			if percent <= 0 then
				GlobalUserItem.setVoiceAble(false)
			else
				GlobalUserItem.setVoiceAble(true)
				GlobalUserItem.setMusicVolume(percent)
				AudioEngine.setMusicVolume(percent/100.0)
			end
		end
		self:setSliderPercent(setType, percent)
	end
end

function SettingLayer:setSliderPercent(setType, percent)
	if setType == 1 then
		cc.find(self.csbNode, "setting_bg/sp_effect/bar"):setPercent(percent)
	else
		cc.find(self.csbNode, "setting_bg/sp_effect_0/bar"):setPercent(percent)
	end
end

function SettingLayer:onclickGameEffect()
	self:setGameEffectEnabled(not self.isGameEffectEnabled)
end

function SettingLayer:setGameEffectEnabled(isEnabled)
	self.isGameEffectEnabled = isEnabled
	if isEnabled then
		cc.find(self.csbNode, "setting_bg/btn_showeffect"):loadTextures("setting/bt_set_open.png", "setting/bt_set_open.png", "setting/bt_set_open.png")
	else
		cc.find(self.csbNode, "setting_bg/btn_showeffect"):loadTextures("setting/bt_set_close.png", "setting/bt_set_close.png", "setting/bt_set_close.png")
	end
end

return SettingLayer