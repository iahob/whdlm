using Game.Entity.Accounts;
using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using Game.Web.UI;
using System;
using System.Data;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Entity.Agent;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AgentManager
{
    public partial class AgentUserUpdate : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1001;
            if(!IsPostBack)
            {
                if(IntParam > 0)
                {
                    AgentInfo info = FacadeManage.aideAgentFacade.GetAgentInfo(IntParam);
                    if(info != null)
                    {
                        AccountsInfo accounts = FacadeManage.aideAccountsFacade.GetAccountInfoByUserId(info.UserID);
                        CtrlHelper.SetText(txtAgentNote, info.AgentNote);
                        CtrlHelper.SetText(txtCompellation, info.Compellation);
                        CtrlHelper.SetText(txtContactAddress, info.ContactAddress);
                        CtrlHelper.SetText(txtContactPhone, info.ContactPhone);
                        CtrlHelper.SetText(txtDomain, info.AgentDomain);
                        CtrlHelper.SetText(txtQQAccount, info.QQAccount);
                        CtrlHelper.SetText(txtWCNickName, accounts.NickName);
                    }
                }
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
            ss.OperatingName = "修改代理";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission(Permission.Edit);
            if(IntParam > 0)
            {
                AgentInfo info = FacadeManage.aideAgentFacade.GetAgentInfo(IntParam);
                if(info != null)
                {
                    info.AgentNote = CtrlHelper.GetTextAndFilter(txtAgentNote);
                    info.Compellation = CtrlHelper.GetTextAndFilter(txtCompellation);
                    info.ContactAddress = CtrlHelper.GetTextAndFilter(txtContactAddress);
                    info.ContactPhone = CtrlHelper.GetTextAndFilter(txtContactPhone);
                    info.AgentDomain = CtrlHelper.GetTextAndFilter(txtDomain);
                    info.QQAccount = CtrlHelper.GetTextAndFilter(txtQQAccount);
                    info.WCNickName = CtrlHelper.GetTextAndFilter(txtWCNickName);
                    string password = CtrlHelper.GetTextAndFilter(txtPassword);
                    if (!string.IsNullOrEmpty(password)) info.Password = Utility.MD5(password);

                    AccountsInfo accounts = FacadeManage.aideAccountsFacade.GetAccountInfoByUserId(info.UserID);
                    if(accounts == null || accounts.UserID <= 0)
                    {
                        MessageBox("代理信息异常，请联系管理员");
                        return;
                    }
                    if(!string.IsNullOrEmpty(accounts.Compellation) && !accounts.Compellation.Equals(info.Compellation))
                    {
                        MessageBox("真实姓名与实名认证资料不符");
                        return;
                    }
                    if(!string.IsNullOrEmpty(accounts.NickName) && !accounts.NickName.Equals(info.WCNickName))
                    {
                        MessageBox("微信昵称与真实昵称不符");
                        return;
                    }

                    int result = FacadeManage.aideAgentFacade.UpdateAgentUser(info);
                    if(result > 0)
                    {
                        MessageBoxCloseRef("修改成功");
                    }
                    else
                    {
                        MessageBox("修改失败");
                    }
                }
            }
        }
    }
}