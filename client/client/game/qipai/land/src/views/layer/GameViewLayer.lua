--
-- Author: zhong
-- Date: 2016-11-02 17:28:24
--
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local module_pre = "game.qipai.land.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local Define = appdf.req(module_pre .. ".models.Define")
local GameRoleItem = appdf.req(module_pre .. ".views.layer.GameRoleItem")
local CardSprite = appdf.req(module_pre .. ".views.layer.gamecard.CardSprite")
local CardsNode = appdf.req(module_pre .. ".views.layer.gamecard.CardsNode")
local GameLogic = appdf.req(module_pre .. ".models.GameLogic")
local GameResultLayer = appdf.req(module_pre .. ".views.layer.GameResultLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local GameChatLayer = appdf.req(module_pre .. ".views.layer.GameChatLayer")
local ShakeLayer = appdf.req(module_pre .. ".views.layer.ShakeLayer")
local SpineObj = appdf.req("client.src.external.SpineObj")


local TAG_ENUM = Define.TAG_ENUM
TAG_ENUM.BT_RECORD = 200
TAG_ENUM.BT_RECORDMASK = 201
TAG_ENUM.BT_RECORDBG = 202
TAG_ENUM.BT_HELP = 203
TAG_ENUM.BT_LOCATION = 204
TAG_ENUM.BT_VOICE = 205
TAG_ENUM.BT_BG = 206
TAG_ENUM.BT_SITDOWN = 207

local TAG_ZORDER = Define.TAG_ZORDER
TAG_ZORDER.PRIROOM_ZORDER = 205
TAG_ZORDER.RESULT_ZORDER = 206
TAG_ZORDER.RECORDCSB_ZORDER = 207
TAG_ZORDER.RECORD_ZORDER = 208
TAG_ZORDER.RECORD_NUM = 209

--非自己出的牌位置调整
local ADJUSTGAP = 62

local GameViewLayer = class("GameViewLayer",function(scene)
        local gameViewLayer = display.newLayer()
    return gameViewLayer
end)

local personPos = 
    {
        cc.p(270, 340), 
        cc.p(200,80), 
        cc.p(1050, 340)
    }

local START_SCALL_BANKER = 4
local START_READY = 5

function GameViewLayer:ctor(scene)
    --注册node事件
    ExternalFun.registerNodeEvent(self)
    self._scene = scene

    --初始化
    self:paramInit()

    --加载资源
    self:loadResource()


end

function GameViewLayer:paramInit()
    -- 聊天层
    self.m_chatLayer = nil
    -- 结算层
    self.m_resultLayer = nil

    -- 手牌数量
    self.m_tabCardCount = {}
    -- 报警动画
    self.m_tabSpAlarm = {}

    -- 庄家牌
    self.m_tabBankerCard = {}
    
    -- 准备标签
    self.m_tabReadySp = {}
    -- 状态标签
    self.m_tabStateSp = {}

    -- 叫分控制
    self.m_callScoreControl = nil
    self.m_nMaxCallScore = 0
    self.m_tabCallScoreBtn = {}

    -- 出牌控制
    self.m_outCardsControl = nil
    -- 能否出牌
    self.m_bCanOutCard = false

    -- 用户头像
    self.m_tabUserHead = {}
    self.m_tabUserHeadAnimate = {}
    self.m_tabUserHeadPos = {}

    -- 用户信息
    self.m_tabUserItem = {}
    -- 用户昵称
    self.m_tabCacheUserNick = {}
    -- 一轮提示组合
    self.m_promptIdx = 0
    self.m_tabTimerPos = {}
    self.m_bIsChange = false

    --存放出牌动画
    self.saveOutCardAnimate = {}

    --庄家视图ID
    self.bankViewID = 0

    -- 扑克
    self.m_tabNodeCards = {}

    --是否叫庄状态
    self.isCallBanker = false

    --剩余牌数
    self.leftCardNum = {}

    --是否托管状态
    self.isAutoOutCard = false

    --当前操作玩家
    self.currentPlayer = nil
end

function GameViewLayer:getParentNode()
    return self._scene
end

function GameViewLayer:addToRootLayer( node , zorder)
    if nil == node then
        return
    end

    self.m_rootLayer:addChild(node)
    if type(zorder) == "number" then
        node:setLocalZOrder(zorder)
    end    
end

function GameViewLayer:loadResource()
    self.m_tabTimerPos[cmd.MY_VIEWID] = cc.p(540, 328)
    self.m_tabTimerPos[START_SCALL_BANKER] = cc.p(475, 324)
    self.m_tabTimerPos[START_READY] = cc.p(520, 212)
    self.m_tabTimerPos[cmd.LEFT_VIEWID] = cc.p(380, 596)
    self.m_tabTimerPos[cmd.RIGHT_VIEWID] = cc.p(954, 596)

    -- 加载卡牌纹理
    cc.Director:getInstance():getTextureCache():addImage("game/card.png")
    
    cc.Director:getInstance():getTextureCache():addImage("game/game.png")
    cc.SpriteFrameCache:getInstance():addSpriteFrames("game/game.plist")
    -- 叫分
    AnimationMgr.loadAnimationFromFrame("call_point_0%d.png", 0, 5, Define.CALLSCORE_ANIMATION_KEY)
    -- 一分
    AnimationMgr.loadAnimationFromFrame("call_point_0%d.png", 5, 3, Define.CALLONE_ANIMATION_KEY)
    -- 两分
    AnimationMgr.loadAnimationFromFrame("call_point_1%d.png", 5, 3, Define.CALLTWO_ANIMATION_KEY)
    -- 三分
    AnimationMgr.loadAnimationFromFrame("call_point_2%d.png", 5, 3, Define.CALLTHREE_ANIMATION_KEY)
    -- 飞机
    -- AnimationMgr.loadAnimationFromFrame("plane_%d.png", 0, 5, Define.AIRSHIP_ANIMATION_KEY)
    -- -- 火箭
    -- AnimationMgr.loadAnimationFromFrame("rocket_%d.png", 0, 5, Define.ROCKET_ANIMATION_KEY)
    -- 报警
    AnimationMgr.loadAnimationFromFrame("game_alarm_0%d.png", 0, 5, Define.ALARM_ANIMATION_KEY)
    -- 炸弹
    -- AnimationMgr.loadAnimationFromFrame("game_bomb_0%d.png", 0, 5, Define.BOMB_ANIMATION_KEY)
    -- 语音动画
    AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, Define.VOICE_ANIMATION_KEY)

    --播放背景音乐
    ExternalFun.playBackgroudAudio("background.mp3")

    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/GameLayer.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode
    -- csbNode:getChildByName("game_bg_0_1"):setVisible(false)
    

    --大厅动画
    self:hallTreeAnimate()
    self:hallCloudAnimate()

    local this = self
    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.began then
            this:onButtonClickedEvent(sender:getTag(), sender)
        end
    end

    self.leftCardNum[cmd.LEFT_VIEWID] = 0
    self.leftCardNum[cmd.RIGHT_VIEWID] = 0
    self.leftCardNum[cmd.MY_VIEWID] = 0

    self.m_cardControl = csbNode:getChildByName("card_control")
    self.m_tabCardCount[cmd.LEFT_VIEWID] = csbNode:getChildByName("atlas_count_1")
    self.m_tabCardCount[cmd.LEFT_VIEWID]:setLocalZOrder(1)
    self.m_tabCardCount[cmd.RIGHT_VIEWID] = csbNode:getChildByName("atlas_count_3")
    self.m_tabCardCount[cmd.RIGHT_VIEWID]:setLocalZOrder(1)
    self.m_tabCardCount[cmd.MY_VIEWID] = csbNode:getChildByName("Button_cardRecord"):getChildByName("atlas_count_2")
    self.m_tabCardCount[cmd.MY_VIEWID]:setString("0")

    local a = csbNode:getChildByName("chat_btn")
    self.m_tabCardCount[cmd.MY_VIEWID]:setLocalZOrder(1)
    csbNode:getChildByName("cardrecord_right"):setVisible(false)
    csbNode:getChildByName("cardrecord_left"):setVisible(false)
    ------
    --顶部菜单
    local top = csbNode:getChildByName("btn_layout")

    --背景
    local btn = csbNode:getChildByName("land_bg")
    btn:setTag(TAG_ENUM.BT_BG)
    btn:addTouchEventListener(btnEvent)

    -- 按钮切换1
    btn = top:getChildByName("btn_switch_1")
    btn:setTag(TAG_ENUM.BT_SWITCH_1)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)
    self.m_btnSwitch_1 = btn
    if GlobalUserItem.bVideo  then
        btn:setVisible(false)
        btn:setEnabled(false)
    end

    -- 切换背景
    self.m_spSwitchBg = top:getChildByName("land_sp_btnlist_bg")
    --self.m_spSwitchBg:setScaleY(0.0000001)
    self.m_spSwitchBg:setVisible(false)

    self.playerNode = csbNode:getChildByName("Node_Person")

    self.m_tabBtn = {}
    --聊天按钮
    btn = csbNode:getChildByName("chat_btn")
    btn:setTag(TAG_ENUM.BT_CHAT)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)
    table.insert(self.m_tabBtn,btn)

    --换桌和托管按钮
    btn = self.m_spSwitchBg:getChildByName("tru_btn")   
    btn:setTag(TAG_ENUM.BT_TRU)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)
    btn:setEnabled(not GlobalUserItem.bPrivateRoom)
    -- if GlobalUserItem.bPrivateGoldRoom then
    --     btn:setEnabled(false)
    --     cmd.GAME_SCENE_FREE
    -- end

    --设置按钮
    btn = self.m_spSwitchBg:getChildByName("set_btn")
    btn:setTag(TAG_ENUM.BT_SET)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)

    --帮助按钮
    btn = csbNode:getChildByName("help_btn")
    btn:setTag(TAG_ENUM.BT_HELP)
    btn:setVisible(false)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)

    --定位
    btn = csbNode:getChildByName("Button_location")
    btn:setTag(TAG_ENUM.BT_LOCATION)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)
    table.insert(self.m_tabBtn,btn)
    

    --记牌按钮
    btn = csbNode:getChildByName("Button_cardRecord")
    btn:setTag(TAG_ENUM.BT_RECORD)
    btn:setZOrder(TAG_ZORDER.RECORD_ZORDER)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)

    -- if GlobalUserItem.bPrivateRoom then
    --     btn:setEnabled(false)
    -- end

    -- if  GlobalUserItem.bMatch then
    --      btn:setEnabled(false)
    -- end

    --退出按钮
    btn = self.m_spSwitchBg:getChildByName("back_btn")
    btn:setTag(TAG_ENUM.BT_EXIT)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)

    --叫分
    self.m_textGameCall = top:getChildByName("atlas_callscore")

    -- 庄家扑克
    self.m_nodeBankerCard = cc.Node:create()
    self.m_nodeBankerCard:setPosition(667, 700)
    top:addChild(self.m_nodeBankerCard)

    --准备按钮
    self.m_btnReady = top:getChildByName("ready_btn")
    self.m_btnReady:setTag(TAG_ENUM.BT_READY)
    self.m_btnReady:addTouchEventListener(btnEvent)
    self.m_btnReady:setEnabled(false)
    self.m_btnReady:setVisible(false)
    self.m_btnReady:loadTextureDisabled("game_new/btn_ready_0.png")


    self.m_btnSit = top:getChildByName("sitdown_btn")
    self.m_btnSit:setTag(TAG_ENUM.BT_SITDOWN)
    self.m_btnSit:addTouchEventListener(btnEvent)
    self.m_btnSit:setVisible(false)

    -- 邀请按钮
    self.m_btnInvite = top:getChildByName("btn_invite")
    self.m_btnInvite:setTag(TAG_ENUM.BT_INVITE)
    self.m_btnInvite:addTouchEventListener(btnEvent)
    --if GlobalUserItem.bPrivateRoom then
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite:setEnabled(false)
    --end

    -- if not GlobalUserItem.bVideo then
    --     -- 语音按钮 gameviewlayer -> gamelayer -> clientscene
    --     self._scene._scene:createVoiceBtn(cc.p(1280, 220), 0, top)
    -- end

    btn = csbNode:getChildByName("voice_btn")
    btn:setTag(TAG_ENUM.BT_VOICE)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)
    table.insert(self.m_tabBtn,btn)

    btn:addTouchEventListener(function (ref, eventType)
        if self._scene.m_bLookOn  then
            showToast(self, "观战中不可使用该功能!", 2)
            return 
        end

        if eventType == ccui.TouchEventType.began then

            self:getParentNode():getParentNode():startVoiceRecord()
        elseif eventType == ccui.TouchEventType.ended 
            or eventType == ccui.TouchEventType.canceled then
            self:getParentNode():getParentNode():stopVoiceRecord()
        end
    end)

    -- 底分
    self.m_atlasDiFeng = top:getChildByName("atlas_score")
    self.m_atlasDiFeng:setString("")
    --顶部菜单
    ------

    local tabCardPosition = 
    {
        cc.p(320, 445),
        cc.p(650, 105),
        cc.p(1040, 445)
    }

    --顶部三张底牌
    local tabBankerCardPosition = 
    {
        cc.p(-103, 7.5),
        cc.p(-44.5, 7.5),
        cc.p(13, 7.5)
    }

    ------
    --用户状态
    local userState = csbNode:getChildByName("userstate_control")  

    --标签
    local str = ""
    local tag = 0
    for i = 1, 3 do
        -- 准备标签
        str = "ready" .. i
        local tmpsp = userState:getChildByName(str)
        self.m_tabReadySp[i] = tmpsp

        -- 状态标签
        str = "state_sp" .. i
        tmpsp = userState:getChildByName(str)
        self.m_tabStateSp[i] = tmpsp

        -- 扑克牌
        self.m_tabNodeCards[i] = CardsNode:createEmptyCardsNode(i,self)
        self.m_tabNodeCards[i]:setPosition(tabCardPosition[i])
        self.m_tabNodeCards[i]:setListener(self)
        self.m_cardControl:addChild(self.m_tabNodeCards[i])

        local tag = self.m_tabNodeCards[i]:getTag()

        -- 庄家扑克牌
        tmpsp = CardSprite:createCard(0)
        tmpsp:setScale(0.3)
        tmpsp:setVisible(false)
        tmpsp:setPosition(tabBankerCardPosition[i])
        self.m_nodeBankerCard:addChild(tmpsp)
        self.m_tabBankerCard[i] = tmpsp

        -- 报警动画
        tmpsp = self.m_cardControl:getChildByName("alarm_" .. i)
        self.m_tabSpAlarm[i] = tmpsp
    end

    --用户状态
    ------
    ------
    --叫分控制
    local callScore = csbNode:getChildByName("callscore_control")
    self.m_callScoreControl = callScore
    self.m_callScoreControl:setVisible(false)
    --叫分按钮
    for i = 1, 4 do
        str = "score_btn" .. i
        btn = callScore:getChildByName(str)
        btn:setTag(TAG_ENUM.BT_CALLSCORE0 + i - 1)
        btn:addTouchEventListener(btnEvent)
        self.m_tabCallScoreBtn[i] = btn
    end

    --叫分控制
    ------

    ------
    --操作控制
    local onGame = csbNode:getChildByName("ongame_control")
    self.m_onGameControl = onGame
    self.m_tabOutButtonPosX = {}

    --不出按钮
    btn = onGame:getChildByName("pass_btn")
    btn:setTag(TAG_ENUM.BT_PASS)
    btn:addTouchEventListener(btnEvent)
    self.m_btnPass = btn
    self.m_tabOutButtonPosX[TAG_ENUM.BT_PASS] = btn:getPositionX()

    --提示按钮
    btn = onGame:getChildByName("suggest_btn")
    btn:setTag(TAG_ENUM.BT_SUGGEST)
    btn:addTouchEventListener(btnEvent)
    self.m_btnSuggest = btn
    self.m_tabOutButtonPosX[TAG_ENUM.BT_SUGGEST] = btn:getPositionX()

    --出牌按钮
    btn = onGame:getChildByName("outcard_btn")
    btn:setTag(TAG_ENUM.BT_OUTCARD)
    btn:addTouchEventListener(btnEvent)
    btn:setSwallowTouches(true)
    self.m_btnOutCard = btn
    self.m_tabOutButtonPosX[TAG_ENUM.BT_OUTCARD] = btn:getPositionX()

    ------
    -- 出牌控制
    self.m_outCardsControl = csbNode:getChildByName("outcards_control")
    -- 出牌控制
    ------
    -- 用户信息
    local infoLayout = csbNode:getChildByName("info")
    self.m_userinfoControl = infoLayout

    -- 头像位置
    self.m_tabUserHeadPos[cmd.LEFT_VIEWID] = cc.p(80, 505)
    self.m_tabUserHeadPos[cmd.MY_VIEWID] = cc.p(85, 295)
    self.m_tabUserHeadPos[cmd.RIGHT_VIEWID] = cc.p(1257, 505)

    -- 提示tip
    self.m_spInfoTip = infoLayout:getChildByName("info_tip")

    -- 倒计时
    self.m_spTimer = infoLayout:getChildByName("bg_clock")
    self.m_atlasTimer = self.m_spTimer:getChildByName("atlas_time")
    -- self.m_tabTimerPos[MY_SCORE] = cc.p(500, 324)
    
    -- 用户信息
    ------
    ------
    -- 游戏取消托管
    self.m_trusteeshipControl = csbNode:getChildByName("tru_control")
    self.m_trusteeshipControl:addTouchEventListener(function( ref, tType)
        if tType == ccui.TouchEventType.ended then
            if self._scene.m_bLookOn  then
                return 
            end
            if this.m_trusteeshipControl:isVisible() then
                self._scene:sendCanceOutCard()
            end
        end
    end)

    --动画层
    self.animate_UI = cc.Node:create()
    self:addToRootLayer(self.animate_UI, TAG_ZORDER.CHAT_ZORDER)

    --人物动画
    self.person_UI = csbNode:getChildByName("person_ui")
    self.person_ME_UI = csbNode:getChildByName("person_me_ui")
    -- self.person_UI = cc.Node:create()
    -- self.m_personUi:addChild()

    self:createCardRecord()
    self:freshCardRecord()

    --测试动画
    -- local landAnimate = self:createFarmerInfoThinking()
    -- landAnimate:setPosition(display.cx, display.cy)
    -- self:bombAnimate()

    for i=1, 3 do
        local scale = 1
        if i == cmd.RIGHT_VIEWID then
            scale = -1
        end

        self.m_tabUserHeadAnimate[i] = {}
        local landAnimate = self:createLandInfoAnimate(scale, i)
        landAnimate:setVisible(false)
        landAnimate:setPosition(personPos[i])
        table.insert(self.m_tabUserHeadAnimate[i], landAnimate)

        local farmerAnimate = self:createFarmerInfoAnimate(scale, i)
        farmerAnimate:setVisible(false)
        farmerAnimate:setPosition(personPos[i])
        table.insert(self.m_tabUserHeadAnimate[i], farmerAnimate)
    end

    --wifi
   self.wifi1 = csbNode:getChildByName("wifi_bg"):getChildByName("Node_wifi")
   self.wifi4g = csbNode:getChildByName("wifi_bg"):getChildByName("Node_wifi1")  
   self.wifi4g:setVisible(false)
   self.wifi1:getChildByName("wifi_4"):setVisible(false)
   self.wifi4g:getChildByName("wifi_4"):setVisible(false)

   self.pTimer100 = csbNode:getChildByName("wifi_bg"):getChildByName("LoadingBar_battery")

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
    self.labelTime = csbNode:getChildByName("wifi_bg"):getChildByName("Text_time")
    self.labelTime:setString("00:00")
    local function timeUpdate()
      self.labelTime:setString(""..os.date("%H")..":"..os.date("%M"))
    end
    self._timeShd = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 1, false)
    -- self:test()
end

function GameViewLayer:test()
    self.airAnimation  = sp.SkeletonAnimation:create("animate/farmer/skeleton.json", "animate/farmer/skeleton.atlas", 1):addTo(self)
    self.airAnimation:setPosition(display.cx, display.cy)
    self.airAnimation:setAnimation(0, "animation", true)
    self.airAnimation:setAnimation(0, "xiao", true)
end

function GameViewLayer:createAnimation()
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    -- 火箭动画
    param.m_strName = Define.ROCKET_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    if nil ~= animate then
        local rep = cc.RepeatForever:create(animate)
        self.m_actRocketRepeat = rep
        self.m_actRocketRepeat:retain()
        local moDown = cc.MoveBy:create(0.1, cc.p(0, -20))
        local moBy = cc.MoveBy:create(2.0, cc.p(0, 500))
        local fade = cc.FadeOut:create(2.0)
        local seq = cc.Sequence:create(cc.DelayTime:create(2.0), cc.CallFunc:create(function()

            end), fade)
        local spa = cc.Spawn:create(cc.EaseExponentialIn:create(moBy), seq)
        self.m_actRocketShoot = cc.Sequence:create(cc.CallFunc:create(function( ref )
            ref:runAction(rep)
        end), moDown, spa, cc.RemoveSelf:create(true))
        self.m_actRocketShoot:retain()
    end

    -- 飞机动画    
    param.m_strName = Define.AIRSHIP_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    if nil ~= animate then
        local rep = cc.RepeatForever:create(animate)
        self.m_actPlaneRepeat = rep
        self.m_actPlaneRepeat:retain()
        local moTo = cc.MoveTo:create(3.0, cc.p(0, yl.HEIGHT * 0.5))
        local fade = cc.FadeOut:create(1.5)
        local seq = cc.Sequence:create(cc.DelayTime:create(1.5), cc.CallFunc:create(function()
            ExternalFun.playSoundEffect("common_plane.wav")
            end), fade)
        local spa = cc.Spawn:create(moTo, seq)
        self.m_actPlaneShoot = cc.Sequence:create(cc.CallFunc:create(function( ref )
            ref:runAction(rep)
        end), spa, cc.RemoveSelf:create(true))
        self.m_actPlaneShoot:retain()
    end

    -- 炸弹动画
    param.m_strName = Define.BOMB_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    if nil ~= animate then
        local fade = cc.FadeOut:create(1.0)
        self.m_actBomb = cc.Sequence:create(animate, fade, cc.RemoveSelf:create(true))
        self.m_actBomb:retain()
    end    
end

function GameViewLayer:unloadResource()
    AnimationMgr.removeCachedAnimation(Define.CALLSCORE_ANIMATION_KEY)
    AnimationMgr.removeCachedAnimation(Define.CALLONE_ANIMATION_KEY)
    AnimationMgr.removeCachedAnimation(Define.CALLTWO_ANIMATION_KEY)
    AnimationMgr.removeCachedAnimation(Define.CALLTHREE_ANIMATION_KEY)
    -- AnimationMgr.removeCachedAnimation(Define.AIRSHIP_ANIMATION_KEY)
    AnimationMgr.removeCachedAnimation(Define.ROCKET_ANIMATION_KEY)
    AnimationMgr.removeCachedAnimation(Define.ALARM_ANIMATION_KEY)
    AnimationMgr.removeCachedAnimation(Define.BOMB_ANIMATION_KEY)
    AnimationMgr.removeCachedAnimation(Define.VOICE_ANIMATION_KEY)

    cc.Director:getInstance():getTextureCache():removeTextureForKey("game/card.png")
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/game.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("game/game.png")
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
end
-- 重置
function GameViewLayer:reSetGame( bFreeState )
    -- 叫分控制
    self.m_callScoreControl:setVisible(false)
    -- 出牌控制
    self.m_onGameControl:setVisible(false)
    -- 清理桌面
    self.m_outCardsControl:removeAllChildren()
    --清理剩余牌
    for k, v in pairs(self.m_tabCardCount) do
        v:setString("")
        if k == cmd.MY_VIEWID then
            v:setString("0")
        end
    end

    self.isAutoOutCard = false

    self:freshCardRecord()

    -- 玩家状态
    if not bFreeState then
        self:reSetUserState()
    end

    if self.animate_UI then
        self.animate_UI:removeAllChildren()
    end

    self.m_spTimer:setVisible(false)
    self.m_atlasTimer:setString("")
    -- 取消托管
    self.m_trusteeshipControl:setVisible(false)

    self.m_bMyCallBanker = false
    self.m_bMyOutCards = false

    if nil ~= self.m_resultLayer then
        self.m_resultLayer:hideGameResult()
    end

end

-- 重置(新一局)
function GameViewLayer:reSetForNewGame()
    self:stopAllActions()
    -- 清理手牌
    for k,v in pairs(self.m_tabNodeCards) do
        v:removeAllCards()

        self.m_tabSpAlarm[k]:stopAllActions()
        self.m_tabSpAlarm[k]:setSpriteFrame("land_blank.png")
    end
    for k,v in pairs(self.m_tabCardCount) do
        v:setString("")
        if k == cmd.MY_VIEWID then
            v:setString("0")
        end
    end
    
    -- 庄家叫分
    self.m_textGameCall:setString("0")
    -- 庄家扑克
    for k,v in pairs(self.m_tabBankerCard) do
        v:setVisible(false)
        v:setCardValue(0)
    end
    -- 用户切换
    for k,v in pairs(self.m_tabUserHead) do
        if v then
            v:reSet()
        end
    end

     --清除卡通头像
    for k, v in pairs(self.m_tabUserHeadAnimate) do
        for m, n in pairs(self.m_tabUserHeadAnimate[k]) do
            if n then
                n:setVisible(false)
            end
        end
    end

    if MatchRoom and GlobalUserItem.bMatch then
        self._scene:KillGameClock()
    end

    --结算界面
    if nil ~= self.m_resultLayer then
        self.m_resultLayer:hideGameResult()
    end
end

--清除结束时闹钟和报警
function GameViewLayer:cleanDesk()
    for k,v in pairs(self.m_tabSpAlarm) do
        self.m_tabSpAlarm[k]:stopAllActions()
        self.m_tabSpAlarm[k]:setSpriteFrame("land_blank.png")
    end
    if self.m_spTimer then
        self.m_spTimer:setVisible(false)
    end
end

--隐藏右侧里边框
function GameViewLayer:cleanListBg()
    if self.m_spSwitchBg:isVisible() then
        self.m_spSwitchBg:setVisible(false)
    end
end

--创建地主动画
function GameViewLayer:createLandInfoAnimate(scale, viewId)
    local temp  = sp.SkeletonAnimation:create("animate/land/skeleton.json", "animate/land/skeleton.atlas", 1)
    temp:setScaleX(scale)

    if viewId == cmd.MY_VIEWID then
        temp:addTo(self.person_ME_UI)
    else
        temp:addTo(self.person_UI)
    end
    return temp
end

function GameViewLayer:createFarmerInfoAnimate(scale, viewId)
    local temp  = sp.SkeletonAnimation:create("animate/farmer/skeleton.json", "animate/farmer/skeleton.atlas", 1)
    temp:setScaleX(scale)
    if viewId == cmd.MY_VIEWID then
        temp:addTo(self.person_ME_UI)
    else
        temp:addTo(self.person_UI)
    end
    return temp
end

-- 重置用户状态
function GameViewLayer:reSetUserState()
    for k,v in pairs(self.m_tabReadySp) do
        v:setVisible(false)
    end

    for k,v in pairs(self.m_tabStateSp) do
        v:setSpriteFrame("land_blank.png")
    end
end

-- 重置用户信息
function GameViewLayer:reSetUserInfo()
    local userItem = self._scene:GetMeUserItem()
    local name =  string.EllipsisByConfig(userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28))
    
    self.playerNode:getChildByName("Text_name"):setString(name)
    self.playerNode:getChildByName("Text_gold"):setString(userItem.lScore)
end

function GameViewLayer:onExit()
    if nil ~= self.m_actRocketRepeat then
        self.m_actRocketRepeat:release()
        self.m_actRocketRepeat = nil
    end

    if nil ~= self.m_actRocketShoot then
        self.m_actRocketShoot:release()
        self.m_actRocketShoot = nil
    end

    if nil ~= self.m_actPlaneRepeat then
        self.m_actPlaneRepeat:release()
        self.m_actPlaneRepeat = nil
    end

    if nil ~= self.m_actPlaneShoot then
        self.m_actPlaneShoot:release()
        self.m_actPlaneShoot = nil
    end

    if nil ~= self.m_actBomb then
        self.m_actBomb:release()
        self.m_actBomb = nil
    end

    -- self.person_UI:removeAllChildren()

    self:unloadResource()

    self.m_tabUserItem = {}
    if self._timeShd then  
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeShd)
        self._timeShd = nil
    end

    if self.timer then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timer)
        self.timer = nil
    end

    if self._timeStatus then
         cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeStatus)
         self._timeStatus = nil
    end

    if self.animate_UI then
        self.animate_UI:removeAllChildren()
    end
end

function GameViewLayer:showSitBtn(isVisible)
    self.m_btnSit:setVisible(isVisible)
end

function GameViewLayer:lookOnSetVisibleBtn(visible)
    
    for k,v in pairs(self.m_tabBtn) do
        v:setVisible(visible)
    end
end


function GameViewLayer:onButtonClickedEvent(tag, ref)   
    ExternalFun.playClickEffect()
    if TAG_ENUM.BT_SWITCH_1 == tag then         -- 开关1
        if self.m_spSwitchBg:isVisible() then
            self.m_spSwitchBg:setVisible(false)
        else
            self.m_spSwitchBg:setVisible(true)
        end
    elseif TAG_ENUM.BT_BG == tag then
        self:cleanListBg()
    elseif TAG_ENUM.BT_CHAT == tag then             --聊天   
        if self._scene.m_bLookOn  then
            showToast(self._scene, "观战中不可使用该功能!", 2)
            return 
        end

        local tabconfig = {}
        tabconfig.csbfile = "game/qipai/land/res/chat/GameChatLayer.csb"
        tabconfig.szItemFrameName = "game/qipai/land/res/chat/word_line.png"

        if nil == self.m_chatLayer then
            self.m_chatLayer = GameChatLayer:create(self._scene._gameFrame, tabconfig)
            self:addToRootLayer(self.m_chatLayer, TAG_ZORDER.CHAT_ZORDER)
        end
        self.m_chatLayer:showGameChat(true)
        self:cleanListBg()
    elseif TAG_ENUM.BT_TRU == tag then

        if self._scene.m_bLookOn  then
            showToast(self, "观战中不可使用该功能!", 2)
            return 
        end

        if self._scene.m_cbGameStatus ~= cmd.GAME_SCENE_FREE then
        --托管
            self:onGameTrusteeship(true)
        else
        --换桌
            self.m_spSwitchBg:setVisible(false)
            self.m_btnSwitch_1:setVisible(true)
            self.m_btnSwitch_1:setEnabled(true)

            if self._scene.m_cbGameStatus == cmd.GAME_SCENE_FREE or self._scene.m_cbGameStatus ==  cmd.GAME_SCENE_END then
                
      
                for k,v in pairs(self.m_tabUserHead) do
                  if v then
                       v:setVisible(false)
                       
                  end
                end


                for i=1,3 do
                    self:onUserReady(i, false)
                end

                self.m_btnReady:setVisible(true)
                self.m_bIsChange = true
                self._scene._gameFrame:QueryChangeDesk()
            else
                showToast(self, "正在游戏中，请先结束游戏", 2)
            end
        end

        self:cleanListBg()
    elseif TAG_ENUM.BT_SET == tag then          --设置
        local set = SettingLayer:create( self )
        self:addChild(set)
        self:cleanListBg()
    elseif TAG_ENUM.BT_HELP == tag then
        self:createHelp()
        self:cleanListBg()
    elseif TAG_ENUM.BT_LOCATION == tag then
        if self._scene.m_bLookOn  then
            showToast(self, "观战中不可使用该功能!", 2)
            return 
        end
        self:showAdressLayer()
        self:cleanListBg()
    elseif TAG_ENUM.BT_EXIT == tag then         --退出



        if MatchRoom and GlobalUserItem.bMatch then 
            --self:onGameTrusteeship(true)
            showToast(self, "比赛进行中,暂不能退出", 2)
            return
        end
        dump({self._scene.m_bLookOn},"===============")
        if self._scene.m_bLookOn  then
            self._scene:onExitRoom()
            return 
        end

        self._scene:onQueryExitGame()
    elseif TAG_ENUM.BT_READY == tag then        --准备
        self:onClickReady()
        self:cleanListBg()
    elseif TAG_ENUM.BT_SITDOWN == tag then        --旁观坐下 
        self._scene._gameFrame.bChangeDesk = true
        self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)


    elseif TAG_ENUM.BT_INVITE == tag then       -- 邀请
        GlobalUserItem.bAutoConnect = false
        self._scene._scene:popTargetShare(function(target, bMyFriend)
            bMyFriend = bMyFriend or false
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    --showToast(self, "分享成功", 2)
                end
                GlobalUserItem.bAutoConnect = true
            end
            local shareTxt = "斗地主游戏精彩刺激, 一起来玩吧! "
            local url = GlobalUserItem.szSpreaderURL or yl.HTTP_URL
            if bMyFriend then
                PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_FRIENDLIST, function( frienddata )
                    dump(frienddata)
                end)
            elseif nil ~= target then
                MultiPlatform:getInstance():shareToTarget(target, sharecall, "斗地主游戏邀请", shareTxt, url, "")
            end
        end)        
        self:cleanListBg()
    elseif TAG_ENUM.BT_CALLSCORE0 == tag then   --不叫
        ExternalFun.playSoundEffect( "cs0.wav", self._scene:GetMeUserItem())
        self._scene:sendCallScore(255)
        self.m_callScoreControl:setVisible(false)
        self:cleanListBg()
    elseif TAG_ENUM.BT_CALLSCORE1 == tag then   --一分
        ExternalFun.playSoundEffect( "cs1.wav", self._scene:GetMeUserItem())
        self._scene:sendCallScore(1)
        self.m_callScoreControl:setVisible(false)
        self:cleanListBg()
    elseif TAG_ENUM.BT_CALLSCORE2 == tag then   --两分
        ExternalFun.playSoundEffect( "cs2.wav", self._scene:GetMeUserItem())
        self._scene:sendCallScore(2)
        self.m_callScoreControl:setVisible(false)
        self:cleanListBg()
    elseif TAG_ENUM.BT_CALLSCORE3 == tag then   --三分
        ExternalFun.playSoundEffect( "cs3.wav", self._scene:GetMeUserItem())
        self._scene:sendCallScore(3)
        self.m_callScoreControl:setVisible(false)
        self:cleanListBg()
    elseif TAG_ENUM.BT_PASS == tag then         --不出
        self:onPassOutCard()
        self:cleanListBg()
    elseif TAG_ENUM.BT_SUGGEST == tag then      --提示
        self:onPromptOut(false)        
        self:cleanListBg()
    elseif TAG_ENUM.BT_OUTCARD == tag then      --出牌
        local sel = self.m_tabNodeCards[cmd.MY_VIEWID]:getSelectCards()
        if 0 == #sel then
            return
        end
        -- 扑克对比
        self._scene:compareWithLastCards(sel, cmd.MY_VIEWID)

        self.m_onGameControl:setVisible(false)
        -- local vec = self.m_tabNodeCards[cmd.MY_VIEWID]:outCard(sel)
        -- self:outCardEffect(cmd.MY_VIEWID, sel, vec)
        self._scene:sendOutCard(sel)
        self:cleanListBg()
    elseif TAG_ENUM.BT_RECORD == tag then
        if not self.leftCardCsb then
            self:createCardRecord()
        end

        if self.leftCardCsb:isVisible() then
            self.leftCardCsb:setVisible(false)
        else
            self.leftCardCsb:setVisible(true)
        end
        self:cleanListBg()
    end
end

function GameViewLayer:createHelp()
    local rootLayer, csbNode = ExternalFun.loadRootCSB("help/help.csb", self)
    csbNode:getChildByName("mask"):addClickEventListener(function (sender)
                                                                        csbNode:removeFromParent()
                                                                    end)
    csbNode:getChildByName("Button_exit"):addClickEventListener(function (sender)
                                                                         csbNode:removeFromParent()
                                                                    end)
end

function GameViewLayer:showAdressLayer()
    local playerList = {}
    local playerNum = 3
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
    local uiLocation = require("game.qipai.land.src.views.layer.uiLocation")
    local locationObj = uiLocation:ctor(self, playerList, validPlayerNum)
end

function GameViewLayer:createCardRecord()
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/cardRecord.csb", self.m_csbNode)
    self.leftCardCsb = csbNode
    self.leftCardCsb:setZOrder(TAG_ZORDER.RECORDCSB_ZORDER)

    csbNode:getChildByName("record_bg"):setPosition(cc.p(710, 36))

    self.leftCardCsb:setVisible(false)

    local function onButtonClickedEvent(tag, ref)
        if tag == TAG_ENUM.BT_RECORDMASK or tag == TAG_ENUM.BT_RECORDBG then
            if self.leftCardCsb then
                self.leftCardCsb:setVisible(false)
            end
        end
    end

    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.began then
            onButtonClickedEvent(sender:getTag(), sender)
        end
    end

    local btn = csbNode:getChildByName("mask")
    btn:setTag(TAG_ENUM.BT_RECORDMASK)
    btn:setSwallowTouches(true)
    btn:addTouchEventListener(btnEvent)
end

function GameViewLayer:freshCardRecord(leftCard)
    if not self.m_tabNodeCards[cmd.MY_VIEWID] then
        return 
    end

    if not self.leftCardCsb then
        return
    end

    local mySelfCount = self.m_tabNodeCards[cmd.MY_VIEWID]:getHandCardsCounts()

    if not leftCard or next(leftCard) == nil then
        for i=1, 15 do
            self.leftCardCsb:getChildByName("record_bg"):getChildByName("record_"..i):setString("")
        end
        return
    end
    local data = leftCard.cbRemainCount[1]

    for i=1, 15 do
        local tmpData = data[i] - mySelfCount[i]
        if tmpData < 0 then 
            tmpData = 0
        end
        self.leftCardCsb:getChildByName("record_bg"):getChildByName("record_"..i):setString(tmpData)
    end
end

function GameViewLayer:onClickReady()
    --self.m_btnReady:setEnabled(false)
    --self.m_btnReady:setVisible(false)

    self._scene:sendReady()

    if self._scene.m_bRoundOver then
        self._scene.m_bRoundOver = false
        -- 界面清理
        self:reSetForNewGame()
    end 
end

--根据状态，切换托管和换桌按钮图片
function GameViewLayer:updateButtonImg(playMode)
    local btn = self.m_spSwitchBg:getChildByName("tru_btn")
    if PriRoom and GlobalUserItem.bPrivateRoom and GlobalUserItem.bPrivateGoldRoom then
        btn:loadTextureNormal("menu/setting/menu_0.png")
        btn:loadTexturePressed("menu/setting/menu_0.png")
        btn:loadTextureDisabled("menu/setting/menu_1.png")
        
        if self._scene.m_cbGameStatus == cmd.GAME_SCENE_FREE then
            btn:setEnabled(false)
        else
            btn:setEnabled(true)
        end
    elseif PriRoom and GlobalUserItem.bPrivateRoom then
        btn:setEnabled(false)
    else
        if self._scene.m_cbGameStatus == cmd.GAME_SCENE_FREE then
            btn:loadTextureNormal("menu/setting/desk1.png")
            btn:loadTexturePressed("menu/setting/desk2.png")
            btn:loadTextureDisabled("menu/setting/desk3.png")
        else
            btn:loadTextureNormal("menu/setting/menu_0.png")
            btn:loadTexturePressed("menu/setting/menu_0.png")
            btn:loadTextureDisabled("menu/setting/menu_1.png")
        end
    end
end

-- 出牌效果
-- @param[outViewId]        出牌视图id
-- @param[outCards]         出牌数据
-- @param[vecCards]         扑克精灵
function GameViewLayer:outCardEffect(outViewId, outCards, vecCards)
    local controlSize = self.m_outCardsControl:getContentSize()
    -- 移除出牌
    self.m_outCardsControl:removeChildByTag(outViewId)
    local holder = cc.Node:create()
    self.m_outCardsControl:addChild(holder)
    holder:setTag(outViewId)

    local outCount = #outCards
    -- 计算牌型
    local cardType = GameLogic:GetCardType(outCards, outCount)
    if GameLogic.CT_THREE_TAKE_ONE == cardType then
        if outCount > 4 then
            cardType = GameLogic.CT_THREE_LINE
        end        
    end
    if GameLogic.CT_THREE_TAKE_TWO == cardType then
        if outCount > 5 then
            cardType = GameLogic.CT_THREE_LINE
        end        
    end

    -- 出牌
    local targetPos = cc.p(0, 0)
    local subTargetPos = cc.p(0, 0)
    local center = outCount * 0.5
    local scale = 0.45
    holder:setPosition(self.m_tabNodeCards[outViewId]:getPosition())
    if cmd.MY_VIEWID == outViewId then
        scale = 0.55
        targetPos = holder:convertToNodeSpace(cc.p(controlSize.width * 0.5, controlSize.height * 0.4))
    elseif cmd.LEFT_VIEWID == outViewId then
        center = 0
        holder:setAnchorPoint(cc.p(0, 0.5))
        targetPos = holder:convertToNodeSpace(cc.p(380, controlSize.height * 0.58))
        subTargetPos = holder:convertToNodeSpace(cc.p(410, controlSize.height * 0.54))
    elseif cmd.RIGHT_VIEWID == outViewId then
        center = outCount
        if outCount > 8 then
            center = 8
        end
        holder:setAnchorPoint(cc.p(1, 0.5))
        targetPos = holder:convertToNodeSpace(cc.p(980, controlSize.height * 0.58))
        subTargetPos = holder:convertToNodeSpace(cc.p(950, controlSize.height * 0.54))
    end
    local nIdx = 0
    local yPos = targetPos.y + ADJUSTGAP
    for k,v in pairs(vecCards) do
        v:retain()
        v:removeFromParent()
        holder:addChild(v)
        v:release()

        v:showCardBack(false)
        if cmd.MY_VIEWID ~= outViewId and k == 9 then
            nIdx = 0
            yPos = subTargetPos.y 
        end
        local pos = cc.p((nIdx - center) * CardsNode.CARD_X_DIS * scale + targetPos.x, yPos)
        local moveTo = cc.MoveTo:create(0.3, pos)
        local spa = cc.Spawn:create(moveTo, cc.ScaleTo:create(0.3, scale))
        v:stopAllActions()
        v:runAction(spa)
        nIdx = nIdx + 1
    end

    local headitem = self.m_tabUserHead[outViewId]
    if nil == headitem then
        return
    end

    -- 牌型音效
    local bCompare = self._scene.m_bLastCompareRes
    if GameLogic.CT_SINGLE == cardType then
        -- 音效
        local poker = yl.POKER_VALUE[outCards[1]]
        if nil ~= poker then
            ExternalFun.playSoundEffect(poker .. ".wav", headitem.m_userItem) 
        end        
    else
        if bCompare then
            -- 音效
            ExternalFun.playSoundEffect("ya" .. math.random(0, 1) .. ".wav", headitem.m_userItem) 
        else
            -- 音效
            ExternalFun.playSoundEffect( "type" .. cardType .. ".wav", headitem.m_userItem)
        end
    end

    self.m_rootLayer:removeChildByName("__effect_ani_name__")
    -- 牌型动画/牌型音效
    if GameLogic.CT_THREE_LINE == cardType then             -- 飞机
        -- self:runAction(cc.Sequence:create(cc.CallFunc:create(function() self:airAnimate() end),
        --                                   cc.DelayTime:create(1), 
        --                                   cc.CallFunc:create(function() 
        --                                                         if self.airAnimation then 
        --                                                             self.airAnimation:setVisible(false) 
        --                                                         end
        --                                                     end)
        --                                   ))

        self:airAnimate()
    elseif GameLogic.CT_BOMB_CARD == cardType then          -- 炸弹
        self:bombAnimate()
        self:shakeScreen()
        -- self:outCardSmileAnimate(outViewId)
    elseif GameLogic.CT_MISSILE_CARD == cardType then       -- 火箭
        self:rocketAnimate()
        self:shakeScreen()
        -- self:outCardSmileAnimate(outViewId)
    end
end

--好牌笑动画
function GameViewLayer:outCardSmileAnimate(viewId)
    if self.bankViewID == viewId then
            local temp = self.m_tabUserHeadAnimate[viewId][1]
            self.m_tabUserHeadAnimate[viewId][1]:setAnimation(0, "xiao", false)
            self:runAction(cc.Sequence:create (cc.DelayTime:create(2), 
                                        cc.CallFunc:create(function() 
                                        self.m_tabUserHeadAnimate[viewId][1]:setAnimation(0, "sikao", true)
                        end)))
    else
        local temp = self.m_tabUserHeadAnimate[viewId][2]
        self.m_tabUserHeadAnimate[viewId][2]:setAnimation(0, "xiao", false)
        self:runAction(cc.Sequence:create(cc.DelayTime:create(2), 
                                        cc.CallFunc:create(function() 
                                        self.m_tabUserHeadAnimate[viewId][2]:setAnimation(0, "sikao", true)
                        end))) 
    end
end

--震屏动画
function GameViewLayer:shakeScreen()
    local shake = ShakeLayer:create(self, 0.25)
    shake:shakeRun()
end

--炸弹动画
function GameViewLayer:bombAnimate()
    ExternalFun.playSoundEffect( "common_bomb.wav" ) 
    self.bombAnimation = cc.CSLoader:createNode("animate/boom/boom.csb"):addTo(self)
    self.bombAnimation:setScale(1.5)
    self.bombAnimation:setPosition(-yl.WIDTH * 0.5-80, -yl.HEIGHT*0.25)
    local ani = cc.CSLoader:createTimeline("animate/boom/boom.csb")
    ani:gotoFrameAndPlay(0, 20, false )
    self.bombAnimation:runAction(ani)
end

--反春天
function GameViewLayer:reversSpringAnimate()
    self.reversSpringAnimation = cc.CSLoader:createNode("animate/spring_resverse/spring_fan.csb"):addTo(self.animate_UI)
    local ani = cc.CSLoader:createTimeline("animate/spring_resverse/spring_fan.csb")
    ani:gotoFrameAndPlay(0, 36, false)
    self.reversSpringAnimation:runAction(ani)
end

--春天
function GameViewLayer:springAnimate()
    self.springAnimation  = sp.SkeletonAnimation:create("animate/spring/skeleton.json", "animate/spring/skeleton.atlas", 1):addTo(self.animate_UI)
    self.springAnimation:setPosition(yl.WIDTH*0.5, yl.HEIGHT*0.5)
    self.springAnimation:setAnimation(0, "animation",false)
end

--火箭
function GameViewLayer:rocketAnimate()
    self.rocketAnimation = cc.CSLoader:createNode("animate/rocket/rocket.csb"):addTo(self.animate_UI)
    local ani = cc.CSLoader:createTimeline("animate/rocket/rocket.csb")
    ani:gotoFrameAndPlay(0, 36, false)
    self.rocketAnimation:runAction(ani)
end

--飞机
function GameViewLayer:airAnimate()
    self.airAnimation  = sp.SkeletonAnimation:create("animate/air/skeleton.json", "animate/air/skeleton.atlas", 1):addTo(self.animate_UI)
    self.airAnimation:setPosition(display.cx, display.cy)
    self.airAnimation:setAnimation(0, "animation", false)
end

--大厅树动画
function GameViewLayer:hallTreeAnimate()
    local node = self.m_csbNode:getChildByName("Node_hallAnimate")
    self.hallAnimation  = sp.SkeletonAnimation:create("animate/tree/skeleton.json", "animate/tree/skeleton.atlas", 1):addTo(node)
    self.hallAnimation:setPosition(yl.WIDTH*0.5+70, yl.HEIGHT*0.5+28)
    self.hallAnimation:setAnimation(0, "animation", true)
end

--大厅云动画
function GameViewLayer:hallCloudAnimate()
    local node = self.m_csbNode:getChildByName("Node_hallAnimate")
    self.hallAnimation  = sp.SkeletonAnimation:create("animate/cloud/skeleton.json", "animate/cloud/skeleton.atlas", 1):addTo(node)
    self.hallAnimation:setPosition(yl.WIDTH*0.5+70, yl.HEIGHT*0.5)
    self.hallAnimation:setAnimation(0, "animation", true)
end

function GameViewLayer:onChangePassBtnState( bEnable )
    self.m_btnPass:setEnabled(bEnable)
    if bEnable then
        self.m_btnPass:setOpacity(255)
    else
        self.m_btnPass:setOpacity(125)
    end
end

function GameViewLayer:onPassOutCard()
    self._scene:sendOutCard({}, true)
    self.m_tabNodeCards[cmd.MY_VIEWID]:reSetCards()
    self.m_onGameControl:setVisible(false)
    -- 提示
    self.m_spInfoTip:setSpriteFrame("land_blank.png")
    -- 显示不出
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game_nooutcard.png")
    if nil ~= frame then
        self.m_tabStateSp[cmd.MY_VIEWID]:setSpriteFrame(frame)
    end

    -- 音效
    ExternalFun.playSoundEffect( "pass" .. math.random(0, 1) .. ".wav", self._scene:GetMeUserItem())
end

function GameViewLayer:getUserNick( viewId )
    if nil ~= self.m_tabUserHead[viewId] then
        return self.m_tabUserHead[viewId].m_userItem.szNickName
    end
    return ""
end

------
-- 扑克代理

-- 扑克选择预处理
-- @param[vecSelectCard]    拖动选择的扑克
-- @param[bRelate]          关联操作
function GameViewLayer:onPreSelectedCards( vecSelectCard, bRelate )
    bRelate = bRelate or false
    local nSelectCount = #vecSelectCard
    local bRes = false
    local vecNeedShoot = {}
    if nSelectCount > 1 and not bRelate then
        -- 是否有已弹出牌
        local bShooted = false
        local vecCardData = {}
        local tmpMapCard = {}
        for k,v in pairs(vecSelectCard) do
            -- 已弹出, 跳出处理
            if v:getCardShoot() then
                bShooted = true
                break
            end
            table.insert(vecCardData, v:getCardData())
            tmpMapCard[v:getCardData()] = v
        end

        if not bShooted then
            -- 构造关联(滑动选择牌数目 全包含 可出提示牌)
            local promptCardList = self._scene.m_tabPromptList
            local promptCount = #promptCardList
            local bRelat = false
            local tabRelatCards = {}

            local tmpPromp = clone(promptCardList)
            table.sort(tmpPromp, function(a, b)
                return #a > #b
            end)

            local compareSele = clone(vecCardData)
            compareSele = GameLogic:SortCardList(compareSele, nSelectCount, 0)
            for i = 1, promptCount do
                local cardlist = tmpPromp[i]
                tabRelatCards = {}
                local containcount = 0
                for k,v in pairs( cardlist ) do
                    for m,n in pairs(compareSele) do
                        if n == v then
                            containcount = containcount + 1
                        end
                    end
                end
                bRelat = (containcount == #cardlist) --(containcount > 2) and (containcount == #cardlist)
                if bRelat then
                    bRes = true
                    tabRelatCards = cardlist
                    break
                end
            end

            if bRelat then
                for k, v in pairs(tabRelatCards) do
                    table.insert(vecNeedShoot, tmpMapCard[v])
                end
            end
        end
    end
    return bRes, vecNeedShoot
end

-- 扑克状态变更
-- @param[cbCardData]       扑克数据
-- @param[status]           状态(ture:选取、false:非选取)
-- @param[cardsNode]        扑克节点
function GameViewLayer:onCardsStateChange( cbCardData, status, cardsNode )
    
end

--托管
function GameViewLayer:onSubAutoOutCard(cmd_table)
    local isAuto = cmd_table.bTrustee
    local viewId = self._scene:SwitchViewChairID(cmd_table.wTrustee)
    if viewId == cmd.MY_VIEWID then
        if isAuto == 1 then
            self.isAutoOutCard = true
            self.m_trusteeshipControl:setVisible(true)
            self.m_callScoreControl:setVisible(false)
        elseif isAuto == 0 then
            self.isAutoOutCard = false
            self.m_trusteeshipControl:setVisible(false)

            if self._scene.m_cbGameStatus == cmd.GAME_SCENE_PLAY and self.currentPlayer~= nil and self.currentPlayer == cmd.MY_VIEWID then
                --取消托管显示按钮
                self.m_onGameControl:setVisible(true)
            end
        end
    end
end

--断线重连托管
function GameViewLayer:updateAutoOutCard(isAutoTable)
    for i=1, 3 do
        local viewId = self._scene:SwitchViewChairID(i-1)

        if viewId == cmd.MY_VIEWID then
            if isAutoTable[i] == 1 then
                self.isAutoOutCard = true
                self.m_trusteeshipControl:setVisible(true)
                self.m_callScoreControl:setVisible(false)
            elseif isAutoTable[i] == 0 then
                self.isAutoOutCard = false
                self.m_trusteeshipControl:setVisible(false)
            end
        end
    end
end

-- 扑克选择
-- @param[selectCards]      选择扑克
-- @param[cardsNode]        扑克节点
-- @param[newSelect]        新选择的
-- @param[bRelate]          关联操作
-- @param[bCancelCard]      取消选择
function GameViewLayer:onSelectedCards( selectCards, cardsNode, newSelect, bRelate, bCancelCard )
    bRelate = bRelate or false
    -- 出牌对比
    local outCards = self._scene.m_tabCurrentCards
    local outCount = #outCards

    local selectCount = #selectCards
    local selectType = GameLogic:GetCardType(selectCards, selectCount)
    local newSelectCount = #newSelect

    -- 對方出牌
    if outCount > 0 and 1 == selectCount and newSelectCount > 0 and not bRelate and not bCancelCard then
        -- 构造关联
        local promptCardList = self._scene.m_tabPromptList
        local promptCount = #promptCardList
        local bRelat = false
        local tabRelatCards = {}

        local compareSele = clone(newSelect)
        compareSele = GameLogic:SortCardList(compareSele, newSelectCount, 0)
        print(" new select ", newSelectCount)
        -- 单选牌
        if 1 == newSelectCount then
            local singleSelectedCard = compareSele[1]
            for i = promptCount, 1, -1 do
                local cardlist = promptCardList[i]
                tabRelatCards = {}
                for k,v in pairs( cardlist ) do
                    if v == singleSelectedCard and 1 ~= #cardlist then
                        print("单选关联")
                        bRelat = true
                        tabRelatCards = cardlist
                        break
                    end
                end
                if bRelat then
                    break
                end
            end
        else
            for i = promptCount, 1, -1 do
                local cardlist = promptCardList[i]
                tabRelatCards = {}
                local samecount = 0
                for k,v in pairs( cardlist ) do
                    for m,n in pairs(compareSele) do
                        if n == v then
                            samecount = samecount + 1
                        end
                    end
                end
                bRelat = (samecount == newSelectCount)
                if bRelat and newSelectCount == #cardlist then
                    bRelat = false
                    print("同牌过滤")
                end
                if bRelat then
                    print("多选关联")
                    tabRelatCards = cardlist
                    break
                end
            end
        end

        if bRelat and #tabRelatCards > 0 then
            -- 已选扑克
            local sel = self.m_tabNodeCards[cmd.MY_VIEWID]:getSelectCards()
            if #sel > 0 then
                -- 提示回位
                if self.m_tabNodeCards[cmd.MY_VIEWID].m_bSuggested then
                    cardsNode:suggestShootCards(sel)
                else
                -- 选牌回位
                    cardsNode:backShootedCards(sel)
                end
            end
            -- 关联弹出
            cardsNode:suggestShootCards(tabRelatCards, true)
            return
        end
    end

    local enable = false
    local opacity = 125

    if 0 == outCount then
        if true == self.m_bCanOutCard and GameLogic.CT_ERROR ~= selectType then
            enable = true
            opacity = 255
        end        
    elseif GameLogic:CompareCard(outCards, outCount, selectCards, selectCount) and true == self.m_bCanOutCard then
        enable = true
        opacity = 255
    end

    -- 当按钮不可见时不处理
    if self.m_btnOutCard:isVisible() then
        self.m_btnOutCard:setEnabled(enable)
        self.m_btnOutCard:setOpacity(opacity)
    end
end

-- 牌数变动
-- @param[outCards]         出牌数据
-- @param[cardsNode]        扑克节点
function GameViewLayer:onCountChange( count, cardsNode, isOutCard )
    isOutCard = isOutCard or false
    local viewId = cardsNode.m_nViewId

    local tmpCount = 0
    self.leftCardNum[viewId] = count
    for k, v in pairs (self.leftCardNum) do
        if k ~= cmd.MY_VIEWID then
            tmpCount = tmpCount + v
        end
    end

    if nil ~= self.m_tabCardCount[viewId] then
        if viewId == cmd.MY_VIEWID then
        else
            self.m_tabCardCount[viewId]:setString(count .. "")
        end
    end

    self.m_tabCardCount[cmd.MY_VIEWID]:setString(tmpCount .. "")

    if count <= 2 and nil ~= self.m_tabSpAlarm[viewId] and isOutCard then
        local param = AnimationMgr.getAnimationParam()
        param.m_fDelay = 0.1
        param.m_strName = Define.ALARM_ANIMATION_KEY
        local animate = AnimationMgr.getAnimate(param)
        local rep = cc.RepeatForever:create(animate)
        self.m_tabSpAlarm[viewId]:runAction(rep)

        -- 音效
        ExternalFun.playSoundEffect( "common_alert.wav" )
    else
        self.m_tabSpAlarm[viewId]:stopAllActions()
        self.m_tabSpAlarm[viewId]:setSpriteFrame("land_blank.png")
    end
end

------
-- 扑克代理

-- 提示出牌
-- @param[bOutCard]        是否出牌
function GameViewLayer:onPromptOut( bOutCard ,isAuto)
    bOutCard = bOutCard or false
    if bOutCard then
        local promptCard = self._scene.m_tabPromptCards
        local promptCount = #promptCard
        if promptCount > 0 and not self.m_btnPass:isEnabled() then

            if isAuto and self.m_btnPass:isEnabled() then
                    self:onPassOutCard()
                    return 
            end

            promptCard = GameLogic:SortCardList(promptCard, promptCount, 0)

            -- 扑克对比
            self._scene:compareWithLastCards(promptCard, cmd.MY_VIEWID)

            local vec = self.m_tabNodeCards[cmd.MY_VIEWID]:outCard(promptCard)
            self:outCardEffect(cmd.MY_VIEWID, promptCard, vec)
            self._scene:sendOutCard(promptCard)
            self.m_onGameControl:setVisible(false)
        else
            self:onPassOutCard()
        end
    else
        if 0 >= self.m_promptIdx then
            self.m_promptIdx = #self._scene.m_tabPromptList
        end

        if 0 ~= self.m_promptIdx then
            -- 提示回位
            local sel = self.m_tabNodeCards[cmd.MY_VIEWID]:getSelectCards()
            if #sel > 0 
                and self.m_tabNodeCards[cmd.MY_VIEWID].m_bSuggested
                and #self._scene.m_tabPromptList > 1 then
                self.m_tabNodeCards[cmd.MY_VIEWID]:suggestShootCards(sel)
            end
            -- 提示扑克
            local prompt = self._scene.m_tabPromptList[self.m_promptIdx]
            print("## 提示扑克")
            for k,v in pairs(prompt) do
                print(yl.POKER_VALUE[v])
            end
            print("## 提示扑克")
            if #prompt > 0 then
                self.m_tabNodeCards[cmd.MY_VIEWID]:suggestShootCards(prompt, true)
            else
                self:onPassOutCard()
            end
            self.m_promptIdx = self.m_promptIdx - 1
        else
            self:onPassOutCard()
        end
    end
end

function GameViewLayer:onGameTrusteeship( bTrusteeship )
    local this = self
    --[[-- 随机速率
    local randomDelay = math.random(0.3, 1.5)
    print("托管 随机延迟 ==> ", randomDelay)]]
    if bTrusteeship then
        -- if self.m_bMyCallBanker then
        --     self.m_bMyCallBanker = false
        --     self.m_callScoreControl:setVisible(false)
        --     self.m_callScoreControl:stopAllActions()
        --     -- self.m_onGameControl:setVisible(false)
        --     -- self.m_callScoreControl:runAction(cc.Sequence:create(cc.DelayTime:create(Define.TRU_DELAY), cc.CallFunc:create(function()
        --     --     print("托管 延时叫分")
        --     --     -- this._scene:sendCallScore(255) 
        --     -- end)))    
        -- end

        -- if self.m_bMyOutCards then
        --     self.m_bMyOutCards = false
        --     self.m_onGameControl:stopAllActions()
        --     -- self.m_onGameControl:runAction(cc.Sequence:create(cc.DelayTime:create(Define.TRU_DELAY), cc.CallFunc:create(function()
        --     --     print("托管 延时出牌")
        --     --     -- this:onPromptOut(true,true)
        --     -- end)))
        -- end

        self._scene:sendAutoOutCard()
    end
end

function GameViewLayer:updateClock( clockId, cbTime)
    self.m_atlasTimer:setString( string.format("%02d", cbTime ))
    if cbTime <= 0 then
        if cmd.TAG_COUNTDOWN_READY == clockId then
            --退出防作弊
            self._scene:getFrame():setEnterAntiCheatRoom(false)
            if self._scene.m_bClubRoom and not self._scene.m_bLookOn then

                if PriRoom:getInstance().m_tabPriData.dwPlayCount  == 0 then
                    self._scene:onExitTable()
                -- else
                --     self:onButtonClickedEvent(TAG_ENUM.BT_READY)
                end

            end

        elseif cmd.TAG_COUNTDOWN_CALLSCORE == clockId then
            -- 私人房无自动托管/自己叫分才进入托管
            -- if (not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom) and self.m_bMyCallBanker then
            --     self:onGameTrusteeship(true)
            -- end            
        elseif cmd.TAG_COUNTDOWN_OUTCARD == clockId then
            -- 私人房无自动托管/自己出牌才进入托管
            -- if (not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom) and self.m_bMyOutCards then
            --     self:onGameTrusteeship(true)
            -- end
        end
    end
end

function GameViewLayer:OnUpdataClockView( viewId, cbTime )
    self.m_spTimer:setVisible(cbTime ~= 0)
    self.m_atlasTimer:setString( string.format("%02d", cbTime ))

    if self._scene:IsValidViewID(viewId) then
        if self.m_callScoreControl:isVisible() then
            self.m_spTimer:setPosition(self.m_tabTimerPos[START_SCALL_BANKER])
        elseif self.m_btnReady:isVisible() then
            self.m_spTimer:setPosition(self.m_tabTimerPos[START_READY])
        else
            self.m_spTimer:setPosition(self.m_tabTimerPos[viewId])
        end
    end
end

function GameViewLayer:priGameLayerZorder()
    return TAG_ZORDER.PRIROOM_ZORDER
end

-- 是否托管状态
function GameViewLayer:isTrusteepship()
    return self.m_trusteeshipControl:isVisible()
end

-- 出牌控制调整
-- @param[bOut] 只出牌
-- @param[bPass] 要不起
-- @param[bAll] 所有显示
function GameViewLayer:outControlButton( bOut, bPass, bAll )
    if bOut then
        -- 出牌按钮显示中间
        self.m_btnOutCard:setPositionX(self.m_tabOutButtonPosX[TAG_ENUM.BT_SUGGEST])
        -- 提示隐藏
        self.m_btnSuggest:setVisible(false)
        self.m_btnSuggest:setEnabled(false)
        -- 不出隐藏
        self.m_btnPass:setVisible(false)
        self.m_btnPass:setEnabled(false)
    end

    if bPass then
        -- 切换纹理
        -- self.m_btnPass:loadTextureNormal("game_bt_pass.png",UI_TEX_TYPE_PLIST)
        -- self.m_btnPass:loadTextureDisabled("game_bt_pass.png",UI_TEX_TYPE_PLIST)
        self.m_btnPass:loadTextureNormal("game/game_bt_pass0.png")
        self.m_btnPass:loadTextureDisabled("game/game_bt_pass1.png")
        -- 不出显示中间
        self.m_btnPass:setPositionX(self.m_tabOutButtonPosX[TAG_ENUM.BT_SUGGEST])
        -- 出牌隐藏
        self.m_btnOutCard:setVisible(false)
        self.m_btnOutCard:setEnabled(false)
        -- 提示隐藏
        self.m_btnSuggest:setVisible(false)
        self.m_btnSuggest:setEnabled(false)
    end

    if bAll then
        -- 出牌
        self.m_btnOutCard:setPositionX(self.m_tabOutButtonPosX[TAG_ENUM.BT_OUTCARD])
        self.m_btnOutCard:setVisible(true)
        self.m_btnOutCard:setEnabled(true)
        self.m_btnOutCard:setOpacity(255)

        -- 提示
        self.m_btnSuggest:setPositionX(self.m_tabOutButtonPosX[TAG_ENUM.BT_SUGGEST])
        self.m_btnSuggest:setVisible(true)
        self.m_btnSuggest:setEnabled(true)
        self.m_btnSuggest:setOpacity(255)

        -- 切换纹理
        self.m_btnPass:loadTextureNormal("game/game_nooutcard.png")
        self.m_btnPass:loadTextureDisabled("game/game_nooutcard1.png")
        -- 不出
        self.m_btnPass:setPositionX(self.m_tabOutButtonPosX[TAG_ENUM.BT_PASS])
        self.m_btnPass:setVisible(true)
        self.m_btnPass:setEnabled(true)
        self.m_btnPass:setOpacity(255)
    end
end
------------------------------------------------------------------------------------------------------------
--更新
------------------------------------------------------------------------------------------------------------
-- 文本聊天
function GameViewLayer:onUserChat(chatdata, viewId)
    local roleItem = self.m_tabUserHead[viewId]
    if nil ~= roleItem then
        roleItem:textChat(chatdata.szChatString)
    end
end

-- 表情聊天
function GameViewLayer:onUserExpression(chatdata, viewId)
    local roleItem = self.m_tabUserHead[viewId]
    if nil ~= roleItem then
        roleItem:browChat(chatdata.wItemIndex)
    end
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = 1

        --获取头像坐标
        for k,v in pairs(self.m_tabUserItem) do
              if v then
                  -- local head = self.nodePlayer[k]:getChildByTag(GameViewLayer.SP_HEAD)
                  if v.dwUserID ==  dwUserID then
                        beginPoint = cc.p(self.m_tabUserHead[k]:getPosition())
                        if k == cmd.MY_VIEWID then
                            beginPoint= cc.p(80,80)
                        end
                  elseif v.dwUserID ==  dwRecvUserID then
                        endPoint = cc.p(self.m_tabUserHead[k]:getPosition())
                        zOrder = self.m_tabUserHead[k]:getZOrder() + 1
                        if k == cmd.MY_VIEWID then
                            endPoint= cc.p(80,80)
                        end
                  end
              end
        end

      if beginPoint and endPoint then
              local actSprite = cc.Sprite:create(string.format("public/prop_%d.png",dwPropID)):move(beginPoint):addTo(self.m_userinfoControl,zOrder)
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
                      self.m_userinfoControl:addChild(spine,zOrder)
                  end)))
              end     
      end
end

function GameViewLayer:deletePlayerInfo(userItem)
     if self._scene.bPriGameEnd  and  GlobalUserItem.bPrivateRoom  then
            return
     end
     for k,v in pairs(self.m_tabUserItem) do
          if v and v.dwUserID == userItem.dwUserID then

              if self.m_tabUserHead[k] then
                    self.m_tabUserHead[k]:setVisible(false)
                   
              end
              self:onUserReady(k, false)

              v = nil
          end
     end
end

-- 用户更新
function GameViewLayer:OnUpdateUser(viewId, userItem, bLeave)
    print(" update user " .. viewId)
    if not viewId or viewId == yl.INVALID_CHAIR then
        print("OnUpdateUser viewId is nil")
        return
    end
    
    if bLeave then
        local roleItem = self.m_tabUserHead[viewId]
        if self._scene.bPriGameEnd ~= true and nil ~= roleItem then
            roleItem:removeFromParent()
            self.m_tabUserHead[viewId] = nil
        end
        self:onUserReady(viewId, false)
    end
    local bHide = ((table.nums(self.m_tabUserHead)) == (self._scene:getFrame():GetChairCount()))
    if not GlobalUserItem.bPrivateRoom then
        self.m_btnInvite:setVisible(not bHide)
        self.m_btnInvite:setEnabled(not bHide)
    end    
    self.m_btnInvite:setVisible(false)
    self.m_btnInvite:setEnabled(false)

    if nil == userItem then
        return
    end
    self.m_tabUserItem[viewId] = clone(userItem)

    print("lallalalalalalalalalalallalalal",userItem.szNickName, userItem.cbUserStatus)
    local bReady = userItem.cbUserStatus == yl.US_READY
    self:onUserReady(viewId, bReady)
    if GlobalUserItem.bVideo or (viewId ==  cmd.MY_VIEWID and userItem.cbUserStatus >= yl.US_READY) then 
        self.m_btnReady:setEnabled(false)
        self.m_btnReady:setVisible(false)
    end

    if self.m_tabUserHead[viewId] then
        self.m_tabUserHead[viewId]:removeFromParent()
        self.m_tabUserHead[viewId] = nil
    end
   
    local roleItem = GameRoleItem:create(clone(userItem), viewId, self)
    roleItem:setPosition(self.m_tabUserHeadPos[viewId])
    self.m_tabUserHead[viewId] = roleItem
    self.m_userinfoControl:addChild(roleItem)

    self.m_tabUserHead[viewId]:updateRole(clone(userItem))


 
    -- 桌主标识
    if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bVideo then
        local bRoomer = PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == userItem.dwUserID
        self.m_tabUserHead[viewId]:switeRoomerState(bRoomer)
    end
    -- 地主标识
    local bankerChair = self._scene.m_cbBankerChair
    if yl.INVALID_CHAIR ~= bankerChair then
        local bankerviewid = self._scene:SwitchViewChairID(bankerChair)
        self.m_tabUserHead[viewId].m_spLandFlag:setVisible(viewId == bankerviewid)
    end

end

function GameViewLayer:onUserReady(viewId, bReady)
    print("用户是否准备=========================")
    if bReady then
        print("用户已经")
        print(viewId)
    end
    if MatchRoom and GlobalUserItem.bMatch then 
        local readySp = self.m_tabReadySp[viewId]
        if nil ~= readySp then
            readySp:setVisible(false)
        end
        return
    end
    --用户准备
    if bReady then
        local readySp = self.m_tabReadySp[viewId]
        if nil ~= readySp then
            readySp:setVisible(true)
        end
    else
        local readySp = self.m_tabReadySp[viewId]
        if nil ~= readySp then
            readySp:setVisible(false)
        end
    end
end

function GameViewLayer:onGetCellScore( score )
    score = score or 0
    local str = ""
    if score < 0 then
        str = "." .. score
    else
        str = "" .. score        
    end 
    if string.len(str) > 11 then
        str = string.sub(str, 1, 11)
        str = str .. "///"
    end  
    self.m_atlasDiFeng:setString(str) 
end

function GameViewLayer:onGetGameFree()
    if MatchRoom and GlobalUserItem.bMatch then
        if nil ~= self._matchView then
            self._matchView:removeTipNode()
        end
        return
    end
    self:reSetGame( true )
    if false == self._scene:getFrame().bEnterAntiCheatRoom then
        if MatchRoom and GlobalUserItem.bMatch then
           return
        end
        self.m_btnReady:setEnabled(true)
        self.m_btnReady:setVisible(true)
    end    
end

function GameViewLayer:onGetGameCallScore()
    self:onGetGamePlay()
end

function GameViewLayer:onGetGamePlay()
    self:reSetGame()
    self:reSetForNewGame()
    self.m_promptIdx = 0
end

function GameViewLayer:onGameStart()
    self:reSetGame()
    self.m_nMaxCallScore = 0
    self.m_textGameCall:setString("0")
    for k,v in pairs(self.m_tabBankerCard) do
        v:setVisible(false)
        v:setCardValue(0)
    end
    self.m_spInfoTip:setSpriteFrame("land_blank.png")
    for k,v in pairs(self.m_tabStateSp) do
        v:stopAllActions()
        v:setSpriteFrame("land_blank.png")
    end

    for k,v in pairs(self.m_tabCardCount) do
        v:setString("")
        if k == cmd.MY_VIEWID then
            v:setString("0")
        end
    end

    self.m_csbNode:getChildByName("cardrecord_left"):setVisible(false)
    self.m_csbNode:getChildByName("cardrecord_right"):setVisible(false)

    self.m_promptIdx = 0
    -- 清理桌面
    self.m_outCardsControl:removeAllChildren()
end

-- 获取到扑克数据
-- @param[viewId] 界面viewid
-- @param[cards] 扑克数据
-- @param[bReEnter] 是否断线重连
-- @param[pCallBack] 回调函数
function GameViewLayer:onGetGameCard(viewId, cards, bReEnter, pCallBack)
    if bReEnter then
        self.m_tabNodeCards[viewId]:updateCardsNode(cards,viewId == cmd.MY_VIEWID, false) --(viewId == cmd.MY_VIEWID and not self._scene.m_bLookOn  ), false)
    else
        if nil ~= pCallBack then
            pCallBack:retain()
        end
        local this = self
        local call = cc.CallFunc:create(function()
            -- 非自己扑克
            local empTyCard = GameLogic:emptyCardList(cmd.NORMAL_COUNT)
            this.m_tabNodeCards[cmd.LEFT_VIEWID]:updateCardsNode(empTyCard, false, true)
            empTyCard = GameLogic:emptyCardList(cmd.NORMAL_COUNT)
            this.m_tabNodeCards[cmd.RIGHT_VIEWID]:updateCardsNode(empTyCard, false, true)

            -- 自己扑克
            if cmd.MY_VIEWID == viewId then
                this.m_tabNodeCards[cmd.MY_VIEWID]:updateCardsNode(cards, true--[[not self._scene.m_bLookOn]], true, pCallBack)
            end
            -- 庄家扑克
            -- 50 525
            -- 50 720
            for k,v in pairs(this.m_tabBankerCard) do
                v:setVisible(true)
            end
        end)
        local call2 = cc.CallFunc:create(function()
            -- 音效
            ExternalFun.playSoundEffect( "dispatch.wav" )
        end)
        local seq = cc.Sequence:create(call2, cc.DelayTime:create(0.3), call)
        self:stopAllActions()
        self:runAction(seq)
    end

    self.m_csbNode:getChildByName("cardrecord_left"):setVisible(true)
    self.m_csbNode:getChildByName("cardrecord_right"):setVisible(true)
end

-- 获取到玩家叫分
-- @param[callViewId]   当前叫分玩家
-- @param[lastViewId]   上个叫分玩家
-- @param[callScore]    当前叫分分数
-- @param[lastScore]    上个叫分分数
-- @param[bReEnter]     是否断线重连
function GameViewLayer:onGetCallScore( callViewId, lastViewId, callScore, lastScore, bReEnter )
    bReEnter = bReEnter or false
    local this = self

    if 255 == lastScore then
        -- 不叫
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game_tips_callscore0.png")
        if nil ~= frame then
            self.m_tabStateSp[lastViewId]:setSpriteFrame(frame)
        end    
    elseif lastScore > 0 and lastScore < 4 then
        local param = AnimationMgr.getAnimationParam()
        param.m_fDelay = 0.1
        param.m_strName = lastScore .. "_score_key"

        -- 播放叫分动画
        if bReEnter then
            local callscore = AnimationMgr.getAnimate(param)
            local frames = callscore:getAnimation():getFrames()
            if #frames > 0 then
                self.m_tabStateSp[lastViewId]:setSpriteFrame(frames[#frames]:getSpriteFrame())
            end
        else
            param.m_strName = Define.CALLSCORE_ANIMATION_KEY
            local animate = AnimationMgr.getAnimate(param)
            param.m_strName = lastScore .. "_score_key"
            local callscore = AnimationMgr.getAnimate(param)
            local call = cc.CallFunc:create(function()
                local frames = callscore:getAnimation():getFrames()
                if #frames > 0 then
                    this.m_tabStateSp[lastViewId]:setSpriteFrame(frames[#frames]:getSpriteFrame())
                end
            end)
            local seq = cc.Sequence:create(animate, callscore, call)
            self.m_tabStateSp[lastViewId]:stopAllActions()
            self.m_tabStateSp[lastViewId]:runAction(seq)
        end
    end

    lastScore = (lastScore > 3) and 0 or lastScore
    if lastScore > self.m_nMaxCallScore then
        self.m_nMaxCallScore = lastScore
    end
    if cmd.MY_VIEWID ~= lastViewId then
        local headitem = self.m_tabUserHead[lastViewId]
        if nil ~= headitem then
            -- 音效
            --ExternalFun.playSoundEffect( "cs" .. lastScore .. ".wav", headitem.m_userItem)
        end
    end

    self.m_bMyCallBanker = (cmd.MY_VIEWID == callViewId)
    if cmd.MY_VIEWID == callViewId and not self._scene.m_bRoundOver and not  self._scene.m_bLookOn  then
        -- 托管不叫
        if self.m_trusteeshipControl:isVisible() then
            self.m_callScoreControl:setVisible(false)
            self.m_callScoreControl:stopAllActions()
            self.m_callScoreControl:runAction(cc.Sequence:create(cc.DelayTime:create(Define.TRU_DELAY), cc.CallFunc:create(function()
                print("托管 延时叫分")
                this._scene:sendCallScore(255) 
            end)))
        else
            self.m_spInfoTip:setSpriteFrame("land_blank.png")
            -- 计算叫分
            local maxCall = self.m_nMaxCallScore + 1
            for i = 2, #self.m_tabCallScoreBtn do
                local btn = self.m_tabCallScoreBtn[i]
                btn:setEnabled(true)
                btn:setOpacity(255)
                if i <= maxCall  then
                    btn:setEnabled(false)
                    btn:setOpacity(125)
                end
            end
            self.m_callScoreControl:setVisible(true)
        end
    else
        if not bReEnter and not self._scene.m_bRoundOver then
            -- 等待叫分
            self.m_spInfoTip:setPosition(yl.WIDTH * 0.5, 274)
            self.m_spInfoTip:setSpriteFrame("game_tips_01.png")
        end        
    end
end

-- 获取到庄家信息
-- @param[bankerViewId]         庄家视图id
-- @param[cbBankerScore]        庄家分数
-- @param[bankerCards]          庄家牌
-- @param[bReEnter]             是否断线重连
function GameViewLayer:onGetBankerInfo(bankerViewId, cbBankerScore, bankerCards, bReEnter)
    bReEnter = bReEnter or false
    self.m_bMyCallBanker = false
    self.bankViewID = bankerViewId

    -- 更新庄家扑克
    if 3 == #bankerCards then
        for k,v in pairs(bankerCards) do
            self.m_tabBankerCard[k]:setVisible(true)
            self.m_tabBankerCard[k]:setCardValue(v)
        end
    end

    -- 庄家切换
    for k,v in pairs(self.m_tabUserHead) do
        -- v:switeGameState(k == bankerViewId)
        self.m_tabStateSp[k]:stopAllActions()
        self.m_tabStateSp[k]:setSpriteFrame("land_blank.png")

        if k == bankerViewId then
            if self.m_tabUserHeadAnimate[k][1] and self.m_tabUserHeadAnimate[k][2] then
                self.m_tabUserHeadAnimate[k][1]:setVisible(true)
                self.m_tabUserHeadAnimate[k][1]:setAnimation(0, "sikao", true)
                self.m_tabUserHeadAnimate[k][2]:setVisible(false)
            end
        else
            if self.m_tabUserHeadAnimate[k][1] and self.m_tabUserHeadAnimate[k][2] then
                self.m_tabUserHeadAnimate[k][1]:setVisible(false)
                self.m_tabUserHeadAnimate[k][2]:setVisible(true)
                self.m_tabUserHeadAnimate[k][2]:setAnimation(0, "sikao", true)
            end
        end
    end

    -- 庄家牌标识
    for k,v in pairs(self.m_tabNodeCards) do
        v:showLandFlag(bankerViewId)
    end

    if false == bReEnter then
        -- 庄家增加牌
        local handCards = self.m_tabNodeCards[bankerViewId]:getHandCards()
        local count = #handCards
        if bankerViewId == cmd.MY_VIEWID then
            handCards[count + 1] = bankerCards[1]
            handCards[count + 2] = bankerCards[2]
            handCards[count + 3] = bankerCards[3]
            handCards = GameLogic:SortCardList(handCards, cmd.MAX_COUNT, 0)
            if self._scene.m_bLookOn then
                self.m_tabNodeCards[cmd.MY_VIEWID]:showCards(false)
            end
        else
            handCards[count + 1] = 0
            handCards[count + 2] = 0
            handCards[count + 3] = 0
        end
        self.m_tabNodeCards[bankerViewId]:addCards(bankerCards, handCards)
    end

    -- 提示
    self.m_spInfoTip:setSpriteFrame("land_blank.png")
end

-- 倍数/叫分
function GameViewLayer:onGetGameTimes( nTimes )
    nTimes = nTimes or 0
    -- 叫分
    self.m_textGameCall:setString(nTimes .. "") 
end

-- 用户出牌
-- @param[curViewId]        当前出牌视图id
-- @param[lastViewId]       上局出牌视图id
-- @param[lastOutCards]     上局出牌
-- @param[bRenter]          是否断线重连
-- @param[bPass]            是否不出
function GameViewLayer:onGetOutCard(curViewId, lastViewId, lastOutCards, bReEnter, bPass)
    self.currentPlayer = curViewId
    bReEnter = bReEnter or false
    local this = self

    self.m_bMyOutCards = (curViewId == cmd.MY_VIEWID)
    if nil ~= self.m_tabStateSp[curViewId] then
        self.m_tabStateSp[curViewId]:setSpriteFrame("land_blank.png")
    end
    -- 自己出牌
    if curViewId == cmd.MY_VIEWID then
        self.m_spInfoTip:setSpriteFrame("land_blank.png")
        -- 托管
        
            -- 移除上轮出牌
            self.m_outCardsControl:removeChildByTag(curViewId)
            if not self._scene.m_bLookOn then
                self.m_onGameControl:setVisible(true)
            else
                self.m_onGameControl:setVisible(false)
            end

            -- 重置状态
            self:outControlButton( false, false, true)
            self.m_btnOutCard:setEnabled(false)
            self.m_btnOutCard:setOpacity(125)

            local promptList = self._scene.m_tabPromptList
            self.m_bCanOutCard = (#promptList > 0)

            -- 出牌控制
            if not self.m_bCanOutCard then
                self.m_spInfoTip:setSpriteFrame("game_tips_00.png")
                self.m_spInfoTip:setPosition(yl.WIDTH * 0.5, 120)

                self:outControlButton( false, true, false)
            else
                local bFirstOut = #self.m_tabNodeCards[cmd.MY_VIEWID]:getHandCards() == cmd.MAX_COUNT
                -- 是否首发出牌
                if bFirstOut then
                    self:outControlButton( true, false, false)
                end

                local sel = self.m_tabNodeCards[cmd.MY_VIEWID]:getSelectCards()
                local selCount = #sel
                local lastOutCount = #lastOutCards
                if selCount > 0 then
                    local selType = GameLogic:GetCardType(sel, selCount)
                    if GameLogic.CT_ERROR ~= selType then
                        if lastViewId == cmd.MY_VIEWID or lastOutCount == 0 then
                            self.m_btnOutCard:setEnabled(true)
                            self.m_btnOutCard:setOpacity(255)
                        elseif lastOutCount > 0 and GameLogic:CompareCard(lastOutCards, lastOutCount, sel, selCount) then
                            self.m_btnOutCard:setEnabled(true)
                            self.m_btnOutCard:setOpacity(255)
                        end
                    end
                end
                self.m_spInfoTip:setSpriteFrame("land_blank.png")

                local cardType = GameLogic:GetCardType(lastOutCards, lastOutCount)
                -- 上輪為火箭, 必須出牌
                if GameLogic.CT_MISSILE_CARD == cardType or (curViewId == lastViewId) then
                    self:onChangePassBtnState(false)
                end
            end
            
          if self.isAutoOutCard then
                self.m_callScoreControl:setVisible(false)
                self.m_onGameControl:setVisible(false)
                self.m_spTimer:setVisible(false)
          end
    else
        self.m_onGameControl:setVisible(false)
    end

    -- 出牌消息
    -- if lastViewId ~= cmd.MY_VIEWID and #lastOutCards > 0 and not bPass then
    --     local vec = self.m_tabNodeCards[lastViewId]:outCard(lastOutCards, bReEnter)
    --     self:outCardEffect(lastViewId, lastOutCards, vec)
    -- end

    if #lastOutCards > 0 and not bPass then
        local vec = self.m_tabNodeCards[lastViewId]:outCard(lastOutCards, bReEnter)
        self:outCardEffect(lastViewId, lastOutCards, vec)
    end

    --托管时自己出牌
    -- if self.isAutoOutCard then
    --     if  #lastOutCards > 0 and not bPass then
    --         local vec = self.m_tabNodeCards[lastViewId]:outCard(lastOutCards, bReEnter)
    --         print("用户出牌=================")
    --         dump(lastOutCards)
    --         self:outCardEffect(lastViewId, lastOutCards, vec)
    --     end
    -- end

    --出牌动画
    local function outCardPlayer(viewId)
        local cardType = GameLogic:GetCardType(lastOutCards, #lastOutCards)

        local temp = nil
        if self.bankViewID == viewId then
            temp = self.m_tabUserHeadAnimate[viewId][1]
            if cardType == GameLogic.CT_BOMB_CARD or cardType == GameLogic.CT_MISSILE_CARD then
                self.m_tabUserHeadAnimate[viewId][1]:setAnimation(0, "xiao", false)
            else
                self.m_tabUserHeadAnimate[viewId][1]:setAnimation(0, "chupai", false)
            end
            self:runAction(cc.Sequence:create (cc.DelayTime:create(2), 
                                            cc.CallFunc:create(function() 
                                            self.m_tabUserHeadAnimate[viewId][1]:setAnimation(0, "sikao", true)
                            end)))
        else
            temp = self.m_tabUserHeadAnimate[viewId][2]
            if cardType == GameLogic.CT_BOMB_CARD or cardType == GameLogic.CT_MISSILE_CARD then
                self.m_tabUserHeadAnimate[viewId][2]:setAnimation(0, "xiao", false)
            else
                self.m_tabUserHeadAnimate[viewId][2]:setAnimation(0, "chupai", false)
            end
            self:runAction(cc.Sequence:create(cc.DelayTime:create(2), 
                                            cc.CallFunc:create(function() 
                                            self.m_tabUserHeadAnimate[viewId][2]:setAnimation(0, "sikao", true)
                            end))) 
        end
        return tmp
    end

    local record = nil
    if not bPass then
        self:runAction(cc.Sequence:create(cc.CallFunc:create(function ()
                                                                if cmd.MY_VIEWID == lastViewId then
                                                                    --自己出牌
                                                                    record = outCardPlayer(cmd.MY_VIEWID)
                                                                elseif lastViewId == cmd.LEFT_VIEWID then
                                                                    --左边出牌
                                                                   record = outCardPlayer(cmd.LEFT_VIEWID)
                                                                elseif lastViewId == cmd.RIGHT_VIEWID then
                                                                    --右边出牌
                                                                    record = outCardPlayer(cmd.RIGHT_VIEWID)
                                                                end 
                                                          end), 
                                          cc.DelayTime:create(1.5), 
                                          cc.CallFunc:create(function ()
                                                            if record then
                                                                record:setAnimation(0, "sikao", true)
                                                            end
                                                          end)
                                          ))
    end

end

-- 用户pass
-- @param[passViewId]       放弃视图id
function GameViewLayer:onGetPassCard( passViewId )
    if passViewId ~= cmd.MY_VIEWID then
        local headitem = self.m_tabUserHead[passViewId]
        if nil ~= headitem then
            -- 音效
            ExternalFun.playSoundEffect( "pass" .. math.random(0, 1) .. ".wav", headitem.m_userItem)
        end        
    end
    self.m_outCardsControl:removeChildByTag(passViewId)

    -- 显示不出
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("game_nooutcard.png")
    if nil ~= frame then
        self.m_tabStateSp[passViewId]:setSpriteFrame(frame)
    end
end

-- 游戏结束
-- @param[rs] 结算数据
-- @param[bNoDelayResult] 非延迟结算动画
function GameViewLayer:onGetGameConclude( rs, bNoDelayResult )
    for k, v in pairs(self.m_tabUserHeadAnimate) do
        if v then
            for m, n in pairs (v) do
                if v then
                  n:setVisible(false)
               end
            end
        end
    end
    
    bNoDelayResult = bNoDelayResult or false
    self.m_spInfoTip:stopAllActions()
    if rs.cbFlag == cmd.kFlagChunTian then
        self:runAction(cc.Sequence:create(cc.CallFunc:create(function() self:springAnimate() end),
                                          cc.DelayTime:create(3), 
                                          cc.CallFunc:create(function() self:showResult(rs, bNoDelayResult) end)
                                          )) 
    elseif rs.cbFlag == cmd.kFlagFanChunTian then
        self:runAction(cc.Sequence:create(cc.CallFunc:create(function() self:reversSpringAnimate() end),
                                          cc.DelayTime:create(3),  
                                          cc.CallFunc:create(function() self:showResult(rs, bNoDelayResult) end)
                                          )) 
    else
        self:showResult(rs, bNoDelayResult)
    end
end

function GameViewLayer:showResult( rs, bNoDelayResult )
    self:cleanDesk()
    local this = self
    -- 界面重置
    self:reSetGame()
    self:stopAllActions()

    -- 隐藏剩余牌
    for k, v in pairs(self.m_tabCardCount) do
        if k == cmd.MY_VIEWID then
        else
            v:setString("")
        end
    end
    self.m_csbNode:getChildByName("cardrecord_left"):setVisible(false)
    self.m_csbNode:getChildByName("cardrecord_right"):setVisible(false)

    -- 取消托管
    self.m_trusteeshipControl:setVisible(false)

    self.m_spInfoTip:setSpriteFrame("land_blank.png")
    self.m_spInfoTip:setPosition(yl.WIDTH * 0.5, 375)

    -- 结算
    if nil == self.m_resultLayer then
        self.m_resultLayer = GameResultLayer:create(self)
        -- self.m_resultLayer:setPosition(cc.p(-display.cx*0.5, -display.cy*0.5))
        self:addChild(self.m_resultLayer, TAG_ZORDER.RESULT_ZORDER)
    end
    if not GlobalUserItem.bPrivateRoom and not bNoDelayResult then
        self:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function()
            this.m_resultLayer:showGameResult(rs)
            -- 显示准备
            if MatchRoom and GlobalUserItem.bMatch then
                
            else
                this.m_btnReady:setEnabled(true)
                this.m_btnReady:setVisible(true)
            end
        end)))        
    else
        self.m_resultLayer:showGameResult(rs)
        -- 显示准备
        if MatchRoom and GlobalUserItem.bMatch then
            
        else
            if not self._scene.m_bLookOn then 
                self.m_btnReady:setEnabled(true)
                self.m_btnReady:setVisible(true)
            end
        end
    end   
    if nil ~= self.m_chatLayer then
        self.m_chatLayer:showGameChat(false)
    end 
    
    self.m_rootLayer:removeChildByName("__effect_ani_name__")
end

------------------------------------------------------------------------------------------------------------
--更新
------------------------------------------------------------------------------------------------------------


return GameViewLayer