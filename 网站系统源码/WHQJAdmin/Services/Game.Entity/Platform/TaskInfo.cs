/*
 * 版本： 4.0
 * 日期：2018/5/14 11:07:40
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Platform
{
    /// <summary>
    /// 实体类 TaskInfo  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class TaskInfo
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "TaskInfo";

        #endregion 

        #region 私有变量

        private int p_taskid;
        private string p_taskname;
        private string p_taskdescription;
        private int p_tasktype;
        private byte p_usertype;
        private byte p_timetype;
        private int p_kindid;
        private int p_serverid;
        private int p_matchid;
        private string p_taskaward;
        private int p_innings;
        private byte p_nullity;
        private DateTime p_collectdate;
        private int p_sortid;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化TaskInfo
        /// </summary>
        public TaskInfo() 
        {
            p_taskid = 0;
            p_taskname = string.Empty;
            p_taskdescription = string.Empty;
            p_tasktype = 0;
            p_usertype = 0;
            p_timetype = 0;
            p_kindid = 0;
            p_serverid = 0;
            p_matchid = 0;
            p_taskaward = "";
            p_innings = 0;
            p_nullity = 0;
            p_collectdate = DateTime.Now;
            p_sortid = 0;
        }

        #endregion

        #region 公共属性 

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
        /// TaskName
        /// </summary>
        public string TaskName
        {
            set
            {
                p_taskname=value;
            }
            get
            {
                return p_taskname;
            }
        }

        /// <summary>
        /// TaskDescription
        /// </summary>
        public string TaskDescription
        {
            set
            {
                p_taskdescription=value;
            }
            get
            {
                return p_taskdescription;
            }
        }

        /// <summary>
        /// TaskType
        /// </summary>
        public int TaskType
        {
            set
            {
                p_tasktype=value;
            }
            get
            {
                return p_tasktype;
            }
        }

        /// <summary>
        /// UserType
        /// </summary>
        public byte UserType
        {
            set
            {
                p_usertype=value;
            }
            get
            {
                return p_usertype;
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

        /// <summary>
        /// KindID
        /// </summary>
        public int KindID
        {
            set
            {
                p_kindid=value;
            }
            get
            {
                return p_kindid;
            }
        }

        /// <summary>
        /// ServerID
        /// </summary>
        public int ServerID
        {
            set
            {
                p_serverid=value;
            }
            get
            {
                return p_serverid;
            }
        }

        /// <summary>
        /// MatchID
        /// </summary>
        public int MatchID
        {
            set
            {
                p_matchid=value;
            }
            get
            {
                return p_matchid;
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
        /// Innings
        /// </summary>
        public int Innings
        {
            set
            {
                p_innings=value;
            }
            get
            {
                return p_innings;
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

        /// <summary>
        /// SortID
        /// </summary>
        public int SortID
        {
            set
            {
                p_sortid = value;
            }
            get
            {
                return p_sortid;
            }
        }

        #endregion
    }
}

