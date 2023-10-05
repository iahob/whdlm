using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Entity.Accounts;
using Game.Web.UI;
using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Utils;
using Game.Entity.Enum;
using Game.Entity.Group;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.ClubManager
{
    public partial class GroupWhiteInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 905;
            if (!IsPostBack)
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
            ss.OperatingName = $"增加大联盟白名单";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission(Permission.Add);

            if (string.IsNullOrEmpty(hidUserID.Value))
            {
                MessageBox("添加失败，请点击检测");
                return;
            }

            int userid = Convert.ToInt32(hidUserID.Value);
            IMGroupwhiteList imw = FacadeManage.aideGroupFacade.GetIMGroupwhiteInfo(userid);
            if (imw != null)
            {
                MessageBox("已存在白名单，请勿重复添加");
                return;
            }

            IMGroupwhiteList model= new IMGroupwhiteList();
            model.UserID = Convert.ToInt64(userid);
      
            int result = FacadeManage.aideGroupFacade.InsertIMGroupwhiteInfo(model);
            if (result > 0)
            {
                ShowInfo("白名单添加成功", "GroupWhiteList.aspx", 1000);
            }
            else
            {
                ShowError("白名单添加失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void BindData()
        {
            if (IntParam > 0)
            {
                IMGroupwhiteList model = FacadeManage.aideGroupFacade.GetIMGroupwhiteInfo(IntParam);
                if (model != null)
                {
                    AccountsInfo info = FacadeManage.aideAccountsFacade.GetAccountInfoByUserId(IntParam);
                    CtrlHelper.SetText(txtGameID, info.GameID.ToString());
                    CtrlHelper.SetText(hidUserID, info.UserID.ToString());
                }
            }
        }
    }
}