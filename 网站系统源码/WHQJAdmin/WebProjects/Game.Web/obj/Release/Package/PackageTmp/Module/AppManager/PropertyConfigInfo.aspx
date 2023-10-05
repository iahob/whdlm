﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PropertyConfigInfo.aspx.cs" Inherits="Game.Web.Module.AppManager.PropertyConfigInfo" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
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
                你当前位置：系统维护 - 道具管理
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="返回" class="btn wd1" onclick="Redirect('PropertyConfigList.aspx');" />
                <asp:Button ID="btnSave" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    道具信息</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                道具名称：
            </td>
            <td>
                <asp:TextBox ID="txtName" runat="server" CssClass="text" validate="{required:true}"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                购买类型：
            </td>
            <td>
                <asp:DropDownList ID="ddlType" runat="server" Width="156px">
                    <asp:ListItem Text="钻石购买" Value="0"></asp:ListItem>
                    <asp:ListItem Text="游戏币购买" Value="1"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                购买价格：
            </td>
            <td>
                <asp:TextBox ID="txtPrice" runat="server" CssClass="text" validate="{number:true}" Text="0"></asp:TextBox>
                <span class="hong">钻石购买表示1钻石买多少个道具，游戏币购买表示1道具多少游戏币</span>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                购买赠送金币：
            </td>
            <td>
                <asp:TextBox ID="txtBuyResultsGold" runat="server" CssClass="text" validate="{digits:true}"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
              使用赠送金币：
            </td>
            <td>
                <asp:TextBox ID="txtUseResultsGold" runat="server" CssClass="text" validate="{digits:true}"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                道具排序：
            </td>
            <td>
                <asp:TextBox ID="txtSortID" runat="server" CssClass="text" validate="{number:true}"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                道具状态：
            </td>
            <td>
                <asp:DropDownList ID="ddlSate" runat="server" Width="156px">
                    <asp:ListItem Text="启用道具" Value="0"></asp:ListItem>
                    <asp:ListItem Text="禁用道具" Value="1"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                使用说明：
            </td>
            <td>
                <asp:TextBox ID="txtRegulationsInfo" runat="server" CssClass="text" Width="300" TextMode="MultiLine" Height="60" MaxLength="256"></asp:TextBox>&nbsp;使用说明字符长度尽量50字以内
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
            </td>
            <td class="hong">
                注意：修改成功后需重启游戏服务端才可生效
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="返回" class="btn wd1" onclick="Redirect('PropertyConfigList.aspx');" />
                <asp:Button ID="btnSave1" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
