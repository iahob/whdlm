/*
 * 版本： 4.0
 * 日期：2018/7/17 10:11:26
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Platform
{
    /// <summary>
    /// 实体类 GamePackage  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class GamePackage
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "GamePackage";

        #endregion 

        #region 私有变量

        private int p_packageid;
        private string p_name;
        private byte p_typeid;
        private int p_sortid;
        private byte p_nullity;
        private string p_describe;
        private DateTime p_collectdate;
        private int p_platformkind;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化GamePackage
        /// </summary>
        public GamePackage() 
        {
            p_packageid = 0;
            p_name = string.Empty;
            p_typeid = 0;
            p_sortid = 0;
            p_nullity = 0;
            p_describe = string.Empty;
            p_collectdate = DateTime.Now;
            p_platformkind = 0;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// PackageID
        /// </summary>
        public int PackageID
        {
            set
            {
                p_packageid=value;
            }
            get
            {
                return p_packageid;
            }
        }

        /// <summary>
        /// Name
        /// </summary>
        public string Name
        {
            set
            {
                p_name=value;
            }
            get
            {
                return p_name;
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
        /// Describe
        /// </summary>
        public string Describe
        {
            set
            {
                p_describe=value;
            }
            get
            {
                return p_describe;
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
        /// PlatformKind
        /// </summary>
        public int PlatformKind
        {
            set
            {
                p_platformkind = value;
            }
            get
            {
                return p_platformkind;
            }
        }

        #endregion
    }
}

