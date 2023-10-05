<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="TaskInfo.aspx.cs" Inherits="Game.Web.Module.AccountManager.TaskInfo" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/My97DatePicker/WdatePicker.js"></script>
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
                你当前位置：用户系统 - 任务管理
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="返回" class="btn wd1" onclick="Redirect('TaskList.aspx')" />
                <input class="btnLine" type="button" />
                <asp:Button ID="btnSave" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">新增任务信息</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                所在游戏：
            </td>
            <td>
                <asp:DropDownList ID="ddlKindID" runat="server" Width="156px" OnSelectedIndexChanged="ddlKindID_SelectedIndexChanged" AutoPostBack="true">
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                所在房间：
            </td>
            <td>
                <asp:DropDownList ID="ddlServerID" runat="server" Width="156px">
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                任务名称：
            </td>
            <td>
                <asp:TextBox ID="txtTaskName" runat="server" CssClass="text" MaxLength="20"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" ErrorMessage="请输入任务名称" Display="Dynamic" ControlToValidate="txtTaskName" ForeColor="Red"></asp:RequiredFieldValidator>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                任务类型：
            </td>
            <td>
                <asp:DropDownList ID="ddlTaskType" runat="server" Width="156px">
                    <asp:ListItem Text="其他任务" Value="0"></asp:ListItem>
                    <asp:ListItem Text="总局任务" Value="1"></asp:ListItem>
                    <asp:ListItem Text="胜局任务" Value="2"></asp:ListItem>
                    <asp:ListItem Text="首胜任务" Value="4"></asp:ListItem>
                    <asp:ListItem Text="连胜任务" Value="8"></asp:ListItem>
                    <%-- <asp:ListItem Text="充值" Value="9"></asp:ListItem>
                    <asp:ListItem Text="分享" Value="10"></asp:ListItem>
                    <asp:ListItem Text="邀请" Value="11"></asp:ListItem>
                    <asp:ListItem Text="实名验证" Value="12"></asp:ListItem>
                    <asp:ListItem Text="手机绑定" Value="13"></asp:ListItem>
                    <asp:ListItem Text="指定牌型" Value="14"></asp:ListItem>
                    <asp:ListItem Text="指定分数" Value="15"></asp:ListItem>
                    <asp:ListItem Text="签到" Value="16"></asp:ListItem>--%>
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                用户类型：
            </td>
            <td>
                <asp:DropDownList ID="ddlUserType" runat="server" Width="156px">
                    <asp:ListItem Text="所有用户" Value="0"></asp:ListItem>
                    <asp:ListItem Text="普通用户" Value="1"></asp:ListItem>
                    <asp:ListItem Text="会员用户" Value="2"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                时间类型：
            </td>
            <td>
                <asp:DropDownList ID="ddlTimeType" runat="server" Width="156px">
                    <asp:ListItem Text="一次任务" Value="1"></asp:ListItem>
                    <asp:ListItem Text="每日任务" Value="2"></asp:ListItem>
                    <asp:ListItem Text="每周任务" Value="4"></asp:ListItem>
                    <asp:ListItem Text="每月任务" Value="8"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>
        <%--<tr>
            <td class="listTdLeft">
                所在比赛：
            </td>
            <td>
                <asp:DropDownList ID="ddlMatchID" runat="server" Width="156px">
                </asp:DropDownList>
            </td>
        </tr>--%>
        <tr>
            <td class="listTdLeft">
                冻结状态：
            </td>
            <td>
                <asp:DropDownList ID="ddlState" runat="server" Width="156px">
                    <asp:ListItem Text="正常" Value="0"></asp:ListItem>
                    <asp:ListItem Text="冻结" Value="1"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                完成目标数：
            </td>
            <td>
                <asp:TextBox ID="txtInnings" runat="server" CssClass="text"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" ErrorMessage="请输入完成目标数" Display="Dynamic" ControlToValidate="txtInnings" ForeColor="Red"></asp:RequiredFieldValidator>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                奖励金币：
            </td>
            <td>
                <asp:TextBox ID="txtGold" runat="server" CssClass="text" MaxLength="20" Text="0"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" ErrorMessage="请输入奖励金币" Display="Dynamic" ControlToValidate="txtGold" ForeColor="Red"></asp:RequiredFieldValidator>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                奖励钻石：
            </td>
            <td>
                <asp:TextBox ID="txtDiamond" runat="server" CssClass="text" MaxLength="20" Text="0"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server" ErrorMessage="请输入奖励金币" Display="Dynamic" ControlToValidate="txtDiamond" ForeColor="Red"></asp:RequiredFieldValidator>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                奖励奖券：
            </td>
            <td>
                <asp:TextBox ID="txtMedal" runat="server" CssClass="text" MaxLength="20" Text="0"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator6" runat="server" ErrorMessage="请输入奖励奖券" Display="Dynamic" ControlToValidate="txtMedal" ForeColor="Red"></asp:RequiredFieldValidator>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                任务描述：
            </td>
            <td>
                <asp:TextBox ID="txtDescription" runat="server" CssClass="text" TextMode="MultiLine" Width="300px" Height="50px"></asp:TextBox>
                <span class="hong">*</span>
                <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" ErrorMessage="请输入任务描述" Display="Dynamic" ControlToValidate="txtDescription" ForeColor="Red"></asp:RequiredFieldValidator>
            </td>
        </tr>

               <%-- <tr>
                    <td class="listTdLeft">排列序号：</td>
                    <td><asp:TextBox ID="txtSortID" runat="server" CssClass="text wd7" Text="0"></asp:TextBox>
                    &nbsp;<span class="hong">*</span>
                        <asp:RequiredFieldValidator ID="rf3" runat="server" ForeColor="Red" Display="Dynamic" ControlToValidate="txtSortID" ErrorMessage="排序字段不能为空"></asp:RequiredFieldValidator>
                        <asp:RegularExpressionValidator ID="re2" runat="server" ForeColor="Red" Display="Dynamic" ControlToValidate="txtSortID" ValidationExpression="^[0-9]\d*$" ErrorMessage="排序字段必须为正整数"></asp:RegularExpressionValidator>
                    </td>
                </tr>--%>
        <tr>
            <td height="20" colspan="2">
                &nbsp;
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="返回" class="btn wd1" onclick="Redirect('TaskList.aspx')" />
                <input class="btnLine" type="button" />
                <asp:Button ID="btnSave1" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
