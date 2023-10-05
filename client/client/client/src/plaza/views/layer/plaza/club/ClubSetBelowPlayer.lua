
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

local ClubSetBelowPlayer = class("ClubSetBelowPlayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "IMAGE_EDIT",           -- 编辑
    "BTN_SERCH",             -- 搜索

}

ClubSetBelowPlayer.LYAER_SETCAPTAIN = 1
ClubSetBelowPlayer.LYAER_SETMEMBER = 2

local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubSetBelowPlayer:ctor( scene, param, level )
    local this = self

    ClubSetBelowPlayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_captainAdd.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._scene = scene
    self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo

    self.curSetType = param.layerType

    self.curCaptain = param.captain or 0


    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local title = spbg:getChildByName("Image_2"):getChildByName("Sprite_3")
    if self.curSetType == ClubSetBelowPlayer.LYAER_SETMEMBER then
        title:setTexture("club/title_setMem.png")
    end

    --成员列表
    local tmp = self.m_spBg:getChildByName("Panel_2")
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
            :setPosition(343,544)
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

    self.serchID = spbg:getChildByName("number")
    self.serchID:setString("输入ID查询")


    self.searchImg = spbg:getChildByName("Image_1")
    self.searchImg:setTag(TAG_ENUM.BTN_SERCH)
    self.searchImg:addTouchEventListener( touchFunC )
    self.searchImg:setTouchEnabled(true)



    self:onRefreshInfo(nil)

    self:onTransitionInFinish()
    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)


end

function ClubSetBelowPlayer:onExit()
    ClubSetBelowPlayer.super.onExit(self)
end

-- 按键监听
function ClubSetBelowPlayer:onButtonClickedEvent(tag,sender)
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

function ClubSetBelowPlayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubSetBelowPlayer:onTransitionInFinish()


end

function ClubSetBelowPlayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubSetBelowPlayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubSetBelowPlayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end


function ClubSetBelowPlayer:onRefreshInfo(id)
    --获取当前选中的大联盟数据
    self.m_tabMemberList = {}
    if self.curCaptain == 0 or self.curSetType == ClubSetBelowPlayer.LYAER_SETMEMBER then 
        for i,v in ipairs(ClubFrame:getInstance().m_tabGroupMember) do
           --先判断大联盟
           if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
                local tabMember = v.memberInfo
                for j=1, #tabMember do
                    if tabMember[j].MemberInfo.nMemberRight == cmd_Club.AUTHORITY_MEMBER then
                        table.insert(self.m_tabMemberList, tabMember[j])
                    end
                end
           end
        end
    else
        for i,v in ipairs(ClubFrame:getInstance().m_tabGroupMember) do
           --先判断大联盟
           if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
                local tabMember = v.memberInfo
                for j=1, #tabMember do
                    if tabMember[j].MemberInfo.dwCaptainID == self.curCaptain and tabMember[j].MemberInfo.nMemberRight == cmd_Club.AUTHORITY_TEAMMEMBER then
                        table.insert(self.m_tabMemberList, tabMember[j])
                    end
                end
           end
        end
    end



    if id and id ~= 0 then
            local temp = {}
            for i,v in ipairs(self.m_tabMemberList) do
                if v and v.UserInfo.dwGameID == id then
                    table.insert(temp,v)
                    break
                end
            end
            self.m_tabMemberList = temp
    end


    if nil ~= id and 0 == #self.m_tabMemberList then
        showToast(self, "没有找到该玩家，请确认玩家ID！", 2)
    end



    --dump(self.m_tabMemberList, "self.m_tabClubInfo", 10)
    self._listMemberView:reloadData()
end

function ClubSetBelowPlayer:cellSizeForTable(view, idx)
    return 692 , 90
end

function ClubSetBelowPlayer:tableCellAtIndex(view, idx)
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
    dump(Memberinfo, "tableCellAtIndex", 10)

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo


    local csb = ExternalFun.loadCSB("club/node_addcap.csb", cell)
    csb:setAnchorPoint(cc.p(0,0))
    csb:setPosition(1, 0)

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
        stutas:setString(tagIMGroupMember.cbStatus==0 and "离线" or "在线")


        local btn = csb:getChildByName("Button_1")
        btn:addTouchEventListener(self.m_itemFunC)
        btn.Memberinfo = Memberinfo
        btn.cbMode = 1

        if self.curSetType == ClubSetBelowPlayer.LYAER_SETMEMBER then
            btn:setTitleText("设为队员")
            btn.cbMode = 2
        end



    return cell

end

-- 子视图数目
function ClubSetBelowPlayer:numberOfCellsInTableView(view)
    return #self.m_tabMemberList
end

function ClubSetBelowPlayer:onItemButtonClickedEvent( tag, ref )
    local Memberinfo = ref.Memberinfo
    if nil == Memberinfo then
        print("成员信息为空")
        return
    end

    local tagIMUserInfo = Memberinfo.UserInfo
    local tagIMGroupMember = Memberinfo.MemberInfo

        if self.curSetType == ClubSetBelowPlayer.LYAER_SETMEMBER then
            ClubFrame:getInstance():setViewFrame(self._scene)
            ClubFrame:getInstance():sendSetMemberChange(ClubFrame:getInstance().m_nCurrentClub,GlobalUserItem.tabAccountInfo.dwUserID,self.curCaptain,{tagIMGroupMember.dwMemberID},1)
        else
            ClubFrame:getInstance():setViewFrame(self._scene)
            ClubFrame:getInstance():sendChangeRight(ClubFrame:getInstance().m_nCurrentClub,tagIMGroupMember.dwMemberID,cmd_Club.AUTHORITY_CAPTAIN,0)
        end
            self:removeFromParent()
end


function ClubSetBelowPlayer:onEditEvent(event, editbox)

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




return ClubSetBelowPlayer