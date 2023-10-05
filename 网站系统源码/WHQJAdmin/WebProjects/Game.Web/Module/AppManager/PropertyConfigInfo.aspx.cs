using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Utils;
using Game.Facade;
using Game.Kernel;
using System.Collections;
using System.Text;
using Game.Entity.Enum;
using Game.Entity.Platform;

namespace Game.Web.Module.AppManager
{
    public partial class PropertyConfigInfo : AdminPage
    {
        #region 窗口事件

        protected void Page_Load( object sender, EventArgs e )
        {
            base.moduleID = 306;
            if ( !Page.IsPostBack )
            {
                //BindArea( );
                BindData( );
            }
        }
        protected void btnSave_Click( object sender, EventArgs e )
        {
            //判断权限
            AuthUserOperationPermission(Permission.Edit);

            GameProperty property = new GameProperty();
            property.ID = IntParam;
            property.ExchangeType = (byte)GameRequest.GetQueryInt("type",0);
            property.BuyResultsGold = CtrlHelper.GetInt(txtBuyResultsGold, 0);
            property.ExchangeRatio = CtrlHelper.GetInt(txtPrice, 0);
            property.Name = CtrlHelper.GetTextAndFilter(txtName);
            property.Nullity = Convert.ToByte(ddlSate.SelectedValue);
            property.RegulationsInfo = CtrlHelper.GetTextAndFilter(txtRegulationsInfo);
            property.SortID = CtrlHelper.GetInt(txtSortID, 0);
            property.UseResultsGold = CtrlHelper.GetInt(txtUseResultsGold, 0);
            property.BuyResultsGold = CtrlHelper.GetInt(txtBuyResultsGold, 0);

            int result = FacadeManage.aidePlatformFacade.UpdatePropertyInfo(property);
            if (result > 0)
            {
                ShowInfo("更新成功", "PropertyConfigList.aspx", 500);
            }
            else
            {
                ShowError("更新失败");
            }
        }
        #endregion

        #region 数据加载

        private void BindData( )
        {
            int typeid = GameRequest.GetQueryInt("type", 0);
            if(IntParam <= 0)
                return;

            GameProperty property = FacadeManage.aidePlatformFacade.GetPropertyInfo(IntParam, typeid);
            if(property == null)
                return;

            CtrlHelper.SetText(txtName, property.Name);
            CtrlHelper.SetText(txtBuyResultsGold, property.BuyResultsGold.ToString());
            CtrlHelper.SetText(txtUseResultsGold, property.UseResultsGold.ToString());
            CtrlHelper.SetText(txtRegulationsInfo, property.RegulationsInfo);
            CtrlHelper.SetText(txtSortID, property.SortID.ToString());
            CtrlHelper.SetText(txtPrice, property.ExchangeRatio.ToString());
            ddlType.SelectedValue = property.ExchangeType.ToString();
            ddlType.Enabled = false;
            ddlSate.SelectedValue = property.Nullity.ToString();

            //禁止用户操作
            NulityInput(property.Kind);
        }
        /// <summary>
        /// 禁止用户操作
        /// </summary>
        /// <param name="Kind"></param>
        public void NulityInput(int Kind)
        {
            switch(Kind)
            {
                case 1:
                    txtUseResultsGold.Enabled = false;
                    txtBuyResultsGold.Enabled = false;
                    break;
                case 2:
                    txtBuyResultsGold.Enabled = false;
                    txtUseResultsGold.Enabled = true;
                    break;
                case 3:
                    txtUseResultsGold.Enabled = false;
                    txtBuyResultsGold.Enabled = false;
                    break;
                case 4:
                    txtUseResultsGold.Enabled = false;
                    txtBuyResultsGold.Enabled = false;
                    break;
                case 5:
                    txtUseResultsGold.Enabled = false;
                    txtBuyResultsGold.Enabled = false;
                    break;
                case 6:
                    txtBuyResultsGold.Enabled = true;
                    txtUseResultsGold.Enabled = false;
                    break;
                case 12:
                    txtBuyResultsGold.Enabled = false;
                    break;
                default:
                    txtUseResultsGold.Enabled = false;
                    txtBuyResultsGold.Enabled = false;
                    break;
            }
        }
        #endregion
    }
}
