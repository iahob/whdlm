<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="UserGame.aspx.cs" Inherits="Game.Web.Module.AccountManager.UserGame" %>

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
                你当前位置：用户系统 - 用户游戏
            </td>
        </tr>
    </table>
    <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
        <tr>
            <td height="28">
                <ul>
                    <li class="tab2" onclick="Redirect('GeneralGame.aspx')">通用游戏</li>
                    <li class="tab1">玩家游戏</li>
                    <li class="tab2" onclick="Redirect('AgentGame.aspx')">代理游戏</li>
                </ul>
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg" style="margin-bottom:3px;">
        <tr>
            <td class="listTdLeft" style="width: 80px">
                用户查询：
            </td>
            <td>
                <asp:TextBox ID="txtSearch" runat="server" CssClass="text"></asp:TextBox>
                <asp:DropDownList ID="ddlSearchType" runat="server">
                    <asp:ListItem Value="1">按游戏ID</asp:ListItem>
                </asp:DropDownList>
                <asp:Button ID="Button1" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery1_Click" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td height="39" class="titleOpBg">
                <input type="button" class="btn wd2" value="添加游戏" onclick="openWindowOwn('AddGame.aspx?typeid=1','添加游戏',600,280)">
                <asp:Button ID="btnDelete" runat="server" Text="删除游戏" CssClass="btn wd2" OnClick="btnDelete_Click" OnClientClick="return deleteop()" />
                <asp:Button ID="btnRefush" runat="server" Text="刷新" CssClass="btn wd2" OnClick="btnRefush_Click" />
            </td>
        </tr>
    </table>
    <div id="content">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box Tmg7" id="list">
            <tr align="center" class="bold">
                <td class="listTitle">
                    <input type="checkbox" name="chkAll" onclick="SelectAll(this.checked);" />
                </td>
                <td class="listTitle2">
                    游戏标识
                </td>
                <td class="listTitle2">
                    游戏名称
                </td>
                <td class="listTitle2">
                    游戏排序
                </td>
                <td class="listTitle2">
                    配置时间
                </td>
            </tr>
            <asp:Repeater ID="rptDataList" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td style="width: 30px;">
                            <input name='cid' type='checkbox' value='<%# Eval("ID").ToString()%>' />
                        </td>
                        <td>
                            <%# Eval( "KindID" ).ToString( )%>
                        </td>
                        <td>
                            <%# GetKindName(Convert.ToInt32(Eval( "KindID" )))%>
                        </td>
                        <td>
                            <%# Eval( "SortID" ).ToString( )%>
                        </td>
                        <td>
                            <%# Eval( "CollectDate" ).ToString( )%>
                        </td>
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="listBg" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td style="width: 30px;">
                            <input name='cid' type='checkbox' value='<%# Eval("ID").ToString()%>' />
                        </td>
                        <td>
                            <%# Eval( "KindID" ).ToString( )%>
                        </td>
                        <td>
                            <%# GetKindName(Convert.ToInt32(Eval( "KindID" )))%>
                        </td>
                        <td>
                            <%# Eval( "SortID" ).ToString( )%>
                        </td>
                        <td>
                            <%# Eval( "CollectDate" ).ToString( )%>
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
