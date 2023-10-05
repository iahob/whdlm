﻿using System;
using Game.Web.UI;
using Game.Kernel;
using System.Text;
using Game.Utils;
using Game.Entity.Record;
using Game.Facade;
using Game.Entity.Enum;
using System.Web.UI.WebControls;

namespace Game.Web.Module.AccountManager
{
    public partial class DiamondChangeList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 100;
            if(!IsPostBack)
            {
                txtStartDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
                txtEndDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
                cbType.DataSource = GetSerialTypeList(typeof(DiamondSerialType));
                cbType.DataTextField = "Description";
                cbType.DataValueField = "EnumValue";
                cbType.DataBind();

                ddlSerialType.DataSource = GetSerialTypeList(typeof(DiamondSerialType));
                ddlSerialType.DataTextField = "Description";
                ddlSerialType.DataValueField = "EnumValue";
                ddlSerialType.DataBind();
                ddlSerialType.Items.Insert(0, new ListItem("全部", "-1"));

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
        /// 设置查询时间
        /// </summary>
        /// <param name="startDate">起始时间</param>
        /// <param name="endDate">结束时间</param>
        private void SetCondition(string startDate, string endDate)
        {
            StringBuilder condition = new StringBuilder();
            condition.AppendFormat(" WHERE UserID={0} ", IntParam);
            if(!string.IsNullOrEmpty(startDate) && !string.IsNullOrEmpty(endDate))
            {
                condition.AppendFormat(" AND CollectDate BETWEEN '{0}' AND '{1}' ", startDate, endDate);
            }

            if (ddlSerialType.SelectedValue != "-1")
            {
                condition.AppendFormat(" AND TypeID={0}", ddlSerialType.SelectedValue);
            }

            ViewState["SearchItems"] = condition.ToString();
        }
        /// <summary>
        /// 时间查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            string startDate = txtStartDate.Text.Trim();
            string endDate = txtEndDate.Text.Trim();

            SetCondition(startDate + " 00:00:00", endDate + " 23:59:59");
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

            SetCondition(startDate, endDate);
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

            SetCondition(startDate, endDate);
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

            SetCondition(startDate, endDate);
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

            SetCondition(startDate, endDate);
            BindData();
        }
        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnType_Click(object sender, EventArgs e)
        {
            StringBuilder condition = new StringBuilder();
            string typeList = "";
            for(int i = 0; i < cbType.Items.Count; i++)
            {
                if(cbType.Items[i].Selected)
                {
                    typeList = typeList + cbType.Items[i].Value + ",";
                }
            }
            if(typeList == "")
            {
                condition.AppendFormat(" WHERE UserID={0} ", IntParam);
            }
            else
            {
                typeList = typeList.Substring(0, (typeList.Length - 1));
                condition.AppendFormat(" WHERE UserID={0} AND TypeID IN({1})", IntParam, typeList);
            }
            string startDate = txtStartDate.Text.Trim();
            string endDate = txtEndDate.Text.Trim();
            if(!string.IsNullOrEmpty(startDate) && !string.IsNullOrEmpty(endDate))
            {
                condition.AppendFormat(" AND CollectDate BETWEEN '{0}' AND '{1}'", startDate + " 00:00:00", endDate + " 23:59:59");
            }
            ViewState["SearchItems"] = condition.ToString();
            BindData();
        }

        protected void btnIn_Click(object sender, EventArgs e)
        {
            string condition = string.Format(" WHERE UserID={0} ", IntParam);
            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);
            if (startDate != "" && endDate != "")
            {
                condition = condition + string.Format(" AND CollectDate BETWEEN '{0}' AND '{1}'", startDate + " 00:00:00", endDate + " 23:59:59");
            }

            condition += " AND ChangeDiamond>0";
            ViewState["SearchItems"] = condition;
            BindData();
        }

        protected void btnOut_Click(object sender, EventArgs e)
        {
            string condition = string.Format(" WHERE UserID={0} ", IntParam);
            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);
            if (startDate != "" && endDate != "")
            {
                condition = condition + string.Format(" AND CollectDate BETWEEN '{0}' AND '{1}'", startDate + " 00:00:00", endDate + " 23:59:59");
            }

            condition += " AND ChangeDiamond<0";
            ViewState["SearchItems"] = condition;
            BindData();
        }


        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            PagerSet pagerSet = FacadeManage.aideRecordFacade.GetList(RecordDiamondSerial.Tablename, SearchItems, Orderby, anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            litNoData.Visible = pagerSet.PageSet.Tables[0].Rows.Count <= 0;
            rptDataList.DataSource = pagerSet.PageSet;
            rptDataList.DataBind();

            //绑定统计
            long addNum = FacadeManage.aideRecordFacade.GetTotalDiamondChange(SearchItems + " AND ChangeDiamond>0");
            long costNum = FacadeManage.aideRecordFacade.GetTotalDiamondChange(SearchItems + " AND ChangeDiamond<0");
            long total = addNum + costNum;
            lbChange.Text = string.Format("{0}{1} （消耗：{2}  增加：+{3}）", (total > 0 ? "+" : total < 0 ? "-" : ""), total, costNum, addNum);
        }

        /// <summary>
        /// 查询条件
        /// </summary>
        public string SearchItems
        {
            get
            {
                if(ViewState["SearchItems"] == null)
                {
                    string condition = string.Format(" WHERE UserID={0} ", IntParam);
                    string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
                    string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);
                    if(startDate != "" && endDate != "")
                    {
                        condition = condition + string.Format(" AND CollectDate BETWEEN '{0}' AND '{1}'", startDate + " 00:00:00", endDate + " 23:59:59");
                    }
                    ViewState["SearchItems"] = condition;
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
                if(ViewState["Orderby"] == null)
                {
                    ViewState["Orderby"] = "ORDER BY CollectDate DESC";
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