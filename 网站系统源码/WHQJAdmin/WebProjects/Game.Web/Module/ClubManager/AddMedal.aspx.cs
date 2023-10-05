using System;
using Game.Entity.Record;
using Game.Facade;
using Game.Entity.Accounts;
using Game.Web.UI;
using Game.Entity.Enum;
using Game.Utils;
using Game.Kernel;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.ClubManager
{
    public partial class AddMedal : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 902;
            if (!IsPostBack)
            {
                AuthUserOperationPermission(Permission.Add);
                trGroupID.Visible = IntParam <= 0;
                
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            int a = 0;
            if (!int.TryParse(txtMedal.Text, out a))
            {
                MessageBox("赠送勋章格式不正确");
                return;
            }
            int medal = a;
            int groupId = IntParam <= 0 ? CtrlHelper.GetInt(txtGroupID, 0) : IntParam;
            if (groupId <= 0)
            {
                MessageBox("请输入大联盟ID");
                return;
            }
            string ip = GameRequest.GetUserIP();
            Message msg = FacadeManage.aideGroupFacade.AddMedal(groupId, medal, ip, userExt.UserID);
            if (msg.Success)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = $"后台赠送勋章，大联盟ID：【{groupId}】";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                MessageBox("赠送成功");
            }
            else
            {
                MessageBox("赠送失败," + msg.Content);

            }
        }
    }
}