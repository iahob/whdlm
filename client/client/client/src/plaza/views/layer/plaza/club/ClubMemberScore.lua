
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local ClubApplyMsgLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubApplyMsgLayer")



-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubMemberScore = class("ClubMemberScore", TransitionLayer)



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
    "BTN_CHANGE1",--多人增加分数
    "BTN_CHANGE2",--多人减少分数
    "BTN_SELECT",--多人选中
    "BTN_CHANGEADD",--个人增加分数
    "BTN_CHANGESUB",--个人减少分数   
    "BTN_SETAMD",  --设置管理员
    "BTN_DETAIL",
    "BTN_RECORD",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubMemberScore:ctor( scene, param, level )
    local this = self

    ClubMemberScore.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/club_MemberScore.csb", self)
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

    self.m_emptyTip = spbg:getChildByName("Text_2")


    -- 关闭
    local btn = spbg:getChildByName("close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

        local btn = spbg:getChildByName("btn_change1")
    btn:setTag(TAG_ENUM.BTN_CHANGE1)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_change1 = btn

        local btn = spbg:getChildByName("btn_change2")
    btn:setTag(TAG_ENUM.BTN_CHANGE2)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)    

    self.m_change2 = btn



        local btn = spbg:getChildByName("btn_record")
    btn:setTag(TAG_ENUM.BTN_RECORD)
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
            :setPosition(886,75)
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

    self.m_isCaptain = false
   
   if bit:_and(self.m_myGroupInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 then 
            self.m_isCaptain = true
   end


    self.m_lastScore = 0

   self:onRefreshInfo(nil)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)

    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance():sendQueryClubScore(ClubFrame:getInstance().m_nCurrentClub)
end

function ClubMemberScore:onExit()
    ClubMemberScore.super.onExit(self)
end

-- 按键监听
function ClubMemberScore:onButtonClickedEvent(tag,sender)
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

    elseif tag == TAG_ENUM.BTN_RECORD then
        local pa = ClubApplyMsgLayer:create(self._scene, {captainid = GlobalUserItem.tabAccountInfo.dwUserID}, ClubApplyMsgLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif tag == TAG_ENUM.BTN_CHANGE1 or tag == TAG_ENUM.BTN_CHANGE2 then    

        if (bit:_and(self.m_myGroupInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) and not ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT3) then
            showToast(self, "队长权限未开启！", 2)
            return                
        end      

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
        self:showChangeScoreLayer(false,self.m_tabSelectPlayer,tag == TAG_ENUM.BTN_CHANGE1 and 1 or 2)
    elseif tag == TAG_ENUM.BTN_DEL then 
        if (bit:_and(self.m_myGroupInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) and not ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT5) then
            showToast(self, "队长权限未开启！", 2)
            return                
        end      

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

function ClubMemberScore:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubMemberScore:onTransitionInFinish()


end

function ClubMemberScore:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubMemberScore:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubMemberScore:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

function ClubMemberScore:updateClubScore(score)
    
    self.m_spBg:getChildByName("clubScore"):setString(score.."")
    self.m_lastScore = score
end


function ClubMemberScore:updateCurDateRecord(_date)

end





function ClubMemberScore:onRefreshInfo(id)



        --获取当前选中的大联盟数据
        self.m_tabMemberList = {}
        self.m_tabSelectPlayer = {}
        for i,v in ipairs(ClubFrame:getInstance().m_tabGroupMember) do
           --先判断大联盟
           if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
                local tabMember = v.memberInfo
                for j=1, #tabMember do
                    if self.m_isCaptain  then
                        if tabMember[j].MemberInfo.dwCaptainID == self.m_myGroupInfo.MemberInfo.dwMemberID then
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

            if v.UserInfo.dwUserID == self.m_nCurrentClubInfo.GroupProperty.dwCreaterID or (self.m_isCaptain and v.UserInfo.dwUserID == self.m_myGroupInfo.MemberInfo.dwMemberID) then
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

        local num = 0
        for k,v in pairs(self.m_tabSelectPlayer) do
            if v == true then
                num = num +1
            end
        end

    if num> 1 then         
        self.m_change1:setTitleText("批量增加勋章")    
        self.m_change2:setTitleText("批量减少勋章")  
    else
        self.m_change1:setTitleText("增加勋章")
        self.m_change2:setTitleText("减少勋章")  
    end

    if nil ~= id and 0 == #self.m_tabMemberList then
        showToast(self, "没有找到该玩家，请确认玩家ID！", 2)
    end

    if self.m_isCaptain and 0 == #self.m_tabMemberList then
        self.m_emptyTip:setVisible(true)
    else
        self.m_emptyTip:setVisible(false)
    end

    --dump(self.m_tabMemberList, "self.m_tabClubInfo", 10)
    self._listMemberView:reloadData()
end


function ClubMemberScore:onItemButtonClickedEvent( tag, ref )
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
            self.m_change1:setTitleText("批量增加勋章")    
            self.m_change2:setTitleText("批量减少勋章")  
        else
            self.m_change1:setTitleText("增加勋章")
            self.m_change2:setTitleText("减少勋章")  
        end



    elseif tag == TAG_ENUM.BTN_CHANGEADD or tag == TAG_ENUM.BTN_CHANGESUB then

        if (bit:_and(self.m_myGroupInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) and not ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT3) then
            showToast(self, "队长权限未开启！", 2)
            return                
        end 
        self:showChangeScoreLayer(true,{[1]=Memberinfo},tag == TAG_ENUM.BTN_CHANGEADD and 1 or 2)
    elseif tag == TAG_ENUM.BTN_SETAMD then
        local right = cmd_Club.AUTHORITY_CAPTAIN
        local changeMode = 0

        if self.m_isCaptain  then
            changeMode = (bit:_and(tagIMGroupMember.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) and 1 or 0
            right = cmd_Club.AUTHORITY_CAPTAIN
        else
            changeMode = (bit:_and(tagIMGroupMember.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0) and 1 or 0
            right = cmd_Club.AUTHORITY_MANAGER
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
local BTN_BACK = 105
local BTN_DELETE = 106
local BTN_SURE = 107
local BTN_ALL = 108


function ClubMemberScore:showChangeScoreLayer(isSingle,tabPayer,scoreType)
        self.m_isSingle = isSingle
        self.m_scoreType = scoreType
        self.m_changePlayer = {}

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


        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_addScore.csb", self)

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

        self.m_scoreString = bg:getChildByName("Text_2")

        self.m_scoreTipStr = self.m_scoreType== 1 and "请输入用户所需增加的勋章数" or "请输入用户所需减少的勋章数"

        self.m_scoreString:setString(self.m_scoreTipStr)

        for i=1,10 do
            --数字键盘
            local btn  = bg:getChildByName("Button_" .. (i-1))
            btn:setTag(i -1)
            btn:addTouchEventListener( touchFunC )
            --btn:setPressedActionEnabled(true)
        end


        local btn  = bg:getChildByName("Button_b")
        btn:setTag(BTN_BACK)
        btn:addTouchEventListener( touchFunC )
        btn:setPressedActionEnabled(true)

        local btn  = bg:getChildByName("Button_x")
        btn:setTag(BTN_DELETE)
        btn:addTouchEventListener( touchFunC )
        btn:setPressedActionEnabled(true)


        local btn  = bg:getChildByName("Button_10")
        btn:setTag(BTN_SURE)
        btn:addTouchEventListener( touchFunC )
        btn:setPressedActionEnabled(true)

        local btn  = bg:getChildByName("Button_11")
        btn:setTag(BTN_ALL)
        btn:addTouchEventListener( touchFunC )
        btn:setPressedActionEnabled(true)

        if not self.m_isSingle then
            btn:setVisible(false)
        end

end

function ClubMemberScore:onClicktEvent(tag, sender)
    if tag >= 0 and tag <= 10 then
        local str = self.m_scoreString:getString()
        if  string.len(str) ==9 then
            showToast(self,"最多输入9位数字！", 2)
            return
        end

        if str == self.m_scoreTipStr then
            str = tag..""
        else
            str = str..tag..""
        end
        
        self.m_scoreString:setString(str)

    elseif tag == BTN_BACK then

        local str = self.m_scoreString:getString()
        if str == self.m_scoreTipStr  or string.len(str) == 1 then 
            self.m_scoreString:setString(self.m_scoreTipStr)
            return
        end
        str = string.sub(str,1,string.len(str)-1)

        self.m_scoreString:setString(str)

    elseif tag == BTN_DELETE then

        self.m_scoreString:setString(self.m_scoreTipStr)
    elseif tag ==  BTN_ALL then 
        if self.m_scoreType ==1 then

            self.m_scoreString:setString(self.m_lastScore)
        else
            self.m_scoreString:setString(self.m_changePlayer[1].MemberInfo.lScore)
        end
        
    elseif tag == BTN_CLOSE  then
        self.m_scoreLayer:removeFromParent()
    elseif tag == BTN_SURE  then

        local str = self.m_scoreString:getString()
        if tonumber(str) and str ~= self.m_scoreTipStr then 
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendChangeClubScore(ClubFrame:getInstance().m_nCurrentClub,tonumber(str),self.m_scoreType ==1 and 0 or 1,self.m_changePlayer)
            self.m_scoreLayer:removeFromParent()

            self.m_tabSelectPlayer = {}
            self:onRefreshInfo()
        end
    end

end

function ClubMemberScore:updateScore()

end

function ClubMemberScore:onEditEvent(event, editbox)

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

function ClubMemberScore:showRemovePlayerLayer(tabPayer)

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

function ClubMemberScore:onRemoveLayerEvent(tag, sender)

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
        if (bit:_and(self.m_myGroupInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) then

            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendSetMemberChange(ClubFrame:getInstance().m_nCurrentClub,GlobalUserItem.tabAccountInfo.dwUserID,GlobalUserItem.tabAccountInfo.dwUserID,delMember,0)
        else
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendDeleteMember(ClubFrame:getInstance().m_nCurrentClub, delMember)
        end

        self.m_removeLayer:removeFromParent()
    end

end



function ClubMemberScore:cellSizeForTable(view, idx)
    if view == self._listMemberView then 
         return 920 , 90
    elseif view == self._listPlayer then 
         return 610.5 , 62
    end
end

function ClubMemberScore:tableCellAtIndex(view, idx)
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


            local csb = ExternalFun.loadCSB("club/node_memberScore.csb", cell)
            csb:setAnchorPoint(cc.p(0,0))
            csb:setPosition(0, 0)

            
            --ID
            local txtID = csb:getChildByName("id")
            if nil ~= txtID then
                txtID:setString(""..tagIMUserInfo.dwGameID)
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

            local memberRight= {[cmd_Club.AUTHORITY_OWNER] = "盟主",
                                                [cmd_Club.AUTHORITY_MANAGER] = "管理员",
                                                [cmd_Club.AUTHORITY_CAPTAIN] = "队长",
                                                [cmd_Club.AUTHORITY_TEAMMEMBER] = "队员"}

            local right = csb:getChildByName("right")
                right:setString(tagIMGroupMember.nMemberRight==0 and "成员" or (memberRight[tagIMGroupMember.nMemberRight] or "成员"))

            local lastScore = csb:getChildByName("score")
                lastScore:setString(tagIMGroupMember.lScore.."")


                local checkbtn = csb:getChildByName("CheckBox_1")
                checkbtn:setTag(TAG_ENUM.BTN_SELECT)
                checkbtn:addTouchEventListener(self.m_itemFunC)
                checkbtn.Memberinfo = Memberinfo
                checkbtn:setSelected(false)

                Memberinfo.index = idx +1

                if self.m_tabSelectPlayer[idx +1] == true then
                    checkbtn:setSelected(true)
                end
                local isCreator = self.m_nCurrentClubInfo.GroupProperty.dwCreaterID ==  tagIMUserInfo.dwUserID  --当前玩家是否为圈主

                local addbtn = csb:getChildByName("btn_add")
                addbtn:setTag(TAG_ENUM.BTN_CHANGEADD)
                addbtn:addTouchEventListener(self.m_itemFunC)
                addbtn.Memberinfo = Memberinfo

                local subbtn = csb:getChildByName("btn_sub")
                subbtn:setTag(TAG_ENUM.BTN_CHANGESUB)
                subbtn:addTouchEventListener(self.m_itemFunC)
                subbtn.Memberinfo = Memberinfo

                -- if not self.m_isCreator then
                --     btn:setPositionX(830)
                -- end


                if self.m_isCaptain and Memberinfo.UserInfo.dwUserID == self.m_myGroupInfo.MemberInfo.dwMemberID then
                    checkbtn:setVisible(false)
                    csb:getChildByName("Sprite_1"):setVisible(true)
                end


                -- local btn = csb:getChildByName("btn_detail")
                -- btn:setTag(TAG_ENUM.BTN_DETAIL)
                -- btn:addTouchEventListener(self.m_itemFunC)
                -- btn.Memberinfo = Memberinfo
                -- btn:setVisible(false)



                -- if (bit:_and(tagIMGroupMember.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0) and self.m_isCreator then
                --     btn:setVisible(true)
                -- end

                -- if (bit:_and(tagIMGroupMember.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0) and (self.m_isCreator or ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT9))then
                --     btn:setVisible(true)
                -- end


                -- if tagIMGroupMember.dwMemberID == GlobalUserItem.tabAccountInfo.dwUserID then
                --     btn:setVisible(false)
                -- end


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
function ClubMemberScore:numberOfCellsInTableView(view)
    if view == self._listPlayer then
        return #self.m_removePlayer
    elseif view == self._listMemberView then
        return #self.m_tabMemberList
    end
end



function ClubMemberScore:onDeleteMember()
    self:onRefreshInfo(nil)

    -- 通知更新    
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

return ClubMemberScore