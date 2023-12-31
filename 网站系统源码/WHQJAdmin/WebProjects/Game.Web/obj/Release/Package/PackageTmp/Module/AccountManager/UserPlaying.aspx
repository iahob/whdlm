﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="UserPlaying.aspx.cs" Inherits="Game.Web.Module.AccountManager.UserPlaying" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
  <title></title>
  <link href="/styles/layout.css" rel="stylesheet" type="text/css"/>
  <script type="text/javascript" src="/scripts/common.js"></script>
  <script type="text/javascript" src="/scripts/comm.js"></script>
  <script type="text/javascript" src="/scripts/My97DatePicker/WdatePicker.js">
  </script>
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
        你当前位置：数据分析 - 房间在线玩家
      </td>
    </tr>
  </table>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg">
    <tr>
      <td class="listTdLeft" style="width: 80px">
        用户查询：
      </td>
      <td>
        <asp:TextBox ID="txtSearch" runat="server" CssClass="text"></asp:TextBox>
        <asp:DropDownList ID="ddlSearchType" runat="server">
          <asp:ListItem Value="1">按游戏ID</asp:ListItem>
        </asp:DropDownList>
        <asp:DropDownList ID="ddlServerID" runat="server">
        </asp:DropDownList>
        <input type="button" class="btnLine"/>
        <asp:Button ID="btnQueryOrder" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQueryOrder_Click"/>
        <input type="button" onclick="Redirect('UserPlaying.aspx');" class="btn wd1" value="刷新"/>
        <input type="button" class="btnLine"/>
        <asp:Button ID="btnCleanLocker" runat="server" Text="清除卡线" CssClass="btn wd2" OnClick="btnCleanLocker_Click"/>
      </td>
    </tr>
  </table>
  <div id="content" class="Tmg7">
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
      <tr align="center" class="bold">
        <td class="listTitle">
          <input type="checkbox" name="chkAll" onclick="SelectAll(this.checked);" />
        </td>
        <td class="listTitle">
          用户ID
        </td>
        <td class="listTitle2">
          用户昵称
        </td>
        <td class="listTitle2">
          游戏ID
        </td>
        <td class="listTitle2">
          所在游戏
        </td>
        <td class="listTitle2">
          所在房间
        </td>
        <td class="listTitle2">
          进入IP
        </td>
        <td class="listTitle2">
          进入时间
        </td>
      </tr>
      <asp:Repeater ID="rptData" runat="server">
        <ItemTemplate>
          <tr align="center" class="list" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
              onmouseout="this.style.backgroundColor = currentcolor">
            <td style="width: 30px;">
              <input name='cid' type='checkbox' value='<%# Eval("UserID").ToString()%>' />
            </td>
            <td>
              <%# Eval("UserID").ToString() %>
            </td>
            <%# GetAccountsInfo(Convert.ToInt32(Eval("UserID"))) %>
            <%# GetGameRoomInfo(Convert.ToInt32(Eval("KindID")),Convert.ToInt32(Eval("ServerID"))) %>
            <td>
              <%# Eval("EnterIP") %>
            </td>
            <td>
              <%# Eval("CollectDate") %>
            </td>
          </tr>
        </ItemTemplate>
        <AlternatingItemTemplate>
          <tr align="center" class="listBg" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
              onmouseout="this.style.backgroundColor = currentcolor">
            <td style="width: 30px;">
              <input name='cid' type='checkbox' value='<%# Eval("UserID").ToString()%>' />
            </td>
            <td>
              <%# Eval("UserID").ToString() %>
            </td>
            <%# GetAccountsInfo(Convert.ToInt32(Eval("UserID"))) %>
            <%# GetGameRoomInfo(Convert.ToInt32(Eval("KindID")),Convert.ToInt32(Eval("ServerID"))) %>
            <td>
              <%# Eval("EnterIP") %>
            </td>
            <td>
              <%# Eval("CollectDate") %>
            </td>
          </tr>
        </AlternatingItemTemplate>
      </asp:Repeater>
      <asp:Literal runat="server" ID="litNoData" Visible="false" Text="<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>"></asp:Literal>
    </table>
  </div>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
      <td class="listTitleBg" style="">
      </td>
      <td align="right" class="page">
        <gsp:AspNetPager ID="anpPage" OnPageChanged="anpPage_PageChanged" runat="server" AlwaysShow="true" FirstPageText="首页" LastPageText="末页"
                         PageSize="20" NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table"
                         NumericButtonCount="5" CustomInfoHTML="总数：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%" UrlPaging="false">
        </gsp:AspNetPager>
      </td>
    </tr>
  </table>
</form>
</body>
</html>
