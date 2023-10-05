using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Text;
using Game.Entity.Accounts;
using Game.Web.UI;
using Game.Kernel;
using Game.Utils;
using Game.Facade;
using Game.Entity.Treasure;
using System.Data;

namespace Game.Web.Module.GoldManager
{
    public partial class RecordGameInOut : AdminPage
    {
        private DataSet gameroom = Fetch.GetKindAndRoom();
        private DataTable usertable;

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 504;
            if(!IsPostBack)
            {
                string time = DateTime.Now.ToString("yyyy-MM-dd");
                txtStartDate.Text = time;
                txtEndDate.Text = time;
                UserInoutDataBind();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            UserInoutDataBind();
        }
        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            UserInoutDataBind();
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

            UserInoutDataBind();
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

            UserInoutDataBind();
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

            UserInoutDataBind();
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

            UserInoutDataBind();
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void UserInoutDataBind()
        {
            StringBuilder condition = new StringBuilder();
            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);
            int typeTime = int.Parse(ddlTimeType.SelectedValue);
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            int typeId = Convert.ToInt32(ddlSearchType.SelectedValue);
            bool isIncludeAndroid = cbIncludeAndroid.Checked;

            condition.Append("WHERE (PlayTimeCount > 0 OR Score <> 0 OR Revenue <> 0 OR LeaveTime IS NULL) ");
            if (!isIncludeAndroid) condition.Append(" AND UserID NOT IN (SELECT UserID FROM WHQJAccountsDB.dbo.AccountsInfo WHERE IsAndroid = 1) ");
            if (!string.IsNullOrEmpty(query))
            {
                if (!Utils.Validate.IsPositiveInt(query))
                {
                    ShowError("输入用户格式不正确");
                    return;
                }
                int id = Convert.ToInt32(query);
                condition.AppendFormat(" AND UserID={0}", typeId == 1 ? GetUserIDByGameID(id) : id);
            }
            if (startDate != "" && endDate != "")
            {
                condition.AppendFormat(" AND EnterTime BETWEEN '{0}' AND '{1}'", 
                    Convert.ToDateTime(startDate), Convert.ToDateTime(endDate).AddDays(1).ToString("yyyy-MM-dd"));
            }

            PagerSet pagerSet = FacadeManage.aideTreasureFacade.GetPageInoutList(condition.ToString(), Orderby,
                anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                usertable = pagerSet.PageSet.Tables[1];
                rptUserInout.DataSource = pagerSet.PageSet.Tables[0];
                rptUserInout.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptUserInout.Visible = pagerSet.RecordCount > 0;
        }
        /// <summary>
        /// 获取离开原因
        /// </summary>
        public string GetLeaveReason(int typeId)
        {
            switch(typeId)
            {
                case 0:
                    return "常规离开";
                case 1:
                    return "系统原因";
                case 2:
                    return "网络原因";
                case 3:
                    return "用户冲突";
                case 4:
                    return "人满为患";
                case 5:
                    return "条件限制";
                default:
                    return "未知";
            }
        }

        /// <summary>
        /// 获取游戏和房间名称
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <param name="serverid">房间标识</param>
        /// <returns></returns>
        protected string GetGameAndRoomName(int kindid, int serverid)
        {
            if (gameroom != null && gameroom.Tables.Count > 0)
            {
                DataRow[] rows = gameroom.Tables[0].Select("KindID=" + kindid);
                string name = string.Format("<td>{0}</td>",
                    rows != null && rows.Length > 0 ? rows[0]["KindName"].ToString() : "");
                rows = gameroom.Tables[1].Select("ServerID=" + serverid);
                name = name + string.Format("<td>{0}</td>",
                    rows != null && rows.Length > 0 ? rows[0]["ServerName"].ToString() : "");
                return name;
            }
            return "<td></td><td></td>";
        }

        /// <summary>
        /// 获取用户信息
        /// </summary>
        /// <param name="userid"></param>
        /// <returns></returns>
        protected string GetAccountsInfo(int userid)
        {
            if(usertable!=null&& usertable.Rows.Count > 0)
            {
                DataRow[] rows = usertable.Select("UserID=" + userid);
                if(rows!=null&& rows.Length > 0)
                {
                    return string.Format("{0}({1})", rows[0]["NickName"], rows[0]["GameID"]);
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
                if(ViewState["Orderby"] == null)
                {
                    ViewState["Orderby"] = "ORDER BY ID DESC";
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