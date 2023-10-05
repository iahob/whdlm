local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)

require("client/src/plaza/models/yl")
local cmd = appdf.req(appdf.GAME_SRC.."yule.runfast.src.models.CMD_Game")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local GameChatLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ResultLayer = appdf.req(appdf.GAME_SRC.."yule.runfast.src.views.layer.ResultLayer")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.runfast.src.models.GameLogic")
local CardLayer = appdf.req(appdf.GAME_SRC.."yule.runfast.src.views.layer.CardLayer")
local SetLayer = appdf.req(appdf.GAME_SRC.."yule.runfast.src.views.layer.SetLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local SpineObj = appdf.req("client.src.external.SpineObj")

local ShakeLayer = appdf.req(appdf.GAME_SRC.."yule.runfast.src.views.layer.ShakeLayer")


GameViewLayer.RES_PATH 				= "game/yule/runfast/res/"
GameViewLayer.BT_EXIT 				= 1
GameViewLayer.BT_START 				= 3
GameViewLayer.BT_CHAT 				= 4
GameViewLayer.BT_TRUSTEE 			= 5
GameViewLayer.BT_OUTNONE 			= 6
GameViewLayer.BT_REELECT 			= 7
GameViewLayer.BT_PROMPT 			= 8
GameViewLayer.BT_OUTCARD 			= 9
GameViewLayer.BT_MENU 			= 10
GameViewLayer.BT_HELP 			= 30
GameViewLayer.BT_ADDRESS 			= 31



GameViewLayer.BT_SET 				= 11
GameViewLayer.BT_CHANGEDESK 		= 12

GameViewLayer.SP_HEAD 				= 15

GameViewLayer.BT_CARDRECORD 		= 40
GameViewLayer.BT_SIT 		= 41
GameViewLayer.BT_ROOMRULE 		= 42

local anchorPointCard = {cc.p(0, 0.5), cc.p(0.5, 0.5), cc.p(1, 0.5)}
local anchorPointBubble = {cc.p(0, 0), cc.p(0, 0), cc.p(1, 0)}
local anchorPointHead = {cc.p(0, 1), cc.p(0, 0), cc.p(1, 1),cc.p(1, 1)}
local posOutCard = {cc.p(350, 570), cc.p(667, 266), cc.p(970, 466)}
local posBubble = {cc.p(96, 598), cc.p(235, 344), cc.p(1230, 520)}
local posHead = {cc.p(70, 200), cc.p(60, 100), cc.p(750, 200)}
local posFirstPlayer = {cc.p(172, 564), cc.p(310, 278), cc.p(1151, 443)}
local posWarn = {cc.p(256, 565), cc.p(358, 371), cc.p(1066, 485)}
local posEndedCard = {cc.p(190, 570), cc.p(667, 266), cc.p(1125, 466)}

--震屏动画
function GameViewLayer:shakeScreen()
    local shake = ShakeLayer:create(self, 0.25)
    shake:shakeRun()
end


function GameViewLayer:onInitData()
    self.nodeOutCard = {}
	self.cardData = {0x13, 0x23, 0x04, 0x24, 0x1C, 0x3D}
	self.nRemainCard = {16, 16, 16}
	self.chatDetails = {}
	self.m_pUserItems = {}
    self.cbGender = {}
    self.bombNum = {0, 0, 0}
    self.spriteWarnAnimate = {}
    --回放
    self.bIsVideo = false 
end

function GameViewLayer:onResetData()
	
	self:setGameBtnStatus(false, false, false)

	self._cardLayer:onResetData()

	-- for i = 1, 3 do
	-- 	local remainCard = self.nodePlayer[i]:getChildByName("Text_remainCard")
	-- 	remainCard:setString("16")
	-- 	remainCard:setVisible(false)
	-- 	self:setBombNum(i, 0)
	-- end
	self:showBaoDan(false,1)
	self:showBaoDan(false,3)

	self.nodeLastCard[1]:setVisible(false)
	self.nodeLastCard[3]:setVisible(false)
	
end



function GameViewLayer:preloadUI()



  
    --路劲
    cc.FileUtils:getInstance():addSearchPath(device.writablePath..GameViewLayer.RES_PATH)
	--加载资源
	local rootLayer
	rootLayer, self._csbNode = ExternalFun.loadRootCSB("rungame_res/GameScene.csb", self)
	--加载动画
	local animation = cc.Animation:create()
	animation:setDelayPerUnit(0.04)
	animation:setLoops(-1)
	for i = 1, 4 do
		local str = string.format("Animate_warn_%d.png", i)
		local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
		if spriteFrame then
			animation:addSpriteFrame(spriteFrame)
		end
	end
	cc.AnimationCache:getInstance():addAnimation(animation, "WARN")

	
end

function GameViewLayer:onExit()
	print("GameViewLayer onExit")

	cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timeSchedule)

	AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
	
    	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    	cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

  
end

local this
function GameViewLayer:ctor(scene)
	this = self
	self._scene = scene
	self.isShowTip = false
	self.isShowRuleTip = false
	self:onInitData()
	self:preloadUI()
	self._resultLayer = ResultLayer:create(self):addTo(self, 11)
	self._cardLayer = CardLayer:create(self):addTo(self._csbNode:getChildByName("Node_outCard"))
	--self._setLayer = SetLayer:create(self):addTo(self, 3)

	--节点事件
	local function onNodeEvent(event)
		if event == "exit" then
			self:onExit()
		end
	end
	self:registerScriptHandler(onNodeEvent)

	--按钮回调
	local btnCallback = function(ref, eventType)
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(ref:getTag(), ref)
		end
	end

	local btn = self._csbNode:getChildByName("Button_caedRecord")
	btn:setTag(GameViewLayer.BT_CARDRECORD)
	btn:addTouchEventListener(btnCallback)

	self.node_cardRecord = self._csbNode:getChildByName("Node_cardRecord")
	self.node_cardRecord:setVisible(false)
	self:showCardRecord()

	self.btnStart = self._csbNode:getChildByName("Button_start")
	self.btnStart:setTag(GameViewLayer.BT_START)
	self.btnStart:setVisible(false)
	self.btnStart:addTouchEventListener(btnCallback)

	self.btnSit = self._csbNode:getChildByName("Button_sit")
	self.btnSit:setTag(GameViewLayer.BT_SIT)
	self.btnSit:setVisible(false)
	self.btnSit:addTouchEventListener(btnCallback)


	local btnChat = self._csbNode:getChildByName("Button_chat")
	btnChat:setTag(GameViewLayer.BT_CHAT)
	btnChat:addTouchEventListener(btnCallback)

	local menuBg = self._csbNode:getChildByName("Button_menu"):getChildByName("Sprite_17")


	self.TouchPanel = self._csbNode:getChildByName("Panel_2")
	self.TouchPanel:addTouchEventListener(function (ref, eventType)
		if eventType == ccui.TouchEventType.ended then
			menuBg:setVisible(false)
		end
	end)--GameViewLayer.BT_MENU


	self.btnTrustee = self._csbNode:getChildByName("Button_trustee")
	self.btnTrustee:setTag(GameViewLayer.BT_TRUSTEE)
	self.btnTrustee:addTouchEventListener(btnCallback)

	local menuBtn = self._csbNode:getChildByName("Button_menu")
	menuBtn:setTag(GameViewLayer.BT_MENU)
	menuBtn:addTouchEventListener(btnCallback)

	self.btnChange = menuBg:getChildByName("Button_change")
	self.btnChange:setTag(GameViewLayer.BT_CHANGEDESK)
	self.btnChange:addTouchEventListener(btnCallback)

	local menuClose = menuBg:getChildByName("Button_close")
	menuClose:setTag(GameViewLayer.BT_MENU)
	menuClose:addTouchEventListener(btnCallback)
	
	local btnSet = menuBg:getChildByName("Button_set")
	btnSet:setTag(GameViewLayer.BT_SET)
	btnSet:addTouchEventListener(btnCallback)

	local btnExit = menuBg:getChildByName("Button_exit")
	btnExit:setTag(GameViewLayer.BT_EXIT)
	btnExit:addTouchEventListener(btnCallback)

	local btnHelp = menuBg:getChildByName("Button_help")
	btnHelp:setTag(GameViewLayer.BT_HELP)
	btnHelp:addTouchEventListener(btnCallback)

	local btnAddress = self._csbNode:getChildByName("Button_address")
	btnAddress:setTag(GameViewLayer.BT_ADDRESS)
	btnAddress:addTouchEventListener(btnCallback)


	local controlBg = self._csbNode:getChildByName("Node_control")

	self.btnOutNone = controlBg:getChildByName("Button_outNone")
	self.btnOutNone:setTag(GameViewLayer.BT_OUTNONE)
	--self.btnOutNone:setVisible(false)
	self.btnOutNone:addTouchEventListener(btnCallback)

	-- self.btnReelect = controlBg:getChildByName("Button_reelect")
	-- self.btnReelect:setTag(GameViewLayer.BT_REELECT)
	-- self.btnReelect:setVisible(false)
	-- self.btnReelect:addTouchEventListener(btnCallback)

	self.btnPrompt = controlBg:getChildByName("Button_tips")
	self.btnPrompt:setTag(GameViewLayer.BT_PROMPT)
	--self.btnPrompt:setVisible(false)
	self.btnPrompt:addTouchEventListener(btnCallback)

	self.btnOutCard = controlBg:getChildByName("Button_outCard")
	self.btnOutCard:setTag(GameViewLayer.BT_OUTCARD)
	--self.btnOutCard:setVisible(false)
	self.btnOutCard:addTouchEventListener(btnCallback)




	self.spriteClock = self._csbNode:getChildByName("Sprite_clock")
	self.clock_time = self.spriteClock:getChildByName("AtlasLabel_time")
	self.spriteClock:setVisible(false)
	

	--玩家节点
	self.nodePlayer = {}
	for i = 1, 3 do
		self.nodePlayer[i] = self._csbNode:getChildByName("Node_player"..i)
		if GlobalUserItem.bVideo then
			self.nodePlayer[i]:setVisible(false)
		end
		self.nodePlayer[i]:setVisible(false)
	end

	--玩家剩余牌
	self.nodeLastCard = {}
	self.nodeLastCard[1] = self._csbNode:getChildByName("Node_lastCard1")
	self.nodeLastCard[3] = self._csbNode:getChildByName("Node_lastCard3")
	self.nodeLastCard[1]:setVisible(false)
	self.nodeLastCard[3]:setVisible(false)

	local autoLayer = self._csbNode:getChildByName("autoLayer")
	autoLayer:setVisible(false)

	local cbtlistener = function (sender,eventType)
	        if eventType == ccui.TouchEventType.ended then
	            --autoLayer:setVisible(false)
	            --self._scene.bMyAutoStatus = false
	            self._scene:sendAutomatism(false)
	        end
	end

	

	autoLayer:addTouchEventListener(cbtlistener)

	--聊天框
	local tabconfig={}
 	tabconfig.csbfile = "chat/GameChatLayer.csb"
	    self._chatLayer = GameChatLayer:create(self._scene._gameFrame , tabconfig)
	    self._chatLayer:addTo(self, 3)
	--聊天泡泡
	self.chatBubble = {}
	for i = 1 , cmd.GAME_PLAYER + 1 do 
		self.chatBubble[i] = self._csbNode:getChildByName("chat"..i)
	end

	local btnVoice = self._csbNode:getChildByName("Button_voice")
	--btnVoice:setName(VOICE_BTN_NAME)
	btnVoice:addTouchEventListener(function(ref, eventType)
 	        if eventType == ccui.TouchEventType.began then
		  self._scene._scene:startVoiceRecord()
	        elseif eventType == ccui.TouchEventType.ended or eventType == ccui.TouchEventType.canceled then
	            self._scene._scene:stopVoiceRecord()
	        end
	end)

          AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)	

          if GlobalUserItem.bVoiceAble == true then  
        		AudioEngine.playMusic("sound/BACK_MUSIC.wav", true)
    	end

    local roomRule = self._csbNode:getChildByName("Button_roomInfo")
    roomRule:setTag(GameViewLayer.BT_ROOMRULE)
    roomRule:addTouchEventListener(btnCallback)


-- 	-- 电量
	self.batteryBar= self._csbNode:getChildByName("Loading_battery")
    self.batteryBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())

	self._csbNode:getChildByName("Text_time"):setString(os.date("%H:%M"))

    self.timeSchedule = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
		self.batteryBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())
 
		self._csbNode:getChildByName("Text_time"):setString(os.date("%H:%M"))

		local netType = MultiPlatform:getInstance():getNetworkType()         
		local Strength = MultiPlatform:getInstance():getNetworkStrength()
		local TypePic = {"img_4G","img_wifi"}
		if netType and Strength and netType>=1 and netType<=2 and Strength>=1 and Strength<=3 then
			local sp = cc.Sprite:create("rungame_res/"..TypePic[netType]..Strength..".png")
			self._csbNode:getChildByName("Sprite_net"):setSpriteFrame(sp:getSpriteFrame())
		end
	end, 1, false)
end

function GameViewLayer:getUserCount()
	return table.nums(self.m_pUserItems)
end

function GameViewLayer:OnUpdateUserExit(userItem)
	
	
	if self._scene.isPriEnd  and  GlobalUserItem.bPrivateRoom  then
		return
	end

	for k,v in pairs(self.m_pUserItems) do
		--dump(v.m_userItem,"=========================")
	          if v and v.dwUserID == userItem.dwUserID then
	              
			self.nodePlayer[k]:setVisible(false)
			self._csbNode:getChildByName("ready"..k):setVisible(false)
			v= nil
		 
	          end
	end


end

--更新用户显示
function GameViewLayer:OnUpdateUser(viewId, userItem)
	if not viewId or viewId == yl.INVALID_CHAIR then
		print("OnUpdateUser viewId is nil")
		return
	end

	if not userItem  then
		return 
	end

	 if GlobalUserItem.bVideo then
	 	if viewId ~= cmd.MY_VIEWID and (self._scene.wPlayerCount == 0 or not self._scene.wPlayerCount) then
	 		return
	 	end
	 end

	--dump(userItem,"===")
	self.m_pUserItems[viewId] = clone(userItem)
	--准备
	local spriteReady = self._csbNode:getChildByName("ready"..viewId)
	spriteReady:setVisible(yl.US_READY == userItem.cbUserStatus)

	if viewId == cmd.MY_VIEWID and yl.US_READY == userItem.cbUserStatus then
		self.btnStart:setVisible(false)
		--self.spriteClock:setVisible(false)
		self._resultLayer:setVisible(false)
		self:onResetData()
		self._scene:KillGameClock()
		print("更新用户显示 我准备")
	end

	self.nodePlayer[viewId]:setVisible(true)

	self.cbGender[viewId] = userItem.cbGender
	--头像
	local headNode = self.nodePlayer[viewId]:getChildByName("Panel_head")
	headNode:removeChildByName("_head_")
	local head = PopupInfoHead:createClipHead(userItem, 90,"rungame_res/headBg.png")
		head:setPosition(45, 45)			--初始位置
		head:setName("_head_")
		head:enableInfoPop(true, posHead[viewId], anchorPointHead[viewId])	--点击弹出的位置0
		headNode:addChild(head)
	    local param = {}
	    param.isProp = true     --显示道具互动，百人游戏不显示
	    param.imgPath = "popInfo/popBg.png" --弹出框背景
	    head:setPropInfo(param)
        head:setPopCsbFile("popInfo/GamePopInfo.csb") -- 自定义CSB
		

	self.nodePlayer[viewId]:getChildByName("Text_name"):setString(userItem.szNickName)



	    if userItem.cbUserStatus == yl.US_OFFLINE and self._scene.cbPlayMode == 0 then
       	           if nil ~= convertToGraySprite then
		            -- 灰度图
		            if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
		                convertToGraySprite(head.m_head.m_spRender)
		            end
	           end        
	           spriteReady:setVisible(false)
	    else
	           -- 普通图
	           if nil ~= convertToNormalSprite then
		           if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
		                convertToNormalSprite(head.m_head.m_spRender)
		           end
	           end
	    end

	self:updateUserScore(viewId, userItem.lScore)

	
	self.btnSit:setVisible(self._scene.m_bLookOn and self._scene:getSitUserCount()<self._scene.wPlayerCount )
end


function GameViewLayer:updateUserScore(viewid,score)
	print("11:",score)
	local scoeTxt = self.nodePlayer[viewid]:getChildByName("Text_score")
	if nil ~= scoeTxt then
	        scoeTxt:setString(self:getScoreString(score))
	        print("22:",self:getScoreString(score))
	        --判断积分房
	        if PriRoom and GlobalUserItem.bPrivateRoom then
	            if PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
					if  PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
						scoeTxt:setString(score - PriRoom:getInstance().m_tabPriData.lIniScore)
						print("33:",score - PriRoom:getInstance().m_tabPriData.lIniScore)
	                end
	            else
	                scoeTxt:setString("0")
	                print("44:",00)
	            end
	        end
	        if GlobalUserItem.bVideo and PriRoom:getInstance().m_tabPriData.lIniScore then
	            scoeTxt:setString(score - PriRoom:getInstance().m_tabPriData.lIniScore)
	        end
	end
end

function GameViewLayer:getScoreString( score )
    if type(score) ~= "number" then
        return ""
    end
    local strScore = ""
    if score < 100000 then
        strScore = strScore..score
    elseif score < 100000000 then
        --print("分数转换1", score/10000)
        strScore = strScore..string.format("%.2f", score/10000).."万"
    else
        --print("分数转换2", score/100000000)
        strScore = strScore..string.format("%.2f", score/100000000).."亿"
    end
    --print("分数转换", score, strScore)
    return strScore
end




--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
    local chatBg = self.chatBubble[viewid] --self.nodePlayer[viewid]:getChildByName("chat")
    if isPlay then

    	    chatBg:setContentSize(200, 70) 
	    chatBg:stopAllActions()
	    chatBg:removeAllChildren()
	    chatBg:setVisible(true)
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
	        spVoice:addTo(chatBg)
	        spVoice:setPosition(chatBg:getContentSize().width * 0.5, chatBg:getContentSize().height * 0.5 +7)
	    
    else
    	chatBg:stopAllActions()
	chatBg:removeAllChildren()
	chatBg:setVisible(false)
    end
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = 1

        --获取头像坐标
        for k,v in pairs(self.m_pUserItems) do
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

function GameViewLayer:showBaoDan(visible,viewid)
	local tip = self._csbNode:getChildByName("Sprite_baoDan"..viewid)
	tip:setVisible(false)
	self._csbNode:removeChildByName("_baodan_"..viewid)

	if visible then
		local spine = SpineObj:create("skeleton/bjd.json", "skeleton/bjd.atlas", true)
    	spine:setPosition(tip:getPosition())
    	spine:setName("_baodan_"..viewid)
    	self._csbNode:addChild(spine)
    	self._scene:PlaySound(GameViewLayer.RES_PATH.."sound/BAO_DAN.mp3")
    end
end

function GameViewLayer:showCardRecord(myCardData, remainCardData)

	if not myCardData then
		myCardData = self._cardLayer.cbCardData
	end

	if not myCardData  or  not remainCardData then
		self:showCardRecord({}, {0,0,0,0,0,0,0,0,0,0,0,0,0})
		
		return;
	end
	
	local value = {2,1,13,12,11,10,9,8,7,6,5,4,3}
	for i=1 , #myCardData do
		local cardValue = GameLogic:GetCardValue(myCardData[i])
		local dataIndex = 0
		for j=1 , #value do
			if cardValue == value[j] then
				dataIndex = j
				break
			end
		end
		if dataIndex~=0 then
			remainCardData[dataIndex] = remainCardData[dataIndex] - 1
		end
	end
	 
	for i=1,13 do
		local cardNode = self.node_cardRecord:getChildByName("card"..value[i])
		cardNode:getChildByName("AtlasLabel_num"):setString(remainCardData and remainCardData[i] or "0")
	end
end

function GameViewLayer:changeTable()
	self._scene._gameFrame:QueryChangeDesk()
	for k,v in pairs(self.m_pUserItems) do
	          if v then
	              self.nodePlayer[k]:setVisible(false)
	              self._csbNode:getChildByName("ready"..k):setVisible(false)
	          end
	end
	self.btnStart:setVisible(true)
end


function GameViewLayer:showHelpLayer()
	local helpLayer = cc.CSLoader:createNode("rungame_res/help/Help2Layer.csb"):addTo(self, 51)
	local bg = helpLayer:getChildByName("Image_bg")

	--按钮回调
	local btnCallback = function(ref, eventType)
		if eventType == ccui.TouchEventType.ended and ref:getTag()==0 then
			helpLayer:removeFromParent()
		elseif eventType == ccui.TouchEventType.ended then
			for i=1 , 4 do
				local btn = bg:getChildByName("Button_"..i)
				btn:setEnabled(i~=ref:getTag())

				local content = bg:getChildByName("Content_"..i)
				content:setVisible(i==ref:getTag())
			end
		end
	end

	

	local panel = helpLayer:getChildByName("Panel_1")
	panel:addTouchEventListener(btnCallback)
	panel:setTag(0)
	local close = bg:getChildByName("Button_close")
	close:addTouchEventListener(btnCallback)
	close:setTag(0)

	for i=1 , 4 do
		local btn = bg:getChildByName("Button_"..i)
		btn:setTag(i)
		btn:addTouchEventListener(btnCallback)
	end


end

function GameViewLayer:updateMenuStatus(flag)
	local menubg = self._csbNode:getChildByName("Button_menu"):getChildByName("Sprite_17")
	if flag ~= nil then
		
		menubg:setVisible(flag)
	else
		menubg:setVisible(not menubg:isVisible())
	end

	self.TouchPanel:setEnabled(menubg:isVisible())
end


function GameViewLayer:switchBtnChangeOrTrust()
	if self._scene.m_cbGameStatus == cmd.GAME_SCENE_FREE then
		if GlobalUserItem.bPrivateRoom then
			self.btnChange:setEnabled(false)
		else
			self.btnChange:setEnabled(true)
		end
		self.btnTrustee:setEnabled(false)
	else
		self.btnTrustee:setEnabled(true)
		self.btnChange:setEnabled(false)
	end
end


function GameViewLayer:onButtonClickedEvent(tag, ref)
	if tag == GameViewLayer.BT_CHAT then
		if self._scene.m_bLookOn then
			showToast(self, "观战中不可使用该功能!", 1)
		else
			self._chatLayer:showGameChat(true)
		end
	elseif tag == GameViewLayer.BT_ROOMRULE then
		print("....1")
		if PriRoom and GlobalUserItem.bPrivateRoom then
        	if nil ~= self._priView and nil ~= self._priView.showRoomRuleRecord then
        		print("....2")
	       		self._priView:showRoomRuleRecord()
        	end
		end
	elseif GameViewLayer.BT_SIT == tag then
		self._scene._gameFrame.bChangeDesk = true
		self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)
	elseif tag == GameViewLayer.BT_CARDRECORD then
		self.node_cardRecord:setVisible(not self.node_cardRecord:isVisible())
	elseif tag == GameViewLayer.BT_MENU then
		-- if (GlobalUserItem.bPrivateRoom and GlobalUserItem.bPrivateGoldRoom == false)  or self._scene.m_cbGameStatus == cmd.GAME_SCENE_FREE then 
		-- 	self.btnTrustee:setEnabled(false)
		-- end
		self:switchBtnChangeOrTrust()
		self:updateMenuStatus()
	
	elseif tag == GameViewLayer.BT_SET then
		print("设置")
		--self._setLayer:showLayer()
		SetLayer:create(self):addTo(self, 50)
		self:updateMenuStatus(false)
	elseif tag == GameViewLayer.BT_EXIT then
		self:updateMenuStatus(false)
		self._scene:onQueryExitGame()
	elseif tag == GameViewLayer.BT_HELP then
		self:updateMenuStatus(false)
		self:showHelpLayer()
	elseif tag == GameViewLayer.BT_ADDRESS then
		if self._scene.m_bLookOn then
			showToast(self, "观战中不可使用该功能!", 1)
		else
			self:showAdressLayer()
		end
	elseif tag == GameViewLayer.BT_START then
		self:onUserReady()
		--self:changeTable()
	elseif tag == GameViewLayer.BT_TRUSTEE then
		print("trustee")
		if self._scene.m_bLookOn then
			showToast(self, "观战中不可使用该功能!", 1)
		else
			self:updateMenuStatus(false)
		end
		self._scene:sendAutomatism(true)
		
	elseif tag == GameViewLayer.BT_OUTNONE then
		print("out none")
		self:setGameBtnStatus(false, nil, false)

		self._scene:sendPassCard()

	elseif tag == GameViewLayer.BT_PROMPT then
		print("prompt")
		local cardData = self._scene:promptCard()
		self._scene.nowTipsCardData = cardData
		dump(cardData,"======================hahaha")
		if cardData then
			self._cardLayer:popupCard(cardData)
			self:setGameBtnStatus(nil, nil, true)
			if self._scene.cbTurnCardData and #self._scene.cbTurnCardData>0 then
				local cardType = GameLogic:GetCardType(self._scene.cbTurnCardData)
            	if cardType==cmd.CT_SINGLE or 
                	cardType==cmd.CT_SINGLE_LINE or 
                	cardType==cmd.CT_DOUBLE_LINE or
                	cardType==cmd.CT_THREE_LINE  or
                	cardType==cmd.CT_BOMB then
                
                	self._cardLayer:setNotSelectCardEffect(self._scene.cbTurnCardData)
            	end
			end
		else
		 	self:onButtonClickedEvent(GameViewLayer.BT_OUTNONE)
		end
	elseif tag == GameViewLayer.BT_OUTCARD then
		print("out card")

		local outCardData = self._cardLayer:getPopupCard()
		if outCardData and #outCardData > 0 then
			self._cardLayer.bClickZero = false
			self:setGameBtnStatus(false, nil, false)
			self._scene:sendOutCard(outCardData)
		end
	elseif tag == GameViewLayer.BT_CHANGEDESK then
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
			self.btnChange:runAction(cc.Sequence:create(cc.CallFunc:create(function()
				self.btnChange:setEnabled(false)
				self:updateMenuStatus(false)
			end), cc.DelayTime:create(1.6), 
			cc.CallFunc:create(function()
				self.btnChange:setEnabled(true)
			end)))
		end
	else
		print("default")
	end
end

--计时器刷新
function GameViewLayer:OnUpdataClockView(viewId, time)
	local clockPoint ={cc.p(240,512),cc.p(500,145),cc.p(1095,512)}
	
	if not viewId or viewId == yl.INVALID_CHAIR or not time then
		self.clock_time:setString("0")
		self.spriteClock:setVisible(false)
	else
		self.clock_time:setString(time)
		self.spriteClock:setVisible(true)
		self.spriteClock:setPosition(clockPoint[viewId])
		if self._scene.m_cbGameStatus == cmd.GAME_SCENE_PLAY and viewId ==  cmd.MY_VIEWID then
			if self._scene.bForceOutCard then
				self.spriteClock:setPosition(cc.p(667,300))
			else
				self.spriteClock:setPosition(cc.p(554.5,300))
			end
		end
	end
end

--玩家准备开始
function GameViewLayer:onUserReady()
		
		self._resultLayer:setVisible(false)
		self._scene:sendStart()
end




function GameViewLayer:showAdressLayer()

	local addressLayer = cc.CSLoader:createNode("rungame_res/LocationLayer.csb"):addTo(self, 51)
	local panel1 = addressLayer:getChildByName("Panel_1")
	local panel2 = addressLayer:getChildByName("Panel_3")
	panel1:setVisible(false)
	panel2:setVisible(true)
	self._scene:showPopWait()

	for i=1,cmd.GAME_PLAYER  do
		if self.nodePlayer[i]  then
			local head = self.nodePlayer[i]:getChildByName("Panel_head"):getChildByName("_head_")
			if head then
				head:queryUserLociton(self.m_pUserItems[i].dwUserID)
			end
		end
	end

	self:runAction(cc.Sequence:create(cc.DelayTime:create(0.6),cc.CallFunc:create(function ()
		self._scene:dismissPopWait()
		panel1:setVisible(true)
		panel2:setVisible(false)

		local closeBtn = addressLayer:getChildByName("Panel_1"):getChildByName("Button_1")
		closeBtn:addTouchEventListener(function (ref, eventType)

			if eventType == ccui.TouchEventType.ended then
			    addressLayer:removeFromParent()
			end
		end)
		
		for i=1,cmd.GAME_PLAYER  do


			local userHead = self.nodePlayer[i]:getChildByName("Panel_head"):getChildByName("_head_")
			if userHead and self.nodePlayer[i]:isVisible() then
				local address = panel1:getChildByName("address"..i)
				address:setVisible(true)
				--dump(self.m_tabUserHead[i].m_userItem ,"============================")
				local userInfo = userHead.m_userInfo
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
				--address:getChildByName("location"):setDimensions(35, 200)

				
				for j=i+1,cmd.GAME_PLAYER  do
					if true then
						local la2 = 0
						local lo2 = 0
						local distanceTxt  = panel1:getChildByName("dis"..i.."_"..j)
						local distanceImg = panel1:getChildByName("xian"..i.."_"..j)

						print(i.."============="..j)
						local userHead = self.nodePlayer[j]:getChildByName("Panel_head"):getChildByName("_head_")
						if userHead then

							local userInfo = userHead.m_userInfo
							if userInfo then
								la2  = userInfo.dLatitude
								lo2 = userInfo.dLongitude
							end
							distanceTxt:setVisible(true)
							
							local sprite = cc.Sprite:createWithSpriteFrame(cc.SpriteFrame:create("rungame_res/xian1.png",cc.rect(0,0,distanceImg:getTag(),3)))
							sprite:setAnchorPoint(0,0.5)
							sprite:setRotation(distanceImg:getRotationSkewX())
							sprite:setPosition(distanceImg:getPosition())
							sprite:addTo(panel1)
								
							
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
		end
	end)))
	



end


--设置游戏按钮显隐
function GameViewLayer:setGameBtnStatus(bVisible, bOutNoneEnabled, bOutCardEnabled)
	-- print(debug.traceback())
	-- if nil ~= bVisible then
	-- 	self.btnOutNone:setVisible(bVisible)
	-- 	--self.btnReelect:setVisible(bVisible)
	-- 	self.btnPrompt:setVisible(bVisible)
	-- 	self.btnOutCard:setVisible(bVisible)
	-- end
	if self._scene.m_bLookOn then
		self._csbNode:getChildByName("Node_control"):setVisible(false)
		return
	end
	if bVisible ~= nil then
		self._csbNode:getChildByName("Node_control"):setVisible(bVisible)
	end

	local tabBtnPosX = {}
	local tabBtn = {self.btnOutNone , self.btnPrompt , self.btnOutCard}
	if self._scene.bForceOutCard then
		tabBtnPosX = {nil,450,884}
	else
		tabBtnPosX = {384,710,955}
	end
	for i=1 , 3 do
		local btn = tabBtn[i]
		if tabBtnPosX[i] then
			btn:setVisible(true)
			btn:setPositionX(tabBtnPosX[i])
		else
			btn:setVisible(false)
		end
	end

	if nil ~= bOutNoneEnabled then
		self.btnOutNone:setEnabled(bOutNoneEnabled)
		self.btnOutNone:setBright(bOutNoneEnabled)

		-- if true == bOutNoneEnabled then
		-- 	if not self._scene.bTrustee[cmd.MY_VIEWID]  then 
		-- 		self:onButtonClickedEvent(GameViewLayer.BT_OUTNONE)
		-- 	end
		-- 	return
		-- end
	end

	if nil ~= bOutCardEnabled then
		self.btnOutCard:setEnabled(bOutCardEnabled)
		self.btnOutCard:setBright(bOutCardEnabled)
	end
end





function GameViewLayer:priGameLayerZorder()
    return 10
end

function GameViewLayer:showTDtip()
	if  self.isShowTip then
		return 
	end
	self.isShowTip = true
	local tip = cc.CSLoader:createNode("rungame_res/help/Help1Layer.csb"):addTo(self, 50)

		--按钮回调
	local btnCallback = function(ref, eventType)
		if eventType == ccui.TouchEventType.ended then
			tip:removeFromParent()
		end
	end


	local panel = tip:getChildByName("Panel_1")
	panel:addTouchEventListener(btnCallback)

	local close = tip:getChildByName("Button_close")
	close:addTouchEventListener(btnCallback)
end

--炸弹数
function GameViewLayer:setBombNum(viewId, num)
	local fonts = {"炸一轮", "炸二轮", "炸三轮", "炸四轮"}
	local textBomb = self.nodePlayer[viewId]:getChildByName("Text_bomb")
	textBomb:setString(fonts[num])
	if num == 0 then
		textBomb:setVisible(false)
	else
		textBomb:setVisible(true)
	end
    self.bombNum[viewId] = num
end


function GameViewLayer:setHandNum(isBg,count)
	
	if isBg and count then
		self.nodeLastCard[1]:setVisible(count[1] ~= nil)
		self.nodeLastCard[3]:setVisible(count[2] ~= nil)
	end
	

	if count then
		self.nodeLastCard[1]:getChildByName("num"):setString(count[1] and count[1].."" or "0")
		self.nodeLastCard[3]:getChildByName("num"):setString(count[2] and count[2].."" or "0")
	else
		self.nodeLastCard[1]:getChildByName("num"):setString("0")
		self.nodeLastCard[3]:getChildByName("num"):setString("0")
	end
end

function GameViewLayer:gameSendCard(cardData, cardCount,isAni)
	



	--玩家剩余牌数
	--for i = 1, 3 do
		-- self.nRemainCard[i] = cardCount[i]
		-- local remainCard = self.nodePlayer[i]:getChildByName("Text_remainCard")
		-- remainCard:setString(self.nRemainCard[i])
		-- remainCard:setVisible(true)
	--end
	self._cardLayer:showSendCardAni(cardData, cardCount,isAni)
	--self._cardLayer.cbCardData = clone(cardData)
	--self._cardLayer:sortHandCard()
	--self._cardLayer:setMyCardTouchEnabled(true)
end

function GameViewLayer:GameVideoSendCard(cardData,cardCount,viewId)
	assert(viewId>=1 and viewId<=3)
	self.nRemainCard[viewId] = cardCount[viewId]
	local remainCard = self.nodePlayer[viewId]:getChildByName("Text_remainCard")
	remainCard:setString(self.nRemainCard[viewId])
	remainCard:setVisible(true)
	self._cardLayer:setVideoHandCard(cardData,cardCount[viewId],viewId)
end

function GameViewLayer:showAutoLayer(bvisible)
	

	local autoLayer = self._csbNode:getChildByName("autoLayer")
	
	--autoLayer:setVisible(true)
	dump({bvisible},"================")
	if bvisible then
		self._scene.bMyAutoStatus = true
	    self._csbNode:getChildByName("Node_control"):setVisible(false)
	    self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),cc.CallFunc:create(function ()
	        autoLayer:setVisible(bvisible)
	    end)))
	else
		autoLayer:setVisible(bvisible)
		self._scene.bMyAutoStatus = false
	end

end


function GameViewLayer:autoPlay(isDelay)
	local delayTime = 0.01
	if isDelay  then
		delayTime = 0.5
	end
	self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ( ... )
		if self._scene.m_cbGameStatus ==  cmd.GAME_SCENE_FREE then
			if self.btnStart:isVisible() then
				self:onButtonClickedEvent(GameViewLayer.BT_START)
			end
		-- elseif self._scene.m_cbGameStatus ==  cmd.GAME_SCENE_PLAY  then
		-- 	local control = self._csbNode:getChildByName("Node_control")
		-- 	if self._scene.wCurrentUser == self._scene._MyChairID  then
		-- 		self:onButtonClickedEvent(GameViewLayer.BT_PROMPT)
		-- 		self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ()
		-- 			if control:isVisible() then
		-- 				self:onButtonClickedEvent(GameViewLayer.BT_OUTCARD)
		-- 			end
		-- 		end)))
				
		-- 	end

		end

		if isDelay then
			self:onButtonClickedEvent(GameViewLayer.BT_OUTNONE)

		end
	end)))
	if self._scene.m_cbGameStatus ~= cmd.GAME_SCENE_FREE then
		self:addErrorCardTips()
	end
	
	if not isDelay then
		self:onButtonClickedEvent(GameViewLayer.BT_OUTNONE)
	end

end

function GameViewLayer:addErrorCardTips(type)--1是没有打得过的牌   2是请选择正确牌型
	local fileName = "rungame_res/img_notCardTips.png"
	if type==2 then
		fileName = "rungame_res/img_errorCardTips.png"
	end
	local sp = cc.Sprite:create(fileName)
	sp:setPosition(cc.p(667,30))
	self._csbNode:addChild(sp)
	sp:setLocalZOrder(20)
	sp:runAction(cc.Sequence:create(cc.DelayTime:create(1) , cc.RemoveSelf:create()))
end



--不出牌
function GameViewLayer:gamePassCard(viewId)
	if viewId < 1 or viewId > 3 then
		return false
	end

	
	self._cardLayer:showPass(viewId)


	--声音
	if self.cbGender[viewId] == 0 then
    	self._scene:PlaySound(GameViewLayer.RES_PATH.."sound/GIRL/PASS_CARD.wav")
	else
    	self._scene:PlaySound(GameViewLayer.RES_PATH.."sound/BOY/PASS_CARD.wav")
	end
end

--游戏结束
function GameViewLayer:gameEnded()
	self.btnStart:setVisible(true)
	-- for i = 1, 3 do
 --    	--self.bombNum[i] = 0
	-- 	--self:setTrusteeVisible(i, false)
	-- 	if self.spriteWarnAnimate[i] then
	-- 		self.spriteWarnAnimate[i]:removeFromParent()
	-- 		self.spriteWarnAnimate[i] = nil
	-- 	end
	-- end
	--self._handCardLayer:gameEnded()
end

function GameViewLayer:removeOutCard(viewId)
	if nil ~= self.nodeOutCard[viewId] then
		self.nodeOutCard[viewId]:removeFromParent()
		self.nodeOutCard[viewId] = nil
	end
end

--用户聊天
function GameViewLayer:userChat(wViewChairId, chatString)
	if chatString and #chatString > 0 and self.nodePlayer[wViewChairId] then
		if wViewChairId == cmd.MY_VIEWID then
			self._chatLayer:showGameChat(false)
		end

		local m_spChatBg = self.chatBubble[wViewChairId] --self.nodePlayer[wViewChairId]:getChildByName("chat")
		m_spChatBg:setVisible(true)
		m_spChatBg:stopAllActions()
		m_spChatBg:removeAllChildren()

		local m_labChat = cc.LabelTTF:create(chatString, "fonts/round_body.ttf", 20, cc.size(170,0), cc.TEXT_ALIGNMENT_CENTER)
	         	m_labChat:setColor(cc.c3b(1,42,48)) 
	        	m_spChatBg:addChild(m_labChat)

	    	self.nodePlayer[wViewChairId]:removeChildByName("EMOJ_CHAT")

	   	 -- 尺寸调整
	    	local labSize = m_labChat:getContentSize()
	    	if labSize.height >= 40 then
	        		m_spChatBg:setContentSize(200, labSize.height + 54)
	    	else
	        		m_spChatBg:setContentSize(200, 70)
	    	end

	    	m_labChat:setPosition(m_spChatBg:getContentSize().width * 0.5, m_spChatBg:getContentSize().height * 0.5 +7)



	    	if wViewChairId ==3 or wViewChairId ==4 then
	        		m_labChat:setScaleX(-1)
	    	end


	    	m_spChatBg:runAction(cc.Sequence:create(cc.DelayTime:create(3.0), cc.Hide:create()))
    end
end

--用户表情
function GameViewLayer:userExpression(wViewChairId, wItemIndex)

	if not self.nodePlayer[wViewChairId] then
		return
	end
	    self.nodePlayer[wViewChairId]:removeChildByName("EMOJ_CHAT")
	    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", wItemIndex))
	    --缓存表情
	    --动画


	      --加载动画
	    local frames = {}
	    local actionTime = 0.2
	    for i=1,32 do
	        local strName = string.format("emoj%d_%d.png",wItemIndex, i)
	        print("browChatSprName--------------------->", strName)
	        local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
	        if spriteFrame then
	            table.insert(frames, spriteFrame)
	        else
	            break
	        end
	    end
	    local  animation = cc.Animation:createWithSpriteFrames(frames,actionTime)    
	    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", wItemIndex))
	    local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
	    self.nodePlayer[wViewChairId]:addChild(spr)
	    spr:setName("EMOJ_CHAT")
	    spr:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
	    self.nodePlayer[wViewChairId]:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()
        
	       self.nodePlayer[wViewChairId]:removeChildByName("EMOJ_CHAT")
	    end)))

end

function GameViewLayer:manageCardType(viewId, cbCardData)
	local str = nil
	if self.cbGender[viewId] == 0 then
		str = GameViewLayer.RES_PATH.."sound/GIRL/"
	else
		str = GameViewLayer.RES_PATH.."sound/BOY/"
	end

	local cbFirstValue = GameLogic:GetCardValue(cbCardData[1])
	local cbCardType = GameLogic:GetCardType(cbCardData)
	if cbCardType == cmd.CT_SINGLE then --单张
		local strPath = str..string.format("SINGLE_%d.wav", cbFirstValue)
		self._scene:PlaySound(strPath)

	elseif cbCardType == cmd.CT_DOUBLE_LINE then --对连
		if #cbCardData == 2 then
			local strPath = str..string.format("DOUBLE_%d.wav", cbFirstValue)
			self._scene:PlaySound(strPath)
		else
			self._scene:PlaySound(str.."DOUBLE_LINE.wav")
		end

	elseif cbCardType == cmd.CT_THREE_LINE then --三连
		if #cbCardData == 3 then
			self._scene:PlaySound(str.."THREE.wav")
		else
			self._scene:PlaySound(str.."THREE_LINE.wav")
		end

	elseif cbCardType == cmd.CT_THREE_LINE_TAKE_SINGLE then --三带单
		self._scene:PlaySound(str.."THREE_TAKE_ONE.wav")
		
	elseif cbCardType == cmd.CT_THREE_LINE_TAKE_DOUBLE then --三带双
		
		self._scene:PlaySound(str.."THREE_TAKE_TWO.wav")
		
	elseif cbCardType == cmd.CT_THREE_TAKE_TWO_SINGLE then --三带2单

		
		self._scene:PlaySound(str.."THREE_TAKE_TWOS.mp3")

	elseif cbCardType == cmd.CT_SINGLE_LINE then -- 顺子
		self._scene:PlaySound(str.."SINGLE_LINE.wav")

	elseif cbCardType == cmd.CT_BOMB then -- 炸弹
		self._scene:PlaySound(str.."BOMB_CARD.wav")
		--self:setBombNum(viewId, self.bombNum[viewId] + 1)
	elseif cbCardType == cmd.CT_FOUR_TAKE_THREE then -- 4带3
		if self.cbGender[viewId] == 1 then
			self._scene:PlaySound(str.."FOUR_TAKE_THREE.wav")
		else
			self._scene:PlaySound(str.."FOUR_TAKE_THREE.mp3")
		end
		

	elseif cbCardType >=  cmd.CT_TWO_THREE_LINE_TAKE_TWO_SINGLE and cbCardType <= cmd.CT_FOUR_THREE_LINE_TAKE_FOUR_SINGLE then
		if #cbCardData < 12 then
			self._scene:PlaySound(str.."PLANE.mp3")
		else
			self._scene:PlaySound(str.."PLANEWINS.mp3")
		end
	end
end

--托管遮盖层
function GameViewLayer:showTrusteeLayer(bShow)
	

end

--警报动画
function GameViewLayer:runWarnAnimate(viewId)
	if self.spriteWarnAnimate[viewId] then
		return
	end

	local animation = cc.AnimationCache:getInstance():getAnimation("WARN")
	local animate = cc.Animate:create(animation)
	print("comming?", animation, animate)
	self.spriteWarnAnimate[viewId] = display.newSprite()
		:move(posWarn[viewId])
		:addTo(self, 2)
	self.spriteWarnAnimate[viewId]:runAction(animate)
end

return GameViewLayer