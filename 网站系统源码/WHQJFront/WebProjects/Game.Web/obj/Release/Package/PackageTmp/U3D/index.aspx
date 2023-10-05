<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="index.aspx.cs" Inherits="Game.Web.U3D.index" %>
<%@ Import Namespace="Game.Facade" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title><%=Title %></title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <link href="/css/index.css" rel="stylesheet" />
    <style>
        .ui-logo {
            width: 724px;
            position: absolute;
            left: 50%;
            top: 64px;
            margin-left: -362px;
        }
    </style>
</head>
<body>
    <div class="ui-page">
        <%--<a href="<%= PlatformDownloadOldUrl %>" style=" position: absolute;color: #deab33;display: block;width: 8rem;height: 0.5rem;left: 77%;top: 0.56rem;z-index: 100;">下载老版>></a>--%>

        <div class="ui-contact">
           
            <a href="http://wpa.b.qq.com/cgi/wpa.php?ln=2&uin=<%= Qq %>" class="ui-qq" target="_blank">
                <img src="/image/qq_pc.png" alt="" /><%= Qq %>
            </a><a href="javascript:;" class="ui-tel">
                <img src="/image/tel_pc.png" alt="" /><%= Tel %>
            </a>
        </div>


        <img src="<%= Mobilebg %>" class="ui-bg" alt="" />
        <img src="<%= Mobilelogo %>" class="ui-logo" alt="" />
        <img src="<%= MobileQrcode %>" class="ui-qrcode" alt="" />
        
        <% if (AppConfig.Mode == AppConfig.CodeMode.Demo)
           { %>
            <a href="<%= PlatformDownloadUrl %>" class="ui-btn">
                <img src="<%= Mobiledown %>" alt="" />
            </a>
            <a href="<%= PlatformDownloadabroadUrl %>" class="ui-abroadbtn">
                <img src="<%= Mobileabroaddown %>" alt="" />
            </a>
        <% }
           else
           {%>
            <a href="<%= PlatformDownloadUrl %>" class="ui-btn">
                <img src="<%= Mobiledown %>" alt="" />
            </a>
        <%} %>

       

        <img src="<%= MobileCase %>" class="ui-product" alt="" />
    </div>
</body>

</html>
