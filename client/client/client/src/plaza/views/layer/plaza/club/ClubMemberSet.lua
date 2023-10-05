
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")



-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubMemberSet = class("ClubMemberSet", TransitionLayer)



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
    "BTN_DEL",--移除队员
    "BTN_CHANGE",--多人修改分数
    "BTN_SELECT",--多人选中
    "BTN_CHANGECELL",--个人修改分数
    "BTN_SETAMD",  --设置管理员
    "BTN_DETAIL",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubMemberSet:ctor( scene, param, level )
    local this = self

    ClubMemberSet.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/club_MemberSet.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myGroupInfo = GlobalUserItem.tabGroupUserInfo


    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    --mask:setTag(TAG_ENUM.TAG_MASK)
    --mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --     local btn = spbg:getChildByName("btn_change")
    -- btn:setTag(TAG_ENUM.BTN_CHANGE)
    -- btn:addTouchEventListener( touchFunC )
    -- btn:setPressedActionEnabled(true)


        local btn = spbg:getChildByName("btn_del")
    btn:setTag(TAG_ENUM.BTN_DEL)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_btnDelete = btn

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


    local editbox = ccui.EditBox:create(cc.size(343, 52),"public/public_blank.png")
            :setPosition(799,75)
            :setFontColor(cc.c3b(181, 161, 212))
            :setAnchorPoint(cc.p(0,0.5))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 23)
            :setFont("fonts/round_body.ttf", 23) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    mask:addChild(editbox)         
    self.m_editID = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.serchID = spbg:getChildByName("Text_8")
    self.serchID:setString("输入ID查询")


    self.searchImg = spbg:getChildByName("Image_3")
    self.searchImg:setTag(TAG_ENUM.BTN_SERCH)
    self.searchImg:addTouchEventListener( touchFunC )
    self.searchImg:setTouchEnabled(true)

   

    --获取大联盟信息
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo



    self.m_tabSelectPlayer = {}


    self.m_isCreator = false

    local groupProperty = self.m_nCurrentClubInfo.GroupProperty
    if groupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID then
       self.m_isCreator = false
    else
       self.m_isCreator = true
    end

    -- self.m_isCaptain = false

    -- if self.m_myGroupInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN   then
    --     self.m_isCaptain = true 
    --     self.m_btnDelete:setTitleText("移除队员")
    -- end


   self:onRefreshInfo(nil)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)

    --ClubFrame:getInstance():setViewFrame(self)
    --ClubFrame:getInstance():sendQueryClubScore(ClubFrame:getInstance().m_nCurrentClub)
end

function ClubMemberSet:onExit()
    ClubMemberSet.super.onExit(self)
end

-- 按键监听
function ClubMemberSet:onButtonClickedEvent(tag,sender)
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
    elseif tag == TAG_ENUM.BTN_CHANGE then    

        if (bit:_and(self.m_myGroupInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) and not ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT3) then
            showToast(self, "队长权限未开启！", 2)
            return                
        end      


        dump(self.m_tabSelectPlayer,"=========99")
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
        self:showChangeScoreLayer(false,self.m_tabSelectPlayer)
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

function ClubMemberSet:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubMemberSet:onTransitionInFinish()


end

function ClubMemberSet:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubMemberSet:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubMemberSet:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

function ClubMemberSet:updateClubScore(score)
    
    self.m_spBg:getChildByName("clubScore"):setString(score.."")
end


function ClubMemberSet:updateCurDateRecord(_date)
    -- local date = _date or tonumber(os.date("%Y%m%d",os.time()))
    -- ClubFrame:getInstance():setViewFrame(self)
    -- ClubFrame:getInstance().m_tabCurDateRecord = {}
    -- ClubFrame:getInstance():sendQueryDateRecord(ClubFrame:getInstance().m_nCurrentClub, date)

end





function ClubMemberSet:onRefreshInfo(id)
        --获取当前选中的大联盟数据
        self.m_tabMemberList = {}
        self.m_tabSelectPlayer = {}
        for i,v in ipairs(ClubFrame:getInstance().m_tabGroupMember) do
           --先判断大联盟
           if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
                local tabMember = v.memberInfo
                for j=1, #tabMember do

                        table.insert(self.m_tabMemberList, tabMember[j])  

                end
           end
        end
        --排序
        for i, v in ipairs(self.m_tabMemberList) do

            if v.UserInfo.dwUserID == self.m_nCurrentClubInfo.GroupProperty.dwCreaterID then
                local clubHolder = v
                table.remove(self.m_tabMemberList, i)
                table.insert(self.m_tabMemberList, 1, clubHolder)
                break
            end
        end

        if nil ~= id and id ~= 0 then
                for k, v in ipairs(self.m_tabMemberList) do
                    if v and  v.UserInfo.dwGameID == id then
                        self.m_tabMemberList ={v}
                        break
                    end
                end
        end



    if nil ~= id and 0 == #self.m_tabMemberList then
        showToast(self, "没有找到该玩家，请确认玩家ID！", 2)
    end

    --dump(self.m_tabMemberList, "self.m_tabClubInfo", 10)
    self._listMemberView:reloadData()
end


function ClubMemberSet:onItemButtonClickedEvent( tag, ref )
    local Memberinfo = ref.Memberinfo
    if nil == Memberinfo then
        print("成员信息为空")
        return
    end

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo
    if tag == TAG_ENUM.BTN_SELECT then

        self.m_tabSelectPlayer[Memberinfo.index] = not self.m_tabSelectPlayer[Memberinfo.index]
        ref:setSelected(self.m_tabSelectPlayer[Memberinfo.index])

        dump(self.m_tabSelectPlayer,"=========33")
        local num = 0
        for k,v in pairs(self.m_tabSelectPlayer) do
            if v == true then
                num = num +1
            end
        end

        if num> 1 then         
            self.m_btnDelete:setTitleText("批量移除成员")    
        else
            self.m_btnDelete:setTitleText("移除成员")
            
        end

        


    elseif tag == TAG_ENUM.BTN_CHANGECELL then
        if (bit:_and(self.m_myGroupInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) and not ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT3) then
            showToast(self, "队长权限未开启！", 2)
            return                
        end 
        self:showChangeScoreLayer(true,{[1]=Memberinfo})
    elseif tag == TAG_ENUM.BTN_SETAMD then
        local right = cmd_Club.AUTHORITY_CAPTAIN
        local changeMode = 0

        if self.m_isCreator  then
            changeMode = (bit:_and(tagIMGroupMember.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0) and 1 or 0
            right = cmd_Club.AUTHORITY_MANAGER
        else
            changeMode = (bit:_and(tagIMGroupMember.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) and 1 or 0
            right = cmd_Club.AUTHORITY_CAPTAIN

            if changeMode ==1 and not ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT11)  then
                showToast(self, "权限未开启！", 2)
                return
            end

            if changeMode == 0 and not ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT10)  then
                showToast(self, "权限未开启！", 2)
                return 
            end

        end

        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendChangeRight(ClubFrame:getInstance().m_nCurrentClub,tagIMGroupMember.dwMemberID,right,changeMode)


    elseif tag == TAG_ENUM.BTN_DETAIL then
        local ClubSetLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubSetLayer")
        ClubSetLayer:showRightLayer(self,tagIMGroupMember.dwMemberID)
    end


    --------------test----------------
   -- ClubFrame:getInstance():sendDeleteGroup(Clubinfo.dwGroupID, 0)
    --ClubFrame:getInstance():sendApplyRequest(Clubinfo.dwGroupID, 0)
end



local BTN_ADD = 100
local BTN_SUB = 101
local BTN_IMG = 102
local BTN_CLOSE = 103
local BTN_OK = 104


function ClubMemberSet:showChangeScoreLayer(isSingle,tabPayer)
        self.m_isSingle = isSingle
        self.m_changePlayer = {}

        dump(tabPayer,"===============555")

        if self.m_isSingle then
            self.m_changePlayer[1] = tabPayer[1]
        end

        if not isSingle then
            for k,v in pairs(tabPayer) do
                if  v == true then
                    table.insert(self.m_changePlayer,self.m_tabMemberList[k])
                end
            end

            if  #self.m_changePlayer ==1 then
                self.m_isSingle = true
            end
        end


        dump(self.m_changePlayer,"===============666")

        local rootLayer, csbNode = ExternalFun.loadRootCSB(self.m_isSingle and "club/nclub_score2.csb" or "club/nclub_score1.csb", self)

        self.m_scoreLayer = rootLayer

        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                    self:onClicktEvent(ref:getTag(),ref)
            end
        end
        local panel = csbNode:getChildByName("Panel_1")
        panel:setTag(BTN_CLOSE)
        panel:addTouchEventListener(touchFunC)



        local bg = csbNode:getChildByName("bg")

        local btn = bg:getChildByName("btn_close")
        btn:setTag(BTN_CLOSE)
        btn:addTouchEventListener(touchFunC)


        local btn = bg:getChildByName("btn_ok")
        btn:setTag(BTN_OK)
        btn:addTouchEventListener(touchFunC)

        local btn = bg:getChildByName("Button_sub")
        btn:setTag(BTN_SUB)
        btn:addTouchEventListener(touchFunC)

        local btn = bg:getChildByName("Button_add")
        btn:setTag(BTN_ADD)
        btn:addTouchEventListener(touchFunC)



    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    local editbox = ccui.EditBox:create(cc.size(150, 50),"public/public_blank.png")
            :setPosition(630,171)
            :setFontColor(cc.c3b(255, 229, 78))
            :setAnchorPoint(cc.p(0,0.5))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 27)
            :setFont("fonts/round_body.ttf", 27) 
            --:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(7)
    panel:addChild(editbox)         
    self.m_editBox1 = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    if self.m_isSingle then
        editbox:setPosition(630,276)
    end

    self.m_editText1 = bg:getChildByName("number")
    self.m_editText1:setString("0")

    self.m_changeScore = 0

    self.m_editImg1 = bg:getChildByName("Image_1")
    self.m_editImg1:setTag(BTN_IMG)
    self.m_editImg1:addTouchEventListener( touchFunC )
    self.m_editImg1:setTouchEnabled(true)


    if self.m_isSingle then
        local name  = bg:getChildByName("Text_2")
        name:setString(ExternalFun.clipText(self.m_changePlayer[1].UserInfo.szNickName,6,true))

        local curScore  = bg:getChildByName("curscore")
        curScore:setString(self.m_changePlayer[1].MemberInfo.lScore.."")

        self.m_changeTxt  = bg:getChildByName("score")
        self.m_changeTxt:setString((self.m_changePlayer[1].MemberInfo.lScore +self.m_changeScore).."")

    else

        local tmp = bg:getChildByName("Panel_2")
        self._listScore = cc.TableView:create(tmp:getContentSize())
        self._listScore:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
        self._listScore:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
        self._listScore:setPosition(cc.p(0, 0))
        --self._listMemberView:setAnchorPoint(cc.p(0, 0))
        self._listScore:setDelegate()
        self._listScore:addTo(tmp)
        self._listScore:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
        self._listScore:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
        self._listScore:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
        self._listScore:reloadData()
    end
end

function ClubMemberSet:onClicktEvent(tag, sender)
    if tag == BTN_ADD  then
        self.m_changeScore = self.m_changeScore + 1
        self:updateScore()
    elseif tag == BTN_SUB  then
        self.m_changeScore = self.m_changeScore - 1

        self:updateScore()
    elseif tag == BTN_IMG  then
        self.m_editBox1:touchDownAction(self.m_editBox1, ccui.TouchEventType.ended)
        self.m_editImg1:setEnabled(false)
    elseif tag == BTN_CLOSE  then
        self.m_scoreLayer:removeFromParent()
    elseif tag == BTN_OK  then
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendChangeClubScore(ClubFrame:getInstance().m_nCurrentClub,math.abs(self.m_changeScore),self.m_changeScore > 0 and 0 or 1,self.m_changePlayer)
            self.m_scoreLayer:removeFromParent()
    end

end

function ClubMemberSet:updateScore()
        if self.m_isSingle then
            self.m_changeTxt:setString((self.m_changePlayer[1].MemberInfo.lScore +self.m_changeScore).."")
        else
            self._listScore:reloadData()
        end
        self.m_editText1:setString(self.m_changeScore.."")
end

function ClubMemberSet:onEditEvent(event, editbox)

    local src = editbox:getText() 
    if self.m_editID == editbox then 
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
            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3),cc.CallFunc:create(function ( ... )
                self.m_bBeginEdit = false
            end)))

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
    elseif self.m_editBox1 == editbox then
        if event == "return" then
            self.m_editImg1:setEnabled(true)
            self.m_editText1:setString(src)
            if self.m_editText1:getContentSize().width == 0 then --没有输入
                self.m_editText1:setString("0")
            end

            if tonumber(src)then 
                self.m_changeScore = tonumber(src)
                self:updateScore()
            else
                showToast(self, "请输入正确的数量！", 2)
            end
            self.m_editText1:setVisible(true)
            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3),cc.CallFunc:create(function ( ... )
                self.m_bBeginEdit = false
            end)))
        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= "0" then
                self.m_editText1:setString(src)
            else
                self.m_editText1:setString("0")
            end
        elseif event == "began" then
            self.m_bBeginEdit = true
            --self.m_editBox1:setText(self.m_editText1:getString())
            if string.len(src) ~= 0 and src ~= "0" then
                self.m_editText1:setString(src)
            else
                self.m_editText1:setString("0")
            end
            self.m_editText1:setVisible(false)
        end


    end

end


--移除用户

function ClubMemberSet:showRemovePlayerLayer(tabPayer)

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
        name:setString(ExternalFun.clipText(self.m_removePlayer[1].UserInfo.szNickName,6,true))

        local curScore  = bg:getChildByName("id")
        curScore:setString(self.m_removePlayer[1].UserInfo.dwGameID.."")

        self.m_changeTxt  = bg:getChildByName("score")
        self.m_changeTxt:setString((self.m_removePlayer[1].MemberInfo.lScore).."")

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

function ClubMemberSet:onRemoveLayerEvent(tag, sender)

    if tag == BTN_CLOSE  then
        self.m_removeLayer:removeFromParent()
    elseif tag == BTN_OK  then

        local delMember = {}
        for k,v in pairs(self.m_removePlayer) do
            if v then
                table.insert(delMember,v.MemberInfo.dwMemberID)
            end
        end

        if #delMember ==0 then
            showToast(self, "请选中至少一个成员！", 2)
            return 
        end
        
        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendDeleteMember(ClubFrame:getInstance().m_nCurrentClub, delMember)
        

        self.m_removeLayer:removeFromParent()
    end

end



function ClubMemberSet:cellSizeForTable(view, idx)
    if view ==  self._listScore then
         return 610 , 60       
    elseif view == self._listMemberView then 
         return 920 , 90
    elseif view == self._listPlayer then 
         return 610.5 , 62
    end
end

function ClubMemberSet:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    if view == self._listMemberView then 
            local Memberinfo = self.m_tabMemberList[idx +1]
            if nil == Memberinfo then
                return cell
            end
            dump(Memberinfo, "tableCellAtIndex", 10)

            local tagIMUserInfo = Memberinfo.UserInfo
            local tagIMGroupMember = Memberinfo.MemberInfo


            local csb = ExternalFun.loadCSB("club/node_memberCell.csb", cell)
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
            local stutas = csb:getChildByName("status")
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


                local checkbtn = csb:getChildByName("CheckBox_1")
                checkbtn:setTag(TAG_ENUM.BTN_SELECT)
                checkbtn:addTouchEventListener(self.m_itemFunC)
                checkbtn.Memberinfo = Memberinfo
                checkbtn:setSelected(false)

                Memberinfo.index = idx +1

                if self.m_tabSelectPlayer[idx +1] == true then
                    checkbtn:setSelected(true)
                end

                --如果是盟主，不会被选中 如果我是管理员，不能选中盟主和管理员
                if tagIMGroupMember.nMemberRight == cmd_Club.AUTHORITY_OWNER or (tagIMGroupMember.nMemberRight == cmd_Club.AUTHORITY_MANAGER and self.m_myGroupInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_MANAGER) then
                    checkbtn:setVisible(false)
                end

                local memberRight= {[cmd_Club.AUTHORITY_OWNER] = "盟主",
                                    [cmd_Club.AUTHORITY_MANAGER] = "管理员",
                                    [cmd_Club.AUTHORITY_CAPTAIN] = "队长",
                                    [cmd_Club.AUTHORITY_TEAMMEMBER] = "队员"}

                local right = csb:getChildByName("right")
                right:setString(tagIMGroupMember.nMemberRight==0 and "成员" or (memberRight[tagIMGroupMember.nMemberRight] or "成员"))


                local isCreator = self.m_nCurrentClubInfo.GroupProperty.dwCreaterID ==  tagIMUserInfo.dwUserID  --当前玩家是否为圈主

                local rightbtn = csb:getChildByName("btn_admset")
                rightbtn:setTag(TAG_ENUM.BTN_SETAMD)
                rightbtn:addTouchEventListener(self.m_itemFunC)
                rightbtn.Memberinfo = Memberinfo

                if isCreator then
                    rightbtn:setVisible(false)
                end

                local setbtn = csb:getChildByName("btn_detail")
                setbtn:setTag(TAG_ENUM.BTN_DETAIL)
                setbtn:addTouchEventListener(self.m_itemFunC)
                setbtn.Memberinfo = Memberinfo
                setbtn:setVisible(false)


                if self.m_isCreator  then
                    if (bit:_and(tagIMGroupMember.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0) then
                            rightbtn:setTitleText("解除管理员")
                            setbtn:setVisible(true)
                    elseif tagIMGroupMember.nMemberRight == cmd_Club.AUTHORITY_MEMBER then
                            rightbtn:setTitleText("设为管理员")
                            setbtn:setVisible(false)
                    else
                        rightbtn:setVisible(false)
                    end
                 else
                    if (bit:_and(tagIMGroupMember.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) then
                            rightbtn:setTitleText("解除队长")

                            if ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT9) then
                                setbtn:setVisible(true)
                            end
                    elseif tagIMGroupMember.nMemberRight == cmd_Club.AUTHORITY_MEMBER then
                            rightbtn:setTitleText("设为队长")
                            setbtn:setVisible(false)
                    else
                            rightbtn:setVisible(false)
                    end

                    if tagIMGroupMember.dwMemberID == GlobalUserItem.tabAccountInfo.dwUserID then
                        setbtn:setVisible(false)
                        rightbtn:setVisible(false)
                    end

                end


    elseif view == self._listScore then 
                local csb = ExternalFun.loadCSB("club/node_score1.csb", cell)
                csb:setAnchorPoint(cc.p(0,0))
                csb:setPosition(0, 0)

                csb:getChildByName("name"):setString(ExternalFun.clipText(self.m_changePlayer[idx+1].UserInfo.szNickName,6,true))
                csb:getChildByName("curscore"):setString(self.m_changePlayer[idx+1].MemberInfo.lScore.."")
                csb:getChildByName("score"):setString((self.m_changePlayer[idx+1].MemberInfo.lScore +self.m_changeScore) .."")
    elseif view == self._listPlayer then 

                local csb = ExternalFun.loadCSB("club/node_remove.csb", cell)
                csb:setAnchorPoint(cc.p(0,0))
                csb:setPosition(0, 0)

                local name = csb:getChildByName("name")
                name:setString(ExternalFun.clipText(self.m_removePlayer[idx+1].UserInfo.szNickName,6,true))

                csb:getChildByName("id"):setString("ID:"..self.m_removePlayer[idx+1].UserInfo.dwGameID)

                csb:getChildByName("score"):setString(self.m_removePlayer[idx+1].MemberInfo.lScore.."")

    end

    return cell

end

-- 子视图数目
function ClubMemberSet:numberOfCellsInTableView(view)
    if view == self._listScore then
        return #self.m_changePlayer
    elseif view == self._listPlayer then
        return #self.m_removePlayer
    elseif view == self._listMemberView then
        return #self.m_tabMemberList
    end
end



function ClubMemberSet:onDeleteMember()
    self:onRefreshInfo(nil)

    -- 通知更新    
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

return ClubMemberSet