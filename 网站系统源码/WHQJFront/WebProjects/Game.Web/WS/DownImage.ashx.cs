using Game.Facade;
using Game.Facade.Enum;
using Game.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using System.Web;

namespace Game.Web.WS
{
    /// <summary>
    /// DownImage 的摘要说明
    /// </summary>
    public class DownImage : IHttpHandler
    {

        public void ProcessRequest(HttpContext context)
        {
            //允许跨站请求域名
            context.Response.AddHeader("Access-Control-Allow-Origin", "*");
            context.Response.ContentType = "application/json";

            string url = GameRequest.GetQueryString("url");
            string time = GameRequest.GetQueryString("time");
            string sign = GameRequest.GetQueryString("sign");

            //数据验证
            AjaxJsonValid ajv = new AjaxJsonValid();
            if (string.IsNullOrEmpty(url))
            {
                ajv.code = (int)ApiCode.VertyParamErrorCode;
                ajv.msg = "参数请求错误";
                context.Response.Write(ajv.SerializeToJson());
                return;
            }
            //ajv = Fetch.VerifySignData(url + AppConfig.MoblieInterfaceKey + time, sign);
            //if(ajv.code == (int)ApiCode.VertySignErrorCode)
            //{
            //    context.Response.Write(ajv.SerializeToJson());
            //    return;
            //}

            //资源下载
            string base64 = string.Empty;
            GC.Collect();
            WebRequest request = null;
            WebResponse response = null;

            //请求url以获取数据
            try
            {
                //设置最大连接数
                ServicePointManager.DefaultConnectionLimit = 200;
                //设置https验证方式
                if (url.StartsWith("https", StringComparison.OrdinalIgnoreCase))
                {
                    ServicePointManager.ServerCertificateValidationCallback =
                            CheckValidationResult;
                }
                request = WebRequest.Create(context.Server.UrlDecode(url));

                request.Method = "GET";

                //获取服务器返回
                response = request.GetResponse();

                using (Stream stream = response.GetResponseStream())
                {
                    Image image = Image.FromStream(stream);
                    Bitmap bitmap = new Bitmap(image);

                    using (MemoryStream ms = new MemoryStream())
                    {
                        bitmap.Save(ms, ImageFormat.Png);
                        byte[] arr = new byte[ms.Length];
                        ms.Position = 0;
                        ms.Read(arr, 0, (int)ms.Length);
                        ms.Close();
                        base64 = Convert.ToBase64String(arr);
                    }
                }
                ajv.SetValidDataValue(true);
                ajv.SetDataItem("Image", base64);
                context.Response.Write(ajv.SerializeToJson());
            }
            catch (Exception)
            {
                ajv.msg = "操作异常";
                ajv.SetValidDataValue(false);
                ajv.SetDataItem("Image", "");
                context.Response.Write(ajv.SerializeToJson());
            }
            finally
            {
                //关闭连接和流
                response?.Close();
                request?.Abort();
            }
        }

        public bool IsReusable
        {
            get
            {
                return false;
            }
        }

        public bool CheckValidationResult(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors errors)
        {
            //直接确认，否则打不开    
            return true;
        }
    }
}