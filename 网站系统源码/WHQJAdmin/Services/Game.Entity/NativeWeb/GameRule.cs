/*
 * 版本： 4.0
 * 日期：2017/8/28 10:06:58
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.NativeWeb
{
    /// <summary>
    /// 实体类 GameRule  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class GameRule
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "GameRule";

        #endregion 

        #region 私有变量
        private int p_id;
        private int p_kindid;
        private string p_kindname;
        private string p_kindicon;
        private string p_kindintro;
        private string p_kindrule;
        private string p_ruleimg;
        private int p_sortid;
        private byte p_nullity; 
        private int p_type;
        private int p_logid;
        private DateTime p_collectdate;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化GameRule
        /// </summary>
        public GameRule() 
        {
            p_id = 0;
            p_kindid = 0;
            p_kindname = string.Empty;
            p_kindicon = string.Empty;
            p_kindintro = string.Empty;
            p_kindrule = string.Empty;
            p_ruleimg = string.Empty;
            p_type = 0;
            p_logid = 0;
            p_nullity = 0;
            p_sortid = 0;
            p_collectdate = DateTime.Now;
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
                p_id = value;
            }
            get
            {
                return p_id;
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
        /// KindName
        /// </summary>
        public string KindName
        {
            set
            {
                p_kindname=value;
            }
            get
            {
                return p_kindname;
            }
        }

        /// <summary>
        /// KindIcon
        /// </summary>
        public string KindIcon
        {
            set
            {
                p_kindicon=value;
            }
            get
            {
                return p_kindicon;
            }
        }

        /// <summary>
        /// KindIntro
        /// </summary>
        public string KindIntro
        {
            set
            {
                p_kindintro=value;
            }
            get
            {
                return p_kindintro;
            }
        }

        /// <summary>
        /// KindRule
        /// </summary>
        public string KindRule
        {
            set
            {
                p_kindrule=value;
            }
            get
            {
                return p_kindrule;
            }
        }


        /// <summary>
        /// RuleImg
        /// </summary>
        public string RuleImg
        {
            set
            {
                p_ruleimg = value;
            }
            get
            {
                return p_ruleimg;
            }
        }
        /// <summary>
        /// Type
        /// </summary>
        public int Type
        {
            set
            {
                p_type = value;
            }
            get
            {
                return p_type;
            }
        }
        /// <summary>
        /// LogID
        /// </summary>
        public int LogID
        {
            set
            {
                p_logid = value;
            }
            get
            {
                return p_logid;
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
        /// 玩法排序
        /// </summary>
        public int SortID
        {
            set { p_sortid = value; }
            get { return p_sortid; }
        }

        #endregion
    }
}

