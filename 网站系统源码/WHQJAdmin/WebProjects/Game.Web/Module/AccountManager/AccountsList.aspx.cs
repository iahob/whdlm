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
using Game.Entity;
using Game.Entity.Enum;
using Game.Facade;
using System.Data;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AccountManager
{
    public partial class AccountsList : AdminPage
    {
        private DataTable SpreadData;
        private DataTable AgentUserData;
        private DataTable AgentInfoData;

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 100;
            if (!IsPostBack)
            {
                AccountsDataBind();
            }
        }

        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged(object sender, EventArgs e)
        {
            AccountsDataBind();
        }

        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            AccountsDataBind();
        }

        /// <summary>
        /// 批量冻结玩家
        /// </summary>
        protected void btnDongjie_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss=new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量冻结玩家";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Enable);
            int result = FacadeManage.aideAccountsFacade.NullityAccountInfo(StrCIdList, 1);
            if (result > 0)
            {
                ShowInfo("冻结成功");
                AccountsDataBind();
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
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量解冻玩家";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);


            //判断权限
            AuthUserOperationPermission(Permission.Enable);
            int result = FacadeManage.aideAccountsFacade.NullityAccountInfo(StrCIdList, 0);
            if (result > 0)
            {
                ShowInfo("解冻成功");
                AccountsDataBind();
            }
            else
            {
                ShowError("解冻失败");
            }
        }

        /// <summary>
        /// 批量设置转账权限
        /// </summary>
        protected void btnSetSingle_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量设置转账权限";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.TransferPower);
            int result = FacadeManage.aideAccountsFacade.TransferPowerAccounts(StrCIdList, 64);
            if (result > 0)
            {
                ShowInfo("设置成功");
                AccountsDataBind();
            }
            else
            {
                ShowError("设置失败");
            }
        }

        /// <summary>
        /// 批量取消转账权限
        /// </summary>
        protected void benCancleSingle_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量取消转账权限";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.TransferPower);
            int result = FacadeManage.aideAccountsFacade.TransferPowerAccounts(StrCIdList, 64, "^");
            if (result > 0)
            {
                ShowInfo("取消成功");
                AccountsDataBind();
            }
            else
            {
                ShowError("取消失败");
            }
        }

        /// <summary>
        /// 批量设置调试权限
        /// </summary>
        protected void btnSetDeBug_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量设置调试权限";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.TransferPower);
            int result = FacadeManage.aideAccountsFacade.TransferPowerAccounts(StrCIdList, 536870912);
            if (result > 0)
            {
                ShowInfo("设置成功");
                AccountsDataBind();
            }
            else
            {
                ShowError("设置失败");
            }
        }

        /// <summary>
        /// 批量取消调试权限
        /// </summary>
        protected void benCancleDeBug_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量取消调试权限";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.TransferPower);
            int result = FacadeManage.aideAccountsFacade.TransferPowerAccounts(StrCIdList, 536870912, "^");
            if (result > 0)
            {
                ShowInfo("取消成功");
                AccountsDataBind();
            }
            else
            {
                ShowError("取消失败");
            }
        }

        /// <summary>
        /// 设置所有人转账权限
        /// </summary>
        protected void btnSetting_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "设置所有人转账权限";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.TransferPower);
            int result = FacadeManage.aideAccountsFacade.TransferPowerAccounts(64);
            if (result > 0)
            {
                ShowInfo("设置成功");
                AccountsDataBind();
            }
            else
            {
                ShowError("设置失败");
            }
        }

        /// <summary>
        /// 取消所有人转账权限
        /// </summary>
        protected void btnCancle_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "取消所有人转账权限";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.TransferPower);
            int result = FacadeManage.aideAccountsFacade.TransferPowerAccounts(64, "^");
            if (result > 0)
            {
                ShowInfo("取消成功");
                AccountsDataBind();
            }
            else
            {
                ShowError("取消失败");
            }
        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void AccountsDataBind()
        {
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            int type = Convert.ToInt32(ddlSearchType.SelectedValue);

            StringBuilder condition = new StringBuilder(" WHERE 1=1");

            if (!string.IsNullOrEmpty(query))
            {
                if(type == 1)
                {
                    if (!Utils.Validate.IsPositiveInt(query))
                    {
                        ShowError("输入查询格式不正确");
                        return;
                    }
                    condition.AppendFormat(" AND GameID={0}", query);
                }
                else
                {
                    condition.AppendFormat(" AND NickName='{0}'", query);
                }
            }

            PagerSet pagerSet = FacadeManage.aideAccountsFacade.GetUserPageList(condition.ToString(), Orderby, anpPage.CurrentPageIndex, anpPage.PageSize);
            anpPage.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                SpreadData = pagerSet.PageSet.Tables[1];
                AgentUserData = pagerSet.PageSet.Tables[2];
                AgentInfoData = pagerSet.PageSet.Tables[3];
                rptDataList.DataSource = pagerSet.PageSet.Tables[0];
                rptDataList.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptDataList.Visible = pagerSet.RecordCount > 0;
        }
        /// <summary>
        /// 获取推广和代理
        /// </summary>
        protected string GetAgentSpread(int spreadid, int userid)
        {
            string info = string.Empty;
            DataRow[] rows = SpreadData.Select("UserID=" + spreadid);
            if (rows != null && rows.Length > 0)
            {
                info = rows[0]["NickName"].ToString() + "（" + rows[0]["GameID"].ToString() + "）" + "/";
            }
            rows = AgentUserData.Select("UserID=" + userid);
            if (rows != null && rows.Length > 0)
            {
                DataRow[] agents = AgentInfoData.Select("AgentID=" + rows[0]["AgentID"]);
                if (agents != null && agents.Length > 0)
                {
                    info = (string.IsNullOrEmpty(info) ? ("/") : info) + agents[0]["NickName"].ToString() + "（" + agents[0]["GameID"].ToString() + "）";
                }
            }
            return info;
        }

        /// <summary>
        /// 获取代理
        /// </summary>
        /// <param name="spreadid"></param>
        /// <param name="userid"></param>
        /// <returns></returns>
        //protected string GetAgentSpread(int spreadid, int userid)
        //{
        //    string info = string.Empty;
        //    DataRow[] rows = AgentUserData.Select("UserID=" + userid);
        //    if (rows != null && rows.Length > 0)
        //    {
        //        DataRow[] agents = AgentInfoData.Select("AgentID=" + rows[0]["AgentID"]);
        //        if (agents != null && agents.Length > 0)
        //        {
        //            info = agents[0]["NickName"].ToString() + "（" + agents[0]["GameID"].ToString() + "）";
        //        }
        //    }
        //    return info;
        //}

        /// <summary>
        /// 排序条件
        /// </summary>
        public string Orderby
        {
            get
            {
                if (ViewState["Orderby"] == null)
                {
                    ViewState["Orderby"] = "ORDER BY UserID DESC";
                }
                return (string) ViewState["Orderby"];
            }
            set { ViewState["Orderby"] = value; }
        }
    }
}
