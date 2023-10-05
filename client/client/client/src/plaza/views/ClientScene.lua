local ClientScene = class("ClientScene", cc.load("mvc").ViewBase)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClientPopWait = appdf.req(appdf.EXTERNAL_SRC .. "ClientPopWait")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local LogonFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LogonFrame")

-- ui
local ClientSceneLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ClientSceneLayer")                  -- 大厅层管理
local PlazaAdLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "PlazaAdLayer")                          -- 大厅广告
local TargetShareLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "TargetShareLayer")                  -- 分享
local RewardLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "RewardLayer")                            -- 奖励
local UserCenterLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ucenter.UserCenterLayer")            -- 用户中心
local GameListLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "GameListLayer")                        -- 游戏列表
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local ShopLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ShopLayer")                               -- 商城
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")

local uiActivity = appdf.req(appdf.PLAZA_VIEW_SRC .. "uiActivity")
local DailyBonusLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "DailyBonusLayer")                    -- 每日签到
-- ui

-- net
local GameFrameEngine = appdf.req(appdf.CLIENT_SRC.."plaza.models.GameFrameEngine")             -- 游戏服务
local QueryQueueFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.QueryQueueFrame")             -- 查询服务
local chat_cmd = appdf.req(appdf.HEADER_SRC.."CMD_ChatServer")                                  -- 聊天服务器
local DailyBounsFrame = appdf.req(appdf.CLIENT_SRC .. "plaza.models.DailyBounsFrame")                           -- 签到

local ModifyFrame = appdf.req(appdf.CLIENT_SRC .. "plaza.models.ModifyFrame")                           -- 信息修改
-- net

-- 初始化
function ClientScene:onCreate()
    -- 是否弹出认证提示
    self.bPopIdentify = false

    -- 触摸屏蔽
    ExternalFun.popupTouchFilter(30, true)
	ExternalFun.registerNodeEvent(self)
	-- 背景
    self._bg = ccui.ImageView:create("public/bg1.png")
        :move(display.center)
        :addTo(self)

	-- 大厅活动层
	self._sceneLayer = ClientSceneLayer:create(self)
		:setContentSize(yl.WIDTH,yl.HEIGHT)
		:addTo(self)
	self._sceneLayer:setKeyboardEnabled(true)
    -- 大厅弹窗层
    self._scenePopLayer = display.newLayer()
        :setContentSize(yl.WIDTH,yl.HEIGHT)
        :addTo(self)
    self.m_bEnableKeyBack = true

    -- 记录游戏
    self:updateCurrentGame()

    -- 网络
    self._gameFrame = GameFrameEngine:create(self,function (code,result)
        self:onFrameEngineCallBack(code,result)
    end)

    GlobalUserItem.m_tabGameFrame = self._gameFrame
    
    PriRoom:getInstance():onEnterPlaza(self, self._gameFrame)

    GameVideo:getInstance():onEnterPlaza(self, self._gameFrame)

    FriendMgr:getInstance():setClientScene(self)

    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance():setClientScene(self)
    ClubFrame:getInstance():reSetAndLogin()

    --签到信息
    self._dailyFrame = DailyBounsFrame:create(self,function (code,result)
        --self:onFrameDailyCallBack(code,result)
        if code == 1 then
            self:dismissPopWait()
            --获取信息成功，判断是否已经签到
        end
    end)
    --请求签到配置信息
    self._dailyFrame:onGetDailyInfo()

    --比赛
    if MatchRoom then
        MatchRoom:getInstance():onEnterPlaza(self, self._gameFrame)
        self._gameFrame:setScene(self)
    end

    local logonCallBack = function (result,message)
        if self.onLogonCallBack then
            self:onLogonCallBack(result,message)
        end
    end
    self._logonFrame = LogonFrame:create(self,logonCallBack)

    -- 后台监听
    setbackgroundcallback(function (bEnter)
        if type(self.onBackgroundCallBack) == "function" then
            self:onBackgroundCallBack(bEnter)
        end
    end)
    -- 游戏喇叭背景
    self.m_spGameTrumpetBg = nil
    self.m_vecNotice = {}
    self.m_nNotifyId = 1

    --正在重连标识
    self.m_bIsReConnet = false


    -- 大厅界面
    self:onChangeShowMode(yl.SCENE_PLAZA)
end

function ClientScene:addPopLayer( lay )
    if nil == lay then
        print("ClientScene:addPopLayer param lay is nil")
        return
    end
    self._scenePopLayer:addChild(lay)
end

-- 配置查询
function ClientScene:startInitQuery()
    if nil ~= self._queryFrame then
        self._queryFrame:onCloseSocket()
        self._queryFrame = nil
    end
    self:showPopWait()
    local this = self
    self._queryFrame = QueryQueueFrame:create(self, function(code, msg)
        if code == QueryQueueFrame.OP_ENUM.OP_END or code == QueryQueueFrame.OP_ENUM.OP_ERROR then
            print("查询结束, msg ==> ", msg)
            this:dismissPopWait()
            this._queryFrame:onCloseSocket()
            this._queryFrame = nil


            
            -- 处理锁表
            local bHandleLockGame = true
            -- 是否锁表、是否更新游戏、是否锁私人房、比赛房
            local lockGame,updateGame,lockPriGame,lockMatchGame
            if PriRoom then
                lockGame, updateGame, lockPriGame = PriRoom:getInstance():onEnterPlazaFinish()
            end

            if MatchRoom then
                lockGame, updateGame, lockMatchGame = MatchRoom:getInstance():onEnterPlazaFinish()
            end

            --金币房
            if not lockPriGame and not lockMatchGame and GlobalUserItem.tabAccountInfo.dwLockKindID ~= 0 then
                self:logonRoomByLockServerID(GlobalUserItem.tabAccountInfo.dwLockServerID)
  
            end


            if not lockGame or not lockPriGame or not lockMatchGame then
                this:handleDeviceCopyRoomID()
                local bHandle = this:handleLaunchData()
                if not bHandle then
                    this:handleDeviceCopyData(function()
                        local nadcount = 0
                        if type(GlobalUserItem.tabAdConfig["adsAlertList"]) == "table" then
                            nadcount = tonumber(GlobalUserItem.tabAdConfig["adsAlertList"]["count"])
                        end
                        self._modifyFrame = ModifyFrame:create(self,function (result,message)
                                            if type(nadcount) == "number" and nadcount > 0 and GlobalUserItem.isShowAdNotice() then

                                                --兼容plazalayer弹出的活动,都加在同一父节点,传兼容plazalayer控制跳转
                                                local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
                                                local obj = uiActivity:ctor(self, plazalayer)
                                                obj:setdelayTime(2.5)
                                                obj:refresh(GlobalUserItem.getAwardData())
                                                obj:setRemoveListener(function()
                                                    print("uiActivity closed callback called==============================")
                                                    --self:popReward()
                                                    --判断
                                                    if not GlobalUserItem.tabDailyInfo.bTodayChecked and GlobalUserItem.tabSystemConfig.IsOpenGameSignIn ==0 then 
                                                        this:popDailyBonus()
                                                    else
                                                        this:popReward()    
                                                    end
                                                end)

                                                -- 大厅广告
                                                -- local pl = PlazaAdLayer:create(this, {}, PlazaAdLayer.SECOND_LEVEL)
                                                -- this._scenePopLayer:addChild(pl)
                                                -- pl:setVisible(false)
                                                -- pl:runAction(cc.Sequence:create(cc.DelayTime:create(2.5), cc.Show:create()))
                                                -- pl:setRemoveListener(function()
                                                --     print("ClientScene PlazaAdLayer remove")
                                                --     this:popReward()
                                                -- end)
                                                GlobalUserItem.setTodayNoAdNotice( true )
                                            else
                                                print("---------=============------------------->", GlobalUserItem.tabDailyInfo.bTodayChecked)
                                                if not GlobalUserItem.tabDailyInfo.bTodayChecked and GlobalUserItem.tabSystemConfig.IsOpenGameSignIn ==0 then 
                                                    this:popDailyBonus()
                                                else
                                                    this:popReward()
                                                end
                                            end
                                end)
                        self._modifyFrame:onQueryUserInfo()
                        
                    end)
                end
            end
        end
    end)
    -- 单个游戏请求约战配置
    if GlobalUserItem.bSingleGame then
        -- 约战配置
        local tPrivate = 
        {
            nOprateCode = QueryQueueFrame.OP_ENUM.OP_ROOMPARAM,
            szServer = yl.LOGONSERVER,
            nPort = yl.LOGONPORT
        }
        self._queryFrame:addToQueryList(tPrivate)
    end
    -- 喇叭查询
    local trumpet = 
    {
        nOprateCode = QueryQueueFrame.OP_ENUM.OP_QUERYTRUMPET,
        szServer = yl.LOGONSERVER,
        nPort = yl.LOGONPORT
    }
    self._queryFrame:addToQueryList(trumpet)
    -- 附加查询
    local uinfo = 
    {
        nOprateCode = QueryQueueFrame.OP_ENUM.OP_QUERYUSERINFO,
        szServer = yl.LOGONSERVER,
        nPort = yl.LOGONPORT
    }
    self._queryFrame:addToQueryList(uinfo)

    self:queryUserScoreInfo(function()
            -- 弹窗奖励
            if self.popReward then
                self:popReward()
            end
        end)
    -- 开始查询
    self._queryFrame:startQuery()
end

-- 网络回调
function ClientScene:onFrameEngineCallBack(code,message)
    print("onFrameEngineCallBack:", code)
    if message then
        showToast(self, message, 1)
    end
    if code == -1 then
        self:dismissPopWait()
        local curScene = self._sceneLayer:getCurrentTag()
        if curScene == yl.SCENE_GAME then
            local curScene = self._sceneLayer:getCurrentLayer()
            if curScene and curScene.onExitRoom then
                curScene:onExitRoom()
            else
                self:onKeyBack()
                if MatchRoom then 
                    MatchRoom:getInstance():onExitPlaza()
                end
            end

            
        end
    end
end

function ClientScene:onLogonCallBack(result,message)
    if result == 10 then --成功
        if GlobalUserItem.tabAccountInfo.dwLockServerID ~= 0 then
            self:logonRoomByLockServerID(GlobalUserItem.tabAccountInfo.dwLockServerID)
        else
            self:dismissPopWait()
            local curScene = self._sceneLayer:getCurrentTag()
            if curScene == yl.SCENE_GAME then
                    self:onKeyBack()
                    if MatchRoom then 
                        MatchRoom:getInstance():onExitPlaza()
                    end    
            end
        end
    elseif result == -1 then --失败
        if type(message) == "string" and message ~= "" then
            showToast(self,message,2,cc.c4b(250,0,0,255));
        end
    end
end


-- 后台回调
function ClientScene:onBackgroundCallBack( bEnter )
    if not bEnter then
        print("onBackgroundCallBack not bEnter")
        if nil ~= self._gameFrame and self._gameFrame:isSocketServer() and GlobalUserItem.bAutoConnect then         
            self._gameFrame:onCloseSocket()
        end

        if ClubFrame:getInstance():isSocketServer() then
            ClubFrame:getInstance():onCloseSocket()
        end

        self._sceneLayer:setKeyboardEnabled(false)
    else
        print("onBackgroundCallBack  bEnter")
        local curTag = self._sceneLayer:getCurrentTag()
        if curTag == yl.SCENE_GAME then               
            if self._gameFrame:isSocketServer() == false and GlobalUserItem.bAutoConnect then
                if GlobalUserItem.bLookOn then
                    self:onKeyBack()
                else
                     self._gameFrame:OnResetGameEngine()
                    -- 约战重连特殊处理
                    if GlobalUserItem.bPrivateRoom and PriRoom and PriRoom:getInstance().m_tabPriData then
                        PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))
                    elseif MatchRoom and GlobalUserItem.bMatch then
                        self._logonFrame:onGetOffLineInfo()
                    else

                        self:onStartGame()
                    end                     
                end

            end
        end

        --俱乐部重连
        ClubFrame:getInstance():reSetAndLogin()

        -- 查询财富
        if GlobalUserItem.bShopPay then
            GlobalUserItem.bShopPay = false
            self:dismissPopWait()
            -- 
            if GlobalUserItem.bJftPay then
                --通知查询     
                local eventListener = cc.EventCustom:new(yl.RY_JFTPAY_NOTIFY)
                cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
            end
			
			--通知查询金币   
            local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
            eventListener.obj = yl.RY_MSG_USERRECHARGE
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
        end
        -- 检查查询
        if nil ~= self._queryFrame then
            self:showPopWait()
            self._queryFrame:checkQueryList()
        else
            if not self:handleLaunchData() then
                self:handleDeviceCopyRoomID()

                --self:handleDeviceCopyData()
            end
        end
        -- 头像重载
        local event = cc.EventCustom:new(appdf.CLIENT_NOTIFY)
        event.what = yl.CLIENT_MSG_BACKGROUND_ENTER
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)

        self._sceneLayer:setKeyboardEnabled(true)
    end

    if MatchRoom and  self._sceneLayer:getCurrentTag()==yl.SCENE_MATCH then
        MatchRoom:getInstance():onBackgroundCallBack(bEnter)
    end
    
end

function ClientScene:onReQueryFailure(code, msg)
    print("房间请求失败")
    self:dismissPopWait()
    if nil ~= msg and type(msg) == "string" then
        showToast(self,msg,2)
    end
    GlobalUserItem.nCurRoomIndex = -1
    GlobalUserItem.bPrivateRoom = false
    GlobalUserItem.bPrivateGoldRoom = false      --金币私人房
    GlobalUserItem.bMatch = false
    self._gameFrame:onCloseSocket()
end

function ClientScene:onEnterRoom()
    print("client onEnterRoom")
    self._gameFrame:SitDown(yl.INVALID_TABLE,yl.INVALID_CHAIR)
end

function ClientScene:onEnterTable()
    print("ClientScene onEnterTable")
    if PriRoom and GlobalUserItem.bPrivateRoom then
        -- 动作记录
        PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_ENTERTABLE
    end
    local tag = self._sceneLayer:getCurrentTag()
    if tag == yl.SCENE_GAME then
        self._gameFrame:setViewFrame(self._sceneLayer:getCurrentLayer())
    else
        self:onChangeShowMode(yl.SCENE_GAME)
    end
end

--启动游戏
function ClientScene:onStartGame()
    local app = self:getApp()
    local entergame = self:getEnterGameInfo()
    if nil == entergame then
        showToast(self, "游戏信息获取失败", 3)
        self:dismissPopWait()
        return
    end
    self:getEnterGameInfo().nEnterRoomIndex = GlobalUserItem.nCurRoomIndex
    self:showPopWait()
    self._gameFrame:setScene(self)
    self._gameFrame:onInitData()
    self._gameFrame:setKindInfo(GlobalUserItem.nCurGameKind, entergame._KindVersion)
    self._gameFrame:setViewFrame(self)
    self._gameFrame:onCloseSocket()
    self._gameFrame:onLogonRoom()
end

function ClientScene:onEnterTransitionFinish()
    -- 注册通知
    self:registerNotifyList()

    self.m_listener = cc.EventListenerCustom:create(appdf.CLIENT_NOTIFY,handler(self, self.onClientNotify))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)

    -- 初始化查询
    self:startInitQuery()
    -- 解除屏蔽
    ExternalFun.dismissTouchFilter()
end

function ClientScene:onExit()
    removebackgroundcallback()
    self:unregisterNotify()
    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
        self.m_listener = nil
    end
    -- 断开网络
    if nil ~= self._queryFrame then
        self._queryFrame:onCloseSocket()
        self._queryFrame = nil
    end
    if nil ~= self._gameFrame then
        self._gameFrame:setScene(nil)
        self._gameFrame:onCloseSocket()
        self._gameFrame = nil
    end

    PriRoom:getInstance():onExitPlaza()
    GameVideo:getInstance():onExitPlaza()
    if MatchRoom  then
        MatchRoom:getInstance():onExitPlaza()
    end
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
end

function ClientScene:registerNotifyList()
    -- 代理帐号不显示
    if GlobalUserItem.bIsAngentAccount then
        return
    end
    --喇叭通知
    local notify7 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_TRUMPET_NOTIFY)
    notify7.name = "trumpet"
    notify7.group = "client_trumpet"
    notify7.fun = handler(self,self.onNotify)
    NotifyMgr:getInstance():registerNotify(notify7)

     --系统消息
    local systemNotify=NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_SYSTEM_MESSAGE)
    systemNotify.name="trumpet"
    systemNotify.group = "client_trumpet"
    systemNotify.fun=handler(self,self.onNotify)
    NotifyMgr:getInstance():registerNotify(systemNotify)
end

function ClientScene:unregisterNotify()
    -- 代理帐号不显示
    if GlobalUserItem.bIsAngentAccount then
        return
    end
    NotifyMgr:getInstance():unregisterNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_TRUMPET_NOTIFY, "trumpet")
end

--网络通知
function ClientScene:onNotify(msg)
    local bHandled = false
    local main = msg.main or 0
    local sub = msg.sub or 0
    local name = msg.name or ""
    local param = msg.param
    local group = msg.group
    
    --喇叭消息
    if group == "client_trumpet" 
        and type(msg.param) == "table" then
        --dump(msg.param, "msg.param", 6)
        local item = {}
        item.title = msg.param.szNickName or "系统"                                      -- 消息标题
        if msg.param.bSysMsg then
            item.content = msg.param.szString or ""
            item.content = string.gsub(item.content, "\n", "")
            item.marqueetitle = msg.param.szString
            item.ntype = yl.SYSTEM_MSG 
        else
            item.content = msg.param.szMessageContent or ""                              -- 消息内容
            item.content = string.gsub(item.content, "\n", "")
            item.marqueetitle = item.title .. " 说: " .. item.content                    -- 跑马灯标题
            item.ntype = yl.TRUMPET_MSG 
        end
        item.color = cc.c4b(255,191,123,255)                                         -- 消息颜色
        item.autoremove = true                                                       -- 自动移除
        item.showcount = 2                                                           -- 显示次数
        item.showtitle = false                                                       -- 是否显示标题(否则跑马灯标题)
        --item.ntype = yl.TRUMPET_MSG
        if self.getNoticeId then                                                  -- 消息类型
            item.id = self:getNoticeId()   -- 消息id
        end                                     
        item.time = os.date("%H:%M:%S", os.time())                                   -- 消息时间
        -- 20170804 游戏中已经播放的喇叭消息不需要再次播放
        --table.insert( self.m_vecNotice, item ) 
        bHandled = true
        -- 记录数据
        FriendMgr:getInstance():insertSystemMsg(item)

        -- 游戏界面
        if nil ~= self._sceneLayer then
            local curTag = self._sceneLayer:getCurrentTag()
            if curTag == yl.SCENE_GAME then
                local cur_layer = self._sceneLayer:getCurrentLayer()
                if nil ~= cur_layer and nil ~= cur_layer.onAddGameTrumpet then
                    cur_layer:onAddGameTrumpet(item)
                end
            -- 大厅界面
            elseif curTag == yl.SCENE_PLAZA then
                local cur_layer = self._sceneLayer:getCurrentLayer()
                if nil ~= cur_layer then
                    cur_layer:onAddNotice(item)
                end
            end
        end
    end
    return bHandled
end

-- 获取id
function ClientScene:getNoticeId()
    local tmp = self.m_nNotifyId
    self.m_nNotifyId = self.m_nNotifyId + 1
    return tmp
end

-- 截屏通知
function ClientScene:onClientNotify( event )
    local what = event.what
    local msg = event.msg
    if what == yl.CLIENT_MSG_TAKE_SCREENSHOT then
        print("截屏啦")
        local gameLayer = self._sceneLayer:getChildByTag(yl.SCENE_GAME)
        if nil ~= gameLayer and nil ~= gameLayer.onTakeScreenShot then
            gameLayer:onTakeScreenShot(msg)
        end
    elseif what == yl.CLIENT_MSG_HTTP_WEALTH then
        self:dismissPopWait()
        local jstable = event.jstable
        dump(jstable, "jstable", 6)
        if type(jstable) == "table" then
            local data = jstable["data"]
            if type(data) == "table" then
                local valid = data["valid"]

                if true == valid then
                    local diamond = tonumber(data["diamond"]) or 0
                    local needupdate = false
                    if diamond ~= GlobalUserItem.tabAccountInfo.lDiamond then
                        needupdate = true
                        GlobalUserItem.tabAccountInfo.lDiamond = diamond
                    end

                    -- 金币
                    local score = tonumber(data["score"]) or 0
                    if nil ~= score and score ~= GlobalUserItem.tabAccountInfo.lUserScore then
                        needupdate = true
                        GlobalUserItem.tabAccountInfo.lUserScore = score
                    end
                    if needupdate then
                        print("update score")
                        --通知更新        
                        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                        eventListener.obj = yl.RY_MSG_USERWEALTH
                        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                    end 
                end
            end
        end
        local queryCallBack = event.queryCallBack
        if type(queryCallBack) == "function" then
            queryCallBack()
        end
    elseif what == yl.CLIENT_MSG_PRIVATEROOM_REFRESH then
        self:onReloadRecordList( true )
    elseif what == yl.CLIENT_MSG_IOS_OPENURL then
        -- ios 从后台切回来，先来 back，再来openurl，所以需要额外获取一次
        self:handleLaunchData()
    elseif what == yl.CLIENT_MSG_SCREEN_CHANGE then
        yl.SCREEN_CHANGE     = tonumber(msg)
    end
end

-- 退出大厅
function ClientScene:exitPlaza()
    -- 删除授权
    if MultiPlatform:getInstance():isAuthorized(yl.ThirdParty.WECHAT) then
        print("OptionLayer 删除微信授权")
        MultiPlatform:getInstance():delAuthorized(yl.ThirdParty.WECHAT)
    end
	self._sceneLayer:setKeyboardEnabled(false)
    --断开好友服务器
    FriendMgr:getInstance():reSetAndDisconnect()
    --通知管理
    NotifyMgr:getInstance():clear()
    -- 私人房数据
    PriRoom:getInstance():reSet()
    -- 回放
    GameVideo:getInstance():reSet()
    --俱乐部重置
    ClubFrame:getInstance():reSetAndDisconnect()
    --比赛
    if MatchRoom then
       MatchRoom:getInstance():reSet()
    end
    -- 重置
    GlobalUserItem.reSetData()
    --读取配置
    GlobalUserItem.LoadData()
	self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.LogonScene","FADE",1)
end

-- 回退
function ClientScene:onKeyBack()
    if not self.m_bEnableKeyBack then
        print("ClientScene onKeyBack 不允许回退")
        return
    end    
    self:onChangeShowMode()
end

-- 切换界面
function ClientScene:onChangeShowMode(nTag, param, transitionCallBack)
    self._sceneLayer:onChangeShowMode(nTag, param)
end

-- 切换结束
function ClientScene:onChangeShowModeEnd( lastTag, curTag )
    -- 从游戏退出
    if yl.SCENE_GAME == lastTag or  curTag == yl.SCENE_PLAZA then
        -- 查询财富

        self:queryUserScoreInfo(function()
            -- 弹窗奖励
            if self.popReward then
                self:popReward()
            end
        end)
    end
end

--显示等待
function ClientScene:showPopWait(isTransparent,delay1,delay2,delay3)

    if not self._popWait then
        self._popWait = ClientPopWait.PopWait:create(isTransparent,delay1,delay2,delay3)
            :show(self,"请稍候！")
        self._popWait:setLocalZOrder(yl.MAX_INT)
    end
end

--关闭等待
function ClientScene:dismissPopWait()

    if self._popWait then
        self._popWait:removeFromParent()
        self._popWait = nil
    end
    self.m_bIsReConnet = false
end


function ClientScene:logonRoomByLockServerID(wServerID)
    --查找房间信息
                for i = 1,#GlobalUserItem.roomlist do
                    local list = GlobalUserItem.roomlist[i]
        if tonumber(list[1]) == tonumber(GlobalUserItem.nCurGameKind) then
            local listinfo = list[2]
            if not listinfo then
                print("not listinfo")
                return nil
            end
            for j=1, #listinfo do
                local info = listinfo[j]
                if info.wServerID == wServerID then
                    GlobalUserItem.nCurRoomIndex = j
                    --判断拉回房间的类型
                    if info.wServerType ~= 16 then
                        GlobalUserItem.bPrivateRoom = false
                    else
                        GlobalUserItem.bPrivateRoom = true
                    end
                    -- 记录进入游戏
                    self:updateCurrentGame()
                   
                    self:onStartGame()
                    break
                end
            end
        end
                end
end
-- 俱乐部服务器断网等待
function ClientScene:showClubSeverReConnect()
        if nil ~= self._popWait then
            self._popWait:removeFromParent()
            self._popWait = nil
        end
        local curTag = self._sceneLayer:getCurrentTag()
        if curTag == yl.SCENE_CLUB or  curTag == yl.SCENE_CLUB_ROOM  then
                self.m_bIsReConnet = true
                local reconnect = ClientPopWait.ReConnectPopWait:create(20, function(dt)


                self.m_bIsReConnet = true
                if dt >0 then

                    --4秒一次
                    if dt%4==0 then
                        ClubFrame:getInstance():reSetAndLogin()
                    end

                else
                    print("重连超时")
                    self:dismissPopWait()  
                    self.m_bIsReConnet = true
                    ClubFrame:getInstance():onCloseSocket()
                    local curTag = self._sceneLayer:getCurrentTag()
                            self.m_netQuery = QueryDialog:create("您的网络出现异常！", function(ok)
                                self.m_bIsReConnet = false
                                if ok then
                                    self:onChangeShowMode(yl.SCENE_PLAZA)
                                else
                                    self:showClubSeverReConnect()
                                end
                                self.m_netQuery = nil
                            end,24,QueryDialog.QUERY_SURE_CANCEL,"重连","退出")
                            :setCanTouchOutside(false)
                            :addTo(self)
                    end
                end)
                self:addChild(reconnect)
                reconnect:setLocalZOrder(yl.MAX_INT)
                self._popWait = reconnect
        end


end

-- 游戏服务器断网等待
function ClientScene:showGameSeverReConnect()
    if nil ~= self._popWait then
        self._popWait:removeFromParent()
        self._popWait = nil
    end
    local reconnect = ClientPopWait.ReConnectPopWait:create(20, function(dt)
        self.m_bIsReConnet = true
        if dt >0 then
            print("尝试重连")
            --4秒一次
            if dt%4==0 then
                self._gameFrame:OnResetGameEngine()
                if GlobalUserItem.bPrivateRoom and PriRoom and PriRoom:getInstance().m_tabPriData then
                    PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))
                else
                    self:onStartGame()
                end

                    ClubFrame:getInstance():reSetAndLogin()

            end
        else
            print("重连超时")
            self:dismissPopWait()  
            self.m_bIsReConnet = true
            self._gameFrame:onCloseSocket()
            self._gameFrame:removeNetQuery()
            local curTag = self._sceneLayer:getCurrentTag()
            if curTag == yl.SCENE_GAME then
                
                self.m_netQuery = QueryDialog:create("您的网络出现异常！", function(ok)
                    self.m_bIsReConnet = false
                    if ok then
                        GlobalUserItem.bIsNeedConnect = true
                        self:onKeyBack()
                    else
                        self:showGameSeverReConnect()
                    end
                    self.m_netQuery = nil
                end,24,QueryDialog.QUERY_SURE_CANCEL,"重连","退出")
                :setCanTouchOutside(false)
                :addTo(self)

            end

        end
    end)
    self:addChild(reconnect)
    reconnect:setLocalZOrder(yl.MAX_INT)
    self._popWait = reconnect
end



function ClientScene:onSubGroupEnterSuccess()
    local curTag = self._sceneLayer:getCurrentTag() --游戏界面不需要切换场景
    if curTag ~= yl.SCENE_GAME then
        self:dismissPopWait()
        self:onChangeShowMode(yl.SCENE_CLUB_ROOM)
    end
end


function ClientScene:enterClub(dwGroupID)
        self:showPopWait()
        ClubFrame:getInstance().m_nCurrentClub = dwGroupID
        ClubFrame:getInstance():OnResetClubData()
        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendEnterGroup(dwGroupID)
end

function ClientScene:updateClubIcon(event)

            local curTag = self._sceneLayer:getCurrentTag()
            if curTag == yl.SCENE_PLAZA then
                local plaza = self._sceneLayer:getCurrentLayer()
                if plaza.m_gameListLayer then
                    plaza.m_gameListLayer:onClubInfoChange(event)
                end
            end
end


-- 刷新当前游戏
function ClientScene:updateCurrentGame()
    local info = self:getGameInfo(GlobalUserItem.nCurGameKind)
    dump(info, "client scene error", 6)
    self:updateEnterGameInfo(info)
end

-- 更新进入游戏记录
function ClientScene:updateEnterGameInfo( info )
    GlobalUserItem.m_tabEnterGame = info
end

-- 获取进入游戏记录
function ClientScene:getEnterGameInfo(  )

    return GlobalUserItem.m_tabEnterGame
end

-- 获取游戏信息
function ClientScene:getGameInfo(wKindID)
    for k,v in pairs(self:getApp()._gameList) do
        if tonumber(v._KindID) == tonumber(wKindID) then
            return v
        end
    end
    return nil
end

-- 语音按钮
function ClientScene:createVoiceBtn(pos, zorder, parent)
    self._sceneLayer:createVoiceBtn(pos, zorder, parent)
end

-- 语音控制
function ClientScene:startVoiceRecord()
    self._sceneLayer:startVoiceRecord()
end

function ClientScene:stopVoiceRecord()
    self._sceneLayer:stopVoiceRecord()
end

function ClientScene:cancelVoiceRecord()
    self._sceneLayer:cancelVoiceRecord()
end

-- 游戏更新
function ClientScene:updateGame(dwKindID)
    return false
end

-- 链接游戏
function ClientScene:loadGameList(dwKindID)
    return false
end

-- 处理启动数据
function ClientScene:handleLaunchData()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    local roomlayer = self._sceneLayer:getChildByTag(yl.SCENE_ROOMLIST)
    if nil == plazalayer and roomlayer == nil then
        print("ClientScene:handleLaunchData plazalayer == nil")
        return false
    end
    --self:removePop() 20170817屏蔽移除

    -- 获取启动数据
    local szLaunchData = MultiPlatform:getInstance():getLaunchData()
    if type(szLaunchData) == "string" and "" ~= szLaunchData then
        -- 解析参数
        local tabParamList = appdf.split(szLaunchData, "&")
        local paramList = {}
        for k,v in pairs(tabParamList) do
            local param = appdf.split(v, "=")
            if #param == 2 and param[1] ~= nil then
                paramList[param[1]] = param[2]
            end
        end
        -- 获取动作
        local action = tonumber(paramList["action"])
        if yl.URL_PRIVATE_ACTION == action then


            -- 约战
            local roomid = paramList["roomid"]
            local kindid = tonumber(paramList["kindid"])
            PriRoom:getInstance().m_szPasswd = paramList["password"] or ""
            if nil ~= roomid and nil ~= kindid then
                self:removeAllPop()
                self._scenePopLayer:removeChildByName("_ShopLayer_")

                GlobalUserItem.nCurGameKind = kindid
                PriRoom:getInstance():getNetFrame():onSearchRoom(roomid)

                -- 处理复制内容
                self:isContainSameRoomID(roomid, true)
                return true
            end
        elseif yl.URL_VIDEO_ACTION == action then
            -- 回放
            self:removeAllPop()

            -- 处理复制内容
            self:isContainSameRoomID(roomid, true)
            return true
        elseif yl.URL_CLUB_ACTION == action then

            local groupId = tonumber(paramList["gr"])
            local code = tonumber(paramList["code"])
            ClubFrame:getInstance().m_curQueryClub = groupId
            ClubFrame:getInstance():sendApplyRequest(groupId, 0,code)

            return true
        end
    end
    return false
end




-- 处理复制数据
function ClientScene:handleDeviceCopyData( handleFun )
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil == plazalayer then
        print("ClientScene:handleDeviceCopyData plazalayer == nil")
        return
    end
    --self:removePop() 20170817屏蔽移除

    -- 获取复制内容
    MultiPlatform:getInstance():getCopyBoardData(function(szCopyData)
        if type(szCopyData) == "string" and "" ~= szCopyData then
            -- 匹配回放
            local bMatchVideo = string.match(szCopyData, "###" .. "***" .. "###")
            -- 匹配约战
            local bMatchPrivate = string.match(szCopyData, "***" .. "###" .. "***")
            if not bMatchVideo and not bMatchPrivate then
                print("非匹配复制内容 ==> ", szCopyData)
                if type(handleFun) == "function" then
                    handleFun()
                end
                return
            end
            self:removeAllPop()

            -- 匹配id
            local id = string.match(szCopyData, "%[(%d+)%]")
            if type(id) == "string" and string.len(id) == 6 then
                if bMatchVideo then
                    if nil ~= plazalayer.popVideoCodeEdit then
                        plazalayer:popVideoCodeEdit(id)
                    end
                elseif bMatchPrivate then
                    -- 获取密码
                    PriRoom:getInstance().m_szPasswd = string.match(szCopyData, "%{(%d+)%}") or ""
                    if nil ~= plazalayer.popPrivateIdEdit then
                        plazalayer:popPrivateIdEdit(id)
                    end
                end
                -- 重置粘贴板
                --MultiPlatform:getInstance():copyToClipboard("")
            end
        else
            if type(handleFun) == "function" then
                handleFun()
            end
        end
    end)
end

-- 处理复制数据,判断是否满足房间id
function ClientScene:handleDeviceCopyRoomID()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil == plazalayer then
        print("ClientScene:handleDeviceCopyData plazalayer == nil")
        return
    end
    print("handleDeviceCopyRoomID------------------------------------------------------------!!!!")

    -- 获取复制内容
    MultiPlatform:getInstance():getCopyBoardData(function(szCopyData)
        if type(szCopyData) == "string" and "" ~= szCopyData then
            self:removeAllPop()

            -- 匹配id
            local id = string.match(szCopyData, "%d+")
            if type(id) == "string" and string.len(id) == 6 then
                --弹出提示，询问玩家是否进入该房间
                local query = QueryDialog:create("是否加入房间:" .. id .. "?", function(ok)
                    if ok == true then
                        -- 查找进入私有房
                        PriRoom:getInstance():getNetFrame():onSearchRoom(tonumber(id))
                    end
                    query = nil
                end):setCanTouchOutside(false)
                    :addTo(plazalayer)
  
                -- 重置粘贴板
                MultiPlatform:getInstance():copyToClipboard("")
            end
        end
    end)
end

-- 处理同id复制内容
function ClientScene:isContainSameRoomID( roomid, bClean )
    if type(roomid) ~= "string" then
        return
    end
    -- 获取复制内容
    MultiPlatform:getInstance():getCopyBoardData(function(szCopyData)
        if type(szCopyData) == "string" and "" ~= szCopyData then
            -- 匹配回放
            local bMatch = string.match(szCopyData, roomid)
            if bMatch and bClean then
                -- 重置粘贴板
                MultiPlatform:getInstance():copyToClipboard("")
            end
        end
    end)
end

function ClientScene:getPopLevel()
    local popLevel = nil
    local popList = self._scenePopLayer:getChildren()
    local popCount = #popList
    if 0 ~= popCount then
        local lastPop = popList[popCount]
        if nil ~= lastPop.getLevel then
            popLevel = popList[popCount]:getLevel() + 1
        else
            popLevel = TransitionLayer.SECOND_LEVEL
        end
    else
        popLevel = TransitionLayer.SECOND_LEVEL
    end
    return popLevel
end

-- 第三方分享
-- @parma[target] (0 无  1 朋友圈  2 微信好友  3 朋友圈和微信好友 4 面对面  5 朋友圈和面对面  6 微信好友和面对面  7 全部)
function ClientScene:popTargetShare( callback, target, level )
    local popLevel = level
    if nil == popLevel then
        popLevel = self:getPopLevel()
    end
    local tl = TargetShareLayer:create(self, {callback = callback, target = target}, popLevel)
    self:addPopLayer(tl)
end

-- 查询用户财富
function ClientScene:queryUserScoreInfo( queryCallBack )
    local url = yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx"
    self:showPopWait()
    local param = "action=getuserwealth&" .. GlobalUserItem:urlUserIdSignParam()
    --logFunc(param, true)
    LogAsset:getInstance():logData("clientscene:queryUserScoreInfo "..os.date()) --debug match
    appdf.onHttpJsionTable(url ,"GET",param,function(jstable,jsdata)
        LogAsset:getInstance():logData("clientscene:queryUserScoreInfo responese "..os.date()) --debug match
        local event = cc.EventCustom:new(yl.CLIENT_NOTIFY)
        event.sender = self
        event.jstable = jstable
        event.what = yl.CLIENT_MSG_HTTP_WEALTH
        event.url = url
        event.queryCallBack = queryCallBack
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
    end)
end

-- 奖励推送
function ClientScene:popReward()
    if GlobalUserItem.bEnableIdentify and not self.bPopIdentify and not GlobalUserItem.isIdentifyGuide() and not GlobalUserItem.isRealAuth() then
        self.bPopIdentify = true
        self:popIdentify()
        -- 增加次数
        GlobalUserItem.setTodayGuide()
    elseif 0 ~= #GlobalUserItem.tabWeathRewardList then
        -- local item = GlobalUserItem.tabRewardList[1]
        -- table.remove(GlobalUserItem.tabRewardList,1)
        local item = {}
        item.tablist =  clone(GlobalUserItem.tabWeathRewardList)
        item.ntype = 1
        --dump(item, "ClientScene:popReward(", 7)
        local popLevel = self:getPopLevel()
        local r = RewardLayer.RankRewardLayer:create(self, item, self:getPopLevel())
        self:addPopLayer(r)
        print("popReward RankRewardLayer !!!!")
        r:setRemoveListener(function()
            self:popReward()
        end)
        GlobalUserItem.tabWeathRewardList = {}
    elseif 0 ~= #GlobalUserItem.tabWinTimesRewardList then
         local item = {}
        item.tablist =  clone(GlobalUserItem.tabWinTimesRewardList)
        item.ntype = 2
        local popLevel = self:getPopLevel()
        local r = RewardLayer.RankRewardLayer:create(self, item, self:getPopLevel())
        self:addPopLayer(r)
        print("popReward RankRewardLayer !!!!")
        r:setRemoveListener(function()
            self:popReward()
        end)
        GlobalUserItem.tabWinTimesRewardList = {}
    elseif 0 ~= #GlobalUserItem.tabRewardList then
        local item = {}
        item = clone(GlobalUserItem.tabRewardList[1])
        local popLevel = self:getPopLevel()
        local rl = RewardLayer:create(self, item, popLevel)
        self:addPopLayer(rl)
        rl:setRemoveListener(function()
            self:popReward()
        end)
        GlobalUserItem.tabRewardList = {}
    else
        -- 更新界面
        local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
        if nil ~= plazalayer and nil ~= plazalayer.refreshPlaza then
            plazalayer:refreshPlaza()
            plazalayer:plazaAnimation()
        end
        local joinedGame = PriRoom:getInstance().m_tabJoinGameRecord[GlobalUserItem.nCurGameKind]
        -- 暂离记录
        if nil ~= joinedGame and nil ~= joinedGame.roomid then
            self:showPopWait()
            PriRoom:getInstance():setViewFrame(self)
            -- 创建列表
            PriRoom:getInstance():getNetFrame():onQueryRoomList()
        else
            -- 必须参与
            if 1 == PriRoom:getInstance().cbIsJoinGame then
                self:showPopWait()
                PriRoom:getInstance():setViewFrame(self)
                -- 创建列表
                PriRoom:getInstance():getNetFrame():onQueryRoomList()
            end
        end
    end
end

-- 奖励推送
function ClientScene:popDailyBonus()
    local popLevel = self:getPopLevel()
    local pa = DailyBonusLayer:create(self, {}, popLevel)
    self:addPopLayer(pa)
    pa:setRemoveListener(function()
        self:popReward()
    end)
end


-- 认证引导
function ClientScene:popIdentify()
    local popLevel = self:getPopLevel()
    local popLevel = self:getPopLevel()
    local il = UserCenterLayer.IdentifyLayer:create(self, {}, popLevel)
    self:addPopLayer(il)
    il:setRemoveListener(function()
        self:popReward()
    end)
end

-- 移除一个弹窗
function ClientScene:removePop()
    local bHandle = false
    local popList = self._scenePopLayer:getChildren()
    local popCount = #popList
    if popCount > 0 then
        local lastChild = popList[popCount]
        if nil ~= lastChild then
            if nil ~= lastChild.onKeyBack then
                bHandle = lastChild:onKeyBack()
            end
            if not bHandle then
                -- 动画
                if nil ~= lastChild.animationRemove then
                    lastChild:animationRemove()
                else
                    lastChild:removeFromParent()
                end
                bHandle = true
            end
        end
    end
    return bHandle
end

-- 移除所有弹窗
function ClientScene:removeAllPop()
    -- 移除弹窗
    local popList = self._scenePopLayer:getChildren()
    local popCount = #popList
    for k,v in pairs(popList) do
        if v:isVisible() then
            if v.animationRemove then
                v:animationRemove()
            else
                v:removeFromParent()
            end
        else
            v:removeFromParent()
        end
    end
end

-- 消息
function ClientScene:popNotice()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.popNotice then
        plazalayer:popNotice()
    end
end

-- 广告
function ClientScene:popAdward()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.popAdward then
        plazalayer:popAdward()
    end
end

-- 设置
function ClientScene:popSet()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.popSet then
        plazalayer:popSet()
    end
end

-- 战绩
function ClientScene:popMark()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.popMark then
        plazalayer:popMark()
    end
end

-- 分享
function ClientScene:popSpread()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.popSpread then
        plazalayer:popSpread()
    end
end

-- 客服
function ClientScene:popService()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.popService then
        plazalayer:popService()
    end
end

-- 商店
function ClientScene:popShopLayer( shopType )
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.popShopLayer then
        plazalayer:popShopLayer(shopType)
    else
            -- 是否开启
            if GlobalUserItem.tabSystemConfig.IsOpenMall == yl.ENABLE_VALUE then
                local sl = ShopLayer:create(self, shopType, ShopLayer.SECOND_LEVEL)
                self:addPopLayer(sl)
            else
                local hl = HelpLayer:create(self, shopType, HelpLayer.SECOND_LEVEL)
                self:addPopLayer(hl)
            end

    end
end

-- 房间
function ClientScene:enterRoomList()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.enterRoomList then
        plazalayer:enterRoomList()
    end
end

-- 数据回调
function ClientScene:onReloadRecordList( keepFrame )
    if not keepFrame then
        self:dismissPopWait()
        PriRoom:getInstance():setViewFrame(nil)
    end

    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.onGetCreateList then
        plazalayer:onGetCreateList()
    end
end

-- 单游戏更新
function ClientScene:singleGameUpdate(nKindID, callback)
    local updategame = self:getGameInfo(nKindID)
    if nil == updategame or type(callback) ~= "function" then
        print("更新游戏为空, 或者更新回调函数异常")
        return false
    end
    local param = {}
    param._nKindID = nKindID
    param._callback = callback
    param._updategame = updategame
    local singleupdater = GameListLayer.GameUpdater:create(self, param, self:getPopLevel())
    self:addPopLayer(singleupdater)
    return true
end

-- 弹出创建
function ClientScene:popCreate()
    local plazalayer = self._sceneLayer:getChildByTag(yl.SCENE_PLAZA)
    if nil ~= plazalayer and nil ~= plazalayer.popCreate then
        plazalayer:popCreate()
    end
end
return ClientScene