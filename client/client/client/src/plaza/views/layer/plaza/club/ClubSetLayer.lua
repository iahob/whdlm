
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local ClubMemberSet = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMemberSet")
local QueryDialog = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubQueryDialog")

-- 用户中心(二级弹窗)
local ClubSetLayer = class("ClubSetLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_SHARE",            -- 分享
    "BTN_SAVE",             -- 保存
    "BTN_EXIT",             -- 退出大联盟
    "BTN_NAMEEDIT",--编辑大联盟名称
    "BTN_COPY",
    "BTN_FREEZE", --冻结
    "BTN_DISMISS",--解散和退出
    "BTN_INOUT",--进出设置
    "BTN_DISSET",--解散设置
    "BTN_MEMBERSET",--成员管理
    "BTN_POWERSET",--权限设置
    "BTN_XUNZHANG", --勋章开关
    "BTN_ROOMLIMIT",--桌子限制
    "BTN_SCORE",--分数开关
    "BTN_MAXWIN",--大赢家开关

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubSetLayer:ctor( scene, param, level ) --param 传房间信息
    local this = self

    ClubSetLayer.super.ctor( self, scene, param, level )


    self.curGroupInfo = GlobalUserItem.tabCurGroupInfo

    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo 


    local path = "club/nclub_set.csb"
    local isCaptainLayer = false
    if self.curGroupInfo.GroupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID and 
        bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0 then
        path = "club/nclub_setLow.csb"
        isCaptainLayer = true
    end

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB(path, self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg



    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("btn_edit")
    btn:setTag(TAG_ENUM.BTN_NAMEEDIT)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_nameEdit = btn

    local btn = spbg:getChildByName("btn_copy")
    btn:setTag(TAG_ENUM.BTN_COPY)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

     --房间信息
    local clubName = spbg:getChildByName("name")
    clubName:setString(self.curGroupInfo.GroupProperty.szGroupName)

    self.m_nameTxt =  clubName

    --人数
    local clubMember = spbg:getChildByName("playerCount")
    clubMember:setString(self.curGroupInfo.GroupProperty.wMemberCount.."/"..self.curGroupInfo.GroupProperty.wMaxMemberCount)
    --创建人
    local clubHolder = spbg:getChildByName("createrNme")
    clubHolder:setString(ExternalFun.clipText(self.curGroupInfo.GroupProperty.szCreaterNickName,6,true).."("..self.curGroupInfo.GroupProperty.dwCreaterGameID..")")

    --ID
    local clubID = spbg:getChildByName("id")
    clubID:setString(""..self.curGroupInfo.GroupProperty.dwGroupID)


    --大联盟介绍编辑框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    -- 设置介绍
    local tmp = spbg:getChildByName("img_des")
    local editbox = ccui.EditBox:create(cc.size(200, 30),"public/public_blank.png")
            :setPosition(292,519)
            :setFontColor(cc.c3b(255, 211, 154))
            :setAnchorPoint(cc.p(0,0.5))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 25)
            :setFont("fonts/round_body.ttf", 25) 
            :setMaxLength(6)
            :setText(self.curGroupInfo.GroupProperty.szGroupName.."")
    mask:addChild(editbox)         
    self.m_editName = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)  




    if isCaptainLayer then
            self.m_nameEdit:setVisible(false)
            self.m_editName:setVisible(false)
            

            local btn_js = spbg:getChildByName("btn_js")
            btn_js:setTag(TAG_ENUM.BTN_DISMISS)
            btn_js:addTouchEventListener( touchFunC )
            btn_js:setPressedActionEnabled(true)
            btn_js:setTitleText("退出大联盟")

            local btn_set = spbg:getChildByName("btn_memSet")
            btn_set:setTag(TAG_ENUM.BTN_MEMBERSET)
            btn_set:addTouchEventListener( touchFunC )
            btn_set:setPressedActionEnabled(true)

            --if bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) == 0 then
                btn_set:setVisible(false)
                btn_js:setPositionX(316)
                
                spbg:getChildByName("Sprite_4_0"):getChildByName("Text_1"):setString("设置")
            --end
    else
            self.tabSetBtn =  {}
            local btn = spbg:getChildByName("btn_dj")
            btn:setTag(TAG_ENUM.BTN_FREEZE)
            btn:addTouchEventListener( touchFunC )
            btn:setPressedActionEnabled(true)
            table.insert(self.tabSetBtn,btn)

                local btn = spbg:getChildByName("btn_js")
            btn:setTag(TAG_ENUM.BTN_DISMISS)
            btn:addTouchEventListener( touchFunC )
            btn:setPressedActionEnabled(true)
            table.insert(self.tabSetBtn,btn)


                local btn = spbg:getChildByName("btn_jc")
            btn:setTag(TAG_ENUM.BTN_INOUT)
            btn:addTouchEventListener( touchFunC )
            btn:setPressedActionEnabled(true)
            table.insert(self.tabSetBtn,btn)


                local btn = spbg:getChildByName("btn_memSet")
            btn:setTag(TAG_ENUM.BTN_MEMBERSET)
            btn:addTouchEventListener( touchFunC )
            btn:setPressedActionEnabled(true)
            table.insert(self.tabSetBtn,btn)

                local btn = spbg:getChildByName("btn_qxSet")
            btn:setTag(TAG_ENUM.BTN_POWERSET)
            btn:addTouchEventListener( touchFunC )
            btn:setPressedActionEnabled(true)
            table.insert(self.tabSetBtn,btn)

                local btn = spbg:getChildByName("btn_xzSet")
            btn:setTag(TAG_ENUM.BTN_XUNZHANG)
            btn:addTouchEventListener( touchFunC )
            btn:setPressedActionEnabled(true)
            table.insert(self.tabSetBtn,btn)


            local btn = spbg:getChildByName("btn_maxWin")
            btn:setTag(TAG_ENUM.BTN_MAXWIN)
            btn:addTouchEventListener( touchFunC )
            btn:setPressedActionEnabled(true)
            table.insert(self.tabSetBtn,btn)

            local btn = spbg:getChildByName("btn_roomLimit")
            btn:setTag(TAG_ENUM.BTN_ROOMLIMIT)
            btn:addTouchEventListener( touchFunC )
            btn:setPressedActionEnabled(true)
            table.insert(self.tabSetBtn,btn)


            if self.curGroupInfo.GroupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID then
                self.m_nameEdit:setVisible(false)
                self.tabSetBtn[2]:setTitleText("退出大联盟")
                for k,v in pairs(self.tabSetBtn) do
                    if v and k~= 2 then
                        v:setEnabled(false)
                    end
                end
            end
           

            if ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT1) then
                self.tabSetBtn[1]:setEnabled(true)
            end

            self:updateFreezeBtn()

            if ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT2) then
                self.tabSetBtn[3]:setEnabled(true)
            end

            if ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT5) or self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN then
                self.tabSetBtn[4]:setEnabled(true)
            end

            if ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT6) then
                self.tabSetBtn[6]:setEnabled(true)
            end

            if ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT14) then
                self.tabSetBtn[7]:setEnabled(true)
            end

            local index = 1
            for i=1,8 do
                if self.tabSetBtn[i]:isVisible() then
                    self.tabSetBtn[i]:setPosition(215+243*((index-1)%4), 273-math.floor((index-1)/4)*86)
                    index = index+1
                end
            end


    end





    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
end

function ClubSetLayer:onExit()

    ClubSetLayer.super.onExit(self)
end


function ClubSetLayer:updateFreezeBtn()
            if self.curGroupInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID
                or ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT1) then
                if self.curGroupInfo.GroupProperty.cbGroupStatus == 1 then
                    self.tabSetBtn[1]:setTitleText("冻结大联盟")
                elseif self.curGroupInfo.GroupProperty.cbGroupStatus == 2 then
                    self.tabSetBtn[1]:setTitleText("解除冻结")
                end
            end
end

-- 按键监听
function ClubSetLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        if self.m_bBeginEdit == true then
            self.m_bBeginEdit = false
            return 
        end
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_NAMEEDIT then
        self.m_editName:touchDownAction(self.m_editName, ccui.TouchEventType.ended)
        self.m_nameEdit:setEnabled(false)
    elseif tag == TAG_ENUM.BTN_COPY then

        MultiPlatform:getInstance():copyToClipboard(self.curGroupInfo.GroupProperty.dwGroupID .. "")

    elseif TAG_ENUM.BTN_DISMISS == tag then --解散
        if self.curGroupInfo.GroupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID then 
            if self._scene then
                self._scene:dismissPopWait()
                self.m_Query = QueryDialog:create(self.curGroupInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID and "是否确认解散大联盟?" or "是否确认退出大联盟?", function(ok)
                    if ok then
                        ClubFrame:getInstance():sendApplyRequest(ClubFrame:getInstance().m_nCurrentClub, 1)
                        self:removeFromParent()
                    end
                    self.m_Query = nil
                end, 26, QueryDialog.QUERY_SURE_CANCEL)
                :setCanTouchOutside(false)
                :addTo(self._scene)
            end
        else
            self:showDissLayer()
        end

    elseif TAG_ENUM.BTN_SHARE == tag then--分享
        self._scene:popTargetShare(function(target, bF2f)
            -- 面对面
            if nil ~= target then
                local function sharecall( isok )
                    if type(isok) == "string" and isok == "true" then
                        showToast(self, "分享成功", 2)
                    end
                end
                local url = GlobalUserItem.szSpreaderURL or yl.HTTP_URL
                MultiPlatform:getInstance():shareToTarget(
                    target, 
                    sharecall, 
                    string.format("【大联盟ID: %06d】", self.groupProperty.dwGroupID), 
                    "我在大联盟游戏，等你来参加！", 
                    url, 
                    "")
                print("分享"..string.format("【大联盟ID: %06d】", self.groupProperty.dwGroupID))
            end
        end, 3)

  
    elseif TAG_ENUM.BTN_FREEZE == tag then
        self:showFreezeLayer()

    elseif TAG_ENUM.BTN_INOUT == tag then
        self:showInOutLayer()
    elseif TAG_ENUM.BTN_DISSET == tag then

    elseif TAG_ENUM.BTN_MEMBERSET == tag then
        local pa = ClubMemberSet:create(self._scene, {}, ClubMemberSet.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BTN_POWERSET == tag then
        self:showRightLayer(self)
    elseif TAG_ENUM.BTN_XUNZHANG == tag then
        self:showXunZhangLayer()
    elseif TAG_ENUM.BTN_ROOMLIMIT == tag then
        self:showFreeTableLimitLayer()
    elseif TAG_ENUM.BTN_SCORE == tag then

    elseif TAG_ENUM.BTN_MAXWIN == tag then
        self:showMaxWinLayer()
    end
end

function ClubSetLayer:onSelectedEvent(tag, sender)
    if self.m_nSelectBox == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelectBox = tag
    for k,v in pairs(self.m_tabCheckBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    self:onRefreshInfo()
end

function ClubSetLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubSetLayer:onTransitionInFinish()
    self:onRefreshInfo()
end

function ClubSetLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubSetLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubSetLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

function ClubSetLayer:onRefreshInfo()
    --获取当前选中的大联盟数据

end

--编辑框回调
function ClubSetLayer:onEditEvent(event, editbox)
    local src = editbox:getText() 
        if event == "return" then
            self.m_nameEdit:setEnabled(true)
            self.m_nameTxt:setString(src)

            self.m_nameTxt:setVisible(true)
            if self.m_nameTxt:getContentSize().width == 0 then --没有输入
                self.m_nameTxt:setString(self.curGroupInfo.GroupProperty.szGroupName)
                editbox:setText(self.curGroupInfo.GroupProperty.szGroupName)
                showToast(self, "大联盟名字不能为空，修改失败!", 3)
                return
            end

        if true == ExternalFun.isContainBadWords(src) then
            showToast(self, "大联盟名字不能含有敏感字，修改失败!", 3)
            editbox:setText(self.curGroupInfo.GroupProperty.szGroupName)
            self.m_nameTxt:setString(self.curGroupInfo.GroupProperty.szGroupName)
            return
        end 

        if ExternalFun.getWordsNum(src) <=1 then
            showToast(self, "大联盟名字太短，修改失败!", 3)
            editbox:setText(self.curGroupInfo.GroupProperty.szGroupName)
            self.m_nameTxt:setString(self.curGroupInfo.GroupProperty.szGroupName)
            return
        end 

        if ExternalFun.isContainSpecial(src) or src ~= self.m_nameTxt:getString() then
            showToast(self, "大联盟名字不能含有字符，修改失败!", 3)
            editbox:setText(self.curGroupInfo.GroupProperty.szGroupName)
            self.m_nameTxt:setString(self.curGroupInfo.GroupProperty.szGroupName)
            return
        end 

        if self.m_nameTxt:getString() ~=  self.curGroupInfo.GroupProperty.szGroupName then
            ClubFrame:getInstance():sendModifyGroupName(ClubFrame:getInstance().m_nCurrentClub,src)
        end

            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3),cc.CallFunc:create(function ( ... )
                self.m_bBeginEdit = false
            end)))

        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= "" then
                self.m_nameTxt:setString(src)
            else
                self.m_nameTxt:setString(self.curGroupInfo.GroupProperty.szGroupName)
            end

        elseif event == "began" then
            self.m_bBeginEdit = true
            --editbox:setText(self.m_nameTxt:getString())
            -- if string.len(src) ~= 0 and src ~= "" then
            --     self.m_nameTxt:setString(src)
            -- else
            --     self.m_nameTxt:setString(self.curGroupInfo.GroupProperty.szGroupName)
            -- end
            self.m_nameTxt:setVisible(false)
        end
end

function ClubSetLayer:onSubUpdateGroup()
    showToast(self, "大联盟信息已修改！",2)
end


function ClubSetLayer:showFreezeLayer()
        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_djie.csb", self)


        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                    rootLayer:removeFromParent()
            end
        end
        local btn = csbNode:getChildByName("Panel_1")
        btn:addTouchEventListener(touchFunC)

        local bg = csbNode:getChildByName("bg")

        local btn = bg:getChildByName("btn_close")
        btn:addTouchEventListener(touchFunC)

        local dongjie= bg:getChildByName("Text_2")
        local jiedong= bg:getChildByName("Text_2_0")


        local btn = bg:getChildByName("btn_ok")
        btn:addTouchEventListener(function (ref, tType)
            if tType == ccui.TouchEventType.ended then
                if self.curGroupInfo.GroupProperty.cbGroupStatus == 1 then
                    ClubFrame:getInstance():setViewFrame(self)
                    ClubFrame:getInstance():sendClubStatus(ClubFrame:getInstance().m_nCurrentClub,2)
                    rootLayer:removeFromParent()
                elseif self.curGroupInfo.GroupProperty.cbGroupStatus == 2 then
                    ClubFrame:getInstance():setViewFrame(self)
                    ClubFrame:getInstance():sendClubStatus(ClubFrame:getInstance().m_nCurrentClub,1)
                    rootLayer:removeFromParent()
                end
            end
        end)

        if self.curGroupInfo.GroupProperty.cbGroupStatus == 1 then
            dongjie:setVisible(true)
            jiedong:setVisible(false)
            btn:setTitleText("确认冻结")
        elseif self.curGroupInfo.GroupProperty.cbGroupStatus == 2 then
            dongjie:setVisible(false)
            jiedong:setVisible(true)
            btn:setTitleText("解除冻结")
        end

end

--进出设置
function ClubSetLayer:showDissLayer()
        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_js.csb", self)


        local checkTab = {}

        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then 
                    rootLayer:removeFromParent()
            end
        end

        local btn = csbNode:getChildByName("Panel_1")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(100)

        local bg = csbNode:getChildByName("bg")

        local btn = bg:getChildByName("btn_close")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(100)

        local btn = bg:getChildByName("btn_ok_0")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(100)

        local btn = bg:getChildByName("btn_ok")
        btn:addTouchEventListener(function (ref, tType)
            if tType == ccui.TouchEventType.ended then

             if self._scene then
                self.m_Query = QueryDialog:create("解散后不可撤销，不可恢复！", function(ok)
                    if ok then
                        ClubFrame:getInstance():sendDeleteGroup(ClubFrame:getInstance().m_nCurrentClub, 0)
                        rootLayer:removeFromParent()
                        self:removeFromParent()
                    end
                    self.m_Query = nil
                end, 26, QueryDialog.QUERY_SURE_CANCEL)
                :setCanTouchOutside(false)
                :addTo(self._scene)
            end


            end
        end)

        local txtFront = bg:getChildByName("Text_3")
       local groupName = txtFront:getChildByName("Text_4")
       groupName:setString(GlobalUserItem.tabCurGroupInfo.GroupProperty.szGroupName.."")
       txtFront:getChildByName("Text_5"):setPositionX(groupName:getContentSize().width + txtFront:getContentSize().width +3)
       txtFront:setPositionX((bg:getContentSize().width - groupName:getContentSize().width - txtFront:getContentSize().width - txtFront:getChildByName("Text_5"):getContentSize().width)/2)

end


--进出设置
function ClubSetLayer:showInOutLayer()
        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_jc.csb", self)

        local intag = self.curGroupInfo.GroupBattleRule.cbInMode
        local outtag = self.curGroupInfo.GroupBattleRule.cbOutMode
        local intype = self.curGroupInfo.GroupBattleRule.cbGroupIDShow

        local checkTab = {}

        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                    local touchTag = ref:getTag()
                    if touchTag == 100 then
                            rootLayer:removeFromParent()
                    else
                            ref:setSelected(true)
                            if touchTag == 1 then
                                checkTab[2]:setSelected(false)
                                intag = 1
                            elseif touchTag == 2 then
                                checkTab[1]:setSelected(false)
                                intag = 0
                            elseif touchTag == 3 then
                                checkTab[4]:setSelected(false)
                                intype = 0
                            elseif touchTag == 4 then
                                checkTab[3]:setSelected(false)
                                intype = 1
                            elseif touchTag == 5 then
                                checkTab[6]:setSelected(false)
                                outtag = 1
                            elseif touchTag == 6 then
                                checkTab[5]:setSelected(false)
                                outtag = 0
                            end
                    end

            end
        end

        local btn = csbNode:getChildByName("Panel_1")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(100)

        local bg = csbNode:getChildByName("bg")

        local btn = bg:getChildByName("btn_close")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(100)


        local btn = bg:getChildByName("btn_ok")
        btn:addTouchEventListener(function (ref, tType)
            if tType == ccui.TouchEventType.ended then
                    ClubFrame:getInstance():sendInOutSet(ClubFrame:getInstance().m_nCurrentClub,intag,intype,outtag)
                    rootLayer:removeFromParent()
            end
        end)


        for i=1,6 do
            local checkbx = bg:getChildByName("CheckBox_"..i)
            checkbx:setTag(i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(false)
            checkbx:addTouchEventListener(touchFunC)
            checkTab[i] = checkbx
        end

        if self.curGroupInfo.GroupBattleRule.cbInMode == 0  then
            checkTab[2]:setSelected(true)
        else
            checkTab[1]:setSelected(true)
        end

        if self.curGroupInfo.GroupBattleRule.cbGroupIDShow == 0  then
            checkTab[3]:setSelected(true)
        else
            checkTab[4]:setSelected(true)
        end

        if self.curGroupInfo.GroupBattleRule.cbOutMode == 0  then
            checkTab[6]:setSelected(true)
        else
            checkTab[5]:setSelected(true)
        end


end

--勋章设置
local BTN_XZ1 = 50
local BTN_XZ2 = 51
local BTN_STATUS1 = 52
local BTN_STATUS2 = 53
local BTN_CURXZ1 = 54
local BTN_CURXZ2 = 55
local BTN_USEXZ1 = 56
local BTN_USEXZ2 = 57
local BTN_CLEANXZ1 = 58
local BTN_CLEANXZ2 = 59
local BTN_OK = 70
local BTN_CLOSE = 71


function ClubSetLayer:showXunZhangLayer()
        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_xk.csb", self)
        self.m_xunzhangLayer = rootLayer
        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then

                    self:onXZbtnEvent(ref:getTag(),ref)
            end
        end

        local btn = csbNode:getChildByName("Panel_1")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_CLOSE)

        local bg = csbNode:getChildByName("bg")
        self.m_xzBg1 = bg

        local btn = bg:getChildByName("btn_close")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_CLOSE)


        local btn = bg:getChildByName("btn_ok")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_OK)

        self.m_xzStatus = {self.curGroupInfo.GroupBattleRule.cbMedalMode,
                                        self.curGroupInfo.GroupBattleRule.cbMedalShow,
                                        self.curGroupInfo.GroupBattleRule.cbCurMedalShow,
                                        self.curGroupInfo.GroupBattleRule.cbMedalRevenueShow,
                                        self.curGroupInfo.GroupBattleRule.cbMedalClear}

        self.m_xzStatusBtn={}
        for i=1,10 do
            local checkbx = bg:getChildByName("CheckBox_"..i)
            checkbx:setTag(BTN_XZ1+i-1)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(false)
            checkbx:addTouchEventListener(touchFunC)
            self.m_xzStatusBtn[i] = checkbx
        end
        self.m_xzStatusBtn[1]:setTouchEnabled(false)
        self.m_xzStatusBtn[1]:setSelected(true)
        self.m_xzStatusBtn[self.curGroupInfo.GroupBattleRule.cbMedalShow ==1 and 3 or 4]:setSelected(true)
        self.m_xzStatusBtn[self.curGroupInfo.GroupBattleRule.cbCurMedalShow ==1 and 5 or 6]:setSelected(true)
        self.m_xzStatusBtn[self.curGroupInfo.GroupBattleRule.cbMedalRevenueShow ==1 and 7 or 8]:setSelected(true)
        self.m_xzStatusBtn[self.curGroupInfo.GroupBattleRule.cbMedalClear ==0 and 9 or 10]:setSelected(true)

        local bg2 = csbNode:getChildByName("bg_s")
        self.m_xzBg2 = bg2


        local btn = bg2:getChildByName("btn_close")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_CLOSE)

        local btn = bg2:getChildByName("btn_ok")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_OK)

        for i=1,2 do
            local checkbx = bg2:getChildByName("CheckBox_"..i)
            checkbx:setTag(BTN_XZ1+i-1)
            checkbx:setTouchEnabled(false)
            checkbx:setSelected(true)
            checkbx:addTouchEventListener(touchFunC)
            if i==1 then
                    checkbx:setTouchEnabled(true)
                    checkbx:setSelected(false)
            end
        end

        if self.curGroupInfo.GroupBattleRule.cbMedalMode == 0 then
            self.m_xzBg1:setVisible(false)
            self.m_xzBg2:setVisible(true)
        else
            self.m_xzBg1:setVisible(true)
            self.m_xzBg2:setVisible(false)
        end

end

function ClubSetLayer:onXZbtnEvent(tag,sender)
        if tag == BTN_CLOSE then
            self.m_xunzhangLayer:removeFromParent()
        elseif tag == BTN_OK then
            ClubFrame:getInstance():sendXunzhangSet(ClubFrame:getInstance().m_nCurrentClub,self.m_xzStatus)
            self.m_xunzhangLayer:removeFromParent()

        elseif tag == BTN_XZ1 then
            self.m_xzBg1:setVisible(true)
            self.m_xzBg2:setVisible(false)
            sender:setSelected(false)
            self.m_xzStatus[1] = 1
        elseif tag == BTN_XZ2 then
            self.m_xzBg1:setVisible(false)
            self.m_xzBg2:setVisible(true)
            sender:setSelected(false)
            self.m_xzStatus[1] = 0
        elseif tag == BTN_STATUS1 then
            self.m_xzStatusBtn[3]:setSelected(true)
            self.m_xzStatusBtn[4]:setSelected(false)
            self.m_xzStatus[2] = 1
        elseif tag == BTN_STATUS2 then
            self.m_xzStatusBtn[3]:setSelected(false)
            self.m_xzStatusBtn[4]:setSelected(true)
            self.m_xzStatus[2] = 0
        elseif tag == BTN_CURXZ1 then
            self.m_xzStatusBtn[5]:setSelected(true)
            self.m_xzStatusBtn[6]:setSelected(false)
            self.m_xzStatus[3] = 1
        elseif tag == BTN_CURXZ2 then
            self.m_xzStatusBtn[5]:setSelected(false)
            self.m_xzStatusBtn[6]:setSelected(true)
            self.m_xzStatus[3] = 0
        elseif tag == BTN_USEXZ1 then
            self.m_xzStatusBtn[7]:setSelected(true)
            self.m_xzStatusBtn[8]:setSelected(false)
            self.m_xzStatus[4] = 1
        elseif tag == BTN_USEXZ2 then
            self.m_xzStatusBtn[7]:setSelected(false)
            self.m_xzStatusBtn[8]:setSelected(true)
            self.m_xzStatus[4] = 0
        elseif tag == BTN_CLEANXZ1 then
            self.m_xzStatusBtn[9]:setSelected(true)
            self.m_xzStatusBtn[10]:setSelected(false)
            self.m_xzStatus[5] = 0
        elseif tag == BTN_CLEANXZ2 then
            self.m_xzStatusBtn[9]:setSelected(false)
            self.m_xzStatusBtn[10]:setSelected(true)
            self.m_xzStatus[5] = 1

        end

end


local BTN_MNG = 100
local BTN_CAP = 101
--权限设置
function ClubSetLayer:showRightLayer(scene,dwTargetID)
        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_right.csb", scene)
        self.m_RightLayer = rootLayer
        self.m_MrightSet = {}
        self.m_CrightSet = {}

        local bg = csbNode:getChildByName("bg")

        local mgPanel = bg:getChildByName("Panel_3")
        mgPanel:setVisible(true)

        local cpPanel = bg:getChildByName("Panel_4")
        cpPanel:setVisible(false)

        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                    if ref:getTag() == BTN_CLOSE  then 
                        rootLayer:removeFromParent()
                    elseif ref:getTag() == BTN_OK  then 

                        local cRight = {[3] = self.m_CrightSet[2],[4] = self.m_CrightSet[3],[5] = self.m_CrightSet[1],[8] = self.m_CrightSet[4]}
                        ClubFrame:getInstance():sendRightSet(ClubFrame:getInstance().m_nCurrentClub,self.m_MrightSet,cRight,dwTargetID)
                        rootLayer:removeFromParent()
                    elseif ref:getTag() < 50 then
                        self.m_MrightSet[ref:getTag()] = (self.m_MrightSet[ref:getTag()]+1)%2
                        ref:setSelected(self.m_MrightSet[ref:getTag()]==1 and true or false)
                    elseif ref:getTag() > 1000 then
                        self.m_CrightSet[ref:getTag() - 1000] = (self.m_CrightSet[ref:getTag() - 1000]+1)%2
                        ref:setSelected(self.m_CrightSet[ref:getTag() - 1000]==1 and true or false)
                    elseif ref:getTag() == BTN_MNG then
                        mgPanel:setVisible(true)
                        cpPanel:setVisible(false)
                        self.m_btnMng:getChildByName("Sprite_13"):setVisible(true)
                        self.m_btnCap:getChildByName("Sprite_13"):setVisible(false)
                    elseif ref:getTag() == BTN_CAP then    
                        mgPanel:setVisible(false)
                        cpPanel:setVisible(true)
                        self.m_btnMng:getChildByName("Sprite_13"):setVisible(false)
                        self.m_btnCap:getChildByName("Sprite_13"):setVisible(true)

                    end
            end
        end

        local btn = csbNode:getChildByName("Panel_1")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_CLOSE)



        local btn = bg:getChildByName("btn_close")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_CLOSE)


        local btn = bg:getChildByName("btn_ok")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_OK)

        local btn = bg:getChildByName("Button_gly")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_MNG)
        if dwTargetID then
            btn:setVisible(false)
        end
        self.m_btnMng = btn

        local btn = bg:getChildByName("Button_dz")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(BTN_CAP)
        if dwTargetID then
            btn:setVisible(false)
        end
        self.m_btnCap = btn

        self.m_managerRight =clone(ClubFrame:getInstance().m_cbManagerRight)
        self.m_captainRight =clone(ClubFrame:getInstance().m_cbCaptainRight)

        if dwTargetID then
             for k,v in pairs(ClubFrame:getInstance().m_tabGroupMember) do
                    if v and v.groupID == ClubFrame:getInstance().m_nCurrentClub then

                        for n,m in pairs(v.memberInfo) do
                                if m and m.UserInfo.dwUserID == dwTargetID then
                                        if m.MemberInfo.cbManagerRightFlag == 1 then
                                            self.m_managerRight = m.MemberInfo.lMemberManagerRight
                                        end
                                end
                         end

                    end
             end
        end

        local managerRight = {cmd_Club.MANAGER_RIGHT1,cmd_Club.MANAGER_RIGHT2,cmd_Club.MANAGER_RIGHT3,cmd_Club.MANAGER_RIGHT4,cmd_Club.MANAGER_RIGHT5,
    cmd_Club.MANAGER_RIGHT6,cmd_Club.MANAGER_RIGHT7,cmd_Club.MANAGER_RIGHT8,cmd_Club.MANAGER_RIGHT9,cmd_Club.MANAGER_RIGHT10,
    cmd_Club.MANAGER_RIGHT11,cmd_Club.MANAGER_RIGHT12,cmd_Club.MANAGER_RIGHT13,cmd_Club.MANAGER_RIGHT14}
        for i=1,14 do
            local checkbx = bg:getChildByName("Panel_3"):getChildByName("CheckBox_"..i)
            checkbx:setTag(i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(bit:_and(self.m_managerRight, managerRight[i]) ~= 0 and true or false)
            checkbx:addTouchEventListener(touchFunC)
            self.m_MrightSet[i] = bit:_and(self.m_managerRight, managerRight[i]) ~= 0 and 1 or 0
        end


        local captainRight = {cmd_Club.CAPTAIN_RIGHT5,cmd_Club.CAPTAIN_RIGHT3,cmd_Club.CAPTAIN_RIGHT4,cmd_Club.CAPTAIN_RIGHT8}
        for i=1,4 do
            local checkbx = bg:getChildByName("Panel_4"):getChildByName("CheckBox_"..i)
            checkbx:setTag(1000+i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(bit:_and(self.m_captainRight, captainRight[i]) ~= 0 and true or false)
            checkbx:addTouchEventListener(touchFunC)
            self.m_CrightSet[i] = bit:_and(self.m_captainRight, captainRight[i]) ~= 0 and 1 or 0
        end



end

--大赢家设置

function ClubSetLayer:showMaxWinLayer()
        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_dyj.csb", self)

        local wintag = self.curGroupInfo.GroupBattleRule.cbBigWinnerShow

        local checkTab = {}

        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                    local touchTag = ref:getTag()
                    if touchTag == 100 then
                            rootLayer:removeFromParent()

                    else
                            ref:setSelected(true)
                            if touchTag == 1 then
                                checkTab[2]:setSelected(false)
                                wintag = 1
                            elseif touchTag == 2 then
                                checkTab[1]:setSelected(false)
                                wintag = 0
                            end
                    end

            end
        end

        local btn = csbNode:getChildByName("Panel_1")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(100)

        local bg = csbNode:getChildByName("bg")

        local btn = bg:getChildByName("btn_close")
        btn:addTouchEventListener(touchFunC)
        btn:setTag(100)


        local btn = bg:getChildByName("btn_ok")
        btn:addTouchEventListener(function (ref, tType)
            if tType == ccui.TouchEventType.ended then
                    ClubFrame:getInstance():sendMaxWinSet(ClubFrame:getInstance().m_nCurrentClub,wintag)
                    rootLayer:removeFromParent()
            end
        end)


        for i=1,2 do
            local checkbx = bg:getChildByName("CheckBox_"..i)
            checkbx:setTag(i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(false)
            checkbx:addTouchEventListener(touchFunC)
            checkTab[i] = checkbx
        end
        if wintag == 0 then
            checkTab[2]:setSelected(true)
        else
            checkTab[1]:setSelected(true)
        end



end


--空桌限制
local BTN_ADD = 100
local BTN_SUB = 101
local BTN_IMG = 102
local BTN_CLOSE1 = 103
local BTN_OK1 = 104


function ClubSetLayer:showFreeTableLimitLayer()

        local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_roomlimit.csb", self)

        self.m_limitLayer = rootLayer

        local touchFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                self:onClicktEvent(ref:getTag(),ref)
            end
        end
        local panel = csbNode:getChildByName("Panel_1")
        panel:setTag(BTN_CLOSE1)
        panel:addTouchEventListener(touchFunC)

        local bg = csbNode:getChildByName("bg")

        local btn = bg:getChildByName("btn_close")
        btn:setTag(BTN_CLOSE1)
        btn:addTouchEventListener(touchFunC)


        local btn = bg:getChildByName("btn_ok")
        btn:setTag(BTN_OK1)
        btn:addTouchEventListener(touchFunC)

        local btn = bg:getChildByName("Button_sub")
        btn:setTag(BTN_SUB)
        btn:addTouchEventListener(touchFunC)

        local btn = bg:getChildByName("Button_add")
        btn:setTag(BTN_ADD)
        btn:addTouchEventListener(touchFunC)


    local editHanlder = function(event,editbox)
        self:onLimitEditEvent(event, editbox)
    end

    local editbox = ccui.EditBox:create(cc.size(150, 50),"public/public_blank.png")
            :setPosition(cc.p(-10,150))
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 1)
            :setFont("fonts/round_body.ttf", 1) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(2)
    panel:addChild(editbox)         
    self.m_editBox1 = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)


    self.m_changeScore = self.curGroupInfo.GroupBattleRule.cbFreeTableLimit

    self.m_editText1 = bg:getChildByName("number")
    self.m_editText1:setString(self.m_changeScore.."")



    self.m_editImg1 = bg:getChildByName("Image_1")
    self.m_editImg1:setTag(BTN_IMG)
    self.m_editImg1:addTouchEventListener( touchFunC )
    self.m_editImg1:setTouchEnabled(false)


end


function ClubSetLayer:onClicktEvent(tag, sender)
    if tag == BTN_ADD  then
        self.m_changeScore = self.m_changeScore + 1
        self.m_editText1:setString(self.m_changeScore.."")
    elseif tag == BTN_SUB  then
        if self.m_changeScore <2 then
            return 
        end

        self.m_changeScore = self.m_changeScore - 1
        self.m_editText1:setString(self.m_changeScore.."")

    elseif tag == BTN_IMG  then
        self.m_editBox1:touchDownAction(self.m_editBox1, ccui.TouchEventType.ended)
        self.m_editImg1:setEnabled(false)
    elseif tag == BTN_CLOSE1  then
        self.m_limitLayer:removeFromParent()
    elseif tag == BTN_OK1  then
            ClubFrame:getInstance():sendFreeTableSet(ClubFrame:getInstance().m_nCurrentClub,self.m_changeScore)
            self.m_limitLayer:removeFromParent()
    end

end

function ClubSetLayer:onLimitEditEvent(event, editbox)
    local src = editbox:getText() 
        if event == "return" then
            self.m_editImg1:setEnabled(true)
            self.m_editText1:setString(src)
            if self.m_editText1:getContentSize().width == 0 then --没有输入
                self.m_editText1:setString(self.m_changeScore.."")
            end
            if not tonumber(src)then 
               
                showToast(self, "请输入正确的数量！", 2)
            end

        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= "0" then
                self.m_editText1:setString(src)
            else
                self.m_editText1:setString(self.m_changeScore.."")
            end

        elseif event == "began" then
            --self.m_bBeginEdit = true
            self.m_editBox1:setText(self.m_editText1:getString())
            -- if string.len(src) ~= 0 and src ~= "0" then
            --     self.m_editText1:setString(src)
            -- else
            --     self.m_editText1:setString("0")
            -- end
        end
end


return ClubSetLayer