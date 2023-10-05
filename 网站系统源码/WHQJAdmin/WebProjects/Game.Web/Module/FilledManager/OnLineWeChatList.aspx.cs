using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Utils;
using Game.Kernel;
using Game.Web.UI;
using Game.Facade;
using Game.Entity.Enum;
using Game.Entity.Treasure;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.FilledManager
{
    public partial class OnLineWeChatList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 202;
            if (!IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 批量删除配置
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量删除在线充值配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Delete);
            int result = FacadeManage.aideTreasureFacade.DeleteOnLineWeChat(StrCIdList);
            if (result > 0)
            {
                ShowInfo("删除成功");
                BindData();
            }
            else
            {
                ShowInfo("删除失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            PagerSet pagerSet = FacadeManage.aideTreasureFacade.GetList(OnLineWeChat.Tablename, SearchItems, Orderby, anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            litNoData.Visible = pagerSet.PageSet.Tables[0].Rows.Count <= 0;
            rptDataList.DataSource = pagerSet.PageSet;
            rptDataList.DataBind();
        }
        /// <summary>
        /// 查询条件
        /// </summary>
        public string SearchItems
        {
            get
            {
                if (ViewState["SearchItems"] == null)
                {
                    ViewState["SearchItems"] = "WHERE 1=1";
                }
                return (string)ViewState["SearchItems"];
            }
            set
            {
                ViewState["SearchItems"] = value;
            }
        }
        /// <summary>
        /// 排序条件
        /// </summary>
        public string Orderby
        {
            get
            {
                if (ViewState["Orderby"] == null)
                {
                    ViewState["Orderby"] = "ORDER BY SortID DESC";
                }
                return (string)ViewState["Orderby"];
            }
            set
            {
                ViewState["Orderby"] = value;
            }
        }
    }
}