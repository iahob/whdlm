<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GroupDebugUserList.aspx.cs" Inherits="Game.Web.Module.DebugManager.GroupDebugUserList" %>
<%@ Import Namespace="System.Web.Services.Description" %>
<%@ Import Namespace="Game.Facade" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
  <title></title>
  <link href="../../styles/layout.css" rel="stylesheet" type="text/css"/>
  <script src="../../scripts/jquery.js"></script>
  <script type="text/javascript" src="../../scripts/common.js"></script>

  <script type="text/javascript" src="../../scripts/comm.js"></script>
  <style type="text/css">
    .tbGroup {
      margin-bottom: 10px;
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
        你当前位置：调试系统 - 增减账号 - 联盟列表 - 用户列表
      </td>
    </tr>
  </table>
  <!-- 头部菜单 End -->
<div class="tbGroup">
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box">
      <tr align="center" class="bold">
        <td class="listTitle2">
          大联盟ID
        </td>
        <td class="listTitle2">
          大联盟名称
        </td>
        <td class="listTitle2">
          联盟人数<br/>（玩家人数 / 调试账号人数）
        </td>
        <td class="listTitle2">
          在线总人数<br/>（玩家人数 / 调试账号人数）
        </td>
        <td class="listTitle2">
          大联盟总积分
        </td>
        <td class="listTitle2">
          剩余未分配积分
        </td>
        <td class="listTitle2">
          调试账号可分配积分
        </td>
        <td class="listTitle2">
          玩家携带积分
        </td>
        <td class="listTitle2">
          调试账号携带积分
        </td>
        <td class="listTitle2">
          操作
        </td>
      </tr>
      <asp:Repeater ID="rptGroupData" runat="server">
        <ItemTemplate>
          <tr align="center" class="list" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
              onmouseout="this.style.backgroundColor = currentcolor">
            <td>
              <%# Eval("GroupID") %>
            </td>
            <td>
              <%# Eval("GroupName") %>
            </td>
            <td>
              <%# Convert.ToInt64(Eval("UserCount"))+ Convert.ToInt64(Eval("AICount")) %> 
              (<%# Eval("UserCount") %>/  <%# Eval("AICount") %>)
            </td>
            <td>
              <%# Convert.ToInt64(Eval("OnlineUserCount"))+ Convert.ToInt64(Eval("OnlinAICount")) %> 
              (<%# Eval("OnlineUserCount") %>/ <%# Eval("OnlinAICount") %>)
            </td>
            <td>
              <%# Eval("AllScore") %>
            </td>
            <td>
              <%# Eval("LaveNoDistributionScore") %>
            </td>
            <td>
              <%# Eval("AIDistributionScore") %>
            </td>
            <td>
              <%# Eval("UserCarryScore") %>
            </td>
            <td>
              <%# Eval("AICarryScore") %>
            </td>
            <td>
              <a href="javascript:;" class="l" onclick="openWindowOwn('AddAIAccounts.aspx?groupid=<%# Eval("GroupID") %>', '', 750, 348);">添加账号</a>
            </td>
          </tr>
        </ItemTemplate>
      </asp:Repeater>
      <asp:Literal runat="server" ID="Literal1" Visible="false" Text="<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>"></asp:Literal>
    </table>
</div>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg">
    <tr>
      <td align="center" style="width: 80px">
        数据查询：
      </td>
      <td>
        <asp:TextBox ID="txtSearch" runat="server" CssClass="text"></asp:TextBox>
        <asp:DropDownList ID="ddlSearchType" runat="server">
            <asp:ListItem Value="0">按大联盟ID</asp:ListItem>
            <asp:ListItem Value="1">按盟主游戏ID</asp:ListItem>
        </asp:DropDownList>
        <asp:Button ID="btnQuery" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery_Click"/>
      </td>
    </tr>
  </table>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
    <tr>
      <td height="39" class="titleOpBg">
        <asp:Button ID="btnForceDown" runat="server" Text="强制下线" CssClass="btn wd3"  OnClientClick="return deleteop()" OnClick="btnForceDown_Click"/>
        <asp:Button ID="btnForceOut" runat="server" Text="强制退出" CssClass="btn wd3"  OnClientClick="return deleteop()" OnClick="btnForceOut_Click"/>
        <asp:Button ID="btnRefresh" runat="server" Text="刷新" CssClass="btn wd1" OnClick="btnRefresh_Click"   />
      </td>
    </tr>
  </table>
  <div id="content">
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
      <tr align="center" class="bold">
        <td class="listTitle">
          <input type="checkbox" name="chkAll" onclick="SelectAll(this.checked);"/>
        </td>
        <td class="listTitle2">
          玩家ID
          </td>
        <td class="listTitle2">
          玩家昵称
        </td>
        <td class="listTitle2">
          状态
        </td>
        <td class="listTitle2">
          身份
        </td>
        <td class="listTitle2">
          类别
        </td>
        <td class="listTitle2">
          所在游戏
        </td>
        <td class="listTitle2">
          房间号
        </td>
        <td class="listTitle2">
          当前积分
        </td>
        <td class="listTitle2">
          今日战绩
        </td>
        <td class="listTitle2">
          历史战绩
        </td>
        <td class="listTitle2">
          今日贡献
        </td>
        <td class="listTitle2">
          历史贡献
        </td>
        <td class="listTitle2">
          累计加分
        </td>
        <td class="listTitle2">
          累计减分
        </td>
        <td class="listTitle2">
          下线权重
        </td>
        <td class="listTitle2">
          在线时长
        </td>
      </tr>
      <asp:Repeater ID="rptDataList" runat="server">
        <ItemTemplate>
          <tr align="center" class="list" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
              onmouseout="this.style.backgroundColor = currentcolor">
            <td style="width: 30px;">
              <input name='cid' type='checkbox' value='<%# Eval("UserID").ToString() %>'/>
            </td>
            <td>
              <%# Eval("GameID") %>
            </td>
            <td>
              <%# Eval("NickName") %>
            </td>
            <td>
              <%# GetUserStatus(Convert.ToInt32(Eval("UserStatus")),Convert.ToInt32(Eval("UserID")))  %>
            </td>
            <td>
              <%# GetGroupIdentity(Convert.ToInt32(Eval("MemberRight")))  %>
            </td>
            <td>
              <%# Convert.ToInt32(Eval("IsAndroid"))==0?"玩家":"AI"  %>
            </td>
             <%# GetOnlineGameRoomInfo(Convert.ToInt32(Eval("GroupID")),Convert.ToInt32(Eval("UserStatus")),Convert.ToInt32(Eval("UserID"))) %>
            <td>
              <%# Eval("CurrentScore") %>
            </td>
            <td>
              <%# Eval("TodayScore") %>
            </td>
            <td>
              <%# Eval("AllScore") %>
            </td>
            <td>
              <%# Eval("TodayRevenue") %>
            </td>
            <td>
              <%# Eval("AllRevenue") %>
            </td>
            <td>
              <%# Eval("AddAllScore") %>
            </td>
            <td>
              <%# Eval("LessAllScore") %>
            </td>
            <td>
              <%# GetOnlineWeightOut(Fetch.GetTimeIntervalMinutes(Convert.ToDateTime( Eval("EnterTime")),Convert.ToDateTime( Eval("LeaveTime"))))  %>%
            </td>
            <td>
              <%# GetUserOnlineTime(Convert.ToInt32( Eval("GameID")),Convert.ToDateTime( Eval("EnterTime")),Convert.ToDateTime( Eval("LeaveTime"))) %>
            </td>
          </tr>
        </ItemTemplate>
        <AlternatingItemTemplate>
          <tr align="center" class="listBg" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
              onmouseout="this.style.backgroundColor = currentcolor">
            <td style="width: 30px;">
              <input name='cid' type='checkbox' value='<%# Eval("UserID").ToString() %>'/>
            </td>
            <td>
              <%# Eval("GameID") %>
            </td>
            <td>
              <%# Eval("NickName") %>
            </td>
            <td>
              <%# GetUserStatus(Convert.ToInt32(Eval("UserStatus")),Convert.ToInt32(Eval("UserID")))  %>
            </td>
            <td>
              <%# GetGroupIdentity(Convert.ToInt32(Eval("MemberRight")))  %>
            </td>
            <td>
              <%# Convert.ToInt32(Eval("IsAndroid"))==0?"玩家":"AI"  %>
            </td>
            <%# GetOnlineGameRoomInfo(Convert.ToInt32(Eval("GroupID")),Convert.ToInt32(Eval("UserStatus")),Convert.ToInt32(Eval("UserID"))) %>
            <td>
              <%# Eval("CurrentScore") %>
            </td>
            <td>
              <%# Eval("TodayScore") %>
            </td>
            <td>
              <%# Eval("AllScore") %>
            </td>
            <td>
              <%# Eval("TodayRevenue") %>
            </td>
            <td>
              <%# Eval("AllRevenue") %>
            </td>
            <td>
              <%# Eval("AddAllScore") %>
            </td>
            <td>
              <%# Eval("LessAllScore") %>
            </td>
            <td>
              <%# GetOnlineWeightOut(Fetch.GetTimeIntervalMinutes(Convert.ToDateTime( Eval("EnterTime")),Convert.ToDateTime( Eval("LeaveTime"))))  %>%
            </td>
            <td>
              <%# GetUserOnlineTime(Convert.ToInt32( Eval("GameID")),Convert.ToDateTime( Eval("EnterTime")),Convert.ToDateTime( Eval("LeaveTime"))) %>
            </td>
          </tr>
        </AlternatingItemTemplate>
      </asp:Repeater>
      <asp:Literal runat="server" ID="litNoData" Visible="false" Text="<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>"></asp:Literal>
    </table>
  </div>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
      <td class="listTitleBg">
        <span>选择：</span>&nbsp;<a class="l1" href="javascript:SelectAll(true);">全部</a>&nbsp;-&nbsp;<a class="l1" href="javascript:SelectAll(false);">无</a>
      </td>
      <td align="right" class="page">
        <gsp:AspNetPager ID="anpPage" OnPageChanged="anpPage_PageChanged" runat="server" AlwaysShow="true" FirstPageText="首页" LastPageText="末页"
                         PageSize="20" NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table"
                         NumericButtonCount="5" CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%" UrlPaging="false">
        </gsp:AspNetPager>
      </td>
    </tr>
  </table>
</form>
</body>
</html>
