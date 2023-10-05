--
-- Author: zhong
-- Date: 2017-05-27 15:44:36
--
-- 
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

-- 用户中心(二级弹窗)
local UserCenterLayer = class("UserCenterLayer", TransitionLayer)
-- 实名认证(三级弹窗)
UserCenterLayer.IdentifyLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ucenter.IdentifyLayer")

-- ui
local UserInfoLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ucenter.UserInfoLayer")                        -- 个人信息
local BankInfoLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ucenter.BankInfoLayer")                        -- 我的银行
local TurnoverInfoLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ucenter.TurnoverInfoLayer")                -- 账户流水
-- ui

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_IDENTIFY",         -- 认证
    "BTN_COPY",             -- 复制
    "BTN_CHARGE",           -- 充值
    "IMAGE_EDIT",           -- 编辑
    "BTN_SURE",             -- 确定

    "CBT_UINFO",            -- 个人信息
    "CBT_BANK",             -- 我的银行
    "CBT_TURNOVER",         -- 账户流水
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function UserCenterLayer:ctor( scene, param, level )
    local this = self

    UserCenterLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("usercenter/UserCenterLayer.csb", self)
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
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    -- 背景
    --self.m_spUserCenterBg = spbg:getChildByName("ucenter_sp_bg")

    self.m_selectBg=spbg:getChildByName("selectbg")

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_tabCheckBox = {}
    self.m_nSelectBox = TAG_ENUM.CBT_UINFO
    -- 
    local checkboxlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end

    -- 个人信息
    local checkbox = spbg:getChildByName("check_uinfo")
    checkbox:setTag(TAG_ENUM.CBT_UINFO)
    checkbox:addEventListener(checkboxlistener)
    checkbox:setSelected(true)
    self.m_tabCheckBox[TAG_ENUM.CBT_UINFO] = checkbox

    -- 我的银行
    checkbox = spbg:getChildByName("check_bank")
    checkbox:setTag(TAG_ENUM.CBT_BANK)
    checkbox:addEventListener(checkboxlistener)
    checkbox:setSelected(false)
    self.m_tabCheckBox[TAG_ENUM.CBT_BANK] = checkbox

    -- 账户流水
    checkbox = spbg:getChildByName("check_turnover")
    checkbox:setTag(TAG_ENUM.CBT_TURNOVER)
    checkbox:addEventListener(checkboxlistener)
    checkbox:setSelected(false)
    self.m_tabCheckBox[TAG_ENUM.CBT_TURNOVER] = checkbox

    -- 界面管理
    self.m_tabLayer = {}

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleTransitionIn(spbg)
end

function UserCenterLayer:onExit()
    if nil ~= self._modifyFrame then
        self._modifyFrame:onCloseSocket()
        self._modifyFrame = nil
    end 
    UserCenterLayer.super.onExit(self)
end

-- 按键监听
function UserCenterLayer:onButtonClickedEvent(tag,sender)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.IMAGE_BG == tag then
        local curLayer = self.m_tabLayer[self.m_nSelectBox]
        if nil ~= curLayer then
            -- 监听 editbox 关闭键盘, 吞噬多余的触摸事件
            if type(curLayer.onTouchImageBg) == "function" then
                curLayer:onTouchImageBg()
            end
        end
    end
end

function UserCenterLayer:onSelectedEvent(tag, sender)
    local curLayer = self.m_tabLayer[self.m_nSelectBox]
    if nil ~= curLayer then
        local targetPlatform = cc.Application:getInstance():getTargetPlatform()
        -- 监听 editbox 关闭键盘
        if curLayer.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
            sender:setSelected(self.m_nSelectBox == tag)
             self.m_selectBg:setPosition(sender:getPosition())
            return
        end
    end
    if self.m_nSelectBox == tag then
        sender:setSelected(true)
        return
    end
    self.m_selectBg:setPosition(sender:getPosition())
    self.m_nSelectBox = tag
    for k,v in pairs(self.m_tabCheckBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    self:onRefreshInfo()
end

function UserCenterLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function UserCenterLayer:onTransitionInFinish()
    self:onRefreshInfo()
end

function UserCenterLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function UserCenterLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function UserCenterLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
    local curLayer = self.m_tabLayer[self.m_nSelectBox]
    if nil ~= curLayer then
        if type(curLayer.onRefreshInfo) == "function" then
            curLayer:onRefreshInfo()
        end
    end
end

function UserCenterLayer:onRefreshInfo()
    for k,v in pairs(self.m_tabLayer) do
        v:setVisible(false)
    end
    local curLayer = self.m_tabLayer[self.m_nSelectBox]
    if nil == curLayer then
        if TAG_ENUM.CBT_UINFO == self.m_nSelectBox then
            curLayer = UserInfoLayer:create(self._scene)
            self.m_spBg:addChild(curLayer)
            self.m_tabLayer[self.m_nSelectBox] = curLayer
        elseif TAG_ENUM.CBT_BANK == self.m_nSelectBox then
            curLayer = BankInfoLayer:create(self._scene)
            self.m_spBg:addChild(curLayer)
            self.m_tabLayer[self.m_nSelectBox] = curLayer
        elseif TAG_ENUM.CBT_TURNOVER == self.m_nSelectBox then
            curLayer = TurnoverInfoLayer:create(self._scene)
            self.m_spBg:addChild(curLayer)
            self.m_tabLayer[self.m_nSelectBox] = curLayer
        end
    end
    --self.m_spUserCenterBg:setVisible(TAG_ENUM.CBT_TURNOVER ~= self.m_nSelectBox)

    if nil ~= curLayer then
        curLayer:setVisible(true)
        if type(curLayer.onRefreshInfo) == "function" then
            curLayer:onRefreshInfo()
        end
    end
end

return UserCenterLayer