using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Utils;
using System;
using System.Text.RegularExpressions;

namespace Game.Web.Mobile
{
    public partial class AgentShareLink : System.Web.UI.Page
    {
        protected string MobileQrcode = string.Empty;
        protected string PlatformDownloadUrl = string.Empty;
        protected static string Plattype = string.Empty;
        protected void Page_Load(object sender, EventArgs e)
        {
            if (IsPostBack) return;
            string type = GameRequest.GetQueryString("y");
            string state = GameRequest.GetQueryString("s");
            Plattype = type;
            if (state == "already")
            {
                int gameid = GameRequest.GetQueryInt("g", 0);
                if (gameid <= 0)
                {
                    return;
                }

                //保存IP绑定信息
                string UserIP = GameRequest.GetUserIP();
                int result = FacadeManage.aideAccountsFacade.UserIPBind(UserIP, gameid);

                int terminalType = Fetch.GetTerminalType(Request);
                ConfigInfo config = FacadeManage.aideNativeWebFacade.GetConfigInfo(AppConfig.SiteConfigKey.MobilePlatformVersion.ToString());
                if (config != null)
                {
                    PlatformDownloadUrl = terminalType == 2 ? config.Field9 : config.Field10;
                }
                Random random = new Random();
                MobileQrcode = Fetch.GetQrCode("http://" + Request.Url.Authority + "/Mobile/AgentShareLink.aspx?r=" + random.Next(1000, 9999)+"&g="+ gameid, 312);
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
                Response.Redirect("/Mobile/WxRegister.aspx?t=83&g=" + gameid + (!string.IsNullOrEmpty(type) ? "&y=" + type : ""));
            }
           
        }
    }
}