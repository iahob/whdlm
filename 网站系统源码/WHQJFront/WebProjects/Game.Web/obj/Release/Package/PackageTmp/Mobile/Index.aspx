<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Index.aspx.cs" Inherits="Game.Web.Mobile.Index" %>

<!DOCTYPE html>
<html>
    <head runat="server">
        <title><%=Title %></title>
        <meta content="text/html; charset=utf-8" http-equiv="content-type"/>
        <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
        <meta name="apple-mobile-web-app-capable" content="yes">
        <meta name="apple-touch-fullscreen" content="yes"/>
        <meta name="format-detection" content="telephone=no">
        <script src="/js/reset.js" type="text/javascript"></script>
        <link href="/css/mindex.css" rel="stylesheet" type="text/css"/>
        <link href="/css/weixin.css" rel="stylesheet" />
        <script src="/js/zepto/1.1.6/zepto.min.js" type="text/javascript"></script>
        <script src="/js/weixin.js" type="text/javascript"></script>
    </head>
    <body>
          <div class="ui-page">
            <img src="<%= Mobilebg %>" class="ui-bg"/>
            <img src="<%= Mobilelogo %>" class="ui-logo" />
            <img src="<%= MobileQrcode %>" class="ui-qrcode" />
            <a href="<%= PlatformDownloadUrl %>" class="ui-btn index"><img src="<%= Mobiledown %>" /></a>
            <img src="<%= MobileCase %>" class="ui-product" />
          
          </div>

         <footer class="ui-footer">
           <a href="http://wpa.b.qq.com/cgi/wpa.php?ln=2&uin=<%= Qq %>" class="fn-left" target="_blank">
             <img src="/image/qqbtn.png" />QQ咨询
           </a> 
           <img src="/image/line.png"  class="ui-line fn-left" />
           <a href="tel:<%= Tel %>"  class="fn-left">
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
