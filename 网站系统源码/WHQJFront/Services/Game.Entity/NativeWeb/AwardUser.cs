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
    /// 实体类 AwardUser  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class AwardUser
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "AwardUser";

        #endregion 

        #region 私有变量

        private int p_recordid;
        private int p_userid;
        private string p_compellation;
        private string p_mobilephone;
        private string p_province;
        private string p_city;
        private string p_area;
        private string p_detailaddress;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化AwardUser
        /// </summary>
        public AwardUser() 
        {
            p_recordid = 0;
            p_userid = 0;
            p_compellation = string.Empty;
            p_mobilephone = string.Empty;
            p_province = string.Empty;
            p_city = string.Empty;
            p_area = string.Empty;
            p_detailaddress = string.Empty;
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
        /// Compellation
        /// </summary>
        public string Compellation
        {
            set
            {
                p_compellation=value;
            }
            get
            {
                return p_compellation;
            }
        }

        /// <summary>
        /// MobilePhone
        /// </summary>
        public string MobilePhone
        {
            set
            {
                p_mobilephone=value;
            }
            get
            {
                return p_mobilephone;
            }
        }

        /// <summary>
        /// Province
        /// </summary>
        public string Province
        {
            set
            {
                p_province=value;
            }
            get
            {
                return p_province;
            }
        }

        /// <summary>
        /// City
        /// </summary>
        public string City
        {
            set
            {
                p_city=value;
            }
            get
            {
                return p_city;
            }
        }

        /// <summary>
        /// Area
        /// </summary>
        public string Area
        {
            set
            {
                p_area=value;
            }
            get
            {
                return p_area;
            }
        }

        /// <summary>
        /// DetailAddress
        /// </summary>
        public string DetailAddress
        {
            set
            {
                p_detailaddress=value;
            }
            get
            {
                return p_detailaddress;
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

