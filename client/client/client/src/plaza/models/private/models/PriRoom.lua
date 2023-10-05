--
-- Author: zhong
-- Date: 2016-12-17 09:48:44
--
-- 私人房数据管理 全局模式

PriRoom = PriRoom or class("PriRoom")
local private_define = appdf.req(appdf.CLIENT_SRC .. "plaza.models.private.header.Define_Private")
local cmd_private = appdf.req(appdf.CLIENT_SRC .. "plaza.models.private.header.CMD_Private")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

-- ui
local PrivateCreateResLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateCreateResLayer")        -- 创建结果
local PrivatePasswdInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivatePasswdInputLayer")    -- 密码输入
-- ui

-- 私人房模块
local MODULE = private_define.tabModule
PriRoom.MODULE = MODULE
-- 私人房界面tag
local LAYTAG = private_define.tabLayTag
PriRoom.LAYTAG = LAYTAG
-- 游戏服务器登陆操作定义
local L_ACTION = private_define.tabLoginAction
PriRoom.L_ACTION = L_ACTION
-- 房间状态
local ROOM_STATE = private_define.tabRoomState
PriRoom.ROOM_STATE = ROOM_STATE

-- 登陆服务器CMD
local cmd_pri_login = cmd_private.login
-- 游戏服务器CMD
local cmd_pri_game = cmd_private.game

local PriFrame = appdf.req(MODULE.PLAZAMODULE .."models.PriFrame")

-- roomID 输入界面
local NAME_ROOMID_INPUT = "___private_roomid_input_layername___"
-- 房间创建界面
local NAME_CREATE_LAYER = "__private_create_roome_layername__"
PriRoom.NAME_CREATE_LAYER = NAME_CREATE_LAYER

local targetPlatform = cc.Application:getInstance():getTargetPlatform()
function PriRoom:ctor()
    -- 私人房游戏资源搜索路径
    self._gameSearchPath = ""
    
    local this = self
    --网络回调
    local privateCallBack = function(command, message, dataBuffer, notShow)
        if type(command) == "table" then
            if command.m == cmd_pri_login.MDM_MB_PERSONAL_SERVICE then
                return this:onPrivateLoginServerMessage(command.s, message, dataBuffer, notShow)
            elseif command.m == cmd_pri_game.MDM_GR_PERSONAL_TABLE then
                return this:onPrivateGameServerMessage(command.s, message, dataBuffer, notShow)
            end
        else
            
            if -1 == command then
                this:dismissPopWait()
            else
                this:popMessage(message, notShow)
            end
        end
        
    end
    self._priFrame = PriFrame:create(self, privateCallBack)

    self:reSet()
end

-- 实现单例
PriRoom._instance = nil
function PriRoom:getInstance( )
    if nil == PriRoom._instance then
        print("new instance")
        PriRoom._instance = PriRoom:create()
    end
    return PriRoom._instance
end

function PriRoom:reSet()
    -- 私人房模式游戏列表
    self.m_tabPriModeGame = {}
    -- 私人房列表
    self.m_tabPriRoomList = {}
    -- 创建记录
    self.m_tabCreateRecord = {}
    -- 参与记录
    self.m_tabJoinRecord = {}
    -- 私人房数据  CMD_GR_PersonalTableTip
    self.m_tabPriData = {}
    -- 私人房属性 tagPersonalRoomOption
    self.m_tabRoomOption = {}
    -- 私人房金币房卡属性 tagPersonalRoomOption
    self.m_tabGoldRoomOption = nil
    -- 私人房费用配置 tagPersonalTableParameter
    self.m_tabFeeConfigList = {}
    -- 私人房金币房配置 tagPersonalTableParameter
    self.m_tabGoldRoomConfigList = {}
    -- 是否自己房主
    self.m_bIsMyRoomOwner = false
    -- 私人房桌子号( 进入/查到到的 )
    self.m_dwTableID = yl.INVALID_TABLE
    -- 选择的私人房配置信息
    self.m_tabSelectRoomConfig = {}

    -- 大厅场景
    self._scene = nil
    -- 网络消息处理层
    self._viewFrame = nil
    -- 私人房信息层
    self._priView = nil

    -- 游戏服务器登陆动作
    self.m_nLoginAction = L_ACTION.ACT_NULL
    self.cbIsJoinGame = 0
    -- 是否已经取消桌子/退出
    self.m_bCancelTable = false
    -- 是否收到结算消息
    self.m_bRoomEnd = false
    -- 参与游戏记录(用于暂离游戏时能返回游戏)
    self.m_tabJoinGameRecord = {}

    -- 分享回调
    self.m_pGetPasswdCallBack = nil
    -- 房间密码(非免密房间)
    self.m_szPasswd = ""

    self.m_bLookOn = false
    -- 积分是否限时免费
    self.m_bLimitTimeFree = false

     -- 金币是否限时免费
    self.m_bGoldLimitTimeFree = false
       
    -- 底分配置
    self.m_tabCellScoreList = {}

    self._queryDialog = nil

    -- 是否代开
    self.m_bCreateForOther = false

    -- 0是积分，1是金币
    self.m_bPlayModel = 0

    -- 金币私有房创建记录
    self.m_tabGoldRoomList = {}
    --列表完成
    self.m_bRoomListFinsh = true

     -- 金币私人房模式游戏列表
    self.m_tabGoldPriModeGame = {}
    --积分是有房列表
    self.m_tabScorePriModeGame = {}

    --当前进入的俱乐部ID ,非俱乐部进入为0
    self.m_CurrentClubID = 0

    -- 额外配置
    self.m_tabRule = {}

    --是否是列表结束
    self.bIsRoomListEnd = true
end

-- 当前游戏是否开启私人房模式
function PriRoom:isCurrentGameOpenPri( nKindID )
    return (self.m_tabPriModeGame[nKindID] or false)
end

-- 当前游戏是否开启金币私人房模式
function PriRoom:isCurrentGameOpenPriGold( nKindID )
    return (self.m_tabGoldPriModeGame[nKindID] or false)
end

-- 当前游戏是否开启积分私人房模式
function PriRoom:isCurrentGameOpenPriScore( nKindID )
    return (self.m_tabScorePriModeGame[nKindID] or false)
end

-- 获取私人房
function PriRoom:getPriRoomByServerID( dwServerID )
    local currentGameRoomList = self.m_tabPriRoomList[GlobalUserItem.nCurGameKind]
    if nil == currentGameRoomList then
        return nil
    end
    --dump(currentGameRoomList, "currentGameRoomList", 6)
    for k,v in pairs(currentGameRoomList) do
        if v.wServerID == dwServerID and v.wServerType == yl.GAME_GENRE_PERSONAL then
            return v
        end
    end
    return nil
end

-- 登陆私人房
function PriRoom:onLoginRoom( dwServerID, bLockEnter )
    local pServer = self:getPriRoomByServerID(dwServerID)
    if nil == pServer then
        print("PriRoom server null")
        local curTag = nil
        if nil ~= self._scene and nil ~= self._scene._sceneLayer then
            curTag = self._scene._sceneLayer:getCurrentTag()
        end
        if curTag == yl.SCENE_PLAZA and GlobalUserItem.bPrivateRoom then
            showToast(self._scene, "房间未找到, 请重试!", 2)
        end
        return false
    end

    -- 登陆房间
    if nil ~= self._priFrame and nil ~= self._priFrame._gameFrame then
        bLockEnter = bLockEnter or false
        -- 锁表进入
        if bLockEnter then
            -- 动作定义
            PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_SEARCHROOM
        end
        -- 记录进入游戏
        self._scene:updateCurrentGame()

        
        self:showPopWait()
        GlobalUserItem.bPrivateRoom = pServer.wServerType == yl.GAME_GENRE_PERSONAL
        self._priFrame._gameFrame:setEnterAntiCheatRoom(false)
        GlobalUserItem.nCurRoomIndex = pServer._nRoomIndex
        self._scene:onStartGame()
        self.m_bCancelTable = false
        return true
    end
    return false
end

-- 
function PriRoom:onEnterPlaza( scene, gameFrame )
    self._scene = scene
    self._priFrame._gameFrame = gameFrame
    self:setViewFrame(self._scene)
end

function PriRoom:onExitPlaza()
    if nil ~= self._priFrame._gameFrame then
        self._priFrame._gameFrame = nil
    end
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
    -- 清理暂离记录
    self.m_tabJoinGameRecord = {}
    self:exitRoom()
    self:reSet()
end

function PriRoom:onEnterPlazaFinish()
    local this = self
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
                local prigame = scene:getGameInfo(GlobalUserItem.tabAccountInfo.dwLockKindID)
                local version = tonumber(app:getVersionMgr():getResVersion(GlobalUserItem.tabAccountInfo.dwLockKindID))
                if not version or prigame._ServerResVersion > version then
                    if false == scene:singleGameUpdate(GlobalUserItem.tabAccountInfo.dwLockKindID, function()
                            print("PriFrame:onEnterPlazaFinish update over")
                            this._scene:updateEnterGameInfo(prigame)
                            this:onLoginRoom(GlobalUserItem.tabAccountInfo.dwLockServerID, true)
                        end) then
                        if nil ~= prigame then
                            local gamename = prigame._GameName or ""
                            local msg = string.format("请更新 %s 游戏资源!", gamename)
                            showToast(scene, msg, 2)
                        else
                            showToast(scene, "请更新游戏资源!", 2)
                        end
                        return true, false, self:onLoginRoom(GlobalUserItem.tabAccountInfo.dwLockServerID, true)
                    end
                    --启动游戏
                    print("PriRoom:onEnterPlazaFinish ==> update lock pri game")
                    return true, true, true
                end
            end
        end

        --启动游戏
        print("PriRoom:onEnterPlazaFinish ==> lock pri game")
        return true, false, self:onLoginRoom(GlobalUserItem.tabAccountInfo.dwLockServerID, true)
    end
    print("PriRoom:onEnterPlazaFinish ==> not lock game")
    return false, false, false
end

-- 登陆后进入房间列表
function PriRoom:onLoginEnterRoomList()
    -- 判断是否开启私人房
    if false == PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
        print("PriRoom:onLoginEnterRoomList: not open prigame")
        return false
    end

    if GlobalUserItem.tabAccountInfo.dwLockServerID ~= 0 and GlobalUserItem.tabAccountInfo.dwLockKindID == GlobalUserItem.nCurGameKind then
        print("PriRoom:onLoginEnterRoomList: onLoginRoom")
        --启动游戏
        return self:onLoginRoom(GlobalUserItem.tabAccountInfo.dwLockServerID, true)
    end    
end

function PriRoom:onLoginPriRoomFinish()
    print("PriRoom:onLoginPriRoomFinish")



    -- 数据校验
    if self.m_nLoginAction == L_ACTION.ACT_CREATEROOM then

        if self._priFrame._clubConfigId and self._priFrame._clubConfigId ~= 0 then 
            self._priFrame:sendClubCreateRoom()
            return 
        end

        -- 动作为创建时, 回调Frame必须为创建层
        assert(nil ~= self._viewFrame and self._viewFrame.__cname == 'PriRoomCreateLayer')
    end
    
    local bHandled = false
    if nil ~= self._viewFrame and nil ~= self._viewFrame.onLoginPriRoomFinish then
        
            bHandled = self._viewFrame:onLoginPriRoomFinish()
        
    end



    -- 清理锁表
    GlobalUserItem.tabAccountInfo.dwLockServerID = 0
    GlobalUserItem.tabAccountInfo.dwLockKindID = 0

    if not bHandled then
        local meUser = self:getMeUserItem()
        if nil == meUser then
            return
        end
        print("自己状态 ", meUser.cbUserStatus)
        print("动作 ", self.m_nLoginAction)
        if (meUser.cbUserStatus == yl.US_FREE or meUser.cbUserStatus == yl.US_NULL) then
            -- 搜索登陆
            if self.m_nLoginAction == L_ACTION.ACT_SEARCHROOM 
                or self.m_nLoginAction == PriRoom.L_ACTION.ACT_ENTERTABLE then
                print("PriRoom:onLoginPriRoomFinish [sendEnterPrivateGame]")
                self:showPopWait()
                -- 进入游戏
                self._priFrame:sendEnterPrivateGame()
            -- 解散登陆
            elseif self.m_nLoginAction == PriRoom.L_ACTION.ACT_DISSUMEROOM then
                print("PriRoom:onLoginPriRoomFinish [sendDissumeGame]")
                self:showPopWait()
                -- 发送解散    
                self._priFrame:sendDissumeGame(self.m_dwTableID)
            -- 查询信息
            elseif self.m_nLoginAction == L_ACTION.ACT_QUERYJOININFO then
                print("PriRoom:onLoginPriRoomFinish [sendQueryJoinInfo]")
                self:showPopWait()
                self._priFrame:sendQueryJoinInfo(self.m_dwTableID)
            else
                print("PriRoom:onLoginPriRoomFinish unknown action ==> ", self.m_nLoginAction)
                self._priFrame._gameFrame:onCloseSocket()
                self._priFrame:onCloseSocket()
                GlobalUserItem.nCurRoomIndex = -1
                -- 退出游戏房间
                self._scene:onKeyBack()
            end
        elseif meUser.cbUserStatus == yl.US_PLAYING or meUser.cbUserStatus == yl.US_READY or meUser.cbUserStatus == yl.US_SIT then
            -- 搜索登陆
            if self.m_nLoginAction == L_ACTION.ACT_SEARCHROOM
                or self.m_nLoginAction == PriRoom.L_ACTION.ACT_ENTERTABLE
                or self.m_nLoginAction == L_ACTION.ACT_QUERYJOININFO then
                -- sit/ready 状态 ACT_QUERYJOININFO 直接处理为进入房间
                print("PriRoom:onLoginPriRoomFinish [SendGameOption]")
                self:showPopWait()
                -- 切换游戏场景
                self._scene:onEnterTable()
                -- 发送配置
                self._priFrame._gameFrame:SendGameOption()
            -- 解散登陆
            elseif self.m_nLoginAction == L_ACTION.ACT_DISSUMEROOM then
                print("PriRoom:onLoginPriRoomFinish [sendDissumeGame]")
                self:showPopWait()
                -- 发送解散    
                self._priFrame:sendDissumeGame(self.m_dwTableID)
            -- 密码输入
            elseif self.m_nLoginAction == L_ACTION.ACT_INPUTPASSWD then
                print("PriRoom:onLoginPriRoomFinish [PasswdInput SendGameOption]")
                self:showPopWait()
                -- 动作记录
                self.m_nLoginAction = PriRoom.L_ACTION.ACT_ENTERTABLE
                -- 切换游戏场景
                self._scene:onEnterTable()
                -- 发送配置
                self._priFrame._gameFrame:SendGameOption()
            end
        end
    end    
end

-- 用户状态变更( 进入、离开、准备 等)
function PriRoom:onEventUserState(viewid, useritem, bLeave)
    bLeave = bLeave or false
    if self.m_bCancelTable then
        return
    end
    if nil ~= self._priView and nil ~= self._priView.onRefreshInviteBtn then
        self._priView:onRefreshInviteBtn()
    end
end

function PriRoom:popMessage(message, notShow)
    notShow = notShow or false
    if type(message) == "string" and "" ~= message then
        if notShow or nil == self._scene then
            print(message)
        elseif nil ~= self._scene then
            showToast(self._scene, message, 2)
        end
    end
end

function PriRoom:onPrivateLoginServerMessage(result, message, dataBuffer, notShow)
    self:popMessage(message, notShow)

    if cmd_pri_login.SUB_MB_QUERY_PERSONAL_ROOM_LIST_RESULT == result 
        or cmd_pri_login.SUB_GR_USER_QUERY_ROOM_SCORE_RESULT == result then
        -- 列表记录
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onReloadRecordList then
            self._viewFrame:onReloadRecordList(true)
        end
    elseif cmd_pri_login.SUB_MB_PERSONAL_FEE_PARAMETER == result then
        print("PriRoom fee list call back")
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshFeeList then
            self._viewFrame:onRefreshFeeList()
        end

    elseif cmd_pri_login.SUB_MB_PERSONAL_PARAMETER == result then

        if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshFeeList then
            self._viewFrame:onRefreshFeeList()
        end

    elseif cmd_pri_login.SUB_MB_QUERY_ROOM_PASSWORD_RESULT == result then
        if type(self.m_pGetPasswdCallBack) == "function" then
            self.m_pGetPasswdCallBack(message)
        end
        self:dismissPopWait()
    elseif cmd_pri_login.SUB_MB_PERSONAL_RULE_RESULT == result then 

        if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshRule  then
            self._viewFrame:onRefreshRule()
        end

    elseif cmd_pri_login.SUB_MB_SEARCH_RESULT == result then
        -- 房间搜索失败
        self:popMessage(message.popMsg, notShow)

        -- 游戏界面退出
        local curTag = nil
        if nil ~= self._scene and nil ~= self._scene._sceneLayer then
            curTag = self._scene._sceneLayer:getCurrentTag()
        end
        if curTag == yl.SCENE_GAME then
            self._scene:onKeyBack()
        end
    elseif cmd_pri_login.SUB_MB_QUERY_GOLD_ROOM_LIST_RESULT == result then
        -- 列表记录
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onReloadGoldRoomList then
            self._viewFrame:onReloadGoldRoomList()
        end
    end
end

function PriRoom:onPrivateGameServerMessage(result, message, dataBuffer, notShow)
    local needClose = false
    self:popMessage(message, notShow)
    self:dismissPopWait()

    if cmd_pri_game.SUB_GR_CREATE_SUCCESS == result then    
        -- 创建成功    
        if self._priFrame._clubConfigId and self._priFrame._clubConfigId ~= 0 then 

            PriRoom:getInstance():getNetFrame():onSearchRoom(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID)
            needClose = false
        elseif nil ~= self._viewFrame and nil ~= self._viewFrame.onRoomCreateSuccess then
            needClose = self._viewFrame:onRoomCreateSuccess()
        end

        -- 清理暂离记录
        PriRoom:getInstance().m_tabJoinGameRecord[GlobalUserItem.nCurGameKind] = {}
        -- 客户端通知
        local event = cc.EventCustom:new(appdf.CLIENT_NOTIFY)
        event.msg = msg
        event.what = yl.CLIENT_MSG_PRIVATEROOM_REFRESH
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
    elseif cmd_pri_game.SUB_GR_CANCEL_TABLE == result then
        print("PriRoom  SUB_GR_CANCEL_TABLE")
        GlobalUserItem.bWaitQuit = true
        self.m_bCancelTable = true
        -- 清理暂离记录
        self.m_tabJoinGameRecord[GlobalUserItem.nCurGameKind] = {}
        -- 解散桌子
        local curTag = nil
        if nil ~= self._scene and nil ~= self._scene._sceneLayer then
            curTag = self._scene._sceneLayer:getCurrentTag()
        end
        logFunc("约战房间解散收到消息 ==> " .. message.szDescribeString, true)
        if curTag == yl.SCENE_GAME and not self.m_bRoomEnd and nil==self._viewFrame:getChildByName("requestCancelDialog") then
            local query = QueryDialog:create(message.szDescribeString, function(ok)
                    GlobalUserItem.bWaitQuit = false
                    if nil ~= self._viewFrame and nil ~= self._viewFrame.onExitRoom then
                        self._viewFrame:onExitRoom()
                    end
                end)
                :setCanTouchOutside(false)
                :setName("requestCancelDialog")
                :addTo(self._viewFrame)
            local zorder = 0
            if nil ~= self._viewFrame.priGameLayerZorder then
                zorder = self._viewFrame:priGameLayerZorder() - 1
            end
            query:setLocalZOrder(zorder)
        else
            showToast(self._viewFrame, message.szDescribeString, 2)
        end
        self.m_bRoomEnd = false   
    elseif cmd_pri_game.SUB_GR_CANCEL_REQUEST == result then
        -- 请求解散
        -- message = game.CMD_GR_CancelRequest_Result
        local useritem = self._priFrame._gameFrame._UserList[message.dwUserID]
        if nil == useritem then
            return needClose
        end
        local curTag = nil
        if nil ~= self._scene and nil ~= self._scene._sceneLayer then
            curTag = self._scene._sceneLayer:getCurrentTag()
        end
        if nil ~= self.m_queryQuit and nil ~= self.m_queryQuit:getParent() then
            self.m_queryQuit:removeFromParent()
        end

        if nil ~= self._queryDialog and tolua.cast(self._queryDialog,"cc.Node")  then
            self._queryDialog:removeFromParent()
        end        
        
        if curTag == yl.SCENE_GAME and self._priView._dismissLayer then
            self._priView._dismissLayer:requestDismissRoom(useritem, message.wCancelTime)
        else

        end        
    elseif cmd_pri_game.SUB_GR_REQUEST_REPLY == result then
        -- 请求答复
        -- message = game.CMD_GR_RequestReply
        local useritem = self._priFrame._gameFrame._UserList[message.dwUserID]
        if nil == useritem then
            return needClose
        end
        local bHandled = false
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onCancellApply then
            bHandled = self._viewFrame:onCancellApply(useritem, message)
        end
        if not bHandled then
            local tips = "同意解散"
            if 0 == message.cbAgree then
                tips = "不同意解散"
            end
            local curTag = nil
            if nil ~= self._scene and nil ~= self._scene._sceneLayer then
                curTag = self._scene._sceneLayer:getCurrentTag()
            end
            if curTag == yl.SCENE_GAME and nil ~= self._priView._dismissLayer then
                --showToast(self._viewFrame, useritem.szNickName .. tips, 2)
                self._priView._dismissLayer:requestReply(useritem, message.cbAgree ~= 0)
            end            
        end
    elseif cmd_pri_game.SUB_GR_REQUEST_RESULT == result then
        -- 请求结果
        -- message = game.CMD_GR_RequestResult
        local curTag = nil
        if nil ~= self._scene and nil ~= self._scene._sceneLayer then
            curTag = self._scene._sceneLayer:getCurrentTag()
        end
        if curTag == yl.SCENE_GAME then
            self._priView._dismissLayer:hideLayer()
            if 0 == message.cbResult then
                showToast(self._viewFrame, "解散房间请求未通过", 2)          
                return needClose
            end
        end

        if self._viewFrame and self._viewFrame.dismissResult then 
            self._viewFrame:dismissResult()
        end
        
        self.m_bCancelTable = true
        local bHandled = false
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onCancelResult then
            bHandled = self._viewFrame:onCancelResult(message)
        end
        if not bHandled then
            
        end
    elseif cmd_pri_game.SUB_GR_WAIT_OVER_TIME == result then
        -- 超时提示
        -- message = game.CMD_GR_WaitOverTime
        local useritem = self._priFrame._gameFrame._UserList[message.dwUserID]
        if nil == useritem then
            return needClose
        end
        local curTag = nil
        if nil ~= self._scene and nil ~= self._scene._sceneLayer then
            curTag = self._scene._sceneLayer:getCurrentTag()
        end
        if curTag == yl.SCENE_GAME then
            local query = QueryDialog:create(useritem.szNickName .. "断线等待超时, 是否继续等待?", function(ok)
                    if ok then
                        self:getNetFrame():sendRequestReply(0)
                    else
                        self:getNetFrame():sendRequestReply(1)
                    end
                    --self:showPopWait()
                end)
                :setCanTouchOutside(false)
                :addTo(self._viewFrame)
        end
    elseif cmd_pri_game.SUB_GR_PERSONAL_TABLE_TIP == result then
        -- 游戏信息
        if nil ~= self._priView and nil ~= self._priView.onRefreshInfo then
            self._priView:onRefreshInfo()
        end
    elseif cmd_pri_game.SUB_GR_PERSONAL_TABLE_END == result then

        local curTag = nil
        if nil ~= self._scene and nil ~= self._scene._sceneLayer then
            curTag = self._scene._sceneLayer:getCurrentTag()
        end

        if curTag == yl.SCENE_GAME and self._priView._dismissLayer then
            self._priView._dismissLayer:hideLayer()
        end

        
        GlobalUserItem.bWaitQuit = true
        -- 屏蔽重连功能
        GlobalUserItem.bAutoConnect = false
        -- 清理暂离记录
        self.m_tabJoinGameRecord[GlobalUserItem.nCurGameKind] = {}
        -- 结束消息
        if nil ~= self._priView and nil ~= self._priView.onPriGameEnd then
            self._priView:onPriGameEnd(message, dataBuffer)
        end
        self.m_bRoomEnd = true
    elseif cmd_pri_game.SUB_GR_CANCEL_TABLE_RESULT == result then        
        -- 解散结果
        -- message = game.CMD_GR_DissumeTable
        if 1 == message.cbIsDissumSuccess then
            -- 发送起立
            self._priFrame._gameFrame:StandUp(1)
            showToast(self._scene, "解散成功", 2)
        end
        -- 更新创建记录
        for k,v in pairs(self.m_tabCreateRecord) do
            if message.dwPersonalRoomID == v.dwPersonalRoomID then
                v.cbIsDisssumRoom = 2 --(房间状态 0 表示未开始 1 表示 游戏中 2 已解散)
                v.sysDissumeTime = message.sysDissumeTime
                local tt = v.sysDissumeTime
                v.sortTimeStmp = os.time({day=tt.wDay, month=tt.wMonth, year=tt.wYear, hour=tt.wHour, min=tt.wMinute, sec=tt.wSecond})
                break
            end
        end
        -- 排序
        table.sort( PriRoom:getInstance().m_tabCreateRecord, function(a, b)
            if a.cbIsDisssumRoom ~= b.cbIsDisssumRoom then
                return a.cbIsDisssumRoom < b.cbIsDisssumRoom
            elseif a.cbIsDisssumRoom == 0 and a.cbIsDisssumRoom == b.cbIsDisssumRoom then
                return a.createTimeStmp > b.createTimeStmp
            else
                return a.sortTimeStmp > b.sortTimeStmp
            end        
        end )
        --刷新列表
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onReloadRecordList then
            self._viewFrame:onReloadRecordList()
        end
    elseif cmd_pri_game.SUB_GF_PERSONAL_MESSAGE == result then
        if nil == self._viewFrame then
                local curTag = nil
            if nil ~= self._scene and nil ~= self._scene._sceneLayer then
                curTag = self._scene._sceneLayer:getCurrentTag()
            end
            if curTag == yl.SCENE_GAME then
                self._scene:onKeyBack()
            end
            return
        end
        logFunc("约战服务器提示消息 ==> " .. message.szMessage, true)
        local query = QueryDialog:create(message.szMessage, function(ok)
                    if nil ~= self._viewFrame and nil ~= self._viewFrame.onExitRoom then
                        self._viewFrame:onExitRoom()
                    end
                end, nil, 1)
                :setCanTouchOutside(false)
                :addTo(self._viewFrame)
    elseif cmd_pri_game.SUB_GR_PLAYING_USER_INFO == result then

         if nil ~= self._viewFrame and nil ~= self._viewFrame.onPlayingUserInfo then
            self._viewFrame:onPlayingUserInfo(message)
        end       

    elseif cmd_pri_game.SUB_GR_CURRECE_ROOMCARD_AND_BEAN == result then
        -- 解散后游戏信息
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshInfo then
            self._viewFrame:onRefreshInfo()
        end
    elseif cmd_pri_game.SUB_GR_DEAL_CANCEL_INFO == result then
        -- 解散处理
        local curTag = nil
        if nil ~= self._scene and nil ~= self._scene._sceneLayer then
            curTag = self._scene._sceneLayer:getCurrentTag()
        end
        if curTag == yl.SCENE_GAME then
            if message.cbIsDeal == 1 and self._priView and self._priView._dismissLayer then
                self._priView._dismissLayer:reStartDismissRoom(message.cbRequestUser, message.cbUserDealInfo[1], message.dwRemainTime)
            elseif message.cbIsDeal == 0 then
                if self._priView and self._priView._dismissLayer then
                    self._priView._dismissLayer:setVisible(false)
                end
            end
        end 
    end
    return needClose
end

-- 网络管理
function PriRoom:getNetFrame()
    return self._priFrame
end

-- 设置网络消息处理层
function PriRoom:setViewFrame(viewFrame)
    self._viewFrame = viewFrame
end

-- 获取自己数据
function PriRoom:getMeUserItem()
    return self._priFrame._gameFrame:GetMeUserItem()
end

-- 获取游戏玩家数(椅子数)
function PriRoom:getChairCount()
    return self._priFrame._gameFrame:GetChairCount()
end

-- 获取大厅场景
function PriRoom:getPlazaScene()
    return self._scene
end

-- 获取创建界面
function PriRoom:getTagLayer( tag, scene, param )
    if LAYTAG.LAYER_CREATEPRIROOME == tag then
        self:exitRoom()
        self:enterRoom(scene)
        dump(param)  
        -- 创建私人房
        local entergame = scene:getEnterGameInfo()
        if nil ~= entergame then
            local modulestr = string.gsub(entergame._KindName, "%.", "/")
            local roomCreateFile = ""
            if cc.PLATFORM_OS_WINDOWS == targetPlatform then
                roomCreateFile = "game/" .. modulestr .. "src/privateroom/PriRoomCreateLayer.lua" .. yl.WIN_SRC_SUFFIX
                if param.clubType then
                    roomCreateFile = "game/" .. modulestr .. "src/privateroom/ClubRoomCreateLayer.lua" .. yl.WIN_SRC_SUFFIX
                end          
            else
                roomCreateFile = "game/" .. modulestr .. "src/privateroom/PriRoomCreateLayer.lua"
                if param.clubType then
                    roomCreateFile = "game/" .. modulestr .. "src/privateroom/ClubRoomCreateLayer.lua"
                end

            end


            if cc.FileUtils:getInstance():isFileExist(roomCreateFile) then
                local lay = appdf.req(roomCreateFile):create( scene,param,scene:getPopLevel() )
                -- 绑定回调
                self:setViewFrame(lay)
                return lay
            end
        end
    elseif LAYTAG.LAYER_CREATERESULT == tag then
        -- 创建结果
        if nil ~= self._scene then
            local roomResLayer = PrivateCreateResLayer:create(self._scene, {}, self._scene:getPopLevel())
            self._scene:addPopLayer(roomResLayer)
        end
    elseif LAYTAG.LAYER_AAPAY == tag then
        self:exitRoom()
        self:enterRoom(scene)
        -- AA
        local entergame = self._scene:getEnterGameInfo()
        if nil ~= entergame then
            local modulestr = string.gsub(entergame._KindName, "%.", "/")
            local gameFile = ""
            if cc.PLATFORM_OS_WINDOWS == targetPlatform then
                gameFile = "game/" .. modulestr .. "src/privateroom/PriRoomCreateLayer.lua" .. yl.WIN_SRC_SUFFIX
            else
                gameFile = "game/" .. modulestr .. "src/privateroom/PriRoomCreateLayer.lua"
            end
            if cc.FileUtils:getInstance():isFileExist(gameFile) then
                local PriRoomCreateLayer = appdf.req(gameFile)
                local paaL = PriRoomCreateLayer.PriRoomAAPayLayer:create( self, param, self._scene:getPopLevel())
                return paaL
            end
        end
    elseif LAYTAG.LAYER_PASSWDIDPUT == tag then
        -- 密码输入
        if nil ~= self._scene then
            local pl = PrivatePasswdInputLayer:create(self._scene, {}, self._scene:getPopLevel())
            return pl
        end
    end
    return nil
end

-- 主界面是否顶、底栏
function PriRoom.haveBottomTop( tag )
    return ( tag == LAYTAG.LAYER_CREATEPRIROOME or tag == LAYTAG.LAYER_MYROOMRECORD or tag == LAYTAG.LAYER_ROOMLIST )
end

-- 是否快速开始
function PriRoom.enableQuickStart( tag )
    return not ( tag == LAYTAG.LAYER_CREATEPRIROOME or tag == LAYTAG.LAYER_MYROOMRECORD or tag == LAYTAG.LAYER_ROOMLIST)
end

-- 是否房间界面
function PriRoom.isRoomListLayer( tag )
    return (tag == LAYTAG.LAYER_ROOMLIST or tag == yl.PLAZA_ROOMLIST_LAYER )
end

-- 是否返回按钮
function PriRoom.enableBackBtn( tag )
    return ( tag == LAYTAG.LAYER_CREATEPRIROOME or tag == LAYTAG.LAYER_MYROOMRECORD )
end

-- 进入游戏房间
function PriRoom:enterRoom( scene )
    local entergame = scene:getEnterGameInfo()
    local bPirMode = PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind)
    if nil ~= entergame and true == bPirMode and "" == self._gameSearchPath then
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        self._gameSearchPath = device.writablePath.."game/" .. modulestr .. "/res/privateroom/"
        cc.FileUtils:getInstance():addSearchPath(self._gameSearchPath)
    end    
end

function PriRoom:exitRoom( )    
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
    self:setViewFrame(nil)
end

-- 进入游戏界面
function PriRoom:enterGame( gameLayer, scene )
    self:exitRoom()
    self:enterRoom(scene)
    local entergame = scene:getEnterGameInfo()
    if nil ~= entergame then
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        local gameFile = ""
        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            gameFile = "game/" .. modulestr .. "src/privateroom/PriGameLayer.lua" .. yl.WIN_SRC_SUFFIX
        else
            gameFile = "game/" .. modulestr .. "src/privateroom/PriGameLayer.lua"
        end
        if cc.FileUtils:getInstance():isFileExist(gameFile) then
            local lay = appdf.req(gameFile):create( gameLayer )
            if nil ~= lay then
                gameLayer:addPrivateGameLayer(lay)
                gameLayer._gameView._priView = lay
                self._priView = lay
            end
            -- 绑定回调
            self:setViewFrame(gameLayer)
        end
    end
    self.m_bRoomEnd = false
end

-- 退出游戏界面
function PriRoom:exitGame()
    self._priView = nil
    GlobalUserItem.bPrivateRoom = false
    GlobalUserItem.bPrivateGoldRoom = false         --金币私人房
    if nil ~= self.m_queryQuit then
        self.m_queryQuit:removeFromParent()
        self.m_queryQuit = nil
    end
    self.m_tabPriData = {}
    -- 房间密码(非免密房间)
    self.m_szPasswd = ""
    -- 是否限时免费
    self.m_bLimitTimeFree = false

    self.m_bGoldLimitTimeFree = false
    -- 底分配置
    self.m_tabCellScoreList = {}
    -- 代开模式
    self.m_bCreateForOther = false

    -- 额外配置
    self.m_tabRule = {}
end

function PriRoom:showPopWait()
    if nil ~= self._scene then
        self._scene:showPopWait()
    end
end

function PriRoom:dismissPopWait()
    if nil ~= self._scene then
        self._scene:dismissPopWait()
    end
end

-- 请求退出游戏
function PriRoom:queryQuitGame( cbGameStatus )
    if self.m_bCancelTable then
        print("PriRoom:queryQuitGame 已经取消!")
        return
    end
    if nil ~= self._queryDialog then
        return
    end
    if 1 == PriRoom:getInstance().cbIsJoinGame 
        and PriRoom:getInstance().m_bIsMyRoomOwner then
        local tip = "你是房主, 是否要解散该房间?"
        self._queryDialog = QueryDialog:create(tip, function(ok)
                if ok == true then
                    --self:showPopWait()
                    self:getNetFrame():sendRequestDissumeGame()
                end
                self._queryDialog = nil
            end)
        :setCanTouchOutside(false)
        :addTo(self._viewFrame)
        return
    end
    
    -- 未玩且free
    if 0 == PriRoom:getInstance().m_tabPriData.dwPlayCount 
        and 0 == cbGameStatus then
        -- 发送起立
        self:getNetFrame()._gameFrame:StandUp(1)
        -- 暂离(20170721修改, 退出房间保留房号)
        self:tempLeaveGame()
        self._scene:onKeyBack()
        return
    end

    local tip = "约战房在游戏中退出需其他玩家同意, 是否申请解散房间?"
    self._queryDialog = QueryDialog:create(tip, function(ok)
            if ok == true then    
                --self:showPopWait()
                self:getNetFrame():sendRequestDissumeGame()
            end
            self._queryDialog = nil
        end)
    :setCanTouchOutside(false)
    :addTo(self._viewFrame)
end

-- 请求解散房间
function PriRoom:queryDismissRoom()
    if self.m_bCancelTable then
        print("PriRoom:queryQuitGame 已经取消!")
        return
    end
    local tip = "约战房在游戏中退出需其他玩家同意, 是否申请解散房间?"
    QueryDialog:create(tip, function(ok)
            if ok == true then 
                --self:showPopWait()   
                self:getNetFrame():sendRequestDissumeGame()
            end
        end)
    :setCanTouchOutside(false)
    :addTo(self._viewFrame)
end

-- 获取邀请分享内容
function PriRoom:getInviteShareMsg( roomDetailInfo )
    local entergame = self._scene:getEnterGameInfo()
    if nil ~= entergame then
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        local gameFile = ""
        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            gameFile = "game/" .. modulestr .. "src/privateroom/PriRoomCreateLayer.lua" .. yl.WIN_SRC_SUFFIX
        else
            gameFile = "game/" .. modulestr .. "src/privateroom/PriRoomCreateLayer.lua"
        end
        if cc.FileUtils:getInstance():isFileExist(gameFile) then
            return appdf.req(gameFile):getInviteShareMsg( roomDetailInfo )
        end
    end
    return {title = "", content = ""}
end

-- 获取复制分享内容
function PriRoom:getCopyShareMsg( roomDetailInfo )
    local entergame = self._scene:getEnterGameInfo()
    if nil ~= entergame then
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        local gameFile = ""
        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            gameFile = "game/" .. modulestr .. "src/privateroom/PriRoomCreateLayer.lua" .. yl.WIN_SRC_SUFFIX
        else
            gameFile = "game/" .. modulestr .. "src/privateroom/PriRoomCreateLayer.lua"
        end
        if cc.FileUtils:getInstance():isFileExist(gameFile) then
            return appdf.req(gameFile):getCopyShareMsg( roomDetailInfo )
        end
    end
    return {content = ""}
end

-- 私人房邀请好友
function PriRoom:priInviteFriend(frienddata, gameKind, wServerNumber, tableId, inviteMsg)
    if nil == frienddata or nil == self._scene.inviteFriend then
        return
    end
    if not gameKind then
        gameKind = GlobalUserItem.nCurGameKind
    else
        gameKind = tonumber(gameKind)
    end
    local id = frienddata.dwUserID
    if nil == id then
        return
    end
    local tab = {}
    tab.dwInvitedUserID = id
    tab.wKindID = gameKind
    tab.wServerNumber = wServerNumber or 0
    tab.wTableID = tableId or 0
    tab.szInviteMsg = inviteMsg or ""
    if FriendMgr:getInstance():sendInvitePrivateGame(tab) then
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToast(runScene, "邀请消息已发送!", 1)
        end
    end
end

-- 分享图片给好友
function PriRoom:imageShareToFriend( frienddata, imagepath, sharemsg )
    if nil == frienddata or nil == self._scene.imageShareToFriend then
        return
    end
    local id = frienddata.dwUserID
    if nil == id then
        return
    end
    self._scene:imageShareToFriend(id, imagepath, sharemsg)
end

-- 暂离游戏
-- @param[nKindId] 游戏kindid
-- @param[dwRoomId] 房间id
function PriRoom:tempLeaveGame( nKindId, dwRoomId )
    nKindId = nKindId or GlobalUserItem.nCurGameKind
    dwRoomId = dwRoomId or self.m_tabPriData.dwPersonalRoomID
    local joinGame = {roomid = dwRoomId}
    self.m_tabJoinGameRecord[nKindId] = joinGame

    self:getNetFrame()._gameFrame:setEnterAntiCheatRoom(false)                
    self:getNetFrame()._gameFrame:onCloseSocket()
end

-- 断开游戏服务器连接
function PriRoom:closeGameSocket()
    -- 清理动作
    self.m_nLoginAction = L_ACTION.ACT_NULL
    -- 清理记录
    self.m_dwTableID = 0

    -- 断开连接
    self:getNetFrame()._gameFrame:setEnterAntiCheatRoom(false)                
    self:getNetFrame()._gameFrame:onCloseSocket()

    -- 清理房间记录
    GlobalUserItem.nCurRoomIndex = -1
end

-- 弹出AA制窗口
function PriRoom:popAAPayLayer( param )
    PriRoom:getInstance():dismissPopWait()
    -- 数据校验
    if nil == self._scene then
        self:closeGameSocket()
        return
    end
    local paaL = self:getTagLayer(LAYTAG.LAYER_AAPAY, self._scene, param)
    if nil == paaL then
        self:closeGameSocket()
        return
    end
    self._scene:addPopLayer(paaL)
end

-- 获取密码
function PriRoom:queryRoomPasswd( roomid, callback )
    self:showPopWait()
    -- todo 判断是否免密
    print("获取密码进入", type(roomid), roomid)
    if type(roomid) == "number" then
        self.m_pGetPasswdCallBack = callback
        -- 请求分享密码
        self:getNetFrame():onQuerySharePasswd(roomid)
    end
end

-- 密码输入
function PriRoom:passwdInput()
    self:dismissPopWait()
    -- 游戏界面
    self._scene:onEnterTable()
    -- 密码输入
    local pL = self:getTagLayer(LAYTAG.LAYER_PASSWDIDPUT, self._scene)
    if nil == pL then
        self:closeGameSocket()
        return
    end
    self._scene:addPopLayer(pL)
end