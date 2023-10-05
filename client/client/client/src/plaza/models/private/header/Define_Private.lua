--
-- Author: zhong
-- Date: 2016-12-03 14:36:31
--
local private_define = {}

private_define.ROOM_ID_LEN = 7                                              -- 私人房ID长度
private_define.MAX_CREATE_PERSONAL_ROOM = 20                                -- 个人创建私人房最大数量
private_define.PERSONAL_ROOM_CHAIR = 20
private_define.RULE_LEN = 100                                               -- 私人房补充规则
private_define.LEN_PERSONAL_ROOM_SCORE = 844                                -- sizeof(tagQueryPersonalRoomUserScore)
private_define.LEN_PERSONAL_TABLE_PARAMETER = 28                            -- sizeof(tagPersonalTableParameter)
private_define.CELL_SCORE_NUM = 5                                           -- 底分配置最大数量
private_define.LEN_PERSONAL_RULE = 1000                                       -- 额外配置信息
private_define.LEN_GOLDROOMINFO = 152  +64                                     -- 金币私有房列表信息单个长度
private_define.LEN_PERSONAL_OTHER = 161                                       -- 额外配置前面的其他数据
local tabModule = {}
-- 模块定义
tabModule.PRIMODULE                = "client.src.plaza.models.private."              -- 私人房模块
tabModule.PRIHEADER                = "client.src.plaza.models.private.header."       -- 私人房header
tabModule.PLAZAMODULE              = "client.src.plaza.models.private."         -- 大厅模块
private_define.tabModule = tabModule

-- 场景/层 tag定义
local tabLayTag = {}
tabLayTag.LAYER_ROOMLIST           = 1001                                   -- 私人房模式房间列表
tabLayTag.LAYER_CREATEPRIROOME     = 1002                                   -- 创建私人房界面
tabLayTag.LAYER_MYROOMRECORD       = 1003                                   -- 我的房间(记录)
tabLayTag.LAYER_HELP               = 1004                                   -- 帮助界面
tabLayTag.LAYER_ROOMID             = 1005                                   -- 房间ID输入
tabLayTag.LAYER_CREATERESULT       = 1006                                   -- 创建结果
tabLayTag.LAYER_EXCHANGESCORE      = 1007                                   -- 房卡兑换游戏币
tabLayTag.LAYER_BUYCARD            = 1008                                   -- 游戏豆购买房卡
tabLayTag.LAYER_FRIENDLIST         = 1009                                   -- 好友分享列表
tabLayTag.LAYER_AAPAY              = 1010                                   -- AA制付款
tabLayTag.LAYER_PASSWDIDPUT        = 1011                                   -- 密码输入          
private_define.tabLayTag = tabLayTag

-- 游戏服务器登陆操作定义
local tabLoginAction = {}
tabLoginAction.ACT_NULL            = 2000                                   -- 未定义
tabLoginAction.ACT_CREATEROOM      = 2001                                   -- 创建房间
tabLoginAction.ACT_SEARCHROOM      = 2002                                   -- 搜索房间
tabLoginAction.ACT_DISSUMEROOM     = 2003                                   -- 解散房间
tabLoginAction.ACT_ENTERTABLE      = 2004                                   -- 进入游戏
tabLoginAction.ACT_QUERYJOININFO   = 2005                                   -- 查询信息
tabLoginAction.ACT_INPUTPASSWD     = 2006                                   -- 密码输入


private_define.tabLoginAction = tabLoginAction

-- 密码设置
local tabPasswdMode = {}
tabPasswdMode.NO_PASSWD            = 0                                      -- 无密码
tabPasswdMode.SET_PASSWD           = 1                                      -- 需要密码
private_define.tabPasswdMode = tabPasswdMode

-- 支付设置
local tabPayMode = {}
tabPayMode.ONE_PAY                 = 0                                      -- 一人付
tabPayMode.AA_PAY                  = 1                                      -- AA制
private_define.tabPayMode = tabPayMode

-- 解散处理
local tabDismissDeal = {}
tabDismissDeal.NO_DEAL             = 0                                      -- 未处理
tabDismissDeal.AGREE               = 1                                      -- 同意
tabDismissDeal.DISAGREE            = 2                                      -- 不同意
private_define.tabDismissDeal = tabDismissDeal

-- 房间状态
local tabRoomState = {}
tabRoomState.FREE_STATE            = 0                                      -- 空闲状态
tabRoomState.PLAYING_STATE         = 1                                      -- 游戏状态
tabRoomState.END_STATE             = 2                                      -- 解散状态(结束状态)
private_define.tabRoomState = tabRoomState

return private_define