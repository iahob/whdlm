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
    /// 实体类 GamePackageGoods  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class GamePackageGoods
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "GamePackageGoods";

        #endregion 

        #region 私有变量

        private int p_goodsid;
        private int p_packageid;
        private byte p_typeid;
        private int p_propertyid;
        private Int64 p_goodsnum;
        private string p_resourceurl;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化GamePackageGoods
        /// </summary>
        public GamePackageGoods() 
        {
            p_goodsid = 0;
            p_packageid = 0;
            p_typeid = 0;
            p_propertyid = 0;
            p_goodsnum = 0;
            p_resourceurl = string.Empty;
            p_collectdate = DateTime.Now;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// GoodsID
        /// </summary>
        public int GoodsID
        {
            set
            {
                p_goodsid=value;
            }
            get
            {
                return p_goodsid;
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
        /// PropertyID
        /// </summary>
        public int PropertyID
        {
            set
            {
                p_propertyid=value;
            }
            get
            {
                return p_propertyid;
            }
        }

        /// <summary>
        /// GoodsNum
        /// </summary>
        public Int64 GoodsNum
        {
            set
            {
                p_goodsnum=value;
            }
            get
            {
                return p_goodsnum;
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

