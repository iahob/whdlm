
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local ClubAllRecordLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubAllRecordLayer")
local ClubShareLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubShareLayer")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local ClubRecordDetailLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubRecordDetailLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")


-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubMyRecordLayer = class("ClubMyRecordLayer", TransitionLayer)



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
    "BTN_CLUBRD",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubMyRecordLayer:ctor( scene, param, level )
    local this = self

    ClubMyRecordLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/club_myRecord.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo
    -- 遮罩
    local mask = csbNode:getChildByName("Panel_6")
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


    local btn = spbg:getChildByName("Button_share")
    btn:setTag(TAG_ENUM.BTN_SHARE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("Button_ref")
    btn:setTag(TAG_ENUM.BTN_UPDATE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("btn_dui")
    btn:setTag(TAG_ENUM.BTN_CLUBRD)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local groupProperty = GlobalUserItem.tabCurGroupInfo.GroupProperty
    if groupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID then
        btn:setVisible(false)
    end

    self.m_bShow = {true,true,true} --大赢家，当前，消耗

    self.m_allCount ={}
    for i=1,4 do
               self.m_allCount[i] =  spbg:getChildByName("Image_2"):getChildByName("num"..i)
    end

    self.m_tip = spbg:getChildByName("Text_3")

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

    self.m_curDate = 0
 


    self.m_curDayScore = nil

    self.m_extra = nil
    for k,v in pairs(ClubFrame:getInstance().m_tabGroupList) do
        if v and v.dwGroupID == ClubFrame:getInstance().m_nCurrentClub then
            self.m_extra = v.cmd_extra
            break
        end
    end


    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
    self:onTransitionInFinish()

end

function ClubMyRecordLayer:onExit()
    ClubMyRecordLayer.super.onExit(self)
end

-- 按键监听
function ClubMyRecordLayer:onButtonClickedEvent(tag,sender)
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
    elseif tag == TAG_ENUM.BTN_UPDATE then    
        self:updateCurDateRecord(self.m_curDate)
    elseif tag == TAG_ENUM.BTN_CLUBRD then    
        local pa = ClubAllRecordLayer:create(self._scene, {}, ClubAllRecordLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_SHARE then 

        local param ={}
        param.title = "我的战绩"

        local str = "大联盟ID:"..ClubFrame:getInstance().m_nCurrentClub

        if self.m_extra then
            if self.m_extra.cbGroupShow ==  0 then
                str = "大联盟邀请码:"..self.m_extra.lInvitationCode
            end
        end

        param.content = str..",我的ID:"..GlobalUserItem.tabAccountInfo.dwGameID..",我的战绩："..self.m_tabDateRecord.allData.nPlayCount.."局，"
        ..self.m_tabDateRecord.allData.nBigWinnerCount.."次大赢家，"
        ..self.m_tabDateRecord.allData.lCurScore.."勋章！"

        local pa = ClubShareLayer:create(self._scene, param, ClubShareLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)   
    end
end

function ClubMyRecordLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubMyRecordLayer:onTransitionInFinish()
    local date = os.date("%Y%m%d",os.time())
    self:updateCurDateRecord(tonumber(date))

end

function ClubMyRecordLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubMyRecordLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubMyRecordLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end




function ClubMyRecordLayer:updateCurDateRecord(date)
    self.m_curDate = date
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabMyDateRecord = {}
    ClubFrame:getInstance():sendQueryUserDetail(ClubFrame:getInstance().m_nCurrentClub,GlobalUserItem.tabAccountInfo.dwUserID,date)
end


function ClubMyRecordLayer:onRefreshDateRecord(data)

    if not data or not data.allData or not data.allData.nPlayCount  then
        return
    end

    if self.m_nCurrentClubInfo.GroupBattleRule.cbBigWinnerShow ==0 and 
        (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0) and 
         (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) == 0)then 

          self.m_bShow[1] =false
    end 

    if self.m_nCurrentClubInfo.GroupBattleRule.cbMedalMode ==0 or 
        (self.m_nCurrentClubInfo.GroupBattleRule.cbCurMedalShow ==0 and 
        (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0) and 
         (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) == 0)) then 

          self.m_bShow[2] =false
    end 

    if self.m_nCurrentClubInfo.GroupBattleRule.cbMedalMode ==0 or 
        (self.m_nCurrentClubInfo.GroupBattleRule.cbMedalRevenueShow ==0 and 
        (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0) and 
         (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) == 0)) then 

          self.m_bShow[3] =false
    end 


    self.m_tabDateRecord = clone(data)

    self.m_allCount[1]:setString(data.allData.nPlayCount.."")
    self.m_allCount[2]:setString(data.allData.nBigWinnerCount.."")
    self.m_allCount[3]:setString(data.allData.lCurScore.."")
    self.m_allCount[4]:setString(data.allData.lGameScore > 0 and "+"..data.allData.lGameScore or ""..data.allData.lGameScore)
    if not self.m_curDayScore  then 
        self.m_curDayScore = data.allData.lCurScore
        if self.m_nCurrentClubInfo.GroupBattleRule.cbMedalClear == 0 then
            self.m_tip:setString("不清零，今日已累计分数为")
        else
            self.m_tip:setString("清零，今日已累计分数为")
        end
        local curScore = self.m_tip:getChildByName("Text_4")
        curScore:setString(self.m_curDayScore.."")
        curScore:setPositionX(self.m_tip:getContentSize().width)
        
    end 

    if not self.m_bShow[1] then
        self.m_allCount[2]:setString("-")
    end
    if not self.m_bShow[2] then
        self.m_allCount[3]:setString("-")
    end
    if not self.m_bShow[3] then
        self.m_allCount[4]:setString("-")
    end

    self._listMemberView:reloadData()
end



function ClubMyRecordLayer:cellSizeForTable(view, idx)
    return 1190 , 90
end

function ClubMyRecordLayer:tableCellAtIndex(view, idx)
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

    local csb = ExternalFun.loadCSB("club/node_myRecord.csb", cell)
    csb:setAnchorPoint(cc.p(0,0))
    csb:setPosition(0, 0)

    

    local txtID = csb:getChildByName("gname")
        txtID:setString(""..itemInfo.szConfigName)


    csb:getChildByName("type"):setString(itemInfo.cbPlayMode==0 and "积分模式" or "比赛模式")

    csb:getChildByName("time1"):setString(itemInfo.EndTime.wMonth.."/"..itemInfo.EndTime.wDay)

    csb:getChildByName("time2"):setString(itemInfo.EndTime.wHour..":"..ExternalFun.changeTimeText(itemInfo.EndTime.wMinute))


    csb:getChildByName("pname"):setString(ExternalFun.clipText(itemInfo.szBigWinnerName,6,true))
    csb:getChildByName("id"):setString("ID:"..itemInfo.dwBigWinnerID)

    if itemInfo.dwBigWinnerID == 0 then -- 百人游戏、没有大赢家
        csb:getChildByName("pname"):setString("")
        csb:getChildByName("id"):setString("—")
        csb:getChildByName("id"):setPositionY(45)
    end

    csb:getChildByName("fanghao"):setString(itemInfo.dwRoomID.."")

    csb:getChildByName("jushu"):setString(itemInfo.nDrawCount.."")

    csb:getChildByName("score"):setString(itemInfo.lScore.."")


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
function ClubMyRecordLayer:numberOfCellsInTableView(view)
    return #self.m_tabDateRecord.cellData
end

function ClubMyRecordLayer:onItemButtonClickedEvent( tag, ref )
    local Memberinfo = ref.Memberinfo
    if nil == Memberinfo then
        print("成员信息为空")
        return
    end

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo

end



return ClubMyRecordLayer