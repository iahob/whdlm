/*
 * 版本： 4.0
 * 日期：2018/7/4 17:24:42
 * 
 * 描述：实体类
 * 
 */

using System;
using System.Collections.Generic;

namespace Game.Entity.NativeWeb
{
    /// <summary>
    /// 实体类 CacheGameRank  (属性说明自动提取数据库字段的描述信息)
    /// </summary>
    [Serializable]
    public partial class CacheGameRank
    {
        #region 常量 

        /// <summary>
        /// 表名
        /// </summary>
        public const string Tablename = "CacheGameRank";

        #endregion 

        #region 私有变量
        private int p_ranknum;
        private int p_userid;
        private int p_gameid;
        private string p_nickname;
        private string p_faceurl;
        private int p_faceid;
        private int p_gender;
        private string p_underwrite;
        private Int64 p_wincount;

        #endregion

        #region 构造函数 

        /// <summary>
        /// 初始化CacheGameRank
        /// </summary>
        public CacheGameRank() 
        {
            p_userid = 0;
            p_gameid = 0;
            p_nickname = string.Empty;
            p_faceurl = string.Empty;
            p_faceid = 0;
            p_gender = 0;
            p_underwrite = string.Empty;
            p_ranknum = 0;
            p_wincount = 0;
        }

        #endregion

        #region 公共属性 
        /// <summary>
        /// RankNum
        /// </summary>
        public int RankNum
        {
            set
            {
                p_ranknum = value;
            }
            get
            {
                return p_ranknum;
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
        /// GameID
        /// </summary>
        public int GameID
        {
            set
            {
                p_gameid=value;
            }
            get
            {
                return p_gameid;
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
        /// FaceUrl
        /// </summary>
        public string FaceUrl
        {
            set
            {
                p_faceurl=value;
            }
            get
            {
                return p_faceurl;
            }
        }

        /// <summary>
        /// FaceID
        /// </summary>
        public int FaceID
        {
            set
            {
                p_faceid=value;
            }
            get
            {
                return p_faceid;
            }
        }

        /// <summary>
        /// Gender
        /// </summary>
        public int Gender
        {
            set
            {
                p_gender = value;
            }
            get
            {
                return p_gender;
            }
        }

        /// <summary>
        /// UnderWrite
        /// </summary>
        public string UnderWrite
        {
            set
            {
                p_underwrite=value;
            }
            get
            {
                return p_underwrite;
            }
        }

        /// <summary>
        /// WinCount
        /// </summary>
        public Int64 WinCount
        {
            set
            {
                p_wincount=value;
            }
            get
            {
                return p_wincount;
            }
        }

        #endregion
    }
}

