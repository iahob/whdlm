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
    /// 实体类 AwardInfo  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class AwardInfo
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "AwardInfo";

        #endregion 

        #region 私有变量

        private int p_awardid;
        private int p_awardtype;
        private string p_awardname;
        private string p_awardimg;
        private int p_unitprice;
        private int p_gold;
        private int p_diamond;
        private int p_inventory;
        private int p_sortid;
        private byte p_nullity;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化AwardInfo
        /// </summary>
        public AwardInfo() 
        {
            p_awardid = 0;
            p_awardtype = 0;
            p_awardname = string.Empty;
            p_awardimg = string.Empty;
            p_unitprice = 0;
            p_gold = 0;
            p_diamond = 0;
            p_inventory = 0;
            p_sortid = 0;
            p_nullity = 0;
            p_collectdate = DateTime.Now;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// AwardID
        /// </summary>
        public int AwardID
        {
            set
            {
                p_awardid=value;
            }
            get
            {
                return p_awardid;
            }
        }

        /// <summary>
        /// AwardType
        /// </summary>
        public int AwardType
        {
            set
            {
                p_awardtype=value;
            }
            get
            {
                return p_awardtype;
            }
        }

        /// <summary>
        /// AwardName
        /// </summary>
        public string AwardName
        {
            set
            {
                p_awardname=value;
            }
            get
            {
                return p_awardname;
            }
        }

        /// <summary>
        /// AwardImg
        /// </summary>
        public string AwardImg
        {
            set
            {
                p_awardimg=value;
            }
            get
            {
                return p_awardimg;
            }
        }

        /// <summary>
        /// UnitPrice
        /// </summary>
        public int UnitPrice
        {
            set
            {
                p_unitprice=value;
            }
            get
            {
                return p_unitprice;
            }
        }

        /// <summary>
        /// Gold
        /// </summary>
        public int Gold
        {
            set
            {
                p_gold=value;
            }
            get
            {
                return p_gold;
            }
        }

        /// <summary>
        /// Diamond
        /// </summary>
        public int Diamond
        {
            set
            {
                p_diamond=value;
            }
            get
            {
                return p_diamond;
            }
        }

        /// <summary>
        /// Inventory
        /// </summary>
        public int Inventory
        {
            set
            {
                p_inventory=value;
            }
            get
            {
                return p_inventory;
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

        #endregion
    }
}

