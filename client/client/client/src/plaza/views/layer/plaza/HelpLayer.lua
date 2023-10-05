--
-- Author: zhong
-- Date: 2017-05-24 20:05:54
--
-- 帮助界面(三级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local HelpLayer = class("HelpLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local BTN_CLOSE = 101   -- 关闭按钮
local TAG_MASK = 102    -- 遮罩
function HelpLayer:ctor( scene, param, level )
    HelpLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("service/HelpLayer.csb", self)
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
    btn:setPressedActionEnabled(true)

    self.m_tabCellHeightCache = {}
    self.m_tabList = {}
    self.m_nListCount = 0

    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function HelpLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_MASK or tag == BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    end
end

function HelpLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function HelpLayer:onTransitionInFinish()
    --self:getData()
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

    -- 领取奖励
    self:showPopWait()
    local param = "action=getquestionandanswerlist&" .. GlobalUserItem:urlUnLoginSignParam()
    print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..param)
    local jsonStr = cjson.encode(action)
    LogAsset:getInstance():logData(jsonStr, true)
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "jstable", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    local list = data["list"]
                    if type(list) == "table" then
                        self.m_tabList = {}
                        for i=1,#list do
                            local infoItem = {}
                            local info = list[i]
                            infoItem.time = info["UpdateAt"]
                            infoItem.question = info["QuestionTitle"]
                            infoItem.answer = info["Answer"]
                            infoItem.sortid = info["SortID"]

                            table.insert(self.m_tabList, infoItem)
                        end
                    end
                    --排序
                    table.sort(self.m_tabList, function (a, b)
                        return a.sortid < b.sortid
                    end )
                    self.m_nListCount = #self.m_tabList

                    self._listView:reloadData()
                end
            end
        end
        if type(msg) == "string" and "" ~= msg then
            showToast(self._scene, msg, 3)
        end
        self:dismissPopWait()
    end) 

end

function HelpLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function HelpLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function HelpLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function HelpLayer:tableCellTouched(view, cell)

end

-- 子视图大小
function HelpLayer:cellSizeForTable(view, idx)
    local cacheHeight = self.m_tabCellHeightCache[idx + 1]
    if nil ~= cacheHeight then
        return view:getViewSize().width, cacheHeight + 10 -- +10为间隔
    end

    local qa = self.m_tabList[idx + 1]
    if nil ~= qa then
        -- 背景
        local imagecell = ccui.ImageView:create("service/help_sp_cellbg.png")
        local imagesize = imagecell:getContentSize()
        -- 内容
        local txtContent = cc.Label:createWithTTF(qa.answer, "fonts/round_body.ttf", 24, cc.size(695,0), cc.TEXT_ALIGNMENT_LEFT)
        txtContent:setLineBreakWithoutSpace(true)
        -- +30 为空隙 +30 为标题高度
        local adjHeight = txtContent:getContentSize().height + 60
        if adjHeight < imagesize.height then
            adjHeight = imagesize.height
        end
        self.m_tabCellHeightCache[idx + 1] = adjHeight
        return view:getViewSize().width, adjHeight + 10 -- +10为间隔
    else
        self.m_tabCellHeightCache[idx + 1] = 160
        return view:getViewSize().width, 160
    end
end


function HelpLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local qa = self.m_tabList[idx + 1]
    -- 背景
    local imagecell = ccui.ImageView:create("service/help_sp_cellbg.png")
    cell:addChild(imagecell)
    imagecell:setScale9Enabled(true)
    local imagesize = imagecell:getContentSize()

    -- 标号
    local spnum = cc.Sprite:create("service/help_sp_numbg.png")
    imagecell:addChild(spnum)
    local num = self.m_nListCount - idx
    local txtnum = cc.Label:createWithTTF(num .. "", "fonts/round_body.ttf", 36)
    txtnum:setColor(cc.c4b(0x71,0xfc,0xc3,0xff))
    txtnum:enableOutline(cc.c4b(0x71,0xfc,0xc3,0xff),1)
    spnum:addChild(txtnum)
    txtnum:setPosition(spnum:getContentSize().width * 0.5-5, spnum:getContentSize().height * 0.5+7)

    -- 问
    local txtQue =cc.Sprite:create("service/ask_icon.png")
    imagecell:addChild(txtQue)
    txtQue:setAnchorPoint(cc.p(0, 1.0))
    local txtTitle = cc.Label:createWithTTF(qa.question, "fonts/round_body.ttf", 24)
    txtTitle:setColor(cc.c4b(0x87,0xd3,0xea,0xff))
    imagecell:addChild(txtTitle)
    txtTitle:setAnchorPoint(cc.p(0, 1.0))
    local titleSize = txtTitle:getContentSize()

    --分隔线
    local splitLine=cc.Sprite:create("service/help_line.png")
    splitLine:setAnchorPoint(cc.p(0,0.5))
    imagecell:addChild(splitLine)

    -- 答
    local txtAns = cc.Sprite:create("service/answer_icon.png")
    imagecell:addChild(txtAns)
    txtAns:setAnchorPoint(cc.p(0, 1.0))
    local txtContent = cc.Label:createWithTTF(qa.answer, "fonts/round_body.ttf", 24, cc.size(695,0), cc.TEXT_ALIGNMENT_LEFT)
    txtContent:setLineBreakWithoutSpace(true)
    imagecell:addChild(txtContent)
    txtContent:setAnchorPoint(cc.p(0, 1.0))
    txtContent:setColor(cc.c4b(0xeb,0xd9,0x88,0xff))
    local contentSize = txtContent:getContentSize()

    -- +30 为空隙
    local adjHeight = titleSize.height + contentSize.height + 30
    if adjHeight < imagesize.height then
        adjHeight = imagesize.height
    end
    local adjContentSize = cc.size(imagesize.width, adjHeight)
    imagecell:setContentSize(adjContentSize)

    -- 调整位置
    spnum:setPosition(60, adjHeight * 0.5)
    txtQue:setPosition(100, adjHeight - 5)
    splitLine:setPosition(100,adjHeight-15-titleSize.height)
    txtTitle:setPosition(150, adjHeight - 10)
    txtAns:setPosition(100, adjHeight - 45)
    txtContent:setPosition(150, adjHeight - 50)
    imagecell:setPosition(view:getViewSize().width * 0.5, imagecell:getContentSize().height * 0.5)
    print("imagecell:getContentSize().width:",imagecell:getContentSize().width)
    print(imagecell:getPosition())

    return cell
end

-- 子视图数目
function HelpLayer:numberOfCellsInTableView(view)
    return self.m_nListCount
end

function HelpLayer:scrollViewDidScroll(view)

end

function HelpLayer:getData()
    self.m_tabList = {}
    table.insert(self.m_tabList, {
        time = "2017.02.05", 
        question = "如何获取元宝?", 
        answer = "如何获取元宝如何获取元宝如何获取元宝",
        })
    table.insert(self.m_tabList, {
        time = "2017.02.07", 
        question = "如何获取钻石?", 
        answer = "如何获取钻石如何获取钻石如何获取钻石",
        })
    table.insert(self.m_tabList, {
        time = "2017.02.07", 
        question = "如何获取房卡?", 
        answer = "如何获取钻石如何获取钻石如何获取钻石",
        })
    table.insert(self.m_tabList, {
        time = "2017.02.05", 
        question = "如何获取元宝1?", 
        answer = "如何获取元宝如何获取元宝如何获取元宝",
        })
    table.insert(self.m_tabList, {
        time = "2017.02.07", 
        question = "如何获取钻石1?", 
        answer = "如何获取钻石如何获取钻石如何获取钻石如何获取钻石如何获取钻石如何获取钻石如何获取钻石如何获取钻石如何获取钻石如何获取钻石如何获取钻石如何获取钻石",
        })
    table.insert(self.m_tabList, {
        time = "2017.02.07", 
        question = "如何获取房卡1?", 
        answer = "如何获取钻石如何获取钻石如何获取钻石",
        })
    table.insert(self.m_tabList, {
        time = "2017.02.05", 
        question = "如何获取元宝2222?", 
        answer = "如何获取元宝如何获取元宝如何获取元宝",
        })
    table.insert(self.m_tabList, {
        time = "2017.02.07", 
        question = "如何获取钻石22222?", 
        answer = "如何获取钻石如何获取钻石如何获取钻石",
        })
    table.insert(self.m_tabList, {
        time = "2017.02.07", 
        question = "如何获取房卡22222?", 
        answer = "如何获取钻石如何获取钻石如何获取钻石",
        })
    self.m_nListCount = #self.m_tabList
end

return HelpLayer