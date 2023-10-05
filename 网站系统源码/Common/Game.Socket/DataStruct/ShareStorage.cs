using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Game.Socket.DataStruct
{

    /// <summary>
    /// 共享库存
    /// </summary>
    public class ShareStorage
    {
        /// <summary>
        /// 注册标识 0：未注册 1：已注册 (收到服务端消息时使用)
        /// </summary>
        public bool GameServerRegister { get; set; } 

        /// <summary>
        /// 状态  0：无状态 11：添加子项 22：修改子项 33：移除子项 44:客户端查询子项 55:服务端当前子项
        /// </summary>
        public int Status { get; set; }

        /// <summary>
        /// 共享库存子项
        /// </summary>
        public ShareStorageItem ShareStorageItem { get; set; }

        /// <summary>
        /// 提示信息
        /// </summary>
        public string ResultMessage { get; set; }

        public ShareStorage()
        {
            ShareStorageItem=new ShareStorageItem();
        }



    }

    /// <summary>
    /// 共享库存子项
    /// </summary>
    public class ShareStorageItem
    {
        /// <summary>
        /// 子项ID（GroupID和Kind的组合）
        /// </summary>
        public long ItemID { get; set; } = 0;

        /// <summary>
        /// 大联盟ID
        /// </summary>
        public int GroupID { get; set; } = 0;

        /// <summary>
        /// 游戏ID
        /// </summary>
        public short KindID { get; set; } = 0;

        /// <summary>
        /// 0积分1金币
        /// </summary>
        public byte PlayMode { get; set; } = 1;

        /// <summary>
        /// 修改子项类型  0：无子项 11：修改库存子项 22：修改暗抽子项 33：修改大奖子项 44:修改临时库存子项
        /// </summary>
        public int ModifySubItemType { get; set; } = 0;

        /// <summary>
        /// 配置库存子项信息
        /// </summary>
        public ConfigSubStorageItem ConfigSubStorageItem { get; set; }

        /// <summary>
        /// 当前库存子项
        /// </summary>
        public CurSubStorageItem CurSubStorageItem { get; set; }

        public ShareStorageItem() {
            ConfigSubStorageItem=new ConfigSubStorageItem();
            CurSubStorageItem=new CurSubStorageItem();
        }




    }

    /// <summary>
    /// 配置库存子项信息
    /// </summary>
    public class ConfigSubStorageItem
    {
        #region 库存配置

        /// <summary>
        /// 配置系统库存
        /// </summary>
        public long ConfigSysStorage { get; set; } = 0;

        /// <summary>
        /// 配置玩家库存
        /// </summary>
        public long ConfigPlayerStorage { get; set; } = 0;

        /// <summary>
        /// 配置系统库存重置比例
        /// </summary>
        public long ConfigParameterK { get; set; } = 0;

        /// <summary>
        /// 配置系统库存重置比例
        /// </summary>
        public long ConfigSysStorResetRate { get; set; } = 0;


        #endregion

        #region 暗抽配置

        /// <summary>
        /// 配置暗抽比例
        /// </summary>
        public long ConfigAnChouRate { get; set; } = 0;


        #endregion

        #region 大奖配置

        /// <summary>
        /// 配置大奖抽取比例
        /// </summary>
        public long ConfigMosgoldRate { get; set; } = 0;

        /// <summary>
        /// 配置派奖概率
        /// </summary>
        public long ConfigMosgoldDispatchRate { get; set; } = 0;

        /// <summary>
        /// 配置大奖库存比
        /// </summary>
        public long ConfigMosgoldStorageRate { get; set; } = 0;

        #endregion

        #region 临时库存配置

        /// <summary>
        /// 配置临时系统库存
        /// </summary>
        public long ConfigTempSysStorage { get; set; } = 0;

        /// <summary>
        /// 配置临时玩家库存
        /// </summary>
        public long ConfigTempPlayerStorage { get; set; } = 0;

        /// <summary>
        /// 配置临时调节系数(百分比)
        /// </summary>
        public long ConfigTempParameterK { get; set; } = 0;

        /// <summary>
        /// 配置临时系统库存重置比例
        /// </summary>
        public long ConfigTempSysStorResetRate { get; set; } = 0;


        #endregion

    }

    /// <summary>
    /// 当前库存子项
    /// </summary>
    public class CurSubStorageItem
    {
        #region 基本配置

        /// <summary>
        /// 当前统计(包括通用库存，房间牌桌调试中系统输赢和暗抽大奖)
        /// </summary>
        public long CurStatisticSysWin { get; set; } = 0;

        /// <summary>
        /// 当前服务比例总数
        /// </summary>
        public long CurStatisticService { get; set; } = 0;

        /// <summary>
        /// 当前系统库存
        /// </summary>
        public long CurSysStorage { get; set; } = 0;

        /// <summary>
        /// 当前玩家库存
        /// </summary>
        public long CurPlayerStorage { get; set; } = 0;

        /// <summary>
        /// 当前调节系数(百分比)
        /// </summary>
        public long CurParameterK { get; set; } = 0;

        /// <summary>
        /// 当前系统库存重置比例
        /// </summary>
        public long CurSysStorResetRate { get; set; } = 0;

        /// <summary>
        /// 当前中奖率
        /// </summary>
        public long CurWinChance { get; set; } = 0;

        /// <summary>
        /// 当前库存重置次数
        /// </summary>
        public long CurResetCount { get; set; } = 0;

        /// <summary>
        /// 当前通用控制系统总输赢
        /// </summary>
        public long CurSysWin { get; set; } = 0;


        #endregion

        #region 暗抽配置

        /// <summary>
        /// 当前暗抽比例
        /// </summary>
        public long CurAnChouRate { get; set; } = 0;

        /// <summary>
        /// 当前暗抽总数
        /// </summary>
        public long CurAnChouTotal { get; set; } = 0;


        #endregion

        #region 大奖配置

        /// <summary>
        /// 当前大奖抽取比例
        /// </summary>
        public long CurMosgoldRate { get; set; } = 0;

        /// <summary>
        /// 当前派奖概率
        /// </summary>
        public long CurMosgoldDispatchRate { get; set; } = 0;

        /// <summary>
        /// 当前大奖库存比例
        /// </summary>
        public long CurMosgoldStorageRate { get; set; } = 0;

        #endregion

        #region 临时库存配置

        /// <summary>
        /// 当前临时系统库存
        /// </summary>
        public long CurTempSysStorage { get; set; } = 0;

        /// <summary>
        /// 当前临时玩家库存
        /// </summary>
        public long CurTempPlayerStorage { get; set; } = 0;

        /// <summary>
        /// 当前临时调节系数(百分比)
        /// </summary>
        public long CurTempParameterK { get; set; } = 0;

        /// <summary>
        /// 当前临时系统库存重置比例
        /// </summary>
        public long CurTempSysStorResetRate { get; set; } = 0;

        /// <summary>
        /// 当前临时中奖率
        /// </summary>
        public long CurTempWinChance { get; set; } = 0;


        #endregion
    }
}
