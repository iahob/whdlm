
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local ClubMemberRecord = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMemberRecord")

-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubMemberLayer = class("ClubMemberLayer", TransitionLayer)



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
    "BTN_DETAIL",
    "BTN_FLAG"
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubMemberLayer:ctor( scene, param, level )
    local this = self

    ClubMemberLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/club_MemberList.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._scene = scene
    self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo


    -- 遮罩
    local mask = csbNode:getChildByName("Panel_6")
    --mask:setTag(TAG_ENUM.TAG_MASK)
    --mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


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
            :setPosition(933,71)
            --:setFontColor(cc.c3b(223, 201, 255))
            :setAnchorPoint(cc.p(0,0.5))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 23)
            :setFont("fonts/round_body.ttf", 23) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    mask:addChild(editbox)         
    self.m_editID = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.serchID = spbg:getChildByName("Text_6")
    self.serchID:setString("输入ID查询")


    self.searchImg = spbg:getChildByName("Image_2")
    self.searchImg:setTag(TAG_ENUM.BTN_SERCH)
    self.searchImg:addTouchEventListener( touchFunC )
    self.searchImg:setTouchEnabled(true)

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


    --获取大联盟信息


    self.m_tabDateRecord = {}
    self.m_curDay = 0


    local img1 = spbg:getChildByName("Image_4")
    local img2 = spbg:getChildByName("Image_5")


    -- 创建的大联盟
    self.m_isNoCap = false
    local checkbox1 = spbg:getChildByName("CheckBox_1")
    checkbox1:addTouchEventListener(function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
                    self.m_isNoCap = not self.m_isNoCap 
                    sender:setSelected(self.m_isNoCap)
                    self:onRefreshInfo()
        end    
    end)
    checkbox1:setSelected(false)


    self.m_isNoEmpty = false
    local checkbox2 = spbg:getChildByName("CheckBox_2")
    checkbox2:addTouchEventListener(function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
                    self.m_isNoEmpty = not self.m_isNoEmpty 
                    sender:setSelected(self.m_isNoEmpty)
                    self:onRefreshInfo()
        end    
    end)
    checkbox2:setSelected(false)



    self.m_isCreator = false

    local groupProperty = self.m_nCurrentClubInfo.GroupProperty
    if groupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID  and bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0 then

       img1:setVisible(false)
       img2:setVisible(true)
       checkbox1:setVisible(false)
       checkbox2:setVisible(false)

       self.m_isCreator = false


    else
       img1:setVisible(true)
       img2:setVisible(false)  
       checkbox1:setVisible(true)  
       checkbox2:setVisible(true)  
       self.m_isCreator = true

       spbg:getChildByName("Sprite_4"):setTexture("club/mem_title.png")
    end

    dump(ClubFrame:getInstance().m_tabGroupMember,"===========",6)

    self.m_bShow = {true,true,true} --大赢家，当前，消耗
    self:onRefreshInfo(nil)

    self:onTransitionInFinish()
    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)


end

function ClubMemberLayer:onExit()
    ClubMemberLayer.super.onExit(self)
end

-- 按键监听
function ClubMemberLayer:onButtonClickedEvent(tag,sender)
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
    end
end

function ClubMemberLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubMemberLayer:onTransitionInFinish()
    local date = os.date("%Y%m%d",os.time())

    self:updateCurDateRecord(tonumber(date))

end

function ClubMemberLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubMemberLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubMemberLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end




function ClubMemberLayer:updateCurDateRecord(date)

    self.m_curDay = date
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabCurDateRecord = {}
    ClubFrame:getInstance():sendQueryDateRecord(ClubFrame:getInstance().m_nCurrentClub, date)
end


function ClubMemberLayer:onRefreshDateRecord(data)
    self.m_tabDateRecord = clone(data)
    --dump(self.m_tabDateRecord,"0000==")
    --self._listMemberView:reloadData()
    self:onRefreshInfo()
end






function ClubMemberLayer:onRefreshInfo(id)
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
    --排序
    for i, v in ipairs(self.m_tabMemberList) do

        if v.UserInfo.dwUserID == self.m_nCurrentClubInfo.GroupProperty.dwCreaterID then
            local clubHolder = v
            table.remove(self.m_tabMemberList, i)
            table.insert(self.m_tabMemberList, 1, clubHolder)
        end
    end

    --去掉队长及成员
    if self.m_isNoCap then
        print("===========m_isNoCap==========")
        for i=#self.m_tabMemberList,1,-1 do
            if self.m_tabMemberList[i].MemberInfo.nMemberRight == 4 or self.m_tabMemberList[i].MemberInfo.nMemberRight == 8 then
                table.remove(self.m_tabMemberList,i)
            end
        end
    end

    if self.m_isNoEmpty then
        for i=#self.m_tabMemberList,1,-1 do
            if not self.m_tabDateRecord[self.m_tabMemberList[i].UserInfo.dwUserID] or self.m_tabDateRecord[self.m_tabMemberList[i].UserInfo.dwUserID].nPlayCount == 0 then
                table.remove(self.m_tabMemberList,i)
            end
        end
    end


    if nil ~= id and 0 == #self.m_tabMemberList then
        showToast(self, "没有找到该玩家，请确认玩家ID！", 2)
    end
    dump(self.m_tabMemberList)

    if self.m_nCurrentClubInfo.GroupBattleRule.cbBigWinnerShow ==0 and 
        (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0) and 
         (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) == 0) then 

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


    --dump(self.m_tabMemberList, "self.m_tabClubInfo", 10)
    self._listMemberView:reloadData()
end

function ClubMemberLayer:cellSizeForTable(view, idx)
    return 1190 , 90
end

function ClubMemberLayer:tableCellAtIndex(view, idx)
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
    --dump(Memberinfo, "tableCellAtIndex", 10)

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo

    local path = "club/node_memList_dy.csb"
    if self.m_isCreator then
            path = "club/node_memList.csb"
    else
            path = "club/node_memList_dy.csb"
    end

    local csb = ExternalFun.loadCSB(path, cell)
    csb:setAnchorPoint(cc.p(0,0))
    csb:setPosition(0, 0)

    --ID
    local txtID = csb:getChildByName("id")
    if nil ~= txtID then
        txtID:setString("ID:"..tagIMUserInfo.dwGameID)
    end
    --昵称
    local txtName = csb:getChildByName("name")
    txtName:setString(ExternalFun.clipText(tagIMUserInfo.szNickName,6,true))


    --状态
    local stutas = csb:getChildByName("stutas")
        
        if tagIMGroupMember.cbStatus==0 then 
            stutas:setColor(cc.c3b(255,0,0))
            stutas:setString("离线")
        elseif tagIMGroupMember.cbStatus==1 then 
            stutas:setColor(cc.c3b(21,150,97))
            stutas:setString("在线")
        elseif tagIMGroupMember.cbStatus==2 then 
            stutas:setColor(cc.c3b(114,66,14))
            stutas:setString("游戏中")
        end


    local curRecord= self.m_tabDateRecord[tagIMUserInfo.dwUserID] or {nPlayCount=0,nBigWinnerCount=0,lScore= 0,lWinScore = 0,lRevenue = 0}

    --约战次数
    local txtGameTimes = csb:getChildByName("jushu")
        txtGameTimes:setString(curRecord.nPlayCount.."局")


    local maxWin = csb:getChildByName("maxScore")
        maxWin:setString(curRecord.nBigWinnerCount.."")
        if not self.m_bShow[1] then

             maxWin:setString("-")
        end


    local loseScore = csb:getChildByName("getScore")
        loseScore:setString(curRecord.lRevenue.."")


    if self.m_isCreator then 
        local btn = csb:getChildByName("btn_detail")
        btn:setTag(TAG_ENUM.BTN_DETAIL)
        btn:addTouchEventListener(self.m_itemFunC)
        btn.Memberinfo = Memberinfo

        local btn = csb:getChildByName("Button_2")
            btn:setTag(TAG_ENUM.BTN_FLAG)
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
            btn:setEnabled(false)
        end


    end 


    return cell

end

-- 子视图数目
function ClubMemberLayer:numberOfCellsInTableView(view)
    return #self.m_tabMemberList
end

function ClubMemberLayer:onItemButtonClickedEvent( tag, ref )
    local Memberinfo = ref.Memberinfo
    if nil == Memberinfo then
        print("成员信息为空")
        return
    end

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo
    local CurRecord = ref.CurRecord

    if tag == TAG_ENUM.BTN_DETAIL then

        local pa = ClubMemberRecord:create(self._scene, {dwQueryUserID = tagIMGroupMember.dwMemberID,dwDateID = self.m_curDay}, ClubMemberRecord.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif tag == TAG_ENUM.BTN_FLAG then 


         if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT8) then

            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendSetStatisticFlag(ClubFrame:getInstance().m_nCurrentClub,tagIMGroupMember.dwMemberID,self.m_curDay,(CurRecord.cbFlag+1)%2)
        else
            showToast(self, "点赞权限未开启！", 2)
        end


    end

    

    --------------test----------------
   -- ClubFrame:getInstance():sendDeleteGroup(Clubinfo.dwGroupID, 0)
    --ClubFrame:getInstance():sendApplyRequest(Clubinfo.dwGroupID, 0)
end


function ClubMemberLayer:onFlagResult(dwUserID,cbFlag)
    print("==============="..dwUserID..",=="..cbFlag)
    for k,v in pairs(self.m_tabDateRecord) do
        if v and v.dwMemberID == dwUserID then
            v.cbFlag = cbFlag
            break
        end
    end
    self:onRefreshInfo()
end




function ClubMemberLayer:onEditEvent(event, editbox)

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


function ClubMemberLayer:onDeleteMember()
    self:onRefreshInfo(nil)

    -- 通知更新    
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

return ClubMemberLayer