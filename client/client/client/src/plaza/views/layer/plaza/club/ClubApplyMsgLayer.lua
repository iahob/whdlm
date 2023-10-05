
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
-- 用户中心(二级弹窗)
local ClubApplyMsgLayer = class("ClubApplyMsgLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_MSG1",          -- 勋章记录
    "BTN_MSG2",        -- 加入申请
    "BTN_MSG3",        -- 退出申请
    "BTN_ALL",
    "BTN_PASS",             -- 通过
    "BTN_DELETE",           -- 删除 
    "BTN_SERCH",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubApplyMsgLayer:ctor( scene, param, level )
    local this = self

    ClubApplyMsgLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_msg.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("Panel_3")
    --mask:setTag(TAG_ENUM.TAG_MASK)
    --mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    --spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_msgBtn = {}
    for i=1 ,3 do
        local btn = spbg:getChildByName("Image_2"):getChildByName("btn_msg"..i)
        btn:setTag(TAG_ENUM.BTN_MSG1+i-1)
        btn:addTouchEventListener( touchFunC )
        btn:setPressedActionEnabled(true)

        self.m_msgBtn[i] =  btn
    end


    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo

    self.m_tabRecord = {}  --勋章记录
    self.m_tabMsg = {} --申请消息列表

    self.m_curType = 1 --1,勋章,2,加入；3，退出 ,

    self.m_captainId = param.captainid 
    self.m_pageIndex = param.pageIndex

    -- 按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listQuery then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listQuery:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listQuery:getViewSize().width, self._listQuery:getViewSize().height)
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
            :setPosition(929,45)
            :setFontColor(cc.c3b(223, 201, 255))
            :setAnchorPoint(cc.p(0,0.5))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 25)
            :setFont("fonts/round_body.ttf", 25) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    mask:addChild(editbox)         
    self.m_editID = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.serchID = spbg:getChildByName("shuru")
    self.serchID:setString("输入ID查询")


    self.searchImg = spbg:getChildByName("Image_7")
    self.searchImg:setTag(TAG_ENUM.BTN_SERCH)
    self.searchImg:addTouchEventListener( touchFunC )
    self.searchImg:setTouchEnabled(true)



--勋章记录
    self.m_xzPanel = self.m_spBg:getChildByName("Panel_1")
    local tmp = self.m_xzPanel:getChildByName("content")
    self._listRecord = cc.TableView:create(tmp:getContentSize())
    self._listRecord:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listRecord:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listRecord:setPosition(0,0)
    self._listRecord:setDelegate()
    self._listRecord:addTo(tmp)
    self._listRecord:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listRecord:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listRecord:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listRecord:reloadData()

--申请记录
    self.m_sqPanel = self.m_spBg:getChildByName("Panel_2")
    self.m_sqPanel:setVisible(false)
    local tmp = self.m_sqPanel:getChildByName("content")
    self._listQuery = cc.TableView:create(tmp:getContentSize())
    self._listQuery:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listQuery:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)      
    self._listQuery:setPosition(0,0)
    self._listQuery:setDelegate()
    self._listQuery:addTo(tmp)
    self._listQuery:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listQuery:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listQuery:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listQuery:reloadData()

    local btn = self.m_sqPanel:getChildByName("btn_all")
    btn:setTag(TAG_ENUM.BTN_ALL)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    self.m_xzPanel:setVisible(true)
    ClubFrame:getInstance():showPopWait()
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabScoreRecord = {}
    ClubFrame:getInstance():sendQueryScoreRecord(ClubFrame:getInstance().m_nCurrentClub,self.m_captainId)

    if ( ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT2) or self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID ) and not self.m_captainId then
        self.m_spBg:getChildByName("Image_2"):setVisible(true)    
        if self.m_pageIndex ~= 1 then
            self:onButtonClickedEvent(TAG_ENUM.BTN_MSG1 +self.m_pageIndex -1 )
        end
    else
        self.m_spBg:getChildByName("Image_2"):setVisible(false)
        self.m_spBg:getChildByName("Panel_1"):setPositionY(19)
    end

    self:onSubApplyMessageList()

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
end


function ClubApplyMsgLayer:onEditEvent(event, editbox)

    local src = editbox:getText() 
    if self.m_editID == editbox then 
        if event == "return" then
            self.searchImg:setEnabled(true)
            self.serchID:setString(src)
            if self.serchID:getContentSize().width == 0 then --没有输入
                self.serchID:setString("输入ID查询")
            end
            if string.len(src) == 6  then 
                if self.m_curType == 1 then
                    self:onRecordRefresh(tonumber(src))
                else
                    self:onSubApplyMessageList(tonumber(src))
                end
            elseif string.len(src) > 0  then 
                showToast(self, "请输入正确的玩家ID！", 2)
            else
                if self.m_curType == 1 then
                    self:onRecordRefresh()
                else
                    self:onSubApplyMessageList()
                end
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
end

function ClubApplyMsgLayer:onExit()

    ClubApplyMsgLayer.super.onExit(self)
end

-- 按键监听
function ClubApplyMsgLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        --self:scaleTransitionOut(self.m_spBg)
        self:removeFromParent()
    elseif TAG_ENUM.BTN_SERCH == tag then

        self.m_editID:touchDownAction(self.m_editID, ccui.TouchEventType.ended)
        self.searchImg:setEnabled(false)

    elseif TAG_ENUM.BTN_ALL == tag then --全部通过
        --遍历通过
        for i,v in ipairs(self.m_tabMsg) do
            if nil ~= v.dwMessageID and v.cbMessageStatus == cmd_Club.APPLY_STATUS_NONE then
                ClubFrame:getInstance():showPopWait()
                ClubFrame:getInstance():setViewFrame(self)
                ClubFrame:getInstance():sendApplyRespond(v.dwMessageID, v.dwGroupID, v.dwApplyerID, cmd_Club.APPLY_STATUS_AGREE)
            end
        end
    elseif TAG_ENUM.BTN_MSG1 <= tag  and TAG_ENUM.BTN_MSG3 >= tag then
        for k,v in ipairs(self.m_msgBtn) do
            if v then
                v:getChildByName("Image_3"):setVisible(false)
            end           
        end

        self.m_curType = tag-TAG_ENUM.BTN_MSG1+1

        print(self.m_curType.."=========sfsfs===========")
        self.m_msgBtn[self.m_curType]:getChildByName("Image_3"):setVisible(true)
        self.m_sqPanel:setVisible(false)
        self.m_xzPanel:setVisible(false)

        if TAG_ENUM.BTN_MSG1 == tag then
            self.m_xzPanel:setVisible(true)
            self:onRecordRefresh()
        else
            self.m_sqPanel:setVisible(true)
            self:onSubApplyMessageList()
        end

    end

end

function ClubApplyMsgLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubApplyMsgLayer:onTransitionInFinish()
    self:onRefreshInfo()
end

function ClubApplyMsgLayer:onTransitionOutFinish()


    self:removeFromParent()
end

function ClubApplyMsgLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubApplyMsgLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

function ClubApplyMsgLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function ClubApplyMsgLayer:onRefreshInfo()
    --获取当前选中的大联盟数据

end


function ClubApplyMsgLayer:cellSizeForTable(view, idx)
    return 1100 , 95
end

function ClubApplyMsgLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    if self._listQuery == view then
            local MsgInfo = self.m_tabMsg[idx +1]
            if nil == MsgInfo then
                return cell
            end

            local csb = ExternalFun.loadCSB("club/node_msg2.csb", cell)
            csb:setPosition(0, 0)

            local isRecord = false


            local strType = self.m_curType == 2 and "加入" or "退出"
            if MsgInfo.cbMessageStatus == cmd_Club.APPLY_STATUS_AGREE then
                local icon = csb:getChildByName("status")
                icon:setVisible(true)
                icon:setTexture("club/message_v.png")
                local txt = icon:getChildByName("Text_10")
                txt:setString("已通过"..strType.."申请")
                txt:setColor(cc.c3b(14, 133, 64))
                isRecord = true
            elseif MsgInfo.cbMessageStatus == cmd_Club.APPLY_STATUS_REFUSE then
                local icon = csb:getChildByName("status")
                icon:setVisible(true)
                icon:setTexture("club/message_x.png")
                local txt = icon:getChildByName("Text_10")
                txt:setString("已拒绝"..strType.."申请")
                txt:setColor(cc.c3b(180, 85, 56))
                isRecord = true
            elseif MsgInfo.cbMessageStatus == cmd_Club.APPLY_STATUS_REPEAL then
                csb:getChildByName("control"):setVisible(true)
                csb:getChildByName("control"):setString("被删除")
                isRecord = true
            end

            local btn = csb:getChildByName("btn_ok")
            btn:setPressedActionEnabled(true)
            btn:setSwallowTouches(false)
            btn:addTouchEventListener(self.m_itemFunC)
            btn.MsgInfo = MsgInfo
            btn:setTag(TAG_ENUM.BTN_PASS)
            btn:setVisible(not isRecord)

            local btn = csb:getChildByName("btn_no")
            btn:setPressedActionEnabled(true)
            btn:setSwallowTouches(false)
            btn:addTouchEventListener(self.m_itemFunC)
            btn.MsgInfo = MsgInfo
            btn:setTag(TAG_ENUM.BTN_DELETE)
            btn:setVisible(not isRecord)


            --获取玩家信息
            local iteminfo = {}
            iteminfo.dwUserID = MsgInfo.dwApplyerID

            -- 头像
            local tmp = csb:getChildByName("Image_2")
            local head = PopupInfoHead:createNormal(iteminfo, tmp:getContentSize().width-1)
            head:enableInfoPop(false)
            csb:addChild(head)
            head:setPosition(tmp:getPosition())
            head:setName("head")


            local data, sign = GlobalUserItem:useridSign(iteminfo.dwUserID)
            local action = string.format("action=GetUserInfo&userid=%s&time=%s&sign=%s", iteminfo.dwUserID, data, sign)
            print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
            local jsonStr = cjson.encode(action)
            LogAsset:getInstance():logData(jsonStr, true)
            appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
                dump(jstable, "获取玩家信息", 7)
                if type(jstable) == "table" then
                    local data = jstable["data"]
                    local msg = jstable["msg"]
                    if type(data) == "table" then
                        local valid = data["valid"]
                        if valid then
                            iteminfo.szSign = data["UnderWrite"]
                            iteminfo.cbGender = tonumber(data["cbGender"])
                            iteminfo.UserID = data["UserID"]
                            iteminfo.szNickName = data["NickName"]
                            iteminfo.dwCustomID = data["CustomID"]
                            iteminfo.dwGameID = data["GameID"]
                        end
                        
                        if csb and tolua.cast(csb,"cc.Node")then
                                csb:removeChildByName("head")
                                local tmp = csb:getChildByName("Image_2")
                                local head = PopupInfoHead:createNormal(iteminfo, tmp:getContentSize().width-1)
                                head:enableInfoPop(false)
                                csb:addChild(head)
                                head:setPosition(tmp:getPosition())
                                head:setName("head")

                        end
                    end
                end
            end)



            local txtID = csb:getChildByName("id")
            if nil ~= txtID then
                txtID:setString("ID:"..MsgInfo.dwApplyerGameID)
            end

            local txtName = csb:getChildByName("name")
            if nil ~= txtName then
                txtName:setString(ExternalFun.clipText(MsgInfo.szApplyerNickName,6,true))
            end

            --申请时间
            local txt_time = csb:getChildByName("time")
            if nil ~= txt_time then
                local strTime = MsgInfo.ApplyDateTime.wMonth .. "/" .. MsgInfo.ApplyDateTime.wDay 
                                    .. "  ".. MsgInfo.ApplyDateTime.wHour .. ":" .. ExternalFun.changeTimeText(MsgInfo.ApplyDateTime.wMinute)
                txt_time:setString(strTime)
            end
    elseif self._listRecord ==  view then 
            print("index "..idx)
            local RecordInfo = self.m_tabRecord[idx +1]
            if nil == RecordInfo then
                return cell
            end

            local csb = ExternalFun.loadCSB("club/node_msg1.csb", cell)
            csb:setPosition(0, 0)

            local mngStr = {[1] = "盟主",[2] = "管理员",[4] = "队长"}
            csb:getChildByName("name"):setString(ExternalFun.clipText(RecordInfo.szReceiveNickName,6,true).."")
            csb:getChildByName("id"):setString("ID:"..RecordInfo.dwReceiveGameID.."")
            csb:getChildByName("time"):setString(RecordInfo.CollectDateTime.wMonth .. "/" .. RecordInfo.CollectDateTime.wDay .. "  ".. RecordInfo.CollectDateTime.wHour .. ":" .. ExternalFun.changeTimeText(RecordInfo.CollectDateTime.wMinute))
            csb:getChildByName("num1"):setString(RecordInfo.lChangeScore.."")
            csb:getChildByName("num2"):setString(RecordInfo.lCurScore.."")
            csb:getChildByName("num2_0"):setString(mngStr[RecordInfo.cbPresentUserRight])
            csb:getChildByName("name_0"):setString(ExternalFun.clipText(RecordInfo.szPresentNickName,6,true))
            csb:getChildByName("id_0"):setString("ID:"..RecordInfo.dwPresentGameID.."")


    end

    return cell

end

-- 子视图数目
function ClubApplyMsgLayer:numberOfCellsInTableView(view)
    if self._listRecord == view then 
            return #self.m_tabRecord
    elseif self._listQuery == view then 
            return #self.m_tabMsg
    end
end

function ClubApplyMsgLayer:onItemButtonClickedEvent( tag, ref )
    local MsgInfo = ref.MsgInfo
    if nil == MsgInfo then
        print("消息为空")
        return
    end
    if tag == TAG_ENUM.BTN_PASS then
        ClubFrame:getInstance():showPopWait()
        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendApplyRespond(MsgInfo.dwMessageID, MsgInfo.dwGroupID, MsgInfo.dwApplyerID, cmd_Club.APPLY_STATUS_AGREE)
    elseif tag == TAG_ENUM.BTN_DELETE then
        ClubFrame:getInstance():showPopWait()
        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendApplyRespond(MsgInfo.dwMessageID, MsgInfo.dwGroupID, MsgInfo.dwApplyerID, cmd_Club.APPLY_STATUS_REFUSE)
    end

end

function ClubApplyMsgLayer:onSubApplyMessageList(id)
    print("刷新消息列表")
    ClubFrame:getInstance():dismissPopWait()
    --刷新列表
    self.m_tabMsg = {}
    local tabMsgTemp = {}
    for i,v in ipairs(ClubFrame:getInstance().m_tabApplyRespondMsg) do
        if v.dwGroupID == ClubFrame:getInstance().m_nCurrentClub then
            if self.m_curType -2  ==  v.cbMessageType then 
                if id then
                    if id ==  v.dwApplyerGameID then
                        table.insert(self.m_tabMsg, v)
                    end
                else
                    table.insert(self.m_tabMsg, v)
                end
            end
            table.insert(tabMsgTemp, v)
        end
    end


    if id and #self.m_tabMsg ==0 then
                showToast(self, "没有该玩家的申请或输入的ID不正确！", 2)
                return  
    end 
    --dump(self.m_tabMsg)

    --是否显示红点
    local isHave = false
    local inRed = false
    local outRed = false
        for k,v in pairs(tabMsgTemp) do
            if v and v.cbMessageStatus == cmd_Club.APPLY_STATUS_NONE then
                if v.cbMessageType == 0 then 
                    inRed = true
                elseif v.cbMessageType == 1 then
                    outRed = true
                end
                if self.m_curType -2  ==  v.cbMessageType  then 
                    isHave = true
                end
            end
        end

    self.m_sqPanel:getChildByName("btn_all"):setVisible(isHave)
    self.m_spBg:getChildByName("Image_2"):getChildByName("red2"):setVisible(inRed)
    self.m_spBg:getChildByName("Image_2"):getChildByName("red3"):setVisible(outRed)



    ---------------------------------
    self._listQuery:reloadData()

end



function ClubApplyMsgLayer:onRecordRefresh(id)
    ClubFrame:getInstance():dismissPopWait()
    self.m_tabRecord = {}
    if id then
            for i,v in ipairs(ClubFrame:getInstance().m_tabScoreRecord) do
        
                if id ==  v.dwReceiveGameID then
                    table.insert(self.m_tabRecord, v)
                end
            end
            if #self.m_tabRecord ==0 then
                showToast(self, "没有该玩家的记录或输入的ID不正确！", 2)
                return  
            end 
        
    else
        self.m_tabRecord = clone(ClubFrame:getInstance().m_tabScoreRecord)
    end
    self._listRecord:reloadData()

end

return ClubApplyMsgLayer