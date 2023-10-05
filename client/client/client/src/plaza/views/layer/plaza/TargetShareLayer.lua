-- Author: zhong
-- Date: 2017-06-12 09:30:35
--
-- 包含(分享界面、面对面二维码)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 分享界面(二级弹窗)
local TargetShareLayer = class("TargetShareLayer", TransitionLayer)
-- 面对面二维码(三级弹窗)
local FtfQrLayer = class("FtfQrLayer", TransitionLayer)
TargetShareLayer.FtfQrLayer = FtfQrLayer

local QRNODE_NAME = "__qr_node_name__"
local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_CYCLE",            -- 朋友圈
    "BTN_WX",               -- 微信
    "BTN_F2F",              -- 面对面
    "BTN_BACK",             -- 返回
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
-- 按钮坐标
local tabPostion = 
{
    {{370}}, 
    {{296}, {550}},
    {{472.49}, {621.65}, {241.5}},
}

function FtfQrLayer:ctor( scene, param, level )
    FtfQrLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/F2fQrLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
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

    local qrcontent = self.param or yl.HTTP_URL
    local tmpqr = self.m_spBg:getChildByName("content")
    -- 二维码
    local qr = QrNode:createQrNode(qrcontent, tmpqr:getContentSize().width, 5, 1)
    self.m_spBg:addChild(qr)
    qr:setPosition(tmpqr:getPosition())
    tmpqr:removeFromParent()

    self:scaleTransitionIn(spbg)
end

-- 按键监听
function FtfQrLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    end
end

function FtfQrLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function FtfQrLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function FtfQrLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function FtfQrLayer:onTransitionOutFinish()
    self:removeFromParent()
end


function TargetShareLayer:ctor( scene, param, level )
    TargetShareLayer.super.ctor( self, scene, param, level )
    self.m_target = 0
    if type(param) == "table" then
        self.m_callback = param.callback
        self.m_target = param.target or 0
    end
    self.m_nSelectTarget = nil

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/TargetShareLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
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
    --btn:setPressedActionEnabled(true)

    -- 朋友圈
    btn = spbg:getChildByName("btn_target_1")
    btn:setTag(TAG_ENUM.BTN_CYCLE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)
    btn:setEnabled(false)

    -- 微信
    btn = spbg:getChildByName("btn_target_2")
    btn:setTag(TAG_ENUM.BTN_WX)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)
    btn:setEnabled(false)

    -- 二维码
    self.m_bF2f = false
    -- 我的二维码
    self.m_txtMyQr = spbg:getChildByName("txt_myqr")
    self.m_txtMyQr:setVisible(false)
    -- 二维码背景
    self.m_spQrBg = spbg:getChildByName("targetshare_qrbg")
    self.m_spQrBg:setVisible(false)

    local spIcon = spbg:getChildByName("plaza_sp_iconMyCode_2")
    spIcon:setVisible(false)
    local spBg = spbg:getChildByName("Image")
    spBg:setVisible(false)

    local bgwidth = spbg:getContentSize().width
    -- 整理列表
    local targetList = {}
    -- 朋友圈
    if 1 == bit:_and(self.m_target, 1) then
        table.insert(targetList, 1)
    end
    -- 微信好友
    if 2 == bit:_and(self.m_target, 2) then
        table.insert(targetList, 2)
    end
    -- 面对面
    if 4 == bit:_and(self.m_target, 4) then
        table.insert(targetList, 3)
    end
    self.m_targetList = targetList
    local nCount = #targetList
    local tabPos = tabPostion[nCount]
    self.m_nTargetCount = nCount
    if nil ~= tabPos then
        for k,v in pairs(targetList) do
            local xPos = tabPos[k][1]  

            -- 二维码
            if v == 3 then
                self.m_nSelectTarget = nil
                self.m_bF2f = true
                local qrcontent, saveCallBack = nil, nil
                if type(self.m_callback) == "function" then
                    qrcontent, saveCallBack = self.m_callback(self.m_nSelectTarget, self.m_bF2f)
                end
                self.m_bF2f = false
                if nil ~= qrcontent then
                    self.m_txtMyQr:setVisible(true)
                    self.m_spQrBg:setVisible(true)

                    local spIcon = spbg:getChildByName("plaza_sp_iconMyCode_2")
                    spIcon:setVisible(true)
                    local spBg = spbg:getChildByName("Image")
                    spBg:setVisible(true)

                    -- 二维码
                    local qr = QrNode:createQrNode(qrcontent, 220, 5, 1)
                    print("------------------------------------------------------>>>>", qr)
                    self.m_spBg:addChild(qr)
                    qr:setName(QRNODE_NAME)
                    qr:setPosition(233, self.m_spBg:getContentSize().height * 0.5)
                    if type(saveCallBack) == "function" then
                        qr:setTouchEnabled(true)
                        qr:addTouchEventListener(function(ref, tType)
                            if tType == ccui.TouchEventType.ended then
                                ExternalFun.popupTouchFilter(0, false)

                                local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
                                local xRate = framesize.width / yl.WIDTH 
                                local yRate = framesize.height / yl.HEIGHT
                                local bgsize = self.m_spBg:getContentSize()
                                local screenPos = self.m_spBg:convertToWorldSpace(cc.p(190, bgsize.height * 0.5))
                                local framePosX = (screenPos.x - 110) * xRate
                                local framePosY = (screenPos.y - 110) * yRate
                                local frameWidth = 220 * xRate
                                local frameHeight = 220 * yRate
                                local qrFrameRect = cc.rect(framePosX, framePosY, frameWidth, frameHeight)

                                captureScreenWithArea(qrFrameRect, "qr_code.png", function(ok, savepath)
                                    ExternalFun.dismissTouchFilter()
                                    saveCallBack(ok, savepath)
                                end)
                            end
                        end)
                    end
                end
            else
                local btn = spbg:getChildByName("btn_target_" .. v)
                local txt = spbg:getChildByName("txt" .. v)
                if nil ~= btn then
                    btn:setPositionX(xPos)
                    print("xpos ", xPos)
                    btn:setVisible(true)
                    btn:setEnabled(true)

                    txt:setVisible(true)
                    txt:setPositionX(xPos)
                end
            end
        end
    end 
    
    -- 加载动画
    self:scaleTransitionIn(spbg)
end

-- 按键监听
function TargetShareLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_CYCLE == tag then
        self.m_nSelectTarget = yl.ThirdParty.WECHAT_CIRCLE
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_WX == tag then
        self.m_nSelectTarget = yl.ThirdParty.WECHAT
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_F2F == tag then
        --[[self.m_nSelectTarget = nil
        self.m_bF2f = true

        local qrcontent, saveCallBack = nil, nil
        if type(self.m_callback) == "function" then
            qrcontent, saveCallBack = self.m_callback(self.m_nSelectTarget, self.m_bF2f)
        end
        if nil ~= qrcontent then
            for k, v in pairs(self.m_targetList) do
                local btn = self.m_spBg:getChildByName("btn_target_" .. v)
                if nil ~= btn then
                    btn:stopAllActions()
                    btn:setEnabled(false)
                    btn:runAction(cc.Sequence:create(
                        cc.Spawn:create(cc.FadeOut:create(0.5), cc.MoveBy:create(0.5, cc.p(-100, 0))),
                        cc.CallFunc:create(function()
                            self.m_txtMyQr:setVisible(true)
                            --self.m_btnBack:setVisible(true)
                        end))
                    )
                end
            end

            -- 二维码
            local qr = QrNode:createQrNode(qrcontent, 220, 5, 1)
            self.m_spBg:addChild(qr)
            qr:setName(QRNODE_NAME)
            qr:setPosition(190, self.m_spBg:getContentSize().height * 0.5)
            if type(saveCallBack) == "function" then
                qr:setTouchEnabled(true)
                qr:addTouchEventListener(function(ref, tType)
                    if tType == ccui.TouchEventType.ended then
                        ExternalFun.popupTouchFilter(0, false)

                        local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
                        local xRate = framesize.width / yl.WIDTH 
                        local yRate = framesize.height / yl.HEIGHT
                        local bgsize = self.m_spBg:getContentSize()
                        local screenPos = self.m_spBg:convertToWorldSpace(cc.p(190, bgsize.height * 0.5))
                        local framePosX = (screenPos.x - 110) * xRate
                        local framePosY = (screenPos.y - 110) * yRate
                        local frameWidth = 220 * xRate
                        local frameHeight = 220 * yRate
                        local qrFrameRect = cc.rect(framePosX, framePosY, frameWidth, frameHeight)

                        captureScreenWithArea(qrFrameRect, "qr_code.png", function(ok, savepath)
                            ExternalFun.dismissTouchFilter()
                            saveCallBack(ok, savepath)
                        end)
                    end
                end)
            end
        else
            print("面对面二维码内容为空!")
            self:scaleTransitionOut(self.m_spBg)
        end]]
    elseif TAG_ENUM.BTN_BACK == tag then
        --[[local qr = self.m_spBg:getChildByName(QRNODE_NAME)
        if nil ~= qr then
            qr:removeFromParent()
        end
        -- 按钮回位
        local tabPos = tabPostion[self.m_nTargetCount]
        if nil ~= tabPos then
            local bgwidth = self.m_spBg:getContentSize().width
            local yPos = self.m_spBg:getContentSize().height * 0.41
            for k, v in pairs(self.m_targetList) do
                local xPos = tabPos[k][1] * bgwidth
                local btn = self.m_spBg:getChildByName("btn_target_" .. v)
                print(btn)
                if nil ~= btn then
                    btn:stopAllActions()
                    btn:setEnabled(true)
                    btn:runAction(cc.Spawn:create(cc.FadeIn:create(0.5), cc.MoveTo:create(0.5, cc.p(xPos, yPos))))
                end
            end
        end

        self.m_txtMyQr:setVisible(false)
        --self.m_btnBack:setVisible(false)]]
    end
end

function TargetShareLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function TargetShareLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function TargetShareLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function TargetShareLayer:onTransitionOutFinish()
    if type(self.m_callback) == "function" then
        self.m_callback(self.m_nSelectTarget, self.m_bF2f)
    end 
    self:removeFromParent()
end

return TargetShareLayer