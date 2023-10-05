/*
 * 版本： 4.0
 * 日期：2019/12/16 17:22:26
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Platform
{
    /// <summary>
    /// 实体类 AIWeightInRatio  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class AIWeightInRatio
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "AIWeightInRatio";

        #endregion 

        #region 私有变量

        private int p_id;
        private int p_timecount;
        private int p_weightin;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化AIWeightInRatio
        /// </summary>
        public AIWeightInRatio() 
        {
            p_id = 0;
            p_timecount = 0;
            p_weightin = 0;
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
        /// TimeCount
        /// </summary>
        public int TimeCount
        {
            set
            {
                p_timecount=value;
            }
            get
            {
                return p_timecount;
            }
        }

        /// <summary>
        /// WeightIn
        /// </summary>
        public int WeightIn
        {
            set
            {
                p_weightin=value;
            }
            get
            {
                return p_weightin;
            }
        }

        #endregion
    }
}

