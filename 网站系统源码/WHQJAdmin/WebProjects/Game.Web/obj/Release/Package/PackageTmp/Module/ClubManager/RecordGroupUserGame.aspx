<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RecordGroupUserGame.aspx.cs" Inherits="Game.Web.Module.ClubManager.RecordGroupUserGame" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>

    <script type="text/javascript" src="../../scripts/My97DatePicker/WdatePicker.js"></script>

    <title>游戏记录</title>
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
                <td width="1232" height="25" valign="top" align="left">你当前位置：用户系统 - 战绩记录
                </td>
            </tr>
        </table>
        <!-- 头部菜单 End -->
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg">
            <tr>
                <td class="listTdLeft" style="width: 80px">信息查询：
                </td>
                <td>
                    <asp:TextBox ID="txtSearch" runat="server" CssClass="text"></asp:TextBox>
                    <asp:DropDownList ID="ddlSearchType" runat="server">
                        <asp:ListItem Text="游戏ID" Value="1"></asp:ListItem>
                        <asp:ListItem Text="用户标识" Value="2"></asp:ListItem>
                        <asp:ListItem Text="大联盟ID" Value="3"></asp:ListItem>
                    </asp:DropDownList>
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
        <div id="content">
            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box Tmg7" id="list">
                <tr align="center" class="bold">
                    <td class="listTitle2">大联盟ID
                    </td>
                    <td class="listTitle2">大联盟名称
                    </td>
                    <td class="listTitle2">游戏名称
                    </td>
                    <td class="listTitle">房间ID
                    </td>
                    <td class="listTitle">局数
                    </td>
                    <td class="listTitle">大赢家ID
                    </td>
                    <td class="listTitle">大赢家昵称
                    </td>
                    <td class="listTitle2">服务费
                    </td>
                    <td class="listTitle2">开始时间
                    </td>
                    <td class="listTitle2">结束时间
                    </td>
                </tr>
                <asp:Repeater ID="rptDataList" runat="server" OnItemDataBound="rptDataList_ItemDataBound">
                    <ItemTemplate>
                        <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                            onmouseout="this.style.backgroundColor=currentcolor">
                            <td>
                                <%# Eval( "GroupID")%>
                             &nbsp;<img id="<%# Eval( "RoomID" ).ToString( )%>" src="../../Images/down.gif" style="cursor: pointer" alt="查看每局记录" title="查看每局记录" onclick="showInfo(this)" />
                            </td>
                            <td>
                                <%# GetGroupName(Convert.ToInt32(Eval( "GroupID" )))  %>
                            </td>
                            <td>
                                <%# Eval( "KindName" )%>
                            </td>
                            <td>
                                <%# Eval( "RoomID" )%>
                            </td>
                            <td>
                                <%# Eval( "DrawCount" )%>
                            </td>
                            <td>
                                <%# Eval( "BigWinnerID" )%>
                            </td>
                            <td>
                                <%# Eval( "BigWinnerName" )%>
                            </td>
                            <td>
                                <%# Eval( "RoomRevenue" )%>
                            </td>
                            <td>
                                <%# Eval( "StartTime", "{0:yyyy-MM-dd HH:mm:ss}" )%>
                            </td>
                            <td>
                                <%# Eval( "EndTime", "{0:yyyy-MM-dd HH:mm:ss}" )%>
                            </td>
                        </tr>
                        <tr style="display: none" id="record<%# Eval( "RoomID" ).ToString( )%>">
                            <td colspan="100">
                                <table width="100%" border="0" align="center">
                                    <tr class="list" align="center">
                                        <td style="font-weight: bold;">局数标识
                                        </td>
                                        <td style="font-weight: bold;">游戏
                                        </td>
                                        <td style="font-weight: bold;">房间
                                        </td>
                                        <td style="font-weight: bold;">大联盟ID
                                        </td>
                                        <td style="font-weight: bold;">大联盟名称
                                        </td>
                                        <td style="font-weight: bold;">开始时间
                                        </td>
                                        <td style="font-weight: bold;">结束时间
                                        </td>
                                    </tr>
                                    <asp:Repeater ID="rptSubData" runat="server" OnItemDataBound="rptSubData_ItemDataBound">
                                        <ItemTemplate>
                                            <tr class="list" align="center">
                                                <td>
                                                    <%# ((System.Data.DataRow)Container.DataItem)["TurnIndex"].ToString( )%>
                                                &nbsp;<img id="<%# ((System.Data.DataRow)Container.DataItem)["DrawID"].ToString( )%>" src="../../Images/down.gif" style="cursor: pointer" alt="查看对局玩家" title="查看对局玩家" onclick="showScore(this)" />
                                                </td>
                                                <%# GetGameAndRoomName(Convert.ToInt32(((System.Data.DataRow)Container.DataItem)["KindID"].ToString( )),Convert.ToInt32(((System.Data.DataRow)Container.DataItem)["ServerID"].ToString( ))) %>
                                                <td>
                                                    <%# ((System.Data.DataRow)Container.DataItem)["GroupID"].ToString( )%>
                                                </td>
                                                <td>
                                                    <%# GetGroupName(Convert.ToInt32(((System.Data.DataRow)Container.DataItem)["GroupID"]))%>
                                                </td>
                                                <td>
                                                    <%# ((System.Data.DataRow)Container.DataItem)["StartTime"].ToString( )%>
                                                </td>
                                                <td>
                                                    <%# ((System.Data.DataRow)Container.DataItem)["ConcludeTime"].ToString( )%>
                                                </td>
                                            </tr>
                                            <tr style="display: none" id="record2<%# ((System.Data.DataRow)Container.DataItem)["DrawID"].ToString( )%>">
                                                <td colspan="100">
                                                    <table width="100%" border="0" align="center">
                                                        <tr class="list" align="center">
                                                            <td style="font-weight: bold;">桌子标识
                                                            </td>
                                                            <td style="font-weight: bold;">游戏ID
                                                            </td>
                                                            <td style="font-weight: bold;">昵称
                                                            </td>
                                                            <td style="font-weight: bold;">椅子编号
                                                            </td>
                                                            <td style="font-weight: bold;">输赢积分
                                                            </td>
                                                            <td style="font-weight: bold;">服务费
                                                            </td>
                                                            <td style="font-weight: bold;">插入时间
                                                            </td>
                                                        </tr>
                                                        <asp:Repeater ID="rptSubtwoData" runat="server">
                                                            <ItemTemplate>
                                                                <tr class="list" align="center">
                                                                    <td>
                                                                        <%# ((System.Data.DataRow)Container.DataItem)["DrawID"].ToString( )%>
                                                                    </td>
                                                                    <%# GetUserInfo(Convert.ToInt32(((System.Data.DataRow)Container.DataItem)["UserID"].ToString( ))) %>
                                                                    <td>
                                                                        <%# ((System.Data.DataRow)Container.DataItem)["ChairID"].ToString( )%>
                                                                    </td>
                                                                    <td>
                                                                        <%# ((System.Data.DataRow)Container.DataItem)["Score"].ToString( )%>
                                                                    </td>
                                                                    <td>
                                                                        <%# ((System.Data.DataRow)Container.DataItem)["Revenue"].ToString( )%>
                                                                    </td>
                                                                    <td>
                                                                        <%# ((System.Data.DataRow)Container.DataItem)["InsertTime"].ToString( )%>
                                                                    </td>
                                                                </tr>
                                                            </ItemTemplate>
                                                        </asp:Repeater>
                                                    </table>
                                                </td>
                                            </tr>
                                        </ItemTemplate>
                                    </asp:Repeater>
                                </table>
                            </td>
                        </tr>
                    </ItemTemplate>
                    <%--<AlternatingItemTemplate>
                    <tr align="center" class="listBg" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td >
                            <%# Eval( "ConcludeTime", "{0:yyyy-MM-dd HH:mm:ss}" )%>
                             &nbsp;<img id="<%# Eval( "DrawID" ).ToString( )%>" src="../../Images/down.gif" style="cursor: pointer" alt="查看同桌玩家" title="查看同桌玩家" onclick="showInfo(this)" />
                        </td>
                        <%# GetGameAndRoomName(Convert.ToInt32(Eval( "KindID" )),Convert.ToInt32(Eval( "ServerID" ))) %>
                        <td>
                            <%# Eval( "GroupID" ).ToString( )%>
                        </td>
                        <td>
                            <%# GetGroupName(Convert.ToInt32(Eval( "GroupID" )))  %>
                        </td>
                        <td>
                            <%# Eval( "TurnIndex" ).ToString( )%>
                        </td>
                        <td>
                            <%# Eval( "ConcludeTime", "{0:yyyy-MM-dd HH:mm:ss}" )%>
                        </td>
                    </tr>
                    <tr style="display: none" id="record<%# Eval( "DrawID" ).ToString( )%>">
                        <td colspan="100">
                            <table width="100%" border="0" align="center">
                                <tr class="list" align="center">
                                    <td style="font-weight: bold;">
                                        昵称
                                    </td> 
                                    <td style="font-weight: bold;">
                                        是否机器人
                                    </td>
                                    <td style="font-weight: bold;">
                                        椅子编号
                                    </td>
                                    <td style="font-weight: bold;">
                                        输赢积分
                                    </td>
                                    <td style="font-weight: bold;">
                                        服务费/服务费
                                    </td>
                                    <td style="font-weight: bold;">
                                        插入时间
                                    </td>
                                </tr>
                                <asp:Repeater ID="rptSubData" runat="server">
                                    <ItemTemplate>
                                        <tr class="list" align="center">
                                            <%# GetUserInfo(Convert.ToInt32(((System.Data.DataRow)Container.DataItem)["UserID"]))  %>
                                            <td>
                                                <%# ((System.Data.DataRow)Container.DataItem)["ChairID"].ToString( )%>
                                            </td>
                                            <td>
                                                <%# ((System.Data.DataRow)Container.DataItem)["Score"].ToString( )%>
                                            </td>
                                            <td>
                                                <%# ((System.Data.DataRow)Container.DataItem)["Revenue"].ToString( )%>
                                            </td>                                          
                                            <td>
                                                <%# ((System.Data.DataRow)Container.DataItem)["InsertTime"].ToString( )%>
                                            </td>               
                                        </tr>
                                    </ItemTemplate>
                                </asp:Repeater>
                            </table>
                        </td>
                    </tr>
                </AlternatingItemTemplate>--%>
                </asp:Repeater>
                <asp:Literal runat="server" ID="litNoData" Visible="false" Text="<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>"></asp:Literal>
                <asp:Literal runat="server" ID="litTip" Text="<tr class='tdbg'><td colspan='100' align='center'><br>默认不显示数据，请选择查询条件搜索!<br><br></td></tr>"></asp:Literal>
            </table>
        </div>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
                <td align="right" class="page">
                    <gsp:AspNetPager ID="anpPage" OnPageChanged="anpPage_PageChanged" runat="server" AlwaysShow="true" FirstPageText="首页" LastPageText="末页" PageSize="20" NextPageText="下页"
                        PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table" NumericButtonCount="5" CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%"
                        UrlPaging="false">
                    </gsp:AspNetPager>
                </td>
            </tr>
        </table>
    </form>
</body>
</html>

<script type="text/javascript">
    function showInfo(obj) {
        var ID = "record" + obj.id;
        if (document.getElementById(ID).style.display == "none") {
            document.getElementById(ID).style.display = "";
            obj.src = "../../Images/up.gif"
        }
        else {
            document.getElementById(ID).style.display = "none";
            obj.src = "../../Images/down.gif"
        }
    }

    function showScore(obj) {
        var ID = "record2" + obj.id;
        if (document.getElementById(ID).style.display == "none") {
            document.getElementById(ID).style.display = "";
            obj.src = "../../Images/up.gif"
        }
        else {
            document.getElementById(ID).style.display = "none";
            obj.src = "../../Images/down.gif"
        }
    }
</script>
