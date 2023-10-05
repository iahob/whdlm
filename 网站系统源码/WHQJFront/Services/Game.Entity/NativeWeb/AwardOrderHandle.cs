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
    /// 实体类 AwardOrderHandle  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class AwardOrderHandle
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "AwardOrderHandle";

        #endregion 

        #region 私有变量

        private int p_handleid;
        private string p_orderid;
        private int p_masterid;
        private byte p_orderstatus;
        private byte p_handlestatus;
        private string p_remark;
        private string p_clientip;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化AwardOrderHandle
        /// </summary>
        public AwardOrderHandle() 
        {
            p_handleid = 0;
            p_orderid = string.Empty;
            p_masterid = 0;
            p_orderstatus = 0;
            p_handlestatus = 0;
            p_remark = string.Empty;
            p_clientip = string.Empty;
            p_collectdate = DateTime.Now;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// HandleID
        /// </summary>
        public int HandleID
        {
            set
            {
                p_handleid=value;
            }
            get
            {
                return p_handleid;
            }
        }

        /// <summary>
        /// OrderID
        /// </summary>
        public string OrderID
        {
            set
            {
                p_orderid=value;
            }
            get
            {
                return p_orderid;
            }
        }

        /// <summary>
        /// MasterID
        /// </summary>
        public int MasterID
        {
            set
            {
                p_masterid=value;
            }
            get
            {
                return p_masterid;
            }
        }

        /// <summary>
        /// OrderStatus
        /// </summary>
        public byte OrderStatus
        {
            set
            {
                p_orderstatus=value;
            }
            get
            {
                return p_orderstatus;
            }
        }

        /// <summary>
        /// HandleStatus
        /// </summary>
        public byte HandleStatus
        {
            set
            {
                p_handlestatus=value;
            }
            get
            {
                return p_handlestatus;
            }
        }

        /// <summary>
        /// Remark
        /// </summary>
        public string Remark
        {
            set
            {
                p_remark=value;
            }
            get
            {
                return p_remark;
            }
        }

        /// <summary>
        /// ClientIP
        /// </summary>
        public string ClientIP
        {
            set
            {
                p_clientip=value;
            }
            get
            {
                return p_clientip;
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

