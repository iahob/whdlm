using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Text;
using Game.Facade;
using Game.Entity.Treasure;
using Game.Web.Helper;

namespace Game.Web.Notify
{
    public partial class PaysAPI : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                Log4Net.WriteInfoLog("收到PaysAPI回调,paysapi_id:：" + Request.Form["paysapi_id"]);
                string token = "";
                string paysapi_id = Request.Form["paysapi_id"];//一定存在。一个24位字符串，是此订单在PaysApi服务器上的唯一编号
                string orderid= Request.Form["orderid"];//一定存在。是您在发起付款接口传入的您的自定义订单号
                string price= Request.Form["price"];//一定存在。是您在发起付款接口传入的订单价格
                string realprice= Request.Form["realprice"];//一定存在。表示用户实际支付的金额。一般会和price值一致，如果同时存在多个用户支付同一金额，就会和price存在一定差额，差额一般在1-2分钱上下，越多人同时付款，差额越大。
                string orderuid = Request.Form["orderuid"];//如果您在发起付款接口带入此参数，我们会原封不动传回。
                string key= Request.Form["key"];//一定存在。我们把使用到的所有参数，连您的Token一起，按参数名字母升序排序。把参数值拼接在一起。做md5-32位加密，取字符串小写。得到key。您需要在您的服务端按照同样的算法，自己验证此key是否正确。只在正确时，执行您自己逻辑中支付成功代码。
                                                //key的拼接顺序：如用到了所有参数，就按这个顺序拼接：orderid + orderuid + paysapi_id + price + realprice + token
                string sin = orderid + orderuid + paysapi_id + price + realprice + token;
                sin =Fetch.EncryptMD5(sin,true,32);
                Log4Net.WriteInfoLog("key：" + key+ "   sin:"+ sin);
                if (sin== key)
                {
                    OnLinePayOrder order = new OnLinePayOrder
                    {
                        OrderID = orderid,
                        Amount = Convert.ToDecimal(price)
                    };
                    FacadeManage.aideTreasureFacade.FinishOnLineOrder(order);
                    Response.Write( "SUCCESS");
                }
                else
                {
                    Response.Write("FAIL");
                }
            }

        }
    }
}