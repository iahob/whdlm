/*
 * 版本： 4.0
 * 日期：2019/8/8 15:58:26
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Group
{
    /// <summary>
    /// 实体类 RecordUserScoreChange  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class RecordUserScoreChange
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "RecordUserScoreChange";

        #endregion 

        #region 私有变量

        private int p_recordid;
        private Int64 p_groupid;
        private int p_userid;
        private Int64 p_beforescore;
        private byte p_changetype;
        private Int64 p_changescore;
        private Int64 p_afterscore;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化RecordUserScoreChange
        /// </summary>
        public RecordUserScoreChange() 
        {
            p_recordid = 0;
            p_groupid = 0;
            p_userid = 0;
            p_beforescore = 0;
            p_changetype = 0;
            p_changescore = 0;
            p_afterscore = 0;
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
        /// GroupID
        /// </summary>
        public Int64 GroupID
        {
            set
            {
                p_groupid=value;
            }
            get
            {
                return p_groupid;
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
        /// BeforeScore
        /// </summary>
        public Int64 BeforeScore
        {
            set
            {
                p_beforescore=value;
            }
            get
            {
                return p_beforescore;
            }
        }

        /// <summary>
        /// ChangeType
        /// </summary>
        public byte ChangeType
        {
            set
            {
                p_changetype=value;
            }
            get
            {
                return p_changetype;
            }
        }

        /// <summary>
        /// ChangeScore
        /// </summary>
        public Int64 ChangeScore
        {
            set
            {
                p_changescore=value;
            }
            get
            {
                return p_changescore;
            }
        }

        /// <summary>
        /// AfterScore
        /// </summary>
        public Int64 AfterScore
        {
            set
            {
                p_afterscore=value;
            }
            get
            {
                return p_afterscore;
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

