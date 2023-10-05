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
using Game.Entity.Agent;
using Game.Entity.Platform;
using Game.Entity.Treasure;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AgentManager
{
    public partial class AgentReturnConfigList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1002;
            if(!IsPostBack)
            {
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
        /// 配置删除
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量删除代理返利配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission(Permission.Delete);
            if(StrCIdList.IndexOf(',') >= 0)
            {
                ShowError("只能选择单个配置进行删除");
                return;
            }
            int result = FacadeManage.aideAgentFacade.DeleteAgentReturnConfig(StrCIdList);
            if(result > 0)
            {
                ShowInfo("删除成功", "AgentReturnConfigList.aspx", 1200);
            }
            else
            {
                ShowError("删除失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            DataSet ds = FacadeManage.aideAgentFacade.GetAgentReturnConfigList();
            if (ds != null && ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
            {
                rptDataList.DataSource = ds.Tables[0];
                rptDataList.DataBind();
            }
            litNoData.Visible = ds.Tables[0].Rows.Count <= 0;
            rptDataList.Visible = ds.Tables[0].Rows.Count > 0;
        }
    }
}