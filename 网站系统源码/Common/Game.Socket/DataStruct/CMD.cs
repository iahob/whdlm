using Game.Utils;

// ReSharper disable InconsistentNaming
// ReSharper disable IdentifierTypo

namespace Game.Socket.DataStruct
{
    [EnumDescription("主命令集")]
    public enum CMD_Main
    {
        #region 主命令

        /// <summary>
        /// 通讯主命令
        /// </summary>
        [EnumDescription("通讯主命令")] Main_Tcp = 0,

        /// <summary>
        /// 网站服务主命令
        /// </summary>
        [EnumDescription("网站服务主命令")] Main_WebService = 9,

        #endregion
    }

    [EnumDescription("子命令集")]
    public enum CMD_Sub
    {
        #region 发送子命令

        /// <summary>
        /// 通讯验证包
        /// </summary>
        [EnumDescription("通讯验证包")]
        Sub_Req_Valid = 2,


        /// <summary>
        /// 更新配置
        /// </summary>
        [EnumDescription("更新配置")]
        Sub_Req_UpdateParamter = 101,

        /// <summary>
        /// 更新玩家财富
        /// </summary>
        [EnumDescription("更新玩家财富")]
        Sub_Req_UpdateUserScore = 102,

        /// <summary>
        /// 更新玩家邮件
        /// </summary>
        [EnumDescription("更新玩家邮件")]
        Sub_Req_UpdateMailMessage = 103,

        /// <summary>
        /// 更新大联盟最大人数
        /// </summary>
        [EnumDescription("更新大联盟最大人数")]
        Sub_Req_UpdateGroupMaxMember = 104,

        /// <summary>
        /// 更新共享库存
        /// </summary>
        [EnumDescription("更新共享库存")]
        Sub_Req_UpdateShareStorage = 106,


        /// <summary>
        /// 更新AI加载配置
        /// </summary>
        [EnumDescription("更新AI加载配置")]
        Sub_Req_UpdateAIPARAMTER = 201,

        /// <summary>
        /// 更新AI加载配置
        /// </summary>
        [EnumDescription("更新AI离开权重配置")]
        Sub_Req_UpdateAIWEIGHTOUT = 202,

        /// <summary>
        /// 更新AI进入房间配置
        /// </summary>
        [EnumDescription("更新AI进入房间配置")]
        Sub_Req_UpdateAIWEIGHTIN = 203,

        /// <summary>
        /// 更新AI人数权重
        /// </summary>
        [EnumDescription("更新AI人数权重")]
        Sub_Req_UpdateAIWEIGHTTOTAL = 204,

        /// <summary>
        /// 强制下线或退出AI
        /// </summary>
        [EnumDescription("强制下线或退出AI")]
        Sub_Req_DELETEAI = 205,

        Sub_Req_HttpHelper = 200,

        #endregion

        #region 接受子命令

        /// <summary>
        /// 通讯验证包
        /// </summary>
        [EnumDescription("测试子命令")]
        ServerTest = 1,

        /// <summary>
        /// 更新成功
        /// </summary>
        [EnumDescription("更新成功")]
        Sub_Resp_UpdateSuccess = 111,

        /// <summary>
        /// 更新失败
        /// </summary>
        [EnumDescription("更新失败")]
        Sub_Resp_UpdateFailure = 112,

        /// <summary>
        /// 操作返回结果
        /// </summary>
        [EnumDescription("操作返回结果")]
        Sub_Resp_OperateResult = 105,

        /// <summary>
        /// 接收共享库存
        /// </summary>
        [EnumDescription("接收共享库存")]
        Sub_Resq_GetShareStorage = 113,


        Sub_Resp_HttpHelper = 210,

        #endregion
    }

    [EnumDescription("更新配置类型")]
    public enum UpdateParamterType
    {
        [EnumDescription("无效操作")] None = 0,
        [EnumDescription("游戏类型配置")] GameKind = 1,
        [EnumDescription("游戏列表配置")] GameList = 2,
        [EnumDescription("系统消息")] SystemMessage = 3,
        [EnumDescription("签到配置")] CheckIn = 4,
        [EnumDescription("道具配置")] Property = 5,
    }

    [EnumDescription("更新玩家财富通知类型")]
    public enum UpdateUserScoreType
    {
        [EnumDescription("无效操作")] None = 0,
        [EnumDescription("普通通知")] Normal = 1,
        [EnumDescription("充值到账通知")] PayNotify = 2,
    }

    [EnumDescription("操作共享库存类型")]
    public enum OperStorgeType
    {
        [EnumDescription("无状态")]
        无状态 = 0,
        [EnumDescription("添加子项")]
        添加子项 = 11,
        [EnumDescription("修改子项")]
        修改子项 = 22,
        [EnumDescription("修改子项")]
        移除子项 = 33,
        [EnumDescription("客户端查询子项")]
        客户端查询子项 = 44,
        [EnumDescription("服务端当前子项")]
        服务端当前子项 = 55,
        [EnumDescription("修改子项结果成功")]
        修改子项结果成功 = 66,
        [EnumDescription("修改子项结果失败")]
        修改子项结果失败 = 77,
        [EnumDescription("客户端查询子项成功")]
        客户端查询子项成功 = 88,
        [EnumDescription("客户端查询子项失败")]
        客户端查询子项失败 = 99,
    }

    [EnumDescription("修改共享库存类型")]
    public enum UpateStorgeSubType
    {
        [EnumDescription("无子项")]
        无子项 = 0,
        [EnumDescription("库存子项")]
        库存子项 = 11,
        [EnumDescription("暗抽子项")]
        暗抽子项 = 22,
        [EnumDescription("大奖子项")]
        大奖子项 = 33,
        [EnumDescription("临时库存子项")]
        临时库存子项 = 44,
    }

    [EnumDescription("强制下线,强制退出类型")]
    public enum DelteAIType
    {
        [EnumDescription("强制下线")]
        强制下线 = 0,
        [EnumDescription("强制退出")]
        强制退出 = 1,
    }
}