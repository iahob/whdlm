--
-- Author: zhong
-- Date: 2017-06-01 09:19:41
--
-- 约战详情(三级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PrivateDetailLayer = class("PrivateDetailLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_DISSUME",          -- 解散
    "BTN_JOIN",             -- 加入
    "BTN_INVITE",           -- 邀请
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
function PrivateDetailLayer:ctor( scene, param, level )
    PrivateDetailLayer.super.ctor( self, scene, param, level )
    local this = self
    -- 是否游戏中
    local onGame = self._param.onGame or false
    -- 是否允许解散
    local enableDismiss = self._param.enableDismiss or false

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("private/PrivateDetailLayer.csb", self)
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

    -- 解散
    btn = spbg:getChildByName("btn_dissume")
    btn:setTag(TAG_ENUM.BTN_DISSUME)
    btn:addTouchEventListener( touchFunC )

    -- 加入
    btn = spbg:getChildByName("btn_join")
    btn:setTag(TAG_ENUM.BTN_JOIN)
    btn:addTouchEventListener( touchFunC )

    -- 邀请
    btn = spbg:getChildByName("btn_invite")
    btn:setTag(TAG_ENUM.BTN_INVITE)
    btn:addTouchEventListener( touchFunC )

    -- 房间ID
    self.m_txtRoomId = spbg:getChildByName("txt_roomid")
    local dwRoomId = param.dwPersonalRoomID or 0
    self.m_txtRoomId:setString(string.format("%06d", dwRoomId))

    -- 创建时间
    local tabTime = param.sysCreateTime
    -- 日期
    spbg:getChildByName("txt_date"):setString(string.format("%d-%02d-%02d", tabTime.wYear, tabTime.wMonth, tabTime.wDay))
    -- 时间
    spbg:getChildByName("txt_time"):setString(string.format("%02d:%02d:%02d", tabTime.wHour, tabTime.wMinute, tabTime.wSecond))

    -- 房主(创建记录, 房主一定为自己)
    self.m_clipRoomer = spbg:getChildByName("txt_roomer")
    local strNickname = string.EllipsisByConfig(GlobalUserItem.tabAccountInfo.szNickName, 160, 
                                            string.getConfig("fonts/FZHTJW.TTF.ttf", 22))
    self.m_clipRoomer:setString(strNickname)
    -- self.m_clipRoomer = ClipText:createClipText(tmp:getContentSize(), GlobalUserItem.tabAccountInfo.szNickName, nil, 24)
    -- self.m_clipRoomer:setAnchorPoint(tmp:getAnchorPoint())
    -- self.m_clipRoomer:setPosition(tmp:getPosition())
    --self.m_clipRoomer:setTextColor(cc.c4b(116, 71, 40, 255))
    -- spbg:addChild(self.m_clipRoomer)
    -- tmp:removeFromParent()

    -- 状态
    local txtstatus = spbg:getChildByName("txt_state")
    self.m_txtStatus = txtstatus
    local joinPlayer = 0
    local maxPlayer = param.lPlayerCount or 0
    local spStatus=spbg:getChildByName("sp_detail_status_tips")
    self.m_spStatus=spStatus
    if param.cbIsDisssumRoom == PriRoom.ROOM_STATE.PLAYING_STATE then
        spStatus:setTexture("private/pri_sp_playing.png")
        --self:playingStatusAnimation()
        --txtstatus:setTextColor(cc.c3b(255, 59, 119))
    else
        spStatus:setTexture("private/pri_sp_wait.png")
        -- 等待加入
        --self:waitingStatusAnimation()
        --txtstatus:setTextColor(cc.c3b(170, 255, 60))
    end

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

function PrivateDetailLayer:onButtonClickedEvent(tag, ref)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.BTN_JOIN then
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onSearchRoom(self._param.dwPersonalRoomID)

        -- 移除
        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.BTN_DISSUME then
        -- 记录标识
        GlobalUserItem.nCurGameKind = self._param.dwGameKindID
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onDissumeRoom(self._param.dwPersonalRoomID)

        -- 移除
        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.BTN_INVITE then
        local this = self
        PriRoom:getInstance():queryRoomPasswd(
            this._param.dwPersonalRoomID, 
            function(cmd_table)
                if nil == this._scene then
                    return
                end
                local dwRoomDwd = cmd_table.dwRoomDwd or 0
                local function _shareFun( dwRoomDwd )
                    -- 第三方分享
                    if nil ~= this._scene.popTargetShare then
                        this._scene:popTargetShare(function(target)
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
                    :addTo(this._scene)
                else
                    _shareFun(dwRoomDwd)
                end
            end
        )
    end
end

function PrivateDetailLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PrivateDetailLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function PrivateDetailLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function PrivateDetailLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function PrivateDetailLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end

function PrivateDetailLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function PrivateDetailLayer:playingStatusAnimation()
    -- -- 游戏中
    -- self.m_txtStatus:setString("进行中")
    -- self.m_txtStatus:stopAllActions()
    -- local call1 = cc.CallFunc:create(function()
    --     self.m_txtStatus:setString("进行中.")
    -- end)
    -- local call2 = cc.CallFunc:create(function()
    --     self.m_txtStatus:setString("进行中..")
    -- end)
    -- local call3 = cc.CallFunc:create(function()
    --     self.m_txtStatus:setString("进行中...")
    -- end)
    -- local call4 = cc.CallFunc:create(function()
    --     self:playingStatusAnimation()
    -- end)
    -- self.m_txtStatus:runAction(cc.Sequence:create(
    --     cc.DelayTime:create(0.5),
    --     call1, 
    --     cc.DelayTime:create(0.5), 
    --     call2, 
    --     cc.DelayTime:create(0.5),
    --     call3,
    --     cc.DelayTime:create(0.5),
    --     call4)
    -- )
end

function PrivateDetailLayer:waitingStatusAnimation()
    -- -- 等待中
    -- self.m_txtStatus:setString("等待加入")
    -- self.m_txtStatus:stopAllActions()
    -- local call1 = cc.CallFunc:create(function()
    --     self.m_txtStatus:setString("等待加入.")
    -- end)
    -- local call2 = cc.CallFunc:create(function()
    --     self.m_txtStatus:setString("等待加入..")
    -- end)
    -- local call3 = cc.CallFunc:create(function()
    --     self.m_txtStatus:setString("等待加入...")
    -- end)
    -- local call4 = cc.CallFunc:create(function()
    --     self:waitingStatusAnimation()
    -- end)
    -- self.m_txtStatus:runAction(cc.Sequence:create(
    --     cc.DelayTime:create(0.5),
    --     call1, 
    --     cc.DelayTime:create(0.5), 
    --     call2, 
    --     cc.DelayTime:create(0.5),
    --     call3,
    --     cc.DelayTime:create(0.5),
    --     call4)
    -- )
end

return PrivateDetailLayer