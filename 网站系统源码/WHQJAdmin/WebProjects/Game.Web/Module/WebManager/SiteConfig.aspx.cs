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
using Game.Entity.NativeWeb;
using Game.Entity.Accounts;
using Game.Entity.Enum;
using Game.Facade;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AppManager
{
    public partial class SiteConfig : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 400;
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
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "修改站点配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Edit);
            if(IntParam > 0)
            {
                ConfigInfo config = FacadeManage.aideNativeWebFacade.GetConfigInfo(IntParam);
                if(config != null)
                {
                    config.ConfigString = CtrlHelper.GetText(txtConfigString);
                    config.Field1 = CtrlHelper.GetText(txtField1);
                    config.Field2 = CtrlHelper.GetText(txtField2);
                    config.Field3 = CtrlHelper.GetText(txtField3);
                    config.Field4 = CtrlHelper.GetText(txtField4);
                    config.Field5 = CtrlHelper.GetText(txtField5);
                    config.Field6 = CtrlHelper.GetText(txtField6);
                    config.Field7 = CtrlHelper.GetText(txtField7);
                    config.Field8 = CtrlHelper.GetText(txtField8);
                    config.Field9=  CtrlHelper.GetText(txtField9);
                    config.Field10 = CtrlHelper.GetText(txtField10);
                    config.Field11 = CtrlHelper.GetText(txtField11);
                    config.Field12 = CtrlHelper.GetText(txtField12);
                    config.Field13 = CtrlHelper.GetText(txtField13);
                    int result = FacadeManage.aideNativeWebFacade.UpdateConfigInfo(config);
                    if(result > 0)
                    {
                        ShowInfo("配置修改成功");
                    }
                    else
                    {
                        ShowInfo("配置修改失败");
                    }
                }
            }
        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            if(IntParam > 0)
            {
                ConfigInfo config = FacadeManage.aideNativeWebFacade.GetConfigInfo(IntParam);
                if(config != null)
                {
                    CtrlHelper.SetText(txtConfigKey, config.ConfigKey);
                    CtrlHelper.SetText(txtConfigName, config.ConfigName);
                    CtrlHelper.SetText(txtConfigString, config.ConfigString);
                    CtrlHelper.SetText(txtField1, config.Field1);
                    CtrlHelper.SetText(txtField2, config.Field2);
                    CtrlHelper.SetText(txtField3, config.Field3);
                    CtrlHelper.SetText(txtField4, config.Field4);
                    CtrlHelper.SetText(txtField5, config.Field5);
                    CtrlHelper.SetText(txtField6, config.Field6);
                    CtrlHelper.SetText(txtField7, config.Field7);
                    CtrlHelper.SetText(txtField8, config.Field8);
                    CtrlHelper.SetText(txtField9, config.Field9);
                    CtrlHelper.SetText(txtField10, config.Field10);
                    CtrlHelper.SetText(txtField11, config.Field11);
                    CtrlHelper.SetText(txtField12, config.Field12);
                    CtrlHelper.SetText(txtField13, config.Field13);
                }
            }
        }
    }
}
