using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Web.UI;
using Game.Utils;
using Game.Entity.Treasure;
using Game.Facade;
using System.Data;
using Game.Entity.Agent;
using Game.Kernel;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AgentManager
{
    public partial class AgentReturnConfigInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1002;
            if (Page.IsPostBack) return;

            ddlAwardType.DataSource = EnumDescription.GetFieldTexts(typeof(AppConfig.AwardType));
            ddlAwardType.DataTextField = "Description";
            ddlAwardType.DataValueField = "EnumValue";
            ddlAwardType.DataBind();
            ddlAwardType.Items.Insert(0, new ListItem("请选择返利类型", "0"));
            BindData();
        }

        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            ReturnAwardConfig cfg = new ReturnAwardConfig();
            if (IntParam > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改代理返利配置";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Edit);
                cfg = FacadeManage.aideAgentFacade.GetAgentReturnConfig(IntParam);
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "新增代理返利配置";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Add);
            }
            cfg.AwardLevel = Convert.ToInt32(ddlLevel.SelectedValue);
            cfg.AwardScale = Convert.ToDecimal(txtPresentScale.Text) / 1000;
            cfg.AwardType = Convert.ToByte(ddlAwardType.SelectedValue);
            cfg.Nullity = Convert.ToBoolean(rblNullity.SelectedValue);
            cfg.UpdateTime = DateTime.Now;
            if (cfg.AwardLevel == 0)
            {
                ShowError("必须选择返利类型");
                return;
            }

            int result = FacadeManage.aideAgentFacade.SaveAgentReturnConfig(cfg);
            if (result > 0)
            {
                ShowInfo("操作成功", "AgentReturnConfigList.aspx", 1200);
            }
            else
            {
                ShowError("返利已存在或操作失败");
            }
        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            if (IntParam <= 0) return;
            ReturnAwardConfig config = FacadeManage.aideAgentFacade.GetAgentReturnConfig(IntParam);
            if (config == null) return;
            CtrlHelper.SetText(txtPresentScale, (config.AwardScale * 1000).ToString("F2"));
            ddlLevel.SelectedValue = config.AwardLevel.ToString();
            ddlAwardType.SelectedValue = config.AwardType.ToString();
            rblNullity.SelectedValue = config.Nullity.ToString();
            ddlAwardType.Enabled = false;
        }
    }
}
