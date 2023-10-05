local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)

if not yl then
require("client.src.plaza.models.yl")
end
local GameChat = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local cmd = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.models.CMD_Game")
local SetLayer = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.views.layer.SetLayer")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local CompareView = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.views.layer.CompareView")
local GameEndView = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.views.layer.GameEndView")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local SpineObj = appdf.req("client.src.external.SpineObj")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
--
GameViewLayer.BT_EXIT 				= 1
GameViewLayer.BT_CHAT 				= 2
GameViewLayer.BT_GIVEUP				= 3
GameViewLayer.BT_READY				= 4
GameViewLayer.BT_LOOKCARD			= 5
GameViewLayer.BT_FOLLOW				= 6
GameViewLayer.BT_ADDSCORE			= 7
GameViewLayer.BT_CHIP				= 201

GameViewLayer.BT_COMPARE 			= 12
GameViewLayer.BT_CARDTYPE			= 13
GameViewLayer.BT_SET				= 14
GameViewLayer.BT_MENU				= 15
GameViewLayer.BT_BANK 				= 16
GameViewLayer.BT_VOICE_ENDED		= 17
GameViewLayer.BT_VOICE_BEGAN		= 18
GameViewLayer.BT_HELP 				= 19
GameViewLayer.BT_BETALL             = 20

GameViewLayer.BT_RULE               = 36               --规则
GameViewLayer.BT_SCORELIST          = 37               --游戏流水

GameViewLayer.CHIPNUM 				= 100
GameViewLayer.BT_ROOMINFO    		=38
GameViewLayer.BT_CHANGE    		=39
GameViewLayer.BT_LOCATION   =40


GameViewLayer.BT_FOLLOW_BEF				= 41
GameViewLayer.BT_CHAT     =42

GameViewLayer.BT_CLOSECHIPBG				= 50

GameViewLayer.ZORDER_SETTING     = 100

GameViewLayer.BT_START     = 101
GameViewLayer.BT_SIT     = 102

local ptPlayer = {cc.p(100, 530), cc.p(100, 290), cc.p(480, 220), cc.p(1234, 290), cc.p(1234, 530)}
local ptCard = {cc.p(210, 550), cc.p(210, 310), cc.p(620, 200), cc.p(1054, 310), cc.p(1054, 550)}
local ptArrow ={cc.p(100, 530), cc.p(100, 290), cc.p(480, 220), cc.p(1234, 290), cc.p(1234, 530)}
local ptReady = {cc.p(244, 520), cc.p(244, 310), cc.p(640, 297), cc.p(1090, 310), cc.p(1090, 550)}
local ptLookCard = {cc.p(245, 541), cc.p(245, 301), cc.p(670, 284), cc.p(1089, 301), cc.p(1089, 541)}
local ptAddScore = {cc.p(245, 542), cc.p(245, 302), cc.p(670, 290), cc.p(1089, 302), cc.p(1089, 542)}
local ptGiveUpCard = {cc.p(245, 541), cc.p(245, 301), cc.p(670, 200), cc.p(1089, 301), cc.p(1089, 541)}

local ptChat = {cc.p(160, 530), cc.p(160, 290), cc.p(480, 300), cc.p(1200, 270), cc.p(1159, 530)}

local ptUserInfo = {cc.p(55.6, 522.29-330), cc.p(55.6, 290), cc.p(50, 46.5), cc.p(1276.4-510, 290), cc.p(1276.4-510, 522.29-330)}
local anchorPoint = {cc.p(0, 1), cc.p(0, 0), cc.p(0, 0), cc.p(1, 0), cc.p(1, 1)}


function GameViewLayer:OnResetView()
	self:stopAllActions()

	if PriRoom and GlobalUserItem.dwCurRoomGroupID ~= 0 then
	else
		self.btReady:setVisible(false)
	end

	self.btChange:setVisible(false)


	self:OnShowIntroduce(false)

	self:OnUpdataHuiHe(0 , 0)

	self.autoFollow = false
	self.m_befFollowIcon:setVisible(self.autoFollow)

	self.m_ChipNode:setVisible(false)
	self.nodeButtomButton:setVisible(false)
	self.nodeBeforehandButton:setVisible(false)
    

	self:SetBanker(yl.INVALID_CHAIR)
	--self:SetRoomHolder(yl.INVALID_CHAIR)
	self:SetAllTableScore(0)
	self:SetCompareCard(false)
	self:CleanAllJettons()

	for i = 1 ,cmd.GAME_PLAYER do
		self:SetUserOperate(i)
		
		self:SetUserTableScore(i, 0)
		self:SetUserCard(i, nil)
        self:clearCard(i)
	end
	self:SetMyCardType()
	self.nAllTableScore = 0 --总分
	self.nChipsNum = 0	  --小筹码个数
	self.BigChipValue = 0 --转换玩的筹码数值
end

function GameViewLayer:onExit()
	print("GameViewLayer onExit")
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile(cmd.RES.."Card.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey(cmd.RES.."Card.png")
	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timeSchedule)

    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
    self.m_actVoiceAni:release()
    self.m_actVoiceAni = nil

    if self._beginTimeId then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
		self._beginTimeId = nil
	end
	
end

function GameViewLayer:getParentNode()
    return self._scene
end

function GameViewLayer:initCsbRes()
	local this = self
	local rootLayer, csbNode = ExternalFun.loadRootCSB("game_res/Game.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode
    self.m_csbNode:setPosition(cc.p(display.width/2 , display.height/2))

    --按钮回调
	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
			this:OnButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --所有下注
	self.m_txtAllScore = csbNode:getChildByName("AllScore")
	self:SetAllTableScore(0)
-------------------------**************************////////////////////////////////================================================================

	
	--底部 预操作 按钮父节点
	self.nodeBeforehandButton = self.m_csbNode:getChildByName("Panel_befOperate")
		:setVisible(false)

	--弃牌按钮
	self.btGiveUp_bef = self.nodeBeforehandButton:getChildByName("Button_giveUp")
		:setEnabled(false)
		:setTag(GameViewLayer.BT_GIVEUP)

	--看牌按钮
	self.btLookCard_bef = self.nodeBeforehandButton:getChildByName("Button_look")
		:setEnabled(false)
		:setTag(GameViewLayer.BT_LOOKCARD)

	--跟注按钮
	self.btFollow_bef = self.nodeBeforehandButton:getChildByName("Button_befFollow")
		:setTag(GameViewLayer.BT_FOLLOW_BEF)

		--跟到底勾选
	self.m_befFollowIcon= self.btFollow_bef:getChildByName("BefFollow_icon")
		:setVisible(self.autoFollow)


-------------------------**************************////////////////////////////////================================================================


	--底部按钮父节点
	self.nodeButtomButton = self.m_csbNode:getChildByName("Panel_operate")
		:setVisible(false)

	--弃牌按钮
	self.btGiveUp = self.nodeButtomButton:getChildByName("Button_giveUp")
		:setEnabled(false)
		:setTag(GameViewLayer.BT_GIVEUP)

	--看牌按钮
	self.btLookCard = self.nodeButtomButton:getChildByName("Button_look")
		:setEnabled(false)
		:setTag(GameViewLayer.BT_LOOKCARD)

	
	--比牌按钮
	self.btCompare = self.nodeButtomButton:getChildByName("Button_compare")
		:setEnabled(false)
		:setTag(GameViewLayer.BT_COMPARE)

	--全压按钮
	self.btBetAll = self.nodeButtomButton:getChildByName("Button_allIn")
		:setEnabled(false)
		:setTag(GameViewLayer.BT_BETALL)
	
	--加注按钮
	self.btAddScore = self.nodeButtomButton:getChildByName("Button_addScore")
		:setEnabled(false)
		:setTag(GameViewLayer.BT_ADDSCORE)
	
	--跟注按钮
	self.btFollow = self.nodeButtomButton:getChildByName("Button_follow")
		:setEnabled(false)
		:setTag(GameViewLayer.BT_FOLLOW)

	self.followBetText = self.btFollow:getChildByName("FollowNum")
		:setVisible(true)

	
	
	self.btGiveUp:addTouchEventListener(btcallback)
	self.btLookCard:addTouchEventListener(btcallback)
	self.btCompare:addTouchEventListener(btcallback)
	self.btAddScore:addTouchEventListener(btcallback)
	self.btFollow:addTouchEventListener(btcallback)
	self.btBetAll:addTouchEventListener(btcallback)

	self.btGiveUp_bef:addTouchEventListener(btcallback)
	self.btLookCard_bef:addTouchEventListener(btcallback)
	self.btFollow_bef:addTouchEventListener(btcallback)
-------------------------**************************////////////////////////////////================================================================
	

	--玩家
	self.nodePlayer = {}
	--比牌判断区域
	self.rcCompare = { cc.rect(5,480,95,95), cc.rect(5,245,95,95) , cc.rect(1500,1500,0,0) , cc.rect(1225,245,95,95) , cc.rect(1225,480,95,95)}

	self.m_UserHead = {}

	self.m_clock = {}

	--手牌显示
	self.userCard = {}

	

	--时钟
	self.m_TimeProgress = {}
	self.m_userChat = {}

	for i = 1, cmd.GAME_PLAYER do
		--玩家总节点
		self.nodePlayer[i] = self.m_csbNode:getChildByName("UserNode_"..i)
							:setVisible(false)
							:setLocalZOrder(1)



		self.m_userChat[i] = self.nodePlayer[i]:getChildByName("Chat_view")
							:setVisible(false)
							
		
		self.m_UserHead[i] = {}
		--玩家背景
		self.m_UserHead[i].bg = self.nodePlayer[i]:getChildByName("Head_bg")

		self.m_clock[i] = self.nodePlayer[i]:getChildByName("Clock")
		self.m_clock[i]:setVisible(false)
		--计时器
		self.m_TimeProgress[i] = cc.ProgressTimer:create(display.newSprite("game_res/clock_pro.png"))
             :setReverseDirection(false)
             :move(cc.p(59,62))
             :setPercentage(100)
             :addTo(self.m_clock[i])
             :setLocalZOrder(1)

        self.m_clock[i]:getChildByName("Clock_bg")
        	:setLocalZOrder(2)

        self.m_clock[i]:getChildByName("Clock_num")
        	:setLocalZOrder(3)


        self.userCard[i] = {}
		self.userCard[i].card = {}

		--牌区域
		self.userCard[i].area = self.nodePlayer[i]:getChildByName("Card_node")
			:setVisible(false)

			--牌显示
		for j = 1, 3 do

			self.userCard[i].card[j] = display.newSprite("#card_back.png")
					:move( (i == cmd.MY_VIEWID and 70 or 37) + (i==cmd.MY_VIEWID and 140 or 73)*(j - 1), i == cmd.MY_VIEWID and 100 or 50)
					:setVisible(false)
					:setLocalZOrder(1)
					:addTo(self.userCard[i].area)
			self.userCard[i].card[j]:setScale(i == cmd.MY_VIEWID and 1.5 or 0.8)
			
		end

	end


	self.m_ChipNode = csbNode:getChildByName("AddScore_view")
	self.m_ChipNode:setVisible(false)
	self.m_ChipNode:setLocalZOrder(2)

	local closeB = self.m_ChipNode:getChildByName("Button_close")
	closeB:setTag(GameViewLayer.BT_CLOSECHIPBG)
	closeB:addTouchEventListener(btcallback)

	self.btChip = {}
	for i = 1, 4 do
		self.btChip[i] = self.m_ChipNode:getChildByName("Chip_"..i)
		self.btChip[i]:setTag(GameViewLayer.BT_CHIP + i)

		self.btChip[i]:addTouchEventListener(btcallback)
		local num = self.btChip[i]:getChildByName("Text")
		num:setString("0")
		num:setTag(GameViewLayer.CHIPNUM)
	end


	--房间规则详情按钮
	self.btRoomInfo = csbNode:getChildByName("Room_info")
	self.btRoomInfo:setTag(GameViewLayer.BT_ROOMINFO)
	self.btRoomInfo:setEnabled((PriRoom and GlobalUserItem.bPrivateRoom) and true or false)
	self.btRoomInfo:addTouchEventListener(btcallback)
		

	
	--定位按钮
	csbNode:getChildByName("Button_location")
		:setEnabled(true)
		:setTag(GameViewLayer.BT_LOCATION)
		:addTouchEventListener(btcallback)

	csbNode:getChildByName("Button_menu")
		:setEnabled(true)
		:setTag(GameViewLayer.BT_MENU)
		:addTouchEventListener(btcallback)



	self.m_AreaMenu = csbNode:getChildByName("Node_menu")
		:setVisible(false)
		:setLocalZOrder(20)

--牌型按钮
	self.m_AreaMenu:getChildByName("Button_cardType")
		:setTag(GameViewLayer.BT_CARDTYPE)
		:addTouchEventListener(btcallback)

	

	--设置按钮
	self.m_AreaMenu:getChildByName("Button_setting")
		:setTag(GameViewLayer.BT_SET)
		:addTouchEventListener(btcallback)

		--换桌
	self.btMenuChange = self.m_AreaMenu:getChildByName("Button_change")
	self.btMenuChange:setTag(GameViewLayer.BT_CHANGE)
	self.btMenuChange:setEnabled( (PriRoom and GlobalUserItem.bPrivateRoom) and false or true)
	self.btMenuChange:addTouchEventListener(btcallback)

	--退出按钮
	self.m_AreaMenu:getChildByName("Button_exit")
		:setTag(GameViewLayer.BT_EXIT)
		:addTouchEventListener(btcallback)


		--准备按钮
	self.btReady = csbNode:getChildByName("Button_ready")
	self.btReady:setVisible(false)
	self.btReady:setLocalZOrder(5)
	self.btReady:setTag(GameViewLayer.BT_READY)
	self.btReady:addTouchEventListener(btcallback)

	--换桌按钮
	self.btChange = csbNode:getChildByName("Button_change")
	self.btChange:setVisible(false)
	self.btChange:setLocalZOrder(5)
	self.btChange:setTag(GameViewLayer.BT_CHANGE)
	self.btChange:addTouchEventListener(btcallback)

	--坐下按钮
	self.btSit = csbNode:getChildByName("Button_sit")
	self.btSit:setVisible(false)
	self.btSit:setLocalZOrder(5)
	self.btSit:setTag(GameViewLayer.BT_SIT)
	self.btSit:addTouchEventListener(btcallback)

	--开始按钮
	self.btStart = csbNode:getChildByName("Button_start")
	self.btStart:setEnabled(true)
	self.btStart:setVisible(false)
	self.btStart:setLocalZOrder(5)
	self.btStart:setTag(GameViewLayer.BT_START)
	self.btStart:addTouchEventListener(btcallback)

		--牌型介绍
	
	self.cardTypeIntroduce = csbNode:getChildByName("Introduce")
		:setVisible(false)
		:setPositionY(15)
		:setLocalZOrder(20)


	local btnChat = csbNode:getChildByName("Button_chat")
		:setTag(GameViewLayer.BT_CHAT)
		:addTouchEventListener(btcallback)

	-- 语音按钮 gameviewlayer -> gamelayer -> clientscene
	local btnVoice = csbNode:getChildByName("Button_voice")
	btnVoice:addTouchEventListener(function(ref, eventType)
 		if eventType == ccui.TouchEventType.began then
 			if self._scene.m_bLookOn then
				showToast(self, "观战中不可使用该功能!", 1)
			else
 				self:getParentNode():getParentNode():startVoiceRecord()
 			end
        elseif eventType == ccui.TouchEventType.ended 
        	or eventType == ccui.TouchEventType.canceled then
        	if not self._scene.m_bLookOn then
        		self:getParentNode():getParentNode():stopVoiceRecord()
        	end
        end
	end)


	-- 电量,时间，信号
	local BatteryTemp = self.btRoomInfo:getChildByName("Battery_loading")
	local sp = cc.Sprite:create(cmd.RES.."game_res/battery.png")
	self.batteryBar = cc.ProgressTimer:create(sp)
	self.batteryBar:setType(cc.PROGRESS_TIMER_TYPE_BAR)
    self.batteryBar:setMidpoint(cc.p(0,0.5))
    self.batteryBar:setBarChangeRate(cc.p(1, 0))
    self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
    self.batteryBar:setPosition(BatteryTemp:getPosition())
    self.btRoomInfo:addChild(self.batteryBar)
    BatteryTemp:removeFromParent()

	self.btRoomInfo:getChildByName("Text_time"):setString(os.date("%H:%M"))

	if not (PriRoom and GlobalUserItem.bPrivateRoom) then
		self.btRoomInfo:getChildByName("Text_time"):setPositionY(45)
		self.batteryBar:setPositionY(45)
		self.btRoomInfo:getChildByName("Net_status"):setPositionY(45)
		self.btRoomInfo:getChildByName("Battery_bg"):setPositionY(45)
	end

    self.timeSchedule = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
		self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())

		self.btRoomInfo:getChildByName("Text_time"):setString(os.date("%H:%M"))

		local netType = MultiPlatform:getInstance():getNetworkType()
		local Strength = MultiPlatform:getInstance():getNetworkStrength()
		local TypePic = {"4G_","Wifi_"}
		if netType and Strength and netType>=1 and netType<=2 and Strength>=1 and Strength<=3 then
			local sp = cc.Sprite:create(cmd.RES.."game_res/"..TypePic[netType]..Strength..".png")
			self.btRoomInfo:getChildByName("Net_status"):setSpriteFrame(sp:getSpriteFrame())
		end
	end, 1, false)



	


end

function GameViewLayer:ctor(scene)
	local this = self

	local function onNodeEvent(event)  
       if "exit" == event then  
            this:onExit()  
        end  
    end  

    self:registerScriptHandler(onNodeEvent)  

	self._scene = scene

	self.nChip = {2,5,8,10}       --加注筹码倍数

	self.bCompareChoose = false   --进入比牌选择模式
	self.autoFollow = false       --当前是否跟到底

	--显示菜单
	self.m_bShowMenu = false--是否显示按钮菜单

	self.bIntroduce = false  --是否显示牌型介绍

	self.nAllTableScore = 0 --总分
	self.nChipsNum = 0	  --小筹码个数
	self.BigChipValue = 0 --转换玩的筹码数值

	self.txtConfig = string.getConfig("fonts/round_body.ttf" , 20)
	self.MytxtConfig = string.getConfig("fonts/round_body.ttf" , 24)

	

	display.loadSpriteFrames(cmd.RES.."Card.plist",cmd.RES.."Card.png")

	-- 语音动画
    AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)


-- 语音动画
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    param.m_strName = cmd.VOICE_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    self.m_actVoiceAni = cc.RepeatForever:create(animate)
    self.m_actVoiceAni:retain()
	
    self:initCsbRes()
	

	

	--点击事件
	local touch = display.newLayer()
		:setLocalZOrder(10)
		:addTo(self)
	touch:setTouchEnabled(true)
	touch:registerScriptTouchHandler(function(eventType, x, y)
		return this:onTouch(eventType, x, y)
	end)

	
	

	--聊天窗口层
	local tabconfig = {} 
	tabconfig.csbfile = cmd.RES.."chat/GameChatLayer.csb"
	self.m_GameChat = GameChat:create(scene._gameFrame ,tabconfig)
		:setLocalZOrder(50)
        :addTo(self)

    --设置层
	self._setLayer = SetLayer:create(self)
		:addTo(self, 10)
	--精简
	--初始化房间信息

    self:initRoomInfo()
    
end

function GameViewLayer:onUpdataClockTime(view , time)
	local num = self.m_clock[view]:getChildByName("Clock_num")
	num:setString(time)
end

function GameViewLayer:createGameEndView()
--普通结算层
	self.m_GameEndView	= GameEndView:create(self)
		:setVisible(false)
		:addTo(self, 20)
end

function GameViewLayer:setChipNum(chipArray)
	self.nChip=chipArray
end



--更新回合
function GameViewLayer:OnUpdataHuiHe(left,all)
	if all and all>0 then
		self.m_csbNode:getChildByName("Text_turn"):setString( "(第" .. left.."/"..all.."轮)")
	else
		self.m_csbNode:getChildByName("Text_turn"):setString( "(第" .. left.."轮)")
	end
end

function GameViewLayer:showAdressLayer()
--self.m_UserHead[viewid].head    self.m_UserHead[viewid].userInfo
	local addressLayer = cc.CSLoader:createNode(cmd.RES.."info/LocationLayer.csb"):addTo(self, 51)
	local panel1 = addressLayer:getChildByName("Panel_1")
	local panel2 = addressLayer:getChildByName("Panel_3")
	panel1:setVisible(false)
	panel2:setVisible(true)
	self._scene:showPopWait()

	for i=1,5 do
		if self.m_UserHead[i].head and self.m_UserHead[i].head:isVisible()  then
			self.m_UserHead[i].head:queryUserLociton(self.m_UserHead[i].userInfo.dwUserID)
			dump(self.m_UserHead[i].userInfo , "玩家视图"..i)
		end
	end



	self:runAction(cc.Sequence:create(cc.DelayTime:create(0.6),cc.CallFunc:create(function ()
		self._scene:dismissPopWait()
		panel1:setVisible(true)
		panel2:setVisible(true)

		 panel2:addTouchEventListener(function(ref)
        	addressLayer:removeFromParent()
    	end)

		 panel1:addTouchEventListener(function(ref)
        	addressLayer:removeFromParent()
    	end)
		
		for i=1,5 do

			if self.m_UserHead[i].head and self.m_UserHead[i].head:isVisible() then
				local address = panel1:getChildByName("address"..i)
				address:setVisible(true)
				dump(self.m_UserHead[i].userInfo ,"============================")
				local userInfo = self.m_UserHead[i].head.m_userInfo
				local ip  =""
				local la1  = 0
				local lo1  = 0 
				local placeName = "暂未获取到地址"
				if userInfo then
					-- ip地址
					local ipTable = ExternalFun.long2ip(userInfo.dwClientAddr)
					local r1 = ipTable.b
					local r2 = ipTable.s
					local r3 = ipTable.m
					local r4 = ipTable.p
					if nil == r1 or nil == r2 or nil == r3 or nil == r4 then
						ip = ""
					else
						ip = r3 .. "." .. r4 .. "." .. r2 .. "." .. r1
					end

					placeName = (userInfo.szPlaceName == "") and "暂未获取到地址" or userInfo.szPlaceName

					la1  = userInfo.dLatitude
					lo1 = userInfo.dLongitude
				end

				address:getChildByName("ip"):setString("IP:"..ip)
				address:setLocalZOrder(5)
				address:getChildByName("location"):setString(placeName)

				
				for j=i+1,5 do
					local la2 = 0
					local lo2 = 0
					local distanceTxt  = panel1:getChildByName("dis"..i.."_"..j)
					local distanceImg = panel1:getChildByName("xian"..i.."_"..j)
					if self.m_UserHead[j].head and self.m_UserHead[j].head:isVisible() then

						local userInfo = self.m_UserHead[i].head.m_userInfo
						if userInfo then
							la2  = userInfo.dLatitude
							lo2 = userInfo.dLongitude
						end
						distanceTxt:setVisible(true)
						if true then
							distanceImg:setVisible(true)
						else
							local sprite = cc.Sprite:createWithSpriteFrame(cc.SpriteFrame:create(cmd.RES.."info/xian1.png",cc.rect(0,0,distanceImg:getTag(),3)))
							sprite:setAnchorPoint(0,0.5)
							sprite:setRotation(distanceImg:getRotationSkewX())
							sprite:setPosition(distanceImg:getPosition())
							sprite:addTo(panel1)
							if i==2 and j== 4 then
								sprite:setScaleX(1.02)
							end
						end
					end
					--la1 = 30.5278142506
					--lo1 = 114.3181514740
					--la2 = 30.3892390220
					--lo2 = 112.9392814636
					if la1 ~= 0 and lo1 ~= 0 and la2 ~= 0 and lo2 ~= 0 then

						local function GetRadian(d)
							return d*3.1415926/180
						end

						local function GetRound(number)
							return number>0 and math.floor(number + 0.5) or math.ceil(number - 0.5)
						end

						local  radLat1 = GetRadian(la1)
						local  radLat2 = GetRadian(la2)
						local  a = radLat1 - radLat2
						local  b = GetRadian(lo1) - GetRadian(lo2)

						local dst = 2 * math.asin((math.sqrt(math.pow(math.sin(a / 2), 2) + math.cos(radLat1) * math.cos(radLat2) * math.pow(math.sin(b / 2), 2) )))

						dst = dst * 6371.393

						dst= math.abs(GetRound(dst * 10000000) / 10000)
						local str ="" 
						if dst >=1000 then
							local num1 = math.floor(dst%1000)
							local str2 =""
							if num1 >=100 then
								str2 = "."..num1
							elseif num1 >=10 then
								str2 = ".0"..num1
							else
								str2 = ".00"..num1
							end

							str = math.floor(dst/1000)..str2.."km"
						else
							str = dst.."m"
						end
						distanceTxt:setString(str)

					else
						distanceTxt:setString("无位置信息")
					end
				
				end
	
			end

			
		end
	end)))
	



end

--更新用户显示
function GameViewLayer:OnUpdateUserScore()
	
	for i=1 , 5 do
		if self.nodePlayer[i]:isVisible() then
			self.nodePlayer[i]:getChildByName("Score_bg")
				:getChildByName("User_score")
				:setString(self:getScoreString(self._scene.m_lUserMaxScore))
		end
	end 
end

--更新用户显示
function GameViewLayer:freshUserScore(viewid, score)
	
	for i=1 , cmd.GAME_PLAYER do
		local wViewChairId = self._scene:SwitchViewChairID(i-1)
		print( GlobalUserItem.bPrivateGoldRoom,GlobalUserItem.dwCurRoomGroupID,self._scene.m_lUserMaxScore,score,"---------------更新用户显示-------5555------")
		-- if self.nodePlayer[wViewChairId]:isVisible() and (GlobalUserItem.bPrivateGoldRoom or (GlobalUserItem.bPrivateGoldRoom and GlobalUserItem.dwCurRoomGroupID ~= 0)) then
			if self.nodePlayer[wViewChairId]:isVisible() then
			local userItem = self._scene.m_GameUserItem[i-1];
			-- dump(userItem,"-------------",6)
			-- print(score,viewid,i,"---------------更新用户显示-------5555------")
			if wViewChairId == viewid then
				if GlobalUserItem.bPrivateGoldRoom or (GlobalUserItem.bPrivateGoldRoom and GlobalUserItem.dwCurRoomGroupID ~= 0) then
					if userItem then
						self.nodePlayer[wViewChairId]:getChildByName("Score_bg")
						:getChildByName("User_score")
						:setString(self:getScoreString(userItem.lScore - score))
					end
					
				else
					self.nodePlayer[wViewChairId]:getChildByName("Score_bg")
					:getChildByName("User_score")
					:setString(self:getScoreString(self._scene.m_lUserMaxScore - score))
				end

				if userItem then

					
				end
			end
		end
	end 
end

--转换分数
function GameViewLayer:getScoreString( score )
    if type(score) ~= "number" then
        return ""
    end
    local strScore = ""
    if score > 100000000 then
        strScore = strScore..string.format("%.2f", score/100000000).."亿"
    elseif score > 10000 then
        --print("分数转换1", score/10000)
        strScore = strScore..string.format("%.2f", score/10000).."万"
    else
        --print("分数转换2", score/100000000)
        strScore = strScore..score
    end
    --print("分数转换", score, strScore)
    return strScore
end

--更新用户显示
function GameViewLayer:OnUpdateUser(viewid,userItem)
	dump(userItem , "更新用户显示")
	
	if not viewid or viewid == yl.INVALID_CHAIR then
		print("OnUpdateUser viewid is nil")
		return
	end
	print("OnUpdateUser viewid is==============> ", viewid)
	if (self._scene.isPriOver  and  GlobalUserItem.bPrivateRoom)  then
		return
	end
	self.nodePlayer[viewid]:setVisible(userItem ~= nil)
	local readyFlag = self.nodePlayer[viewid]:getChildByName("Ready_icon")
	local name = self.nodePlayer[viewid]:getChildByName("NickName")
	local score = self.nodePlayer[viewid]:getChildByName("Score_bg"):getChildByName("User_score")

	if not userItem then
		if self.m_UserHead[viewid].head then
			self.m_UserHead[viewid].head:setVisible(false)
		end
		name:setString("")
		score:setString("")
		readyFlag:setVisible(false)
		self.m_UserHead[viewid].userInfo = nil
	else
		self._scene.m_GameUserItem[userItem.wChairID] = userItem
		self.m_UserHead[viewid].userInfo = userItem
		self.nodePlayer[viewid]:setVisible(true)
		
		name:setString(string.EllipsisByConfig(userItem.szNickName,viewid == cmd.MY_VIEWID and 150 or 105,viewid == cmd.MY_VIEWID and self.MytxtConfig or self.txtConfig))
		score:setString(self:getScoreString(userItem.lScore))
		
		--判断积分房
        if PriRoom and GlobalUserItem.bPrivateRoom  then
            if PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
                if  PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
                	print("OnUpdateUser score ===================>", userItem.lScore, PriRoom:getInstance().m_tabPriData.lIniScore)
                    self:OnUpdateUserScore()
                end
            -- else
            --     self.m_UserHead[viewid].score:setString("0")
            end
            if GlobalUserItem.bVideo then
            	score:setString("0")
            end
        end
		readyFlag:setVisible(yl.US_READY == userItem.cbUserStatus)

		if yl.US_READY == userItem.cbUserStatus and viewid == cmd.MY_VIEWID then
			self.btReady:setVisible(false);--++
			self._scene:KillGameClock()
		end

		if not self.m_UserHead[viewid].head then
			self.m_UserHead[viewid].head = PopupInfoHead:createNormal(userItem, 72)
		    local param = {}
		    param.isProp = true     --显示道具互动，百人游戏不显示
		    param.imgPath = "game/popBg.png" --弹出框背景
		    self.m_UserHead[viewid].head:setPropInfo(param)
			self.m_UserHead[viewid].head:move(51, 51)
			self.m_UserHead[viewid].head:enableInfoPop(true, ptUserInfo[viewid], anchorPoint[viewid])
			self.m_UserHead[viewid].head:addTo(self.m_UserHead[viewid].bg)
		else
			self.m_UserHead[viewid].head:updateHead(userItem)
		end

		self.m_UserHead[viewid].head:enableInfoPop(not self._scene.m_bLookOn, ptUserInfo[viewid], anchorPoint[viewid])

		self.m_UserHead[viewid].head:setVisible(true)

		self:updataUserInfo()

		--判断房主
		--print("设置房主信息", PriRoom, userItem.dwUserID, PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID)
		if PriRoom and GlobalUserItem.bPrivateRoom and GlobalUserItem.bPrivateGoldRoom==false then
			if userItem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then
				self:SetRoomHolder(viewid)
			end
		end
	end

end

--屏幕点击
function GameViewLayer:onTouch(eventType, x, y)

print(x, y,"xxxxxxxxxyyyyyyyyyy")
	if eventType == "began" then
		--牌型显示判断
		if self.bIntroduce == true then
			return true
		end

		if self.m_bShowMenu == true then
			local rc = self.m_AreaMenu:getBoundingBox()
			if rc then
				rc.x = rc.x + 667
				rc.y = rc.y + 375
				print(rc.x , rc.y , "rc boundingBox X y")
				if not cc.rectContainsPoint(rc,cc.p(x,y)) then
					self:ShowMenu(false)
					return true
				end
			end
		end

		--比牌选择判断
		if self.bCompareChoose == true then
			for i = 1, cmd.GAME_PLAYER do
				if cc.rectContainsPoint(self.rcCompare[i],cc.p(x,y)) then
					return true
				end
			end
		end

		

		return false
	elseif eventType == "ended" then
		--取消牌型显示
		if self.bIntroduce == true then
			local rectIntroduce = self.cardTypeIntroduce:getBoundingBox()
			rectIntroduce.x = rectIntroduce.x + 667
			rectIntroduce.y = rectIntroduce.y + 375
			if rectIntroduce and not cc.rectContainsPoint(rectIntroduce, cc.p(x, y)) then
				self:OnShowIntroduce(false)
			end
		end

		--比牌选择
		if self.bCompareChoose == true then
			for i = 1, cmd.GAME_PLAYER do
				if cc.rectContainsPoint(self.rcCompare[i],cc.p(x,y)) then
					self._scene:OnCompareChoose(i)
					break
				end
			end
		end
	end

	return true
end

--牌类型介绍的弹出与弹入
function GameViewLayer:OnShowIntroduce(bShow)
	if self.bIntroduce == bShow then
		return
	end

	local point
	if bShow then
		point = cc.p(-525, 15) 			--移入的位置
	else
		point = cc.p(-810, 15)			--移出的位置
	end
	self.bIntroduce = bShow
	self.cardTypeIntroduce:stopAllActions()
	if bShow == true then
		self.cardTypeIntroduce:setVisible(true)
		self:ShowMenu(false)
	end
	local this = self
	self.cardTypeIntroduce:runAction(cc.Sequence:create(
		cc.MoveTo:create(0.3, point), 
		cc.CallFunc:create(function()
				this.cardTypeIntroduce:setVisible(this.bIntroduce)
			end)
		))

end

-- function GameViewLayer:createJettonByBet(wViewChairId , jettonCount ,jettonValue , notAni )
-- 	local nodeChipPool = self.m_csbNode:getChildByName("Chip_pool")
-- 	nodeChipPool:setLocalZOrder(10)
-- 	local chipscore = num
-- 	local imageIndex = 1
-- 	for i=1 , 4 do
-- 		if jettonValue==self.nChip[i]*self.m_lCellScore then
-- 			imageIndex = i
-- 			break;
-- 		end
-- 	end

-- 	for i = 1 , jettonCount do
-- 		local chip = display.newSprite("game_res/bigChip_"..imageIndex..".png")
-- 			:setScale(0.5)
-- 			:addTo(nodeChipPool)
-- 			self.nChipsNum = self.nChipsNum +1	  --小筹码个数
-- 			chip:setTag(1000 *imageIndex+ self.nChipsNum)
	
-- 		cc.Label:createWithTTF(jettonValue, "fonts/round_body.ttf", 18)
-- 			:move(54, 53)
-- 			:setColor(cc.c3b(48, 48, 48))
-- 			:addTo(chip)

-- 		if notani == true then
-- 			if wViewChairId < 3 then	
-- 				chip:move( cc.p(30+ math.random(150), 30 + math.random(210)))
-- 			elseif wViewChairId > 3 then
-- 				chip:move(cc.p(360+ math.random(160), 30 + math.random(210)))
-- 			else
-- 				chip:move(cc.p(180+ math.random(180), 30 + math.random(130)))
-- 			end
-- 		else
-- 			local coinPos = {cc.p(-255,140),cc.p(-255,-100),cc.p(-260,-235),cc.p(785,-100),cc.p(785,140)}
-- 			chip:move(coinPos[wViewChairId])
-- 			if wViewChairId < 3 then	
-- 				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(30+ math.random(150), 30 + math.random(210))), callFun))
-- 			elseif wViewChairId > 3 then
-- 				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(360+ math.random(160), 30 + math.random(210))), callFun))
-- 			else
-- 				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(180+ math.random(180), 30 + math.random(150))), callFun))
-- 			end

-- 		end
-- 	end


-- end

-- function GameViewLayer:createJettonByAllIn(wViewChairId, num,notAni)
-- 	local nodeChipPool = self.m_csbNode:getChildByName("Chip_pool")
-- 	nodeChipPool:setLocalZOrder(10)
-- 	local chipscore = num
-- end

-- --筹码移动
-- function GameViewLayer:PlayerJetton(wViewChairId, num,notani)
-- 	print(wViewChairId, num,notani ,self.m_lCellScore)
-- 	--3  89999999  nil  20  self.m_lCellScore
-- 	if not num or num < 1 or not self.m_lCellScore or self.m_lCellScore < 1 then
-- 		return
-- 	end

-- 	for i = 1 , 8 do
-- 		if num == self.m_lCellScore*self.nChip[i]*math.ceil(i/4) then
-- 			self:createJettonByBet(wViewChairId, math.ceil(i/4) ,self.m_lCellScore*self.nChip[i],notani)
-- 			return;
-- 		end
-- 	end
-- 	self:createJettonByAllIn(wViewChairId, num,notani)
	
-- 	if not notAni then
-- 		self._scene:PlaySound(cmd.RES.."sound_res/ADD_SCORE.wav")
-- 	end
	
-- 	local maxChip =  math.min( 1000 , self.m_lCellScore)

-- 	if num>=50000 then
-- 		maxChip = math.modf(num /  64)
-- 	end
	
-- 	while chipscore > 0 
-- 	do
-- 		if 0 == chipscore then
-- 			break
-- 		end
-- 		--print("创建！！",chipscore,self.nChipsNum,self.m_lCellScore)
-- 		local strChip
-- 		local strScore 
-- 		local ChipsType = 0
-- 		if chipscore >= maxChip*5 then
-- 			strChip = "game_res/bigChip_4.png"
-- 			chipscore = chipscore - maxChip*5
-- 			strScore = (maxChip*5)..""
-- 			ChipsType = 1
-- 		elseif chipscore >= maxChip*2 then
-- 			strChip = "game_res/bigChip_3.png"
-- 			chipscore = chipscore - maxChip*2
-- 			strScore = (maxChip*2)..""
-- 			ChipsType = 2
-- 		elseif chipscore >= maxChip then
-- 			strChip = "game_res/bigChip_2.png"
-- 			chipscore = chipscore - maxChip
-- 			strScore = (maxChip)..""
-- 			ChipsType = 3
-- 		else
-- 			strChip = "game_res/bigChip_1.png"
-- 			chipscore = chipscore - 1
-- 			strScore = "1"
-- 			ChipsType = 4
-- 		end

-- 		local chip = display.newSprite(strChip)
-- 			:setScale(0.5)
-- 			:addTo(nodeChipPool)
-- 			self.nChipsNum = self.nChipsNum +1	  --小筹码个数
-- 			chip:setTag(1000 *ChipsType+ self.nChipsNum)
	
-- 		cc.Label:createWithTTF(strScore, "fonts/round_body.ttf", 18)
-- 			:move(54, 53)--393 , 372
-- 			:setColor(cc.c3b(48, 48, 48))
-- 			:addTo(chip)
-- 		if notani == true then
-- 			if wViewChairId < 3 then	
-- 				chip:move( cc.p(30+ math.random(150), 30 + math.random(210)))
-- 			elseif wViewChairId > 3 then
-- 				chip:move(cc.p(360+ math.random(160), 30 + math.random(210)))
-- 			else
-- 				chip:move(cc.p(180+ math.random(180), 30 + math.random(130)))
-- 			end
-- 		else
-- 			--print("刷新筹码，筹码分值", chipscore)
-- 			local coinPos = {cc.p(-255,140),cc.p(-255,-100),cc.p(-260,-235),cc.p(785,-100),cc.p(785,140)}
-- 			chip:move(coinPos[wViewChairId])
-- 			function callbackWithArgs(chipscore)
-- 	              local ret = function ()
-- 	              	if 0 == chipscore then --最后的筹码动作完成，做合成大筹码动作
-- 						--获取大筹码个数
-- 						nodeChipPool:setLocalZOrder(0)
-- 						local children = nodeChipPool:getChildren()
-- 						local type1Num = 0
-- 						local type2Num = 0
-- 						local type3Num = 0
-- 						local type4Num = 0
-- 						for k, v in pairs(children) do
-- 							if v:getTag() > 1000 and v:getTag() < 2000 then
-- 								type1Num = type1Num +1
-- 							end
-- 							if v:getTag() > 2000 and v:getTag() < 3000 then
-- 								type2Num = type2Num +1
-- 							end
-- 							if v:getTag() > 3000 and v:getTag() < 4000 then
-- 								type3Num = type3Num +1
-- 							end
-- 							if v:getTag() > 4000 then
-- 								type4Num = type4Num +1
-- 							end
-- 						end
-- 						--不同档次到达指定额度，都会刷新(多加2个，防止操用户正在下注的筹码)
-- 						if type1Num > 10 then
-- 							print("刷新游戏币池")
-- 							self:ChangeAllJettons(1, 10,num)
-- 						end

-- 						if type2Num > 5 then
-- 							print("刷新游戏币池")
-- 							self:ChangeAllJettons(2, 5,num)
-- 						end

-- 						if type3Num > 10 then
-- 							print("刷新游戏币池")
-- 							self:ChangeAllJettons(3, 10,num)
-- 						end

-- 						if type4Num > 30 then
-- 							print("刷新游戏币池")
-- 							self:ChangeAllJettons(4, 30,num)
-- 						end
-- 					end
-- 	              end
-- 	              return ret
-- 	        end
-- 	        local callFun = cc.CallFunc:create(callbackWithArgs(chipscore))
	
-- 			if wViewChairId < 3 then	
-- 				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(30+ math.random(150), 30 + math.random(210))), callFun))
-- 			elseif wViewChairId > 3 then
-- 				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(360+ math.random(160), 30 + math.random(210))), callFun))
-- 			else
-- 				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(180+ math.random(180), 30 + math.random(150))), callFun))
-- 	end
-- 			end
-- 		end

-- 	if not notani then
-- 		self._scene:PlaySound(cmd.RES.."sound_res/ADD_SCORE.wav")
-- 	end
-- end

--筹码移动
function GameViewLayer:PlayerJetton(wViewChairId, num,notani)
	print(wViewChairId, num,notani ,self.m_lCellScore)
	--3  89999999  nil  20  self.m_lCellScore
	if not num or num < 1 or not self.m_lCellScore or self.m_lCellScore < 1 then
		return
	end
	local nodeChipPool = self.m_csbNode:getChildByName("Chip_pool")
	nodeChipPool:setLocalZOrder(10)
	local chipscore = num
	local maxChip =  num

	if num > self.m_lCellScore*self.nChip[4]*20 then
		maxChip = math.modf(num /  20)
	end
	
	while chipscore > 0 
	do
		if 0 == chipscore then
			break
		end
		--print("创建！！",chipscore,self.nChipsNum,self.m_lCellScore)
		local strChip
		local strScore 
		local ChipsType = 0
		if chipscore >= maxChip*5 then
			strChip = "game_res/bigChip_4.png"
			chipscore = chipscore - maxChip*5
			strScore = (maxChip*5)..""
			ChipsType = 1
		elseif chipscore >= maxChip*2 then
			strChip = "game_res/bigChip_3.png"
			chipscore = chipscore - maxChip*2
			strScore = (maxChip*2)..""
			ChipsType = 2
		elseif chipscore >= maxChip then
			strChip = "game_res/bigChip_2.png"
			chipscore = chipscore - maxChip
			strScore = (maxChip)..""
			ChipsType = 3
		else
			strChip = "game_res/bigChip_1.png"
			chipscore = chipscore - 1
			strScore = "1"
			ChipsType = 4
		end

		local chip = display.newSprite(strChip)
			:setScale(0.5)
			:addTo(nodeChipPool)
			self.nChipsNum = self.nChipsNum +1	  --小筹码个数
			chip:setTag(1000 *ChipsType+ self.nChipsNum)
	
		cc.Label:createWithTTF(strScore, "fonts/round_body.ttf", 18)
			:move(54, 53)--393 , 372
			:setColor(cc.c3b(48, 48, 48))
			:addTo(chip)
		if notani == true then
			if wViewChairId < 3 then	
				chip:move( cc.p(30+ math.random(150), 30 + math.random(210)))
			elseif wViewChairId > 3 then
				chip:move(cc.p(360+ math.random(160), 30 + math.random(210)))
			else
				chip:move(cc.p(180+ math.random(180), 30 + math.random(130)))
			end
		else
			--print("刷新筹码，筹码分值", chipscore)
			local coinPos = {cc.p(-255,140),cc.p(-255,-100),cc.p(-260,-235),cc.p(785,-100),cc.p(785,140)}
			chip:move(coinPos[wViewChairId])
			function callbackWithArgs(chipscore)
	              local ret = function ()
	              	if 0 == chipscore then --最后的筹码动作完成，做合成大筹码动作
						--获取大筹码个数
						nodeChipPool:setLocalZOrder(0)
						local children = nodeChipPool:getChildren()
						local type1Num = 0
						local type2Num = 0
						local type3Num = 0
						local type4Num = 0
						for k, v in pairs(children) do
							if v:getTag() > 1000 and v:getTag() < 2000 then
								type1Num = type1Num +1
							end
							if v:getTag() > 2000 and v:getTag() < 3000 then
								type2Num = type2Num +1
							end
							if v:getTag() > 3000 and v:getTag() < 4000 then
								type3Num = type3Num +1
							end
							if v:getTag() > 4000 then
								type4Num = type4Num +1
							end
						end
						--不同档次到达指定额度，都会刷新(多加2个，防止操用户正在下注的筹码)
						if type1Num > 13 then
							print("刷新游戏币池")
							--self:ChangeAllJettons(1, 10,maxChip)
						end

						if type2Num > 8 then
							print("刷新游戏币池")
							--self:ChangeAllJettons(2, 5,maxChip)
						end

						if type3Num > 13 then
							print("刷新游戏币池")
							--self:ChangeAllJettons(3, 10,maxChip)
						end

						if type4Num > 35 then
							print("刷新游戏币池")
							--self:ChangeAllJettons(4, 30,maxChip)
						end
					end
	              end
	              return ret
	        end
	        local callFun = cc.CallFunc:create(callbackWithArgs(chipscore))
	
			if wViewChairId < 3 then	
				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(30+ math.random(150), 30 + math.random(210))), callFun))
			elseif wViewChairId > 3 then
				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(360+ math.random(160), 30 + math.random(210))), callFun))
			else
				chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.2, cc.p(180+ math.random(180), 30 + math.random(150))), callFun))
	end
			end
		end

	if not notani then
		self._scene:PlaySound(cmd.RES.."sound_res/ADD_SCORE.wav")
	end
end



--比牌
function GameViewLayer:CompareCard(firstuser,seconduser,firstcard,secondcard,bfirstwin,callback)
	--比牌层
	self.m_CompareView = CompareView:create()
		:addTo(self, 21)
		self:runAction(
		cc.Sequence:create(
		cc.DelayTime:create(0.5),
		cc.CallFunc:create(
			function ()
				self.m_CompareView:CompareCard(firstuser,seconduser,firstcard,secondcard,bfirstwin,callback)
			end)))

	
end

--底注显示
function GameViewLayer:SetCellScore(cellscore)
	print("底注显示!!!")
	self.m_lCellScore = cellscore
	if not cellscore then
		for i = 1, 4 do
			self.btChip[i]:getChildByTag(GameViewLayer.CHIPNUM):setString("")
		end
	else
		for i = 1, 4 do
			self.btChip[i]:getChildByTag(GameViewLayer.CHIPNUM):setString(cellscore*self.nChip[i])
		end
	end
end

function GameViewLayer:SetCellText(cellscore)
	--Text_cellScore
	self.cellTextScore = cellscore
	local cellText = self.m_csbNode:getChildByName("Text_cellScore")
	if not cellscore then
		cellText:setString("底注：0")
	else
		cellText:setString("底注："..cellscore)
	end
end




--庄家显示
function GameViewLayer:SetBanker(viewid)
	if not viewid or viewid == yl.INVALID_CHAIR then
		return
	end
	for i=1 , cmd.GAME_PLAYER do
		local flag = self.nodePlayer[i]:getChildByName("Banker_icon")
		if viewid==i then
			flag:setVisible(true)
		else
			flag:setVisible(false)
		end
	end
end

--房主显示
function GameViewLayer:SetRoomHolder(viewid)
	if not viewid or viewid == yl.INVALID_CHAIR then
		return
	end
	for i=1 , cmd.GAME_PLAYER do
		local flag = self.nodePlayer[i]:getChildByName("Room_owner")
		if viewid==i and GlobalUserItem.dwCurRoomGroupID == 0 then
			flag:setVisible(true)
		else
			flag:setVisible(false)
		end
	end
end



--下注总额
function GameViewLayer:SetAllTableScore(score)
	if not score or score == 0 then
		self.m_txtAllScore:setString("")
	else
		self.nAllTableScore = score
		self.m_txtAllScore:setString("下注总额:"..score)
	end
	
end

--玩家下注
function GameViewLayer:SetUserTableScore(viewid, score)
	--增加桌上下注游戏币
	local scoreFrame = self.nodePlayer[viewid]:getChildByName("AddScore")
	local scoreTxt = scoreFrame:getChildByName("num")
	if not score or score == 0 then
		scoreFrame:setVisible(false)
		scoreTxt:setString("")
	else
		scoreFrame:setVisible(true)
		scoreTxt:setString(score)

		--刷新玩家分
		self:freshUserScore(viewid, score)
	end
end

--发牌
function GameViewLayer:SendCard(viewid,index,fDelay)
	if not viewid or viewid == yl.INVALID_CHAIR then
		return
	end
	local fInterval = 0.1

	local this = self
	local nodeCard = self.userCard[viewid]
	nodeCard.area:setVisible(true)

	local cardStartPos = {cc.p(555,193) , cc.p(555,425) , cc.p(208,570) , cc.p(-335,424), cc.p(-336,192)}
	local spriteCard = nodeCard.card[index]
	spriteCard:stopAllActions()
	spriteCard:setScale(1.0)
	spriteCard:setVisible(true)
	spriteCard:setSpriteFrame("card_back.png")
	spriteCard:move(cardStartPos[viewid])
	spriteCard:runAction(
		cc.Sequence:create(
		cc.DelayTime:create(fDelay),
		cc.CallFunc:create(
			function ()
				this._scene:PlaySound(cmd.RES.."sound_res/CENTER_SEND_CARD.wav")
			end
			),
			cc.Spawn:create(
				cc.ScaleTo:create(0.25,viewid==cmd.MY_VIEWID and 1.5 or 0.8),
				cc.MoveTo:create(0.25, cc.p(
					(viewid == cmd.MY_VIEWID and 70 or 37) + (viewid==cmd.MY_VIEWID and 140 or 73)*(index - 1), viewid == cmd.MY_VIEWID and 100 or 50))
				)
			)
		)
end

--玩家操作状态
function GameViewLayer:SetUserOperate(viewid , operate)--1已看牌  --2弃牌   3失败
	print(viewid , operate , "SetUserOperate")
	local operateIcon = self.nodePlayer[viewid]:getChildByName("Card_node"):getChildByName("Operate")
	operateIcon:setLocalZOrder(5)
	if not operate then
		operateIcon:setVisible(false)
		return
	end

	if viewid==cmd.MY_VIEWID and operate==1 and self._scene.m_bLookOn then 

	elseif viewid==cmd.MY_VIEWID and operate==1 then
		operateIcon:setVisible(false)
		return
	elseif viewid==cmd.MY_VIEWID then
		self:SetMyCardType(nil)
	end

	local sprite = display.newSprite("game_res/operate_"..operate..".png")
	operateIcon:setSpriteFrame(sprite:getSpriteFrame())
	operateIcon:setVisible(true)

	

end



--清理牌
function GameViewLayer:clearCard(viewid)
	local nodeCard = self.userCard[viewid]
	for i = 1, 3 do
		nodeCard.card[i]:setSpriteFrame("card_break.png")
		nodeCard.card[i]:setVisible(false)
	end
	
end

--显示牌值
function GameViewLayer:SetUserCard(viewid, cardData)
	if not viewid or viewid == yl.INVALID_CHAIR then
		return
	end
	--dump(cardData, "SetUserCard")
	print("SetUserCard", viewid)

	for i = 1, 3 do
		self.userCard[viewid].card[i]:stopAllActions()
		if viewid ~= cmd.MY_VIEWID then
			self.userCard[viewid].card[i]:setScale(0.8)
		else
			self.userCard[viewid].card[i]:setScale(1.5)
		end
		self.userCard[viewid].card[i]:move((viewid == cmd.MY_VIEWID and 70 or 37) + (viewid==cmd.MY_VIEWID and 140 or 73)*(i - 1), viewid == cmd.MY_VIEWID and 100 or 50)
	end
	--纹理
	if not cardData then
		for i = 1, 3 do
			self.userCard[viewid].card[i]:setSpriteFrame("card_back.png")
			self.userCard[viewid].card[i]:setVisible(false)
		end
	else
		for i = 1, 3 do
			local spCard = self.userCard[viewid].card[i]
			if not cardData[i] or cardData[i] == 0 or cardData[i] == 0xff  then
				spCard:setSpriteFrame("card_back.png")
			else
				local strCard = string.format("card_player_%02d.png",cardData[i])
				print("SetUserCard", strCard)
				spCard:setSpriteFrame(strCard)
			end
			self.userCard[viewid].card[i]:setVisible(true)
		end
	end
end

GameViewLayer.RES_CARD_TYPE = {"card_type_0.png","card_type_1.png","card_type_2.png","card_type_3.png","card_type_4.png","card_type_5.png"}
--显示牌类型
function GameViewLayer:SetMyCardType(cardtype)
	local myChair = self._scene:GetMeChairID()
	local spriteCardType = self.nodePlayer[cmd.MY_VIEWID]:getChildByName("Card_type")
	if cardtype and cardtype >= 1 and cardtype <= 6 and self._scene.m_cbPlayStatus[myChair+1]==1 then
		local sprite = display.newSprite("game_res/"..GameViewLayer.RES_CARD_TYPE[cardtype])
		spriteCardType:setSpriteFrame(sprite:getSpriteFrame())
		spriteCardType:setVisible(true)
	else
		spriteCardType:setVisible(false)
	end
end

--赢得筹码
function GameViewLayer:WinTheChip(wWinner)
	--筹码动作
	local headPos = {cc.p(-335 , 142) , cc.p(-336 , -90) , cc.p(-342 , -330) , cc.p(885 , -90) , cc.p(882 , 140)}
	local nodeChipPool = self.m_csbNode:getChildByName("Chip_pool")
	local children = nodeChipPool:getChildren()
	for k, v in pairs(children) do
		v:runAction(cc.Sequence:create(cc.DelayTime:create(0.1*(#children - k)),
			cc.MoveTo:create(0.4, cc.p(headPos[wWinner].x, headPos[wWinner].y )),
			cc.CallFunc:create(function(node)
				node:removeFromParent()
			end)))
	end
end

--清理筹码
function GameViewLayer:CleanAllJettons()
	self.m_csbNode:getChildByName("Chip_pool"):removeAllChildren()
end

--筹码转换
function GameViewLayer:ChangeAllJettons(type, num , allNum)
	-- body
	--绘制大额筹码
	local nodeChipPool = self.m_csbNode:getChildByName("Chip_pool")

	local strChip = "game_res/bigchip_gray.png"
	local maxChip =  math.min( 1000 , self.m_lCellScore)

	if allNum>=1000 then
		maxChip = math.modf(allNum /  64)
	end

	local numTab = {maxChip*50 , maxChip*10 , maxChip*10 , 30}
	local strScore = numTab[type]..""
	self.nChipsNum  = self.nChipsNum +1
	local chip = display.newSprite(strChip)
		:setScale(0.5)
		:addTo(nodeChipPool)
		:setTag(self.nChipsNum)
	cc.Label:createWithTTF(strScore, "fonts/round_body.ttf", 18)
		:move(54, 53)
		:setColor(cc.c3b(48, 48, 48))
		:addTo(chip)
	chip:setPosition( cc.p(30+ math.random(490), 30 + math.random(200)))
	local  pos = cc.p(chip:getPositionX(),chip:getPositionY())
	--print("重新绘制筹码", pos.x, pos.y)

	--移除十个大筹码
	local numMove = 0
	local children = nodeChipPool:getChildren()
	for k, v in pairs(children) do
		if v:getTag() > type* 1000 and v:getTag() < (type+1)* 1000 then
			--print("重新绘制筹码1", pos.x, pos.y)
			v:runAction(cc.Sequence:create(cc.MoveTo:create(0.4, pos),
				cc.CallFunc:create(function(node)
					node:removeFromParent()
				end)))
			numMove = numMove +1
			if numMove > num -1 then
				break
			end
		end
	end
end

--取消比牌选择
function GameViewLayer:SetCompareCard(bchoose,status)
	print(bchoose)
	dump(status,"status")
	self.bCompareChoose = bchoose
	
    for i = 1, cmd.GAME_PLAYER do
    	local arrow = self.nodePlayer[i]:getChildByName("Compare_flag")
    	
    	if bchoose and status and status[i] and arrow then
    	 	arrow:setVisible(true)
    	 	arrow:runAction(cc.RepeatForever:create(cc.Sequence:create(
    	 		cc.ScaleTo:create(0.3,0.7),
    	 		cc.ScaleTo:create(0.3,1.0)
    	 		)))
    	elseif arrow then
    		arrow:stopAllActions()
    	 	arrow:setVisible(false)
    	end 
        
    end
end

--按键响应
function GameViewLayer:OnButtonClickedEvent(tag,ref)
	if tag == GameViewLayer.BT_EXIT then
		self._scene:onQueryExitGame()
	elseif tag == GameViewLayer.BT_READY then
		-- if self._scene.m_bNoScore then--
  --           local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
  --           local msg = self._scene.m_szScoreMsg or "你的游戏币不足，无法继续游戏"
  --           local query = QueryDialog:create(msg, function(ok)
  --               if ok == true then
  --                   self._scene:onExitTable()
  --               end
  --           query = nil
  --           end, 32, QueryDialog.QUERY_SURE):setCanTouchOutside(false)
  --           :addTo(self._scene)
  --       else
            self._scene:onStartGame(true)
        -- end
	elseif tag == GameViewLayer.BT_GIVEUP then
		self._scene:onGiveUp()
	elseif tag == GameViewLayer.BT_LOOKCARD then
		self._scene:onLookCard()
	elseif tag == GameViewLayer.BT_FOLLOW_BEF then
		self.autoFollow = not self.autoFollow
		self.m_befFollowIcon:setVisible(self.autoFollow)
	elseif tag == GameViewLayer.BT_ADDSCORE then
		self.nodeButtomButton:setVisible(false)
		self.m_ChipNode:setVisible(true)
	elseif tag == GameViewLayer.BT_COMPARE then
		self._scene:onCompareCard()
	elseif tag == GameViewLayer.BT_CARDTYPE then
		print("点击牌型介绍")
		self:OnShowIntroduce(true)
	elseif tag == GameViewLayer.BT_FOLLOW then
		self._scene:addScore(0,true)
	elseif tag==GameViewLayer.BT_BETALL then
		self._scene:betAll()
	elseif tag > GameViewLayer.BT_CHIP and tag <= GameViewLayer.BT_CHIP+4 then
		self._scene:addScore(self.nChip[tag-GameViewLayer.BT_CHIP])
	elseif tag == GameViewLayer.BT_CLOSECHIPBG then
		self.m_ChipNode:setVisible(false)
		self.nodeButtomButton:setVisible(true)
	elseif tag == GameViewLayer.BT_CHAT then
		if self._scene.m_bLookOn then
			showToast(self, "观战中不可使用该功能!", 1)
		else
			self.m_GameChat:showGameChat(true)
		end
		--self:ShowMenu(false)
	elseif tag == GameViewLayer.BT_MENU then
		self:ShowMenu(not self.m_bShowMenu)
	elseif tag == GameViewLayer.BT_HELP then
		--self._scene._scene:popHelpLayer2(cmd.KIND_ID, 0)
	elseif tag == GameViewLayer.BT_SET then
		print("点击设置")
		self:ShowMenu(false)
		self._setLayer:showLayer()
	elseif tag == GameViewLayer.BT_BANK then
		showToast(self, "该功能尚未开放，敬请期待...", 1)
	elseif tag == GameViewLayer.BT_ROOMINFO then
		self:showRoomInfo(true)
	elseif tag == GameViewLayer.BT_LOCATION then
		if self._scene.m_bLookOn then
			showToast(self, "观战中不可使用该功能!", 1)
		else
			self:showAdressLayer()
		end
	elseif tag == GameViewLayer.BT_CHANGE then
		self:ShowMenu(false)
		self._scene:onChangeDesk()
		self.btReady:setVisible(false)
		self.btChange:setVisible(false)

	elseif GameViewLayer.BT_SIT == tag then
		self._scene._gameFrame.bChangeDesk = true
		self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)
	elseif GameViewLayer.BT_START == tag then
		self.btStart:setVisible(false)
		self._scene._gameFrame:SendUserBegin()
	end
end
--self.btSit
--self.btStart

function GameViewLayer:showSitBtn(visible,time)
	if not self._scene.m_bLookOn then
		if visible then
			if not self._beginTimeId and time then
		                 self.m_beginTimeNum = time
		                 self.btStart:getChildByName("Text_1"):setString(""..self.m_beginTimeNum)
			    self._beginTimeId = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
			    	self:updateBeginTime()
			    end, 1, false)		
			end
		else
			if self._beginTimeId then
				cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
				self._beginTimeId = nil
			end			
		end	
	end
	self.btSit:setVisible(visible and self._scene.m_bLookOn)
	self.btStart:setVisible(visible and not self._scene.m_bLookOn)
	if visible and not self._scene.m_bLookOn then
		self.btReady:setVisible(false);
	end
end


function GameViewLayer:updateBeginTime()
	if self.m_beginTimeNum and self.m_beginTimeNum >0 then 
		self.m_beginTimeNum = self.m_beginTimeNum -1
		self.btStart:getChildByName("Text_1"):setString(""..self.m_beginTimeNum)	
	else
		if self._beginTimeId then
			cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
			self._beginTimeId = nil
		end
	end

end

function GameViewLayer:ShowMenu(bShow)
	local this = self
	if self.m_bShowMenu ~= bShow then
		self.m_bShowMenu = bShow
		if self.m_bShowMenu == true and not self.m_AreaMenu:isVisible() then
			self.m_AreaMenu:setVisible(true)
		elseif self.m_AreaMenu:isVisible() == true then
			self.m_AreaMenu:setVisible(false)
		end
	end
end



function GameViewLayer:runAddTimesAnimate(viewid)
	display.newSprite("game_res/ani_addScore.png")
		:move(viewid == cmd.MY_VIEWID and cc.p(210,220) or cc.p(100,80))
		:setScale(viewid == cmd.MY_VIEWID and 1.3 or 1)
		:setLocalZOrder(10)
		:addTo(self.userCard[viewid].area)
		:runAction(cc.Sequence:create(
						cc.DelayTime:create(2),
						cc.CallFunc:create(function(ref)
							ref:removeFromParent()
						end)
						))
end

--显示聊天
function GameViewLayer:ShowUserChat(viewid ,message)
	if message and #message > 0 then
		if viewid==cmd.MY_VIEWID then
			self.m_GameChat:showGameChat(false)
		end 
			
		self.m_userChat[viewid]:stopAllActions()
		self.m_userChat[viewid]:removeAllChildren()
		self.m_userChat[viewid]:setVisible(true)

		local chatTxt = cc.LabelTTF:create(message, "fonts/round_body.ttf", 20, cc.size(150,0), cc.TEXT_ALIGNMENT_CENTER)
		chatTxt:addTo(self.m_userChat[viewid])

		local labSize = chatTxt:getContentSize()
        self.m_userChat[viewid]:setContentSize(labSize.width+54, labSize.height + 54)

    	chatTxt:setPosition(cc.p(self.m_userChat[viewid]:getContentSize().width/2+10 , self.m_userChat[viewid]:getContentSize().height/2))
    	if viewid>=4 then
    		chatTxt:setPositionX(self.m_userChat[viewid]:getContentSize().width/2-3)
    	end

		--动作
		self.m_userChat[viewid]:runAction(cc.Sequence:create(
						cc.DelayTime:create(3),
						cc.CallFunc:create(function()
							self.m_userChat[viewid]:removeAllChildren()
							self.m_userChat[viewid]:setVisible(false)
						end)
				))
	end
end

--显示表情
function GameViewLayer:ShowUserExpression(viewid,index)
	if viewid==cmd.MY_VIEWID then
		self.m_GameChat:showGameChat(false)
	end
	--取消上次
	self.m_userChat[viewid]:stopAllActions()
	self.m_userChat[viewid]:removeAllChildren()
	self.m_userChat[viewid]:setVisible(true)

	----********************
	cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", index))
    --缓存表情
      --加载动画
    local frames = {}
    local actionTime = 0.2
    for i=1,32 do
        local strName = string.format("emoj%d_%d.png",index, i)
        print("browChatSprName--------------------->", strName)
        local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
        if spriteFrame then
            table.insert(frames, spriteFrame)
        else
            break
        end
    end

    local  animation = cc.Animation:createWithSpriteFrames(frames,actionTime)    
    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", index))
    local expression = cc.Sprite:createWithSpriteFrame(spriteFrame)
    
    self.m_userChat[viewid]:addChild(expression)
    self.m_userChat[viewid]:setContentSize(140, 140)
    expression:setPosition(cc.p(75,75))

    expression:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    self.m_userChat[viewid]:runAction(cc.Sequence:create(cc.DelayTime:create(3.0) ,cc.CallFunc:create(function()
        
        self.m_userChat[viewid]:removeAllChildren()
		self.m_userChat[viewid]:setVisible(false)
    end)))
	
end

function GameViewLayer:priGameLayerZorder()
    return 10
end

--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
	--取消文字，表情
	self.m_userChat[viewid]:stopAllActions()
	self.m_userChat[viewid]:removeAllChildren()
	self.m_userChat[viewid]:setVisible(true)
	self.m_userChat[viewid]:setContentSize(199, 73)
	if isPlay == false then
		self.m_userChat[viewid]:setVisible(false)
	else
		--创建帧动画
	    -- 聊天表情
	    local sp = display.newSprite("game_res/zhajinhua_blank.png")

	    sp:setAnchorPoint(cc.p(0.5, 0.5))
		sp:runAction(self.m_actVoiceAni)
		sp:addTo(self.m_userChat[viewid])
		sp:setPosition(cc.p(100, 36))

		if viewid>3 then
			sp:setScaleX(1)
		else
			sp:setScaleX(-1)
		end
		

	end
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = 1
print("赠送道具信息",dwUserID,dwRecvUserID,dwPropID)
        --获取头像坐标
        dump(self.m_UserHead)
        local posHead = {cc.p(60,520) , cc.p(60,295) , cc.p(60,50) , cc.p(1280,295) , cc.p(1280,520)}
        for k,v in pairs(self.m_UserHead) do
              if v.userInfo then
                  -- local head = self.nodePlayer[k]:getChildByTag(GameViewLayer.SP_HEAD)
                  if v.userInfo.dwUserID ==  dwUserID then
                        beginPoint = posHead[k]
                  elseif v.userInfo.dwUserID ==  dwRecvUserID then
                        endPoint = posHead[k]
                        zOrder = v.head:getZOrder() + 1
                  end
              end
        end
        print("beginPoint ," , beginPoint.x , beginPoint.y)
        print("endPoint ," , endPoint.x , endPoint.y)
      if beginPoint and endPoint then
              local actSprite = cc.Sprite:create(string.format("public/prop_%d.png",dwPropID)):move(beginPoint):addTo(self,zOrder)
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

----------------------------------------------------------新大厅--------------------------------------------------
--初始化新大厅房间信息
function GameViewLayer:initRoomInfo()
    self.RoomInfoLayer = cc.CSLoader:createNode(cmd.RES.."info/Layer_info.csb"):addTo(self, GameViewLayer.ZORDER_SETTING)
    --self.RoomInfoLayer:setPosition(cc.p(0, 0))
    self.RoomInfoLayer:setVisible(false)
    print("初始化新大厅房间信息------------------------->", self.RoomInfoLayer)

    local PanelLayer = self.RoomInfoLayer:getChildByName("Panel")
    -- 遮罩
    PanelLayer:addTouchEventListener(function(ref)
        self.RoomInfoLayer:setVisible(false)
    end)

    


    local boxCallback = function(sender,eventType)

    	if eventType == ccui.TouchEventType.ended  then 
    		if self._scene.m_bLookOn then
				showToast(self.RoomInfoLayer, "观战中不可使用该功能!", 1)
				return
			end
    	
    		self.m_tabBoxGame[GameViewLayer.BT_RULE]:setEnabled(GameViewLayer.BT_SCORELIST == sender:getTag())
    		self.m_tabBoxGame[GameViewLayer.BT_SCORELIST]:setEnabled(GameViewLayer.BT_RULE == sender:getTag())
        	self.RoomInfoLayer:getChildByName("Node_rule"):setVisible(GameViewLayer.BT_RULE == sender:getTag())
        	self.RoomInfoLayer:getChildByName("Node_score"):setVisible(GameViewLayer.BT_SCORELIST == sender:getTag())
    		return
    	end
    end

    --按钮控制
    self.m_tabBoxGame = {}
    local checkbx = self.RoomInfoLayer:getChildByName("Btn_rule")
    checkbx:setTag(GameViewLayer.BT_RULE)
    checkbx:addTouchEventListener(boxCallback)
    self.m_tabBoxGame[GameViewLayer.BT_RULE] = checkbx

    checkbx = self.RoomInfoLayer:getChildByName("Btn_score")
    checkbx:setTag(GameViewLayer.BT_SCORELIST)
    checkbx:addTouchEventListener(boxCallback)
    self.m_tabBoxGame[GameViewLayer.BT_SCORELIST] = checkbx

    -- 房间信息界面
    --解散按钮
    local btnDismiss = self.RoomInfoLayer:getChildByName("Node_rule"):getChildByName("Node_score"):getChildByName("btn_dismiss")
    if nil ~= btnDismiss then
        btnDismiss:addTouchEventListener(function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                PriRoom:getInstance():queryDismissRoom(self._scene.m_cbGameStatus, self._scene:onGetSitUserNum())
                self.RoomInfoLayer:setVisible(false)
            end
        end)
    end
   
    
    --游戏流水界面
    -- 列表
    local tmpcontent = self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("ListView")
    local contentsize = tmpcontent:getContentSize()
    self.m_tabList = {}
    self._listView = cc.TableView:create(contentsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listView:setPosition(tmpcontent:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.RoomInfoLayer:getChildByName("Node_score"))
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()
    tmpcontent:removeFromParent()

    --self:updataUserInfo()
end

function GameViewLayer:updateRoomInfo(rule)
	
	local scoreNode = self.RoomInfoLayer:getChildByName("Node_rule"):getChildByName("Node_score")
	local goldNode = self.RoomInfoLayer:getChildByName("Node_rule"):getChildByName("Node_gold")

	if GlobalUserItem.bPrivateGoldRoom then
		goldNode:setVisible(true)
		scoreNode:setVisible(false)

		goldNode:getChildByName("rule_cellscore"):setString(rule.cellScore.."分")
		goldNode:getChildByName("rule_enter"):setString(PriRoom:getInstance().m_tabPriData.lEnterScore)
		goldNode:getChildByName("rule_exit"):setString(PriRoom:getInstance().m_tabPriData.lExitScore)
		goldNode:getChildByName("rule_maxtrun"):setString(rule.maxtrun .. "轮")
		goldNode:getChildByName("rule_minmeng"):setString(rule.minmeng .. "轮")
		goldNode:getChildByName("rule_compare"):setString(rule.compare .. "轮")
	else
		goldNode:setVisible(false)
		scoreNode:setVisible(true)

		scoreNode:getChildByName("rule_cellscore"):setString(rule.cellScore.."分")
		scoreNode:getChildByName("rule_playnum"):setString((rule.playnum==0 and "2-5" or rule.playnum) .."人")
		scoreNode:getChildByName("rule_count"):setString(PriRoom:getInstance().m_tabPriData.dwDrawCountLimit.."局")
		scoreNode:getChildByName("rule_score"):setString(rule.score .. "分")
		scoreNode:getChildByName("rule_maxtrun"):setString(rule.maxtrun .. "轮")
		scoreNode:getChildByName("rule_minmeng"):setString(rule.minmeng .. "轮")
		scoreNode:getChildByName("rule_compare"):setString(rule.compare .. "轮")
	end
	
end

function GameViewLayer:showRoomInfo(bIsShow)
	print("showRoomInfo", bIsShow)

	self.m_tabBoxGame[GameViewLayer.BT_RULE]:setEnabled(false)
    self.m_tabBoxGame[GameViewLayer.BT_SCORELIST]:setEnabled(true)
    self.RoomInfoLayer:getChildByName("Node_rule"):setVisible(true)
    self.RoomInfoLayer:getChildByName("Node_score"):setVisible(false)
    self.RoomInfoLayer:setVisible(bIsShow)



    
end

function GameViewLayer:updataUserInfo()
    local nodeScore = self.RoomInfoLayer:getChildByName("Node_score")
    local myChair = self._scene:GetMeChairID()
    print("updataUserInfo updataUserInfo updataUserInfo",GlobalUserItem.bPrivateGoldRoom)

    if GlobalUserItem.bPrivateGoldRoom then

    	for i=1,5 do
        	local labName = nodeScore:getChildByName("Text_name"..i)
        	if i==1 then
            	local userItem = self._scene:getUserInfoByChairID(myChair)
        		if nil ~= userItem then
            		local strNickname = string.EllipsisByConfig(userItem.szNickName, 90, 
                                		string.getConfig("fonts/round_body.ttf", 20))
            		labName:setString(strNickname)
        		end
            else
            	labName:setString("")
        	end
    	end


    	
    else
    	for i=1,5 do
        	local userItem = self._scene:getUserInfoByChairID(i -1)
        	local labName = nodeScore:getChildByName("Text_name"..i)
        	if nil ~= userItem then
            	local strNickname = string.EllipsisByConfig(userItem.szNickName, 90, 
                                	string.getConfig("fonts/round_body.ttf", 20))
            	labName:setString(strNickname)
            else
            	labName:setString("")
        	end
    	end
    end

    
end

function GameViewLayer:updataScoreList(scoreList)
    self.m_tabList = clone(scoreList)
    if 0 < #self.m_tabList then
        self._listView:reloadData()
    end
end

-- 子视图大小
function GameViewLayer:cellSizeForTable(view, idx)
    return 571, 61
end

function GameViewLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local iteminfo = self.m_tabList[idx + 1]
    if nil == iteminfo then
        return cell 
    end

    local csb = cc.CSLoader:createNode(cmd.RES.."info/Node_cell.csb")
    cell:addChild(csb)
    csb:setPosition(view:getViewSize().width * 0.5, 61 * 0.5)

    


    --局数
    local strRound = (idx + 1).."/"..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    if GlobalUserItem.bPrivateGoldRoom then
    	strRound = idx + 1
    end
    csb:getChildByName("Text_round"):setString(strRound)

    local myChair = self._scene:GetMeChairID()

    if GlobalUserItem.bPrivateGoldRoom then
    	local score = iteminfo[myChair+1]
        local userItem = self._scene:getUserInfoByChairID(myChair)
        if nil ~= userItem  then
	        local labScore = csb:getChildByName("Text_score1")
	        if nil ~= score then
	            labScore:setString(score.."")
	        end
	    end
    else
    	--分数
    	for i=1, 5 do
        	local score = iteminfo[i]
        	local userItem = self._scene:getUserInfoByChairID(i -1)
        	if nil ~= userItem  then
	        	local labScore = csb:getChildByName("Text_score"..i)
	        	if nil ~= score then
	            	labScore:setString(score.."")
	        	end
	    	end
    	end
    end

    
    return cell
end

-- 子视图数目
function GameViewLayer:numberOfCellsInTableView(view)
    return #self.m_tabList
end

return GameViewLayer