﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RecordScoreBattleList.aspx.cs" Inherits="Game.Web.Module.Diamond.RecordScoreBattleList" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <link href="/styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="/scripts/common.js"></script>
    <script type="text/javascript" src="/scripts/comm.js"></script>
    <script type="text/javascript" src="/scripts/My97DatePicker/WdatePicker.js"></script>
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
                你当前位置：钻石系统 - 积分约战开房
            </td>
        </tr>
    </table>
    <!-- 头部菜单 End -->
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg">
        <tr>
            <td class="listTdLeft" style="width: 80px">
                日期查询：
            </td>
            <td>
                <asp:TextBox ID="txtStartDate" runat="server" CssClass="text wd2" onfocus="WdatePicker({dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"></asp:TextBox><img
                    src="../../Images/btn_calendar.gif" onclick="WdatePicker({el:'txtStartDate',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"
                    style="cursor: pointer; vertical-align: middle" />
                至
                <asp:TextBox ID="txtEndDate" runat="server" CssClass="text wd2" onfocus="WdatePicker({dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"></asp:TextBox><img
                    src="../../Images/btn_calendar.gif" onclick="WdatePicker({el:'txtEndDate',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"
                    style="cursor: pointer; vertical-align: middle" />
                <asp:Button ID="btnQuery" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery_Click" />
                <asp:Button ID="btnQueryTD" runat="server" Text="今天" CssClass="btn wd1" OnClick="btnQueryTD_Click" />
                <asp:Button ID="btnQueryYD" runat="server" Text="昨天" CssClass="btn wd1" OnClick="btnQueryYD_Click" />
                <asp:Button ID="btnQueryTW" runat="server" Text="本周" CssClass="btn wd1" OnClick="btnQueryTW_Click" />
                <asp:Button ID="btnQueryYW" runat="server" Text="上周" CssClass="btn wd1" OnClick="btnQueryYW_Click" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg  Tmg7">
        <tr>
            <td class="listTdLeft" style="width: 80px">
                用户查询：
            </td>
            <td>
                <asp:TextBox ID="txtSearch" runat="server" CssClass="text"></asp:TextBox>
                <asp:DropDownList ID="ddlSearchType" runat="server">
                    <asp:ListItem Value="0">按房间号</asp:ListItem>
                    <asp:ListItem Value="1">按游戏ID</asp:ListItem>
                    <asp:ListItem Value="2">按用户标识</asp:ListItem>
                </asp:DropDownList>
                <asp:Button ID="Button1" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery1_Click" />
            </td>
        </tr>
    </table>
    <div id="content">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box Tmg7" id="list">
            <tr align="center" class="bold">
                <td class="listTitle2">
                    创建时间
                </td>
                <td class="listTitle2">
                    房间状态
                </td>
                <td class="listTitle2">
                    房主ID
                </td>
                <td class="listTitle2">
                    房主昵称
                </td>
                <td class="listTitle2">
                    游戏房间
                </td>
                <td class="listTitle2">
                    房间号
                </td>
                <td class="listTitle2">
                    游戏底分
                </td>
                <td class="listTitle2">
                    局数限制
                </td>
                <td class="listTitle2">
                    付费模式
                </td>
                <td class="listTitle2">
                    消耗钻石
                </td>
                <td class="listTitle2">
                    解散时间
                </td>
                <td class="listTitle">
                    管理操作
                </td>
            </tr>
            <asp:Repeater ID="rptDataList" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td>
                            <%# Eval( "CreateDate" )%>
                        </td>
                        <td>
                            <%# Eval( "RoomStatus").ToString()=="2"?"已解散": Eval( "RoomStatus").ToString()=="1"?"游戏中": "等待中" %>
                        </td>
                        <%# GetAccountsInfo(Convert.ToInt32(Eval( "UserID" ))) %>
                        <td>
                            <%# GetRoomName(Convert.ToInt32(Eval( "ServerID" ))) %>
                        </td>
                        <td>
                            <%# Eval( "RoomID" )%>
                        </td>
                        <td>
                            <%# Eval( "CellScore" )%>
                        </td>
                        <td>
                            <%# Eval( "CountLimit" )%>
                        </td>
                        <td>
                            <%# Eval( "PayMode").ToString()=="1"?"AA支付":"房主支付" %>
                        </td>
                        <td>
                            <%# Eval( "NeedRoomCard" )%>
                        </td>
                        <td>
                            <%# Eval( "DissumeDate" )%>
                        </td>
                        <td>
                            <a class="l" href="javascript:void(0)" 
                                onclick="javascript:openWindowOwn('RecordScoreBattleInfo.aspx?rid=<%#Eval("RoomID") %>&uid=<%#Eval("UserID") %>&mode=<%#Eval("PlayMode") %>&time=<%# Convert.ToDateTime(Eval("CreateDate")).ToString("yyyy-MM-dd HH:mm:ss") %>','RoomScore',600,500);">查看战绩</a>
                        </td>
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="listBg" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td>
                            <%# Eval( "CreateDate" )%>
                        </td>
                        <td>
                            <%# Eval( "RoomStatus").ToString()=="2"?"已解散": Eval( "RoomStatus").ToString()=="1"?"游戏中": "等待中" %>
                        </td>
                        <%# GetAccountsInfo(Convert.ToInt32(Eval( "UserID" ))) %>
                        <td>
                            <%# GetRoomName(Convert.ToInt32(Eval( "ServerID" ))) %>
                        </td>
                        <td>
                            <%# Eval( "RoomID" )%>
                        </td>
                        <td>
                            <%# Eval( "CellScore" )%>
                        </td>
                        <td>
                            <%# Eval( "CountLimit" )%>
                        </td>
                        <td>
                            <%# Eval( "PayMode").ToString()=="1"?"AA支付":"房主支付" %>
                        </td>
                        <td>
                            <%# Eval( "NeedRoomCard" )%>
                        </td>
                        <td>
                            <%# Eval( "DissumeDate" )%>
                        </td>
                        <td>
                            <a class="l" href="javascript:void(0)" 
                                onclick="javascript:openWindowOwn('RecordScoreBattleInfo.aspx?rid=<%#Eval("RoomID") %>&uid=<%#Eval("UserID") %>&mode=<%#Eval("PlayMode") %>&time=<%# Convert.ToDateTime(Eval("CreateDate")).ToString("yyyy-MM-dd HH:mm:ss") %>','RoomScore',600,500);">查看战绩</a>
                        </td>
                    </tr>
                </AlternatingItemTemplate>
            </asp:Repeater>
            <asp:Literal runat="server" ID="litNoData" Visible="false" Text="<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>"></asp:Literal>
        </table>
    </div>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td align="right" class="page">
                <gsp:AspNetPager ID="anpNews" OnPageChanged="anpPage_PageChanged" runat="server" AlwaysShow="true" FirstPageText="首页" LastPageText="末页"
                    PageSize="20" NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table"
                    NumericButtonCount="5" CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%" UrlPaging="false">
                </gsp:AspNetPager>
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
