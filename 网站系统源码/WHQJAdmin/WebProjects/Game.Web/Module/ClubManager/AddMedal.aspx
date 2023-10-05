<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AddMedal.aspx.cs" Inherits="Game.Web.Module.ClubManager.AddMedal" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
  <title>赠送金币</title>
  <link href="../../styles/layout.css" rel="stylesheet" type="text/css"/>

  <script type="text/javascript" src="/scripts/common.js"></script>
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
      目前操作功能：大联盟系统 - 赠送勋章
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
  <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2" style="height: 99px;">
    <tr runat="server" id="trGroupID">
      <td class="listTdLeft">
        大联盟ID：
      </td>
      <td>
        <asp:TextBox ID="txtGroupID" runat="server" CssClass="text wd4" MaxLength="7"></asp:TextBox>
        <span class="hong">*</span>
      </td>
    </tr>
    <tr>
      <td class="listTdLeft">
        赠送勋章数：
      </td>
      <td>
        <asp:TextBox ID="txtMedal" runat="server" CssClass="text wd4" ></asp:TextBox>
        <span class="hong">*</span>
        <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" ErrorMessage="请输入赠送勋章" Display="Dynamic" ControlToValidate="txtMedal" ForeColor="Red"></asp:RequiredFieldValidator>
       <%-- <asp:RegularExpressionValidator ID="RegularExpressionValidator2" runat="server" ErrorMessage="赠送金币格式不正确" Display="Dynamic" ControlToValidate="txtGold" ForeColor="Red" ></asp:RegularExpressionValidator>--%>
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

