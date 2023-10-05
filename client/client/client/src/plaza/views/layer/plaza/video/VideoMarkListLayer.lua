--
-- Author: zhong
-- Date: 2017-05-26 15:55:56
--
-- 包含( 战绩列表-VideoMarkListLayer、 回放码输入-VideoCodeEditLayer)

-- 战绩列表(二级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local VideoMarkListLayer = class("VideoMarkListLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- ui
local VideoMarkDetailLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "video.VideoMarkDetailLayer")              -- 详情
-- ui

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_SEARCH",           -- 搜索
    "IMAGE_EDIT",           -- 编辑按钮
    "BTN_LAST_PAGE",        -- 上一页
    "BTN_NEXT_PAGE",        -- 下一页
    "BTN_CREATE_ROOM",      -- 创建房间
    "BOX_DAIKAI_ROOM",      -- 代开房间
    "BOX_MY_ROOM",      -- 我的房间

    "BTN_SCORE",        -- 积分约战
    "BTN_GOLD",         -- 金币约战
    "BTN_NORMAL",       -- 金币场
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

-- 回放码输入
local VideoCodeEditLayer = class("VideoCodeEditLayer", TransitionLayer)
VideoMarkListLayer.VideoCodeEditLayer = VideoCodeEditLayer
function VideoCodeEditLayer:ctor( scene, param, level )
    VideoCodeEditLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("video/VideoCodeEditLayer.csb", self)

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
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 查看
    btn = spbg:getChildByName("btn_search")
    btn:setTag(TAG_ENUM.BTN_SEARCH)
    btn:addTouchEventListener( touchFunC )
    btn:setEnabled(false)
    btn:setPressedActionEnabled(true)
    self.m_btnSearch = btn

    -- 编辑区
    self.m_editBox = nil
    self.m_bEditInput = false
    -- 编辑内容
    self.m_txtEdit = spbg:getChildByName("txt_edit")
    self.m_txtEdit:setString("")
    -- 编辑按钮
    btn = spbg:getChildByName("editbg")
    btn:setTag(TAG_ENUM.IMAGE_EDIT)
    btn:addTouchEventListener( touchFunC )
    self.m_imageEdit = btn
    self.m_imageEdit:setEnabled(false)

    self:scaleShakeTransitionIn(spbg)
end

function VideoCodeEditLayer:onButtonClickedEvent( tag, ref )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_SEARCH == tag then
        local backcode = self.m_editBox:getText()
        if string.len(backcode) == 6 and nil ~= tonumber(backcode) then
            local param = {}
            param.nBackCode = tonumber(backcode)
            param.nFrom = VideoMarkDetailLayer.FROM_BACKCODE
            local md = VideoMarkDetailLayer:create( self._scene, param, self:getLevel() )
            self._scene:addPopLayer(md)

            self:scaleTransitionOut(self.m_spBg)
        end
    elseif TAG_ENUM.IMAGE_EDIT == tag then
        self.m_editBox:setVisible(true)
        self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_imageEdit:setEnabled(false)
    end
end

function VideoCodeEditLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditInput = string.len(src) ~= 0
        self.m_txtEdit:setVisible(false)
    elseif event == "changed" then
        self.m_bEditInput = string.len(src) ~= 0
    elseif event == "return" then
        editbox:setVisible(false)
        self.m_txtEdit:setVisible(true)
        self.m_txtEdit:setString(src)
        self.m_imageEdit:setEnabled(true)
    end
end

function VideoCodeEditLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function VideoCodeEditLayer:onTransitionInFinish()
    -- 编辑框
    local tmp = self.m_imageEdit
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 100, tmp:getContentSize().height - 10),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("输入好友的回放码")
            :setPlaceholderFont("fonts/round_body.ttf", 24)
            :setFont("fonts/round_body.ttf", 24)  
            --:setFontColor(cc.c4b(0,0,0,255))
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    self.m_spBg:addChild(editbox)         
    self.m_editBox = editbox
    self.m_editBox:setVisible(false)
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_imageEdit:setEnabled(true)
    if type(self._param) == "string" and string.len(self._param) == 6 then
        self.m_editBox:setText(self._param)
        self:onButtonClickedEvent(TAG_ENUM.IMAGE_EDIT, self.m_imageEdit)
    end
    self.m_btnSearch:setEnabled(true)

end

function VideoCodeEditLayer:onTransitionOutFinish()
    self:removeFromParent()
end

-- 战绩列表
function VideoMarkListLayer:ctor( scene, param, level )
    local this = self

    VideoMarkListLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("video/VideoMarkListLayer.csb", self)

    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 战绩按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == this._listView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = this._listView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, this._listView:getViewSize().width, this._listView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                this:onItemButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:setTouchEnabled(true)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 查看
    btn = spbg:getChildByName("btn_search")
    btn:setTag(TAG_ENUM.BTN_SEARCH)
    btn:addTouchEventListener( touchFunC )
    btn:setEnabled(false)
    self.m_btnSearch = btn

    self.m_modeBtn = {}
    --积分约战
    local btn = spbg:getChildByName("btn_score")
    btn:setTag(TAG_ENUM.BTN_SCORE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(false)
    btn:setVisible(false)
    table.insert(self.m_modeBtn,btn)

    local btn = spbg:getChildByName("btn_gold")
    btn:setTag(TAG_ENUM.BTN_GOLD)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(false)
    btn:setVisible(false)
    table.insert(self.m_modeBtn,btn)

    local btn = spbg:getChildByName("btn_normal")
    btn:setTag(TAG_ENUM.BTN_NORMAL)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(false)
    btn:setVisible(false)
    table.insert(self.m_modeBtn,btn)

    self.m_curBtn = nil
    self:updateModeBtn()

    --选中提示
     self._spSelected = spbg:getChildByName("sp_selected")

     --不同场次node
    self._nodeScore = spbg:getChildByName("node_score")
    self._nodeGold = spbg:getChildByName("node_gold")
    self._nodeNormal = spbg:getChildByName("node_normal")

    -- 编辑区
    self.m_editBox = nil
    self.m_bEditInput = false
    -- 编辑按钮
    btn = spbg:getChildByName("editbg")
    btn:setTag(TAG_ENUM.IMAGE_EDIT)
    btn:addTouchEventListener( touchFunC )
    self.m_imageEdit = btn
    self.m_imageEdit:setEnabled(false)

    -- 编辑内容
    self.m_txtEdit = btn:getChildByName("txt_videocode")
    self.m_txtEdit:setString("输入好友的回放码")


    self.m_tabList = {}
    self.m_nListCount = 0
    -- 列表1(积分约战)
    local tmp = self._nodeScore:getChildByName("content")
    local tmpsize = tmp:getContentSize()
    self.m_nCellSize = 100
    -- 上下页按钮显示区域条件
    self.m_fPageRange = self.m_nCellSize * 0.5
    self._listView = cc.TableView:create(tmpsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self._nodeScore)
    --self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    --self._listView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    tmp:removeFromParent()
    self._listView:reloadData()

    --战绩列表
    self.m_tabGoldDetailList = {}  --金币约战
    self.m_tabNormalDetailList = {}    --金币场

    --金币场界面
    local tmp = self._nodeGold:getChildByName("content")
    self._listGoldView = cc.TableView:create(tmp:getContentSize())
    self._listGoldView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listGoldView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listGoldView:setPosition(tmp:getPosition())
    self._listGoldView:setDelegate()
    self._listGoldView:addTo(self._nodeGold)
    --self._listGoldView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listGoldView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listGoldView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listGoldView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    --self._listView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    tmp:removeFromParent()

    -- 金币战绩按钮
    self.m_itemGoldFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == this._listGoldView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = this._listGoldView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, this._listGoldView:getViewSize().width, this._listGoldView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                this:onItemGoldButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end
  
    --普通场界面
    local tmp = self._nodeNormal:getChildByName("content")
    self._listNormalView = cc.TableView:create(tmp:getContentSize())
    self._listNormalView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listNormalView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listNormalView:setPosition(tmp:getPosition())
    self._listNormalView:setDelegate()
    self._listNormalView:addTo(self._nodeNormal)
    --self._listGoldView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listNormalView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listNormalView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listNormalView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    --self._listView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    tmp:removeFromParent()

    -- 分页控制
    -- self.m_layPageControl = spbg:getChildByName("page_content")
    -- self.m_layPageControl:setLocalZOrder(1)
    -- self.m_layPageControl:setVisible(false)

    -- -- 上页
    -- btn = self.m_layPageControl:getChildByName("btn_lastpage")
    -- btn:setTag(TAG_ENUM.BTN_LAST_PAGE)
    -- btn:addTouchEventListener(touchFunC)
    -- self.m_btnLastPage = btn

    -- -- 下页
    -- btn = self.m_layPageControl:getChildByName("btn_nextpage")
    -- btn:setTag(TAG_ENUM.BTN_NEXT_PAGE)
    -- btn:addTouchEventListener(touchFunC)
    -- self.m_btnNextPage = btn

    -- 有下一页视频信息
    self.m_bHaveNextPageVideo = true
    -- 0:约战场 1:普通场
    self.m_nQueryType = GameVideo.DEFINE.QUERY_PRIVATE_SCORE_VIDEO--GameVideo.DEFINE.QUERY_NORMAL_VIDEO --GameVideo.DEFINE.QUERY_PRIVATE_SCORE_VIDEO
    -- 切换查询类型
    GameVideo:getInstance():changeQueryType(self.m_nQueryType)

    -- 创建提示
    -- self.m_layTipControl = spbg:getChildByName("tip_content")
    -- self.m_layTipControl:setVisible(false)
    -- -- 创建按钮
    -- btn = self.m_layTipControl:getChildByName("btn_create")
    -- btn:setTag(TAG_ENUM.BTN_CREATE_ROOM)
    -- btn:addTouchEventListener(touchFunC)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end


function VideoMarkListLayer:updateModeBtn()
    --1,积分房，2，金币房卡，3，金币场
    local index = {{1},{2},{1,2},{3},{1,3},{2,3},{1,2,3}} --定义与网站一致
    local point = {505,308,111}
    local recordType = GlobalUserItem.tabSystemConfig.MobileBattleRecordMask or 7
    if recordType <= #index then
        for k,v in pairs(index[recordType]) do
                self.m_modeBtn[v]:setPositionY(point[k])
                self.m_modeBtn[v]:setVisible(true)
                if k==1 then
                    self.m_curBtn = self.m_modeBtn[v]
                end
        end
    end
end

-- 按键监听
function VideoMarkListLayer:onButtonClickedEvent( tag,sender )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_SEARCH == tag then
        -- 查看
        local backcode = self.m_editBox:getText()
        if string.len(backcode) == 6 and nil ~= tonumber(backcode) then
            local param = {}
            param.nBackCode = tonumber(backcode)
            param.nFrom = VideoMarkDetailLayer.FROM_BACKCODE
            local md = VideoMarkDetailLayer:create( self._scene, param, self:getLevel() )
            self._scene:addPopLayer(md)

            self:scaleTransitionOut(self.m_spBg)
        else
            showToast(self, "回放码为空或不足6位!", 1)
        end
    elseif TAG_ENUM.IMAGE_EDIT == tag then
        self.m_editBox:setVisible(true)
        self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_imageEdit:setEnabled(false)
    elseif tag == TAG_ENUM.BTN_LAST_PAGE then
        self:onQueryLastPageVideo()
    elseif tag == TAG_ENUM.BTN_NEXT_PAGE then
        self:onQueryNextPageVideo()
    elseif tag == TAG_ENUM.BTN_CREATE_ROOM then
        --self._scene:popCreate()
        if not PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
            showToast(self, "约战房未开启!", 1)
            return
        end
        local cl = PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_CREATEPRIROOME, self._scene, {})
        if nil ~= cl then
            PriRoom:getInstance():setViewFrame(cl)
            self._scene:addPopLayer(cl)
            cl:setName(PriRoom.NAME_CREATE_LAYER)
        end
        PriRoom:getInstance().m_CurrentClubID = 0
    elseif tag == TAG_ENUM.BTN_SCORE then
        self._nodeScore:setVisible(true)
        self._nodeGold:setVisible(false)
        self._nodeNormal:setVisible(false)
        if sender then
            if self.m_curBtn then
                self.m_curBtn:getChildByName("sp_selected"):setVisible(false)
            end
            self.m_curBtn = sender
            self.m_curBtn:getChildByName("sp_selected"):setVisible(true)

        end

        --self._spSelected:setPositionY(505)

        self.m_nQueryType = 2
        GameVideo:getInstance():changeQueryType(self.m_nQueryType)
        GameVideo:getInstance():queryPageVideoInfo(0, self.m_nQueryType)

        --显示搜索
        self.m_imageEdit:setVisible(true)
        self.m_btnSearch:setVisible(true)
    elseif tag == TAG_ENUM.BTN_GOLD then
        self._nodeScore:setVisible(false)
        self._nodeGold:setVisible(true)
        self._nodeNormal:setVisible(false)

        if sender then
            if self.m_curBtn then
                self.m_curBtn:getChildByName("sp_selected"):setVisible(false)
            end
            self.m_curBtn = sender
            self.m_curBtn:getChildByName("sp_selected"):setVisible(true)

        end
        --self._spSelected:setPositionY(308)
        if 0 == #self.m_tabGoldDetailList then
            self:onRequestRecord(1)
        end

        --self.m_nQueryType = 3
        --GameVideo:getInstance():changeQueryType(self.m_nQueryType)
        --GameVideo:getInstance():queryPageVideoInfo(0, self.m_nQueryType)

        --显示搜索
        self.m_imageEdit:setVisible(false)
        self.m_btnSearch:setVisible(false)
    elseif tag == TAG_ENUM.BTN_NORMAL then
        self._nodeScore:setVisible(false)
        self._nodeGold:setVisible(false)
        self._nodeNormal:setVisible(true)

       -- self._spSelected:setPositionY(111)
       if sender then
            if self.m_curBtn then
                self.m_curBtn:getChildByName("sp_selected"):setVisible(false)
            end
            self.m_curBtn = sender
            self.m_curBtn:getChildByName("sp_selected"):setVisible(true)

        end

        if 0 == #self.m_tabNormalDetailList then
            self:onRequestRecord(2)
        end

        --不显示搜索
        self.m_imageEdit:setVisible(false)
        self.m_btnSearch:setVisible(false)
    end
end

-- 战绩按钮
function VideoMarkListLayer:onItemButtonClickedEvent( tag, sender )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    local markinfo = sender.info
    --dump(markinfo, "desciption", 6)
    if nil ~= markinfo then
        local param = {}
        param.list = markinfo
        param.nFrom = VideoMarkDetailLayer.FROM_MARKLIST
        local md = VideoMarkDetailLayer:create( self._scene, param, self:getLevel() + 1 )
        self._scene:addPopLayer(md)
    end
end

--金币房详情
function VideoMarkListLayer:onItemGoldButtonClickedEvent( tag, sender )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    local markinfo = sender.info
    --dump(markinfo, "desciption", 6)
    if nil ~= markinfo then
        local param = {}
        param.list = markinfo
        param.nFrom = VideoMarkDetailLayer.FROM_GOLDLIST
        local md = VideoMarkDetailLayer:create( self._scene, param, self:getLevel() + 1 )
        self._scene:addPopLayer(md)
        -- local param = {}
        -- param.nBackCode = tonumber(markinfo)
        -- param.nFrom = VideoMarkDetailLayer.FROM_BACKCODE
        -- local md = VideoMarkDetailLayer:create( self._scene, param, self:getLevel() )
        -- self._scene:addPopLayer(md)

        -- self:scaleTransitionOut(self.m_spBg)
    end
end


function VideoMarkListLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditInput = string.len(src) ~= 0
        self.m_txtEdit:setVisible(false)
    elseif event == "changed" then
        self.m_bEditInput = string.len(src) ~= 0
    elseif event == "return" then
        editbox:setVisible(false)
        self.m_txtEdit:setVisible(true)
        self.m_txtEdit:setString(src)
        self.m_imageEdit:setEnabled(true)
        if "" == src then
            self.m_txtEdit:setString("输入好友的回放码")
        end
    end
end

function VideoMarkListLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function VideoMarkListLayer:onTransitionInFinish()
    -- 编辑框
    local tmp = self.m_imageEdit
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("输入好友的回放码")
            :setPlaceholderFont("fonts/round_body.ttf", 22)
            :setFont("fonts/round_body.ttf", 22)  
            --:setFontColor(cc.c4b(0,0,0,255))
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    self.m_spBg:addChild(editbox)         
    self.m_editBox = editbox
    self.m_editBox:setVisible(false)
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.m_btnSearch:setEnabled(true)
    self.m_imageEdit:setEnabled(true)
    
    GameVideo:getInstance():setViewFrame(self)
    if self.m_curBtn then
        self:onButtonClickedEvent(self.m_curBtn:getTag(),self.m_curBtn)
    end
end

function VideoMarkListLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function VideoMarkListLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function VideoMarkListLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

--解析时间
function getTimeDes(strTime)
    --针对标准时间特殊处理
    local tabTime = {}
    tabTime._year = string.sub(strTime, 1, 4)
    tabTime._mouth = string.sub(strTime, 6, 7)
    tabTime._day = string.sub(strTime, 9, 10)
    tabTime._hour = string.sub(strTime, 12, 13)
    tabTime._min = string.sub(strTime, 15, 16)
    tabTime._second = string.sub(strTime, 18, 19)

    return tabTime
end

-- function VideoMarkListLayer:tableCellTouched(view, cell)

-- end

-- 子视图大小
function VideoMarkListLayer:cellSizeForTable(view, idx)
    if self._listView == view then
        return view:getViewSize().width, self.m_nCellSize
    elseif self._listGoldView == view then
        return 900, 100
    elseif self._listNormalView == view then
        return 900, 100
    end
    return 0, 0
end

function VideoMarkListLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    if self._listView == view then
        -- CMD_MB_S_QueryVideoInfoResult
        -- 第一个cell
        local iteminfo = self.m_tabList[idx +1]
        dump(iteminfo, "_listView", 1)

        if nil ~= iteminfo then
            local csb = self:createRecordItem(iteminfo, cell)
            local imagecell = csb:getChildByName("mark_sp_cellbg")
            local imagesize = imagecell:getContentSize()
            csb:setPosition(view:getViewSize().width * 0.5 , self.m_nCellSize * 0.5)

            imagecell:setSwallowTouches(false)
            imagecell.info = iteminfo
            imagecell:addTouchEventListener(self.m_itemFunC)
        end
    elseif self._listGoldView == view then
        local roomerinfo = self.m_tabGoldDetailList[idx +1]
        if nil == roomerinfo then
            return cell
        end
        dump(roomerinfo, "_listGoldView", 1)

        local csb = ExternalFun.loadCSB("video/VideoRecordNode.csb", cell)
        csb:setPosition(450, 50)

        local imagecell = csb:getChildByName("mark_sp_cellbg")
        imagecell:setSwallowTouches(false)
        imagecell.info = roomerinfo
        imagecell:addTouchEventListener(self.m_itemGoldFunC)

        -- 房号
        local txtID = csb:getChildByName("txt_roomid_tit")
        txtID:setString(string.format("%06d", roomerinfo.roomID))

        -- 时间
        local txtTime = csb:getChildByName("txt_time")
        local tabTime = getTimeDes(roomerinfo.playTime)
        txtTime:setString(tabTime._mouth .. "." .. tabTime._day .. " " .. tabTime._hour .. ":" .. tabTime._min)

        local szfile = ""
        -- 战绩
        local txtScore = csb:getChildByName("txt_mark")
        local score = roomerinfo.markScore or 0
        local szscore = ""
        if score > 0 then
            szscore = "+" .. ExternalFun.formatScoreText(score)
        else
            szscore = ExternalFun.formatScoreText(score)
            txtScore:setTextColor(cc.c3b(0xf0, 0x51, 0x67))
        end
        txtScore:setString(szscore)
        
        --游戏名字
        local txtGameName = csb:getChildByName("txt_gameName")
        if nil ~= txtGameName then
            txtGameName:setString(roomerinfo.gameName)
        end

        --低分
        local txt_cellscore = csb:getChildByName("txt_cellscore")
        if nil ~= txt_cellscore then
            txt_cellscore:setString(roomerinfo.cellScore)
        end

        --按钮
        local btn = csb:getChildByName("Button_11")
        btn.info = roomerinfo
        btn:addTouchEventListener(self.m_itemGoldFunC)

    elseif self._listNormalView == view then
        local roomerinfo = self.m_tabNormalDetailList[idx +1]
        dump(roomerinfo, "_listNormalView", 1)
        if nil == roomerinfo then
            return cell
        end

        local csb = ExternalFun.loadCSB("video/VideoRecordNode_normal.csb", cell)
        csb:setPosition(450, 50)

        -- 房号
        local txtID = csb:getChildByName("txt_roomid_tit")
        txtID:setString(string.format("%d", roomerinfo.PlayerCount))

 
        -- 时间
        local txtTime = csb:getChildByName("txt_time")
        local tabTime = getTimeDes(roomerinfo.playTime)
        txtTime:setString(tabTime._mouth .. "." .. tabTime._day .. " " .. tabTime._hour .. ":" .. tabTime._min)

        -- 战绩
        local txtScore = csb:getChildByName("txt_mark")
        local score = roomerinfo.markScore or 0
        local szscore = ""
        if score > 0 then
            szscore = "+" .. ExternalFun.formatScoreText(score)
        else
            szscore = ExternalFun.formatScoreText(score)
            txtScore:setTextColor(cc.c3b(0xf0, 0x51, 0x67))
        end
        txtScore:setString(szscore)

        --名字
        local txt_gameName = csb:getChildByName("txt_gameName")
        if nil ~= txt_gameName then
            txt_gameName:setString(roomerinfo.gameName)
        end
    end
    -- 第二个cell
    -- iteminfo = self.m_tabList[idx + 1]
    -- if nil ~= iteminfo then
    --     local csb = self:createRecordItem(iteminfo, cell)
    --     local imagecell = csb:getChildByName("mark_sp_cellbg")
    --     local imagesize = imagecell:getContentSize()
    --     csb:setPosition(view:getViewSize().width * 0.5 + imagesize.width * 0.5 + 60, self.m_nCellSize * 0.5 - 5)

    --     imagecell:setSwallowTouches(false)
    --     imagecell.info = iteminfo
    --     imagecell:addTouchEventListener(self.m_itemFunC)
    -- end

    return cell
end

-- 子视图数目
function VideoMarkListLayer:numberOfCellsInTableView(view)
    if self._listView == view then
        return self.m_nListCount
    elseif self._listGoldView == view then
        print("numberOfCellsInTableView _listGoldView", #self.m_tabGoldDetailList)
        return #self.m_tabGoldDetailList
    elseif self._listNormalView == view then
        print("numberOfCellsInTableView _lisNormaltView", #self.m_tabNormalDetailList)
        return #self.m_tabNormalDetailList
    end
    return 0
end

-- function VideoMarkListLayer:scrollViewDidScroll(view)
--     -- local offset = view:getContentOffset()
--     -- if -self.m_fPageRange <= offset.y and offset.y <= self.m_fPageRange then
--     --     if self.m_nListCount < 4 then
--     --         local bEnableLast = GameVideo:getInstance().m_nPageIndex > GameVideo.DEFAULT_PAGE
--     --         --self.m_layPageControl:setVisible(bEnableLast)
--     --     else
--     --         --print("moveEnd")
--     --         --self.m_layPageControl:setVisible(true)
--     --     end
--     --     self:refreshPageButton()
--     -- else
--     --     local bEnableLast = GameVideo:getInstance().m_nPageIndex > GameVideo.DEFAULT_PAGE
--     --     --self.m_layPageControl:setVisible(bEnableLast)
--     -- end
-- end

function VideoMarkListLayer:refreshPageButton()
    --local bEnableLast = GameVideo:getInstance().m_nPageIndex > GameVideo.DEFAULT_PAGE

    -- 是否上页
    -- self.m_btnLastPage:setEnabled(bEnableLast)
    -- if bEnableLast then
    --     self.m_btnLastPage:setOpacity(204)
    -- else
    --     self.m_btnLastPage:setOpacity(100)
    -- end

    -- -- 是否下页
    -- self.m_btnNextPage:setEnabled(self.m_bHaveNextPageVideo)
    -- if self.m_bHaveNextPageVideo then
    --     self.m_btnNextPage:setOpacity(204)
    -- else
    --     self.m_btnNextPage:setOpacity(100)
    -- end
end

function VideoMarkListLayer:createRecordItem(tabData, parent)
    local csb = ExternalFun.loadCSB("video/VideoRecordNode.csb", parent)
    -- 提取数据
    local roomerinfo = nil
    for k,v in pairs(tabData) do
        if v.dwGameID == GlobalUserItem.tabAccountInfo.dwGameID then
            roomerinfo = v
        end
    end
    -- 
    if nil ~= roomerinfo then
        -- 房号
        local txtID = csb:getChildByName("txt_roomid_tit")
        txtID:setString(string.format("%06d", roomerinfo.dwPersonalRoomID))

        local createDate = roomerinfo.CreateDate
        -- 时间
        local sztime = string.format("%02d.%02d %02d:%02d", createDate.wMonth, createDate.wDay, createDate.wHour, createDate.wMinute)
        local txtTime = csb:getChildByName("txt_time")
        txtTime:setString(sztime)

        local szfile = ""
        -- 战绩
        local txtScore = csb:getChildByName("txt_mark")
        local score = roomerinfo.dwTotalScore or 0
        local szscore = ""
        if score > 0 then
            szscore = "+" .. ExternalFun.formatScoreText(score)
        else
            szscore = ExternalFun.formatScoreText(score)
            txtScore:setTextColor(cc.c3b(0xf0, 0x51, 0x67))
        end
        txtScore:setString(szscore)
        
        --游戏名字
        local txtGameName = csb:getChildByName("txt_gameName")
        local videogame = self._scene:getGameInfo(roomerinfo.wKindID)
        
        dump(videogame, "回放游戏信息", 10)
        if nil ~= videogame then
            txtGameName:setString(videogame._GameName)
        end


        --低分
        local txt_cellscore = csb:getChildByName("txt_cellscore")
        if nil ~= txt_cellscore then
            txt_cellscore:setString(roomerinfo.lCellScore)
        end

        --按钮
        local btn = csb:getChildByName("Button_11")
        btn.info = tabData
        btn:addTouchEventListener(self.m_itemFunC)
        -- 角标
        -- local file = "gameicon/gamemark_" .. roomerinfo.wKindID .. ".png"
        -- if cc.FileUtils:getInstance():isFileExist(file) then
        --     csb:getChildByName("sp_gamemark"):setSpriteFrame(cc.Sprite:create(file):getSpriteFrame())
        -- else
        --     csb:getChildByName("sp_gamemark"):setSpriteFrame(cc.Sprite:create("public/public_blank.png"):getSpriteFrame())
        -- end
    end

    return csb
end

-- 列表回调
function VideoMarkListLayer:onRefreshList()
    print("VideoListLayer:onRefreshList")
    GameVideo:getInstance():dismissPopWait()
    GameVideo:getInstance():setViewFrame(nil)

    self.m_tabList = GameVideo:getInstance():getVideoListInfo()
    local count = #self.m_tabList
    self.m_nListCount = count
    -- if count > 0 then
    --     if math.mod(count ,2) ~= 0 then
    --         self.m_nListCount = self.m_nListCount + 1
    --     end
    -- end
    self.m_bHaveNextPageVideo = not (self.m_nListCount < 5)
    -- 刷新列表
    self._listView:reloadData()
    -- 刷新按钮
    self:refreshPageButton()

    -- 控制创建提示
    -- if 0 == self.m_nListCount and GameVideo:getInstance().m_nPageIndex == GameVideo.DEFAULT_PAGE then
    --     self.m_layTipControl:setVisible(true)
    -- else
    --     self.m_layTipControl:setVisible(false)
    -- end
    return true
end

-- 上一页视频
function VideoMarkListLayer:onQueryLastPageVideo()
    local curidx = GameVideo:getInstance().m_nPageIndex
    if curidx <= 2 then
        return
    end
    GameVideo:getInstance().m_nPageIndex = curidx - 1
    -- 是否有本页信息
    local tmpInfo = GameVideo:getInstance():getVideoListInfo()
    if 0 ~= #tmpInfo then
        print("VideoListLayer:onQueryLastPageVideo 有当前页信息")
        self:onRefreshList()
        return
    end

    GameVideo:getInstance():showPopWait()
    GameVideo:getInstance():setViewFrame(self)
    -- 0:加载自己视频 0:加载约战视频
    GameVideo:getInstance():queryPageVideoInfo(0, self.m_nQueryType)
end

-- 下一页视频
function VideoMarkListLayer:onQueryNextPageVideo()
    local curidx = GameVideo:getInstance().m_nPageIndex
    GameVideo:getInstance().m_nPageIndex = curidx + 1
    -- 是否有本页信息
    local tmpInfo = GameVideo:getInstance():getVideoListInfo()
    if 0 ~= #tmpInfo then
        print("VideoListLayer:onQueryNextPageVideo 有当前页信息")
        self:onRefreshList()
        return
    end
    
    GameVideo:getInstance():showPopWait()
    GameVideo:getInstance():setViewFrame(self)
    -- 0:加载自己视频 0:加载约战视频
    GameVideo:getInstance():queryPageVideoInfo(0, self.m_nQueryType)
end

--请求房间数据
function VideoMarkListLayer:onRequestRecord(nType)
    local data, sign = GlobalUserItem:useridSign(GlobalUserItem.tabAccountInfo.dwUserID)
    local action = string.format("action=getbattlerecord&typeid=%d&userid=%s&time=%s&sign=%s", nType, GlobalUserItem.tabAccountInfo.dwUserID, data, sign)
    print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
    local jsonStr = cjson.encode(action)
    LogAsset:getInstance():logData(jsonStr, true)
    self:showPopWait()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
        dump(jstable, "获取玩家战绩", 7)
        self:dismissPopWait()
        if type(jstable) == "table" then
            local data = jstable["data"]
            if type(data) == "table" then
                local list = data["bList"]
                 dump(list, "获取玩家战绩bList", 7)
                if type(list) == "table" then
                    --赋值
                    local tagTable = {}
                    if 1 == nType then
                        self.m_tabGoldDetailList = {}
                        tagTable = self.m_tabGoldDetailList
                    elseif 2 == nType then
                        self.m_tabNormalDetailList = {}
                        tagTable = self.m_tabNormalDetailList
                    end
                    for i=1,#list do
                        local info = list[i]
                        local battleInfo = {}
                        battleInfo.gameName = info["KindName"]
                        battleInfo.roomID = tonumber(info["RoomID"])
                        battleInfo.owenerNickName =  info["NickName"]
                        battleInfo.owenerID =  info["GameID"]
                        battleInfo.playTime =  info["StartTime"]
                        battleInfo.cellScore =  tonumber(info["BaseScore"])
                        battleInfo.markScore =  tonumber(info["Score"])
                        battleInfo.szPersonalGUID = info["GUID"]
                        battleInfo.KindID = info["KindID"]
                        battleInfo.owenerUserID = info["RoomHostID"]

                        --战绩详情
                        battleInfo.DetailsList = {}
                        local details = info["GamesNum"]
                        for j=1,#details do
                            table.insert(battleInfo.DetailsList, details[j])
                        end

                        table.insert(tagTable, battleInfo)
                        dump(tagTable, "啥子房"..nType, 6)
                    end

                    --刷新对应界面
                    if 1 == nType then
                        self._listGoldView:reloadData()
                    elseif 2 == nType then
                        --self._listNormalView:reloadData()
                    end
                end
            end
            local list = data["sList"]
            --dump(list, "sList", 7)
            if type(list) == "table" then
                if 2 == nType then
                    self.m_tabNormalDetailList = {}
                end
                for i=1,#list do
                    local info = list[i]
                    local battleInfo = {}
                    battleInfo.gameName = info["KindName"]
                    battleInfo.playTime =  info["GameTime"]
                    battleInfo.markScore =  tonumber(info["Score"])
                    battleInfo.PlayerCount =  tonumber(info["UserCount"])

                    table.insert(self.m_tabNormalDetailList, battleInfo)
                end
                dump(self.m_tabNormalDetailList, "m_tabNormalDetailList", 7)
                self._listNormalView:reloadData()
            end
        end
    end)
end

return VideoMarkListLayer