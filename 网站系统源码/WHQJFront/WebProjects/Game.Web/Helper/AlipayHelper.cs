using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Net;
using System.Web;
using Aop.Api.Util;
using Game.Utils;
// ReSharper disable IdentifierTypo
// ReSharper disable StringLiteralTypo
// ReSharper disable InconsistentNaming

namespace Game.Web.Helper
{
    public class AlipayHelper
    {
        //支付宝签约账号
        private static string Partner => ApplicationSettings.Get("ZFBPARTNER");

        //收款支付宝账号
        //支付宝密钥

        //支付宝公钥
        // ReSharper disable once InconsistentNaming
        private static string RSAPublicKey => ApplicationSettings.Get("ZFBPUBLICKEY");

        //字符编码格式
        private const string InputCharset = "utf-8";

        //支付宝消息验证地址
        private const string Veryfyurl = "https://mapi.alipay.com/gateway.do?service=notify_verify&";


        //支付宝下单配置

        // 应用ID,您的APPID
        public static string app_id = ApplicationSettings.Get("ZFBH5APPID");

        // 支付宝网关
        public static string gatewayUrl = "https://openapi.alipay.com/gateway.do";

        // 商户私钥，您的原始格式RSA私钥
        public static string private_key = ApplicationSettings.Get("ZFBH5PRIVATEKEY");

        // 支付宝公钥,查看地址：https://openhome.alipay.com/platform/keyManage.htm 对应APPID下的支付宝公钥。
        public static string alipay_public_key = ApplicationSettings.Get("ZFBH5PUBLICKEY");

        // 签名方式
        public static string sign_type = "RSA2";

        // 编码格式
        public static string charset = "UTF-8";



        /// <summary>
        /// 获取支付宝POST过来通知消息，并以“参数名=参数值”的形式组成数组
        /// </summary>
        /// <returns>request回来的信息组成的数组</returns>
        public static SortedDictionary<string, string> GetRequestPost()
        {
            int i;
            var sArray = new SortedDictionary<string, string>();
            var coll = HttpContext.Current.Request.Form;

            var requestItem = coll.AllKeys;
            for(i = 0; i < requestItem.Length; i++)
            {
                sArray.Add(requestItem[i], HttpContext.Current.Request.Form[requestItem[i]]);
            }

            return sArray;
        }

        /// <summary>
        ///  验证消息是否是支付宝发出的合法消息
        /// </summary>
        /// <param name="inputPara">通知返回参数数组</param>
        /// <param name="paytype"></param>
        /// <returns>验证结果</returns>
         public static bool Verify(SortedDictionary<string, string> inputPara,string paytype)
        {
            //获取返回时的签名验证结果
            //bool isSign = signType.ToUpper() == "RSA" ? GetSignVeryfyRSA(inputPara, sign) : GetSignVeryfyMD5(inputPara, sign);
            string notifyId = inputPara["notify_id"];
            string sign = inputPara["sign"];
            string signType = inputPara["signType"];
            string publicKey = string.Empty;
            if (paytype == "APP")
            {
                publicKey = RSAPublicKey;
            }
            else if (paytype == "H5")
            {
                publicKey = alipay_public_key;
            }
            var isSign = AlipaySignature.RSACheckV1(inputPara, publicKey, InputCharset, signType,false);
            if (signType == "RSA2")
            {
                return isSign;
            }
            //获取是否是支付宝服务器发来的请求的验证结果
            var responseTxt = "false";
            if(!string.IsNullOrEmpty(notifyId)) { responseTxt = GetResponseTxt(notifyId); }
            return (responseTxt == "true" && isSign);
        }

        /// <summary>
        /// 获取是否是支付宝服务器发来的请求的验证结果
        /// </summary>
        /// <param name="notifyId">通知验证ID</param>
        /// <returns>验证结果</returns>
        private static string GetResponseTxt(string notifyId)
        {
            var veryfyUrl = Veryfyurl + "partner=" + Partner + "&notify_id=" + notifyId;

            //获取远程服务器ATN结果，验证是否是支付宝服务器发来的请求
            var responseTxt = Get_Http(veryfyUrl, 120000);

            return responseTxt;
        }
        /// <summary>
        /// 获取远程服务器ATN结果
        /// </summary>
        /// <param name="strUrl">指定URL路径地址</param>
        /// <param name="timeout">超时时间设置</param>
        /// <returns>服务器ATN结果</returns>
        private static string Get_Http(string strUrl, int timeout)
        {
            string strResult;
            try
            {
                var myReq = (HttpWebRequest)WebRequest.Create(strUrl);
                myReq.Timeout = timeout;
                var httpWResp = (HttpWebResponse)myReq.GetResponse();
                var myStream = httpWResp.GetResponseStream();
                if (myStream != null)
                {
                    var sr = new StreamReader(myStream, Encoding.Default);
                    var strBuilder = new StringBuilder();
                    while (-1 != sr.Peek())
                    {
                        strBuilder.Append(sr.ReadLine());
                    }

                    strResult = strBuilder.ToString();
                }
                else
                {
                    strResult = "";
                }
            }
            catch(Exception exp)
            {
                strResult = "错误：" + exp.Message;
            }

            return strResult;
        }
    }
}