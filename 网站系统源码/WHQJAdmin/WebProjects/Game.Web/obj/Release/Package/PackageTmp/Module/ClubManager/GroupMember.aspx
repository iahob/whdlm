<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GroupMember.aspx.cs" Inherits="Game.Web.Module.ClubManager.GroupMember" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="../../scripts/common.js"></script>
    <title>亲友列表</title>
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
                你当前位置：大联盟系统 - 亲友列表
            </td>
        </tr>
    </table>
    <!-- 头部菜单 End -->
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
            </td>
        </tr>
    </table>
    <div id="content">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
            <tr align="center" class="bold">
                <td class="listTitle2">
                    游戏ID
                </td>
                <td class="listTitle2">
                    游戏昵称
                </td>
                <td class="listTitle2">
                    勋章数
                </td>
                <td class="listTitle2">
                    约战次数
                </td>
                <td class="listTitle2">
                    加入时间
                </td>
            </tr>
            <asp:Repeater ID="rptDataList" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <%# GetUserInfo(Convert.ToInt32(Eval( "UserID" ))) %>
                        <td>
                            <%# Eval( "Score" )%>
                        </td>
                        <td>
                            <%# Eval( "BattleCount" )%>
                        </td>
                         <td>
                            <%# Eval( "JoinDateTime" )%>
                        </td>   
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="listBg" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <%# GetUserInfo(Convert.ToInt32(Eval( "UserID" ))) %>
                        <td>
                            <%# Eval( "Score" )%>
                        </td>
                        <td>
                            <%# Eval( "BattleCount" )%>
                        </td>
                         <td>
                            <%# Eval( "JoinDateTime" )%>
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
