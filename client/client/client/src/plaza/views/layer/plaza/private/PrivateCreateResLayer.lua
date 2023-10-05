--
-- Author: zhong
-- Date: 2017-05-31 19:01:03
--
-- 创建结果(二级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PrivateCreateResLayer = class("PrivateCreateResLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_COPY",             -- 复制
    "BTN_ENTER",            -- 加入
    "BTN_INVITE",           -- 分享
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function PrivateCreateResLayer:ctor( scene, param, level )
    PrivateCreateResLayer.super.ctor( self, scene, param, level )
    local this = self
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("private/PrivateCreateResLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 复制
    btn = spbg:getChildByName("btn_copy")
    btn:setTag(TAG_ENUM.BTN_COPY)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 加入
    btn = spbg:getChildByName("btn_enter")
    btn:setTag(TAG_ENUM.BTN_ENTER)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 分享
    btn = spbg:getChildByName("btn_invite")
    btn:setTag(TAG_ENUM.BTN_INVITE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 房间id
    spbg:getChildByName("txt_roomid"):setString(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID .. "")

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleTransitionIn(spbg)
end

function PrivateCreateResLayer:onButtonClickedEvent( tag, ref )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_COPY == tag then
        PriRoom:getInstance():queryRoomPasswd( 
            PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, 
            function(cmd_table)
                local m_tabDetail = {} 
                m_tabDetail.dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                m_tabDetail.dwPlayTurnCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit

                -- 复制分享
                local msgTab = PriRoom:getInstance():getCopyShareMsg(m_tabDetail)
                local content = msgTab.content or ""
                if nil ~= cmd_table.dwRoomDwd and 0 ~= cmd_table.dwRoomDwd then
                    content = content .. "密码{" .. cmd_table.dwRoomDwd .. "}"
                end
                -- 附加验证
                content = content .. "(复制此消息打开游戏可直接进入游戏房间***" .. "###" .. "***)"
                MultiPlatform:getInstance():copyToClipboard(content)
            end
        )
    elseif TAG_ENUM.BTN_ENTER == tag then
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onSearchRoom(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID)

        -- 移除自己
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_INVITE == tag then
        PriRoom:getInstance():queryRoomPasswd(
            PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, 
            function(cmd_table)
                if nil == self._scene then
                    return
                end
                local dwRoomDwd = cmd_table.dwRoomDwd or 0
                local function _shareFun( dwRoomDwd )
                    -- 第三方分享
                    if nil ~= self._scene.popTargetShare then
                        self._scene:popTargetShare(function(target)
                            if nil ~= target then
                                local m_tabDetail = {} 
                                m_tabDetail.dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                m_tabDetail.dwPlayTurnCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit                            
                                m_tabDetail.dwRoomDwd = dwRoomDwd

                                local msgTab = PriRoom:getInstance():getInviteShareMsg(m_tabDetail)
                                local content = msgTab.content or ""
                                MultiPlatform:getInstance():shareToTarget(target, function(isok)
                                    end, msgTab.title, msgTab.content, msgTab.url)
                            end
                        end, 3)
                    end
                end
                if 0 ~= dwRoomDwd then
                    local query = QueryDialog:create("约战密码为 " .. dwRoomDwd .. " 是否邀请好友游戏?", function(ok)
                        if ok then
                            _shareFun(dwRoomDwd)
                        end
                    end)
                    :setCanTouchOutside(false)
                    :addTo(self._scene)
                else
                    _shareFun(dwRoomDwd)
                end
            end
        )
    end
end

function PrivateCreateResLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PrivateCreateResLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function PrivateCreateResLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function PrivateCreateResLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function PrivateCreateResLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function PrivateCreateResLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end

return PrivateCreateResLayer