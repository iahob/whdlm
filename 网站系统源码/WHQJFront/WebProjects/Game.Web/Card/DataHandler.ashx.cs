using Game.Entity.Accounts;
using Game.Entity.Agent;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.SessionState;
using Game.Entity.NativeWeb;
using Game.Facade.DataStruct;
using Game.Facade.Enum;
using Game.Web.Card.DataStruct;
using Game.Web.Helper;

// ReSharper disable InconsistentNaming

namespace Game.Web.Card
{
    /// <summary>
    /// DataHandler 的摘要说明
    /// </summary>
    public class DataHandler : IHttpHandler, IRequiresSessionState
    {
        #region Fields

        /// <summary>
        /// 实例是否可重复使用
        /// </summary>
        public bool IsReusable => true;

        /// <summary>
        /// 响应实体
        /// </summary>
        private static AjaxJsonValid _ajv;

        /// <summary>
        /// 通用用户标识
        /// </summary>
        private static int UserId => _agentInfo.UserID;

        private static int AgentId => _agentInfo.AgentID;

        private static string Password => _agentInfo.Password;

        private static Entity.Agent.AgentInfo _agentInfo { get; set; }

        #endregion

        #region Router

        public void ProcessRequest(HttpContext context)
        {
            context.Response.ContentType = "application/json";
            string action = Fetch.GetFilterString("action").ToLower();
            int version = GameRequest.GetInt("version", 2);

            #region Version 2.0 Router

            if (version == 2)
            {
                try
                {
                    //不需要认证的action
                    string[] unNeedAuthActions = {"agentauth", "gettoken" };
                   
                    string token = Fetch.GetFilterString("token");
                    string gameid = Fetch.GetFilterString("gameid");
                    string authheader = context.Request.Headers["Authorization"];
                    _ajv = new AjaxJsonValid();
                    _ajv.SetDataItem("apiVersion", 20180316);
                    if (!string.IsNullOrEmpty(gameid))
                    {
                        unNeedAuthActions[1] = "setpassword";
                    }

                    //排除不需要认证后判断认证是否正确
                    if (!unNeedAuthActions.Contains(action))
                    {
                        if (string.IsNullOrEmpty(token) &&
                            (string.IsNullOrEmpty(authheader) || !authheader.Contains("Bearer")))
                        {
                            _ajv.code = (int) ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " token 缺失");
                            context.Response.Write(_ajv.SerializeToJson());
                            return;
                        }
                        string authToken = !string.IsNullOrEmpty(token) ? token : authheader.Replace("Bearer ", "");
                        AgentTokenInfo authInfo = FacadeManage.aideAgentFacade.VerifyAgentToken(authToken);
                        if (authInfo == null)
                        {
                            _ajv.code = (int) ApiCode.Unauthorized;
                            _ajv.msg = EnumHelper.GetDesc(ApiCode.Unauthorized);
                            context.Response.Write(_ajv.SerializeToJson());
                            return;
                        }
                        //认证完成后 设置到全局
                        _agentInfo = FacadeManage.aideAgentFacade.GetAgentInfo(authInfo.AgentID, authInfo.UserID);
                    }

                    switch (action)
                    {
                        case "agentauth": //1.0
                            AgentAuth(Fetch.GetFilterString("gameid"), Fetch.GetFilterString("pass"));
                            break;
                        case "getinfo": //1.1
                            GetAgentInfo();
                            break;
                        case "gettoken": //1.0
                            Gettoken();
                            break;
                        case "getnicknamebygameid": //1.2
                            GetNickNameByGameID(GameRequest.GetInt("gameid", 0));
                            break;
                        case "getrecord": //1.3
                            GetRecord(Fetch.GetFilterString("type"));
                            break;
                        case "getbelowlist": //1.4
                            GetBelowList(Fetch.GetFilterString("type"), Fetch.GetFilterString("query"));
                            break;
                        case "getawardinfo": //1.5
                            GetAwardInfo();
                            break;
                        case "presentscore": //1.6
                            TakeScoreORDiamond(GameRequest.GetInt("gameid", 0), Fetch.GetFilterString("password"),
                                GameRequest.GetInt("count", 0), Convert.ToByte(GameRequest.GetInt("type", 0)));
                            break;
                        case "presentdiamondorscore":
                            PresentDiamondOrScore(GameRequest.GetInt("gameid", 0), Fetch.GetFilterString("password"),
                              GameRequest.GetInt("count", 0), Convert.ToByte(GameRequest.GetInt("type", 0)));
                            break;
                        case "setpassword": //1.7
                            SetSafePass(Fetch.GetFilterString("password"));
                            break;
                        case "updatepassword": //1.8
                            UpdateSafePass(Fetch.GetFilterString("oldPassword"), Fetch.GetFilterString("newPassword"));
                            break;
                        case "updateinfo": //1.9
                            UpdateAgentInfo(Fetch.GetFilterString("address"), Fetch.GetFilterString("phone"),
                                Fetch.GetFilterString("qq"));
                            break;
                        case "addagent": //1.10
                            AddBelowAgent(GameRequest.GetInt("gameid", 0), Fetch.GetFilterString("agentDomain"),
                                Fetch.GetFilterString("compellation"),
                                "", Fetch.GetFilterString("phone"),
                                ""
                            );
                            break;
                        default:
                            _ajv.code = (int) ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                                " action 对应接口不存在");
                            break;
                    }

                    context.Response.Write(_ajv.SerializeToJson());
                }
                catch (Exception ex)
                {
                    Log4Net.WriteInfoLog("下面一条为接口故障信息", "Agent_DataHandler");
                    Log4Net.WriteErrorLog(ex, "Agent_DataHandler");
                    _ajv = new AjaxJsonValid
                    {
                        code = (int) ApiCode.LogicErrorCode,
                        msg = EnumHelper.GetDesc(ApiCode.LogicErrorCode)
                    };
                    context.Response.Write(_ajv.SerializeToJson());
                }
            }

            #endregion
        }

        #endregion

        #region Version 2.0 Logic

        #region 认证接口
        /// <summary>
        /// 获取token
        /// </summary>
        private static void Gettoken()
        {
            string token = "";
            if (HttpRuntime.Cache.Get("token") != null)
            {
                token = HttpRuntime.Cache.Get("token").ToString();
                HttpRuntime.Cache.Remove("token");
                _ajv.SetValidDataValue(true);
                _ajv.SetDataItem("token", token);
                return;
            }
            _ajv.code = (int)ApiCode.Unauthorized;
            _ajv.msg = EnumHelper.GetDesc(ApiCode.Unauthorized);
        }
        /// <summary>
        /// 代理账号+安全密码认证 换取 Token
        /// </summary>
        /// <param name="mobile"></param>
        /// <param name="pass"></param>
        private static void AgentAuth(string mobile, string pass)
        {
            if (string.IsNullOrEmpty(mobile) || string.IsNullOrEmpty(pass))
            {
                _ajv.code = (int) ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " mobile、pass 缺失");
                return;
            }
            Message msg = FacadeManage.aideAgentFacade.AgentMobileLogin(mobile, pass, GameRequest.GetUserIP());
            if (msg.Success)
            {

                Entity.Agent.AgentInfo info = msg.EntityList[0] as Entity.Agent.AgentInfo;
                if (info != null)
                {
                    string token =
                        Fetch.SHA256Encrypt(
                            $"<{info.UserID}>,<{info.AgentID}>,<{info.AgentDomain}>,<{Fetch.ConvertDateTimeToUnix(DateTime.Now)}>");

                    FacadeManage.aideAgentFacade.SaveAgentToken(info, token);

                    _ajv.SetValidDataValue(true);
                    _ajv.SetDataItem("token", token);
                    _ajv.SetDataItem("expirtAt", DateTime.Now.AddDays(1));
                    return;
                }
            }

            _ajv.code = (int) ApiCode.Unauthorized;
            _ajv.msg = EnumHelper.GetDesc(ApiCode.Unauthorized);
        }

        #endregion

        #region 查询系列接口

        /// <summary>
        /// 获取代理信息汇总
        /// </summary>
        private static void GetAgentInfo()
        {
            Entity.Agent.SystemStatusInfo diamondSave =
                FacadeManage.aideAgentFacade.GetSystemStatusInfo(AppConfig.AgentConfig.ReceiveDiamondSave.ToString());
            Entity.Agent.SystemStatusInfo goldSave =
                FacadeManage.aideAgentFacade.GetSystemStatusInfo(AppConfig.AgentConfig.ReceiveGoldSave.ToString());

            AccountsInfo userInfo = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(UserId);
            //AccountsFace face = FacadeManage.aideAccountsFacade.GetAccountsFace(userInfo.CustomID);
            Entity.Agent.AgentInfo agentInfo =
                FacadeManage.aideAgentFacade.GetAgentInfo(AgentId, UserId);

            DataStruct.AgentInfo info = new DataStruct.AgentInfo
            {
                //来源用户表
                UserID = userInfo.UserID,
                GameID = userInfo.GameID,
                AgentID = AgentId,
                //FaceUrl = "",
                NickName = userInfo.NickName,
                //来源代理表
                AgentLevel = agentInfo.AgentLevel == 1 ? "一级代理" : (agentInfo.AgentLevel == 2 ? "二级代理" : "三级代理"),
                AgentDomain = agentInfo.AgentDomain,
                Compellation = agentInfo.Compellation,
                ContactAddress = agentInfo.ContactAddress,
                ContactPhone = agentInfo.ContactPhone,
                WCNickName = agentInfo.WCNickName,
                QQAccount = agentInfo.QQAccount,
                //来源各种统计
                BelowAgent = FacadeManage.aideAgentFacade.GetBelowAgentsAgent(AgentId),
                BelowUser = FacadeManage.aideAgentFacade.GetBelowAgentsUser(AgentId),
                DiamondAward = agentInfo.DiamondAward,
                GoldAward = agentInfo.GoldAward,
                TotalDiamondAward = FacadeManage.aideAgentFacade.GetTotalDiamondAward(UserId),
                TotalGoldAward = FacadeManage.aideAgentFacade.GetTotalGoldAward(UserId),
                BelowAgentsUser = FacadeManage.aideAgentFacade.GetBelowAgentsAllUser(AgentId),
                BelowAgentsAgent = FacadeManage.aideAgentFacade.GetBelowAgentsAllAgent(AgentId) - 1,
                IsHasPassword = !agentInfo.Password.Equals(""),
                Diamond = FacadeManage.aideTreasureFacade.GetUserWealth(UserId).Diamond
            };
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("info", info);
            _ajv.SetDataItem("DiamondSave", diamondSave?.StatusValue ?? 0);
            _ajv.SetDataItem("GoldSave", goldSave?.StatusValue ?? 0);
        }

        /// <summary>
        /// 根据GameID查询用户昵称（检查对象存在用）
        /// </summary>
        /// <param name="gameId"></param>
        private static void GetNickNameByGameID(int gameId)
        {
            if (gameId == 0)
            {
                _ajv.code = (int) ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " gameid 缺失");
                return;
            }
            AccountsInfo userInfo = FacadeManage.aideAccountsFacade.GetAccountsInfoByGameID(gameId);
            if (userInfo?.UserID > 0)
            {
                _ajv.SetDataItem("NickName", userInfo.NickName);
            }
            else
            {
                _ajv.msg = "所查询的GameID不存在";
            }
            _ajv.SetValidDataValue(true);
        }

        /// <summary>
        /// 获取记录列表 by type
        /// </summary>
        /// <param name="type"></param>
        private static void GetRecordbak(string type)
        {
            int number = GameRequest.GetInt("pagesize", 10);
            int page = GameRequest.GetInt("pageindex", 1);
            string query = Fetch.GetFilterString("query");
            string startDate = Fetch.GetFilterString("startdate");
            string endDate = Fetch.GetFilterString("enddate");
            PagerSet ps;
            string where;
            string sqlUserId="";
            if (!string.IsNullOrEmpty(query))
            {
                sqlUserId = " AND SourceUserID IN (SELECT UserID FROM WHQJAccountsDB.DBO.AccountsInfo(NOLOCK) " +
                            (Validate.IsPositiveInt(query)
                                ? $"WHERE GameID={query} OR NickName='{query}') "
                                : $"WHERE NickName = '{query}') ");
            }
            string sqlDate = "";
            if (!string.IsNullOrEmpty(startDate)) 
                sqlDate = $" AND CollectDate >= '{startDate} 00:00:00'";
            if (!string.IsNullOrEmpty(endDate))
                sqlDate += $"AND CollectDate <= '{endDate} 23:59:59' ";

            switch (type)
            {
                case "pay": //获取充值返利记录
                    where = $" WHERE TargetUserID = {UserId} AND AwardType  = 1 {sqlDate} {sqlUserId}";
                    ps = FacadeManage.aideAgentFacade.GetList(ReturnAwardRecord.Tablename, page, number, where,
                        $"ORDER BY {ReturnAwardRecord._CollectDate} DESC");
                    IList<ReturnAwardRecord> payList = new List<ReturnAwardRecord>();
                    if (ps?.RecordCount > 0)
                    {
                        foreach (DataRow dr in ps.PageSet.Tables[0].Rows)
                        {
                            int userId = Convert.ToInt32(dr["SourceUserID"].ToString());
                            AccountsInfo ai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(userId);
                            payList.Add(new ReturnAwardRecord
                            {
                                CollectDate = Convert.ToDateTime(dr["CollectDate"]),
                                ReturnBase = Convert.ToInt64(dr["ReturnBase"]),
                                SourceUserID = Convert.ToInt32(dr["SourceUserID"].ToString()),
                                NickName = ai?.NickName ?? "",
                                GameID = ai?.GameID ?? 0,
                                IsAgent = ai?.AgentID > 0,
                                Award = Convert.ToInt32(dr["Award"].ToString())
                            });
                        }
                    }
                    _ajv.SetDataItem("record", payList);
                    break;
                case "revenue": //获取税收返利记录
                    where = $" WHERE TargetUserID = {UserId} AND AwardType  = 2 {sqlDate} {sqlUserId}";
                    ps = FacadeManage.aideAgentFacade.GetList(ReturnAwardRecord.Tablename, page, number, where,
                        $"ORDER BY {ReturnAwardRecord._CollectDate} DESC");
                    IList<ReturnAwardRecord> revenueList = new List<ReturnAwardRecord>();
                    if (ps?.RecordCount > 0)
                    {
                        foreach (DataRow dr in ps.PageSet.Tables[0].Rows)
                        {
                            int userId = Convert.ToInt32(dr["SourceUserID"].ToString());
                            AccountsInfo ai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(userId);
                            revenueList.Add(new ReturnAwardRecord
                            {
                                CollectDate = Convert.ToDateTime(dr["CollectDate"]),
                                ReturnBase = Convert.ToInt64(dr["ReturnBase"]),
                                SourceUserID = Convert.ToInt32(dr["SourceUserID"].ToString()),
                                NickName = ai?.NickName ?? "",
                                GameID = ai?.GameID ?? 0,
                                IsAgent = ai?.AgentID > 0,
                                Award = Convert.ToInt32(dr["Award"].ToString())
                            });
                        }
                    }
                    _ajv.SetDataItem("record", revenueList);
                    break;
                default:
                    _ajv.code = (int) ApiCode.VertyParamErrorCode;
                    _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " type 无对应记录");
                    return;
            }

            _ajv.SetDataItem("pageCount", ps?.PageCount);
            _ajv.SetDataItem("recordCount", ps?.RecordCount);
            _ajv.SetValidDataValue(true);
        }

        /// <summary>
        /// 获取记录列表 by type
        /// </summary>
        /// <param name="type"></param>
        private static void GetRecord(string type)
        {
            int number = GameRequest.GetInt("pagesize", 10);
            int page = GameRequest.GetInt("pageindex", 1);
            string query = Fetch.GetFilterString("query");
            string startDate = Fetch.GetFilterString("startdate");
            string endDate = Fetch.GetFilterString("enddate");
            PagerSet ps;
            string where;
            string sqlUserId = "";
            if (!string.IsNullOrEmpty(query))
            {
                sqlUserId = " SourceUserID IN (SELECT UserID FROM WHQJAccountsDB.DBO.AccountsInfo(NOLOCK) " +
                            (Validate.IsPositiveInt(query)
                                ? $"WHERE GameID={query} OR NickName='{query}') "
                                : $"WHERE NickName = '{query}') ");
            }
            else
            {
                sqlUserId = " 1=1 ";
            }
            string sqlDate = "";
            if (!string.IsNullOrEmpty(startDate))
                sqlDate = $" AND CollectDate >= '{startDate} 00:00:00'";
            if (!string.IsNullOrEmpty(endDate))
                sqlDate += $"AND CollectDate <= '{endDate} 23:59:59' ";

            switch (type)
            {
                case "pay": //获取充值返利记录
                    where = $" WHERE TargetUserID = {UserId} AND AwardType  = 1 {sqlDate} AND {sqlUserId}";
                    ps = FacadeManage.aideAgentFacade.GetList(ReturnAwardRecord.Tablename, page, number, where,
                        $"ORDER BY {ReturnAwardRecord._CollectDate} DESC");
                    IList<ReturnAwardRecord> payList = new List<ReturnAwardRecord>();
                    if (ps?.RecordCount > 0)
                    {
                        foreach (DataRow dr in ps.PageSet.Tables[0].Rows)
                        {
                            int userId = Convert.ToInt32(dr["SourceUserID"].ToString());
                            AccountsInfo ai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(userId);
                            payList.Add(new ReturnAwardRecord
                            {
                                CollectDate = Convert.ToDateTime(dr["CollectDate"]),
                                ReturnBase = Convert.ToInt64(dr["ReturnBase"]),
                                SourceUserID = Convert.ToInt32(dr["SourceUserID"].ToString()),
                                NickName = ai?.NickName ?? "",
                                GameID = ai?.GameID ?? 0,
                                IsAgent = ai?.AgentID > 0,
                                Award = Convert.ToInt32(dr["Award"].ToString())
                            });
                        }
                    }
                    _ajv.SetDataItem("record", payList);
                    break;
                case "revenue": //获取税收返利记录
                    where = $" WHERE TargetUserID = {UserId} AND AwardType  = 2 {sqlDate} AND {sqlUserId}";
                    ps = FacadeManage.aideAgentFacade.GetList(ReturnAwardRecord.Tablename, page, number, where,
                        $"ORDER BY {ReturnAwardRecord._CollectDate} DESC");
                    IList<ReturnAwardRecord> revenueList = new List<ReturnAwardRecord>();
                    if (ps?.RecordCount > 0)
                    {
                        foreach (DataRow dr in ps.PageSet.Tables[0].Rows)
                        {
                            int userId = Convert.ToInt32(dr["SourceUserID"].ToString());
                            AccountsInfo ai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(userId);
                            revenueList.Add(new ReturnAwardRecord
                            {
                                CollectDate = Convert.ToDateTime(dr["CollectDate"]),
                                ReturnBase = Convert.ToInt64(dr["ReturnBase"]),
                                SourceUserID = Convert.ToInt32(dr["SourceUserID"].ToString()),
                                NickName = ai?.NickName ?? "",
                                GameID = ai?.GameID ?? 0,
                                IsAgent = ai?.AgentID > 0,
                                Award = Convert.ToInt32(dr["Award"].ToString())
                            });
                        }
                    }
                    _ajv.SetDataItem("record", revenueList);
                    break;
                case "diamond": // 赠送钻石记录
                    where = $" WHERE TradeType = 1 AND (SourceUserID = {UserId} OR TargetUserID = {UserId}) {sqlDate} AND {sqlUserId}";
                    if (!string.IsNullOrEmpty(query))
                    {
                        where = $" WHERE TradeType = 1 AND (SourceUserID = {UserId} OR TargetUserID = {UserId}) {sqlDate} AND ({sqlUserId}" +
                            $" OR TargetUserID IN ( SELECT UserID FROM WHQJAccountsDB.DBO.AccountsInfo(NOLOCK) " +
                            (Validate.IsPositiveInt(query)
                                ? $"WHERE GameID={query} OR NickName='{query}')) "
                                : $"WHERE NickName = '{query}')) ");
                    }

                    ps = FacadeManage.aideAgentFacade.GetList(ReturnAwardGrant.Tablename, page, number, where,
                        $"ORDER BY {ReturnAwardGrant._CollectDate} DESC");
                    IList<ReturnAwardGrant> dList = new List<ReturnAwardGrant>();
                    if (ps?.RecordCount > 0)
                    {
                        foreach (DataRow dr in ps.PageSet.Tables[0].Rows)
                        {
                            int userId = Convert.ToInt32(dr["SourceUserID"].ToString());
                            int receiveUserId = Convert.ToInt32(dr["TargetUserID"].ToString());
                            AccountsInfo ai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(userId);
                            AccountsInfo rai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(receiveUserId);
                            dList.Add(new ReturnAwardGrant
                            {
                                CollectDate = Convert.ToDateTime(dr["CollectDate"]),
                                TargetUserID = Convert.ToInt32(dr["TargetUserID"]),
                                SourceUserID = Convert.ToInt32(dr["SourceUserID"].ToString()),
                                //SourceBefore = Convert.ToInt64(dr["SourceBefore"]),
                                //TargetBefore = Convert.ToInt64(dr["TargetBefore"]),
                                Amount = Convert.ToInt64(dr["Amount"]),
                                NickName = ai?.NickName ?? "",
                                ReceiveNickName = rai?.NickName ?? "",
                                GameID = ai?.GameID ?? 0,
                                ReceiveGameID = rai?.GameID ?? 0
                            });
                        }
                    }
                    _ajv.SetDataItem("record", dList);
                    break;
                case "gold": // 赠送金币记录
                    where = $" WHERE TradeType = 2 AND (SourceUserID = {UserId} OR TargetUserID = {UserId}) {sqlDate} AND {sqlUserId}";
                    if (!string.IsNullOrEmpty(query))
                    {
                        where = $" WHERE TradeType = 2 AND (SourceUserID = {UserId} OR TargetUserID = {UserId}) {sqlDate} AND ({sqlUserId}" +
                            $" OR TargetUserID IN ( SELECT UserID FROM WHQJAccountsDB.DBO.AccountsInfo(NOLOCK) " +
                            (Validate.IsPositiveInt(query)
                                ? $"WHERE GameID={query} OR NickName='{query}')) "
                                : $"WHERE NickName = '{query}')) ");
                    }

                    ps = FacadeManage.aideAgentFacade.GetList(ReturnAwardGrant.Tablename, page, number, where,
                        $"ORDER BY {ReturnAwardGrant._CollectDate} DESC");
                    IList<ReturnAwardGrant> gList = new List<ReturnAwardGrant>();
                    if (ps?.RecordCount > 0)
                    {
                        foreach (DataRow dr in ps.PageSet.Tables[0].Rows)
                        {
                            int userId = Convert.ToInt32(dr["SourceUserID"].ToString());
                            int receiveUserId = Convert.ToInt32(dr["TargetUserID"].ToString());
                            AccountsInfo ai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(userId);
                            AccountsInfo rai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(receiveUserId);
                            gList.Add(new ReturnAwardGrant
                            {
                                CollectDate = Convert.ToDateTime(dr["CollectDate"]),
                                TargetUserID = Convert.ToInt32(dr["TargetUserID"]),
                                SourceUserID = Convert.ToInt32(dr["SourceUserID"].ToString()),
                                //SourceBefore = Convert.ToInt64(dr["SourceBefore"]),
                                //TargetBefore = Convert.ToInt64(dr["TargetBefore"]),
                                Amount = Convert.ToInt64(dr["Amount"]),
                                NickName = ai?.NickName ?? "",
                                ReceiveNickName = rai?.NickName ?? "",
                                GameID = ai?.GameID ?? 0,
                                ReceiveGameID = rai?.GameID ?? 0
                            });
                        }
                    }
                    _ajv.SetDataItem("record", gList);
                    break;
                default:
                    _ajv.code = (int)ApiCode.VertyParamErrorCode;
                    _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " type 无对应记录");
                    return;
            }

            _ajv.SetDataItem("pageCount", ps?.PageCount);
            _ajv.SetDataItem("recordCount", ps?.RecordCount);
            _ajv.SetValidDataValue(true);
        }

        /// <summary>
        /// 用户代理列表获取接口
        /// </summary>
        private static void GetBelowList(string type, string query)
        {
            int number = GameRequest.GetInt("pagesize", 15);
            int page = GameRequest.GetInt("pageindex", 1);
            long pCount = 0;
            BelowList list = new BelowList();
            string sqlUserId = "";
            if (!string.IsNullOrEmpty(query))
            {
                sqlUserId = " AND UserID IN (SELECT UserID FROM WHQJAccountsDB.DBO.AccountsInfo(NOLOCK) " +
                            (Validate.IsPositiveInt(query)
                                ? $"WHERE GameID={query} OR NickName='{query}') "
                                : $"WHERE NickName = '{query}') ");
            }
            string sqlWhere = $"WHERE AgentID IN (SELECT AgentID FROM AgentInfo(NOLOCK) WHERE UserID={UserId})";
            if (!string.IsNullOrEmpty(type))
            {
                sqlWhere += type == "agent"
                    ? " AND UserID IN (SELECT UserID FROM AgentInfo(NOLOCK)) "
                    : " AND UserID NOT IN (SELECT UserID FROM AgentInfo(NOLOCK)) ";
            }
            sqlWhere += sqlUserId;
            var ps = FacadeManage.aideAgentFacade.GetBelowListPagerSet(sqlWhere, page, number);
            if (ps?.PageCount > 0)
            {
                foreach (DataRow dr in ps.PageSet.Tables[0].Rows)
                {
                    AccountsInfo ai =
                        FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(
                            Convert.ToInt32(dr["UserID"].ToString()));
                    BelowDetail detail = new BelowDetail
                    {
                        UserID = ai.UserID,
                        GameID = ai.GameID,
                        NickName = ai.NickName,
                        AgentID = ai.AgentID,
                        RegisterDate = Convert.ToDateTime(dr["CollectDate"]).ToString("yyyy/MM/dd HH:mm:ss"),
                        TotalDiamond = FacadeManage.aideAgentFacade.GetTotalDiamondAward(UserId, ai.UserID),
                        TotalGold = FacadeManage.aideAgentFacade.GetTotalGoldAward(UserId, ai.UserID)
                    };
                    pCount++;
                    list.dataList.Add(detail);
                }
            }
            _ajv.SetDataItem("list", list.dataList);
            _ajv.SetDataItem("total", ps?.RecordCount ?? 0);
            _ajv.SetDataItem("count", pCount);
            _ajv.SetDataItem("pageCount", ps?.PageCount ?? 0);
            _ajv.SetValidDataValue(true);
        }

        /// <summary>
        /// 代理返利情况
        /// </summary>
        private static void GetAwardInfo()
        {
            Dictionary<string, long> dicGoldDetail = new Dictionary<string, long>
            {
                {"TotalAward", FacadeManage.aideAgentFacade.GetTotalGoldAward(UserId)},
                {"MonthAward", FacadeManage.aideAgentFacade.GetTotalGoldAward(UserId, Fetch.GetMonthTime())},
                {"LastMonthAward", FacadeManage.aideAgentFacade.GetTotalGoldAward(UserId, Fetch.GetLastMonthTime())},
                {"TotalReceive", FacadeManage.aideAgentFacade.GetTotalGoldReceive(UserId)}
            };
            Dictionary<string, long> dicDiamondDetail = new Dictionary<string, long>
            {
                {"TotalAward", FacadeManage.aideAgentFacade.GetTotalDiamondAward(UserId)},
                {"MonthAward", FacadeManage.aideAgentFacade.GetTotalDiamondAward(UserId, Fetch.GetMonthTime())},
                {"LastMonthAward", FacadeManage.aideAgentFacade.GetTotalDiamondAward(UserId, Fetch.GetLastMonthTime())},
                {"TotalReceive", FacadeManage.aideAgentFacade.GetTotalDiamondReceive(UserId)}
            };
            Dictionary<string, object> dicAwardInfo =
                new Dictionary<string, object> {{"Gold", dicGoldDetail}, {"Diamond", dicDiamondDetail}};
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("info", dicAwardInfo);
        }

        #endregion

        #region 业务操作接口

        /// <summary>
        /// 提取钻石返利或金币返利
        /// </summary>
        /// <param name="gameId"></param>
        /// <param name="pass"></param>
        /// <param name="count"></param>
        /// <param name="type"></param>
        private static void TakeScoreORDiamond(int gameId, string pass, int count, byte type)
        {
            if (count == 0)
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " count 缺失");
                return;
            }
            if (gameId > 0 && string.IsNullOrEmpty(pass))
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " pass 转赠时安全密码必填");
                return;
            }
            if (gameId > 0 && !IsPassChecked(pass))
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " pass 安全密码不正确");
                return;
            }

            //代理领取返利
            Message msg =
                FacadeManage.aideAgentFacade.ReceiveAgentAward(UserId, type, count);
            if (msg.Success && gameId > 0) //当填写了别人的GameID为转赠功能
            {
                //代理转赠返利
                msg = FacadeManage.aideAgentFacade.GiveAgentAward(UserId, gameId, type, count, pass);
                if (!msg.Success)
                {
                    _ajv.SetValidDataValue(true);
                    _ajv.msg = msg.Content + "，已成功提取到自己账号";
                    return;
                }
            }

            if (msg.Success)
            {
                _ajv.SetValidDataValue(true);
                _ajv.msg = gameId > 0 ? "提取并转赠成功" : "提取成功";
            }
            else
            {
                _ajv.code = msg.MessageID;
                _ajv.msg = msg.Content;
            }
        }

        /// <summary>
        /// 赠送身上钻石或金币接口
        /// </summary>
        /// <param name="gameId"></param>
        /// <param name="pass"></param>
        /// <param name="count"></param>
        /// <param name="type"></param>
        private static void PresentDiamondOrScore(int gameId, string pass, int count, byte type)
        {
            if (count == 0)
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " count 缺失");
                return;
            }
            if (gameId > 0 && string.IsNullOrEmpty(pass))
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " pass 转赠时安全密码必填");
                return;
            }
            if (gameId > 0 && !IsPassChecked(pass))
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " pass 安全密码不正确");
                return;
            }

            //赠送钻石
            Message msg = FacadeManage.aideAgentFacade.GiveAgentAward(UserId, gameId, type, count, pass);

            if (msg.Success)
            {
                _ajv.SetValidDataValue(true);
                _ajv.msg = "转赠成功";
            }
            else
            {
                _ajv.code = msg.MessageID;
                _ajv.msg = msg.Content;
            }
        }

        /// <summary>
        /// 首次设置安全密码接口
        /// </summary>
        /// <param name="pass"></param>
        private static void SetSafePass(string pass)
        {
            if (string.IsNullOrEmpty(pass) || pass.Length != 32)
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " password 参数不正确或未加密");
                return;
            }
            int spResult = FacadeManage.aideAgentFacade.SetAgentSafePassword(AgentId, pass);
            if (spResult > 0)
            {
                _ajv.SetValidDataValue(true);
                _ajv.msg = "设置安全密码成功";
            }
            else
            {
                _ajv.msg = "设置安全密码失败";
            }
        }

        /// <summary>
        /// 修改安全密码接口
        /// </summary>
        /// <param name="oldPass"></param>
        /// <param name="newPass"></param>
        private static void UpdateSafePass(string oldPass, string newPass)
        {
            if (!IsPassChecked(oldPass))
            {
                _ajv.code = (int) ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " oldPassword 不正确");
                return;
            }
            if (string.IsNullOrEmpty(newPass) || newPass.Length != 32)
            {
                _ajv.code = (int) ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " newPassword 参数不正确或未加密");
                return;
            }
            int upResult = FacadeManage.aideAgentFacade.SetAgentSafePassword(AgentId, newPass);
            if (upResult > 0)
            {
                _ajv.SetValidDataValue(true);
                _ajv.msg = "修改安全密码成功";
            }
            else
            {
                _ajv.msg = "修改安全密码失败";
            }
        }

        /// <summary>
        /// 修改代理信息接口
        /// </summary>
        /// <param name="address"></param>
        /// <param name="phone"></param>
        /// <param name="qq"></param>
        private static void UpdateAgentInfo(string address, string phone, string qq)
        {
            if (string.IsNullOrEmpty(address) || string.IsNullOrEmpty(phone) ||
                string.IsNullOrEmpty(qq))
            {
                _ajv.code = (int) ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " address、phone、qq 缺失");
                return;
            }
            if (!Validate.IsMobileCode(phone))
            {
                _ajv.msg = "抱歉，联系电话格式不正确";
                return;
            }
            Entity.Agent.AgentInfo uiAgent = new Entity.Agent.AgentInfo()
            {
                UserID = UserId,
                ContactAddress = address,
                ContactPhone = phone,
                QQAccount = qq
            };
            int uiResult = FacadeManage.aideAgentFacade.UpdateAgentInfo(uiAgent);
            if (uiResult > 0)
            {
                _ajv.SetValidDataValue(true);
                _ajv.msg = "修改代理信息成功";
            }
            else
            {
                _ajv.msg = "修改代理信息失败";
            }
        }

        /// <summary>
        /// 添加下级代理接口
        /// </summary>
        /// <param name="gameId"></param>
        /// <param name="agentDomain"></param>
        /// <param name="compllation"></param>
        /// <param name="note"></param>
        /// <param name="address"></param>
        /// <param name="phone"></param>
        /// <param name="qq"></param>
        /// <param name="wcNickName"></param>
        private static void AddBelowAgent(int gameId, string agentDomain, string compllation,
            string address, string phone, string note)
        {
            if (gameId <= 0)
            {
                _ajv.msg = "抱歉，添加代理游戏ID不能为空";
                return;
            }
            if (string.IsNullOrEmpty(compllation))
            {
                _ajv.msg = "抱歉，真实姓名不能为空";
                return;
            }
            if (string.IsNullOrEmpty(phone))
            {
                _ajv.msg = "抱歉，联系电话不能为空";
                return;
            }
            if (!Validate.IsMobileCode(phone))
            {
                _ajv.msg = "抱歉，联系电话格式不正确";
                return;
            }
            if (string.IsNullOrEmpty(agentDomain))
            {
                _ajv.msg = "抱歉，代理域名不能为空";
                return;
            }

            AccountsInfo account = FacadeManage.aideAccountsFacade.GetAccountsInfoByGameID(gameId);
            if (account == null)
            {
                _ajv.msg = "抱歉，添加代理异常，请稍后重试";
                return;
            }

            Entity.Agent.AgentInfo info = new Entity.Agent.AgentInfo
            {
                AgentDomain = agentDomain,
                AgentNote = note,
                Compellation = compllation,
                ContactAddress = address,
                ContactPhone = phone,
                WCNickName = account.NickName
            };

            Message msg = FacadeManage.aideAgentFacade.AddAgent(UserId, info, gameId);
            if (msg.Success)
            {
                _ajv.SetValidDataValue(true);
                _ajv.msg = "添加下级代理成功";
            }
            else
            {
                _ajv.msg = msg.Content;
            }
        }




        #endregion

        #region 辅助方法

        /// <summary>
        /// 验证安全密码是否正确
        /// </summary>
        /// <param name="pass"></param>
        /// <returns></returns>
        private static bool IsPassChecked(string pass)
        {
            return pass.ToLower() == Password.ToLower();
        }

        #endregion

        #endregion
    }
}
