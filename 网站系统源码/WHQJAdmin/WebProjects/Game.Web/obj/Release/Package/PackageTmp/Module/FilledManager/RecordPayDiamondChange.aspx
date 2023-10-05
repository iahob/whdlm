<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RecordPayDiamondChange.aspx.cs" Inherits="Game.Web.Module.FilledManager.RecordPayDiamondChange" %>

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
        你当前位置：充值系统 - 充值记录 - 订单处理
      </td>
    </tr>
  </table>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
      <td class="titleOpBg Lpd10">
        <input id="btnReturn" type="button" value="关闭" class="btn wd1" onclick="window.close();"/>
        <asp:Button ID="btnSave" runat="server" Text="处理订单" CssClass="btn wd2" OnClick="btnSave_Click"/>
      </td>
    </tr>
  </table>
      <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
          <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
            <div class="hg3  pd7">
              订单信息
            </div>
          </td>
        </tr>
        <tr>
          <td class="listTdLeft">
            订单号：
          </td>
          <td>
            <asp:Label runat="server" ID="lblOrderID"></asp:Label>
          </td>
        </tr>
        <tr>
          <td class="listTdLeft">
            订单金额：
          </td>
          <td>
            <asp:Label runat="server" ID="lblAmount"></asp:Label>
          </td>
        </tr>
        <tr>
          <td class="listTdLeft">
            订单状态：
          </td>
          <td>
            <asp:Label runat="server" ID="lblStatus"></asp:Label>
          </td>
        </tr>
      </table>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
      <td class="titleOpBg Lpd10">
        <input id="btnReturn2" type="button" value="关闭" class="btn wd1" onclick="window.close()"/>
        <asp:Button ID="btnSave2" runat="server" Text="处理订单" CssClass="btn wd2" OnClick="btnSave_Click"/>
      </td>
    </tr>
  </table>
</form>
</body>
</html>
