﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Web;
using Game.Utils;
using Game.Utils.Cache;
using System.IO;
using Game.Entity.NativeWeb;
using Game.Entity.Accounts;
using System.Security.Cryptography;
using Game.Facade.Enum;
using System.Drawing;
using System.Net;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using ThoughtWorks.QRCode.Codec;
using System.Net;
using System.Drawing.Drawing2D;
using System.Text.RegularExpressions;

// ReSharper disable InconsistentNaming

namespace Game.Facade
{
    /// <summary>
    /// 为网站提供全局服务，如：用户Cookie读写等等
    /// </summary>
    public static class Fetch
    {
        #region 构造方法

        #endregion

        #region 公用方法

        /// <summary>
        /// 接口签名验证
        /// </summary>
        /// <param name="signStr">待验证签名字符串</param>
        /// <param name="signData">验证签名数据</param>
        /// <returns></returns>
        public static AjaxJsonValid VerifySignData(string signStr, string signData)
        {
            AjaxJsonValid ajv = new AjaxJsonValid();
            if (!string.IsNullOrEmpty(signData) && Utility.MD5(signStr) == signData) return ajv;
            ajv.code = (int)ApiCode.VertySignErrorCode;
            ajv.msg = EnumHelper.GetDesc(ApiCode.VertySignErrorCode);
            return ajv;
        }

        public static string GetQrCode(object p, int v)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// 获取交易流水号
        /// </summary>
        /// <param name="prefix"></param>
        /// <returns></returns>
        public static string GetOrderIDByPrefix(string prefix)
        {
            //构造订单号 (形如:20101201102322159111111)
            int orderIDLength = 32;
            int randomLength = 6;
            StringBuffer tradeNoBuffer = new StringBuffer();

            tradeNoBuffer += prefix;
            tradeNoBuffer += TextUtility.GetDateTimeLongString();

            if ((tradeNoBuffer.Length + randomLength) > orderIDLength)
                randomLength = orderIDLength - tradeNoBuffer.Length;

            tradeNoBuffer += TextUtility.CreateRandom(randomLength, 1, 0, 0, 0, "");

            return tradeNoBuffer.ToString();
        }

        /// <summary>
        /// 获取给定日期距离1900-01-01的天数
        /// </summary>
        /// <param name="dateTime"></param>
        /// <returns></returns>
        public static int GetDateID(DateTime dateTime)
        {
            TimeSpan ts1 = new TimeSpan(dateTime.Ticks);
            TimeSpan ts2 = new TimeSpan(Convert.ToDateTime("1900-01-01").Ticks);
            TimeSpan ts = ts1.Subtract(ts2).Duration();
            return ts.Days;
        }

        /// <summary>
        /// 客户端终端类型
        /// </summary>
        /// <returns>1:终端为android手机 2：终端为苹果的ipad、iphone、ipod</returns>
        public static int GetTerminalType(HttpRequest request)
        {
            string userAgent = request.Headers["User-Agent"];
            if (userAgent == null)
            {
                return 0;
            }
            userAgent = userAgent.ToLower();
            if (userAgent.Contains("android"))
            {
                return 1;
            }
            else if (userAgent.Contains("ipad") || userAgent.Contains("iphone"))
            {
                return 2;
            }
            return 0;
        }

        /// <summary>
        /// 判断是否微信内置浏览器
        /// </summary>
        /// <param name="request"></param>
        /// <returns></returns>
        public static bool isWeChat(HttpRequest request)
        {
            string userAgent = request.Headers["User-Agent"];
            return userAgent != null && userAgent.Contains("MicroMessenger");
        }

        /// <summary>
        /// 获取网站广告缓存
        /// </summary>
        /// <returns></returns>
        public static IList<Ads> GetAdsList()
        {
            IList<Ads> list = null;
            object obj = WHCache.Default.Get<AspNetCache>(AppConfig.AdsConfigCache);
            if (obj != null)
            {
                list = obj as List<Ads>;
            }
            if (obj == null || list == null)
            {
                list = FacadeManage.aideNativeWebFacade.GetAdsList();
                WHCache.Default.Save<AspNetCache>(AppConfig.AdsConfigCache, list, AppConfig.ResourceTimeOut);
            }
            return list;
        }

        /// <summary>
        /// 获取网站站点配置
        /// </summary>
        /// <returns></returns>
        public static ConfigInfo GetWebSiteConfig()
        {
            return FacadeManage.aideNativeWebFacade.GetConfigInfo(AppConfig.SiteConfigKey.WebSiteConfig.ToString());
        }
        /// <summary>
        /// 获取移动版大厅配置
        /// </summary>
        /// <returns></returns>
        public static ConfigInfo GetMobileConfig()
        {
            return FacadeManage.aideNativeWebFacade.GetConfigInfo(AppConfig.SiteConfigKey.MobilePlatformVersion.ToString());
        }

        /// <summary>
        /// 获取上传的图片URL
        /// </summary>
        /// <param name="imageDomain">图片服务器地址</param>
        /// <param name="fileUrl">图片相对路径</param>
        /// <returns></returns>
        public static string GetUploadFileUrl(string imageDomain, string fileUrl)
        {
            return imageDomain + fileUrl.ToLower().Replace("upload/", "");
        }

        /// <summary>
        /// 获取上传的图片URL
        /// </summary>
        /// <param name="fileUrl">图片相对路径</param>
        /// <returns></returns>
        public static string GetUploadFileUrl(string fileUrl)
        {
            ConfigInfo config = GetWebSiteConfig();
            return GetUploadFileUrl((config != null ? config.Field2 : ""), fileUrl);
        }

        /// <summary>
        /// 获取客服服务配置
        /// </summary>
        /// <returns></returns>
        public static ConfigInfo GetCustomerService()
        {
            return FacadeManage.aideNativeWebFacade.GetConfigInfo(AppConfig.SiteConfigKey.SysCustomerService.ToString());
        }

        /// <summary>
        /// 获取注册来源
        /// </summary>
        /// <param name="typeId"></param>
        /// <returns></returns>
        public static string RegisterOrigin(int typeId)
        {
            string rValue;
            switch (typeId)
            {
                case 0:
                    rValue = "PC";
                    break;
                case 1:
                    rValue = "模拟器";
                    break;
                case 16:
                case 17:
                case 18:
                    rValue = "Android";
                    break;
                case 32:
                    rValue = "iTouch";
                    break;
                case 48:
                case 49:
                case 50:
                    rValue = "iPhone";
                    break;
                case 64:
                case 65:
                case 66:
                    rValue = "iPad";
                    break;
                case 81:
                    rValue = "WEB推广页";
                    break;
                case 82:
                    rValue = "WEB约战页";
                    break;
                case 90:
                    rValue = "H5";
                    break;
                default:
                    rValue = "未知";
                    break;
            }
            return rValue;
        }

        /// <summary>
        /// 格式化时间换行显示
        /// </summary>
        /// <returns></returns>
        public static string FormatTimeWrap(DateTime time)
        {
            return time.ToString("yyyy-MM-dd") + "<br/>" + time.ToString("HH:mm:ss");
        }

        /// <summary>
        /// 获取微信参数
        /// </summary>
        /// <param name="encrypt">微信加密参数字符</param>
        /// <returns></returns>
        public static WxUser GetWxUser(string encrypt)
        {
            string decparam = DESDecrypt(encrypt, AppConfig.WxUrlKey);
            if (string.IsNullOrEmpty(decparam) || decparam.IndexOf(',') <= 0)
            {
                return null;
            }

            //由原来逗号分割，改为正则匹配
            string pattern = @"\<.*?\>";
            MatchCollection list = Regex.Matches(decparam, pattern);
            if (list.Count != 5)
            {
                return null;
            }
            try
            {
                WxUser wu = new WxUser
                {
                    openid = list[0].Value.Substring(1, list[0].Value.Length - 2),
                    unionid = list[1].Value.Substring(1, list[1].Value.Length - 2),
                    nickname = list[2].Value.Substring(1, list[2].Value.Length - 2),
                    sex = Convert.ToByte(list[3].Value.Substring(1, list[3].Value.Length - 2)),
                    headimgurl = list[4].Value.Substring(1, list[4].Value.Length - 2)
                };
                return wu;
            }
            catch (Exception ex)
            {
                return null;
            }
        }

        #endregion

        #region 登录用户

        /// <summary>
        /// 设置用户cookie
        /// </summary>
        /// <param name="userTicket">用户信息</param>
        public static void SetUserCookie(UserTicketInfo userTicket)
        {
            if (userTicket == null || userTicket.UserID <= 0)
            {
                return;
            }
            WHCache.Default.Save<SessionCache>(AppConfig.UserLoginCacheKey, userTicket, AppConfig.UserLoginTimeOut);
        }

        /// <summary>
        /// 获取用户对象
        /// </summary>
        /// <returns></returns>
        public static UserTicketInfo GetUserCookie()
        {
            object obj = WHCache.Default.Get<SessionCache>(AppConfig.UserLoginCacheKey);
            return obj as UserTicketInfo;
        }

        /// <summary>
        /// 删除用户cookie
        /// </summary>
        public static void DeleteUserCookie()
        {
            WHCache.Default.Delete<SessionCache>(AppConfig.UserLoginCacheKey);
        }

        #endregion

        #region DES 加密解密

        /// <summary>
        /// 进行DES加密。
        /// </summary>
        /// <param name="pToEncrypt">要加密的字符串。</param>
        /// <param name="sKey">密钥，且必须为8位。</param>
        /// <returns>以Base64格式返回的加密字符串。</returns>
        public static string DESEncrypt(string pToEncrypt, string sKey)
        {
            using (DESCryptoServiceProvider des = new DESCryptoServiceProvider())
            {
                byte[] inputByteArray = Encoding.UTF8.GetBytes(pToEncrypt);
                des.Key = Encoding.UTF8.GetBytes(sKey);
                des.IV = Encoding.UTF8.GetBytes(sKey);
                MemoryStream ms = new MemoryStream();
                using (CryptoStream cs = new CryptoStream(ms, des.CreateEncryptor(), CryptoStreamMode.Write))
                {
                    cs.Write(inputByteArray, 0, inputByteArray.Length);
                    cs.FlushFinalBlock();
                    cs.Close();
                }
                string str = Convert.ToBase64String(ms.ToArray());
                ms.Close();
                return str;
            }
        }

        // <summary>
        // 进行DES解密。
        // </summary>
        // <param name="pToDecrypt">要解密的以Base64</param>
        // <param name="sKey">密钥，且必须为8位。</param>
        // <returns>已解密的字符串。</returns>
        public static string DESDecrypt(string pToDecrypt, string sKey)
        {
            byte[] inputByteArray = Convert.FromBase64String(pToDecrypt);
            using (DESCryptoServiceProvider des = new DESCryptoServiceProvider())
            {
                des.Key = Encoding.UTF8.GetBytes(sKey);
                des.IV = Encoding.UTF8.GetBytes(sKey);
                MemoryStream ms = new MemoryStream();
                using (CryptoStream cs = new CryptoStream(ms, des.CreateDecryptor(), CryptoStreamMode.Write))
                {
                    cs.Write(inputByteArray, 0, inputByteArray.Length);
                    cs.FlushFinalBlock();
                    cs.Close();
                }
                string str = Encoding.UTF8.GetString(ms.ToArray());
                ms.Close();
                return str;
            }
        }

        #endregion

        #region AES 加密解密

        /// <summary>
        /// H5 公用加密方式
        /// </summary>
        /// <param name="inputdata">加密数据</param>
        /// <param name="key">密钥</param>
        /// <param name="iv">向量</param>
        /// <returns></returns>
        public static string AESEncrypt(string inputdata, string key, string iv)
        {
            byte[] keyArray = Encoding.UTF8.GetBytes(key);
            byte[] ivArray = Encoding.UTF8.GetBytes(iv);
            byte[] toEncryptArray = Encoding.UTF8.GetBytes(inputdata);

            RijndaelManaged rDel = new RijndaelManaged
            {
                Key = keyArray,
                IV = ivArray,
                Mode = CipherMode.CBC,
                Padding = PaddingMode.Zeros
            };

            ICryptoTransform cTransform = rDel.CreateEncryptor();
            byte[] resultArray = cTransform.TransformFinalBlock(toEncryptArray, 0, toEncryptArray.Length);

            return Convert.ToBase64String(resultArray, 0, resultArray.Length);
        }

        #endregion

        #region SHA-256 加密

        /// <summary>
        /// SHA256 哈希值
        /// </summary>
        /// <param name="strIN"></param>
        /// <param name="isBase64"></param>
        /// <returns></returns>
        public static string SHA256Encrypt(string strIN, bool isBase64 = false)
        {
            //string strIN = getstrIN(strIN);
            SHA256 sha256 = new SHA256CryptoServiceProvider();
            var tmpByte = sha256.ComputeHash(GetKeyByteArray(strIN));
            sha256.Clear();
            StringBuilder sb = new StringBuilder();
            if (isBase64) return Convert.ToBase64String(tmpByte);
            foreach (byte t in tmpByte)
            {
                sb.Append(t.ToString("x2"));
            }
            return sb.ToString();
        }

        /// <summary>
        /// string 转为 byte[]
        /// </summary>
        /// <param name="strKey"></param>
        /// <returns></returns>
        private static byte[] GetKeyByteArray(string strKey)
        {
            return Encoding.UTF8.GetBytes(strKey);
        }

        #endregion

        #region MD5加密
        /// <summary>
        /// MD5加密函数
        /// </summary>
        /// <param name="text">需要加密的字符串</param>
        /// <param name="isLower">是否小写</param>
        /// <param name="bit">16位或32位</param>
        /// <returns>密文</returns>
        public static string EncryptMD5(string text, bool isLower, int bit)
        {
            string ciphertext = string.Empty;
            if (bit != 32 && bit != 16)
            {
                return ciphertext;
            }
            if (bit == 32)    //32位
            {
                MD5 md5 = MD5.Create();//实例化一个md5对像
                byte[] s = md5.ComputeHash(Encoding.UTF8.GetBytes(text));    // 加密后是一个字节类型的数组，这里要注意编码UTF8/Unicode等的选择　
                // 通过使用循环，将字节类型的数组转换为字符串，此字符串是常规字符格式化所得
                for (int i = 0; i < s.Length; i++)
                {
                    ciphertext = ciphertext + s[i].ToString("X");   // 将得到的字符串使用十六进制类型格式。格式后的字符是小写的字母，如果使用大写（X）则格式后的字符是大写字符
                }
            }
            else    //16位
            {
                MD5CryptoServiceProvider md5 = new MD5CryptoServiceProvider();
                ciphertext = BitConverter.ToString(md5.ComputeHash(UTF8Encoding.Default.GetBytes(text)), 4, 8);
                ciphertext = ciphertext.Replace("-", "");
            }
            if (isLower)
            {
                ciphertext = ciphertext.ToLower();
            }
            return ciphertext;
        }
        #endregion


        #region 画二维码

        /// <summary>
        /// 获取二维码图片
        /// </summary>
        /// <param name="sData"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public static string GetQrCode(string sData, int size)
        {
            //二维码版本,大小获取
            Color qrCodeBackgroundColor = Color.White;
            Color qrCodeForegroundColor = Color.Black;
            int length = System.Text.Encoding.UTF8.GetBytes(sData).Length;

            //生成二维码数据
            QRCodeEncoder qrCodeEncoder = new QRCodeEncoder();
            qrCodeEncoder.QRCodeEncodeMode = QRCodeEncoder.ENCODE_MODE.BYTE;
            qrCodeEncoder.QRCodeErrorCorrect = QRCodeEncoder.ERROR_CORRECTION.H; //使用H纠错级别
            qrCodeEncoder.QRCodeVersion = 0;
            var encodedData = qrCodeEncoder.Encode(sData, System.Text.Encoding.UTF8);

            //绘制图片
            int x = 0, y = 0;
            int w = 0, h = 0;
            // 二维码矩阵单边数据点数目
            int count = encodedData.Length;
            // 获取单个数据点边长
            double sideLength = Convert.ToDouble(size - 30) / count;
            // 初始化背景色画笔
            SolidBrush backcolor = new SolidBrush(qrCodeBackgroundColor);
            // 初始化前景色画笔
            SolidBrush forecolor = new SolidBrush(qrCodeForegroundColor);
            // 定义画布
            Bitmap image = new Bitmap(size, size);
            // 获取GDI+绘图图画
            Graphics graph = Graphics.FromImage(image);
            // 先填充背景色
            graph.FillRectangle(backcolor, 0, 0, size, size);

            // 变量数据矩阵生成二维码
            for (int row = 0; row < count; row++)
            {
                for (int col = 0; col < count; col++)
                {
                    // 计算数据点矩阵起始坐标和宽高
                    x = Convert.ToInt32(Math.Round(col * sideLength)) + 15;
                    y = Convert.ToInt32(Math.Round(row * sideLength)) + 15;
                    w = Convert.ToInt32(Math.Ceiling((col + 1) * sideLength) - Math.Floor(col * sideLength));
                    h = Convert.ToInt32(Math.Ceiling((row + 1) * sideLength) - Math.Floor(row * sideLength));

                    // 绘制数据矩阵
                    graph.FillRectangle(encodedData[col][row] ? forecolor : backcolor, x, y, w, h);
                }
            }
            try
            {
                //添加LOGO
                string url = Fetch.GetUploadFileUrl("/Site/qrsmall.png");
                Bitmap logoImage = null;
                HttpWebRequest webRequest = (HttpWebRequest)HttpWebRequest.Create(url);

                HttpWebResponse webReponse = (HttpWebResponse)webRequest.GetResponse();
                if (webReponse.StatusCode == HttpStatusCode.OK)
                {
                    using (Stream stream = webReponse.GetResponseStream())
                    {
                        Image img = Image.FromStream(stream);
                        logoImage = new Bitmap(img);
                        img.Dispose();

                        if (logoImage != null)
                        {
                            image = CoverImage(image, logoImage, graph);
                            logoImage.Dispose();
                        }

                        using (MemoryStream ms = new MemoryStream())
                        {
                            image.Save(ms, System.Drawing.Imaging.ImageFormat.Png);
                            byte[] bts = ms.ToArray();
                            string baseImg = Convert.ToBase64String(bts);
                            image.Dispose();
                            return "data:image/jpeg;base64," + baseImg;
                        }
                    }
                }
                return "";
            }
            catch (Exception ex)
            {
                return "";
            }
        }

        /// <summary>
        /// 层叠图片
        /// </summary>
        /// <param name="original">原始图片(目前只支持正方形)</param>
        /// <param name="image">层叠图片(目前只支持正方形)</param>
        /// <returns>处理以后的图片</returns>
        private static Bitmap CoverImage(Bitmap original, Bitmap image, Graphics graph = null)
        {
            //缩放附加图片
            int sideSLen = original.Width;
            int sideTLen = sideSLen / 4;
            image = ResizeImage(image, sideTLen, sideTLen);

            // 获取GDI+绘图图画
            graph = graph == null ? Graphics.FromImage(original) : graph;

            // 将附加图片绘制到原始图中央
            graph.DrawImage(image, (original.Width - sideTLen - 10) / 2, (original.Height - sideTLen - 10) / 2, sideTLen,
                sideTLen);

            // 释放GDI+绘图图画内存
            graph.Dispose();

            // 返回处理结果
            return original;
        }

        /// <summary>
        /// 图片缩放
        /// </summary>
        /// <param name="bmp">原始Bitmap</param>
        /// <param name="newW">新的宽度</param>
        /// <param name="newH">新的高度</param>
        /// <returns>处理以后的图片</returns>
        private static Bitmap ResizeImage(Bitmap original, int width, int height)
        {
            try
            {
                Bitmap image = new Bitmap(width, height);
                Graphics graph = Graphics.FromImage(image);
                // 插值算法的质量
                graph.CompositingQuality = CompositingQuality.HighQuality;
                graph.SmoothingMode = SmoothingMode.HighQuality;
                graph.InterpolationMode = InterpolationMode.HighQualityBicubic;
                graph.DrawImage(original, new Rectangle(0, 0, width, height),
                    new Rectangle(0, 0, original.Width, original.Height), GraphicsUnit.Pixel);
                graph.Dispose();
                return image;
            }
            catch
            {
                return null;
            }
        }

        #endregion

        #region 日期格式与Unix时间戳

        /// <summary>
        /// DateTime转为Uinx时间戳
        /// </summary>
        /// <param name="time"></param>
        /// <returns></returns>
        public static string ConvertDateTimeToUnix(DateTime time)
        {
            return ((time.ToUniversalTime().Ticks - 621355968000000000) / 10000000).ToString();
        }

        /// <summary>
        /// Uinx时间戳转为DateTime
        /// </summary>
        /// <param name="unix"></param>
        /// <returns></returns>
        public static DateTime ConvertUnixToDateTime(string unix)
        {
            DateTime startUnixTime = TimeZoneInfo.ConvertTime(new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc), TimeZoneInfo.Local);
            return startUnixTime.AddSeconds(double.Parse(unix));
        }

        #endregion

        #region 日期处理

        /// <summary>
        /// 获取开始时间
        /// </summary>
        /// <param name="bgDate"></param>
        /// <returns></returns>
        public static string GetStartTime(DateTime bgDate)
        {
            DateTime bgTime = new DateTime(bgDate.Year, bgDate.Month, bgDate.Day, 0, 0, 0);
            return Convert.ToString(bgTime);
        }

        /// <summary>
        /// 获取结束时间
        /// </summary>
        /// <param name="enDate"></param>
        /// <returns></returns>
        public static string GetEndTime(DateTime enDate)
        {
            DateTime enTime = new DateTime(enDate.Year, enDate.Month, enDate.Day, 23, 59, 59);
            return Convert.ToString(enTime);
        }

        /// <summary>
        /// 获取指定日期的开始时间和结束时间(日期格式：开始时间$结束时间)
        /// </summary>
        /// <param name="bgDate"></param>
        /// <param name="enDate"></param>
        /// <returns></returns>
        public static string[] GetTimeByDate(DateTime bgDate, DateTime enDate)
        {
            DateTime bgTime = new DateTime(bgDate.Year, bgDate.Month, bgDate.Day, 0, 0, 0);
            DateTime enTime = new DateTime(enDate.Year, enDate.Month, enDate.Day, 23, 59, 59);
            return new[] { bgTime.ToString("yyyy-MM-dd HH:mm:ss"), enTime.ToString("yyyy-MM-dd HH:mm:ss") };
        }


        /// <summary>
        /// 获取当天的开始时间和结束时间
        /// </summary>
        public static string[] GetTodayTime()
        {
            DateTime dt = DateTime.Now;
            return GetTimeByDate(dt, dt);
        }

        /// <summary>
        /// 获取昨天的开始时间和结束时间
        /// </summary>
        /// <returns></returns>
        public static string[] GetYesterdayTime()
        {
            DateTime dt = DateTime.Now.AddDays(-1);
            return GetTimeByDate(dt, dt);
        }

        /// <summary>
        /// 获取本周的开始时间和结束时间
        /// </summary>
        public static string[] GetWeekTime()
        {
            DateTime dt = DateTime.Now;
            DateTime startWeek = dt.AddDays(0 - Convert.ToInt32(dt.DayOfWeek.ToString("d")));  //本周周日
            DateTime endWeek = startWeek.AddDays(6);  //本周周六
            return GetTimeByDate(startWeek, endWeek);
        }

        /// <summary>
        /// 获取上周的开始时间和结束时间
        /// </summary>
        public static string[] GetLastWeekTime()
        {
            DateTime dt = DateTime.Now;
            DateTime startWeek = dt.AddDays(0 - 7 - Convert.ToInt32(dt.DayOfWeek.ToString("d")));  //本周周日
            DateTime endWeek = startWeek.AddDays(6);  //本周周日六
            return GetTimeByDate(startWeek, endWeek);
        }

        /// <summary>
        /// 获取本月的开始时间和结束时间
        /// </summary>
        /// <returns></returns>
        public static string[] GetMonthTime()
        {
            DateTime dt = DateTime.Now;
            DateTime startMonth = dt.AddDays(1 - dt.Day);  //本月月初
            DateTime endMonth = startMonth.AddMonths(1).AddDays(-1);  //本月月末
            return GetTimeByDate(startMonth, endMonth);
        }

        /// <summary>
        /// 获取上月的开始时间和结束时间
        /// </summary>
        /// <returns></returns>
        public static string[] GetLastMonthTime()
        {
            DateTime dt = DateTime.Now.AddMonths(-1);
            DateTime startMonth = dt.AddDays(1 - dt.Day);
            DateTime endMonth = startMonth.AddMonths(1).AddDays(-1);
            return GetTimeByDate(startMonth, endMonth);
        }

        /// <summary>
        /// 获取本年的开始时间和结束时间
        /// </summary>
        /// <returns></returns>
        public static string[] GetYearTime()
        {
            DateTime dt = DateTime.Now;
            DateTime startYear = dt.AddDays(1 - dt.DayOfYear);//本年年初
            DateTime endYear = startYear.AddYears(1).AddDays(-1);  //本年年末
            return GetTimeByDate(startYear, endYear);
        }

        /// <summary>
        /// 获取时间间隔
        /// </summary>
        /// <param name="dtStartDate">开始时间</param>
        /// <param name="dtEndDate">结束时间</param>
        /// <returns></returns>
        public static string GetTimeSpan(DateTime dtStartDate, DateTime dtEndDate)
        {
            StringBuilder sb = new StringBuilder();
            TimeSpan ts = dtEndDate.Subtract(dtStartDate);
            if (ts.Days != 0)
                sb.AppendFormat("{0}天", ts.Days);
            if (ts.Hours != 0)
                sb.AppendFormat("{0}小时", ts.Hours);
            if (ts.Minutes != 0)
                sb.AppendFormat("{0}分钟", ts.Minutes);
            if (ts.Seconds != 0)
                sb.AppendFormat("{0}秒", ts.Seconds);
            if (string.IsNullOrEmpty(sb.ToString()))
                return "0秒";
            return sb.ToString();
        }
        /// <summary>
        /// 秒数转换成 0天0小时0分钟0秒
        /// </summary>
        /// <param name="seconds">秒</param>
        /// <returns></returns>
        public static string ConverTimeToDHMS(long seconds)
        {
            StringBuilder sb = new StringBuilder();
            if (seconds <= 0)
                return "0秒";
            long day = seconds / 0x15180;
            long hour = (seconds % 0x15180) / 0xe10;
            long minute = (seconds % 0xe10) / 60;
            long second = seconds % 60;
            if (day > 0)
                sb.AppendFormat("{0}天", day);
            if (hour > 0)
                sb.AppendFormat("{0}小时", hour);
            if (minute > 0)
                sb.AppendFormat("{0}分钟", minute);
            if (second > 0)
                sb.AppendFormat("{0}秒", second);
            if (string.IsNullOrEmpty(sb.ToString()))
                return "0秒";
            return sb.ToString();

        }
        #endregion

        #region 防注入

        /// <summary>
        /// SQL过滤，Script过滤
        /// </summary>
        /// <param name="seconds">秒</param>
        /// <returns></returns>
        public static string GetFilterString(string key)
        {
            var str = GameRequest.GetString(key);
            return TextFilter.FilterScript(TextFilter.FilterSql(str));
        }


        #endregion

        public static string GetMD5(string str)
        {
            MD5 md5 = new MD5CryptoServiceProvider();
            byte[] result = md5.ComputeHash(Encoding.UTF8.GetBytes(str));
            StringBuilder strReturn = new StringBuilder();

            for (int i = 0; i < result.Length; i++)
            {
                strReturn.Append(Convert.ToString(result[i], 16).PadLeft(2, '0'));
            }
            return strReturn.ToString().PadLeft(32, '0');
        }

        #region HttpClient

        /// <summary>
        /// 处理http GET请求，返回数据
        /// </summary>
        /// <param name="url">请求的url地址</param>
        /// <returns>http GET成功后返回的数据，失败抛WebException异常</returns>
        public static string Get(string url)
        {
            GC.Collect();
            string result;

            HttpWebRequest request = null;
            HttpWebResponse response = null;

            //请求url以获取数据
            try
            {
                //设置最大连接数
                ServicePointManager.DefaultConnectionLimit = 200;
                //设置https验证方式
                if(url.StartsWith("https", StringComparison.OrdinalIgnoreCase))
                {
                    ServicePointManager.ServerCertificateValidationCallback =
                            CheckValidationResult;
                }

                request = (HttpWebRequest)WebRequest.Create(url);

                request.Method = "GET";

                //获取服务器返回
                response = (HttpWebResponse)request.GetResponse();

                //获取HTTP返回数据
                // ReSharper disable once AssignNullToNotNullAttribute
                StreamReader sr = new StreamReader(stream: response.GetResponseStream(), encoding: Encoding.UTF8);
                result = sr.ReadToEnd().Trim();
                sr.Close();
            }
            finally
            {
                //关闭连接和流
                response?.Close();
                request?.Abort();
            }
            return result;
        }

        public static string Post(string param, string url, int timeout)
        {
            GC.Collect();//垃圾回收，回收没有正常关闭的http连接

            string result;//返回结果

            HttpWebRequest request = null;
            HttpWebResponse response = null;

            try
            {
                //设置最大连接数
                ServicePointManager.DefaultConnectionLimit = 200;
                //设置https验证方式
                if(url.StartsWith("https", StringComparison.OrdinalIgnoreCase))
                {
                    ServicePointManager.ServerCertificateValidationCallback =
                            CheckValidationResult;
                }
                request = (HttpWebRequest)WebRequest.Create(url);

                request.Method = "POST";
                request.Timeout = timeout * 1000;

                //设置POST的数据类型和长度
                request.ContentType = "application/x-www-form-urlencoded";
                byte[] data = Encoding.UTF8.GetBytes(param);
                request.ContentLength = data.Length;

                //往服务器写入数据
                var reqStream = request.GetRequestStream();
                reqStream.Write(data, 0, data.Length);
                reqStream.Close();

                //获取服务端返回
                response = (HttpWebResponse)request.GetResponse();

                //获取服务端返回数据
                // ReSharper disable once AssignNullToNotNullAttribute
                StreamReader sr = new StreamReader(stream: response.GetResponseStream(), encoding: Encoding.UTF8);
                result = sr.ReadToEnd().Trim();
                sr.Close();
            }
            finally
            {
                //关闭连接和流
                response?.Close();
                request?.Abort();
            }
            return result;
        }
        public static bool CheckValidationResult(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors errors)
        {
            //直接确认，否则打不开    
            return true;
        }

        #endregion
    }
}
