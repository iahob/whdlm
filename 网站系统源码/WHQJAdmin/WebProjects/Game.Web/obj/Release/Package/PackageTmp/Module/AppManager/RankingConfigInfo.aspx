<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RankingConfigInfo.aspx.cs" Inherits="Game.Web.Module.AppManager.RankingConfigInfo" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

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
                你当前位置：系统维护 - 排行配置
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input id="btnReturn" type="button" value="返回" class="btn wd1" onclick="Redirect('RankingConfigList.aspx')" />
                <asp:Button ID="btnCreate" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    排行信息</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                排行榜类型：
            </td>
            <td>
                <asp:DropDownList ID="ddlType" runat="server" Width="155" Height="24" CssClass="text">
                   <%-- <asp:ListItem Text="财富排行榜" Value="1"></asp:ListItem>--%>
                    <asp:ListItem Text="胜局排行榜" Value="2"></asp:ListItem>

                </asp:DropDownList>
            </td>
        </tr> 
        <tr>
            <td class="listTdLeft">
                排行榜排名：
            </td>
            <td>
                <asp:DropDownList ID="ddlRank" runat="server" Width="155" Height="24" CssClass="text">
                    <asp:ListItem Text="第一名" Value="1"></asp:ListItem>
                    <asp:ListItem Text="第二名" Value="2"></asp:ListItem>
                    <asp:ListItem Text="第三名" Value="3"></asp:ListItem>
                    <asp:ListItem Text="第4-10名" Value="4"></asp:ListItem>
                    <asp:ListItem Text="第11-20名" Value="5"></asp:ListItem>
                    <asp:ListItem Text="第21-50名" Value="6"></asp:ListItem>
                    <asp:ListItem Text="第51-100名" Value="7"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr> 
        <tr>
            <td class="listTdLeft">
                奖励钻石数：
            </td>
            <td>
                <asp:TextBox ID="txtDiamond" runat="server" CssClass="text"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" ErrorMessage="请输入钻石数" ControlToValidate="txtDiamond" Display="Dynamic" ForeColor="Red" Text="0"></asp:RequiredFieldValidator>
                <asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server" ErrorMessage="钻石数格式不正确" Display="Dynamic" ControlToValidate="txtDiamond" ValidationExpression="^\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
            </td>
        </tr> 
        <tr>
            <td class="listTdLeft">
                奖励金币数：
            </td>
            <td>
                <asp:TextBox ID="txtGold" runat="server" CssClass="text"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" ErrorMessage="请输入金币数" ControlToValidate="txtDiamond" Display="Dynamic" ForeColor="Red" Text="0"></asp:RequiredFieldValidator>
                <asp:RegularExpressionValidator ID="RegularExpressionValidator2" runat="server" ErrorMessage="金币数格式不正确" Display="Dynamic" ControlToValidate="txtDiamond" ValidationExpression="^\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
            </td>
        </tr> 
         <tr>
            <td class="listTdLeft">
                奖励奖券数：
            </td>
            <td>
                <asp:TextBox ID="txtAwardTicket" runat="server" CssClass="text"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" ErrorMessage="请输入奖券数" ControlToValidate="txtAwardTicket" Display="Dynamic" ForeColor="Red" Text="0"></asp:RequiredFieldValidator>
                <asp:RegularExpressionValidator ID="RegularExpressionValidator3" runat="server" ErrorMessage="奖券数格式不正确" Display="Dynamic" ControlToValidate="txtAwardTicket" ValidationExpression="^\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
            </td>
        </tr> 
        <tr>
            <td colspan="2" >
                <span style="color:red; padding-left:5%;">
                    注：奖励数值填写0时，客户端将不显示该奖励。
                </span>
            </td>
        </tr>
    </table>

    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input id="btnReturn2" type="button" value="返回" class="btn wd1" onclick="Redirect('RankingConfigList.aspx')" />
                <asp:Button ID="btnSave2" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
