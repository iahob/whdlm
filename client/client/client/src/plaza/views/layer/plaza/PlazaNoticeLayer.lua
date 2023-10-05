--
-- Author: zhong
-- Date: 2017-05-23 16:33:48
--
-- 大厅消息(二级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PlazaNoticeLayer = class("PlazaNoticeLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

-- ui
local ItemBuyLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ItemBuyLayer")              -- 购买
-- ui

local QueryQueueFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.QueryQueueFrame")             -- 查询服务

local BTN_CLOSE = 101   -- 关闭按钮
local TAG_MASK = 102    -- 遮罩
local BTN_SEND = 103    -- 发送
local IMAGE_EDIT = 104  -- 编辑
local BTN_BUY = 105  -- 购买
function PlazaNoticeLayer:ctor( scene, param, level )
    PlazaNoticeLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("service/PlazaNoticeLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --购买
    local btn = spbg:getChildByName("btn_dot")
    btn:setTag(BTN_BUY)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)




    -- 使用提示
    --self.m_spUserTips = spbg:getChildByName("sp_trumpetuse")

    -- 大喇叭数量
    self.m_txtTrumpetCount = spbg:getChildByName("txt_trumpetcount")
    if GlobalUserItem.nLargeTrumpetCount > 9999 then
        self.m_txtTrumpetCount:setString("9999")
    else
        self.m_txtTrumpetCount:setString(GlobalUserItem.nLargeTrumpetCount .. "")
    end

    
    self._queryFrame = QueryQueueFrame:create(self, function(code, msg)
            if GlobalUserItem.nLargeTrumpetCount > 9999 then
                self.m_txtTrumpetCount:setString("9999")
            else
                self.m_txtTrumpetCount:setString(GlobalUserItem.nLargeTrumpetCount .. "")
            end
        end)
        -- 喇叭查询
    local trumpet = 
    {
        nOprateCode = QueryQueueFrame.OP_ENUM.OP_QUERYTRUMPET,
        szServer = yl.LOGONSERVER,
        nPort = yl.LOGONPORT
    }
    self._queryFrame:addToQueryList(trumpet)
    -- 开始查询
    self._queryFrame:startQuery()


    -- 编辑区
    self.m_editTrumpet = nil
    self.m_bEditInput = false
    -- btn = spbg:getChildByName("edit_bg_1")
    -- btn:setTag(IMAGE_EDIT)
    -- btn:addTouchEventListener( touchFunC )

    -- 发送按钮
    btn = spbg:getChildByName("btn_send")
    btn:setTag(BTN_SEND)
    btn:setEnabled(false)
    btn:setPressedActionEnabled(true)
    btn:addTouchEventListener( touchFunC )
    self.m_btnSend = btn

    self.m_tabCellHeightCache = {}
    self.m_tabList = {} --param
    self.m_nListCount = 0 --#param

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 刷新公告列表
function PlazaNoticeLayer:refreshNoticeList( tabNotice )
    if type(tabNotice) ~= "table" then
        dump(tabNotice, "PlazaNoticeLayer:refreshNoticeList 参数非法", 6)
        return
    end
    self.m_tabList = FriendMgr:getInstance():getUserSystemRecordMsg()
    self.m_nListCount = #self.m_tabList
    if nil ~= self._listView then
        self._listView:reloadData()
    end

    -- 刷新喇叭数
    if GlobalUserItem.nLargeTrumpetCount > 9999 then
        self.m_txtTrumpetCount:setString("9999")
    else
        self.m_txtTrumpetCount:setString(GlobalUserItem.nLargeTrumpetCount .. "")
    end
end

-- 按键监听
function PlazaNoticeLayer:onButtonClickedEvent(tag,sender)
    -- local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    -- if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
    --     self.m_bEditInput = false
    --     -- editbox 隐藏键盘时屏蔽按钮
    --     return
    -- end
    if tag == TAG_MASK or tag == BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif tag == BTN_SEND then
        if GlobalUserItem.nLargeTrumpetCount < 1 then
            local item = nil
            for i=1, #GlobalUserItem.tabPropertyItem do
                if 306 == GlobalUserItem.tabPropertyItem[i].dwPropID and 0 == GlobalUserItem.tabPropertyItem[i].bExchangeType then
                    item = GlobalUserItem.tabPropertyItem[i]
                    break
                end
            end
            if not item then
                showToast(self, "没有喇叭", 2)
                return
            end


            local ib = ItemBuyLayer:create(self._scene, {}, self:getLevel() + 1)
            self._scene:addPopLayer(ib)
            return
        end
        local content = self.m_editTrumpet:getText()
        local isEmpty = string.gsub(content, "^%s*(.-)%s*$", "%1")--去掉空格
        if string.len(isEmpty) <1 then
            showToast(self, "喇叭内容不能为空", 3)
        else
            --判断emoji
            if ExternalFun.isContainEmoji(content) then
                showToast(self, "喇叭内容包含非法字符,请重试", 2)
                return
            end

            --敏感词过滤  
            if true == ExternalFun.isContainBadWords(content) then
                showToast(self, "喇叭内容包含敏感词汇!", 3)
                return
            end 

            local msgTab = {}
            msgTab.dwSenderID = GlobalUserItem.tabAccountInfo.dwUserID
            msgTab.szNickName = GlobalUserItem.tabAccountInfo.szNickName
            msgTab.dwFontColor = 16777215
            msgTab.cbFontSize = 0
            msgTab.cbFontAttri = 0
            msgTab.szFontName = "微软雅黑"
            msgTab.szMessageContent = content
            --喇叭使用后数量控制在使用结果处理
            FriendMgr:getInstance():sendTrupmet(msgTab)

            --self:scaleTransitionOut(self.m_spBg)
            self.m_editTrumpet:setText("")
        end 
    elseif tag == IMAGE_EDIT then
        if nil ~= self.m_editTrumpet then
            self.m_editTrumpet:touchDownAction(self.m_editTrumpet, ccui.TouchEventType.ended)
        end
    elseif tag == BTN_BUY then

        local item = nil
            for i=1, #GlobalUserItem.tabPropertyItem do
                if 306 == GlobalUserItem.tabPropertyItem[i].dwPropID and 0 == GlobalUserItem.tabPropertyItem[i].bExchangeType then
                    item = GlobalUserItem.tabPropertyItem[i]
                    break
                end
            end
            if not item then
                showToast(self, "没有配置喇叭购买", 2)
                return
            end

        local ib = ItemBuyLayer:create(self._scene, {}, self:getLevel() + 1)
        self._scene:addPopLayer(ib)
    end
end

function PlazaNoticeLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PlazaNoticeLayer:onTransitionInFinish()
    self.m_tabList = FriendMgr:getInstance():getUserSystemRecordMsg()
    self.m_nListCount = #self.m_tabList --#param

    -- 公告列表
    local tmp = self.m_spBg:getChildByName("content")
    self._listView = cc.TableView:create(tmp:getContentSize())
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.m_spBg)
    self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    tmp:removeFromParent()
    self._listView:reloadData()

    -- 编辑框
    local tmp = self.m_spBg:getChildByName("edit_bg")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入信息,每条消耗大喇叭X1")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFontName("fonts/round_body.ttf") 
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE) 
            :setMaxLength(64)
    self.m_spBg:addChild(editbox)         
    self.m_editTrumpet = editbox
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.m_btnSend:setEnabled(true)
end

function PlazaNoticeLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function PlazaNoticeLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function PlazaNoticeLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
        if GlobalUserItem.nLargeTrumpetCount > 9999 then
            self.m_txtTrumpetCount:setString("9999")
        else
            self.m_txtTrumpetCount:setString(GlobalUserItem.nLargeTrumpetCount .. "")
        end
    end
end

function PlazaNoticeLayer:tableCellTouched(view, cell)

end

-- 子视图大小
function PlazaNoticeLayer:cellSizeForTable(view, idx)
    local cacheHeight = nil --self.m_tabCellHeightCache[idx + 1]
    if nil ~= cacheHeight then
        return view:getViewSize().width, cacheHeight + 10 -- +10为间隔
    end

    local notice = self.m_tabList[idx + 1]
    if nil ~= notice then
        -- 背景
        local imagesize = cc.size(882, 112) -- service/notice_sp_cellbg.png 尺寸

        local msgType = notice.ntype or yl.SYSTEM_MSG
        local contentwidth = 800
        -- 调整标题图
        if msgType == yl.TRUMPET_MSG then
            contentwidth = 800
        end

        -- 内容
        local txtContent = cc.Label:createWithTTF(notice.content, "fonts/FZHTJW.TTF", 24, cc.size(contentwidth,0), cc.TEXT_ALIGNMENT_LEFT)
        txtContent:setLineBreakWithoutSpace(true)

        -- +40 为空隙 +36 为标题高度
        local adjHeight = txtContent:getContentSize().height + 40
        if adjHeight < imagesize.height then
            adjHeight = imagesize.height
        end

        self.m_tabCellHeightCache[idx + 1] = adjHeight
        return view:getViewSize().width, adjHeight + 4 -- +10为间隔
    else
        self.m_tabCellHeightCache[idx + 1] = 116
        return view:getViewSize().width, 116
    end
end

function PlazaNoticeLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local notice = self.m_tabList[idx + 1]
    if nil == notice then
        return cell
    end
    -- 消息类型
    local msgType = notice.ntype or yl.SYSTEM_MSG

    local bSysMsg=  msgType == yl.SYSTEM_MSG
    print("消息类型==================================================》", msgType)
    dump(notice, "消息详情", 10)

    local csb = ExternalFun.loadCSB("service/PlazaNoticeItemNode.csb", cell)

    -- 背景
    local imagecell = csb:getChildByName("image_bg")--ccui.ImageView:create()
    imagecell:setScale9Enabled(true)
    local imagesize = imagecell:getContentSize()

    -- 时间背景
    local timebg = imagecell:getChildByName("image_title") --ccui.ImageView:create("service/notice_sp_cellbg.png")
    --昵称背景
    local nickbg = imagecell:getChildByName("image_nickbg")
    if bSysMsg then
        nickbg:loadTexture("service/notice_nickbg2.png")
    else
        nickbg:loadTexture("service/notice_nickbg1.png")
    end
    -- timebg:setScale9Enabled(true)
    -- local timebgsize = timebg:getContentSize()    

    -- 时间
    local txtDate = cc.Label:createWithTTF(notice.time, "fonts/FZHTJW.TTF", 20)
    imagecell:addChild(txtDate)
    --txtDate:setColor(cc.c3b(0,0,0))
    txtDate:setAnchorPoint(cc.p(0, 0.5))
    local dateSize = txtDate:getContentSize()

    if bSysMsg then
        txtDate:setColor(cc.c4b(0xff,0xe8,0x7f,0xff))
    else
        txtDate:setColor(cc.c4b(0xea,0xd9,0xff,0xff))
    end

    -- 标题
    local txtTitle = cc.Label:createWithTTF(notice.title, "fonts/FZHTJW.TTF", 20)
    local length = txtTitle:getContentSize().width
    nickbg:setContentSize(cc.size(length +15, 28))

    --local txtTitle = ClipText:createClipText(cc.size(350, 30), notice.title, nil, 20) 
    nickbg:addChild(txtTitle)
    txtTitle:setAnchorPoint(cc.p(0, 0.5))
    local clipTextSize = txtTitle:getContentSize()
    local txtsay = nil
    local contentwidth = 800
    -- 调整标题图
    if msgType == yl.SYSTEM_MSG then
        txtTitle:setColor(cc.c3b(0xff, 0xe8, 0x7f))
        --timebg:loadTexture("service/notice_sp_cellbg_2.png")
    elseif msgType == yl.TRUMPET_MSG then
        txtTitle:setColor(cc.c3b(0x9e, 0xe9, 0xf7))
        --timebg:loadTexture("service/notice_sp_cellbg_1.png")
        txtsay = cc.Label:createWithTTF("说:", "fonts/FZHTJW.TTF", 20)
        txtsay:setColor(cc.c4b(0xea,0xd9,0xff,0xff))
        imagecell:addChild(txtsay)
        txtsay:setAnchorPoint(cc.p(0, 0.5))
        contentwidth = 800
    end

    -- 内容
    local txtContent = cc.Label:createWithTTF(notice.content, "fonts/FZHTJW.TTF", 24, cc.size(contentwidth,0), cc.TEXT_ALIGNMENT_LEFT)
    txtContent:setLineBreakWithoutSpace(true)
    imagecell:addChild(txtContent)
    txtContent:setAnchorPoint(cc.p(0, 0.5))
    local contentSize = txtContent:getContentSize()

    if bSysMsg then
        txtContent:setColor(cc.c4b(0xff,0xe8,0x7f,0xff))
    else
        txtContent:setColor(cc.c4b(0xea,0xd9,0xff,0xff))
    end

    -- +40 为空隙
    local adjHeight = contentSize.height + 40
    if adjHeight < imagesize.height then
        adjHeight = imagesize.height
    end
    local adjContentSize = cc.size(imagesize.width, adjHeight)
    imagecell:setContentSize(adjContentSize)

    -- 调整位置
    txtTitle:setPosition(8, 14)
    if nil ~= txtsay then
        txtsay:setPosition(clipTextSize.width + 130, nickbg:getPositionY())
        txtDate:setPosition(clipTextSize.width + 40, nickbg:getPositionY())
        txtContent:setPosition(50, 40)
    else
        txtContent:setPosition(50, 40)
        txtDate:setPosition(clipTextSize.width + 40, nickbg:getPositionY())
    end
    -- timebg:setPosition(0, adjHeight)
    -- timebg:setContentSize(cc.size(160 + clipTextSize.width, timebgsize.height))
    -- timebg:setAnchorPoint(cc.p(0, 1.0))

    imagecell:setPosition(view:getViewSize().width * 0.5, imagecell:getContentSize().height +2)
    return cell
end

-- 子视图数目
function PlazaNoticeLayer:numberOfCellsInTableView(view)
    return self.m_nListCount
end

function PlazaNoticeLayer:scrollViewDidScroll(view)

end

function PlazaNoticeLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditInput = string.len(src) ~= 0
        --self.m_spUserTips:setVisible(false)
    elseif event == "changed" then
        self.m_bEditInput = string.len(src) ~= 0
    elseif event == "return" then
        --self.m_spUserTips:setVisible(true)
    end
end

function PlazaNoticeLayer:getEmptySpace( sz )

end

return PlazaNoticeLayer