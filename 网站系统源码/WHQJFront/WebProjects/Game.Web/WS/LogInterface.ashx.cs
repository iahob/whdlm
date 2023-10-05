using Game.Facade;
using Game.Utils;
using Game.Web.Helper;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Web;
using Game.Utils;
using System.Net;

namespace Game.Web.WS
{
    /// <summary>
    /// LogInterface 的摘要说明
    /// </summary>
    public class LogInterface : IHttpHandler
    {
        public static object lockobj = new object();
        public void ProcessRequest(HttpContext context)
        {
            //允许跨站请求域名
            context.Response.AddHeader("Access-Control-Allow-Origin", AppConfig.MoblieInterfaceDomain);
            //接口返回数据格式
            context.Response.ContentType = "application/json";
            //接口请求类型
            string action = GameRequest.GetQueryString("action").ToLower();

            try
            {
                lock (lockobj)
                {
                    System.IO.Stream s = context.Request.InputStream;
                    StreamReader sr = new StreamReader(s,System.Text.ASCIIEncoding.UTF8);
                    string ret = sr.ReadToEnd();
                    //Log4Net.WriteInfoLog("ret:" + ret);

                    string userid = context.Request.Form["userid"];
                    //string log = context.Request.Form["logtext"];
                    int platformType = GameRequest.GetInt("platformType", 0);
                    string TypeName = "";
                    if (platformType == 1)
                    {
                        TypeName = "Android";
                    }
                    else if (platformType == 2)
                    {
                        TypeName = "IOS";
                    }
                    Writelog(ret, userid, TypeName);
                    context.Response.Write("true");
                }
            }
            catch (Exception ex)
            {
                Log4Net.WriteInfoLog("下面一条为接口故障信息", "LogInterface");
                Log4Net.WriteErrorLog(ex, "LogInterface");
                context.Response.Write(ex);
            }
        }


        public void Writelog(string text, string userid,string TypeName)
        {
            string time = DateTime.Now.ToString("yyyyMMddHHmmssfff");
            string filePath = AppDomain.CurrentDomain.BaseDirectory + "\\" + "GameLog\\" + userid + "-" + TypeName + "" + "[" + time + "]" + ".txt";
            Save(filePath, text);
        }


        /// <summary>
        /// 创建文件
        /// </summary>
        /// <param name="path">文件路径</param>
        /// <param name="str">文件内容</param>
        public static void Save(string path, string str)
        {
            string s = path.Substring(0, path.LastIndexOf('\\'));
            Directory.CreateDirectory(s);//如果文件夹不存在就创建它
            FileStream fs = new FileStream(path, FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite);
            byte[] data = System.Text.Encoding.Default.GetBytes(str);
            //开始写入
            fs.Write(data, 0, data.Length);
            //清空缓冲区、关闭流
            fs.Flush();
            fs.Close();
        }

        public bool IsReusable
        {
            get
            {
                return false;
            }
        }
    }
}