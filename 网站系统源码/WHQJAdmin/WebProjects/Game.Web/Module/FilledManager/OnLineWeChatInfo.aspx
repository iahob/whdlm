<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="OnLineWeChatInfo.aspx.cs" Inherits="Game.Web.Module.FilledManager.OnLineWeChatInfo" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css"/>

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>

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
                你当前位置：充值系统 - 在线充值配置
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input id="btnReturn" type="button" value="返回" class="btn wd1" onclick="Redirect('OnLineWeChatList.aspx')"/>
                <asp:Button ID="btnCreate" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click"/>
            </td>
        </tr>
    </table>

     <asp:ScriptManager ID="ScriptManager1" runat="server">
    </asp:ScriptManager>
    <asp:UpdatePanel ID="UpdatePanel1" runat="server" ChildrenAsTriggers="True">
        <ContentTemplate>
            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
                <tr>
                    <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                        <div class="hg3  pd7">
                            配置信息
                        </div>
                    </td>
                </tr>
                <tr>
                    <td class="listTdLeft">
                        配置名称：
                    </td>
                    <td>
                        <asp:TextBox ID="txtConfigName" runat="server" CssClass="text"></asp:TextBox>
                        <span class="hong">*</span>
                        <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" ErrorMessage="请输入配置名称" Display="Dynamic" ControlToValidate="txtConfigName" ForeColor="Red"></asp:RequiredFieldValidator>
                    </td>
                </tr>
                <tr>
                    <td class="listTdLeft">
                        微信号：
                    </td>
                    <td>
                        <asp:TextBox ID="txtWeChat" runat="server" CssClass="text"></asp:TextBox>
                        <span class="hong">*</span>
                        <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" ErrorMessage="请输入微信账号" Display="Dynamic" ControlToValidate="txtWeChat" ForeColor="Red"></asp:RequiredFieldValidator>
                    </td>
                </tr>
                <tr>
                    <td class="listTdLeft">
                        昵称：
                    </td>
                    <td>
                        <asp:TextBox ID="txtNickName" runat="server" CssClass="text"></asp:TextBox>
                        <span class="hong">*</span>
                        <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" ErrorMessage="请输入昵称" Display="Dynamic" ControlToValidate="txtNickName" ForeColor="Red"></asp:RequiredFieldValidator>
                    </td>
                </tr>
                <tr>
            <td class="listTdLeft">
                标签类型：
            </td>
            <td>
                <asp:DropDownList ID="ddlTagID" runat="server" Width="155" Height="24" CssClass="text">
                    <asp:ListItem Text="普通" Value="0"></asp:ListItem>
                    <asp:ListItem Text="人气" Value="1"></asp:ListItem>
                </asp:DropDownList>
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
                启用禁用：
            </td>
            <td>
                <asp:RadioButtonList ID="rblNullity" runat="server" RepeatDirection="Horizontal">
                    <asp:ListItem Text="启用" Value="0" Selected="True"></asp:ListItem>
                    <asp:ListItem Text="禁用" Value="1"></asp:ListItem>
                </asp:RadioButtonList>
            </td>
        </tr> 
               
            </table>
        </ContentTemplate>
    </asp:UpdatePanel>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input id="btnReturn2" type="button" value="返回" class="btn wd1" onclick="Redirect('OnLineWeChatList.aspx')"/>
                <asp:Button ID="btnSave2" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click"/>
            </td>
        </tr>
    </table>
</form>
</body>
</html>
