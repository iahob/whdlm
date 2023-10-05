local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)

if not yl then
require("client.src.plaza.models.yl")
end

local GameChatLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.GameChatLayer")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local cmd = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.models.CMD_Game")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local OperateLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.OperateLayer")
local BankerList = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.BankerList")
local ResultLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.ResultLayer")
local SetLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.SetLayer")
local PlayerInfo = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.PlayerInfo")
local ShrimpConfig = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.ShrimpConfig")
local RecordLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.RecordLayer")
local BetInfoLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.BetInfoLayer")
local BankLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.BankLayer")
local ChipLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.ChipLayer")
local SpineObj = appdf.req("client.src.external.SpineObj")

local start = {cc.p(77,258),cc.p(77,390),cc.p(77,528),
			   cc.p(255,685),cc.p(366,685),cc.p(638,698),cc.p(947,685),cc.p(1070,685),
			   cc.p(1260,528),cc.p(1260,391),cc.p(1260,258),cc.p(43,43)}

local endPos = {{x=350,y=445},{x=682,y=445},{x=1012,y=445},{x=350,y=215},{x=682,y=215},{x=1012,y=215}}

local NAME_RESULT_LAYER = "ResultLayer"
local NAME_CAHT_LAYER   = "GameChatLayer"
local NAME_BET_INIFO_LAYER = "BetInfoLayer"
local NAME_BANKERLIST_LAYER = "BankerList"
local NAME_SET_LAYER = "SetLayer"
local NAME_RULE_LAYER = "RuleLayer"
local NAME_OPERATE_LAYER = "OperateLayer"
local NAME_RECORDLAYER = "RecordLayer"
local NAME_BANK_LAYER  = "BankLayer"
local NAME_WAIIT_GAME_SPINE = "WaitGameSpine"
local NAEM_WAIT_SPINE_NODE = "waitSpineNode"
local NAEM_GAME_END_SPINE = "gameEndSpine"
local NAME_CONTROL_LAYER = "ControlLayer"

local SHOW_NUO_CHUAN   = 4
local SHOW_NUO_DAN 	   = 3
local SHOW_LIAN_CHUAN  = 2
local SHOW_BAOZI       = 1
local SHOW_NIL		   = 0

function GameViewLayer:onExit()
	print("GameViewLayer onExit")
	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
end

function GameViewLayer:getParentNode()
    return self._scene
end
function GameViewLayer:ctor(scene)
	--初始化房间信息
    -- self:initRoomInfo()
    ExternalFun.playBackgroudAudio("bg.mp3")
    self._scene = scene
	local function onNodeEvent(event)  
       if "exit" == event then  
            self:onExit()  
        end  
    end  
    self:registerScriptHandler(onNodeEvent)

    self._bDismissed = false

    self:gameDataInit()
    self:initRoomUI()
    self:initChipPanel()
    self:initMenuPanel()
    self:intiActionPanel()
    self:initAvatrPaenl()
    self:initRoomInfoPaenl()
    self:initPlatformPanel()
    self:initLastTimeDice()
    self:initBellPanel()
    self:initBetPanel()
    self:initChipLayer()
    self:initWanPanel()
    self:initBetNumberLayer()
    self:setBankerOperationpanel()

    self.historyBtn = self.m_csbNode:getChildByName("history")
    self.betinfoBtn = self.m_csbNode:getChildByName("betinfo")
    self.chatBtn = self.m_csbNode:getChildByName("chat")
    self.voiceBtn = self.m_csbNode:getChildByName("voice")
    self.shangzhuangBtn = self.m_csbNode:getChildByName("shangzhuang")
    self.startGame = self.m_csbNode:getChildByName("startGame")

    self.startGame:setVisible(false)
    self.voiceBtn:setVisible(not GlobalUserItem.bVideo)

    
	self.shangzhuangBtn:setVisible(not GlobalUserItem.bPrivateRoom)
	if GlobalUserItem.bPrivateGoldRoom then 
		self.shangzhuangBtn:setVisible(true)
	end

	self.btn_control = self.m_csbNode:getChildByName("control")
	self.btn_control:setVisible(false)

    self:addBtnEventListener()
    self:setDcieAniOver(true)

	self.myText = self.m_csbNode:getChildByName("Text_9_1")
	self.allText = self.m_csbNode:getChildByName("Text_9_2")
	self.notBanker = self.m_csbNode:getChildByName("not_banker")
	self.notBanker:setVisible(false)


end



function GameViewLayer:gameDataInit()


	-- dump(self:getParentNode():getParentNode(),"11",12)
    --搜索路径
    local gameList = self:getParentNode():getParentNode():getApp()._gameList;
    local gameInfo = {};
    for k,v in pairs(gameList) do
        if tonumber(v._KindID) == tonumber(cmd.KIND_ID) then
            gameInfo = v;
            break;
        end
    end

    if nil ~= gameInfo._Module then
    	self._searchPath = device.writablePath.."game/" .. "yule/shrimp" .. "/res/";
      	cc.FileUtils:getInstance():addSearchPath(self._searchPath);
    end

  	AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

end

function GameViewLayer:initRoomUI()

	local rootLayer, csbNode = ExternalFun.loadRootCSB("ui/GameLayer.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode
end

function GameViewLayer:initChipPanel()
	-- body

	self.chipPanel = self.m_csbNode:getChildByName("chipPanel")

	self.betChipBet = {}

	local function betCallBack( sender )
		for k,v in pairs(self.betChipBet) do
			v.isCheck = false
			v.effect:setVisible(false)
			v.effect:stopAllActions()
		end
		sender.effect:setVisible(true)
		sender.isCheck = true
 		local blink = cc.Blink:create(1,2)
 		sender.effect:runAction(cc.RepeatForever:create(blink))
	end

	for i=1,5 do
		local btn = self.chipPanel:getChildByName("Button_"..i)
		local text =  ccui.Text:create()
		text:setFontName("fonts/round_body.ttf")
		text:setFontSize(25)
		text:setString(self._scene:formatScoreTextEx(200))

		-- local text =  cc.LabelAtlas:create("200","chip/chip_num.png",16,21,string.byte("0"))
		btn:addChild(text)
		text:setAnchorPoint(0.5,0.5)
		text:setPosition(54.5, 56.5)
		btn.betText = text
		btn.TextNum = 200

		local effect = btn:getChildByName("sprite")
 		effect:setVisible(false)
 		btn.effect = effect

 		btn.isCheck = false

 		btn:addClickEventListener(betCallBack)
		self.betChipBet[#self.betChipBet+1] = btn
	end
end

function GameViewLayer:initMenuPanel()
	-- body
	local node = self.m_csbNode:getChildByName("menuPanel")
	self.menuPanel = node:getChildByName("menuSp")
	self.menuPanel:setPositionY(827)
	self.menuPanel:setVisible(false)

	node:setPositionX(50)

	self.menuBtn = node:getChildByName("menuBtn")
	self.ruleBtn = self.menuPanel:getChildByName("ruleBtn")
	self.setBtn = self.menuPanel:getChildByName("setBtn")
	self.tableBtn = self.menuPanel:getChildByName("tableBtn")
	self.exitBtn = self.menuPanel:getChildByName("exitBtn")

	self.menuBtn:addClickEventListener(function ()
		-- body
		local b = self.menuPanel:isVisible()
		self.menuBtn:setFlippedY(not b)
		self.menuPanel:setVisible(not b)
		local move = nil
		if self.menuPanel:isVisible() then 
			move = cc.MoveTo:create(0.3,cc.p(0,382))
		else
			move = cc.MoveTo:create(0,cc.p(0,827))
		end
		self.menuPanel:stopAllActions()
		self.menuPanel:runAction(move)
	end)
end

function GameViewLayer:setTableBtnIsOk(b)
	-- body
	b = b or false
	self.tableBtn:setEnabled(b)

	local config = self._scene:getRoomConfig()
	dump(config,"setTableBtnIsOk",12)
	if config.bIsPersonalRoom then
		print("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111")
		self.tableBtn:setEnabled(false)
	end

end

function GameViewLayer:intiActionPanel()
	-- body
	self.showSpecial = SHOW_NIL
	self.actionPanel = self.m_csbNode:getChildByName("actionPanel")
	self.baoZiBtn = self.actionPanel:getChildByName("Button_6")
	self.lianChuanBtn = self.actionPanel:getChildByName("Button_6_0")
	self.nuoDanBtn = self.actionPanel:getChildByName("Button_6_1")
	self.nuoChuanBtn = self.actionPanel:getChildByName("Button_6_2")
end

function GameViewLayer:initAvatrPaenl()
	-- body
	self.avatrPanel = self.m_csbNode:getChildByName("avatrPanel")
	self.player = {}
	self.avatrBtn = {}
	for i=1,12 do
		local btn = self.avatrPanel:getChildByName("avatr_"..i)
		btn.id = i
		self.avatrBtn[#self.avatrBtn+1] = btn
	end

	self.avatrPanel.removeAllHead = function ( )
		for k,v in pairs(self.avatrBtn) do
			if v then 
				v:removeAllChildren()
			end
		end
		-- body
	end

	self.avatrPanel.show = function (id,useritem)
		-- self.avatrBtn[id]:removeAllChildren()
		-- print(debug.traceback())

		-- for k,v in pairs(self.avatrBtn) do
		-- 	v:removeAllChildren()
		-- end

		print("show id = ",id)
		-- dump(useritem,"show useritem" , 12)
		useritem = clone(useritem)
		local head = self.avatrBtn[id]:getChildByName("head")
		if head then 
			if self._scene.isGameEnd then 
				print("self._scene.isGameEnd = ",self._scene.isGameEnd)
				useritem = head.useritem
			end
			head:removeFromParent() 
		end

		local head = PlayerInfo:create(useritem,id)
		self.player[id] = head
		self.avatrBtn[id]:addChild(head)
		head:setName("head")
		head:setPosition(self.avatrBtn[id]:getContentSize().width/2,self.avatrBtn[id]:getContentSize().height/2)


		if self._scene.bankerid == useritem.wChairID and id == 6 then 
			self.avatrPanel.showBanker(true)
		end

		-- if useritem.wChairID == self._scene:GetMeChairID() then
		-- 	self.avatrPanel.show(12,item)
		-- end
	end

	self.avatrPanel.hide = function (useritem)
		-- dump(useritem,"useritem == ",12)
		for k,v in pairs(self._scene.gameUserChair) do
			if v == useritem.wChairID then
				local head = self.avatrBtn[k]:getChildByName("head")
				if head then 
					head:removeFromParent() 
				end
				if k == 6 then 
					self.avatrPanel.showBanker(false)
				end
	        end
		end
	end

	self.avatrPanel.updataName = function (id,name)
		local head = self.avatrBtn[id]:getChildByName("head")
		if head then 
			head.name:setString(name)
		end
	end

	self.avatrPanel.updataMoney = function (id,money)
		local head = self.avatrBtn[id]:getChildByName("head")
		if head then 
		    if GlobalUserItem.bPrivateRoom then 
		        local score = money - PriRoom:getInstance().m_tabPriData.lIniScore
		        head.money:setString(score)
		    else
		        local score = ExternalFun.formatScoreText(money)
		        head.money:setString(score)
		    end
		end
	end

	self.avatrPanel.updataReady = function (id,pBool)
		local head = self.avatrBtn[id]:getChildByName("head")
		if head --[[and id ~= 6 ]]then 
			head.ready:setVisible(false)
		end
	end

	self.avatrPanel.showBanker = function (pShow)
		-- body
		-- print(debug.traceback())
		local sp = self.avatrBtn[6]:getChildByName("banker_icon")
		if sp then sp:removeFromParent() end

		if pShow then 
			sp = cc.Sprite:create("avatar/banker_icon.png")
			self.avatrBtn[6]:addChild(sp)
			sp:setPosition(15.42,77.35)
			sp:setName("banker_icon")
		end
	end	
end

function GameViewLayer:initRoomInfoPaenl()
	-- body
	self.roomInfoPanel = self.m_csbNode:getChildByName("roomInfoPanel")
	self.roomId = self.roomInfoPanel:getChildByName("roomNum")
	self.roomNum = self.roomInfoPanel:getChildByName("number")

	self.roomId:setString("0")
	self.roomNum:setString("第0/0局")

	self.roomInfoPanel.setRoomId = function (id)
		self.roomId:setString(""..id)
	end

	self.roomInfoPanel.setRoomNum = function (num,total)
		self.roomId:setString("第"..num.."/"..total.."局")
	end
	self.roomInfoPanel:setVisible(false)
end

function GameViewLayer:initPlatformPanel()
	-- body
	local platformPanel = self.m_csbNode:getChildByName("platformPanel")
	local timeText = platformPanel:getChildByName("timeText")
	local wifi = platformPanel:getChildByName("wifi")
	local signal = platformPanel:getChildByName("signal")
	local loadingBar = platformPanel:getChildByName("loadingBar")
	-- platformPanel:setVisible(false)


	schedule(timeText,function()
		timeText:setString(os.date("%H:%M",os.time()))
	end,1)
	
	schedule(wifi,function() 
		wifi:setTexture("public/wifi_"..MultiPlatform:getInstance():getNetworkStrength()..".png")
	end,1)
	
	schedule(loadingBar,function() 
		loadingBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())
	end,1)

	signal:setVisible(false)
	-- timeText:setString(os.date("%H:%M",os.time()))
	-- wifi:setTexture("public/wifi_"..MultiPlatform:getInstance():getWIFIStatus()..".png")
	-- loadingBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())
	-- schedule(timeText,function()
	-- 	timeText:setString(os.date("%H:%M",os.time()))
	-- end,1)
	-- schedule(wifi,function() 
	-- 	wifi:setTexture("public/wifi_"..MultiPlatform:getInstance():getWIFIStatus()..".png")
	-- end,1)
	-- schedule(loadingBar,function() 
	-- 	loadingBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())
	-- end,1)
end

function GameViewLayer:initLastTimeDice()
	-- body
	self.lastTimeDice = self.m_csbNode:getChildByName("dicePanel")
	local dice_1 = self.lastTimeDice:getChildByName("dice_1")
	local dice_2 = self.lastTimeDice:getChildByName("dice_2")
	local dice_3 = self.lastTimeDice:getChildByName("dice_3")

	if self._scene.bIsDoubleSice then 
		dice_1:setPosition(cc.p(470.41,681.43))
		dice_2:setPosition(cc.p(534.06,681.79))
		dice_3:setPosition(cc.p(540.25,793.18))
	else
		dice_1:setPosition(cc.p(497.54,698.33))
		dice_2:setPosition(cc.p(527.68,665.14))
		dice_3:setPosition(cc.p(468.12,662.03))
	end 

	dice_1:setVisible(false)
	dice_2:setVisible(false)
	dice_3:setVisible(false)

	self.lastTimeDice.updataDice = function (lastSice )
		-- body	
		-- dump(lastSice,"lastSice",12)

		dice_1:setVisible(false)
		dice_2:setVisible(false)
		dice_3:setVisible(false)

		if self._scene.bIsDoubleSice then 
			dice_1:setPosition(cc.p(470.41,681.43))
			dice_2:setPosition(cc.p(534.06,681.79))
			dice_3:setPosition(cc.p(540.25,793.18))
		else
			dice_1:setPosition(cc.p(497.54,698.33))
			dice_2:setPosition(cc.p(527.68,665.14))
			dice_3:setPosition(cc.p(468.12,662.03))
		end 
		

		if lastSice[1] < 255 then 
			dice_1:setVisible(true)
			dice_1:setTexture("dice/x_dice_"..self._scene:getDiceIndex().."_"..lastSice[1]..".png")
		end

		if lastSice[2] < 255 then 
			dice_2:setVisible(true)
			dice_2:setTexture("dice/x_dice_"..self._scene:getDiceIndex().."_"..lastSice[2]..".png")
		end

		if lastSice[3] < 255 then 
			dice_3:setVisible(true)
			dice_3:setTexture("dice/x_dice_"..self._scene:getDiceIndex().."_"..lastSice[3]..".png")
		end
	end
	-- self.lastTimeDice:setVisible(false)
end

function GameViewLayer:initBellPanel()
	-- body
	self.bell = self.m_csbNode:getChildByName("bell")
	local bellTime = self.bell:getChildByName("bell_time")

	self.bell.updataTime  = function (time)
		-- body
		-- print("updataTime",time)
		self.bell.show()
		bellTime:setString(""..time)
		bellTime:stopAllActions()
		schedule(bellTime,function()
			-- body
			time = time - 1
			-- print("updataTime",time)
			bellTime:setString(""..time)
			if time <= 0 then 
				bellTime:stopAllActions()
				self.bell.hide()
			end
		end,1)
	end

	self.bell.show = function ()
		-- body
		self.bell:setVisible(true)
	end

	self.bell.hide = function ()
		-- body
		self.bell:setVisible(false)
	end

	self.aniBell = self.m_csbNode:getChildByName("aniBell")

	local aniBellTime = self.aniBell:getChildByName("bell_time")
	self.aniBell.updataTime  = function (time)
		-- body
		-- print("updataTime",time)
		self.aniBell.show()
		aniBellTime:setString(""..time)
		aniBellTime:stopAllActions()
		schedule(aniBellTime,function()
			-- body
			time = time - 1
			-- print("updataTime",time)
			aniBellTime:setString(""..time)
			if time <= 0 then 
				aniBellTime:stopAllActions()
				self.aniBell.hide()
			end
		end,1)
	end

	self.aniBell.show = function ()
		-- body
		self.aniBell:setVisible(true)
	end

	self.aniBell.hide = function ()
		-- body
		self.aniBell:setVisible(false)
	end

	self.bell.hide()
	self.aniBell.hide()
end

function GameViewLayer:initBetPanel()
	-- 下注区域
	self.betPanel = self.m_csbNode:getChildByName("betPanel")
	local betBnt = {}
	for i=1,6 do
		local id = i
		local btn = self.betPanel:getChildByName("bet_"..id)
		btn:addClickEventListener(function ()
			-- body
			print("self.isDiceAniOver == ",self.isDiceAniOver)
			if self._scene.gameStatus ~= cmd.GAME_STATUS_PLAY or self.isDiceAniOver then
				-- showToast(self._scene,"不是下注阶段~~",1)
				return 
			end



			-- self:buttonSound()
			ExternalFun.playSoundEffect("betBaozi.mp3")

			if self:isMyBanker() then
				return
			end
			local betNum = 0
			for k,v in pairs(self.betChipBet) do
				if v.isCheck then 
					betNum = v.betNum-- tonumber(v.betText:getString())
				end
			end


			if betNum > self._scene:GetMeUserItem().lScore and not GlobalUserItem.bPrivateRoom then 
				showToast(self._scene,"金币不足~~",1)
				return 
			end


			local index = self.config[id]
			self._scene:sendGameBetInfo(1,index,betNum,0)
		end)
	end
end

function GameViewLayer:initChipLayer()

	local spriteFrameCache = cc.SpriteFrameCache:getInstance()
	spriteFrameCache:addSpriteFrames("chip/fly_chip.plist","chip/fly_chip.png")

	self.chipLaer = ChipLayer:create(self._scene)
	self.m_csbNode:getChildByName("chipLayer"):addChild(self.chipLaer)
end

function GameViewLayer:initWanPanel()
	-- body
	if self.wanPanel then
		self.wanPanel:removeFromParent()
	end

	self.wanPanel = ccui.Layout:create()
	self.wanPanel:ignoreContentAdaptWithSize(false)
	self.wanPanel:setAnchorPoint(0.5, 0.5)
	self.wanPanel:setPosition(822, 675)
	self.wanPanel:setContentSize(cc.size(150,150))
	self.lastTimeDice:addChild(self.wanPanel)

	self.wanyin = cc.Sprite:create("public/wanyin.png")
	self.wanyin:setPosition(75.5854,5)
	self.wanPanel:addChild(self.wanyin)

	self.wandi = cc.Sprite:create("public/wandi.png")
	self.wandi:setPosition(75.5854, 42.6547)
	self.wandi:setScaleX(0.4000)
	self.wandi:setScaleY(0.4000)
	self.wanPanel:addChild(self.wandi)
	
	self.dice1 = cc.Sprite:create("dice/x_dice_2_0.png")
	self.dice1:setPosition(45, 70)
	self.dice1:setScaleX(0.8000)
	self.dice1:setScaleY(0.8000)
	self.wanPanel:addChild(self.dice1)

	self.dice2 = cc.Sprite:create("dice/x_dice_2_2.png")
	self.dice2:setPosition(105, 70)
	self.dice2:setScaleX(0.8000)
	self.dice2:setScaleY(0.8000)
	self.wanPanel:addChild(self.dice2)

	self.dice3 = cc.Sprite:create("dice/x_dice_2_1.png")
	self.dice3:setPosition(75, 40)
	self.dice3:setScaleX(0.8000)
	self.dice3:setScaleY(0.8000)
	self.wanPanel:addChild(self.dice3)

	self.dice1:setVisible(false)
	self.dice2:setVisible(false)
	self.dice3:setVisible(false)

	self.gai = cc.Sprite:create("public/gai.png")
	self.gai:setPosition(76, 75)
	self.gai:setScaleX(0.4000)
	self.gai:setScaleY(0.4000)
	self.wanPanel:addChild(self.gai)

	self.wanHand = cc.Sprite:create("public/hand.png")
	self.wanPanel:addChild(self.wanHand)
	self.wanHand:setAnchorPoint(0,1)
	self.wanHand:setPosition(100,50)
	self.wanHand:setVisible(false)

	local move1 = cc.MoveTo:create(0.3,cc.p(80,80))
	local move2 = cc.MoveTo:create(0.3,cc.p(100,50))
	local seq = cc.Sequence:create({move1,move2})
	self.wanHand:runAction(cc.RepeatForever:create(seq))

	self.yaoBtn = ccui.Button:create()
   	self.yaoBtn:setScale9Enabled(true)
    self.yaoBtn:setContentSize(150,150)
    self.yaoBtn:setPosition(150/2, 150/2)
    self.yaoBtn:setEnabled(false)
    self.yaoBtn:addClickEventListener(function()
		self.wanHand:setVisible(false)
		self.yaoBtn:setEnabled(false)
		self._scene:sendBankerYao()
	end)
	self.wanPanel:addChild(self.yaoBtn)

	self.kaiBtn = ccui.Button:create()
   	self.kaiBtn:setScale9Enabled(true)
    self.kaiBtn:setContentSize(150,150)
    self.kaiBtn:setPosition(150/2, 150/2)
    self.kaiBtn:setEnabled(false)
    self.kaiBtn:addClickEventListener(function()
		self.wanHand:setVisible(false)
		self.kaiBtn:setEnabled(false)
		self._scene:sendBankerOpen()
	end)
	self.wanPanel:addChild(self.kaiBtn)
end

function GameViewLayer:initBetNumberLayer()
	-- body
	self.betNumberLayer = self.m_csbNode:getChildByName("betNumberLayer")
	self.betNumberLayer.myText = {}
	self.betNumberLayer.allText = {}
	for i=1,6 do
		self.betNumberLayer.myText[#self.betNumberLayer.myText+1] = self.betNumberLayer:getChildByName("my"..i)
		self.betNumberLayer.allText[#self.betNumberLayer.allText+1] = self.betNumberLayer:getChildByName("all"..i)
	end

	self.betNumberLayer.showMyBetNum = function (data)
		if data.cbJettonType == 1 then 
			for k,v in pairs(self.config) do
				if v == data.cbJettonArea then 
					if data.wChairID == self._scene:GetMeChairID() then 
						self._scene.m_cbGameStatus = cmd.GAME_STATUS_PLAY
						local score = tonumber(self.betNumberLayer.myText[k]:getString())
						self.betNumberLayer.myText[k]:setString(tostring(score + data.lJettonScore))
					end
					local score = tonumber(self.betNumberLayer.allText[k]:getString())
					self.betNumberLayer.allText[k]:setString(score + data.lJettonScore )
				end
			end
		end

		if data.cbJettonType == 3 then 
			for k,v in pairs(self.config) do
				if v == data.cbOldJettonArea then 
					local score = tonumber(self.betNumberLayer.allText[k]:getString())
					self.betNumberLayer.allText[k]:setString(score - data.lJettonScore )
				end
				if v == data.cbJettonArea then 
					local score = tonumber(self.betNumberLayer.allText[k]:getString())
					self.betNumberLayer.allText[k]:setString(score + data.lJettonScore )
				end
			end
		end

		if data.cbJettonType == 5 then 
			for k,v in pairs(self.config) do
				if v == data.cbJettonArea then 
					local score = tonumber(self.betNumberLayer.allText[k]:getString())
					self.betNumberLayer.allText[k]:setString(score + data.lJettonScore )
				end
			end
		end

	end

	self.betNumberLayer.initBetNum = function ()
		for i=1,6 do
			self.betNumberLayer.myText[i]:setString("0")
			self.betNumberLayer.allText[i]:setString("0")
		end
	end

	self.betNumberLayer.showAllBetInfo = function (lAreaJettonScore,lUserBetScore)
		-- body
		-- print(debug.traceback())
		-- dump(lAreaJettonScore,"lAreaJettonScore",12)
		lUserBetScore = lUserBetScore or {}
		for k,v in pairs(self.config) do
			for m,l in pairs(lAreaJettonScore) do
				if m-1 == v then
					self.betNumberLayer.allText[k]:setString(tostring(l))
					local num = tonumber(l)
					for i=#self.lJettonNum,1,-1 do
						print("num == ",num)
						print("self.lJettonNum[i] == ",self.lJettonNum[i])
						if num >= self.lJettonNum[i] then 
							local index = math.floor(tonumber(num / self.lJettonNum[i] ))
							num = num - index*self.lJettonNum[i]
							for j=1,index do
								self.chipLaer:playerJetton(0,k ,self.lJettonNum[i], true)
							end
						end
					end
				end
			end


			for m,l in pairs(lUserBetScore) do
				if m-1 == v then
					if l > 0 then 
						self._scene.m_cbGameStatus = cmd.GAME_STATUS_PLAY
					end
					self.betNumberLayer.myText[k]:setString(tostring(l))
				end
			end

		end
	end

end

function GameViewLayer:intiBetlimit()
	self.myText:setString(self._scene.lUserMaxScore.."")
	self.allText:setString(self._scene.lAreaLimitScore.."")
end

function GameViewLayer:wanYao()
	self.aniBell.hide()

	if self.YaoID then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.YaoID)
		self.YaoID = nil
	end

	self.wanyin:setVisible(false)
	self.wanHand:setVisible(false)
	self.yaoBtn:setEnabled(false)
	self.yaoRun = true
	if self.startskeleton then 
		self.startskeleton:stopAllActions()
		self.startskeleton:removeFromParent()
		self.startskeleton = nil
	end

	self:removeChildByName(NAEM_WAIT_SPINE_NODE)

	local moveTo = cc.MoveTo:create(0.5,cc.p(667,400))
	local scaleTo = cc.ScaleTo:create(0.5,1.8)
	local spawn1 = cc.Spawn:create(moveTo,scaleTo)
	
	local move1 = cc.MoveTo:create(0.3,cc.p(667,500))
	local move2 = cc.MoveTo:create(0.3,cc.p(667,400))

	local move3 = cc.MoveTo:create(0.5,cc.p(822,675))
	local scale2 = cc.ScaleTo:create(0.5,1)
	local spawn2 = cc.Spawn:create(move3,scale2)

	local delaytime = cc.DelayTime:create(0.5)

	self.wanPanel:stopAllActions()
	self.wanPanel:runAction(cc.Sequence:create({delaytime,cc.CallFunc:create(function()
		-- body
		ExternalFun.playSoundEffect("yao.mp3")
	end)}))

	local callfunc = cc.CallFunc:create(function()
		self.wanyin:setVisible(true)
		self:startGameBetSpine()
	end)

	local seq = cc.Sequence:create({spawn1,move1,move2,spawn2,callfunc})
	self.wanPanel:runAction(seq)
end

function GameViewLayer:wanKai( )
	-- 本局开奖骰子
	self.wanyin:setVisible(false)
	self.wanHand:setVisible(false)
	self.kaiBtn:setEnabled(false)
	
	if not self._scene.gameEndData.cbSice then 
		print("结算开骰 nil")
		return 
	end
	local cbSice = clone(self._scene.gameEndData.cbSice[1])
	self.kaiRun = true

	self.aniBell.hide()
	self:removeChildByName(NAEM_GAME_END_SPINE)

	self.dice1:setVisible(false)
	self.dice2:setVisible(false)
	self.dice3:setVisible(false)
			
	if cbSice[1] < 255 then 
		self.dice1:setTexture("dice/x_dice_"..self._scene:getDiceIndex().."_"..cbSice[1]..".png")
		self.dice1:setVisible(true)
	end

	if cbSice[2] < 255 then 
		self.dice2:setTexture("dice/x_dice_"..self._scene:getDiceIndex().."_"..cbSice[2]..".png")
		self.dice2:setVisible(true)
	end

	if cbSice[3] < 255 then 
		self.dice3:setTexture("dice/x_dice_"..self._scene:getDiceIndex().."_"..cbSice[3]..".png")
		self.dice3:setVisible(true)
	end

	local moveTo = cc.MoveTo:create(0.5,cc.p(667,400))
	local scaleTo = cc.ScaleTo:create(0.5,1.8)
	local spawn1 = cc.Spawn:create(moveTo,scaleTo)
	self.wanPanel:stopAllActions()
	local function callback()
		-- body
		local move = cc.MoveTo:create(0.3,cc.p(100,140))
		local rotateto = cc.RotateTo:create(0.3, 15)
		local spawn = cc.Spawn:create(move,rotateto)
		local fadeout = cc.FadeOut:create(0.5)

		self.gai:runAction(cc.Sequence:create({spawn,fadeout}))

		local delaytime = cc.DelayTime:create(0.3)
		local fadeout1 = cc.FadeOut:create(0.5)

		local function diceMove()

			local dice_1 = self.lastTimeDice:getChildByName("dice_1")
			local dice_2 = self.lastTimeDice:getChildByName("dice_2")
			local dice_3 = self.lastTimeDice:getChildByName("dice_3")

			local sound = {[2] = {"yu.mp3","xia.mp3","xie.mp3","shi.mp3","lao.mp3","hu.mp3"},
						   [3] = {"yu.mp3","xia.mp3","xie.mp3","ji.mp3","lao.mp3","hu.mp3"}}
			ExternalFun.playSoundEffect("opencard.mp3")
			-- body
			local sound_time = 1.3
			for i=1,#cbSice do
				if cbSice[i] ~= 255 then 
					performWithDelay(dice_1,function ()
						ExternalFun.playSoundEffect(sound[self._scene:getDiceIndex()][cbSice[i]+1])
					end,sound_time)
					sound_time = sound_time + 1
				end
			end
				
			
			if self._scene.bIsDoubleSice then 
				dice_1:setPosition(cc.p(470.41,681.43))
				dice_2:setPosition(cc.p(534.06,681.79))
				dice_3:setPosition(cc.p(540.25,793.18))
			else
				dice_1:setPosition(cc.p(497.54,698.33))
				dice_2:setPosition(cc.p(527.68,665.14))
				dice_3:setPosition(cc.p(468.12,662.03))
			end 

			local x , y = dice_1:getPosition()
			local move1 = cc.MoveTo:create(0.3,self.wanPanel:convertToNodeSpace(cc.p(x,y)))
			x , y = dice_2:getPosition()
			local move2 = cc.MoveTo:create(0.3,self.wanPanel:convertToNodeSpace(cc.p(x,y)))
			x , y = dice_3:getPosition()
			local move3 = cc.MoveTo:create(0.3,self.wanPanel:convertToNodeSpace(cc.p(x,y)))

			local scale1 = cc.ScaleTo:create(0.3,0.55)
			local scale2 = cc.ScaleTo:create(0.3,0.55)
			local scale3 = cc.ScaleTo:create(0.3,0.55)

			self.dice1:runAction(move1)
			self.dice2:runAction(move2)
			self.dice3:runAction(move3)

			self.dice1:runAction(scale1)
			self.dice2:runAction(scale2)
			self.dice3:runAction(scale3)

			performWithDelay(self.dice1,function ()
				-- 显示单局结算
				self:flyChip(0,0,0,true)
				self.lastTimeDice.updataDice(clone(self._scene.gameEndData.cbSice[1]))
				self:initWanPanel()

			end,0.5)

		end
		local callfunc = cc.CallFunc:create(diceMove)
		self.wandi:runAction(cc.Sequence:create({delaytime,fadeout1,callfunc}))
	end

	local callfunc = cc.CallFunc:create(callback)

	local seq = cc.Sequence:create({spawn1,move1,move2,spawn2,callfunc})
	self.wanPanel:runAction(seq)
end

function GameViewLayer:flyChip(id,index,num ,isGameEnd )
	isGameEnd = isGameEnd or false
	if isGameEnd then 
		self.chipLaer:bankerWinFlyChip()
	else
		self.chipLaer:playerJetton(id,index,num,false)
	end

end

function GameViewLayer:removeAllChip()
	-- body
	self.chipLaer:removeAllChildren()
end

function GameViewLayer:addBtnEventListener()
	-- body
	self.startGame:addClickEventListener(handler(self,self.onStartGame))

	self.ruleBtn:addClickEventListener(handler(self,self.onRule))
	self.setBtn:addClickEventListener(handler(self,self.onSet))
	self.tableBtn:addClickEventListener(handler(self,self.onTableGame))
	self.exitBtn:addClickEventListener(handler(self,self.onExitGame))

	self.baoZiBtn:addClickEventListener(handler(self,self.onBaoZi))
	self.lianChuanBtn:addClickEventListener(handler(self,self.onLianChuan))
	self.nuoDanBtn:addClickEventListener(handler(self,self.onNuoDan))
	self.nuoChuanBtn:addClickEventListener(handler(self,self.onNuoChuan))

    self.historyBtn:addClickEventListener(handler(self,self.onHisetory))
    self.betinfoBtn:addClickEventListener(handler(self,self.onBetInfo))
    self.chatBtn:addClickEventListener(handler(self,self.onChat))
    self.voiceBtn:addTouchEventListener(handler(self,self.onVoice))
    self.shangzhuangBtn:addClickEventListener(handler(self,self.onShangZhuang))
    self.btn_control:addClickEventListener(handler(self,self.showControlLater))
end

--[[
	以下是按钮回调函数
]]

-- 开始游戏
function GameViewLayer:onStartGame(sender)
	print("开始游戏")
	self:buttonSound()
	self._scene:sendGameStart()
    sender:setVisible(false)
    -- self._scene:SendUserReady()
    self.bell.hide()
    self._scene:KillGameClock()
	self.avatrPanel.updataReady(1,true)
	self.betNumberLayer.initBetNum()
	self.chipLaer:removeAllChip()

end

-- 规则
function GameViewLayer:onRule() 
	print("规则")
	self:buttonSound()

	local csbnode = cc.CSLoader:createNode("ui/RuleLayer.csb");
	self:addChild(csbnode,40)
	csbnode:setName(NAME_RULE_LAYER)

	csbnode:getChildByName("clode"):addClickEventListener(function ()
		-- body
		csbnode:removeFromParent()
	end)

end

-- 设置
function GameViewLayer:onSet() 
	print("设置")
	self:buttonSound()
	local l = SetLayer:create(self._scene)
	self:addChild(l,40)
	l:setName(NAME_SET_LAYER)
end

-- 换桌
function GameViewLayer:onTableGame() 
	print("换桌")
	self:buttonSound()
	self._scene:onChangeDesk()
end

-- 退出游戏
function GameViewLayer:onExitGame() 
	print("退出游戏")
	self:buttonSound()
	-- self._scene:onExitTable()

	local creatorID = nil
	if GlobalUserItem.bPrivateGoldRoom then
		creatorID = PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID
	end
	self._scene:onQueryExitGame(creatorID)

	-- self._scene:onQueryExitGame()
end

-- 豹子下注
function GameViewLayer:onBaoZi() 
	print("豹子下注")
	self:buttonSound()
	if self:isMyBanker() then
		return
	end
	self.showSpecial = SHOW_BAOZI
	self._scene:checkTheBaoZi()
end

-- 连串
function GameViewLayer:onLianChuan() 
	print("连串")
	self:buttonSound()
	if self:isMyBanker() then
		return
	end	
	self.showSpecial = SHOW_LIAN_CHUAN
	self._scene:checkTheLianChuan()
end

-- 挪单
function GameViewLayer:onNuoDan() 
	print("挪单")
	self:buttonSound()
	if self:isMyBanker() then
		return
	end	
	self.showSpecial = SHOW_NUO_DAN
	self._scene:checkTheOrder()
end

-- 挪串
function GameViewLayer:onNuoChuan() 
	print("挪串")
	self:buttonSound()
	if self:isMyBanker() then
		return
	end	
	self.showSpecial = SHOW_NUO_CHUAN
	self._scene:checkTheNuoChuan()
end

function GameViewLayer:showOperateLayer(id,data)
	-- id = id - 0xfa

	local l = self.m_csbNode:getChildByName("Panel_1")

	local layer = OperateLayer:create(self._scene,self.config)
	l:addChild(layer,40)
	layer:setName(NAME_OPERATE_LAYER)
	layer:show(id,data)
	layer:setExitCall(function ()
		self.chipPanel:setVisible(true)
		self.actionPanel:setVisible(true)
	end)

	self.chipPanel:setVisible(false)
	self.actionPanel:setVisible(false)

end

function GameViewLayer:hideOperateLayer()
	-- body
	local l = self.m_csbNode:getChildByName("Panel_1")
	local layer = l:getChildByName(NAME_OPERATE_LAYER)
	if layer then 
		layer:removeFromParent()
	end

end

-- 历史纪录
function GameViewLayer:onHisetory() 
	print("历史纪录")
	self:buttonSound()
	self._scene:getHistoryRecord()
end

-- 下注详情 
function GameViewLayer:onBetInfo() 
	print("下注详情")
	self:buttonSound()
	self._scene:checkTheBetting()
end

-- 聊天
function GameViewLayer:onChat() 
	print("聊天")
	self:buttonSound()
    local l = GameChatLayer:create(self._scene._gameFrame,tabconfig)
    self:addChild(l,40)
    l:showGameChat(true)
    l:setName(NAME_CAHT_LAYER)


end

function GameViewLayer:showControlLater()
	-- body
	self:buttonSound()
    local tab = {}
    tab.cbReqType = 1
    tab.lStorageStart = 1
    tab.lStorageDeduct = 1
    tab.lStorageCurrent = 1
    tab.lStorageMax1 = 1
    tab.lStorageMul1 = 1
    tab.lStorageMax2 = 1
    tab.lStorageMul2 = 1

	self._scene:sendUpDataInventory(tab)
end

-- 语音
function GameViewLayer:onVoice(ref, tType) 
	print("语音")
	-- self:buttonSound()
	if tType == ccui.TouchEventType.began then
	    self._scene._scene:startVoiceRecord()

	elseif tType == ccui.TouchEventType.ended or tType == ccui.TouchEventType.canceled then
	    self._scene._scene:stopVoiceRecord()

	end
	-- self:startGameSpine()
end

-- 上庄 
function GameViewLayer:onShangZhuang() 
	print("上庄")
	self:buttonSound()
	local layer = BankerList:create(self._scene)
	layer:showList(self._scene.bankerList)
	self:addChild(layer,40)
	layer:setName(NAME_BANKERLIST_LAYER)
end


--[[
	以上是按钮回调函数

]]

function GameViewLayer:initBankerLiet(bankerList)
	-- body
	local layer = self:getChildByName(NAME_BANKERLIST_LAYER)
	if layer then
		layer:showList(bankerList)
	end
	self:changeBankerBtn()
end

function GameViewLayer:showLastSice( lastSice )
	-- body
	lastSice = lastSice or {255,255,255}
	self.lastTimeDice.updataDice(lastSice)
end

-- 显示准备
function GameViewLayer:showPreparation()
	-- body
	self.startGame:setVisible(false)
	if GlobalUserItem.bPrivateRoom then 
		local index = 0
		for k,v in pairs(self._scene.m_GameUserItem) do
			if v then 
				index = index + 1
			end
		end

		if index >= 2 and self._scene.bankerid == self._scene:GetMeChairID() and PriRoom:getInstance().m_tabPriData.dwPlayCount <= 0 then 
			self.startGame:setVisible(true)
		end
	else
		self.startGame:setVisible(false)
	end

end

function GameViewLayer:showTimes(times)
	-- body
	self.bell.updataTime(times)
end

function GameViewLayer:showBtnNum(data)
	-- body
	self.betChipBet[1].betText:setString(self._scene:formatScoreTextEx(data.lJettonNum1))
	self.betChipBet[2].betText:setString(self._scene:formatScoreTextEx(data.lJettonNum2))
	self.betChipBet[3].betText:setString(self._scene:formatScoreTextEx(data.lJettonNum3))
	self.betChipBet[4].betText:setString(self._scene:formatScoreTextEx(data.lJettonNum4))
	self.betChipBet[5].betText:setString(self._scene:formatScoreTextEx(data.lJettonNum5))

	self.betChipBet[1].betNum = data.lJettonNum1
	self.betChipBet[2].betNum = data.lJettonNum2
	self.betChipBet[3].betNum = data.lJettonNum3
	self.betChipBet[4].betNum = data.lJettonNum4
	self.betChipBet[5].betNum = data.lJettonNum5

	self.betChipBet[1].effect:setVisible(true)	
	self.betChipBet[1].isCheck = true
	local blink = cc.Blink:create(1,2)
	self.betChipBet[1].effect:runAction(cc.RepeatForever:create(blink))

	self.lJettonNum = {}
	self.lJettonNum[#self.lJettonNum+1] = data.lJettonNum1
	self.lJettonNum[#self.lJettonNum+1] = data.lJettonNum2
	self.lJettonNum[#self.lJettonNum+1] = data.lJettonNum3
	self.lJettonNum[#self.lJettonNum+1] = data.lJettonNum4
	self.lJettonNum[#self.lJettonNum+1] = data.lJettonNum5

	self.chipLaer:setChipConfig(self.lJettonNum)
end

function GameViewLayer:showPlayerBet(data)
	-- 转换下下注区域

	local index = 1
	for k,v in pairs(self.config) do
		if v == data.cbJettonArea then
			index = k
			break
		end
	end

	local myChair = self._scene:GetMeChairID() 
	if data.cbJettonType == 1 then
		for k,v in pairs(self._scene.gameUserChair) do
			if v == data.wChairID and data.wChairID ~= 6 then 
				self:flyChip(k,index,data.lJettonScore)
			end
		end
	end

	self.betNumberLayer.showMyBetNum(data)
end

-- 按钮音效
function GameViewLayer:buttonSound()
	-- body
	ExternalFun.playSoundEffect("buttonSound.mp3") 

end

function GameViewLayer:isMyBanker()
	-- body
	if self._scene.isMyBanker then
		showToast(self._scene, "庄家请不要下注!", 1)
		return true
	end
	return false
end

-- 更新玩家
function GameViewLayer:updataDeskChair(userItem,deskChair)
	-- body
	for k,v in pairs(deskChair) do
		if v == userItem.wChairID and v ~= 65535 then
			self.avatrPanel.show(k,userItem)
			self.avatrPanel.updataReady(k, (userItem.cbUserStatus == yl.US_READY))
        end
	end
end

-- 更新玩家
function GameViewLayer:updataDeskBanker(userItem,deskChair)

	for i=1,#self.avatrBtn do
		local head = self.avatrBtn[i]:getChildByName("head")
		if head then 
			head:removeFromParent() 
		end
	end
	
	self.avatrPanel.showBanker(false)

	for k,v in pairs(deskChair) do
		for _,l in pairs(userItem) do
			if l then 
				if v == l.wChairID and v ~= 65535 then
					self.avatrPanel.show(k,l)
		        end
			end
		end
	end
	self.avatrPanel.show(12,clone(self._scene:GetMeUserItem()))
end

function GameViewLayer:changeBankerBtn()
	if self._scene.bankerid == self._scene:GetMeChairID() then 
		self.shangzhuangBtn:loadTextures("button/btn_xiazhuang.png",0)
		self.shangzhuangBtn:loadTextureNormal("button/btn_xiazhuang.png",0)
		self.shangzhuangBtn:loadTexturePressed("button/btn_xiazhuang.png",0)
	else
		self.shangzhuangBtn:loadTextures("button/btn_shangzhuang_0.png",0)
		self.shangzhuangBtn:loadTextureNormal("button/btn_shangzhuang_0.png",0)
		self.shangzhuangBtn:loadTexturePressed("button/btn_shangzhuang_0.png",0)	
	end

	-- dump(self._scene.bankerList)
	for k,v in pairs(self._scene.bankerList) do
		if v == self._scene:GetMeChairID() then
			self.shangzhuangBtn:loadTextures("button/btn_quxiao.png",0)
			self.shangzhuangBtn:loadTextureNormal("button/btn_quxiao.png",0)
			self.shangzhuangBtn:loadTexturePressed("button/btn_quxiao.png",0)	
		end
	end
end

function GameViewLayer:startGameSpine()
	-- body
	-- ExternalFun.playSoundEffect("failgame.mp3") 
	self.yaoRun = false
	self:setGameFree()
	self:removeAllChip()
	self.actionPanel:setVisible(false)
	self.chipPanel:setVisible(false)
	

   	self.startskeleton = sp.SkeletonAnimation:create("spine/startGame/skeleton.json", "spine/startGame/skeleton.atlas", 1)
    self.startskeleton:setAnimation(0, "animation", false)
    self:addChild(self.startskeleton)
    self.startskeleton:setPosition(667,375)
    self.startskeleton:registerSpineEventHandler(function ()
    	self:waitYao()
    end,sp.EventType.ANIMATION_END)
    
    local index = 5
    self.aniBell.updataTime(index)
    self.YaoID = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
		index = index - 1 
		if index <= 0 and self.yaoRun == false then
			self:wanYao()
		end
    end,1,false)
end


function GameViewLayer:startGameBetSpine()
	-- body
	-- ExternalFun.playSoundEffect("failgame.mp3")
	ExternalFun.playSoundEffect("startbet.mp3")
	ExternalFun.playSoundEffect("startbet_1.mp3")
	self:removeChildByName(NAME_WAIIT_GAME_SPINE)
    local skeletonNode = sp.SkeletonAnimation:create("spine/startBet/skeleton.json", "spine/startBet/skeleton.atlas", 1)
    skeletonNode:setAnimation(0, "animation", false)
    self:addChild(skeletonNode)
    skeletonNode:setPosition(667,375)
    skeletonNode:registerSpineEventHandler(function ()
    	-- body
    	-- print("开始下注")
    	self:setDcieAniOver(false)
    	self:setBankerOperationpanel()
    	-- self.bell.updataTime(50)
    end,sp.EventType.ANIMATION_END)
end

function GameViewLayer:waitGameSpine()
	-- body
	self:removeChildByName(NAME_WAIIT_GAME_SPINE)
	if self._scene.bankerid and self._scene.bankerid >= 255 then
		self.notBanker:setVisible(true)
		return
	end
	
    self.gameFreeNode = sp.SkeletonAnimation:create("spine/wait/skeleton.json", "spine/wait/skeleton.atlas", 1)
    self.gameFreeNode:setAnimation(0, "animation", true)
    self:addChild(self.gameFreeNode)
    self.gameFreeNode:setPosition(667,375)
    self.gameFreeNode:setName(NAME_WAIIT_GAME_SPINE)
end

function GameViewLayer:waitYao()
	-- body
	if self._scene.bankerid == self._scene:GetMeChairID() then 
		self.wanHand:setVisible(true)
		self.yaoBtn:setEnabled(true)
	end
	self:removeChildByName(NAEM_WAIT_SPINE_NODE)
    self.waitSpineNode = sp.SkeletonAnimation:create("spine/startGame_1/skeleton.json", "spine/startGame_1/skeleton.atlas", 1)
    self.waitSpineNode:setAnimation(0, "animation", true)
    self:addChild(self.waitSpineNode)
    self.waitSpineNode:setPosition(667,375)
    self.waitSpineNode:setName(NAEM_WAIT_SPINE_NODE)

end

function GameViewLayer:gameEndSpine()
	-- body
	self.kaiRun = false
	if self._bDismissed then 
		self._bDismissed = false
		self:showResultLayer()
		return 
	end

	self:setGameFree()
	ExternalFun.playSoundEffect("endbet.mp3")
	ExternalFun.playSoundEffect("endbet_1.mp3")
	if self._scene.bankerid == self._scene:GetMeChairID() then 
		self.wanHand:setVisible(true)
		self.kaiBtn:setEnabled(true)
	end

	self:removeChildByName(NAEM_GAME_END_SPINE)
    self.shakeDice = sp.SkeletonAnimation:create("spine/stopGame/skeleton.json", "spine/stopGame/skeleton.atlas", 1)
    self.shakeDice:setAnimation(0, "animation", true)
    self:addChild(self.shakeDice)
    self.shakeDice:setPosition(667,375)
    self.shakeDice:setName(NAEM_GAME_END_SPINE)

    self.aniBell.updataTime(5)

end

-- 单局结算
function GameViewLayer:showResultLayer()
	local l = ResultLayer:create(self._scene)
	self:addChild(l,40)
	l:setName(NAME_RESULT_LAYER)
end

function GameViewLayer:setGameFree()
	-- body
	self:removeChildByName(NAME_RESULT_LAYER)
	self:removeChildByName(NAME_CAHT_LAYER)
	self:removeChildByName(NAME_BET_INIFO_LAYER)
	self:removeChildByName(NAME_BANKERLIST_LAYER)
	self:removeChildByName(NAME_SET_LAYER)
	self:removeChildByName(NAME_RULE_LAYER)
	-- self:removeChildByName(NAME_OPERATE_LAYER)
	self:hideOperateLayer()
	self:removeChildByName(NAME_RECORDLAYER)
	self:removeChildByName(NAME_WAIIT_GAME_SPINE)
	self:removeChildByName(NAEM_WAIT_SPINE_NODE)
	self:removeChildByName(NAEM_GAME_END_SPINE)

	self:setBankerOperationpanel()
	self:setDcieAniOver(false)

end

--显示聊天
function GameViewLayer:ShowUserChat(viewid ,message)
	-- print("viewid = ",viewid)
	-- dump(message)
	if message and #message > 0 then
		print("viewid = ",viewid)
		viewid = tonumber(viewid)
	   	self:removeChildByName(NAME_CAHT_LAYER)

		local head = self.avatrBtn[viewid]:getChildByName("head")
		-- dump(head,"headddddddddd",12)
		
		if head then 

			local qipao = head.qipao
			qipao:removeAllChildren()
			qipao:setVisible(true)

			--创建label
			local text = nil
			local limWidth = 20*12
			local labCountLength = cc.LabelTTF:create(message,"Arial", 20)  
			if labCountLength:getContentSize().width > limWidth then
				text = cc.LabelTTF:create(message,"Arial", 20, cc.size(limWidth, 0))
			else
				text = cc.LabelTTF:create(message,"Arial", 20)
			end
			text:addTo(qipao)
			text:setPosition(8,18)
			text:setAnchorPoint(cc.p(0, 0))
			text:setColor(cc.WHITE)
			text:enableStroke(cc.c3b(21,160,75), 1)
			-- if viewid < 8 then
			-- 	text:setPosition(10,45)
			-- 	text:setAnchorPoint( cc.p(0, 1) )
			-- else
			-- 	text:setPosition(8,45)
			-- 	text:setAnchorPoint(cc.p(0, 0.5))
			-- end
			--改变气泡大小
			qipao:setContentSize(text:getContentSize().width+28, text:getContentSize().height + 27)
				:setVisible(true)
			--动作
			text:runAction(cc.Sequence:create(
							cc.DelayTime:create(3),
							cc.CallFunc:create(function()
								qipao:setVisible(false)
								text:removeFromParent()
								text=nil
							end)
					))

		end
	end
end

function GameViewLayer:ShowUserExpression(viewid,wItemIndex)
	print("ShowUserExpression")

	if wItemIndex and viewid then
		print("viewid = ",viewid)
		viewid = tonumber(viewid)
	   	self:removeChildByName(NAME_CAHT_LAYER)

		local head = self.avatrBtn[viewid]:getChildByName("head")
		if head then 
			head:browChat(wItemIndex)
		end

	end

end

-- 语音播放开始
function GameViewLayer:onUserVoiceStart(viewid)
	viewid = tonumber(viewid)
   	self:removeChildByName(NAME_CAHT_LAYER)

	local head = self.avatrBtn[viewid]:getChildByName("head")
	
	if head then 

		local qipao = head.qipao
		qipao:removeAllChildren()
		qipao:setVisible(true)

	    -- 语音动画
	    local param = AnimationMgr.getAnimationParam()
	    param.m_fDelay = 0.1
	    param.m_strName = cmd.VOICE_ANIMATION_KEY
	    local animate = AnimationMgr.getAnimate(param)
	    local actVoiceAni = cc.RepeatForever:create(animate)

	    local spVoice = display.newSprite("public/public_blank.png")
	    spVoice:setAnchorPoint(cc.p(0.5, 0.5))
	    spVoice:runAction(actVoiceAni)
	    spVoice:setRotation(180)

		--动作
	    qipao:setContentSize(spVoice:getContentSize().width+28, spVoice:getContentSize().height + 27)
	    spVoice:addTo(qipao)
	    spVoice:setPosition(qipao:getContentSize().width * 0.5, qipao:getContentSize().height * 0.5 +7)
	end
end

-- 语音播放结束
function GameViewLayer:onUserVoiceEnded(viewid)
	viewid = tonumber(viewid)
   	self:removeChildByName(NAME_CAHT_LAYER)
	local head = self.avatrBtn[viewid]:getChildByName("head")
	if head then
		local qipao = head.qipao
		qipao:removeAllChildren()
		qipao:setVisible(false)
	end
end

function GameViewLayer:showSpecialBet(cmd_data)
	-- body
	local data = {}
	-- dump(cmd_data, "更新下注限制", 10)
	-- print(self.showSpecial)
	-- self.betNumberLayer.showAllBetInfo(cmd_data.lAreaJettonScore[1])

	if self.showSpecial == SHOW_BAOZI then
		data = cmd_data.lAllBaoZiLimit[1]
	elseif self.showSpecial == SHOW_LIAN_CHUAN then
		data = cmd_data.lAllChuanLimit[1]
	elseif self.showSpecial == SHOW_NUO_DAN then
		data = cmd_data.lAreaJettonScore[1]
	elseif self.showSpecial == SHOW_NUO_CHUAN then 
		data = cmd_data.lChuanJettonScore[1]
	end

	local l_UserLimit = cmd_data.l_UserLimit
	if l_UserLimit < 0 then l_UserLimit = 0 end

	data.l_UserLimit = l_UserLimit
	
	if self.showSpecial ~= SHOW_NIL then
		self:showOperateLayer(self.showSpecial,data)
		self.showSpecial = SHOW_NIL
	end

end

function GameViewLayer:showRecordLayer(data)

	if #data <= 0 then
		showToast(self._scene, "当前没有记录!", 1)
		return
	end

	local layer = RecordLayer:create(self._scene)
	self:addChild(layer,40)
	layer:initView(data)
	layer:setName(NAME_RECORDLAYER)
end

function GameViewLayer:intBetInfo( data )
	-- body	


	local tab = {}

	-- dump(data,"intBetInfo",12)

	for k,v in pairs(self._scene.m_GameUserItem) do
		if v then 
			local item = {}
			item.useritem = v
			
			item.lAllAreaInfo = data.lAllAreaInfo[v.wChairID+1]
			item.lAllBaoZiInfo = data.lAllBaoZiInfo[v.wChairID+1]
			item.lAllChuanInfo = data.lAllChuanInfo[v.wChairID+1]
			item.nuo_dan_info = data.nuo_dan_info[v.wChairID+1]
			item.nuo_chuan_info = data.nuo_chuan_info[v.wChairID+1]
			tab[#tab+1] = item
		end
	end

	for k,v in pairs(self._scene.gameUserChair) do
		if v ~= nil and v < 65535 then
			for _,i in pairs(self._scene.m_GameUserItem) do
				if i.wChairID == v then

					break
				end
			end
		end
	end

	-- 清空没有下注的玩家
	local betInfo = {}
	for k,v in pairs(tab) do
		local isNil = 0
		for m,l in pairs(v) do
			if m == "lAllAreaInfo" or m == "lAllBaoZiInfo" or m == "lAllChuanInfo" then 
				for w,n in pairs(l) do
					isNil = isNil + n
				end
			end

			if m == "nuo_dan_info" or m == "nuo_chuan_info" then
				for w,n in pairs(l) do
					isNil = isNil + n.score
				end
			end
		end
		if isNil > 0 then 
			betInfo[#betInfo+1] = v
		end
	end

	if #betInfo<=0 then 
		showToast(self._scene, "暂无下注详情!", 1)
		return
	end

	local layer = BetInfoLayer:create(self._scene)
	self:addChild(layer,40)
	layer:setData(betInfo)
	layer:setName(NAME_BET_INIFO_LAYER)

end


function GameViewLayer:setGameModle()
	print("setGameModle")
    self.config = {}
    if self._scene.bIsDoubleSice then 
    	self.config = {4,5,3,2,0,1} -- two
    else
    	self.config = {4,5,3,1,0,2} -- three
    end

    local bgSp = self.m_csbNode:getChildByName("bg"):getChildByName("bgSp")
    bgSp:setTexture("public/two_bet_desktops.png")

    if self._scene.bIsDoubleSice then 
    	bgSp:setTexture("public/two_bet_desktops.png") -- two
    else
    	bgSp:setTexture("public/three_bet_desktops.png") -- three
    end

    	-- 三骰 没有豹子玩法
	if not self._scene.bIsDoubleSice then 
		self.baoZiBtn:setVisible(false)
		self.baoZiBtn:setEnabled(false)
		self.nuoChuanBtn:setPosition(374.62,50.67)
		self.nuoDanBtn:setPosition(224.39,50.67)
		self.lianChuanBtn:setPosition(74.15,50.67)
    end

    self.chipLaer:setDiceConfig(self.config)

end

function GameViewLayer:setDcieAniOver(isOver)
	-- body
	isOver = isOver or false
	self.isDiceAniOver = isOver
end

function GameViewLayer:setBankerOperationpanel()
	-- 庄家不显示下注操作界面哈
	-- print(debug.traceback())
	-- print("setBankerOperationpanel ",isMyBanker)

	if self._scene.gameStatus == cmd.GAME_STATUS_PLAY then 
		if self._scene.bankerid == self._scene:GetMeChairID() then
			self.actionPanel:setVisible(false)
			self.chipPanel:setVisible(false)
		else
			self.actionPanel:setVisible(true)
			self.chipPanel:setVisible(true)	
		end
	else
		self.actionPanel:setVisible(false)
		self.chipPanel:setVisible(false)
	end
end

function GameViewLayer:showBankLayer()
	-- body
	--银行未开通
	if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
		showToast(self,"初次使用，请先开通银行！",1)
		return
	end

	--房间规则
	local rule = self:getParentNode()._roomRule
	if rule == yl.GAME_GENRE_SCORE 
	or rule == yl.GAME_GENRE_EDUCATE then 
		print("练习 or 积分房")
	end
	-- 当前游戏币场
	local rom = GlobalUserItem.GetRoomInfo()
	if nil ~= rom then
		if rom.wServerType ~= yl.GAME_GENRE_GOLD then
			showToast(self, "当前房间禁止操作银行!", 2)
			return
		end
	end

	-- if false == self:getParentNode():getFrame():OnGameAllowBankTake() then
	-- 	--showToast(self,"不允许银行取款操作操作",1)
	-- 	--return
	-- end

	local l = BankLayer:create(self._scene)
	l:setName(NAME_BANK_LAYER)
	self:addChild(l,40)
end

--银行操作成功
function GameViewLayer:onBankSuccess( )
	print("GameViewLayer:onBankSuccess")
	local bank_success = self._scene.bank_success
	if nil == bank_success then
		return
	end
	GlobalUserItem.tabAccountInfo.lUserScore = bank_success.lUserScore
	GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure
	local bankLayer = self:getChildByName(NAME_BANK_LAYER)
	if nil ~= bankLayer then
		print("GameViewLayer:onBankSuccess refreshScore")
		bankLayer:refreshScore()
	end

	showToast(self._scene, bank_success.szDescribrString, 2)
end


--银行操作失败
function GameViewLayer:onBankFailure( )
	print("GameViewLayer:onBankFailure")
	local bank_fail = self:getParentNode().bank_fail
	if nil == bank_fail then
		return
	end

	showToast(self._scene, bank_fail.szDescribeString, 2)
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
	print("GameViewLayer:onGetBankInfo")
	bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
	local bankLayer = self:getChildByName(NAME_BANK_LAYER)
	if nil ~= bankLayer then
		local str = "温馨提示:取款将扣除" .. bankinfo.wRevenueTake .. "%的手续费"
		bankLayer.m_bankLayer.m_textTips:setString(str)
	end
end

function GameViewLayer:dismissResult()
  	self._bDismissed = true
end

function GameViewLayer:OnResetGameEngine()
	-- body
   	self._bDismissed = false

    self:setGameFree()

    self.startGame:setVisible(false)
    self.voiceBtn:setVisible(not GlobalUserItem.bVideo)
    self.shangzhuangBtn:setVisible(not GlobalUserItem.bPrivateRoom)
	if GlobalUserItem.bPrivateGoldRoom then 
		self.shangzhuangBtn:setVisible(true)
	end
    self:removeAllChip()
    self:initWanPanel()

	if self.YaoID then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.YaoID)
		self.YaoID = nil
	end

	if self.startskeleton then 
		self.startskeleton:stopAllActions()
		self.startskeleton:removeFromParent()
		self.startskeleton = nil
	end

	self.notBanker:setVisible(false)
	self:setBankerOperationpanel()
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = 1

-- self.avatrPanel = self.m_csbNode:getChildByName("avatrPanel")

        for k,v in pairs(self.avatrBtn) do
              if v then
                  local head = v:getChildByName("head")
                  if head then 
	                  if head.useritem.dwUserID ==  dwUserID then
	                        beginPoint = cc.p(v:getPosition())
	                  elseif head.useritem.dwUserID ==  dwRecvUserID then
	                        endPoint = cc.p(v:getPosition())
	                        zOrder = v:getZOrder() + 1
	                  end
	              end
              end
        end

      if beginPoint and endPoint then
              local actSprite = cc.Sprite:create(string.format("public/prop_%d.png",dwPropID)):move(beginPoint):addTo(self.avatrPanel,zOrder)
              if actSprite then
                  actSprite:runAction(cc.Sequence:create(cc.DelayTime:create(0.2),cc.MoveTo:create(0.5,endPoint),cc.CallFunc:create(function ( ... )
                      actSprite:removeFromParent()
                      local animateName = {"chun","hua","fanqie","ji","jidan","zan"}
                      local spine = SpineObj:create("public/interact.json", "public/interact.atlas", false)
                      spine:setAnimation(0, animateName[dwPropID], false)
                      spine:registerSpineEventHandler(function (event)
                        spine:destroy()
                      end, 2)
                      spine:setPosition(endPoint)
                      self:addChild(spine,zOrder)
                  end)))
              end     
      end
end

return GameViewLayer