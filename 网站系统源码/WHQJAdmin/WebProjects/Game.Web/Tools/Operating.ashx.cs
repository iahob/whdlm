using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.SessionState;
using Game.Utils;
using Game.Entity.PlatformManager;
using Game.Facade;
using Game.Entity.Platform;
using Game.Kernel;
using System.Data;
using System.IO;
using System.Net.Configuration;
using Game.Entity.Enum;
using Game.Entity.Accounts;
using Game.Entity.Treasure;
using Game.Entity.Record;
using System.Web.Script.Serialization;
using LitJson;

namespace Game.Web.Tools
{
    /// <summary>
    /// 后台涉及的一些异步操作
    /// </summary>
    public class Operating : IHttpHandler, IRequiresSessionState
    {
        public AjaxJsonValid ajv = new AjaxJsonValid();

        public void ProcessRequest(HttpContext context)
        {
            context.Response.ContentType = "application/json";

            //执行操作
            string action = GameRequest.GetQueryString("action").ToLower();
            switch (action)
            {
                case "getuserinfo":
                    GetUserInfo(context);
                    break;
                case "getplatformdiamond":
                    GetPlatformDiamond(context);
                    break;
                case "getplatformgold":
                    GetPlatformGold(context);
                    break;
                case "getplatformpay":
                    GetPlatformPay(context);
                    break;
                case "getregisterstatictics":
                    GetRegisterStatictics(context);
                    break;
                case "getuseropenroom":
                    GetUserOpenRoom(context);
                    break;
                case "getrevenuestatictics":
                    GetRevenueStatictics(context);
                    break;
                case "getwastestatictics":
                    GetWasteStatictics(context);
                    break;
                case "getuseronlinestatictics":
                    GetUserOnlineStatictics(context);
                    break;
                case "httpservice":
                    HttpService(context);
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// 通过游戏ID获取游戏信息
        /// </summary>
        /// <param name="context"></param>
        private void GetUserInfo(HttpContext context)
        {
            int gameID = GameRequest.GetFormInt("GameID", 0);
            if (gameID == 0)
            {
                ajv.msg = "游戏ID输入非法";
                context.Response.Write(ajv.SerializeToJson());
                return;
            }

            AccountsInfo info = FacadeManage.aideAccountsFacade.GetAccountInfoByGameId(gameID);
            if (info?.UserID > 0)
            {
                ajv.SetValidDataValue(true);
                ajv.AddDataItem("UserID", info.UserID);
                ajv.AddDataItem("Accounts", info.Accounts);
                ajv.AddDataItem("NickName", info.NickName);
                ajv.AddDataItem("Compellation", info.Compellation);
                ajv.AddDataItem("AgentID", info.AgentID);
            }
            else
            {
                ajv.msg = "未找到该用户信息";
            }

            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取平台钻石信息
        /// </summary>
        /// <param name="context"></param>
        private void GetPlatformDiamond(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            DateTime sDate = string.IsNullOrEmpty(stime) ? DateTime.Now.AddDays(-1) : Convert.ToDateTime(stime);
            DateTime eDate = string.IsNullOrEmpty(etime) ? DateTime.Now.AddDays(-7) : Convert.ToDateTime(etime);
            if (sDate >= eDate)
            {
                sDate = eDate.AddDays(-7);
            }

            DataSet ds =
                FacadeManage.aideNativeWebFacade.GetDataStatistics(Fetch.GetDateID(sDate), Fetch.GetDateID(eDate));
            List<StatisticsChart> data = new List<StatisticsChart>();
            if (ds != null && ds.Tables[0].Rows.Count > 0)
            {
                StatisticsChart sc = null;
                foreach (DataRow item in ds.Tables[0].Rows)
                {
                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt64(item["Diamond"]);
                    sc.type = "平台钻石";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt64(item["PayDiamond"]);
                    sc.type = "充值钻石";
                    data.Add(sc);
                }
            }

            ajv.SetValidDataValue(true);
            ajv.AddDataItem("data", data);
            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取平台金币信息
        /// </summary>
        /// <param name="context"></param>
        private void GetPlatformGold(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            DateTime sDate = string.IsNullOrEmpty(stime) ? DateTime.Now.AddDays(-1) : Convert.ToDateTime(stime);
            DateTime eDate = string.IsNullOrEmpty(etime) ? DateTime.Now.AddDays(-7) : Convert.ToDateTime(etime);
            if (sDate >= eDate)
            {
                sDate = eDate.AddDays(-7);
            }

            DataSet ds =
                FacadeManage.aideNativeWebFacade.GetDataStatistics(Fetch.GetDateID(sDate), Fetch.GetDateID(eDate));
            List<StatisticsChart> data = new List<StatisticsChart>();
            if (ds != null && ds.Tables[0].Rows.Count > 0)
            {
                StatisticsChart sc = null;
                foreach (DataRow item in ds.Tables[0].Rows)
                {
                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt64(item["Gold"]);
                    sc.type = "平台金币";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt64(item["PayGold"]);
                    sc.type = "充值金币";
                    data.Add(sc);
                }
            }

            ajv.SetValidDataValue(true);
            ajv.AddDataItem("data", data);
            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取平台充值信息
        /// </summary>
        /// <param name="context"></param>
        private void GetPlatformPay(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            DateTime sDate = string.IsNullOrEmpty(stime) ? DateTime.Now.AddDays(-1) : Convert.ToDateTime(stime);
            DateTime eDate = string.IsNullOrEmpty(etime) ? DateTime.Now.AddDays(-7) : Convert.ToDateTime(etime);
            if (sDate >= eDate)
            {
                sDate = eDate.AddDays(-7);
            }

            DataSet ds =
                FacadeManage.aideNativeWebFacade.GetDataStatistics(Fetch.GetDateID(sDate), Fetch.GetDateID(eDate));
            List<StatisticsPayChart> data = new List<StatisticsPayChart>();
            if (ds != null && ds.Tables[0].Rows.Count > 0)
            {
                StatisticsPayChart sc = null;
                foreach (DataRow item in ds.Tables[0].Rows)
                {
                    sc = new StatisticsPayChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.amount = Convert.ToDecimal(item["PayAmount"]);
                    sc.type = "充值金额";
                    data.Add(sc);
                }
            }

            ajv.SetValidDataValue(true);
            ajv.AddDataItem("data", data);
            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取玩家开房统计
        /// </summary>
        /// <param name="context"></param>
        private void GetUserOpenRoom(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            DateTime sDate = string.IsNullOrEmpty(stime) ? DateTime.Now.AddDays(-1) : Convert.ToDateTime(stime);
            DateTime eDate = string.IsNullOrEmpty(etime) ? DateTime.Now.AddDays(-7) : Convert.ToDateTime(etime);
            if (sDate >= eDate)
            {
                sDate = eDate.AddDays(-7);
            }

            DataSet ds =
                FacadeManage.aideNativeWebFacade.GetDataStatistics(Fetch.GetDateID(sDate), Fetch.GetDateID(eDate));
            List<StatisticsChart> data = new List<StatisticsChart>();
            if (ds != null && ds.Tables[0].Rows.Count > 0)
            {
                StatisticsChart sc = null;
                foreach (DataRow item in ds.Tables[0].Rows)
                {
                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt32(item["CreateScoreRoom"]);
                    sc.type = "积分约战";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt32(item["CreateGoldRoom"]);
                    sc.type = "金币约战";
                    data.Add(sc);
                }
            }

            ajv.SetValidDataValue(true);
            ajv.AddDataItem("data", data);
            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取用户注册统计
        /// </summary>
        /// <param name="context"></param>
        private void GetRegisterStatictics(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            DateTime sDate = string.IsNullOrEmpty(stime) ? DateTime.Now.AddDays(-1) : Convert.ToDateTime(stime);
            DateTime eDate = string.IsNullOrEmpty(etime) ? DateTime.Now.AddDays(-7) : Convert.ToDateTime(etime);
            if (sDate >= eDate)
            {
                sDate = eDate.AddDays(-7);
            }

            DataSet ds =
                FacadeManage.aideNativeWebFacade.GetDataStatistics(Fetch.GetDateID(sDate), Fetch.GetDateID(eDate));
            List<StatisticsChart> data = new List<StatisticsChart>();
            if (ds != null && ds.Tables[0].Rows.Count > 0)
            {
                StatisticsChart sc = null;
                foreach (DataRow item in ds.Tables[0].Rows)
                {
                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt32(item["WebRegister"]);
                    sc.type = "网站注册";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt32(item["GameRegister"]);
                    sc.type = "游戏注册";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt32(item["H5Register"]);
                    sc.type = "H5注册";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt32(item["H5Register"]) + Convert.ToInt32(item["WebRegister"]) +
                               Convert.ToInt32(item["GameRegister"]);
                    sc.type = "合计注册";
                    data.Add(sc);
                }
            }

            ajv.SetValidDataValue(true);
            ajv.AddDataItem("data", data);
            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取服务费统计
        /// </summary>
        /// <param name="context"></param>
        private void GetRevenueStatictics(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            DateTime sDate = string.IsNullOrEmpty(stime) ? DateTime.Now.AddDays(-1) : Convert.ToDateTime(stime);
            DateTime eDate = string.IsNullOrEmpty(etime) ? DateTime.Now.AddDays(-7) : Convert.ToDateTime(etime);
            if (sDate >= eDate)
            {
                sDate = eDate.AddDays(-7);
            }

            DataSet ds =
                FacadeManage.aideNativeWebFacade.GetDataStatistics(Fetch.GetDateID(sDate), Fetch.GetDateID(eDate));
            List<StatisticsChart> data = new List<StatisticsChart>();
            if (ds != null && ds.Tables[0].Rows.Count > 0)
            {
                StatisticsChart sc = null;
                foreach (DataRow item in ds.Tables[0].Rows)
                {
                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt64(item["GameRevenue"]);
                    sc.type = "游戏服务费";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt64(item["InsureRevenue"]);
                    sc.type = "银行服务费";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt64(item["GameRevenue"]) + Convert.ToInt64(item["InsureRevenue"]);
                    sc.type = "合计服务费";
                    data.Add(sc);
                }
            }

            ajv.SetValidDataValue(true);
            ajv.AddDataItem("data", data);
            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取损耗统计
        /// </summary>
        /// <param name="context"></param>
        private void GetWasteStatictics(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            DateTime sDate = string.IsNullOrEmpty(stime) ? DateTime.Now.AddDays(-1) : Convert.ToDateTime(stime);
            DateTime eDate = string.IsNullOrEmpty(etime) ? DateTime.Now.AddDays(-7) : Convert.ToDateTime(etime);
            if (sDate >= eDate)
            {
                sDate = eDate.AddDays(-7);
            }

            DataSet ds =
                FacadeManage.aideNativeWebFacade.GetDataStatistics(Fetch.GetDateID(sDate), Fetch.GetDateID(eDate));
            List<StatisticsChart> data = new List<StatisticsChart>();
            if (ds != null && ds.Tables[0].Rows.Count > 0)
            {
                StatisticsChart sc = null;
                foreach (DataRow item in ds.Tables[0].Rows)
                {
                    sc = new StatisticsChart();
                    sc.time = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd");
                    sc.count = Convert.ToInt64(item["GameWaste"]);
                    sc.type = "游戏损耗";
                    data.Add(sc);
                }
            }

            ajv.SetValidDataValue(true);
            ajv.AddDataItem("data", data);
            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取游戏服务费统计
        /// </summary>
        /// <param name="context"></param>
        private void GetGameRevenueStatictics(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            if (string.IsNullOrEmpty(stime) || string.IsNullOrEmpty(etime))
            {
                return;
            }

            stime = stime + " 00:00:00";
            etime = etime + " 23:59:59";
            //IList<StatisticsRevenue> list = FacadeManage.aideTreasureFacade.GetDayGameRevenue(stime, etime);
            //List<StatisticsChart> data = new List<StatisticsChart>();
            //if(list != null && list.Count > 0)
            //{
            //    StatisticsChart sc = null;
            //    foreach(StatisticsRevenue item in list)
            //    {
            //        sc = new StatisticsChart();
            //        sc.time = item.TimeDate;
            //        sc.count = item.Revenue;
            //        sc.type = "游戏服务费";
            //        data.Add(sc);
            //    }
            //}
            //ajv.SetValidDataValue(true);
            //ajv.AddDataItem("data", data);
            //context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// 获取在线人数统计
        /// </summary>
        /// <param name="context"></param>
        private void GetUserOnlineStatictics(HttpContext context)
        {
            string stime = GameRequest.GetQueryString("stime");
            string etime = GameRequest.GetQueryString("etime");

            stime = string.IsNullOrEmpty(stime) ? DateTime.Now.ToString("yyyy-MM-dd") : stime;
            etime = string.IsNullOrEmpty(etime) ? DateTime.Now.ToString("yyyy-MM-dd") : etime;
            stime = stime + " 00:00:00";
            etime = etime + " 23:59:59";
            IList<StatisticsOnline> list = FacadeManage.aidePlatformFacade.GetUserOnlineStatistics(stime, etime);
            List<StatisticsChart> data = new List<StatisticsChart>();
            if (list != null && list.Count > 0)
            {
                StatisticsChart sc = null;
                foreach (StatisticsOnline item in list)
                {
                    sc = new StatisticsChart();
                    sc.time = item.DTime.ToString("yyyy-MM-dd HH:mm:ss");
                    sc.count = item.RUser;
                    sc.type = "在线用户";
                    data.Add(sc);

                    sc = new StatisticsChart();
                    sc.time = item.DTime.ToString("yyyy-MM-dd HH:mm:ss");
                    sc.count = item.AUser;
                    sc.type = "在线机器人";
                    data.Add(sc);
                }
            }

            ajv.SetValidDataValue(true);
            ajv.AddDataItem("data", data);
            context.Response.Write(ajv.SerializeToJson());
        }

        /// <summary>
        /// http服务工具
        /// </summary>
        /// <param name="ctx"></param>
        protected void HttpService(HttpContext ctx)
        {
            var method = GameRequest.GetString("method");
            var param = GameRequest.GetString("params");
            var url = GameRequest.GetString("url");
            var encoding = string.IsNullOrEmpty(GameRequest.GetString("encoding"))
                ? Encoding.UTF8
                : Encoding.GetEncoding(GameRequest.GetString("encoding"));
            var format = GameRequest.GetString("format");
            var result = "";
            var code = 0;
            var contentType = "";
            if (string.IsNullOrEmpty(url))
            {
                code = -1;
                result = "url 参数为必填参数";
                goto Resp;
            }

            if (string.IsNullOrEmpty(method))
            {
                method = ctx.Request.HttpMethod.ToUpper();
            }

            if (string.IsNullOrEmpty(format))
            {
                format = "json";
            }

            try
            {
                result = method == "POST" ? Fetch.Post(param, url,out contentType,encoding) : Fetch.Get(url + "?" + param,out contentType, encoding);
            }
            catch (Exception ex)
            {
                code = -1;
                result = "errMsg:" + ex.Message;
            }

            Resp:
            if (format == "json")
            {
                var dic = new Dictionary<string, object>
                {
                    {"code", code},
                    {"encoding", encoding.WebName},
                    {"contentType", contentType},
                    {"response", result}
                };
                ctx.Response.Write(JsonMapper.ToJson(dic));
            }
            else if (format == "raw")
            {
                ctx.Response.ContentType = contentType;
                ctx.Response.Write(result);
            }
        }

        public bool IsReusable
        {
            get { return false; }
        }
    }
}