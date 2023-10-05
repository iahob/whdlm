﻿using System;
using System.Collections.Generic;
using Game.Utils;

namespace Game.Entity.Enum
{
    /// <summary>
    ///管理员操作权限
    /// </summary>
    [Serializable]
    [EnumDescription( "管理员操作权限" )]
    public enum Permission : int
    {
        /// <summary>
        /// 查看权限
        /// </summary>
        [EnumDescription( "查看权限" )]
        Read = 1,

        /// <summary>
        /// 添加权限
        /// </summary>
        [EnumDescription( "添加权限" )]
        Add = 2,

        /// <summary>
        /// 编辑权限
        /// </summary>
        [EnumDescription( "编辑权限" )]
        Edit = 4,

        /// <summary>
        /// 删除权限
        /// </summary>
        [EnumDescription( "删除权限" )]
        Delete = 8,

        /// <summary>
        /// 赠送会员权限
        /// </summary>
        [EnumDescription( "赠送会员权限" )]
        GrantMember = 16,

        /// <summary>
        /// 赠送金币权限
        /// </summary>
        [EnumDescription( "赠送金币权限" )]
        GrantTreasure = 32,

        /// <summary>
        /// 赠送经验权限
        /// </summary>
        [EnumDescription( "赠送经验权限" )]
        GrantExperience = 64,

        /// <summary>
        /// 赠送积分权限
        /// </summary>
        [EnumDescription( "赠送积分权限" )]
        GrantScore = 128,

        /// <summary>
        /// 赠送靓号权限
        /// </summary>
        [EnumDescription( "赠送靓号权限" )]
        GrantGameID = 256,

        /// <summary>
        /// 清零积分权限
        /// </summary>
        [EnumDescription( "清零积分权限" )]
        ClearScore = 512,

        /// <summary>
        /// 清零逃率权限
        /// </summary>
        [EnumDescription( "清零逃率权限" )]
        ClearFlee = 1024,

        /// <summary>
        /// 生成实卡权限
        /// </summary>
        [EnumDescription( "生成实卡权限" )]
        CreateCard = 2048,

        /// <summary>
        /// 导出实卡权限
        /// </summary>
        [EnumDescription( "导出实卡权限" )]
        ExportCard = 4096,

        /// <summary>
        /// 冻/解帐号权限
        /// </summary>
        [EnumDescription( "冻/解帐号权限 " )]
        Enable = 8192,

        /// <summary>
        /// 设置/取消机器人
        /// </summary>
        [EnumDescription( "设置/取消机器人" )]
        IsRobot = 16384,

        /// <summary>
        /// 启动/禁用商品或商品分类
        /// </summary>
        [EnumDescription( "启动/禁用" )]
        IsNulity = 32768,

        /// <summary>
        /// 订单操作
        /// </summary>
        [EnumDescription( "订单操作" )]
        OrderOperating = 65536,

        /// <summary>
        /// 发送邮件
        /// </summary>
        [EnumDescription( "发送邮件" )]
        SendMail = 131072,

        /// <summary>
        /// 赠送钻石
        /// </summary>
        [EnumDescription("赠送钻石")]
        GrantDiamond = 262144,

        /// <summary>
        /// 转账权限
        /// </summary>
        [EnumDescription("转账权限")]
        TransferPower = 524288,

        /// <summary>
        /// 调试权限
        /// </summary>
        [EnumDescription("调试权限")]
        DebugPower = 131072

    }

    /// <summary>
    /// 辅助类
    /// </summary>
    public class PermissionHelper
    {
        /// <summary>
        /// 获取描述
        /// </summary>
        /// <param name="permission"></param>
        /// <returns></returns>
        public static string GetPermissionDes( Permission permission )
        {
            return EnumDescription.GetFieldText( permission );
        }

        /// <summary>
        /// 获取列表
        /// </summary>
        /// <returns></returns>
        public static IList<EnumDescription> GetPermissionList()
        {
            IList<EnumDescription> list = EnumDescription.GetFieldTexts( typeof( Permission ) );
            return list;
        }
    }
}