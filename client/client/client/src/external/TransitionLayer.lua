--
-- Author: zhong
-- Date: 2017-05-23 15:47:17
--
--[[
 带切换动画层

 主要用于弹窗显示
 1、二级弹窗之上, 有三级弹窗的情况, 二级弹窗需要注册监听
 2、三级弹窗需要在显示/隐藏的时候发送通知
 3、弹窗/界面统一以上级弹窗/界面的父节点为父节点: 
    二级弹窗, 以一级界面的parent为父节点 SettingLayer 以 PlazaLayer的parent(ClientScene)为父节点
    三级弹窗, 以二级弹窗的parent为父节点 LocaMachineLayer 以 SettingLayer的parent(ClientScene)为父节点
]]
local TransitionLayer = class("TransitionLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- listen
TransitionLayer.EVENT_LISTENER = "__transition_event_listener__"
-- event
TransitionLayer.SHOW_EVENT = "__transition_show_event__"
TransitionLayer.HIDE_EVENT = "__transition_hide_event__"
-- level
TransitionLayer.SECOND_LEVEL = 2    -- 第二层级
TransitionLayer.THIRD_LEVEL = 3     -- 第三层级
TransitionLayer.FOURTH_LEVEL = 4    -- 第四层级
-- ...

-- 网络监听
TransitionLayer.HTTPNET_LISTENER = "__transition_httpnet_listener__"

function TransitionLayer:ctor( scene, param, level )
    ExternalFun.registerNodeEvent(self)
    self.m_bTransition = false
    self._scene = scene
    self._param = param

    level = level or TransitionLayer.SECOND_LEVEL
    -- 显示层级
    self.m_nLevel = level
    -- 移除监听
    self.m_removeCallBack = nil
    -- 事件监听
    self.m_listener = nil
    -- http请求监听
    self.m_httpnetListener = nil
end

-- 移除监听
function TransitionLayer:setRemoveListener( callback )
    self.m_removeCallBack = callback
end

-- 显示等待
function TransitionLayer:showPopWait()
    if nil ~= self._scene and nil ~= self._scene.showPopWait then
        self._scene:showPopWait()
    end
end

-- 隐藏等待
function TransitionLayer:dismissPopWait()
    if nil ~= self._scene and nil ~= self._scene.dismissPopWait then
        self._scene:dismissPopWait()
    end
end

-- 更新层级
function TransitionLayer:setLevel( nLevel )
    nLevel = nLevel or 1
    self.m_nLevel = nLevel
end

-- 获取层级
function TransitionLayer:getLevel()
    return self.m_nLevel
end

-- 动画移除
function TransitionLayer:animationRemove()
    self:sendHideEvent()
    self:removeFromParent()
end

-- 缩小放大 入场动画
-- @param[actionNode] 缩放节点
function TransitionLayer:scaleTransitionIn( actionNode )
    if nil == actionNode then
        print("TransitionLayer:transitionIn actionNode is Nil")
        return
    end
    if self.m_bTransition then
        print("TransitionLayer scaleTransitionIn bTransition")
        return
    end
    local this = self
    actionNode:setScale(0.05)
    local begin = cc.CallFunc:create(function()
        this:onTransitionInBegin()
    end)
    local scale = cc.ScaleTo:create(0.2, 1.0)
    local finish = cc.CallFunc:create(function()
        this:onTransitionInFinish()
        this.m_bTransition = false
    end)
    this.m_bTransition = true
    local seq = cc.Sequence:create(begin, scale, cc.DelayTime:create(0.2), finish)
    actionNode:stopAllActions()
    actionNode:runAction(seq)
end

-- 缩小放大 入场动画
-- @param[actionNode] 缩放节点
function TransitionLayer:scaleShakeTransitionIn( actionNode )
    if nil == actionNode then
        print("TransitionLayer:transitionIn actionNode is Nil")
        return
    end
    if self.m_bTransition then
        print("TransitionLayer scaleTransitionIn bTransition")
        return
    end
    local this = self
    actionNode:setScale(0.8)
    actionNode:setOpacity(0)
    local begin = cc.CallFunc:create(function()
        this:onTransitionInBegin()
    end)
    --local fadeOut = cc.FadeOut:create(0.01) 

    local scale = cc.ScaleTo:create(0.2, 1)
    local fadeIn = cc.FadeIn:create(0.4)
    --local scale1 cc.ScaleTo:create(0.1, 0.4)

    --local scale2 = cc.Sequence:create(scale, scale1)
    local show = cc.Spawn:create(scale, fadeIn)


    local finish = cc.CallFunc:create(function()
        this:onTransitionInFinish()
        this.m_bTransition = false
    end)
    this.m_bTransition = true
    local seq = cc.Sequence:create(begin, show, cc.DelayTime:create(0.2), finish)
    actionNode:stopAllActions()
    actionNode:runAction(seq)
end


-- 直接显示
-- @param[actionNode] 缩放节点
function TransitionLayer:noActionTransitionIn( actionNode )
    if nil == actionNode then
        print("TransitionLayer:transitionIn actionNode is Nil")
        return
    end
    if self.m_bTransition then
        print("TransitionLayer scaleTransitionIn bTransition")
        return
    end
    local this = self
    local begin = cc.CallFunc:create(function()
        this:onTransitionInBegin()
    end)
    local finish = cc.CallFunc:create(function()
        this:onTransitionInFinish()
        this.m_bTransition = false
    end)
    this.m_bTransition = true
    local seq = cc.Sequence:create(begin, cc.DelayTime:create(0.1), finish)
    actionNode:stopAllActions()
    actionNode:runAction(seq)
end


function TransitionLayer:onTransitionInBegin()

end

function TransitionLayer:onTransitionInFinish()

end

function TransitionLayer:scaleTransitionOut( actionNode )
    if nil == actionNode then
        print("TransitionLayer:transitionIn actionNode is Nil")
        return
    end
    if self.m_bTransition then
        print("TransitionLayer scaleTransitionOut bTransition")
        return
    end
    --[[
    actionNode:setScale(1.0)
    local begin = cc.CallFunc:create(function()
        self.m_bTransition = true
        self:onTransitionOutBegin()
    end)
    local scale = cc.ScaleTo:create(0.1, 0.5)
    local finish = cc.CallFunc:create(function()
        self:onTransitionOutFinish()
        self.m_bTransition = false
    end)
    local seq = cc.Sequence:create(begin, scale, cc.Hide:create(), cc.DelayTime:create(0.2), finish)
    actionNode:stopAllActions()
    actionNode:runAction(seq)]]
    
    self:onTransitionOutBegin()
    self:onTransitionOutFinish()
end

function TransitionLayer:scaleTransitionOutAction( actionNode )
        if nil == actionNode then
        print("TransitionLayer:transitionIn actionNode is Nil")
        return
    end
    if self.m_bTransition then
        print("TransitionLayer scaleTransitionOut bTransition")
        return
    end
    actionNode:setScale(1.0)
    local begin = cc.CallFunc:create(function()
        self.m_bTransition = true
        self:onTransitionOutBegin()
    end)
    local scale = cc.ScaleTo:create(0.1, 0.2)
    local finish = cc.CallFunc:create(function()
        self:onTransitionOutFinish()
        self.m_bTransition = false
    end)
    local seq = cc.Sequence:create(begin, scale, cc.Hide:create(), cc.DelayTime:create(0.2), finish)
    actionNode:stopAllActions()
    actionNode:runAction(seq)
    
    --self:onTransitionOutBegin()
    --self:onTransitionOutFinish()
end


function TransitionLayer:onTransitionOutBegin()

end

function TransitionLayer:onTransitionOutFinish()

end

function TransitionLayer:onOtherShowEvent()

end

function TransitionLayer:onOtherShowEventFinish()

end

function TransitionLayer:onOtherHideEvent()

end

function TransitionLayer:onOtherHideEventFinish()

end

-- ui事件
function TransitionLayer:onEvent( event )
    print("--- TransitionLayer:onEvent ---, cname ==> ", self.__cname)
    local sender = event.sender
    -- 自己消息
    if type(sender.__cname) ~= "string" or sender.__cname == self.__cname then
        --print("自己消息")
        return
    end
    -- 层级消息
    local senderLev = event.senderLevel or 1
    if self.m_nLevel + 1 ~= senderLev then
        print("越级消息 get, send", self.m_nLevel, senderLev)
        return
    end
    print("event msg ==> ", event.msg)
    if event.msg == TransitionLayer.SHOW_EVENT then
        self:onOtherShowEvent()
        self:onOtherShowEventFinish()
    elseif event.msg == TransitionLayer.HIDE_EVENT then
        self:onOtherHideEvent()
        self:onOtherHideEventFinish()
    end
end

function TransitionLayer:registerEventListenr()
    self:unregisterEventListener()
    self.m_listener = cc.EventListenerCustom:create(TransitionLayer.EVENT_LISTENER,handler(self, self.onEvent))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end

function TransitionLayer:unregisterEventListener()
    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
        self.m_listener = nil
    end
end

function TransitionLayer:sendShowEvent()
    local event = cc.EventCustom:new(TransitionLayer.EVENT_LISTENER)
    event.msg = TransitionLayer.SHOW_EVENT
    event.sender = self
    event.senderLevel = self.m_nLevel

    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

function TransitionLayer:sendHideEvent()
    local event = cc.EventCustom:new(TransitionLayer.EVENT_LISTENER)
    event.msg = TransitionLayer.HIDE_EVENT
    event.sender = self
    event.senderLevel = self.m_nLevel

    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end
-- ui事件

-- net事件
function TransitionLayer:onHttpRequestFinish( jstable, jsdata )

end

function TransitionLayer:onHttpReceiveEvent( event )
    print("--- TransitionLayer:onHttpReceiveEvent ---")
    if nil ~= event then
        self:onHttpRequestFinish(event.jstable, event.jsdata, event)
    end
    self:unRegisterHttpNetListener()
end

function TransitionLayer:doHttpRequest( url, method, param )
    local this = self
    self:registerHttpNetListener()
    appdf.onHttpJsionTable(url, method, param, function(jstable, jsdata)
        local event = cc.EventCustom:new(TransitionLayer.HTTPNET_LISTENER)
        event.sender = this
        event.jstable = jstable
        event.jsdata = jsdata
        event.url = url
        event.method = method
        event.param = param
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
    end)
end

function TransitionLayer:registerHttpNetListener()
    self:unRegisterHttpNetListener()
    self.m_httpnetListener = cc.EventListenerCustom:create(TransitionLayer.HTTPNET_LISTENER,handler(self, self.onHttpReceiveEvent))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_httpnetListener, self)
end

function TransitionLayer:unRegisterHttpNetListener()
    if nil ~= self.m_httpnetListener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_httpnetListener)
        self.m_httpnetListener = nil
    end
end
-- net事件

function TransitionLayer:onEnterTransitionFinish()

end

function TransitionLayer:onExit()
    
end

function TransitionLayer:onCleanup()
    print("TransitionLayer:onCleanup, cname ==> ", self.__cname)
    self:unregisterEventListener()
    self:unRegisterHttpNetListener()
end

function TransitionLayer:onRefreshInfo()
    print("TransitionLayer:onRefreshInfo")
end

return TransitionLayer