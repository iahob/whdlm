using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Kernel;
using Game.Utils;
using Game.Facade;
using Game.Entity.Enum;
using Game.Entity.Treasure;
using Game.Entity.Group;
using System.Data;
using System.Text;


namespace Game.Web.Module.ClubManager
{
    public partial class RecordBoxScoreList : AdminPage
    {
        private DataTable usertable;
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 903;
            if (!IsPostBack)
            {
                txtStartDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
                txtEndDate.Text = DateTime.Now.ToString("yyyy-MM-dd");

                RecordInsureDataBind();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            RecordInsureDataBind();
        }

        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {

            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);

            startDate = Convert.ToDateTime(startDate).ToString("yyyy-MM-dd") + " 00:00:00";
            endDate = Convert.ToDateTime(endDate).ToString("yyyy-MM-dd") + " 23:59:59";

            RecordInsureDataBind();
        }
        /// <summary>
        /// 查询今天
        /// </summary>
        protected void btnQueryTD_Click(object sender, EventArgs e)
        {
            string startDate = Fetch.GetTodayTime().Split('$')[0].ToString();
            string endDate = Fetch.GetTodayTime().Split('$')[1].ToString();

            CtrlHelper.SetText(txtStartDate, Convert.ToDateTime(startDate).ToString("yyyy-MM-dd"));
            CtrlHelper.SetText(txtEndDate, Convert.ToDateTime(endDate).ToString("yyyy-MM-dd"));

            RecordInsureDataBind();
        }
        /// <summary>
        /// 查询昨天
        /// </summary>
        protected void btnQueryYD_Click(object sender, EventArgs e)
        {
            string startDate = Fetch.GetYesterdayTime().Split('$')[0].ToString();
            string endDate = Fetch.GetYesterdayTime().Split('$')[1].ToString();

            CtrlHelper.SetText(txtStartDate, Convert.ToDateTime(startDate).ToString("yyyy-MM-dd"));
            CtrlHelper.SetText(txtEndDate, Convert.ToDateTime(endDate).ToString("yyyy-MM-dd"));

            RecordInsureDataBind();
        }
        /// <summary>
        /// 查询本周
        /// </summary>
        protected void btnQueryTW_Click(object sender, EventArgs e)
        {
            string startDate = Fetch.GetWeekTime().Split('$')[0].ToString();
            string endDate = Fetch.GetWeekTime().Split('$')[1].ToString();

            CtrlHelper.SetText(txtStartDate, Convert.ToDateTime(startDate).ToString("yyyy-MM-dd"));
            CtrlHelper.SetText(txtEndDate, Convert.ToDateTime(endDate).ToString("yyyy-MM-dd"));

            RecordInsureDataBind();
        }
        /// <summary>
        /// 查询上周
        /// </summary>
        protected void btnQueryYW_Click(object sender, EventArgs e)
        {
            string startDate = Fetch.GetLastWeekTime().Split('$')[0].ToString();
            string endDate = Fetch.GetLastWeekTime().Split('$')[1].ToString();

            CtrlHelper.SetText(txtStartDate, Convert.ToDateTime(startDate).ToString("yyyy-MM-dd"));
            CtrlHelper.SetText(txtEndDate, Convert.ToDateTime(endDate).ToString("yyyy-MM-dd"));

            RecordInsureDataBind();
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void RecordInsureDataBind()
        {
            string queryContent = CtrlHelper.GetTextAndFilter(txtSearch);
            int searchType = Convert.ToInt32(ddlSearchType.SelectedValue);
            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);

            StringBuilder condition = new StringBuilder("WHERE 1=1");
            if (!string.IsNullOrEmpty(queryContent))
            {
                if (!Utils.Validate.IsPositiveInt(queryContent))
                {
                    ShowError("查询的游戏ID格式错误");
                    return;
                }
                condition.AppendFormat(" AND {0}={1}", searchType == 1 ? "GroupID" : "UserID",
                    searchType == 1 ? queryContent : GetUserIDByGameID(Convert.ToInt32(queryContent)).ToString());
            }
            if (!string.IsNullOrEmpty(startDate) && !string.IsNullOrEmpty(endDate))
            {
                condition.AppendFormat(" AND CollectTime BETWEEN '{0}' AND '{1}'", startDate + " 00:00:00", endDate + " 23:59:59");
            }

            PagerSet pagerSet = FacadeManage.aideGroupFacade.GetPageGroupBoxScoreList(condition.ToString(), Orderby, anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                usertable = pagerSet.PageSet.Tables[1];
                rptInsure.DataSource = pagerSet.PageSet.Tables[0];
                rptInsure.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptInsure.Visible = pagerSet.RecordCount > 0;
        }

        /// <summary>
        /// 获取用户信息
        /// </summary>
        /// <param name="userid"></param>
        /// <returns></returns>
        public string GetAccountsInfo(int userid)
        {
            if (usertable != null && usertable.Rows.Count > 0)
            {
                DataRow[] rows = usertable.Select("UserID=" + userid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("{0}（{1}）", rows[0]["NickName"], rows[0]["GameID"]);
                }
            }
            return "";
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
                    ViewState["Orderby"] = "ORDER BY RecordID DESC";
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