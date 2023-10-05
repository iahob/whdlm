using System;
using System.Collections.Generic;
using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Utils;
using Game.Web.Helper;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

// ReSharper disable IdentifierTypo
// ReSharper disable CommentTypo
// ReSharper disable StringLiteralTypo

namespace Game.Web.Mobile
{
    public partial class ScanCodeAuthorize : System.Web.UI.Page
    {
        public string ErrorMsg { get; set; }

        protected void Page_Load(object sender, EventArgs e)
        {
            var code = Request.QueryString["code"];
            var state = Request.QueryString["state"];

            if (string.IsNullOrEmpty(code))
            {
                ErrorMsg = "抱歉，您取消了授权操作！";
                return;
            }

            //获取access_token
            var appid = AppConfig.GetSetting("WXWebID");
            var appsecret = AppConfig.GetSetting("WXWebAPPSECRET");

            if (string.IsNullOrEmpty(appid) || string.IsNullOrEmpty(appsecret))
            {
                ErrorMsg = "微信网站应用配置有误，请与管理员联系！";
                return;
            }

            try
            {
                //构造获取openid及access_token的url
                var dic = new SortedDictionary<string, object>
                {
                    {"appid", appid}, {"secret", appsecret}, {"code", code}, {"grant_type", "authorization_code"}
                };
                var url = "https://api.weixin.qq.com/sns/oauth2/access_token?" + WxAuthorize.ToUrl(dic);

                //请求url以获取数据
                var result = WxHttpService.Get(url);

                Log4Net.WriteInfoLog(result, "微信扫码登录");
                //保存access_token，用于收货地址获取
                var jd = (JObject) JsonConvert.DeserializeObject(result);
                var errcode = jd["errcode"] != null ? (int) jd["errcode"] : 0;
                var errmsg = jd["errcode"] != null ? (string) jd["errmsg"] : "";
                var accessToken = (string) jd["access_token"];
                var refreshToken = (string) jd["refresh_token"];

                if (errcode == 40029)
                {
                    return;
                }

                if (errcode > 0)
                {
                    ErrorMsg = errmsg;
                    return;
                }

                //获取用户openid
                var openid = (string) jd["openid"];
                //获取用户unionid
                if (jd["unionid"] == null)
                {
                    ErrorMsg = "未关联开放平台，请与管理员联系！";
                    return;
                }

                var record = FacadeManage.aideNativeWebFacade.GetWxScanLoginRecordByKey(state);
                var unionid = (string) jd["unionid"];
                if (record == null)
                    record = new WxScanLoginRecord();

                record.Key = state;
                record.AccessToken = accessToken;
                record.RefreshToken = refreshToken;
                record.OpenID = openid;
                record.UnionID = unionid;
                record.CollectClientIP = GameRequest.GetUserIP();
                record.State = 0;

                FacadeManage.aideNativeWebFacade.SaveWxScanLoginRecord(record);
            }
            catch (Exception ex)
            {
                Log4Net.WriteErrorLog(ex, "微信扫码登录");
                ErrorMsg = "授权异常，请重试！";
            }
        }
    }
}