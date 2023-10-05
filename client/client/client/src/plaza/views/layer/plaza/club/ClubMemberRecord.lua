
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local ClubShareLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubShareLayer")
local ClubRecordDetailLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubRecordDetailLayer")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")

local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")


-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubMemberRecord = class("ClubMemberRecord", TransitionLayer)



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

function ClubMemberRecord:ctor( scene, param, level )
    local this = self

    ClubMemberRecord.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_dr.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._scene = scene
    self._param = param
    self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo
    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    --mask:setTag(TAG_ENUM.TAG_MASK)
    --mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    --spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    self.m_bShow = {true,true,true} --大赢家，当前，消耗

    self.m_allCount ={}
    for i=1,2 do
               self.m_allCount[i] =  spbg:getChildByName("Image_3"):getChildByName("count"..i)
    end

    self.m_pName = spbg:getChildByName("Image_3"):getChildByName("name")
    self.m_pId = spbg:getChildByName("Image_3"):getChildByName("name_0")
    self.m_pCount = spbg:getChildByName("Image_3"):getChildByName("jushu")


    self.m_tabDateRecord = {}
    self.m_tabDateRecord.cellData = {}

    --成员列表
    local tmp = self.m_spBg:getChildByName("Panel_2")
    self._listRecordView = cc.TableView:create(tmp:getContentSize())
    self._listRecordView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listRecordView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listRecordView:setPosition(cc.p(0, 0))
    --self._listRecordView:setAnchorPoint(cc.p(0, 0))
    self._listRecordView:setDelegate()
    self._listRecordView:addTo(tmp)
    self._listRecordView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listRecordView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listRecordView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listRecordView:reloadData()
    --tmp:removeFromParent()


    -- 按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listRecordView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listRecordView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listRecordView:getViewSize().width, self._listRecordView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                this:onItemButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end

    

    self:updateCurDateRecord(param.dwQueryUserID,param.dwDateID)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
    self:onTransitionInFinish()

end

function ClubMemberRecord:onExit()
    ClubMemberRecord.super.onExit(self)
end

-- 按键监听
function ClubMemberRecord:onButtonClickedEvent(tag,sender)
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

    elseif tag == TAG_ENUM.BTN_SHARE then 
        local param ={}
        param.title = "【大联盟ID:"..ClubFrame:getInstance().m_nCurrentClub.."】"
        param.content = "ID:"..GlobalUserItem.tabAccountInfo.dwGameID..",我的战绩："..self.m_tabDateRecord.allData.nPlayCount.."局，"
        ..self.m_tabDateRecord.allData.nBigWinnerCount.."次大赢家，"
        ..self.m_tabDateRecord.allData.lCurScore.."勋章！"

        local pa = ClubShareLayer:create(self._scene, param, ClubShareLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)   
    end
end

function ClubMemberRecord:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubMemberRecord:onTransitionInFinish()
    --local date = os.date("%Y%m%d",os.time())
    --self:updateCurDateRecord(tonumber(date))

end

function ClubMemberRecord:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubMemberRecord:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubMemberRecord:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end




function ClubMemberRecord:updateCurDateRecord(dwUserID,dwDateID)

    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabMyDateRecord = {}
    ClubFrame:getInstance():sendQueryUserDetail(ClubFrame:getInstance().m_nCurrentClub,dwUserID,dwDateID)
end


function ClubMemberRecord:onRefreshDateRecord(data)

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

    dump(data,"======================")

    self.m_allCount[1]:setString(data.allData.nBigWinnerCount.."")
    self.m_allCount[2]:setString(data.allData.lGameScore.."")

    self.m_pName:setString(data.allData.szNickName.."")
    self.m_pId:setString("ID:"..data.allData.dwGameID)
    self.m_pCount:setString(data.allData.nPlayCount.."")


    if not self.m_bShow[1] then
        self.m_allCount[1]:setString("-")
    end
    if not self.m_bShow[2] then
        self.m_allCount[2]:setString("-")
    end


    self._listRecordView:reloadData()
end



function ClubMemberRecord:cellSizeForTable(view, idx)
    return 1133 , 90
end

function ClubMemberRecord:tableCellAtIndex(view, idx)
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

    local csb = ExternalFun.loadCSB("club/node_dr.csb", cell)
    csb:setAnchorPoint(cc.p(1,0))
    csb:setPosition(0, 0)

    

    local txtID = csb:getChildByName("gname")
        txtID:setString(""..itemInfo.szConfigName)

    csb:getChildByName("type"):setString(itemInfo.cbPlayMode==0 and "积分模式" or "比赛模式")
    
    csb:getChildByName("time1"):setString(itemInfo.EndTime.wMonth.."/"..itemInfo.EndTime.wDay)

    csb:getChildByName("time2"):setString(itemInfo.EndTime.wHour..":"..ExternalFun.changeTimeText(itemInfo.EndTime.wMinute))


    csb:getChildByName("pname"):setString(ExternalFun.clipText(itemInfo.szBigWinnerName,6,true))
    csb:getChildByName("id"):setString("ID:"..itemInfo.dwBigWinnerID)

    csb:getChildByName("fanghao"):setString(itemInfo.dwRoomID.."")

    csb:getChildByName("jushu"):setString(itemInfo.nDrawCount.."")


    csb:getChildByName("score"):setString(itemInfo.lScore.."")

    local btn = csb:getChildByName("btn_detail")
        btn:addTouchEventListener(function (ref, tType)
            if tType == ccui.TouchEventType.ended then
                        local pa = ClubRecordDetailLayer:create(self._scene, {dwRoomID = itemInfo.dwRoomID,dwDateID = self._param.dwDateID,endtime = itemInfo.EndTime }, ClubRecordDetailLayer.SECOND_LEVEL)
                        self._scene:addPopLayer(pa)
            end
        end)


    return cell

end

-- 子视图数目
function ClubMemberRecord:numberOfCellsInTableView(view)
    return #self.m_tabDateRecord.cellData
end

function ClubMemberRecord:onItemButtonClickedEvent( tag, ref )
    local Memberinfo = ref.Memberinfo
    if nil == Memberinfo then
        print("成员信息为空")
        return
    end

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo

end



return ClubMemberRecord