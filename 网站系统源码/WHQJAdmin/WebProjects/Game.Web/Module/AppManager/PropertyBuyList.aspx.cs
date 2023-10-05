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
using Game.Entity.NativeWeb;
using Game.Entity.Accounts;

namespace Game.Web.Module.AppManager
{
    public partial class PropertyBuyList : AdminPage
    {
        private DataTable usertable;
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                txtStartDate.Text = DateTime.Now.AddDays(-7).ToString("yyyy-MM-dd");
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
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            BindData();
        }

        /// <summary>
        /// 获取类型
        /// </summary>
        protected string GetTypeName(int typeid)
        {
            return typeid == 1 ? "游戏币购买" : typeid == 0 ? "钻石购买" : "未知类别";
        }

        /// <summary>
        /// 获取用户信息
        /// </summary>
        protected string GetUserInfo(int userid)
        {
            if(usertable!=null&& usertable.Rows.Count > 0)
            {
                DataRow[] rows = usertable.Select("UserID=" + userid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("<td>{0}</td><td>{1}</td>", rows[0]["GameID"], rows[0]["NickName"]);
                }
            }
            return "<td></td><td></td>";
        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            string stime = CtrlHelper.GetTextAndFilter(txtStartDate);
            string etime = CtrlHelper.GetTextAndFilter(txtEndDate);

            string where = "WHERE 1=1";
            if (!string.IsNullOrEmpty(query))
            {
                if (!Utils.Validate.IsPositiveInt(query))
                {
                    ShowError("输入查询格式不正确");
                    return;
                }
                UserInfo info = FacadeManage.aideAccountsFacade.GetUserInfo(0, Convert.ToInt32(query));
                where = where + string.Format(" AND UserID={0}", info != null ? info.UserID : -1);
            }
            if (!string.IsNullOrEmpty(stime) && !string.IsNullOrEmpty(etime))
            {
                where = where + string.Format(" AND CollectDate>='{0}' AND CollectDate<='{1}'", stime+" 00:00:00", etime+" 23:59:59");
            }

            PagerSet pagerSet = FacadeManage.aideRecordFacade.GetPageBuyPropertyList(where, " ORDER BY CollectDate DESC", anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                usertable = pagerSet.PageSet.Tables[1];
                rptDataList.DataSource = pagerSet.PageSet.Tables[0];
                rptDataList.DataBind();
            }
            rptDataList.Visible = pagerSet.RecordCount > 0;
            litNoData.Visible = pagerSet.RecordCount <= 0;
        }

    }
}