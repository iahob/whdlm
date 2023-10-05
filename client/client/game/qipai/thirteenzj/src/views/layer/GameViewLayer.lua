--//////////////////////////////////////////////////////////////////////////////////////
-- Author: Tang
-- Date: 2016-12-08 15:41:53
--/////////////////////////////////////////////////////////////////////////////////////
local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.qipai.thirteenzj.src"
local RES_PATH = "game/qipai/thirteenzj/res/"

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local CardSprite = appdf.req(module_pre..".views.layer.CardSprite")
local SelectLayer = appdf.req(module_pre..".views.layer.SelectLayer")
local GameLogic = appdf.req(module_pre..".models.GameLogic")
local cmd = appdf.req(module_pre..".models.cmd_game")
local GameChatLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local SpineObj = appdf.req("client.src.external.SpineObj")

local QueryDialog   = appdf.req("app.views.layer.other.QueryDialog")
GameViewLayer.Enum = 
{
   COLOR_BTN       = 1,
   COLOR_SELECT_OK = 6,
   READY_BTN       = 7,
   CLOCK           = 8,
   MENU            = 9,   --菜单
   MENU_CHAT       = 10,  --聊天
   MENU_HELP       = 11,  --帮助
   MENU_SET        = 12,  --设置
   MENU_TRUSTTEE   = 13,  --托管
   MENU_QUIT       = 14,  --退出
   MENU_CHANGE      = 15,--换桌
   BTN_LOCATION      = 16,--定位
   PLAYER_CARD     = 100,
   SET_VIEW        = 200,
   CHAT_VIEW       = 1001, -- 
   BT_GAME_START   = 21,    --游戏开始
   BT_PLAYER_SIT   = 22,    --坐下按钮
}

local TAG = GameViewLayer.Enum
GameViewLayer.TopZorder = 30
GameViewLayer.MenuZorder = 20
GameViewLayer.SelectZorder = 21
GameViewLayer.TipsZorder = 35

local seatpos = {cc.p(500,90),cc.p(800,350),cc.p(420,220),cc.p(160,350)}
local scorecell = {100, 1000, 10000, 100000}
local goldnum = {12, 13, 15, 15, 15 }

--飞出多少金币
function GameViewLayer:getgoldnum(score)
  local cellvalue = 1
  for i=1,4 do
    if score > scorecell[i] then
      cellvalue = i + 1
    end
  end
  return goldnum[cellvalue]
end


function GameViewLayer:priGameLayerZorder()
    return GameViewLayer.TopZorder - 1
end

function GameViewLayer:dismissResult()
  self._bDismissed = true
end

function GameViewLayer:ctor(scene)

  self._scene = scene
  self._userList = {}
  self._bMenu = false
  self._bTrustTee = false
  self._bDismissed = false

  self._bSound = GlobalUserItem.bSoundAble
  self._bMusic = GlobalUserItem.bVoiceAble

  --是否发牌完成
  self._bComSend = false
  --是否获取推荐数据
  self._bGetTypeData = false
  --牌组
  self._CardList = {{},{},{},{}}
  self._exitScore = {0,0,0,0}

  self._bCompareCard = false

  --金币数
  self._goldlist = {}
  --头像位置
  self._seatposlist = {}

  self:gameDataInit()
  --导入资源
  self:loadRes()
  	--初始化csb界面
  self:initCsbRes()

  --注册事件
  --注册node事件
  ExternalFun.registerNodeEvent(self)

  ExternalFun.playBackgroudAudio("music_game.mp3")

end

function GameViewLayer:onExit()
  self:gameDataReset()
  self.m_actTip:release()
  self.m_actTip = nil
  self.m_actVoiceAni:release()
  self.m_actVoiceAni = nil

    if self._schSingal then
          cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._schSingal)
          self._schSingal = nil
  end

    if self._beginTimeId then
      cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
      self._beginTimeId = nil
    end

end

function GameViewLayer:resetData()
    self._bTrustTee = false
    self._bDismissed = false
    self._exitScore = {0,0,0,0}
end

function GameViewLayer:loadRes( )
  cc.Director:getInstance():getTextureCache():addImage("game_res/card_b.png")
  cc.Director:getInstance():getTextureCache():addImage("game_res/selectCard/Cards_big.png")
  -- 语音动画
  AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

  --手枪动画初始化
  self:initGame()
end

function GameViewLayer:unLoadRes()
  cc.Director:getInstance():getTextureCache():removeTextureForKey("game_res/card_b.png")
  cc.Director:getInstance():getTextureCache():removeTextureForKey("game_res/selectCard/Cards_big.png")
  AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
   --移除动画缓存
  cc.AnimationCache:getInstance():removeAnimation("gun_anim")

  cc.AnimationCache:getInstance():removeAnimation("TrustTee")

  cc.Director:getInstance():getTextureCache():removeUnusedTextures()
  cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

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

function GameViewLayer:initCsbRes()
    local _meusbt = ccui.Button:create()
    _meusbt:setScale9Enabled(true)
    _meusbt:setContentSize(yl.WIDTH, yl.HEIGHT)
    _meusbt:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
    _meusbt:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
        if self._bMenu == true then
          self._bMenu = false
          self:showMenu()
          -- self._menus:stopAllActions()
          -- self._menus:runAction(cc.MoveTo:create(0.2, cc.p(1233, 1065)))
        end
      end
    end)
    self:addChild(_meusbt)

    local rootLayer, csbNode = ExternalFun.loadRootCSB("game_res/Game.csb",self)
    self._rootNode = csbNode

    self:initButtonEvent()
    self:gameClean()

  
    for i=1,cmd.GAME_PLAYER do
      local panelCard = self._rootNode:getChildByName(string.format("Panel_card_%d",i))
      for j=1, cmd.HAND_CARD_COUNT do
        local pcard = CardSprite:createCard()
        --pcard:setMaCard("game_res/ma_image.png")
        pcard:setVisible(false)
        panelCard:addChild(pcard)
        pcard:setLocalZOrder(j)
        table.insert(self._CardList[i], pcard)
        pcard:setPosition(45+18*(j-1), panelCard:getContentSize().height/2)
      end

      local lightbg = self._rootNode:getChildByName(string.format("im_headbg_light_%d",i))
      lightbg:setVisible(false)
      local posx, posy = lightbg:getPosition()
      table.insert(self._seatposlist, cc.p(posx, posy))
    end

    --倒计时
    self._clockTime = csbNode:getChildByName("time_clock")
    self._clockTime:setString("30")
    self:setClockVisible(false)
end

function GameViewLayer:initGame()

  --托管动画
  local frames = {}
  for j=1,2 do
    local frame = cc.SpriteFrame:create("game_res/".."ico_robot"..string.format("_%d.png", j-1),cc.rect(0,0,39,45))
    table.insert(frames, frame)
  end
  local animation =cc.Animation:createWithSpriteFrames(frames,0.2)
  cc.AnimationCache:getInstance():addAnimation(animation, "TrustTee")


  --开枪动画
  local function getAnimation()
    local frames = {}
    for j=1,2 do
      local frame = cc.SpriteFrame:create("game_res/gun/"..string.format("im_gun_0.png", j),cc.rect(0,0,389,271))
      table.insert(frames, frame)
    end

    --停留在第一帧
    local index = 1
    local frame = cc.SpriteFrame:create("game_res/gun/"..string.format("im_gun_%d.png", index),cc.rect(0,0,389,271))
    table.insert(frames, frame)

    local animation =cc.Animation:createWithSpriteFrames(frames,0.1)
    cc.AnimationCache:getInstance():addAnimation(animation, "gun_anim")
  end

  -- 聊天动画
  local sc = cc.ScaleTo:create(0.1, 1.0, 1.0)
  local show = cc.Show:create()
  local spa = cc.Spawn:create(sc, show)
  self.m_actTip = cc.Sequence:create(spa, cc.DelayTime:create(2.0), cc.ScaleTo:create(0.1, 0.00001, 1.0), cc.Hide:create())
  self.m_actTip:retain()

  -- 语音动画
  local param = AnimationMgr.getAnimationParam()
  param.m_fDelay = 0.1
  param.m_strName = cmd.VOICE_ANIMATION_KEY
  local animate = AnimationMgr.getAnimate(param)
  self.m_actVoiceAni = cc.RepeatForever:create(animate)
  self.m_actVoiceAni:retain()

  --动画缓存
  getAnimation()
end

function GameViewLayer:getParentNode()
  return self._scene
end

function GameViewLayer:getDataMgr( )
  return self:getParentNode():getDataMgr()
end

function GameViewLayer:showPopWait( )
  self:getParentNode():showPopWait()
end

function GameViewLayer:showReadyBtn(isShow)
  print("showReadyBtn",isShow,debug.traceback())
  local btn = self._rootNode:getChildByName("btn_ready")
  btn:setVisible(isShow)
  local change = self._rootNode:getChildByName("btn_change"):setVisible(false)
  -- if not GlobalUserItem.bPrivateRoom then
  --    change:setVisible(isShow)
  -- end 

end

function GameViewLayer:dismissResult()
  self._bDismissed = true
end

--初始化按钮
function GameViewLayer:initButtonEvent()
  --准备
  local btn = self._rootNode:getChildByName("btn_ready")
  btn:setTag(TAG.READY_BTN)
  btn:addTouchEventListener(handler(self, self.onEvent))

  self.btn_game_start = self._rootNode:getChildByName("btn_game_start")
  self.btn_game_start:setTag(TAG.BT_GAME_START)
  self.btn_game_start:addTouchEventListener(handler(self, self.onEvent))

  self.btn_player_sit = self._rootNode:getChildByName("btn_player_sit")
  self.btn_player_sit:setTag(TAG.BT_PLAYER_SIT)
  self.btn_player_sit:addTouchEventListener(handler(self, self.onEvent))

  if GlobalUserItem.bPrivateRoom then
     btn:setPositionX(667)
  end

  --游戏菜单项
  btn = self._rootNode:getChildByName("bt_menu")
  btn:setTag(TAG.MENU)
  btn:addTouchEventListener(handler(self, self.onEvent))

  btn = self._rootNode:getChildByName("bt_location")
  btn:setTag(TAG.BTN_LOCATION)
  btn:addTouchEventListener(handler(self, self.onEvent))


  --菜单栏
  self._menus = self._rootNode:getChildByName("bt_menu_bg")

  --设置
  btn = self._menus:getChildByName("bt_set")
  btn:setTag(TAG.MENU_SET)
  btn:addTouchEventListener(handler(self, self.onEvent))

  --退出
  btn = self._menus:getChildByName("bt_quit")
  btn:setTag(TAG.MENU_QUIT)
  btn:addTouchEventListener(handler(self, self.onEvent))

    --玩法
  btn = self._menus:getChildByName("bt_help")
  btn:setTag(TAG.MENU_HELP)
  btn:addTouchEventListener(handler(self, self.onEvent))

    --托管
  btn = self._menus:getChildByName("bt_trustee")
  btn:setTag(TAG.MENU_TRUSTTEE)
  btn:addTouchEventListener(handler(self, self.onEvent))
  self.btnTrustee = btn

  --换桌
  btn = self._menus:getChildByName("bt_change")
  btn:setTag(TAG.MENU_CHANGE)
  btn:addTouchEventListener(handler(self, self.onEvent))
  if GlobalUserItem.bPrivateRoom then
    btn:setEnabled(false)
  end
  
    --换桌
  btn = self._rootNode:getChildByName("btn_change")
  btn:setTag(TAG.MENU_CHANGE)
  btn:addTouchEventListener(handler(self, self.onEvent))
  -- if GlobalUserItem.bPrivateRoom then
     btn:setVisible(false)
  -- end




  --聊天
  btn = self._rootNode:getChildByName("bt_yuyin")
  btn:setTag(TAG.MENU_CHAT)
  btn:addTouchEventListener(handler(self, self.onEvent))


  --语音
  btn = self._rootNode:getChildByName("bt_talk")
  btn:setVisible(true)
  btn:addTouchEventListener(function(sender,eventType)
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

  -- --取消托管
  -- local cancelrobot = self._rootNode:getChildByName("btn_cancel_robot")
  -- cancelrobot:setVisible(false)
  -- cancelrobot:addTouchEventListener(function(sender,eventType)
  --     if eventType == ccui.TouchEventType.ended then
  --       self:canceltrusTee()
  --     end
  --   end)
    self.m_endScore =  self._rootNode:getChildByName("end_score")

    self:onScheduleSingal()
    self._schSingal = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function() self:onScheduleSingal() end, 5, false)

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

    local panel_battery = self._rootNode:getChildByName("Image_19")
    local node_wifi = self._rootNode:getChildByName("wifi")
    --local node_4G = self._rootNode:getChildByName("4g")
    local labelTime = self._rootNode:getChildByName("curTime")

    self._rootNode:getChildByName("battleTxt"):setString(powerPercent.."%")


    --node_wifi:setVisible(isWifi)
    --node_4G:setVisible(not isWifi)

   -- local node = isWifi and node_wifi or node_4G
    --dump({isWifi,netStatus},"=====================11")
    local path = isWifi and "game_res/wifi-"..netStatus..".png" or "game_res/4g"..netStatus..".png"
       node_wifi:setTexture(path)
    
    panel_battery:setContentSize(cc.size(30*powerPercent*0.01, 30))

    if powerPercent<= 25 then
        panel_battery:setColor(cc.c3b(0xff,0,0))
    else
        panel_battery:setColor(cc.c3b(0xff,0xff,0xff))
    end
   
    labelTime:setString(""..os.date("%H : %M"))
end


function GameViewLayer:setRoomInfo()

    local topbg = self._rootNode:getChildByName("topBg")
    topbg:setVisible(true)
    local mapaiNode = self._rootNode:getChildByName("mpNode")
    mapaiNode:setVisible(true)
    if GlobalUserItem.bPrivateRoom then
        topbg:setTexture("game_res/topbg2.png")
        mapaiNode:setPositionY(0)
        topbg:setPositionY(720)
        if GlobalUserItem.bPrivateGoldRoom then
            mapaiNode:setPositionY(26)
        end
    else
        mapaiNode:setPositionY(55)
        topbg:setTexture("game_res/topbg1.png")
    end

    local maTxt = mapaiNode:getChildByName("Sprite_23")
    if self._scene._cbMaCardData > 0 then
          maTxt:setTexture("game_res/mapaiTxt.png")
          maTxt:setPositionX(62)
          mapaiNode:getChildByName("mpKuang"):setVisible(true)
          local ma = CardSprite:createCard(self._scene._cbMaCardData)
          ma:setPosition(112, 601)
          ma:setScale(0.67,0.67)
          mapaiNode:addChild(ma)
    else
          maTxt:setTexture("game_res/noMapai.png")
          maTxt:setPositionX(112)
          mapaiNode:getChildByName("mpKuang"):setVisible(false)
    end
end

--用户信息
function GameViewLayer:showUserInfo( useritem )

  local viewPos = self._scene:getUserIndex(useritem.wChairID)
  print("the viewPos is:"..viewPos..",the chairID is ================= >"..useritem.wChairID)
  --玩家头像
  local anr = {cc.p(0.0,0.0),cc.p(1.0,0.0),cc.p(1.0,1.0),cc.p(0.0,0.0)}
  

  local headBG = self._rootNode:getChildByName(string.format("user_%d",viewPos))
  headBG:setVisible(true)
  local head = PopupInfoHead:createNormal(useritem, 76)
  headBG:removeChildByTag(1)
  head:setTag(1)
  head:setAnchorPoint(cc.p(0.5,0.5))
  head:setPosition(cc.p(headBG:getContentSize().width/2,headBG:getContentSize().height/2))
  headBG:addChild(head)
  head:enableInfoPop(true,seatpos[viewPos] , anr[viewPos])

  if self._scene.m_bLookOn then
      head:enableInfoPop(false,seatpos[viewPos] , anr[viewPos])
  end


  local param = {}
  param.isProp = true     --显示道具互动，百人游戏不显示
  param.imgPath = "game_res/propBg.png" --弹出框背景
  head:setPropInfo(param)

  --head:setPopCsbFile("game_res/propBg.csb") -- 自定义CSB


  -- if useritem.cbUserStatus == yl.US_OFFLINE  and (not GlobalUserItem.bPrivateRoom  or (GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom )) then
  --   self:showReady(useritem.wChairID, false)
  --   --if nil ~= convertToGraySprite then
  --     -- 灰度图
  --     --if nil ~= head and nil ~= head.m_spRender then
  --     print("=================showUserInfoshowUserInfo=================")
  --       convertToGraySprite(head.m_head.m_spRender)
  --     --end
  --   --end
  -- end

  if useritem.cbUserStatus == yl.US_READY then
      self:showReady(useritem.wChairID, true)
  else
      self:showReady(useritem.wChairID, false)
  end

  local posRecord = {user=useritem,pos=viewPos}
  for i=1,#self._userList do
    local posRecord = self._userList[i]
    if posRecord.pos == viewPos then
        table.remove(self._userList,i)
      break
    end
  end

  table.insert(self._userList,posRecord)
  local pnick = headBG:getChildByName("txt_name")    

  --昵称
  local nick =  ClipText:createClipText(pnick:getContentSize(),useritem.szNickName,"fonts/round_body.ttf",18)
  headBG:removeChildByTag(2)
  nick:setTag(2)
  nick:setColor(cc.c3b(255, 255, 237))
  nick:setAnchorPoint(cc.p(0.5, 0.5))
  nick:setPosition(pnick:getPosition())
  headBG:addChild(nick)
  pnick:setVisible(false)

  --分数
  local scoreNode = headBG:getChildByName("txt_score")
  assert(scoreNode)

  local _scoreUser = 0
  if nil ~= useritem then
     _scoreUser = useritem.lScore

  end 

  local str = ExternalFun.formatScoreText(_scoreUser)
  if not self._bCompareCard  then
     scoreNode:setString(str)
  end
 
  --房主标识
  if PriRoom and GlobalUserItem.bPrivateRoom then

    local fanzhuicon = headBG:getChildByName("icon_fangzhu")
    if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == useritem.dwUserID and not GlobalUserItem.bPrivateGoldRoom then
      self:showFangzhuIcon(self:getParentNode():getUserIndex(useritem.wChairID))
    end

    --积分场分数显示
    if PriRoom:getInstance().m_tabPriData.lIniScore ~= nil and 
        PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
      dump(PriRoom:getInstance().m_tabPriData, "约战房信息", 10)
     _scoreUser = _scoreUser - PriRoom:getInstance().m_tabPriData.lIniScore
     if scoreNode then
       if self._bCompareCard  then
         function callWithArgs(args)
            local ret = function() scoreNode:setString(string.format("%d", args)) end
            return ret
         end
         self:runAction(cc.Sequence:create(cc.DelayTime:create(6.0),cc.CallFunc:create(callWithArgs(_scoreUser))))
       else
         scoreNode:setString(string.format("%d", _scoreUser))
       end
     end
    end
  end
  
end

--删除用户
function GameViewLayer:deleteUserInfo(useritem)
  if useritem == self._scene:GetMeUserItem() then
    return
  end

  local userinfo = clone(self._scene:getUserInfoByUserID(useritem.dwUserID))
  if not userinfo then
    return
  end
  local pos = self._scene:getUserIndex(userinfo.wChairID)
 
-- local userinfo = useritem
  self._scene.m_tabUserItem[userinfo.wChairID+1] = nil

  local func = function()
    if userinfo ~= nil then
      local headBG = self._rootNode:getChildByName(string.format("user_%d",pos))
      headBG:removeChildByTag(1)
      headBG:removeChildByTag(2)
      headBG:setVisible(false)
      table.remove(self._userList,i)

      --隐藏准备按钮
      --self:showReady(i,false)
      local readyIcon = self._rootNode:getChildByName(string.format("txt_ready_%d", pos))
      if readyIcon then
        readyIcon:setVisible(false)
      end
      --隐藏牌
      local panel = self._rootNode:getChildByName(string.format("Panel_card_%d", pos))
      if panel == nil then
        return
      end
      --清除扑克牌
      if self._CardList[pos] ~= nil then
        for k,v in pairs(self._CardList[pos]) do
          --v:setVisible(false)
        end
      end
      local tips = panel:getChildByName("im_tips")
      tips:setVisible(false)
      local special = panel:getChildByName("im_special")
      special:setVisible(false)
    end
  end
  local delayTime = 0.0
  if (self._exitScore[userinfo.wChairID+1] ~= 0) then
     delayTime = 2.0
  end
  self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(func)))
  
end

--获取用户
function GameViewLayer:getuseritemByindex(userindex)
  for k,v in pairs(self._userList) do
    if v.pos == userindex then
      return v.user
    end
  end
  return nil
end

--刷新分数
function GameViewLayer:reFreshScore(useritem)

   local viewPos = self._scene:getUserIndex(useritem.wChairID)
   local headBG = self._rootNode:getChildByName(string.format("user_%d", viewPos))
   local scoreNode = headBG:getChildByName("txt_score")

   local _scoreUser = 0
    if nil ~= useritem then
      _scoreUser = useritem.lScore
    end 

   local str = ExternalFun.formatScoreText(_scoreUser)
   
    scoreNode:setString(""..str)
        

   if PriRoom and GlobalUserItem.bPrivateRoom then
    if PriRoom:getInstance().m_tabPriData.lIniScore ~= nil and PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
      _scoreUser = _scoreUser - PriRoom:getInstance().m_tabPriData.lIniScore
      if scoreNode then
         
           scoreNode:setString(string.format("%d", _scoreUser))
         
      end
    end
     
   end
  
end


function GameViewLayer:setBaseScore(score)
      self._rootNode:getChildByName("difen_txt"):setString(score.."分")

end

--刷新分数
function GameViewLayer:updateScore(item)
   self:reFreshScore(item)
end

--倒计时结束处理
function GameViewLayer:LogicTimeZero(clockID)
print("倒计时结束处理")
  if self._scene.m_cbGameStatus == cmd.GS_WK_FREE then
    print("clockID=",clockID, PriRoom:getInstance().m_tabPriData.dwPlayCount, GlobalUserItem.dwCurRoomGroupID)
    if self._scene.m_bLookOn then
      return
    end
    if GlobalUserItem.dwCurRoomGroupID ~= 0 and (cmd.Clock_end == clockID or cmd.Clock_free == clockID) then
      if PriRoom:getInstance().m_tabPriData.dwPlayCount > 0 and PriRoom:getInstance().m_tabPriData.dwPlayCount < PriRoom:getInstance().m_tabPriData.dwDrawCountLimit then
        print("倒计时结束处理，自动准备")
        -- self:onStartReady()
      end
    elseif not (PriRoom and GlobalUserItem.bPrivateRoom)  or (GlobalUserItem.bPrivateGoldRoom and GlobalUserItem.dwCurRoomGroupID == 0) then
      self._scene:onExitTable()
    end
    
  else
    if nil ~= self._selectLayer then 
      if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom then
        --self._selectNode.selectNode:logicTimeZero()
      end
       
    end 
  end
end

--游戏清除
function GameViewLayer:gameClean()
   self:stopAllActions()
   for i=1,cmd.GAME_PLAYER do
      local panel = self._rootNode:getChildByName(string.format("Panel_card_%d", i))
      --清除扑克牌
      if self._CardList[i] ~= nil then
        for k,v in pairs(self._CardList[i]) do
          v:setVisible(false)
          v:showCardBack(true)
          v:showMaCard(false)
          v:retain()
          v:removeFromParent()
        end
      end

      local userbg = self._rootNode:getChildByName(string.format("user_%d", i))
      local txtscore = userbg:getChildByName("txt_end_score")
      txtscore:setVisible(false)

      local lightbg = self._rootNode:getChildByName(string.format("im_headbg_light_%d", i))
      lightbg:stopAllActions()
      lightbg:setVisible(false)
      
      local tips = panel:getChildByName("im_tips")
      tips:setLocalZOrder(20)
      tips:setVisible(false)
      tips:retain()
      tips:removeFromParent()
      
      local special = panel:getChildByName("im_special")
      special:setLocalZOrder(20)
      special:setVisible(false)
      special:retain()
      special:removeFromParent()

      panel:removeAllChildren()
      if self._CardList[i] ~= nil then
        for k,v in pairs(self._CardList[i]) do
          panel:addChild(v)
          v:release()
        end
      end
      panel:addChild(tips)
      panel:addChild(special)
      tips:release()
      special:release()

      --枪孔
      for j=1,3 do
        local hole = self._rootNode:getChildByName(string.format("hole_%d_%d", i, j-1))
        hole:setVisible(false)
      end
      
      --手枪
      for j=1,3 do
        local gun = self._rootNode:getChildByName("gun_"..string.format("%d_%d",i,j-1))
        gun:setVisible(false)
      end
     
   end

   --结算分数
   for i=1,4 do
     for j=1,4 do
      local cell = self._rootNode:getChildByName("end_score"):getChildByName("end_cell_"..i.."_"..(j-1))
       cell:setVisible(false)
     end
   end

   --金币移除
   for k,v in pairs(self._goldlist) do
     v:removeFromParent()
   end
   self._goldlist = {}

   if self._selectLayer ~= nil  then
      self._selectLayer:removeFromParent()
      self._selectLayer = nil
   end

   self._bCompareCard = false
   self._bGetTypeData = false
   self._bComSend = false

   self:stopAllActions()
end

function GameViewLayer:trustTeeDeal(userIndex,bTrustee)
  local headBG = self._rootNode:getChildByName(string.format("user_%d",userIndex))
  if not headBG then
    return
  end
  headBG:removeChildByTag(10)

  if userIndex == 1 and bTrustee == false then
    self._bTrustTee = false
    local cancelrobot = self._rootNode:getChildByName("btn_cancel_robot")
    cancelrobot:setVisible(false)
  end

  if not bTrustee then
     return
  end
 
  local Icon = cc.Sprite:create("game_res/ico_robot_0.png")
  Icon:setTag(10)
  Icon:setAnchorPoint(cc.p(1.0,0.0))
  Icon:setPosition(cc.p(124,2))
  headBG:addChild(Icon,20)

  local animation = cc.AnimationCache:getInstance():getAnimation("TrustTee")
  local action = cc.Animate:create(animation)
  Icon:runAction(cc.RepeatForever:create(action))
end




function GameViewLayer:onEvent(sender,eventType)
  local tag = sender:getTag()

  if eventType == ccui.TouchEventType.ended  then
    if tag == TAG.READY_BTN then
        self:onStartReady(sender)
    elseif tag == TAG.BT_PLAYER_SIT then   --坐下
      -- print("-------------坐下--------4444444----")
      -- if self._scene:onGetSitUserNum() == cmd.GAME_PLAYER then 
      --   showToast(self, "没有空位可以坐下！", 2)
      --   return 
      -- end

      -- if PriRoom:getInstance().m_tabPriData.dwPlayCount ==  PriRoom:getInstance().m_tabPriData.dwDrawCountLimit then 
      --   showToast(self, "最后一局，无法坐下！", 2)
      --   return 
      -- end
      -- for i=1,cmd.GAME_PLAYER do
      --     self._rootNode:getChildByName("txt_ready_"..i):setVisible(false)
      --     local userbg = self._rootNode:getChildByName(string.format("user_%d", i))
      --     if userbg and i ~= cmd.ME_VIEW_CHAIR then
      --         userbg:setVisible(false)
      --     end
      -- end
      
      -- self:showSitBtn(false)
      -- self._scene._gameFrame.m_bSendOption = false
      -- self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)
      -- self:gameClean()
      -- self._scene.m_bLookOn = false
      -- self._scene.m_bOnGame = false

      self._scene._gameFrame.bChangeDesk = true
      self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)

    elseif tag == TAG.BT_GAME_START then   --房主开始游戏 
      self:showSitBtn(false)
      self._scene._gameFrame:SendUserBegin()
    elseif tag == TAG.MENU then   --游戏菜单
      self._bMenu = not self._bMenu
      self:showMenu()
      if GlobalUserItem.bPrivateRoom and GlobalUserItem.bPrivateGoldRoom == false then
        self.btnTrustee:setEnabled(false)
      end
      print("GlobalUserItem.nMusic,GlobalUserItem.bVoiceAble",GlobalUserItem.nMusic,GlobalUserItem.bVoiceAble)
  -- ExternalFun.playBackgroudAudio("music_game.mp3")
      
    elseif tag == TAG.MENU_HELP then     --帮助
      self._bMenu = false
      self:showMenu()
      self:showHelp()
      --self._scene:getParentNode():popHelpLayer2(cmd.KIND_ID, 0, 5)
    elseif tag == TAG.MENU_CHANGE then
      if self._scene.m_bLookOn then
          showToast(self, "观战中不可使用该功能", 1)
          return
      end
      self._bMenu = false
      self:showMenu()
      self._scene._gameFrame.bChangeDesk = true
      self:changeTable()
    elseif tag == TAG.MENU_CHAT then     --聊天
      if self._scene.m_bLookOn then
        showToast(self, "观战中不可使用该功能", 1)
        return
      end
      self._bMenu = false
      self:showMenu()
      self:showChat()
    elseif tag == TAG.MENU_SET then     --设置
      
      self._bMenu = false
      self:showMenu()
      self:showSet()
    elseif tag == TAG.MENU_TRUSTTEE then --托管
      if self._scene.m_bLookOn then
        showToast(self, "观战中不可使用该功能", 1)
        return
      end
      self._bMenu = false
      self:showMenu()
      self:trustTee()
    elseif tag == TAG.MENU_QUIT then  --退出
      self._bMenu = false
      self:showMenu()
      self:backRoom()   
    elseif tag == TAG.BTN_LOCATION then  --定位 
      if self._scene.m_bLookOn then
        showToast(self, "观战中不可使用该功能", 1)
        return
      end
      self:onClickLocation()
    end
  end
end

function GameViewLayer:changeTable()
      local change = self._rootNode:getChildByName("btn_change")
      if change:isVisible() then
        change:setEnabled(false)
        change:runAction(cc.Sequence:create(cc.DelayTime:create(1),cc.CallFunc:create(function ( ... )
          change:setEnabled(true)
        end)))
      end
      if self._scene.m_cbGameStatus == cmd.GS_WK_FREE or  not self._scene._bUserGameStatus[self._scene._wchair +1] then
                self._scene._gameFrame:QueryChangeDesk()
                for i=1,cmd.GAME_PLAYER do
                    self._rootNode:getChildByName("txt_ready_"..i):setVisible(false)
                    local userbg = self._rootNode:getChildByName(string.format("user_%d", i))

                    if userbg and i ~= cmd.ME_VIEW_CHAIR then
                        userbg:setVisible(false)
                    end
                end

                self:showReadyBtn(true)
        else
                showToast(self, "正在游戏中，请先结束游戏", 2)
        end
end

function GameViewLayer:showHelp()

    local helpLayer,csbNode = ExternalFun.loadRootCSB("game_res/IntroduceLayer.csb",self)
    helpLayer:setName("_helpLayer")
    helpLayer:setLocalZOrder(GameViewLayer.TopZorder)
    local closeBtn = csbNode:getChildByName("image_bg"):getChildByName("btn_close")
      closeBtn:addTouchEventListener(function(sender,eventType)
    if eventType == ccui.TouchEventType.ended then
        helpLayer:removeFromParent()
    end
  end)

end

--显示菜单栏
function GameViewLayer:showMenu()
  if self._menus == nil then
    return
  end
  --local btpull = self._rootNode:getChildByName("bt_menu")
  self._menus:setVisible(self._bMenu)
    -- if self._bMenu == true then
    --     self._menus:stopAllActions()
    --     self._menus:runAction(cc.MoveTo:create(0.2, cc.p(1233, 560)))
    -- else
    --     self._menus:stopAllActions()
    --     self._menus:runAction(cc.MoveTo:create(0.2, cc.p(1233, 1065)))
    -- end
end

--聊天界面
function GameViewLayer:showChat()
  --聊天，调用本地资源，不用通用
  local tabconfig = {}
  tabconfig.csbfile = RES_PATH.."chat_res/GameChatLayer.csb"
  
  tabconfig.szItemSelectFrameName = RES_PATH.."chat_res/public_chat_txtSelect.png"
  tabconfig.szItemFrameName = RES_PATH.."chat_res/public_chatLine.png"

  --   self._chatLayer = GameChatLayer:create(self._scene._gameFrame, tabconfig):addTo(self, GameViewLayer.TopZorder)  --聊天框
  --   self._chatLayer:showGameChat(true)
  --   self._chatLayer:setTag(TAG.CHAT_VIEW)

    if nil == self.m_chatLayer then
                 self.m_chatLayer = GameChatLayer:create(self._scene._gameFrame,tabconfig)
                 self:addChild(self.m_chatLayer, GameViewLayer.TopZorder, "_chatLayer")
    end

    self.m_chatLayer:showGameChat(true)

end

function GameViewLayer:showSitBtn(visible, time)
  if self._scene.m_bLookOn then 
      self.btn_player_sit:setVisible(visible)
      self.btn_game_start:setVisible(false)
      -- self:showReadyBtn(false)
  else
      self.btn_game_start:setVisible(visible)
      self.btn_player_sit:setVisible(false)
      self.btn_game_start:getChildByName("time_start"):setString("")
      if visible then
        if not self._beginTimeId and time then
            self.m_beginTimeNum = time
            self.btn_game_start:getChildByName("time_start"):setString("("..self.m_beginTimeNum..")")
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
      
      -- self:showReadyBtn(false)
  end
end

function GameViewLayer:updateBeginTime()
  if self.m_beginTimeNum and self.m_beginTimeNum >0 then 
    self.m_beginTimeNum = self.m_beginTimeNum -1
    self.btn_game_start:getChildByName("time_start"):setString("("..self.m_beginTimeNum..")")  
  else
    self.btn_game_start:getChildByName("time_start"):setString("")
    if self._beginTimeId then
      cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
      self._beginTimeId = nil
    end
  end

end

--设置界面
function GameViewLayer:showSet()
  self:removeChildByTag(TAG.SET_VIEW)

  local Mask = ccui.ImageView:create()
  Mask:setTag(TAG.SET_VIEW)
  Mask:setContentSize(cc.size(yl.WIDTH, yl.HEIGHT))
  Mask:setScale9Enabled(true)
  Mask:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  Mask:setTouchEnabled(true)
  self:addChild(Mask,GameViewLayer.TopZorder)

  Mask:addTouchEventListener(function(sender,eventType)
    if eventType == ccui.TouchEventType.ended then
        Mask:removeFromParent()
    end
  end)

 --加载CSB
  local setNode = cc.CSLoader:createNode("game_res/Set.csb");
  setNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  Mask:addChild(setNode)

  -- 版本号
  local mgr = self._scene:getParentNode():getApp():getVersionMgr()
  local verstr = mgr:getResVersion(51) or "0"
  verstr = "游戏版本:" .. appdf.BASE_C_VERSION .. "." .. verstr
  local txt_ver = setNode:getChildByName("txt_version")
  txt_ver:setString(verstr)

  local function updateVolum()
    local tmp = GlobalUserItem.nMusic 
      if tmp >100 then
          tmp = 100
      elseif tmp < 0 then
          tmp = 0
      end
      print("tmptmp",tmp)
      AudioEngine.setMusicVolume(tmp/100.0) 

      tmp = GlobalUserItem.nSound 
      if tmp >100 then
          tmp = 100
      elseif tmp < 0 then
          tmp = 0
      end
      AudioEngine.setEffectsVolume(tmp/100.0) 
  end

  local BT_MUSIC = 12
  local BT_EFFECT = 22
  local function onSliderEvent(sender, event)
    local bSlience = true
      local bChange = false
      local tag = sender:getTag()
      local changePer = sender:getPercent()
      if event == ccui.SliderEventType.percentChanged then
          if BT_MUSIC == tag then
              bChange = changePer ~= GlobalUserItem.nMusic
              GlobalUserItem.nMusic = changePer
              print("GlobalUserItem.nMusic",GlobalUserItem.nMusic)
          elseif BT_EFFECT == tag then
              bChange = changePer ~= GlobalUserItem.nSound
              GlobalUserItem.nSound = changePer
          end
      end

      if 0 ~= GlobalUserItem.nMusic or 0 ~= GlobalUserItem.nSound then
          bSlience = false
      end
      if bChange then
          if GlobalUserItem.nMusic == 0 then
              GlobalUserItem.setVoiceAble(false)
          else
            print("tag,changePer",tag,changePer)
              GlobalUserItem.bVoiceAble = true
              --背景音乐
              ExternalFun.playBackgroudAudio( "music_game.mp3" )
          end

          if GlobalUserItem.nSound == 0 then
              GlobalUserItem.setSoundAble(false)
          else
              GlobalUserItem.bSoundAble = true
          end
          updateVolum()
      end
  end

  local sliderFunc = function (sender, eventType)
      onSliderEvent(sender, eventType)
  end

  --音乐
  local btn = setNode:getChildByName("Slider_music")
  btn:setTag(BT_MUSIC)
  btn:addEventListener(sliderFunc)
  btn:setPercent(GlobalUserItem.nMusic)

  --音效
  local btn = setNode:getChildByName("Slider_effect")
  btn:setTag(BT_EFFECT)
  btn:addEventListener(sliderFunc)
  btn:setPercent(GlobalUserItem.nSound)
end

--托管
function GameViewLayer:trustTee()
  self._bTrustTee = not self._bTrustTee
  if true == self._bTrustTee then
    local useritem = self._scene:GetMeUserItem()

    if useritem.cbUserStatus == yl.US_SIT  then 
      local btn = self._rootNode:getChildByTag(TAG.READY_BTN)
      self:onStartReady(btn)
    end

    self:trustTeeDeal(1,true)
    self._scene:sendRobot(1)
    local cancelrobot = self._rootNode:getChildByName("btn_cancel_robot")
    cancelrobot:setVisible(true)
  else 
    self:trustTeeDeal(1,false)
    self._scene:sendRobot(0)
  end
end

--取消托管
function GameViewLayer:canceltrusTee()                             
  self._bTrustTee = false
  local cancelrobot = self._rootNode:getChildByName("btn_cancel_robot")
  cancelrobot:setVisible(false)
  self:trustTeeDeal(1,false)
  self._scene:sendRobot(0)
end

--返回
function GameViewLayer:backRoom()
    self:getParentNode():onQueryExitGame()
end

--准备事件
function GameViewLayer:onStartReady(sender)
    self:gameClean()

    if self._scene._timeShow ~= nil then
      self._scene.m_tTimeShowCard = self._scene._timeShow
    end
   
    -- self:showReadyBtn(false)
print("发送准备")
    self._scene:SendUserReady()
    self:setClockVisible(false)
    self._scene:KillGameClock()

    --显示准备状态
    -- local item = self._scene:GetMeUserItem()
    -- self:showReady(item.wChairID, true)
end

--显示准备
function GameViewLayer:showReady(wChairID,isShow)
  if wChairID == self._scene:GetMeChairID() and isShow then
    self:showReadyBtn(false)
  end

  local  viewPos = self._scene:getUserIndex(wChairID)
  local readyIcon = self._rootNode:getChildByName(string.format("txt_ready_%d", viewPos))
  if readyIcon then
    readyIcon:setVisible(isShow)
  end
end

function GameViewLayer:setClockVisible( visible )
  self._clockTime:setVisible(visible)
end

--刷新倒计时
function GameViewLayer:UpdataClockTime(clockID,time)

  if time < 0 or not self._scene then
    return
  end

  if nil ~= self._scene._clockTimeLeave then
      self._scene._clockTimeLeave = self._scene._clockTimeLeave - 1
  end

  if clockID == cmd.Clock_end then
      self:setClockVisible(self._rootNode:getChildByName("btn_ready"):isVisible())
      self._clockTime:setString(string.format("%d",time))
  elseif cmd.Clock_status == clockID  then
    self:setClockVisible(true)
    self._clockTime:setString(string.format("%d",time))

   if self._selectLayer and self._selectLayer.UpdataClockTime then
      self._selectLayer:UpdataClockTime(clockID, time)
    end

  elseif cmd.Clock_free == clockID then
    self:setClockVisible(true)
    self._clockTime:setString(string.format("%d",time))
  end

  if time == 0 then
      self._scene:KillGameClock()

      self._scene._clockTimeLeave = 0
      self:setClockVisible(false)
      self:LogicTimeZero(clockID)
      return
  end
end

--收到数据
function GameViewLayer:SpecialCardInfo()
  -- self._bGetTypeData = true
  -- if self._bComSend == true then
  --   self:popSlectcardView()
  -- end

    local delayTime = 0
  if self._scene._enterMessage == cmd.GS_WK_FREE then
     delayTime = 2
  end
    self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime), cc.CallFunc:create(function()
        self:popSlectcardView()
        end)))

end

function GameViewLayer:GameVideoSendCard(param)
    local dispatchInfo = param
    local sendTotalRound = dispatchInfo.cardCount
    local nPlayerNum = #dispatchInfo.playerIndex

    for i=1,nPlayerNum do
      local viewIndex = dispatchInfo.playerIndex[i]
      self:showSelectStatus(viewIndex,cmd.GS_WK_SETCHIP - cmd.GS_WK_CALL_BANKER)    --显示选牌中

      local panelCard = self._rootNode:getChildByName(string.format("Panel_card_%d",viewIndex))
      for i=1,sendTotalRound do
        local card  = self._CardList[viewIndex][i]
        if nil ~= card then
          card:stopAllActions()
          card:setVisible(true)
          card:setPosition(45, panelCard:getContentSize().height/2)
          card:setRotation(0)
          card:setPosition(45+18*(i-1), panelCard:getContentSize().height/2)
        end
        
      end
    end

end



--游戏开始发牌
function GameViewLayer:dispatchCard(param,action,pop)
  if action == true then
      ExternalFun.playSoundEffect("start.mp3")
      local beginNode = ExternalFun.loadCSB("beginAction.csb", self)
      beginNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
      local beginAction = ExternalFun.loadTimeLine("beginAction.csb")
      beginAction:gotoFrameAndPlay(0, false)
      beginNode:runAction(beginAction)
      beginNode:runAction(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function()
        beginNode:removeFromParent()
        end)))
  end
  
  local function sendCard(param, action, pop)
    local dispatchInfo = param
    local sendTotalRound = dispatchInfo.cardCount
    local nPlayerNum = #dispatchInfo.playerIndex

    for i=1,nPlayerNum do
      local viewIndex = dispatchInfo.playerIndex[i]
      print("发牌动画")
      if viewIndex ~= cmd.ME_VIEW_CHAIR then
        if pop == true or self._scene.m_bjoinGame == false then
          local statuscallfunc = cc.CallFunc:create(function()
          self:showSelectStatus(viewIndex,cmd.GS_WK_SETCHIP - cmd.GS_WK_CALL_BANKER)    --显示选牌中
          self:showSelectAction(viewIndex)
          print("显示选牌中")
          end)
          local delaytime = 0.1
          if action then
            delaytime = 0.4
          end 
          self:runAction(cc.Sequence:create(cc.DelayTime:create(delaytime),statuscallfunc))
        end
         
      end
      --派发扑克 
      local panelCard = self._rootNode:getChildByName(string.format("Panel_card_%d",viewIndex))

      for i=1,sendTotalRound do
        local card  = self._CardList[viewIndex][i]
        if nil ~= card then
          card:stopAllActions()
          card:setVisible(true)
          card:setPosition(45, panelCard:getContentSize().height/2)
          card:setRotation(0)
          if action then
            if viewIndex == 2 then
              card:runAction(cc.MoveTo:create(0.3,cc.p(-40+18*(i-1),panelCard:getContentSize().height/2)))
            else
              card:runAction(cc.MoveTo:create(0.3,cc.p(45+18*(i-1),panelCard:getContentSize().height/2)))
            end
          else
            if viewIndex == 2 then
              card:setPosition(-40+18*(i-1), panelCard:getContentSize().height/2)
            else
              card:setPosition(45+18*(i-1), panelCard:getContentSize().height/2)
            end
          end
        end
      end
    end
 
    --弹出选牌界面
    -- if pop then
    --   local callfunc = cc.CallFunc:create(function()
    --     self._bComSend = true
    --       if self._bGetTypeData == true then
    --         self:popSlectcardView()
    --       end 
    --   end)
    --   self:runAction(cc.Sequence:create(cc.DelayTime:create(1),callfunc))
    -- end
  end
  local waittime = action == true and 1 or 0
  print("发牌等待时间", waittime)
  self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
      sendCard(param, action, pop)
    end)))
end

--选牌操作界面
function GameViewLayer:popSlectcardView()

  if self._selectLayer then
    self._selectLayer:removeFromParent()
    self._selectLayer = nil
  end
  ExternalFun.playSoundEffect("start_poker.mp3")

  local _selectLayer = SelectLayer:create(self, self._scene._cbHandCard)
  self:addChild(_selectLayer, GameViewLayer.TopZorder)
  self._selectLayer = _selectLayer

  --选牌倒计时
  local time  = (self._scene.m_tTimeShowCard == 0) and 60 or self._scene.m_tTimeShowCard
  time = self._scene.cbTimeLeave  or time
  if not self.bIsVideo then
      self._scene:SetGameClock(self._scene._wchair,cmd.Clock_status, time)
  end
 
  
end

--选牌状态提示
function GameViewLayer:showSelectStatus(userindex,status)
  if cmd.ME_VIEW_CHAIR == userindex then
    return
  end

  local tips = self._rootNode:getChildByName(string.format("Panel_card_%d",userindex ))

  --当前状态
  local statusIcon = tips:getChildByName("im_tips")
  if nil ~= statusIcon then
    statusIcon:setVisible(true)
  end
end

--特殊牌型标识显示
function GameViewLayer:showSpecialIcon(userindex,value)
  local tips = self._rootNode:getChildByName(string.format("Panel_card_%d",userindex ))

  --当前状态
  local statusIcon = tips:getChildByName("im_special")
  if nil ~= statusIcon then
    statusIcon:setVisible(value)
  end
end

--配置完成 
function GameViewLayer:showSelectedCard( userIndex,pokerData,isVideo )

    local panel = self._rootNode:getChildByName(string.format("Panel_card_%d", userIndex))

    --配牌中提示
    local tips = panel:getChildByName("im_tips")
    if nil ~= tips then
      tips:setVisible(false)
    end
    
    --设置小扑克
    local posIndex = 1
    local data = pokerData or {0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff}
    for i=1,cmd.HAND_CARD_COUNT do
      local card = self._CardList[userIndex][i]
      if nil ~= card then
        card:stopAllActions()
        card:setCardValue(data[i])
        card:setLocalZOrder(i)
        if (isVideo ~= nil) or (userIndex == cmd.ME_VIEW_CHAIR or showAll) and not self._scene.m_bLookOn then
           card:showCardBack(false)
           card:updateSprite()
        end
      end
    end

    -- local anglelist = {-30, -15, 0, 15, 30}
    -- local anglelist = {0, 0, 0, 0, 0}
    -- local offsety = {-18, -5, 0, -5, -18}
    -- local offsety = {0, 0, 0, 0, 0}
    local beginy = panel:getContentSize().height/2 - 60
    if userIndex == cmd.ME_VIEW_CHAIR then
      beginy = panel:getContentSize().height/2
    end

    local pos = {cc.p(84.7, 185.46), cc.p(115.96, 190.17), cc.p(153.12, 180.55)}
    local anglelist = {-17, 0, 17}
    for j=1,3 do
      card = self._CardList[userIndex][j]
      card:setScale(0.83)
      card:setAnchorPoint(0.5,0.5)
      card:runAction(cc.MoveTo:create(0.17, pos[j]))--cc.p(panel:getContentSize().width/2-90+j*40, beginy+130+offsety[j+1])))
      card:runAction(cc.RotateTo:create(0.17, anglelist[j]))
    end

    local pos = {cc.p(55.51, 110.07), cc.p(85.69, 119.64), cc.p(115.83, 128.74), cc.p(149.68, 120.29), cc.p(178.01, 101.23)}
    local anglelist = {-32, -17, 0, 17, 34}
    for j=1,5 do
      card = self._CardList[userIndex][j+3]
      card:setScale(0.83)
      card:setAnchorPoint(0.5,0.5)
      card:runAction(cc.MoveTo:create(0.17, pos[j]))--cc.p(panel:getContentSize().width/2-134+j*40, beginy+65+offsety2[j])))
      card:runAction(cc.RotateTo:create(0.17, anglelist[j]))
    end

    local pos = {cc.p(56.67,38.66), cc.p(86.27,53.74), cc.p(115.56, 57.76), cc.p(152.37, 52.73), cc.p(186.07, 36.75)}
    local anglelist = {-33, -19, 0, 19, 33}
    for j=1,5 do
      card = self._CardList[userIndex][j+8]
      card:setScale(0.83)
      card:setAnchorPoint(0.5,0.5)
      card:runAction(cc.MoveTo:create(0.17, pos[j]))--cc.p(panel:getContentSize().width/2-134+j*40, beginy+offsety[j])))
      card:runAction(cc.RotateTo:create(0.17, anglelist[j]))
    end

end

--设置牌数据
function GameViewLayer:setSelectCardData(userIndex)
  local cardlist = {}
  for k,v in pairs(self._scene._sortedCard[userIndex].Front) do
    table.insert(cardlist, v)
  end
  for k,v in pairs(self._scene._sortedCard[userIndex].Mid) do
    table.insert(cardlist, v)
  end
  for k,v in pairs(self._scene._sortedCard[userIndex].Tail) do
    table.insert(cardlist, v)
  end
  
  for i=1,cmd.HAND_CARD_COUNT do
    local card = self._CardList[userIndex][i]
    card:setCardValue(cardlist[i])
  end
end

--其他玩家选牌中动画显示
function GameViewLayer:showSelectAction(userIndex)
  if userIndex == cmd.ME_VIEW_CHAIR then
    return
  end

  for i=1,cmd.HAND_CARD_COUNT do
    local action = cc.Sequence:create(cc.MoveBy:create(0.1, cc.p(0,20)), cc.MoveBy:create(0.1, cc.p(0, -20)))
    local repaction = cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(i*0.05), action, cc.DelayTime:create((cmd.HAND_CARD_COUNT-i)*0.05 )))
    local card = self._CardList[userIndex][i]
    if nil ~= card then
      card:stopAllActions()
      card:runAction(repaction)
    end
  end
end

--选牌完成
function GameViewLayer:finishSelect( param, value )
  assert(table.nums(param)==13)

  self._selectLayer:removeFromParent()
  self._selectLayer = nil

  --发送分段数据
  local dataBuffer = CCmd_Data:create(14)
  for k,v in pairs(param) do
    dataBuffer:pushbyte(v)
  end 
  --是否有特殊牌型
  if nil ~= value then
    dataBuffer:pushbyte(value)
  else
    dataBuffer:pushbyte(0)
  end

  dataBuffer:resetread()
  local test = ExternalFun.read_netdata(cmd.CMD_C_ShowCard, dataBuffer)

 self._scene:SendData(cmd.SUB_C_SHOWCARD,dataBuffer)

  --玩家摊牌数据
 local cardInfo = {}
 cardInfo.Front = {param[1], param[2], param[3]}
 cardInfo.Mid   = {param[4], param[5], param[6], param[7], param[8]}
 cardInfo.Tail  = {param[9], param[10], param[11], param[12], param[13]}
 self._scene._sortedCard[cmd.ME_VIEW_CHAIR] = cardInfo

 --设置扑克数据
 self:showSelectedCard(cmd.ME_VIEW_CHAIR, param)

--删除倒计时
 --self._scene:KillGameClock()
end

function GameViewLayer:gunTest( compareBuffer )
  --显示打枪
  local function gunBeginShow()
    local gunNode = ExternalFun.loadCSB("gunAction.csb", self)
    gunNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
    local gunAction = ExternalFun.loadTimeLine("gunAction.csb")
    gunAction:gotoFrameAndPlay(0, false)
    gunNode:runAction(gunAction)
    gunNode:runAction(cc.Sequence:create(cc.DelayTime:create(1.2), cc.CallFunc:create(function()
      gunNode:removeFromParent()
      end)))
  end

  --显示全垒打
  local function dataiallShow()
    local dataiallNode = ExternalFun.loadCSB("HomeRun.csb", self)
    dataiallNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
    local dataiallAction = ExternalFun.loadTimeLine("HomeRun.csb")
    dataiallAction:gotoFrameAndPlay(0, false)
    dataiallNode:runAction(dataiallAction)
    dataiallNode:runAction(cc.Sequence:create(cc.DelayTime:create(1.2), cc.CallFunc:create(function()
      dataiallNode:removeFromParent()
      end)))
  end

  local gunlist= self:getGunAcitionList(compareBuffer)
  local waittime = 1
  --要显示打枪
  if #gunlist ~= 0 then
    self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
        gunBeginShow()
        ExternalFun.playSoundEffect("daqiang1.mp3")
      end)))
    waittime = waittime + 1.4
    for k,v in pairs(gunlist) do
      self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
        self:gunActionShow(v)
      end)))
      waittime = waittime + 2.8
    end
    
    --全垒打
    local bdetaiall = false
    if compareBuffer.wAllKillUser >=0 and compareBuffer.wAllKillUser < 4 then
      bdetaiall = true
    end
    if (bdetaiall == true) and (self._scene:getBankerMode()==0) then
      self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
        dataiallShow()
        ExternalFun.playSoundEffect("special1.mp3")
      end)))
      waittime = waittime + 1.4
    end
  end
end

--开始比牌
function GameViewLayer:compareCard(users,compareBuffer)

  self._bCompareCard = true
  --删除倒计时
  self._scene:KillGameClock()
  self:setClockVisible(false)

  if self._selectLayer ~= nil then
     self._selectLayer:removeFromParent()
     self._selectLayer = nil
   end 

  self._bComSend = false
  self._bGetTypeData = false

  for i=1,cmd.GAME_PLAYER do
    --选牌提示
    local tips = self._rootNode:getChildByName(string.format("Panel_card_%d",i ))
    local imtips = tips:getChildByName("im_tips")
    if imtips then
      imtips:setVisible(false)
    end

    --特殊牌型显示
    if self._scene._bSpecialType[i] ~= 0 then
      self:showSpecialIcon(i, true)
    end
  end

  --翻牌动作数据
  local function getActionData(actiontype, userindex, actionindex)
    local table = {}
    table.actiontype = actiontype or 0 --0显示牌类型，1显示分数
    table.userindex = userindex or 0 --用户顺序
    table.actionindex = actionindex or 0 --0显示头墩，1显示中墩，2显示尾墩
    return table
  end

  --插入头墩顺序
  --index 0显示头墩，1显示中墩，2显示尾墩
  local function insertshowlist(actionlist, userlist, index)
    for i=1,#userlist do
      local action = getActionData(0, userlist[i], index)
      table.insert(actionlist, action)
    end
    if self._scene.m_bjoinGame == true then
      local score = getActionData(1, 1, index)
      table.insert(actionlist, score)
    end
  end

  --同墩大小排序
  --index 0比较头墩，1比较中墩，2比较尾墩
  local function comparelist(userlist, index)
    local userorder = clone(userlist)
    if #userlist == 1 then
      return userorder
    end
    local cardlist = {}

   for k,v in pairs(userlist) do
      if index == 0 then
        table.insert(cardlist, self._scene._sortedCard[v].Front)
      elseif index == 1 then
        table.insert(cardlist, self._scene._sortedCard[v].Mid)
      elseif index == 2 then
        table.insert(cardlist, self._scene._sortedCard[v].Tail)
      end
    end
    
    -- for i=1,#users do
    --   for j=i+1,#users do
    --     local value = GameLogic:CompareCard(cardlist[i], cardlist[j], #cardlist[i], #cardlist[j], true,self._scene.m_tonghuaFlag)
    --     if value == GameLogic.enCRLess then
    --       cardlist[i],cardlist[j] = cardlist[j],cardlist[i]
    --       userorder[i],userorder[j] = userorder[j],userorder[i]
    --     end
    --   end
    -- end
    return userorder
  end

  --墩位对比
  local actionlist = {}
  for i=1,3 do
    local userorder = comparelist(users, i-1)
    insertshowlist(actionlist, userorder, i-1)
  end

  --userindex 玩家位置
  --cardindex 牌墩
  local function showCardType(userindex, cardindex)
    local headBG = self._rootNode:getChildByName(string.format("user_%d",userindex)) 
    if not headBG:isVisible()  then 
      return
    end
    local cardlist = {}
    local beginpos = 1
    local midpos = 2
    local length = 2
    if cardindex == 0 then
      cardlist = self._scene._sortedCard[userindex].Front
    elseif cardindex == 1 then
      beginpos = 4
      length = 4
      midpos = 6
      cardlist = self._scene._sortedCard[userindex].Mid
    elseif cardindex == 2 then
      beginpos = 9
      midpos = 11
      length = 4
      cardlist = self._scene._sortedCard[userindex].Tail
    end

    local panellayout = self._rootNode:getChildByName(string.format("Panel_card_%d",userindex))
    for i=beginpos,beginpos+length do
      local pcard = self._CardList[userindex][i]
      if nil ~= pcard then
        pcard:stopAllActions()
        pcard:setLocalZOrder(i+13)
        pcard:showCardBack(false)
        pcard:updateSprite()
        if pcard:getCardData() == self._scene:getMaCode() then
            pcard:showMaCard(true)
        end
        pcard:runAction(cc.Sequence:create(cc.ScaleTo:create(0.2, 1.2), cc.DelayTime:create(0.2),cc.ScaleTo:create(0.2, 0.83), cc.CallFunc:create(function()
          pcard:setLocalZOrder(i)
          end)))

        if i == midpos then
          local typebg = cc.Sprite:create("game_res/im_cardtype_bg.png")
          typebg:setPosition(pcard:getPositionX(), pcard:getPositionY()-55)
          typebg:setLocalZOrder(30)
          panellayout:addChild(typebg)

          local cardType = GameLogic:GetCardType(cardlist,#cardlist)
          if cardType >= GameLogic.CT_FIVE_MIXED_FLUSH_NO_A  and  cardType <= GameLogic.CT_FIVE_MIXED_FLUSH_BACK_A then
            cardType = GameLogic.CT_FIVE_MIXED_FLUSH_NO_A
          elseif cardType >= GameLogic.CT_FIVE_STRAIGHT_FLUSH and cardType >= GameLogic.CT_FIVE_STRAIGHT_FLUSH_BACK_A then
            cardType = GameLogic.CT_FIVE_STRAIGHT_FLUSH
          end
          assert(cardType ~= GameLogic.CT_INVALID)
          local icon = ccui.ImageView:create(string.format("game_res/CardType/txt_cardtype_show_%d.png",cardType))
          icon:setPosition(typebg:getContentSize().width/2, typebg:getContentSize().height/2)
          typebg:addChild(icon)

          if not self._scene.m_bIsQuick then
            ExternalFun.playSoundEffect("common"..cardType..".mp3")
          end

          if self._scene._bUserGameStatus[self._scene:GetMeChairID()+1] then
            --self:runAction(cc.Sequence:create())
            local cellbg = self.m_endScore:getChildByName("end_cell_"..userindex.."_"..cardindex)
            --cellbg:setVisible(true)
            local txtscore = cellbg:getChildByName("txt_score")
            local score = self._scene._scoreTimes[userindex][cardindex+1]*self._scene._lScoreCell
            if score >= 0 then
              cellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/win.png")
              score = math.abs(score)
              txtscore:setProperty(str, "game_res/gameOver/num_win.png", 16, 28, ".")
            else
              cellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/lose.png")
              score = math.abs(score)
              txtscore:setProperty(str, "game_res/gameOver/num_lose.png", 16, 28, ".")
            end
            txtscore:setString("."..score)
          end

          typebg:runAction(cc.Sequence:create(cc.DelayTime:create(0.6), cc.CallFunc:create(function()
              typebg:removeFromParent()
            end)))
        end
      end
    end
  end

  -- --显示分数
  -- local function showUserDunScore(cardindex)
  --   local cellbg = self.m_endScore:getChildByName(string.format("end_cell_1_%d",cardindex)) 
  --   --cellbg:setPositionX(yl.WIDTH + 127)
  --   --cellbg:runAction(cc.MoveTo:create(0.37, cc.p(937, cellbg:getPositionY())))
  --   cellbg:setVisible(true)

  --   local txtscore = cellbg:getChildByName("txt_score")
  --   local score = self._scene._scoreTimes[1][cardindex+1]*self._scene._lScoreCell
  --   if score >= 0 then
  --     cellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/win.png")
  --     score = math.abs(score)
  --     txtscore:setProperty(str, "game_res/gameOver/num_win.png", 16, 22, ".")
  --   else
  --     cellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/lose.png")
  --     score = math.abs(score)
  --     txtscore:setProperty(str, "game_res/gameOver/num_lose.png", 16, 28, ".")
  --   end
  --   -- local bei = self:isFanBei(self._scene:getUserChair(1))
  --   --       if  bei ~=1 and cardindex ~= 1 then
  --   --           score = score.."."..bei
  --   --       end
  --   txtscore:setString("/"..score)

    
  --   if cardindex == 2 then
  --     local endscore = 0
  --     for i=1,cardindex+1 do
  --       endscore = endscore + self._scene._scoreTimes[1][i]
  --     end
  --     endscore = endscore*self._scene._lScoreCell

  --     local allcellbg = self.m_endScore:getChildByName("end_cell_1_3")
  --     local txtendscore =allcellbg:getChildByName("txt_score")
  --     --allcellbg:setPositionX(yl.WIDTH + 127)
  --     --allcellbg:runAction(cc.Sequence:create(cc.DelayTime:create(0.17), cc.MoveTo:create(0.37, cc.p(937, allcellbg:getPositionY()))))
  --     allcellbg:setVisible(true)

  --     if endscore >= 0 then
  --       allcellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/txt_all_score1.png")
  --       endscore = math.abs(endscore)
  --       txtendscore:setProperty(str, "game_res/gameOver/num_win.png", 16, 22, ".")
  --     else
  --       allcellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/txt_all_score2.png")
  --       endscore = math.abs(endscore)
  --       txtendscore:setProperty(str, "game_res/gameOver/num_lose.png", 16, 22, ".")
  --     end
  --     txtendscore:setString(string.format("/%d", endscore))
  --   end
  -- end

  local waittime = 1

  local function cardcompareshow(actionlist)
    local temptime = 0
    local flag = 1
    local last = nil
    local lastTime = nil
    local count = 0
    local maxCard = {}
    local soundFlag = {true,true,true}



  --统计每蹲最大的牌
  for k,v in pairs(actionlist) do
    if v.actiontype == 0 then

        local curCard = nil
            if v.actionindex == 0 then
              curCard  = self._scene._sortedCard[v.userindex].Front
            elseif v.actionindex == 1  then
              curCard  = self._scene._sortedCard[v.userindex].Mid
            elseif v.actionindex == 2  then
              curCard  = self._scene._sortedCard[v.userindex].Tail
            end
        if maxCard[v.actionindex+1] then
              local value = GameLogic:CompareCard(maxCard[v.actionindex+1], curCard, #maxCard[v.actionindex+1], #curCard, true,self._scene.m_tonghuaFlag)
              if value == GameLogic.enCRGreater then
                  maxCard[v.actionindex+1] = clone(curCard)
              end
        else
          maxCard[v.actionindex+1] = clone(curCard)
        end
    end
  end
  dump(maxCard,"====================================")


    --特殊牌型不比牌
    for k,v in pairs(self._scene._bSpecialType) do

        if v ~= 0 then
            count = count+1
        end

    end


    for k,v in pairs(actionlist) do
      if v.actiontype == 0 then

        if self._scene.m_bIsQuick then
            if  lastTime and lastTime.actionindex ~= v.actionindex  then
                temptime = temptime + 0.8
            end
        else
            temptime = temptime + 0.8
        end

        self:runAction(cc.Sequence:create(cc.DelayTime:create(temptime), cc.CallFunc:create(function()
          showCardType(v.userindex, v.actionindex)


          if (last and last.actionindex ~= v.actionindex ) or (self._scene.m_bIsQuick and soundFlag[v.actionindex+1]) then
                   local index = nil
                    if self._scene.m_bIsQuick and soundFlag[v.actionindex+1]  then
              
                          local cardType = GameLogic:GetCardType(maxCard[v.actionindex+1],#maxCard[v.actionindex+1])
                            if cardType >= GameLogic.CT_FIVE_MIXED_FLUSH_NO_A  and  cardType <= GameLogic.CT_FIVE_MIXED_FLUSH_BACK_A then
                              cardType = GameLogic.CT_FIVE_MIXED_FLUSH_NO_A
                            elseif cardType >= GameLogic.CT_FIVE_STRAIGHT_FLUSH and cardType >= GameLogic.CT_FIVE_STRAIGHT_FLUSH_BACK_A then
                              cardType = GameLogic.CT_FIVE_STRAIGHT_FLUSH
                            end

                            ExternalFun.playSoundEffect("common"..cardType..".mp3")

                            soundFlag[v.actionindex+1] = false
                            index = v.actionindex
                    else
                        index = last.actionindex
                    end

                for i=1,cmd.GAME_PLAYER do
                      local curUser = self._scene:getUserIndex(i-1)
                      if self._scene._bUserGameStatus[i] then
                        --print("=======uuuuuuuuuuuuu=========="..curUser..","..last.actionindex)
                          local cellbg = self.m_endScore:getChildByName("end_cell_"..curUser.."_"..index)   
                          local font = cellbg:getChildByName("txt_front")
                          local score = cellbg:getChildByName("txt_score")
                          cellbg:setVisible(true)  

                          if self._scene._bSpecialType[1] ~= 0 and curUser ~= 1 then
                              font:setTexture("game_res/gameOver/win.png")
                              -- font:setPositionX(10)
                              score:setVisible(false)
                          else
                              -- font:setPositionX(-32)
                              score:setVisible(true)
                              if self._scene._bSpecialType[curUser] ~= 0 then 
                                font:setTexture("game_res/gameOver/win.png")
                                score:setProperty(str, "game_res/gameOver/num_win.png", 16, 28, ".")
                                score:setString(".0")
                              end 
                          end 
                      end      
                 end

          end 

          if v.actionindex == 2 then 
            count = count+1
            if count == self._scene.m_curPlayCount then
                        local time = 1
                        if self.m_wAllKillUser ~= yl.INVALID_CHAIR then 
                            time = time +1
                        end 

                        local gunTimes = 0
                        for m=1,cmd.GAME_PLAYER do
                            for n=1,cmd.GAME_PLAYER do
                                if self._scene.m_bAllWin[m][n] >0 then
                                    gunTimes = gunTimes+1
                                end

                            end

                            --显示第三道得分
                            local curUser = self._scene:getUserIndex(m-1)
                            if self._scene._bUserGameStatus[m] == true then
                                  local cellbg = self.m_endScore:getChildByName("end_cell_"..curUser.."_"..2)   
                                  cellbg:setVisible(true)   
                                  local fontimg = cellbg:getChildByName("txt_front")
                                  local scoretxt = cellbg:getChildByName("txt_score")

                                  if self._scene._bSpecialType[1] ~= 0 and curUser ~= 1 then
                                      fontimg:setTexture("game_res/gameOver/win.png")
                                      -- fontimg:setPositionX(10)
                                      scoretxt:setVisible(false)
                                  else
                                      -- fontimg:setPositionX(-32)
                                      scoretxt:setVisible(true)
                                      if self._scene._bSpecialType[curUser] ~= 0 then 
                                          fontimg:setTexture("game_res/gameOver/win.png")
                                          scoretxt:setProperty(str, "game_res/gameOver/num_win.png", 16, 28, ".")
                                          scoretxt:setString(".0")
                                      end
                                  end 
                              end
                        end

                        self:runAction(cc.Sequence:create(cc.DelayTime:create(time+gunTimes*1),cc.CallFunc:create(function ( ... )
                                for i=1,cmd.GAME_PLAYER do
                                      local curUser = self._scene:getUserIndex(i-1)
                                      if self._scene._bUserGameStatus[i] == true and self._scene._bUserGameStatus[self._scene:GetMeChairID()+1] then
                                            
                                          
                                          local bei = self:isFanBei(i-1)

                                          local cellbg = self.m_endScore:getChildByName("end_cell_"..curUser.."_3")
                                          if curUser ~= 1 then 
                                                cellbg:setVisible(true)
                                          end

                                          local fanbei = cellbg:getChildByName("txt_front")
                                          if bei~=1 then
                                              fanbei:setTexture("game_res/gameOver/txt_bei_score2.png")
                                          else
                                              fanbei:setTexture("game_res/gameOver/txt_bei_score1.png")
                                          end

                                            if  bei ~=1 and curUser ~= 1 then
                                                for j=1,3 do
                                                    local cellbg = self.m_endScore:getChildByName("end_cell_"..curUser.."_"..(j-1))   
                                                    local txtscore = cellbg:getChildByName("txt_score")
                                                    local score = self._scene._scoreTimes[curUser][j]*self._scene._lScoreCell
                                                    score = math.abs(score)

                                                    if self._scene._bSpecialType[curUser] == 0 then
                                                      if not (self._scene._wBankerUser ~= yl.INVALID_CHAIR and score == 0) then 
                                                        txtscore:setString("."..score.."/"..bei)
                                                      end
                                                    end
                                                end
                                            end
                                            if  curUser == 1 then
                                                for j=1,4 do
                                                    local cellbg = self.m_endScore:getChildByName("end_cell_1_"..(j-1))   
                                                    cellbg:setVisible(true)
                                                    local txtscore = cellbg:getChildByName("txt_score")
                                                    local myChair = self._scene._wchair
                                                    local score = self._scene.lScoreTimesPlus[myChair+1][j]*self._scene._lScoreCell


                                                    
                                                    if score >= 0 then
                                                        cellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/win.png")
                                                        txtscore:setProperty(str, "game_res/gameOver/num_win.png", 16, 28, ".")
                                                      else
                                                        cellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/lose.png")
                                                        txtscore:setProperty(str, "game_res/gameOver/num_lose.png", 16, 28, ".")
                                                      end
                                                    if j==4 then 
                                                        score = self._scene.lScoreTimesPlus[myChair+1][j]
                                                        if score >= 0 then
                                                            cellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/txt_all_score1.png")
                                                        else
                                                            cellbg:getChildByName("txt_front"):setTexture("game_res/gameOver/txt_all_score2.png")
                                                        end
                                                        print("22222222222222222222score",score)
                                                    end 
                                                    score = math.abs(score)
                                                    txtscore:setString("."..score)
                                                    
                                                end
                                            end


                                      end      
                                end

                      end)))
            end 
          end 

          last = clone(v)
          end)))


        lastTime = clone(v)

      elseif v.actiontype == 1 then
        self:runAction(cc.Sequence:create(cc.DelayTime:create(temptime), cc.CallFunc:create(function()
          --showUserDunScore(v.actionindex)
          end)))
        temptime = temptime + 0.4
      end
    end
  end

    last = nil
 --统计比牌等待时间
  for k,v in pairs(actionlist) do
    if v.actiontype == 0 then

          if self._scene.m_bIsQuick then
              if  last and last.actionindex ~= v.actionindex  then
                  waittime = waittime + 0.8
              end
          else
              waittime = waittime + 0.8
          end

        last = clone(v)
    elseif v.actiontype == 1 then
      waittime = waittime + 0.4
    end
  end


  --显示开始比牌动画
  local compareNode = ExternalFun.loadCSB("compared.csb", self)
  compareNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  local compareAction = ExternalFun.loadTimeLine("compared.csb")
  compareAction:gotoFrameAndPlay(0, false)
  compareNode:runAction(compareAction)
  compareNode:runAction(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function()
    compareNode:removeFromParent()
    end)))
  ExternalFun.playSoundEffect("start_compare.mp3")


  self:runAction(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function()
      cardcompareshow(actionlist)
    end)))

  --显示打枪
  local function gunBeginShow()
    local gunNode = ExternalFun.loadCSB("gunAction.csb", self)
    gunNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
    local gunAction = ExternalFun.loadTimeLine("gunAction.csb")
    gunAction:gotoFrameAndPlay(0, false)
    gunNode:runAction(gunAction)
    gunNode:runAction(cc.Sequence:create(cc.DelayTime:create(1.2), cc.CallFunc:create(function()
      gunNode:removeFromParent()
      end)))
  end

  --显示全垒打
  local function dataiallShow()
    local dataiallNode = ExternalFun.loadCSB("HomeRun.csb", self)
    dataiallNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
    local dataiallAction = ExternalFun.loadTimeLine("HomeRun.csb")
    dataiallAction:gotoFrameAndPlay(0, false)
    dataiallNode:runAction(dataiallAction)
    dataiallNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.2), cc.CallFunc:create(function()
      dataiallNode:removeFromParent()
      end)))
  end
  
  local gunlist= self:getGunAcitionList(compareBuffer)
  
  --要显示打枪
  if #gunlist ~= 0 then
    self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
        gunBeginShow()
        ExternalFun.playSoundEffect("daqiang1.mp3")
      end)))
    waittime = waittime + 1.4
    for k,v in pairs(gunlist) do
      self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
        self:gunActionShow(v)
      end)))
      waittime = waittime + 1.8
    end
    
    --全垒打
    local bdetaiall = false
    if compareBuffer.wAllKillUser >=0 and compareBuffer.wAllKillUser < 4 then
      bdetaiall = true
    end
    if (bdetaiall == true) and (self._scene:getBankerMode()==0) then
      self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
        dataiallShow()
        ExternalFun.playSoundEffect("special1.mp3")
      end)))
      waittime = waittime + 1.4
    end
  end

  --判断是否特殊牌型显示  
  local specialtype, specialIndex = self:checkSpecialData(self._scene._bSpecialType)
  if specialtype ~= 0 then
    self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
        self:specialActionShow(specialtype, specialIndex)
      end)))

    waittime = waittime + 2.7 
    end

    self:runAction(cc.Sequence:create(cc.DelayTime:create(waittime), cc.CallFunc:create(function()
        for i=1,cmd.GAME_PLAYER do
          self:trustTeeDeal(i,false)
        end
        if self._scene.m_bjoinGame == true  and self._scene.m_isGameEnd == false then
          self._scene:sendCompleteCompare()
        end
      end)))
end

function GameViewLayer:isFanBei(chiairId)

      local myChair = self._scene._wchair
      local bei = 1
      if self._scene.m_wAllKillUser >=0 and self._scene.m_wAllKillUser < 4 then
           if chiairId == self._scene.m_wAllKillUser or myChair == self._scene.m_wAllKillUser then 
               bei  = bei*2
           end 
      end

      local userIndex = self._scene:getUserIndex(chiairId)
      if  self._scene._cbMaCardData ~= 0   then 
            if userIndex ~=1 then 
                for k,v in pairs(self._scene._cbHandCard) do
                      if v and v == self._scene:getMaCode() then
                        bei  = bei*2
                        break
                      end
                end
            end
            for k,v in pairs(self._scene._sortedCard[userIndex].Front) do

              if v and v == self._scene:getMaCode() then
                bei  = bei*2
                break
              end
            end

            for k,v in pairs(self._scene._sortedCard[userIndex].Mid) do
              if v and v == self._scene:getMaCode() then
                bei  = bei*2
                break
              end
            end

            for k,v in pairs(self._scene._sortedCard[userIndex].Tail) do
              if v and v == self._scene:getMaCode() then
                bei  = bei*2
                break
              end
            end
      end




            if self._scene.m_bAllWin[myChair +1][chiairId +1] >0 then
                bei  = bei*2
            end
            if self._scene.m_bAllWin[chiairId +1][myChair +1] >0 then
                bei  = bei*2
            end

  return bei

end


--获取打枪动作列表
function GameViewLayer:getGunAcitionList(compareBuffer)
  
  --打枪数据结构
  --gunindex 打枪
  --holeindex 被打枪
  local function getActionData(gunindex, holeindex,directionindex)
      local table = {}
      table.gunindex = gunindex
      table.holeindex = holeindex
      table.directionindex = directionindex
      return table
  end

  --打枪动作队列
  local gundatalist = {}

  --单蹦
  local record = {}
  print("#compareBuffer.bAllWin count is ",#compareBuffer.bAllWin)
  for i=1,#compareBuffer.bAllWin do
   local wchair = compareBuffer.bAllWin[i]
   for j=1,#wchair do
     local other = wchair[j]
     if 0 < other then
       local oneRecord = {}
       local firstIndex = self._scene:getUserIndex(i-1)
       local otherIndex = self._scene:getUserIndex(j-1)
       table.insert(oneRecord, firstIndex)
       table.insert(oneRecord, otherIndex)
       table.insert(record, oneRecord)
     end
   end
  end

 -- dump(record, "the record is ======", 6)

  for i=1,#record do
     local oneRecord = record[i]
     local firstIndex = oneRecord[1]
     local otherIndex = oneRecord[2]
     local directionindex = 0

     --每个玩家对应三个方向 具体索引参照CSB文件
     if firstIndex == 1 then
        if otherIndex == 2  then
          directionindex = 0
        elseif otherIndex == 3 then
           directionindex = 2
        elseif otherIndex == 4 then
           directionindex = 1
        end
     elseif firstIndex == 2 then
        if otherIndex == 1  then
          directionindex = 2
        elseif otherIndex == 3 then
           directionindex = 0
        elseif otherIndex == 4 then
           directionindex = 1
        end 
     elseif firstIndex == 3 then
        if otherIndex == 1  then
          directionindex = 1
        elseif otherIndex == 2 then
           directionindex = 2
        elseif otherIndex == 4 then
           directionindex = 0
        end 
     elseif firstIndex == 4 then
        if otherIndex == 1  then
          directionindex = 2
        elseif otherIndex == 2 then
           directionindex = 1
        elseif otherIndex == 3 then
           directionindex = 0
        end      
     end

     table.insert(gundatalist, getActionData(firstIndex,otherIndex,directionindex))
  end

  return gundatalist
end 

--打枪动作显示
function GameViewLayer:gunActionShow(actiondata)
    ExternalFun.playSoundEffect("bullets.mp3")
    local tempGunNode
    local gunnode = self._rootNode:getChildByName(string.format("gun_%d_%d", actiondata.gunindex,actiondata.directionindex))
    local holelist = {}
    for i=1,3 do
      table.insert(holelist, self._rootNode:getChildByName(string.format("hole_%d_%d", actiondata.holeindex, i-1)))
    end
    if gunnode ~= nil then
      local animation = cc.AnimationCache:getInstance():getAnimation("gun_anim")
      local action = cc.Repeat:create(cc.Animate:create(animation), 3) 
      gunnode:setVisible(true)
      gunnode:runAction(cc.Sequence:create(action, cc.DelayTime:create(0.3), cc.CallFunc:create(function()
          gunnode:setVisible(false)
        end)))
            ExternalFun.playSoundEffect("gun.wav")
      for i=1,3 do
        local hole = holelist[i]
        hole:runAction(cc.Sequence:create(cc.DelayTime:create(0.2*i), cc.CallFunc:create(function()
            hole:setVisible(true)
          end), cc.DelayTime:create(0.9-0.2*i), cc.CallFunc:create(function()
            hole:setVisible(false)
          end)))
      end
    end
end

--特殊牌型效果显示
function GameViewLayer:specialActionShow(specialtype, specialIndex)
  local specialNode = ExternalFun.loadCSB("Special_poker.csb", self)
  specialNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  local specialAction = ExternalFun.loadTimeLine("Special_poker.csb")
  specialAction:gotoFrameAndPlay(0, false)
  specialNode:runAction(specialAction)
  specialNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.4), cc.CallFunc:create(function()
    specialNode:removeFromParent()
    end)))
  ExternalFun.playSoundEffect("special"..specialtype..".mp3")


  local txtspecial = specialNode:getChildByName("santonghua_5")
  txtspecial:setTexture(cc.Director:getInstance():getTextureCache():addImage(string.format("Special_poker/txt_cardtype_%d.png", specialtype)))
    
  local cardlist = {}
  for k,v in pairs(self._scene._sortedCard[specialIndex].Tail) do
    table.insert(cardlist, v)
  end
  for k,v in pairs(self._scene._sortedCard[specialIndex].Mid) do
    table.insert(cardlist, v)
  end
  for k,v in pairs(self._scene._sortedCard[specialIndex].Front) do
    table.insert(cardlist, v)
  end

  local cardbg = specialNode:getChildByName("Column_4")
  for i=1,#cardlist do
    local pcard = CardSprite:createCard(cardlist[i])
    pcard:setMaCard("game_res/ma_image.png")
    pcard:setPosition(206+i*41, cardbg:getContentSize().height/2)
    cardbg:addChild(pcard)
    pcard:showCardBack(false)
    pcard:updateSprite()
  end

  local puser = self:getuseritemByindex(specialIndex)
  if puser ~= nil then
    local head = PopupInfoHead:createNormal(puser, 100)
    head:setAnchorPoint(cc.p(0.5,0.5))
    head:setPosition(cc.p(88,95))
    cardbg:addChild(head)

    --昵称
    local nick =  ClipText:createClipText(cc.size(110,30),puser.szNickName,"fonts/round_body.ttf",22)
    nick:setColor(cc.c3b(255, 255, 237))
    nick:setAnchorPoint(cc.p(0.5, 0.5))
    nick:setPosition(cc.p(90, 22))
    cardbg:addChild(nick)
  end

  --牌处理
  for i=1,cmd.GAME_PLAYER do
    if self._scene._bSpecialType[i] ~= 0 then
      self:showSpecialIcon(i, false)
      for k,v in pairs(self._CardList[i]) do
        v:showCardBack(false)
        v:updateSprite()
      end      
    end
  end
end

--获取是否有特殊牌型
function GameViewLayer:checkSpecialData(bSpecialType)
  dump(bSpecialType, "特殊牌型查询", 10)
  local specialtype = 0
  local specialIndex = 0
  for i=1,cmd.GAME_PLAYER do
    local temptype = bSpecialType[i]
    if temptype ~= 0 then
      if temptype > specialtype then
        specialtype = temptype
        specialIndex = i
      end
    end
  end
  return specialtype, specialIndex
end


--


--游戏结算
function GameViewLayer:showEndScore(endScore, bshow)

  if self._selectLayer then
    self._selectLayer:removeFromParent()
    self._selectLayer = nil
  end

  local showdelaytime = 0
  self:runAction(cc.Sequence:create(cc.DelayTime:create(showdelaytime), cc.CallFunc:create(function()
     
      self:resetData()
      self:setClockVisible(true)
      if GlobalUserItem.bPrivateRoom == true and PriRoom and not  GlobalUserItem.bPrivateGoldRoom then
          self:setClockVisible(false)
          if PriRoom:getInstance().m_tabPriData.dwPlayCount == PriRoom:getInstance().m_tabPriData.dwDrawCountLimit  then
              self:showReadyBtn(false)
          end
      end
    end)))

  --非正常结算
  if bshow ~= 0 then
  end

  local function getRandPos() 
    local beginpos = cc.p(yl.WIDTH/2, yl.HEIGHT/2)
    local offsetx = math.random()
    local offsety = math.random()

    return cc.p(beginpos.x + offsetx*120, beginpos.y + offsety*120)
  end

  local function getMoveAction(endpos ,inorout)
    local action = cc.MoveTo:create(0.27, endpos)
    if inorout == 0 then
      return cc.EaseOut:create(action, 1)
    else
      return cc.EaseIn:create(action, 1)
    end
  end

  if self._scene.m_bjoinGame == true then
    --自己总输赢
    local allscorecell = self.m_endScore:getChildByName("end_cell_1_3")
    allscorecell:setVisible(true)
    local txtscore = allscorecell:getChildByName("txt_score")
    local score = self._scene._selfscoreEnd*self._scene._lScoreCell
    print("score==================",score)
    if score == 0 then
      ExternalFun.playSoundEffect("no_award.mp3")
    elseif score > 0 then
      ExternalFun.playSoundEffect("win.mp3")
  
    elseif score < 0 then
      ExternalFun.playSoundEffect("lose.mp3")
    end

      if score >= 0 then
        allscorecell:getChildByName("txt_front"):setTexture("game_res/gameOver/txt_all_score1.png")
        score = math.abs(score)
        txtscore:setProperty(str, "game_res/gameOver/num_win.png", 16, 28, ".")
      else
        allscorecell:getChildByName("txt_front"):setTexture("game_res/gameOver/txt_all_score2.png")
        score = math.abs(score)
        txtscore:setProperty(str, "game_res/gameOver/num_lose.png", 16, 28, ".")
      end

    txtscore:setString(string.format(".%d", score))
  elseif GlobalUserItem.bPrivateGoldRoom then
     self:showReadyBtn(true)
  end
  
  --赢的玩家列表
  local winlist = {}
  local winnerIndex = 1
  local temp  = 0
  for i=1,cmd.GAME_PLAYER do
    if (self._scene._bUserGameStatus[i] == true) then
      local userIndex = self._scene:getUserIndex(i-1)

      local tempScore = self._scene._scoreEnd[i]
      local endscore = tempScore

      --分数效果显示
      local userbg = self._rootNode:getChildByName(string.format("user_%d", userIndex))
      local txtscore = userbg:getChildByName("txt_end_score")
      --txtscore:setVisible(true)
      txtscore:setScale(0.1)
     -- txtscore:runAction(cc.ScaleTo:create(0.37, 1))
      
      txtscore:setProperty(str, "game_res/num_add.png", 30, 39, "/")
      --txtscore:runAction(cc.MoveTo:create(0.37, cc.p(51, 145)))
      -- if math.abs(endscore) > 9999 and userIndex == 4  then
      --     txtscore:setAnchorPoint(cc.p(0,0.5))
      --     txtscore:setPositionX(10)
      --     txtscore:runAction(cc.MoveTo:create(0.37, cc.p(10, 212)))
      -- elseif userIndex == 2  then
      --     txtscore:setAnchorPoint(cc.p(1,0.5))
      --     txtscore:setPositionX(126)
      --     txtscore:runAction(cc.MoveTo:create(0.37, cc.p(126, 212)))
      -- else
      --     txtscore:setAnchorPoint(cc.p(0.5,0.5))
      --     txtscore:setPositionX(75)
      --     txtscore:runAction(cc.MoveTo:create(0.37, cc.p(75, 212)))
      -- end
       --输了
      if tempScore < 0 then
        txtscore:setProperty(str, "game_res/num_plus.png", 30, 39, "/")
        endscore = math.abs(endscore)
        ExternalFun.playSoundEffect("coinCollide.wav")

      elseif tempScore > 0 then
         if tempScore > temp then
           temp = tempScore
           winnerIndex = userIndex
         
         end
         
         table.insert(winlist, userIndex) 
      end
      txtscore:setString(string.format("/%d", endscore))
    end
  
  end

  --结算界面 
   self:showGameEndLayer()
  
    
  self:runAction(cc.Sequence:create(cc.DelayTime:create(0.8), cc.CallFunc:create(function()
      local goldnum = #self._goldlist
      local usernum = #winlist
      local cellnum = math.floor(goldnum/usernum)
  
      local addnum = 0
      for i=1,usernum do
        ExternalFun.playSoundEffect("coinCollide.wav")
        local movenum = cellnum
        if usernum == i then
          movenum = goldnum - addnum
        end
        local userindex = winlist[i]
        for j=addnum+1,movenum+addnum do
          local pgold = self._goldlist[j]
          local pos = self._seatposlist[userindex]
          if nil ~= pgold and nil ~= pos then
            local pos = self._seatposlist[userindex]
            pgold:runAction(cc.Sequence:create(cc.DelayTime:create(math.random()*0.3), getMoveAction(pos, 1), cc.CallFunc:create(function()
                pgold:setVisible(false)
              end)))
          end
        end
        addnum = addnum + movenum
      end
    end))) 
end



function GameViewLayer:showGameEndLayer()

    local time  = (self._scene.m_tTimeStartGame == 0) and 30 or self._scene.m_tTimeStartGame
    if not GlobalUserItem.bPrivateRoom  and not self.bIsVideo then

        self._scene:SetGameClock(self._scene._wchair,cmd.Clock_end, time)
        self:setClockVisible(true)
    end

    -- self:reFreshScore()

    -- local rootLayer, csbNode = ExternalFun.loadRootCSB("game_res/GameEndLayer.csb",self)
    -- rootLayer:setName("_gameEndLayer")
    -- rootLayer:setLocalZOrder(GameViewLayer.TopZorder)

    -- local endBg = csbNode:getChildByName("Sprite_1")
    -- local playNum = 0
    -- for i=1,cmd.GAME_PLAYER do
    --       if self._scene._bUserGameStatus[i] == true then
    --             playNum =playNum +1
    --             local cellBg =  endBg:getChildByName("FileNode_"..playNum):getChildByName("Sprite_1")
    --             endBg:getChildByName("FileNode_"..playNum):setVisible(true)
    --             local userIndex = self._scene:getUserIndex(i-1)
    --             local useritem = self._scene:getUserInfoByChairID(i - 1)

    --             if useritem then

    --                 if self._scene._wBankerUser == i-1 then
    --                     cellBg:getChildByName("Sprite_2"):setVisible(true)
    --                     cellBg:getChildByName("Sprite_2"):setLocalZOrder(10)
    --                 end

    --               local head = PopupInfoHead:createNormal(useritem, 89)
    --                 head:setAnchorPoint(cc.p(0.5,0.5))
    --                 head:setPosition(cc.p(57, 52))
    --                 cellBg:addChild(head)

    --                 --昵称
    --                 local tempnick = cellBg:getChildByName("name")
    --                 local nick =  ClipText:createClipText(cc.size(140, 22),useritem.szNickName,"fonts/round_body.ttf",21);
    --                 nick:setAnchorPoint(cc.p(0.5, 0.5))
    --                 nick:setPosition(cc.p(tempnick:getPosition()))
    --                 nick:setColor(tempnick:getColor())
    --                 cellBg:addChild(nick)
    --                 tempnick:removeFromParent()

    --                 --玩家ID
    --                 local gameID = cellBg:getChildByName("id")
    --                 gameID:setString("ID:"..useritem.dwGameID)
                  
    --                local cardNode = cellBg:getChildByName("cardNode")
    --                --local cardData = {self._scene._sortedCard[userIndex].Front,self._scene._sortedCard[userIndex].Mid,self._scene._sortedCard[userIndex].Tail}
    --                local posX= 0
    --                --dump(self._scene._cardData,"=========================")
    --                for j=1,#self._scene._cardData[i] do
                         
    --                     local handCard = CardSprite:createCard(self._scene._cardData[i][j])
    --                         handCard:setScale(0.7)
    --                         handCard:setPosition(posX, 0)
    --                         if self._scene._cardData[i][j] == self._scene:getMaCode()  then 
    --                             handCard:showMaCard(true)
    --                         end
    --                         cardNode:addChild(handCard)
    --                         posX = posX + 40
                         
    --                      if j==3 or j==8 then
    --                           posX = posX + 100
    --                       end
    --                end

    --                 local scoreTxt = cellBg:getChildByName("AtlasLabel_1")
    --                 local score = self._scene._scoreEnd[i]
    --                 if score <0 then

    --                   score = math.abs(score)
    --                   scoreTxt:setProperty("", "game_res/endLoseNum.png", 28, 41, "0")
    --                 end
    --                 scoreTxt:setString(":"..score)
    --             end

    --       end
    -- end

    if self._scene._selfscoreEnd < 0 then
        -- endBg:getChildByName("title"):setTexture("game_res/endTitleLose.png")
        ExternalFun.playSoundEffect("lose.mp3")
    else
        ExternalFun.playSoundEffect("win.mp3")
    end

    -- local closeBtn = endBg:getChildByName("btn_close")
    --   closeBtn:addTouchEventListener(function(sender,eventType)
    --     if eventType == ccui.TouchEventType.ended then
    --           rootLayer:removeFromParent()
    --           self._scene._bIsEndLayer = true
    --           if self._scene.m_bPriEnd and GlobalUserItem.bPrivateRoom then
    --               self._priView:showPrivateEndLayer()
    --           end
    --     end 
    --   end)

    --   local change = endBg:getChildByName("Button_change")
    --   change:addTouchEventListener(function(sender,eventType)
    --     if eventType == ccui.TouchEventType.ended then
    --           rootLayer:removeFromParent()
    --           self:changeTable()
    --     end 
    --   end)

    --   local continue = endBg:getChildByName("Button_continue")
    --   continue:addTouchEventListener(function(sender,eventType)
    --     if eventType == ccui.TouchEventType.ended then
    --           rootLayer:removeFromParent()
    --           self._scene._bIsEndLayer = true
              -- if self._scene.m_bPriEnd and GlobalUserItem.bPrivateRoom then
              --     self._priView:showPrivateEndLayer()
              -- else
                  -- self:onStartReady()
              -- end
    --     end 
    --   end)

      -- if  GlobalUserItem.bPrivateRoom then
      --     change:setVisible(false)
      --     continue:setPositionX(646)
      -- end

end

--显示庄家标识
function GameViewLayer:showBankerIcon(viewId)
  for i=1,cmd.GAME_PLAYER do
    local userbg = self._rootNode:getChildByName(string.format("user_%d",i))
    if nil ~= userbg then
      local bankericon = userbg:getChildByName("icon_banker")
      bankericon:setLocalZOrder(5)
      bankericon:setVisible(false)
      if i == viewId then
        bankericon:setVisible(true)
      end
    end
  end
end

--显示房主
function GameViewLayer:showFangzhuIcon(viewId)
  for i=1,cmd.GAME_PLAYER do
    local userbg = self._rootNode:getChildByName(string.format("user_%d",i))
    if nil ~= userbg then
      local fangzhuicon = userbg:getChildByName("icon_fangzhu")
      fangzhuicon:setVisible(false)
      if i == viewId and not GlobalUserItem.bPrivateGoldRoom then
        fangzhuicon:setVisible(true)
      end
    end
  end
end


-- 表情聊天
function GameViewLayer:onUserExpression(chatdata, viewId)

    local chatbg = self._rootNode:getChildByName(string.format("im_chat_%d",viewId))
    if nil ~= chatbg then
        chatbg:removeAllChildren()
        chatbg:stopAllActions()
    end
     
     local playerNode = self._rootNode:getChildByName(string.format("user_%d",viewId))
     print("用户表情================="..viewId..","..chatdata.wItemIndex)
    
    playerNode:stopAllActions()
    playerNode:removeChildByName("EMOJ_CHAT")
    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", chatdata.wItemIndex))
    --缓存表情
      --加载动画
    local frames = {}
    local actionTime = 0.2
    for i=1,32 do
        local strName = string.format("emoj%d_%d.png",chatdata.wItemIndex, i)

        local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
        if spriteFrame then
            table.insert(frames, spriteFrame)
        else
            break
        end
    end
    local  animation = cc.Animation:createWithSpriteFrames(frames,actionTime)    
    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", chatdata.wItemIndex))
    local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
    playerNode:addChild(spr)
    spr:setPosition(51, 51)
    spr:setName("EMOJ_CHAT")
    spr:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    playerNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()  
       playerNode:removeChildByName("EMOJ_CHAT")
    end)))

end

--显示聊天
function GameViewLayer:onUserChat(chatdata, viewId)
  self:removeChildByTag(TAG.CHAT_VIEW)
  self._chatLayer = nil
  local chatbg = self._rootNode:getChildByName(string.format("im_chat_%d",viewId))
  if nil ~= chatbg then
    chatbg:removeAllChildren()
    chatbg:stopAllActions()

    local _labChat = cc.LabelTTF:create(chatdata.szChatString, "fonts/round_body.ttf", 20, cc.size(150,0), cc.TEXT_ALIGNMENT_CENTER)        
    chatbg:addChild(_labChat)

    local labSize = _labChat:getContentSize()
    if labSize.height >= 40 then
        chatbg:setContentSize(180, labSize.height + 50)
    else
        chatbg:setContentSize(180, 50)
    end
    _labChat:setPosition(chatbg:getContentSize().width * 0.5, chatbg:getContentSize().height * 0.5+10)
    chatbg:runAction(self.m_actTip) 
  end
end

--开始语音聊天
function GameViewLayer:onUserVoiceStart(viewId)
  local chatbg = self._rootNode:getChildByName(string.format("im_chat_%d",viewId))
  if nil ~= chatbg then
    chatbg:removeAllChildren()
    chatbg:stopAllActions()

    chatbg:setContentSize(120, 50)
    chatbg:setVisible(true)
    chatbg:setScale(1)

    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("record_play_ani_1.png")
    if nil ~= frame then
      local vioceanim = cc.Sprite:createWithSpriteFrame(frame)
      vioceanim:runAction(self.m_actVoiceAni)
      vioceanim:setPosition(chatbg:getContentSize().width * 0.5, chatbg:getContentSize().height * 0.5+5)
      if viewId == 1 or viewId == 4 then
        vioceanim:setRotation(180)
      end
      chatbg:addChild(vioceanim)
    end
  end
end

--结束语音聊天
function GameViewLayer:onUserVoiceEnded(viewId)
  local chatbg = self._rootNode:getChildByName(string.format("im_chat_%d",viewId))
  if nil ~= chatbg then
    chatbg:removeAllChildren()
    chatbg:stopAllActions()
    chatbg:setVisible(false)
  end
end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
        local beginPoint = nil
        local endPoint = nil

        --获取头像坐标
        for k,v in pairs(self._userList) do
              if v then
                  local head = self._rootNode:getChildByName(string.format("user_%d",v.pos))
                  if  v.user.dwUserID ==  dwUserID then
                        beginPoint = cc.p(head:getPosition())
                  elseif v.user.dwUserID ==  dwRecvUserID then
                        endPoint = cc.p(head:getPosition())
                  end
              end
        end

      if beginPoint and endPoint then
              local actSprite = cc.Sprite:create(string.format("public/prop_%d.png",dwPropID)):move(beginPoint):addTo(self._rootNode)
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
                      self._rootNode:addChild(spine)
                  end)))
              end     
      end
end


--获取玩家人数
function GameViewLayer:getUserNum()
  if self._rootNode == nil then
    return 0
  end

  local num = 0
  for i=1,cmd.GAME_PLAYER do
    local headBG = self._rootNode:getChildByName(string.format("user_%d",i))
    if headBG ~= nil then
      if headBG:isVisible() == true then
        num = num + 1
      end
    end
  end
  
  return num
end

function GameViewLayer:onClickLocation()
    local playerList = {}
    local validPlayerNum = 0
    for i=1,cmd.GAME_PLAYER do
        local viewid = self._scene:getUserIndex( i-1 )
        local useritem = self._scene.m_tabUserItem[i]
        -- local isVisible = self.nodePlayer[viewid]:isVisible()
        if useritem then
            useritem = clone(useritem)
            local playerData = {}
            playerData.uid = useritem.dwUserID
            playerData.viewid = viewid
            playerData.ip = ""
            playerData.dLatitude = 0   --纬度
            playerData.dLongitude = 0 --经度
            playerData.szPlaceName = "未知"
            playerData.ipStr = ""

            table.insert(playerList, playerData)
            validPlayerNum = validPlayerNum + 1
        end
    end

    local uiLocation = require(appdf.GAME_SRC .. "qipai.thirteenzj.src.views.layer.uiLocation")
    local locationObj = uiLocation:ctor(self, playerList, validPlayerNum)
end
return GameViewLayer