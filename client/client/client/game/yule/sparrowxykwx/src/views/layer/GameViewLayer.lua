local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.CMD_Game")

local GameViewLayer = class("GameViewLayer",function(scene)
    return cc.CSLoader:createNode(cmd.RES_PATH.."GameScene.csb")
end)

require("client/src/plaza/models/yl")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local GameChatLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.GameLogic")
local CardLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.layer.CardLayer")
local ResultLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.layer.ResultLayer")
local SettingLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.layer.SettingLayer")
local PlayerInfo = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.layer.uiPlayerInfo")
local uiHuTipStatic = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiHuTipStatic")
local actionMgr = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "utils.ActionMgr")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local SpineObj = appdf.req("client.src.external.SpineObj")

local anchorPointHead = {cc.p(1, 1), cc.p(0, 0.5), cc.p(0, 0), cc.p(1, 0.5)}
local posHead = {cc.p(577, 295), cc.p(165, 332), cc.p(166, 257), cc.p(724, 273)}
local posReady = {cc.p(-333, 0), cc.p(135, 0), cc.p(516, -80), cc.p(-134, 0)}
local posPlate = {cc.p(667, 589), cc.p(237, 464), cc.p(667, 174), cc.p(1093, 455)}
local posChat = {cc.p(873, 660), cc.p(229, 558), cc.p(270, 285), cc.p(1095, 528)}


local VOICE_BTN_NAME = "__voice_record_button__"  --语音按钮名字，可以获取语音按钮，控制显示与否

-- GameViewLayer.BT_MENU				= 10 				--按钮开关按钮
-- GameViewLayer.BT_CHAT 				= 11				--聊天按钮
-- GameViewLayer.BT_SET 				= 12				--设置
-- GameViewLayer.CBX_SOUNDOFF 			= 13				--声音开关
-- GameViewLayer.BT_EXIT	 			= 14			--退出按钮
-- GameViewLayer.BT_TRUSTEE 			= 15				--托管按钮
-- GameViewLayer.BT_HOWPLAY 			= 16				--玩法按钮

-- GameViewLayer.BT_START 				= 20				--开始按钮
-- GameViewLayer.NODE_INFO_TAG			= 100				--信息界面

-- GameViewLayer.BT_CHI				= 30				--游戏操作按钮吃
-- GameViewLayer.BT_GANG 				= 31				--游戏操作按钮杠
-- GameViewLayer.BT_PENG				= 32				--游戏操作按钮碰
-- GameViewLayer.BT_HU 				= 33				--游戏操作按钮胡
-- GameViewLayer.BT_GUO				= 34				--游戏操作按钮过
-- GameViewLayer.BT_TING				= 35				--游戏操作按钮听
GameViewLayer.BT_RULE               = 36               --规则
GameViewLayer.BT_SCORELIST          = 37               --游戏流水
GameViewLayer.BT_CHANGEDESK			=	38

GameViewLayer.ZORDER_OUTCARD = 38
GameViewLayer.ZORDER_ACTION = 50
GameViewLayer.ZORDER_CHAT = 60
GameViewLayer.ZORDER_SETTING = 70
GameViewLayer.ZORDER_INFO = 90
GameViewLayer.ZORDER_RESULT = 100

GameViewLayer.OP_TYPE_PENG = 1
GameViewLayer.OP_TYPE_GANG = 2
GameViewLayer.OP_TYPE_HU = 3
GameViewLayer.OP_TYPE_GUO = 4
GameViewLayer.OP_TYPE_SHOW = 5
GameViewLayer.OP_TYPE_DISCARD = 6

GameViewLayer.BT_SIT = 200
GameViewLayer.BT_BEGIN = 201


function GameViewLayer:onInitData()
    print("GameViewLayer:onInitData================================")
	self.cbActionCard = 0
	self.cbOutCardTemp = 0
	self.bListenBtnEnabled = false
	self.chatDetails = {}
	self.cbAppearCardIndex = {}
	self.bChoosingHu = false
	self.m_bNormalState = {}
	self.m_nLeftCard = 0
    self.m_nAllCard = 0

	-- 用户头像
    self.m_tabUserHead = {}

	--房卡需要
	self.m_UserItem = {}

	--回放
	self.bIsVideo = false
	-- 语音动画
    AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

    self.isGameOverObjDestroy = false
end

function GameViewLayer:onResetData()
	self._cardLayer:onResetData()
	self:stopAllActions()
	self.bChoosingHu = false
	self.cbOutCardTemp = 0
	self.cbAppearCardIndex = {}
	self.m_nLeftCard = 0
            self.nCardLeft = 84 --self.m_nAllCard

	-- self.TrustShadow:setVisible(false)
            --self.btStart:setVisible(false)

	self.labelCardLeft:setString(string.format("%d", self.m_nLeftCard))
	self:ShowGameBtn(GameLogic.WIK_NULL)

	for i=1,cmd.GAME_PLAYER do
        local userItem = self._scene:getUserInfoByChairID(i)
        if userItem then
            local viewid = self._scene:SwitchViewChairID(i-1)
            local seatObj = self.m_tabUserHead[viewid]
            -- dump(userItem, "#useritem" .. "_" .. i)
            -- print("seatObj====", seatObj)
    		if nil ~= seatObj  then
                seatObj:showBank(false)
    			seatObj:setIsReady(userItem.cbUserStatus == 3)
    		end
        end
	end

	self:showOutCard(nil, nil, false)
	self._cardLayer:onResetData()
    self._cardLayer:setBanOutList(self._scene:getBanOutList())

	self:resetMagicInfo()

    if self._objHuTip then
        self._objHuTip:destroy()
        self._objHuTip = nil
    end

    if self.uiShowOutObj then
        self.uiShowOutObj:hide()
    end

    if self._scene.endLayer and tolua.isnull(self._scene.endLayer) then 
            self._scene.endLayer:hide()
            self._scene.endLayer = nil
    end

    self.isGameOverObjDestroy = false
    self._scene.m_bGameEnd  = false

    self:setPiaoVisible(false)

    self:hideAllShowOutTip()
    print("GameViewLayer:onResetData================================")
end

function GameViewLayer:onExit()
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("gameScene.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("gameScene.png")
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    self.m_UserItem = {}
    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)

    if self._objHuTip then
        self._objHuTip:destroy()
        self._objHuTip = nil
    end

    if self.schPoolObj then
        self.schPoolObj:clearAll()
        self.schPoolObj = nil
    end

    if self.objFinalReport then
        self.objFinalReport:destroy()
        self.objFinalReport = nil
    end

    if self.gameOverObj then
        self.gameOverObj:destroy()
        self.gameOverObj = nil
    end

    if self.uiShowOutObj then
        self.uiShowOutObj:destroy()
        self.uiShowOutObj = nil
    end

    if self._beginTimeId then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
        self._beginTimeId = nil
    end

    self.isGameOverObjDestroy = false
end

function GameViewLayer:getParentNode()
    return self._scene
end

function GameViewLayer:showShowOutTip(viewid)
    cc.find(self, "Node_HuTip/sp_" .. (viewid-1)):setVisible(true)
end

function GameViewLayer:showHuTipStatic(viewid, cardValues)
    -- uiHuTipStatic:ctor(self.huNodeTip, {34,35,37}, 2)
    -- if viewid and viewid ~= cmd.MY_VIEWID then
        uiHuTipStatic:ctor(self.huNodeTip, cardValues, viewid)
    -- end
end

function GameViewLayer:hideAllShowOutTip()
    --print("GameViewLayer:hideAllShowOutTip===============================================================")
    for i=1,cmd.GAME_PLAYER do
        cc.find(self, "Node_HuTip/sp_" .. (i-1)):setVisible(false)
        local child = self.huNodeTip:getChildByTag(i)
        if child then
            child:removeFromParent()
        end
    end
end

function GameViewLayer:ctor(scene)
	self._scene = scene
	self:onInitData()
    self:initButtons()
	self:preloadUI()
	self:initUserInfo()
	self._cardLayer = CardLayer:create(self):addTo(self:getChildByName("Node_MaJong"))					--牌图层
    self._chatLayer = GameChatLayer:create(self._scene._gameFrame):addTo(self, GameViewLayer.ZORDER_CHAT)	--聊天框
    -- self._resultLayer = ResultLayer:create(self):addTo(self, GameViewLayer.ZORDER_RESULT):setVisible(false)  --结算框

    self.labelClock = cc.find(self, "sp_clock/Sprite_1/AsLab_time")
    cc.find(self, "img_menu_popout"):setVisible(false)   --右上角菜单
    -- cc.find(self, "Sprite_singal_bg"):setVisible(false)  --左上角信号
    cc.find(self, "FileNode_Op"):setVisible(false)       --操作按钮
    -- cc.find(self, "bt_location"):setVisible(false)       --操作按钮
    cc.find(self, "sp_hand_off"):setVisible(false)       --托管
    cc.find(self, "node_piao"):setVisible(false)       --托管

    
    self.labelCardLeft = cc.find(self, "Sprite_singal_bg/AtlasLabel_1")

    self.huNodeTip = cc.find(self, "Node_HuTip")

    --出牌界面
    self.sprOutCardBg = cc.Sprite:create(cmd.RES_PATH.."kwx_img_discard_bg.png"):addTo(self, GameViewLayer.ZORDER_OUTCARD)
    self.sprOutCardBg:setVisible(false)
    self.sprMajong = self._cardLayer:createMyActiveCardSprite(0x35, false):addTo(self.sprOutCardBg)
    self.sprMajong:setPosition(self.sprOutCardBg:getContentSize().width/2, self.sprOutCardBg:getContentSize().height/2)

    --播放背景音乐
    if GlobalUserItem.bVoiceAble == true then
        AudioEngine.playMusic("sound/backgroud.mp3", true)
	end

    --玩家胡牌提示
    self.nodeTips = cc.Node:create()
    self.nodeTips:addTo(self, GameViewLayer.ZORDER_ACTION)
    self.nodeTips:setPosition(cc.p(667, 215))

    --初始化房间信息
    self:initRoomInfo()
    self:hideAllShowOutTip()



    -- 电量,时间，信号
    -- local leftTopBg = self:getChildByName("leftTopBg")
    -- local BatteryBg = leftTopBg:getChildByName("battery")
    -- local sp = cc.Sprite:create("game/batteryBar.png")
    -- self.batteryBar = cc.ProgressTimer:create(sp)
    -- self.batteryBar:setType(cc.PROGRESS_TIMER_TYPE_BAR)
    -- self.batteryBar:setMidpoint(cc.p(0,0.5))
    -- self.batteryBar:setBarChangeRate(cc.p(1, 0))
    -- self.batteryBar:setPercentage(MultiPlatform:getInstance():getBatteryLevel())
    -- self.batteryBar:setPosition(BatteryBg:getContentSize().width/2 - 1 , BatteryBg:getContentSize().height/2 + 3)
    -- BatteryBg:addChild(self.batteryBar)

    cc.find(self, "Sprite_singal_bg"):runAction(cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(3), cc.CallFunc:create(function()self:updateFunc()end))))
    self:updateFunc()


end

function GameViewLayer:updateFunc()
    cc.find(self, "Sprite_singal_bg/Text_time"):setString(os.date("%H:%M"))

    local spBattery = cc.find(self, "Sprite_singal_bg/Sprite_singal_1")
    local numBayyery = MultiPlatform:getInstance():getBatteryLevel()
    local fileName = {0,20,40,50,60,80,100}
    local filePath = cmd.RES_PATH.."singal/kwx_battery_100.png"
    for i=1 , 7 do
        if numBayyery>=fileName[i] then
            filePath = cmd.RES_PATH.."singal/kwx_battery_"..fileName[i]..".png"
        end
    end
    spBattery:setSpriteFrame( cc.Sprite:create(filePath):getSpriteFrame() )
    cc.find(self, "Sprite_singal_bg/Text_1"):setString(numBayyery.."%")
 
    local Strength = MultiPlatform:getInstance():getNetworkStrength()
    filePath = cmd.RES_PATH.."singal/kwx_sig_"..Strength..".png"
    cc.find(self, "Sprite_singal_bg/Sprite_singal"):setSpriteFrame( cc.Sprite:create(filePath):getSpriteFrame() )
end

function GameViewLayer:preloadUI()
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
end

--初始化玩家信息
function GameViewLayer:initUserInfo()
    print("GameViewLayer:initUserInfo===================")
	local nodeName = {"FileNode_1","FileNode_2","FileNode_3"}
	local faceNode = self:getChildByName("Node_User")

	self.nodePlayer = {}
	for i = 1, cmd.GAME_PLAYER do
		self.nodePlayer[i] = faceNode:getChildByName(nodeName[i])
        self.nodePlayer[i]:setVisible(true)
	end
end

--更新玩家准备状态
function GameViewLayer:showUserState(viewid, isReady)
    local seatObj = self.m_tabUserHead[viewid]
    if seatObj ~= nil then
        seatObj:setIsReady(isReady)
    end
end

function GameViewLayer:onClickStart(sender)
    -- print("GameViewLayer:onClickStart=====")
    --sender:setVisible(false)
    self._scene:sendGameStart()
    --倒计时隐藏
    cc.find(self, "sp_clock"):setVisible(false)
end



function GameViewLayer:onClickSitOrBegin(sender)

        if sender:getTag() == GameViewLayer.BT_SIT then 
            self._scene._gameFrame.bChangeDesk = true
            self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)               
        elseif sender:getTag() == GameViewLayer.BT_BEGIN then 
            self.btSitAndBegin:setVisible(false)
            self._scene._gameFrame:SendUserBegin()
        end

end


function GameViewLayer:onClickMenu()
    local imgMenu = cc.find(self, "img_menu_popout")
    self:setMenuVisible(not imgMenu:isVisible())

    -- local uiActivity = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiActivity")
    -- local obj = uiActivity:ctor(self)

    -- local uiHuTipStatic = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiHuTipStatic")
    -- local obj = uiHuTipStatic:ctor(self.huNodeTip, {34,35,37}, 2)

    -- local uiFinalReport = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiFinalReport")
    -- local obj = uiFinalReport:ctor(self)

    -- local uiRoomRule = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiRoomRule")
    -- local obj = uiRoomRule:ctor(self)
    -- obj:refresh({channel="半频道",maMode="亮倒自摸买马", maNum="买一送一"})
end

function GameViewLayer:onClickLocation()
    if self._scene.m_bLookOn then
            showToast(self, "观战中不可使用该功能", 1)
            return
    end
    local playerList = {}
    local playerNum = self._scene:getPlayerNum()
    for i=1,playerNum do
        local viewid = self._scene:SwitchViewChairID(i-1)
        local useritem = self.m_UserItem[viewid]
        if useritem then
            useritem = clone(useritem)
            local playerData = {}
            playerData.uid = useritem.dwUserID
            playerData.viewid = viewid
            table.insert(playerList, playerData)
        end
    end

    local uiLocation = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiLocation")
    local locationObj = uiLocation:ctor(self, playerList, playerNum)
end

function GameViewLayer:onClickChat()
    if self._scene.m_bLookOn then
            showToast(self, "观战中不可使用该功能", 1)
            return
    end
    self._chatLayer:showGameChat(true)
    self:setMenuVisible(false)
end

--吃碰杠听胡过1,2,3,4,5,6
function GameViewLayer:onClickOpBtns(btnType)
    print("GameViewLayer:onClickOpBtns=====", btnType)

    if btnType == 1 then --碰
        local cbOperateCard = {self.cbActionCard, self.cbActionCard, self.cbActionCard}
        self._scene:sendOperateCard(GameLogic.WIK_PENG, cbOperateCard)
    elseif btnType == 2 then --杠
        if self._scene.wCurrentUser == self._scene:GetMeChairID() then
            local cardGang = self._cardLayer:findUserGangCard()
            if 1 < #cardGang then
                --进入选牌模式
                self._cardLayer:showSelectCardModel(cardGang, cmd.SELECT_CARD_GANG)
            else
                if 1 == #cardGang then
                    self.cbActionCard = cardGang[1]
                end
                local cbOperateCard = {self.cbActionCard, self.cbActionCard, self.cbActionCard}
                self._scene:sendOperateCard(GameLogic.WIK_GANG, cbOperateCard)
            end
        else
            local cbOperateCard = {self.cbActionCard, self.cbActionCard, self.cbActionCard}
            self._scene:sendOperateCard(GameLogic.WIK_GANG, cbOperateCard)
        end
    elseif btnType == 3 then --胡
        local cbOperateCard = {self.cbActionCard, 0, 0}
        self._scene:sendOperateCard(GameLogic.WIK_CHI_HU, cbOperateCard)
    elseif btnType == 4 then --过
        if not self.bListenBtnEnabled and not self._cardLayer.bChoosingOutCard and not self.bChoosingHu then
            local cbOperateCard = {0, 0, 0}
            self._scene:sendOperateCard(GameLogic.WIK_NULL, cbOperateCard)
        end
    elseif btnType == 5 then --亮倒
        print("选牌亮倒")
        self:onClickShowCards()
    elseif btnType == 6 then --弃
        print("放弃")
    end
    self:ShowGameBtn(GameLogic.WIK_NULL)
end

function GameViewLayer:onClickPiao(type)
    print("用户叫漂==", type)
    self._scene:sendUserPiao(type)
    self:setPiaoVisible(false)
end

function GameViewLayer:getPreActionMask()
    return self._pre_action_mask
end

function GameViewLayer:onClickShowCards()
    local uiShowOut = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiShowOut")

    if not self.uiShowOutObj then
        self.uiShowOutObj = uiShowOut:ctor(self, self._cardLayer, self._scene, function()
            self.uiShowOutObj = nil
        end)
    end
end

function GameViewLayer:onClickSettingSet(sender)
    print("GameViewLayer:onClickSettingSet=====")
    sender:getParent():setVisible(false)

    local set = SettingLayer:create( self )
    self:addChild(set, GameViewLayer.ZORDER_SETTING)
end

function GameViewLayer:onClickSettingHelp(sender)
    print("GameViewLayer:onClickSettingHelp=====")
    sender:getParent():setVisible(false)

    local uiHelp = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiHelp")
    uiHelp:ctor(self)
end

function GameViewLayer:onClickSettingQuit(sender)
    print("GameViewLayer:onClickSettingQuit=====")

    sender:getParent():setVisible(false)
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

end

function GameViewLayer:onClickTrust()
    print("onClickTrust")
    if self._scene.m_bLookOn then
            showToast(self, "观战中不可使用该功能", 1)
            return
    end

    self._scene:sendUserTrustee()
end

function GameViewLayer:onClickBackground()

    local imgMenu = cc.find(self, "img_menu_popout")
    if imgMenu:isVisible() then
        self:setMenuVisible(false)
    end
end

function GameViewLayer:setPiaoVisible(isVisible)
    cc.find(self, "node_piao"):setVisible(isVisible)
end

function GameViewLayer:setTrustShadowVisible(isVisible)
    cc.find(self, "sp_hand_off"):setVisible(isVisible)       --托管
end

--初始化界面上button
function GameViewLayer:initButtons()
    local bg = cc.funAddChildListener(self, "background_sparrowHz", function()  self:onClickBackground() end)
    bg:setTouchEnabled(true)

    cc.funAddChildListener(self, "node_piao/Button_1", function() self:onClickPiao(0) end)
    cc.funAddChildListener(self, "node_piao/Button_2", function() self:onClickPiao(1) end)
    cc.funAddChildListener(self, "node_piao/Button_3", function() self:onClickPiao(2) end)

    local btn_peng = cc.funAddChildListener(self, "FileNode_Op/Button_peng", function()  self:onClickOpBtns(1) end)
    local btn_gang = cc.funAddChildListener(self, "FileNode_Op/Button_gang", function() self:onClickOpBtns(2) end)
    local btn_hu = cc.funAddChildListener(self, "FileNode_Op/Button_hu", function() self:onClickOpBtns(3) end)
    local btn_guo = cc.funAddChildListener(self, "FileNode_Op/Button_guo", function() self:onClickOpBtns(4) end)
    local btn_show = cc.funAddChildListener(self, "FileNode_Op/Button_show", function() self:onClickOpBtns(5) end)
    local btn_discard = cc.funAddChildListener(self, "FileNode_Op/Button_discard", function() self:onClickOpBtns(6) end)

    self.optBtnList = {
        [self.OP_TYPE_PENG] = btn_peng,
        [self.OP_TYPE_GANG] = btn_gang,
        [self.OP_TYPE_HU] = btn_hu,
        [self.OP_TYPE_GUO] = btn_guo,
        [self.OP_TYPE_SHOW] = btn_show,
        [self.OP_TYPE_DISCARD] = btn_discard,
    }

    cc.funAddChildListener(self, "btn_chat", function() self:onClickChat() end)
    cc.funAddChildListener(self, "bt_menu", function() self:onClickMenu() end)
    cc.funAddChildListener(self, "bt_location", function() self:onClickLocation() end)
    cc.funAddChildListener(self, "img_menu_popout/btn_setting", function(sender) self:onClickSettingSet(sender) end)
    cc.funAddChildListener(self, "img_menu_popout/btn_help", function(sender) self:onClickSettingHelp(sender) end)
    cc.funAddChildListener(self, "img_menu_popout/btn_quit", function(sender) self:onClickSettingQuit(sender) end)
    cc.funAddChildListener(self, "sp_hand_off/Button_handoff", function(sender) self:onClickTrust(sender) end)
    self.btStart = cc.funAddChildListener(self, "bt_ready", function(sender) self:onClickStart(sender) end)
    self.btStart:setLocalZOrder(self.ZORDER_OUTCARD+1)

    self.btSitAndBegin = cc.funAddChildListener(self, "bt_start", function(sender) self:onClickSitOrBegin(sender) end)
    self.btSitAndBegin:setLocalZOrder(self.ZORDER_OUTCARD+1)
    self.btSitAndBegin:setVisible(false)    

    cc.funAddTouchEventListener(self, "btn_voice", function(_,eventType)
        if self._scene.m_bLookOn then
                showToast(self, "观战中不可使用该功能", 1)
                return
        end
        if eventType == ccui.TouchEventType.began then
            self:getParentNode():getParentNode():startVoiceRecord()
        elseif eventType == ccui.TouchEventType.ended
            or eventType == ccui.TouchEventType.canceled then
            self:getParentNode():getParentNode():stopVoiceRecord()
        end
    end)
end

--按钮控制（下拉菜单下拉，隐藏语音按钮）
function GameViewLayer:setMenuVisible(isVisible)
    cc.find(self, "img_menu_popout"):setVisible(isVisible)
end

function GameViewLayer:getUserItemByViewId(viewId)
    local seatObj = self.m_tabUserHead[viewId]
    if seatObj then
        return seatObj:getUserItem()
    end
end

--更新用户显示
function GameViewLayer:OnUpdateUser(viewId, userItem)
     dump(userItem, "#GameViewLayer:OnUpdateUser=========")

	if not viewId or viewId == yl.INVALID_CHAIR then
		print("GameViewLayer:OnUpdateUser<viewId,userItem> viewId is nil")
		return
	end

	if nil == userItem then
        print("GameViewLayer:OnUpdateUser<viewId,userItem> userItem is nil")
        return
    end

    local seatObj = self.m_tabUserHead[viewId]
    if nil == seatObj then
        seatObj = PlayerInfo.new(userItem, viewId,self._scene)
        self.m_tabUserHead[viewId] = seatObj
        self.nodePlayer[viewId]:addChild(seatObj)
    end
    --刷名字,得分,在线情况等
    seatObj:refresh(userItem)
    seatObj:setIsReady(userItem.cbUserStatus == yl.US_READY)

    self.m_UserItem[viewId] = userItem

    if GlobalUserItem.bVideo or (viewId == cmd.MY_VIEWID and userItem.cbUserStatus >= yl.US_READY) then 
        self.btStart:setVisible(false)
    end

    --判断房主
	if PriRoom and GlobalUserItem.bPrivateRoom then
		if userItem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then
			self.m_tabUserHead[viewId]:showRoomHolder(true)
		else
			self.m_tabUserHead[viewId]:showRoomHolder(false)
		end
	end
end

--玩家退出，移除头像信息
function GameViewLayer:OnUpdateUserExit(viewId)
    -- print("GameViewLayer:OnUpdateUserExit================", viewId)
    -- do return end
	print("移除用户", viewId)
	if nil ~= self.m_tabUserHead[viewId] then
		self.m_tabUserHead[viewId] = nil  --退出依然保存信息
		self.nodePlayer[viewId]:removeAllChildren()
		self.m_UserItem[viewId] = nil

        -- if nil ~= self.readySpr[viewId] then
        --     self.readySpr[viewId]:setVisible(false)
        -- end
	end
end

-- 文本聊天
function GameViewLayer:onUserChat(chatdata, viewId)
    print("GameViewLayer:onUserChat================", viewId)

    local playerItem = self.m_tabUserHead[viewId]
    print("获取当前显示聊天的玩家头像", playerItem, viewId, chatdata.szChatString)
    if nil ~= playerItem then
        playerItem:textChat(chatdata.szChatString)
        self._chatLayer:showGameChat(false)
    end
end

-- 表情聊天
function GameViewLayer:onUserExpression(chatdata, viewId)
    print("GameViewLayer:onUserExpression================", viewId)
    local playerItem = self.m_tabUserHead[viewId]
    if nil ~= playerItem then
        playerItem:browChat(chatdata.wItemIndex)
        self._chatLayer:showGameChat(false)
    end
end

--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
    print("GameViewLayer:ShowUserVoice================", viewId, isPlay)
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



function GameViewLayer:showSitBtn(visible,time)


    if self._scene.m_bLookOn then 
        self.btSitAndBegin:setTag(GameViewLayer.BT_SIT)

        self.btSitAndBegin:loadTextureNormal("kwx_btn_sit1.png")
        self.btSitAndBegin:loadTexturePressed("kwx_btn_sit2.png")
        self.btSitAndBegin:setVisible(true)

        self.btSitAndBegin:getChildByName("Text_2"):setVisible(false)
    else
        self.btSitAndBegin:setTag(GameViewLayer.BT_BEGIN)

        self.btSitAndBegin:loadTextureNormal("kwx_btn_start_game.png")
        self.btSitAndBegin:loadTexturePressed("kwx_btn_start_game_2.png")        
        if visible then
            if not self._beginTimeId and time then
                         self.m_beginTimeNum = time
                         self.btSitAndBegin:getChildByName("Text_2"):setString("("..self.m_beginTimeNum..")")
                         self.btSitAndBegin:getChildByName("Text_2"):setVisible(true)
                self._beginTimeId = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
                    if self.updateBeginTime then 
                        self:updateBeginTime()
                    end
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


--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = 1

        --获取头像坐标
        for k,v in pairs(self.m_tabUserHead) do
              if v then
                  if v.m_userItem.dwUserID ==  dwUserID then
                        beginPoint = cc.p(self.nodePlayer[k]:getPositionX()+71, self.nodePlayer[k]:getPositionY()+112)
                  elseif v.m_userItem.dwUserID ==  dwRecvUserID then
                        endPoint = cc.p(self.nodePlayer[k]:getPositionX()+71, self.nodePlayer[k]:getPositionY()+112)
                        zOrder = self.nodePlayer[k]:getZOrder() + 1
                  end
              end
        end
      local faceNode = self:getChildByName("Node_User")
      if beginPoint and endPoint then
              local actSprite = cc.Sprite:create(string.format("public/prop_%d.png",dwPropID)):move(beginPoint):addTo(faceNode,zOrder)
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
                      faceNode:addChild(spine,zOrder)
                  end)))
              end     
      end
end

--显示胡牌提示(当前能胡哪些牌,还有多少张,多少翻)
function GameViewLayer:showHuTipLayer(data)
    -- dump(data, "#GameViewLayer:showHuTipLayer==")

    if not self._objHuTip then
        local uiHuTip = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiHuTip")
        local obj = uiHuTip:ctor(self, self._cardLayer)
        self._objHuTip = obj
    end
    self._objHuTip:refresh(data)
end

function GameViewLayer:removeHuTipLayer()
    if self._objHuTip then
        self._objHuTip:destroy()
        self._objHuTip = nil
    end
end

--更新操作按钮状态
function GameViewLayer:ShowGameBtn(cbActionMask)
    self._pre_action_mask = cbActionMask
    local OpNode = cc.find(self, "FileNode_Op")



    if cbActionMask == GameLogic.WIK_NULL  or self._scene.m_bLookOn then
        OpNode:setVisible(false)

        --操作之后不再提示胡牌
        -- self.nodeTips:removeAllChildren()

        --清掉数据
        self._cardLayer.cbListenList = {}
        self._cardLayer.cbHuCard = {}
        self._cardLayer.cbHuCardFan = {}
        self._cardLayer.cbHuCardLeft = {}

        -- if self.uiShowOutObj then
        --     self.uiShowOutObj:hide()
        -- end
        return
    end

    OpNode:setVisible(true)

    local showBtnList = {}
    if bit:_and(cbActionMask, GameLogic.WIK_GANG) ~= 0 then
        table.insert(showBtnList, self.optBtnList[self.OP_TYPE_GANG])
    end

    if bit:_and(cbActionMask, GameLogic.WIK_PENG) ~= 0 then
        table.insert(showBtnList, self.optBtnList[self.OP_TYPE_PENG])
    end

    local isHasMing = false
    if bit:_and(cbActionMask, GameLogic.WIK_CHI_HU) ~= 0 then
        table.insert(showBtnList, self.optBtnList[self.OP_TYPE_HU])
    else
        if bit:_and(cbActionMask, GameLogic.WIK_LISTEN) ~= 0 then
            table.insert(showBtnList, self.optBtnList[self.OP_TYPE_SHOW])
            table.insert(showBtnList, self.optBtnList[self.OP_TYPE_DISCARD])
            isHasMing = true
        end
    end

    if not isHasMing then
        table.insert(showBtnList, self.optBtnList[self.OP_TYPE_GUO])
    end

    for k,v in ipairs(self.optBtnList) do
        v:setVisible(false)
    end

    --设置按钮的位置,计算每个按钮的位置,第一个按钮(-110,170)依次往左边推
    local gap = -20
    local showBtnNums = #showBtnList
    for k,v in ipairs(showBtnList) do
        local idx = showBtnNums - k + 1
        v:setPosition(-(idx-1)*(145+gap)-230, 220)
        v:setVisible(true)
    end
end

local rotation = {0,90,180,270}
--玩家指向刷新
function GameViewLayer:OnUpdataClockPointView(viewId, selfChairID)
    -- print("GameViewLayer:OnUpdataClockPointView=====", viewId, selfChairID)
	local viewImage =
	{
		cmd.RES_PATH.."round/kwx_img_roll_1.png",
		cmd.RES_PATH.."round/kwx_img_roll_2.png",
		cmd.RES_PATH.."round/kwx_img_roll_4.png",
        cmd.RES_PATH.."round/kwx_img_roll_3.png",
	}
    local sp_clock = cc.find(self, "sp_clock")
    -- sp_clock:setRotation(rotation[selfChairID+1])

	if not viewId or viewId == yl.INVALID_CHAIR then
	    sp_clock:setVisible(false)
	else
        sp_clock:setTexture(viewImage[viewId])
        sp_clock:setVisible(true)
	end
end

function GameViewLayer:setClockVisible(isVisible)
    cc.find(self, "sp_clock"):setVisible(isVisible)
end

 --设置转盘时间
function GameViewLayer:OnUpdataClockTime(time)
    -- print("GameViewLayer:OnUpdataClockTime=====")
 	if 10 > time then
 		self.labelClock:setString(string.format("0%d", time))
 	else
 		self.labelClock:setString(string.format("%d", time))
 	end
end

--刷新剩余牌数
function GameViewLayer:onUpdataLeftCard( numCard )
    -- print("GameViewLayer:onUpdataLeftCard=====", numCard)
    numCard = numCard or 0
    self.nCardLeft = numCard
    self.labelCardLeft:setString(string.format("%d", self.nCardLeft))
end

--消息操作处理
function GameViewLayer:resetMagicInfo()
 --    print("GameViewLayer:resetMagicInfo=====")
	-- local majongNode = self.gameInfoNode:getChildByName("magicBg_4")
	-- majongNode:setVisible(false)
	-- for i=1,2 do
	-- 	local card = majongNode:getChildByName(string.format("majongNode_%d", i))
	-- 	card:setVisible(false)
	-- end
end

--显示癞子信息
function GameViewLayer:onUpdataMagicCard( cbCardData, isAni ) --此处为索引
    -- print("GameViewLayer:onUpdataMagicCard=====", cbCardData, isAni)
    do return end
	if nil ~= cbCardData then
		--左上角游戏信息
    	--local gameInfoNode = self:getChildByTag(GameViewLayer.NODE_INFO_TAG)
    	local majongNode = self.gameInfoNode:getChildByName("magicBg_4")
    	if nil == majongNode then
    		return
    	end
		for i=1,#cbCardData do
			if 34 ~= cbCardData[i] then --无效值为34
				--majongNode:setVisible(true)
				--获取麻将
				local card = majongNode:getChildByName(string.format("majongNode_%d", i))
				if nil ~= card then
					--card:setVisible(true)
					--获取数值
					local cardIndex = cbCardData[i] +1
					local sprPath = cmd.RES_PATH.."card/my_small/tile_meUp_"
					if cardIndex < 10 then
						sprPath = sprPath..string.format("0%d", cardIndex)..".png"
					else
						sprPath = sprPath..string.format("%d", cardIndex)..".png"
					end
					local spriteValue = display.newSprite(sprPath)
					--获取精灵
					local sprCard = card:getChildByName("card_value")
					if nil ~= sprCard then
						sprCard:setSpriteFrame(spriteValue:getSpriteFrame())
					end

					local sprFlag = card:getChildByName("flag")
					if nil ~=  sprFlag then
						sprFlag:setVisible(true)
					end
                    if isAni then --动画sortHandCard
					    card:runAction(cc.Sequence:create(cc.DelayTime:create(3 *i), cc.Show:create()))
                    else
                        card:runAction(cc.Sequence:create(cc.DelayTime:create(0.1), cc.Show:create()))
                    end
                    if 1 == i and 33 ~= cbCardData[i] then
                        --剩余牌数(第一张时候数量-1, 白板为鬼不翻鬼牌)
                        self:onUpdataLeftCard(self.nCardLeft - 1)
                    end
				end

				if isAni then --动画sortHandCard
					local cardData = GameLogic.SwitchToCardData(cbCardData[i] +1) --本地是1到34，服务器是0到33
					local cardMagic = self._cardLayer:createStandCardSprite(cmd.MY_VIEWID, cardData, true)
					if cardMagic then
						cardMagic:addTo(self)
						cardMagic:setPosition(cc.p(667, 450))
						cardMagic:setVisible(false)
						cardMagic:runAction(cc.Sequence:create(cc.DelayTime:create(3 *(i -1)),
								cc.Show:create(), cc.DelayTime:create(2.5), cc.MoveTo:create(0.5, cc.p(70 +(i-1)*55, 705)), cc.CallFunc:create(function (node)
								node:removeFromParent()

                                self._cardLayer.cbMagicData[i] = cbCardData[i]
								--刷新我的手牌
								self._cardLayer:sortHandCard(cmd.MY_VIEWID)
						end)))
					end
					--动画(第一个麻将飞过去后)
					majongNode:runAction(cc.Sequence:create(cc.DelayTime:create(3.2), cc.Show:create()))
				else
					--动画(立刻显示)
					majongNode:runAction(cc.Sequence:create(cc.DelayTime:create(0.1), cc.Show:create()))
				end
			end
		end
	end
end

        -- --游戏额外规则100字节
        -- buffer:pushbyte(1) --固定1
        -- buffer:pushbyte(self.playerNum) --玩家数
        -- buffer:pushbyte(3) --最大玩家数
        -- buffer:pushbyte(self.piao) --0:漂,1:漂一次,2每次要
        -- buffer:pushbyte(selTypeList[ma]) --0:不买马,1:自摸买马,2:亮倒自摸买马
        -- buffer:pushbyte(selNumList[lotter]) --0:无,1:买1马,2:买6马,3:买一送一
        -- buffer:pushbyte(self.chnnl) --0:全频道,1:半频道



--显示规则信息
function GameViewLayer:onshowRule(piaoMode, channel, maMode, maNum)
    --0:漂,1:漂一次,2每次要
    --0:不买马,1:自摸买马,2:亮倒自摸买马
    --0:无,1:买1马,2:买6马,3:买一送一
    local nodeRule = self.RoomInfoLayer:getChildByName("Node_rule")

    nodeRule:getChildByName("rule_select_10"):setVisible(piaoMode>0)
    nodeRule:getChildByName("rule_select_11"):setVisible(piaoMode==0)
    nodeRule:getChildByName("rule_select_20"):setVisible(channel==0)
    nodeRule:getChildByName("rule_select_21"):setVisible(channel==1)

    nodeRule:getChildByName("rule_select_32"):setVisible(maMode==0)
    nodeRule:getChildByName("rule_select_30"):setVisible(maMode==1)
    nodeRule:getChildByName("rule_select_31"):setVisible(maMode==2)

    if maMode > 0 then
        nodeRule:getChildByName("rule_select_40"):setVisible(maNum==1)
        nodeRule:getChildByName("rule_select_41"):setVisible(maNum==2)
        nodeRule:getChildByName("rule_select_42"):setVisible(maNum==3)
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
	--print("玩家出牌， 位置，卡牌数值", viewid, value)
	self.sprOutCardBg:setVisible(isShow)
	self.sprOutCardBg:setPosition(posOurCard[viewid])
	--获取数值
	local cardIndex = GameLogic.SwitchToCardIndex(value)
	local sprPath = cmd.RES_PATH.."card/my_normal/tile_me_"
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
function GameViewLayer:showOperateAction(viewId, actionCode)
	-- body
    local posACtion =
    {
        cc.p(667, 230),
        cc.p(1085, 420),
        cc.p(260, 420),
        cc.p(667, 575)
    }

    local actoins = actionMgr:createOpActions(self, actionCode, posACtion[viewId].x, posACtion[viewId].y, GameViewLayer.ZORDER_ACTION)
    do return end
    local posACtion =
    {
        cc.p(667, 230),
        cc.p(1085, 420),
        cc.p(260, 420),
        cc.p(667, 575)
    }

	local strPath = ""
	if actionCode == GameLogic.WIK_PENG then
		strPath = strPath.."peng"
	end
	if actionCode == GameLogic.WIK_GANG then
		strPath = strPath.."gang"
	end
	if actionCode == GameLogic.WIK_CHI_HU then
		strPath = strPath.."hu"
	end

	local animation = cc.Animation:create()
	for i=1,9 do
		local strPath = cmd.RES_PATH.."game/".. strPath ..string.format("/%d.png", i)
		print("动画资源路径", strPath,  viewId, actionCode)
		local spriteFrame = cc.Sprite:create(strPath):getSpriteFrame()
		if spriteFrame then
			animation:addSpriteFrame(spriteFrame)
		else
			break
		end
		animation:setLoops(1)
		animation:setDelayPerUnit(0.1)
	end
	local animate = cc.Animate:create(animation)
	local spr = cc.Sprite:create(cmd.RES_PATH.."game/".. strPath ..string.format("/%d.png", 1))
	spr:addTo(self, GameViewLayer.ZORDER_ACTION)
	spr:setPosition(posACtion[viewId])
	spr:runAction(cc.Sequence:create(animate, cc.CallFunc:create(function()
			spr:removeFromParent()
		end)))
end

--用户胡牌提示 tagHuInfo
function GameViewLayer:showHuTips(cbHuInfo)
	local infoShow = 0
	local allCard = false
	local isTooMuchCard = false
	if #cbHuInfo == 34 then
		infoShow = 1
		allCard = true
	elseif #cbHuInfo > 7 then
		infoShow = #cbHuInfo
		isTooMuchCard = true
	else
		infoShow = #cbHuInfo
	end
	--判断个数奇
	local pos = cc.p(0, 0)
	if 0 == math.mod(infoShow, 2) then --偶数
		pos = cc.p(-90, 0)
	end
	if allCard then
		local huNode = cc.CSLoader:createNode(cmd.RES_PATH.."game/Node_tips.csb")
		huNode:addTo(self.nodeTips)
		huNode:setPosition(pos)
	elseif isTooMuchCard then
		local huNode = cc.CSLoader:createNode(cmd.RES_PATH.."game/Node_tips.csb")
		huNode:addTo(self.nodeTips)
		huNode:setPosition(pos)

		local tips_all = huNode:getChildByName("tips_all")
		if nil ~= tips_all then
			tips_all:setVisible(false)
		end

		--显示张数
		local node = huNode:getChildByName("Node_num")
		if nil ~= node then
			node:setVisible(true)
			--张数
			local num = node:getChildByName("label_num")
			if nil ~= num then
				num:setString(string.format("%d", infoShow))
			end
		end
	else
		for i=1, infoShow do
			local huNode = cc.CSLoader:createNode(cmd.RES_PATH.."game/Node_tips.csb")
			huNode:addTo(self.nodeTips)
			print("用户胡牌提示1", pos.x, pos.y)
			local posTag = pos
			if 0 == math.mod(i, 2) then
				posTag = cc.p(pos.x +math.floor(i /2) *180, pos.y)
			else
				posTag = cc.p(pos.x -math.floor(i /2) *180, pos.y)
			end
			print("用户胡牌提示2", posTag.x, posTag.y)
			huNode:setPosition(posTag)

			--信息
			local huInfo = cbHuInfo[i]
			local tips_fan = huNode:getChildByName("tips_fan")
			if nil ~= tips_fan then
				tips_fan:setVisible(true)
			end
			local tips_huCardNum = huNode:getChildByName("tips_huCardNum")
			if nil ~= tips_huCardNum then
				tips_huCardNum:setVisible(true)
			end
			local tips_all = huNode:getChildByName("tips_all")
			if nil ~= tips_all then
				tips_all:setVisible(false)
			end
			local Node_Majong = huNode:getChildByName("Node_Majong")
			if nil ~= Node_Majong then
				Node_Majong:setVisible(true)
				--获取数值
				local cardIndex = GameLogic.SwitchToCardIndex(huInfo.cbCardValue)
				local sprPath = cmd.RES_PATH.."card/my_small/tile_meUp_"
				if cardIndex < 10 then
					sprPath = sprPath..string.format("0%d", cardIndex)..".png"
				else
					sprPath = sprPath..string.format("%d", cardIndex)..".png"
				end
				local spriteValue = display.newSprite(sprPath)
				--获取精灵
				local sprCard = Node_Majong:getChildByName("card_value")
				if nil ~= sprCard then
					sprCard:setSpriteFrame(spriteValue:getSpriteFrame())
				end
			end

			local label_Fan = huNode:getChildByName("label_Fan")
			if nil ~= label_Fan then
				label_Fan:setVisible(true)
				label_Fan:setString(string.format("%d", huInfo.cbFan))
			end
			local label_Num = huNode:getChildByName("label_Num")
			if nil ~= label_Num then
				label_Num:setVisible(true)
				label_Num:setString(string.format("%d", huInfo.cbNum))
			end
		end
	end


end

--用户摸马动画
function GameViewLayer:showMoMaAction(bankChair, huChair, viewId, maNum, maData, validMaNum, validMaData, endDelayTime, showResultCallback)
    dump(maData, "#maData")
    dump(validMaData, "#validMaData")
    print("bankChair, huChair, viewId, maNum, maData, validMaNum, validMaData, endDelayTime==", bankChair, huChair, viewId, maNum, maData, validMaNum, validMaData, endDelayTime)
    local delayTime = 2.0
    if endDelayTime and 0 ~= endDelayTime then
        delayTime = endDelayTime
    end

    local moMaNode = cc.CSLoader:createNode(cmd.RES_PATH.."privateroom/Node_moma.csb")
    moMaNode:addTo(self, GameViewLayer.ZORDER_RESULT +1)
    moMaNode:setPosition(cc.p(667, 400))
    moMaNode:setVisible(false)
    moMaNode:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime), cc.Show:create()))


--[[
    local userItem = self.m_tabUserHead[viewId].m_userItem
    --设置姓名
    local userName = moMaNode:getChildByName("Text_name")
    if nil ~= userName then
        local clipNick = ClipText:createClipText(cc.size(110, 20), userItem.szNickName)
        clipNick:setAnchorPoint(cc.p(0.5, 0.5))
        clipNick:setPosition(userName:getPosition())
        moMaNode:addChild(clipNick)
        clipNick:setTextColor(cc.c3b(0x00,0x93,0xFC))
        --userName:setString(userItem.szNickName)
        userName:setVisible(false)
    end


    --摸马倍数
    local MaNum = moMaNode:getChildByName("text_maNum")
    if nil ~= MaNum then --初始为0
        MaNum:setString("0")
        MaNum:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime + 1.0 *maNum), cc.CallFunc:create(function()
            MaNum:setString(string.format("%d", validMaNum))
        end)))
    end
 ]]

    --摸马动画
    local cardNode = moMaNode:getChildByName("Node_majong")

    --播完买马动画,立马回调,通知
    local function callfunc(sender)
        --最后一个买马动画做完的回调
        if sender:getTag() >= maNum then
            moMaNode:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function(sender)
                sender:removeFromParent()
                if type(showResultCallback) == "function" then
                    showResultCallback()
                end
            end)))
        end
    end

    --这里要计算麻将的位置
    local totalCardNum = maNum
    local cardWidth = 80
    local gap = 20
    local totalCardWidth = totalCardNum * cardWidth + (totalCardNum-1)*gap
    local startx = -totalCardWidth * 0.5 + cardWidth * 0.5

    for i=1,maNum do
        --创建麻将
        local card = self._cardLayer:createMyActiveCardSprite(maData[i], false)
        card:setTag(i)
        card:addTo(cardNode)
        card:setVisible(false)
        -- card:setPosition(80 * (i -1), 0)
        card:setPosition(startx+(i-1)*(cardWidth+gap), -40)
        -- if not self:isValidMa(bankChair, huChair, maData[i]) then
        --  card:setColor(cc.c3b(127,127,127))
        -- end
        --判断马牌
        local isValidMa = false
        for j=1,validMaNum do
            if maData[i] == validMaData[j] then
                isValidMa = true
                break
            end
        end
        if not isValidMa then
            card:setColor(cc.c3b(127,127,127))
        end
        card:runAction(cc.Sequence:create(
            cc.DelayTime:create(delayTime + 1.0 * (i -1)),
            cc.Show:create(),
            cc.CallFunc:create(callfunc) ))
    end
--[[
    moMaNode:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime + 1.0 *maNum +2), cc.CallFunc:create(function()
            moMaNode:removeFromParent()
            if type(showResultCallback) == "function" then
                showResultCallback()
            end
        end)))
    ]]
end

--判断胡牌玩家是否买中了马
function GameViewLayer:isValidMa( bankChair, huChair, cardData )
	local MaData =
	{
		{0x01, 0x05, 0x09, 0x11, 0x15, 0x19, 0x21, 0x25, 0x29, 0x31},
		{0x02, 0x06,  0x12, 0x16, 0x22, 0x26, 0x32, 0x35},
		{0x04, 0x08,  0x14, 0x18, 0x24, 0x28, 0x34, 0x37},
		{0x03, 0x07, 0x13, 0x17, 0x23, 0x27, 0x33, 0x36},
	}
	--转换
	local tag = yl.INVALID_CHAIR
    local nChairCount = 4--self._scene._gameFrame:GetChairCount()

 	 local right = bankChair +1
 	 right = right >= nChairCount and right - nChairCount or right
 	 local left  = bankChair -1
 	 left = left < 0 and left + nChairCount or left
 	 local top = bankChair + 2
 	 top = top >= nChairCount and top - nChairCount or top

 	 if huChair == bankChair then
 	 	tag = cmd.MY_VIEWID
 	 elseif huChair == right then
 	 	tag = cmd.RIGHT_VIEWID
 	 elseif huChair == left then
 	 	tag = cmd.LEFT_VIEWID
 	 elseif huChair == top then
 	 	tag = cmd.TOP_VIEWID
 	 end
 	 for i=1,#MaData[tag] do
 	 	if cardData == MaData[tag][i] then
 	 		return true
 	 	end
 	 end
 	 return false
end

--开始
function GameViewLayer:gameStart(startViewId, cbCardData, cbCardCount, cbUserAction, cbMagicData)
    cc.find(self, "Sprite_singal_bg"):setVisible(true)  --左上角信号

    -- dump(cbCardData, "#GameViewLayer:gameStart cbCardData======")
    -- if #cbCardData >= 14 then
    --     local i = 3
    -- end

	--每次发四张,第四次一张
	local viewid = startViewId
    -- local tableView = {1, 2, 4, 3} --对面索引为3
	local tableView = {1, 2, 3} --对面索引为3
	local cardIndex = 1 --读取自己卡牌的索引
	local actionList = {}
    local playerNum = self._scene:getPlayerNum() --cmd.GAME_PLAYER
	for i=1,4 do
		local cardCount = (i == 4 and 1 or 4)
		for k=1,playerNum do
                                      if self._scene.m_cbPlayStatus[k] == 1 then 
                                            local viewid = self._scene:SwitchViewChairID(k-1)
                                            local myCardDate = {}
                                            if viewid == cmd.MY_VIEWID  then
                                                for j=1,cardCount do
                                                -- print("开始发牌,我的卡牌", cardIndex, cbCardData[cardIndex])
                                                myCardDate[j] = cbCardData[cardIndex]
                                                cardIndex = cardIndex +1
                                                end
                                            end

                                            function callbackWithArgs(viewid, myCardDate, cardCount)
                                            	              local ret = function ()
                                            	              	self._cardLayer:sendCardToPlayer(viewid, myCardDate, cardCount)
                                            	              	self:onUpdataLeftCard(self.nCardLeft - cardCount)
                                            	              end
                                               return ret  
                                            end
                                            local callFun = cc.CallFunc:create(callbackWithArgs(tableView[viewid], myCardDate, cardCount))
                                            table.insert(actionList, cc.DelayTime:create(0.5))
                                            table.insert(actionList, callFun)
                                      end

		end
	end

	--发完手牌给庄家发牌
	local myCardDate = {}
	if startViewId == cmd.MY_VIEWID then
		myCardDate[1] = cbCardData[14]
		--如果我是庄家，允许触摸
		self._cardLayer:setMyCardTouchEnabled(true)
	end
	function callbackWithArgs(viewid, myCardDate, cardCount, cbUserAction, cbMagicData)
		local ret = function ()
			self:onUpdataLeftCard(self.nCardLeft - cardCount)
			self._cardLayer:sendCardToPlayer(viewid, myCardDate, cardCount)
			self:onUpdataMagicCard(cbMagicData, true)

			if viewid == cmd.MY_VIEWID then
				--判断有没有操作
				if cbUserAction ~= GameLogic.WIK_NULL then
					if bit:_and(GameLogic.WIK_GANG, cbUserAction) ~= GameLogic.WIK_NULL then
                        -- local cardGang = self._scene:findUserGangCard(self._cardLayer.cbCardData[cmd.MY_VIEWID])
						local cardGang = self._scene:findUserGangCard(cbCardData)
                        -- dump(cardGang, "#callbackWithArgs=====a")
                        -- dump(self._cardLayer.cbCardData[cmd.MY_VIEWID], "#callbackWithArgs=====b")
						if nil ~= cardGang[1] then
							self.cbActionMask = cbUserAction
							self.cbActionCard = cardGang[1]
							self:ShowGameBtn(cbUserAction)
						end
					else
						self:ShowGameBtn(cbUserAction)
					end
				end
			end
		end
		return ret
    end
	local callFun = cc.CallFunc:create(callbackWithArgs(startViewId, myCardDate, 1, cbUserAction, cbMagicData))
	table.insert(actionList, cc.DelayTime:create(0.5))
    table.insert(actionList, callFun)

	self:runAction(cc.Sequence:create(actionList))
end


--用户出牌
function GameViewLayer:gameOutCard(viewId, card, bIsSysOut)

	--print("用户出牌", viewId, card)
	self:showOutCard(viewId, card, true) --展示出牌
	if viewId ~= cmd.MY_VIEWID then
		self._cardLayer:outCard(viewId, card)
	elseif self._scene.bTrustee or bIsSysOut or self._scene.m_bLookOn then
        --dump(self._cardLayer.cbCardData[cmd.MY_VIEWID], "#促排前===")
		self._cardLayer:outCardTrustee(card)
        --dump(self._cardLayer.cbCardData[cmd.MY_VIEWID], "#促排后===")
	end

	self.cbOutCardTemp = card
	self.cbOutUserTemp = viewId
	--self._cardLayer:discard(viewId, card)
end
--用户抓牌
function GameViewLayer:gameSendCard(viewId, card)
	--发牌
	if viewId == cmd.MY_VIEWID and not self._scene:isTingOut(self._scene:GetMeChairID())  and not self._scene.m_bLookOn then
		self._cardLayer:setMyCardTouchEnabled(true)
	end
    print(self.nCardLeft , self.nCardLeft-1)
	self:onUpdataLeftCard(self.nCardLeft - 1)
	self._cardLayer:sendCardToPlayer(viewId, {card}, 1)
end
-----------------------------------------------回放-------------------------------------
function GameViewLayer:updateCardsNode(handCard, outCard, activityCard, magicCard, leftCard)
	--先清除数据
	self._cardLayer:onResetData()

	--癞子
	self._cardLayer.cbMagicData = magicCard
	self:onUpdataMagicCard(self._cardLayer.cbMagicData, false)
	--剩余牌数
	self:onUpdataLeftCard(leftCard)

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

--回放开始1130

function GameViewLayer:gameVideoStart(startChair, cbCardData, cbMagicData)
    dump(cbCardData, "#GameViewLayer:gameVideoStart cbCardData==")
    --每次发四张,第四次一张
    local chairIndex = startChair
    local cardIndex = {1, 1, 1, 1} --读取自己卡牌的索引
    local actionList = {}
    local playerNum = self._scene:getPlayerNum()
    for i=1,4 do
        local cardCount = (i == 4 and 1 or 4)
        for k=1,playerNum do

            if playerNum == chairIndex then
                chairIndex = 0
            end
            local viewid = self._scene:SwitchViewChairID(chairIndex)

            local myCardDate = {}
            for j=1,cardCount do
                -- print("gameVideoStart",chairIndex, viewid)
                -- print("开始发牌,我的卡牌", cardIndex[viewid], cbCardData[viewid][cardIndex[viewid]])
                myCardDate[j] = cbCardData[viewid][cardIndex[viewid]]
                cardIndex[viewid] = cardIndex[viewid] +1
            end

            function callbackWithArgs(viewid, myCardDate, cardCount)
                  local ret = function ()
                    self._cardLayer:sendCardToPlayer(viewid, myCardDate, cardCount)
                    self:onUpdataLeftCard(self.nCardLeft - cardCount)
                  end
                  return ret
            end
            local callFun = cc.CallFunc:create(callbackWithArgs(viewid, myCardDate, cardCount))
            table.insert(actionList, cc.DelayTime:create(0.2))
            table.insert(actionList, callFun)
            --如果是我要发卡牌信息过去
            chairIndex = chairIndex +1
        end
    end
    --发完手牌给庄家发牌
    local myCardDate = {}
    local viewid = self._scene:SwitchViewChairID(startChair)
    myCardDate[1] = cbCardData[viewid][14]

    function callbackWithArgs(viewid, myCardDate, cardCount)
        local ret = function ()
            self:onUpdataLeftCard(self.nCardLeft - cardCount)
            self._cardLayer:sendCardToPlayer(viewid, myCardDate, cardCount)

            self:onUpdataMagicCard(cbMagicData, true)
        end
        return ret
    end
    local callFun = cc.CallFunc:create(callbackWithArgs(viewid, myCardDate, 1))
    table.insert(actionList, cc.DelayTime:create(0.2))
    table.insert(actionList, callFun)
    self:runAction(cc.Sequence:create(actionList))
end

function GameViewLayer:updateCardsNode(handCard, outCard, activityCard, magicCard, leftCard)
	--先清除数据
	self._cardLayer:onResetData()

	--癞子
	self._cardLayer.cbMagicData = magicCard
	self:onUpdataMagicCard(self._cardLayer.cbMagicData, false)
	--剩余牌数
	self:onUpdataLeftCard(leftCard)

	--先设置碰杠的牌
    local playerNum = self._scene:getPlayerNum()
	for i=1,playerNum do
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
	for i = 1, playerNum do
		local viewId = self._scene:SwitchViewChairID(i - 1)
		if #handCard[viewId] > 0 then
			self._cardLayer:createHandCard(viewId, handCard[viewId], #handCard[viewId])
		end
	end

	--设置已经出的牌
	for i = 1, playerNum do
		local viewId = self._scene:SwitchViewChairID(i - 1)
		if #outCard[viewId] > 0 then
			self._cardLayer:createOutCard(viewId, outCard[viewId], #outCard[viewId])
		end
	end
end
----------------------------------------------------------新大厅--------------------------------------------------
--初始化新大厅房间信息
function GameViewLayer:initRoomInfo()
    self.RoomInfoLayer = cc.CSLoader:createNode(cmd.RES_PATH.."Layer_info.csb"):addTo(self, GameViewLayer.ZORDER_SETTING)
    self.RoomInfoLayer:setVisible(false)

    local PanelLayer = self.RoomInfoLayer:getChildByName("Panel")
    local btn_close = self.RoomInfoLayer:getChildByName("Button_1")
    -- 遮罩
    PanelLayer:addTouchEventListener(function(ref)
        self.RoomInfoLayer:setVisible(false)
    end)
    btn_close:addTouchEventListener(function(ref)
        self.RoomInfoLayer:setVisible(false)
    end)

    local boxCallback = function(sender,eventType)
        self.m_tabBoxGame[GameViewLayer.BT_RULE]:setSelected(GameViewLayer.BT_RULE == sender:getTag())
        self.m_tabBoxGame[GameViewLayer.BT_SCORELIST]:setSelected(GameViewLayer.BT_SCORELIST == sender:getTag())

        self.RoomInfoLayer:getChildByName("Node_rule"):setVisible(GameViewLayer.BT_RULE == sender:getTag())
        self.RoomInfoLayer:getChildByName("Node_score"):setVisible(GameViewLayer.BT_SCORELIST == sender:getTag())
    end
    --按钮控制
    self.m_tabBoxGame = {}
    local checkbx = self.RoomInfoLayer:getChildByName("CheckBox_1")
    checkbx:setTag(GameViewLayer.BT_RULE)
    checkbx:addEventListener(boxCallback)
    checkbx:setSelected(true)
    self.m_tabBoxGame[GameViewLayer.BT_RULE] = checkbx

    checkbx = self.RoomInfoLayer:getChildByName("CheckBox_2")
    checkbx:setTag(GameViewLayer.BT_SCORELIST)
    checkbx:addEventListener(boxCallback)
    self.m_tabBoxGame[GameViewLayer.BT_SCORELIST] = checkbx

    -- 房间信息界面
    --解散按钮
    local btnDismiss = self.RoomInfoLayer:getChildByName("Node_rule"):getChildByName("btn_dismiss")
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

    -- self:updataUserInfo()

    self.RoomInfoLayer:getChildByName("Node_rule"):setVisible(true)
end

function GameViewLayer:showRoomInfo(bIsShow)
    self.RoomInfoLayer:setVisible(bIsShow)
    local btnDismiss = self.RoomInfoLayer:getChildByName("Node_rule"):getChildByName("btn_dismiss")
    -- if GlobalUserItem.bPrivateGoldRoom and btnDismiss then
    --     btnDismiss:setVisible(false)
    -- end
    if bIsShow then
    	self._scene:onSendQueryRecord()
    end
end

function GameViewLayer:updataUserInfo()
    print("GameViewLayer:updataUserInfo=====", self._scene.playMode)
    local nodeScore = self.RoomInfoLayer:getChildByName("Node_score")
    -- if self._scene.playMode == 1 then
    if false then
    	for i=1,cmd.GAME_PLAYER do
            print("getUserInfoByChairID===", i)
        	local userItem = self._scene:getUserInfoByChairID(i)
        	local labName = nodeScore:getChildByName("Text_name"..i)
        	labName:setString("")
    	end
    	local userItem = self._scene:getUserInfoByChairID(self._scene:GetMeChairID()+1)
    	local labName = nodeScore:getChildByName("Text_name1")
    	local strNickname = string.EllipsisByConfig(userItem.szNickName, 90,
                                string.getConfig("fonts/round_body.ttf", 20))
        labName:setString(strNickname)
    else
    	for i=1,cmd.GAME_PLAYER do
            print("getUserInfoByChairID===", i)
        	local userItem = self._scene:getUserInfoByChairID(i)
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
    return 600, 50
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

    local csb = cc.CSLoader:createNode(cmd.RES_PATH.."Node_cell.csb")
    cell:addChild(csb)
    csb:setPosition(view:getViewSize().width * 0.5, 50 * 0.5)

    --背景
    -- if 1 == math.mod(idx, 2) then
    --     local sprBg =  csb:getChildByName("Image")
    --     if nil ~= sprBg then
    --         sprBg:loadTexture(cmd.RES_PATH.."game/info/cell2.png")
    --     end
    -- end

    --局数
    --局数
    local strRound = (idx + 1).."/"..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    if self._scene.playMode == 1 then
    	strRound = idx + 1
    end
    csb:getChildByName("Text_round"):setString(strRound)
    -- dump(iteminfo , "iteminfo")
    --分数
    for i=1,cmd.GAME_PLAYER do
    	local score=nil
    	local labScore=nil
        -- if self._scene.playMode == 1 then
    	if false then
        	score = iteminfo[self._scene:GetMeChairID()+1]
        	labScore = csb:getChildByName("Text_score1")
        else
        	score = iteminfo[i]
        	labScore = csb:getChildByName("Text_score"..i)
        end

        if nil ~= score then
            labScore:setString(score.."")

            -- --判断是不是自己
            -- if i -1 == self._scene:GetMeChairID() then
            --     if score > 0 then
            --         labScore:setColor(cc.c3b(255,0,0))
            --     else
            --         labScore:setColor(cc.c3b(54,176,11))
            --     end
            -- end
        end
    end
    return cell
end

-- 子视图数目
function GameViewLayer:numberOfCellsInTableView(view)
    return #self.m_tabList
end

function GameViewLayer:isCanShowFinal()
    -- print("isCanShowFinal=====<isdestroy uigameover, obj>",self.isGameOverObjDestroy, self.gameOverObj)
    -- if not self.isGameOverObjDestroy or self.gameOverObj then
    --     return false
    -- end
    return true
end

function GameViewLayer:setBtnStartVisible(isVisible)
    self.btStart:setVisible(isVisible)
end

function GameViewLayer:closeFinalResult()
    if self.objFinalReport then
        self.objFinalReport:onClickQuit()
    end
end

function GameViewLayer:showFinalResult()
    if self.objFinalReport then
        self.objFinalReport:show()
    end
end

function GameViewLayer:showResultLayer(parent, dataList)
    local function showFinalResult()
        print("有总结算显示总结算=============", self.objFinalReport)
        if self.objFinalReport then
            self.objFinalReport:show()
        end
    end
    local function callback(etype)



        if etype == "continue" then
            --不要隐藏错误
            assert(PriRoom~=nil, "GameViewLayer:showResultLayer PriRoom is nil")
            -- print("GlobalUserItem.bPrivateRoom========", GlobalUserItem.bPrivateRoom, PriRoom:getInstance().m_tabPriData.dwPlayCount, PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)
            if GlobalUserItem.bPrivateRoom and PriRoom:getInstance().m_tabPriData.dwPlayCount < PriRoom:getInstance().m_tabPriData.dwDrawCountLimit and not self._scene.isPriOver then
                self._scene:sendGameStart()
            else
                if self.objFinalReport then
                    showFinalResult()
                    print("callback 点击继续,showFinalResult============================================")
                else
                    self._scene:sendGameStart()
                end
            end
            self._scene.m_bGameEnd = false
            --self.btStart:setVisible(false)
        elseif etype == "share" then
            self:share()
        elseif etype == "onExit" then
            self.gameOverObj = nil
            self._scene.m_bGameEnd  = false
            showFinalResult()
        else
            if etype == "quit" then
                self.isGameOverObjDestroy = true
            end
            -- self._scene:onQueryExitGame()
            self._scene.endLayer = nil
            showFinalResult()
            print("callback 点击关闭,showFinalResult============================================")
        end
    end



    local SchedulerPool = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "utils.SchedulerPool")
    local uiGameOver = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiGameOver")
    local gameOverObj = uiGameOver:ctor(parent or self, callback)
    gameOverObj:refresh(dataList)
    gameOverObj:hide()
    self.gameOverObj = gameOverObj

    self._scene.endLayer = gameOverObj

    self.schPoolObj = SchedulerPool.new()
    print("延迟显示小结算============================================", self.isGameOverObjDestroy)
    self.schPoolObj:delayCall(function()
        gameOverObj:show()
        self.btStart:setVisible(true)
    end, 2)
end

function GameViewLayer:share()
     PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
        local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "分享成功", 2)
            end
            GlobalUserItem.bAutoConnect = true
        end
        local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
        -- 截图分享
        local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
        local area = cc.rect(0, 0, framesize.width, framesize.height)
        local imagename = "grade_share.jpg"
        ExternalFun.popupTouchFilter(0, false)
        captureScreenWithArea(area, imagename, function(ok, savepath)
            ExternalFun.dismissTouchFilter()
            if ok then
                if nil ~= target then
                    GlobalUserItem.bAutoConnect = false
                    MultiPlatform:getInstance():shareToTarget(target, sharecall, "我的约战房战绩", "分享我的约战房战绩", url, savepath, "true")
                end
            end
        end)
    end, 3)

end

return GameViewLayer