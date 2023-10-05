<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="index.aspx.cs" Inherits="Game.Web.U3D.Mobile.index" %>

<%@ Import Namespace="Game.Facade" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title><%=Title %></title>
    <meta content="text/html; charset=utf-8" http-equiv="content-type" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="apple-touch-fullscreen" content="yes" />
    <meta name="format-detection" content="telephone=no">
    <script src="../../js/reset.js" type="text/javascript"></script>
    <link href="../../css/mindex.css" rel="stylesheet" type="text/css" />
    <link href="../../css/weixin.css" rel="stylesheet" />
    <script src="../../js/zepto/1.1.6/zepto.min.js" type="text/javascript"></script>
    <script src="../../js/weixin.js"></script>
    <style>
        .ui-logo {
            width: 7.24rem;
            position: absolute;
            left: 50%;
            top: 0.56rem;
            margin-left: -3.62rem;
        }
    </style>
</head>
<body>
    <div class="ui-page">

        <img src="<%= Mobilebg %>" class="ui-bg" />
        <%--<a href="<%= PlatformDownloadOldUrl %>" style=" position: absolute;color: #deab33;display: block;width: 2rem;height: 0.5rem;left: 77%;top: 0.56rem;z-index: 100;">下载老版>></a>--%>
        <img src="<%= Mobilelogo %>" class="ui-logo" />
        <img src="<%= MobileQrcode %>" class="ui-qrcode" />
        <% if (AppConfig.Mode == AppConfig.CodeMode.Demo)
            { %>
        <a href="<%= PlatformDownloadUrl %>" class="ui-btn">
            <img src="<%= Mobiledown %>" /></a>
        <a href="<%= PlatformDownloadabroadUrl %>" class="ui-abroadbtn">
            <img src="<%= Mobileabroaddown %>" /></a>
        <% }
            else
            {%>
        <a href="<%= PlatformDownloadUrl %>" class="ui-btn">
            <img src="<%= Mobiledown %>" /></a>
        <%} %>

        <img src="<%= MobileCase %>" class="ui-product" />
       
    </div>

    <footer class="ui-footer">
        <a href="http://wpa.b.qq.com/cgi/wpa.php?ln=2&uin=<%= Qq %>" class="fn-left" target="_blank">
            <img src="/image/qqbtn.png" />QQ咨询
        </a>
        <img src="/image/line.png" class="ui-line fn-left" />
        <a href="tel:<%= Tel %>" class="fn-left">
            <img src="/image/telbtn.png" />电话咨询
        </a>
    </footer>
    <div id="weixin-tip" class="ui-weixin-tip fn-hide">
        <p>
            <img src="/image/live_weixin.png" alt="微信打开" />
            <span title="关闭" class="close">×</span>
        </p>
    </div>

    <%--<script type="text/javascript" src="/js/mindex.js"></script>
        <script type="text/javascript">
          $(document).ready(function() {
            var msg = "<%=msg %>";
            var action = "<%=action %>";
            if (action === "payreturn" && msg) {
              alert(msg);
              window.location.href = "newryclient://?action=3&msg=" + msg;
            }
          });
        </script>--%>
</body>
</html>
