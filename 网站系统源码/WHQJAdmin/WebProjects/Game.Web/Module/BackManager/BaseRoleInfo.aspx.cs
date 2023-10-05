﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Utils;
using Game.Kernel;
using Game.Entity.PlatformManager;
using Game.Entity.Enum;
using Game.Facade;


namespace Game.Web.Module.BackManager
{
    public partial class BaseRoleInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 800;
            if(!Page.IsPostBack)
            {
                GameRoleDataBind();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            Base_Roles role = new Base_Roles();
            if(IntParam > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改角色";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Edit);
                role = FacadeManage.aidePlatformManagerFacade.GetRoleInfo(IntParam);
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "新增角色";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Add);
            }
            role.RoleName = CtrlHelper.GetTextAndFilter(txtRoleName);
            role.Description = CtrlHelper.GetTextAndFilter(txtDescription);

            int result = IntParam>0? FacadeManage.aidePlatformManagerFacade.UpdateRole(role) : FacadeManage.aidePlatformManagerFacade.InsertRole(role);
            if(result > 0)
            {
                ShowInfo("角色信息操作成功", "BaseRoleList.aspx", 1200);
            }
            else
            {
                ShowError("角色信息操作失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void GameRoleDataBind()
        {
            if(IntParam > 0)
            {
                Base_Roles role = FacadeManage.aidePlatformManagerFacade.GetRoleInfo(IntParam);
                if(role != null)
                {
                    CtrlHelper.SetText(txtRoleName, role.RoleName);
                    CtrlHelper.SetText(txtDescription, role.Description);
                }
            }
        }     
    }
}
