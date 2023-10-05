local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(appdf.GAME_SRC.."yule.linefruit.src.models.CMD_Game")
local ListStatus = 
{
    "RollNormall_Status",       --正常滚动中
    "SlowDown_Status",          --减速中
    "Stop_Status"               --静止状态
}
local LIST_STATE = ExternalFun.declarEnumWithTable(0, ListStatus);


local GameRollList = class("GameRollList",cc.ClippingRectangleNode)


--GameRollList.m_RollLayout = {}       --滚动层

function GameRollList:ctor(gameView)
    self.gameView = gameView
    self.m_listdata = {}
    self.ListWidth      = 141    --滚动宽度
    self.ListHeight     = 347    --滚动高度

    self.MoveSpeed      = 60     --滚动速度
    self.speed          = 60
    self.speedDown      = 5
    self.MinSpeed       = 50
     
    self.hideItem       = 5      --隐藏的数量

    self.bottomHeight = 58
    self.itemHeight = 115

    self.m_scheduleUpdate = nil

    self:initNotice()
    self:setListType()
end

function GameRollList:initNotice()
    self:setClippingRegion(cc.rect(0,0,self.ListWidth,self.ListHeight))
    self:setClippingEnabled(true)
end

function GameRollList:setListType()
    self.m_RollLayout = ccui.Layout:create()
    self.m_RollLayout:setContentSize(cc.size(self.ListWidth, self.ListHeight))
    self.m_RollLayout:setAnchorPoint(0.5, 0.5)
    self.m_RollLayout:setPosition(cc.p(self.ListWidth / 2, self.ListHeight / 2))
    self:initImage(self.m_RollLayout, true);
    self:addChild(self.m_RollLayout)


--    local compareView =  display.newLayer(cc.c4b(0, 0, 0, 125))
--    self:addChild(compareView)
end

--初始化item
function GameRollList:initImage(playout, rand)
    -- dump(playout)
    if playout then
        playout:removeAllChildren()
    end
    self.gameEnd = false
    if rand then
        self.bollPos = {}
        for i = 1, 3 + self.hideItem do
            local imagetype = math.random(0,100) % 10
            -- dump(self.m_listdata)
            local pSprite = display.newSprite(string.format("#game/im_fruit_%d.png", imagetype))
            -- print("game/im_element_%d.png",imagetype)

            pSprite:setPosition(cc.p(self.ListWidth / 2, self.bottomHeight +(i - 1) * self.itemHeight))
            self.bollPos[i] = pSprite:getPositionY()
            pSprite:setTag(i)
            pSprite:setScale(720/750)
            playout:addChild(pSprite)
        end
    else
        for i = 1, 3 do
            local imagetype = self.m_listdata[i - 1]
            print(imagetype)
             dump(self.m_listdata,"**"..i)
            local pSprite = display.newSprite(string.format("#game/im_fruit_%d.png", imagetype))
            -- print("game/im_element_%d.png",imagetype)

            pSprite:setPosition(cc.p(self.ListWidth / 2, self.bottomHeight +(i - 1) * self.itemHeight))
            pSprite:setTag(i)
            pSprite:setScale(720/750)
            playout:addChild(pSprite)
        end
    end
end

function GameRollList:update(time)
    if self.m_Status == LIST_STATE.RollNormall_Status then
        self.speed = self.MoveSpeed
    elseif self.m_Status == LIST_STATE.SlowDown_Status then
        self.speed = self.speed - self.speedDown
        if self.speed <= self.MinSpeed then
            self.speed = self.MinSpeed
        end
    end
    if self.speed == self.MinSpeed then
        if self.gameEnd then
--        if self.m_RollLayout:getChildByTag(1 + self.hideItem):getPositionY() <= self.bottomHeight + 1 and self.m_RollLayout:getChildByTag(1 + self.hideItem):getPositionY() >= self.bottomHeight then
--            self:setStatus(2,self.stop)
--            return
--        end
        local s = self.speed
        for index = 1, self.m_RollLayout:getChildrenCount() do
            self.bollPos[index] = self.bollPos[index] - s
            self.m_RollLayout:getChildByTag(index):setPositionY(self.bollPos[index])
            if self.m_RollLayout:getChildByTag(index):getPositionY() <= (- self.itemHeight + self.bottomHeight) and index == self.hideItem then
                self:setStatus(2,self.stop)

                for index = 1, self.m_RollLayout:getChildrenCount() do
                    self.m_RollLayout:getChildByTag(index):runAction(cc.Sequence:create(cc.CallFunc:create(function() ExternalFun.playSoundEffect("run_end.wav") end),
                    cc.MoveBy:create(0.2,cc.p(0,-5)),
                    cc.MoveBy:create(0.2,cc.p(0,10)),
                    cc.MoveBy:create(0.2,cc.p(0,-5))
                        )
                    )
                end

                return
            end
        end
        else
        local s = self.speed
        for index = 1, self.m_RollLayout:getChildrenCount() do
            self.bollPos[index] = self.bollPos[index] - s
            self.m_RollLayout:getChildByTag(index):setPositionY(self.bollPos[index])
            if self.m_RollLayout:getChildByTag(index):getPositionY() <= (- self.itemHeight + self.bottomHeight) then
                self.bollPos[index] = 900 + (self.m_RollLayout:getChildByTag(index):getPositionY() + self.itemHeight - self.bottomHeight)
                self.m_RollLayout:getChildByTag(index):setPosition(cc.p(self.ListWidth / 2,self.bollPos[index]))
                if index == 1 then
                    for i = 1, 3 do
                        local imagetype = self.m_listdata[i - 1]
                        self.m_RollLayout:getChildByTag(i + self.hideItem):setSpriteFrame(string.format("game/im_fruit_%d.png", imagetype))
                    end
                    self.gameEnd = true
                end
            end
        end
        end
    else
        local s = self.speed
        for index = 1, self.m_RollLayout:getChildrenCount() do
            self.bollPos[index] = self.bollPos[index] - s
            self.m_RollLayout:getChildByTag(index):setPositionY(self.bollPos[index])
            if self.m_RollLayout:getChildByTag(index):getPositionY() <= (- self.itemHeight + self.bottomHeight) then
                self.bollPos[index] = 900 + (self.m_RollLayout:getChildByTag(index):getPositionY() + self.itemHeight - self.bottomHeight)
                self.m_RollLayout:getChildByTag(index):setPosition(cc.p(self.ListWidth / 2,self.bollPos[index]))
--                local imagetype = math.random(100) % 10
                local imagetype = math.random(0, 11)
                self.m_RollLayout:getChildByTag(index):setSpriteFrame(string.format("game/im_fruit_%d.png", imagetype))
            end
        end
    end
end

function GameRollList:setStatus(status,stop)
    print("GameRollList setStatus", status,stop)
    self.m_Status = status
    self.stop = stop
    local function update(dt)
        -- print("t",dt)
        self:update(dt)
    end
    if self.m_Status == LIST_STATE.Stop_Status then
        if self.m_scheduleUpdate ~= nil then
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.m_scheduleUpdate)
            self.m_scheduleUpdate = nil
            self:initImage(self.m_RollLayout, false)

            if self.gameView.nextNum ~= 5 then
                local eventListener = cc.EventCustom:new("EVENT_NEXT")
	    	    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
            end

            if self.stop then
                self.gameView:changeStartButton(false)
				self.gameView:gameEndScoreDeal()
            end
        end
    else
        if self.m_scheduleUpdate == nil then
            self:initImage(self.m_RollLayout, true);
            self.m_scheduleUpdate = cc.Director:getInstance():getScheduler():scheduleScriptFunc(update, 0, false)
        end
    end
end

function GameRollList:setListData( data1,data2,data3 )
    self.m_listdata[2] = data1;
    self.m_listdata[1] = data2;
    self.m_listdata[0] = data3;
end

function GameRollList:onExit()
    if self.m_scheduleUpdate ~= nil then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.m_scheduleUpdate)
        self.m_scheduleUpdate = nil
    end
end

return GameRollList
