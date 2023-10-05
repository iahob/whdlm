
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

-- 用户中心(二级弹窗)
local ClubCreateLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubCreateLayer")
local ClubRoomLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubRoomLayer")
local ClubJionLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubJionLayer")
local SpineObj = appdf.req("client.src.external.SpineObj")


local ClubMain = class("ClubMain", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_MSG",              -- 消息
    "BTN_HELP",             -- 帮助
    "BTN_CREATE",           -- 创建
    "IMAGE_EDIT",           -- 编辑
    "BTN_JOIN",             -- 加入大联盟

    "BTN_FIRST",            -- 创建的大联盟
    "BTN_ENTER",             -- 加入的大联盟
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubMain:ctor( scene, param, level )
    local this = self

    ClubMain.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/club_MainLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    self.m_tabClubInfo = {}
    self.m_newIndex = 0

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_back")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --创建大联盟按钮
    local btn = spbg:getChildByName("btn_create")
    btn:setTag(TAG_ENUM.BTN_CREATE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --加入大联盟按钮
    local btn = spbg:getChildByName("btn_jion")
    btn:setTag(TAG_ENUM.BTN_JOIN)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    --提示语
    self.m_labTips = spbg:getChildByName("txt_tips")
    self.m_labMyClubNum = spbg:getChildByName("txt_myClubNum")

    self.m_rightIcon = csbNode:getChildByName("Sprite_2")
    self.m_rightIcon:setVisible(false)
    self.m_leftIcon = csbNode:getChildByName("Sprite_2_0")
    self.m_leftIcon:setVisible(false)


    self.m_clubSort = {}
    local clubNum = cc.UserDefault:getInstance():getIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID.."clubNum",0)
    for i=1,clubNum do
        local str = cc.UserDefault:getInstance():getStringForKey(GlobalUserItem.tabAccountInfo.dwUserID.."clubSort"..i,"0")
        if str ~= "0" then 
            local topFlag = tonumber(string.sub(str,1,1))
            local groupId = tonumber(string.sub(str,3,-1))

            table.insert(self.m_clubSort,{group = groupId,top = topFlag})
        end
    end

    self.m_lastClub = math.floor(cc.UserDefault:getInstance():getDoubleForKey(GlobalUserItem.tabAccountInfo.dwUserID.."lastClub",0))
    local lastNeedFirst = cc.UserDefault:getInstance():getBoolForKey(GlobalUserItem.tabAccountInfo.dwUserID.."lastFlag",false)
    dump({self.m_lastClub,lastNeedFirst},"============")
    
    if self.m_lastClub ~= 0 and lastNeedFirst then
        local changeIndex = 0
        local lastIndex = 0
        for i = 1,#self.m_clubSort do
             if self.m_clubSort[i].top == 0 and changeIndex == 0 then
                changeIndex = i
             end

             if self.m_lastClub == self.m_clubSort[i].group and lastIndex == 0 then
                lastIndex = i
             end
        end

        --如果上次进入的是置顶的，放到第一位
        if lastIndex ~= 0 and self.m_clubSort[lastIndex].top ==1 then
            changeIndex =1
        end 

        if changeIndex ~= 0 and lastIndex ~= 0 and changeIndex ~= lastIndex then

            local temp = self.m_clubSort[lastIndex]
            table.remove(self.m_clubSort,lastIndex)

            if changeIndex ~= 1 then 
                for i = 1,#self.m_clubSort do
                     if self.m_clubSort[i].top == 0 and changeIndex == 0 then
                        changeIndex = i
                     end
                end
            end

            table.insert(self.m_clubSort,changeIndex,temp)
        end

    end

        --大联盟列表
    local tmp = self.m_spBg:getChildByName("content")
    self._listClubView = cc.TableView:create(tmp:getContentSize())
    self._listClubView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)    
    self._listClubView:setPosition(tmp:getPosition())
    self._listClubView:setDelegate()
    self._listClubView:addTo(self.m_spBg)
    self._listClubView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listClubView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    self._listClubView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listClubView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tmp:removeFromParent()



    self.sp_txtMyClub = self.m_spBg:getChildByName("tipTxt1")
    self.sp_txtMyClub:setString("已加入大联盟：")

    -- 按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listClubView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listClubView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listClubView:getViewSize().width, self._listClubView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                this:onButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end



    -- 注册事件监听
    --self:registerEventListenr()
    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onClubInfoChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)



     -- local obj = SpineObj:create("club/animate/clubTitle.json", "club/animate/clubTitle.atlas", true)
     --     obj:setPosition(667,675)
     --     --obj:setLocalZOrder(-1)
     --     spbg:addChild(obj)   


    self:noActionTransitionIn(spbg)


    --发送登录大联盟，连接网络
    --ClubFrame:getInstance():setViewFrame(self)
    --ClubFrame:getInstance():reSetAndLogin()
    if not ClubFrame:getInstance():isSocketServer() then
        ClubFrame:getInstance():reSetAndLogin()
    end
end

function ClubMain:onExit()
    if nil ~= self._modifyFrame then
        self._modifyFrame:onCloseSocket()
        self._modifyFrame = nil
    end 
    ClubMain.super.onExit(self)
end

-- 按键监听
function ClubMain:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then


        --退出关系连接
        --ClubFrame:getInstance():reSetAndDisconnect()
        ClubFrame:getInstance().m_curQueryClub = 0
        ClubFrame:getInstance().m_nCurrentClub = 0
        self._scene:onChangeShowMode(yl.SCENE_PLAZA)
        --self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_CREATE == tag then --创建
        local pa = ClubCreateLayer:create(self._scene, {}, ClubMain.SECOND_LEVEL +1)
        self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BTN_JOIN == tag then--加入
        ----------------------test ---------------------
        --ClubFrame:getInstance():sendApplyRequest(6, 0)
        local pa = ClubJionLayer:create(self._scene, {}, ClubJionLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif TAG_ENUM.CBT_MYCLUB == tag then 
        self.m_nSelectBox = TAG_ENUM.CBT_MYCLUB

        self.m_tabCheckBox[TAG_ENUM.CBT_MYCLUB]:getChildByName("Sprite_2"):setVisible(true)
        self.m_tabCheckBox[TAG_ENUM.CBT_JOINCLUB]:getChildByName("Sprite_2"):setVisible(false)
        self:onRefreshInfo()

    elseif TAG_ENUM.CBT_JOINCLUB == tag then
        self.m_nSelectBox = TAG_ENUM.CBT_JOINCLUB
        self.m_tabCheckBox[TAG_ENUM.CBT_MYCLUB]:getChildByName("Sprite_2"):setVisible(false)
        self.m_tabCheckBox[TAG_ENUM.CBT_JOINCLUB]:getChildByName("Sprite_2"):setVisible(true)

        self:onRefreshInfo()
   
    elseif TAG_ENUM.BTN_MSG == tag then --消息   

        -- local pa = ClubMyApplyMsgLayer:create(self._scene, {}, ClubMyApplyMsgLayer.SECOND_LEVEL)
        -- self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BTN_HELP == tag then  --帮助
        local pa = ClubHelpLayer:create(self._scene, {}, ClubHelpLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif TAG_ENUM.BTN_FIRST == tag then  --置顶 
        local Clubinfo = sender.Clubinfo
        if nil == Clubinfo then
            print("大联盟信息为空")
            return
        end   
        if #self.m_clubSort > 0 then    
                for k,v in pairs(self.m_clubSort) do
                    if v and v.group == Clubinfo.dwGroupID  then
                        local oldIndex = k
                        local newIndex = 0
                        if v.top == 1 then 
                            local temp = clone(self.m_clubSort[oldIndex])
                            temp.top = 0

                            --table.remove(self.m_clubSort,oldIndex)

                            for i = 1,#self.m_clubSort do
                                 if self.m_clubSort[i].top == 0 and newIndex == 0 then
                                    newIndex = i
                                 end
                            end

                            if newIndex == 0 then
                                newIndex = #self.m_clubSort+1
                            end


                            sender:loadTextureNormal("club/first_sure.png")
                            sender:loadTexturePressed("club/first_sure.png")

                            table.insert(self.m_clubSort,newIndex,temp)
                            table.remove(self.m_clubSort,oldIndex)
                            
                        else
                            v.top = 1
                            sender:loadTextureNormal("club/first_cancel.png")
                            sender:loadTexturePressed("club/first_cancel.png")

                            local temp = self.m_clubSort[oldIndex]
                            table.remove(self.m_clubSort,oldIndex)
                            table.insert(self.m_clubSort,1,temp)
                        end

                        break
                    end

                end
        end
            --print(k.."======="..#self.m_first)
            --dump(self.m_first,"=======m_first2========")

            --cc.UserDefault:getInstance():setIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID.."clubNum", #self.m_clubSort.group)
            for i=1,#self.m_clubSort do
                cc.UserDefault:getInstance():setStringForKey(GlobalUserItem.tabAccountInfo.dwUserID.."clubSort"..i, self.m_clubSort[i].top.."_"..self.m_clubSort[i].group)
            end
            cc.UserDefault:getInstance():setBoolForKey(GlobalUserItem.tabAccountInfo.dwUserID.."lastFlag", false)
            self:onRefreshInfo()


    elseif TAG_ENUM.BTN_ENTER == tag then  --进入大联盟   

        local Clubinfo = sender.Clubinfo
        if nil == Clubinfo then
            print("大联盟信息为空")
            return
        end
        sender:setEnabled(false)
        sender:runAction(cc.Sequence:create(cc.DelayTime:create(2.5), cc.CallFunc:create(function ()
            sender:setEnabled(true)
        end)))

        --发送进入大联盟
        ClubFrame:getInstance().m_nCurrentClub = Clubinfo.dwGroupID
        ClubFrame:getInstance():showPopWait()
        ClubFrame:getInstance():OnResetClubData()
        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendEnterGroup(Clubinfo.dwGroupID)



    end
end



function ClubMain:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubMain:onTransitionInFinish()


        -- --如果是从大联盟游戏房间回来

    --刷新界面
    print("onTransitionInFinish")
    self:onRefreshInfo()
end

function ClubMain:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubMain:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubMain:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
    print("onOtherHideEvent")
    self:onRefreshInfo()
end

function ClubMain:onRefreshInfo()
    print("ClubMain:onRefreshInfo")
    --获取当前选中的大联盟数据
    self.m_tabClubInfo = {}
    local allGroup = clone(ClubFrame:getInstance().m_tabGroupList)


    if #self.m_clubSort == 0 and  #allGroup ~= 0 then 
        cc.UserDefault:getInstance():setIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID.."clubNum", #allGroup)
        for i=1,#allGroup do
                table.insert(self.m_clubSort,{group = allGroup[i].dwGroupID,top = 0})                         
                cc.UserDefault:getInstance():setStringForKey(GlobalUserItem.tabAccountInfo.dwUserID.."clubSort"..i, "0_"..allGroup[i].dwGroupID)              
        end
    elseif #self.m_clubSort ~= 0 and #allGroup ~= 0  then

        for i=1,#allGroup do
            for j=1,#self.m_clubSort do
                if self.m_clubSort[j].group ==  allGroup[i].dwGroupID then 
                    break
                elseif j == #self.m_clubSort then
                    table.insert(self.m_clubSort,{group = allGroup[i].dwGroupID,top = 0})   
                end
            end
        end

        for i=#self.m_clubSort,1,-1 do
            for j=1,#allGroup do
                if self.m_clubSort[i].group ==  allGroup[j].dwGroupID then 
                    break
                elseif j ==#allGroup then
                    table.remove(self.m_clubSort,i)
                end
            end
        end

        for i=1,#self.m_clubSort do
            cc.UserDefault:getInstance():setStringForKey(GlobalUserItem.tabAccountInfo.dwUserID.."clubSort"..i, self.m_clubSort[i].top.."_"..self.m_clubSort[i].group)
        end
        cc.UserDefault:getInstance():setBoolForKey(GlobalUserItem.tabAccountInfo.dwUserID.."lastFlag", false)
    end


    for i=1,#self.m_clubSort do
        for j=1,#allGroup do
            if self.m_clubSort[i].group == allGroup[j].dwGroupID then
                table.insert(self.m_tabClubInfo,clone(allGroup[j]))

                break
            end
        end
    end


    
    if #self.m_tabClubInfo == 0 then 
        self.m_labTips:setVisible(true)
        self.m_labTips:setString("还没有创建/加入的大联盟哦，快去创建/加入吧~")
    else
        self.m_labTips:setVisible(false)
    end 

    self.m_labMyClubNum:setString(#self.m_tabClubInfo)

    -- if #self.m_tabClubInfo > 4 then
    --     self.m_rightIcon:setVisible(true)
    -- else
    --     self.m_rightIcon:setVisible(false)
    -- end



    --dump(self.m_tabClubInfo, "self.m_tabClubInfo", 10)
    self._listClubView:reloadData()


end


function ClubMain:scrollViewDidScroll(view)
        local point = view:getContentOffset()
        local width,height = self:cellSizeForTable()
        local num = #self.m_tabClubInfo
        local tableWidth = 1264
        local totalWidth = width*num

        if num > 5  then
            
            if totalWidth - (math.abs(point.x) + tableWidth) >= 2/3*width then
                self.m_rightIcon:setVisible(true)
            else
                self.m_rightIcon:setVisible(false)
            end

            if math.abs(point.x) > 2/3*width then
                self.m_leftIcon:setVisible(true)
            else
                self.m_leftIcon:setVisible(false)
            end

            if point.x > 0 then
                self.m_leftIcon:setVisible(false)
                self.m_rightIcon:setVisible(true)
            end

        end

end

function ClubMain:cellSizeForTable(view, idx)
    return 365 , 468
end

function ClubMain:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local Clubinfo = self.m_tabClubInfo[idx +1]
    if nil == Clubinfo then
        return cell
    end

    dump(Clubinfo)
    local csb = ExternalFun.loadCSB("club/node_clubcell.csb", cell)

    csb:setPosition(182.5, view:getViewSize().height * 0.5)

    local bg = csb:getChildByName("bg")
    bg:addTouchEventListener(self.m_itemFunC)
    bg:setTag(TAG_ENUM.BTN_ENTER)
    bg:setSwallowTouches(false)
    bg.Clubinfo = Clubinfo

    local btn_first = bg:getChildByName("Button_2")
    btn_first:addTouchEventListener(self.m_itemFunC)
    btn_first:setTag(TAG_ENUM.BTN_FIRST)
    btn_first:setSwallowTouches(false)
    btn_first.Clubinfo = Clubinfo
    btn_first:loadTextureNormal("club/first_sure.png")
    btn_first:loadTexturePressed("club/first_sure.png")

    for k,v in pairs(self.m_clubSort) do
        if v and v.group == Clubinfo.dwGroupID then
            if v.top == 1  then 
                btn_first:loadTextureNormal("club/first_cancel.png")
                btn_first:loadTexturePressed("club/first_cancel.png")
                btn_first:getChildByName("Text_44"):setString("取消")
                break
            else
                btn_first:loadTextureNormal("club/first_sure.png")
                btn_first:loadTexturePressed("club/first_sure.png")
                btn_first:getChildByName("Text_44"):setString("置顶")
            end
        end
    end




    bg:getChildByName("txt_clubName"):setString(Clubinfo.szGroupName.."")

    local iconImg = bg:getChildByName("Sprite_5")
    local txtTip = bg:getChildByName("Text_6")
    local txtID = bg:getChildByName("txt_clubID")

    if Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_OWNER or 
       Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_MANAGER then

       iconImg:setTexture("club/clubRight2.png")
       txtTip:setString("我的邀请码:")
       txtID:setString(Clubinfo.cmd_extra.lInvitationCode.."")

    elseif Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_CAPTAIN then
        iconImg:setTexture("club/clubRight1.png")
        txtTip:setString("我的邀请码:")
        txtID:setString(Clubinfo.cmd_extra.lInvitationCode.."")
    elseif Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_TEAMMEMBER or Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_MEMBER then
        iconImg:setVisible(false)
        txtTip:setString("上级邀请码:")
        txtID:setString(Clubinfo.cmd_extra.lInvitationCode.."")
    end

    if Clubinfo.cmd_extra.cbGroupShow == 1  then
        txtTip:setString("大联盟ID:")
        txtID:setString(Clubinfo.dwGroupID.."")
    end

    bg:getChildByName("txt_num"):setString(Clubinfo.wMemberCount.."/"..Clubinfo.wMaxMemberCount)

    if Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_OWNER or 
           Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_MANAGER then
           bg:loadTexture("club/clubBtn3.png")  
    elseif Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_CAPTAIN then
            bg:loadTexture("club/clubBtn2.png")  
    elseif Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_TEAMMEMBER or 
        Clubinfo.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_MEMBER then
            bg:loadTexture("club/clubBtn1.png")  
    end



    -- local txtName = bg:getChildByName("txt_clubName")
    -- if nil ~= txtName then
    --     txtName:setString(Clubinfo.szGroupName)
    -- end

    -- local txtHolderNick = bg:getChildByName("txt_HolderNick")
    -- if nil ~= txtHolderNick then
    --     local strNickname = string.EllipsisByConfig(Clubinfo.szCreaterNickName, 120, 
    --                                         string.getConfig("fonts/round_body.ttf", 24))
    --     txtHolderNick:setString(strNickname)
    -- end

    -- local txt_num = bg:getChildByName("txt_num")
    -- if nil ~= txt_num then
    --     txt_num:setString(Clubinfo.wMemberCount.."/"..Clubinfo.wMaxMemberCount)
    -- end


            -- --获取玩家信息
            -- local iteminfo = {}
            -- iteminfo.dwUserID = Clubinfo.dwCreaterID

            -- -- 头像
            -- local tmp = bg:getChildByName("Node_1")
            -- local head = PopupInfoHead:createClipHead(iteminfo, 95,"club/createrHead1.png")
            -- head:enableInfoPop(false)
            -- head:setAnchorPoint(cc.p(0.5,0.5))
            -- tmp:addChild(head)
            -- head:setPosition(0,2)
            -- head:setName("head")


            -- local data, sign = GlobalUserItem:useridSign(iteminfo.dwUserID)
            -- local action = string.format("action=GetUserInfo&userid=%s&time=%s&sign=%s", iteminfo.dwUserID, data, sign)
            -- appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
            --     if type(jstable) == "table" then
            --         local data = jstable["data"]
            --         local msg = jstable["msg"]
            --         if type(data) == "table" then
            --             local valid = data["valid"]
            --             if valid then
            --                 iteminfo.szSign = data["UnderWrite"]
            --                 iteminfo.cbGender = tonumber(data["cbGender"])
            --                 iteminfo.UserID = data["UserID"]
            --                 iteminfo.szNickName = data["NickName"]
            --                 iteminfo.dwCustomID = data["CustomID"]
            --                 iteminfo.dwGameID = data["GameID"]
            --             end
                        
            --             if bg and tolua.cast(bg,"cc.Node")then
                                
            --                     local tmp = bg:getChildByName("Node_1")
            --                     tmp:removeChildByName("head")
            --                     local head = PopupInfoHead:createClipHead(iteminfo, 96,"club/createrHead1.png")
            --                     head:enableInfoPop(false)
            --                     head:setAnchorPoint(cc.p(0.5,0.5))
            --                     tmp:addChild(head)
            --                     head:setPosition(0,2)
            --                     head:setName("head")

            --             end
            --         end
            --     end
            -- end)



    return cell

end

-- 子视图数目
function ClubMain:numberOfCellsInTableView(view)
    return #self.m_tabClubInfo
end



--加入房间回调
function ClubMain:onSubGroupEnterSuccess()
    --收到此消息说明进入大联盟成功了
    -- local pa = ClubRoomLayer:create(self._scene, {}, ClubMain.SECOND_LEVEL +1)
    -- self._scene:addPopLayer(pa)
    -- self:setVisible(true)
    ClubFrame:getInstance():dismissPopWait()
    self._scene:onChangeShowMode(yl.SCENE_CLUB_ROOM)
    --pa:onSubGroupEnterSuccess()


end

function ClubMain:onSubApplyMessageList()

end

--刷新列表
function ClubMain:onSubGroupList()
    self:onRefreshInfo()
end

function ClubMain:onSubEnterSuccess()
    self:onRefreshInfo()
end

function ClubMain:onSubLoginGroupSuccess()
   

end


function ClubMain:onClubInfoChange( event  )
    print("----------ClubMain userinfo change notify------------")
    local msgWhat = event.obj
    
    print("----------userinfo change notify------------", msgWhat)
    if nil ~= msgWhat then
       if msgWhat == yl.RY_CULB_MEMBER_NOTIFY or msgWhat == yl.RY_CULB_DELETE_NOTIFY or msgWhat == yl.RY_CULB_OUT_NOTIFY then
            self:onRefreshInfo()
        end

        if msgWhat == yl.RY_CULB_ENTER_NOTIFY then
            local dwGroupID = event.dwGroupID
            if dwGroupID then

                ClubFrame:getInstance().m_nCurrentClub = dwGroupID
                ClubFrame:getInstance():OnResetClubData()
                ClubFrame:getInstance():showPopWait()
                ClubFrame:getInstance():setViewFrame(self)
                ClubFrame:getInstance():sendEnterGroup(dwGroupID)

            end
        end
    end
end



return ClubMain