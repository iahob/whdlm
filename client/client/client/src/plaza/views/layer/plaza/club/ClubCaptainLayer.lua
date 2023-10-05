
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local ClubMemberRecord = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMemberRecord")
local ClubSetBelowPlayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubSetBelowPlayer")
local ClubApplyMsgLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubApplyMsgLayer")
local ClubBelowMemberLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubBelowMemberLayer")

-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubCaptainLayer = class("ClubCaptainLayer", TransitionLayer)



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
    "BTN_ADD",
    "BTN_LOW",
    "BTN_CTL",
    "BTN_FLAG",
    "BTN_DETAIL",
    "BTN_RECORD",
    "BTN_RESET",
    "BTN_BELOWM",
    "BTN_BELOWC",
    "BTN_SETPOINT",

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubCaptainLayer:ctor( scene, param, level )
    local this = self

    ClubCaptainLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/club_CaptainList.csb", self)
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


    -- 添加
    local btn = spbg:getChildByName("Button_add")
    btn:setTag(TAG_ENUM.BTN_ADD)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
       bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0 then
       btn:setTitleText("添加队长")
    else
       btn:setTitleText("添加下属队长")
    end

    self.m_addBtn  = btn

    -- 我的下属队员
    local btn = spbg:getChildByName("Button_low")
    btn:setTag(TAG_ENUM.BTN_LOW)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
       bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0 then
       btn:setVisible(false)
    end



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
                this:onItemButtonClickedEvent(ref:getTag(), ref,toViewPoint)  
            end  
        end
    end

    --搜索输入框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    -- 设置介绍

    local editbox = ccui.EditBox:create(cc.size(343, 52),"public/public_blank.png")
            :setPosition(935,58)
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


    self.datePanel = spbg:getChildByName("Panel_3")
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

    self.m_totalNum = {}
    for i=1,5 do
        self.m_totalNum[i] = spbg:getChildByName("Node_1"):getChildByName("num"..i)
        self.m_totalNum[i]:setString("0")

    end

    self.m_cPanel = spbg:getChildByName("Panel_2")
    self.m_cPanel:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
                 self.m_cPanel:setVisible(false)     
        end
    end )


    for i=1,5 do
        local btn = self.m_cPanel:getChildByName("Image_3"):getChildByName("Button_"..i)
        btn:setTag(TAG_ENUM.BTN_RECORD+(i-1))
        btn:addTouchEventListener( touchFunC )
        btn:setPressedActionEnabled(true)
    end



    self.m_tabDateRecord = {}
    self.m_tabCaptainList = {}
    self.m_curDay = 0

    self.curCaptain = param.captain or 0  --当前界面显示的是谁的队长列表


    self.clickCaptain = 0 --当前选中的是哪个队长
    self.m_bShow = {true,true} --大赢家，消耗
    self:onRefreshInfo(nil)


    if self.curCaptain ~= 0 and (self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
       bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0 ) then
        self.m_addBtn:setVisible(false)
    end

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画

    if param.intype ==1 then
        self:onTransitionInFinish()        
    else
        self:scaleTransitionIn(spbg)

    end

end

function ClubCaptainLayer:onExit()
    ClubCaptainLayer.super.onExit(self)
end

-- 按键监听
function ClubCaptainLayer:onButtonClickedEvent(tag,sender)
    
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
    elseif tag == TAG_ENUM.BTN_ADD then

        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or
          ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT10) or 
          bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 then
            local pa = ClubSetBelowPlayer:create(self, { captain = self.curCaptain,layerType = ClubSetBelowPlayer.LYAER_SETCAPTAIN}, ClubSetBelowPlayer.SECOND_LEVEL)
            self._scene:addPopLayer(pa)
        else
            showToast(self, "管理员权限未开启!", 2)
        end
    elseif tag == TAG_ENUM.BTN_LOW then
        local pa = ClubBelowMemberLayer:create(self._scene, {captain = self.curCaptain}, ClubBelowMemberLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
        
    elseif tag == TAG_ENUM.BTN_RECORD then
        self.m_cPanel:setVisible(false)
        local pa = ClubApplyMsgLayer:create(self._scene, {captainid = self.clickCaptain}, ClubApplyMsgLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif tag == TAG_ENUM.BTN_RESET then  

        self.m_cPanel:setVisible(false)
        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
         ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT11) or 
         bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 then
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendChangeRight(ClubFrame:getInstance().m_nCurrentClub,self.clickCaptain,cmd_Club.AUTHORITY_CAPTAIN,1)
        else
            showToast(self, "管理员权限未开启!", 2)
        end



    elseif tag == TAG_ENUM.BTN_BELOWC then  

        if bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 and self.curCaptain ~= self.m_myClubInfo.MemberInfo.dwMemberID then
            showToast(self, "权限不足，暂时无法查看!", 2)
            return
        end
        self.m_cPanel:setVisible(false)
        local pa = ClubCaptainLayer:create(self._scene, {captain = self.clickCaptain }, ClubCaptainLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif tag == TAG_ENUM.BTN_BELOWM then  

        if bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 and self.curCaptain ~= self.m_myClubInfo.MemberInfo.dwMemberID then
            showToast(self, "权限不足，暂时无法查看!", 2)
            return
        end

        self.m_cPanel:setVisible(false)
        local pa = ClubBelowMemberLayer:create(self._scene, {captain = self.clickCaptain}, ClubBelowMemberLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif tag == TAG_ENUM.BTN_SETPOINT then     
        self.m_cPanel:setVisible(false)
        self:showSetPointLayer(self.clickCaptain)
    end
end

function ClubCaptainLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubCaptainLayer:onTransitionInFinish()
    local date = os.date("%Y%m%d",os.time())

    self:updateCurDateRecord(tonumber(date))

end

function ClubCaptainLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubCaptainLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubCaptainLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

function ClubCaptainLayer:onFlagResult(dwUserID,cbFlag)
    for k,v in pairs(self.m_tabDateRecord.list) do
        if v and v.dwUserID == dwUserID then
            v.cbFlag = cbFlag
            break
        end
    end
    self:onRefreshInfo()
end


function ClubCaptainLayer:updateCurDateRecord(date)

    self.m_curDay = date or self.m_curDay
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabCaptainList = nil
    ClubFrame:getInstance():sendQueryCaptainList(ClubFrame:getInstance().m_nCurrentClub,GlobalUserItem.tabAccountInfo.dwUserID,self.curCaptain,self.m_curDay) 
end


function ClubCaptainLayer:onRefreshDateRecord(data)
    self.m_tabDateRecord = clone(data)

    self.m_totalNum[1]:setString(data.nCaptainCount.."")
    self.m_totalNum[2]:setString(data.nDrawCount.."")
    self.m_totalNum[3]:setString(data.nBigWinnerCount.."")
    self.m_totalNum[4]:setString(data.lRevenueCount.."")
    self.m_totalNum[5]:setString(data.lContributionCount.."")


    self:onRefreshInfo()
end






function ClubCaptainLayer:onRefreshInfo(id)

    self.m_tabCaptainList = {}

    if id and id ~= 0 then
        for k,v in pairs(self.m_tabDateRecord.list) do
            if v and v.dwGameID == id then
                table.insert(self.m_tabCaptainList, v)
                break
            end
        end
    else
        self.m_tabCaptainList = self.m_tabDateRecord.list or {}
    end


    if nil ~= id and 0 == #self.m_tabCaptainList then
        showToast(self, "没有找到该玩家，请确认玩家ID！", 2)
    end
    dump(self.m_tabCaptainList)

    if self.m_nCurrentClubInfo.GroupBattleRule.cbBigWinnerShow ==0 and 
        (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0) and 
         (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) == 0) then 

          self.m_bShow[1] =false
    end 


    if self.m_nCurrentClubInfo.GroupBattleRule.cbMedalMode ==0 or 
        (self.m_nCurrentClubInfo.GroupBattleRule.cbMedalRevenueShow ==0 and 
        (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0) and 
         (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) == 0)) then 

          self.m_bShow[2] =false
    end 


    --dump(self.m_tabMemberList, "self.m_tabClubInfo", 10)
    self._listMemberView:reloadData()
end

function ClubCaptainLayer:cellSizeForTable(view, idx)
    return 1190 , 90
end

function ClubCaptainLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local captainInfo = self.m_tabCaptainList[idx +1]
    if nil == captainInfo then
        return cell
    end


    local csb = ExternalFun.loadCSB("club/node_captain.csb", cell)
    csb:setAnchorPoint(cc.p(0,0))
    csb:setPosition(0, 0)

    --ID
    local txtID = csb:getChildByName("id")
    if nil ~= txtID then
        txtID:setString("ID:"..captainInfo.dwGameID)
    end
    --昵称
    local txtName = csb:getChildByName("name")

    txtName:setString(ExternalFun.clipText(captainInfo.szNickName,6,true))
    

    --约战次数
    local txtGameTimes = csb:getChildByName("jushu")
        txtGameTimes:setString(captainInfo.nPlayCount.."局")


    local maxWin = csb:getChildByName("maxWin")
        maxWin:setString(captainInfo.nBigWinnerCount.."")
        if not self.m_bShow[1] then

             maxWin:setString("-")
        end

    local lastScore = csb:getChildByName("gongxian")
        lastScore:setString(captainInfo.lRevenue.."")

        if not self.m_bShow[2] then
             lastScore:setString("-")
        end 

    local score = csb:getChildByName("score")
        score:setString(captainInfo.lContribution.."")


        local btn = csb:getChildByName("Button_2")
            btn:setTag(TAG_ENUM.BTN_FLAG)
            btn:addTouchEventListener(self.m_itemFunC)
            btn.captainInfo = captainInfo

        if captainInfo.nPlayCount ~= 0  then 
            btn:setVisible(true)
            if captainInfo.cbFlag ==0 then 
                btn:loadTextureNormal("club/dt_xin2.png")
                btn:loadTexturePressed("club/dt_xin2.png")
            else
                btn:loadTextureNormal("club/dt_xin1.png")
                btn:loadTexturePressed("club/dt_xin1.png")
            end
        else
            btn:setEnabled(false)
        end

        local btn = csb:getChildByName("btn_control")
        btn:setTag(TAG_ENUM.BTN_CTL)
        btn:addTouchEventListener(self.m_itemFunC)
        btn.captainInfo = captainInfo

        local btn = csb:getChildByName("btn_detail")
        btn:setTag(TAG_ENUM.BTN_DETAIL)
        btn:addTouchEventListener(self.m_itemFunC)
        btn.captainInfo = captainInfo


    return cell

end

-- 子视图数目
function ClubCaptainLayer:numberOfCellsInTableView(view)
    return #self.m_tabCaptainList
end

function ClubCaptainLayer:onItemButtonClickedEvent( tag, ref, point)
    local captainInfo = ref.captainInfo
    if nil == captainInfo then
        print("成员信息为空")
        return
    end

    if tag == TAG_ENUM.BTN_CTL then



        self.m_cPanel:setVisible(true)
        local img = self.m_cPanel:getChildByName("Image_3")
        if point.y >255 then
            img:loadTexture("club/captainMngBg1.png")
            img:setAnchorPoint(cc.p(0,0.87))
            img:setContentSize(cc.size(401,275))
        else
            img:loadTexture("club/captainMngBg2.png")
            img:setAnchorPoint(cc.p(0,0.12))
            img:setContentSize(cc.size(401,275))
        end
        img:setPositionY(point.y+113)

        if bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 then
            img:setContentSize(cc.size(401,195))
            img:getChildByName("Button_3"):setVisible(false)
            img:getChildByName("Button_4"):setVisible(false)
            img:getChildByName("Button_1"):setPositionY(140)
            img:getChildByName("Button_2"):setPositionY(140)
            
        end

        self.clickCaptain = captainInfo.dwUserID
    elseif tag == TAG_ENUM.BTN_FLAG then


        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or  --圈主
            ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT8) or --管理员
            ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT4) then --队长
           
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendSetStatisticFlag(ClubFrame:getInstance().m_nCurrentClub,captainInfo.dwUserID,self.m_curDay,(captainInfo.cbFlag+1)%2)
        else
            showToast(self, "点赞权限未开启！", 2)
        end
    elseif tag == TAG_ENUM.BTN_DETAIL then

    end

    -- local pa = ClubMemberRecord:create(self._scene, {dwQueryUserID = tagIMGroupMember.dwMemberID,dwDateID = self.m_curDay}, ClubMemberRecord.SECOND_LEVEL)
    --     self._scene:addPopLayer(pa)
    --------------test----------------
   -- ClubFrame:getInstance():sendDeleteGroup(Clubinfo.dwGroupID, 0)
    --ClubFrame:getInstance():sendApplyRequest(Clubinfo.dwGroupID, 0)
end






local BTN_CLOSE = 100
local BTN_IMG = 101

--设置点数
function ClubCaptainLayer:showSetPointLayer(dwCaptainID)
        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_setPoint.csb", self)

        local captainInfo = nil
        for i,v in ipairs(ClubFrame:getInstance().m_tabGroupMember) do
           --先判断大联盟
           if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
                local tabMember = v.memberInfo
                for j=1, #tabMember do
                        if tabMember[j].UserInfo.dwUserID == dwCaptainID then
                            captainInfo = clone(tabMember[j])
                        end
                end
           end
        end

        if not captainInfo then
            return 
        end
        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                    local touchTag = ref:getTag()
                    if touchTag == BTN_CLOSE then
                            rootLayer:removeFromParent()

                    elseif touchTag == BTN_IMG then
                                self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
                                self.m_editImg:setEnabled(false)
                    end

            end
        end

        local panel = csbNode:getChildByName("Panel_1")
        panel:addTouchEventListener(touchFunC)
        panel:setTag(BTN_CLOSE)

        local bg = csbNode:getChildByName("bg")

        local btn = bg:getChildByName("btn_close")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_CLOSE)


        bg:getChildByName("name"):setString(captainInfo.UserInfo.szNickName.."")
        bg:getChildByName("id"):setString(captainInfo.UserInfo.dwGameID.."")
        bg:getChildByName("stutas"):setString(captainInfo.MemberInfo.cbStatus==0 and "离线" or "在线")
        bg:getChildByName("per"):setString(captainInfo.MemberInfo.nAwardPoint.."%")
    

        local myMemInfo = GlobalUserItem.tabGroupUserInfo
        local curClubInfo = GlobalUserItem.tabCurGroupInfo
        local curPer = myMemInfo.MemberInfo.nAwardPoint

        if bit:_and(myMemInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0 then
            
            for i,v in ipairs(ClubFrame:getInstance().m_tabGroupMember) do
               --先判断大联盟
               if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
                    local tabMember = v.memberInfo
                    for j=1, #tabMember do
                            if tabMember[j].MemberInfo.dwMemberID == self.m_nCurrentClubInfo.GroupProperty.dwCreaterID then
                                curPer = tabMember[j].MemberInfo.nAwardPoint
                                break
                            end
                    end
               end
               break
            end
        end

        bg:getChildByName("mypoint"):setString(curPer.."%")



    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    local editbox = ccui.EditBox:create(cc.size(150, 50),"public/public_blank.png")
            :setPosition(719,326)
            :setFontColor(cc.c3b(255, 220, 25))
            :setAnchorPoint(cc.p(0,0.5))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 27)
            :setFont("fonts/round_body.ttf", 27) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(3)
    panel:addChild(editbox) 

    self.m_editBox = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)
    editbox:setText(captainInfo.MemberInfo.nAwardPoint.."")

    self.m_editPoint = bg:getChildByName("point")
    self.m_editPoint:setString(captainInfo.MemberInfo.nAwardPoint.."")

    self.m_editImg = bg:getChildByName("Image_11")
    self.m_editImg:setTag(BTN_IMG)
    self.m_editImg:addTouchEventListener( touchFunC )
    self.m_editImg:setTouchEnabled(true)


        local btn = bg:getChildByName("btn_ok")
        btn:addTouchEventListener(function (ref, tType)
            if tType == ccui.TouchEventType.ended then
                    local point = tonumber(self.m_editPoint:getString())

                    if not point or point <= captainInfo.MemberInfo.nAwardPoint or point > curPer or point >100 then
                        showToast(self, "请输入符合要求的点数！", 2)
                        return
                    end

                    ClubFrame:getInstance():sendQueryCaptainPoint(ClubFrame:getInstance().m_nCurrentClub,GlobalUserItem.tabAccountInfo.dwUserID,captainInfo.UserInfo.dwUserID,point)
                    rootLayer:removeFromParent()
            end
        end)

end

function ClubCaptainLayer:onEditEvent(event, editbox)

    local src = editbox:getText()     
    if editbox == self.m_editID then
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
                self.m_bBeginEdit = true
                if string.len(src) ~= 0 and src ~= "输入ID查询" then
                    self.serchID:setString(src)
                else
                    self.serchID:setString("输入ID查询")
                end
                self.serchID:setVisible(false)
            end
    elseif editbox == self.m_editBox then
            if event == "return" then
                self.m_editImg:setEnabled(true)
                self.m_editPoint:setString(src)
                if self.m_editPoint:getContentSize().width == 0 then --没有输入
                    self.m_editPoint:setString("0")
                end
                self.m_editPoint:setVisible(true)
            elseif event == "changed" then       
                if string.len(src) ~= 0 and src ~= "0" then
                    self.m_editPoint:setString(src)
                else
                    self.m_editPoint:setString("0")
                end

            elseif event == "began" then
                self.m_bBeginEdit = true
                if string.len(src) ~= 0 and src ~= "0" then
                    self.m_editPoint:setString(src)
                else
                    self.m_editPoint:setString("0")
                end
                self.m_editPoint:setVisible(false)
            end
    end

end


return ClubCaptainLayer