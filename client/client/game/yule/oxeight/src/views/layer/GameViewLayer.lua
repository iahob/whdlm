local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)

require("client/src/plaza/models/yl")
local cmd = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.CMD_Game")
local SetLayer = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.views.layer.SetLayer")
local CardSprite = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.views.layer.CardSprite")
local PlayerInfo = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.views.layer.PlayerInfo")
--local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local GameChatLayer = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.views.layer.GameChatLayer")
local CardLayer = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.views.layer.CardLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local SpineObj = appdf.req("client.src.external.SpineObj")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.GameLogic")
local VOICE_BTN_NAME = "__voice_record_button__"  --语音按钮名字，可以获取语音按钮，控制显示与否

local SuperControlLayer = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.views.layer.SuperControlLayer")
local TrustLayer = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.views.layer.TrustLayer")
-- GameViewLayer.BT_SWITCH 			= 12
-- GameViewLayer.BT_SET 				= 13
-- GameViewLayer.BT_CHAT 				= 14
-- GameViewLayer.BT_HOWPLAY 			= 15
-- GameViewLayer.BT_EXIT 				= 16
--GameViewLayer.BT_SOUND 				= 15
--GameViewLayer.BT_TAKEBACK 			= 16

GameViewLayer.TAG_START				= 100
local enumTable = 
{
	"BT_START",	  --开始
	"BT_CHANGEDESK",--换桌
	"BT_SWITCH",  --菜单
	"BT_HELP",    --帮助
	"BT_SET",     --设置
	"BT_CHAT",    --聊天
	"BT_EXIT",    --退出
	"BT_TIPS",  --提示
	"BT_CONFIRM",  --确认按钮
	"BT_CHIP1",   --下注按钮
	"BT_CHIP2",   --下注按钮
	"BT_CHIP3",   --下注按钮
	"BT_CHIP4",   --下注按钮
	"NUM_CHIP", 	--下注数字
	"BT_MUL1",   --倍数按钮
	"BT_MUL2",   --倍数按钮
	"BT_MUL3",   --倍数按钮
	"BT_MUL4",   --倍数按钮
	"BT_MUL5",   --倍数按钮
	"NUM_MUL", 		--倍数数字
	"BT_CALLBANKER", --叫庄
	"BT_CANCEL",  --不叫庄
	"BT_CALL",--自由抢庄
	"BT_SHOW",  --亮牌
	"BT_CUO",  --搓牌
	"BT_AUTO",  --托管
	"BT_INVITE",  --邀请
	"BT_ADDRESS",
	"BT_SIT",--旁观状态坐下
	"BT_BEDIN",--房主开始游戏
	"BT_SHOWAUTO",--打开托管界面
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(GameViewLayer.TAG_START, enumTable);

GameViewLayer.TAG_ENUM = TAG_ENUM

GameViewLayer.ZORDER_BET = 1
GameViewLayer.ZORDER_AUTO = 2
GameViewLayer.TopZorder = 30
GameViewLayer.MenuZorder = 20
GameViewLayer.ZORDER_CHAT = 20

GameViewLayer.ZORDER_SETTING = 70
GameViewLayer.ZORDER_INFO = 90
GameViewLayer.ZORDER_TRUST = 99	
GameViewLayer.ZORDER_RESULT = 100	
GameViewLayer.ZORDER_CONTROL = 101

GameViewLayer.BT_RULE		= 500



GameViewLayer.FRAME 				= 1
GameViewLayer.NICKNAME 				= 2
GameViewLayer.SCORE 				= 3
GameViewLayer.FACE 					= 7

GameViewLayer.TIMENUM   			= 1
GameViewLayer.CHIPNUM 				= 1
GameViewLayer.SCORENUM 				= 1

--牌间距
GameViewLayer.CARDSPACING 			= 35

--GameViewLayer.VIEWID_CENTER 		= 5

GameViewLayer.RES_PATH 				= "game/yule/oxeight/res/"


--local pointCard = {cc.p(667, 617), cc.p(298, 402), cc.p(667, 110), cc.p(1028, 402)}
--local pointClock = {cc.p(1017, 640), cc.p(88, 560), cc.p(157, 255), cc.p(1238, 560)}
--local pointOpenCard = {cc.p(437, 625), cc.p(288, 285), cc.p(917, 115), cc.p(1038, 285)}
--local pointTableScore = {cc.p(667, 505), cc.p(491, 410), cc.p(667, 342), cc.p(835, 410)}
--local pointBankerFlag = {cc.p(948, 714), cc.p(159, 499), cc.p(228, 204), cc.p(1168, 495)}

--local pointChat = {cc.p(767, 690), cc.p(160, 525), cc.p(230, 250), cc.p(1173, 525)}
local pointUserInfo = {cc.p(70, 447), cc.p(70, 301), cc.p(119, 101), cc.p(1267, 302), cc.p(1267, 453), cc.p(922, 699),cc.p(666, 699),cc.p(433, 699),}
--local anchorPoint = {cc.p(1, 1), cc.p(0, 0.5), cc.p(0, 0), cc.p(1, 0.5)}

local AnimationRes = 
{
	{name = "victory", file = GameViewLayer.RES_PATH.."animation/victory_", nCount = 19, fInterval = 0.07, nLoops = 2},
-- 	{name = "lose", file = GameViewLayer.RES_PATH.."animation_lose/lose_", nCount = 17, fInterval = 0.1, nLoops = 1},
-- 	{name = "start", file = GameViewLayer.RES_PATH.."animation_start/start_", nCount = 11, fInterval = 0.15, nLoops = 1},
}

local BANKER_ACT = 101
local FLY_COIN_TAG = 102

function GameViewLayer:getParentNode()
	return self._scene
end

function GameViewLayer:onInitData()
	self.bCardOut = {false, false, false, false, false}
	self.lUserMaxScore = {0, 0, 0, 0}
	self.cbCombineCard = {}
	self.chatDetails = {}
	self.bCanMoveCard = false
	self.cbGender = {}
	self.bBtnInOutside = false
	self.bSpecialType  = false --特殊牌型标识
	self.cbSpecialCardType = 0 --特殊牌型代码
	self.bIsShowMenu = false --是否显示菜单
	self.bSendCardCtr= false

	self.bIsAuto = false
	self.bIsMaxBet = false 
	self.nAutoStatus = cmd.AUTO_PLAY_FREE
	self.bLastCall = TAG_ENUM.BT_CANCEL
	-- 用户头像
    	self.m_tabUserHead = {}
    	self.bIsTimeOutPlay = false

    	self.goldlist  = {} --金币列表

	--房卡需要
	--self.m_UserItem = {}

	--回放
	self.bIsVideo = false 

	-- 是否翻牌
	self._bShowCard = false
end
function GameViewLayer:removeAllChildByTag(tag)
	local child = self:getChildByTag(tag)
	if child then
		child:stopAllActions()
		child:removeFromParent()
		return self:removeAllChildByTag(tag)
	end
end

function GameViewLayer:cleanFlyScore()
    for i=1,cmd.GAME_PLAYER do
    	self._csbNode:getChildByName("Node_FlyScore"):removeAllChildren()
    	-- local score = self:getChildByName("flyScore"..i)
    	-- print("*******************************cleanFlyScore***************",score)
    	-- if score then
    	-- 	score:stopAllActions()
    	-- 	score:removeFromParent()
    	-- end
    end
end

function GameViewLayer:onResetView()
	print("********************************************onResetView")
	self._csbNode:getChildByName("Node_AniCard"):stopAllActions()
	local children = self._csbNode:getChildByName("Node_AniCard"):getChildren()
	for k,v in pairs(children) do
		v:stopAllActions()
		v:removeFromParent()
	end
	local endAction = self:getChildByName("endAction")
	if endAction then
		endAction:stopAllActions()
		endAction:removeFromParent()
	end
	self:removeAllChildByTag(FLY_COIN_TAG)
	--self.nodeLeaveCard:removeAllChildren()
	--self.spriteBankerFlag:setVisible(false)
	self.spriteCalculate:setVisible(false)
	self._scene:stopAllActions()
	self:stopActionByTag(BANKER_ACT)
	--清除飘分动画
	self:cleanFlyScore()

    for i=1,cmd.GAME_PLAYER do
      self.flag_openCard[i]:setVisible(false)
      self.flag_openCard[i]:setScale(1)
      self.flag_openCard[i]:removeAllChildren()
      for j=1, cmd.MAX_CARDCOUNT do
        local pcard = self.nodeCard[i][j]
        pcard:setVisible(false)
        pcard:setFiveCard(false)
        self:setCardTextureRect(i,j)
      end
      self._csbNode:getChildByName("Node_betScore"):getChildByName("coinBg_"..i):setVisible(false)
      if self.m_tabUserHead[i] then
      	self.m_tabUserHead[i]:showCallInScore("",false)
      	self.m_tabUserHead[i]:hiddenMultiple()
      	self.m_tabUserHead[i]:setCallingBankerStatus(false)
      	self.m_tabUserHead[i]:showBank(false)
      	self.m_tabUserHead[i]:setFlashBg(false)
      	self.m_tabUserHead[i].csbNode:getChildByName("banker_waitIcon"):setVisible(false)
      end
    end

	self.bCardOut = {false, false, false, false, false}
	self.bBtnMoving = false
	self:updateCardType(0,false)

	self.labCardType:setString("")
	for i = 1, 3 do
		self.labAtCardPrompt[i]:setString("")
	end

	local nodeCard = self._csbNode:getChildByName("Node_Card")
	    for i=1, cmd.MAX_CARDCOUNT do
	    	local panelCard = nodeCard:getChildByName(string.format("Node_%d",i))
	        local card = self.nodeCard[cmd.MY_VIEWID][i]
	        card:setPosition(130*(i-1), panelCard:getContentSize().height/2)
	    end

	self.nodeChip:removeAllChildren()
	self.nodeCallMul:removeAllChildren()
	self.btMul = {}
	self.btCuo:setVisible(false) --隐藏确定
	self:updateButton(false,false)



	--移除庄家按钮
	self:setBankerUser()

	self.cbCombineCard = {}	   --组合扑克
	self.bSpecialType  = false --特殊牌型标识
	self.cbSpecialCardType = 0 --特殊牌型代码

	self._scene.wBankerUser = yl.INVALID_CHAIR

	self._scene.bAddScore = false
    	self._scene.m_lMaxTurnScore = 0

	for viewId=1,cmd.GAME_PLAYER do
		if viewId ~= cmd.MY_VIEWID then 
			self:stopCardAni(viewId)
		end
	end	
	self._scene.m_enterMessage = cmd.GS_TK_FREE
	self:removeChildByName("banker_xuanBei")
	self:closeCuoLayer()
	
	-- 是否翻牌
	self._bShowCard = false
	local spBanker = self._csbNode:getChildByName("Sprite_banker")
	if spBanker then
		spBanker:removeFromParent()
	end
end


function GameViewLayer:resetAutoSet()
	print("*************resetAutoSet",self.bIsAuto)
	if self.bIsAuto == true then
		self:onButtonClickedEvent(TAG_ENUM.BT_AUTO)
	end
	-- self.bIsAuto = false
	-- self.bIsMaxBet = false 
	-- self.nAutoStatus = cmd.AUTO_PLAY_FREE
	-- self.bLastCall = TAG_ENUM.BT_CANCEL
	-- self.btAuto:loadTextureNormal(cmd.RES_PATH.."game/auto1.png")
 -- 	self.btAuto:loadTexturePressed(cmd.RES_PATH.."game/auto1.png")
 -- 	self.minBet:loadTextureNormal(cmd.RES_PATH.."game/btn_min2.png")
 -- 	self.minBet:loadTexturePressed(cmd.RES_PATH.."game/btn_min2.png")

 -- 	self.maxBet:loadTextureNormal(cmd.RES_PATH.."game/btn_max1.png")
 -- 	self.maxBet:loadTexturePressed(cmd.RES_PATH.."game/btn_max1.png")
 -- 	self.maxBet:getParent():setVisible(false)
 -- 	self.btAuto:getChildByName("Button_down"):setTag(1)
end

function GameViewLayer:onExit()
	print("GameViewLayer onExit")
	self:gameDataReset()

	cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.schedule)
	cc.Director:getInstance():getTextureCache():removeTextureForKey("game/card.png")

	-- cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("gameScene_oxnew.plist")
	-- cc.Director:getInstance():getTextureCache():removeTextureForKey("gameScene_oxnew.png")
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
    for i=1,#AnimationRes do
    	cc.AnimationCache:getInstance():removeAnimation(AnimationRes[i].name)
    end
    
    --重置搜索路径
	local oldPaths = cc.FileUtils:getInstance():getSearchPaths();

	local newPaths = {};
	for k,v in pairs(oldPaths) do
		if tostring(v) ~= tostring(self._searchPath1) and tostring(v) ~= tostring(self._searchPath2) then
			table.insert(newPaths, v);
		end
	end
	cc.FileUtils:getInstance():setSearchPaths(newPaths);

	if self._beginTimeId then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
		self._beginTimeId = nil
	end
end

function GameViewLayer:gameDataInit(  )
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
    	self._searchPath = device.writablePath.."game/" .. gameInfo._Module .. "/res/";
   		cc.FileUtils:getInstance():addSearchPath(self._searchPath);
    end
end
function GameViewLayer:gameDataReset()

  --播放大厅背景音乐
  self.m_bMusic = false
  AudioEngine.stopMusic()
  ExternalFun.playPlazzBackgroudAudio()

  self:unLoadRes()

  --重置搜索路径
  local oldPaths = cc.FileUtils:getInstance():getSearchPaths()
  local newPaths = {};
  for k,v in pairs(oldPaths) do
    if tostring(v) ~= tostring(self._searchPath) then
      table.insert(newPaths, v);
    end
  end
  cc.FileUtils:getInstance():setSearchPaths(newPaths)
end

local this
function GameViewLayer:ctor(scene)
	this = self
	self._scene = scene

	self.m_tabUserItem = {}

  	self:gameDataInit()

	self:onInitData()
	self:preloadUI()

	--牌节点
	self.nodeCard = {}
	--牌的类型
	self.cardType = {}
	-- --准备标志
	self.flag_ready = {}
	--摊牌标志
	self.flag_openCard = {}

  	--导入资源
	self:loadRes()
	--初始化csb界面
  	self:initCsbRes()
	--四个玩家
	self.nodePlayer = {}

	self:initUserInfo()

    local mgr = self._scene._scene:getApp():getVersionMgr()
    local verstr = mgr:getResVersion(cmd.KIND_ID) or "0"
    verstr = "游戏版本:" .. appdf.BASE_C_VERSION .. "." .. verstr
	self._setLayer = SetLayer:create(verstr)
    self:addChild(self._setLayer)
    self._setLayer:setLocalZOrder(30)
    self._setLayer:setVisible(false)

	--节点事件
	local function onNodeEvent(event)
		if event == "exit" then
			self:onExit()
		end
	end
	self:registerScriptHandler(onNodeEvent)

	--self.nodeLeaveCard = cc.Node:create():addTo(self)

    --播放背景音乐
    if GlobalUserItem.bVoiceAble == true then  
        AudioEngine.playMusic("sound_res/backMusic.mp3", true)
    end

	--房卡需要
	--语音按钮 gameviewlayer -> gamelayer -> clientscene
   -- self._scene._scene:createVoiceBtn(cc.p(1250, 200), 0, self)

    -- 语音动画
    AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)	


    

	-- local  btcallback = function(ref, type)
 --        if type == ccui.TouchEventType.ended then
 --         	this:onButtonClickedEvent(ref:getTag(),ref)
 --        end
 --    end
 	
	
	--点击事件
	self:setTouchEnabled(true)
	self:registerScriptTouchHandler(function(eventType, x, y)
		return self:onEventTouchCallback(eventType, x, y)
	end)

	-- 玩家头像
	-- self.m_bNormalState = {}

end

--加载资源
function GameViewLayer:loadRes( )
  cc.Director:getInstance():getTextureCache():addImage("game/card.png")
  -- 语音动画
  AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

end

--移除资源
function GameViewLayer:unLoadRes()
	--卡牌
  cc.Director:getInstance():getTextureCache():removeTextureForKey("game/card.png")
  --语音动画
  AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)

  cc.Director:getInstance():getTextureCache():removeUnusedTextures()
  cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

end

--加载csb
function GameViewLayer:initCsbRes()

    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/GameScene.csb",self)
    self._csbNode = csbNode


    self:initMenus()
    self:initButtonEvent()
    -- self:gameClean()

    -- self:showUserInfo(self._scene:GetMeUserItem())
    
    --卡牌
    local nodeCard = self._csbNode:getChildByName("Node_Card")
    for i=1,cmd.GAME_PLAYER do
      local panelCard = nodeCard:getChildByName(string.format("Node_%d",i))
      local cardState = panelCard:getChildByName("Sprite_state")
      cardState:setLocalZOrder(9)
      self.flag_openCard[i] = cardState   --牌类型
      self.nodeCard[i] = {}
      for j=1, cmd.MAX_CARDCOUNT do
        local pcard = CardSprite:createCard()
        pcard:setVisible(false)
        pcard:setTag(j)
        panelCard:addChild(pcard)
        pcard:setAnchorPoint(cc.p(0.5,0.5))
        pcard:setLocalZOrder(j)
        table.insert(self.nodeCard[i], pcard)
        self:setCardTextureRect(i,j)
        if i == cmd.MY_VIEWID then
			pcard:setPosition(130*(j-1), 0)
        else
        	pcard:setPosition(33+25*(j-1), 0)
        	pcard:setScale(0.6)
        end

      end
    end

    
    for i=1,cmd.GAME_PLAYER do
    	--准备标识
    	self.flag_ready[i] = self._csbNode:getChildByName("ready_" .. i)
    	self.flag_ready[i]:setVisible(false)
    end

    --倒计时
    self._clockTimeBg = self._csbNode:getChildByName("Sprite_clockBg")
    self._clockTime = self._clockTimeBg:getChildByName("num_clock")
    self._clockTime:setString("30")
    self:setClockVisible(false)
    self:setClockBgVisible(false)

    -- self._csbNode:getChildByName("Sprite_13"):getChildByName("Text_5"):setString("")
    self._csbNode:getChildByName("clock_node"):setVisible(false)

    -- self._csbNode:getChildByName("Sprite_logo"):setVisible(false)
    
    --计分器
    self.spriteCalculate = self._csbNode:getChildByName("FileNode_calculate") 
    self.spriteCalculate:setVisible(false)

	--牌值
	self.labAtCardPrompt = {}
	for i = 1, 3 do
		self.labAtCardPrompt[i] = self.spriteCalculate:getChildByName(string.format("Text_num%d",i)) 
	end
	self.labCardType = self.spriteCalculate:getChildByName("Text_num4")

	self:initRoomInfo()

	--[[local btnVoice = ccui.Button:create(cmd.RES_PATH.."common/btn_Voice1.png", cmd.RES_PATH.."common/btn_Voice2.png",
		cmd.RES_PATH.."common/btn_Voice2.png")
		:move(1268, 200)
		:addTo(self)
	btnVoice:setName(VOICE_BTN_NAME)
	btnVoice:addTouchEventListener(function(ref, eventType)
 		if eventType == ccui.TouchEventType.began then
 			self:getParentNode():getParentNode()._sceneLayer:startVoiceRecord()
        elseif eventType == ccui.TouchEventType.ended 
        	or eventType == ccui.TouchEventType.canceled then
        	self:getParentNode():getParentNode()._sceneLayer:stopVoiceRecord()
        end
	end)]]

	-- 电量,时间，信号
	local bg = self._csbNode:getChildByName("top_Bg")
	local BatteryBg = bg:getChildByName("dianliang")
	local sp = cc.Sprite:create("game/yule/oxeight/res/game/dianliang2.png")
	self.batteryBar = cc.ProgressTimer:create(sp)
	self.batteryBar:setType(cc.PROGRESS_TIMER_TYPE_BAR)
    self.batteryBar:setMidpoint(cc.p(0,0.5))
    self.batteryBar:setBarChangeRate(cc.p(1, 0))
    self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
    self.batteryBar:setPosition(BatteryBg:getContentSize().width/2-2, BatteryBg:getContentSize().height/2)
    BatteryBg:addChild(self.batteryBar)
	bg:getChildByName("Time"):setString(os.date("%H:%M"))
    self.schedule = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
		self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
		bg:getChildByName("Time"):setString(os.date("%H:%M"))
		local netType = MultiPlatform:getInstance():getNetworkType()
		local Strength = MultiPlatform:getInstance():getNetworkStrength()
		local TypePic = {"net_","wifi_"}
		if TypePic[netType] ==nil or Strength == nil then
			return;
		end
		bg:getChildByName("wifi"):setTexture("game/"..TypePic[netType]..Strength..".png")
	end, 1, false)

end

function GameViewLayer:initMenus()
	local _menusMask = ccui.ImageView:create()
	_menusMask:setContentSize(cc.size(yl.WIDTH, yl.HEIGHT))
	_menusMask:setScale9Enabled(true)
	_menusMask:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
	_menusMask:setTouchEnabled(true)
	self:addChild(_menusMask,GameViewLayer.MenuZorder)
	_menusMask:addTouchEventListener(function (sender,eventType)
	          if eventType == ccui.TouchEventType.ended then
	              _menusMask:setVisible(false)
	          end
	        end)

	self._menu = _menusMask
	self._menu:setVisible(false)
	local layer, node = ExternalFun.loadRootCSB("game/Menus.csb",_menusMask)
	self.nodeMenu = node:getChildByName("menu")
	self.nodeMenu:setContentSize(76,442)

end

function GameViewLayer:initButtonEvent( )

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

	--四个下注的筹码按钮
	self.nodeChip = self._csbNode:getChildByName("Node_score")
	self.nodeCallMul = self._csbNode:getChildByName("Node_callMul")
	self.btChip = {}
	self.btMul = {}

	--叫庄按钮
	self.nodeCall = self._csbNode:getChildByName("Node_call")

	--不叫庄按钮
	self.btCancel = self.nodeCall:getChildByName("Button_nocall")
	self.btCancel:setTag(TAG_ENUM.BT_CANCEL)
	self.btCancel:addTouchEventListener(btcallback)

	--叫庄按钮
	self.btCall = self.nodeCall:getChildByName("Button_call")
	self.btCall:addTouchEventListener(btcallback)
	self.btCall:setTag(TAG_ENUM.BT_CALL)

	--菜单按钮
	self.btSwitch = self._csbNode:getChildByName("Button_switch")
		:setTag(TAG_ENUM.BT_SWITCH)
	self.btSwitch:addTouchEventListener(btcallback)

	--开始
	self.btStart = self._csbNode:getChildByName("Button_start")
		:setTag(TAG_ENUM.BT_START)
	self.btStart:addTouchEventListener(btcallback)
	self.btStart:setVisible(false)

	self.btSitAndBegin = self._csbNode:getChildByName("Button_sit")
	self.btSitAndBegin:setTag(TAG_ENUM.BT_SIT)
	self.btSitAndBegin:addTouchEventListener(btcallback)
	self.btSitAndBegin:setVisible(false)	

	--桌面托管按钮
	self.Btn_changeDesk = self._csbNode:getChildByName("Button_changeDesk")
		:setTag(TAG_ENUM.BT_CHANGEDESK)
	self.Btn_changeDesk:addTouchEventListener(btcallback)
	self.Btn_changeDesk:setVisible(false)

           --定位
	self.btAddress = self._csbNode:getChildByName("Button_address")
		:setTag(TAG_ENUM.BT_ADDRESS)
	self.btAddress:addTouchEventListener(btcallback)

	local ani1 = ExternalFun.loadCSB("game/locationAnim.csb", self.btAddress)
	ani1:setTag(163)
	ani1:setPosition(-5,-3)
	local aniAction = ExternalFun.loadTimeLine("game/locationAnim.csb")
	aniAction:gotoFrameAndPlay(0,25,true)
	ani1:runAction(aniAction)

	--换桌
	self.btChangedesk = self.nodeMenu:getChildByName("Button_change")
	self.btChangedesk:setTag(TAG_ENUM.BT_CHANGEDESK)
	self.btChangedesk:addTouchEventListener(btcallback)
	self.btChangedesk:setVisible(true)
	self.btChangedesk:setPositionY(130)

	--邀请
	self.btnInvite = self.nodeMenu:getChildByName("Button_invite")
	self.btnInvite:addTouchEventListener(btcallback)
	self.btnInvite:setTag(TAG_ENUM.BT_INVITE)
	self.btnInvite:setPositionY(210)
	
	if GlobalUserItem.bPrivateRoom then
		self.btChangedesk:setEnabled(false)
		self.btnInvite:setVisible(true)
	else
		self.nodeMenu:setContentSize(76,362)
		self.btnInvite:setVisible(false)
	end

	--设置
	local btn = self.nodeMenu:getChildByName("Button_set")
	btn:addTouchEventListener(btcallback)
	btn:setTag(TAG_ENUM.BT_SET)
	if not GlobalUserItem.bPrivateRoom then
		btn:setPositionY(210)
	end
	--退出
	local btn = self.nodeMenu:getChildByName("Button_leave")
	btn:addTouchEventListener(btcallback)
	btn:setTag(TAG_ENUM.BT_EXIT)

	local btn = self.nodeMenu:getChildByName("Button_help")
	btn:addTouchEventListener(btcallback)
	btn:setTag(TAG_ENUM.BT_HELP)
	if not GlobalUserItem.bPrivateRoom then
		btn:setPositionY(290)
	end


	local btn = self._csbNode:getChildByName("btn_talk")
	btn:addTouchEventListener(btcallback)
	btn:setTag(TAG_ENUM.BT_CHAT)

    --确认按钮
   -- self.btConfirm = self._csbNode:getChildByName("Button_ok")
  --  self.btConfirm:addTouchEventListener(btcallback)
    --self.btConfirm:setTag(TAG_ENUM.BT_CONFIRM)
    --self.btConfirm:setVisible(false)

    --亮牌
    self.btShow = self._csbNode:getChildByName("Button_tip")
    self.btShow:addTouchEventListener(btcallback)
    self.btShow:setTag(TAG_ENUM.BT_SHOW)
    self.btShow:setVisible(false)


    --搓牌
    self.btCuo = self._csbNode:getChildByName("Button_ok")
    self.btCuo:addTouchEventListener(btcallback)
    self.btCuo:setTag(TAG_ENUM.BT_CUO)
    self.btCuo:setVisible(false)


    -- self.btShow.text = self.btShow:getChildByName("text")
    -- self.btCuo.text = self.btCuo:getChildByName("text")
   


    -- self.btAuto = self._csbNode:getChildByName("Button_8")
    -- self.btAuto:addTouchEventListener(btcallback)
    -- self.btAuto:setTag(TAG_ENUM.BT_AUTO)

    local betNode = self._csbNode:getChildByName("Node_10")
    
    betNode:setLocalZOrder(GameViewLayer.ZORDER_AUTO)

    -- local down =self.btAuto:getChildByName("Button_down")
    -- down:setTag(1)
    -- down:addTouchEventListener(function(ref, event)

	   --      if event == ccui.TouchEventType.ended then
	   --      		if ref:getTag() == 1 then
	   --      			betNode:setVisible(true)
	   --      			ref:setTag(2)
	   --      		else
	   --      			betNode:setVisible(false)
	   --      			ref:setTag(1)
	   --      		end
	   --      		--down:setRotation(180)
	   --      end
	    
    -- end)

    -- self.minBet = betNode:getChildByName("Button_min")
    -- self.minBet:addTouchEventListener(function(ref, event)

	   --      if event == ccui.TouchEventType.ended then
	        		
    --     		self.minBet:loadTextureNormal(cmd.RES_PATH.."game/btn_min2.png")
 	  --   		self.minBet:loadTexturePressed(cmd.RES_PATH.."game/btn_min2.png")

 	  --   		self.maxBet:loadTextureNormal(cmd.RES_PATH.."game/btn_max1.png")
 	  --   		self.maxBet:loadTexturePressed(cmd.RES_PATH.."game/btn_max1.png")

 	  --   		self.bIsMaxBet = false
 	  --   		betNode:setVisible(false)
 	  --   		down:setTag(1)
	   --      end
	    
    -- end)

    -- self.maxBet = betNode:getChildByName("Button_max")
    -- self.maxBet:addTouchEventListener(function(ref, event)

	   --      if event == ccui.TouchEventType.ended then
	        		
	   --      		self.minBet:loadTextureNormal(cmd.RES_PATH.."game/btn_min1.png")
 	  --   		self.minBet:loadTexturePressed(cmd.RES_PATH.."game/btn_min1.png")

 	  --   		self.maxBet:loadTextureNormal(cmd.RES_PATH.."game/btn_max2.png")
 	  --   		self.maxBet:loadTexturePressed(cmd.RES_PATH.."game/btn_max2.png")
 	  --   		self.bIsMaxBet = true
 	  --   		betNode:setVisible(false)
 	  --   		down:setTag(1)
	   --      end
	    
    -- end)

    	local btnVoice = self._csbNode:getChildByName("btn_voice")
	btnVoice:setName(VOICE_BTN_NAME)
	btnVoice:addTouchEventListener(function(ref, eventType)

		if self._scene.m_bLookOn  then
		            showToast(self._scene, "观战中不可使用该功能!", 2)
		            return 
		end

 		if eventType == ccui.TouchEventType.began then
			self._scene._scene:startVoiceRecord()
        elseif eventType == ccui.TouchEventType.ended 
        	or eventType == ccui.TouchEventType.canceled then
            self._scene._scene:stopVoiceRecord()
        end
	end)

	local btn = self._csbNode:getChildByName("Button_Auto")
    btn:addTouchEventListener(btcallback)
    btn:setTag(TAG_ENUM.BT_SHOWAUTO)
    self.btAuto = btn
	
	self._csbNode:getChildByName("Panel_trust"):getChildByName("btn_cancelTrust"):addTouchEventListener(function( ref, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(TAG_ENUM.BT_AUTO)
		end
	end)

	--self._scene.cbCardData = {}
	--self._scene.cbCardData[self._scene:GetMeChairID() +1] ={0x18,0x39,0x3d,0x0b,0x05}


	--self.cardLayer = CardLayer:create(self)

	--self:addChild(self.cardLayer,50)

	
	
end

function GameViewLayer:updateButton(isCuo,isVisible)
	if self._scene.m_bLookOn then 
		return 
	end
	self.btShow:setTag(isCuo == true and TAG_ENUM.BT_SHOW or TAG_ENUM.BT_TIPS)
	self.btShow:setVisible(isVisible)
	-- self.btShow.text:setString(isCuo == true and "亮牌" or "提示")

	self.btCuo:setTag(isCuo == true and TAG_ENUM.BT_CUO or TAG_ENUM.BT_CONFIRM)
	-- self.btCuo.text:setString(isCuo == true and "搓牌" or "摊牌")
	self.btCuo:setVisible(isVisible)
	if isCuo then
		-- self.btShow:setPosition(1065, 86)
		self.btCuo:setPositionY(143)

		self.btCuo:loadTextureNormal(cmd.RES_PATH.."game/btn_Ok.png")
    	self.btCuo:loadTexturePressed(cmd.RES_PATH.."game/btn_Ok.png")
	else

		-- self.btShow:setPosition(1101, 143)
		self.btCuo:setPositionY( 64)
		self.btCuo:loadTextureNormal(cmd.RES_PATH.."game/btn_Bg31.png")
    	self.btCuo:loadTexturePressed(cmd.RES_PATH.."game/btn_Bg31.png")
    	if isVisible then
    		self.btCuo:setVisible(false)
    	end
	    	
	end
end


function GameViewLayer:initUserInfo()
	local nodeName = 
	{
		"FileNode_1",
		"FileNode_2",
		"FileNode_3",
		"FileNode_4",
        "FileNode_5",
        "FileNode_6",
        "FileNode_7",
        "FileNode_8",
	}

	local faceNode = self._csbNode:getChildByName("Node_User")
	print("faceNode",faceNode)
	self.nodePlayer = {}
	for i = 1, cmd.GAME_PLAYER do
		self.nodePlayer[i] = faceNode:getChildByName(nodeName[i])
		self.nodePlayer[i]:setLocalZOrder(1)
		self.nodePlayer[i]:setVisible(true)
	end
end




function GameViewLayer:OnUpdateUserExit(userItem)
	
	
	if self._scene.isPriOver  and  GlobalUserItem.bPrivateRoom then
		return
	end

	for k,v in pairs(self.m_tabUserHead) do
		--dump(v.m_userItem,"=========================")
        if v and v.m_userItem and v.m_userItem.dwUserID == userItem.dwUserID then
	              
          	self:setReadyVisible(k, false)
			v:setVisible(false)
			print("OnUpdateUserExit========================"..userItem.szNickName,self.bActComplete)
			v= nil
		 
	    end
	end


end

--更新用户显示
function GameViewLayer:OnUpdateUser(viewId, userItem, bUpdate)
	if not viewId or viewId == yl.INVALID_CHAIR then
		print("OnUpdateUser viewId is nil")
		return
	end
	dump(userItem, "the user item is =====")
	
	if nil == userItem then
        		return
    	end

    --self.m_UserItem[viewId] = clone(userItem)
    self:showAdressLayer(false)

    local bReady = userItem.cbUserStatus == yl.US_READY
    print("更新用户显示 viewId bReady,bUpdate", viewId,bReady,bUpdate)

        self:setReadyVisible(viewId, bReady)
    	
        local lastHead = self.nodePlayer[viewId]:getChildByTag(666)
    if not bUpdate then
    	return
    end

        local playerInfo = PlayerInfo:create(userItem, viewId,self)

        if lastHead then
        	local zStatus1 = playerInfo.csbNode:getChildByName("banker_calling_icon")
        	local zStatus2 = lastHead.csbNode:getChildByName("banker_calling_icon")
        	zStatus1:setVisible(zStatus2:isVisible())

        	local bStatus1 = playerInfo.csbNode:getChildByName("banker_waitIcon")
        	local bStatus2 = lastHead.csbNode:getChildByName("banker_waitIcon")
        	bStatus1:setVisible(bStatus2:isVisible())
        	bStatus1:setPosition(bStatus2:getPosition())
        	playerInfo:setHeadStatus(lastHead:getHeadStatus())
        	lastHead:removeFromParent()
        end
        playerInfo:setTag(666)
        
        self.nodePlayer[viewId]:addChild(playerInfo)
        self.nodePlayer[viewId]:setVisible(true)

    
        self.m_tabUserHead[viewId] = playerInfo
        --self.m_tabUserHead[viewId].m_userItem = clone(userItem)

        self.m_tabUserHead[viewId]:updateStatus()
        if self.bIsAuto and viewId == cmd.MY_VIEWID then
        	self.m_tabUserHead[viewId]:setHeadStatus(cmd.US_Auto)
        end
       
        if self._scene.wBankerUser ~= yl.INVALID_CHAIR and viewId == self._scene:SwitchViewChairID(self._scene.wBankerUser)then
	       	self.m_tabUserHead[viewId]:showBank(true)
	       	if self._scene._cbCallBankerTimes then
	       		-- self.m_tabUserHead[viewId]:setMultiple(self._scene._cbCallBankerTimes[self._scene.wBankerUser+1])
	       		self:setBankerMultiple(viewId)
	       	end
	    end

    --判断房主
    if PriRoom and GlobalUserItem.bPrivateRoom then
        if userItem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID and self._scene.cbPlayMode == 0 then
            self.m_tabUserHead[viewId]:showRoomHolder(true)
        else
            self.m_tabUserHead[viewId]:showRoomHolder(false)
        end
    end
end

--****************************      计时器        *****************************--
function GameViewLayer:OnUpdataClockView(viewId, time)
	--print("计时器,viewId,time",viewId,time)
	if not viewId or viewId == yl.INVALID_CHAIR or not time then
		self._clockTime:setString("")
		self:setClockVisible(false)
	else
		-- self:setClockVisible(true)
		self._clockTime:setString(time)

		--勋章房
		if time <= 1 then
			-- print(GlobalUserItem.dwCurRoomGroupID, self._scene.m_cbGameStatus, PriRoom:getInstance().m_tabPriData.dwPlayCount,"------------超时等待-----------------")
			if PriRoom and GlobalUserItem.dwCurRoomGroupID ~= 0 and self._scene.m_cbGameStatus == cmd.GS_TK_FREE then
				if PriRoom:getInstance().m_tabPriData.dwPlayCount == 0 then
					self._scene:onExitTable()
				-- else
				-- 	--准备
				-- 	self:onButtonClickedEvent(TAG_ENUM.BT_START,nil);
				end
			end
		end
		
	end
end

-- function GameViewLayer:setClockPosition(viewId)
-- 	if viewId then
-- 		self._clockTimeBg:move(pointClock[viewId])
-- 	else
-- 		self._clockTimeBg:move(display.cx, display.cy + 50)
-- 	end
--     self._clockTime:setVisible(true)
-- end

--**************************      点击事件        ****************************--
--点击事件
function GameViewLayer:onEventTouchCallback(eventType, x, y)
	if eventType == "began" then
		print("btgin eventType,x,y",eventType,x,y)
		-- if self.bBtnInOutside then
		-- 	self:onButtonSwitchAnimate()
		-- 	return false
		-- end
	elseif eventType == "ended" then
		print("begin eventType,x,y",eventType,x,y)
		--用于触发手牌
		if self.bCanMoveCard ~= true then
			return false
		end
		local nodeCard = self._csbNode:getChildByName("Node_Card")
	    local panelCard = nodeCard:getChildByName(string.format("Node_%d",cmd.MY_VIEWID))
		-- local size1 = self.nodeCard[cmd.MY_VIEWID][1]:getTextureRect()   --getContentSize()
		-- local pos1 = cc.p(self.nodeCard[cmd.MY_VIEWID][1]:getPositionX(),self.nodeCard[cmd.MY_VIEWID][1]:getPositionY())
		-- pos1 = panelCard:convertToWorldSpace(pos1) 
		for i = 1, 5 do
			local card = self.nodeCard[cmd.MY_VIEWID][i]
			local pos2 = cc.p(card:getPositionX(),card:getPositionY())
			local pos2World = panelCard:convertToWorldSpace(pos2)  
			local size2 = card:getTextureRect()   --getContentSize()
			local rect = card:getTextureRect()
			local pos = cc.p(pos2World.x - size2.width/2, pos2World.y - size2.height/2)
			rect.x = pos.x  --x1 - size1.width/2 + x2 - size2.width/2
			rect.y = pos.y  --y1 - size1.height/2 + y2 - size2.height/2
			--dump(rect,"点击事件",5)
			if cc.rectContainsPoint(rect, cc.p(x, y)) then
				if false == self.bCardOut[i] then
					--检测是否有三个牌了
					local selcetNum = 0
					for j=1,#self.bCardOut do
						if true == self.bCardOut[j] then
							selcetNum = selcetNum + 1
						end
					end
					if selcetNum >= 3 then
						print("已经有三张牌弹出")
						card:runAction(cc.Sequence:create(cc.MoveBy:create(0.1,cc.p(0,10)),cc.MoveBy:create(0.1,cc.p(0,-10))))
						return
					else
						card:runAction(cc.MoveTo:create(0.2,cc.p(pos2.x, pos2.y + 30)))
					end
					--card:move(pos2.x, pos2.y + 30)
					--self.cardFrame[i]:setSelected(true)
				elseif true == self.bCardOut[i] then
					--card:move(pos2.x, pos2.y - 30)
					card:runAction(cc.MoveTo:create(0.25,cc.p(pos2.x, pos2.y - 30)))
					--self.cardFrame[i]:setSelected(false)
				end
				self.bCardOut[i] = not self.bCardOut[i]
				self:updateCardPrompt()
				return true
			end
		end
	end
	return true
end


function moveCardEnd()
	
            local eventListener = cc.EventCustom:new("moveCardEnd")
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

function GameViewLayer:showCuoLayer()
	if self.CuoLayer then return end
	self.CuoLayer = cc.CSLoader:createNode(cmd.RES_PATH.."game/Layer.csb"):addTo(self, 50)
	local myCardData = clone(self._scene:getMeCardValue())

	local ardValue = yl.POKER_VALUE[myCardData[5]] 
    	local cardColor = yl.CARD_COLOR[myCardData[5]]  

	local cardLyaer = PageTurn:create(cmd.RES_PATH.."game/bigCard/cardBack.png",cmd.RES_PATH.."game/bigCard/card_"..cardColor.."_"..ardValue,cc.p(667,395))
	local panel = self.CuoLayer:getChildByName("Panel_1")

	-- for i=1,4 do

	-- 	local card =CardSprite:createCard(myCardData[i])
	-- 	card:setPosition(457+(i-1)*140, 620)
	-- 	panel:addChild(card)

	-- end

	-- local closeBtn = self.CuoLayer:getChildByName("Panel_1"):getChildByName("Button_13")
	-- closeBtn:addTouchEventListener(function (ref, eventType)

	-- 	if eventType == ccui.TouchEventType.ended then
	-- 	    self:closeCuoLayer()
	-- 	end
	-- end)
	local bfirst = cc.UserDefault:getInstance():getBoolForKey("oxFirstPlay")
	print("****************************bfirst",bfirst)
	if bfirst then
		panel:setVisible(false)
	else
		cc.UserDefault:getInstance():setBoolForKey("oxFirstPlay", true)
		panel:setVisible(true)
	end

	self.CuoLayer:addChild(cardLyaer,10,10)
	panel:setLocalZOrder(11)

	local touchLayer = cc.Layer:create()
	self.CuoLayer:addChild(touchLayer,20,20)

	local finger = cc.Sprite:create(cmd.RES_PATH.."game/bigCard/finger.png"):move(867,125):addTo(panel,11)
	-- finger:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.MoveTo:create(1.2,cc.p(667,400)),cc.MoveTo:create(0.01,cc.p(367,200)),
	-- 	cc.MoveTo:create(1.2,cc.p(867,200)),cc.MoveTo:create(0.01,cc.p(667,100)))))

	local dispatcher = touchLayer:getEventDispatcher()
      	local listener = cc.EventListenerTouchOneByOne:create()
      		listener:setSwallowTouches(true)
      		--listener:setTouchEnabled(true)
      		listener:registerScriptHandler(function ( touches, event )
		      	local point = touches:getLocation()
		      
		      	

		      		panel:setVisible(false)

		      	if point.x > 378 and point.x < 952 and point.y> 91 and point.y <482 then
		      		-- if finger then
		      		-- 	finger:removeFromParent()
		      		-- 	finger = nil
		      		-- end
		      		panel:setVisible(false)
		      	end
		      	
		      
		          return false
          end, cc.Handler.EVENT_TOUCH_BEGAN)

        dispatcher:addEventListenerWithSceneGraphPriority(listener, touchLayer)

	--监听
	    local function eventCall( event )
	             self:cuoActionEnd(true)
	    end


	self.m_listener = cc.EventListenerCustom:create("moveCardEnd",handler(self, eventCall))
    	cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end


function GameViewLayer:cuoActionEnd(isDelay)
	if self.btShow:isVisible() and self.btShow:getTag() == TAG_ENUM.BT_SHOW then

		self:onButtonClickedEvent(TAG_ENUM.BT_SHOW,nil)
	end
	local delayTime = 0.01
	if isDelay then
		delayTime = 1.5
	end
	self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ()
			self:closeCuoLayer()
		end)))
end

function GameViewLayer:closeCuoLayer()
	print("**********删除搓牌")
	if self.CuoLayer then
		self.CuoLayer:removeFromParent()
		self.CuoLayer = nil
	end
	if nil ~= self.m_listener then
	        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
	        self.m_listener = nil
	end
end


function GameViewLayer:showAdressLayer(isShow)

	local addressLayer = cc.CSLoader:createNode(cmd.RES_PATH.."game/LocationLayer.csb"):addTo(self, 51)
	local node = addressLayer:getChildByName("Sprite_1")
	self._scene:showPopWait()
	addressLayer:setVisible(isShow)

	local action = cc.Sequence:create(cc.FadeOut:create(0.5), cc.FadeIn:create(0.5))
	local repaction = cc.RepeatForever:create(action)
	node:getChildByName("tips_2"):runAction(repaction)

	local btn = node:getChildByName("Button_close")
	btn:addTouchEventListener(function(sender,eventType)
	if eventType == ccui.TouchEventType.ended  then
		    addressLayer:removeFromParent()
		end 
	end)
	addressLayer:getChildByName("Panel_2"):addTouchEventListener(function(sender,eventType)
		if eventType == ccui.TouchEventType.ended  then
		    addressLayer:removeFromParent()
		end 
	end)


	for i=1,cmd.GAME_PLAYER do
		if self.m_tabUserHead[i]  then
			self.m_tabUserHead[i].m_popHead:queryUserLociton(self.m_tabUserHead[i].m_userItem.dwUserID)
		end
	end

	
  self:runAction(cc.Sequence:create(cc.DelayTime:create(0.6),cc.CallFunc:create(function ()
    self._scene:dismissPopWait()
    
    self.m_tipsTable = {}
    for i=1,cmd.GAME_PLAYER do
      -- local seat = self._bottomNode:getChildByName(string.format("seat_%d", i))
      -- local headbg = seat:getChildByName("head_bg")
      -- local head = headbg:getChildByTag(1)
      if self.m_tabUserHead[i] then
        local userInfo = self.m_tabUserHead[i].m_popHead.m_userInfo
        -- dump(userInfo , "1111")
        local la1  = 0
        local lo1  = 0 
        if userInfo then
          la1  = userInfo.dLatitude
          lo1 = userInfo.dLongitude
        end
        
        for j=i + 1,cmd.GAME_PLAYER do
          local la2 = 0
          local lo2 = 0
          local userInfo2
	      if self.m_tabUserHead[j] then
	        userInfo2 = self.m_tabUserHead[j].m_popHead.m_userInfo
	        -- dump(userInfo2,"222222")
            if userInfo2 then
              la2  = userInfo2.dLatitude
              lo2 = userInfo2.dLongitude
            end
          end
          if la1 ~= 0 and lo1 ~= 0 and la2 ~= 0 and lo2 ~= 0  then
			local dst = self:algorithm(lo1 , la1 , lo2 , la2)
            -- local function GetRadian(d)
            --   return d*3.1415926/180
            -- end

            -- local function GetRound(number)
            --   return number>0 and math.floor(number + 0.5) or math.ceil(number - 0.5)
            -- end

            -- local  radLat1 = GetRadian(la1)
            -- local  radLat2 = GetRadian(la2)
            -- local  a = radLat1 - radLat2
            -- local  b = GetRadian(lo1) - GetRadian(lo2)

            -- local dst = 2 * math.asin((math.sqrt(math.pow(math.sin(a / 2), 2) + math.cos(radLat1) * math.cos(radLat2) * math.pow(math.sin(b / 2), 2) )))

            -- dst = dst * 6371.393

            -- dst= math.abs(GetRound(dst * 10000000) / 10000)

            print("dst ====",dst)
            if dst <=100 and userInfo2 and userInfo then
                local nameT = {}
                nameT[1] = userInfo.szNickName
                nameT[2] = userInfo2.szNickName
                table.insert(self.m_tipsTable , nameT) 
            end
          end
        end
      end
    end
	-- dump(self.m_tipsTable)
	if #self.m_tipsTable == 0 then
		node:getChildByName("tips_2"):setVisible(false)
		local lab = cc.LabelTTF:create("检测结果无异常","fonts/round_body.ttf",36)
		lab:setPosition(375,262)
		lab:setColor(cc.c3b(255,224,128))
		lab:addTo(node)
		self.btAddress:removeChildByTag(163)

      local ani1 = ExternalFun.loadCSB("game/locationAnim.csb", self.btAddress)
      ani1:setTag(163)
      ani1:setPosition(-5,-3)

      local aniAction = ExternalFun.loadTimeLine("game/locationAnim.csb")
      aniAction:gotoFrameAndPlay(0,25,true)
      ani1:runAction(aniAction)
    else
      self.btAddress:removeChildByTag(163)

      local ani1 = ExternalFun.loadCSB("game/locationAnim_red.csb", self.btAddress)
      ani1:setTag(163)
      ani1:setPosition(-5,-3)

      local aniAction = ExternalFun.loadTimeLine("game/locationAnim_red.csb")
      aniAction:gotoFrameAndPlay(0,25,true)
      ani1:runAction(aniAction)
	end
    local tmpcontent = node:getChildByName("Panel_1")
    local contentsize = tmpcontent:getContentSize()
    local _listView = cc.TableView:create(contentsize)
    _listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    _listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    _listView:setPosition(tmpcontent:getPosition())
    _listView:setPosition(22,16)
    _listView:setDelegate()
    _listView:addTo(node)
    _listView:registerScriptHandler(handler(self, self.cellSizeForTable2), cc.TABLECELL_SIZE_FOR_INDEX)
    _listView:registerScriptHandler(handler(self, self.tableCellAtIndex2), cc.TABLECELL_SIZE_AT_INDEX)
    _listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView2), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    _listView:reloadData()
    tmpcontent:removeFromParent()
  end)))
end
function GameViewLayer:algorithm(longitude1, latitude1, longitude2, latitude2)
    local function rad(d)
        return d*math.pi/180
    end

    local function round(n)
        return math.floor(n+0.5)
    end

    local a = rad(latitude1) - rad(latitude2)
    local b = rad(longitude1) - rad(longitude2)
    local s = 2 * math.asin(math.sqrt(math.pow(math.sin(a / 2), 2) +
        math.cos(latitude1) * math.cos(latitude2) * math.pow(math.sin(b / 2), 2)))

    s = s * 6371.393

    s = round(s * 10000) / 10000
    return s
end
-- 子视图大小
function GameViewLayer:cellSizeForTable2(view, idx)
    return 480, 50
end

function GameViewLayer:tableCellAtIndex2(view, idx)
	print("11111111111111111111111")
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local iteminfo = self.m_tipsTable[idx + 1]
    if nil == iteminfo then
        return cell 
    end

	print("22222222222222222222222222222")
    local csb = cc.CSLoader:createNode("game/locationCell.csb")
    cell:addChild(csb)
    csb:setPosition(view:getViewSize().width * 0.5, 61 * 0.5)

    csb:getChildByName("Name_1"):setString(iteminfo[1])
    csb:getChildByName("Name_2"):setString(iteminfo[2])

    
    return cell
end

-- 子视图数目
function GameViewLayer:numberOfCellsInTableView2(view)
    return #self.m_tipsTable
end



function GameViewLayer:showSitBtn(visible,time)
	self.m_beginTotalTime = time
	if self._scene.m_bLookOn then 
		self.btSitAndBegin:setTag(TAG_ENUM.BT_SIT)
		self.btSitAndBegin:getChildByName("sprite"):setTexture("game/sit.png"):setPositionX(130)
		self.btSitAndBegin:getChildByName("Text_1"):setString("")
		self.btSitAndBegin:setPositionY(240)
	else
		self.btSitAndBegin:getChildByName("sprite"):setTexture("game/begin.png"):setPositionX(110)
		self.btSitAndBegin:getChildByName("Text_1"):setString("开始游戏")
		self.btSitAndBegin:setTag(TAG_ENUM.BT_BEDIN)
		self.btSitAndBegin:setPositionY(230)
		if visible then
			if not self._beginTimeId and time then
		                 self.m_beginTimeNum = time
		                 self.btSitAndBegin:getChildByName("Text_1"):setString("("..self.m_beginTimeNum..")")
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
	self.btSitAndBegin:setVisible(visible)
end


function GameViewLayer:updateBeginTime()
	if self.m_beginTimeNum and self.m_beginTimeNum >0 then 
		self.m_beginTimeNum = self.m_beginTimeNum -1
		self.btSitAndBegin:getChildByName("Text_1"):setString("("..self.m_beginTimeNum..")")	

		if self.bIsAuto and self.m_beginTotalTime - self.m_beginTimeNum == 5 then
			self:onButtonClickedEvent(TAG_ENUM.BT_BEDIN)
		end
	else
		if self._beginTimeId then
			cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
			self._beginTimeId = nil
		end
	end

end


function GameViewLayer:setBtnchangeShow(bshow,bstart)
	print(bshow,bstart,debug.traceback())
	if GlobalUserItem.bPrivateRoom then
		self.Btn_changeDesk:setVisible(false)
		self.btStart:setVisible(bstart)
		self.btStart:setPositionX(667)
	else
		self.Btn_changeDesk:setVisible(bshow)
		self.btStart:setVisible(bstart)
		if bstart then
			self.Btn_changeDesk:setPositionX(520)
		else
			self.Btn_changeDesk:setPositionX(667)
		end
	end
end

function GameViewLayer:setStartBtnShow(bshow)
	self.btStart:setVisible(bshow)
	if GlobalUserItem.bPrivateRoom then
		self.Btn_changeDesk:setVisible(false)
		self.btStart:setPositionX(667)
	else
		if bshow then
			self.btStart:setPositionX(814)
			self.Btn_changeDesk:setPositionX(520)
		end
	end
end

--按钮点击事件
function GameViewLayer:onButtonClickedEvent(tag,ref)
	if self._scene.isPriOver then
		return 
	end
	AudioEngine.playEffect("sound_res/btn.mp3")
	if tag == TAG_ENUM.BT_START then   --准备
		-- self:setStartBtnShow(false)
		self:setBtnchangeShow(false,false)
		if self._scene then 
			self._scene:onStartGame()
			self._scene.cbPlayStatus[self._scene:GetMeChairID()+1] = 1
		end
	elseif tag == TAG_ENUM.BT_SIT then   --坐下

		self._scene._gameFrame.bChangeDesk = true
		self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)


	elseif tag == TAG_ENUM.BT_BEDIN then   --房主开始游戏	
		self.btSitAndBegin:setVisible(false)
		self._scene._gameFrame:SendUserBegin()

	elseif tag == TAG_ENUM.BT_CHANGEDESK then  --换桌
		self._menu:setVisible(false)
		if self._scene.m_cbGameStatus > 0 and self._scene.cbPlayStatus[self._scene:GetMeChairID()+1] == 1 then
			showToast(self, "游戏开始后不能换桌", 2)
			return
		end

		for i=1,cmd.GAME_PLAYER do
			if self.m_tabUserHead[i] then
				
				self:setReadyVisible(i, false)
				self.m_tabUserHead[i]:removeFromParent()
				self.m_tabUserHead[i]= nil
			end
		end
		
		self:onResetView() 	
		-- self.btStart:setVisible(true)
		self.Btn_changeDesk:setVisible(true)

		ref:setEnabled(false)
		ref:runAction(cc.Sequence:create(cc.DelayTime:create(1),cc.CallFunc:create(function ( ... )
			ref:setEnabled(true)
		end)))

		self._scene._gameFrame.bChangeDesk = true

		self._scene:onChangeDesk()

	elseif tag == TAG_ENUM.BT_SWITCH then  --菜单
		self:onButtonSwitchAnimate()
		if self._scene.m_cbGameStatus > 0 and self._scene.cbPlayStatus[self._scene:GetMeChairID()+1] == 1 then
			self.btChangedesk:setEnabled(false)
		end
	elseif tag == TAG_ENUM.BT_CHAT then
		if self._scene.m_bLookOn  then
		            showToast(self._scene, "观战中不可使用该功能!", 2)
		            return 
		end

		self._menu:setVisible(false)
		
	           if nil == self.m_chatLayer then
	               self.m_chatLayer = GameChatLayer:create(self._scene._gameFrame)
	               self:addChild(self.m_chatLayer, GameViewLayer.ZORDER_CHAT, "_chatLayer")
	          end
	          self.m_chatLayer:showGameChat(true)

		-- if not self._chatLayer then
		-- 	--聊天，调用本地资源，不用通用
		-- 	local tabconfig = {}
		-- 	tabconfig.csbfile = cmd.RES_PATH.."chat/GameChatLayer.csb"
		-- 	--图片要加入缓存
		-- 	local sprbg = cc.Sprite:create(cmd.RES_PATH.."chat/chat_cell.png")
		-- 	if sprbg then
		-- 		cc.SpriteFrameCache:getInstance():addSpriteFrame(sprbg:getSpriteFrame(), "chat_cell.png")
		-- 		tabconfig.szItemFrameName = "chat_cell.png"
		-- 	end
		
		-- 	self._chatLayer = GameChatLayer:create(self._scene._gameFrame, tabconfig):addTo(self, GameViewLayer.ZORDER_CHAT)	--聊天框
			
		-- end
    		
		-- self._chatLayer:showGameChat(true)
	elseif tag == TAG_ENUM.BT_SET then
		print("设置")
		self._menu:setVisible(false)
		self._setLayer:showLayer(true)
	elseif tag == TAG_ENUM.BT_HELP then
		print("玩法")
		self._menu:setVisible(false)
		self:showHelp()
	elseif tag == TAG_ENUM.BT_EXIT then
		self._menu:setVisible(false)
		self._scene:onQueryExitGame()

	elseif tag == TAG_ENUM.BT_ADDRESS then
		if self._scene.m_bLookOn  then
		            showToast(self._scene, "观战中不可使用该功能!", 2)
		            return 
		end
		self:showAdressLayer(true)

	elseif tag == TAG_ENUM.BT_CONFIRM then
		self:onButtonConfirm(cmd.MY_VIEWID)
		self._scene:onOpenCard(self.cbCombineCard)
		dump(self.cbCombineCard, "the orignal cards")
		self.nAutoStatus = cmd.AUTO_PLAY_FREE
		self._scene:stopAllActions()

	elseif tag == TAG_ENUM.BT_PROMPT then
		self:promptOx()
	elseif tag == TAG_ENUM.BT_CALLBANKER then
		for i = 1, #self.btMul do
			self.btMul[i]:setVisible(false)
		end
		self:showCallBankerMul()
	elseif tag == TAG_ENUM.BT_CANCEL then
		for i = 1, #self.btMul do

			if self.btMul[i] then
				self.btMul[i]:setVisible(false)
			end
		end
		self.nodeCall:setVisible(false)
		self._scene:onBanker(false,0)
		self.bLastCall = TAG_ENUM.BT_CANCEL

		self.nAutoStatus = cmd.AUTO_PLAY_FREE
		self._scene:stopAllActions()

	elseif tag == TAG_ENUM.BT_CALL then
		
		self.nodeCall:setVisible(false)
		self._scene:onBanker(true,1)

		self.bLastCall = TAG_ENUM.BT_CALL
		self.nAutoStatus = cmd.AUTO_PLAY_FREE
		self._scene:stopAllActions()
		-- self:playEffect("rober_bank_",GlobalUserItem.tabAccountInfo.cbGender)

	elseif tag == TAG_ENUM.BT_TIPS then
		print("TAG_ENUM.BT_TIPS==============")
		self:promptOx()

	elseif tag == TAG_ENUM.BT_SHOW then
		if self._scene:isPlayerPlaying(cmd.MY_VIEWID) then
			self._bShowCard = true
		      self:setSpecialInfo(self._scene.m_SpecialType,self._scene.m_initType)
		      

		      self:showCalculate(true,true)
		      
		     if self._scene.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR  and  
		      self._scene.m_tabPrivateRoomConfig.bankGameType ~=  cmd.BANERGAMETYPE_CONFIG.BGT_TONGBI then
		     	self:openCardAnimate(cmd.MY_VIEWID,5)
		     else
		      	self:openCardAnimate(cmd.MY_VIEWID,nil,false)
		     end
		end

	elseif tag == TAG_ENUM.BT_CUO then

		if self._scene.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR then
			self:showCuoLayer()
		else
			self.CuoLayer = CardLayer:create(self)

			self:addChild(self.CuoLayer,50)
		end


	elseif  tag == TAG_ENUM.BT_AUTO then
		if self._scene.m_bLookOn  then
		            showToast(self._scene, "观战中不可使用该功能!", 2)
		            return 
		end
		print("==========1")
		self.bIsAuto = not self.bIsAuto 

		-- self.btAuto:loadTextureNormal(cmd.RES_PATH.."game/auto"..(self.bIsAuto==true and 2 or 1)..".png")
  --   	self.btAuto:loadTexturePressed(cmd.RES_PATH.."game/auto"..(self.bIsAuto==true and 2 or 1)..".png")
    	dump({self.bIsAuto,self.nAutoStatus},"===================2")
		if self.bIsAuto and self._scene.cbDynamicJoin == 0 then
			self:autoPlay(self.nAutoStatus)
		end
		self._csbNode:getChildByName("Panel_trust"):setVisible(self.bIsAuto)

		if self.bIsAuto then
			self.m_tabUserHead[cmd.MY_VIEWID]:setHeadStatus(cmd.US_Auto)
		else
			self.m_tabUserHead[cmd.MY_VIEWID]:setHeadStatus(cmd.US_Normal)
		end

	elseif tag - TAG_ENUM.BT_CHIP1 == 0 or tag - TAG_ENUM.BT_CHIP1 == 1 or tag - TAG_ENUM.BT_CHIP1 == 2 or tag - TAG_ENUM.BT_CHIP1 == 3 or tag - TAG_ENUM.BT_CHIP1 == 4  then
		-- self.nodeChip:removeAllChildren()
		local index = tag - TAG_ENUM.BT_CHIP1+1
		dump(self.lUserJettonScore,"lUserJettonScore")
		print("index",index)
		print("score is .....====================",self.lUserJettonScore[index])

		--if self.lUserJettonScore[index] > self._scene.m_lMaxTurnScore then
			--self.lUserJettonScore[index] = self._scene.m_lMaxTurnScore
		--end
		self:getParentNode():onAddScore(self.lUserJettonScore[index])

		self.nAutoStatus = cmd.AUTO_PLAY_FREE
		self._scene:stopAllActions()
	elseif tag - TAG_ENUM.BT_MUL1 == 0 or tag - TAG_ENUM.BT_MUL1 == 1 or tag - TAG_ENUM.BT_MUL1 == 2 or tag - TAG_ENUM.BT_MUL1 == 3 or tag - TAG_ENUM.BT_MUL1 == 4 then
		for i = 1, #self.btMul do
			self.btMul[i]:setVisible(false)
		end
		local index = tag - TAG_ENUM.BT_MUL1+1

		print("index",index)
		self._scene:onBanker(true,index)
 		-- self:playEffect("rober_bank_",GlobalUserItem.tabAccountInfo.cbGender)

		self.bLastCall = tag
		self.nAutoStatus = cmd.AUTO_PLAY_FREE
		self._scene:stopAllActions()

	elseif tag == TAG_ENUM.BT_INVITE then
        PriRoom:getInstance():queryRoomPasswd(
            PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, 
            function(cmd_table)
                -- 第三方分享
                local scene = PriRoom:getInstance():getPlazaScene()
                if nil == scene then
                    return
                end
                local dwRoomDwd = cmd_table.dwRoomDwd or 0

                local function _shareFun( dwRoomDwd )
                    if nil ~= scene.popTargetShare then
                        scene:popTargetShare(function(target)
                            if nil ~= target then
                                local function sharecall( isok )
                                    if type(isok) == "string" and isok == "true" then
                                        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
                                            cc.CallFunc:create(function()
                                                showToast(self, "分享成功", 2)
                                        end)))
                                    end
                                end
                                local passwd = dwRoomDwd
                                local dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                local drawCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0

                                local cardType = {"经典模式","疯狂加倍"}
                                local sendType = {"发四等五","下注发牌"}
                                local bankerType = {"霸王庄","明牌抢庄","牛牛上庄","无牛下庄","自由抢庄","通比玩法"}
                                local config = self._scene:getPrivateRoomConfig()
                                local chairCountstr = self._scene.wPlayerCountRule == 0 and "2-8人" or string.format("%d人", self._scene.wPlayerCountRule)
                                local configStr = string.format("%s,%d局,", chairCountstr,PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)
                                configStr = configStr..cardType[config.cardType-22+1]..","..sendType[config.sendCardType-32+1]..","..bankerType[config.bankGameType-52+1]

                                local shareTxt = configStr .. "。新八人牛牛精彩刺激, 一起来玩吧!"
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=57&a=1" .. "&p=" .. passwd

                                MultiPlatform:getInstance():shareToTarget(
                                    target, 
                                    sharecall, 
                                    string.format("【约战房间: %06d】", dwRoomID), 
                                    shareTxt, 
                                    url, 
                                    "")
                            end
                        end, 3)
                    end
                end
                
                if 0 ~= dwRoomDwd then
                    local query = QueryDialog:create("约战密码为 " .. dwRoomDwd .. " 是否邀请好友游戏?", function(ok)
                        if ok then
                            _shareFun(dwRoomDwd)
                        end
                    end)
                    :setCanTouchOutside(false)
                    :addTo(self)
                else
                    _shareFun( dwRoomDwd )
                end
            end
        )
	elseif  tag == TAG_ENUM.BT_SHOWAUTO then
		if self._scene.m_bLookOn  then
            showToast(self._scene, "观战中不可使用该功能!", 2)
            return 
		end
		if not self.trustLayer then
			self.trustLayer = TrustLayer:create(self):addTo(self, GameViewLayer.ZORDER_TRUST)
		else
			self.trustLayer:showLayer(self.bIsAuto)
		end
	else
		print("tag",tag)
		showToast(self,"功能尚未开放！",1)
	end
end

function GameViewLayer:autoPlay(stataus)
	if self._scene.m_bLookOn then
		return
	end

	if stataus == cmd.AUTO_PLAY_FREE then

	elseif stataus == cmd.AUTO_PLAY_CALL then

		-- self:onButtonClickedEvent(self.bLastCall)
		if not self.trustLayer then
			self:onButtonClickedEvent(self.bLastCall)
			return
		end

		dump(self.trustLayer._callSelect,"self.trustLayer._callSelect[")
		if self.trustLayer._callSelect[1] == 1 then
			self:onButtonClickedEvent(TAG_ENUM.BT_CANCEL)
		else
			local tag = self.trustLayer._callSelect[2]
			print("******************tag******************************")
			print(tag)
			print("******************tag******************************")
			if tag == 1 then
				self:onButtonClickedEvent(TAG_ENUM.BT_MUL1)
			else
        		local cardtype = GameLogic:analyseCardType(self._scene.cbCardData[self._scene:GetMeChairID() + 1])
        		if tag - 1 <= cardtype - GameLogic.CT_OX_VALUE7 then
					self:onButtonClickedEvent(TAG_ENUM.BT_MUL1 + tag -1)
				else
					self:onButtonClickedEvent(TAG_ENUM.BT_CANCEL)
				end
			end
		end



	elseif stataus == cmd.AUTO_PLAY_BET then
		print("自动下注")
		if self._scene.wBankerUser ~=  self._scene:GetMeChairID() then
			local index = ((self.bIsMaxBet == false) and 1 or #self.btChip)
			if not self.lUserJettonScore[index] then
				index = 1
			end

			if self.trustLayer then
				if self.trustLayer._betSelect[1] == 1 then
					index = 1
				elseif self.trustLayer._betSelect[1] == 2 then
					index = #self.btChip
				else
					local tag = self.trustLayer._betSelect[2]
	        		local cardtype = GameLogic:analyseCardType(self._scene.cbCardData[self._scene:GetMeChairID() + 1])
	        		if tag <= cardtype - GameLogic.CT_OX_VALUE7 then
						index = #self.btChip
					else
						index = 1
					end
				end
			end
			self:onButtonClickedEvent(TAG_ENUM.BT_CHIP1 + index -1)
		end

	elseif stataus == cmd.AUTO_PLAY_OPENCARD then
		print("=================AUTO_PLAY_OPENCARD")

		for i=1,cmd.MAX_CARDCOUNT do
			local card = self.nodeCard[cmd.MY_VIEWID][i]
			self._scene:openOneCard(cmd.MY_VIEWID,i,false)

		end
		self:promptOx()
		self:onButtonClickedEvent(TAG_ENUM.BT_CONFIRM,nil)
	elseif stataus == cmd.AUTO_PLAY_START then
		if self.btStart:isVisible() then
			self:onButtonClickedEvent(TAG_ENUM.BT_START,nil)
		end
	end


end

function GameViewLayer:showHelp()
	local helpLayer = cc.CSLoader:createNode(cmd.RES_PATH.."help_res/IntroduceLayer.csb"):addTo(self, 51)

	local imgBg = helpLayer:getChildByName("image_bg")

	local panel = imgBg:getChildByName("content")
	
	-- local viewSize = panel:getContentSize()
	-- local scrollView = ccui.ScrollView:create()
 --                          :setContentSize(viewSize)
 --                          :setPosition(panel:getPosition())
 --                          :setAnchorPoint(panel:getAnchorPoint())
 --                          :setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
 --                          :setBounceEnabled(true)
 --                          :setScrollBarEnabled(false)
 --                          :addTo(imgBg)
          	--local str ="普通牌型\n    游戏准备过后，系统会先发4张牌发给每个准备的玩家，然后玩家开始进行下注，玩家下注过后，会获得第五张牌，牌局开始每个人手中都会有五张牌，玩家需要将牌型组成3张+2张的形式，3张牌的总和必须为10或者10的倍数（每张牌按照点数进行计算，比如A代表1点，2带表2点，10，J，Q，K都代表10点，王牌为万能牌可以代表任意点数，如果王在牌型中进行比对的话，相同牌型的情况下带有王的牌型是最小的），这样是被称之为“牛”，若没有按照此类型组成则称之为“无牛”。接下来在将其余的两张的点数相加得出的点数取个位数进行比较，牌型大的获胜。\n    牛 --- 摆牌过后，3张牌无法构成10或者10的倍数，所以整体牌型称之为“无牛”\n    牛一 ~ 牛九 --- 摆牌过后，所剩个位数为1 ~ 9的牌型\n    牛牛 --- 摆牌过后，牌型定格为10的倍数，也就是个位数是0的牌型”\n特殊牌型\n    游戏中出现的特殊牌型不会按照普通牌型进行比牌， 所有的特殊牌型都比普通牌型最大的牌型（牛牛）在最后比牌时要大。当组成的牌型重叠时（玩家牌型中即包含“四花牛”又包括“葫芦”），则摆拍显示牌型默认显示最大牌型即可。\n    四花牛 --- 四张牌为J，Q，K中的任意一个。第5张牌为10的牌型\n    五花牛 --- 五张牌都为J，Q，K的牌型\n    顺子 --- 五张牌为连牌，例如3，4，5，6，7这种牌型。\n    同花 --- 五张牌花色相同，例如梅花3，梅花6，梅花9，梅花10，梅花J组成的牌型。\n    葫芦 --- 三张相同点数的牌加上两张相同点数的牌，例如 梅花3，方块3，红桃3，黑桃2，红桃2，组成的牌型\n    炸弹 --- 由四张相同点数的牌加上任意一张牌组成的牌型，例如 梅花3，方块3，红桃3，黑桃3,红桃2，组成的牌型\n    同花顺 --- 五张牌皆为同花色连牌，例如，梅花3，梅花4，梅花5，梅花6，梅花7组成的牌型\n    五小牛 --- 五张牌用加法计算总和≤10的牌型。\n牌型大小\n    数字比较 --- K>Q>J>10>9>8>7>6>5>4>3>2>A\n    花色比较 --- 黑桃>红桃>梅花>方块\n    牌型比较 --- 无牛<有牛<牛牛<四花牛<五花牛<顺子<同花<葫芦<炸弹<同花顺<五小牛\n    无牛牌型比较 --- 取其中最大的一张牌比较大小，牌大的赢，大小相同比花色。\n    有牛牌型比较 --- 比牛数；牛数相同庄吃闲。\n    牛牛牌型比较 --- 取其中最大的一张牌比较大小，牌大的赢，大小相同比花色。\n    四花牛牌型比较 --- 取其中最大的一张牌比较大小，牌大的赢，大小相同比最大牌的花色。\n    五花牛牌型比较 --- 取其中最大的一张牌比较大小，牌大的赢，大小相同比最大牌的花色。\n    顺子牌型比较 --- 取其中最大的一张牌比较大小，牌大的赢，大小相同比最大牌的花色。\n    同花牌型比较 --- 取其中最大的一张牌比较大小，牌大的赢，大小相同比花色。\n    葫芦牌型比较 --- 取葫芦牌比较大小。\n    炸弹牌型比较 --- 取炸弹牌比较大小。\n    同花顺牌型比较 --- 取其中最大的一张牌比较大小，牌大的赢，大小相同比最大牌的花色。\n    五小牛牌型比较 --- 庄吃闲。"

     --       local strLabel = cc.Label:createWithTTF(str, "fonts/round_body.ttf", 25)
     --                         :setLineBreakWithoutSpace(true)
     --                         :setMaxLineWidth(viewSize.width)
     --                         :setTextColor(cc.c4b(149,149,149,255))
     --                         :setAnchorPoint(cc.p(0.5, 1.0))
     --                         :addTo(scrollView)

     --       local labelSize = strLabel:getContentSize()
    	-- local fHeight = labelSize.height > viewSize.height and labelSize.height or viewSize.height
    	-- strLabel:setPosition(cc.p(viewSize.width * 0.5, fHeight))
    	-- scrollView:setInnerContainerSize(cc.size(760, 1530))
    	-- local alltype ={{card ={0x01,0x08,0x19,0x27,0x36},path1 ="help_res/bei1.png",path2 ="help_res/niu0.png",txt ="摆过牌后，任意3张牌无法构成10或者\n10的倍数，所以整体牌型称之为“无牛”"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x31},path1 ="help_res/bei1.png",path2 ="help_res/niu1.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为1"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x32},path1 ="help_res/bei1.png",path2 ="help_res/niu2.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为2"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x33},path1 ="help_res/bei1.png",path2 ="help_res/niu3.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为3"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x34},path1 ="help_res/bei1.png",path2 ="help_res/niu4.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为4"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x35},path1 ="help_res/bei1.png",path2 ="help_res/niu5.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为5"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x36},path1 ="help_res/bei1.png",path2 ="help_res/niu6.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为6"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x37},path1 ="help_res/bei2.png",path2 ="help_res/niu7.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为7"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x38},path1 ="help_res/bei2.png",path2 ="help_res/niu8.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为8"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x39},path1 ="help_res/bei3.png",path2 ="help_res/niu9.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张对10余数为9"},
    	-- 		{card ={0x01,0x0a,0x19,0x2a,0x3a},path1 ="help_res/bei4.png",path2 ="help_res/niu10.png",txt ="摆过牌后，3张牌能构成10或者10的倍\n数，剩余两张为10的整数倍"},
    	-- 		{card ={0x0b,0x2c,0x3d,0x2b,0x3d},path1 ="help_res/bei5.png",path2 ="help_res/niu_wh.png",txt ="四张牌为J,Q,K中的任意一个。第5张\n牌为10或者J,Q,K"},
    	-- 		{card ={0x02,0x23,0x34,0x25,0x36},path1 ="help_res/bei5.png",path2 ="help_res/niu_sz.png",txt ="五张牌为连牌"},
    	-- 		{card ={0x01,0x03,0x05,0x06,0x09},path1 ="help_res/bei6.png",path2 ="help_res/niu_th.png",txt ="五张牌花色相同"},
    	-- 		{card ={0x01,0x11,0x21,0x03,0x33},path1 ="help_res/bei6.png",path2 ="help_res/niu_hl.png",txt ="三张相同点数的牌加上两张相同点数\n的牌"},
    	-- 		{card ={0x01,0x11,0x21,0x31,0x35},path1 ="help_res/bei7.png",path2 ="help_res/niu_zd.png",txt ="四张相同点数的牌加上任意一张牌"},
    	-- 		{card ={0x01,0x02,0x03,0x04,0x05},path1 ="help_res/bei7.png",path2 ="help_res/niu_ths.png",txt ="五张牌为同花色连牌"},
    	-- 	}
    	-- local typeStr = {"无牛","牛一","牛二","牛三","牛四","牛五","牛六","牛七","牛八","牛九","牛牛","五花牛","顺子牛","同花牛","葫芦牛","炸弹牛","同花顺牛"}
    	-- local num = {"1倍","1倍","1倍","1倍","1倍","1倍","1倍","2倍","2倍","3倍","4倍","5倍","5倍","6倍","6倍","7倍","7倍"}

    	-- for i=1,17 do
    	-- 	local bei = cc.LabelTTF:create(typeStr[i], "fonts/FZHTJW.ttf", 24):setAnchorPoint(cc.p(0,0.5)):move(222 + (i-1)%2 * 410,1286 +170- (math.ceil(i/2)-1)*170):addTo(scrollView)
    	-- 	local niu = cc.LabelTTF:create(num[i], "fonts/FZHTJW.ttf", 40):setAnchorPoint(cc.p(0,0.5)):move(222 + (i-1)%2 * 410,1320 +170- (math.ceil(i/2)-1)*170):addTo(scrollView)
    	-- 	if i < 8 then
    	-- 		niu:setColor(cc.c3b(192,155,124))
    	-- 		bei:setColor(cc.c3b(192,155,124))
    	-- 	elseif i < 10 then
    	-- 		niu:setColor(cc.c3b(142,213,231))
    	-- 		bei:setColor(cc.c3b(142,213,231))
    	-- 	else
    	-- 		niu:setColor(cc.c3b(255,232,42))
    	-- 		bei:setColor(cc.c3b(255,232,42))
    	-- 	end
    	-- 	-- local niu =cc.Sprite:create(alltype[i].path1):setAnchorPoint(cc.p(0,0.5)):move(222 + (i-1)%2 * 410,1286 +170- (math.ceil(i/2)-1)*170):addTo(scrollView)
    	-- 	-- local bei =cc.Sprite:create(alltype[i].path2):setAnchorPoint(cc.p(0,0.5)):move(222 + (i-1)%2 * 410,1320 +170- (math.ceil(i/2)-1)*170):addTo(scrollView)
    	-- 	for j=1, 5 do
    	-- 		local card = CardSprite:createCard(alltype[i].card[j])
    	-- 		card:setAnchorPoint(cc.p(0.5,0.5))
    	-- 		card:setPosition(43 + (i-1)%2 * 410 + (j-1)*30,1300 +170 - (math.ceil(i/2)-1)*170)
    	-- 		card:setScale(0.57)
    	-- 		card:addTo(scrollView)
    	-- 	end

    	-- 	local strLabel = cc.Label:createWithTTF(alltype[i].txt, "fonts/FZHTJW.ttf", 20)
     --                          :setLineBreakWithoutSpace(true)
     --                          --:setMaxLineWidth(viewSize.width)
     --                          :setTextColor(cc.c4b(175,175,175,255))
     --                          :setAnchorPoint(cc.p(0, 1.0))
     --                          :setPosition(5 + (i-1)%2 * 410,1243 +170 - (math.ceil(i/2)-1)*170)
     --                          :addTo(scrollView)
     --                 local xian =cc.Sprite:create("help_res/xian.png"):move(178 + (i-1)%2 * 410,1188 +170- (math.ceil(i/2)-1)*170):addTo(scrollView)
    	-- end


	local closeBtn = helpLayer:getChildByName("image_bg"):getChildByName("btn_close")
	closeBtn:addTouchEventListener(function (ref, eventType)

		if eventType == ccui.TouchEventType.ended then
		    helpLayer:removeFromParent()
		end
	end)


end

--自己开牌视图
function GameViewLayer:onButtonConfirm(viewid)
	self.bCanMoveCard = false  --牌是否可以移动
	self.btCuo:setVisible(false) --隐藏确定框
	self.spriteCalculate:stopAllActions()
	self.spriteCalculate:setVisible(false) --隐藏计算框
	self.btShow:setVisible(false)
	self.btCuo:setVisible(false)
	self:updateCardType(0,false)
    local nodeCard = self._csbNode:getChildByName("Node_Card")
    local panelCard = nodeCard:getChildByName(string.format("Node_%d",viewid))

	--牌回复位置
	for i=1, cmd.MAX_CARDCOUNT do
        local card = self.nodeCard[cmd.MY_VIEWID][i]
		card:setPosition(130*(i-1), panelCard:getContentSize().height/2)
    end
end

function GameViewLayer:onButtonSwitchAnimate()
	self._menu:setVisible(true)
end

--叫庄
function GameViewLayer:gameCallBanker(callBankerViewId, bFirstTimes,cbMaxCallBankerTimes,isNoCall)
	if callBankerViewId == cmd.MY_VIEWID then
		if self._scene.cbDynamicJoin == 0 then
			print("self.btCallBanker",self.btCallBanker)
        	self:showCallBankerMul(nil,cbMaxCallBankerTimes,isNoCall)
        end
    end

    -- for i=1,cmd.GAME_PLAYER do
    -- 	local viewid = self._scene:SwitchViewChairID(i-1)
    -- 	if self.m_tabUserHead[viewid] then
    -- 	    self.m_tabUserHead[viewid]:hiddenMultiple()
    -- 		self.m_tabUserHead[viewid]:showFlashBg(3)
    -- 		if viewid ~= cmd.MY_VIEWID and not isNoCall then
    -- 			--self.m_tabUserHead[viewid]:setCallingBankerStatus(true)
    -- 		end
    -- 	end
    -- end

    -- --背景闪烁
    -- if not GlobalUserItem.bVideo then
    -- 	--self.m_tabUserHead[callBankerViewId]:showFlashBg(1)
    -- end
	

    if bFirstTimes then

    end
end
function GameViewLayer:setCallMultiple( callBankerViewId,multiple ,bBanker)
	print("setCallMultiple===========================2,"..callBankerViewId..","..multiple)
	if self.m_tabUserHead[callBankerViewId] ~= nil then
		self.m_tabUserHead[callBankerViewId]:setMultiple(multiple)
		self.m_tabUserHead[callBankerViewId]:setMultipleBankerPos(bBanker)
	end
end

function GameViewLayer:setBankerMultiple(BankerViewId)
	for i=1,cmd.GAME_PLAYER do
    	local viewid = self._scene:SwitchViewChairID(i-1)
    	if self.m_tabUserHead[viewid] then
    		if viewid ~= BankerViewId  then
    			-- self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
    				-- cc.CallFunc:create(function ()
    					self.m_tabUserHead[viewid]:hiddenMultiple()
    			-- end)))
    		else
    			
    			if self.m_tabUserHead[viewid]:getMultiple() == 0 then
    				self:setCallMultiple(viewid,1,true)
    			end
    			
    		end
    		self.m_tabUserHead[viewid]:setCallingBankerStatus(false)
			self.m_tabUserHead[viewid]:setMultipleBankerPos(true)
    	end
    end
end

function GameViewLayer:setBankerWaitIcon(ViewId,isShow,isNoraml,isXuan)
	if isXuan and self.nodePlayer[ViewId] then
		local point = cc.p(self.nodePlayer[ViewId]:getPosition())
		local point  ={cc.p(145,-60),cc.p(145,-60),cc.p(145,-60),cc.p(-145,-60),cc.p(-145,-60),cc.p(31,-86),cc.p(0,-138),cc.p(31,-86)}
		cc.Sprite:create(cmd.RES_PATH.."game/banker_xuanBei.png"):setName("banker_xuanBei"):move(point[ViewId]):addTo(self)--._csbNode)
		return
	end
	if self.m_tabUserHead[ViewId] then
		
		self.m_tabUserHead[ViewId]:setBankerWaitStatus(isShow,isNoraml,ViewId,isXuan)
	end
	
end

--显示叫庄倍数
function GameViewLayer:showCallBankerMul(visiable,cbMaxCallBankerTimes,isNoCall)

	if self._scene.cbDynamicJoin == 1 or GlobalUserItem.bVideo then
		return
	end
	
	if visiable == 0 then
		self.nodeCallMul:removeAllChildren()
		self.btMul = {}
		return
	end

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
         	if ref:getTag() ~= TAG_ENUM.BT_CANCEL then
         		--print("the tag ",ref:getTag())
         		--print("======,"..TAG_ENUM.BT_CANCEL)
         		-- self:playEffect("rober_bank_",GlobalUserItem.tabAccountInfo.cbGender)
         	end
        end
    end
    self.nodeCallMul:removeAllChildren()
    --配置下注积分的数目
    local tabStr = {"不叫","1 倍","2 倍","3 倍","4 倍","5 倍"}

	for i = 1, 5 do
		--按钮
		local file = "common/btn_shot1.png"
		if i == 1 then
			file = "game/btn_noCall.png"
		end
		self.btMul[i] = ccui.Button:create(file)
			:move(300 + 170*(i - 1), 224)
			:setTag(TAG_ENUM.BT_MUL1 + i -2)
			:setVisible(true)
			:addTo(self.nodeCallMul)
		self.btMul[i]:addTouchEventListener(btcallback)
		--self.btMul[i]:setScaleX(0.8)
		--倍数  

        local label = cc.LabelAtlas:create(i-1,"game/btn_num.png",20,28,string.byte("0"))
        		:setTag(TAG_ENUM.NUM_MUL)
        		:addTo(self.btMul[i])
        		:move(cc.p(self.btMul[i]:getContentSize().width/2 - 20,self.btMul[i]:getContentSize().height/2 - 14))

		local sp = display.newSprite(GameViewLayer.RES_PATH .. string.format("game/betFont.png"))
				:addTo(label)
			sp:setPosition(35, 13)

    	if i == 1 then

    		if isNoCall then
    			self.btMul[i]:setVisible(false)
    			if cbMaxCallBankerTimes then
        			-- self.nodeCallMul:setPositionX(70+85*(5-cbMaxCallBankerTimes ))
        			self.nodeCallMul:setPositionX(70-85)
        		end
    		else
    			self.btMul[i]:setTag(TAG_ENUM.BT_CANCEL)
    			label:setVisible(false)
    			-- if cbMaxCallBankerTimes then
        		-- 	self.btMul[i]:setPosition(260 + 150*(cbMaxCallBankerTimes), 210)
        		-- 	self.nodeCallMul:setPositionX(120+85*(5-cbMaxCallBankerTimes -1))
        		-- end
    		end
    		
    	end
		
		--动画
    	self.btMul[i]:runAction(cc.Sequence:create(
	    	cc.DelayTime:create(0.1*(i-1)),
	    	cc.MoveBy:create(0.1,cc.p(0,25)),
	    	cc.MoveBy:create(0.1,cc.p(0,-15))
    	))
    	if cbMaxCallBankerTimes and cbMaxCallBankerTimes  <i-1 and i~= 1 then
    		self.btMul[i]:setVisible(false)
    	end
	end
end

--游戏开始
function GameViewLayer:showChipBtn(bankerViewId)
	--dump(self.btChip)
    if bankerViewId ~= cmd.MY_VIEWID then
    	if self._scene.cbDynamicJoin == 0 then
	        for i = 1, #self.btChip do
	            self.btChip[i]:setVisible(true)
	        	self.btChip[i]:runAction(cc.Sequence:create(
		        	cc.DelayTime:create(0.1*(i-1)),
		        	cc.MoveBy:create(0.1,cc.p(0,25)),
		        	cc.MoveBy:create(0.1,cc.p(0,-15))
		        	))
	        end
	    end
    end
end

function GameViewLayer:resetEffect( )
	   --停止闪烁
    for i=1,cmd.GAME_PLAYER do
    	local viewid = self._scene:SwitchViewChairID(i-1)
    	if self.m_tabUserHead[viewid]  then
    		if  self._scene.wBankerUser ~= i-1 then
    			self.m_tabUserHead[viewid]:showFlashBg(3)
    		else
    			self.m_tabUserHead[viewid]:showFlashBg(5)
    		end
    	end
    end
end

function GameViewLayer:setCombineCard( data )
	self.cbCombineCard = clone(data)
end

function GameViewLayer:setSpecialInfo( bSpecial,cardType )


	--还有一种无牛的情况 扑克也不可操作
	self.bSpecialType = bSpecial
	self.bCanMoveCard = not bSpecial
	if bSpecial then
		self.cbSpecialCardType = cardType
	elseif not GameLogic:getOxCard(self.cbCombineCard) then 
		self.bSpecialType = true
		self.cbSpecialCardType = 0
		self.bCanMoveCard = false

		return
	end

end


function GameViewLayer:showCallBankerAni(callIndex,banker)
	if #callIndex ==1 then
		local viewid = self._scene:SwitchViewChairID(banker)
	    	if self.m_tabUserHead[viewid] then
	    		self.m_tabUserHead[viewid]:showFlashBg(4)
	    	end
	end
	local actionTable = {}
	local delayTime = 0.12
	for j=1, 4 do
		for i=1,#callIndex do

    		local viewid = self._scene:SwitchViewChairID(callIndex[i]-1)
	    	if self.m_tabUserHead[viewid] then
		    	self:runAction(cc.Sequence:create(
		    		cc.CallFunc:create(function() self.m_tabUserHead[viewid]:showFlashBg(1) end),
		    		cc.DelayTime:create(1.5),
		    		cc.CallFunc:create(function()
		    			if viewid == self._scene:SwitchViewChairID(banker) then
		    				self.m_tabUserHead[viewid]:runAction(cc.Sequence:create(
			    				cc.DelayTime:create(1),
			    				cc.CallFunc:create(function( ... )
			    					self.m_tabUserHead[viewid]:showFlashBg(5)
			    				end)
				    			))
			    		else
			    			self.m_tabUserHead[viewid]:showFlashBg(3)
			    		end
		    		end)
		    		))
	    	end
		    	
		 end
	end
	
end

function GameViewLayer:beginRobAnimation(callIndex, bankerChairId)
    local bankerViewId = self._scene:SwitchViewChairID(bankerChairId);
    print("bankerViewId =",bankerViewId)
    -- self.robLayer:setVisible(true);
    -- self.robRect:setOpacity(0);

    local totalTime=1.8;
    local tabDelay={1,2,3,4,5}; --轮数越多越快
    local s=0;
    for i=1,#tabDelay do
        s=s+tabDelay[i];
    end
    local tabPos={};
    local tabAct={};
    local tabViewId={};
    for i=1,#callIndex do
		local viewid = self._scene:SwitchViewChairID(callIndex[i]-1)
		print(viewid,self._scene.cbPlayStatus[i],self.m_tabUserHead[viewid])
        if self.m_tabUserHead[viewid] and self._scene.cbPlayStatus[callIndex[i]] == 1 then
            -- table.insert(tabPos,cc.p(self.node_player[viewId]:getPosition()));
            table.insert(tabViewId,viewid);
        end
    end
    dump(tabViewId,"beginRobAnimation")
    dump(callIndex,"callIndex")

	table.insert(tabAct,cc.DelayTime:create(0.5))
    local unitDelay=totalTime/(s*#tabViewId)
    for i=1,#tabDelay do
        for j=1,#tabViewId do --人数
            local act1=cc.CallFunc:create(function() 
                self.m_tabUserHead[tabViewId[j]]:showFlashBg(5)
                AudioEngine.playEffect("sound_res/rob.mp3")
            end)
            local act2=cc.DelayTime:create(tabDelay[i]*unitDelay)
            table.insert(tabAct,act1)
            table.insert(tabAct,act2)
            table.insert(tabAct,cc.CallFunc:create(function() self.m_tabUserHead[tabViewId[j]]:showFlashBg(3) end))

            if i==#tabDelay and tabViewId[j]==bankerViewId then
            	table.insert(tabAct,cc.CallFunc:create(function() self.m_tabUserHead[tabViewId[j]]:showFlashBg(5) end))
                table.insert(tabAct,cc.DelayTime:create(0.1))
                break
            end
        end
    end

    table.insert(tabAct,cc.CallFunc:create(function() self:setBankerMultiple(bankerViewId);end));
    table.insert(tabAct,cc.DelayTime:create(0.1))

    local act3 = cc.CallFunc:create(function() 
        self:setBankerUser(bankerViewId, true);
    end)
    table.insert(tabAct,act3);

    table.insert(tabAct,cc.DelayTime:create(1))
    table.insert(tabAct,cc.CallFunc:create(function() 
    		if self.m_tabUserHead[bankerViewId] then
    			self.m_tabUserHead[bankerViewId]:showFlashBg(4) 
    		end
    	end))
    local action = cc.Sequence:create(tabAct)
    action:setTag(BANKER_ACT)
    if #tabViewId>1 then
        self:runAction(action);
    end

end

function GameViewLayer:gameAddScore(viewId, score,isAni)

	if self.m_tabUserHead[viewId] then
		self.m_tabUserHead[viewId]:showCallInScore(score,true,isAni)
		self.m_tabUserHead[viewId]:setHeadStatus(cmd.US_Normal)
	end
	--self.nodePlayer[viewId]:setVisible(true)

    -- 自己下注, 隐藏下注信息
    if viewId == cmd.MY_VIEWID then
    	-- for i = 1, #self.btChip do
	    --     self.btChip[i]:setVisible(false)
	    -- end
	    self.nodeChip:removeAllChildren()
    end
end

--发牌
function GameViewLayer:gameSendCard(firstViewId, sendCount)

	print("开始发牌")
	if sendCount == 0 then
		print("发牌数为0")
		return
	end

	--开始发牌
	local wViewChairId = firstViewId  --首先发牌的玩家
	local delayTime = 0.07    --等待时间
	local actionList = {}
	local playerCount = 0    --游戏人数
	for i=1,cmd.GAME_PLAYER do
		if self:getParentNode():isPlayerPlaying(wViewChairId) then
			playerCount = playerCount + 1
			local function callbackWithArgsSendCard(viewid,count)
	              local ret = function ()
					self:sendCardAnimate(viewid,count)
					if self._scene:getPlayNum() == playerCount then
						self._scene:sendCardFinish()
					end
	              end
	              return ret
	        end
			table.insert(actionList, cc.DelayTime:create(delayTime))
			if i == 1 then
				table.insert(actionList, 
				cc.CallFunc:create(function() 
					if sendCount ~= 1 then
						AudioEngine.playEffect("sound_res/fapai.mp3")
					end
				end))
			end
	    	table.insert(actionList,  cc.CallFunc:create(callbackWithArgsSendCard(wViewChairId,sendCount)))
		end
		wViewChairId = wViewChairId + 1
		if wViewChairId > cmd.GAME_PLAYER  then
			wViewChairId = 1
		end
	end
	self:runAction(cc.Sequence:create(actionList))
end

--parm viewId 发送到哪一个位置
--parm count 发送的数量
function GameViewLayer:sendCardAnimate(viewId,sendCount)
	print("viewId,sendCount",viewId,sendCount)
    local nodeCard = self._csbNode:getChildByName("Node_Card")
    local panelCard = nodeCard:getChildByName(string.format("Node_%d",viewId))
	self.animateCard = self._csbNode:getChildByName("Node_AniCard")
	for i=1,sendCount do
		--用于发牌动作的牌
        local card = CardSprite:createCard()
        card:setScale(0.6)
        card:setVisible(true)
        self.animateCard:addChild(card)
        card:setLocalZOrder(i)
        card:setTag(i)
		--开始位置
		local beginpos = cc.p(card:getPositionX(),card:getPositionY())
		--结束位置
		local endpos = cc.p(0,0)
		local spawn = nil
		local moveTime = 0.25
		--获取目标地点相对于玩家手牌的相对位置
		local index = i    --牌的索引
		--特殊处理4号位的玩家
		if viewId == 4 then  
			if sendCount == 1 then
				index = 1
			elseif sendCount == 4 then
				index = i + 1
			end
		else
			if sendCount == 1 then
				index = 5
			end
		end
		if viewId ==  cmd.MY_VIEWID then
			local posWorld = panelCard:convertToWorldSpace(cc.p(130*(index-1), 0))
			endpos = self.animateCard:convertToNodeSpace(posWorld)
			
			if sendCount ==1 then
				spawn = cc.Spawn:create(self:getMoveActionEx(moveTime,beginpos,endpos),cc.ScaleTo:create(moveTime,1),cc.RotateBy:create(moveTime, 720))
			else
				spawn = cc.Spawn:create(self:getMoveActionEx(moveTime,beginpos,endpos),cc.ScaleTo:create(moveTime,1))
			end
		else
			local posWorld = panelCard:convertToWorldSpace(cc.p(33+25*(index-1), 0))
			endpos = self.animateCard:convertToNodeSpace(posWorld)
			if sendCount ==1 then
				spawn = cc.Spawn:create(self:getMoveActionEx(moveTime,beginpos,endpos),cc.ScaleTo:create(moveTime,0.5),cc.RotateBy:create(moveTime, 720))
			else
				spawn = cc.Spawn:create(self:getMoveActionEx(moveTime,beginpos,endpos),cc.ScaleTo:create(moveTime,0.5))
			end
		end
		card:runAction(cc.Sequence:create(
			cc.DelayTime:create((i-1)*0.01),
			spawn,
			cc.CallFunc:create(function( )
				card:removeFromParent()
				if (viewId == cmd.MY_VIEWID or GlobalUserItem.bVideo ) and not self._scene.m_bLookOn then
					--print(sendCount..",==========================>sendCount,"..self._scene.m_tabPrivateRoomConfig.sendCardType)
					--dump({self._scene.isCanCuo},"================99999")
					--dump({sendCount,self._scene.m_tabPrivateRoomConfig,self._scene.isCanCuo},"99999999999999999999999===")
					if ((sendCount == 1 and self._scene.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR  and self._scene.isCanCuo) or 
					 (self._scene.m_tabPrivateRoomConfig.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_BETFIRST  and self._scene.isCanCuo) or 
					 (self._scene.m_tabPrivateRoomConfig.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_TONGBI  and self._scene.isCanCuo))and not GlobalUserItem.bVideo then
						--print("=================================")
	      				self:updateButton(true,true)
	      			else
		      				
						self:openCardAnimate(viewId,index)
					end
				end

				-- if i == 3 then
					if sendCount == 1 then
						AudioEngine.playEffect("sound_res/SEND_CARD.wav")
					else
						-- AudioEngine.playEffect("sound_res/fapai.mp3")
					end
				-- end
				-- if viewId ~= cmd.MY_VIEWID and 
				if (sendCount == cmd.MAX_CARDCOUNT or sendCount == 1) and not GlobalUserItem.bVideo then
					self:setOpenCardVisible(viewId,true,false)
					-- self:puttingCardAni(viewId)
				end
				print("viewId,index",viewId,index)
				self.nodeCard[viewId][index]:setVisible(not self._scene.bSendCardCtr)
			end)
			))
	end
end

--摆牌动画
function GameViewLayer:puttingCardAni(viewId)
	for i=1,cmd.MAX_CARDCOUNT do
	    local action = cc.Sequence:create(cc.MoveBy:create(0.1, cc.p(0,10)), cc.MoveBy:create(0.1, cc.p(0, -10)))
	    local repaction = cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(i*0.05), action, cc.DelayTime:create((cmd.MAX_CARDCOUNT-i)*0.05 )))
	    local card = self.nodeCard[viewId][i]
	    if nil ~= card then
	      card:stopAllActions()
	      card:setFiveCard(false)
	        if viewId == cmd.MY_VIEWID then
				card:setPosition(130*(i-1), 0)
	        else
	        	card:setPosition(33+25*(i-1), 0)
	        end
	      card:runAction(repaction)
	    end
  	end
end

--停止卡牌摆牌运动
function GameViewLayer:stopCardAni( wViewChairId )
	if wViewChairId ~= cmd.MY_VIEWID then
		local nodeCard = self._csbNode:getChildByName("Node_Card")
		local panelCard = nodeCard:getChildByName(string.format("Node_%d",wViewChairId))
		for i=1,cmd.MAX_CARDCOUNT do
			self.nodeCard[wViewChairId][i]:stopAllActions()
			self.nodeCard[wViewChairId][i]:setPosition(33+25*(i-1), 0)

		end
	end
end



--获取移动动画
--inorout,0表示加速飞出,1表示加速飞入
--isreverse,0表示不反转,1表示反转
function GameViewLayer:getMoveActionEx(time,startPoint, endPoint,height,angle)
   	--把角度转换为弧度
    angle = angle or 90
    height = height or 50
    local radian = angle*3.14159/180.0
    --第一个控制点为抛物线左半弧的中点  
    local q1x = startPoint.x+(endPoint.x - startPoint.x)/4.0;  
    local q1 = cc.p(q1x, height + startPoint.y+math.cos(radian)*q1x);         
    -- 第二个控制点为整个抛物线的中点  
    local q2x = startPoint.x + (endPoint.x - startPoint.x)/2.0;  
    local q2 = cc.p(q2x, height + startPoint.y+math.cos(radian)*q2x);  
    --曲线配置  
    local bezier = {
        q1,
        q2,
        endPoint
    }
    --使用EaseInOut让曲线运动有一个由慢到快的变化，显得更自然  
    local beaction = cc.BezierTo:create(time, bezier)
    --if inorout == 0 then
    local easeoutaction = cc.EaseOut:create(beaction, 1)
    return easeoutaction
    --else
        --return cc.EaseIn:create(beaction, 1)
    --end
end

--获取移动动画
--inorout,0表示加速飞出,1表示加速飞入
--isreverse,0表示不反转,1表示反转
-- function GameViewLayer:getMoveAction(time,beginpos, endpos, inorout, isreverse)
--     local offsety = (endpos.y - beginpos.y)*0.7
--     local controlpos = cc.p(beginpos.x, beginpos.y+offsety)
--     if isreverse == 1 then
--         offsety = (beginpos.y - endpos.y)*0.7
--         controlpos = cc.p(endpos.x, endpos.y+offsety)
--     end
--     local bezier = {
--         controlpos,
--         endpos,
--         endpos
--     }
--     local beaction = cc.BezierTo:create(time, bezier)
--     if inorout == 0 then
--         return cc.EaseOut:create(beaction, 1)
--     else
--         return cc.EaseIn:create(beaction, 1)
--     end
-- end

--开牌动画 --viewid 玩家视图位置  index 
function GameViewLayer:openCardAnimate( viewid,index,isEnd)
	local scaleMul = viewid == cmd.MY_VIEWID and 1 or 0.6
	if index then --单张牌
		local card = self.nodeCard[viewid][index]
		card:runAction(cc.Sequence:create(
			cc.ScaleTo:create(0.15,0.1,scaleMul),
			cc.CallFunc:create(function ( )
				if viewid ~= cmd.MY_VIEWID then
					print("index is ===========================",index)
				end
				self._scene:openOneCard(viewid,index)
			end),
			cc.ScaleTo:create(0.15,scaleMul,scaleMul)
			))
	else --五张牌
		for i=1,cmd.MAX_CARDCOUNT do
			local card = self.nodeCard[viewid][i]
			card:runAction(cc.Sequence:create(
				cc.DelayTime:create(i*0.05),
				cc.ScaleTo:create(0.15,0.1,scaleMul),
				cc.CallFunc:create(function (  )
					self._scene:openOneCard(viewid,i,isEnd)
				end),
				cc.ScaleTo:create(0.15,scaleMul,scaleMul)
				))
		end
		local chairid = self._scene:SwitchChairID(viewid) -1
		local userInfo = self._scene:getUserInfoByChairID(chairid)
		print("=====>>>>openCardAnimate",viewid, chairid, userInfo, soundFile,cardType)
		if isEnd then
			local cbGender = 0
			if userInfo then
				cbGender = userInfo.cbGender
			end
			local soundFile, cardType = self:getSoundFile( self._scene.lCardType[chairid+1] )
			self:playEffect(soundFile,cbGender)
			local sprFlag = cc.Sprite:create(GameViewLayer.RES_PATH.."game/"..cardType)
	    	self.flag_openCard[viewid]:setSpriteFrame(sprFlag:getSpriteFrame())
	    	self.flag_openCard[viewid]:setVisible(true)

	    	local multip = self._scene._cardTypeTimes[self._scene.lCardType[chairid+1] + 1]
	    	if self._scene.lCardType[chairid+1] >= 7 then
		    	-- local label = cc.LabelAtlas:create("/"..multip,"game/multipFont.png",60,72, string.byte("/"))
		    	local str = "huang"
		    	if self._scene.lCardType[chairid+1] >= 10 then
		    		str = "hong"
		    	end
		    	local file = string.format("game/shuzi/%s_%d.png", str, multip)
		    	local sp = cc.Sprite:create("game/shuzi/"..str.."_x.png")
		    	sp:setAnchorPoint(0,0.5)
		    	sp:addTo(self.flag_openCard[viewid])
		    	sp:setPosition(220,46)
		    	if multip >= 10 then
			    	local file = string.format("game/shuzi/%s_%d.png", str, 1)
			    	local sp = cc.Sprite:create(file)
			    	sp:setAnchorPoint(0,0.5)
			    	sp:addTo(self.flag_openCard[viewid])
			    	sp:setPosition(270,46)
			    	local file = string.format("game/shuzi/%s_%d.png", str, multip-10)
			    	local sp = cc.Sprite:create(file)
			    	sp:setAnchorPoint(0,0.5)
			    	sp:addTo(self.flag_openCard[viewid])
			    	sp:setPosition(310,46)
			    else
			    	local file = string.format("game/shuzi/%s_%d.png", str, multip)
			    	local sp = cc.Sprite:create(file)
			    	sp:setAnchorPoint(0,0.5)
			    	sp:addTo(self.flag_openCard[viewid])
			    	sp:setPosition(280,46)
			    end

		    end
	    	if viewid ~= cmd.MY_VIEWID then
	    		self.flag_openCard[viewid]:setScale(0.5)
    			self.flag_openCard[viewid]:setPositionX(88)
	    	end
		end
	end
end

function GameViewLayer:showPlayerCardType( viewid, CardType)
	local soundFile, cardType = self:getSoundFile( CardType )
	local sprFlag = cc.Sprite:create(GameViewLayer.RES_PATH.."game/"..cardType)
	self.flag_openCard[viewid]:setSpriteFrame(sprFlag:getSpriteFrame())
	self.flag_openCard[viewid]:setVisible(true)

	local multip = self._scene._cardTypeTimes[CardType + 1]
	if CardType >= 7 then
    	-- local label = cc.LabelAtlas:create("/"..multip,"game/multipFont.png",60,72, string.byte("/"))
    	local str = "huang"
    	if CardType >= 10 then
    		str = "hong"
    	end
    	local file = string.format("game/shuzi/%s_%d.png", str, multip)
    	local sp = cc.Sprite:create("game/shuzi/"..str.."_x.png")
    	sp:setAnchorPoint(0,0.5)
    	sp:addTo(self.flag_openCard[viewid])
    	sp:setPosition(220,46)
    	if multip >= 10 then
	    	local file = string.format("game/shuzi/%s_%d.png", str, 1)
	    	local sp = cc.Sprite:create(file)
	    	sp:setAnchorPoint(0,0.5)
	    	sp:addTo(self.flag_openCard[viewid])
	    	sp:setPosition(270,46)
	    	local file = string.format("game/shuzi/%s_%d.png", str, multip-10)
	    	local sp = cc.Sprite:create(file)
	    	sp:setAnchorPoint(0,0.5)
	    	sp:addTo(self.flag_openCard[viewid])
	    	sp:setPosition(310,46)
	    else
	    	local file = string.format("game/shuzi/%s_%d.png", str, multip)
	    	local sp = cc.Sprite:create(file)
	    	sp:setAnchorPoint(0,0.5)
	    	sp:addTo(self.flag_openCard[viewid])
	    	sp:setPosition(280,46)
	    end

    end
	if viewid ~= cmd.MY_VIEWID then
		self.flag_openCard[viewid]:setScale(0.5)
		self.flag_openCard[viewid]:setPositionX(88)
	end
end
function GameViewLayer:resetCardByType(cards,cardType ,viewid)
	-- dump(cards, "the card data =====")
	-- dump(cardType, "the card type =====")
	for i=1,cmd.GAME_PLAYER do
		for idx=1,cmd.MAX_CARDCOUNT do
			if cardType[i] and cards[i] and (cards[i][idx] ~= 0) and (cardType[i] >= 0) then
				local viewId = self._scene:SwitchViewChairID(i-1)
				if viewid == viewId then
					local card = self.nodeCard[viewId][idx]
					assert(card)
					local nodeCard = self._csbNode:getChildByName("Node_Card")
					local panelCard = nodeCard:getChildByName(string.format("Node_%d",viewId))
					local posX = (viewId==cmd.MY_VIEWID) and 130*(idx-1) or 33+25*(idx-1)
					if (cardType[i] > 0 and idx > 3) then
						posX = posX + ((viewId==cmd.MY_VIEWID) and 40 or 25)
					end
					
					if self._scene.cbLastCardData and self._scene.cbLastCardData[i] == cards[i][idx]   then
						
						card:setFiveCard(true)
						if viewId ~=cmd.MY_VIEWID then
							card:setPositionY(15)
						end
					end
				
					card:setPositionX(posX)	
				end
			end
		end
	end
end

function GameViewLayer:getSoundFile( CardType )
	local soundFile 
	local cardType = "ox_0.png"
	if CardType >=0 and  CardType <= 10 then
		print("CardType",CardType)
		soundFile = "niu_"..string.format("%d_",CardType)
		cardType = string.format("ox_%d.png",CardType)
	elseif CardType > 10 and CardType < 20 then --特殊牌型
		print("type value is =====",CardType)
		
		if CardType == GameLogic.CT_CLASSIC_OX_VALUE_FOURFLOWER then --四花牛
			cardType = "ox_11.png"
			soundFile = "niu_sh_"
		elseif CardType == GameLogic.CT_CLASSIC_OX_VALUE_FIVEFLOWER then --五花牛
			cardType = "ox_12.png"
			soundFile = "niu_wh_"
		elseif CardType == GameLogic.CT_CLASSIC_OX_VALUE_SHUNZI then --顺子
			cardType = "ox_13.png"
			soundFile = "niu_sz_"
		elseif CardType == GameLogic.CT_CLASSIC_OX_VALUE_SAMEFLOWER then--同花
			cardType = "ox_14.png"
			soundFile = "niu_th_"
		elseif CardType == GameLogic.CT_CLASSIC_OX_VALUE_HULU then--葫芦
			cardType = "ox_15.png"
			soundFile = "niu_hl_"
		elseif CardType == GameLogic.CT_CLASSIC_OX_VALUE_BOMB then--炸弹
			cardType = "ox_16.png"
			soundFile = "niu_zd_"
		elseif CardType == GameLogic.CT_CLASSIC_OX_VALUE_TONGHUASHUN then--同花顺
			cardType = "ox_17.png"
			soundFile = "niu_ths_"
		elseif CardType == GameLogic.CT_CLASSIC_OX_VALUE_FIVESNIUNIU then--五小牛
			cardType = "ox_18.png"
			soundFile = "niu_wx_"
		end
	end
	return soundFile,cardType
end


function GameViewLayer:showAllKillAni(lScore)
	
	local isKill = true
	local isWin = true
	local num = 0
	for i=1, cmd.GAME_PLAYER do
		if self._scene.cbPlayStatus[i] == 1 and self._scene.wBankerUser ~= i-1 then
			num = num+1
			if lScore[i] > 0 then
				isKill = false
			elseif lScore[i] < 0  then
				isWin = false
			end
		end
	end


	if num >1 and (isKill or isWin) and self._scene.wBankerUser ~= 65535 then
		local num = isKill and "tongsha" or "tongpei"
		
		local resultNode = ExternalFun.loadCSB(""..num..".csb", self)
		    resultNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
		    local resultAction = ExternalFun.loadTimeLine(""..num..".csb")
		    resultAction:gotoFrameAndPlay(0, false)
		    resultNode:runAction(resultAction)
		    resultNode:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function()
		      resultNode:removeFromParent()
		      end)))

		return true
	end

	return false
end

--游戏结束
function GameViewLayer:gameEnd(lScore,lCardType)
		
	--移除
	self.nodeChip:removeAllChildren()
	self.nodeCallMul:removeAllChildren()
	self.nodeCall:setVisible(false)
	self.btMul = {}
	self.btCuo:setVisible(false)
	self:removeChildByName("banker_xuanBei")

	self.bActComplete = false


	local bankerViewId = self._scene:SwitchViewChairID(self._scene.wBankerUser)
	dump(lScore,"GameViewLayer:gameEnd")
	local index = self._scene._MyChairID + 1
	print("index",index)
	local meScore = lScore[index]

	local delayTime = 0
	-- 	if self:showAllKillAni(lScore) then
	-- 		delayTime = 1
	-- 	end
	local function playEndSpine( ... )
		-- body
		if self._scene:isPlayerPlaying(self._scene:SwitchViewChairID(index-1)) then
			self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ()
			    local action = sp.SkeletonAnimation:create("game/endAction/yw.json","game/endAction/yw.atlas")
			    self:addChild(action)
			    action:setName("endAction")
			    action:setPosition(667, 375)
				if meScore > 0 then
					self:playEffect("gameWin.mp3")
				    -- local action = sp.SkeletonAnimation:create("game/endAction/yw.json","game/endAction/yw.atlas")
				    action:setAnimation(0, "animation", false)
				    -- self:addChild(action, 100)
				    -- action:setPosition(667, 375)
				    -- action:runAction(cc.Sequence:create(cc.DelayTime:create(3), cc.CallFunc:create(function() action:removeFromParent() end)))
				elseif meScore < 0 then
				    -- local action = sp.SkeletonAnimation:create("game/endAction/yw.json","game/endAction/yw.atlas")
				    action:setAnimation(0, "animation2", false)
				    -- self:addChild(action, 100)
				    -- action:setPosition(667, 375)
				    -- action:runAction(cc.Sequence:create(cc.DelayTime:create(3), cc.CallFunc:create(function() action:removeFromParent() end)))

					self:playEffect("gameLose.mp3")
				end
			    action:runAction(cc.Sequence:create(cc.DelayTime:create(3), 
			    	cc.CallFunc:create(function() 
			    		action:removeFromParent()
			    		-- self.bActComplete = true
			    		-- print("++++*****************动画结束",self.bActComplete)
			     end)))
			end)))
		else
			-- self.bActComplete = true
		end

	    for i=1,cmd.GAME_PLAYER do
	      if self.m_tabUserHead[i] then
	      	self.m_tabUserHead[i]:showCallInScore("",false)
	      end
	    end
	end
		

	-- if not GlobalUserItem.bVideo and not (GlobalUserItem.bPrivateRoom and PriRoom:getInstance().m_tabPriData.dwPlayCount == PriRoom:getInstance().m_tabPriData.dwDrawCountLimit) then 
	-- 	self.btStart:setVisible(true)
	-- 	self:setBtnchangeShow(true)
	-- end
    
	-- 隐藏牌信息
	self.spriteCalculate:setVisible(false)

	local action = {} --动画表
	local loseList = {} --输家
	local winList = {} --赢家 
	self.goldList = {} --金币
	for i=1,cmd.GAME_PLAYER do
		
		--类型
		local viewid = self._scene:SwitchViewChairID(i-1)
		--分数
		-- if self._scene:isPlayerPlaying(viewid) then
		-- 	self:runWinLoseAnimate(viewid,lScore[i])
		-- end

		if GlobalUserItem.bVideo then
			if self.m_tabUserHead[viewid] then
				self.m_tabUserHead[viewid].m_clipScore:setString(self.m_tabUserHead[viewid]:getScoreString(lScore[i]))
			end
		end

		--赢输列表
		local viewid = self._scene:SwitchViewChairID(i-1)
		if lScore[i] ~= 0  and viewid ~= bankerViewId and self._scene:isPlayerPlaying(viewid) then
			
			if lScore[i] < 0 then
				table.insert(loseList,viewid)
			elseif lScore[i] > 0 then
				table.insert(winList,viewid)
			end
		end
	end

	--随机位置
	local function getRandPos(pos) 

		if nil == pos then
			return nil
		end

		local beginpos = pos --cc.p(x,y)
		local offsetx = math.random()
		local offsety = math.random()

		return cc.p(beginpos.x + offsetx*20, beginpos.y + offsety*20)
	end
	local bankerViewId = self._scene:SwitchViewChairID(self._scene.wBankerUser)
	if self._scene.m_tabPrivateRoomConfig.bankGameType ==  cmd.BANERGAMETYPE_CONFIG.BGT_TONGBI then --通比玩法
		bankerViewId = winList[1]
		winList = {}
	end

	print("bankerViewId",bankerViewId)
	dump(pointUserInfo[bankerViewId])

	local goldPoint = {cc.p(10,36),cc.p(0,33),cc.p(16,15),cc.p(26,12),cc.p(-8,23),cc.p(-21,16),cc.p(23,0),cc.p(3,7),cc.p(-10,6),cc.p(-8,-10),cc.p(-22,2),cc.p(-32,-9)}
	

	local actionTime = 0.5
	local goldTable1 = {}
	--金币动画
	for i=1,#loseList do
		local viewid = loseList[i]
		local chairid = self._scene:SwitchChairID(viewid)
		local goldnum = 12--self:getgoldnum(lScore[chairid])
		--local loseNode = faceNode:getChildByName(string.format("FileNode_%d",viewid))
		--local losePosX,losePosY =  loseNode:getPosition()
		goldTable1[i] = {}
		for j=1,goldnum do
			local pgold = cc.Sprite:create("game/im_fly_gold_1.png")
			goldTable1[i][j] = pgold
			pgold:setLocalZOrder(10+12-j)
			local loseBeginpos = pointUserInfo[viewid]--cc.p(pointUserInfo[viewid].x+goldPoint[j].x,pointUserInfo[viewid].y+goldPoint[j].y)
			local bankerPos = pointUserInfo[bankerViewId] or cc.p(0,0)--getRandPos(pointUserInfo[bankerViewId]) or cc.p(0,0)

			pgold:setPosition(loseBeginpos)
			pgold:setVisible(false)
			pgold:runAction(cc.Sequence:create(cc.DelayTime:create(0.4+j*0.02),
				cc.Show:create(),
				cc.CallFunc:create(function()
					if j == 1 and i == 1 then
						AudioEngine.playEffect("sound_res/CoinFly.wav")		
					end
				end),
				cc.DelayTime:create(0.1),
				cc.MoveTo:create(actionTime,bankerPos),
				-- self:getMoveActionEx(actionTime,loseBeginpos, bankerPos),
				cc.CallFunc:create(function()
					-- if j == 1 and i == 1 then
					-- 	AudioEngine.playEffect("sound_res/CoinFly.wav")		
					-- end
					if j==12 then
	      				self:runAction(cc.Sequence:create(cc.DelayTime:create(0),cc.CallFunc:create(function ( ... )
	      					for n=1,12 do
	      						if goldTable1[i][n] then
	      							goldTable1[i][n]:removeFromParent()
	      						end
	      					end
	      				end)))
	      				
						if #winList == 0 then
		  					for k=1,cmd.GAME_PLAYER do
							    --类型
								local viewid = self._scene:SwitchViewChairID(k-1)
								--分数
								if self._scene:isPlayerPlaying(viewid) then
									self:runWinLoseAnimate(viewid,lScore[k])
								end
							end
							playEndSpine()
						end
	      			end
					
				end)))
			table.insert(self.goldlist, pgold)
			self:addChild(pgold, 10,FLY_COIN_TAG)
		end
	end

	--延迟时间
	local winDelayTime = #loseList > 0 and 0.8 or 0
	local bankerPos = pointUserInfo[bankerViewId] or cc.p(0,0)--getRandPos(pointUserInfo[bankerViewId]) or cc.p(0,0)
	local goldTable2 = {}
	for i=1,#winList do
		local viewid = winList[i]
		local chairid = self._scene:SwitchChairID(viewid)
		local goldnum = 12--self:getgoldnum(lScore[chairid])
		--local winNode = faceNode:getChildByName(string.format("FileNode_%d",viewid))
		--local winPosX,winPosY =  winNode:getPosition()
		goldTable2[i] = {}
        for j=1,goldnum do
	          local pgold = cc.Sprite:create("game/im_fly_gold_1.png")
	          goldTable2[i][j] = pgold
	          local winEndPos = pointUserInfo[viewid]--getRandPos(pointUserInfo[viewid])
	          pgold:setLocalZOrder(10+12-j)
	          local beganPoint = cc.p(bankerPos.x+goldPoint[j].x,bankerPos.y+goldPoint[j].y)
	          pgold:setPosition(beganPoint)
	          pgold:setVisible(false)
	          pgold:runAction(cc.Sequence:create(cc.DelayTime:create(0.5+j*0.01+winDelayTime),
	          	cc.Show:create(),
	          	cc.CallFunc:create(function()
					if j == 1 and i == 1 then
						AudioEngine.playEffect("sound_res/CoinFly.wav")		
					end
				end),
				cc.MoveTo:create(actionTime,winEndPos),
	          		-- self:getMoveActionEx(actionTime,beganPoint , winEndPos),
	          		cc.CallFunc:create(function()
	     --      			if j == 1 and i == 1 then
						-- 	AudioEngine.playEffect("sound_res/CoinFly.wav")
						-- end
	          			if j==12 then
	          				self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ( ... )
	          					for n=1,12 do
	          						if goldTable2[i][n] then
	          							goldTable2[i][n]:removeFromParent()
	          						end
	          					end
	          					playEndSpine()
	          					for k=1,cmd.GAME_PLAYER do
								    --类型
									local viewid = self._scene:SwitchViewChairID(k-1)
									--分数
									if self._scene:isPlayerPlaying(viewid) then
										self:runWinLoseAnimate(viewid,lScore[k])
									end
								end
	          				end)))
	          			
	          			end
	              
	            end)))
	          table.insert(self.goldlist, pgold)
	          self:addChild(pgold, 10, FLY_COIN_TAG)
        end
	end

end

function GameViewLayer:showRoomRule( config)

	dump(config, "the config is ")
    local roomRuleInfoStr = ""
    local roomRuleInfoTTF =  self._csbNode:getChildByName("Text_info")
    assert(roomRuleInfoTTF)

      --房间类型
    -- local cardType = {"经典模式","疯狂加倍"}
    -- local sendType = {"发四等五","下注发牌"}
    -- local bankerType = {"霸王庄","明牌抢庄","牛牛上庄","无牛下庄","自由抢庄","通比玩法"}
    -- --dump(config, "config is =========")

    -- if config.cardType ~= 0  and config.cardType < 24 then
    -- 	roomRuleInfoStr = roomRuleInfoStr..cardType[config.cardType-22+1]..","..sendType[config.sendCardType-32+1]..","..bankerType[config.bankGameType-52+1]
    -- end
    
    --print("config str is =============",roomRuleInfoStr)
    roomRuleInfoTTF:setVisible(true)
    -- local index1= config.bankGameType == cmd.BANERGAMETYPE_CONFIG.BGT_ROB and 1 or 2 
    -- local index2= config.gtType == cmd.KING_CONFIG.GT_HAVEKING and 2 or 1
    local idx1 = config.cardType-22+1
    local idx2 = config.sendCardType-32+1
    local idx3 = config.bankGameType-52
    print("***************房间类型*******************",idx1,idx2,idx3,self._csbNode:getChildByName("sendType"))
    self._csbNode:getChildByName("sendType"):setTexture("game/sendType_"..idx2..".png")
    self._csbNode:getChildByName("bankerType"):setTexture("game/bankerType_"..idx3..".png")
    self._csbNode:getChildByName("playType"):setTexture("game/cardType_"..idx1..".png")

    dump(self._scene._cardTypeTimes)
    if idx1 == 2 then
    	-- roomRuleInfoStr = "无牛*1 牛一*1 牛二*2 牛三*3 牛四*4 牛五*5 牛六*6 牛七*7 牛八*8 牛九*9 牛牛*10"
    else
    	-- roomRuleInfoStr = string.format("无牛-牛六*%d 牛七-牛八*%d 牛九*%d 牛牛*%d",self._scene._cardTypeTimes[1],self._scene._cardTypeTimes[8],self._scene._cardTypeTimes[10],self._scene._cardTypeTimes[11])
    end
    -- roomRuleInfoTTF:setString(roomRuleInfoStr)

end

--房卡规则界面
function GameViewLayer:showPriRule( config )
	--游戏牌型(经典模式,疯狂加倍)
	local spriteNormalDot = cc.Sprite:create("game/info/dot_0.png")
	local selectedDot =  cc.Sprite:create("game/info/dot_1.png")
	local modes = {}
	for i=1,2 do
		local mode = self.RoomInfoLayer:getChildByName("Node_rule1"):getChildByName("dot_mode_"..i)
		mode:setSpriteFrame(spriteNormalDot:getSpriteFrame())
		table.insert(modes, mode)
	end

	local target = (config.cardType == cmd.CARDTYPE_CONFIG.CT_CLASSIC) and modes[1] or modes[2]
	target:setSpriteFrame(selectedDot:getSpriteFrame())
	modes = {}

	--发牌模式
	for i=1,2 do
		local mode = self.RoomInfoLayer:getChildByName("Node_rule1"):getChildByName("dot_send_"..i)
		mode:setSpriteFrame(spriteNormalDot:getSpriteFrame())
		table.insert(modes, mode)
	end

	target = (config.sendCardType == cmd.SENDCARDTYPE_CONFIG.ST_SENDFOUR) and modes[1] or modes[2]
	target:setSpriteFrame(selectedDot:getSpriteFrame())
	modes = {}

	--庄家模式
	for i=1,6 do
		local mode = self.RoomInfoLayer:getChildByName("Node_rule1"):getChildByName("dot_bank_"..i)
		mode:setSpriteFrame(spriteNormalDot:getSpriteFrame())
		table.insert(modes, mode)
	end

	local dotindex = config.bankGameType-52+1
	modes[dotindex]:setSpriteFrame(selectedDot:getSpriteFrame())

	local btnDismiss = self.RoomInfoLayer:getChildByName("Node_rule1"):getChildByName("btn_dismiss")
	    if GlobalUserItem.bPrivateGoldRoom then
	         btnDismiss:setVisible(false)
	    end
end


--初始化新大厅房间信息
function GameViewLayer:initRoomInfo()
    self.RoomInfoLayer = cc.CSLoader:createNode(cmd.RES_PATH.."game/info/Layer_info.csb"):addTo(self, GameViewLayer.ZORDER_SETTING)
    self.RoomInfoLayer:setVisible(false)

    local PanelLayer = self.RoomInfoLayer:getChildByName("Panel")
    self.RoomInfoLayer:getChildByName("Node_score"):setVisible(false)
    -- 遮罩
    PanelLayer:addTouchEventListener(function(ref)
        self.RoomInfoLayer:setVisible(false)
    end)

    local boxCallback = function(sender,eventType)


		if self._scene.m_bLookOn and 2 == sender:getTag() then
			self.m_tabBoxGame[2]:setSelected(false)
		            showToast(self._scene, "观战中不可使用该功能!", 2)
		            return 
		end
        self.m_tabBoxGame[1]:setSelected(1 == sender:getTag())
        self.m_tabBoxGame[2]:setSelected(2 == sender:getTag())

        self.RoomInfoLayer:getChildByName("Node_rule1"):setVisible(1 == sender:getTag())
        self.RoomInfoLayer:getChildByName("Node_score"):setVisible(2 == sender:getTag())
    end

    --按钮控制
    self.m_tabBoxGame = {}
    local checkbx = self.RoomInfoLayer:getChildByName("CheckBox_1")
    checkbx:setTag(1)
    checkbx:addEventListener(boxCallback)
    checkbx:setSelected(true)
    self.m_tabBoxGame[1] = checkbx

    checkbx = self.RoomInfoLayer:getChildByName("CheckBox_2")
    checkbx:setTag(2)
    checkbx:setSelected(false)
    checkbx:addEventListener(boxCallback)
    self.m_tabBoxGame[2] = checkbx

    local btnClose = self.RoomInfoLayer:getChildByName("btn_close")
    btnClose:addTouchEventListener(function ( sender,eventType )
    	if eventType == ccui.TouchEventType.ended then
    		self.RoomInfoLayer:setVisible(false)
    	end
    end)

    -- 房间信息界面
    --解散按钮
    local btnDismiss = self.RoomInfoLayer:getChildByName("Node_rule1"):getChildByName("btn_dismiss")
        btnDismiss:addTouchEventListener(function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                PriRoom:getInstance():queryDismissRoom(self._scene.m_cbGameStatus, self._scene:onGetSitUserNum())
                self.RoomInfoLayer:setVisible(false)
            end
        end)

    --游戏流水界面
    -- 列表
    local tmpcontent = self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("ListView")
    local contentsize = tmpcontent:getContentSize()
    self.m_tabList = {_data={},index=0}
    self._listView = cc.TableView:create(contentsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    --self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listView:setPosition(tmpcontent:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.RoomInfoLayer:getChildByName("Node_score"))
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()
    --tmpcontent:removeFromParent()
end

function GameViewLayer:showRoomInfo(bIsShow)
    self.RoomInfoLayer:setVisible(bIsShow)

    self:updataScoreList()
    if bIsShow then
    	self:showPriRule(self._scene:getPrivateRoomConfig())
    	if self._scene.cbPlayMode == 0 then
    		--self.RoomInfoLayer:getChildByName("CheckBox_2"):setVisible(false)
    		self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    		self._listView:reloadData()
    	end
    end

end

function GameViewLayer:updateCardType(cardType,isVisible,isMid)
	local typeTxt = self._csbNode:getChildByName("cardType")
	typeTxt:setVisible(false)

	--local typeData = {}
	--if self._scene.m_tabPrivateRoomConfig.cardType == cmd.CARDTYPE_CONFIG.CT_CLASSIC then  --经典模式
		print("=================1")
	local typeData = {0,1,2,3,4,5,6,7,8,9,10,11,12,14,15,18,17,16,13}
	-- elseif self._scene.m_tabPrivateRoomConfig.cardType == cmd.CARDTYPE_CONFIG.CT_ADDTIMES then --疯狂加倍
	-- 	print("=================2")
	-- 	typeData = {0,1,2,3,4,5,6,7,8,9,10,14,15,18,11,12,13,17,16}
	-- end

	if isVisible then
		typeTxt:setTexture(GameViewLayer.RES_PATH.."game/niux/niu"..typeData[cardType+1]..".png")
	end
	if isMid then
		typeTxt:setPosition(675,206)
		typeTxt:setAnchorPoint(cc.p(0.5,0.5))
	else
		typeTxt:setAnchorPoint(cc.p(0,0.5))
		typeTxt:setPosition(888,238)
	end

end


--计算器动画
--1.是否显示 2.是否有动画 
function GameViewLayer:showCalculate(isShow,isAni)
	if self._scene.cbDynamicJoin == 1 then
		return
	end
	

	self.nodeChip:removeAllChildren()

	if self.bSpecialType then  --特殊牌型 只显示牌型按钮
		self:updateButton(false,false)
		self.spriteCalculate:setVisible(false)
		self.btCuo:setVisible(true)
			
		self:updateCardType(self.cbSpecialCardType,true,true)

		return
	else

		self:updateButton(false,true)
		self:showCardType()
	end
	
	self.spriteCalculate:setVisible(false)
				
end

--游戏状态
function GameViewLayer:gameScenePlaying()
	if self._scene.cbDynamicJoin == 0 then
		if self.bSpecialType then 
			self:updateButton(false,false)
			self.btCuo:setVisible(true)
				
			self:updateCardType(self.cbSpecialCardType,true,true)
			return
		end

	    self.spriteCalculate:setVisible(true)
	end
end

--设置底分
function GameViewLayer:setCellScore(cellscore)
	if not cellscore then
		self.txt_CellScore:setString("底注：")
	else
		self.txt_CellScore:setString("底注："..cellscore)
	end
end

--设置纹理
function GameViewLayer:setCardTextureRect(viewId, tag, cardValue, cardColor)
	--print("viewId, tag, cardValue, cardColor",viewId, tag, cardValue, cardColor)
	if viewId < 1 or viewId > cmd.GAME_PLAYER or tag < 1 or tag > 5 then
		print("card texture rect error!")
		return
	end
	if cardValue == nil or  cardColor == nil then --背面牌
		--print("背面牌")
		local card = self.nodeCard[viewId][tag]
		local rectCard = card:getTextureRect()
		rectCard.x = rectCard.width*2
		rectCard.y = rectCard.height*4
		card:setTextureRect(rectCard)
	else
		--特殊处理大小王
		local tempCardValue = cardValue
		if cardValue == 14 then --小王
			tempCardValue = 1
		elseif cardValue == 15 then --大王
			tempCardValue = 2
		end
		local card = self.nodeCard[viewId][tag]
		local rectCard = card:getTextureRect()
		rectCard.x = rectCard.width*(tempCardValue - 1)
		rectCard.y = rectCard.height*cardColor
		card:setTextureRect(rectCard)
	end

end


function GameViewLayer:updateScore(viewId)
	if self.m_tabUserHead[viewId] then
		self.m_tabUserHead[viewId]:updateStatus()
	end	
end

function GameViewLayer:setTableID(id)
	if not id or id == yl.INVALID_TABLE then
		self.txt_TableID:setString("桌号：")
	else
		self.txt_TableID:setString("桌号："..(id + 1))
	end
end




function GameViewLayer:setUserScore(wViewChairId, lScore)
	self.nodePlayer[wViewChairId]:getChildByTag(GameViewLayer.SCORE):setString(lScore)
end

function GameViewLayer:setReadyVisible(wViewChairId, isVisible)
	self.flag_ready[wViewChairId]:setVisible(isVisible)
	if true == isVisible then
		self.flag_openCard[wViewChairId]:setVisible(false)

		if wViewChairId == cmd.MY_VIEWID then
			self.btStart:setVisible(false)
			if self._scene then
        		self._scene:KillGameClockOnStart()
        	end
		end
		
	end
end


--开牌标识
function GameViewLayer:setOpenCardVisible(wViewChairId, isVisible,isOver)
	if not self.flag_openCard[wViewChairId] then
		return
	end

	self.flag_openCard[wViewChairId]:setVisible(false)
	if isOver then
		-- local sprFlag = cc.Sprite:create(GameViewLayer.RES_PATH.."game/Sprite_complete.png")
		-- self.flag_openCard[wViewChairId]:setSpriteFrame(sprFlag:getSpriteFrame())
	    -- 头像搓牌状态
	    for i=1,cmd.GAME_PLAYER do
	        local viewid = self._scene:SwitchViewChairID(i-1)
	        if self.m_tabUserHead[viewid] and wViewChairId == viewid then
	            if self._scene.cbPlayStatus[i] == 1 then
	                self.m_tabUserHead[viewid]:setHeadStatus(cmd.US_CuoWan)
	            end
	        end
	    end
	else
		-- if wViewChairId == cmd.MY_VIEWID then
		-- 	self.flag_openCard[wViewChairId]:setVisible(false)
		-- 	print("摊牌标识是自己")
		-- 	return 
		-- end
		-- local sprFlag = cc.Sprite:create(GameViewLayer.RES_PATH.."game/Sprite_puting.png")
		-- self.flag_openCard[wViewChairId]:setSpriteFrame(sprFlag:getSpriteFrame())

	    -- 头像搓牌状态
	    for i=1,cmd.GAME_PLAYER do
	        local viewid = self._scene:SwitchViewChairID(i-1)
	        print("头像搓牌状态",self.m_tabUserHead[viewid],viewid,wViewChairId,self._scene.cbPlayStatus[i])
	        if self.m_tabUserHead[viewid] and wViewChairId == viewid then
	            if self._scene.cbPlayStatus[i] == 1 then
	                self.m_tabUserHead[viewid]:setHeadStatus(cmd.US_CuoPai)
	            end
	        end
	    end
	end
end


-- 积分按钮
function GameViewLayer:setScoreJetton( tabJetton ,lPlayerBetBtEx)
	if self._scene.cbDynamicJoin == 1 then
		return
	end
	
	self.lUserJettonScore = clone(tabJetton)
	dump(self.lUserJettonScore, "lUserJettonScore is ")
	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    self:showCallBankerMul(0)
    self.nodeChip:removeAllChildren()

    self.btChip = {}
    
    dump(tabJetton,"=================setScoreJetton")
    for i=1,5 do
    	local betNum = 0
    	local breakFlag = false
    	if tabJetton[i] and tabJetton[i] ~= 0 and i ~= 5 then
    		betNum  = tabJetton[i]*self._scene.lCellScore
    		self.lUserJettonScore[i] = tabJetton[i]*self._scene.lCellScore
    	else
    		breakFlag = true
    		 if lPlayerBetBtEx ~= 0 and self._scene.cbPlayMode == 0 and GlobalUserItem.bPrivateRoom  then
    		 	betNum =  lPlayerBetBtEx*self._scene.lCellScore
    		 	self.lUserJettonScore[i] = lPlayerBetBtEx*self._scene.lCellScore
    		 	
    		 end
    	end

    	if betNum ~= 0 then
    		print("betNum=================="..betNum)
	    	self.btChip[i] = ccui.Button:create("common/btn_shot1.png","common/btn_shot1.png","common/btn_shot4.png")
				:move(260 + 170*(i - 1), 224)
				:setTag(TAG_ENUM.BT_CHIP1 + i -1)
				:setVisible(false)
				:addTo(self.nodeChip)
			self.btChip[i]:addTouchEventListener(btcallback)

			-- local label = cc.LabelTTF:create(self:getScoreString(betNum), "fonts/round_body.ttf", 36, cc.size(150,0), cc.TEXT_ALIGNMENT_CENTER) 
        	local label = cc.LabelAtlas:create(self:getScoreString(betNum),"game/btn_num.png",20,28,string.byte("0"))
        				:setAnchorPoint(0.5,0.5)
			        	:setTag(TAG_ENUM.NUM_CHIP)
			        	:addTo(self.btChip[i])
			        	:move(cc.p(self.btChip[i]:getContentSize().width/2,self.btChip[i]:getContentSize().height/2))
		        	-- label:setColor(cc.c3b(128,66,6))

		        	self.btChip[i]:runAction(cc.Sequence:create(
			        	cc.DelayTime:create(0.1*(i-1)),
			        	cc.MoveBy:create(0.1,cc.p(0,25)),
			        	cc.MoveBy:create(0.1,cc.p(0,-15))
			        	))
		if self._scene.m_lMaxTurnScore < betNum and not (GlobalUserItem.bPrivateRoom and self._scene.cbPlayMode == 0) then
			self.btChip[i]:setEnabled(false)
		end
		
	end

	if breakFlag then
		if lPlayerBetBtEx~= 0 and betNum ~= 0 then

			--local point = cc.p(self.btChip[i]:getPosition())
			self.btChip[i]:loadTextureNormal("common/btn_shot3.png")
        			self.btChip[i]:loadTexturePressed("common/btn_shot3.png")
        			--self.btChip[i]:setAnchorPoint(cc.p(0,0.5))
        			--self.btChip[i]:setPosition(point.x -80, point.y)
        			self.btChip[i]:getChildByTag(TAG_ENUM.NUM_CHIP):setColor(cc.c3b(255, 255, 255))
        			
		end

		local index = (betNum == 0) and (5-i +1) or (5-i)
		self.nodeChip:setPositionX(70+index*100)
		break
	end

    end

end

--转换分数
function GameViewLayer:getScoreString( score )
    if type(score) ~= "number" then
        return ""
    end
    local strScore = ""
    if score <= 999999 then
        strScore = strScore..score
    elseif score > 999999 then
        --print("分数转换1", score/10000)
        strScore = strScore..math.floor(score/10000).."万"
    else
        --print("分数转换2", score/100000000)
        strScore = strScore..string.format("%.2f", score/100000000).."亿"
    end
    --print("分数转换", score, strScore)
    return strScore
end

function GameViewLayer:setBankerUser(wViewChairId,isAni)
	print("wViewChairId,isAni",wViewChairId,isAni)
	for i=1,cmd.GAME_PLAYER do
    	local viewid = self._scene:SwitchViewChairID(i-1)
    	if self.m_tabUserHead[viewid] then
    		self.m_tabUserHead[viewid]:showBank(false)
    	end
	end
	--庄家提示
	if self.m_tabUserHead[wViewChairId] then
		if not isAni then
			self.m_tabUserHead[wViewChairId]:showBank(true)
			return
		end

		local sp = cc.Sprite:create("game/Sprite_banker.png")
		self._csbNode:addChild(sp)
		sp:setName("Sprite_banker")
		sp:setPosition(667,375)
  		AudioEngine.playEffect("sound_res/gameStart.mp3")

	    local m_nodeInfo1 = self.m_tabUserHead[wViewChairId].m_nodeInfo1
	    local spBanker = m_nodeInfo1:getChildByName("Sprite_banker")

		local pos2 = cc.p(spBanker:getPositionX(),spBanker:getPositionY())
	    local wpos = m_nodeInfo1:convertToWorldSpace(pos2)
	    local pos = self._csbNode:convertToNodeSpace(wpos)

		sp:runAction(cc.Sequence:create(
			cc.DelayTime:create(1),
			cc.Spawn:create(cc.ScaleTo:create(0.3,0.3), cc.MoveTo:create(0.3, pos)),
			cc.CallFunc:create(function()
				self.m_tabUserHead[wViewChairId]:showBank(true)
				sp:removeFromParent()
			end)
			))
	end
end

function GameViewLayer:setUserTableScore(wViewChairId, lScore)
	if lScore == 0 then
		return
	end
	if self.m_tabUserHead[wViewChairId] then
		self.m_tabUserHead[wViewChairId]:showCallInScore(lScore,true)
	end 
end

--检查牌类型
function GameViewLayer:updateCardPrompt()
	--弹出牌显示，统计和
	local nSumTotal = 0
	local nSumOut = 0
	local nCount = 1
	local bJoker = false  --大小王百搭标识
	--dump(self._scene.cbCardData)
	self.cbCombineCard = {}
	local normalCard = {}
	for i = 1, 5 do
		local nCardValue = self._scene:getMeCardLogicValue(i)
		nSumTotal = nSumTotal + nCardValue
		if self.bCardOut[i] then  --选中的卡牌
	 		if nCount <= 3 then
	 			local  temp = nCardValue
	 			if temp == GameLogic.KingValue then 
	 				temp = 10 
	 				nCardValue = temp
	 				bJoker = true
	 				self.labAtCardPrompt[nCount]:setString("王")
	 			else
	 				self.labAtCardPrompt[nCount]:setString(temp)
	 			end
	 			
	 		end
	 		nCount = nCount + 1
			nSumOut = nSumOut + nCardValue
			table.insert(self.cbCombineCard,self._scene:getMeCardValue(i))
		else
			table.insert(normalCard, i)	
		end
		
	end

	for i = nCount, 3 do
		self.labAtCardPrompt[i]:setString("")
		self.btCuo:setVisible(false)
		self.btShow:setVisible(true)
		self:updateCardType(0,false)
		self:setCombineCard(self._scene:getMeCardValue())
	end

	--判断是否构成牛
	if nCount == 1 then
		self.labCardType:setString("")
		self.btCuo:setVisible(false)
		self.btShow:setVisible(true)
		self:updateCardType(0,false)
		self:setCombineCard(self._scene:getMeCardValue())
	elseif nCount == 3 then 		--弹出两张牌
		self.labCardType:setString("")
		self.btCuo:setVisible(false)
		self.btShow:setVisible(true)
		self:updateCardType(0,false)
		self:setCombineCard(self._scene:getMeCardValue())
	elseif nCount == 4 then 		--弹出三张牌
		if true == bJoker then
			nSumOut = nSumOut - 10
			local mod = math.mod(nSumOut,10)
			nSumOut = nSumOut - mod
			nSumOut = nSumOut + 10
		end
		self.labCardType:setString(nSumOut)

		for i=1,#normalCard do
			local index = normalCard[i]
			table.insert(self.cbCombineCard,self._scene:getMeCardValue(index))
		end
		self.btCuo:setVisible(true)
		self.btShow:setVisible(false)
		local ox_type = GameLogic:getCardType(self.cbCombineCard)
		self:updateCardType(ox_type,true)
		--self.btCuo.text:setString(GameLogic.CT_OX_CLASSIC_LIST[ox_type+1])
		normalCard = {}
	else
		self.labCardType:setString("")
		self.btCuo:setVisible(false)
		self.btShow:setVisible(true)
		self:updateCardType(0,false)
		self:setCombineCard(self._scene:getMeCardValue())
	end
end

function GameViewLayer:preloadUI()
	for i = 1, #AnimationRes do
		local animation = cc.Animation:create()
		animation:setDelayPerUnit(AnimationRes[i].fInterval)
		animation:setLoops(AnimationRes[i].nLoops)

		for j = 1, AnimationRes[i].nCount do
			local strFile = AnimationRes[i].file..string.format("%d.png", j)
			animation:addSpriteFrameWithFile(strFile)
		end

		cc.AnimationCache:getInstance():addAnimation(animation, AnimationRes[i].name)
	end
end

function GameViewLayer:getAnimate(name, bEndRemove)
	print("name",name)
	local animation = cc.AnimationCache:getInstance():getAnimation(name)
	print("animation",animation)
	local animate = cc.Animate:create(animation)

	if bEndRemove then
		animate = cc.Sequence:create(animate, cc.CallFunc:create(function(ref)
			ref:removeFromParent()
		end))
	end

	return animate
end

function GameViewLayer:promptOx()
	--首先将牌复位
	for i = 1, 5 do
		if self.bCardOut[i] == true then
			local card = self.nodeCard[cmd.MY_VIEWID][i]
			local x, y = card:getPosition()
			y = y - 30
			card:move(x, y)
			self.bCardOut[i] = false
		end
	end
	--将牛牌弹出
	local index = self._scene:GetMeChairID() + 1
	local cbDataTemp = self:copyTab(self._scene.cbCardData[index])		

	if GameLogic:getAutoOxCard(cbDataTemp) then
		for i = 1, 3 do
			for j = 1, 5 do
				if self._scene.cbCardData[index][j] == cbDataTemp[i] then
					local card = self.nodeCard[cmd.MY_VIEWID][j]
					local x, y = card:getPosition()
					y = y + 30
					card:move(x, y)
					self.bCardOut[j] = true
				end
			end
		end
		--dump(self.bCardOut,"弹出")
		self:updateCardPrompt()
	end
end


function GameViewLayer:showCardType()
	local index = self._scene:GetMeChairID() + 1
	local cbDataTemp = self:copyTab(self._scene.cbCardData[index])		

	if GameLogic:getAutoOxCard(cbDataTemp) then
		
		self:setCombineCard(cbDataTemp)
		local ox_type = GameLogic:getCardType(self.cbCombineCard)
		self:updateCardType(ox_type,true,true)
		self.btCuo:setVisible(true)
		self.btShow:setVisible(false)
	end

	
end

-- 文本聊天
function GameViewLayer:onUserChat(chatdata, viewId)
    local playerItem = self.m_tabUserHead[viewId]
    print("获取当前显示聊天的玩家头像", playerItem, viewId, chatdata.szChatString)
    if nil ~= playerItem then
        playerItem:textChat(chatdata.szChatString)
           if self._chatLayer then
             self._chatLayer:showGameChat(false)
          end
    end
end

-- 表情聊天
function GameViewLayer:onUserExpression(chatdata, viewId)
    local playerItem = self.m_tabUserHead[viewId]
    if nil ~= playerItem then
        playerItem:browChat(chatdata.wItemIndex)
        --self._chatLayer:showGameChat(false)
    end
end

--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
    --取消文字，表情
    local playerItem = self.m_tabUserHead[viewid]
    dump(self.m_tabUserHead,"显示语音")
    print("显示语音,playerItem",playerItem)
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

--拷贝表
function GameViewLayer:copyTab(st)
    local tab = {}
    for k, v in pairs(st) do
        if type(v) ~= "table" then
            tab[k] = v
        else
            tab[k] = self:copyTab(v)
        end
    end
    return tab
 end

--取模
function GameViewLayer:mod(a,b)
    return a - math.floor(a/b)*b
end

--运行输赢动画
function GameViewLayer:runWinLoseAnimate(viewid, score)

	if not self.m_tabUserHead[viewid] then
		return
	end

	local strNum
	local bg = nil
	if score >= 0 then
		strNum = GameViewLayer.RES_PATH.."game/num_win.png"
		bg = GameViewLayer.RES_PATH.."game/scoreBg2.png"
	else
		score = -score
		strNum = GameViewLayer.RES_PATH.."game/num_lose.png"
		bg = GameViewLayer.RES_PATH.."game/scoreBg1.png"
	end

	local px,py =self.m_tabUserHead[viewid]:getParent():getPosition()

	local bgImg = cc.Sprite:create(bg):move(px,py):addTo(self._csbNode:getChildByName("Node_FlyScore"))
	bgImg:setName("flyScore"..viewid)
	if viewid ==2 or viewid ==1 then
		bgImg:setAnchorPoint(cc.p(0,0.5))
		bgImg:setPositionX(px- 80)
	elseif viewid ==4 or viewid ==5  then
		bgImg:setAnchorPoint(cc.p(1,0.5))
		bgImg:setPositionX(px+ 80)
	elseif viewid >= 6 and viewid <= 8 then
		bgImg:setAnchorPoint(cc.p(1,0.5))
		bgImg:setPositionX(px +80)
		bgImg:setPositionY(py - 40)
	end
	--加减
	local labAtNum = cc.LabelAtlas:_create(string.format("/" .. score), strNum, 29, 37, string.byte("."))		--数字
		:setAnchorPoint(cc.p(0.5, 0.5))
		:addTo(bgImg)
		:move(115,42)

	if viewid ==  7 then
		px = px + 100
		py = py -  45
		bgImg:setPosition(px, py)
	end

	--底部动画
	local nTime = 0.3
	bgImg:runAction(cc.Sequence:create(
		cc.Spawn:create(
			cc.MoveBy:create(nTime, cc.p(0, 50)), 
			cc.FadeIn:create(nTime)
		),
		cc.DelayTime:create(3),
		cc.CallFunc:create(function(ref)
				bgImg:removeFromParent()
				self.bActComplete = true
				if not GlobalUserItem.bVideo and not (GlobalUserItem.bPrivateRoom and PriRoom:getInstance().m_tabPriData.dwPlayCount == PriRoom:getInstance().m_tabPriData.dwDrawCountLimit) then 
					if not self._scene.m_bLookOn then 
						-- self.btStart:setVisible(true)
						-- self:setBtnchangeShow(true)
					end
				end
		end)
	))

end

local scorecell = {100, 1000, 10000, 100000,1000000}
local goldnum = {12, 13, 15, 15, 15 ,18}

--飞出多少金币
function GameViewLayer:getgoldnum(score)
  local cellvalue = 1
  score = math.abs(score)
  for i=1,5 do
    if score > scorecell[i] then
      cellvalue = i + 1
    end
  end
  return goldnum[cellvalue]
end


function GameViewLayer:setClockVisible( visible )
	local node = self._csbNode:getChildByName("clock_node")
  	node:setVisible(visible)
end

function GameViewLayer:setClockBgVisible( visible )
	

	self.bIsTimeOutPlay = visible
	--self._clockTimeBg:setVisible(visible)
end

function GameViewLayer:logicClockInfo(chair,time,clockId)

	-- if true then --GlobalUserItem.bVideo then
	-- 	return
	-- end
	 -- body 
	 if time < 0 then
	 	time = 0
	 end
    local tipTxt = ""
    if clockId == cmd.IDI_NULLITY then
        if time <= 3 then
        	if self._scene.cbDynamicJoin == 0 then
            	self:playEffect("GAME_WARN.WAV")
       		end
        end
    elseif clockId == cmd.IDI_START_GAME then
        if time <= 0 then
        	if self._scene.cbDynamicJoin == 0 and self.bIsTimeOutPlay then
           		--self._scene._gameFrame:setEnterAntiCheatRoom(false)--退出防作弊
           		if not GlobalUserItem.bPrivateRoom or (GlobalUserItem.bPrivateRoom and self._scene.cbPlayMode == 1) then --and self._scene.cbPlayMode ~= 1 then
           		-- 	self:onButtonClickedEvent(TAG_ENUM.BT_AUTO)
           			 
           		-- 	self:autoPlay(self.nAutoStatus)
           		-- else
           		
           			if GlobalUserItem.dwCurRoomGroupID ~= 0 then
           				return;
           			end
           			self._scene:onExitTable()
           		end
        	end
        	if self._scene then
        		self._scene:KillGameClock()
        	end
            
            --self:setClockBgVisible(false)
        elseif time <= 3 then
            self:playEffect("GAME_WARN.WAV")
        end
        tipTxt = "clockTips_4.png"
        if self.bIsTimeOutPlay then
        	tipTxt = "clockTips_9.png"
        end
    elseif clockId == cmd.IDI_CALL_BANKER then
        if time < 1 then
        	if self._scene.cbDynamicJoin == 0  and self.bIsTimeOutPlay then
        		   -- 非私人房处理叫庄
	           -- if not GlobalUserItem.bPrivateRoom then
	            --    self:onButtonClickedEvent(TAG_ENUM.BT_CANCEL)
	            --    print("-----------------不叫庄----------------")
	           -- end


	            --self:onButtonClickedEvent(TAG_ENUM.BT_AUTO)
	            -- if not (GlobalUserItem.bPrivateRoom and self._scene.cbPlayMode == 0) then
	           	 self:autoPlay(self.nAutoStatus)
	           	 self:setClockBgVisible(false)
	        	-- end
        	end	
         
            self._scene:KillGameClock()
            --self:setClockBgVisible(false)
        elseif time <= 3 then
        	if self._scene.cbDynamicJoin == 0 then
           		self:playEffect("GAME_WARN.WAV")
        	end
        end
        tipTxt = "clockTips_1.png"
        if self.bIsTimeOutPlay then
        	tipTxt = "clockTips_5.png"
        end
    elseif clockId == cmd.IDI_TIME_USER_ADD_SCORE   then
        if time < 1 then
        	if self._scene.cbDynamicJoin == 0 and self.bIsTimeOutPlay then
        		--if not GlobalUserItem.bPrivateRoom then
	            	--if self._scene.wBankerUser ~= self._scene._MyChairID then
	            		--self:onButtonClickedEvent(TAG_ENUM.BT_CHIP1)
	            	--end
            	--end
            	--self:onButtonClickedEvent(TAG_ENUM.BT_AUTO)
            	-- if not (GlobalUserItem.bPrivateRoom and self._scene.cbPlayMode == 0) then
            		self:autoPlay(self.nAutoStatus)
            		self:setClockBgVisible(false)
            	-- end
        	end	
            
            self._scene:KillGameClock()
            --self:setClockBgVisible(false)
        elseif time <= 3 then
        	if self._scene.cbDynamicJoin == 0 then
           		self:playEffect("GAME_WARN.WAV")
        	end
        end
        tipTxt = "clockTips_3.png"
        if self.bIsTimeOutPlay then
        	tipTxt = "clockTips_7.png"
        end
    elseif clockId == cmd.IDI_TIME_OPEN_CARD then
        if time < 1 then
        	if self._scene.cbDynamicJoin == 0  and self.bIsTimeOutPlay then
        		-- 非私人房处理摊牌
	           -- if not GlobalUserItem.bPrivateRoom then
	            	--self:setCombineCard(self._scene:getMeCardValue())
	                --self:onButtonClickedEvent(self.btCuo:getTag())
	                --error("open card",0)
	            --end
	            --self:onButtonClickedEvent(TAG_ENUM.BT_AUTO)
	            -- if not (GlobalUserItem.bPrivateRoom and self._scene.cbPlayMode == 0) then
	            	self:autoPlay(self.nAutoStatus)
	            	self:setClockBgVisible(false)
	            -- end
        	end	
            
            self._scene:KillGameClock()
           -- self:setClockBgVisible(false)
        elseif time <= 3 then
        	if self._scene.cbDynamicJoin == 0 then
           		self:playEffect("GAME_WARN.WAV")
        	end
        end
        tipTxt = "clockTips_2.png"
        if self.bIsTimeOutPlay then
        	tipTxt = "clockTips_6.png"
        end
    end

    if self and self._csbNode then
    	local node = self._csbNode:getChildByName("clock_node")
		local clockBg = node:getChildByName("clockBg_24"):setVisible(true)
    	local tips = node:getChildByName("Sprite_13")
    	tips:setTexture("game/"..tipTxt)
    	local width = tips:getContentSize().width
    	local clockNum = node:getChildByName("Text_5"):setString(time.."")
    	if time <=3 then
			clockNum:stopAllActions()
			clockNum:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.RotateTo:create(0.15,-10),cc.RotateTo:create(0.15,10))))
			clockBg:stopAllActions()
			clockBg:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.RotateTo:create(0.15,-10),cc.RotateTo:create(0.15,10))))
		else
			clockNum:rotate(0)
			clockBg:rotate(0)
			clockNum:stopAllActions()
			clockBg:stopAllActions()
		end
    	node:setVisible(true)
		for i = 1, 3 do
    		dot = node:getChildByName("dot_"..i)
    		dot:setPositionX(width/2 - 12 + 10*i)
    	end
    	if node:getNumberOfRunningActions()>0 then
    		return
    	end
     	local dot1 = node:getChildByName("dot_1"):setVisible(false)
    	local dot2 = node:getChildByName("dot_2"):setVisible(false)
    	local dot3 = node:getChildByName("dot_3"):setVisible(false)

       	node:runAction(cc.RepeatForever:create(cc.Sequence:create(
    			cc.DelayTime:create(1),
    			cc.CallFunc:create(function() dot1:setVisible(true) end),
    			cc.DelayTime:create(1),
    			cc.CallFunc:create(function() dot2:setVisible(true) end),
    			cc.DelayTime:create(1),
    			cc.CallFunc:create(function() dot3:setVisible(true) end),
    			cc.DelayTime:create(1),
    			cc.CallFunc:create(function() 
    				dot1:setVisible(false)
    				dot2:setVisible(false)
    				dot3:setVisible(false)
    			end)
    		)))
    end
    
    

end

function GameViewLayer:showWaitOtherTips()
	self._csbNode:getChildByName("clock_node"):setVisible(true)
	local node = self._csbNode:getChildByName("clock_node")
	node:getChildByName("clockBg_24"):setVisible(false)
	local tips = node:getChildByName("Sprite_13")
	tips:setTexture("game/clockTips_8.png")
	local width = tips:getContentSize().width
	local clockNum = node:getChildByName("Text_5"):setString("")
	for i = 1, 3 do
		dot = node:getChildByName("dot_"..i)
		dot:setPositionX(width/2 - 12 + 10*i)
	end
	if node:getNumberOfRunningActions()>0 then
		return
	end
 	local dot1 = node:getChildByName("dot_1"):setVisible(false)
	local dot2 = node:getChildByName("dot_2"):setVisible(false)
	local dot3 = node:getChildByName("dot_3"):setVisible(false)

   	node:runAction(cc.RepeatForever:create(cc.Sequence:create(
			cc.DelayTime:create(1),
			cc.CallFunc:create(function() dot1:setVisible(true) end),
			cc.DelayTime:create(1),
			cc.CallFunc:create(function() dot2:setVisible(true) end),
			cc.DelayTime:create(1),
			cc.CallFunc:create(function() dot3:setVisible(true) end),
			cc.DelayTime:create(1),
			cc.CallFunc:create(function() 
				dot1:setVisible(false)
				dot2:setVisible(false)
				dot3:setVisible(false)
			end)
		)))
end

function GameViewLayer:updataScoreList()
    self.m_tabList = clone(self._scene.m_userPriRecord)
    local scoreList = self.m_tabList

    if 0 < #scoreList then
        self._listView:reloadData()
    end

    local userList ={}
    for i = 1, cmd.GAME_PLAYER do

         local item = self._scene:getUserInfoByChairID(i-1)
                    table.insert(userList,item)
    end
    self:updateDetailNick(userList)
   -- dump(userList,"===============")
end

function GameViewLayer:updateDetailNick( userList )

	if not self.RoomInfoLayer or (#userList == 0) then
		return 
	end

	local num = 0
	for i=1,cmd.GAME_PLAYER do
		local item = userList[i]
		if item then
		
			num = num+1
			if self._scene.cbPlayMode == 0 then
				
				local strNickname = string.EllipsisByConfig(item.szNickName, 74, 
		                                                string.getConfig("fonts/round_body.ttf", 18))

				local nickTTF = self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("txt_title"..num)
					nickTTF:setVisible(true)
					nickTTF:setString(strNickname)
					nickTTF:setPositionX(nickTTF:getPositionX())

				local meBG = self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("Image_"..num)
				meBG:setVisible(false)
				meBG:setPositionX(meBG:getPositionX())


				if item.wChairID == self._scene._MyChairID then 
					meBG:setVisible(true)
				end
			else
				
				if item.wChairID == self._scene._MyChairID then 
					local strNickname = string.EllipsisByConfig(item.szNickName, 74, 
		                                                string.getConfig("fonts/round_body.ttf", 18))

					local nickTTF = self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("txt_title1")
					nickTTF:setVisible(true)
					nickTTF:setString(strNickname)

					local meBG = self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("Image_1")
					local meGoldBg = self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("Image_9")
					if GlobalUserItem.bPrivateGoldRoom then
						nickTTF:setPositionX(163*3)
						self.RoomInfoLayer:getChildByName("Node_score"):getChildByName("txt_title0"):setPositionX(163)
						-- meGoldBg:setVisible(true)
						meBG:setVisible(false)
					else
						meGoldBg:setVisible(false)
						meBG:setVisible(true)
					end
				end
			end
		end
	end
end

-- 子视图大小
function GameViewLayer:cellSizeForTable(view, idx)
    return 650, 61
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


   --分割线
   local spreateLine = cc.Sprite:create("game/info/table_cell_line.png")
   spreateLine:setPosition(view:getContentSize().width/2,61)
   spreateLine:setScaleX(670/spreateLine:getContentSize().width)
   cell:addChild(spreateLine)

    --局数
    local strRound = (idx + 1).."/"..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    if self._scene.cbPlayMode == 1 then
    	strRound = (#self.m_tabList - (idx + 1) +1)..""
    end
    local roundTTF = cc.LabelTTF:create(strRound, "fonts/round_body.ttf", 18, cc.size(60,0), cc.TEXT_ALIGNMENT_CENTER) 
    roundTTF:setAnchorPoint(0.5,0.5)
    roundTTF:setColor(cc.c3b(234, 212, 139))
    roundTTF:setPosition(55,61/2)
   	roundTTF:setTag(1)
   	cell:addChild(roundTTF)


   	local userNum = #self.m_tabList[idx + 1]
   	local record =  self.m_tabList[idx + 1]
   	local num =0
   	for i=1,cmd.GAME_PLAYER do
   		
   		if  self._scene:getUserInfoByChairID(i-1) then
   			num = num+1
   			local scoreStr = tostring(record[i])

	   		if record[i] >=0 then
	   			scoreStr = "+"..scoreStr
	   		end

	   		local color = cc.c3b(206, 178, 125)
	   		if self._scene._MyChairID+1 == i then
	   			if record[i] >=0 then
	   				color = cc.c3b(80, 249, 195)
	   			else
	   				color = cc.c3b(215, 87, 99)	
	   			end
	   			if self._scene.cbPlayMode == 1 then
			   		local scoreTTF = cc.LabelTTF:create(scoreStr, "fonts/round_body.ttf", 22, cc.size(85,0), cc.TEXT_ALIGNMENT_CENTER) 
			   		scoreTTF:setColor(color)
				    	scoreTTF:setAnchorPoint(0.5,0.5)
				    	scoreTTF:setPosition(163*3,61/2)
				   	scoreTTF:setTag(1)
				   	cell:addChild(scoreTTF)
			   		roundTTF:setPositionX(163)
		   		end

	   		end
	   		if self._scene.cbPlayMode == 0 then
		   		local scoreTTF = cc.LabelTTF:create(scoreStr, "fonts/round_body.ttf", 22, cc.size(85,0), cc.TEXT_ALIGNMENT_CENTER) 
		   		scoreTTF:setColor(color)
			    	scoreTTF:setAnchorPoint(0.5,0.5)
			    	scoreTTF:setPosition(130+(num-1)*77,61/2)
			   	scoreTTF:setTag(1)
			   	cell:addChild(scoreTTF)
		   	end


   		end

   		
   	end
    return cell
end

-- 子视图数目
function GameViewLayer:numberOfCellsInTableView(view)
	local scoreList = self.m_tabList
    return #scoreList
end



function GameViewLayer:playEffect( file,sex )
  if not GlobalUserItem.bSoundAble then
    return
  end

  if nil ~= sex then
  	assert((sex==0) or (sex==1))
  	if (sex>1) or (sex<0) then
  		return
  	end
  	local extra = (sex==0) and "w.mp3" or "m.mp3"
  	file = "sound_res/"..file..extra
  else
  	file = "sound_res/"..file
  end

  print("the file is =================================",file)
 
  AudioEngine.playEffect(file)
end

function GameViewLayer:runStartAction(callback)
	print("*******************播放开始动画")
    local action = sp.SkeletonAnimation:create("game/skeleton.json","game/skeleton.atlas")
    action:setAnimation(0, "animation", false)
    self:addChild(action, 100)
    action:setPosition(667, 426)
    action:runAction(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function() action:removeFromParent() end)))

  	AudioEngine.playEffect("sound_res/gameStart.mp3")

    -- action:registerSpineEventHandler(function (event)
    --         -- print(string.format("[spine] %d complete:", event.trackIndex))
    --         action:setVisible(false)
    --         if callback and type(callback) == "function" then callback() end

    -- end,  sp.EventType.ANIMATION_END)
end

function GameViewLayer:showSuperCtrl(userItem)
	self._scene:requestCurDebugConfig(userItem.wChairID)
end

function GameViewLayer:SuperCtrlUpdateCurStatus(data)
	if self.superCtrlLayer then
		self.superCtrlLayer:updateStatus(data)
	else
		local debugUserItem = self._scene:getUserInfoByChairID(data.stConfig.wDebugUserChair)
    	self.superCtrlLayer = SuperControlLayer:create(self, debugUserItem):addTo(self, GameViewLayer.ZORDER_CONTROL)
		self.superCtrlLayer:updateStatus(data)
	end
end

function GameViewLayer:SuperCtrlUpdate(cards, bRequestType)
	if self.superCtrlLayer then
		self.superCtrlLayer:updateHandCard(cards,bRequestType)
	end
end

function GameViewLayer:SuperCtrlOpenCardNode(bCards)
	if self.superCtrlLayer then
		self.superCtrlLayer:showCardNode(bCards)
	end
end

function GameViewLayer:removeSuperCtrl()
	self.superCtrlLayer:removeFromParent()
	self.superCtrlLayer = nil
end

function GameViewLayer:setUserHeadStatus(viewid, status)
	if self.m_tabUserHead[viewid] ~= nil then
		print("status = ",status)
		self.m_tabUserHead[viewid]:setHeadStatus(status)
	end
end

return GameViewLayer