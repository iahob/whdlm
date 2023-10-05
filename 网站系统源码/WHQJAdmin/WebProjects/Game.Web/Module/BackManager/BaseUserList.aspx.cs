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
using System.Data;

namespace Game.Web.Module.BackManager
{
    public partial class BaseUserList : AdminPage
    {
        private DataTable roletable;
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 800;
            if(!IsPostBack)
            {
                UserDataBind();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            UserDataBind();
        }
        /// <summary>
        /// 批量删除用户
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量删除系统账号";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Delete);
            int result = FacadeManage.aidePlatformManagerFacade.DeleteUser(StrCIdList);
            if(result > 0)
            {
                ShowInfo("删除成功");
                UserDataBind();
            }
            else
            {
                ShowError("删除失败");
            }
        }
        /// <summary>
        /// 批量冻结用户
        /// </summary>
        protected void btnDongjie_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量冻结系统账号";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Enable);
            int result = FacadeManage.aidePlatformManagerFacade.NullityUser(StrCIdList, 1);
            if(result > 0)
            {
                ShowInfo("冻结成功");
                UserDataBind();
            }
            else
            {
                ShowError("冻结失败");
            }
        }
        /// <summary>
        /// 批量解冻玩家
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnJiedong_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量解冻系统账号";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Enable);
            int result = FacadeManage.aidePlatformManagerFacade.NullityUser(StrCIdList, 0);
            if(result > 0)
            {
                ShowInfo("解冻成功");
                UserDataBind();
            }
            else
            {
                ShowError("解冻失败");
            }
        }
//        /// <summary>
//        /// 获取角色名称
//        /// </summary>
//        /// <param name="roleID">角色标识</param>
//        /// <returns></returns>
//        protected string GetRoleName(int roleID)
//        {
//            if(roletable!=null&& roletable.Rows.Count > 0)
//            {
//                DataRow[] rows = roletable.Select("RoleID=" + roleID);
//                if (rows != null && rows.Length > 0)
//                {
//                    return rows[0]["RoleName"].ToString();
//                }
//            }
//            return "";
//        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void UserDataBind()
        {
            PagerSet pagerSet = FacadeManage.aidePlatformManagerFacade.GetList(Base_Users.Tablename, "", "ORDER BY UserID ASC",
                anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                roletable = FacadeManage.aidePlatformManagerFacade.GetRoleList().Tables[0];
                rptUser.DataSource = pagerSet.PageSet;
                rptUser.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptUser.Visible = pagerSet.RecordCount > 0;
        }
    }
}
