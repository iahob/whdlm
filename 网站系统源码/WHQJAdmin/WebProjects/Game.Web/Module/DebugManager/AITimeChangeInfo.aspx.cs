using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Utils;
using Game.Entity.Enum;
using Game.Entity.Accounts;
using Game.Entity.Platform;
using Game.Socket;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.DebugManager
{
    public partial class AITimeChangeInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1303;
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
            ss.OperatingName = "AI人数调整";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AITimeChange atc = FacadeManage.aidePlatformFacade.GetAITimeChangeInfo(IntParam);
            if (atc == null)
            {
                ShowError("配置不存在");
            }

           
            atc.AICount = Convert.ToInt32(txtAICount.Text.Trim());

            int result = FacadeManage.aidePlatformFacade.SaveAITimeChangeInfo(atc);
            if (result > 0)
            {
                SocketManage.aideWebSocketFacade.UpdateAiWeightTotal(atc, isSent => { });
                ShowInfo("AI人数调整操作成功", "AITimeChangeList.aspx", 1000);
            }
            else
            {
                ShowError("AI人数调整操作失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void BindData()
        {
            AITimeChange atc = FacadeManage.aidePlatformFacade.GetAITimeChangeInfo(IntParam);
            if (atc != null)
            {
                CtrlHelper.SetText(txtAITime, atc.AITime.ToString());
                CtrlHelper.SetText(txtAICount, atc.AICount.ToString());
            }
        }
    }
}