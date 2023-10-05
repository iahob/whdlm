local ShopLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ShopLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local uiBase = appdf.req(appdf.EXTERNAL_SRC .. "uiBase")

local ACT_TYPE = {
    image = 1,  --图片
    notice = 2, --公告
}

local TAG_MASK = 10086

local uiActivity = uiBase:ctor{
    zorder = 1000,
    needMoveInAction = true,
}

function uiActivity:ctor(...)
    if self:init(...) then
        self:registerNodeEvent()
        self.downLoadCount = 0
        return self
    end
end

function uiActivity:init(parent, plazalayer)
    if parent:getChildByName("uiActivity_LayerActivity.csb") then
        parent:removeChildByName("uiActivity_LayerActivity.csb")
    end

    local csbNode = cc.CSLoader:createNode("activity/LayerActivity.csb")
    csbNode:setName("uiActivity_LayerActivity.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode
    self._imageBg = cc.find(csbNode, "Image_bg")
    self.plazalayer = plazalayer

    cc.funAddChildListener(self._imageBg, "Button_1", function() self:onClickQuit() end)
    -- cc.funAddChildListener(csbNode, "Sprite_4/Panel_1", function() self:onClickNotice() end)
    -- cc.funAddChildListener(csbNode, "Sprite_4/Panel_2", function() self:onClickAct() end)
    -- cc.funAddChildListener(csbNode, "Panel_image/Button_4", function() self:onClickJump() end)
    cc.funAddChildListener(self._imageBg, "Panel_image/Image_1", function() self:onClickJump() end)
    
    --cc.funAddChildListener(self.widget, "Panel_3", function() self:onClickQuit() end)
    self._mask = cc.find(self.widget, "Panel_3")

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            if ref:getTag() == TAG_MASK then
                if self._opEnable then
                    self:destroy()
                end
            end        
        end
    end
    self._mask:addTouchEventListener( touchFunC )
    self._mask:setTouchEnabled(true)
    self._mask:setTag(TAG_MASK)

    self._opEnable = false

    cc.find(self._imageBg, "Panel_image/Image_1"):setVisible(false)
    self.listView = cc.find(self._imageBg, "ListView_1")
    self.listView:setScrollBarEnabled(false)
    self.item = cc.find(self._imageBg, "Button_left_item")
    self.item:setTag(1)
    self.item:retain()
    self.item:setVisible(false)

    self:refresh(GlobalUserItem.getAwardData())

    return self
end

function uiActivity:selectType(_type)
    -- cc.find(self.widget, "Sprite_4/Button_1"):setVisible(_type == ACT_TYPE.notice)
    -- cc.find(self.widget, "Sprite_4/Button_2"):setVisible(_type == ACT_TYPE.image)
    cc.find(self._imageBg, "Panel_text"):setVisible(_type == ACT_TYPE.notice)
    -- --cc.find(self.widget, "Panel_image"):setVisible(_type == ACT_TYPE.image)

    -- cc.find(self._imageBg, "Panel_image/Image_1"):setVisible(false)
    self._selectType = _type
end

function uiActivity:onClickQuit()
    if self.m_webView then
        self.m_webView:removeFromParent()
        self.m_webView = nil
    else
        if self._opEnable then
            self:destroy()
        end
    end
end

function uiActivity:onClickJump()
    print("点击跳转", self._curSelectIdx)
    if self._selectType ~= ACT_TYPE.image then
        return
    end
    local data = self.activtList[self._curSelectIdx]
    print("跳转到==", data.LinkURL)
    if not data.LinkURL or data.LinkURL == "" then
        print("跳转类型有问题=============")
        return
    end

    local idx = tonumber(data.LinkURL)
    if idx then
        if idx == 1 then --签到
        elseif idx == 2 then --分享
            self.plazalayer:popSpread()
        elseif idx == 3 then --比赛
            if MatchRoom then
                self.plazalayer._scene:onChangeShowMode(yl.SCENE_MATCH)
            end
        elseif idx == 4 then --商城
            self.plazalayer:popShopLayer(ShopLayer.DIAMOND_CHARGE)
        end

        self:destroy()
    else--web页
        if device.platform == "android" or device.platform == "ios" then
            local width = display.width*0.7
            local height = display.height*0.7

            self.m_webView = ccexp.WebView:create()
            self.m_webView:setAnchorPoint(cc.p(0.5,0.5))
            self.m_webView:setPosition(display.width*0.5, display.width*0.5-height*0.5)
            self.m_webView:setContentSize(width,height)
            self.m_webView:setScalesPageToFit(true)
            display.getRunningScene():addChild(self.m_webView)
        end
    end
end

function uiActivity:onClickNotice()
    self:selectType(ACT_TYPE.notice)
    self:refreshActivitList(self.activtList, self.noticeList)
end

function uiActivity:onClickAct()
    self:selectType(ACT_TYPE.image)
    self:refreshActivitList(self.activtList, self.noticeList)
end

-- local dataList_test = {
--     {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
--     {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
--     {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
--     {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
--     {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
--     {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
--     {tittle="张三", atype=1, timeStamp=os.time(), ctx="答题有奖活动!!", jumpto=1},
-- }

function uiActivity:refreshActivitList(dataList_act, dataList_notice)
    self._curSelectIdx = self._curSelectIdx or 1
    local dataList

    if self._selectType == ACT_TYPE.notice then
        dataList = dataList_notice
        if not dataList[self._curSelectIdx] then
            self._curSelectIdx = 1
        end
        self:refreshNoticeSingle(dataList[self._curSelectIdx])
    else
        dataList = dataList_act
        if not dataList[self._curSelectIdx] then
            self._curSelectIdx = 1
        end
        if dataList[self._curSelectIdx] then
            self:refreshActivitySingle(dataList[self._curSelectIdx])
        end
    end

    self.listView:removeAllItems()
    for idx,data in ipairs(dataList) do
        local item = self.item:clone()
        self.listView:pushBackCustomItem(item)
        item:setTag(idx)
        item:setVisible(true)
        item:addClickEventListener(function(sender)
            -- sender:setHighlighted(true)
            sender:loadTextures("activity/img_left_item_choose.png", "activity/img_left_item_choose.png", "activity/img_left_item_choose.png")
            self:onClickItem(sender:getTag())
        end)
        self:refreshActivityItem(item, data)
    end
    self:updateSelect()
end

function uiActivity:refreshActivityItem(btn, data)
    btn:getChildByName("Text_1"):setString(data.NoticeTitle or data.Title)
end

function uiActivity:updateSelect()
    --刷新标题
    local items = self.listView:getItems()
    local items_count = table.getn(items)
    for i=1,items_count do
        local item = self.listView:getItem(i-1)
        -- item:setHighlighted(self._curSelectIdx == item:getTag())
        if self._curSelectIdx == item:getTag() then
            item:loadTextures("activity/img_left_item_choose.png", "activity/img_left_item_choose.png", "activity/img_left_item_choose.png")
        else
            item:loadTextures("activity/img_left_item.png", "activity/img_left_item.png", "activity/img_left_item.png")
        end
    end
end

function uiActivity:refreshNoticeSingle(data)
    local Text_Tittle = cc.find(self._imageBg, "Panel_text/Text_Tittle")
    local Text_time = cc.find(self._imageBg, "Panel_text/Text_time")
    local Text_ctx = cc.find(self._imageBg, "Panel_text/Text_ctx")
    Text_Tittle:setString(data.NoticeTitle)
    Text_time:setString(data.PublisherTime)
    Text_ctx:setString(data.MoblieContent)
end

function uiActivity:refreshActivitySingle(data)
    local fileUtil = cc.FileUtils:getInstance()

    local path = string.format("%sdownload/activity/%s", device.writablePath, data.imageName)
    if fileUtil:isFileExist(path) then
        local sp = cc.find(self._imageBg, "Panel_image/Image_1")
        sp:loadTexture(path)
        sp:setVisible(true)
        local size = sp:getContentSize()
        sp:setScale(890/size.width, 500/size.height)
    else
        print("文件不存在==>", path)
    end
end

function uiActivity:onClickItem(whichOne)
    self._curSelectIdx = whichOne
    self:updateSelect()
    self:refreshActivitList(self.activtList, self.noticeList)
end

function uiActivity:refresh(activityList, noticeList)
    --先隐藏
    self:hide()

    self.downLoadCount = 0
    local needDownList = {}
    local fileUtil = cc.FileUtils:getInstance()
    self.activtList = {}
    self.noticeList = noticeList

    for idx,data in pairs(activityList) do
        if data.ResourceURL and data.ResourceURL ~= "" then
            local imageName = ExternalFun.getFileName(data.ResourceURL)
            local path = string.format("%sdownload/activity/%s", device.writablePath, imageName)
            if not fileUtil:isFileExist(path) then
                needDownList[imageName] = data.ResourceURL
                self.downLoadCount = self.downLoadCount + 1
                print("refresh--------->downLoadCount",self.downLoadCount)
            end

            local act = {}
            act.imageName = imageName
            act.LinkURL = data.LinkURL --数字代表跳转界面,链接打开网页
            act.SortID = data.SortID
            act.NoticeTitle = data.Title
            table.insert(self.activtList, act)
        end
    end

    if false then
        local downFileAsync = downFileAsync
        for filename,url in pairs(needDownList) do
            downFileAsync(url, filename, string.format("%sdownload/activity", device.writablePath), function(...)self:downLoadCallback(...) end)
        end
    else
        self:selectType(ACT_TYPE.notice)
        self:refreshActivitList(self.activtList, self.noticeList)
        self:show()
    end
end

function uiActivity:downLoadCallback(main, sub)
    if main == appdf.DOWN_PRO_INFO then
    elseif main == appdf.DOWN_COMPELETED then
        self.downLoadCount = self.downLoadCount - 1

        print("下载完成",main, sub, self.downLoadCount)
        --下载完毕,刷新
        if self.downLoadCount <= 0 then
            self:selectType(ACT_TYPE.notice)
            self:refreshActivitList(self.activtList, self.noticeList)
            self:show()
        end
    else
        --超时
        if sub == 28 then
            print("Time out!!!")
        end
        self:destroy()
    end
end

function uiActivity:onShow()
    --local panel = self.widget:getChildByName("Panel_3")
    print("onShow@@@@@@@@@@@@@@@@@@@@@@@onShow")
    self._opEnable = true
end

function uiActivity:setRemoveListener(callback)
    self._removeCallback = callback
end

function uiActivity:setdelayTime(delayTime)
    self._delayTime = delayTime or 0
end

function uiActivity:moveIn()
    local actionNode = self._imageBg
    actionNode:setAnchorPoint(cc.p(0.5, 0.5))
    actionNode:setPosition(display.width*0.5, display.height*0.5)

    actionNode:setScale(0.8)
    actionNode:setOpacity(0)

    local delay = cc.DelayTime:create(self._delayTime or 2.5)
    local scale = cc.ScaleTo:create(0.2, 1)
    local fadeIn = cc.FadeIn:create(0.8)
    local callback = cc.CallFunc:create(function() self:onShow() end)
    local show = cc.Spawn:create(scale, fadeIn)
    local seq = cc.Sequence:create(cc.Hide:create(), delay, cc.Show:create(), show, callback)

    actionNode:stopAllActions()
    actionNode:runAction(seq)
end

function uiActivity:onExit()
    self.downLoadCount = 0
    self.item:release()
    if self.m_webView then
        self.m_webView:removeFromParent()
        self.m_webView = nil
    end

    if type(self._removeCallback) == "function" then
        self._removeCallback()
    end
end

return uiActivity