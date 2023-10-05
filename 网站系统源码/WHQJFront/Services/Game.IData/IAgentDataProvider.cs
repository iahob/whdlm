﻿using Game.Kernel;
using Game.Entity.Agent;
using System.Data;

// ReSharper disable InconsistentNaming

namespace Game.IData
{
    /// <summary>
    ///  代理库数据层接口
    /// </summary>
    public interface IAgentDataProvider //:IProvider
    {
        #region 系统配置

        /// <summary>
        /// 获取系统配置信息
        /// </summary>
        /// <param name="key">配置键值</param>
        /// <returns></returns>
        SystemStatusInfo GetSystemStatusInfo(string key);

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
        PagerSet GetList(string tableName, int pageIndex, int pageSize, string condition, string orderby);

        #endregion

        #region 代理信息

        /// <summary>
        /// 获取代理信息（通过代理标识或用户标识）
        /// </summary>
        /// <param name="agentId">代理标识</param>
        /// <param name="userId">用户标识</param>
        /// <returns></returns>
        AgentInfo GetAgentInfo(int agentId, int userId);

        /// <summary>
        /// 代理用户登录（微信）
        /// </summary>
        /// <param name="unionid">微信标识</param>
        /// <param name="ip">登录ip</param>
        /// <returns></returns>
        Message AgentWXLogin(string unionid, string ip);

        /// <summary>
        /// 代理用户登录（手机号码+安全密码）
        /// </summary>
        /// <param name="mobile"></param>
        /// <param name="pass"></param>
        /// <param name="ip"></param>
        /// <returns></returns>
        Message AgentMobileLogin(string mobile, string pass, string ip);

        /// <summary>
        /// 玩家绑定代理
        /// </summary>
        /// <param name="userId">玩家用户标识</param>
        /// <param name="gameId">预绑定上级游戏标识</param>
        /// <returns></returns>
        Message UserBindAgent(int userId, int gameId);

        /// <summary>
        /// 根据代理域名获取游戏ID
        /// </summary>
        /// <param name="domain">代理域名</param>
        /// <returns></returns>
        int GetGameIDByAgentDomain(string domain);

        /// <summary>
        /// 设置代理安全密码
        /// </summary>
        /// <param name="agentid">代理标识</param>
        /// <param name="password">安全密码</param>
        /// <returns></returns>
        int SetAgentSafePassword(int agentid, string password);

        /// <summary>
        /// 修改代理信息
        /// </summary>
        /// <param name="agent">代理信息</param>
        /// <returns></returns>
        int UpdateAgentInfo(AgentInfo agent);

        /// <summary>
        /// 代理添加下线玩家
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="pGameId">父级代理游戏ID</param>
        /// <returns></returns>
        Message UserAgentBind(int userid, int pGameId, string ip);

        /// <summary>
        /// 获取上级代理信息
        /// </summary>
        /// <param name="userId"></param>
        /// <returns></returns>
        AgentInfo GetParentAgentInfo(int userId);
        #endregion

        #region 返利相关

        /// <summary>
        /// 代理返利
        /// </summary>
        /// <param name="userId"></param>
        /// <param name="awardType"></param>
        /// <param name="returnBase"></param>
        /// <returns></returns>
        Message AgentAward(int userId, byte awardType, long returnBase);

        /// <summary>
        /// 领取奖励
        /// </summary>
        /// <param name="userId">代理用户标识</param>
        /// <param name="awardType">1：充值返利【钻石】 2：游戏税收返利【金币】</param>
        /// <param name="award">领取的数量</param>
        /// <returns></returns>
        Message ReceiveAgentAward(int userId, byte awardType, long award);

        /// <summary>
        /// 转赠奖励（单独使用时则为单向转赠，与领取返利一起使用则转赠返利）
        /// </summary>
        /// <param name="userId">代理用户标识</param>
        /// <param name="gameId">被转赠目标游戏标识</param>
        /// <param name="awardType">1：充值返利【钻石】 2：游戏税收返利【金币】</param>
        /// <param name="award">转赠数额</param>
        /// <param name="password">安全密码</param>
        /// <returns></returns>
        Message GiveAgentAward(int userId, int gameId, byte awardType, long award, string password);

        /// <summary>
        /// 获取代理累计总奖励 by awardType 不传为 充值返利，传2 为游戏税收返利 
        /// </summary>
        /// <param name="userId">必填 代理的用户标识</param>
        /// <param name="typeId">默认不传统计代理充值返利</param>
        /// <param name="sourceUserId">默认不传统计代理所有该类返利</param>
        /// <param name="timeSpan">时间期间</param>
        /// <returns></returns>
        long GetTotalAward(int userId, int typeId = 1, int sourceUserId = 0, string[] timeSpan = null);

        /// <summary>
        /// 获取代理累计提取总返利 by awardType 不传为 充值返利，传2 为游戏税收返利 
        /// </summary>
        /// <param name="userId">必填 代理的用户标识</param>
        /// <param name="typeId">默认不传，统计代理充值返利</param>
        /// <param name="timeSpan">时间期间</param>
        /// <returns></returns>
        long GetTotalReceive(int userId, int typeId = 1, string[] timeSpan = null);

        /// <summary>
        /// 获取返利配置信息列表
        /// </summary>
        /// <returns></returns>
        DataSet GetReturnAwardConfig();

        #endregion

        #region 直属下线

        /// <summary>
        /// 获取我的直属下线（玩家和代理）分页列表
        /// </summary>
        /// <param name="sqlWhere"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        PagerSet GetBelowListPagerSet(string sqlWhere, int pageIndex, int pageSize);

        /// <summary>
        /// 获取直属代理的下线玩家
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        int GetBelowAgentsUser(int agentid);

        /// <summary>
        /// 获取直属代理的下线代理
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        int GetBelowAgentsAgent(int agentid);

        /// <summary>
        /// 获取所有下级玩家
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        int GetBelowAgentsAllUser(int agentid);

        /// <summary>
        /// 获取所有下级代理
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        int GetBelowAgentsAllAgent(int agentid);

        /// <summary>
        /// 代理添加直属代理
        /// </summary>
        /// <param name="userId">代理用户标识</param>
        /// <param name="info">代理资料</param>
        /// <param name="gameId">直属代理游戏标识</param>
        /// <returns></returns>
        Message AddAgent(int userId, AgentInfo info, int gameId);

        #endregion

        #region 代理Token管理

        /// <summary>
        /// 检查代理登录凭证
        /// </summary>
        /// <param name="token"></param>
        /// <returns></returns>
        AgentTokenInfo VerifyAgentToken(string token);

        /// <summary>
        /// 保存代理登录凭证
        /// </summary>
        /// <param name="info"></param>
        /// <param name="token"></param>
        /// <returns></returns>
        int SaveAgentToken(AgentInfo info, string token);

        #endregion
    }
}
