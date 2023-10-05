using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Utils;
using Game.Web.UI;
using Game.Kernel;
using Game.Facade;
using Game.Entity.NativeWeb;
using System.Data;
using Game.Entity.Accounts;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.MallManager
{
    public partial class MallOrder : AdminPage
    {
        #region 窗口事件
        protected void Page_Load( object sender, EventArgs e )
        {
            base.moduleID = 1102;
            if( !IsPostBack )
            {
                Inition();
            }
        }
        /// <summary>
        /// 订单处理
        /// </summary>
        protected void btnConfirm_Click( object sender, EventArgs e )
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "商品订单处理";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            if (!string.IsNullOrEmpty( StrParam))
            {
                AuthUserOperationPermission( Permission.OrderOperating );
                AwardOrder order = FacadeManage.aideNativeWebFacade.GetAwardOrderByID(StrParam);
                Button bt = sender as Button;
                int state = int.Parse( bt.CommandArgument );

                //防刷新
                if( state == order.OrderStatus )
                {
                    return;
                }

                string log = string.Format( "【 {0}{1} 操作人：{2}】<br/>", DateTime.Now, state == 1 ? "确认货已发出 " : state == 2 ? "确认客户已收货 " :
                   state == 4 ? "同意退货" : state == 5 ? "拒绝退货" : state == 6 ? "退货成功并退款" : "", userExt.UserName);
                int num = FacadeManage.aideNativeWebFacade.UpdateOrderState( state, StrParam);
                if( num > 0 )
                {
                   
                    Inition();
                }
                else
                {
                    ShowError( "订单处理失败" );
                }
            }
        }
        #endregion

        #region 窗口方法
        /// <summary>
        /// 数据初始化
        /// </summary>
        private void Inition()
        {
            if(!string.IsNullOrEmpty(StrParam))
            {
                AwardOrder order = FacadeManage.aideNativeWebFacade.GetAwardOrderByID(StrParam);
                btnConfirm.Visible = true;
                //btnRefused.Visible = false;
                btnConfirm2.Visible = true;
                //btnRefused2.Visible = false;

                //设置按钮属性
                switch( order.OrderStatus )
                {
                    case 0:
                        btnConfirm.Text = "确认已发货";
                        btnConfirm.CommandArgument = "1";
                        btnConfirm2.Text = "确认已发货";
                        btnConfirm2.CommandArgument = "1";
                        break;
                    case 1:
                        btnConfirm.Text = "确认客户已收货";
                        btnConfirm.CommandArgument = "2";
                        btnConfirm2.Text = "确认客户已收货";
                        btnConfirm2.CommandArgument = "2";
                        break;
                    case 2:
                        btnConfirm.Visible = false;
                        btnConfirm2.Visible = false;
                        break;
                
                    case 5:
                        btnConfirm.Visible = false;
                        btnConfirm2.Visible = false;
                        break;
                    case 6:
                        btnConfirm.Visible = false;
                        btnConfirm2.Visible = false;
                        break;
                }
                string OrderStatus = "";
                if (order.OrderStatus == 0)
                {
                    OrderStatus = "处理中";
                }
                else if(order.OrderStatus == 1) {
                    OrderStatus = "已发货";
                }
                else if (order.OrderStatus == 2)
                {
                    OrderStatus = "已收货";
                }


                CtrlHelper.SetText( lbOrderStatus, OrderStatus);
                CtrlHelper.SetText( lbAwardCount, order.BuyNum.ToString() );
                CtrlHelper.SetText( lbAwardPrice, order.UnitPrice.ToString() );
                CtrlHelper.SetText( lbAwardID, FacadeManage.aideNativeWebFacade.GetAwardInfoByID( order.AwardID ).AwardName );
                CtrlHelper.SetText( lbBuyDate, order.CollectDate.ToString() );
                CtrlHelper.SetText( lbBuyIP, order.ClinetIP);
                CtrlHelper.SetText( lbCompellation, order.Compellation );
                CtrlHelper.SetText( lbDwellingPlace, order.Province+ order.City+ order.Area+ order.DetailAddress);
                CtrlHelper.SetText( lbMobilePhone, order.MobilePhone );
                CtrlHelper.SetText( lbOrderID, order.OrderID.ToString() );
                CtrlHelper.SetText( lbTotalAmount, order.PayTicket.ToString() );
                CtrlHelper.SetText(lbAwardGold, order.Gold.ToString());
                CtrlHelper.SetText(lbAwardDiamond, order.Diamond.ToString());
                CtrlHelper.SetText( lbUserID, GetAccounts( order.UserID ) );
            }
        }
        #endregion
    }
}