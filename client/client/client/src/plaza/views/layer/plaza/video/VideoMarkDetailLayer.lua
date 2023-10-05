--
-- Author: zhong
-- Date: 2017-05-26 17:29:10
--
-- 战绩详情界面(三级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local VideoMarkDetailLayer = class("VideoMarkDetailLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local X_OFFSET = 120        -- 除去 总分、局数 的x位置偏移
local INFO_WIDTH = 550      -- 玩家昵称、分数、总分信息最大宽度

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩 
    "BTN_SEARCH",           -- 搜索
    "BTN_DETAIL",           -- 详情
    "BTN_REPLAY",           -- 回放
    "BTN_SHARE",            -- 分享
    "QUERY_VIDEO_LIST",     -- 查询列表
    "QUERY_VIDEO_DETAIL",   -- 查询详情
    "FROM_MARKLIST",        -- 从战绩列表来
    "FROM_PRIVATE",         -- 从约战列表来
    "FROM_BACKCODE",        -- 从回放码来
    "FROM_GOLDLIST",        -- 从金币约战列表来
    "QUERY_VIDEO_USERINFO", -- 查询用户信息，金币房卡使用
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
VideoMarkDetailLayer.FROM_MARKLIST = TAG_ENUM.FROM_MARKLIST
VideoMarkDetailLayer.FROM_PRIVATE = TAG_ENUM.FROM_PRIVATE
VideoMarkDetailLayer.FROM_BACKCODE = TAG_ENUM.FROM_BACKCODE
VideoMarkDetailLayer.FROM_GOLDLIST = TAG_ENUM.FROM_GOLDLIST

-- 房主标识(4人)
--local tabRoomerPosition = {{130, 25}, {305, 25}, {475, 25}, {645, 25}}
-- 房主标识(6人)
local tabRoomerPosition = {{180, 30}, {310, 30}, {440, 30}, {570, 30}, {700, 30}, {830, 30}}
-- 昵称位置(4人)
--local tabNickPosition = {{195, 25}, {365, 25}, {535, 25}, {705, 25}}
-- 昵称位置(6人)
local tabNickPosition = {{180, 30}, {310, 30}, {440, 30}, {570, 30}, {700, 30}, {830, 30}}


-- @param[scene] Client场景
-- @param[param] 详情参数(战绩-详情: param.list={CMD_MB_S_QueryVideoInfoResult}数组; 约战房间-详情: param.list=tagPersonalRoomInfo)
function VideoMarkDetailLayer:ctor( scene, param, level )
    VideoMarkDetailLayer.super.ctor( self, scene, param, level )

    self.m_replayScript = nil
    self.m_videoGame = nil
    -- 加载csb资源
    --local rootLayer, csbNode = ExternalFun.loadRootCSB("video/VideoMarkDetailLayer.csb", self)
    local rootLayer, csbNode = ExternalFun.loadRootCSB("video/VideoMarkDetailLayer_6.csb", self)

    -- 按钮事件
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

    self.m_nFrom = self._param.nFrom

    --积分房卡
    self._nodeScore = spbg:getChildByName("node_score")
    self._nodeScore:setVisible(TAG_ENUM.FROM_GOLDLIST ~=  self.m_nFrom)
    --金币房卡
    self._nodeGold = spbg:getChildByName("node_gold")
    self._nodeGold:setVisible(TAG_ENUM.FROM_GOLDLIST == self.m_nFrom)


    self.m_layoutContent = self._nodeScore:getChildByName("content_2")
    self.m_contentSize = self.m_layoutContent:getContentSize()
    self.m_nRowCount = 0
    -- 列宽
    self.m_nRowWidth = 0
    -- 玩家昵称
    local layNick = self._nodeScore:getChildByName("content_1")
    self.m_layoutNick = layNick

    -- 总成绩
    --self.m_txtTotalScore = self.m_layoutContent:getChildByName("txt_total")
    --self.m_fTotalXPos = 53--self.m_txtTotalScore:getPositionX() -- 4人
    self.m_fTotalXPos = 56--self.m_txtTotalScore:getPositionX() -- 6人

    -- 分享
    self.m_btnShare = self.m_layoutContent:getChildByName("btn_share")
    self.m_btnShare:setTag(TAG_ENUM.BTN_SHARE)
    self.m_btnShare:addTouchEventListener( touchFunC )
    self.m_btnShare:setVisible(false)

    -- 操作
    --self.m_txtAction = self.m_layoutNick:getChildByName("txt_action")
    --self.m_fActionXPos = 913--self.m_txtAction:getPositionX() -- 4人
    self.m_fActionXPos = 955--self.m_txtAction:getPositionX() -- 6人

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 房号
    self.m_txtRoomId = spbg:getChildByName("txt_roomid")

    -- 日期
    --self.m_txtDate = spbg:getChildByName("txt_date")

    -- 时间
    self.m_txtTime = spbg:getChildByName("txt_time")

    -- 约战房唯一id
    self.szPersonalGUID = ""
    -- 视频类型
    self.m_nVideoType = GameVideo.DEFINE.QUERY_NORMAL_VIDEO

    -- 房主
    local tmp = self._nodeScore:getChildByName("txt_roomer")
    self.m_clipRoomer = ClipText:createClipText(tmp:getContentSize(), "", nil, 22)
    self.m_clipRoomer:setAnchorPoint(tmp:getAnchorPoint())
    self.m_clipRoomer:setPosition(tmp:getPosition())
    self.m_clipRoomer:setTextColor(cc.c4b(0xad, 0xa7, 0xfc, 255))
    self._nodeScore:addChild(self.m_clipRoomer)
    tmp:removeFromParent()
    self.m_dwRoomerUid = nil

    -- 用户列表
    self.m_tabItemList = {}
    -- 视频列表
    self.m_tabListDetail = {}
    -- 查询类型
    self.m_nQueryType = nil
    self.m_dwRoomID = 0
    -- 记录当前自己视频列表页
    self.m_nCurPageIdx = GameVideo:getInstance().m_nPageIndex
    -- 回放主视图ID
    self.m_dwPlayBackUserID = 0



    if self.m_nFrom == TAG_ENUM.FROM_PRIVATE
        or self.m_nFrom == TAG_ENUM.FROM_BACKCODE then
        self.m_nQueryType = TAG_ENUM.QUERY_VIDEO_LIST

        self.m_tabListDetail = GameVideo:getInstance():getVideoDetailInfo()
    elseif self.m_nFrom == TAG_ENUM.FROM_MARKLIST then
        self.m_nQueryType = TAG_ENUM.QUERY_VIDEO_DETAIL

        -- 用户列表
        self.m_tabItemList = self._param.list
        self:initVideoDetail()
    elseif self.m_nFrom == TAG_ENUM.FROM_GOLDLIST then
        self.m_nQueryType = TAG_ENUM.QUERY_VIDEO_USERINFO
         -- 房号
        self.m_txtRoomId:setString(self._param.list.roomID .. "")
        self.m_dwRoomID = self._param.list.roomID

        --解析时间
        function getTimeDes(strTime)
            --针对标准时间特殊处理
            local tabTime = {}
            tabTime._year = string.sub(strTime, 1, 4)
            tabTime._mouth = string.sub(strTime, 6, 7)
            tabTime._day = string.sub(strTime, 9, 10)
            tabTime._hour = string.sub(strTime, 12, 13)
            tabTime._min = string.sub(strTime, 15, 16)
            tabTime._second = string.sub(strTime, 18, 19)

            return tabTime
        end
        -- 时间
        local tabTime = getTimeDes(self._param.list.playTime)
        self.m_txtTime:setString(tabTime._mouth .. "." .. tabTime._day .. " " .. tabTime._hour .. ":" .. tabTime._min)
  

        --dump(self._param.list.DetailsList, "aaaaaaaaaaaaaaaaaaaaa", 7)
        self.m_tabGoldListDetail = {}
        for i=1, #self._param.list.DetailsList do
            --只显示自己的战绩
            local DetailsListInfo = self._param.list.DetailsList[i]
            local battleInfo = DetailsListInfo.BattleUser
            for j=1, #battleInfo do
                if battleInfo[j].GameID == GlobalUserItem.tabAccountInfo.dwGameID then
                    table.insert(self.m_tabGoldListDetail, DetailsListInfo)
                    break
                end
            end
        end

        -- 房主ID
        self.m_dwRoomerUid = self._param.list.owenerUserID
        -- 约战房唯一id
        self.szPersonalGUID = self._param.list.szPersonalGUID
        -- 游戏标识
        self.m_nKindID = self._param.list.KindID
        -- 视频类型
        self.m_nVideoType = GameVideo.DEFINE.QUERY_PRIVATE_VIDEO
        -- 回放主视图
        self.m_dwPlayBackUserID = GlobalUserItem.tabAccountInfo.dwUserID
    end
    
    -- 按钮
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
    self.m_nListCount = 0
    self._listView = nil
    -- 信息列表
    if TAG_ENUM.FROM_GOLDLIST ~= self.m_nFrom then
        local tmp = self._nodeScore:getChildByName("content")
        self.m_fCount = tmp:getContentSize().height / 6
        self._listView = cc.TableView:create(tmp:getContentSize())
        self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
        self._listView:setPosition(tmp:getPosition())
        self._listView:setDelegate()
        self._listView:addTo(self._nodeScore)
        self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
        self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
        self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
        tmp:removeFromParent()
        self._listView:reloadData()
    else
        local tmp = self._nodeGold:getChildByName("content")
        self._listView = cc.TableView:create(tmp:getContentSize())
        self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)  
        self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)    
        self._listView:setPosition(tmp:getPosition())
        self._listView:setDelegate()
        self._listView:addTo(self._nodeGold)
        self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
        self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
        self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
        tmp:removeFromParent()
        self._listView:reloadData()
    end

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function VideoMarkDetailLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        GameVideo:getInstance():getNetFrame():onCloseSocket()
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_SHARE == tag then
        if nil ~= self.szPersonalGUID or "" ~= self.szPersonalGUID then
            -- 分享
            GameVideo:getInstance():showPopWait()
            GameVideo:getInstance():getNetFrame():onRequestPlayBackCode_YZ(self.szPersonalGUID)
        end
    end
end

-- 按键监听
function VideoMarkDetailLayer:onItemButtonClickedEvent(tag, sender)
    local info = sender.info -- CMD_MB_S_QueryVideoDetailsResult
    if type(info) ~= "table" then
        dump(info, "VideoMarkDetailLayer:onItemButtonClickedEvent 参数非法", 6)
        return
    end
    --dump(info, "info", 6)
    --dump(self._param, "self._param", 6)
    if TAG_ENUM.BTN_DETAIL == tag then
        local fun = function()
            local bPrivate = (self.m_nVideoType ==  GameVideo.DEFINE.QUERY_PRIVATE_VIDEO)
            GameVideo:getInstance():getVideoNetMessage(info.szVideoNumber, bPrivate, function(szVideoNumber, bPrivate, cmdlist)
                print("videonumber bPrivate ", szVideoNumber, bPrivate)
                --logFunc(cmdlist, true)
                if nil ~= self.m_replayScript then
                    local resultLayer = self.m_replayScript:getVideoResultView(cmdlist)
                    if nil ~= resultLayer and type(resultLayer) == "userdata" then
                        resultLayer:setLevel(self:getLevel() + 1)
                        self._scene:addPopLayer(resultLayer)
                        return
                    end
                end
                showToast(self, "无战绩详情!", 3)
            end)
        end
        -- 游戏版本检查
        if nil ~= self._scene and nil ~= self._scene.getApp then
            if cc.PLATFORM_OS_WINDOWS == targetPlatform and yl.DISABLE_WIN_UPDATE then
                print("win32 跳过更新")
            elseif nil ~= self.m_videoGame then
                local app = self._scene:getApp()
                local version = tonumber(app:getVersionMgr():getResVersion(self.m_videoGame._KindID))
                if not version or self.m_videoGame._ServerResVersion > version then
                    if false == self._scene:singleGameUpdate(self.m_videoGame._KindID, function()
                            fun()
                        end) then
                        if nil ~= self.m_videoGame then
                            local gamename = self.m_videoGame._GameName or ""
                            local msg = string.format("请更新 %s 游戏资源!", gamename)
                            showToast(self._scene, msg, 2)
                        else
                            showToast(self._scene, "请更新游戏资源!", 2)
                        end
                    end
                    self:dismissPopWait()
                    return
                end
            end
        else
            showToast(self._scene, "游戏版本异常!", 2)
            return
        end
        fun()
    elseif TAG_ENUM.BTN_REPLAY == tag then
        local tabGameInfo = {}
        tabGameInfo.nKindID = self.m_nKindID
        tabGameInfo.szPersonalGUID = info.szPersonalGUID
        -- 视频类型
        tabGameInfo.bPrivateGame = (self.m_nVideoType ==  GameVideo.DEFINE.QUERY_PRIVATE_VIDEO)
        tabGameInfo.dwPersonalRoomID = self.m_dwRoomID
        tabGameInfo.dwMainChairUserID = self.m_dwPlayBackUserID
        tabGameInfo.dwTableOwnerUserID = self.m_dwRoomerUid

        
        --dump(self.m_tabItemList, "回放信息2", 3)
        print("回放信息3=================>", info.szVideoNumber, info.GamesNum)
        --筛选玩家回放id
        local itemList = {}
        for i=1, #self.m_tabItemList do
            local infoItem = self.m_tabItemList[i]
            if TAG_ENUM.FROM_GOLDLIST ~= self.m_nFrom then
                table.insert(itemList, infoItem)
            else
                if infoItem.nGameNum == info.GamesNum then
                    table.insert(itemList, infoItem)
                end
            end
        end
        dump(itemList, "回放玩家信息---------》", 7)
        --assert(false)

        GameVideo:getInstance():downloadRecordFileList({ {szVideoNumber = info.szVideoNumber, nRound = 1} }, itemList, tabGameInfo)
    end
end

function VideoMarkDetailLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function VideoMarkDetailLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function VideoMarkDetailLayer:onTransitionInFinish()
    if TAG_ENUM.FROM_GOLDLIST ~= self.m_nFrom then
        if 0 == #self.m_tabListDetail.tabVecRoundInfo then
            -- 加载
            GameVideo:getInstance():showPopWait()
            GameVideo:getInstance():setViewFrame(self)
            if self.m_nFrom == TAG_ENUM.FROM_PRIVATE then
                GameVideo:getInstance().m_nPageIndex = 1
                GameVideo:getInstance().m_tabVecVideoList = {}
                -- 加载列表(通过房间ID加载视频列表)
                GameVideo:getInstance():getNetFrame():onLoadVideoList(0, 1, 0, 0, self._param.dwPersonalRoomID)
            elseif self.m_nFrom == TAG_ENUM.FROM_MARKLIST then
                print("onTransitionInFinish", self.szPersonalGUID)
                GameVideo:getInstance():getNetFrame():onQueryVideoDetail(self.szPersonalGUID)
            elseif self.m_nFrom == TAG_ENUM.FROM_BACKCODE then
                GameVideo:getInstance().m_nPageIndex = 1
                GameVideo:getInstance().m_tabVecVideoList = {}
                -- 加载列表(通过回放码加载视频列表)
                GameVideo:getInstance():getNetFrame():onLoadVideoList(0, 1, self._param.nBackCode, 2, "")
            elseif self.m_nFrom == TAG_ENUM.FROM_GOLDLIST then
                --请求回放码
                -- GameVideo:getInstance():showPopWait()
                -- GameVideo:getInstance():setViewFrame(self)
                -- GameVideo:getInstance():getNetFrame():onRequestPlayBackCode_YZ(self.szPersonalGUID)
    
            end
        else
            self:onRefreshList()
        end
    else
        GameVideo:getInstance():showPopWait()
        GameVideo:getInstance():setViewFrame(self)

        GameVideo:getInstance():changeQueryType(3)
        GameVideo:getInstance():queryPageVideoInfoEX(0, 3, self.szPersonalGUID)
        --GameVideo:getInstance():getNetFrame():onQueryVideoDetail(self.szPersonalGUID)
        print("onTransitionInFinish=======================>", self.szPersonalGUID)
    end
end

function VideoMarkDetailLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function VideoMarkDetailLayer:onTransitionOutFinish()
    -- 重置当前自己视频页
    GameVideo:getInstance().m_nPageIndex = self.m_nCurPageIdx
    self:removeFromParent()
end

function VideoMarkDetailLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function VideoMarkDetailLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end

function VideoMarkDetailLayer:initVideoDetail()
    print("初始化战绩信息!!!!", #self.m_tabItemList)
    -- 提取数据
    dump(self.m_tabItemList, "aaaaaaaaaaaaaaa", 10)
    local roomerinfo = nil
    for k,v in pairs(self.m_tabItemList) do
        if v.iVideoType ~=1 then
            if v.cbCreateRoom == 1 then
                roomerinfo = v
                break
            end
        end
    end
    if nil ~= roomerinfo then
        -- 房号
        self.m_txtRoomId:setString(roomerinfo.dwPersonalRoomID .. "")
        self.m_dwRoomID = roomerinfo.dwPersonalRoomID

        local createDate = roomerinfo.CreateDate
        -- 日期
        -- local szdate = string.format("%d-%02d-%02d", createDate.wYear, createDate.wMonth, createDate.wDay)
        -- self.m_txtDate:setString(szdate)

        -- 时间
        local sztime = string.format("%02d.%02d %02d:%02d",createDate.wMonth, createDate.wDay, createDate.wHour, createDate.wMinute)
        self.m_txtTime:setString(sztime)

        dump(roomerinfo,"=====================9999999999")
        -- 房主
        self.m_clipRoomer:setString(roomerinfo.szNickName)
        -- 房主ID
        self.m_dwRoomerUid = roomerinfo.dwUserID
        -- 约战房唯一id
        self.szPersonalGUID = roomerinfo.szPersonalGUID
        -- 游戏标识
        self.m_nKindID = roomerinfo.wKindID
        -- 视频类型
        self.m_nVideoType = roomerinfo.iVideoType
        -- 回放主视图
        self.m_dwPlayBackUserID = roomerinfo.dwPlayBackUserID

        -- 更新当前游戏
        GlobalUserItem.nCurGameKind = roomerinfo.wKindID
        self._scene:updateCurrentGame()
        local replay, videogame = GameVideo:getInstance():getReplayScriptFile()
        self.m_replayScript = replay
        self.m_videoGame = videogame
    end
    self.m_tabListDetail = GameVideo:getInstance():getVideoDetailInfo(self.szPersonalGUID)
    dump(self.m_tabListDetail, "视频详情", 6)

    if self.m_nFrom == TAG_ENUM.FROM_MARKLIST 
        or self.m_nFrom == TAG_ENUM.FROM_PRIVATE then
        -- 战绩列表/房间列表 主视图为自己且可以分享
        self.m_dwPlayBackUserID = GlobalUserItem.tabAccountInfo.dwUserID

        self.m_btnShare:setVisible(true)
    end
end

-- 子视图大小
function VideoMarkDetailLayer:cellSizeForTable(view, idx)
    if TAG_ENUM.FROM_GOLDLIST ~= self.m_nFrom then
        return view:getViewSize().width, 60
    else
        return view:getViewSize().width, 120
    end
end

function VideoMarkDetailLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    if TAG_ENUM.FROM_GOLDLIST ~= self.m_nFrom then
        local iteminfo = self.m_tabListDetail.tabVecRoundInfo[idx + 1]
        if nil == iteminfo then
            return cell
        end

        -- 背景
        --local imagecell = ccui.ImageView:create("video/mark_sp_detail_itembg.png") -- 4人
        local imagecell = ccui.ImageView:create("video/mark_sp_detail_itemline.png") -- 6人
        --imagecell:setScale9Enabled(true)
        --imagecell:setContentSize(cc.size(1100, 58))
        local imagesize = cc.size(1000, 60)
        imagecell:setPosition(view:getViewSize().width * 0.5, 1)
        cell:addChild(imagecell)

        -- 局数
        local szround = string.format("%d/%d", iteminfo.nRound, self.m_nListCount)
        local txtround = cc.Label:createWithTTF(szround,"fonts/FZHTJW.TTF",30)
        txtround:setPosition(cc.p(self.m_fTotalXPos, imagesize.height * 0.5))
        imagecell:addChild(txtround)
        txtround:setColor(cc.c4b(0xac,0xa8,0xd6,255))

        -- 分数
        for i = 1, self.m_nRowCount do
            local uid = self.m_tabListDetail.tabArrangeRule[i]
            local item = iteminfo.list[uid] or {}
            local score = item.lScore or 0
            local szscore = ""
            local ccolor = cc.c4b(0x15, 0xf0, 0x39,255)
            if score > 0 then
                ccolor = cc.c4b(0xdf, 0xc4, 0x90, 255)
                szscore = "+" .. ExternalFun.formatScoreText(score)
            else
                ccolor = cc.c4b(0xf0, 0x51, 0x67, 255)
                szscore = ExternalFun.formatScoreText(score)
            end

            local txtscore = cc.Label:createWithTTF(szscore,"fonts/FZHTJW.TTF",30)
            txtscore:setVisible(false)
            local pos = tabNickPosition[i]
            if nil ~= pos then
                txtscore:setVisible(true)
                txtscore:setPosition(pos[1], imagesize.height * 0.5)
            end
            imagecell:addChild(txtscore)
            txtscore:setColor(ccolor)
        end

        local enableDetail = false
        -- 判断是否支持详情显示
        if nil ~= self.m_replayScript and nil ~= self.m_replayScript.enableResultView and true == self.m_replayScript:enableResultView() then
            -- 详情
            local btndetail = ccui.Button:create("video/mark_btn_detail_0.png","video/mark_btn_detail_0.png", "video/mark_btn_detail_0.png")
            imagecell:addChild(btndetail)
            btndetail:setPosition(self.m_fActionXPos - 40, imagesize.height * 0.5)
            btndetail:setTag(TAG_ENUM.BTN_DETAIL)
            btndetail.info = iteminfo
            btndetail:addTouchEventListener(self.m_itemFunC)
            enableDetail = true
        end

        -- 回放
        local btnreplay = ccui.Button:create("video/mark_btn_detail_rp_0.png","video/mark_btn_detail_rp_0.png", "video/mark_btn_detail_rp_0.png")
        imagecell:addChild(btnreplay)
        if enableDetail then
            btnreplay:setPosition(self.m_fActionXPos + 40, imagesize.height * 0.5)
        else
            btnreplay:setPosition(self.m_fActionXPos, imagesize.height * 0.5)
        end
        btnreplay:setTag(TAG_ENUM.BTN_REPLAY)
        btnreplay.info = iteminfo
        btnreplay:addTouchEventListener(self.m_itemFunC)
    else
        local iteminfo = self.m_tabGoldListDetail[idx + 1]
        dump(iteminfo, "金币约战信息", 5)
        if nil == iteminfo then
            return cell
        end

        -- 背景
        local line1 = ccui.ImageView:create("video/mark_sp_detail_itemline.png") -- 6人
        line1:setPosition(view:getViewSize().width * 0.5, 1)
        cell:addChild(line1)

        local line2 = ccui.ImageView:create("video/mark_sp_detail_itemline.png") -- 6人
        line2:setPosition(view:getViewSize().width * 0.5, 61)
        cell:addChild(line2)

     
        -- 局数
        local txtRound = cc.Label:createWithTTF("局数","fonts/FZHTJW.TTF",24)
        txtRound:setPosition(cc.p(self.m_fTotalXPos, 90))
        cell:addChild(txtRound)
        txtRound:setColor(cc.c4b(0xac,0xa8,0xd6,255))

        local szround = string.format("%d", tonumber(idx + 1))
        local txtround = cc.Label:createWithTTF(szround,"fonts/FZHTJW.TTF",24)
        txtround:setPosition(cc.p(self.m_fTotalXPos, 30))
        cell:addChild(txtround)
        txtround:setColor(cc.c4b(0xac,0xa8,0xd6,255))

        --玩家和分数
        --获取总的参与玩家信息
        local battleInfo = iteminfo.BattleUser
        for i=1,#battleInfo do
            --玩家昵称
            -- 昵称
            if i > 6 then
                break
            end
            print("坐标=========》",tabNickPosition[i][1], i)

            
            local strNickname = string.EllipsisByConfig(battleInfo[i].NickName, 120, 
                                            string.getConfig("fonts/FZHTJW.TTF.ttf", 24))
            local txtNick = cc.Label:createWithTTF(strNickname,"fonts/FZHTJW.TTF",24)
            txtNick:setPosition(cc.p(tabNickPosition[i][1], 90))
            txtNick:setColor(cc.c4b(0xac,0xa8,0xd6,255))
            cell:addChild(txtNick)
            if battleInfo[i].GameID == GlobalUserItem.tabAccountInfo.dwGameID then
                txtNick:setColor(cc.c4b(0x15,0xf0,0x39,255))
            end

            --分数
            local szscore = ""
            local ccolor = cc.c4b(255,255,255,255)
            local score = tonumber(battleInfo[i].Score)
            if score > 0 then
                szscore = "+" .. ExternalFun.formatScoreText(score)
                ccolor = cc.c4b(0xdf, 0xc4, 0x90, 255)
            else
                szscore = ExternalFun.formatScoreText(score)
                ccolor = cc.c4b(0xf0, 0x51, 0x67, 255)
            end
            local txtScore = cc.Label:createWithTTF(szscore,"fonts/FZHTJW.TTF",24)
            txtScore:setPosition(cc.p(tabNickPosition[i][1], 30))
            txtScore:setColor(ccolor)
            cell:addChild(txtScore) 
        end
        -- 回放
        local btnreplay = ccui.Button:create("video/mark_btn_detail_rp_0.png","video/mark_btn_detail_rp_0.png", "video/mark_btn_detail_rp_0.png")
        cell:addChild(btnreplay)
        if enableDetail then
            btnreplay:setPosition(self.m_fActionXPos + 40, 30)
        else
            btnreplay:setPosition(self.m_fActionXPos, 30)
        end
        btnreplay:setTag(TAG_ENUM.BTN_REPLAY)
        iteminfo.szVideoNumber = battleInfo[1].VideoNumber
        btnreplay.info = iteminfo
        btnreplay:addTouchEventListener(self.m_itemFunC)

    end

    return cell
end

-- 子视图数目
function VideoMarkDetailLayer:numberOfCellsInTableView(view)
    if TAG_ENUM.FROM_GOLDLIST ~= self.m_nFrom then
        return self.m_nListCount
    else
        --print("numberOfCellsInTableView", #self.m_tabGoldListDetail)
        return #self.m_tabGoldListDetail
    end
end

-- 列表回调
function VideoMarkDetailLayer:onRefreshList()
    GameVideo:getInstance():dismissPopWait()
    print("===================刷新战绩信息列表======================", self.szPersonalGUID, self.m_nQueryType)
    local unGetInfoFun = function()
        showToast(self._scene, "视频信息未找到, 请重试!", 2)
        GameVideo:getInstance():dismissPopWait()
        GameVideo:getInstance():setViewFrame(nil)
        self:scaleTransitionOut(self.m_spBg) 
    end
    if self.m_nQueryType == TAG_ENUM.QUERY_VIDEO_LIST then  -- 查询列表
        local tabListInfo = GameVideo:getInstance():getVideoListInfo()
        dump(tabListInfo, "战绩信息", 10)
        if 0 == #tabListInfo then
            unGetInfoFun()
            return true
        end
        local iteminfo = tabListInfo[1]
        if 0 == #iteminfo then
            unGetInfoFun()
            return true
        end
        local userinfo = iteminfo[1]
        if nil == userinfo.szPersonalGUID or "" == userinfo.szPersonalGUID then
            unGetInfoFun()
            return true
        end
        self.m_nQueryType = TAG_ENUM.QUERY_VIDEO_DETAIL
        self.szPersonalGUID = userinfo.szPersonalGUID
        self.m_tabItemList = iteminfo
        self.m_nKindID = userinfo.wKindID
        self.m_nVideoType = userinfo.iVideoType
        -- 查询详情
        GameVideo:getInstance():getNetFrame()._szVideoGUID = userinfo.szPersonalGUID
        if nil ~= GameVideo:getInstance().m_tabVideoDetail[userinfo.szPersonalGUID] then
            GameVideo:getInstance().m_tabVideoDetail[userinfo.szPersonalGUID] = nil
        end
        GameVideo:getInstance():getNetFrame():sendGetVideoDetail()

        -- 刷新界面
        self:initVideoDetail()

        return false
    elseif self.m_nQueryType == TAG_ENUM.QUERY_VIDEO_DETAIL then -- 查询详情
        GameVideo:getInstance():dismissPopWait()
        GameVideo:getInstance():setViewFrame(nil)
        self.m_tabListDetail = GameVideo:getInstance():getVideoDetailInfo(self.szPersonalGUID)

        self.m_nListCount = #self.m_tabListDetail.tabVecRoundInfo
        self.m_nRowCount = #self.m_tabListDetail.tabArrangeRule
        self.m_nRowWidth = INFO_WIDTH / self.m_nRowCount
        -- 刷新列表
        self._listView:reloadData()

        dump(self.m_tabListDetail, "战绩信息", 10)
        local maxscore = 0
        local maxIndex = 1

        -- 昵称、总成绩
        for i = 1, self.m_nRowCount do
            local uid = self.m_tabListDetail.tabArrangeRule[i]
            local sznick = self.m_tabListDetail.tabTotalInfo[uid].szNickName or ""
            -- 昵称
            local strNickname = string.EllipsisByConfig(sznick, 120, 
                                            string.getConfig("fonts/FZHTJW.TTF.ttf", 24))
            local clipnick = cc.Label:createWithTTF(strNickname,"fonts/FZHTJW.TTF",24)
            clipnick:setVisible(false)
            clipnick:setAnchorPoint(cc.p(0.5, 0.5))
            clipnick:setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
            clipnick:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
            clipnick:setColor(cc.c4b(0xac,0xa8,0xd6,255))

            local pos = tabNickPosition[i]
            if nil ~= pos then
                clipnick:setVisible(true)
                clipnick:setPosition(pos[1], pos[2])
            end
            self.m_layoutNick:addChild(clipnick)
            local bRoomer = (uid == self.m_dwRoomerUid)
            
            -- 分数
            local score = self.m_tabListDetail.tabTotalInfo[uid].lRoundTotalScore or 0
            local szscore = ""
            local ccolor = cc.c4b(255,255,255,255)
            if score > 0 then
                szscore = "+" .. ExternalFun.formatScoreText(score)
                ccolor = cc.c4b(255, 248, 69, 255)
                if maxscore < score then
                    maxscore = score
                    maxIndex = i
                end
                -- -- 赢家标识
                -- local spwinner = self.m_layoutNick:getChildByName("sp_winnerflag_" .. i)
                -- if nil ~= spwinner  then
                --     spwinner:setVisible(true)
           
                -- end
            else
                szscore = ExternalFun.formatScoreText(score)
                ccolor = cc.c4b(147, 69, 52, 255)
            end
            local txtscore = self.m_layoutContent:getChildByName("txt_score_" .. i)
            if nil ~= txtscore then
                txtscore:setString(szscore)

                -- 房主
                if bRoomer then
                    txtscore:setColor(ccolor)
                    self.m_dwRoomerUid = uid

                    -- 房主标识
                    local sproomer = self.m_layoutNick:getChildByName("sp_roomerflag_" .. i)
                    if nil ~= sproomer then
                        sproomer:setVisible(true)
                    end
                end
            end
        end
        -- -- 赢家标识
         local spwinner = self.m_layoutNick:getChildByName("sp_winnerflag_" .. maxIndex)
         if nil ~= spwinner  then
             spwinner:setVisible(true)
         end

        return true
    elseif self.m_nQueryType == TAG_ENUM.QUERY_VIDEO_USERINFO then -- 查询详情
        GameVideo:getInstance():dismissPopWait()
        GameVideo:getInstance():setViewFrame(nil)
        local tabListInfo = GameVideo:getInstance():getVideoListInfo()
        dump(tabListInfo, "战绩信息 QUERY_VIDEO_USERINFO", 10)
        if 0 == #tabListInfo then
            unGetInfoFun()
            return true
        end
        local iteminfo = tabListInfo[1]
        if 0 == #iteminfo then
            unGetInfoFun()
            return true
        end
        local userinfo = iteminfo[1]
        if nil == userinfo.szPersonalGUID or "" == userinfo.szPersonalGUID then
            unGetInfoFun()
            return true
        end
        self.m_nQueryType = TAG_ENUM.QUERY_VIDEO_USERINFO
        self.szPersonalGUID = userinfo.szPersonalGUID
        self.m_tabItemList = iteminfo
        self.m_nKindID = userinfo.wKindID
        self.m_nVideoType = userinfo.iVideoType
    else
        showToast(self._scene, "未知查询类型", 2)
        GameVideo:getInstance():dismissPopWait()
        GameVideo:getInstance():setViewFrame(nil)
        -- self:scaleTransitionOut(self.m_spBg)
        return true
    end
end



return VideoMarkDetailLayer