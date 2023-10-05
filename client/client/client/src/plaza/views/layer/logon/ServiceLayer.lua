--[[
	服务条款页面
	2016_06_03 Ravioyla
	功能：显示服务条款
]]
-- 登陆(二级弹窗)
-- 设置(三级弹窗)

local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ServiceLayer = class("ServiceLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local HelpLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "HelpLayer")

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭
    "TAG_MASK",             -- 遮罩
    "TAG_TEL",              -- 电话
    "TAG_WX",               -- 微信
    "TAG_QQ",               -- qq
    "BTN_HELP"              --帮助
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ServiceLayer:ctor( scene, param, level )
    ServiceLayer.super.ctor( self, scene, param, level )
	-- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("service/ServiceLayer.csb", self)
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

    -- 电话
     self.m_txtTel = spbg:getChildByName("txt_tel")
    -- local image = spbg:getChildByName("image_tel")
    -- image:setTag(TAG_ENUM.TAG_TEL)
    -- image:addTouchEventListener( touchFunC )
    local btn = spbg:getChildByName("btn_tel")
    btn:setTag(TAG_ENUM.TAG_TEL)
    btn:addTouchEventListener( touchFunC )

    -- qq
    self.m_txtQQ = spbg:getChildByName("txt_qq")
    -- image = spbg:getChildByName("image_qq")
    -- image:setTag(TAG_ENUM.TAG_QQ)
    -- image:addTouchEventListener( touchFunC )
    btn = spbg:getChildByName("btn_qq")
    btn:setTag(TAG_ENUM.TAG_QQ)
    btn:addTouchEventListener( touchFunC )

    -- wx
    self.m_txtWX = spbg:getChildByName("txt_wx")
    -- image = spbg:getChildByName("image_wx")
    -- image:setTag(TAG_ENUM.TAG_WX)
    -- image:addTouchEventListener( touchFunC )
    btn = spbg:getChildByName("btn_wx")
    btn:setTag(TAG_ENUM.TAG_WX)
    btn:addTouchEventListener( touchFunC )

      -- 帮助
    btn = spbg:getChildByName("btn_help")
    btn:setTag(TAG_ENUM.BTN_HELP)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.img_shadow = spbg:getChildByName("img_shadow")
    self.img_shadow:setLocalZOrder(1)

    -- 刷新信息
    self:refreshInfo()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function ServiceLayer:onButtonClickedEvent(tag,sender)
	if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.TAG_TEL then
        local szText = GlobalUserItem.tabServiceCache["Phone"]
        if type(szText) == "string" and szText ~= "" then
            MultiPlatform:getInstance():systemCall(szText)
        end
    elseif tag == TAG_ENUM.TAG_QQ then
        local szText = GlobalUserItem.tabServiceCache["QQ"]
        if type(szText) == "string" and szText ~= "" then
            MultiPlatform:getInstance():copyToClipboard(szText)
        end
    elseif tag == TAG_ENUM.TAG_WX then
        local szText = GlobalUserItem.tabServiceCache["WeiXin"]
        if type(szText) == "string" and szText ~= "" then
            MultiPlatform:getInstance():copyToClipboard(szText)
        end
    elseif tag == TAG_ENUM.BTN_HELP then
        local hl = HelpLayer:create(self._scene, {}, self:getLevel() + 1)
        self._scene:addChild(hl)
	end
end

function ServiceLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function ServiceLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function ServiceLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ServiceLayer:refreshInfo()
    local qrcontent = yl.HTTP_URL
    local sztel = ""
    local szqq = ""
    local szwx = ""
    dump(GlobalUserItem.tabServiceCache, "ServiceLayer:refreshInfo", 5)
    if nil ~= GlobalUserItem.tabServiceCache then
        qrcontent = tostring(GlobalUserItem.tabServiceCache["Link"]) or ""
        local tmp = tostring(GlobalUserItem.tabServiceCache["Phone"]) or ""
        sztel = string.format("%s", tmp)
        tmp = tostring(GlobalUserItem.tabServiceCache["QQ"]) or ""
        szqq = string.format("%s", tmp)
        tmp = tostring(GlobalUserItem.tabServiceCache["WeiXin"]) or ""
        szwx = string.format("%s", tmp)
    end
    -- 电话
    self.m_txtTel:setString(sztel)
    -- qq
    self.m_txtQQ:setString(szqq)
    -- wx
    self.m_txtWX:setString(szwx)


    local imageName ="shareMa.png"
    local path = string.format("%sdownload/Service/%s", device.writablePath, imageName)
    
        downFileAsync(qrcontent, imageName, string.format("%sdownload/Service", device.writablePath), function(...)
                    if self.downLoadCallback then
                        self:downLoadCallback(...) 
                    end
            end)

    
    local tmpqr = self.m_spBg:getChildByName("tmp_qr")


        tmpqr:setTouchEnabled(true)
        tmpqr:addTouchEventListener(function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                ExternalFun.popupTouchFilter(0, false)

                --显示阴影
                self.img_shadow:setVisible(false)

                local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
                local xRate = framesize.width / yl.WIDTH 
                local yRate = framesize.height / yl.HEIGHT
                local bgsize = self.m_spBg:getContentSize()
                local screenPos = self.m_spBg:convertToWorldSpace(cc.p(623, 280))
                local framePosX = (screenPos.x - 100) * xRate
                local framePosY = (screenPos.y - 100) * yRate
                local frameWidth = 200 * xRate
                local frameHeight = 200 * yRate
                local qrFrameRect = cc.rect(framePosX, framePosY, frameWidth, frameHeight)

                captureScreenWithArea(qrFrameRect, "qr_code.png", function(ok, savepath)
                    ExternalFun.dismissTouchFilter()
                    if ok then
                        if MultiPlatform:getInstance():saveImgToSystemGallery(savepath, "qr_code.png") then
                            local runScene = cc.Director:getInstance():getRunningScene()
                            showToast(runScene, "二维码图片已保存至系统相册", 1)
                        end
                    end
                end)
            elseif tType == ccui.TouchEventType.began then
                --显示阴影
                self.img_shadow:setVisible(true)
            end
        end)

end


function ServiceLayer:downLoadCallback(main, sub)
    if main == appdf.DOWN_PRO_INFO then
    elseif main == appdf.DOWN_COMPELETED then
            local path = string.format("%sdownload/Service/shareMa.png", device.writablePath)
            if cc.FileUtils:getInstance():isFileExist(path) then
                local img = cc.Sprite:create(path)
                if img then     
                    local bg= self.m_spBg:getChildByName("tmp_qr")
                    img:setPosition(0,0)
                    img:setAnchorPoint(cc.p(0,0))
                    img:setScale(182/img:getContentSize().width)
                    bg:addChild(img)

                end
            end
    end
end


return ServiceLayer