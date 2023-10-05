using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Text;

using Game.Web.UI;
using Game.Kernel;
using Game.Utils;
using Game.Entity.Enum;
using Game.Facade;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.BackManager
{
    public partial class BaseRoleList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 800;
            if(!IsPostBack)
            {
                GameRoleDataBind();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            GameRoleDataBind();
        }

        /// <summary>
        /// 批量删除角色
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量删除角色";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Delete);
            int result = FacadeManage.aidePlatformManagerFacade.DeleteRole(StrCIdList);
            if(result > 0)
            {
                ShowInfo("删除成功");
                GameRoleDataBind();
            }
            else
            {
                ShowError("删除失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void GameRoleDataBind()
        {
            PagerSet pagerSet = FacadeManage.aidePlatformManagerFacade.GetList(Base_Roles.Tablename, "", "ORDER BY RoleID ASC",
                anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                rptRole.DataSource = pagerSet.PageSet;
                rptRole.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptRole.Visible = pagerSet.RecordCount > 0;
        }               
    }
}
