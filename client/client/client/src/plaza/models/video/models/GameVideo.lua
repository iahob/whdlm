--
-- Author: zhong
-- Date: 2017-03-20 11:00:41
--
-- 录像回放
GameVideo = GameVideo or class("GameVideo")
local gamevideo_define = appdf.req(appdf.CLIENT_SRC .. "plaza.models.video.header.Define_GameVideo")
local cmd_video = appdf.req(appdf.CLIENT_SRC .. "plaza.models.video.header.CMD_GameVideo")
local QueryDialog = appdf.req(appdf.BASE_SRC.."app.views.layer.other.QueryDialog")
local targetPlatform = cc.Application:getInstance():getTargetPlatform()
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

-- 回放模块
local MODULE = gamevideo_define.tabModule
GameVideo.MODULE = MODULE
-- 私人房界面tag
local LAYTAG = gamevideo_define.tabLayTag
GameVideo.LAYTAG = LAYTAG
-- 错误代码
local ERRORCODE = gamevideo_define.tabErrorCode
GameVideo.ERRORCODE = ERRORCODE
-- define
GameVideo.DEFINE = gamevideo_define

-- 登陆服务器CMD
local cmd_video_login = cmd_video.login
-- 游戏服务器CMD
local cmd_video_game = cmd_video.game

local VideoFrame = appdf.req(MODULE.PLAZAMODULE .."VideoFrame")

GameVideo.DEFAULT_PAGE = 2
local targetPlatform = cc.Application:getInstance():getTargetPlatform()
function GameVideo:ctor()
    -- 回放文件存储
    self.m_szVideoFilePath = device.writablePath .. "/videofile/"
    if false == cc.FileUtils:getInstance():isDirectoryExist(self.m_szVideoFilePath) then
        cc.FileUtils:getInstance():createDirectory(self.m_szVideoFilePath)
    end

    --网络回调
    local videoCallBack = function(command, message, dataBuffer, notShow)
        if type(command) == "table" then
            if command.m == cmd_video_login.MDM_MB_VIDEO_PLAY_BACK_INFO then
                return self:onVideoLoginServerMessage(command.s, message, dataBuffer, notShow)
            elseif command.m == cmd_video_game.MDM_GR_PERSONAL_TABLE then
                return self:onVideoGameServerMessage(command.s, message, dataBuffer, notShow)
            end
        else
            self:popMessage(message, notShow)
            if -1 == command then
                self:dismissPopWait()
                self:setViewFrame(nil)
            end
        end
    end
    self._videoFrame = VideoFrame:create(self, videoCallBack)

    self:reSet()
end

GameVideo._instance = nil
function GameVideo:getInstance()
    if nil == GameVideo._instance then
        print(" GameVideo new instance ")
        GameVideo._instance = GameVideo:create()
    end
    return GameVideo._instance
end

function GameVideo:reSet()
    -- 回放索引(步骤)
    self.m_nMessageIdx = 1
    -- 消息总数
    self.m_nMessageCount = 0
    -- 回放消息队列
    self.m_tabVideoMessageQueue = {}
    -- 回放步骤数据(存储每一个回放消息, 界面需要的数据)
    self.m_tabStepVideoMessage = {}
    -- 登陆玩家id
    self.m_dwLoginUserId = 0
    -- 登陆玩家昵称
    self.m_szLoginUserNick = ""

    -- 列表索引页(1页为查询视频用, 默认2开始)
    self.m_nPageIndex = GameVideo.DEFAULT_PAGE
    -- 回放列表(map)
    self.m_tabVideoList = {}
    -- 回放列表(vector)
    self.m_tabVecVideoList = {}
    -- 视频详情
    self.m_tabVideoDetail = {}
    -- 详情ID(以约战roomid为key)
    self.m_tabGUID = {}
    -- 查询类型
    self.m_nQueryType = GameVideo.DEFINE.QUERY_PRIVATE_SCORE_VIDEO

    -- 回放信息(以回放码为key, 存储回放数据)
    self.m_tabVideoInfo = {}

    --当前查询的回放码
    self.m_currentBackCode = 0
end

function GameVideo:showPopWait()
    if nil ~= self._scene then
        self._scene:showPopWait()
    end
end

function GameVideo:dismissPopWait()
    if nil ~= self._scene then
        self._scene:dismissPopWait()
    end
end

-- 切换查询类型
function GameVideo:changeQueryType( nQueryType )
    self.m_nQueryType = nQueryType
    self.m_tabVideoList = {}
    self.m_tabVecVideoList = {}
    self.m_nPageIndex = GameVideo.DEFAULT_PAGE
end

-- 查询分页信息
function GameVideo:queryPageVideoInfo( dwPlayBackCode, nQueryType, nRoomID)
    local pageVideoCount = 10
    local nStartIdx = (self.m_nPageIndex - 2) * pageVideoCount
    local nEndIdx = nStartIdx + pageVideoCount
    print("GameVideo:queryPageVideoInfo nStart nEnd ==> ", nStartIdx, nEndIdx)
    local roomid = nRoomID or ""
    print("GameVideo:queryPageVideoInf ==================================>", roomid)
    self:getNetFrame():onLoadVideoList(nStartIdx, nEndIdx, dwPlayBackCode, nQueryType, roomid)
end

function GameVideo:queryPageVideoInfoEX( dwPlayBackCode, nQueryType, nGUID)
    local pageVideoCount = 10
    local nStartIdx = (self.m_nPageIndex - 2) * pageVideoCount
    local nEndIdx = nStartIdx + pageVideoCount
    print("GameVideo:queryPageVideoInfo nStart nEnd ==> ", nStartIdx, nEndIdx)
    local personalGUID = nGUID or ""
    print("GameVideo:queryPageVideoInfoEX ==================================>", personalGUID)
    self:getNetFrame():onLoadVideoListEX(nStartIdx, nEndIdx, dwPlayBackCode, nQueryType, 0, personalGUID)
end

function GameVideo:addVideoInfoToList( listinfo )
    print("addVideoInfoToList############################", self.m_nQueryType)
    if type(listinfo) ~= "table" then
        dump(listinfo, "GameVideo:addVideoInfoToList 视频信息非法", 6)
        return
    end
    if nil == self.m_tabVideoList[self.m_nPageIndex] then
        self.m_tabVideoList[self.m_nPageIndex] = {}
    end
    if GameVideo.DEFINE.QUERY_NORMAL_VIDEO == self.m_nQueryType then        -- 普通场回放记录
        if nil == listinfo.szVideoNumber then
            dump(listinfo, "GameVideo:addVideoInfoToList 视频信息非法", 6)
            return
        end
        if nil == self.m_tabVideoList[self.m_nPageIndex][listinfo.szVideoNumber] then
            self.m_tabVideoList[self.m_nPageIndex][listinfo.szVideoNumber] = {}
        end
        table.insert(self.m_tabVideoList[self.m_nPageIndex][listinfo.szVideoNumber], listinfo)
    elseif GameVideo.DEFINE.QUERY_PRIVATE_VIDEO == self.m_nQueryType or GameVideo.DEFINE.QUERY_PRIVATE_SCORE_VIDEO == self.m_nQueryType 
    or GameVideo.DEFINE.QUERY_PRIVATE_GOLD_VIDEO == self.m_nQueryType then   -- 约战场回放记录
        if 0 == listinfo.dwPersonalRoomID then
            dump(listinfo, "GameVideo:addVideoInfoToList 视频信息非法", 6)
            return
        end
        if nil == self.m_tabVideoList[self.m_nPageIndex][listinfo.dwPersonalRoomID] then
            self.m_tabVideoList[self.m_nPageIndex][listinfo.dwPersonalRoomID] = {}
        end
        table.insert(self.m_tabVideoList[self.m_nPageIndex][listinfo.dwPersonalRoomID], listinfo)
    else
        print(" 查询类型未定义 ==> ", self.m_nQueryType)
        self.m_tabVideoList = {}
    end
end

function GameVideo:getVideoListInfo( pageIdx )
    pageIdx = pageIdx or self.m_nPageIndex
    print("getVideoListInfo=================================>pageIdx", pageIdx)
    local cache = self.m_tabVecVideoList[pageIdx]
    if nil ~= cache and 0 ~= #cache then
        return cache
    end
    dump(self.m_tabVideoList, "getVideoListInfo=================================>", 7)
    local tmp = self.m_tabVideoList[pageIdx] or {}
    local list = {}
    for k,v in pairs(tmp) do
        table.insert(list, v)
    end
    table.sort(list, function(a, b)
        local aInfo = a[1] or {}
        local bInfo = b[1] or {}

        local aTimestamp = aInfo.lCreateTimestamp or 0
        local bTimestamp = bInfo.lCreateTimestamp or 0

        -- tableview 是升序取数据, 因此这里按倒序排序
        return aTimestamp > bTimestamp
    end)
    self.m_tabVecVideoList[pageIdx] = list
    return list
end

-- 唯一id
function GameVideo:getVideoDetailInfo( szVideoGUID )
    if nil == szVideoGUID or nil == self.m_tabVideoDetail[szVideoGUID] then
        return 
        {
            tabRoundInfo = {},
            tabArrangeRule = {},
            tabTotalInfo = {},
            tabVecRoundInfo = {}
        }
    end
    return self.m_tabVideoDetail[szVideoGUID]
end

-- 约战房id
function GameVideo:getVideoDetailInfoByRoomID( szRoomID )
    local szVideoGUID = self.m_tabGUID[szRoomID]
    return self:getVideoDetailInfo(szVideoGUID)
end

-- 添加回放信息
function GameVideo:addVideoNetMessage( szVideoNumber, tabInfo )
    if type(tabInfo) ~= "table" or nil == szVideoNumber then
        print("GameVideo addVideoInfo 参数非法, szVideoNumber ==> ", szVideoNumber)
        dump(tabInfo, "tabInfo", 6)
        return
    end
    self.m_tabVideoInfo[szVideoNumber] = tabInfo
end

-- 获取回放信息
-- @param[szVideoNumber] 回放编号
-- @param[bPrivate] 是否约战回放
function GameVideo:getVideoNetMessage( szVideoNumber, bPrivate, callback )
    if type(callback) ~= "function" then
        return
    end
    if nil == szVideoNumber then
        callback(szVideoNumber, bPrivate, {})
    end
    if nil ~= self.m_tabVideoInfo[szVideoNumber] then
        callback(szVideoNumber, bPrivate, self.m_tabVideoInfo[szVideoNumber])
    else
        local replay, videogame = self:getReplayScriptFile()
        if nil ~= videogame then
            if nil ~= replay then
                local lay = replay
                local function _readVideo( szVideoNumber, szVideoFile, bPrivate )
                    local round_table = {}
                    -- 读取对应视频信息
                    local readbuffer = readGameVideoFile(szVideoFile)
                    if nil ~= readbuffer then
                        local bEmpty = true
                        local cmd_table = lay:onReadMessage(readbuffer)
                        while nil ~= cmd_table do
                            -- 记录局数
                            cmd_table.nGameRound = 1
                            -- 记录是否约战房
                            cmd_table.bIsPrivateRoom = bPrivate

                            table.insert(round_table, cmd_table)
                            cmd_table = lay:onReadMessage(readbuffer)
                        end
                    else
                        print("GameVideo 回放资源读取异常")
                        showToast(self._scene, "回放资源加载异常, CODE" .. GameVideo.ERRORCODE.CODE_RES_MISSING, 2)
                        return
                    end
                    self:addVideoNetMessage(szVideoNumber, round_table)
                    return round_table
                end
                local videofile = self.m_szVideoFilePath .. szVideoNumber .. ".ryvideo"
                if cc.FileUtils:getInstance():isFileExist(videofile) then
                    callback(szVideoNumber, bPrivate, _readVideo(szVideoNumber, videofile, bPrivate))
                else
                    -- 下载资源
                    self:showPopWait()
                    local nRetryCount = 3
                    local function _downFun( bOk )
                        --列表完成
                        if bOk then
                            print(" downloadRecordFile finish ")
                            self:dismissPopWait()
                            callback(szVideoNumber, bPrivate, _readVideo(szVideoNumber, videofile, bPrivate))
                            return 
                        end

                        local url = yl.HTTP_URL_DOWNLOAD .. "/Mobile/MoblieVideoInfo.aspx?vNumber=" .. szVideoNumber .. "&" .. GlobalUserItem:urlUserIdSignParam()
                        local path = self.m_szVideoFilePath
                        local vfile = szVideoNumber .. ".ryvideo"
                        print("from ==> " .. url .. " download " .. vfile)
                        --调用C++下载
                        downFileAsync(url, vfile, path, function(main,sub)
                            --下载回调
                            if main == appdf.DOWN_PRO_INFO then --进度信息

                            elseif main == appdf.DOWN_COMPELETED then --下载完毕
                                _downFun( true )
                            else
                                -- 重试
                                if sub == 28 and nRetryCount > 0 then
                                    nRetryCount = nRetryCount - 1
                                    _downFun( false )
                                else
                                    self:dismissPopWait()
                                    -- 下载失败
                                    if nil ~= self._scene then
                                        showToast(self._scene, "视频信息加载失败, 请重试!", 2)
                                    end
                                    --失败信息
                                    print("视频下载失败信息 main, sub ", main, sub)
                                end
                            end
                        end)
                    end
                    _downFun( false )
                end
                return
            end
        end
        callback(szVideoNumber, bPrivate, {})
    end
end

function GameVideo:onEnterPlaza( scene, gameFrame )
    self._scene = scene
    self._gameFrame = gameFrame
end

function GameVideo:onExitPlaza()
    self._gameFrame = nil
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
    self:reSet()
end

-- 界面切换
function GameVideo:getTagLayer(tag, param, scene)
    if LAYTAG.LAYER_VIDEOCONTROL == tag then
        -- 回放控制
        local lay = appdf.req(appdf.PLAZA_VIEW_SRC .. "video.VideoPlayControlLayer"):create(param, scene)
        return lay
    end
    return nil
end

-- 下载录像文件列表
-- @param[vecRecInfo] 视频列表
-- @param[userList] 玩家列表
-- @param[tabGameInfo] 游戏信息
function GameVideo:downloadRecordFileList( vecRecInfo, userList, tabGameInfo )
    if nil == vecRecInfo or 0 == #vecRecInfo then
        if nil ~= self._scene then
            showToast(self._scene, "视频信息为空!", 2)
        end
        return
    end
    local nCount = #vecRecInfo
    self:showPopWait()

    local nRetryCount = 3
    local function _downFun(nIdx)
        --列表完成
        if nIdx == (nCount + 1) then
            print(" downloadRecordFileList finish ")
            -- 游戏版本检查
            if nil ~= self._scene and nil ~= self._scene.getApp then
                if cc.PLATFORM_OS_WINDOWS == targetPlatform and yl.DISABLE_WIN_UPDATE then
                    print("win32 跳过更新")
                else
                    local gamekind = tonumber(tabGameInfo.nKindID)
                    local videogame = self._scene:getGameInfo(gamekind)
                    local app = self._scene:getApp()
                    local version = tonumber(app:getVersionMgr():getResVersion(gamekind))
                    if not version or videogame._ServerResVersion > version then
                        if false == self._scene:singleGameUpdate(videogame._KindID, function()
                                print("GameVideo:downloadRecordFileList update over")
                                self._scene:stopAllActions()
                                self._scene:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function()
                                    self:readVideoFileList( vecRecInfo, userList, tabGameInfo )
                                end)))
                            end) then
                            if nil ~= videogame then
                                local gamename = videogame._GameName or ""
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
                self:dismissPopWait()
                return
            end

            self:readVideoFileList( vecRecInfo, userList, tabGameInfo )
            return 
        end

        local recInfo = vecRecInfo[nIdx]
        local url = yl.HTTP_URL_DOWNLOAD .. "/Mobile/MoblieVideoInfo.aspx?vNumber=" .. recInfo.szVideoNumber .. "&" .. GlobalUserItem:urlUserIdSignParam()
        local path = self.m_szVideoFilePath
        local vfile = recInfo.szVideoNumber .. ".ryvideo"
        -- 移除本地文件
        local vfilepath = path .. vfile
        if cc.FileUtils:getInstance():isFileExist(vfilepath) then
            print("移除本地文件 ", vfilepath)
            cc.FileUtils:getInstance():removeFile(vfilepath)
        end
        print("from ==> " .. url .. " download " .. vfile)
        --调用C++下载
        downFileAsync(url, vfile, path, function(main,sub)
            --下载回调
            if main == appdf.DOWN_PRO_INFO then --进度信息

            elseif main == appdf.DOWN_COMPELETED then --下载完毕
                _downFun(nIdx + 1)
            else
                -- 重试
                if sub == 28 and nRetryCount > 0 then
                    nRetryCount = nRetryCount - 1
                    _downFun(nIdx)
                else
                    self:dismissPopWait()
                    -- 下载失败
                    if nil ~= self._scene then
                        showToast(self._scene, "视频信息加载失败, 请重试!", 2)
                    end
                    --失败信息
                    print("视频下载失败信息 main, sub ", main, sub)
                end
            end
        end)
    end
    _downFun(1)
end

-- 读取回放列表
--[[
tabParam = 
{
    vecFilePath,-- 回放文件路径列表
    nKindId,    -- 回放游戏标识
}
]]
function GameVideo:readVideoFileList( vecRecInfo, userList, tabGameInfo )
    self:reSet()
    local gamekind = tonumber(tabGameInfo.nKindID)
    if nil == gamekind or 0 == gamekind then
        if nil ~= self._scene then
            showToast(self._scene, "视频信息为空!", 2)
        end
        self:dismissPopWait() 
        return
    end
    GlobalUserItem.nCurGameKind = gamekind

    local replay, videogame = self:getReplayScriptFile()
    if nil ~= videogame then
        if nil ~= replay then
            local lay = replay

            local nCount = #vecRecInfo
            -- 合并视频数据(视频数据倒序读取)
            for i = nCount, 1, -1 do
                local vinfo = vecRecInfo[i]
                print("read round " , vinfo.nRound)
                local round_table = {}

                local videofile = self.m_szVideoFilePath .. vinfo.szVideoNumber .. ".ryvideo"
                local readbuffer = readGameVideoFile(videofile)
                if nil ~= readbuffer then
                    local bEmpty = true
                    local cmd_table = lay:onReadMessage(readbuffer)
                    while nil ~= cmd_table do
                        -- 记录局数
                        cmd_table.nGameRound = vinfo.nRound
                        -- 记录是否约战房
                        cmd_table.bIsPrivateRoom = tabGameInfo.bPrivateGame

                        table.insert(self.m_tabVideoMessageQueue, cmd_table)
                        table.insert(round_table, clone(cmd_table))
                        cmd_table = lay:onReadMessage(readbuffer)
                    end
                else
                    print("GameVideo 回放资源读取异常")
                    showToast(self._scene, "回放资源加载异常, CODE" .. GameVideo.ERRORCODE.CODE_RES_MISSING, 2)
                    self:dismissPopWait() 
                    return
                end

                self:addVideoNetMessage(vinfo.szVideoNumber, round_table)
            end

            -- 约战房总结算
            if tabGameInfo.bPrivateGame and PriRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
                -- 20170818 单局回放模式, 去除总结算
                --[[local detailinfo = GameVideo:getInstance():getVideoDetailInfo(tabGameInfo.szPersonalGUID)
                local cmd_table = lay:onGetPrivateEndMessage(detailinfo, userList)
                if nil ~= cmd_table then
                    table.insert(self.m_tabVideoMessageQueue, cmd_table)
                end]]

                -- 房间id
                PriRoom:getInstance().m_tabPriData.dwPersonalRoomID = tabGameInfo.dwPersonalRoomID
                -- 初始局数
                PriRoom:getInstance().m_tabPriData.dwPlayCount = 0
                -- 总局数
                PriRoom:getInstance().m_tabPriData.dwDrawCountLimit = nCount
                -- 房主ID
                PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID = tabGameInfo.dwTableOwnerUserID
            end

            self.m_nMessageCount = #self.m_tabVideoMessageQueue
            if 0 ~= self.m_nMessageCount then
                GlobalUserItem.bVideo = true
                -- 进入游戏, 回放
                if nil ~= self._scene then
                    if nil ~= self._scene.updateEnterGameInfo then
                        self._scene:updateEnterGameInfo(videogame)
                    end

                    -- 设置主视图ID
                    GlobalUserItem.dwVideoUserID = tabGameInfo.dwMainChairUserID
                    self._gameFrame._UserList = {}
                    -- 添加玩家
                    local nCount = 0
                    local player = self:getPlayer(userList)
                    for k, v in pairs(player) do
                        nCount = nCount + 1
                        self._gameFrame._UserList[v.dwUserID] = v

                        -- 设置椅子号(自己)
                        if GlobalUserItem.dwVideoUserID == v.dwUserID then
                            self._gameFrame._wChairID = v.wChairID
                        end
                    end

                        
                    -- 设置桌子号
                    self._gameFrame._wTableID = 1
                    -- 设置游戏人数
                    self._gameFrame._wChairCount = nCount

                    if nil ~= self._scene.onChangeShowMode then
                        self._scene:onChangeShowMode(yl.SCENE_GAME, tabGameInfo)
                    end
                    self:dismissPopWait()
                end
            else
                local gamename = videogame._GameName or ""
                showToast(self._scene, gamename .. " ## 录像数据读取异常", 2)
                self:dismissPopWait() 
            end
        else
            print("GameVideoReplay Missing")
            local gamename = videogame._GameName or ""
            local msg = string.format("请更新 %s 游戏资源!", gamename)
            showToast(self._scene, msg, 2)
            self:dismissPopWait() 
        end
    else
        print("GameVideo kind Missing")
        showToast(self._scene, "回放资源加载异常 Missing Kind", 2)
        self:dismissPopWait()
    end


end

-- 进入游戏界面
function GameVideo:enterGame( gameLayer, scene, param )
    if nil == gameLayer then
        print("GameLayer is Nil")
        return 
    end
    local config = gameLayer:getVideoLayerConfig()
    if nil == config then
        config = 
        {
            pos = cc.p(667, 100),
            wChairCount = self._gameFrame._wChairCount,
            maskOpacity = 120
        }
    end

    
    GlobalUserItem.bAutoConnect = false
    -- 用户进入
    for k, v in pairs(self._gameFrame._UserList) do
        gameLayer:onEventUserEnter(1, v.wChairID, v)
    end

    -- 约战房信息
    if param.bPrivateGame then
        if PriRoom and PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
            PriRoom:getInstance():enterGame(gameLayer, scene)

            -- 刷新信息
            PriRoom:getInstance()._priView:onRefreshInfo()
        end
    end
 
    -- 控制层
    local controlLayer = self:getTagLayer(LAYTAG.LAYER_VIDEOCONTROL, config, self._scene)
    if nil ~= controlLayer then
        controlLayer._gameLayer = gameLayer
        gameLayer:addVideoControlLayer(controlLayer)
    end
end

-- 退出游戏界面
function GameVideo:exitGame()
    GlobalUserItem.bVideo = false
    GlobalUserItem.bAutoConnect = true
    self._gameFrame._wChairID = yl.INVALID_CHAIR
    self._gameFrame._wChairCount = 0
    GlobalUserItem.dwVideoUserID = 0
    self._gameFrame._UserList = {}
    if PriRoom then
        -- 移除搜索路径
        PriRoom:getInstance():exitRoom()
    end
end

-- 获取消息
function GameVideo:getVideoMessage()
    print("播放索引 ==> ", self.m_nMessageIdx)
    if self.m_nMessageIdx > self.m_nMessageCount then
        return nil, true, ""
    end
    return self.m_tabVideoMessageQueue[self.m_nMessageIdx]
end

-- 索引快进
function GameVideo:forwardMessageIdx( nStep )
    nStep = nStep or 1
    self.m_nMessageIdx = self.m_nMessageIdx + 1
end

-- 回退索引
function GameVideo:backwardMessageIdx( nStep )
    nStep = nStep or 1
    self.m_nMessageIdx = self.m_nMessageIdx - nStep
    if self.m_nMessageIdx < 1 then
        self.m_nMessageIdx = 1
    end
end

-- 剩余消息数量
function GameVideo:leftVideoMessageCount()
    return self.m_nMessageCount - self.m_nMessageIdx
end

-- 获取玩家
function GameVideo:getPlayer( tabParam )
    local tabPlayer = {}
    local i = 0
    for k,v in pairs(tabParam) do
        local useritem = v
        useritem.cbMemberOrder = 0
        useritem.lScore = 0
        useritem.lIngot = 0
        useritem.dBeans = 0
        useritem.lGrade = 0
        useritem.lInsure = 0
        tabPlayer[v.dwUserID] = useritem
    end

    return tabPlayer
end

-- 网络管理
function GameVideo:getNetFrame()
    return self._videoFrame
end

-- 设置网络消息处理层
function GameVideo:setViewFrame(viewFrame)
    self._viewFrame = viewFrame
end

function GameVideo:popMessage(message, notShow)
    notShow = notShow or false
    if type(message) == "string" and "" ~= message then
        if notShow or nil == self._viewFrame then
            print(message)
        elseif nil ~= self._viewFrame then
            showToast(self._viewFrame, message, 2)
        end
    end
end

-- 返回值决定是否要断开网络
function GameVideo:onVideoLoginServerMessage(result, message, dataBuffer, notShow)
    if cmd_video_login.SUB_MB_QUERY_VIDEO_INFO_END == result
    or cmd_video_login.SUB_MB_QUERY_VIDEO_DETAILS_END == result then
        -- 列表记录
        if nil ~= self._viewFrame and nil ~= self._viewFrame.onRefreshList then
            return self._viewFrame:onRefreshList()
        end
        return true
    elseif cmd_video_login.SUB_MB_QUERY_PLAYBACK_CODE_RESULT == result
    or cmd_video_login.SUB_MB_QUERY_PLAYBACK_YZ_CODE_RESULT == result then
        self:dismissPopWait()

        local szmsg = "您的回放码为" .. message.dwPlayBackCode .. ", 是否分享给好友?"
        QueryDialog:create(szmsg,function(ok)
            if ok then
                local replay, videogame = self:getReplayScriptFile()
                if nil ~= videogame then
                    local modulestr = string.gsub(videogame._KindName, "%.", "/")
                    local gameFile = ""
                    if cc.PLATFORM_OS_WINDOWS == targetPlatform then
                        gameFile = "game/" .. modulestr .. "src/gamevideo/GameVideoReplay.lua" .. yl.WIN_SRC_SUFFIX
                    else
                        gameFile = "game/" .. modulestr .. "src/gamevideo/GameVideoReplay.lua"
                    end
                    if cc.FileUtils:getInstance():isFileExist(gameFile) then
                        local tabParam = appdf.req(gameFile):getShareMessage({dwPlayBackCode = message.dwPlayBackCode})
                        local szmessage = message.dwPlayBackCode .. ""
                        self._scene:popTargetShare(function(target, bF2f)
                            if nil ~= target then
                                MultiPlatform:getInstance():shareToTarget(target, function(isok)
                                    end, tabParam.title, tabParam.content, tabParam.url)
                            end
                        end, 3)

                    end
                end
            end
        end)
        :addTo(self._scene)
    end
end

-- 获取 GameVideoReplay
function GameVideo:getReplayScriptFile()
    print("getReplayScriptFile GameKind ==> ", GlobalUserItem.nCurGameKind)
    local videogame = nil
    -- 寻找游戏
    if nil ~= self._scene then
        videogame = self._scene:getGameInfo(GlobalUserItem.nCurGameKind)
    end
    if nil ~= videogame then
        local modulestr = string.gsub(videogame._KindName, "%.", "/")
        local gameFile = ""
        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            gameFile = "game/" .. modulestr .. "src/gamevideo/GameVideoReplay.lua" .. yl.WIN_SRC_SUFFIX
        else
            gameFile = "game/" .. modulestr .. "src/gamevideo/GameVideoReplay.lua"
        end
        if cc.FileUtils:getInstance():isFileExist(gameFile) then
            return appdf.req(gameFile), videogame
        end
    end
    return nil, nil
end