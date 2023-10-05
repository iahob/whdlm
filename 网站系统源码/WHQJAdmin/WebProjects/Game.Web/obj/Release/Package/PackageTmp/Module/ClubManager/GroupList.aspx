<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GroupList.aspx.cs" Inherits="Game.Web.Module.ClubManager.GroupList" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
  <title></title>
  <link href="../../styles/layout.css" rel="stylesheet" type="text/css"/>
  <script src="../../scripts/jquery.js"></script>
  <script type="text/javascript" src="../../scripts/common.js"></script>

  <script type="text/javascript" src="../../scripts/comm.js">
  </script>
  <script type="text/javascript">
      $(function() {
          $("#btnMdifiy").on("click",
              function () {
                  if (deleteop()) {
                      var ids = "";
                      $("input:checkbox[name=cid]:checked").each(function (i) {
                          if (0 == i) {
                              ids = $(this).val();
                          } else {
                              ids += ("," + $(this).val());
                          }
                      });
                      openWindowOwn('ModifyAwardPoint.aspx?param=' + ids + '', 'ModifyAwardPoint', 600, 200);
                  }
              });

          $("#btnMdifiyMaxMemeberCount").on("click",
            function () {
              if (deleteop()) {
                var ids = "";
                $("input:checkbox[name=cid]:checked").each(function (i) {
                  if (0 == i) {
                    ids = $(this).val();
                  } else {
                    ids += ("," + $(this).val());
                  }
                });
                openWindowOwn('ModifyMaxMemeberCount.aspx?param=' + ids + '', 'ModifyAwardPoint', 600, 200);
              }
            });
      });
  </script>
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
        你当前位置：大联盟系统 - 大联盟管理
      </td>
    </tr>
  </table>
  <!-- 头部菜单 End -->
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
          <input type="button" value="赠送勋章" class="btn" style="width:85px;" onclick="openWindowOwn('AddMedal.aspx', '', 600, 200);" />
      </td>
    </tr>
  </table>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
    <tr>
      <td height="39" class="titleOpBg">
        <asp:Button ID="btnDongjie" runat="server" Text="冻结" CssClass="btn wd1" OnClick="btnDongjie_Click" OnClientClick="return deleteop()"/>
        <asp:Button ID="btnJiedong" runat="server" Text="解冻" CssClass="btn wd1" OnClick="btnJiedong_Click" OnClientClick="return deleteop()"/>
        <asp:Button ID="btnOpenMedal" runat="server" Text="开启勋章模式" CssClass="btn wd3" OnClick="btnOpenMedal_Click" OnClientClick="return deleteop()"/>
         <asp:Button ID="btnCloseMedal" runat="server" Text="关闭勋章模式" CssClass="btn wd3" OnClick="btnCloseMedal_Click" OnClientClick="return deleteop()"/>
        <input type="button" id="btnMdifiy" class="btn wd3" value="批量修改点位" name="批量修改点位" />
        <input type="button" id="btnMdifiyMaxMemeberCount" class="btn wd3" value="批量修改最大人数" name="批量修改最大人数" />
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
          大联盟ID
        </td>
        <td class="listTitle2">
          大联盟名称
        </td>
        <td class="listTitle2">
          盟主游戏ID
        </td>
        <td class="listTitle2">
          盟主游戏昵称
        </td>
        <td class="listTitle2">
          大联盟状态
        </td>
        <td class="listTitle2">
           勋章模式
        </td>
        <td class="listTitle2">
          大联盟人数/上限
        </td>
        <!-- <td class="listTitle2">
          大联盟钻石
        </td>
        <td class="listTitle2">
          大联盟消耗钻石
        </td>
        <td class="listTitle2">
          大联盟开房次数
        </td> -->
        <td class="listTitle2">
          大联盟勋章
        </td>
        <td class="listTitle2">
          盟主点位
        </td>
        <td class="listTitle2">
          大联盟创建时间
        </td>
        <td class="listTitle2">
          管理操作
        </td>
      </tr>
      <asp:Repeater ID="rptDataList" runat="server">
        <ItemTemplate>
          <tr align="center" class="list" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
              onmouseout="this.style.backgroundColor = currentcolor">
            <td style="width: 30px;">
              <input name='cid' type='checkbox' value='<%# Eval("GroupID").ToString() %>'/>
            </td>
            <td>
              <%# Eval("GroupID") %>
            </td>
            <td>
              <%# Eval("GroupName") %>
            </td>
            <td>
              <%# Eval("CreaterGameID") %>
            </td>
            <td>
              <%# Eval("CreaterNickName") %>
            </td>
              <td>
              <%# Eval("GroupStatus").ToString()=="1"?"可用":"<span class=\"hong\">不可用</span>" %>
            </td>
              <td>
                  <%# Eval("CanUseMedal").ToString()=="1"?"开启":"<span class=\"hong\">关闭</span>" %>
              </td>
            <td>
              <%# Eval("MemberCount") %>/<%# Eval("MaxMemberCount") %>
            </td>
            <%# GetGroupWealth(Convert.ToInt32(Eval("GroupID"))) %>
            <%# GetGroupMasterMember(Convert.ToInt32(Eval("GroupID"))) %>
            <td>
              <%# Eval("CreateDateTime") %>
            </td>
            <td>
                <a href="javascript:;" class="l" onclick="openWindowOwn('GroupMember.aspx?gid=<%# Eval("GroupID") %>', '', 700, 750);">查看成员</a> 
                <a class="l" href="javascript:void(0)" onclick="javascript:openWindowOwn('ModifyAwardPoint.aspx?param=<%#Eval("GroupID").ToString() %>','ModifyAwardPoint',600,200);">修改点位</a>
              <a class="l" href="javascript:void(0)" onclick="javascript:openWindowOwn('ModifyMaxMemeberCount.aspx?param=<%#Eval("GroupID").ToString() %>','ModifyMaxMemeberCount',600,200);">修改最大人数</a>
              <a class="l" href="javascript:void(0)" onclick="javascript:openWindowOwn('AddMedal.aspx?param=<%#Eval("GroupID").ToString() %>','GrantGold',600,200);">赠送勋章</a>
              <!-- <a href="javascript:;" class="l" onclick="openWindowOwn('RecordDiamondList.aspx?gid=<%# Eval("GroupID") %>', '', 800, 700);">钻石记录</a>  -->
            </td>
          </tr>
        </ItemTemplate>
        <AlternatingItemTemplate>
          <tr align="center" class="listBg" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
              onmouseout="this.style.backgroundColor = currentcolor">
            <td style="width: 30px;">
              <input name='cid' type='checkbox' value='<%# Eval("GroupID").ToString() %>'/>
            </td>
            <td>
              <%# Eval("GroupID") %>
            </td>
            <td>
              <%# Eval("GroupName") %>
            </td>
            <td>
              <%# Eval("CreaterGameID") %>
            </td>
            <td>
              <%# Eval("CreaterNickName") %>
            </td>
              <td>
              <%# Eval("GroupStatus").ToString()=="1"?"可用":"<span class=\"hong\">不可用</span>" %>
            </td>
              <td>
                  <%# Eval("CanUseMedal").ToString()=="1"?"开启":"<span class=\"hong\">关闭</span>" %>
              </td>
            <td>
              <%# Eval("MemberCount") %>/<%# Eval("MaxMemberCount") %>
            </td>
              <%# GetGroupWealth(Convert.ToInt32(Eval("GroupID"))) %>
              <%# GetGroupMasterMember(Convert.ToInt32(Eval("GroupID"))) %>
            <td>
              <%# Eval("CreateDateTime") %>
            </td>
            <td>
               
              <a href="javascript:;" class="l" onclick="openWindowOwn('GroupMember.aspx?gid=<%# Eval("GroupID") %>', '', 700, 750);">查看成员</a>
                <a class="l" href="javascript:void(0)" onclick="javascript:openWindowOwn('ModifyAwardPoint.aspx?param=<%#Eval("GroupID").ToString() %>','ModifyAwardPoint',600,200);">修改点位</a>
              <a class="l" href="javascript:void(0)" onclick="javascript:openWindowOwn('ModifyMaxMemeberCount.aspx?param=<%#Eval("GroupID").ToString() %>','ModifyMaxMemeberCount',600,200);">修改最大人数</a>
                <a class="l" href="javascript:void(0)" onclick="javascript:openWindowOwn('AddMedal.aspx?param=<%#Eval("GroupID").ToString() %>','GrantGold',600,200);">赠送勋章</a>
              <!-- <a href="javascript:;" class="l" onclick="openWindowOwn('RecordDiamondList.aspx?gid=<%# Eval("GroupID") %>', '', 800, 700);">钻石记录</a>  -->
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
