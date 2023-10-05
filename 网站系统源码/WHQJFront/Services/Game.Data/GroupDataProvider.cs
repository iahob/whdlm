using Game.Kernel;
using Game.Entity.Group;
using Game.IData;
using System.Collections.Generic;
using System.Data.Common;
using System.Data;

// ReSharper disable InconsistentNaming

namespace Game.Data
{
    /// <summary>
    /// 群组数据访问层
    /// </summary>
    public sealed class GroupDataProvider : BaseDataProvider, IGroupDataProvider
    {
        #region Fields

        private readonly ITableProvider _tbIMGroupOptionProvider;
        private readonly ITableProvider _tbIMGroupPropertyProvider;

        #endregion

        #region 构造方法

        public GroupDataProvider(string connString)
            : base(connString)
        {
            _tbIMGroupOptionProvider = GetTableProvider(IMGroupOption.Tablename);
            _tbIMGroupPropertyProvider = GetTableProvider(IMGroupProperty.Tablename);
        }

        #endregion

        #region 公共分页
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <param name="condition">查询条件</param>
        /// <param name="orderby">排序</param>
        /// <returns></returns>
        public PagerSet GetList(string tableName, int pageIndex, int pageSize, string condition, string orderby)
        {
            PagerParameters pagerPrams = new PagerParameters(tableName, orderby, condition, pageIndex, pageSize);
            return GetPagerSet2(pagerPrams);
        }
        #endregion

        #region 系统配置
        /// <summary>
        /// 获取系统配置信息
        /// </summary>
        /// <param name="key">配置键值</param>
        /// <returns></returns>
        public IMGroupOption GetGroupOption(string key)
        {
            return _tbIMGroupOptionProvider.GetObject<IMGroupOption>($"WHERE OptionName = N'{key}'");
        }
        #endregion

        #region 俱乐部信息

        /// <summary>
        /// 获取俱乐部信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        public IMGroupProperty GetGroupInfo(long groupId)
        {
            return _tbIMGroupPropertyProvider.GetObject<IMGroupProperty>($" WHERE GroupID={groupId} ");
        }

        /// <summary>
        /// 俱乐部IP绑定
        /// </summary>
        /// <param name="userIp"></param>
        /// <param name="groupId"></param>
        /// <param name="groupId"></param>
        /// <returns></returns>
        public int GroupIPBind(string userIp, int groupId,long code)
        {
            string sqlQuery = "DELETE FROM dbo.IMGroupBindIP WHERE UserIP=@UserIP AND GroupID=@GroupID;INSERT INTO dbo.IMGroupBindIP( UserIP ,GroupID ,InvitationCode ,InsertTime)VALUES  ( @UserIP,@GroupID,@InvitationCode,GETDATE())";
            List<DbParameter> prams = new List<DbParameter>
            {
                Database.MakeInParam("UserIP", userIp),
                Database.MakeInParam("groupId",groupId),
                Database.MakeInParam("InvitationCode",code),
            };
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        #endregion
    }
}
