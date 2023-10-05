--
-- Author: zhong
-- Date: 2017-08-03 14:59:42
--
-- 包含(GameListLayer 游戏列表层, GameUpdater 单游戏更新层)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local UIPageView = appdf.req(appdf.EXTERNAL_SRC .. "UIPageView")
local MultiUpdater = appdf.req(appdf.EXTERNAL_SRC.."MultiUpdater")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local LogonFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LogonFrame")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local ClubRoomLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubRoomLayer")

local GameListLayer = class("GameListLayer", TransitionLayer)
local GameUpdater = class("GameUpdater", TransitionLayer)
GameListLayer.GameUpdater = GameUpdater

local ShopLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ShopLayer")                               -- 商城
local SpreadLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "SpreadLayer")                           -- 推广
local DailyBonusLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "DailyBonusLayer")                    -- 每日签到
local SpineObj = appdf.req("client.src.external.SpineObj")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")

-- 坐标位置
local tabPosition = 
{
    {{0.5, 0.5}},  -- 1
    {{0.35, 0.5}, {0.65, 0.5}},  -- 2
    {{0.2, 0.5}, {0.5, 0.5}, {0.8, 0.5}},  -- 3
    {{0.125, 0.5}, {0.375, 0.5}, {0.625, 0.5}, {0.875, 0.5}},  -- 4
    {{0.125, 0.775}, {0.375, 0.775}, {0.625, 0.775}, {0.875, 0.775}, {0.125, 0.325}},  -- 5
    {{0.125, 0.775}, {0.375, 0.775}, {0.625, 0.775}, {0.875, 0.775}, {0.125, 0.325}, {0.375, 0.325}},  -- 6
    {{0.125, 0.775}, {0.375, 0.775}, {0.625, 0.775}, {0.875, 0.775}, {0.125, 0.325}, {0.375, 0.325}, {0.625, 0.325}},  -- 7
    {{0.125, 0.775}, {0.375, 0.775}, {0.625, 0.775}, {0.875, 0.775}, {0.125, 0.325}, {0.375, 0.325}, {0.625, 0.325}, {0.875, 0.325}},  -- 8
}

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

local ACTION_START          = 300
local actionEnum = 
{
    "ACTION_DEFAULT",       -- 默认
    "ACTION_PRIVATE",       -- 约战房间
    "ACTION_SCORELIST",     -- 金币房间
}
local ACTION_ENUM = ExternalFun.declarEnumWithTable(ACTION_START, actionEnum)
GameListLayer.ACTION_ENUM = ACTION_ENUM

local this 

function GameListLayer:ctor( scene,param,level )
    GameListLayer.super.ctor(self, scene, param, level)
    self._param = self._param or {}
    this = self

    local logonCallBack = function (result,message)
        if this.onLogonCallBack then
            this:onLogonCallBack(result,message)
        end
    end
    self._logonFrame = LogonFrame:create(self,logonCallBack)


    -- 动作
    self.m_action = self._param.action or ACTION_ENUM.ACTION_DEFAULT

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/GameListLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )
    self.m_layMask = mask

    -- 关闭
    -- local btn = mask:getChildByName("btn_close")
    -- btn:setTag(TAG_ENUM.BTN_CLOSE)
    -- btn:addTouchEventListener( touchFunC )

    -- pageview
    -- local tmp = mask:getChildByName("content")
    -- self.m_PageView = UIPageView.new {
    --     viewRect = cc.rect(0, 0, tmp:getContentSize().width, tmp:getContentSize().height),
    --     padding = {left = 0, right = 0, top = 0, bottom = 0},
    --     bCirc = false}
    --     :onTouch(handler(self, self.onTouchPageListener))
    --     :setPosition(tmp:getPosition())
    --     :addTo(mask)
    -- tmp:removeFromParent()
    self.m_tabPageItem = {}

    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onClubInfoChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)

    -- 
    --title
    -- self.titlePri = mask:getChildByName("plaza_title_gamelist")
    -- self.titleGold = mask:getChildByName("plaza_title_gold")
    -- self.titlePri:setVisible(self.m_action == ACTION_ENUM.ACTION_PRIVATE)
    -- self.titleGold:setVisible(self.m_action == ACTION_ENUM.ACTION_SCORELIST)
    self._gameList = {}
    self.m_curClickButton = nil
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
                if GlobalUserItem.bIsNeedConnect then
                    GlobalUserItem.bIsNeedConnect = false
                    self.m_curClickButton = ref
                    self._logonFrame:onGetOffLineInfo()
                else
                    self:onGameIconClickEvent(ref:getTag(), ref)  
                end
                
            end  
        end
    end

    --筛选游戏
    local tmpConfig = self._param.list or {}
    local tabConfig = {}
    -- 处理游戏 --只有金币场

    for k,v in pairs(tmpConfig) do
        -- 未开启约战房间, 不加入游戏列表
        if PriRoom:getInstance():isCurrentGameOpenPri(tonumber(v._KindID)) or GlobalUserItem.GetGameRoomCount(tonumber(v._KindID)) > 0 
            or GlobalUserItem.GetGameRoomCount(tonumber(v._KindID)) > 0 then
            if tonumber(v._KindID) == 145 then
                if GlobalUserItem.GetGameRoomCount(tonumber(v._KindID)) > 0 then
                    table.insert(self._gameList, v)
                end
            else
                 table.insert(self._gameList, v)
            end
        end
    end
    dump(self._gameList, "游戏列表", 7)

    -- 弹窗
    self._query = nil

    --点击间隔标识
    self._clickDelay = -1

    self.activtList = {}





    -- 注册事件监听
    --self:registerEventListenr()
    -- 加载动画
    self:noActionTransitionIn(mask)
end


function GameListLayer:onLogonCallBack(result,message)

    if result == 10 then --成功
        if GlobalUserItem.tabAccountInfo.dwLockServerID ~= 0 then
            self._scene:logonRoomByLockServerID(GlobalUserItem.tabAccountInfo.dwLockServerID)
        else
            if self.m_curClickButton then
                self:onGameIconClickEvent(self.m_curClickButton:getTag(), self.m_curClickButton)  
            end
        end
    elseif result == -1 then --失败
        if type(message) == "string" and message ~= "" then
            showToast(self,message,2,cc.c4b(250,0,0,255));
        end
    end
end



function GameListLayer:onButtonClickedEvent(tag, ref)
    if tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_layMask)
    end
end

function GameListLayer:onGameIconClickEvent(tag, ref)
    local config = ref.config
    if type(config) ~= "table" then
        print("参数非法")
        return
    end
    --下载/更新资源 clientscene:getApp
    local app = self._scene:getApp()
    local nKindID = tonumber(config._KindID)
    local version = tonumber(app:getVersionMgr():getResVersion(nKindID))
    if not version or config._ServerResVersion > version then
        -- 判断是否更新
        if nil ~= GlobalUserItem.tabUpdater[nKindID] or self:isGameUnZip(nKindID) then
            print("GameListLayer:onGameIconClickEvent 正在更新 ", config._GameName)
            return
        end

        -- parent=pageitem
        self:addDownloadInfo(ref:getParent(), nKindID)

        self:updateGame(config)
    else
        --if self._clickDelay ~=  nKindID then
            --self._clickDelay = nKindID 
            self:onEnterGame(config)
            --ref:runAction(cc.Sequence:create(cc.DelayTime:create(0.4),cc.CallFunc:create(function ( ... )
             --   self._clickDelay = -1 
            --end)))
            
        --end
    end
end

-- 增加下载信息
function GameListLayer:addDownloadInfo( ref, nKindID )
    -- 校验
    if nil == ref or nil == nKindID then
        return
    end
    --先移除，再添加
    self:removeDownloadInfo(nKindID)

    ref:removeChildByName(nKindID .. "_sp_tipbg")

    --获取按钮背景
     local icon = ref:getChildByName(nKindID .. "_icon")
     local posx = icon:getPositionX()
     local posy = icon:getPositionY()


    -- 底图
    local tipbg = cc.Sprite:create("gameicon/gameicon_sp_downloadbg.png")
    tipbg:setName(nKindID .. "_sp_tipbg")
    tipbg:setPosition(posx, posy - 94)
    ref:addChild(tipbg)


    -- 进度
    --进度条
    local totalBar = ccui.LoadingBar:create()
    totalBar:loadTexture("gameicon/gameicon_sp_downloadbar.png") 
    ref:addChild(totalBar)
    totalBar:setName(nKindID .. "_sp_totalBar")
    totalBar:setPosition(posx, posy - 94)
    totalBar:setPercent(0)

    local bar_tips = cc.LabelAtlas:create("0:", "gameicon/gameicon_sp_downloadnum.png", 18, 18, string.byte("0"))
    bar_tips:setAnchorPoint(cc.p(0.5, 0.5))
    ref:addChild(bar_tips)
    bar_tips:setName(nKindID .. "_bar_tips")
    bar_tips:setPosition(posx, posy - 112)
end

function GameListLayer:isGameUnZip(nKindID)
    if nil == nKindID then
        return
    end
    -- 进度信息
    for k,v in pairs(self.m_tabPageItem) do
        -- icon
        local icon = v:getChildByName(nKindID .. "_icon")
        if icon then
            if  v:getChildByName(nKindID .. "_sp_totalBar") then
                return true
            end
        end
    end
    return false
end

-- 移除下载信息
function GameListLayer:removeDownloadInfo( nKindID )
    if nil == nKindID then
        return
    end
    -- 进度信息
    for k,v in pairs(self.m_tabPageItem) do
        -- icon
        local icon = v:getChildByName(nKindID .. "_icon")
        if nil ~= icon then
            -- 背景
            v:removeChildByName(nKindID .. "_sp_tipbg")

            v:removeChildByName(nKindID .. "_sp_totalBar")

            -- 提示
            v:removeChildByName(nKindID .. "_bar_tips")

            break
        end
    end
end

function GameListLayer:onTransitionInFinish()

    --游戏列表
    local tmp = self.m_layMask:getChildByName("content")
    self._listView = cc.TableView:create(tmp:getContentSize())
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)    
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.m_layMask)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()


    --获取活动信息
    self:refresh(GlobalUserItem.getAwardData())

    local tmpConfig = self._param.list or {}
    local tabConfig = {}

    for k,v in pairs(tmpConfig) do
        -- 未开启约战房间, 不加入游戏列表
        if PriRoom:getInstance():isCurrentGameOpenPri(tonumber(v._KindID)) or GlobalUserItem.GetGameRoomCount(tonumber(v._KindID)) > 0 then
            table.insert(tabConfig, v)
        end
    end


    -- 判断是否有下载
    for k,v in pairs(GlobalUserItem.tabUpdater) do
        for k1,v1 in pairs(self.m_tabPageItem) do
            local ref = v1:getChildByName(k .. "_icon")
            if nil ~= ref then
                self:addDownloadInfo(v1, k)
            end
        end
        v._listener = self
    end
end

function GameListLayer:animationRemove()
    self:scaleTransitionOut(self.m_layMask)
end

function GameListLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function GameListLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end

function GameListLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function GameListLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function GameListLayer:onTransitionOutFinish()
    -- 关闭通知
    for k,v in pairs(GlobalUserItem.tabUpdater) do
        v._listener = nil
    end
    self:removeFromParent()
end

function GameListLayer:onTouchPageListener( event )
    --dump(event, "onTouchPageListener - event:")
end

function GameListLayer:onEnterGame( gameinfo )
    -- if self.m_action == ACTION_ENUM.ACTION_PRIVATE then
    --     if nil ~= self._scene._scenePopLayer:getChildByName(PriRoom.NAME_CREATE_LAYER) then
    --         return
    --     end
    --     if nil == gameinfo then

    --     end
    --     local nKindID = tonumber(gameinfo._KindID)
    --     if not PriRoom:getInstance():isCurrentGameOpenPri(nKindID) then
    --         showToast(self, "约战房未开启!", 1)
    --         return
    --     end
    --     GlobalUserItem.nCurGameKind = nKindID
    --     self._scene:updateEnterGameInfo(gameinfo)
        
    --     local cl = PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_CREATEPRIROOME, self._scene, {})
    --     if nil ~= cl then
    --         self._scene:addPopLayer(cl)
    --         cl:setName(PriRoom.NAME_CREATE_LAYER)
    --     end
    --     PriRoom:getInstance().m_CurrentClubID = 0
    -- elseif self.m_action == ACTION_ENUM.ACTION_SCORELIST then
        
            if nil ~= self._scene._scenePopLayer:getChildByName(yl.PLAZA_ROOMLIST_LAYER) then
                return
            end
            local nKindID = tonumber(gameinfo._KindID)
            GlobalUserItem.nCurGameKind = nKindID
            self._scene:updateEnterGameInfo(gameinfo)
            self._scene:enterRoomList()
        
    --end
end

function GameListLayer:updateGame( gameinfo )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if cc.PLATFORM_OS_WINDOWS == targetPlatform and yl.DISABLE_WIN_UPDATE then
        print("GameListLayer win32 跳过更新")
        -- 移除信息
        self:removeDownloadInfo(gameinfo._KindID)

        local app = self._scene:getApp()
        --更新版本号
        app:getVersionMgr():setResVersion(gameinfo._ServerResVersion, gameinfo._KindID)
        self:onEnterGame(gameinfo)
    else
        local app = self._scene:getApp()
        local version = tonumber(app:getVersionMgr():getResVersion(gameinfo._KindID))
        if version == nil then
            print("GameListLayer 准备下载"..gameinfo._Module)
            self:onGameDownload(gameinfo)
        else
            self:onGameUpdate(gameinfo)
        end
    end
end

--更新游戏
function GameListLayer:onGameUpdate(gameinfo)
    if not gameinfo then
        showToast(self,"游戏版本信息错误！",1)
        return
    end
    local update = GlobalUserItem.tabUpdater[gameinfo._KindID]
    --失败重试
    if update ~= nil then
        update:UpdateFile()
        return 
    end

    --更新参数
    local newfileurl = self._scene:getApp()._updateUrl.."/game/" .. gameinfo._Module .. "res/filemd5List.json"
    local dst = device.writablePath .. "game/" .. gameinfo._Type .. "/"
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if cc.PLATFORM_OS_WINDOWS == targetPlatform then
        dst = device.writablePath .. "download/game/" .. gameinfo._Type .. "/"
    end
    
    local src = device.writablePath.."game/" .. gameinfo._Module .. "res/filemd5List.json"
    local downurl = self._scene:getApp()._updateUrl .. "/game/" .. gameinfo._Type .. "/"

    --创建更新
    update = MultiUpdater:create(newfileurl,dst,src,downurl)
    update:upDateClient(self._scene:getApp(), self, gameinfo)
    GlobalUserItem.tabUpdater[gameinfo._KindID] = update
end

--下载游戏
function GameListLayer:onGameDownload(gameinfo, index)

    if GlobalUserItem.tabUpdater[gameinfo._KindID] then
        showToast(nil, "正在下载 请稍后", 2)
        return
    end
    local app = self._scene:getApp()
    local updateUrl = self._scene:getApp()._updateUrl

    --下载地址
    local fileurl = updateUrl .. "/game/" .. string.sub(gameinfo._Module, 1, -2) .. ".zip"
    --local fileurl = updateUrl .. "/game/"

    --文件名
    local pos = string.find(gameinfo._Module, "/")
    local savename = string.sub(gameinfo._Module, pos + 1, -2) .. ".zip"

    --fileurl = fileurl .. savename
    print("onGameDownload------------>", fileurl)

    --保存路径
    local savepath = nil
    local unzippath = nil
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if cc.PLATFORM_OS_WINDOWS == targetPlatform then
        savepath = device.writablePath .. "download/game/" .. gameinfo._Type .. "/"
        unzippath = device.writablePath .. "download/"
        print(unzippath.."=================33")
    else
        savepath = device.writablePath .. "game/" .. gameinfo._Type .. "/"

        local x,y= string.find(gameinfo._Module,'[/]');
        local name = string.sub(gameinfo._Module,1,x-1)
        unzippath = device.writablePath .. "game/".. name

    end

     --创建更新
    update = MultiUpdater:create(unzippath,savepath,savename,fileurl)
    update:upDownClient(self._scene:getApp(), self, gameinfo)
    GlobalUserItem.tabUpdater[gameinfo._KindID] = update


--    GlobalUserItem.tabUpdater[gameinfo._KindID] = gameinfo

   --下载游戏压缩包
--    downFileAsync(fileurl, savename, savepath, function(main, sub)
--            --下载回调
--            if main == appdf.DOWN_PRO_INFO then --进度信息
--                print("更新进度 "..gameinfo._Module)
--                self:updateProgress(gameinfo, "","", sub)
--            elseif main == appdf.DOWN_COMPELETED then --下载完毕

--                local zipfile = savepath .. savename

--                --解压
--                unZipAsync(zipfile, unzippath, function(result)
--                        --删除压缩文件
-- --                        os.remove(zipfile)
--                        print("解压完毕 "..zipfile)
--                        self:updateResult(gameinfo, true, "")
--                end)
--            else
--                self:updateResult(gameinfo, false, "")
--                showToast(nil, "下载失败，错误码：" .. main .. ", " .. sub, 2)
--            end
--    end)
end


function GameListLayer:onUpdataNotify()
    showToast(self,"游戏版本信息错误！",1)
end

--更新进度
function GameListLayer:updateProgress(updateinfo, sub, msg, mainpersent)
    if type(updateinfo) ~= "table" or nil == updateinfo._KindID then
        return
    end
    -- 进度信息
    for k,v in pairs(self.m_tabPageItem) do
        local icon = v:getChildByName(updateinfo._KindID .. "_icon")
        if nil ~= icon then
            -- 进度文字
            local tips = v:getChildByName(updateinfo._KindID .. "_bar_tips")
            if nil ~= tips then
                tips:setString(string.format("%d:", mainpersent))

                local totalBar = v:getChildByName(updateinfo._KindID .. "_sp_totalBar")
                if nil ~= totalBar then
                    totalBar:setPercent(mainpersent)
                end
                break
            end
        end
    end
end

--更新结果
function GameListLayer:updateResult(updateinfo, result, msg)
    if type(updateinfo) ~= "table" or nil == updateinfo._KindID then
        return
    end
    if nil ~= self._query then
        self._query:removeFromParent() 
    end
    -- 移除信息
    self:removeDownloadInfo(updateinfo._KindID)
    -- 清理下载管理
    GlobalUserItem.tabUpdater[updateinfo._KindID] = nil

    if result == true then
        local app = self._scene:getApp()
        -- 更新版本号
        for k,v in pairs(app._gameList) do
            if v._KindID == updateinfo._KindID then
                app:getVersionMgr():setResVersion(v._ServerResVersion, v._KindID)
                v._Active = true
                break
            end
        end
        self:onEnterGame(updateinfo)
        
    else
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            self._query = QueryDialog:create(msg.."\n是否重试？",function(bReTry)
                if bReTry == true then
                    for k1,v1 in pairs(self.m_tabPageItem) do
                        local ref = v1:getChildByName(updateinfo._KindID .. "_icon")
                        if nil ~= ref then
                            self:addDownloadInfo(v1, updateinfo._KindID)
                        end
                    end
                    self:onGameUpdate(updateinfo)
                end
                self._query = nil
            end)
            :addTo(runScene)
        end     
    end
end


--子视图大小
function GameListLayer:cellSizeForTable(view, idx)
    if 0 == idx then
        return 430 , 500
    else
        return 280 , 480
    end
end

--子视图数目
function GameListLayer:numberOfCellsInTableView(view)
    return math.ceil(#self._gameList/2) +1   --加1为活动
end

    
--获取子视图
function GameListLayer:tableCellAtIndex(view, idx)  
    local cell = view:dequeueCell()
    if not cell then
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local app = self._scene:getApp()
    if 0 == idx then
        if GlobalUserItem.isOpenGroupView  == 1 then 
            local btn = ccui.Button:create("plaza/plaza_sp_adlist_est.png", "plaza/plaza_sp_adlist_est.png", "plaza/plaza_sp_adlist_est.png")
            btn:setPressedActionEnabled(false)
            btn:setSwallowTouches(true)
            cell:addChild(btn)
            btn:setTouchEnabled(true)
            btn:setPosition(cc.p(215, 250))
            btn:setContentSize(430, 500)
            btn:setOpacity(0)

        
            self.m_PageView = UIPageView.new {
                viewRect = cc.rect(0, 0, 386, 491),
                padding = {left = 0, right = 0, top = 0, bottom = 0},
                bCirc = true,
                bAutoScroll = true}
                :onTouch(handler(self, self.onTouchPageListener))
                :setPosition(22, 0)
                :addTo(cell)
            self.m_PageView:setIndicatorEnabled(true, "plaza/plaza_sp_indicator_0.png", "plaza/plaza_sp_indicator_1.png")
            self.m_PageView:setIndicatorPosition(193, 20)
            self.m_PageView:setTouchEnabled(true)

            local count = #self.activtList
            local list = self.activtList
            local showcount = 0
            for i = 1, count do
                local item = self.m_PageView:newItem()
                local adimage = list[i].imageName
                item.info = list[i]
                --adimage = cc.FileUtils:getInstance():fullPathForFilename("activity/" .. adimage)
                local path = string.format("%sdownload/activity/%s", device.writablePath, adimage)
                if cc.FileUtils:getInstance():isFileExist(path) then
                    local imageView = ccui.ImageView:create(path)
                    imageView:setPosition(cc.p(item:getContentSize().width * 0.5, item:getContentSize().height * 0.5 +10))
                    item:addChild(imageView, 10, 10)
                    showcount = showcount + 1

                    --背景框
                    local sp = cc.Sprite:create("plaza/plaza_sp_adlist_est.png")
                    item:addChild(sp)
                    sp:setPosition(cc.p(item:getContentSize().width * 0.5 +1, 223))
                end
                self.m_PageView:addItem(item)
            end
            --self.m_PageView:setTouchEnabled(showcount ~= 0)
            self.m_PageView:reload()

        else 
                local csb = ExternalFun.loadCSB("plaza/node_clubEnter.csb", cell)
                csb:setPosition(215, 250)


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

                self.m_tabClubInfo = {}
                local allGroup = clone(ClubFrame:getInstance().m_tabGroupList)

                if self.m_lastClub ~= 0 then

                            for k,v in pairs(allGroup) do
                                if v and v.dwGroupID == self.m_lastClub then
                                    table.insert(self.m_tabClubInfo,v)
                                    break
                                end
                            end

                end

                if self.m_lastClub == 0 or #self.m_tabClubInfo == 0 then

                    for i=1,#self.m_clubSort do
                        for k,v in pairs(allGroup) do
                            if v and self.m_clubSort[i].group == v.dwGroupID then
                                table.insert(self.m_tabClubInfo,clone(v))
                                table.remove(allGroup,k)
                                break
                            end
                        end
                    end

                    for k,v in pairs(allGroup) do
                        if v then
                            table.insert(self.m_tabClubInfo,clone(v))
                        end
                    end
                end


                local curClub = {}
                if #self.m_tabClubInfo <=0 then
                    csb:getChildByName("Image_3"):setVisible(true)
                    csb:getChildByName("Image_1"):setVisible(false)
                    --
                    local obj = SpineObj:create("plaza/animate/club/clubEnter.json", "plaza/animate/club/clubEnter.atlas", true)
        --            if self.isFirstEnter then
                        self.isFirstEnter = false
                        obj:setAnimation(0, "animation2", false)
                        obj:registerSpineEventHandler(function (event)
                                    obj:setAnimation(0, "animation3", false)
                        end, 2)
                    -- else
                    --     obj:setAnimation(0, "animation3", false)
                    -- end
                    obj:setPosition(0,0)
                    csb:addChild(obj)

                else
                    csb:getChildByName("Image_3"):setVisible(false)
                    local bg = csb:getChildByName("Image_1")
                    bg:setVisible(true)
                    curClub = self.m_tabClubInfo[1]

                    csb:getChildByName("name"):setString(curClub.szGroupName.."")


                    local iconImg = csb:getChildByName("Sprite_5")
                    local txtTip = csb:getChildByName("Text_2")
                    local txtID = csb:getChildByName("code")

                    if curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_OWNER or 
                       curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_MANAGER then

                       iconImg:setTexture("club/clubRight2.png")
                       txtTip:setString("我的邀请码:")
                       txtID:setString(curClub.cmd_extra.lInvitationCode.."")

                    elseif curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_CAPTAIN then
                        iconImg:setTexture("club/clubRight1.png")
                        txtTip:setString("我的邀请码:")
                        txtID:setString(curClub.cmd_extra.lInvitationCode.."")
                    elseif curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_TEAMMEMBER or curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_MEMBER then
                        iconImg:setVisible(false)
                        txtTip:setString("上级邀请码:")
                        txtID:setString(curClub.cmd_extra.lInvitationCode.."")
                    end

                    if curClub.cmd_extra.cbGroupShow == 1  then
                        txtTip:setString("大联盟ID:")
                        txtID:setString(curClub.dwGroupID.."")


                    end

                    if curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_OWNER or 
                           curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_MANAGER then
                           bg:loadTexture("club/clubBtn3.png")  
                    elseif curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_CAPTAIN then
                            bg:loadTexture("club/clubBtn2.png")  
                    elseif curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_TEAMMEMBER or 
                        curClub.cmd_extra.nMemberRight ==  cmd_Club.AUTHORITY_MEMBER then
                            bg:loadTexture("club/clubBtn1.png")  
                    end


                    csb:getChildByName("allNum"):setString(curClub.wMemberCount.."/"..curClub.wMaxMemberCount)

                    local clubList = csb:getChildByName("Image_1"):getChildByName("Button_1")
                    --clubList:setSwallowTouches(false)
                    clubList:addTouchEventListener(function(ref, tType)
                        if tType == ccui.TouchEventType.ended then
                                self._scene:onChangeShowMode(yl.SCENE_CLUB)
                        end
                    end)

                end

                local enterClub = csb:getChildByName("Image_1")
                enterClub:addTouchEventListener(function(ref, tType)
                    if tType == ccui.TouchEventType.ended then

                        self._scene:enterClub(curClub.dwGroupID)

                    end
                end)

                    local clubList = csb:getChildByName("Image_3")
                    clubList:addTouchEventListener(function(ref, tType)
                        if tType == ccui.TouchEventType.ended then
                                self._scene:onChangeShowMode(yl.SCENE_CLUB)
                        end
                    end)
            end





    else
        local posY = {366, 120}
        for i=1, 2 do
            local config = self._gameList[(idx - 1) * 2 + i]
            if nil == config then
                break
            end
            local file = string.format("gameicon/gameicon_%d.png", config._KindID)
            local notexit = false
            if not cc.FileUtils:getInstance():isFileExist(file) then
                file = "gameicon/gameicon_default.png"
                notexit = true
            end
            local btn = ccui.Button:create(file, file, file)
            btn.config = config
            btn:addTouchEventListener( self.m_itemFunC )
            btn:setPressedActionEnabled(false)
            btn:setSwallowTouches(false)
            btn:setName(config._KindID .. "_icon")
            cell:addChild(btn)
            btn:setPosition(cc.p(120, posY[i]))


            --动画
            --判断资源是否存在
            local resPath = "gameicon/gameicon_action/" .. "action_" .. config._KindID .. ".atlas"
            local resConfig = "gameicon/gameicon_action/" .. "action_" .. config._KindID .. ".json"
            --print("获取子图标动画配置--------------》", resConfig, resPath)
            if cc.FileUtils:getInstance():isFileExist(resPath) and cc.FileUtils:getInstance():isFileExist(resConfig) then 
                local obj = SpineObj:create(resConfig, resPath, true)
                obj:setPosition(btn:getPositionX(),btn:getPositionY() -15)
                cell:addChild(obj)
                btn:setOpacity(0)
            elseif notexit then
                local kindname = cc.Label:createWithTTF(config._GameName, "fonts/round_body.ttf", 30)
                btn:addChild(kindname)
                kindname:setPosition(140, 60)
            
            end


            -- 检查下载/更新
            local nKindID = tonumber(config._KindID)
            local version = tonumber(app:getVersionMgr():getResVersion(nKindID))
            if not version then
                -- 下载
                local tipbg = cc.Sprite:create("gameicon/gameicon_sp_tipbg.png")
                tipbg:setPosition(50, posY[i] + 70)
                tipbg:setName(config._KindID .. "_sp_tipbg")
                cell:addChild(tipbg)
                tipbg:setLocalZOrder(10)
                -- 下载标识
                -- local downloadtip = cc.Sprite:create("gameicon/gameicon_sp_downloadtip.png")
                -- downloadtip:setPosition(tipbg:getContentSize().width * 0.5, tipbg:getContentSize().height * 0.5)
                -- tipbg:addChild(downloadtip)
            elseif config._ServerResVersion > version then
                -- 更新
                local tipbg = cc.Sprite:create("gameicon/gameicon_sp_updatetip.png")
                tipbg:setPosition(55, posY[i] + 70)
                tipbg:setName(config._KindID .. "_sp_tipbg")
                cell:addChild(tipbg)
                tipbg:setLocalZOrder(10)
            end

            table.insert(self.m_tabPageItem, cell)

            -- -- 判断是否有下载
            for k,v in pairs(GlobalUserItem.tabUpdater) do
                if k == config._KindID then
                    self:addDownloadInfo(cell, k)
                    v._listener = self
                    break
                end
            end
        end
    end 
    cell:setVisible(true)
    return cell
end





function GameListLayer:refresh(activityList)
    self.downLoadCount = 0
    local needDownList = {}
    local fileUtil = cc.FileUtils:getInstance()
    self.activtList = {}

    for idx,data in pairs(activityList) do
        if data.ResourceURL and data.ResourceURL ~= "" and tonumber(data.PlatformType) == 2 then
            local imageName = ExternalFun.getFileName(data.ResourceURL)
            local path = string.format("%sdownload/activity/%s", device.writablePath, imageName)
            if not fileUtil:isFileExist(path) then
                needDownList[imageName] = data.ResourceURL
                self.downLoadCount = self.downLoadCount + 1
            end

            local act = {}
            act.imageName = imageName
            act.LinkURL = data.LinkURL --数字代表跳转界面,链接打开网页
            act.SortID = data.SortID 
            act.NoticeTitle = data.Title
            table.insert(self.activtList, act)
        end
    end

    if next(needDownList) then
        local downFileAsync = downFileAsync
        for filename,url in pairs(needDownList) do
            downFileAsync(url, filename, string.format("%sdownload/activity", device.writablePath), function(...) 
                    if self and self.downLoadCallback then 
                        self:downLoadCallback(...) 
                    end
                end)
        end
    else
        self._listView:reloadData()
    end
end

function GameListLayer:downLoadCallback(main, sub)
    print("下载--------------->",main, sub, self.downLoadCount)
    --self._listView:reloadData()
    if main == appdf.DOWN_PRO_INFO then
    elseif main == appdf.DOWN_COMPELETED then
        self.downLoadCount = self.downLoadCount - 1
        print("下载完成--------------->",main, sub, self.downLoadCount)
        --下载完毕,刷新
        if self.downLoadCount <= 0 and self._listView  then
            self._listView:reloadData()
        end
    else
        --超时
        if sub == 28 then
            print("Time out!!!")
        end
    end
end

function GameListLayer:onClubInfoChange( event  )
    print("----------ClubMain userinfo change notify------------")
    local msgWhat = event.obj
    
    print("----------userinfo change notify------------", msgWhat)

    if nil ~= msgWhat  then
        if msgWhat == yl.RY_CULB_ENTER_NOTIFY then
            local dwGroupID = event.dwGroupID
            if dwGroupID then
                self._scene:enterClub(dwGroupID)
            end
        end

       if msgWhat == yl.RY_CULB_DELETE_NOTIFY or msgWhat == yl.RY_CULB_MEMBER_NOTIFY  then
            if self._listView then 
                self._listView:updateCellAtIndex(0)
            end
        end


    end

end

-- 翻页监听
function GameListLayer:onTouchPageListener( event )
    --dump(event, "onTouchPageListener - event:")

    if event.name == "pageChange" then
   
    elseif event.name == "clicked" then
        if nil ~= event.item and type(event.item.info) == "table" then
            local info = event.item.info
            local idx = tonumber(info.LinkURL)
            print("onTouchPageListener---------------->", idx)
            if idx then
                if idx == 1 then --签到
                    local pa = DailyBonusLayer:create(self._scene, {}, DailyBonusLayer.SECOND_LEVEL)
                    self._scene:addPopLayer(pa)
                elseif idx == 2 then --分享
                    local sl = SpreadLayer.SpreadSimpleLayer:create(self._scene, {}, SpreadLayer.SECOND_LEVEL)
                    self._scene:addPopLayer(sl)
                    
                elseif idx == 3 then --比赛
                    if MatchRoom then
                        self._scene:onChangeShowMode(yl.SCENE_MATCH)
                    end
                elseif idx == 4 then --商城
                    local sl = ShopLayer:create(self._scene, ShopLayer.DIAMOND_CHARGE, ShopLayer.SECOND_LEVEL)
                    self._scene:addPopLayer(sl)
                end
            else--web页
                if (device.platform == "android" or device.platform == "ios") and info.LinkURL ~= "" then
                    -- local width = display.width*0.7
                    -- local height = display.height*0.7

                    -- self.m_webView = ccexp.WebView:create()
                    -- self.m_webView:setAnchorPoint(cc.p(0.5,0.5))
                    -- self.m_webView:setPosition(display.width*0.5, display.width*0.5-height*0.5)
                    -- self.m_webView:setContentSize(width,height)
                    -- self.m_webView:setScalesPageToFit(true)
                    -- display.getRunningScene():addChild(self.m_webView)
                    MultiPlatform:getInstance():openBrowser( info.LinkURL )
                end
            end
        end
    end
 
end


-- 游戏更新层
function GameUpdater:ctor( scene, param, level )
    GameUpdater.super.ctor( self, scene, param, level )
    self._param = param or {}
    self._downloadCallBack = self._param._callback or nil
    self._updategame = self._param._updategame or nil

    self._query = nil

    -- 屏蔽层
    local mask = ccui.Layout:create()
    mask:setContentSize(yl.WIDTH, yl.HEIGHT)
    mask:setBackGroundColorType(LAYOUT_COLOR_SOLID)
    mask:setBackGroundColor(cc.c3b(0, 0, 0))
    mask:setBackGroundColorOpacity(200)
    mask:setTouchEnabled(true)
    self:addChild(mask)

    local updatekind = self._param._nKindID
    self.m_spKindIcon = nil
    -- 游戏图标
    local file = string.format("gameicon/gameicon_%d.png", updatekind)
    if cc.FileUtils:getInstance():isFileExist(file) then
        local kindicon = cc.Sprite:create(file)
        kindicon:setPosition(yl.WIDTH * 0.5, yl.HEIGHT * 0.5)
        self.m_spKindIcon = kindicon
        self:addChild(kindicon)
    else
        local kindicon = cc.Sprite:create("gameicon/gameicon_default.png")
        kindicon:setPosition(yl.WIDTH * 0.5, yl.HEIGHT * 0.5)
        self.m_spKindIcon = kindicon
        self:addChild(kindicon)

        local szname = self._updategame._GameName or ""
        local labttf = cc.Label:createWithTTF(szname, "fonts/round_body.ttf", 24)
        labttf:setPosition(140, 60)
        self.m_spKindIcon:addChild(labttf)
    end

    self.m_updater = nil
    --self:onGameUpdate()

    local app = self._scene:getApp()
    local version = tonumber(app:getVersionMgr():getResVersion(updatekind))
    if version == nil then
        self:onGameDownload()
    else
        self:onGameUpdate()
    end

end

-- 屏蔽返回
function GameUpdater:onKeyBack()
    return true
end

function GameUpdater:onGameUpdate()
    if type(self._updategame) ~= "table" then
        showToast(self._scene, "游戏参数有误！", 2)
        self:removeFromParent()
        return
    end
    if nil ~= self.m_updater then
        self.m_updater:UpdateFile()
        return
    end
    --更新参数
    local newfileurl = self._scene:getApp()._updateUrl.."/game/" .. self._updategame._Module .. "res/filemd5List.json"
    local dst = device.writablePath .. "game/" .. self._updategame._Type .. "/"
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if cc.PLATFORM_OS_WINDOWS == targetPlatform then
        dst = device.writablePath .. "download/game/" .. self._updategame._Type .. "/"
    end
    
    local src = device.writablePath.."game/" .. self._updategame._Module .. "res/filemd5List.json"
    local downurl = self._scene:getApp()._updateUrl .. "/game/" .. self._updategame._Type .. "/"

    --创建更新
    self.m_updater = MultiUpdater:create(newfileurl,dst,src,downurl)

    self:addDownloadInfo()
    self.m_updater:upDateClient(self._scene:getApp(), self, self._updategame)
end



--下载游戏
function GameUpdater:onGameDownload()


    local app = self._scene:getApp()
    local updateUrl = self._scene:getApp()._updateUrl

    --下载地址
    local fileurl = updateUrl .. "/game/" .. string.sub(self._updategame._Module, 1, -2) .. ".zip"
    --local fileurl = updateUrl .. "/game/"

    --文件名
    local pos = string.find(self._updategame._Module, "/")
    local savename = string.sub(self._updategame._Module, pos + 1, -2) .. ".zip"

    --保存路径
    local savepath = nil
    local unzippath = nil
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if cc.PLATFORM_OS_WINDOWS == targetPlatform then
        savepath = device.writablePath .. "download/game/" .. self._updategame._Type .. "/"
        unzippath = device.writablePath .. "download/"
        print(unzippath.."=================33")
    else
        savepath = device.writablePath .. "game/" .. self._updategame._Type .. "/"

        local x,y= string.find(self._updategame._Module,'[/]');
        local name = string.sub(self._updategame._Module,1,x-1)
        unzippath = device.writablePath .. "game/".. name

    end

     --创建更新
    update = MultiUpdater:create(unzippath,savepath,savename,fileurl)
    self:addDownloadInfo()
    update:upDownClient(self._scene:getApp(), self, self._updategame)
    GlobalUserItem.tabUpdater[self._updategame._KindID] = update

end



function GameUpdater:addDownloadInfo()
    if nil == self.m_spKindIcon then
        return
    end
    -- 底图
    local tipbg = cc.Sprite:create("gameicon/gameicon_sp_downloadbg.png")
    tipbg:setName("update_sp_tipbg")
    tipbg:setPosition(125, 28)
    self.m_spKindIcon:addChild(tipbg)

    -- 进度
    --进度条
    self.m_totalBar = ccui.LoadingBar:create()
    self.m_totalBar:loadTexture("gameicon/gameicon_sp_downloadbar.png") 
    self.m_spKindIcon:addChild(self.m_totalBar)
    self.m_totalBar:setName("update_sp_tipbar")
    self.m_totalBar:setPercent(0)
    self.m_totalBar:setPosition(125, 28)

    local bar_tips = cc.LabelAtlas:create("0:", "gameicon/gameicon_sp_downloadnum.png", 18, 18, string.byte("0"))
    bar_tips:setAnchorPoint(cc.p(0.5, 0.5))
    self.m_spKindIcon:addChild(bar_tips)
    bar_tips:setName("update_bar_tips")
    bar_tips:setPosition(125, 10)
end

function GameUpdater:removeDownloadInfo()
    if nil == self.m_spKindIcon then
        return
    end
    self.m_spKindIcon:removeChildByName("update_sp_tipbar")
    self.m_spKindIcon:removeChildByName("update_sp_tipbg")
    self.m_spKindIcon:removeChildByName("update_bar_tips")
end

-- 更新进度
function GameUpdater:updateProgress(updateinfo, sub, msg, mainpersent)
    if type(updateinfo) ~= "table" or nil == updateinfo._KindID or nil == self.m_spKindIcon then
        return
    end

    -- 进度文字
    local tips = self.m_spKindIcon:getChildByName("update_bar_tips")
    if nil ~= tips then
        tips:setString(string.format("%d:", mainpersent))
    end
    self.m_totalBar:setPercent(mainpersent)
end

-- 更新结果
function GameUpdater:updateResult(updateinfo, result, msg)
    if type(updateinfo) ~= "table" or nil == updateinfo._KindID then
        self:removeFromParent()
        return
    end
    if nil ~= self._query then
        self._query:removeFromParent() 
    end
    -- 移除信息
    self:removeDownloadInfo()

    if result == true then
        local app = self._scene:getApp()
        -- 更新版本号
        for k,v in pairs(app._gameList) do
            if v._KindID == updateinfo._KindID then
                app:getVersionMgr():setResVersion(v._ServerResVersion, v._KindID)
                v._Active = true
                break
            end
        end
        if type(self._downloadCallBack) == "function" then
            self._downloadCallBack()
        end
        if not tolua.isnull(self) then self:removeFromParent() end 
    else
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            self._query = QueryDialog:create(msg.."\n是否重试？",function(bReTry)
                self._query = nil
                if bReTry == true then
                    self:addDownloadInfo()
                    self:onGameUpdate()
                else
                    self:removeFromParent()
                end
            end)
            :addTo(runScene)
        end     
    end
end



return GameListLayer