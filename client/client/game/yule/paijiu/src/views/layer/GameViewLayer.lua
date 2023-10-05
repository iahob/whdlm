local cmd = appdf.req(appdf.GAME_SRC.."yule.paijiu.src.models.CMD_Game")

local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local GameRoleItem=appdf.req(appdf.GAME_SRC.."yule.paijiu.src.views.layer.GameRoleItem")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.paijiu.src.models.GameLogic")

local GameChatLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local SettingLayer = appdf.req(appdf.GAME_SRC .. "yule.paijiu.src.views.layer.SettingLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local SpineObj = appdf.req("client.src.external.SpineObj")


local anchorPointHead = {cc.p(1, 1), cc.p(0, 0.5), cc.p(0, 0), cc.p(1, 0.5)}
local posReady = {cc.p(-333, 0), cc.p(135, 0), cc.p(516, -80), cc.p(-134, 0)}
local posPlate = {cc.p(667, 589), cc.p(237, 464), cc.p(667, 174), cc.p(1093, 455)}
local posChat = {cc.p(873, 660), cc.p(229, 558), cc.p(270, 285), cc.p(1095, 528)}

-- local posHead = {cc.p(492, 150), cc.p(1260, 304), cc.p(1260, 520), cc.p(492, 669),cc.p(72,520),cc.p(72,304)}
						  --100
local posHead = {cc.p(495, 106), cc.p(1247, 281), cc.p(1247, 510), cc.p(495, 675),cc.p(87,510),cc.p(87,281)}
local posRobSp= {}
local posCards={{},{},{},{},{},{}} --六个viewID的四张牌的位置
local posCardPoint={{},{},{},{},{},{}}
local posResultSp={{},{}}
local posScore={}
local posChip={}
local posReadySp={}
local posRect={{},{},{},{},{},{}}

local GameViewLayer = class("GameViewLayer",function(scene)
	return cc.CSLoader:createNode("game/GameView.csb")
end)

local Zorder_PriGameLayer=2

local Zorder_RectLayer=0
local Zorder_CardLayer=1
local Zorder_EndLayer=2
local Zorder_SetNode=99
local Zorder_Clock=2
local Zorder_Chat=90

local Zorder_SettingLayer=100
local Zorder_HelpLayer=110
local Zorder_ZhanjiLayer=120

local Zorder_Head= 10
local Zorder_Flycoin= 11

local TagEnum={}
TagEnum.BTN_READY=1

TagEnum.BTN_CHIP=10000

TagEnum.BTN_ROB =2
TagEnum.BTN_NOROB=3
TagEnum.BTN_NOTIFY=4
TagEnum.BTN_ZUPAI=5

TagEnum.BTN_SET=6
TagEnum.BTN_HELP=7
TagEnum.BTN_EXIT=8
TagEnum.BTN_CHAT=9

TagEnum.BT_AUTO = 105

TagEnum.BTN_TRUST=10

TagEnum.BTN_DISMISS=11

TagEnum.BTN_CANCEL=12

TagEnum.BTN_LOCATION =13

TagEnum.BTN_CHANGEDESK =14

local tag_flycoin =13017

local cardScale=0.6 --非自己牌缩小

GameViewLayer.TagEnum=TagEnum

local Tag_Dice=1111


 function GameViewLayer:resetData(bReadyBtn)
 	self:initData()

 	for i=1,2 do
 		local dice=self:getChildByTag(Tag_Dice+i)
 		if dice and not tolua.isnull(dice) then
 			dice:removeSelf()
 		end
 	end
 	for viewid=1,cmd.GAME_PLAYER do
 		 if self.tabHeadSp[viewid] and not tolua.isnull( self.tabHeadSp[viewid]) then
 		 	 if bReadyBtn then
 		 	 	self.tabHeadSp[viewid]:hideBankerFlag()
 		 	 elseif not GlobalUserItem.bVideo then
 		 	 	self.tabHeadSp[viewid]:removeSelf()
 		 	 end
 		 end
 	end
 	self.tabRobState={}

 	self.readyBtn:stopAllActions()

 	for i=1,cmd.GAME_PLAYER do
 		self:removeActionSps(i,bReadyBtn)
 	end

 	for i=1,cmd.GAME_PLAYER do
 		self:setShowZupaiFinishedSp(i,false)
 	end
 	self:stopAllActions()
 	if self:getChildByName("robRectBg") then
 		self:getChildByName("robRectBg"):removeSelf()
 	end


 	local flycoin=self:getChildByTag(tag_flycoin)
 	while(flycoin) do
 		flycoin:removeSelf()
 		flycoin=self:getChildByTag(tag_flycoin)
 	end
 end

 function GameViewLayer:initData()

 	if self.tableCards then
 		for i=1,32 do
 			if self.tableCards[i] and not tolua.isnull(self.tableCards[i]) then
 				self.tableCards[i]:removeSelf()
 			end
 		end
 	end

 	self:stopAllActions()
 	if self._cardLayer and not tolua.isnull(self._cardLayer) then
 		self._cardLayer:removeSelf()
 	end

 	if self._RectLayer and not tolua.isnull(self._RectLayer) then
 		self._RectLayer:removeSelf()
 	end

 	if self._EndLayer and not tolua.isnull(self._EndLayer) then
 		self._EndLayer:removeSelf()
 	end

 	if self._chipLayer and not tolua.isnull(self._chipLayer) then
 		self._chipLayer:removeSelf()
 	end

 	if self.tabRobSp then
 		for k,v in pairs(self.tabRobSp) do
 			if not tolua.isnull(v) then
 				v:removeSelf()
 			end
 		end
 	end
 	self.tabRobSp={}

 	self:showPoints()

 	self._cardLayer=cc.Layer:create():addTo(self,Zorder_CardLayer)

 	self.tabPlayStatus={cmd.PS_NONE,cmd.PS_NONE,cmd.PS_NONE,cmd.PS_NONE,cmd.PS_NONE,cmd.PS_NONE}

 	self.tabCardData=    {{},{},{},{},{},{}}
 	self.tabHandCardNode={{},{},{},{},{},{}}
 	self.bMyCardPoped={}
 	self.tempTabUserItem={}

 	self.bGameEnd=false
	self.bSendCardFinished=false

 end


function GameViewLayer:updateTableCards(cardN)
	local tableCardNode=self:getChildByName("tableCards")

	self.leftCard=cardN

	local w,h=43,73
	if not self.tableCardPos then
		self.tableCardPos={}
		local begX,begY= 480,570 --左上角
	 	for i=1,4 do
	 		for j=1,8 do
	 			local k=(i-1)*8+j
	 			local x=begX+j*w
	 			local y=begY-i*h
	 			self.tableCardPos[(i-1)*8+j]=cc.p(x,y)
	 		end
	 	end
	end

	self.tableCards=self.tableCards or {}
	for i=1,self.CARDN do
		if nil==self.tableCards[i] or tolua.isnull(self.tableCards[i]) then
			self.tableCards[i]=cc.Sprite:create("game/backvertical.png"):addTo(tableCardNode):setScale(w/70, h/140)
		end
		self.tableCards[i]:setPosition(self.tableCardPos[i]):setVisible(i<=self.leftCard)
	end

	for i=self.CARDN+1,36 do
		if self.tableCards[i] and not tolua.isnull(self.tableCards[i]) then
			self.tableCards[i]:removeSelf()
		end
	end
end

function GameViewLayer:resetHeadSpPos()
	for viewid,headSp in pairs(self.tabHeadSp) do
		if not tolua.isnull(headSp) then
			print("viewid:",viewid)
			dump(posHead[viewid],"posHead",6)
			print(posHead[viewid].x,posHead[viewid].y)
			assert(posHead[viewid].x>0)
			assert(posHead[viewid].y>0)
			headSp:setPosition(posHead[viewid])
		end
	end
end

function GameViewLayer:getDir(viewid)
	local bRight,bTop,bLeft=false
	if self._scene.wPlayerCount==4 then
		bRight=viewid==cmd.FOUR_RIGHT_VIEWID
		bTop=viewid==cmd.FOUR_TOP_VIEWID
		bLeft=viewid==cmd.FOUR_LEFT_VIEWID
	else
		bRight=viewid==cmd.RB_VIEWID or viewid==cmd.RT_VIEWID
		bTop=viewid==cmd.TOP_VIEWID
		bLeft=viewid==cmd.LB_VIEWID or viewid==cmd.LT_VIEWID
	end
	return bRight,bTop,bLeft
end

function GameViewLayer:initPos( )
	if self._scene.wPlayerCount==4 then
		posHead = {cc.p(492, 150), cc.p(1260, 412), cc.p(492, 669),cc.p(72,412),cc.p(-1000,-1000), cc.p(-1000, -1000)} --后两个无用
	end

	local w,h=70,140 --牌宽高
	local hs=90 --头像宽高
	
	for i=1, cmd.GAME_PLAYER do --viewid

		if i==cmd.MY_VIEWID then
			w,h=70,140
		else
			w,h=70*cardScale,140*cardScale
		end
		local bRight,bTop,bLeft=self:getDir(i)
		if bRight then 
			--posCards[i][4]=cc.p(posHead[i].x-94,posHead[i].y)
			--posCards[i][3]=cc.p(posCards[i][4].x-w+1,posCards[i][4].y)
			--posCards[i][1]=cc.p(posCards[i][3].x-h/2-w/2-7,posHead[i].y+30)
			--posCards[i][2]=cc.p(posCards[i][1].x,posCards[i][1].y-w-1)

			posCards[i][1]=cc.p(posHead[i].x-h/2-hs/2-w*2-20,posHead[i].y+20)
			if self._scene.cardNumLimit==2 then
				posCards[i][1]=cc.p(posHead[i].x-h/2-hs/2-20,posHead[i].y+17)
				--posCards[i][2]=cc.p(posCards[i][1].x,posCards[i][1].y-w-1)
			end

			posChip[i]=cc.p(posHead[i].x-hs/2-70  ,posHead[i].y+hs/2+10) --下注大小位置

			posRobSp[i]=cc.p(posHead[i].x-hs/2-40,posHead[i].y)  --抢弃图片位置

		else
			posCards[i][1]=cc.p(posHead[i].x+hs/2+h/2+15,posHead[i].y+(i==cmd.MY_VIEWID and 35 or 20))--左上角牌
			-- posCards[i][2]=cc.p(posCards[i][1].x,posCards[i][1].y-w-1)
			-- posCards[i][3]=cc.p(posCards[i][2].x+h/2+8+w/2,posCards[i][2].y+w/2-2)
			-- posCards[i][4]=cc.p(posCards[i][3].x+w+1,posCards[i][3].y)

			posChip[i]=cc.p(posHead[i].x+hs/2+70  ,posHead[i].y+hs/2+10) --下注大小位置
			if i==cmd.MY_VIEWID then
				posChip[i]=cc.p(posHead[i].x+hs/2+70  ,posHead[i].y+hs/2+60)
			elseif bTop then
				posChip[i]=cc.p(posHead[i].x+hs/2+70  ,posHead[i].y+hs/2-h-22)
			end

			posRobSp[i]=cc.p(posHead[i].x+hs/2+40,posHead[i].y)  --抢弃图片位置
			-- posReadySp[i]=cc.p(posHead[i].x+hs/2+25,posHead[i].y) --准备图片位置
		end

		posCards[i][2]=cc.p(posCards[i][1].x,posCards[i][1].y-w-1)
		posCards[i][3]=cc.p(posCards[i][2].x+h/2+6+w/2,posCards[i][2].y+w/2-2)
		posCards[i][4]=cc.p(posCards[i][3].x+w+1,posCards[i][3].y)


		posResultSp[1][i]=cc.p(posCards[i][1].x+h/2+2,posCards[i][1].y-w/2+6) --大牌九胜败图片位置
		posCardPoint[i][1]=cc.p(posCards[i][1].x,posCards[i][1].y-1.5*w-20+(i==cmd.MY_VIEWID and 3 or 0)) --点数1图片位置
		posCardPoint[i][2]=cc.p(posCards[i][3].x+w/2,posCards[i][3].y-h/2-(i==cmd.MY_VIEWID and 14 or 18)) --点数2图片位置


		posRect[i][1]=cc.p(posCards[i][1].x+2,posCards[i][3].y+2)
		posRect[i][2]=cc.p(posCards[i][3].x+w/2+1,posCards[i][3].y+2)

		posReadySp[i]=cc.p(posHead[i].x,posHead[i].y-hs/2-55) --准备图片位置
		if i==cmd.MY_VIEWID then
			posReadySp[i]=cc.p(posHead[i].x,posHead[i].y+hs/2+55)
		end

		posResultSp[2][i]=cc.p(posCards[i][1].x+3,posCards[i][1].y-w/2+5) --小牌九胜败图片位置

		--分数图片位置
		local addon=10
		if bRight then -- anchorpoint(1,0.5)
			posScore[i]=cc.p(posHead[i].x+hs/2,posHead[i].y+hs/2+addon)
		else  ---- anchorpoint(0,0.5)
			posScore[i]=cc.p(posHead[i].x-hs/2,posHead[i].y+hs/2+addon)
			if bTop then
				posScore[i].y=posScore[i].y-hs*1.7
			else
				posScore[i].y=posScore[i].y+30
			end
		end
	end

end

 function GameViewLayer:ctor(scene)
 	
 	self.name="GameViewLayer"
 	ExternalFun.registerNodeEvent(self)

 	self.CARDN=32

 	self.tabUserItem={}
 	self.tabUserItemCache={}
 	self.tabHeadSp={}
 	--self:initPos()
 
	self.bIsMaxBet = false

	self.cbTrusteeMode = 0

 	self._scene=scene
 	self:initData()
 	
 	local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(sender:getTag(), sender)
        end
    end


	self.trustBtn=self:getChildByName("btn_trustee")
	-- self.trustBtn:setTag(TagEnum.BTN_TRUST):addTouchEventListener(btnEvent)
	self.trustBtn:setVisible(false) --(not GlobalUserItem.bPrivateRoom)

 	self.readyBtn=self:getChildByName("btn_ready")
 	self.readyBtn:setTag(TagEnum.BTN_READY):addTouchEventListener(btnEvent)
 	self.readyBtn:setVisible(false)

 	self.btnChangeDesk=self:getChildByName("btn_ChangeDesk")
 	self.btnChangeDesk:setTag(TagEnum.BTN_CHANGEDESK):addTouchEventListener(btnEvent)
 	self.btnChangeDesk:setVisible(false)

 	self.robBtn=self:getChildByName("btn_qiang")
 	self.robBtn:setTag(TagEnum.BTN_ROB):addTouchEventListener(btnEvent)
 	self.noRobBtn=self:getChildByName("btn_buqiang")
 	self.noRobBtn:setTag(TagEnum.BTN_NOROB):addTouchEventListener(btnEvent)
 	self.chipNode=self:getChildByName("chipbtns")
 	
 	
 	self.chipBtns={}
 	for i=1,5 do --temp 5
 		self.chipBtns[i]=self.chipNode:getChildByName("chipbtn_"..i)
 		self.chipBtns[i]:setTag(TagEnum.BTN_CHIP+i):addTouchEventListener(btnEvent)
 	end

 	self.zupaiBtn=self:getChildByName("btn_zupai")
 	self.zupaiBtn:setTag(TagEnum.BTN_ZUPAI):addTouchEventListener(btnEvent)

 	self.notifyBtn=self:getChildByName("btn_notify")
 	self.notifyBtn:setTag(TagEnum.BTN_NOTIFY):addTouchEventListener(btnEvent)

 	self.cancelBtn=self:getChildByName("btn_cancel")
 	self.cancelBtn:setTag(TagEnum.BTN_CANCEL):addTouchEventListener(btnEvent)

 	--self:getChildByName("btn_menu"):addClickEventListener(function() self._scene:onExitTable() end) --temp

 	self.tabButtons={self.readyBtn,self.robBtn,self.noRobBtn,self.chipNode,self.zupaiBtn,self.notifyBtn,self.cancelBtn}


 	self.menuBtn=self:getChildByName("btn_menu")
 	self.menuBtn:loadTextureNormal("game/menudown.png")
 	local setNode=self:getChildByName("setnode")
 	setNode:setVisible(false):setLocalZOrder(Zorder_SetNode)
 	self.setNode=setNode

 	ExternalFun.registerTouchEvent(setNode)
 	setNode.onTouchBegan=function() return true end
 	setNode.onTouchEnded=function() 
	 	if setNode:isVisible() then 
	 		setNode:setVisible(false)
	 		self.menuBtn:loadTextureNormal("game/menudown.png")
	 	end
 	end

 	self.menuBtn:addClickEventListener(function()  
 		local b=setNode:isVisible()
 		self.menuBtn:loadTextureNormal( b and "game/menudown.png" or  "game/menuup.png")
 		setNode:setVisible(not b)
 		self:switchBtnChange()
 				--self:testPlayCoinAnimation(cc.p(900,600),cc.p(200,200) )--posHead[math.random(6)],posHead[math.random(6)])

 		end  )

 	local btn_set=setNode:getChildByName("btn_set")
 	btn_set:setTag(TagEnum.BTN_SET):addTouchEventListener(btnEvent)
 	btn_set:setPosition(cc.p(-11, -40))

 	local btn_help=setNode:getChildByName("btn_help")
 	btn_help:setTag(TagEnum.BTN_HELP):addTouchEventListener(btnEvent)
 	btn_help:setVisible(false)

 	local btn_exit=setNode:getChildByName("btn_exit")
 	btn_exit:setTag(TagEnum.BTN_EXIT):addTouchEventListener(btnEvent)
 	btn_exit:setPosition(cc.p(-11, -120))

 	self.btn_trust=setNode:getChildByName("btn_trustee")
 	self.btn_trust:setTag(TagEnum.BTN_TRUST):addTouchEventListener(btnEvent)
 	self.btn_trust:setPosition(cc.p(-11, -200))

 	self.btnChangeMenu = ccui.Button:create("game/btnChange.png","game/btnChange.png","game/btnChange.png")
 	self.btnChangeMenu:setTag(TagEnum.BTN_CHANGEDESK):addTouchEventListener(btnEvent)
 	self.btnChangeMenu:setPosition(cc.p(-11, -200))
 	setNode:addChild(self.btnChangeMenu)

 	local chatBtn=self:getChildByName("btn_chat") 
 	chatBtn:setTag(TagEnum.BTN_CHAT)
 	chatBtn:addTouchEventListener(btnEvent)

 	local tabconfig={}
 	tabconfig.csbfile = "game/GameChatLayer.csb"
 	tabconfig.szItemFrameName = "game/textline.png"
 	tabconfig.szItemSelectFrameName = "public/textbg.png"

 	self._chatLayer = GameChatLayer:create(self._scene._gameFrame,tabconfig):addTo(self, Zorder_Chat)	--聊天框
 	--self._chatLayer.m_spChatBg:setTexture("game/chatbg.png")

 	local btnVoice=self:getChildByName("btn_voice")
 	btnVoice:addTouchEventListener(function(ref, eventType)
 		if eventType == ccui.TouchEventType.began then
 			self._scene:getParentNode():startVoiceRecord()
        elseif eventType == ccui.TouchEventType.ended 
        	or eventType == ccui.TouchEventType.canceled then
        	self._scene:getParentNode():stopVoiceRecord()
        end
	end)

	 AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

	 self:initPos()
	 self.tabReadySp={}
	 for viewid=1,cmd.GAME_PLAYER do
	 	print(viewid,posReadySp[viewid])
	 	assert(posReadySp[viewid])
	 	self.tabReadySp[viewid]=cc.Sprite:create("game/readysp.png"):addTo(self):setPosition(posReadySp[viewid]):setVisible(false)
	 end

	 self.clock=cc.Sprite:create("game/clock.png"):setPosition(yl.WIDTH/2,yl.HEIGHT/2+90):addTo(self,Zorder_Clock):setVisible(false)
	self.clockTime=cc.LabelAtlas:create("","game/clocktime.png",30,38,string.byte("00")):addTo(self.clock):setPosition(cc.p(23,40))


	self.m_TimeProgress={}
	self.headParticles={}

		--定位按钮
	self.btLocation = self:getChildByName("btn_addr") --ccui.Button:create("game/btn_local1.png","game/btn_local2.png","game/btn_local1.png",ccui.TextureResType.localType)
	self.btLocation:setTag(TagEnum.BTN_LOCATION)
	
 		--:move(1080, 702)
	self.btLocation:addTouchEventListener(btnEvent)


	--test
	-- self:showActionSps(3)
	-- 托管中
	local trustBg = cc.Sprite:create("game/trusteeBg.png"):setPosition(yl.WIDTH/2,97)
	trustBg:addTo(self,1000)
	trustBg:setVisible(false)
	local trustTip = cc.Sprite:create("game/trusteeTip.png")
	trustTip:addTo(trustBg)
	trustTip:setPosition(yl.WIDTH/2,97)
	local btn = ccui.Button:create("game/bt_cancelTrustee.png","game/bt_cancelTrustee.png","game/bt_cancelTrustee.png",ccui.TextureResType.localType)
    btn:addClickEventListener(function() self._scene:setTrustee() end)
	btn:addTo(trustBg)
	btn:setPosition(yl.WIDTH/2,184)
	self.trustNode = trustBg
	ExternalFun.registerTouchEvent(trustBg)
 	trustBg.onTouchBegan=function() return true end
 	trustBg.onTouchEnded=function() 
	 	if trustBg:isVisible() then 
	 		-- self._scene:setTrustee()
	 	end
 	end

 	--------------------------------------------------------
 	--自动托管相关
 	local node_auto = self:getChildByName("node_auto")
 	local node_bet_menu = self:getChildByName("node_bet_menu")
 	node_bet_menu:setVisible(false)

 	self.btAuto = node_auto:getChildByName("btn_auto")
    self.btAuto:addTouchEventListener(btnEvent)
    self.btAuto:setTag(TagEnum.BT_AUTO)

    local btn_bet_menu = node_auto:getChildByName("btn_bet_menu")
    btn_bet_menu:setTag(1)
    btn_bet_menu:addTouchEventListener(function(ref, event)
	        if event == ccui.TouchEventType.ended then
	        		if ref:getTag() == 1 then
	        			node_bet_menu:setVisible(true)
	        			ref:setTag(2)

	        			btn_bet_menu:loadTextureNormal("game/btn_arrow_up.png")
 	    				btn_bet_menu:loadTexturePressed("game/btn_arrow_up.png")
	        		else
	        			node_bet_menu:setVisible(false)
	        			ref:setTag(1)

	        			btn_bet_menu:loadTextureNormal("game/btn_arrow_down.png")
 	    				btn_bet_menu:loadTexturePressed("game/btn_arrow_down.png")
	        		end
	        end
    end)

    self.minBet = node_bet_menu:getChildByName("btn_min_bet")
    self.minBet:addTouchEventListener(function(ref, event)
	        if event == ccui.TouchEventType.ended then	
	        	self._scene:sendTrusteeMode(0)
	        end
    end)

    self.maxBet = node_bet_menu:getChildByName("btn_max_bet")
    self.maxBet:addTouchEventListener(function(ref, event)
	        if event == ccui.TouchEventType.ended then
	        	self._scene:sendTrusteeMode(1)
	        end
	    
    end)
    --------------------------------------------------------
	-- 电量,时间，信号
	local bg = self:getChildByName("topTipsBg")
	local BatteryBg = bg:getChildByName("dianliang")
	local sp = cc.Sprite:create("game/yule/paijiu/res/game/dianliang2.png")
	self.batteryBar = cc.ProgressTimer:create(sp)
	self.batteryBar:setType(cc.PROGRESS_TIMER_TYPE_BAR)
    self.batteryBar:setMidpoint(cc.p(0,0.5))
    self.batteryBar:setBarChangeRate(cc.p(1, 0))
    self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
    self.batteryBar:setPosition(BatteryBg:getContentSize().width/2-2, BatteryBg:getContentSize().height/2)
    BatteryBg:addChild(self.batteryBar)
	bg:getChildByName("Text_1"):setString(os.date("%H:%M"))
    self.schedule = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
		self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
		bg:getChildByName("Text_1"):setString(os.date("%H:%M"))
		local netType = MultiPlatform:getInstance():getNetworkType()
		local Strength = MultiPlatform:getInstance():getNetworkStrength()
		local TypePic = {"net_","wifi_"}
		bg:getChildByName("netType"):setTexture("game/"..TypePic[netType]..Strength..".png")
	end, 1, false)
end

--更换最大最小下注显示
function GameViewLayer:changeMaxOrMinBet()
	local node_auto = self:getChildByName("node_auto")
 	local node_bet_menu = self:getChildByName("node_bet_menu")
	local btn_bet_menu = node_auto:getChildByName("btn_bet_menu")

	if self.cbTrusteeMode == 0 then -- 最小下注
        self.minBet:loadTextureNormal("game/btn_min_2.png")
		self.minBet:loadTexturePressed("game/btn_min_2.png")

		self.maxBet:loadTextureNormal("game/btn_max_1.png")
		self.maxBet:loadTexturePressed("game/btn_max_1.png")

		self.bIsMaxBet = false
		node_bet_menu:setVisible(false)
		btn_bet_menu:setTag(1)
    elseif self.cbTrusteeMode == 1 then --最大下注
        self.minBet:loadTextureNormal("game/btn_min_1.png")
		self.minBet:loadTexturePressed("game/btn_min_1.png")

		self.maxBet:loadTextureNormal("game/btn_max_2.png")
		self.maxBet:loadTexturePressed("game/btn_max_2.png")
		self.bIsMaxBet = true
		node_bet_menu:setVisible(false)
		btn_bet_menu:setTag(1)
    end 
end

function GameViewLayer:isShowAutoTrust()
	local node_auto = self:getChildByName("node_auto");
	if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
		node_auto:setVisible(false);
	else
		node_auto:setVisible(true);
	end
	
end

--更换是否正在托管（自动进行中的图片）
function GameViewLayer:changeAutoImg()
	self.btAuto:loadTextureNormal("game/btn_auto_"..(self._scene.bTrustee==true and 2 or 1)..".png")
 	self.btAuto:loadTexturePressed("game/btn_auto_"..(self._scene.bTrustee==true and 2 or 1)..".png")
end

function GameViewLayer:resetAutoSet()
	self.bIsMaxBet = false 
	-- self.nAutoStatus = cmd.AUTO_PLAY_FREE
	-- self.bLastCall = TAG_ENUM.BT_CANCEL
	self.btAuto:loadTextureNormal("game/btn_auto_1.png")
 	self.btAuto:loadTexturePressed("game/btn_auto_1.png")
 	self.minBet:loadTextureNormal("game/btn_min_2.png")
 	self.minBet:loadTexturePressed("game/btn_min_2.png")

 	self.maxBet:loadTextureNormal("game/btn_max_1.png")
 	self.maxBet:loadTexturePressed("game/btn_max_1.png")
 	self.maxBet:getParent():setVisible(false)
 	local node_auto = self:getChildByName("node_auto")
 	node_auto:getChildByName("btn_bet_menu"):setTag(1)
end

function GameViewLayer:showWithAction(sp,kind)
	if nil==kind or 1==kind then
		--sp:stopAllActions()
		sp:setVisible(true)
		sp:setScale(0.0001)
		sp:runAction( cc.ScaleTo:create(0.1, 1.0))
	end
end


function GameViewLayer:showActionSps(kind,viewid)

	local robAnimDt=self.robAnimDt or 0
	local sendCardDt=self.cardAnimDt or 0
	local endAnimDt=self.endAnimDt or 0
	local tabTime={self._scene.cbTimeCallBanker,self._scene.cbTimeSetChip,self._scene.cbTimeRangeCard}  
	if self._scene.bReceivedGameMsg then
		tabTime={self._scene.cbTimeCallBanker,math.max(0,self._scene.cbTimeSetChip-robAnimDt),math.max(0,self._scene.cbTimeRangeCard-sendCardDt)}  
	end
	if GlobalUserItem.bVideo then
		tabTime={10,math.max(0,10-robAnimDt),math.max(0,10-sendCardDt)}  --CMD_S_GameStart消息里加cbTimexxx
	end
	local time
	if kind==0 then --准备
		time=self._scene.cbTimeStartGame
		if self._scene.bReceivedGameMsg then
			time=math.max(0,time -endAnimDt)
		end
	else
		time=tabTime[kind]
	end
	print("mind:",kind)
	--assert(time>0)
	if nil==time or time<=0 then return end
    ---------------------above newly added ----------------------
	-------------------------------------------------------------
	--同步重连状态
	local percentage = 100;
	if self._scene.bReceivedGameMsg then
	else
		if kind == 3 then
			percentage = 100 * (self._scene.cbTimeRemain/self._scene.cbTimeRangeCard);
			time = self._scene.cbTimeRemain;
		elseif kind == 2 then
			percentage = 100 * (self._scene.cbTimeRemain/self._scene.cbTimeSetChip);
			time = self._scene.cbTimeRemain;
		elseif kind == 1 then
			percentage = 100 * (self._scene.cbTimeRemain/self._scene.cbTimeCallBanker);
			time = self._scene.cbTimeRemain;
		end
	end
	------------------------------------------------------------
	local paths={"game/robbingsp.png","game/chippingsp.png","game/combiningsp.png"}
	self.actSps=self.actSps or {}
	local tabW={93,122,122} --三图宽
	local robW=56 --抢宽
	local dotW=4*20
	for i=1,cmd.GAME_PLAYER do --viewid
		if (nil==viewid or i==viewid) and i~=cmd.MY_VIEWID and (self.tabPlayStatus[i]==cmd.PS_SITTING or kind==0) and not (i==self.viewBanker and kind==cmd.actChip) then 
			-- showToast(self._scene,"showActionSps viewid "..i,2)
			self:createHeadAnim(i)
		 	local progress = self.m_TimeProgress[i]  
			for j=1,2 do
				if progress[j] then
		            progress[j]:setPercentage(percentage)
		            progress[j]:setVisible(true)
		            progress[j]:stopAllActions()
		            progress[j]:runAction(cc.Sequence:create(cc.ProgressTo:create(time, 0), cc.CallFunc:create(function()
		                progress[j]:setVisible(false)
		            end)))
		        end
		    end
		    self:setShowParticle(i,time,true)

		    if kind~=0 then
				self.actSps[i]=self.actSps[i] or {}
				 
				local bRight,bTop=self:getDir(i)

				local x,y =posRobSp[i].x,posRobSp[i].y+(kind==cmd.actChip and 10 or 0)
				local offX=tabW[kind]/2
				
				if kind==cmd.actCombine then
					x,y=(posCards[i][1].x+posCards[i][3].x)/2,posCards[i][1].y-80
					if bTop then y=y-39 end
					if bRight then x=x-20 end
					if bRight and self._scene.cardNumLimit==2 then
						x=x-90
					end
				else
					if bRight then
						x=posRobSp[i].x-(tabW[kind]-robW)/2-dotW
					else
						x=posRobSp[i].x+(tabW[kind]-robW)/2
					end
				end

				local sp=cc.Sprite:create(paths[kind]):addTo(self) 
			
				sp:setPosition( cc.p(x,y))
				table.insert(self.actSps[i],sp)
				self:createBlinkDots(i,cc.p(x+offX,y+10),0.2,bReverse)
				-- self:createBlinkDots(i,cc.p(b and x-130 or x+50,y+10),0.1,bReverse)
			end
		end
	end
end

function GameViewLayer:removeActionSps(viewid,bNotRemoveHeadAnims)
	--showToast(self._scene,"removeActionSps "..debug.getinfo(2).name.." "..debug.getinfo(3).name,5)
	
	if self.actSps and self.actSps[viewid] then  
		for i=1,#self.actSps[viewid] do
			self.actSps[viewid][i]:removeSelf()
			self.actSps[viewid][i]=nil
		end
	end

	if true==bNotRemoveHeadAnims and self.tabUserItem[viewid] then return end

	self:setShowParticle(viewid,0,false)

	local progress = self.m_TimeProgress[viewid]  
	for i=1,2 do
		if progress and progress[i] then
            progress[i]:setPercentage(100)
            progress[i]:setVisible(false)
            progress[i]:stopAllActions()
        end
    end

end


function GameViewLayer:createHeadAnim(i)--viewid
	self.m_TimeProgress=self.m_TimeProgress or {}
	self.headParticles=self.headParticles or {}
	self.m_TimeProgress[i]=self.m_TimeProgress[i] or {}
	if nil==self.m_TimeProgress[i][1] or tolua.isnull(self.m_TimeProgress[i][1]) then
		self.m_TimeProgress[i][1]=cc.ProgressTimer:create(display.newSprite( (i == cmd.MY_VIEWID and "game/progressTimerSp1.png" or "game/progressTimerSp1.png")))--"game/game_time_progressex.png" or "game/game_time_progressex.png")))
	         :setReverseDirection(true)
	         :move(cc.p(posHead[i].x,posHead[i].y-2))
	         :setVisible(false)
	         :setPercentage(100)
	         :addTo(self)
	end

	if nil==self.m_TimeProgress[i][1] or tolua.isnull(self.m_TimeProgress[i][2]) then
    	self.m_TimeProgress[i][2]=cc.ProgressTimer:create(display.newSprite( (i == cmd.MY_VIEWID and "game/progressTimerSp2.png" or "game/progressTimerSp2.png")))--"game/game_time_progressex.png" or "game/game_time_progressex.png")))
         :setReverseDirection(true)
         :move(cc.p(posHead[i].x,posHead[i].y-2))
         :setVisible(false)
         :setPercentage(100)
         :addTo(self)
    end
    
    self.headParticles[i]=self.headParticles[i] or {}

    if nill==self.headParticles[i] or tolua.isnull(self.headParticles[i]) then
	    self.headParticles[i]=cc.ParticleSystemQuad:create("game/NewParticle.plist")
	    self.headParticles[i]:addTo(self):setScale(0.1,0.05)--:setScale(0.1,0.05)--:setVisible(false) :setScale(0.4)
	end
	
end


function GameViewLayer:setShowParticle(ith,t,bShow)--viewid
	
	if nill==self.headParticles[ith] or tolua.isnull(self.headParticles[ith]) then
		print("ith:",ith)
		
		return 
	end

	if not self._scene.bReceivedGameMsg then
		bShow = false;
	end

	self.headParticles[ith]:setVisible(bShow)
	self.headParticles[ith]:stopAllActions()
	if not bShow then
		--self.headParticles[ith]:removeSelf()
		return
	end
	
	local w,h=104,160

 	local totalDis=2*(w+h)
 	local points={
 		cc.p(w/2,0),
 		cc.p(0,-h),
 		cc.p(-w,0),
 		cc.p(0,h),
 		cc.p(w/2,0)
 	}

 	local angles={180,90,0,270,180}

 	tabDis={w/2,h,w,h,w/2}

 	local tabAct={}
	for i=1,#points do
		table.insert(tabAct,cc.CallFunc:create(function() self.headParticles[ith]:setAngle(angles[i]) end))
		table.insert(tabAct,cc.MoveBy:create(t*tabDis[i]/totalDis,points[i]))
	end
 	------------------------------------------------ 
 -- 	local tabPercent = {tabDis[1]/totalDis*100, tabDis[2]/totalDis*100, tabDis[3]/totalDis*100, tabDis[4]/totalDis*100, tabDis[5]/totalDis*100};
 -- 	dump(tabPercent, "-------------------", 6)
 -- 	local index = 1;
 -- 	local isCanAdd = true;
 -- 	local totolPer = 0;
 -- 	if percentage and percentage > 0 and percentage ~= 100 then
 -- 		totalDis = totalDis * percentage/100;
	--  	for i = 1, 5 do
	--  		totolPer = totolPer + tabPercent[i];
	--  		if totolPer >= (100 - percentage) and isCanAdd then
	--  			index = i;
	--  			isCanAdd = false;
	--  		end
	--  	end
	--  	print("-------------66666666666666666666----------index------", index)

	--  	for i=1,#points do
	--  		if i >= index then
	--  			table.insert(tabAct,cc.CallFunc:create(function() self.headParticles[ith]:setAngle(angles[i]) end))
	--  			table.insert(tabAct,cc.MoveBy:create(t*tabDis[i]/totalDis,points[i]))
	--  		end
	--  	end
	-- else
	-- 	for i=1,#points do
	--  		table.insert(tabAct,cc.CallFunc:create(function() self.headParticles[ith]:setAngle(angles[i]) end))
	--  		table.insert(tabAct,cc.MoveBy:create(t*tabDis[i]/totalDis,points[i]))
	--  	end
 -- 	end
 -- 	print("-------------777777777777777777777777777----------index------", index)
 	------------------------------------------------

 	
 	
 	self.headParticles[ith]:setPosition(cc.p(posHead[ith].x,posHead[ith].y+h/2-5))
 	
 	self.headParticles[ith]:runAction( cc.Sequence:create(tabAct))  
end

function GameViewLayer:createBlinkDots(viewid,pos,dt,bReverse)
	
	dt =dt or 0.5
	local offX=bReverse and -20 or 20
	for i=1,3 do
		local dot=cc.Sprite:create("game/dot.png"):addTo(self)
		dot:setPosition(cc.p(pos.x+i*offX,pos.y))
		dot:setVisible(i==1)
		if i~=1 then
			local d1,d2=dt,2*dt --d1隐藏时间 d2显示时间
			if i==3 then d1,d2=2*dt,dt end
			local seq=cc.Sequence:create(cc.DelayTime:create(d1),cc.Show:create(),cc.DelayTime:create(d2),cc.Hide:create())
			dot:runAction(cc.RepeatForever:create(seq))
		end
		table.insert(self.actSps[viewid],dot)
	end
	
end

function GameViewLayer:testPlayCoinAnimation(posA,posB,n)

	local function playCoinFlyAnimation(posA,posB,n)--从posA飞到posB

		local function getMoveAction(time,startPoint, endPoint,height,angle)
		   	--把角度转换为弧度
		    angle = angle or 90
		    height = height or 50
		    local radian = angle*3.14159/180.0
		    --第一个控制点为抛物线左半弧的中点  
		    local q1x = startPoint.x+(endPoint.x - startPoint.x)*0.25;  
		    local q1 = cc.p(q1x, height + startPoint.y+math.cos(radian)*q1x);         
		    -- 第二个控制点为整个抛物线的中点  
		    local q2x = startPoint.x + (endPoint.x - startPoint.x)*0.5;  
		    local q2 = cc.p(q2x, height + startPoint.y+math.cos(radian)*q2x);  

		     local q3x = startPoint.x + (endPoint.x - startPoint.x)*0.75;  
		    local q3 = cc.p(q3x, height + startPoint.y+math.cos(radian)*q3x);  
		    --曲线配置  
		    local bezier = {

		        q1,
		        q3,
		      
		        endPoint
		    }
		    --使用EaseInOut让曲线运动有一个由慢到快的变化，显得更自然  
		    local beaction = cc.BezierTo:create(time, bezier)
		    --if inorout == 0 then
		    local easeoutaction = cc.EaseInOut:create(beaction,9.5)
		    return easeoutaction
		    --else
		        --return cc.EaseIn:create(beaction, 1)
		    --end
		end

		for jth=1,1 do
			
			--posB=cc.p(posB.x+(jth-2)*100,posB.y+(jth-2)*100)
			n=n or 50
			local a,b=10,15
			for i=1,n do
				adjPosA=cc.p(posA.x+math.random(-a,a),posA.y+math.random(-a,a))
				adjPosB=cc.p(posB.x+math.random(-b,b),posB.y+math.random(-b,b))
				local tabAct={}
				local t=math.floor(i/3)
				table.insert(tabAct,cc.DelayTime:create(4*(t/n)))--math.random()*0.3))
				table.insert(tabAct,cc.FadeTo:create(0.1,255))
				local mov=cc.MoveTo:create(0.36,adjPosB)
				table.insert(tabAct, mov)--cc.EaseIn:create(mov,2))--getMoveAction(1.5,posA,posB))
				table.insert(tabAct,cc.DelayTime:create(2*(1-t/n)))
				table.insert(tabAct,cc.RemoveSelf:create())

				local sp=cc.Sprite:create("game/flycoin.png"):addTo(self,Zorder_Flycoin):setPosition(adjPosA):setOpacity(0):setScale(0.75)
				sp:runAction(cc.Sequence:create(tabAct))
			end
		end
	end

	if self.tsp then self.tsp:removeFromParent() end
	local time1=2
	playCoinFlyAnimation(posA,posB,n)
	-- local scaleBy=cc.ScaleBy:create(time1,1.5)
	-- self.tsp=cc.Sprite:create("game/shengju.png"):addTo(self)
	-- self.tsp:setPosition(posResultSp[self._scene.cardNumLimit>2 and 1 or 2][1])
	-- 	:setOpacity(255)
	-- local act1=cc.Sequence:create( cc.Spawn:create(cc.FadeIn:create(time1),cc.EaseBounceOut:create(scaleBy)),scaleBy:reverse())
	-- local jumpBy=cc.JumpBy:create(time1/2,cc.p(0,50),100,1)
	-- local act2=cc.Sequence:create( cc.Spawn:create(cc.FadeIn:create(time1/2),jumpBy,scaleBy),cc.Spawn:create(scaleBy:reverse(),jumpBy:reverse()))
	-- self.tsp:runAction(cc.EaseBounceOut:create(cc.JumpBy:create(time1,cc.p(0,50),50,1)))
end

 function GameViewLayer:onExit()
 	
	print("GameViewLayer onExit")
	cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.schedule)
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    self.m_UserItem = {}
    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
end

function GameViewLayer:hideAllButtons( )
	for i=1,#self.tabButtons do
		self.tabButtons[i]:setVisible(false)
	end
end

function GameViewLayer:setShowReadyBtn(bShow)
	print(debug.traceback())
	self.readyBtn:setVisible(bShow)
	if GlobalUserItem.bPrivateRoom then
		self.btnChangeDesk:setVisible(false)
		self.readyBtn:setPositionX(667)
	else
		self.btnChangeDesk:setVisible(bShow)
		if bShow then
			self:showWithAction(self.btnChangeDesk)
		end
	end
	if bShow then
		self:showWithAction(self.readyBtn)
	end
end

function GameViewLayer:setPlayStatus(viewid,status)
	self.tabPlayStatus[viewid]=status
end

function GameViewLayer:getTableCardNum()
	local leftCard=self._scene.allCardCount
	for i=1,cmd.GAME_PLAYER do
		if self.tabPlayStatus[i]==cmd.PS_SITTING then
			leftCard=leftCard - self._scene.cardNumLimit
		end
	end
	return leftCard
end

 function GameViewLayer:setShowRobBtn(bShow)
 	if bShow and self._scene.bTrustee then
 		bShow = false;
 	end
 	self.robBtn:setVisible(bShow)
 	self.noRobBtn:setVisible(bShow)
 end



function GameViewLayer:beginRobAnimation(bankerViewId)
	print("[bankerViewId]:",bankerViewId)
	print("posHead[bankerViewId]:",posHead[bankerViewId])

	self.bInRobAnimation=true

	local tabAct={
		cc.FadeTo:create(0.1,255),
		cc.DelayTime:create(0.1),
		--cc.MoveTo:create(0.7,posHead[bankerViewId]),
		cc.ScaleTo:create(0.1,0.2),
		--cc.FadeTo:create(0.2,0),
		cc.CallFunc:create(function() 
			self.bInRobAnimation=false 
			print("bankerViewId:",bankerViewId)
			self.tabHeadSp[bankerViewId]:setBankerState(true) 
			if self.toShowChipBtns then
				self:setShowChipBtns(true,self.tabChipNum)
			end
			self.toShowChipBtns=false
			if self._scene.chipMsgTime then
				self.robAnimDt=(currentTime()-self._scene.chipMsgTime)/1000
			else
				self.robAnimDt=0
			end
			self:showActionSps(cmd.actChip)
			if self.tabPlayStatus[cmd.MY_VIEWID]==cmd.PS_SITTING and cmd.MY_VIEWID~=bankerViewId then
				self._scene:setChipClock(self.robAnimDt)
			end
		end
		),
		cc.CallFunc:create(function() 
			if self.robRect.layerBg and not tolua.isnull(self.robRect.layerBg) then
		 		self.robRect.layerBg:removeSelf() 
		 		self.robRect.layerBg=nil
		 	end
		 end) --self.robRect.layerBg
		,
		cc.RemoveSelf:create()
	}
	self.bankerAni=cc.Sprite:create("game/bankerani.png"):addTo(self.robRect.layerBg or self,40):setPosition(yl.WIDTH/2,yl.HEIGHT/2)
	:setOpacity(0):setPosition(posHead[bankerViewId])
	self.bankerAni:runAction(cc.Sequence:create(tabAct))
end

function GameViewLayer:beginRobAnimation2(bankerViewId)
	 self.bInRobAnimation=true

	if self.robRect and not tolua.isnull(self.robRect) then
		self.robRect:removeSelf()
	end

	local totalTime=3
	local tabDelay={1,2,3,4,5,6,7} --轮数越多越快
	local s=0
	for i=1,#tabDelay do
		s=s+tabDelay[i]
	end
	local tabPos={}
	local tabAct={}
	local tabViewId={}
	for viewid=1,cmd.GAME_PLAYER do
		if self.tabHeadSp[viewid] and not tolua.isnull(self.tabHeadSp[viewid]) and self.tabRobState[viewid]  then
			print(self.tabHeadSp[viewid] , tolua.isnull(self.tabHeadSp[viewid]) , self.tabRobState[viewid])
			print("view:"..viewid,self.tabHeadSp[viewid]:getPosition())
			table.insert(tabPos,cc.p(self.tabHeadSp[viewid]:getPosition()))
			table.insert(tabViewId,viewid)
		end
	end

	local unitDelay=totalTime/(s*#tabPos)

	for i=1,#tabDelay do
		for j=1,#tabPos do --人数
			local act1=cc.CallFunc:create(function() self.robRect:setPosition(tabPos[j])  end)
			local act2=cc.DelayTime:create(tabDelay[i]*unitDelay)
			table.insert(tabAct,act1)
			table.insert(tabAct,act2)
			if i==#tabDelay and tabViewId[j]==bankerViewId then
				table.insert(tabAct,cc.DelayTime:create(0.5))
				break
			end
		end
	end
	-- table.insert(tabAct,cc.CallFunc:create(function() 
	-- 	self.bInRobAnimation=false 
	-- 	self.tabHeadSp[bankerViewId]:setBankerState(true) 
	-- 	if self.toShowChipBtns then
	-- 		self:setShowChipBtns(true,self.tabChip)
	-- 	end
	-- 	self.toShowChipBtns=false
	-- end))
	table.insert(tabAct,cc.CallFunc:create(function() self:beginRobAnimation(bankerViewId) end))
	--table.insert(tabAct,cc.CallFunc:create(function() self.robRect.layerBg:removeSelf() end) )
	--table.insert(tabAct,cc.RemoveSelf:create())
	
	--layerBg:runAction(cc.DelayTime:create(),cc.RemoveSelf:create())
	self.robRect=cc.Sprite:create("game/bankerrect.png"):setScale(1.05)
	
	if #tabViewId>1 then
		local layerBg=cc.LayerColor:create(cc.c4b(30,30,30,160),1334,750)
		layerBg:addTo(self,100000) 
		layerBg:setName("robRectBg")
		self.robRect:addTo(layerBg)
		self.robRect:runAction(cc.Sequence:create( tabAct))
		self.robRect.layerBg=layerBg
	else
		self.robRect.layerBg=nil
		self.robRect:addTo(self,100000)
		self.robRect:setPosition(posHead[bankerViewId]) 
		--self.robRect:runAction(cc.Sequence:create(cc.Blink:create(3,6),cc.CallFunc:create(function() self:beginRobAnimation(bankerViewId) end)))
		self.robRect:setOpacity(0)
		self.robRect:runAction(cc.Sequence:create(cc.FadeTo:create(0.01,1),cc.CallFunc:create(function() self:beginRobAnimation(bankerViewId) end)))
	end

end

function GameViewLayer:endRobAnimation()
	if self.robRect and not tolua.isnull(self.robRect) then
		self.robRect:removeFromParent()
	end
	self.bInRobAnimation=false
	self.toShowChipBtns=false

	if self.bankerAni and not tolua.isnull(self.bankerAni) then
		self.bankerAni:removeFromParent()
		self.bankerAni=nil
	end
end

function GameViewLayer:setBankerUser(viewid,bRobBanker)
	self.viewBanker=viewid
	self:setShowRobBtn(false)
	--庄家头像显示庄家标志
	-- self.tabHeadSp[viewid]:setBankerState(true)

	if bRobBanker then --如果非轮庄
		--self:beginRobAnimation(viewid)
		self:beginRobAnimation2(viewid)
	else
		self.tabHeadSp[viewid]:setBankerState(true)
	end
	
end


function GameViewLayer:showRobResult(viewid,robState) --0没动作 1放弃 2抢 

	if nil==viewid then
		for k,v in pairs(self.tabRobSp) do
			v:runAction(cc.Sequence:create(cc.FadeTo:create(1,0),cc.RemoveSelf:create()))
		end
		return
	end

	self.tabRobState=self.tabRobState or {}
	self.tabRobState[viewid]= robState==2

	if robState==0 then return end

	self:removeActionSps(viewid)

	local bRob=robState==2
	local path= bRob==true and "game/qiang.png" or "game/buqiang.png"
	if self.tabRobSp[viewid] and not tolua.isnull(self.tabRobSp[viewid]) then
		self.tabRobSp[viewid]:removeSelf()
	end
	self.tabRobSp[viewid]=cc.Sprite:create(path):addTo(self):setPosition(posRobSp[viewid])
	self:showWithAction(self.tabRobSp[viewid])
	-- if bRob then
	-- 	local jmp=cc.JumpBy:create(0.5,cc.p(0,10),10,1)
	-- 	self.tabRobSp[viewid]:runAction(cc.Sequence:create(jmp,jmp:reverse()))
	-- end
end

function GameViewLayer:setShowChipBtns(bShow,tabChip)
	self.tabChipNum=tabChip
	if bShow and self.bInRobAnimation then self.toShowChipBtns=true self.tabChip=tabChip return end
	
	if bShow and self._scene.bTrustee then
 		bShow = false;
 	end

	self.chipNode:setVisible(bShow)
	print("#tabChip",#tabChip)
	local k=0
	if tabChip then
		for i=1,math.min(5,#tabChip) do
			print("i:",i)
			local btn=self.chipNode:getChildByName("chipbtn_"..i)
			if tabChip[i]>0 then
				btn:setVisible(true)
				btn:getChildByName("chipnum"):setString(tabChip[i])
			else
				btn:setVisible(false)
				k=k+1
			end
		end
		self.chipNode:setPositionX(yl.WIDTH/2+k*186/2+7)
		self.chipNode:setPositionY(160-17)
	end
end

function GameViewLayer:showUserChip(viewid,chip)

	if viewid==self.viewBanker then  
	 	return 
	end

	--显示viewid下注大小
	if self._chipLayer==nil or tolua.isnull(self._chipLayer) then
		self._chipLayer=cc.Layer:create():addTo(self)
	end

	local bRight=self:getDir(viewid)
	local offX =bRight and -25 or 30

	local chipbg=ccui.ImageView:create("game/chipbg.png"):addTo(self._chipLayer):setPosition(cc.p(posChip[viewid].x+offX-6,posChip[viewid].y+2)) 
	chipbg:setScale9Enabled(true)
	
	local node=cc.LabelAtlas:create(chip,"game/chipDigits.png",20,30,string.byte('0')):addTo(self._chipLayer)
				--:setAnchorPoint(cc.p(bRight and 1 or 0,0.5))
				:setAnchorPoint(cc.p(0.5,0.5))
	--chipbg:setAnchorPoint(cc.p(bRight and 1 or 0,0.5))

	local dx=100
	node:setPosition(cc.p(posChip[viewid].x-dx+offX,posChip[viewid].y))
	node:runAction(cc.MoveBy:create(0.1,cc.p(dx,0)))
   
	chipbg:setContentSize(cc.size(node:getContentSize().width+40,chipbg:getContentSize().height))
	--chipbg:setScaleX(1.4*node:getContentSize().width/chipbg:getContentSize().width)
	--local x=node:getPositionX()-node:getContentSize().width/2
	
end

function GameViewLayer:setZupaiFinished(bFinished)
	if not bFinished and self._scene.bTrustee then
 		bFinished = true;
 	end
	self.bZupaiFinished=bFinished
	self.zupaiBtn:setVisible(not bFinished)
	self.notifyBtn:setVisible(self._scene.cardNumLimit>2 and not bFinished)
	self.cancelBtn:setVisible(false)
end

function GameViewLayer:createCardNodes(myCards)
	
	self.myCardData=myCards

	for viewid=1,cmd.GAME_PLAYER do
		for j=1,self._scene.cardNumLimit do
			if self.tabPlayStatus[viewid]==cmd.PS_SITTING then
				local cardvalue=viewid==cmd.MY_VIEWID and self.myCardData[j] or nil 
				self:createCardNode(viewid,j,cardvalue)
			end
		end
	end
end


function GameViewLayer:createAllOpenCardNodes(allOpenCards)--video 索引chairid
	
	self.myCardData=allOpenCards[self._scene:GetMeChairID()+1]
	
	for i=1,cmd.GAME_PLAYER do
		local viewid=self._scene:SwitchViewChairID(i-1)
		for j=1,self._scene.cardNumLimit do
			if self.tabPlayStatus[viewid]==cmd.PS_SITTING then
				local cardvalue=allOpenCards[i][j]
				self:createCardNode(viewid,j,cardvalue)
			end
		end
	end
end


function GameViewLayer:createCardNode(viewid,sendRound,cardvalue)

	if viewid==cmd.MY_VIEWID and cardvalue==nil then
		return
	end
	if cardvalue then
		path=string.format("card/%d.png",cardvalue)
	else 
		path=sendRound>2 and "game/backvertical.png" or "game/backvertical.png"  --"game/backhorizontal.png"
	end 
	local cardW=70
	if viewid==cmd.MY_VIEWID then
		cardW=70
		local mycard=ccui.ImageView:create(path):addTo(self._cardLayer)
		mycard:setPosition(posHead[viewid].x+20+(cardW+1)*sendRound ,posHead[viewid].y)
		local function onImageViewClicked(sender)
            self:popMyCard(sender.index)
        end
        mycard.dir=1
        mycard.popDis=20
        mycard.index=sendRound
		mycard:setTouchEnabled(true)
		mycard:addClickEventListener(onImageViewClicked)
		self.tabHandCardNode[viewid][sendRound]=mycard
	else
		cardW=70*cardScale
		local sign=self:getDir(viewid) and -1 or 1
		self.tabHandCardNode[viewid][sendRound]=cc.Sprite:create(path):addTo(self._cardLayer):setScale(cardScale)
		self.tabHandCardNode[viewid][sendRound]:setPosition(posHead[viewid].x+(29+(cardW+1)*sendRound)*sign ,posHead[viewid].y) --25
		-- if self._scene._cardNumLimit==2  then
		-- 	self.tabHandCardNode[viewid][sendRound]:setRotation(90)
		-- else 
		-- 	if sendRound<3 then
		-- 		self.tabHandCardNode[viewid][sendRound]:setRotation(90)
		-- 	end
		-- end
	end

	if viewid==cmd.MY_VIEWID and sendRound==self._scene.cardNumLimit then
		self:setZupaiFinished(false)
	end
end

function GameViewLayer:showDice(num,index)

    local frames = {}

    for i=1,31 do
      local frame = cc.SpriteFrame:create("game/SICK"..num..".png",cc.rect(70*(i-1),0,70,128))
      table.insert(frames, frame)
    end

    local  animation =cc.Animation:createWithSpriteFrames(frames,0.05)

    local sprite = cc.Sprite:createWithSpriteFrame(cc.SpriteFrame:create("game/SICK"..num..".png",cc.rect(0,0,70,128)))
    
    sprite:runAction(cc.Sequence:create(cc.Animate:create(animation),cc.FadeTo:create(0.5,0),cc.RemoveSelf:create()))
    sprite:setTag(Tag_Dice+index)
    
    sprite:setPosition(610+(index-1)*120,300)
    self:addChild(sprite) 
   -- ExternalFun.playSoundEffect("DRAW_SICE.wav")
end

function GameViewLayer:createZhanjiView()
	if self.zhanjiLayer and not tolua.isnull(self.zhanjiLayer) then 
    	self.zhanjiLayer:setVisible(true)
    	return true
    end

	 local zhanjiLayer=cc.CSLoader:createNode("game/ZhanjiLayer2.csb")
	 zhanjiLayer:addTo(self,Zorder_ZhanjiLayer)
	 self.zhanjiLayer=zhanjiLayer
	 zhanjiLayer:getChildByName("Panel_1"):setTouchEnabled(true)
	 :setSwallowTouches(true)
	 --:addClickEventListener(function() zhanjiLayer:removeSelf() end)
	
	 local bg=zhanjiLayer:getChildByName("sp_bg")
	  bg:getChildByName("btn_close"):addClickEventListener(function() zhanjiLayer:removeSelf() end)
	 local content=bg:getChildByName("content")
	 local cellWidth=content:getContentSize().width

	 local tag_wanfa,tag_liushui=1,2
	 local check_wanfa=bg:getChildByName("CheckBox_1"):setTag(tag_wanfa)
	 local check_liushui=bg:getChildByName("CheckBox_2"):setTag(tag_liushui)

	 -----test--------------
	 -- local testN=5
	 -- for viewid=1,testN do
	 -- 	self.tabUserItem[viewid]=self.tabUserItem[cmd.MY_VIEWID]
	 -- end
	 -- for viewid=1+testN,6 do
	 -- 	self.tabUserItem[viewid]=nil
	 -- end
	 -----test end--------------

	
	 local playerN=0
	 for i=1,cmd.GAME_PLAYER do --chairid
		local viewid=self._scene:SwitchViewChairID(i-1)
		if self.tabUserItem[viewid] then
			playerN=playerN+1
		end
	 end
	 local scoreWidth=cellWidth/(1+playerN)

	 local title=bg:getChildByName("liushui_title"):setVisible(false)
	 self.liushuiTitle=title
	 title:getChildByName("jushu"):setString("局数")
	 title:getChildByName("jushu"):setPositionX(0.5*scoreWidth)


	 local j=1
	 for i=1,cmd.GAME_PLAYER do --chairid
		local viewid=self._scene:SwitchViewChairID(i-1)
		if self.tabUserItem[viewid] then
			j=j+1
			local strNickname = string.EllipsisByConfig(self.tabUserItem[viewid].szNickName, scoreWidth-5, 
                                            string.getConfig("fonts/round_body.ttf", (viewid==cmd.MY_VIEWID and 22 or 20)))
			title:getChildByName("score"..i):setString(strNickname)
			if viewid==cmd.MY_VIEWID then  
				--title:getChildByName("score"..i):setFontSize(24)
				title:getChildByName("score"..i):setTextColor(cc.c4b(0xbe,0x93,0xdb,0xff))
			end
			title:getChildByName("score"..i):setPositionX((j-0.5)*scoreWidth)
		else
			--local strNickname = string.EllipsisByConfig("啊as对方答复撒范德萨发", 80, 
                                            --string.getConfig("fonts/round_body.ttf", 20)) --test
			--title:getChildByName("score"..i):setString(strNickname) --test
			title:getChildByName("score"..i):setString("")
		end

		if GlobalUserItem.bPrivateGoldRoom then
        	if viewid ~= cmd.MY_VIEWID then
				title:getChildByName("score"..i):setString("")
			else
				title:getChildByName("jushu"):setPositionX(650/4)
				title:getChildByName("score"..i):setPositionX(650*3/4)
			end
		end
	 end

	 local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(sender:getTag(), sender)
        end
     end
	 
	 function onSelectEvent(sender,eventType) 
	 	local tag=sender:getTag()
	 	bg:getChildByTag(tag_wanfa):setSelected(tag==tag_wanfa)
	 	bg:getChildByTag(tag_liushui):setSelected(tag==tag_liushui)
	 	self.view_wanfa:setVisible(tag==tag_wanfa)
	 	self.zhanjiView:setVisible(tag==tag_liushui)
	 	self.liushuiTitle:setVisible(tag==tag_liushui)
	 end

	 check_wanfa:addEventListener(onSelectEvent)
	 check_liushui:addEventListener(onSelectEvent)
	 check_wanfa:setSelected(true)
	 check_liushui:setSelected(false)



	 local function numberOfCellsInTableView()
            --return #tabScoreList --test
        local n=0
        if self._scene.zhanjiRecord then
        	n=#self._scene.zhanjiRecord
        end
        return math.min(math.max(0,PriRoom:getInstance().m_tabPriData.dwPlayCount-1),n)
    end

    local function cellSizeForTable(view,idx) 
        return cellWidth,60
    end

     LogAsset:getInstance():logData("cjson.encode(self._scene.zhanjiRecord)")
    LogAsset:getInstance():logData(cjson.encode(self._scene.zhanjiRecord))

    local function tableCellAtIndex(view, idx) --idx从0开始
    	tabScoreList=self._scene.zhanjiRecord or {}
    	
        local tabScore=tabScoreList[idx+1]
        -- if nil==tabScore and idx+1<PriRoom:getInstance().m_tabPriData.dwPlayCount then
        -- 	self._scene:sendZhanjiRequest(idx+1,idx+1+7)
        -- end

        local cell=view:dequeueCell()
        if nil==cell then
            cell=cc.TableViewCell:create()
        end
        if nil==tabScore then  cell:removeAllChildren()  return cell end
        if cell:getChildByTag(1)==nil and tabScore~=nil then
            cc.CSLoader:createNode("game/Node_Zhanji.csb")
                :addTo(cell)
                :setTag(1)
                :setPosition(9,5)
        end
        
        local node=cell:getChildByTag(1)
        node:getChildByName("jushu"):setPositionX(0.5*scoreWidth):setTextColor(cc.c4b(0xbe,0x93,0xdb,0xff))
        node:getChildByName("jushu"):setString(string.format("%d/%d",idx+1,PriRoom:getInstance().m_tabPriData.dwDrawCountLimit))

        if GlobalUserItem.bPrivateGoldRoom then
        	node:getChildByName("jushu"):setString(string.format("%d", idx+1))
        end

        local j=1
        for i=1,cmd.GAME_PLAYER do
        	local viewid=self._scene:SwitchViewChairID(i-1)
        	if nil~=self.tabUserItem[viewid] then
        		j=j+1
	            local score=tabScore[viewid]
	            if viewid==cmd.MY_VIEWID then
		            if score>=0 then 
		                score="+"..score 
		                node:getChildByName("score"..i):setTextColor(cc.c4b(0x3f,0xdf,0xa8,0xff))
		            else 
		                node:getChildByName("score"..i):setTextColor(cc.c4b(0xfc,0x57,0x7e,0xff))
		            end
		        else
		        	 node:getChildByName("score"..i):setTextColor(cc.c4b(0xbe,0x93,0xdb,0xff))
		        end
		        	score=string.EllipsisByConfig(score, scoreWidth-5, string.getConfig("fonts/round_body.ttf", 22))
		            node:getChildByName("score"..i):setString(score)
		            node:getChildByName("score"..i):setPositionX((j-0.5)*scoreWidth)
            else 
            	node:getChildByName("score"..i):setString("") 
            end

            if GlobalUserItem.bPrivateGoldRoom then 
            	if viewid ~= cmd.MY_VIEWID then
					node:getChildByName("score"..i):setString("")
				else
					node:getChildByName("jushu"):setPositionX(650/4)
					node:getChildByName("score"..i):setPositionX(650*3/4)
				end
			end
        end
    
        return cell
     end

     

    local tableView=cc.TableView:create(content:getContentSize())
    tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(content:getPosition())
    tableView:setDelegate()
    tableView:registerScriptHandler(numberOfCellsInTableView,cc.NUMBER_OF_CELLS_IN_TABLEVIEW)  
    tableView:registerScriptHandler(cellSizeForTable,cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex,cc.TABLECELL_SIZE_AT_INDEX)
    tableView:reloadData()
    tableView:setVerticalFillOrder(0)
    tableView:addTo(bg)
    tableView:setVisible(false)
    self.zhanjiView=tableView

    self.view_wanfa=bg:getChildByName("wanfa") 

	self.view_wanfa:getChildByName(self._scene.cardNumLimit==4 and "CheckBox_1" or "CheckBox_2"):setSelected(true)
	self.view_wanfa:getChildByName("btn_dismiss"):setTag(TagEnum.BTN_DISMISS)
 	:addTouchEventListener(btnEvent)

 	if GlobalUserItem.bPrivateGoldRoom then
        self.view_wanfa:getChildByName("btn_dismiss"):setVisible(false)
    end
end

function GameViewLayer:createZhanjiView2()

    if self.zhanjiLayer and not tolua.isnull(self.zhanjiLayer) then 
    	self.zhanjiLayer:setVisible(true)
    	return true
    end


    local zhanjiLayer=cc.CSLoader:createNode("game/ZhanjiLayer.csb")
    ExternalFun.registerTouchEvent(zhanjiLayer,true)
    self.zhanjiLayer=zhanjiLayer
    zhanjiLayer:addChild(cc.LayerColor:create(cc.c4b(30, 30, 30, 200), 1334, 750),-1)
    zhanjiLayer:addTo(self,Zorder_ZhanjiLayer)

    zhanjiLayer.onTouchBegan=function() return true end

     local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(sender:getTag(), sender)
        end
    end
    zhanjiLayer:getChildByName("btn_dismiss"):setTag(TagEnum.BTN_DISMISS)
 	:addTouchEventListener(btnEvent)

    local bg=zhanjiLayer:getChildByName("bg")
    bg:getChildByName("btn_close"):addClickEventListener(
    	function()
    		self.zhanjiView=nil
    		zhanjiLayer:removeSelf()
    	end
    	)    
    local title=bg:getChildByName("title")
	title:getChildByName("jushu"):setString("局数")
	for i=1,cmd.GAME_PLAYER do --chairid
		local viewid=self._scene:SwitchViewChairID(i-1)
		if self.tabUserItem[viewid] then
			local strNickname = string.EllipsisByConfig(self.tabUserItem[viewid].szNickName, 80, 
                                            string.getConfig("fonts/round_body.ttf", 20))
			title:getChildByName("score"..i):setString(strNickname)
			if viewid==cmd.MY_VIEWID then  
				--title:getChildByName("score"..i):setFontSize(24)
				title:getChildByName("score"..i):setTextColor(cc.c4b(0xdc,0x8f,0xb6,0xff))
			end

			if GlobalUserItem.bPrivateGoldRoom and viewid ~= cmd.MY_VIEWID then
				title:getChildByName("score"..i):setString("")
			end
		else
			--local strNickname = string.EllipsisByConfig("啊as对方答复撒范德萨发", 80, 
                                            --string.getConfig("fonts/round_body.ttf", 20)) --test
			--title:getChildByName("score"..i):setString(strNickname) --test
			title:getChildByName("score"..i):setString("")
		end

		if GlobalUserItem.bPrivateGoldRoom and viewid ~= cmd.MY_VIEWID then
			title:getChildByName("score"..i):setString("")
		end
	end

    local function numberOfCellsInTableView()
            --return #tabScoreList --test
        return math.max(PriRoom:getInstance().m_tabPriData.dwPlayCount-1,0)
    end

    local function cellSizeForTable(view,idx) 
        return 596,60
    end

    local function tableCellAtIndex(view, idx) --idx从0开始
    	tabScoreList=self._scene.zhanjiRecord or {}
        local tabScore=tabScoreList[idx+1]
        if nil==tabScore and idx+1<PriRoom:getInstance().m_tabPriData.dwPlayCount then
        	self._scene:sendZhanjiRequest(idx+1,idx+1+7)
        end
        local cell=view:dequeueCell()
        if nil==cell then
            cell=cc.TableViewCell:create()
        end
        if nil==tabScore then return cell end
        if cell:getChildByTag(1)==nil and tabScore~=nil then
            cc.CSLoader:createNode("game/Node_Zhanji.csb")
                :addTo(cell)
                :setTag(1)
                :setPosition(9,5)
        end
        
        local node=cell:getChildByTag(1)
        node:getChildByName("jushu"):setString(string.format("%d/%d",idx+1,PriRoom:getInstance().m_tabPriData.dwDrawCountLimit))
        if GlobalUserItem.bPrivateGoldRoom then
        	node:getChildByName("jushu"):setString(string.format("%d", idx+1))
        end
        for i=1,cmd.GAME_PLAYER do
        	local viewid=self._scene:SwitchViewChairID(i-1)
        	if nil~=self.tabUserItem[viewid] then
	            local score=tabScore[viewid]
	            if score>=0 then 
	                score="+"..score 
	                node:getChildByName("score"..i):setTextColor(cc.c4b(0x59,0xd8,0xad,0xff))
	            else 
	                node:getChildByName("score"..i):setTextColor(cc.c4b(0xff,0x68,0x54,0xff))
	            end
	            node:getChildByName("score"..i):setString(score)
            else 
            	node:getChildByName("score"..i):setString("") 
            end
            if GlobalUserItem.bPrivateGoldRoom and viewid ~= cmd.MY_VIEWID then
				node:getChildByName("score"..i):setString("")
			end
        end
    
        return cell
     end

    local tableView=cc.TableView:create(cc.size(596,450))
    tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(cc.p(100,0))
    tableView:setDelegate()
    tableView:registerScriptHandler(numberOfCellsInTableView,cc.NUMBER_OF_CELLS_IN_TABLEVIEW)  
    tableView:registerScriptHandler(cellSizeForTable,cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex,cc.TABLECELL_SIZE_AT_INDEX)
    tableView:reloadData()
    tableView:setVerticalFillOrder(0)
    tableView:addTo(bg)
    self.zhanjiView=tableView
end

function GameViewLayer:refreshZhanjiView( )
	if self.zhanjiView and not tolua.isnull(self.zhanjiView) then
		self.zhanjiView:reloadData()
	end
end

--发牌
function GameViewLayer:sendCards(startViewId,tabCardData,sice,tableCardCount) 
	for i=1,cmd.GAME_PLAYER do
		self:removeActionSps(i)
	end
	
	local tabAction={}
	table.insert(tabAction,cc.CallFunc:create(
		function()
			self:showDice(sice[1],1)
			self:showDice(sice[2],2)
		end
		))
	table.insert(tabAction,cc.DelayTime:create(2))
	table.insert(tabAction,cc.CallFunc:create(function() self:updateTableCards(tableCardCount) end))
	table.insert(tabAction,cc.DelayTime:create(0.2))

	self.myCardData=tabCardData[cmd.MY_VIEWID]

	local viewid=startViewId
	local function nextViewId(id)
		id=id+1>cmd.GAME_PLAYER and 1 or id+1
		-- while self.tabPlayStatus[id]<cmd.PS_SITTING then
		-- 	id=id+1>cmd.GAME_PLAYER and 1 or id+1
		-- end
		return id
	end
	self.leftCard=tableCardCount or 32
	local function sendCardToPlayer(sendRound,viewid,cardvalue) --一次发一张
		return function()
			if cardvalue then
				path=string.format("card/%d.png",cardvalue)
			else 
				path=sendRound>2 and "game/backvertical.png" or "game/backvertical.png"  --"game/backhorizontal.png"
			end
			print("self.leftCard:",self.leftCard)
			self.tableCards[self.leftCard]:runAction(cc.Sequence:create(
				cc.MoveTo:create(0.1,posCards[viewid][sendRound]),
				 cc.CallFunc:create(function() self:createCardNode(viewid,sendRound,cardvalue) 
				 		ExternalFun.playSoundEffect("SEND_CARD.wav")
				 	end),
				cc.Hide:create()))

			self.leftCard=self.leftCard-1
			
		end
	end

	
	print("self._scene.cardNumLimit:",self._scene.cardNumLimit)
	local cn=0
	for i=1,self._scene.cardNumLimit do
		for j=1,cmd.GAME_PLAYER do
			if self.tabPlayStatus[viewid]==cmd.PS_SITTING then
				table.insert(tabAction,cc.CallFunc:create(sendCardToPlayer(i,viewid, tabCardData[viewid][i])))  --viewid==cmd.MY_VIEWID and mycards[i] or nil)))
				table.insert(tabAction,cc.DelayTime:create(0.1))
				cn=cn+1
			end
			viewid=nextViewId(viewid)
		end
	end
	table.insert(tabAction,cc.CallFunc:create(function() 

		if self._scene.cardMsgTime then
			self.cardAnimDt=(currentTime()-self._scene.cardMsgTime)/1000
		else
			self.cardAnimDt=0
		end

		if self.tabPlayStatus[cmd.MY_VIEWID]==cmd.PS_SITTING then
			self._scene:SetOpenCardClock(self.cardAnimDt) 
		end

		self:showActionSps(cmd.actCombine)
		self.bSendCardFinished=true

		if self.bGameEnd and not GlobalUserItem.bVideo then
			self:dealWithGameEnd()
		end
	end))
	-- table.insert(tabAction,cc.DelayTime:create(0.4))
	self:runAction(cc.Sequence:create(tabAction))
end

function GameViewLayer:setShowZupaiFinishedSp(viewid,bShow)
	if viewid==cmd.MY_VIEWID then return end
	self.tabZupaiedSp=self.tabZupaiedSp or {}
	if self.tabZupaiedSp[viewid] and not tolua.isnull(self.tabZupaiedSp[viewid] ) then
		self.tabZupaiedSp[viewid]:removeSelf()
	end
	if bShow then
		self.tabZupaiedSp[viewid] =cc.Sprite:create("game/zupaied.png"):addTo(self)
		local bRight,bTop=self:getDir(viewid)
		local x,y=(posCards[viewid][1].x+posCards[viewid][3].x)/2,posCards[viewid][1].y-90
		if bTop then y=y-39 end
		if bRight and self._scene.cardNumLimit==2 then
			x=x-40
		end
		self.tabZupaiedSp[viewid]:setPosition(cc.p(x,y))--:setOpacity(0)
		--self.tabZupaiedSp[viewid]:runAction(cc.FadeTo:create(0.6,255))
		self:showWithAction(self.tabZupaiedSp[viewid])
	end
end

function GameViewLayer:showHandCard(viewid,cards)
	if self.tabPlayStatus[viewid]<cmd.PS_SITTING then return end
	if viewid==cmd.MY_VIEWID and self.bZupaiFinished then return end
	for i=1,#self.tabHandCardNode[viewid] do
		print("viewid,i",viewid,i)
		local card=self.tabHandCardNode[viewid][i]
		if viewid==cmd.MY_VIEWID then
			card:loadTexture(string.format("card/%d.png",cards[i]))
		else
			if cards then card:setTexture(string.format("card/%d.png",cards[i])) end
		end
		if i<=2 then card:setRotation(90) end

		card:setPosition(posCards[viewid][i])
	end
end

function GameViewLayer:priGameLayerZorder()
	return Zorder_PriGameLayer
end

function GameViewLayer:setGameInfo(bBigPaiJiu)
	self.gameinfoText=self:getChildByName("gameinfo")
	if self.gameinfoText then
		self.gameinfoText:setString(bBigPaiJiu==true and "大牌九" or "小牌九")
	end
end

function GameViewLayer:gameEnd(tabScore,tabCardData,tabWinInfo,endMode)
	self.bGameEnd=true
	self.gameEndParams={tabScore,tabCardData,tabWinInfo,endMode}
	if self.bSendCardFinished or nil==self._scene.cardMsgTime or GlobalUserItem.bVideo then
		self:dealWithGameEnd( )
	end
end

function GameViewLayer:dealWithGameEnd()
	self:endRobAnimation()
	local tabScore,tabCardData,tabWinInfo,endMode=self.gameEndParams[1],self.gameEndParams[2],self.gameEndParams[3],self.gameEndParams[4]
	self.bGameEnd=false
	self.bSendCardFinished=false
	for i=1,cmd.GAME_PLAYER do
		self:removeActionSps(i)
		self:setShowZupaiFinishedSp(i,false)
	end

	if self.tabRobSp then
 		for k,v in pairs(self.tabRobSp) do
 			if not tolua.isnull(v) then
 				v:removeSelf()
 			end
 		end
 	end

	if self.pointSp and self.pointSp[1] then self.pointSp[1]:removeSelf() end
	if self.pointSp and self.pointSp[2] then self.pointSp[2]:removeSelf() end
	self.pointSp=nil
	self:hideAllButtons()
	
	if self._chipLayer and not tolua.isnull(self._chipLayer) then
		self._chipLayer:removeSelf()
		self._chipLayer=nil

	end

	for i=1,cmd.GAME_PLAYER do
		if self.tabPlayStatus[i]==cmd.PS_SITTING then
			self:showHandCard(i,tabCardData[i])
		end
	end

	-- local time1=1 --胜败图出现时间
	-- local time2=2 --输赢金额出现时间

	local time1=2 --金币飞舞时间
	local time2=1 --胜败图片
	local time3=1 --飘分

	self.readyBtn:runAction(cc.Sequence:create(cc.DelayTime:create(time1+time2+time3+0.5),
		cc.CallFunc:create(function() 
			self:setShowReadyBtn(true)
			if self._scene.endMsgTime then
				self.endAnimDt=(currentTime()-self._scene.endMsgTime)/1000
			else
				self.endAnimDt=0
			end
			self._scene:setReadyClock(self.endAnimDt)

			for i=1,cmd.GAME_PLAYER do
				if self.tabHeadSp[i] and not tolua.isnull(self.tabHeadSp[i] ) and not GlobalUserItem.bVideo then
					print("1487 i "..i)
					self:showActionSps(0,i)
				end
			end

		end)
		))

	self.bZupaiFinished=true
	if self._EndLayer==nil or tolua.isnull(self._EndLayer) then
		self._EndLayer=cc.Layer:create():addTo(self,Zorder_EndLayer)
	end

	local function playCoinFlyAnimation(posA,posB,delay,n)--从posA飞到posB

		local function getMoveAction(time,startPoint, endPoint,height,angle)
		   	--把角度转换为弧度
		    angle = angle or 90
		    height = height or 50
		    local radian = angle*3.14159/180.0
		    --第一个控制点为抛物线左半弧的中点  
		    local q1x = startPoint.x+(endPoint.x - startPoint.x)*0.25;  
		    local q1 = cc.p(q1x, height + startPoint.y+math.cos(radian)*q1x);         
		    -- 第二个控制点为整个抛物线的中点  
		    local q2x = startPoint.x + (endPoint.x - startPoint.x)*0.5;  
		    local q2 = cc.p(q2x, height + startPoint.y+math.cos(radian)*q2x);  

		    local q3x = startPoint.x + (endPoint.x - startPoint.x)*0.75;  
		    local q3 = cc.p(q2x, height + startPoint.y+math.cos(radian)*q2x);  
		    --曲线配置  
		    local bezier = {
		        q1,
		        q3,
		        endPoint
		    }
		    --使用EaseInOut让曲线运动有一个由慢到快的变化，显得更自然  
		    local beaction = cc.BezierTo:create(time, bezier)
		    --if inorout == 0 then
		    local easeoutaction =  cc.EaseInOut:create(beaction,9.5) --cc.EaseOut:create(beaction, 9.5) --EaseInOut
		    return easeoutaction
		    --else
		        --return cc.EaseIn:create(beaction, 1)
		    --end
		end

			--if math.abs(posA.x-posB.x)<10 then
		-- for jth=1,3 do
			
		-- 	posB=cc.p(posB.x+(jth-2)*50,posB.y+(jth-2)*50)
			
		-- 	n=n or 50
		-- 	for i=1,n do
		-- 		local tabAct={}
		-- 		table.insert(tabAct,cc.DelayTime:create(math.random()*0.3+0.5))
		-- 		table.insert(tabAct,cc.FadeTo:create(0.05,255))
		-- 		table.insert(tabAct,getMoveAction(1,posA,posB))
		-- 		table.insert(tabAct,cc.DelayTime:create(0.2))
		-- 		table.insert(tabAct,cc.RemoveSelf:create())  

		-- 		local sp=cc.Sprite:create("game/flycoin.png"):addTo(self._EndLayer):setPosition(posA):setOpacity(0):setScale(0.75)
		-- 		sp:runAction(cc.Sequence:create(tabAct))
		-- 	end
		-- end

		local speed=2239.516
		n=n or 50
		local a,b=10,15
		for i=1,n do
			adjPosA=cc.p(posA.x+math.random(-a,a),posA.y+math.random(-a,a))
			adjPosB=cc.p(posB.x+math.random(-b,b),posB.y+math.random(-b,b))
			local dis=cc.pGetDistance(adjPosA,adjPosB)
			local tabAct={}
			local t=math.floor(i/3)
			table.insert(tabAct,cc.DelayTime:create(4*(t/n)))--math.random()*0.3))
			table.insert(tabAct,cc.FadeTo:create(0.1,255))
			local mov=cc.MoveTo:create(dis/speed,adjPosB)
			table.insert(tabAct, mov)--cc.EaseIn:create(mov,2))--getMoveAction(1.5,posA,posB))
			table.insert(tabAct,cc.DelayTime:create(2*(1-t/n)))
			table.insert(tabAct,cc.RemoveSelf:create())

			local sp=cc.Sprite:create("game/flycoin.png"):addTo(self,Zorder_Flycoin):setPosition(adjPosA):setOpacity(0):setScale(0.75)
			sp:runAction(cc.Sequence:create(tabAct))
			sp:setTag(tag_flycoin)
		end
	end

	local function formatScore(score) --+(-)替换成. ,替换成/          ./0的string.byte()值依次递增1
		local str=""
		if score==0 then return ".0" end 
		if score<0 then score=-score end
		while score>0 do
			if score>=1000 then
				local r=score%1000
				if r<10 then r="00"..r elseif r<100 then r="0"..r end
				str="/"..r..str
				score=math.floor(score/1000)
			else
				str=score..str
				break
			end
		end
		str="."..str
		return str
	end

	--显示输赢分数
	--显示和/赢/输 图片
	for i=1,cmd.GAME_PLAYER do --viewid
		if self.tabPlayStatus[i]==cmd.PS_SITTING then
			print("i:",i)
			assert(tabCardData[i][1])
			assert(tabCardData[i][2])

			local tabAnchorX={0,1,1,0,0,0}
			if self._scene.wPlayerCount==4 then
				tabAnchorX={0,1,0,0}
			end
			local filepath="game/winscore_digits.png"
			if tabScore[i]<0 then filepath="game/lostscore_digits.png" end
			local spawn=cc.Spawn:create(cc.MoveBy:create(time3*0.2,cc.p(0,12)),cc.FadeTo:create(time3*0.2,100))
			cc.LabelAtlas:create(formatScore(tabScore[i]),filepath,30,44,string.byte('.')):addTo(self._EndLayer)
				:setPosition(posScore[i]):setAnchorPoint(cc.p(tabAnchorX[i],0.5)):setOpacity(0)
				:runAction(cc.Sequence:create(cc.DelayTime:create(time1+time2+0.2),cc.FadeIn:create(0.01),cc.MoveBy:create(time3*0.8,cc.p(0,48)),spawn,cc.RemoveSelf:create()))

			if endMode==cmd.GER_NORMAL and self.tabHandCardNode[i] and #self.tabHandCardNode[i]>0 then --已发牌

				if self._scene.cardNumLimit==4 then
					if self._RectLayer==nil or tolua.isnull(self._RectLayer) then
						self._RectLayer=cc.Layer:create():addTo(self,Zorder_RectLayer)
					end
					for i=1,cmd.GAME_PLAYER do
						if self.tabPlayStatus[i]==cmd.PS_SITTING and i~=self.viewBanker then
							for j=1,2 do
								local path=tabWinInfo[i][j]==true and "game/rectwin.png" or "game/rectlose.png"
								local scale= i==cmd.MY_VIEWID and 1.05 or (cardScale+0.03)
								cc.Sprite:create(path):addTo(self._RectLayer):setPosition(posRect[i][j]):setScale(scale)
							end
						end
					end
				end

				--点数
				local t1={tabCardData[i][1],tabCardData[i][2]}
				local t2={tabCardData[i][3],tabCardData[i][4]}

				local function func_(t,ith)
					print("ith:",ith)
					assert(t[1])
					assert(t[2])
					local bCardType =GameLogic.GetCardType(t, 2)
					local bCardPoint =-1
					if (GameLogic.CT_P9_NORMAL == bCardType) then
						 bCardPoint = GameLogic.GetCardPoint(t, 2)
						 filepath=string.format("game/point%d.png",bCardPoint)
					else
						filepath=string.format("game/type%d.png",bCardType)
					end
					local sp=cc.Sprite:create(filepath):addTo(self._EndLayer):setPosition(posCardPoint[i][ith])
					self:showWithAction(sp)
				end
				func_(t1,1)
				if self._scene.cardNumLimit==4 then
					func_(t2,2)
				end
			end

			if i~=self.viewBanker and tabScore[i]~=0 then
				local posA,posB=posHead[i],posHead[self.viewBanker]
				if tabScore[i]>0 then posA,posB=posHead[self.viewBanker],posHead[i] end
				playCoinFlyAnimation(posA,posB,0)
			end

			local soundPath
			if tabScore[i]<0 then
				filepath="game/baiju.png"
				soundPath="GAME_LOST.wav"
			elseif tabScore[i]==0 then
				filepath="game/heju.png"
				soundPath="GAME_END.wav"
			else
				filepath="game/shengju.png"
				soundPath="GAME_WIN.wav"
			end

			if i==cmd.MY_VIEWID then
				ExternalFun.playSoundEffect(soundPath)
			end

			local scaleBy=cc.ScaleBy:create(time2/2,1.5)
			local sp=cc.Sprite:create(filepath):addTo(self._EndLayer):setPosition(posResultSp[self._scene.cardNumLimit>2 and 1 or 2][i])--:setOpacity(0)
			-- if tabScore[i]<=0 then
			-- 	sp:runAction(cc.FadeTo:create(time2,255))
			-- else
			-- 	sp:runAction(cc.Sequence:create(cc.DelayTime:create(time1) ,cc.Spawn:create(cc.FadeIn:create(time2/2),cc.EaseOut:create(scaleBy,1.1)),cc.EaseOut:create(scaleBy:reverse(),1.1)))
			
			--local jmp=cc.JumpBy:create(time2/2,cc.p(0,20),20,1)
			--sp:runAction(cc.Sequence:create(cc.DelayTime:create(time1) ,jmp,jmp:reverse()))
			sp:setScale(0.00001)
			sp:runAction(cc.Sequence:create(cc.DelayTime:create(time1) ,cc.ScaleTo:create(0.1,1)))
			--self:showWithAction(sp)
			--end
		end
	end

	--显示点数
	--显示
end

function GameViewLayer:popMyCard(index,bDownDir)
	if not self._scene.bTrustee then
		if self.bZupaiFinished then return end
	end

	local cardNode=self.tabHandCardNode[cmd.MY_VIEWID][index]

	if not cardNode then
		return;
	end

	if bDownDir==true then
		self.bMyCardPoped[index]=false
		if cardNode.dir<0 then
			cardNode:setPositionY(cardNode:getPositionY()+cardNode.dir*cardNode.popDis)
			cardNode.dir=-cardNode.dir
		end
	else
		self.bMyCardPoped[index]=cardNode.dir>0
		cardNode:setPositionY(cardNode:getPositionY()+cardNode.dir*cardNode.popDis)
		cardNode.dir=-cardNode.dir
	end
	if self._scene.cardNumLimit==4 then
		local pn=0
		for i=1,4 do
			pn=pn+(self.bMyCardPoped[i] and 1 or 0)
		end
		self.zupaiBtn:setVisible(pn==2)
	end
	self:showPoints()
end

function GameViewLayer:createHelpLayer()
	local helpLayer=cc.CSLoader:createNode("game/HelpLayer.csb")
	ExternalFun.registerTouchEvent(helpLayer,true)
	helpLayer:addTo(self,Zorder_HelpLayer)
	helpLayer:addChild(cc.LayerColor:create(cc.c4b(30, 30, 30, 200), 1334, 750),-1)
	local bg=helpLayer:getChildByName("bg")
	local fileNode=bg:getChildByName("FileNode"):setVisible(false)
	bg:getChildByName("btn_close"):addClickEventListener(function() helpLayer:removeSelf() end)
	 helpLayer:getChildByName("bt_next"):setVisible(false)
	 --addClickEventListener(function() 
	-- 	self.helpListView:setVisible(not self.helpListView:isVisible())
	-- 	fileNode:setVisible(not fileNode:isVisible())
	--   end) 

	helpLayer.onTouchBegan=function() return true end

	local tabCardValue={{},{},{0x11,0x16},{0x66,0x25},{0x11,0x35},{0x66,0x35},{0x11,0x36},{0x66,0x36},   --GameLogic
	 {0x36,0x45},{0x35,0x26},{0x34,0x25},{0x32,0x14},{0x46,0x46},{0x56,0x56},{0x15,0x15},{0x16,0x16},
	 {0x22,0x22},{0x33,0x33},{0x55,0x55},{0x13,0x13},{0x44,0x44},{0x11,0x11},{0x66,0x66},{0x24,0x12} }
	local tabStr={"","","地高九","天高九","地杠","天杠","地王","天王",
				"杂九","杂八","杂七","杂五","双红头","双斧头","双零霖","双高脚",
				"双板凳","双长三","双梅","双鹅","双人","双地","双天","至尊宝"}

	-- local noBigSign={}
	-- local tempT={3, 10,11,12, 14,15,16, 18,19}
	-- for k,v in ipairs(tempT) do
	-- 	noBigSign[v]=true
	-- end
	-- for i=3,24 do
	-- 	local node=fileNode:getChildByName("FileNode_"..i)
	-- 	node:getChildByName("name"):setString(tabStr[i])
	-- 	node:getChildByName("card1"):setTexture(string.format("card/%d.png",tabCardValue[i][1]))
	-- 	node:getChildByName("card2"):setTexture(string.format("card/%d.png",tabCardValue[i][2]))
	-- 	if noBigSign[i] then node:getChildByName("bigsign"):setVisible(false) end
	-- end
	-- fileNode:setVisible(true)

	local tabStr2=
	 {
	 "至尊宝：歇后语“丁三配二四--- 绝配”由此而来，特点是点数3 + 6 = 9",
	 "双天：由两张天牌组成",
	 "双地：由两张地牌组成",
	 "双人：由两张人牌组成",
	 "双鹅：由两张鹅牌组成",
	 "双梅：由两张梅花牌组成",
	 "双长三：由两张长三牌组成",
	 "双板凳：由两板凳牌组成",
	 "双斧头：由两张斧头牌组成",
	 "双红头：由两张红头牌组成",
	 "双高脚：由两张高脚牌组成",
	 "双零霖：由两张零六牌组成",
	 "杂九：由两张杂九牌组成",
	 "杂八：由两张杂八牌组成",
	 "杂七：由两张杂七牌组成",
	 "杂五：由两张杂五牌组成",
	 "天王：天牌配合任何一种9点的牌，即天牌配杂九牌，特点是点数为12+9=21，算个位数是1",
	 "地王：地牌配合任何一种9点的牌，即地牌配杂九牌，特点是点数为2+9=1，算个位数是1",
	 "天杠：天牌配合任何一种8点的牌，即天牌配人牌或者杂八牌，特点是点数为12+8=20，算个位数是0",
	 "地杠：地牌配任何一种8点的牌，即地牌配人牌或杂八牌，特点是点数为2+8=10，算个位数是0",
	 "天高九：天牌配杂七中的二五牌，特点是点数为12+7=19，算个位数是9",
	 "地高九：地牌配合高脚七牌，特点是点数为2+7=9，算个位数是9"
	}

	local cellH=90

	local function numberOfCellsInTableView()
		print("#tabStr2",#tabStr2)
		return #tabStr2+1
	end

	local function cellSizeForTable(view,idx)
		if idx==0 then return bg:getContentSize().width,577 end
		return bg:getContentSize().width-50,cellH
	end

	local function tableCellAtIndex(view, idx)
		local cell = view:dequeueCell()
	    if not cell then        
	        cell = cc.TableViewCell:new()
	    else
	        cell:removeAllChildren()
	    end
	    if idx==0 then
	    	cc.Sprite:create("game/sortImg.png"):addTo(cell):setPosition(cc.p(400,270)):setScale(1.4)
	    else
		    local w,h=37,70
		    cc.Sprite:create(string.format("card/%d.png",tabCardValue[3+idx-1][1])):addTo(cell):setPosition(w/2,cellH/2):setScale(0.5)
		    cc.Sprite:create(string.format("card/%d.png",tabCardValue[3+idx-1][2])):addTo(cell):setPosition(1.5*w+3,cellH/2):setScale(0.5)
		    cc.Label:createWithTTF(tabStr2[#tabStr2-(idx-1)], "fonts/round_body.ttf", 24, cc.size(695,0), cc.TEXT_ALIGNMENT_LEFT)
		    :setPosition(2*w+10,cellH/2)
		    :setAnchorPoint(cc.p(0, 0.5))
		    :addTo(cell)
		end
	    return cell
	end

	self.helpListView = cc.TableView:create({width=850,height=cellH*5})
    self.helpListView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self.helpListView:setPosition(cc.p(50,67))
    self.helpListView:setDelegate()
    self.helpListView:addTo(bg)
    self.helpListView:registerScriptHandler( cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    self.helpListView:registerScriptHandler(tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
    self.helpListView:registerScriptHandler(numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self.helpListView:reloadData()
end

function GameViewLayer:switchBtnChange()
	if self._scene.m_cbGameStatus == cmd.GAME_STATUS_FREE then
		self.btnChangeMenu:setVisible(not GlobalUserItem.bPrivateRoom)
	else
		self.btnChangeMenu:setVisible(false)
	end
end

function GameViewLayer:onButtonClickedEvent(tag,btn)
	if btn==nil then
		btn=self:getChildByTag(tag)
	end
	if tag==TagEnum.BTN_SET then
		SettingLayer:create( self ):addTo(self,Zorder_SettingLayer)
		self.setNode:setVisible(false)
		self.menuBtn:loadTextureNormal("game/menudown.png")
	elseif tag==TagEnum.BTN_HELP then
		self:createHelpLayer()
		-- self._scene._scene:popHelpLayer2(cmd.KIND_ID, 0)
		self.setNode:setVisible(false)
		self.menuBtn:loadTextureNormal("game/menudown.png")
	elseif tag==TagEnum.BTN_DISMISS then
		if self.zhanjiLayer and not tolua.isnull(self.zhanjiLayer) then 
			self.zhanjiLayer:removeSelf()
		end
		PriRoom:getInstance():queryDismissRoom()
	elseif tag==TagEnum.BTN_EXIT then
		self.setNode:setVisible(false)
		self.menuBtn:loadTextureNormal("game/menudown.png")

		--判断自己是否在游戏中
		if self._scene.cbPlayStatus and  self._scene.cbPlayStatus[self._scene:GetMeChairID()+1] ~= cmd.PS_SITTING then
			self._scene.m_cbGameStatus=cmd.GAME_STATUS_FREE;
		end

		self._scene:onQueryExitGame()
		
	elseif tag==TagEnum.BTN_CHAT then
		self._chatLayer:showGameChat(true)
	elseif  tag == TagEnum.BT_AUTO then
		--自动托管
		self:onButtonClickedEvent(TagEnum.BTN_TRUST)

		if self._scene.bTrustee then
			self._scene:sendTrustee(false)
		else
			self._scene:sendTrustee(true)
		end
	elseif tag==TagEnum.BTN_TRUST then
		self.setNode:setVisible(false)
		-- self._scene:setTrustee()
	elseif tag==TagEnum.BTN_READY then
		self._scene:SendUserReady()
		self:removeExitedUsers()
		self:updateTableCards(0)
		-- btn:setVisible(false)
		self:setShowReadyBtn(false)
		self:resetData(true)
		self._scene.bEnd=false

	elseif tag==TagEnum.BTN_ROB or tag==TagEnum.BTN_NOROB then
		self._scene:sendRobBanker(tag==TagEnum.BTN_ROB)
		self:setShowRobBtn(false)
	elseif tag>TagEnum.BTN_CHIP and tag<TagEnum.BTN_CHIP+6 then --temp 6
		if self.viewBanker~=cmd.MY_VIEWID then self._scene:sendChip(self.tabChipNum[tag-TagEnum.BTN_CHIP]) end
		self.chipNode:setVisible(false)
	elseif tag==TagEnum.BTN_NOTIFY then
		local tabIndex=GameLogic.combine(self.myCardData)
		for i=1,self._scene.cardNumLimit do
			self:popMyCard(i,true)
		end
		self:popMyCard(tabIndex[1])
		self:popMyCard(tabIndex[2])
		self:showPoints(self.myCardData,tabIndex)
		self.zupaiBtn:setVisible(true)
		self.cancelBtn:setVisible(true)
		self.notifyBtn:setVisible(false)
	elseif tag==TagEnum.BTN_CANCEL then
		for i=1,4 do
			self:popMyCard(i,true)
		end
		self:showPoints()
		self.zupaiBtn:setVisible(false)
		self.cancelBtn:setVisible(false)
		self.notifyBtn:setVisible(true)
	elseif tag==TagEnum.BTN_LOCATION then
		self:showAdressLayer()
	elseif tag==TagEnum.BTN_CHAT then
		self._chatLayer:showGameChat(true)
	elseif tag==TagEnum.BTN_ZUPAI then
		
		if self._scene.cardNumLimit<=2 then

			--托管时不发消息
			if not self._scene.bTrustee then
				self._scene:sendOpen({self.myCardData[1],self.myCardData[2]})
			end
			
			self.tabHandCardNode[cmd.MY_VIEWID][1]:setPosition(posCards[cmd.MY_VIEWID][1]):setRotation(90)
			self.tabHandCardNode[cmd.MY_VIEWID][2]:setPosition(posCards[cmd.MY_VIEWID][2]):setRotation(90)
			self:setZupaiFinished(true)
			print(posCards[cmd.MY_VIEWID][1].x,posCards[cmd.MY_VIEWID][1].y)
			print(posCards[cmd.MY_VIEWID][2].x,posCards[cmd.MY_VIEWID][2].y)
			return
		end

		--托管时不发消息
		if not self._scene.bTrustee then
			local popN=0
			local comb1={}
			local comb2={}
			for i=1,self._scene.cardNumLimit do
				if self.bMyCardPoped[i] then
					popN=popN+1
					table.insert(comb1,self.myCardData[i])
				else
					table.insert(comb2,self.myCardData[i])
				end
			end
			if popN~=2 then
				showToast(self._cardLayer,"请选择两张牌组合",2)
				return
			end

			self._scene:sendOpen(comb1,comb2)
		end
		
		print("self.name:",self.name)
		dump(self,"self:",10)
		self:setZupaiFinished(true)
		
		--托管时
		-- if self._scene.bTrustee then
		-- 	return;
		-- end

		local k1,k2=1,3
		for i=1,self._scene.cardNumLimit do
			if self.bMyCardPoped[i] then
				if self.tabHandCardNode[cmd.MY_VIEWID][i] then
					self.tabHandCardNode[cmd.MY_VIEWID][i]:setPosition(posCards[cmd.MY_VIEWID][k1]):setRotation(90)
					k1=k1+1
				end
			else
				if self.tabHandCardNode[cmd.MY_VIEWID][i] then
					self.tabHandCardNode[cmd.MY_VIEWID][i]:setPosition(posCards[cmd.MY_VIEWID][k2])
					k2=k2+1
				end
			end
		end
	elseif tag == TagEnum.BTN_CHANGEDESK then
		if GlobalUserItem.bPrivateRoom == true then
			return
		end

		--判断状态
		if self._scene:GetMeUserItem().cbUserStatus >= yl.US_PLAYING then
	        local query = QueryDialog:create("您正在游戏状态无法换桌!", function(ok)
                if ok then
                    return
                end
            end, 30, QueryDialog.QUERY_SURE)
            :setCanTouchOutside(false)
            :addTo(self, GameViewLayer.ZORDER_RESULT)
		else
			self._scene._gameFrame:QueryChangeDesk()

			--1秒只能点击一次换桌
			btn:runAction(cc.Sequence:create(cc.CallFunc:create(function()
				btn:setEnabled(false)
				self.setNode:setVisible(false)
			end), cc.DelayTime:create(1), 
			cc.CallFunc:create(function()
				btn:setEnabled(true)
			end)))
		end
	end
end

function GameViewLayer:showPoints( myCardData,tabIndex)
	if self.pointSp and self.pointSp[1] then self.pointSp[1]:removeSelf() end
	if self.pointSp and self.pointSp[2] then self.pointSp[2]:removeSelf() end
	self.pointSp=nil
	if nil==myCardData then return end
	--点数
	local t1={myCardData[tabIndex[1]],myCardData[tabIndex[2]]}
	local t2={myCardData[tabIndex[3]],myCardData[tabIndex[4]]}
	self.pointSp=self.pointSp or {}
	local function func_(t,ith)
		local bCardType =GameLogic.GetCardType(t, 2)
		local bCardPoint =-1
		if (GameLogic.CT_P9_NORMAL == bCardType) then
			 bCardPoint = GameLogic.GetCardPoint(t, 2)
			 filepath=string.format("game/point%d.png",bCardPoint)
		else
			filepath=string.format("game/type%d.png",bCardType)
		end
		print("i,ith:",i,ith,filepath)
		assert(self.pointSp)
		assert(cc.Sprite:create(filepath))
		
		self.pointSp[ith]=cc.Sprite:create(filepath):addTo(self):setPosition(posCardPoint[cmd.MY_VIEWID][ith])
		self:showWithAction(self.pointSp[ith])
	end
	func_(t1,1)
	if self._scene.cardNumLimit==4 then
		func_(t2,2)
	end
end

function GameViewLayer:setMyCardNodeOpen()--假设前两个一组
	for i=1,self._scene.cardNumLimit do
		self.tabHandCardNode[cmd.MY_VIEWID][i]:setPosition(posCards[cmd.MY_VIEWID][i])
		if i<=2 then
			self.tabHandCardNode[cmd.MY_VIEWID][i]:setRotation(90)
		end
	end
end

function GameViewLayer:removeExitedUsers() --游戏开始后离开的玩家
	for viewId=1,cmd.GAME_PLAYER do
		if self.tabUserItem[viewId] then
			if viewId~=cmd.MY_VIEWID and self.tabUserItem[viewId].bExit==true  then
				self.tabHeadSp[viewId]:removeSelf()
				self:removeActionSps(viewId)
				self.tabHeadSp[viewId] = nil  
				self.tabUserItem[viewId] = nil
			end
		end
	end
end


function GameViewLayer:updateClockView(time,viewid)
	self.clock:setVisible(false)
	self.clockTime:setVisible(false)
	if true then return end

	--------------------------------
	if viewid==yl.INVALID_CHAIR then
		self.clock:setVisible(false)
	else
		self.clock:setVisible(true)
		if time and time<10 then
			time="0"..time
		end
		print("time:",time)
		self.clockTime:setString(time or "00")
	end
end

function GameViewLayer:resetUserItem(oldViewId,newViewId )
	--assert(self._scene.m_bOnGame)
	print("oldViewId:",oldViewId)
	print(oldViewId,newViewId)
	if self.tempTabUserItem[oldViewId] then
		self:OnUpdateUser(newViewId,self.tempTabUserItem[oldViewId])
	end
end

function GameViewLayer:OnUpdateUser(viewid, useritem)
	-- local s
	-- if useritem then
	-- 	s="  "..useritem.cbUserStatus
	-- else
	-- 	s=" useritem nil "
	-- end
	-- showToast(self,(viewid or "nil")..s,3)
	--print("viewid",viewid)
	--dump(posHead[viewid],"posHead"..viewid)
	if self._scene.m_bOnGame then
		if not GlobalUserItem.bVideo then
			assert(viewid<=self._scene.wPlayerCount)
		end
	end
	if viewid>6 or viewid<1  then return end

	if not self._scene.m_bOnGame  then
		self.tempTabUserItem=self.tempTabUserItem or {}
		self.tempTabUserItem[viewid]=useritem
		return
	end


	if useritem then
		if self.tabHeadSp[viewid]==nil or tolua.isnull(self.tabHeadSp[viewid]) then

			self.tabHeadSp[viewid]= GameRoleItem:create(useritem,viewid)
			-- print("viewid:",viewid)
			-- print("name:",useritem.szNickName)
			-- print("status:",useritem.cbUserStatus)
			self.tabHeadSp[viewid]:addTo(self,Zorder_Head):setPosition(posHead[viewid])
		else
			self.tabHeadSp[viewid]:updateRole(useritem)
		end
		
		self.tabUserItem[viewid]=useritem
		self.tabUserItem[viewid].bExit=false
		self.tabUserItemCache[viewid]=useritem
	

		local bRoomer =PriRoom and PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == useritem.dwUserID
	    -- 桌主标识
	    if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
	        self.tabHeadSp[viewid]:setRoomerState(bRoomer)
	    end

	else

		if self._scene.bEnd==true and self.tabUserItem[viewid]  then
			self.tabUserItem[viewid].bExit=true
		else
			self.tabUserItem[viewid]=nil
			if self.tabHeadSp[viewid] and not tolua.isnull(self.tabHeadSp[viewid]) then
				self.tabHeadSp[viewid]:removeSelf()
				self:removeActionSps(viewid)
				self.tabHeadSp[viewid]=nil

				--玩家退出时置空
				self.tabUserItemCache[viewid]=nil
			end
		end
	end
	
	self.tabReadySp[viewid]:setPosition(posReadySp[viewid])
	self.tabReadySp[viewid]:setVisible(useritem and useritem.cbUserStatus==yl.US_READY)
	--print("viewid "..viewid,"tabReadySp_pos:",self.tabReadySp[viewid]:getPosition())
	--if useritem then showToast(self,useritem.cbUserStatus.." , "..self._scene.m_cbGameStatus,3) end
	local bStatus=self._scene.m_cbGameStatus2==cmd.GAME_STATUS_FREE or self._scene.m_cbGameStatus2==cmd.SUB_S_GAME_END or self._scene.m_cbGameStatus2==cmd.SUB_S_WINLOSE_INFO
	if not GlobalUserItem.bVideo and bStatus then
		if (useritem and useritem.cbUserStatus==yl.US_READY)  or nil==useritem then 
			--if (useritem and useritem.cbUserStatus==yl.US_READY) then  showToast(self._scene,useritem.szNickName,3) end
			self:removeActionSps(viewid)
		else 
			self:showActionSps(0,viewid)
		end
	end

	-- if nil==useritem then
	-- 	self:removeActionSps(viewid)
	-- end

end

-- function GameViewLayer:showReadySp(viewid,bShow)
-- 	self.tabReadySp[viewid]:setVisible(bShow)
-- end

-- 文本聊天
function GameViewLayer:onUserChat(chatdata, viewId)
    local playerItem = self.tabHeadSp[viewId]
    print("获取当前显示聊天的玩家头像", playerItem, viewId, chatdata.szChatString)
    if nil ~= playerItem then
        playerItem:textChat(chatdata.szChatString)
        --self._chatLayer:showGameChat(false)
    end
end

-- 表情聊天
function GameViewLayer:onUserExpression(chatdata, viewId)
    local playerItem = self.tabHeadSp[viewId]
    if nil ~= playerItem then
        playerItem:browChat(chatdata.wItemIndex)
        --self._chatLayer:showGameChat(false)
    end
end

--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
	--取消文字，表情
	local playerItem = self.tabHeadSp[viewid]
    if nil ~= playerItem then
    	if isPlay then
    		playerItem:onUserVoiceStart()
    	else
    		playerItem:onUserVoiceEnded()
    	end
    end
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = Zorder_Head + 1

        --获取头像坐标
        for k,v in pairs(self.tabUserItem) do
              if v then
                  -- local head = self.nodePlayer[k]:getChildByTag(GameViewLayer.SP_HEAD)
                  if v.dwUserID ==  dwUserID then
                        beginPoint = cc.p(self.tabHeadSp[k]:getPosition())
                  elseif v.dwUserID ==  dwRecvUserID then
                        endPoint = cc.p(self.tabHeadSp[k]:getPosition())
                  end
              end
        end

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


function GameViewLayer:showAdressLayer()

	function  getAngleByTwoPoint( p1,p2)

		local dx=p2.x-p1.x
		local dy=p2.y-p1.y
		if dx==0 then return 90 end
		return math.deg(math.atan(dy/dx))
	end

--self.m_UserHead[viewid].head    self.m_UserHead[viewid].userInfo
	local addressLayer = cc.CSLoader:createNode("info/LocationLayer.csb"):addTo(self, 51)
	local panel1 = addressLayer:getChildByName("Panel_1")
	local panel2 = addressLayer:getChildByName("Panel_3")
	panel1:setVisible(false)
	panel2:setVisible(true)
	self._scene:showPopWait()
  
	for i=1,cmd.GAME_PLAYER do
		if self.tabHeadSp[i] and self.tabHeadSp[i].m_popHead  then
			self.tabHeadSp[i].m_popHead:queryUserLociton(self.tabUserItem[i].dwUserID)
			-- dump(self.m_UserHead[i].userInfo , "玩家视图"..i)
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


		for i=1,cmd.GAME_PLAYER do
			for j=1,cmd.GAME_PLAYER do
				local distanceTxt  = panel1:getChildByName("dis"..i.."_"..j)
				local distanceImg = panel1:getChildByName("xian"..i.."_"..j)
				if distanceTxt then distanceTxt:setVisible(false) end
				if distanceImg then distanceImg:setVisible(false) end
			end
		end
		
		for i=1,cmd.GAME_PLAYER do
			local address = panel1:getChildByName("address"..i)
			address:setVisible(false)
			if self.tabHeadSp[i] and self.tabHeadSp[i].m_popHead then
				address:setVisible(true)
				
				local userInfo = self.tabHeadSp[i].m_popHead.m_userInfo
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

				
				for j=i+1,cmd.GAME_PLAYER do
					local pi=posHead[i]
					local pj=posHead[j]

					local la2 = 0
					local lo2 = 0
					local distanceTxt  = panel1:getChildByName("dis"..i.."_"..j)
					local distanceImg = panel1:getChildByName("xian"..i.."_"..j)
					
					
					if self.tabHeadSp[j] and self.tabHeadSp[j].m_popHead then

						if nil==distanceImg then
							distanceImg=ccui.ImageView:create("info/xian1.png")
							distanceImg:setScale9Enabled(true)
							distanceImg:addTo(panel1):setName("xian"..i.."_"..j)
						end
						if nil==distanceTxt then
							distanceTxt=ccui.Text:create("","fonts/round_body.ttf",20)
							distanceTxt:addTo(panel1):setName("dis"..i.."_"..j)
							distanceTxt:setTextColor(cc.c3b(0xff,0xff,0x0))
						end
						distanceImg:setPosition(cc.pMidpoint(pi,pj))
						distanceImg:setContentSize(cc.pGetDistance(pi,pj)*0.95,distanceImg:getContentSize().height)
						distanceImg:setRotation(-1.359234)
						distanceImg:setRotation(-getAngleByTwoPoint(pi,pj))
						distanceTxt:setPosition(cc.p(distanceImg:getPositionX()+10,distanceImg:getPositionY()+10))
						 
						distanceTxt:setRotation(distanceImg:getRotation())

						local userInfo = self.tabHeadSp[i].m_popHead.m_userInfo
						if userInfo then
							la2  = userInfo.dLatitude
							lo2 = userInfo.dLongitude
						end
						distanceTxt:setVisible(true)
						if true then
							distanceImg:setVisible(true)
						else
							local sprite = cc.Sprite:createWithSpriteFrame(cc.SpriteFrame:create("info/xian1.png",cc.rect(0,0,distanceImg:getTag(),3)))
							sprite:setAnchorPoint(0,0.5)
							sprite:setRotation(distanceImg:getRotationSkewX())
							sprite:setPosition(distanceImg:getPosition())
							sprite:addTo(panel1)
							if i==2 and j== 4 then
								sprite:setScaleX(1.02)
							end
						end

						if la1 == 0 or lo1 == 0 or la2 == 0 or lo2 == 0 then
							distanceTxt:setString("无位置信息")
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
						--distanceTxt:setString("无位置信息")
					end
				
				end
	
			end

			
		end
	end)))
	



end




return GameViewLayer