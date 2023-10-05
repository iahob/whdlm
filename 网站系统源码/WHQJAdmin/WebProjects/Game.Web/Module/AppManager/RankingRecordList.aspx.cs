using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Utils;
using Game.Kernel;
using Game.Web.UI;
using Game.Facade;
using Game.Entity.Enum;
using System.Data;
using System.Text;
using Game.Entity.Platform;
using Game.Entity.Treasure;
using Game.Entity.Record;
using Game.Entity.Accounts;
using Game.Entity.NativeWeb;

namespace Game.Web.Module.AppManager
{
    public partial class RankingRecordList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            if(!IsPostBack)
            {
                txtStartDate.Text = DateTime.Now.AddDays(-6).ToString("yyyy-MM-dd");
                txtEndDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
                BindData();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
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
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string search = CtrlHelper.GetTextAndFilter(txtSearch);
            string stime = CtrlHelper.GetTextAndFilter(txtStartDate);
            string etime = CtrlHelper.GetTextAndFilter(txtEndDate);
            int typeid = Convert.ToInt32(ddlRankType.SelectedValue);

            int userid = 0;
            int sdate = 0;
            int edate = 0;
            if (!string.IsNullOrEmpty(search))
            {
                UserInfo info = FacadeManage.aideAccountsFacade.GetUserInfo(0, Convert.ToInt32(search));
                userid = info != null ? info.UserID : -1;
            }
            if (!string.IsNullOrEmpty(stime) && !string.IsNullOrEmpty(etime))
            {
                sdate = Convert.ToInt32(Fetch.GetDateID(Convert.ToDateTime(stime)));
                edate = Convert.ToInt32(Fetch.GetDateID(Convert.ToDateTime(etime)));
            }

            DataSet ds = FacadeManage.aideNativeWebFacade.GetRankAwardList(typeid, userid, sdate, edate);
            litNoData.Visible = ds.Tables[0].Rows.Count > 0 ? false : true;
            rptDataList.DataSource = ds.Tables[0];
            rptDataList.DataBind();
        }
    }
}