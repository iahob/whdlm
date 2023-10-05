/*
 * 版本： 4.0
 * 日期：2018/7/24 10:45:09
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.NativeWeb
{
    /// <summary>
    /// 实体类 AwardType  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class AwardType
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "AwardType";

        #endregion 

        #region 私有变量

        private int p_typeid;
        private string p_typename;
        private int p_sortid;
        private byte p_nullity;
        private DateTime p_collectdate;
        private string p_classname;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化AwardType
        /// </summary>
        public AwardType() 
        {
            p_typeid = 0;
            p_typename = string.Empty;
            p_sortid = 0;
            p_nullity = 0;
            p_collectdate = DateTime.Now;
            p_classname = string.Empty;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// TypeID
        /// </summary>
        public int TypeID
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
        /// TypeName
        /// </summary>
        public string TypeName
        {
            set
            {
                p_typename=value;
            }
            get
            {
                return p_typename;
            }
        }

        /// <summary>
        /// SortID
        /// </summary>
        public int SortID
        {
            set
            {
                p_sortid=value;
            }
            get
            {
                return p_sortid;
            }
        }

        /// <summary>
        /// Nullity
        /// </summary>
        public byte Nullity
        {
            set
            {
                p_nullity=value;
            }
            get
            {
                return p_nullity;
            }
        }

        /// <summary>
        /// CollectDate
        /// </summary>
        public DateTime CollectDate
        {
            set
            {
                p_collectdate=value;
            }
            get
            {
                return p_collectdate;
            }
        }
        /// <summary>
        /// CollectDate
        /// </summary>
        public string ClassName
        {
            set
            {
                p_classname = value;
            }
            get
            {
                return p_classname;
            }
        }

        #endregion
    }
}

