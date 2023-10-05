/*
 * 版本： 4.0
 * 日期：2018/5/14 17:38:47
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Record
{
    /// <summary>
    /// 实体类 RecordTask  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class RecordTask
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "RecordTask";

        #endregion 

        #region 私有变量

        private int p_recordid;
        private int p_dateid;
        private int p_userid;
        private int p_taskid;
        private byte p_timetype;
        private string p_taskaward;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化RecordTask
        /// </summary>
        public RecordTask() 
        {
            p_recordid = 0;
            p_dateid = 0;
            p_userid = 0;
            p_taskid = 0;
            p_timetype = 0;
            p_taskaward = "";
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
        /// TaskID
        /// </summary>
        public int TaskID
        {
            set
            {
                p_taskid=value;
            }
            get
            {
                return p_taskid;
            }
        }

        /// <summary>
        /// TimeType
        /// </summary>
        public byte TimeType
        {
            set
            {
                p_timetype=value;
            }
            get
            {
                return p_timetype;
            }
        }

        public string TaskAward
        {
            set
            {
                p_taskaward = value;
            }
            get
            {
                return p_taskaward;
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

