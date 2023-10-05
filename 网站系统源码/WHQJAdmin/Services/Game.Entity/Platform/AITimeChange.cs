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
    /// 实体类 AITimeChange  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class AITimeChange
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "AITimeChange";

        #endregion 

        #region 私有变量

        private int p_id;
        private int p_aitime;
        private int p_aicount;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化AITimeChange
        /// </summary>
        public AITimeChange() 
        {
            p_id = 0;
            p_aitime = 0;
            p_aicount = 0;
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
        /// AITime
        /// </summary>
        public int AITime
        {
            set
            {
                p_aitime=value;
            }
            get
            {
                return p_aitime;
            }
        }

        /// <summary>
        /// AICount
        /// </summary>
        public int AICount
        {
            set
            {
                p_aicount=value;
            }
            get
            {
                return p_aicount;
            }
        }

        #endregion
    }
}

