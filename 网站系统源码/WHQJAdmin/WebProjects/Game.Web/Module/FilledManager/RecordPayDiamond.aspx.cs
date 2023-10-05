using System;
using Game.Web.UI;
using System.Text;
using Game.Kernel;
using Game.Utils;
using Game.Facade;
using Game.Entity.Treasure;
using System.Collections.Generic;

namespace Game.Web.Module.FilledManager
{
    public partial class RecordPayDiamond : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 201;
            if (!IsPostBack)
            {
                txtStartDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
                txtEndDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
                ShareInfoDataBind();
            }
        }

        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            ShareInfoDataBind();
        }

        /// <summary>
        /// 设置查询条件
        /// </summary>
        private void SetCondition()
        {
            int service = int.Parse(ddlGlobalShareInfo.SelectedValue);
            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate) + " 00:00:00";
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate) + " 23:59:59";
            StringBuilder condition = new StringBuilder("WHERE 1=1");
            if (service > 0)
            {
                condition.AppendFormat(" AND ShareID={0} ", service);
            }
            if (!string.IsNullOrEmpty(startDate) && !string.IsNullOrEmpty(endDate))
            {
                condition.AppendFormat(" AND OrderDate BETWEEN '{0}' AND '{1}'", startDate, endDate);
            }
            ViewState["SearchItems"] = condition.ToString();
        }

        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            SetCondition();
            ShareInfoDataBind();
        }

        /// <summary>
        /// 查询今天
        /// </summary>
        protected void btnQueryTD_Click(object sender, EventArgs e)
        {
            CtrlHelper.SetText(txtStartDate,
                Convert.ToDateTime(Fetch.GetTodayTime().Split('$')[0]).ToString("yyyy-MM-dd"));
            CtrlHelper.SetText(txtEndDate,
                Convert.ToDateTime(Fetch.GetTodayTime().Split('$')[1]).ToString("yyyy-MM-dd"));

            btnQuery_Click(sender, e);
        }

        /// <summary>
        /// 查询昨天
        /// </summary>
        protected void btnQueryYD_Click(object sender, EventArgs e)
        {
            CtrlHelper.SetText(txtStartDate,
                Convert.ToDateTime(Fetch.GetYesterdayTime().Split('$')[0]).ToString("yyyy-MM-dd"));
            CtrlHelper.SetText(txtEndDate,
                Convert.ToDateTime(Fetch.GetYesterdayTime().Split('$')[1]).ToString("yyyy-MM-dd"));

            btnQuery_Click(sender, e);
        }

        /// <summary>
        /// 查询本周
        /// </summary>
        protected void btnQueryTW_Click(object sender, EventArgs e)
        {
            CtrlHelper.SetText(txtStartDate,
                Convert.ToDateTime(Fetch.GetWeekTime().Split('$')[0]).ToString("yyyy-MM-dd"));
            CtrlHelper.SetText(txtEndDate,
                Convert.ToDateTime(Fetch.GetWeekTime().Split('$')[1]).ToString("yyyy-MM-dd"));

            btnQuery_Click(sender, e);
        }

        /// <summary>
        /// 查询上周
        /// </summary>
        protected void btnQueryYW_Click(object sender, EventArgs e)
        {
            CtrlHelper.SetText(txtStartDate,
                Convert.ToDateTime(Fetch.GetLastWeekTime().Split('$')[0]).ToString("yyyy-MM-dd"));
            CtrlHelper.SetText(txtEndDate,
                Convert.ToDateTime(Fetch.GetLastWeekTime().Split('$')[1]).ToString("yyyy-MM-dd"));

            btnQuery_Click(sender, e);
        }

        /// <summary>
        /// 用户查询
        /// </summary>
        protected void btnQueryAcc_Click(object sender, EventArgs e)
        {
            int type = Convert.ToInt32(ddlSearchType.SelectedValue);
            int status = Convert.ToInt32(ddlPayStatus.SelectedValue);
            string queryContent = CtrlHelper.GetTextAndFilter(txtSearch);

            StringBuilder condition = new StringBuilder("WHERE 1=1");
            if (status >= 0)
            {
                condition.AppendFormat(" AND OrderStatus='{0}'", status);
            }
            if (!string.IsNullOrEmpty(queryContent))
            {
                switch (type)
                {
                    case 0:
                        condition.AppendFormat(" AND OrderID='{0}'", queryContent);
                        break;
                    default:
                        if (!Utils.Validate.IsPositiveInt(queryContent))
                        {
                            ShowError("输入的查询格式不正确");
                            return;
                        }
                        condition.AppendFormat(" AND GameID={0}", queryContent);
                        break;
                }
            }

            ViewState["SearchItems"] = condition.ToString();
            ShareInfoDataBind();
        }

        /// <summary>
        /// 充值类型
        /// </summary>
        /// <param name="shareId"></param>
        /// <returns></returns>
        public string GetOrderShareName(int shareId)
        {
            switch (shareId)
            {
                case 101:
                    return "手机微信充值";
                case 102:
                    return "H5微信充值";
                case 201:
                    return "手机支付宝充值";
                case 301:
                    return "手机零钱充值";
                case 302:
                    return "竣付通微信充值";
                case 303:
                    return "竣付通支付宝充值";
                case 800:
                    return "手机苹果充值";
                default:
                    return "未知类型";
            }
        }

        /// <summary>
        /// 充值状态
        /// </summary>
        /// <param name="status"></param>
        /// <returns></returns>
        protected string GetPayStatus(int status, string url = "", string winName = "Record_Status", int w = 500, int h = 300)
        {
            switch (status)
            {
                case 2:
                    return "<span>已完成</span>";
                case 1:
                    return $"<a class='l' href='javascript:;' onclick='openWindowOwn(\"{url}\",\"{winName}\",{w},{h})'>已支付，未处理</a>"; ;
                default:
                    return "<span class='hong'>未支付</span>";
            }
        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void ShareInfoDataBind()
        {
            PagerSet pagerSet = FacadeManage.aideTreasureFacade.GetList(OnLinePayOrder.Tablename,
                 SearchItems, Orderby,anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            litNoData.Visible = pagerSet.PageSet.Tables[0].Rows.Count <= 0;
            ltTotal.Text = $"已支付金额：{FacadeManage.aideTreasureFacade.GetTotalPayAmount(SearchItems)}元 已支付订单数：{FacadeManage.aideTreasureFacade.GetTotalPayOrderCount(SearchItems)} (当前条件统计)";
            rptShareInfo.DataSource = pagerSet.PageSet;
            rptShareInfo.DataBind();
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
                    SetCondition();
                }
                return (string) ViewState["SearchItems"];
            }
            set { ViewState["SearchItems"] = value; }
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
                    ViewState["Orderby"] = "ORDER BY OnLineID DESC";
                }
                return (string) ViewState["Orderby"];
            }
            set { ViewState["Orderby"] = value; }
        }
    }
}