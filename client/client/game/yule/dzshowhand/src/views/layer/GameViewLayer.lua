--//////////////////////////////////////////////////////////////////////////////////////
-- Author: Tang
-- Date: 2016-12-08 15:41:53
--/////////////////////////////////////////////////////////////////////////////////////
local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.yule.dzshowhand.src"
local RES_PATH = "game/yule/dzshowhand/res/"

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local cmd = appdf.req(module_pre..".models.CMD_Game")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local QueryDialog   = appdf.req("app.views.layer.other.QueryDialog")
local CardSprite = require(module_pre..".views.layer.CardSprite")
local GameChat = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local SpineObj = appdf.req("client.src.external.SpineObj")



GameViewLayer.Enum = 
{
  tag_readybtn = 1,
  tag_allIn    = 2,
  tag_add      = 3,
  tag_pre_btn  = 4, --+3
  tag_opreate_btn = 10, --+ 10
  tag_chat        = 40,
  tag_chatview    = 50,

  tag_menu     = 100,
  tag_standUp  = 101,
  tag_cardType = 102,
  tag_scoreAdd = 103,--补充计分牌
  tag_dismiss  = 104,
  tag_back     = 105,
  tag_priRecord= 106,
  tag_invite   = 107,
  tag_addChip  = 300,
  tag_set  =301,
  tag_last  = 302,
  tag_location  = 303,
  tag_change = 304,
  tag_sit = 305,
  tag_begin = 306,
         
}

local TAG = GameViewLayer.Enum
local TOP_ZOrder      = 30 
function GameViewLayer:priGameLayerZorder()
    return 30
end

function GameViewLayer:dismissResult()
  self._bDismissed = true
end

function GameViewLayer:ctor(scene)

  --初始变量
  self._scene = scene
  self._userList = {}
  self._bMenu = false
  self._bDismissed = false
  self.m_bPreOperate = false   --预操作标识
  self.m_nPreOperateCode = 65535 --无效操作码 (1跟任何注 2弃牌 3过牌)
  self.m_nOperateTag = 65535
  self.m_lCurSlider  = 0       --滑动加注额
  self.m_lDeskChip = 0
  self.m_bShowhand = {false,false,false,false,false,false,false,false,false}

  self._cardList = {{},{},{},{},{},{},{},{},{}}
  self.m_centerCard = {}


  self.isEnabledChange = true


  --音乐音效
  self._bSound = GlobalUserItem.bVoiceAble
  self._bMusic = GlobalUserItem.bSoundAble

  --导入资源
  self:gameDataInit()
  self:loadRes()
  self:loadAnimation()

  --初始界面
  self:initCsbRes()

  --聊天窗口层
  self.m_GameChat = GameChat:create(scene._gameFrame)
    :setLocalZOrder(50)
        :addTo(self)

  --注册事件
  ExternalFun.registerNodeEvent(self)
  --if GlobalUserItem.bVoiceAble == true then
  ExternalFun.playBackgroudAudio("common/bg2.wav")
         --self._scene:PlaySound("sound/common/bg2.wav")
  --end
end

function GameViewLayer:onExit()
  self:deletePriSecond()
  self:gameDataReset()
  --self.m_actTip:release()
  --self.m_actTip = nil
  self.m_actVoiceAni:release()
  self.m_actVoiceAni = nil
  cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.clockShd)
  cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.schedule)

  if self._beginTimeId then
    cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
    self._beginTimeId = nil
  end 

  self:removeTimer()
end

function GameViewLayer:onResetView()
    --界面重置
    for i,v in ipairs(self._cardList) do
      local cardlist = v
      for j=1,#cardlist do
        local card = cardlist[j]
        if card then
            card:removeFromParent()
        end
      end
    end

    for i,v in ipairs(self.m_centerCard) do
       if v then
            v:removeFromParent()
       end
    end

    self:removeChildByTag(TAG.tag_addChip)
	
	for i=1,cmd.GAME_PLAYER do
		--桌面筹码
      self:showAddChipScore(i,false)
      self._rootNode:getChildByName("desk_chip_"..i):setVisible(false)
	end
		
    self:stopAllActions()
    self.m_bPreOperate = false   --预操作标识
    self.m_nPreOperateCode = 65535 --无效操作码 (1跟任何注 2弃牌 3过牌)
    self.m_nOperateTag = 65535
    self.m_lCurSlider  = 0       --滑动加注额
    self.m_lDeskChip = 0
    self.m_bShowhand = {false,false,false,false,false,false,false,false,false}
    self.m_centerCard = {}
    self._cardList = {{},{},{},{},{},{},{},{},{}}

    self._scene:resetData()
    self._rootNode:getChildByName("Node_1"):removeAllChildren()
    self._bottomNode:getChildByName("btn_ctrl_layout"):setVisible(false)
    --不显示我的牌型
    self._rootNode:getChildByName("cardType"):setVisible(false)

    
    local buyLayer =  self:getChildByTag(88)
    if buyLayer and buyLayer:isVisible() then
      buyLayer:removeFromParent()
    end
    self:stopAllActions()
    
end

function GameViewLayer:resetData()
  self._cardList = {{},{},{},{},{},{},{},{},{}}
  self._scene:resetData()
end

function GameViewLayer:loadRes( )
  --加载扑克
  cc.Director:getInstance():getTextureCache():addImage("game_res/game_poker.png")
end

function GameViewLayer:unLoadRes()
  AnimationMgr.removeCachedAnimation(cmd.VOICE_ANIMATION_KEY)
  cc.Director:getInstance():getTextureCache():removeTextureForKey("game_res/game_poker.png")
  cc.Director:getInstance():getTextureCache():removeUnusedTextures()
  cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
end

function GameViewLayer:showRoleAnimation(indexNum)
 

    local role = self:getChildByName("_roleani_")
    if role then
        role:stopAllActions()
        self:removeChildByName("_roleani_")
    end
    --local time ={1.25,1.5,1.7}
    local csbPath ={"meinv_duzui.csb","meinv_feiwen.csb","meinv_meiyan.csb"}
    local ani1 = ExternalFun.loadCSB(csbPath[indexNum], self)
    ani1:setName("_roleani_")
    ani1:setPosition(0,0)
    local gunAction = ExternalFun.loadTimeLine(csbPath[indexNum])
    gunAction:gotoFrameAndPlay(0, false)
    ani1:runAction(gunAction)
    ani1:runAction(cc.Sequence:create(cc.DelayTime:create(5), cc.CallFunc:create(function()
        local indext = math.random(3)
        self:showRoleAnimation(indext ==0 and  1 or indext )
      end)))

end


function GameViewLayer:gameDataInit()

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

--init csb
function GameViewLayer:initCsbRes()
    local csbNode = cc.CSLoader:createNode("game_res/Game.csb");
    csbNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
    self:addChild(csbNode)
    self._rootNode = csbNode

    self._bottomNode = self._rootNode:getChildByName("Node_0")

    for i=1,cmd.GAME_PLAYER do
      --self:showSitStatus(0,i)
      local seat = self._bottomNode:getChildByName("seat_"..i.."_0")
      seat:setVisible(true)
      seat:setEnabled(true)
      local seat1 = self._bottomNode:getChildByName(string.format("seat_%d", i))
      seat1:setVisible(false)
    end

    -- --坐下
    -- local seat0 = self._bottomNode:getChildByName("seat_1_0")
    -- seat0:stopAllActions()
    -- seat0:addTouchEventListener(function(sender,eventType)
    --   if eventType == ccui.TouchEventType.ended then
    --       self:stopSitAnimation()
    --       --设置携带
    --       self:setTakeChip()
    --   end
    -- end)

    self:initButtonEvent()

    self:showRoleAnimation(1)

    self.isTip = false
    self.curTime = 0

    local function timeUpdate()

        if self._timer and not self.isTip  then
            if self.curTime ~= 0 and self._timer:getPercentage()*0.01*self.curTime <= 5 then
                self.isTip = true
                self:playSound("timetip.wav")
            end
        end
    end
    
    self.clockShd = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 1, false)


    -- 电量,时间，信号
    self.batteryBar = self._rootNode:getChildByName("Bar_ele")
    self.batteryBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())

    self.txt_time = self._rootNode:getChildByName("Text_time"):setString(os.date("%H:%M"))

    self.netStatus = self._rootNode:getChildByName("netStatus")

    self.schedule = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
        self.batteryBar:setPercent(MultiPlatform:getInstance():getBatteryLevel())

        self.txt_time:setString(os.date("%H:%M"))

        local netType = MultiPlatform:getInstance():getNetworkType()
        local Strength = MultiPlatform:getInstance():getNetworkStrength()
        local TypePic = {"4G_","wifi_"}
        self.netStatus:setSpriteFrame(display.newSprite("game_res/"..TypePic[netType]..Strength..".png"):getSpriteFrame()  )
    end, 1, false)
    
end


function GameViewLayer:setSeatEvent()
  
      local table = self._scene._gameFrame:GetTableID()

      for i=1,cmd.GAME_PLAYER  do
        local seat = self._bottomNode:getChildByName("seat_"..i.."_0")
        local chair = math.mod(i-1 + self._scene._wchair,cmd.GAME_PLAYER)

        seat:addTouchEventListener(function (sender,eventType)
              if eventType == ccui.TouchEventType.ended and self.isEnabledChange and not GlobalUserItem.bPrivateRoom then
                  if self._scene.m_cbGameStatus == cmd.GS_FREE and self._scene.m_nowRoomStatus == cmd.GS_FREE then
                      print("==========================chair,"..chair)
                      self.isEnabledChange = false
                      sender:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                        self.isEnabledChange = true
                      end)))
    
                      self._scene._gameFrame:QueryChangeChair(table ,chair)
                      for j=1, cmd.GAME_PLAYER  do
                          self._bottomNode:getChildByName(string.format("seat_%d", j)):setVisible(false)
                          self._rootNode:getChildByName("ready_"..j):setVisible(false)
                          self._bottomNode:getChildByName("seat_"..j.."_0"):setVisible(true)
                      end

                      self._scene._wchair = yl.INVALID_CHAIR

                  else
                      showToast(self._scene, "当局游戏结束后，才能更换座位！", 2)
                  end
              elseif not self.isEnabledChange then
                showToast(self._scene, "更换桌子或座位频繁，请稍后再试！", 2)
              elseif GlobalUserItem.bPrivateRoom then
                showToast(self._scene, "只有在金币场允许更换座位！", 2)
              end
            end)
      end


end

function GameViewLayer:loadAnimation()

    -- 语音动画
  AnimationMgr.loadAnimationFromFrame("record_play_ani_%d.png", 1, 3, cmd.VOICE_ANIMATION_KEY)

  -- 聊天动画
  --local sc = cc.ScaleTo:create(0.1, 1.0, 1.0)
  --local show = cc.Show:create()
  --local spa = cc.Spawn:create(sc, show)
  --self.m_actTip = cc.Sequence:create(spa, cc.DelayTime:create(2.0), cc.ScaleTo:create(0.1, 0.00001, 1.0), cc.Hide:create())
  --self.m_actTip:retain()

  -- 语音动画
  local param = AnimationMgr.getAnimationParam()
  param.m_fDelay = 0.1
  param.m_strName = cmd.VOICE_ANIMATION_KEY
  local animate = AnimationMgr.getAnimate(param)
  self.m_actVoiceAni = cc.RepeatForever:create(animate)
  self.m_actVoiceAni:retain()
end

function GameViewLayer:initButtonEvent()
  local touchFunc = function ( sender ,TouchEventType )
      if TouchEventType == ccui.TouchEventType.ended then
         self:onButtonClickedEvent(sender:getTag(), sender)
      end
  end

  --定位
  local btn = self._rootNode:getChildByName("btn_location")
  btn:setTag(TAG.tag_location)
  btn:addTouchEventListener(touchFunc)
  self.locationBtn = btn

  local ani1 = ExternalFun.loadCSB("locationAnim.csb", btn)
  ani1:setTag(163)
  ani1:setPosition(0,0)

  local aniAction = ExternalFun.loadTimeLine("locationAnim.csb")
  aniAction:gotoFrameAndPlay(0,25,true)
  ani1:runAction(aniAction)



  --准备
  local btn = self._rootNode:getChildByName("btn_ready")
  btn:setTag(TAG.tag_readybtn)
  btn:addTouchEventListener(touchFunc)
  self.m_readyBtn = btn

  local role = self._rootNode:getChildByName("Image_2")
  --role:setTag(TAG.tag_readybtn)
  role:setTouchEnabled(true)
  role:addTouchEventListener(function (sender,eventType)
            if eventType == ccui.TouchEventType.ended then
                self:showRoleAnimation(2)
            end
          end)


  --菜单
  local btn = self._rootNode:getChildByName("btn_menu")
  btn:setTag(TAG.tag_menu)
  btn:addTouchEventListener(touchFunc)

  local menuBG = self._rootNode:getChildByName("Panel_menu")

--桌面换桌
  self.btn_change1 = self._rootNode:getChildByName("btn_change")
  self.btn_change1:setTag(TAG.tag_change)
  self.btn_change1:addTouchEventListener(touchFunc)

--扩展换桌
  self.btn_change2 = menuBG:getChildByName("menu_bg_1"):getChildByName("btn_change")
  self.btn_change2:setTag(TAG.tag_change)
  self.btn_change2:addTouchEventListener(touchFunc)


  -- --起立
  -- btn = menuBG:getChildByName("btn_standup")
  -- btn:setTag(TAG.tag_standUp)
  -- btn:addTouchEventListener(touchFunc)

  --牌型
  btn = self._rootNode:getChildByName("btn_cardType")
  btn:setTag(TAG.tag_cardType)
  btn:addTouchEventListener(touchFunc)

  --牌局回顾
  btn = self._rootNode:getChildByName("btn_last")
  btn:setTag(TAG.tag_last)
  btn:addTouchEventListener(touchFunc)
  

  --买入
  btn = self._rootNode:getChildByName("btn_buy")
  btn:setTag(TAG.tag_scoreAdd)
  btn:addTouchEventListener(touchFunc)

  --返回
  btn = menuBG:getChildByName("menu_bg_1"):getChildByName("btn_back")
  btn:setTag(TAG.tag_back)
  btn:addTouchEventListener(touchFunc)

  --返回
  btn = menuBG:getChildByName("menu_bg_1"):getChildByName("btn_set")
  btn:setTag(TAG.tag_set)
  btn:addTouchEventListener(touchFunc)

  --聊天
  btn = self._rootNode:getChildByName("btn_chat")
  btn:setTag(TAG.tag_chat)
  btn:addTouchEventListener(touchFunc)

  --语音
  btn = self._rootNode:getChildByName("btn_voice")
  btn:setVisible(true)
  btn:addTouchEventListener(function(sender,eventType)
    if eventType == ccui.TouchEventType.began then
      if self._scene.m_bLookOn then
        showToast(self, "观战中不可使用该功能!", 1)
      else
        self:getParentNode():getParentNode():startVoiceRecord()
      end
    elseif eventType == ccui.TouchEventType.ended 
      or eventType == ccui.TouchEventType.canceled then
      self:getParentNode():getParentNode():stopVoiceRecord() 
    end 
  end)

  --坐下按钮
  self.btSit = self._rootNode:getChildByName("btn_sit"):setPositionY(-47)
  self.btSit:setVisible(false)
  -- self.btSit:setLocalZOrder(5)
  self.btSit:setTag(TAG.tag_sit)
  self.btSit:addTouchEventListener(touchFunc)

  --开始按钮
  self.btStart = self._rootNode:getChildByName("btn_begin")
  self.btStart:setEnabled(true)
  self.btStart:setVisible(false)
  -- self.btStart:setLocalZOrder(5)
  self.btStart:setTag(TAG.tag_begin)
  self.btStart:addTouchEventListener(touchFunc)
end

function GameViewLayer:getParentNode( )
    return self._scene
end

function GameViewLayer:showSitStatus(status,viewid)
  if viewid<1 or viewid>9 then
    assert(false)
    return
  end

  local seat0 = self._bottomNode:getChildByName(string.format("seat_%d_0", viewid))
  local seat1 = self._bottomNode:getChildByName(string.format("seat_%d", viewid))

  if status == 0 then
      seat0:setVisible(true)
      seat1:setVisible(false)
  elseif status == 1 then
      seat0:setVisible(false)
      seat1:setVisible(true)
  end
end

function GameViewLayer:showReadyFlag(viewid,isVisible)
  print(isVisible,viewid,debug.traceback())
      if viewid >=1 and viewid <=9 then 
        self._rootNode:getChildByName("ready_"..viewid):setVisible(isVisible)
      end
      self._rootNode:getChildByName("ready_1"):setPositionY(-20)

end

function GameViewLayer:setSitStatus(viewid,isVisible)
    local sitView = self._rootNode:getChildByName("Node_0"):getChildByName("seat_"..viewid.."_0")

    sitView:removeChildByTag(888)

    
    if not isVisible then
          
          local sprite = cc.Sprite:create(RES_PATH.."game_res/icon_sit.png"):move(55,55):addTo(sitView)
          sprite:setTag(888)
          convertToGraySprite(sprite)
    end

end

function GameViewLayer:showLastTurn(cmd_data)
        local csbNode = ExternalFun.loadCSB(RES_PATH.."game_res/backLayer.csb", self)
        
        csbNode:setLocalZOrder(50)
        self.lastData = clone(cmd_data)
        local lastBg= csbNode:getChildByName("Sprite_1")
          local btn = lastBg:getChildByName("Button_close")
            
            btn:addTouchEventListener(function (ref, tType)
                if tType == ccui.TouchEventType.ended then
                    csbNode:removeFromParent()
                end
            end)

            local btn = csbNode:getChildByName("Panel_2")
            
            btn:addTouchEventListener(function (ref, tType)
                if tType == ccui.TouchEventType.ended then
                    csbNode:removeFromParent()
                end
            end)

      local Panel_2 = lastBg:getChildByName("Panel_1")
            self._listView = cc.TableView:create(cc.size(Panel_2:getContentSize().width,Panel_2:getContentSize().height))
            self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
            self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
            self._listView:setAnchorPoint(0,0)
            self._listView:setPosition(0,0)
            self._listView:setDelegate()
            self._listView:addTo(Panel_2)
            self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
            self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
            self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
            self._listView:reloadData()
            

end


-- 子视图大小
function GameViewLayer:cellSizeForTable(view, idx)
    return 573, 110
end

function GameViewLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    local index = 0 
    local cueIndex = 0
    for i=1,9 do
        if self.lastData.szNickName[1][i] ~="" then
          index = index +1
          if idx +1 == index then
                cueIndex = i
          end
        end
    end

    if cueIndex == 0 then
        return cell
    end



    local csb = ExternalFun.loadCSB(RES_PATH.."game_res/backCell.csb", cell)
    csb:setPosition(286.5, 55)

    --用户昵称
    --local clipNick = ClipText:createClipText(cc.size(150, 25), self.lastData.szNickName[1][cueIndex])
    local txt= csb:getChildByName("Text_1")
    --clipNick:setTextColor(cc.WHITE)
    --clipNick:setVisible(true)
    --clipNick:setString(self.lastData.szNickName[1][cueIndex])
    --clipNick:setAnchorPoint(0.5,0.5)
    --clipNick:setPosition(cc.p(-193,0))
    --csb:addChild(clipNick)

    local clipNick = ClipText:createClipText(cc.size(100, 20), self.lastData.szNickName[1][cueIndex], "" , 18)
      clipNick:setAnchorPoint(cc.p(0.5, 0.5))
      clipNick:setPosition(txt:getPosition())
      clipNick:setName("Text_1")
      csb:addChild(clipNick)



    local cardType = csb:getChildByName("Text_2")
    if self.lastData.cbPlayStatus[1][cueIndex] ==0 then
        cardType:setString("弃牌")
        cardType:setColor(cc.c3b(255, 255, 255))

        for i=1,2 do
            local card = CardSprite:createCard()
                card:setPosition(cc.p(-78+(i-1)*18,0))
                card:setScale(0.4)
                csb:addChild(card)
        end

    else
        cardType:setString(""..cmd.CardType[self.lastData.cbCardType[1][cueIndex]])
        cardType:setColor(cc.c3b(208, 190, 120))

        for i=1,2 do
            local card = CardSprite:createCard(self.lastData.cbHandData[cueIndex][i])
                card:setPosition(cc.p(-78+(i-1)*18,0))
                card:setScale(0.4)
                csb:addChild(card)
                card:showCardBack(false)
                card:setColor(cc.c3b(127,127,127))
            for j=1 , 5 do
              if self.lastData.cbHandData[cueIndex][i] == self.lastData.cbMaxCard[cueIndex][j] then
                card:setColor(cc.c3b(255,255,255))
                break
              end
            end 
        end

        local point = {cc.p(-6,0),cc.p(12,0),cc.p(30,0),cc.p(82,0),cc.p(132,0)}
        for i=1,5 do
            local card = CardSprite:createCard(self.lastData.cbCenterCardData[1][i])
                card:setPosition(point[i])
                card:setScale(0.4)
                csb:addChild(card)
                card:showCardBack(false)
                card:setColor(cc.c3b(127,127,127))

                for j=1 , 5 do
                  if self.lastData.cbCenterCardData[1][i] == self.lastData.cbMaxCard[cueIndex][j] then
                    card:setColor(cc.c3b(255,255,255))
                    break
                  end
                end 
        end

    end

    local score = csb:getChildByName("Text_3")
    if self.lastData.lScore[1][cueIndex] >= 0 then
      score:setString("+".. self.lastData.lScore[1][cueIndex])
      score:setColor(cc.c3b(215, 73, 73))
    else
      score:setString("".. self.lastData.lScore[1][cueIndex])
      score:setColor(cc.c3b(123, 201, 112))
    end

    if self.lastData.wMinChipInUser == cueIndex -1  then
        csb:getChildByName("Sprite_x"):setVisible(true)
    end

    if self.lastData.wMaxChipInUser == cueIndex -1 then
      csb:getChildByName("Sprite_d"):setVisible(true)
    end

    return cell
end

-- 子视图数目
function GameViewLayer:numberOfCellsInTableView(view)
  local num = 0 
    for i=1,9 do
        if self.lastData.szNickName[1][i] ~="" then
          num = num +1
        end
    end
    --print("numberOfCellsInTableView==============="..num)
    return num
end


--坐下动画
function GameViewLayer:showSitAnimation()
  local seat = self._bottomNode:getChildByName("seat_1_0")
  seat:stopAllActions()
  local scale0 = cc.ScaleTo:create(0.4,1.2)
  local scale1 = cc.ScaleTo:create(0.4,1.0)
  local sequcen = cc.Sequence:create(scale0,scale1)
  local repeats = cc.Repeat:create(sequcen, cmd.INT_MAX)
  seat:runAction(repeats)
end

function GameViewLayer:stopSitAnimation()
   local seat = self._bottomNode:getChildByName("seat_1_0")
   seat:stopAllActions()
end

--用户信息
function GameViewLayer:showUserInfo(useritem,lTakeScore)
    --dump(useritem,"==================")

    if not useritem or self._scene._wchair == yl.INVALID_CHAIR then
 
      return
    end
    if useritem.cbUserStatus == yl.US_LOOKON then
      return
    end

    if self._scene:GetMeChairID() == useritem.wChairID and useritem.cbUserStatus == yl.US_READY then
      self:showReadyBtn(false)
      self:removeTimer()
      self:onResetView()
    end
    local viewid = self._scene:getUserIndex(useritem.wChairID)
    local seat = self._bottomNode:getChildByName(string.format("seat_%d", viewid))
    local headbg = seat:getChildByName("head_bg")
    local image = seat:getChildByName("Image_1")
    local txt_nick = seat:getChildByName("Text_1")
    local take_score = seat:getChildByName("Text_2")
    local txt_operate = seat:getChildByName("txt_operate")
    local seatButton = self._bottomNode:getChildByName("seat_"..viewid.."_0")
    seatButton:setVisible(false)

    seat:setVisible(true)
    seat:removeChildByTag(666)--
    txt_nick:setVisible(false)
    txt_operate:setVisible(false)
    image:setVisible(false)

    seat:removeChildByName("_clipNick")
    local clipNick = ClipText:createClipText(cc.size(100, 20), useritem.szNickName, "" , 18)
      clipNick:setAnchorPoint(cc.p(0.5, 0.5))
      clipNick:setPosition(txt_nick:getPosition())
      clipNick:setName("_clipNick")
      seat:addChild(clipNick)

    if self._rootNode:getChildByName("Node_1"):getChildByTag(1000+viewid) then
        clipNick:setVisible(false)
    end

    local posTable = 
    {
      cc.p(667,375-195),
      cc.p(667+326-510,375-190),
      cc.p(667+494-510,375-25),
      cc.p(667+494-510,375+158-330),
      cc.p(667+205-510,375+270-330),
      cc.p(667-205,375+270-330),
      cc.p(667-494,375+158-330),
      cc.p(667-494,375-25),
      cc.p(667-326,375-190)
    }

    local anchorTable = 
    {
      cc.p(0,0),
      cc.p(1,0),
      cc.p(1,0),
      cc.p(1,1),
      cc.p(1,1),
      cc.p(0,1),
      cc.p(0,1),
      cc.p(0,0),
      cc.p(0,0)
    }
   --头像
    local head = PopupInfoHead:createNormal(useritem, 105)
    local param = {}
    param.isProp = true     --显示道具互动，百人游戏不显示
    param.imgPath = "game/popBg.png" --弹出框背景
    head:setPropInfo(param)
    head:enableInfoPop(true, posTable[viewid], anchorTable[viewid])
    headbg:removeChildByTag(1)
    head:setTag(1)
    head:setAnchorPoint(cc.p(0.5,0.5))
    head:setPosition(cc.p(headbg:getContentSize().width/2,headbg:getContentSize().height/2))
    headbg:addChild(head)

    if useritem.cbUserStatus == yl.US_OFFLINE then
      if nil ~= convertToGraySprite then
        -- 灰度图
        if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
          convertToGraySprite(head.m_head.m_spRender)
        end
      end
    end
    
    --昵称
    if not self.m_bShowhand[useritem.wChairID+1] or (self._scene.m_cbPlayStatus[useritem.wChairID+1] == 1) then 
        clipNick:setString(useritem.szNickName)
    end
    
    --携带筹码
    if lTakeScore and take_score then
      take_score:setString(""..ExternalFun.formatScoreText(lTakeScore))
    end

    --self:checkAdress()
end

function GameViewLayer:showAdressLayer()

 
    local addressLayer = cc.CSLoader:createNode("game_res/LocationLayer.csb"):addTo(self, 51)
    local node = addressLayer:getChildByName("Sprite_1")
    -- self._scene:showPopWait()

    local action = cc.Sequence:create(cc.FadeOut:create(0.5), cc.FadeIn:create(0.5))
    local repaction = cc.RepeatForever:create(action)
    node:getChildByName("tips_2"):runAction(repaction)

    local btn = node:getChildByName("Button_close")
    btn:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended  then
        addressLayer:removeFromParent()
      end 
    end)



  for i=1 , 9 do
    local useritem = self._scene.m_tabUserItem[i]
    if useritem then
        local seat = self._bottomNode:getChildByName(string.format("seat_%d", self._scene:getUserIndex(i-1)))
        local headbg = seat:getChildByName("head_bg")
        local head = headbg:getChildByTag(1)
        if head and head:isVisible()  then--
            print("请求定位",i,self._scene:getUserIndex(i-1))
            head:queryUserLociton(useritem.dwUserID)
        end
    end
  end

  self:runAction(cc.Sequence:create(cc.DelayTime:create(0.6),cc.CallFunc:create(function ()
    
    
    self.m_tipsTable = {}
    for i=1,9 do
      local seat1 = self._bottomNode:getChildByName(string.format("seat_%d", i))
      local headbg1 = seat1:getChildByName("head_bg")
      local head1 = headbg1:getChildByTag(1)
      if head1 and head1:isVisible() then
        local userInfo1 = head1.m_userInfo
        dump(userInfo , "1111")
        local la1  = 0
        local lo1  = 0 
        if userInfo1 then
          la1  = userInfo1.dLatitude
          lo1 = userInfo1.dLongitude
        end

        
        for j=i+1,9 do
          local la2 = 0
          local lo2 = 0
          local userInfo2
          local seat2 = self._bottomNode:getChildByName(string.format("seat_%d", j))
          local headbg2 = seat2:getChildByName("head_bg")
          local head2 = headbg2:getChildByTag(1)
          if head2 and head2:isVisible() then
            userInfo2 = head2.m_userInfo
          dump(userInfo2,"222222")
            if userInfo2 then
              la2  = userInfo2.dLatitude
              lo2 = userInfo2.dLongitude
              
            end
          end
          la1 = 100
          lo1 = 100
          la2 = 100
          lo2 = 100
          if la1 ~= 0 and lo1 ~= 0 and la2 ~= 0 and lo2 ~= 0  then

            local dst = self:algorithm(lo1 , la1 , lo2 , la2)
            if dst <=10 and userInfo1 and userInfo2 then
                local nameT = {}
                nameT[1] = userInfo1.szNickName
                nameT[2] = userInfo2.szNickName
                table.insert(self.m_tipsTable , nameT) 
            end
          end
        end
      end
    end
  
    local tmpcontent = node:getChildByName("Panel_1")
    local contentsize = tmpcontent:getContentSize()
    self._listView = cc.TableView:create(contentsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listView:setPosition(tmpcontent:getPosition())
    self._listView:setPositionX(420)
    self._listView:setDelegate()
    self._listView:addTo(addressLayer)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable2), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex2), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView2), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()
    tmpcontent:removeFromParent()
 
    end)))

  

  self:runAction(cc.Sequence:create(cc.DelayTime:create(2),cc.CallFunc:create(function ()
    
      self._scene:dismissPopWait()
    
    if #self.m_tipsTable == 0 then
      
        showToast(self._scene, "对局无异常，请放心游戏！", 2)  
        addressLayer:removeFromParent()
      

      self.locationBtn:removeChildByTag(163)

      local ani1 = ExternalFun.loadCSB("locationAnim.csb", self.locationBtn)
      ani1:setTag(163)
      ani1:setPosition(0,0)

      local aniAction = ExternalFun.loadTimeLine("locationAnim.csb")
      aniAction:gotoFrameAndPlay(0,25,true)
      ani1:runAction(aniAction)
    else
      
      self.locationBtn:removeChildByTag(163)
      
      local ani1 = ExternalFun.loadCSB("locationAnim_red.csb", self.locationBtn)
      ani1:setTag(163)
      ani1:setPosition(0,0)

      local aniAction = ExternalFun.loadTimeLine("locationAnim_red.csb")
      aniAction:gotoFrameAndPlay(0,25,true)
      ani1:runAction(aniAction)
    end
    end)))
end

function GameViewLayer:checkAdress()

  for i=1 , 9 do
    local useritem = self._scene.m_tabUserItem[i]
    if useritem then
        local seat = self._bottomNode:getChildByName(string.format("seat_%d", self._scene:getUserIndex(i-1)))
        local headbg = seat:getChildByName("head_bg")
        local head = headbg:getChildByTag(1)
        if head and head:isVisible()  then--
            print("请求定位",i,self._scene:getUserIndex(i-1))
            head:queryUserLociton(useritem.dwUserID)
        end
    end
  end

  local isAbnormal = false

  self:runAction(cc.Sequence:create(cc.DelayTime:create(0.6),cc.CallFunc:create(function ()

    for i=1,9 do
      local seat1 = self._bottomNode:getChildByName(string.format("seat_%d", i))
      local headbg1 = seat1:getChildByName("head_bg")
      local head1 = headbg1:getChildByTag(1)
      if head1 and head1:isVisible() then
        local userInfo1 = head1.m_userInfo
        local la1  = 0
        local lo1  = 0 
        if userInfo1 then
          la1  = userInfo1.dLatitude
          lo1 = userInfo1.dLongitude
        end

        
        for j=i+1,9 do
          local la2 = 0
          local lo2 = 0
          local userInfo2
          local seat2 = self._bottomNode:getChildByName(string.format("seat_%d", j))
          local headbg2 = seat2:getChildByName("head_bg")
          local head2 = headbg2:getChildByTag(1)
          if head2 and head2:isVisible() then
            userInfo2 = head2.m_userInfo
            if userInfo2 then
              la2  = userInfo2.dLatitude
              lo2 = userInfo2.dLongitude
              
            end
          end
          la1 = 100
          lo1 = 100
          la2 = 100
          lo2 = 100
          if la1 ~= 0 and lo1 ~= 0 and la2 ~= 0 and lo2 ~= 0  then

            local dst = self:algorithm(lo1 , la1 , lo2 , la2)
            if dst <=10 and userInfo1 and userInfo2 then
                isAbnormal = true
            end
          end
        end
      end
    end
    self.locationBtn:removeChildByTag(163)
    if isAbnormal then
      local ani1 = ExternalFun.loadCSB("locationAnim_red.csb", self.locationBtn)
      ani1:setTag(163)
      ani1:setPosition(0,0)

      local aniAction = ExternalFun.loadTimeLine("locationAnim_red.csb")
      aniAction:gotoFrameAndPlay(0,25,true)
      ani1:runAction(aniAction)
    else
      local ani1 = ExternalFun.loadCSB("locationAnim.csb", self.locationBtn)
      ani1:setTag(163)
      ani1:setPosition(0,0)

      local aniAction = ExternalFun.loadTimeLine("locationAnim.csb")
      aniAction:gotoFrameAndPlay(0,25,true)
      ani1:runAction(aniAction)
    end
    
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

    local csb = cc.CSLoader:createNode("game_res/locationCell.csb")
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

function GameViewLayer:removeUserFromView( wChairId )
    local viewid = self._scene:getUserIndex(wChairId-1)
    local seat = self._bottomNode:getChildByName(string.format("seat_%d", viewid))
    local headbg = seat:getChildByName("head_bg")
    headbg:removeChildByTag(1)
    seat:setVisible(false)
    self:onUserVoiceEnded(viewid)
    self._bottomNode:getChildByName("seat_"..viewid.."_0"):setVisible(true)
    self._rootNode:getChildByName("ready_"..viewid):setVisible(false)
    --self:showSitStatus(0,self._scene:getUserIndex(wChairId-1))  
    self:playSound("exit.wav")

    for k,v in pairs(self._cardList[viewid]) do
      if v then
        v:removeFromParent()
        v=nil
      end
    end
    self._cardList[viewid] ={}
end

function GameViewLayer:deleteUserHg(viewid)
      self._bottomNode:getChildByName(string.format("seat_%d", viewid)):setVisible(false)
      self._bottomNode:getChildByName("seat_"..viewid.."_0"):setVisible(true)
      self._rootNode:getChildByName("ready_"..viewid):setVisible(false)

      self:onUserVoiceEnded(viewid)
end

--D玩家
function GameViewLayer:showDUser(wChairId)
  for i=1,cmd.GAME_PLAYER do
    local icon = self._rootNode:getChildByName("icon_banker_"..i)
    icon:setVisible(false)
  end
  local viewid = self._scene:getUserIndex(wChairId)
  local icon = self._rootNode:getChildByName("icon_banker_"..viewid)
  icon:setVisible(true)
end

--私有房规则
function GameViewLayer:showPriRule( roomid,cellscore )
  --房号
  local roominfo = self._bottomNode:getChildByName("room_pri_info")
  self.m_atlasRoomID = roominfo:getChildByName("txt_2")
  self.m_atlasRoomID:setString(string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))

  --盲注
  local roomcell = roominfo:getChildByName("txt_3")
  roomcell:setString(""..string.format("%d",cellscore/2).."/"..cellscore)
end

--刷新用户信息  
function GameViewLayer:updateUserInfo(viewid,score,extrastr)
  local seat = self._bottomNode:getChildByName(string.format("seat_%d", viewid))
  local headbg = seat:getChildByName("head_bg")
  local txt_operate = seat:getChildByName("txt_operate")
  local take_score = seat:getChildByName("Text_2")

  --昵称
  if extrastr then
    txt_operate:setString(extrastr)
  end

  --携带筹码
  if take_score then
    take_score:setString(""..ExternalFun.formatScoreText(score))
  end
end

--显示加注额
function GameViewLayer:showAddChipScore(viewid,visible)
  local add_chipbg = self._rootNode:getChildByName("add_chipbg_"..viewid)
  add_chipbg:setVisible(visible)
end

--设置玩家下注筹码
function GameViewLayer:setChipScore(viewid,score,isAni)
	if score <= 0 then
            self:showAddChipScore(viewid,false)
		return
	end

    self:playSound("bet_x.wav")

    local add_chipbg = self._rootNode:getChildByName("add_chipbg_"..viewid)
    local txt_score = add_chipbg:getChildByName("score")

    if isAni then

      local viewImg = self._bottomNode:getChildByName(string.format("seat_%d", viewid))

      local pointFrom  = cc.p(viewImg:getPosition())
      local pointTo = cc.p(add_chipbg:getPosition())

      local aniNode = self._rootNode:getChildByName("Node_1")
      for i=1,2 do
            local spriteGold = cc.Sprite:create("game_res/jetton_2.png"):move(pointFrom.x,pointFrom.y-64):addTo(aniNode)
            spriteGold:runAction(cc.Sequence:create(cc.DelayTime:create(0.05*(i-1)),cc.MoveTo:create(0.2,cc.p(pointTo.x - 38.6,pointTo.y -2)),cc.DelayTime:create(i==1 and 0.2 or 0.1),cc.CallFunc:create(function ( ... )
              spriteGold:removeFromParent()
              if i==2 then
                  add_chipbg:setVisible(true)
                  txt_score:setString(""..score)

                  local len = string.len(""..score)
                  if len>4 then
                    local bgSize = add_chipbg:getContentSize()
                    add_chipbg:setContentSize(105 + (15*(len-4)) , 28)
                  else
                      add_chipbg:setContentSize(105,28)
                  end
              end
            end)))

      end
    else
      txt_score:setString(""..score)
      local len = string.len(""..score)
      if len>4 then
        local bgSize = add_chipbg:getContentSize()
        add_chipbg:setContentSize(105 + (15*(len-4)) , bgSize.height)
      else
        add_chipbg:setContentSize(105,28)
      end
    end
end

--底池
function GameViewLayer:showDeskChipScore(visible)
  local deskChipBg = self._rootNode:getChildByName("desk_chip_1")
  deskChipBg:setVisible(visible)
end

--更新底池
function GameViewLayer:updateDeskChip(lchip)
  self.m_lDeskChip = lchip
  local deskChipBg = self._rootNode:getChildByName("desk_chip_1")
  local txt_desk_score = deskChipBg:getChildByName("score")
  local len = string.len(""..lchip)
  if len>6 then
    local bgSize = deskChipBg:getContentSize()
    deskChipBg:setContentSize(160 + (15*(len-6)) , bgSize.height)
  else
    deskChipBg:setContentSize(160,29)
  end
  txt_desk_score:setString(""..lchip) 
  self:updateChipImg(1,lchip)
end

function GameViewLayer:showGoldToTable(viewid)

      local viewImg = self._rootNode:getChildByName("add_chipbg_"..viewid)
      local tableImg = self._rootNode:getChildByName("desk_chip_1")

      local pointFrom  = cc.p(viewImg:getPosition())
      local pointTo = cc.p(tableImg:getPosition())


      local aniNode = self._rootNode:getChildByName("Node_1")
      for i=1,2 do
            local spriteGold = cc.Sprite:create("game_res/jetton_2.png"):move(pointFrom.x -38.6,pointFrom.y-2):addTo(aniNode)
            spriteGold:runAction(cc.Sequence:create(cc.DelayTime:create(0.05*(i-1)),cc.MoveTo:create(0.2,cc.p(pointTo.x - 70,pointTo.y -2)),cc.DelayTime:create(i==1 and 0.2 or 0.1),cc.CallFunc:create(function ( ... )
              spriteGold:removeFromParent()
             
            end)))

      end


end

function GameViewLayer:showTableToPool(poolIndex,score)

      local tableFrom = self._rootNode:getChildByName("desk_chip_1")
      local tableTo = self._rootNode:getChildByName("desk_chip_"..poolIndex)

      local  pointFrom = cc.p(tableFrom:getPosition())
      local  pointTo = cc.p(tableTo:getPosition())


      local aniNode = self._rootNode:getChildByName("Node_1")
      for i=1, 5 do
            local spriteGold = cc.Sprite:create("game_res/chip"..i..".png"):move(pointFrom.x -70,pointFrom.y-2):addTo(aniNode)
            spriteGold:runAction(cc.Sequence:create(cc.DelayTime:create(0.06*(i-1)),cc.MoveTo:create(0.5,cc.p(pointTo.x-41,pointTo.y-1)),cc.DelayTime:create(0.5),cc.CallFunc:create(function ( ... )
              spriteGold:removeFromParent()
             
            end)))
      end

      tableTo:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ( ... )
            
            tableTo:setVisible(true)
            tableTo:getChildByName("score"):setString(""..score)

            local len = string.len(""..score)
            if len>5 then
              local bgSize = tableTo:getContentSize()
              tableTo:setContentSize(110 + (15*(len-5)) , bgSize.height)
            else
              tableTo:setContentSize(110,28)
            end
            self:updateChipImg(poolIndex,score)
      end)))

end


function GameViewLayer:updateChipImg(index,score)

    local chipPool = self._rootNode:getChildByName("desk_chip_"..index)
    local chipImg = chipPool:getChildByName("jetton_icon")
    local chipNum = {10000,5000,1000,500,100,50,10,5,1}
    local lastNum = score

    chipImg:removeAllChildren()

    if lastNum == 0 then
        chipImg:setTexture("game_res/chip4.png")
        return
    end

    local imgNum = 0
    for i=1,9 do
        if lastNum <=0 then
          break
        end
        local curNum = math.floor(lastNum/chipNum[i])
        if curNum >0 then
          for j=1,curNum do
              if imgNum >= 5 then
                 break
              end
              if imgNum == 0 then
                  chipImg:setTexture("game_res/chip"..(10-i)..".png")
              else
                  local chiptemp = cc.Sprite:create("game_res/chip"..(10-i)..".png"):move(14,14 +3*imgNum):addTo(chipImg)
              end
              imgNum = imgNum+1
          end
        end
      if imgNum >= 5 then
                 break
      end
      lastNum = lastNum%chipNum[i]
    end
end

function GameViewLayer:showGoldToPlayer(index,viewid)
      local viewImg = self._bottomNode:getChildByName(string.format("seat_%d", viewid))
      local tableImg = self._rootNode:getChildByName("desk_chip_"..index)

      local  pointTo = cc.p(viewImg:getPosition())
      local  pointFrom = cc.p(tableImg:getPosition())

      if index == 1 then
          pointFrom = cc.p(pointFrom.x -70,pointFrom.y-2)
      else
          pointFrom = cc.p(pointFrom.x -41,pointFrom.y-1)
      end

      local aniNode = self._rootNode:getChildByName("Node_1")
      for i=1, 5 do
            local spriteGold = cc.Sprite:create("game_res/chip"..i..".png"):move(pointFrom):addTo(aniNode)
            spriteGold:runAction(cc.Sequence:create(cc.DelayTime:create(0.06*(i-1)),cc.MoveTo:create(0.8,cc.p(pointTo.x,pointTo.y)),cc.DelayTime:create(0.8),cc.CallFunc:create(function ( ... )
              spriteGold:removeFromParent()
            end)))
      end
end

--放弃状态 头像变灰
function GameViewLayer:showGiveUpStatus(viewid)

  local allCard = self._bottomNode:getChildByName("Panel_poker_"..viewid):getChildren()

  if viewid ==  cmd.MY_VIEW_INDEX then
      for k,v in pairs(allCard) do
          if v and nil ~= convertToGraySprite then
              if v.showCardBack and v.m_spBack then
                  v:showCardBack(true)
                  convertToGraySprite(v.m_spBack)
              end
              
          end
      end
      self._bottomNode:getChildByName("btn_ctrl_layout"):setVisible(false)
  else
     for i=1,2 do
          local card = self._bottomNode:getChildByName("_card_view_"..viewid..i)
          if card and card.m_spBack then
                  card:showCardBack(true)
                  convertToGraySprite(card.m_spBack)
          end
     end
  end

  

   -- local seat = self._bottomNode:getChildByName(string.format("seat_%d", viewid))
   -- local headbg = seat:getChildByName("head_bg")
   -- local head = headbg:getChildByTag(1)
   -- if nil ~= convertToGraySprite then
   --    -- 灰度图
   --    if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
   --      convertToGraySprite(head.m_head.m_spRender)
   --    end
   --  end
end

function GameViewLayer:showLeftSecond()

  if not self._priView.m_priTimeTxt then
    return 
  end

  local hou =  string.format("%02d:",math.floor(self._scene.m_cbLeftSecond/3600))
  local min =  string.format("%02d:",math.floor(math.mod(self._scene.m_cbLeftSecond,3600)/60))
  local sec =  string.format("%02d",math.floor(math.mod(self._scene.m_cbLeftSecond,60)))
  print("11111")
  local complexStr = hou..min..sec
  self._priView.m_priTimeTxt:setString(complexStr)

end


function GameViewLayer:updatePriSecond(bStart)

  -- if GlobalUserItem.bPrivateGoldRoom then
  --   self._priView.m_priTimeTxt:setString("∞")
  -- end
  if self._scene.m_cbLeftSecond == self._scene.m_cbPriTime*60  then
    return
  end

  if self._scene.m_cbLeftSecond < 0 then
    return
  end

  local function update(dt)
    if self._scene.m_cbLeftSecond-1 >= 0 then
       self._scene.m_cbLeftSecond = self._scene.m_cbLeftSecond - 1
    else
       self._scene.m_cbLeftSecond = 0
       self:deletePriSecond()
    end

    self:showLeftSecond()
  end

  local scheduler = cc.Director:getInstance():getScheduler()

  if nil == self.m_secondCountSchedule then
    self.m_secondCountSchedule = scheduler:scheduleScriptFunc(update, 1.0, false)
  end
end

function GameViewLayer:deletePriSecond()
  --房卡倒计时定时器
  if nil ~= self.m_secondCountSchedule then 
      local scheduler = cc.Director:getInstance():getScheduler()
      scheduler:unscheduleScriptEntry(self.m_secondCountSchedule)
      self.m_secondCountSchedule = nil
  end
end

function GameViewLayer:addScoreView()
  self.m_lCurSlider = (self._scene.m_lAddLessScore < self._scene.m_lTurnMaxScore) and self._scene.m_lAddLessScore or self._scene.m_lTurnMaxScore
  local addNode = cc.CSLoader:createNode("game_res/add_node.csb");
  addNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  addNode:setTag(TAG.tag_addChip)
  self:addChild(addNode,TOP_ZOrder)

  --滑动额度
  local txt_score = addNode:getChildByName("txt_dynamic")

  --遮罩
  local mask = addNode:getChildByName("Panel_bg")
  mask:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
          addNode:removeFromParent()
          local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")
          btnLayout:setVisible(true)
      end
  end)

  --梭哈
  local allIn_btn = addNode:getChildByName("btn_allIn")
  allIn_btn:setTag(TAG.tag_allIn)
  allIn_btn:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
          addNode:removeFromParent()
          self:onButtonClickedEvent(TAG.tag_allIn, sender)
      end
  end)

  --加注
  local add_btn = addNode:getChildByName("btn_add")
  add_btn:setTag(TAG.tag_add)
  add_btn:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
          addNode:removeFromParent()
          self:onButtonClickedEvent(TAG.tag_add, sender)
      end
  end)

  --区间按钮
  for i=1,5 do
    local btn = addNode:getChildByName("btn_jetton_"..i)
    self._scene.m_lAddScoreCount = self._scene.m_lAddScoreCount or 0
    local addNum = math.min(self._scene.m_lTurnMaxScore,math.floor((self._scene.m_lTurnMaxScore - self._scene.m_lAddLessScore)/5*i)+ self._scene.m_lAddLessScore)
    
    if addNum <= 0 then
      addNum = self._scene.m_lTurnMaxScore
    end
    print("self._scene.m_lTurnMaxScore",self._scene.m_lTurnMaxScore)
    print("self._scene.m_lAddLessScore",self._scene.m_lAddLessScore)
    print("self._scene.m_lAddScoreCount",self._scene.m_lAddScoreCount)
    print("addNum",addNum)

    btn:setTitleText(""..addNum)
    btn:setTag(TAG.tag_add)
    btn:addTouchEventListener(function ( sender,eventType )
      if eventType == ccui.TouchEventType.ended then
         addNode:removeFromParent()
         self.m_lCurSlider = addNum
         self:onButtonClickedEvent(TAG.tag_add, sender)
      end
    end)
  end

  local ani1 = ExternalFun.loadCSB("ALLIN.csb", addNode)
       ani1:setTag(666)
       ani1:setPosition(0,0)

       local aniAction = ExternalFun.loadTimeLine("ALLIN.csb")
       aniAction:gotoFrameAndPlay(0,29, true)
       ani1:runAction(aniAction)
  local lastPer = 5
  --滑动条
  local slider = addNode:getChildByName("Slider_1")
  slider:setPercent(5)
  slider:addEventListener(function( ref,eventType )
      local value = ref:getPercent()
      if value <= 5 then
          ref:setPercent(5)
          self.m_lCurSlider = self._scene.m_lAddLessScore
          txt_score:setString(""..self.m_lCurSlider)
      end

      if value >= 95 then

        if lastPer ~= 95 then
          self:playSound("allin.wav")
        end

        ref:setPercent(95)
        self.m_lCurSlider = self._scene.m_lTurnMaxScore
        txt_score:setString(""..self.m_lCurSlider)

        local aniAction = ExternalFun.loadTimeLine("ALLIN.csb")
        aniAction:gotoFrameAndPlay(30,75, true)
        ani1:stopAllActions()
        ani1:runAction(aniAction)
        lastPer = 95


      end

      if value > 5 and value < 95 then
         self.m_lCurSlider = math.min(self._scene.m_lAddLessScore + math.floor((value-5)/100*self._scene.m_lTurnMaxScore),self._scene.m_lTurnMaxScore)
         txt_score:setString(""..self.m_lCurSlider)

         if lastPer == 95 then
            local aniAction = ExternalFun.loadTimeLine("ALLIN.csb")
           aniAction:gotoFrameAndPlay(0,29, true)
           ani1:stopAllActions()
           ani1:runAction(aniAction)
           lastPer = value
         end

      end
  end)

  if self._scene.m_lAddLessScore > self._scene.m_lTurnMaxScore then
    slider:setPercent(95)
    slider:setEnabled(false)
  end

  txt_score:setString(""..self.m_lCurSlider)
end

--游戏中显示 加注 跟注 弃牌 梭哈 
function GameViewLayer:showOperateFlag(viewId, operateCode)
  --1加注 2跟注 3弃牌 4梭哈 5过牌
   if operateCode == 65535 then
     return
   end
   print(viewId, operateCode)
   local str = {"加注","跟注","弃牌","All In","让牌"}
   local color = {cc.c3b(255, 198, 86),cc.c3b(117, 211, 129),cc.c3b(159, 159, 159),cc.c3b(117, 211, 129),cc.c3b(117, 211, 129)}

   local seat = self._bottomNode:getChildByName(string.format("seat_%d", viewId))
   seat:removeChildByTag(666)
   local text_nick = seat:getChildByName("_clipNick")
   local txt_operate = seat:getChildByName("txt_operate")
   if text_nick then
        text_nick:setVisible(false)
   end
   print("11111111",txt_operate)
   txt_operate:setColor(color[operateCode])
   txt_operate:setVisible(true)
   txt_operate:setString(str[operateCode])
   if operateCode == 4 then
      txt_operate:setVisible(false)
      local aniNode = self._rootNode:getChildByName("Node_1")
      local ani1 = ExternalFun.loadCSB("touxiang.csb", aniNode)
      ani1:setTag(1000+viewId)
      ani1:setPosition(seat:getPosition())
      local gunAction = ExternalFun.loadTimeLine("touxiang.csb")
      gunAction:gotoFrameAndPlay(0, true)
      ani1:runAction(gunAction)
      --ani1:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(1.66),cc.CallFunc:create(function ( ... )
          --ani1:runAction(gunAction)
      --end)))
      local allIn = cc.Sprite:create("game_res/allin.png"):move(0,65):addTo(ani1)
   end

end

function GameViewLayer:cleanAllInImg()

      local child = self._rootNode:getChildByName("Node_1"):getChildren()
      for k,v in pairs(child) do
          if v and v:getTag() > 1000 then
              v:removeFromParent()
          end
      end

end


--聊天界面
function GameViewLayer:showChat()

        if not self._chatLayer then
      --聊天，调用本地资源，不用通用
      local tabconfig = {}
      tabconfig.csbfile = RES_PATH.."chat/GameChatLayer.csb"
      --图片要加入缓存
      local sprbg = cc.Sprite:create(RES_PATH.."chat/chat_cell.png")
      if sprbg then
        cc.SpriteFrameCache:getInstance():addSpriteFrame(sprbg:getSpriteFrame(), "chat_cell.png")
        tabconfig.szItemFrameName = "chat_cell.png"
      end
    
      self._chatLayer = GameChatLayer:create(self._scene._gameFrame, tabconfig):addTo(self, 51)  --聊天框
      
      self._chatLayer:setTag(TAG.tag_chatview)
    end
        
    self._chatLayer:showGameChat(true)

end

--[[
  @cheif:updateControl  更新操作按钮状态
  @param:是否预操作+(按钮索引+状态),such as:{bPre = true ,datas={{id=1,status=true},{id=2,status=false}},索引为1的按钮可见,状态enable,索引为1的按钮可见,状态unable
  索引: bPre:true,预操作状态,1(一律跟注) 2(过牌) 3(弃牌/过牌)
        bPre:fasle,自己操作状态，1(加注) 2(跟注) 3(弃牌) 4(过牌) 5(梭哈)
  ]]
function GameViewLayer:updateControl(status,noAction)
      --隐藏不显示按钮
      dump(status , "status status")
    local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")

    if self._scene.m_cbPlayStatus[self._scene._wchair+1] == 0  or not status or self._scene.m_bLookOn then 
      btnLayout:setVisible(false)
      return
    end
    btnLayout:stopAllActions()
    btnLayout:setPosition(0, -375)

    local delayTime = 0.1
    if  self.m_bPreOperate and not status.bPre and (self.m_nPreOperateCode ~= 65535) then 
        delayTime = 0.6
    end
    local actionList = {}
    table.insert(actionList,cc.DelayTime:create(delayTime))

    local action1 = cc.MoveBy:create(0.1,cc.p(0,-110))
    local action2 = cc.CallFunc:create(function ( ... )

           btnLayout:setVisible(true)

           for i=1,6 do
              local btn = btnLayout:getChildByName("btn_"..i)
              btn:setVisible(false)
              btn:setColor(cc.c3b(255, 255, 255))
              local selectedIcon =  btn:getChildByName("selected_icon")
              if selectedIcon then
                selectedIcon:setVisible(false)
              end
           end

            if not status or (#status.datas == 0) then

              return
            end


            if self.m_bPreOperate and not status.bPre and (self.m_nPreOperateCode ~= 65535) then 
               local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")
              --(1跟任何注 3弃牌 2过牌)
               if self.m_nPreOperateCode == 1  then 
                 local bRet = false
                 for i=1,#status.datas do
                   local record = status.datas[i]
                   if record.id == 2 or record.id == 5 then
                     bRet = true
                     break
                   end
                 end
                 if bRet then
                   self.m_bPreOperate = false
                   self.m_nPreOperateCode = 65535
                   self.m_nOperateTag = 65535
                   self:onEventAddChip(self._scene.m_lTurnLessScore)
                   btnLayout:setVisible(false)
                   btnLayout:stopAllActions()
                   return 
                 end    
               elseif self.m_nPreOperateCode == 2 or self.m_nPreOperateCode == 3 then
                 local bRet = false
                 for i=1,#status.datas do
                   local record = status.datas[i]
                   if record.id == 4 then
                     bRet = true
                     break
                   end
                 end
                   if bRet then
                     self.m_bPreOperate = false
                     self.m_nPreOperateCode = 65535
                     self.m_nOperateTag = 65535
                     self:onEventAddChip(0)
                     btnLayout:setVisible(false)
                     btnLayout:stopAllActions()
                     --btnLayout:runAction(cc.MoveBy:create(0.3,cc.p(0,-110)))
                     return 
                   elseif self.m_nPreOperateCode == 3 then

                      self.m_bPreOperate = false
                      self.m_nPreOperateCode = 65535
                      self.m_nOperateTag = 65535
                      self:onEventGiveUp()
                      btnLayout:setVisible(false)
                      btnLayout:stopAllActions()
                      --btnLayout:runAction(cc.MoveBy:create(0.3,cc.p(0,-110)))
                      return
                   end
               end
           end

           if status.bPre == true then
            print("status.bPre == true",self.m_nPreOperateCode)
              local str = {"跟任何注","让牌","让或弃"}
              local touchFunc = function ( sender ,TouchEventType )
                if TouchEventType == ccui.TouchEventType.ended then
                   self:onButtonClickedEvent(sender:getTag(), sender)
                end
              end

              local pics = {"game_res/btn_pre1.png","game_res/btn_pre1.png","game_res/btn_pre1.png"}
              for i=1,3 do
                local btn = btnLayout:getChildByName("btn_"..i)
                btn:setVisible(true)
                btn:loadTextures(pics[i],pics[i],pics[i])
                btn:setTag(TAG.tag_pre_btn+i-1)
                btn.operateCode = i
                btn:addTouchEventListener(touchFunc)
                btn:setEnabled(status.datas[i].status)

                local selectedIcon = btn:getChildByName("selected_icon")
                selectedIcon:setVisible(false)
                if btn.operateCode == self.m_nPreOperateCode then
                    selectedIcon:setVisible(true)
                end


                local txt = btn:getChildByName("Text_1")
                txt:setString(str[i])
                txt:setPositionX(146)
                if i==1 then
                  txt:setPositionX(150)              
                end
              end
            else
              self.m_bPreOperate = false
              self.m_nPreOperateCode = 65535
              self.m_nOperateTag = 65535

              local nid = {}
              local bStatus = {}
              for i=1,#status.datas do
                local record = status.datas[i]
                local id = record.id
                local bEnable = record.status
                table.insert(nid, id)
                table.insert(bStatus, bEnable)
              end

              self:playSound("tip.wav")
              --1(加注) 2(跟注) 3(弃牌) 4(过牌) 5(梭哈)
              local str = {"加注","跟注","弃牌","让牌","All In"}

              local touchFunc = function ( sender ,TouchEventType )
                if TouchEventType == ccui.TouchEventType.ended then
                   self:onButtonClickedEvent(sender:getTag(), sender)
                end
              end

              local res = {"game_res/btn_addNum.png","game_res/btn_add_1.png","game_res/btn_add_2.png"}
              for i=1,3 do
                local btn = btnLayout:getChildByName("btn_"..i)
                btn:loadTextures(res[i],res[i],res[i])
                btn:setVisible(true)
                btn:setEnabled(bStatus[i])
                if not bStatus[i] then
                  btn:setColor(cc.c3b(127, 127, 127))
                end
                btn:setTag(TAG.tag_opreate_btn+nid[i])
                btn:addTouchEventListener(touchFunc)

                local txt = btn:getChildByName("Text_1")
                txt:setPositionX(123)
                txt:setString(str[nid[i]])
                if nid[i] == 2 and self._scene.m_lTurnLessScore then
                  txt:setString(str[nid[i]].."("..self._scene.m_lTurnLessScore ..")")
                  if self._scene.m_lTurnLessScore<=0 then
                    txt:setString("让牌")
                  end
                end
                
              end

              -- 3倍大盲、4倍大盲、底池 or 1/2底池、2/3底池、底池
              local lTableScore = 0
              for i=1,cmd.GAME_PLAYER do
                lTableScore = lTableScore + self._scene.m_lTableScore[i]
              end

              if self._scene.m_cbBalanceCount > 0 then
                  local lLimit = {math.floor(self.m_lDeskChip/2),math.floor(self.m_lDeskChip*2/3),self.m_lDeskChip}
                  local str = {"1/2底池","2/3底池","底池"}
                  for i=4,6 do
                     local btn = btnLayout:getChildByName("btn_"..i)
                     btn:setVisible(true)
                     btn:setEnabled(true)
                     btn.lChip = lLimit[i-3]
                     btn:setColor(cc.c3b(255, 255, 255))
                     btn:setTag(TAG.tag_opreate_btn+10+i)
                     btn:getChildByName("Text_8"):setString(str[i-3])
                     --btn:setTitleText(str[i-3])
                     btn:addTouchEventListener(touchFunc)

                     if lLimit[i-3] > self._scene.m_lTurnMaxScore or lLimit[i-3] < self._scene.m_lTurnLessScore then 
                        btn:setEnabled(false)
                        btn:setColor(cc.c3b(127, 127, 127))
                     end
                  end 
                
              else
                 local lLimit = {3*self._scene.m_lCellScore,4*self._scene.m_lCellScore,self.m_lDeskChip}
                 local str = {"3倍大盲","4倍大盲","底池"}
                 for i=4,6 do
                   local btn = btnLayout:getChildByName("btn_"..i)
                   btn:setVisible(true)
                   btn:setEnabled(true)
                   btn.lChip = lLimit[i-3]
                   btn:setColor(cc.c3b(255, 255, 255))
                   btn:setTag(TAG.tag_opreate_btn+10+i)
                   btn:getChildByName("Text_8"):setString(str[i-3])
                   --btn:setTitleText(str[i-3])
                   btn:addTouchEventListener(touchFunc)
                   if lLimit[i-3] > self._scene.m_lTurnMaxScore or lLimit[i-3] < self._scene.m_lTurnLessScore or i==6 then 
                      btn:setEnabled(false)
                      btn:setColor(cc.c3b(127, 127, 127))
                   end
                end
              end
           end

    end)
    
    local action3 = cc.MoveBy:create(0.1,cc.p(0,110))

    if noAction then
        table.insert(actionList,action2)
    else
        table.insert(actionList,action1)
        table.insert(actionList,action2)
        table.insert(actionList,action3)
    end

    btnLayout:runAction(cc.Sequence:create(actionList))

end

--使用互动道具
function GameViewLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
  --local viewid = self._scene:getUserIndex(useritem.wChairID)
  --GameLayer:getSitUserInfoByChairID(chairId)
        local beginPoint = nil
        local endPoint = nil
        local zOrder = 1
        local posTable = 
          {
            cc.p(667,375-195),
            cc.p(667+326,375-190),
            cc.p(667+494,375-25),
            cc.p(667+494,375+158),
            cc.p(667+205,375+270),
            cc.p(667-205,375+270),
            cc.p(667-494,375+158),
            cc.p(667-494,375-25),
            cc.p(667-326,375-190)
          }

        for i=1 , 9 do
          local userItem = self._scene:getSitUserInfoByChairID(i-1)
          if userItem then
            local view = self._scene:getUserIndex(i-1)
            local seat = self._bottomNode:getChildByName(string.format("seat_%d", view))
            if userItem.dwUserID ==  dwUserID then
              beginPoint = posTable[view]
            elseif userItem.dwUserID ==  dwRecvUserID then
              endPoint = posTable[view]
              zOrder = self._bottomNode:getZOrder() + 1
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

--发牌
function GameViewLayer:sendCard(data,bAction,isStart)
  
  local delayTime = 0.1

  if isStart then
      delayTime = 1

      local ani1 = ExternalFun.loadCSB("fapai.csb", self)
      ani1:setPosition(0,0)
      local gunAction = ExternalFun.loadTimeLine("fapai.csb")
      gunAction:gotoFrameAndPlay(0, false)
      ani1:runAction(gunAction)
      ani1:runAction(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function()
          ani1:removeFromParent()
        end)))

  end

  self:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ()
            --新一轮
            self.m_bPreOperate = false
            self.m_nPreOperateCode = 65535
            
            local node= self._bottomNode
            local heguanPanel = node:getChildByName("Panel_poker_he") --荷官位置
            local beginPos = cc.p(heguanPanel:getPositionX(),heguanPanel:getPositionY()) --发牌初始位置
            local actions = data
            local no_action_time_interval = 0.0
            if actions.round == 1 then
              local animations = {}
              for i=1,actions.perCount do
                for j=1,#actions.views do
                    local animate = cc.CallFunc:create(function()
                      local viewIndex = actions.views[j]
                      local pokerPanel = node:getChildByName("Panel_poker_"..viewIndex) 
                      local card = CardSprite:createCard()
                      card:setName("_card_view_"..viewIndex..i)
                      card:setPosition(beginPos)
                      card:setScale(0.3)
                      node:addChild(card)
                      self:playSound("sendCard.wav")

                      table.insert(self._cardList[viewIndex],card)

                      local anglelist = {-15,15}
                      local offsetY = {0,5}
                      local ftime = bAction and 0.2 or no_action_time_interval
                      card:runAction(cc.MoveTo:create(ftime, cc.p(pokerPanel:getPositionX()+i*10 , pokerPanel:getPositionY())))
                      card:runAction(cc.RotateBy:create(ftime, anglelist[i]))
                    end)
                    local fDelay = bAction and actions.interval or no_action_time_interval
                    local delayTime = cc.DelayTime:create(actions.interval)
                    table.insert(animations, animate)
                    table.insert(animations, delayTime)
                end
              end
              
              --显示自己手牌
              if self._scene.m_cbPlayStatus[self._scene._wchair+1] == 1  then 
                local animate = cc.CallFunc:create(function()
                    local cardList = self._cardList[cmd.MY_VIEW_INDEX]
                    for j=1,#cardList do
                      local card = cardList[j]
                      card:removeFromParent()
                    end
                    self._cardList[cmd.MY_VIEW_INDEX] = {}

                    local pokerPanel = self._bottomNode:getChildByName("Panel_poker_1") 
                    for i=1,cmd.MAX_COUNT do
                      local handCard = CardSprite:createCard(self._scene.m_cbHandCardData[self._scene._wchair+1][i])
                      handCard:setAnchorPoint(0.0,0.5)
                      handCard:setScale(0.6)
                      handCard:setPosition((i-1)*65,pokerPanel:getContentSize().height/2)
                      handCard:showCardBack(false)
                      pokerPanel:addChild(handCard)

                      table.insert(self._cardList[cmd.MY_VIEW_INDEX],handCard)
                    end
                end)
                table.insert(animations,animate)
              end
              
              if #animations >0 then
                self:runAction(cc.Sequence:create(animations))    
              end
           else
              local animations = {}
              local nCards = #self.m_centerCard
              local offsetX = 40
              
              for i = 1 ,actions.perCount do
                  local animate = cc.CallFunc:create(function()
                      local centerPanel = node:getChildByName("Panel_poker_desk") 
                      local card = CardSprite:createCard(self._scene.m_cbCenterCardData[i+nCards])
                      card:setPosition(beginPos)
                      if bAction then
                        card:showCardBack(true)
                      else
                        card:showCardBack(false)
                      end
                      card:setScale(0.3)
                      node:addChild(card)
                      self:playSound("sendCard.wav")

                      local moveTo = cc.MoveTo:create(0.2, cc.p(centerPanel:getPositionX()+offsetX+(i-1+nCards)*card:getContentSize().width , centerPanel:getPositionY()))
                      local scale = cc.ScaleTo:create(0.2,1.0) 
                      local spaw = cc.Spawn:create(moveTo,scale)

                      --带参回调
                      function callbackWithArgs(target)
                          local ret = function ()
                              if target and target.updateSprite then
                                target:showCardBack(false)
                                target:updateSprite()
                              end
                          end
                          return ret
                      end

                     --翻转动画
                      local call = cc.CallFunc:create(callbackWithArgs(card))
                      local sequence = cc.Sequence:create(cc.OrbitCamera:create(0.2, 1, 0, 0, 130, 0, 0),
                                                          call,
                                                          cc.OrbitCamera:create(0.016, 1, 0, 130, -130, 0, 0))


                      local complex = cc.Sequence:create(spaw,cc.DelayTime:create(0.5),sequence)
                      if bAction then
                        card:runAction(complex)
                      else
                        card:setPosition(cc.p(centerPanel:getPositionX()+offsetX+(i-1+nCards)*card:getContentSize().width , centerPanel:getPositionY()))
                        card:setScale(1.0)
                        --card:showCardBack(false)
                        --card:updateSprite()
                      end
                      table.insert(self.m_centerCard,card)
                end)
                    if actions.perCount == 5 and i >= 3 then
                        actions.interval = 1.2
                    elseif actions.perCount == 2 then
                        actions.interval = 1.2
                    end
                    local delayTime = cc.DelayTime:create(actions.interval)
                    table.insert(animations, animate)
                    table.insert(animations, delayTime)
              end
              
              if #animations >0  then 
                  self:runAction(cc.Sequence:create(animations))    
              end
            end
   end))) 
end

function GameViewLayer:reconnGiveUp(viewId)

  local r = {-15,15}
  for i=1 ,2 do
    local pokerPanel = self._bottomNode:getChildByName("Panel_poker_"..viewId) 
    local card = CardSprite:createCard()
    card:setName("_card_view_"..viewId..i)
    card:setPosition( cc.p(pokerPanel:getPositionX()+i*10 , pokerPanel:getPositionY()) )
    card:setScale(0.3)
    self._bottomNode:addChild(card)
    card:setRotation(r[i])

    card:showCardBack(true)
    convertToGraySprite(card.m_spBack)

    table.insert(self._cardList[viewId],card)
  end
                
end

--公牌数目
function GameViewLayer:getCenterCardCount()
  return #self.m_centerCard
end

--设置携带
function GameViewLayer:setTakeChip(flag, maxScore)
   local bAllow = false
   if self._scene.m_cbPlayStatus[self._scene._wchair+1] == 0 or 
      (self._scene.m_cbPlayStatus[self._scene._wchair+1] == 1 and self._scene.m_cbGameStatus == cmd.GS_FREE) then
      bAllow = true
   end

   if flag ==1  then
      showToast(self._scene, "携带不足，请买入!", 2)  
   end
print(maxScore , "maxScore")
   local userItem = self._scene:GetMeUserItem()
   --dump(userItem, "the userItem", 6)

   local enableScore = maxScore or userItem.lScore
   
   self.m_lUserTakeChip = math.min(self._scene.m_lMinBetScore, enableScore)
   local csbNode = cc.CSLoader:createNode("game_res/purchaseLayer.csb");
   csbNode:setLocalZOrder(50)
   csbNode:setTag(88)
   self:addChild(csbNode)

   local mask = csbNode:getChildByName("Panel_mask")
   mask:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
          csbNode:removeFromParent()
      end
    end)

   --用户库存
   local txt_userchip = csbNode:getChildByName("txt_user_chip")
   txt_userchip:setString(""..ExternalFun.formatScoreText(enableScore))

   --最小带入
   local txt_limit_low = csbNode:getChildByName("txt_limit_low")
   txt_limit_low:setString(""..ExternalFun.formatScoreText(self._scene.m_lMinBetScore))

   --最大带入
   local txt_limit_top = csbNode:getChildByName("txt_limit_top")
   txt_limit_top:setString(""..ExternalFun.formatScoreText(self._scene.m_lMaxBetScore))

   --当前带入
   local txt_cur_take = csbNode:getChildByName("txt_cur_chip")
   txt_cur_take:setString(""..ExternalFun.formatScoreText(self._scene.m_lMinBetScore))

   --确定按钮
   local sure_btn = csbNode:getChildByName("btn_sure")
   sure_btn:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
          self:playSound("button.wav")
          if self._scene.m_lTakeScore[self._scene._wchair + 1] >= self._scene.m_lMaxBetScore then
            showToast(self._scene, "你携带的筹码大于最大携带，无法买入!", 2)  
            csbNode:removeFromParent()
            return
          end

         if self.m_lUserTakeChip < self._scene.m_lMinBetScore then
            showToast(self._scene, "筹码不足!", 2)  
            if csbNode then
                csbNode:removeFromParent()
            end
            return
         end

         if self._scene.m_cbGameStatus == cmd.GS_FREE then
            showToast(self._scene, "买入成功！", 2)  
            self:playSound("buy_s.wav")
         else
            showToast(self._scene, "本局结束后，自动买入！", 2)  
         end

         self:onEventSetChip(self.m_lUserTakeChip)
         csbNode:removeFromParent()

         --self:setProgressTimer(self._scene.m_startTime,cmd.MY_VIEW_INDEX)
      end
   end)

   --滑动条
  local slider = csbNode:getChildByName("Slider_1")
  slider:setPercent(0)
  slider:addEventListener(function( ref,eventType )
     local value = ref:getPercent()
     local min = self._scene.m_lMinBetScore
     local max = self._scene.m_lMaxBetScore
     --print(min , max)
     self.m_lUserTakeChip = math.min(math.floor(min + (max-min)*value/100),max)
     print(self.m_lUserTakeChip , "111")
     self.m_lUserTakeChip = math.max(self.m_lUserTakeChip,min)
     print(self.m_lUserTakeChip, "222",enableScore)
     self.m_lUserTakeChip = math.min(self.m_lUserTakeChip, enableScore)
     print(self.m_lUserTakeChip, "333")
     txt_cur_take:setString(""..ExternalFun.formatScoreText(self.m_lUserTakeChip))
  end)

end

--设置progressTimer
function GameViewLayer:setProgressTimer(ftime,viewId)
print("ftime,viewIdftime,viewIdftime,viewId",ftime,viewId)
    if self._timer then
      self._timer:removeFromParent()
      self._timer = nil
    end
    
    self.isTip = false
    self.curTime = ftime
    self.timerViewId = viewId

    local seat  = self._bottomNode:getChildByName("seat_"..viewId)
  --创建进度条  
    local progress = cc.ProgressTimer:create(display.newSprite("game_res/game_clock.png"))
             :setReverseDirection(true)
             :move(seat:getContentSize().width/2,seat:getContentSize().height/2)
             :setPercentage(100)
             :addTo(seat)

    progress:runAction(cc.Sequence:create(cc.ProgressTo:create(ftime, 0),cc.CallFunc:create(function()
        self:logicTimeZero()
    end)))

    self._timer = progress
  
end



--设置界面
function GameViewLayer:showSet()
  self:removeChildByName("_setLayer_")

  local Mask = ccui.ImageView:create()
  Mask:setName("_setLayer_")
  Mask:setContentSize(cc.size(yl.WIDTH, yl.HEIGHT))
  Mask:setScale9Enabled(true)
  Mask:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  Mask:setTouchEnabled(true)
  self:addChild(Mask,50)

  Mask:addTouchEventListener(function(sender,eventType)
    if eventType == ccui.TouchEventType.ended then
        Mask:removeFromParent()
    end
  end)

 --加载CSB
  local setNode = cc.CSLoader:createNode("game_res/setNode.csb");
  setNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  Mask:addChild(setNode)

  -- 版本号
  -- local mgr = self._scene:getParentNode():getApp():getVersionMgr()
  -- local verstr = mgr:getResVersion(7) or "0"
  -- verstr = "游戏版本:" .. appdf.BASE_C_VERSION .. "." .. verstr
  -- local txt_ver = setNode:getChildByName("txt_version")
  -- txt_ver:setString(verstr)

  --音乐按钮
  local btn = setNode:getChildByName("btn_sound")
  if self._bSound  then
      btn:loadTextureNormal("game_res/set_btn2.png")
      btn:loadTexturePressed("game_res/set_btn2.png")
  else
      btn:loadTextureNormal("game_res/set_btn1.png")
      btn:loadTexturePressed("game_res/set_btn1.png")
  end

  btn:addTouchEventListener(function(sender,eventType)
    if eventType == ccui.TouchEventType.ended  then
        self._bSound = not self._bSound
        if self._bSound  then
          sender:loadTextureNormal("game_res/set_btn2.png")
          sender:loadTexturePressed("game_res/set_btn2.png")
        else
            sender:loadTextureNormal("game_res/set_btn1.png")
            sender:loadTexturePressed("game_res/set_btn1.png")
        end

        local music = not GlobalUserItem.bVoiceAble
       GlobalUserItem.setVoiceAble(music)
      if GlobalUserItem.bVoiceAble == true then
        ExternalFun.playBackgroudAudio("common/bg1.wav")
      end
 
    end
  end)

  btn = setNode:getChildByName("btn_music")
  if self._bMusic  then
      btn:loadTextureNormal("game_res/set_btn2.png")
      btn:loadTexturePressed("game_res/set_btn2.png")
  else
      btn:loadTextureNormal("game_res/set_btn1.png")
      btn:loadTexturePressed("game_res/set_btn1.png")
  end
  btn:addTouchEventListener(function(sender,eventType)
    if eventType == ccui.TouchEventType.ended  then
       self._bMusic = not self._bMusic
       if self._bMusic  then
          sender:loadTextureNormal("game_res/set_btn2.png")
          sender:loadTexturePressed("game_res/set_btn2.png")
       else
          sender:loadTextureNormal("game_res/set_btn1.png")
          sender:loadTexturePressed("game_res/set_btn1.png")
       end
        local effect = not GlobalUserItem.bSoundAble
        GlobalUserItem.setSoundAble(effect)

    end
  end)

    btn = setNode:getChildByName("Button_1")
    btn:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended  then

            Mask:removeFromParent()

        end
    end)



end


--显示准备
function GameViewLayer:showReadyBtn(visible)
  if self._scene.m_bLookOn then
    visible = false
  end
  self.m_readyBtn:setVisible(visible)
  if not (PriRoom and GlobalUserItem.bPrivateRoom) and visible then
      self.btn_change1:setPositionX(120)
  elseif not visible then
    self.btn_change1:setPositionX(0)
  end
end

--显示换桌
function GameViewLayer:showChangeBtn(visible)
  if visible then
    self.btn_change1:setVisible(visible)
    self.m_readyBtn:setPositionX(-120)

    self.btn_change2:setEnabled(true)
  else
    self.btn_change1:setVisible(visible)
    self.m_readyBtn:setPositionX(0)

    self.btn_change2:setEnabled(false)
  end
end

--结算
function GameViewLayer:showGameConclude(cmd_data)
  --dump(cmd_data, "游戏结束结构体", 6)
  --加注额
  for i=1,cmd.GAME_PLAYER do
    --桌面筹码
      self:showAddChipScore(i,false)
  end

    self:cleanAllInImg()

    self._rootNode:getChildByName("cardType"):setVisible(false)

      if cmd_data.cbTotalEnd ==1 then
    	     --显示牌型
    	     self:showCardType(cmd_data.cbLastCardType[1])
            --显示最大组合牌
            self:showComplexCard(cmd_data , false)
          
      end

	--显示输赢分数
	self:showGameDetail(clone(cmd_data))--cmd_data.lGameScore[1],cmd_data.lUserMaxScore[1],)


	
	
	
end

--组合扑克
function GameViewLayer:showComplexCard(cmd_data,bAllIn)
  --显示用户手牌
  --dump(self._scene.m_cbCenterCardData,"m_cbCenterCardData===============11111")
  local cbCards = {}
  cbCards = clone(cmd_data.cbCardData)
  for i=1,cmd.GAME_PLAYER do
    if self._scene.m_cbPlayStatus[i] == 1 and ( (bAllIn and self._scene.m_lTakeScore[i] == 0) or (not bAllIn and self._scene.m_lTakeScore[i] ~= 0) or not self._scene.m_allInConclude )  then 
        --if i-1 ~= self._scene:GetMeChairID() then 
           local viewid = self._scene:getUserIndex(i-1)
           for k,v in pairs(self._cardList[viewid]) do
              v:removeFromParent()
           end
           self._cardList[viewid] = {}

           --2张手牌
            local seat = self._bottomNode:getChildByName("seat_"..viewid) 
            for j=1,2 do
              local card = CardSprite:createCard(cmd_data.cbCardData[i][j])
              card:setAnchorPoint(cc.p(0,0))
              card:setPosition(cc.p((j-1)*33 , 29))
              card:setScale(0.71)
              seat:addChild(card)
              table.insert(self._cardList[viewid],card)

              if i-1 == self._scene._wchair then
                  for n=1,5 do
                      if cmd_data.cbCardData[i][j] == cmd_data.cbLastCenterCardData[self._scene._wchair+1][n] or cmd_data.cbLastCenterCardData[self._scene._wchair+1][n] == 0 then
                         break
                      elseif n==5 and self._scene.m_cbCenterCardData[5] ~= 0  then --如果中途弃牌，不显示阴影
                         card:showSelectEffect(true)
                      end
                  end
              end

            end
        --end
    end
  end

  --自己用到的中心扑克
  for i=1,5 do
      for j=1,5 do
          if self._scene.m_cbCenterCardData[i] == cmd_data.cbLastCenterCardData[self._scene._wchair+1][j] or cmd_data.cbLastCenterCardData[self._scene._wchair+1][j] == 0 then
             break
          elseif j==5 and self._scene.m_cbCenterCardData[5] ~= 0 and  self.m_centerCard[i] then --如果中途弃牌，不显示阴影
             self.m_centerCard[i]:showSelectEffect(true)
          end
      end
  end

end

function GameViewLayer:showCardType(lastType)
  dump(lastType, "last type =====", 6)
  dump(self._scene.m_cbPlayStatus , "self._scene.m_cbPlayStatus")
   for i=1,cmd.GAME_PLAYER do
     if self._scene.m_cbPlayStatus[i] == 1 then
       local viewId = self._scene:getUserIndex(i-1)
       print("viewId",viewId)
       local seat = self._bottomNode:getChildByName(string.format("seat_%d", viewId))
       local text_nick = seat:getChildByName("_clipNick")
       local txt_cardType = seat:getChildByName("txt_operate")
       seat:removeChildByTag(666)
       if text_nick then 
         text_nick:setVisible(false)
      end
       txt_cardType:setVisible(true)

       if lastType[i]>=cmd.CT_SINGLE and lastType[i]<=cmd.CT_KING_TONG_HUA_SHUN then
         txt_cardType:setString(cmd.CardType[lastType[i]])
         txt_cardType:setColor(cc.c3b(255, 255, 255))

       end

       if lastType[i] == 0 then
          if text_nick then 
              text_nick:setVisible(true)
          end
          txt_cardType:setVisible(false)
       end

       if lastType[i] >= cmd.CT_HU_LU and lastType[i]<=cmd.CT_KING_TONG_HUA_SHUN and i == (self._scene._wchair + 1) then

                

                local path = {"tspx_hl.csb","tspx_st.csb","tspx_ths.csb","tspx_hjths.csb"}
                local pathS = {"cardType_hl.wav","cardType_jg.wav","cardType_th.wav","cardType_hjths.wav"}

                local index  = lastType[i] - cmd.CT_HU_LU +1
                local ani1 = ExternalFun.loadCSB(path[index], self)
                ani1:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
                local aniAction = ExternalFun.loadTimeLine(path[index])
                aniAction:gotoFrameAndPlay(0, false)
                ani1:runAction(aniAction)
                ani1:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function()
                    ani1:removeFromParent()
                  end)))
                self:playSound(pathS[index])
       end

     end
   end
end

--展示我的牌型
function GameViewLayer:showMyCardType(cardType)
    local txt_cardType = self._rootNode:getChildByName("cardType"):getChildByName("txt_myCardType")
    
    if cardType >= cmd.CT_SINGLE and cardType <= cmd.CT_KING_TONG_HUA_SHUN then
      self._rootNode:getChildByName("cardType"):setVisible(true)
      txt_cardType:setString(cmd.CardType[cardType])
    end
end


--输赢
function GameViewLayer:showGameDetail(cmd_data)
  local cbWinnder = 65535
  local lWinner   = -1
  local lScore = cmd_data.lGameScore[1]

  for i=1,cmd.GAME_PLAYER do

    local pool = self._rootNode:getChildByName("desk_chip_"..i)
    self:updateChipImg(i,0)
    pool:getChildByName("score"):setString("0")
    if i==1 then
      pool:setContentSize(160,29)
    else
      pool:setContentSize(110,28)
    end
    pool:setVisible(false)

    if cmd_data.cbPoolWinnerCount[1][i] >0  then

        for j=1,cmd_data.cbPoolWinnerCount[1][i] do

            self:showGoldToPlayer(i,self._scene:getUserIndex(cmd_data.wPoolWinner[i][j]))
        end
      
    end


    if self._scene.m_cbPlayStatus[i] == 1 then 
        if lScore[i] > lWinner then
           lWinner = lScore[i]
           cbWinnder = self._scene:getUserIndex(i-1)

        end
        local viewId = self._scene:getUserIndex(i-1)

        if i-1 == self._scene._wchair then
            if lScore[i] >= 0 then
                self:playSound("win.wav")
                if cmd_data.cbLastCardType[1][i] < cmd.CT_HU_LU and lScore[i] ~= 0 then
                    self:runAction(cc.Sequence:create(cc.DelayTime:create(1.2),cc.CallFunc:create(function ()
                      self:showWinAni()
                    end)))
                end
            else
                self:playSound("lose.wav")
            end
        end

        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.4),cc.CallFunc:create(function ( ... )
                  self._scene.m_lTakeScore = clone(cmd_data.lUserMaxScore[1])
                  self:showPlayerScoreChange(lScore[i],viewId)
                  if self._scene.m_lTakeScore[i] >= 0 then
                    self:showUserInfo(self._scene.m_tabUserItem[i],self._scene.m_lTakeScore[i])
                  end
        end)))
    end
  end

  --self:updateDeskChip(0)
  if cbWinnder ~= 65535 then
     local seat = self._bottomNode:getChildByName(string.format("seat_%d", cbWinnder))
     local image = seat:getChildByName("Image_1")
     image:setVisible(true)
  end
end

function GameViewLayer:showWinAni()
        local animation =cc.Animation:create()
        local sprite  = cc.Sprite:create("winAni/1.png")
        sprite:addTo(self)
        sprite:setPosition(yl.WIDTH/2, yl.HEIGHT/2 + 60)
        for i=1,19 do  
                       
           animation:addSpriteFrameWithFile("winAni/"..i..".png")                                                   
        end  
          animation:setDelayPerUnit(0.1)          --设置两个帧播放时间                   
          animation:setLoops(1)    --动画执行后还原初始状态    

          local action =cc.Animate:create(animation)                                                         
          sprite:runAction(cc.Sequence:create(action,cc.CallFunc:create(function ()
            sprite:removeFromParent()
          end)
          ))  
end

function GameViewLayer:showPlayerScoreChange(score,viewId)

        local imgPath = score >= 0 and "game_res/icon_4.png" or "game_res/icon_2.png"

        local seat = self._bottomNode:getChildByName(string.format("seat_%d", viewId))
        local labAtNum = cc.LabelAtlas:_create(string.format("-" .. math.abs(score)), imgPath, 26, 36, string.byte("-"))   --数字
          :setAnchorPoint(cc.p(0.5, 0.5))
          :addTo(self._rootNode:getChildByName("Node_1"))
          :move(seat:getPosition())

          labAtNum:runAction(cc.Sequence:create(cc.MoveBy:create(1,cc.p(0,100)),cc.DelayTime:create(1.5),cc.RemoveSelf:create()))
end


function GameViewLayer:playSound(path,chair)
      if chair then
        local useritem = self._scene.m_tabUserItem[chair+1]
        if useritem then
          self._scene:PlaySound("sound_res/"..useritem.cbGender.."/"..path)
        end
      else
         self._scene:PlaySound("sound_res/common/"..path)
      end
end
--按钮事件
function GameViewLayer:onButtonClickedEvent(tag, ref)
  if tag == TAG.tag_readybtn then

    if self._scene.m_lTakeScore[self._scene._wchair+1] < self._scene.m_lMinBetScore  then
        self:onEventGetEnableScore()
                return
    end

     ref:setVisible(false)
     self._scene:SendUserReady()
     self:onResetView()
     if not (PriRoom and GlobalUserItem.bPrivateRoom) then
      self.btn_change1:setPositionX(0)
     end
     self:removeTimer()
     self:playSound("button.wav")
 
  elseif tag == TAG.tag_pre_btn or tag == TAG.tag_pre_btn+1 or  tag == TAG.tag_pre_btn+2  then --预操作
     local btn = ref

     if self.m_nOperateTag == 65535 then 
        self.m_nOperateTag = tag
        self.m_bPreOperate = true
     else
       if self.m_nOperateTag == tag then
          self.m_bPreOperate = not self.m_bPreOperate 
          if not self.m_bPreOperate then
              self.m_nOperateTag = 65535
          else
             self.m_nOperateTag = tag  
          end
       else
          self.m_bPreOperate = true
          self.m_nOperateTag = tag  
       end
     end
    
     print("self.m_nOperateTag ===========",self.m_nOperateTag)
     if self.m_bPreOperate then
        self.m_nPreOperateCode = btn.operateCode
     end
     print("self.m_nPreOperateCode======",self.m_nPreOperateCode)

     local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")
     for i=1,3 do
        local btn = btnLayout:getChildByName("btn_"..i)
        local selectedIcon = btn:getChildByName("selected_icon")
        selectedIcon:setVisible(false)
        if btn:getTag() == self.m_nOperateTag then
           selectedIcon:setVisible(true)
        end
     end
     self:playSound("button.wav")
  elseif tag - TAG.tag_opreate_btn == 1  then --1(加注) 2(跟注) 3(弃牌) 4(过牌) 5(梭哈)
      --弹出加注
      local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")
      btnLayout:setVisible(false)
      self:addScoreView()

      self:playSound("button.wav")
      print("加注")
  elseif tag == TAG.tag_add then
      self:onEventAddChip(self.m_lCurSlider)
      local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")
      btnLayout:setVisible(false)   
      self:playSound("button.wav") 
  elseif tag - TAG.tag_opreate_btn == 2 then
      local lScore = self._scene.m_lTurnLessScore
      
      local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")
      
      btnLayout:runAction(cc.Sequence:create(cc.MoveBy:create(0.1,cc.p(0,-110)),cc.CallFunc:create(function ( ... )
          btnLayout:setVisible(false)
          self:onEventAddChip(lScore)
      end)))

      print("跟注")
  elseif tag - TAG.tag_opreate_btn == 3 then
      
      local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")

      btnLayout:runAction(cc.Sequence:create(cc.MoveBy:create(0.1,cc.p(0,-110)),cc.CallFunc:create(function ( ... )
          btnLayout:setVisible(false)
          self:onEventGiveUp()
      end)))

      print("弃牌")
  elseif tag - TAG.tag_opreate_btn == 4 then

      local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")
      btnLayout:runAction(cc.Sequence:create(cc.MoveBy:create(0.1,cc.p(0,-110)),cc.CallFunc:create(function ( ... )
          btnLayout:setVisible(false)
          self:onEventAddChip(0)
      end)))

      print("过牌")
  elseif tag - TAG.tag_opreate_btn == 5 or tag == TAG.tag_allIn then
      local lScore = self._scene.m_lTurnMaxScore

      local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")
      btnLayout:runAction(cc.Sequence:create(cc.MoveBy:create(0.1,cc.p(0,-110)),cc.CallFunc:create(function ( ... )
          btnLayout:setVisible(false)
          self:onEventAddChip(lScore)
      end)))

      print("梭哈")
  elseif tag==TAG.tag_opreate_btn+10+4 or tag==TAG.tag_opreate_btn+10+5 or tag==TAG.tag_opreate_btn+10+6 then
      local lScore = ref.lChip
      
      local btnLayout = self._bottomNode:getChildByName("btn_ctrl_layout")

      btnLayout:runAction(cc.Sequence:create(cc.MoveBy:create(0.1,cc.p(0,-110)),cc.CallFunc:create(function ( ... )
          btnLayout:setVisible(false)
          self:onEventAddChip(lScore)
      end)))
  elseif tag == TAG.tag_last then   
      self:onEventLastTurn()
  elseif tag == TAG.tag_cardType then
      local menu = self._rootNode:getChildByName("Panel_menu")
      menu:setVisible(false)
      local csbNode = cc.CSLoader:createNode("game_res/typeLayer.csb");
      self:addChild(csbNode,TOP_ZOrder)

      --遮罩
      local Panel_mask = csbNode:getChildByName("Panel_1")
      Panel_mask:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            csbNode:removeFromParent()
            --self:playSound("button.wav")
        end
      end)

      --关闭
      local btn = Panel_mask:getChildByName("btn_close")
      btn:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            csbNode:removeFromParent()
            self:playSound("button.wav")
        end
      end)
      self:playSound("button.wav")
  elseif tag == TAG.tag_standUp then
      local menu = self._rootNode:getChildByName("Panel_menu")
      menu:setVisible(false) 
      if self._scene.m_bCanStandUp then 
         self:onEventStandUp()
         self._scene.m_bCanStandUp = false
      else
        showToast(self._scene, "游戏未开始,暂不能起立!", 2)  
      end
  elseif tag == TAG.tag_scoreAdd then
    if self._scene.m_bLookOn then
      showToast(self, "观战中不可使用该功能!", 1)
    end
      local menu = self._rootNode:getChildByName("Panel_menu")
      menu:setVisible(false)

      --发消息获取可以买入的筹码
      self:onEventGetEnableScore()

      --self:setTakeChip()
      self:playSound("button.wav")
  elseif tag == TAG.tag_dismiss then
      local menu = self._rootNode:getChildByName("Panel_menu")
      menu:setVisible(false)
      if PriRoom and GlobalUserItem.bPrivateRoom then
        PriRoom:getInstance():queryDismissRoom()
      end
      self:playSound("button.wav")
  elseif tag == TAG.tag_back then
      local menu = self._rootNode:getChildByName("Panel_menu")
      menu:setVisible(false)  
      if not self._scene:isGaming() then
        self:getParentNode():onExitTable()
      else
        self:getParentNode():onQueryExitGame()
      end
  elseif tag == TAG.tag_set then
      local menu = self._rootNode:getChildByName("Panel_menu")
      menu:setVisible(false) 

      self:showSet()
      self:playSound("button.wav")
  elseif tag == TAG.tag_menu then
       local menu = self._rootNode:getChildByName("Panel_menu")
       menu:setVisible(true)
       --menu:setPosition(-667,-375)
       menu:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            menu:setVisible(false)
        end
      end)
       self:playSound("button.wav")
  elseif tag == TAG.tag_priRecord then
      if self._recordView  then
        self._recordView:setVisible(true)
        self:refreshRecord()
        return
      end
      local csbNode = cc.CSLoader:createNode("game_res/game_record.csb");
      self:addChild(csbNode,TOP_ZOrder)

      self._recordView = csbNode
      self._rootNode:getChildByName("Panel_menu"):setVisible(false)
      --遮罩
      local Panel_mask = csbNode:getChildByName("Panel_1")
      Panel_mask:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            csbNode:setVisible(false)
        end
      end)

      --关闭
      local btn = csbNode:getChildByName("btn_close")
      btn:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            csbNode:setVisible(false)
        end
      end)

      self:refreshRecord()
    elseif tag == TAG.tag_chat then
      if self._scene.m_bLookOn then
        showToast(self, "观战中不可使用该功能!", 1)
      else
        self.m_GameChat:showGameChat(true)
      end
    elseif tag == TAG.tag_location then
      if self._scene.m_bLookOn then
        showToast(self, "观战中不可使用该功能!", 1)
      else
        self:showAdressLayer()
      end
    elseif tag == TAG.tag_change then
        local menu = self._rootNode:getChildByName("Panel_menu")
        menu:setVisible(false)
      
        if self._scene.m_cbGameStatus == cmd.GS_FREE then
            if self.isEnabledChange then
                self.isEnabledChange = false
                ref:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                    self.isEnabledChange = true
                end)))
                self.btn_change1:setVisible(false)
                self:showReadyFlag(cmd.MY_VIEW_INDEX,false)
                self._scene:onChangeDesk()
            else
                showToast(self._scene, "更换桌子或座位频繁，请稍后再试！", 2)
            end
        else
            showToast(self._scene,"游戏中不能换桌！",2)
        end
    elseif TAG.tag_sit == tag then
      self._scene._gameFrame.bChangeDesk = true
      self._scene._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID,yl.INVALID_CHAIR)
    elseif TAG.tag_begin == tag then
      self.btStart:setVisible(false)
      self._scene._gameFrame:SendUserBegin()
    end  
end

function GameViewLayer:showSitBtn(visible,time)
  if not self._scene.m_bLookOn then
    if visible then
      if not self._beginTimeId and time then
           self.m_beginTimeNum = time
           self.btStart:getChildByName("Text"):setString("("..self.m_beginTimeNum..")")
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
    self.m_readyBtn:setVisible(false);
  end
end

function GameViewLayer:updateBeginTime()
  if self.m_beginTimeNum and self.m_beginTimeNum >0 then 
    self.m_beginTimeNum = self.m_beginTimeNum -1
    self.btStart:getChildByName("Text"):setString("("..self.m_beginTimeNum..")")  
  else
    if self._beginTimeId then
      cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._beginTimeId)
      self._beginTimeId = nil
    end
  end

end

function GameViewLayer:refreshRecord()
  if not self._recordView  then 
    return
  end
  local resource = self._scene._gameReord
  --dump(resource, "携带筹码", 6)
  local curIdx = 1
  for i=1,9 do
    if resource.lTakeScore[1][i] > 0  then
        local node = self._recordView:getChildByName("FileNode_"..curIdx)
        node:setVisible(true)

        local useritem  = self._scene:getSitUserInfoByChairID(i-1)
        assert(useritem)
        --昵称
        local nick = node:getChildByName("txt_1")
        --nick:setString(useritem.szNickName)

        local clipNick = ClipText:createClipText(cc.size(100, 20), useritem.szNickName, "" , 18)
        clipNick:setAnchorPoint(cc.p(0.5, 0.5))
        clipNick:setPosition(nick:getPosition())
        clipNick:setName("txt_1")
        node:addChild(clipNick)

        nick:removeFromParent()

        --携带
        local take = node:getChildByName("txt_2")
        take:setString(""..resource.lTakeScore[1][i])

        --盈利
        local win = node:getChildByName("txt_3")
        local str = ""
        win:setColor(cc.YELLOW)
        if resource.lAllScore[1][i] >= 0 then
          str = "+"..resource.lAllScore[1][i]
          win:setColor(cc.YELLOW)
        else
          str = ""..resource.lAllScore[1][i]
          win:setColor(cc.RED)
        end
        win:setString(str)
        curIdx = curIdx + 1
    end
  end
end

--加注
function GameViewLayer:onEventAddChip(lchip)
   lchip = math.min(lchip, self._scene.m_lTakeScore[self._scene._wchair+1])
   local dataBuffer = CCmd_Data:create(8)
   dataBuffer:pushscore(lchip)
   self._scene:SendData(cmd.SUB_C_ADD_SCORE,dataBuffer)
end

--放弃
function GameViewLayer:onEventGiveUp()
  local dataBuffer = CCmd_Data:create()
  self._scene:SendData(cmd.SUB_C_GIVE_UP,dataBuffer)
  self._scene.m_cbPlayStatus[self._scene._wchair+1] = 0
end

--放弃
function GameViewLayer:onEventLastTurn()
  local dataBuffer = CCmd_Data:create()
  self._scene:SendData(cmd.SUB_C_LAST_RECORD,dataBuffer)
  print("onEventLastTurn============================")
end


--设置携带
function GameViewLayer:onEventSetChip(lchip)
  print("=========================1设置携带")
  local dataBuffer = CCmd_Data:create(9)
  dataBuffer:pushbool(true)
  dataBuffer:pushscore(lchip)
  self._scene:SendData(cmd.SUB_C_SET_CHIP,dataBuffer)
end

--发送结束 重新下局
function GameViewLayer:onEventNewGame()
  local dataBuffer = CCmd_Data:create()
  self._scene:SendData(cmd.SUB_C_GAME_END,dataBuffer)
end

--起立
function GameViewLayer:onEventStandUp()
  local dataBuffer = CCmd_Data:create(2)
  dataBuffer:pushword(self._scene._wchair)
  self._scene:SendData(cmd.CMD_S_StandUp,dataBuffer)
end

--请求流水
function GameViewLayer:onEventGetRecord()
  local dataBuffer = CCmd_Data:create()
  self._scene:SendData(cmd.SUB_C_GAME_RECORD, dataBuffer)
end

--请求玩家可用金币
function GameViewLayer:onEventGetEnableScore()
  local dataBuffer = CCmd_Data:create()
  self._scene:SendData(cmd.SUB_C_GET_REST_SCORE, dataBuffer)
end


function GameViewLayer:removeTimer()
  if self._timer then
      self._timer:stopAllActions()
      self._timer:removeFromParent()
      self._timer = nil
  end
end

--倒计时0处理
function GameViewLayer:logicTimeZero()

  self:removeTimer()

  if not self._scene then
    return 
  end

  -- if (PriRoom and GlobalUserItem.bPrivateRoom) and not GlobalUserItem.bPrivateGoldRoom and GlobalUserItem.wCurRoomGroupID == 0 then
  if (PriRoom and GlobalUserItem.bPrivateRoom) and not GlobalUserItem.bPrivateGoldRoom then
      return    
  end
  if PriRoom and GlobalUserItem.wCurRoomGroupID ~= 0 and self._scene.m_bOnGame == false then
    return
  end

  if self._scene.m_cbGameStatus == cmd.GS_FREE and self.timerViewId == cmd.MY_VIEW_INDEX then

    self._scene:onExitTable()
  end

  --local wChairID = self._scene._wchair
  if self._scene and self._scene.m_wCurrentUser == self._scene._wchair then
     --self:onEventGiveUp()
     local addNode = self:getChildByTag(TAG.tag_addChip)
     if addNode then
        addNode:removeFromParent()
     end
  end
end

--显示聊天
function GameViewLayer:ShowUserChat(viewid ,message)
    print("发言的viewid",viewid)
    if message and #message > 0 then
        if viewid==cmd.MY_VIEW_INDEX then
            self.m_GameChat:showGameChat(false)
        end
        --取消上次
        local chatbg = self._rootNode:getChildByName(string.format("im_chat_%d",viewid))
        if chatbg then
            chatbg:stopAllActions()
            chatbg:removeAllChildren()
            local _labChat = cc.LabelTTF:create(message, "fonts/round_body.ttf", 20, cc.size(150,0), cc.TEXT_ALIGNMENT_CENTER)        
            chatbg:addChild(_labChat)

            local labSize = _labChat:getContentSize()
            if labSize.height >= 40 then
                chatbg:setContentSize(180, labSize.height + 50)
            else
                chatbg:setContentSize(180, 50)
            end
            _labChat:setPosition(chatbg:getContentSize().width * 0.5, chatbg:getContentSize().height * 0.5+10)

            local sc = cc.ScaleTo:create(0.1, 1.0, 1.0)
            local show = cc.Show:create()
            local spa = cc.Spawn:create(sc, show)
            chatbg:runAction(cc.Sequence:create(spa, cc.DelayTime:create(2.0), cc.ScaleTo:create(0.1, 0.00001, 1.0), cc.Hide:create())) 
        end
    end
end

--显示表情
function GameViewLayer:ShowUserExpression(viewid,index)
    if viewid==cmd.MY_VIEWID then
        self.m_GameChat:showGameChat(false)
    end
  --取消上次
    local seat = self._bottomNode:getChildByName(string.format("seat_%d", viewid))
    local lastBrow = seat:getChildByName("head_bg"):getChildByTag(99)
    if lastBrow then
        lastBrow:stopAllActions()
        lastBrow:removeFromParent()
    end

  ----********************
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
    lastBrow = cc.Sprite:createWithSpriteFrame(spriteFrame)
    lastBrow:setPosition(54,80)
    lastBrow:setTag(99)
    seat:getChildByName("head_bg"):addChild(lastBrow,11)

    lastBrow:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    lastBrow:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()
        
        if lastBrow then
            lastBrow:stopAllActions()
            lastBrow:removeFromParent()
            lastBrow = nil
        end
    end)))
  
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
      vioceanim:setScale(0.8)
      vioceanim:setRotation(180)
      vioceanim:setPosition(chatbg:getContentSize().width * 0.5, chatbg:getContentSize().height * 0.5+4.3)
      if viewId == 3 or viewId == 4 then
        vioceanim:setRotation(0)
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

function GameViewLayer:showRoomInfo(isShow)
    if self._recordView  then
        self._recordView:setVisible(true)
        self:refreshRecord()
        return
    end
    local csbNode = cc.CSLoader:createNode("game_res/game_record.csb");
    self:addChild(csbNode,TOP_ZOrder)

    self._recordView = csbNode
    self._rootNode:getChildByName("Panel_menu"):setVisible(false)
    --遮罩
    local Panel_mask = csbNode:getChildByName("Panel_1")
    Panel_mask:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
          csbNode:setVisible(false)
      end
    end)

    --关闭
    local btn = csbNode:getChildByName("btn_close")
    btn:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
          csbNode:setVisible(false)
      end
    end)

    self:refreshRecord()
end


return GameViewLayer
