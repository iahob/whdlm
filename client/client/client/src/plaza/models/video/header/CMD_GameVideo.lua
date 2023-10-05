--
-- Author: zhong
-- Date: 2017-05-08 17:54:34
--
local video_struct = appdf.req(appdf.CLIENT_SRC .. "plaza.models.video.header.Struct_GameVideo")
local video_define = appdf.req(appdf.CLIENT_SRC .. "plaza.models.video.header.Define_GameVideo")

local cmd_gamevideo = {}
cmd_gamevideo.struct = video_struct
cmd_gamevideo.define = video_define

----------------------------------------------------------------------------------------------
-- 登陆服务器
local login = {}
------
-- 命令
------
-- 视频命令
login.MDM_MB_VIDEO_PLAY_BACK_INFO       = 102

-- 视频信息
login.SUB_MB_QUERY_VIDEO_INFO           = 1             -- 视频查询
login.SUB_MB_QUERY_VIDEO_DETAILS        = 2             -- 视频详情
login.SUB_MB_QUERY_PLAYBACK_CODE_YZ     = 3             -- 回放码申请(约战)
login.SUB_MB_QUERY_PLAYBACK_CODE        = 4             -- 回放码申请(普通)
login.SUB_MB_QUERY_VIDEO_DETAILS_BY_ROOMID = 5          -- 视频详情(通过约战房ID获取)
login.SUB_MB_QUERY_VIDEO_INFOEX           = 6             -- 视频查询(回放)

-- 返回结果
login.SUB_MB_QUERY_VIDEO_INFO_RESULT    = 101           -- 视频查询
login.SUB_MB_QUERY_VIDEO_INFO_END       = 102           -- 查询结束
login.SUB_MB_QUERY_VIDEO_DETAILS_RESULT = 103           -- 详情结果
login.SUB_MB_QUERY_VIDEO_DETAILS_END    = 104           -- 详情结束
login.SUB_MB_QUERY_PLAYBACK_YZ_CODE_RESULT = 105           -- 返回回放码(约战)
login.SUB_MB_QUERY_PLAYBACK_CODE_RESULT = 106           -- 返回回放码(普通)

------
-- 消息结构
------
-- 视频信息查询
login.CMD_MB_C_QueryVideoInfo = 
{
    {k = "iVideoType", t = "int"},                      -- 0:约战 1:普通
    {k = "dwUserID", t = "dword"},                      -- 用户ID
    {k = "dwPlayBack", t = "dword"},                    -- 查询视频( 0全部, 其他是回放码)
    --{k = "szRoomID", t = "tchar", s = video_define.LEN_ROOMID}, -- 约战房ID
    {k = "dwPersonalRoomID", t = "dword"},
    -- 记录索引, 用于分页查询
    {k = "wIndexBegin", t = "word"},                    -- 索引开始
    {k = "wIndexEnd", t = "word"},                      -- 索引结束
}

-- 视频信息查询(回放使用)
login.CMD_MB_C_QueryVideoInfoEX = 
{
    {k = "iVideoType", t = "int"},                      -- 0:约战 1:普通
    {k = "dwUserID", t = "dword"},                      -- 用户ID
    {k = "dwPlayBack", t = "dword"},                    -- 查询视频( 0全部, 其他是回放码)
    {k = "szdwPersonalGUID", t = "tchar", s = 32}, -- 约战房GUID
    {k = "dwPersonalRoomID", t = "dword"},
    -- 记录索引, 用于分页查询
    {k = "wIndexBegin", t = "word"},                    -- 索引开始
    {k = "wIndexEnd", t = "word"},                      -- 索引结束
}

-- 视频详情
login.CMD_MB_C_QueryVideoDetails = 
{
    -- 约战房唯一ID
    {k = "szPersonalGUID", t = "tchar", s = video_define.PERSONAL_ROOM_GUID},
}

-- 视频详情
login.CMD_MB_C_QueryVideoDetailsByRoomID = 
{
    -- 约战房唯一ID
    --{k = "szRoomID", t = "tchar", s = video_define.LEN_ROOMID},
    {k = "dwPersonalRoomID", t = "dword"},
}

-- 获取回放码(约战)
login.CMD_MB_C_QueryPlayBackCode_YZ = 
{
    -- 用户ID
    {k = "dwUserID", t = "dword"},
    -- 约战房唯一ID                  
    {k = "szPersonalGUID", t = "tchar", s = video_define.PERSONAL_ROOM_GUID},
}

-- 获取回放码(普通)
login.CMD_MB_C_QueryPlayBackCode = 
{
    -- 用户ID
    {k = "dwUserID", t = "dword"},
    -- 视频唯一ID                
    {k = "szVideoNumber", t = "tchar", s = video_define.LEN_VIDEO_NUMBER},
}

-- 回包列表数量
login.CMD_MB_S_QueryListCount = 
{
    {k = "wListCount", t = "word"},
}

-- 录像用户信息(单个)
login.CMD_MB_S_QueryVideoInfoResult = 
{
    -- 0:约战 1:普通
    {k = "iVideoType", t = "int"},
    -- 用户ID
    {k = "dwUserID", t = "dword"},
    -- 房间编号
    --{k = "szServerID", t = "string", s = 7},
    {k = "dwPersonalRoomID", t = "dword"},
    -- 用户昵称
    {k = "szNickName", t = "string", s = yl.LEN_NICKNAME},
    -- 约战房唯一ID
    {k = "szPersonalGUID", t = "string", s = video_define.PERSONAL_ROOM_GUID},
    -- 游戏ID
    {k = "dwGameID", t = "dword"},
    -- 头像标识
    {k = "wFaceID", t = "word"},
    -- 自定义头像
    {k = "dwCustomID", t = "dword"},
    -- 是否房主
    {k = "cbCreateRoom", t = "byte"},
    -- 总成绩
    {k = "dwTotalScore", t = "score"},
    -- 创建时间
    {k = "CreateDate", t = "table", d = video_struct.SYSTEMTIME},
    -- 视频唯一ID 
    {k = "szVideoNumber", t = "string", s = video_define.LEN_VIDEO_NUMBER},
    -- 座位id
    {k = "wChairID", t = "word"},
    -- 游戏类型
    {k = "wKindID", t = "word"},
    -- 性别
    {k = "cbGender", t = "byte"},
    -- 回放码用户ID(分享用户)
    {k = "dwPlayBackUserID", t = "dword"},
    {k = "lCellScore", t = "int"}, --底分
    {k = "nGameNum", t = "int"}, --局数
}

-- 详情返回
login.CMD_MB_S_QueryVideoDetailsResult = 
{
    -- 用户ID
    {k = "dwUserID", t = "dword"},
    -- 约战房唯一ID
    {k = "szPersonalGUID", t = "string", s = video_define.PERSONAL_ROOM_GUID},
    -- 视频唯一ID 
    {k = "szVideoNumber", t = "string", s = video_define.LEN_VIDEO_NUMBER},
    -- 用户昵称
    {k = "szNickName", t = "string", s = yl.LEN_NICKNAME},
    -- 局数
    {k = "dwGamesNum", t = "word"},
    -- 游戏模式
    {k = "cbGameMode", t = "byte"},
    -- 当前圈数
    {k = "cbLoopCount", t = "byte"},
    -- 总成绩
    {k = "lScore", t = "score"},
    -- 游戏结算数据(根据不同游戏, 判断是否需要特殊读取) 20170808 移除
    --{k = "szGameoverData", t = "byte", l = {video_define.LEN_GAMEOVER_DATA}}
}

-- 回放码(约战)
login.CMD_MB_S_QueryPlayBackCode_YZ_Result = 
{
    -- 用户ID
    {k = "dwUserID", t = "dword"},
    -- 约战房唯一ID
    {k = "szPersonalGUID", t = "string", s = video_define.PERSONAL_ROOM_GUID},
    -- 回放码
    {k = "dwPlayBackCode", t = "dword"},
}

-- 回放码(普通)
login.CMD_MB_S_QueryPlayBackCode_Result = 
{
    -- 用户ID
    {k = "dwUserID", t = "dword"},
    -- 视频唯一ID 
    {k = "szVideoNumber", t = "string", s = video_define.LEN_VIDEO_NUMBER},
    -- 回放码
    {k = "dwPlayBackCode", t = "dword"},
}

cmd_gamevideo.login = login
-- 登陆服务器
----------------------------------------------------------------------------------------------

return cmd_gamevideo