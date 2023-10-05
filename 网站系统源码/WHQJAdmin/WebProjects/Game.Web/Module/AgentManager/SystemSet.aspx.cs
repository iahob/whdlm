using System;
using Game.Utils;
using Game.Web.UI;
using Game.Kernel;
using Game.Entity.Agent;
using Game.Entity.Enum;
using Game.Facade;
using System.Data;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AgentManager
{
    public partial class SystemSet : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1000;
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

            SystemStatusInfo config = new SystemStatusInfo
            {
                StatusName = CtrlHelper.GetText(txtStatusName),
                StatusValue = CtrlHelper.GetInt(txtStatusValue, 0),
                StatusString = CtrlHelper.GetTextAndFilter(txtStatusString),
                StatusTip = CtrlHelper.GetTextAndFilter(txtStatusTip),
                StatusDescription = CtrlHelper.GetTextAndFilter(txtStatusDescription)
            };

            int result = FacadeManage.aideAgentFacade.UpdateSystemStatusInfo(config);
            if(result > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = $"修改代理配置，操作项：【{config.StatusName}】";
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
            DataSet ds = FacadeManage.aideAgentFacade.GetSystemStatusList();
            if (ds != null && ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
            {
                rptDataList.DataSource = ds.Tables[0];
                rptDataList.DataBind();

                SystemStatusInfo config = FacadeManage.aideAgentFacade.GetSystemStatusInfo(string.IsNullOrEmpty(StrParam) ? "AgentAwardType" : StrParam);
                if (config != null)
                {
                    CtrlHelper.SetText(txtStatusName, config.StatusName);
                    CtrlHelper.SetText(txtStatusValue, config.StatusValue.ToString());
                    CtrlHelper.SetText(txtStatusTip, config.StatusTip);
                    CtrlHelper.SetText(txtStatusString, config.StatusString);
                    CtrlHelper.SetText(txtStatusDescription, config.StatusDescription);
                }
            }
        }
    }
}
