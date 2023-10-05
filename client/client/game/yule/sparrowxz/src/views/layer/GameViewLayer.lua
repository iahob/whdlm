local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.models.CMD_Game")


require("client/src/plaza/models/yl")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.models.GameLogic")
local CardLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.views.layer.CardLayer")
local ResultLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.views.layer.ResultLayer")
local GameChatLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.views.layer.GameChatLayer")
local PlayerInfo = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.views.layer.PlayerInfo")
local SettingLayer = appdf.req(appdf.GAME_SRC .. "yule.sparrowxz.src.views.layer.SettingLayer")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local SpineObj = appdf.req("client.src.external.SpineObj")

local anchorPointHead = {cc.p(1, 1), cc.p(0, 0.5), cc.p(0, 0), cc.p(1, 0.5)}
local posHead = {cc.p(577, 295), cc.p(165, 332), cc.p(166, 257), cc.p(724, 273)}
local posReady = {cc.p(-333, 0), cc.p(135, 0), cc.p(516, -80), cc.p(-134, 0)}
local posPlate = {cc.p(667, 589), cc.p(237, 464), cc.p(667, 174), cc.p(1093, 455)}
local posChat = {cc.p(873, 660), cc.p(229, 558), cc.p(270, 285), cc.p(1095, 528)}


local VOICE_BTN_NAME = "__voice_record_button__"  --语音按钮名字，可以获取语音按钮，控制显示与否


local GameViewLayer = class("GameViewLayer",function(scene)
	local gameViewLayer =  cc.CSLoader:createNode("game/GameScene.csb")
    return gameViewLayer
end)

GameViewLayer.BT_MENU				= 10 				--按钮开关按钮
GameViewLayer.BT_CHAT 				= 11				--聊天按钮
GameViewLayer.BT_SET 				= 12				--设置
GameViewLayer.CBX_SOUNDOFF 			= 13				--声音开关
GameViewLayer.BT_EXIT	 			= 14			--退出按钮
GameViewLayer.BT_TRUSTEE 			= 15				--托管按钮
GameViewLayer.BT_HOWPLAY 			= 16				--玩法按钮
GameViewLayer.BT_CHANGEDESK         = 17                -- 换桌

GameViewLayer.BT_START 				= 20				--开始按钮
GameViewLayer.BT_GAME_START 		= 21				--游戏开始
GameViewLayer.BT_PLAYER_SIT 		= 22				--坐下按钮

GameViewLayer.NODE_INFO_TAG			= 100				--信息界面

GameViewLayer.BT_CHI				= 30				--游戏操作按钮吃
GameViewLayer.BT_GANG 				= 31				--游戏操作按钮杠
GameViewLayer.BT_PENG				= 32				--游戏操作按钮碰
GameViewLayer.BT_HU 				= 33				--游戏操作按钮胡
GameViewLayer.BT_GUO				= 34				--游戏操作按钮过
GameViewLayer.BT_TING				= 35				--游戏操作按钮听

GameViewLayer.ZORDER_OUTCARD = 40
GameViewLayer.ZORDER_LIUSHUI=50
GameViewLayer.ZORDER_RESULT = 110
GameViewLayer.ZORDER_HUSIGN= 45
GameViewLayer.ZORDER_ACTION = 70
GameViewLayer.ZORDER_CHAT = 80
GameViewLayer.ZORDER_SETTING = 900
GameViewLayer.ZORDER_INFO = 100
GameViewLayer.ZORDER_PRIGAMELAYER = 10

function GameViewLayer:onInitData()

	self.cbActionCard = 0
	self.cbOutCardTemp = 0
	self.bListenBtnEnabled = false
	self.chatDetails = {}
	self.cbAppearCardIndex = {}
	self.bChoosingHu = false
	self.m_bNormalState = {}
	self.m_nAllCard = 108
	self.m_nLeftCard = self.m_nAllCard
	-- 用户头像
    self.m_tabUserHead = {}

	--房卡需要
	self.m_UserItem = {}
	self.m_UserItemCache={}

	self.husigns={}

	-- 语音动画
    AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

end

function GameViewLayer:onResetData()

	self.bChoosingHu = false
	self.cbOutCardTemp = 0
	self.cbAppearCardIndex = {}
	self:onUpdataLeftCard(self.m_nAllCard)

	self.TrustShadow:setVisible(false)

	self:ShowGameBtn(GameLogic.WIK_NULL)

	for i=2,cmd.GAME_PLAYER do
		self.callingCardSp[i]:setVisible(false)
	end

	for i=1,3 do
		self.callCardBtns[i]:stopAllActions()
	end
	self.callCardBar:setVisible(false)

	self:setShowChangeTypeSps(false)

	for i=1,cmd.GAME_PLAYER do
		if nil ~=  self.m_tabUserHead[i] then
			self.m_tabUserHead[i]:showBank(false)
			self.m_tabUserHead[i]:setShowCallCardKind(false)
		end
	end
	self.changeCardBg:setVisible(false)

	if self.operateResultSp and not tolua.isnull(self.operateResultSp) then
		self.operateResultSp:removeSelf()
	end

	for i=1,cmd.GAME_PLAYER do
		if self.husigns[i] then
			self.husigns[i]:removeSelf()
			self.husigns[i]=nil
		end
	end

	self:stopAllActions()
	self:showOutCard(nil, nil, false)
	self._cardLayer:onResetData()
	
	self.bClickedChangeBtn = false

	if self.liushuiLayer and not tolua.isnull(self.liushuiLayer) then
		self.liushuiLayer:removeSelf()
		self.liushuiLayer = nil
	end

	if self._resultLayer and not tolua.isnull(self._resultLayer) then
		self._resultLayer:removeSelf()
		self._resultLayer = nil
	end
end

function GameViewLayer:updateSelfScore( )
	if GlobalUserItem.bPrivateRoom and PriRoom and 1 == PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
       	if self.m_tabUserHead[cmd.MY_VIEWID] and self.m_UserItem[cmd.MY_VIEWID] then
        	self.m_tabUserHead[cmd.MY_VIEWID]:updateScore(self.m_UserItem[cmd.MY_VIEWID].lScore-PriRoom:getInstance().m_tabPriData.lIniScore)
        end
    end
end

function GameViewLayer:setShowCallCard(bShow)
	for i=1,cmd.GAME_PLAYER do
		if nil ~=  self.m_tabUserHead[i] then
			self.m_tabUserHead[i]:setShowCallCardKind(bShow)
		end
	end
end

function GameViewLayer:removeExitedUsers() --游戏开始后离开的玩家
	for viewId=1,cmd.GAME_PLAYER do
		if self.m_UserItem[viewId] then
			if viewId~=cmd.MY_VIEWID and self.m_UserItem[viewId].bExit==true  then
				self.m_tabUserHead[viewId] = nil  --退出依然保存信息
				self.nodePlayer[viewId]:removeAllChildren()
				self.m_UserItem[viewId] = nil
			elseif self.m_UserItem[viewId].bExit==true then
				self.m_UserItem[viewId].bExit=false
			end
		end
	end
end

function GameViewLayer:onExit()
	print("GameViewLayer onExit")
	self:removeSchedule()
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("gameScene.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("gameScene.png")
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    self.m_UserItem = {}
    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
end

function GameViewLayer:getParentNode()
    return self._scene
end

function GameViewLayer:ctor(scene)
	
	self.popLayers={}
	self.name="sparrowxz_gameViewlayer"
	self._scene = scene
	local nodeOpBar = self:getChildByName("FileNode_Op")	

	if self:getChildByName("sp_tableBtBg") then
		self:getChildByName("sp_tableBtBg"):setVisible(false)
	end

	 self:onInitData()
	-- --self:preloadUI()
	self:initButtons()
	self:initUserInfo()
	self._cardLayer = CardLayer:create(self):addTo(self:getChildByName("Node_MaJong"))					--牌图层
    self._chatLayer = GameChatLayer:create(self._scene._gameFrame):addTo(self, GameViewLayer.ZORDER_CHAT)	--聊天框

 --    --左上角游戏信息
 --    --local CsbgameInfoNode = self:getChildByName("FileNode_info")

    --播放背景音乐
    if GlobalUserItem.bVoiceAble == true then  
        AudioEngine.playMusic("sound/backgroud.mp3", true)
	end

    -- self.gameInfoNode = cc.CSLoader:createNode("game/NodeInfo.csb"):addTo(self, GameViewLayer.ZORDER_INFO)
    -- self.gameInfoNode:setPosition(cc.p(0, 750))

    -- --剩余牌数
    -- self.cellScoreNode=self.gameInfoNode:getChildByName("AtlasLabel_cellscore")
    -- assert(self.labelCardLeft)
    -- assert(self.cellScoreNode)
    --指针
    self.userPoint = self:getChildByName("sp_clock")
    self.userPoint:setVisible(false)

    --倒计时
    self.labelClock = self.userPoint:getChildByName("AsLab_time")

    --出牌界面
    self.sprOutCardBg = cc.Sprite:create("game/outCardBg.png"):addTo(self, GameViewLayer.ZORDER_OUTCARD)
    self.sprOutCardBg:setVisible(false)
    self.sprMajong = cc.CSLoader:createNode("card/Node_majong_my_downnormal.csb"):addTo(self.sprOutCardBg)
    self.sprMajong:setPosition(self.sprOutCardBg:getContentSize().width/2, self.sprOutCardBg:getContentSize().height/2)

 --    --准备按钮
 --   	local btnReady = ccui.Button:create("common/btn_start1.png", "common/btn_start2.png",
	-- 	"common/btn_start1.png")
	-- btnReady:addTo(self.sprNoWin)
	-- btnReady:setPosition(ccp(249, 80))

	-- --按钮回调
	-- local btnReadyCallback = function(ref, eventType)
	-- 	if eventType == ccui.TouchEventType.ended then
	-- 		-- 准备
	-- 		self._scene:sendGameStart()
	-- 		self:showNoWin(false)
	-- 		self:onResetData()
	-- 	end
	-- end

	-- btnReady:addTouchEventListener(btnReadyCallback)

	--准备状态
	local posACtion = 
	{
		cc.p(667, 230),
		cc.p(1085, 420),
		cc.p(260, 420),
		cc.p(667, 575)
	}
	self.readySpr = {}
	for i=1,cmd.GAME_PLAYER do
		local sprPath = nil
		if i == cmd.MY_VIEWID or i == cmd.TOP_VIEWID then
			sprPath = "game/Ready_1.png"
		else
			sprPath = "game/Ready_2.png"
		end
		local sprReady = ccui.ImageView:create(sprPath)
		sprReady:addTo(self)
		sprReady:setVisible(false)
		sprReady:setPosition(posACtion[i])
		table.insert(self.readySpr,sprReady)
	end

	-- --节点事件
	-- local function onNodeEvent(event)
	-- 	if event == "exit" then
	-- 		self:onExit()
	-- 	end
	-- end
	-- self:registerScriptHandler(onNodeEvent)


	--托管覆盖层
	self.TrustShadow = self:getChildByName("Panel_trust")
	self.TrustShadow:setTouchEnabled(true)
	self.TrustShadow:setVisible(false)
	--取消托管按钮
	self.TrustShadow:addTouchEventListener(function(ref, eventType)
		if eventType == ccui.TouchEventType.ended then
			-- 取消托管
			if self._scene.m_bLookOn then
				showToast(self, "观战中不可使用该功能", 1)
				-- self.TrustShadow:setVisible(false)
				self.TrustShadow:setTouchEnabled(false)
		        return
		    end
			-- assert(self._scene.m_bTrustee==true)
			self._scene:sendUserTrustee(false)
			self.TrustShadow:setVisible(false)
		end
	end)

	-- --玩家出牌
	-- self.spOutCardBg = cc.Sprite:create("game/outCardBg.png")
	-- self.spOutCardBg:addTo(self)
	-- self.spOutCardBg:setVisible(false)

	--玩家胡牌提示
	self.nodeTips = cc.Node:create()
	self.nodeTips:addTo(self, GameViewLayer.ZORDER_ACTION)
	self.nodeTips:setPosition(cc.p(667, 215))

	self.callCardBtns={}
	self.callCardBar=self:getChildByName("FileNode_CallCard")
	for i=1,3 do
		self.callCardBtns[i]=self.callCardBar:getChildByName("Button_"..i)
		self.callCardBtns[i]:addClickEventListener(function() 
			self._scene:sendCallCard(i) 
			self.callCardBar:setVisible(false) 
			self._cardLayer:setCallcardKind(i)
			end)
	end
	self.callCardBar:setVisible(false)

	-- self.liushuiBtn=self:getChildByName("btn_liushui")
	-- self.liushuiBtn:addClickEventListener(function() self:showDuiJuLiuShui(self._scene:getMyTagCharts()) end)

	self.changeTypeSps={}
	self.changeCard={}
	for i=1,4 do
		self.changeTypeSps[i]=self:getChildByName("change_"..i)
		self.changeCard[i] = self:getChildByName("Node_changeCard_"..i):setScale(0.7)
	end
	self.callingCardSp={}
	self.changeStatusSp = {}
	for i=2,4 do
		self.callingCardSp[i]=self:getChildByName("callingcard"..i)
		self.changeStatusSp[i]=self:getChildByName("changeStatus_"..i)
	end

	self.changeCardBg=self:getChildByName("tipBg")
	self.changeCardBtn=self.changeCardBg:getChildByName("btn_changcardok")
	self.changeCardBtn:addClickEventListener(function()
		local cards=self._cardLayer:getChangeCards()
		if #cards~=3 then 
			print(" #(self._cardLayer:getChangeCards()):", #(self._cardLayer:getChangeCards()))
			showToast(self,"请选择三张相同花色的牌",1)
		elseif  math.ceil(cards[1]/16)~=math.ceil(cards[2]/16)
			or math.ceil(cards[1]/16)~=math.ceil(cards[3]/16) then
			showToast(self,"请选择三张相同花色的牌",1)
		else
			self.bClickedChangeBtn = true
			self._scene:sendChangeCard(cards)
			local t=self._cardLayer.cbCardData[cmd.MY_VIEWID]
			print("#t:",#t)
			assert(#t==10 or #t==11)
			-- self._cardLayer:sortHandCard(cmd.MY_VIEWID)
			t=self._cardLayer.cbCardData[cmd.MY_VIEWID]
			assert(#t==10 or #t==11)
			for i=1,#t do
				print(i..":",t[i])
			end
			self:showChangCardHint(false)
			self.changeCard[1]:setVisible(true)
			t=self._cardLayer.cbCardData[cmd.MY_VIEWID]
			assert(#t==10 or #t==11)
			for i=1,#t do
				print(i..":",t[i])
			end
		end
	 end)

	self.leftcardAtlas=self:getChildByName("leftcardNum")
	self.leftcardAtlas:setScale(0.7,0.7)


	self:getChildByName("Text_playrule"):setPositionY(160+self:getChildByName("Text_playrule"):getPositionY())

	self.debugBtn=self:getChildByName("debugBtn")
	self.debugBtn:addClickEventListener( function() self._scene:sendAskHandCard() end)
	self.debugBtn:setVisible(false)

	-- 中上条形按钮
	self.btnTop = self:getChildByName("btn_top")
	print("---------------------------------GlobalUserItem.bPrivateRoom0",GlobalUserItem.bPrivateRoom)
	self.btnTop:setTouchEnabled(GlobalUserItem.bPrivateRoom)
	self.btnTop:addClickEventListener(function() self:showDuiJuLiuShui(self._scene:getMyTagCharts()) print("top touched") end)

	local topText_1 = self.btnTop:getChildByName("Text_1"):setVisible(false)
	local topText_2 = self.btnTop:getChildByName("Text_2"):setVisible(false)

	-- 电量,时间，信号
	local BatteryBg = self:getChildByName("BatteryBg")
	local sp = cc.Sprite:create("game/yule/sparrowxz/res/game/Battery.png")
	self.batteryBar = cc.ProgressTimer:create(sp)
	self.batteryBar:setType(cc.PROGRESS_TIMER_TYPE_BAR)
    self.batteryBar:setMidpoint(cc.p(0,0.5))
    self.batteryBar:setBarChangeRate(cc.p(1, 0))
    self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
    self.batteryBar:setPosition(BatteryBg:getContentSize().width/2, BatteryBg:getContentSize().height/2)
    BatteryBg:addChild(self.batteryBar)

    BatteryBg:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function()self:updateFunc()end))))

    self:updateFunc()
    -- local schedule = cc.Director:getInstance():getScheduler()
    -- self._schSingal = schedule:scheduleScriptFunc( function()self:updateFunc()end, 5, false)

    cc.funAddChildListener(self, "Button_location", function() 
    	if self._scene.m_bLookOn then
          	showToast(self, "观战中不可使用该功能", 1)
          	return
      	end
    	self:onClickLocation()
    end)
    self:getChildByName("Button_Record"):setVisible(GlobalUserItem.bPrivateRoom)
    cc.funAddChildListener(self, "Button_Record", function() 
    	if self._scene.m_bLookOn then
          	showToast(self, "观战中不可使用该功能", 1)
          	return
      	end
    	self:sendAskRCRecord() 
    end)

	local btChat = self:getChildByName("Button_Chat")	--聊天
	btChat:setTag(GameViewLayer.BT_CHAT)
	btChat:addTouchEventListener(
		--按钮回调
		function(ref, eventType)
			if eventType == ccui.TouchEventType.ended then
				if self._scene.m_bLookOn then
		          	showToast(self, "观战中不可使用该功能", 1)
		          	return
		      	end
				self:onButtonClickedEvent(ref:getTag(), ref)
			end
		end)
end

function GameViewLayer:updateFunc()
	self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
	self:getChildByName("Time"):setString(os.date("%H:%M"))
	local netType = MultiPlatform:getInstance():getNetworkType()
	local Strength = MultiPlatform:getInstance():getNetworkStrength()
	local TypePic = {"xinhao_","wifi-"}
	self:getChildByName("Sprite_Wifi"):setTexture("game/"..TypePic[netType]..Strength..".png")
end

function GameViewLayer:showDuiJuLiuShui(tagCharts)

	--local tagChart={charttype=cmd.CHARTTYPE.HUJIAOZHUANYI_TYPE,lTimes=200,lScore=0,bEffectChairID={true,true,true,true}}
	--local tagChart2={charttype=cmd.CHARTTYPE.HUJIAOZHUANYI_TYPE,lTimes=200,lScore=20000,bEffectChairID={true,true,true,true}}
	--tagCharts={tagChart,tagChart,tagChart,tagChart,tagChart,tagChart,tagChart2,tagChart,tagChart,tagChart,tagChart2}

	--print("me:",GlobalUserItem.szNickName)
	--print("#self.myTagCharts:",#tagCharts)
	if self._scene.m_bLookOn then
      	showToast(self, "观战中不可使用该功能", 1)
      	return
  	end
	local temp={}
	for i=1,#(tagCharts or {}) do
		print("self.myTagCharts"..i..".charttype:", tagCharts[i].charttype)
		local tagChart=tagCharts[i]
		if tagChart and tagChart.lScore~=0 and tagChart.charttype~=cmd.CHARTTYPE.INVALID_CHARTTYPE then
			table.insert(temp,tagChart)
		end
	end
	tagCharts=temp

	local topInfoLayer=cc.CSLoader:createNode("game/Layer_liushui.csb")
	topInfoLayer.onTouchBegan=function(node,touch, event)  return true end
	topInfoLayer.onTouchEnded=function(node,touch, event)
    	local pos = touch:getLocation() 
    	local m_spBg = node:getChildByName("bg")
   		 pos = m_spBg:convertToNodeSpace(pos)
    	local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    	if false == cc.rectContainsPoint(rec, pos) then
        	node:removeFromParent()
    	end
	end

	ExternalFun.registerTouchEvent(topInfoLayer, false)
	self:addChild(topInfoLayer,self.ZORDER_LIUSHUI)

    local liushuiPanel = topInfoLayer:getChildByName("Panel_1")
    local roomPanel = topInfoLayer:getChildByName("Panel_2")
    local btnSure = roomPanel:getChildByName("Button_3")
	btnSure:addTouchEventListener(
	function(ref, eventType)
		if eventType == ccui.TouchEventType.began then
			return true
		elseif eventType == ccui.TouchEventType.ended then
			-- topInfoLayer:removeFromParent()
			print("  btnSure ")
			topInfoLayer:removeSelf()
		end
	end)
    -- cc.funAddChildListener(roomPanel, "Button_3", function() topInfoLayer:removeSelf() end)


	local btnRoom = topInfoLayer:getChildByName("Btn_wanfa")
	local btnLiushui = topInfoLayer:getChildByName("Btn_liushui")
	local function switchBtnState(roomShow)
		btnRoom:setBright(not roomShow)
		roomPanel:setVisible(roomShow)
		btnLiushui:setBright(roomShow)
		liushuiPanel:setVisible(not roomShow)
	end
	btnRoom:addTouchEventListener(
	--按钮回调
	function(ref, eventType)
		if eventType == ccui.TouchEventType.ended then
			switchBtnState(true)
		end
	end)
	btnLiushui:addTouchEventListener(
	--按钮回调
	function(ref, eventType)
		if eventType == ccui.TouchEventType.ended then
			switchBtnState(false)
		end
	end)

	switchBtnState(true) -- 默认房间玩法界面


	local totalScore=0
	for i=1,#tagCharts do
		totalScore=totalScore+tagCharts[i].lScore
	end
	if totalScore>=0 then 
    	totalScore="+"..totalScore 
    end
    liushuiPanel:getChildByName("curScore"):setString(totalScore)

    local ruleStr = "玩法："
	if self._scene.bHuanSanZhang then --true为允许换三张
		ruleStr = ruleStr.."换三张 "
	end
	if self._scene.bHuJiaoZhuanYi then --true允许呼叫转移
		ruleStr = ruleStr.."呼叫转移"
	end
    roomPanel:getChildByName("Text_1"):setString(ruleStr)

    local fanXing = roomPanel:getChildByName("Text_2")
    local str = "番型："
	if self._scene.bZiMoAddTimes then --true允许自摸加倍
		str = str .. "自摸加倍 "
	end
	if self._scene.bTianDiHu then --true允许天地胡
		str = str .. "天地胡 "
	end
	if self._scene.bHaiDiLaoYue then --true允许海底捞月
		str = str .. "海底捞月 "
	end
	if self._scene.bMenQing then --true允许门清加倍
		str = str .. "门清加倍"
	end
	fanXing:setString(str)

    local Text_3 = roomPanel:getChildByName("Text_3")
    local Text_4 = roomPanel:getChildByName("Text_4")

    if GlobalUserItem.bPrivateRoom then
    	--0积分,1金币
		if self._scene.cbPlayMode == 1 then
			-- Text_3:setVisible(false)
			Text_4:setVisible(false)
			Text_3:setString("底分："..self._scene.lCellScore)
			local m_tabPriData = PriRoom:getInstance().m_tabPriData
			local lEnterScore = m_tabPriData.lEnterScore or 0
			local lExitScore = m_tabPriData.lExitScore or 0
			if lEnterScore and lExitScore then
				Text_4:setString("入场设定："..lEnterScore.."  离场设定："..lExitScore)
				Text_4:setVisible(true)
			end
		else
			local needPassword = PriRoom:getInstance().m_tabPriData.bRoomPwd
			local roomAAPay = PriRoom:getInstance().m_tabPriData.bAAPay
		    local strPasswordset = needPassword and "需要密码" or "不要密码"
		    local strPayType = roomAAPay and "AA支付" or "房主支付"
			Text_3:setString("密码配置："..strPasswordset)
			Text_4:setString("支付方式："..strPayType)
		end
	else
		Text_3:setVisible(false)
		Text_4:setVisible(false)
	end

    -- self.topInfoLayer=topInfoLayer

	local function numberOfCellsInTableView()
		local n=0
		for k,tagChart in pairs(tagCharts) do
			if tagChart.charttype~=cmd.CHARTTYPE.INVALID_CHARTTYPE  then
				n=n+1
			end
		end
		return n
	end

	local function cellSizeForTable(view,idx) 
    	return 600,56
	end

	local function tableCellAtIndex(view, idx) --idx从0开始
		local tagChart=tagCharts[idx+1]
     	local cell=view:dequeueCell()
    	if nil==cell then
    		cell=cc.TableViewCell:create()
    	end
    	if cell:getChildByTag(1)==nil and tagChart~=nil then
    		cc.CSLoader:createNode("game/Node_liushui_list.csb")
    			:addTo(cell)
    			:setTag(1)
    			:setPosition(9,5)
    	end
    	if tagChart==nil or tagChart.charttype==cmd.CHARTTYPE.INVALID_CHARTTYPE  then cell:removeAllChildren() return cell end
    	print("tagChart.charttype:",tagChart.charttype)
    	local node=cell:getChildByTag(1)
    	assert(node:getChildByName("kind"))
    	print("tagChart.charttype:",tagChart.charttype)
    	node:getChildByName("kind"):setString(cmd.CHARTTYPESTR[tagChart.charttype-21])
    	node:getChildByName("times"):setString(tagChart.lTimes.."倍")
    	local score=tagChart.lScore
    	if score>=0 then 
    		score="+"..score 
    		node:getChildByName("score"):setTextColor(cc.c4b(0xc0,0x4d,0x45,0xff))
    	else 
    		node:getChildByName("score"):setTextColor(cc.c4b(0x58,0x6e,0xbe,0xff))
    	end
    	node:getChildByName("score"):setString(score)
    	local strs={}
    	local objs={"","下家","上家","对家"}
    	local t=tagChart
    	print("tagChart.charttype:",tagChart.charttype)
    	print(t.charttype,t.lTimes,t.lScore,t.bEffectChairID[1],t.bEffectChairID[2],t.bEffectChairID[3],t.bEffectChairID[4])
    	
    	print("对局流水：")
    	for i=1,cmd.GAME_PLAYER do
    		if tagChart.bEffectChairID[i]==true then
    			local viewid=self._scene:SwitchViewChairID(i-1)
    			print("effect viewID: ",viewid)
   				if viewid~=cmd.MY_VIEWID then table.insert(strs,objs[viewid]) end
   			end
    	end
    	node:getChildByName("obj"):setString(table.concat(strs, "、"))
    	return cell
     end

    if tagCharts==nil or #tagCharts==0 then return end
    local tableView=cc.TableView:create(cc.size(500,350))
	tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(cc.p(0,-50))
    tableView:setDelegate()
    tableView:registerScriptHandler(numberOfCellsInTableView,cc.NUMBER_OF_CELLS_IN_TABLEVIEW)  
    tableView:registerScriptHandler(cellSizeForTable,cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex,cc.TABLECELL_SIZE_AT_INDEX)
    tableView:reloadData()
    tableView:setVerticalFillOrder(0)
    tableView:addTo(liushuiPanel:getChildByName("Node_1"))
end

function GameViewLayer:preloadUI()
    print("欢迎来到召唤师峡谷！")
    --导入动画
    local animationCache = cc.AnimationCache:getInstance()
    for i = 1, 12 do
    	local strColor = ""
    	local index = 0
    	if i <= 6 then
    		strColor = "white"
    		index = i
    	else
    		strColor = "red"
    		index = i - 6
    	end
		local animation = cc.Animation:create()
		animation:setDelayPerUnit(0.1)
		animation:setLoops(1)
		for j = 1, 9 do
			local strFile = "Animate_sice_"..strColor..string.format("/sice_%d.png", index)
			local spFrame = cc.SpriteFrame:create(strFile, cc.rect(133*(j - 1), 0, 133, 207))
			animation:addSpriteFrame(spFrame)
		end

		local strName = "sice_"..strColor..string.format("_%d", index)
		animationCache:addAnimation(animation, strName)
	end
end

function GameViewLayer:setShowChangeTypeSps(bShow,kind)
	--showToast(self,"setShowChangeTypeSps",3)
	self:otherChangeStates(false)
	for i=1,4 do
		self.changeTypeSps[i]:setVisible(bShow)
		self.changeCard[i]:setVisible(bShow)
	end
	if bShow then
		for i=1,4 do
			self.changeTypeSps[i]:runAction(cc.FadeTo:create(1,0))
			self.changeCard[i]:runAction(cc.FadeTo:create(1,0))
		end
	else
		for i=1,4 do
			self.changeTypeSps[i]:stopAllActions()
			self.changeTypeSps[i]:setOpacity(255)
			self.changeCard[i]:stopAllActions()
			self.changeCard[i]:setOpacity(255)
		end
	end

	local rot = {180, 90, 270, 0}
	if kind == 2 then
		print("kind:",kind)
		assert(kind==1 or kind==2)
		for i=1,4 do
			self.changeTypeSps[i]:setScaleX(-1)
			self.changeTypeSps[i]:setRotation(rot[i]-90)
		end
	else
		for i=1,4 do
			self.changeTypeSps[i]:setScaleX(1)
			self.changeTypeSps[i]:setRotation(rot[i])
		end
	end
end

function GameViewLayer:removeMyChangeCards()
	if self.bClickedChangeBtn == false then
		if #self._scene.cbPriorChangeCardData[1] == 3 then
			self._cardLayer:setPopChangeCard(self._scene.cbPriorChangeCardData)
			self._cardLayer:removeHandCardsByIndex(cmd.MY_VIEWID,self._cardLayer:getChangeCardsIndex())
		end
	else
		self._cardLayer:removeHandCardsByIndex(cmd.MY_VIEWID,self._cardLayer:getChangeCardsIndex())
	end
	self._cardLayer:sortHandCard(cmd.MY_VIEWID)
end


function GameViewLayer:exchangeCard(cards,bMeBanker)
	assert(#cards==3)
	self._cardLayer:exchangeCard(cards)
end

function GameViewLayer:setShowCallCardBar(bShow)
	if bShow and self._scene.m_bLookOn then
		return
	end
	self.callCardBar:setVisible(bShow)
end

function GameViewLayer:showCallCardHint()
	local kind=self._cardLayer:getCallCardHint()
	for i=2,4 do
		self.callingCardSp[i]:setVisible(true)
	end
	if not self._scene.m_bLookOn then
		self.callCardBar:setVisible(true)
	end
	
	local fadeAct=cc.FadeOut:create(1) 
	local repAct=cc.Repeat:create(cc.Sequence:create(fadeAct,fadeAct:reverse()),20)
	self.callCardBtns[kind]:setOpacity(255)
	self.callCardBtns[kind]:runAction(repAct)
	self.callCardBar:getChildByName("Sprite_Select"):setPositionX(self.callCardBtns[kind]:getPositionX())
end

function GameViewLayer:setShowCallingCard(viewid,bShow)
	self.callingCardSp[viewid]:setVisible(bShow)
end

function GameViewLayer:calledCard(viewId,callCardKind)
	assert(self.callingCardSp)
	print("viewId:",viewId)
	if callCardKind==nil and viewId~=cmd.MY_VIEWID then --收到CMD_S_CallCard
		self.callingCardSp[viewId]:setVisible(false)--隐藏“选缺中”
	else --收到bankerinfo
		if viewId~=cmd.MY_VIEWID then
			self.callingCardSp[viewId]:setVisible(false)
		end
		self.m_tabUserHead[viewId]:showCallCard(callCardKind)
		--end
	end
end

--初始化玩家信息
function GameViewLayer:initUserInfo()
	local nodeName = 
	{
		"FileNode_3",
		"FileNode_4",
		"FileNode_2",
		"FileNode_1",
	}

	local faceNode = self:getChildByName("Node_User")
	self.nodePlayer = {}
	for i = 1, cmd.GAME_PLAYER do
		self.nodePlayer[i] = faceNode:getChildByName(nodeName[i])
		self.nodePlayer[i]:setLocalZOrder(1)
		self.nodePlayer[i]:setVisible(true)
	end
	self.nodePlayer[4]:setPositionY(self.nodePlayer[4]:getPositionY()+15) --temp
	self.nodePlayer[1]:setPositionY(self.nodePlayer[1]:getPositionY()-15)
end

--更新玩家准备状态
function GameViewLayer:showUserState(viewid, isReady)
	print("更新用户状态", viewid, isReady, #self.readySpr)
	local spr = self.readySpr[viewid]
	if nil ~= spr then
		spr:setVisible( isReady)
	end
end

--初始化界面上button
function GameViewLayer:initButtons()
	--按钮回调
	local btnCallback = function(ref, eventType)
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(ref:getTag(), ref)
		elseif eventType == ccui.TouchEventType.began and ref:getTag() == GameViewLayer.BT_VOICE then
			--self:onButtonClickedEvent(GameViewLayer.BT_VOICEOPEN, ref)
		end
	end

	--桌子操作按钮屏蔽层
	--按钮背景
	local menuBg = self:getChildByName("sp_tableBtBg")
	local function callbackShield(ref)
		local pos = ref:getTouchEndPosition()
        local rectBg = menuBg:getBoundingBox()
        if not cc.rectContainsPoint(rectBg, pos)then
        	self:showMenu(false)
        end
	end
	self.layoutShield = self:getChildByName("Image_Touch")
		:setTouchEnabled(false)
	self.layoutShield:addClickEventListener(callbackShield)

	--右上角按钮控制开关
	local btnMenu = self:getChildByName("bt_menu")
	btnMenu:addTouchEventListener(btnCallback)
	btnMenu:setTag(GameViewLayer.BT_MENU)
	self.btnMenu = btnMenu

	local btSet = menuBg:getChildByName("bt_set")
	btSet:addTouchEventListener(btnCallback)
	btSet:setTag(GameViewLayer.CBX_SOUNDOFF)

	local btExit = menuBg:getChildByName("bt_exit")	--退出
	btExit:addTouchEventListener(btnCallback)
	btExit:setTag(GameViewLayer.BT_EXIT)
	self.btExit = btExit

	self.btchangedesk = menuBg:getChildByName("bt_changedesk")	--换桌
	self.btchangedesk:addTouchEventListener(btnCallback)
	self.btchangedesk:setTag(GameViewLayer.BT_CHANGEDESK)

	


	local btTrustee = menuBg:getChildByName("bt_trustee")	--托管
	btTrustee:addTouchEventListener(btnCallback)
	btTrustee:setTag(GameViewLayer.BT_TRUSTEE)
	btTrustee:setPosition(self.btchangedesk:getPosition())
	btTrustee:setVisible(false)
	self.btTrustee = btTrustee

	-- local btHowPlay = menuBg:getChildByName("bt_help")	--玩法
	-- btHowPlay:addTouchEventListener(btnCallback)
	-- btHowPlay:setTag(GameViewLayer.BT_HOWPLAY)

	--开始
	self.btStart = self:getChildByName("bt_start")
		:setVisible(false)
	self.btStart:setPositionX(yl.WIDTH/2)
	self.btStart:addTouchEventListener(btnCallback)
	self.btStart:setTag(GameViewLayer.BT_START)

	--游戏开始
	self.btn_game_start = self:getChildByName("btn_game_start")
		:setVisible(false)
	self.btn_game_start:setPositionX(yl.WIDTH/2)
	self.btn_game_start:addTouchEventListener(btnCallback)
	self.btn_game_start:setTag(GameViewLayer.BT_GAME_START)

	--坐下
	self.btn_player_sit = self:getChildByName("btn_player_sit")
		:setVisible(false)
	self.btn_player_sit:setPositionX(yl.WIDTH/2)
	self.btn_player_sit:addTouchEventListener(btnCallback)
	self.btn_player_sit:setTag(GameViewLayer.BT_PLAYER_SIT)

	-- 语音按钮 gameviewlayer -> gamelayer -> clientscene
    -- self:getParentNode():getParentNode():createVoiceBtn(cc.p(1268, 310), 0, self)
	-- local btnVoice = ccui.Button:create("game/btn_voice1.png", "game/btn_voice2.png",
	-- 	"game/btn_voice2.png")
	-- 	:move(1268, 212)
	-- 	:addTo(self)
	-- btnVoice:setName(VOICE_BTN_NAME)
	local btnVoice = self:getChildByName("Button_Voice")
	btnVoice:addTouchEventListener(function(ref, eventType)
		if self._scene.m_bLookOn then
          	showToast(self, "观战中不可使用该功能", 1)
          	return
      	end
 		if eventType == ccui.TouchEventType.began then
 			self._scene:getParentNode():startVoiceRecord()
        elseif eventType == ccui.TouchEventType.ended 
        	or eventType == ccui.TouchEventType.canceled then
        	self._scene:getParentNode():stopVoiceRecord()
        end
	end)
	--聊天窗口
    -- self.m_chatLayer = GameChatLayer:create(self._scene._gameFrame)
    -- 	:setLocalZOrder(9)
    -- 	:addTo(self._csbNode)
    -- 	:setVisible(false)


	--游戏操作按钮
	--获取操作按钮node
	local nodeOpBar = self:getChildByName("FileNode_Op")
	--广东麻将只有4个，不同游戏自行添加
	local btGang = nodeOpBar:getChildByName("Button_gang") 	--杠
	btGang:setEnabled(false)
	btGang:addTouchEventListener(btnCallback)
	btGang:setTag(GameViewLayer.BT_GANG)

	local btPeng = nodeOpBar:getChildByName("Button_pen") 	--碰
	btPeng:setEnabled(false)
	btPeng:addTouchEventListener(btnCallback)
	btPeng:setTag(GameViewLayer.BT_PENG)

	local btHu = nodeOpBar:getChildByName("Button_hu") 	--胡
	btHu:setEnabled(false)  
	btHu:addTouchEventListener(btnCallback)
	btHu:setTag(GameViewLayer.BT_HU)

	local btGuo = nodeOpBar:getChildByName("Button_guo") 	--过
	btGuo:setEnabled(false)
	btGuo:addTouchEventListener(btnCallback)
	btGuo:setTag(GameViewLayer.BT_GUO)

end


function GameViewLayer:priGameLayerZorder()
	return GameViewLayer.ZORDER_PRIGAMELAYER
end


function GameViewLayer:setBtnTrustEnabled(bEnabled)
	--self.btTrustee:setEnabled(bEnabled)
end

function GameViewLayer:setShowStartBtn(bShow)

	self.btStart:setVisible(bShow)
end

--按钮控制（下拉菜单下拉，隐藏语音按钮）
function GameViewLayer:showMenu(bVisible)
	--按钮背景
	local menuBg = self:getChildByName("sp_tableBtBg")
	if menuBg:isVisible() == bVisible then
		return false
	end

	local btnMenu = self:getChildByName("bt_menu")
	-- btnMenu:setVisible(not bVisible)
	self.layoutShield:setTouchEnabled(bVisible)
	menuBg:setVisible(bVisible)

	-- --显示菜单按钮时，隐藏录音按钮
	-- local btnVoice = self:getChildByName(VOICE_BTN_NAME)
	-- btnVoice:setVisible(not bVisible)

	return true
end



function GameViewLayer:showGameResult(tabHupaiInfo,pGameEnd)
	local bankerViewId = self._scene:SwitchViewChairID(self._scene.m_wBankerUser)
	self._resultLayer=ResultLayer:create(self,self.m_UserItemCache,tabHupaiInfo, pGameEnd, self._scene.leftUserViewId,bankerViewId)
	self._resultLayer:addTo(self,GameViewLayer.ZORDER_RESULT)
	self._resultLayer:setVisible(false)
	if not self._scene.m_bLookOn then 
		self.btStart:setVisible(true)
	end
	
	self._cardLayer:runAction(
		cc.Sequence:create(
		cc.DelayTime:create(0.5), 
		cc.CallFunc:create(function()print("显示结算界面")self._resultLayer:setVisible(true)  end)
		))
end

--更新用户显示
function GameViewLayer:OnUpdateUser(viewId, userItem)
	if not viewId or viewId == yl.INVALID_CHAIR then
		--assert(false)
		return
	end

	if nil == userItem then
        return
    end
    dump("更新用户显示",userItem)
    self.m_UserItem[viewId] = userItem
    self.m_UserItemCache[viewId]=userItem

	if nil == self.m_tabUserHead[viewId] then
        local playerInfo = PlayerInfo:create(userItem, viewId)
        playerInfo.parent = self;
        self.m_tabUserHead[viewId] = playerInfo
        self.nodePlayer[viewId]:addChild(playerInfo)
    else  --断线or上线
        self.m_tabUserHead[viewId].m_userItem = userItem
        self.m_tabUserHead[viewId]:updateStatus()
    end
    if PriRoom and GlobalUserItem.bPrivateRoom then
		if userItem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then
			self.m_tabUserHead[viewId]:showRoomHolder(true)
		else
			self.m_tabUserHead[viewId]:showRoomHolder(false)
		end

		if  self._priView and  self._priView.onRefreshInviteBtn  then
            self._priView:onRefreshInviteBtn()
        end
	end

end

--玩家退出，移除头像信息
function GameViewLayer:OnUpdateUserExit(viewId)
	print("移除用户", viewId)
	if nil ~= self.m_tabUserHead[viewId] and self._scene.bEnd~=true then
		self.m_tabUserHead[viewId] = nil  --退出依然保存信息
		self.nodePlayer[viewId]:removeAllChildren()
		self.m_UserItem[viewId] = nil
	elseif self._scene.bEnd==true then
		self.m_UserItem[viewId].bExit=true
		if self.m_tabUserHead[viewId] and not tolua.isnull(self.m_tabUserHead[viewId]) then
			self.m_tabUserHead[viewId]:updateStatus(true)
		end
	end
end

-- 文本聊天
function GameViewLayer:onUserChat(chatdata, viewId)
    local playerItem = self.m_tabUserHead[viewId]
    print("获取当前显示聊天的玩家头像", playerItem, viewId, chatdata.szChatString)
    if nil ~= playerItem then
        playerItem:textChat(chatdata.szChatString)
        self._chatLayer:showGameChat(false)
    end
end

-- 表情聊天
function GameViewLayer:onUserExpression(chatdata, viewId)
    local playerItem = self.m_tabUserHead[viewId]
    if nil ~= playerItem then
        playerItem:browChat(chatdata.wItemIndex)
        self._chatLayer:showGameChat(false)
    end
end

--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
	--取消文字，表情
	local playerItem = self.m_tabUserHead[viewid]
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
        local zOrder = 1

        --获取头像坐标
        for k,v in pairs(self.m_tabUserHead) do
              if v then
                  -- local head = self.nodePlayer[k]:getChildByTag(GameViewLayer.SP_HEAD)
                  if v.m_userItem.dwUserID ==  dwUserID then
                        beginPoint = cc.p(self.nodePlayer[k]:getPosition())
                  elseif v.m_userItem.dwUserID ==  dwRecvUserID then
                        endPoint = cc.p(self.nodePlayer[k]:getPosition())
                        zOrder = self.nodePlayer[k]:getZOrder() + 1
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

-- 换桌和托管按钮切换显示
function GameViewLayer:switchChangeAndTrustBtn()
	
	if self._scene.m_cbGameStatus == cmd.GAME_STATUS_FREE then
		self.btTrustee:setVisible(false)
		self.btchangedesk:setVisible(true)
	else
		self.btTrustee:setVisible(true)
		self.btchangedesk:setVisible(false)
	end
	if GlobalUserItem.bPrivateRoom then
	    if GlobalUserItem.bPrivateGoldRoom then
			self.btTrustee:setVisible(true)
			self.btchangedesk:setVisible(false)
		else
			self.btchangedesk:setVisible(false)
			self.btchangedesk:setEnabled(false)
			self.btTrustee:setVisible(false)
			self.btExit:setPosition(-0.11, -189)  --私人房隐藏下拉换桌
		end
	end
end

function GameViewLayer:showSitBtn(visible)
	if self._scene.m_bLookOn then 
		self.btn_player_sit:setVisible(visible)
		self.btn_game_start:setVisible(false)
		-- self.btStart:setVisible(false)
	else
		self.btn_game_start:setVisible(visible)
		self.btn_player_sit:setVisible(false)
		-- self.btStart:setVisible(false)
	end
end

--按键事件
function GameViewLayer:onButtonClickedEvent(tag, ref)
	-- if tag>=GameViewLayer.BT_GANG then
	-- 	self._scene.file:write("GameViewLayer.cbActionCard: ",self.cbActionCard,"\n")
	-- end
	if tag == GameViewLayer.BT_START then
		print("麻将开始！")
		self:stopAllActions()
		-- self.btStart:setVisible(false)
		if self.liushuiLayer and not tolua.isnull(self.liushuiLayer) then
			self.liushuiLayer:removeSelf()
			self.liushuiLayer = nil
		end

		if self._resultLayer and not tolua.isnull(self._resultLayer) then
			self._resultLayer:removeSelf()
			self._resultLayer = nil
		end
		self:switchChangeAndTrustBtn()
		self._scene:sendGameStart()
	elseif tag == GameViewLayer.BT_PLAYER_SIT then   --坐下
		-- if self._scene:onGetSitUserNum() == cmd.GAME_PLAYER then 
		-- 	showToast(self, "没有空位可以坐下！", 2)
		-- 	return 
		-- end

		-- if PriRoom:getInstance().m_tabPriData.dwPlayCount ==  PriRoom:getInstance().m_tabPriData.dwDrawCountLimit then 
		-- 	showToast(self, "最后一局，无法坐下！", 2)
		-- 	return 
		-- end

		-- for i=1,cmd.GAME_PLAYER do
		-- 	if self.m_tabUserHead[i] then
				
		-- 		self:showUserState(i, false)
		-- 		self.m_tabUserHead[i]:removeFromParent()
		-- 		self.m_tabUserHead[i]= nil
		-- 	end
		-- end
		-- print("------------坐下-------------4444---")
		-- -- self._scene:OnResetGameEngine()
		-- self:showSitBtn(false)
		-- self._scene._gameFrame.m_bSendOption = false
		-- self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)
		-- self._scene:OnInitGameEngine()
		-- self._scene.m_bLookOn = false
		self._scene._gameFrame.bChangeDesk = true
		self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)
	elseif tag == GameViewLayer.BT_GAME_START then   --房主开始游戏	
		-- self.btSitAndBegin:setVisible(false)
		-- self._scene._gameFrame:SendUserBegin()
	elseif tag == GameViewLayer.BT_MENU then
		print("按钮开关")
		local menuBg = self:getChildByName("sp_tableBtBg")
		self:showMenu(not menuBg:isVisible())
		self:switchChangeAndTrustBtn()
	elseif tag == GameViewLayer.BT_CHAT then
		print("聊天！")
		if self._scene.m_bLookOn then
          	showToast(self, "观战中不可使用该功能", 1)
          	return
      	end
		self._chatLayer:showGameChat(true)
		self:showMenu(false)
	elseif tag == GameViewLayer.CBX_SOUNDOFF then
		print("设置开关！")
		local set = SettingLayer:create( self )
        self:addChild(set, GameViewLayer.ZORDER_SETTING)
	elseif tag == GameViewLayer.BT_HOWPLAY then
		print("玩法！")
		self._scene._scene:popHelpLayer2(cmd.KIND_ID, 0)
	elseif tag == GameViewLayer.BT_EXIT then
		print("退出！")
		self:removeSchedule()
		self._scene:onQueryExitGame()
	elseif tag == GameViewLayer.BT_CHANGEDESK then	
		if GlobalUserItem.bPrivateRoom == true then
			return
		end
		if self._scene.m_bLookOn then
          	showToast(self, "观战中不可使用该功能", 1)
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
			self._scene:queryChangeDesk()

			--1秒只能点击一次换桌
			self.btchangedesk:runAction(cc.Sequence:create(cc.CallFunc:create(function()
				self.btchangedesk:setEnabled(false)
				self:showMenu(false)
			end), cc.DelayTime:create(1.6), 
			cc.CallFunc:create(function()
				self.btchangedesk:setEnabled(true)
			end)))
		end
	elseif tag == GameViewLayer.BT_TRUSTEE then
		print("托管")
		if self._scene.m_bLookOn then
          	showToast(self, "观战中不可使用该功能", 1)
          	return
      	end
		self._scene:sendUserTrustee(true)
		-- self:showMenu(false)
		self:ShowGameBtn(GameLogic.WIK_NULL)
	elseif tag == GameViewLayer.BT_PENG then
		print("碰！")

		--发送碰牌
		local cbOperateCard = self.cbActionCard
		self._scene:sendOperateCard(GameLogic.WIK_PENG, cbOperateCard)

		self:ShowGameBtn(GameLogic.WIK_NULL)
	elseif tag == GameViewLayer.BT_GANG then
		print("杠！")
		local cbOperateCard =self.cbActionCard
		self._scene:sendOperateCard(GameLogic.WIK_GANG, cbOperateCard)

		self:ShowGameBtn(GameLogic.WIK_NULL)
	elseif tag == GameViewLayer.BT_HU then
		print("胡！")

		local cbOperateCard = self.cbActionCard
		self._scene:sendOperateCard(GameLogic.WIK_CHI_HU, cbOperateCard)

		self:ShowGameBtn(GameLogic.WIK_NULL)
	elseif tag == GameViewLayer.BT_GUO then
		print("过！")
		if not self._cardLayer.bChoosingOutCard then
			self._scene:sendOperateCard(GameLogic.WIK_NULL, 0)
		end

		self:ShowGameBtn(GameLogic.WIK_NULL)
	else
		print("default")
	end
end

function GameViewLayer:disableOpereteBtn()
	local OpNode = self:getChildByName("FileNode_Op")
	local btGang = OpNode:getChildByName("Button_gang") 	--杠
	local btPeng = OpNode:getChildByName("Button_pen") 	--碰
	local btHu = OpNode:getChildByName("Button_hu") 	--胡
	btGang:setEnabled(false)
	btPeng:setEnabled(false)
	if OpNode:isVisible() and not btHu:isEnabled() then
		OpNode:setVisible(false)
	end 
end

--更新操作按钮状态
function GameViewLayer:ShowGameBtn(cbActionMask,cbActionCard)
	--assert(cbActionCard~=0)
	--self.cbActionCard=cbActionCard or self.cbActionCard
	--获取node
	if self._scene.m_bLookOn then
		return;
	end
	local OpNode = self:getChildByName("FileNode_Op")
	local btGang = OpNode:getChildByName("Button_gang") 	--杠
	local btPeng = OpNode:getChildByName("Button_pen") 	--碰
	local btHu = OpNode:getChildByName("Button_hu") 	--胡
	local btGuo = OpNode:getChildByName("Button_guo") 	--过

	OpNode:setVisible(true)
	if cbActionMask == GameLogic.WIK_NULL or cbActionCard==nil or math.ceil(cbActionCard/16)==self._cardLayer.callcardKind then
		OpNode:setVisible(false)
		btGang:setEnabled(false)
		btPeng:setEnabled(false)
		btHu:setEnabled(false) 
		btGuo:setEnabled(false)
		self.cbActionCard=GameLogic.WIK_NULL
		return
	end

	self.cbActionCard=cbActionCard

	local showBtnList = {}
	btGang:setVisible(false)
	btPeng:setVisible(false)
	btHu:setVisible(false) 

	--通过动作码，判断操作按钮状态
	if bit:_and(cbActionMask, GameLogic.WIK_PENG) ~= GameLogic.WIK_NULL then
		btPeng:setEnabled(true)
		table.insert(showBtnList, btPeng)
	end

	if bit:_and(cbActionMask, GameLogic.WIK_GANG) ~= GameLogic.WIK_NULL then
		btGang:setEnabled(true)
		table.insert(showBtnList, btGang)
	end

	if bit:_and(cbActionMask, GameLogic.WIK_CHI_HU) ~= GameLogic.WIK_NULL then
		--showToast(self,"show 吃胡提示",3)
		btHu:setEnabled(true)
		table.insert(showBtnList, btHu)
	end
	btGuo:setEnabled(true)

	-- 显示的按钮重新排列
	for i , bt in ipairs(showBtnList) do
		bt:setPositionX(btGuo:getPositionX() - bt:getContentSize().width*1.5*i)
		bt:setVisible(true)
	end
end

--玩家指向刷新
function GameViewLayer:OnUpdataClockPointView(viewId) 

	local i=self._scene.myChairID

	--默认chairID为0->东， 3->北
	
	-- local chairIdToSeat={2,}
	-- i=chairIdToSeat[i+1]

	--print("指针文件路径", viewImage[viewId])
	local file=string.format("game/dir/dir_%d_%d.png",i,viewId)
	if not viewId or viewId == yl.INVALID_CHAIR then
	    self.userPoint:setVisible(false)
	else
		--通过ID设置纹理
		local sprPoint = cc.Sprite:create(file)
		if nil ~=  sprPoint then
			self.userPoint:setSpriteFrame(sprPoint:getSpriteFrame())
			self.userPoint:setVisible(true)
		end
	end
end

function  GameViewLayer:setShowLabelClock(bShow)
	self.labelClock:setVisible(bShow)
end

 --设置转盘时间
 function GameViewLayer:OnUpdataClockTime(time)
 	if 10 > time then
 		self.labelClock:setString(string.format("0%d", time))
 	else
 		self.labelClock:setString(string.format("%d", time))
 	end
 end

--刷新剩余牌数
function GameViewLayer:onUpdataLeftCard( numCard )
	if numCard<0 then self.m_nLeftCard=self.m_nLeftCard+numCard
	else self.m_nLeftCard= numCard end
	 self.leftcardAtlas:setString(string.format("%d",self.m_nLeftCard))
end


--显示规则信息
function GameViewLayer:SetCellScore( cellScore)
	-- --底注
	-- local labelCellScore = self.gameInfoNode:getChildByName("AtlasLabel_cellscore")
	-- labelCellScore:setString(string.format("%d", cellScore))
end

function GameViewLayer:SetBankerUser(bankerViewId)
	if self.m_tabUserHead[bankerViewId] then
		self.m_tabUserHead[bankerViewId]:showBank(true)
	end
end

--显示出牌
function GameViewLayer:showOutCard(viewid, value, isShow)
	if not isShow then
		self.sprOutCardBg:setVisible(false)
		return
	end

	if nil == value then  --无效值
		return
	end

	local posOurCard = 
	{
		cc.p(667, 230),
		cc.p(1085, 420),
		cc.p(260, 420),
		cc.p(667, 575)
	}
	print("玩家出牌， 位置，卡牌数值", viewid, value)
	self.sprOutCardBg:setVisible(isShow)
	self.sprOutCardBg:setPosition(posOurCard[viewid])
	--获取数值
	local cardIndex = GameLogic.SwitchToCardIndex(value)
	local sprPath = "card/my_normal/tile_me_"
	if cardIndex < 10 then
		sprPath = sprPath..string.format("0%d", cardIndex)..".png"
	else
		sprPath = sprPath..string.format("%d", cardIndex)..".png"
	end
	local spriteValue = display.newSprite(sprPath)
	--获取精灵
	local sprCard = self.sprMajong:getChildByName("card_value")
	if nil ~= sprCard then
		sprCard:setSpriteFrame(spriteValue:getSpriteFrame())
	end
end

--用户操作动画
function GameViewLayer:showOperateAction(viewId, charttype)
	-- body
	local posACtion = 
	{
		cc.p(667, 230),
		cc.p(1085, 420),
		cc.p(260, 420),
		cc.p(667, 575)
	}

	if charttype == 22 or charttype == 24 then
		  --加载动画
		local frames = {}
		local actionTime = 0.2
		local num = charttype == 22 and 8 or 6
		local name = charttype == 22 and "wind" or "rain"
		cc.SpriteFrameCache:getInstance():addSpriteFrames("game/"..name..".plist")
		for i=1,num do
		    local strName = string.format(name.."_%d.png", i)
		    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
		    if spriteFrame then
		        table.insert(frames, spriteFrame)
		    else
		        break
		    end
		end
		local animation = cc.Animation:createWithSpriteFrames(frames,actionTime)    
		local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format(name.."_%d.png", 1))
		local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
		self:addChild(spr, GameViewLayer.ZORDER_ACTION)
		spr:setPosition(posACtion[viewId])
		spr:setName("EMOJ")
		spr:runAction(cc.Animate:create(animation))
		self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0) ,cc.CallFunc:create(function()
		    self:removeChildByName("EMOJ")
		end)))
		return
	end

	local strPath=string.format("game/op%d.png",charttype)

	local spr = cc.Sprite:create(strPath)
	if spr==nil then return end
	spr:addTo(self, GameViewLayer.ZORDER_ACTION)
	spr:setPosition(posACtion[viewId])
	if charttype~=64 then
			spr:runAction(cc.Sequence:create(cc.FadeTo:create(1.5,0),cc.RemoveSelf:create()))
	else
		spr:runAction(cc.Sequence:create(cc.FadeTo:create(1.5,0),self:setShowHuSign(viewId,true),cc.RemoveSelf:create()))
	end
	self.operateResultSp=spr  
end

function GameViewLayer:setShowHuSign(viewId,bShow)
	if not bShow then
		if self.husigns[viewId] and not tolua.isnull(self.husigns[viewId]) then
			self.husigns[viewId]:removeSelf()
			self.husigns[viewId]=nil
		end
		return
	end

	local posACtion = 
	{
		cc.p(667, 130),
		cc.p(1185, 420),
		cc.p(200, 420),
		cc.p(667, 655)
	}
	local strPath=string.format("game/husign.png",charttype)

	local spr = cc.Sprite:create(strPath)
	if spr==nil then return end
	spr:addTo(self, GameViewLayer.ZORDER_HUSIGN)
	spr:setPosition(posACtion[viewId])
	self.husigns[viewId]=spr
end

--显示荒庄
function GameViewLayer:showNoWin( isShow )
	self.sprNoWin:setVisible(isShow)
end

--开始
function GameViewLayer:gameStart(startViewId, cbCardData, cbCardCount, cbUserAction,ChangeCardData)
	print("cbCardCount",cbCardCount)
	dump(cbCardData)
	--  切换换桌按钮
	self:switchChangeAndTrustBtn()

	self.btStart:setVisible(false)
	for i=1,cmd.GAME_PLAYER do
		self:showUserState(i, false)
	end
	assert(self.m_nLeftCard==108)
	self:OnUpdataClockPointView(5)
	self.labelClock:setVisible(false)

	--每次发四张,第四次一张
	local viewid = startViewId
	local tableView = {1, 2, 4, 3} --对面索引为3
	local cardIndex = 1 --读取自己卡牌的索引
	local actionList = {}

	local function callbackWithArgs(viewid, myCardDate, cardCount)
          local ret = function ()
          	self._cardLayer:sendCardToPlayer(viewid, myCardDate, cardCount)
          	self.m_nLeftCard=self.m_nLeftCard-cardCount
          	self:onUpdataLeftCard(self.m_nLeftCard)
          end
          return ret
    end

	for i=1,4 do
		local cardCount = (i == 4 and 1 or 4)
		for k=1,cmd.GAME_PLAYER do

			if 5 == viewid then
				viewid = 1
			end
			local myCardDate = {}
			if viewid == cmd.MY_VIEWID  then
				for j=1,cardCount do
					print("开始发牌,我的卡牌", cardIndex, cbCardData[cardIndex])
					myCardDate[j] = cbCardData[cardIndex]
					cardIndex = cardIndex +1
				end
			end

	        local callFun = cc.CallFunc:create(callbackWithArgs(tableView[viewid], myCardDate, cardCount))
	        table.insert(actionList, cc.DelayTime:create(0.3))
	        table.insert(actionList, callFun)
		    --如果是我要发卡牌信息过去
			viewid = viewid +1
		end
	end
	--发完手牌给庄家发牌
	local myCardDate = {}
	if startViewId == cmd.MY_VIEWID then
		myCardDate[1] = cbCardData[14]
	end
	
	local callFun = cc.CallFunc:create(callbackWithArgs(startViewId, myCardDate, 1))
	table.insert(actionList, cc.DelayTime:create(0.5))
    table.insert(actionList, callFun)
    table.insert(actionList, cc.DelayTime:create(0.5))
 
 	-- if not self._scene.m_bLookOn then
		if self._scene.bHuanSanZhang==true then
	  		table.insert(actionList,cc.CallFunc:create(function() 
	  			self._scene:SetGameClock(cmd.MY_VIEWID,cmd.IDI_CHANGE_CARD,5)
	  			self:OnUpdataClockTime(5)
	  			self.labelClock:setVisible(true)
	  			self._cardLayer:setPopChangeCard(ChangeCardData)
	  			self:showChangCardHint(true) 
	  			self:otherChangeStates(true)
	  		end))
	  	else
	    	--弹出选缺提示
	   	 	table.insert(actionList,cc.CallFunc:create(function()  
	   	 		self._scene:SetGameClock(cmd.MY_VIEWID,cmd.IDI_CALL_CARD,5)
	   	 		self:OnUpdataClockTime(5)
	   	 		self.labelClock:setVisible(true)
	   	 		self:showCallCardHint()
	   	 	end))
		end
    -- end
  	
	self:runAction(cc.Sequence:create(actionList))
end

function GameViewLayer:videoGameStart(startViewId)
	-- self:initGameInfo()

	for i=1,cmd.GAME_PLAYER do 
		assert(#self._scene.cbHandCardData[i]==(i==startViewId and 14 or 13))
		assert(#self._cardLayer.cbCardData[i]==0)
	end

	assert(startViewId>=1 and startViewId<=4)
	local nextViewId={cmd.RIGHT_VIEWID,cmd.TOP_VIEWID,cmd.MY_VIEWID,cmd.LEFT_VIEWID} --逆时针发牌
	--local nextViewId={cmd.LEFT_VIEWID,cmd.MY_VIEWID,cmd.TOP_VIEWID,cmd.RIGHT_VIEWID} --顺时针发牌
	--每次发四张,第四次一张
	local viewid = startViewId
	local cardIndex = {1,1,1,1} --读取自己卡牌的索引
	local actionList = {}
	print("viewid：",viewid)
	assert(viewid>=1 and viewid<=4)

	local function callbackWithArgs(viewid, CardDate, cardCount)
          local ret = function ()
          	self._cardLayer:sendCardToPlayer(viewid, CardDate, cardCount)
          	self.m_nLeftCard=self.m_nLeftCard-cardCount
          	self:onUpdataLeftCard(self.m_nLeftCard)
          end
          return ret
    end

	for i=1,4 do
		local cardCount = (i == 4 and 1 or 4)
		for k=1,cmd.GAME_PLAYER do
			print("i"..i,"k"..k,"viewid:",viewid)
			local cbCardData =self._scene.cbHandCardData[viewid]
			local tempCard={}
			for j=1,cardCount do
				tempCard[j] = cbCardData[cardIndex[viewid]]
				cardIndex[viewid] = cardIndex[viewid] +1
			end

	        local callFun = cc.CallFunc:create(callbackWithArgs(viewid, tempCard, cardCount))
	        table.insert(actionList, cc.DelayTime:create(0.3))
	        table.insert(actionList, callFun)
			viewid = nextViewId[viewid]
			assert(viewid>=1 and viewid<=4)
		end
	end
	--发完手牌给庄家发牌
	local tempCard={self._scene.cbHandCardData[startViewId][14]}
	
	local callFun = cc.CallFunc:create(callbackWithArgs(startViewId, tempCard, 1))
	table.insert(actionList, cc.DelayTime:create(0.3))
    table.insert(actionList, callFun)
	self:runAction(cc.Sequence:create(actionList))
end


-----------------------------------------------回放-------------------------------------
function GameViewLayer:updateCardsNode(handCard, outCard, activityCard, leftCard,callcards)
	--先清除数据
	self._cardLayer:onResetData()

	self._cardLayer:setCallcardKindList(callcards or {})

	--先设置碰杠的牌
	for i=1,cmd.GAME_PLAYER do
		local wViewChairId = self._scene:SwitchViewChairID(i - 1)
		if #activityCard[wViewChairId] > 0 then
			for j = 1, #activityCard[wViewChairId] do
				local cbOperateData = activityCard[wViewChairId][j]
				--排序
				table.sort(cbOperateData.cbCardValue, function (a, b)
					return a < b
				end)
				self._cardLayer:createActiveCardReEnter(wViewChairId, cbOperateData)
			end
		end
	end

	--手牌
	for i = 1, cmd.GAME_PLAYER do
		local viewId = self._scene:SwitchViewChairID(i - 1)
		if #handCard[viewId] > 0 then
			self._cardLayer:createHandCard(viewId, handCard[viewId], #handCard[viewId])
		end
	end

	--设置已经出的牌
	for i = 1, cmd.GAME_PLAYER do
		local viewId = self._scene:SwitchViewChairID(i - 1)
		if #outCard[viewId] > 0 then
			self._cardLayer:createOutCard(viewId, outCard[viewId], #outCard[viewId])
		end
	end
end


function GameViewLayer:showPlayRule(bHuanSanZhang,bHuJiaoZhuanYi)
	local str=""
	if bHuanSanZhang==true then str="换三张 " end
	if bHuJiaoZhuanYi==true then str=str.."呼叫转移" end
	if str~="" then str="本局玩法："..str end
	self:getChildByName("Text_playrule"):setString(str)
end

--用户出牌
function GameViewLayer:gameOutCard(viewId, card)

	print("用户出牌", viewId, card)
	self:showOutCard(viewId, card, true) --展示出牌

	-- if (viewId ~= cmd.MY_VIEWID or self._scene.m_bLookOn) then
	-- 	print("用户出牌----22222------", self._scene.m_bLookOn, self._scene.m_bTrustee)
	-- 	self._cardLayer:outCard(viewId, card)
	-- elseif (viewId == cmd.MY_VIEWID or self._scene.m_bLookOn) and not self._scene.m_bTrustee then
	-- 	print("用户出牌----3333------", self._scene.m_bLookOn, self._scene.m_bTrustee)
	-- 	self._cardLayer:outCardTrustee(viewId, card)
	-- elseif self._scene.m_bTrustee or self._scene.m_bLookOn then
	-- 	print("#############GameViewLayer:gameOutCard#####################",#self._cardLayer.cbCardData[cmd.MY_VIEWID],#self._cardLayer.cbCardData[cmd.MY_VIEWID]%3)
	-- 	self._cardLayer:outCardTrustee(card)
	-- --else
	-- 	--assert(false) --调试，自己不主动出牌时不应该执行到这里。 如果服务端发送sendcard到托管玩家，客户端在服务端给托管玩家发送outcard之前取消了托管。导致outcard消息到达时已经非托管状态
	-- end

	if viewId ~= cmd.MY_VIEWID then
		self._cardLayer:outCard(viewId, card)
	elseif viewId == cmd.MY_VIEWID and not self._scene.m_bTrustee then
		self._cardLayer:outCardTrustee(card)
	elseif self._scene.m_bTrustee or self._scene.m_bLookOn then
		print("#############GameViewLayer:gameOutCard#####################",#self._cardLayer.cbCardData[cmd.MY_VIEWID],#self._cardLayer.cbCardData[cmd.MY_VIEWID]%3)
		self._cardLayer:outCardTrustee(card)
	--else
		--assert(false) --调试，自己不主动出牌时不应该执行到这里。 如果服务端发送sendcard到托管玩家，客户端在服务端给托管玩家发送outcard之前取消了托管。导致outcard消息到达时已经非托管状态
	end


	self.cbOutCardTemp = card
	self.cbOutUserTemp = viewId
	--self._cardLayer:discard(viewId, card)
end

function GameViewLayer:showGangScore(scores) --分数飘字
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

	local tabcorePos={cc.p(660,200),cc.p(990,475),cc.p(300,475),cc.p(660,570)}
	for i=1,cmd.GAME_PLAYER do
		local filepath="game/winscore_digits.png"
		if scores[i]<0 then filepath="game/lostscore_digits.png" end
		if scores[i]~=0 then
			local scoreLabel=cc.LabelAtlas:create(formatScore(scores[i]),filepath,46,88,string.byte('.')):addTo(self,1000)
			scoreLabel:setAnchorPoint(cc.p(0.5,0.5))
			scoreLabel:setPosition(tabcorePos[self._scene:SwitchViewChairID(i-1)])
			local moveAct=cc.MoveBy:create(2,cc.p(5,50)) --JumpBy
			local removeAct=cc.CallFunc:create(function() scoreLabel:removeSelf() end)
			scoreLabel:runAction(cc.Sequence:create(moveAct,removeAct))
		end
	end
end

function GameViewLayer:setCallCardKind(viewid,callCardKind)--都选完缺了
	if callCardKind~=nil then --头像上显示选缺种类
		self:calledCard(viewid,callCardKind)
	end
end

function GameViewLayer:showChangCardHint(bShow)
	if false== bShow then
		self.changeCardBg:setVisible(false)
		self._cardLayer:finishChangeCard()
	else
		if not self._scene.m_bLookOn then
			self.changeCardBg:setVisible(true)
			
		end
		self._cardLayer:startChangeCard()
	end
end

function GameViewLayer:otherChangeStates(bShow)

	for i=2,4 do
		self.changeStatusSp[i]:setVisible(bShow)
	end
end

function GameViewLayer:getMyHandCards()
	return self._cardLayer.cbCardData[cmd.MY_VIEWID]
end

function GameViewLayer:createZhanjiView(tabScoreList,ncount)

	--for test
		-- tabScoreList={}
		-- for i=1,20 do
		-- 	table.insert(tabScoreList,{234,-34543,34,2})
		-- end
	---test end

	local function numberOfCellsInTableView()
			--return #tabScoreList --test
		return math.max(math.min(ncount,PriRoom:getInstance().m_tabPriData.dwPlayCount-1),0)
	end

	local function cellSizeForTable(view,idx) 
    	return 596,60
	end

	local function tableCellAtIndex(view, idx) --idx从0开始
		local tabScore=tabScoreList[idx+1]
     	local cell=view:dequeueCell()
    	if nil==cell then
    		cell=cc.TableViewCell:create()
    	end
    	if cell:getChildByTag(1)==nil and tabScore~=nil then
    		cc.CSLoader:createNode("game/Node_Zhanji_list.csb")
    			:addTo(cell)
    			:setTag(1)
    			:setPosition(9,5)
    	end
    	
    	local node=cell:getChildByTag(1)
    	local strJS=string.format("%d/%d",idx+1,PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)
    	if GlobalUserItem.bPrivateGoldRoom then
    		strJS=idx+1
    	end
    	node:getChildByName("jushu"):setString(strJS)
        for i=1,cmd.GAME_PLAYER do
        	local score=tabScore[i]
        	if score>=0 then 
        		score="+"..score 
        		node:getChildByName("score"..i):setTextColor(cc.c4b(0x21,0x7f,0x34,0xff))
        	else 
        		node:getChildByName("score"..i):setTextColor(cc.c4b(0x91,0x21,0x2e,0xff))
        	end
        	node:getChildByName("score"..i):setString(score)
        	--if nil==self.m_UserItem[i] then node:getChildByName("score"..i):setString("") end
        	if i~=cmd.MY_VIEWID and GlobalUserItem.bPrivateGoldRoom then
        		node:getChildByName("score"..i):setVisible(false)
        	end
        	if i == cmd.MY_VIEWID and GlobalUserItem.bPrivateGoldRoom then
        		node:getChildByName("jushu"):setPositionX(143)
        		node:getChildByName("score"..i):setPositionX(400)
        	end
        end
    
    	return cell
     end

    local tableView=cc.TableView:create(cc.size(596,300))
	tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(cc.p(10,0))
    tableView:setDelegate()
    tableView:registerScriptHandler(numberOfCellsInTableView,cc.NUMBER_OF_CELLS_IN_TABLEVIEW)  
    tableView:registerScriptHandler(cellSizeForTable,cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex,cc.TABLECELL_SIZE_AT_INDEX)
    tableView:reloadData()
    tableView:setVerticalFillOrder(0)
    tableView:addTo(self.infoLayer:getChildByName("liushuibg"))
end

function GameViewLayer:setTrustee(bTrustee)
	self.TrustShadow:setVisible(bTrustee)
end
--用户抓牌
function GameViewLayer:gameSendCard(viewId, card,bTrustee)
	if viewId==cmd.MY_VIEWID then
		print("MY_VIEWID GameViewLayer:gameSendCard:",card)
	end
	--发牌
	if viewId == cmd.MY_VIEWID then
		self._cardLayer:setMyCardTouchEnabled(true)
		self._cardLayer:sendCardToPlayer(viewId, {card}, 1,bTrustee)
	else
		self._cardLayer:sendCardToPlayer(viewId, nil, 1,bTrustee)
	end
end

function GameViewLayer:sendAskRCRecord( )

	if self.infoLayer  and not tolua.isnull(self.infoLayer) then
		self.infoLayer:removeSelf( )
		self.infoLayer=nil
		return
	else
		--showToast(self,"sendAskRCRecord",3)
		self._scene:sendAskRCRecord()
	end	
end

function GameViewLayer:showRoomInfo(tabScoreList,ncount)
	--if bShow==true then
		if self.infoLayer and not tolua.isnull(self.infoLayer) then
			self.infoLayer:setVisible(true)
		else
			self.infoLayer = cc.CSLoader:createNode(cmd.RES_PATH.."game/Layer_zhanji.csb")
			self.infoLayer:addTo(self, GameViewLayer.ZORDER_SETTING)
			self.infoLayer:getChildByName("Panel"):setSwallowTouches(true)	
			self.infoLayer:getChildByName("Panel"):setTouchEnabled(true)	
			self.infoLayer:getChildByName("Panel"):addTouchEventListener(function() 
				--self.infoLayer:removeSelf()  
				self.infoLayer:runAction(cc.Sequence:create(cc.DelayTime:create(0.05),cc.RemoveSelf:create()))
				end)
			

			self.infoLayer:getChildByName("Image_2"):setTouchEnabled(true)
			local wanfaBg=self.infoLayer:getChildByName("wanfabg")
			local liushuiBg=self.infoLayer:getChildByName("liushuibg")
			wanfaBg:getChildByName("btn_dismiss"):addClickEventListener(function() 
				PriRoom:getInstance():queryDismissRoom(self._scene.m_cbGameStatus, self._scene:onGetSitUserNum())
				self.infoLayer:removeSelf()
			end)

			local wanfa={self._scene.bHuanSanZhang,self._scene.bHuJiaoZhuanYi}
			local fanxing={self._scene.bZiMoAddTimes,self._scene.bTianDiHu,self._scene.bHaiDiLaoYue,self._scene.bMenQing}

			for i=1,2 do
				if wanfa[i] then
					
					local node=wanfaBg:getChildByName(string.format("Sprite_1_%d",i))
					cc.Sprite:create("game/choosed.png"):addTo(wanfaBg):setPosition(node:getPosition())
				end
			end

			for i=1,4 do
				if fanxing[i] then
					print("番型"..i)
					local node=wanfaBg:getChildByName(string.format("Sprite_2_%d",i))
					cc.Sprite:create("game/choosed.png"):addTo(wanfaBg):setPosition(node:getPosition())
				end
			end
			
			local funcA=function() wanfaBg:setVisible(true) liushuiBg:setVisible(false) end
			local funcB=function() wanfaBg:setVisible(false) liushuiBg:setVisible(true) end
			self.infoLayer:getChildByName("Button_wanfa"):setVisible(false)
			self.infoLayer:getChildByName("Button_liushui"):setVisible(false)
			if GlobalUserItem.bPrivateGoldRoom then
				wanfaBg:getChildByName("btn_dismiss"):setVisible(false)
			end
			liushuiBg:setVisible(true)
			wanfaBg:setVisible(false)

			local title=liushuiBg:getChildByName("title")
			title:getChildByName("jushu"):setString("局数")
			if GlobalUserItem.bPrivateGoldRoom then
				title:getChildByName("jushu"):setPositionX(143)
			end
			for i=1,cmd.GAME_PLAYER do
				if self.m_UserItem[i] then
					local strNickname = string.EllipsisByConfig(self.m_UserItem[i].szNickName, 80, 
	                                                string.getConfig("fonts/round_body.ttf", 20))
					title:getChildByName("score"..i):setString(strNickname)
					if i==cmd.MY_VIEWID then -- or i==mychairid 
						title:getChildByName("score"..i):setFontSize(24)
						title:getChildByName("score"..i):setTextColor(cc.c4b(0xba,0x0c,0x20,0xff))
						if GlobalUserItem.bPrivateGoldRoom then
							title:getChildByName("score"..i):setPositionX(400)
						end
					end
				else
					--local strNickname = string.EllipsisByConfig("啊as对方答复撒范德萨发", 80, 
	                                                --string.getConfig("fonts/round_body.ttf", 20)) --test
					--title:getChildByName("score"..i):setString(strNickname) --test
					title:getChildByName("score"..i):setString("")
				end

				if i~=cmd.MY_VIEWID and GlobalUserItem.bPrivateGoldRoom then
					title:getChildByName("score"..i):setVisible(false)
				end
			end

			self:createZhanjiView(tabScoreList,ncount)
		end
	-- else
	-- 	self.infoLayer:removeSelf()
	-- end
	
end

function GameViewLayer:removeSchedule()

    local schedule = cc.Director:getInstance():getScheduler()
    if self._schSingal then
        schedule:unscheduleScriptEntry(self._schSingal)
        self._schSingal = nil
    end
end
function GameViewLayer:initGameInfo(num, ServerName)
	local topText_1 = self.btnTop:getChildByName("Text_1")
	local topText_2 = self.btnTop:getChildByName("Text_2")
	topText_1:setVisible(false)
    topText_2:setVisible(false)
    print("GameViewLayer:initGameInfo===============",num, ServerName,PriRoom , GlobalUserItem.bPrivateRoom,PriRoom:getInstance().m_tabPriData.dwPlayCount .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)
	if GlobalUserItem.bPrivateRoom then
		topText_1:setString("房号:"..PriRoom:getInstance().m_tabPriData.dwPersonalRoomID)
	    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
	    if GlobalUserItem.bPrivateGoldRoom then
		    topText_2:setString("底分:"..num)
		else
		    topText_2:setString("局数:"..strcount)
		end
		topText_1:setVisible(true)
	    topText_2:setVisible(true)
	elseif num ~= nil and ServerName ~= nil then
		topText_1:setString("金币场:"..ServerName)
		topText_2:setString("底分:"..num)
		topText_1:setVisible(true)
	    topText_2:setVisible(true)
	end
end

function GameViewLayer:updatePlayCount()
	local topText_2 = self.btnTop:getChildByName("Text_2")
	if PriRoom and GlobalUserItem.bPrivateRoom then
	    if GlobalUserItem.bPrivateGoldRoom then
	    	topText_2:setString("底分："..self._scene.lCellScore)
	    else
		    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
			topText_2:setString("局数："..strcount)
		end
	end
end

function GameViewLayer:onClickLocation()
    local playerList = {}
    local validPlayerNum = 0
    for i=1,cmd.GAME_PLAYER do
        local viewid = self._scene:SwitchViewChairID(i-1)
        local useritem = self.m_UserItem[viewid]
        local isVisible = self.nodePlayer[viewid]:isVisible()
        if useritem and isVisible then
            useritem = clone(useritem)
            local playerData = {}
            playerData.uid = useritem.dwUserID
            playerData.viewid = viewid
            table.insert(playerList, playerData)
            validPlayerNum = validPlayerNum + 1
        end
    end

    local uiLocation = require(appdf.GAME_SRC .. "yule.sparrowxz.src.views.layer.uiLocation")
    local locationObj = uiLocation:ctor(self, playerList, validPlayerNum)
end
return GameViewLayer