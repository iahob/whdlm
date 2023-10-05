using Game.Utils;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Security.Cryptography;
using System.Web;

namespace Game.Facade
{
    public  class GetMobileCode
    {


        public static string MyGetCode(string mobile)
        {
           
            string tmp = RndNum(6);
            string uid = ApplicationSettings.Get("SMS_uid");			//用户名
            string pass = ApplicationSettings.Get("SMS_pwd");	//密码
            string extno = ApplicationSettings.Get("SMS_extno");//接入码
            string strContent = string.Format(ApplicationSettings.Get("SMS_content"), tmp);
            strContent = strContent.Replace("{0}", tmp);
          
            string pwd = getMD5(pass+ extno + strContent+ mobile);
            strContent = HttpUtility.UrlEncode(strContent, Encoding.UTF8);
            string url = string.Format("http://118.190.211.36:7862/sms");
            var param = string.Format("action=send&account={0}&password={1}&mobile={2}&content={3}&extno={4}&rt=json", uid, pwd, mobile, strContent, extno);
            try
            {
                //string rs = Function.MdSmsSend_u(uid, pwd, mobile, strContent, "", "", "");
                string rs = RequestUri(url, param);
               JObject jd = (JObject)JsonConvert.DeserializeObject(rs);
                if ((string)jd["status"] == "0")
                {
                    return tmp;
                }
            }
          catch(Exception e)
            {
                //Log.Write(e.Message);
            }
            return "";
        }


        /// <summary>
        /// 获取md5码
        /// </summary>
        /// <param name="source">待转换字符串</param>
        /// <returns>md5加密后的字符串</returns>
        public static string getMD5(string source)
        {
            string result = "";
            try
            {
                MD5 getmd5 = new MD5CryptoServiceProvider();
                byte[] targetStr = getmd5.ComputeHash(UnicodeEncoding.UTF8.GetBytes(source));
                result = BitConverter.ToString(targetStr).Replace("-", "");
                return result;
            }
            catch (Exception)
            {
                return "0";
            }

        }

        //获取随机数
        private static string RndNum(int VcodeNum)
        {
            String Vchar = "0,1,2,3,4,5,6,7,8,9";
            String[] VcArray = Vchar.Split(',');
            String VNum = "";
            Random random = new Random();
            for (int i = 1; i <= VcodeNum; i++)
            {
                int iNum = 0;
                while ((iNum = Convert.ToInt32(VcArray.Length * random.NextDouble())) == VcArray.Length)
                {
                    iNum = Convert.ToInt32(VcArray.Length * random.NextDouble());
                }
                VNum += VcArray[iNum];
                // VNum += VcArray[Convert.ToInt32(VcArray.Length*random.NextDouble())];
            }
            return VNum;
        }
        public static string RequestUri(string uri, string param)
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(uri);
            request.Method = "POST";
            request.ContentType = "application/x-wwww-form-urlencoded";
           // request.ContentLength = Encoding.UTF8.GetByteCount(param);
            StreamWriter write = new StreamWriter(request.GetRequestStream(), Encoding.GetEncoding("utf-8"));
            write.Write(param);
            write.Flush();
            HttpWebResponse response = null;
            try
            {
                response = (HttpWebResponse)request.GetResponse();
            }
            catch (Exception)
            {
            }
            string strJson = string.Empty;
            if (response != null)
            {
                Stream responseStream = response.GetResponseStream();
                StreamReader reader = new StreamReader(responseStream, Encoding.GetEncoding("utf-8"));
                strJson = reader.ReadToEnd();
                reader.Close();
                responseStream.Close();
            }
            return strJson;
        }
    }
}
