using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Kernel;
using System.Text;
using System.Data;
using Game.Utils;
using Game.Entity.Accounts;
using Game.Facade;
using Game.Entity.Platform;
using Game.Entity.Record;

namespace Game.Web.Module.ClubManager
{
    public partial class RecordDiamondList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 902;
            if (!IsPostBack)
            {
                txtStartDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
                txtEndDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
                BindData();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery1_Click(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 时间查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);
            BindData();
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

            BindData();
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

            BindData();
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

            BindData();
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

            BindData();
        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string gid = GameRequest.GetQueryString("gid");
            int type = Convert.ToInt32(ddlSearchType.SelectedValue);
            string stime = CtrlHelper.GetTextAndFilter(txtStartDate);
            string etime = CtrlHelper.GetTextAndFilter(txtEndDate);
            string where = "WHERE GroupID=" + gid;
            if (stime != "" && etime != "")
            {
                where = where + string.Format(" AND CollectDate BETWEEN '{0}' AND '{1}'", stime + " 00:00:00", etime + " 23:59:59");
            }
            if (type > 0)
            {
                where = where + string.Format(" AND TradeType={0}", type);
            }

            PagerSet pagerSet = FacadeManage.aideGroupFacade.GetList("RecordGroupIngotChange",
                where, Orderby, anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            litNoData.Visible = pagerSet.PageSet.Tables[0].Rows.Count > 0 ? false : true;
            rptDataList.DataSource = pagerSet.PageSet;
            rptDataList.DataBind();
        }

        /// <summary>
        /// 创建操作类型
        /// </summary>
        /// <param name="typeid"></param>
        /// <returns></returns>
        protected string GetTradeName(int typeid)
        {
            return typeid == 1 ? "存入钻石" : typeid == 2 ? "取出钻石" : typeid == 3 ? "开房消耗" : typeid == 4 ? "解散返还" : "未知";
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