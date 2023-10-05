local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = "game.yule.line.src.models.CMD_Game"
local ListStatus = 
{
    "RollNormall_Status",       --正常滚动中
    "SlowDown_Status",          --减速中
    "Stop_Status"               --静止状态
}
local LIST_STATE = ExternalFun.declarEnumWithTable(0, ListStatus);


local GameRollList = class("GameRollList",cc.ClippingRectangleNode)


--GameRollList.m_RollLayout = {}       --滚动层

function GameRollList:ctor(gameLayer)
    self._scene=gameLayer
    self.m_listdata = {}
    self.m_RollLayout = {}
    self.ListWidth      = 150    --滚动宽度
    self.ListHeight     = 538    --滚动高度

    self.ListSlowTimes  = 45     --减速帧数
    self.MoveSpeed      = 40     --滚动速度
    self.trusteeMoveSpeed      = 40     --托管时滚动速度
    self.MinSpeed       = 10
    self.trusteeMinSpeed =10

    self.m_Stopindex = nil 

    self.m_slowdown_distant = 0  --减速动作滑动距离

    self.m_scheduleUpdate = nil

    self.m_soundindex = 0

    self:initNotice()
    self:setListType()

end

function GameRollList:initNotice()
    self:setClippingRegion(cc.rect(0,0,self.ListWidth,self.ListHeight))
    self:setClippingEnabled(true)
end

function GameRollList:setListType()
    for i=1,2 do
       self.m_RollLayout[i] = ccui.Layout:create()
       self.m_RollLayout[i]:setContentSize(cc.size(self.ListWidth,self.ListHeight))
       self.m_RollLayout[i]:setAnchorPoint(0.5,0)
       self.m_RollLayout[i]:setPosition(cc.p(self.ListWidth/2, 0))
       if i == 1 then
          self.m_RollLayout[i]:setPosition(cc.p(self.ListWidth/2, self.ListHeight));
       end

       self:initImage(self.m_RollLayout[i], true);
       self:addChild(self.m_RollLayout[i])
    end

end
--初始化item
function GameRollList:initImage( playout,rand )
    --dump(playout)
    if playout then
        playout:removeAllChildren()
    else
        return
    end
    for i=1,3 do
        local imagetype = rand and math.random(100)%10 or self.m_listdata[i-1]
        --dump(self.m_listdata)
        local pSprite = cc.Sprite:create(string.format("game/im_element_%d.png",imagetype))
        --print("game/im_element_%d.png",imagetype)

        pSprite:setPosition(cc.p(self.ListWidth/2,84+(i-1)*170))
        playout:addChild(pSprite)
    end

end

function GameRollList:update( time)
    self.m_soundindex = self.m_soundindex + 1
    -- print("self.m_Status",self.m_Status)
    -- print("self.RollNormall_Status",LIST_STATE.RollNormall_Status)
    local bAuto=self._scene.m_bIsAuto
    local moveSpeed=bAuto and self.trusteeMoveSpeed or self.MoveSpeed
    local minSpeed=bAuto and self.trusteeMinSpeed or self.MinSpeed

    if self.m_Status == LIST_STATE.RollNormall_Status then
        for index=1,2 do
            self.m_RollLayout[index]:setPositionY(self.m_RollLayout[index]:getPositionY() - moveSpeed);

            if self.m_RollLayout[index]:getPositionY() < -self.ListHeight then
                local pointy = self.m_RollLayout[index]:getPositionY();
                self.m_RollLayout[index]:setPositionY(self.ListHeight*2+pointy);
                self:initImage(self.m_RollLayout[index], true);
            end

        end
        self.m_soundindex = self.m_soundindex % 9;
        if self.m_soundindex == 0 then --and self.m_stopcall then
            --播放音效
            ExternalFun.playSoundEffect("ElementMove.wav")
        end
        return
    end
    --print("LIST_STATE.SlowDown_Status",LIST_STATE.SlowDown_Status)
    if self.m_Status == LIST_STATE.SlowDown_Status then
        local times = self.ListSlowTimes
        if self.m_slowdown_distant < minSpeed*self.ListSlowTimes then
            times = self.ListSlowTimes/2;
        end

        local movespeedTemp = self.m_slowdown_distant/self.ListSlowTimes;
        if movespeedTemp > moveSpeed then
            movespeedTemp = moveSpeed;
        end
        if movespeedTemp < minSpeed then
            movespeedTemp = minSpeed
        end
        --print("移动速度 %d", movespeedTemp);
        self.m_slowdown_distant = self.m_slowdown_distant - movespeedTemp;

        if self.m_slowdown_distant <= 0 then
            -- print("self.m_Stopindex",self.m_Stopindex)
            -- print("(self.m_Stopindex+1)%3",(self.m_Stopindex+1)%3)
            self.m_RollLayout[self.m_Stopindex]:setPosition(cc.p(self.ListWidth/2, 0));
            self.m_RollLayout[self.m_Stopindex%2+1]:setPosition(cc.p(self.ListWidth/2, self.ListHeight));


            if self.m_scheduleUpdate ~= nil then
                local scheduler = cc.Director:getInstance():getScheduler()
                scheduler:unscheduleScriptEntry(self.m_scheduleUpdate)
                self.m_scheduleUpdate = nil
            end

            for index=1,3 do
                self:initImage(self.m_RollLayout[index], false);
            end
            self.m_Status = LIST_STATE.Stop_Status;

            return;


        end
        for index=1,2 do
            self.m_RollLayout[index]:setPositionY(self.m_RollLayout[index]:getPositionY()-movespeedTemp);
           if self.m_RollLayout[index]:getPositionY() < -self.ListHeight then
                local pointy = self.m_RollLayout[index]:getPositionY();
                self.m_RollLayout[index]:setPositionY(self.ListHeight*2+pointy);
                if index == self.m_Stopindex and self.m_slowdown_distant < self.ListHeight*2+10 then
                    self:initImage(self.m_RollLayout[index], false);
                else
                    self:initImage(self.m_RollLayout[index], true);
                end
            end
            
        end

        self.m_soundindex = self.m_soundindex%18;
        if self.m_soundindex == 0 then--and self.m_stopcall then
            ExternalFun.playSoundEffect("ElementMove.wav")
        end

    end
end

function GameRollList:setStatus( status )
    print("GameRollList setStatus",status)

    self.m_Status = status
    --print("LIST_STATE.RollNormall_Status",LIST_STATE.RollNormall_Status)
    if self.m_Status == LIST_STATE.RollNormall_Status then

        if self._scene.m_bIsAuto then
            -- 发送结束
            self._scene:sendMessageGameStop();
        end

        local function update(dt)
            --print("t",dt)
            self:update(dt)
        end
        if self.m_scheduleUpdate ~= nil then
            local scheduler = cc.Director:getInstance():getScheduler()
            scheduler:unscheduleScriptEntry(self.m_scheduleUpdate)
            self.m_scheduleUpdate = nil
        end
        local scheduler = cc.Director:getInstance():getScheduler()
        self.m_scheduleUpdate = scheduler:scheduleScriptFunc(update, 0, false)
        return
    end
    if self.m_Status == LIST_STATE.SlowDown_Status then
        local rect = cc.rect(0, 0, self.ListWidth, self.ListHeight/2)

        for index=1,3 do
            local  pointx,pointy = self.m_RollLayout[index]:getPosition();
            local pointtop = cc.p(pointx, pointy+self.ListHeight/2);
            -- print("cc.p(pointx,pointy))",pointx,pointy)
            -- print("pointtop)",pointx,pointy+self.ListHeight/2)
            if cc.rectContainsPoint(rect,cc.p(pointx,pointy)) or cc.rectContainsPoint(rect,pointtop) then
                
                self.m_Stopindex =index;
                self.m_slowdown_distant = pointy+self.ListHeight*(self._scene.m_bIsAuto and 2 or 4);
                
                print("停止index——%d, 移动距离——%f __%f", self.m_Stopindex, self.m_slowdown_distant, pointy);
                return;
            end
        end
    end
    if self.m_Status == LIST_STATE.Stop_Status then
        print("LIST_STATE.Stop_Status")
        self:initImage(self.m_RollLayout[1], false);
        self.m_RollLayout[1]:setPosition(cc.p(self.ListWidth/2, 0));
        self.m_RollLayout[2]:setPosition(cc.p(self.ListWidth/2, self.ListHeight));

        if self.m_scheduleUpdate ~= nil then
            --print("self.m_scheduleUpdate",self.m_scheduleUpdate)
            local scheduler = cc.Director:getInstance():getScheduler()
            scheduler:unscheduleScriptEntry(self.m_scheduleUpdate)
            self.m_scheduleUpdate = nil

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
        --print("self.m_scheduleUpdate",self.m_scheduleUpdate)
        local scheduler = cc.Director:getInstance():getScheduler()
        scheduler:unscheduleScriptEntry(self.m_scheduleUpdate)
        self.m_scheduleUpdate = nil

    end
end

return GameRollList
