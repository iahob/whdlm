/*
 * 版本： 4.0
 * 日期：2018/8/8 16:15:54
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.NativeWeb
{
    /// <summary>
    /// 实体类 ShareLog  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class ShareLog
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "ShareLog";

        #endregion 

        #region 私有变量

        private Int64 p_id;
        private Int64 p_userid;
        private Int16 p_logtype;
        private int p_dateid;
        private int p_timesharegold;
        private int p_timesharediamond;
        private string p_remark;
        private DateTime? p_logtime;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化ShareLog
        /// </summary>
        public ShareLog() 
        {
            p_id = 0;
            p_userid = 0;
            p_logtype = 0;
            p_dateid = 0;
            p_timesharegold = 0;
            p_timesharediamond = 0;
            p_remark = string.Empty;
            p_logtime = null;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// ID
        /// </summary>
        public Int64 ID
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
        /// UserID
        /// </summary>
        public Int64 UserID
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
        /// LogType
        /// </summary>
        public Int16 LogType
        {
            set
            {
                p_logtype=value;
            }
            get
            {
                return p_logtype;
            }
        }

        /// <summary>
        /// DateID
        /// </summary>
        public int DateID
        {
            set
            {
                p_dateid=value;
            }
            get
            {
                return p_dateid;
            }
        }

        /// <summary>
        /// TimeShareGold
        /// </summary>
        public int TimeShareGold
        {
            set
            {
                p_timesharegold=value;
            }
            get
            {
                return p_timesharegold;
            }
        }

        /// <summary>
        /// TimeShareDiamond
        /// </summary>
        public int TimeShareDiamond
        {
            set
            {
                p_timesharediamond=value;
            }
            get
            {
                return p_timesharediamond;
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
        /// LogTime
        /// </summary>
        public DateTime? LogTime
        {
            set
            {
                p_logtime=value;
            }
            get
            {
                return p_logtime;
            }
        }

        #endregion
    }
}

