
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local ClubRecordDetailLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubRecordDetailLayer")


-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubAllRecordLayer = class("ClubAllRecordLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "IMAGE_EDIT",           -- 编辑
    "BTN_SERCH",             -- 搜索
    "BTN_DATE",             -- 日期
    "BTN_DATEBG",
    "BTN_SHARE",--分享
    "BTN_UPDATE",--刷新
    "BTN_MYRECORD",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubAllRecordLayer:ctor( scene, param, level )
    local this = self

    ClubAllRecordLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_qyqRecord.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    --self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myGroupInfo = GlobalUserItem.tabGroupUserInfo
    self.m_tabAllRecord = {}


    -- 遮罩
   -- local mask = csbNode:getChildByName("Panel_6")
    --mask:setTag(TAG_ENUM.TAG_MASK)
    --mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    --spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)



    local btn = spbg:getChildByName("btn_update")
    btn:setTag(TAG_ENUM.BTN_UPDATE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("myrecord")
    btn:setTag(TAG_ENUM.BTN_MYRECORD)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_damdCount = spbg:getChildByName("zuanshi")
    self.m_playCount = spbg:getChildByName("jushu")
    self.m_revenue = spbg:getChildByName("gongxian")

    self.m_tabDateRecord = {}
    self.m_tabDateRecord.cellData={}


    --成员列表
    local tmp = self.m_spBg:getChildByName("Panel_1")
    self._listMemberView = cc.TableView:create(tmp:getContentSize())
    self._listMemberView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listMemberView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listMemberView:setPosition(cc.p(0, 0))
    --self._listMemberView:setAnchorPoint(cc.p(0, 0))
    self._listMemberView:setDelegate()
    self._listMemberView:addTo(tmp)
    self._listMemberView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listMemberView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listMemberView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listMemberView:reloadData()
    --tmp:removeFromParent()


    -- 按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listMemberView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listMemberView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listMemberView:getViewSize().width, self._listMemberView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                this:onItemButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end


    self.dateImg = spbg:getChildByName("Image_1")
    self.dateImg:setTag(TAG_ENUM.BTN_DATE)
    self.dateImg:addTouchEventListener( touchFunC )
    self.dateImg:setTouchEnabled(true)


    self.datePanel = spbg:getChildByName("Panel_2")
    self.datePanel:setTag(TAG_ENUM.BTN_DATEBG)
    self.datePanel:addTouchEventListener( touchFunC )
    self.datePanel:setTouchEnabled(true)

    --local curDate  = os.date("%Y%m%d",os.time())
    self.dateImg:getChildByName("AtlasLabel_1"):setString(os.date("%m",os.time()))
    self.dateImg:getChildByName("AtlasLabel_1_0"):setString(os.date("%d",os.time()))

    for i=1,7 do
        local btn = self.datePanel:getChildByName("Button_"..i)
            btn:setTag(i)
            btn:addTouchEventListener( function(ref, tType)
                if tType == ccui.TouchEventType.ended then
                        self:updateCurDateRecord(tonumber(os.date("%Y%m%d",os.time()-(i-1)*24*60*60)))
                        self.datePanel:setVisible(false)
                        self.dateImg:getChildByName("AtlasLabel_1"):setString(os.date("%m",os.time()-(i-1)*24*60*60))
                        self.dateImg:getChildByName("AtlasLabel_1_0"):setString(os.date("%d",os.time()-(i-1)*24*60*60))
                end
            end )
            btn:setTitleText(os.date("%m/%d",os.time()-(i-1)*24*60*60))

    end

    self.m_curDate = 0--tonumber(os.date("%Y%m%d",os.time()))

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
    self:onTransitionInFinish()

end

function ClubAllRecordLayer:onExit()
    ClubAllRecordLayer.super.onExit(self)
end

-- 按键监听
function ClubAllRecordLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        --self:scaleTransitionOut(self.m_spBg)
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_SERCH then
        self.m_editID:touchDownAction(self.m_editID, ccui.TouchEventType.ended)
        self.searchImg:setEnabled(false)
    elseif tag == TAG_ENUM.BTN_DATE then
        self.datePanel:setVisible(true)
    elseif tag == TAG_ENUM.BTN_DATEBG then
        self.datePanel:setVisible(false)
    elseif tag == TAG_ENUM.BTN_MYRECORD then
        local ClubMyRecordLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMyRecordLayer")
        local pa = ClubMyRecordLayer:create(self._scene, {}, ClubMyRecordLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_UPDATE then        
        self:updateCurDateRecord(self.m_curDate)
        self.m_spBg:runAction(cc.Blink:create(0.05, 1))
    end
end

function ClubAllRecordLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubAllRecordLayer:onTransitionInFinish()
    local date = os.date("%Y%m%d",os.time())
    self:updateCurDateRecord(tonumber(date))

end

function ClubAllRecordLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubAllRecordLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubAllRecordLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end




function ClubAllRecordLayer:updateCurDateRecord(date)

    self.m_curDate = date
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabClubDateRecord = {}
    ClubFrame:getInstance():sendQueryClubDetail(ClubFrame:getInstance().m_nCurrentClub,date)
end


function ClubAllRecordLayer:onRefreshDateRecord(data)

    if not data or not  data.allData.nDiamond then
        return
    end
    
    self.m_tabDateRecord = clone(data)

    self.m_damdCount:setString(data.allData.nDiamond.."")
    self.m_playCount:setString(data.allData.nCount.."")
    self.m_revenue:setString(data.allData.lAllRoomRevenue.."")


    self._listMemberView:reloadData()
end



function ClubAllRecordLayer:cellSizeForTable(view, idx)
    return 1190 , 90
end

function ClubAllRecordLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local itemInfo = self.m_tabDateRecord.cellData[idx+1]
    if nil == itemInfo then
        return cell
    end
    --dump(Memberinfo, "tableCellAtIndex", 10)

    local csb = ExternalFun.loadCSB("club/node_qr.csb", cell)
    csb:setAnchorPoint(cc.p(0,0))
    csb:setPosition(0, 0)

    

    local txtID = csb:getChildByName("gname")
        txtID:setString(""..itemInfo.szConfigName)


    --状态
    csb:getChildByName("time1"):setString(itemInfo.EndTime.wMonth.."/"..itemInfo.EndTime.wDay)

    csb:getChildByName("time2"):setString(itemInfo.EndTime.wHour..":"..ExternalFun.changeTimeText(itemInfo.EndTime.wMinute))


    csb:getChildByName("pname"):setString(ExternalFun.clipText(itemInfo.szBigWinnerName,6,true).."")

    csb:getChildByName("id"):setString("ID:"..itemInfo.dwBigWinnerID)

    csb:getChildByName("fanghao"):setString(itemInfo.dwRoomID.."")

    csb:getChildByName("jushu"):setString(itemInfo.nDrawCount.."")

    csb:getChildByName("zuanshi"):setString(itemInfo.lDiamond.."")

    csb:getChildByName("gongxian"):setString(itemInfo.lRoomRevenue.."")

    local btn = csb:getChildByName("btn_detail")
        btn:addTouchEventListener(function (ref, tType)
            if tType == ccui.TouchEventType.ended then
                        local pa = ClubRecordDetailLayer:create(self._scene, {dwRoomID = itemInfo.dwRoomID,dwDateID = self.m_curDate,endtime = itemInfo.EndTime }, ClubRecordDetailLayer.SECOND_LEVEL)
                        self._scene:addPopLayer(pa)
            end
        end)

    return cell

end

-- 子视图数目
function ClubAllRecordLayer:numberOfCellsInTableView(view)
    return #self.m_tabDateRecord.cellData
end

function ClubAllRecordLayer:onItemButtonClickedEvent( tag, ref )
    local Memberinfo = ref.Memberinfo
    if nil == Memberinfo then
        print("成员信息为空")
        return
    end

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo

end



return ClubAllRecordLayer