/*
 * 版本： 4.0
 * 日期：2018/8/8 16:15:54
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.NativeWeb
{
    /// <summary>
    /// 实体类 ShareConfig  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class ShareConfig
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "ShareConfig";

        #endregion 

        #region 私有变量

        private int p_id;
        private int p_daysharelmt;
        private int p_timesharegold;
        private int p_timesharediamond;
        private int p_nullity;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化ShareConfig
        /// </summary>
        public ShareConfig() 
        {
            p_id = 0;
            p_daysharelmt = 0;
            p_timesharegold = 0;
            p_timesharediamond = 0;
            p_nullity = 0;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// ID
        /// </summary>
        public int ID
        {
            set
            {
                p_id=value;
            }
            get
            {
                return p_id;
            }
        }

        /// <summary>
        /// DayShareLmt
        /// </summary>
        public int DayShareLmt
        {
            set
            {
                p_daysharelmt=value;
            }
            get
            {
                return p_daysharelmt;
            }
        }

        /// <summary>
        /// TimeShareGold
        /// </summary>
        public int TimeShareGold
        {
            set
            {
                p_timesharegold=value;
            }
            get
            {
                return p_timesharegold;
            }
        }

        /// <summary>
        /// TimeShareDiamond
        /// </summary>
        public int TimeShareDiamond
        {
            set
            {
                p_timesharediamond=value;
            }
            get
            {
                return p_timesharediamond;
            }
        }
        /// <summary>
        /// Nullity
        /// </summary>
        public int Nullity
        {
            set
            {
                p_nullity = value;
            }
            get
            {
                return p_nullity;
            }
        }

        #endregion
    }
}

