<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RankingRecordList.aspx.cs" Inherits="Game.Web.Module.AppManager.RankingRecordList" %>

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
                你当前位置：系统维护 - 奖励记录
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
        <tr>
            <td height="28">
                <ul>
                    <li class="tab2" onclick="Redirect('RankingConfigList.aspx')">排行配置</li>
                    <li class="tab1">奖励记录</li>
                </ul>
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
                <asp:DropDownList ID="ddlRankType" runat="server">
                   <%-- <asp:ListItem Value="1">财富排行榜</asp:ListItem>--%>
                    <asp:ListItem Value="2">胜局排行榜</asp:ListItem>
                </asp:DropDownList>
                发奖时间：
                <asp:TextBox ID="txtStartDate" runat="server" CssClass="text wd2" onfocus="WdatePicker({dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"></asp:TextBox><img
                                                                                                                                                                                       src="../../Images/btn_calendar.gif" onclick="WdatePicker({ el: 'txtStartDate', dateFmt: 'yyyy-MM-dd', maxDate: '#F{$dp.$D(\'txtEndDate\')}' })"
                                                                                                                                                                                       style="cursor: pointer; vertical-align: middle"/>
                至
                <asp:TextBox ID="txtEndDate" runat="server" CssClass="text wd2" onfocus="WdatePicker({dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"></asp:TextBox><img
                                                                                                                                                                                       src="../../Images/btn_calendar.gif" onclick="WdatePicker({ el: 'txtEndDate', dateFmt: 'yyyy-MM-dd', minDate: '#F{$dp.$D(\'txtStartDate\')}' })"
                                                                                                                                                                                       style="cursor: pointer; vertical-align: middle"/>
                <asp:Button ID="Button1" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery1_Click" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <td height="39" class="titleOpBg">         
            </td>
    </table>
    <div id="content">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
            <tr align="center" class="bold">
                <td class="listTitle2">
                    发奖时间
                </td>
                <td class="listTitle2">
                    游戏ID
                </td>                
                <td class="listTitle2">
                    游戏昵称
                </td>                
                <td class="listTitle2">
                    排行榜类型
                </td>  
                <td class="listTitle2">
                    排行榜排名
                </td>   
                <td class="listTitle2">
                    累计金币/累计胜局
                </td>  
                <td class="listTitle2">
                    奖励钻石数
                </td>    
                <td class="listTitle2">
                    奖励金币数
                </td> 
                <td class="listTitle2">
                    奖励奖券数
                </td>        
            </tr>
            <asp:Repeater ID="rptDataList" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td>
                            <%# Eval( "CollectDate" )%>
                        </td>   
                        <td>
                            <%# Eval( "GameID" )%>
                        </td> 
                        <td>
                            <%# Eval( "NickName" )%>
                        </td> 
                        <td>
                            <%# Eval( "TypeID" ).ToString()=="1"?"财富排行榜":"胜局排行榜" %>
                        </td> 
                        <td>
                            <%# Eval( "RankNum" )%>
                        </td> 
                        <td>
                            <%# Eval( "RankValue" )%>
                        </td> 
                        <td>
                            <%# Eval( "Diamond" )%>
                        </td> 
                        <td>
                            <%# Eval( "Gold" )%>
                        </td> 
                         <td>
                            <%# Eval( "AwardTicket" )%>
                        </td> 
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td>
                            <%# Eval( "CollectDate" )%>
                        </td>   
                        <td>
                            <%# Eval( "GameID" )%>
                        </td> 
                        <td>
                            <%# Eval( "NickName" )%>
                        </td> 
                        <td>
                            <%# Eval( "TypeID" ).ToString()=="1"?"财富排行榜":"胜局排行榜" %>
                        </td> 
                        <td>
                            <%# Eval( "RankNum" )%>
                        </td> 
                        <td>
                            <%# Eval( "RankValue" )%>
                        </td> 
                        <td>
                            <%# Eval( "Diamond" )%>
                        </td> 
                        <td>
                            <%# Eval( "Gold" )%>
                        </td> 
                          <td>
                            <%# Eval( "AwardTicket" )%>
                        </td>                                 
                    </tr>
                </AlternatingItemTemplate>
            </asp:Repeater>
            <asp:Literal runat="server" ID="litNoData" Visible="false" Text="<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>"></asp:Literal>
        </table>
    </div>
    </form>
</body>
</html>
