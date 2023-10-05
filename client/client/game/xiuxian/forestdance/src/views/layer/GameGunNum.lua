local GameGunNum = class("GameGunNum", cc.ClippingRectangleNode)
local scheduler = cc.Director:getInstance():getScheduler()
function GameGunNum:ctor(scene)
    self._scene = scene
    self:setClippingRegion(cc.rect(0,0,100,50))
    self._bg = cc.Node:create()
    self._bg:setAnchorPoint(cc.p(0.5,0))
    self._bg:setPosition(0,0)
    self:addChild(self._bg)

    self._runNum0 = cc.Sprite:create("game_res/im_gun_num.png")
    self._runNum0:setAnchorPoint(0.5,0)
    self._runNum0:setPosition(50,0)
    self._bg:addChild(self._runNum0)

    self._runNum1 = cc.Sprite:create("game_res/im_gun_num.png")
    self._runNum1:setAnchorPoint(0.5,0)
    self._runNum1:setPosition(50,504)
    self._bg:addChild(self._runNum1)

    self._nums = {self._runNum0,self._runNum1}

    local function onNodeEvent( event )
        if event == "exit" and nil ~= self.onExit then
          self:onExit()
        end
    end

   self:registerScriptHandler(onNodeEvent)
end

function GameGunNum:onExit()
      if nil ~= self.m_scheduleUpdate then
        scheduler:unscheduleScriptEntry(self.m_scheduleUpdate)
        self.m_scheduleUpdate = nil
    end
end

function GameGunNum:setStopIndex( index )
  self._moveIndex = 0
  local stopIndex = 9 - index
  local moveDistance = 504*12+stopIndex*50.4
  self._bg:runAction(cc.EaseInOut:create(cc.MoveBy:create(14, cc.p(0,-moveDistance)),2))
  
   if nil == self.m_scheduleUpdate then
      local function update(dt)
         self:MoveUpdate(dt)
      end 
      self.m_scheduleUpdate = scheduler:scheduleScriptFunc(update, 0.02, false)
   end
end

function GameGunNum:MoveUpdate( dt )
    local pos = cc.p(self._bg:getPositionX(),self._bg:getPositionY())
    local posy = math.abs(pos.y)
    local dealIndex = math.floor(posy/504)
    if dealIndex ~= self._moveIndex then
       self._moveIndex = dealIndex
       local value = math.mod(self._moveIndex+1,2)
       local value1 = math.mod(self._moveIndex,2)
       local _posy
       if value1 == 0 then
          _posy = self._runNum0:getPositionY()
       elseif value1 == 1 then
          _posy = self._runNum1:getPositionY()
       end
       self._nums[value+1]:setPosition(50,_posy+504)
    end
end

return GameGunNum