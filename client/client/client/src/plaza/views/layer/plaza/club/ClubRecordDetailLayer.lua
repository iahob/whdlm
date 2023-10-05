
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local ClubShareLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubShareLayer")


-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubRecordDetailLayer = class("ClubRecordDetailLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "IMAGE_EDIT",           -- 编辑
    "BTN_SHARE",
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubRecordDetailLayer:ctor( scene, param, level )
    local this = self

    ClubRecordDetailLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_dj.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._scene = scene

    self.m_tabMemberList = ClubFrame:getInstance().m_tabGroupMember
    self.m_myGroupInfo = GlobalUserItem.tabGroupUserInfo


    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    --spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    --spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    -- 分享
    local btn = spbg:getChildByName("btn_share")
    btn:setTag(TAG_ENUM.BTN_SHARE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_gName = spbg:getChildByName("gameName")
    self.m_roomNum = spbg:getChildByName("roomNum")
    self.m_playCount = spbg:getChildByName("jushu")
    self.m_minScore = spbg:getChildByName("score")

    self.m_totalScore = {}
    for i=1,9 do
        self.m_totalScore[i] =  spbg:getChildByName("ap"..i)
        self.m_totalScore[i]:setVisible(false)
    end

    self.m_roomData = {}
    self.m_roomData.turnData = {}

    --成员列表
    local tmp = self.m_spBg:getChildByName("Panel_2")
    self._listDetailView = cc.TableView:create(tmp:getContentSize())
    self._listDetailView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listDetailView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listDetailView:setPosition(cc.p(0, 0))
    self._listDetailView:setDelegate()
    self._listDetailView:addTo(tmp)
    self._listDetailView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listDetailView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listDetailView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

    self._listDetailView:reloadData()

    self.m_extra = nil
    for k,v in pairs(ClubFrame:getInstance().m_tabGroupList) do
        if v and v.dwGroupID == ClubFrame:getInstance().m_nCurrentClub then
            self.m_extra = v.cmd_extra
            break
        end
    end



    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
    ClubFrame:getInstance().m_tabRoomDetail = nil
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance():sendQueryRoomDetail(ClubFrame:getInstance().m_nCurrentClub,param.dwRoomID,param.dwDateID,param.endtime)

end

function ClubRecordDetailLayer:onExit()
    ClubRecordDetailLayer.super.onExit(self)
end

-- 按键监听
function ClubRecordDetailLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        --self:scaleTransitionOut(self.m_spBg)
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_SHARE then
                local param ={}
        param.title = "大联盟战绩"

        local str = "大联盟ID:"..ClubFrame:getInstance().m_nCurrentClub

        if self.m_extra then
            if self.m_extra.cbGroupShow ==  0 then
                str = "大联盟邀请码:"..self.m_extra.lInvitationCode
            end
        end

        local myScore = 0
        for i=1,self.m_roomData.cbPlayerCount do
            if self.m_myGroupInfo.UserInfo.szNickName ==  self.m_roomData.szNickName[1][i] then
                myScore = self.m_roomData.lAllScore[1][i]
            end
        end


        param.content = str..",我的ID:"..GlobalUserItem.tabAccountInfo.dwGameID..",总局数:"..
        self.m_roomData.nDrawCount.."局,总得分:"..myScore.."."

        local pa = ClubShareLayer:create(self._scene, param, ClubShareLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)  


    elseif tag == TAG_ENUM.BTN_DATE then

    elseif tag == TAG_ENUM.BTN_DATEBG then

    end
end

function ClubRecordDetailLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubRecordDetailLayer:onTransitionInFinish()
 

end

function ClubRecordDetailLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubRecordDetailLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubRecordDetailLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end


function ClubRecordDetailLayer:onRefreshInfo(data)
    if not data then
        return
    end
    
    self.m_roomData = clone(data)
    local allData = clone(data)

    self.m_gName:setString(data.szKindName.."")
    self.m_roomNum:setString(data.dwRoomID.."")
    self.m_playCount:setString(data.nDrawCount.."")
    self.m_minScore:setString(data.lMinTableScore.."")

    for i=1,16 do
            for j =1, 16 -i do
                 if allData.lAllScore[1][j] < allData.lAllScore[1][j+1] then
                    local temp1 = allData.lAllScore[1][j]
                    local temp2 = allData.szNickName[1][j]

                    allData.lAllScore[1][j] = allData.lAllScore[1][j+1]
                    allData.szNickName[1][j] = allData.szNickName[1][j+1]

                    allData.lAllScore[1][j+1] = temp1
                    allData.szNickName[1][j+1] = temp2

                 end
            end
    end


    local allScoreData = {}
    local index = 0
    for i=1,16 do
            if allData.szNickName[1][i] ~= "" then
                index = index+1
                self.m_totalScore[index]:getChildByName("name"):setString(ExternalFun.clipText(allData.szNickName[1][i],4,true))
                self.m_totalScore[index]:getChildByName("score"):setString(allData.lAllScore[1][i].."")
                self.m_totalScore[index]:setVisible(true)
            end
    end



    self._listDetailView:reloadData()
end



function ClubRecordDetailLayer:cellSizeForTable(view, idx)
    return 1090 , 230
end

function ClubRecordDetailLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local cellData = self.m_roomData.turnData[idx +1]
    if nil == cellData then
        return cell
    end


    local csb = ExternalFun.loadCSB("club/node_dj.csb", cell)
    csb:setAnchorPoint(cc.p(0,0))
    csb:setPosition(0, 0)

    

    csb:getChildByName("Text_2"):setString(cellData.nTurnIndex.."/"..self.m_roomData.nDrawCount)


    csb:getChildByName("Text_2_0"):setString(cellData.StartTime.wYear.."/"..cellData.StartTime.wMonth.."/"..cellData.StartTime.wDay.." "
        ..cellData.StartTime.wHour..":"..ExternalFun.changeTimeText(cellData.StartTime.wMinute)..":"..ExternalFun.changeTimeText(cellData.StartTime.wSecond))
    dump(cellData.StartTime)

    local index = 0
    for i=1, 16 do
        if cellData.bFlag[1][i] and self.m_roomData.szNickName[1][i] ~= "" then 
            index = index+1
            local nick =  ClipText:createClipText(cc.size(141,26),self.m_roomData.szNickName[1][i],"fonts/FZHTJW.TTF",23)
            nick:setColor(cc.c3b(255, 255, 255))
            nick:setAnchorPoint(cc.p(0, 0.5))
            nick:setPosition(50+math.floor((index-1)/3)*395,131-(index-1)%3*40)
            csb:addChild(nick)

            local score = cc.Label:createWithTTF(cellData.lTurnScore[1][i].."", "fonts/FZHTJW.TTF", 23)
            score:setColor(cc.c3b(255, 234, 0))
            score:setAnchorPoint(cc.p(0, 0.5))
            score:setPosition(189+math.floor((index-1)/3)*395,131-(index-1)%3*40)
            csb:addChild(score)
        end
    end


    local btn = csb:getChildByName("btn_share")
    btn:addTouchEventListener(function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                    local param ={}
                    param.title = "大联盟战绩"

                    local str = "大联盟ID:"..ClubFrame:getInstance().m_nCurrentClub

                    if self.m_extra then
                        if self.m_extra.cbGroupShow ==  0 then
                            str = "大联盟邀请码:"..self.m_extra.lInvitationCode
                        end
                    end

                    local myScore = 0
                    for i=1,16 do
                        if self.m_myGroupInfo.UserInfo.szNickName ==  self.m_roomData.szNickName[1][i] then
                            myScore = cellData.lTurnScore[1][i]
                        end
                    end

                    param.content = str..",我的ID:"..GlobalUserItem.tabAccountInfo.dwGameID..",局数:"..
                    cellData.nTurnIndex.."/"..self.m_roomData.nDrawCount..",得分:"..myScore.."."

                    local pa = ClubShareLayer:create(self._scene, param, ClubShareLayer.SECOND_LEVEL)
                    self._scene:addPopLayer(pa)  
            end
        end)


    return cell

end

-- 子视图数目
function ClubRecordDetailLayer:numberOfCellsInTableView(view)
    return #self.m_roomData.turnData
end




return ClubRecordDetailLayer