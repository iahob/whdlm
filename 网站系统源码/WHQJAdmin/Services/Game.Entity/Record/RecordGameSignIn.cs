/*
 * 版本： 4.0
 * 日期：2018/7/18 9:55:05
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Record
{
    /// <summary>
    /// 实体类 RecordGameSignIn  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class RecordGameSignIn
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "RecordGameSignIn";

        #endregion 

        #region 私有变量

        private int p_recordid;
        private int p_userid;
        private byte p_signtype;
        private string p_packagename;
        private string p_packagegoods;
        private int p_probability;
        private int p_needday;
        private int p_totalday;
        private string p_clinetip;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化RecordGameSignIn
        /// </summary>
        public RecordGameSignIn() 
        {
            p_recordid = 0;
            p_userid = 0;
            p_signtype = 0;
            p_packagename = string.Empty;
            p_packagegoods = string.Empty;
            p_probability = 0;
            p_needday = 0;
            p_totalday = 0;
            p_clinetip = string.Empty;
            p_collectdate = DateTime.Now;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// RecordID
        /// </summary>
        public int RecordID
        {
            set
            {
                p_recordid=value;
            }
            get
            {
                return p_recordid;
            }
        }

        /// <summary>
        /// UserID
        /// </summary>
        public int UserID
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
        /// SignType
        /// </summary>
        public byte SignType
        {
            set
            {
                p_signtype=value;
            }
            get
            {
                return p_signtype;
            }
        }

        /// <summary>
        /// PackageName
        /// </summary>
        public string PackageName
        {
            set
            {
                p_packagename=value;
            }
            get
            {
                return p_packagename;
            }
        }

        /// <summary>
        /// PackageGoods
        /// </summary>
        public string PackageGoods
        {
            set
            {
                p_packagegoods=value;
            }
            get
            {
                return p_packagegoods;
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
        /// TotalDay
        /// </summary>
        public int TotalDay
        {
            set
            {
                p_totalday=value;
            }
            get
            {
                return p_totalday;
            }
        }

        /// <summary>
        /// ClinetIP
        /// </summary>
        public string ClinetIP
        {
            set
            {
                p_clinetip=value;
            }
            get
            {
                return p_clinetip;
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

        #endregion
    }
}

