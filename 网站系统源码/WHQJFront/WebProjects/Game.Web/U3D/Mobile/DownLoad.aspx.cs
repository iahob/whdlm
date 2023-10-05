using Game.Entity.NativeWeb;
using Game.Facade;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Game.Web.U3D.Mobile
{
    public partial class DownLoad : System.Web.UI.Page
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                string userAgent = Request.Headers["User-Agent"];
                if (userAgent.Contains("MicroMessenger"))
                {
                    Response.Redirect("/U3D/Mobile/index.aspx");
                }
                else
                {
                    int terminalType = Fetch.GetTerminalType(Page.Request);
                    ConfigInfo config = FacadeManage.aideNativeWebFacade.GetConfigInfo(AppConfig.SiteConfigKey.MobilePlatformVersion.ToString());
                    string PlatformDownloadUrl = terminalType == 2 ? config.Field9 : config.Field10;
                    Response.Redirect(PlatformDownloadUrl);
                }
            }
        }
    }
}