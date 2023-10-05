--
-- Author: zhong
-- Date: 2016-12-28 16:16:34
--
-- 私人房网络处理
local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local PriFrame = class("PriFrame",BaseFrame)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")

local cmd_private = appdf.req(PriRoom.MODULE.PRIHEADER .. "CMD_Private")
local define_private = appdf.req(PriRoom.MODULE.PRIHEADER .. "Define_Private")
local struct_private = appdf.req(PriRoom.MODULE.PRIHEADER .. "Struct_Private")

-- 登陆服务器CMD
local cmd_pri_login = cmd_private.login
-- 游戏服务器CMD
local cmd_pri_game = cmd_private.game

PriFrame.OP_CREATEROOM = cmd_pri_login.SUB_MB_QUERY_GAME_SERVER                 -- 创建房间
PriFrame.OP_SEARCHROOM = cmd_pri_login.SUB_MB_SEARCH_SERVER_TABLE               -- 查询房间
PriFrame.OP_ROOMPARAM = cmd_pri_login.SUB_MB_GET_PERSONAL_PARAMETER             -- 私人房配置
PriFrame.OP_QUERYLIST = cmd_pri_login.SUB_MB_QUERY_PERSONAL_ROOM_LIST           -- 私人房列表
PriFrame.OP_DISSUMEROOM = cmd_pri_login.SUB_MB_DISSUME_SEARCH_SERVER_TABLE      -- 解散桌子
PriFrame.OP_QUERYJOINLIST = cmd_pri_login.SUB_GR_USER_QUERY_ROOM_SCORE          -- 查询参与列表
PriFrame.OP_EXCHANGEROOMCARD = cmd_pri_login.SUB_MB_ROOM_CARD_EXCHANGE_TO_SCORE -- 房卡兑换游戏币
PriFrame.OP_QUERYSHAREPASSWD = cmd_pri_login.SUB_MB_QUERY_ROOM_PASSWORD         -- 查询分享密码
PriFrame.OP_QUERYGOLDLIST = cmd_pri_login.SUB_MB_QUERY_GOLD_ROOM_LIST           --请求金币私有房列表

local function LOGINSERVER(code)
    return { m = cmd_pri_login.MDM_MB_PERSONAL_SERVICE, s = code }
end
local function GAMESERVER(code)
    return { m = cmd_pri_game.MDM_GR_PERSONAL_TABLE, s = code }
end

function PriFrame:ctor(view,callbcak)
    PriFrame.super.ctor(self,view,callbcak)
end

-- 创建房间
function PriFrame:onCreateRoom(configId)
    --操作记录
    self._oprateCode = PriFrame.OP_CREATEROOM
    self._clubConfigId = configId or 0

    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
    -- 动作定义
    PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_CREATEROOM
end

-- 查询房间
function PriFrame:onSearchRoom( roomId ,isLookOn)

    GlobalUserItem.bLookOn = isLookOn or false
    --操作记录
    self._oprateCode = PriFrame.OP_SEARCHROOM
    self._roomId = roomId or 0
    self:onCloseSocket()
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
end

-- 私人房间配置
function PriFrame:onGetRoomParameter()
    --操作记录
    self._oprateCode = PriFrame.OP_ROOMPARAM
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
end

-- 查询私人房列表
function PriFrame:onQueryRoomList()
    --操作记录
    self._oprateCode = PriFrame.OP_QUERYLIST
    self:onCloseSocket()
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
end

-- 解散房间
function PriFrame:onDissumeRoom( roomId )
    --操作记录
    self._oprateCode = PriFrame.OP_DISSUMEROOM
    self._roomId = roomId or 0
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
end

-- 查询参与列表
function PriFrame:onQueryJoinList()
    --操作记录
    self._oprateCode = PriFrame.OP_QUERYJOINLIST
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
end

-- 房卡兑换游戏币
function PriFrame:onExchangeScore( lCount )
    --操作记录
    self._oprateCode = PriFrame.OP_EXCHANGEROOMCARD
    self._lExchangeRoomCard = lCount
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
end

-- 请求分享密码
function PriFrame:onQuerySharePasswd( roomId )
    self:onCloseSocket()
    --操作记录
    self._oprateCode = PriFrame.OP_QUERYSHAREPASSWD
    self._roomId = roomId or 0
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
end

-- 查询金币私人房列表
function PriFrame:onQueryGoldRoomList()
    --操作记录
    self._oprateCode = PriFrame.OP_QUERYGOLDLIST
    self:onCloseSocket()
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"")
    end
end

function PriFrame:onSocketError(pData)
    if self._threadid == nil then
        return
    end


    --self._scene:dismissPopWait()
    self:onCloseSocket()

    local curTag = nil
    if nil ~= PriRoom:getInstance()._scene then
        curTag = PriRoom:getInstance()._scene._sceneLayer:getCurrentTag()
    end

    if curTag == yl.SCENE_GAME then
            if nil ~= PriRoom:getInstance()._scene and  PriRoom:getInstance()._scene.m_bIsReConnet == false  then
                PriRoom:getInstance()._scene.m_bIsReConnet = true
                PriRoom:getInstance()._scene:showGameSeverReConnect()
            end
            return 
    end
 
        if self._callBack ~= nil then
            if not pData then
                self._callBack(-1,"网络断开！")
            elseif type(pData) == "string" then
                self._callBack(-1,pData)
            else
                local errorcode = pData:readword()
                if errorcode == nil then
                    self._callBack(-1,"网络断开！")
                elseif errorcode == 6 then
                    self._callBack(-1,"长时间无响应，网络断开！")
                elseif errorcode == 3 then
                    self._callBack(-1,"网络连接超时, 请重试!")
                    -- 切换地址
                    if nil ~= yl.SERVER_LIST[yl.CURRENT_INDEX] then
                        yl.LOGONSERVER = yl.SERVER_LIST[yl.CURRENT_INDEX]
                    end
                    yl.CURRENT_INDEX = yl.CURRENT_INDEX + 1
                    if yl.CURRENT_INDEX > yl.TOTAL_COUNT then
                        yl.CURRENT_INDEX = 1
                    end     
                else
                    if errorMsg ~= nil and errorMsg[errorcode] ~= nil then
                        --self._callBack(-1,"网络错误, " .. errorMsg[errorcode])  
                    end        
                end
            end
        end
    
end


--连接结果
function PriFrame:onConnectCompeleted()

    if self._oprateCode == PriFrame.OP_CREATEROOM then              -- 创建房间
        self:sendCreateRoom()
    elseif self._oprateCode == PriFrame.OP_SEARCHROOM then          -- 查询房间
        self:sendSearchRoom()
    elseif self._oprateCode == PriFrame.OP_ROOMPARAM then           -- 私人房配置
        self:sendQueryRoomParam()
    elseif self._oprateCode == PriFrame.OP_QUERYLIST then           -- 请求私人房列表
        self:sendQueryRoomList()
    elseif self._oprateCode == PriFrame.OP_DISSUMEROOM then         -- 解散桌子
        self:sendDissumeRoom()
    elseif self._oprateCode == PriFrame.OP_QUERYJOINLIST then       -- 参与列表
        self:sendQueryJoinRoomList()
    elseif self._oprateCode == PriFrame.OP_EXCHANGEROOMCARD then    -- 房卡兑换游戏币
        self:sendExchangeScore()
    elseif self._oprateCode == PriFrame.OP_QUERYSHAREPASSWD then    -- 分享密码
        self:sendQuerySharePasswd()
    elseif self._oprateCode == PriFrame.OP_QUERYGOLDLIST then           -- 请求金币私人房列表
        self:sendQueryGoldRoomList()
    else
        self:onCloseSocket()
        if nil ~= self._callBack then
            self._callBack(LOGINSERVER(-1),"未知操作模式！")
        end 
        PriRoom:getInstance():dismissPopWait()
    end
end

-- 发送创建房间
function PriFrame:sendCreateRoom()
    print("PriFrame:sendCreateRoom")

    if self._clubConfigId and self._clubConfigId ~= 0 then
        for k,v in pairs(ClubFrame:getInstance().m_tabGroupConfig[ClubFrame:getInstance().m_nCurrentClub]) do
            if v and v.dwConfigID ==  self._clubConfigId then
                PriRoom:getInstance().m_bPlayModel = v.cbPlayMode
                GlobalUserItem.nCurGameKind = v.dwKindID
                break
            end
        end
    end

    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_MB_QueryGameServer)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_MB_QUERY_GAME_SERVER)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(GlobalUserItem.nCurGameKind)
    buffer:pushbyte(PriRoom:getInstance().m_tabRoomOption.cbIsJoinGame)

    buffer:pushbyte(PriRoom:getInstance().m_bPlayModel)
    buffer:pushdword(ClubFrame:getInstance().m_nCurrentClub)
    buffer:pushdword(self._clubConfigId)
    
    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送创建房间失败！")
    end
end

-- 发送查询私人房
function PriFrame:sendSearchRoom()
    print("PriFrame:sendSearchRoom")
    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_MB_SerchServerTable)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_MB_SEARCH_SERVER_TABLE)
    --buffer:pushstring(self._roomId, define_private.ROOM_ID_LEN)
    buffer:pushdword(self._roomId)
    --buffer:pushdword(GlobalUserItem.nCurGameKind)
    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送查询房间失败！")
    end
end

-- 发送请求配置
function PriFrame:getSendQueryRoomParamBuffer()
    print("PriFrame:getSendQueryRoomParamBuffer")
    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_MB_GetPersonalParameter)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_MB_GET_PERSONAL_PARAMETER)
    buffer:pushdword(GlobalUserItem.nCurGameKind)
    

    return buffer
end

function PriFrame:sendQueryRoomParam()
    local buffer = self:getSendQueryRoomParamBuffer()
    if nil == buffer or (not self:sendSocketData(buffer) and nil ~= self._callBack) then
        self._callBack(LOGINSERVER(-1),"发送请求配置失败！")
    end
end

-- 发送查询私人房列表
function PriFrame:sendQueryRoomList()
    print("PriFrame:sendQueryRoomList")
    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_MB_QeuryPersonalRoomList)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_MB_QUERY_PERSONAL_ROOM_LIST)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    --buffer:pushdword(GlobalUserItem.nCurGameKind)
    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送查询房间列表失败！")
    end
end
-- 发送查询金币私人房列表
function PriFrame:sendQueryGoldRoomList()
    print("PriFrame:sendQueryGoldRoomList")
    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_MB_S_C_QueryGameServer)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_MB_QUERY_GOLD_ROOM_LIST)
    buffer:pushword(GlobalUserItem.nCurGameKind)
    buffer:pushbyte(1)
    buffer:pushword(0)
    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送查询房间列表失败！")
    end
end

-- 发送解散房间
function PriFrame:sendDissumeRoom()
    print("PriFrame:sendDissumeRoom")
    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_MB_SearchServerTable)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_MB_DISSUME_SEARCH_SERVER_TABLE)
    --buffer:pushstring(self._roomId, define_private.ROOM_ID_LEN)
    buffer:pushdword(self._roomId)
    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送解散房间失败！")
    end
end

-- 发送查询参与列表
function PriFrame:sendQueryJoinRoomList()
    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_MB_QeuryPersonalRoomList)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_GR_USER_QUERY_ROOM_SCORE)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    --buffer:pushdword(GlobalUserItem.nCurGameKind)
    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送查询参与列表失败！")
    end
end

-- 请求分享密码
function PriFrame:sendQuerySharePasswd()
    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_MB_SearchServerTable)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_MB_QUERY_ROOM_PASSWORD)
    --buffer:pushstring(self._roomId, define_private.ROOM_ID_LEN)
    buffer:pushdword(self._roomId)
    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送解散房间失败！")
    end
end

-- 发送房卡兑换游戏币
function PriFrame:sendExchangeScore()
    local buffer = ExternalFun.create_netdata(cmd_pri_login.CMD_GP_ExchangeScoreByRoomCard)
    buffer:setcmdinfo(cmd_pri_login.MDM_MB_PERSONAL_SERVICE,cmd_pri_login.SUB_MB_ROOM_CARD_EXCHANGE_TO_SCORE)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushscore(self._lExchangeRoomCard)
    buffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送兑换游戏币失败！")
    end
end

-- 发送游戏服务器消息
function PriFrame:sendGameServerMsg( buffer )    
    if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
        if not self._gameFrame:sendSocketData(buffer) then
            self._callBack(GAMESERVER(-1),"发送失败！")
        end
    end    
end

-- 发送进入私人房
function PriFrame:sendEnterPrivateGame( dwPasswd )
    if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
        local c = yl.INVALID_TABLE,yl.INVALID_CHAIR
        -- 找座椅
        local chaircount = self._gameFrame._wChairCount
        for i = 1, chaircount  do
            local sc = i - 1
            if nil == self._gameFrame:getTableUserItem(PriRoom:getInstance().m_dwTableID, sc) then
                c = sc
                break
            end
        end
        print( "PriFrame:sendEnterPrivateGame ==> private enter " .. PriRoom:getInstance().m_dwTableID .. " ## " .. c)
        dwPasswd = dwPasswd or 0

        self._gameFrame:SitDown(PriRoom:getInstance().m_dwTableID, c, nil, dwPasswd)
        self._gameFrame:SendGameOption()
    end
end

-- 发送旁观私人房
function PriFrame:sendLookOnPrivateGame()
    if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then

        self._gameFrame:LookOn(PriRoom:getInstance().m_dwTableID, 0)
        self._gameFrame:SendGameOption()
    end
end



-- 强制解散游戏
function PriFrame:sendDissumeGame( tableId )
    tableId = tableId or 0
    if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
        local buffer = ExternalFun.create_netdata(cmd_pri_game.CMD_GR_HostDissumeGame)
        buffer:setcmdinfo(cmd_pri_game.MDM_GR_PERSONAL_TABLE,cmd_pri_game.SUB_GR_HOSTL_DISSUME_TABLE)
        buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
        buffer:pushdword(tableId)
        if not self._gameFrame:sendSocketData(buffer) then
            self._callBack(GAMESERVER(-1),"发送解散游戏失败！")
        end
    end
end

-- 请求解散游戏
function PriFrame:sendRequestDissumeGame()
    if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
        print("game socket sendRequestDissumeGame")
        local buffer = ExternalFun.create_netdata(cmd_pri_game.CMD_GR_CancelRequest)
        buffer:setcmdinfo(cmd_pri_game.MDM_GR_PERSONAL_TABLE,cmd_pri_game.SUB_GR_CANCEL_REQUEST)
        buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
        buffer:pushdword(self._gameFrame:GetTableID())
        buffer:pushdword(self._gameFrame:GetChairID())
        if not self._gameFrame:sendSocketData(buffer) then
            self._callBack(GAMESERVER(-1),"请求解散游戏失败！")
        end
    end
end

-- 回复请求
function PriFrame:sendRequestReply( cbAgree )
    if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
        local buffer = ExternalFun.create_netdata(cmd_pri_game.CMD_GR_RequestReply)
        buffer:setcmdinfo(cmd_pri_game.MDM_GR_PERSONAL_TABLE,cmd_pri_game.SUB_GR_REQUEST_REPLY)
        buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
        buffer:pushdword(self._gameFrame:GetTableID())
        buffer:pushbyte(cbAgree)
        if not self._gameFrame:sendSocketData(buffer) then
            self._callBack(GAMESERVER(-1),"回复请求失败！")
        end
    end
end

-- 请求加入信息
function PriFrame:sendQueryJoinInfo( dwTableID )
    if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
        print("QueryJoinTable ==> ", dwTableID)
        local buffer = ExternalFun.create_netdata(cmd_pri_game.CMD_GR_QueryJoinInfo)
        buffer:setcmdinfo(cmd_pri_game.MDM_GR_PERSONAL_TABLE,cmd_pri_game.CMD_GR_QUERY_JOIN_INFO)
        buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
        buffer:pushdword(dwTableID)
        if not self._gameFrame:sendSocketData(buffer) then
            self._callBack(GAMESERVER(-1),"加入信息请求失败")
        end
    end
end


-- 俱乐部玩法创建房间
function PriFrame:sendClubCreateRoom()
        local clubConfigInfo = {}
        local configList = ClubFrame:getInstance().m_tabGroupConfig[ClubFrame:getInstance().m_nCurrentClub]
        for k,v in pairs(configList) do
            if v and v.dwConfigID ==  self._clubConfigId then
                clubConfigInfo = clone(v)
            end
        end

        local buffer = ExternalFun.create_netdata(128) --CCmd_Data:create(188)
            buffer:setcmdinfo(cmd_pri_game.MDM_GR_PERSONAL_TABLE,cmd_pri_game.SUB_GR_CREATE_TABLE)
            buffer:pushscore(clubConfigInfo.lCellScore)
            buffer:pushdword(clubConfigInfo.dwDrawCountLimit)
            buffer:pushdword(clubConfigInfo.dwDrawTimeLimit)
            buffer:pushword(clubConfigInfo.wJoinGamePeopleCount)
            buffer:pushdword(clubConfigInfo.dwRoomTax)
            -- 密码设置
            buffer:pushbyte(clubConfigInfo.cbPassword)
            -- 支付方式
            buffer:pushbyte(clubConfigInfo.cbPayMode)


            for i = 1, 100 do
                buffer:pushbyte(clubConfigInfo.cbGameRule[1][i])
            end
            buffer:pushbyte(clubConfigInfo.cbGameMode)
            buffer:pushscore(clubConfigInfo.lEnterScore)
            buffer:pushscore(clubConfigInfo.lLeaveScore)
            
            buffer:pushbyte(clubConfigInfo.cbJoinMode)

            buffer:pushdword(clubConfigInfo.dwGroupID)
            buffer:pushdword(clubConfigInfo.dwConfigID)

        if not self._gameFrame:sendSocketData(buffer) then
            self._callBack(GAMESERVER(-1),"加入信息请求失败")
        end

end



--网络信息(短连接)
function PriFrame:onSocketEvent( main,sub,pData )
    print("PriFrame:onSocketEvent ==> " .. main .. "##" .. sub)
    local needClose = true
    local dissmissPop = true
    if cmd_pri_login.MDM_MB_PERSONAL_SERVICE == main then
        if cmd_pri_login.SUB_MB_SEARCH_RESULT == sub then                           -- 房间搜索结果
            dissmissPop = self:onSubSearchRoomResult(pData)
        elseif cmd_pri_login.SUB_MB_DISSUME_SEARCH_RESULT == sub then               -- 解散搜索结果
            self:onSubDissumeSearchReasult(pData)
        elseif cmd_pri_login.SUB_MB_QUERY_PERSONAL_ROOM_LIST_RESULT == sub then     -- 私人房列表
            self:onSubPrivateRoomList(pData)
            needClose = false
        elseif cmd_pri_login.SUB_GR_USER_QUERY_ROOM_SCORE_RESULT == sub then        -- 参与列表
            self:onSubJoinRoomList(pData)
        elseif cmd_pri_login.SUB_MB_PERSONAL_PARAMETER == sub then                  -- 私人房间属性
            needClose = false
            self:_onSubParameter(pData)
        elseif cmd_pri_login.SUB_MB_PERSONAL_FEE_PARAMETER == sub then              -- 私人房费用配置
            needClose = false
            self:onSubFeeParameter(pData)
        elseif cmd_pri_login.SUB_MB_QUERY_GAME_SERVER_RESULT == sub then            -- 创建结果
            dissmissPop = self:onSubGameServerResult(pData)
        elseif cmd_pri_login.SUB_GP_EXCHANGE_ROOM_CARD_RESULT == sub then           -- 房卡兑换游戏币结果
            self:onSubExchangeRoomCardResult(pData)
        elseif cmd_pri_login.SUB_MB_QUERY_ROOM_PASSWORD_RESULT == sub then          -- 分享密码
            self:onSubSharePasswdResult(pData)
        elseif cmd_pri_login.SUB_MB_PERSONAL_CELL_SCORE == sub then                 -- 私人房底分配置
            needClose = false
            self:onSubCellScoreList(pData)
        elseif cmd_pri_login.SUB_MB_PERSONAL_RULE_RESULT == sub then                 -- 私人房额外配置
            self:onSubRuleResult(pData)
        elseif cmd_pri_login.SUB_MB_QUERY_PERSONAL_END == sub then
            needClose = true
        elseif cmd_pri_login.SUB_MB_QUERY_GOLD_ROOM_LIST_RESULT == sub then     -- 金币私人房列表
            needClose = false
            self:onSubPrivateGoldRoomList(pData)
        elseif cmd_pri_login.SUB_MB_QUERY_GOLD_ROOM_LIST_FINISH == sub then     -- 金币私人房列表结束
            needClose = true
            PriRoom:getInstance().m_bRoomListFinsh = true
        end
    end

    if needClose then        
        self:onCloseSocket()
        if dissmissPop then
            PriRoom:getInstance():dismissPopWait()
        end
    end    
end

-- 房间搜索结果
function PriFrame:onSubSearchRoomResult( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_login.CMD_MB_SearchResult, pData)
    dump(cmd_table, "CMD_MB_SearchResult", 6)
    if 0 == cmd_table.dwServerID then
        if nil ~= self._callBack then
            cmd_table.popMsg = "该房间ID不存在, 房间不存在或已解散!"
            self._callBack(LOGINSERVER(cmd_pri_login.SUB_MB_SEARCH_RESULT), cmd_table)
            PriRoom:getInstance().m_tabJoinGameRecord[GlobalUserItem.nCurGameKind] = {}

            -- 客户端通知
            local event = cc.EventCustom:new(appdf.CLIENT_NOTIFY)
            event.msg = msg
            event.what = yl.CLIENT_MSG_PRIVATEROOM_REFRESH
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
        end
        return true
    end
    local fun = function()
        -- 信息记录
        PriRoom:getInstance().m_dwTableID = cmd_table.dwTableID
        -- 游戏标识
        GlobalUserItem.nCurGameKind = cmd_table.dwKindID

        GlobalUserItem.dwCurRoomGroupID = cmd_table.dwGroupID

        -- 动作定义
        PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_QUERYJOININFO
        -- 发送登陆
        PriRoom:getInstance():onLoginRoom(cmd_table.dwServerID)
    end
    
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    -- 游戏版本检查
    if nil ~= PriRoom:getInstance()._scene and nil ~= PriRoom:getInstance()._scene.getApp then
        if cc.PLATFORM_OS_WINDOWS == targetPlatform and yl.DISABLE_WIN_UPDATE then
            print("win32 跳过更新")
        else
            local scene = PriRoom:getInstance()._scene
            local app = PriRoom:getInstance()._scene:getApp()
            local prigame = scene:getGameInfo(cmd_table.dwKindID)
            local version = tonumber(app:getVersionMgr():getResVersion(cmd_table.dwKindID))
            if not version or prigame._ServerResVersion > version then
                if false == scene:singleGameUpdate(cmd_table.dwKindID, function()
                        print("PriFrame:onSubSearchRoomResult update over")
                        PriRoom:getInstance():showPopWait()
                        fun()
                    end) then
                    if nil ~= prigame then
                        local gamename = prigame._GameName or ""
                        local msg = string.format("请更新 %s 游戏资源!", gamename)
                        showToast(scene, msg, 2)
                    else
                        showToast(scene, "请更新游戏资源!", 2)
                    end
                    return true
                end
                PriRoom:getInstance():dismissPopWait()
                return false
            end
        end
    else
        showToast(PriRoom:getInstance()._scene, "游戏版本异常!", 2)
        PriRoom:getInstance():dismissPopWait()
        return true
    end
    fun()
    return false
end

-- 解散搜索结果
function PriFrame:onSubDissumeSearchReasult( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_login.CMD_MB_DissumeSearchResult, pData)
    dump(cmd_table, "CMD_MB_DissumeSearchResult", 6)

    -- 信息记录
    PriRoom:getInstance().m_dwTableID = cmd_table.dwTableID
    -- 动作定义
    PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_DISSUMEROOM
    -- 发送登陆
    PriRoom:getInstance():onLoginRoom(cmd_table.dwServerID)
end

-- 私人房列表
function PriFrame:onSubPrivateRoomList( pData )
    if PriRoom:getInstance().bIsRoomListEnd then
        PriRoom:getInstance().m_tabCreateRecord = {}
    end
    print("PriFrame:onSubPrivateRoomList  获取私有房列表")

    local cmd_table = ExternalFun.read_netdata(cmd_pri_login.CMD_MB_PersonalRoomInfoList, pData)
    dump(cmd_table, "@@@@onSubPrivateRoomList@@@", 7)

    PriRoom:getInstance().bIsRoomListEnd = cmd_table.bListEnd
    local listCount = cmd_table.nRoomCount
    
    --logFunc(cmd_table, true)
    --local listinfo = cmd_table.PersonalRoomInfo[1]
    for i = 1, listCount do
        local info = ExternalFun.read_netdata(struct_private.tagPersonalRoomInfo, pData)
        if info.dwPersonalRoomID ~= 0 then
            --info.lScore = info.lTaxCount--self:getMyReword(info.PersonalUserScoreInfo[1])
            -- 时间戳
            local tt = info.sysDissumeTime
            info.sortTimeStmp = os.time({day=tt.wDay, month=tt.wMonth, year=tt.wYear, hour=tt.wHour, min=tt.wMinute, sec=tt.wSecond}) or 0
            tt = info.sysCreateTime
            info.createTimeStmp = os.time({day=tt.wDay, month=tt.wMonth, year=tt.wYear, hour=tt.wHour, min=tt.wMinute, sec=tt.wSecond}) or 0
            table.insert(PriRoom:getInstance().m_tabCreateRecord, info)
        else
            break
        end
    end
    table.sort( PriRoom:getInstance().m_tabCreateRecord, function(a, b)
        if a.cbIsDisssumRoom ~= b.cbIsDisssumRoom then
            return a.cbIsDisssumRoom < b.cbIsDisssumRoom
        elseif a.cbIsDisssumRoom == 0 and a.cbIsDisssumRoom == b.cbIsDisssumRoom then
            return a.createTimeStmp > b.createTimeStmp
        else
            return a.sortTimeStmp > b.sortTimeStmp
        end        
    end )

    print("------------------------------------------------->", cmd_table.bListEnd, PriRoom:getInstance().bIsRoomListEnd, #PriRoom:getInstance().m_tabCreateRecord)
    if cmd_table.bListEnd then
        self:onCloseSocket()
        PriRoom:getInstance():dismissPopWait()
        --接收完成，执行回调
        if nil ~= self._callBack then
            self._callBack(LOGINSERVER(cmd_pri_login.SUB_MB_QUERY_PERSONAL_ROOM_LIST_RESULT))
        end
    end
end

-- 金币私人房列表
function PriFrame:onSubPrivateGoldRoomList( pData )
    if PriRoom:getInstance().m_bRoomListFinsh then
        PriRoom:getInstance().m_tabGoldRoomList = {}
    end
    --收到列表开始设置为false，收到完成，设置为true
    PriRoom:getInstance().m_bRoomListFinsh = false

    --计算数目
    local len = pData:getlen()
    print("onSubPrivateGoldRoomList:"..len)
    local itemcount =  math.floor(len/define_private.LEN_GOLDROOMINFO)
    print("=============== onSubPrivateGoldRoomList ================ ", itemcount)
    if (len - itemcount * define_private.LEN_GOLDROOMINFO) ~= 0 then
        print("Goldroomlist_len_error"..len)
        return
    end

    for i = 1, itemcount do
        local cmd_table = ExternalFun.read_netdata(struct_private.tagGoldPersonalRoomInfo, pData)
        local roomInfo = cmd_table 
        local buffer = {}
        for j=1,100 do
            local data = pData:readbyte()
            table.insert(buffer, data)
        end
        roomInfo.cbGameRule = buffer
        local cmd_data1=ExternalFun.read_netdata({{t = "string", k = "szCreateTime", s = 32}}, pData)
        roomInfo.szCreateTime = cmd_data1.szCreateTime

        if cmd_table.dwPersonalRoomID ~= 0 then
            table.insert(PriRoom:getInstance().m_tabGoldRoomList, roomInfo)
        else
            break
        end
    end
    --dump(PriRoom:getInstance().m_tabGoldRoomList, "onSubPrivateGoldRoomList", 2)
    -- table.sort( PriRoom:getInstance().m_tabCreateRecord, function(a, b)
    --     if a.cbIsDisssumRoom ~= b.cbIsDisssumRoom then
    --         return a.cbIsDisssumRoom < b.cbIsDisssumRoom
    --     elseif a.cbIsDisssumRoom == 0 and a.cbIsDisssumRoom == b.cbIsDisssumRoom then
    --         return a.createTimeStmp > b.createTimeStmp
    --     else
    --         return a.sortTimeStmp > b.sortTimeStmp
    --     end        
    -- end )
    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_pri_login.SUB_MB_QUERY_GOLD_ROOM_LIST_RESULT))
    end
end


-- 参与列表
function PriFrame:onSubJoinRoomList( pData )
    PriRoom:getInstance().m_tabJoinRecord = {}
    -- 计算数目
    local len = pData:getlen()
    local itemcount = math.floor(len/define_private.LEN_PERSONAL_ROOM_SCORE)
    print("PriFrame onSubJoinRoomList ==> ", itemcount)
    for i = 1, itemcount do
        local pServer = ExternalFun.read_netdata(struct_private.tagQueryPersonalRoomUserScore, pData)
        pServer.lScore = self:getMyReword(pServer["PersonalUserScoreInfo"][1])
        -- 时间戳
        local tt = pServer.sysDissumeTime
        pServer.sortTimeStmp = os.time({day=tt.wDay, month=tt.wMonth, year=tt.wYear, hour=tt.wHour, min=tt.wMinute, sec=tt.wSecond}) or 0
        pServer.bFlagOnGame = (pServer.sortTimeStmp == 0)
        pServer.nOnGameOrder = pServer.bFlagOnGame and 1 or 0
        tt = pServer.sysCreateTime
        pServer.sortCreateTimeStmp = os.time({day=tt.wDay, month=tt.wMonth, year=tt.wYear, hour=tt.wHour, min=tt.wMinute, sec=tt.wSecond}) or 0
        table.insert(PriRoom:getInstance().m_tabJoinRecord, pServer)        
    end
    table.sort( PriRoom:getInstance().m_tabJoinRecord, function(a, b)
        if a.bFlagOnGame ~= b.bFlagOnGame then
            if a.nOnGameOrder == b.nOnGameOrder then
                return a.sortCreateTimeStmp < b.sortCreateTimeStmp
            else
                return a.nOnGameOrder < b.nOnGameOrder
            end
        else
            return a.sortTimeStmp < b.sortTimeStmp
        end
    end )
    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_pri_login.SUB_GR_USER_QUERY_ROOM_SCORE_RESULT))
    end
end

function PriFrame:getMyReword( list )
    if type(list) ~= "table" then
        return 0
    end
    for k,v in pairs(list) do
        if v["dwUserID"] == GlobalUserItem.tabAccountInfo.dwUserID then
            return (tonumber(v.lScore) or 0)
        end
    end
    return 0
end

-- 房间属性
function PriFrame:_onSubParameter( pData )
    --计算类型数目
    local len = pData:getlen()
    print("=============== _onSubParameter ================")
    print("_onSubParameter:"..len)
    if (len - math.floor(len/78)*78) ~= 0 then
        print("len_error"..len)
        return
    end

    PriRoom:getInstance().m_tabGoldRoomOption = {}
    local itemcount =  math.floor(len/78)
    for i = 1,itemcount do
        local cmd_data = ExternalFun.read_netdata( struct_private.tagPersonalRoomOption, pData )
        if 0 == cmd_data.cbPlayMode then --积分房卡
            PriRoom:getInstance().m_tabRoomOption = cmd_data
            dump(PriRoom:getInstance().m_tabRoomOption, "tagPersonalRoomOption")
            PriRoom:getInstance().cbIsJoinGame = PriRoom:getInstance().m_tabRoomOption.cbIsJoinGame

            PriRoom:getInstance().m_bLimitTimeFree = false
            -- 限免判断
            local wStart = cmd_data.wBeginFeeTime
            local wEnd = cmd_data.wEndFeeTime
            -- 当前时间判定
            local curDate = os.date("*t")
            local curHour = tonumber(curDate.hour)
            if wStart <= curHour and curHour < wEnd then
                PriRoom:getInstance().m_bLimitTimeFree = true
            end
        elseif 1 == cmd_data.cbPlayMode then --金币房卡
            PriRoom:getInstance().m_tabGoldRoomOption = cmd_data
            dump(PriRoom:getInstance().m_tabGoldRoomOption, "tagPersonalGoldRoomOption")
            
            PriRoom:getInstance().m_bGoldLimitTimeFree = false
            -- 限免判断
            local wStart = cmd_data.wBeginFeeTime
            local wEnd = cmd_data.wEndFeeTime
            -- 当前时间判定
            local curDate = os.date("*t")
            local curHour = tonumber(curDate.hour)
            if wStart <= curHour and curHour < wEnd then
                PriRoom:getInstance().m_bGoldLimitTimeFree = true
            end

        end
    end

    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_pri_login.SUB_MB_PERSONAL_PARAMETER))
    end

end

-- 私人房费用配置
function PriFrame:_onSubFeeParameter( pData )

    PriRoom:getInstance().m_tabFeeConfigList = {}
    PriRoom:getInstance().m_tabGoldRoomConfigList = {}
    local len = pData:getlen()
    local count = math.floor(len/define_private.LEN_PERSONAL_TABLE_PARAMETER)
    for idx = 1, count do
        local param = ExternalFun.read_netdata( struct_private.tagPersonalTableParameter, pData )
        if 0 == param.cbPlayMode then  --区分积分房卡配置和金币房卡配置
            table.insert(PriRoom:getInstance().m_tabFeeConfigList, param)
        elseif 1 == param.cbPlayMode then
            table.insert(PriRoom:getInstance().m_tabGoldRoomConfigList, param)
        end
    end
    table.sort( PriRoom:getInstance().m_tabFeeConfigList, function(a, b)
        if a.dwDrawCountLimit>0 then
            return a.dwDrawCountLimit < b.dwDrawCountLimit
        else
            return a.dwDrawTimeLimit < b.dwDrawTimeLimit
        end
    end )
end

function PriFrame:onSubFeeParameter( pData )
    self:_onSubFeeParameter(pData)
    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_pri_login.SUB_MB_PERSONAL_FEE_PARAMETER))
    end
end

-- 创建结果
function PriFrame:onSubGameServerResult( pData )
    local cmd_table = ExternalFun.read_netdata( cmd_pri_login.CMD_MB_QueryGameServerResult, pData )
    dump(cmd_table, "CMD_MB_QueryGameServerResult", 6)
    local tips = cmd_table.szErrDescrybe

    if false == cmd_table.bCanCreateRoom then
        if nil ~= self._callBack and type(tips) == "string" then
            self._callBack(LOGINSERVER(-1), tips)
        end
        return true
    end
    if 0 == cmd_table.dwServerID and true == cmd_table.bCanCreateRoom then
        if nil ~= self._callBack and type(tips) == "string" then
            self._callBack(LOGINSERVER(-1), tips)
        end
        return true
    end    
    -- 发送登陆
    PriRoom:getInstance():onLoginRoom(cmd_table.dwServerID)
    return false
end

-- 房卡兑换游戏币结果
function PriFrame:onSubExchangeRoomCardResult( pData )
    local cmd_table = ExternalFun.read_netdata( cmd_pri_login.CMD_GP_ExchangeRoomCardResult, pData )
    dump(cmd_table, "CMD_GP_ExchangeRoomCardResult", 6)
    if true == cmd_table.bSuccessed then
        -- 个人财富
        GlobalUserItem.tabAccountInfo.lUserScore = cmd_table.lCurrScore
        GlobalUserItem.tabAccountInfo.lRoomCard = cmd_table.lRoomCard
        -- 通知更新        
        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
        eventListener.obj = yl.RY_MSG_USERWEALTH
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
    end

    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_pri_login.SUB_GP_EXCHANGE_ROOM_CARD_RESULT), cmd_table.szNotifyContent)
    end
end

-- 分享密码
function PriFrame:onSubSharePasswdResult( pData )
    local cmd_table = ExternalFun.read_netdata( cmd_pri_login.CMD_MB_QueryRoomPwdResult, pData )
    dump(cmd_table, "CMD_MB_QueryRoomPwdResult", 6)
    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_pri_login.SUB_MB_QUERY_ROOM_PASSWORD_RESULT), cmd_table)
    end
end

-- 房卡底分配置
function PriFrame:onSubCellScoreList( pData )
    local cmd_table = ExternalFun.read_netdata( struct_private.tagPersonalCellScore, pData )
    PriRoom:getInstance().m_tabCellScoreList = cmd_table.nCellScore[1]
    dump(PriRoom:getInstance().m_tabCellScoreList, "SUB_MB_PERSONAL_CELL_SCORE", 6)
   
    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_pri_login.SUB_MB_PERSONAL_CELL_SCORE))
    end
end

--额外配置
function PriFrame:onSubRuleResult( pData )
    --先读掉通用配置
    local cmd_table = ExternalFun.read_netdata( struct_private.tagPersonalRuleNormal, pData )
    

     PriRoom:getInstance().m_tabRule = clone(pData)


    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_pri_login.SUB_MB_PERSONAL_RULE_RESULT))
    end


end


--网络消息(长连接)
function PriFrame:onGameSocketEvent( main,sub,pData )    
    local needClose = false
    print("main =="..main..",sub == "..sub)
    if cmd_pri_game.MDM_GR_PERSONAL_TABLE == main then
        if cmd_pri_game.SUB_GR_CREATE_SUCCESS == sub then               -- 创建成功
            needClose = self:onSubCreateSuccess(pData)
        elseif cmd_pri_game.SUB_GR_CREATE_FAILURE == sub then           -- 创建失败
            self:onSubCreateFailure(pData)
        elseif cmd_pri_game.SUB_GR_CANCEL_TABLE == sub then             -- 解散桌子
            self:onSubTableCancel(pData)
        elseif cmd_pri_game.SUB_GR_CANCEL_REQUEST == sub then           -- 请求解散
            self:onSubCancelRequest(pData)
        elseif cmd_pri_game.SUB_GR_REQUEST_REPLY == sub then            -- 请求答复
            self:onSubRequestReply(pData)
        elseif cmd_pri_game.SUB_GR_REQUEST_RESULT == sub then           -- 请求结果
            self:onSubReplyResult(pData)
        elseif cmd_pri_game.SUB_GR_WAIT_OVER_TIME == sub then           -- 超时等待
            self:onSubWaitOverTime(pData)
        elseif cmd_pri_game.SUB_GR_PERSONAL_TABLE_TIP == sub then       -- 提示信息/游戏信息
            self:onSubTableTip(pData)
        elseif cmd_pri_game.SUB_GR_PERSONAL_TABLE_END == sub then       -- 结束
            self:onSubTableEnd(pData)
        elseif cmd_pri_game.SUB_GR_CANCEL_TABLE_RESULT == sub then      -- 私人房解散结果
            self:onSubCancelTableResult(pData)
        elseif cmd_pri_game.SUB_GR_CURRECE_ROOMCARD_AND_BEAN == sub then -- 强制解散桌子后的游戏豆和房卡数量
            self:onSubCancelTableScoreInfo(pData)
        elseif cmd_pri_game.SUB_GR_CHANGE_CHAIR_COUNT == sub then       -- 改变椅子数量
            self:onSubChangeChairCount(pData)
        elseif cmd_pri_game.SUB_GF_PERSONAL_MESSAGE == sub then         -- 私人房消息
            local cmd_table = ExternalFun.read_netdata(cmd_pri_game.Personal_Room_Message, pData)
            -- cbMessageType 0表示坐下type, 1表示断线重连
            if 1 == cmd_table.cbMessageType then
                print("PriFrame 断线重连发送起立")
                if nil ~= self._callBack then
                    self._callBack(GAMESERVER(cmd_pri_game.SUB_GF_PERSONAL_MESSAGE), cmd_table)
                end
                -- 发送起立
                if self._gameFrame:StandUp(1) then
                    needClose = true
                end
            else
                if nil ~= self._callBack then
                    self._callBack(GAMESERVER(-1), cmd_table.szMessage)
                end
                needClose = true
            end
            GlobalUserItem.nCurRoomIndex = -1
        elseif cmd_pri_game.CMD_GR_QUERY_JOIN_INFO_RESULT == sub then   -- 请求信息结果
            self:onSubQueryJoinInfoResult(pData)

        elseif cmd_pri_game.SUB_GR_PLAYING_USER_INFO == sub then   -- 请求信息结果

            local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_PlayingUserInfo, pData)
            if nil ~= self._callBack then
                self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_PLAYING_USER_INFO), cmd_table)
            end

            
        elseif cmd_pri_game.SUB_GR_DEAL_CANCEL_INFO == sub then         -- 解散处理
            self:onSubDealCancelInfo( pData )
        end
    end
    if needClose then
        if self._gameFrame:isSocketServer() then
            self._gameFrame:onCloseSocket()
        end
    end
end

-- 创建成功
function PriFrame:onSubCreateSuccess( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_CreateSuccess, pData)
    -- 更新私人房数据
    PriRoom:getInstance().m_tabPriData.dwDrawTimeLimit = cmd_table.dwDrawTimeLimit
    PriRoom:getInstance().m_tabPriData.dwDrawCountLimit = cmd_table.dwDrawCountLimit
    PriRoom:getInstance().m_tabPriData.dwPersonalRoomID = cmd_table.dwPersonalRoomID
    -- 个人财富
    --GlobalUserItem.tabAccountInfo.dUserBeans = cmd_table.dBeans
    --GlobalUserItem.tabAccountInfo.lRoomCard = cmd_table.lRoomCard
    GlobalUserItem.tabAccountInfo.lDiamond = cmd_table.lDiamond

    -- 通知更新        
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERWEALTH
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

    -- 
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_CREATE_SUCCESS))
    end

    if 0 == PriRoom:getInstance().m_tabRoomOption.cbIsJoinGame or PriRoom:getInstance().m_bPlayModel == 1 then
        print("onSubCreateSuccess ----------------------------------->",  PriRoom:getInstance().m_bPlayModel)
        --assert(false)
        if self._gameFrame:isSocketServer() then
            self._gameFrame:onCloseSocket()
            print("onSubCreateSuccess ==============> have close ")
        end
    else
        PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_SEARCHROOM
    end
end

-- 创建失败
function PriFrame:onSubCreateFailure( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_CreateFailure, pData)
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(-1), cmd_table.szDescribeString)
    end
    -- 代开重置
    PriRoom:getInstance().m_bCreateForOther = false
end

-- 解散桌子
function PriFrame:onSubTableCancel( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_CancelTable, pData)
    dump(cmd_table, "CMD_GR_CancelTable", 6)

    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_CANCEL_TABLE), cmd_table)
    end
end

-- 请求解散
function PriFrame:onSubCancelRequest( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_CancelRequest_Result, pData)
    dump(cmd_table, "请求解散")
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_CANCEL_REQUEST), cmd_table)
    end
end

-- 请求答复
function PriFrame:onSubRequestReply( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_RequestReply, pData)
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_REQUEST_REPLY), cmd_table)
    end
end

-- 请求结果
function PriFrame:onSubReplyResult( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_RequestResult, pData)
    dump(cmd_table, "CMD_GR_RequestResult", 3)
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_REQUEST_RESULT), cmd_table)
    end
end

-- 超时等待
function PriFrame:onSubWaitOverTime( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_WaitOverTime, pData)
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_WAIT_OVER_TIME), cmd_table)
    end
end

-- 提示信息
function PriFrame:onSubTableTip( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_PersonalTableTip, pData)
    PriRoom:getInstance().m_tabPriData = cmd_table
    PriRoom:getInstance().m_tabPriData.bRoomPwd = (cmd_table.bRoomPwd == 1)
    PriRoom:getInstance().m_bIsMyRoomOwner = (cmd_table.dwTableOwnerUserID == GlobalUserItem.tabAccountInfo.dwUserID)
    PriRoom:getInstance().cbIsJoinGame = PriRoom:getInstance().m_tabPriData.cbIsJoinGame
    GlobalUserItem.dwCurRoomGroupID = cmd_table.dwGroupID
    ClubFrame:getInstance().m_nCurrentClub = cmd_table.dwGroupID

    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_PERSONAL_TABLE_TIP), cmd_table)
    end
    --dump(PriRoom:getInstance().m_tabPriData, "PriRoom:getInstance().m_tabPriData", 6)
end

-- 结束消息
function PriFrame:onSubTableEnd( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_PersonalTableEnd, pData)
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_PERSONAL_TABLE_END), cmd_table, pData)
    end
    print("onSubTableEnd==================================")
    -- 结束主动起立
    --self._gameFrame:StandUp(1)
end

-- 私人房解散结果
function PriFrame:onSubCancelTableResult( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_DissumeTable, pData)
    print("onSubCancelTableResult==================================")
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_CANCEL_TABLE_RESULT), cmd_table)
    end

    if self._gameFrame:isSocketServer() then
        self._gameFrame:onCloseSocket()
    end
end

-- 解散后财富信息
function PriFrame:onSubCancelTableScoreInfo( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_CurrenceRoomCardAndBeans, pData)
    -- 个人财富
    --GlobalUserItem.tabAccountInfo.dUserBeans = cmd_table.dbBeans
    --GlobalUserItem.tabAccountInfo.lRoomCard = cmd_table.lRoomCard
    GlobalUserItem.tabAccountInfo.lDiamond = cmd_table.lDiamond
    -- 通知更新        
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERWEALTH
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_CURRECE_ROOMCARD_AND_BEAN), cmd_table)
    end
end

-- 改变椅子数量
function PriFrame:onSubChangeChairCount( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_ChangeChairCount, pData)
    if nil ~= cmd_table.dwChairCount then
        self._gameFrame._wChairCount = cmd_table.dwChairCount
    end
end

-- 请求加入信息结果
function PriFrame:onSubQueryJoinInfoResult( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_QueryJoinInfoResult, pData)
    dump( cmd_table, "信息结果", 6)
    
    --print("================================>,"..pData:getlen())
    local bPasswd = (cmd_table.bRoomPwd == 1)
    if cmd_table.cbPayMode == define_private.tabPayMode.AA_PAY then
        local param = {}
        param.dwRoomId = self._roomId
        param.dwDrawCountLimit = cmd_table.dwDrawCountLimit
        param.lDiamondFee = cmd_table.lDiamondFee
        param.cbPayMode = cmd_table.cbPayMode
        param.bRoomPwd = bPasswd
        param.dwRommerID = cmd_table.dwRommerID

        param.buffer = pData -- 如果没有立即使用,需要retain增加引用
        dump(param,"==========================")
        -- aa
        PriRoom:getInstance():popAAPayLayer(param)
    else
        -- 判断是否密码, 且非房主
        if bPasswd and GlobalUserItem.tabAccountInfo.dwUserID ~= cmd_table.dwRommerID then
            PriRoom:getInstance():passwdInput()
        else
            PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_SEARCHROOM
            PriRoom:getInstance():showPopWait()
            if GlobalUserItem.bLookOn then
                self:sendLookOnPrivateGame()
            else
                self:sendEnterPrivateGame()
            end

        end

    end
end

-- 解散处理
function PriFrame:onSubDealCancelInfo( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_pri_game.CMD_GR_DealCancelInfo, pData)
    dump( cmd_table, "解散处理", 6)
    if nil ~= self._callBack then
        self._callBack(GAMESERVER(cmd_pri_game.SUB_GR_DEAL_CANCEL_INFO), cmd_table)
    end
end

return PriFrame