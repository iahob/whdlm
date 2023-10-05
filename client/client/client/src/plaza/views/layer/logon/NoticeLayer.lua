--
-- Author: zhong
-- Date: 2017-05-23 16:33:48
--
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local NoticeLayer = class("NoticeLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local BTN_CLOSE = 101   -- 关闭按钮
local TAG_MASK = 102    -- 遮罩
function NoticeLayer:ctor( scene, param, level )
    NoticeLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("service/NoticeLayer.csb", self)
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
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    --btn:setPressedActionEnabled(true)

    self.m_tabCellHeightCache = {}
    self.m_tabList = param
    self.m_nListCount = #param

    -- 加载动画
    --self:scaleTransitionIn(spbg)
    self:scaleShakeTransitionIn(spbg)
    
end

-- 按键监听
function NoticeLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_MASK or tag == BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    end
end

function NoticeLayer:onTransitionInFinish()
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
end

function NoticeLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function NoticeLayer:tableCellTouched(view, cell)

end

-- 子视图大小
function NoticeLayer:cellSizeForTable(view, idx)
    local cacheHeight = self.m_tabCellHeightCache[idx + 1]
    if nil ~= cacheHeight then
        return view:getViewSize().width, cacheHeight + 10 -- +20为间隔
    end

    local notice = self.m_tabList[idx + 1]
    if nil ~= notice then
        -- 背景
     
        local imagesize =cc.size(880, 80)
        -- 内容
        local msg = notice.MoblieContent or ""
        local txtContent = cc.Label:createWithTTF(msg, "fonts/round_body.ttf", 24, cc.size(840,0), cc.TEXT_ALIGNMENT_LEFT)
        txtContent:setLineBreakWithoutSpace(true)
        -- +40 为空隙 + 30 为标题高度
        local adjHeight = txtContent:getContentSize().height + 50
        if adjHeight < imagesize.height then
            adjHeight = imagesize.height
        end
        self.m_tabCellHeightCache[idx + 1] = adjHeight
        return view:getViewSize().width, adjHeight + 10 -- +20为间隔
    else
        self.m_tabCellHeightCache[idx + 1] = 80
        return view:getViewSize().width, 80
    end
end

function NoticeLayer:tableCellAtIndex(view, idx)
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

    -- 背景
    local imagecell = ccui.ImageView:create("service/notice_cellbg.png")
    imagecell:setScale9Enabled(true)
    cell:addChild(imagecell)
    imagecell:setContentSize(880,80)
    local imagesize = imagecell:getContentSize()

    -- 时间
    local szdate = ""
    print("公告时间", notice.PublisherTime)
    -- local date = tonumber(string.match(notice.PublisherTime,"(%d+)"))
    -- if nil ~= date then
    --     szdate = os.date("%Y-%m-%d", date / 1000)
    -- end

    local txtDate = cc.Label:createWithTTF(notice.PublisherTime, "fonts/round_body.ttf", 20)
    imagecell:addChild(txtDate)
    txtDate:setAnchorPoint(cc.p(1.0, 1.0))
    txtDate:setColor(cc.c3b(0xa6,0x7d,0xd7))
    local dateSize = txtDate:getContentSize()

   

    local titleBg=ccui.ImageView:create("service/notice_titlebg.png")
    imagecell:addChild(titleBg)
    titleBg:setScale9Enabled(true)
    titleBg:setAnchorPoint(cc.p(0.0, 1.0))

     -- 标题
    --local txtTitle = ClipText:createClipText(cc.size(438, 30), notice.NoticeTitle, nil, 20) 
     local strTitle = string.EllipsisByConfig(notice.NoticeTitle, 600, 
                                            string.getConfig("fonts/round_body.ttf", 20))
    local txtTitle = cc.Label:createWithTTF(strTitle,"fonts/round_body.ttf",20)
    imagecell:addChild(txtTitle)
    txtTitle:setAnchorPoint(cc.p(0.0, 1.0))
    txtTitle:setColor(cc.c3b(0xf9,0x98,0xf1))
    txtTitle:setPositionY(15)

    titleBg:setContentSize(txtTitle:getContentSize().width +10, 30)

    -- 内容
    local msg = notice.MoblieContent or ""
    local txtContent = cc.Label:createWithTTF(msg, "fonts/round_body.ttf", 24, cc.size(840,0), cc.TEXT_ALIGNMENT_LEFT)
    txtContent:setLineBreakWithoutSpace(true)
    --txtContent:setColor(cc.c4b(100,54,27,255))
    imagecell:addChild(txtContent)
    txtContent:setAnchorPoint(cc.p(0, 0.5))
    txtContent:setColor(cc.c3b(0xf2,0xde,0xff))
    local contentSize = txtContent:getContentSize()

    local splitLine=cc.Sprite:create("service/sysnotice_line.png"):addTo(imagecell)

    -- +40 为空隙
    local adjHeight = 30 + contentSize.height + 20
    if adjHeight < imagesize.height then
        adjHeight = imagesize.height
    end
    local adjContentSize = cc.size(imagesize.width, adjHeight)
    imagecell:setContentSize(adjContentSize)

    -- 调整位置
    txtDate:setPosition(860, adjHeight - 6)
    txtTitle:setPosition(15, adjHeight - 7)
    titleBg:setPosition(10, adjHeight - 3)
    splitLine:setPosition(450,adjHeight - 37)
    txtContent:setPosition(27, (adjHeight -37) * 0.5)
    imagecell:setPosition(view:getViewSize().width * 0.5, imagecell:getContentSize().height * 0.5)
    print("imagecell:getPosition",imagecell:getPosition())
    return cell
end

-- 子视图数目
function NoticeLayer:numberOfCellsInTableView(view)
    return self.m_nListCount
end

function NoticeLayer:scrollViewDidScroll(view)
end

return NoticeLayer