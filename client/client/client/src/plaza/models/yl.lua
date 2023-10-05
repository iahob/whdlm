yl = yl or {}

yl.WIDTH								= 1334--1136--1334
yl.HEIGHT								= 750--640--750

yl.DEVICE_TYPE							= 0x10
yl.KIND_ID								= 122
yl.STATION_ID							= 1                       

-- 登陆地址列表
yl.SERVER_LIST = {}




-- 演示
-- table.insert(yl.SERVER_LIST, "120.79.6.238") --@login_ip -- 演示 登陆地址
--table.insert(yl.SERVER_LIST, "172.16.0.78") --@login_ip -- 演示 登陆地址
yl.LOGONPORT                            	= 6801 --@login_port -- 演示 登陆端口
yl.FRIENDPORT                           	= 6802 --@friend_port -- 演示 好友端口
yl.CLUBPORT                             	= 6806 --@club_port --演示

table.insert(yl.SERVER_LIST, "10.215.167.229") --@login_ip -- 演示 登陆地址
yl.HTTP_URL                             	= "http://localhost:4778" --@http_url --演示 http请求链接地址
yl.IMAGE_URL                            	= "http://localhost:4778/Upload" --@image_url --演示 图片服务器

yl.HTTP_URL_DOWNLOAD                = "http://localhost:4778" --@http_url --演示 http请求链接地址
--210
-- table.insert(yl.SERVER_LIST, "172.16.3.210") --@login_ip -- 演示 登陆地址
-- -- table.insert(yl.SERVER_LIST, "172.16.0.73") --@login_ip -- 演示 登陆地址
-- yl.HTTP_URL                             = "http://172.16.3.210:6805" --@http_url --演示 http请求链接地址
-- yl.IMAGE_URL                            = "http://172.16.3.210:6810" --@image_url --演示 图片服务器

-- yl.LOGONPORT                            	= 6801 --@login_port -- 演示 登陆端口
-- yl.FRIENDPORT                           	= 6802 --@friend_port -- 演示 好友端口
-- yl.CLUBPORT                             	= 6806 --@club_port --演示



-- 当前地址索引
yl.CURRENT_INDEX = 1
yl.TOTAL_COUNT = #yl.SERVER_LIST
-- 登陆地址
yl.LOGONSERVER                          = yl.SERVER_LIST[yl.CURRENT_INDEX]
--编译码
--yl.VALIDATE 							= "8C3AC3BC-EB40-462f-B436-4BBB141FC7F9" --@compilation
-- 约战分享
yl.PRIVATE_SHARE_URL                    = yl.HTTP_URL .. "/Mobile/RoomLink.aspx"

yl.GROUP_SHARE_URL                    = yl.HTTP_URL .. "/Mobile/GroupShareLink.aspx"

-- http请求支持(loginScene)
yl.HTTP_SUPPORT				                      			= true
-- 是否显示信息弹窗的ip和位置信息
yl.SHOW_IP_ADDRESS                      = true
-- 是否单游戏模式(游戏列表数目为1生效)
yl.SINGLE_GAME_MODOLE                   = false
-- 是否自动登录
yl.AUTO_LOGIN                           = true
-- windows过滤更新
yl.DISABLE_WIN_UPDATE                   = true
-- windows lua代码检查后缀
yl.WIN_SRC_SUFFIX                       = ""

-- 判断条件
yl.ENABLE_VALUE                         = 0

yl.WIN32_DEVID 							= 'A501164B366ECFC9E249163873094D51';
yl.WIN32_DEVID 							= '111111';

-- 消息类型
yl.SYSTEM_MSG                           = 1     -- 系统消息
yl.TRUMPET_MSG                          = 2     -- 喇叭消息

-- url动作类型
yl.URL_PRIVATE_ACTION                   = 1     -- 约战动作
yl.URL_VIDEO_ACTION                     = 2     -- 回放动作
yl.URL_CLUB_ACTION                     = 3     -- 俱乐部推广动作

-- 广告动作
yl.AD_ACTION_TOSHOP                     = "ad_to_shop_action"   -- 跳转商店
yl.AD_ACTION_TOSPREAD                   = "ad_to_spread_action" -- 跳转推广
yl.AD_ACTION_CREATEROOM                 = "ad_to_createroom_action" -- 创建界面
yl.AD_ACTION_VIDEO                      = "ad_to_video_action" -- 回放
yl.AD_ACTION_IDENTIFY                   = "ad_to_identify_action" -- 认证

yl.PLAZA_ROOMLIST_LAYER                 = "__plaza_roomlist_layer__"

-- 奖励类型
yl.REWARD_LIST = 
{
    SPREAD_REWARD = 1,      -- 推广奖励
    RANK_REWARD = 2,        -- 排行奖励
    REGISTER_REWARD = 3,    -- 注册奖励
}

yl.COMMON_LOADING_ANI                   = "common_loading_animation" -- 通用loading

yl.DOWN_PRO_INFO					= 1
yl.DOWN_UNZIP_STATUS					= 2
yl.DOWN_COMPELETED					= 3
yl.DOWN_ERROR_PATH					= 4 									--路径出错
yl.DOWN_ERROR_CREATEFILE				= 5 									--文件创建出错
yl.DOWN_ERROR_CREATEURL				= 6 									--创建连接失败
yl.DOWN_ERROR_NET		 			= 7 									--下载失败
yl.DOWN_ERROR_UNZIP 					= 8

yl.SCENE_PLAZA                          = 1         -- 大厅界面
yl.SCENE_ROOMLIST                       = 2         --房间列表
yl.SCENE_GAME                           = 21        -- 游戏界面
yl.SCENE_MATCH							= 30
yl.SCENE_CLUB                           = 40
yl.SCENE_CLUB_ROOM                           = 41

yl.SCENE_EX_END 						= 50

yl.MAIN_SOCKET_INFO						= 0

yl.SUB_SOCKET_CONNECT					= 1
yl.SUB_SOCKET_ERROR						= 2
yl.SUB_SOCKET_CLOSE						= 3

yl.US_NULL								= 0x00		--没有状态
yl.US_FREE								= 0x01		--站立状态
yl.US_SIT								= 0x02		--坐下状态
yl.US_READY							= 0x03		--同意状态
yl.US_LOOKON							= 0x04		--旁观状态
yl.US_PLAYING					 		= 0x05		--游戏状态
yl.US_OFFLINE							= 0x06		--断线状态

yl.INVALID_TABLE						= 65535
yl.INVALID_CHAIR						= 65535
yl.INVALID_ITEM							= 65535
yl.INVALID_USERID						= 0			--无效用户
yl.INVALID_BYTE                         = 255
yl.INVALID_WORD                         = 65535
yl.INVALID_DWORD                         = 0xFFFFFFFF

yl.GENDER_FEMALE						= 0				--女性性别
yl.GENDER_MANKIND						= 1				--男性性别

yl.GAME_GENRE_GOLD						= 0x0001		--金币类型
yl.GAME_GENRE_SCORE						= 0x0002		--点值类型
yl.GAME_GENRE_MATCH						= 0x0004		--比赛类型
yl.GAME_GENRE_EDUCATE					= 0x0008		--训练类型
yl.GAME_GENRE_PERSONAL 					= 0x0010 		-- 约战类型

yl.SERVER_GENRE_PASSWD					= 0x0002 		-- 密码类型

yl.SR_ALLOW_AVERT_CHEAT_MODE			= 0x00000040	--隐藏信息

yl.UR_OPEN_TRANSFER                     = 0x00000040    -- 开放转账

yl.LEN_GAME_SERVER_ITEM					= 183 +68+2			--房间长度
--yl.LEN_GAME_SERVER_ITEM					= 177			--房间长度
yl.LEN_TASK_PARAMETER					= 813			--任务长度
yl.LEN_TASK_STATUS						= 5             --任务长度

yl.LEN_MD5								= 33			--加密密码
yl.LEN_ACCOUNTS							= 32			--帐号长度
yl.LEN_NICKNAME							= 32			--昵称长度
yl.LEN_PASSWORD							= 33			--密码长度
yl.LEN_USER_UIN							= 33
yl.LEN_QQ                               = 16            --Q Q 号码
yl.LEN_EMAIL                            = 33            --电子邮件
yl.LEN_COMPELLATION 					= 16			--真实名字
yl.LEN_SEAT_PHONE                       = 33            --固定电话
yl.LEN_MOBILE_PHONE                     = 12            --移动电话
yl.LEN_PASS_PORT_ID                     = 19            --证件号码
yl.LEN_COMPELLATION                     = 16            --真实名字
yl.LEN_DWELLING_PLACE                   = 128           --联系地址
yl.LEN_UNDER_WRITE                      = 32            --个性签名
yl.LEN_PHONE_MODE                       = 21            --手机型号
yl.LEN_SERVER                           = 32            --房间长度
yl.LEN_TRANS_REMARK						= 32			--转账备注
yl.LEN_TASK_NAME						= 64			--任务名称
yl.LEN_WEEK                             = 7
yl.LEN_DEVICE_TOKEN                     = 65            --绑定标志长度
yl.LEN_FACE_URL                         = 500           --头像链接长度

yl.LEN_MACHINE_ID						= 33			--序列长度
yl.LEN_USER_CHAT						= 128			--聊天长度
yl.SOCKET_TCP_BUFFER					= 16384			--网络缓冲

ylSUB_CM_SYSTEM_MESSAGE					= 1				--系统消息
ylSUB_CM_ACTION_MESSAGE					= 2				--动作消息

yl.SMT_CHAT								= 0x0001		--聊天消息
yl.SMT_EJECT							= 0x0002		--弹出消息
yl.SMT_GLOBAL							= 0x0004		--全局消息
yl.SMT_PROMPT							= 0x0008		--提示消息
yl.SMT_TABLE_ROLL						= 0x0010		--滚动消息

yl.SMT_CLOSE_ROOM						= 0x0100		--关闭房间
yl.SMT_CLOSE_GAME						= 0x0200		--关闭游戏
yl.SMT_CLOSE_LINK						= 0x0400		--中断连接
yl.SMT_CLOSE_INSURE						= 0x0800		--关闭银行

--货币类型
yl.CONSUME_TYPE_GOLD					= 0x01			--游戏币
yl.CONSUME_TYPE_USEER_MADEL				= 0x02			--元宝
yl.CONSUME_TYPE_CASH					= 0x03			--游戏豆
yl.CONSUME_TYPE_LOVELINESS				= 0x04			--魅力值

--发行范围
yl.PT_ISSUE_AREA_PLATFORM				= 0x01			--大厅道具（大厅可以使用）
yl.PT_ISSUE_AREA_SERVER					= 0x02			--房间道具（在房间可以使用）
yl.PT_ISSUE_AREA_GAME					= 0x04			--游戏道具（在玩游戏时可以使用）
--喇叭物品
yl.LARGE_TRUMPET                        = 306           --大喇叭id
yl.SMALL_TRUMPET                        = 307           --小喇叭id

yl.PROP_INTERACT							=1	        --互动道具类型

yl.SCREEN_CHANGE = 1

--赠送目标类型
yl.PRESEND_NICKNAME						= 0
yl.PRESEND_GAMEID						= 1

--notifycation
yl.RY_USERINFO_NOTIFY					= "ry_userinfo_notify"		-- 玩家信息更新通知
yl.RY_MSG_USERHEAD						= 101						-- 更新用户头像
yl.RY_MSG_USERWEALTH					= 102						-- 更新用户财富
yl.RY_MSG_USERIDENTIFY                  = 103                       -- 实名认证
yl.RY_CULB_DIAMOND_NOTIFY               = 104
yl.RY_CULB_MEMBER_NOTIFY                = 105
yl.RY_CULB_DELETE_NOTIFY                = 106
yl.RY_CULB_PRIROOM_HIDE                 = 107                       --俱乐部私有房隐藏
yl.RY_CULB_PRIROOM_CHANGE               = 108                       --俱乐部私有房切换
yl.RY_CULB_PRIROOM_CHANGETYPE           = 109                       --更新到新的俱乐部
yl.RY_CULB_OUT_NOTIFY           = 110                       --退出俱乐部
yl.RY_CULB_ENTER_NOTIFY   = 111		--进入俱乐部

yl.RY_CLUBINFO_NOTIFY                   = "ry_clubinfo_notify"      -- 俱乐部信息更新通知
yl.RY_MSG_APPLYMSG                      = 101                       -- 有新的申请消息



yl.RY_MSG_USERRECHARGE					= 104						-- 更新用户支付结果

yl.RY_FRIEND_NOTIFY                     = "ry_friend_notify"        --好友更新
yl.RY_MSG_FRIENDDEL                     = 101                       --好友删除

yl.TRUMPET_COUNT_UPDATE_NOTIFY			= "ry_trumpet_count_update" --喇叭数量更新

yl.RY_JFTPAY_NOTIFY						= "ry_jftpay_notify"		--竣付通支付后台切换通知

yl.RY_NEARUSER_NOTIFY                   = "ry_nearuser_notify"      --附近玩家信息获取
yl.RY_IMAGE_DOWNLOAD_NOTIFY             = "ry_image_download_notify"--图片下载结束

yl.CLIENT_NOTIFY                        = appdf.CLIENT_NOTIFY       -- 客户端通知
yl.CLIENT_MSG_TAKE_SCREENSHOT           = appdf.CLIENT_MSG_TAKE_SCREENSHOT -- 截屏事件
yl.CLIENT_MSG_HTTP_WEALTH               = appdf.CLIENT_MSG_TAKE_SCREENSHOT + 1 -- http请求用户财富
yl.CLIENT_MSG_SYSTEMINFO                = appdf.CLIENT_MSG_TAKE_SCREENSHOT + 2 -- 系统信息
yl.CLIENT_MSG_PRIVATEROOM_REFRESH       = appdf.CLIENT_MSG_TAKE_SCREENSHOT + 3 -- 约战房间刷新
yl.CLIENT_MSG_IOS_OPENURL               = appdf.CLIENT_MSG_TAKE_SCREENSHOT + 4 -- ios客户端打开链接
yl.CLIENT_MSG_BACKGROUND_ENTER          = appdf.CLIENT_MSG_TAKE_SCREENSHOT + 5 -- 客户端从后台切换回来
yl.CLIENT_MSG_SCREEN_CHANGE          = appdf.CLIENT_MSG_TAKE_SCREENSHOT + 6 -- 屏幕切换通知

yl.RY_VOICE_NOTIFY                      = "ry_game_voice_notify"    --录音监听
yl.RY_MSG_VOICE_START                   = 201                       --录音开始
yl.RY_MSG_VOICE_END                     = 202                       --录音结束

--分享配置
yl.SocialShare =
{
	title 								= "网狐旗舰", --@share_title_social
	content 							= "欢迎体验网狐旗舰版，不一样的精彩等你来！", --@share_content_social
	url 								= yl.HTTP_URL
}

-- 分享错误代码
yl.ShareErrorCode = 
{
    NOT_CONFIG                          = 1
}

--微信配置定义
yl.WeChat = 
{
	AppID 								= "wx67b653ce4491dbd3", --@wechat_appid_wx
	AppSecret 							= "816aa8c930a9effcd2945c048308d568", --@wechat_secret_wx
	-- 商户id
	PartnerID 							= " ", --@wechat_partnerid_wx
	-- 支付密钥					        
	PayKey 								= " ", --@wechat_paykey_wx
	URL 								= yl.HTTP_URL,
}

--支付宝配置
yl.AliPay = 
{
	-- 应用id pid
	AppID							    = "2018120662454440", --@alipay_partnerid_zfb
	
	-- 合作者身份id
	PartnerID							= "2088211548234518", --@alipay_partnerid_zfb
	-- 收款支付宝账号						
	SellerID							= "WHNetPayID@foxuc.cn", --@alipay_sellerid_zfb
	-- rsa密钥
	RsaKey								= "MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBAKY0Hmny7UCXppOg8Qryc18zUZSAE9TbkBIBrxfMx1BPs5/hswl57XHMJTqVW4lUNgFwy/9QdEM/tYswl4Jav8KqHuB4GIUk6+k8hTCzl2tDHTPEnx0Iw5sb+dS3lJOA1Q9JOnoOz9zZXGyzJWMDKzNewUZX4br82K+l8WhLiMqHAgMBAAECgYEAjahKa98jnDQlRBqel8gB0Zh55eor/PaOmupYhB6Dms3zuUezQ1ZqrN1ktcPrWuctiJjNJnU6Qr8gStxFplMWsMKB0JfEs+ZIAk4Qo7ZFeAEcJtVYMm2785TKU5xjxcgrcQK9ICYKajOQG8Jv0HV6kUtJLSKHdKNZH7cc65TNwaECQQDSajjCpixEMyIDh0RYzCol7DogNmo6deUJ7PFgJr3k2eEsGycBLE7DK9jgwYAaZnMEllKwm+A3ljTUtTVYXZvZAkEAyjXmt+y8BeQHxRwXBMh1GfuCctWASFuLxlRhvR7eH/enqiINRoxyOMpVK0abEY/qGGYbnBsZOYOxEgSq6Gt9XwJAaQUHS2+xDGERCv3KiR9tsfvTjxxZrNAG/ws59imgAccVcfQjZZx9uz/5QUSS8ZpMse9tWiSSpmvb9oZzF6WxEQJAEMW/yWm9fs+XRWY7Eo5+CBWwkE3dQiM1yNGe+czhp80m/xmr+DoWD1fZEKnemupRBI0TpTMqOHcP7OquMnEYCQJAKoVniHoTaKZm3NYtVgPuy7DmpLtUusLhR4/iEYTAo/pnmQCmUXDkI+zXAyP2zSmOkEOAkE43R3H10/oAc4qvww==", --@alipay_rsa_zfb
	NotifyURL							= yl.HTTP_URL .. "/Notify/Alipay.aspx",
	-- ios支付宝Schemes
	AliSchemes							= "WHQJAliPay", --@alipay_schemes_zfb
}

--竣付通配置
yl.JFT =
{
	--商户支付密钥
	PayKey 								= " ", --@jft_paykey_jtpay
	--商户id											
	PartnerID 							= " ", --@jft_partnerid_jtpay
	--token												
	TokenURL							= "http://api.jtpay.com/jft/sdk/token/", --@jft_tokenurl_jtpay
	--后台通知url
	NotifyURL							= yl.HTTP_URL .. "/Pay/JFTAPP/Notify.aspx",
	--appid				
	JftAppID							= " ", --@jft_appid_jtpay								
	JftAesKey							= " ", --@jft_aeskey_jtpay
	JftAesVec 							= " ", --@jft_aesvec_jtpay
}

--高德配置
yl.AMAP = 
{
	-- 开发KEY
	AmapKeyIOS							= "3ff877c05a3d8d8424e0a9e7d36ae415", --@ios_devkey_amap
	AmapKeyAndroid						= "b6e9a7c6c9b556a1811314a192de11a4", --@android_devkey_amap
}

yl.PLATFORM_WX							= 5				--微信平台

--竣付通配置
yl.LQ =
{
    --商户ID
    comp_id                              = "CP000126937584", --@
    --产品ID                                          
    prod_id                           = "PD000174926805", --@
    --支付URL                                            
    prepayUrl                            = "", --@
    --登录参数
    noPwdRegParam                           = "",
    --登录参数
    noPwdRegsign                           = "",
}

--第三方平台定义(同java/ios端定义值一致)
yl.ThirdParty = 
{
	WECHAT 								= 0,	-- 微信
	WECHAT_CIRCLE						= 1,	-- 朋友圈
	ALIPAY								= 2,	-- 支付宝
	JFT								 	= 3,	-- 俊付通
	AMAP 								= 4,	-- 高德地图
	IAP 							 	= 5,	-- ios iap
    SMS                                 = 6,    -- 分享到短信
    LQ                                  = 7,    -- 零钱支付
}
--平台id列表(服务器登陆用)
yl.PLATFORM_LIST = {}
yl.PLATFORM_LIST[yl.ThirdParty.WECHAT]	= 5

yl.MAX_INT                              = 2 ^ 15
--是否动态加入
yl.m_bDynamicJoin                       = false
--设备类型
yl.DEVICE_TYPE_LIST = {}
yl.DEVICE_TYPE_LIST[cc.PLATFORM_OS_WINDOWS] = 0x01
yl.DEVICE_TYPE_LIST[cc.PLATFORM_OS_ANDROID] = 0x11
yl.DEVICE_TYPE_LIST[cc.PLATFORM_OS_IPHONE] 	= 0x31
yl.DEVICE_TYPE_LIST[cc.PLATFORM_OS_IPAD] 	= 0x41

local poker_data = 
{
	0x00,
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, -- 方块
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, -- 梅花
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, -- 红桃
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, -- 黑桃
    0x4E, 0x4F
}
-- 逻辑数值
yl.POKER_VALUE = {}
-- 逻辑花色
yl.POKER_COLOR = {}
-- 纹理花色
yl.CARD_COLOR = {}
function yl.GET_POKER_VALUE()
	for k,v in pairs(poker_data) do
		yl.POKER_VALUE[v] = math.mod(v, 16)
		yl.POKER_COLOR[v] = bit:_and(v , 0XF0)
		yl.CARD_COLOR[v] = math.floor(v / 16)
	end
end
yl.GET_POKER_VALUE()

-- 界面z轴定义
function yl.GET_ZORDER_ENUM( nStart, nStep, keyTable )
    local enStart = 1
    if nil ~= nStart then
        enStart = nStart
    end

    local args = keyTable
    local enum = {}
    for i=1,#args do
        enum[args[i]] = enStart
        enStart = enStart + nStep
    end

    yl.ZORDER = enum
end
local zOrderTab = 
{
    "Z_FILTER_LAYER",       -- 触摸过滤
    "Z_AD_WEBVIEW",         -- 首页广告
    "Z_HELP_WEBVIEW",       -- 帮助界面(网页对话框)
    "Z_HELP_BUTTON",        -- 通用帮助按钮
    "Z_TARGET_SHARE",       -- 通用分享选择层
    "Z_VOICE_BUTTON",       -- 通用语音按钮
    "Z_INVITE_DLG",         -- 邀请对话框
}
yl.GET_ZORDER_ENUM( yl.MAX_INT, -1, zOrderTab )