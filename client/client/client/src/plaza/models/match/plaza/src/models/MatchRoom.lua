--
-- Author: zhouweixiang
-- Date: 2017-02-13 14:36:37
-- 比赛数据管理 全局模式

MatchRoom = MathcRoom or class("MatchRoom")
local cmd_match = appdf.req(appdf.CLIENT_SRC .. "plaza.models.match.header.CMD_Match")
local struct_match = appdf.req(appdf.CLIENT_SRC .. "plaza.models.match.header.Struct_Match")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

-- 比赛模块
local MODULE = cmd_match.define.tabModule
MatchRoom.MODULE = MODULE
-- 比赛界面tag
local LAYTAG = cmd_match.define.tabLayTag
MatchRoom.LAYTAG = LAYTAG

--比赛定义
local DEFINE = cmd_match.define
MatchRoom.DEFINE = DEFINE

-- 游戏服务器CMD
local cmd_match_game = cmd_match.game
MatchRoom.CMD_MATCH = cmd_match_game

local MatchFrame = appdf.req(MODULE.PLAZAMODULE .."models.MatchFrame")

local targetPlatform = cc.Application:getInstance():getTargetPlatform()

function MatchRoom:ctor()
    -- 比赛资源搜索路径
    self._searchPath = device.writablePath.."client/src/plaza/models/match/plaza/res"
    cc.FileUtils:getInstance():addSearchPath(self._searchPath)
    -- 比赛游戏资源搜索路径
    self._gameSearchPath = ""
    
    --网络回调
    local matchCallBack = function(command, message, dataBuffer, notShow)
       
         -- LogAsset:getInstance():logData(message)
        if type(command) == "table" then
             -- LogAsset:getInstance():logData(cjson.encode(command))
            if command.m == cmd_match_game.MDM_GR_MATCH then
                return self:onMatchGameServerMessage(command.s, message, dataBuffer, notShow)
            elseif command.m == DEFINE.MDM_CM_SYSTEM then
                    local runScene = cc.Director:getInstance():getRunningScene()
                    if nil ~= runScene then
                        showToast(runScene, message, 2)
                    end 
                    print(command.s.."===============9999")
                    if command.s == DEFINE.SMT_CLOSE_ROOM then
                        if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubMatchUStatus then
                            self._viewFrame:onSubMatchUStatus(DEFINE.MUS_NULL)
                        end
                        return 
                    end
            
                if command.s == DEFINE.SYSMESSAGE_WAIT_RESULT then
                    if self._scene == nil then
                        return
                    end
                    GlobalUserItem.bWaitQuit = true
                    QueryDialog:create(message, function(ok)
                        if ok == true then
                            
                        end
                    end, QueryDialog.DEF_TEXT_SIZE, QueryDialog.QUERY_SURE)
                    :setCanTouchOutside(false)
                    :addTo(self._viewFrame)
                else
                    self:popMessage(message, notShow)
                    if -1 == command.s then
                        self:dismissPopWait()
                    end
                end    
            end
        else
            self:popMessage(message, notShow)
            if -1 == command then
                self:dismissPopWait()
            end
        end
        
    end
    self._matchFrame = MatchFrame:create(self, matchCallBack)

    self:reSet()
end

-- 实现单例
MatchRoom._instance = nil
function MatchRoom:getInstance( )
    if nil == MatchRoom._instance then
        print("new instance")
        MatchRoom._instance = MatchRoom:create()
    end
    return MatchRoom._instance
end

function MatchRoom:reSet()
    -- 比赛模式游戏列表
    self.m_tabMatchModeGame = {}
    -- 比赛列表
    self.m_tabMatchRoomList = {}
    --比赛规则数据
    self.m_tabMatchRoomRuleList = {}
    --所有比赛规则数据
    self.m_tabAllMatchRule = {}

    -- 大厅场景
    self._scene = nil
    -- 网络消息处理层
    self._viewFrame = nil
    -- 私人房信息层
    self._matchView = nil
    --是否报名 
    self.bSignup = false
end

-- 当前游戏是否开启比赛模式
function MatchRoom:isCurrentGameOpenMatch( nKindID )
    return (self.m_tabMatchModeGame[nKindID] or false)
end

-- 获取比赛数据
function MatchRoom:getMatchRoomByServerID( dwServerID)
   print("dwServerID",dwServerID)
   dump(m_tabMatchRoomList, "jack 000000", 6)
    for k,v in pairs(self.m_tabMatchRoomList) do
        for tk,tv in pairs(v) do
            if tv.wServerID == dwServerID then
                return tv
            end
        end
    end
end

--获取比赛规则数据
function MatchRoom:getMatchRoomRuleByServerID( dwServerID )

  
    for k,v in pairs(self.m_tabMatchRoomRuleList) do
        if v.wServerID == dwServerID then
            return v
        end
    end
end

--获取比赛规则数据
function MatchRoom:getMatchRoomRuleByMatchID( dwMatchID )
    for k,v in pairs(self.m_tabMatchRoomRuleList) do
        if v.dwMatchID == dwMatchID then
            return v
        end
    end
end

function MatchRoom:getLoginServerID()
    return self.m_loginServerID
end

function MatchRoom:setLoginServerID(dwServerID)
    self.m_loginServerID=dwServerID
end

-- 登陆比赛房
function MatchRoom:onLoginRoom( dwServerID)

    local pServer = self:getMatchRoomByServerID(dwServerID)
    if nil == pServer then
        print("MatchRoom server null")
        return false
    end

    self.m_loginServerID=dwServerID

    -- 登陆房间
    print("比赛登录房间", self._matchFrame, self._matchFrame._gameFrame)
    if nil ~= self._matchFrame and nil ~= self._matchFrame._gameFrame then
        self:showPopWait()
        GlobalUserItem.bMatch = pServer.wServerType == yl.GAME_GENRE_MATCH
        self._matchFrame._gameFrame:setEnterAntiCheatRoom(false)
        GlobalUserItem.nCurRoomIndex = pServer._nRoomIndex
        -- LogAsset:getInstance():logData("MatchRoom:onLoginRoom")
        self._scene:onStartGame()

        return true
    end
    return false
end

function MatchRoom:onBackgroundCallBack( bEnter )
    if self._viewFrame and self._viewFrame.onBackgroundCallBack then
        self._viewFrame:onBackgroundCallBack(bEnter)
    end
end

-- 
function MatchRoom:onEnterPlaza(scene, gameFrame)
    self._scene = scene
    self._matchFrame._gameFrame = gameFrame
end

function MatchRoom:onExitPlaza()
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
    self:exitRoom()
end

--第一次启动比赛检查游戏资源 
function MatchRoom:MatchRoomCheckKindResVersion( nKindID)
    local scene = self._scene
    local app = scene:getApp()
    local game = scene:getGameInfo(nKindID)
    local version = tonumber(app:getVersionMgr():getResVersion(nKindID))
    dump(game, "Jack GAME", 6)
    print("Version.........",version)
    if not version or game._ServerResVersion > version then
        local bSucceed=scene:singleGameUpdate(nKindID, function() 
             print("更新完成！")
         end)
      
        if false ==bSucceed  then
            if nil ~= game then
                local gamename = game._GameName or ""
                local msg = string.format("请更新 %s 游戏资源!", gamename)
                showToast(scene, msg, 2)
            else
                showToast(scene, "请更新游戏资源!", 2)
            end
            return false
        end

        return true
    end
    return true
end



function MatchRoom:onEnterPlazaFinish()
    -- 判断锁表 
    print("判断锁表", GlobalUserItem.tabAccountInfo.dwLockServerID) 
    if GlobalUserItem.tabAccountInfo.dwLockServerID ~= 0 then
        GlobalUserItem.nCurGameKind = GlobalUserItem.tabAccountInfo.dwLockKindID
        -- 更新逻辑 
        if not self._scene:updateGame(GlobalUserItem.tabAccountInfo.dwLockKindID) then
            -- 
            local entergame = self._scene:getGameInfo(GlobalUserItem.tabAccountInfo.dwLockKindID)
            if nil ~= entergame then
                self._scene:updateEnterGameInfo(entergame)
                dump(entergame, "the enter game info", 6)
                --启动游戏
                print("MatchRoom:onEnterPlazaFinish ==> lock match game")
                return true, false, self:onLoginRoom(GlobalUserItem.tabAccountInfo.dwLockServerID)
            end            
        end
        print("MatchRoom:onEnterPlazaFinish ==> lock and update game")
        return true, true, false
    end
    print("MatchRoom:onEnterPlazaFinish ==> not lock game")
    return false, false, false


    --[[local this = self
    -- 判断锁表
    if GlobalUserItem.tabAccountInfo.dwLockServerID ~= 0 and GlobalUserItem.tabAccountInfo.dwLockKindID ~= 0 then
        GlobalUserItem.nCurGameKind = GlobalUserItem.tabAccountInfo.dwLockKindID

        local targetPlatform = cc.Application:getInstance():getTargetPlatform()

        -- 游戏版本检查
        if nil ~= self._scene and nil ~= self._scene.getApp then
            if cc.PLATFORM_OS_WINDOWS == targetPlatform and yl.DISABLE_WIN_UPDATE then
                print("win32 跳过更新")
            else

                local scene = this._scene
                local app = scene:getApp()
                local matchgame = scene:getGameInfo(GlobalUserItem.tabAccountInfo.dwLockKindID)
                local version = tonumber(app:getVersionMgr():getResVersion(GlobalUserItem.tabAccountInfo.dwLockKindID))
                if not version or matchgame._ServerResVersion > version then
                    if false == scene:singleGameUpdate(GlobalUserItem.tabAccountInfo.dwLockKindID, function()
                            print("MatchRoom:onEnterPlazaFinish update over")
                            this._scene:updateEnterGameInfo(matchgame)
                            this:onLoginRoom(GlobalUserItem.tabAccountInfo.dwLockServerID, true)
                        end) then
                        if nil ~= matchgame then
                            local gamename = matchgame._GameName or ""
                            local msg = string.format("请更新 %s 游戏资源!", gamename)
                            showToast(scene, msg, 2)
                        else
                            showToast(scene, "请更新游戏资源!", 2)
                        end
                        return true, false, self:onLoginRoom(GlobalUserItem.tabAccountInfo.dwLockServerID, true)
                    end
                    --启动游戏
                    print("MatchRoom:onEnterPlazaFinish ==> update lock pri game")
                    return true, true, true
                end
            end
        end

        --启动游戏
        print("MatchRoom:onEnterPlazaFinish ==> lock pri game")
        return true, false, self:onLoginRoom(GlobalUserItem.tabAccountInfo.dwLockServerID, true)
    end
    print("MatchRoom:onEnterPlazaFinish ==> not lock game")
    return false, false, false]]
end

-- 登陆后进入房间列表
function MatchRoom:onLoginEnterRoomList()
    if MatchRoom and false == MatchRoom:getInstance():isCurrentGameOpenMatch(GlobalUserItem.nCurGameKind) then
        print("MatchRoom:onLoginEnterRoomList: not open matchgame")
        return false
    end    
    if GlobalUserItem.dwLockServerID ~= 0 and GlobalUserItem.dwLockKindID == GlobalUserItem.nCurGameKind then
        print("MatchRoom:onLoginEnterRoomList: onLoginRoom")
        --启动游戏
        return self:onLoginRoom(GlobalUserItem.dwLockServerID)
    end
    return true
end

function MatchRoom:onLoginMatchRoomFinish() 
    self:dismissPopWait()
    local meUser = self:getMeUserItem()
    if meUser.cbUserStatus == yl.US_PLAYING or meUser.cbUserStatus == yl.US_READY or meUser.cbUserStatus == yl.US_SIT then
        print("锁在比赛房间中")
        GlobalUserItem.dwLockServerID = 0
        GlobalUserItem.dwLockKindID = 0
        self:showPopWait()
            -- 切换游戏场景
        -- LogAsset:getInstance():logData("MatchRoom:onLoginMatchRoomFinish() onEnterTable "..os.date()) --debug
        self._scene:onEnterTable()
            -- 发送配置
        self._scene._gameFrame:SendGameOption()
        return
    end
   
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onLoginMatchRoomFinish then
         self._viewFrame:onLoginMatchRoomFinish()
    end
end

-- 用户状态变更( 进入、离开、准备 等)
function MatchRoom:onEventUserState(viewid, useritem, bLeave)
    bLeave = bLeave or false
    -- if nil ~= self._matchView and nil ~= self._matchView.onRefreshInviteBtn then
    --     self._matchView:onRefreshInviteBtn()
    -- end
end

function MatchRoom:popMessage(message, notShow)
    notShow = notShow or false
    if type(message) == "string" and "" ~= message then
        if notShow or nil == self._matchView then
            print(message)
        else--if not tolua.isnull(self._matchView) then
            local runScene = cc.Director:getInstance():getRunningScene()
            if nil ~= runScene then
                showToast(runScene, message, 2)
            end 
            --showToast(self._matchView, message, 2)
        end
    end
end

function MatchRoom:onMatchLoginServerMessage(result, message, dataBuffer, notShow)
   
end

function MatchRoom:onMatchGameServerMessage(result, message, dataBuffer, notShow)
    self:popMessage(message, notShow)

    if cmd_match_game.SUB_GR_MATCH_INFO == result then    
        -- 更新比赛信息    
        if nil ~= self._matchView and nil ~= self._matchView.onUpdateMatchInfo then
            print("更新比赛信息")
            self._matchView:onUpdateMatchInfo(dataBuffer)
        end
    elseif cmd_match_game.SUB_GR_MATCH_RULE == result then
       
    elseif cmd_match_game.SUB_GR_MATCH_WAIT_TIP == result then
        if nil ~= self._matchView and nil ~= self._matchView.onSubMatchTip then
            print("比赛提示")
            self._matchView:onSubMatchTip(dataBuffer)
        end
    elseif cmd_match_game.SUB_GR_MATCH_RESULT == result then
       
        if nil ~= self._matchView and nil ~= self._matchView.onSubMatchResult then
            print("比赛结果")
            self._matchView:onSubMatchResult(dataBuffer)
        end
    elseif cmd_match_game.SUB_GR_MATCH_FEE == result then
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubMatchFee then
            self._viewFrame:onSubMatchFee(dataBuffer)
        end
    elseif cmd_match_game.SUB_GR_MATCH_NUM == result then
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubMatchNum then
            self._viewFrame:onSubMatchNum(dataBuffer)
        end
    elseif cmd_match_game.SUB_GR_MATCH_USTATUS == result then
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubMatchUStatus then
            self._viewFrame:onSubMatchUStatus(dataBuffer)
        end
    elseif cmd_match_game.SUB_GR_MATCH_STATUS == result then
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubMatchStatus then
            self._viewFrame:onSubMatchStatus(dataBuffer)
        end
    elseif cmd_match_game.SUB_GR_MATCH_PROMOTION == result then
        
        if nil ~= self._matchView and nil ~= self._matchView.onSubMatchPromotion then
            self._matchView:onSubMatchPromotion(dataBuffer )
        end
        
    end
    self:dismissPopWait()
end

--比赛人数刷新
function MatchRoom:onSubMatchSignUpCount(dataBuffer)
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubMatchSignUpCount then
        self._viewFrame:onSubMatchSignUpCount(dataBuffer)
    end
end

function MatchRoom:onSubMyMatchHistoryResult(tabRecord)
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onSubMyMatchHistoryResult then
        self._viewFrame:onSubMyMatchHistoryResult(tabRecord)
    end
end

-- 网络管理
function MatchRoom:getNetFrame()
    return self._matchFrame
end

-- 设置网络消息处理层
function MatchRoom:setViewFrame(viewFrame)
    self._viewFrame = viewFrame
end

-- 获取自己数据
function MatchRoom:getMeUserItem()
    return self._matchFrame._gameFrame:GetMeUserItem()
end

-- 获取游戏玩家数(椅子数)
function MatchRoom:getChairCount()
    return self._matchFrame._gameFrame:GetChairCount()
end

-- 获取大厅场景
function MatchRoom:getPlazaScene()
    return self._scene
end

-- 界面切换
function MatchRoom:getTagLayer(tag)
   if LAYTAG.MATCH_ROOMLIST == tag then
   
        local list = appdf.req(MODULE.PLAZAMODULE .. "views.MatchRoomListLayer"):create( self._scene )
        -- 绑定回调
        self:setViewFrame(list)
        return list
    end
end

-- 主界面是否顶、底栏
function MatchRoom.haveBottomTop( tag )
    return ( tag == LAYTAG.MATCH_TYPELIST or tag == LAYTAG.MATCH_ROOMLIST)
end

-- 是否快速开始
function MatchRoom.enableQuickStart( tag )
    return not ( tag == LAYTAG.MATCH_ROOMLIST )
end

-- 是否房间界面
function MatchRoom.isRoomListLayer( tag )
    return (tag == LAYTAG.MATCH_ROOMLIST or tag == LAYTAG.MATCH_TYPELIST )
end

-- 是否返回按钮
function MatchRoom.enableBackBtn( tag )
    return ( tag == LAYTAG.MATCH_ROOMLIST or tag == LAYTAG.MATCH_TYPELIST)
end

-- 进入游戏房间
function MatchRoom:enterRoom( scene )
    local entergame = scene:getEnterGameInfo()
    local bMatchMode = MatchRoom:getInstance():isCurrentGameOpenMatch(GlobalUserItem.nCurGameKind)
    if nil ~= entergame and true == bMatchMode and "" == self._gameSearchPath then
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        self._gameSearchPath = device.writablePath.."client/src/plaza/models/match/game/" .. modulestr .. "/res"
        cc.FileUtils:getInstance():addSearchPath(self._gameSearchPath)
    end    
end

function MatchRoom:exitRoom( )
    --重置搜索路径
    local oldPaths = cc.FileUtils:getInstance():getSearchPaths()
    local newPaths = {}
    for k,v in pairs(oldPaths) do
        if tostring(v) ~= tostring(self._gameSearchPath) then
            table.insert(newPaths, v)
        end
    end
    cc.FileUtils:getInstance():setSearchPaths(newPaths)
    self._gameSearchPath = ""
end

-- 进入游戏界面
function MatchRoom:enterGame( gameLayer, scene )
    self:enterRoom(scene)
    local entergame = scene:getEnterGameInfo()
    if nil ~= entergame then
        self:setViewFrame(gameLayer)
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        local gameFile = ""
        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            gameFile = "client/src/plaza/models/match/game/" .. modulestr .. "src/MatchGameLayer.lua"
        else
            gameFile = "client/src/plaza/models/match/game/" .. modulestr .. "src/MatchGameLayer.lua"
        end
        if cc.FileUtils:getInstance():isFileExist(gameFile) then
            local lay = appdf.req(gameFile):create( gameLayer )
            if nil ~= lay then
                gameLayer._gameView:addChild(lay, gameLayer:matchGameLayerZorder())
                gameLayer._gameView._matchView = lay
                self._matchView = lay
            end
        end
    end
end

-- 退出游戏界面
function MatchRoom:exitGame()
    self._matchView = nil
    self._viewFrame = nil
    self._matchFrame._gameFrame._viewFrame = nil
    print("3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333")
end

function MatchRoom:showPopWait()
    self._scene:showPopWait()
end

function MatchRoom:dismissPopWait()
    self._scene:dismissPopWait()
end

function MatchRoom:queryQuitGame(cbGameStatus)
    local tip = "你正在比赛中，强行退出将会被系统托管，输赢自负，确定要强退吗？"
    QueryDialog:create(tip, function(ok)
            if ok == true then    
                self._scene:onKeyBack()
            end
        end)
    :setCanTouchOutside(false)
    :addTo(self._viewFrame)
 end 

--解析比赛规则列表
function MatchRoom:onSubMatchRuleList( pData)
    self:getNetFrame():onSubMatchRuleList(pData)
end

--比赛规则分组
function MatchRoom:onSubMatchRuleGroup()
   --dump(self.m_tabAllMatchRule, "match rule", 6)

   for k,v in pairs(self.m_tabAllMatchRule) do
        table.insert(self.m_tabMatchRoomRuleList, v)
    end
    self.m_tabAllMatchRule = {}
end

-- 分享图片给好友
function MatchRoom:imageShareToFriend( frienddata, imagepath, sharemsg )
    if nil == frienddata or nil == self._scene.imageShareToFriend then
        return
    end
    local id = frienddata.dwUserID
    if nil == id then
        return
    end
    self._scene:imageShareToFriend(id, imagepath, sharemsg)
end

