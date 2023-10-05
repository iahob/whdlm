local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowhz.src.models.CMD_Game")

local GameViewLayer = class("GameViewLayer",function(scene)
	local gameViewLayer =  cc.CSLoader:createNode(cmd.RES_PATH.."game/GameScene.csb")
    return gameViewLayer
end)

require("client/src/plaza/models/yl")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.sparrowhz.src.models.GameLogic")
local CardLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowhz.src.views.layer.CardLayer")

local SetLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowhz.src.views.layer.SetLayer")
-- local GameChatLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local GameChatLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowhz.src.views.layer.GameChatLayer")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local SpineObj = appdf.req("client.src.external.SpineObj")
local schedule = cc.Director:getInstance():getScheduler()

local anchorPointHead = {cc.p(1, 1), cc.p(0, 0.5), cc.p(0, 0), cc.p(1, 0.5)}
local posHead = {cc.p(577, 295), cc.p(165, 332), cc.p(166, 257), cc.p(724, 273)}
local posReady = {cc.p(-333, 0), cc.p(135, 0), cc.p(516, -140), cc.p(-134, 0)}
local posPlate = {cc.p(667, 589), cc.p(237, 464), cc.p(667, 174), cc.p(1093, 455)}
local posChat = {cc.p(850, 668), cc.p(262, 507), cc.p(301, 319), cc.p(1052, 535)}

GameViewLayer.SP_TABLE_BT_BG		= 1					--桌子按钮背景
GameViewLayer.BT_CHAT 				= 41				--聊天按钮
GameViewLayer.BT_SET 				= 42				--设置
GameViewLayer.BT_EXIT	 			= 43				--退出按钮
GameViewLayer.BT_TRUSTEE 			= 44				--托管按钮
GameViewLayer.BT_HOWPLAY 			= 45				--玩法按钮
GameViewLayer.BT_CHANGE 			= 46 				--开始按钮

GameViewLayer.BT_RULE 				= 50

GameViewLayer.BT_SCORELIST 			= 51

GameViewLayer.BT_SIT 			= 200  --坐下

GameViewLayer.BT_BEGIN 			= 201 --房主开始游戏

GameViewLayer.BT_SWITCH 			= 2 				--按钮开关按钮


GameViewLayer.BT_START 				= 3 				--开始按钮

GameViewLayer.BT_VOICE 				= 5					--语音按钮（语音关闭）
-- GameViewLayer.BT_VOICEOPEN 			= 55				--语音按钮（语音开启）

GameViewLayer.SP_GAMEBTN 			= 6					--游戏操作按钮
GameViewLayer.BT_BUMP 				= 62				--游戏操作按钮碰
GameViewLayer.BT_BRIGDE 			= 63				--游戏操作按钮杠
GameViewLayer.BT_LISTEN 			= 64				--游戏操作按钮听
GameViewLayer.BT_WIN 				= 65				--游戏操作按钮胡
GameViewLayer.BT_PASS 				= 66				--游戏操作按钮过

GameViewLayer.SP_ROOMINFO 			= 7					--房间信息
GameViewLayer.TEXT_ROOMNUM 			= 1					--房间信息房号
GameViewLayer.TEXT_ROOMNAME 		= 2					--房间信息房名
GameViewLayer.TEXT_INDEX 			= 3					--房间信息局数
GameViewLayer.TEXT_INNINGS 			= 4					--房间信息剩多少局

GameViewLayer.SP_ANNOUNCEMENT 		= 8					--公告

GameViewLayer.SP_CLOCK 				= 9					--计时器
GameViewLayer.ASLAB_TIME 			= 1					--计时器时间

GameViewLayer.SP_LISTEN 			= 10				--听牌提示

GameViewLayer.NODEPLAYER_1 			= 11				--玩家节点1
GameViewLayer.NODEPLAYER_2 			= 12				--玩家节点2
GameViewLayer.NODEPLAYER_3 			= 13				--玩家节点3
GameViewLayer.NODEPLAYER_4 			= 14				--玩家节点4
GameViewLayer.SP_HEAD 				= 1					--玩家头像
GameViewLayer.SP_HEADCOVER 			= 2					--玩家头像覆盖层
GameViewLayer.TEXT_NICKNAME 		= 3					--玩家昵称
GameViewLayer.ASLAB_SCORE 			= 4					--玩家游戏币
--GameViewLayer.SP_READY 				= 5					--玩家准备标志
GameViewLayer.SP_TRUSTEE 			= 6					--玩家托管标志
GameViewLayer.SP_BANKER 			= 7					--庄家
GameViewLayer.SP_ROOMHOST 			= 8 				--房主

-- GameViewLayer.BT_EXIT	 			= 17				--退出按钮
-- GameViewLayer.BT_TRUSTEE 			= 18				--托管按钮

GameViewLayer.SP_PLATE 				= 19				--牌盘
GameViewLayer.SP_PLATECARD		 	= 1					--排盘中的牌

GameViewLayer.TEXT_REMAINNUM 		= 20				--牌堆剩多少张

GameViewLayer.SP_SICE1 				= 27				--筛子1
GameViewLayer.SP_SICE2 				= 28				--筛子2
GameViewLayer.SP_OPERATFLAG			= 29				--操作标志

GameViewLayer.SP_TRUSTEEBG 			= 1					--托管底图
GameViewLayer.BT_TRUSTEECANCEL 		= 30 				--取消托管


local zOrder =
{
	"LAYER_CARD",
	"LISTEN_BG",
	"CARD_PLATE",
	"BUTTON",
	"NODE_PLAYER",
	"LAYER_TRUSTEE",
	"PRI_ROOM_INFO",
	"MENU_BAR",
	"LAYER_CHAT",
	"LAYER_DISMISS",
	"PRI_ROOM_END",
	"LAYER_RESULT",
}
GameViewLayer.Z_ORDER_ENUM = ExternalFun.declarEnumWithTable(0, zOrder)



function GameViewLayer:onInitData()
	self.cbActionCard = 0
	self.cbOutCardTemp = 0

	--回放
	self.bIsVideo = false

	self.chatDetails = {}
	self.cbAppearCardIndex = {}
	self.zhuangUserId = -1

	self.m_beginTimeNum = 0
	--房卡需要
	self.m_sparrowUserItem = {nil,nil,nil,nil}
end

function GameViewLayer:onResetData()
	self._cardLayer:onResetData()

	self.spListenBg:removeAllChildren()
	self.spListenBg:setVisible(false)
	self.cbOutCardTemp = 0

	self.cbAppearCardIndex = {}
	local spFlag = self:getChildByTag(GameViewLayer.SP_OPERATFLAG)
	if spFlag then
		spFlag:removeFromParent()
	end
	self.spCardPlate:setVisible(false)
	self.spCardPlate:stopAllActions()
	self.spTrusteeCover:setVisible(false)
	for i = 1, cmd.GAME_PLAYER do
		self.nodePlayer[i]:getChildByTag(GameViewLayer.SP_TRUSTEE):setVisible(false)
		self.nodePlayer[i]:getChildByTag(GameViewLayer.SP_BANKER):setVisible(false)
	end
	self:setRemainCardNum(112)

	self:removeChildByTag(9898)
	self.spGameBtn:getChildByTag(GameViewLayer.BT_PASS):setEnabled(true):setColor(cc.c3b(255, 255, 255))
end

function GameViewLayer:onExit()
	print("GameViewLayer onExit")
    local schedule = cc.Director:getInstance():getScheduler()
    if self._timeShd then
        schedule:unscheduleScriptEntry(self._timeShd)
        self._timeShd = nil
    end
    if self._schSingal then
        schedule:unscheduleScriptEntry(self._schSingal)
        self._schSingal = nil
    end

    	if self._beginTimeId then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
		self._beginTimeId = nil
	end

	self._scene:KillGameClock()
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("gameScene.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("gameScene.png")
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
end

local this
function GameViewLayer:ctor(scene)
	this = self
	self._scene = scene
	self:onInitData()
	self:preloadUI()
	self:initButtons()
	self._cardLayer = CardLayer:create(self):addTo(self,GameViewLayer.Z_ORDER_ENUM.LAYER_CARD)							--牌图层

    	self._chatLayer = GameChatLayer:create(self._scene._gameFrame):addTo(self, GameViewLayer.Z_ORDER_ENUM.LAYER_CHAT)	--聊天框
    	self._setLayer = SetLayer:create(self):addTo(self, 4)
	--聊天泡泡
	self.chatBubble = {}
	for i = 1 , cmd.GAME_PLAYER do
		local strFile = ""
		if i == 1 or i == 4 then
			strFile = "#sp_bubble_2.png"
		else
			strFile = "#sp_bubble_1.png"
		end
		self.chatBubble[i] = display.newSprite(strFile, {scale9 = true ,capInsets = cc.rect(0, 0, 204, 68)})
			:setAnchorPoint(cc.p(0.5, 0.5))
			:move(posChat[i])
			:setVisible(false)
			:addTo(self, 3)
	end

	--节点事件
	local function onNodeEvent(event)
		if event == "exit" then
			self:onExit()
		end
	end
	self:registerScriptHandler(onNodeEvent)

	self.nodePlayer = {}
	for i = 1, cmd.GAME_PLAYER do
		self.nodePlayer[i] = self:getChildByName("FileNode_"..i)
		self.nodePlayer[i]:setLocalZOrder(1)
		self.nodePlayer[i]:setVisible(false)
		self.nodePlayer[i]:getChildByTag(GameViewLayer.SP_HEADCOVER):setLocalZOrder(1)
		self.nodePlayer[i]:getChildByTag(GameViewLayer.TEXT_NICKNAME):setLocalZOrder(1)
		self.nodePlayer[i]:getChildByName("sp_ready"):setLocalZOrder(1) --:move(posReady[i])
		local sp_trustee = self.nodePlayer[i]:getChildByTag(GameViewLayer.SP_TRUSTEE)
			:setVisible(false)
		local sp_banker = self.nodePlayer[i]:getChildByTag(GameViewLayer.SP_BANKER)
			:setLocalZOrder(1)
			:setVisible(false)
		local sp_roomHost = self.nodePlayer[i]:getChildByTag(GameViewLayer.SP_ROOMHOST)
			:setVisible(false)
		if i == 2 or i == cmd.MY_VIEWID then
			sp_trustee:move(65, -41)
			sp_banker:move(44, 55)
			sp_roomHost:move(-79, -24)
		end
	end

	self.spListenBg = self:getChildByTag(GameViewLayer.SP_LISTEN)
		:setLocalZOrder(3)
		:setVisible(false)
		:setScale(0.7)
	--庄家
	self:getChildByTag(GameViewLayer.SP_ANNOUNCEMENT):setLocalZOrder(2):setVisible(false)
	--托管覆盖层
	self.spTrusteeCover = cc.Layer:create():setVisible(false):addTo(self, 4)
	--阴影层
	display.newSprite(cmd.RES_PATH.."game/sp_trusteeCover.png")
		:move(667, 70)
		:setScaleY(1.6)
		:setTag(GameViewLayer.SP_TRUSTEEBG)
		:addTo(self.spTrusteeCover)
	--取消托管按钮
	self.btTrusteeCancel = ccui.Button:create("bt_trusteeCancel_1.png","bt_trusteeCancel_2.png","bt_trusteeCancel_1.png",ccui.TextureResType.plistType)
		:move(667, 70)
		:setTag(GameViewLayer.BT_TRUSTEECANCEL)
		:addTo(self.spTrusteeCover, 1)
        :setVisible(false)
	self.btTrusteeCancel:addTouchEventListener(function(ref, eventType)
			if self._scene.m_bLookOn then
			          	showToast(self, "观战中不可使用该功能", 1)
			          	return
		      	end

		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(ref:getTag(), ref)
		end
	end)
	display.newSprite(cmd.RES_PATH.."game/sp_trusteeMan.png")
		:move(1067, 70)
		:addTo(self.spTrusteeCover)
	self.spTrusteeCover:setTouchEnabled(false)
	-- self.spTrusteeCover:registerScriptTouchHandler(function(eventType, x, y)
	-- 	return self:onTrusteeTouchCallback(eventType, x, y)
	-- end)
	--牌盘
	self.spCardPlate = self:getChildByTag(GameViewLayer.SP_PLATE):setLocalZOrder(3):setVisible(false)
	display.newSprite("game/font_middle/card_down.png")
		:move(61, 74)
		--:setTag(GameViewLayer.SP_PLATECARD)
		--:setTextureRect(cc.rect(0, 0, 69, 107))
		:addTo(self.spCardPlate)
	display.newSprite("game/font_middle/font_3_5.png")
		:move(61, 82)
		:setTag(GameViewLayer.SP_PLATECARD)
		:addTo(self.spCardPlate)

	-- self.spClock = self:getChildByTag(GameViewLayer.SP_CLOCK)
	-- self.asLabTime = self:getChildByName("AsLab_time"):setString("0")

	self.spClock = cc.find(self, "sp_clock")
	self.asLabTime = self.spClock:getChildByName("AsLab_time"):setString("0")

    self.labelTime = cc.find(self, "sp_top_left/Text_time")
    self.labelTime:setString(os.date("%H:%M"))
    local function timeUpdate()
        self.labelTime:setString(os.date("%H:%M"))
    end
    self._timeShd = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 1, false)

    self:onScheduleSingal()
    self._schSingal = schedule:scheduleScriptFunc(function() self:onScheduleSingal() end, 5, false)
    cc.funAddChildListener(self, "bt_location", function() self:onClickLocation() end)
    cc.funAddChildListener(self, "bt_chat", function() self:onClickChat() end)
    cc.funAddChildListener(self, "background_sparrowHz", function() self:onClickBackground() end)

	if GlobalUserItem.bPrivateRoom then
	   	cc.funAddChildListener(self, "Image_top", function() self:showRoomInfo(true) end)
	end
end

function GameViewLayer:onClickChat()
	if self._scene.m_bLookOn then
	          	showToast(self, "观战中不可使用该功能", 1)
	          	return
      	end
	    self._chatLayer:showGameChat(true)
	    self:showTableBt(bVisible)
end

function GameViewLayer:onClickBackground()
	if self._scene.m_bLookOn then
	          	return
      	end

	if self.spTrusteeCover:isVisible() then
		self._scene:sendUserTrustee(false)
	end

	self:showTableBt(false)
end

function GameViewLayer:onClickLocation()

	if self._scene.m_bLookOn then
	          	showToast(self, "观战中不可使用该功能", 1)
	          	return
	end
    local playerList = {}
    local playerNum = self._scene.cbPlayerCount
    local validPlayerNum = 0
    for i=1,playerNum do
        local viewid = self._scene:SwitchViewChairID(i-1)
        local useritem = self.m_sparrowUserItem[viewid]
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

    local uiLocation = require(appdf.GAME_SRC .. "yule.sparrowhz.src.views.layer.uiLocation")
    local locationObj = uiLocation:ctor(self, playerList, validPlayerNum)

    -- local uiMaCollect = require(appdf.GAME_SRC .. "yule.sparrowhz.src.views.layer.uiMaCollect")
    -- local obj = uiMaCollect:ctor(self)
    -- obj:refresh({0x15,0x15,0x15,0x16,0x15,0x15})
end

function GameViewLayer:refreshMaCount()
	local maCount = self._scene.cbMaCount
	local spMa = cc.find(self, "Sprite_ma")
	-- if maCount == 1 then
	-- 	spMa:setTexture("game/img_ma_1.png")
	-- else
	-- 	spMa:setTexture(string.format("game/img_ma_%s.png", maCount))
	-- end
	spMa:setTexture(string.format("game/img_ma_%s.png", maCount))
end

function GameViewLayer:preloadUI()
    print("欢迎来到我的酒馆！")
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
			local strFile = cmd.RES_PATH.."Animate_sice_"..strColor..string.format("/sice_%d.png", index)
			local spFrame = cc.SpriteFrame:create(strFile, cc.rect(133*(j - 1), 0, 133, 207))
			animation:addSpriteFrame(spFrame)
		end

		local strName = "sice_"..strColor..string.format("_%d", index)
		animationCache:addAnimation(animation, strName)
	end

    -- 语音动画
    AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)
end

function GameViewLayer:initButtons()
	--按钮回调
	local btnCallback = function(ref, eventType)
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(ref:getTag(), ref)
		end
	end

	self.btExit = self:getChildByName("bt_exit")	--退出
	self.btExit:setTag(GameViewLayer.BT_EXIT)
	self.btExit:setLocalZOrder(GameViewLayer.Z_ORDER_ENUM.MENU_BAR +1)
	self.btExit:addTouchEventListener(btnCallback)
	self.btExit:setVisible(false)

	local btSet = self.btExit:getChildByName("bt_set")
	btSet:setTag(GameViewLayer.BT_SET)
	btSet:addTouchEventListener(btnCallback)

	self.btTrustee = self.btExit:getChildByName("bt_trustee")	--托管
	self.btTrustee:setTag(GameViewLayer.BT_TRUSTEE)
	self.btTrustee:addTouchEventListener(btnCallback)

	local btChange = self.btExit:getChildByName("bt_change")
	btChange:setTag(GameViewLayer.BT_CHANGE)
	btChange:addTouchEventListener(btnCallback)
	btChange:setPosition(self.btTrustee:getPosition())
	if GlobalUserItem.bPrivateRoom then
		btChange:setVisible(false)
	end
	self.btChange = btChange
	--local btChat = self.spTableBtBg:getChildByTag(GameViewLayer.BT_CHAT)	--聊天
	--btChat:addTouchEventListener(btnCallback)

	
	--local btHowPlay = self.spTableBtBg:getChildByTag(GameViewLayer.BT_HOWPLAY)	--玩法
	--btHowPlay:addTouchEventListener(btnCallback)

	--桌子按钮开关
	self.btSwitch = self:getChildByTag(GameViewLayer.BT_SWITCH)
		:setLocalZOrder(2)
	self.btSwitch:addTouchEventListener(btnCallback)


	--开始
	self.btStart = self:getChildByName("bt_start")
		:setLocalZOrder(2)
		:setVisible(false)
	self.btStart:addTouchEventListener(btnCallback)

	self.btSitAndBegin = self:getChildByName("bt_sit")
	self.btSitAndBegin:setTag(GameViewLayer.BT_SIT)
	self.btSitAndBegin:setLocalZOrder(2)
	self.btSitAndBegin:addTouchEventListener(btnCallback)
	self.btSitAndBegin:setVisible(false)	

	-- --游戏操作按钮
	-- self.spGameBtn = self:getChildByTag(GameViewLayer.SP_GAMEBTN)
	self.spGameBtn = cc.find(self, "Node_op")
		:setLocalZOrder(3)
		:setVisible(false)
	local btBump = self.spGameBtn:getChildByTag(GameViewLayer.BT_BUMP) 	--碰
		:setEnabled(false)
		:setColor(cc.c3b(158, 112, 8))
	btBump:addTouchEventListener(btnCallback)
	local btBrigde = self.spGameBtn:getChildByTag(GameViewLayer.BT_BRIGDE) 		--杠
		:setEnabled(false)
		:setColor(cc.c3b(158, 112, 8))
	btBrigde:addTouchEventListener(btnCallback)
	local btWin = self.spGameBtn:getChildByTag(GameViewLayer.BT_WIN)		--胡
		:setEnabled(false)
		:setColor(cc.c3b(158, 112, 8))
	btWin:addTouchEventListener(btnCallback)
	local btPass = self.spGameBtn:getChildByTag(GameViewLayer.BT_PASS)		--过
		:setEnabled(false)
		:setColor(cc.c3b(158, 112, 8))
	btPass:addTouchEventListener(btnCallback)

	--语音
	local btVoice = self:getChildByTag(GameViewLayer.BT_VOICE)
	btVoice:setLocalZOrder(3)
	--btVoice:setVisible(false)
	btVoice:addTouchEventListener(function(ref, eventType)
		if self._scene.m_bLookOn then
		          	showToast(self, "观战中不可使用该功能", 1)
		          	return
	      	end
		if eventType == ccui.TouchEventType.began then
			self._scene._scene:startVoiceRecord()
        elseif eventType == ccui.TouchEventType.ended
        	or eventType == ccui.TouchEventType.canceled then
            self._scene._scene:stopVoiceRecord()
        end
	end)
end

function GameViewLayer:showTableBt(bVisible)
	-- self.btSwitch:setVisible(not bVisible)
	self.btExit:setVisible(bVisible)

	return true
end

function GameViewLayer:deletePlayerInfo(userItem)

     --print("1hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh")
     if self._scene.isPriOver  and  GlobalUserItem.bPrivateRoom  then
	return
     end

     -- if self._scene.isGameStart and  GlobalUserItem.bPrivateRoom then

     -- 	return
     -- end

     for k,v in pairs(self.m_sparrowUserItem) do
          if v and v.dwUserID == userItem.dwUserID then
              self.nodePlayer[k]:setVisible(false)
              v = nil
          end
     end
end


--更新用户显示
function GameViewLayer:OnUpdateUser(viewId, userItem,flag,cleanReady)
	--print("888888888888888888888888888888888888")
	if not viewId or viewId == yl.INVALID_CHAIR then
		print("OnUpdateUser viewId is nil")
		return
	end
	--print("uuuuuuuuuuuuuuuuuuu,"..viewId)
	if self._scene.isPriOver  and  GlobalUserItem.bPrivateRoom  then
		return
     	end


	--头像
dump(userItem , "userItem")
	if  userItem then
		self.m_sparrowUserItem[viewId] = clone(userItem)
		-- print("ttttttttttttttttttttttttttttttt,"..viewId..","..userItem.szNickName)
		self.nodePlayer[viewId]:setVisible(true)
		self.nodePlayer[viewId]:getChildByName("sp_ready"):setVisible(userItem.cbUserStatus == yl.US_READY)
		if cleanReady then
			self.nodePlayer[viewId]:getChildByName("sp_ready"):setVisible(false)
		end

		if GlobalUserItem.bVideo or (viewId == cmd.MY_VIEWID and userItem.cbUserStatus >= yl.US_READY) then
			self.btStart:setVisible(false)
		end

		self.nodePlayer[viewId]:removeChildByTag(GameViewLayer.SP_HEAD)
	          local head = PopupInfoHead:createNormal(userItem, 82)
		      head:setPosition(1, 12)			--初始位置
		      head:enableHeadFrame(false)
		      if not self._scene.m_bLookOn then 
		      	head:enableInfoPop(true, posHead[viewId], anchorPointHead[viewId])			--点击弹出的位置0
		      end
		      head:setTag(GameViewLayer.SP_HEAD)
		      self.nodePlayer[viewId]:addChild(head)
			  local param = {}
			  param.isProp = true     --显示道具互动，百人游戏不显示
			  param.imgPath = "game/popInfo/popBg.png" --弹出框背景
			  head:setPropInfo(param)
              head:setPopCsbFile("game/popInfo/GamePopInfo.csb") -- 自定义CSB


		if flag and not  GlobalUserItem.bPrivateGoldRoom  then
			convertToGraySprite(head.m_head.m_spRender)
		end


		self:updateUserScore(viewId,userItem.lScore)

		--昵称
		self.nodePlayer[viewId]:removeChildByTag(776)
    		local clipNick = ClipText:createClipText(cc.size(80, 16), userItem.szNickName,"fonts/round_body.ttf",14)
    		clipNick:setAnchorPoint(cc.p(0.5, 0.5))
   		clipNick:setPosition(-1, -43)
    		clipNick:setTag(776)
    		self.nodePlayer[viewId]:addChild(clipNick,100)
		--local strNickname = string.EllipsisByConfig(userItem.szNickName, 90, string.getConfig("fonts/round_body.ttf", 14))
		--self.nodePlayer[viewId]:getChildByTag(GameViewLayer.TEXT_NICKNAME):setString(strNickname)

		self._scene.cbGender[viewId] = userItem.cbGender
		--print("9999999999")
		if GlobalUserItem.bPrivateRoom  then
			--print("9999999999,"..userItem.dwUserID..","..PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID..","..viewId)
			if userItem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID and not GlobalUserItem.bPrivateGoldRoom then
				self._scene.wRoomHostViewId = viewId
				--self.nodePlayer[viewId]:getChildByName("sp_roomHost"):setVisible(true)
			end
		end
	else
		self.nodePlayer[viewId]:setVisible(false)
	end
end

function GameViewLayer:updateUserScore(viewId,lScore)

	local strScore = self:numInsertPoint(lScore)

	self.nodePlayer[viewId]:getChildByName("playScore"):setString(strScore)
	 if GlobalUserItem.bPrivateRoom then
	            if PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
	                if  PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then

	                    self.nodePlayer[viewId]:getChildByTag(GameViewLayer.ASLAB_SCORE):setString(lScore - PriRoom:getInstance().m_tabPriData.lIniScore)

	                end

	            else
	                self.nodePlayer[viewId]:getChildByTag(GameViewLayer.ASLAB_SCORE):setString("0")
	            end

	 end
	 if self.bIsVideo and PriRoom:getInstance().m_tabPriData.lIniScore then
            		self.nodePlayer[viewId]:getChildByTag(GameViewLayer.ASLAB_SCORE):setString(lScore - PriRoom:getInstance().m_tabPriData.lIniScore)

        	end
end

--用户聊天
function GameViewLayer:userChat(wViewChairId, chatString)
	if chatString and #chatString > 0 then
		self._chatLayer:showGameChat(false)
		--取消上次
		if self.chatDetails[wViewChairId] then
			self.chatDetails[wViewChairId]:stopAllActions()
			self.chatDetails[wViewChairId]:removeFromParent()
			self.chatDetails[wViewChairId] = nil
		end

		--创建label
		local limWidth = 24*12
		local labCountLength = cc.Label:createWithTTF(chatString,"fonts/round_body.ttf", 24)
		if labCountLength:getContentSize().width > limWidth then
			self.chatDetails[wViewChairId] = cc.Label:createWithTTF(chatString,"fonts/round_body.ttf", 24, cc.size(limWidth, 0))
		else
			self.chatDetails[wViewChairId] = cc.Label:createWithTTF(chatString,"fonts/round_body.ttf", 24)
		end
		self.chatDetails[wViewChairId]:setColor(cc.c3b(0, 0, 0))
		self.chatDetails[wViewChairId]:move(posChat[wViewChairId].x, posChat[wViewChairId].y + 15)
		self.chatDetails[wViewChairId]:setAnchorPoint(cc.p(0.5, 0.5))
		self.chatDetails[wViewChairId]:addTo(self, 3)

	    --改变气泡大小
		self.chatBubble[wViewChairId]:setContentSize(self.chatDetails[wViewChairId]:getContentSize().width+38, self.chatDetails[wViewChairId]:getContentSize().height + 54)
			:setVisible(true)
		--动作
	    self.chatDetails[wViewChairId]:runAction(cc.Sequence:create(
	    	cc.DelayTime:create(3),
	    	cc.CallFunc:create(function(ref)
	    		self.chatDetails[wViewChairId]:removeFromParent()
				self.chatDetails[wViewChairId] = nil
				self.chatBubble[wViewChairId]:setVisible(false)
	    	end)))
    end
end

--用户表情
function GameViewLayer:onUserExpression(chatdata, viewId)
	local playerNode = self.nodePlayer[viewId]
	if not playerNode then
		print("不存在的用户,viewid========", viewId)
		return
	end

	local idx = chatdata.wItemIndex
    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", idx))
    local frames = {}
    local actionTime = 0.2
    for i=1,32 do
        local strName = string.format("emoj%d_%d.png",idx, i)
        print("browChatSprName--------------------->", strName)
        local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
        if spriteFrame then
            table.insert(frames, spriteFrame)
        else
            break
        end
    end
    local animation = cc.Animation:createWithSpriteFrames(frames, actionTime)
    animation:setLoops(-1)
    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", idx))
    local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
    playerNode:addChild(spr, 101, 1)
    spr:runAction(cc.Animate:create(animation))
    playerNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0), cc.CallFunc:create(function()
    	spr:removeFromParent()
    end)))
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = 1

        --获取头像坐标
        for k,v in pairs(self.m_sparrowUserItem) do
              if v then
                  -- local head = self.nodePlayer[k]:getChildByTag(GameViewLayer.SP_HEAD)
                  if v.dwUserID ==  dwUserID then
                        beginPoint = cc.p(self.nodePlayer[k]:getPosition())
                  elseif v.dwUserID ==  dwRecvUserID then
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

function GameViewLayer:onUserVoiceStart(viewId)
	--取消上次
	if self.chatDetails[viewId] then
		self.chatDetails[viewId]:stopAllActions()
		self.chatDetails[viewId]:removeFromParent()
		self.chatDetails[viewId] = nil
	end
     -- 语音动画
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    param.m_strName = cmd.VOICE_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    self.m_actVoiceAni = cc.RepeatForever:create(animate)

    self.chatDetails[viewId] = cc.Sprite:create("game/blank.png")
    	:move(posChat[viewId].x, posChat[viewId].y + 9)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:addTo(self, 3)
	if viewId == 2 or viewId == 3 then
		self.chatDetails[viewId]:setRotation(180)
	end
	self.chatDetails[viewId]:runAction(self.m_actVoiceAni)

    --改变气泡大小
	self.chatBubble[viewId]:setVisible(true)
end

function GameViewLayer:onUserVoiceEnded(viewId)
	if self.chatDetails[viewId] then
	    self.chatDetails[viewId]:removeFromParent()
	    self.chatDetails[viewId] = nil
	    self.chatBubble[viewId]:setVisible(false)
	end
end




function GameViewLayer:showSitBtn(visible,time)
	if self._scene.m_bLookOn then 
		self.btSitAndBegin:setTag(GameViewLayer.BT_SIT)

		self.btSitAndBegin:loadTextureNormal("game/btn_sit1.png")
                	self.btSitAndBegin:loadTexturePressed("game/btn_sit2.png")
                	self.btSitAndBegin:setVisible(true)

                	self.btSitAndBegin:getChildByName("Text_2"):setVisible(false)
	else
		self.btSitAndBegin:setTag(GameViewLayer.BT_BEGIN)

		self.btSitAndBegin:loadTextureNormal("game/bt_start_1.png")
                	self.btSitAndBegin:loadTexturePressed("game/bt_start_2.png")		
		if visible then
			if not self._beginTimeId and time then
		                 self.m_beginTimeNum = time
		                 self.btSitAndBegin:getChildByName("Text_2"):setString("("..self.m_beginTimeNum..")")
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
		self.btSitAndBegin:setVisible(visible)
	end
	
end


function GameViewLayer:updateBeginTime()
	if self.m_beginTimeNum and self.m_beginTimeNum >0 then 
		self.m_beginTimeNum = self.m_beginTimeNum -1
		self.btSitAndBegin:getChildByName("Text_2"):setString("("..self.m_beginTimeNum..")")	
	else
		if self._beginTimeId then
			cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
			self._beginTimeId = nil
		end
	end

end



-- 换桌和托管按钮切换显示
function GameViewLayer:switchChangeAndTrustBtn()
	if self._scene.m_cbGameStatus == cmd.GAME_SCENE_FREE then
		self.btTrustee:setVisible(false)
		self.btChange:setVisible(true)
	else
		self.btTrustee:setVisible(true)
		self.btChange:setVisible(false)
	end
	if GlobalUserItem.bPrivateRoom then
	    if GlobalUserItem.bPrivateGoldRoom then
			self.btTrustee:setVisible(true)
			self.btChange:setVisible(false)
		else
			self.btChange:setVisible(false)
			self.btChange:setEnabled(false)
			self.btTrustee:setVisible(false)
		end
	end
end

function GameViewLayer:onButtonClickedEvent(tag, ref)
	if tag == GameViewLayer.BT_START then

		if PriRoom and GlobalUserItem.bPrivateRoom then
		        if PriRoom:getInstance().m_tabPriData.dwPlayCount == PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 
		        	self._scene.m_bLookOn  then
		        	return
		        end
		end

		print("红中麻将开始！")
		--self.btStart:setVisible(false)
		self:showTableBt(false)
		self._scene:sendGameStart()
		self:switchChangeAndTrustBtn()
	elseif tag == GameViewLayer.BT_SIT then

		self._scene._gameFrame.bChangeDesk = true
		self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)
	elseif tag == GameViewLayer.BT_BEGIN then

		self.btSitAndBegin:setVisible(false)
		self._scene._gameFrame:SendUserBegin()

	elseif tag == GameViewLayer.BT_SWITCH then
		print("按钮开关")
		if self.btExit:isVisible() then
			self:showTableBt(false)
		else
			self:showTableBt(true)
		end
		self:switchChangeAndTrustBtn()
	elseif tag == GameViewLayer.BT_CHAT then
		if self._scene.m_bLookOn then
		          	showToast(self, "观战中不可使用该功能", 1)
		          	return
	      	end
		print("聊天！")
		self:showTableBt(false)
		self._chatLayer:showGameChat(true)
		--self._chatLayer:setLocalZOrder(yl.MAX_INT)
	elseif tag == GameViewLayer.BT_SET then
		print("设置开关")
		self:showTableBt(false)
		self._setLayer:showLayer()
	elseif tag == GameViewLayer.BT_CHANGE then
		print("换桌")
		self:showTableBt(false)
		if self._scene.m_cbGameStatus == cmd.GAME_SCENE_FREE then
			self._scene._gameFrame:QueryChangeDesk()
			for k,v in pairs(self.m_sparrowUserItem) do
			          if v then
			              self.nodePlayer[k]:setVisible(false)
			              self.nodePlayer[k]:getChildByName("sp_ready"):setVisible(false)
			          end
			end
			self.btStart:setVisible(true)
		else
			showToast(self, "正在游戏中，请先结束游戏", 2)
		end



	elseif tag == GameViewLayer.BT_HOWPLAY then
		print("玩法！")
		self:showTableBt(false)
        		--self._scene._scene:popHelpLayer(yl.HTTP_URL .. "/Mobile/Introduce.aspx?kindid=389&typeid=0")
		self._scene._scene:popHelpLayer2(389, 0)

	elseif tag == GameViewLayer.BT_EXIT then
		print("退出！")

		local bNoGame = false --没有参与游戏
		for i=1,#self._scene.m_playingUser do
		        if self._scene.m_playingUser[i] == GlobalUserItem.tabAccountInfo.dwUserID  then
		            bNoGame = false
		            break
		        elseif i == #self._scene.m_playingUser then
		            bNoGame = true
		        end
		end

		if self._scene.m_bLookOn or bNoGame then
		        self._scene:onExitTable()
		        return
		end
		self._scene:onQueryExitGame()
	elseif tag == GameViewLayer.BT_TRUSTEE then
		if self._scene.m_bLookOn then
		          	showToast(self, "观战中不可使用该功能", 1)
		          	return
	      	end
		print("托管")
		self:showTableBt(false)
		self._scene:sendUserTrustee(true)
	elseif tag == GameViewLayer.BT_TRUSTEECANCEL then
		if self._scene.m_bLookOn then
		          	return
	      	end
		print("取消托管")
		self._scene:sendUserTrustee(false)

	elseif tag == GameViewLayer.BT_BUMP then
		print("碰！")

		--发送碰牌
		local cbOperateCard = {self.cbActionCard, self.cbActionCard, self.cbActionCard}
		self._scene:sendOperateCard(GameLogic.WIK_PENG, cbOperateCard)

		self:HideGameBtn()
	elseif tag == GameViewLayer.BT_BRIGDE then
		print("杠！")
		local cbGangCard = self._cardLayer:getGangCard(self.cbActionCard)
		local cbOperateCard = {cbGangCard, cbGangCard, cbGangCard}
		print("kkkkkkkkkkkkkkkkkkk"..cbGangCard)
		self._scene:sendOperateCard(GameLogic.WIK_GANG, cbOperateCard)

		self:HideGameBtn()
	elseif tag == GameViewLayer.BT_WIN then
		print("胡！")

		local cbOperateCard = {self.cbActionCard, 0, 0}
		self._scene:sendOperateCard(GameLogic.WIK_CHI_HU, cbOperateCard)

		self:HideGameBtn()
	elseif tag == GameViewLayer.BT_PASS then
		print("过！")
		local cbOperateCard = {0, 0, 0}
		self._scene:sendOperateCard(GameLogic.WIK_NULL, cbOperateCard)

		self:HideGameBtn()
	else
		print("default")
	end
end

--计时器刷新
function GameViewLayer:OnUpdataClockView(viewId, time)

	local myChairID = yl.INVALID_CHAIR
	local chairCount = self._scene.chairCount
	    if self._scene:GetMeChairID() ~= yl.INVALID_CHAIR then
	    	myChairID = self._scene:GetMeChairID()
	    else
	    	myChairID = self._scene.myChairID
	    end


	if not viewId or viewId == yl.INVALID_CHAIR or not time then
		--self.spClock:setVisible(false)
		--print("hhhhhhhhhhhhhhhhhhhhhhhh1")
		self.asLabTime:setString(0)
	else
		--self.spClock:setVisible(true)
		--local res = string.format("sp_clock_%d.png", viewId)
		--self.spClock:setSpriteFrame(res)
		self.asLabTime:setString(time)

		if chairCount ==2 then
			local du = 0
			local index = 0
			if myChairID == 0 then
				index =  viewId == 3 and 1 or 3
			else
				du = 180
				index =  viewId == 3 and 3 or 1
			end


			self.spClock:setSpriteFrame("sp_clock_"..index..".png")
			self.spClock:setRotation(du)
			self.asLabTime:setRotation(-du)
			self.spClock:setVisible(true)
		else
			local rotaiton = {{3,2,1,4},{2,1,4,3},{1,4,3,2},{4,3,2,1}}

			self.spClock:setSpriteFrame("sp_clock_"..rotaiton[myChairID+1][viewId]..".png")
			self.spClock:setRotation(90*myChairID)
			self.asLabTime:setRotation(-90*myChairID)
			self.spClock:setVisible(true)

			-- print(myChairID.."============="..rotaiton[myChairID+1][viewId])
		end


	end
end

--开始
function GameViewLayer:gameStart( cbCardData, cbCardCount)
	self._cardLayer:sendCard(cbCardData, cbCardCount)
	self:switchChangeAndTrustBtn()
end
--用户出牌
function GameViewLayer:gameOutCard(viewId, card)
	self:showCardPlate(viewId, card)
	self._cardLayer:removeHandCard(viewId, {card}, true)

	self.cbOutCardTemp = card
	self.cbOutUserTemp = viewId
	--self._cardLayer:discard(viewId, card)
	--把上一个人打出的牌丢入弃牌堆
	if self.cbOutCardTemp ~= 0 then
		self._cardLayer:discard(self.cbOutUserTemp, self.cbOutCardTemp)
		self.cbOutUserTemp = nil
		self.cbOutCardTemp = 0
	end

	--清理之前的出牌
	self.spCardPlate:runAction(cc.Sequence:create(
		cc.DelayTime:create(0.5),
		cc.CallFunc:create(function()
			self:showCardPlate(nil)
			self:showOperateFlag(nil)
		end)))
end
--用户抓牌
function GameViewLayer:gameSendCard(viewId, card, bTail)

	--当前的人抓牌
	if self.bIsVideo then
		self._cardLayer:catchCardVideo(viewId, card)
	else
		self._cardLayer:catchCard(viewId, card, bTail)
	end
end


function GameViewLayer:sendCardFinish()

	self._scene:sendCardFinish()
end

function GameViewLayer:gameConclude()
    	for i = 1, cmd.GAME_PLAYER do
		self:setUserTrustee(i, false)
	end
	self._cardLayer:gameEnded()
end

function GameViewLayer:HideGameBtn()
	for i = GameViewLayer.BT_BUMP, GameViewLayer.BT_PASS do
		local bt = self.spGameBtn:getChildByTag(i)
		if bt then
			bt:setEnabled(false)
			bt:setColor(cc.c3b(158, 112, 8))
		end
	end
	self.spGameBtn:setVisible(false)
end

--识别动作掩码
function GameViewLayer:recognizecbActionMask(cbActionMask, cbCardData, noClock)
	print("收到提示操作：", cbActionMask, cbCardData)
	if cbActionMask == GameLogic.WIK_NULL or cbActionMask == 32 then
		assert("false")
		return false
	end

	if self._scene.m_bLookOn then 
		return 
	end 

	if cbCardData and cbCardData ~= 0 then
		self.cbActionCard = cbCardData
	end

	local enableBtnList = {}

	if bit:_and(cbActionMask, GameLogic.WIK_PENG) ~= GameLogic.WIK_NULL then
		if self._cardLayer:isUserCanPeng() then
			local btn = self.spGameBtn:getChildByTag(GameViewLayer.BT_BUMP)
			btn:setEnabled(true)
			btn:setColor(cc.c3b(255, 255, 255))
			table.insert(enableBtnList, btn)
		end
	end

	--通过动作码，判断操作按钮状态
	if bit:_and(cbActionMask, GameLogic.WIK_GANG) ~= GameLogic.WIK_NULL then
		local btn = self.spGameBtn:getChildByTag(GameViewLayer.BT_BRIGDE)
		btn:setEnabled(true)
		btn:setColor(cc.c3b(255, 255, 255))

		table.insert(enableBtnList, btn)
	end

	if bit:_and(cbActionMask, GameLogic.WIK_CHI_HU) ~= GameLogic.WIK_NULL then
		local btn = self.spGameBtn:getChildByTag(GameViewLayer.BT_WIN)
			btn:setEnabled(true)
			btn:setColor(cc.c3b(255, 255, 255))
		table.insert(enableBtnList, btn)
	end

	if not self._cardLayer:isUserMustWin() then
		local btn = self.spGameBtn:getChildByTag(GameViewLayer.BT_PASS)
				btn:setEnabled(true)
				btn:setColor(cc.c3b(255, 255, 255))
		table.insert(enableBtnList, btn)
	end

	---------------------------------------只显示操作的按钮
	for k,child in pairs(self.spGameBtn:getChildren()) do
		child:setVisible(false)
	end
	local totalBtnCount = #enableBtnList
	for k,btn in pairs(enableBtnList) do
		btn:setVisible(true)
		local idx = totalBtnCount - k + 1
		btn:setPositionX(-153*(idx-1)-212)
	end
	---------------------------------------只显示操作的按钮

	self.spGameBtn:setVisible(true)
    if not noClock then
	   self._scene:SetGameOperateClock()
    end
	return true
end

function GameViewLayer:getAnimate(name, bEndRemove)
	local animation = cc.AnimationCache:getInstance():getAnimation(name)
	local animate = cc.Animate:create(animation)

	if bEndRemove then
		animate = cc.Sequence:create(animate, cc.CallFunc:create(function(ref)
			ref:removeFromParent()
		end))
	end

	return animate
end
--设置听牌提示
function GameViewLayer:setListeningCard(cbCardData,leftCount)
	if cbCardData == nil or self._scene.m_bLookOn then
		self.spListenBg:setVisible(false)
		return
	end
	assert(type(cbCardData) == "table")
	self.spListenBg:removeAllChildren()
	self.spListenBg:setVisible(true)

	local cbCardCount = #cbCardData
	local bTooMany = (cbCardCount >= 16)
	--拼接块
	local width = 44
	local height = 67
	local posX = 327
	local fSpacing = 100
	if not bTooMany then
		for i = 1, fSpacing*cbCardCount do
			display.newSprite("#sp_listenBg_2.png")
				:move(posX, 46.5)
				:setAnchorPoint(cc.p(0, 0.5))
				:addTo(self.spListenBg)
			posX = posX + 1
			if i > 700 then
				break
			end
		end
	end
	--尾块
	display.newSprite("#sp_listenBg_3.png")
		:move(posX, 46.5)
		:setAnchorPoint(cc.p(0, 0.5))
		:addTo(self.spListenBg)
	--可胡牌过多，屏幕摆不下
	if bTooMany then
		local cardBack = display.newSprite("game/font_small/card_down.png")
			:move(183 + 40, 46)
			:addTo(self.spListenBg)
		local cardFont = display.newSprite("game/font_small/font_3_5.png")
			:move(width/2, height/2 + 8)
			:addTo(cardBack)

		local strFilePrompt = ""
		local spListenCount = nil
		if cbCardCount == 28 then 		--所有牌
			strFilePrompt = "#389_sp_listen_anyCard.png"
		else
			strFilePrompt = "#389_sp_listen_manyCard.png"
			spListenCount = cc.Label:createWithTTF(cbCardCount.."", "fonts/round_body.ttf", 30)
		end

		local spPrompt = display.newSprite(strFilePrompt)
			:move(183 + 110, 46)
			:setAnchorPoint(cc.p(0, 0.5))
			:addTo(self.spListenBg)
		if spListenCount then
			spListenCount:move(70, 12):addTo(spPrompt)
		end

		-- cc.Label:createWithTTF("厉害了word哥！你可以胡的牌太多，摆不下了....", "fonts/round_body.ttf", 50)
		-- 	:move(260, 40)
		-- 	:setAnchorPoint(cc.p(0, 0.5))
		-- 	:setColor(cc.c3b(0, 0, 0))
		-- 	:addTo(self.spListenBg, 1)
	end
	--牌、番、数
	self.cbAppearCardIndex = GameLogic.DataToCardIndex(self._scene.cbAppearCardData)
	for i = 1, cbCardCount do
		if bTooMany then
			break
		end
		local tempX = fSpacing*(i - 1)
		--local rectX = self._cardLayer:switchToCardRectX(cbCardData[i])
		local cbCardIndex = GameLogic.SwitchToCardIndex(cbCardData[i])
		local nLeaveCardNum = leftCount[i]
		--牌底
		local card = display.newSprite("game/font_small/card_down.png")
			--:setTextureRect(cc.rect(width*rectX, 0, width, height))
			:move(183 + tempX, 46)
			:addTo(self.spListenBg)
		--字体
		local nValue = math.mod(cbCardData[i], 16)
		local nColor = math.floor(cbCardData[i]/16)
		local strFile = "game/font_small/font_"..nColor.."_"..nValue..".png"
		local cardFont = display.newSprite(strFile)
			:move(width/2, height/2 + 8)
			:addTo(card)
		cc.Label:createWithTTF("1", "fonts/round_body.ttf", 16)		--番数
			:move(220 + tempX, 61)
			:setColor(cc.c3b(254, 246, 165))
			:addTo(self.spListenBg)
		display.newSprite("#sp_listenTimes.png")
			:move(244 + tempX, 61)
			:addTo(self.spListenBg)
		cc.Label:createWithTTF(nLeaveCardNum.."", "fonts/round_body.ttf", 16) 		--剩几张
			:move(220 + tempX, 31)
			:setColor(cc.c3b(254, 246, 165))
			:setTag(cbCardIndex)
			:addTo(self.spListenBg)
		display.newSprite("#sp_listenNum.png")
			:move(244 + tempX, 31)
			:addTo(self.spListenBg)
	end
end

--减少可听牌数
function GameViewLayer:reduceListenCardNum(cbCardData)
	local cbCardIndex = GameLogic.SwitchToCardIndex(cbCardData)
	if #self.cbAppearCardIndex == 0 then
		self.cbAppearCardIndex = GameLogic.DataToCardIndex(self._scene.cbAppearCardData)
	end
	self.cbAppearCardIndex[cbCardIndex] = self.cbAppearCardIndex[cbCardIndex] + 1
	local labelLeaveNum = self.spListenBg:getChildByTag(cbCardIndex)
	if labelLeaveNum then
		local nLeaveCardNum = 4 - self.cbAppearCardIndex[cbCardIndex]
		labelLeaveNum:setString(nLeaveCardNum.."")
	end
end

function GameViewLayer:setBanker(viewId)
	if viewId < 1 or viewId > cmd.GAME_PLAYER then
		print("chair id is error!")
		return false
	end
	local spBanker = self.nodePlayer[viewId]:getChildByTag(GameViewLayer.SP_BANKER)
	spBanker:setVisible(true)

	return true
end

function GameViewLayer:setUserTrustee(viewId, bTrustee)
	if viewId <=4 and viewId>=1 then
		--self.nodePlayer[viewId]:getChildByTag(GameViewLayer.SP_TRUSTEE):setVisible(bTrustee)
		if viewId == 1 then
			self.nodePlayer[viewId]:getChildByTag(GameViewLayer.SP_TRUSTEE):setPositionY(30)
		end
		if viewId == cmd.MY_VIEWID then
			self.spTrusteeCover:setVisible(bTrustee)
		end

		-- local node_face = self.nodePlayer[viewId]:getChildByTag(GameViewLayer.SP_HEAD)
		-- if node_face then
		-- 	if bTrustee then
		-- 		convertToGraySprite(node_face.m_head.m_spRender)
		-- 	else
		-- 		convertToNormalSprite(node_face.m_head.m_spRender)
		-- 	end
		-- end
	end
end

--设置房间信息
function GameViewLayer:setRoomInfo(tableId, chairId)
end

function GameViewLayer:onTrusteeTouchCallback(event, x, y)
	if not self.spTrusteeCover:isVisible() then
		return false
	end

	local rect = self.spTrusteeCover:getChildByTag(GameViewLayer.SP_TRUSTEEBG):getBoundingBox()
	if cc.rectContainsPoint(rect, cc.p(x, y)) then
		return true
	else
		return false
	end
end

--设置剩余牌
function GameViewLayer:setRemainCardNum(num)
	local labelNum = cc.find(self, "sp_top_left/sp_left/AtlasLabel")
	labelNum:setString(num)
end

--牌托
function GameViewLayer:showCardPlate(viewId, cbCardData)
	if nil == viewId then
		self.spCardPlate:setVisible(false)
		return
	end
	self.spCardPlate:stopAllActions()
	--local rectX = self._cardLayer:switchToCardRectX(cbCardData)
	local nValue = math.mod(cbCardData, 16)
	local nColor = math.floor(cbCardData/16)
	local strFile = "game/font_middle/font_"..nColor.."_"..nValue..".png"
	self.spCardPlate:getChildByTag(GameViewLayer.SP_PLATECARD):setTexture(strFile)
	self.spCardPlate:move(posPlate[viewId]):setVisible(true)
end
--操作效果
function GameViewLayer:showOperateFlag(viewId, operateCode)
	local spFlag = self:getChildByTag(GameViewLayer.SP_OPERATFLAG)
	if spFlag then
		spFlag:removeFromParent()
	end
	if nil == viewId then
		return false
	end
	local strFile = "#"
	if operateCode == GameLogic.WIK_NULL then
		return false
	elseif operateCode == GameLogic.WIK_CHI_HU then
		strFile = "#sp_flag_win.png"
	elseif operateCode == GameLogic.WIK_LISTEN then
		strFile = "#sp_flag_listen.png"
	elseif operateCode == GameLogic.WIK_GANG then
		strFile = "#sp_flag_bridge.png"
	elseif operateCode == GameLogic.WIK_PENG then
		strFile = "#sp_flag_bump.png"
	elseif operateCode <= GameLogic.WIK_RIGHT then
		strFile = "#sp_flag_eat.png"
	end
	display.newSprite(strFile)
		:setTag(GameViewLayer.SP_OPERATFLAG)
		:move(posPlate[viewId])
		:addTo(self, 2)

	return true
end

--数字中插入点
function GameViewLayer:numInsertPoint(lScore)
	local strRes = ""
	if lScore<0 then
		strRes = ""..lScore
	elseif  lScore>=0 and lScore<10000 then
		strRes = ""..lScore
	elseif lScore>=10000 and lScore<100000000 then

		local num1 = lScore%10000   > 0   and lScore%10000  or 0
		local num2 = num1>0 and math.floor(num1/100) or 0
		if num2>0 then
		       local str1= num2<10 and "0"..num2 or ""..num2
		       strRes = math.floor(lScore/10000).."."..str1.."万"
		else
		       strRes = math.floor(lScore/10000).."万"
		end

	elseif lScore>=100000000 then
		local num1 = lScore%100000000   > 0   and lScore%100000000  or 0
		local num2 = num1>0 and math.floor(num1/1000000) or 0
		if num2>0 then
		       local str1= num2<10 and "0"..num2 or ""..num2
		       strRes = math.floor(lScore/100000000).."."..str1.."亿"
		else
		       strRes = math.floor(lScore/100000000).."亿"
		end
	end
		--todo
	--[[
	assert(lScore >= 0)
	local strRes = ""
	local str = string.format("%d", lScore)
	local len = string.len(str)

	local times = math.floor(len/3)
	local remain = math.mod(len, 3)
	strRes = strRes..string.sub(str, 1, remain)
	for i = 1, times do
		if strRes ~= "" then
			strRes = strRes.."/"
		end
		local index = (i - 1)*3 + remain + 1	--截取起始位置
		strRes = strRes..string.sub(str, index, index + 2)
	end
]]
	return strRes
end


function GameViewLayer:setRoomHost(viewId)
	for i = 1, cmd.GAME_PLAYER do
		if not self.nodePlayer[i] then
			print()
		end
		self.nodePlayer[i]:getChildByTag(GameViewLayer.SP_ROOMHOST):setVisible(false)
	end
	if not GlobalUserItem.bPrivateGoldRoom then
		--self.nodePlayer[viewId]:getChildByTag(GameViewLayer.SP_ROOMHOST):setVisible(true)
	end
end


-----------------------------------------------回放-------------------------------------
function GameViewLayer:updateCardsNode(handCard, outCard, activityCard, leftCard)
	--先清除数据
	self._cardLayer:onResetData()

	self.cbOutCardTemp = 0
	local spFlag = self:getChildByTag(GameViewLayer.SP_OPERATFLAG)
	if spFlag then
		spFlag:removeFromParent()
	end
	self.spCardPlate:setVisible(false)
	self.spCardPlate:stopAllActions()
	self._cardLayer.nRemainCardNum = leftCard
	--剩余牌数
	self:setRemainCardNum(leftCard)

	--先设置碰杠的牌
	for i=1,self._scene.chairCount do
		local wViewChairId = self._scene:SwitchViewChairID(i - 1)
		if #activityCard[wViewChairId] > 0 then
			for j = 1, #activityCard[wViewChairId] do
				local cbOperateData = activityCard[wViewChairId][j]

				self._cardLayer:bumpOrBridgeCard(wViewChairId, cbOperateData.cbCardValue, cbOperateData.cbType)

			end
		end
	end

	--手牌
	for i = 1, self._scene.chairCount do
		local viewId = self._scene:SwitchViewChairID(i - 1)
		if #handCard[viewId] > 0 then
			--self._cardLayer:createHandCard(viewId, handCard[viewId], #handCard[viewId])
			GameLogic.SortCardList(handCard[viewId])
			self._cardLayer:setHandCard(viewId, #handCard[viewId], handCard[viewId])
		end
	end

	--设置已经出的牌
	for i = 1, self._scene.chairCount do
		local viewId = self._scene:SwitchViewChairID(i - 1)
		if #outCard[viewId] > 0 then
			for j=1,#outCard[viewId] do
				self._cardLayer:discard(viewId, outCard[viewId][j])
			end

		end
	end
end


----------------------------------------------------------新大厅--------------------------------------------------
--初始化新大厅房间信息
function GameViewLayer:initRoomInfo()
    self.RoomInfoLayer = cc.CSLoader:createNode(cmd.RES_PATH.."game/RoomInfoLayerVer2.csb"):addTo(self, 10)
    --self.RoomInfoLayer:setVisible(false)

    local PanelLayer = self.RoomInfoLayer:getChildByName("Panel_2")
    -- 遮罩
    PanelLayer:addTouchEventListener(function(ref)
        self.RoomInfoLayer:removeFromParent()
    end)

    local boxCallback = function(sender,eventType)

        		if self._scene.m_bLookOn then
        			if GameViewLayer.BT_RULE == sender:getTag() then 
        				sender:setSelected(true)
        			 	
        			else
        				sender:setSelected(false)
		          		showToast(self._scene, "观战中不可使用该功能", 1)
        			end 

		          	return
	      	end
        self.m_tabBoxGame[GameViewLayer.BT_RULE]:setSelected(GameViewLayer.BT_RULE == sender:getTag())
        self.m_tabBoxGame[GameViewLayer.BT_SCORELIST]:setSelected(GameViewLayer.BT_SCORELIST == sender:getTag())

        self.RoomInfoLayer:getChildByName("Node_wanfa"):setVisible(GameViewLayer.BT_RULE == sender:getTag())
        self.RoomInfoLayer:getChildByName("Node_liushui"):setVisible(GameViewLayer.BT_SCORELIST == sender:getTag())
    end
    --按钮控制
    self.m_tabBoxGame = {}
    local checkbx = self.RoomInfoLayer:getChildByName("CheckBox_wanfa")
    checkbx:setTag(GameViewLayer.BT_RULE)
    checkbx:addEventListener(boxCallback)
    checkbx:setSelected(true)
    self.m_tabBoxGame[GameViewLayer.BT_RULE] = checkbx

    checkbx = self.RoomInfoLayer:getChildByName("CheckBox_liushui")
    checkbx:setTag(GameViewLayer.BT_SCORELIST)
    checkbx:addEventListener(boxCallback)
    self.m_tabBoxGame[GameViewLayer.BT_SCORELIST] = checkbx

    local titleBg = self.RoomInfoLayer:getChildByName("Node_liushui"):getChildByName("Sprite_12")
    local playerNum = self._scene:onGetSitUserNum()


    for i=1, playerNum do
    	local viewId = self._scene:SwitchViewChairID(i-1)
    	if self.m_sparrowUserItem[viewId] then
    		if GlobalUserItem.bPrivateGoldRoom   then
    			if viewId ==  cmd.MY_VIEWID then
	    			local name = ClipText:createClipText(cc.size(100, 27), self.m_sparrowUserItem[self._scene:SwitchViewChairID(i-1)].szNickName,"fonts/round_body.ttf",26):addTo(titleBg)
		    		name:setAnchorPoint(cc.p(0.5, 0.5))
			   		name:setPosition(501, 23.5)
			   		break
		   		end
    		else
	    		local name = ClipText:createClipText(cc.size(100, 27), self.m_sparrowUserItem[self._scene:SwitchViewChairID(i-1)].szNickName,"fonts/round_body.ttf",26):addTo(titleBg)
	    		name:setAnchorPoint(cc.p(0.5, 0.5))
		   		name:setPosition(210+(i-1)*134, 23.5)
	   		end
	   		--name:setTextColor(cc.c3b(15, 51, 87))
	   	end
    end

    if GlobalUserItem.bPrivateGoldRoom then
    	local Node_liushui = self.RoomInfoLayer:getChildByName("Node_liushui")
    	local topBg = Node_liushui:getChildByName("Sprite_12")
    	for i = 0, 2 do
    		topBg:getChildByName("Sprite_14_"..i):setVisible(false)
    	end
    	topBg:getChildByName("Sprite_14"):setPositionX(339)
    	Node_liushui:getChildByName("Text_8"):setPositionX(177)
    end


    -- local cbMaCount = self._scene._scene:getMaCount()
    local strMa = ""
    if self._scene.cbMaCount == 1 then
        strMa = "一码全中"
    else
        strMa = string.format("%s个扎马", self._scene.cbMaCount)
    end
    local strPlayerCount = self._scene.cbPlayerCount
    cc.find(self.RoomInfoLayer, "Node_wanfa/sp_0/Text_1"):setString(strMa)
    cc.find(self.RoomInfoLayer, "Node_wanfa/sp_1/Text_1"):setString(strPlayerCount)

    local nodePassword = cc.find(self.RoomInfoLayer, "Node_wanfa/sp_2/Text_1")
    local nodePayType = cc.find(self.RoomInfoLayer, "Node_wanfa/sp_3/Text_1")
	if not GlobalUserItem.bPrivateRoom then
		nodePassword:setVisible(false)
		nodePayType:setVisible(false)
	else
		--0积分,1金币
		if self._scene.cbPlayMode == 1 then
			nodePassword:getParent():setVisible(false)
			nodePayType:getParent():setVisible(false)
		else
			local needPassword = PriRoom:getInstance().m_tabPriData.bRoomPwd
			local roomAAPay = PriRoom:getInstance().m_tabPriData.bAAPay
		    local strPasswordset = needPassword and "需要密码" or "不要密码"
		    local strPayType = roomAAPay and "AA支付" or "房主支付"
			nodePassword:setString(strPasswordset)
			nodePayType:setString(strPayType)
		end
	end

    -- 房间信息界面
    --解散按钮
    local btnDismiss = self.RoomInfoLayer:getChildByName("Node_wanfa"):getChildByName("Button_2")
    if GlobalUserItem.bPrivateGoldRoom  then
    	btnDismiss:setVisible(false)
    end
    if nil ~= btnDismiss then
        btnDismiss:addTouchEventListener(function(ref, tType)

        		if self._scene.m_bLookOn then
		          	showToast(self, "观战中不可使用该功能", 1)
		          	return
	      	end

            if tType == ccui.TouchEventType.ended then
                PriRoom:getInstance():queryDismissRoom(self._scene.m_cbGameStatus, self._scene:onGetSitUserNum())
                self.RoomInfoLayer:removeFromParent()
            end
        end)
    end


local btnClose = self.RoomInfoLayer:getChildByName("Button_1")
	btnClose:addTouchEventListener(function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                self.RoomInfoLayer:removeFromParent()
            end
        end)



    --游戏流水界面
    -- 列表
    local tmpcontent = self.RoomInfoLayer:getChildByName("Node_liushui"):getChildByName("Panel_1")
    tmpcontent:setVisible(true)
    local contentsize = tmpcontent:getContentSize()
    self.m_tabList = {}
    self._listView = cc.TableView:create(cc.size(670,325))
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
    self._listView:setPosition(cc.p(0,0))
    self._listView:setAnchorPoint(cc.p(0,0))
    self._listView:setDelegate()
    self._listView:addTo(tmpcontent)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()
    --tmpcontent:removeFromParent()

    --self:updataUserInfo()
end

function GameViewLayer:showRoomInfo(bIsShow)
	self:initRoomInfo()
end



-- 子视图大小
function GameViewLayer:cellSizeForTable(view, idx)
    return 670, 47
end

function GameViewLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    print("ppppppppppppppp"..idx)



    local cellbg = cc.Sprite:create("game/cell"..(idx%2==0 and 2 or 1)..".png"):setAnchorPoint(cc.p(0,0)):move(0,0):addTo(cell)

    local allNum =  GlobalUserItem.bPrivateGoldRoom  and  "" or "/"..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    local jushu = ClipText:createClipText(cc.size(100, 25), (idx+1)..allNum,"fonts/round_body.ttf",23):addTo(cellbg)
    		jushu:setAnchorPoint(cc.p(0.5, 0.5))
   		jushu:setPosition(75, 23.5)
   		jushu:setTextColor(cc.c3b(15, 51, 87))
    		--clipNick:setTag(776)
    dump(self._scene.m_userRecord,"uuuuuuuuuuuuuuuuuuu1")
    for i=1,self._scene.chairCount do
    	local score = self._scene.m_userRecord[i].lDetailScore[idx+1] >=0 and  "+"..self._scene.m_userRecord[i].lDetailScore[idx+1] or ""..self._scene.m_userRecord[i].lDetailScore[idx+1]
    	local defen = ClipText:createClipText(cc.size(100, 25), score,"fonts/round_body.ttf",23):addTo(cellbg)
    		defen:setAnchorPoint(cc.p(0.5, 0.5))
   		defen:setPosition(213.5+(i-1)*134, 23.5)
   		defen:setTextColor(cc.c3b(15, 51, 87))
   		local viewId = self._scene:SwitchViewChairID(i-1)

   		if GlobalUserItem.bPrivateGoldRoom   then
   			if viewId ==  cmd.MY_VIEWID then
   				defen:setPosition(501, 23.5)
   				jushu:setPosition(167, 23.5)

   				if self._scene.m_userRecord[i].lDetailScore[idx+1] >=0 then
	   				defen:setTextColor(cc.c3b(255, 0, 0))
	   			else
	   				defen:setTextColor(cc.c3b(55, 176, 13))
	   			end
	   			break
   			else
				defen:setVisible(false)
   			end
   		else

	   		if  viewId == cmd.MY_VIEWID then
	   			if self._scene.m_userRecord[i].lDetailScore[idx+1] >=0 then
	   				defen:setTextColor(cc.c3b(255, 0, 0))
	   			else
	   				defen:setTextColor(cc.c3b(55, 176, 13))
	   			end
	   		end
   		end

    end

    return cell
end

-- 子视图数目
function GameViewLayer:numberOfCellsInTableView(view)
	if self._scene.m_bLookOn then 
		return 0
	end
	if self._scene.m_userRecord and self._scene.m_userRecord[1].lDetailScore then

		return #self._scene.m_userRecord[1].lDetailScore
	else
		print("hhhhhhhhhhhhhhhhhh333")
		return 0
	end

end

function GameViewLayer:onRefreshInfo()
	local lab_room_id = cc.find(self, "Image_top/Text_1")
	local lab_round_count = cc.find(self, "Image_top/Text_1_0")

	if GlobalUserItem.bPrivateRoom then
		--约牌场：房号和局数
		local roomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
	    lab_room_id:setString(string.format("房号:%06d", roomID))
	    local dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount
	    local dwDrawCountLimit = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
	    local strcount = string.format("局数:%s / %s", dwPlayCount, dwDrawCountLimit)
	    lab_round_count:setString(strcount)

        if GlobalUserItem.bPrivateGoldRoom then
            local strcount = string.format("局数:%s", dwPlayCount)
            lab_round_count:setString(strcount)
        end
	else
		-- 金币场:中级场, 底分:1
		local lvstr = "高级场"
		print("场子级别==========", lvstr)
		lab_room_id:setString(string.format("金币场: %s", lvstr))
		lab_round_count:setString(string.format("底分: %s", self._scene.lCellScore))
	end
end

local singalColors = {cc.c3b(0xff,0,0), cc.c3b(0xc7,0x95,0x19), cc.c3b(0xff,0xff,0xff)}
function GameViewLayer:onScheduleSingal()
    -- local integer,fract = math.modf(MultiPlatform:getInstance():getBatteryLevel()/10)
    local rawPowerValue = MultiPlatform:getInstance():getBatteryLevel()
    powerPercent = math.ceil(rawPowerValue)
    powerPercent = math.max(25, powerPercent)

    local ins = MultiPlatform:getInstance()

    local netType,isWifi,netStatus
    if device.platform == "windows" then
        netType,isWifi,netStatus = 1,true,3
        rawPowerValue,powerPercent = 25,25
    else
        netType = MultiPlatform:getInstance():getNetworkType()
        isWifi = netType==2 --(device.platform == "android" and netType==1) or (device.platform == "ios" and netType==2)
        netStatus = MultiPlatform:getInstance():getNetworkStrength()
    end

    local panel_battery = cc.find(self, "sp_top_left/sp_battery_edge/Panel_1")
    local node_wifi = cc.find(self, "sp_top_left/Node_wifi")
    local node_4G = cc.find(self, "sp_top_left/Node_4g")
    node_wifi:setVisible(isWifi)
    node_4G:setVisible(not isWifi)

    local node = isWifi and node_wifi or node_4G
    for i=1,netStatus do
        local sp = node:getChildByName("Sprite_"..i)
        sp:setVisible(true)
        sp:setColor(singalColors[netStatus])
    end
    for i=netStatus+1,3 do
        node:getChildByName("Sprite_"..i):setVisible(false)
    end
    panel_battery:setContentSize(cc.size(50*powerPercent*0.01, 200))
    if rawPowerValue<= 25 then
        panel_battery:getChildByName("sp_battery_full"):setColor(cc.c3b(0xff,0,0))
    else
        panel_battery:getChildByName("sp_battery_full"):setColor(cc.c3b(0xff,0xff,0xff))
    end
end

return GameViewLayer