using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Utils;
using Game.Web.UI;
using Game.Kernel;
using System.Text;
using Game.Entity.Platform;
using Game.Entity.Accounts;
using Game.Entity.Enum;
using Game.Facade;
using System.Data;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AppManager
{
    public partial class SystemSet : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 303;
            if(!Page.IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            AuthUserOperationPermission(Permission.Edit);

            SystemStatusInfo config = new SystemStatusInfo();
            config.StatusName = CtrlHelper.GetText(txtStatusName);
            config.StatusValue = CtrlHelper.GetInt(txtStatusValue, 0);
            config.StatusString = CtrlHelper.GetTextAndFilter(txtStatusString);
            config.StatusTip = CtrlHelper.GetTextAndFilter(txtStatusTip);
            config.StatusDescription = CtrlHelper.GetTextAndFilter(txtStatusDescription);

            int result = FacadeManage.aideAccountsFacade.UpdateSystemStatusInfo(config);
            if(result > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = $"修改了系统配置,配置项为【{config.StatusName}】";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                ShowInfo("修改成功");
            }
            else
            {
                ShowError("修改失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string status = GameRequest.GetQueryString("param");
            DataSet ds = FacadeManage.aideAccountsFacade.GetSystemStatusList();
            if (ds != null && ds.Tables.Count > 0)
            {
                DataTable table = ds.Tables[0];
                rptDataList.DataSource = table;
                rptDataList.DataBind();

                status = string.IsNullOrEmpty(status) ? "EnjoinLogon" : status;
                foreach (DataRow item in table.Rows)
                {
                    if(item["StatusName"].ToString() == status)
                    {
                        CtrlHelper.SetText(txtStatusName, item["StatusName"].ToString());
                        CtrlHelper.SetText(txtStatusValue, item["StatusValue"].ToString());
                        CtrlHelper.SetText(txtStatusTip, item["StatusTip"].ToString());
                        CtrlHelper.SetText(txtStatusString, item["StatusString"].ToString());
                        CtrlHelper.SetText(txtStatusDescription, item["StatusDescription"].ToString());
                        break;
                    }
                }
            }
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
                    ViewState["SearchItems"] = "WHERE StatusName NOT LIKE 'Club%' ";
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
                    ViewState["Orderby"] = "ORDER BY SortID ASC";
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
