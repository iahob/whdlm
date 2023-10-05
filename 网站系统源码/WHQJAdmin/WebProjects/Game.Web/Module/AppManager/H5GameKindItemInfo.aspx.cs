using Game.Entity.Enum;
using Game.Entity.Platform;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using Game.Web.UI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Game.Web.Module.AppManager
{
    public partial class H5GameKindItemInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 301;
            if (!Page.IsPostBack)
            {
                DataBindKindList();
                H5MobileKindItemDataBind();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            H5GameKindItem model = null;
            int kindid = Convert.ToInt32(ddlKind.SelectedValue);
            if (IntParam > 0)
            {
                AuthUserOperationPermission(Permission.Edit);
                model = FacadeManage.aidePlatformFacade.GetH5GameKindItemInfo(IntParam);
            }
            else
            {
                AuthUserOperationPermission(Permission.Add);
                model = FacadeManage.aidePlatformFacade.GetH5GameKindItemInfo(kindid);
                if (model != null)
                {
                    ShowError("添加的游戏已存在");
                    return;
                }
                model = new H5GameKindItem();
                model.KindMark = 3;
                model.Nullity = 0;
            }

            model.KindID = kindid;
            model.KindName = ddlKind.SelectedItem.Text;
            model.ModuleName = CtrlHelper.GetTextAndFilter(txtModuleName);
            model.ClientVersion = CalVersion2(CtrlHelper.GetTextAndFilter(txtClientVersion));
            model.ResVersion = CtrlHelper.GetInt(txtResVersion, 0);
            model.SortID = CtrlHelper.GetInt(txtSortID, 0);

            int result = IntParam > 0 ? FacadeManage.aidePlatformFacade.UpdateH5GameKindItem(model) : FacadeManage.aidePlatformFacade.InsertH5GameKindItem(model);
            if (result > 0)
            {
                ShowInfo("操作成功", "MobileKindList.aspx", 1200);
            }
            else
            {
                ShowError("操作失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void H5MobileKindItemDataBind()
        {
            if (IntParam > 0)
            {
                H5GameKindItem model = FacadeManage.aidePlatformFacade.GetH5GameKindItemInfo(IntParam);
                if (model != null)
                {
                    ddlKind.SelectedValue = model.KindID.ToString();
                    CtrlHelper.SetText(txtModuleName, model.ModuleName.Trim());
                    CtrlHelper.SetText(txtClientVersion, CalVersion(model.ClientVersion));
                    CtrlHelper.SetText(txtResVersion, model.ResVersion.ToString());
                    CtrlHelper.SetText(txtSortID, model.SortID.ToString().Trim());
                }
            }
        }
        /// <summary>
        /// 下拉列表数据
        /// </summary>
        private void DataBindKindList()
        {
            PagerSet ps = FacadeManage.aidePlatformFacade.GetList(GameGameItem.Tablename, "", "ORDER BY GameID DESC", 1, 1000);
            ddlKind.DataSource = ps.PageSet;
            ddlKind.DataTextField = "GameName";
            ddlKind.DataValueField = "GameID";
            ddlKind.DataBind();
        }
    }
}