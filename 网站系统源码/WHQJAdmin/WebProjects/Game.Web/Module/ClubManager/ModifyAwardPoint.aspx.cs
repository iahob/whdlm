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
    public partial class ModifyAwardPoint : AdminPage
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

            int point = 0;
            if (!int.TryParse(txtPoint.Text, out point))
            {
                MessageBox("赠送勋章格式不正确");
                return;
            }

            if (point > 100)
            {
                MessageBox("点位不能超过100");
                return;
            }

            bool VerifyResult = FacadeManage.aideGroupFacade.CheckAwrdPoint(StrParam, point);
            if (VerifyResult)
            {
                MessageBox("修改点位不能大于原始点位，请稍后重试");
                return;
            }

            int result = FacadeManage.aideGroupFacade.ModifyAwardPoint(StrParam.ToString(), point);
            if (result>0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = $"修改大联盟点位，大联盟ID：【{StrParam}】";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                MessageBoxCloseRef("修改成功");
            }
            else
            {
                MessageBox("修改失败");

            }
        }
    }
}