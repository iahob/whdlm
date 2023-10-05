<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AgentShareLink.aspx.cs" Inherits="Game.Web.Mobile.AgentShareLink" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

    <title></title>
    <meta content="text/html; charset=utf-8" http-equiv="content-type" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no" />
    <meta name="apple-mobile-web-app-capable" content="yes" />
    <meta name="apple-touch-fullscreen" content="yes" />
    <meta name="format-detection" content="telephone=no" />
    <%--<link href="/css/friendshare.css" rel="stylesheet" />--%>
    <link rel="stylesheet" href="/css/Agentsharestyle.css" />
    <link href="/css/weixin.css" rel="stylesheet" />
    <script type="text/javascript" src="/js/zepto/1.1.6/zepto.min.js"></script>
    <%--<script type="text/javascript" src="/js/share.js"></script>--%>
    <script type="text/javascript" src="/js/weixin.js"></script>
    <style type="text/css">
        .auto-style1 {
            width: 100%;
            position: relative;
            display: block;
            z-index: -100;
            left: 0px;
            top: 0px;
        }
    </style>
</head>
<body>
    <main class="ui-container">
        <img src="/image/share_bg.png" class="auto-style1" />
        <div class="ui-qrcode">
            <img src="<%=MobileQrcode %>" />
        </div>
    </main>
    <div id="weixin-tip" class="ui-weixin-tip fn-hide">
        <img src="/image/live_weixin.png" alt="微信打开" />
        <span title="关闭" class="close">×</span>
    </div>
    <script src="/js/agenctsharereset.js"></script>
    <script type="text/javascript">
        var link = '<%=PlatformDownloadUrl %>';
        $(function () {
            var isweixin = false;
            var ua = navigator.userAgent.toLowerCase();//获取判断用的对象
            if (ua.match(/MicroMessenger/i) == "micromessenger") {
                //在微信中打开
                isweixin = true;
            }

            // 判断useragent，当前设备为Android设备
            var loadDateTime = new Date();

            // 设置时间阈值，在规定时间里面没有打开对应App的话，直接去下载地址。
            window.setTimeout(function () {
                var timeOutDateTime = new Date();
                if (timeOutDateTime - loadDateTime < 10000) {
                    if (!isweixin) {
                        window.location = link; // Android端URL Schema
                    }
                } else {
                    window.close();
                }
            },
              2000);
            var type = '<%=Plattype%>';
            if (type == "u3d") {
                window.location = "newqjunityclient://?1=1";　　 // Android端URL Schema
            } else {
                window.location = "newqjclient://?1=1";　　 // Android端URL Schema
            }

        });
  </script>
</body>
</html>
