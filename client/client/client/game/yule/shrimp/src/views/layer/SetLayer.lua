--
-- Author: tom
-- Date: 2017-02-27 17:26:42
--
local SetLayer = class("SetLayer", function(scene)
	local setLayer = display.newLayer()
	return setLayer
end)

local cmd = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.models.CMD_Game")

local TAG_BT_MUSICON = 1
local TAG_BT_MUSICOFF = 2
local TAG_BT_EFFECTON = 3
local TAG_BT_EFFECTOFF = 4
local TAG_BT_EXIT = 5

function SetLayer:onInitData()
end

function SetLayer:onResetData()
end

local this
function SetLayer:ctor(scene)
	this = self
	self._scene = scene
	self:onInitData()

	self.colorLayer = cc.LayerColor:create(cc.c4b(0, 0, 0, 125))
		:setContentSize(display.width, display.height)
		:addTo(self)
	self.colorLayer:setTouchEnabled(true)
	self.colorLayer:registerScriptTouchHandler(function(eventType, x, y)
		return this:onTouch(eventType, x, y)
	end)

	local funCallback = function(ref)
		this:onButtonCallback(ref:getTag(), ref)
	end

	--UI
	self.sp_layerBg = display.newSprite("set/di.png")
		:move(display.center)
		:addTo(self)

	self.sp_layerBg = display.newSprite("set/icon.png")
		:move(667,680)
		:addTo(self)

	self.btMusicOn = ccui.Button:create("button/btn_close_0.png","button/btn_close_1.png")
		:setTag(TAG_BT_EXIT)
		:move(1072, 660)
		:addTo(self)
	self.btMusicOn:addClickEventListener(funCallback)

	self.btMusicOn = ccui.Button:create("set/bt_on_0.png","set/bt_off_0.png","set/bt_on_0.png")
		:setTag(TAG_BT_MUSICON)
		:move(728, 433)
		:addTo(self)
	self.btMusicOn:addClickEventListener(funCallback)

	self.btMusicOff = ccui.Button:create("set/bt_off_0.png","set/bt_on_0.png","set/bt_off_0.png")
		:setTag(TAG_BT_MUSICOFF)
		:move(728, 433)
		:addTo(self)
	self.btMusicOff:addClickEventListener(funCallback)

	self.btEffectOn = ccui.Button:create("set/bt_on_0.png","set/bt_off_0.png","set/bt_on_0.png")
		:setTag(TAG_BT_EFFECTON)
		:move(728, 295)
		:addTo(self)
	self.btEffectOn:addClickEventListener(funCallback)

	self.btEffectOff = ccui.Button:create("set/bt_off_0.png","set/bt_on_0.png","set/bt_off_0.png")
		:setTag(TAG_BT_EFFECTOFF)
		:move(728, 295)
		:addTo(self)
	self.btEffectOff:addClickEventListener(funCallback)

	--声音
	self.btMusicOn:setVisible(GlobalUserItem.bVoiceAble)
	self.btMusicOff:setVisible(not GlobalUserItem.bVoiceAble)
	self.btEffectOn:setVisible(GlobalUserItem.bSoundAble)
	self.btEffectOff:setVisible(not GlobalUserItem.bSoundAble)
	if GlobalUserItem.bVoiceAble then
		AudioEngine.playMusic("sound_res/bg.mp3", true)
	end
	--版本号
	local mgr = self._scene._scene:getApp():getVersionMgr()
	local nVersion = mgr:getResVersion(cmd.KIND_ID) or "0"
	local strVersion = "游戏版本："..appdf.BASE_C_VERSION.."."..nVersion
	cc.Label:create()
		:move(1050, 100)
		:setString(strVersion)
		:setAnchorPoint(cc.p(1, 0.5))
		:setTextColor(cc.c3b(114, 76, 45))
		:setSystemFontSize(30)
		:addTo(self)

	-- self:setVisible(false)
end

function SetLayer:onButtonCallback(tag, ref)
	if tag == TAG_BT_MUSICON then
		print("音乐状态本开")
		GlobalUserItem.setVoiceAble(false)
		self.btMusicOn:setVisible(false)
		self.btMusicOff:setVisible(true)
	elseif tag == TAG_BT_MUSICOFF then
		print("音乐状态本关")
		GlobalUserItem.setVoiceAble(true)
		self.btMusicOn:setVisible(true)
		self.btMusicOff:setVisible(false)
		AudioEngine.playMusic("sound_res/BACK_MUSIC.wav", true)
	elseif tag == TAG_BT_EFFECTON then
		print("音效状态本开")
		GlobalUserItem.setSoundAble(false)
		self.btEffectOn:setVisible(false)
		self.btEffectOff:setVisible(true)
	elseif tag == TAG_BT_EFFECTOFF then
		print("音效状态本关")
		GlobalUserItem.setSoundAble(true)
		self.btEffectOn:setVisible(true)
		self.btEffectOff:setVisible(false)
	elseif tag == TAG_BT_EXIT then
		print("离开")
		self:hideLayer()
	end
end

function SetLayer:onTouch(eventType, x, y)
	if eventType == "began" then
		return true
	end

	local pos = cc.p(x, y)
    local rectLayerBg = self.sp_layerBg:getBoundingBox()
    if not cc.rectContainsPoint(rectLayerBg, pos) then
    	self:hideLayer()
    end

    return true
end

function SetLayer:showLayer()
	self.colorLayer:setTouchEnabled(true)
	self:setVisible(true)
end

function SetLayer:hideLayer()
	self.colorLayer:setTouchEnabled(false)
	self:setVisible(false)
	self:onResetData()
	self:removeFromParent()
end

return SetLayer