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
    /// 实体类 IMGroupShareStorageProperty  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class IMGroupShareStorageProperty
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "IMGroupShareStorageProperty";

        #endregion 

        #region 私有变量

        private Int64 p_groupid;
        private int p_kindid;
        private byte p_playmode;
        private string p_configsubstorageitem;
        private string p_cursubstorageitem;
        private Int64 p_configsysstorage;
        private Int64 p_configplayerstorage;
        private Int64 p_configparameterk;
        private Int64 p_curresetcount;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化IMGroupShareStorageProperty
        /// </summary>
        public IMGroupShareStorageProperty() 
        {
            p_groupid = 0;
            p_kindid = 0;
            p_playmode = 0;
            p_configsubstorageitem = string.Empty;
            p_cursubstorageitem = string.Empty;
            p_configsysstorage = 0;
            p_configplayerstorage = 0;
            p_configparameterk = 0;
            p_curresetcount = 0;
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
        /// PlayMode
        /// </summary>
        public byte PlayMode
        {
            set
            {
                p_playmode=value;
            }
            get
            {
                return p_playmode;
            }
        }

        /// <summary>
        /// ConfigSubStorageItem
        /// </summary>
        public string ConfigSubStorageItem
        {
            set
            {
                p_configsubstorageitem=value;
            }
            get
            {
                return p_configsubstorageitem;
            }
        }

        /// <summary>
        /// CurSubStorageItem
        /// </summary>
        public string CurSubStorageItem
        {
            set
            {
                p_cursubstorageitem=value;
            }
            get
            {
                return p_cursubstorageitem;
            }
        }

        /// <summary>
        /// ConfigSysStorage
        /// </summary>
        public Int64 ConfigSysStorage
        {
            set
            {
                p_configsysstorage=value;
            }
            get
            {
                return p_configsysstorage;
            }
        }

        /// <summary>
        /// ConfigPlayerStorage
        /// </summary>
        public Int64 ConfigPlayerStorage
        {
            set
            {
                p_configplayerstorage=value;
            }
            get
            {
                return p_configplayerstorage;
            }
        }

        /// <summary>
        /// ConfigParameterK
        /// </summary>
        public Int64 ConfigParameterK
        {
            set
            {
                p_configparameterk=value;
            }
            get
            {
                return p_configparameterk;
            }
        }

        /// <summary>
        /// CurResetCount
        /// </summary>
        public Int64 CurResetCount
        {
            set
            {
                p_curresetcount=value;
            }
            get
            {
                return p_curresetcount;
            }
        }

        #endregion
    }
}

