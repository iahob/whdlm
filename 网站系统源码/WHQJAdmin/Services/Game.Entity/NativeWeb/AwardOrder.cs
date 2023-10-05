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
    /// 实体类 AwardOrder  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class AwardOrder
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "AwardOrder";

        #endregion 

        #region 私有变量

        private string p_orderid;
        private int p_userid;
        private int p_gameid;
        private int p_awardid;
        private int p_awardtype;
        private string p_awardname;
        private int p_unitprice;
        private int p_gold;
        private int p_diamond;
        private int p_buynum;
        private int p_payticket;
        private string p_compellation;
        private string p_mobilephone;
        private string p_province;
        private string p_city;
        private string p_area;
        private string p_detailaddress;
        private byte p_orderstatus;
        private string p_clinetip;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化AwardOrder
        /// </summary>
        public AwardOrder() 
        {
            p_orderid = string.Empty;
            p_userid = 0;
            p_gameid = 0;
            p_awardid = 0;
            p_awardtype = 0;
            p_awardname = string.Empty;
            p_unitprice = 0;
            p_gold = 0;
            p_diamond = 0;
            p_buynum = 0;
            p_payticket = 0;
            p_compellation = string.Empty;
            p_mobilephone = string.Empty;
            p_province = string.Empty;
            p_city = string.Empty;
            p_area = string.Empty;
            p_detailaddress = string.Empty;
            p_orderstatus = 0;
            p_clinetip = string.Empty;
            p_collectdate = DateTime.Now;
        }

        #endregion

        #region 公共属性 

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
        /// GameID
        /// </summary>
        public int GameID
        {
            set
            {
                p_gameid=value;
            }
            get
            {
                return p_gameid;
            }
        }



        /// <summary>
        /// AwardID
        /// </summary>
        public int AwardID
        {
            set
            {
                p_awardid = value;
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
        /// BuyNum
        /// </summary>
        public int BuyNum
        {
            set
            {
                p_buynum=value;
            }
            get
            {
                return p_buynum;
            }
        }

        /// <summary>
        /// PayTicket
        /// </summary>
        public int PayTicket
        {
            set
            {
                p_payticket=value;
            }
            get
            {
                return p_payticket;
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

