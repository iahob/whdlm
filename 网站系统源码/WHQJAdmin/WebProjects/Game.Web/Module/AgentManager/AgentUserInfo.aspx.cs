using Game.Entity.Agent;
using Game.Entity.Enum;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using Game.Web.UI;
using System;
using Game.Entity.Accounts;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AgentManager
{
    public partial class AgentUserInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1001;
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "新增代理";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);


            AuthUserOperationPermission(Permission.Add);
            string agentNote = CtrlHelper.GetTextAndFilter(txtAgentNote);
            string compellation = CtrlHelper.GetTextAndFilter(txtCompellation);
            string address = CtrlHelper.GetTextAndFilter(txtContactAddress);
            string phone = CtrlHelper.GetTextAndFilter(txtContactPhone);
            string domain = CtrlHelper.GetTextAndFilter(txtDomain);
            int gameid = CtrlHelper.GetInt(txtGameID, 0);
            string qqaccount = CtrlHelper.GetTextAndFilter(txtQQAccount);
            string wxnickname = CtrlHelper.GetTextAndFilter(txtWCNickName);

            //判断用户是否存在
            AccountsInfo info = FacadeManage.aideAccountsFacade.GetAccountInfoByGameId(gameid);
            if(info==null || info.UserID <= 0)
            {
                MessageBox("游戏ID无效，请重新输入");
                return;
            }
            if(!string.IsNullOrEmpty(info.Compellation)&&!info.Compellation.Equals(compellation))
            {
                MessageBox("真实姓名与实名认证资料不符");
                return;
            }
            AgentInfo agent = new AgentInfo
            {
                AgentDomain = domain,
                AgentLevel = 1,
                AgentNote = agentNote,
                Compellation = compellation,
                ContactAddress = address,
                ContactPhone = phone,
                QQAccount = qqaccount,
                WCNickName = wxnickname,
                UserID = info.UserID
            };

            Message msg = FacadeManage.aideAgentFacade.InsertAgentUser(agent);
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