using Game.Entity.Treasure;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Facade;
using Game.Utils;
using Game.Web.Helper;
using Game.Kernel;

namespace Game.Web.Notify
{
    public partial class jftycPay : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (IsPostBack) return;
            JFTYCModel.NoticeModel model=new JFTYCModel.NoticeModel();
            Helper.JFTYCPay.JFTYCNotify notify = new JFTYCPay.JFTYCNotify(GameRequest.Request, out model);

            Log4Net.WriteInfoLog($"VerifySign:{notify.VerifySign()},status:{model.p4_status}");
            //验证签名且支付状态为成功
            if (notify.VerifySign() && model.p4_status == "1")
            {
                OnLinePayOrder order = new OnLinePayOrder
                {
                    OrderID = model.p2_order,
                    Amount = Convert.ToDecimal(model.p3_ordermoney)/100
                };

                Log4Net.WriteInfoLog($"orderID:{order.OrderID},Amount:{order.Amount}");
                Message msg = FacadeManage.aideTreasureFacade.FinishOnLineOrder(order);
                Log4Net.WriteInfoLog($"status:{msg.Success},content:{msg.Content}");
                Response.Write("success");
            }
            else
            {
                Response.Write("fail");
            }
        }
    }
}