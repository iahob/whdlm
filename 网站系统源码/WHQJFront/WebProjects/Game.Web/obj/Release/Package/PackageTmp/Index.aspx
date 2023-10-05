<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Index.aspx.cs" Inherits="Game.Web.Index" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">

<head runat="server">
    <title><%=Title %></title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <link href="/css/index.css" rel="stylesheet" />
</head>
<body>
    <div class="ui-page">

    <div class="ui-contact">
      <a href="http://wpa.b.qq.com/cgi/wpa.php?ln=2&uin=<%= Qq %>" class="ui-qq" target="_blank">
        <img src="/image/qq_pc.png" alt="" /><%= Qq %>
      </a><a href="javascript:;" class="ui-tel">
        <img src="/image/tel_pc.png" alt=""/><%= Tel %>
      </a>
    </div>


    <img src="<%= Mobilebg %>" class="ui-bg" alt=""/>
    <img src="<%= Mobilelogo %>" class="ui-logo" alt=""/>
    <img src="<%= MobileQrcode %>" class="ui-qrcode" alt=""/>
    <a href="<%= PlatformDownloadUrl %>" class="ui-btn index">
       <img src="<%= Mobiledown %>" alt=""/>
    </a>
    <img src="<%= MobileCase %>" class="ui-product" alt=""/>
  </div>
</body>

</html>