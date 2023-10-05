--
-- Author: Tang
-- Date: 2016-12-08 15:41:53
--
local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.xiuxian.forestdance.src"
--local module_pre = "game.battle.forestdance.src"

local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"
local PopupInfoHead = appdf.EXTERNAL_SRC .. "PopupInfoHead"
local cmd = module_pre .. ".models.cmd_game"
local BankerList = require(module_pre..".views.layer.BankerList")
local Bank = require(module_pre..".views.layer.Bank")
local GameGunNum = require(module_pre..".views.layer.GameGunNum")
local UserList = require(module_pre..".views.layer.UserList")
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
--local QueryDialog = appdf.req("base.src.app.views.layer.QueryDialog")
local QueryDialog = appdf.req(appdf.BASE_SRC.."app.views.layer.other.QueryDialog")
local scheduler = cc.Director:getInstance():getScheduler()

GameViewLayer.Tag = 
{   
     clock_num        =1,
     btn_userlist     =2,
     btn_bankerlist   =3,
     btn_bank         =4,
     btn_sound        =5,
     btn_backRoom     =6,
     btn_help         =7,
     Tag_GunNum       = 200
}
--动作Tag
GameViewLayer.AnimalTag = 
{
  Tag_Animal = 1
}

local Tag = GameViewLayer.Tag

GameViewLayer.TopZorder = 30
GameViewLayer.ViewZorder = 20

function GameViewLayer:ctor(scene)
  self._scene = scene
  self._userList = {}
  self._colorList = {}
  self._bMenu = false

  self._bSound = true
  self._bMusic = true
  self.bContinueRecord = true  
  self:gameDataInit()

  self:initConstValue()

 self:loadRes()


  --注册事件
  ExternalFun.registerTouchEvent(self,true)
  --dyj1
  --hallBG
  --hallBackground:setVisible(false)
  --dyj2
end

function GameViewLayer:onExit()
  --hallBackground:setVisible(true)
  self:gameDataReset()
  self:unSchedule()
  AudioEngine.stopMusic()
  self:removeAnimations()
end

function GameViewLayer:resetData()
  if self:GetJettonRecord() == 0 then
    self.bContinueRecord = true --续压按钮
  else
    self.bContinueRecord = false
  end
end

function GameViewLayer:readOnly(t)
    local _table = {}
    local mt = {
        __index = t,
        __newindex = function()
            error(" the table is read only ")
        end
    }
    setmetatable(_table, mt)
    return _table
end

function GameViewLayer:initConstValue()
  --设置const数组
   local value = {13,13,14,13} --动物正常动画时间
   self._animalTimeFree = self:readOnly(value) 

   value = {5.5,4,4,3}  --动物胜利动画时间
   self._animalTimeWin = self:readOnly(value)

   value = {100,1000,10000,100000,1000000,10000000} --下注筹码
   self._jettonArray = self:readOnly(value)
end

function GameViewLayer:removeAnimations()
     cc.AnimationCache:getInstance():removeAnimation("CJAnim")
     cc.AnimationCache:getInstance():removeAnimation("SDAnim")
     cc.AnimationCache:getInstance():removeAnimation("SXAnim")
     cc.AnimationCache:getInstance():removeAnimation("SYAnim")
end

function GameViewLayer:loadRes( )
  self._scene:dismissPopWait()
  --加载层
  self:loading()
  self._resLoadFinish = false
  self._scene._scene:showPlazaBackgroup(false)
  
--2d资源
  self._2dResCount = 0
  self._2dResTotal = 4
  cc.Director:getInstance():getTextureCache():addImageAsync("game_res/game.png", handler(self, self.load2DModelCallBack))
  cc.Director:getInstance():getTextureCache():addImageAsync("game_res/anim_sd.png", handler(self, self.load2DModelCallBack))
  cc.Director:getInstance():getTextureCache():addImageAsync("game_res/anim_sx.png", handler(self, self.load2DModelCallBack))
  cc.Director:getInstance():getTextureCache():addImageAsync("game_res/anim_sy.png", handler(self, self.load2DModelCallBack))



      --dump( cc.FileUtils:getInstance():getSearchPaths());

  --3D资源
  local modelFiles = {}
  table.insert(modelFiles, "3d_res/model_0/wujian.c3b")
  table.insert(modelFiles, "3d_res/model_1/wujian02.c3b")
  table.insert(modelFiles, "3d_res/model_2/wujian04.c3b")
  table.insert(modelFiles, "3d_res/model_3/wujian03.c3b")
  table.insert(modelFiles, "3d_res/model_4/wujian07.c3b")
  table.insert(modelFiles, "3d_res/model_5/wujian07.c3b")
  table.insert(modelFiles, "3d_res/model_6/wujian08.c3b")
  table.insert(modelFiles, "3d_res/model_7/wujian11.c3b")
  table.insert(modelFiles, "3d_res/model_8/wujian10.c3b")
  table.insert(modelFiles, "3d_res/model_bottom/dibu.c3b")
  table.insert(modelFiles, "3d_res/model_bottom1/dibu2.c3b")
  table.insert(modelFiles, "3d_res/model_monkey/monkey.c3b")
  table.insert(modelFiles, "3d_res/model_lion/lion.c3b")
  table.insert(modelFiles, "3d_res/model_panda/panda.c3b")
  table.insert(modelFiles, "3d_res/model_rabbit/rabbit.c3b")
  table.insert(modelFiles, "3d_res/model_seat/di.c3b")

  self._3dResCount = #modelFiles
  self._3dIndex = 0

  for i,v in ipairs(modelFiles) do
    local file = v
    cc.Sprite3D:createAsync(file,handler(self, self.load3DModelCallBack))
  end
end

function GameViewLayer:unLoadRes()
  cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game_res/game.plist")
  cc.Director:getInstance():getTextureCache():removeTextureForKey("game_res/game.png")

  cc.Sprite3DCache:getInstance():removeAllSprite3DData()
  cc.Sprite3DCache:destroyInstance()
end

function GameViewLayer:load2DModelCallBack( texture )
  
  self._2dResCount = self._2dResCount + 1
  if self._3dIndex == self._3dResCount and self._2dResCount == self._2dResTotal and not self._resLoadFinish then

  -- print("111===","load2DModelCallBack")
    cc.SpriteFrameCache:getInstance():addSpriteFrames("game_res/game.plist")
    cc.SpriteFrameCache:getInstance():addSpriteFrames("game_res/anim_sd.plist")
    cc.SpriteFrameCache:getInstance():addSpriteFrames("game_res/anim_sx.plist")
    cc.SpriteFrameCache:getInstance():addSpriteFrames("game_res/anim_sy.plist")

    local function readAnimation(file, key, num, time)
        local frames = {}
        local actionTime = time
        for i=1,num do
            local frameName = string.format(file.."%d.png", i-1)
            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(frameName) 
            table.insert(frames, frame)
        end

        local  animation =cc.Animation:createWithSpriteFrames(frames,actionTime)
        cc.AnimationCache:getInstance():addAnimation(animation, key)
    end

    readAnimation("cj","CJAnim",15,0.07)
    readAnimation("sd","SDAnim",15,0.07)
    readAnimation("sx","SXAnim",15,0.07)
    readAnimation("sy","SYAnim",15,0.07)

    self._scene._scene:removeChildByTag(23)
    self:init3DModel()
    self:initAnimal()
    self:initCsbRes()
    self._resLoadFinish = true
    self._scene._bCaijinStatus = true
    
    if self._scene._gameModel._bScene then
      self._animLayer:setRotation3D(cc.vec3(0,360-self._scene._gameModel._sceneData.nAnimalRotateAngle*15,0))
      self._arrow:setRotation3D(cc.vec3(0,self._scene._gameModel._sceneData.nPointerRatateAngle*15+180,0))
    end

    if self._scene._gameStatus <= g_var(cmd).IDI_GAME_BET then --空闲或下注状态
      self._loadLayer:removeFromParent()
      self._loadLayer = nil

      --self:updateColor()

      self:setGameStatus(self._scene._gameStatus)

      if self._scene._gameStatus == g_var(cmd).IDI_GAME_BET then --下注状态
        --更新区域倍率
        self:updateAreaMultiple()
        --弹出下注层
        self:popPlaceJettonLayer()
          --更新按钮状态
        self:updateControl()
      end
    end
  end
end
--下注层
function GameViewLayer:popPlaceJettonLayer(pos)
    if pos == g_var(cmd).NormalPos or not pos then    --弹出
      if self._loadLayer then
        self._loadLayer:removeFromParent()
      end
      self._PlaceJettonLayer:setVisible(true)
      self._PlaceJettonLayer:runAction(cc.MoveTo:create(0.2,cc.p(yl.WIDTH/2,yl.HEIGHT/2 - 60)))
      self._posStyle = g_var(cmd).NormalPos
      self:playEffect("START_BET.wav")
    elseif pos == g_var(cmd).bottomHidden then --弹到底部
      self._PlaceJettonLayer:setVisible(true)
      self._PlaceJettonLayer:runAction(cc.MoveTo:create(0.2,cc.p(yl.WIDTH/2,-250)))
      self._posStyle = g_var(cmd).bottomHidden
    elseif pos == g_var(cmd).hidden then       --全部隐藏
      self._PlaceJettonLayer:runAction(
        cc.Sequence:create(cc.MoveTo:create(0.2,cc.p(yl.WIDTH/2, -yl.HEIGHT)),
        cc.CallFunc:create(function()
              self._PlaceJettonLayer:setVisible(false)
              self._posStyle = g_var(cmd).hidden
          end))
        )
    end
end

function GameViewLayer:load3DModelCallBack(...)
   -- print("load 3d model")
   self._3dIndex = self._3dIndex + 1
   if self._3dIndex == self._3dResCount and self._2dResCount == self._2dResTotal and not self._resLoadFinish then
     self._scene._scene:removeChildByTag(23)
     self:init3DModel()
     self:initAnimal()
     self:initCsbRes()
     self._resLoadFinish = true
     self._scene._bCaijinStatus = true
  
     if self._scene._gameModel._bScene then
      self._animLayer:setRotation3D(cc.vec3(0,360-self._scene._gameModel._sceneData.nAnimalRotateAngle*15,0))
      self._arrow:setRotation3D(cc.vec3(0,self._scene._gameModel._sceneData.nPointerRatateAngle*15+180,0))
     end

     if self._scene._gameStatus <= g_var(cmd).IDI_GAME_BET then --空闲或下注状态
      self._loadLayer:removeFromParent()
      self._loadLayer = nil
      --self:updateColor()

      self:setGameStatus(self._scene._gameStatus)

      if self._scene._gameStatus == g_var(cmd).IDI_GAME_BET then --下注状态
        --更新区域倍率
        self:updateAreaMultiple()
        --弹出下注层
        self.popPlaceJettonLayer()
        --更新按钮状态
        self:updateControl()
      end
     end
   end
end

function GameViewLayer:updateControl()  --更新按钮状态
  --下注筹码
  local btns = {}
  local jettonCount = 6
  for i=1,jettonCount do
    local btn = self._PlaceJettonLayer.rootNode:getChildByName(string.format("btn_%d", i))
    btn:setEnabled(false)
    table.insert(btns, btn)
  end

  local useritem = self._scene:GetMeUserItem()
  local minScore = useritem.lScore

  --过滤当前庄家是自己
  if self._scene.m_wBankerUser == useritem.wChairID then

     local btn = self._PlaceJettonLayer.rootNode:getChildByName("btn_continue")
     btn:setEnabled(false)
    return
  end

  --非庄家坐庄
  if self._scene.m_wBankerUser ~= yl.INVALID_CHAIR then
    local itemBanker = self._scene._gameModel:getUserByChair(self._scene:getUserList(),self._scene.m_wBankerUser)
    if itemBanker then
      minScore = math.min(minScore,itemBanker.lScore)
    end
  end

  for i=1,#btns do
    local btn = btns[i]
    local value = self._jettonArray[i]
    if value > minScore then
      btn:setEnabled(false)
      if i == 1 then
        self:setJettonIndex(-1)
      end
    else
      btn:setEnabled(true)  
    end
  end

  --如果当前筹码不符合条件,切换成第一个筹码
  if ( self._scene._curJettonIndex ~= -1 ) and ( not btns[self._scene._curJettonIndex]:isEnabled() ) then 
    self:setJettonIndex(1)
  end

 --续压
  local btn = self._PlaceJettonLayer.rootNode:getChildByName("btn_continue")
  if self.bContinueRecord then 
      btn:setEnabled(false)
  else
      btn:setEnabled(true) 
  end
end

function GameViewLayer:initJettonIndex()

  local useritem = self._scene:GetMeUserItem()
  local minScore = useritem.lScore
  -- print("minScore is ===================="..minScore)
  -- print("the array count is ==================="..#self._jettonArray)
  local bfirstInvalid = true
  for i=1,6 do
    local value = self._jettonArray[i]
    -- print("value is ======================>"..value)
    if value > minScore then
      if 1 == i then
        bfirstInvalid = false
        break
      end
    end
  end
    
  --默认第一个筹码 
  if true == bfirstInvalid then
    self:setJettonIndex(1)
  else
    self:setJettonIndex(-1)
  end
end

function GameViewLayer:setJettonIndex( index )
  if index == -1 then
    self:blinkEffect(false)
    return
  end

  self._scene._curJettonIndex = index
  self:blinkEffect(true,index)
end

function GameViewLayer:getCurJettonIndex( )
  return self._scene._curJettonIndex
end

function GameViewLayer:blinkEffect(isAction,index) --闪动效果
  local circles = {}
  for i=1,6 do
    local circle = self._PlaceJettonLayer.rootNode:getChildByName(string.format("circle_%d", i))
    table.insert(circles, circle)
    circle:setVisible(false)
    circle:stopAllActions()
  end

  if true == isAction then
    local circle = circles[index]
    circle:setVisible(true)
    circle:runAction(cc.RepeatForever:create(cc.Blink:create(1, 2)))
  end
end

function GameViewLayer:updateAreaMultiple()

end

--重置游戏
function GameViewLayer:resetGame()

  if self._loadLayer then
    self._loadLayer:removeFromParent()
    self._loadLayer = nil
  end

  self:resetAnimal()
  self._seat:stopAllActions()

  self._rewardLayer:setVisible(false)

end

function GameViewLayer:GetJettonRecord()
  local record = 0
  for i=1,#self._scene.m_lContinueRecord do
    record = record + self._scene.m_lContinueRecord[i]
  end
  return record
end

function GameViewLayer:resetAnimal()
    for i=1,#self._animals do
      local animal = self._animals[i]
      local index = animal:getTag()
      local animIndex = math.mod(index-1,4)
      local modelFile = self:getAnimRes(animIndex)
      local texture = self:getAnimIMG(animIndex)
      local angle = (i-1)*15
      animal:setPosition3D(cc.vec3(19*math.sin(math.rad(angle)), 0, -19*math.cos(math.rad(angle))))
      animal:setRotation3D(cc.vec3(0, 360-angle, 0))

      animal:setTexture(texture)

      if index == self._scene._drawIndex then
        animal:stopActionByTag(GameViewLayer.AnimalTag.Tag_Animal)
        animal:runAction(cc.Sequence:create(delay,cc.CallFunc:create(function()
          local fTime = math.random(0,1)*5
          local animtion = cc.Animation3D:create(modelFile)
          local delay = cc.DelayTime:create(fTime)
          local action =  cc.Animate3D:create(animtion, 0, self._animalTimeFree[animIndex+1])
          local rep = cc.RepeatForever:create(action)
          rep:setTag(GameViewLayer.AnimalTag.Tag_Animal)
          animal:runAction(rep)
        end)))
      end
    end

    local normal = "3d_res/model_5/tex.jpg"
    self._seat:setTexture(normal)

    self._scene._drawIndex = 0

    self._camera:runAction(cc.MoveTo:create(0.5,g_var(cmd).Camera_Normal_Vec3))
end

function GameViewLayer:init3DModel()

  self.m_3dLayer = cc.Layer:create()
  self:addChild(self.m_3dLayer)

  self._camera = cc.Camera:createPerspective(60,yl.WIDTH/yl.HEIGHT,1,1000)
  self._camera:setPosition3D(g_var(cmd).Camera_Normal_Vec3)
  self._camera:lookAt(cc.vec3(0, 0, 0))
  self._camera:setCameraFlag(cc.CameraFlag.USER1)
  self.m_3dLayer:addChild(self._camera)

  local sprite = cc.Sprite3D:create("3d_res/model_bottom/dibu.c3b")
  sprite:setScale(1.0)
  sprite:setPosition3D(cc.vec3(0, -1.5, 0))
  sprite:setCameraMask(cc.CameraFlag.USER1)
  self.m_3dLayer:addChild(sprite)

  sprite = cc.Sprite3D:create("3d_res/model_bottom1/dibu2.c3b")
  sprite:setScale(1.0)
  sprite:setPosition3D(cc.vec3(0, -1.5, 0))
  sprite:setRotation3D(cc.vec3(0, 60, 0))
  sprite:setCameraMask(cc.CameraFlag.USER1)
  self.m_3dLayer:addChild(sprite)

  for i=1,6 do
    local wujian1 = cc.Sprite3D:create("3d_res/model_0/wujian.c3b")
    wujian1:setScale(1.0)
    wujian1:setPosition3D(cc.vec3(0, -0.1, 0))
    wujian1:setRotation3D(cc.vec3(0, 60*(i-1)+30, 0))
    wujian1:setGlobalZOrder(1)
    wujian1:setCameraMask(cc.CameraFlag.USER1)
    self.m_3dLayer:addChild(wujian1)

    local wujian2 = cc.Sprite3D:create("3d_res/model_1/wujian02.c3b")
    wujian2:setScale(1.0)
    wujian2:setPosition3D(cc.vec3(0, -0.1, 0))
    wujian2:setRotation3D(cc.vec3(0, 60*(i-1)+30, 0))
    wujian2:setGlobalZOrder(1)
    wujian2:setCameraMask(cc.CameraFlag.USER1)
    self.m_3dLayer:addChild(wujian2)

    local wujian3 = cc.Sprite3D:create("3d_res/model_3/wujian03.c3b")
    wujian3:setScale(1.0)
    wujian3:setPosition3D(cc.vec3(0, -0.1, 0))
    wujian3:setRotation3D(cc.vec3(0, 60*(i-1)+30, 0))
    wujian3:setGlobalZOrder(1)
    wujian3:setCameraMask(cc.CameraFlag.USER1)
    self.m_3dLayer:addChild(wujian3)

    local wujian4 = cc.Sprite3D:create("3d_res/model_2/wujian04.c3b")
    wujian4:setScale(1.0)
    wujian4:setPosition3D(cc.vec3(0, -0.1, 0))
    wujian4:setRotation3D(cc.vec3(0, 60*(i-1), 0))
    wujian4:setGlobalZOrder(1)
    wujian4:setCameraMask(cc.CameraFlag.USER1)
    self.m_3dLayer:addChild(wujian4)
  end
end
--加载层
function GameViewLayer:loading()
  self._scene:dismissPopWait()
  if self._loadLayer then
    self._loadLayer:removeFromParent()
  end
  self._loadLayer = cc.Layer:create()
  self:addChild(self._loadLayer,GameViewLayer.TopZorder)

  --加载背景
  local bg = cc.Sprite:create("load_res/im_loadbg_0.png")
  bg:setAnchorPoint(cc.p(0.5,0.5))
  bg:setPosition(cc.p(yl.WIDTH/2,yl.HEIGHT/2))
  self._loadLayer:addChild(bg)
  
  --title
  local title = cc.Sprite:create("load_res/im_title_0.png")
  title:setAnchorPoint(cc.p(0.5,0.5))
  title:setPosition(cc.p(yl.WIDTH/2,yl.HEIGHT/2))
  self._loadLayer:addChild(title)

  local frames = {}
  for j=1,2 do
    local frame = cc.SpriteFrame:create("load_res/"..string.format("im_loadbg_%d.png", j-1),cc.rect(0,0,1334,750))
    table.insert(frames, frame)
  end
  local animation =cc.Animation:createWithSpriteFrames(frames,0.5)
  local animate = cc.Animate:create(animation)
  bg:runAction(cc.RepeatForever:create(animate))

  frames = {}
  for j=1,2 do
    local frame = cc.SpriteFrame:create("load_res/"..string.format("im_title_%d.png", j-1),cc.rect(0,0,782,375))
    table.insert(frames, frame)
  end
  animation =cc.Animation:createWithSpriteFrames(frames,0.5)
  animate = cc.Animate:create(animation)
  title:runAction(cc.RepeatForever:create(animate))
end

function GameViewLayer:getAnimRes(animIndex)
  local res = "3d_res/model_lion/lion.c3b"

  if animIndex == 1 then
      res = "3d_res/model_panda/panda.c3b"
  elseif animIndex == 2 then
      res = "3d_res/model_monkey/monkey.c3b"
  elseif animIndex == 3 then
      res = "3d_res/model_rabbit/rabbit.c3b"
  end

  return res
end

function GameViewLayer:getAnimIMG(animIndex)
   local res = "3d_res/model_lion/tex.jpg"

  if animIndex == 1 then
      res = "3d_res/model_panda/tex.jpg"
  elseif animIndex == 2 then
      res = "3d_res/model_monkey/tex.jpg"
  elseif animIndex == 3 then
      res = "3d_res/model_rabbit/tex.jpg"
  end

  return res
end

function GameViewLayer:initAnimal()
  self._animLayer = cc.Sprite3D:create()
  self._animLayer:setPosition3D(cc.vec3(0, 0,0))
  self.m_3dLayer:addChild(self._animLayer)
  local file1 = "3d_res/model_8/wujian10.c3b"
  local file2 = "3d_res/model_seat/di.c3b"

  self._animals = {}

  math.randomseed( tonumber(tostring(os.time()):reverse():sub(1,6))) 

  for i=1,24 do
    local angle = 15*(i-1)
    local sprite = cc.Sprite3D:create(file1)
    sprite:setScale(1.0)
    sprite:setPosition3D(cc.vec3(13*math.sin(math.rad(angle)), -0.1, -13*math.cos(math.rad(angle))))
    sprite:setRotation3D(cc.vec3(0, 360-angle, 0))
    sprite:setGlobalZOrder(2)
    sprite:setCameraMask(cc.CameraFlag.USER1)
    self.m_3dLayer:addChild(sprite)

    table.insert(self._colorList, sprite)


    local dizuo = cc.Sprite3D:create(file2)
    dizuo:setScale(1.0)
    dizuo:setPosition3D(cc.vec3(19*math.sin(math.rad(angle)), -0.1, -19*math.cos(math.rad(angle))))
    dizuo:setRotation3D(cc.vec3(-90, 360-angle, 0))
    dizuo:setGlobalZOrder(2)
    dizuo:setCameraMask(cc.CameraFlag.USER1)
    self._animLayer:addChild(dizuo)

    local animIndex = math.mod(i-1,4)
    local modelFile = self:getAnimRes(animIndex)
    local animal = cc.Sprite3D:create(modelFile)
    animal:setScale(1.0)
    animal:setPosition3D(cc.vec3(19*math.sin(math.rad(angle)), 0, -19*math.cos(math.rad(angle))))
    animal:setRotation3D(cc.vec3(0, 360-angle, 0))
    animal:setGlobalZOrder(2)
    animal:setTag(i)
    animal:setCameraMask(cc.CameraFlag.USER1)
    self._animLayer:addChild(animal)

    table.insert(self._animals, animal)
    
    animal:runAction(cc.Sequence:create(delay,cc.CallFunc:create(function()
        local fTime = math.random(0,1)*5
        local animtion = cc.Animation3D:create(modelFile)
        local delay = cc.DelayTime:create(fTime)
        local action =  cc.Animate3D:create(animtion, 0, self._animalTimeFree[animIndex+1])
        local rep = cc.RepeatForever:create(action)
        rep:setTag(GameViewLayer.AnimalTag.Tag_Animal)
        animal:runAction(rep)
    end)))
  end

  --透明转动层
    self._alphaSprite = cc.Sprite3D:create("3d_res/model_6/wujian08.c3b")
    self._alphaSprite:setScale(1.0)
    self._alphaSprite:setPosition3D(cc.vec3(0, -9, 1))
    self._alphaSprite:setGlobalZOrder(1)
    self._alphaSprite:setCameraMask(cc.CameraFlag.USER1)
    self.m_3dLayer:addChild(self._alphaSprite)

    local dizuo = cc.Sprite3D:create("3d_res/model_4/dizuo.c3b")
    dizuo:setScale(1.0)
    dizuo:setPosition3D(cc.vec3(0, 0, 0))
    dizuo:setGlobalZOrder(2)
    dizuo:setCameraMask(cc.CameraFlag.USER1)
    self.m_3dLayer:addChild(dizuo)

    self._seat = cc.Sprite3D:create("3d_res/model_5/wujian07.c3b")
    self._seat:setScale(1.0)
    self._seat:setPosition3D(cc.vec3(0, 0, -0.5))
    self._seat:setGlobalZOrder(1)
    self._seat:setCameraMask(cc.CameraFlag.USER1)
    dizuo:addChild(self._seat)

    self._arrow = cc.Sprite3D:create("3d_res/model_7/wujian11.c3b")
    self._arrow:setScale(0.91)
    self._arrow:setPosition3D(cc.vec3(0,0,0))
    self._arrow:setGlobalZOrder(2)
    self._arrow:setCameraMask(cc.CameraFlag.USER1)
    self.m_3dLayer:addChild(self._arrow)
end

--刷新彩灯
function GameViewLayer:updateColor()
--   if not (self._scene._gameModel._bScene and self._resLoadFinish) then
--     return
--   end
   local colorList = result_start.colorList
   for i=1,#colorList do
     local color = colorList[i]
     local file = "3d_res/model_8/hong.jpg"
     if color == 1 then
       file ="3d_res/model_8/lv.jpg"
     elseif color == 2 then
       file = "3d_res/model_8/huang.jpg"
     end

     local mess =  self._colorList[i]
     
     local function callBackWithArgs(param)
       local ret 
       ret = function()
         mess:setTexture(param)
       end
       return ret 
     end 

     self._colorList[i]:runAction(cc.Sequence:create(cc.DelayTime:create(0.03*(i-1)),cc.CallFunc:create(callBackWithArgs(file))))
   end
end

--指针转动
function GameViewLayer:arrowRunAction() 
    --开奖颜色
    eColorType = result_end.stWinAnimal.STAnimalInfo.eColorType
    local winColorList = {}
    for i = 1,24 do
        if eColorType == result_start.colorList[i] then
            table.insert(winColorList,i)
        end
    end
  self._scene._drawData.nPointerAngle = winColorList[math.random(1,#winColorList)] --随机中奖颜色下标
  
   --中奖动物 狮子0 熊1 猴子2 兔子3
   --动物下标 狮子1 ...
   eAnimalType = result_end.stWinAnimal.STAnimalInfo.eAnimalType
   local winAnimalList = {}
    for i,v in ipairs(self._animals) do
        if math.mod((i-1),4)==eAnimalType then
            table.insert(winAnimalList,i)
        end
    end
    self._scene._drawData.nAnimalAngle = winAnimalList[math.random(1,#winAnimalList)] --随机中奖动物下标
  local stopAngle = math.mod(360+180 - (self._scene._drawData.nPointerAngle-1)*15 ,360)
  stopAngle = stopAngle + 360*12
  local action = cc.EaseCircleActionInOut:create(cc.RotateTo:create(5, cc.vec3(0,stopAngle,0)))
  self._arrow:runAction(action)

  self._alphaSprite:runAction(action:clone())
  self._seat:runAction(action:clone())
end

--动物转动
function GameViewLayer:runRotateAction(dt)
  if self._scene._rotateStatus == g_var(cmd).Stop then
    return
  end

--cmd.Stop        = 0 --静止
--cmd.Speed       = 1--加速
--cmd.ConSpeed    = 2 --匀速
--cmd.SlowDown    = 3--减速
--cmd.RightJust   = 4  --调整位置

  self._scene._rotateTime = self._scene._rotateTime + dt*2
  local angle = self._animLayer:getRotation3D().y - self._scene._rotateSpeed
  angle = (angle<0) and (angle+360) or (math.mod(angle,360))
  self._animLayer:setRotation3D(cc.vec3(0,angle,0))

  if self._scene._rotateStatus == g_var(cmd).Speed then
    self._scene._rotateSpeed = self._scene._rotateSpeed + dt*0.8*2
    if self._scene._rotateSpeed > g_var(cmd).RotateMax then
      self._scene._rotateSpeed = g_var(cmd).RotateMax
      self._scene._rotateStatus = g_var(cmd).ConSpeed
    end
  elseif self._scene._rotateStatus == g_var(cmd).ConSpeed then
     if self._scene._rotateTime > 8 then
       local angle = self._animLayer:getRotation3D().y
       local angle0 = 360
       if  self._scene._drawData.nAnimalAngle then
        angle0 = 360 - self._scene._drawData.nAnimalAngle*15
       end
       local angle1 = angle0 - 120
       if angle1 < 0 then
         angle1 = angle1 + 360
         if angle < angle0 or angle < angle1 then
           self._scene._rotateStatus = g_var(cmd).SlowDown
         end
       elseif angle < angle0 and angle > angle1 then
           self._scene._rotateStatus = g_var(cmd).SlowDown
       end
     end
  elseif self._scene._rotateStatus == g_var(cmd).SlowDown then
     self._scene._rotateSpeed = self._scene._rotateSpeed - dt*1.1*2
     if self._scene._rotateSpeed < g_var(cmd).RotateMin then
       self._scene._rotateStatus = g_var(cmd).RightJust
       self._scene._rotateSpeed = g_var(cmd).RotateMin
     end

  elseif self._scene._rotateStatus == g_var(cmd).RightJust then
    local angle = self._animLayer:getRotation3D().y
    zhizhenjiaodu = math.mod(360+180 - (self._scene._drawData.nPointerAngle-1)*15 ,360)
    local stopAngle =1080 - (360+(360+180 -zhizhenjiaodu) - ((self._scene._drawData.nAnimalAngle-1)*15))
    local _angle =  angle - stopAngle
    --if math.mod(math.abs(_angle),360) < g_var(cmd).RotateMin then
      self._animLayer:setRotation3D(cc.vec3(0,stopAngle,0))
      self._scene._rotateStatus = g_var(cmd).Stop
      self._scene._bAnimalAction = false
      --self:runAction(cc.Sequence:create(cc.DelayTime:create(9), cc.CallFunc:create(function()
            self:rotateEnd()
     -- end)))
      
    --end
  end
end

function GameViewLayer:rotateEnd()
  self._scene._caijinStatus = 0 

--  if self._scene._gameStatus ~= g_var(cmd).IDI_GAME_DRAW then
--    return
--  end

--  if 0 ~= bit:_and(self._scene._drawData.dwAnimationFlag,g_var(cmd).ANIMATION_PAYOUTGUN) then
--    self:gunDeal() --打枪处理
--    return
--  elseif  0 ~= bit:_and(self._scene._drawData.dwAnimationFlag,g_var(cmd).ANIMATION_PAYOUTDSY) then
--    self:showSeatAnim(1)
--    return
--  elseif  0 ~= bit:_and(self._scene._drawData.dwAnimationFlag,g_var(cmd).ANIMATION_PAYOUTDSX) then
--    self:showSeatAnim(0)
--    return 
--  else
--      --中奖动物
--      eAnimalType = result_end.stWinAnimal.STAnimalInfo.eAnimalType

--      --local index = (g_var(cmd).BET_ITEM_TOTAL_COUNT - eAnimalType) + (g_var(cmd).BET_ITEM_TOTAL_COUNT-self._scene._drawData.nPointerAngle)
--      --index = math.mod(index,g_var(cmd).BET_ITEM_TOTAL_COUNT) 
        self._scene._drawIndex = self._scene._drawData.nAnimalAngle

      for i,v in ipairs(self._animals) do
      
        local animal = v
        if animal:getTag() == self._scene._drawIndex then --self._scene._drawIndex 中奖动物下标
        -- print("中奖动物下标==="..self._scene._drawIndex)
          local angle = self._animLayer:getRotation3D().y

          angle = 360-angle+180
          -- print("angle==="..angle)
          animal:runAction(cc.MoveTo:create(0.5,cc.vec3(0,4,0)))
          animal:runAction(cc.RotateTo:create(0.5, cc.vec3(0,angle,0)))

          index = self._scene._drawData.nAnimalAngle - 1
          local resType = math.mod(index,4)
          local modelFile = self:getAnimRes(resType)
          local animtion = cc.Animation3D:create(modelFile)
          animal:stopActionByTag(GameViewLayer.AnimalTag.Tag_Animal)
          local animate = cc.Animate3D:create(animtion, self._animalTimeFree[resType+1], self._animalTimeWin[resType+1])
          local action  = cc.RepeatForever:create(animate)
          action:setTag(GameViewLayer.AnimalTag.Tag_Animal)
          animal:runAction(action)
          --中奖动物
          eAnimalType = result_end.stWinAnimal.STAnimalInfo.eAnimalType
          --中奖颜色
          eColorType = result_end.stWinAnimal.STAnimalInfo.eColorType
          local soundIndex = prizeTurntoIndex(eAnimalType,eColorType) -1
          -- print("eAnimalType==="..eAnimalType,"         eColorType==="..eColorType,"       soundIndex==="..soundIndex)
          self:playEffect(string.format("Animal_%d.wav", soundIndex))
          break
        end
      end

--      if 0 ~= bit:_and(self._scene._drawData.dwAnimationFlag,g_var(cmd).ANIMATION_PAYOUTMGOLD) then
        self:showSeatAnim(1)
--      else
--        self:showSeatAnim(0)
--      end
      self:playEffect("START_DRAW.wav")
      self._camera:runAction(cc.Sequence:create(cc.MoveTo:create(
                                              0.5, g_var(cmd).Camera_Win_Vec3),
                                              cc.DelayTime:create(0.5),
                                              cc.CallFunc:create(function()
                                                      self._rewardLayer:setVisible(true)
                                                      self:showDrawResult(result_end) --显示开奖结果
                                            end)
                                            ))


--  end
end

function GameViewLayer:showDSXAnim(  )
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sx_0.png")
    local dsx = cc.Sprite:createWithSpriteFrame(frame)
    dsx:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
    self:addChild(dsx)

    local anim = cc.AnimationCache:getInstance():getAnimation("SXAnim")
    local animate = cc.Animate:create(anim)
    local action = cc.Sequence:create(animate,cc.CallFunc:create(function(  )
        dsx:removeFromParent()
    end))

    dsx:runAction(action)
end

function GameViewLayer:showDSYAnim(  )
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sy_0.png")
    local dsy = cc.Sprite:createWithSpriteFrame(frame)
    dsy:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
    self:addChild(dsy)

    local anim = cc.AnimationCache:getInstance():getAnimation("SYAnim")
    local animate = cc.Animate:create(anim)
    local action = cc.Sequence:create(animate,cc.CallFunc:create(function(  )
        dsy:removeFromParent()
    end))

    dsy:runAction(action)
end

function GameViewLayer:sohwCJAnim()
   for i=1,#self._animals do 
      local animal = self._animals[i]
      animal:setTexture("3d_res/im_yellow.png")
   end

   self:runAction(cc.Sequence:create(cc.DelayTime:create(1.05),cc.CallFunc:create(function (  )
      for i=1,#self._animals do 
          local animal = self._animals[i]
          local animIndex = math.mod(index-1,4)
          local modelFile = self:getAnimRes(animIndex)
          local texture = self:getAnimIMG(animIndex)
          animal:setTexture(texture)
      end
   end)))

   local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("cj_0.png")
   local cj = cc.Sprite:createWithSpriteFrame(frame)
   cj:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
   self:addChild(cj)

    local anim = cc.AnimationCache:getInstance():getAnimation("CJAnim")
    local animate = cc.Animate:create(anim)
    local action = cc.Sequence:create(animate,cc.CallFunc:create(function(  )
        cj:removeFromParent()
    end))

    cj:runAction(action)
end

function GameViewLayer:showSDAnim()
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sd_0.png")
    local sd = cc.Sprite:createWithSpriteFrame(frame)
    sd:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
    self:addChild(sd)

    local anim = cc.AnimationCache:getInstance():getAnimation("SDAnim")
    local animate = cc.Animate:create(anim)
    local action = cc.Sequence:create(animate,cc.CallFunc:create(function(  )
        sd:removeFromParent()
    end))

    sd:runAction(action)

    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("sd_x%d.png", self._scene._drawData.nLevinMulti))
    local multiple = cc.Sprite:createWithSpriteFrame(frame)
    multiple:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
    multiple:setVisible(false)
    multiple:setScale(0.1)
    self:addChild(multiple)

    local delay0 = cc.DelayTime:create(0.5)
    local callfun0 = cc.CallFunc:create(function() multiple:setVisible(true) end)
    local scaleTo = cc.ScaleTo:create(0.17,1.0)
    local callfun1 = cc.CallFunc:create(function() multiple:removeFromParent() end)

    local sequence = cc.Sequence:create(delay0,callfun0,scaleTo,callfun1)
    multiple:runAction(sequence)
end

function GameViewLayer:showDQAnim()
  local frame = cc.Sprite:create("game_res/im_gun_frame.png",cc.rect(0,0,188,139))
  frame:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
  frame:setTag(Tag.Tag_GunNum)
  frame:setScale(0.77)
  self:addChild(frame)

  local gamegunNum = GameGunNum:create(self)
  gamegunNum:setPosition(45,44)
  frame:addChild(gamegunNum)
  gamegunNum:setStopIndex(self._scene._drawData.nShootCount)


  -- print("count is =============="..self._scene._drawData.nShootCount)
end

function GameViewLayer:showSeatAnim(nType)
  local redStr = "3d_res/model_5/red.png"
  local greenStr = "3d_res/model_5/green.png"
  local yellowStr = "3d_res/model_5/yellow.png"

  if 0 == nType then
    local color = self._scene._drawData.cbPointerColor
    local file = redStr
    if 1 == color then
      file = greenStr
    elseif 2 == color then
      file = yellowStr  
    end
    local normal = "3d_res/model_5/tex.jpg"
    local action = cc.Sequence:create(
      cc.DelayTime:create(0.37),
      cc.CallFunc:create(function () self._seat:setTexture(file) end),
      cc.DelayTime:create(0.37),
      cc.CallFunc:create(function ()self._seat:setTexture(normal) end)
      )
    self._seat:runAction(cc.RepeatForever:create(action))
  else
      local action =  cc.Sequence:create(
        cc.DelayTime:create(0.37),
        cc.CallFunc:create(function ()  self._seat:setTexture(redStr) end),
        cc.DelayTime:create(0.37),
        cc.CallFunc:create(function () self._seat:setTexture(greenStr) end),
        cc.DelayTime:create(0.37),
        cc.CallFunc:create(function () self._seat:setTexture(yellowStr) end)
        )
      self._seat:runAction(cc.RepeatForever:create(action))
  end
end

function GameViewLayer:gunDeal()
  self._gunIndex = 0
  local index = (g_var(cmd).BET_ITEM_TOTAL_COUNT-self._scene._drawData.nAnimalAngle) + (g_var(cmd).BET_ITEM_TOTAL_COUNT-self._scene._drawData.nPointerAngle)
  index = math.mod(index,g_var(cmd).BET_ITEM_TOTAL_COUNT)
  local animal = self._animals[index+1]
  animal:setTexture("3d_res/im_yellow.png")

  local listIndex = g_var(cmd).BET_ITEM_TOTAL_COUNT-self._scene._drawData.nPointerAngle+1
  if listIndex > g_var(cmd).BET_ITEM_TOTAL_COUNT then
     listIndex = g_var(cmd).BET_ITEM_TOTAL_COUNT
  end
  -- print("listindex 2 is ===================== >"..listIndex)
  --local soundIndex = math.mod(index,4)*3+ self._scene._gameModel._sceneData.cbColorLightIndexList[1][listIndex]
  --self:playEffect(string.format("Animal_%d.wav", soundIndex))
 --游戏定时器
   if nil == self._gunScheduleUpdate then
      local function update(dt)
         self:GunUpdate(dt)
      end 
      self._gunScheduleUpdate = scheduler:scheduleScriptFunc(update, 14.8, false)
   end
end

function GameViewLayer:unSchedule()
    --游戏定时器
    if nil ~= self._gunScheduleUpdate then
        scheduler:unscheduleScriptEntry(self._gunScheduleUpdate)
        self._gunScheduleUpdate = nil
    end
end

function GameViewLayer:GunUpdate( dt )
   if self._gunIndex >= self._scene._drawData.nShootCount then
      self:unSchedule()
      self:showSeatAnim(1)
      self:removeChildByTag(Tag.Tag_GunNum)
      return
   end
   self._scene._drawData.nPointerAngle = math.mod((self._scene._drawData.nPointerAngle + self._scene._drawData.nShootItemAngle[1][self._gunIndex+1]),g_var(cmd).BET_ITEM_TOTAL_COUNT)  
   self._scene._drawIndex = ( g_var(cmd).BET_ITEM_TOTAL_COUNT - self._scene._drawData.nAnimalAngle ) + ( g_var(cmd).BET_ITEM_TOTAL_COUNT - self._scene._drawData.nPointerAngle )
   self._scene._drawIndex  = math.mod(self._scene._drawIndex,g_var(cmd).BET_ITEM_TOTAL_COUNT) + 1
   local stopAngle = math.mod((self._scene._drawData.nPointerAngle*15+180 ),360)
   stopAngle = stopAngle + 360*12
   local action = cc.EaseCircleActionInOut:create(cc.RotateTo:create(5, cc.vec3(0,stopAngle,0)))
   self._arrow:runAction(action)
   self._alphaSprite:runAction(action:clone())
   self._seat:runAction(action:clone())

   self:runAction(cc.Sequence:create(cc.DelayTime:create(14),cc.CallFunc:create(function (  )
        local Animal = self._animals[self._scene._drawIndex]
        Animal:setTexture("3d_res/im_yellow.png")
        local listIndex = g_var(cmd).BET_ITEM_TOTAL_COUNT-self._scene._drawData.nPointerAngle+1
        if listIndex > g_var(cmd).BET_ITEM_TOTAL_COUNT then
          listIndex = g_var(cmd).BET_ITEM_TOTAL_COUNT
        end
        -- print("listindex is ==================>"..listIndex)
        --local soundIndex = math.mod(self._scene._drawIndex-1,4)*3 + self._scene._gameModel._sceneData.cbColorLightIndexList[1][listIndex]

        --self:playEffect(string.format("Animal_%d.wav", soundIndex))
   end)))

   self._scene:SetGameClock(0,0,self._scene._gameModel._sceneData.cbDrawTimeCount)
   self._gunIndex = self._gunIndex + 1
end

function GameViewLayer:gameDataInit(  )
    self._bMusic = true

    AudioEngine.stopMusic()
    --搜索路径
    local gameList = self:getParentNode():getParentNode():getApp()._gameList;
    local gameInfo = {};
    for k,v in pairs(gameList) do
        if tonumber(v._KindID) == tonumber(g_var(cmd).KIND_ID) then
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
  AudioEngine.pauseAllEffects()
  ExternalFun.playPlazzBackgroudAudio()

  self:unLoadRes()

  --重置搜索路径
  local oldPaths = cc.FileUtils:getInstance():getSearchPaths();
  local newPaths = {};
  for k,v in pairs(oldPaths) do
    if tostring(v) ~= tostring(self._searchPath) then
      table.insert(newPaths, v);
    end
  end
  cc.FileUtils:getInstance():setSearchPaths(newPaths);
end

function GameViewLayer:playEffect( file )
  if not GlobalUserItem.bSoundAble then
    return
  end
  file = "sound_res/"..file

  AudioEngine.playEffect(file)
end

function GameViewLayer:initCsbRes()

    --菜单层
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game_res/Top.csb",self)
    self._rootNode = csbNode
    --隐藏路单
      for i=1,5 do
       local winColor = self._rootNode:getChildByName(string.format("recordbg_%d", i)) 
       winColor:setVisible(false)
      end
    --下注层
    self:initPlaceJettonLayer()

    --派彩层
    self:initRewardLayer()

    self:initButtonEvent()
end

function GameViewLayer:initGame()

end
--初始化中奖层 中奖结果
function GameViewLayer:initRewardLayer()
   --加载CSB
  local csbnode = cc.CSLoader:createNode("game_res/GameEnd.csb");
  csbnode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  self._rewardLayer = csbnode
  self:addChild(csbnode,GameViewLayer.TopZorder)

  self._rewardLayer:setVisible(false)

end
--初始化下注层
function GameViewLayer:initPlaceJettonLayer()
  self._posStyle = g_var(cmd).NormalPos
  self._PlaceJettonLayer =  ccui.ImageView:create()
  self._PlaceJettonLayer:setContentSize(cc.size(yl.WIDTH, yl.HEIGHT))
  self._PlaceJettonLayer:setScale9Enabled(true)
  self._PlaceJettonLayer:setAnchorPoint(cc.p(0.5,0.5))
  self._PlaceJettonLayer:setPosition(yl.WIDTH/2, -yl.HEIGHT)
  self._PlaceJettonLayer:setTouchEnabled(true)
  self:addChild(self._PlaceJettonLayer,GameViewLayer.TopZorder)

  self._PlaceJettonLayer:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
          self:popPlaceJettonLayer(g_var(cmd).bottomHidden)
        end
      end)

  --加载CSB
  local csbnode = cc.CSLoader:createNode("game_res/PlaceJetton.csb");
  csbnode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  self._PlaceJettonLayer:addChild(csbnode)
  self._PlaceJettonLayer.rootNode = csbnode

  local jettonBg = csbnode:getChildByName("Image_1")
  jettonBg:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
          self:popPlaceJettonLayer( g_var(cmd).NormalPos)
        end
      end)

  --初始化按钮事件
  local btns = {}
  for i=1,6 do
    local btn = csbnode:getChildByName(string.format("btn_%d", i))
    btn:setTag(i)
    btn:addTouchEventListener(function(sender,eventType)
       if eventType == ccui.TouchEventType.ended then
          self:setJettonIndex(sender:getTag())
       end
    end)
  end

  --下注区域
  for i=1,g_var(cmd).BET_ITEM_COUNT do
    local bet = self._PlaceJettonLayer.rootNode:getChildByName(string.format("btn_bet_%d", i))
    bet:setTag(i)
    assert(bet)
    bet:addTouchEventListener(function(sender,eventType)
       if eventType == ccui.TouchEventType.ended then
          self:PlaceJettonEvent(sender:getTag())
       end
    end)
  end  

  --续压
  local btn = self._PlaceJettonLayer.rootNode:getChildByName("btn_continue")
  btn:addTouchEventListener(function(sender,eventType)
    if eventType == ccui.TouchEventType.ended then
        sender:setEnabled(false)
        self.bContinueRecord = true     -- 一局只能续压一次
        for i=1,#self._scene.m_lContinueRecord do  
          if self._scene.m_lContinueRecord[i] > 0 then
            --发送加注 i是逻辑索引
            self:continueEvent(i)
          end
        end
    end
  end)
end

function GameViewLayer:initButtonEvent()
  --玩家列表
  local btn = self._rootNode:getChildByName("btn_userlist")
  btn:setTag(Tag.btn_userlist)
  btn:addTouchEventListener(handler(self, self.onEvent))

  btn = self._rootNode:getChildByName("btn_bankerlist")
  btn:setTag(Tag.btn_bankerlist)
  btn:addTouchEventListener(handler(self, self.onEvent))

  btn = self._rootNode:getChildByName("btn_bank")
  btn:setTag(Tag.btn_bank)
  btn:addTouchEventListener(handler(self, self.onEvent))


  btn = self._rootNode:getChildByName("btn_sound")
  btn:setTag(Tag.btn_sound)
  btn:addTouchEventListener(handler(self, self.onEvent))
  if not GlobalUserItem.bVoiceAble and not GlobalUserItem.bSoundAble then 
    btn:loadTextureNormal("anniu8.png",UI_TEX_TYPE_PLIST)
  end

  btn = self._rootNode:getChildByName("btn_backRoom")
  btn:setTag(Tag.btn_backRoom)
  btn:addTouchEventListener(handler(self, self.onEvent))

  btn = self._rootNode:getChildByName("btn_help")
  btn:setTag(Tag.btn_help)
  btn:addTouchEventListener(handler(self, self.onEvent))
end


function GameViewLayer:setGameStatus(status)
-- print("status==="..status)
  if self._scene._gameStatus == status then
    return
  end
  self._scene._gameStatus = status
  self:updateClockType(self._scene._gameStatus-g_var(cmd).IDI_GAME_FREE) -- 设置时间
end

--刷新分数
function GameViewLayer:reFreshScore(useritem)

end

--倒计时结束处理
function GameViewLayer:LogicTimeZero()

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

function GameViewLayer:updateScore(item)
   self:reFreshScore(item)
end

function GameViewLayer:updateBankerScore(score)
  
end

--游戏清除
function GameViewLayer:gameClean()
  self._scene._selfBetItemScore = {0,0,0,0,0,0,0,0,0,0,0,0} 
  if not self._scene._gameModel._sceneData.lBetTotalCount then
      self._scene._gameModel._sceneData.lBetTotalCount = {}
  end
  self._scene._gameModel._sceneData.lBetTotalCount[1] = {0,0,0,0,0,0,0,0,0,0,0,0}
  result_bet = {}
  result_bet.iTotalPlayerJetton = 0
  self:popPlaceJettonLayer(g_var(cmd).hidden)
  self:updateScoreItem() 
end

function GameViewLayer:onEvent(sender,eventType)
  local tag = sender:getTag()
  if eventType == ccui.TouchEventType.ended  then
    if tag == Tag.btn_userlist then
      if self._UserView == nil then
        self._UserView = UserList:create(self._scene._gameModel,self._scene)
        self:addChild(self._UserView,30)
        self._UserView:reloadData()
      else
        self._UserView:setVisible(true)
        self._UserView:popLayer()
        self._UserView:reloadData()
      end
    elseif tag == Tag.btn_bankerlist then
    --申请上庄功能,服务端没有
--       if self._bankerView == nil then
--        self._bankerView = BankerList:create(self._scene._gameModel,self._scene)
--        self:addChild(self._bankerView,30)
--        self._bankerView:reloadData()
--      else
--        self._bankerView:setVisible(true)
--        self._bankerView:popLayer()
--        self._bankerView:reloadData()
--      end
    elseif tag == Tag.btn_bank then
        if self._bankView == nil then
        self._bankView = Bank:create(self._scene._gameModel,self._scene)
        self:addChild(self._bankView,30)
      else
        self._bankView:setVisible(true)
        self._bankView:popLayer()
      end 
    elseif tag == Tag.btn_sound then
      GlobalUserItem.bSoundAble = not GlobalUserItem.bSoundAble
      GlobalUserItem.bVoiceAble = GlobalUserItem.bSoundAble
      if GlobalUserItem.bVoiceAble then
        sender:loadTextureNormal("anniu7.png",UI_TEX_TYPE_PLIST)
      else
        sender:loadTextureNormal("anniu8.png",UI_TEX_TYPE_PLIST)
      end

      GlobalUserItem.setVoiceAble( GlobalUserItem.bVoiceAble)
      GlobalUserItem.setSoundAble(GlobalUserItem.bSoundAble)
    elseif tag == Tag.btn_help then
      --self._scene._scene:popHelpLayer2(g_var(cmd).KIND_ID, 0, yl.ZORDER.Z_HELP_WEBVIEW)
      --self._scene._scene:popHelpLayer2(g_var(cmd).KIND_ID, 0, 30)
   -- self._scene._scene:popHelpLayer(yl.HTTP_URL .. "/Mobile/Introduce.aspx?kindid=614&typeid=3")

    elseif tag == Tag.btn_backRoom then
        self._scene:onQueryExitGame()
		--self:getParentNode():onQueryExitGame()
    end
  end
end

--下注
function GameViewLayer:PlaceJettonEvent(tag)
-- print("tag"..tag)
  if self._scene._curJettonIndex <= 0 or self._scene.m_wBankerUser == self._scene:GetMeUserItem().wChairID  then 
    return
  end

  local itemScore = self._scene._selfBetItemScore[tag]
  local topLimit = self._scene._gameModel._sceneData.lUserItemBetTopLimit
  --topLimit =10000000
  if itemScore >= topLimit then
     return
  end

  if itemScore + self._jettonArray[self._scene._curJettonIndex] > topLimit then
    local index = 1
    if self._scene._curJettonIndex == 1 then
      index = -1
    end
    self:setJettonIndex(index)
    return
  end

  local allScore = 0
  topLimit = self._scene._gameModel._sceneData.lUserTotalBetTopLimit
  --topLimit =10000000
  for i=1,g_var(cmd).BET_ITEM_COUNT do
    allScore = allScore + self._scene._selfBetItemScore[i]
  end

  if allScore >= topLimit then
    return
  end

  if allScore + self._jettonArray[self._scene._curJettonIndex] > topLimit then
      local index = 1
      if self._scene._curJettonIndex == 1 then
        index = -1
      end
      self:setJettonIndex(index)
      return
  end

  local item = self._scene:GetMeUserItem()
  local tmp = item.lScore - self._jettonArray[self._scene._curJettonIndex]

  if tmp < 0 then
      local index = 1
      if self._scene._curJettonIndex == 1 then
        index = -1
      end
      self:setJettonIndex(index)
      return
  end

  item.lScore = tmp
  self._scene._selfBetItemScore[tag] = self._scene._selfBetItemScore[tag] + self._jettonArray[self._scene._curJettonIndex]

  if not self._scene.m_bPlaceRecord then
      self._scene.m_lContinueRecord = {0,0,0,0,0,0,0,0,0,0,0,0}
      self._scene.m_bPlaceRecord = true
      self.bContinueRecord = true
  end

  self._scene.m_lContinueRecord[tag] = self._scene.m_lContinueRecord[tag] + self._jettonArray[self._scene._curJettonIndex]
  -- print("self._scene.m_lContinueRecord[tag]",self._scene.m_lContinueRecord[tag])
  --发送加注
--  local  dataBuffer = CCmd_Data:create(9)
--  local itemIndex = tag-1
--  dataBuffer:pushbyte(itemIndex)
--  dataBuffer:pushscore(self._jettonArray[self._scene._curJettonIndex])
--  self._scene:SendData(g_var(cmd).SUB_C_USER_BET,dataBuffer)
  self:playEffect("BetItem.wav")
  
  --下注
--eColorType_Red		= 0,
--eColorType_Green,
--eColorType_Yellow,

--eAnimalType_Lion	= 0,
--eAnimalType_Panda,
--eAnimalType_Monkey,
  local eAnimalType = 0  --动物
  local eColorType = 0 --颜色
  --动物
  if tag==1 or tag == 2 or tag ==3 then
    eAnimalType = 0
  elseif tag==4 or tag == 5 or tag == 6 then
    eAnimalType = 1
  elseif tag==7 or tag == 8 or tag ==9 then
    eAnimalType = 2
  elseif tag==10 or tag == 11 or tag ==12 then
    eAnimalType = 3
  end
  --颜色
  if tag == 1 or tag == 4 or tag == 7 or tag ==10 then
    eColorType = 0
  elseif tag == 2 or tag == 5 or tag == 8 or tag ==11 then
    eColorType = 1
  elseif tag == 3 or tag == 6 or tag == 9 or tag ==12 then
    eColorType = 2
  end

  local  dataBuffer = CCmd_Data:create(25)
  local eGamble = 0 --3d动物
  local eEnjoyGameInfo = -1 --庄闲和(客户端无，默认不押注)
  local iPlaceJettonScore = self._jettonArray[self._scene._curJettonIndex]  --下注分数
  local isClient = true
  dataBuffer:pushint(eGamble)
  dataBuffer:pushint(eAnimalType)
  dataBuffer:pushint(eColorType)
  dataBuffer:pushint(eEnjoyGameInfo)
  dataBuffer:pushscore(iPlaceJettonScore)
  dataBuffer:pushbool(isClient)
  -- print("eAnimalType=="..eAnimalType,"eColorType=="..eColorType,"iPlaceJettonScore=="..iPlaceJettonScore)
  self._scene:SendData(g_var(cmd).SUB_C_USER_BET,dataBuffer)
end

function GameViewLayer:continueEvent(index)
  if self._scene.m_wBankerUser == self._scene:GetMeUserItem().wChairID then
    return
  end

  local itemScore = self._scene.m_lContinueRecord[index]
  local topLimit = self._scene._gameModel._sceneData.lUserItemBetTopLimit
  if itemScore >= topLimit then
     return
  end

  local allScore = 0
  topLimit = self._scene._gameModel._sceneData.lUserTotalBetTopLimit
  for i=1,g_var(cmd).BET_ITEM_COUNT do
    allScore = allScore + self._scene.m_lContinueRecord[i]
  end

  if allScore >= topLimit then
    return
  end

  local item = self._scene:GetMeUserItem()
  local tmp = item.lScore - self._scene.m_lContinueRecord[index]
 
  if tmp < 0 then
    return
  end

  item.lScore = tmp
  self._scene._selfBetItemScore[index] = self._scene._selfBetItemScore[index]+self._scene.m_lContinueRecord[index]

  --发送加注
--  local  dataBuffer = CCmd_Data:create(9)
--  local itemIndex = index-1
--  dataBuffer:pushbyte(itemIndex)
--  dataBuffer:pushscore(self._scene.m_lContinueRecord[index])
--  self._scene:SendData(g_var(cmd).SUB_C_USER_BET,dataBuffer)
  local eAnimalType = 0  --动物
  local eColorType = 0 --颜色
  tag = index
  --动物
  if tag==1 or tag == 2 or tag ==3 then
    eAnimalType = 0
  elseif tag==4 or tag == 5 or tag == 6 then
    eAnimalType = 1
  elseif tag==7 or tag == 8 or tag ==9 then
    eAnimalType = 2
  elseif tag==10 or tag == 11 or tag ==12 then
    eAnimalType = 3
  end
  --颜色
  if tag == 1 or tag == 4 or tag == 7 or tag ==10 then
    eColorType = 0
  elseif tag == 2 or tag == 5 or tag == 8 or tag ==11 then
    eColorType = 1
  elseif tag == 3 or tag == 6 or tag == 9 or tag ==12 then
    eColorType = 2
  end

  local  dataBuffer = CCmd_Data:create(25)
  local eGamble = 0 --3d动物
  local eEnjoyGameInfo = -1 --庄闲和(客户端无，默认不押注)
  local iPlaceJettonScore = self._scene.m_lContinueRecord[index]  --下注分数
  local isClient = true
  dataBuffer:pushint(eGamble)
  dataBuffer:pushint(eAnimalType)
  dataBuffer:pushint(eColorType)
  dataBuffer:pushint(eEnjoyGameInfo)
  dataBuffer:pushscore(iPlaceJettonScore)
  dataBuffer:pushbool(isClient)
  -- print("eAnimalType=="..eAnimalType,"eColorType=="..eColorType,"iPlaceJettonScore=="..iPlaceJettonScore)
  self._scene:SendData(g_var(cmd).SUB_C_USER_BET,dataBuffer)
end

--返回
--function GameViewLayer:backRoom()
--    self._scene:onExitTable()
--end

--设置倒计时
function GameViewLayer:setClockView(chair,id,time)
print("time===3==="..time)
  if time and 0 ~= time then
     self._scene._clockTimeLeave = time
     local timeNode = self._rootNode:getChildByName("node_time")

     --时间
     local timeStr = string.format("%d", time)
     local timeNum = self._rootNode:getChildByTag(Tag.clock_num)
     if not timeNum then 
       timeNum = cc.LabelAtlas:_create(timeStr,"game_res/shuzi4.png",27,38,string.byte("0"))
       timeNum:setTag(Tag.clock_num)
       timeNum:setAnchorPoint(cc.p(0.5,0.5))
       timeNum:setPosition(cc.p(timeNode:getPositionX(),timeNode:getPositionY()))
       self._rootNode:addChild(timeNum)
     else
       local timestr = string.format("%d",time)
       if time < 10 then
         timestr = "0"..timestr
       end
       timeNum:setString(timestr)
     end
  end
end

--更新分数
function GameViewLayer:updateScoreItem()
 --刷新玩家自己分数
 local myScore = self._PlaceJettonLayer.rootNode._myScore
 local item = self._scene:GetMeUserItem()
 if not myScore then
   local str =  ExternalFun.formatScoreText(item.lScore)
   myScore = g_var(ClipText):createClipText(cc.size(140, 22),str,"fonts/round_body.ttf",20)
   myScore:setTextColor(cc.YELLOW)
   myScore:setAnchorPoint(cc.p(0.0,0.5))
   myScore:setPosition(cc.p(260,262))
   self._PlaceJettonLayer.rootNode:addChild(myScore)
   self._PlaceJettonLayer.rootNode._myScore = myScore
 else
    local str =  ExternalFun.formatScoreText(item.lScore)
    myScore:setString(str)
 end
 --自己分数
  for i=1,g_var(cmd).BET_ITEM_COUNT do
   local betPanel = self._PlaceJettonLayer.rootNode:getChildByName(string.format("Panel_%d", i))
   local userScore = betPanel:getChildByTag(2)  --我的下注
   local userNum = self._scene._selfBetItemScore[i]
	if not userScore then
		local str = ExternalFun.formatScoreText(userNum)
		userScore = g_var(ClipText):createClipText(cc.size(90, 22),str,"fonts/round_body.ttf",18)
		userScore:setTag(2)
		userScore:setAnchorPoint(cc.p(0.0,0.5))
		userScore:setPosition(cc.p(46,63))
		betPanel:addChild(userScore)
	elseif userScore then
		local str = ExternalFun.formatScoreText(userNum)
		userScore:setString(str)
	end
 end
 --区域分数
	if result_bet and result_bet.iPlaceJettonScore then
        local eAnimalType = result_bet.STAnimalInfo.eAnimalType --下注动物
        local eColorType = result_bet.STAnimalInfo.eColorType --下注颜色
        if eAnimalType == 0 then
	        if eColorType == 0 then
		        index = 1
	        elseif eColorType == 1 then
		        index = 2
	        elseif eColorType == 2 then
		        index = 3
	        end
        elseif eAnimalType == 1 then
	        if eColorType == 0 then
		        index = 4
	        elseif eColorType == 1 then
		        index = 5
	        elseif eColorType == 2 then
		        index = 6
	        else 
	        end
        elseif eAnimalType == 2 then
	        if eColorType == 0 then
		        index = 7
	        elseif eColorType == 1 then
		        index = 8
	        elseif eColorType == 2 then
		        index = 9
	        end
        elseif eAnimalType == 3 then
	        if eColorType == 0 then
		        index = 10
	        elseif eColorType == 1 then
		        index = 11
	        elseif eColorType == 2 then
		        index = 12
	        end
        end
	end	
    for i=1,g_var(cmd).BET_ITEM_COUNT do
        if index and index == i then
            if not self._scene._gameModel._sceneData.lBetTotalCount then
                self._scene._gameModel._sceneData.lBetTotalCount = {}
            end
            table.remove(self._scene._gameModel._sceneData.lBetTotalCount[1],i)
            table.insert(self._scene._gameModel._sceneData.lBetTotalCount[1],i,result_bet.iTotalPlayerJetton)
        end
    end
    for i=1,g_var(cmd).BET_ITEM_COUNT do
        local betPanel = self._PlaceJettonLayer.rootNode:getChildByName(string.format("Panel_%d", i))
        local totalScore = betPanel:getChildByTag(1) --总注额
        if not self._scene._gameModel._sceneData.lBetTotalCount then
            self._scene._gameModel._sceneData.lBetTotalCount[1] = {}
        end
        local totalNum = self._scene._gameModel._sceneData.lBetTotalCount[1][i]
        if not totalScore then 
            local str =  ExternalFun.formatScoreText(totalNum)
            totalScore = g_var(ClipText):createClipText(cc.size(90, 22),str,"fonts/round_body.ttf",18)
            totalScore:setTag(1)
            totalScore:setAnchorPoint(cc.p(0.0,0.5))
            totalScore:setPosition(cc.p(46,19))
            betPanel:addChild(totalScore)
        elseif  totalScore then
            local str =  ExternalFun.formatScoreText(totalNum)
            totalScore:setString(str)
        end
    end
end


--更新区域赔率
function GameViewLayer:updateBetItemRatio()
  for i=1,g_var(cmd).BET_ITEM_COUNT do
    local bet = self._PlaceJettonLayer.rootNode:getChildByName(string.format("btn_bet_%d", i))
    assert(bet)
    --local multiple = self._scene._gameModel._sceneData.cbBetItemRatioList[1][i]
    local multiple = result_start.STAnimalAtt[1][i].dwMul
    local multipleTTF = bet:getChildByTag(1)
    if not multipleTTF then
      multipleTTF = cc.LabelAtlas:_create(string.format("%d",multiple),"game_res/shuzi2.png",17,26,string.byte("0"))
      multipleTTF:setTag(1)
      multipleTTF:setAnchorPoint(cc.p(0.5,0.0))
      multipleTTF:setPosition(cc.p(72,25))
      bet:addChild(multipleTTF)
    else
      multipleTTF:setString(string.format("%d",multiple))
    end
  end
end

--彩池
function GameViewLayer:updateCaijin()
 if not self._caijinTTF  then
   self._caijinTTF = cc.LabelAtlas:_create(timeStr,"game_res/shuzi1.png",18,25,string.byte("0"))
   self._caijinTTF:setAnchorPoint(cc.p(0.5,0.5))
   self._caijinTTF:setPosition(cc.p(380,710))
   self._rootNode:addChild(self._caijinTTF)
 end

 if self._scene._caijinStatus == 1 then
    local count = math.random(1,3) + 4
    local value = 1
    for i=1,count do
      value = value + math.random(1,10)*math.pow(10, i)
    end
    self._caijinTTF:setString(string.format("%d", value))
 elseif self._scene._caijinStatus == 0 then
    local lPayOutTotalCount = 0
    if nil ~= self._scene._drawData then
      lPayOutTotalCount = self._scene._drawData.lPayOutTotalCount
    end
    --%d, %i - 接受一个数字并将其转化为有符号的整数格式
    lPayOutTotalCount = 0 --服务端无返回值
    self._caijinTTF:setString(string.format("%d", lPayOutTotalCount)) 
 end
end

--刷新倒计时
function GameViewLayer:UpdataClockTime(chair,time)
  if time < 0 then
    return
  end

  if nil ~= self._scene._clockTimeLeave then
      self._scene._clockTimeLeave = self._scene._clockTimeLeave - 1
  end

  if time <= 5 then
  
  end

  if time == 0 then
      self._scene:KillGameClock()
      self:LogicTimeZero()

      self._scene._clockTimeLeave = 0
  end

  local timeNode = self._rootNode:getChildByName("node_time")
  local timeNum = self._rootNode:getChildByTag(Tag.clock_num)
  local timestr = string.format("%d",self._scene._clockTimeLeave)
  if self._scene._clockTimeLeave < 10 then
     timestr = "0"..timestr
   end
  timeNum:setString(timestr)
end

--刷新时间类型
function GameViewLayer:updateClockType(clockType)
  if not self._rootNode then
    return
  end
  --cbTimeLeave
  local clockTypeIMG = self._rootNode:getChildByName("time_type")
  if 0 == clockType then --空闲
  print("clockType","0")   
    self._scene:SetGameClock(0,0,result_Free.cbTimeLeave) -- 设置计时器
    clockTypeIMG:loadTexture("biaoti7.png",UI_TEX_TYPE_PLIST)
  elseif 1 == clockType then       
  print("clockType","1")  
  print("clockType--cb",self._scene._gameModel._sceneData.cbBetTimeCount)  
  print("clockType--start",result_start.cbTimeLeave)                                                                
    --self._scene:SetGameClock(0,0,self._scene._gameModel._sceneData.cbBetTimeCount) --下注
    self._scene:SetGameClock(0,0,result_start.cbTimeLeave or self._scene._gameModel._sceneData.cbBetTimeCount) --下注
    --local tempTime = 30
    --self._scene:SetGameClock(0,0,tempTime) --下注
    clockTypeIMG:loadTexture("biaoti6.png",UI_TEX_TYPE_PLIST)
  elseif 2 == clockType then
  print("clockType","2")   
    self._scene:SetGameClock(0,0,self._scene._gameModel._sceneData.cbDrawTimeCount)
    --self._scene:SetGameClock(0,0,result_start.cbTimeLeave)
    clockTypeIMG:loadTexture("biaoti5.png",UI_TEX_TYPE_PLIST)
  elseif 3 == clockType then
  print("clockType","3")   
    self._scene:SetGameClock(0,0,result_start.cbTimeLeave)
    clockTypeIMG:loadTexture("biaoti4.png",UI_TEX_TYPE_PLIST)
  end
end

--显示开奖结果
function GameViewLayer:showDrawResult(result)
  if result.bDrawMGold then
    --播放音效
  end

  local winColor = self._rewardLayer:getChildByName("winType")  
  --local value = bit:_or(result.dwDrawBetItem,result.dwDrawWeaveItem)   

    eColorType = result_end.stWinAnimal.STAnimalInfo.eColorType
    eAnimalType = result_end.stWinAnimal.STAnimalInfo.eAnimalType
    self:getResultInfo(eColorType,eAnimalType,winColor) 

  --自己下注
  local allScore = 0
  for i=1,g_var(cmd).BET_ITEM_COUNT do
   allScore = allScore + self._scene._selfBetItemScore[i]
  end
  --dump(self._scene._selfBetItemScore, "the add score is =============== >", 5)
  if not self._MeAddScoreTTF then
    self._MeAddScoreTTF = cc.LabelAtlas:_create(string.format("%d",allScore),"game_res/shuzi1.png",18,25,string.byte("0"))
    self._MeAddScoreTTF:setAnchorPoint(cc.p(0.0,0.5))
    self._MeAddScoreTTF:setPosition(cc.p(0,-162))
    self._rewardLayer:addChild(self._MeAddScoreTTF)
  else
    self._MeAddScoreTTF:setString(string.format("%d",allScore))
  end
  
  --输赢
  --local getScore = result.lGameScore
  local getScore = result.iUserScore
  if not self._getAddScoreTTF then
    local str = string.format("%d",getScore)
    if getScore < 0 then
      str = string.format(";%d",getScore)
    end
    self._getAddScoreTTF = cc.LabelAtlas:_create(str,"game_res/shuzi1.png",18,25,string.byte("0"))
    self._getAddScoreTTF:setAnchorPoint(cc.p(0.0,0.5))
    self._getAddScoreTTF:setPosition(cc.p(0,-258))
    self._rewardLayer:addChild(self._getAddScoreTTF)
  else
    local str = string.format("%d",getScore)
    if getScore < 0 then
      str = string.format(";%d",getScore)
    end
    self._getAddScoreTTF:setString(str)
  end

  self._scene.m_lMeGrade =  self._scene.m_lMeGrade + getScore
  
  --更新用户列表自己信息
  if self._UserView then
    local updateInfo = {}
    local userItem  = self._scene:GetMeUserItem()
    updateInfo.nick = userItem.szNickName
    updateInfo.score= userItem.lScore + getScore
    updateInfo.win  = self._scene.m_lMeGrade
    updateInfo.item = userItem 

    self._UserView:setCurUserInfo(updateInfo)
  end

  --庄家信息
  if self._bankerView then
    self._scene.m_lBankerGrade = self._scene.m_lBankerGrade + getScore
    self._bankerView:reloadData()
  end
end
--显示结果(new)
function GameViewLayer:getResultInfo(color,eAnimalType,target)

  if color == 0 then
    color = g_var(cmd).RESULT_RED
  elseif color == 1 then
    color = g_var(cmd).RESULT_GREEN
  elseif color == 2 then
    color = g_var(cmd).RESULT_YELLOW
  end

  target:loadTexture(string.format("tubiao%d.png",color),UI_TEX_TYPE_PLIST) 

  local winAnimal = target:getChildByName("win_animal")
  --29 狮子 30 熊猫 31 猴子 32 兔子
  winAnimal:loadTexture(string.format("tubiao%d.png", 29+eAnimalType),UI_TEX_TYPE_PLIST)
  winAnimal:setVisible(true)
end

--function GameViewLayer:getResultInfo(color,target)
----    local color = bit:_and(routeData,g_var(cmd).BET_ITEM_TYPE_MASK)
----    if 0 ~= bit:_and(routeData,g_var(cmd).DRAW_RESULT_DSY) then
----        color = g_var(cmd).RESULT_DSY
----    end

--  if color == 0 then
--    color = g_var(cmd).RESULT_RED
--  elseif color == 1 then
--    color = g_var(cmd).RESULT_GREEN
--  elseif color == 2 then
--    color = g_var(cmd).RESULT_YELLOW
--  end

-- -- -- print("the color is =============="..color)
--  target:loadTexture(string.format("tubiao%d.png",color),UI_TEX_TYPE_PLIST) 

--  local animalType = {g_var(cmd).DRAW_RESULT_LION,g_var(cmd).DRAW_RESULT_PANDER,g_var(cmd).DRAW_RESULT_MONKEY,g_var(cmd).DRAW_RESULT_RABBIT}
--  local winAnimal = target:getChildByName("win_animal")
--  winAnimal:setTag(1)
--  --29 狮子 30 熊猫 31 猴子 32 兔子
--  winAnimal:loadTexture(string.format("tubiao%d.png", 28+i),UI_TEX_TYPE_PLIST)
--  winAnimal:setVisible(true)

--  for i=2,6 do
--     target:removeChildByTag(i)
--  end


----  --大三元
----  if 0 ~= bit:_and(routeData,g_var(cmd).DRAW_RESULT_DSY) then
----    for i=1,g_var(cmd).BET_ITEM_KIND do
----      if bit:_and(routeData,animalType[i]) ~= 0 then
----         winAnimal:loadTexture(string.format("tubiao%d.png", 28+i),UI_TEX_TYPE_PLIST)
----         break
----      end
----    end
----    return  target
----  end

----  --大四喜
----  if 0 ~= bit:_and(routeData,g_var(cmd).DRAW_RESULT_DSX) then
----    winAnimal:loadTexture("tubiao33.png",UI_TEX_TYPE_PLIST) 
----    return target
----  end

----  --闪电
----  if 0 ~= bit:_and(routeData,g_var(cmd).DRAW_RESULT_LEVIN) then
----     local index = 35
----     if 0 ~=  bit:_and(routeData,g_var(cmd).LEVIN_RATIO_MULTI3) then
----       index = 36
----     end
----      for i=1,g_var(cmd).BET_ITEM_KIND do
----          if bit:_and(routeData,animalType[i]) ~= 0 then
----               winAnimal:loadTexture(string.format("tubiao%d.png", 28+i),UI_TEX_TYPE_PLIST)
----               local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("tubiao%d.png", index))
----               local light = cc.Sprite:createWithSpriteFrame(frame)
----               light:setPosition(cc.p(target:getContentSize().width/2,target:getContentSize().height/2))
----               light:setTag(2)
----               target:addChild(light)
----               break
----          end
----      end 
----      return  target
----  end

----  --打枪
----  if 0 ~= bit:_and(routeData,g_var(cmd).DRAW_RESULT_GUN) then
----     winAnimal:setVisible(false)
----     local shootnum = bit:_and(routeData,g_var(cmd).SHOOT_COUNT_MASK)
----     shootnum =  bit:_rshift(shootnum,8)
----     local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("tubiao34.png")
----     local gun = cc.Sprite:createWithSpriteFrame(frame)
----     gun:setPosition(cc.p(target:getContentSize().width/2,target:getContentSize().height/2))
----     gun:setTag(3)
----     target:addChild(gun)

----     local shootNumTTF = cc.LabelAtlas:_create(string.format("%d",shootnum ),"game_res/shuzi3.png",20,26,string.byte("0"))
----     shootNumTTF:setPosition(cc.p(target:getContentSize().width/2+20,target:getContentSize().height/2))
----     shootNumTTF:setTag(4)
----     target:addChild(shootNumTTF)
----     return target
----  end

----  for i=1,g_var(cmd).BET_ITEM_KIND do
----    if bit:_and(routeData,animalType[i]) ~= 0 then
----       winAnimal:loadTexture(string.format("tubiao%d.png", 28+i),UI_TEX_TYPE_PLIST)
----       if bit:_and(routeData,g_var(cmd).DRAW_RESULT_GOLD) ~= 0 then 
----          local guan = cc.Sprite:create("game_res/im_guan.png")
----          guan:setPosition(cc.p(target:getContentSize().width/2,target:getContentSize().height/2))
----          guan:setTag(5)
----          target:addChild(guan)
----       end
----       break
----    end
----  end

--  return target
--end

--显示路单记录
function GameViewLayer:showRouteRecord(record)
  if not record then
    --record = self._scene._gameModel._sceneData.dwRouteListData[1]
    return
  end
  dump(tempData,"tempData",3)
  if tempData and tempData[1] ~= "" then
    if #tempData >=4 then
        record = tempData
        tempData = {}
    end
  end
  local resultTotal = #record
  local offSet = 0
  for i=1,5 do
    local oneRecord = record[resultTotal-offSet]
    if not oneRecord then
      return
    end
    local winColor = self._rootNode:getChildByName(string.format("recordbg_%d", i)) 
    eColorType = oneRecord.eColorType
    eAnimalType = oneRecord.eAnimalType
    self:getResultInfo(eColorType,eAnimalType,winColor) 
    winColor:setVisible(true)
    offSet = offSet + 1
  end
end

--插入路单记录
function GameViewLayer:insertRecord(STAnimalInfo)
--  table.insert(self._scene._gameModel._sceneData.dwRouteListData[1], oneRecord)
--  dump(self._scene._gameModel._sceneData.dwRouteListData[1])
--  self:showRouteRecord(self._scene._gameModel._sceneData.dwRouteListData[1])
    if not recordListData then
        recordListData = {}
    end
--    recoredData.eColorType = result_end.stWinAnimal.STAnimalInfo.eColorType
--  recoredData.eColorType =result_end.stWinAnimal.STAnimalInfo.eAnimalType
    table.insert(recordListData, STAnimalInfo)
    --dump(recordListData,"recordListData",3)


     self:showRouteRecord(recordListData)
end


function GameViewLayer:onTouchBegan(touch, event)
  return true 
end

function GameViewLayer:onTouchMoved(touch, event)

end

function GameViewLayer:onTouchEnded(touch, event )

end
function prizeTurntoIndex(eAnimalType,eColorType)
    if eAnimalType == 0 then
	    if eColorType == 0 then
		    index = 1
	    elseif eColorType == 1 then
		    index = 2
	    elseif eColorType == 2 then
		    index = 3
	    end
    elseif eAnimalType == 1 then
	    if eColorType == 0 then
		    index = 4
	    elseif eColorType == 1 then
		    index = 5
	    elseif eColorType == 2 then
		    index = 6
	    else 
	    end
    elseif eAnimalType == 2 then
	    if eColorType == 0 then
		    index = 7
	    elseif eColorType == 1 then
		    index = 8
	    elseif eColorType == 2 then
		    index = 9
	    end
    elseif eAnimalType == 3 then
	    if eColorType == 0 then
		    index = 10
	    elseif eColorType == 1 then
		    index = 11
	    elseif eColorType == 2 then
		    index = 12
	    end
    end
    return index
end
return GameViewLayer