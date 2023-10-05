--
-- Author: tom
-- Date: 2017-02-27 17:26:42
--
local SetLayer = class("SetLayer", function(scene)
	local setLayer = display.newLayer()
	return setLayer
end)


local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local TAG_SD_MUSIC = 1
local TAG_SD_EFFECT = 2
local TAG_BT_EXIT = 3

local this
function SetLayer:ctor(scene)
	this = self
	self._scene = scene

	local rootLayer, csbNode = ExternalFun.loadRootCSB("rungame_res/SetLayer.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode


	local funCallback = function(sender,eventType)

		this:onButtonCallback(sender:getTag(), sender , eventType)
	end

	local mask = csbNode:getChildByName("Panel_1")
	mask:addTouchEventListener(funCallback)
	mask:setTag(TAG_BT_EXIT)



	

	--UI
	self.sdMusic =  csbNode:getChildByName("Slider_music")
	self.sdMusic:addEventListener(funCallback)
	self.sdMusic:setTag(TAG_SD_MUSIC)

	self.sdEffect =  csbNode:getChildByName("Slider_effect")
	self.sdEffect:addEventListener(funCallback)
	self.sdEffect:setTag(TAG_SD_EFFECT)

	self.btClose =  csbNode:getChildByName("bt_close")
	self.btClose:addTouchEventListener(funCallback)
	self.btClose:setTag(TAG_BT_EXIT)

	--声音
	self.sdMusic:setPercent(GlobalUserItem.nMusic)
	self.sdEffect:setPercent(GlobalUserItem.nSound)


	

	--版本号
	local mgr = self._scene._scene._scene:getApp():getVersionMgr()
	local nVersion = mgr:getResVersion(240) or "0"
	local strVersion = "游戏版本："..appdf.BASE_C_VERSION.."."..nVersion
	
	csbNode:getChildByName("Text_1"):setString(strVersion)

	

end

function SetLayer:onButtonCallback(tag, ref , eventType)
	if tag == TAG_SD_MUSIC then 
		GlobalUserItem.setMusicVolume(ref:getPercent())
	elseif tag == TAG_SD_EFFECT then 
		GlobalUserItem.setEffectsVolume(ref:getPercent())
	
	elseif tag == TAG_BT_EXIT and eventType==ccui.TouchEventType.ended then
		print("离开")
		self:removeFromParent()
	end
end

return SetLayer