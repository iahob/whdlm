using System;
using System.Collections.Generic;
using System.Data;
using Game.Kernel;
using Game.IData;
using System.Data.Common;
using Game.Entity.Agent;
using Game.Utils;

namespace Game.Data
{
    /// <summary>
    /// 代理数据访问层
    /// </summary>
    public sealed class AgentDataProvider : BaseDataProvider, IAgentDataProvider
    {
        #region Fields

        private readonly ITableProvider _tbAgentInfoProvider;
        private readonly ITableProvider _tbSystemStatusInfoProvider;

        #endregion

        #region 构造方法

        public AgentDataProvider(string connString)
            : base(connString)
        {
            _tbSystemStatusInfoProvider = GetTableProvider(SystemStatusInfo.Tablename);
            _tbAgentInfoProvider = GetTableProvider(AgentInfo.Tablename);
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
        public SystemStatusInfo GetSystemStatusInfo(string key)
        {
            return _tbSystemStatusInfoProvider.GetObject<SystemStatusInfo>($"WHERE StatusName = N'{key}'");
        }

        #endregion

        #region 代理信息

        /// <summary>
        /// 获取代理信息（通过代理标识或用户标识）
        /// </summary>
        /// <param name="agentId">代理标识</param>
        /// <param name="userId">用户标识</param>
        /// <returns></returns>
        public AgentInfo GetAgentInfo(int agentId, int userId)
        {
            return _tbAgentInfoProvider.GetObject<AgentInfo>($"WHERE AgentID = {agentId} OR UserID = {userId}");
        }

        /// <summary>
        /// 代理用户登录（微信）
        /// </summary>
        /// <param name="unionid">微信标识</param>
        /// <param name="ip">登录ip</param>
        /// <returns></returns>
        public Message AgentWXLogin(string unionid, string ip)
        {
            List<DbParameter> prams =
                new List<DbParameter>
                {
                    Database.MakeInParam("strUserUin", unionid),
                    Database.MakeInParam("strClientIP", ip),
                    Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
                };

            return MessageHelper.GetMessageForObject<AgentInfo>(Database, "NET_AT_AgentLogin_WX", prams);
        }

        /// <summary>
        /// 代理用户登录（手机号码+安全密码）
        /// </summary>
        /// <param name="mobile"></param>
        /// <param name="pass"></param>
        /// <param name="ip"></param>
        /// <returns></returns>
        public Message AgentMobileLogin(string mobile, string pass, string ip)
        {
            List<DbParameter> prams =
                new List<DbParameter>
                {
                    Database.MakeInParam("strGameid", mobile),
                    Database.MakeInParam("strPassword", pass),
                    Database.MakeInParam("strClientIP", ip),
                    Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
                };

            return MessageHelper.GetMessageForObject<AgentInfo>(Database, "NET_AT_AgentLogin_MP", prams);
        }

        /// <summary>
        /// 玩家绑定代理
        /// </summary>
        /// <param name="userId">玩家用户标识</param>
        /// <param name="gameId">预绑定上级游戏标识</param>
        /// <returns></returns>
        public Message UserBindAgent(int userId, int gameId)
        {
            List<DbParameter> parms = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", userId),
                Database.MakeInParam("dwGameID", gameId),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };
            return MessageHelper.GetMessage(Database, "NET_PB_UserAgentBind", parms);
        }

        /// <summary>
        /// 根据代理域名获取游戏ID
        /// </summary>
        /// <param name="domain">代理域名</param>
        /// <returns></returns>
        public int GetGameIDByAgentDomain(string domain)
        {
            string sqlQuery =
                @"SELECT GameID FROM WHQJAccountsDB.dbo.AccountsInfo WITH(NOLOCK) WHERE UserID IN(SELECT UserID FROM AgentInfo WITH(NOLOCK) WHERE AgentDomain=@AgentDomain)";
            List<DbParameter> parms = new List<DbParameter> { Database.MakeInParam("AgentDomain", domain) };
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery, parms.ToArray());
            return obj != null ? Convert.ToInt32(obj) : 0;
        }
        /// <summary>
        /// 设置代理安全密码
        /// </summary>
        /// <param name="agentid">代理标识</param>
        /// <param name="password">安全密码</param>
        /// <returns></returns>
        public int SetAgentSafePassword(int agentid, string password)
        {
            string sql = "UPDATE AgentInfo SET [Password]=@Password WHERE AgentID=@AgentID";
            List<DbParameter> prams =
                new List<DbParameter>
                {
                    Database.MakeInParam("Password", password),
                    Database.MakeInParam("AgentID", agentid)
                };
            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 修改代理信息
        /// </summary>
        /// <param name="agent">代理信息</param>
        /// <returns></returns>
        public int UpdateAgentInfo(AgentInfo agent)
        {
            string sql =
                "UPDATE AgentInfo SET QQAccount=@QQAccount,ContactPhone=@ContactPhone,ContactAddress=@ContactAddress WHERE UserID=@UserID";

            DbParameter[] param =
            {
                Database.MakeInParam("QQAccount", agent.QQAccount),
                Database.MakeInParam("ContactPhone", agent.ContactPhone),
                Database.MakeInParam("ContactAddress", agent.ContactAddress),
                Database.MakeInParam("UserID", agent.UserID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, param);
        }

        /// <summary>
        /// 获取上级代理信息
        /// </summary>
        /// <param name="userId"></param>
        /// <returns></returns>
        public AgentInfo GetParentAgentInfo(int userId)
        {
            return _tbAgentInfoProvider.GetObject<AgentInfo>($"WHERE AgentID = (SELECT AgentID FROM dbo.AgentBelowInfo WHERE UserID={userId})");
        }

        #endregion

        #region 返利相关

        /// <summary>
        /// 代理返利
        /// </summary>
        /// <param name="userId"></param>
        /// <param name="awardType"></param>
        /// <param name="returnBase"></param>
        /// <returns></returns>
        public Message AgentAward(int userId, byte awardType, long returnBase)
        {
            List<DbParameter> parms = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", userId),
                Database.MakeInParam("dwReturnBase", returnBase),
                Database.MakeInParam("dwAwardType", awardType),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };
            return MessageHelper.GetMessage(Database, "NET_PB_RecordAgentAward", parms);
        }

        public Message ReceiveAgentAward(int userId, byte awardType, long award)
        {
            List<DbParameter> parms = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", userId),
                Database.MakeInParam("dwAward", award),
                Database.MakeInParam("dwAwardType", awardType),
                Database.MakeInParam("strClientIP", GameRequest.GetUserIP()),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };
            return MessageHelper.GetMessage(Database, "NET_AT_ReceiveAgentAward", parms);
        }

        /// <summary>
        /// 转赠奖励（单独使用时则为单向转赠，与领取返利一起使用则转赠返利）
        /// </summary>
        /// <param name="userId">代理用户标识</param>
        /// <param name="gameId">被转赠目标游戏标识</param>
        /// <param name="awardType">1：充值返利【钻石】 2：游戏税收返利【金币】</param>
        /// <param name="award">转赠数额</param>
        /// <param name="password">安全密码</param>
        /// <returns></returns>
        public Message GiveAgentAward(int userId, int gameId, byte awardType, long award, string password)
        {
            List<DbParameter> parms = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", userId),
                Database.MakeInParam("dwAward", award),
                Database.MakeInParam("dwAwardType", awardType),
                Database.MakeInParam("strClientIP", GameRequest.GetUserIP()),
                Database.MakeInParam("strPassword", password),
                Database.MakeInParam("dwGameID", gameId),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };
            return MessageHelper.GetMessage(Database, "NET_AT_GiveAgentAward", parms);
        }

        /// <summary>
        /// 获取代理累计总奖励 by awardType 不传为 充值返利，传2 为游戏税收返利 
        /// </summary>
        /// <param name="userId">必填 代理的用户标识</param>
        /// <param name="typeId">默认不传统计代理充值返利</param>
        /// <param name="sourceUserId">默认不传统计代理所有该类返利</param>
        /// <param name="timeSpan">时间期间</param>
        /// <returns></returns>
        public long GetTotalAward(int userId, int typeId = 1, int sourceUserId = 0, string[] timeSpan = null)
        {
            string sql =
                $" SELECT ISNULL(SUM({ReturnAwardRecord._Award}),0) FROM {ReturnAwardRecord.Tablename}(NOLOCK) WHERE {ReturnAwardRecord._TargetUserID} = {userId} AND {ReturnAwardRecord._AwardType}= {typeId} ";
            if (sourceUserId > 0)
            {
                sql += $"AND {ReturnAwardRecord._SourceUserID} = {sourceUserId} ";
            }
            if (timeSpan != null)
            {
                sql += $" AND {ReturnAwardRecord._CollectDate} BETWEEN '{timeSpan[0]}' AND '{timeSpan[1]}' ";
            }
            object result = Database.ExecuteScalar(CommandType.Text, sql);
            return Convert.ToInt64(result);
        }

        /// <summary>
        /// 获取代理累计提取总返利 by awardType 不传为 充值返利，传2 为游戏税收返利 
        /// </summary>
        /// <param name="userId">必填 代理的用户标识</param>
        /// <param name="typeId">默认不传，统计代理充值返利</param>
        /// <param name="timeSpan">时间期间</param>
        /// <returns></returns>
        public long GetTotalReceive(int userId, int typeId = 1, string[] timeSpan = null)
        {
            string sql =
                $" SELECT ISNULL(SUM({ReturnAwardReceive._ReceiveAward}),0) FROM {ReturnAwardReceive.Tablename}(NOLOCK) WHERE {ReturnAwardReceive._UserID}={userId} AND {ReturnAwardReceive._AwardType}= {typeId} ";
            if (timeSpan != null)
            {
                sql += $" AND {ReturnAwardReceive._CollectDate} BETWEEN '{timeSpan[0]}' AND '{timeSpan[1]}' ";
            }
            object result = Database.ExecuteScalar(CommandType.Text, sql);
            return Convert.ToInt64(result);
        }

        /// <summary>
        /// 代理添加下线玩家
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="pGameId">父级代理游戏ID</param>
        /// <param name="ip">IP</param>
        /// <returns></returns>
        public Message UserAgentBind(int userid, int pGameId, string ip)
        {
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", userid),
                Database.MakeInParam("dwGameID", pGameId),
                Database.MakeInParam("strClientIP", ip),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };

            return MessageHelper.GetMessage(Database, "NET_PB_UserAgentBind", prams);
        }

        /// <summary>
        /// 获取返利配置信息列表
        /// </summary>
        /// <returns></returns>
        public DataSet GetReturnAwardConfig()
        {
            string sql = "SELECT * FROM dbo.ReturnAwardConfig WHERE Nullity=0";
            return Database.ExecuteDataset(sql);
        }

        #endregion

        #region 直属下线

        /// <summary>
        /// 获取我的直属下线（玩家和代理）分页列表
        /// </summary>
        /// <param name="sqlWhere"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        public PagerSet GetBelowListPagerSet(string sqlWhere, int pageIndex, int pageSize)
        {
            string[] returnField = {"AgentID", "UserID", "CollectDate"};
            PagerParameters prams = new PagerParameters(AgentBelowInfo.Tablename,
                $"ORDER BY {AgentBelowInfo._CollectDate} DESC",
                sqlWhere, pageIndex, pageSize)
            {
                Fields = returnField,
                CacherSize = 2
            };
            return GetPagerSet2(prams);
        }

        /// <summary>
        /// 获取直属代理的下线玩家
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        public int GetBelowAgentsUser(int agentid)
        {
            return (int)Database.ExecuteScalar(CommandType.Text,
                $" SELECT COUNT(UserID) FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE AgentID=0 AND UserID IN(SELECT UserID FROM AgentBelowInfo(NOLOCK) WHERE AgentID={agentid})");
        }

        /// <summary>
        /// 获取直属代理的下线代理
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        public int GetBelowAgentsAgent(int agentid)
        {
            return (int)Database.ExecuteScalar(CommandType.Text,
                $" SELECT COUNT(AgentID) FROM AgentInfo(NOLOCK) WHERE ParentAgent={agentid}");
        }

        /// <summary>
        /// 获取所有下级玩家
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        public int GetBelowAgentsAllUser(int agentid)
        {
            return (int)Database.ExecuteScalar(CommandType.Text,
                $" SELECT COUNT(UserID) FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE AgentID=0 AND UserID IN(SELECT UserID FROM AgentBelowInfo(NOLOCK) WHERE AgentID IN(SELECT AgentID FROM WF_GetAgentBelowAgent({agentid})))");
        }

        /// <summary>
        /// 获取所有下级代理
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        public int GetBelowAgentsAllAgent(int agentid)
        {
            return (int)Database.ExecuteScalar(CommandType.Text,
                $" SELECT COUNT(UserID) FROM dbo.WF_GetAgentBelowAgent({agentid})");
        }

        /// <summary>
        /// 代理添加直属代理
        /// </summary>
        /// <param name="userId">代理用户标识</param>
        /// <param name="info">代理资料</param>
        /// <param name="gameId">直属代理游戏标识</param>
        /// <returns></returns>
        public Message AddAgent(int userId, AgentInfo info, int gameId)
        {
            List<DbParameter> parms = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", userId),
                Database.MakeInParam("dwGameID", gameId),
                Database.MakeInParam("strCompellation", info.Compellation),
                Database.MakeInParam("strAgentDomain", info.AgentDomain),
                Database.MakeInParam("strQQAccount", info.QQAccount),
                Database.MakeInParam("strWCNickName", info.WCNickName),
                Database.MakeInParam("strContactPhone", info.ContactPhone),
                Database.MakeInParam("strContactAddress", info.ContactAddress),
                Database.MakeInParam("strAgentNote", info.AgentNote),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };
            return MessageHelper.GetMessage(Database, "NET_AT_AddAgent", parms);
        }

        #endregion

        #region 代理Token管理

        /// <summary>
        /// 检查代理登录凭证
        /// </summary>
        /// <param name="token"></param>
        /// <returns></returns>
        public AgentTokenInfo VerifyAgentToken(string token)
        {
            Database.ExecuteNonQuery(CommandType.Text, "DELETE AgentTokenInfo WHERE ExpirtAt < GETDATE() ");
            List<DbParameter> parms = new List<DbParameter>
            {
                 Database.MakeInParam("token", token),
            };
            string sql = "SELECT * FROM AgentTokenInfo(NOLOCK) WHERE Token = @token AND ExpirtAt > GETDATE() ";
            return Database.ExecuteObject<AgentTokenInfo>(sql, parms);
        }

        /// <summary>
        /// 保存代理登录凭证
        /// </summary>
        /// <param name="info"></param>
        /// <param name="token"></param>
        /// <returns></returns>
        public int SaveAgentToken(AgentInfo info, string token)
        {
            Database.ExecuteNonQuery(CommandType.Text, "DELETE AgentTokenInfo WHERE ExpirtAt < GETDATE() ");
            string sql =
                $" IF EXISTS (SELECT 1 FROM AgentTokenInfo WHERE UserID = {info.UserID} AND AgentID = {info.AgentID})  " +
                $" BEGIN  UPDATE AgentTokenInfo SET Token = N'{token}',ExpirtAt = GETDATE() + 1 WHERE UserID = {info.UserID} AND AgentID = {info.AgentID}  END" +
                $" ELSE BEGIN INSERT AgentTokenInfo (UserID,AgentID,Token) VALUES ({info.UserID},{info.AgentID},N'{token}') END ";
            return Database.ExecuteNonQuery(CommandType.Text, sql);
        }
        #endregion
    }
}
