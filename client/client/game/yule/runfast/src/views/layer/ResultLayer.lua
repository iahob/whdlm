local ResultLayer = class("ResultLayer", function(scene)
	local resultlayer = cc.LayerColor:create(cc.c4b(0, 0, 0, 100)):setVisible(false)
	return resultlayer
end)

local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.runfast.src.models.GameLogic")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local cmd = appdf.req(appdf.GAME_SRC.."yule.runfast.src.models.CMD_Game")
local SpineObj = appdf.req("client.src.external.SpineObj")

ResultLayer.BT_EXIT = 1
ResultLayer.BT_CONTINUS = 2
ResultLayer.BT_CHANGE = 3
ResultLayer.BT_CLOSE = 4

function ResultLayer:onInitData()

end

function ResultLayer:ctor(scene)
	self._scene = scene
	self:onInitData()

	self._nodeUI = ExternalFun.loadCSB(cmd.RES_PATH.."result/ResultLayer.csb", self)

	local btnExit = self._nodeUI:getChildByName("Button_exitResult")
	btnExit:addClickEventListener(function(ref)
		self:setVisible(false)
		if PriRoom and GlobalUserItem.bPrivateRoom then
			local priEndLayer = self._scene._priView.priEndLayer
			if priEndLayer then
				priEndLayer:setVisible(true)
			else
				self._scene._scene:onQueryExitGame()
			end
			return
		end
		self._scene._scene:onQueryExitGame()
		
	end)

	local btnContinue = self._nodeUI:getChildByName("Button_continue")
	btnContinue:addClickEventListener(function(ref)
		self:setVisible(false)
		if PriRoom and GlobalUserItem.bPrivateRoom then
			local priEndLayer = self._scene._priView.priEndLayer
			if priEndLayer then
				priEndLayer:setVisible(true)
			else
				self._scene:onUserReady()
			end
			return
		end
		self._scene:onUserReady()
		
	end)

	local btnChange = self._nodeUI:getChildByName("Button_change")
	btnChange:addClickEventListener(function(ref)
		self:setVisible(false)
		if PriRoom and GlobalUserItem.bPrivateRoom then
			local priEndLayer = self._scene._priView.priEndLayer
			if priEndLayer then
				priEndLayer:setVisible(true)
			else
				self._scene:changeTable()
			end
			return
		end
		self._scene:changeTable()
	end)

	local btnClose = self._nodeUI:getChildByName("Button_close")
	btnClose:addClickEventListener(function(ref)
		self:setVisible(false)
		if PriRoom and GlobalUserItem.bPrivateRoom then
			local priEndLayer = self._scene._priView.priEndLayer
			if priEndLayer then
				priEndLayer:setVisible(true)
			end
		end
	end)

	if GlobalUserItem.bPrivateRoom  then
		btnExit:setPositionX(500)
		btnChange:setVisible(false)
		btnContinue:setPositionX(834)
	end
	self.resultSpine = nil
end





function ResultLayer:setResult(result, bMeWin,meChairId)
	
	self._nodeUI:getChildByName("FileNode_3"):setVisible(self._scene._scene.wPlayerCount == 3)
	if self.resultSpine then
		self.resultSpine:removeFromParent()
	end

	if bMeWin then
		self.resultSpine = SpineObj:create("skeleton/sl.json", "skeleton/sl.atlas", false)
	else
		self.resultSpine = SpineObj:create("skeleton/sb.json", "skeleton/sb.atlas", false)
	end


    self.resultSpine:setPosition(cc.p(667,500))
    self._nodeUI:addChild(self.resultSpine)


	local index  = 0
	for i=1,self._scene._scene.wPlayerCount do
		local userNode = self._nodeUI:getChildByName("FileNode_"..i)
		userNode:getChildByName("Sprite_me"):setVisible(meChairId == i-1)
		userNode:getChildByName("Text_nickname"):setString(result[i].userItem.szNickName)
		userNode:getChildByName("Text_leftCard"):setString(result[i].count)
		local nowScore = nil
		if result[i].lScore>=0 then
			nowScore = userNode:getChildByName("AtlasLabel_win")
			userNode:getChildByName("AtlasLabel_lose"):setVisible(false)
		else
			nowScore = userNode:getChildByName("AtlasLabel_lose")
			userNode:getChildByName("AtlasLabel_win"):setVisible(false)
		end
		nowScore:setVisible(true)
		nowScore:setString(result[i].lScore>=0 and "/"..result[i].lScore or "/"..math.abs(result[i].lScore))
		userNode:getChildByName("Sprite_win"):setVisible(result[i].lScore>0)
		userNode:getChildByName("Sprite_all"):setVisible(result[i].count==16)
	end

	self:setVisible(true)
end

return ResultLayer