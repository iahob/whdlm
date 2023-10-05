using Game.Utils;
using Game.Facade;
using System;
using System.Collections.Generic;
using System.Web;
using Game.Facade.Enum;
using Game.Facade.DataStruct;
using Game.Entity.NativeWeb;
using Game.Entity.Treasure;
using Game.Kernel;
using Game.Entity.Platform;
using Game.Web.Helper;
using System.Data;
using System.Linq;
using Game.Entity.Accounts;
using Game.Entity.Group;
using System.Text.RegularExpressions;
using Game.Entity.Agent;

namespace Game.Web.WS
{
    /// <summary>
    /// NewMoblieInterface 的摘要说明
    /// </summary>
    public class NewMoblieInterface : IHttpHandler
    {
        // ReSharper disable once FieldCanBeMadeReadOnly.Local
        private AjaxJsonValid _ajv;

        private int _userid;
        private string _device;
        private long _groupid;

        /// <summary>
        /// 统一处理入口（主要验证）
        /// </summary>
        /// <param name="context"></param>
        public void ProcessRequest(HttpContext context)
        {
            try
            {
                //允许跨站请求域名
                context.Response.AddHeader("Access-Control-Allow-Origin", AppConfig.MoblieInterfaceDomain);
                //接口返回数据格式
                context.Response.ContentType = "application/json";
                //接口请求类型
                string action = GameRequest.GetQueryString("action").ToLower();

                //获取参数
                _userid = GameRequest.GetQueryInt("userid", 0);
                _device = Fetch.GetFilterString("device");
                _groupid = !string.IsNullOrEmpty(Fetch.GetFilterString("groupid"))
                    ? Convert.ToInt64(Fetch.GetFilterString("groupid"))
                    : 0;
                _ajv = new AjaxJsonValid();
                //#if !DEBUG //DEBUG情况下不验证
                //            string time = GameRequest.GetQueryString("time");
                //            string sign = GameRequest.GetQueryString("sign");

                //            //签名验证
                //            _ajv =
                //Fetch.VerifySignData((context.Request.QueryString["userid"] == null ? "" : _userid.ToString()) + AppConfig.MoblieInterfaceKey + time,
                //                sign);
                ////            Log4Net.WriteInfoLog("signStr:"+(context.Request.QueryString["userid"] == null ? "" : _userid.ToString()) + AppConfig.MoblieInterfaceKey + time + " sign:"+sign);
                //            if (_ajv.code == (int)ApiCode.VertySignErrorCode)
                //            {
                //                context.Response.Write(_ajv.SerializeToJson());
                //                return;
                //            }
                //#endif
                //参数验证
                if (context.Request.QueryString["userid"] != null && _userid <= 0)
                {
                    _ajv.code = (int)ApiCode.VertyParamErrorCode;
                    _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " userid 错误");
                    context.Response.Write(_ajv.SerializeToJson());
                    return;
                }
                //获取其他参数
                int configid = GameRequest.GetQueryInt("configid", 0);
                int typeid = GameRequest.GetQueryInt("typeid", 0);

                switch (action)
                {
                    case "webversion":
                        _ajv.SetDataItem("apiVersion", 20171220);
                        _ajv.SetDataItem("webVersion", "V1.0.1");
                        _ajv.SetDataItem("updateAt", "2018/04/08");
                        _ajv.SetValidDataValue(true);
                        break;
                    //获取手机端登录数据
                    case "getmobilelogindata":
                        _ajv.SetDataItem("apiVersion", 20171017);
                        GetMobileLoginData();
                        break;
                    //获取手机端登录后数据
                    case "getmobileloginlater":
                        _ajv.SetDataItem("apiVersion", 20171213);
                        GetMobileLoginLater();
                        break;
                    //获取充值产品列表
                    case "getpayproduct":
                        _ajv.SetDataItem("apiVersion", 20171028);
                        //获取参数
                        int typeId = GameRequest.GetQueryInt("typeid", 0);
                        GetPayProduct(typeId);
                        break;
                    //领取推广有效好友奖励
                    case "receivespreadaward":
                        //参数验证
                        if (configid <= 0)
                        {
                            _ajv.code = (int)ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " configid 错误");
                            context.Response.Write(_ajv.SerializeToJson());
                            return;
                        }
                        ReceiveSpreadAward(configid);
                        break;
                    case "getgameintrolist":
                        _ajv.SetDataItem("apiVersion", 20171107);
                        GetGameIntroList();
                        break;
                    //钻石充值下单
                    case "createpayorder":
                        _ajv.SetDataItem("apiVersion", 20180327);
                        //获取参数
                        string paytype = GameRequest.GetQueryString("paytype");
                        string openid = GameRequest.GetQueryString("openid");
                        string subtype = GameRequest.GetQueryString("subtype");

                        //参数验证
                        if (configid <= 0 || paytype.Equals(""))
                        {
                            _ajv.code = (int)ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), "");
                            context.Response.Write(_ajv.SerializeToJson());
                            return;
                        }
                        context.Response.Write(CreatePayOrder(configid, paytype, openid, subtype).SerializeToJson());
                        return;

                    //在线充值列表
                    case "getonlinewechatlist":
                        GetOnLineWeChatList();
                        break;
                    //获取排行榜数据
                    case "getrankingdata":
                        _ajv.SetDataItem("apiVersion", 20171129);
                        //参数验证
                        if (typeid <= 0 || typeid > 7)
                        {
                            _ajv.code = (int)ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " typeid 错误");
                            context.Response.Write(_ajv.SerializeToJson());
                            return;
                        }

                        GetRankingData(_userid, typeid);
                        //GetRankingData(typeid);
                        break;
                    //获取财富信息
                    case "getuserwealth":
                        GetUserWealth();
                        break;
                    //获取玩家信息
                    case "getuserinfo":
                        _ajv.SetDataItem("apiVersion", 20171208);
                        GetUserInfo();
                        break;
                    //领取排行奖励
                    case "receiverankingaward":
                        _ajv.SetDataItem("apiVersion", 20171213);
                        //获取参数
                        int dateid = GameRequest.GetQueryInt("dateid", 0);

                        //参数验证
                        if (dateid <= 0 || typeid <= 0)
                        {
                            _ajv.code = (int)ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), "");
                            context.Response.Write(_ajv.SerializeToJson());
                            return;
                        }
                        ReceiveRankingAward(dateid, typeid);
                        break;
                    //获取游戏列表
                    case "getgamelist":
                        GetGameList();
                        break;
                    //领取注册赠送奖励
                    case "receiveregistergrant":
                        _ajv.SetDataItem("apiVersion", 20171213);
                        ReceiveRegisterGrant();
                        break;
                    //金币流水记录
                    case "recordtreasuretrade":
                        RecordTreasureTrade();
                        break;
                    //钻石流水记录
                    case "recorddiamondstrade":
                        RecordDiamondsTrade();
                        break;
                    //钻石兑换金币
                    case "diamondexchgold":
                        _ajv.SetDataItem("apiVersion", 20171213); //for 响应规范
                        if (configid <= 0 || typeid < 0)
                        {
                            _ajv.code = (int)ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                                " configid或typeid 错误");
                            context.Response.Write(_ajv.SerializeToJson());
                            return;
                        }
                        DiamondExchGold(configid, typeid);
                        break;
                    //钻石兑换金币列表
                    case "diamondexchgoldconfig":
                        DiamondExchGoldConfig();
                        break;
                    case "getpayorderstatus":
                        _ajv.SetDataItem("apiVersion", 20171127);
                        string orderid = Fetch.GetFilterString("orderid");
                        if (string.IsNullOrEmpty(orderid))
                        {
                            _ajv.code = (int)ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " orderid 错误");
                            context.Response.Write(_ajv.SerializeToJson());
                            return;
                        }
                        GetPayOrderStatus(orderid);
                        break;
                    case "getquestionandanswerlist":
                        _ajv.SetDataItem("apiVersion", 20180125);
                        GetQuestionAndAnswerList();
                        break;
                    case "agentsynclogin":
                        _ajv.SetDataItem("apiVersion", 20180309);
                        AgentSyncLogin();
                        break;
                    case "getbattlerecord":
                        _ajv.SetDataItem("apiVersion", 20180424);
                        GetBattleRecord(typeid);
                        break;
                    case "getgroupbattlerecord":
                        _ajv.SetDataItem("apiVersion", 20180424);
                        if (_groupid <= 0)
                        {
                            _ajv.code = (int)ApiCode.VertyParamErrorCode;
                            _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                                " groupid 错误");
                            return;
                        }
                        GetGroupBattleRecord();
                        break;
                    case "getcode":
                        GetCode(context);
                        break;
                    case "bindmobile":
                        BindMobile(context);
                        break;
                    case "bindmobileconfig":
                        BindMobileConfig(context);
                        break;
                    case "sharetimesreward":
                        ShareTimesReward(context);
                        break;
                    case "getsharereward":
                        GetShareReward(context);
                        break;


                    #region 代理接口模块
                    //获取代理商用户信息
                    case "getagentinfo":
                        _ajv.SetDataItem("apiVersion", 20190220);
                        GetAgentInfo();
                        break;
                    //根据GameID查询用户昵称（检查对象存在用）
                    case "getnicknamebygameid":
                        GetNickNameByGameID(GameRequest.GetInt("gameid", 0));
                        break;
                    //获取记录列表 by type
                    case "getrecord": //1.3
                        GetRecord(Fetch.GetFilterString("type"));
                        break;
                    //用户代理列表获取接口
                    case "getbelowlist": //1.4
                        GetBelowList(Fetch.GetFilterString("type"), Fetch.GetFilterString("query"));
                        break;
                    // 代理返利情况
                    case "getawardinfo": //1.5
                        GetAwardInfo();
                        break;
                    //提取钻石返利或金币返利
                    case "presentscore": //1.6
                        TakeScoreORDiamond(GameRequest.GetInt("gameid", 0), Fetch.GetFilterString("password"),
                            GameRequest.GetInt("count", 0), Convert.ToByte(GameRequest.GetInt("type", 0)));
                        break;
                    //赠送身上钻石或金币接口
                    case "presentdiamondorscore":
                        PresentDiamondOrScore(GameRequest.GetInt("gameid", 0), Fetch.GetFilterString("password"),
                          GameRequest.GetInt("count", 0), Convert.ToByte(GameRequest.GetInt("type", 0)));
                        break;
                    //设置安全密码
                    case "setpassword": //1.7
                        SetSafePass(Fetch.GetFilterString("password"));
                        break;
                    //修改安全密码接口
                    case "updatepassword": //1.8
                        UpdateSafePass(Fetch.GetFilterString("oldPassword"), Fetch.GetFilterString("newPassword"));
                        break;
                    //修改代理信息接口
                    case "updateinfo": //1.9
                        UpdateAgentInfo(Fetch.GetFilterString("address"), Fetch.GetFilterString("phone"),
                            Fetch.GetFilterString("qq"));
                        break;
                    //添加下级代理
                    case "addagent": //1.10
                        AddBelowAgent(GameRequest.GetInt("gameid", 0), Fetch.GetFilterString("agentDomain"),
                            Fetch.GetFilterString("compellation"),
                            "", Fetch.GetFilterString("phone"),
                            ""
                        );
                        break;
                    //获取返利配置信息列表
                    case "getreturnawardconfig":
                        GetReturnAwardConfig(context);
                        break;
                    //绑定上级代理
                    case "bindagent":
                        BindAgent();
                        break;
                    //获取我的提取赠送列表
                    case "getmytakegrantrecord":
                        GetMyTakeGrantRecord();
                        break;
                    #endregion 

                    default:
                        _ajv.code = (int)ApiCode.VertyParamErrorCode;
                        _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " action 错误");
                        break;
                }

                context.Response.Write(_ajv.SerializeToJson());
            }
            catch (Exception ex)
            {
                Log4Net.WriteInfoLog("下面一条为接口故障信息", "MobileInterface");
                Log4Net.WriteErrorLog(ex, "MobileInterface");
                _ajv = new AjaxJsonValid
                {
                    code = (int)ApiCode.LogicErrorCode,
                    msg = EnumHelper.GetDesc(ApiCode.LogicErrorCode)
                };
                context.Response.Write(_ajv.SerializeToJson());
            }
            context.Response.End();
        }

        /// <summary>
        /// 获取手机端登录数据
        /// </summary>
        private void GetMobileLoginData()
        {
            int PlatformType = GameRequest.GetQueryInt("PlatformType", 1);
            ConfigInfo webConfig = Fetch.GetWebSiteConfig();
            ConfigInfo MobileConfig = Fetch.GetMobileConfig();
            string imageServerHost = webConfig.Field2;
            //获取登录数据
            DataSet ds = FacadeManage.aideNativeWebFacade.GetMobileLoginInfo();
            //获取系统配置信息
            MobileSystemConfig config = GetMobileSystemConfig(ds.Tables[0]);

            GroupSystemConfig Groupconfig = GetGroupConfig(ds.Tables[9]);
            //大喇叭道具信息由道具表提供
            //GameProperty Diamonprop = FacadeManage.aidePlatformFacade.GetGameProperty(306,0);//钻石购买道具
            //GameProperty Goldprop = FacadeManage.aidePlatformFacade.GetGameProperty(306, 1);//金币购买道具

            //if (PlatformType == 1)
            //{
            //    config.GoldBuyPropCount = Convert.ToInt32(ds.Tables[6].Rows[0][0]);//金币购买道具
            //}
            //else if (PlatformType == 2)
            //{
            //    config.DiamondBuyPropCount = Convert.ToInt32(ds.Tables[5].Rows[0][0]);//钻石购买道具
            //}
            //else
            //{
            //config.GoldBuyPropCount = Convert.ToInt32(ds.Tables[6].Rows[0][0]);//金币购买道具
            // config.DiamondBuyPropCount = Convert.ToInt32(ds.Tables[5].Rows[0][0]);//钻石购买道具
            //}

            //获取客服界面配置
            MobileCustomerService mcs = DataHelper.ConvertRowToObject<MobileCustomerService>(ds.Tables[1].Rows[0]);
            mcs.Link = imageServerHost + "/site/qrcustomer.png";
            //获取系统公告配置
            IList<NoticeMobile> noticelist = DataHelper.ConvertDataTableToObjects<NoticeMobile>(ds.Tables[2]);
            //获取手机固定位广告图
            IList<AdsMobile> plate = DataHelper.ConvertDataTableToObjects<AdsMobile>(ds.Tables[3]);
            foreach (AdsMobile ads in plate)
            {
                ads.ResourceURL = ads.ResourceURL.IndexOf("http://", StringComparison.Ordinal) < 0
                    ? imageServerHost + ads.ResourceURL
                    : ads.ResourceURL;
            }
            //获取手机弹出广告图
            IList<AdsMobile> alert =
                DataHelper.ConvertDataTableToObjects<AdsMobile>(_device == "h5" ? ds.Tables[3] : ds.Tables[4]);
            foreach (AdsMobile ads in alert)
            {
                ads.ResourceURL = ads.ResourceURL.IndexOf("http://", StringComparison.Ordinal) < 0
                    ? imageServerHost + ads.ResourceURL
                    : ads.ResourceURL;
            }

            //获取手机活动广告
            IList<AdsMobile> activity =
                DataHelper.ConvertDataTableToObjects<AdsMobile>(ds.Tables[7]);
            foreach (AdsMobile ads in activity)
            {
                ads.ResourceURL = ads.ResourceURL.IndexOf("http://", StringComparison.Ordinal) < 0
                    ? imageServerHost + ads.ResourceURL
                    : ads.ResourceURL;
            }
            //获取签到配置 
            //IList<GameSignInMobile> GameSign =
            //   DataHelper.ConvertDataTableToObjects<GameSignInMobile>(ds.Tables[8]);

            //foreach (GameSignInMobile Sign in GameSign)
            //{
            //    Sign.ResourceURL = Sign.ResourceURL.IndexOf("http://", StringComparison.Ordinal) < 0
            //        ? imageServerHost + Sign.ResourceURL
            //        : Sign.ResourceURL;
            //}

            //输出数据
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("systemConfig", config);
            _ajv.SetDataItem("Groupconfig", Groupconfig);
            _ajv.SetDataItem("customerService", mcs);
            _ajv.SetDataItem("systemNotice", noticelist);
            _ajv.SetDataItem("adsList", plate);  //广告位取消
            _ajv.SetDataItem("adsAlertList", alert);
            _ajv.SetDataItem("activityList", activity);
            _ajv.SetDataItem("imageServerHost", imageServerHost);
            _ajv.SetDataItem("MobileConfig", MobileConfig);
            _ajv.SetDataItem("shareUrl", "http://"+webConfig.Field4);
        }

        /// <summary>
        /// 获取手机端登录后数据
        /// </summary>
        private void GetMobileLoginLater()
        {
            //获取登录成功后数据
            DataSet ds = FacadeManage.aideAccountsFacade.GetMobileLoginLaterData(_userid);
            //获取推广链接（线上版本请将第三个参数设置成true，内部版本则为false）
            string shareLink = GetSpreadLink(ds.Tables[0], false);
            string U3DShareLink = GetU3DSpreadLink(ds.Tables[0]);
            //获取注册奖励
            DataTable table = ds.Tables[1];
            int grantDiamond = (table != null && table.Rows.Count > 0)
                ? Convert.ToInt32(table.Rows[0]["GrantDiamond"])
                : 0;
            int grantGold = (table != null && table.Rows.Count > 0)
                ? Convert.ToInt32(table.Rows[0]["GrantGold"])
                : 0;
            //获取推广配置
            IList<SpreadConfigMobile> spreadList =
                DataHelper.ConvertDataTableToObjects<SpreadConfigMobile>(ds.Tables[2]);
            // 获取玩家的排行版信息 上210需注释
            IList<CacheRankAward> wealthRank =
                DataHelper.ConvertDataTableToObjects<CacheRankAward>(ds.Tables[3]);
            IList<CacheRankAward> winCountRank =
                DataHelper.ConvertDataTableToObjects<CacheRankAward>(ds.Tables[4]);
            //获取玩家的排行版信息 旧排行榜
            //IList<RankingRecevieMobile> rankList =
            //    DataHelper.ConvertDataTableToObjects<RankingRecevieMobile>(ds.Tables[3]);
            //获取有效好友
            table = ds.Tables[5];/*ds.Tables[4];*/
            int friendCount = (table != null && table.Rows.Count > 0) ? Convert.ToInt32(table.Rows[0]["Total"]) : 0;

            // 获取排行榜奖励配置 上210需注释
            IList<RankingConfig> rankconfig =
                DataHelper.ConvertDataTableToObjects<RankingConfig>(ds.Tables[6]);

            //输出信息
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("sharelink", shareLink);
            _ajv.SetDataItem("U3DShareLink", U3DShareLink);
            _ajv.SetDataItem("hasGrant", grantDiamond > 0 || grantGold > 0);
            _ajv.SetDataItem("grantDiamond", grantDiamond);
            _ajv.SetDataItem("grantGold", grantGold);
            _ajv.SetDataItem("friendcount", friendCount);
            _ajv.SetDataItem("spreadlist", spreadList);
            /*_ajv.SetDataItem("ranklist", rankList);*/ //旧排行版上210
            _ajv.SetDataItem("wealthrank", wealthRank);
            _ajv.SetDataItem("wincountrank", winCountRank);
            _ajv.SetDataItem("rankconfig", rankconfig);
        }

        /// <summary>
        /// 获取充值产品列表
        /// </summary>
        /// <param name="typeId"></param>
        private void GetPayProduct(int typeId)
        {
            //获取充值数据
            DataSet ds = FacadeManage.aideTreasureFacade.GetAppPayConfigList(typeId, _userid);
            //获取首充状态
            DataTable table = ds.Tables[0];
            bool flag = (table != null && table.Rows.Count == 0);
            //获取充值产品列表
            IList<AppPayConfigMoile> list = DataHelper.ConvertDataTableToObjects<AppPayConfigMoile>(ds.Tables[1]);

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("list", list);
        }

        /// <summary>
        /// 领取推广有效好友奖励
        /// </summary>
        private void ReceiveSpreadAward(int configid)
        {
            //领取奖励
            Message msg =
                FacadeManage.aideTreasureFacade.ReceiveSpreadAward(_userid, configid, GameRequest.GetUserIP());
            _ajv.SetValidDataValue(msg.Success);
            _ajv.msg = msg.Content;
        }

        /// <summary>
        /// 获取游戏简介列表
        /// </summary>
        private void GetGameIntroList()
        {
            IList<GameRule> gameRules = FacadeManage.aideNativeWebFacade.GetGameRuleList();
            IList<MobileGameRule> ruleList = new List<MobileGameRule>();
            ConfigInfo webConfig = Fetch.GetWebSiteConfig();
            string imageServerHost = webConfig.Field2;
            if (gameRules.Count > 0)
            {
                foreach (GameRule gameRule in gameRules)
                {
                    ruleList.Add(new MobileGameRule()
                    {
                        KindID = gameRule.KindID,
                        KindName = gameRule.KindName,
                        Content = gameRule.KindIntro,
                        RuleImg = imageServerHost + gameRule.RuleImg.Replace("Upload/", ""),
                        Type = gameRule.Type,
                        LogID = gameRule.LogID
                    });
                }
            }
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("ruleList", ruleList);
        }

        /// <summary>
        /// 钻石充值下单
        /// </summary>
        /// <param name="configid"></param>
        /// <param name="paytype"></param>
        /// <param name="openid"></param>
        /// <param name="subtype"></param>
        /// <returns>AjaxJsonValid</returns>
        private AjaxJsonValid CreatePayOrder(int configid, string paytype, string openid, string subtype)
        {
            //下单信息
            OnLinePayOrder order = new OnLinePayOrder
            {
                UserID = _userid,
                ConfigID = configid,
                OrderAddress = GameRequest.GetUserIP()
            };
            switch (paytype)
            {
                case "wx":
                    order.ShareID = 101;
                    order.OrderID = Fetch.GetOrderIDByPrefix("WXAPP");
                    break;
                case "zfb":
                    order.ShareID = 201;
                    order.OrderID = Fetch.GetOrderIDByPrefix("ZFBAPP");
                    break;
                case "hwx":
                    order.ShareID = 102;
                    order.OrderID = Fetch.GetOrderIDByPrefix("HWX");
                    break;
                case "lq":
                    order.ShareID = 301;
                    order.OrderID = Fetch.GetOrderIDByPrefix("360LQ");
                    break;
                case "jft":
                    switch (subtype)
                    {
                        case "wx":
                            order.ShareID = 302;
                            order.OrderID = Fetch.GetOrderIDByPrefix("JFTH5WX");
                            break;
                        case "zfb":
                            order.ShareID = 303;
                            order.OrderID = Fetch.GetOrderIDByPrefix("JFTH5ZFB");
                            break;
                        default:
                            order.ShareID = 300;
                            order.OrderID = Fetch.GetOrderIDByPrefix("JFT");
                            break;
                    }
                    break;
                case "jftyc":
                    switch (subtype)
                    {
                        case "wx":
                            order.ShareID = 401;
                            order.OrderID = Fetch.GetOrderIDByPrefix("JFTYCWX");
                            break;
                        case "zfb":
                            order.ShareID = 402;
                            order.OrderID = Fetch.GetOrderIDByPrefix("JFTYCZFB");
                            break;
                        case "YSF":
                            order.ShareID = 403;
                            order.OrderID = Fetch.GetOrderIDByPrefix("JFTYCZFBYSF");
                            break;
                        default:
                            order.ShareID = 400;
                            order.OrderID = Fetch.GetOrderIDByPrefix("JFTYC");
                            break;
                    }
                    break;
                //case "pays":
                //    switch (subtype)
                //    {
                //        case "wx":
                //            order.ShareID = 401;
                //            order.OrderID = Fetch.GetOrderIDByPrefix("PaysWX");
                //            break;
                //        case "zfb":
                //            order.ShareID = 402;
                //            order.OrderID = Fetch.GetOrderIDByPrefix("PaysZFB");
                //            break;
                //        default:
                //            order.ShareID = 400;
                //            order.OrderID = Fetch.GetOrderIDByPrefix("Pays");
                //            break;
                //    }
                //    break;
                default:
                    _ajv.code = (int)ApiCode.VertyParamErrorCode;
                    _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode), " paytype（充值类型） 错误");
                    return _ajv;
            }

            //下单操作
            Message umsg = FacadeManage.aideTreasureFacade.CreatePayOrderInfo(order, _device);
            if (umsg.Success)
            {
                OnLinePayOrder orderReturn = umsg.EntityList[0] as OnLinePayOrder;
                if (paytype == "wx" || paytype == "hwx")
                {
                    _ajv.SetDataItem("PayPackage",
                    GetWxPayPackage(orderReturn, paytype, openid, GameRequest.GetCurrentFullHost()));
                }
                else if (paytype == "lq")
                {
                    LQPay.LQPayRequest request =
                        new LQPay.LQPayRequest(orderReturn, subtype == "zfb" ? "alipay" : "weixin");
                    _ajv.SetDataItem("PayUrl", HttpUtility.UrlDecode(LQPay.GetPayPackage(request.ToUrl("PayUrl"))));
                }
                else if (paytype == "jft")
                {
                    JFTPay.JFTH5Request request =
                        new JFTPay.JFTH5Request(orderReturn?.OrderID, orderReturn?.Amount.ToString("F2"),
                            subtype == "zfb" ? "ZFB" : "WX", orderReturn?.GameID.ToString(),
                            Utility.UserIP.Replace(".", "_"))
                        {
                            p25_terminal = string.IsNullOrEmpty(Fetch.GetFilterString("terminaltype")) ? "3" : Fetch.GetFilterString("terminaltype")
                        };
                    //request.SetTerminal(Fetch.GetTerminalType(GameRequest.Request));
                    if (AppConfig.Mode == AppConfig.CodeMode.Dev)
                    {
                        request.paytype = "ZZ"; //测试服用，正式时请注释掉此行
                    }
                    _ajv.SetDataItem("testUrl", request.ToUrl());
                    _ajv.SetDataItem("PayUrl", JFTPay.Config.JFTH5Url);
                    _ajv.SetDataItem("Params", request.UrlParams());
                }
                else if (paytype == "jftyc")
                {
                    JFTYCPay.JFTYCRequest request = new JFTYCPay.JFTYCRequest(orderReturn, subtype, GameRequest.GetCurrentFullHost());
                    _ajv.SetDataItem("PayUrl", request.GetPayUrl());
                }
                _ajv.SetDataItem("OrderID", orderReturn?.OrderID ?? "");
            }
            _ajv.SetValidDataValue(umsg.Success);
            _ajv.code = umsg.MessageID;
            _ajv.msg = umsg.Content;
            return _ajv;
        }

        /// <summary>
        /// 获取排行榜数据
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="typeid">排行类型</param>上211
        private void GetRankingData(int userid, int typeid)
        {
            //获取排行榜数据
            IList<CacheWealthRank> wealthRank = null;
            IList<CacheGameRank> gameRank = null;
            bool isRank = false;
            DataSet ds = FacadeManage.aideNativeWebFacade.GetDayRankingData(userid, typeid);
            switch (typeid)
            {
                case 1:
                    wealthRank = DataHelper.ConvertDataTableToObjects<CacheWealthRank>(ds.Tables[0]);
                    if (ds.Tables[1].Rows.Count > 0)
                    {
                        isRank = true;
                        CacheWealthRank wealth = DataHelper.ConvertRowToObject(typeof(CacheWealthRank), ds.Tables[1].Rows[0]) as CacheWealthRank;
                        wealthRank.Insert(0, wealth);
                    }
                    break;
                case 2:
                    gameRank = DataHelper.ConvertDataTableToObjects<CacheGameRank>(ds.Tables[0]);
                    if (ds.Tables[1].Rows.Count > 0)
                    {
                        isRank = true;
                        CacheGameRank game = DataHelper.ConvertRowToObject(typeof(CacheGameRank), ds.Tables[1].Rows[0]) as CacheGameRank;
                        gameRank.Insert(0, game);
                    }
                    break;
            }

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("IsRank", isRank);
            _ajv.SetDataItem("WealthRank", wealthRank);
            _ajv.SetDataItem("GameRank", gameRank);
        }


        /// <summary>
        /// 获取财富信息
        /// </summary>
        private void GetUserWealth()
        {
            //获取财富信息
            long score = 0;
            long insureScore = 0;
            long diamond = 0;
            long awardticket = 0;
            UserWealth wealth = FacadeManage.aideTreasureFacade.GetUserWealth(_userid);
            if (wealth != null)
            {
                score = wealth.Score;
                insureScore = wealth.InsureScore;
                diamond = wealth.Diamond;
                awardticket = wealth.AwardTicket;
            }

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("diamond", diamond);
            _ajv.SetDataItem("score", score);
            _ajv.SetDataItem("insure", insureScore);
            _ajv.SetDataItem("awardticket", awardticket);
        }

        /// <summary>
        /// 获取玩家信息
        /// </summary>
        private void GetUserInfo()
        {
            //获取财富信息
            AccountsInfo userInfo = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(_userid);
            if (userInfo == null) return;
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("UserID", userInfo.UserID);
            _ajv.SetDataItem("GameID", userInfo.GameID);
            _ajv.SetDataItem("CustomID", userInfo.CustomID);
            _ajv.SetDataItem("NickName", userInfo.NickName);
            _ajv.SetDataItem("UnderWrite", userInfo.UnderWrite);
            _ajv.SetDataItem("LastLogonIP", userInfo.LastLogonIP);
            _ajv.SetDataItem("PlaceName", userInfo.PlaceName);
        }

        /// <summary>
        /// 领取排行榜奖励
        /// </summary>
        /// <param name="dateid"></param>
        /// <param name="typeid"></param>
        private void ReceiveRankingAward(int dateid, int typeid)
        {
            //领取排行榜奖励
            Message msg =
                FacadeManage.aideNativeWebFacade.ReceiveRankingAward(_userid, dateid, typeid, GameRequest.GetUserIP());
            if (msg.Success)
            {
                _ajv.SetValidDataValue(true);
                Entity.NativeWeb.ReceiveRankAward currency = msg.EntityList[0] as Entity.NativeWeb.ReceiveRankAward;
                _ajv.SetDataItem("score", currency.Score);
                _ajv.SetDataItem("diamond", currency.Diamond);
            }
            _ajv.msg = msg.Content;
        }

        /// <summary>
        /// 获取游戏列表
        /// </summary>
        private void GetGameList()
        {
            //获取游戏列表
            DataSet ds = FacadeManage.aidePlatformFacade.GetMobileGameAndVersion();
            //获取大厅版本
            string version = "";
            string downloadUrl = "";
            string resVersion = "";
            string iosUrl = "";
            DataRow row = (ds.Tables[0] != null && ds.Tables[0].Rows.Count > 0) ? ds.Tables[0].Rows[0] : null;
            if (row != null)
            {
                downloadUrl = row["Field1"].ToString();
                version = row["Field2"].ToString();
                resVersion = row["Field3"].ToString();
                iosUrl = row["Field4"].ToString();
            }
            //获取游戏列表
            IList<MobileKindItem> list = DataHelper.ConvertDataTableToObjects<MobileKindItem>(ds.Tables[1]);

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("downloadurl", downloadUrl);
            _ajv.SetDataItem("clientversion", version);
            _ajv.SetDataItem("resversion", resVersion);
            _ajv.SetDataItem("ios_url", iosUrl);
            _ajv.SetDataItem("gamelist", list);
        }

        /// <summary>
        /// 领取注册赠送奖励
        /// </summary>
        private void ReceiveRegisterGrant()
        {
            //领取注册奖励
            Message msg = FacadeManage.aideTreasureFacade.ReceiveRegisterAward(_userid, GameRequest.GetUserIP());
            if (msg.Success)
            {
                _ajv.SetValidDataValue(true);
                UserWealth wealth = msg.EntityList[0] as UserWealth;
                _ajv.SetDataItem("score", wealth?.Score ?? 0);
                _ajv.SetDataItem("insure", wealth?.InsureScore ?? 0);
                _ajv.SetDataItem("diamond", wealth?.Diamond ?? 0);
            }
            _ajv.msg = msg.Content;
        }

        /// <summary>
        /// 金币流水记录
        /// </summary>
        private void RecordTreasureTrade()
        {
            //获取参数
            int pageIndex = GameRequest.GetQueryInt("page", 1);
            int pageSize = GameRequest.GetQueryInt("size", 10);

            //获取流水记录
            IList<TreasureStream> list = new List<TreasureStream>();
            string where = $" WHERE UserID={_userid}";
            PagerSet ps = FacadeManage.aideRecordFacade.GetGoldStreamList(where, pageIndex, pageSize);
            DataTable table = ps.PageSet.Tables[0];
            if (table != null && table.Rows.Count > 0)
            {
                foreach (DataRow item in table.Rows)
                {
                    TreasureStream stream = new TreasureStream
                    {
                        SerialNumber = item["SerialNumber"].ToString(),
                        SerialTime = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd HH:mm:ss"),
                        BeforeGold = Convert.ToInt64(item["CurScore"]) + Convert.ToInt64(item["CurInsureScore"]),
                        ChangeGold = Convert.ToInt32(item["ChangeScore"]),
                        AfterGold = Convert.ToInt64(item["CurScore"]) + Convert.ToInt64(item["CurInsureScore"]) +
                                    //银行存取操作不需要加上变化值
                                    ((RecordTreasureType)item["TypeID"] == RecordTreasureType.存入银行 ||
                                     (RecordTreasureType)item["TypeID"] == RecordTreasureType.银行取出
                                        ? 0
                                        : Convert.ToInt32(item["ChangeScore"])),
                        TypeName = EnumHelper.GetDesc((RecordTreasureType)item["TypeID"])
                    };
                    list.Add(stream);
                }
            }

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("list", list);
        }

        /// <summary>
        /// 钻石流水记录
        /// </summary>
        private void RecordDiamondsTrade()
        {
            int pageIndex = GameRequest.GetQueryInt("page", 1);
            int pageSize = GameRequest.GetQueryInt("size", 10);

            //获取流水记录
            IList<DiamondStream> list = new List<DiamondStream>();
            string where = $" WHERE UserID={_userid}";
            PagerSet ps = FacadeManage.aideRecordFacade.GetDiamondStreamList(where, pageIndex, pageSize);
            DataTable table = ps.PageSet.Tables[0];
            if (table != null && table.Rows.Count > 0)
            {
                foreach (DataRow item in table.Rows)
                {
                    DiamondStream stream = new DiamondStream
                    {
                        SerialNumber = item["SerialNumber"].ToString(),
                        SerialTime = Convert.ToDateTime(item["CollectDate"]).ToString("yyyy-MM-dd HH:mm:ss"),
                        BeforeDiamond = Convert.ToInt64(item["CurDiamond"]),
                        ChangeDiamond = Convert.ToInt32(item["ChangeDiamond"]),
                        AfterDiamond = Convert.ToInt64(item["CurDiamond"]) + Convert.ToInt32(item["ChangeDiamond"]),
                        TypeName = EnumHelper.GetDesc((RecordDiamondType)item["TypeID"])
                    };
                    list.Add(stream);
                }
            }

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("list", list);
        }


        private void DiamondExchGoldConfig()
        {
            IList<CurrencyExchConfig> Configlist = FacadeManage.aideTreasureFacade.GetDiamondExchangeGoldConfig();
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("list", Configlist);
        }
        /// <summary>
        /// 钻石兑换金币
        /// </summary>
        /// <param name="configid"></param>
        /// <param name="typeid"></param>
        private void DiamondExchGold(int configid, int typeid)
        {
            Message msg =
                FacadeManage.aideTreasureFacade.DiamondExchangeGold(_userid, configid, typeid, GameRequest.GetUserIP());
            if (msg.Success)
            {
                var dataSet = msg.EntityList[0] as DataSet;
                if (dataSet != null)
                {
                    DiamondExchRecord record =
                        DataHelper.ConvertRowToObject<DiamondExchRecord>(dataSet.Tables[0].Rows[0]);
                    if (record == null) return;
                    _ajv.SetValidDataValue(true);
                    _ajv.SetDataItem("afterDiamond", record.AfterDiamond);
                    _ajv.SetDataItem("afterInsure", record.AfterInsureScore);
                    _ajv.SetDataItem("afterScore", record.AfterScore);
                    _ajv.SetDataItem("exchDiamond", record.ExchDiamond);
                    _ajv.SetDataItem("presentGold", record.PresentGold);
                }
            }
            _ajv.msg = msg.Content;
        }

        /// <summary>
        /// 充值通用查询接口
        /// </summary>
        /// <param name="orderid"></param>
        private void GetPayOrderStatus(string orderid)
        {
            var olOrder = FacadeManage.aideTreasureFacade.GetPayOnLinePayOrder(orderid);

            if (olOrder == null || olOrder.OrderStatus == 0)
            {
                _ajv.SetDataItem("OrderID", orderid);
                if (olOrder != null) {
                    _ajv.SetDataItem("PayStatus", olOrder.OrderStatus);
                    _ajv.SetDataItem("PayStatusDesc", "未支付");
                } else {
                    _ajv.SetDataItem("PayStatusDesc", "订单不存在");
                }
            }
            else
            {
                var userwealth = FacadeManage.aideTreasureFacade.GetUserWealth(olOrder.UserID);
                _ajv.SetDataItem("OrderID", orderid);
                _ajv.SetDataItem("PayStatusDesc", "已支付");
                _ajv.SetDataItem("PayStatus", olOrder.OrderStatus);
                _ajv.SetDataItem("PayAmount", olOrder.Amount);
                _ajv.SetDataItem("ScoreType", olOrder.ScoreType);
                _ajv.SetDataItem("Score", olOrder.Score);
                _ajv.SetDataItem("CurScore", userwealth.Score);
                _ajv.SetDataItem("CurDiomand", userwealth.Diamond);
            }

            _ajv.SetValidDataValue(true);
        }

        /// <summary>
        /// 常见问题接口
        /// </summary>
        private void GetQuestionAndAnswerList()
        {
            IList<Question> list = FacadeManage.aideNativeWebFacade.GetQAList();
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("list", list);
        }

        /// <summary>
        /// 代理同步登录接口
        /// </summary>
        private void AgentSyncLogin()
        {
            AccountsInfo aai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(_userid);
            if (aai?.AgentID > 0)
            {
                string clientParams = Fetch.DESEncrypt($"<>,<{aai.UserUin}>,<{aai.NickName}>,<>,<>",
                    AppConfig.WxUrlKey);
                _ajv.SetDataItem("link",
                    string.IsNullOrEmpty(Fetch.GetWebSiteConfig().Field5)
                        ? $"/Card/?w={clientParams}"
                        : Fetch.GetWebSiteConfig().Field5 + $"/?w={clientParams}");
                _ajv.SetValidDataValue(true);
            }
            else
            {
                _ajv.code = 2003;
                _ajv.msg = "抱歉，您的账号不是代理账号";
            }
        }

        /// <summary>
        /// 获取大厅战绩
        /// </summary>
        /// <param name="typeId"></param>
        private void GetBattleRecord(int typeId)
        {
            List<HallBattleInfo> bList = null;
            List<HalllScoreInfo> sList = null;

            DataSet ds = FacadeManage.aidePlatformFacade.GetHallBattleRecord(_userid, typeId);
            if (ds != null && ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
            {
                if (typeId >= 2)
                {
                    sList = new List<HalllScoreInfo>();
                    DataTable dscore = ds.Tables[0];
                    DataTable draw = ds.Tables[1];
                    DataTable game = ds.Tables[2];
                    HalllScoreInfo info = null;
                    foreach (DataRow item in dscore.Rows)
                    {
                        info = new HalllScoreInfo();
                        info.GameTime = Convert.ToDateTime(item["InsertTime"]);
                        DataRow[] kinds = game.Select("KindID=" + item["KindID"]);
                        info.KindID = Convert.ToInt32(item["KindID"]);
                        info.KindName = kinds != null && kinds.Length > 0 ? kinds[0]["KindName"].ToString() : "";
                        info.Score = Convert.ToInt32(item["Score"]);
                        DataRow[] draws = draw.Select("DrawID=" + item["DrawID"]);
                        info.UserCount = draws != null && draws.Length > 0 ? Convert.ToInt32(draws[0]["UserCount"]) : 0;
                        sList.Add(info);
                    }
                }
                else
                {
                    bList = new List<HallBattleInfo>();
                    DataTable battleinfo = ds.Tables[0];
                    DataTable game = ds.Tables[1];
                    DataTable user = ds.Tables[2];
                    DataTable battle = ds.Tables[4];
                    DataTable host = ds.Tables[3];

                    if (battle != null && battle.Rows.Count > 0)
                    {
                        //获取战绩
                        HallBattleInfo hall = null;
                        foreach (DataRow item in battle.Rows)
                        {
                            hall = new HallBattleInfo();
                            DataRow[] kinds = game.Select("KindID=" + item["KindID"]);
                            hall.KindID = Convert.ToInt32(item["KindID"]);
                            hall.RoomHostID = Convert.ToInt32(item["RoomHostID"]);
                            hall.KindName = kinds != null && kinds.Length > 0 ? kinds[0]["KindName"].ToString() : "";
                            hall.BaseScore = Convert.ToInt32(item["CellScore"]);
                            hall.RoomID = Convert.ToInt32(item["RoomID"]);
                            hall.Score = Convert.ToInt32(item["Score"]);
                            hall.GUID = item["PersonalRoomGUID"].ToString();
                            hall.StartTime = Convert.ToDateTime(item["StartTime"]);
                            DataRow[] rinfo = item["RoomHostID"] == null || item["RoomHostID"].ToString() == "0" ? null : host.Select("UserID=" + item["RoomHostID"]);
                            hall.GameID = rinfo != null && rinfo.Length > 0 ? Convert.ToInt32(rinfo[0]["GameID"]) : 0;
                            hall.NickName = rinfo != null && rinfo.Length > 0 ? rinfo[0]["NickName"].ToString() : "";
                            hall.PlayBackCode = Convert.ToInt32(item["PlayBackCode"]);
                            hall.ChairID = Convert.ToInt32(item["ChairID"]);

                            //获取局数
                            DataRow[] drow = battleinfo.Select("RoomID=" + item["RoomID"]);
                            List<int> gamesnum = new List<int>();
                            foreach (DataRow c in drow)
                            {
                                if (!gamesnum.Contains(Convert.ToInt32(c["GamesNum"])))
                                {
                                    gamesnum.Add(Convert.ToInt32(c["GamesNum"]));
                                }
                            }

                            List<HallGamesNum> list = new List<HallGamesNum>();
                            HallGamesNum gn = null;

                            foreach (int g in gamesnum)
                            {
                                List<HalllBattleUser> BattleUser = new List<HalllBattleUser>();
                                gn = new HallGamesNum();
                                HalllBattleUser bu = null;
                                DataRow[] rows = battleinfo.Select("GamesNum=" + g);
                                foreach (DataRow u in rows)
                                {
                                    if (u["RoomID"].ToString() == item["RoomID"].ToString())
                                    {
                                        bu = new HalllBattleUser();
                                        DataRow[] uinfo = user.Select("UserID=" + u["UserID"]);
                                        bu.GameID = uinfo != null && uinfo.Length > 0 ? Convert.ToInt32(uinfo[0]["GameID"]) : 0;
                                        bu.NickName = uinfo != null && uinfo.Length > 0 ? uinfo[0]["NickName"].ToString() : "";
                                        bu.Score = Convert.ToInt32(u["Score"]);
                                        bu.VideoNumber = u["ID"].ToString();
                                        BattleUser.Add(bu);
                                    }
                                }
                                gn.GamesNum = g;
                                gn.BattleUser = BattleUser;
                                list.Add(gn);
                            }
                            hall.GamesNum = list;
                            bList.Add(hall);
                        }
                    }
                }
            }
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("bList", bList);
            _ajv.SetDataItem("sList", sList);
        }

        /// <summary>
        /// 获取俱乐部战绩
        /// </summary>
        private void GetGroupBattleRecord()
        {
            int groupid = GameRequest.GetQueryInt("groupid", 0);
            //int typeid = GameRequest.GetQueryInt("typeid", 0);

            List<ClubBattleInfo> bList = null;

            DateTime start = DateTime.Now.AddHours(-48);
            DateTime end = DateTime.Now;

            DataSet ds = FacadeManage.aidePlatformFacade.GetClubBattleRecord(_userid, groupid, start, end);
            if (ds != null && ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
            {
                bList = new List<ClubBattleInfo>();
                DataTable battleinfo = ds.Tables[0];
                DataTable game = ds.Tables[1];
                DataTable user = ds.Tables[2];
                DataTable battle = ds.Tables[4];
                DataTable host = ds.Tables[3];

                if (battle != null && battle.Rows.Count > 0)
                {
                    //获取战绩
                    ClubBattleInfo hall = null;
                    foreach (DataRow item in battle.Rows)
                    {
                        hall = new ClubBattleInfo();
                        DataRow[] kinds = game.Select("KindID=" + item["KindID"]);
                        hall.KindID = Convert.ToInt32(item["KindID"]);
                        hall.RoomHostID = Convert.ToInt32(item["RoomHostID"]);
                        hall.KindName = kinds != null && kinds.Length > 0 ? kinds[0]["KindName"].ToString() : "";
                        hall.BaseScore = Convert.ToInt32(item["CellScore"]);
                        hall.RoomID = Convert.ToInt32(item["RoomID"]);
                        hall.Score = Convert.ToInt32(item["Score"]);
                        hall.GUID = item["PersonalRoomGUID"].ToString();
                        hall.StartTime = Convert.ToDateTime(item["StartTime"]);
                        DataRow[] rinfo = item["RoomHostID"] == null || item["RoomHostID"].ToString() == "0" ? null : host.Select("UserID=" + item["RoomHostID"]);
                        hall.GameID = rinfo != null && rinfo.Length > 0 ? Convert.ToInt32(rinfo[0]["GameID"]) : 0;
                        hall.NickName = rinfo != null && rinfo.Length > 0 ? rinfo[0]["NickName"].ToString() : "";
                        hall.PlayBackCode = Convert.ToInt32(item["PlayBackCode"]);
                        hall.PlayMode = Convert.ToInt32(item["PlayMode"]);
                        hall.ChairID = Convert.ToInt32(item["ChairID"]);

                        //获取局数
                        DataRow[] drow = battleinfo.Select("RoomID=" + item["RoomID"]);
                        List<int> gamesnum = new List<int>();
                        foreach (DataRow c in drow)
                        {
                            if (!gamesnum.Contains(Convert.ToInt32(c["GamesNum"])))
                            {
                                gamesnum.Add(Convert.ToInt32(c["GamesNum"]));
                            }
                        }

                        List<ClubGamesNum> list = new List<ClubGamesNum>();
                        ClubGamesNum gn = null;

                        foreach (int g in gamesnum)
                        {
                            List<ClubBattleUser> BattleUser = new List<ClubBattleUser>();
                            gn = new ClubGamesNum();
                            ClubBattleUser bu = null;
                            DataRow[] rows = battleinfo.Select("GamesNum=" + g);
                            foreach (DataRow u in rows)
                            {
                                if (u["RoomID"].ToString() == item["RoomID"].ToString())
                                {
                                    bu = new ClubBattleUser();
                                    DataRow[] uinfo = user.Select("UserID=" + u["UserID"]);
                                    bu.GameID = uinfo != null && uinfo.Length > 0 ? Convert.ToInt32(uinfo[0]["GameID"]) : 0;
                                    bu.NickName = uinfo != null && uinfo.Length > 0 ? uinfo[0]["NickName"].ToString() : "";
                                    bu.Score = Convert.ToInt32(u["Score"]);
                                    bu.VideoNumber = u["ID"].ToString();
                                    BattleUser.Add(bu);
                                }
                            }
                            gn.GamesNum = g;
                            gn.BattleUser = BattleUser;
                            list.Add(gn);
                        }
                        hall.GamesNum = list;
                        bList.Add(hall);
                    }
                }
            }
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("bList", bList);
        }


        /// <summary> 
        /// 获取在线充值微信账号列表
        /// </summary>
        private void GetOnLineWeChatList()
        {
            IList<OnLineWeChat> list = FacadeManage.aideTreasureFacade.GetOnLineWeChatList();
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("List", list);
        }
        /// <summary>
        /// 获取验证码
        /// </summary>
        /// <param name="context"></param>
        protected void GetCode(HttpContext context)
        {
            bool result = true;
            string Mobile = GameRequest.GetQueryString("Mobile");
            int userid = GameRequest.GetQueryInt("userid", 0);//用户标识
            int type = GameRequest.GetQueryInt("type", 0);//1注册2修改


            //电话号码非空验证
            if (string.IsNullOrEmpty(Mobile))
            {
                _ajv.SetValidDataValue(false);
                _ajv.msg = "电话号码不能为空";
                return;
            }
            //验证码时间限制
            int CheckCodeTime =
                FacadeManage.aideAccountsFacade.GetSystemStatusInfo("CheckCodeValidTime")?.StatusValue ?? 2;
            if (CheckCodeTime > 0)
            {
                result = FacadeManage.aideAccountsFacade.VaildSendOnTime(Mobile, CheckCodeTime);
            }

            if (!result)
            {
                _ajv.SetValidDataValue(false);
                _ajv.msg = "已完成发送，请稍后重试";
                return;
            }

            int rs = 0;
            if (userid == 0 || type > 0)
            {
                int MolibleNum = FacadeManage.aideAccountsFacade.GetSystemStatusInfo("MolibleNum")?.StatusValue ?? 3;
                int lognum = Convert.ToInt32(FacadeManage.aideAccountsFacade.GetSMSLogCount(Mobile));

                AccountsInfo info = FacadeManage.aideAccountsFacade.GetAccountsInfoByAccounts(Mobile);
                if (type == 1 && info != null)
                {

                    rs = -2;//账号已注册
                    _ajv.SetValidDataValue(true);
                    _ajv.SetDataItem("rs", rs);
                    return;
                }
                if (MolibleNum > 0 && lognum >= MolibleNum)
                {
                    rs = -1;//已超过获取验证码限制次数
                    _ajv.SetValidDataValue(true);
                    _ajv.SetDataItem("rs", rs);
                    return;
                }


                if (type == 2 && info == null)
                {
                    rs = -3;//账号未注册
                    _ajv.SetValidDataValue(true);
                    _ajv.SetDataItem("rs", rs);
                    return;
                }
            }
            string code = Facade.GetMobileCode.MyGetCode(Mobile);
            if (!string.IsNullOrEmpty(code))
            {
                rs = FacadeManage.aideAccountsFacade.InsertSMSInfo(userid, Mobile, code);

            }
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("rs", rs);
        }
        /// <summary>
        /// 绑定手机
        /// </summary>
        /// <param name="context"></param>
        protected void BindMobile(HttpContext context)
        {

            string code = GameRequest.GetQueryString("code");
            string Mobile = GameRequest.GetQueryString("Mobile");
            string userid = GameRequest.GetQueryString("userid");
            string time = ApplicationSettings.Get("SMS_time");
            int rs = 0;
            Message msg = new Message();
            //验证手机号

            msg = CheckingMobilePhoneNumFormat(Mobile, false);
            if (!msg.Success)
            {
                _ajv.msg = "请输入正确的手机号码!";
                return;
            }
            if (string.IsNullOrEmpty(code) || code.Length < 6)
            {
                _ajv.msg = "请填写正确的验证码!";
                return;
            }

            if (!string.IsNullOrEmpty(code) && !string.IsNullOrEmpty(Mobile) && !string.IsNullOrEmpty(userid))
            {
                Message umsg = FacadeManage.aideAccountsFacade.BindMobile(Convert.ToInt32(userid), Mobile, code, Convert.ToInt32(time));
                rs = Convert.ToInt32(umsg.Content);
            }
            if (rs == -1)
            {
                _ajv.msg = "验证码错误!";
            }
            else if (rs == -2)
            {
                _ajv.msg = "验证码过期!";
            }
            else if (rs == 1)
            {
                _ajv.msg = "绑定成功!";
            }

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("rs", rs);
        }
        /// <summary>
        /// 获取手机绑定验证配置
        /// </summary>
        /// <param name="context"></param>
        protected void BindMobileConfig(HttpContext context)
        {
            string userid = GameRequest.GetQueryString("userid");
            AccountsInfo info = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(Convert.ToInt32(userid));
            int isBind = 0;
            int BindMolibleReward = 0;
            if (!string.IsNullOrEmpty(info.RegisterMobile))
            {
                isBind = 1;
            }
            Entity.Accounts.SystemStatusInfo System = FacadeManage.aideAccountsFacade.GetSystemStatusInfo("BindMolibleReward");

            if (System != null)
            {
                BindMolibleReward = System.StatusValue;
            }


            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("isBind", isBind);
            _ajv.SetDataItem("BindMolibleReward", BindMolibleReward);
        }
        /// <summary>
        /// 每日分享
        /// </summary>
        /// <param name="context"></param>
        protected void ShareTimesReward(HttpContext context)
        {
            string userid = GameRequest.GetQueryString("userid");
            DataSet ds = FacadeManage.aideNativeWebFacade.GetTimesReward(Convert.ToInt32(userid), GameRequest.GetUserIP());
            DataTable dt = ds.Tables[0];
            int rs = -1;
            int TimeShareGold = 0;
            int TimeShareDiamond = 0;
            if (dt.Rows.Count > 0)
            {
                rs = Convert.ToInt32(dt.Rows[0]["rst"]);
                TimeShareGold = Convert.ToInt32(dt.Rows[0]["TimeShareGold"]);
                TimeShareDiamond = Convert.ToInt32(dt.Rows[0]["TimeShareDiamond"]);
            }
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("rs", rs);
            _ajv.SetDataItem("TimeShareGold", TimeShareGold);
            _ajv.SetDataItem("TimeShareDiamond", TimeShareDiamond);

        }

        protected void GetShareReward(HttpContext context)
        {
            DataSet ds = FacadeManage.aideNativeWebFacade.GetShareReward();
            List<ShareConfig> list = new List<ShareConfig>();
            if (ds.Tables[0].Rows.Count > 0)
            {
                DataTable dt = ds.Tables[0];
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    ShareConfig sc = new ShareConfig();
                    sc.DayShareLmt = Convert.ToInt32(dt.Rows[i]["DayShareLmt"].ToString());
                    sc.TimeShareGold = Convert.ToInt32(dt.Rows[i]["TimeShareGold"].ToString());
                    sc.TimeShareDiamond = Convert.ToInt32(dt.Rows[i]["TimeShareDiamond"].ToString());
                    sc.Nullity = Convert.ToInt32(dt.Rows[i]["Nullity"].ToString());
                    list.Add(sc);
                }
            }

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("list", list);

        }


        #region 代理接口模块
        /// <summary>
        /// 获取代理信息汇总
        /// </summary>
        protected void GetAgentInfo()
        {
            Entity.Agent.SystemStatusInfo diamondSave =
                FacadeManage.aideAgentFacade.GetSystemStatusInfo(AppConfig.AgentConfig.ReceiveDiamondSave.ToString());
            Entity.Agent.SystemStatusInfo goldSave =
                FacadeManage.aideAgentFacade.GetSystemStatusInfo(AppConfig.AgentConfig.ReceiveGoldSave.ToString());


            AccountsInfo userInfo = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(_userid);
            //AccountsFace face = FacadeManage.aideAccountsFacade.GetAccountsFace(userInfo.CustomID);
            Entity.Agent.AgentInfo agentInfo =
                FacadeManage.aideAgentFacade.GetAgentInfo(userInfo.AgentID, _userid);

            int ParentAgentGameID = 0;
            int ParerntAgentID = 0;
            Entity.Agent.AgentInfo parentAgent = FacadeManage.aideAgentFacade.GetParentAgentInfo(userInfo.UserID);
            if (parentAgent != null)
            {
                ParerntAgentID = parentAgent.AgentID;
                ParentAgentGameID = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(parentAgent.UserID).GameID;
            }

            if (agentInfo == null)
            {
                _ajv.SetValidDataValue(false);
                _ajv.SetDataItem("ParentAgentID", ParerntAgentID);
                _ajv.SetDataItem("ParentGameID", ParentAgentGameID);
                _ajv.code = -1;
                _ajv.msg = "该账号还不是代理";
                return;
            }

            Card.DataStruct.AgentInfo info = new Card.DataStruct.AgentInfo
            {
                //来源用户表
                UserID = userInfo.UserID,
                GameID = userInfo.GameID,
                AgentID = userInfo.AgentID,
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
                BelowAgent = FacadeManage.aideAgentFacade.GetBelowAgentsAgent(userInfo.AgentID),
                BelowUser = FacadeManage.aideAgentFacade.GetBelowAgentsUser(userInfo.AgentID),
                DiamondAward = agentInfo.DiamondAward,
                GoldAward = agentInfo.GoldAward,
                TotalDiamondAward = FacadeManage.aideAgentFacade.GetTotalDiamondAward(_userid),
                TotalGoldAward = FacadeManage.aideAgentFacade.GetTotalGoldAward(_userid),
                BelowAgentsUser = FacadeManage.aideAgentFacade.GetBelowAgentsAllUser(userInfo.AgentID),
                BelowAgentsAgent = FacadeManage.aideAgentFacade.GetBelowAgentsAllAgent(userInfo.AgentID) - 1,
                IsHasPassword = !agentInfo.Password.Equals(""),
                ParentAgentID = ParerntAgentID,
                ParentGameID = ParentAgentGameID
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
        protected void GetNickNameByGameID(int gameId)
        {
            if (gameId == 0)
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
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
        protected void GetRecord(string type)
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
                    where = $" WHERE TargetUserID = {_userid} AND AwardType  = 1 {sqlDate} {sqlUserId}";
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
                    where = $" WHERE TargetUserID = {_userid} AND AwardType  = 2 {sqlDate} {sqlUserId}";
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
        protected void GetBelowList(string type, string query)
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
            string sqlWhere = $"WHERE AgentID IN (SELECT AgentID FROM AgentInfo(NOLOCK) WHERE UserID={_userid})";
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
                        TotalDiamond = FacadeManage.aideAgentFacade.GetTotalDiamondAward(_userid, ai.UserID),
                        TotalGold = FacadeManage.aideAgentFacade.GetTotalGoldAward(_userid, ai.UserID)
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
        protected void GetAwardInfo()
        {
            Dictionary<string, long> dicGoldDetail = new Dictionary<string, long>
            {
                {"TotalAward", FacadeManage.aideAgentFacade.GetTotalGoldAward(_userid)},
                {"MonthAward", FacadeManage.aideAgentFacade.GetTotalGoldAward(_userid, Fetch.GetMonthTime())},
                {"LastMonthAward", FacadeManage.aideAgentFacade.GetTotalGoldAward(_userid, Fetch.GetLastMonthTime())},
                {"TotalReceive", FacadeManage.aideAgentFacade.GetTotalGoldReceive(_userid)}
            };
            Dictionary<string, long> dicDiamondDetail = new Dictionary<string, long>
            {
                {"TotalAward", FacadeManage.aideAgentFacade.GetTotalDiamondAward(_userid)},
                {"MonthAward", FacadeManage.aideAgentFacade.GetTotalDiamondAward(_userid, Fetch.GetMonthTime())},
                {"LastMonthAward", FacadeManage.aideAgentFacade.GetTotalDiamondAward(_userid, Fetch.GetLastMonthTime())},
                {"TotalReceive", FacadeManage.aideAgentFacade.GetTotalDiamondReceive(_userid)}
            };
            Dictionary<string, object> dicAwardInfo =
                new Dictionary<string, object> { { "Gold", dicGoldDetail }, { "Diamond", dicDiamondDetail } };
            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("info", dicAwardInfo);
        }


        /// <summary>
        /// 提取钻石返利或金币返利
        /// </summary>
        /// <param name="gameId"></param>
        /// <param name="pass"></param>
        /// <param name="count"></param>
        /// <param name="type"></param>
        protected void TakeScoreORDiamond(int gameId, string pass, int count, byte type)
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
                FacadeManage.aideAgentFacade.ReceiveAgentAward(_userid, type, count);
            if (msg.Success && gameId > 0) //当填写了别人的GameID为转赠功能
            {
                //代理转赠返利
                msg = FacadeManage.aideAgentFacade.GiveAgentAward(_userid, gameId, type, count, pass);
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
        protected void PresentDiamondOrScore(int gameId, string pass, int count, byte type)
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
            Message msg = FacadeManage.aideAgentFacade.GiveAgentAward(_userid, gameId, type, count, pass);

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
        protected void SetSafePass(string pass)
        {
            AccountsInfo userInfo = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(_userid);
            Entity.Agent.AgentInfo agentInfo =
               FacadeManage.aideAgentFacade.GetAgentInfo(userInfo.AgentID, _userid);

            if (agentInfo == null)
            {
                _ajv.msg = "该玩家不是代理";
                return;

            }

            if (string.IsNullOrEmpty(pass) || pass.Length != 32)
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " password 参数不正确或未加密");
                return;
            }
            int spResult = FacadeManage.aideAgentFacade.SetAgentSafePassword(agentInfo.AgentID, pass);
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
        protected void UpdateSafePass(string oldPass, string newPass)
        {

            AccountsInfo userInfo = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(_userid);
            Entity.Agent.AgentInfo agentInfo =
               FacadeManage.aideAgentFacade.GetAgentInfo(userInfo.AgentID, _userid);

            if (!IsPassChecked(oldPass))
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " oldPassword 不正确");
                return;
            }
            if (string.IsNullOrEmpty(newPass) || newPass.Length != 32)
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
                _ajv.msg = string.Format(EnumHelper.GetDesc(ApiCode.VertyParamErrorCode),
                    " newPassword 参数不正确或未加密");
                return;
            }
            int upResult = FacadeManage.aideAgentFacade.SetAgentSafePassword(agentInfo.AgentID, newPass);
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
        protected void UpdateAgentInfo(string address, string phone, string qq)
        {
            if (string.IsNullOrEmpty(address) || string.IsNullOrEmpty(phone) ||
                string.IsNullOrEmpty(qq))
            {
                _ajv.code = (int)ApiCode.VertyParamErrorCode;
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
                UserID = _userid,
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
        protected void AddBelowAgent(int gameId, string agentDomain, string compllation,
           string address, string phone, string note)
        {
            if (gameId <= 0)
            {
                _ajv.msg = "抱歉，添加代理游戏ID不能为空";
                return;
            }
            //if (string.IsNullOrEmpty(compllation))
            //{
            //    _ajv.msg = "抱歉，真实姓名不能为空";
            //    return;
            //}
            //if (string.IsNullOrEmpty(phone))
            //{
            //    _ajv.msg = "抱歉，联系电话不能为空";
            //    return;
            //}
            if (!string.IsNullOrEmpty(phone) && !Validate.IsMobileCode(phone))
            {
                _ajv.msg = "抱歉，联系电话格式不正确";
                return;
            }
            //if (string.IsNullOrEmpty(agentDomain))
            //{
            //    _ajv.msg = "抱歉，代理域名不能为空";
            //    return;
            //}

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

            Message msg = FacadeManage.aideAgentFacade.AddAgent(_userid, info, gameId);
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

        protected void BindAgent()
        {
            int gameId = GameRequest.GetInt("gameid", 0);
            if (gameId == 0)
            {
                _ajv.SetValidDataValue(false);
                _ajv.code = -1;
                _ajv.msg = "请输入要绑定代理的游戏ID";
                return;
            }

            AccountsInfo info = FacadeManage.aideAccountsFacade.GetAccountsInfoByGameID(gameId);
            if (info == null)
            {
                _ajv.SetValidDataValue(false);
                _ajv.code = -1;
                _ajv.msg = "请输入要绑定代理不存在";
                return;
            }

            Message msg = FacadeManage.aideAgentFacade.UserAgentBind(_userid, info.GameID, GameRequest.GetUserIP());
            if (msg.Success)
            {
                _ajv.SetValidDataValue(true);
                _ajv.msg = "上级代理绑定成功";
            }
            else
            {
                _ajv.msg = msg.Content;
            }
        }

        /// <summary>
        /// 获取返利配置信息列表
        /// </summary>
        /// <param name="context"></param>
        protected void GetReturnAwardConfig(HttpContext context)
        {
            DataSet ds = FacadeManage.aideAgentFacade.GetReturnAwardConfig();
            List<ReturnAwardConfig> list = new List<ReturnAwardConfig>();
            if (ds.Tables[0].Rows.Count > 0)
            {
                DataTable dt = ds.Tables[0];
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    ReturnAwardConfig rac = new ReturnAwardConfig();
                    rac.ConfigID = Convert.ToInt32(dt.Rows[i]["ConfigID"].ToString());
                    rac.AwardLevel = Convert.ToInt32(dt.Rows[i]["AwardLevel"].ToString());
                    rac.AwardType = Convert.ToByte(dt.Rows[i]["AwardType"].ToString());
                    rac.AwardScale = Convert.ToDecimal(dt.Rows[i]["AwardScale"].ToString());
                    rac.Nullity = Convert.ToBoolean(dt.Rows[i]["Nullity"].ToString());
                    list.Add(rac);
                }
            }

            _ajv.SetValidDataValue(true);
            _ajv.SetDataItem("list", list);

        }

        /// <summary>
        /// 获取我的提取赠送列表
        /// </summary>
        protected void GetMyTakeGrantRecord()
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

            where = $" WHERE TargetUserID = {_userid} OR SourceUserID = {_userid} AND TradeType  = 2 {sqlDate} {sqlUserId}";
            ps = FacadeManage.aideAgentFacade.GetList(ReturnAwardGrant.Tablename, page, number, where,
                $"ORDER BY {ReturnAwardGrant._CollectDate} DESC");
            IList<ReturnAwardGrant> GrantList = new List<ReturnAwardGrant>();
            if (ps?.RecordCount > 0)
            {
                foreach (DataRow dr in ps.PageSet.Tables[0].Rows)
                {
                    int userId = Convert.ToInt32(dr["SourceUserID"].ToString());
                    AccountsInfo ai = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(userId);
                    GrantList.Add(new ReturnAwardGrant
                    {
                        RecordID = Convert.ToInt32(dr["RecordID"]),
                        SourceUserID = Convert.ToInt32(dr["SourceUserID"]),
                        TargetUserID = Convert.ToInt32(dr["TargetUserID"]),
                        TradeType = Convert.ToByte(dr["TradeType"]),
                        SourceBefore = Convert.ToInt32(dr["SourceBefore"]),
                        TargetBefore = Convert.ToInt32(dr["TargetBefore"]),
                        Amount = Convert.ToInt32(dr["Amount"]),
                        CollectDate = Convert.ToDateTime(dr["CollectDate"])
                    });
                }
            }
            _ajv.SetDataItem("record", GrantList);
            _ajv.SetDataItem("pageCount", ps?.PageCount);
            _ajv.SetDataItem("recordCount", ps?.RecordCount);
            _ajv.SetValidDataValue(true);
        }
        #endregion



        #region 辅助方法

        /// <summary>
        /// 获取手机端配置
        /// </summary>
        private MobileSystemConfig GetMobileSystemConfig(DataTable table)
        {
            MobileSystemConfig config = new MobileSystemConfig();
            if (table == null || table.Rows.Count <= 0) return config;
            foreach (DataRow item in table.Rows)
            {
                // ReSharper disable once SwitchStatementMissingSomeCases
                switch (item["StatusName"].ToString())
                {
                    case "JJOpenMobileMall":
                        config.IsOpenMall = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "JJPayBindSpread":
                        config.IsPayBindSpread = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "JJBindSpreadPresent":
                        config.BindSpreadPresent = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "JJRankingListType":
                        config.RankingListType = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "JJPayChannel":
                        config.PayChannel = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "JJDiamondBuyProp":
                        config.DiamondBuyPropCount = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "JJRealNameAuthentPresent":
                        config.RealNameAuthentPresent = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "JJEffectiveFriendGame":
                        config.EffectiveFriendGame = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "IOSNotStorePaySwitch":
                        config.IOSNotStorePaySwitch = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "JJGoldBuyProp":
                        config.GoldBuyPropCount = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "EnjoinInsure":
                        config.EnjoinInsure = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "TransferStauts":
                        config.TransferStauts = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "MobileBattleRecordMask":
                        config.MobileBattleRecordMask = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "OpenGameSignIn":
                        config.IsOpenGameSignIn = Convert.ToInt32(item["StatusValue"]);
                        break;
                    case "IsRedemptionCenter":
                        config.IsOpenRedemptionCenter = Convert.ToInt32(item["StatusValue"]);
                        break;
                }
            }
            return config;
        }

        /// <summary>
        /// 获取亲友圈配置
        /// </summary>
        private GroupSystemConfig GetGroupConfig(DataTable table)
        {
            GroupSystemConfig config = new GroupSystemConfig();
            if (table == null || table.Rows.Count <= 0) return config;
            foreach (DataRow item in table.Rows)
            {
                // ReSharper disable once SwitchStatementMissingSomeCases
                switch (item["OptionName"].ToString())
                {
                    case "MaxMemberCount":
                        config.MaxMemberCount = Convert.ToInt32(item["OptionValue"]);
                        break;
                    case "MaxCreateGroupCount":
                        config.MaxCreateGroupCount = Convert.ToInt32(item["OptionValue"]);
                        break;
                    case "CreateGroupTakeIngot":
                        config.CreateGroupTakeIngot = Convert.ToInt32(item["OptionValue"]);
                        break;
                    case "CreateGroupDeductIngot":
                        config.CreateGroupDeductIngot = Convert.ToInt32(item["OptionValue"]);
                        break;
                    case "MaxJoinGroupCount":
                        config.MaxJoinGroupCount = Convert.ToInt32(item["OptionValue"]);
                        break;
                    case "GroupPayType":
                        config.GroupPayType = Convert.ToInt32(item["OptionValue"]);
                        break;
                    case "GroupPayTypeChange":
                        config.GroupPayTypeChange = Convert.ToInt32(item["OptionValue"]);
                        break;
                    case "GroupRoomType":
                        config.GroupRoomType = Convert.ToInt32(item["OptionValue"]);
                        break;
                }
            }
            return config;
        }



        /// <summary>
        /// 获取推广链接
        /// </summary>
        private string GetSpreadLink(DataTable table, bool flag)
        {
            string shareLink = string.Empty;
            if (table != null && table.Rows.Count > 0)
            {
                DataRow row = table.Rows[0];
                //获取分享域名，没有则取当前域名
                ConfigInfo webConfig = Fetch.GetWebSiteConfig();
                string domain = string.IsNullOrEmpty(webConfig.Field4)
                    ? GameRequest.GetCurrentFullHost()
                    : webConfig.Field4;
                if (domain.IndexOf("http://") <= 0 || domain.IndexOf("https://")<=0)
                {
                    domain = "http://" + domain;
                }

                //线上版本
                if (flag)
                {
                    if (Convert.ToInt32(row["AgentID"]) > 0)
                    {
                        shareLink = "http://" + row["AgentDomain"] + "/Mobile/ShareLink.aspx";
                    }
                    else
                    {
                        string[] domainStr = domain.Split('.');
                        shareLink = domainStr.Length != 3
                            ? (domain + "/Mobile/ShareLink.aspx?g=" + row["GameID"])
                            : ("http://" + row["GameID"] + "." + domainStr[1] + "." + domainStr[2] +
                               "/Mobile/ShareLink.aspx");
                    }
                }
                else
                {
                    if (Convert.ToInt32(row["AgentID"]) > 0)
                    {
                        shareLink = domain + "/Mobile/AgentShareLinkLua.aspx?g=" + row["GameID"];
                    }
                    else
                    {
                        shareLink = domain + "/Mobile/ShareLinkNew.aspx?g=" + row["GameID"];
                    }

                }
            }
            return shareLink;
        }

        /// <summary>
        /// 获取U3D推广链接
        /// </summary>
        private string GetU3DSpreadLink(DataTable table)
        {
            string shareLink = string.Empty;
            if (table != null && table.Rows.Count > 0)
            {
                DataRow row = table.Rows[0];
	                //获取分享域名，没有则取当前域名
	                ConfigInfo webConfig = Fetch.GetWebSiteConfig();
	                string domain = string.IsNullOrEmpty(webConfig.Field4)
	                    ? GameRequest.GetCurrentFullHost()
	                    : webConfig.Field4;
	                if (domain.IndexOf("http://") <= 0 || domain.IndexOf("https://") <= 0)
	                {
	                    domain = "http://" + domain;
	                }

                if (Convert.ToInt32(row["AgentID"]) > 0)
                {
                    shareLink = domain + "/Mobile/AgentShareLink.aspx?g=" + row["GameID"] + "&y=u3d";
                }
                else
                {
                    shareLink = domain + "/Mobile/ShareLinkNew.aspx?g=" + row["GameID"] + "&y=u3d";
                }
            }
            return shareLink;
        }

        /// <summary>
        /// 获取微信预支付信息包
        /// </summary>
        /// <returns></returns>
        private string GetWxPayPackage(OnLinePayOrder orderReturn, string paytype, string openid,
            string authority)
        {
            string domain = string.IsNullOrEmpty(AppConfig.FrontSiteDomain) ? authority : AppConfig.FrontSiteDomain;
            WxPayInfo wx = new WxPayInfo
            {
                AppID = ApplicationSettings.Get(paytype == "wx" ? "WXAPPID" : "WXNATIVEAPPID"),
                AppSecret = ApplicationSettings.Get(paytype == "wx" ? "WXAPPSECRET" : "WXNATIVESECRET"),
                // ReSharper disable once SpecifyACultureInStringConversionExplicitly
                Body = orderReturn.Amount.ToString(),
                Key = ApplicationSettings.Get(paytype == "wx" ? "WXAPPKEY" : "WXNATIVEKEY"),
                Mchid = ApplicationSettings.Get(paytype == "wx" ? "WXAPPMCHID" : "WXNATIVEMCHID"),
                NotifyUrl = "http://" + domain + "/Notify/" + (paytype == "wx" ? "WxApp.aspx" : "WxWeb.aspx"),

                OpenId = openid,
                OutTradeNo = orderReturn.OrderID,
                TotalFee = (orderReturn.Amount * 100).ToString("F0"),
                TradeType = paytype == "wx" ? "APP" : "JSAPI"
            };
            WxPayHelper helper = new WxPayHelper(wx);
            return helper.GetPrepayIDSign();
        }
        /// <summary>
        /// 验证移动电话号码
        /// </summary>
        /// <param name="mobilePhoneNum"></param>
        /// <returns></returns>
        public Message CheckingMobilePhoneNumFormat(string mobilePhoneNum, bool isAllowNull)
        {
            Message msg = new Message();
            if (isAllowNull && string.IsNullOrEmpty(mobilePhoneNum))
            {
                return msg;
            }
            if (!isAllowNull && string.IsNullOrEmpty(mobilePhoneNum))
            {
                msg.Success = false;
                msg.Content = "电话号码不能为空";
                return msg;
            }

            Regex isMobile = new Regex(@"^\d{11}$", RegexOptions.Compiled);
            if (!isMobile.IsMatch(mobilePhoneNum))
            {
                msg.Success = false;
                msg.Content = "移动电话格式不正确";
                return msg;
            }
            return msg;
        }



        /// <summary>
        /// 验证安全密码是否正确
        /// </summary>
        /// <param name="pass"></param>
        /// <returns></returns>
        protected bool IsPassChecked(string pass)
        {
            AccountsInfo userInfo = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(_userid);
            Entity.Agent.AgentInfo agentInfo =
               FacadeManage.aideAgentFacade.GetAgentInfo(userInfo.AgentID, _userid);
            return pass.ToLower() == agentInfo.Password.ToLower();
        }
        #endregion



        public bool IsReusable => false;
    }
}
