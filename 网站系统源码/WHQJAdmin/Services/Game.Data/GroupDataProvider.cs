using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using Game.Entity.Accounts;
using Game.IData;
using Game.Kernel;
using Game.Entity.Agent;
using Game.Entity.Group;

namespace Game.Data
{
    /// <summary>
    /// 代理库数据层
    /// </summary>
    public class GroupDataProvider : BaseDataProvider, IGroupDataProvider
    {
        #region 构造方法

        /// <summary>
        /// 构造函数
        /// </summary>
        public GroupDataProvider(string connString)
            : base(connString)
        {
        }

        #endregion 构造方法

        #region 公用分页
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <param name="fields">查询字段</param>
        /// <returns></returns>
        public PagerSet GetList(string tableName, string where, string order, int pageIndex, int pageSize, string[] fields = null)
        {
            PagerParameters pagerPrams = new PagerParameters(tableName, where, order, pageIndex, pageSize, fields);
            return GetPagerSet(pagerPrams);
        }
        /// <summary>
        /// 获取汇总统计
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <param name="field">汇总字段</param>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public long GetTotal(string tableName, string field, string where) {
            string sql = $"SELECT ISNULL(SUM([{field}]),0) AS [Total{field}] FROM {tableName}(NOLOCK) {where}";
            object o = Database.ExecuteScalar(CommandType.Text, sql);
            return Convert.ToInt64(o);
        }
        #endregion 公用分页

        #region 系统配置

        /// <summary>
        /// 获取配置信息
        /// </summary>
        /// <param name="optionName">配置键值</param>
        /// <returns></returns>
        public IMGroupOption GetGroupOption(string optionName)
        {
            string sqlQuery =
                $"SELECT * FROM {IMGroupOption.Tablename} WITH(NOLOCK) WHERE {IMGroupOption._OptionName}=@OptionName";
            var prams = new List<DbParameter> {Database.MakeInParam("OptionName", optionName)};
            return Database.ExecuteObject<IMGroupOption>(sqlQuery, prams);
        }

        /// <summary>
        /// 修改配置信息
        /// </summary>
        /// <param name="optionInfo">配置信息</param>
        /// <returns></returns>
        public int UpdateGroupOption(IMGroupOption optionInfo)
        {
            string sqlQuery = @"UPDATE IMGroupOption SET OptionValue=@OptionValue,SortID=@SortID,
                    OptionTip=@OptionTip,OptionDescribe=@OptionDescribe WHERE OptionName=@OptionName";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("OptionValue", optionInfo.OptionValue),
                Database.MakeInParam("SortID", optionInfo.SortID),
                Database.MakeInParam("OptionTip", optionInfo.OptionTip),
                Database.MakeInParam("OptionDescribe", optionInfo.OptionDescribe),
                Database.MakeInParam("OptionName", optionInfo.OptionName)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        #endregion

        #region 大联盟信息
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("IMGroupProperty", where, order, pageIndex, pageSize, null, "WEB_PageView_Group");
            return GetPagerSet(pagerPrams);
        }

        
        /// <summary>
        /// 获取大联盟玩家勋章流水记录
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        public PagerSet GetPageGroupUserScoreChangeList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("RecordUserScoreChange", where, order, pageIndex, pageSize, null, "WEB_PageView_GroupUserScoreChange");
            return GetPagerSet(pagerPrams);
        }

        /// <summary>
        /// 获取大联盟玩家勋章提取记录
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        public PagerSet GetPageGroupBoxScoreList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("RecordBoxScore", where, order, pageIndex, pageSize, null, "WEB_PageView_GroupBoxScore");
            return GetPagerSet(pagerPrams);
        }
        /// <summary>
        /// 大联盟战绩分页
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        public PagerSet GetPageGroupDrawList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("RecordGroupDrawInfo", where, order, pageIndex, pageSize, null, "WEB_PageView_GroupDrawInfo");
            return GetPagerSet(pagerPrams);
        }
        /// <summary>
        /// 获取大联盟亲友列表
        /// </summary>
        /// <param name="groupid">大联盟标识</param>
        /// <returns></returns>
        public DataSet GetGroupMemberList(long groupid)
        {
            string sqlQuery = string.Format(@"SELECT UserID,BattleCount,JoinDateTime,Score,MemberRight,CaptainID FROM IMGroupMember(NOLOCK) WHERE GroupID={0};
                SELECT UserID,GameID,Accounts,NickName FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID IN(SELECT UserID FROM IMGroupMember(NOLOCK) WHERE GroupID={0})",
                groupid);
            //string sqlQuery = string.Format(@"SELECT [MemberID] AS UserID,BattleCount,JoinDateTime FROM IMGroupMember(NOLOCK) WHERE GroupID={0};
            //    SELECT UserID,GameID,Accounts,NickName FROM WHQJ.dbo.AccountsInfo(NOLOCK) WHERE UserID IN(SELECT [MemberID] FROM IMGroupMember(NOLOCK) WHERE GroupID={0})",
            //    groupid);
            return Database.ExecuteDataset(sqlQuery);
        }

        /// <summary>
        /// 获取大联盟信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        public IMGroupProperty GetGroupInfo(long groupId)
        {
            return Database.ExecuteObject<IMGroupProperty>(
                $"SELECT * FROM {IMGroupProperty.Tablename}(NOLOCK) WHERE GroupID ={groupId} ");
        }

        /// <summary>
        /// 批量冻结、解冻大联盟
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：解冻 1：冻结</param>
        /// <returns></returns>
        public int NullityGroup(string ids, byte nullity)
        {
            return Database.ExecuteNonQuery(CommandType.Text,
                $" UPDATE IMGroupProperty SET GroupStatus = {nullity} WHERE GroupID IN ({ids})");
        }

        /// <summary>
        /// 批量开启、关闭大联盟模式
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：关闭 1：开始</param>
        /// <returns></returns>
        public int CanUseMedalGroup(string ids, byte nullity)
        {
            return Database.ExecuteNonQuery(CommandType.Text,
                $" UPDATE IMGroupProperty SET CanUseMedal = {nullity} WHERE GroupID IN ({ids})");
        }

        /// <summary>
        /// 批量强制解散
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <returns></returns>
        public int DeleteGroup(string ids)
        {
            return Database.ExecuteNonQuery(CommandType.Text, $"DELETE IMGroupProperty WHERE GroupID IN ({ids})");
        }

        /// <summary>
        /// 移交会长
        /// </summary>
        /// <param name="info">群组信息</param>
        /// <returns></returns>
        public int UpdateGroup(IMGroupProperty info)
        {
            string sql = $"UPDATE {IMGroupProperty.Tablename} " +
                         $"  SET {IMGroupProperty._CreaterID}={info.CreaterID} " +
                         $"         ,{IMGroupProperty._CreaterGameID}={info.CreaterGameID} " +
                         $"         ,{IMGroupProperty._CreaterNickName}='{info.CreaterNickName}' " +
                         $" WHERE {IMGroupProperty._GroupID}={info.GroupID}";
            return Database.ExecuteNonQuery(CommandType.Text,sql);
        }

        /// <summary>
        /// 获取大联盟在线人数（金币房间与比赛房间）
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        public int GetGroupMemberOnline(long groupId)
        {
            string sql =
                $"DECLARE @treasureOnline INT SELECT @treasureOnline = COUNT(1) FROM WHQJTreasureDB.DBO.GameScoreLocker WHERE UserID IN (SELECT UserID FROM IMUserGroupInfo WHERE CHARINDEX(';{groupId};',GroupIDArray)>0) " +
                $"DECLARE @matchOnline INT SELECT @matchOnline = COUNT(1) FROM WHQJGameMatchDB.DBO.GameScoreLocker WHERE UserID IN (SELECT UserID FROM IMUserGroupInfo WHERE CHARINDEX(';{groupId};',GroupIDArray)>0) " +
                "SELECT @treasureOnline+@matchOnline AS memberOnline";
            return (int) Database.ExecuteScalar(CommandType.Text, sql);
        }

        /// <summary>
        /// 获取大联盟钻石数量
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        public long GetGroupWealth(long groupId)
        {
            return (long) Database.ExecuteScalar(CommandType.Text,
                $"SELECT Ingot FROM IMGroupWealth WHERE GroupID = {groupId}");
        }

        /// <summary>
        /// 获取大联盟约战统计 
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="userId">用户标识（两个维度：userId为0是为大联盟统计，大于0时为该用户在该大联盟的统计）</param>
        /// <returns></returns>
        public StreamGroupBalance GetGroupBattleRoomStream(long groupId, int userId=0)
        {
            string sql =
                $"SELECT ISNULL(SUM(BattleCount),0) AS BattleCount,ISNULL(SUM(BalanceCount),0) AS BalanceCount,ISNULL(MAX(CollectDate),GETDATE()) AS CollectDate FROM StreamGroupBalance WHERE GroupID = {groupId} ";
            if (userId > 0) sql += $" AND UserID = {userId}";
            else
            {
                sql += " GROUP BY GroupID ";
            }
            StreamGroupBalance sgb = Database.ExecuteObject<StreamGroupBalance>(sql) ?? new StreamGroupBalance();
            sgb.GroupID = groupId;
            sgb.UserID = userId;
            return sgb;
        }

        /// <summary>
        /// 获取大联盟约战当前房间数
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="roomStatus">房间状态</param>
        /// <returns></returns>
        public int GetGroupBattleRoomCurrent(long groupId, int roomStatus = -1)
        {
            string sql =
                $"SELECT COUNT(1) FROM WHQJPlatformDB.DBO.StreamCreateTableFeeInfo WHERE GroupID = {groupId} ";
            if (roomStatus > -1) sql += $" AND RoomStatus = {roomStatus}";
            return (int) Database.ExecuteScalar(CommandType.Text, sql);
        }

        /// <summary>
        /// 后台赠送勋章
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="medal">勋章数</param>
        /// <param name="clientIp">操作地址</param>
        /// <param name="masterId">操作管理员</param>
        /// <returns></returns>
        public Message AddMedal(int groupId, int medal, string clientIp, int masterId)
        {
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("MasterID",masterId),
                Database.MakeInParam("ClientIP",clientIp),
                Database.MakeInParam("GroupID",groupId),
                Database.MakeInParam("AddMedal",medal),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };

            return MessageHelper.GetMessage(Database, "WSP_PM_GrantMedal", prams);
        }

        /// <summary>
        /// 检查是否可以批量修改
        /// </summary>
        /// <param name="ids"></param>
        /// <param name="point"></param>
        /// <returns></returns>
        public bool CheckAwrdPoint(string ids, int point)
        {
            string sql = $"SELECT COUNT(*) FROM IMGroupMember WHERE MemberRight=1 AND AwardPoint > {point} AND GroupID IN ({ids})";
            object o = Database.ExecuteScalar(CommandType.Text, sql);
            return Convert.ToInt32(o) > 0 ;
        }

        /// <summary>
        /// 批量修改盟主点位
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：解冻 1：冻结</param>
        /// <returns></returns>
        public int ModifyAwardPoint(string ids, int point)
        {
            return Database.ExecuteNonQuery(CommandType.Text,
                $" UPDATE IMGroupMember SET AwardPoint = {point} WHERE GroupID IN ({ids})AND MemberRight=1");
        }

        /// <summary>
        /// 检查是否可以批量修改最大人数
        /// </summary>
        /// <param name="ids"></param>
        /// <param name="point"></param>
        /// <returns></returns>
        public bool CheckMaxMemberCount(string ids, int count)
        {
            string sql = $"SELECT COUNT(*) FROM IMGroupProperty WHERE  MemberCount > {count} AND GroupID IN ({ids})";
            object o = Database.ExecuteScalar(CommandType.Text, sql);
            return Convert.ToInt32(o) > 0;
        }

        /// <summary>
        /// 批量修改盟主点位
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="count">人数</param>
        /// <returns></returns>
        public int ModifyMaxMemberCount(string ids, int count)
        {
            string sql = $" UPDATE IMGroupProperty SET MaxMemberCount = {count} ";
            if (!string.IsNullOrEmpty(ids))
            {
                sql += $" WHERE GroupID IN ({ids})";
            }
            return Database.ExecuteNonQuery(CommandType.Text,sql);
        }

        #endregion

        #region 白名单管理
        /// <summary>
        /// 获取白名单实体 by userId
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        public IMGroupwhiteList GetIMGroupwhiteInfo(int userId)
        {
            return Database.ExecuteObject<IMGroupwhiteList>($"SELECT * FROM DBO.[IMGroupwhiteList] WHERE UserID={userId}");
        }

        /// <summary>
        /// 新增白名单
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int InsertIMGroupwhiteInfo(IMGroupwhiteList model)
        {
            var parm = new List<DbParameter>()
            {
                Database.MakeInParam("UserID", model.UserID)
            };

            const string sqlInsert =
                " INSERT DBO.[IMGroupwhiteList] (UserID,InsertTime) VALUES (@UserID,GETDATE()) ";
            return Database.ExecuteNonQuery(CommandType.Text, sqlInsert, parm.ToArray());
        }

        /// <summary>
        /// 批量删白名单
        /// </summary>
        /// <param name="ids"></param>
        /// <returns></returns>
        public int DeleteIMGroupwhiteInfo(string ids)
        {
            return Database.ExecuteNonQuery($"DELETE DBO.[IMGroupwhiteList] WHERE UserID IN ({ids})");
        }
        #endregion

        #region 总控系统

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupDebugList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("IMGroupProperty", where, order, pageIndex, pageSize, null, "WEB_PageView_GroupDebug");
            return GetPagerSet(pagerPrams);
        }

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupDebugUserList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("IMGroupMember", where, order, pageIndex, pageSize, null, "WEB_PageView_GroupDebugUser");
            return GetPagerSet(pagerPrams);
        }


        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupGameControlList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("IMGroupProperty", where, order, pageIndex, pageSize, null, "WEB_PageView_GroupGameControl");
            return GetPagerSet(pagerPrams);
        }

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupGameControlSubList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("IMGroupShareStorageProperty", where, order, pageIndex, pageSize, null, "WEB_PageView_GroupGameControlSub");
            return GetPagerSet(pagerPrams);
        }

        /// <summary>
        /// 获取总控增减账号联盟信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        public DataTable GetGroupDebugInfo(int groupId)
        {
            string sql =
                $@"SELECT GroupID,CreaterID,CreaterGameID,CreaterNickName,GroupName,GroupStatus,MemberCount,MaxMemberCount,CreateDateTime,CanUseMedal,
			(SELECT  COUNT(0) FROM dbo.IMGroupMember igm INNER JOIN WHQJAccountsDB.dbo.AccountsInfo  ai ON ai.UserID = igm.UserID WHERE igm.GroupID=IMGroupProperty.GroupID AND ai.IsAndroid=0) AS UserCount,
			(SELECT  COUNT(0) FROM dbo.IMGroupMember igm INNER JOIN WHQJAccountsDB.dbo.AccountsInfo  ai ON ai.UserID = igm.UserID WHERE igm.GroupID=IMGroupProperty.GroupID AND ai.IsAndroid=1) AS AICount,
			(SELECT  COUNT(0) FROM dbo.IMGroupMember igm INNER JOIN WHQJAccountsDB.dbo.AccountsInfo  ai ON ai.UserID = igm.UserID WHERE igm.GroupID=IMGroupProperty.GroupID AND igm.UserStatus=1 AND ai.IsAndroid=0 ) AS OnlineUserCount,
			(SELECT  COUNT(0) FROM dbo.IMGroupMember igm INNER JOIN WHQJAccountsDB.dbo.AccountsInfo  ai ON ai.UserID = igm.UserID WHERE igm.GroupID=IMGroupProperty.GroupID AND ai.IsAndroid=1 AND igm.UserStatus=1 ) AS OnlinAICount,
			(SELECT ISNULL(SUM(ChangeScore),0) FROM RecordGroupScoreChange WHERE  GroupID=IMGroupProperty.GroupID AND ChangeType=2) AS AllScore,
			(SELECT Score FROM dbo.IMGroupWealth WHERE GroupID=IMGroupProperty.GroupID) AS LaveNoDistributionScore,
			(SELECT ISNULL(SUM(DistributeTotalScore),0) FROM WHQJAccountsDB.dbo.GroupAIParameter WHERE GroupID=IMGroupProperty.GroupID) AS AIDistributionScore,
			(SELECT ISNULL(SUM(igm.Score),0) FROM dbo.IMGroupMember igm INNER JOIN WHQJAccountsDB.dbo.AccountsInfo ai ON ai.UserID = igm.UserID WHERE  igm.GroupID=IMGroupProperty.GroupID AND ai.IsAndroid=0) AS UserCarryScore,
			(SELECT ISNULL(SUM(igm.Score),0) FROM dbo.IMGroupMember igm INNER JOIN WHQJAccountsDB.dbo.AccountsInfo ai ON ai.UserID = igm.UserID WHERE  igm.GroupID=IMGroupProperty.GroupID AND ai.IsAndroid=1) AS AICarryScore
			FROM IMGroupProperty(NOLOCK) WHERE GroupID={groupId} ";

            return Database.ExecuteDataset(sql).Tables[0];
        }

        /// <summary>
        /// 获取总控游戏控制联盟信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        public DataTable GetGroupGameControlInfo(int groupId)
        {
            string sql =
                $@"SELECT  *,(SELECT ISNULL(SUM(ScoreCount),0) FROM dbo.RecordUserDayInfo WHERE GroupID=IMGroupProperty.GroupID) AS AllScore,
			(SELECT ISNULL(SUM(ScoreCount),0) FROM dbo.RecordUserDayInfo WHERE GroupID=IMGroupProperty.GroupID AND DATEDIFF(DAY,InsertTime,GETDATE())=0) AS TodayScore,
			(SELECT ISNULL(SUM(RevenueCount),0) FROM dbo.RecordUserDayInfo WHERE GroupID=IMGroupProperty.GroupID) AS AllRevenue,
			(SELECT ISNULL(SUM(RevenueCount),0) FROM dbo.RecordUserDayInfo WHERE GroupID=IMGroupProperty.GroupID AND DATEDIFF(DAY,InsertTime,GETDATE())=0) AS TodayRevenue
			FROM IMGroupProperty(NOLOCK) WHERE GroupID={groupId} ";

            return Database.ExecuteDataset(sql).Tables[0];
        }


        /// <summary>
        /// 获取当前共享库存配置
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="kindId">游戏ID</param>
        /// <param name="playmode">模式 0.积分 1.金币 </param>
        /// <returns></returns>
        public IMGroupShareStorageProperty GetShareStorage(int groupId, int kindId, int playmode)
        {
            return Database.ExecuteObject<IMGroupShareStorageProperty>(
                $"SELECT * FROM DBO.[IMGroupShareStorageProperty] WHERE GroupID={groupId} AND KindID={kindId} AND PlayMode={playmode}");
        }

        /// <summary>
        /// 获取大联盟财富
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        public IMGroupWealth GetGroupWealth(int groupId)
        {
            return Database.ExecuteObject<IMGroupWealth>(
                $"SELECT * FROM DBO.[IMGroupWealth] WHERE GroupID={groupId}");
        }

        #endregion
    }
}
