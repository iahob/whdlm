
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")

-- 用户中心(二级弹窗)
local ClubRoomLayer = class("ClubRoomLayer", TransitionLayer)

local ClubApplyMsgLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubApplyMsgLayer")
local ClubMemberLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMemberLayer")
local ClubCaptainLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubCaptainLayer")
local ClubGameSelectLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubGameSelectLayer")
local ClubNoticeLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubNoticeLayer")
local ClubMemberScore = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMemberScore")
local ClubSetLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubSetLayer")
local ClubMyRecordLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMyRecordLayer")
local ClubFastGameSet = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubFastGameSet")
local ClubShareLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubShareLayer")
local ClubSpreadLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubSpreadLayer")
local ClubScoreBox = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubScoreBox")

local ShopLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ShopLayer")                               -- 商城
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local RoomListLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "RoomLayer")                       -- 房间列表
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")

local SpineObj = appdf.req("client.src.external.SpineObj")


local targetPlatform = cc.Application:getInstance():getTargetPlatform()


local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_MENU",              -- 菜单
    "BTN_CAPTAIN",        -- 统计
    "BTN_MEMBER",            -- 人员
    "BTN_APPLYMSG",         -- 申请信息
    "BTN_INFO",             -- 信息
    "BTN_SET",           -- 设置
    "BTN_CHECK",             -- 查询信息
    "BT_DIAMOND",              -- 大联盟砖石
    "BT_CLUBGOLD",              -- 大联盟币
    "BTN_MESSAGE",          --消息
    "BTN_CLUBAD",               --大联盟公告
    "BTN_SHARE",                --分享
    "BTN_CREATE",           --积分，金币切换
    "BTN_RULESET",
    "BTN_DAMD",
    "BTN_CHANGE",
    "BTN_FASTBEGAN",
    "BTN_SCOREBOX",
    "BTN_BROADCAST",    --广播
    "BTN_SCORE",        --勋章

}


local PlayerChairPoint ={{},{cc.p(80,-48),cc.p(-80,101)},
{cc.p(-80,-49),cc.p(80,-49),cc.p(0,100)},
{cc.p(-80,-48),cc.p(80,-48),cc.p(80,101),cc.p(-80,101)},
{cc.p(-80,-48),cc.p(80,-48),cc.p(111,68),cc.p(0,101),cc.p(-111,68)},
{cc.p(-80,-48),cc.p(80,-48),cc.p(127,28),cc.p(80,101),cc.p(-80,101),cc.p(-127,28)},
{cc.p(-80,-48),cc.p(80,-48),cc.p(127,28),cc.p(127,101),cc.p(0,101),cc.p(-80,101),cc.p(-127,28)},
{cc.p(-80,-48),cc.p(0,-48),cc.p(80,-48),cc.p(127,28),cc.p(80,101),cc.p(0,101),cc.p(-80,101),cc.p(-127,28)},
{cc.p(-80,-48),cc.p(0,-48),cc.p(80,-48),cc.p(127,-6),cc.p(127,62),cc.p(50,101),cc.p(-50,101),cc.p(-127,62),cc.p(-127,-6)}}

local majongGame = {302,389,391, 399}

local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubRoomLayer:ctor( scene, param, level )
    local this = self

    self._scene = scene

    ClubRoomLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/club_GameListLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    self.scrollBg = csbNode:getChildByName("yuanjing")

    -- 关闭
    local btn = spbg:getChildByName("btn_back")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --设置
    local btn = spbg:getChildByName("btn_set")
    btn:setTag(TAG_ENUM.BTN_SET)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)




    --积分金币切换
    local btn = spbg:getChildByName("btn_create")
    btn:setTag(TAG_ENUM.BTN_CREATE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(false)

    self.btn_create = btn



     local obj = SpineObj:create("club/animate/topBg.json", "club/animate/topBg.atlas", true)
             obj:setPosition(750,90)
             --obj:setLocalZOrder(-1)
             self.scrollBg:addChild(obj)             

    --管理玩法
    local btn = spbg:getChildByName("btn_wanfa")
    btn:setTag(TAG_ENUM.BTN_RULESET)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

     local obj = SpineObj:create("club/animate/configEdit.json", "club/animate/configEdit.atlas", true)
             obj:setPosition(90,45)
             --obj:setLocalZOrder(-1)
             btn:addChild(obj)   


    cc.UserDefault:getInstance():setDoubleForKey(GlobalUserItem.tabAccountInfo.dwUserID.."lastClub", ClubFrame:getInstance().m_nCurrentClub)
    cc.UserDefault:getInstance():setBoolForKey(GlobalUserItem.tabAccountInfo.dwUserID.."lastFlag", true)

    self.m_lastGame = math.floor(cc.UserDefault:getInstance():getDoubleForKey(GlobalUserItem.tabAccountInfo.dwUserID.."lastGame",0))

    ClubFrame:getInstance().m_cbClubCurPlayMode = cc.UserDefault:getInstance():getIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID..ClubFrame:getInstance().m_nCurrentClub.."lastMode",1)
    
    if ClubFrame:getInstance().m_cbClubCurPlayMode == 0 then
        self.btn_create:loadTextureNormal("club/scoreChange1.png")
        self.btn_create:loadTexturePressed("club/scoreChange1.png")
        self.btn_create:loadTextureDisabled("club/scoreChange1.png")
        
    else
        self.btn_create:loadTextureNormal("club/scoreChange2.png")
        self.btn_create:loadTexturePressed("club/scoreChange2.png")
        self.btn_create:loadTextureDisabled("club/scoreChange2.png")
    end


    self.m_OxeightCurIndex = cc.UserDefault:getInstance():getIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID.."oxeightId",0)
    --菜单按钮
    -- local btn = spbg:getChildByName("btn_menu")
    -- btn:setTag(TAG_ENUM.BTN_MENU)
    -- btn:addTouchEventListener( touchFunC )
    -- btn:setPressedActionEnabled(true)
    -- btn:setLocalZOrder(101)

    --菜单背景
    -- self.m_spMenuBg = spbg:getChildByName("Image_menuBg")
    -- self.m_spMenuBg:setLocalZOrder(100)

    self.m_needMoveBtn={}
    --队长管理
    local btn = self.m_spBg:getChildByName("btn_statistics")
    btn:setTag(TAG_ENUM.BTN_CAPTAIN)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    self.m_btnCaptain = btn


        --分享
    local btn = self.m_spBg:getChildByName("btn_share")
    btn:setTag(TAG_ENUM.BTN_SHARE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    table.insert(self.m_needMoveBtn,btn)

        --查询战绩
    local btn = spbg:getChildByName("btn_check")
    btn:setTag(TAG_ENUM.BTN_CHECK)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    table.insert(self.m_needMoveBtn,btn)

    --成员
    local btn = self.m_spBg:getChildByName("btn_member")
    btn:setTag(TAG_ENUM.BTN_MEMBER)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_btnMember = btn

    --广播
    local btn = self.m_spBg:getChildByName("btn_broadcast")
    btn:setTag(TAG_ENUM.BTN_BROADCAST)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_btnNotice = btn


    --公告
    local btn = self.m_spBg:getChildByName("btn_ad")
    btn:setTag(TAG_ENUM.BTN_CLUBAD)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_btnGongGao = btn

    --勋章界面
    local btn = self.m_spBg:getChildByName("btn_score")
    btn:setTag(TAG_ENUM.BTN_SCORE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_btnScore = btn

    --信息
    local btn = self.m_spBg:getChildByName("btn_message")
    btn:setTag(TAG_ENUM.BTN_MESSAGE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --积分提取
    local btn = self.m_spBg:getChildByName("btn_box")
    btn:setTag(TAG_ENUM.BTN_SCOREBOX)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_scoreBox = btn

    self.m_groupId = self.m_spBg:getChildByName("txt_clubID")

    self.m_goldString = self.m_spBg:getChildByName("glodBg"):getChildByName("AtlasLabel_1")

     self.m_damdString = self.m_spBg:getChildByName("damdBg"):getChildByName("AtlasLabel_1")  


     self.m_clubName =  self.m_spBg:getChildByName("txt_clubName")

     self.m_emptyTip =  self.m_spBg:getChildByName("empty_tip")

     self.m_rightIcon = self.m_spBg:getChildByName("club_sp_swarrow")

    

     self.m_notice = self.m_spBg:getChildByName("Image_1"):getChildByName("Panel_1"):getChildByName("Text_1")   

    self.m_curRollIndex = 0
    if #ClubFrame:getInstance().m_tabClubRollMsg > 0 then
             self:updateRollMsg()
    end


    self.m_roomList = {}

    self.m_sGameRoomList = {} --选中游戏的房间列表

    --当前选中房间信息
    self.m_nCurrentRoomInfo = {}

    --玩法列表
    self.m_configList={}

    self.m_sGameConfigList ={} --选中游戏的玩法列表

    self.m_oxeightConfigList = {} --八人牛牛的玩法列表

    self.m_memberInfo = {}

    self.m_myClubInfo = {} --我在当前大联盟的数据

    self.m_curSelectInfo =nil

    self.m_updateCallBack = {}

    self.m_cbMsgType = 1--1,勋章,2,加入；3，退出 

    ClubFrame:getInstance().m_curQueryClub = 0
 

    self.m_curRoomMove = 0
    --游戏房间列表
    local tmp = self.m_spBg:getChildByName("content")
    self._listRoomView = cc.TableView:create(tmp:getContentSize())
    self._listRoomView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)    
    self._listRoomView:setAnchorPoint(cc.p(0,0))
    self._listRoomView:setPosition(cc.p(0,0))
    self._listRoomView:setDelegate()
    self._listRoomView:addTo(tmp)
    self._listRoomView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listRoomView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    self._listRoomView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listRoomView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listRoomView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listRoomView:reloadData()


    self.m_itemRoomFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listRoomView then
                return
            end

            -- -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listRoomView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listRoomView:getViewSize().width, self._listRoomView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                self:onItemRoomButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end

   --八人牛牛分类
    local tmp = self.m_spBg:getChildByName("fsatPanel")
    self._listOxeightView = cc.TableView:create(tmp:getContentSize())
    self._listOxeightView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)    
    self._listOxeightView:setAnchorPoint(cc.p(0,0))
    self._listOxeightView:setPosition(cc.p(0,0))
    self._listOxeightView:setDelegate()
    self._listOxeightView:addTo(tmp)
    self._listOxeightView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listOxeightView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    self._listOxeightView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listOxeightView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listOxeightView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    --self._listOxeightView:reloadData()
    self._listOxeightView:setVisible(false)


    self.m_itemOxeightFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listOxeightView then
                return
            end

            -- -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listOxeightView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listOxeightView:getViewSize().width, self._listOxeightView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                self:onItemOxeightButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end


    self:updateCurPriModeGame()

    self.m_battleGame = {145}
    self.m_curSelectGame = 1 --默认全部

    local tmp = self.m_spBg:getChildByName("gameList")
    self._listGameView = cc.TableView:create(tmp:getContentSize())
    self._listGameView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listGameView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)   
    self._listGameView:setAnchorPoint(cc.p(0,0))
    self._listGameView:setPosition(cc.p(0,0))
    self._listGameView:setDelegate()
    self._listGameView:addTo(tmp)
    self._listGameView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listGameView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    self._listGameView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listGameView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listGameView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

    self.m_itemGameFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listGameView then
                return
            end

            -- -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listGameView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listGameView:getViewSize().width, self._listGameView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                self:onItemGameButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end

    self._listGameView:reloadData()


    if self.m_lastGame ~= 0 then 
        for i =1,#self.m_tabGameList do
            if self.m_lastGame == self.m_tabGameList[i] then 
                self:onItemGameButtonClickedEvent(i)
                break
            end
        end
    end

    --当前大联盟信息
    self.m_nCurrentClubInfo = {}

    -- 事件监听
    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onUserInfoChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)

    self.m_Screenlistener = cc.EventListenerCustom:create(appdf.CLIENT_NOTIFY,handler(self, self.onScreenChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_Screenlistener, self)

    -- 注册事件监听
    --self:registerEventListenr()
    -- 加载动画
    self:noActionTransitionIn(spbg)

    self.isShowRule = false

    ClubFrame:getInstance().m_cRoomView = self
    --进入查询房间列表
    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance():sendGetTableList(0, ClubFrame:getInstance().m_nCurrentClub)



    self:onScreenChange(1)

    self:onSubGroupEnterSuccess()
end


function ClubRoomLayer:onScreenChange(event)


    local msg = yl.SCREEN_CHANGE
    local needShow = false



    if event ==1 then
        needShow = true
    elseif event and event.what == yl.CLIENT_MSG_SCREEN_CHANGE   then
        msg = tonumber(event.msg)
        needShow = true
    end

    if needShow then
        local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
        local xRate = framesize.width / yl.WIDTH 

        local targetPlatform = cc.Application:getInstance():getTargetPlatform()
        if cc.PLATFORM_OS_IPHONE == targetPlatform then
            if (framesize.width >= 2436 and framesize.height >= 1125) or (framesize.width == 1792 and framesize.height == 828) then
                local moveX = 0
                local bgImg = self.m_spBg:getChildByName("gameList"):getChildByName("Image_2")      

                if msg==2 then
                    moveX = 50*xRate
                end

                bgImg:setContentSize(cc.size(115+moveX,543))
                bgImg:setPositionX(-moveX)
                self.m_spBg:getChildByName("gameList"):setPositionX(moveX)
                self.m_spBg:getChildByName("content"):setPositionX(116+moveX)
            end
            
        end
    end
    

end




function ClubRoomLayer:onExit()
    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
        self.m_listener = nil
    end 
    if nil ~= self.m_Screenlistener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_Screenlistener)
        self.m_Screenlistener = nil
    end 

    ClubRoomLayer.super.onExit(self)
end

function ClubRoomLayer:onUserInfoChange( event  )
    print("----------ClubRoomLayer userinfo change notify------------")
    local msgWhat = event.obj
    
    print("----------userinfo change notify------------", msgWhat)
    if nil ~= msgWhat then
        if msgWhat == yl.RY_CULB_DIAMOND_NOTIFY then
            --更新钻石

        elseif msgWhat == yl.RY_CULB_MEMBER_NOTIFY then
            self:onRefreshMember()
        elseif msgWhat == yl.RY_CULB_DELETE_NOTIFY then
            local runScene = cc.Director:getInstance():getRunningScene()
            if nil ~= runScene then
                showToast(runScene, "大联盟已解散！", 2)
            end
            ClubFrame:getInstance().m_nCurrentClub = 0
            self._scene:onChangeShowMode()
        elseif msgWhat == yl.RY_CULB_OUT_NOTIFY then
            local runScene = cc.Director:getInstance():getRunningScene()
            if nil ~= runScene then
                if event.type and event.type == 0 then
                    showToast(runScene, "退出大联盟成功！", 2)
                else
                    showToast(runScene, "您已经不在该大联盟！", 2)
                end
            end
           
            ClubFrame:getInstance().m_nCurrentClub = 0
            self._scene:onChangeShowMode()

        elseif msgWhat == yl.RY_CULB_PRIROOM_HIDE then
            -- local layer = self._scene:getChildByName("ClubGameSelectLayer")
            -- if nil ~= layer then
            --     layer:removeFromParent()
            -- end
            --self.btn_create:setEnabled(true)


                local runScene = cc.Director:getInstance():getRunningScene()
            if nil ~= runScene then
            
                showToast(runScene, "哈哈哈哈！", 2)
            end

        elseif msgWhat == yl.RY_CULB_PRIROOM_CHANGETYPE then
            local gameKind = event.msg
            print("onUserInfoChange gameKind" , event.cbMedalMode)
            self:showGameConfigSetLayer(gameKind,{cbMedalMode = event.cbMedalMode,cbClubCurPlayMode = ClubFrame:getInstance().m_cbClubCurPlayMode })

        end
    end
end



function ClubRoomLayer:updateCurPriModeGame()
        print("updateCurPriModeGame--------->", ClubFrame:getInstance().m_cbClubCurPlayMode)

        self.m_tabGameList = {} 
        ClubFrame:getInstance().m_selectGameList ={}
    
        local gameList = self._scene:getApp()._gameList

        for k,v in pairs(gameList) do
            if ClubFrame:getInstance().m_cbClubCurPlayMode == 0  then
                if PriRoom:getInstance():isCurrentGameOpenPriScore(tonumber(v._KindID)) then 
                    table.insert(self.m_tabGameList, v._KindID)
                    table.insert(ClubFrame:getInstance().m_selectGameList, v)
                end
            else
                if PriRoom:getInstance():isCurrentGameOpenPriGold(tonumber(v._KindID)) then 
                    table.insert(self.m_tabGameList, v._KindID)
                    table.insert(ClubFrame:getInstance().m_selectGameList, v)
                end
            end

        end 

        --暂时开启的游戏

        local curGame = {0,33,51,56,57,59,200,302,391,399,240,389,707,241,145,61,3,239}

        for i=#self.m_tabGameList, 1,-1 do
            for k,v in pairs(curGame) do
                if v ==  self.m_tabGameList[i] then
                    break
                elseif k == #curGame then
                    table.remove(self.m_tabGameList, i)
                    table.remove(ClubFrame:getInstance().m_selectGameList, i)
                end
            end
        end


end



function ClubRoomLayer:showGameConfigSetLayer(gameKind,_param)
            if not self:checkGameUpdate(gameKind) then
                self.m_updateCallBack.enterType = 1
                self.m_updateCallBack.gameKind = gameKind
                self.m_updateCallBack.param = _param
                return
            end
            
            GlobalUserItem.nCurGameKind = gameKind
            self._scene:updateCurrentGame()
            if not PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
                showToast(self, "约战房未开启!", 1)
                return
            end

            local param = _param or {}
            param.clubID = ClubFrame:getInstance().m_nCurrentClub
            param.clubType = true

            local cl = PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_CREATEPRIROOME, self._scene, param)
            if nil ~= cl then
                PriRoom:getInstance():setViewFrame(cl)
                self._scene:addPopLayer(cl)
                cl:setName(PriRoom.NAME_CREATE_LAYER)
            end
end

-- 按键监听
function ClubRoomLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then

                -- 通知更新    
    -- local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    -- eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
    -- cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
    --     self:scaleTransitionOut(self.m_spBg)
        ClubFrame:getInstance():sendExitGroup(ClubFrame:getInstance().m_nCurrentClub)
        ClubFrame:getInstance().m_nCurrentClub = 0
        self._scene:onChangeShowMode()

    elseif TAG_ENUM.IMAGE_BG == tag then --创建
        --self.m_spMenuBg:setVisible(false)
    elseif TAG_ENUM.BTN_MESSAGE == tag then 
        local pa = ClubApplyMsgLayer:create(self._scene, {pageIndex=self.m_cbMsgType}, ClubApplyMsgLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BTN_CREATE == tag then --创建

        ClubFrame:getInstance().m_cbClubCurPlayMode = ClubFrame:getInstance().m_cbClubCurPlayMode == 0 and 1 or 0
         
        if ClubFrame:getInstance().m_cbClubCurPlayMode == 0 then
            self.btn_create:loadTextureNormal("club/scoreChange1.png")
            self.btn_create:loadTexturePressed("club/scoreChange1.png")
            self.btn_create:loadTextureDisabled("club/scoreChange1.png")
        else
            self.btn_create:loadTextureNormal("club/scoreChange2.png")
            self.btn_create:loadTexturePressed("club/scoreChange2.png")
            self.btn_create:loadTextureDisabled("club/scoreChange2.png")
        end
        cc.UserDefault:getInstance():setIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID..ClubFrame:getInstance().m_nCurrentClub.."lastMode", ClubFrame:getInstance().m_cbClubCurPlayMode)
        

        local lastGame = 0
        if self.m_curSelectGame ~= 0 and self.m_tabGameList[self.m_curSelectGame] then 
            lastGame = self.m_tabGameList[self.m_curSelectGame]
        end

        self.btn_create:setEnabled(false)
        self.btn_create:runAction(cc.Sequence:create(cc.DelayTime:create(1),cc.CallFunc:create(function ( ... )
                self.btn_create:setEnabled(true)
        end)))

            
        self:updateCurPriModeGame()

        self._listGameView:reloadData()

        if lastGame ~= 0 then 
            for i =1,#self.m_tabGameList do
                if lastGame == self.m_tabGameList[i] then 
                    self:onItemGameButtonClickedEvent(i)
                    break
                elseif i ==#self.m_tabGameList then
                    self:onItemGameButtonClickedEvent(1)
                end
            end
        end


        self:updateConfigList()





    elseif TAG_ENUM.BTN_MENU == tag then --按钮

    elseif TAG_ENUM.BTN_INFO == tag then --大联盟信息
        local pa = ClubInfoLayer:create(self._scene, self.m_nCurrentClubInfo, ClubInfoLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
        --self.m_spMenuBg:setVisible(false)
    elseif TAG_ENUM.BTN_MEMBER == tag then --大联盟成员信息
        local pa = ClubMemberLayer:create(self._scene, self.m_nCurrentClubInfo, ClubMemberLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
        --self.m_spMenuBg:setVisible(false)
    elseif TAG_ENUM.BTN_CAPTAIN == tag then --统计
        local param = {}
        param.captain = 0
        param.intype = 1
        if self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN then
                param.captain = self.m_myClubInfo.MemberInfo.dwMemberID
        end

        local pa = ClubCaptainLayer:create(self._scene, param, ClubCaptainLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
        --self.m_spMenuBg:setVisible(false)
    elseif TAG_ENUM.BT_DIAMOND == tag then --大联盟钻石
        --设置信息
        local groupProperty = self.m_nCurrentClubInfo.GroupProperty
        local isHolder = (groupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID ) and true or false
        if isHolder then
            local pa = ClubDiamond:create(self._scene, self.m_nCurrentClubInfo, ClubDiamond.SECOND_LEVEL)
            self._scene:addPopLayer(pa)
        else
            local sl = ShopLayer:create(self._scene, ShopLayer.DIAMOND_CHARGE, ShopLayer.SECOND_LEVEL)
            self._scene:addPopLayer(sl)
        end
        --self.m_spMenuBg:setVisible(false)
    elseif TAG_ENUM.BTN_CHECK == tag then --查询战绩

        --self.m_spMenuBg:setVisible(false)
        local pa = ClubMyRecordLayer:create(self._scene, {}, ClubMyRecordLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BTN_RULESET == tag then --玩法设置

        self:showRuleSet()
    elseif TAG_ENUM.BTN_BROADCAST == tag or TAG_ENUM.BTN_CLUBAD == tag then
        local msgType = TAG_ENUM.BTN_BROADCAST == tag and 1 or 2
        local pa = ClubNoticeLayer:create(self._scene, {_msgType = msgType}, ClubNoticeLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif TAG_ENUM.BTN_SCORE == tag then
        local pa = ClubMemberScore:create(self._scene, {}, ClubMemberScore.SECOND_LEVEL)
        self._scene:addPopLayer(pa)

    elseif TAG_ENUM.BTN_SET == tag then
        local pa = ClubSetLayer:create(self._scene, {}, ClubSetLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)      
    
       --self:updateFastBtn(true)
    elseif TAG_ENUM.BTN_SHARE == tag then   
        -- local param ={}
        --         param.title = "【大联盟ID:"..ClubFrame:getInstance().m_nCurrentClub.."】"
        --         param.content = "我在大联盟游戏，等你来参加！"

        local pa = ClubSpreadLayer:create(self._scene, {}, ClubSpreadLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)      
    elseif TAG_ENUM.BTN_SCOREBOX == tag then 
        local pa = ClubScoreBox:create(self._scene, {}, ClubScoreBox.SECOND_LEVEL)
        self._scene:addPopLayer(pa)         
    end
end


function ClubRoomLayer:updateRollMsg(isNew)
     --dump(ClubFrame:getInstance().m_tabClubRollMsg,"===========")
     local curMsg = {}
     if self.m_curRollIndex ==0 then
        for k,v in pairs(ClubFrame:getInstance().m_tabClubRollMsg) do
            if v and v.cbFlag ==1 and v.szRollMsg ~= "" then
                self.m_curRollIndex = k
                curMsg = v
                break
            end
        end
        if self.m_curRollIndex == 0 then
            return
        end
     else
        if isNew then
            return --新广播来时，不打断当前广播
        end
        self.m_curRollIndex = self.m_curRollIndex +1
        if self.m_curRollIndex <= 3 then --只有三种类型的广播
            curMsg = ClubFrame:getInstance().m_tabClubRollMsg[self.m_curRollIndex]

            if not curMsg or curMsg.cbFlag ==0 or curMsg.szRollMsg == "" then
                local isNotice = false
                for k,v in pairs(ClubFrame:getInstance().m_tabClubRollMsg) do
                    if v and v.cbFlag ==1 and v.szRollMsg ~= "" then
                        isNotice = true --有广播可以播
                        break
                    end
                end
                if isNotice then
                    self:updateRollMsg()
                else
                    self.m_curRollIndex = 0
                end
                return
            end
        else
            self.m_curRollIndex = 0
            local delayTime = 0
            for k,v in pairs(ClubFrame:getInstance().m_tabClubRollMsg) do
                if v  then
                    delayTime = v.nTime
                    break
                end
            end
            self.m_notice:runAction(cc.Sequence:create(cc.DelayTime:create(delayTime),cc.CallFunc:create(function ( ... )
                self:updateRollMsg()
            end)))
            return
        end
     end

    self.m_notice:setString(curMsg.szRollMsg.."")
    self.m_notice:stopAllActions()

    local time = math.floor((self.m_notice:getContentSize().width+490)/70)

    self.m_notice:setVisible(true)
    self.m_notice:setPositionX(493)
    self.m_notice:runAction(cc.Sequence:create(cc.MoveTo:create(time,cc.p(-self.m_notice:getContentSize().width,2)),
        cc.CallFunc:create(function ( ... )
            self.m_notice:setPositionX(491)
            self:updateRollMsg()
        end)))

end

function ClubRoomLayer:updateFastBtn(isBegan)

    --dump(self.m_myClubInfo,"=======================")
    -- if self.m_myClubInfo.MemberInfo.dwFavoriteConfig ==0 then
    --     self.btn_create:setVisible(true)
    --     self.btn_fastBegan:setVisible(false)
    --     self.btn_changeConfig:setVisible(false)
    -- else
    --     self.btn_create:setVisible(false)
    --     self.btn_fastBegan:setVisible(true)
    --     self.btn_changeConfig:setVisible(true)
    --     for k,v in pairs(self.m_configList) do
    --         if v and v.dwConfigID == self.m_myClubInfo.MemberInfo.dwFavoriteConfig then
    --             self.btn_fastBegan:getChildByName("Text_2"):setString(v.szConfigName.."")
    --         end
    --     end
    -- end

    -- --设置玩法之后，直接开始
    -- if isBegan then
    --     for k,v in pairs(self.m_configList) do
    --         if v and v.dwConfigID == self.m_myClubInfo.MemberInfo.dwFavoriteConfig then

    --             self:onItemRoomButtonClickedEvent( nil, {roomInfo = v} )

    --             break
    --         end
    --     end
    -- end


end

--是否是百人游戏
function ClubRoomLayer:isBattleGame(dwKindID)
   
    for k,v in pairs(self.m_battleGame) do
        if dwKindID == v then
            return true
        end
    end
    return false
end



function ClubRoomLayer:animationRemove()
   --self:scaleTransitionOut(self.m_spBg)
end

function ClubRoomLayer:onTransitionInFinish()
 
end

function ClubRoomLayer:onTransitionOutFinish()
    self:removeFromParent()

end

function ClubRoomLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function ClubRoomLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubRoomLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end





function ClubRoomLayer:tableCellTouched(view, cell)
    -- if self._listRoomView == view then
    --     local index = cell:getIdx()
    --     print("tableCellTouched", index)
    --     local info = self.m_roomList[index +1]
    --     if nil == info then
    --         return
    --     end
    --     --请求桌子玩家信息
    --     self.m_nCurrentRoomInfo.Roominfo = info

    --     ClubFrame:getInstance():setViewFrame(self)
    --     ClubFrame:getInstance():sendGetTableUserList(ClubFrame:getInstance().m_nCurrentClub, info.dwPersonalRoomID)
    -- end
end


function ClubRoomLayer:scrollViewDidScroll(view)

    if view ==self._listRoomView then
       local point = view:getContentOffset()
            if math.abs(point.x)>=1200 then
                self.scrollBg:setPositionX(667+ 103*point.x/math.abs(point.x))
            else
                self.scrollBg:setPositionX(667+point.x*103/1200)
            end

        self.m_curRoomMove = math.abs(point.x)
        
    end

end



-- 子视图大小
function ClubRoomLayer:cellSizeForTable(view, idx)
    if view ==self._listRoomView then
        local dwKindID = self.m_tabGameList[self.m_curSelectGame];

        if self:isBattleGame(dwKindID) then
            return 609 , 513
        else
            return 406 , 513
        end
        
    elseif view ==self._listRule then
        return 607 , 139
    elseif view ==self._listGameView then
        return 108 , 92
    elseif view ==self._listOxeightView then
        return 200 , 48
    end
end

function ClubRoomLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    if view == self._listRoomView then

        local dwKindID = self.m_tabGameList[self.m_curSelectGame];
        
        if self:isBattleGame(dwKindID) then
            local roomInfo = {}
            local roomType =1  --1，玩法桌子；2，房间桌子
            if (idx + 1 ) <=  #self.m_sGameConfigList  then
                roomType =1
                roomInfo = self.m_sGameConfigList[idx +1]
            else
                roomType =2
                roomInfo = self.m_sGameRoomList[idx +1  - #self.m_sGameConfigList]
            end
            roomInfo.roomType = roomType

            local ClubDeskItem = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.game.ClubDeskItem_" .. dwKindID);
            local deskItem = ClubDeskItem:create(self, roomInfo, idx);
            deskItem:setPosition(304, 260+15)
            cell:addChild(deskItem);
           
            local btn = deskItem._csbNode:getChildByName("Image_2")
            btn:setSwallowTouches(false)
            btn:addTouchEventListener(self.m_itemRoomFunC)
            btn.roomInfo =roomInfo
            return cell;
        end

        for i=1,2 do
            local index = idx*2 +i
            if (idx*2 +i) > (#self.m_sGameRoomList + #self.m_sGameConfigList) then
                break 
            end

            local roomInfo = {}
            local roomType =1  --1，玩法桌子；2，房间桌子
            if (idx*2 +i) <= #self.m_sGameConfigList  then
                roomType =1
                roomInfo = self.m_sGameConfigList[idx*2 +i]
            else
                roomType =2
                roomInfo = self.m_sGameRoomList[idx*2 +i - #self.m_sGameConfigList]
            end

            roomInfo.roomType = roomType

            local gameType = 1
            for k,v in pairs(majongGame) do
                if roomInfo.dwKindID == v  then
                    gameType = 2
                    break
                end
            end


            local csb = ExternalFun.loadCSB("club/club_Desk.csb", cell)
            csb:setPosition(203, 385-(i-1)*255)

            local btn = csb:getChildByName("Image_2")
            btn:setSwallowTouches(false)
            btn:addTouchEventListener(self.m_itemRoomFunC)
            btn.roomInfo =roomInfo

            local bg = csb:getChildByName("Image_1")
            
            local bgImg = "club/deskBg"..gameType.."_"..roomInfo.dwKindID..".png"
            if cc.FileUtils:getInstance():isFileExist(bgImg) then
                bg:loadTexture(bgImg)
            end

            if roomType == 1 then


                local btn = csb:getChildByName("fastBtn")
                btn:setSwallowTouches(true)
                btn:setVisible(true)
                btn:setEnabled(true)
                btn:addTouchEventListener(function (ref, tType)  
                        if tType == ccui.TouchEventType.ended then
                            local beginPos = ref:getTouchBeganPosition()
                            local endPos = ref:getTouchEndPosition()
                            if math.abs(endPos.x - beginPos.x) > 30 
                                or math.abs(endPos.y - beginPos.y) > 30
                                or nil == self._listRoomView then
                                return
                            end

                            PriRoom:getInstance():showPopWait()
                            ClubFrame:getInstance():setViewFrame(self)
                            ClubFrame:getInstance():sendQueryCreate(roomInfo.dwGroupID,roomInfo.dwConfigID)
                            -- btn:setEnabled(false)
                            -- self:runAction(cc.Sequence:create(cc.DelayTime:create(2),cc.CallFunc:create(function ( )
                            --     btn:setEnabled(true)
                            -- end)))
                        end
                end)

            end


            local playeCount = roomInfo.wJoinGamePeopleCount

            local infoNode = csb:getChildByName("Node_1")

            local name = infoNode:getChildByName("tableDes")
            name:setString(roomInfo.szConfigName.."")

            local count = infoNode:getChildByName("jushu")
            count:setString(roomInfo.dwDrawCountLimit.."局")

            if roomInfo.dwKindID ==3 or self:isBattleGame(roomInfo.dwKindID) then
                count:setString(roomInfo.dwDrawTimeLimit.."分钟")
            end

            --牌九加锅玩法显示
            if roomInfo.dwKindID == 59 and roomInfo.cbGameRule[1][4] == 1 then
                count:setString("加锅")
            end

            local num = infoNode:getChildByName("renshu")
            num:setString(roomInfo.lCellScore.."底")

            if ClubFrame:getInstance().m_cbClubCurPlayMode == 0 then
                num:setString(roomInfo.wJoinGamePeopleCount.."人")
            end
            

            local wanfa = infoNode:getChildByName("wanfaText")
            wanfa:setString("玩法"..((idx*2 +i)>=10 and (idx*2 +i) or "0"..(idx*2 +i)))     

            if roomType == 2 then
                --dump(self.m_sGameRoomList,"==")
                for k,v in pairs(self.m_sGameConfigList) do
                    if v and v.dwConfigID == roomInfo.dwConfigID then
                        wanfa:setString("玩法"..(k>= 10 and k or "0"..k))
                    end
                end
            end

            for j=1,playeCount do
                local head = csb:getChildByName("head"..j)
                head:setPosition(PlayerChairPoint[playeCount][j])
                head:setVisible(true)
            end
            if roomType == 2 then 
                    if roomInfo.bGameStart then
                        local status = csb:getChildByName("Sprite_2")
                        status:setVisible(true)
                        status:setTexture("club/playingStutas"..gameType..".png")
                        infoNode:setVisible(false)
                        csb:getChildByName("playing"):setVisible(true)
                    end
                    
                    for j=1,9 do --最大16个椅子
                                local head = csb:getChildByName("head"..j)
                                if roomInfo.dwPlayerUserId[1][j] ~= 0 and j<= playeCount  then
                                        local node =head:getChildByName("head")
                                        head:getChildByName("Sprite_4"):setVisible(false)
                                        local iteminfo = {}
                                        iteminfo.dwUserID = roomInfo.dwPlayerUserId[1][j]
                                        --头像
                                        -- local head = PopupInfoHead:createClipHead(iteminfo, 95,"club/Sprite_head1.png")
                                        -- head:enableInfoPop(false)
                                        -- head:setScale(0.5)
                                        --node:addChild(head)
                                        -- head:setPosition(0,0)
                                        -- head:setName("_head_")

                                        local data, sign = GlobalUserItem:useridSign(iteminfo.dwUserID)
                                        local action = string.format("action=GetUserInfo&userid=%s&time=%s&sign=%s", iteminfo.dwUserID, data, sign)
                                        
                                        appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
                                           -- dump(jstable, "获取玩家信息", 7)
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

                                                        if node and tolua.cast(node,"cc.Node")then
                                                            node:removeChildByName("_head_")
                                                            local head = PopupInfoHead:createClipHead(iteminfo, 99,"club/Sprite_head1.png")
                                                            head:enableInfoPop(false)
                                                            head:setScale(0.51)
                                                            node:addChild(head)
                                                            head:setPosition(0,0)
                                                            head:setName("_head_")
                                                            --node:getChildByName("name"):setString(iteminfo.szNickName or "" )
                                                        end
                                                    end
                                                end
                                            end
                                        end)
                                else
                                    --if node and tolua.cast(node,"cc.Node")then
                                        --node:setVisible(false)
                                    --end
                                end
                     end
             end


        end


    elseif view ==self._listRule then

        local configInfo = self.m_configList[idx+1]

        local csb = ExternalFun.loadCSB("club/node_wfcell.csb", cell)
        csb:setPosition(0, 2)

        if not configInfo then
            csb:getChildByName("Image_1"):setVisible(false)
            csb:setVisible(self._configEditShow)
            local btn = csb:getChildByName("btn_create")
            btn:setPressedActionEnabled(false)
            btn:setSwallowTouches(false)
            btn:setVisible(true)
            btn:addTouchEventListener(function (ref, tType)  
                    if tType == ccui.TouchEventType.ended then
                        local beginPos = ref:getTouchBeganPosition()
                        local endPos = ref:getTouchEndPosition()
                        if math.abs(endPos.x - beginPos.x) > 30 
                            or math.abs(endPos.y - beginPos.y) > 30
                            or nil == self._listRule then
                            return
                        end

                        local toViewPoint = self._listRule:convertToNodeSpace(endPos)
                        local rec = cc.rect(0, 0, self._listRule:getViewSize().width, self._listRule:getViewSize().height)
                        if true == cc.rectContainsPoint(rec, toViewPoint) then
                                local param = {}
                                param.clubID = ClubFrame:getInstance().m_nCurrentClub
                                param.cbPayTypeMask = self.m_nCurrentClubInfo.GroupBattleRule.cbPayTypeMask
                                param.cbMedalMode = self.m_nCurrentClubInfo.GroupBattleRule.cbMedalMode

                                local pa = ClubGameSelectLayer:create(self._scene, param, ClubGameSelectLayer.SECOND_LEVEL)

                                self._scene:addPopLayer(pa)
                        end  
                    end
            end)

            return cell
        end
        local btn = csb:getChildByName("btn_edit")
        btn:setPressedActionEnabled(false)
        btn:setSwallowTouches(false)
        btn:setVisible(self._configEditShow)
        btn.tag = idx+1
        btn:addTouchEventListener(function (ref, tType)  
                if tType == ccui.TouchEventType.ended then
                    local beginPos = ref:getTouchBeganPosition()
                    local endPos = ref:getTouchEndPosition()
                    if math.abs(endPos.x - beginPos.x) > 30 
                        or math.abs(endPos.y - beginPos.y) > 30
                        or nil == self._listRule then
                        return
                    end

                    self:modifyConfig(configInfo,self.m_nCurrentClubInfo.GroupBattleRule.cbMedalMode)
                end
        end)

        if self:isBattleGame(configInfo.dwKindID) then
            btn:setVisible(false)
        end


        local editHanlder = function(event,editbox)
            self:onEditEvent(event, editbox)
        end

        local editbox = ccui.EditBox:create(cc.size(160, 30),"public/public_blank.png")
                :setPosition(cc.p(185,28))
                :setFontColor(cc.c3b(216, 216, 216))
                :setAnchorPoint(cc.p(0,0.5))  
                :setPlaceHolder("")
                :setPlaceholderFont("fonts/round_body.ttf", 24)
                :setFont("fonts/round_body.ttf", 24) 
                --:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
                :setMaxLength(6)
                :setLocalZOrder(-1)
        csb:addChild(editbox)         
        --self.m_editBox = editbox
        editbox:registerScriptEditBoxHandler(editHanlder)
        editbox:setText(configInfo.szConfigName)

        local editText = csb:getChildByName("Text_9_1")
        editText:setString(configInfo.szConfigName.."")
        if configInfo.szConfigName == "" then
            editText:setString(ExternalFun.getGameNameByKind(configInfo.dwKindID).."")
        end

        editBtn = csb:getChildByName("btn_editName")
        editBtn:setSwallowTouches(false)
        editBtn:setVisible(self._configEditShow)
        editBtn:addTouchEventListener(function (ref, tType)  
                if tType == ccui.TouchEventType.ended then
                        self.m_curConfig = clone(configInfo)

                        self.m_editText = editText
                        self.m_editBtn = editBtn

                        --self.m_editBox:setText(configInfo.szConfigName)
                        editbox:touchDownAction(editbox, ccui.TouchEventType.ended)
                        editBtn:setEnabled(false)
                end
        end)


        local jushu = csb:getChildByName("jushu")
        jushu:setString(configInfo.dwDrawCountLimit.."局")


        if configInfo.dwKindID ==3 or self:isBattleGame(configInfo.dwKindID) then
                csb:getChildByName("Text_9"):setString("时间:")
                jushu:setString(configInfo.dwDrawTimeLimit.."分钟")
        end

        local renshu = csb:getChildByName("renshu")
        renshu:setString(configInfo.wJoinGamePeopleCount.."")

        local gName = csb:getChildByName("gName")
        gName:setString(ExternalFun.getGameNameByKind(configInfo.dwKindID).."")


        local wanfa = csb:getChildByName("wanfa")
        wanfa:setString("玩法"..(idx+1 >=10 and idx+1 or "0"..(idx+1)))

        local gameImg = csb:getChildByName("game")
        local filePath = "club/configGame_"..configInfo.dwKindID..".png"
        if cc.FileUtils:getInstance():isFileExist(filePath) then
            gameImg:setTexture(filePath)
            gameImg:setScale(0.8)
        end



    elseif view ==self._listGameView then 
        local gameKind = self.m_tabGameList[idx+1]
        if not gameKind then
            return 
        end

        local file = "club/gameList_"..gameKind.."_"..(self.m_curSelectGame==idx+1 and 1 or 2)..".png"
        if not cc.FileUtils:getInstance():isFileExist(file) then
                file = "club/gameList_200_"..(self.m_curSelectGame==idx+1 and 1 or 2)..".png"
        end
        local gameIcon = ccui.Button:create(file,file, file)
        gameIcon:setAnchorPoint(cc.p(0,0))
        gameIcon:setPosition(0, 4)
        gameIcon:setPressedActionEnabled(false)
        gameIcon:setSwallowTouches(false)
        gameIcon:addTouchEventListener(self.m_itemGameFunC) 
        gameIcon:setTag(idx+1)
        gameIcon:addTo(cell)

        local selectFlag =cc.Sprite:create("club/gameSelect.png"):move(98,72):addTo(cell)
        selectFlag:setVisible(self.m_curSelectGame==idx+1)
    elseif view ==self._listOxeightView then


        local configInfo = self.m_oxeightConfigList[idx]

        if idx == 0 then
            configInfo = {}
            configInfo.dwConfigID = 0
        end

        local csb = ExternalFun.loadCSB("club/node_fasticon.csb", cell)
        csb:setAnchorPoint(cc.p(0,0))
        csb:setPosition(12, 0)


        local btn = csb:getChildByName("Button_1")
        btn:setSwallowTouches(false)
        btn:addTouchEventListener(self.m_itemOxeightFunC)
        btn.configInfo =configInfo


        if self.m_OxeightCurIndex == configInfo.dwConfigID  then
            
            btn:loadTextureNormal("club/fastBtn4.png")
            btn:loadTexturePressed("club/fastBtn4.png")
        end


        local txt = csb:getChildByName("Text_1")

        if idx == 0 then
            txt:setString("全部")
        else
            local bankerMode = configInfo.cbGameRule[1][7]



            local szbankerMode=""
            if bankerMode == 52 then
                szbankerMode = "霸王庄"
            elseif bankerMode == 53 then
                szbankerMode = "明牌抢庄"  
            elseif bankerMode == 54 then
                szbankerMode = "牛牛上庄"
            elseif bankerMode == 55 then 
                szbankerMode = "无牛下庄"
            elseif bankerMode == 56 then 
                szbankerMode = "自由抢庄"
            elseif bankerMode == 57 then 
                szbankerMode = "通比玩法"
            end

            txt:setString(szbankerMode..configInfo.lCellScore.."底")
        end

    end
        

    return cell
end


function ClubRoomLayer:onEditEvent(event, editbox)

    if not tolua.cast(self.m_editText,"cc.Node") or not tolua.cast(self.m_editBtn,"cc.Node") then

        return
    end

    local gameName = ExternalFun.getGameNameByKind(self.m_curConfig.dwKindID)..""

    local src = editbox:getText() 
        if event == "return" then
            self.m_editBtn:setEnabled(true)
            self.m_editText:setString(src)
            local configName = src
            if self.m_editText:getContentSize().width == 0 then --没有输入
                self.m_editText:setString(gameName)
                configName=gameName
            end

            if true == ExternalFun.isContainBadWords(src) then
                showToast(self, "玩法名包含敏感词汇!", 2)
                return
            end 

            self.m_editText:setVisible(true)
            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3),cc.CallFunc:create(function ( ... )
                self.m_bBeginEdit = false
            end)))

            if self.m_curConfig.szConfigName == configName then
                return 
            end
            self.m_curConfig.szConfigName = configName
            ClubFrame:getInstance():sendModifyConfigName(self.m_curConfig)



        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= gameName then
                self.m_editText:setString(src)
            else
                self.m_editText:setString(gameName)
            end

        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= gameName then
                self.m_editText:setString(src)
            else
                self.m_editText:setString(gameName)
            end
            self.m_editText:setVisible(false)
        end
end

-- 子视图数目
function ClubRoomLayer:numberOfCellsInTableView(view)
    if view ==self._listRoomView then
        if not self.m_curSelectGame then
            return
        end
        local dwKindID = self.m_tabGameList[self.m_curSelectGame];
        if self:isBattleGame(dwKindID) then
            return math.ceil((#self.m_sGameRoomList + #self.m_sGameConfigList)/1)
        else
            return math.ceil((#self.m_sGameRoomList + #self.m_sGameConfigList)/2)
        end
           
    elseif view ==self._listRule then
        return #self.m_configList +1 or 1
    elseif view ==self._listGameView then
        return #self.m_tabGameList or 0
    elseif view ==self._listOxeightView then
        return #self.m_oxeightConfigList +1
    end
end



function ClubRoomLayer:onItemRoomButtonClickedEvent( tag, ref )
    local Roominfo = ref.roomInfo
    if nil == Roominfo then
        print("大联盟信息为空")
        return
    end
    --dump(Roominfo, "大联盟信息为空", 6)

    --PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(Roominfo.dwPersonalRoomID))

    --进入房间，断掉大联盟连接http://g.click.taobao.com/q?pid=mm_40344568_24672213_83172933&rf=http%3A%2F%2Ftao.duba.com%2Fminisite%2F1611%2Findex.html%3Fdpi%3D100%26t%3D1526523&rd=2&et=92763423&pgid=9274e1d38444e0c530093fc83dc46e18&ct=url%3Dhttp%253A%252F%252Fitem.taobao.com%252Fitem.htm%253Fid%253D566759434130&v=1.2&ttype=1&cm=835%2C269&ck=&cw=0
    --ClubFrame:getInstance():reSetAndDisconnect()

        if not self:checkGameUpdate(Roominfo.dwKindID) then
                self.m_updateCallBack.enterType = 2
                self.m_updateCallBack.roominfo = Roominfo
                return
        end

        self:showEnterLayer(Roominfo)


    -- if Roominfo.roomType ==2  then
    --     PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(Roominfo.dwPersonalRoomID))
    -- else
    --     ClubFrame:getInstance():setViewFrame(self)
    --     ClubFrame:getInstance():sendQueryCreate(Roominfo.dwGroupID,Roominfo.dwConfigID)
    -- end

end

function ClubRoomLayer:onItemOxeightButtonClickedEvent( tag, ref )

    local configInfo = ref.configInfo
    if nil == configInfo then
        print("玩法信息为空")
        return
    end

    self.m_OxeightCurIndex = configInfo.dwConfigID

    cc.UserDefault:getInstance():setIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID.."oxeightId", self.m_OxeightCurIndex)

    self:updateConfigList()
    self._listOxeightView:reloadData()

end

function ClubRoomLayer:onItemGameButtonClickedEvent( tag, ref )

    cc.UserDefault:getInstance():setDoubleForKey(GlobalUserItem.tabAccountInfo.dwUserID.."lastGame",self.m_tabGameList[tag] )

    local lastGame = self.m_curSelectGame
    self.m_curSelectGame = tag
    self._listGameView:updateCellAtIndex(lastGame -1)
    self._listGameView:updateCellAtIndex(self.m_curSelectGame-1)
    self:updateConfigList()
end


function ClubRoomLayer:onGetCreateInfo(bCanCreate,dwConfigID,dwRoomID)
    --dump({bCanCreate,dwConfigID,dwRoomID})
    if bCanCreate then
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom(dwConfigID)
    else
        PriRoom:getInstance():getNetFrame():onSearchRoom(dwRoomID)
    end
end

function ClubRoomLayer:onItemRoomInfoButtonClickedEvent( tag, ref )
    local Roominfo = ref.Roominfo
    if nil == Roominfo then
        print("大联盟信息为空")
        return
    end
    --请求桌子玩家信息
    self.m_nCurrentRoomInfo.Roominfo = Roominfo

    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance():sendGetTableUserList(ClubFrame:getInstance().m_nCurrentClub, Roominfo.dwPersonalRoomID)
   
end

function ClubRoomLayer:onItemRoomDeleteButtonClickedEvent( tag, ref )
    local Roominfo = ref.Roominfo
    if nil == Roominfo then
        print("大联盟信息为空")
        return
    end
    --删除房间
    -- 记录标识
    if self._scene then
        self._scene:dismissPopWait()
        self.m_Query = QueryDialog:create("是否确认删除房间?", function(ok)
            if ok then
                --删除成员
                GlobalUserItem.nCurGameKind = Roominfo.dwKindID
                PriRoom:getInstance():showPopWait()
                PriRoom:getInstance():getNetFrame():onDissumeRoom(Roominfo.dwPersonalRoomID)
            end
            self.m_Query = nil
        end, nil, QueryDialog.QUERY_SURE_CANCEL)
        :setCanTouchOutside(false)
        :addTo(self._scene)
    end

end



local TAG_START1             = 100
local enumRuleTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_ADD",           -- 编辑
    "BTN_SERCH",             -- 搜索

}
local TAG_RENUM1 = ExternalFun.declarEnumWithTable(TAG_START1, enumRuleTable)

function ClubRoomLayer:showRuleSet()

    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_wanfaedit.csb", self)
        local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onRuleButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._ruleLayer = rootLayer

    self._configEditShow = false

    if ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT7) then
        self._configEditShow = true
    end

    if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID then
        self._configEditShow = true
    end



    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_RENUM1.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    local bg = csbNode:getChildByName("bg")

       --玩法列表
    local tmp = bg:getChildByName("Panel_2")
    self._listRule = cc.TableView:create(tmp:getContentSize())
    self._listRule:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listRule:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listRule:setPosition(cc.p(0,0))
    self._listRule:setAnchorPoint(cc.p(0,0))
    self._listRule:setDelegate()
    self._listRule:addTo(tmp)
    self._listRule:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listRule:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listRule:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listRule:reloadData()

    self.m_emptyTip2 = bg:getChildByName("tip")

    if #self.m_configList==0 then
        self.m_emptyTip2:setVisible(true)
    else
        self.m_emptyTip2:setVisible(false)  
    end

    self.m_curConfig = nil

    local add = bg:getChildByName("btn_add")
    add:setTag(TAG_RENUM1.BTN_ADD)
    add:addTouchEventListener( touchFunC )
    add:setVisible(false)



end


function ClubRoomLayer:onRuleButtonClickedEvent(tag,ref)

    if tag == TAG_RENUM1.TAG_MASK  then
        self._ruleLayer:removeFromParent()
    elseif tag == TAG_RENUM1.BTN_ADD  then

        local param = {}
        param.clubID = ClubFrame:getInstance().m_nCurrentClub
        param.cbPayTypeMask = self.m_nCurrentClubInfo.GroupBattleRule.cbPayTypeMask
        param.cbMedalMode = self.m_nCurrentClubInfo.GroupBattleRule.cbMedalMode

        local pa = ClubGameSelectLayer:create(self._scene, param, ClubGameSelectLayer.SECOND_LEVEL)

        self._scene:addPopLayer(pa)
    end

end



local TAG_START2             = 100
local enumRuleTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_ENTER",           -- 编辑
    "BTN_SHARE",             -- 搜索
    "BTN_DISMISS",             -- 解散    
    "BTN_LOOK",--旁观

}
local TAG_RENUM2 = ExternalFun.declarEnumWithTable(TAG_START2, enumRuleTable)

function ClubRoomLayer:showEnterLayer(param)

    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_editName.csb", self)
        local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onEnterButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._enterLayer = rootLayer
    self._enterGame = param
    self._enterNode = csbNode

    local bg = csbNode:getChildByName("bg")
    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_RENUM2.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    local close = bg:getChildByName("btn_close")
    close:setTag(TAG_RENUM2.BTN_CLOSE)
    close:addTouchEventListener( touchFunC )


    local btnTab = {}

    local lookOn = bg:getChildByName("btn_look")
    lookOn:setTag(TAG_RENUM2.BTN_LOOK)
    lookOn:addTouchEventListener( touchFunC )
    lookOn:setVisible(false)    

    if self._enterGame.roomType ==2 then
        if not self:isBattleGame(self._enterGame.dwKindID) then
            lookOn:setVisible(true)
            table.insert(btnTab,lookOn)
        end
        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendQueryTableBattleInfo(param.dwGroupID,param.dwServerID,param.wTableID)
    end

    local dismiss = bg:getChildByName("btn_dis")
    dismiss:setTag(TAG_RENUM2.BTN_DISMISS)
    dismiss:addTouchEventListener( touchFunC )
    dismiss:setVisible(false)

    if self._enterGame.roomType ==2 then
        if ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT7)
        or self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID then
                dismiss:setVisible(true)
                table.insert(btnTab,dismiss)
        end
    end

    local rule = bg:getChildByName("Text_10")
    rule:setString("基本玩法")
    
    dump(self._enterGame.cbGameRule[1])
    local tabRule = self:getGameRule(self._enterGame.cbGameRule[1], self._enterGame.dwKindID)
    local str = ExternalFun.getGameNameByKind(self._enterGame.dwKindID)..","..self._enterGame.dwDrawCountLimit.."局,"
    if self:isBattleGame(self._enterGame.dwKindID) or self._enterGame.dwKindID == 3 then
        str = ExternalFun.getGameNameByKind(self._enterGame.dwKindID)..","..self._enterGame.dwDrawTimeLimit.."分钟,"
    end
    --牌九加锅玩法显示
    if self._enterGame.dwKindID == 59 and self._enterGame.cbGameRule[1][4] == 1 then
        str = ExternalFun.getGameNameByKind(self._enterGame.dwKindID)..",";
    end
    if tabRule then
            for j=1, #tabRule do
                if j == #tabRule then
                    str = str ..  tabRule[j]
                else
                    str = str ..  tabRule[j] .. ","
                end
            end
    else
        str=str.."基本玩法"
    end

    rule:setString(str)


    local share = bg:getChildByName("btn_add")
    share:setTag(TAG_RENUM2.BTN_SHARE)
    share:addTouchEventListener( touchFunC )
    share.rule = rule:getString()
    table.insert(btnTab,share)


    local enter = bg:getChildByName("btn_enter")
    enter:setTag(TAG_RENUM2.BTN_ENTER)
    enter:addTouchEventListener( touchFunC )
    table.insert(btnTab,enter)



    local title = bg:getChildByName("Text_2")
    title:setString(self._enterGame.szConfigName.."")

    local game = bg:getChildByName("gameName")
    game:setString(ExternalFun.getGameNameByKind(self._enterGame.dwKindID).."")


    local num = bg:getChildByName("playerCount")
    num:setString("0/"..self._enterGame.wJoinGamePeopleCount)

    if self._enterGame.roomType ==2 then
            num:setString(self._enterGame.wCurSitPlayer.."/"..self._enterGame.wJoinGamePeopleCount)
    end

    local pointX = {{},{325,280},{184,281},{184,187}} --起点，间隔

    for i=1,#btnTab do
        btnTab[i]:setPositionX(pointX[#btnTab][1]+(i-1)*pointX[#btnTab][2])
    end
    for i=1,self._enterGame.wJoinGamePeopleCount do
        local ScrollView =  bg:getChildByName("ScrollView")
        local sitPlayer =  ScrollView:getChildByName("sit"..i)
        if not sitPlayer then
            return
        end
        sitPlayer:setVisible(true)
        local sitBg = sitPlayer:getChildByName("noPlayer")
        sitBg:setVisible(true)
        local player = sitPlayer:getChildByName("player")
        player:setVisible(false)
        if self._enterGame.roomType ==2 and self._enterGame.dwPlayerUserId[1][i] ~= 0 then
            sitBg:setVisible(false)
            player:setVisible(true)
            local node =player:getChildByName("Image_4")
            local iteminfo = {}
            iteminfo.dwUserID = self._enterGame.dwPlayerUserId[1][i]

            local data, sign = GlobalUserItem:useridSign(iteminfo.dwUserID)
            local action = string.format("action=GetUserInfo&userid=%s&time=%s&sign=%s", iteminfo.dwUserID, data, sign)
            print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
            local jsonStr = cjson.encode(action)
            LogAsset:getInstance():logData(jsonStr, true)
            appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
               -- dump(jstable, "获取玩家信息", 7)
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

                            if node and tolua.cast(node,"cc.Node")then
                                node:removeChildByName("_head_")
                                local head = PopupInfoHead:createClipHead(iteminfo, 120,"club/enterHead2.png")
                                head:enableInfoPop(false)
                                head:setScale(0.5)
                                node:addChild(head)
                                head:setAnchorPoint(cc.p(0.5,0.5))
                                head:setPosition(node:getContentSize().width/2,node:getContentSize().height/2)
                                head:setName("_head_")
                                player:getChildByName("gameN"):setString(ExternalFun.clipText(iteminfo.szNickName,5,true))--
                            end
                        end
                    end
                end
            end)

        end
         
    end

end


function ClubRoomLayer:updateTableBattleInfo(data)

    if  self._enterNode and tolua.cast(self._enterNode,"cc.Node") then
        for i=1,self._enterGame.wJoinGamePeopleCount do
            local ScrollView =  self._enterNode:getChildByName("bg"):getChildByName("ScrollView")
            local sitPlayer =  ScrollView:getChildByName("sit"..i)

            if not sitPlayer then
                return
            end
            local score = sitPlayer:getChildByName("player"):getChildByName("score")
            score:setString(data.lGameScore[1][i].."")
        end   
    end

end



function ClubRoomLayer:onEnterButtonClickedEvent(tag,ref)
    local Roominfo = self._enterGame
    if tag == TAG_RENUM2.TAG_MASK  or tag == TAG_RENUM2.BTN_CLOSE  then
        if tolua.cast(self._enterLayer,"cc.Node") then 
            self._enterLayer:removeFromParent()
        end
    elseif tag == TAG_RENUM2.BTN_ENTER  then

        self._enterLayer:removeFromParent()

        if Roominfo.roomType ==2  then
             PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(Roominfo.dwPersonalRoomID))
        else
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendQueryCreate(Roominfo.dwGroupID,Roominfo.dwConfigID)
        end

    elseif tag == TAG_RENUM2.BTN_SHARE  then
        local param ={}
        param.title = "【大联盟ID:"..ClubFrame:getInstance().m_nCurrentClub.."】"
        param.content = Roominfo.szConfigName..","..ref.rule..",一起来玩！"

        local pa = ClubShareLayer:create(self._scene, param, ClubShareLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)   

    elseif tag == TAG_RENUM2.BTN_DISMISS  then
        ClubFrame:getInstance():sendDismissTable(ClubFrame:getInstance().m_nCurrentClub,tonumber(Roominfo.dwPersonalRoomID))
        self._enterLayer:removeFromParent()

    elseif tag == TAG_RENUM2.BTN_LOOK  then
        self._enterLayer:removeFromParent()
        PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(Roominfo.dwPersonalRoomID),true)

    end

end




function ClubRoomLayer:modifyConfig(configInfo,cbMedalMode)
        local param = {}
        param.tabConfigInfo = configInfo
        param.cbMedalMode = cbMedalMode
        self:showGameConfigSetLayer(configInfo.dwKindID,param)

end


--房间切换
function ClubRoomLayer:onSubGroupEnterSuccess()
    print("大联盟切换", ClubFrame:getInstance().m_nCurrentClub)
    --收到此消息说明进入大联盟成功了

    ClubFrame:getInstance():dismissPopWait()


    GlobalUserItem.dwCurRoomGroupID = ClubFrame:getInstance().m_nCurrentClub

    self.m_configList =  ClubFrame:getInstance().m_tabGroupConfig[ClubFrame:getInstance().m_nCurrentClub]


    
    if #self.m_configList==0 then
        self.m_emptyTip:setVisible(true)
    else
        self.m_emptyTip:setVisible(false)  
    end

     self:updateClubData()

     self:updateMsgRedPoint()


    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance().m_tabRoomInfoList = {}
    ClubFrame:getInstance():sendGetTableList(0, ClubFrame:getInstance().m_nCurrentClub)
end

function ClubRoomLayer:updateConfigList()
    self.m_configList =  ClubFrame:getInstance().m_tabGroupConfig[ClubFrame:getInstance().m_nCurrentClub]
    self.m_roomList =ClubFrame:getInstance().m_tabRoomInfoList
    self.m_sGameConfigList = {}
    self.m_sGameRoomList = {}
    self.m_oxeightConfigList = {}


    if self.m_curSelectGame ~= 0 then 
        for k,v in pairs(self.m_configList) do
            if v and v.dwKindID == self.m_tabGameList[self.m_curSelectGame] 
                and not self:isBattleGame(v.dwKindID) 
                and v.cbPlayMode == ClubFrame:getInstance().m_cbClubCurPlayMode then
                if v.dwKindID == 57  then

                    table.insert(self.m_oxeightConfigList,v)

                    if self.m_OxeightCurIndex == 0 then
                        table.insert(self.m_sGameConfigList,v)
                    elseif self.m_OxeightCurIndex == v.dwConfigID then
                        table.insert(self.m_sGameConfigList,v)
                    end
                    
                else
                    table.insert(self.m_sGameConfigList,v)
                end
            end
        end

        for k,v in pairs(self.m_roomList) do
            if v and v.dwKindID == self.m_tabGameList[self.m_curSelectGame] and v.cbPlayMode == ClubFrame:getInstance().m_cbClubCurPlayMode  then
                if v.dwKindID == 57  then
                    if self.m_OxeightCurIndex == 0 then
                        table.insert(self.m_sGameRoomList,v)
                    elseif self.m_OxeightCurIndex == v.dwConfigID then
                        table.insert(self.m_sGameRoomList,v)
                    end       
                else
                    table.insert(self.m_sGameRoomList,v)
                end
            end
        end

    else

        --百人游戏不显示玩法
        for k,v in pairs(self.m_configList) do
            if v and not self:isBattleGame(v.dwKindID) then
                table.insert(self.m_sGameConfigList,v)
            end
        end

        self.m_sGameRoomList = self.m_roomList
    end
    
    if #self.m_sGameConfigList==0 and #self.m_sGameRoomList == 0 then
        self.m_emptyTip:setVisible(true)
    else
        self.m_emptyTip:setVisible(false)  
    end

    if #self.m_sGameConfigList + #self.m_sGameRoomList > 6 then
        self.m_rightIcon:setVisible(true)
    else
        self.m_rightIcon:setVisible(false)
    end

    table.sort(self.m_sGameConfigList, function (a,b)
        return a.lTimeSecond < b.lTimeSecond
    end )


    if self._listRoomView and  tolua.cast(self._listRoomView,"cc.Node") then

        local curMove = self.m_curRoomMove
        self._listRoomView:reloadData()

        if #self.m_sGameConfigList  + #self.m_sGameRoomList > 0 then  --保持当前滑动距离不变
            local num = self:numberOfCellsInTableView(self._listRoomView)
            local cellWidth,cellHeight = self:cellSizeForTable(self._listRoomView, 0)
            local viewWidth = 1218
            local needMove = 0

            if num*cellWidth < viewWidth then
                needMove = 0
            else
                if curMove + viewWidth > num*cellWidth then
                    needMove = num*cellWidth - viewWidth
                else
                    needMove = curMove
                end
            end
           
            self.m_curRoomMove = needMove
            self._listRoomView:setContentOffset(cc.p(-needMove,0),false)
        end

    end

    local configTemp = clone(self.m_configList)
    for i=#configTemp,1,-1 do
        if configTemp[i] then
            if configTemp[i].cbPlayMode ~= ClubFrame:getInstance().m_cbClubCurPlayMode then 
                table.remove(configTemp,i)
            else

                for k,v in pairs(self.m_tabGameList) do
                    if v == configTemp[i].dwKindID  then
                        break
                    elseif k == #self.m_tabGameList then
                        table.remove(configTemp,i)
                    end
                end
                if #self.m_tabGameList == 0 then
                    configTemp = {}
                end
            end
        end
    end

    self.m_configList = configTemp

    if self._listRule and  tolua.cast(self._listRule,"cc.Node") then
        self._listRule:reloadData()

        if #self.m_configList==0 then
            self.m_emptyTip2:setVisible(true)
        else
            self.m_emptyTip2:setVisible(false)  
        end

    end

    if self.m_tabGameList[self.m_curSelectGame]  == 57 then
        self._listOxeightView:setVisible(true)
        self._listOxeightView:reloadData()

        if  self.m_OxeightCurIndex ~= 0  and #self.m_sGameConfigList == 0  then
            self.m_OxeightCurIndex = 0 
            self:updateConfigList()
        end

    else
        self._listOxeightView:setVisible(false)
    end



end


--刷新单个桌子
function ClubRoomLayer:updateRoomTable(roomID)

    local dwKindID = self.m_tabGameList[self.m_curSelectGame];

        local index = 0
        for k,v in pairs(self.m_sGameRoomList) do
            if v.dwPersonalRoomID == roomID then
                index = k
            end
        end
        
        index = index 
        if index ~= 0 then
            index = index + #self.m_sGameConfigList
            self._listRoomView:updateCellAtIndex((self:isBattleGame(dwKindID) and index or math.ceil(index/2)) -1)
        end
end


-- function ClubRoomLayer:updateCurClubImg()
--     local allCell = self._listClubView:
-- end

--刷新列表
function ClubRoomLayer:onSubGroupList()
    ClubFrame:getInstance():dismissPopWait()
    self:updateClubData()
end


function ClubRoomLayer:updateClubData()

    for i,v in ipairs(ClubFrame:getInstance().m_tabGroupItem) do
        if v.groupID == ClubFrame:getInstance().m_nCurrentClub then
            self.m_nCurrentClubInfo = v.groupItem
            GlobalUserItem.tabCurGroupInfo = self.m_nCurrentClubInfo
            break
        end
    end

     for k,v in pairs(ClubFrame:getInstance().m_tabGroupMember) do
            if v and v.groupID == ClubFrame:getInstance().m_nCurrentClub then
                    self.m_memberInfo = v.memberInfo
            end
     end


    for k,v in pairs(self.m_memberInfo) do
            if v and v.UserInfo.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
                    self.m_myClubInfo = v
                    GlobalUserItem.tabGroupUserInfo = self.m_myClubInfo
            end
     end

    
    if self.m_nCurrentClubInfo then
        self.m_damdString:setString(GlobalUserItem.tabAccountInfo.lDiamond.."")
        self.m_goldString:setString(self.m_myClubInfo.MemberInfo.lScore.."")
        
        self.m_groupId:setString("ID:"..self.m_nCurrentClubInfo.GroupProperty.dwGroupID)
        self.m_clubName:setString(self.m_nCurrentClubInfo.GroupProperty.szGroupName.."")

        if self.m_nCurrentClubInfo.GroupBattleRule.cbMedalMode ==0 then
            self.m_goldString:getParent():setVisible(false)

            --如果关闭勋章模式，按钮不可切换，显示积分界面
            ClubFrame:getInstance().m_cbClubCurPlayMode = 0
            self.btn_create:loadTextureNormal("club/scoreChange4.png")
            self.btn_create:loadTexturePressed("club/scoreChange4.png")
            self.btn_create:loadTextureDisabled("club/scoreChange4.png")
            cc.UserDefault:getInstance():setIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID..ClubFrame:getInstance().m_nCurrentClub.."lastMode", ClubFrame:getInstance().m_cbClubCurPlayMode)

            local lastGame = 0
            if self.m_curSelectGame ~= 0 and self.m_tabGameList[self.m_curSelectGame] then 
                lastGame = self.m_tabGameList[self.m_curSelectGame]
            end

            self.btn_create:setEnabled(false)
            self:updateCurPriModeGame()

            self._listGameView:reloadData()

            if lastGame ~= 0 then 
                for i =1,#self.m_tabGameList do
                    if lastGame == self.m_tabGameList[i] then 
                        self:onItemGameButtonClickedEvent(i)
                        break
                    elseif i ==#self.m_tabGameList then
                        self:onItemGameButtonClickedEvent(1)
                    end
                end
            end


            self:updateConfigList()

        else
            if ClubFrame:getInstance().m_cbClubCurPlayMode == 0 then
                self.btn_create:loadTextureNormal("club/scoreChange1.png")
                self.btn_create:loadTexturePressed("club/scoreChange1.png")
                self.btn_create:loadTextureDisabled("club/scoreChange1.png")
                
            else
                self.btn_create:loadTextureNormal("club/scoreChange2.png")
                self.btn_create:loadTexturePressed("club/scoreChange2.png")
                self.btn_create:loadTextureDisabled("club/scoreChange2.png")
            end
            self.btn_create:setEnabled(true)


            if (self.m_nCurrentClubInfo.GroupBattleRule.cbMedalShow == 0 and self.m_nCurrentClubInfo.GroupBattleRule.cbMedalMode ==1)  then
                if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID then
                    self.m_goldString:getParent():setVisible(false)
                else
                    self.m_goldString:getParent():setVisible(true)
                end
            else
                self.m_goldString:getParent():setVisible(true)
            end
            
        end 


        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID then
            self.m_damdString:getParent():setVisible(false)
            self.m_goldString:getParent():setPositionX(423)
        else
            self.m_damdString:getParent():setVisible(true)
        end

        --控制按钮位置
        local moveX = 0
        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
            ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT9) or 
            bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0 then
                self.m_btnCaptain:setVisible(true)
        else
                self.m_btnCaptain:setVisible(false)
                moveX = 160
        end


        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
            bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0 then
                self.m_btnMember:setVisible(true)
                self.m_btnMember:setPositionX(780+moveX)
        else
                self.m_btnMember:setVisible(false)
                moveX = moveX+160
        end 

        for i=1,2 do
                self.m_needMoveBtn[i]:setPositionX(460+moveX +160*(i-1))
        end  

        local btnTable = {}

        --控制勋章按钮显示
         if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
            ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT5) or 
            bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_CAPTAIN) ~= 0  then
                self.m_btnScore:setVisible(true)
                table.insert(btnTable,self.m_btnScore)
        else
                self.m_btnScore:setVisible(false)

        end


        --控制公告按钮显示
         --if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
            --ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT3)  then
                self.m_btnGongGao:setVisible(true)
                table.insert(btnTable,self.m_btnGongGao)
        --else
                --self.m_btnGongGao:setVisible(false)

        --end

        --控制广播按钮显示
         if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID or 
            ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT4) or 
            ClubFrame:getInstance():isCaptainAndRight(cmd_Club.CAPTAIN_RIGHT8)  then
                self.m_btnNotice:setVisible(true)
                table.insert(btnTable,self.m_btnNotice)
        else
                self.m_btnNotice:setVisible(false)

        end

        for i=1,#btnTable do
            btnTable[i]:setPositionX(1089 -89*(i-1))
        end

    end
end

function ClubRoomLayer:updateMsgRedPoint()
       self.m_tabMsg = {}
        for i,v in ipairs(ClubFrame:getInstance().m_tabApplyRespondMsg) do
            if v.dwGroupID == ClubFrame:getInstance().m_nCurrentClub  then
                    table.insert(self.m_tabMsg, v)
            end
        end

        self.m_cbMsgType  = 1
        local isShow = false
        for k,v in pairs(self.m_tabMsg) do
            if v and v.cbMessageStatus == cmd_Club.APPLY_STATUS_NONE then
                isShow = true
                if v.cbMessageType == 0 then 
                    self.m_cbMsgType = 2
                elseif v.cbMessageType == 1 and self.m_cbMsgType ~= 2 then
                    self.m_cbMsgType = 3
                end
            end
        end
        if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID ~= GlobalUserItem.tabAccountInfo.dwUserID and  
            bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) == 0 then
            isShow = false
        end

        self.m_spBg:getChildByName("red"):setVisible(isShow)

end


--获取玩家信息
function ClubRoomLayer:onSubGetTableUserList(tab)
    if self.isShowRule then
        return
    end

    --收到此消息说明获取玩家信息成功了
    self.m_nCurrentRoomInfo.userinfo = tab
    --显示玩家详情
    local pa = ClubTableInfoLayer:create(self._scene, self.m_nCurrentRoomInfo, ClubTableInfoLayer.SECOND_LEVEL)
    self._scene:addPopLayer(pa)
end

--删除大联盟
function ClubRoomLayer:onSubDeleteGroup()
    --self:scaleTransitionOut(self.m_spBg)
end

function ClubRoomLayer:onRefreshMember()
    print("ClubRoomLayer:onInsertMember")
    self.m_tabClubInfo = clone(ClubFrame:getInstance().m_tabGroupList)
    -- for i,v in ipairs(self.m_tabClubInfo) do
    --     if v.dwGroupID == ClubFrame:getInstance().m_nCurrentClub then
    --         --移动到最后
    --         local tagInfo = clone(v)
    --         table.remove(self.m_tabClubInfo, i)
    --         table.insert(self.m_tabClubInfo, tagInfo)
    --         break
    --     end
    -- end

    --self._listClubView:reloadData()
end

function ClubRoomLayer:getGameRule(pData, nGameKind)
    local gamelayer = self:getGameFile(nGameKind)
    if nil ~= gamelayer and nil ~= gamelayer.getGameRule then
        return gamelayer:getGameRule(pData)
    end
    return nil
end

-- 获取 GameVideoReplay
function ClubRoomLayer:getGameFile(nGameKind)
    local videogame = nil
    print("getGameFile", nGameKind)
    -- 寻找游戏
    if nil ~= self._scene then
        videogame = self._scene:getGameInfo(nGameKind)
    end
    if nil ~= videogame then
        local modulestr = string.gsub(videogame._KindName, "%.", "/")
        local gameFile = ""
        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            gameFile = "game/" .. modulestr .. "src/views/GameLayer.lua" .. yl.WIN_SRC_SUFFIX
        else
            gameFile = "game/" .. modulestr .. "src/views/GameLayer.lua"
        end
        print("getGameFile", gameFile)
        if cc.FileUtils:getInstance():isFileExist(gameFile) then
            return appdf.req(gameFile)
        end
    end
    return nil
end

function ClubRoomLayer:checkGameUpdate(nKindID)
    print("检测资源更新")
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    local scene = self._scene
    local app = scene:getApp()
    local game = scene:getGameInfo(nKindID)
    local version = tonumber(app:getVersionMgr():getResVersion(nKindID))
    if cc.PLATFORM_OS_WINDOWS == targetPlatform then
        return true
    end

    if not version or game._ServerResVersion > version then
        if false == scene:singleGameUpdate(nKindID, function()
               print("大联盟资源更新成功回调")
               if self.m_updateCallBack.enterType then 
                    if self.m_updateCallBack.enterType == 1 then
                        self:showGameConfigSetLayer(self.m_updateCallBack.gameKind,self.m_updateCallBack.param)
                    elseif self.m_updateCallBack.enterType == 2 then
                            self:showEnterLayer(self.m_updateCallBack.roominfo)
                    end
                    self.m_updateCallBack = {}
               end

            end) then
            if nil ~= game then
                local gamename = game._GameName or ""
                local msg = string.format("请更新 %s 游戏资源!", gamename)
                showToast(scene, msg, 2)
            else
                showToast(scene, "请更新游戏资源!", 2)
            end
            return false
        end
        return false
    end
    return true
end

return ClubRoomLayer