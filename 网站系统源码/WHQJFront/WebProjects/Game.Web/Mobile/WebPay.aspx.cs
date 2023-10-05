using Aop.Api;
using Aop.Api.Domain;
using Aop.Api.Request;
using Aop.Api.Response;
using Game.Entity.Treasure;
using Game.Facade;
using Game.Facade.DataStruct;
using Game.Kernel;
using Game.Utils;
using Game.Web.Helper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Game.Web.Mobile
{
    public partial class WebPay : System.Web.UI.Page
    {
        private static int ConfigId = 0;//产品ID
        private static int UserId = 0;
        private static int State = 0;//区分是大厅充值还是房间充值
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                ConfigId = GameRequest.GetInt("configid", 0);
                UserId = GameRequest.GetInt("userid", 0);
                State = GameRequest.GetInt("state", 0);
            }
        }

        protected void btnWxPay_Click(object sender, EventArgs e)
        {
            if (ConfigId == 0 || UserId == 0)
            {
                Response.Redirect("/404.html");
                return;
            }


            //下单信息
            OnLinePayOrder order = new OnLinePayOrder
            {
                UserID = UserId,
                ConfigID = ConfigId,
                OrderAddress = GameRequest.GetUserIP()
            };

            order.ShareID = 102;
            order.OrderID = Fetch.GetOrderIDByPrefix("HWX");

            //下单操作
            Message umsg = FacadeManage.aideTreasureFacade.CreatePayOrderInfo(order, "");
            if (umsg.Success)
            {
                OnLinePayOrder orderReturn = umsg.EntityList[0] as OnLinePayOrder;
                string domain = AppConfig.FrontSiteDomain;
                Log4Net.WriteInfoLog("domain：" + domain);
                Log4Net.WriteInfoLog("order.Amount：" + order.Amount);
                WxPayInfo wx = new WxPayInfo
                {
                    AppID = ApplicationSettings.Get("WXNATIVEAPPID"),
                    AppSecret = ApplicationSettings.Get("WXNATIVESECRET"),
                    // ReSharper disable once SpecifyACultureInStringConversionExplicitly
                    Body = orderReturn.Amount.ToString(),
                    Key = ApplicationSettings.Get("WXNATIVEKEY"),
                    Mchid = ApplicationSettings.Get("WXNATIVEMCHID"),
                    NotifyUrl = "http://" + domain + "/Notify/" + "WxWeb.aspx",
                    OpenId = "",
                    OutTradeNo = orderReturn.OrderID,
                    TotalFee = (orderReturn.Amount * 100).ToString("F0"),
                    TradeType = "MWEB"
                };
                WxPayHelper helper = new WxPayHelper(wx);
                string link = helper.GetWXPayLink();
                Log4Net.WriteInfoLog("微信支付链接：" + link);
                Response.Redirect(link);

            }

        }

        protected void btnAliPay_Click(object sender, EventArgs e)
        {
            //下单信息
            OnLinePayOrder order = new OnLinePayOrder
            {
                UserID = UserId,
                ConfigID = ConfigId,
                OrderAddress = GameRequest.GetUserIP()
            };

            order.ShareID = 201;
            order.OrderID = Fetch.GetOrderIDByPrefix("ZFBAPP");

            //下单操作
            Message umsg = FacadeManage.aideTreasureFacade.CreatePayOrderInfo(order, "");
            if (umsg.Success)
            {
                OnLinePayOrder orderReturn = umsg.EntityList[0] as OnLinePayOrder;
                string domain = AppConfig.FrontSiteDomain;
                DefaultAopClient client = new DefaultAopClient(AlipayHelper.gatewayUrl, AlipayHelper.app_id, AlipayHelper.private_key, "json", "1.0", AlipayHelper.sign_type, AlipayHelper.alipay_public_key, AlipayHelper.charset, false);

                // 外部订单号，商户网站订单系统中唯一的订单号
                string out_trade_no = orderReturn.OrderID;

                // 订单名称
                string subject = orderReturn.OrderID;

                // 付款金额
                string total_amout = orderReturn.Amount.ToString();

                // 商品描述
                string body = "";

                // 支付中途退出返回商户网站地址
                string quit_url = "";

                // 组装业务参数model
                AlipayTradeWapPayModel model = new AlipayTradeWapPayModel();
                model.Body = body;
                model.Subject = subject;
                model.TotalAmount = total_amout;
                model.OutTradeNo = out_trade_no;
                model.ProductCode = "QUICK_WAP_WAY";
                model.QuitUrl = quit_url;

                AlipayTradeWapPayRequest request = new AlipayTradeWapPayRequest();
                // 设置支付完成同步回调地址
                request.SetReturnUrl("http://" + domain + "/Notify/" + "H5Alipay.aspx");
                // 设置支付完成异步通知接收地址
                request.SetNotifyUrl("http://" + domain + "/Notify/" + "H5Alipay.aspx");
                // 将业务model载入到request
                request.SetBizModel(model);

                AlipayTradeWapPayResponse response = null;
                try
                {
                    response = client.pageExecute(request, null, "post");
                    Response.Write(response.Body);
                }
                catch (Exception exp)
                {
                    throw exp;
                }
            }
        }
    }
}