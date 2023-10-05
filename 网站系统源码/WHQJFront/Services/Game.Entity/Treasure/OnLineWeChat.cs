/*
 * 版本： 4.0
 * 日期：2018/7/18 9:55:29
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Treasure
{
    /// <summary>
    /// 实体类 OnLineWeChat  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class OnLineWeChat
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "OnLineWeChat";

        #endregion 

        #region 私有变量

        private int p_configid;
        private string p_configname;
        private string p_wechat;
        private string p_nickname;
        private int p_sortid;
        private byte p_tagid;
        private byte p_nullity;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化OnLineWeChat
        /// </summary>
        public OnLineWeChat() 
        {
            p_configid = 0;
            p_configname = string.Empty;
            p_wechat = string.Empty;
            p_nickname = string.Empty;
            p_sortid = 0;
            p_tagid = 0;
            p_nullity = 0;
            p_collectdate = DateTime.Now;
        }

        #endregion

        #region 公共属性 

        /// <summary>
        /// ConfigID
        /// </summary>
        public int ConfigID
        {
            set
            {
                p_configid=value;
            }
            get
            {
                return p_configid;
            }
        }

        /// <summary>
        /// ConfigName
        /// </summary>
        public string ConfigName
        {
            set
            {
                p_configname=value;
            }
            get
            {
                return p_configname;
            }
        }

        /// <summary>
        /// WeChat
        /// </summary>
        public string WeChat
        {
            set
            {
                p_wechat=value;
            }
            get
            {
                return p_wechat;
            }
        }

        /// <summary>
        /// NickName
        /// </summary>
        public string NickName
        {
            set
            {
                p_nickname=value;
            }
            get
            {
                return p_nickname;
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
        /// TagID
        /// </summary>
        public byte TagID
        {
            set
            {
                p_tagid=value;
            }
            get
            {
                return p_tagid;
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

