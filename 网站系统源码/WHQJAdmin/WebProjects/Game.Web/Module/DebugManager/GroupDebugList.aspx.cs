using System;
using System.Data;
using System.Text;
using Game.Entity.Accounts;
using Game.Entity.Agent;
using Game.Entity.Enum;
using Game.Entity.Group;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using Game.Web.UI;


namespace Game.Web.Module.DebugManager
{
    public partial class GroupDebugList : AdminPage
    {

        #region 窗体事件
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 902;
            if (!IsPostBack)
            {
                DBDataBind();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged(object sender, EventArgs e)
        {
            DBDataBind();
        }
        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            DBDataBind();
        }

        #endregion

        #region 数据绑定
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void DBDataBind()
        {
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            int type = Convert.ToInt32(ddlSearchType.SelectedValue);

            string where = " WHERE GroupStatus<>0";
            if (!string.IsNullOrEmpty(query))
            {
                where = where + string.Format(" AND {0}={1}", type == 1 ? "CreaterGameID" : "GroupID", query);
            }

            PagerSet pagerSet = FacadeManage.aideGroupFacade.GetPageGroupDebugList(where,
                Orderby, anpPage.CurrentPageIndex, anpPage.PageSize);
            anpPage.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                rptDataList.DataSource = pagerSet.PageSet.Tables[0];
                rptDataList.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptDataList.Visible = pagerSet.RecordCount > 0;
        }


        #endregion

        #region 公共属性
        /// <summary>
        /// 排序条件
        /// </summary>
        public string Orderby
        {
            get
            {
                if (ViewState["Orderby"] == null)
                {
                    ViewState["Orderby"] = "ORDER BY GroupID DESC";
                }
                return (string)ViewState["Orderby"];
            }
            set
            {
                ViewState["Orderby"] = value;
            }
        }

        #endregion
    }
}