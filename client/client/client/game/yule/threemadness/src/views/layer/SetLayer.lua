--
-- Author: tom
-- Date: 2017-02-27 17:26:42
--
local SetLayer = class("SetLayer", function(scene)
	local setLayer = display.newLayer()
	return setLayer
end)

local cmd = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.models.CMD_Game")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local TAG_BT_MUSIC = 1
local TAG_BT_EFFECT = 2
local TAG_BT_EXIT = 3

function SetLayer:onInitData()
end

function SetLayer:onResetData()
end

local this
function SetLayer:ctor(scene)
	this = self
	self._scene = scene

	local rootLayer, csbNode = ExternalFun.loadRootCSB("setting/Setting.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode

	

	

	local funCallback = function(sender,eventType)
		this:onButtonCallback(sender:getTag(), sender)
	end

	local bg = csbNode:getChildByName("Bg")
	--UI
	self.cbMusic =  bg:getChildByName("CheckBox_music")
	self.cbMusic:addEventListener(funCallback)
	self.cbMusic:setTag(TAG_BT_MUSIC)

	self.cbEffect =  bg:getChildByName("CheckBox_effect")
	self.cbEffect:addEventListener(funCallback)
	self.cbEffect:setTag(TAG_BT_EFFECT)

	self.btClose =  bg:getChildByName("Button_close")
	self.btClose:addTouchEventListener(funCallback)
	self.btClose:setTag(TAG_BT_EXIT)

	--声音
	self.cbMusic:setSelected(GlobalUserItem.bVoiceAble)
	self.cbEffect:setSelected(GlobalUserItem.bSoundAble)
	if GlobalUserItem.bVoiceAble then
		AudioEngine.playMusic("sound_res/BACK_MUSIC.wav", true)
	end
	--版本号
	local mgr = self._scene._scene._scene:getApp():getVersionMgr()
	local nVersion = mgr:getResVersion(cmd.KIND_ID) or "0"
	local strVersion = "游戏版本："..appdf.BASE_C_VERSION.."."..nVersion
	cc.Label:create()
		:move(1000, 170)
		:setString(strVersion)
		:setAnchorPoint(cc.p(1, 0.5))
		:setTextColor(cc.c3b(0, 0, 0))
		:setSystemFontSize(20)
		:addTo(self)

	self:setVisible(false)
end

function SetLayer:onButtonCallback(tag, ref)
	if tag == TAG_BT_MUSIC then
		print("音乐状态本开")
		GlobalUserItem.setVoiceAble(ref:isSelected())
		if ref:isSelected() then
			AudioEngine.playMusic("sound_res/BACK_MUSIC.wav", true)
		end
	elseif tag == TAG_BT_EFFECT then
		print("音效状态本开")
		GlobalUserItem.setSoundAble(ref:isSelected())
	elseif tag == TAG_BT_EXIT then
		print("离开")
		self:hideLayer()
	end
end



function SetLayer:showLayer()
	self:setVisible(true)
end

function SetLayer:hideLayer()
	self:setVisible(false)
end

return SetLayer