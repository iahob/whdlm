using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Utils;
using Game.Entity.Treasure;
using Game.Facade;
using System.Data;
using Game.Kernel;
using Game.Entity.Enum;
using Game.Entity.NativeWeb;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AppManager
{
    public partial class RankingConfigInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 305;
            if(!Page.IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            RankingConfig config = new RankingConfig();

          
            config.TypeID = Convert.ToByte(ddlType.SelectedValue);
            config.RankType = Convert.ToInt32(ddlRank.SelectedValue);

            if (IntParam > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改排行配置";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Edit);
                config = FacadeManage.aideNativeWebFacade.GetRankingConfigById(IntParam);
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "新增排行配置";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Add);
                if (FacadeManage.aideNativeWebFacade.ExistRankingConfig(config.TypeID, config.RankType))
                {
                    ShowError("相同配置信息已存在");
                    return;
                }
            }
            config.AwardTicket = CtrlHelper.GetInt(txtAwardTicket, 0);
            config.Diamond = CtrlHelper.GetInt(txtDiamond, 0);
            config.Gold = Convert.ToInt64(CtrlHelper.GetTextAndFilter(txtGold));



            int result = 0;
            if(IntParam > 0)
            {
                result = FacadeManage.aideNativeWebFacade.UpdateRankingConfig(config);
            }
            else
            {
                result = FacadeManage.aideNativeWebFacade.InsertRankingConfig(config);
            }
            if(result > 0)
            {
                ShowInfo("操作成功", "RankingConfigList.aspx", 1200);
            }
            else
            {
                ShowError("操作失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            if(IntParam > 0)
            {
                RankingConfig config = FacadeManage.aideNativeWebFacade.GetRankingConfigById(IntParam);
                if(config != null)
                {
                    CtrlHelper.SetText(txtAwardTicket, config.AwardTicket.ToString());
                    CtrlHelper.SetText(txtDiamond, config.Diamond.ToString());
                    CtrlHelper.SetText(txtGold, config.Gold.ToString());
                    ddlType.SelectedValue = config.TypeID.ToString();
                    ddlRank.SelectedValue = config.RankType.ToString();
                }
            }
        }
    }
}