using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Game.Web.Mobile
{
    public partial class ShareLinkNew : System.Web.UI.Page
    {
        protected string MobileQrcode = string.Empty;
        protected string PlatformDownloadUrl = string.Empty;
        protected static string Plattype = string.Empty; //平台类型
        protected void Page_Load(object sender, EventArgs e)
        {
            if (IsPostBack) return;
            string type = GameRequest.GetQueryString("y");
            Plattype = type;

            string state = GameRequest.GetQueryString("s");
            if (state == "already")
            {
                int terminalType = Fetch.GetTerminalType(Request);
                int gameid = GameRequest.GetQueryInt("g", 0);
                if (gameid <= 0)
                {
                    return;
                }

                //保存IP绑定信息
                string UserIP = GameRequest.GetUserIP();
                int result = FacadeManage.aideAccountsFacade.UserIPBind(UserIP, gameid);

                ConfigInfo config = FacadeManage.aideNativeWebFacade.GetConfigInfo(AppConfig.SiteConfigKey.MobilePlatformVersion.ToString());
                if (config != null)
                {
                    PlatformDownloadUrl = terminalType == 2 ? config.Field5 : config.Field6;
                    if (type == "u3d")
                    {
                        PlatformDownloadUrl = terminalType == 2 ? config.Field9 : config.Field10;
                    }
                }
                Random random = new Random();
                MobileQrcode = Fetch.GetQrCode("http://" + Request.Url.Authority + "/Mobile/ShareLinkNew.aspx?r=" + random.Next(1000, 9999) + "&g=" + gameid+"&y="+ type, 312);

            }
            else
            {
                string domain = Request.Url.Authority;
                int gameid = FacadeManage.aideAgentFacade.GetGameIDByAgentDomain(domain);
                if (gameid <= 0)
                {
                    string[] domainStr = domain.Split('.');
                    if (domainStr.Length == 3 && Regex.IsMatch(domainStr[0], "^[0-9]*$"))
                    {
                        gameid = Convert.ToInt32(domainStr[0]);
                    }
                    else
                    {
                        gameid = GameRequest.GetQueryInt("g", 0);
                    }
                }
                Response.Redirect("/Mobile/WxRegister.aspx?t=81&g=" + gameid + (!string.IsNullOrEmpty(type) ? "&y=" + type : ""));
            }

        }
    }
}