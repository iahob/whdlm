/*
 * 版本： 4.0
 * 日期：2019/11/7 20:09:00
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.NativeWeb
{
    /// <summary>
    /// 实体类 WxSacnLoginRecord  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class WxScanLoginRecord
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "WxScanLoginRecord";

        #endregion 

        #region 私有变量

        private int p_recordid;
        private string p_key;
        private string p_openid;
        private string p_unionid;
        private string p_accesstoken;
        private string p_refreshtoken;
        private byte p_state;
        private DateTime p_collectdate;
        private string p_collectclientip;
        private DateTime? p_pickdate;
        private string p_pickclientip;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化WxSacnLoginRecord
        /// </summary>
        public WxScanLoginRecord() 
        {
            p_recordid = 0;
            p_key = string.Empty;
            p_openid = string.Empty;
            p_unionid = string.Empty;
            p_accesstoken = string.Empty;
            p_refreshtoken = string.Empty;
            p_state = 0;
            p_collectdate = DateTime.Now;
            p_collectclientip = string.Empty;
            p_pickdate = null;
            p_pickclientip = string.Empty;
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
        /// Key
        /// </summary>
        public string Key
        {
            set
            {
                p_key=value;
            }
            get
            {
                return p_key;
            }
        }

        /// <summary>
        /// OpenID
        /// </summary>
        public string OpenID
        {
            set
            {
                p_openid=value;
            }
            get
            {
                return p_openid;
            }
        }

        /// <summary>
        /// UnionID
        /// </summary>
        public string UnionID
        {
            set
            {
                p_unionid=value;
            }
            get
            {
                return p_unionid;
            }
        }

        /// <summary>
        /// AccessToken
        /// </summary>
        public string AccessToken
        {
            set
            {
                p_accesstoken=value;
            }
            get
            {
                return p_accesstoken;
            }
        }

        /// <summary>
        /// RefreshToken
        /// </summary>
        public string RefreshToken
        {
            set
            {
                p_refreshtoken=value;
            }
            get
            {
                return p_refreshtoken;
            }
        }

        /// <summary>
        /// State
        /// </summary>
        public byte State
        {
            set
            {
                p_state=value;
            }
            get
            {
                return p_state;
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
        /// CollectClientIP
        /// </summary>
        public string CollectClientIP
        {
            set
            {
                p_collectclientip=value;
            }
            get
            {
                return p_collectclientip;
            }
        }

        /// <summary>
        /// PickDate
        /// </summary>
        public DateTime? PickDate
        {
            set
            {
                p_pickdate=value;
            }
            get
            {
                return p_pickdate;
            }
        }

        /// <summary>
        /// PickClientIP
        /// </summary>
        public string PickClientIP
        {
            set
            {
                p_pickclientip=value;
            }
            get
            {
                return p_pickclientip;
            }
        }

        #endregion
    }
}

