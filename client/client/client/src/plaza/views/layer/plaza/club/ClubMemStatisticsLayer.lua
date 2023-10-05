
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")



-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubMemStatisticsLayer = class("ClubMemStatisticsLayer", TransitionLayer)



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
    "BTN_TEAM"
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubMemStatisticsLayer:ctor( scene, param, level )
    local this = self

    ClubMemStatisticsLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_mt.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    --self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo
    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    self.m_tabAllRecord = {}


    -- 遮罩
    local mask = csbNode:getChildByName("Panel_3")
    --mask:setTag(TAG_ENUM.TAG_MASK)
    --mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    --spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    --spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)



    local btn = spbg:getChildByName("Button_12")
    btn:setTag(TAG_ENUM.BTN_TEAM)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_tabMemberList = {}
    self.m_tabDateRecord = {}

    --成员列表
    local tmp = self.m_spBg:getChildByName("Panel_4")
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

    --搜索输入框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    -- 设置介绍

    local editbox = ccui.EditBox:create(cc.size(343, 52),"public/public_blank.png")
            :setPosition(903,71)
            --:setFontColor(cc.c3b(223, 201, 255))
            :setAnchorPoint(cc.p(0,0.5))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 25)
            :setFont("fonts/round_body.ttf", 25) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    mask:addChild(editbox)         
    self.m_editID = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.serchID = spbg:getChildByName("Text_11")
    self.serchID:setString("输入ID查询")


    self.searchImg = spbg:getChildByName("Image_5")
    self.searchImg:setTag(TAG_ENUM.BTN_SERCH)
    self.searchImg:addTouchEventListener( touchFunC )
    self.searchImg:setTouchEnabled(true)



    self.dateImg = spbg:getChildByName("Image_3")
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


    self.m_isNoEmpty = false
    local checkbox = spbg:getChildByName("CheckBox_1")
    checkbox:addTouchEventListener(function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
                    self.m_isNoEmpty = not self.m_isNoEmpty 
                    sender:setSelected(self.m_isNoEmpty)
                    self:onRefreshInfo()
        end    
    end)
    checkbox:setSelected(false)

    self.m_isNoCap = false
    local checkbox = spbg:getChildByName("CheckBox_2")
    checkbox:addTouchEventListener(function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
                    self.m_isNoCap = not self.m_isNoCap 
                    sender:setSelected(self.m_isNoCap)
                    self:onRefreshInfo()
        end    
    end)
    checkbox:setSelected(false)


    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
    self:onTransitionInFinish()

end

function ClubMemStatisticsLayer:onExit()
    ClubMemStatisticsLayer.super.onExit(self)
end



function ClubMemStatisticsLayer:onEditEvent(event, editbox)

        local src = editbox:getText() 
        if event == "return" then
            self.searchImg:setEnabled(true)
            self.serchID:setString(src)
            if self.serchID:getContentSize().width == 0 then --没有输入
                self.serchID:setString("输入ID查询")
                self:onRefreshInfo()
            end
            if string.len(src) == 6  then 
                self:onRefreshInfo(tonumber(src))
            elseif string.len(src) > 0  then
                showToast(self, "请输入正确的玩家ID！", 2)
            end
            self.serchID:setVisible(true)
        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= "输入ID查询" then
                self.serchID:setString(src)
            else
                self.serchID:setString("输入ID查询")
            end

        elseif event == "began" then
            --self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "输入ID查询" then
                self.serchID:setString(src)
            else
                self.serchID:setString("输入ID查询")
            end
            self.serchID:setVisible(false)
        end

end



-- 按键监听
function ClubMemStatisticsLayer:onButtonClickedEvent(tag,sender)
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
    end
end

function ClubMemStatisticsLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubMemStatisticsLayer:onTransitionInFinish()
    local date = os.date("%Y%m%d",os.time())
    self:updateCurDateRecord(tonumber(date))

end

function ClubMemStatisticsLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubMemStatisticsLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubMemStatisticsLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end




function ClubMemStatisticsLayer:updateCurDateRecord(date)

    self.m_curDate = date
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabCurDateRecord = {}
    ClubFrame:getInstance():sendQueryDateRecord(ClubFrame:getInstance().m_nCurrentClub,date)
end


function ClubMemStatisticsLayer:onRefreshDateRecord(data)


    self.m_tabDateRecord = clone(data)


    self:onRefreshInfo()
end


function ClubMemStatisticsLayer:onRefreshInfo(id)
    --获取当前选中的大联盟数据
    self.m_tabMemberList = {}
    for i,v in ipairs(ClubFrame:getInstance().m_tabGroupMember) do
       --先判断大联盟
       if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
            local tabMember = v.memberInfo
            for j=1, #tabMember do
                if nil ~= id then
                    if tabMember[j].UserInfo.dwGameID == id then
                        table.insert(self.m_tabMemberList, tabMember[j])
                    end
                else
                    table.insert(self.m_tabMemberList, tabMember[j])
                end
            end
       end
    end
    -- --排序
    -- for i, v in ipairs(self.m_tabMemberList) do

    --     if v.UserInfo.dwUserID == self.m_nCurrentClubInfo.GroupProperty.dwCreaterID then
    --         local clubHolder = v
    --         table.remove(self.m_tabMemberList, i)
    --         table.insert(self.m_tabMemberList, 1, clubHolder)
    --     end
    -- end
    --去掉无记录成员

    if self.m_isNoEmpty then
        for i=#self.m_tabMemberList,1,-1 do
            if not self.m_tabDateRecord[self.m_tabMemberList[i].UserInfo.dwUserID] then
                table.remove(self.m_tabMemberList,i)
            end
        end
    end


    --去掉队长及成员
    if self.m_isNoCap then
        for i=#self.m_tabMemberList,1,-1 do
            if  (bit:_and(self.m_tabMemberList[i].MemberInfo.nMemberRight, 4) ~= 0) or (bit:_and(self.m_tabMemberList[i].MemberInfo.nMemberRight, 8) ~= 0)   then
                table.remove(self.m_tabMemberList,i)
            end
        end
    end

    if nil ~= id and 0 == #self.m_tabMemberList then
        showToast(self, "没有找到该玩家，请确认玩家ID！", 2)
    end


    self._listMemberView:reloadData()
end

function ClubMemStatisticsLayer:cellSizeForTable(view, idx)
    return 1132 , 90
end

function ClubMemStatisticsLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local Memberinfo = self.m_tabMemberList[idx +1]
    if nil == Memberinfo then
        return cell
    end

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo

    local csb = ExternalFun.loadCSB("club/node_dt.csb", cell)
    csb:setAnchorPoint(cc.p(0,0))
    csb:setPosition(0, 0)

    

      local txtID = csb:getChildByName("Text_1_0")
    if nil ~= txtID then
        txtID:setString("ID:"..tagIMUserInfo.dwGameID)
    end
    --昵称
    local txtName = csb:getChildByName("Text_1")
    if nil ~= txtName then
        local strNickname = string.EllipsisByConfig(tagIMUserInfo.szNickName, 200, 
                                             string.getConfig("fonts/round_body.ttf", 25))
        txtName:setString(strNickname)
    end

    --状态
    local stutas = csb:getChildByName("Text_1_1")
        stutas:setString(tagIMGroupMember.cbStatus==0 and "离线" or "在线")


    local curRecord= self.m_tabDateRecord[tagIMUserInfo.dwUserID] or {nPlayCount=0,nBigWinnerCount=0,lScore= 0,lWinScore = 0,lRevenue = 0}


    --约战次数
    local txtGameTimes = csb:getChildByName("Text_1_2")
        txtGameTimes:setString(curRecord.nPlayCount.."局")


    local maxWin = csb:getChildByName("Text_1_4")
        maxWin:setString(curRecord.nBigWinnerCount.."")

    local lastScore = csb:getChildByName("Text_1_5")
        lastScore:setString(curRecord.lScore.."")

    local loseScore = csb:getChildByName("Text_1_6")
        loseScore:setString(curRecord.lWinScore.."")

    local lRevenueScore = csb:getChildByName("Text_1_3")
        lRevenueScore:setString(curRecord.lRevenue.."")

        local btn = csb:getChildByName("Button_1")
            btn:addTouchEventListener(self.m_itemFunC)
            btn.Memberinfo = Memberinfo
            btn.CurRecord = curRecord

        if curRecord.nPlayCount ~= 0  then 
            btn:setVisible(true)
            if curRecord.cbFlag ==0 then 
                btn:loadTextureNormal("club/dt_xin2.png")
                btn:loadTexturePressed("club/dt_xin2.png")
            else
                btn:loadTextureNormal("club/dt_xin1.png")
                btn:loadTexturePressed("club/dt_xin1.png")
            end
        else
            btn:setVisible(false)
        end


    return cell

end

-- 子视图数目
function ClubMemStatisticsLayer:numberOfCellsInTableView(view)
    return #self.m_tabMemberList
end

function ClubMemStatisticsLayer:onItemButtonClickedEvent( tag, ref )
    local Memberinfo = ref.Memberinfo
    if nil == Memberinfo then
        print("成员信息为空")
        return
    end

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo
    local CurRecord = ref.CurRecord

     if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT8) then

        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendSetStatisticFlag(ClubFrame:getInstance().m_nCurrentClub,tagIMGroupMember.dwMemberID,self.m_curDate,(CurRecord.cbFlag+1)%2)
    else
        showToast(self, "点赞权限未开启！", 2)
    end
end



return ClubMemStatisticsLayer