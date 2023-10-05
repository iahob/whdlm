<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AddSensitiveWords.aspx.cs" Inherits="Game.Web.Module.AccountManager.AddSensitiveWords" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>录入敏感字符</title>
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css"/>

    <script type="text/javascript" src="/scripts/common.js"></script>
    <script type="text/javascript" src="/scripts/jquery.js"></script>

 
</head>
<body>
<!-- 头部菜单 Start -->
<table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
    <tr>
        <td width="19" height="25" valign="top" class="Lpd10">
            <div class="arr">
            </div>
        </td>
        <td width="1232" height="25" valign="top" align="left">
            目前操作功能：用户系统 - 录入敏感字符
        </td>
    </tr>
</table>
<!-- 头部菜单 End -->
<form runat="server" id="form1">
    <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="关闭" class="btn wd1" onclick="window.close();"/>
                <asp:Button ID="btnSave" runat="server" Text="确认" CssClass="btn wd1" OnClick="btnSave_Click"/>
            </td>
        </tr>
    </table>
    <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2" style="height: 136px;">
        <tr runat="server" id="trGameID">
            <td class="listTdLeft">
                敏感字符：
            </td>
            <td>
                <asp:TextBox ID="txtWords" runat="server" TextMode="MultiLine" Height="50" Width="300" ></asp:TextBox>
                <span class="hong">*</span>
            </td>
        </tr>
    </table>
    <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="关闭" class="btn wd1" onclick="window.close();"/>
                <asp:Button ID="btnSave1" runat="server" Text="确认" CssClass="btn wd1" OnClick="btnSave_Click"/>
            </td>
        </tr>
    </table>
</form>
</body>
</html>

