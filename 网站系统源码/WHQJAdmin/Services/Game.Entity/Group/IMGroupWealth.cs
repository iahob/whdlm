/*
 * 版本： 4.0
 * 日期：2019/12/18 17:15:44
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.Group
{
    /// <summary>
    /// 实体类 IMGroupWealth  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class IMGroupWealth
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "IMGroupWealth";

        #endregion 

        #region 私有变量

        private Int64 p_groupid;
        private Int64 p_ingot;
        private Int64 p_consumeingot;
        private int p_battlecreate;
        private Int64 p_score;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化IMGroupWealth
        /// </summary>
        public IMGroupWealth() 
        {
            p_groupid = 0;
            p_ingot = 0;
            p_consumeingot = 0;
            p_battlecreate = 0;
            p_score = 0;
        }

        #endregion

        #region 公共属性 

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
        /// Ingot
        /// </summary>
        public Int64 Ingot
        {
            set
            {
                p_ingot=value;
            }
            get
            {
                return p_ingot;
            }
        }

        /// <summary>
        /// ConsumeIngot
        /// </summary>
        public Int64 ConsumeIngot
        {
            set
            {
                p_consumeingot=value;
            }
            get
            {
                return p_consumeingot;
            }
        }

        /// <summary>
        /// BattleCreate
        /// </summary>
        public int BattleCreate
        {
            set
            {
                p_battlecreate=value;
            }
            get
            {
                return p_battlecreate;
            }
        }

        /// <summary>
        /// Score
        /// </summary>
        public Int64 Score
        {
            set
            {
                p_score=value;
            }
            get
            {
                return p_score;
            }
        }

        #endregion
    }
}

