<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ShareConfigInfo.aspx.cs" Inherits="Game.Web.Module.AppManager.ShareConfigInfo" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="/scripts/common.js"></script>
    <script type="text/javascript" src="/scripts/comm.js"></script>
    <script type="text/javascript" src="/scripts/jquery.js"></script>
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <!-- 头部菜单 Start -->
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top" class="Lpd10">
                <div class="arr">
                </div>
            </td>
            <td width="1232" height="25" valign="top" align="left">
                你当前位置：系统维护 - 每日分享管理
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="返回" class="btn wd1" onclick="Redirect('ShareConfigList.aspx');" />
                <asp:Button ID="btnSave" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    分享配置</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                每日分享次数：
            </td>
            <td>
                <asp:TextBox ID="txtDayShareLmt" runat="server" CssClass="text" validate="{required:true}"></asp:TextBox>
            </td>
        </tr>
       
      
        <tr>
            <td class="listTdLeft">
               分享获得金币：
            </td>
            <td>
                <asp:TextBox ID="txtTimeShareGold" runat="server" CssClass="text" validate="{digits:true}"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
              分享获得钻石：
            </td>
            <td>
                <asp:TextBox ID="txtTimeShareDiamond" runat="server" CssClass="text" validate="{digits:true}"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                分享状态：
            </td>
            <td>
                <asp:DropDownList ID="ddlSate" runat="server" Width="156px">
                    <asp:ListItem Text="启用" Value="0"></asp:ListItem>
                    <asp:ListItem Text="禁用" Value="1"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>

    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="返回" class="btn wd1" onclick="Redirect('ShareConfigList.aspx');" />
                <asp:Button ID="btnSave1" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
