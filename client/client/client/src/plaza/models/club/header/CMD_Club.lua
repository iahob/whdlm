local club = {}


if club.TEST_SERVER then
	club.URL_CLUB_SHARE 					= "http://testwww.foxuc.com/Mobile/WxGroupShare.aspx"
else
	club.URL_CLUB_SHARE 					= "http://phone.foxuc.com/Mobile/WxGroupShare.aspx"
end

club.SUB_MB_LIST_ACCESS 					= 108 							--网关服务

club.LEN_CREATE_OPTION_ITEM				= 93+40+16+2


club.LEN_ACCESS_ITEM						= 68

--大联盟权限
club.AUTHORITY_MEMBER          =0x00        --普通大联盟成员
club.AUTHORITY_OWNER           =0x01        --圈主
club.AUTHORITY_MANAGER     =0x02        --管理员
club.AUTHORITY_CAPTAIN         =0x04        --队长
club.AUTHORITY_TEAMMEMBER      =0x08        --队员

--管理员权限
club.MANAGER_RIGHT1 = 0x01--冻结大联盟
club.MANAGER_RIGHT2 = 0x02--进出设置
club.MANAGER_RIGHT3 = 0x04--设置公告
club.MANAGER_RIGHT4 = 0x08--设置跑马灯
club.MANAGER_RIGHT5 = 0x10--用户设置
club.MANAGER_RIGHT6 = 0x20--勋章模式设置
club.MANAGER_RIGHT7 = 0x40--玩法管理
club.MANAGER_RIGHT8 = 0x80--点赞标记
club.MANAGER_RIGHT9 = 0x100--队长权限
club.MANAGER_RIGHT10 = 0x200--添加队长
club.MANAGER_RIGHT11 = 0x400--解除队长
club.MANAGER_RIGHT12 = 0x800--解除队员
club.MANAGER_RIGHT13 = 0x1000--调配队员
club.MANAGER_RIGHT14 = 0x2000--大赢家设置


club.CAPTAIN_RIGHT1 = 0x01--勋章开关
club.CAPTAIN_RIGHT2 = 0x02--勋章显示
club.CAPTAIN_RIGHT3 = 0x04--勋章修改
club.CAPTAIN_RIGHT4 = 0x08--点赞标记
club.CAPTAIN_RIGHT5 = 0x10--解除队员
club.CAPTAIN_RIGHT6 = 0x20--添加队员
club.CAPTAIN_RIGHT7 = 0x40--要求队员
club.CAPTAIN_RIGHT8 = 0x80--发送广播


--接入子项
club.tagAccessItem = 
{
    -- 接入标识
    {t = "word", k = "wAccessID"},
    -- 服务端口
    {t = "word", k = "wServicePort"},
    -- 服务地址 
    {t = "string", k = "szServiceAddr", s = 32},
}

----------------------------------------------------------------------------------

club.LEN_GROUP_NAME				= 32									--社团名字
club.LEN_GROUP_INTRODUCE			= 64									--群组介绍

----------------------------------------------------------------------------------
--IM定义
--更新类型
club.UPDATE_KIND_WEALTH			= 0x01									--财富类型
club.UPDATE_KIND_PROPERTY			= 0x02									--属性类型
club.UPDATE_KIND_BATTLERULE		= 0x04									--规则类型

--消息类型
club.APPLY_MSG_TYPE_REQUEST		= 1										--请求消息
club.APPLY_MSG_TYPE_RESPOND		= 2										--响应消息
club.APPLY_MSG_TYPE_DELETE      = 3                                      --消息删除（只有群主会受到）

--申请状态
club.APPLY_STATUS_NONE			= 0										--审核状态
club.APPLY_STATUS_AGREE			= 1										--同意状态
club.APPLY_STATUS_REFUSE			= 2										--拒绝状态
club.APPLY_STATUS_REPEAL			= 3										--被删除

club.GROUP_BATTLE_MASK_GYKF		= 0x01                                  --馆员开房


--桌子用户
club.tagTableUserItem = 
{
	 -- 头像标识 2
    {t = "word", k = "wFaceID"},
    -- 用户方位 2
    {t = "word", k = "wFaceID"},
    --用户标识 4
    {t = "dword", k = "dwUserID"},
    -- 游戏标识 4
    {t = "dword", k = "dwGameID"},
   -- 头像标识 4
    {t = "dword", k = "dwCustomID"},
     -- 用户昵称 
    {t = "string", k = "szNickName", s = yl.LEN_NICKNAME},
}

----------------------------------------------------------------------------------
--IM结构定义

--网络信息
club.tagTableUserItem = 
{
	 -- 网络标识
    {t = "word", k = "wAccessID"},
    -- 令牌标识
    {t = "dword", k = "dwTokenID"},
}

--群组约战配置
club.tagIMGroupOption = 
{
	 -- 网络标识
    {t = "dword", k = "dwStationID"},
    -- 支付掩码
    {t = "byte", k = "cbPayTypeMask"},
    -- 货币掩码
    {t = "byte", k = "cbCurrencyKindMask"},
}


--群组财富
club.tagIMGroupWealth = 
{
	 -- 剩余钻石
    {t = "score", k = "lIngot"},
    -- 剩余房卡
    {t = "score", k = "lRoomCard"},
    --勋章
    {t = "score", k = "lScore"}
}


--创建时间信息
club.DateTime = 
{
	--时间
    {t = "word", k = "wYear"},
    {t = "word", k = "wMonth"},
    {t = "word", k = "wDayOfWeek"},
    {t = "word", k = "wDay"},
    {t = "word", k = "wHour"},
    {t = "word", k = "wMinute"},
    {t = "word", k = "wSecond"},
    {t = "word", k = "wMilliseconds"},
}

--群组属性
club.tagIMGroupProperty = 
{
	 -- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 群主标识
    {t = "dword", k = "dwCreaterID"},
     -- 群组标识
    {t = "dword", k = "dwCreaterGameID"},
      -- 群组级别
    {t = "word", k = "wGroupLevel"},
    -- 群组名称 
    {t = "string", k = "szGroupName", s = 32},
     -- 群主昵称 
    {t = "string", k = "szCreaterNickName", s = yl.LEN_ACCOUNTS},

    --成员信息
    -- 成员数量
    {t = "word", k = "wMemberCount"},
     -- 成员最大数量
    {t = "word", k = "wMaxMemberCount"},
    --时间信息
     {t = "table", k = "CreateDateTime", d = club.DateTime},
     -- 群组介绍 
    {t = "string", k = "szGroupIntroduce", s = club.LEN_GROUP_INTRODUCE},

    {t = "byte", k = "cbGroupStatus"},--群组状态0已删除，1生效，2冻结
}

club.tagIMGroupExtraInfo = 
{
     -- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 玩家标识
    {t = "dword", k = "dwUserID"},

    {t = "int", k = "nMemberRight"},

    {t = "score", k = "lInvitationCode"},   --邀请码
     
    {t = "byte", k = "cbGroupShow"},     --0不显示，1显示   

    {t = "string", k = "szQQ", s = yl.LEN_NICKNAME},

    {t = "string", k = "szWX", s = yl.LEN_NICKNAME},

    {t = "string", k = "szPhone", s = 12},

}



--群组规则
club.tagIMGroupBattleRule = 
{
	--支付类型
	{t = "byte", k = "cbPayTypeMask"},
	--货币类型
	{t = "byte", k = "cbCurrencyKind"},
	--约战掩码
	{t = "byte", k = "cbBattleMask"},

            {t = "score", k = "lManagerRight"}, --管理员权限

            {t = "score", k = "lCaptainRight"}, --队长权限

            {t = "byte", k = "cbMedalMode"}, --勋章模式0关闭，1开启

            {t = "byte", k = "cbMedalShow"},--勋章栏显示0不显示，1显示

            {t = "byte", k = "cbCurMedalShow"},--当前勋章显示0不显示，1显示

            {t = "byte", k = "cbMedalRevenueShow"},--勋章损耗显示0不显示，1显示

            {t = "byte", k = "cbMedalClear"},--勋章清零0不清零，1零点清零

            {t = "byte", k = "cbInMode"},--进入规则0无需审核，1审核

            {t = "byte", k = "cbOutMode"},--退出规则0无需审核，1审核

            {t = "byte", k = "cbFreeTableLimit"},--空座限制

            {t = "byte", k = "cbBigWinnerShow"},--0不显示，1显示
            
            {t = "byte", k = "cbGroupIDShow"},--0不显(邀请码)，1显示 (大联盟ID)

}


--约战玩法
club.tagIMBattleConfig = 
{
	-- 类型标识
    {t = "word", k = "wKindID"},
    -- 约战配置 
    {t = "string", k = "szBattleConfig", s = 256},
}


--群组子项
club.tagIMGroupItem = 
{
	--群组财富
    {t = "table", k = "GroupWealth", d = club.tagIMGroupWealth},
	-- 类型标识
    {t = "table", k = "GroupProperty", d = club.tagIMGroupProperty},
    -- 约战配置 
    {t = "table", k = "GroupBattleRule", d = club.tagIMGroupBattleRule},
}

--用户信息78
club.tagIMUserInfo = 
{
	-- 用户 I D
    {t = "dword", k = "dwUserID"},
    -- 约战配置 
    {t = "dword", k = "dwGameID"},
    -- 用户性别 
    {t = "byte", k = "cbGender"},
    -- 用户状态 
    {t = "byte", k = "cbUserStatus"},
     -- 玩家昵称 
    {t = "string", k = "szNickName", s = yl.LEN_NICKNAME},
     -- 头像标识 4
    {t = "dword", k = "dwCustomID"},
}



--群组成员22
club.tagIMGroupMember = 
{
	-- 成员标识
    {t = "dword", k = "dwMemberID"},
    -- 约战场次
    {t = "dword", k = "wBattleCount"},
     --时间信息
    {t = "table", k = "JoinDateTime", d = club.DateTime},

    {t = "score", k = "lScore"}, --勋章

    {t = "int", k = "nMemberRight"}, --权限

    {t = "dword", k = "dwCaptainID"}, --队长ID

    {t = "byte", k = "cbStatus"},  --0，离线；1，在线 状态

    {t = "dword", k = "dwFavoriteConfig"},  --设置的玩法

    {t = "byte", k = "cbManagerRightFlag"},  --是否单独设置管理员权限

    {t = "score", k = "lMemberManagerRight"}, --成员管理员权限

    {t = "int", k = "nAwardPoint"}, --队长点位

    {t = "score", k = "lInvitationCode"}, --邀请码


    {t = "string", k = "szQQ", s = yl.LEN_NICKNAME},

    {t = "string", k = "szWX", s = yl.LEN_NICKNAME},

    {t = "string", k = "szPhone", s = 12},


}



--群组成员100
club.tagIMGroupMemberUser =
{
	-- 成员标识
    {t = "table", k = "UserInfo", d = club.tagIMUserInfo},
    -- 成员信息
    {t = "table", k = "MemberInfo", d = club.tagIMGroupMember},
}


--申请消息
club.tagIMApplyMessage =
{
	-- 成员标识
    {t = "dword", k = "dwMessageID"},
   	-- 消息状态
    {t = "byte", k = "cbMessageStatus"},
    
    {t = "byte", k = "cbMessageType"},--消息类型 0加入，1退出

   	-- 用户标识
    {t = "dword", k = "dwApplyerID"},
    -- 游戏标识
    {t = "dword", k = "dwApplyerGameID"},
   	-- 头像标识
    {t = "dword", k = "dwApplyerCustomID"},
    -- 用户昵称 
    {t = "string", k = "szApplyerNickName", s = yl.LEN_NICKNAME},
    --时间信息
    {t = "table", k = "ApplyDateTime", d = club.DateTime},
    -- 群组ID
    {t = "dword", k = "dwGroupID"},
     -- 群主ID
    {t = "dword", k = "dwCreaterID"},
    -- 群组名称 
    {t = "string", k = "szGroupName", s = club.LEN_GROUP_NAME},
}
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------

--逻辑服务
club.MDM_GA_LOGIC_SERVICE			= 2									    --逻辑服务

--请求命令
club.SUB_GA_LOGON_SERVER			= 1									    --登录服务
club.SUB_GA_SEARCH_GROUP			= 2									    --搜索群组
club.SUB_GA_CREATE_GROUP			= 3									    --创建群组
club.SUB_GA_UPDATE_GROUP			= 4									    --更新群组
club.SUB_GA_DELETE_GROUP			= 5									    --删除群组
club.SUB_GA_DELETE_MEMBER			= 7									    --删除成员		
club.SUB_GA_APPLY_REQUEST			= 8									    --申请请求
club.SUB_GA_APPLY_RESPOND			= 9									    --申请响应
club.SUB_GA_APPLY_DELETE			= 10									--申请删除
club.SUB_GA_APPLY_REPEAL            = 11									--申请撤销
club.SUB_GA_SETTLE_BATTLE			= 12									--约战结算
club.SUB_GA_QUERY_BATTLE            = 13                                    --请求约战详情
club.SUB_GA_QUERY_TOTAL_BATTLE      = 14                                    --请求约战总数

club.SUB_GA_PRESENT_SCORE           = 15                            --赠送勋章
club.SUB_GA_QUERY_PRESENT           = 16                            --查询赠送

club.SUB_GA_APPEND_CONFIG           = 18                            --创建玩法
club.SUB_GA_MODIFY_CONFIG           = 19                            --修改玩法


club.SUB_GA_DELETE_CONFIG           = 20                                    --删除玩法

club.SUB_GA_MODIFY_RIGHT           = 21									--修改权限

club.SUB_GA_QUERY_CREATE           = 22									--查询创建

club.SUB_GA_QUERY_MEDALMODE = 23                                                                                                        --查询勋章模式

club.SUB_GA_QUERY_DAYRECORD    =   24                                                                                                   --查询日统计
    
club.SUB_GA_SET_ROLLMSG = 25                                                                                                                    --设置跑马灯

 club.SUB_GA_EDIT_NOTICE = 26                                                                                                                    --设置公告

 club.SUB_GA_SET_GROUP_STATUS = 27                                                                                                        --设置大联盟状态

 club.SUB_GA_SET_INOUT = 28                                                                                                                            --进出设置

 club.SUB_GA_RIGHT_MANAGEMENT = 30                                                                                                         --权限管理

 club.SUB_GA_SET_MEDAL = 31                                                                                                                           --勋章设置

 club.SUB_GA_SET_FREE_TABLE = 32                                                                                                                           --空桌设置

 club.SUB_GA_SET_BIGWINNER_SHOW = 33                                                                                                    --大赢家设置

 club.SUB_GA_QUERY_USER_DETAIL = 34                                                                                                    --查询玩家一天详情

club.SUB_GA_QUERY_GROUP_DETAIL = 35                                                                                                    --查询大联盟单日详情

club.SUB_GA_SET_FLAG = 36                                                                                                    --设置点赞

club.SUB_GA_DISMISS_CLUB_TABLE = 37                                                                                                    --解散大联盟桌子

club.SUB_GA_SET_FAVORITE_CONFIG = 38                                                                                                    --设置快速开始玩法

club.SUB_GA_MODIFY_GROUP_NAME = 39                                                                                                    --修改大联盟名称

club.SUB_GA_QUERY_ROOM_DETAIL = 40                                                                                                    --对局详情

club.SUB_GA_QUERY_CAPTAIN_LIST = 41                                                                                                    --查询队长列表

club.SUB_GA_ADD_CAPTAIN_TEAMER = 42                                                                                                    --调配队员

club.SUB_GA_QUERY_TEAM_DAY_RECORD = 43                                                                                                    --查询小队日统计

club.SUB_GA_QUERY_TEAMER_RECORD = 44                                                                                                    --查询小队成员日统计

club.SUB_GA_SET_PRIVACY = 45                                                                                                                     --隐私设置

club.SUB_GA_SET_CAPTAIN_POINT = 46                                                                                                    --设置队长点位

club.SUB_GA_OPEN_BOX = 47                                                                                                    --打开积分宝箱

club.SUB_GA_GET_AWARDSCORE = 48                                                                                                    --提取积分

club.SUB_GA_QUERY_INVITATION_CODE = 49                                                                                                    --查询邀请码

club.SUB_GA_BLIND_INVITATION_CODE = 50                                                                                                    --绑定邀请码

club.SUB_GA_QUERY_SCORE = 51                                                                                                   --查询可分配勋章

club.SUB_GA_QUERY_ROOM_BATTLE_INFO = 52                                                                         --查询桌子对局信息

club.SUB_GA_MODIFY_INFO = 53                                                                         --微信资料修改

--应答操作
club.SUB_GA_GROUP_OPTION			= 100									--群组配置
club.SUB_GA_APPLY_MESSAGE			= 101									--申请消息
club.SUB_GA_LOGON_FAILURE			= 102									--登录失败
club.SUB_GA_SEARCH_RESULT		    = 103									--搜索结果
club.SUB_GA_WEALTH_UPDATE			= 104									--财富更新
club.SUB_GA_APPLY_DEL_RESULT		= 105 									--删除结果
club.SUB_GA_QUERY_BATTLE_RESULT     = 106                                   --请求约战详情结果
club.SUB_GA_SETTLE_BATTLE_RESULT    = 107                                   --设置统计结果
club.SUB_GA_TOTAL_RESULT            = 108                                   --请求约战总数结果
club.SUB_GA_PRESENT_SUCCESS            = 109                                   --赠送成功

club.SUB_GA_QUERY_PRESENT_LIST            = 110                                   --赠送列表
club.SUB_GA_QUERY_CREATE_RESULT            = 112                                   --创建查询结果

club.SUB_GA_QUERY_DAYRECORD_LIST            = 113                                   --日统计结果，大联盟所有成员

club.SUB_GA_MEDAL_MODE            = 114                                  --勋章模式

club.SUB_GA_DAY_DETAIL_LIST            = 116                                  --日详情列表，自己的

club.SUB_GA_GROUP_DAY_DETAIL_LIST            = 117                                  --大联盟单日战绩

club.SUB_GA_FAVORITE_CONFIG            = 118                                  --收藏玩法

club.SUB_GA_ROOM_DETAIL_INFO            = 119                                  --房间详情

club.SUB_GA_CAPTAIN_LIST            = 120                                  --队长列表

club.SUB_GA_TEAM_DAY_RECORD            = 121                                  --小队日统计

club.SUB_GA_TEAMER_DAY_RECORD            = 122                                  --小队成员日统计

club.SUB_GA_SET_CAPTAIN_POINT_RESULT = 123                                     --队长点位

club.SUB_GA_BOX_INFO = 124                                     --积分宝箱

club.SUB_GA_GETBOXSCORE_RESULT = 125                                     --提取结果

club.SUB_GA_INVITATION_CODE = 126                                     --查询邀请码

club.SUB_GA_BLIND_CODE_RES = 127                                     --绑定结果

club.SUB_GA_QUERY_SCORE_RES = 128                                     --查询俱乐部可分配勋章

club.SUB_GA_QUERY_TABLE_RES = 129                                     --查询对局信息结果

--操作结果
club.SUB_GA_OPERATE_SUCCESS		= 200									--操作成功
club.SUB_GA_OPERATE_FAILURE		= 201									--操作失败	

club.SUB_GA_SYSTEM_MESSAGE		= 300									--系统消息

----------------------------------------------------------------------------------
--登录服务
club.SUB_GA_LogonServer = 
{
	-- 成员标识
    {t = "dword", k = "dwUserID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
}

--查询群组 消息头
club.SUB_GA_QueryGroup = 
{
	-- 成员标识
    {t = "dword", k = "dwUserID"},
   -- 成员标识
    {t = "word", k = "wGroupCount"},
}

--搜索群组
club.SUB_GA_SearchGroup = 
{
	-- 成员标识
    {t = "dword", k = "dwUserID"},
   -- 成员标识
    {t = "dword", k = "dwGroupID"},
    --dwGroupID为((DWORD)(0xFFFFFFFF))时服务器会发送10个推荐群组其他值就是去查找对应的群组
}

--大联盟搜索结果
club.SUB_GA_S_SearchResult = 
{
	-- 群组数量
    {t = "word", k = "wGroupCount"},
    -- .....后面接  tagIMGroupProperty
}

--创建群组
club.SUB_GA_CreateGroup = 
{
	-- 支付方式
    {t = "byte", k = "cbPayType"},
   -- 货币
    {t = "byte", k = "cbCurrencyKind"},
    -- 约战
    {t = "byte", k = "cbBattleMask"},
    -- 用户信息
    {t = "dword", k = "dwCreateID"},
    -- 站点标识
    {t = "dword", k = "dwStationID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
    -- 大联盟名字 
    {t = "tchar", k = "szGroupName", s = club.LEN_GROUP_NAME},
     -- 登录密码 
    {t = "tchar", k = "szGroupIntroduce", s = club.LEN_GROUP_INTRODUCE},
}

--创建成功
club.SUB_GA_OperateFailure = 
{
	-- 操作代码
    {t = "int", k = "lErrorCode"},
    -- 登录密码 
    {t = "string", k = "szErrorDescribe", s = 128},
}

--创建成功
club.SUB_GA_GroupProperty = 
{
	-- 群信息
    {t = "table", k = "GroupProperty", d = club.tagIMGroupProperty},
}

--财富更新
club.SUB_GA_WealthUpdate = 
{
	--用户钻石
	{t = "score", k = "lUserIngot"},
	--用户房卡
	{t = "score", k = "lUserRoomCard"},
}

--操作成功
club.SUB_GA_OperateSuccess = 
{
	-- 操作代码
    {t = "int", k = "lOperateCode"},
    -- 登录密码 
    {t = "string", k = "szOperateNotice", s = 128},
}

--更新群组
club.SUB_GA_UpdateGroup = 
{
	-- 群组标识
    {t = "dword", k = "dwGroupID"},
	-- 站点标识
    {t = "dword", k = "dwStationID"},
   	-- 更新信息
    {t = "byte", k = "cbUpdateKind"},
   	-- 用户信息
    {t = "dword", k = "dwCreateID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
}


--赠送勋章
club.SUB_GA_PresentScore = 
{
   
    {t = "dword", k = "dwGroupID"}, -- 群组标识
    
    {t = "dword", k = "dwPresentUserID"},-- 赠送玩家
    
    {t = "byte", k = "dwReceiveUserID"},-- 接收玩家

    {t = "score", k = "lPresentScore"},    -- 赠送数目

    {t = "byte", k = "cbType"},    -- 0赠送，1消耗

    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},    -- 登录密码
}

--赠送成功
club.SUB_GA_PresentSuccess = 
{
   
    {t = "dword", k = "dwPresentUserID"},-- 赠送玩家
    
    {t = "byte", k = "cbSuccess"},-- 接收玩家

    {t = "score", k = "lCurScore"},    -- 赠送数目

}


--查询赠送记录
club.SUB_GA_QueryPresent = 
{
   
    {t = "dword", k = "dwGroupID"}, -- 群组标识
    
    {t = "dword", k = "dwQueryUserID"},-- 查询玩家
    

}


--查询赠送记录
club.SUB_GA_Present_List = 
{
   
    {t = "dword", k = "dwGroupID"}, -- 群组标识
    
    {t = "int", k = "nCount"},-- 查询玩家

    {t = "bool", k = "bListEnd"},    -- 是否结束
    

}

--赠送记录列表
club.tagIMPresentItem = 
{

    {t = "dword", k = "dwGroupID"},--大联盟标识

    {t = "dword", k = "dwPresentID"},--操作玩家

    {t = "dword", k = "dwPresentGameID"},--操作玩家游戏ID

    {t = "string", k = "szPresentNickName",s = yl.LEN_NICKNAME},--操作玩家昵称

    {t = "byte", k = "cbPresentUserRight"},    -- 操作玩家身份1圈主，2管理员

    {t = "dword", k = "dwReceiveID"},--接受玩家ID

    {t = "dword", k = "dwReceiveGameID"}, --接受玩家游戏ID

    {t = "string", k = "szReceiveNickName",s = yl.LEN_NICKNAME},--接收玩家昵称

    {t = "score", k = "lChangeScore"},  --分数变化值

    {t = "score", k = "lCurScore"},  --变化后分数

    {t = "byte", k = "cbType"},    -- 0赠送，1消耗

    {t = "table", k = "CollectDateTime", d = club.DateTime}, --时间

}



--修改玩法
club.SUB_GA_ModifyConfig = 
{
 

    {t = "dword", k = "dwUserID"}, --用户标识

    {t = "dword", k = "dwGroupID"},  --大联盟ID

    {t = "dword", k = "dwConfigID"},  --玩法ID   

    {t = "tchar", k = "szConfigName",s = 13},--玩法名

   -- {t = "word", k = "wKindID"},--游戏ID

    {t = "byte", k = "cbPlayMode"},--结算模式0积分，1勋章

    {t = "score", k = "lCellScore"}, --底分设置

    {t = "dword", k = "dwDrawCountLimit"},  --局数设置

    {t = "dword", k = "dwDrawTimeLimit"}, --时间设置

    {t = "word", k = "wJoinGamePeopleCount"}, --参与游戏的人数

    --{t = "dword", k = "dwRoomTax"}, --

    {t = "byte", k = "cbPassword"}, --密码设置 0:无密码 1:需要密码

    {t = "byte", k = "cbPayMode"},--支付方式

    {t = "byte", k = "cbGameRule",l=100}, --游戏额外规则

    {t = "byte", k = "cbGameMode"},  --游戏模式

    {t = "score", k = "lEnterScore"}, --进入房间的分数下限

    {t = "score", k = "lLeaveScore"}, --离开房间的分数下限

   -- {t = "byte", k = "cbJoinMode"}, --加入模式

    {t = "byte", k = "cbTaxMode"}, --税收模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家

    {t = "dword", k = "dwTaxCount"}, --税收数值

    {t = "byte", k = "cbLessScoreDismiss"}, --分数不足解散

}



club.SUB_GA_QueryDayRecord = 
{
    {t = "dword", k = "dwGroupID"}, --大联盟ID

    {t = "dword", k = "dwQueryUserID"},  --用户标识

    {t = "dword", k = "dwDateID"},  --玩法ID   

}

--大联盟成员
club.tagIMMemberDayRecord = 
{
    {t = "dword", k = "dwMemberID"}, --成员标识

    {t = "int", k = "nPlayCount"},  --游戏局数

    {t = "int", k = "nBigWinnerCount"},  --大赢家局数

    {t = "score", k = "lScore"}, --当日最后勋章

    {t = "score", k = "lWinScore"},  --当日输赢分

    {t = "score", k = "lRevenue"},  --当时贡献税收

    {t = "byte", k = "cbFlag"},  --点赞   
}


--玩家单日战绩
club.SUB_GA_QueryMyDayRecord = 
{
    {t = "dword", k = "dwGroupID"}, --大联盟ID

    {t = "dword", k = "dwUserID"},  --用户标识

    {t = "dword", k = "dwGameID"},  

    {t = "string", k = "szNickName",s = 32},--玩家昵称

    {t = "int", k = "nPlayCount"},  --局数

    {t = "int", k = "nBigWinnerCount"},  --大赢家数量  

    {t = "score", k = "lCurScore"}, --当前分数

    {t = "score", k = "lGameScore"},  --输赢分数

    {t = "int", k = "nCount"},  --数量  

    {t = "bool", k = "bListEnd"},  --数量  

}


club.tagIMMyDayRecord = 
{
    {t = "dword", k = "dwRoomID"}, --房间标识

    {t = "byte", k = "cbPlayMode"}, --房间类型

    {t = "string", k = "szConfigName",s = 32},--房间名称

    {t = "int", k = "nDrawCount"},  --局数

    {t = "dword", k = "dwBigWinnerID"},  --大赢家ID

    {t = "string", k = "szBigWinnerName",s = 32},--大赢家名称

    {t = "score", k = "lScore"}, --得分

     {t = "table", k = "EndTime", d = club.DateTime}, --游戏结束时间
}

--大联盟单日战绩
club.SUB_GA_QueryClubDayRecord = 
{
    {t = "dword", k = "dwGroupID"}, --大联盟ID

    {t = "int", k = "nCount"},  --数量  

    {t = "int", k = "nDiamond"},  --消耗钻石

    {t = "score", k = "lAllRoomRevenue"}, --贡献

    {t = "bool", k = "bListEnd"},  --结束  

}


club.tagIMClubDayRecord = 
{
    {t = "dword", k = "dwRoomID"}, --房间标识

    {t = "byte", k = "cbPlayMode"}, --房间类型

    {t = "string", k = "szConfigName",s = 32},--房间名称

    {t = "int", k = "nDrawCount"},  --局数

    {t = "dword", k = "dwBigWinnerID"},  --大赢家ID

    {t = "string", k = "szBigWinnerName",s = 32},--大赢家名称

    {t = "score", k = "lDiamond"}, --钻石消耗

    {t = "score", k = "lRoomRevenue"}, --贡献

    {t = "table", k = "EndTime", d = club.DateTime}, --游戏结束时间
}


--房间详情
club.SUB_GA_QueryRoomDetail_List = 
{
    {t = "dword", k = "dwRoomID"}, --房间标识

    {t = "string", k = "szKindName",s = 32},--游戏名称

    {t = "score", k = "lMinTableScore"},  --最底入场分

    {t = "int", k = "nDrawCount"},  --总局数

    {t = "byte", k = "cbPlayerCount"},  --玩家人数

    {t = "string", k = "szNickName",s = 32,l ={20}},--玩家昵称

    {t = "score", k = "lAllScore",l={20}}, --总分

    {t = "int", k = "nListCount"},  

    {t = "bool", k = "bListEnd"},  

}

club.tagIMRoomTurnDetail = 
{
    {t = "int", k = "nTurnIndex"},  --局数

    {t = "table", k = "StartTime", d = club.DateTime}, --游戏结束时间

    {t = "score", k = "lTurnScore",l={20}}, --总分  

    {t = "bool", k = "bFlag",l={20}}, --是否参与
}

--队长列表总数
club.SUB_GA_CaptainTotalData = 
{
    {t = "dword", k = "dwGroupID"},  --大联盟ID

    {t = "int", k = "nCaptainCount"}, --下属队长数量

    {t = "int", k = "nDrawCount"}, --总局数

    {t = "int", k = "nBigWinnerCount"}, --大赢家总数

    {t = "score", k = "lRevenueCount"}, --产生税收累计

    {t = "score", k = "lContributionCount"}, --贡献累计

    {t = "int", k = "nCount"}, --数量

    {t = "bool", k = "bListEnd"}, --结束

}

--队长列表
club.tagIMCaptainList = 
{
    {t = "dword", k = "dwUserID"},  --成员标识

    {t = "dword", k = "dwGameID"},  --队长GameID

    {t = "string", k = "szNickName",s = 32},--队长名称

    {t = "int", k = "nPlayCount"}, --局数

    {t = "int", k = "nBigWinnerCount"}, --大赢家次数

    {t = "score", k = "lRevenue"}, --当日输赢分

    {t = "score", k = "lContribution"}, --当日贡献税收

    {t = "byte", k = "cbFlag"}, --点赞

}

--所有小队日统计
club.tagTeamDayRecord = 
{
    {t = "dword", k = "dwMemberID"},  --成员标识

    {t = "int", k = "nPlayCount"}, --局数

    {t = "int", k = "nBigWinnerCount"}, --大赢家次数

    {t = "score", k = "lScore"}, --当日最后勋章

    {t = "score", k = "lWinScore"}, --当日输赢分

    {t = "score", k = "lRevenue"}, --当日贡献税收

}


--小队成员日统计
club.tagTeamerDayRecord = 
{
    {t = "dword", k = "dwMemberID"},  --成员标识

    {t = "dword", k = "dwGameID"},  --队长GameID

    {t = "string", k = "szNickName",s = 32},--队长名称

    {t = "int", k = "nPlayCount"}, --局数

    {t = "int", k = "nBigWinnerCount"}, --大赢家次数

    {t = "score", k = "lRevenue"}, --当日产生税收

    {t = "score", k = "lContribution"}, --当日给上级产生的贡献值

    {t = "byte", k = "cbFlag"}, --点赞

}


club.SUB_GA_BoxInfo = 
{
    {t = "dword", k = "dwGroupID"},  --成员标识

    {t = "dword", k = "dwUserID"},  --玩家ID

    {t = "word", k = "wPoint"},  --点位

    {t = "score", k = "lAwardScore"}, --奖励分

    {t = "score", k = "lTodayGetScore"}, --今日已提取分

    {t = "int", k = "nCount"}, --提取记录数量  

    {t = "bool", k = "bListEnd"}, --结束

}


club.tagIMAwardBoxRecord = 
{

    {t = "score", k = "lBeforeScore"}, --提取前

    {t = "score", k = "lScore"}, --提取额度

    {t = "score", k = "lAfterScore"}, --提取后

    {t = "table", k = "DateTime", d = club.DateTime}, --提取时间

}

--查询邀请码结果
club.SUB_GA_InvitationCode = 
{

    {t = "dword", k = "dwGroupID"}, 

    {t = "dword", k = "dwUserID"}, 

    {t = "score", k = "lInvitationCode"}, --邀请码


}

club.SUB_GA_QueryScoreResult = 
{

    {t = "dword", k = "dwGroupID"}, 

    {t = "dword", k = "dwUserID"}, 

    {t = "score", k = "lScore"}, --可用分


}


club.SUB_GA_QueryTableBattleResult = 
{

    {t = "dword", k = "dwGroupID"}, --群组标识

    {t = "dword", k = "dwRoomID"}, --房间ID

    {t = "dword", k = "dwAllCount"}, --总局数

    {t = "dword", k = "dwPlayCount"}, --当前局数

    {t = "dword", k = "dwUserID", l = {20}}, --玩家Id

    {t = "score", k = "lGameScore", l = {20}}, --玩家当前输赢

}



--更新类型
club.UPGROUP_KIND_WEALTH			=0x01								--财富类型
club.UPGROUP_KIND_PROPERTY			=0x02								--属性类型
club.UPGROUP_KIND_BATTLERULE		=0x04								--规则类型

club.SUB_GA_GroupUpdate =
{
	-- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 更新信息
    {t = "byte", k = "cbUpdateKind"},
}

--组包（根据类型分别读取）
-- (cbUpdateKind ==UPGROUP_KIND_WEALTH)tagIMGroupWealth
-- (cbUpdateKind==UPGROUP_KIND_PROPERTY) tagIMGroupProperty
-- (cbUpdateKind==UPGROUP_KIND_BATTLERULE) tagIMGroupBattleRule

club.SUB_GA_DeleteGroup = 
{
	-- 群组标识
    {t = "dword", k = "dwGroupID"},
	-- 站点标识
    {t = "dword", k = "dwStationID"},
   	-- 用户信息
    {t = "dword", k = "dwCreateID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
}

club.SUB_GA_GroupDelete = 
{
	-- 群组标识
    {t = "dword", k = "dwGroupID"},
}

--删除成员
club.SUB_GA_DeleteMember = 
{
		-- 群组标识
    {t = "dword", k = "dwGroupID"},
	-- 成员信息
    {t = "dword", k = "dwMemberID"},
   	-- 用户信息
    {t = "dword", k = "dwCreateID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
}

--删除成功
club.SUB_GA_MemberDelete = 
{
			-- 群组标识
    {t = "dword", k = "dwGroupID"},
	-- 成员信息
    {t = "dword", k = "dwUserID"},

    {t = "byte", k = "cbByte"},--0申请退出，1，被删除
    -- 用户信息
    {t = "int", k = "nCount"},
}

--申请请求
club.SUB_GA_ApplyRequest = 
{
	-- 群组标识
    {t = "dword", k = "dwGroupID"},
	-- 用户标识
    {t = "dword", k = "dwApplyerID"},   

    {t = "byte", k = "cbApplyType"},  --申请类型0加入，1退出
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},

    {t = "score", k = "lInvitationCode"},
}
--成功回（群主和申请人都收到）SUB_GA_APPLY_MESSAGE 消息结构 头 SUB_GA_ApplyMessage 组包 tagIMApplyMessage

--申请响应
club.SUB_GA_ApplyRespond = 
{
	-- 消息标识
    {t = "dword", k = "dwMessageID"},
    -- 用户标识
    {t = "dword", k = "dwCreaterID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
    -- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 用户标识	
    {t = "dword", k = "dwApplyerID"},
     -- 申请状态	
    {t = "byte", k = "cbApplyStatus"},
}
-- 成功回（群主和申请人都收到）
-- SUB_GA_APPLY_MESSAGE
-- 消息结构
-- 头SUB_GA_ApplyMessage
-- 组包tagIMApplyMessage
-- 回
-- SUB_GA_OPERATE_SUCCESS
-- 操作成功SUB_GA_OperateSuccess

--添加成员
club.SUB_GA_MemberInsert = 
{
	-- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 群组标识
    {t = "word", k = "wMemberCount"},
    --成员
    {t = "table", k = "MemberUser", d = club.tagIMGroupMemberUser},
}



--申请删除
club.SUB_GA_ApplyDelete = 
{
	-- 消息标识
    {t = "dword", k = "dwMessageID"},
    -- 消息类型	
    {t = "byte", k = "cbMessageType"},
    -- 用户标识
    {t = "dword", k = "dwCreaterID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
}

--删除结果
club.SUB_GA_ApplyDelResult = 
{
	-- 消息标识
    {t = "dword", k = "dwMessageID"},
    -- 消息类型	
    {t = "byte", k = "cbMessageType"},
     -- 消息类型	
    {t = "bool", k = "bDelResult"},
    --错误信息
    {t = "string", k = "szErrorDescribe", s = 128},
}

club.SUB_GA_QueryBattle = 
{
    -- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 用户信息
    {t = "dword", k = "dwCreateID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
}

--约战信息
club.SUB_GA_BattleResult = 
{
    -- 创建数目
    {t = "dword", k = "dwCreateCount"},
    -- 消耗数目
    {t = "score", k = "lConsumeIngot"},
      -- 信息数目
    {t = "dword", k = "dwRecordCount"},
}

--详情
club.tagClubSettleBattleInfo = 
{
    -- 用户ID
    {t = "dword", k = "dwUserID"},
    -- 创建数目
    {t = "dword", k = "dwBattleCount"},
    -- 统计次数
    {t = "dword", k = "dwSettleCount"},
}

club.SUB_GA_SetSettle = 
{
    -- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 群组标识
    {t = "dword", k = "dwOpUserID"},
    -- 群组标识
    {t = "dword", k = "wSetNum"},
    -- 用户信息
    {t = "dword", k = "dwCreateID"},
    -- 登录密码 
    {t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
}





----------------------------------------------------------------------------------

--群组服务
club.MDM_GA_GROUP_SERVICE			= 3										--群组服务

--请求命令
club.SUB_GA_ENTER_GROUP			= 1										--进入群组
club.SUB_GA_LEAVE_GROUP			= 2										--离开群组

--应答命令
club.SUB_GA_GROUP_ITEM			       = 100									--群组信息
club.SUB_GA_GROUP_PROPERTY 		    = 101									--群组属性
club.SUB_GA_GROUP_MEMBER			= 102									--群组成员
club.SUB_GA_GROUP_UPDATE			= 103									--群组更新
club.SUB_GA_GROUP_DELETE			= 104									--群组移除
club.SUB_GA_MEMBER_INSERT			= 105									--添加成员
club.SUB_GA_MEMBER_DELETE			= 106									--删除成员
club.SUB_GA_MEMBER_UPDATE           = 107									--成员更新
club.SUB_GA_LOGON_SUCCESS           = 109                                       --登录成功
club.SUB_GA_QUERY_PROPERTY          = 110
club.SUB_GA_QUERY_PROPERTY_END          = 111

club.SUB_AG_GROUP_EXTRA_INFO          = 112             --更新俱乐部信息


--约战玩法
club.SUB_GA_BATTLE_CONFIG         = 120									--约战玩法
club.SUB_GA_CONFIG_APPEND         = 121									--玩法添加
club.SUB_GA_CONFIG_MODIFY         = 122									--玩法修改
club.SUB_GA_CONFIG_DELETE         = 123									--玩法删除

club.SUB_GA_RIGHT_CHANGE         = 124                                 --修改权限

club.SUB_GA_ROLL_MSG =  125      --跑马灯

club.SUB_GA_GROUP_NOTICE = 126     --公告

club.SUB_GA_GROUP_STATUS = 127     --大联盟状态

club.SUB_GA_INOUT_MODE = 128 --进出设置

club.SUB_GA_FREE_TABLE_LIMIT = 129 --空桌数量

club.SUB_GA_BIGWINNER_SHOW_MODE = 130 --大赢家设置

club.SUB_GA_SET_MANAGER_RIGHT = 131 --设置管理员权限

club.SUB_GA_XUNZHANG_MODE = 132 --勋章设置

club.SUB_GA_MEMBER_STATUS = 133 --玩家状态

club.SUB_GA_SET_FLAG_RESULT = 134 --点赞

club.SUB_GA_SET_CLEAR_MEDAL = 135 --勋章清零

club.SUB_GA_GROUP_NAME = 136 --修改大联盟名称

club.SUB_GA_ADD_TEAMER = 137 --调配队员

club.SUB_GA_GROUP_PRIVACY = 138 --大联盟隐私

club.SUB_GA_GROUP_COMMON_INFO = 139 --俱乐部公共配置


club.SUB_GA_ENTER_SUCCESS			= 200									--进入成功
club.SUB_GA_ENTER_FAILURE			= 201									--进入失败

club.SUB_GA_GETREPOND               = 9                                   --服务器回应
----------------------------------------------------------------------------------

--进入群组
club.SUB_GA_EnterGroup = 
{
	-- 群组标识
    {t = "dword", k = "dwUserID"},
	-- 群组标识
    {t = "dword", k = "dwGroupID"},
}

--进入失败
club.SUB_GA_EnterFailure = 
{
	-- 错误代码
    {t = "int", k = "lErrorCode"},
    -- 登录密码 
    {t = "string", k = "szErrorDescribe", s = 128},
}

--成功
club.SUB_GA_GroupItem = 
{
	{t = "table", k = "GroupItem", d = club.tagIMGroupItem},
}

--组包发送 
--消息头
--群组成员
club.SUB_GA_GroupMember = 
{
	--群组标识
	{t = "dword", k = "dwGroupID"},
}
--消息体 tagIMGroupMemberUser
 --SUB_GA_ENTER_SUCCESS  只发送消息 无附加消息结构


--离开群组
club.SUB_GA_LeaveGroup = 
{
	-- 群组标识
    {t = "dword", k = "dwUserID"},
	-- 群组标识
    {t = "dword", k = "dwGroupID"},
}
--不回消息 客户端主动断开链接



--玩法列表
club.tagGroupConfigInfo = 
{
 
    {t = "dword", k = "dwConfigID"}, --玩法ID

    {t = "dword", k = "dwGroupID"},  --大联盟ID

    {t = "string", k = "szConfigName",s = 13},--玩法名

    {t = "word", k = "dwKindID"},--游戏ID

    {t = "byte", k = "cbPlayMode"},--结算模式0积分，1勋章

    {t = "score", k = "lCellScore"}, --底分设置

    {t = "dword", k = "dwDrawCountLimit"},  --局数设置

    {t = "dword", k = "dwDrawTimeLimit"}, --时间设置

    {t = "word", k = "wJoinGamePeopleCount"}, --参与游戏的人数

    {t = "dword", k = "dwRoomTax"}, --

    {t = "byte", k = "cbPassword"}, --密码设置 0:无密码 1:需要密码

    {t = "byte", k = "cbPayMode"},--支付方式

    {t = "byte", k = "cbGameRule",l={100}}, --游戏额外规则

    {t = "byte", k = "cbGameMode"},  --游戏模式

    {t = "score", k = "lEnterScore"}, --进入房间的分数下限

    {t = "score", k = "lLeaveScore"}, --离开房间的分数下限

    {t = "byte", k = "cbJoinMode"}, --加入模式

    {t = "byte", k = "cbTaxMode"}, --税收模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家

    {t = "dword", k = "dwTaxCount"}, --税收数值

    {t = "byte", k = "cbLessScoreDismiss"}, --分数不足解散

    {t = "score", k = "lTimeSecond"}, --创建时间

    {t = "byte", k = "cbGroupPersonalMaxChair"}, --大联盟百人游戏(创建玩法就是创建桌子)

}


club.SUB_GA_RollMsg = 
{
 
    {t = "dword", k = "dwGroupID"}, --大联盟ID

    {t = "byte", k = "cbFlag"},  --0开启，1关闭

    {t = "int", k = "nTime"},  --播放间隔单位秒

    {t = "string", k = "szRollMsg",s = 33},--公告

    {t = "byte", k = "cbMsgType"},  --1，圈主；2，队长；3，自己为队长
}


club.SUB_GA_GroupNotice = 
{
 
    {t = "dword", k = "dwGroupID"}, --大联盟ID

    {t = "string", k = "szNotice",s = 109},--内容

}


club.SUB_GA_GroupName = 
{
 
    {t = "dword", k = "dwGroupID"}, --大联盟ID

    {t = "string", k = "szGroupName",s = club.LEN_GROUP_NAME},--大联盟名称

}


club.SUB_GA_ManagerRight = 
{
 
    {t = "dword", k = "dwGroupID"}, --大联盟ID

    {t = "dword", k = "dwTargetID"}, --目标玩家

    {t = "score", k = "lManagerRight"}, --管理员权限

    {t = "score", k = "lCaptainRight"}, --队长权限

}

club.SUB_GR_Add_Teamer = 
{
 
    {t = "dword", k = "dwGroupID"}, --大联盟ID

    {t = "dword", k = "dwUserID"}, --操作ID

    {t = "dword", k = "dwCaptainID"}, --队长ID

    {t = "dword", k = "dwMemberID"}, --成员ID

    {t = "byte", k = "cbMode"}, --0移除，1添加

}




-----------------------------------------------------------------------------------------------------------
                                            --约战房
------------------------------------------------------------------------------------------------------------

--约战服务
club.MDM_GA_BATTLE_SERVICE        = 1

club.SUB_GA_QUERY_TABLE           = 110                                 --查询桌子
club.SUB_GA_GET_TABLE_USERLIST    = 113                                 --获取列表
club.SUB_GA_TABLE_LIST            = 201                                 --桌子列表
club.SUB_GA_OPERATE_FAILED        = 202                                 --操作失败
club.SUB_GA_TABLE_USERLIST        = 205                                 --用户列表
club.SUB_GA_TABLE_LIST_FINISH     = 207                                 --列表完成

--消息推送
club.SUB_GA_USER_TABLE_STATUS     = 300                                 --用户坐下
club.SUB_GA_TABLE_START           = 301                                 --桌子开始
club.SUB_GA_TABLE_CREATE          = 303                                 --桌子创建
club.SUB_GA_TABLE_DISMISS         = 304                                 --桌子解散
club.SUB_GA_TABLE_CHANGEBANKER    = 305                                 --切换庄家

--查询桌子
club.CMD_GA_QueryTable = 
{
    -- 类型标识
    {t = "word", k = "wKindID"},
    -- 群组标识
    {t = "dword", k = "dwGroupID"},
}

--获取玩家列表
club.CMD_GA_GetTableUserList = 
{
    -- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 房间号
    {t = "dword", k = "dwPersonalRoomID"},
}

--桌子列表（组包发送 后面接tagPersonalTableInfo）
club.CMD_GA_TableList = 
{
    -- 群组标识
    {t = "dword", k = "dwGroupID"},
    -- 桌子数目
    {t = "word", k = "wTableCount"},
}

--操作失败
club.CMD_GA_OperateFailed = 
{
    -- 错误代码
    {t = "word", k = "lErrorCode"},
    -- 登录密码 
    {t = "string", k = "szErrorDescribe", s = 128},
}

--用户坐下
club.CMD_GA_UserTableStatus = 
{
    -- 类型ID
    {t = "dword", k = "dwKindID"},
    -- 房间ID
    {t = "dword", k = "dwServerID"},
    -- 桌子ID
    {t = "word", k = "wTableID"},
    -- 房间号
    {t = "dword", k = "dwPersonalRoomID"},
    -- 大联盟ID
    {t = "dword", k = "dwGroupID"},
    -- 用户标识
    {t = "dword", k = "dwUserID"},
    -- 用户椅子
    {t = "word", k = "wChairID"},
        -- 大联盟ID
    {t = "bool", k = "bSit"},

     -- {t = "dword", k = "dwBankerUserID"},
}

--用户列表
club.CMD_GA_TableUserList = 
{
     -- 用户数目
    {t = "word", k = "wUserCount"},
    -- 群组标识
    {t = "dword", k = "dwUserID", l = {20}},
}

--桌子开始
club.CMD_GA_TableStart = 
{
     -- 类型ID
    {t = "dword", k = "dwKindID"},
    -- 房间ID
    {t = "dword", k = "dwServerID"},
    -- 桌子ID
    {t = "word", k = "wTableID"},
    -- 大联盟ID
    {t = "dword", k = "dwGroupID"},
    -- 房间号
    {t = "dword", k = "dwPersonalRoomID"},

    {t = "bool", k = "bStart"},    --是否开始

    -- {t = "dword", k = "dwBankerUserID"},
}

--切换庄家
club.CMD_GA_BankerChange = 
{
     -- 类型ID
    {t = "dword", k = "dwKindID"},
    -- 房间ID
    {t = "dword", k = "dwServerID"},
    -- 桌子ID
    {t = "word", k = "wTableID"},
    -- 大联盟ID
    {t = "dword", k = "dwGroupID"},
    -- 房间号
    {t = "dword", k = "dwPersonalRoomID"},

    {t = "dword", k = "dwBankerUserID"},    --百人游戏庄家USERID(无效为0)
}

--解散桌子
club.CMD_CS_S_DismissClubTable = 
{
     -- 类型ID
    {t = "dword", k = "dwKindID"},
    -- 房间ID
    {t = "dword", k = "dwServerID"},
    -- 桌子ID
    {t = "word", k = "wTableID"},
    -- 房间号
    {t = "dword", k = "dwPersonalRoomID"},
    -- 大联盟ID
    {t = "dword", k = "dwGroupID"},

    {t = "bool", k = "bAllRoom"}, --是否删除服务器房间
}

club.tagPersonalTableInfo = 
{
    -- 房间ID
    {t = "dword", k = "dwServerID"},
    -- 类型ID
    {t = "dword", k = "dwKindID"},
    -- 桌子ID
    {t = "word", k = "wTableID"},
      -- 桌主I D
    {t = "dword", k = "dwUserID"},
    -- 局数限制
    {t = "dword", k = "dwDrawCountLimit"},
    -- 时间限制
    {t = "dword", k = "dwDrawTimeLimit"},
    -- 房间底分 
    {t = "score", k = "lCellScore"},
        -- 约战房间ID
    {t = "dword", k = "dwPersonalRoomID"},
  
    -- 参与游戏的人数 
    {t = "word", k = "wJoinGamePeopleCount"},
    -- 当前坐下的玩家 
    {t = "word", k = "wCurSitPlayer"},
        -- 当前坐下的玩家 
    {t = "byte", k = "cbPlayMode"},
        -- 入场 
    {t = "score", k = "lEnterScoreLimit"},
        -- 离场 
    {t = "score", k = "lLeaveScoreLimit"},
    -- 是否允许陌生人加入    0 不允许   1 允许
    {t = "byte", k = "cbJoinMode"},
    -- 大联盟ID
    {t = "dword", k = "dwGroupID"},
        -- 游戏开始
    {t = "bool", k = "bGameStart"},
    --房主昵称
    {t = "string", k = "szNickName", s = 32},
    --创建时间
    {t = "string", k = "szCreateTime", s = 32},
    --支付模式
    {t = "byte", k = "cbPayMode"},
    -- 当前坐下的玩家 
    {t = "byte", k = "cbGameRule", l = {100}},

    {t = "dword", k = "dwPlayerUserId",l = {20}},

    {t = "dword", k = "dwBankerUserID"},

    {t = "score", k = "lMaxChairGameMinChip"},

    {t = "dword", k = "dwConfigID"},

    {t = "string", k = "szConfigName",s = 13},--玩法名

}

club.tagIMcommonItem = 
{

    {t = "string", k = "szOptionName", s = 32},

    {t = "int", k = "nOptionValue"},
}



----------------------------------------大联盟战绩详情---------------------------

return club