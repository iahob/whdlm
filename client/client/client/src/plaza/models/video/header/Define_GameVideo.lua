--
-- Author: zhong
-- Date: 2017-03-22 15:32:24
--
local gamevideo_define = {}

gamevideo_define.PERSONAL_ROOM_GUID     = 32                                -- 约战房间唯一标识
gamevideo_define.LEN_VIDEO_NUMBER       = 22                                -- 视频ID长度
gamevideo_define.LEN_ROOMID             = 7                                 -- 约战房ID
gamevideo_define.LEN_GAMEOVER_DATA      = 50                                -- 数据长度(游戏结算)
local tabModule = {}
-- 模块定义
tabModule.VIDEOHEADER                   = "client.src.plaza.models.video.header."           -- 回放header
tabModule.PLAZAMODULE                   = "client.src.plaza.models.video.models."           -- 大厅模块
gamevideo_define.tabModule = tabModule

-- 场景/层 tag定义
local tabLayTag = {}
tabLayTag.LAYER_BEGIN                  = 4000
tabLayTag.LAYER_VIDEOCONTROL           = 4001                               -- 视频控制层
tabLayTag.LAYER_VIDEOLIST              = 4002                               -- 视频列表
tabLayTag.LAYER_VIDEOINFO              = 4003                               -- 视频详情
tabLayTag.LAYER_END                    = 4004
gamevideo_define.tabLayTag = tabLayTag

-- 错误代码
local tabErrorCode = {}
tabErrorCode.CODE_NO_DEFINE             = 5001                              -- 未定义错误
tabErrorCode.CODE_REPLAYLOADER_ERROR    = 5002                              -- 游戏回放控制加载异常( GameVideoReplay )
tabErrorCode.CODE_CACHE_ERROR           = 5003                              -- 快退消息处理异常
tabErrorCode.CODE_VIEW_ERROR            = 5004                              -- 回放界面为空或异常
tabErrorCode.CODE_RES_MISSING           = 5005                              -- 录像消息文件丢失
gamevideo_define.tabErrorCode = tabErrorCode

-- 视频查询类型
gamevideo_define.QUERY_NORMAL_VIDEO     = 1                                 -- 查询普通
gamevideo_define.QUERY_PRIVATE_VIDEO    = 0                                 -- 约战查询
gamevideo_define.QUERY_PRIVATE_SCORE_VIDEO    = 2                           --积分战绩
gamevideo_define.QUERY_PRIVATE_GOLD_VIDEO    = 3                            --金币战绩
return gamevideo_define