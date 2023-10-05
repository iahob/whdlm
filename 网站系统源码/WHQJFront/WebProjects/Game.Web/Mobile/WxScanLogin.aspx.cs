using System;
using System.Collections.Generic;
using System.Web.UI;
using CsharpHttpHelper;
using Game.Facade;
using Game.Utils;
using Game.Web.Helper;
using System.Text.RegularExpressions;

// ReSharper disable IdentifierTypo
// ReSharper disable CommentTypo
// ReSharper disable StringLiteralTypo

namespace Game.Web.Mobile
{
    public partial class WxScanLogin : Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            var state = Fetch.GetFilterString("state");

            if (string.IsNullOrEmpty(state))
            {
                Response.Write("参数错误");
                return;
            }

            var ajv = new AjaxJsonValid();

            if (GameRequest.IsGet())
            {
                //获取access_token
                var appid = AppConfig.GetSetting("WXWebID");

                if (string.IsNullOrEmpty(appid))
                {
                    Response.Write("微信网站应用配置有误，请与管理员联系！");
                    return;
                }

                var dic = new SortedDictionary<string, object>
                    {
                        {"appid", appid},
                        {
                            "redirect_uri",
                            Utility.UrlEncode(
                                $"{Request.Url.Scheme}://{Request.Url.Authority}/Mobile/ScanCodeAuthorize.aspx")
                        },
                        {"response_type", "code"}, {"scope", "snsapi_login"},
                        {"state", state}
                    };
                var url = "https://open.weixin.qq.com/connect/qrconnect?" + WxAuthorize.ToUrl(dic);

                var isreg = GameRequest.GetInt("reg",0);
                if (isreg == 0) 
                    Response.Redirect(url);
                else
                {
                    Response.ContentType = "application/json;charset=UTF-8";
                    try {
                        var result = Fetch.Get(url);
                        var reg = new Regex(@"<img\b[^<>]*?\bsrc[\s\t\r\n]*=[\s\t\r\n]*[""']?[\s\t\r\n]*(?<imgUrl>[^\s\t\r\n""'<>]*)[^<>]*?/?[\s\t\r\n]*>", RegexOptions.IgnoreCase);
                        var match = reg.Match(result);
                        ajv.SetValidDataValue(match.Success);
                        if (match.Success) 
                        {
                            ajv.code = 0;
                            ajv.msg = "成功匹配二维码";
                            ajv.SetDataItem("qrurl",$"https://open.weixin.qq.com{match.Groups["imgUrl"].Value}");
                        }
                        else 
                        {
                            ajv.msg = "未找到二维码";
                            ajv.code = 404;
                        }
                    }
                    catch (Exception ex)
                    {
                        ajv.code = 500;
                        ajv.msg = ex.Message;
                    }
                    Response.Write(ajv.SerializeToJson());
                }
            }
            else if (GameRequest.IsPost())
            {
                Response.ContentType = "application/json;charset=UTF-8";
                var record = FacadeManage.aideNativeWebFacade.GetWxScanLoginRecordByKey(state);
                if (record != null)
                {
                    ajv.code = 0;
                    ajv.msg = "获取到授权记录，状态：" + (record.State == 1 ? "已使用" : "未使用");
                    record.PickClientIP = record.State == 0 ? null : record.PickClientIP;
                    record.PickDate = record.State == 0 ? null : record.PickDate;
                    ajv.SetValidDataValue(true);
                    ajv.SetDataItem("record", record);
                    if (record.State == 0)
                    {
                        record.PickClientIP = GameRequest.GetUserIP();
                        record.State = 1;
                        FacadeManage.aideNativeWebFacade.PickWxScanLoginRecord(record);
                    }
                }
                else
                {
                    ajv.code = 404;
                    ajv.msg = "未获取到授权记录";
                }

                Response.Write(ajv.SerializeToJson());
            }
        }
    }
}
