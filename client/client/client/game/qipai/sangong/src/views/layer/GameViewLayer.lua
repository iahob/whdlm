local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)

if not yl then
require("client.src.plaza.models.yl")
end
local GameChat = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer") --appdf.req(appdf.GAME_SRC.."qipai.sangong.src.views.layer.GameChatLayer")
local cmd = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.CMD_Game")
local SettingLayer = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.views.layer.SettingLayer")
local CuoCardLayer = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.views.layer.CuoCardLayer")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local CardSprite = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.views.layer.CardSprite")
local HelpLayer = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.views.layer.HelpLayer")
local TrustLayer = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.views.layer.TrustLayer")

local SpineObj = appdf.req("client.src.external.SpineObj")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

GameViewLayer.Enum = 
{
    BT_BG          = 1,
    BT_SWITCH      = 2,
    BT_ADDRESS     = 3,
    BT_VOICE       = 4,
    BT_CHAT        = 5,
    BT_TIPS        = 6,
    BT_READY       = 7,
    BT_CHANGEDESK  = 8,
    BT_SHOW        = 9,
    BT_CHECK       = 10,

    BT_GETBANKER   = 11,
    BT_NOTGETBANKER= 12,
    BT_GETBANKER1  = 13,
    BT_GETBANKER2  = 14,
    BT_GETBANKER3  = 15,
    BT_GETBANKER4  = 16,
    BT_GETBANKER5  = 17,

    BT_SETTING     = 21,
    BT_HELP        = 22,
    BT_EXIT        = 23,
    BT_TRUST       = 24,

    BT_TRUST_CANCEL= 25,

    --下注按钮
    BT_JETTON  = 58,

    --解散按钮
    BT_DISMISS  = 80,

    BT_CHECKBOX  = 90,

    --游戏开始
    BT_START  = 100,
    BT_SIT  = 101,
}

local TAG_ENUM = GameViewLayer.Enum

GameViewLayer.CHIPNUM 				= 100
GameViewLayer.ZORDER_SETTING        = 101

local ptUserInfo = {cc.p(680, 160), cc.p(680, 310),  cc.p(365, 125), cc.p(170, 300), cc.p(170, 160), cc.p(380, 300), cc.p(630, 300), cc.p(375, 300)}
local anchorPoint = {cc.p(1, 1), cc.p(1, 0), cc.p(0, 0), cc.p(0, 0), cc.p(0, 1), cc.p(0, 1), cc.p(0, 1), cc.p(1, 1)}

local ptPlayer = {cc.p(325, 600), cc.p(85, 415), cc.p(254, 93), cc.p(1245, 415), cc.p(1035, 600), cc.p(660, 645)}
local ptCoin = {cc.p(225, 525), cc.p(50, 300), cc.p(50, 70), cc.p(1210-10, 316+5), cc.p(1050, 525), cc.p(620, 570)}
local ptCard = {cc.p(390, 490), cc.p(240, 330), cc.p(670, 95), cc.p(1020, 330), cc.p(870, 490), cc.p(630, 500)}
local ptArrow = {cc.p(185, 620), cc.p(185, 380), cc.p(0, 0), cc.p(1149, 380), cc.p(1149, 620), cc.p(0, 620)}
local ptReady = {cc.p(353, 525), cc.p(218, 388), cc.p(670, 95), cc.p(1111, 388), cc.p(1014, 525), cc.p(667, 562)}

local ptCallBanker = {cc.p(353, 525), cc.p(218, 388), cc.p(670, 95), cc.p(1111, 388), cc.p(1014, 525), cc.p(667, 562)}
local ptWinLose = {cc.p(220, 655), cc.p(30, 510), cc.p(135, 155), cc.p(1192, 510), cc.p(930, 655), cc.p(585, 695)}
local ptCountCard = {cc.p(390+35, 490), cc.p(240+35, 330), cc.p(670, 95), cc.p(1020+35, 330), cc.p(870+35, 490), cc.p(630+35, 500)}

local ptLookCard = {cc.p(245, 541), cc.p(245, 301), cc.p(670, 284), cc.p(1089, 301), cc.p(1089, 541), cc.p(670, 284)}
local ptAddScore = {cc.p(245, 542), cc.p(245, 302), cc.p(670, 290), cc.p(1089, 302), cc.p(1089, 542), cc.p(670, 290)}
local ptGiveUpCard = {cc.p(245, 541), cc.p(245, 301), cc.p(670, 290), cc.p(1089, 301), cc.p(1089, 541), cc.p(670, 290)}
local ptChat = {cc.p(248, 629), cc.p(105, 418), cc.p(165, 148), cc.p(1042, 434), cc.p(951, 625), cc.p(619, 649)}

GameViewLayer.JETTON_MODE = 0   --托管下注模式

function GameViewLayer:OnResetView()
	self:stopAllActions()
end

function GameViewLayer:onClearTips()
    for i = 1, cmd.GAME_PLAYER do
        local flagReady = self.m_csbNode:getChildByName("sp_readyFlag" .. i)
        if flagReady then
            flagReady:setVisible(false)
        end
    end
end

function GameViewLayer:onExit()
	print("GameViewLayer onExit")

    self:onClearTips()

	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timeSchedule)

    AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)

    if self._beginTimeId then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
        self._beginTimeId = nil
    end
end

function GameViewLayer:getParentNode()
    return self._scene
end

function GameViewLayer:ctor(scene)
    --注册node事件
    ExternalFun.registerNodeEvent(self)
    self._scene = scene

    --初始化
    self:paramInit()

    --加载资源
    self:loadResource()

end

--初始化信息
function GameViewLayer:paramInit()
    self.CuoLayer = nil 
    self.actionGoldList = {} --金币
end


--清理桌面
function GameViewLayer:cleanDesk()
    for i=1, cmd.GAME_PLAYER  do
        --清理桌面扑克
        local nodeCard = self.nodeCard:getChildByName("nodeCard" .. i)
        local node = nodeCard:getChildByName("nodeCard")
        for j=1,3 do
            local sp = node:getChildByName("img" .. j)
            sp:removeAllChildren()
        end
        local node = nodeCard:getChildByName("nodeType")
        node:removeAllChildren()

        --清理下注
        local spGoldBg = self.nodeGold:getChildByName("sp_scoreBg" .. i)
        if spGoldBg then
            spGoldBg:setVisible(false)
            local txt = spGoldBg:getChildByName("txtScore")
            txt:setString("")
        end

        --清理庄家显示
        --查找玩家头像,控制庄家可见
        local userNode = self.m_csbNode:getChildByName("face" .. i)
        if userNode then
            local sp_banker = userNode:getChildByName("sp_banker")
            sp_banker:setVisible(false)

            local spBankerBox = userNode:getChildByName("sp_faceBoxBanker")
            spBankerBox:stopAllActions()
            spBankerBox:setVisible(false)

            --清理倍数显示
            local nodeRatio = userNode:getChildByName("nodeBankerRatio")
            nodeRatio:setVisible(false)
        end
    end

    --清理通比奖池
    local spGoldBg = self.nodeGold:getChildByName("sp_scoreBg")
    if spGoldBg then
        spGoldBg:setVisible(false)
        local txt = spGoldBg:getChildByName("txtScore")
        txt:setString("")
    end

    --按钮
    --搓牌
    local btn = self.m_csbNode:getChildByName("btn_check")
    btn:setVisible(false)

    --看牌
    local btn = self.m_csbNode:getChildByName("btn_show")
    btn:setVisible(false)

    --设置换桌按钮可用
    if not GlobalUserItem.bPrivateRoom then
        local btn = self.btnBg:getChildByName("btn_changeDesk")
        btn:setEnabled(true)

        self.btn_changeDesk:setEnabled(true)
    end

    --操作界面
    self.nodeGetBanker:setVisible(false)
    self.nodeBets:setVisible(false)

    self:hideGetBankerRaito()

    --清理定时器
    self:OnUpdataClockViewByStatus(0, 0)

    --初始化操作spine
    self:initUserOpAction()
 
    self:stopAllActions()

    --飞金币动画node 
    self.nodeGoldAction:removeAllChildren()
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

 --加载资源
function GameViewLayer:loadResource()

    --播放背景音乐
    ExternalFun.playBackgroudAudio("Bgm.mp3")

    cc.Director:getInstance():getTextureCache():addImage("game/card.png")
    -- 语音动画
    AnimationMgr.loadAnimationFromFrame("sp_voice%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

    -- 语音动画
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    param.m_strName = cmd.VOICE_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    self.m_actVoiceAni = cc.RepeatForever:create(animate)
    self.m_actVoiceAni:retain()

    --加载场景
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/GameScene.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode

    print("loadResource", csbNode)

    local this = self
    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.began then
            this:onButtonClickedEvent(sender:getTag(), sender)
        end
    end

    --背景
    local btn = csbNode:getChildByName("game_table")
    btn:setTouchEnabled(true)
    btn:setTag(TAG_ENUM.BT_BG)
    btn:addTouchEventListener(btnEvent)

    --菜单栏
    local btn = csbNode:getChildByName("btn_switch")
    btn:setTag(TAG_ENUM.BT_SWITCH)
    btn:addTouchEventListener(btnEvent)

    --定位
    local btn = csbNode:getChildByName("btn_address")
    btn:setTag(TAG_ENUM.BT_ADDRESS)
    btn:addTouchEventListener(btnEvent)

    --语音
    local btn = csbNode:getChildByName("btn_voice")
    btn:setTag(TAG_ENUM.BT_VOICE)
    btn:addTouchEventListener(btnEvent)

    --聊天
    local btn = csbNode:getChildByName("btn_chat")
    btn:setTag(TAG_ENUM.BT_CHAT)
    btn:addTouchEventListener(btnEvent)

    --牌型
    local btn = csbNode:getChildByName("btn_tips")
    btn:setTag(TAG_ENUM.BT_TIPS)
    btn:addTouchEventListener(btnEvent)

    --准备
    local btn = csbNode:getChildByName("btn_ready")
    btn:setTag(TAG_ENUM.BT_READY)
    btn:addTouchEventListener(btnEvent)
    self.btReady = btn

    --换桌
    local btn = csbNode:getChildByName("btn_changeDesk")
    btn:setTag(TAG_ENUM.BT_CHANGEDESK)
    btn:addTouchEventListener(btnEvent)
    self.btChangeDesk = btn

    --搓牌
    local btn = csbNode:getChildByName("btn_check")
    btn:setTag(TAG_ENUM.BT_CHECK)
    btn:addTouchEventListener(btnEvent)

    --看牌
    local btn = csbNode:getChildByName("btn_show")
    btn:setTag(TAG_ENUM.BT_SHOW)
    btn:addTouchEventListener(btnEvent)

    --坐下
    local btn = csbNode:getChildByName("btn_sit")
    btn:setTag(TAG_ENUM.BT_SIT)
    btn:addTouchEventListener(btnEvent)
    self.btSit = btn

    --来时
    local btn = csbNode:getChildByName("btn_start")
    btn:setTag(TAG_ENUM.BT_START)
    btn:addTouchEventListener(btnEvent)
    self.btStart = btn

    self.btnTrust = csbNode:getChildByName("btn_trust")
    self.btnTrust:setTag(TAG_ENUM.BT_TRUST)
    self.btnTrust:addTouchEventListener(btnEvent)

    --下拉菜单
    self.btnBg = csbNode:getChildByName("sp_btnBg")
    -- self.btnBg:setVisible(false)

    local btn = self.btnBg:getChildByName("btn_setting")
    btn:setTag(TAG_ENUM.BT_SETTING)
    btn:addTouchEventListener(btnEvent)

    local btn = self.btnBg:getChildByName("btn_changeDesk")
    btn:setTag(TAG_ENUM.BT_CHANGEDESK)
    btn:addTouchEventListener(btnEvent)
    self.btn_changeDesk = btn

    local btn = self.btnBg:getChildByName("btn_help")
    btn:setTag(TAG_ENUM.BT_HELP)
    btn:addTouchEventListener(btnEvent)

    local btn = self.btnBg:getChildByName("btn_exit")
    btn:setTag(TAG_ENUM.BT_EXIT)
    btn:addTouchEventListener(btnEvent)

    --托管
    self.nodeTrust = csbNode:getChildByName("nodeTrust")
    local btn = self.nodeTrust:getChildByName("btn_trustN")
    btn:setTag(TAG_ENUM.BT_TRUST_CANCEL)
    btn:addTouchEventListener(btnEvent)
    

    --定时器和操作提示
    self.sp_alarm = csbNode:getChildByName("sp_alarm")
    self.sp_alarm:setVisible(false)

    --游戏信息（私有房）
    self.sp_gameInfo = csbNode:getChildByName("sp_gameInfo")
    self.sp_gameInfo:setVisible(false)

    --下注界面
    self.nodeGold = csbNode:getChildByName("nodeGold")
    -- self.nodeGold:setVisible(false)
    
    --抢庄界面
    self.nodeGetBanker = csbNode:getChildByName("nodeGetBanker")
    self.nodeGetBanker:setVisible(false)

    --下注界面
    self.nodeBets = csbNode:getChildByName("nodeBets")
    self.nodeBets:setVisible(false)

     --扑克界面
    self.nodeCard = csbNode:getChildByName("nodeCard")

    --飞金币node
    self.nodeGoldAction = cc.Node:create()
    csbNode:addChild(self.nodeGoldAction, 10)
    self.nodeGoldAction:setPosition(cc.p(0, 0))

    --牌型提示
    self.spTips = csbNode:getChildByName("sp_tipsbg")
    local scroView = self.spTips:getChildByName("ScrollView")
    local spTips = cc.Sprite:create("game/sp_tipsInfo.png")
    scroView:addChild(spTips)
    spTips:setAnchorPoint(cc.p(0, 0))
    spTips:setPosition(cc.p(20, 0))

    --聊天窗口层
    local tabconfig = {} 
    tabconfig.csbfile = cmd.RES.."chat/GameChatLayer.csb"
    tabconfig.color = cc.c3b(42,103,138)
    self.m_GameChat = GameChat:create(self._scene._gameFrame ,tabconfig)
        :setLocalZOrder(50)
        :addTo(self)

    --初始化操作spine
    self:initUserOpAction()

    --设置层
    self._setLayer = SettingLayer:create(self)
        :addTo(self, 10)
    self._setLayer:hideLayer()

    --玩法
    self._helpLayer = HelpLayer:create(self)
        :addTo(self, 10)
    self._helpLayer:hideLayer()


    self._trustLayer = TrustLayer:create(self._scene)
        :addTo(self, 10)
    self._trustLayer:hideLayer()

    --电池电量wifi 时间
    local nodeMBInfo = csbNode:getChildByName("nodeMBInfo")
    self.batteryBar = nodeMBInfo:getChildByName("batteryLoading")
    self.batteryBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())
    self.txt_battery = nodeMBInfo:getChildByName("txt_battary")
    self.txt_battery:setString(MultiPlatform:getInstance():getBatteryLevel() .. "%")

    self.sp_wifi = nodeMBInfo:getChildByName("spWifi")
    self.txt_time = nodeMBInfo:getChildByName("txt_time")

    self.timeSchedule = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
        self.batteryBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())

        self.txt_time:setString(os.date("%H:%M"))

        local netType = MultiPlatform:getInstance():getNetworkType()
        local Strength = MultiPlatform:getInstance():getNetworkStrength()
        local TypePic = {"4G","sp_wifi"}
        if netType and Strength and netType>=1 and netType<=2 and Strength>=1 and Strength<=3 then
            self.sp_wifi:loadTexture(cmd.RES.."game/"..TypePic[netType]..Strength..".png")
        end
    end, 1, false)
end

--初始化玩家操作动画spine 
function GameViewLayer:initUserOpAction()  
    for i=1, cmd.GAME_PLAYER do
        local viewID = self._scene:SwitchViewChairID(i -1)

        --查找玩家头像
        local userNode = self.m_csbNode:getChildByName("face" .. viewID)
        if userNode then
            local spine = userNode:getChildByName("spineOP")
            if spine then
                spine:setVisible(false)
            else
                --创建动画，隐藏
                spine = SpineObj:create(cmd.RES.."game/action/op.json", cmd.RES.."game/action/op.atlas", true)
                userNode:addChild(spine)
                spine:setName("spineOP")
                spine:setVisible(false)
            end

        end  
    end
end

function GameViewLayer:onButtonClickedEvent(tag, ref)   
    -- ExternalFun.playClickEffect()
    self._scene:PlaySound(cmd.RES .. "sound_res/anjiansheng.mp3")
    if TAG_ENUM.BT_BG == tag then         -- 开关1
        --关闭弹窗
        self.spTips:runAction(cc.MoveTo:create(0.2, cc.p(-150, 265)))
        self.btnBg:setVisible(false)
        self.btnTrust:setVisible(true)
        if not GlobalUserItem.bPrivateGoldRoom then
            self.btnTrust:setVisible(false)
        end
    elseif TAG_ENUM.BT_SWITCH == tag then
        
        if self.btnBg:isVisible() then
            self.btnBg:setVisible(false)
            self.btnTrust:setVisible(true)
        else
            self.btnBg:setVisible(true)
            self.btnTrust:setVisible(false)
        end

        if not GlobalUserItem.bPrivateGoldRoom then
            self.btnTrust:setVisible(false)
        end
    elseif TAG_ENUM.BT_READY == tag then
        self._scene:sendReady()
        --清理
        self:cleanDesk()
        --隐藏倒计时
        self:OnUpdataClockViewByStatus(0, 0)

        --控制准备和换桌不可见
        local btn = self.m_csbNode:getChildByName("btn_ready")
        btn:setVisible(false)
        --换桌
        local btn = self.m_csbNode:getChildByName("btn_changeDesk")
        btn:setVisible(false)

        if not GlobalUserItem.bPrivateGoldRoom then
            self.btnTrust:setVisible(false)
        end
    elseif TAG_ENUM.BT_TIPS == tag then
        self.spTips:runAction(cc.MoveTo:create(0.2, cc.p(220, 265)))
    elseif TAG_ENUM.BT_CHAT == tag then
        self.m_GameChat:showGameChat(true)
    elseif TAG_ENUM.BT_SETTING  == tag then
        self._setLayer:showLayer()
        self.btnBg:setVisible(false)
    elseif TAG_ENUM.BT_HELP  == tag then
       self._helpLayer:showLayer()
       self.btnBg:setVisible(false)
    elseif TAG_ENUM.BT_EXIT  == tag then
        -- self._scene:onExitTable()
        self.btnBg:setVisible(false)
        self._scene:onQueryExitGame()
     elseif TAG_ENUM.BT_TRUST_CANCEL == tag then
        self._scene.trust = false
        self:ShowTrust(false)
    elseif tag == TAG_ENUM.BT_CHANGEDESK then
        self:onChangeDesk ()
        self.btnBg:setVisible(false)
        self.btnTrust:setVisible(true)
        self.btn_changeDesk:setEnabled(false)
    elseif TAG_ENUM.BT_TRUST  == tag then
        --托管
        local Roomparam = {}
        Roomparam.cbRobBankerType = self._scene.cbRobBankerType
        Roomparam.cbRobBankerRatio = self._scene.cbRobBankerRatio[1]
        self._trustLayer:initLayer(Roomparam)

        self._trustLayer:showLayer()
    elseif TAG_ENUM.BT_CHECK == tag then
        self._scene:onCuoCard()
        --搓牌
        local btn = self.m_csbNode:getChildByName("btn_check")
        btn:setVisible(false)

        --看牌
        local btn = self.m_csbNode:getChildByName("btn_show")
        btn:setVisible(false)
    elseif TAG_ENUM.BT_SHOW == tag then
        self._scene:PlaySound(cmd.RES .. "sound_res/cuopai.mp3")
        self._scene:onOpenCard()
    elseif TAG_ENUM.BT_GETBANKER == tag 
    or TAG_ENUM.BT_NOTGETBANKER == tag 
    or TAG_ENUM.BT_GETBANKER1 == tag 
    or TAG_ENUM.BT_GETBANKER2 == tag 
    or TAG_ENUM.BT_GETBANKER3 == tag 
    or TAG_ENUM.BT_GETBANKER4 == tag 
    or TAG_ENUM.BT_GETBANKER5 == tag then
        print("发送抢庄倍数！---------》", ref.ratio)
        self._scene:onGetBanker(ref.ratio)

    elseif tag == TAG_ENUM.BT_SIT then   --坐下
        self.btSit:setVisible(false)

        self._scene._gameFrame.bChangeDesk = true
        self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)

    elseif tag == TAG_ENUM.BT_START then   --房主开始游戏 
        self.btStart:setVisible(false)
        self._scene._gameFrame:SendUserBegin()

        if self._beginTimeId then
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
            self._beginTimeId = nil
        end 
    else
        if TAG_ENUM.BT_JETTON <= tag and TAG_ENUM.BT_JETTON + cmd.EM_JETTON_MAX  > tag then
            self._scene:onAddScore(ref.score)
        end
    end
end

--换桌
function GameViewLayer:onChangeDesk()
    if GlobalUserItem.bPrivateRoom then --and not GlobalUserItem.bPrivateGoldRoom then
        return
    end
    --控制准备和换桌不可见
    local btn = self.m_csbNode:getChildByName("btn_ready")
    btn:setVisible(false)
    --换桌
    local btn = self.m_csbNode:getChildByName("btn_changeDesk")
    btn:setVisible(false)

    self._scene:requestChangeDesk()
end


--更新时钟
function GameViewLayer:OnUpdataClockViewByStatus(status,time)
    local numAlarm = self.sp_alarm:getChildByName("numAlarm")
    if time and time > 0 then
        --提示语
        local tipsSpPath = ""
        if status == cmd.EM_STATUS_FREE then
            tipsSpPath = "game/sp_readyTips.png"
            self.sp_alarm:setPositionX(0)
        elseif status == cmd.EM_STATUS_ROBBANKER then
            if self._scene.isRobBanker then
                tipsSpPath = "game/sp_getBanker1.png"
                self.sp_alarm:setPositionX(-75)
            else
                tipsSpPath = "game/sp_getBanker.png"
                self.sp_alarm:setPositionX(0)
            end
        elseif status == cmd.EM_STATUS_JETTON then
            if self._scene.wBankerUser == self._scene:GetMeChairID() then
                tipsSpPath = "game/sp_flagJetton.png"
            else
                tipsSpPath = "game/sp_scoreChioce.png"
            end
            self.sp_alarm:setPositionX(-75)
        elseif status == cmd.EM_STATUS_OPEN_CARD then
            tipsSpPath = "game/sp_checkCard.png"
            self.sp_alarm:setPositionX(0) 
        elseif status == cmd.EM_STATUS_GAME_END then
            tipsSpPath = "game/sp_flagEnd.png"
            self.sp_alarm:setPositionX(0)      
        end

        print("OnUpdataClockViewByStatus--------------->", status, tipsSpPath)
        --设置提示语
        local spTips = self.sp_alarm:getChildByName("sp_opTips")
        if tipsSpPath == "" then
            self.sp_alarm:setPositionX(92)
            spTips:setVisible(false)
        else
            spTips:setVisible(true)
            local sp = cc.Sprite:create(tipsSpPath)
            if sp then
                spTips:setSpriteFrame(sp:getSpriteFrame())
            end
        end

        numAlarm:setString(time)

        self.sp_alarm:setVisible(true)
    else
        self.sp_alarm:setVisible(false)
    end
end

--更新用户显示
function GameViewLayer:OnUpdateUser(viewid,userItem)
    if not viewid or viewid == yl.INVALID_CHAIR then
        print("OnUpdateUser viewid is nil")
        return
    end
    
    if self._scene.isPriOver  and  GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
        return
    end

    
    --查找玩家头像
    local userNode = self.m_csbNode:getChildByName("face" .. viewid)
    if not userNode then
        print("OnUpdateUser viewid is error---->", viewid)
        return
    end

    if not userItem then
        userNode:setVisible(false)
        local flagReady = self.m_csbNode:getChildByName("sp_readyFlag" .. viewid)
        if flagReady then
            flagReady:setVisible(false)
        end

        if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
            -- self._scene:checkReady()
        end
    else
        --设置头像可见
        userNode:setVisible(true)

        --设置玩家信息
        local nodeInfo = userNode:getChildByName("nodeInfo")
        --设置位置
        if viewid == 3 or viewid == 6 or viewid == 7 or viewid == 8 then            
            nodeInfo:setPosition(cc.p(100, 0))
        else
            nodeInfo:setPosition(cc.p(0, -75))
        end


        local txtNick = nodeInfo:getChildByName("nick")
        txtNick:setString(string.EllipsisByConfig(userItem.szNickName, 110, string.getConfig("fonts/FZHTJW.TTF" , 20)))

        --分数
        local txtScore = nodeInfo:getChildByName("score")
        local score = 0
        if userItem.lScore >= 10000 then
            score =ExternalFun.keepTwoDecimalPlaces(userItem.lScore/10000) .."万"
        else
            score = ExternalFun.keepTwoDecimalPlaces(userItem.lScore)
        end
        txtScore:setString(string.EllipsisByConfig(score, 110, string.getConfig("fonts/FZHTJW.TTF" , 20)))

        --头像
        local spNodeNull = userNode:getChildByName("sp_faceNull")

        local head = PopupInfoHead:createNormal(userItem, 77)
        local param = {}
        param.isProp = true     --显示道具互动，百人游戏不显示
        -- param.imgPath = "game/popBg.png" --弹出框背景
        head:setPropInfo(param)
        head:setPopCsbFile("game/GamePopInfo.csb") -- 自定义CSB
        head:enableInfoPop(true, ptUserInfo[viewid], anchorPoint[viewid])
        spNodeNull:addChild(head)
        head:setPosition(cc.p(38.5, 38.5))



        --判断积分房
        if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
            if PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
                if  PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
                    txtScore:setString(string.EllipsisByConfig(string.formatNumberThousands(userItem.lScore -PriRoom:getInstance().m_tabPriData.lIniScore,true),viewid == cmd.MY_VIEWID and 120 or 85,viewid == cmd.MY_VIEWID and self.MytxtConfig or self.txtConfig))
                end
            else
                txtScore:setString("0")
            end
        end

        --准备显示
        local flagReady = self.m_csbNode:getChildByName("sp_readyFlag" .. viewid)
        if flagReady then
            flagReady:setVisible(yl.US_READY == userItem.cbUserStatus)
        end

        if viewid == cmd.MY_VIEWID and yl.US_READY == userItem.cbUserStatus then
            --如果我自己变为准备状态，不显示按钮和倒计时（大联盟会自动准备）
            --清理
            self:cleanDesk()

            --隐藏倒计时
            self:OnUpdataClockViewByStatus(0, 0)
            self._scene:KillGameClock()

            --控制准备和换桌不可见
            local btn = self.m_csbNode:getChildByName("btn_ready")
            btn:setVisible(false)
            --换桌
            local btn = self.m_csbNode:getChildByName("btn_changeDesk")
            btn:setVisible(false)
        end

        if userItem.cbUserStatus == yl.US_OFFLINE then
            if nil ~= convertToGraySprite then
                -- 灰度图
                if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
                   convertToGraySprite(head.m_head.m_spRender)
                end
            end
        else
            if nil ~= convertToGraySprite then
              -- 正常图
              if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
                convertToNormalSprite(head.m_head.m_spRender)
              end
            end
        end

        --判断房主
        --print("设置房主信息", PriRoom, userItem.dwUserID, PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID)
        if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
            -- self._scene:checkReady()
        end
    end
end

--通比模式切换背景
function GameViewLayer:showGameTypeTB()
    --通比没有翻倍
    local txt_rule =  self.m_csbNode:getChildByName("txt_rule")
    txt_rule:setVisible(false)

    -- local game_table =  self.m_csbNode:getChildByName("game_table")
    -- game_table:loadTexture(cmd.RES.."game/game_table1.jpg")
end

--显示游戏玩法
function GameViewLayer:showGameTypeTimesRule(cardTypeTimesRule, cardTypeTime, cellScore)
    local txt_rule =  self.m_csbNode:getChildByName("txt_rule")
    if cardTypeTimesRule == cmd.EM_CT_TIMES_NULL then
        txt_rule:setString("无翻倍 底分：" .. cellScore)
    elseif cardTypeTimesRule == cmd.EM_CT_TIMES_TY then
        --不处理，默认值
        txt_rule:setString("爆玖*5  炸弹*4  三公*3  九点~七点*2  六点~零点*1 底分：" .. cellScore)
    elseif cardTypeTimesRule == cmd.EM_CT_TIMES_PZ then
        --配置玩法只显示前3个
        txt_rule:setString("爆玖*".. cardTypeTime[cmd.CT_MAX] .. "炸弹*" .. cardTypeTime[cmd.CT_MAX -1] .. "三公*" .. cardTypeTime[cmd.CT_MAX -2] .. "底分：" .. cellScore)
    end
end

--游戏动画
function GameViewLayer:showGamBeginOrCopareBegin(actionType)
    local spine = sp.SkeletonAnimation:create(cmd.RES.."game/action/ksyx.json", cmd.RES.."game/action/ksyx.atlas")
    self.m_csbNode:addChild(spine)
    spine:setPosition(cc.p(667, 450))

    if 1 == actionType then
        spine:setAnimation(0, "animation", false)
    elseif 2 == actionType then
        spine:setAnimation(0, "animation2", false)
    end

    local this = self
    spine:registerSpineEventHandler(function (event)
        spine:runAction(cc.Sequence:create(cc.DelayTime:create(0.01), cc.CallFunc:create(function()
            spine:removeFromParent()
        end)))
    end, 2)  
end

--通杀通赔动画
function GameViewLayer:showGamResultAction(actionType)
    local spine = sp.SkeletonAnimation:create(cmd.RES.."game/action/tongsha.json", cmd.RES.."game/action/tongsha.atlas")
    self.m_csbNode:addChild(spine)
    spine:setPosition(cc.p(667, 450))

    if 1 == actionType then
        spine:setAnimation(0, "animation", false)
    elseif 2 == actionType then
        spine:setAnimation(0, "animation2", false)
    end

    local this = self
    spine:registerSpineEventHandler(function (event)
        spine:runAction(cc.Sequence:create(cc.DelayTime:create(0.5), cc.CallFunc:create(function()
            spine:removeFromParent()
        end)))
    end, 2)  
end

--倒计时动画
function GameViewLayer:showGamCountdownWarning()
    local spine = sp.SkeletonAnimation:create(cmd.RES.."game/action/nz1.json", cmd.RES.."game/action/nz1.atlas")
    spine:setAnimation(0, "animation", false)
    spine:setPosition(cc.p(575, 280))

    local spAlarm = self.sp_alarm:getChildByName("sp_alarm")
    spAlarm:setVisible(false)

    local numAlarm = self.sp_alarm:getChildByName("numAlarm")
    numAlarm:setVisible(false)

    self.sp_alarm:addChild(spine)

    local this = self
    spine:runAction(cc.Sequence:create(cc.DelayTime:create(3), cc.CallFunc:create(function()
        spine:removeFromParent()
        spAlarm:setVisible(true)
        numAlarm:setString(0)
        numAlarm:setVisible(true)
    end)))
end



--显示玩家等待其他玩家游戏的状态
function GameViewLayer:showuserWaiting(isShow)
    local btn = self.m_csbNode:getChildByName("btn_changeDesk")
    local spFlag = self.m_csbNode:getChildByName("sp_waitFlag")

    if isShow then
        --换桌
        btn:setVisible(true)
        btn:setPositionX(667)

        spFlag:setVisible(true)
    else
        --换桌
        btn:setPositionX(802)
        spFlag:setVisible(false)
    end

    if GlobalUserItem.bPrivateRoom then
        self.btChangeDesk:setVisible(false);
        self.btn_changeDesk:setEnabled(false);
    end
end

--开始按钮
function GameViewLayer:showSitBtn(visible,time)
    if not self._scene.m_bLookOn then
        if visible then
            if not self._beginTimeId and time then
                 if self._scene.trust then
                    self.btStart:setVisible(false)
                    self._scene._gameFrame:SendUserBegin()

                    return
                 end

                self.m_beginTimeNum = time
                self.btStart:getChildByName("txtTime"):setString(""..self.m_beginTimeNum)
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
        self.btStart:getChildByName("txtTime"):setString(""..self.m_beginTimeNum)    
    else
        -- self.btStart:setVisible(false);
        if self._beginTimeId then
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
            self._beginTimeId = nil
        end
    end
end


--显示游戏状态，以及该状态下按钮
function GameViewLayer:showGamStatusAndButton(status)
    print("showGamStatusAndButton------------->", status)
    if status == cmd.EM_STATUS_FREE then
        --空闲状态
        local btnReady = self.m_csbNode:getChildByName("btn_ready")
        btnReady:setVisible(true)

        --换桌
        local btnChange = self.m_csbNode:getChildByName("btn_changeDesk")
        btnChange:setVisible(true)
        btnChange:setPositionX(802)

        --清理
        self:cleanDesk()
        self:showuserWaiting(false)

        if GlobalUserItem.bPrivateRoom then
            btnReady:setPositionX(667)
            btnChange:setVisible(false)
        
            self.btn_changeDesk:setEnabled(false);
        end

        if self._scene.m_bLookOn then
            btnReady:setVisible(false)
            btnChange:setVisible(false)
            self.btn_changeDesk:setEnabled(false);
        end

        if self._scene.trust then
            self._scene:sendReady()
    
            --隐藏倒计时
            -- self:OnUpdataClockViewByStatus(0, 0)

            --控制准备和换桌不可见
            local btn = self.m_csbNode:getChildByName("btn_ready")
            btn:setVisible(false)
            --换桌
            local btn = self.m_csbNode:getChildByName("btn_changeDesk")
            btn:setVisible(false)
        end
    elseif status == cmd.EM_STATUS_ROBBANKER then

        local delayTime = 0
        if self._scene.cbRobBankerType <= cmd.EM_OP_BANKER_ZYQZ then
            delayTime = 3.5
        end

        if self._scene.cbRobBankerType == cmd.EM_OP_BANKER_MPQZ then
            delayTime = 4.5
        end 

        local this = self
        self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime), cc.CallFunc:create(function ()
            if 1 == this._scene.m_cbPlayStatus[this._scene:GetMeChairID() + 1]  then
                this._scene:SetGameClock(this._scene:GetMeChairID(), cmd.EM_STATUS_ROBBANKER, this._scene.cbOperateBankerTime)
            end
            --旁观不显示叫庄
            if not self._scene.m_bLookOn then
                --显示叫庄
                this.nodeGetBanker:setVisible(true)
            end
        end), cc.DelayTime:create(2.0), cc.CallFunc:create(function ()
            if this._scene.trust and 1 == self._scene.m_cbPlayStatus[self._scene:GetMeChairID() + 1] then
                if 1 == this._scene.trustConfig.BankerType then
                    this._scene:onGetBanker(0)
                else
                    this._scene:onGetBanker(this._scene.trustConfig.BankerRatio)
                end
            end
        end)))

    elseif status == cmd.EM_STATUS_JETTON then
        --不是庄家的玩家显示下注中
        -- for i=1, cmd.GAME_PLAYER  do
        --     local viewID = self._scene:SwitchViewChairID(i - 1)
        --     local userNode = self.m_csbNode:getChildByName("face" .. viewID)
        --     if userNode:isVisible() then
        --         local spOp = userNode:getChildByName("spineOP")
        --         if (i -1) ~= self._scene.wBankerUser and viewID ~= cmd.MY_VIEWID and 1 == self._scene.m_cbPlayStatus[i] then
        --             spOp:setVisible(true)
        --             spOp:setAnimation(0, "animation2", true) 
        --         end
        --     end
        -- end
    elseif status == cmd.EM_STATUS_OPEN_CARD then

        --其他玩家显示搓牌中
        for i=1, cmd.GAME_PLAYER  do
            local userNode = self.m_csbNode:getChildByName("face" .. i)
            if userNode:isVisible() then
                local spOp = userNode:getChildByName("spineOP")
                if i ~= cmd.MY_VIEWID and 1 == self._scene.m_cbPlayStatus[i] then
                    spOp:setVisible(true)
                    spOp:setAnimation(0, "animation", true) 
                end
            end
        end

        if self._scene.trust and 1 == self._scene.m_cbPlayStatus[self._scene:GetMeChairID() + 1] then


            local delayTime = 3
            --明牌抢庄延时发牌
            if self._scene.cbRobBankerType == cmd.EM_OP_BANKER_MPQZ then
                delayTime = 5.5
            end

            self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime), cc.CallFunc:create(function ()
                --显示叫庄
                self._scene:PlaySound(cmd.RES .. "sound_res/cuopai.mp3")
                self._scene:onOpenCard()
            end)))
        else
            self:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function ()
                if not self._scene.m_bLookOn then
                  --显示搓牌和开牌按钮
                    local btn = self.m_csbNode:getChildByName("btn_check")
                    btn:setVisible(true)

                    --看牌
                    local btn = self.m_csbNode:getChildByName("btn_show")
                     btn:setVisible(true)
                end
            end)))
        end

    elseif status == cmd.EM_STATUS_GAME_END then
        --比牌界面
        --显示比牌动画
        self:showGamBeginOrCopareBegin(2)
        self._scene:PlaySound(cmd.RES .. "sound_res/bipai.mp3")

       --显示搓牌和开牌按钮
        --搓牌
        local btn = self.m_csbNode:getChildByName("btn_check")
        btn:setVisible(false)

        --看牌
        local btn = self.m_csbNode:getChildByName("btn_show")
        btn:setVisible(false)
    end

end

--初始化抢庄界面
function GameViewLayer:initNodeGetBanker(robBankerType, robBankerRatio)
    local this = self
    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.began then
            this:onButtonClickedEvent(sender:getTag(), sender)
        end
    end

    --自由抢庄
    local nodeZY = self.nodeGetBanker:getChildByName("nodeZY")
    --绑定按钮事件
    local btn = nodeZY:getChildByName("btn_N")
    btn:setTag(TAG_ENUM.BT_NOTGETBANKER)
    btn:addTouchEventListener(btnEvent)
    btn.ratio = 0

    local btn = nodeZY:getChildByName("btn_Y")
    btn:setTag(TAG_ENUM.BT_GETBANKER)
    btn:addTouchEventListener(btnEvent)
    btn.ratio = 1

    --名牌抢庄
    local nodeMP = self.nodeGetBanker:getChildByName("nodeMP")
    --绑定按钮事件
    local btn = nodeMP:getChildByName("btn_N")
    btn:setTag(TAG_ENUM.BT_GETBANKER1)
    btn:addTouchEventListener(btnEvent)
    btn.ratio = 0

    local btnNum = 1 --统计按钮数目，做偏移用 1 为不抢
    for i = 1, 4 do
        local btn = nodeMP:getChildByName("btn_" .. i)
        btn:setTag(TAG_ENUM.BT_GETBANKER1 + i)
        btn:addTouchEventListener(btnEvent)
        btn.ratio = robBankerRatio[i]

        if robBankerRatio[i] > 0 then
            btnNum = btnNum + 1
        else
            btn:setVisible(false)
        end

        --倍数 
        local txtRaito = btn:getChildByName("txt_times")
        txtRaito:setString(robBankerRatio[i].. "")
    end

    --计算偏移btnNum
    nodeMP:setPositionX(95 * (5 - btnNum))


    if robBankerType == cmd.EM_OP_BANKER_MPQZ then
        --名牌抢庄
        nodeMP:setVisible(true)
        nodeZY:setVisible(false)
    elseif robBankerType == cmd.EM_OP_BANKER_ZYQZ then
        nodeZY:setVisible(true)
        nodeMP:setVisible(false)
    end
end


--显示玩家抢庄倍数
function GameViewLayer:showGetBankerRaito(chairID, robBankerRatio)
    --转换视角
    local viewID = self._scene:SwitchViewChairID(chairID)

    local nodeNoitfy = self.m_csbNode:getChildByName("nodeNoitfy")

    local nodeRaito = nodeNoitfy:getChildByName("nodeRaito" .. viewID)
    nodeRaito:setVisible(true)

    local nodeY = nodeRaito:getChildByName("nodeRaito")
    local sp_bankerN = nodeRaito:getChildByName("sp_bankerN")
    if robBankerRatio > 0 then

        if self._scene.cbRobBankerType == cmd.EM_OP_BANKER_ZYQZ then
            nodeY:setVisible(false)
            sp_bankerN:setVisible(true)
            sp_bankerN:loadTexture("game/sp_bankerY.png")
        else
            nodeY:setVisible(true)
            sp_bankerN:setVisible(false)
        end

        --设置倍数
        local txtRaito = nodeY:getChildByName("txtRaito")
        if 0 == robBankerRatio then
            txtRaito:setString("1")
        else
            txtRaito:setString(robBankerRatio .. "")
        end

        local spIcon = nodeY:getChildByName("sp_icon")
        if robBankerRatio < 10 then
            spIcon:setPositionX(62)
        elseif robBankerRatio < 100 then
            spIcon:setPositionX(90)
        else
            spIcon:setPositionX(118)
        end
    else
        nodeY:setVisible(false)
        sp_bankerN:setVisible(true)
        sp_bankerN:loadTexture("game/sp_bankerN.png")
    end

    --设置抢庄倍数的的值~当选中庄家后显示
    local userNode = self.m_csbNode:getChildByName("face" .. viewID)
    if not userNode then
        return 
    end 
   --倍数显示
    local nodeRatio = userNode:getChildByName("nodeBankerRatio")
    local txt = nodeRatio:getChildByName("txt")
    if 0 == robBankerRatio then
        txt:setString("1")
    else
        txt:setString(robBankerRatio .. "")
    end

    local spTimes = nodeRatio:getChildByName("sp_times")
    if robBankerRatio > 10 then
        spTimes:setPositionX(28)
    else
        spTimes:setPositionX(12)
    end
end

--清理玩家抢庄倍数
function GameViewLayer:hideGetBankerRaito()

    for i=1, cmd.GAME_PLAYER do
            --转换视角
        local viewID = self._scene:SwitchViewChairID(i -1)

        local nodeNoitfy = self.m_csbNode:getChildByName("nodeNoitfy")

        local nodeRaito = nodeNoitfy:getChildByName("nodeRaito" .. viewID)
        nodeRaito:setVisible(false)
    end
end

function GameViewLayer:showCallBanker(callBankerUser, notCallBankerUser)
    local showUser = {}
    if 0 == #callBankerUser then
        showUser = notCallBankerUser
    else
        showUser = callBankerUser
    end 
    dump(showUser, "showCallBanker------->", 5)

    for i=1, #showUser do
        --转换视角
        local chairID = showUser[i]
        local viewID = self._scene:SwitchViewChairID(chairID)

        local userNode = self.m_csbNode:getChildByName("face" .. viewID)
        if userNode:isVisible() then
            local spBankerBox = userNode:getChildByName("sp_faceBoxBanker")
            spBankerBox:runAction(cc.Sequence:create(cc.DelayTime:create(0.2), cc.Show:create(), cc.Blink:create(1.0, 6), cc.Hide:create())) 
        end 
    end

    -- --转换视角
    -- local viewID = self._scene:SwitchViewChairID(chairID)

    -- --庄家动画
    -- local userNode = self.m_csbNode:getChildByName("face" .. viewID)
    -- if not userNode then
    --     return 
    -- end 
    -- local spBankerBox = userNode:getChildByName("sp_faceBoxBanker")
    -- spBankerBox:runAction(cc.RepeatForever:create(cc.Blink:create(1.0, 4)))
end

function GameViewLayer:showCallBankerEnd(chairID)
    --转换视角
    local viewID = self._scene:SwitchViewChairID(chairID)

    --庄家动画
    local userNode = self.m_csbNode:getChildByName("face" .. viewID)
    if not userNode then
        return 
    end 
    local spBankerBox = userNode:getChildByName("sp_faceBoxBanker")

   --倍数显示
    local nodeRatio = userNode:getChildByName("nodeBankerRatio")
    

    local spine = SpineObj:create(cmd.RES.."game/action/kuang.json", cmd.RES.."game/action/kuang.atlas", false)
    userNode:addChild(spine)
    spine:setName("SpineCallBanker")

    self._scene:PlaySound(cmd.RES .. "sound_res/dingzhuang.WAV")

    --动画结束之后显示亮框
    local this = self
    spine:registerSpineEventHandler(function (event)
        spine:runAction(cc.Sequence:create(cc.DelayTime:create(0.01), cc.CallFunc:create(function()
            spine:removeFromParent()
            spBankerBox:stopAllActions()
            spBankerBox:setVisible(true)

            --显示倍数
            nodeRatio:setVisible(true)
        end)))
    end, 2) 
end


--设置玩家抢庄倍数
function GameViewLayer:showBankerRatio(ratioTable)
    for i=1, cmd.GAME_PLAYER do
        --转换视角
        local viewID = self._scene:SwitchViewChairID(i -1)

        local userNode = self.m_csbNode:getChildByName("face" .. viewID)

        local nodeRatio = userNode:getChildByName("nodeBankerRatio")
        local txt = nodeRatio:getChildByName("txt")
        if 0 == ratioTable[i] then
            txt:setString("1")
        else
            txt:setString(ratioTable[i] .. "")
        end
    end
end

--设置庄家
function GameViewLayer:showBanker(bankerChairID, isAni)
    --转换视角
    local viewID = self._scene:SwitchViewChairID(bankerChairID)

    local this = self
    --查找玩家头像,控制庄家可见
    for i=1, cmd.GAME_PLAYER do
        local userNode = self.m_csbNode:getChildByName("face" .. i)
        if userNode then
            local sp_banker = userNode:getChildByName("sp_banker")
            if viewID ~= i then
                sp_banker:setVisible(false)
            end
            local spBankerBox = userNode:getChildByName("sp_faceBoxBanker")
            spBankerBox:stopAllActions()
            spBankerBox:setVisible(false)
            -- userNode:removeChildByName("SpineCallBanker")
        end
    end

    --庄家动画
    local userNode = self.m_csbNode:getChildByName("face" .. viewID)
    local sp_banker = userNode:getChildByName("sp_banker")
    sp_banker:setLocalZOrder(100)

    if isAni then
        local spine = SpineObj:create(cmd.RES.."game/action/zhuang.json", cmd.RES.."game/action/zhuang.atlas", false)
        this.m_csbNode:addChild(spine)
        spine:setPosition(cc.p(667, 450))

        spine:registerSpineEventHandler(function (event)
            spine:destroy()
            --创建庄家文字
            local sp_Banker = cc.Sprite:create("game/sp_banker.png")
            this.m_csbNode:addChild(sp_Banker)
            sp_Banker:setPosition(cc.p(667, 450))
            sp_Banker:setScale(4)

            sp_Banker:runAction(cc.Sequence:create(cc.Spawn:create(cc.MoveTo:create(0.5, cc.p(userNode:getPositionX() + 40, userNode:getPositionY() + 30)), cc.ScaleTo:create(0.5, 1)), 
                cc.CallFunc:create(function ()
                    sp_banker:setVisible(true)
                    sp_Banker:removeFromParent()
                    --显示闪烁动画
                    this:showCallBankerEnd(bankerChairID)
                end)))
        end, 2)
    else
        sp_banker:setVisible(true)
        --倍数显示
        local nodeRatio = userNode:getChildByName("nodeBankerRatio")
        nodeRatio:setVisible(true)
    end
end

--初始化下注界面
function GameViewLayer:initNodeBets(lJettonButton)
    local this = self
    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.began then
            this:onButtonClickedEvent(sender:getTag(), sender)
        end
    end

    --获取按钮
    for i=1, cmd.EM_JETTON_MAX do
        local btn = self.nodeBets:getChildByName("btn_" .. i)
        btn:setTag(TAG_ENUM.BT_JETTON + i -1)
        btn:addTouchEventListener(btnEvent)
        btn.score = lJettonButton[i]

        local txtScore = btn:getChildByName("txtScore")
        txtScore:setString(lJettonButton[i] .. "")
    end
end

--设置下注按钮是否可用
function GameViewLayer:setJettonButton(minScore, maxScore)
    for i=1, cmd.EM_JETTON_MAX do
        local btn = self.nodeBets:getChildByName("btn_" .. i)
  
        if btn.score < minScore or btn.score > maxScore then
            --设置按钮不可用
            btn:setEnabled(false)  
        end
    end
    if not self._scene.m_bLookOn then
        --显示下注
        self.nodeBets:setVisible(true)
    end
end
 

--金币动画
function GameViewLayer:showGoldAction(lScore, bankerViewId)

    local loseList = {} --输家
    local winList = {} --赢家 
    self.actionGoldList = {} --金币
    for i=1,cmd.GAME_PLAYER do
        
        --类型
        local viewid = self._scene:SwitchViewChairID(i-1)
        -- if GlobalUserItem.bVideo then
        --     if self.m_tabUserHead[viewid] then
        --         self.m_tabUserHead[viewid].m_clipScore:setString(self.m_tabUserHead[viewid]:getScoreString(lScore[i]))
        --     end
        -- end

        --赢输列表
        local viewid = self._scene:SwitchViewChairID(i-1)
        if lScore[i] ~= 0  and viewid ~= bankerViewId then
            
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

    local goldPoint = {cc.p(10,36),cc.p(0,33),cc.p(16,15),cc.p(26,12),cc.p(-8,23),cc.p(-21,16),cc.p(23,0),cc.p(3,7),cc.p(-10,6),cc.p(-8,-10),cc.p(-22,2),cc.p(-32,-9), cc.p(13,0),cc.p(8,1),cc.p(-15,16),cc.p(-1,-15),cc.p(-12,21),cc.p(-2,-19)}
    

    local pointBanker = cc.p(667, 326)
    if self._scene.cbRobBankerType ~= cmd.EM_OP_BANKER_TB then --非通比玩法
        local bankerViewId = self._scene:SwitchViewChairID(self._scene.wBankerUser)
        local userBankerNode = self.m_csbNode:getChildByName("face" .. bankerViewId)
        pointBanker = cc.p(userBankerNode:getPositionX(), userBankerNode:getPositionY())
    else --通比玩法
        -- local  callInScoreBg = self.nodeGold:getChildByName("sp_scoreBg") 
        -- pointBanker = cc.p(callInScoreBg:getPositionX(), callInScoreBg:getPositionY())
        bankerViewId = winList[1]
        local userBankerNode = self.m_csbNode:getChildByName("face" .. bankerViewId)
        pointBanker = cc.p(userBankerNode:getPositionX(), userBankerNode:getPositionY())
        --通比没有赢家
        winList = {}
    end

    local actionTime = 0.3
    local goldTable1 = {}
    --金币动画
    for i=1,#loseList do
        local viewid = loseList[i]
        local goldnum = 18
        goldTable1[i] = {}
        -- self._scene:PlaySound(cmd.RES .. "sound_res/fly_gold.wav")
        for j=1,goldnum do
            local pgold = cc.Sprite:create("game/im_fly_gold_1.png")
            goldTable1[i][j] = pgold
            pgold:setLocalZOrder(10+12-j)

            local userNode = self.m_csbNode:getChildByName("face" .. viewid)
            local pointUser = cc.p(userNode:getPositionX(), userNode:getPositionY())
            local loseBeginpos = cc.p(pointUser.x+goldPoint[j].x, pointUser.y+goldPoint[j].y)
            local bankerPos = getRandPos(pointBanker) or cc.p(0,0)

            pgold:setPosition(loseBeginpos)
            pgold:setVisible(true)
            pgold:runAction(cc.Sequence:create(cc.DelayTime:create(1+j*0.05), cc.CallFunc:create(function ()
                if 1 == j then
                    self._scene:PlaySound(cmd.RES .. "sound_res/fly_gold.wav")
                end
            end),
            self:getMoveActionEx(actionTime,loseBeginpos, bankerPos),cc.CallFunc:create(function()
                if j==12 then
                    self:runAction(cc.Sequence:create(cc.DelayTime:create(1),cc.CallFunc:create(function ( ... )
                        for n=1, goldnum do
                            if goldTable1[i][n] then
                                goldTable1[i][n]:removeFromParent()
                            end
                        end
                    end)))
                end
                
            end)))
            table.insert(self.actionGoldList, pgold)
            self.nodeGoldAction:addChild(pgold, 10)
        end
    end

    --延迟时间
    local winDelayTime = #loseList > 0 and 0.5 or 0
    local bankerPos = getRandPos(pointBanker) or cc.p(0,0)
    local goldTable2 = {}
    for i=1,#winList do
        local viewid = winList[i]
        local goldnum = 18
        goldTable2[i] = {}
        -- self._scene:PlaySound(cmd.RES .. "sound_res/fly_gold.wav")
        for j=1,goldnum do
            local pgold = cc.Sprite:create("game/im_fly_gold_1.png")
            goldTable2[i][j] = pgold

            local userNode = self.m_csbNode:getChildByName("face" .. viewid)
            local pointUser = cc.p(userNode:getPositionX(), userNode:getPositionY())
            local winEndPos = getRandPos(pointUser)
            pgold:setLocalZOrder(10+12-j)

            local beganPoint = cc.p(bankerPos.x+goldPoint[j].x,bankerPos.y+goldPoint[j].y)
            pgold:setPosition(beganPoint)
            pgold:setVisible(true)
            pgold:runAction(cc.Sequence:create(cc.DelayTime:create(1+j*0.05+winDelayTime),cc.CallFunc:create(function ()
                if 1 == j then
                    self._scene:PlaySound(cmd.RES .. "sound_res/fly_gold.wav")
                end
            end),
                self:getMoveActionEx(actionTime,beganPoint , winEndPos),cc.CallFunc:create(function()
                    if j==12 then
                        self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ( ... )
                            for n=1, goldnum do
                                if goldTable2[i][n] then
                                    goldTable2[i][n]:removeFromParent()
                                end
                            end
                        end)))
                    
                    end
              
            end)))
            table.insert(self.actionGoldList, pgold)
            self.nodeGoldAction:addChild(pgold, 10)
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
end

--显示玩家下注分数
function GameViewLayer:showCallInScore(viewID, score, isAni)
    if 0 == score then
        return
    end

    local callInScoreBg = self.nodeGold:getChildByName("sp_scoreBg" .. viewID)
    local userNode = self.m_csbNode:getChildByName("face" .. viewID)
    local pointUser = cc.p(userNode:getPositionX(), userNode:getPositionY())

    --是否是通比模式
    -- if self._scene.cbRobBankerType == cmd.EM_OP_BANKER_TB then
    --     callInScoreBg = self.nodeGold:getChildByName("sp_scoreBg")
    -- end

    if callInScoreBg then
        local txt = callInScoreBg:getChildByName("txtScore")
        txt:setString(score .. "")
        -- if self._scene.cbRobBankerType ~= cmd.EM_OP_BANKER_TB  then
        --     txt:setString(score .. "")
        -- else
        --     if txt:getString() ~= "" then
        --         local scoreCur = tonumber(txt:getString())
        --         txt:setString((score + scoreCur) .. "")
        --     else
        --         txt:setString(score .. "")
        --     end
        -- end
    end

    if isAni then
        self._scene:PlaySound(cmd.RES .. "sound_res/fly_gold.wav")
        for i=1,2 do
            local gold = cc.Sprite:create("game/im_fly_gold_1.png"):move(pointUser.x+10*i, pointUser.y-i*10):addTo(self.nodeGoldAction, 10)

            gold:runAction(cc.Sequence:create(cc.DelayTime:create(0.1+0.05*i),cc.Spawn:create(cc.MoveTo:create(0.3,cc.p(callInScoreBg:getPositionX() - 48.5,callInScoreBg:getPositionY())),cc.ScaleTo:create(0.3,0.6)),cc.DelayTime:create(0.2),cc.CallFunc:create(function ( ... )
                callInScoreBg:setVisible(true)
            end),cc.RemoveSelf:create()))
        end
    else
        callInScoreBg:setVisible(true)
    end
end


--显示下注金额
function GameViewLayer:showJettonScore(wChairID, dScore)
    if 0 == dScore then  --金额为0，场景消息使用
        return
    end

    --转换视角
    local viewID = self._scene:SwitchViewChairID(wChairID)

    --显示金币
    self.nodeGold:setVisible(true)
    local spGoldBg = self.nodeGold:getChildByName("sp_scoreBg" .. viewID)
    if spGoldBg then
        -- spGoldBg:setVisible(true)
        local txt = spGoldBg:getChildByName("txtScore")
        txt:setString(dScore .. "")

        self:showCallInScore(viewID, dScore, true)
    end

    --下注操作隐藏
    -- local userNode = self.m_csbNode:getChildByName("face" .. viewID)
    -- local spOp = userNode:getChildByName("spineOP")
    -- spOp:setVisible(false)
end

--发牌动画开始
function GameViewLayer:sendCardBegin()
    --数据校验
    if 0 == self._scene.sendCardCount or 0 == #self._scene.cbCardData then
        return
    end

    -- self._scene:PlaySound(cmd.RES .. "sound_res/SEND_CARD_BEGIN.wav")
    local delayCount = 1
    local count = 0
    for index = 1, self._scene.sendCardCount do
        for i = 1, cmd.GAME_PLAYER do
            if self._scene.onSaveUser[i] and self._scene.onSaveUser[i].wChairID ~= yl.INVALID_CHAIR and 1 == self._scene.m_cbPlayStatus[i]then
                if self._scene.cbRobBankerType == cmd.EM_OP_BANKER_MPQZ and 3 == self._scene.sendCardCount then
                    --名牌抢庄，第二次只发第三张
                    if 3 == index then
                        count = count + 1
                        self:SendCard(self._scene:SwitchViewChairID(i -1), index, delayCount * 0.01, self._scene.cbCardData[i][index])
                        delayCount = delayCount + 1
                    end
                else
                    count = count + 1
                    self:SendCard(self._scene:SwitchViewChairID(i -1), index, delayCount * 0.01, self._scene.cbCardData[i][index])
                    delayCount = delayCount + 1
                end
            end
        end
    end
end

--发牌动画
function GameViewLayer:SendCard(viewid,index,fDelay, cardData)
    if not viewid or viewid == yl.INVALID_CHAIR then
         print("SendCard---------------------->", viewid)
        return
    end

    print("SendCard---------------------->", viewid, index, cardData)

    local fInterval = 0.1

    --获取node位置
    local nodeCard = self.nodeCard:getChildByName("nodeCard" .. viewid)
    local posX = nodeCard:getPositionX()
    local posy = nodeCard:getPositionY()
    nodeCard:setVisible(true)
    local offset = {-1 , 0 , 1}
    local tagPos = cc.p(posX + (viewid==cmd.MY_VIEWID and 95 or 35) *offset[index], posy)

    local this = self
    
    --创建卡牌
    local spriteCard = CardSprite:createCard(cardData)
    self.nodeCard:addChild(spriteCard)
    spriteCard:showCardBack(true)
    spriteCard:stopAllActions()
    spriteCard:setScale(1.0)
    spriteCard:setVisible(true)
    spriteCard:move(667, 450)

    local cardIndex = index;

    spriteCard:runAction(
        cc.Sequence:create(
            cc.DelayTime:create(fDelay),
            cc.CallFunc:create(
                function ()
                    this._scene:PlaySound(cmd.RES .. "sound_res/fapai.mp3")
                    -- this._scene:PlaySound(cmd.RES.."sound_res/CENTER_SEND_CARD.wav")
                end
            ),
            cc.Spawn:create(
                cc.ScaleTo:create(0.15,viewid==cmd.MY_VIEWID and 1.0 or 0.5),
                cc.MoveTo:create(0.15, tagPos)),
            cc.DelayTime:create(fInterval),
            cc.CallFunc:create(
                function ()
                    spriteCard:removeFromParent()
                    --重新创建cardNode卡牌
                    local node = nodeCard:getChildByName("nodeCard")
                    local sp = node:getChildByName("img" .. cardIndex)

                    local spriteCardNew = nil

                    print("SendCard-------------->", cardIndex, viewid)
                    if (cardIndex == 3 and viewid == cmd.MY_VIEWID) or viewid ~= cmd.MY_VIEWID then
                        -- spriteCardNew:showCardBack(true)
                        spriteCardNew = CardSprite:createCard()
                    else
                        spriteCardNew = CardSprite:createCard(cardData)
                    end

                    if sp then 
                        sp:removeAllChildren()
                        sp:addChild(spriteCardNew)
                        spriteCardNew:setScale(1.0)
                        -- spriteCardNew:setVisible(true)
                        spriteCardNew:move(65, 90)
                        spriteCardNew:setTag(5)
                    end

                    --旁观不显示牌
                    if self._scene.m_bLookOn then
                        spriteCardNew:showCardBack(true)
                    end
                end
            )
        )
    )

end

--显示牌值
function GameViewLayer:SetUserCard(viewid, cardData, isAni, cardNum, showNum)
    print("GameViewLayer:SetUserCard++++++++++++++>", viewid, cardNum, showNum)
    if not viewid or viewid == yl.INVALID_CHAIR then
        return
    end

    --纹理
    local nodeCard = self.nodeCard:getChildByName("nodeCard" .. viewid)
    for i = 1, 3 do
        local node = nodeCard:getChildByName("nodeCard")
        local sp = node:getChildByName("img" .. i)

        if i <= cardNum then
            --设置牌值
            local spriteCard = sp:getChildByTag(5)
            print("GameViewLayer:SetUserCard ---------->", cardData.cbCards[1][i], spriteCard)

            if not isAni then
                if spriteCard then
                    spriteCard:setCardValue(cardData.cbCards[1][i])
                    spriteCard:updateSprite()
                    spriteCard:showCardBack(false)
                else
                    nodeCard:setVisible(true)
                    sp:removeAllChildren()
                    spriteCard = CardSprite:createCard(cardData.cbCards[1][i])
                    sp:addChild(spriteCard)
                    spriteCard:setScale(1.0)
                    spriteCard:setVisible(true)
                    spriteCard:move(65, 90)
                    spriteCard:setTag(5)
                end
                if showNum and i > showNum then
                    spriteCard:showCardBack(true)
                end
            else
                if not spriteCard then
                    nodeCard:setVisible(true)
                    sp:removeAllChildren()
                    spriteCard = CardSprite:createCard()
                    sp:addChild(spriteCard)
                    spriteCard:setScale(1.0)
                    spriteCard:setVisible(true)
                    spriteCard:move(65, 90)
                    spriteCard:setTag(5)
                end
                if viewid ~= cmd.MY_VIEWID or (viewid == cmd.MY_VIEWID and i == 3) or self._scene.m_bLookOn then
                    --做翻牌动画
                    spriteCard:runAction(cc.Sequence:create(
                        cc.DelayTime:create(i*0.05),
                        cc.ScaleTo:create(0.15,0.1, 1),
                        cc.CallFunc:create(function ()
                            spriteCard:showCardBack(false)
                            spriteCard:setCardValue(cardData.cbCards[1][i])
                            spriteCard:updateSprite()
                        end),
                        cc.ScaleTo:create(0.15, 1, 1)))
                end
            end
        end
    end

    --搓牌状态隐藏
    local userNode = self.m_csbNode:getChildByName("face" .. viewid)
    local spOp = userNode:getChildByName("spineOP")
    spOp:setVisible(false)
end

--搓牌监听
function moveCardEnd()
    local eventListener = cc.EventCustom:new("moveCardEnd")
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

--搓牌
function GameViewLayer:SetUserCuoCard(cardData)
    self.CuoLayer = cc.CSLoader:createNode(cmd.RES.."game/CuoLayer.csb"):addTo(self, 50)

    local cardValue = yl.POKER_VALUE[cardData] 
    local cardColor = yl.CARD_COLOR[cardData]  

    local cardLyaer = PageTurn:create(cmd.RES.."game/bigCard/cardBack.png",cmd.RES.."game/bigCard/card_"..cardColor.."_"..cardValue,cc.p(667,280))
    local panel = self.CuoLayer:getChildByName("shadow")


    local closeBtn = self.CuoLayer:getChildByName("btn_close")
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

--搓牌结束回调
function GameViewLayer:cuoActionEnd(isDelay)
    self._scene:PlaySound(cmd.RES .. "sound_res/cuopai.mp3")
    self._scene:onOpenCard()
    local delayTime = 0.01
    if isDelay then
        delayTime = 1.5
    end
    self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ()
            self:closeCuoLayer()
        end)))
end

--搓牌界面关闭
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

--比牌，显示牌型   
function GameViewLayer:SetUserCardType(viewid, cbCardType)
        
    --纹理
    local nodeCard = self.nodeCard:getChildByName("nodeCard" .. viewid)

    local node = nodeCard:getChildByName("nodeType")
    node:removeAllChildren()

    --获取倍数
    local times = self._scene.lCardTypeTime[1][cbCardType + 1]

    local typeName = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "s", "z", "zz"}

    --创建骨骼
    local spine = SpineObj:create(cmd.RES.."game/action/px.json", cmd.RES.."game/action/px.atlas", false)
    node:addChild(spine)

    print("SetUserCardType------>",cbCardType, typeName[cbCardType], times)
    if 0 == cbCardType then
        spine:setAnimation(0, "animation", false)
    elseif 0 < cbCardType and 7 > cbCardType then
        spine:setSkin(typeName[cbCardType].. ".".. times .. "d")
        print("SetUserCardType------>" ..typeName[cbCardType].. ".".. times .. "d")
        spine:setAnimation(0, "animation2", false)   
    elseif 7 <= cbCardType and 11 > cbCardType then
        if cbCardType <= 9 then
            spine:setSkin(typeName[cbCardType].. ".".. times.. "d")
        else
            spine:setSkin(typeName[cbCardType].. times)
        end
        spine:setAnimation(0, "animation3", false)
    elseif 11 <= cbCardType  then
        spine:setSkin(typeName[cbCardType].. times)
        spine:setAnimation(0, "animation4", false)
    end
end

--显示玩家输赢分数
function GameViewLayer:showUserWinLoseScore(bankerChairID, dUserWinloseScore)
    local bankViewID = self._scene:SwitchViewChairID(bankerChairID)

    for i=1, cmd.GAME_PLAYER do
        local viewid = self._scene:SwitchViewChairID(i -1)
        local score = dUserWinloseScore[i]

        local winLoseTitle = cc.CSLoader:createNode(cmd.RES.."game/NodeWinLoseScore.csb"):addTo(self.m_csbNode, 50) 

        --获取头像位置
        local userNode = self.m_csbNode:getChildByName("face" .. viewid)
        winLoseTitle:setPosition(cc.p(userNode:getPositionX(), userNode:getPositionY() - 28))
        
        local winNode = winLoseTitle:getChildByName("nodeWin");
        local loseNode = winLoseTitle:getChildByName("nodeLose");

        if 0 == score then
            winLoseTitle:removeFromParent()
        elseif score > 0 then  -- 赢
            winNode:setVisible(true)
            loseNode:setVisible(false)

            local txtScore = winNode:getChildByName("txtScore")

            local scoreTxt = "" .. score
            local flagPath = ""
            if score >= 100000000 then
                scoreTxt =ExternalFun.keepTwoDecimalPlaces(score/100000000)
                flagPath = cmd.RES.."game/sp_resultNumWinY.png"
            elseif score >= 10000 then
                scoreTxt =ExternalFun.keepTwoDecimalPlaces(score/10000)
                flagPath = cmd.RES.."game/sp_resultNumWinW.png"
            else
                scoreTxt = ExternalFun.keepTwoDecimalPlaces(score)
            end
            txtScore:setString("/" .. score)

        
            local spFlag = winNode:getChildByName("imgFlag")
            if flagPath == "" then
                spFlag:setVisible(false)
                txtScore:setPositionX(0)
            else
                spFlag:setVisible(true)
                spFlag:loadTexture(flagPath)
                local lenth = txtScore:getContentSize().width
                spFlag:setPositionX(lenth /2 +5)
            end
      
        else
            winNode:setVisible(false)
            loseNode:setVisible(true)

            local txtScore = loseNode:getChildByName("txtScore")

            score = math.abs(score)
            local scoreTxt = "" .. score
            local flagPath = ""
            if score >= 100000000 then
                scoreTxt =ExternalFun.keepTwoDecimalPlaces(score/100000000)
                flagPath = cmd.RES.."game/sp_resultNumLoseY.png"
            elseif score >= 10000 then
                scoreTxt =ExternalFun.keepTwoDecimalPlaces(score/10000)
                flagPath = cmd.RES.."game/sp_resultNumLoseW.png"
            else
                scoreTxt = ExternalFun.keepTwoDecimalPlaces(score)
            end
            txtScore:setString("/" .. score)

        
            local spFlag = loseNode:getChildByName("imgFlag")
            if flagPath == "" then
                spFlag:setVisible(false)
                txtScore:setPositionX(0)
            else
                spFlag:setVisible(true)
                spFlag:loadTexture(flagPath)
                local lenth = txtScore:getContentSize().width
                spFlag:setPositionX(lenth /2 +5)
            end
        end

        -- winLoseTitle:setScale(2.0)
        winLoseTitle:runAction(cc.Sequence:create(cc.Spawn:create(cc.FadeIn:create(0.5), cc.MoveBy:create(0.5,cc.p(0, 56))), 
            cc.DelayTime:create(1), cc.Spawn:create(cc.ScaleTo:create(0.5, 0.5), cc.FadeOut:create(0.5)), cc.CallFunc:create(function ()
            winLoseTitle:removeFromParent()
        end)))
    end
end


--显示托管
function GameViewLayer:ShowTrust(isTrust)  
    self._scene.trust = isTrust
    self.nodeTrust:setVisible(isTrust)

    if isTrust and 1 == self._scene.m_cbPlayStatus[self._scene:GetMeChairID() + 1] then
        --判断当前状态
        if self._scene.cbStatus == cmd.EM_STATUS_ROBBANKER then
            if self.nodeGetBanker:isVisible() then
                --托管抢庄
                if 1 == self._scene.trustConfig.BankerType then
                    self._scene:onGetBanker(0)
                else
                    self._scene:onGetBanker(self._scene.trustConfig.BankerRatio)
                end
            end
        elseif self._scene.cbStatus == cmd.EM_STATUS_JETTON then
            if self.nodeBets:isVisible() then
                self._scene:addScoreTrust()
            end
        elseif self._scene.cbStatus == cmd.EM_STATUS_FREE then
            local btn = self.m_csbNode:getChildByName("btn_ready")
            if btn:isVisible() then
                self._scene:sendReady()
                --清理
                self:cleanDesk()
                --隐藏倒计时
                self:OnUpdataClockViewByStatus(0, 0)

                --控制准备和换桌不可见
                local btn = self.m_csbNode:getChildByName("btn_ready")
                btn:setVisible(false)
                --换桌
                local btn = self.m_csbNode:getChildByName("btn_changeDesk")
                btn:setVisible(false)
            end
            --如果已经出现开始按钮
            if self._beginTimeId and self.m_beginTimeNum > 0 then
                self.btStart:setVisible(false)
                self._scene._gameFrame:SendUserBegin()

                if self._beginTimeId then
                    cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
                    self._beginTimeId = nil
                end 
            end
        elseif self._scene.cbStatus == cmd.EM_STATUS_OPEN_CARD then
            self:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function ()
                --显示翻牌
                self._scene:PlaySound(cmd.RES .. "sound_res/cuopai.mp3")
                self._scene:onOpenCard()
            end)))
        end
    end
end 

--初始化房间信息
function GameViewLayer:initRoomInfo()
    local csbNode = cc.CSLoader:createNode(cmd.RES .. "game/info/LayerInfo.csb"):addTo(self, 50)
    csbNode:setVisible(false)
    self.RoomInfoLayer = csbNode

    local infoNode = csbNode:getChildByName("nodeInfo")
    local recordNode = csbNode:getChildByName("nodeRecord")
    recordNode:setVisible(false)

    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            if TAG_ENUM.BT_CHECKBOX + 1 == sender:getTag() then 
                infoNode:setVisible(true)
                recordNode:setVisible(false)
                csbNode:getChildByName("check_1"):setSelected(true)
                csbNode:getChildByName("check_2"):setSelected(false)
            elseif TAG_ENUM.BT_CHECKBOX + 2 == sender:getTag() then
                infoNode:setVisible(false)
                recordNode:setVisible(true)
                csbNode:getChildByName("check_1"):setSelected(false)
                csbNode:getChildByName("check_2"):setSelected(true)
            end   
        end
    end

    --切换按钮
    for i = 1, 2 do
        local checkbx = csbNode:getChildByName("check_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(TAG_ENUM.BT_CHECKBOX + i)
            checkbx:addTouchEventListener(cbtlistener)
        end
    end   
    csbNode:getChildByName("check_1"):setSelected(true)
    csbNode:getChildByName("check_2"):setSelected(false)

    --初始化信息
    --玩法
    local rule = {"霸王庄", "轮流坐庄", "牌大坐庄", "明牌抢庄", "自由抢庄", "通比模式"}
    local txt_rule = infoNode:getChildByName("txt_rule")
    txt_rule:setString(rule[self._scene.cbRobBankerType])

    --底分
    local txt_enterScore = infoNode:getChildByName("txt_enterScore")
    txt_enterScore:setString(self._scene.dCellScore)

    --翻倍规则
    local betMode = {"不翻倍", "通用翻倍", "几分几倍"}
    local txt_bet  = infoNode:getChildByName("txt_bet ")
    txt_bet :setString(betMode[self._scene.cbCardTypeTimesRule +1])

    local txt_round = infoNode:getChildByName("txt_round")  
    txt_round:setString(PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)

    local txt_xScore = infoNode:getChildByName("txt_xScore")
    txt_xScore:setString(self._scene.bSupportTuizhu and "翻倍" or "无")

    --注册按钮事件
    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.began then
            if TAG_ENUM.BT_EXIT == sender:getTag() then 
                self.RoomInfoLayer:setVisible(false)
            elseif TAG_ENUM.BT_DISMISS == sender:getTag() then
                print("BT_DISMISS!!")
                self.RoomInfoLayer:setVisible(false)
                PriRoom:getInstance():queryDismissRoom(self._scene.m_cbGameStatus, self._scene:onGetSitUserNum())
            end   
        end
    end

    --解散按钮
    local btn = infoNode:getChildByName("btn_dismiss")
    btn:setTag(TAG_ENUM.BT_DISMISS)
    btn:addTouchEventListener(btnEvent)
    
     --背景
    local shadow = csbNode:getChildByName("shadow")
    shadow:setTag(TAG_ENUM.BT_EXIT)
    shadow:addTouchEventListener(btnEvent)

    -- 退出按钮
    local btn = csbNode:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BT_EXIT)
    btn:addTouchEventListener(btnEvent)

    
    --初始化流水
    -- 列表
    local tmpcontent = self.RoomInfoLayer:getChildByName("nodeRecord"):getChildByName("Panel_1")
    local contentsize = tmpcontent:getContentSize()
    self.m_tabList = {_data={},index=0}
    self._listView = cc.TableView:create(contentsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    --self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN) 
    self._listView:setPosition(tmpcontent:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.RoomInfoLayer:getChildByName("nodeRecord"))
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()

end

--显示房间详情
function GameViewLayer:showRoomInfo()
    self:updataScoreList()
    self.RoomInfoLayer:setVisible(true)
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
        if item then
            table.insert(userList,item)
        end
    end
    self:updateDetailNick(userList)
    dump(userList,"===============")
end

function GameViewLayer:updateDetailNick( userList )
    if not self.RoomInfoLayer or (#userList == 0) then
        return 
    end

    local num = 0
    for i=1,cmd.GAME_PLAYER do
        local item = userList[i]
        dump(item, "updateDetailNick", 6)
        if item then
        
            num = num+1
            if self._scene.cbPlayMode == 0 then
                
                local strNickname = string.EllipsisByConfig(item.szNickName, 74, 
                                                        string.getConfig("fonts/FZHTJW.TTF", 20))

                local nickTTF = self.RoomInfoLayer:getChildByName("nodeRecord"):getChildByName("name"..num)
                nickTTF:setVisible(true)
                nickTTF:setString(strNickname)

                print("updateDetailNick---->", num)

                if item.wChairID == self._scene:GetMeChairID() then 
                    nickTTF:setColor(cc.c3b(255, 255, 0))
                end
            else
                
                if item.wChairID == self._scene._MyChairID then 
                    local strNickname = string.EllipsisByConfig(item.szNickName, 74, 
                                                        string.getConfig("fonts//FZHTJW.TTF", 20))

                    local nickTTF = self.RoomInfoLayer:getChildByName("nodeRecord"):getChildByName("name4")
                    nickTTF:setVisible(true)
                    nickTTF:setString(strNickname)
                end
            end
        end
    end
end

-- 子视图大小
function GameViewLayer:cellSizeForTable(view, idx)
    return 760, 50
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

    -- 局数
    local strRound =(idx + 1) .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit

    if GlobalUserItem.bPrivateGoldRoom then
        strRound = idx + 1
    end

    cc.Label:createWithTTF(strRound, "fonts/round_body.ttf", 24)
    :setAnchorPoint(cc.p(0.5, 0.5))
    :setColor(cc.c3b(143, 172, 231))
    :move(45, 25)
    :addTo(cell)

    local MyChair = self._scene:GetMeChairID() + 1
    -- 分数
    for i = 1, cmd.GAME_PLAYER do
        if GlobalUserItem.bPrivateGoldRoom then
            if i == MyChair then
                local score = iteminfo[i]
                local userItem = self._scene:getUserInfoByChairID(i - 1)
                if nil ~= userItem then
                    local l = cc.Label:createWithTTF("" .. score, "fonts/round_body.ttf", 20)
                    :setAnchorPoint(cc.p(0.5, 0.5))
                    -- :setColor(cc.c3b(114, 84, 9))
                    :move(85 * 4 + 31, 25)
                    :addTo(cell)
                    if i - 1 == self._scene:GetMeChairID() then
                        l:setColor(cc.c3b(255, 255, 0))
                    end
                end
            end
        else
            local score = iteminfo[i]
            local userItem = self._scene:getUserInfoByChairID(i - 1)
            if nil ~= userItem then
                local l = cc.Label:createWithTTF("" .. score, "fonts/round_body.ttf", 20)
                :setAnchorPoint(cc.p(0.5, 0.5))
                -- :setColor(cc.c3b(114, 84, 9))
                :move(85 * i + 31, 25)
                :addTo(cell)

                if score > 0 then
                    l:setColor(cc.c3b(253, 30, 61))
                else
                    l:setColor(cc.c3b(132, 208, 255))
                end

                if i - 1 == self._scene:GetMeChairID() then
                    l:setColor(cc.c3b(255, 255, 0))
                end
            end
        end
    end

    --线
    local spLine = display.newSprite(cmd.RES.."game/info/sp_line.png")
    :setAnchorPoint(cc.p(0.5, 0.5))
    :move(381, 1)
    :addTo(cell)

    return cell
end

-- 子视图数目
function GameViewLayer:numberOfCellsInTableView(view)
    return #self.m_tabList + 1
end

--显示聊天
function GameViewLayer:ShowUserChat(viewid ,message)
    print("ShowUserChat---------->", viewid, message)
    if message and #message > 0 then
        self.m_GameChat:showGameChat(false) 

        local sp_chat = self.m_csbNode:getChildByName("sp_chat" .. viewid)
        dump(sp_chat, "", 7)
        if not sp_chat then
            return
        end

        --判断是否正在显示聊天
        if sp_chat:isVisible() then
            sp_chat:stopAllActions()
        else
            sp_chat:setVisible(true)
        end

        local txt = sp_chat:getChildByName("txt")
        txt:setString(message)

        if txt:getContentSize().width > 80 then
            sp_chat:setContentSize(cc.size(txt:getContentSize().width + 30, 49))
        else
            sp_chat:setContentSize(cc.size(113, 49))
        end
        sp_chat:runAction(cc.Sequence:create(
                        cc.DelayTime:create(3),
                        cc.CallFunc:create(function()
                            sp_chat:setVisible(false)
                        end)
                ))
    end
end

--表情
function GameViewLayer:ShowUserExpression(viewid,idx)
 
    --获取玩家头像
    local userNode = self.m_csbNode:getChildByName("face" .. viewid)
    if not userNode or not userNode:isVisible() then
        return
    end

    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", idx))

      --加载动画
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
    local  animation = cc.Animation:createWithSpriteFrames(frames,actionTime)    
    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", idx))
    local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
    userNode:addChild(spr)
    spr:setName("EMOJ_CHAT")
    spr:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    userNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()
        print("removeFromParent!!!!!!!!!!!!!!!!!")
        if spr and not tolua.isnull(spr) then
            spr:removeFromParent()
        end
    end)))
end

--显示语音
function GameViewLayer:ShowUserVoice(viewid, isPlay)
    --取消文字
    local sp_chat = self.m_csbNode:getChildByName("sp_chat" .. viewid)
    dump(sp_chat, "", 7)
    if not sp_chat then
        return
    end

     --判断是否正在显示聊天
    if sp_chat:isVisible() then
        sp_chat:stopAllActions()
    else
        sp_chat:setVisible(true)
    end

    if isPlay == false then
        sp_chat:setVisible(false)
        sp_chat:removeChildByName("sp_voice")
    else
        --创建帧动画
        -- 聊天表情
        sp_chat:removeChildByName("sp_voice")
        local sp = display.newSprite(cmd.RES.."game/sp_voice1.png")
        sp:setAnchorPoint(cc.p(0.5, 0.5))
        sp:runAction(self.m_actVoiceAni)
        sp:addTo(sp_chat)
        sp:setName("sp_voice")

        sp:setPosition(cc.p(50, 25))
        -- 转向
        if viewid == 6 or viewid == 7 or viewid == 8 then
            sp:setRotation(180)
        end
    end
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    local beginPoint = nil
    local endPoint = nil
    local zOrder = 1
    
    --获取头像坐标
    for k,v in pairs(self._scene.onSaveUser) do
          if v.wChairID ~= yl.INVALID_CHAIR then
            local viewid = self._scene:SwitchViewChairID(v.wChairID)
            local userNode = self.m_csbNode:getChildByName("face" .. viewid)
            if not userNode or not userNode:isVisible() then
                return
            end
            if v.dwUserID == dwUserID then
                beginPoint = cc.p(userNode:getPosition())
            elseif v.dwUserID ==  dwRecvUserID then
                endPoint = cc.p(userNode:getPosition())
                zOrder = userNode:getZOrder() + 1
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



--------------------------------------------------aaaaaaaaaaaaaaaaaaa---------------------------------



function GameViewLayer:createBlinkDots(parent,pos,dt,n,bReverse)
		dt =dt or 0.5
		local offX=bReverse and -10 or 10
		n= n or 5
		for i=1,n do
			local dot=cc.Sprite:create("game/dot.png"):addTo(parent)
			dot:setPosition(cc.p(pos.x+i*offX,pos.y))
			dot:setVisible(false)
			
			local d1,d2=i*dt,(n-i)*dt --d1隐藏时间 d2显示时间
			local seq=cc.Sequence:create(cc.DelayTime:create(d1),cc.Show:create(),cc.DelayTime:create(d2),cc.Hide:create())
			dot:runAction(cc.RepeatForever:create(seq))
		end
end

function GameViewLayer:updateUI()
	if GlobalUserItem.bPrivateGoldRoom then
		for i = 1, cmd.GAME_PLAYER do
			if self.m_ScoreView[i].frame then
				if i == 4 then
					self.m_ScoreView[i].frame:move(ptCoin[i].x-40, ptCoin[i].y-15)
				else
					self.m_ScoreView[i].frame:move(ptCoin[i].x-40, ptCoin[i].y)
				end
			end
    	end
    end
end

--托管界面是否可见
function GameViewLayer:updateAutoOutCardVisible()
	if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
		self.autoLayer:setVisible(false)
	else
		self.autoLayer:setVisible(true)
	end
end


--屏幕点击
function GameViewLayer:onTouch(eventType, x, y)
	if eventType == "began" then
		--牌型显示判断
--		if self.bIntroduce == true then
--			return true
--		end

		if self.m_bShowMenu == true then
			local rc = self.m_AreaMenu:getBoundingBox()
			if rc then
				if not cc.rectContainsPoint(rc,cc.p(x,y)) then
					self:ShowMenu(false)
					return true
				end
			end
		end

		--结算框
		if self.m_GameEndView:isVisible() then
			local rc = self.m_GameEndView:GetMyBoundingBox()
			if rc and not cc.rectContainsPoint(rc, cc.p(x, y)) then
				self.m_GameEndView:setVisible(false)
				return true
			end
		end

		return false
	elseif eventType == "ended" then
	 	-- if self.trustBg:isVisible() then 
	 	-- 	self._scene:sendTrustee(false)
	 	-- end
	end

	return true
end

--金币
function GameViewLayer:playerGold(wViewChairId, num)
--    for i = 1,num do
    for i = 1, 6 do
	    local gold = display.newSprite("#"..cmd.GAME.."bg_gold.png")
	        :setPosition(cc.p(ptPlayer[wViewChairId].x+ math.random(80) - 50, ptPlayer[wViewChairId].y + math.random(80) - 50))
            :setTag(wViewChairId)
		    :addTo(self.nodeGoldPool)
    end
end

--输的金币
function GameViewLayer:loseTheGold()
    local children = self.nodeGoldPool:getChildren()
    local childrenTag = { { }, { }, { }, { }, { }, { } }
    for k, v in pairs(children) do
        table.insert(childrenTag[v:getTag()], v)
    end
    for m, n in pairs(childrenTag) do
        for k, v in pairs(n) do
            v:runAction(cc.Sequence:create(cc.DelayTime:create(((#n - k) >= 10 and 1 or 0.1 * (#n - k))),
            cc.MoveTo:create(0.4, cc.p(667 + math.random(80) - 50, 335 + math.random(40) -30 )))
            )
        end
    end
end

--赢得金币
function GameViewLayer:winTheGold(wWinners)
    local children = self.nodeGoldPool:getChildren()
    self.nodeGoldPool:removeAllChildren()
    local countBefore = 0
    local countAfter = 0
    self.userWinLight = { }
    for m, n in pairs(wWinners) do
        self.userWinLight[m] = cc.Director:getInstance():getScheduler():scheduleScriptFunc( function()
            self:runAction(cc.Sequence:create(
            cc.CallFunc:create( function()
                if self.userWinLight and self.userWinLight[m] then
                    -- 注销时钟
                    cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.userWinLight[m])
                    self.userWinLight[m] = nil
                end
                self.userWinLight[m] = cc.Director:getInstance():getScheduler():scheduleScriptFunc( function()
                    if self._scene.m_cbGameStatus == cmd.GS_TK_FREE then
                        self:runAction(cc.Sequence:create(
                        cc.CallFunc:create( function()
                            if self._scene.m_cbGameStatus == cmd.GS_TK_FREE then
                                self.m_UserHead[m].bgls:setVisible(true)
                                self.m_UserHead[m].bgll:setVisible(false)
                            else
                                self:onClearTips()
                            end
                        end ),
                        cc.DelayTime:create(1),
                        cc.CallFunc:create( function()
                            if self._scene.m_cbGameStatus == cmd.GS_TK_FREE then
                                self.m_UserHead[m].bgls:setVisible(false)
                                self.m_UserHead[m].bgll:setVisible(true)
                            else
                                self:onClearTips()
                            end
                        end ),
                        cc.DelayTime:create(1)
                        ))
                    else
                         self:onClearTips()
                    end
                end , 2, false)
                if self._scene.m_cbGameStatus == cmd.GS_TK_FREE then
                    self.m_UserHead[m].bgls:setVisible(true)
                    self.m_UserHead[m].bgll:setVisible(false)
                else
                     self:onClearTips()
                end
            end ),
            cc.DelayTime:create(1),
            cc.CallFunc:create( function()
                if self._scene.m_cbGameStatus == cmd.GS_TK_FREE then
                    self.m_UserHead[m].bgls:setVisible(false)
                    self.m_UserHead[m].bgll:setVisible(true)
                else
                    self:onClearTips()
                end
            end ),
            cc.DelayTime:create(1)
            ))
        end , 0, false)
        countAfter = 6 + countAfter

        for i = 1, 6 do
	    local gold = display.newSprite("#"..cmd.GAME.."bg_gold.png")
	        :setPosition(cc.p(667 + math.random(80) - 50, 335 + math.random(40) -30 ))
		    :addTo(self.nodeGoldPool)
        end
        children = self.nodeGoldPool:getChildren()

        for k, v in pairs(children) do
            if k <= countAfter and k >= countBefore then
                v:stopAllActions()
                v:runAction(cc.Sequence:create(cc.DelayTime:create(((#children - k) >= 10 and 1 or 0.1 * (#children - k))),
                cc.MoveTo:create(0.4, cc.p(ptPlayer[m].x + math.random(80) -50, ptPlayer[m].y + math.random(80) -50)),
--                cc.DelayTime:create(1),
                cc.CallFunc:create( function(node)
                    node:removeFromParent()
                end )))
            end
        end
        countBefore = 6 + countBefore
    end
    
    for i = 1, cmd.GAME_PLAYER do
        if not self._scene.onSaveUser[i] then
            local k = self._scene:SwitchViewChairID(i - 1)
            if self.userWinLight then
                if self.userWinLight[k] then
                   -- 注销时钟
                   cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.userWinLight[k])
                   self.userWinLight[k] = nil
                end
            end
        end
    end
end

--清理金币
function GameViewLayer:CleanAllGold()
	self.nodeGoldPool:removeAllChildren()
end

--停止比牌动画
function GameViewLayer:StopCompareCard()
	-- self.m_CompareView:setVisible(false)
	-- self.m_CompareView:StopCompareCard()
end

--比牌
function GameViewLayer:CompareCard(userCard,firstuser,seconduser,firstcard,secondcard,bfirstwin,callback)
	-- self.m_CompareView:setVisible(true)
	-- self.m_CompareView:CompareCard(userCard,firstuser,seconduser,firstcard,secondcard,bfirstwin,callback)
end

--庄家显示
function GameViewLayer:SetBanker(viewid)
	if not viewid or viewid == yl.INVALID_CHAIR then
		self.m_BankerFlag:setVisible(false)
		return
	end
	local x
	local y
	if viewid == 2 or viewid == 4 then
		x = ptPlayer[viewid].x + 55 
		y = ptPlayer[viewid].y + 81
	elseif viewid == 3 then
		x = ptPlayer[viewid].x + 110
		y = ptPlayer[viewid].y + 49
    else
        x = ptPlayer[viewid].x + 100
		y = ptPlayer[viewid].y + 44
	end

	self.m_BankerFlag:setPosition(x, y)
	self.m_BankerFlag:setVisible(true)
end

--房主显示
function GameViewLayer:SetRoomHolder(viewid)
	if not viewid or viewid == yl.INVALID_CHAIR then
		self.m_RoomHolderFlag:setVisible(false)
		return
	end
	local x
	local y
	if viewid == 2 or viewid == 4 then
		x = ptPlayer[viewid].x - 60
		y = ptPlayer[viewid].y + 90
    elseif viewid == 3 then
        x = ptPlayer[viewid].x - 122 -30 - 10
		y = ptPlayer[viewid].y + 55
	else
		x = ptPlayer[viewid].x - 111 -30 - 10
		y = ptPlayer[viewid].y + 51
	end

	self.m_RoomHolderFlag:setPosition(x, y)
	self.m_RoomHolderFlag:setVisible(true)
end

--玩家下注
function GameViewLayer:SetUserTableScore(viewid, score)
	--增加桌上下注游戏币
	if not score or score == 0 then
	    self.m_ScoreView[viewid].frame:setVisible(false)
		self.m_ScoreView[viewid].score:setVisible(false)
	else
	    self.m_ScoreView[viewid].frame:setVisible(true)
		self.m_ScoreView[viewid].score:setVisible(true)
		self.m_ScoreView[viewid].score:setString(ExternalFun.keepTwoDecimalPlaces(score))
	end
    if self._scene.wBankerUser then
       print("GameLayer:onSubAddScore", viewid, score, self._scene:SwitchViewChairID(self._scene.wBankerUser))
       if viewid == self._scene:SwitchViewChairID(self._scene.wBankerUser) then
           self.m_ScoreView[viewid].frame:setVisible(false)
           self.m_ScoreView[viewid].score:setVisible(false)
       end
   end
end

--按键响应
function GameViewLayer:OnButtonClickedEvent(tag,ref)
	local myUserItem = self._scene:GetMeUserItem()
	--最大输赢分数
	if tag == GameViewLayer.BT_EXIT then
		local MyChair =self._scene:GetMeChairID()+1
	    if self._scene.cbPlayStatus and self._scene.cbPlayStatus[MyChair] == 0 then
	    	self._scene.m_cbGameStatus = cmd.GAME_STATUS_FREE
	    end
        self._scene:onQueryExitGame()
    elseif tag == GameViewLayer.BT_GAMEBEGIN then
--		self._scene:onGameBegin(true)
        self._scene:onStartGame(true)
	elseif tag == GameViewLayer.BT_READY then
		self._scene:onStartGame(true)
    elseif tag == GameViewLayer.BT_CALLBANKER then
		self._scene:onCallBanker(true)
    elseif tag == GameViewLayer.BT_NO_CALLBANKER then
		self._scene:onCallBanker(false)
    elseif tag == GameViewLayer.BT_ADDSCORE_1 then
    	if myUserItem.lScore < self._scene.lCellScore * cmd.MAX_TIMES then
    		showToast(self, "金币不足！请重新选择！", 1)
    		return
    	end
		self._scene:onAddScore(self._scene.lCellScore)
    elseif tag == GameViewLayer.BT_ADDSCORE_2 then
    	if myUserItem.lScore < 2*self._scene.lCellScore then
    		showToast(self, "金币不足！请重新选择！", 1)
    		return
    	end
		self._scene:onAddScore(2*self._scene.lCellScore)
    elseif tag == GameViewLayer.BT_ADDSCORE_3 then
    	if myUserItem.lScore < 3*self._scene.lCellScore * cmd.MAX_TIMES then
    		showToast(self, "金币不足！请重新选择！", 1)
    		return
    	end
		self._scene:onAddScore(3*self._scene.lCellScore)
    elseif tag == GameViewLayer.BT_ADDSCORE_4 then
    	if myUserItem.lScore < 4*self._scene.lCellScore * cmd.MAX_TIMES then
    		showToast(self, "金币不足！请重新选择！", 1)
    		return
    	end
		self._scene:onAddScore(4*self._scene.lCellScore)
    elseif tag == GameViewLayer.BT_ADDSCORE_5 then
    	if myUserItem.lScore < 5*self._scene.lCellScore * cmd.MAX_TIMES then
    		showToast(self, "金币不足！请重新选择！", 1)
    		return
    	end
		self._scene:onAddScore(5*self._scene.lCellScore)
    elseif tag == GameViewLayer.BT_FLIP_CARD then
		self._scene:onFlipCard()
    elseif tag == GameViewLayer.BT_CUO_CARD then
		self._scene:onCuoCard()
    elseif tag == GameViewLayer.BT_OPEN_CARD then
        self._scene:PlaySound(cmd.RES .. "sound_res/cuopai.mp3")
		self._scene:onOpenCard()
	elseif tag == GameViewLayer.BT_CHAT then
		self.m_GameChat:showGameChat(true)
		self:ShowMenu(false)
	elseif tag == GameViewLayer.BT_MENU then
		self:ShowMenu(not self.m_bShowMenu)
	elseif tag == GameViewLayer.BT_SET then
		self:ShowMenu(false)
		self._settingLayer:showLayer()
	elseif tag == GameViewLayer.BT_BANK then
		showToast(self, "该功能尚未开放，敬请期待...", 1)
	elseif tag == GameViewLayer.BT_LOCATION then
		self:showAdressLayer()
	elseif tag == GameViewLayer.BT_CHAT then
		self._chatLayer:showGameChat(true)
	elseif tag == GameViewLayer.BT_CHANGEDESK then
		self:onChangeDesk()
	elseif tag == GameViewLayer.BT_TRUSTEE then
		if self._scene.trust then
			self._scene:sendTrustee(false, GameViewLayer.JETTON_MODE)
		else
			self._scene:sendTrustee(true, GameViewLayer.JETTON_MODE)
		end
	elseif tag == GameViewLayer.BT_LIST then
		self:updateJettonState()
	elseif tag == GameViewLayer.BT_MIN then
		GameViewLayer.JETTON_MODE = 0
		self._scene:sendTrusteeJetton(GameViewLayer.JETTON_MODE)
	elseif tag == GameViewLayer.BT_MAX then
		GameViewLayer.JETTON_MODE = 1
		self._scene:sendTrusteeJetton(GameViewLayer.JETTON_MODE)
	end
end

--显示和隐藏最大最小下注金额
function GameViewLayer:updateJettonState()
	if self.btMax:isVisible() then
		self.btMax:setVisible(false)
		self.btMin:setVisible(false)
		self.btDownList:loadTextureNormal("game/btn_down1.png")
        self.btDownList:loadTexturePressed("game/btn_down2.png")
	else
		self.btMax:setVisible(true)
		self.btMin:setVisible(true)
		self.btDownList:loadTextureNormal("game/btn_up1.png")
        self.btDownList:loadTexturePressed("game/btn_up2.png")
	end
end

--更换下注额度的图片
function GameViewLayer:showJettonNum()
	if GameViewLayer.JETTON_MODE == 0 then
		self.btMax:loadTextureNormal("game/btn_max1.png")
		self.btMin:loadTextureNormal("game/btn_min2.png")
	elseif GameViewLayer.JETTON_MODE == 1 then
		self.btMax:loadTextureNormal("game/btn_max2.png")
		self.btMin:loadTextureNormal("game/btn_min1.png")
	end
end

--更换是否正在托管（自动进行中的图片）
function GameViewLayer:changeAutoImg()
	if self._scene.trust then
		self.btAuto:loadTextureNormal("game/btn_autoing1.png")
		self.btAuto:loadTexturePressed("game/btn_autoing2.png")
		self.btAuto:loadTextureDisabled("game/btn_autoing2.png")
	else
		self.btAuto:loadTextureNormal("game/btn_auto1.png")
		self.btAuto:loadTexturePressed("game/btn_auto2.png")
		self.btAuto:loadTextureDisabled("game/btn_auto2.png")
	end
end

function GameViewLayer:setBtChangeDeskEnabled(bEnable1,bEnable2)
	if GlobalUserItem.bPrivateRoom then
		bEnable1,bEnable2=false,false
		self.btReady:setPositionX(670)
	end

	self.btChangeDesk:stopAllActions()
	self.btMenuChangeDesk:stopAllActions()
	self.btChangeDesk:setVisible(bEnable1)
	self.btMenuChangeDesk:setEnabled(bEnable2)
	self.btMenuChangeDesk:setColor(bEnable2 and cc.c3b(255,255,255) or cc.c3b(100,100,100))
end

function GameViewLayer:ShowMenu(bShow)
	local this = self
	if self.m_bShowMenu ~= bShow then
		self.m_bShowMenu = bShow
		self.m_AreaMenu:stopAllActions()
		if self.m_bShowMenu == true and not self.m_AreaMenu:isVisible() then
			self.m_AreaMenu:setVisible(true)
			self.m_AreaMenu:runAction(
				cc.Sequence:create(
					cc.MoveTo:create(0.3,cc.p(1283,745)),
					cc.CallFunc:create(
					function()
						this:setMenuBtnEnabled(true)
					end
				)))
		elseif self.m_AreaMenu:isVisible() == true then
			self:setMenuBtnEnabled(false)
			self.m_AreaMenu:runAction(
				cc.Sequence:create(
					cc.MoveTo:create(0.3,cc.p(1283,745)),
					cc.CallFunc:create(
					function()
						this.m_AreaMenu:setVisible(false)
					end
				)))
		end
	end
end

function GameViewLayer:setMenuBtnEnabled(bAble)
	self.m_AreaMenu:getChildByTag(GameViewLayer.BT_SET):setEnabled(bAble)
--	self.m_AreaMenu:getChildByTag(GameViewLayer.BT_HELP):setEnabled(bAble)
	--self.m_AreaMenu:getChildByTag(GameViewLayer.BT_CHAT):setEnabled(bAble)
	self.m_AreaMenu:getChildByTag(GameViewLayer.BT_EXIT):setEnabled(bAble)
end

function GameViewLayer:runAddTimesAnimate(viewid)
	display.newSprite("#game_flag_addscore.png")
		:move(ptAddScore[viewid])
		:setScale(viewid == cmd.MY_VIEWID and 1.3 or 1.1)
		:addTo(self)
		:runAction(cc.Sequence:create(
						cc.DelayTime:create(2),
						cc.CallFunc:create(function(ref)
							ref:removeFromParent()
						end)
						))
end



--显示表情
-- function GameViewLayer:ShowUserExpression(viewid,index)
-- 	self.m_GameChat:showGameChat(false)
-- 	--取消上次
-- 	if self.m_UserChat[viewid] then
-- 		self.m_UserChat[viewid]:stopAllActions()
-- 		self.m_UserChat[viewid]:removeFromParent()
-- 		self.m_UserChat[viewid] = nil
-- 	end
-- 	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame( string.format("e(%d).png", index))
-- 	if frame then
-- 		self.m_UserChat[viewid] = cc.Sprite:createWithSpriteFrame(frame)
-- 			:addTo(self)
-- --		if viewid <= 3 then
-- --			self.m_UserChat[viewid]:move(ptChat[viewid].x + 45 , ptChat[viewid].y + 5)
-- --		else
-- 			self.m_UserChat[viewid]:move(ptChat[viewid].x - 45 , ptChat[viewid].y + 5)
-- --		end
-- 		self.m_UserChatView[viewid]:setVisible(true)
-- 			:setContentSize(90,65)
-- 		self.m_UserChat[viewid]:runAction(cc.Sequence:create(
-- 						cc.DelayTime:create(3),
-- 						cc.CallFunc:create(function()
-- 							self.m_UserChatView[viewid]:setVisible(false)
-- 							self.m_UserChat[viewid]:removeFromParent()
-- 							self.m_UserChat[viewid]=nil
-- 						end)
-- 				))
-- 	end
-- end



----------------------------------------------------------新大厅--------------------------------------------------
--初始化新大厅房间信息

function GameViewLayer:updateRoomInfo(bCallBanker, lCellScore)
    self.nodePlayMode1:setSpriteFrame(bCallBanker == false and cmd.GAME .. "gameinfo_select_1.png" or cmd.GAME .. "gameinfo_select_0.png")
    self.nodePlayMode2:setSpriteFrame(bCallBanker == false and cmd.GAME .. "gameinfo_select_0.png" or cmd.GAME .. "gameinfo_select_1.png")
end


function GameViewLayer:updataUserInfo()
    self.nodeScoreTitle:removeAllChildren()
    self.updataUserInfos = {}
    self.titleCountInfo = cc.Label:createWithTTF("局数", "fonts/round_body.ttf", 26)
            :setAnchorPoint(cc.p(0,0.5))
            :move(90,340)
            :setColor(cc.c3b(114,84,9))
            :addTo(self.nodeScoreTitle)
    local MyChair = self._scene:GetMeChairID() + 1
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._scene:getUserInfoByChairID(i - 1)
        if nil ~= userItem then
            if GlobalUserItem.bPrivateGoldRoom then
                 if i == MyChair then
                    local strNickname = string.EllipsisByConfig(userItem.szNickName, 60, 
                                string.getConfig("fonts/round_body.ttf", 20))
                    self.updataUserInfos[i] = cc.Label:createWithTTF(strNickname, "fonts/round_body.ttf", 26)
                    :setAnchorPoint(cc.p(0.5,0.5))
                    :move(105 + 4 * 85,340)
                    :setColor(cc.c3b(114,84,9))
                    :addTo(self.nodeScoreTitle)
                    self.updataUserInfos[i]:setColor(cc.c3b(110,13,61))
                 end
            else
                local strNickname = string.EllipsisByConfig(userItem.szNickName, 60, 
                                string.getConfig("fonts/round_body.ttf", 20))
                self.updataUserInfos[i] = cc.Label:createWithTTF(strNickname, "fonts/round_body.ttf", 26)
                :setAnchorPoint(cc.p(0.5,0.5))
                :move(105 + i * 85,340)
                :setColor(cc.c3b(114,84,9))
                :addTo(self.nodeScoreTitle)
                if i -1 == self._scene:GetMeChairID() then
                    self.updataUserInfos[i]:setColor(cc.c3b(110,13,61))
                end
            end
        end
    end
end

function GameViewLayer:hideLayer()
    self.RoomInfoLayer:setTouchEnabled(false)
    self.RoomInfoLayer:setVisible(false)
end

function GameViewLayer:onRoomInfoTouch(eventType, x, y)
	if eventType == "began" then
		return true
	end

	local pos = cc.p(x, y)
    local rectLayerBg = self.roomInfoBg:getBoundingBox()
    if not cc.rectContainsPoint(rectLayerBg, pos) then
        self:hideLayer()
    end

    return true
end

function GameViewLayer:showPlayRule( str1,str2 )
	self.ruleText:setVisible(GlobalUserItem.bPrivateRoom)
	self.cellScoreText:setVisible(GlobalUserItem.bPrivateRoom)
	if not GlobalUserItem.bPrivateRoom then return end
	self.ruleText:setString(str1)
	self.cellScoreText:setString("底分:"..str2)
end

function GameViewLayer:showAdressLayer()
	local posHead=ptPlayer
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
		if self.m_UserHead[i] and self.m_UserHead[i].head  then
			-- self.m_UserHead[i].head:queryUserLociton(self.m_UserHead[i].dwUserID)
			local userInfo = self.m_UserHead[i].head.m_useritem
			if nil ~= userInfo then
				self.m_UserHead[i].head:queryUserLociton(userInfo.dwUserID)
			end
			print("玩家"..i.."的userid是"..userInfo.dwUserID)
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
			address:setPosition(posHead[i])
			if i==cmd.MY_VIEWID then address:setPositionY(posHead[i].y+20) end
			address:setVisible(false)
			if self.m_UserHead[i] and self.m_UserHead[i].head then
				address:setVisible(true)
				
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
				local label = cc.Label:createWithTTF("", "fonts/round_body.ttf", 18)
				label:addTo(address)
				label:setAnchorPoint(0.5, 1)
				local x,y = address:getChildByName("location"):getPosition()
				address:getChildByName("location"):setVisible(false)
				label:setPosition(x-40,y+10)
				label:setDimensions(400,200)
				label:setString(placeName)
				label:setHorizontalAlignment(kCCTextAlignmentCenter)

				
				for j=i+1,cmd.GAME_PLAYER do
					local pi=posHead[i]
					local pj=posHead[j]

					local la2 = 0
					local lo2 = 0
					local distanceTxt  = panel1:getChildByName("dis"..i.."_"..j)
					local distanceImg = panel1:getChildByName("xian"..i.."_"..j)
					
					
					if self.m_UserHead[j] and self.m_UserHead[j].head then

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

						local userInfo = self.m_UserHead[i].head.m_userInfo
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