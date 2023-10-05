--
-- Author: zhong
-- Date: 2017-05-12 08:56:38
--
-- 回放网络处理
local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local VideoFrame = class("VideoFrame",BaseFrame)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local cmd_video = appdf.req(GameVideo.MODULE.VIDEOHEADER .. "CMD_GameVideo")
local define_video = appdf.req(GameVideo.MODULE.VIDEOHEADER .. "Define_GameVideo")
local struct_video = appdf.req(GameVideo.MODULE.VIDEOHEADER .. "Struct_GameVideo")

-- 登陆服务器CMD
local cmd_video_login = cmd_video.login
-- 游戏服务器CMD
local cmd_video_game = cmd_video.game

VideoFrame.OP_LOADVIDEOLIST         = cmd_video_login.SUB_MB_QUERY_VIDEO_INFO           -- 视频列表
VideoFrame.OP_GETVIDEODETAIL        = cmd_video_login.SUB_MB_QUERY_VIDEO_DETAILS        -- 视频详情
VideoFrame.OP_QUERYPLAYBACKCODE     = cmd_video_login.SUB_MB_QUERY_PLAYBACK_CODE        -- 申请回放码(普通)
VideoFrame.OP_QUERYPLAYBACKCODE_YZ  = cmd_video_login.SUB_MB_QUERY_PLAYBACK_CODE_YZ     -- 申请回放码(约战)
VideoFrame.OP_GETVIDEODETAILBYROOMID = cmd_video_login.SUB_MB_QUERY_VIDEO_DETAILS_BY_ROOMID -- 视频详情(通过约战房ID获取)
VideoFrame.OP_LOADVIDEOLISTEX        = cmd_video_login.SUB_MB_QUERY_VIDEO_INFOEX    -- 视频详情(金币回放)

local function LOGINSERVER(code)
    return { m = cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO, s = code }
end

local function GAMESERVER(code)
    --return { m = cmd_pri_game.MDM_GR_PERSONAL_TABLE, s = code }
end

function VideoFrame:ctor(view,callbcak)
    self._oprateCode = 0
    VideoFrame.super.ctor(self,view,callbcak)
end

-- 视频列表查询
-- @param[nStartIdx] 起始索引
-- @param[nEndIdx] 结束索引
-- @param[dwPlayback] 是否回放码查询(0 非回放码)
-- @param[nQueryType] 查询类型 0:约战 1:普通
-- @param[dwRoomID] 约战房ID
function VideoFrame:onLoadVideoList( nStartIdx, nEndIdx, dwPlayback, nQueryType, dwRoomID )
    -- 操作记录
    self._oprateCode = VideoFrame.OP_LOADVIDEOLIST

    self._nStartIdx = nStartIdx
    self._nEndIdx = nEndIdx
    self._dwPlayback = dwPlayback
    self._nQueryType = nQueryType
    self._dwRoomID = dwRoomID
    self:onCloseSocket()
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"建立连接失败！")
    end
end

function VideoFrame:onLoadVideoListEX( nStartIdx, nEndIdx, dwPlayback, nQueryType, dwRoomID, personalGuid)
    -- 操作记录
    self._oprateCode = VideoFrame.OP_LOADVIDEOLISTEX

    self._nStartIdx = nStartIdx
    self._nEndIdx = nEndIdx
    self._dwPlayback = dwPlayback
    self._nQueryType = nQueryType
    self._dwRoomID = dwRoomID
    self._szGUID = personalGuid
    
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"建立连接失败！")
    end
end

-- 视频详情查询
function VideoFrame:onQueryVideoDetail( szVideoGUID )
    -- 操作记录
    self._oprateCode = VideoFrame.OP_GETVIDEODETAIL

    self._szVideoGUID = szVideoGUID
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"建立连接失败！")
    end
end

-- 视频详情查询(通过约战房ID)
function VideoFrame:onQueryVideoDetailByRoomID( dwRoomID )
    -- 操作记录
    self._oprateCode = VideoFrame.OP_GETVIDEODETAILBYROOMID

    self._dwRoomID = dwRoomID
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"建立连接失败！")
    end
end

-- 回放码申请(约战)
function VideoFrame:onRequestPlayBackCode_YZ( szVideoGUID )
    -- 操作记录
    self._oprateCode = VideoFrame.OP_QUERYPLAYBACKCODE_YZ

    self._szVideoGUID = szVideoGUID
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"建立连接失败！")
    end
end

-- 回放码申请(普通)
function VideoFrame:onRequestPlayBackCode( szVideoNumber )
    -- 操作记录
    self._oprateCode = VideoFrame.OP_QUERYPLAYBACKCODE

    self._szVideoNumber = szVideoNumber
    if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"建立连接失败！")
    end
end

--连接结果
function VideoFrame:onConnectCompeleted()
    if VideoFrame.OP_LOADVIDEOLIST == self._oprateCode then
        self:sendLoadVideoList()
    elseif VideoFrame.OP_GETVIDEODETAIL == self._oprateCode then
        self:sendGetVideoDetail()
    elseif VideoFrame.OP_QUERYPLAYBACKCODE == self._oprateCode then
        self:sendQueryPlayBackCode()
    elseif VideoFrame.OP_QUERYPLAYBACKCODE_YZ == self._oprateCode then
        self:sendQueryPlayBackCode_YZ()
    elseif VideoFrame.OP_GETVIDEODETAILBYROOMID == self._oprateCode then
        self:sendGetVideoDetailByRoomID()
    elseif VideoFrame.OP_LOADVIDEOLISTEX == self._oprateCode then
        self:sendLoadVideoListEX()
    end
end

-- 发送加载视频列表
function VideoFrame:sendLoadVideoList()
    print("VideoFrame:sendLoadVideoList ==> ")
    local buffer = ExternalFun.create_netdata(cmd_video_login.CMD_MB_C_QueryVideoInfo)
    buffer:setcmdinfo(cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO, cmd_video_login.SUB_MB_QUERY_VIDEO_INFO)
    buffer:pushint(self._nQueryType)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(self._dwPlayback)
    --buffer:pushstring(self._szRoomID, define_video.LEN_ROOMID)
    buffer:pushdword(self._dwRoomID)
    buffer:pushword(self._nStartIdx)
    buffer:pushword(self._nEndIdx)

    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送加载视频列表失败！")
    end
end

-- 发送加载视频列表（回放用）
function VideoFrame:sendLoadVideoListEX()
    print("VideoFrame:sendLoadVideoListEX ==> ")
    local buffer = ExternalFun.create_netdata(cmd_video_login.CMD_MB_C_QueryVideoInfoEX)
    buffer:setcmdinfo(cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO, cmd_video_login.SUB_MB_QUERY_VIDEO_INFOEX)
    buffer:pushint(self._nQueryType)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushdword(self._dwPlayback)
    buffer:pushstring(self._szGUID, 32)
    buffer:pushdword(self._dwRoomID)
    buffer:pushword(self._nStartIdx)
    buffer:pushword(self._nEndIdx)

    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送加载视频列表失败！")
    end
end


-- 发送视频详情查询
function VideoFrame:sendGetVideoDetail()
    print("VideoFrame:sendGetVideoDetail")
    local buffer = ExternalFun.create_netdata(cmd_video_login.CMD_MB_C_QueryVideoDetails)
    buffer:setcmdinfo(cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO, cmd_video_login.SUB_MB_QUERY_VIDEO_DETAILS)
    buffer:pushstring(self._szVideoGUID, define_video.PERSONAL_ROOM_GUID)

    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送视频详情查询失败！")
    end
end

-- 发送视频详情查询
function VideoFrame:sendGetVideoDetailByRoomID()
    print("VideoFrame:sendGetVideoDetailByRoomID")
    local buffer = ExternalFun.create_netdata(cmd_video_login.CMD_MB_C_QueryVideoDetailsByRoomID)
    buffer:setcmdinfo(cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO, cmd_video_login.SUB_MB_QUERY_VIDEO_DETAILS_BY_ROOMID)
    --buffer:pushstring(self._szRoomID, define_video.LEN_ROOMID) -- 20170810 调整
    buffer:pushdword(self._dwRoomID)

    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送视频详情查询失败！")
    end
end

-- 发送回放码查询(约战)
function VideoFrame:sendQueryPlayBackCode_YZ()
    print("VideoFrame:sendQueryPlayBackCode_YZ ==> ", self._szVideoGUID)
    local buffer = ExternalFun.create_netdata(cmd_video_login.CMD_MB_C_QueryPlayBackCode_YZ)
    buffer:setcmdinfo(cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO, cmd_video_login.SUB_MB_QUERY_PLAYBACK_CODE_YZ)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(self._szVideoGUID, define_video.PERSONAL_ROOM_GUID)

    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送回放码查询失败！")
    end
end

-- 发送回放码查询(普通)
function VideoFrame:sendQueryPlayBackCode()
    print("VideoFrame:sendQueryPlayBackCode ==> ", self._szVideoNumber)
    local buffer = ExternalFun.create_netdata(cmd_video_login.CMD_MB_C_QueryPlayBackCode)
    buffer:setcmdinfo(cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO, cmd_video_login.SUB_MB_QUERY_PLAYBACK_CODE)
    buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID)
    buffer:pushstring(self._szVideoNumber, define_video.LEN_VIDEO_NUMBER)

    if not self:sendSocketData(buffer) and nil ~= self._callBack then
        self._callBack(LOGINSERVER(-1),"发送回放码查询失败！")
    end
end

--网络信息(短连接)
function VideoFrame:onSocketEvent( main,sub,pData )
    print("VideoFrame:onSocketEvent ==> " .. main .. "##" .. sub)
    local needClose = true
    if cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO == main then
        if cmd_video_login.SUB_MB_QUERY_VIDEO_INFO_RESULT == sub then               -- 视频列表开始
            needClose = false
            self:onGetVideoInfoResult( pData )
        elseif cmd_video_login.SUB_MB_QUERY_VIDEO_INFO_END == sub then              -- 视频列表结束
            needClose = self:onGetVideoInfoResultEnd( pData )
            --self:onCloseSocket()
        elseif cmd_video_login.SUB_MB_QUERY_VIDEO_DETAILS_RESULT == sub then        -- 视频详情
            needClose = false
            self:onGetVideoDetailResult( pData )
        elseif cmd_video_login.SUB_MB_QUERY_VIDEO_DETAILS_END == sub then           -- 视频详情结束
            self:onGetVideoDetailResultEnd( pData )
        elseif cmd_video_login.SUB_MB_QUERY_PLAYBACK_CODE_RESULT == sub then        -- 视频回放码(普通)
            self:onGetVideoPlayBackCodeReult( pData )
        elseif cmd_video_login.SUB_MB_QUERY_PLAYBACK_YZ_CODE_RESULT == sub then     -- 视频回放码(约战)
            self:onGetVideoPlayBackCodeReult_YZ( pData )
        end
    end
    if needClose then
        self:onCloseSocket()
    end
end

function VideoFrame:onGetVideoInfoResult( pData )
    -- 获取数量
    local cmd_table = ExternalFun.read_netdata(cmd_video_login.CMD_MB_S_QueryListCount, pData)

    -- 读取列表
    for i = 1, cmd_table.wListCount do
        local listitem = ExternalFun.read_netdata(cmd_video_login.CMD_MB_S_QueryVideoInfoResult, pData)
        --dump(listitem, "aaaa", 10)
        -- 时间戳
        local tt = listitem.CreateDate
        listitem.lCreateTimestamp = os.time({day=tt.wDay, month=tt.wMonth, year=tt.wYear, hour=tt.wHour, min=tt.wMinute, sec=tt.wSecond}) or 0
        GameVideo:getInstance():addVideoInfoToList(listitem)
 
    end

end

function VideoFrame:onGetVideoInfoResultEnd( pData )
    if nil ~= self._callBack then
        return self._callBack(LOGINSERVER(cmd_video_login.SUB_MB_QUERY_VIDEO_INFO_END))
    end
    return true
end

function VideoFrame:onGetVideoDetailResult( pData )
    -- 获取数量
    local cmd_table = ExternalFun.read_netdata(cmd_video_login.CMD_MB_S_QueryListCount, pData)

    local guid = nil
    -- 读取列表
    for i = 1, cmd_table.wListCount do
        local listitem = ExternalFun.read_netdata(cmd_video_login.CMD_MB_S_QueryVideoDetailsResult, pData)
        -- 数据校验
        if nil == listitem.szVideoNumber or "" == listitem.szVideoNumber then
            print("视频唯一ID异常")
            break
        end
        guid = listitem.szPersonalGUID
        local round = listitem.dwGamesNum
        local uid = listitem.dwUserID
        if nil == GameVideo:getInstance().m_tabVideoDetail[guid] then
            GameVideo:getInstance().m_tabVideoDetail[guid] = {}

            -- 局信息
            GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo = {}
            -- 一栏排列规则(按userid的插入顺序排列)
            GameVideo:getInstance().m_tabVideoDetail[guid].tabArrangeRule = {}
            -- 总信息
            GameVideo:getInstance().m_tabVideoDetail[guid].tabTotalInfo = {}
        end
        -- 局信息
        if nil == GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo[round] then
            GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo[round] = {}
            GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo[round].nRound = round
            GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo[round].szVideoNumber = listitem.szVideoNumber
            GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo[round].szPersonalGUID = listitem.szPersonalGUID
            GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo[round].list = {}
        end
        -- 总分信息
        if nil == GameVideo:getInstance().m_tabVideoDetail[guid].tabTotalInfo[uid] then
            GameVideo:getInstance().m_tabVideoDetail[guid].tabTotalInfo[uid] = {}
            GameVideo:getInstance().m_tabVideoDetail[guid].tabTotalInfo[uid].lRoundTotalScore = 0
            GameVideo:getInstance().m_tabVideoDetail[guid].tabTotalInfo[uid].szNickName = listitem.szNickName

            -- 排列规则
            table.insert(GameVideo:getInstance().m_tabVideoDetail[guid].tabArrangeRule, uid)
        end

        local tmpTotal = GameVideo:getInstance().m_tabVideoDetail[guid].tabTotalInfo[uid].lRoundTotalScore
        tmpTotal = tmpTotal + listitem.lScore
        GameVideo:getInstance().m_tabVideoDetail[guid].tabTotalInfo[uid].lRoundTotalScore = tmpTotal
        
        GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo[round].list[uid] = listitem
    end

    -- 处理数据
    if nil ~= guid then
        local tabDetailInfo = GameVideo:getInstance().m_tabVideoDetail[guid]
        if nil ~= tabDetailInfo then
            local tmpVecRound = {}
            GameVideo:getInstance().m_tabVideoDetail[guid].tabVecRoundInfo = {}
            local roundinfo = GameVideo:getInstance().m_tabVideoDetail[guid].tabRoundInfo
            for k,v in pairs(roundinfo) do
                table.insert(tmpVecRound, v)
            end

            -- 排序
            table.sort(tmpVecRound, function(a,b)
                return a.nRound > b.nRound
            end)
            GameVideo:getInstance().m_tabVideoDetail[guid].tabVecRoundInfo = tmpVecRound
        end
    end
end

function VideoFrame:onGetVideoDetailResultEnd( pData )
    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_video_login.SUB_MB_QUERY_VIDEO_DETAILS_END))
    end
end

function VideoFrame:onGetVideoPlayBackCodeReult_YZ( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_video_login.CMD_MB_S_QueryPlayBackCode_YZ_Result, pData)
    GameVideo:getInstance().m_currentBackCode = cmd_table.dwPlayBackCode
    dump(cmd_table, "CMD_MB_S_QueryPlayBackCode_Result", 6)
    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_video_login.SUB_MB_QUERY_PLAYBACK_YZ_CODE_RESULT), cmd_table)
    end
end

function VideoFrame:onGetVideoPlayBackCodeReult( pData )
    local cmd_table = ExternalFun.read_netdata(cmd_video_login.CMD_MB_S_QueryPlayBackCode_Result, pData)
    dump(cmd_table, "CMD_MB_S_QueryPlayBackCode_Result", 6)
    if nil ~= self._callBack then
        self._callBack(LOGINSERVER(cmd_video_login.SUB_MB_QUERY_PLAYBACK_CODE_RESULT), cmd_table)
    end
end

return VideoFrame