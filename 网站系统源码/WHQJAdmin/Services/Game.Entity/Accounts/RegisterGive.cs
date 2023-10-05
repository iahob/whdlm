/*
 * 版本： 4.0
 * 日期：2018/10/18 15:51:35
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Accounts
{
    /// <summary>
    /// 实体类 RegisterGive  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class RegisterGive
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "RegisterGive";

        #endregion 

        #region 私有变量

        private int p_configid;
        private int p_scorecount;
        private int p_diamondcount;
        private int p_platformtype;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化RegisterGive
        /// </summary>
        public RegisterGive() 
        {
            p_configid = 0;
            p_scorecount = 0;
            p_diamondcount = 0;
            p_platformtype = 0;
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
        /// ScoreCount
        /// </summary>
        public int ScoreCount
        {
            set
            {
                p_scorecount=value;
            }
            get
            {
                return p_scorecount;
            }
        }

        /// <summary>
        /// DiamondCount
        /// </summary>
        public int DiamondCount
        {
            set
            {
                p_diamondcount=value;
            }
            get
            {
                return p_diamondcount;
            }
        }

        /// <summary>
        /// PlatformType
        /// </summary>
        public int PlatformType
        {
            set
            {
                p_platformtype=value;
            }
            get
            {
                return p_platformtype;
            }
        }

        #endregion
    }
}

