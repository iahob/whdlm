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
    /// 实体类 AIWeightOutRatio  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class AIWeightOutRatio
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "AIWeightOutRatio";

        #endregion 

        #region 私有变量

        private int p_id;
        private int p_timecount;
        private int p_weightout;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化AIWeightOutRatio
        /// </summary>
        public AIWeightOutRatio() 
        {
            p_id = 0;
            p_timecount = 0;
            p_weightout = 0;
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
        /// WeightOut
        /// </summary>
        public int WeightOut
        {
            set
            {
                p_weightout=value;
            }
            get
            {
                return p_weightout;
            }
        }

        #endregion
    }
}

