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
using System.Data;
using Game.Facade;
using Game.Entity.Treasure;
using Game.Entity.Platform;

namespace Game.Web.Module.ClubManager
{
    public partial class RecordGroupUserGame : AdminPage
    {
        private DataSet gameroom = Fetch.GetKindAndRoom();
        private DataTable infotable;
        private DataTable scoretable;
        
        private DataTable usertable;

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 505;
            if (!IsPostBack)
            {
                string time = DateTime.Now.ToString("yyyy-MM-dd");
                txtStartDate.Text = time;
                txtEndDate.Text = time;
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged(object sender, EventArgs e)
        {
            BindDataGameScore();
        }

        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            BindDataGameScore();
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

            BindDataGameScore();
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

            BindDataGameScore();
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

            BindDataGameScore();
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

            BindDataGameScore();
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindDataGameScore()
        {
            StringBuilder condition = new StringBuilder();
            string startDate = CtrlHelper.GetTextAndFilter(txtStartDate);
            string endDate = CtrlHelper.GetTextAndFilter(txtEndDate);
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            int typeId = Convert.ToInt32(ddlSearchType.SelectedValue);
            if (string.IsNullOrEmpty(query))
            {
                ShowError("请输入信息进行查询");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(query))
            {
                ShowError("输入用户格式不正确");
                return;
            }
            int id = Convert.ToInt32(query);
            if (typeId == 3)
            {
                condition.AppendFormat(" WHERE GroupID={0}", id);
            }
            else
            {
                condition.AppendFormat(" WHERE RoomID IN (SELECT DISTINCT RoomID FROM  RecordGroupDrawScore WHERE UserID={0})", typeId == 1 ? GetUserIDByGameID(id) : id);
            }

            
            if (!string.IsNullOrEmpty(startDate))
                condition.AppendFormat(" AND StartTime >= '{0}' ", startDate);
            if (!string.IsNullOrEmpty(endDate))
                condition.AppendFormat(" AND StartTime < '{0}'", Convert.ToDateTime(endDate).AddDays(1).ToString("yyyy-MM-dd"));

            PagerSet pagerSet = FacadeManage.aideGroupFacade.GetPageGroupDrawList(condition.ToString(), Orderby, anpPage.CurrentPageIndex, anpPage.PageSize);
            anpPage.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                infotable = pagerSet.PageSet.Tables[1];
                scoretable = pagerSet.PageSet.Tables[2];
                usertable = pagerSet.PageSet.Tables[3];
                rptDataList.DataSource = pagerSet.PageSet.Tables[0];
                rptDataList.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptDataList.Visible = pagerSet.RecordCount > 0;
            litTip.Visible = false;
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
        public string GetUserInfo(int userid)
        {
            if (usertable != null && usertable.Rows.Count > 0)
            {
                DataRow[] rows = usertable.Select("UserID=" + userid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("<td>{0}</td><td>{1}</td>", rows[0]["GameID"], rows[0]["NickName"].ToString());
                }
            }
            return "<td></td><td></td>";
        }
        /// <summary>
        /// 子数据绑定
        /// </summary>
        protected void rptDataList_ItemDataBound(object sender, RepeaterItemEventArgs e)
        {
            if ((e.Item.ItemType == ListItemType.Item) || (e.Item.ItemType == ListItemType.AlternatingItem))
            {
                DataRowView dataItem = (DataRowView)e.Item.DataItem;
                if (infotable != null && infotable.Rows.Count > 0)
                {
                    DataRow[] rows = infotable.Select("RoomID=" + dataItem["RoomID"]);
                    if (rows != null && rows.Length > 0)
                    {
                        Repeater repeater = (Repeater)e.Item.FindControl("rptSubData");
                        if (repeater != null)
                        {
                            repeater.DataSource = rows;
                            repeater.DataBind();
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 二级子数据绑定
        /// </summary>
        protected void rptSubData_ItemDataBound(object sender, RepeaterItemEventArgs e)
        {
            if ((e.Item.ItemType == ListItemType.Item) || (e.Item.ItemType == ListItemType.AlternatingItem))
            {
                DataRow dataItem = (DataRow)e.Item.DataItem;
                if (scoretable != null && scoretable.Rows.Count > 0)
                {
                    DataRow[] rows = scoretable.Select("DrawID=" + dataItem["DrawID"]);
                    if (rows != null && rows.Length > 0)
                    {
                        Repeater repeater = (Repeater)e.Item.FindControl("rptSubtwoData");
                        if (repeater != null)
                        {
                            repeater.DataSource = rows;
                            repeater.DataBind();
                        }
                    }
                }
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
                    ViewState["Orderby"] = "ORDER BY RoomID DESC";
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