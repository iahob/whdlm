--
-- Author: zhong
-- Date: 2017-06-29 17:20:09
--
-- 包含两个界面(大厅等待、断网重连)
local ClientPopWait = {}
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local scheduler = cc.Director:getInstance():getScheduler()

-- 加载等待
local PopWait = class("PopWait", ccui.Layout)
ClientPopWait.PopWait = PopWait
function PopWait:ctor(isTransparent, delayRemove, aniDelay,showNow)
    if width == nil or height == nil then
        self:setContentSize(display.width,display.height)
    else
        self:setContentSize(width,height)
    end
    self.m_animationDelay = aniDelay or 0.03
    self:setTouchEnabled(true)
    self.m_beginTime = showNow or 1 

    if not isTransparent then
        self:setBackGroundColorType(LAYOUT_COLOR_SOLID)
        self:setBackGroundColorOpacity(0)
        self:setBackGroundColor(cc.c3b(0, 0, 0))
        
        local spRound = cc.Sprite:create("public/public_blank.png")
            :addTo(self)
            :move(appdf.WIDTH * 0.5,appdf.HEIGHT * 0.5)
        self:stopAllActions()
        self:runAction(cc.Sequence:create(
            cc.DelayTime:create(self.m_beginTime),
            cc.CallFunc:create(function()
                self:setBackGroundColorOpacity(125)
                local param = AnimationMgr.getAnimationParam()
                param.m_fDelay = self.m_animationDelay
                param.m_bRestore = true
                -- 动画
                param.m_strName = yl.COMMON_LOADING_ANI
                local animate = AnimationMgr.getAnimate(param)
                if nil ~= animate then
                    local rep = cc.RepeatForever:create(animate)
                    spRound:runAction(rep)
                end
            end))
        )
    end 
end

--显示
function PopWait:show(parent,message)
    self:addTo(parent)
    return self
end

--显示状态
function PopWait:isShow()
    return not self._dismiss 
end

--取消显示
function PopWait:dismiss()
    if self._dismiss then
        return
    end
    self._dismiss  = true
    self:stopAllActions()
    self:runAction(cc.RemoveSelf:create())
end

-- 断网重连
local ReConnectPopWait = class("ReConnectPopWait", cc.Layer)
ClientPopWait.ReConnectPopWait = ReConnectPopWait
-- @param[waittime] 重连时间
-- @param[callback] 回调
function ReConnectPopWait:ctor( waittime, callback )
    self._callback = callback
    -- 注册触摸事件
    ExternalFun.registerTouchEvent(self, true)

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/ReConnectPopWait.csb", self)

    -- 
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 旋转
    local spRound = spbg:getChildByName("sp_waitround")
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.03
    -- 动画
    param.m_strName = yl.COMMON_LOADING_ANI
    param.m_bRestore = true
    local animate = AnimationMgr.getAnimate(param)
    if nil ~= animate then
        local rep = cc.RepeatForever:create(animate)
        spRound:runAction(rep)
    end

    -- 文本
    local txttip = spbg:getChildByName("txt_waittip")
    txttip:setString("您的网络出现异常,  正在重连 20")
    self.m_txtWatiTip = txttip

    self.m_nPopCount = 20
    -- 定时器
    local function countDown(dt)
        self:reConnectUpdate()
    end
    self.m_scheduler = scheduler:scheduleScriptFunc(countDown, 1.0, false)
end

function ReConnectPopWait:onExit()
    if nil ~= self.m_scheduler then
        scheduler:unscheduleScriptEntry(self.m_scheduler)
        self.m_scheduler = nil
    end
end

function ReConnectPopWait:reConnectUpdate()
    self.m_nPopCount = self.m_nPopCount - 1
    if self.m_nPopCount >= 0 then
        self.m_txtWatiTip:setString(string.format("您的网络出现异常,  正在重连 %02d", self.m_nPopCount))
    end
    if type(self._callback) == "function" then
        self._callback(self.m_nPopCount)
    end
end

return ClientPopWait