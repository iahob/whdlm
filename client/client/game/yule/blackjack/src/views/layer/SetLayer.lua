--
-- Author: tom
-- Date: 2017-02-27 17:26:42
--
local SetLayer = class("SetLayer", function(scene)
	local setLayer = display.newLayer()
	return setLayer
end)

local cmd = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.models.CMD_Game")

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
	self.colorLayer:setTouchEnabled(false)
	self.colorLayer:registerScriptTouchHandler(function(eventType, x, y)
		return this:onTouch(eventType, x, y)
	end)

	local funCallback = function(ref)
		this:onButtonCallback(ref:getTag(), ref)
	end

	--UI
	self.sp_layerBg = display.newSprite(cmd.RES.."setting/bg.png")
		:move(display.center)
		:addTo(self)

	self.btMusicOn = ccui.Button:create(cmd.RES.."setting/btn_no.png","",cmd.RES.."setting/btn_no.png",ccui.TextureResType.localType)
		:setTag(TAG_BT_MUSICON)
		:move(728, 433)
		:addTo(self)
	self.btMusicOn:addClickEventListener(funCallback)

	self.btMusicOff = ccui.Button:create(cmd.RES.."setting/btn_off.png","",cmd.RES.."setting/btn_off.png",ccui.TextureResType.localType)
		:setTag(TAG_BT_MUSICOFF)
		:move(728, 433)
		:addTo(self)
	self.btMusicOff:addClickEventListener(funCallback)

	self.btEffectOn = ccui.Button:create(cmd.RES.."setting/btn_no.png","",cmd.RES.."setting/btn_no.png",ccui.TextureResType.localType)
		:setTag(TAG_BT_EFFECTON)
		:move(728, 295)
		:addTo(self)
	self.btEffectOn:addClickEventListener(funCallback)

	self.btEffectOff = ccui.Button:create(cmd.RES.."setting/btn_off.png","",cmd.RES.."setting/btn_off.png",ccui.TextureResType.localType)
		:setTag(TAG_BT_EFFECTOFF)
		:move(728, 295)
		:addTo(self)
	self.btEffectOff:addClickEventListener(funCallback)

	display.newSprite(cmd.RES.."setting/music.png")
		:move(450, 433)
		:addTo(self)

		display.newSprite(cmd.RES.."setting/effect.png")
		:move(450, 295)
		:addTo(self)

		display.newSprite(cmd.RES.."setting/titile.png")
		:move(667, 557)
		:addTo(self)

	self.btClose = ccui.Button:create(cmd.RES.."info/bt_close.png","",cmd.RES.."info/bt_close.png",ccui.TextureResType.localType)
		:setTag(TAG_BT_EXIT)
		:move(967, 375+180)
		:addTo(self)
	self.btClose:addClickEventListener(funCallback)

	--声音
	self.btMusicOn:setVisible(GlobalUserItem.bVoiceAble)
	self.btMusicOff:setVisible(not GlobalUserItem.bVoiceAble)
	self.btEffectOn:setVisible(GlobalUserItem.bSoundAble)
	self.btEffectOff:setVisible(not GlobalUserItem.bSoundAble)
	if GlobalUserItem.bVoiceAble then
		AudioEngine.playMusic(cmd.RES.."sound_res/new/blackJack_beijingyinyue.mp3", true)
	end
	--版本号
	local mgr = self._scene._scene._scene:getApp():getVersionMgr()
	local nVersion = mgr:getResVersion(cmd.KIND_ID) or "0"
	local strVersion = "游戏版本："..appdf.BASE_C_VERSION.."."..nVersion
	cc.Label:create()
		:move(667+350-10, 375-200)
		:setString(strVersion)
		:setAnchorPoint(cc.p(1, 0.5))
		:setTextColor(cc.c3b(0, 0, 0))
		:setSystemFontSize(20)
		:addTo(self)

	self:setVisible(false)
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
end

return SetLayer