<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="LogoU3DSet.aspx.cs" Inherits="Game.Web.Module.WebManager.LogoU3DSet" %>

<%@ Register Src="~/Themes/TabSiteConfig.ascx" TagName="Config" TagPrefix="qp" %>
<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <link href="/styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="/scripts/common.js"></script>
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top" class="Lpd10">
                <div class="arr">
                </div>
            </td>
            <td width="1232" height="25" valign="top" align="left">
                你当前位置：网站系统 - 站点设置
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="Lpd10 Rpd10 hong f14">
                * 温馨提示：修改图片资源无需重启网站前台服务器即可立即生效！
            </td>
        </tr>
    </table>
    <qp:Config runat="server" ID="config"></qp:Config>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td class="listTdLeft">
                网站前台LOGO：
            </td>
            <td>
                <asp:FileUpload ID="fuFrontLogo" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
            </td>
            <td>
                <asp:Image ID="igFrontLogo" Width="120px" runat="server" />
            </td>
        </tr>
        <tr style="display:none;">
            <td class="listTdLeft">
                网站后台LOGO：
            </td>
            <td>
                <asp:FileUpload ID="fuAdminLogo" runat="server" />
            </td>
        </tr>
        <tr style="display:none;">
            <td class="listTdLeft">
            </td>
            <td>
                <asp:Image ID="igAdminLogo" Width="120px" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                网站下载按钮：
            </td>
            <td>
                <asp:FileUpload ID="fuDownload" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
            </td>
            <td>
                <asp:Image ID="igDownload" Width="120px" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                网站底部效果：
            </td>
            <td>
                <asp:FileUpload ID="fuSpreadCase" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
            </td>
            <td>
                <asp:Image ID="igSpreadCase" Width="120px" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                网站PC背景：
            </td>
            <td>
                <asp:FileUpload ID="fuPcBack" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
            </td>
            <td>
                <asp:Image ID="igPcBack" Width="120px" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                网站手机背景：
            </td>
            <td>
                <asp:FileUpload ID="fuMobileBack" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
            </td>
            <td>
                <asp:Image ID="igMobileBack" Width="120px" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                二维码Logo：
            </td>
            <td>
                <asp:FileUpload ID="fuQrSmall" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
            </td>
            <td>
                <asp:Image ID="igQrSmall" Width="120px" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                客服二维码：
            </td>
            <td>
                <asp:FileUpload ID="fuCustomer" runat="server" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
            </td>
            <td>
                <asp:Image ID="igCustomer" Width="120px" runat="server" />
            </td>
        </tr>
        <tr>
            <td style="height:10px;">
                &nbsp;
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <asp:Button ID="Button1" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
