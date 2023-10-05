
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

-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubBelowMemberLayer = class("ClubBelowMemberLayer", TransitionLayer)



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
    "BTN_SETMEMBER",
    "BTN_DETAIL",
    "BTN_FLAG",
    "BTN_REMOVE",
    "BTN_CONTROL",
    "BTN_SELECT",
    "BTN_DEL",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubBelowMemberLayer:ctor( scene, param, level )
    local this = self

    ClubBelowMemberLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/club_captainLow.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._scene = scene
    self.m_tabMemberList = {}
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

    -- 调配成员
    local btn = spbg:getChildByName("Button_set")
    btn:setTag(TAG_ENUM.BTN_SETMEMBER)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_setBtn = btn

    if param.captain == 0 then
        self.m_setBtn:setVisible(false)
    end

    if self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN then
        self.m_setBtn:setTag(TAG_ENUM.BTN_DEL)
        self.m_setBtn:setTitleText("移除队员")
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


    self.m_tabSelectPlayer = {}

    self.m_memberInfo ={}
    for k,v in pairs(ClubFrame:getInstance().m_tabGroupMember) do
            if v and v.groupID == ClubFrame:getInstance().m_nCurrentClub then
                    self.m_memberInfo = v.memberInfo
            end
     end


    -- -- 创建的大联盟
    -- self.m_haveCap = false
    -- local checkbox = spbg:getChildByName("CheckBox_1")
    -- checkbox:addEventListener(function (sender,eventType)
    --     if eventType == ccui.TouchEventType.ended then
    --                 self.m_haveCap = not self.m_haveCap 
    --                 sender:setSelected(self.m_haveCap)
    --                 self:onRefreshInfo()
    --     end    
    -- end)
    -- checkbox:setSelected(false)


    self.m_isNoEmpty = false
    local checkbox = spbg:getChildByName("CheckBox_2")
    checkbox:addTouchEventListener(function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
                    self.m_isNoEmpty = not self.m_isNoEmpty 
                    sender:setSelected(self.m_isNoEmpty)
                    self:onRefreshInfo()
        end    
    end)
    checkbox:setSelected(false)


    self.m_cPanel = spbg:getChildByName("Panel_3")
    self.m_cPanel:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
                 self.m_cPanel:setVisible(false)     
        end
    end )

    local btn = self.m_cPanel:getChildByName("Image_3"):getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_REMOVE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = self.m_cPanel:getChildByName("Image_3"):getChildByName("Button_2")
    btn:setTag(TAG_ENUM.BTN_DETAIL)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.curCaptain = param.captain or 0

    self.clickMember = 0 --当前选中的是哪个成员

    self.m_bShow = {true,true} --大赢家，消耗
    self:onRefreshInfo(nil)

    self:onTransitionInFinish()
    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)


end

function ClubBelowMemberLayer:onExit()
    ClubBelowMemberLayer.super.onExit(self)
end

-- 按键监听
function ClubBelowMemberLayer:onButtonClickedEvent(tag,sender)
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
    elseif tag == TAG_ENUM.BTN_SETMEMBER then    
        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or  
            ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT13) then        
            local pa = ClubSetBelowPlayer:create(self, { captain = self.curCaptain,layerType = ClubSetBelowPlayer.LYAER_SETMEMBER}, ClubSetBelowPlayer.SECOND_LEVEL)
            self._scene:addPopLayer(pa)
        else
            showToast(self, "管理员权限未开启!", 2)
        end

    elseif tag == TAG_ENUM.BTN_REMOVE then 
        self.m_cPanel:setVisible(false)

        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or  
            ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT12)  or 
            ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT5) then
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendSetMemberChange(ClubFrame:getInstance().m_nCurrentClub,GlobalUserItem.tabAccountInfo.dwUserID,self.curCaptain,{self.clickMember},0)
        else
            showToast(self, "权限未开启!", 2)
        end

    elseif tag == TAG_ENUM.BTN_DETAIL then
        self.m_cPanel:setVisible(false)
        local pa = ClubMemberRecord:create(self._scene, {dwQueryUserID = self.clickMember,dwDateID = self.m_curDay}, ClubMemberRecord.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    elseif tag == TAG_ENUM.BTN_DEL then 
   

        local flag = false
        for k,v in pairs(self.m_tabSelectPlayer) do
            if v == true then
                flag = true
            end
        end
        if not flag then
            showToast(self, "请选中至少一个成员！", 2)
            return 
        end
        self:showRemovePlayerLayer(self.m_tabSelectPlayer)

    end
end

function ClubBelowMemberLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubBelowMemberLayer:onTransitionInFinish()
    local date = os.date("%Y%m%d",os.time())

    self:updateCurDateRecord(tonumber(date))

end

function ClubBelowMemberLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubBelowMemberLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubBelowMemberLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end




function ClubBelowMemberLayer:updateCurDateRecord(date)

    self.m_curDay = date or self.m_curDay

    print(self.m_curDay.."==========")
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabTeamerRecord = {}
    ClubFrame:getInstance():sendQueryTeamRecord(ClubFrame:getInstance().m_nCurrentClub,GlobalUserItem.tabAccountInfo.dwUserID,self.curCaptain,self.m_curDay)
end


function ClubBelowMemberLayer:onRefreshDateRecord(data)
    self.m_tabDateRecord = clone(data) or {}
    --dump(self.m_tabDateRecord,"0000==")
    --self._listMemberView:reloadData()

    self:onRefreshInfo()
end



--移除用户

local BTN_CLOSE = 101
local BTN_OK = 102

function ClubBelowMemberLayer:showRemovePlayerLayer(tabPayer)

        self.m_isSingle = false
        self.m_removePlayer = {}

        dump(tabPayer,"===============555")

        for k,v in pairs(tabPayer) do
            if  v == true then
                table.insert(self.m_removePlayer,self.m_tabMemberList[k])
            end
        end

        if  #self.m_removePlayer ==1 then
            self.m_isSingle = true
        end


        dump(self.m_removePlayer,"===============666")

        local rootLayer, csbNode = ExternalFun.loadRootCSB(self.m_isSingle and "club/nclub_remove1.csb" or "club/nclub_remove2.csb", self)

        self.m_removeLayer = rootLayer

        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                    self:onRemoveLayerEvent(ref:getTag(),ref)
            end
        end
        local panel = csbNode:getChildByName("Panel_1")
        panel:setTag(BTN_CLOSE)
        panel:addTouchEventListener(touchFunC)

        local bg = csbNode:getChildByName("bg")

        bg:getChildByName("Image_2"):getChildByName("Sprite_3"):setTexture("club/remove_title1.png")



        local btn = bg:getChildByName("btn_close")
        btn:setTag(BTN_CLOSE)
        btn:addTouchEventListener(touchFunC)


        local btn = bg:getChildByName("btn_ok")
        btn:setTag(BTN_OK)
        btn:addTouchEventListener(touchFunC)

        local btn = bg:getChildByName("btn_cancel")
        btn:setTag(BTN_CLOSE)
        btn:addTouchEventListener(touchFunC)

    if self.m_isSingle then
        local name  = bg:getChildByName("name")
        name:setString(ExternalFun.clipText(self.m_removePlayer[1].szNickName,6,true))

        local curScore  = bg:getChildByName("id")
        curScore:setString(self.m_removePlayer[1].dwGameID.."")

        self.m_changeTxt  = bg:getChildByName("score")
        self.m_changeTxt:setString((self.m_removePlayer[1].lContribution).."")

    else
        local tmp = bg:getChildByName("Panel_5")
        self._listPlayer = cc.TableView:create(tmp:getContentSize())
        self._listPlayer:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
        self._listPlayer:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
        self._listPlayer:setPosition(cc.p(0, 0))
        --self._listMemberView:setAnchorPoint(cc.p(0, 0))
        self._listPlayer:setDelegate()
        self._listPlayer:addTo(tmp)
        self._listPlayer:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
        self._listPlayer:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
        self._listPlayer:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
        self._listPlayer:reloadData()
    end

end

function ClubBelowMemberLayer:onRemoveLayerEvent(tag, sender)

    if tag == BTN_CLOSE  then
        self.m_removeLayer:removeFromParent()
    elseif tag == BTN_OK  then

        local delMember = {}
        for k,v in pairs(self.m_removePlayer) do
            if v then
                table.insert(delMember,v.dwMemberID)
            end
        end

        if #delMember ==0 then
            showToast(self, "请选中至少一个成员！", 2)
            return 
        end
        
        ClubFrame:getInstance():setViewFrame(self)
        for i=1, #delMember do
            ClubFrame:getInstance():sendChangeRight(ClubFrame:getInstance().m_nCurrentClub,delMember[i],cmd_Club.AUTHORITY_TEAMMEMBER,1)
        end
            
        self.m_removeLayer:removeFromParent()
    end

end





function ClubBelowMemberLayer:onRefreshInfo(id)
    --获取当前选中的大联盟数据
    self.m_tabMemberList = clone(self.m_tabDateRecord)
    self.m_tabSelectPlayer = {}

    if id and id ~= 0 then
            local temp = {}
            for i,v in pairs(self.m_tabMemberList) do
                if v and v.dwGameID == id then
                    table.insert(temp,v)
                    break
                end
            end
            self.m_tabMemberList = temp
    end


    if self.m_isNoEmpty then
        for i=#self.m_tabMemberList,1,-1 do
            if self.m_tabMemberList[i].nPlayCount == 0 then
                table.remove(self.m_tabMemberList,i)
            end
        end
    end




        --排序
    for i, v in pairs(self.m_tabMemberList) do
        for j,m in pairs(self.m_memberInfo) do
            if v.dwMemberID == m.MemberInfo.dwMemberID then
                if bit:_and(m.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 then
                    local captain = clone(v)
                    table.remove(self.m_tabMemberList, i)
                    table.insert(self.m_tabMemberList, 1, captain)
                end
            end
        end
    end



    -- --去掉队长及成员
    -- if not self.m_haveCap then
    --     local curGroupMember = {}

    --     for k,v in ipairs(ClubFrame:getInstance().m_tabGroupMember) do
    --            --先判断大联盟
    --            if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
    --                 curGroupMember= v.memberInfo
    --            end
    --      end

    --     for i=#self.m_tabMemberList,1,-1 do
    --             for n, m in ipairs(curGroupMember) do
    --                 if self.m_tabMemberList[i].dwMemberID == m.MemberInfo.dwMemberID and bit:_and(m.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 then
    --                     table.remove(self.m_tabMemberList,i)
    --                     break
    --                 end
    --             end
    --      end
    -- end

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
        (self.m_nCurrentClubInfo.GroupBattleRule.cbMedalRevenueShow ==0 and 
        (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0) and 
         (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) == 0)) then 

          self.m_bShow[2] =false
    end 


    --dump(self.m_tabMemberList, "self.m_tabClubInfo", 10)
    self._listMemberView:reloadData()
end

function ClubBelowMemberLayer:cellSizeForTable(view, idx)
    if view == self._listPlayer then 
         return 610.5 , 62
    elseif view == self._listMemberView then 
        return 1190 , 90
    end
end

function ClubBelowMemberLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    if view == self._listMemberView  then 
            local Memberinfo = self.m_tabMemberList[idx +1]
            if nil == Memberinfo then
                return cell
            end

            local csb = ExternalFun.loadCSB("club/node_captainLow.csb", cell)
            csb:setAnchorPoint(cc.p(0,0))
            csb:setPosition(0, 0)

            --ID
            local txtID = csb:getChildByName("id")
            if nil ~= txtID then
                txtID:setString("ID:"..Memberinfo.dwGameID)
            end
            --昵称
            local txtName = csb:getChildByName("name")

            txtName:setString(ExternalFun.clipText(Memberinfo.szNickName,6,true))


            --约战次数
            local txtGameTimes = csb:getChildByName("jushu")
                txtGameTimes:setString(Memberinfo.nPlayCount.."局")


            local maxWin = csb:getChildByName("maxScore")
                maxWin:setString(Memberinfo.nBigWinnerCount.."")
                if not self.m_bShow[1] then

                     maxWin:setString("-")
                end

            local score1 = csb:getChildByName("score1")
                score1:setString(Memberinfo.lRevenue.."")

                if not self.m_bShow[2] then

                     score1:setString("-")
                end 

            local score2 = csb:getChildByName("score2")
                score2:setString(Memberinfo.lContribution.."")

                local detailbtn = csb:getChildByName("btn_detail")
                detailbtn:setTag(TAG_ENUM.BTN_DETAIL)
                detailbtn:addTouchEventListener(self.m_itemFunC)
                detailbtn.Memberinfo = Memberinfo

                local controlbtn = csb:getChildByName("btn_control")
                controlbtn:setTag(TAG_ENUM.BTN_CONTROL)
                controlbtn:addTouchEventListener(self.m_itemFunC)
                controlbtn.Memberinfo = Memberinfo    

                if self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN then
                    detailbtn:setVisible(true)
                    controlbtn:setVisible(false)
                else
                    detailbtn:setVisible(false)
                    controlbtn:setVisible(true)
                end    

                local btn = csb:getChildByName("Button_2")
                btn:setTag(TAG_ENUM.BTN_FLAG)
                btn:addTouchEventListener(self.m_itemFunC)
                btn.Memberinfo = Memberinfo

                if Memberinfo.nPlayCount ~= 0  then 
                    btn:setEnabled(true)
                    if Memberinfo.cbFlag ==0 then 
                        btn:loadTextureNormal("club/dt_xin2.png")
                        btn:loadTexturePressed("club/dt_xin2.png")
                    else
                        btn:loadTextureNormal("club/dt_xin1.png")
                        btn:loadTexturePressed("club/dt_xin1.png")
                    end
                else
                    btn:setEnabled(false)
                end

                    local checkbtn = csb:getChildByName("CheckBox_2")
                    checkbtn:setTag(TAG_ENUM.BTN_SELECT)
                    checkbtn:addTouchEventListener(self.m_itemFunC)
                    checkbtn.Memberinfo = Memberinfo
                    checkbtn:setSelected(false)

                    Memberinfo.index = idx +1

                    if self.m_tabSelectPlayer[idx +1] == true then
                        checkbtn:setSelected(true)
                    end

            if self.m_myClubInfo.MemberInfo.nMemberRight ~= cmd_Club.AUTHORITY_CAPTAIN  or Memberinfo.dwMemberID == GlobalUserItem.tabAccountInfo.dwUserID  then
                checkbtn:setVisible(false)
            end

            if self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN and Memberinfo.dwMemberID == GlobalUserItem.tabAccountInfo.dwUserID then
                csb:getChildByName("Sprite_4"):setVisible(true)
            end


    elseif view == self._listPlayer  then
        
            local csb = ExternalFun.loadCSB("club/node_remove.csb", cell)
            csb:setAnchorPoint(cc.p(0,0))
            csb:setPosition(0, 0)

            local name = csb:getChildByName("name")
            name:setString(ExternalFun.clipText(self.m_removePlayer[idx+1].szNickName,6,true))

            csb:getChildByName("id"):setString("ID:"..self.m_removePlayer[idx+1].dwGameID)

            csb:getChildByName("score"):setString(self.m_removePlayer[idx+1].lContribution.."")
    end

    return cell

end

-- 子视图数目
function ClubBelowMemberLayer:numberOfCellsInTableView(view)
    

    if view == self._listPlayer then 
         return #self.m_removePlayer
    elseif view == self._listMemberView then 
        return #self.m_tabMemberList
    end

end

function ClubBelowMemberLayer:onItemButtonClickedEvent( tag, ref ,point)
    local Memberinfo = ref.Memberinfo
    if nil == Memberinfo then
        print("成员信息为空")
        return
    end
    if tag == TAG_ENUM.BTN_FLAG then

        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
            ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT8) or 
            ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT4) then
            
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendSetStatisticFlag(ClubFrame:getInstance().m_nCurrentClub,Memberinfo.dwMemberID,self.m_curDay,(Memberinfo.cbFlag+1)%2)
        else
            showToast(self, "点赞权限未开启！", 2)
        end

    elseif tag == TAG_ENUM.BTN_DETAIL then
            local pa = ClubMemberRecord:create(self._scene, {dwQueryUserID = Memberinfo.dwMemberID,dwDateID = self.m_curDay}, ClubMemberRecord.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif tag == TAG_ENUM.BTN_CONTROL then   
        self.m_cPanel:setVisible(true)
        local img = self.m_cPanel:getChildByName("Image_3")
        img:setPositionY(point.y+93)
        self.clickMember = Memberinfo.dwMemberID
    elseif tag == TAG_ENUM.BTN_SELECT then 
        self.m_tabSelectPlayer[Memberinfo.index] = not self.m_tabSelectPlayer[Memberinfo.index]
        ref:setSelected(self.m_tabSelectPlayer[Memberinfo.index])

        
        local num = 0
        for k,v in pairs(self.m_tabSelectPlayer) do
            if v == true then
                num = num +1
            end
        end

        if num> 1 then         
            self.m_setBtn:setTitleText("批量移除队员") 
        else
            self.m_setBtn:setTitleText("移除队员")
            
        end
    end

end

function ClubBelowMemberLayer:onFlagResult(dwUserID,cbFlag)
    print("==============="..dwUserID..",=="..cbFlag)
    for k,v in pairs(self.m_tabDateRecord) do
        if v and v.dwMemberID == dwUserID then
            v.cbFlag = cbFlag
            break
        end
    end
    self:onRefreshInfo()
end






function ClubBelowMemberLayer:onEditEvent(event, editbox)

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


function ClubBelowMemberLayer:onDeleteMember()
    self:onRefreshInfo(nil)

    -- 通知更新    
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

return ClubBelowMemberLayer