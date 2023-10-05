local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local RewardLayer = class("RewardLayer", cc.Layer)

RewardLayer.BT_CLOSE = 1
RewardLayer.BT_BLANK = 2
RewardLayer.BT_BEGIN = 3
RewardLayer.BT_END = 4

RewardLayer.SPRITE_FRUIT = 100
RewardLayer.SPRITE_BOOM = 101

--构造
function RewardLayer:ctor(rabbitStartInfo)
    dump(rabbitStartInfo,"RewardLayer:ctor")
--    self.allFruitNum = rabbitStartInfo.nSuperRabbit
    self.allFruitNum = 20
    self.fruitNum = 0
    self.fruitTime = 2


    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("RewardLayer/RewardLayer.csb", self)
    self._csbNode:setPosition(0,0)  

    self._csbNode = self._csbNode:getChildByName("Node")

    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(sender:getTag(),sender)
        end
    end
    --背景
    local sp_bg = self._csbNode:getChildByName("Sprite_bg")
    self.m_spBg = sp_bg
    self.fruitNode = cc.Node:create()
        :addTo(self.m_spBg)

    self.rabbit = self._csbNode:getChildByName("Sprite_rabbit")
    self.spriteBasket = self.rabbit:getChildByName("Sprite_basket")

    self.sp_basket = self._csbNode:getChildByName("sp_basket")

    local spriteCountdown = self._csbNode:getChildByName("Sprite_countdown")
    self.artCountdown = spriteCountdown:getChildByName("Art_countdown")

    self.artScore = self._csbNode:getChildByName("Sprite_score"):getChildByName("Art_score")

    self.spriteBlank = self._csbNode:getChildByName("Sprite_blank")
    self.spriteBlank:setVisible(true)

    self.spriteBegin = self._csbNode:getChildByName("Sprite_begin")
    self.spriteBegin:setVisible(true)
    self.btnBegin = self.spriteBegin:getChildByName("Button_begin")
    self.btnBegin:setTag(RewardLayer.BT_BEGIN)
    self.btnBegin:addTouchEventListener(cbtlistener)
    self.artBegin = self.btnBegin:getChildByName("Art_begin")
    self.spriteBegin:runAction(cc.Sequence:create(
        cc.DelayTime:create(1),
        cc.CallFunc:create(function() self.artBegin:setString(4) end),
        cc.DelayTime:create(1),
        cc.CallFunc:create(function() self.artBegin:setString(3) end),
        cc.DelayTime:create(1),
        cc.CallFunc:create(function() self.artBegin:setString(2) end),
        cc.DelayTime:create(1),
        cc.CallFunc:create(function() self.artBegin:setString(1) end),
        cc.DelayTime:create(1),
        cc.CallFunc:create(function() self:onButtonClickedEvent(RewardLayer.BT_BEGIN) end)
    ))

    self.spriteEnd = self._csbNode:getChildByName("Sprite_end")
    self.artEnd = self.spriteEnd:getChildByName("Art_end")
    self.btnEnd = self.spriteEnd:getChildByName("Button_end")
    self.btnEnd:setTag(RewardLayer.BT_END)
    self.btnEnd:addTouchEventListener(cbtlistener)

    self.countDown = 30
    self.score = 0
    self.playing = false
    self:setScale(720/750)

end

function RewardLayer:onButtonClickedEvent(tag, sender)
    if RewardLayer.BT_BEGIN == tag then
        self.playing = true
        self.spriteBegin:stopAllActions()        
        self.spriteBlank:setVisible(false)
        self.spriteBegin:setVisible(false)
        math.randomseed(tonumber(tostring(os.time()):reverse():sub(1, 6)))
        self.catchFruitScheduler = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function() 
            local r = math.random(1, 100)
            print("self.fruitNum",self.fruitNum)
            if r > 50 and self.fruitNum > 0 then
                local a = math.random(0, 10)
                self:catchFruit(a) 
                self.fruitNum = self.fruitNum - 1
            else
                self:catchFruit(11) 
            end
        end, 1 ,false)
        self.countDownScheduler = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function() 
            if self.countDown == 0 then
                self.playing = false
                self:releaseAllScheduler()
                
                self.m_parent._scene.rabbitLayer = self
                self.m_parent._scene:sendMessageRabbitEnd(self.score)
            else
                if self.countDown then
                self.countDown = self.countDown - 1
                self.artCountdown:setString(self.countDown)
                if self.countDown == 10 then
                    self.fruitTime = self.fruitTime / 3 * 2
                    self.fruitNum = math.floor(self.allFruitNum*0.2)
                elseif self.countDown == 20 then
                    self.fruitTime = self.fruitTime / 3 * 2
                    self.fruitNum = math.floor(self.allFruitNum*0.3)
                elseif self.countDown == 29 then
                    self.fruitNum = math.floor(self.allFruitNum*0.5)
                end
                end
            end
        end,1,false)

        self.fruitScheduler = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function() 
            self:fruitUpdate()
        end,0.1,false)

    elseif RewardLayer.BT_END == tag then
        self:showLayer(false)
    end
end

function RewardLayer:showEndResult(score)
    self.spriteBlank:setVisible(true)
    self.spriteEnd:setVisible(true)
    self.artEnd:setString(score)
end

--随机出水果
function RewardLayer:catchFruit(a)
    --    print(math.random(0, 10))
    local imagetype = a
    local fSprite = display.newSprite(string.format("#game/im_fruit_%d.png", imagetype))
    if imagetype == 11 then
        fSprite:setTag(RewardLayer.SPRITE_BOOM)
    else
        fSprite:setTag(RewardLayer.SPRITE_FRUIT)
    end
    fSprite:setPosition(cc.p(math.random(50, 1250), 750))
    self.fruitNode:addChild(fSprite)
    fSprite:runAction(cc.MoveBy:create(self.fruitTime,cc.p(0,-850)))
end

function RewardLayer:showLayer( var )
    self:setVisible(var)
end

--触摸回调
function RewardLayer:onTouchBegan(touch, event)
--    print("RewardLayer:onTouchBegan",self.rabbit)
   -- dump(self.rabbit:getBoundingBox(),"兔子触摸范围--------------------")
--    dump(touch:getLocation())
    if self.spriteBlank:isVisible() then
        return false
    end

    if cc.rectContainsPoint(self.rabbit:getBoundingBox(), touch:getLocation()) then
        self.target = self.rabbit
    else
        self.target = nil
    end
    return true
end

function RewardLayer:onTouchMoved(touch, event)
--    print("RewardLayer:onTouchMoved")
    if not self.playing then
        return true
    end
    local location = touch:getLocation()
    if self.target then
        local target = self.target
        local posX, posY = target:getPosition()
        local delta = touch:getDelta()
        if (posX + delta.x) <= 100 then
            return true
        elseif (posX + delta.x) >= 1140 then
            return true
        end
        target:setPosition(cc.p(posX + delta.x, posY))

        --------------
        local basket = self.sp_basket;
        local posX1, posY1 = basket:getPosition()
        basket:setPosition(cc.p(posX1 + delta.x, posY1))
    end

--     --检测碰撞
--     local t = self.fruitNode:getChildren()
--     for i = 1, #t do
--         -- if cc.rectContainsPoint(self.rabbit:getBoundingBox(), t[i]:getBoundingBox()) then
--         --     self:onCatch(t[i])
--         -- end
--     end
    return true
end

function RewardLayer:onTouchEnded(touch, event)
--    print("RewardLayer:onTouchEnded")
end

function RewardLayer:fruitUpdate()
    if not self.playing then
        return true
    end

    --检测碰撞
    local t = self.fruitNode:getChildren()
    for i = 1, #t do
        if cc.rectIntersectsRect(self.sp_basket:getBoundingBox(), t[i]:getBoundingBox()) then
            self:onCatch(t[i])
        end

        -- if cc.rectIntersectsRect(self.spriteBasket:getBoundingBox(), t[i]:getBoundingBox()) then
        --     self:onCatch(t[i])
        -- end

    end

end

function RewardLayer:onCatch(o)
    local fruit = o
    if fruit:getTag() == RewardLayer.SPRITE_BOOM then
        self.playing = false
        self:releaseAllScheduler()

        self.m_parent._scene.rabbitLayer = self
        self.m_parent._scene:sendMessageRabbitEnd(self.score)
    elseif fruit:getTag() == RewardLayer.SPRITE_FRUIT then
        if self.playing then
            local artScore = cc.LabelAtlas:create("90", "RewardLayer/num_countdown.png", 48, 64, string.byte("/"))
            :move(fruit:getPosition())
            :addTo(self.m_spBg)
            artScore:runAction(cc.Sequence:create(
            cc.MoveBy:create(1, cc.p(0, 100)),
            cc.CallFunc:create( function() artScore:removeFromParent() end))
            )
            self.score = self.score + 1
            self.artScore:setString(self.score)
        end
    end
    fruit:removeFromParent()
end

function RewardLayer:releaseAllScheduler()
    if self.catchFruitScheduler then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.catchFruitScheduler)
        self.catchFruitScheduler = nil
    end
    if self.countDownScheduler then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.countDownScheduler)
        self.countDownScheduler = nil
    end

    if self.fruitScheduler then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.fruitScheduler)
        self.fruitScheduler = nil
    end

end

return RewardLayer