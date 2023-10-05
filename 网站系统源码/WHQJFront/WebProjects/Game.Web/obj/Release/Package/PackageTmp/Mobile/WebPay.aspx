<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="WebPay.aspx.cs" Inherits="Game.Web.Mobile.WebPay" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:Button ID="btnWxPay" runat="server" Text="微信支付" OnClick="btnWxPay_Click" />
        <asp:Button ID="btnAliPay" runat="server" Text="支付宝支付" OnClick="btnAliPay_Click"  />
    </div>
    </form>
</body>
</html>
