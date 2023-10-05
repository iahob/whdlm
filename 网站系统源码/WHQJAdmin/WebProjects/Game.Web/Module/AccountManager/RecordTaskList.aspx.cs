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
using Game.Entity.Enum;
using Game.Facade;
using Game.Entity.Accounts;
using Game.Entity.Record;
using System.Data;

namespace Game.Web.Module.AccountManager
{
    public partial class RecordTaskList : AdminPage
    {
        private DataTable usertable;
        private DataTable tasktable;
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 104;
            if (!IsPostBack)
            {
                string time = DateTime.Now.ToString("yyyy-MM-dd");
                txtStartDate.Text = time;
                txtEndDate.Text = time;
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
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);
            int type = int.Parse(ddlSearchType.SelectedValue);
            StringBuilder condition = new StringBuilder();
            condition.Append(" WHERE 1=1");

            if (!string.IsNullOrEmpty(query))
            {
                if (!Utils.Validate.IsPositiveInt(query))
                {
                    ShowError("输入的查询格式不正确");
                    return;
                }
                condition.AppendFormat(" AND UserID={0} ", GetUserIDByGameID(Convert.ToInt32(query)));
            }
            if (startDate != "" && endDate != "")
            {
                condition.AppendFormat(" AND CollectDate>='{0}' AND CollectDate<='{1}'", startDate + " 00:00:00", endDate + " 23:59:59");
            }

            PagerSet pagerSet = FacadeManage.aideRecordFacade.GetPageTaskRecordList(condition.ToString(), Orderby,
                anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                usertable= pagerSet.PageSet.Tables[1];
                tasktable = pagerSet.PageSet.Tables[2];
                rptDataList.DataSource = pagerSet.PageSet.Tables[0];
                rptDataList.DataBind();
            }
            rptDataList.Visible = pagerSet.RecordCount > 0;
            litNoData.Visible = pagerSet.RecordCount <= 0;
        }

        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            BindData();
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

            BindData();
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

            BindData();
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

            BindData();
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

            BindData();
        }

        /// <summary>
        /// 获取用户信息
        /// </summary>
        protected string GetUserInfo(int userid)
        {
            if (usertable != null && usertable.Rows.Count > 0)
            {
                DataRow[] rows = usertable.Select("UserID="+ userid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("<td>{0}</td><td>{1}</td>", rows[0]["GameID"], rows[0]["NickName"]);
                }
            }
            return "<td></td><td></td>";
        }

        /// <summary>
        /// 获取任务信息
        /// </summary>
        protected string GetTaskInfo(int taskid)
        {
            if (tasktable != null && tasktable.Rows.Count > 0)
            {
                DataRow[] rows = tasktable.Select("TaskID=" + taskid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("<td>{0}</td><td>{1}</td><td>{2}</td>",
                   rows[0]["TaskName"], TaskTypeName(Convert.ToInt32(rows[0]["TaskType"])), TimeTypeName(Convert.ToInt32(rows[0]["TimeType"])));
                }
            }
            return "<td></td><td></td><td></td>";
        }
        /// <summary>
        /// 任务类型
        /// </summary>
        protected string TaskTypeName(int typeid)
        {
            return typeid == 1 ? "总局任务" : typeid == 2 ? "胜局任务" : typeid == 4 ? "首胜任务" : typeid == 8 ? "连胜任务" : "其他任务";
        }
        /// <summary>
        /// 用户类型
        /// </summary>
        protected string UserTypeName(int typeid)
        {
            return typeid == 1 ? "普通用户" : typeid == 2 ? "会员用户" : "所有用户";
        }
        /// <summary>
        /// 时间类型
        /// </summary>
        protected string TimeTypeName(int typeid)
        {
            return typeid == 1 ? "一次任务" : typeid == 2 ? "每日任务" : typeid == 4 ? "每周任务" : "每月任务";
        }
        /// <summary>
        /// 奖励的类型
        /// </summary>
        private string AwardName(int typeid)
        {
            return typeid == 1 ? "金币" : typeid == 2 ? "钻石" : typeid == 3 ? "奖券" : "其他";
        }
        /// <summary>
        /// 任务奖励
        /// </summary>
        /// <returns></returns>
        protected string GetTaskAward(string award)
        {
            string value = "";
            if (!string.IsNullOrEmpty(award))
            {
                string[] ad = null;
                if (award.IndexOf(';') > 0)
                {
                    string[] arry = award.Split(';');
                    foreach (string item in arry)
                    {
                        ad = item.Split(':');
                        value = value + AwardName(Convert.ToInt32(ad[0])) + "：" + ad[1].ToString() + ",";
                    }
                }
                else
                {
                    ad = award.Split(':');
                    value = value + AwardName(Convert.ToInt32(ad[0])) + "：" + ad[1].ToString() + ",";
                }
                return value != "" ? value.Substring(0, value.Length - 1) : "";
            }
            return "";
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
                    ViewState["SearchItems"] = " WHERE 1=1";
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