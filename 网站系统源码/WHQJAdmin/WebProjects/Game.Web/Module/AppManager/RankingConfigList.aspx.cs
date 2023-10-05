using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Utils;
using Game.Kernel;
using Game.Web.UI;
using Game.Facade;
using Game.Entity.Enum;
using System.Data;
using System.Text;
using Game.Entity.Platform;
using Game.Entity.Treasure;
using Game.Entity.NativeWeb;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AppManager
{
    public partial class RankingConfigList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 305;
            if(!IsPostBack)
            {
                BindData();
            }
        }

        /// <summary>
        /// 获取排名
        /// </summary>
        protected string GetRankType(int typeid)
        {
            switch (typeid)
            {
                case 1:
                    return "第一名";
                case 2:
                    return "第二名";
                case 3:
                    return "第三名";
                case 4:
                    return "第4-10名";
                case 5:
                    return "第11-20名";
                case 6:
                    return "第21-50名";
                case 7:
                    return "第51-100名";
                default:
                    return "";
            }
        }

        /// <summary>
        /// 删除配置
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量删除排行配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission(Permission.Delete);
            if(StrCIdList.IndexOf(',') >= 0)
            {
                ShowError("只能选择单个配置进行删除");
                return;
            }
            int result = FacadeManage.aideNativeWebFacade.DeleteRankingConfig(Convert.ToInt32(StrCIdList));
            if(result > 0)
            {
                ShowInfo("删除成功", "RankingConfigList.aspx", 1200);
            }
            else
            {
                ShowError("删除失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            DataSet ds = FacadeManage.aideNativeWebFacade.GetRankingConfigList();
            litNoData.Visible = ds.Tables[0].Rows.Count > 0 ? false : true;
            rptDataList.DataSource = ds.Tables[0];
            rptDataList.DataBind();
        }
    }
}