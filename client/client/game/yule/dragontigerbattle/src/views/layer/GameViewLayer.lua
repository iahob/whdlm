local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC.."HeadSprite")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local SettingLayer = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.SettingLayer")
local GameRecord = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.GameRecord")
local CardSprite = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.CardSprite")
local PlayerSite = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.PlayerSite")
local GAME_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.GameLogic")
local UserListLayer = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.UserListLayer")
local BankLayer = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.BankLayer")
local HelpLayer = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.HelpLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local headFrame = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.HeadFrame")
local BankerLayer = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.BankerLayer")
local SpineObj = appdf.req("client.src.external.SpineObj")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")


local GameViewLayer = class("GameViewLayer",function(scene)
        local gameViewLayer = display.newLayer()
    return gameViewLayer
end)

local enumTable = {
    "HEAD_MYSELF",
    "BT_SET",
    "BT_QUIT",
    "BT_MENU",
    "BT_HELP",
    "BT_LEFT",
    "BT_RIGHT",
    "BT_REJECT",
    "BT_ONLINE",
    "BT_BANK",
    "BT_APPLY",
    "BT_RECORD_BG",
    "BT_RECORD",
    "BT_JETTONAREA_1",
    "BT_JETTONAREA_2",
    "BT_JETTONAREA_3",
    "BT_SITE_1",
    "BT_SITE_2",
    "BT_SITE_3",
    "BT_SITE_4",
    "BT_SITE_5",
    "BT_SITE_6",
}

local enumLayer = {
    "ZORDER_CARD",
    "ZORDER_GOLD",
    "ZORDER_SCORE",
    "ZORDER_SITE",        --玩家头像
    "ZORDER_CHAIR",
    "ZORDER_GOLD",
    "ZORDER_HEADLAYER",
    "ZORDER_JETTONNOTICE",
    "ZORDER_ANIMATE",
    "ZORDER_SURFACE",  --新建的界面
    "ZORDER_HEADFRAME",
}

local MaxTimes = 1   ---最大赔率


--自己头像位置
local selfheadpoint = cc.p(80, 60)

--玩家列表位置
local userListPos = cc.p(85, 122)

--庄家金币位置
local bankerGoldPos = cc.p(253.37, 541.95)

GameViewLayer.cardPos = {cc.p(580.05,675), cc.p(770.21,675)}

local TAG_ENUM = ExternalFun.declarEnumWithTable(0, enumTable)
local ZORDER_LAYER = ExternalFun.declarEnumWithTable(100, enumLayer)


function GameViewLayer:ctor(scene)
	  --注册node事件
    ExternalFun.registerNodeEvent(self, true)	

    self._scene = scene
    self.banker_score = 0
    --初始化
    self:paramInit()

    --加载资源
    self:loadResource()

    ExternalFun.playBackgroudAudio("bg.mp3")
end

function GameViewLayer:paramInit()
    --游戏状态
    self.cbGameStatus = GAME_CMD.GAME_SCENE_FREE
    --剩余时间
    self.cbTimeLeave = 0

    --区域限制
    self.lAreaLimitScore = 0

    --每个区域的总分
    self.lAreaInAllScore = {0,0,0}

    --玩家每个区域的下注
    self.lUserInAllScore = {0,0,0}

    --最大下注
    self.lUserMaxScore = 0

    --自己金币
    self.my_gold = self:getMeUserItem().lScore

    --庄家的金币
    self.banker_gold = 0

    --庄家的名字
    self.banker_name = ""

     --庄家椅子号
    self.wBankerUser = yl.INVALID_CHAIR

    --自己已经下注总数
    self.myAllJettonScore = 0

    --桌面扑克
    self.cbTableCardArray = {}

    --选中的按钮
    self.m_nJettonSelect = 0

    --选中的按钮
    self.m_select = nil

    -- 额度选择
    self.m_BetAmount = 0

    --按钮是否可点击
    self.m_JettonBtn = {true, true, true, true, true, true, true, true}

    --上一次按钮状态记录
    self.m_lastJettonBtn = {true, true, true, true, true, true, true, true}

    --重复下注按钮是否点击过
    self.isReject = false

    --上一局自己区域下注分数
    self.m_lastRoundJettonScore = {0,0,0}

    --管理两张牌
    self.cardArray = {}

    --坐下用户管理
    self.siteUser = {nil, nil, nil, nil, nil, nil}

    --占位椅子ID
    self.wOccupySeatChairIDArray = {}

    --免费占位条件
    self.lOccupySeatFree = 0

    --每个区域下注的金币
    self.goldJetton = {{}, {}, {}}

    self.goldLayer = nil

    --坐下按钮管理
    self.siteButton = {}

    --所有玩家下注信息按椅子号，按区域顺序
    self.allUserJetton = {}

    --简洁路单
    self.simpleChart = {}

    --简洁路单，记录龙虎点数
    self.simplePoint = {}

    --高级路单，记录龙虎点数
    self.advancedPoint = {}

    --高级路单
    self.advancedChart = {}

    --路单界面
    self.gameRecordLayer = nil

    --每个玩家每个区域的输赢
    self.lUserAreaWinScore = {}

    --动画层
    self._animateLayer = nil

    --断线重连
    self.isRelink = false

    --等待下一局
    self.waitNextRound = nil

    --等待上庄
    self.waitShangZhuang = nil

    --结算界面
    -- self.resultLayer = nil

    --所有玩家成绩
    self.lUserWinScore = {}

    --玩家成绩标签
    self.resultScore = {}

    --申请上庄条件
    self.lApplyBankerCondition = 0

    --庄家列表层
    self.bankerListLayer = nil

    --自己结算成绩
    self.lUserScore = 0

    --庄家结算成绩
    self.lBankerWinScore = 0

    --是否坐庄
    self.isHaveBanker = false

    self.isMyBet = false
end

function GameViewLayer:loadResource()
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game.csb", self) 
    --加载卡牌资源
    cc.SpriteFrameCache:getInstance():addSpriteFrames("card/card.plist")

    self.csbNode = csbNode

	local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
  end

  --菜单
  btn = csbNode:getChildByName("Button_menu")
  btn:setTag(TAG_ENUM.BT_MENU)
  btn:addTouchEventListener(btnEvent)

  --设置
  self.menu = csbNode:getChildByName("Node_menu")
  self.menu:setLocalZOrder(ZORDER_LAYER.ZORDER_CHAIR+1)
  btn = self.menu:getChildByName("Button_setting")
  btn:setTag(TAG_ENUM.BT_SET)
  btn:addTouchEventListener(btnEvent)

  --帮助
  btn = self.menu:getChildByName("Button_help")
  btn:setTag(TAG_ENUM.BT_HELP)
  btn:addTouchEventListener(btnEvent)

  --银行
  btn = self.menu:getChildByName("Button_bank")
  btn:setTag(TAG_ENUM.BT_BANK)
  btn:addTouchEventListener(btnEvent)
  -- 是否开启银行服务
  local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
  btn:setVisible(enableBank)

  self.bankerBtn = btn;

  --申上庄
  self.bankerNode = csbNode:getChildByName("Node_applyBanker")


  local panel = csbNode:getChildByName("Node_applyBanker_0") 
  btn = panel:getChildByName("Button_apply")
  btn:setTag(TAG_ENUM.BT_APPLY)
  btn:addTouchEventListener(btnEvent)

  local list = self:getDataMgr():getApplyBankerUserList()
  local txt = panel:getChildByName("Text_1_0")
  txt:setString(#list)
  schedule(txt,function ()
      local list = self:getDataMgr():getApplyBankerUserList()
      txt:setString(#list)
  end,1)


  --退出
  btn = csbNode:getChildByName("Button_exit")
  btn:setTag(TAG_ENUM.BT_QUIT)
  btn:addTouchEventListener(btnEvent)

  --路单bg
  btn = csbNode:getChildByName("Button_ludan1")
  btn:setTag(TAG_ENUM.BT_RECORD_BG)
  btn:addTouchEventListener(btnEvent)

  --路单
  btn = csbNode:getChildByName("Button_ludan2")
  btn:setTag(TAG_ENUM.BT_RECORD)
  btn:addTouchEventListener(btnEvent)

  --在线
  btn = csbNode:getChildByName("Button_online")
  btn:setTag(TAG_ENUM.BT_ONLINE)
  btn:addTouchEventListener(btnEvent)

  self.lineTxt = btn:getChildByName("AtlasLabel_1")
  self.lineTxt:setString("0")
  self:setLinePlayer()

  --重复下注
  btn = csbNode:getChildByName("Button_reject")
  btn:setTag(TAG_ENUM.BT_REJECT)
  btn:addTouchEventListener (btnEvent)

  --左滑右滑
  btn = csbNode:getChildByName("Button_left")
  btn:setTag(TAG_ENUM.BT_LEFT)
  btn:addTouchEventListener (btnEvent)

  btn = csbNode:getChildByName("Button_right")
  btn:setTag(TAG_ENUM.BT_RIGHT)
  btn:addTouchEventListener (btnEvent)

  --自己头像框
  csbNode:getChildByName("selfinfo"):setLocalZOrder(ZORDER_LAYER.ZORDER_CHAIR)
  btn = csbNode:getChildByName("selfinfo"):getChildByName("Button_headFrame")
  btn:setTag(TAG_ENUM.HEAD_MYSELF)
  btn:addTouchEventListener(btnEvent)

  --下注区域
  for i=1, 3 do
      local str = string.format("Button_Area_%d", i)
      btn = csbNode:getChildByName(str)
      btn:setTag(TAG_ENUM.BT_JETTONAREA_1 + i-1)
      btn:addTouchEventListener(btnEvent)
  end

  --坐下按钮
  self.siteName = {}
  for i=1, 6 do
        local str = string.format("Button_site_%d", i)
        btn = csbNode:getChildByName(str)
        btn:setLocalZOrder(ZORDER_LAYER.ZORDER_CHAIR)
        btn:setTag(TAG_ENUM.BT_SITE_1 + i-1)
        btn:addTouchEventListener(btnEvent)
        table.insert(self.siteButton, btn)

        self.siteName[i] = csbNode:getChildByName('name_site_'..i)
  end



    self.goldLayer = cc.Layer:create():addTo(self.csbNode, ZORDER_LAYER.ZORDER_GOLD)

    --动画层
    self._animateLayer = cc.Node:create():addTo(self.csbNode, ZORDER_LAYER.ZORDER_ANIMATE)

    self.jettonList = ccui.Layout:create()
    self.jettonList:setContentSize(cc.size(615+125, 138))   
    self.jettonList:setPosition(cc.p(180,-150))
    self.jettonList:addTo(self.csbNode)
    self:upDateBTJetton()

    --wifi
   self.wifi1 = self.csbNode:getChildByName("Node_wifi")
   self.wifi2 = self.csbNode:getChildByName("Node_wifi2"):setVisible(false)

    self.pTimer0 = cc.Sprite:create("sign/battery_1.png")
    :setPosition(cc.p(150 , 725.05))
    :setFlippedX(true)
    :addTo(self.csbNode)

    self.pTimer100 = ccui.LoadingBar:create("sign/battery_2.png")
    self.pTimer100:move(cc.p(145.5 , 725.05))
    self.pTimer100:setPercent(100)
    self.pTimer100:addTo(self.csbNode )


    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if cc.PLATFORM_OS_ANDROID == targetPlatform then
      self._timeStatus = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
         --电量
          local bCount1 , bCount2 = math.modf(MultiPlatform:getInstance():getBatteryLevel())
          self.pTimer100:setPercent(bCount1)

          -- local netType = MultiPlatform:getInstance():getNetWorkType() -- -1,0没网  1数据网络 2WiFi
          -- local netType = 2
          -- if netType == 1 then
          --   self.wifi1:setVisible(false)
          --   self.wifi2:setVisible(true)
          -- else
          --   self.wifi1:setVisible(true)
          --   self.wifi2:setVisible(false)
          -- end

          --网络
          local state = MultiPlatform:getInstance():getNetworkStrength()
          for i=1, 3 do
                if i <= state then
                    self.wifi1:getChildByName(string.format("Sprite_%d", i)):setVisible(true)
                    -- self.wifi2:getChildByName(string.format("Sprite_%d", i)):setVisible(true)
                else
                    -- self.wifi1:getChildByName(string.format("sign%d", i)):setVisible(false)
                    -- self.wifi2:getChildByName(string.format("sign%d", i)):setVisible(false)
                end
          end
      end, 5, false)
    else
      self._timeStatus = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
    --        --电量
          local bCount1 , bCount2 = math.modf(MultiPlatform:getInstance():getBatteryLevel())
          self.pTimer100:setPercent(bCount1)

          local state = MultiPlatform:getInstance():getNetworkStrength()
          for i=1, 3 do
                if i <= state then
                    self.wifi1:getChildByName(string.format("Sprite_%d", i)):setVisible(true)
                else
                    self.wifi1:getChildByName(string.format("Sprite_%d", i)):setVisible(false)
                end
          end

      end, 2, false)
    end

    --时间
    self.labelTime = self.csbNode:getChildByName("Text_time")
    self.labelTime:setString("00:00")
    -- self.labelTime:setPosition(235, 715)
    local function timeUpdate()
      self.labelTime:setString(""..os.date("%H")..":"..os.date("%M"))
    end
    self._timeShd = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 1, false)

    self:createCardBack()


    local spine = SpineObj:create("animation/waitingTiger/skeleton.json", "animation/waitingTiger/skeleton.atlas", false)
    spine:setAnimation(0, "animation", true)
    spine:setPosition(1030,690)
    self.csbNode:getChildByName("spine"):addChild(spine)

    -- local spine = SpineObj:create("animation/waitingLong/skeleton.json", "animation/waitingLong/skeleton.atlas", false)
    -- spine:setAnimation(0, "animation", true)
    -- spine:setPosition(280,400)
    -- self.csbNode:getChildByName("spine"):addChild(spine)

    local spine = SpineObj:create("animation/deng/skeleton.json", "animation/deng/skeleton.atlas", false)
    spine:setAnimation(0, "animation", true)
    spine:setPosition(676,371)
    self.csbNode:getChildByName("spine"):addChild(spine)

    local spine = SpineObj:create("animation/deng/skeleton.json", "animation/deng/skeleton.atlas", false)
    spine:setAnimation(0, "animation", true)
    spine:setPosition(-354,371)
    self.csbNode:getChildByName("spine"):addChild(spine)

    AnimationMgr.loadAnimationFromFrame("animation/waitingLong/skeleton-animation_%d.png", 0, 90, "waitingLong",2)
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.04
    param.m_strName = "waitingLong"
    local animate = AnimationMgr.getAnimate(param)

    local sp = display.newSprite("animation/waitingLong/skeleton-animation_0.png")
    sp:setAnchorPoint(cc.p(0.5, 0.5))
    sp:setPosition(300,700)
    self.csbNode:getChildByName("spine"):addChild(sp)
    sp:runAction(cc.RepeatForever:create(animate))

    -- local sp = display.newSprite("animation/waitingLong/yun2.png")
    -- sp:setAnchorPoint(cc.p(0.5, 0.5))
    -- sp:setPosition(500,700)
    -- self:addChild(sp)
    -- sp:runAction(cc.RepeatForever:create(animate))

    --[[local flower = cc.ParticleFlower:createWithTotalParticles(1000)
    flower:setPosition(667,375)
    self:addChild(flower)


    local move1 = cc.MoveTo:create(1,cc.p(1200,375))
    local move2 = cc.MoveTo:create(1,cc.p(100,375))
    flower:runAction(cc.Sequence:create({move1,move2,cc.CallFunc:create(function ()
      -- body
      flower:setLife(0.01)
      flower:setStartSize(50)
      flower:setEndSize(100)
      flower:setScale(0.4)

    end)}))--]]

    for i=1, 3 do

       local tmp = self.csbNode:getChildByName(string.format("lottery%d", i))
       tmp:setVisible(false)

    end
end

function GameViewLayer:onExit()
    self:resetGameData()
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

    AnimationMgr.removeCachedAnimation("waitingLong")
end

function GameViewLayer:hideBankerBtn()
    if GlobalUserItem.dwCurRoomGroupID ~= 0 then
      self.bankerBtn:setVisible(false)
    end
end

function GameViewLayer:upDateBTJetton()
    -- local scale = 0.836
    -- local pos = cc.p(65,72)
    -- local item = nil
    -- self.jettonList:removeAllChildren()
    -- for i=1,#self.lChipArray do    
    --     local str1 = string.format("game/bt_jetton_%d.png", i)
    --     local str2 = string.format("game/bt_jetton_abnormal.png")
    --     item = ccui.Button:create(str1, str1, str2)
    --     -- item:setSwallowTouches(false)
    --     item:setScale(scale)
    --     item:setAnchorPoint(cc.p(0,0))
    --     item:setPosition(cc.p(124+124*(i-1), 138))
    --     item:setName("btn"..i)
    --     item:setTag(i)
    --     self.jettonList:addChild(item)
    --     item:addClickEventListener(function() self:tableCellTouched(item) end)

    --     local textLayout = self:createJettonText(self.lChipArray[i])
    --     textLayout:setAnchorPoint(cc.p(0.5,0.5))
    --     textLayout:setScale(scale)
    --     textLayout:setName("label"..i)
    --     textLayout:setPosition(item:getContentSize().width/2,item:getContentSize().height/2)
    --     item:addChild(textLayout)

    --     --判断该按钮是否可以点击
    --     if not self.m_JettonBtn[i] then
    --         self.jettonList:getChildByName("btn"..i):setEnabled(false)
    --     else
    --         self.jettonList:getChildByName("btn"..i):setEnabled(true)
    --     end

    --     if i == self.m_nJettonSelect and item:isEnabled() then
    --         self.m_select = item
    --         local effect = cc.Sprite:create("game/jettoneffect.png") 
    --         effect:setScale(scale)
    --         effect:setPosition(item:getContentSize().width/2,item:getContentSize().height/2)
    --         effect:setName("im_jetton_effect")
    --         self.jettonList:addChild(effect)
    --         effect:runAction(cc.RepeatForever:create(cc.Blink:create(1.0,1)))
    --     end
    -- end
end

--创建筹码上的文字
function GameViewLayer:createJettonText(score)
    local function formatScoreText(score)
        if score < 10000 then
            return score
        elseif score < 10000000 then
            return score/10000
        else
            return score/10000000
        end
    end

    local startX = 10
    local startY = 75
    local width = 0
    local labelWidth = 0
    local imgWidth = 0
    --数字与汉字之间的距离
    local gap = 2       

    local ly = ccui.Layout:create()
    local text = formatScoreText(score)
    local label = cc.LabelAtlas:create(text,"no/chouma.png",21,27,string.byte("0"))
    -- num = cc.Label:createWithTTF(text, "fonts/round_body.ttf", 27)
    label:setAnchorPoint(cc.p(0.5,0.5))
    labelWidth = label:getContentSize().width

    if text == 0 then
        label:setVisible(false)
    else
        label:setVisible(true)
    end
    
    local img = nil
    if score < 10000 then
    elseif score < 10000000 then
        img = cc.Sprite:create("no/wan.png")
    else
        img = cc.Sprite:create("no/qianwan.png")
    end

    if img ~= nil then
        img:setAnchorPoint(cc.p(0.5, 0.5))
        imgWidth = img:getContentSize().width
    end

    --标签设置
    if imgWidth == 0 then
        width = labelWidth
    else
         width = labelWidth + imgWidth + gap
    end
    ly:setContentSize(cc.size(width, 36))  

    --添加数字和汉字
    label:setPosition(cc.p(labelWidth /2, 18))
    ly:addChild(label) 
    if img ~= nil then
        img:setPosition(cc.p(labelWidth+gap + imgWidth /2 , 18))
        ly:addChild(img)
    end

    return ly
end

function GameViewLayer:cellSizeForTable()
    return 124, 138
end


function GameViewLayer:onButtonClickedEvent(tag, ref)

    if tag < TAG_ENUM.BT_JETTONAREA_1 or tag > TAG_ENUM.BT_JETTONAREA_3 then 
        ExternalFun.playClickEffect()
    end

    if tag == TAG_ENUM.BT_MENU then
        self:showMenu()
    elseif tag == TAG_ENUM.BT_SET then
        self:showSettingLayer()
    elseif tag == TAG_ENUM.BT_HELP then
        self:showHelpLayer()
    elseif tag == TAG_ENUM.BT_RECORD_BG or tag == TAG_ENUM.BT_RECORD then
        if self.gameRecordLayer == nil then
            if self.advancedChart == nil or #self.advancedChart == 0 then
                -- showToast(self, "路单", 1)
               return;
            end
            self.gameRecordLayer = GameRecord:create(self.advancedChart, self):addTo(self.csbNode, ZORDER_LAYER.ZORDER_SURFACE)
            self.gameRecordLayer:freshSimpleChart(self.simpleChart)
        else
            self.gameRecordLayer:setVisible(true)
        end
    elseif tag == TAG_ENUM.BT_ONLINE then
      self._scene:sendLinePalyer()
        -- self:showUserList()
    elseif tag == TAG_ENUM.BT_REJECT then
        self:sendReject()
    elseif tag == TAG_ENUM.BT_QUIT then
      -- if self:isMeIsBanker() and self.gameState ~= GAME_CMD.GAME_SCENE_FREE then
      --   
      -- else
      if self:isMeIsBanker()  then
          self._scene.m_cbGameStatus = GAME_CMD.GAME_SCENE_JETTON;
          -- self._scene:onExitRoom()
      else
          
      end
        self._scene:onQueryExitGame()
      -- end
    elseif tag == TAG_ENUM.BT_BANK then
        --银行
        if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
            showToast(self, "初次使用，请先开通银行！", 1)
            return 
        end

        self.bankLayer = BankLayer:create(self):addTo(self):setPosition(cc.p(display.cx, display.cy))
        --空闲状态才能存款
        -- if self.gameState ~= GAME_CMD.GAME_SCENE_FREE then
        --     showToast(self, "只有空闲状态可以进行银行操作", 1)
        -- else
        --     
        -- end
    elseif tag >= TAG_ENUM.BT_JETTONAREA_1 and tag <= TAG_ENUM.BT_JETTONAREA_3 then
        if self:isMeChair(self.wBankerUser) then
            return
        end
        self:onJettonAreaClicked(ref:getTag()-TAG_ENUM.BT_JETTONAREA_1 + 1, ref)
    elseif tag == TAG_ENUM.BT_LEFT or tag == TAG_ENUM.BT_RIGHT then
        self:changeTableView(tag)
    elseif tag >= TAG_ENUM.BT_SITE_1 and tag <= TAG_ENUM.BT_SITE_6 then
        self:onPlayerSite(ref:getTag()-TAG_ENUM.BT_SITE_1+1, ref)
    elseif tag == TAG_ENUM.HEAD_MYSELF then
        self:showSelfInfo()
    elseif tag == TAG_ENUM.BT_APPLY then
        if not self.bankerListLayer then
            self.bankerListLayer = BankerLayer:create(self):addTo(self)
        else
            self.bankerListLayer:setVisible(true)
        end

        local userList = self:getDataMgr():getApplyBankerUserList()

        for k,v in pairs(userList) do
            if v and v.wChairID == self.GetMeChairID then 
                self.bankerListLayer:onCancelBanker()
            end
        end

        if self:isMeIsBanker() then
            self.bankerListLayer:showMeBankerBet()
        end

    end
end

--设置自己信息
function GameViewLayer:setSelfInfo()
    local head = PopupInfoHead:createNormal(self:getMeUserItem(),55)
    self.csbNode:getChildByName("selfinfo"):getChildByName("Node_1"):addChild(head)


    local temp = self.csbNode:getChildByName("selfinfo"):getChildByName("name")
    local name =  string.EllipsisByConfig(self:getMeUserItem().szNickName,180,string.getConfig("fonts/round_body.ttf" , 28))
    local pselfname = ClipText:createClipText(cc.size(145, 26), name)
    pselfname:setAnchorPoint(temp:getAnchorPoint())
    pselfname:setPosition(temp:getPosition())
    pselfname:setName(temp:getName())
    temp:removeFromParent()
    self.csbNode:getChildByName("selfinfo"):addChild(pselfname)

    self.my_gold  = self:getMeUserItem().lScore
    self.csbNode:getChildByName("selfinfo"):getChildByName("gold"):setString(ExternalFun.formatScoreText(self.my_gold))
end

--设置庄家信息
function GameViewLayer:upDateBankerInfo()
    if self.wBankerUser == yl.INVALID_CHAIR then
        self.bankerNode:getChildByName("Text_name"):setString("系统坐庄")
        self.bankerNode:getChildByName("AtlasLabel_gold"):setString("")
        self.bankerNode:getChildByName("enough_large"):setVisible(true)
        self:setSystemBanker(true)
        self:resetBankerState()
        return
    end



    self.bankerNode:getChildByName("enough_large"):setVisible(false)
    self:setSystemBanker(false)
    local useritem = self:getBankerInfo()
    if l == useritem then return end
    self.bankerNode:getChildByName("Text_name"):setString(useritem.szNickName)


   local lScore =  tostring(useritem.lScore)
   self.banker_score = useritem.lScore
   self:updateBankerMoney(self.banker_score)
    --断线重连或者庄家退出重进游戏
    self:resetBankerState()

    self.csbNode:getChildByName("Node_applyBanker_0"):setVisible(self.isHaveBanker)

end

function GameViewLayer:updateBankerMoney(lScore)
   if lScore >= 100000000 then
        self.bankerNode:getChildByName("Text_wan"):setVisible(false)
        self.bankerNode:getChildByName("Text_yi"):setVisible(true)
        lScore = string.format("%.2f", lScore  / 100000000)
    elseif lScore >= 10000 then
        self.bankerNode:getChildByName("Text_wan"):setVisible(true)
        self.bankerNode:getChildByName("Text_yi"):setVisible(false)
        lScore = string.format("%.2f", lScore  / 10000)
    else
        self.bankerNode:getChildByName("Text_wan"):setVisible(false)
        self.bankerNode:getChildByName("Text_yi"):setVisible(false)
    end

    local gold_panel = self.bankerNode:getChildByName("AtlasLabel_gold")
    lScore = string.gsub(lScore,'%.','/')
    gold_panel:setString(lScore)
    self.bankerNode:getChildByName("Text_wan"):setPositionX(gold_panel:getPositionX() + gold_panel:getContentSize().width)
    self.bankerNode:getChildByName("Text_yi"):setPositionX(gold_panel:getPositionX() + gold_panel:getContentSize().width)
end

--重置庄家状态
function GameViewLayer:resetBankerState()
  if not self.bankerListLayer then
      self.bankerListLayer = BankerLayer:create(self):addTo(self)
      self.bankerListLayer:setVisible(false)
  end

  self.bankerListLayer:changeBanker()
end

--移除庄家
function GameViewLayer:removeBanker()
    self.wBankerUser = yl.INVALID_CHAIR
end

--移除银行
function GameViewLayer:removeBankerLayer()
    if self.bankLayer then
        self.bankLayer:removeFromParent()
        self.bankLayer = nil
    end
end

function GameViewLayer:getBankerInfo()



    local userItem = self:getDataMgr():getChairUserList()[self.wBankerUser+1]
    -- dump(userItem,"getBankerInfo suserItem",12)
    return userItem
end

function GameViewLayer:showSelfInfo()
    local x, y = self.csbNode:getChildByName("selfinfo"):getChildByName("Button_headFrame"):getPosition()
    local pos = cc.p(x+350, y+100)
    self.my_headFrame = headFrame:create(self:getMeUserItem(), pos):addTo(self.csbNode):setLocalZOrder(ZORDER_LAYER.ZORDER_HEADFRAME)
end

function GameViewLayer:upDateSelfInfo()
    self.csbNode:getChildByName("selfinfo"):getChildByName("gold"):setString(ExternalFun.formatScoreText(self.my_gold))
end

--菜单是否可见
function GameViewLayer:showMenu()
    if self.menu:isVisible() then
        self.menu:setVisible(false)
    else
        self.menu:setVisible(true)
    end
end

--设置界面
function GameViewLayer:showSettingLayer()
    SettingLayer:create():addTo(self.csbNode, ZORDER_LAYER.ZORDER_SURFACE)
end

--帮助界面
function GameViewLayer:showHelpLayer()
    HelpLayer:create():addTo(self.csbNode, ZORDER_LAYER.ZORDER_SURFACE)
end

--玩家列表
function GameViewLayer:showUserList(userInfo)
    local userList = self:getParentNode():getDataMgr():getChairUserList()
    if next(userList) == nil then  showToast(self, "目前没有玩家", 1) end
        --显示玩家列表信息
    if self.userListLayer then
        self.userListLayer:removeFromParent()
        self.userListLayer = nil
    end
    self.userListLayer = UserListLayer:create(userList,userInfo):addTo(self.csbNode, ZORDER_LAYER.ZORDER_SURFACE)
    
end

function GameViewLayer:setLinePlayer()
  -- body
    self.lineTxt:stopAllActions()
    schedule( self.lineTxt, function()
        local userList = self:getParentNode():getDataMgr():getUserList()
        self.lineTxt:setString(#userList)
    end, 1)
end

--发送重复下注
function GameViewLayer:sendReject()
    self._scene:sendContinueJetton()
end

--申请上庄
function GameViewLayer:sendApplyBanker()
    self._scene:sendApplyBanker()
end

--申请下庄
function GameViewLayer:sendCancelApply()
    self._scene:sendCancelApply()
end

--庄家非空闲下庄
function GameViewLayer:onGameMessageBankerQuit()
   if self.bankerListLayer then
      self.bankerListLayer:onGameMessageBankerQuit()
   end
end


--下注区域
function GameViewLayer:onJettonAreaClicked(tag)
    --保存上一次按钮状态
    self.m_lastJettonBtn = self.m_JettonBtn 
    print(self.gameState,GAME_CMD.GAME_SCENE_JETTON,"----------------11111---------")
    --非下注状态
    if self.gameState ~= GAME_CMD.GAME_SCENE_JETTON  then
        return 
    end

    local jettonScore = self.m_BetAmount

    local selfScore = (self.myAllJettonScore + jettonScore) * MaxTimes
    
    -- if selfScore > self.my_gold or self.lUserMaxScore < (self.myAllJettonScore + jettonScore) then
    if  jettonScore > self.my_gold or self.lUserMaxScore < (self.myAllJettonScore + jettonScore) then
        showToast(self, "已超过个人最大下注值", 1)
        -- self:setJettonEnable(false)
        return 
    end
    local areaScore = self.lUserInAllScore[tag] + jettonScore
    if areaScore > self.lAreaLimitScore then
        showToast(self,"已超过该区域最大下注值",1)
        -- self:setJettonEnable(false)
        return
    end
  print(jettonScore,tag-1,"-------------下注区域------------")
    self:upDateRejettonButton()
    self:getParentNode():SendPlaceJetton(jettonScore, tag-1)

    --判断是否需要重新加载按钮状态
    -- for i=1, #self.m_lastJettonBtn do
    --     if self.m_lastJettonBtn[i] ~= self.m_JettonBtn[i] then
    --         self:updateJettonList(self.lUserMaxScore)
    --         return
    --     end
    -- end
end

--坐下按钮
function GameViewLayer:onPlayerSite(tag)
    --头像的点击效果(弹出任务信息框)
    if self.siteUser[tag] then
        local useritem = self.siteUser[tag]:getUser()
        local x, y = self.siteButton[tag]:getPosition()
        local pos = cc.p(0,0)

        if tag <= 3 then
            pos = cc.p(x+250, y)
        else
            pos = cc.p(x-250, y)
        end 

        headFrame:create(useritem, pos):addTo(self.csbNode):setLocalZOrder(ZORDER_LAYER.ZORDER_HEADFRAME)
        return
    end


    if self.my_gold < self.lOccupySeatFree then
        showToast(self, "金币不足",1)
        return
    end

    local chair = self._scene:GetMeChairID()
    self._scene:sendOccupySeat(chair, tag-1)
end

--移除下注用户
function GameViewLayer:removeSiteUser()
end
------------------------------
function GameViewLayer:isMeChair( wchair )
    local useritem = self:getDataMgr():getChairUserList()[wchair + 1]
    if nil == useritem then
        return false
    else 
        return useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID
    end
end

--获得玩家的位置
function GameViewLayer:getUserPos( wchair )
    local userItem = self:getDataMgr():getChairUserList()[wchair + 1]
    if self:isMeChair(wchair) then
        return selfheadpoint
    end

    --庄家的位置
    if wchair  == yl.INVALID_CHAIR then
        return bankerGoldPos
    end

    for k,v in pairs(self.siteUser) do
        if userItem.dwUserID == v.dwUserID then
            return cc.p(self.siteButton[k]:getPosition())
        end
    end
    return userListPos
end

--获取数据
function GameViewLayer:getMeUserItem()
    return self:getParentNode():GetMeUserItem()
end

function GameViewLayer:getDataMgr( )
    return self:getParentNode():getDataMgr()
end

function GameViewLayer:getParentNode()
    return self._scene
end

------------------------------------------------------
--场景数据
function GameViewLayer:onGameSceneFree( cmd_table )
  dump(cmd_table,"--------------场景数据-------------",6)
    self:resetGameData()
    self:upDateJettonBtn(cmd_table.lChipArray[1])
    self.gameState = GAME_CMD.GAME_SCENE_FREE
    self.lUserMaxScore = cmd_table.lUserMaxScore
    self.cbTimeLeave = cmd_table.cbTimeLeave
    self.lAreaLimitScore = cmd_table.lAreaLimitScore
    self.lOccupySeatFree = cmd_table.lOccupySeatFree
    self.lApplyBankerCondition = cmd_table.lApplyBankerCondition
    self.wBankerUser = cmd_table.wBankerUser
    self.isHaveBanker = cmd_table.bEnableBanker

    self:setSelfInfo()
    self:upDateBankerInfo()
    self:setJettonEnable(false)
    self:updateJettonList(self.lUserMaxScore) 
    self:upDateRejettonButton()
    self:updateGameMessageOccupy(cmd_table)
    self:updateIsHaveBanker(self.isHaveBanker)

    local txt = self.csbNode:getChildByName("Node_applyBanker_0"):getChildByName("Text_1")
    txt:setString(ExternalFun.formatScoreText(self.lApplyBankerCondition))
end

function GameViewLayer:onGameScenePlaying( cmd_table, cbGameStatus )
  dump(cmd_table,"--------------场景数据-------111111111------",6)
    self.isRelink = true
    self:resetGameData()
    self:upDateJettonBtn(cmd_table.lChipArray[1])
    self.gameState = cbGameStatus
    self.cbTimeLeave = cmd_table.cbTimeLeave
    self.lAreaInAllScore = cmd_table.lAreaInAllScore[1] 
    self.lUserInAllScore = cmd_table.lUserInAllScore[1]    
    self.lUserMaxScore   = cmd_table.lUserMaxScore  
    self.lAreaLimitScore  = cmd_table.lAreaLimitScore   
    self.cbTableCardArray  = cmd_table.cbTableCardArray[1] 
    self.lUserWinScore = cmd_table.lUserWinScore[1]
    self.lUserScore  = cmd_table.lEndUserScore
    self.lBankerWinScore = cmd_table.lBankerWinScore
    self.lOccupySeatFree = cmd_table.lOccupySeatFree
    self.wOccupySeatChairIDArray = cmd_table.wOccupySeatChairIDArray[1]
    self.lApplyBankerCondition = cmd_table.lApplyBankerCondition
    self.wBankerUser = cmd_table.wBankerUser
    self.isHaveBanker = cmd_table.bEnableBanker

    self:setSelfInfo()
    self:upDateBankerInfo()

    if self.gameState == GAME_CMD.GAME_SCENE_JETTON  then
        self:updateJettonList(self.lUserMaxScore) 
    else
        self:setJettonEnable(false)
    end

    self:upDateRejettonButton()

    --所有玩家每个区域输赢
    self:getUserScoreInAllArea(cmd_table)
    self:updateGameMessageOccupy(cmd_table)
    self:updateIsHaveBanker(self.isHaveBanker)
    self:updateJettonNotice()

    self:updateAreaScore()

    --创建桌上的金币
    for i=1, 3 do
        local area = self.csbNode:getChildByName(string.format("Button_Area_%d", i) )
        local tmp = self:getGold(area, self.lAreaInAllScore[i])
        for k, v in pairs (tmp) do
            table.insert(self.goldJetton[i], v)
        end
    end

    local txt = self.csbNode:getChildByName("Node_applyBanker_0"):getChildByName("Text_1")
    txt:setString(ExternalFun.formatScoreText(self.lApplyBankerCondition))
end
---------------------------
--游戏数据
--游戏空闲
function GameViewLayer:onGameFree( cmd_table )
    self:resetGameData()
    self.gameState = GAME_CMD.GAME_SCENE_FREE
    self.cbTimeLeave = cmd_table.cbTimeLeave
    self.lUserInAllScore = {0,0,0}

    self:resetGameData()
    
    self:setJettonEnable(false)
    self:updateJettonNotice()
    self:setSelfInfo()

    --将庄家申请下庄置为可用
    if self.bankerListLayer then
        self.bankerListLayer:resetBtnState()
    end

    for i=1, 3 do

       local tmp = self.csbNode:getChildByName(string.format("lottery%d", i))
       tmp:setVisible(false)

    end
    self.isMyBet = false

    -- self.csbNode:getChildByName("Node_jettonNotice"):setVisible (false)
    for i=1, #self.cardArray do
        self.cardArray[i]:removeFromParent()
    end
    self.cardArray = {}
    self:createCardBack()

    if self.bankLayer and self:isMeChair(self.wBankerUser) then
        showToast(self,"现在是空闲时间，可以进行银行操作",1)
    end

end

--游戏开始，开始下注
function GameViewLayer:onGameStart(cmd_table)
   self.gameState = GAME_CMD.GAME_SCENE_JETTON
   self.cbTimeLeave = cmd_table.cbTimeLeave 
   self.lUserMaxScore = cmd_table.lUserMaxScore
   self.wBankerUser = cmd_table.wBankerUser 
   
   self:upDateBankerInfo()
   self:upDateRejettonButton()
   self:updateJettonList(self.lUserMaxScore)
   self:updateJettonNotice()
   self:showGameStart()
   self.isMyBet = false


    for i=1, 3 do

       local tmp = self.csbNode:getChildByName(string.format("lottery%d", i))
       tmp:setVisible(false)

     local btn = self.csbNode:getChildByName(string.format("Button_Area_%d", i))
      btn:setEnabled(false)

    end

end

--游戏结束
function GameViewLayer:onGameEnd( cmd_table )
    self.gameState = GAME_CMD.GAME_SCENE_END
    self.cbTimeLeave = cmd_table.cbTimeLeave
    self.cbTableCardArray = cmd_table.cbTableCardArray[1]
    --保存上一局自己下注信息
    self.m_lastRoundJettonScore = self.lUserInAllScore or {0,0,0}

    --玩家成绩
    self.lUserScore = cmd_table.lUserScore
    self.lBankerWinScore  = cmd_table.lBankerWinScore
    self:setJettonEnable(false)

    --所有玩家每个区域输赢
    self.lUserWinScore = cmd_table.lUserWinScore
    self:getUserScoreInAllArea(cmd_table)
    self:upDateRejettonButton()
    self:updateJettonNotice()

    self:upDateSelfInfo()
    self.isMyBet = false
    self:endJetton()
    
end

--筹码配置
function GameViewLayer:upDateJettonBtn(chouma)
    self.lChipArray = chouma
    self:updateBetAmount(self.lChipArray)
end

--用户下注
function GameViewLayer:onPlaceJetton(cmd_table)
    if self:isMeChair(cmd_table.wChairID) then
        local jettonScore = self.lUserInAllScore[cmd_table.cbJettonArea+1]
        self.lUserInAllScore[cmd_table.cbJettonArea+1] = jettonScore + cmd_table.lJettonScore
        self.my_gold = self.my_gold - cmd_table.lJettonScore
        self:upDateSelfInfo()
        self.myAllJettonScore = self.myAllJettonScore + cmd_table.lJettonScore

        if self.m_BetAmount > self.my_gold then 
            self:updateBetAmount()
        end

        --已下注后不能续压
        self.isReject = true;
    end

    local allJettonScore = self.lAreaInAllScore[cmd_table.cbJettonArea+1]
    self.lAreaInAllScore[cmd_table.cbJettonArea+1] = allJettonScore + cmd_table.lJettonScore 

    -- self:upDateUserJetton(cmd_table)
    self:updateAreaScore()

    
    
    self:upDateRejettonButton()

    --飞金币
    self:showGoldFlyJetton( cmd_table )
end

--更新是否坐庄
function GameViewLayer:updateIsHaveBanker(bool)
    -- self.csbNode:getChildByName("Node_applyBanker"):setVisible(bool)
    self.csbNode:getChildByName("Node_applyBanker_0"):setVisible(bool)
end

--下注失败
function GameViewLayer:onPlaceJettonFail(cmd_table)
    if self:isMeChair(cmd_table.wPlaceUser)  then
        --什么都不需要做
    end
end 

--申请上庄
function GameViewLayer:onApplyBanker(cmd_table)
    if self:isMeChair(cmd_table.wApplyUser) then
        if self.bankerListLayer then
            self.bankerListLayer:onApplyBanker()
        end
    end
    self:updateBankerList()
end

--申请下庄
function GameViewLayer:onCancelBanker(cmd_table)
    if self:isMeChair(cmd_table.wCancelUser) then
        if self.bankerListLayer then
            self.bankerListLayer:onCancelBanker()
        end
    end
    self:updateBankerList()
end

--切换庄家
function GameViewLayer:changeBanker(cmd_table)
    --如果以前自己坐庄，改变状态
    if self:isMeChair(self.wBankerUser) then
        if self.bankerListLayer then
            self.bankerListLayer:onCancelBanker()
        end
    end

    self.wBankerUser = cmd_table.wBankerUser

    --隐藏等待上庄
    if self.waitShangZhuang and self.wBankerUser ~= yl.INVALID_CHAIR  then
        self.waitShangZhuang:setVisible(false)
    else
        --等待上庄
       self:showWaitBnaker();
    end

    self:upDateBankerInfo()

    if self.bankerListLayer then
        self.bankerListLayer:changeBanker()
    end

    self._scene:removeBankerFromBankerList(self.wBankerUser)
    self:updateBankerList()

    self:updateBankerBetAmount()
end

--更新庄家列表
function GameViewLayer:updateBankerList()
    if self.bankerListLayer then
        self.bankerListLayer:updateBankerList()
    end
end

--更新用户列表
function GameViewLayer:refreshUserList()
    -- local userList = self:getParentNode():getDataMgr():getChairUserList()
    -- -- self:getParentNode():getDataMgr():getChairUserList()
    -- if next(userList) == nil then  showToast(self, "目前没有玩家", 1) end
    -- if self.userListLayer then
    --     self.userListLayer:refreshUserList(userList)
    -- end
end

--判断自己是否是庄家
function GameViewLayer:isMeIsBanker()
    return self:isMeChair(self.wBankerUser)
end

--判断自己是否在庄家列表中
function GameViewLayer:isMeInBankList()
    local userItem = self:getMeUserItem()
    if userItem then
        return self:getDataMgr():isMeInBankList(userItem)
    end
    return nil
end

--占位结果
function GameViewLayer:onGameMessageOccupy(cmd_table)
    local chair = cmd_table.wOccupySeatChairID
    local index = cmd_table.cbOccupySeatIndex + 1
    local str = string.format("Button_site_%d", index)
    local seat = self.csbNode:getChildByName(str)
    local size = seat:getContentSize()
    local x, y = seat:getPosition()
    local pos = cc.p(x, y+2)

    seat:setOpacity(0)

    local userItem = self:getDataMgr():getChairUserList()[chair+1]
    self.siteUser[index] = PlayerSite:create(userItem):addTo(self.csbNode):setPosition(pos):setLocalZOrder(ZORDER_LAYER.ZORDER_SITE)
    self.siteUser[index]:headAnimation()
    self.siteName[index]:setString(string.EllipsisByConfig(userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28)))
end

--占位失败
function GameViewLayer:onGameMessageOccupyFaild(cmd_table)
    -- dump(cmd_table)
    local str = ""
    if cmd_table.bSeatBusy then
        str = string.format("此位置已经被玩家%d占有", cmd_table.cbAlreadyOccupySeatIndex)

        for i=1, #cmd_table.wOccupySeatChairIDArray[1] do
            if cmd_table.wOccupySeatChairIDArray[1][i] == self._scene:GetMeChairID() then
                str = string.format("你已经占有%d号座位！", i)
            end
        end
    end

    showToast(self, str, 1)
end

--更新占位
function GameViewLayer:updateGameMessageOccupy(cmd_table)
    for i=1, GAME_CMD.MAX_OCCUPY_SEAT_COUNT do
         local chairId = cmd_table.wOccupySeatChairIDArray[1][i]
         if chairId == yl.INVALID_CHAIR then
            if nil ~= self.siteUser[i] then
                self.siteUser[i]:removeFromParent() 
                self.siteUser[i] = nil 

                local str = string.format("Button_site_%d", i)
                local seat = self.csbNode:getChildByName(str)
                seat:setOpacity(255)
                self.siteName[i]:setString("点击坐下")
            end
         else
              local userItem = self:getDataMgr():getChairUserList()[chairId+1]
              local str = string.format("Button_site_%d", i)
              local seat = self.csbNode:getChildByName(str)
              local size = seat:getContentSize()
              local x, y = seat:getPosition()
              local pos = cc.p(x, y+2)
              if self.siteUser[i] ~= nil then
                  self.siteUser[i].userInfo = userItem
              else
                  self.siteUser[i] = PlayerSite:create(userItem, i, pos):addTo(self.csbNode):setPosition(pos):setLocalZOrder(ZORDER_LAYER.ZORDER_SITE)
                  local str = string.format("Button_site_%d", i)
                  local seat = self.csbNode:getChildByName(str)
                  seat:setOpacity(0)
                  self.siteName[i]:setString(string.EllipsisByConfig(userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28)))
              end

         end
    end
end

--占位起立
function GameViewLayer:onGameMessageOccupyLeave(cmd_table)
    for i=1, GAME_CMD.MAX_OCCUPY_SEAT_COUNT do
         local chairId = cmd_table.wOccupySeatChairIDArray[1][i]
         if chairId == yl.INVALID_CHAIR then
            if nil ~= self.siteUser[i] then
                self.siteUser[i]:removeFromParent() 
                self.siteUser[i] = nil 

                local str = string.format("Button_site_%d", i)
                local seat = self.csbNode:getChildByName(str)
                seat:setOpacity(255)
                self.siteName[i]:setString("点击坐下")
            end
         end
    end
end

--简洁路单
function GameViewLayer:onGameMessageSimpleChart()
    local data = self.simplePoint
    self.simpleChart = {}
    for i=1, #data do
          local a = GameLogic.compare(data[i][1], data[i][2])
          if a == GAME_CMD.AREA_LONG then
            table.insert(self.simpleChart, GAME_CMD.AREA_LONG)
          elseif a == GAME_CMD.AREA_HU then
            table.insert(self.simpleChart, GAME_CMD.AREA_HU)
          elseif a == GAME_CMD.AREA_PING then
            table.insert(self.simpleChart, GAME_CMD.AREA_PING)
          end
    end
    self:updateSimpleChart()
end

--更新简洁路单
function GameViewLayer:updateSimpleChart()
    if next(self.simpleChart) == nil then
        --没有路单信息
        return 
    end

    local node = self.csbNode:getChildByTag(TAG_ENUM.BT_RECORD_BG)
    local startX = 27
    local startY = 23
    local gap = 52
    local index = #self.simpleChart >= 16 and 16 or #self.simpleChart
    node:removeAllChildren()
    for i=1, index do
        local count = #self.simpleChart - index
        local str = "" 
        if self.simpleChart[i+count] == GAME_CMD.AREA_LONG then
            str = "game/win_1.png"
        elseif self.simpleChart[i+count] == GAME_CMD.AREA_HU then
            str = "game/win_3.png"
        elseif self.simpleChart[i+count] == GAME_CMD.AREA_PING then
            str = "game/win_2.png"
        end

        local img = cc.Sprite:create(str)
        img:setPosition(cc.p(startX+gap*(i-1), startY))
        img:addTo(node)

        if index == i then
            local new = cc.Sprite:create("game/new.png")
            new:setPosition(cc.p(startX+gap*(i-1) - 6, startY + 6))
            new:addTo(node)

        end

    end

    if self.gameRecordLayer and self.gameRecordLayer:isVisible() then
        self.gameRecordLayer:freshSimpleChart(self.simpleChart)
    end
end

--高级路单
function GameViewLayer:onGameMessageAdvancedChart()
    local data = self.advancedPoint
    self.advancedChart = {}
    for i=1, #data do
          local a = GameLogic.compare(data[i][1], data[i][2])
          if a == GAME_CMD.AREA_LONG then
            table.insert(self.advancedChart, GAME_CMD.AREA_LONG)
          elseif a == GAME_CMD.AREA_HU then
            table.insert(self.advancedChart, GAME_CMD.AREA_HU)
          elseif a == GAME_CMD.AREA_PING then
            table.insert(self.advancedChart, GAME_CMD.AREA_PING)
          end
    end

    if self.gameRecordLayer and self.gameRecordLayer:isVisible() then
        self.gameRecordLayer:freshGameRecord(self.advancedChart)
    end
end

--重复下注
function GameViewLayer:onGameMessageReject(cmd_table)
    self.isReject = true
    self:upDateRejettonButton()
    -- self:updateJettonList( self.lUserMaxScore )

    -- local score = 0
    -- for i=1, 3 do
    --     self.lAreaInAllScore[i] = self.lAreaInAllScore[i] + cmd_table.lCurUserInAllScore[1][i]
    --     score = score + cmd_table.lCurUserInAllScore[1][i]

    --     self.lUserInAllScore[i] = self.lUserInAllScore[i] + cmd_table.lCurUserInAllScore[1][i]
    -- end

    -- for i=1, 3 do
    --     local data = {}
    --     -- data.wChairID = self._scene:GetMeChairID() 
    --     data.wChairID = cmd_table.wPlaceUser
    --     data.cbJettonArea = i-1
    --     data.lJettonScore = cmd_table.lCurUserInAllScore[1][i]
    --     if data.lJettonScore > 0 then
    --         self:goldFlyReject(data)
    --     end
    -- end

    --更新自己分数显示
    -- if self:isMeChair(cmd_table.wPlaceUser) then
    --     self.my_gold = self.my_gold - score
    --     self:upDateSelfInfo()
    -- end

    -- self:updateAreaScore()
end

--重复下注飞金币
function GameViewLayer:goldFlyReject(data)
   local tmp = {}
   local score = data.lJettonScore
   tmp.wChairID = data.wChairID
   tmp.cbJettonArea = data.cbJettonArea

    for i=#self.lChipArray, 1, -1 do
        local num = math.floor(score/self.lChipArray[i])
        local leftScore = score%self.lChipArray[i]
        tmp.lJettonScore = 0
        for j=1, num do
            tmp.lJettonScore = self.lChipArray[i]
            self:showGoldFlyJetton(tmp)
        end
        if leftScore < self.lChipArray[1] then
            return 
        else
            score = leftScore
        end
    end
   
    return
end

--更新区域分数显示
function GameViewLayer:updateAreaScore()
    local node = self.csbNode:getChildByName("Node_areaScore"):setLocalZOrder(ZORDER_LAYER.ZORDER_SCORE)
    for i=1, 3 do
        local str = string.format("areaScore_%d", i)
        local font_wan = string.format("areaScore_wan_%d", i)
        local font_yi = string.format("areaScore_yi_%d", i)



        if self.lAreaInAllScore[i] == 0 then
            node:getChildByName(str):setString("")
            node:getChildByName(font_wan):setVisible(false)
            node:getChildByName(font_yi):setVisible(false)
        else

            local lScore,isShowWan,isShowYi =  GAME_CMD.numTextFormat(self.lAreaInAllScore[i])
            node:getChildByName(font_wan):setVisible(isShowWan)
            node:getChildByName(font_yi):setVisible(isShowYi)

            node:getChildByName(str):setString(lScore)
            local size = node:getChildByName(str):getContentSize()
            local x = node:getChildByName(str):getPositionX()
            node:getChildByName(font_wan):setPositionX(x + size.width/2)
            node:getChildByName(font_yi):setPositionX(x + size.width/2)

        end

        local str = string.format("areaScore_me_%d", i)
        local str1 = string.format("score_sp_%d", i)
        local font_wan = string.format("areaScore_me_wan_%d", i)
        local font_yi = string.format("areaScore_me_yi_%d", i)

        if self.lUserInAllScore[i] == 0 then
            node:getChildByName(str):setString("")
            node:getChildByName(str1):setVisible(false)

            node:getChildByName(font_wan):setVisible(false)
            node:getChildByName(font_yi):setVisible(false)
        else

            local lScore,isShowWan,isShowYi =  GAME_CMD.numTextFormat(self.lUserInAllScore[i])
            -- print(lScore,isShowWan,isShowYi)
            node:getChildByName(font_wan):setVisible(isShowWan)
            node:getChildByName(font_yi):setVisible(isShowYi)
            if i == 2 then lScore = string.gsub(lScore,"%.","/") end
            node:getChildByName(str):setString(lScore)
            node:getChildByName(str1):setVisible(true)
            self.isMyBet = true

            local size = node:getChildByName(str):getContentSize()
            local x = node:getChildByName(str):getPositionX()
            node:getChildByName(font_wan):setPositionX(x + size.width)
            node:getChildByName(font_yi):setPositionX(x + size.width)



        end
    end
end

--刷新用户分数
function GameViewLayer:onGetUserScore( useritem )
    --自己
    if useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
        self.my_gold = useritem.lScore
        self:upDateSelfInfo()
    end
end

--获得所有玩家的输赢
function GameViewLayer:getUserScore(data)
    if #data.lUserWinScore[1] == GAME_CMD.GAME_PLAYER then
        for i=1, GAME_CMD.GAME_PLAYER do
            self.lUserWinScore[i] = data.lUserWinScore[1][i]
        end
    end
end

--获得所有玩家每个区域的输赢
function GameViewLayer:getUserScoreInAllArea(data)
    if #data.lUserAreaWinScore[1] == GAME_CMD.GAME_PLAYER*GAME_CMD.AREA_ALL then
        for i=1, GAME_CMD.GAME_PLAYER do
            self.lUserAreaWinScore[i] = {}
        end

        local index = 1
        for i=1, #data.lUserAreaWinScore[1] do
            table.insert(self.lUserAreaWinScore[index], data.lUserAreaWinScore[1][i])
            if i%GAME_CMD.AREA_ALL == 0 then
                index  = index + 1
            end
        end
    end
end

--判断下注按钮状态
function GameViewLayer:updateJettonList( score )
    -- body
    -- local judgescore = 0
    -- local judgeIndex = 0
    -- for i=1, #self.lChipArray do
    --     judgescore = self.lChipArray[i]*MaxTimes
    --     if judgescore > score then
    --         self.m_JettonBtn[i] = false
    --     else
    --         self.m_JettonBtn[i] = true
    --         judgeIndex = i
    --     end
    -- end

    -- if self:isMeChair(self.wBankerUser) then
    --     self:setJettonEnable(false)
    -- end

    -- if judgeIndex == 0 then
    --     self:setJettonEnable(false)
    -- end

    -- if self.m_nJettonSelect > judgeIndex then
    --     self.m_nJettonSelect = judgeIndex
    -- end

    -- if self.m_nJettonSelect == 0 then
    --     self.m_nJettonSelect = 1
    -- end
    -- self:upDateBTJetton()
end

--判断重复下注按钮
function GameViewLayer:upDateRejettonButton()
    local score = 0
    for i=1, #self.m_lastRoundJettonScore do
        score = self.m_lastRoundJettonScore[i] + score
    end

    --庄家
    if self:isMeChair(self.wBankerUser) then
        self.csbNode:getChildByTag(TAG_ENUM.BT_REJECT):setEnabled(false)
        return 
    end

    --游戏状态
    if self.gameState ~= GAME_CMD.GAME_SCENE_JETTON then
        self.csbNode:getChildByTag(TAG_ENUM.BT_REJECT):setEnabled(false)
        return 
    end

    --只能点击一次
    if self.isReject then
        self.csbNode:getChildByTag(TAG_ENUM.BT_REJECT):setEnabled(false)
        return
    end

    --上局未下注
    if score == 0 then
        self.csbNode:getChildByTag(TAG_ENUM.BT_REJECT):setEnabled(false)
        return
    end

    --分数不够
    if self.my_gold < score then
        self.csbNode:getChildByTag(TAG_ENUM.BT_REJECT):setEnabled(false)
        return
    end

    self.csbNode:getChildByTag(TAG_ENUM.BT_REJECT):setEnabled(true)
end

--所有下注按钮是否可以点击
function GameViewLayer:setJettonEnable(bool)
    for i=1, #self.m_JettonBtn do
        self.m_JettonBtn[i] = bool
    end

    if not bool then
        self.m_nJettonSelect = 0
    end
    self:upDateBTJetton()
end

--请下注是否显示
function GameViewLayer:updateJettonNotice()
    local label = self.csbNode:getChildByName("Node_jettonNotice"):setLocalZOrder(ZORDER_LAYER.ZORDER_JETTONNOTICE)
    if self.gameState == GAME_CMD.GAME_SCENE_JETTON then
        label:getChildByName("Image"):setTexture("game/image20.png")
        -- label:setVisible(true)
    elseif self.gameState == GAME_CMD.GAME_SCENE_FREE then
        label:getChildByName("Image"):setTexture("game/image18.png")
        -- label:setVisible(true)
    elseif self.gameState == GAME_CMD.GAME_SCENE_END then 
        label:getChildByName("Image"):setTexture("game/image19.png")
        -- label:setVisible(false)
    end
end

function GameViewLayer:OnUpdataClockView(chair, time)
    local label = self.csbNode:getChildByName("Node_jettonNotice")
    label:getChildByName("time"):setString(time)
    
    if self.gameState == GAME_CMD.GAME_SCENE_JETTON then 
        
    elseif self.gameState == GAME_CMD.GAME_SCENE_FREE then
        -- label:getChildByName("time"):setString(time-5)
    end

    if not self.isRelink then
        if self.gameState == GAME_CMD.GAME_SCENE_FREE then
            if 4 == time then
                -- self.csbNode:getChildByName("Node_jettonNotice"):setVisible (false)
                -- for i=1, #self.cardArray do
                --     self.cardArray[i]:removeFromParent()
                -- end
                -- self.cardArray = {}
                -- self:createCardBack()
            end
        elseif self.gameState == GAME_CMD.GAME_SCENE_JETTON then
            -- if 1 == time then
            --     --播放停止下注动画 
            --     self:setJettonEnable(false)
            --     self.csbNode:getChildByTag(TAG_ENUM.BT_REJECT):setEnabled(false)
            --     self:endJetton()
            -- end
        elseif self.gameState == GAME_CMD.GAME_SCENE_END then

        end
    else
        --断线重连
        self.isRelink = false
        if self.gameState == GAME_CMD.GAME_SCENE_JETTON then
            self:showCardBack(true)
            if time > 2 then
                -- self:startJetton()
            elseif time == 1 then
                -- self:endJetton()
            end
        elseif  self.gameState == GAME_CMD.GAME_SCENE_FREE then
            self:showCardBack(true)
            if time < 5 then
                -- self.csbNode:getChildByName("Node_jettonNotice"):setVisible (false)
            end
            if time == 0 then
                -- self:startJetton()
            end
        else
            --显示等待下一局
            self:showPleaseWait()
            if self.myAllJettonScore > 0 then
                self:showGameEndResult()
            end
        end
    end
end

--请等待下一局显示
function GameViewLayer:showPleaseWait()
    self.waitNextRound = ExternalFun.loadCSB("gameWait.csb", self) 

    local dataiallAction = ExternalFun.loadTimeLine("gameWait.csb")
    dataiallAction:gotoFrameAndPlay(0, true)
    self.waitNextRound:runAction(dataiallAction)
    -- dataiallNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.5), cc.CallFunc:create(function()
    --   dataiallNode:removeFromParent()
    --   end)))

    -- local move1 = cc.MoveTo:create(0.2,cc.p(829.85))

end

--大联盟等待上庄显示
function GameViewLayer:showWaitBnaker()
    if GlobalUserItem.dwCurRoomGroupID ~= 0 and self.wBankerUser == yl.INVALID_CHAIR then
        self.waitShangZhuang = ExternalFun.loadCSB("gameWait.csb", self) 
        local Panel_1 = self.waitShangZhuang:getChildByName("Panel_1");
        Panel_1:setVisible(false);
        local wait_2_2 = self.waitShangZhuang:getChildByName("wait_2_2");
        local wait_2_3 = self.waitShangZhuang:getChildByName("wait_2_3");
        local wait_2_4 = self.waitShangZhuang:getChildByName("wait_2_4");
        local wait_2_5 = self.waitShangZhuang:getChildByName("wait_2_5");
        wait_2_2:setTexture("game/image61.png")
        wait_2_3:setTexture("game/image62.png")
        wait_2_4:setTexture("game/image63.png")
        wait_2_5:setTexture("game/image64.png")

        local dataiallAction = ExternalFun.loadTimeLine("gameWait.csb")
        dataiallAction:gotoFrameAndPlay(0, true)
        self.waitShangZhuang:runAction(dataiallAction)
    end
    
end

--龙vs虎动画
function GameViewLayer:showGameStart()
    ExternalFun.playSoundEffect("vs.mp3") 
    self.csbNode:getChildByName("Node_jettonNotice"):setVisible(false)
    local spine = SpineObj:create("animation/startGame/skeleton.json", "animation/startGame/skeleton.atlas", false)
    spine:setAnimation(0, "animation", false)
    spine:registerSpineEventHandler(function (event)
      spine:destroy()
      self:startJetton()
    end, 2)
    spine:setPosition(display.cx,display.cy+200)
    self._animateLayer:addChild(spine)

end

--开始下注动画
function GameViewLayer:startJetton()
    ExternalFun.playSoundEffect("show.mp3") 
    ExternalFun.playSoundEffect("start.mp3") 

    self.startJettonAnimate  = sp.SkeletonAnimation:create("animation/startJetton/skeleton.json", "animation/startJetton/skeleton.atlas", 1)
    self.startJettonAnimate:setPosition(display.cx,display.cy)
    self.startJettonAnimate:setAnimation(0, "animation", false)
    self.startJettonAnimate:addTo(self._animateLayer)
    self.startJettonAnimate:registerSpineEventHandler(function (event)
        self.csbNode:getChildByName("Node_jettonNotice"):setVisible(true)
        self:updateBetAmount()

        for i=1,3 do
          local btn = self.csbNode:getChildByName(string.format("Button_Area_%d", i))
          btn:setEnabled(true)
        end

    end, 2)




end

--停止下注动画
function GameViewLayer:endJetton()

    for i=1,6 do
        local btn = self.csbNode:getChildByName("Button_Amount_"..i)
        btn:setEnabled(false)

        local effect = btn:getChildByName("effect")
        if effect then effect:removeFromParent() end
        
    end

    ExternalFun.playSoundEffect("show.mp3") 
    ExternalFun.playSoundEffect("stop.mp3") 
    self.endJettonAnimate  = sp.SkeletonAnimation:create("animation/endJetton/skeleton.json", "animation/endJetton/skeleton.atlas", 1)
    self.endJettonAnimate:setPosition(display.cx,display.cy)
    self.endJettonAnimate:setAnimation(0, "animation", false)
    self.endJettonAnimate:addTo(self._animateLayer)
    self.endJettonAnimate:registerSpineEventHandler(function (event)

        self:showCardValue()
    end, 2)


end

--胜利动画
function GameViewLayer:winAnimate()
    -- ExternalFun.playSoundEffect("wingame.mp3") 
    -- self.endJettonAnimate  = sp.SkeletonAnimation:create("animation/win/skeleton.json", "animation/win/skeleton.atlas", 1)
    -- self.endJettonAnimate:setPosition(display.cx,display.cy)
    -- self.endJettonAnimate:setAnimation(0, "animation", false)
    -- self.endJettonAnimate:addTo(self.resultLayer)
end

--失败动画
function GameViewLayer:failAnimate()
    -- ExternalFun.playSoundEffect("failgame.mp3") 
    -- self.endJettonAnimate  = sp.SkeletonAnimation:create("animation/fail/skeleton.json", "animation/fail/skeleton.atlas", 1)
    -- self.endJettonAnimate:setPosition(display.cx,display.cy)
    -- self.endJettonAnimate:setAnimation(0, "animation", false)
    -- self.endJettonAnimate:addTo(self.resultLayer)
end

--创建牌背面
function GameViewLayer:createCardBack()
    local img = CardSprite:createCard():addTo(self.csbNode, ZORDER_LAYER.ZORDER_CARD):setPosition(GameViewLayer.cardPos[1])
    local img1 = CardSprite:createCard():addTo(self.csbNode, ZORDER_LAYER.ZORDER_CARD):setPosition(GameViewLayer.cardPos[2])
    img:showCardBack(true)
    img1:showCardBack(true)

    table.insert(self.cardArray, img)
    table.insert(self.cardArray, img1)
end

--显示牌背面
function GameViewLayer:showCardBack(bool)
    for i=1, #self.cardArray do
        self.cardArray[i]:showCardBack(bool)
    end
end

--显示牌
function GameViewLayer:showCardValue()
    for i=1, #self.cardArray do
        self.cardArray[i]:runAction(cc.Sequence:create(cc.DelayTime:create(0.01+(i-1)*1), cc.CallFunc:create(function()
                  self.cardArray[i]:showCardBack(false)
                  self.cardArray[i]:setCardValue(self.cbTableCardArray[i])
                  local isHuWin = GameLogic.compare(self.cbTableCardArray[1], self.cbTableCardArray[2]) == GAME_CMD.AREA_HU
                  self.cardArray[i]:openCardAnimation(i,isHuWin )
              end)))

        if i==2 then
            self:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function() 

                  local result = GameLogic.compare(self.cbTableCardArray[1], self.cbTableCardArray[2])
                  local str = ""
                  local str1 = ""
                  if result == GAME_CMD.AREA_LONG then
                      str = "long_win.mp3"
                      str1 = "DragonCall.mp3"
                      if self.cardArray[1] ~= nil then self.cardArray[1].effect:setVisible(true) end
                  elseif result == GAME_CMD.AREA_PING then
                      str = "he.mp3"
                      if self.cardArray[1] ~= nil then self.cardArray[1].effect:setVisible(true) end
                      if self.cardArray[2] ~= nil then self.cardArray[2].effect:setVisible(true) end
                  elseif result == GAME_CMD.AREA_HU then
                      str = "hu_win.mp3"
                      str1 = "TigerCall.mp3"
                      if self.cardArray[2] ~= nil then self.cardArray[2].effect:setVisible(true) end
                  end
                  ExternalFun.playSoundEffect(str)
                  if str1 ~= "" then ExternalFun.playSoundEffect(str1) end
                  local pos = {cc.p(371,279),cc.p(678,279),cc.p(986,279)}
                  -- ExternalFun.playSoundEffect("win_bet.mp3")
                  local areaIndex = result + 1
                  for i=1, 3 do
                      if i == areaIndex then
                          local spine = SpineObj:create("animation/lottery"..i.."/skeleton.json", "animation/lottery"..i.."/skeleton.atlas", false)
                          spine:setAnimation(0, "animation", false)
                          spine:setPosition(pos[i])
                          self._animateLayer:addChild(spine)
                          spine:registerSpineEventHandler(function (event) 
                              spine:destroy() 
                              self:gameEndGoldFly(areaIndex)
                              self:onGameMessageSimpleChart()
                          end, 2)
                      end
                  end

                  for i=1, 3 do
                      if i == areaIndex then
                         local tmp = self.csbNode:getChildByName(string.format("lottery%d", i))
                         tmp:setVisible(true)
                         tmp:runAction(cc.Sequence:create(cc.Blink:create(2,6), cc.CallFunc:create(function () tmp:setVisible(false) end)))
                      end
                  end


            end)))
        end
    end
end

function GameViewLayer:cleanGameData()
end

function GameViewLayer:showPopWait( )
    self:getParentNode():showPopWait()
end

-- --银行操作成功
function GameViewLayer:onBankSuccess( )
    local bank_success = self:getParentNode().bank_success
    if nil == bank_success then
        return
    end
    GlobalUserItem.tabAccountInfo.lUserScore = bank_success.lUserScore
    GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure

    if self.bankLayer then
        self.bankLayer:refreshBankScore()
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
    bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
    if self.bankLayer then
        self.bankLayer:refreshBankRate(bankinfo)
    end
end

--移除银行
function GameViewLayer:removeBankLayer()
    if self.bankLayer then
        self.bankLayer:removeFromParent()
        self.bankLayer = nil
    end
end

--重置游戏数据
function GameViewLayer:resetGameData()
    self.lAreaInAllScore = {0,0,0}
    self.lUserInAllScore = {0,0,0}
    self.lUserScore = 0
    self.lBankerWinScore = 0
    self.lUserMaxScore = 0
    self.myAllJettonScore = 0
    self.isReject = false

    --移除所有金币
    if self.goldLayer ~= nil then
        self.goldLayer:removeAllChildren()
    end

    self.goldJetton = {{}, {}, {}}

    if self._animateLayer ~= nil then
        self._animateLayer:stopAllActions()
        self._animateLayer:removeAllChildren()
    end

    if self.waitNextRound then
        self.waitNextRound:setVisible(false)
    end

    if self.waitShangZhuang then
        self.waitShangZhuang:setVisible(false)
    end

    -- if self.resultLayer then
    --     self.resultLayer:stopAllActions()
    --     self.resultLayer:removeFromParent()
    --     self.resultLayer = nil
    -- end

    if self.timer then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timer)
        self.timer = nil
    end

    if next(self.resultScore) ~= nil then
      for k, v in pairs(self.resultScore) do
          if nil ~= v then
              v:removeFromParent()
          end
      end
      self.resultScore  = {}
    end

    self.lUserWinScore = {}

    self:updateAreaScore()
end
-----------------------------------------------------------------------------------------------
--金币
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


    -- local beaction = cc.MoveTo:create(0.42, endpos)
    -- if inorout == 0 then
    --     return cc.EaseOut:create(beaction, 1)
    -- else
    --     return cc.EaseIn:create(beaction, 1)
    -- end
end

--获取随机显示位置
function GameViewLayer:getRandPos(nodeArea, index)
    local beginpos = cc.p(nodeArea:getPositionX(), nodeArea:getPositionY())
    
    local offsetx = math.random(-90,90)
    local offsety = math.random(-90,90)

    return cc.p(beginpos.x + offsetx, beginpos.y + offsety)
end

--创建金币
function GameViewLayer:createGold(index)
    local str = string.format("game/bt_jetton_%d.png", index)
    local bg = cc.Sprite:create(str)
    local label = self:createJettonText(self.lChipArray[index])
    label:setAnchorPoint(cc.p(0.5,0.5))
    label:setPosition(bg:getContentSize().width/2,bg:getContentSize().height/2+7)
    bg:addChild(label)
    return bg
end

local bet_time = 0
--下注飞金币
function GameViewLayer:showGoldFlyJetton(cmd_table)
    local score = cmd_table.lJettonScore
    -- ExternalFun.playSoundEffect("bet.mp3")
    --if cmd_table.wChairID == self._scene:GetMeChairID() then 
    ExternalFun.playSoundEffect("bet.mp3")
    --end  

    for i=1, #self.lChipArray do
        if score == self.lChipArray[i] then
            local img = self:createGold(i)
            img:setScale(0.3)
            img:addTo(self.goldLayer)
            table.insert(self.goldJetton[cmd_table.cbJettonArea+1], img)
            local beginpos = self:getUserPos(cmd_table.wChairID)
            img.beginPos = beginpos
            local area = self.csbNode:getChildByName(string.format("Button_Area_%d", cmd_table.cbJettonArea+1) )
            local endpos = self:getRandPos(area, cmd_table.cbJettonArea+1)



            local move = cc.MoveTo:create(0.3, endpos)
            local action = cc.EaseOut:create(move, 1)


            img:setPosition(beginpos)
            img:runAction(action)
            img:runAction(cc.Sequence:create(cc.ScaleTo:create(0.3,0.55),cc.ScaleTo:create(0.12,0.45),cc.CallFunc:create(
                function()
                    if cmd_table.wChairID == self._scene:GetMeChairID() then 
                        ExternalFun.playSoundEffect("sound-bet.mp3")
                    end  
                end
            )))

            -- if cmd_table

        end
    end

    --头像动画
    for k,v in pairs(self.siteUser) do
        local str = string.format("Button_site_%d", k)
        local seat = self.csbNode:getChildByName(str)
        local useritem = v:getUser()
        if useritem.wChairID == cmd_table.wChairID then
            if not self:isMeChair(cmd_table.wChairID) then
                local x,y = v:getPosition()
                if k < 4 then
                    local action = cc.Sequence:create(cc.MoveBy:create(0.08, cc.p(30, 0)), cc.MoveBy:create(0.08, cc.p(-30, 0)))
                    local action1 = cc.Sequence:create(cc.MoveBy:create(0.08, cc.p(30, 0)), cc.MoveBy:create(0.08, cc.p(-30, 0)))
                    v:runAction(action)
                    seat:runAction(action1)
                else
                   local action = cc.Sequence:create(cc.MoveBy:create(0.08, cc.p(-30, 0)), cc.MoveBy:create(0.08, cc.p(30, 0)))
                   local action1 = cc.Sequence:create(cc.MoveBy:create(0.08, cc.p(-30, 0)), cc.MoveBy:create(0.08, cc.p(30, 0)))
                    v:runAction(action)
                    seat:runAction(action1)
                end
            end
            if self:isMeChair(useritem.wChairID) then
              v:updateScore(self.my_gold)
            else
              v:updateScore(useritem.lScore-cmd_table.lJettonScore)
            end
        end
    end
end

--根据分数创建金币
function GameViewLayer:getGold(area, score)

    local gold = {}
    for i=#self.lChipArray, 1, -1 do
        local num = math.floor(score/self.lChipArray[i])
        local leftScore = score%self.lChipArray[i]

        for j=1, num do
            local pos = self:getRandPos(area)
            local img = self:createGold(i)
            img:setScale(0.5)
            img:setPosition(pos)
            img.beginPos = userListPos
            img:addTo(self.goldLayer)
            table.insert(gold, img)
        end

        if leftScore < self.lChipArray[1] then
            return gold
        else
            score = leftScore
        end
    end
    return gold
end

--金币飞向胜利区域
function GameViewLayer:gameEndGoldFly(areaIndex)
    --显示胜利动画  
    local act_all_time = 0
    local isRun = false
    for i=1,#self.goldJetton do
        local act_time = 0
        for j=1,#self.goldJetton[i] do
            local chip = self.goldJetton[i][j]
            local beginpos = cc.p(chip:getPosition())
            local endpos = bankerGoldPos
            -- local moveaction = self:getMoveAction(beginpos, endpos, 0, 0)
            local moveaction = cc.MoveTo:create(0.4,endpos)
            isRun = true
            self.goldJetton[i][j]:runAction(cc.Sequence:create({cc.DelayTime:create(act_time),moveaction,cc.CallFunc:create(function()
                --if areaIndex ~= i then 
                    chip:setVisible(false)
                --end
            end)}))
            act_time = act_time + 0.01
            if act_all_time < 0.4 + act_time then
                act_all_time = 0.4 + act_time
            end
        end

    end
    if isRun then ExternalFun.playSoundEffect("sound-popscore.mp3") end

     local local_time = 0
    self:runAction(cc.Sequence:create(cc.DelayTime:create(act_all_time+0.4),cc.CallFunc:create(function()
        -- body
        local isRun = false
        for i=1, 3 do
            for j=1, #self.goldJetton[i] do 
                local area = self.csbNode:getChildByName(string.format("Button_Area_%d", areaIndex) )
                local beginpos = cc.p(self.goldJetton[i][j]:getPosition())
                local endpos = self:getRandPos(area)
                local moveaction = cc.MoveTo:create(0.4,endpos)
                self.goldJetton[i][j]:setScale(0.4)
                -- local moveaction = self:getMoveAction(beginpos, endpos, 0, 0)
                self.goldJetton[i][j]:setVisible(true)

                self.goldJetton[i][j]:runAction(cc.Sequence:create(cc.DelayTime:create(local_time),moveaction))
                isRun = true
                local_time = local_time + 0.01
            end
        end
        if isRun then ExternalFun.playSoundEffect("sound-result-jetton.mp3") end
    end),cc.DelayTime:create(local_time+1),cc.CallFunc:create(function()
        -- body
        local_time = 0
        local isRun = false
        for i=1, 3 do
            for j=1, #self.goldJetton[i] do 
                local area = self.csbNode:getChildByName(string.format("Button_Area_%d", areaIndex) )
                local beginpos = cc.p(self.goldJetton[i][j]:getPosition())
                local endpos = self.goldJetton[i][j].beginPos
                -- local moveaction = self:getMoveAction(beginpos, endpos, 0, 0)
                local moveaction = cc.MoveTo:create(0.4,endpos)
                self.goldJetton[i][j]:runAction(cc.Sequence:create(cc.DelayTime:create(local_time),moveaction,cc.CallFunc:create(
                    function()
                        self.goldJetton[i][j]:setVisible(false)
                    end
                )))

                isRun = true
                local_time = local_time + 0.01
            end
        end
        if isRun then ExternalFun.playSoundEffect("sound-result-jetton.mp3") end
    end),cc.DelayTime:create(local_time+0.4),cc.CallFunc:create(function()

        local winNum = 0
        local result = GameLogic.compare(self.cbTableCardArray[1], self.cbTableCardArray[2])
        local userList = self:getDataMgr():getChairUserList()
        local myChair  = self._scene:GetMeChairID()
        local myDwUserId = self:getMeUserItem().dwUserID
        self:showScore(self.lUserScore, 0)
        for i=1, 6 do
            if self.siteUser[i] ~= nil then
                local tmpUserItem = self.siteUser[i].userInfo
                --找到下注玩家
                if tmpUserItem ~= nil then
                    if self.lUserWinScore and self.lUserWinScore[1] then 
                        for k, v in pairs(self.lUserWinScore[1]) do
                            if tmpUserItem.wChairID + 1 == k then
                                local score = v
                                self:showScore(score, i)
                            end
                        end
                    end
                end
            end
        end
        if not self:isMeChair(self.wBankerUser) and self.wBankerUser ~= yl.INVALID_CHAIR then
            self:showScore(self.lBankerWinScore, 7)
        end 
        -- print()
        if self.banker_score then 
          self.banker_score = self.banker_score + self.lBankerWinScore
          self:updateBankerMoney(self.banker_score)
        end
    end)))
end

function GameViewLayer:showScore(winScore, chairIndex)

    local tmpCsb = nil
    local parentNode = nil
    
    tmpCsb = ExternalFun.loadCSB("Node_score.csb", self.csbNode)
    tmpCsb:setLocalZOrder(ZORDER_LAYER.ZORDER_ANIMATE)

    local node = tmpCsb:getChildByName("Node_score")
    local text_wan = nil
    local text_yi = nil

    local label = nil
    if winScore > 0 then
        label = node:getChildByName("AtlasLabel_score")
        text_wan = node:getChildByName("win_wan")
        text_yi = node:getChildByName("win_yi")
    elseif winScore < 0 then
        label = node:getChildByName("AtlasLabel_score1")
        text_wan = node:getChildByName("lose_wan")
        text_yi = node:getChildByName("lose_yi")

    else
        tmpCsb:removeFromParent()
        return
    end

    winScore = math.abs(winScore)

    local score,isShowWan,isShowYi = GAME_CMD.numTextFormat(winScore)
    text_wan:setVisible(isShowWan)
    text_yi:setVisible(isShowYi)
    winScore = string.gsub(score,"%.","%/")
    label:setString("."..winScore)

    text_wan:setPositionX(label:getPositionX()+label:getContentSize().width)
    text_yi:setPositionX(label:getPositionX()+label:getContentSize().width)

    local tmpSize = label:getContentSize()
    node:setContentSize(cc.size(tmpSize.width, tmpSize.height))

    local move = cc.MoveBy:create(1, cc.p(0, 30))
    if chairIndex == 7  then
        tmpCsb:setPosition(328,519)
        move = cc.MoveBy:create(1, cc.p(0, 60))
    elseif chairIndex == 0 then 
        parentNode = self.csbNode:getChildByName("selfinfo"):getChildByName("Button_headFrame")
        local x,y = parentNode:getPosition()
        tmpCsb:setPosition(x+20,y)
        move = cc.MoveBy:create(1, cc.p(0, 60))
    else
        parentNode = self.siteButton[chairIndex]
        local x,y = parentNode:getPosition()
        tmpCsb:setPosition(x,y)
        --坐下玩家
        if chairIndex >= 4 and chairIndex <= 6 then
            label:setAnchorPoint(cc.p(1, 0.5))
            label:setPosition(cc.p(0, 0))
            text_wan:setPositionX(label:getPositionX()+1)
            text_yi:setPositionX(label:getPositionX()+1)
        end
    end

    if node ~= nil then
        ExternalFun.playSoundEffect("sound-gold.mp3")
        node:runAction(cc.Sequence:create(move,cc.DelayTime:create(1.5),cc.CallFunc:create(function ()
          -- body
            node:setVisible(false)
        end)))
        table.insert(self.resultScore, node)
    end
end

--显示结算界面
function GameViewLayer:showGameEndResult()
    
end

--下注额度筹码配置
function GameViewLayer:updateBetAmount(  )
  -- body

    for i=1,6 do
        local btn = self.csbNode:getChildByName("Button_Amount_"..i)
        local textLayout = btn:getChildByName("textLayout")
        if textLayout then textLayout:removeFromParent() end
        local textLayout = self:createJettonText(self.lChipArray[i])
        textLayout:setAnchorPoint(cc.p(0.5,0.5))
        textLayout:setPosition(btn:getContentSize().width/2,btn:getContentSize().height/2+8)
        btn:addChild(textLayout)
        textLayout:setName("textLayout")
        -- textLayout:setScale(0.8)
        btn:addClickEventListener(function()
          -- body
          self:onBtnBetAmount(i)
        end)

        if self.lChipArray[i] > self.my_gold then 
            btn:setEnabled(false)
        else
            btn:setEnabled(true)
        end
    end

    self:onBtnBetAmount(1)
end

function GameViewLayer:updateBankerBetAmount()

    if self:isMeIsBanker() then 
        for i=1,6 do
            local btn = self.csbNode:getChildByName("Button_Amount_"..i)
            btn:setEnabled(false)

            local effect = btn:getChildByName("effect")
            if effect then effect:removeFromParent() end

        end
    else
        self:updateBetAmount()
    end
end

function GameViewLayer:onBtnBetAmount(index )
  -- body

    local score = self.lChipArray[index]

    if score > self._scene:GetMeUserItem().lScore then 
        return showToast(self, "您的金币不足！", 1)
    end

    for i=1,6 do
        local btn = self.csbNode:getChildByName("Button_Amount_"..i)
        local effect = btn:getChildByName("effect")
        if effect then effect:removeFromParent() end
    end

    local btn = self.csbNode:getChildByName("Button_Amount_"..index)
    if index and  btn:isEnabled() then
        local effect = cc.Sprite:create("game/jettoneffect.png") 
        effect:setScale(1.20)
        effect:setPosition(btn:getContentSize().width/2,btn:getContentSize().height/2+7)
        effect:setName("effect")
        btn:addChild(effect)
        effect:runAction(cc.RepeatForever:create(cc.Blink:create(1.0,1)))
    end

    self.m_BetAmount = self.lChipArray[index]
end

function GameViewLayer:setSystemBanker(isSystem)
  -- body

    local enough_large = self.bankerNode:getChildByName("enough_large")
    self.bankerNode:getChildByName("title_3"):setVisible(not isSystem)
    self.bankerNode:getChildByName("AtlasLabel_gold"):setVisible(not isSystem)
    self.bankerNode:getChildByName("Text_name"):setVisible(not isSystem)
    enough_large:setVisible(isSystem)

    local long = enough_large:getChildByName("long")
    local hu = enough_large:getChildByName("hu")

    long:stopAllActions()
    hu:stopAllActions()

    local skewto1 = cc.SkewTo:create(1, 3, 3)
    local skewto2 = cc.SkewTo:create(1, 0, 0)
    long:runAction(cc.RepeatForever:create(cc.Sequence:create({skewto1,skewto2})))

    local skewto1 = cc.SkewTo:create(1, 3, 3)
    local skewto2 = cc.SkewTo:create(1, 0, 0)
    hu:runAction(cc.RepeatForever:create(cc.Sequence:create({skewto1,skewto2})))
end



return GameViewLayer

