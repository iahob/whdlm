using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Text;

using Game.Utils;
using Game.Kernel;
using Game.Web.UI;
using Game.Facade;
using System.Collections;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.OperationLog
{
    public partial class SystemSecurityList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 801;
            if(!IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnSearch_OnClick(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string keyword = CtrlHelper.GetTextAndFilter(txtKeyword);
            string where = "WHERE 1=1";
            if (!string.IsNullOrEmpty(keyword))
            {
                where = string.Format("WHERE OperatingAccounts='{0}'", keyword);
            }

            PagerSet pagerSet = FacadeManage.aidePlatformManagerFacade.GetList(SystemSecurity.Tablename,
                where, "ORDER BY RecordID DESC", anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                rptDataList.DataSource = pagerSet.PageSet;
                rptDataList.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptDataList.Visible = pagerSet.RecordCount > 0;
        }
    }
}
