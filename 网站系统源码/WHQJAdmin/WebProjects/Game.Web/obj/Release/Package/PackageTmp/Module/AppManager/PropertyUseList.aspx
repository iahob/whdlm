﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PropertyUseList.aspx.cs" Inherits="Game.Web.Module.AppManager.PropertyUseList" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>
    <script type="text/javascript" src="../../scripts/My97DatePicker/WdatePicker.js"></script>

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
                你当前位置：系统维护 - 道具管理
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
        <tr>
            <td height="28">
                <ul>
                    <li class="tab2" onclick="Redirect('PropertyConfigList.aspx')">道具配置</li>
                    <li class="tab2" onclick="Redirect('PropertyBuyList.aspx')">购买记录</li>
                    <li class="tab1">使用记录</li>
                </ul>
            </td>
        </tr>
    </table>
    <div id="content">
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
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
          <td height="39" class="titleOpBg">
          </td>
        </tr>
      </table>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
            <tr align="center" class="bold">
              <td class="listTitle2">
                使用时间
              </td>
              <td class="listTitle2">
                游戏ID
              </td>
              <td class="listTitle2">
                玩家昵称
              </td>
              <td class="listTitle2">
                道具名称
              </td>
              <td class="listTitle2">
                使用数量
              </td>
              <td class="listTitle2">
                使用增加游戏币
              </td>
              <td class="listTitle2">
                使用增加魅力值
              </td>
              <td class="listTitle2">
                接收增加魅力值
              </td>
              <td class="listTitle2">
                使用地址
              </td>
            </tr>
            <asp:Repeater ID="rptDataList" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                      <td>
                        <%# Eval( "UseDate" ).ToString( )%>
                      </td>
                      <%# GetUserInfo(Convert.ToInt32(Eval( "SourceUserID" ))) %>
                      <td>
                        <%# Eval( "PropertyName" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "PropertyCount" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "UseResultsGold" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "LovelinessSend" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "LovelinessRcv" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "ClientIP" ).ToString( )%>
                      </td>
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                      <td>
                        <%# Eval( "UseDate" ).ToString( )%>
                      </td>
                      <%# GetUserInfo(Convert.ToInt32(Eval( "SourceUserID" ))) %>
                      <td>
                        <%# Eval( "PropertyName" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "PropertyCount" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "UseResultsGold" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "LovelinessSend" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "LovelinessRcv" ).ToString( )%>
                      </td>
                      <td>
                        <%# Eval( "ClientIP" ).ToString( )%>
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
