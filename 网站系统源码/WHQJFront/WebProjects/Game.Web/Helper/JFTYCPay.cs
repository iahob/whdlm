using CsharpHttpHelper;
using Game.Entity.Treasure;
using Game.Facade;
using Game.Utils;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Security.Cryptography;
using System.Security.Policy;
using System.Text;
using System.Threading;
using System.Web;

namespace Game.Web.Helper
{
    public static class JFTYCPay
    {
        /// <summary>
        /// 竣付通云C平台配置
        /// </summary>
        public static class Config
        {
            public const string JFTYCUrl = "http://eee.hft-pay.cn/api/order";//提单地址
            public static string JFTYCMerchant = AppConfig.GetSetting("JFTYCMerchant");//商户号
            public static string JFTYCKEY = AppConfig.GetSetting("JFTYCKEY");//支付秘钥
        }

        public class  JFTYCRequest
        {

            /// <summary>
            /// 商户号
            /// </summary>
            public string p1_merchant => Config.JFTYCMerchant;
            /// <summary>
            /// 订单号
            /// </summary>
            public string p2_order { get; set; }
            /// <summary>
            /// 订单金额。支持两种格式，精确到元或精确到分
            /// </summary>
            public string p3_money { get; set; }

            /// <summary>
            /// 回调地址
            /// </summary>
            public string p4_returnurl { get; set; }

            /// <summary>
            /// 异步通知地址
            /// </summary>
            public string p5_noticeurl { get; set; }

            /// <summary>
            /// 终端账号
            /// </summary>
            public string p6_custom { get; set; }

            /// <summary>
            /// 签名类型（MD5）
            /// </summary>
            public string p7_signtype = "MD5";

            /// <summary>
            /// 签名(大写)
            /// </summary>
            public string p8_sign => Sign();

            /// <summary>
            /// 支付方式编码 支付宝：ZFB 微信：WX 云闪付：YSF
            /// </summary>
            public string p10_paytype { get; set; }

            /// <summary>
            /// 支付场景编码 WAP/PC
            /// </summary>
            public string p11_payscene = "WAP";

            /// <summary>
            /// 展示方式编码
            /// </summary>
            public string p12_showtype { get; set; }


            /// <summary>
            /// 卡类或银行编码
            /// </summary>
            public string p13_bankcardcode { get; set; }

            /// <summary>
            /// 备注
            /// </summary>
            public string p14_remark { get; set; }

            /// <summary>
            /// 扩展参数
            /// </summary>
            public string p15_extend { get; set; }

            public JFTYCRequest(OnLinePayOrder order, string paytype, string authority)
            {
                string domain = string.IsNullOrEmpty(AppConfig.FrontSiteDomain) ? authority : AppConfig.FrontSiteDomain;
                this.p2_order = order.OrderID;
                this.p3_money = (order.Amount * 100).ToString("f0");
                this.p4_returnurl= "http://" + domain + "/Notify/jftycPay.aspx";
                this.p5_noticeurl= "http://" + domain + "/Notify/jftycPay.aspx";
                this.p6_custom = order.GameID.ToString();
                this.p10_paytype = paytype;


            }

            public string Sign()
            {
                string md5Str = $"p1_merchant={p1_merchant}&p2_order={p2_order}&p3_money={p3_money}&p4_returnurl={p4_returnurl}&p5_noticeurl={p5_noticeurl}&p6_custom={p6_custom}&key={Config.JFTYCKEY}";
                return Utility.MD5(md5Str).ToUpper();
            }

            /// <summary>
            /// 获取支付链接
            /// </summary>
            /// <returns></returns>
            public string GetPayUrl()
            {
                var payUrl = "";
                JFTYCModel.ParameterModel parameter = new JFTYCModel.ParameterModel()
                {
                    p1_merchant = Config.JFTYCMerchant,
                    p2_order = this.p2_order,
                    p3_money = this.p3_money,
                    p4_returnurl = this.p4_returnurl,
                    p5_noticeurl = this.p5_noticeurl,
                    p6_custom = this.p6_custom,
                    p7_signtype = this.p7_signtype,
                    p8_sign = this.p8_sign,
                    p10_paytype = this.p10_paytype,
                    p11_payscene = this.p11_payscene,
                    p12_showtype = ""
                };
                //发送数据
                var postData = Newtonsoft.Json.JsonConvert.SerializeObject(parameter);
                HttpHelper http = new HttpHelper();
                HttpItem item = new HttpItem()
                {
                    URL = $"{Config.JFTYCUrl}",//URL     必需项  
                    Method = "POST",//URL     可选项 默认为Get  
                    Timeout = 100000,//连接超时时间     可选项默认为100000  
                    ContentType = "application/json",//返回类型    可选项有默认值  
                    Postdata = $"{postData}"//Post数据     可选项GET时不需要写
                };
                HttpResult result = http.GetHtml(item);
                //获取跳转地址重定向
                if (result.StatusCode.ToString() == "Redirect")
                {
                    payUrl = result.RedirectUrl;
                   
                }
                else
                {
                    Log4Net.WriteInfoLog(result.Html,"竣付通提交订单返回信息");
                }
                return payUrl;
            }


            /// <summary>
            /// Url拼接方法
            /// </summary>
            /// <returns></returns>
            public string UrlParams()
            {
                Type t = GetType();
                PropertyInfo[] PropertyList = t.GetProperties();
                string result = "";
                foreach (PropertyInfo item in PropertyList)
                {
                    string name = item.Name;
                    object value = item.GetValue(this, null);
                    if (value != null)
                    {
                        result += $"{name}={value}&";
                    }
                }
                return result.Substring(0, result.Length - 1);
            }

            /// <summary>
            /// Post提交数据
            /// </summary>
            /// <param name="postUrl">URL</param>
            /// <param name="paramData">参数</param>
            /// <returns></returns>
            private string PostWebRequest(string postUrl, string paramData)
            {
                string ret = string.Empty;
                string payUrl = string.Empty;
                try
                {
                    if (!postUrl.StartsWith("http://"))
                        return "";

                    byte[] byteArray = Encoding.Default.GetBytes(paramData); //转化
                    HttpWebRequest webReq = (HttpWebRequest)WebRequest.Create(new Uri(postUrl));
                    webReq.Method = "POST";
                    webReq.ContentType = "application/x-www-form-urlencoded";

                    webReq.ContentLength = byteArray.Length;
                    Stream newStream = webReq.GetRequestStream();
                    newStream.Write(byteArray, 0, byteArray.Length);//写入参数
                    newStream.Close();
                    HttpWebResponse response = (HttpWebResponse)webReq.GetResponse();
                    StreamReader sr = new StreamReader(response.GetResponseStream(), Encoding.UTF8);
                    ret = sr.ReadToEnd();

                    JObject jd = (JObject)JsonConvert.DeserializeObject(ret);
                    if ((string)jd["Flag"] == "True")
                    {
                        payUrl = (string)jd["payUrl"];
                    }
                    else
                    {
                        Log4Net.WriteInfoLog("JFTYC下单失败返回信息：" + ret);
                    }

                    sr.Close();
                    response.Close();
                    newStream.Close();
                }
                catch (Exception ex)
                {
                    Log4Net.WriteErrorLog(ex, "JFTYC支付异常");
                }
                return payUrl;
            }
        }

        public class JFTYCNotify
        {
            
            protected static JFTYCModel.NoticeModel model =new JFTYCModel.NoticeModel();

            public JFTYCNotify(HttpRequest request,out JFTYCModel.NoticeModel noticeModel)
            {
                string postStr = string.Empty;
                using (System.IO.StreamReader sr = new System.IO.StreamReader(request.InputStream))
                    postStr = sr.ReadToEnd();
                Log4Net.WriteInfoLog("JFTYC支付返回参数：" + postStr);
                if (postStr.Contains("{"))
                {
                    noticeModel = Newtonsoft.Json.JsonConvert.DeserializeObject<JFTYCModel.NoticeModel>(postStr);
                }
                else
                {
                    var dys = GetUrlParams(postStr);
                    noticeModel = DicToObject<JFTYCModel.NoticeModel>(dys);
                }

                if (noticeModel != null)
                {
                    model = noticeModel;
                }
            }

            /// <summary>
            /// 验证签名
            /// </summary>
            /// <returns></returns>
            public bool VerifySign()
            {
                string signData = $"p1_usercode={model.p1_usercode}&p2_order={model.p2_order}&p3_ordermoney={model.p3_ordermoney}&p4_status={model.p4_status}&p5_tradeorder={model.p5_tradeorder}&p6_paymethod={model.p6_paymethod}&p7_transamount={model.p7_transamount}";
                string sign = Utility.MD5(signData + $"&key={Config.JFTYCKEY.Trim()}").ToUpper();
                return model.p10_sign == sign;
            }


            /// <summary>
            /// 获URL取参数信息
            /// </summary>
            /// <param name="UrlParamStr"></param>
            /// <returns></returns>
            public static Dictionary<string, string> GetUrlParams(string UrlParamStr)
            {
                string[] _str = UrlParamStr.Split('&');
                var dic = new Dictionary<string, string>();
                foreach (var item in _str)
                {
                    var key = item.Substring(0, item.IndexOf('='));
                    dic.Add(key, item.Substring(item.IndexOf('=') + 1));
                }

                return dic;
            }

            /// <summary>
            /// 字典类型转化为对象
            /// </summary>
            /// <param name="dic"></param>
            /// <returns></returns>
            public T DicToObject<T>(Dictionary<string, string> dic) where T : new()
            {
                var md = new T();
                CultureInfo cultureInfo = Thread.CurrentThread.CurrentCulture;
                TextInfo textInfo = cultureInfo.TextInfo;
                foreach (var d in dic)
                {
                    //var filed = textInfo.ToTitleCase(d.Key);
                    try
                    {
                        var value = d.Value;
                        md.GetType().GetProperty(d.Key).SetValue(md, value, null);
                    }
                    catch (Exception e)
                    {

                    }
                }
                return md;
            }
        }
    }
}