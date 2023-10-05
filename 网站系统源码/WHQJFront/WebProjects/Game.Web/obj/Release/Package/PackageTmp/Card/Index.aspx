<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Index.aspx.cs" Inherits="Game.Web.Card.Index" %>
<%@ Import Namespace="Game.Facade" %>

<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
</head>
<body>
<form runat="server">

 
    <p  style="text-align:center;">代理账号：<asp:TextBox ID="txtGameid" runat="server" Text=""></asp:TextBox>
    </p>
    <p  style="text-align:center;">代理密码：<asp:TextBox ID="txtPassword" runat="server" Text=""></asp:TextBox>
    </p>
 
  <p style="text-align:center;">
    <asp:Button ID="btnAuth" runat="server" Text="登录" OnClick="btnAuth_OnClick"/>
  </p>
</form>
</body>
</html>
