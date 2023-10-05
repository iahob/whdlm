local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)

if not yl then
require("client.src.plaza.models.yl")
end
local GameChat = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local cmd = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.models.CMD_Game")
local SetLayer = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.views.layer.SetLayer")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local CardLayer = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.views.layer.CardLayer")


local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.blackjack.src.models.GameLogic")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local SpineObj = appdf.req("client.src.external.SpineObj")


GameViewLayer.BT_EXIT 				= 1
GameViewLayer.BT_CHAT 				= 2
GameViewLayer.BT_READY				= 4
GameViewLayer.BT_BEGAN				= 5
GameViewLayer.BT_SIT				= 6

GameViewLayer.BT_ADDSCORE				= 200


GameViewLayer.BT_SET				= 14
GameViewLayer.BT_MENU				= 15
GameViewLayer.BT_BANK 				= 16
GameViewLayer.BT_VOICE_ENDED		= 17
GameViewLayer.BT_VOICE_BEGAN		= 18
GameViewLayer.BT_HELP 				= 19

GameViewLayer.BT_RULE               = 36               --规则
GameViewLayer.BT_SCORELIST          = 37               --游戏流水

GameViewLayer.BT_INFO          		= 38               --房间信息

GameViewLayer.BT_SPLITCARD    = 40               --分牌
GameViewLayer.BT_STOPCARD    = 41               --停牌
GameViewLayer.BT_ADDCARD    = 42               --加牌
GameViewLayer.BT_DOUBLE    	= 43               --双倍下注

GameViewLayer.BT_INSUREYES  = 44               --保险
GameViewLayer.BT_INSURENO  = 45               --保险

GameViewLayer.BT_CHANGE  = 46               --换桌

GameViewLayer.BT_CUO	= 47               --搓牌
GameViewLayer.BT_SHOW	= 48               --亮牌
GameViewLayer.BT_LOCATE	= 49               --定位

GameViewLayer.CHIPNUM 				= 100

GameViewLayer.ZORDER_SETTING     = 100

local ptPlayer = {cc.p(120, 85), cc.p(75, 470), cc.p(285, 620), cc.p(635, 660), cc.p(1080, 600),cc.p(1260, 470)}

local ptCard = {cc.p(430, 81), cc.p(185, 385), cc.p(405	, 580), cc.p(675, 540), cc.p(885, 540), cc.p(1050, 385)}
local mySpacingS = 40  --小牌间距
local mySpacingB = 150  --大牌间距
local myCard1ToCard2X = 157+100	--1副牌和2副牌 首座标X差值

local otherSpacingS = 25  --小牌间距
local otherSpacingB = 85  --大牌间距

local ptReady = {cc.p(667, 200), cc.p(75, 380), cc.p(285, 530), cc.p(635, 570), cc.p(1080, 510),cc.p(1260, 380)}
local ptChat = {cc.p(120, 100), cc.p(75, 470), cc.p(285, 620), cc.p(635, 660), cc.p(1080, 600),cc.p(1260, 470)}

local ptEndScore = {cc.p(230, 100), cc.p(75, 400), cc.p(285, 550), cc.p(635, 590), cc.p(1080, 530),cc.p(1260, 400)}

local ptUserInfo =  {cc.p(120, 100), cc.p(75, 470-245), cc.p(285, 620-245), cc.p(635, 660-245), cc.p(1080-508, 600-245),cc.p(1260-508, 470-245)}
local anchorPoint = {cc.p(0, 0), cc.p(0, 1), cc.p(0, 1), cc.p(0, 1), cc.p(1, 1), cc.p(1, 1)}

local ptScore = {cc.p(260, 100), cc.p(215, 450), cc.p(425, 650), cc.p(775, 615), cc.p(1080-140, 615),cc.p(1260-140, 450)}

function GameViewLayer:OnResetView()
	self:stopAllActions()

	

	self.nodeOperation:setVisible(false)
	self.nodeInsure:setVisible(false)
	self.nodeAddScore:setVisible(false)
	self.nodeCuoOrShow:setVisible(false)
    

	self:SetBanker(yl.INVALID_CHAIR)
	--self:SetRoomHolder(yl.INVALID_CHAIR)
	

	self.userCard[cmd.MY_VIEWID].area:setPositionX(myCard1ToCard2X)
	for i = 1 ,cmd.GAME_PLAYER do
		self:SetUserCardType(i)
		self:SetUserCard(i, 0,nil)

		self:SetUserCard(i, 1,nil)
        self:clearCard(i)

        self.m_addScore[i]:setVisible(false)
        self.m_insureScore[i]:setVisible(false)
	end

	self.m_beginTimeNum = 0

end

function GameViewLayer:onExit()
	print("GameViewLayer onExit")
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile(cmd.RES.."game_zjh_res.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey(cmd.RES.."game_zjh_res.png")
	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
	cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

	AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
	self.m_actVoiceAni:release()
	self.m_actVoiceAni = nil

	if self._timeShd then  
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeShd)
		self._timeShd = nil
	end

	if self._timeStatus then  
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeStatus)
		self._timeStatus = nil
	end

	if self._beginTimeId then
		cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
		self._beginTimeId = nil
	end

end

function GameViewLayer:getParentNode()
    return self._scene
end

function GameViewLayer:ctor(scene)
	local this = self

	local function onNodeEvent(event)  
       if "exit" == event then  
            this:onExit()  
        end  
    end  

    self.m_UserChat = {}
  
    self:registerScriptHandler(onNodeEvent)  

	self._scene = scene


	self.m_maxRound = 0
	display.loadSpriteFrames(cmd.RES.."game_zjh_res.plist",cmd.RES.."game_zjh_res.png")

	-- 语音动画
    AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

	--背景显示
	display.newSprite(cmd.RES.."game/game_desk.png")--mark.png
		:move(667,375)
		:addTo(self)

	cc.Sprite:create(cmd.RES.."game/mark.jpg")--cmd.RES.."game/mark.png"
		:setAnchorPoint(1,1)
		:move(1334,750)
		:addTo(self)
		:setLocalZOrder(3)

	--按钮回调
	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.began then
            ExternalFun.popupTouchFilter(1, false)
        elseif type == ccui.TouchEventType.canceled then
            ExternalFun.dismissTouchFilter()
        elseif type == ccui.TouchEventType.ended then
        	ExternalFun.dismissTouchFilter()
			this:OnButtonClickedEvent(ref:getTag(),ref)
        end
    end

    local offx=30

    --搓牌亮牌父节点
	self.nodeCuoOrShow = cc.Node:create()
		:setVisible(false)
		:addTo(self)
	--亮牌
	self.btShow = ccui.Button:create(cmd.RES.."game/btn_show1.png",cmd.RES.."game/btn_show2.png",cmd.RES.."game/btn_show1.png",ccui.TextureResType.localType)
		:move(467 , 200)
		:setEnabled(true)
		:setTag(GameViewLayer.BT_SHOW)
		:addTo(self.nodeCuoOrShow)
	self.btShow:addTouchEventListener(btcallback)

	--搓牌
	self.btCuo = ccui.Button:create(cmd.RES.."game/btn_cuo1.png",cmd.RES.."game/btn_cuo2.png",cmd.RES.."game/btn_cuo1.png",ccui.TextureResType.localType)
		:move(867 , 200)
		:setEnabled(true)
		:setTag(GameViewLayer.BT_CUO)
		:addTo(self.nodeCuoOrShow)
	self.btCuo:addTouchEventListener(btcallback)


	--下注父节点
	self.nodeAddScore = cc.Node:create()
		:setVisible(false)
		:addTo(self)

	self.btAddScore = {}
	self.btScoreLabel = {}


	for i=1 , 4 do
			--下注按钮
		self.btAddScore[i] = ccui.Button:create(cmd.RES.."game/bt_addScore1.png",cmd.RES.."game/bt_addScore2.png",cmd.RES.."game/bt_addScore1.png",ccui.TextureResType.localType)
			:move(167 + (i*200) , 200)
			:setEnabled(true)
			:setTag(GameViewLayer.BT_ADDSCORE+i)
			:addTo(self.nodeAddScore)
		self.btAddScore[i]:addTouchEventListener(btcallback)

		local sprite = display.newSprite(cmd.RES.."game/txt_score.png")
			:move(90 , 38)
			:setTag(11)
			:addTo(self.btAddScore[i])

		self.btScoreLabel[i] = cc.LabelAtlas:create("",cmd.RES.."game/num_score.png",22,29,string.byte("0"))
			:setAnchorPoint(1,0.5)
			:addTo(self.btAddScore[i])
			:setPosition(75,38)
	end

	--操作父节点
	self.nodeOperation = cc.Node:create()
		:setVisible(false)
		:addTo(self)

--分牌
	self.btSplitCard = ccui.Button:create(cmd.RES.."game/bt_splitCard1.png",cmd.RES.."game/bt_splitCard2.png",cmd.RES.."game/bt_splitCard3.png",ccui.TextureResType.localType)
			:move(350 , 200)
			:setEnabled(false)
			:setTag(GameViewLayer.BT_SPLITCARD)
			:addTo(self.nodeOperation)
	self.btSplitCard:addTouchEventListener(btcallback)


	--停牌
	self.btStopCard = ccui.Button:create(cmd.RES.."game/bt_stopCard1.png",cmd.RES.."game/bt_stopCard2.png",cmd.RES.."game/bt_stopCard3.png",ccui.TextureResType.localType)
			:move(561 , 200)
			:setEnabled(false)
			:setTag(GameViewLayer.BT_STOPCARD)
			:addTo(self.nodeOperation)
	self.btStopCard:addTouchEventListener(btcallback)

	--加牌
	self.btAddCard = ccui.Button:create(cmd.RES.."game/bt_addCard1.png",cmd.RES.."game/bt_addCard2.png",cmd.RES.."game/bt_addCard3.png",ccui.TextureResType.localType)
			:move(772 , 200)
			:setEnabled(false)
			:setTag(GameViewLayer.BT_ADDCARD)
			:addTo(self.nodeOperation)
	self.btAddCard:addTouchEventListener(btcallback)


	--双倍下注
	self.btDouble = ccui.Button:create(cmd.RES.."game/bt_double1.png",cmd.RES.."game/bt_double2.png",cmd.RES.."game/bt_double3.png",ccui.TextureResType.localType)
			:move(984 , 200)
			:setEnabled(false)
			:setTag(GameViewLayer.BT_DOUBLE)
			:addTo(self.nodeOperation)
	self.btDouble:addTouchEventListener(btcallback)


	--保险父节点
	self.nodeInsure = cc.Node:create()
		:setVisible(false)
		:addTo(self)

--保险
	self.btInsureYes = ccui.Button:create(cmd.RES.."game/bt_insureYes1.png",cmd.RES.."game/bt_insureYes2.png",cmd.RES.."game/bt_insureYes1.png",ccui.TextureResType.localType)
			:move(467 , 200)
			:setEnabled(true)
			:setTag(GameViewLayer.BT_INSUREYES)
			:addTo(self.nodeInsure)
	self.btInsureYes:addTouchEventListener(btcallback)


	--不保险
	self.btInsureNo = ccui.Button:create(cmd.RES.."game/bt_insureNo1.png",cmd.RES.."game/bt_insureNo2.png",cmd.RES.."game/bt_insureNo1.png",ccui.TextureResType.localType)
			:move(867 , 200)
			:setEnabled(true)
			:setTag(GameViewLayer.BT_INSURENO)
			:addTo(self.nodeInsure)
	self.btInsureNo:addTouchEventListener(btcallback)

	--定位
	self.btLocation = ccui.Button:create("game/bt_location0.png","game/bt_location1.png","game/bt_location0.png")
			:move(1170, 702)
			:setTag(GameViewLayer.BT_LOCATE)
			:addTo(self, 4)
	self.btLocation:addTouchEventListener(btcallback)
	

	--玩家
	self.nodePlayer = {}
	

	self.m_UserHead = {}

	self.txtConfig = string.getConfig("fonts/round_body.ttf" , 20)
	self.MytxtConfig = string.getConfig("fonts/round_body.ttf" , 24)

	--时钟
	self.m_TimeProgress = {}
	self.m_TimeBg = {}

	self.m_addScore = {}
	self.m_insureScore = {}

	for i = 1, cmd.GAME_PLAYER do
		--玩家总节点
		self.nodePlayer[i] = cc.Node:create()
							:move(ptPlayer[i])
							:setVisible(false)
							:addTo(self)

		
		self.m_UserHead[i] = {}
		--玩家背景
		self.m_UserHead[i].bg = display.newSprite(cmd.RES.."game/headBg.png")
			:addTo(self.nodePlayer[i])

		
		
		local scorepos = i == cmd.MY_VIEWID and cc.p(175,69) or cc.p(32,28)

		--昵称
		self.m_UserHead[i].name = cc.Label:createWithTTF("", "fonts/round_body.ttf", 18)
			:move(cc.p(63,130) )
			:setAnchorPoint(cc.p(0.5,0.5))
			:setLineBreakWithoutSpace(false)
			:setColor(cc.c3b(255, 255, 255))
			:addTo(self.m_UserHead[i].bg)
		--游戏币
		self.m_UserHead[i].score = cc.Label:createWithTTF("", "fonts/round_body.ttf", 18)
			:move(cc.p(63,21))
			:setAnchorPoint(cc.p(0.5,0.5))
			:setColor(cc.c3b(250, 250, 0))
			:addTo(self.m_UserHead[i].bg)

		

		self.m_TimeBg[i] = display.newSprite(cmd.RES.."game/game_time_progressBg2.png")
			:addTo(self)
			:move(ptPlayer[i])
			:setVisible(false)

			display.newSprite(cmd.RES.."game/game_time_progressBg1.png")
			:addTo(self.m_TimeBg[i])
			:move(cc.p(65,77))
			:setVisible(true)

		--计时器
		self.m_TimeProgress[i] = cc.ProgressTimer:create(display.newSprite(cmd.RES.."game/game_time_progress.png"))
             :setReverseDirection(true)
             :move(ptPlayer[i])
             :setVisible(false)
             :setPercentage(100)
             :addTo(self)

        --下注分
		self.m_addScore[i] = display.newSprite(cmd.RES.."game/addScoreBg.png")
			:addTo(self)
			:move(cc.p(ptScore[i].x , ptScore[i].y+35))
			:setVisible(false)

		cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
			:move(cc.p(40,15))
			:setAnchorPoint(cc.p(0,0.5))
			:setColor(cc.c3b(255, 232, 142))
			:setTag(10)
			:addTo(self.m_addScore[i])

			--保险分
		self.m_insureScore[i] = display.newSprite(cmd.RES.."game/addScoreBg.png")
			:addTo(self)
			:move(cc.p(ptScore[i].x , ptScore[i].y))
			:setVisible(false)

		cc.Label:createWithTTF("0", "fonts/round_body.ttf", 24)
			:move(cc.p(40,15))
			:setAnchorPoint(cc.p(0,0.5))
			:setColor(cc.c3b(255, 232, 142))
			:setTag(10)
			:addTo(self.m_insureScore[i])
	end
	--手牌显示
	self.userCard = {}
	
	--准备显示
	self.m_flagReady = {}
	--结算漂分
	self.m_endScoreWin = {}
	self.m_endScoreLose = {}

	self.m_cardType1 = {}
	self.m_cardType2 = {}

	self.m_cardBao = {}

	self.m_stopIcon = {}

	self.m_cardTypeFive = {}
	for i = 1, cmd.GAME_PLAYER do
		
		self.m_cardType1[i] = {}
		self.m_cardType2[i] = {}
		self.m_stopIcon[i] = {}

		self.m_cardTypeFive[i] = {}

		self.m_cardBao[i] = {}

		self.userCard[i] = {}
		self.userCard[i].card = {}
		--牌区域
		self.userCard[i].area = cc.Node:create()
			:setVisible(true)
			:addTo(self)

			if i == cmd.MY_VIEWID then
				self.userCard[i].area:setPositionX(myCard1ToCard2X)
			end 
		--牌显示
		for j = 1, 10 do
			local x = 0
			local y = 0
			if i==cmd.MY_VIEWID then
				x = ptCard[i].x + mySpacingS*(j - 1) + (j>5 and mySpacingB or 0)
				y = ptCard[i].y
			else
				x = ptCard[i].x + otherSpacingS*(j>5 and j-6 or j- 1) 
				y = ptCard[i].y - (j>5 and otherSpacingB or 0)
			end
			self.userCard[i].card[j] = display.newSprite(cmd.RES.."game/poker/card_back.png")
					:move(x,y)
					:setVisible(false)
					:addTo(self.userCard[i].area)

			if i == cmd.MY_VIEWID then
				self.userCard[i].card[j]:setScale(0.78)
			else
				self.userCard[i].card[j]:setScale(0.45)
			end
			if j%5 == 0 then
				
				self.m_stopIcon[i][j/5] = display.newSprite(cmd.RES.."game/stopIcon.png")
					:move(x,y)
					:setVisible(false)
					:addTo(self.userCard[i].area)


				self.m_cardBao[i][j/5] = display.newSprite(cmd.RES.."game/bao.png")
					:move(x,y)
					:setVisible(false)
					:addTo(self.userCard[i].area)

				self.m_cardType1[i][j/5] = display.newSprite(cmd.RES.."game/cardType1.png")
					:move(x,y)
					:setVisible(false)
					:addTo(self.userCard[i].area)

				cc.LabelAtlas:create("18",cmd.RES.."game/cardNum1.png",19,31,string.byte("0"))
					:setAnchorPoint(0.5,0.5)
					:setTag(10)
					:addTo(self.m_cardType1[i][j/5])
					:setPosition(cc.p(36,32))
					:setVisible(true)

				self.m_cardType2[i][j/5] = display.newSprite(cmd.RES.."game/cardType2.png")
					:move(x,y)
					:setVisible(false)
					:addTo(self.userCard[i].area)

				cc.LabelAtlas:create("18",cmd.RES.."game/cardNum2.png",19,31,string.byte("0"))
					:setAnchorPoint(0.5,0.5)
					:setTag(10)
					:addTo(self.m_cardType2[i][j/5])
					:setPosition(cc.p(36,32))
					:setVisible(true)

				self.m_cardTypeFive[i][j/5] = display.newSprite(cmd.RES.."game/cardType2.png")
					:move(x,y)
					:setVisible(false)
					:addTo(self.userCard[i].area)

					display.newSprite(cmd.RES.."game/sp_five.png")
						:move(37.5 , 30)
						:addTo(self.m_cardTypeFive[i][j/5])


			end
		end

		--牌类型
		self.userCard[i].cardType = display.newSprite("#card_type_0.png")
			:move(ptCard[i].x +  (i==cmd.MY_VIEWID and 50 or 35), ptCard[i].y- (i == 3 and 32 or 21))
			:setVisible(false)
			:addTo(self.userCard[i].area)

		self.m_flagReady[i] =  display.newSprite(cmd.RES.."game/sp_ready.png")
			:move(ptReady[i])
			:setVisible(false)
			:addTo(self)

		self.m_endScoreWin[i] =  cc.LabelAtlas:create(".8",cmd.RES.."game/endScore_win.png",32,57,string.byte("."))
			:setAnchorPoint(0.5,0.5)
			:addTo(self)
			:setPosition(ptEndScore[i])
			:setVisible(false)



		self.m_endScoreLose[i] =   cc.LabelAtlas:create(".8",cmd.RES.."game/endScore_lose.png",32,57,string.byte("."))
			:setAnchorPoint(0.5,0.5)
			:addTo(self)
			:setPosition(ptEndScore[i])
			:setVisible(false)



		if i ~= cmd.MY_VIEWID then
			self.userCard[i].cardType:setScale(0.7)
		end
				
	end

	
			
	--庄家
	self.m_BankerFlag = display.newSprite(cmd.RES.."game/sp_banker.png")
		:setVisible(false)
		:addTo(self)
	--房主
	self.m_RoomHolderFlag = cc.Sprite:create(cmd.RES.."game/sp_roomHolder.png")
		:setVisible(false)
		:addTo(self)

	

	
			--菜单按钮
	ccui.Button:create(cmd.RES.."game/bt_menu1.png",cmd.RES.."game/bt_menu2.png",cmd.RES.."game/bt_menu1.png",ccui.TextureResType.localType)
		:move(1265,702)
		:setTag(GameViewLayer.BT_MENU)
		:addTo(self,4)
		:addTouchEventListener(btcallback)
	

	--显示菜单
	self.m_bShowMenu = false
	--菜单背景
	self.m_AreaMenu = display.newSprite(cmd.RES.."game/sp_btnBg.png")
		:move(1265,552)
		:setVisible(false)
		:addTo(self, 2)

	
	--设置按钮
	ccui.Button:create(cmd.RES.."game/bt_setting1.png",cmd.RES.."game/bt_setting2.png",cmd.RES.."game/bt_setting1.png",ccui.TextureResType.localType)
		:move(38,245)
		:setTag(GameViewLayer.BT_SET)
		:addTo(self.m_AreaMenu)
		:addTouchEventListener(btcallback)
	-- --帮助按钮
	 ccui.Button:create(cmd.RES.."game/bt_help1.png", cmd.RES.."game/bt_help2.png", cmd.RES.."game/bt_help1.png",ccui.TextureResType.localType)
	 	:move(38, 180)
	 	:setTag(GameViewLayer.BT_HELP)
	 	:addTo(self.m_AreaMenu)
	 	:addTouchEventListener(btcallback)

	 	--换桌按钮
	ccui.Button:create(cmd.RES.."game/bt_changeDesk1.png",cmd.RES.."game/bt_changeDesk2.png",cmd.RES.."game/bt_changeDesk3.png",ccui.TextureResType.localType)
		:move(38,115)
		:setTag(GameViewLayer.BT_CHANGE)
		:setEnabled(true )
		:addTo(self.m_AreaMenu)
		:addTouchEventListener(btcallback)
	
	--退出按钮
	ccui.Button:create(cmd.RES.."game/bt_exit1.png",cmd.RES.."game/bt_exit2.png",cmd.RES.."game/bt_exit1.png",ccui.TextureResType.localType)
		:move(38,50)
		:setTag(GameViewLayer.BT_EXIT)
		:addTo(self.m_AreaMenu)
		:addTouchEventListener(btcallback)

	--准备按钮
	self.btReady = ccui.Button:create(cmd.RES.."game/bt_ready1.png",cmd.RES.."game/bt_ready2.png",cmd.RES.."game/bt_ready1.png",ccui.TextureResType.localType)
		:move(667,200)
		:setVisible(false)
		:setTag(GameViewLayer.BT_READY)
		:addTo(self)
	self.btReady:addTouchEventListener(btcallback)

	--开始按钮
	self.btSitAndBegin = ccui.Button:create(cmd.RES.."game/bt_began1.png",cmd.RES.."game/bt_began2.png",cmd.RES.."game/bt_began1.png",ccui.TextureResType.localType)
		:move(667,200)
		:setVisible(false)
		:setTag(GameViewLayer.BT_BEGAN)
		:addTo(self)
	self.btSitAndBegin:addTouchEventListener(btcallback)

           local strLabel = cc.Label:createWithTTF("(30)", "fonts/round_body.ttf", 25)
                         --:setTextColor(cc.c4b(175,93,54,255))
                         :move(200,40)
                         :setAnchorPoint(cc.p(0.5, 0.5))
                         :setName("timetxt")
                         :addTo(self.btSitAndBegin)
	
	--详情按钮
	ccui.Button:create(cmd.RES.."game/btn_info.png", cmd.RES.."game/btn_info.png", cmd.RES.."game/btn_info.png",ccui.TextureResType.localType)
		:move(75,690)
		:setTag(GameViewLayer.BT_INFO)
		:addTo(self)
		:addTouchEventListener(btcallback)
		
	self:getChildByTag(GameViewLayer.BT_INFO):setVisible(GlobalUserItem.bPrivateRoom)
		

	--缓存聊天
	self.m_UserChatView = {}
	--聊天泡泡
	for i = 1 , cmd.GAME_PLAYER do
		if i <= 4 then
		self.m_UserChatView[i] = display.newSprite(cmd.RES.."game/sp_chatBgL.png"	,{scale9 = true ,capInsets=cc.rect(50, 30, 46, 20)})
			:setAnchorPoint(cc.p(0,0.5))
			:move(ptChat[i])
			:setVisible(false)
			:addTo(self)
		else
		self.m_UserChatView[i] = display.newSprite( cmd.RES.."game/sp_chatBgR.png",{scale9 = true ,capInsets=cc.rect(50, 30, 46, 20)})
			:setAnchorPoint(cc.p(1,0.5))
			:move(ptChat[i])
			:setVisible(false)
			:addTo(self)
		end
	end

	

	--点击事件
	local touch = display.newLayer()
		:setLocalZOrder(10)
		:addTo(self)
	touch:setTouchEnabled(true)
	touch:registerScriptTouchHandler(function(eventType, x, y)
		return this:onTouch(eventType, x, y)
	end)

	
	

	--聊天窗口层
	self.m_GameChat = GameChat:create(scene._gameFrame)
		:setLocalZOrder(10)
        :addTo(self)

    --设置层
	self._setLayer = SetLayer:create(self)
		:addTo(self, 10)

		-- --聊天按钮
	 ccui.Button:create(cmd.RES.."game/bt_chat1.png",cmd.RES.."game/bt_chat2.png",cmd.RES.."game/bt_chat1.png",ccui.TextureResType.localType)
	 	:move(1270,130)
	 	:setTag(GameViewLayer.BT_CHAT)
	 	:addTo(self)
	 	:addTouchEventListener(btcallback)


	-- 语音按钮 gameviewlayer -> gamelayer -> clientscene
	local btnVoice = ccui.Button:create(cmd.RES.."game/bt_voice1.png",cmd.RES.."game/bt_voice2.png",cmd.RES.."game/bt_voice1.png",ccui.TextureResType.localType)
		:move(1270, 220)
		:addTo(self)
	btnVoice:addTouchEventListener(function(ref, eventType)
			if self._scene.m_bLookOn then
			          	showToast(self._scene, "观战中不可使用该功能", 1)
			          	return
		      	end

 		if eventType == ccui.TouchEventType.began then
 			self:getParentNode():getParentNode():startVoiceRecord()
        elseif eventType == ccui.TouchEventType.ended 
        	or eventType == ccui.TouchEventType.canceled then
        	self:getParentNode():getParentNode():stopVoiceRecord()
        end
	end)
    -- 语音动画
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    param.m_strName = cmd.VOICE_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    self.m_actVoiceAni = cc.RepeatForever:create(animate)
    self.m_actVoiceAni:retain()


	--精简
	--初始化房间信息
    self:initRoomInfo()
--自己 107，147      0.78，
--别人 63，87       
--137，185

	--wifi,电池电量，信号
	self:createTool()
end

function GameViewLayer:createTool()
	local csbNode = cc.CSLoader:createNode("wifi/Node_wifi.csb"):addTo(self)
	csbNode:setPositionY(10)
	self.wifi1 = csbNode:getChildByName("Node_wifi")
   	self.wifi4g = csbNode:getChildByName("Node_4g")  
    self.wifi4g:setVisible(false)
    self.wifi1:getChildByName("wifi_4"):setVisible(false)
    self.wifi4g:getChildByName("wifi_4"):setVisible(false)

    self.pTimer100 = csbNode:getChildByName("LoadingBar_battery")

    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if cc.PLATFORM_OS_ANDROID == targetPlatform then
      self._timeStatus = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
         --电量
          local level = math.floor(MultiPlatform:getInstance():getBatteryLevel()+0.5)
          self.pTimer100:setPercent(level)

          --网络
          local netType = MultiPlatform:getInstance():getNetworkType()   --返回值可能为-1,0,1,2  (-1,0)没网 (1)数据网络,(2)WiFi
          local state = MultiPlatform:getInstance():getNetworkStrength() --返回值可能为1,2,3

          if netType == 1 then
                self.wifi4g:setVisible(true)
                self.wifi1:setVisible(false)
          elseif netType == 2 then
                self.wifi4g:setVisible(false)
                self.wifi1:setVisible(true)
          else
                self.wifi4g:setVisible(false)
                self.wifi1:setVisible(false)
          end

          for i=1, 4 do
                if i == state+1 then
                    self.wifi1:getChildByName(string.format("wifi_%d", i)):setVisible(true)
                    self.wifi4g:getChildByName(string.format("wifi_%d", i)):setVisible(true)
                else
                    self.wifi1:getChildByName(string.format("wifi_%d", i)):setVisible(false)
                    self.wifi4g:getChildByName(string.format("wifi_%d", i)):setVisible(false)
                end
          end
      end, 5, false)
    else
      self._timeStatus = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
           --电量
          local level = math.floor(MultiPlatform:getInstance():getBatteryLevel()+0.5)
          self.pTimer100:setPercent(level)

          local netType = MultiPlatform:getInstance():getNetworkType()
          local state = MultiPlatform:getInstance():getNetworkStrength()

          self.wifi1:getChildByName("wifi_4"):setVisible(false)
          self.wifi4g:getChildByName("wifi_4"):setVisible(false)

          if netType == 1 then
                self.wifi4g:setVisible(true)
                self.wifi1:setVisible(false)
          elseif netType == 2 then
                self.wifi4g:setVisible(false)
                self.wifi1:setVisible(true)
          else
                self.wifi4g:setVisible(false)
                self.wifi1:setVisible(false)
          end

          for i=1, 4 do
                if i == state+1 then
                    self.wifi1:getChildByName(string.format("wifi_%d", i)):setVisible(true)
                    self.wifi4g:getChildByName(string.format("wifi_%d", i)):setVisible(true)
                else
                    self.wifi1:getChildByName(string.format("wifi_%d", i)):setVisible(false)
                    self.wifi4g:getChildByName(string.format("wifi_%d", i)):setVisible(false)
                end
          end

      end, 2, false)
    end

    --时间
    self.labelTime = csbNode:getChildByName("Text_time")
    self.labelTime:setString("00:00")
    local function timeUpdate()
      self.labelTime:setString(""..os.date("%H")..":"..os.date("%M"))
    end
    self._timeShd = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 1, false)
end

function GameViewLayer:showAdressLayer()
    local playerList = {}
    local playerNum = 6
    local validPlayerNum = 0
    for i=1,playerNum do
        local viewid = self._scene:SwitchViewChairID(i-1)
        local useritem = self._scene._gameFrame:getTableUserItem(self._scene._gameFrame:GetTableID(), i-1)
        -- local isVisible = self.nodePlayer[viewid]:isVisible()
        if useritem then
            useritem = clone(useritem)
            local playerData = {}
            playerData.uid = useritem.dwUserID
            playerData.viewid = viewid
            table.insert(playerList, playerData)
            validPlayerNum = validPlayerNum + 1
        end
    end
    local uiLocation = require("game.yule.blackjack.src.views.layer.uiLocation")
    local locationObj = uiLocation:ctor(self, playerList, validPlayerNum)
end

--[[
if viewid==cmd.MY_VIEWID then
				x = ptCard[viewid].x + mySpacingS*(i+(index*5) - 1) + (i+(index*5)>5 and mySpacingB or 0)
				y = ptCard[viewid].y
			else
				x = ptCard[viewid].x + otherSpacingS*(i+(index*5)>5 and i+(index*5)-6 or i+(index*5)- 1) 
				y = ptCard[viewid].y - (i+(index*5)>5 and otherSpacingB or 0)
			end
]]
function GameViewLayer:showStopCard(view , index)
	
	self.m_stopIcon[view][index]:setVisible(true)
	local x = 0
	local y = 0
	local cardNum = self._scene.cardCount[view][index]
	if view==cmd.MY_VIEWID then
		x = ptCard[view].x + mySpacingS*(cardNum+(index-1)*5 - 1) + (cardNum+(index-1)*5>5 and mySpacingB or 0)
		y = ptCard[view].y
	else
		x = ptCard[view].x + otherSpacingS*(cardNum-1)
		y = ptCard[view].y - (index>1 and otherSpacingB or 0)
	end

	print("showStopCard",view , index ,x,y)
	self.m_stopIcon[view][index]:setPosition(cc.p(x+(view==cmd.MY_VIEWID and 48 or 27) , y-(view==cmd.MY_VIEWID and 0 or 20)))

	local seq = cc.Sequence:create(cc.DelayTime:create(1.5), cc.CallFunc:create(
			function (node)
				node:setVisible(false)
			end
			))
	self.m_stopIcon[view][index]:runAction(seq)
end

function GameViewLayer:hideMyCardType(index)
	self.m_cardBao[cmd.MY_VIEWID][index]:setVisible(false)

	self.m_cardTypeFive[cmd.MY_VIEWID][index]:setVisible(false)

	self.m_cardType1[cmd.MY_VIEWID][index]:setVisible(false)

	self.m_cardType2[cmd.MY_VIEWID][index]:setVisible(false)
end

function GameViewLayer:showMyCardType(myNowCard)
	local allScore = {0,0}
	local ANum = {0,0}
	local cardNum = {0,0}
	
	 if self._scene.m_cbPlayStatus[cmd.MY_VIEWID] ~= 1 or self._scene.m_bLookOn then 
	 	return
	 end

	for i=1 , 2 do
		for j=1 , #myNowCard[i] do
			local num = GameLogic:getCardValue(myNowCard[i][j])>=10 and 10 or GameLogic:getCardValue(myNowCard[i][j])
			if num==1 then
            	ANum[i] = ANum[i] + 1
        	end
        	cardNum[i] = cardNum[i] + 1
        	allScore[i] = allScore[i] + (num==1 and 11 or num)
		end
		for k=1 , ANum[i] do
        	if allScore[i]>21 then
            	allScore[i] = allScore[i] - 10
        	end
    	end
	end

	self.m_cardBao[cmd.MY_VIEWID][1]:setVisible(false)
	self.m_cardBao[cmd.MY_VIEWID][2]:setVisible(false)

	self.m_cardTypeFive[cmd.MY_VIEWID][1]:setVisible(false)
	self.m_cardTypeFive[cmd.MY_VIEWID][2]:setVisible(false)

	self.m_cardType1[cmd.MY_VIEWID][1]:setVisible(false)
	self.m_cardType1[cmd.MY_VIEWID][2]:setVisible(false)

	self.m_cardType2[cmd.MY_VIEWID][1]:setVisible(false)
	self.m_cardType2[cmd.MY_VIEWID][2]:setVisible(false)

	for index = 1 , 2 do
		for i=1 , #myNowCard[index] do
			local x = ptCard[cmd.MY_VIEWID].x + mySpacingS*(i+(index-1)*5 - 1) + (i+(index-1)*5>5 and mySpacingB or 0)
			local y = ptCard[cmd.MY_VIEWID].y
			if i==1 and allScore[index]>21 then
				self.m_cardBao[cmd.MY_VIEWID][index]:setVisible(true)
				self.m_cardBao[cmd.MY_VIEWID][index]:setPosition(cc.p(x-70 , y+48))
			elseif allScore[index]<=21 and cardNum[index]==5 then
				self.m_cardTypeFive[cmd.MY_VIEWID][index]:setVisible(true)
				self.m_cardTypeFive[cmd.MY_VIEWID][index]:setPosition(cc.p(x+48 , y+48))
			elseif allScore[index]==21  then
				self.m_cardType2[cmd.MY_VIEWID][index]:setVisible(true)
				self.m_cardType2[cmd.MY_VIEWID][index]:setPosition(cc.p(x+48 , y+48))
				self.m_cardType2[cmd.MY_VIEWID][index]:getChildByTag(10):setString(allScore[index])
			else
				self.m_cardType1[cmd.MY_VIEWID][index]:setVisible(true)
				self.m_cardType1[cmd.MY_VIEWID][index]:setPosition(cc.p(x+48 , y+48))
				self.m_cardType1[cmd.MY_VIEWID][index]:getChildByTag(10):setString(allScore[index])
			end
		end
	end

	
end

--更新时钟
function GameViewLayer:OnUpdataClockView(viewid,time)

end

function GameViewLayer:setChipNum(score)
	local t = {1,2,5,10}
	for i=1 , 4 do
		self.btScoreLabel[i]:setString(t[i]*score)
		self.btScoreLabel[i]:setPositionX(75 + self.btScoreLabel[i]:getContentSize().width / 2)

		local sp = self.btAddScore[i]:getChildByTag(11)
		sp:setPositionX(90 + self.btScoreLabel[i]:getContentSize().width / 2)
	end
end




function GameViewLayer:showSitBtn(visible,time)
	if self._scene.m_bLookOn then 
		self.btSitAndBegin:setTag(GameViewLayer.BT_SIT)

		self.btSitAndBegin:loadTextureNormal("game/bt_sit1.png")
                	self.btSitAndBegin:loadTexturePressed("game/bt_sit2.png")
                	self.btSitAndBegin:setVisible(true)

                	self.btSitAndBegin:getChildByName("timetxt"):setVisible(false)
	else
		self.btSitAndBegin:setTag(GameViewLayer.BT_BEGAN)

		self.btSitAndBegin:loadTextureNormal("game/bt_began1.png")
                	self.btSitAndBegin:loadTexturePressed("game/bt_began2.png")		
		if visible then
			if not self._beginTimeId and time then
		                 self.m_beginTimeNum = time
		                 self.btSitAndBegin:getChildByName("timetxt"):setString("("..self.m_beginTimeNum..")")
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
		self.btSitAndBegin:getChildByName("timetxt"):setString("("..self.m_beginTimeNum..")")	
	else
		if self._beginTimeId then
			cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
			self._beginTimeId = nil
		end
	end

end




--更新用户显示
function GameViewLayer:OnUpdateUser(viewid,userItem)
	--dump(userItem , "更新用户显示")
	if not viewid or viewid == yl.INVALID_CHAIR then
		print("OnUpdateUser viewid is nil")
		return
	end
	--print("OnUpdateUser viewid is==============> ", viewid)
	--print(self._scene.isPriOver  ,  GlobalUserItem.bPrivateRoom)
	if self._scene.isPriOver  and  GlobalUserItem.bPrivateRoom then
		return
	end

	self.nodePlayer[viewid]:setVisible(userItem ~= nil)
	--self.m_UserHead[viewid]:setVisible(userItem ~= nil)

	if not userItem then
		if self.m_UserHead[viewid].head then
			self.m_UserHead[viewid].head:setVisible(false)
		end
		self.m_UserHead[viewid].name:setString("")
		self.m_UserHead[viewid].score:setString("")
		self.m_flagReady[viewid]:setVisible(false)

		self.m_TimeBg[viewid]:setVisible(false)

		--计时器
		self.m_TimeProgress[viewid]:setVisible(false)
	else
		self._scene.m_GameUserItem[userItem.wChairID+1] = userItem
		self.nodePlayer[viewid]:setVisible(true)
		self.m_UserHead[viewid].name:setString(string.EllipsisByConfig(userItem.szNickName,viewid == cmd.MY_VIEWID and 150 or 105,viewid == cmd.MY_VIEWID and self.MytxtConfig or self.txtConfig))
		self.m_UserHead[viewid].score:setString(self:changeGoldNum(userItem.lScore))
		
            
	            if GlobalUserItem.bVideo then
	            	self.m_UserHead[viewid].score:setString("0")
	            end
        
		self.m_flagReady[viewid]:setVisible(yl.US_READY == userItem.cbUserStatus)
		if not self.m_UserHead[viewid].head then
			self.m_UserHead[viewid].head = PopupInfoHead:createNormal(userItem, 75)
			local param = {}
		    param.isProp = true     --显示道具互动，百人游戏不显示
		    param.imgPath = "game/popBg.png" --弹出框背景
		    self.m_UserHead[viewid].head:setPropInfo(param)
			self.m_UserHead[viewid].head:move(63, 77)
			self.m_UserHead[viewid].head:enableInfoPop(true, ptUserInfo[viewid], anchorPoint[viewid])
			self.m_UserHead[viewid].head:addTo(self.m_UserHead[viewid].bg)
		else
			self.m_UserHead[viewid].head:updateHead(userItem)
		end
		self.m_UserHead[viewid].head:setVisible(true)
		self:updataUserInfo()

		--判断房主
		--print("设置房主信息", PriRoom, userItem.dwUserID, PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID)
		if PriRoom and GlobalUserItem.bPrivateRoom then
			if userItem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then
				self:SetRoomHolder(viewid)
			end
		end
		
		if viewid ==cmd.MY_VIEWID and   userItem.cbUserStatus >=yl.US_READY then
			self.btReady:setVisible(false)
		end
	end
	self._listView:reloadData()
end


--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = 1
print("赠送道具信息",dwUserID,dwRecvUserID,dwPropID)
        --获取头像坐标
        dump(self.m_UserHead)

        for i=1 , 6 do
        	local userItem = self._scene:getUserInfoByChairID(i)
        	if userItem then
        		local view = self._scene:SwitchViewChairID(i-1)
        		if userItem.dwUserID ==  dwUserID then
                	beginPoint = ptPlayer[view]
            	elseif userItem.dwUserID ==  dwRecvUserID then
                	endPoint = ptPlayer[view]
                	zOrder = 5
            	end
        	end
        end
       -- print("beginPoint ," , beginPoint.x , beginPoint.y)
       -- print("endPoint ," , endPoint.x , endPoint.y)
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

--金币数字处理
function GameViewLayer:changeGoldNum(lScore)
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

    return strRes
end

--屏幕点击
function GameViewLayer:onTouch(eventType, x, y)

	if eventType == "began" then
		--牌型显示判断
		if self.m_bShowMenu == true then
			local rc = self.m_AreaMenu:getBoundingBox()
			if rc then
				if not cc.rectContainsPoint(rc,cc.p(x,y)) then
					self:ShowMenu(false)
					return true
				end
			end
		end
		return false
	elseif eventType == "ended" then
		--取消牌型显示
	end

	return true
end

--庄家显示
function GameViewLayer:SetBanker(viewid)
	if not viewid or viewid == yl.INVALID_CHAIR then
		self.m_BankerFlag:setVisible(false)
		return
	end
	local x
	local y
		x = ptPlayer[viewid].x + 50
		y = ptPlayer[viewid].y +25

	self.m_BankerFlag:setPosition(x, y)
	self.m_BankerFlag:setVisible(true)

	self.m_bankerView = viewid
end

--房主显示
function GameViewLayer:SetRoomHolder(viewid)
	if not viewid or viewid == yl.INVALID_CHAIR then
		self.m_RoomHolderFlag:setVisible(false)
		return
	end
	local x
	local y
		x = ptPlayer[viewid].x - 50
		y = ptPlayer[viewid].y - 25

	self.m_RoomHolderFlag:setPosition(x, y)
	self.m_RoomHolderFlag:setVisible(true)
end


function GameViewLayer:splitCardAnim(splitUserView , index)
	local anIndex = 1
	for i=1 , 6 do
		for j = 1, 10 do
			local card = self.userCard[i].card[j]
			card:stopActionByTag(2)
			
			card:setOpacity(255)
			
		end
	end
	if index==1 or index==2 then
	index = (index-1)*5
	for i = 1, 10 do
		local card = self.userCard[splitUserView].card[i]
		
		if i>index and i<=index+5 then
			local action = cc.Sequence:create(cc.FadeOut:create(0.5), cc.FadeIn:create(0.5))
	    	local repaction = cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(anIndex*0.05), action, cc.DelayTime:create(anIndex*0.05 )))
	    	repaction:setTag(2)
	    	anIndex = anIndex + 1
	    	card:runAction(repaction)
		end
	end
end
end

function GameViewLayer:showGameEnd(score , endCard)

	local action = {} --动画表
	local loseList = {} --输家
	local winList = {} --赢家 

	local bankerViewId = self._scene:SwitchViewChairID(self._scene.m_wBankerUser)

	for i=1 , 6 do
		if self.m_endScoreWin[i] and self.m_endScoreLose[i] and score[i]~=nil then
			local temp = nil
			if score[i]>=0 then
				self.m_endScoreWin[i]:setVisible(true)
				self.m_endScoreLose[i]:setVisible(false)
				self.m_endScoreWin[i]:setString("."..score[i])
				temp = self.m_endScoreWin[i]
			else
				self.m_endScoreWin[i]:setVisible(false)
				self.m_endScoreLose[i]:setVisible(true)
				self.m_endScoreLose[i]:setString("."..math.abs(score[i]))
				temp = self.m_endScoreLose[i]
			end
			temp:stopAllActions()
			temp:setOpacity(255)
			temp:setVisible(true)
			temp:setPosition(ptEndScore[i])
			--local action = cc.Spawn:create(, )
			local seq = cc.Sequence:create(cc.MoveBy:create(2, cc.p(0,50)) , cc.FadeOut:create(3), cc.CallFunc:create(
			function ()
				temp:setVisible(false)
			end
			))
			temp:runAction(seq)
			if bankerViewId~=i then
				if score[i] < 0 then
					table.insert(loseList,i)
				elseif score[i] > 0 then
					table.insert(winList,i)
				end
			end

		end
	end

	for i=1 , cmd.GAME_PLAYER do
		for j=1 , 2 do
			self:SetUserCard(self._scene:SwitchViewChairID(i-1) , j-1 , endCard[(i-1)*2+j])
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
	
	

	print("bankerViewId",bankerViewId)
	dump(ptPlayer[bankerViewId])

	local goldPoint = {cc.p(10,36),cc.p(0,33),cc.p(16,15),cc.p(26,12),cc.p(-8,23),cc.p(-21,16),cc.p(23,0),cc.p(3,7),cc.p(-10,6),cc.p(-8,-10),cc.p(-22,2),cc.p(-32,-9)}
	
	if not GlobalUserItem.bPrivateRoom then
		local actionTime = 0.5
		self._scene:PlaySound(cmd.RES.."sound_res/ADD_GOLD.wav")
		--金币动画
		for i=1,#loseList do
			local viewid = loseList[i]
			local chairid = self._scene:SwitchChairID(viewid)
			local goldnum = 12
		
			for j=1,goldnum do
				local pgold = cc.Sprite:create(cmd.RES.."game/im_fly_gold_1.png")
				pgold:setLocalZOrder(10+12-j)
				local loseBeginpos = cc.p(ptPlayer[viewid].x+goldPoint[j].x,ptPlayer[viewid].y+goldPoint[j].y)
				local bankerPos = getRandPos(ptPlayer[bankerViewId]) or cc.p(0,0)
				pgold:setPosition(loseBeginpos)
				pgold:setVisible(true)
				pgold:runAction(cc.Sequence:create(cc.DelayTime:create(1+j*0.05), 
								self:getMoveActionEx(actionTime,loseBeginpos, bankerPos) ,cc.DelayTime:create(1) , cc.RemoveSelf:create()) )
				self:addChild(pgold, 10)
			end
		end

		--延迟时间
		local winDelayTime = #loseList > 0 and 1.5 or 0
		local bankerPos = getRandPos(ptPlayer[bankerViewId]) or cc.p(0,0)

local this = self
		self:runAction(
		cc.Sequence:create(
		cc.DelayTime:create(winDelayTime),
		cc.CallFunc:create(
			function ()
				this._scene:PlaySound(cmd.RES.."sound_res/ADD_GOLD.wav")
			end
			)))
	
		for i=1,#winList do
			local viewid = winList[i]
			local chairid = self._scene:SwitchChairID(viewid)
			local goldnum = 12
			for j=1,goldnum do
				local pgold = cc.Sprite:create(cmd.RES.."game/im_fly_gold_1.png")
				local winEndPos = getRandPos(ptPlayer[viewid])
				pgold:setLocalZOrder(10+12-j)
				local beganPoint = cc.p(bankerPos.x+goldPoint[j].x,bankerPos.y+goldPoint[j].y)
				pgold:setPosition(beganPoint)
				pgold:setVisible(true)
				pgold:runAction(cc.Sequence:create(cc.DelayTime:create(1+j*0.05+winDelayTime),
								self:getMoveActionEx(actionTime,bankerPos, winEndPos) ,cc.DelayTime:create(1) , cc.RemoveSelf:create()) )
				self:addChild(pgold, 10)
			end
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


--发牌
function GameViewLayer:SendCard(viewid,index,card,fDelay)
	if not viewid or viewid == yl.INVALID_CHAIR then
		return
	end

	local this = self
	local nodeCard = self.userCard[viewid]
	nodeCard.area:setVisible(true)

	if viewid==cmd.MY_VIEWID and not self._scene.isSplitCard[cmd.MY_VIEWID] then
		nodeCard.area:setPositionX(nodeCard.area:getPositionX()-20)
	end

	local x = 0
	local y = 0
	if viewid==cmd.MY_VIEWID then
		x = ptCard[viewid].x + mySpacingS*(index - 1) + (index>5 and mySpacingB or 0)
		y = ptCard[viewid].y
	else
		x = ptCard[viewid].x + otherSpacingS*(index>5 and index-6 or index- 1) 
		y = ptCard[viewid].y - (index>5 and otherSpacingB or 0)
	end

	local spriteCard = nodeCard.card[index]
	spriteCard:stopAllActions()
	spriteCard:setScale(1.0)
	spriteCard:setVisible(true)
	spriteCard:setSpriteFrame(display.newSprite(cmd.RES.."game/poker/card_back.png"):getSpriteFrame())
	spriteCard:move(display.cx, display.cy + 170)
	spriteCard:runAction(
		cc.Sequence:create(
		cc.DelayTime:create(fDelay*0.1),
		cc.CallFunc:create(
			function ()
				this._scene:PlaySound(cmd.RES.."sound_res/CENTER_SEND_CARD.wav")
			end
			),
			cc.Spawn:create(
				cc.ScaleTo:create(0.3,viewid==cmd.MY_VIEWID and 0.78 or 0.45),
				cc.MoveTo:create(0.3, cc.p(x,y))	
							),
			cc.CallFunc:create(
			function (ref)
				print("牌动画执行完 翻开",card)
				if not card or card == 0 or card == 0xff  then
					ref:setSpriteFrame(display.newSprite(cmd.RES.."game/poker/card_back.png"):getSpriteFrame())
				else
					local strCard = string.format(cmd.RES.."game/poker/card_player_%02d.png",card)
					print("SetUserCard", strCard)
					ref:setSpriteFrame(display.newSprite(strCard):getSpriteFrame())
				end
			end
			)
			))

end
--string.format("card_player_%02d.png",cardData[i])

--显示操作框
function GameViewLayer:showOperationNode(isInsure,isShow,delayTime)
	
	if self._scene.m_bLookOn then
		return
	end

	local this = self
	self:runAction(cc.Sequence:create(
		cc.DelayTime:create(delayTime or 0),
		cc.CallFunc:create(
			function ()
				if isInsure and this._scene.m_cbPlayStatus[cmd.MY_VIEWID]==1 then
					this.nodeInsure:setVisible(true)
					this.nodeOperation:setVisible(false)
				elseif  this._scene.m_cbPlayStatus[cmd.MY_VIEWID]==1 then
					this.nodeOperation:setVisible(true)
					this.nodeInsure:setVisible(false)
					this.btSplitCard:setEnabled(isShow[1] or false)
					this.btStopCard:setEnabled(isShow[2] or false)
					this.btAddCard:setEnabled(isShow[3] or false)
					this.btDouble:setEnabled(isShow[4] or false)
				end
			end
							)
			))

end


--清理牌
function GameViewLayer:clearCard(viewid)
	local nodeCard = self.userCard[viewid]
	for i = 1, 10 do
		nodeCard.card[i]:setSpriteFrame(display.newSprite(cmd.RES.."game/poker/card_back.png"):getSpriteFrame())
		nodeCard.card[i]:setVisible(false)
	end
	
end

function GameViewLayer:openOneCard(viewid,opertionCardIndex , cardIndex,cardData)
	if not viewid or viewid == yl.INVALID_CHAIR then
		return
	end
	local scaleMul = viewid == cmd.MY_VIEWID and 0.78 or 0.45

	local card = self.userCard[viewid].card[cardIndex+(opertionCardIndex*5)]
	card:runAction(cc.Sequence:create(
			cc.ScaleTo:create(0.15,0.1,scaleMul),
			cc.CallFunc:create(function ( )
				local cIndex = GameLogic:getCardColor(cardData)*13 + GameLogic:getCardValue(cardData)
				local strCard = string.format(cmd.RES.."game/poker/card_player_%02d.png",cIndex) 
				card:setSpriteFrame(display.newSprite(strCard):getSpriteFrame())
			end),
			cc.ScaleTo:create(0.15,scaleMul,scaleMul)
			))
end

--显示牌值
function GameViewLayer:SetUserCard(viewid,index ,cardData)
	if not viewid or viewid == yl.INVALID_CHAIR then
		return
	end
	

	for i = 1, 5 do
		--self.userCard[viewid].card[i]:stopAllActions()
		if viewid == cmd.MY_VIEWID then
			self.userCard[viewid].card[i+(index*5)]:setScale(0.78)
		else
			self.userCard[viewid].card[i+(index*5)]:setScale(0.45)
		end
		
	end
	--纹理
	if not cardData then
		for i = 1, 5 do
			self.userCard[viewid].card[i+(index*5)]:setSpriteFrame(display.newSprite(cmd.RES.."game/poker/card_back.png"):getSpriteFrame())
			self.userCard[viewid].card[i+(index*5)]:setVisible(false)

			self.m_cardType1[viewid][index+1]:setVisible(false)
			self.m_cardType2[viewid][index+1]:setVisible(false)
			self.m_cardBao[viewid][index+1]:setVisible(false)
			self.m_cardTypeFive[viewid][index+1]:setVisible(false)

			self.m_stopIcon[viewid][index+1]:setVisible(false)
		end
	else
		local allScore = 0
		local ANum = 0
		local cardNum = 0
		for i=1 , 5 do
			local num = GameLogic:getCardValue(cardData[i])>=10 and 10 or GameLogic:getCardValue(cardData[i])
        	if num==1 then
            	ANum = ANum + 1
        	end
        	if num>=1 then
        		cardNum = cardNum + 1
        	end
        	allScore = allScore + (num==1 and 11 or num)
		end

		for i=1 , ANum do
        	if allScore>21 then
            	allScore = allScore - 10
        	end
    	end

    	
--48      27
		for i = 1, 5 do
			local spCard = self.userCard[viewid].card[i+(index*5)]

			local x = 0
			local y = 0
			if viewid==cmd.MY_VIEWID then
				x = ptCard[viewid].x + mySpacingS*(i+(index*5) - 1) + (i+(index*5)>5 and mySpacingB or 0)
				y = ptCard[viewid].y
			else
				x = ptCard[viewid].x + otherSpacingS*(i+(index*5)>5 and i+(index*5)-6 or i+(index*5)- 1) 
				y = ptCard[viewid].y - (i+(index*5)>5 and otherSpacingB or 0)
			end


			if not cardData[i] or cardData[i] == 0 or cardData[i] == 0xff  then
				spCard:setSpriteFrame(display.newSprite(cmd.RES.."game/poker/card_back.png"):getSpriteFrame())
			else
				local cIndex = GameLogic:getCardColor(cardData[i])*13 + GameLogic:getCardValue(cardData[i])
				print(cardData[i] , "=====>",cIndex)
				local strCard = string.format(cmd.RES.."game/poker/card_player_%02d.png",cIndex) 
				
				spCard:setSpriteFrame(display.newSprite(strCard):getSpriteFrame())
				spCard:setVisible(true)

				if i==1 and allScore>21 then
					self.m_cardBao[viewid][index+1]:setVisible(true)
					self.m_cardBao[viewid][index+1]:setPosition(cc.p(x-(viewid==cmd.MY_VIEWID and 70 or 55) , y+(viewid==cmd.MY_VIEWID and 48 or 27)))
				elseif allScore<=21 and cardNum==5 then
					self.m_cardTypeFive[viewid][index+1]:setVisible(true)
					self.m_cardTypeFive[viewid][index+1]:setPosition(cc.p(x+(viewid==cmd.MY_VIEWID and 48 or 27) , y+(viewid==cmd.MY_VIEWID and 48 or 27)))
				elseif allScore==21  then
					self.m_cardType2[viewid][index+1]:setVisible(true)
					self.m_cardType2[viewid][index+1]:setPosition(cc.p(x+(viewid==cmd.MY_VIEWID and 48 or 27) , y+(viewid==cmd.MY_VIEWID and 48 or 27)))
					self.m_cardType2[viewid][index+1]:getChildByTag(10):setString(allScore)
				else
					self.m_cardType1[viewid][index+1]:setVisible(true)
					self.m_cardType1[viewid][index+1]:setPosition(cc.p(x+(viewid==cmd.MY_VIEWID and 48 or 27) , y+(viewid==cmd.MY_VIEWID and 48 or 27)))
					self.m_cardType1[viewid][index+1]:getChildByTag(10):setString(allScore)
				end
			end
			
		end
	end
end

GameViewLayer.RES_CARD_TYPE = {"card_type_0.png","card_type_1.png","card_type_2.png","card_type_3.png","card_type_4.png","card_type_5.png"}
--显示牌类型
function GameViewLayer:SetUserCardType(viewid,cardtype)
	local spriteCardType = self.userCard[viewid].cardType
	if cardtype and cardtype >= 1 and cardtype <= 6 then
		spriteCardType:setSpriteFrame(GameViewLayer.RES_CARD_TYPE[cardtype])
		spriteCardType:setVisible(true)
	else
		spriteCardType:setVisible(false)
	end
end

function GameViewLayer:showHelp()
	local helpLayer = cc.CSLoader:createNode(cmd.RES.."game/IntroduceLayer.csb"):addTo(self, 51)

	local imgBg = helpLayer:getChildByName("image_bg")

	local panel = imgBg:getChildByName("content")
	
	local viewSize = panel:getContentSize()
	local scrollView = ccui.ScrollView:create()
                          :setContentSize(viewSize)
                          :setPosition(panel:getPosition())
                          :setAnchorPoint(panel:getAnchorPoint())
                          :setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
                          :setBounceEnabled(true)
                          :setScrollBarEnabled(false)
                          :addTo(imgBg)
          	local str ="一、游戏简介\n21点又名黑杰克（Blackjack），起源于法国，该游戏由2到6个人玩，使用除大小王之外的52张牌，游戏者的目标是使手中的牌的点数之和不超过21点且尽量大。\n二、游戏规则\n1、点数计算\n#  每张牌都有点数， 2 到 10 的牌的点数就是其牌面的数字； \n#  J 、 Q 、 K 的点数是 10 点；\n#  A 有两种算法， 1 或者 11 ，如果 A 算为 11 时总和大于 21 ，则 A 算为 1 。例如 （A,8） 是 19 点，（A,7,J） 则为 18 点。\n2、黑杰克（BlackJack）\n由两张牌组成，一张 A 带一张 10 点的牌，比其他 21 点的牌大，例如（A,Q）。\n3、要牌\n玩家根据自己手上的点数决定是否要牌，如果要牌，则系统再发一张牌给玩家。如果要牌后总点数超过 21 ，则算爆牌，玩家输掉本轮游戏；若要牌后点数为 21 点，则不能再要；如果要牌后总点数不到 21 点，则玩家可以继续“要牌”“停牌”“加倍”，庄家要牌规则，庄家在低于16点必须要牌，高于17点停止要牌\n4、停牌\n玩家不再要牌，等待其他玩家操作\n5、加倍双倍下注(Double) 如果您已经抽取了两张纸牌，在这两张牌不是“黑杰克”的前提下，如果认为第三张牌可以让您赢过庄家的手牌，您可以要求「双倍押注」。 您的赌注将增加双倍，而您只可以再抽取一张额外纸牌。\n6、爆牌\n当玩家手中牌的总点数超过 21 点的时候则会爆牌，爆牌后无法继续要牌"

           local strLabel = cc.Label:createWithTTF(str, "fonts/round_body.ttf", 25)
                             :setLineBreakWithoutSpace(true)
                             :setMaxLineWidth(viewSize.width)
                             :setTextColor(cc.c4b(175,93,54,255))
                             :setAnchorPoint(cc.p(0.5, 1.0))
                             :addTo(scrollView)

           local labelSize = strLabel:getContentSize()
    	local fHeight = labelSize.height > viewSize.height and labelSize.height or viewSize.height
    	strLabel:setPosition(cc.p(viewSize.width * 0.5+10, fHeight))
    	scrollView:setInnerContainerSize(cc.size(viewSize.width, labelSize.height))


	local closeBtn = helpLayer:getChildByName("image_bg"):getChildByName("btn_close")
	closeBtn:addTouchEventListener(function (ref, eventType)

		if eventType == ccui.TouchEventType.ended then
		    helpLayer:removeFromParent()
		end
	end)


end




--按键响应
function GameViewLayer:OnButtonClickedEvent(tag,ref)

	if tag == GameViewLayer.BT_EXIT then

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
	elseif tag == GameViewLayer.BT_READY then
		if self._scene.m_bLookOn then
		          	return
		end
		self._scene:onStartGame(true)
	elseif tag == GameViewLayer.BT_BEGAN then
		self.btSitAndBegin:setVisible(false)
		self._scene._gameFrame:SendUserBegin()
	elseif tag == GameViewLayer.BT_SIT then
		self._scene._gameFrame.bChangeDesk = true
		self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)
	elseif tag == GameViewLayer.BT_INFO then
		self:showRoomInfo(true)
	elseif tag==GameViewLayer.BT_BETALL then
		self._scene:betAll()
	elseif tag >= GameViewLayer.BT_ADDSCORE+1 and tag <= GameViewLayer.BT_ADDSCORE+4 then
		self._scene:onSendAddScore(tag-GameViewLayer.BT_ADDSCORE)
	elseif tag == GameViewLayer.BT_CHAT then
		if self._scene.m_bLookOn then
		          	showToast(self._scene, "观战中不可使用该功能", 1)
		          	return
		end
		self.m_GameChat:showGameChat(true)
		self:ShowMenu(false)
	elseif tag == GameViewLayer.BT_MENU then
		self:ShowMenu(not self.m_bShowMenu)
	elseif tag == GameViewLayer.BT_HELP then
		self:showHelp()
	elseif tag == GameViewLayer.BT_SPLITCARD then--分牌
		self.nodeOperation:setVisible(false)
		self._scene:onSendSplitCard()
	elseif tag == GameViewLayer.BT_STOPCARD then--停牌
		self.nodeOperation:setVisible(false)
		self._scene:onSendStopCard()
	elseif tag == GameViewLayer.BT_ADDCARD then--加牌
		self.nodeOperation:setVisible(false)
		self._scene:onSendAddCard()
	elseif tag == GameViewLayer.BT_DOUBLE then--双倍下注
		self.nodeOperation:setVisible(false)
		self._scene:onSendDouble()                             
	elseif tag == GameViewLayer.BT_INSUREYES then--保险
		self.nodeInsure:setVisible(false)
		--self.nodeOperation:setVisible(true)
		self._scene:onSendInsure(true)
	elseif tag == GameViewLayer.BT_LOCATE then--定位
		if self._scene.m_bLookOn then
		          	showToast(self._scene, "观战中不可使用该功能", 1)
		          	return
		end
		self:showAdressLayer()
	elseif tag == GameViewLayer.BT_INSURENO then--不保险
		self.nodeInsure:setVisible(false)
		--self.nodeOperation:setVisible(true)
		self._scene:onSendInsure(false)
	elseif tag == GameViewLayer.BT_SET then
		self:ShowMenu(false)
		self._setLayer:showLayer()
	elseif tag == GameViewLayer.BT_CUO then

		self:showCuoLayer(self._scene.myNowCard[self._scene.nowOperationCard[cmd.MY_VIEWID]])

	elseif tag == GameViewLayer.BT_SHOW then

		self:openOneCard(cmd.MY_VIEWID,
						self._scene.nowOperationCard[cmd.MY_VIEWID]-1 , 
						self._scene.cardCount[cmd.MY_VIEWID][self._scene.nowOperationCard[cmd.MY_VIEWID]],
						self._scene.lastSendCard)
		self._scene:cuoCardFinish()
		
	elseif tag == GameViewLayer.BT_CHANGE then
		if self._scene.m_bLookOn then
		          	showToast(self._scene, "观战中不可使用该功能", 1)
		          	return
		end

		if GlobalUserItem.bPrivateRoom == true then
			return
		end

		if self._scene.m_cbGameStatus == cmd.GAME_STATUS_FREE then
			self._scene:OnResetGameEngine()
			self._scene._gameFrame:QueryChangeDesk()
			self:ShowMenu(false)
		else
			showToast(self, "正在游戏中，请先结束游戏", 2)
		end
	end
end

function moveCardEnd()
    local eventListener = cc.EventCustom:new("moveCardEnd")
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

function GameViewLayer:showCuoLayer(card)
	self.CuoLayer = cc.CSLoader:createNode(cmd.RES.."game/CuoLayer.csb"):addTo(self, 50)
	local myCardData = clone(card)

	local ardValue = GameLogic:getCardValue(myCardData[#myCardData])
    local cardColor = GameLogic:getCardColor(myCardData[#myCardData])

	local cardLyaer = PageTurn:create(cmd.RES.."game/bigCard/cardBack.png",cmd.RES.."game/bigCard/card_"..cardColor.."_"..ardValue,cc.p(667,280))
	local panel = self.CuoLayer:getChildByName("Panel_1")

	
	for i=1,#myCardData-1 do
		local ardValue = GameLogic:getCardValue(myCardData[i])
    	local cardColor = GameLogic:getCardColor(myCardData[i])
		local strCard = string.format(cmd.RES.."game/poker/card_player_%02d.png",cardColor*13+ardValue)
		local card = display.newSprite(strCard)
			:move(457+(i-1)*140, 620)
			:addTo(panel)
	end

	local closeBtn = self.CuoLayer:getChildByName("Panel_1"):getChildByName("Button_13")
	closeBtn:addTouchEventListener(function (ref, eventType)

		if eventType == ccui.TouchEventType.ended then
		    self:closeCuoLayer()
		end
	end)

	self.CuoLayer:addChild(cardLyaer,10,10)

	local touchLayer = cc.Layer:create()
	self.CuoLayer:addChild(touchLayer,20,20)

	local finger = cc.Sprite:create(cmd.RES.."game/bigCard/finger.png"):move(667,125):addTo(self.CuoLayer,11)
	finger:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.MoveTo:create(1.2,cc.p(667,400)),cc.MoveTo:create(0.01,cc.p(667,100)))))

	local dispatcher = touchLayer:getEventDispatcher()
      	local listener = cc.EventListenerTouchOneByOne:create()
      		listener:setSwallowTouches(true)
      		--listener:setTouchEnabled(true)
      		listener:registerScriptHandler(function ( touches, event )
      	local point = touches:getLocation()
      
      	

      	if point.x > 378 and point.x < 952 and point.y> 91 and point.y <482 then
      		if finger then
      			finger:removeFromParent()
      			finger = nil
      		end
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
	self:OnButtonClickedEvent(GameViewLayer.BT_SHOW,nil)
	

	local delayTime = 0.01
	if isDelay then
		delayTime = 1.5
	end
	self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ()
			self:closeCuoLayer()
			--self._scene:cuoCardFinish()
		end)))
end

function GameViewLayer:closeCuoLayer()
	if self.CuoLayer then
		self.CuoLayer:removeFromParent()
		self.CuoLayer = nil
	end
	if nil ~= self.m_listener then
	        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
	        self.m_listener = nil
	end
end

function GameViewLayer:ShowMenu(bShow)
	local this = self
	if self.m_bShowMenu ~= bShow then
		self.m_bShowMenu = bShow
		self.m_AreaMenu:stopAllActions()
		if self.m_bShowMenu == true and not self.m_AreaMenu:isVisible() then
			self.m_AreaMenu:setVisible(true)
			
		elseif self.m_AreaMenu:isVisible() == true then
			self.m_AreaMenu:setVisible(false)
			
		end
	end
end

function GameViewLayer:setMenuBtnEnabled(bAble)
	self.m_AreaMenu:getChildByTag(GameViewLayer.BT_SET):setEnabled(bAble)
	-- self.m_AreaMenu:getChildByTag(GameViewLayer.BT_HELP):setEnabled(bAble)
	-- self.m_AreaMenu:getChildByTag(GameViewLayer.BT_CHAT):setEnabled(bAble)
	self.m_AreaMenu:getChildByTag(GameViewLayer.BT_EXIT):setEnabled(bAble)
end



--显示聊天
function GameViewLayer:ShowUserChat(viewid ,message)
	if message and #message > 0 then
		self.m_GameChat:showGameChat(false) --设置聊天不可见，要显示私有房的邀请按钮（如果是房卡模式）
		--取消上次
		if self.m_UserChat[viewid] then
			self.m_UserChat[viewid]:stopAllActions()
			self.m_UserChat[viewid]:removeFromParent()
			self.m_UserChat[viewid] = nil
		end

		--创建label
		local limWidth = 20*12
		local labCountLength = cc.Label:createWithSystemFont(message,"Arial", 20)  
		if labCountLength:getContentSize().width > limWidth then
			self.m_UserChat[viewid] = cc.Label:createWithSystemFont(message,"Arial", 20, cc.size(limWidth, 0))
		else
			self.m_UserChat[viewid] = cc.Label:createWithSystemFont(message,"Arial", 20)
		end
		self.m_UserChat[viewid]:addTo(self)
		if viewid <= 4 then
			self.m_UserChat[viewid]:move(ptChat[viewid].x + 25 , ptChat[viewid].y )
				:setAnchorPoint( cc.p(0, 0.5) )
		else
			self.m_UserChat[viewid]:move(ptChat[viewid].x - 25 , ptChat[viewid].y )
				:setAnchorPoint(cc.p(1, 0.5))
		end
		--改变气泡大小
		self.m_UserChatView[viewid]:setContentSize(self.m_UserChat[viewid]:getContentSize().width+40, self.m_UserChat[viewid]:getContentSize().height + 45)
			:setVisible(true)
		--动作
		self.m_UserChat[viewid]:runAction(cc.Sequence:create(
						cc.DelayTime:create(3),
						cc.CallFunc:create(function()
							self.m_UserChatView[viewid]:setVisible(false)
							self.m_UserChat[viewid]:removeFromParent()
							self.m_UserChat[viewid]=nil
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
	if self.m_UserChat[viewid] then
		self.m_UserChat[viewid]:stopAllActions()
		self.m_UserChat[viewid]:removeFromParent()
		self.m_UserChat[viewid] = nil
	end
	self.m_UserChatView[viewid]:setVisible(false)
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
    

	if animation and spriteFrame then
		self.m_UserChat[viewid] = cc.Sprite:createWithSpriteFrame(spriteFrame)
			:addTo(self)
		self.m_UserChat[viewid]:move(ptPlayer[viewid].x , ptPlayer[viewid].y)
		--self.m_UserChatView[viewid]:setVisible(true)
		--	:setContentSize(90,65)
		self.m_UserChat[viewid]:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
		self.m_UserChat[viewid]:runAction(cc.Sequence:create(
						cc.DelayTime:create(3),
						cc.CallFunc:create(function()
							--self.m_UserChatView[viewid]:setVisible(false)
							self.m_UserChat[viewid]:removeFromParent()
							self.m_UserChat[viewid]=nil
						end)
				))
	end
end

--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
	--取消文字，表情
	if self.m_UserChat[viewid] then
		self.m_UserChat[viewid]:stopAllActions()
		self.m_UserChat[viewid]:removeFromParent()
		self.m_UserChat[viewid] = nil
	end
	self.m_UserChatView[viewid]:stopAllActions()
	self.m_UserChatView[viewid]:removeAllChildren()
	self.m_UserChatView[viewid]:setVisible(isPlay)
	if isPlay == false then

	else
		--创建帧动画
	    -- 聊天表情
	    local sp = display.newSprite("game/zhajinhua_blank.png")

	    sp:setAnchorPoint(cc.p(0.5, 0.5))
		sp:runAction(self.m_actVoiceAni)
		sp:addTo(self.m_UserChatView[viewid])
		sp:setPosition(cc.p(60, 35))
		-- 转向
		if viewid <= 4 then
			sp:setRotation(180)
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

    	if eventType == ccui.TouchEventType.ended then
    	if self.RoomInfoLayer:getChildByName("Node_rule"):isVisible() then
    		if self._scene.m_bLookOn then
		          	showToast(self._scene, "观战中不可使用该功能", 1)
		          	return
		end
    		self.RoomInfoLayer:getChildByName("Node_rule"):setVisible(false)
        	self.RoomInfoLayer:getChildByName("Node_score"):setVisible(true)

        	sender:loadTextureDisabled(cmd.RES.."info/btn_recoad.png")
        	sender:loadTextureNormal(cmd.RES.."info/btn_recoad.png")
        	sender:loadTexturePressed(cmd.RES.."info/btn_recoad.png")

    	else
    		self.RoomInfoLayer:getChildByName("Node_rule"):setVisible(true)
        	self.RoomInfoLayer:getChildByName("Node_score"):setVisible(false)

        	sender:loadTextureDisabled(cmd.RES.."info/btn_rule.png")
        	sender:loadTextureNormal(cmd.RES.."info/btn_rule.png")
        	sender:loadTexturePressed(cmd.RES.."info/btn_rule.png")
    	end
    end
    end
    local btn = self.RoomInfoLayer:getChildByName("Button_change")
    btn:addTouchEventListener(boxCallback)

    

    -- 房间信息界面
    --解散按钮
    local btnDismiss = self.RoomInfoLayer:getChildByName("Node_rule"):getChildByName("btn_dismiss")
    if nil ~= btnDismiss then
        btnDismiss:addTouchEventListener(function(ref, tType)
    	if self._scene.m_bLookOn then
	          	showToast(self._scene, "观战中不可使用该功能", 1)
	          	return
	end
            if tType == ccui.TouchEventType.ended then
                PriRoom:getInstance():queryDismissRoom(self._scene.m_cbGameStatus, self._scene:onGetSitUserNum())
                self.RoomInfoLayer:setVisible(false)
            end
        end)
    end

    --关闭按钮
    local btnClose = self.RoomInfoLayer:getChildByName("Button_close")
    if nil ~= btnClose then
        btnClose:addTouchEventListener(function(ref, tType)
            if tType == ccui.TouchEventType.ended then
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

function GameViewLayer:updateRoomInfo(playerCount , bankerMode)
	
	--1 霸王庄    2 黑杰克
	local nodeRule = self.RoomInfoLayer:getChildByName("Node_rule")
	local pc =  nodeRule:getChildByName("Text_playerCount") 
	local bm =  nodeRule:getChildByName("Text_bankerMode") 
	
	pc:setString(playerCount.."人")
	bm:setString(bankerMode==1 and "霸王庄" or "BJ上庄")
end

function GameViewLayer:showRoomInfo(bIsShow)
	print("showRoomInfo", bIsShow)
    self.RoomInfoLayer:setVisible(bIsShow)
end

function GameViewLayer:updataUserInfo()
	--[[
    local nodeScore = self.RoomInfoLayer:getChildByName("Node_score")
    for i=1,6 do
        local userItem = self._scene:getUserInfoByChairID(i -1)
        local labName = nodeScore:getChildByName("Text_name"..i)
        if nil ~= userItem then
            local strNickname = string.EllipsisByConfig(userItem.szNickName, 90, 
                                string.getConfig("fonts/round_body.ttf", 20))
            labName:setString(strNickname)
            if i -1 == self._scene:GetMeChairID() then
                local posMyBgPosX = {191.5, 329.5, 467.5, 605.5}
                local mybg = nodeScore:getChildByName("myNameBg")
                if nil ~= mybg then
                    mybg:setPositionX(posMyBgPosX[i])
                end
            end
        end
    end
    ]]
end

function GameViewLayer:updataScoreList(scoreList , maxRound)--
    self.m_tabList = clone(scoreList)
    self.m_maxRound = maxRound
    dump(self.m_tabList , "哈哈哈哈")
    if 0 < #self.m_tabList then
        self._listView:reloadData()
    end
end

-- 子视图大小
function GameViewLayer:cellSizeForTable(view, idx)
    return 804, 62
end

function GameViewLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    local csb = cc.CSLoader:createNode(cmd.RES.."info/Node_cell.csb")
    cell:addChild(csb)
    csb:setPosition(343, 31)

    if idx==0 then
    	for i=1, 6 do
        	local userItem = self._scene:getUserInfoByChairID(i)
        	if nil ~= userItem then
	        	local labName = csb:getChildByName("Text_name"..i)
	        	if nil ~= labName then
	            	local strNickname = string.EllipsisByConfig(userItem.szNickName, 90, 
                                string.getConfig("fonts/round_body.ttf", 20))
            		labName:setString(strNickname)
	        	end
	        else
	        	local labName = csb:getChildByName("Text_name"..i)
	        	if nil ~= labName then
            		labName:setString("")
	        	end
	    	end
    	end

    else
    	

    	local strRound = (self.m_maxRound - (idx-1)).."/"..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    	if GlobalUserItem.bPrivateGoldRoom then
			strRound = self.m_maxRound - (idx-1)
		end
		csb:getChildByName("Text_title"):setString(strRound)



    	for i=1 , 6 do
    		local userItem = self._scene:getUserInfoByChairID(i)
    		if userItem then
    			local score = self.m_tabList[i][#self.m_tabList[i] - (idx-1)]
    			if score then
    				local labScore = csb:getChildByName("AtlasLabel_score"..i)
	            	if score>=0 then
	            		labScore:setProperty("/"..math.abs(score) , cmd.RES.."info/num_win.png", 20, 26, "/")
	            	else
	            		labScore:setProperty("/"..math.abs(score) , cmd.RES.."info/num_lose.png", 20, 26, "/")
	            	end
	        	end
    		end
    	end
	end

    
    return cell
end

-- 子视图数目
function GameViewLayer:numberOfCellsInTableView(view)
    return self.m_maxRound+1
end

return GameViewLayer