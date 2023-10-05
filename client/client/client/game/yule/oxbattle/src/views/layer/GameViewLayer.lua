
--
-- Author: zhouweixiang
-- Date: 2016-11-28 14:17:03
--
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC.."HeadSprite")
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")

local Game_CMD = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.CMD_Game")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.GameLogic")
local NIU_TYPE = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.niuType")
local SpineObj = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.SpineObj")

-- local CardSprite = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.CardSprite")
local CardSprite = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.CardSpriteVer2")
local SitRoleNode = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.SitRoleNode")
local uiResult = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.uiResult")

--弹出层
local SettingLayer = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.SettingLayer")
local UserListLayer = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.UserListLayer")
local ApplyListLayer = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.ApplyListLayer")
local GameRecordLayer = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.GameRecordLayer")
local GameResultLayer = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.GameResultLayer")
local HelpLayer = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.HelpLayer")
local SuperRobLayer = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.SuperRobLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local ActionMgr = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.ActionMgr")
local schedule = cc.Director:getInstance():getScheduler()

local GameViewLayer = class("GameViewLayer",function(scene)
        local gameViewLayer = display.newLayer()
    return gameViewLayer
end)

local TAG_START             = 100
local enumTable =
{
    "HEAD_BANKER",  --庄家头像
    "TAG_CARD",     --牌
    "TAG_NIU_TXT",  --牛点数
    "BT_MENU",		--菜单按钮
    "BT_LUDAN",     --路单
    "BT_BANK",		--银行
    "BT_CLOSEBANK", --关闭银行
    "BT_TAKESCORE",	--银行取款
    "BT_SAVESCORE",
    "BT_SET",       --设置
    "BT_HELP",      --帮助
    "BT_QUIT",      --退出
    "BT_REJECT",    --重复下注
    "BT_LEFT",      --左滑按钮
    "BT_RIGHT",     --右滑按钮
    "BT_SUPPERROB", --超级抢庄
    "BT_APPLY",     --申请上庄
    "BT_USERLIST",  --用户列表
    "BT_JETTONAREA_0",  --下注区域
    "BT_JETTONAREA_1",
    "BT_JETTONAREA_2",
    "BT_JETTONAREA_3",
    "BT_JETTONSCORE_0", --下注按钮
    "BT_JETTONSCORE_1",
    "BT_JETTONSCORE_2",
    "BT_JETTONSCORE_3",
    "BT_JETTONSCORE_4",
    "BT_JETTONSCORE_5",
    "BT_JETTONSCORE_6",
    "BT_SEAT_0",       --坐下
    "BT_SEAT_1",
    "BT_SEAT_2",
    "BT_SEAT_3",
    "BT_SEAT_4",
    "BT_SEAT_5",
    "BT_HELP",

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

enumTable = {
    "ZORDER_JETTON_GOLD_Layer", --下注时游戏币层级
    "ZORDER_GOLD_Layer", --游戏币层
    "ZORDER_CARD_Layer", --牌层
    "ZORDER_Other_Layer", --用户列表层等
}
local ZORDER_LAYER = ExternalFun.declarEnumWithTable(2, enumTable)

local enumApply =
{
    "kCancelState",
    "kApplyState",
    "kApplyedState",
    "kSupperApplyed"
}

GameViewLayer._apply_state = ExternalFun.declarEnumWithTable(0, enumApply)
local APPLY_STATE = GameViewLayer._apply_state
local GAME_START_ANIMATION_NAME = "GAME_START_ANIMATION_NAME"


local MaxTimes = 10   ---最大赔率

--下注数值
GameViewLayer.m_BTJettonScore  = {100, 1000, 10000, 100000, 1000000, 5000000, 10000000}

JettonSelect = 0

--下注值对应游戏币个数
GameViewLayer.m_JettonGoldBaseNum = {1, 1, 2, 2, 3, 3, 4}
--获得基本游戏币个数
GameViewLayer.m_WinGoldBaseNum = {2, 2, 4, 4, 6, 6, 6}
--获得最多游戏币个数
GameViewLayer.m_WinGoldMaxNum = {6, 6, 8, 8, 12, 12, 12}

--发牌位置
-- local cardpoint = {cc.p(690, 682), cc.p(196, 245), cc.p(196+246, 245), cc.p(196+246*2, 245), cc.p(196+246*3,245)}
-- local cardpoint = {cc.p(720, 682), cc.p(165, 245), cc.p(165+265, 245), cc.p(165+265*2, 245), cc.p(165+265*3,245)}
local cardpoint = {cc.p(670, 678), cc.p(264, 227), cc.p(264+233, 227), cc.p(264+233*2, 227), cc.p(264+233*3,227)}
--翻牌位置
local showCardOrder = {cc.p(165, 245), cc.p(165+265, 245), cc.p(165+265*2, 245), cc.p(165+265*3,245), cc.p(700, 682)}
--自己头像位置
local selfheadpoint = cc.p(63, 57)
--庄家头像位置
-- local bankerheadpoint = cc.p(445, 670)
local bankerheadpoint = cc.p(463+45, 640+45)
--玩家列表按钮位置
-- local userlistpoint = cc.p(55, 698)
local userlistpoint = cc.p(1281, 47)

local BANK_BTN_NUM = {0,0,0,0,0}


function GameViewLayer:ctor(scene)
	--注册node事件
    ExternalFun.registerNodeEvent(self)

	self._scene = scene

    --初始化
    self:paramInit()

	--加载资源
	self:loadResource()

    ExternalFun.playBackgroudAudio("ingameBGMMono.mp3")

    self._ActionMgr = ActionMgr:create()
end

function GameViewLayer:paramInit()
    --用户列表
    self:getDataMgr():initUserList(self:getParentNode():getUserList())

    --是否显示菜单层
    self.m_bshowMenu = false

    --菜单栏
    self.m_menulayout = nil

    --庄家名称
    self.m_bankerName = nil

    --自己背景框
    self.m_selfbg = nil

    --下注筹码
    self.m_JettonBtn = {}

    --下注按钮背后光
    self.m_JettonLight = nil

    --选中筹码
    self.m_nJettonSelect = 0

    --自己区域下注分
    self.m_lUserJettonScore = {}

    --上一局自己区域下注分数
    self.m_lastRoundJettonScore = {0,0,0,0,0}

    --自己下注总分
    self.m_lUserAllJetton = 0

    --玩家区域总下注分
    self.m_lAllJettonScore = {}
    --下注区域
    self.m_JettonArea = {}

    --自己下注分数背景
    self.m_selfJettonBG = {}
    --总下注分数文字
    self.m_tAllJettonScore = {}

    --牌显示层
    self.m_cardLayer = nil

    --游戏币显示层
    self.m_goldLayer = nil

    --游戏币列表
    self.m_goldList = {{}, {}, {}, {}, {}}

    --玩家列表层
    self.m_userListLayer = nil

    --上庄列表层
    self.m_applyListLayer = nil

    --游戏银行层
    self.m_bankLayer = nil

    --路单层
    self.m_GameRecordLayer = nil

    --游戏结算层
    self.m_gameResultLayer = nil

    --倒计时Layout
    self.m_timeLayout = nil

    --当前庄家
    self.m_wBankerUser = yl.INVALID_CHAIR
    --当前庄家分数
    self.m_lBankerScore = 0

    --系统能否做庄
    self.m_bEnableSysBanker = false

    --游戏状态
    self.m_cbGameStatus = Game_CMD.GAME_SCENE_FREE
    --剩余时间
    self.m_cbTimeLeave = 0

    --dump(self:getMeUserItem(),"=====================>")
    --显示分数
    self.m_showScore = self:getMeUserItem().lScore or 0

    --最大下注
    self.m_lUserMaxScore = 0

    --申请条件
    self.m_lApplyBankerCondition = 0

    --区域限制
    self.m_lAreaLimitScore = 0

    --桌面扑克数据
    self.m_cbTableCardArray = {}
    --桌面扑克
    self.m_CardArray = {}
    --桌面牛点数显示
    self.m_niuPointLayout = {}
    --赢了闪光效果
    -- self.m_winEffect = {}
    --输了的透明层
    -- self.m_failedEffect = {}

    --区域输赢
    self.m_bUserOxCard = {}
    --区域输赢倍数
    self.m_Multiple = {}
    --牛数值
    self.m_niuPoint = {}

    --是否练习房，练习房不能使用银行
    self.m_bGenreEducate = false

    --自己占位
    self.m_nSelfSitIdx = nil

    --用户坐下配置
    self.m_tabSitDownConfig = {}
    self.m_tabSitDownUser = {}
    --自己坐下
    self.m_nSelfSitIdx = nil

    --座位
    self.m_TableSeat = {}

    --游戏结算数据
    --坐下玩家赢分
    self.m_lOccupySeatUserWinScore = nil

    --本局赢分
    self.m_lSelfWinScore = 0

    --庄家赢分
    self.m_lBankerWinScore = 0
    --庄家昵称
    self.m_tBankerName = ""

    --超级抢庄按钮
    self.m_btSupperRob = nil
    --申请状态
    self.m_enApplyState = APPLY_STATE.kCancelState
    --超级抢庄申请
    self.m_bSupperRobApplyed = false
    --超级抢庄配置
    self.m_tabSupperRobConfig = {}
    --游戏币抢庄提示
    self.m_bRobAlert = false
    --当前抢庄用户
    self.m_wCurrentRobApply = yl.INVALID_CHAIR

    --是否播放游戏币飞入音效
    self.m_bPlayGoldFlyIn = true
    --下注倒计时
    self.m_fJettonTime = 0.1

    --是否重复下注
    self.isReject = false

    --不可以点击的下注按钮
    self.unUseJetton = {true, true, true, true, true, true, true}

    --动画管理节点
    self._animateUi = nil

    --是否断线重连
    self.isRelink = false

    self.startGameAnimate = nil
end

function GameViewLayer:loadResource()
    cc.SpriteFrameCache:getInstance():addSpriteFrames("game_res.plist", "game_res.png")

    --加载卡牌纹理
    cc.Director:getInstance():getTextureCache():addImage("im_card.png")

    local rootLayer, csbNode = ExternalFun.loadRootCSB("GameSceneVer2.csb", self)
	self.m_rootLayer = rootLayer
    self.m_scbNode = csbNode

	local function btnEvent( sender, eventType )
        if TAG_ENUM.BT_JETTONAREA_0 == sender:getTag() then
    end

        self:onButtonClickedEvent(sender:getTag(), sender)
    end

    cc.funAddChildListenerWithTag(csbNode, "bt_menu", TAG_ENUM.BT_MENU, btnEvent)
    cc.funAddChildListener(csbNode, "Panel_bg", function() self.m_bshowMenu = true self:showMenu() end)
    self.m_menulayout = csbNode:getChildByName("Panel_top_right")
    self.m_menulayout:setScale(1.0, 0.01)
    self.m_menulayout:setVisible(false)
    
    local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
    csbNode:getChildByName("Panel_top_right"):getChildByName("Button_bank"):setVisible(enableBank)

    cc.funAddChildListenerWithTag(csbNode, "bt_ludan", TAG_ENUM.BT_LUDAN, btnEvent)
    cc.funAddChildListenerWithTag(csbNode, "Panel_top_right/Button_bank", TAG_ENUM.BT_BANK, btnEvent)
    cc.funAddChildListenerWithTag(csbNode, "Panel_top_right/Button_help", TAG_ENUM.BT_HELP, btnEvent)
    cc.funAddChildListenerWithTag(csbNode, "Panel_top_right/Button_set", TAG_ENUM.BT_SET, btnEvent)
    cc.funAddChildListenerWithTag(csbNode, "Sprite_top_left/Button_quit", TAG_ENUM.BT_QUIT, btnEvent)

    cc.funAddChildListenerWithTag(csbNode, "bt_left", TAG_ENUM.BT_LEFT, btnEvent)
    cc.funAddChildListenerWithTag(csbNode, "bt_right", TAG_ENUM.BT_RIGHT, btnEvent)
    cc.funAddChildListenerWithTag(csbNode, "bt_supperrob", TAG_ENUM.BT_SUPPERROB, btnEvent)
    cc.funAddChildListenerWithTag(csbNode, "bt_apply", TAG_ENUM.BT_APPLY, btnEvent)
    cc.funAddChildListenerWithTag(csbNode, "bt_userlist", TAG_ENUM.BT_USERLIST, btnEvent)

    self.m_btRejetton = cc.funAddChildListenerWithTag(csbNode, "bt_rejet", TAG_ENUM.BT_REJECT, btnEvent)
    self.m_timeLayout = csbNode:getChildByName("layout_time")--倒计时
    self.m_timeLayout:setVisible(false)
    self._animateUi = cc.Node:create()--:addTo(self, ZORDER_LAYER.ZORDER_Other_Layer)
    -- self:addToRootLayer(self._animateUi, ZORDER_LAYER.ZORDER_Other_Layer-1)
    self:addToRootLayer(self._animateUi, ZORDER_LAYER.ZORDER_CARD_Layer)

    self.m_selfbg = cc.find(csbNode, "Panel_1/Sprite_self")
    self.m_spGameTrumpetBg = cc.find(csbNode, "Sprite_trump")
    cc.find(csbNode, "Sprite_trump/Panel_trump/text_trump"):setVisible(false)

    self.m_spGameTrumpetBg:setVisible(false)
    self.m_spGameTrumpetBg:setScale(0.00001)
    --下注区域
    for i=1,4 do
        local str = string.format("Node_area/bt_area_%d", i-1)
        -- btn = cc.find(csbNode, str)
        -- btn:setTag(TAG_ENUM.BT_JETTONAREA_0+i-1)
        -- btn:addTouchEventListener(btnEvent)

        local btn = cc.funAddChildListenerWithTag(csbNode, str, TAG_ENUM.BT_JETTONAREA_0+i-1, btnEvent)
        self.m_JettonArea[i] = btn

        local Score = btn:getChildByName("txt_score")
        self.m_tAllJettonScore[i] = Score
        Score:setVisible(false)

        local myScore = btn:getChildByName("txt_Myscore")
        self.m_selfJettonBG[i] = myScore
        myScore:setVisible(false)
    end

    self._seatPositions = {}
    --座位
    for i=1,6 do
        local str = string.format("Node_sitdowns/bt_seat_%d", i)
        -- btn = cc.find(csbNode, str)
        -- btn:setTag(TAG_ENUM.BT_SEAT_0+i-1)
        -- btn:addTouchEventListener(btnEvent)

        local btn = cc.funAddChildListenerWithTag(csbNode, str, TAG_ENUM.BT_SEAT_0+i-1, btnEvent)
        self.m_TableSeat[i] = btn

        local x,y = btn:getPosition()
        table.insert(self._seatPositions, {x=x,y=y})
    end

    --自己下注分数
    -- for i=1,4 do
    --     local str = string.format("im_jetton_score_bg_%d", i-1)
    --     btn = csbNode:getChildByName(str)
    --     self.m_selfJettonBG[i] = btn
    --     btn:setVisible(false)
    -- end

    self:initBankerInfo()
    self:initSelfInfo()

    --牌类层
    self.m_cardLayer = cc.Layer:create()
    self:addToRootLayer(self.m_cardLayer, ZORDER_LAYER.ZORDER_CARD_Layer)
    for i=1,5 do
        --赢的效果
        -- if i >= 2 then
            -- self.m_winEffect[i] = ccui.ImageView:create("im_effect_1.png", UI_TEX_TYPE_PLIST)
            -- self.m_winEffect[i]:setPosition(cardpoint[i].x+105, cardpoint[i].y)
            -- self.m_winEffect[i]:setVisible(false)
            -- self.m_cardLayer:addChild(self.m_winEffect[i])
        -- end
        local temp = {}
        for j=1,5 do
            temp[j] = CardSprite:createCard(0)
            temp[j]:setVisible(false)
            temp[j]:setAnchorPoint(0, 0.5)
            --TAG_CARD_BEGIN+i- whichOne*5, For ActionMgr's playing cards actions, distigus all cards
            --Don't change the card's tag
            temp[j]:setTag(ActionMgr.TAG_CARD_BEGIN+j-i*5)
            self.m_cardLayer:addChild(temp[j])
        end
        self.m_CardArray[i] = temp
        --输的效果
        -- if i >= 2 then
            -- self.m_failedEffect[i] = ccui.ImageView:create("im_effect_0.png", UI_TEX_TYPE_PLIST)
            -- self.m_failedEffect[i]:setPosition(cardpoint[i].x+105, cardpoint[i].y)
            -- self.m_failedEffect[i]:setVisible(false)
            -- self.m_cardLayer:addChild(self.m_failedEffect[i])
        -- end

        self.m_niuPointLayout[i] = ccui.ImageView:create("game_res_v2_plist/im_point_win_bg.png", 0)
        -- self.m_niuPointLayout[i]:setTag(TAG_ENUM.TAG_CARD+10) --why set the tag confused with cards
        self.m_niuPointLayout[i]:setPosition(cardpoint[i].x+58, cardpoint[i].y)
        self.m_niuPointLayout[i]:setVisible(false)
        self.m_cardLayer:addChild(self.m_niuPointLayout[i])
    end

    --游戏币层
    self.m_goldLayer = cc.Layer:create()
    self:addToRootLayer(self.m_goldLayer, ZORDER_LAYER.ZORDER_JETTON_GOLD_Layer)

    -- if device.platform == "android" or device.platform == "ios" then
        self:onScheduleSingal()
        self._schSingal = schedule:scheduleScriptFunc(function() self:onScheduleSingal() end, 5, false)
    -- end

    self.labelTime = cc.find(csbNode, "Sprite_top_left/Text_time")
    self.labelTime:setString(os.date("%H:%M"))
    local function timeUpdate()
        self.labelTime:setString(os.date("%H:%M"))
    end
    self._timeShd = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 1, false)


    -- cc.SpriteFrameCache:getInstance():addSpriteFrames("brnn_start.plist")
    -- AnimationMgr.loadAnimationFromFrame("brnn_start%d.png", 1, 10, GAME_START_ANIMATION_NAME, 1)

    -------------------------------------------------------------------------------------------
    self._listView = cc.TableView:create(cc.size(730, 150))
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)
    self._listView:setPosition(cc.p(197+130,29-40))
    self._listView:setDelegate()
    self._listView:addTo(self.m_scbNode )
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(self.numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
end

function GameViewLayer:loadAnimation()
    -- cc.SpriteFrameCache:getInstance():addSpriteFrames("brnn_start.plist")

    -- --缓存表情
    -- local dict = cc.FileUtils:getInstance():getValueMapFromFile("brnn_start.plist")
    -- local framesDict = dict["frames"]
    -- if nil ~= framesDict and type(framesDict) == "table" then
    --     local framesNum = #framesDict
    --     local animation = cc.Animation:create()
    --     local times = 0.15
    --     for i=1,framesNum do
    --         local strName = string.format("brnn_start%d.png", i)
    --         local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
    --         if spriteFrame then
    --             animation:addSpriteFrame(spriteFrame)
    --         else
    --             break
    --         end
    --         animation:setLoops(-1)
    --         animation:setDelayPerUnit(times)
    --     end

    --     local animate = cc.Animate:create(animation)
    --     local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame("brnn_start1.png")
    --     local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
    --     return spr, animate
    -- end
    -- return nil

    local animation = cc.Animation:create()
    local name
    for i = 1, 10 do
        name = string.format("brnn_start/brnn_start%d.png", i)
        animation:addSpriteFrameWithFile(name)
    end
    animation:setDelayPerUnit(0.1)
    animation:setRestoreOriginalFrame(true)
    animation:setLoops(3)

    local action = cc.Animate:create(animation)
    local sp = cc.Sprite:create("brnn_start/brnn_start1.png")
    return sp,action
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

    local panel_battery = cc.find(self.m_scbNode, "Sprite_top_left/Panel_2")
    local node_wifi = cc.find(self.m_scbNode, "Sprite_top_left/Node_wifi")
    local node_4G = cc.find(self.m_scbNode, "Sprite_top_left/Node_4g")
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
        panel_battery:getChildByName("Sprite_power_full"):setColor(cc.c3b(0xff,0,0))
    else
        panel_battery:getChildByName("Sprite_power_full"):setColor(cc.c3b(0xff,0xff,0xff))
    end
end

function GameViewLayer:createHeadSprite(src)
    maskFile = "game_res_v2_plist/img_face_mask.png"
    -- local src = display.newSprite(srcFile)
    local mask = display.newSprite(maskFile)

    local size_src = src:getContentSize()
    local size_mask = mask:getContentSize()

    local canva = cc.RenderTexture:create(size_src.width, size_src.height, cc.TEXTURE2_D_PIXEL_FORMAT_RGB_A8888)

    local ratiow =  size_mask.width/size_src.width
    local ratioh = size_mask.height / size_src.height
    src:setScaleX(ratiow)
    src:setScaleY(ratioh)

    mask:setPosition(size_src.width / 2, size_src.height / 2)
    src:setPosition(size_src.width / 2, size_src.height / 2)

    local blendfunc_mask = cc.blendFunc(gl.ONE, gl.ZERO)
    mask:setBlendFunc(blendfunc_mask)
    local blendfunc_src = cc.blendFunc(gl.DST_ALPHA, gl.ZERO)
    src:setBlendFunc(blendfunc_src)

    canva:begin()
    mask:visit()
    src:visit()
    canva:endToLua()

    local masked_sprite = cc.Sprite:createWithTexture(canva:getSprite():getTexture())
    masked_sprite:setFlippedY(true)
    -- masked_sprite:runAction(cc.RotateTo:create( 0.001, 180))
    return masked_sprite
end

function GameViewLayer:tableCellTouched(view, cell)
    self.m_nJettonSelect = cell:getIdx() + 1
    self._listView:reloadData()
end

function GameViewLayer.cellSizeForTable()
        return 145, 147
end

function GameViewLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()

    local item = nil

    local str1 = string.format("game/chip_%d.png", idx+1)
    local str2 = string.format("game/jetton_Diss.png", idx+1)

    if nil == cell then
        cell = cc.TableViewCell:new()
        item = ccui.Button:create(str1, str1, str2)
        item:setSwallowTouches(false)
        item:setAnchorPoint(cc.p(0,0))
        item:setPosition(cc.p(0,0))
        item:setName("btn")
        local str = self.m_JettonList[idx+1]
        self:addLabelAtlasWithParent(item, str)
        -- if str >= 10000 then
        --     str = str / 10000 .."万"
        -- end
        -- print("str ===",str)
        -- local tmp = cc.LabelTTF:create(str, "fonts/round_body.ttf", 26)
        -- tmp:setPosition(75,80)
        -- tmp:addTo(item)
        cell:addChild(item)
    else
        cell:removeAllChildren()
        item = ccui.Button:create(str1, str1, str2)

        item:setSwallowTouches(false)
        item:setAnchorPoint(cc.p(0,0))
        item:setPosition(cc.p(0,0))
        local str = self.m_JettonList[idx+1]

        self:addLabelAtlasWithParent(item, str)
        -- if str >= 10000 then
        --     str = str / 10000 .."万"
        -- end
        -- print("str ===",str)
        -- local tmp = cc.LabelTTF:create(str, "fonts/round_body.ttf", 26)
        -- tmp:setPosition(75,80)
        -- tmp:addTo(item)
        cell:addChild(item)
    end

        --如果是庄家
    if self:isMeChair(self.m_wBankerUser) then
        item:setEnabled(false)
    elseif self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_JETTON then
        --非下注状态
        item:setEnabled(false)
    end

    for i=1, 7 do
        if not self.unUseJetton[idx+1] then
            item:setEnabled(false)
        end
    end

    --选中按钮
    if (idx + 1) == self.m_nJettonSelect and item:isEnabled() then
        local effect = cc.Sprite:create("game/jettonbg.png")
        effect:setAnchorPoint(cc.p(0,0))
        effect:setPosition(cc.p(-8,2))
        effect:setName("im_jetton_effect")
        effect:setScale(0.8)
        item:addChild(effect)
        effect:runAction(cc.RepeatForever:create(cc.Blink:create(1.0,1)))
    end

    return cell
end

function GameViewLayer.numberOfCellsInTableView()
    return 7
end

------------------------------------------------------------------------------------------------------

function GameViewLayer:addLabelAtlasWithParent(parent, str)
    local lab = cc.LabelAtlas:create("","game/atlas.png",18, 27, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2 + 5)
    local num = str
    lab:setString(num)
    if num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("game/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 + 5)
    end
end

function GameViewLayer:getFormatString(score)
    local scorestr = ExternalFun.formatScore(score)
    if score < 10000 then
        return scorestr
    end

    if score < 100000000 then
        scorestr = string.format("%.2f万", score / 10000)
        return scorestr
    end
    scorestr = string.format("%.2f亿", score / 100000000)
    return scorestr
end


--初始化庄家信息
function GameViewLayer:initBankerInfo()
    local temp = cc.find(self.m_scbNode, "Panel_zhuang/Sprite_23")

    local pbankername = ClipText:createClipText(cc.size(100, 26), "无人坐庄");
    pbankername:setAnchorPoint(temp:getAnchorPoint())
    pbankername:setName("TEXT_ZHUANG")
    pbankername:setPosition(temp:getPosition())

    temp:getParent():addChild(pbankername)
    temp:removeFromParent()

    self.m_bankerName = pbankername
    self.m_bankerScore = cc.find(self.m_scbNode, "Panel_zhuang/Text_gold")
end

--刷新庄家信息
function GameViewLayer:resetBankerInfo()
     if self:isMeChair(self.m_wBankerUser) then
          --显示上庄成功提示
        local index = 2
        local function upDate ()
           index = index -1
           if index == 0 then
            showToast(self, "上庄成功", 1)
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timer1)
           end
        end
        self.timer1 = cc.Director:getInstance():getScheduler():scheduleScriptFunc(upDate, 1, false)
    end

    local spFace = cc.find(self.m_scbNode, "Panel_zhuang/Sprite_zhuang")
    spFace:removeAllChildren()

    --被替换的节点
    -- local temp = cc.find(self.m_scbNode, "Panel_zhuang/TEXT_ZHUANG")

    local phead = nil
    if self.m_wBankerUser == yl.INVALID_CHAIR then
        phead = HeadSprite:createClipHead(nil, 100, "game_res_v2_plist/img_face_mask.png")
        phead:setTag(TAG_ENUM.HEAD_BANKER)

        if self.m_bEnableSysBanker == false then
            self.m_bankerName:setString("无人坐庄")
            self.m_bankerScore:setString("")
            -- temp:setString("-------")
        else
            self.m_bankerName:setString("系统坐庄")
            local bankerstr = ExternalFun.formatScoreText(self.m_lBankerScore)
            self.m_bankerScore:setString(bankerstr)
            -- temp:setString(bankerstr)
        end
    else
        local userItem = self:getDataMgr():getChairUserList()[self.m_wBankerUser+1]
            dump(userItem,"resetBankerInfo")
        if nil ~= userItem then
            self.m_bankerName:setString(userItem.szNickName)
            print("m_lBankerScore",self.m_lBankerScore, debug.traceback())
            local bankerstr = ExternalFun.formatScoreText(self.m_lBankerScore)
            self.m_bankerScore:setString(bankerstr)

            phead = HeadSprite:createClipHead(userItem, 100, "game_res_v2_plist/img_face_mask.png")
            if phead then
                phead:setTag(TAG_ENUM.HEAD_BANKER)
            end
        end
    end

    if phead ~= nil then
        -- self.m_scbNode:getChildByName("im_banker_bg"):getChildByName("Button_banker"):setEnabled(true)
        --[[
        local headImg = self:createHeadSprite(phead)
        local spriteFrame = headImg:getSpriteFrame()

        local aniNode = cc.CSLoader:createNode("animate/ruzuo.csb"):addTo(self.m_scbNode)
        aniNode:getChildByName("Sprite_2"):setSpriteFrame(spriteFrame):setFlippedY(true)

        local ani = cc.CSLoader:createTimeline("animate/ruzuo.csb")
        ani:gotoFrameAndPlay(0,false)
        aniNode:runAction(ani)
        ]]

        local spFace = cc.find(self.m_scbNode, "Panel_zhuang/Sprite_zhuang")
        phead:setPosition(44, 44)
        spFace:addChild(phead)

        local emitter = cc.ParticleSystemQuad:create("battle_particle/frame.plist")
        local tex2d = cc.Director:getInstance():getTextureCache():addImage("battle_particle/frame.png")
        emitter:setTexture(tex2d)
        emitter:setPosition(44,44)
        emitter:setAutoRemoveOnFinish(true)
        spFace:addChild(emitter)
    end
end

--刷新庄家分数
function GameViewLayer:resetBankerScore()
    local userItem = self:getDataMgr():getChairUserList()[self.m_wBankerUser+1]
    if nil ~= userItem then
        self.m_bankerName:setString(userItem.szNickName)
        local bankerstr = ExternalFun.formatScoreText(self.m_lBankerScore)
        self.m_bankerScore:setString(bankerstr)
    end
end

--初始化自己信息
function GameViewLayer:initSelfInfo()
    local temp = PopupInfoHead:createClipHead(self:getMeUserItem(), 100, "game_res_v2_plist/img_face_mask.png")
    temp:setPosition(44, 46)
    self.m_selfbg:addChild(temp)
    temp:enableInfoPop(true)
    local param = {}
    param.isProp = false     --显示道具互动，百人游戏不显示
    param.imgPath = "game_res/popBg.png" --弹出框背景
    temp:setPropInfo(param)
    -- temp:setPopCsbFile("game/popInfo/GamePopInfo.csb") -- 自定义CSB

    local temp = cc.find(self.m_scbNode, "Panel_1/Text_nick")
    local pselfname = ClipText:createClipText(cc.size(145, 26), self:getMeUserItem().szNickName);
    pselfname:setAnchorPoint(temp:getAnchorPoint())
    pselfname:setPosition(temp:getPosition())
    pselfname:setName(temp:getName())
    temp:removeFromParent()
    self.m_selfbg:addChild(pselfname)

    local text_score = cc.find(self.m_scbNode, "Panel_1/Text_gold")
    text_score:setString(ExternalFun.formatScoreText(self.m_showScore))
end

--刷新自己信息
function GameViewLayer:resetSelfInfo()
    local txt_score = cc.find(self.m_scbNode, "Panel_1/Text_gold")
    txt_score:setString(self:getFormatString(self.m_showScore))
end

--开始下一局，清空上局数据
function GameViewLayer:resetGameData()
    if nil ~= self.m_cardLayer then
        self.m_cardLayer:stopAllActions()
    end

    for i=1,5 do
        if self.m_niuPointLayout[i] ~= nil then
            self.m_niuPointLayout[i]:removeAllChildren()
            self.m_niuPointLayout[i]:setVisible(false)
        end
        -- if self.m_winEffect[i] ~= nil then
        --     self.m_winEffect[i]:stopAllActions()
        --     self.m_winEffect[i]:setVisible(false)
        -- end
        -- if self.m_failedEffect[i] ~= nil then
        --     self.m_failedEffect[i]:setVisible(false)
        -- end
        if self.m_CardArray[i] ~= nil then
            for k,v in pairs(self.m_CardArray[i]) do
                v:stopAllActions()
                v:setVisible(false)
            end
        end
    end
    self.m_lAllJettonScore = {0,0,0,0,0}
    self.m_lUserJettonScore = {0,0,0,0,0}
    self.m_lUserAllJetton = 0
    self:updateAreaScore(false)
    self:showGameWait(false)

    --清空坐下用户下注分数
    for i=1,Game_CMD.MAX_OCCUPY_SEAT_COUNT do
        if nil ~= self.m_tabSitDownUser[i] then
            self.m_tabSitDownUser[i]:clearJettonScore()
        end
    end

    --清除开始下注动画
    if self.gameStart then
        self.gameStart:removeFromParent()
        self.gameStart = nil
    end

    --游戏币清除
    self.m_goldLayer:removeAllChildren()
    self.m_goldList = {{}, {}, {}, {}, {}}
    -- self.m_goldLayer:retain()
    -- self.m_goldLayer:removeFromParent()
    -- self:addToRootLayer(self.m_goldLayer, ZORDER_LAYER.ZORDER_JETTON_GOLD_Layer)
    -- self.m_goldLayer:release()

    self.m_bPlayGoldFlyIn = true
end

function GameViewLayer:onExit()
    self._animateUi:removeAllChildren()
    self:stopAllActions()
    self:unloadResource()

    local schedule = cc.Director:getInstance():getScheduler()
    if self._timeShd then
        schedule:unscheduleScriptEntry(self._timeShd)
        self._timeShd = nil
    end
    if self._schSingal then
        schedule:unscheduleScriptEntry(self._schSingal)
        self._schSingal = nil
    end
    if self.timer then
        schedule:unscheduleScriptEntry(self.timer)
        self.timer = nil
    end

    self._ActionMgr:destroy()
end

--释放资源
function GameViewLayer:unloadResource()
    --特殊处理game_res blank.png 冲突
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game_res.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("game_res.png")

    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game_bank.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("game_bank.png")

    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("win_effect.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("win_effect.png")

    cc.Director:getInstance():getTextureCache():removeTextureForKey("im_card.png")
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    --播放大厅背景音乐
    ExternalFun.playPlazzBackgroudAudio()

    self:getDataMgr():removeAllUser()
    self:getDataMgr():clearRecord()

    if nil ~= self.m_gameResultLayer then
        self.m_gameResultLayer:clear()
    end

    if nil ~= self.pTimer100 then
        self.pTimer100:removeFromParent()
    end
end

function GameViewLayer:onButtonClickedEvent(tag, ref)
	ExternalFun.playClickEffect()
	if TAG_ENUM.BT_MENU == tag then
        -- local idx = 1

        -- local node = self.m_TableSeat[idx]
        -- local node2 = self.m_tabSitDownUser[idx]

        -- ActionMgr:bettingAction(node, 100, self._seatPositions[idx].x, self._seatPositions[idx].y)
        -- ActionMgr:bettingAction(node2, 100, self._seatPositions[idx].x, self._seatPositions[idx].y)

		self:showMenu()
    elseif TAG_ENUM.BT_LUDAN == tag then
        if nil == self.m_GameRecordLayer then
            self.m_GameRecordLayer = GameRecordLayer:create(self)
            self:addToRootLayer(self.m_GameRecordLayer, ZORDER_LAYER.ZORDER_Other_Layer)
        end
        local recordList = self:getDataMgr():getGameRecord()
        self.m_GameRecordLayer:refreshRecord(recordList)
    elseif TAG_ENUM.BT_BANK == tag then
        self:showMenu()
        if self.m_bGenreEducate == true then
            showToast(self, "练习模式，不能使用银行", 1.5)
            return
        end
        if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
            showToast(self, "初次使用，请先开通银行！", 1)
            return
        end


        --空闲状态才能存款
        if nil == self.m_bankLayer then
            self:createBankLayer()
        end
        self.m_bankLayer:setVisible(true)
        self:getParentNode():sendRequestBankInfo()
        self:refreshBankScore()
    elseif TAG_ENUM.BT_CLOSEBANK == tag  then
        if nil ~= self.m_bankLayer then
            self.m_bankLayer:setVisible(false)
        end
    elseif TAG_ENUM.BT_TAKESCORE == tag then
        self:onTakeScore()
    elseif TAG_ENUM.BT_SAVESCORE == tag then
        self:onSaveScore()
    elseif TAG_ENUM.BT_SET == tag then
        self:showMenu()
        local mgr = self._scene:getParentNode():getApp():getVersionMgr()
        local verstr = mgr:getResVersion(Game_CMD.KIND_ID) or "0"
        verstr = "游戏版本:" .. appdf.BASE_C_VERSION .. "." .. verstr
        local setting = SettingLayer:create(verstr)
        self:addToRootLayer(setting, ZORDER_LAYER.ZORDER_Other_Layer)
    elseif TAG_ENUM.BT_HELP == tag then
        self:showMenu()
        -- self._scene:getParentNode():popHelpLayer2(Game_CMD.KIND_ID, 0, 5)
        local layer = HelpLayer:create()
        self:addToRootLayer(layer, ZORDER_LAYER.ZORDER_Other_Layer)
    elseif TAG_ENUM.BT_QUIT == tag then
        -- self:showMenu()
        self._scene:onQueryExitGame()
        --self._scene:onExitRoom()
    --座位按钮
    elseif TAG_ENUM.BT_SEAT_0 <= tag and TAG_ENUM.BT_SEAT_5 >= tag then
        self:onSitDownClick(ref:getTag()-TAG_ENUM.BT_SEAT_0+1, ref)
    -- 下注区域
    elseif TAG_ENUM.BT_JETTONAREA_0 <= tag and  TAG_ENUM.BT_JETTONAREA_3 >= tag then
        self:onJettonAreaClicked(ref:getTag()-TAG_ENUM.BT_JETTONAREA_0+1, ref)
    elseif tag == TAG_ENUM.BT_USERLIST then
        if nil == self.m_userListLayer then
            self.m_userListLayer = UserListLayer:create()
            self:addToRootLayer(self.m_userListLayer, ZORDER_LAYER.ZORDER_Other_Layer)
        end
        local userList = self:getDataMgr():getUserList()
        self.m_userListLayer:refreshList(userList)
    elseif tag == TAG_ENUM.BT_APPLY then
        if nil == self.m_applyListLayer then
            self.m_applyListLayer = ApplyListLayer:create(self)
            self:addToRootLayer(self.m_applyListLayer, ZORDER_LAYER.ZORDER_Other_Layer)
        end
        local userList = self:getDataMgr():getApplyBankerUserList()
        self.m_applyListLayer:refreshList(userList)
    elseif tag == TAG_ENUM.BT_SUPPERROB then
        --超级抢庄
        if self.m_lApplyBankerCondition + self.m_tabSupperRobConfig.lSuperBankerConsume >  self.m_lUserMaxScore then
            showToast(self,"金币不足", 1)
            return
        end

        if self:getApplyState() >= GameViewLayer._apply_state.kApplyedState then
            showToast(self,"当前状态不支持", 1)
            return
        end
        dump(self.m_tabSupperRobConfig,"self.m_tabSupperRobConfig")
        if Game_CMD.SUPERBANKER_CONSUMETYPE == self.m_tabSupperRobConfig.superbankerType then
            SuperRobLayer:create(self, function(ok)
                if ok == true then
                    self:getParentNode():sendRobBanker()
                end
            end, self.m_tabSupperRobConfig.lSuperBankerConsume):addTo(self)

            -- local str = "超级抢庄将花费 " .. self.m_tabSupperRobConfig.lSuperBankerConsume .. ",确定抢庄?"
            -- local query = QueryDialog:create(str, function(ok)
            --     if ok == true then
            --         self:getParentNode():sendRobBanker()
            --     end
            -- end):setCanTouchOutside(false)
            --     :addTo(self)
        else
            self:getParentNode():sendRobBanker()
        end
    elseif tag == TAG_ENUM.BT_REJECT then
        self:showRejecton()
    elseif tag == TAG_ENUM.BT_LEFT or tag == TAG_ENUM.BT_RIGHT then
        self:changeTableView(tag)
    else
        showToast(self,"功能尚未开放！",1)
	end
end

--通过按钮滑动tableview
function GameViewLayer:changeTableView(tag)
    if tag == TAG_ENUM.BT_LEFT then
        local xOffset = self._listView:getContentOffset().x + 145

        if xOffset >= 0 then
            self._listView:setContentOffset(cc.p(0,0),true)
        else
            self._listView:setContentOffset(cc.p(xOffset,0),true)
        end
    elseif tag == TAG_ENUM.BT_RIGHT then
        local xOffset = self._listView:getContentOffset().x - 145
        if xOffset>-290 then
            self._listView:setContentOffset(cc.p(xOffset,0),true)
        else
            self._listView:setContentOffset(cc.p(-290,0),true)
        end
    end
end

--重复下注
function GameViewLayer:showRejecton()
    self:getParentNode():sendRejectton()
end

--设置重复下注按钮状态
function GameViewLayer:changeRejettonState()
    local score = 0

    for i=2, #self.m_lastRoundJettonScore do
        score = score + self.m_lastRoundJettonScore[i]
    end

    --非下注状态
    if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_JETTON then
        self.m_btRejetton:setEnabled(false)
        return
    end

    --上一局未下注
    if score == 0 then
        self.m_btRejetton:setEnabled(false)
        return
    end

    --自己是庄家
    if self:isMeChair(self.m_wBankerUser) == true then
        self.m_btRejetton:setEnabled(false)
        return
    end

    --携带金额不足时
    if self.m_showScore < score*10 then
        self.m_btRejetton:setEnabled(false)
        return
    end

    if self.isReject then
        self.m_btRejetton:setEnabled(false)
        return
    end

    self.m_btRejetton:setEnabled(true)
end

--下注区域
function GameViewLayer:onJettonAreaClicked(tag, ref)
    --非下注状态
    if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_JETTON or self.m_nJettonSelect == 0 then
        return
    end

    if self:isMeChair(self.m_wBankerUser) == true then
        return
    end

    if self.m_bEnableSysBanker == 0 and self.m_wBankerUser == yl.INVALID_CHAIR then
        showToast(self,"无人坐庄，不能下注",1)
        return
    end

    local jettonscore = self.m_JettonList[self.m_nJettonSelect]
    --
    local selfscore  = (jettonscore + self.m_lUserAllJetton)*MaxTimes
    if  selfscore > self.m_lUserMaxScore then
        showToast(self,"已超过个人最大下注值",1)
        return
    end

    local areascore = self.m_lAllJettonScore[tag+1] + jettonscore
    if areascore > self.m_lAreaLimitScore then
        showToast(self,"已超过该区域最大下注值",1)
        return
    end

    if self.m_wBankerUser ~= yl.INVALID_CHAIR then
        local allscore = jettonscore
        for k,v in pairs(self.m_lAllJettonScore) do
            allscore = allscore + v
        end
        allscore = allscore*MaxTimes
        if allscore > self.m_lBankerScore then
            showToast(self,"总下注已超过庄家下注上限",1)
            return
        end
    end

    self.m_lUserAllJetton = self.m_lUserAllJetton + jettonscore
    self:updateJettonList(self.m_lUserMaxScore - self.m_lUserAllJetton*MaxTimes)
    local userself = self:getMeUserItem()
    self:getParentNode():SendPlaceJetton(jettonscore, tag)
end

function GameViewLayer:onSitDownClick( tag, sender )
    print("sit ==> " .. tag)
    local useritem = self:getMeUserItem()
    if nil == useritem then
        return
    end

    --重复判断
    if nil ~= self.m_nSelfSitIdx and tag == self.m_nSelfSitIdx then
        return
    end

    if nil ~= self.m_nSelfSitIdx then --and tag ~= self.m_nSelfSitIdx  then
        showToast(self, "当前已占 " .. self.m_nSelfSitIdx .. " 号位置,不能重复占位!", 1)
        return
    end

    if nil ~= self.m_tabSitDownUser[tag] then
        showToast(self, self.m_tabSitDownUser[tag]:getNickName().."已经捷足先登了!", 1)
        return
    end

    if useritem.lScore < self.m_tabSitDownConfig.lForceStandUpCondition then
        local str = "坐下需要携带 " .. self.m_tabSitDownConfig.lForceStandUpCondition .. "游戏币,游戏币不足!"
        showToast(self, str, 2)
        return
    end

    --坐下条件限制
    if self.m_tabSitDownConfig.occupyseatType == Game_CMD.OCCUPYSEAT_CONSUMETYPE then --游戏币占座
        if useritem.lScore < self.m_tabSitDownConfig.lOccupySeatConsume then
            local str = "坐下需要消耗 " .. self.m_tabSitDownConfig.lOccupySeatConsume .. " 游戏币,游戏币不足!"
            showToast(self, str, 1)
            return
        end
        local str = "坐下将花费 " .. self.m_tabSitDownConfig.lOccupySeatConsume .. ",确定坐下?"
            local query = QueryDialog:create(str, function(ok)
                if ok == true then
                    self:getParentNode():sendSitDown(tag - 1, useritem.wChairID)
                end
            end):setCanTouchOutside(false)
                :addTo(self)
    elseif self.m_tabSitDownConfig.occupyseatType == Game_CMD.OCCUPYSEAT_VIPTYPE then --会员占座
        if useritem.cbMemberOrder < self.m_tabSitDownConfig.enVipIndex then
            local str = "坐下需要会员等级为 " .. self.m_tabSitDownConfig.enVipIndex .. " 会员等级不足!"
            showToast(self, str, 1)
            return
        end
        self:getParentNode():sendSitDown(tag - 1, self:getMeUserItem().wChairID)
    elseif self.m_tabSitDownConfig.occupyseatType == Game_CMD.OCCUPYSEAT_FREETYPE then --免费占座
        if useritem.lScore < self.m_tabSitDownConfig.lOccupySeatFree then
            local str = "免费坐下需要携带游戏币大于 " .. self.m_tabSitDownConfig.lOccupySeatFree .. " ,当前携带游戏币不足!"
            showToast(self, str, 1)
            return
        end
        self:getParentNode():sendSitDown(tag - 1, self:getMeUserItem().wChairID)
    end
end

function GameViewLayer:OnUpdataClockView(chair, time)
    local selfchair = self:getParent():GetMeChairID()
    if chair == self:getParentNode():SwitchViewChairID(selfchair) then
        local temp = self.m_timeLayout:getChildByName("txt_time")
        temp:setString(string.format("%02d", time))

        --停止下注
        if not self._obj_stop_bet and time <= 1 and self.m_timeLayout:isVisible() and  self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_FREE then
            local obj = SpineObj:create("animate/sp_stop_bet/skeleton.json", "animate/sp_stop_bet/skeleton.atlas", false)
            obj:setCompleteCallback(function() obj:destroy() end)
            obj:setPosition(display.width*0.5, display.height*0.5+50)
            self:addChild(obj)
            self._obj_stop_bet = obj
        end

        if self.m_cbGameStatus == Game_CMD.GAME_SCENE_JETTON then
            self.m_bPlayGoldFlyIn = true
            self.m_fJettonTime = math.min(0.1, time)
        end
    else

        self._obj_stop_bet = nil

        if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_END then
            return
        end
        if time == self.m_cbTimeLeave then
            print("*******************开始发牌",time,self.m_cbTimeLeave)

            for i=1,5 do
                if self.m_CardArray[i] ~= nil then
                    for k,v in pairs(self.m_CardArray[i]) do
                        v:setVisible(false)
                    end
                end
            end
            self.m_cardLayer:setVisible(true)
            --发牌处理
            self:sendCard(true)
        elseif time == self.m_cbTimeLeave-4  then
            --显示牌值
            self:showCard(true)
        elseif time == self.m_cbTimeLeave - 6 then
            --显示点数
            self:showCardPoint()
        elseif time == self.m_cbTimeLeave-15 then
            --游戏币处理
            self:showGoldMove()
        end
    end

  --空闲状态
    if self.m_cbGameStatus == Game_CMD.GAME_SCENE_FREE then
        -- self.m_timeLayout:setVisible(false)
        -- if self.isRelink then
        --     self.isRelink = false
        --     if time >= 4 then
        --             -- if time == 4 then
        --             --     --self:showGameWait(false)
        --             -- else
        --                 self.m_timeLayout:setVisible(false)
        --                 self:showGameWait(true)
        --             --end
        --     --else
        --         --self:showGameStart()
        --     end
        -- else
            local temp = self.m_timeLayout:getChildByName("txt_time")
            -------------------没负号
            if time >= 0 then
                temp:setString(string.format("%02d", time))
            end
            -------------------没负号
            -- if time == 4 then
            --     self.m_timeLayout:setVisible(false)
            --      --开始动画
            --     self:showGameStart()
            -- end
        --end
    end

    --下注状态
    if self.m_cbGameStatus == Game_CMD.GAME_SCENE_JETTON and self.isRelink then
        self.isRelink = false
        -- if time > 5 then
        --     self:showGameStart()
        -- end
    end
end

--获取数据
function GameViewLayer:getParentNode()
    return self._scene
end

function GameViewLayer:getMeUserItem()
    if nil ~= GlobalUserItem.tabAccountInfo.dwUserID then
        return self:getDataMgr():getUidUserList()[GlobalUserItem.tabAccountInfo.dwUserID]
    end
    return nil
end

function GameViewLayer:isMeChair( wchair )
    local useritem = self:getDataMgr():getChairUserList()[wchair + 1]
    if nil == useritem then
        return false
    else
        return useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID
    end
end

function GameViewLayer:addToRootLayer( node , zorder)
    if nil == node then
        return
    end

    self.m_rootLayer:addChild(node)
    node:setLocalZOrder(zorder)
end

function GameViewLayer:getChildFromRootLayer( tag )
    if nil == tag then
        return nil
    end
    return self.m_rootLayer:getChildByTag(tag)
end

function GameViewLayer:getDataMgr( )
    return self:getParentNode():getDataMgr()
end

function GameViewLayer:getApplyState(  )
    return self.m_enApplyState
end

function GameViewLayer:getApplyCondition(  )
    return self.m_llCondition
end

--获取能否上庄
function GameViewLayer:getApplyable(  )
    --自己超级抢庄已申请，则不可进行普通申请
    if APPLY_STATE.kSupperApplyed == self.m_enApplyState then
        return false
    end

    local userItem = self:getMeUserItem()
    if nil ~= userItem then
        return userItem.lScore > self.m_llCondition
    else
        return false
    end
end

--获取能否取消上庄
function GameViewLayer:getCancelable(  )
    return self.m_cbGameStatus == Game_CMD.GAME_SCENE_FREE
end

function GameViewLayer:logData(msg)
    local p = self:getParentNode()
    if nil ~= p.logData then
        p:logData(msg)
    end
end

function GameViewLayer:showPopWait( )
    self:getParentNode():showPopWait()
end

function GameViewLayer:dismissPopWait( )
    self:getParentNode():dismissPopWait()
end

----网络消息处理-------
function GameViewLayer:onGameSceneFree(cmd_table)
    --玩家分数
    self.m_lUserMaxScore = cmd_table.lUserMaxScore
    self.m_lApplyBankerCondition = cmd_table.lApplyBankerCondition
    self.m_JettonList = cmd_table.lChipArray[1]
    dump(self.m_JettonList)
    self.m_cbTimeLeave = cmd_table.cbTimeLeave
    self.m_wBankerUser = cmd_table.wBankerUser
    self.m_lBankerScore = cmd_table.lBankerScore
    self.m_bEnableSysBanker = cmd_table.bEnableSysbanker
    self.m_lAreaLimitScore = cmd_table.lAreaLimitScore
    self.m_bGenreEducate = cmd_table.bGenreEducate
    self:resetBankerInfo()
    self:resetSelfInfo()
    self.m_cbGameStatus = Game_CMD.GAME_SCENE_FREE
    self:showGameStatus()
    self.m_lAllJettonScore = {0,0,0,0,0}
    self.m_lUserJettonScore = {0,0,0,0,0}
    self.m_lUserAllJetton = 0
    --获取到占位信息
    self:onGetSitDownInfo(cmd_table.occupyseatConfig, cmd_table.wOccupySeatChairID[1])
    --抢庄条件
    self:onGetApplyBankerCondition(cmd_table.lApplyBankerCondition, cmd_table.superbankerConfig)
    self:setJettonEnable(true)
    self:updateAreaScore(false)
    self:updateJettonList(self.m_lUserMaxScore)

    --更改重复下注状态
    self:changeRejettonState()
end

function GameViewLayer:onGameScenePlaying(cmd_table, cbGameStatus)
    --玩家分数
    print("*******************************onGameScenePlaying*************************************")
    dump(cmd_table)
    self.m_cardLayer:setVisible(false)
    self:resetGameData()
    self.m_JettonList = cmd_table.lChipArray[1]
    dump(self.m_JettonList,"self.m_JettonList")
    self.m_lUserMaxScore = cmd_table.lUserMaxScore
    self.m_lApplyBankerCondition = cmd_table.lApplyBankerCondition
    self.m_cbTimeLeave = cmd_table.cbTimeLeave
    self.m_wBankerUser = cmd_table.wBankerUser
    self.m_lBankerScore = cmd_table.lBankerScore
    self.m_bEnableSysBanker = cmd_table.bEnableSysbanker
    self.m_lAreaLimitScore = cmd_table.lAreaLimitScore
    self.m_bGenreEducate = cmd_table.bGenreEducate
    self:resetBankerInfo()
    self:resetSelfInfo()
    self.m_cbGameStatus = cbGameStatus
    self.m_lAllJettonScore = cmd_table.lAllJettonScore[1]
    self.m_lUserJettonScore = cmd_table.lUserJettonScore[1]
    self.m_lBankerWinScore = cmd_table.lBankerWinScore
    self.isRelink = true

    --更改重复下注状态
    self:changeRejettonState()

    local bankername = "系统"
    if  self.m_wBankerUser ~= yl.INVALID_CHAIR then
        local useritem = self:getDataMgr():getChairUserList()[self.m_wBankerUser + 1]
        if nil ~= useritem then
            bankername = useritem.szNickName
        end
    end
    self.m_tBankerName = bankername

    self.m_lSelfWinScore = cmd_table.lEndUserScore
    for k,v in pairs(self.m_lUserJettonScore) do
        self.m_lUserAllJetton = self.m_lUserAllJetton + v
    end

    self.m_cbTableCardArray = cmd_table.cbTableCardArray
    if cmd_table.cbGameStatus == Game_CMD.GAME_SCENE_JETTON then
        self.m_cbGameStatus = Game_CMD.GAME_SCENE_JETTON
        self:getParent():SetGameClock(self:getParent():GetMeChairID(), 1, cmd_table.cbTimeLeave)
        self:setJettonEnable(true)
        self:updateJettonList(self.m_lUserMaxScore - self.m_lUserAllJetton*MaxTimes)
        self:updateAreaScore(true)

        --多于两秒播放下注动画
        -- if cmd_table.cbTimeLeave > 2 then
        --      --开始动画
        --     self:showGameStart()
        -- end
    else
        self:setJettonEnable(false)
        --自己是否下注
        local jettonscore = 0
        for k,v in pairs(cmd_table.lUserJettonScore[1]) do
            jettonscore = jettonscore + v
        end
        --自己是否有输赢
        jettonscore = jettonscore + self.m_lSelfWinScore
        if self.m_cbGameStatus == Game_CMD.GAME_SCENE_END then
            self:showGameWait(true)
        else
            -- if self.m_cbGameStatus == Game_CMD.GAME_SCENE_FREE then
                -- if self.m_cbTimeLeave < 3 then
                --     self:showGameStart()
                -- else
                    self:showGameWait(true)
                --end
            -- else
                --结束界面显示请等待下一局开始动画
                -- self.nextRound = cc.Sprite:create("game/notice.png"):addTo(self.m_scbNode):setPosition(display.cx, display.cy)
                -- self:showGameWait()
            -- end
        end
    end
    self:showGameStatus()
    --获取到占位信息
    self:onGetSitDownInfo(cmd_table.occupyseatConfig, cmd_table.wOccupySeatChairID[1])
    --抢庄条件
    self:onGetApplyBankerCondition(cmd_table.lApplyBankerCondition, cmd_table.superbankerConfig)

end

--空闲
function GameViewLayer:onGameFree(cmd_table)
    self.isReject = false
    self.isRelink = false
    self.m_cbGameStatus = Game_CMD.GAME_SCENE_FREE
    self.m_cbTimeLeave = cmd_table.cbTimeLeave
    self:showGameStatus()

    self.m_lAllJettonScore = {0,0,0,0,0}
    self.m_lUserJettonScore = {0,0,0,0,0}
    -- self:resetGameData()
    self:setJettonEnable(false)
    self:updateJettonList(self.m_lUserMaxScore)
    --上庄条件刷新
    self:refreshCondition()

    --申请按钮状态更新
    self:refreshApplyBtnState()

    if nil ~= self.m_gameResultLayer then
        -- self.m_gameResultLayer:setVisible(false)
        self.m_gameResultLayer:hide()
    end

    --移除等待下一局界面
    self:showGameWait(false)

    self:resetGameData()

    if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() and self:isMeChair(self.m_wBankerUser) then
        showToast(self,"现在是空闲时间，可以进行银行操作",1)
    end
    -- self:collectgarbageAll()
end

--内存收集
function GameViewLayer:collectgarbageAll()
    local c1 = collectgarbage("count")
    print(c1)
end

--开始下注
function GameViewLayer:onGameStart(cmd_table)
    self._listView:reloadData()
    self.m_cbGameStatus = Game_CMD.GAME_SCENE_JETTON
    self.m_cbTimeLeave = cmd_table.cbTimeLeave
    self.m_lUserMaxScore = cmd_table.lUserMaxScore
    self.m_wBankerUser = cmd_table.wBankerUser
    self.m_lBankerScore = cmd_table.lBankerScore
    self:showGameStatus()
    self:setJettonEnable(true)
    self:resetBankerScore()
    self:updateJettonList(self.m_lUserMaxScore)
    if self:isMeChair(self.m_wBankerUser) == true then
        self:setJettonEnable(false)
    end

    self:resetBankerScore()
    --申请按钮状态更新
    self:refreshApplyBtnState()

    --更改重复下注状态
    self:changeRejettonState()
end

--结束
function GameViewLayer:onGameEnd(cmd_table)
    self.m_cbGameStatus = Game_CMD.GAME_SCENE_END
    self.m_cbTableCardArray = cmd_table.cbTableCardArray
    self.m_lSelfWinScore = cmd_table.lUserScore
    self.m_lBankerWinScore = cmd_table.lBankerScore
    self.m_lOccupySeatUserWinScore = cmd_table.lOccupySeatUserWinScore

    local bankername = "系统"
    if  self.m_wBankerUser ~= yl.INVALID_CHAIR then
        local useritem = self:getDataMgr():getChairUserList()[self.m_wBankerUser + 1]
        if nil ~= useritem then
            bankername = useritem.szNickName
        end
    end
    self.m_tBankerName = bankername

    self:showGameStatus()
    self:setJettonEnable(false)

    --保存上一局自己下注信息
    self.m_lastRoundJettonScore = self.m_lUserJettonScore or {0,0,0,0,0}
    --更改重复下注状态
    self:changeRejettonState()
end

--用户下注
function GameViewLayer:onPlaceJetton(cmd_table)
    -- dump(cmd_table, "#用户下注===================================")
    local isme = self:isMeChair(cmd_table.wChairID)

    if isme == true then
        local oldscore = self.m_lUserJettonScore[cmd_table.cbJettonArea+1]
        self.m_lUserJettonScore[cmd_table.cbJettonArea+1] = oldscore + cmd_table.lJettonScore

        self.m_showScore = self.m_showScore - cmd_table.lJettonScore
        self:resetSelfInfo()
        self._scene.m_cbGameStatus = Game_CMD.GAME_SCENE_JETTON
    end

    local oldscore = self.m_lAllJettonScore[cmd_table.cbJettonArea+1]
    self.m_lAllJettonScore[cmd_table.cbJettonArea+1] = oldscore + cmd_table.lJettonScore

    self:showUserJetton(cmd_table)
    self:updateAreaScore(true)

    --更改重复下注状态
    self:changeRejettonState()

    ------------------------------下注座位的动画
    local seatIdx,node_user = self:getSitDownUserBySeatId(cmd_table.wChairID)
    local node_seat = self.m_TableSeat[seatIdx]
    if node_user and not isme then
        local offsetx = 100
        if seatIdx >= 3 then offsetx = -100 end
        ActionMgr:bettingAction(node_seat, offsetx, self._seatPositions[seatIdx].x, self._seatPositions[seatIdx].y)
        ActionMgr:bettingAction(node_user, offsetx, self._seatPositions[seatIdx].x, self._seatPositions[seatIdx].y)
    else
        -- print("位子上没有玩家========================================", idx)
    end
    ------------------------------下注座位的动画
end

function GameViewLayer:getSitDownUserBySeatId(seatId)
    if self.m_tabSitDownUser then
        for i,user in pairs(self.m_tabSitDownUser) do
            if user.m_wChair and user.m_wChair == seatId then
                return i,self.m_tabSitDownUser[i]
            end
        end
    end
end

--下注失败
function GameViewLayer:onPlaceJettonFail(cmd_table)
    if self:isMeChair(cmd_table.wPlaceUser) == true then
        self.m_lUserAllJetton = self.m_lUserAllJetton - cmd_table.lPlaceScore
    end
end

--重复下注
function GameViewLayer:onReJetton(cmd_table)
    --dump(cmd_table, "#重复下注=====")
    self.isReject = true
    local tmp = {}
    local jettonscore = 0
    tmp.wChairID = cmd_table.wChairID

    for i=1, 4 do
        tmp.cbJettonArea = i
        tmp.lJettonScore = cmd_table.lJettonScore[1][i+1]

        if self:isMeChair(tmp.wChairID) then
            local oldscore = self.m_lUserJettonScore[tmp.cbJettonArea+1]
            self.m_lUserJettonScore[tmp.cbJettonArea+1] = oldscore + tmp.lJettonScore
            jettonscore = jettonscore + tmp.lJettonScore
            --更改重复下注状态
            self:changeRejettonState()
        end

        local oldscore = self.m_lAllJettonScore[tmp.cbJettonArea+1]
        self.m_lAllJettonScore[tmp.cbJettonArea+1] = oldscore + tmp.lJettonScore

        if tmp.lJettonScore > 0 then
            self:showUserJetton(tmp)
        end
        self:updateAreaScore(true)

    end

    if self:isMeChair(tmp.wChairID) then
        self.m_showScore = self.m_showScore - jettonscore
        self:resetSelfInfo()
    end
end

--重复下注失败
function GameViewLayer:onReJettonFail(cmd_table)

end

--提前开牌
function GameViewLayer:onAdvanceOpenCard()
    showToast(self, "下注已超上限，提前开牌", 1)
end

--申请上庄
function GameViewLayer:onApplyBanker( cmd_table)
    if self:isMeChair(cmd_table.wApplyUser) == true then
        self.m_enApplyState = APPLY_STATE.kApplyState
    end

    self:refreshApplyList()
end

--切换庄家
function GameViewLayer:onChangeBanker(cmd_table)
    dump(cmd_table, "#切换庄家=====")

    --上一个庄家是自己，且当前庄家不是自己，标记自己的状态
    if self.m_wBankerUser ~= wBankerUser and self:isMeChair(self.m_wBankerUser) then
        self.m_enApplyState = APPLY_STATE.kCancelState
    end
    self.m_wBankerUser = cmd_table.wBankerUser
    self.m_lBankerScore = cmd_table.lBankerScore
    self:resetBankerInfo()

    --自己上庄
    if self:isMeChair(cmd_table.wBankerUser) == true then
        self.m_enApplyState = APPLY_STATE.kApplyedState
    end

    --如果是超级抢庄用户上庄
    if cmd_table.wBankerUser == self.m_wCurrentRobApply then
        self.m_wCurrentRobApply = yl.INVALID_CHAIR
        self:refreshCondition()
    end

    --坐下用户庄家
    local chair = -1
    for i = 1, Game_CMD.MAX_OCCUPY_SEAT_COUNT do
        if nil ~= self.m_tabSitDownUser[i] then
            chair = self.m_tabSitDownUser[i]:getChair()
            self.m_tabSitDownUser[i]:updateBanker(chair == cmd_table.wBankerUser)
        end
    end
end

--取消申请
function GameViewLayer:onGetCancelBanker(cmd_table)
    if self:isMeChair(cmd_table.wCancelUser) == true then
        self.m_enApplyState = APPLY_STATE.kCancelState
    end

    --庄家申请下庄，下庄动画
    -- if self.m_wBankerUser == cmd_table.wCancelUser then
    --     self.m_scbNode:getChildByName("im_banker_bg"):getChildByName("Button_banker"):setEnabled(false)
    --     local spriteFrame = cc.Sprite:create("game/banker_headframe1.png"):getSpriteFrame()

    --     local aniNode = cc.CSLoader:createNode("animate/xiazhuang.csb"):addTo(self)
    --     aniNode:getChildByName("Sprite_12"):setSpriteFrame(spriteFrame)
    --     local ani = cc.CSLoader:createTimeline("animate/xiazhuang.csb")
    --     ani:gotoFrameAndPlay(0,false)
    --     aniNode:runAction(ani)
    -- end

    self:refreshApplyList()
end

--开始动画
function GameViewLayer:showGameStart()
    local obj = SpineObj:create("animate/sp_start_round/skeleton.json", "animate/sp_start_round/skeleton.atlas", false)
    obj:setCompleteCallback(function() obj:destroy() end)
    obj:setPosition(display.width*0.5, display.height*0.5+50)
    -- self:addChild(obj)
    self._animateUi:addChild(obj)

    self._spineObjGameStart = obj

    ExternalFun.playSoundEffect("gamestart.mp3")
end

--等待下一局动画
function GameViewLayer:showGameWait(bool)
    if bool then
        if self.startGameAnimate then
            self.startGameAnimate:setVisible(bool)
        else
            self.startGameAnimate = cc.CSLoader:createNode("animate/gamestart/startgame.csb"):addTo(self._animateUi):setPosition(display.cx+115, display.cy)
            local ani = cc.CSLoader:createTimeline("animate/gamestart/startgame.csb")
            ani:gotoFrameAndPlay(0,true)
            self.startGameAnimate:runAction(ani)
            ExternalFun.playSoundEffect("gamestart.mp3")
        end
    else
        if self.startGameAnimate then
            self.startGameAnimate:setVisible(false)
        end
    end
end

--抢庄条件
function GameViewLayer:onGetApplyBankerCondition( llCon , rob_config)
    self.m_llCondition = llCon
    --超级抢庄配置
    self.m_tabSupperRobConfig = rob_config
    dump(self.m_tabSupperRobConfig,"self.m_tabSupperRobConfig")

    self:refreshCondition()
end

--超级抢庄申请
function GameViewLayer:onGetSupperRobApply(  )
    if yl.INVALID_CHAIR ~= self.m_wCurrentRobApply then
        self.m_bSupperRobApplyed = true
        ExternalFun.enableBtn(self.m_btSupperRob, false)
    end
    --如果是自己
    if true == self:isMeChair(self.m_wCurrentRobApply) then
        --普通上庄申请不可用
        self.m_enApplyState = APPLY_STATE.kSupperApplyed
    end
end

--超级抢庄用户离开
function GameViewLayer:onGetSupperRobLeave( wLeave )
    if yl.INVALID_CHAIR == self.m_wCurrentRobApply then
        --普通上庄申请不可用
        self.m_bSupperRobApplyed = false

        ExternalFun.enableBtn(self.m_btSupperRob, true)
    end
end

--座位坐下信息
function GameViewLayer:onGetSitDownInfo( config, info )
    self.m_tabSitDownConfig = config

    local pos = cc.p(0,0)
    --获取已占位信息
    for i = 1, Game_CMD.MAX_OCCUPY_SEAT_COUNT do
        print("sit chair " .. info[i])
        self:onGetSitDown(i - 1, info[i], false)
    end
end

--座位坐下
function GameViewLayer:onGetSitDown( index, wchair, bAni )
    if wchair ~= nil
        and nil ~= index
        and index ~= Game_CMD.SEAT_INVALID_INDEX
        and wchair ~= yl.INVALID_CHAIR then
        local useritem = self:getDataMgr():getChairUserList()[wchair + 1]

        if nil ~= useritem then
            --下标加1
            index = index + 1
            if nil == self.m_tabSitDownUser[index] then
                self.m_tabSitDownUser[index] = SitRoleNode:create(self, index)
                self.m_tabSitDownUser[index]:setPosition(self.m_TableSeat[index]:getPosition())

                cc.find(self.m_scbNode, "Node_all_seats"):addChild(self.m_tabSitDownUser[index])
                -- self:addToRootLayer(self.m_tabSitDownUser[index], 1)
            end
            self.m_tabSitDownUser[index]:onSitDown(useritem, bAni, wchair == self.m_wBankerUser)

            if useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
                self.m_nSelfSitIdx = index
            end
        end
    end
end

--座位失败/离开
function GameViewLayer:onGetSitDownLeave( index )
    if index ~= Game_CMD.SEAT_INVALID_INDEX
        and nil ~= index then
        index = index + 1
        if nil ~= self.m_tabSitDownUser[index] then
            self.m_tabSitDownUser[index]:removeFromParent()
            self.m_tabSitDownUser[index] = nil
        end

        if self.m_nSelfSitIdx == index then
            self.m_nSelfSitIdx = nil
        end
    end
end

--银行操作成功
function GameViewLayer:onBankSuccess( )
    local bank_success = self:getParentNode().bank_success
    if nil == bank_success then
        return
    end
    GlobalUserItem.tabAccountInfo.lUserScore = bank_success.lUserScore
    GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure

    if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
        self:refreshBankScore()
    end

    showToast(self, bank_success.szDescribrString, 2)
end

--银行操作失败
function GameViewLayer:onBankFailure( )
    local bank_fail = self:getParentNode().bank_fail
    if nil == bank_fail then
        return
    end

    showToast(self, bank_fail.szDescribeString, 2)
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
    dump(bankinfo,"bankinfo",11)
    bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
    BANK_BTN_NUM = bankinfo.lBankOPButtonArray[1]
    if nil ~= self.m_bankLayer then
        local str = "温馨提示:取款将扣除" .. bankinfo.wRevenueTake .. "‰的手续费"
        self.m_bankLayer.m_textTips:setString(str)
        for i = 1, 3 do
            self:addLabelAtlas(self.bankBtns[i], BANK_BTN_NUM[i])
        end
    end
end

-------界面显示更新--------
--菜单栏操作
function GameViewLayer:showMenu()
    if self.m_bshowMenu == false then
        self.m_bshowMenu = true
        self.m_menulayout:setVisible(true)
        self.m_menulayout:runAction(cc.ScaleTo:create(0.2, 1.0))
    else
    -- dump(TAG_ENUM.BT_MENU, "隐藏菜单层", 6)
        self.m_bshowMenu = false
        self.m_menulayout:runAction(cc.Sequence:create(
            cc.ScaleTo:create(0.2, 1.0, 0.0001),
            cc.CallFunc:create(
                function()
                    self.m_menulayout:setVisible(false)
                end
                )
            )
        )
    end
end

--更新游戏状态显示
function GameViewLayer:showGameStatus()
    if self.m_cbGameStatus == Game_CMD.GAME_SCENE_END then
        self.m_timeLayout:setVisible(false)
    else
        self.m_timeLayout:setVisible(true)
        local content = self.m_timeLayout:getChildByName("im_txt")
        local time = self.m_timeLayout:getChildByName("txt_time")
        time:setString(string.format("%02d", self.m_cbTimeLeave))
        if self.m_cbGameStatus == Game_CMD.GAME_SCENE_FREE then --休息一下
            print("休息一下==============================")
            content:loadTexture("game_res_v2_plist/img_reset.png", 0)
        elseif self.m_cbGameStatus == Game_CMD.GAME_SCENE_JETTON then --请下注
            print("请下注==============================")
            content:loadTexture("game_res_v2_plist/img_raise.png", 0)
            self.m_timeLayout:setVisible(true)

            if self._spineObjGameStart then
                self._spineObjGameStart:destroy()
                self._spineObjGameStart = nil
            end

            local obj = SpineObj:create("animate/sp_start_bet/skeleton.json", "animate/sp_start_bet/skeleton.atlas", false)
            obj:setCompleteCallback(function() obj:destroy() end)
            obj:setPosition(display.width*0.5, display.height*0.5+50)
            self:addChild(obj)
        end
    end
end

--发牌动画
function GameViewLayer:sendCard(banim)
    -- if banim then
    --     local delaytime = 0.1
    --     for i=1,5 do
    --         for j=1,5 do
    --             local card = self.m_CardArray[i][j]
    --             local index = (i-1)*5 + j - 1
    --             card:setPosition(652, 514)
    --             card:runAction(cc.Sequence:create(cc.DelayTime:create(delaytime*index), cc.CallFunc:create(
    --                 function()
    --                     if j == 1 then
    --                         ExternalFun.playSoundEffect("send_card.wav")
    --                     end
    --                     card:setOpacity(0)
    --                     card:setVisible(true)
    --                     card:setVisible(false)
    --                     card:runAction(cc.FadeTo:create(0.04, 255))
    --                     card:runAction(cc.Sequence:create(cc.MoveTo:create(0.33, cardpoint[i]),
    --                         cc.MoveBy:create(0.04*(j-1), cc.p(32*(j-1),0))))
    --                 end
    --             )))
    --         end
    --     end
    -- else
    --     for i=1,5 do
    --         for j=1,5 do
    --             local card = self.m_CardArray[i][j]
    --             card:setVisible(true)
    --             -- card:setPosition(cardpoint[i].x + (j-1)*32, cardpoint[i].y)
    --             card:setPosition(cardpoint[i].x + (j-1)*27, cardpoint[i].y)
    --         end
    --     end
    -- end

    -- local dstPosList = {}
    -- for i=1,5 do
    --     local pos = cardpoint[i]
    --     pos = cc.p(pos.x+(i-1)*32, pos.y)
    --     table.insert(dstPosList, pos)
    -- end


    self._ActionMgr:createSendCardAction(self.m_CardArray, nil, self.m_cardLayer)
end

--显示牌跟牌值
function GameViewLayer:showCard(banim)
    --切换游戏币层层级显示
    self.m_goldLayer:retain()
    self.m_goldLayer:removeFromParent()
    self:addToRootLayer(self.m_goldLayer, ZORDER_LAYER.ZORDER_GOLD_Layer)
    self.m_goldLayer:release()

    self.m_niuPoint[5] = GameLogic:GetCardType(self.m_cbTableCardArray[5], 5)
--[[
    for i=1,5 do
        local function showFunction(value)
            for j=1,5 do
                local card = self.m_CardArray[value][j]
                local cardvalue = self.m_cbTableCardArray[value][j]
                card:setCardValue(cardvalue)
                card:showCardBack(false)
            end
            if pbglayout ~= nil then
                -- pbglayout:setVisible(true)
                pbglayout:setVisible(false)--换皮不要这个
            end
        end

        if banim then
            local index = 0
            local j = 0

            if i == 1 then
                index = 5
            else
                index = i - 1
            end

            self.m_cardLayer:runAction(cc.Sequence:create(cc.DelayTime:create((index-1)*1.2), cc.CallFunc:create(
                function ()
                    showFunction(i)
                end
                )))
        else
            showFunction(i)
        end
    end
    ]]

    ----------------------------------------------------排序
    for i=1, 5 do
        local niuvalue = GameLogic:GetCardType(self.m_cbTableCardArray[i], 5)
        self.m_niuPoint[i] = niuvalue
    end
    ----------------------------------------------------排序

    self._ActionMgr:playOpenAction(self.m_cardLayer, self.m_cbTableCardArray)
end

--显示点数以及播放声音
function GameViewLayer:showCardPoint()
    local tmp = {}
    local niuVl = {}

     --显示输赢动画
    local function showWinAnimate(index)
        --判断庄家输赢
        local bankWin = 0
        for i=2, 5 do
            bankWin = bankWin + self.m_bUserOxCard[i]
        end
        if index == 6 then
            for i=2, 5 do
                if self.m_bUserOxCard[i] == 1 then
                    -- self.m_winEffect[i]:setVisible(true)
                    -- self.m_winEffect[i]:runAction(cc.RepeatForever:create(cc.Blink:create(1.0,1)))
                else
                    -- self.m_failedEffect[i]:setVisible(true)
                end
            end
            --庄家输赢
            if bankWin == 3 then
                -- self.m_winEffect[1]:setVisible(true)
                -- self.m_winEffect[1]:runAction(cc.RepeatForever:create(cc.Blink:create(1.0,1)))
            elseif bankWin == -3 then
                -- self.m_failedEffect[1]:setVisible(true)
            end
        end
    end

    self.resultTable = {}
    for i=1, 5 do
        local niuvalue = GameLogic:GetCardType(self.m_cbTableCardArray[i], 5)
        self.m_niuPoint[i] = niuvalue
        local pbglayout = self.m_niuPointLayout[i]
        --------------------------------隐藏背景
        pbglayout:setOpacity(0)
        --------------------------------隐藏背景

        pbglayout:loadTexture("game_res_v2_plist/im_point_win_bg.png", 0)

        -- local ptemp = self:getNiuTxt("game_res/num_point.png", niuvalue)
        -- local box = ptemp:getBoundingBox()
        -- ptemp:setAnchorPoint(cc.p(0.5, 0.5))
        -- ptemp:setPosition(pbglayout:getContentSize().width/2-box.width*0.5, pbglayout:getContentSize().height/2)
        -- pbglayout:addChild(ptemp)

        local parentSize = pbglayout:getContentSize()

        local bg = cc.Sprite:create("game_res/n_bg.png")
        local size = bg:getContentSize()
        bg:setScaleX(201/size.width)
        bg:setAnchorPoint(cc.p(0.5,0))
        bg:setPosition(size.width*0.5+29, 19)
        pbglayout:addChild(bg)

        local niuTypeCfg = NIU_TYPE[niuvalue]
        if not niuTypeCfg then
            error(string.format("牛值错误:%s", niuvalue or "nil"))
        end
        local spNiuType = cc.Sprite:create(niuTypeCfg.res)
        spNiuType:setPosition(parentSize.width*0.5, 32)
        pbglayout:addChild(spNiuType)

        -----------------------------------------------------------------
         if i > 1 then
            local a, b = GameLogic:CompareCard(self.m_cbTableCardArray[1], self.m_niuPoint[1], self.m_cbTableCardArray[i], self.m_niuPoint[i])
            if a == 0 then
                print("比牌错误", self.m_niuPoint[1], self.m_niuPoint[i])
                dump(self.m_cbTableCardArray[1])
                dump(self.m_cbTableCardArray[i])
                return
            end
            self.m_bUserOxCard[i], self.m_Multiple[i] = a, b
            -- if a == 1 then
            --     pbglayout:loadTexture("game_res_v2_plist/im_point_win_bg.png", 0)
            -- end

            local pwinscorebg = ccui.ImageView:create("game_res_v2_plist/im_jetton_score_bg.png", 0)
            pwinscorebg:setAnchorPoint(0.5, 1)
            pwinscorebg:setPosition(parentSize.width*0.5, 20)
            pbglayout:addChild(pwinscorebg)

            --------------------------------隐藏背景
            pwinscorebg:setOpacity(0)
            --------------------------------隐藏背景

            local pwinscore = ccui.Text:create("", "fonts/round_body.ttf", 22)
            pwinscore:setFontSize(22)
            pwinscore:setColor(cc.c3b(255, 255, 0))
            pwinscore:setPosition(pwinscorebg:getContentSize().width/2, pwinscorebg:getContentSize().height/2)
            pwinscorebg:addChild(pwinscore)
            pwinscore:setVisible(false)

            print(i.."=======================heheh")
            if self.m_lUserJettonScore[i] > 0 then
                local str = ExternalFun.formatScoreText(self.m_lUserJettonScore[i]*b)
                if a  == 1 then
                    str = "+"..str
                    pwinscore:setColor(cc.c3b(255, 255, 255))
                else
                    -- pwinscore:setColor(cc.c3b(30, 140, 210))
                    pwinscore:setColor(cc.c3b(255, 255, 255))
                    str = "-"..str
                end

                
                pwinscore:setString(str)
                self.resultTable[i] = pwinscore
                --pwinscore:setVisible(true)
            else
                pwinscore:setVisible(false)
                -- pwinscore:setString("没有下注")
                local sp = cc.Sprite:create("game_res_v2_plist/img_no_bet.png")
                sp:setAnchorPoint(pwinscore:getAnchorPoint())
                sp:setPosition(pwinscore:getPosition())
                pwinscorebg:addChild(sp)
            end
        end
        -------------------------------------------------------------------
        if i~= 1 then
            table.insert(tmp, pbglayout)
            table.insert(niuVl, self.m_niuPoint[i])
        end
    end

    table.insert(tmp, self.m_niuPointLayout[1])
    table.insert(niuVl, self.m_niuPoint[1])

    local index = 1
    local function upDate ()
        if index == 6 then
            -- showWinAnimate(index)
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timer)
            self.timer = nil
            return
        end
        if tmp[index] then
            tmp[index]:setVisible(true)
            local openIndex = index
            if openIndex +1 > 5 then
                openIndex = 0
            end
            if index == 5 then
                    self:showResult()
            end
            self._ActionMgr:openFiveCard(self.m_cardLayer, openIndex+1, self.m_cbTableCardArray[openIndex+1])
        end


        ExternalFun.playSoundEffect("open_card.wav")
        local soundValue = niuVl[index]-1
        soundValue = soundValue > 10 and 10 or soundValue
        ExternalFun.playSoundEffect("OX_"..soundValue.."_BANKER_SOUND.wav")
        ---------------------------------------------------------
        index = index + 1
    end
    self.timer = cc.Director:getInstance():getScheduler():scheduleScriptFunc(upDate, 1, false)
end


function GameViewLayer:showResult()
    for k,v in pairs(self.resultTable) do
        if v and tolua.cast(v, "cc.Node") then
            v:setVisible(true)
        end
    end
end


function GameViewLayer:createChip(chipNum)
    if chipNum <= 0  then
        return
    end
    local str = ""
    local idx = {1,2,3,4,5,6,7}
    for k, v in pairs(self.m_JettonList) do
        if v == chipNum then
            str = string.format("game/chip_%s.png", idx[k])
        end
    end

    local multi = chipNum / 10000

    local node = cc.CSLoader:createNode("Node_chip_small.csb")
    node:setScale(0.6)
    local chipNumStr
    if multi < 1 then
        chipNumStr = tostring(chipNum)
    else
        chipNumStr = multi.."万"
    end
    node:getChildByName("sp"):setTexture(str)
    local bmf = node:getChildByName("bmf")
    bmf:setString(chipNumStr)

    -- local strlen = string.len(tostring(multi))
    -- if strlen>=3 and multi>1 then
        -- bmf:setScale(1.6)
    -- elseif strlen>=3 then
    --     bmf:setScale(0.8)
    -- end
    return node
end

--显示用户下注
function GameViewLayer:showUserJetton(cmd_table)
    --如果是自己，游戏币从自己出飞出
    if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_JETTON then
        return
    end
    -- local goldnum = self:getGoldNum(cmd_table.lJettonScore)
    local beginpos = userlistpoint
    local offsettime = math.min(self.m_fJettonTime, 1)
    if self:isMeChair(cmd_table.wChairID) == true then
        beginpos = selfheadpoint
        ExternalFun.playSoundEffect("coins_fly_in.wav")
    else
        local seatUser = self:getIsUserSit(cmd_table.wChairID)
        --坐下玩家下注
        if seatUser ~= nil then
            local posindex = seatUser:getIndex()
            seatUser:addJettonScore(cmd_table.lJettonScore, cmd_table.cbJettonArea)
            beginpos = cc.p(self.m_TableSeat[posindex]:getPosition())
            ExternalFun.playSoundEffect("coins_fly_in.wav")
        --其他玩家下注
        else
            offsettime = math.min(self.m_fJettonTime, 3)
            if self.m_bPlayGoldFlyIn == true then
                ExternalFun.playSoundEffect("coins_fly_in.wav")
                self.m_bPlayGoldFlyIn = false
            end
        end
    end

    --需要计算金币的大小,数量
    local goldList = self:resovle(cmd_table.lJettonScore)
    for _,data in pairs(goldList) do
        local score,num = unpack(data)
        for i=1,num do
            local pgold = self:createChip(score)
            pgold:setPosition(beginpos)
            self.m_goldLayer:addChild(pgold)
            local moveaction = self:getMoveAction(beginpos, self:getRandPos(self.m_JettonArea[cmd_table.cbJettonArea]), 0, 0)
            pgold:runAction(moveaction)
            table.insert(self.m_goldList[cmd_table.cbJettonArea+1], pgold)
        end
    end
end

function GameViewLayer:resovle(score)
    --print("resovle=====", score)
    local goldList = {}
    local _500_wan,_100_wan,_10_wan,_1_wan,_1_qian,_1_bai
    if score >= self.m_JettonList[7] then
        _500_wan = math.floor(score / (self.m_JettonList[7]) )
        score = score - _500_wan * self.m_JettonList[7]
        table.insert(goldList, {self.m_JettonList[7], _500_wan})
    end

    if score >= self.m_JettonList[6] then
        _100_wan = math.floor(score / self.m_JettonList[6])
        score = score - _100_wan *self.m_JettonList[6]
        table.insert(goldList, {self.m_JettonList[6], _100_wan})
    end

    if score >= self.m_JettonList[5] then
        _10_wan = math.floor(score / self.m_JettonList[5])
        score = score - _10_wan *self.m_JettonList[5]
        table.insert(goldList, {self.m_JettonList[5], _10_wan})
    end

    if score >= self.m_JettonList[4] then
        _1_wan = math.floor(score / self.m_JettonList[4])
        score = score - _1_wan *self.m_JettonList[4]
        table.insert(goldList, {self.m_JettonList[4], _1_wan})
    end

    -- if score >= 50 then
    --     local num = math.floor(score / 50)
    --     score = score - num * 50
    --     table.insert(goldList, {50, num})
    -- end

    if score >= self.m_JettonList[3] then
        _1_qian = math.floor(score / self.m_JettonList[3])
        score = score - _1_qian * self.m_JettonList[3]
        table.insert(goldList, {self.m_JettonList[3], _1_qian})
    end

    if score >= self.m_JettonList[2] then
        local num = math.floor(score / self.m_JettonList[2])
        score = score - num * self.m_JettonList[2]
        table.insert(goldList, {self.m_JettonList[2], num})
    end
    if score >= self.m_JettonList[1] then
        _1_bai = math.floor(score / self.m_JettonList[1])
        score = score - _1_bai * self.m_JettonList[1]
        table.insert(goldList, {self.m_JettonList[1], _1_bai})
    end

    --dump(goldList, "#goldList==")
    return goldList
end

--结算游戏币处理
function GameViewLayer:showGoldMove()
    if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_END then
        return
    end
    local winAreaNum = 0
    local winScore = 0
    for i=1,Game_CMD.AREA_COUNT do
        --表示该区域庄家赢分
        if self.m_bUserOxCard[i+1] == -1 then
            winAreaNum = winAreaNum + 1
            winScore = winScore + self.m_lAllJettonScore[i+1]
            self:showGoldToZ(i)
        end
    end

    --庄家未赢钱
    if winScore == 0 then
       self:showGoldToArea()
    else
        self.m_goldLayer:runAction(cc.Sequence:create(cc.DelayTime:create(0.6), cc.CallFunc:create(
                function ()
                    self:showGoldToArea()
                end
            )))
    end

    local resultScore = self.m_lOccupySeatUserWinScore[1]
    for i,score in pairs(resultScore) do
        if score ~= 0 and self.m_tabSitDownUser[i] then
            if score > 0 then
                self.m_tabSitDownUser[i]:addWinEffect(score)
            else
                self.m_tabSitDownUser[i]:addLoseEffect(score)
            end
        end
    end
end

--显示游戏币飞到庄家处
function GameViewLayer:showGoldToZ(cbArea)
    local goldnum = #self.m_goldList[cbArea+1]
    if goldnum == 0 then
        return
    end
    ExternalFun.playSoundEffect("coinCollide.wav")
    --分十次飞行完成
    local cellnum = math.floor(goldnum/10)
    if cellnum == 0 then
        cellnum = 1
    end
    local cellindex = 0
    local outnum = 0
    for i=goldnum, 1, -1 do
        local pgold = self.m_goldList[cbArea+1][i]
        table.remove(self.m_goldList[cbArea+1], i)
        table.insert(self.m_goldList[1], pgold)
        outnum = outnum + 1
        local moveaction = self:getMoveAction(cc.p(pgold:getPosition()), bankerheadpoint, 1, 0)
        pgold:runAction(cc.Sequence:create(cc.DelayTime:create(cellindex*0.03), moveaction, cc.CallFunc:create(
                function ()
                    pgold:setVisible(false)
                end
            )))
        if outnum >= cellnum then
            cellindex = cellindex + 1
            outnum = 0
        end
    end
end

--显示游戏币庄家飞到下注区域
function GameViewLayer:showGoldToArea()
    if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_END then
        return
    end
    local failureScore = 0
    for i=1,Game_CMD.AREA_COUNT do
        --表示该区域庄家输分
        if self.m_bUserOxCard[i+1] == 1 then
            local lJettonScore = self.m_lAllJettonScore[i+1]
            if lJettonScore > 0 then
                self:showGoldToAreaWithID(i)
                failureScore = failureScore + lJettonScore
            end
        end
    end

    --庄家全赢
    if failureScore == 0 then
        --坐下用户
        for i = 1, Game_CMD.MAX_OCCUPY_SEAT_COUNT do
            if nil ~= self.m_tabSitDownUser[i] then
                local chair = self.m_tabSitDownUser[i]:getChair()
                local score = self.m_lOccupySeatUserWinScore[1][i]
                local useritem = self:getDataMgr():getChairUserList()[chair + 1]
                --游戏币动画
                self.m_tabSitDownUser[i]:gameEndScoreChange(useritem, score)
            end
        end
        self:showGameEnd()
    else
        self.m_goldLayer:runAction(cc.Sequence:create(cc.DelayTime:create(0.8), cc.CallFunc:create(
                function ()
                    self:showGoldToUser()
                end
            )))
    end
end

function GameViewLayer:getZhuangFixNum()
end

function GameViewLayer:showGoldToAreaWithID(cbArea)
    ExternalFun.playSoundEffect("coinCollide.wav")
    local goldnum = self:getWinGoldNumWithAreaID(cbArea)
    local listnum = #self.m_goldList[1]
    --当前列表游戏币数不足
    local addnum = 0
    if goldnum > listnum then
        addnum = goldnum - listnum
    end
    local fornum = math.min(goldnum, listnum)

    if fornum > 0 then
        for i=1,fornum do
            local pgold = self.m_goldList[1][listnum-i+1]
            table.remove(self.m_goldList[1], listnum-i+1)
            table.insert(self.m_goldList[cbArea + 1], pgold)
            pgold:setPosition(bankerheadpoint)
            pgold:stopAllActions()
            pgold:runAction(cc.Sequence:create(cc.DelayTime:create(0.01*i), cc.CallFunc:create(
                    function ()
                        local moveaction = self:getMoveAction(bankerheadpoint, self:getRandPos(self.m_JettonArea[cbArea]), 0, 1)
                        pgold:setVisible(true)
                        pgold:runAction(moveaction)
                    end
                )
            ))
        end
    end

    if addnum == 0 then
        return
    end

    local _500_wan,_100_wan,_10_wan,_1_wan,_1_qian,_1_bai
    local goldList = {}
    if addnum > 0 then
        local score = self.m_lAllJettonScore[cbArea + 1] - self.m_lUserJettonScore[cbArea+1]
        goldList = self:resovle(score)
    end

    local flyAction = function(score, idx)
        local pgold = self:createChip(score)
        if not pgold then return end

        pgold:setPosition(bankerheadpoint)
        pgold:setVisible(false)
        self.m_goldLayer:addChild(pgold)

        table.insert(self.m_goldList[cbArea + 1], pgold)
        pgold:runAction(cc.Sequence:create(cc.DelayTime:create(0.01*(idx+fornum)), cc.CallFunc:create(
                function ()
                    local moveaction = self:getMoveAction(bankerheadpoint, self:getRandPos(self.m_JettonArea[cbArea]), 0, 1)
                    pgold:setVisible(true)
                    pgold:runAction(moveaction)
                end
            )
        ))
    end

    for _,data in pairs(goldList) do
        local score,num = unpack(data)
        for i=1,num do
            flyAction(score, i)
        end
    end

--[[

    for i=1,addnum do
        -- local pgold = self:createChip(cmd_table.lJettonScore)
        local pgold = cc.Sprite:createWithSpriteFrameName("im_gold.png")
        pgold:setPosition(bankerheadpoint)
        pgold:setVisible(false)
        self.m_goldLayer:addChild(pgold)

        table.insert(self.m_goldList[cbArea + 1], pgold)
        pgold:runAction(cc.Sequence:create(cc.DelayTime:create(0.01*(i+fornum)), cc.CallFunc:create(
                function ()
                    local moveaction = self:getMoveAction(bankerheadpoint, self:getRandPos(self.m_JettonArea[cbArea]), 0, 1)
                    pgold:setVisible(true)
                    pgold:runAction(moveaction)
                end
            )
        ))
    end

    ]]
end

--显示游戏币下注区域飞到玩家
function GameViewLayer:showGoldToUser()
    if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_END then
        return
    end
    for i=1,Game_CMD.AREA_COUNT do
        --表示该区域庄家输分
        if self.m_bUserOxCard[i+1] == 1 then
            local lJettonScore = self.m_lAllJettonScore[i+1]
            if lJettonScore > 0 then
                self:showGoldAreaToUser(i)
            end
        end
    end

    self:runAction(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(
            function ()
                self:showGameEnd()
            end
        )))
end

function GameViewLayer:showGoldAreaToUser(cbArea)
    print("GameViewLayer:showGoldAreaToUser==================================", cbArea)
    ExternalFun.playSoundEffect("coinCollide.wav")
    local listnum = #self.m_goldList[cbArea + 1]
    local nmultiple = self.m_Multiple[cbArea + 1]
    local selfgoldnum = self:getWinGoldNum(self.m_lUserJettonScore[cbArea+1]*nmultiple)
    if self.m_lUserJettonScore[cbArea+1] == self.m_lAllJettonScore[cbArea+1] then
        selfgoldnum = listnum
    end
    --自己游戏币移动
    self:GoldMoveToUserDeal(cbArea, selfgoldnum, selfheadpoint)

    --坐下用户
    for i = 1, Game_CMD.MAX_OCCUPY_SEAT_COUNT do
        local score = self.m_lOccupySeatUserWinScore[1][i]
        if nil ~= self.m_tabSitDownUser[i] then
            local seatJettonScore = self.m_tabSitDownUser[i]:getJettonScoreWithArea(cbArea)
            if seatJettonScore > 0 then
                local seatGoldNum = self:getWinGoldNum(seatJettonScore*nmultiple)
                print("坐下用户游戏币数", seatGoldNum)
                local endpos = cc.p(self.m_TableSeat[i]:getPosition())
                self:GoldMoveToUserDeal(cbArea, seatGoldNum, endpos)

                local chair = self.m_tabSitDownUser[i]:getChair()
                local useritem = self:getDataMgr():getChairUserList()[chair + 1]
                --游戏币动画
                self.m_tabSitDownUser[i]:gameEndScoreChange(useritem, score)
            end
        end
    end

    listnum = #self.m_goldList[cbArea + 1]
    self:GoldMoveToUserDeal(cbArea, listnum, userlistpoint)
end

function GameViewLayer:GoldMoveToUserDeal(cbArea, goldNum, endpos)
    local listnum = #self.m_goldList[cbArea + 1]
    if goldNum > listnum then
        goldnum = listnum
    end
    if goldnum == 0 then
        return
    end
    for i=1,goldNum do
        local pgold = self.m_goldList[cbArea+1][listnum-i+1]
        if tolua.cast(pgold,"cc.Node") then
            table.remove(self.m_goldList[cbArea+1], listnum-i+1)
            pgold:runAction(cc.Sequence:create(cc.DelayTime:create(i*0.01), cc.CallFunc:create(
                    function ()
                        local moveaction = self:getMoveAction(cc.p(pgold:getPosition()), endpos, 1, 1)
                        pgold:runAction(cc.Sequence:create(moveaction, cc.CallFunc:create(
                                function ()
                                    pgold:removeFromParent()
                                end
                            )))
                    end
                )))
        end
    end
end

--显示游戏结算
function GameViewLayer:showGameEnd(bRecord)
    if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_END then
        return
    end
    if bRecord == nil then
        local record = Game_CMD.getEmptyGameRecord()
        record.bWinTianMen = self.m_bUserOxCard[2] == 1 and true or false
        record.bWinDiMen = self.m_bUserOxCard[3] == 1 and true or false
        record.bWinXuanMen = self.m_bUserOxCard[4] == 1 and true or false
        record.bWinHuangMen = self.m_bUserOxCard[5] == 1 and true or false

        if self.m_lUserJettonScore[2] > 0 then
            record.bMeJettonShunMen = true
        else
            record.bMeJettonShunMen = false
        end
        if self.m_lUserJettonScore[3] > 0 then
            record.bMeJettonDuiMen = true
        else
            record.bMeJettonDuiMen = false
        end
        if self.m_lUserJettonScore[4] > 0 then
            record.bMeJettonDaoMen = true
        else
            record.bMeJettonDaoMen = false
        end
        if self.m_lUserJettonScore[5] > 0 then
            record.bMeJettonHuang = true
        else
            record.bMeJettonHuang = false
        end

        self:getDataMgr():addGameRecord(record)
        self:refreshGameRecord()
    end

    --表示未赢分
    local jettonscore = 0
    for i,v in ipairs(self.m_lUserJettonScore) do
        jettonscore = jettonscore + v
    end
    if self.m_lSelfWinScore == 0 and jettonscore == 0 then
        return
    end

    --自己是庄家
    local isBanker = self:isMeChair(self.m_wBankerUser)

    -- if nil == self.m_gameResultLayer then
        if self.m_gameResultLayer then
            self.m_gameResultLayer:destroy()
        end
        self.m_gameResultLayer = uiResult:ctor(self)

        -- self.m_gameResultLayer = GameResultLayer:create(self)
        -- self:addToRootLayer(self.m_gameResultLayer, ZORDER_LAYER.ZORDER_Other_Layer)
    -- end
    self.m_gameResultLayer:refresh(self.m_lBankerWinScore, self.m_lSelfWinScore, self.m_tBankerName, isBanker)
    self.m_gameResultLayer:show()
    -- self.m_gameResultLayer:showGameResult(self.m_lSelfWinScore, self.m_lBankerWinScore, self.m_tBankerName, isBanker)
end

--设置下注按钮是否可以点击
function GameViewLayer:setJettonEnable(value)
    if value then
        self.unUseJetton = {true, true, true, true, true, true, true}
    else
        self.unUseJetton = {false, false, false, false, false, false, false}
    end

    self._listView:reloadData()
end

--更新下注按钮
--score：可以下注金额*MaxTimes
function GameViewLayer:updateJettonList(score)
    local btjettonscore = 0
    local judgeindex = 0
    if self.m_nJettonSelect == 0 then
        self.m_nJettonSelect = 1
    end

    local lastSelect = self.m_nJettonSelect

    for i=1,7 do
        btjettonscore = btjettonscore + self.m_JettonList[i]
        local judgescore = btjettonscore*MaxTimes
        if judgescore > score then
            self.unUseJetton[i] = false
            if i == lastSelect then
                --重新选择另一个按钮
                self.m_nJettonSelect = i-1
                lastSelect = i - 1
            end
        else
            self.unUseJetton[i] = true
            judgeindex = i
        end
    end

    if self.m_nJettonSelect > judgeindex then
        self.m_nJettonSelect = judgeindex
        if judgeindex == 0 then
            self:setJettonEnable(false)
        end
    end

    if self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_JETTON then
        self:setJettonEnable(false)
    else
        self._listView:reloadData()
    end
end

--更新选中按钮的动画显示
function GameViewLayer:updateJettonLight(index)
    for i=1, #self.m_JettonBtn do
        if i== index then
            self.m_JettonBtn[i]:getChildByName("im_jetton_effect"):setVisible(true)
            self.m_JettonBtn[i]:getChildByName("im_jetton_effect"):runAction(cc.RepeatForever:create(cc.Blink:create(1.0,1)))
        else
            self.m_JettonBtn[i]:getChildByName("im_jetton_effect"):stopAllActions()
            self.m_JettonBtn[i]:getChildByName("im_jetton_effect"):setVisible(false)
        end
    end
end

--更新下注分数显示
function GameViewLayer:updateAreaScore(bshow)
    if bshow == false then
        for k,v in pairs(self.m_tAllJettonScore) do
            v:setVisible(false)
        end
        for k,v in pairs(self.m_selfJettonBG) do
            v:setVisible(false)
        end
        return
    end

    for i=1,4 do
        if self.m_lUserJettonScore[i+1] > 0 then
            self.m_selfJettonBG[i]:setVisible(true)
            local txt_score = self.m_selfJettonBG[i]
            txt_score:setString(ExternalFun.formatScoreText(self.m_lUserJettonScore[i+1]))
        end
        if self.m_lAllJettonScore[i+1] > 0 then
            self.m_tAllJettonScore[i]:setVisible(true)
            self.m_tAllJettonScore[i]:setString(ExternalFun.formatScoreText(self.m_lAllJettonScore[i+1]))
        end
    end
end

--刷新游戏记录
function GameViewLayer:refreshGameRecord()
    if nil ~= self.m_GameRecordLayer and self.m_GameRecordLayer:isVisible() then
        local recordList = self:getDataMgr():getGameRecord()
        dump(recordList, "#record")
        self.m_GameRecordLayer:refreshRecord(recordList)
    end
end

--刷新列表
function GameViewLayer:refreshApplyList(  )
    if nil ~= self.m_applyListLayer and self.m_applyListLayer:isVisible() then
        local userList = self:getDataMgr():getApplyBankerUserList()
        self.m_applyListLayer:refreshList(userList)
    end
end

--刷新申请列表按钮状态
function GameViewLayer:refreshApplyBtnState(  )
    if nil ~= self.m_applyListLayer and self.m_applyListLayer:isVisible() then
        self.m_applyListLayer:refreshBtnState()
    end
end

--刷新用户列表
function GameViewLayer:refreshUserList(  )
    if nil ~= self.m_userListLayer and self.m_userListLayer:isVisible() then
        local userList = self:getDataMgr():getUserList()
        self.m_userListLayer:refreshList(userList)
    end
end

--刷新抢庄按钮
function GameViewLayer:refreshCondition(  )
    local applyable = self:getApplyable()
    if applyable then
        ------
        --超级抢庄

        --如果当前有超级抢庄用户且庄家不是自己
        if (yl.INVALID_CHAIR ~= self.m_wCurrentRobApply) or (true == self:isMeChair(self.m_wBankerUser)) then
            ExternalFun.enableBtn(self.m_btSupperRob, false)
        else
            local useritem = self:getMeUserItem()
            --判断抢庄类型
            if Game_CMD.SUPERBANKER_VIPTYPE == self.m_tabSupperRobConfig.superbankerType then
                --vip类型
                ExternalFun.enableBtn(self.m_btSupperRob, useritem.cbMemberOrder >= self.m_tabSupperRobConfig.enVipIndex)
            elseif Game_CMD.SUPERBANKER_CONSUMETYPE == self.m_tabSupperRobConfig.superbankerType then
                --游戏币消耗类型(抢庄条件+抢庄消耗)
                local condition = self.m_tabSupperRobConfig.lSuperBankerConsume + self.m_llCondition
                ExternalFun.enableBtn(self.m_btSupperRob, useritem.lScore >= condition)
            end
        end
    else
        ExternalFun.enableBtn(self.m_btSupperRob, false)
    end
end

--刷新用户分数
function GameViewLayer:onGetUserScore( useritem )
    --自己
    if useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
        self.m_showScore = useritem.lScore
        self:resetSelfInfo()

        GlobalUserItem.tabAccountInfo.lUserScore = useritem.lScore

    end

    --坐下用户
    for i = 1, Game_CMD.MAX_OCCUPY_SEAT_COUNT do
        if nil ~= self.m_tabSitDownUser[i] then
            if useritem.wChairID == self.m_tabSitDownUser[i]:getChair() then
                self.m_tabSitDownUser[i]:updateScore(useritem.lScore)
            end
        end
    end

    --庄家
    if self.m_wBankerUser == useritem.wChairID then
        --庄家游戏币
        self.m_lBankerScore = useritem.lScore
        self:resetBankerScore()
    end
end

--获取牛显示字体
function GameViewLayer:getNiuTxt(imagefile, niuvalue)
    local width = 56
    local height = 60
    local playout = ccui.Layout:create()
    playout:setAnchorPoint(0.5, 0.5)
    playout:setContentSize(cc.size(width, height))
    if niuvalue == GameLogic.BRNN_CT_SPECIAL_NIUNIUXW or niuvalue == GameLogic.BRNN_CT_SPECIAL_NIUNIUDW then
        -- playout:setContentSize(cc.size(width*3, height))
    else
        -- playout:setContentSize(cc.size(width*2, height))
    end

    if niuvalue == GameLogic.BRNN_CT_SPECIAL_BOMEBOME then
        local psprite = cc.Sprite:create(imagefile)
        psprite:setTextureRect(cc.rect(width*16, 0, width*2, height))
        psprite:setAnchorPoint(0.0, 0.5)
        psprite:setPosition(0, height/2)
        playout:addChild(psprite)
        return playout
    end

    local pniu = cc.Sprite:create(imagefile)
    pniu:setTextureRect(cc.rect(width*10, 0, width, height))
    pniu:setAnchorPoint(0.0, 0.5)

    if niuvalue <= GameLogic.BRNN_CT_SPECIAL_NIUNIU then
        local pnum = cc.Sprite:create(imagefile)
        pnum:setTextureRect(cc.rect((niuvalue-1)*width, 0, width, height))
        pnum:setAnchorPoint(0.0, 0.5)
        if niuvalue == GameLogic.BRNN_CT_POINT then
            pnum:setPosition(0,height/2)
            pniu:setPosition(width,height/2)
            playout:addChild(pnum)
            playout:addChild(pniu)
            return playout
        end
        pnum:setPosition(width,height/2)
        pniu:setPosition(0,height/2)
        playout:addChild(pnum)
        playout:addChild(pniu)
        return playout
    elseif niuvalue == GameLogic.BRNN_CT_SPECIAL_NIUYING or niuvalue == GameLogic.BRNN_CT_SPECIAL_NIUKING then
        local posx = niuvalue==GameLogic.BRNN_CT_SPECIAL_NIUYING and (niuvalue-2)*width or (niuvalue-4)*width
        local pnum = cc.Sprite:create(imagefile)
        pnum:setTextureRect(cc.rect(posx, 0, width, height))
        pnum:setAnchorPoint(0.0, 0.5)
        pnum:setPosition(0,height/2)
        pniu:setPosition(width,height/2)
        playout:addChild(pnum)
        playout:addChild(pniu)
        return playout
    elseif niuvalue == GameLogic.BRNN_CT_SPECIAL_NIUNIUXW or niuvalue == GameLogic.BRNN_CT_SPECIAL_NIUNIUDW then
        local posx = niuvalue==GameLogic.BRNN_CT_SPECIAL_NIUNIUXW and (niuvalue+2)*width or (niuvalue)*width
        local pnum = cc.Sprite:create(imagefile)
        pnum:setTextureRect(cc.rect(posx, 0, width, height))
        pnum:setAnchorPoint(0.0, 0.5)
        local pwang = cc.Sprite:create(imagefile)
        pwang:setTextureRect(cc.rect(15*width, 0, width, height))
        pwang:setAnchorPoint(0.0, 0.5)
        pnum:setPosition(0,height/2)
        pwang:setPosition(width, height/2)
        pniu:setPosition(width*2,height/2)
        playout:addChild(pwang)
        playout:addChild(pnum)
        playout:addChild(pniu)
    end

    return playout
end

--获取下注显示游戏币个数
function GameViewLayer:getGoldNum(lscore)
    local goldnum = 1
    for i=1,7 do
        if lscore >= self.m_JettonList[i] then
            goldnum = i
        end
    end
    return GameViewLayer.m_JettonGoldBaseNum[goldnum]
end

--获取输钱区域需要游戏币数
function GameViewLayer:getWinGoldNumWithAreaID(cbArea)
    local goldnum = 0
    local nmultiple = self.m_Multiple[cbArea + 1]
    local lAllJettonScore = self.m_lAllJettonScore[cbArea + 1]
    goldnum = goldnum + self:getWinGoldNum(self.m_lUserJettonScore[cbArea + 1]*nmultiple)
    --全是自己下注
    if self.m_lUserJettonScore[cbArea+1] == self.m_lAllJettonScore[cbArea + 1] then
        return goldnum
    end
    lAllJettonScore = lAllJettonScore - self.m_lUserJettonScore[cbArea+1]

    --坐下用户
    for i = 1, Game_CMD.MAX_OCCUPY_SEAT_COUNT do
        if nil ~= self.m_tabSitDownUser[i] then
            local seatJettonScore = self.m_tabSitDownUser[i]:getJettonScoreWithArea(cbArea)
            if seatJettonScore > 0 then
               goldnum = goldnum + self:getWinGoldNum(seatJettonScore*nmultiple)
               lAllJettonScore = lAllJettonScore - seatJettonScore
            end
        end
    end

    if lAllJettonScore <= 0 then
        return goldnum
    end

    goldnum = goldnum + self:getWinGoldNum(lAllJettonScore*nmultiple)
    return goldnum
end


--获取赢钱需要游戏币数
function GameViewLayer:getWinGoldNum(lscore)
    if lscore == 0 then
        return 0
    end
    local goldnum = 0
    for i=1,7 do
        if lscore >= self.m_JettonList[i] then
            goldnum = i
        end
    end
    return GameViewLayer.m_WinGoldBaseNum[goldnum] or 0
end

--获取随机显示位置
function GameViewLayer:getRandPos(nodeArea)
    local beginpos = cc.p(nodeArea:getPositionX()-80, nodeArea:getPositionY()-65)

    local offsetx = math.random()
    local offsety = math.random()

    return cc.p(beginpos.x + offsetx*160, beginpos.y + offsety*130)
end

--获取移动动画
--inorout,0表示加速飞出,1表示加速飞入
--isreverse,0表示不反转,1表示反转
function GameViewLayer:getMoveAction(beginpos, endpos, inorout, isreverse)
    local offsety = (endpos.y - beginpos.y)*0.7
    local controlpos = cc.p(beginpos.x, beginpos.y+offsety)
    if isreverse == 1 then
        offsety = (beginpos.y - endpos.y)*0.7
        controlpos = cc.p(endpos.x, endpos.y+offsety)
    end
    local bezier = {
        controlpos,
        endpos,
        endpos
    }
    local beaction = cc.BezierTo:create(0.42, bezier)
    if inorout == 0 then
        return cc.EaseOut:create(beaction, 1)
    else
        return cc.EaseIn:create(beaction, 1)
    end
end

--判断该用户是否坐下
function GameViewLayer:getIsUserSit(wChair)
    for i = 1, Game_CMD.MAX_OCCUPY_SEAT_COUNT do
        if nil ~= self.m_tabSitDownUser[i] then
            if wChair == self.m_tabSitDownUser[i]:getChair() then
                return self.m_tabSitDownUser[i]
            end
        end
    end
    return nil
end

--上庄状态
function GameViewLayer:applyBanker( state )
    if state == APPLY_STATE.kCancelState then
        self:getParentNode():sendApplyBanker()
    elseif state == APPLY_STATE.kApplyState then
        self:getParentNode():sendCancelApply()
    elseif state == APPLY_STATE.kApplyedState then
        self:getParentNode():sendCancelApply()
    end
end

------
--银行节点
function GameViewLayer:createBankLayer()
    self.m_bankLayer = cc.Node:create()
    self:addToRootLayer(self.m_bankLayer, ZORDER_LAYER.ZORDER_Other_Layer)

    --加载csb资源
    local csbNode = ExternalFun.loadCSB("BankLayer.csb", self.m_bankLayer)
    local sp_bg = csbNode:getChildByName("sp_bg")

    ------
    --按钮事件
    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(sender:getTag(), sender)
        end
    end
    --关闭按钮
    local btn = sp_bg:getChildByName("close_btn")
    btn:setTag(TAG_ENUM.BT_CLOSEBANK)
    btn:addTouchEventListener(btnEvent)

    local layout_bg = csbNode:getChildByName("layout_bg")
    layout_bg:setTag(TAG_ENUM.BT_CLOSEBANK)
    layout_bg:addTouchEventListener(btnEvent)

    --取款按钮
    btn = sp_bg:getChildByName("out_btn")
    btn:setTag(TAG_ENUM.BT_TAKESCORE)
    btn:addTouchEventListener(btnEvent)

    --存款按钮
    btn = sp_bg:getChildByName("in_btn")
    btn:setTag(TAG_ENUM.BT_SAVESCORE)
    btn:addTouchEventListener(btnEvent)
    ------

    ------
    --编辑框
    --取款金额
    local tmp = sp_bg:getChildByName("count_temp")
    local editbox = ccui.EditBox:create(tmp:getContentSize(),"game_bank_v2_plist/im_bank_edit.png",0)
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入取款金额")
    sp_bg:addChild(editbox)
    self.m_bankLayer.m_editNumber = editbox
    tmp:removeFromParent()

    --取款密码
    tmp = sp_bg:getChildByName("passwd_temp")
    editbox = ccui.EditBox:create(tmp:getContentSize(),"game_bank_v2_plist/im_bank_edit.png",0)
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("存款不需要密码")
    sp_bg:addChild(editbox)
    self.m_bankLayer.m_editPasswd = editbox
    tmp:removeFromParent()
    ------

    --当前游戏币
    self.m_bankLayer.m_textCurrent = sp_bg:getChildByName("text_current")

    --银行游戏币
    self.m_bankLayer.m_textBank = sp_bg:getChildByName("text_bank")

    --取款费率
    self.m_bankLayer.m_textTips = sp_bg:getChildByName("text_tips")

    self.bankBtns = {}
    for i = 1, 6 do 
        local btn = sp_bg:getChildByName("btn_"..i)
        btn:setTag(i)
        if i == 6 then
            btn:addTouchEventListener(function(sender) 
                ExternalFun.playClickEffect() 
                self.m_bankLayer.m_editNumber:setText("")
            end)
        else
            if i < 4 then
                self:addLabelAtlas(btn,BANK_BTN_NUM[i])
                table.insert(self.bankBtns,btn)
            end
            btn:addTouchEventListener(function(sender) 
                ExternalFun.playClickEffect() 
                self.m_bankLayer.m_editNumber:setText(BANK_BTN_NUM[sender:getTag()])
                if sender:getTag() == 4 then
                    self.m_bankLayer.m_editNumber:setText(BANK_BTN_NUM[sender:getTag()] - self.m_lUserAllJetton * 10)
                end
            end)
        end
    end
end

function GameViewLayer:addLabelAtlas(parent, str)
    local tag = 11
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","game_bank_v2_plist/num2.png",19, 27, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2 + 4 )
    local num = str
    lab:setString(num)
    if num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("game_bank_v2_plist/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2+ 3)
    end
end

--取款
function GameViewLayer:onTakeScore()
    if not self:canUseBank() then
        return
    end
    --参数判断
    local szScore = string.gsub(self.m_bankLayer.m_editNumber:getText(),"([^0-9])","")
    local szPass = self.m_bankLayer.m_editPasswd:getText()

    if #szScore < 1 then
        showToast(self,"请输入操作金额！",2)
        return
    end

    local lOperateScore = tonumber(szScore)
    if lOperateScore<1 then
        showToast(self,"请输入正确金额！",2)
        return
    end

    if #szPass < 1 then
        showToast(self,"请输入银行密码！",2)
        return
    end
    if #szPass <6 then
        showToast(self,"密码必须大于6个字符，请重新输入！",2)
        return
    end

    self:showPopWait()
    self:getParentNode():sendTakeScore(szScore,szPass)
end

-- 存款
function GameViewLayer:onSaveScore()

    if not self:canUseBank() then
        return
    end
    --参数判断
    local szScore = string.gsub(self.m_bankLayer.m_editNumber:getText(),"([^0-9])","")

    if #szScore < 1 then 
        showToast(self,"请输入操作金额！",2)
        return
    end

    local lOperateScore = tonumber(szScore)
    if lOperateScore<1 then
        showToast(self,"请输入正确金额！",2)
        return
    end

    self:showPopWait()
    self:getParentNode():sendSaveScore(szScore) 
end

--刷新银行游戏币
function GameViewLayer:refreshBankScore( )
    --携带游戏币
    local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    str = str:gsub(",", ".")
    self.m_bankLayer.m_textCurrent:setString(str)

    --银行存款
    str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    str = str:gsub(",", ".")
    self.m_bankLayer.m_textBank:setString(str)

    self.m_bankLayer.m_editNumber:setText("")
    self.m_bankLayer.m_editPasswd:setText("")
end

function GameViewLayer:onAddGameTrumpet(item)
    self.m_gameTrumpetList = self.m_gameTrumpetList or {}
    item = item or {}
    table.insert(self.m_gameTrumpetList, clone(item))

    if not self.m_spGameTrumpetBg:isVisible() then
        self.m_spGameTrumpetBg:runAction(cc.Sequence:create(cc.Show:create(), cc.ScaleTo:create(0.5, 1.0), cc.CallFunc:create(function()
            self:onGameTrumpet()
        end)))
    end
end

function GameViewLayer:onGameTrumpet()
    local item = self.m_gameTrumpetList[1]

    local szmarqueetitle = nil
    local color = nil
    if item then
        szmarqueetitle = item.marqueetitle
        color = item.color
        if true == item.autoremove then
            local count = item.showcount or 0
            item.showcount = count - 1
            if item.showcount <= 0 then
                local idx = nil
                for k,v in pairs(self.m_gameTrumpetList) do
                    if nil ~= v.id and v.id == item.id then
                        idx = k
                        break
                    end
                end
                if nil ~= idx then
                    table.remove(self.m_gameTrumpetList, idx)
                end
            end
        end
    else
        print("没有消息了")
        return
    end

    local text = cc.find(self.m_spGameTrumpetBg, "Panel_trump/text_trump")
    text:getParent():setClippingEnabled(true)

    local bgsize = self.m_spGameTrumpetBg:getChildByName("Panel_trump"):getContentSize()
    if text and szmarqueetitle then
        text:setString(szmarqueetitle)
        text:setPositionX(bgsize.width)
        text:setColor(cc.c3b(color.r,color.g,color.b))
        text:setOpacity(color.a)
        text:stopAllActions()
        local tmpWidth = text:getContentSize().width

        text:runAction(cc.Sequence:create(cc.Show:create(), cc.MoveTo:create(16 + (tmpWidth / bgsize.width), cc.p(0-tmpWidth, text:getPositionY())), cc.CallFunc:create(function()
            if 0 ~= #self.m_gameTrumpetList then
                self:onGameTrumpet()
            else
                self.m_spGameTrumpetBg:runAction(cc.Sequence:create(cc.ScaleTo:create(0.5, 0.0001, 1.0), cc.Hide:create() ) )
            end
        end)))
    end
end
------
function GameViewLayer:canUseBank()
    if self.m_lUserAllJetton > 0 then 
        showToast(self,"已下注不可进行银行操作",1)
        return false
    end
    if self:isMeChair(self.m_wBankerUser) and self.m_cbGameStatus ~= Game_CMD.GAME_SCENE_FREE then
        showToast(self,"非空闲时间段不可进行银行操作",1)
        return false
    end
    return true
end
return GameViewLayer

