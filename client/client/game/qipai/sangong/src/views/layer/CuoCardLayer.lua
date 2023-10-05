local CuoCardLayer = class("CuoCardLayer", function(scene)
	local CuoCardLayer = display.newLayer()
	return CuoCardLayer
end)
local cmd = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.CMD_Game")

local TAG_BT_EXIT = 1

function CuoCardLayer:onInitData()
end

function CuoCardLayer:onResetData()
end

function CuoCardLayer:ctor(scene)
	self._scene = scene
	self:onInitData()

	self.colorLayer = cc.LayerColor:create(cc.c4b(0, 0, 0, 125))
		:setContentSize(display.width, display.height)
		:addTo(self)
--	self.colorLayer:setTouchEnabled(false)
--	self.colorLayer:registerScriptTouchHandler(function(eventType, x, y)
--		return self:onTouch(eventType, x, y)
--	end)

	local funCallback = function(ref)
		self:onButtonCallback(ref:getTag(), ref)
	end

	--BG
--	self.sp_layerBg = display.newSprite("#sp_setLayerBg_zjh.png")
--		:move(display.center)
--		:addTo(self)

	self.btExit = ccui.Button:create(cmd.GAME.."bt_end_close_0.png",cmd.GAME.."bt_end_close_1.png",cmd.GAME.."bt_end_close_0.png",ccui.TextureResType.plistType)
		:setTag(TAG_BT_EXIT)
		:move(display.width-100, display.height-80)
		:addTo(self)
	self.btExit:addClickEventListener(funCallback)

	self:setVisible(false)
end

function CuoCardLayer:onButtonCallback(tag, ref)
	if tag == TAG_BT_EXIT then
		print("离开")
		self:hideLayer()
        self._scene._scene:onFlipCard()
	end
end

function CuoCardLayer:onTouch(eventType, x, y)
	if eventType == "began" then
		return true
	end

	local pos = cc.p(x, y)
    local rectLayerBg = self.sp_layerBg:getBoundingBox()
    if not cc.rectContainsPoint(rectLayerBg, pos) then
--    	self:hideLayer()
    end

    return true
end

function CuoCardLayer:showLayer(viewid)
	self.colorLayer:setTouchEnabled(true)
	self:setVisible(true)
    local xLeft,yLeft = self._scene.cuoCard[viewid].card[1]:getPosition()
    local xMid,yMid = self._scene.cuoCard[viewid].card[2]:getPosition()
    local xRight,yRight = self._scene.cuoCard[viewid].card[3]:getPosition()
    local right = true
    local left = true
    -- 触摸开始
    local function onTouchBegan(touch, event)
--        print("onTouchBegan")
        if cc.rectContainsPoint(self._scene.cuoCard[viewid].card[3]:getBoundingBox(), touch:getLocation()) then
            self.target = self._scene.cuoCard[viewid].card[3]
        elseif cc.rectContainsPoint(self._scene.cuoCard[viewid].card[2]:getBoundingBox(), touch:getLocation()) then
            self.target = self._scene.cuoCard[viewid].card[2]
        elseif cc.rectContainsPoint(self.btExit:getBoundingBox(), touch:getLocation()) then
            self:hideLayer()
            self._scene._scene:onFlipCard()
        else 
            self.target = nil
        end
        return true
    end

    -- 触摸中
    local function onTouchMove(touch, event)
--        print("onTouchMove")   
        local location = touch:getLocation()
--        local xMoveL,yMoveL = self._scene.cuoCard[viewid].card[1]:getPosition()
--        local xMoveR,yMoveR = self._scene.cuoCard[viewid].card[3]:getPosition()
--        if left and location.x < xLeft and location.x > 0 then
--            if (xLeft - xMoveL) >= 100 then
--                location.x = 100
--                self._scene.cuoCard[viewid].card[1]:move(xLeft - location.x,yLeft)  --牌的间距
--                left = false   
--            elseif (xLeft - xMoveL) < 100 and (xLeft - xMoveL) >= 0  then
--                if location.x >= 10 then
--                    location.x = 1
--                end
--                self._scene.cuoCard[viewid].card[1]:move(xMoveL - location.x,yLeft)  --牌的间距
--            end
--        end
--        if right and location.x > xRight and location.x > 0  then
--            if (xMoveR - xRight) >= 100 then
--                location.x = 100
--                self._scene.cuoCard[viewid].card[3]:move(xRight + location.x,yRight)  --牌的间距
--                right = false   
--            elseif (xMoveR - xRight) < 100 and (xMoveR - xRight) >= 0 then
--                if location.x >= 10 then
--                    location.x = 1
--                end
--                self._scene.cuoCard[viewid].card[3]:move(xMoveR + location.x,yRight)  --牌的间距
--            end
--        end
        if self.target then
            local target = self.target  --获取当前的控件
            local posX,posY = target:getPosition()  --获取当前的位置
            local delta = touch:getDelta() --获取滑动的距离
            target:setPosition(cc.p(posX + delta.x, posY + delta.y)) --给精灵重新设置位置
        end
        return true
    end

    -- 触摸结束
    local function onTouchEnd(touch, event)
        -- 获取点击位置
--        local location = touch:getLocation()
--        print(location.x .. "  "..location.y)
        local close = true
        local checkRect = {height = 93.5,width = 112.5,x = 492,y = 368}
        if cc.rectIntersectsRect(self._scene.cuoCard[viewid].card[3]:getBoundingBox(), checkRect) or cc.rectIntersectsRect(self._scene.cuoCard[viewid].card[2]:getBoundingBox(), checkRect) then
            close = false
        end
        if close then
            if self.eventDispatcher then
                self.eventDispatcher:removeEventListener(self.listener2)
                self.eventDispatcher:removeEventListener(self.listener3)
                self.eventDispatcher = nil
            end
            self:runAction(cc.Sequence:create(
            cc.DelayTime:create(0.1),
            cc.CallFunc:create(function() 
                self._scene.cuoCard[viewid].card[1]:move(xLeft - 100,yLeft)
                self._scene.cuoCard[viewid].card[2]:move(xMid,yMid)
                self._scene.cuoCard[viewid].card[3]:move(xRight + 100,yRight)
            end),
            cc.DelayTime:create(0.1),
            cc.CallFunc:create(function()
                self._scene.cuoCard[viewid].card[1]:runAction(cc.MoveBy:create(0.3,cc.p(100,0)))
                self._scene.cuoCard[viewid].card[3]:runAction(cc.MoveBy:create(0.3,cc.p(-100,0)))
            end),
            cc.DelayTime:create(0.4),
            cc.CallFunc:create(function()
                self._scene.cuoCard[viewid].card[1]:runAction(cc.MoveBy:create(0.3,cc.p(0,-400)))
                self._scene.cuoCard[viewid].card[2]:runAction(cc.MoveBy:create(0.3,cc.p(0,-400)))
                self._scene.cuoCard[viewid].card[3]:runAction(cc.MoveBy:create(0.3,cc.p(0,-400)))
            end),
            cc.DelayTime:create(0.5),
            cc.CallFunc:create(function()
                self:hideLayer()
                self._scene._scene:onFlipCard()
            end)
            )
            )
        end
    end

    self.listener2 = cc.EventListenerTouchOneByOne:create()
    --触摸消息不向下传递,在弹出的界面上点击下层的按钮，就不会响应了
    self.listener2:setSwallowTouches(true)
    --注册两个回调监听方法
    self.listener2:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN)
    self.listener2:registerScriptHandler(onTouchMove,cc.Handler.EVENT_TOUCH_MOVED)
    self.listener2:registerScriptHandler(onTouchEnd,cc.Handler.EVENT_TOUCH_ENDED)

    self.listener3 = cc.EventListenerTouchOneByOne:create()
    --触摸消息不向下传递,在弹出的界面上点击下层的按钮，就不会响应了
    self.listener3:setSwallowTouches(true)
    --注册两个回调监听方法
    self.listener3:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN)
    self.listener3:registerScriptHandler(onTouchMove,cc.Handler.EVENT_TOUCH_MOVED)
    self.listener3:registerScriptHandler(onTouchEnd,cc.Handler.EVENT_TOUCH_ENDED)
    --事件派发器
    self.eventDispatcher = cc.Director:getInstance():getEventDispatcher()
    --绑定触摸事件到层当中
    self.eventDispatcher:addEventListenerWithSceneGraphPriority(self.listener2, self._scene.cuoCard[viewid].card[2])
    self.eventDispatcher:addEventListenerWithSceneGraphPriority(self.listener3, self._scene.cuoCard[viewid].card[3])
    --解绑
--    self.eventDispatcher:removeAllEventListeners()
end

function CuoCardLayer:hideLayer()
    if self.eventDispatcher then
        self.eventDispatcher:removeEventListener(self.listener2)
        self.eventDispatcher:removeEventListener(self.listener3)
        self.eventDispatcher = nil
    end
	self.colorLayer:setTouchEnabled(false)
	self:setVisible(false)
	self:onResetData()
end

return CuoCardLayer