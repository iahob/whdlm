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
    /// 实体类 RecordGroupScorePresent  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class RecordGroupScorePresent
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "RecordGroupScorePresent";

        #endregion 

        #region 私有变量

        private int p_recordid;
        private Int64 p_groupid;
        private int p_presentuserid;
        private int p_presentgameid;
        private string p_presentnickname;
        private int p_receiveuserid;
        private int p_receivegameid;
        private string p_receivenickname;
        private Int64 p_curscore;
        private byte p_changetype;
        private Int64 p_changescore;
        private DateTime p_collectdate;
        private byte p_presentuserright;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化RecordGroupScorePresent
        /// </summary>
        public RecordGroupScorePresent() 
        {
            p_recordid = 0;
            p_groupid = 0;
            p_presentuserid = 0;
            p_presentgameid = 0;
            p_presentnickname = string.Empty;
            p_receiveuserid = 0;
            p_receivegameid = 0;
            p_receivenickname = string.Empty;
            p_curscore = 0;
            p_changetype = 0;
            p_changescore = 0;
            p_collectdate = DateTime.Now;
            p_presentuserright = 0;
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
        /// PresentUserID
        /// </summary>
        public int PresentUserID
        {
            set
            {
                p_presentuserid=value;
            }
            get
            {
                return p_presentuserid;
            }
        }

        /// <summary>
        /// PresentGameID
        /// </summary>
        public int PresentGameID
        {
            set
            {
                p_presentgameid=value;
            }
            get
            {
                return p_presentgameid;
            }
        }

        /// <summary>
        /// PresentNickName
        /// </summary>
        public string PresentNickName
        {
            set
            {
                p_presentnickname=value;
            }
            get
            {
                return p_presentnickname;
            }
        }

        /// <summary>
        /// ReceiveUserID
        /// </summary>
        public int ReceiveUserID
        {
            set
            {
                p_receiveuserid=value;
            }
            get
            {
                return p_receiveuserid;
            }
        }

        /// <summary>
        /// ReceiveGameID
        /// </summary>
        public int ReceiveGameID
        {
            set
            {
                p_receivegameid=value;
            }
            get
            {
                return p_receivegameid;
            }
        }

        /// <summary>
        /// ReceiveNickName
        /// </summary>
        public string ReceiveNickName
        {
            set
            {
                p_receivenickname=value;
            }
            get
            {
                return p_receivenickname;
            }
        }

        /// <summary>
        /// CurScore
        /// </summary>
        public Int64 CurScore
        {
            set
            {
                p_curscore=value;
            }
            get
            {
                return p_curscore;
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
        /// PresentUserRight
        /// </summary>
        public byte PresentUserRight
        {
            set
            {
                p_presentuserright=value;
            }
            get
            {
                return p_presentuserright;
            }
        }

        #endregion
    }
}

