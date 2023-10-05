using Game.Entity.Accounts;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using Game.Web.UI;
using System;

namespace Game.Web.Module.AccountManager
{
    public partial class AddSuperUser : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 100;
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "添加超管";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission(Permission.Add);
            AccountsInfo info = new AccountsInfo()
            {
                Accounts = CtrlHelper.GetTextAndFilter(txtAccounts),
                LogonPass = Utility.MD5(CtrlHelper.GetTextAndFilter(txtPassword)),
                UserRight = CtrlHelper.GetInt(txtGrantGold,0)
            };

            Message msg = FacadeManage.aideAccountsFacade.InsertSuperUser(info);
            if(msg.Success)
            {
                MessageBoxCloseRef(msg.Content);
            }
            else
            {
                MessageBox(msg.Content);
            }
        }
    }
}