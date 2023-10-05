using Game.Entity.Accounts;
using Game.Entity.Enum;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using Game.Web.UI;
using System;
using System.Data;
using System.Text;
using Game.Entity.Agent;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AgentManager
{
    public partial class AgentList : AdminPage
    {
        private DataTable usertable;
        private DataTable parenttable;
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1001;
            if (!IsPostBack)
            {
                AgentDataBind();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged(object sender, EventArgs e)
        {
            AgentDataBind();
        }
        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            AgentDataBind();
        }
        /// <summary>
        /// 批量冻结玩家
        /// </summary>
        protected void btnDongjie_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量冻结代理";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Enable);
            int result = FacadeManage.aideAgentFacade.NullityAgentUser(StrCIdList, 1);
            if(result > 0)
            {
                ShowInfo("冻结成功");
                AgentDataBind();
            }
            else
            {
                ShowError("冻结失败");
            }
        }
        /// <summary>
        /// 批量解冻玩家
        /// </summary>
        protected void btnJiedong_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量解冻代理";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Enable);
            int result = FacadeManage.aideAgentFacade.NullityAgentUser(StrCIdList, 0);
            if(result > 0)
            {
                ShowInfo("解冻成功");
                AgentDataBind();
            }
            else
            {
                ShowError("解冻失败");
            }
        }
        /// <summary>
        /// 获取用户信息
        /// </summary>
        /// <returns></returns>
        protected string GetAccountsInfo(int userid, int agentid)
        {
            if (usertable != null && usertable.Rows.Count > 0)
            {
                DataRow[] rows = usertable.Select("UserID=" + userid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("<td>{0}</td><td><a class=\"l\" href=\"javascript:void(0)\" onclick=\"openWindowOwn('AgentUserUpdate.aspx?param={1}', '{2}', 700,490);\">{2}</a></td>",
                        rows[0]["GameID"], agentid, rows[0]["NickName"]);
                }
            }
            return "<td></td><td></td>";
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void AgentDataBind()
        {
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            int typeid = Convert.ToInt32(ddlSearchType.SelectedValue);
            StringBuilder condition = new StringBuilder(" WHERE 1=1");
            if (!string.IsNullOrEmpty(query))
            {
                if (typeid == 1)
                {
                    if (!Utils.Validate.IsPositiveInt(query))
                    {
                        ShowError("输入查询格式不正确");
                        return;
                    }
                    UserInfo info = FacadeManage.aideAccountsFacade.GetUserInfo(0, Convert.ToInt32(query));
                    condition.AppendFormat(" AND UserID={0}", info != null ? info.UserID : 0);
                }
                else
                {
                    condition.AppendFormat(" AND {0}={1}", typeid == 2 ? "Compellation" : "ContactPhone", query);
                }
            }
            
            PagerSet pagerSet = FacadeManage.aideAgentFacade.GetPageAgentList(condition.ToString(), "ORDER BY AgentID DESC", anpPage.CurrentPageIndex, anpPage.PageSize);
            anpPage.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                usertable = pagerSet.PageSet.Tables[1];
                parenttable = pagerSet.PageSet.Tables[2];
                rptDataList.DataSource = pagerSet.PageSet.Tables[0];
                rptDataList.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptDataList.Visible = pagerSet.RecordCount > 0;
        }
        
        /// <summary>
        /// 通过代理ID获取代理信息
        /// </summary>
        /// <param name="agentid"></param>
        /// <returns></returns>
        protected string GetAgentInfo(int agentid)
        {
            if (parenttable != null && parenttable.Rows.Count > 0 && agentid>0)
            {
                DataRow[] rows = parenttable.Select("AgentID=" + agentid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("{0}（{1}）", rows[0]["NickName"], rows[0]["GameID"]);
                }
            }
            return "";
        }
    }
}