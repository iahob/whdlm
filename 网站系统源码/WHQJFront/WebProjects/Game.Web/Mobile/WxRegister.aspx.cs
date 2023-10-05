using Game.Entity.Accounts;
using Game.Facade;
using Game.Utils;
using System;
using Game.Entity.NativeWeb;
using Game.Kernel;
using Game.Web.Helper;

namespace Game.Web.Mobile
{
    public partial class WxRegister : System.Web.UI.Page
    {
        //页面参数
        protected int Typeid = GameRequest.GetQueryInt("t", 0);

        protected int Gameid = GameRequest.GetQueryInt("g", 0);
        protected int Roomid = GameRequest.GetQueryInt("r", 0);
        protected int Groupid = GameRequest.GetQueryInt("gr", 0);
        protected int Kindid = GameRequest.GetQueryInt("k", 0);
        protected int Action = GameRequest.GetQueryInt("a", 0);
        protected string Password = GameRequest.GetQueryString("p");
        protected string Wxparam = GameRequest.GetQueryString("w");
        protected string PlatformType = GameRequest.GetQueryString("y");

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            if (IsPostBack) return;
            if ((Typeid != 81 && Typeid != 82 && Typeid != 83&& Typeid != 84) || Gameid <= 0)
            {
                Response.Redirect("/404.html");
                return;
            }


            string userAgent = Request.UserAgent;
            bool IsWeiXin = false;
            if (userAgent.ToLower().Contains("micromessenger"))
            {
                IsWeiXin = true;
            }
            else
            {
                IsWeiXin = false;
            }

            //获取公众号配置
            string appid = AppConfig.GetSetting("WXNATIVEAPPID");


            if (string.IsNullOrEmpty(Wxparam) && IsWeiXin && !string.IsNullOrEmpty(appid))
            {
                string path = Typeid == 82
                    ? $"/Mobile/WxRegister.aspx?t=82&g={Gameid}&gr={Groupid}&r={Roomid}&k={Kindid}&a={Action}&p={Password}"
                    : "/Mobile/WxRegister.aspx?t=81&g=" + Gameid;
                if (Typeid == 83)
                {
                    path = "/Mobile/WxRegister.aspx?t=83&g=" + Gameid;
                }
                else if (Typeid == 84)
                {
                    path = "/Mobile/WxRegister.aspx?t=84&g=" + Gameid;
                }

                ConfigInfo webConfig = Fetch.GetWebSiteConfig();
                string domain = string.IsNullOrEmpty(webConfig.Field4)
                    ? GameRequest.GetCurrentFullHost()
                    : webConfig.Field4;
                if (domain.IndexOf("http://") <= 0 || domain.IndexOf("https://") <= 0)
                {
                    domain = "http://" + domain;
                }

                string param = $"{domain}{path}";

                if (!string.IsNullOrEmpty(PlatformType))
                {
                    param += $"&y={PlatformType}";
                    //                    TextLogger.Write($"HAuthorizeUrl：{AppConfig.HAuthorizeURL}?url={Server.UrlEncode(param)}");
                    //                    Response.Redirect($"{AppConfig.HAuthorizeURL}?url={Server.UrlEncode(param)}");
                    //                    return;
                }
                Response.Redirect($"{AppConfig.AuthorizeURL}?url={Server.UrlEncode(param)}");
            }
            else if(!string.IsNullOrEmpty(Wxparam) && IsWeiXin && !string.IsNullOrEmpty(appid))
            {
                WxUser wu = Fetch.GetWxUser(Wxparam);
                if (wu == null)
                {
                    Log4Net.WriteInfoLog("获取不到微信信息，Gameid" + Gameid);
                    Response.Redirect("/404.html");
                    return;
                }
                AccountsInfo info = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserUin(wu.unionid);
                if (info == null)
                {
                    //注册帐号
                    UserInfo user = new UserInfo
                    {
                        NickName = wu.nickname,
                        Gender = wu.sex,
                        RegisterIP = GameRequest.GetUserIP(),
                        GameID = Gameid,
                        UserUin = wu.unionid
                    };
                    FacadeManage.aideAccountsFacade.RegisterWX(user, Typeid, wu.headimgurl);
                }
                else if (info.SpreaderID == 0 && info.AgentID == 0)
                {
                    AccountsInfo infos = FacadeManage.aideAccountsFacade.GetAccountsInfoByGameID(Gameid);
                    if (infos != null && infos.AgentID > 0)
                    {
                        Message mesg = new Message();
                        mesg = FacadeManage.aideAgentFacade.UserAgentBind(info.UserID, infos.GameID, GameRequest.GetUserIP());
                    }
                }


                string redirect;
                if (PlatformType == "h5")
                {
                    string wxParam = $"<{wu.openid}>,<{wu.unionid}>,<{wu.nickname}>,<{wu.sex}>,<{wu.headimgurl}>";
                    ConfigInfo webCfg = Fetch.GetWebSiteConfig();
                    string w = Server.UrlEncode(Fetch.AESEncrypt(wxParam, AppConfig.WxH5Key,
                        AppConfig.WxH5Key));
                    string baseUrl = "/h5/";
                    if (Typeid == 82)
                    {
                        switch (Kindid)
                        {
                            case 200:
                                baseUrl += "land";
                                break;
                            case 50:
                                baseUrl += "oxsixx";
                                break;

                            case 122:
                                baseUrl += "baccarat";
                                break;
                            case 33:
                                baseUrl += "sangong";
                                break;
                            case 391:
                                baseUrl += "sparrowgdex";
                                break;
                            case 51:
                                baseUrl += "thirteenzj";
                                break;
                            case 6:
                                baseUrl += "zhajinhua";
                                break;
                            case 302:
                                baseUrl += "bloodWar";
                                break;
                            case 390:
                                baseUrl += "sparrowhz";
                                break;
                            case 57:
                                baseUrl += "oxeight";
                                break;
                            case 56:
                                baseUrl += "fksanzhang";
                                break;
                            case 14:
                                baseUrl += "paiGow";
                                break;
                            case 242:
                                baseUrl += "pushBobbin";
                                break;
                            case 389:
                                baseUrl += "sparrowhz";
                                break;
                            default:
                                baseUrl += "hall";
                                break;
                        }
                    }
                    else
                    {
                        baseUrl += "hall";
                    }
                    redirect = baseUrl + (Typeid == 82
                                   ? $"/?w={w}&r={Roomid}&a={Action}&p={Password}"
                                   : $"/?w={w}");
                }
                else
                {
                    redirect = Typeid == 82
                        ? "/Mobile/RoomLink.aspx?" +
                          $"s=already&g={Gameid}&r={Roomid}&gr={Groupid}&k={Kindid}&a={Action}&p={Password}&y={PlatformType}"
                        : $"/Mobile/ShareLinkNew.aspx?s=already&g={Gameid}&y={PlatformType}";

                    if (Typeid == 83)
                    {
                        redirect = $"/Mobile/AgentShareLink.aspx?s=already&g={Gameid}&y={PlatformType}";
                    }
                    else if (Typeid == 84)
                    {
                        redirect = $"/Mobile/AgentShareLinkLua.aspx?s=already&g={Gameid}";
                    }
                }
                Response.Redirect(redirect);
            }
            else
            {
                //保存IP绑定信息
                string UserIP = GameRequest.GetUserIP();
                int result = FacadeManage.aideAccountsFacade.UserIPBind(UserIP, Gameid);

                string redirectUrl = "";
                redirectUrl = Typeid == 82
                    ? "/Mobile/RoomLink.aspx?" +
                      $"s=already&g={Gameid}&r={Roomid}&gr={Groupid}&k={Kindid}&a={Action}&p={Password}&y={PlatformType}"
                    : $"/Mobile/ShareLinkNew.aspx?s=already&g={Gameid}&y={PlatformType}";

                if (Typeid == 83)
                {
                    redirectUrl = $"/Mobile/AgentShareLink.aspx?s=already&g={Gameid}";
                }
                else if (Typeid == 84)
                {
                    redirectUrl = $"/Mobile/AgentShareLinkLua.aspx?s=already&g={Gameid}";
                }
                Response.Redirect(redirectUrl);
            }
        }
    }
}
