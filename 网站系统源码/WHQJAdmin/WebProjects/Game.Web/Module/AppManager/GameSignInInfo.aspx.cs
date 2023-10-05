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
using Game.Entity.Platform;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AppManager
{
    public partial class GameSignInInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!Page.IsPostBack)
            {
                DataSet ds = FacadeManage.aidePlatformFacade.GetGamePackageList();
                ddlPackageID.DataSource = ds.Tables[0];
                ddlPackageID.DataValueField = "PackageID";
                ddlPackageID.DataTextField = "Name";
                ddlPackageID.DataBind();
                BindData();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            GameSignIn config = new GameSignIn();
            if (IntParam > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改签到配置";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Edit);
                config = FacadeManage.aidePlatformFacade.GetGameSignIn(IntParam);
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改签到配置";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);
            }


            config.PackageID = Convert.ToInt32(ddlPackageID.SelectedValue);
            config.TypeID = Convert.ToByte(ddlTypeID.SelectedValue);
            config.Probability = CtrlHelper.GetInt(txtProbability, 0);
            config.NeedDay = CtrlHelper.GetInt(txtNeedDay, 0);
            config.SortID = CtrlHelper.GetInt(txtSortID, 0);
            config.Nullity = Convert.ToByte(rblNullity.SelectedValue);

            //string filepath = upImage.FilePath;

            //if (string.IsNullOrEmpty(filepath))
            //{
            //    ShowError("广告图片未上传");
            //    return;
            //}

            //config.ResourceURL = filepath.Substring(7, filepath.Length - 7);
            int result = IntParam > 0 ? FacadeManage.aidePlatformFacade.UpdateGameSignIn(config) : FacadeManage.aidePlatformFacade.InsertGameSignIn(config);
            if (result > 0)
            {
                ShowInfo("操作成功", "GameSignInList.aspx", 1200);
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
            if (IntParam > 0)
            {
                GameSignIn config = FacadeManage.aidePlatformFacade.GetGameSignIn(IntParam);
                if (config != null)
                {
                    CtrlHelper.SetText(txtProbability, config.Probability.ToString());
                    CtrlHelper.SetText(txtNeedDay, config.NeedDay.ToString());
                    CtrlHelper.SetText(txtSortID, config.SortID.ToString());
                    rblNullity.SelectedValue = config.Nullity.ToString();
                    ddlTypeID.SelectedValue = config.TypeID.ToString();
                    ddlPackageID.SelectedValue = config.PackageID.ToString();
                   

                    //if (!string.IsNullOrEmpty(config.ResourceURL))
                    //{
                    //    upImage.FilePath = "/Upload" + config.ResourceURL;
                    //}
                }
            }
        }
    }
}