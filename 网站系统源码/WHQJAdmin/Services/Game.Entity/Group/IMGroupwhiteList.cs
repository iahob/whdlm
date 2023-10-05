/*
 * 版本： 4.0
 * 日期：2019/9/9 14:47:35
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Group
{
    /// <summary>
    /// 实体类 IMGroupwhiteList  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class IMGroupwhiteList
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "IMGroupwhiteList";

        #endregion 

        #region 私有变量

        private Int64 p_userid;
        private DateTime p_inserttime;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化IMGroupwhiteList
        /// </summary>
        public IMGroupwhiteList() 
        {
            p_userid = 0;
            p_inserttime = DateTime.Now;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// UserID
        /// </summary>
        public Int64 UserID
        {
            set
            {
                p_userid=value;
            }
            get
            {
                return p_userid;
            }
        }

        /// <summary>
        /// InsertTime
        /// </summary>
        public DateTime InsertTime
        {
            set
            {
                p_inserttime=value;
            }
            get
            {
                return p_inserttime;
            }
        }

        #endregion
    }
}

