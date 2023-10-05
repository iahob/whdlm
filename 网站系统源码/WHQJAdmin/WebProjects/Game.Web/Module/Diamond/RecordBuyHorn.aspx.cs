using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Kernel;
using System.Text;
using Game.Utils;
using Game.Entity.Accounts;
using Game.Facade;
using Game.Entity.Record;
using System.Data;

namespace Game.Web.Module.Diamond
{
    public partial class RecordBuyHorn : AdminPage
    {
//        private DataTable usertable;

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 601;
            if(!IsPostBack)
            {
                txtStartDate.Text = DateTime.Now.ToString("yyyy-MM-") + "01";
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
//        /// <summary>
//        /// 获取用户信息
//        /// </summary>
//        /// <param name="userid"></param>
//        /// <returns></returns>
//        public string GetAccountsInfo(int userid)
//        {
//            if(usertable!=null&& usertable.Rows.Count > 0)
//            {
//                DataRow[] rows = usertable.Select("UserID=" + userid);
//                if (rows != null && rows.Length > 0)
//                {
//                    return string.Format("<td>{0}</td><td>{1}</td>",
//                        rows[0]["GameID"], rows[0]["NickName"]);
//                }
//            }
//            return "<td></td><td></td>";
//        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string sDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string eDate = CtrlHelper.GetTextAndFilter(txtEndDate);
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            int type = Convert.ToInt32(ddlSearchType.SelectedValue);
            StringBuilder condition = new StringBuilder();
            condition.Append("WHERE 1=1");

            if (!string.IsNullOrEmpty(query))
            {
                if (!Utils.Validate.IsPositiveInt(query))
                {
                    ShowError("输入的查询格式不正确");
                    return;
                }
                condition.AppendFormat(" AND UserID={0}", type == 1 ? GetUserIDByGameID(Convert.ToInt32(query)) : Convert.ToInt32(query));
            }
            if (sDate != "" && eDate != "")
            {
                condition.AppendFormat(" AND CollectDate BETWEEN '{0}' AND '{1}'", sDate, eDate);
            }

            //PagerSet pagerSet = FacadeManage.aideRecordFacade.GetPageBuyHornList(condition.ToString(), 
            //    Orderby,anpNews.CurrentPageIndex, anpNews.PageSize);
            //anpNews.RecordCount = pagerSet.RecordCount;
            //if (pagerSet.RecordCount > 0)
            //{
            //    usertable = pagerSet.PageSet.Tables[1];
            //    rptDataList.DataSource = pagerSet.PageSet.Tables[0];
            //    rptDataList.DataBind();
            //}
            //litNoData.Visible = pagerSet.RecordCount<=0;
            //rptDataList.Visible = pagerSet.RecordCount > 0;
        }
        /// <summary>
        /// 排序条件
        /// </summary>
        public string Orderby
        {
            get
            {
                if(ViewState["Orderby"] == null)
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