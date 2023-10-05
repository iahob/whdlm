using Game.Entity.Accounts;
using Game.Entity.NativeWeb;
using Game.Entity.Platform;
using Game.Facade;
using Game.Utils;
using System;
using System.Collections.Generic;


namespace Game.Web.Mobile
{
    public partial class GroupShareLink : System.Web.UI.Page
    {
        //公用属性
        protected string Nickname = string.Empty;
        protected string PlatformDownloadUrl = string.Empty;
        protected string KindName = string.Empty;
        protected string KindIco = string.Empty;
        protected string Mobilelogo = string.Empty;
        protected bool Finish = true;

        //页面参数

        protected int Groupid = GameRequest.GetQueryInt("gr", 0);
        protected string code = GameRequest.GetQueryString("code");
        protected int Action = GameRequest.GetQueryInt("a", 0);
        protected string Type = GameRequest.GetQueryString("y");

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                if (Groupid <= 0 || string.IsNullOrEmpty(code) || Action <= 0)
                {
                    return;
                }


                //保存IP绑定邀请码信息
                string UserIP = GameRequest.GetUserIP();
                int result = FacadeManage.aideGroupFacade.GroupIPBind(UserIP, Groupid, Convert.ToInt64(code));


                int terminalType = Fetch.GetTerminalType(Page.Request);
                string imgDomain = string.Empty;
                IList<ConfigInfo> list = FacadeManage.aideNativeWebFacade.GetConfigInfoList();
                foreach (var item in list)
                {
                    if (item.ConfigKey == AppConfig.SiteConfigKey.MobilePlatformVersion.ToString())
                    {
                        if (Type == "u3d")
                        {
                            PlatformDownloadUrl = terminalType == 2 ? item.Field9 : item.Field10;
                        }
                        else
                        {
                            PlatformDownloadUrl = terminalType == 2 ? item.Field5 : item.Field6;
                        }

                    }
                    if (item.ConfigKey == AppConfig.SiteConfigKey.WebSiteConfig.ToString())
                    {
                        imgDomain = item.Field2;
                    }
                }
                Title = "亲友圈邀请 - " + AppConfig.PageTitle;
            }
        }
    }
}