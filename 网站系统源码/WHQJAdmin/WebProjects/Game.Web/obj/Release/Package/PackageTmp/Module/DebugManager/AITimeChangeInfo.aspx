<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AITimeChangeInfo.aspx.cs" Inherits="Game.Web.Module.DebugManager.AITimeChangeInfo" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <link href="/styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="/scripts/common.js"></script>
    <script type="text/javascript" src="/scripts/comm.js"></script>
    <script type="text/javascript" src="/scripts/jquery.js"></script>
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
        <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
            <tr>
                <td width="19" height="25" valign="top" class="Lpd10">
                    <div class="arr"></div>
                </td>
                <td width="1232" height="25" valign="top" align="left">
                    你当前位置：调试系统 - AI设置 - AI人数调整 - AI人数调整修改
                </td>
            </tr>
        </table>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
                <td class="titleOpBg Lpd10">
                    <input id="btnReturn" type="button" value="返回" class="btn wd1" onclick="Redirect('AITimeChangeList.aspx')" />
                    <asp:Button ID="btnCreate" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
                </td>
            </tr>
        </table>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2"> 
            <tr>
                <td class="listTdLeft">AI批次时间：</td>
                <td>
                   <asp:TextBox runat="server" ID="txtAITime" Enabled="False" MaxLength="8"></asp:TextBox>
                  <span class="hong">*</span>
                    <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" ErrorMessage="请输入赠送数值" Display="Dynamic" ControlToValidate="txtAITime" ForeColor="Red"></asp:RequiredFieldValidator>
                    <asp:RegularExpressionValidator ID="RegularExpressionValidator2" runat="server" ErrorMessage="赠送数值格式不正确" Display="Dynamic" ControlToValidate="txtAITime" ForeColor="Red" ValidationExpression="^[0-9]\d*$"></asp:RegularExpressionValidator>
                 
                </td>
            </tr>
            <tr>
                <td class="listTdLeft">AI数量：</td>
                <td>
                   <asp:TextBox runat="server" ID="txtAICount" MaxLength="8"></asp:TextBox>
                  <span class="hong">*</span>
                     <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" ErrorMessage="请输入AI数量" Display="Dynamic" ControlToValidate="txtAICount" ForeColor="Red"></asp:RequiredFieldValidator>
                        <asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server" ErrorMessage="AI数量格式不正确" Display="Dynamic" ControlToValidate="txtAICount" ForeColor="Red" ValidationExpression="^[0-9]\d*$"></asp:RegularExpressionValidator>
                </td>
            </tr>
        </table>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
                <td class="titleOpBg Lpd10">
                    <input id="btnReturn2" type="button" value="返回" class="btn wd1" onclick="Redirect('AITimeChangeList.aspx')" />
                    <asp:Button ID="btnSave2" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
                </td>
            </tr>
        </table>
    </form>
</body>
</html>
