--
-- Author: zhong
-- Date: 2017-03-27 09:21:55
--
-- 录像回放控制
local VideoPlayControlLayer = class("VideoPlayControlLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local QueryDialog = appdf.req(appdf.BASE_SRC.."app.views.layer.other.QueryDialog")

local scheduler = cc.Director:getInstance():getScheduler()
local targetPlatform = cc.Application:getInstance():getTargetPlatform()

local BTN_BACKWARD  = 1001          -- 快退
local BTN_STOP      = 1002          -- 停止
local BTN_FORWARD   = 1003          -- 快进
local BTN_BACK      = 1004          -- 返回 
local BTN_START     = 1005          -- 开始

local PLAY_SPEED    = 0.1           -- 播放速度
local CONTROL_STEP  = 6             -- 快进/退消息数量
function VideoPlayControlLayer:ctor( tabConfig, scene )
    ExternalFun.registerNodeEvent(self)
    self._scene = scene
    -- 进入的游戏
    self.m_tabEnterGame = self._scene:getEnterGameInfo()
    local modulestr = string.gsub(self.m_tabEnterGame._KindName, "%.", "/")
    if cc.PLATFORM_OS_WINDOWS == targetPlatform then
        gameFile = "game/" .. modulestr .. "src/gamevideo/GameVideoReplay.lua" .. yl.WIN_SRC_SUFFIX
    else
        gameFile = "game/" .. modulestr .. "src/gamevideo/GameVideoReplay.lua"
    end
    self.m_gameVideo = nil
    if cc.FileUtils:getInstance():isFileExist(gameFile) then
        self.m_gameVideo = appdf.req(gameFile):create()
    end
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("video/VideoPlayControlLayer.csb", self)

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    -- 遮罩
    local mask = csbNode:getChildByName("mask_panel")
    -- 透明度
    local opacity = tabConfig.maskOpacity or 255
    mask:setBackGroundColorOpacity(opacity)

    -- 控制
    local sp_control = csbNode:getChildByName("sp_videocontrol")
    -- 设置位置
    local pos = tabConfig.pos
    if nil ~= pos then
        sp_control:setPosition(pos)
    end

    -- 快退
    local btn = sp_control:getChildByName("btn_backward")
    btn:setTag(BTN_BACKWARD)
    btn:addTouchEventListener(touchFunC)
    btn:setOpacity(200)

    -- 停止
    btn = sp_control:getChildByName("btn_stop")
    btn:setTag(BTN_STOP)
    btn:addTouchEventListener(touchFunC)
    self.m_btnStart = btn
    btn:setOpacity(200)

    -- 快进
    btn = sp_control:getChildByName("btn_forward")
    btn:setTag(BTN_FORWARD)
    btn:addTouchEventListener(touchFunC)
    btn:setOpacity(200)

    --返回
    btn = sp_control:getChildByName("btn_back")
    btn:setTag(BTN_BACK)
    btn:addTouchEventListener(touchFunC)
    btn:setOpacity(200)

    self.m_scheduler = nil

    -- 速度控制
    self.m_nSpeedStep = 1
    self.m_nSpeed = 10
    -- 是否允许快进、快退
    self.m_bEnableForward = true

    -- 退出弹窗
    self._quitQuery = nil
end

function VideoPlayControlLayer:onButtonClickedEvent(tag, ref)
    if BTN_BACKWARD == tag then
        if not self.m_bEnableForward then
            print("不允许快退")
            return
        end
        -- 停止定时器
        self:stopAllActions()
        self:stopScheduler()
        self:toStartState()

        -- 快退
        local acts = {}
        local leftCount = GameVideo:getInstance():leftVideoMessageCount()
        local backwardCount = CONTROL_STEP --(leftCount < (CONTROL_STEP)) and leftCount or (CONTROL_STEP)
        print("快退数量 ==> ", backwardCount)
        GameVideo:getInstance():backwardMessageIdx(backwardCount)
        self:runAction(cc.Sequence:create(cc.CallFunc:create(function()
            self:popMessageUpdate(false, true)
        end), cc.DelayTime:create(0.5), cc.CallFunc:create(function()
            self:startScheduler()
        end)))
    elseif BTN_STOP == tag then
        -- 停止定时器
        self:stopAllActions()
        self:stopScheduler()
        self.m_btnStart:setTag(BTN_START)
        self.m_btnStart:loadTextureDisabled("video/videoplay_btn_start_0.png")
        self.m_btnStart:loadTextureNormal("video/videoplay_btn_start_0.png")
        self.m_btnStart:loadTexturePressed("video/videoplay_btn_start_1.png")
        --showToast(self, "回放暂停", 1)
    elseif BTN_FORWARD == tag then
        if not self.m_bEnableForward then
            print("不允许快进")
            return
        end
        -- 停止定时器
        self:stopAllActions()
        self:stopScheduler()
        self:toStartState()

        local acts = {}
        local leftCount = GameVideo:getInstance():leftVideoMessageCount()
        local forwardCount = (leftCount < CONTROL_STEP) and leftCount or CONTROL_STEP
        print("快进数量 ==> ", forwardCount)
        for i = 1, forwardCount do
            local call = cc.CallFunc:create(function()
                self:popMessageUpdate(true, false)
            end)
            table.insert(acts, call)
            local dela = cc.DelayTime:create(0.5)
            table.insert(acts, dela)
        end
        local call = cc.CallFunc:create(function()
            print("重新开始")
            self:startScheduler()
        end)
        table.insert(acts, call)
        self:runAction(cc.Sequence:create(acts))
    elseif BTN_BACK == tag then
        if nil ~= self._quitQuery then
            self._quitQuery:removeFromParent()
        end
        self._quitQuery = QueryDialog:create("是否退出回放?",function(ok)
            if true == ok then
                if nil ~= self._scene and nil ~= self._scene.onKeyBack then
                    self:stopAllActions()
                    self:stopScheduler()
                    self.m_nSpeedStep = 1

                    if nil ~= self._scene and nil ~= self._scene.onKeyBack then
                        self._scene:onKeyBack()
                    end
                end
            end
            self._quitQuery = nil
        end)
        :setCanTouchOutside(false)
        :addTo(self)
    elseif BTN_START == tag then
        self:stopAllActions()
        self:startScheduler()
        self:toStartState()
    end
end

function VideoPlayControlLayer:toStartState()
    self.m_nSpeedStep = 1
    self.m_btnStart:setTag(BTN_STOP)
    self.m_btnStart:loadTextureDisabled("video/videoplay_btn_stop_0.png")
    self.m_btnStart:loadTextureNormal("video/videoplay_btn_stop_0.png")
    self.m_btnStart:loadTexturePressed("video/videoplay_btn_stop_1.png")
end

function VideoPlayControlLayer:onExit()
	self.m_nSpeedStep = 1
    self:stopAllActions()
    self:stopScheduler()
end

function VideoPlayControlLayer:onEnterTransitionFinish()
    if nil ~= self.m_gameVideo then
        self:stopAllActions()
        self:stopScheduler()
        -- 开启定时器
        self:startScheduler()
    else
        QueryDialog:create("回放资源加载异常, CODE" .. GameVideo.ERRORCODE.CODE_REPLAYLOADER_ERROR .. ", 请重试!",function(ok)
            if nil ~= self._scene and nil ~= self._scene.onKeyBack then
                self._scene:onKeyBack()
            end
        end, nil, 1)
        :addTo(self)
    end
end

-- bForward, bBackward 
function VideoPlayControlLayer:popMessageUpdate( bForward, bBackward )
    local msg, bEnd, szTip = GameVideo:getInstance():getVideoMessage()
    if bEnd then
        self:stopAllActions()
        self:stopScheduler()
        QueryDialog:create("播放结束, 是否退出?",function(ok)
            if true == ok then
                if nil ~= self._scene and nil ~= self._scene.onKeyBack then
                    self._scene:onKeyBack()
                end
            end
        end)
        :setCanTouchOutside(false)
        :addTo(self)
        return
    end
    
    local nextSpeed, bEnableForward, bError, szMessage = self.m_gameVideo:onHandleMessage(self._gameLayer, msg, bForward, bBackward)
    nextSpeed = nextSpeed or 10
    if bError then
        if type(szMessage) == "string" and "" ~= szMessage then
            showToast(self, szMessage, 1)
        else
            showToast(self, "游戏回放异常 CODE" .. GameVideo.ERRORCODE.CODE_NO_DEFINE, 1)
        end
        self:stopAllActions()
        self:stopScheduler()
        return
    end
    GameVideo:getInstance():forwardMessageIdx()
    
    self.m_nSpeed = nextSpeed
    self.m_bEnableForward = bEnableForward
end

function VideoPlayControlLayer:startScheduler()
    -- 定时器
    local function countDown(dt)
        self.m_nSpeedStep = self.m_nSpeedStep + 1
        if 0 == math.mod(self.m_nSpeedStep, self.m_nSpeed) then
            self.m_nSpeedStep = 1
            self:popMessageUpdate()
        end
    end
    self.m_scheduler = scheduler:scheduleScriptFunc(countDown, PLAY_SPEED, false)
end

function VideoPlayControlLayer:stopScheduler()
    if nil ~= self.m_scheduler then
        scheduler:unscheduleScriptEntry(self.m_scheduler)
        self.m_scheduler = nil
    end
end

return VideoPlayControlLayer