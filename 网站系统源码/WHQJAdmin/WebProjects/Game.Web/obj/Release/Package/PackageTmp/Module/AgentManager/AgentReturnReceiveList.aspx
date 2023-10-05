﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AgentReturnReceiveList.aspx.cs" Inherits="Game.Web.Module.AgentManager.AgentReturnReceiveList" %>
<%@ Import Namespace="Game.Utils" %>
<%@ Import Namespace="Game.Facade" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>
    <script type="text/javascript" src="/scripts/My97DatePicker/WdatePicker.js"></script>
    <title></title>
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
                你当前位置：代理系统 - 领取记录
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
        <tr>
            <td height="28">
                <ul>
                    <li class="tab2" onclick="Redirect('AgentReturnConfigList.aspx')">返利配置</li>
                    <li class="tab2" onclick="Redirect('AgentReturnRecordList.aspx')">返利记录</li>
                    <li class="tab1">领取记录</li>
                    <li class="tab2" onclick="Redirect('AgentReturnGrantList.aspx')">转赠记录</li>
                </ul>
            </td>
        </tr>
    </table>
      <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg">
        <tr>
          <td class="listTdLeft" style="width: 80px">
            游戏ID：
          </td>
          <td>
            <asp:TextBox ID="txtSearch" runat="server" CssClass="text"></asp:TextBox> <asp:DropDownList runat="server" ID="ddlAwardType"/>
              日期查询：
              <asp:TextBox ID="txtStartDate" runat="server" CssClass="text wd2" onfocus="WdatePicker({dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"></asp:TextBox><img
                    src="../../Images/btn_calendar.gif" onclick="WdatePicker({el:'txtStartDate',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"
                    style="cursor: pointer; vertical-align: middle" />
                至
                <asp:TextBox ID="txtEndDate" runat="server" CssClass="text wd2" onfocus="WdatePicker({dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"></asp:TextBox><img
                    src="../../Images/btn_calendar.gif" onclick="WdatePicker({el:'txtEndDate',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"
                    style="cursor: pointer; vertical-align: middle" />
            <asp:Button ID="btnQuery" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery_Click" />
          </td>
        </tr>
      </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
        <tr>
            <td height="39" class="titleOpBg">
             </td>
        </tr>
    </table>
    <div id="content">
      <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
        <tr align="center" class="bold">
          <td class="listTitle2">
            记录时间
          </td>
          <td class="listTitle2">
            代理
          </td>
          <td class="listTitle2">
            返利类型
          </td>
          <td class="listTitle2">
            领取前数值
          </td>
          <td class="listTitle2">
            领取数值
          </td>
        </tr>
        <asp:Repeater ID="rptDataList" runat="server">
          <ItemTemplate>
            <tr align="center" class="list" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
                onmouseout="this.style.backgroundColor = currentcolor">
              <td>
                <%# Convert.ToDateTime(Eval("CollectDate").ToString()).ToString("yyyy-MM-dd HH:mm:ss") %>
              </td>
              <td>
                <a href="javascript:;" class="l" onclick="openWindowOwn('/Module/AccountManager/AccountsBaseInfo.aspx?param=<%# Eval("UserID") %>')"><%# GetGameName(Convert.ToInt32(Eval("UserID"))) %></a>
              </td>
              <td>
                <%# EnumDescription.GetFieldText((AppConfig.AwardType)Convert.ToByte(Eval("AwardType").ToString())) %>
              </td>
              <td>
                <%# Eval("ReceiveBefore") %>
              </td>
              <td>
                <%# Eval("ReceiveAward") %>
              </td>
            </tr>
          </ItemTemplate>
          <AlternatingItemTemplate>
            <tr align="center" class="list" onmouseover="currentcolor = this.style.backgroundColor;this.style.backgroundColor = '#caebfc';this.style.cursor = 'default';"
                onmouseout="this.style.backgroundColor = currentcolor">
              <td>
                <%# Convert.ToDateTime(Eval("CollectDate").ToString()).ToString("yyyy-MM-dd HH:mm:ss") %>
              </td>
              <td>
                <a href="javascript:;" class="l" onclick="openWindowOwn('/Module/AccountManager/AccountsBaseInfo.aspx?param=<%# Eval("UserID") %>')"><%# GetGameName(Convert.ToInt32(Eval("UserID"))) %></a>
              </td>
              <td>
                <%# EnumDescription.GetFieldText((AppConfig.AwardType)Convert.ToByte(Eval("AwardType").ToString())) %>
              </td>
              <td>
                <%# Eval("ReceiveBefore") %>
              </td>
              <td>
                <%# Eval("ReceiveAward") %>
              </td>
            </tr>
          </AlternatingItemTemplate>
        </asp:Repeater>
        <asp:Literal runat="server" ID="litNoData" Visible="false" Text="<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>"></asp:Literal>
      </table>
    </div>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="listTitleBg"><span>选择：</span>&nbsp;<a class="l1" href="javascript:SelectAll(true);">全部</a>&nbsp;-&nbsp;<a class="l1" href="javascript:SelectAll(false);">无</a></td>                
            <td align="right" class="page">                
                <gsp:AspNetPager ID="anpNews" runat="server" onpagechanged="anpNews_PageChanged" AlwaysShow="true" FirstPageText="首页" LastPageText="末页" PageSize="20" 
                    NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table" NumericButtonCount="5"
                    CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%">
                </gsp:AspNetPager>
            </td>
        </tr>
    </table> 
    </form>
</body>
</html>
