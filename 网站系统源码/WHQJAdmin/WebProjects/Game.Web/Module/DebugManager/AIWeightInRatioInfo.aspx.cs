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
    public partial class AIWeightInRatioInfo : AdminPage
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
            ss.OperatingName = "进入房间权重修改";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AIWeightInRatio aiwitr = FacadeManage.aidePlatformFacade.GetAIWeightInRatioInfo(IntParam);
            if (aiwitr == null)
            {
                ShowError("配置不存在");
            }

            aiwitr.TimeCount = Convert.ToInt32(txtTimeCount.Text.Trim());
            aiwitr.WeightIn = Convert.ToInt32(txtWeightIn.Text.Trim());

            try
            {
                int result = FacadeManage.aidePlatformFacade.SaveAIWeightInRatioInfo(aiwitr);
                if (result > 0)
                {
                    SocketManage.aideWebSocketFacade.UpdateGroupAIWeightIn(aiwitr,isSent => { });
                    ShowInfo("进入房间权重修改操作成功", "AIWeightInRatioList.aspx", 1000);
                }
                else
                {
                    ShowError("进入房间权重修改操作失败");
                }
            }
            catch (Exception)
            {
                
                ShowError("操作失败,不允许插入重复的AI时间");
            }

        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void BindData()
        {
            if (IntParam > 0)
            {
                AIWeightInRatio aiwitr = FacadeManage.aidePlatformFacade.GetAIWeightInRatioInfo(IntParam);
                if (aiwitr != null)
                {
                    CtrlHelper.SetText(txtTimeCount, aiwitr.TimeCount.ToString());
                    CtrlHelper.SetText(txtWeightIn, aiwitr.WeightIn.ToString());
                }
            }
        }
    }
}