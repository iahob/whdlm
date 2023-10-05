--
-- Author: zhong
-- Date: 2017-05-27 10:01:40
--
-- 大厅广告(二级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PlazaAdLayer = class("PlazaAdLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local UIPageView = appdf.req(appdf.EXTERNAL_SRC .. "UIPageView")

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
function PlazaAdLayer:ctor( scene, param, level )
    PlazaAdLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/PlazaAdLayer.csb", self)
    self.m_rootLayer = rootLayer

    -- 按钮事件
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
    btn:setLocalZOrder(1)
    btn:setPressedActionEnabled(true)

    -- pageview
    local tmp = spbg:getChildByName("content")
    self.m_PageView = UIPageView.new {
        viewRect = cc.rect(0, 0, tmp:getContentSize().width, tmp:getContentSize().height),
        padding = {left = 0, right = 0, top = 0, bottom = 0},
        bCirc = true}
        :onTouch(handler(self, self.onTouchPageListener))
        :setPosition(tmp:getPosition())
        :addTo(tmp)
    --tmp:removeFromParent()
    -- 

    -- 移除监听
    self.m_removeCallBack = nil
    -- 加载动画
    self:scaleTransitionIn(self.m_spBg)
end

function PlazaAdLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PlazaAdLayer:onTransitionInFinish()
    local pageviewsize = self.m_PageView:getContentSize()
    
    local alert = GlobalUserItem.tabAdConfig["adsAlertList"]
    if type(alert) == "table" then
        local list = alert["list"]
        if type(list) == "table" then
            local count = alert["count"] or 0
            local showcount = 0
            for i = 1, count do
                local item = self.m_PageView:newItem()
                local adimage = list[i]["res"] or ""
                adimage = cc.FileUtils:getInstance():fullPathForFilename("ads/alert/" .. adimage)
                if cc.FileUtils:getInstance():isFileExist(adimage) then
                    if string.match(string.lower(adimage), "gif") then
                        local cacheGif = createCacheGif(cc.FileUtils:getInstance():fullPathForFilename(adimage))
                        cacheGif:setPosition(cc.p(pageviewsize.width * 0.5, pageviewsize.height * 0.5))  
                        item:addChild(cacheGif)
                    else
                        local imageView = ccui.ImageView:create(adimage)
                        imageView:setPosition(cc.p(pageviewsize.width * 0.5, pageviewsize.height * 0.5))  
                        item:addChild(imageView)
                    end
                    showcount = showcount + 1
                end
                self.m_PageView:addItem(item)    
            end
            local touchEnable = showcount > 1
            self.m_PageView:setTouchEnabled(touchEnable)
            if touchEnable then
                self.m_PageView:setIndicatorEnabled(true, "plaza/plaza_sp_indicator_0.png", "plaza/plaza_sp_indicator_1.png")
                self.m_PageView:setIndicatorPosition(pageviewsize.width * 0.5, 20)
            end
        end
    end
    self.m_PageView:reload()
end

function PlazaAdLayer:onTouchPageListener( event )
    --dump(event, "onTouchPageListener - event:")
end

function PlazaAdLayer:onButtonClickedEvent(tag, ref)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    end
end

function PlazaAdLayer:onTransitionOutFinish()
    if type(self.m_removeCallBack) == "function" then
        self.m_removeCallBack()
    end
    self:removeFromParent()
end

return PlazaAdLayer