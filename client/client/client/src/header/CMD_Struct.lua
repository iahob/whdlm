--
-- Author: zhong
-- Date: 2017-06-02 10:26:54
--
local cmd_struct = {}

-- 房间信息
cmd_struct.tagGameServer = 
{
    {t = "word", k = "wKindID"},                -- 名称索引
    {t = "word", k = "wNodeID"},                -- 节点索引
    {t = "word", k = "wSortID"},                -- 排序索引   
    {t = "word", k = "wServerID"},              -- 房间索引
    {t = "word", k = "wServerKind"},            -- 房间类型
    {t = "word", k = "wServerType"},            -- 房间类型
    {t = "word", k = "wServerLevel"},           -- 房间等级
    {t = "word", k = "wServerPort"},            -- 房间端口
    {t = "byte", k = "cbPlayMode"},          -- 0积分，1金币

    {t = "score", k = "lCellScore"},            -- 单元积分
    {t = "byte", k = "cbEnterMember"},          -- 进入会员
    {t = "score", k = "lEnterScore"},           -- 进入积分

    {t = "dword", k = "dwServerRule"},          -- 房间规则
    {t = "dword", k = "dwOnLineCount"},         -- 在线人数
    {t = "dword", k = "dwAndroidCount"},        -- 机器人数
    {t = "dword", k = "dwFullCount"},           -- 满员人数

    {t = "string", k = "szServerAddr", s = 32}, -- 房间名称
    {t = "string", k = "szServerName", s = 32}, -- 房间名称
    {t = "dword", k = "dwSurportType"},         -- 支持类型
    {t = "word", k = "wTableCount"},            -- 桌子数目
    --配置属性
    {t = "dword", k = "dwSetPlayerCount"},      -- 设置人数
    {t = "string", k = "szGameInfomation", s = 32}, -- 房间简介
    {t = "bool", k = "bGroupPersonalMaxChair"},            -- 桌子数目
}

-- 用户信息
cmd_struct.tagMobileUserInfoHead = 
{
    -- 用户属性
    {t = "dword", k = "dwGameID"},              -- 游戏 I D
    {t = "dword", k = "dwUserID"},              -- 用户 I D

    -- 头像信息
    {t = "word", k = "wFaceID"},                -- 头像索引
    {t = "dword", k = "dwCustomID"},            -- 自定标识

    -- 用户属性
    {t = "byte", k = "cbGender"},               -- 用户性别
    {t = "byte", k = "cbMemberOrder"},          -- 会员等级

    -- 用户状态
    {t = "word", k = "wTableID"},               -- 桌子索引
    {t = "word", k = "wChairID"},               -- 椅子索引
    {t = "byte", k = "cbUserStatus"},           -- 用户状态

    -- 积分信息
    {t = "score", k = "lScore"},                -- 用户分数
    {t = "score", k = "lIngot"},                -- 用户元宝
    {t = "double", k = "dBeans"},               -- 用户游戏豆

    -- 游戏信息
    {t = "dword", k = "dwWinCount"},            -- 胜利盘数
    {t = "dword", k = "dwLostCount"},           -- 失败盘数
    {t = "dword", k = "dwDrawCount"},           -- 和局盘数
    {t = "dword", k = "dwFleeCount"},           -- 逃跑盘数
    {t = "dword", k = "dwExperience"},          -- 用户经验
    {t = "score", k = "lIntegralCount"},        -- 积分总数

    -- 代理信息
    {t = "dword", k = "dwAgentID"},             -- 代理 I D

    {t = "dword", k = "dwIpAddress"},           -- ip地址
}

--  用户积分
cmd_struct.tagMobileUserScore = 
{
    -- 积分信息
    {t = "score", k = "lScore"},                -- 用户分数
    {t = "double", k = "dBeans"},               -- 用户游戏豆

    -- 输赢信息
    {t = "dword", k = "dwWinCount"},            -- 胜利盘数
    {t = "dword", k = "dwLostCount"},           -- 失败盘数
    {t = "dword", k = "dwDrawCount"},           -- 和局盘数
    {t = "dword", k = "dwFleeCount"},           -- 逃跑盘数

    -- 全局信息
    {t = "dword", k = "dwExperience"},          -- 用户经验
}

-- 桌子状态
cmd_struct.tabTableStatus = 
{
    {t = "byte", k = "cbTableLock"},            -- 锁定标识
    {t = "byte", k = "cbPlayStatus"},           -- 游戏标识
    {t = "int", k = "lCellScore"},              -- 单元积分
}

-- 用户状态
cmd_struct.tagUserStatus = 
{
    {t = "word", k = "wTableID"},               -- 桌子索引
    {t = "word", k = "wChairID"},               -- 椅子索引
    {t = "byte", k = "cbUserStatus"},           -- 用户状态
}
return cmd_struct