
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local ClubMemberSet = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMemberSet")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

-- 用户中心(二级弹窗)
local ClubShareLayer = class("ClubShareLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_SHARE",            -- 分享
    "BTN_SAVE",             -- 保存
    "BTN_EXIT",             -- 退出大联盟
    "BTN_WX",               --微信
    "BTN_WXC",              --朋友圈

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubShareLayer:ctor( scene, param, level ) --param 传房间信息
    local this = self

    ClubShareLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_share.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    self.m_param = param

    dump(param)
    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_WX)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    local btn = spbg:getChildByName("Button_1_0")
    btn:setTag(TAG_ENUM.BTN_WXC)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_szShareLink = GlobalUserItem.tabSpreadInfo["sharelink"] or yl.HTTP_URL

    if param.url and  param.url ~= "" then
            self.m_szShareLink = param.url
    end

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
end

function ClubShareLayer:onExit()

    ClubShareLayer.super.onExit(self)
end

-- 按键监听
function ClubShareLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then

        self:removeFromParent()

    elseif TAG_ENUM.BTN_WX == tag or TAG_ENUM.BTN_WXC == tag then
        local index = TAG_ENUM.BTN_WX == tag and 0 or 1

        MultiPlatform:getInstance():shareToTarget(index, function(isok)

            end, self.m_param.title,self.m_param.content, self.m_szShareLink)


    end

end



function ClubShareLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubShareLayer:onTransitionInFinish()
    self:onRefreshInfo()
end

function ClubShareLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubShareLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubShareLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end



return ClubShareLayer