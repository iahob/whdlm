/*
 * 版本： 4.0
 * 日期：2018/7/6 18:49:18
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.NativeWeb
{
    /// <summary>
    /// 实体类 RankingConfig  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class RankingConfig
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "RankingConfig";

        #endregion 

        #region 私有变量

        private int p_configid;
        private byte p_typeid;
        private int p_ranktype;
        private Int64 p_gold;
        private int p_diamond;
        private int p_awardticket;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化RankingConfig
        /// </summary>
        public RankingConfig() 
        {
            p_configid = 0;
            p_typeid = 0;
            p_ranktype = 0;
            p_gold = 0;
            p_diamond = 0;
            p_awardticket = 0;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// ConfigID
        /// </summary>
        public int ConfigID
        {
            set
            {
                p_configid=value;
            }
            get
            {
                return p_configid;
            }
        }

        /// <summary>
        /// TypeID
        /// </summary>
        public byte TypeID
        {
            set
            {
                p_typeid=value;
            }
            get
            {
                return p_typeid;
            }
        }

        /// <summary>
        /// RankType
        /// </summary>
        public int RankType
        {
            set
            {
                p_ranktype=value;
            }
            get
            {
                return p_ranktype;
            }
        }

        /// <summary>
        /// Gold
        /// </summary>
        public Int64 Gold
        {
            set
            {
                p_gold=value;
            }
            get
            {
                return p_gold;
            }
        }

        /// <summary>
        /// Diamond
        /// </summary>
        public int Diamond
        {
            set
            {
                p_diamond=value;
            }
            get
            {
                return p_diamond;
            }
        }
        /// <summary>
        /// AwardTicket
        /// </summary>
        public int AwardTicket
        {
            set
            {
                p_awardticket = value;
            }
            get
            {
                return p_awardticket;
            }
        }

        #endregion
    }
}

