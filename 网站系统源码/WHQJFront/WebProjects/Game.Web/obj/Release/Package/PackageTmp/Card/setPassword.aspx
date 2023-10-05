<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="setPassword.aspx.cs" Inherits="Game.Web.Card.setPassword" %>

<%@ Import Namespace="Game.Facade" %>

<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
</head>
<body>
<form runat="server">

 
    <p  style="text-align:center;">设置代理密码：<asp:TextBox ID="txtPassword" runat="server" Text=""></asp:TextBox>
    </p>
    <p  style="text-align:center;">确认代理密码：<asp:TextBox ID="txtrepeatPassword" runat="server" Text=""></asp:TextBox>
    </p>

  <p style="text-align:center;">
    <asp:Button ID="btnAuth" runat="server" Text="确认" OnClick="btnAuth_OnClick"/>
  </p>
</form>
    <script >
        location
    </script>
</body>
</html>
