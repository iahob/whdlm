﻿using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Linq;
using System.Xml.XPath;
using Game.IData;
using Game.Kernel;
using Game.Entity.Agent;

namespace Game.Data
{
    /// <summary>
    /// 代理库数据层
    /// </summary>
    public class AgentDataProvider : BaseDataProvider, IAgentDataProvider
    {
        #region 构造方法

        /// <summary>
        /// 构造函数
        /// </summary>
        public AgentDataProvider(string connString)
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
        #endregion 公用分页

        #region 系统配置
        /// <summary>
        /// 获取代理配置列表
        /// </summary>
        /// <returns></returns>
        public DataSet GetSystemStatusList()
        {
            string sqlQuery = @"SELECT * FROM SystemStatusInfo WITH(NOLOCK) ORDER BY SortID ASC";
            return Database.ExecuteDataset(sqlQuery);
        }
        /// <summary>
        /// 获取配置信息
        /// </summary>
        /// <param name="statusName">配置键值</param>
        /// <returns></returns>
        public SystemStatusInfo GetSystemStatusInfo(string statusName)
        {
            string sqlQuery = @"SELECT * FROM SystemStatusInfo WITH(NOLOCK) WHERE StatusName=@StatusName";
            var prams = new List<DbParameter> {Database.MakeInParam("StatusName", statusName)};
            return Database.ExecuteObject<SystemStatusInfo>(sqlQuery, prams);
        }

        /// <summary>
        /// 修改配置信息
        /// </summary>
        /// <param name="statusinfo">配置信息</param>
        /// <returns></returns>
        public int UpdateSystemStatusInfo(SystemStatusInfo statusinfo)
        {
            string sqlQuery = @"UPDATE SystemStatusInfo SET StatusValue=@StatusValue,StatusString=@StatusString,
                    StatusTip=@StatusTip,StatusDescription=@StatusDescription WHERE StatusName=@StatusName";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("StatusValue", statusinfo.StatusValue),
                Database.MakeInParam("StatusString", statusinfo.StatusString),
                Database.MakeInParam("StatusTip", statusinfo.StatusTip),
                Database.MakeInParam("StatusDescription", statusinfo.StatusDescription),
                Database.MakeInParam("StatusName", statusinfo.StatusName)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        #endregion

        #region 返利配置
        /// <summary>
        /// 获取返利配置列表
        /// </summary>
        /// <returns></returns>
        public DataSet GetAgentReturnConfigList()
        {
            string sqlQuery = @"SELECT * FROM ReturnAwardConfig WITH(NOLOCK) ORDER BY AwardType ASC";
            return Database.ExecuteDataset(sqlQuery);
        }
        /// <summary>
        /// 获取代理返利配置
        /// </summary>
        /// <param name="configId"></param>
        /// <returns></returns>
        public ReturnAwardConfig GetAgentReturnConfig(int configId)
        {
            return Database.ExecuteObject<ReturnAwardConfig>($"SELECT * FROM {ReturnAwardConfig.Tablename} WHERE {ReturnAwardConfig._ConfigID}={configId}");
        }

        /// <summary>
        /// 删除代理返利配置
        /// </summary>
        /// <param name="ids"></param>
        /// <returns></returns>
        public int DeleteAgentReturnConfig(string ids)
        {
            return Database.ExecuteNonQuery(CommandType.Text,
                $"DELETE {ReturnAwardConfig.Tablename} WHERE {ReturnAwardConfig._ConfigID} IN ({ids})");
        }

        /// <summary>
        /// 保存代理返利配置（新增，修改）
        /// </summary>
        /// <param name="cfg"></param>
        /// <returns></returns>
        public int SaveAgentReturnConfig(ReturnAwardConfig cfg)
        {
            var prams = new List<DbParameter>
            {
                Database.MakeInParam(ReturnAwardConfig._AwardLevel, cfg.AwardLevel),
                Database.MakeInParam(ReturnAwardConfig._AwardScale, cfg.AwardScale),
                Database.MakeInParam(ReturnAwardConfig._AwardType, cfg.AwardType),
                Database.MakeInParam(ReturnAwardConfig._Nullity, cfg.Nullity),
                Database.MakeInParam(ReturnAwardConfig._UpdateTime, cfg.UpdateTime)
            };

            string sql;
            if (cfg.ConfigID > 0)
            {
                prams.Add(Database.MakeInParam(ReturnAwardConfig._ConfigID, cfg.ConfigID));
                sql = $" UPDATE {ReturnAwardConfig.Tablename} " +
                      $"    SET {ReturnAwardConfig._AwardLevel} = @{ReturnAwardConfig._AwardLevel}," +
                      $"           {ReturnAwardConfig._AwardScale} = @{ReturnAwardConfig._AwardScale}," +
                      $"           {ReturnAwardConfig._AwardType} = @{ReturnAwardConfig._AwardType}," +
                      $"           {ReturnAwardConfig._Nullity} = @{ReturnAwardConfig._Nullity}," +
                      $"           {ReturnAwardConfig._UpdateTime} = @{ReturnAwardConfig._UpdateTime} " +
                      $"WHERE {ReturnAwardConfig._ConfigID} = @{ReturnAwardConfig._ConfigID}";
            }
            else
            {
                sql = $" IF NOT EXISTS (SELECT 1 FROM {ReturnAwardConfig.Tablename}(NOLOCK) WHERE {ReturnAwardConfig._AwardLevel}={cfg.AwardLevel} AND {ReturnAwardConfig._AwardType}={cfg.AwardType})" +
                      "BEGIN "+
                    $" INSERT {ReturnAwardConfig.Tablename} ({ReturnAwardConfig._AwardLevel},{ReturnAwardConfig._AwardScale},{ReturnAwardConfig._AwardType} ,{ReturnAwardConfig._Nullity},{ReturnAwardConfig._UpdateTime})" +
                    $"    VALUES (@{ReturnAwardConfig._AwardLevel},@{ReturnAwardConfig._AwardScale},@{ReturnAwardConfig._AwardType},@{ReturnAwardConfig._Nullity},@{ReturnAwardConfig._UpdateTime}) " +
                      "END ";
            }

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        #endregion

        #region 代理系统
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageAgentList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("AgentInfo", where, order, pageIndex, pageSize, null, "WEB_PageView_Agent");
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
        public PagerSet GetPageAgentReturnList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("ReturnAwardRecord", where, order, pageIndex, pageSize, null, "WEB_PageView_AgentReturn");
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
        public PagerSet GetPageAgentGrantList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("ReturnAwardGrant", where, order, pageIndex, pageSize, null, "WEB_PageView_AgentGrant");
            return GetPagerSet(pagerPrams);
        }
        /// <summary>
        /// 获取代理信息
        /// </summary>
        /// <param name="agentId">代理标识</param>
        /// <returns></returns>
        public AgentInfo GetAgentInfo(int agentId)
        {
            string sql = $"SELECT * FROM AgentInfo WITH(NOLOCK) WHERE AgentID = {agentId}";
            return Database.ExecuteObject<AgentInfo>(sql);
        }

        /// <summary>
        /// 添加代理信息
        /// </summary>
        /// <param name="agent">代理信息</param>
        /// <param name="pGameId">父级代理游戏ID</param>
        /// <returns></returns>
        public Message InsertAgentUser(AgentInfo agent, int pGameId)
        {
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", agent.UserID),
                Database.MakeInParam("strCompellation", agent.Compellation),
                Database.MakeInParam("strAgentDomain", agent.AgentDomain),
                Database.MakeInParam("strQQAccount", agent.QQAccount),
                Database.MakeInParam("strWCNickName", agent.WCNickName),
                Database.MakeInParam("strContactPhone", agent.ContactPhone),
                Database.MakeInParam("strContactAddress", agent.ContactAddress),
                Database.MakeInParam("dwAgentLevel", agent.AgentLevel),
                Database.MakeInParam("strAgentNote", agent.AgentNote),
                Database.MakeInParam("dwParentGameID", pGameId),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };


            return MessageHelper.GetMessage(Database, "NET_PM_AddAgent", prams);
        }

        /// <summary>
        /// 更新代理基本信息
        /// </summary>
        /// <param name="agent">代理信息</param>
        /// <returns></returns>
        public int UpdateAgentUser(AgentInfo agent)
        {
            string sqlQuery =
                @"UPDATE AgentInfo SET Password=@Password,Compellation=@Compellation,QQAccount=@QQAccount,WCNickName=@WCNickName,
            ContactPhone=@ContactPhone,ContactAddress=@ContactAddress,AgentDomain=@AgentDomain,AgentNote=@AgentNote WHERE AgentID=@AgentID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("Compellation", agent.Compellation),
                Database.MakeInParam("QQAccount", agent.QQAccount),
                Database.MakeInParam("WCNickName", agent.WCNickName),
                Database.MakeInParam("ContactPhone", agent.ContactPhone),
                Database.MakeInParam("ContactAddress", agent.ContactAddress),
                Database.MakeInParam("AgentDomain", agent.AgentDomain),
                Database.MakeInParam("AgentNote", agent.AgentNote),
                Database.MakeInParam("AgentID", agent.AgentID),
                Database.MakeInParam("Password", agent.Password)
            };


            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 冻结解冻代理
        /// </summary>
        /// <param name="strList">代理标识列表</param>
        /// <param name="nullity">代理状态</param>
        /// <returns></returns>
        public int NullityAgentUser(string strList, int nullity)
        {
            string sql = $"UPDATE AgentInfo SET Nullity={nullity} WHERE AgentID IN({strList})";
            return Database.ExecuteNonQuery(sql);
        }

        #endregion

        #region 直属下线

        /// <summary>
        /// 获取代理下线信息
        /// </summary>
        /// <param name="userId"></param>
        /// <returns></returns>
        public AgentBelowInfo GetAgentBelowInfo(int userId)
        {
            string sql = $"SELECT * FROM {AgentBelowInfo.Tablename}(NOLOCK) WHERE {AgentBelowInfo._UserID}={userId}";
            return Database.ExecuteObject<AgentBelowInfo>(sql);
        }

        /// <summary>
        /// 获取直属下线的注册情况
        /// </summary>
        /// <param name="agentId"></param>
        /// <returns></returns>
        public DataSet GetAgentBelowRegisterList(int agentId)
        {
            string sql =
                "DECLARE @UserID INT " +
                $" SELECT @UserID = UserID FROM AgentInfo(NOLOCK) WHERE AgentID = {agentId} " +
                " SELECT * FROM WF_GetAgentBelowUserRegister (@UserID) ";
            return Database.ExecuteDataset(sql);
        }

        #endregion
    }
}
