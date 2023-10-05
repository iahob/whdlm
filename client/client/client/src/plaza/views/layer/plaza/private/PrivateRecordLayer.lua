--
-- Author: zhong
-- Date: 2017-05-31 19:08:58
--
-- 约战记录(二级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PrivateRecordLayer = class("PrivateRecordLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- ui
local PrivateDetailLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateDetailLayer")      -- 详情
local VideoMarkDetailLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "video.VideoMarkDetailLayer")    -- 回放详情
-- ui

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BOX_ROOMDAIKAI",       --代开
    "BOX_ROOMMY",           --我的
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

local fromTable = 
{
    "FROM_IDINPUT",         -- 从id输入界面进入
    "FROM_BACKGAME",        -- 从返回游戏进入
}
local FROM_ENUM = ExternalFun.declarEnumWithTable(10, fromTable)
PrivateRecordLayer.FROM_ENUM = FROM_ENUM
function PrivateRecordLayer:ctor( scene, param, level )
    PrivateRecordLayer.super.ctor( self, scene, param, level )
    self._param = param or {}
    -- 记录来源
    self.m_nFrom = self._param.from or FROM_ENUM.FROM_IDINPUT

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("private/PrivateRecordLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --房间切换
    local Boxlistener = function (sender,eventType)
        self:onBoxSelectedEvent(sender:getTag(),sender)
    end

    self.boxRoomDaiKai = spbg:getChildByName("CheckBox_1")
    self.boxRoomDaiKai:setTag(TAG_ENUM.BOX_ROOMDAIKAI)
    self.boxRoomDaiKai:addEventListener(Boxlistener)

    self.boxRoomMy = spbg:getChildByName("CheckBox_2")
    self.boxRoomMy:setTag(TAG_ENUM.BOX_ROOMMY)
    self.boxRoomMy:addEventListener(Boxlistener)

    self.boxSelect = spbg:getChildByName("pri_sp_tab_select")

    --tips
    self.tipsTxt = spbg:getChildByName("txt_tips")


    -- 详情按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listView:getViewSize().width, self._listView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                self:onItemButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end

    self.m_tabList = self._param.list or {}
    self.m_nListCount = 0

    --用于显示的列表
    self.m_showList = {}
    self.m_nShowListCount = 0

    -- 列表
    local tmp = spbg:getChildByName("content")
    local tmpsize = tmp:getContentSize()
    self.m_nCellSize = tmpsize.height / 3
    self._listView = cc.TableView:create(tmpsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(spbg)
    --self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    --self._listView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    tmp:removeFromParent()
    self._listView:reloadData()

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleTransitionIn(spbg)
end

-- 按键监听
function PrivateRecordLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    end
end

-- 按键监听
function PrivateRecordLayer:onItemButtonClickedEvent( tag,sender )
    local tabData = sender.info
    if type(tabData) ~= "table" then
        dump(tabData, "详情数据非法", 6)
        return
    end
    local bOnGame = (tabData.cbIsDisssumRoom ~= PriRoom.ROOM_STATE.END_STATE)
    local tabDetail = tabData
    tabDetail.onGame = bOnGame
    tabDetail.enableDismiss = true

    if bOnGame then
        -- 约战详情
        local rd = PrivateDetailLayer:create( self._scene, tabDetail, self:getLevel() + 1)
        self._scene:addPopLayer(rd)
    else
        -- 移除回调
        PriRoom:getInstance():setViewFrame(nil)

        -- 来自约战记录
        local param = {}
        param.dwPersonalRoomID = tabDetail.dwPersonalRoomID
        param.nFrom = VideoMarkDetailLayer.FROM_PRIVATE
        -- 战绩详情
        local vd = VideoMarkDetailLayer:create( self._scene, param, self:getLevel() + 1)
        self._scene:addPopLayer(vd)

        -- 移除自己
        self:scaleTransitionOut(self.m_spBg)
    end
end

function PrivateRecordLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PrivateRecordLayer:onTransitionInFinish()
    if nil == self.m_tabList or 0 == #(self.m_tabList) then
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():setViewFrame(self)
        -- 请求记录列表
        PriRoom:getInstance():getNetFrame():onQueryRoomList()
    else
        local count = #(self.m_tabList)
        self.m_nListCount = math.floor( count/2 )
        if count > 0 then
            if math.mod(count ,2) ~= 0 then
                self.m_nListCount = self.m_nListCount + 1
            end
        end
        self:onGetShowRoomList()
        self._listView:reloadData()
    end
end

function PrivateRecordLayer:onTransitionOutFinish()
    -- 移除回调
    PriRoom:getInstance():setViewFrame(nil)
    self:removeFromParent()
end

function PrivateRecordLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
        -- 移除回调
        PriRoom:getInstance():setViewFrame(nil)
    end
end

function PrivateRecordLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
        -- 设置回调
        PriRoom:getInstance():setViewFrame(self)
    end
end

-- 数据回调
function PrivateRecordLayer:onReloadRecordList()
    PriRoom:getInstance():setViewFrame(nil)
    local count = #(PriRoom:getInstance().m_tabCreateRecord)
    if self.m_nFrom == FROM_ENUM.FROM_BACKGAME then
        local list = PriRoom:getInstance().m_tabCreateRecord
        -- 过滤已结束
        local playingRoomList = {}
        for k,v in pairs(list) do
            if v.cbIsDisssumRoom ~= PriRoom.ROOM_STATE.END_STATE then
                table.insert(playingRoomList, v)
            end
        end
        count = #playingRoomList
        self.m_tabList = playingRoomList

        -- 通知大厅(返回游戏按钮刷新)
        if nil ~= self._scene and nil ~= self._scene.onReloadRecordList then
            self._scene:onReloadRecordList()
        end
    else
        self.m_tabList = PriRoom:getInstance().m_tabCreateRecord
    end
    
    self.m_nListCount = math.floor( count/2 )
    if count > 0 then
        if math.mod(count ,2) ~= 0 then
            self.m_nListCount = self.m_nListCount + 1
        end
    end
    self:onGetShowRoomList()
    self._listView:reloadData()
end


function PrivateRecordLayer:tableCellTouched(view, cell)

end

-- 子视图大小
function PrivateRecordLayer:cellSizeForTable(view, idx)
    return view:getViewSize().width, self.m_nCellSize
end

function PrivateRecordLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    idx = self.m_nShowListCount - (idx + 1)
    idx = idx * 2 + 1
    -- tagPersonalRoomInfo
    -- 第一个cell
    local iteminfo = self.m_showList[idx]
    if nil ~= iteminfo then
        local csb, isEnd = self:createRecordItem(iteminfo, cell)
        local imagecell = csb:getChildByName("mark_sp_cellbg")
        local imagesize = imagecell:getContentSize()
        csb:setPosition(view:getViewSize().width * 0.5 - imagesize.width * 0.5 - 30, self.m_nCellSize * 0.5)

        imagecell:setSwallowTouches(false)
        imagecell.info = iteminfo
        if isEnd then
            imagecell:loadTexture("video/mark_sp_cellbg_3.png")
        else
            imagecell:addTouchEventListener(self.m_itemFunC)
        end
    end

    -- 第二个cell
    iteminfo = self.m_showList[idx + 1]
    if nil ~= iteminfo then
        local csb, isEnd = self:createRecordItem(iteminfo, cell)
        local imagecell = csb:getChildByName("mark_sp_cellbg")
        local imagesize = imagecell:getContentSize()
        csb:setPosition(view:getViewSize().width * 0.5 + imagesize.width * 0.5 + 30, self.m_nCellSize * 0.5)

        imagecell:setSwallowTouches(false)
        imagecell.info = iteminfo
        if isEnd then
            imagecell:loadTexture("video/mark_sp_cellbg_3.png")
        else
            imagecell:addTouchEventListener(self.m_itemFunC)
        end
    end

    return cell
end

-- 子视图数目
function PrivateRecordLayer:numberOfCellsInTableView(view)
    return self.m_nShowListCount
end

function PrivateRecordLayer:scrollViewDidScroll(view)

end

function PrivateRecordLayer:createRecordItem( tabData, parent )
    local csb = ExternalFun.loadCSB("private/PrivateRecordNode.csb", parent)

    -- 房号
    local txtID = csb:getChildByName("txt_roomid")
    txtID:setString(string.format("%06d", tabData.dwPersonalRoomID))

    -- 创建时间
    local tabTime = tabData.sysCreateTime
    -- 日期 
    -- local txtData = csb:getChildByName("txt_date")
    -- txtData:setString(string.format("%d-%02d-%02d", tabTime.wYear, tabTime.wMonth, tabTime.wDay))
    -- 时间
    local txtTime = csb:getChildByName("txt_time")
    txtTime:setString(string.format("%02d.%02d %02d:%02d", tabTime.wMonth, tabTime.wDay, tabTime.wHour, tabTime.wMinute))

    -- 人数
    local joinPlayer = tabData.wCurSitCount
    local maxPlayer = tabData.lPlayerCount or 0
    local txtjoinPlayer = csb:getChildByName("txt_count")
    txtjoinPlayer:setString(joinPlayer.."/"..maxPlayer)
    -- local txtmaxPlayer = csb:getChildByName("txt_count_1")
    -- txtmaxPlayer:setString("/" .. maxPlayer)

    -- 状态
    local spstatus = csb:getChildByName("pri_sp_state")
    local spfile = ""
    if tabData.cbIsDisssumRoom == PriRoom.ROOM_STATE.FREE_STATE then
        -- 等待加入
        --txtstatus:setString("等待加入...")
        spfile = "private/pri_sp_state_free.png"
        --txtstatus:setTextColor(cc.c3b(170, 255, 60))
    elseif tabData.cbIsDisssumRoom == PriRoom.ROOM_STATE.PLAYING_STATE then
        -- 游戏中
        --txtstatus:setString("进行中...")
        spfile = "private/pri_sp_state_play.png"

    elseif tabData.cbIsDisssumRoom == PriRoom.ROOM_STATE.END_STATE then
        -- 已结束
        --txtstatus:setString("已结束")
        spfile = "private/pri_sp_state_end.png"

        local sprIcon = "private/pri_sp_timeicon.png"
        local sprTimeIcon = csb:getChildByName("mark_sp_timeIcon")
        --设置纹理
        local spriteValue = cc.Sprite:create(sprIcon)
        if nil ~= sprTimeIcon then
            sprTimeIcon:setSpriteFrame(spriteValue:getSpriteFrame())
        end

        csb:getChildByName("txt_count"):setString("")
        csb:getChildByName("pri_icon_playerNum_2"):setVisible(false) --隐藏人数
        

        csb:getChildByName("txt_roomid_tit"):setTextColor(cc.c3b(0xae, 0xab, 0xbc))
        txtID:setTextColor(cc.c3b(0xc4, 0xc0, 0xd8))
        txtTime:setTextColor(cc.c3b(0xae, 0xab, 0xbc))

        local txtGameName = csb:getChildByName("txt_gameName")
        txtGameName:setTextColor(cc.c3b(0xc0, 0xc5, 0xca))
    else
        txtstatus:setString("")
    end
    --设置状态纹路
    local spriteState = cc.Sprite:create(spfile)
    if nil ~= spstatus then
        spstatus:setSpriteFrame(spriteState:getSpriteFrame())
    end

    --游戏名字
    local txtGameName = csb:getChildByName("txt_gameName")
    local videogame = self._scene:getGameInfo(tabData.dwGameKindID)
    print("@@@@@@@@@@@@@@@@@@@@@@", tabData.dwGameKindID)
    dump(videogame, "@@@@@@@@@@@@@@@@@@@@", 6)
    if nil ~= videogame then
        txtGameName:setString(videogame._GameName)
    end
    -- -- 角标
    -- local file = "gameicon/gamemark_" .. tabData.dwGameKindID .. ".png"
    -- if cc.FileUtils:getInstance():isFileExist(file) then
    --     csb:getChildByName("sp_gamemark"):setSpriteFrame(cc.Sprite:create(file):getSpriteFrame())
    -- else
    --     csb:getChildByName("sp_gamemark"):setSpriteFrame(cc.Sprite:create("public/public_blank.png"):getSpriteFrame())
    -- end

    return csb, tabData.cbIsDisssumRoom == PriRoom.ROOM_STATE.END_STATE
end

--筛选用于显示的房间
function PrivateRecordLayer:onGetShowRoomList()
    self.m_showList = {}
    if self.boxRoomDaiKai:isSelected() then --如果是代开
        for i=1, #self.m_tabList do
            local iteminfo = self.m_tabList[i]
            --判断我是否参与了游戏
            local isMyJoin = false
            for j=1, 8 do
                local userInfo = iteminfo.PersonalUserScoreInfo[1][j]
                if nil ~= userInfo then
                    if userInfo.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
                        isMyJoin = true
                        break
                    end
                end
            end
            if not isMyJoin then
                table.insert(self.m_showList, iteminfo)
            end
        end
    else
        for i=1, #self.m_tabList do
            local iteminfo = self.m_tabList[i]
            for j=1, 8 do
                local userInfo = iteminfo.PersonalUserScoreInfo[1][j]
                if nil ~= userInfo then
                    if userInfo.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
                        table.insert(self.m_showList, iteminfo)
                        break
                    end
                end
            end
        end
    end

    local jsonStr = cjson.encode(self.m_showList)
    LogAsset:getInstance():logData(jsonStr, true)

    local count = #self.m_showList
    self.m_nShowListCount = math.floor( count/2 )
    if count > 0 then
        if math.mod(count ,2) ~= 0 then
            self.m_nShowListCount = self.m_nShowListCount + 1
        end

        self.tipsTxt:setVisible(false)
    else
        self.tipsTxt:setVisible(true)
    end
    print("######用于显示的列表个数", self.m_nShowListCount)
end
--房间类型切换函数
function PrivateRecordLayer:onBoxSelectedEvent( tag, sender )
    self.boxRoomDaiKai:setSelected(tag == TAG_ENUM.BOX_ROOMDAIKAI)
    self.boxRoomMy:setSelected(tag == TAG_ENUM.BOX_ROOMMY)
    self:onGetShowRoomList()
    self._listView:reloadData()

    if tag == TAG_ENUM.BOX_ROOMDAIKAI then 
        self.boxSelect:setPositionX(382)
    elseif tag == TAG_ENUM.BOX_ROOMMY then
        self.boxSelect:setPositionX(562)
    end

end

return PrivateRecordLayer