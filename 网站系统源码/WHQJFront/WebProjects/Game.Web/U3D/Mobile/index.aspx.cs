using Game.Entity.NativeWeb;
using Game.Facade;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;

namespace Game.Web.U3D.Mobile
{
    public partial class index : System.Web.UI.Page
    {
        //公用属性
        protected string Tel = string.Empty;
        protected string Qq = string.Empty;
        protected string PlatformDownloadUrl = string.Empty;
        protected string PlatformDownloadabroadUrl = string.Empty;
        protected string Mobilebg = string.Empty;
        protected string Mobilelogo = string.Empty;
        protected string Mobiledown = string.Empty;
        protected string Mobileabroaddown = string.Empty;
        protected string MobileQrcode = string.Empty;
        protected string MobileCase = string.Empty;
        //protected string action = string.Empty;
        //protected string msg = string.Empty;

        /// <summary>
        /// 初始化验证
        /// </summary>
        /// <param name="e"></param>
        protected override void OnInit(EventArgs e)
        {
            base.OnInit(e);

            if (Fetch.GetTerminalType(Page.Request) == 0)
            {
                Response.Redirect("/U3D/Index.aspx");
            }
        }

        /// <summary>
        /// 添加配置信息
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPreLoad(EventArgs e)
        {
            base.OnPreLoad(e);
            AddMetaTag("keywords", HttpUtility.HtmlEncode(AppConfig.PageKey));
            AddMetaTag("description", HttpUtility.HtmlEncode(AppConfig.PageDescript));
        }

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                int terminalType = Fetch.GetTerminalType(Page.Request);
                string imgDomain = string.Empty;
                IList<ConfigInfo> list = FacadeManage.aideNativeWebFacade.GetConfigInfoList();
                foreach (var item in list)
                {
                    if (item.ConfigKey == AppConfig.SiteConfigKey.MobilePlatformVersion.ToString())
                    {
                        PlatformDownloadUrl = terminalType == 2 ? item.Field9 : item.Field10;
                        PlatformDownloadabroadUrl = terminalType == 2 ? item.Field11 : item.Field12;
                    }
                    if (item.ConfigKey == AppConfig.SiteConfigKey.SysCustomerService.ToString())
                    {
                        Qq = item.Field3;
                        Tel = item.Field1;
                    }
                    if (item.ConfigKey == AppConfig.SiteConfigKey.WebSiteConfig.ToString())
                    {
                        imgDomain = item.Field2;
                    }
                }
                Mobilebg = Fetch.GetUploadFileUrl(imgDomain, "/Site/u3dmobilebg.png");
                Mobilelogo = Fetch.GetUploadFileUrl(imgDomain, "/Site/u3dfrontlogo.png");
                Mobiledown = Fetch.GetUploadFileUrl(imgDomain, "/Site/u3ddownLoad.png");
                Mobileabroaddown= Fetch.GetUploadFileUrl(imgDomain, "/Site/u3dabroaddownLoad.png");
                MobileCase = Fetch.GetUploadFileUrl(imgDomain, "/Site/u3dspreadcase.png");
                Random random = new Random();
                MobileQrcode = Fetch.GetQrCode("http://" + Request.Url.Authority + "/U3D/Mobile/DownLoad.aspx?r=" + random.Next(1000, 9999), 312);
                Title = AppConfig.PageTitle;


                //action = GameRequest.GetQueryString("action");
                //msg = GameRequest.GetQueryString("msg");
            }
        }

        /// <summary>
        /// 添加meta标签
        /// </summary>
        /// <param name="name">标签名称</param>
        /// <param name="value">标签值</param>
        protected virtual void AddMetaTag(string name, string value)
        {
            HtmlMeta meta = new HtmlMeta
            {
                Name = name,
                Content = value
            };
            Page.Header.Controls.Add(meta);
        }
    }
}