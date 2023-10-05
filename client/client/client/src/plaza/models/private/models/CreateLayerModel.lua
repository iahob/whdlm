--
-- Author: zhong
-- Date: 2016-12-31 09:41:59
--
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local CreateLayerModel = class("CreateLayerModel", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd_private = appdf.req(PriRoom.MODULE.PRIHEADER .. "CMD_Private")

function CreateLayerModel:ctor( scene, param, level )
    CreateLayerModel.super.ctor( self, scene, param, level )
    self._scene = scene
    self._cmd_pri_login = cmd_private.login
    self._cmd_pri_game = cmd_private.game
    self._cmd_pri = cmd_private
end

function CreateLayerModel:onTransitionInFinish()
    -- 请求配置
    if not GlobalUserItem.bSingleGame then
        PriRoom:getInstance():showPopWait()
        -- 弹出 创建界面时 已经将创建界面设置为ViewFrame
        --PriRoom:getInstance():setViewFrame(self)
        -- 请求私人房配置
        PriRoom:getInstance():getNetFrame():onGetRoomParameter()
    else
        self:onRefreshFeeList()
    end
end

function CreateLayerModel:onTransitionInBegin()
    self:sendShowEvent()
end

function CreateLayerModel:onTransitionOutBegin()
    self:sendHideEvent()

    -- -- 通知更新    
    -- local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    -- eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
    -- cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

function CreateLayerModel:onOtherHideEventFinish()
    if self:isVisible() then
        PriRoom:getInstance():setViewFrame(self)
    end
end

-- 刷新界面
function CreateLayerModel:onRefreshInfo()
    print("base onRefreshInfo")
end

-- 刷新费用
function CreateLayerModel:onRefreshFeeList()
    print("base onRefreshFeeList")
end

-- 获取邀请分享内容
function CreateLayerModel:getInviteShareMsg( roomDetailInfo )
    print("base get invite")
    return {title = "", content = ""}
end

-- 获取邀请分享内容
function CreateLayerModel:getCopyShareMsg( roomDetailInfo )
    print("base get copy")
    return {content = ""}
end

------
-- 网络消息
------
-- 私人房创建成功
function CreateLayerModel:onRoomCreateSuccess()
    local needClose = false
    -- 创建成功
    if (0 == PriRoom:getInstance().m_tabRoomOption.cbIsJoinGame and 0 == PriRoom:getInstance(). m_bPlayModel) 
        or (0 == PriRoom:getInstance().m_tabGoldRoomOption.cbIsJoinGame and 1 == PriRoom:getInstance(). m_bPlayModel) then 
        -- 代开游戏
        if true == PriRoom:getInstance().m_bCreateForOther then
            -- 非必须加入
            PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_CREATERESULT, nil, self._scene)
            needClose = true
        else
            PriRoom:getInstance():getNetFrame():onSearchRoom(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID)
            needClose = false
            --assert()
        end
    end
    self:onRefreshInfo()
    return needClose
end

-- 私人房登陆完成
function CreateLayerModel:onLoginPriRoomFinish()
    print("base login finish")
    return false
end

return CreateLayerModel