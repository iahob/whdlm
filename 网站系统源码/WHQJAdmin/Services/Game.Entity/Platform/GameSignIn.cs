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
    /// 实体类 GameSignIn  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class GameSignIn
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "GameSignIn";

        #endregion 

        #region 私有变量

        private int p_signid;
        private byte p_typeid;
        private int p_packageid;
        private int p_probability;
        private int p_needday;
        private int p_sortid;
        private byte p_nullity;
        private DateTime p_collectdate;
        private string p_resourceurl;
        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化GameSignIn
        /// </summary>
        public GameSignIn() 
        {
            p_signid = 0;
            p_typeid = 0;
            p_packageid = 0;
            p_probability = 0;
            p_needday = 0;
            p_sortid = 0;
            p_nullity = 0;
            p_collectdate = DateTime.Now;
            p_resourceurl = string.Empty;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// SignID
        /// </summary>
        public int SignID
        {
            set
            {
                p_signid=value;
            }
            get
            {
                return p_signid;
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
        /// Probability
        /// </summary>
        public int Probability
        {
            set
            {
                p_probability=value;
            }
            get
            {
                return p_probability;
            }
        }

        /// <summary>
        /// NeedDay
        /// </summary>
        public int NeedDay
        {
            set
            {
                p_needday=value;
            }
            get
            {
                return p_needday;
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
        /// ResourceURL
        /// </summary>
        public string ResourceURL
        {
            set
            {
                p_resourceurl = value;
            }
            get
            {
                return p_resourceurl;
            }
        }

        #endregion
    }
}

