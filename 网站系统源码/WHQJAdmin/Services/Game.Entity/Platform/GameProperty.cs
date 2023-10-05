/*
 * 版本： 4.0
 * 日期：2018/7/11 15:28:29
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Platform
{
    /// <summary>
    /// 实体类 GameProperty  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class GameProperty
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "GameProperty";

        #endregion 

        #region 私有变量

        private int p_id;
        private byte p_exchangetype;
        private int p_exchangeratio;
        private string p_name;
        private int p_kind;
        private byte p_usearea;
        private byte p_servicearea;
        private Int64 p_buyresultsgold;
        private Int64 p_sendloveliness;
        private Int64 p_recvloveliness;
        private Int64 p_useresultsgold;
        private int p_useresultsvalidtime;
        private int p_useresultsvalidtimescoremultiple;
        private int p_useresultsgiftpackage;
        private string p_regulationsinfo;
        private byte p_recommend;
        private int p_sortid;
        private byte p_nullity;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化GameProperty
        /// </summary>
        public GameProperty() 
        {
            p_id = 0;
            p_exchangetype = 0;
            p_exchangeratio = 0;
            p_name = string.Empty;
            p_kind = 0;
            p_usearea = 0;
            p_servicearea = 0;
            p_buyresultsgold = 0;
            p_sendloveliness = 0;
            p_recvloveliness = 0;
            p_useresultsgold = 0;
            p_useresultsvalidtime = 0;
            p_useresultsvalidtimescoremultiple = 0;
            p_useresultsgiftpackage = 0;
            p_regulationsinfo = string.Empty;
            p_recommend = 0;
            p_sortid = 0;
            p_nullity = 0;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// ID
        /// </summary>
        public int ID
        {
            set
            {
                p_id=value;
            }
            get
            {
                return p_id;
            }
        }

        /// <summary>
        /// ExchangeType
        /// </summary>
        public byte ExchangeType
        {
            set
            {
                p_exchangetype=value;
            }
            get
            {
                return p_exchangetype;
            }
        }

        /// <summary>
        /// ExchangeRatio
        /// </summary>
        public int ExchangeRatio
        {
            set
            {
                p_exchangeratio=value;
            }
            get
            {
                return p_exchangeratio;
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
        /// Kind
        /// </summary>
        public int Kind
        {
            set
            {
                p_kind=value;
            }
            get
            {
                return p_kind;
            }
        }

        /// <summary>
        /// UseArea
        /// </summary>
        public byte UseArea
        {
            set
            {
                p_usearea=value;
            }
            get
            {
                return p_usearea;
            }
        }

        /// <summary>
        /// ServiceArea
        /// </summary>
        public byte ServiceArea
        {
            set
            {
                p_servicearea=value;
            }
            get
            {
                return p_servicearea;
            }
        }

        /// <summary>
        /// BuyResultsGold
        /// </summary>
        public Int64 BuyResultsGold
        {
            set
            {
                p_buyresultsgold=value;
            }
            get
            {
                return p_buyresultsgold;
            }
        }

        /// <summary>
        /// SendLoveLiness
        /// </summary>
        public Int64 SendLoveLiness
        {
            set
            {
                p_sendloveliness=value;
            }
            get
            {
                return p_sendloveliness;
            }
        }

        /// <summary>
        /// RecvLoveLiness
        /// </summary>
        public Int64 RecvLoveLiness
        {
            set
            {
                p_recvloveliness=value;
            }
            get
            {
                return p_recvloveliness;
            }
        }

        /// <summary>
        /// UseResultsGold
        /// </summary>
        public Int64 UseResultsGold
        {
            set
            {
                p_useresultsgold=value;
            }
            get
            {
                return p_useresultsgold;
            }
        }

        /// <summary>
        /// UseResultsValidTime
        /// </summary>
        public int UseResultsValidTime
        {
            set
            {
                p_useresultsvalidtime=value;
            }
            get
            {
                return p_useresultsvalidtime;
            }
        }

        /// <summary>
        /// UseResultsValidTimeScoreMultiple
        /// </summary>
        public int UseResultsValidTimeScoreMultiple
        {
            set
            {
                p_useresultsvalidtimescoremultiple=value;
            }
            get
            {
                return p_useresultsvalidtimescoremultiple;
            }
        }

        /// <summary>
        /// UseResultsGiftPackage
        /// </summary>
        public int UseResultsGiftPackage
        {
            set
            {
                p_useresultsgiftpackage=value;
            }
            get
            {
                return p_useresultsgiftpackage;
            }
        }

        /// <summary>
        /// RegulationsInfo
        /// </summary>
        public string RegulationsInfo
        {
            set
            {
                p_regulationsinfo=value;
            }
            get
            {
                return p_regulationsinfo;
            }
        }

        /// <summary>
        /// Recommend
        /// </summary>
        public byte Recommend
        {
            set
            {
                p_recommend=value;
            }
            get
            {
                return p_recommend;
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

        #endregion
    }
}

