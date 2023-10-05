/*
 * 版本： 4.0
 * 日期：2019/12/11 15:34:58
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Accounts
{
    /// <summary>
    /// 实体类 GroupAIParameter  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class GroupAIParameter
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "GroupAIParameter";

        #endregion 

        #region 私有变量

        private int p_recordid;
        private int p_groupid;
        private short p_kindid;
        private int p_aicount;
        private Int64 p_distributetotalscore;
        private int p_playingairatemin;
        private int p_playingairatemax;
        private Int64 p_distirbutesinglescore;
        private Int64 p_distirbutescoreminratio;
        private Int64 p_distirbutescoremaxratio;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化GroupAIParameter
        /// </summary>
        public GroupAIParameter() 
        {
            p_recordid = 0;
            p_groupid = 0;
            p_kindid = 0;
            p_aicount = 0;
            p_distributetotalscore = 0;
            p_playingairatemin = 0;
            p_playingairatemax = 0;
            p_distirbutesinglescore = 0;
            p_distirbutescoreminratio = 0;
            p_distirbutescoremaxratio = 0;
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
        public int GroupID
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
        /// KindID
        /// </summary>
        public short KindID
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
        /// AICount
        /// </summary>
        public int AICount
        {
            set
            {
                p_aicount=value;
            }
            get
            {
                return p_aicount;
            }
        }

        /// <summary>
        /// DistributeTotalScore
        /// </summary>
        public Int64 DistributeTotalScore
        {
            set
            {
                p_distributetotalscore=value;
            }
            get
            {
                return p_distributetotalscore;
            }
        }

        /// <summary>
        /// PlayingAIRateMin
        /// </summary>
        public int PlayingAIRateMin
        {
            set
            {
                p_playingairatemin=value;
            }
            get
            {
                return p_playingairatemin;
            }
        }

        /// <summary>
        /// PlayingAIRateMax
        /// </summary>
        public int PlayingAIRateMax
        {
            set
            {
                p_playingairatemax=value;
            }
            get
            {
                return p_playingairatemax;
            }
        }

        /// <summary>
        /// DistirbuteSingleScore
        /// </summary>
        public Int64 DistirbuteSingleScore
        {
            set
            {
                p_distirbutesinglescore=value;
            }
            get
            {
                return p_distirbutesinglescore;
            }
        }

        /// <summary>
        /// DistirbuteScoreMinRatio
        /// </summary>
        public Int64 DistirbuteScoreMinRatio
        {
            set
            {
                p_distirbutescoreminratio=value;
            }
            get
            {
                return p_distirbutescoreminratio;
            }
        }

        /// <summary>
        /// DistirbuteScoreMaxRatio
        /// </summary>
        public Int64 DistirbuteScoreMaxRatio
        {
            set
            {
                p_distirbutescoremaxratio=value;
            }
            get
            {
                return p_distirbutescoremaxratio;
            }
        }

        #endregion
    }
}

