<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GamePackageConfigInfo.aspx.cs" Inherits="Game.Web.Module.AppManager.GamePackageConfigInfo" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>
    <title></title>
    <style type="text/css">
        .auto-style1 {
            height: 21px;
            white-space: nowrap;
            width: 150px;
            text-align: right;
            text-overflow: ellipsis;
            word-break: keep-all;
            overflow: hidden;
            padding: 0px 5px;
        }
        .auto-style2 {
            height: 21px;
        }
    </style>
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
                你当前位置：系统维护 - 签到礼包配置
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input id="btnReturn" type="button" value="返回" class="btn wd1" onclick="Redirect('GamePackageConfigList.aspx')" />
                <asp:Button ID="btnCreate" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    礼包信息</div>
            </td>
        </tr>
        <tr>
            <td class="auto-style1">
                礼包名称：
            </td>
            <td class="auto-style2">
                <asp:TextBox ID="txtName" runat="server" CssClass="text"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" ErrorMessage="请输入礼包名称" ControlToValidate="txtName" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
               
            </td>
        </tr> 
        
        <tr>
            <td class="listTdLeft">
                礼包类型：
            </td>
            <td>
                <asp:DropDownList ID="ddlTypeID" runat="server" Width="155" Height="24" CssClass="text">
                    <asp:ListItem Text="抽奖签到礼包" Value="0"></asp:ListItem>
                    <asp:ListItem Text="累计签到礼包" Value="1"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>
         <tr>
            <td class="listTdLeft">
               平台类型：
            </td>
            <td>
                <asp:DropDownList ID="Dropplatform" runat="server" Width="155px">
                    <asp:ListItem Text="通用" Value="0"></asp:ListItem>
                    <asp:ListItem Text="LUA" Value="1"></asp:ListItem>
                    <asp:ListItem Text="H5" Value="2"></asp:ListItem>
                    <asp:ListItem Text="U3D" Value="3"></asp:ListItem>
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
                礼包描述：
            </td>
            <td>
                <asp:TextBox ID="txtDescribe" runat="server" CssClass="text" Width="300" TextMode="MultiLine" Height="60" MaxLength="256"></asp:TextBox>&nbsp;使用说明字符长度尽量50字以内
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

    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input id="btnReturn2" type="button" value="返回" class="btn wd1" onclick="Redirect('GamePackageConfigList.aspx')" />
                <asp:Button ID="btnSave2" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
