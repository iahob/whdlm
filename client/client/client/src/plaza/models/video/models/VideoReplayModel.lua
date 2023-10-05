--
-- Author: zhong
-- Date: 2017-03-20 11:46:57
--
-- 游戏回放模板
local VideoReplayModel = class("VideoReplayModel")
local define_video = appdf.req(GameVideo.MODULE.VIDEOHEADER .. "Define_GameVideo")

function VideoReplayModel:ctor()
end

-- 回放邀请信息
function VideoReplayModel:getShareMessage( tabParam )
    return 
    {
        title = "",
        content = "",
        url = "",
        imagepath = "",
        isImage = "false",
    }
end

-- 读取单局结算详情
function VideoReplayModel:onReadSingleGameSettlement( pBuffer )
    print("VideoReplayModel:onReadSingleGameSettlement")
    local cmd_table = {}
    for i = 1, define_video.LEN_GAMEOVER_DATA do
        pBuffer:readbyte()
    end
    return cmd_table
end

-- 单局结算
function VideoReplayModel:getVideoResultView( tabParam )
    print("VideoReplayModel:getVideoResultView")
    return nil
end

-- 支持单局结算功能
function VideoReplayModel:enableResultView()
    print("VideoReplayModel:enableResultView")
    return false
end

-- 消息读取
function VideoReplayModel:onReadMessage( pBuffer )
    print("VideoReplayModel:onReadMessage")
    return nil
end

-- 生成约战总结算消息
function VideoReplayModel:onGetPrivateEndMessage( videoDetailInfo, userList )
    print("VideoReplayModel:onGetPrivateEndMessage")
    return nil
end

-- 消息处理
-- @param[gameLayer] 游戏层(GameModel)
-- @param[msgTable] 消息数据
-- @param[bForward] 快进处理
-- @param[bBackward] 快退处理
-- @return 10(下一个消息处理间隔) true(当前消息是否允许快进快退) true(当前消息是否处理异常) ""(提示内容)
function VideoReplayModel:onHandleMessage( gameLayer, msgTable, bForward, bBackward )
    dump(msgTable, "VideoReplayModel:onHandleMessage", 10)
    return 10, true, true, "回放功能暂未开放"
end

return VideoReplayModel