--
-- Author: zhong
-- Date: 2017-10-11 18:10:28
--
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local RoomLayer = class("RoomLayer", TransitionLayer)
 local LogonFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LogonFrame")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local PrivateIdInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateIdInputLayer")   -- 约战ID
local PlazaIntroductionLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "PlazaIntroductionLayer")   -- 玩法
local ShopLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ShopLayer")                               -- 商城
local PlazaNoticeLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "PlazaNoticeLayer")                 -- 大厅消息列表
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local SpineObj = appdf.req("client.src.external.SpineObj")

local targetPlatform = cc.Application:getInstance():getTargetPlatform()
local TAG_START             = 100
local enumTable = 
{
    "BT_EXIT",              -- 退出
    "BT_MARK",              -- 战绩
    "BT_SPREAD",            -- 推广
    "BT_SET",               -- 设置
    "BT_SHOP",              -- 钻石
    "BT_GOLD", --金币
    "BT_REWARD",--奖券
    "BT_NOTICE",            -- 系统消息
    "BT_SERVICE",           -- 客服
    "BT_ADWARD",            -- 广告/活动
    "BT_FRESHEN",            -- 刷新
    "BT_CREATE",            -- 创建
    "BT_ENTER",            -- 进入
    "BT_FAST",             -- 快速开始
    "BT_FASTNORMAL",        -- 快速开始普通场次
    "BT_HELP",--帮助
    "BT_CK_GOLD", --金币房
    "BT_CK_SCORE",--积分房
    "BT_CK_NORMAL",--金币场
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)


function RoomLayer:ctor(scene, param, level)
    RoomLayer.super.ctor( self, scene, param, level )
    local this = self
    self.m_param = param
    self._scene = scene

    ExternalFun.registerNodeEvent(self)
    local logonCallBack = function (result,message)
        this:onLogonCallBack(result,message)
    end
    self._logonFrame = LogonFrame:create(self,logonCallBack)
    
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/RoomLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    local bgIndex = cc.UserDefault:getInstance():getIntegerForKey("option_imgBG_Index", 1)
    spbg:setBackGroundImage("public/bg"..bgIndex..".png")

    -- 退出
    local btn = spbg:getChildByName("Button_back")
    btn:setTag(TAG_ENUM.BT_EXIT)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)


    self.m_szModuleStr = ""
    -- 房间标题
    local spRoomTitle = spbg:getChildByName("gameName")
    local titlefile = "public/public_blank.png"
    local enterGameInfo = self._scene:getEnterGameInfo()
    self.m_enterGameInfo = enterGameInfo
    if nil ~= enterGameInfo then
        local modulestr = string.gsub(enterGameInfo._KindName, "%.", "/")
        self.m_szModuleStr = modulestr
        titlefile = "game/" .. modulestr .. "res/roomlist/icon_roomlist_title.png"
    end
    if cc.FileUtils:getInstance():isFileExist(titlefile) then
        spRoomTitle:setSpriteFrame(cc.Sprite:create(titlefile):getSpriteFrame())
    else
        spRoomTitle:setVisible(false)
    end

    local helpBtn = spRoomTitle:getChildByName("Button_help")
    helpBtn:setTag(TAG_ENUM.BT_HELP)
    helpBtn:addTouchEventListener(touchFunC)
    helpBtn:setPressedActionEnabled(true)
    helpBtn:setPositionX(spRoomTitle:getContentSize().width)




    -- --快速开始
    -- self.btn_fastCreate = spbg:getChildByName("btn_fastCreate")
    -- self.btn_fastCreate:setTag(TAG_ENUM.BT_CREATE)
    -- self.btn_fastCreate:addTouchEventListener(touchFunC)
    -- self.btn_fastCreate:setPressedActionEnabled(false)
    -- self.btn_fastCreate:setEnabled(false)


    --刷新金币钻石奖券
    self:refreshUserScore()

    --提示
    --self.labNoRoomTips = spbg:getChildByName("txt_tips")
    -- self.listTitle =  spbg:getChildByName("plaza_sp_rommlist_title")

    -- self.sp_tips = spbg:getChildByName("sp_tips")
    

    -- 钻石
    btn = spbg:getChildByName("Node_score"):getChildByName("plaza_image_diamondbg")
    btn:setTag(TAG_ENUM.BT_SHOP)
    btn:addTouchEventListener(touchFunC)

    -- 金币
    btn = spbg:getChildByName("Node_score"):getChildByName("plaza_image_exchange")
    btn:setTag(TAG_ENUM.BT_GOLD)
    btn:addTouchEventListener(touchFunC)

     -- 奖券
    btn = spbg:getChildByName("Node_score"):getChildByName("plaza_image_reward")
    btn:setTag(TAG_ENUM.BT_REWARD)
    btn:addTouchEventListener(touchFunC)

    if self.m_param.clubID ~= nil then
        spbg:getChildByName("Node_score"):setVisible(false)
        spRoomTitle:setAnchorPoint(cc.p(0.5,0.5))
        spRoomTitle:setPositionX(667)
    else
        spRoomTitle:setAnchorPoint(cc.p(0,0.5))
        spRoomTitle:setPositionX(178)
    end





    --模式切换按钮
    local gameRoomlistener = function (sender,eventType)
            self:onGameRoomSelectedEvent(sender:getTag(),sender)
    end

    self.m_roomTypeBtn = {}

        btn = self.m_spBg:getChildByName("CheckBox_normal")
    btn:setTag(TAG_ENUM.BT_CK_NORMAL)
    btn:addEventListener(gameRoomlistener)
    btn:setVisible(false)
    table.insert(self.m_roomTypeBtn,btn)

    
    btn = self.m_spBg:getChildByName("CheckBox_score")
    btn:setTag(TAG_ENUM.BT_CK_SCORE)
    btn:addEventListener(gameRoomlistener)
    btn:setVisible(false)
    table.insert(self.m_roomTypeBtn,btn)

    local btn = self.m_spBg:getChildByName("CheckBox_gold")
    btn:setTag(TAG_ENUM.BT_CK_GOLD)
    btn:addEventListener(gameRoomlistener)
    btn:setVisible(false)
    table.insert(self.m_roomTypeBtn,btn)






    --公告显示
    -- 消息内容
    self.m_tabSystemNotice = {}
    -- 播放索引
    self._sysIndex = 1
    -- 消息是否运行
    self.m_bNotifyRunning = false
    -- 跑马灯内容
    self.m_tabMarquee = {}
    -- 消息id
    self.m_nNotifyId = 0

    self.m_curGameKind = GlobalUserItem.nCurGameKind


            -- 系统消息
    btn = self.m_spBg:getChildByName("Button_notice")
    btn:setTag(TAG_ENUM.BT_NOTICE)
    btn:addTouchEventListener(touchFunC)
    local noticeSize = btn:getContentSize()
    self.m_noticeArea = noticeSize

        -- 消息区域
    local stencil = display.newSprite()
        :setAnchorPoint(cc.p(0,0.5))
    stencil:setTextureRect(cc.rect(0, 0, noticeSize.width - 130, noticeSize.height))
    self._notifyClip = cc.ClippingNode:create(stencil)
        :setAnchorPoint(cc.p(0,0.5))
    self._notifyClip:setInverted(false)
    self._notifyClip:move(80,noticeSize.height * 0.5 + 3)
    self._notifyClip:addTo(btn)
    self._notifyText = cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
                                :addTo(self._notifyClip)
                                --:setTextColor(cc.c4b(255,191,123,255))
                                :setAnchorPoint(cc.p(0,0.5))
                                :enableOutline(cc.c4b(79,48,35,255), 1)

   
    --点击间隔
    self._clickDelay = false

    self.m_tabList = {}
    self.m_nListCount = 0

    --没金币房卡界面
    self._listViewNoPri = nil

    -- 金币房卡列表
    self._listGoldRoomView = nil
    self.m_tabGoldRoomList = {}
    self.m_nGoldRoomListCount = 0

    GlobalUserItem.dwCurRoomGroupID = 0
    ClubFrame:getInstance().m_nCurrentClub = 0


    -- 金币房卡列表
    self._goldPanel = self.m_spBg:getChildByName("Panel_gold")
    self._createPanel = self._goldPanel:getChildByName("Panel_7")
    self._listGoldRoomView = cc.TableView:create(self._createPanel:getContentSize())
    self._listGoldRoomView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listGoldRoomView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN) 
    self._listGoldRoomView:setPosition(cc.p(0,0))
    self._listGoldRoomView:setDelegate()
    self._listGoldRoomView:addTo(self._createPanel)
    self._listGoldRoomView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listGoldRoomView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listGoldRoomView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)


        --进入
    local addButton = self._goldPanel:getChildByName("Button_add")
    addButton:setTag(TAG_ENUM.BT_ENTER)
    addButton:addTouchEventListener(touchFunC)
    addButton:setPressedActionEnabled(true)


    
    -- if #self.m_tabGoldRoomList <=0 then
    --     self._goldPanel:getChildByName("Image_1"):setVisible(true)
    -- else
    --     self._goldPanel:getChildByName("Image_1"):setVisible(false)
    -- end
    local function createGoldRoom(ref, tType)
        if tType == ccui.TouchEventType.ended then
                    self._scorePanel:removeChildByName(PriRoom.NAME_CREATE_LAYER)
                    self._scorePanel:setVisible(true)
                    self:showPriCreateLayer(false)
                    self._goldPanel:setVisible(false)
        end
    end
    local createBtn = self._goldPanel:getChildByName("Button_create")
    createBtn:addTouchEventListener(createGoldRoom)
    self._createPanel:addTouchEventListener(createGoldRoom)

        -- --刷新
     self.btn_freshen = self._goldPanel:getChildByName("Button_update")
     self.btn_freshen:setTag(TAG_ENUM.BT_FRESHEN)
     self.btn_freshen:addTouchEventListener(touchFunC)
     self.btn_freshen:setPressedActionEnabled(true)

     self._goldPanel:getChildByName("Image_1"):setVisible(true)


      -- 获取房间列表
    self.m_tabList = {}
    local buttonHave = {false,false,false}

    --dump(GlobalUserItem.roomlist, "房间列表", 6)
    for k,v in pairs(GlobalUserItem.roomlist) do
        if tonumber(v[1]) == self.m_curGameKind then
            local listinfo = v[2]
            if type(listinfo) ~= "table" then
                break
            end
            local normalList = {}
            for k,v in pairs(listinfo) do
                if v.wServerType ~= yl.GAME_GENRE_PERSONAL and v.wServerType ~= yl.GAME_GENRE_MATCH then
                    table.insert( normalList, v)
                end
                if v.wServerType == yl.GAME_GENRE_PERSONAL then
                    print("getRoomSurportType", v.dwSurportType)
                    if v.dwSurportType == 0 then
                        buttonHave[2] = true   --支持积分房卡
                    elseif v.dwSurportType == 1 then
                        buttonHave[1] = true
                    end
                end
                if v.wServerType == yl.GAME_GENRE_GOLD and not self.m_param.clubID then
                    buttonHave[3] = true
                end
            end
            self.m_tabList = normalList
            self.m_nListCount = #self.m_tabList
            break
        end
    end 
    if self.m_curGameKind ==  145 then 
        buttonHave[1] = false --龙虎斗不显示金币约战，20191230
    end

    table.sort(self.m_tabList, function (a, b)
        return a.wServerLevel < b.wServerLevel
    end )


    self._scorePanel = self.m_spBg:getChildByName("Panel_score")

    --进入
    self.m_addButton = self._scorePanel:getChildByName("Button_add")
    self.m_addButton:setTag(TAG_ENUM.BT_ENTER)
    self.m_addButton:addTouchEventListener(touchFunC)
    self.m_addButton:setPressedActionEnabled(true)




    --普通金币场
    self._normalPanel = self.m_spBg:getChildByName("Panel_nomal")
    
    --快速开始
    self.fastStartBtn = self._normalPanel:getChildByName("btn_fast")
    self.fastStartBtn:setTag(TAG_ENUM.BT_FASTNORMAL)
    self.fastStartBtn:addTouchEventListener(touchFunC)
    self.fastStartBtn:setPressedActionEnabled(false)
        --获取我能加入的房间信息
        --先按入场分排序
        local _listInfo = clone(self.m_tabList)
        if #_listInfo > 0 then
            table.sort( _listInfo, function (a, b)
                return a.lEnterScore > b.lEnterScore
            end )
            for i=1,#_listInfo do
                local roominfo = _listInfo[i]
                if GlobalUserItem.tabAccountInfo.lUserScore >= roominfo.lEnterScore and bit:_and(roominfo.wServerKind, yl.SERVER_GENRE_PASSWD) == 0 
                    and bit:_and(roominfo.wServerType ,yl.GAME_GENRE_MATCH )  == 0 then
                    if self.fastStartBtn.roominfo then
                        if self.fastStartBtn.roominfo.wServerLevel <  roominfo.wServerLevel then 
                            self.fastStartBtn.roominfo = roominfo
                        end
                    else
                        self.fastStartBtn.roominfo = roominfo
                    end

                    print("快速开始房间信息------------------》", roominfo.wServerLevel)
                end
            end
        end

        -- 普通金币房列表
        local tablePanel = self._normalPanel:getChildByName("Panel_5")
        self._listViewNoPri = cc.TableView:create(tablePanel:getContentSize())
        self._listViewNoPri:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)      
        self._listViewNoPri:setPosition(cc.p(0,0))
        self._listViewNoPri:setDelegate()
        self._listViewNoPri:addTo(tablePanel)
        self._listViewNoPri:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
        self._listViewNoPri:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
        self._listViewNoPri:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
        

        -- 按钮
        self.m_itemFunC = function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                local beginPos = ref:getTouchBeganPosition()
                local endPos = ref:getTouchEndPosition()
                if math.abs(endPos.x - beginPos.x) > 30 
                    or math.abs(endPos.y - beginPos.y) > 30
                    or nil == this._listViewNoPri then
                    return
                end

                -- widget add 到 tableview 会在非可见区域触发touch事件
                local toViewPoint = this._listViewNoPri:convertToNodeSpace(endPos)
                local rec = cc.rect(0, 0, this._listViewNoPri:getViewSize().width, this._listViewNoPri:getViewSize().height)
                if true == cc.rectContainsPoint(rec, toViewPoint) then
                    this:onItemButtonClickedEvent(ref:getTag(), ref)  
                end  
            end
        end
        self._listViewNoPri:reloadData()

        local roomNum = 0
        dump(buttonHave,"================11")
        for i=1,3 do
            if buttonHave[i] then
                roomNum = roomNum +1
            end
        end
    self.m_curRoom = TAG_ENUM.BT_CK_GOLD
    if roomNum ==1 then
        if buttonHave[1] then
            --self:onGameRoomSelectedEvent(TAG_ENUM.BT_CK_GOLD)
            self.m_curRoom = TAG_ENUM.BT_CK_GOLD
        elseif buttonHave[2] then
            --self:onGameRoomSelectedEvent(TAG_ENUM.BT_CK_SCORE)
            self.m_curRoom = TAG_ENUM.BT_CK_SCORE
        elseif buttonHave[3] then 
            --self:onGameRoomSelectedEvent(TAG_ENUM.BT_CK_NORMAL)
            self.m_curRoom = TAG_ENUM.BT_CK_NORMAL

            cc.Sprite:create("plaza/plaza_btn_right_0.png"):move(30,375):setRotation(180):addTo(self._normalPanel)
        end

    elseif roomNum ==2 then
        if buttonHave[3] then
            self.m_roomTypeBtn[1]:setPositionY(520-168/2)
            self.m_roomTypeBtn[1]:setVisible(true)

            --self:onGameRoomSelectedEvent(TAG_ENUM.BT_CK_NORMAL)
            self.m_curRoom = TAG_ENUM.BT_CK_NORMAL
        end

        if buttonHave[2] then
            self.m_roomTypeBtn[2]:setPositionY(buttonHave[3] and 520-168*(1/2+1) or 520-168/2)
            self.m_roomTypeBtn[2]:setVisible(true)
            if not buttonHave[3] then
                --self:onGameRoomSelectedEvent(TAG_ENUM.BT_CK_SCORE)
                self.m_curRoom = TAG_ENUM.BT_CK_SCORE
            end
        end

        if buttonHave[1] then
            self.m_roomTypeBtn[3]:setVisible(true)
            self.m_roomTypeBtn[3]:setPositionY(520-168*(1/2+1))
        end

    elseif roomNum ==3 then
        for i=1,3 do
            self.m_roomTypeBtn[i]:setVisible(true)
        end
        --self:onGameRoomSelectedEvent(TAG_ENUM.BT_CK_SCORE)
        self.m_curRoom = TAG_ENUM.BT_CK_NORMAL
       
    end

    self._goldPanel:setVisible(false)
    self._normalPanel:setVisible(false)
    self._scorePanel:setVisible(false)
    -- 事件监听
    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onUserInfoChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)

    self:onReloadGoldRoomList()

    -- 注册事件监听
    --self:registerEventListenr()
    -- 加载动画
    self:noActionTransitionIn(self)

end


function RoomLayer:onGameRoomSelectedEvent(tag,sender)


    local imgPath = {"txt_normal","txt_score","txt_gold"}
    for k,v in pairs(self.m_roomTypeBtn) do
        if v then
            v:setSelected(false)
            v:getChildByName("img"):setTexture("room/"..imgPath[k].."1.png")
        end
    end
    self._goldPanel:setVisible(false)
    self._normalPanel:setVisible(false)
    self._scorePanel:setVisible(false)

    self._scene:updateCurrentGame()
    if tag == TAG_ENUM.BT_CK_GOLD then
        self.m_roomTypeBtn[3]:setSelected(true)
        self.m_roomTypeBtn[3]:getChildByName("img"):setTexture("room/"..imgPath[3].."2.png")
        self._goldPanel:setVisible(true)
        
        self.btn_freshen:setEnabled(true)
        self.m_addButton:setVisible(false)
        if self.m_param.clubID ~= nil then
            self._scorePanel:removeChildByName(PriRoom.NAME_CREATE_LAYER)
            self._scorePanel:setVisible(true)
            self:showPriCreateLayer(false)
            self._goldPanel:setVisible(false)
        else
                    --请求金币私有房列表
            PriRoom:getInstance():showPopWait()
            PriRoom:getInstance():setViewFrame(self)
            PriRoom:getInstance():getNetFrame():onQueryGoldRoomList()
        end

    elseif tag == TAG_ENUM.BT_CK_SCORE then
        self.m_roomTypeBtn[2]:setSelected(true)
        self.m_roomTypeBtn[2]:getChildByName("img"):setTexture("room/"..imgPath[2].."2.png")
        self._scorePanel:removeChildByName(PriRoom.NAME_CREATE_LAYER)
        self._scorePanel:setVisible(true)
        self:showPriCreateLayer(true)
        self.m_addButton:setVisible(true)

    elseif tag == TAG_ENUM.BT_CK_NORMAL then
        self.m_roomTypeBtn[1]:setSelected(true)
        self.m_roomTypeBtn[1]:getChildByName("img"):setTexture("room/"..imgPath[1].."2.png")
        self._normalPanel:setVisible(true)
    end

end


function RoomLayer:showPriCreateLayer(isScore)
        if not PriRoom:getInstance():isCurrentGameOpenPri(self.m_curGameKind) then
            showToast(self, "约战房未开启!", 1)
            return
        end
        self.m_param.cbGameMode = isScore
        if self.m_param.clubID ~=nil then
            PriRoom:getInstance().m_CurrentClubID = self.m_param.clubID
        end

        local cl = PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_CREATEPRIROOME, self._scene, self.m_param)
        if nil ~= cl then
            PriRoom:getInstance():setViewFrame(cl)
            self._scorePanel:addChild(cl)
            cl:setName(PriRoom.NAME_CREATE_LAYER)
        end
        
end

function RoomLayer:onAddNotice( msg )
    if true then
        return 
    end
    if type(msg) ~= "table" then
        dump(msg, "PlazaLayer:onAddNotice 参数非法", 6)
        return
    end
    table.insert( self.m_tabSystemNotice, msg )
    table.insert( self.m_tabMarquee, msg )
    if not self.m_bNotifyRunning then
        self:onChangeNotify(self.m_tabMarquee[self._sysIndex])
    end
end

function RoomLayer:onChangeNotify( msg )
    self._notifyText:stopAllActions()
    if not msg or not msg.title or #msg.title == 0 then
        self._notifyText:setString("")
        self.m_bNotifyRunning = false
        self._tipIndex = 1
        return
    end
    local showMessage = msg.title
    if not msg.showtitle then
        showMessage = msg.marqueetitle
    end
    if not showMessage or #showMessage == 0 then
        self._notifyText:setString("")
        self.m_bNotifyRunning = false
        self._tipIndex = 1
        return
    end
    self.m_bNotifyRunning = true
    local msgcolor = msg.color or cc.c4b(255,191,123,255)
    self._notifyText:setVisible(false)
    self._notifyText:setString(showMessage)
    --self._notifyText:setTextColor(msgcolor)

    if true == msg.autoremove then
        msg.showcount = msg.showcount or 0
        msg.showcount = msg.showcount - 1
        if msg.showcount <= 0 then
            self:removeNoticeById(msg.id)
        end
    end
    
    local tmpWidth = self._notifyText:getContentSize().width
    self._notifyText:runAction(
        cc.Sequence:create(
            cc.CallFunc:create( function()
                self._notifyText:move(self.m_noticeArea.width, 0)
                self._notifyText:setVisible(true)
            end),
            cc.MoveTo:create(16 + (tmpWidth / self.m_noticeArea.width) * 8,cc.p(0 - tmpWidth,0)),
            cc.CallFunc:create( function()
                local tipsSize = 0
                local tips = {}
                local index = 1
                -- 系统公告
                local tmp = self._sysIndex + 1
                if tmp > #self.m_tabMarquee then
                    tmp = 1
                end
                self._sysIndex = tmp
                self:onChangeNotify(self.m_tabMarquee[self._sysIndex])             
            end)
        )
    )
end

-- 移除公告
function RoomLayer:removeNoticeById(id)
    if nil == id then
        return
    end
    local idx = nil
    for k,v in pairs(self.m_tabMarquee) do
        if nil ~= v.id and v.id == id then
            idx = k
            break
        end
    end
    if nil ~= idx then
        table.remove(self.m_tabMarquee, idx)
    end
end

-- 弹出消息
function RoomLayer:popNotice()
    local pn = PlazaNoticeLayer:create(self._scene, self.m_tabSystemNotice, PlazaNoticeLayer.SECOND_LEVEL)
    pn:setName(PLAZA_NOTICE_LAYER)
    self._scene:addPopLayer(pn)
end

function RoomLayer:onButtonClickedEvent( tag, ref )
    if TAG_ENUM.BT_EXIT == tag then
        --self:scaleTransitionOut(self.m_spBg)
        if self.m_param.clubID ~= nil then
                    self:scaleTransitionOut(self.m_spBg)
                    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
                    eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
                    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
        else
                    self._scene:onChangeShowMode(yl.SCENE_PLAZA)
        end
    elseif TAG_ENUM.BT_NOTICE == tag then
        print("============================8888888888888")
        self:popNotice()
    elseif TAG_ENUM.BT_ADWARD == tag then
        self._scene:popAdward()
    elseif TAG_ENUM.BT_SET == tag then
        self._scene:popSet()
    elseif TAG_ENUM.BT_MARK == tag then
        self._scene:popMark()
    elseif TAG_ENUM.BT_SPREAD == tag then
        self._scene:popSpread()
        self.m_imageSpreadBubble:setVisible(GlobalUserItem.bSpreadTipShow)
    elseif TAG_ENUM.BT_SERVICE == tag then
        self._scene:popService()
    elseif TAG_ENUM.BT_SHOP == tag then
        local st = ShopLayer:create(self._scene, ShopLayer.DIAMOND_CHARGE, ShopLayer.SECOND_LEVEL)
        self:addChild(st)
    elseif TAG_ENUM.BT_GOLD == tag then
        local st = ShopLayer:create(self._scene, ShopLayer.GOLD_EXCHANGE, ShopLayer.SECOND_LEVEL)
        self:addChild(st)
    elseif TAG_ENUM.BT_REWARD == tag then
        local st = ShopLayer:create(self._scene, ShopLayer.EXCHANGE, ShopLayer.SECOND_LEVEL)
        self:addChild(st)
    elseif TAG_ENUM.BT_HELP == tag then
        local pa = PlazaIntroductionLayer:create(self._scene, {kindId = self.m_curGameKind}, PlazaIntroductionLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BT_ENTER == tag then                         -- 加入房间
        local pa = PrivateIdInputLayer:create(self._scene, {}, PrivateIdInputLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BT_CREATE == tag then  
        if not PriRoom:getInstance():isCurrentGameOpenPri(self.m_curGameKind) then
            showToast(self, "约战房未开启!", 1)
            return
        end

        if not self._clickDelay  then
            self._clickDelay = true  

            local cl = PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_CREATEPRIROOME, self._scene, {})
            if nil ~= cl then
                PriRoom:getInstance():setViewFrame(cl)
                self._scene:addPopLayer(cl)
                cl:setName(PriRoom.NAME_CREATE_LAYER)
            end
            
            if not self.m_param.clubID then
                PriRoom:getInstance().m_CurrentClubID = self.m_param.clubID
            else
                PriRoom:getInstance().m_CurrentClubID = 0
            end

            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.4),cc.CallFunc:create(function ( ... )
                self._clickDelay = false
            end)))
            
        end


    elseif TAG_ENUM.BT_FRESHEN == tag then  
        --请求金币私有房列表
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():setViewFrame(self)
        PriRoom:getInstance():getNetFrame():onQueryGoldRoomList()
        self.btn_freshen:getChildByName("Sprite_25"):setVisible(false)
        self.btn_freshen:setEnabled(false)
            local obj = SpineObj:create("room/skeleton.json", "room/skeleton.atlas", false)
            obj:setPosition(self.btn_freshen:getPositionX(),self.btn_freshen:getPositionY())
            obj:runAction(cc.Sequence:create(cc.DelayTime:create(1),cc.CallFunc:create(function ( ... )
                self.btn_freshen:getChildByName("Sprite_25"):setVisible(true)
                self.btn_freshen:setEnabled(true)
                obj:destroy()
            end)))
            self._goldPanel:addChild(obj)

    elseif TAG_ENUM.BT_FASTNORMAL == tag or TAG_ENUM.BT_FAST == tag then  
        --查找可以加入的房间
        local _listInfo = clone(self.m_tabList)
        table.sort( _listInfo, function (a, b)
            return a.lEnterScore > b.lEnterScore
        end )

        local isFindRoom = false
        for i=1,#_listInfo do
            local roominfo = _listInfo[i]
            if GlobalUserItem.tabAccountInfo.lUserScore >= roominfo.lEnterScore and bit:_and(roominfo.wServerKind, yl.SERVER_GENRE_PASSWD) == 0 
                and bit:_and(roominfo.wServerType ,yl.GAME_GENRE_MATCH )  == 0 then
                local roominfo = roominfo
                GlobalUserItem.nCurRoomIndex = roominfo._nRoomIndex
                GlobalUserItem.bPrivateRoom = (roominfo.wServerType == yl.GAME_GENRE_PERSONAL)
                GlobalUserItem.bMatch = (roominfo.wServerType == yl.GAME_GENRE_MATCH)
                GlobalUserItem.bPrivateGoldRoom = (roominfo.dwSurportType == 1)         --金币私人房
             
                -- 密码
                if bit:_and(roominfo.wServerKind, yl.SERVER_GENRE_PASSWD) ~= 0 then
                    print("房间需要密码")
                    return
                end

                -- 比赛
                if bit:_and(roominfo.wServerType ,yl.GAME_GENRE_MATCH )  ~= 0 then
                    showToast(self,"暂不支持比赛房间！",1)
                    return
                end
                self._scene:onStartGame()
                isFindRoom = true

                break
            end
        end
        if not isFindRoom then
            showToast(self,"您没有可以加入的房间，请检查您的金币！",1)
        end
    end
end

function RoomLayer:onItemButtonClickedEvent( tag, ref )
    local roominfo = ref.roominfo
    if nil == roominfo then
        print("房间信息为空")
        return
    end

    GlobalUserItem.nCurRoomIndex = roominfo._nRoomIndex
    GlobalUserItem.bPrivateRoom = (roominfo.wServerType == yl.GAME_GENRE_PERSONAL)
    GlobalUserItem.bMatch = (roominfo.wServerType == yl.GAME_GENRE_MATCH)
    GlobalUserItem.bPrivateGoldRoom = (roominfo.dwSurportType == 1)         --金币私人房
 
    -- 密码
    if bit:_and(roominfo.wServerKind, yl.SERVER_GENRE_PASSWD) ~= 0 then
        print("房间需要密码")
        return
    end

    -- 比赛
    if bit:_and(roominfo.wServerType ,yl.GAME_GENRE_MATCH )  ~= 0 then
        showToast(self,"暂不支持比赛房间！",1)
        return
    end
    self._scene:onStartGame()
end

function RoomLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function RoomLayer:onTransitionInFinish()

    -- local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    -- if bPirGoldMode and not self.m_param.clubID  then
    --     --请求金币私有房列表
    --     PriRoom:getInstance():showPopWait()
    --     PriRoom:getInstance():setViewFrame(self)
    --     self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ()
    --         PriRoom:getInstance():getNetFrame():onQueryGoldRoomList()
    --     end)))   
    -- end
    self:runAction(cc.Sequence:create(cc.DelayTime:create(0.2),cc.CallFunc:create(function ( ... )
        self:onGameRoomSelectedEvent(self.m_curRoom)
    end)))
    

    self:refreshUserScore()
    --请求人数信息
    --self._logonFrame:onGetOnline(self._roomCount, self.m_roomList)
end

function RoomLayer:onTransitionOutFinish()
    PriRoom:getInstance():setViewFrame(nil)
    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
        self.m_listener = nil
    end 
    self:removeFromParent() 
end

function RoomLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function RoomLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
    --请求金币私有房列表
    -- PriRoom:getInstance():showPopWait()
    -- PriRoom:getInstance():setViewFrame(self)
    -- PriRoom:getInstance():getNetFrame():onQueryGoldRoomList()
    --刷新金钱
    self:refreshUserScore()
    
end

function RoomLayer:refreshUserScore()
       --更新财富
        local diamondbg = self.m_spBg:getChildByName("Node_score"):getChildByName("plaza_image_diamondbg")
        local tmptxt = diamondbg:getChildByName("txt_diamond")
        local lScore = GlobalUserItem.tabAccountInfo.lDiamond
        local szscore = 0
        local pathStr = nil
        if lScore < 10000 then
            szscore = lScore
        elseif lScore >= 10000 and lScore < 100000000 then
            if lScore / 10000 == 10000 then
                szscore = szscore .."1.00"
                pathStr = "plaza/score_yi.png"
            else
                szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
                pathStr = "plaza/score_wan.png"
            end
        else
            szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
            pathStr = "plaza/score_yi.png"
        end

        tmptxt:setString(szscore.."")

        tmptxt:removeChildByName("_type_")
         if pathStr ~= nil then
            local scoreSize = tmptxt:getContentSize()
            local spWan = cc.Sprite:create(pathStr)
            spWan:setName("_type_")
            spWan:setAnchorPoint(cc.p(0,0.5))
            spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
            spWan:addTo(tmptxt)
        end


        -- 金币
        local scorebg = self.m_spBg:getChildByName("Node_score"):getChildByName("plaza_image_exchange")
        local tmptxt = scorebg:getChildByName("txt_score")
        local lScore = GlobalUserItem.tabAccountInfo.lUserScore

        local szscore = 0
        local pathStr = nil
        if lScore < 10000 then
            szscore = lScore
        elseif lScore >= 10000 and lScore < 100000000 then
            if lScore / 10000 == 10000 then
                szscore = szscore .."1.00"
                pathStr = "plaza/score_yi.png"
            else
                szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
                pathStr = "plaza/score_wan.png"
            end
        else
            szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
            pathStr = "plaza/score_yi.png"
        end

        tmptxt:setString(szscore.."")

        tmptxt:removeChildByName("_type_")
         if pathStr ~= nil then
            local scoreSize = tmptxt:getContentSize()
            local spWan = cc.Sprite:create(pathStr)
            spWan:setName("_type_")
            spWan:setAnchorPoint(cc.p(0,0.5))
            spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
            spWan:addTo(tmptxt)
        end

         -- 奖券
        local ticketbg = self.m_spBg:getChildByName("Node_score"):getChildByName("plaza_image_reward")
        local tmptxt = ticketbg:getChildByName("txt_diamond")
        local lScore = GlobalUserItem.tabAccountInfo.nAwardsTickets

        local szscore = 0
        local pathStr = nil
        if lScore < 10000 then
            szscore = lScore
        elseif lScore >= 10000 and lScore < 100000000 then
            if lScore / 10000 == 10000 then
                szscore = szscore .."1.00"
                pathStr = "plaza/score_yi.png"
            else
                szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
                pathStr = "plaza/score_wan.png"
            end
        else
            szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
            pathStr = "plaza/score_yi.png"
        end
        tmptxt:setString(szscore.."")

        tmptxt:removeChildByName("_type_")
         if pathStr ~= nil then
            local scoreSize = tmptxt:getContentSize()
            local spWan = cc.Sprite:create(pathStr)
            spWan:setName("_type_")
            spWan:setAnchorPoint(cc.p(0,0.5))
            spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
            spWan:addTo(tmptxt)
        end
    
end
 
function RoomLayer:onUserInfoChange( event  )
    print("----------userinfo change notify------------")

    local msgWhat = event.obj
    
    if nil ~= msgWhat then
       if msgWhat == yl.RY_MSG_USERWEALTH or msgWhat == yl.RY_CULB_DIAMOND_NOTIFY then
            --更新财富
            self:refreshUserScore()
        end
    end
end

-- 子视图大小
function RoomLayer:cellSizeForTable(view, idx)
    if self._listGoldRoomView == view then
        return 1070 , 223
    elseif self._listViewNoPri == view then
        return 350 , 426
    end
end

function RoomLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    if self._listGoldRoomView == view then

        for i=1,2 do
                --金币房卡房间列表
                local roominfo = self.m_tabGoldRoomList[idx*2 +i]
                if nil == roominfo then
                    return cell
                end

                local csb = ExternalFun.loadCSB("room/goldCell.csb", cell)
                csb:setPosition(2+561*(i-1), 0)

                local btnCallBack = function(ref, tType)
                    if tType == ccui.TouchEventType.ended then
                        
                    end
                end
                local btnCallBack = function(ref, tType)
                        if tType == ccui.TouchEventType.ended then
                            local beginPos = ref:getTouchBeganPosition()
                            local endPos = ref:getTouchEndPosition()
                            if math.abs(endPos.x - beginPos.x) > 30 
                                or math.abs(endPos.y - beginPos.y) > 30
                                or nil == self._listGoldRoomView then
                                return
                            end

                            -- widget add 到 tableview 会在非可见区域触发touch事件
                            local toViewPoint = self._listGoldRoomView:convertToNodeSpace(endPos)
                            local rec = cc.rect(0, 0, self._listGoldRoomView:getViewSize().width, self._listGoldRoomView:getViewSize().height)
                            if true == cc.rectContainsPoint(rec, toViewPoint) then
                                        local roominfo = ref.roominfo
                                        if nil == roominfo then
                                            print("房间信息为空")
                                            return
                                        end
                                        --发送进入私有房
                                        GlobalUserItem.bPrivateGoldRoom = (roominfo.dwSurportType == 1)         --金币私人房
                                        
                                        PriRoom:getInstance():showPopWait()
                                        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function()
                                            PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(roominfo.dwPersonalRoomID))
                                        end)))
                            end  
                        end
                    end
                    
                local btn = csb:getChildByName("Button_2")
                btn:setSwallowTouches(false)
                btn:addTouchEventListener(btnCallBack)
                btn.roominfo = roominfo

                --信息
                --底分
                local txtCellScore = csb:getChildByName("AtlasLabel_1")
                txtCellScore:setString(roominfo.lCellScore)
                --规则
                --local txt_rule = bg:getChildByName("txt_rule")
                local tabRule = self:getGameRule(roominfo.cbGameRule)
                local btnRuleCallBack = function(ref, tType)
                    if tType == ccui.TouchEventType.ended then
                        local runningScene = cc.Director:getInstance():getRunningScene()
                        local posWorld = csb:convertToWorldSpace(cc.p(185, 146))
                        local posNode = runningScene:convertToNodeSpace(posWorld)
                        print("####################", posNode.x, posNode.y)

                        local csbInfo = ExternalFun.loadCSB("plaza/RoonInfoLayer.csb", runningScene)
                        local imagecell = csbInfo:getChildByName("sp_bg")
                        csbInfo:setName("node_rule")
                        --runningScene:addChild(csbInfo, 100)
                        imagecell:setPosition(posNode)

                        local touchFunC = function(ref, tType)
                            if tType == ccui.TouchEventType.ended then
                                runningScene:getChildByName("node_rule"):removeFromParent()
                            end
                        end
                        -- 底板
                        local spbg = csbInfo:getChildByName("panel_mask")
                        spbg:addTouchEventListener( touchFunC )


                        local str = ""
                        for j=1, #tabRule do
                            if j == #tabRule then
                                str = str ..  tabRule[j]
                            else
                                str = str ..  tabRule[j] .. ","
                            end
                        end

                        local txtContent = cc.Label:createWithTTF(str, "fonts/FZHTJW.TTF", 24, cc.size(300, 0), cc.TEXT_ALIGNMENT_CENTER)
                        --txtContent:setLineBreakWithoutSpace(true)
                        txtContent:setAnchorPoint(cc.p(0.5, 0.5))
                        txtContent:setColor(cc.c3b(0xb8, 0xad, 0xfe))
                        txtContent:setPosition(cc.p(163, 70))
                        imagecell:addChild(txtContent)
                    end
                end

                local btnInfo = csb:getChildByName("Button_1")
                btnInfo:setSwallowTouches(false)
                btnInfo:setTouchEnabled(true)
                btnInfo:addTouchEventListener(btnRuleCallBack)
                btnInfo.roominfo = tabRule
                btnInfo:setVisible(false)


                csb:getChildByName("rule1"):setVisible(false)
                csb:getChildByName("rule2"):setVisible(false)
                csb:getChildByName("rule3"):setVisible(false)

                if nil ~= tabRule then
                    --获取数量
                    local ruleNum = #tabRule
                    if 1 == ruleNum then
                        local rule = csb:getChildByName("rule2")
                        rule:setVisible(true)
                        rule:getChildByName("Text_1"):setString(""..tabRule[1])
                        rule:getChildByName("Text_1"):setContentSize(cc.size(175, 22)) 
                    else
                        for j=1,ruleNum do
                            if j>3 then
                                btnInfo:setVisible(true)
                                break
                            end
                            local rule = csb:getChildByName("rule"..j)
                            rule:setVisible(true)
                            rule:getChildByName("Text_1"):setString(""..tabRule[j])
                            if ruleNum <=3  then
                                    rule:getChildByName("Text_1"):setContentSize(cc.size(175, 22))
                            end
                        end
                       
                    end
                    
                else
                        local rule = csb:getChildByName("rule2")
                        rule:setVisible(true)
                        rule:getChildByName("Text_1"):setString("普通模式")
                end
                --id
                local txt_id = csb:getChildByName("roomId")
                txt_id:setString("ID:"..roominfo.dwPersonalRoomID)


                --入场分
                local txt_enterscore = csb:getChildByName("enterScore")
                txt_enterscore:setString(roominfo.lEnterScoreLimit)
                local goldimg = txt_enterscore:getChildByName("Sprite_7")
                goldimg:setPositionX(txt_enterscore:getContentSize().width)
                --离场分
                local txt_exitscore = csb:getChildByName("exitScore")
                txt_exitscore:setString(roominfo.lLeaveScoreLimit)
                local goldimg = txt_exitscore:getChildByName("Sprite_7")
                goldimg:setPositionX(txt_exitscore:getContentSize().width)

                --人数
                local txt_player = csb:getChildByName("playerNum")
                txt_player:setString(roominfo.lPlayerCount.."/"..roominfo.wPlayerCount)
        end
        
    elseif self._listViewNoPri == view then
        local roominfo = self.m_tabList[idx + 1]
        if nil == roominfo then
            return cell
        end
        local wLv = roominfo.wServerLevel
        local rule = roominfo.dwServerRule
        -- wLv = (bit:_and(yl.SR_ALLOW_AVERT_CHEAT_MODE, rule) ~= 0) and 10 or roominfo.wServerLevel
        -- wLv = (wLv ~= 0) and wLv or 1
        local iconfile = string.format("public/roomicon_%d.png", wLv)
        if not cc.FileUtils:getInstance():isFileExist(iconfile) then
            iconfile = "public/roomicon_0.png"
        end


        local csb = ExternalFun.loadCSB("public/Node_RoomInfo_NoGold.csb", cell)
        csb:setPosition(175, view:getViewSize().height * 0.5)
        local btn = csb:getChildByName("btn")

        if roominfo.wKindID == 148 then
            iconfile = "public/hongbaoBg.png"
            local num = btn:getChildByName("AtlasLabel_1")
            num:setString(""..wLv)
            num:setPositionY(230)
            num:setVisible(true)
            local txt = btn:getChildByName("Sprite_1")
            txt:setVisible(true)
            txt:setPositionY(230)

        end

        btn:loadTextures(iconfile, iconfile, iconfile)
        btn:setPressedActionEnabled(false)
        btn:setSwallowTouches(false)
        btn:addTouchEventListener(self.m_itemFunC)
        btn.roominfo = roominfo

        --dump(roominfo,"=======================")
        local txtScore = btn:getChildByName("txt_score")
        if nil ~= txtScore then
            txtScore:setString(roominfo.lEnterScore)
        end
        csb:runAction(cc.Sequence:create(cc.DelayTime:create(0.88*idx),cc.CallFunc:create(function ( ... )
                local obj = SpineObj:create("plaza/animate/roomlist/skeleton.json", "plaza/animate/roomlist/skeleton.atlas", true)
                    obj:setPosition(0, 0)
                    csb:addChild(obj)
        end)))


        function getScoreString( num )
            local str = ""
            if num < 1000 then
                str = num
            elseif num >= 1000 and num < 100000 then
                str = string.format("%d", num / 1000)
                num = math.mod(num, 1000)
                str = str .. "."
                str = str .. string.format("%d", num / 100)
                num = math.mod(num, 100)
                str = str .. string.format("%d", num / 10)
                num = math.mod(num, 10)
                str = str .. string.format("%d", num / 1)
            elseif num >= 100000 and num < 10000000 then
                str = string.format("%d", num / 10000)
                str = str .. "/"
            else
                str = string.format("%d", num / 10000000)
                num = math.mod(num, 10000000)
                str = str .. "."
                str = str .. string.format("%d", num / 1000000)
                num = math.mod(num, 1000000)
                str = str .. string.format("%d", num / 100000)
                num = math.mod(num, 100000)
                str = str .. string.format("%d", num / 10000)
                num = math.mod(num, 10000)
                str = str .. "/"
            end
            return str
        end


        local txtLv = btn:getChildByName("txt_cellScore")
        if nil ~= txtLv then
            --txtLv:setString(roominfo.lCellScore)
            local lCellScore = roominfo.lCellScore
            if lCellScore < 10000 then
                txtLv:setString(lCellScore)
            else
                local str = string.gsub(string.format("%.1f", lCellScore / 10000), "[.]", ".")
                str = str .. "/"
                txtLv:setString(str)
            end
        end


        if roominfo.wKindID == 148 then
            txtLv:setVisible(false)
        end

        local txtPlayer = btn:getChildByName("txt_player")
        if nil ~= txtPlayer then
            --txtPlayer:setString(roominfo.dwOnLineCount + roominfo.dwAndroidCount)
        end
        

        local txtTips = btn:getChildByName("txt_tips")
        if nil ~= txtTips then
            if "" ~= roominfo.szGameInfomation and nil ~= roominfo.szGameInfomation then
                local strTips = string.EllipsisByConfig(roominfo.szGameInfomation, 250, 
                                    string.getConfig("fonts/round_body.ttf", 20))
                txtTips:setString(strTips)
            else
                txtTips:setString("")
            end
        end

    end

    return cell
end

-- 子视图数目
function RoomLayer:numberOfCellsInTableView(view)
    if self._listGoldRoomView == view then
        return math.ceil(self.m_nGoldRoomListCount/2)
    elseif self._listViewNoPri == view then
        return self.m_nListCount
    end

end

function RoomLayer:onLogonCallBack(result,message)
    if result == logincmd.SUB_MB_SERVER_ONLINE then --成功
        if nil == self._logonFrame._OnlineInfo or 0 == #self._logonFrame._OnlineInfo then
            return
        end

        --刷新人数
        for i=1,self.m_nListCount do
            local roominfo = self.m_tabList[i]
            for j=1, #self._logonFrame._OnlineInfo do
                local info = self._logonFrame._OnlineInfo[j]
                if info.wServerID == roominfo.wServerID then
                    roominfo.dwOnLineCount = info.dwOnlineCount ~= nil and info.dwOnlineCount or 0
                    roominfo.dwAndroidCount = info.dwAndroidCount ~= nil and info.dwAndroidCount or 0
                end
            end
        end
       
     elseif result == -1 then --失败
		if type(message) == "string" and message ~= "" then
			showToast(self,message,2,cc.c4b(250,0,0,255));
		end
     elseif result == 10 then --重复绑定
     end
end

function RoomLayer:onReloadGoldRoomList()
    print("=============onReloadGoldRoomList==============")
    -- 获取房间列表
    self.m_tabGoldRoomList = {}
   -- dump(PriRoom:getInstance().m_tabGoldRoomList, "onReloadGoldRoomList", 2)
    for i=1,#PriRoom:getInstance().m_tabGoldRoomList do
        local info = PriRoom:getInstance().m_tabGoldRoomList[i]
        if nil ~= info.dwPersonalRoomID and 0 ~= info.dwPersonalRoomID then
            table.insert( self.m_tabGoldRoomList,info)
        end
    end
    
    table.sort(self.m_tabGoldRoomList,function (a,b)

        if a.lPlayerCount == b.lPlayerCount then
            if a.lCellScore==b.lCellScore then
                return a.szCreateTime > b.szCreateTime
            else
                return a.lCellScore>b.lCellScore
            end
            
        else
            return a.lPlayerCount>b.lPlayerCount
        end
        
    end)


    --dump(self.m_tabGoldRoomList, "onReloadGoldRoomList", 2)
    self.m_nGoldRoomListCount = #self.m_tabGoldRoomList
    if #self.m_tabGoldRoomList <=0 then
        self._goldPanel:getChildByName("Image_1"):setVisible(true)
        --self._createPanel:setEnabled(true)
        self._createPanel:setTouchEnabled(true)
    else
        self._goldPanel:getChildByName("Image_1"):setVisible(false)
        self._createPanel:setTouchEnabled(false)
    end

    self._listGoldRoomView:reloadData()

    -- --如果没有房间显示提示
    -- if 0 == self.m_nGoldRoomListCount then
    --     --self.labNoRoomTips:setVisible(true)
    --     self.btn_fastCreate:setEnabled(true)
    --     self.sp_tips:setVisible(true)
    -- else
    --     --self.labNoRoomTips:setVisible(false)
    --     self.btn_fastCreate:setEnabled(false)
    --     self.sp_tips:setVisible(false)
    -- end
end

function RoomLayer:getGameRule(pData)
    local gamelayer = self:getGameFile()
    if nil ~= gamelayer and nil ~= gamelayer.getGameRule then
        return gamelayer:getGameRule(pData)
    end
    return nil
end

-- 获取 GameVideoReplay
function RoomLayer:getGameFile()
    print("getReplayScriptFile GameKind ==> ", GlobalUserItem.nCurGameKind)
    local videogame = nil
    -- 寻找游戏
    if nil ~= self._scene then
        videogame = self._scene:getGameInfo(self.m_curGameKind)
    end
    if nil ~= videogame then
        local modulestr = string.gsub(videogame._KindName, "%.", "/")
        local gameFile = ""
        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            gameFile = "game/" .. modulestr .. "src/views/GameLayer.lua" .. yl.WIN_SRC_SUFFIX
        else
            gameFile = "game/" .. modulestr .. "src/views/GameLayer.lua"
        end
        if cc.FileUtils:getInstance():isFileExist(gameFile) then
            return appdf.req(gameFile)
        end
    end
    return nil
end

return RoomLayer