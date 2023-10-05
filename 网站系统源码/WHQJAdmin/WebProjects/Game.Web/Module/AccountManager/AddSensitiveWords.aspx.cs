using System;
using System.Threading;
using Game.Entity.Record;
using Game.Facade;
using Game.Entity.Accounts;
using Game.Web.UI;
using Game.Entity.Enum;
using Game.Utils;
using Game.Kernel;
using Game.Socket;
using Game.Socket.DataStruct;


namespace Game.Web.Module.AccountManager
{
    public partial class AddSensitiveWords : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 500;
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
            string Words = CtrlHelper.GetTextAndFilter(txtWords);

            if (string.IsNullOrEmpty(Words))
            {
                MessageBox("敏感字符不能为空");
                return;
            }

            bool IsExist = FacadeManage.aidePlatformFacade.IsExsitSensitiveWords(Words);
            if (IsExist)
            {
                MessageBox("敏感字符已存在");
                return;
            }

            int result = FacadeManage.aidePlatformFacade.AddSensitiveWords(Words);
            if (result > 0)
            {
                MessageBox("录入成功");
            }
            else
            {
                MessageBox("录入失败，请稍后重试");
            }
        }
    }
}