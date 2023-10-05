using System;
using Game.Entity.Record;
using Game.Facade;
using Game.Entity.Accounts;
using Game.Web.UI;
using Game.Entity.Enum;
using Game.Utils;
using Game.Kernel;
using Game.Socket;
using Game.Entity.Group;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.ClubManager
{
    public partial class ModifyMaxMemeberCount : AdminPage
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

            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(StrParam))
            {
                return;
            }

            int MaxMemeberCount = 0;
            if (!int.TryParse(txtMaxMemeberCount.Text, out MaxMemeberCount))
            {
                MessageBox("最大人数格式不正确");
                return;
            }

            bool VerifyResult = FacadeManage.aideGroupFacade.CheckMaxMemberCount(StrParam, MaxMemeberCount);
            if (VerifyResult)
            {
                MessageBox("最大人数不能小于当前人数，请稍后重试");
                return;
            }

            int res = FacadeManage.aideGroupFacade.ModifyMaxMemberCount(StrParam, MaxMemeberCount);
            if (res > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = $"修改大联盟最大人数，大联盟ID：【{StrParam}】";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                string[] Idlist = StrParam.Split(',');
                if (Idlist.Length > 0)
                {
                    foreach (var id in Idlist)
                    {
                        //SocketManage.aideWebSocketFacade.UpdateSuccessARE = new AutoResetEvent(false);
                        SocketManage.aideWebSocketFacade.UpdateGroupMaxMember(Convert.ToInt32(id), Convert.ToInt16(MaxMemeberCount), isSent => { });
                        //SocketManage.aideWebSocketFacade.UpdateSuccessARE.WaitOne();
                    }
                    MessageBoxCloseRef("修改成功");
                }
                else
                {
                    MessageBox("参数错误");
                }

               
            }
        }
    }
}