<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GroupWhiteList.aspx.cs" Inherits="Game.Web.Module.ClubManager.GroupWhiteList" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>

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
                <td width="1232" height="25" valign="top" align="left">你当前位置：大联盟系统 - 白名单管理
                </td>
            </tr>
        </table>
        <div id="content">
            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg">
                <tr>
                    <td align="center" style="width: 80px">用户查询：
                    </td>
                    <td>
                        <asp:TextBox ID="txtSearch" runat="server" CssClass="text"></asp:TextBox>
                        <asp:DropDownList ID="ddlSearchType" runat="server">
                            <asp:ListItem Text="游戏ID" Value="1"></asp:ListItem>
                            <asp:ListItem Text="昵称" Value="2"></asp:ListItem>
                        </asp:DropDownList>
                        <asp:Button ID="btnQuery" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery_Click" />
                    </td>
                </tr>
            </table>
            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
                <tr>
                    <td height="39" class="titleOpBg">
                        <input type="button" value="新增" class="btn wd1" onclick="openWindowOwn('GroupWhiteInfo.aspx', 'addagent', 700, 600)" />
                        <asp:Button ID="btnDelete" runat="server" Text="删除" CssClass="btn wd1" OnClick="btnDelete_Click" OnClientClick="return deleteop()"/>
                    </td>
                </tr>
            </table>
            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
                <tr align="center" class="bold">
                    <td style="width:30px;" class="listTitle"><input type="checkbox" name="chkAll" onclick="SelectAll(this.checked);" /></td>
                    <td class="listTitle2">管理
                    </td>
                    <td class="listTitle2">玩家信息
                    </td>
                    <td class="listTitle2">插入时间
                    </td>
                </tr>
                <asp:Repeater ID="rptDataList" runat="server">
                    <ItemTemplate>
                        <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                            onmouseout="this.style.backgroundColor=currentcolor">
                            <td><input name='cid' type='checkbox' value='<%# Eval("UserID")%>'/></td>      
                            <td>
                                <a href="GroupWhiteInfo.aspx?param=<%# Eval("UserID").ToString()%>" class="l">编辑</a>
                            </td>
                            <td>
                                <%# GetAccountsInfo( Convert.ToInt32(Eval( "UserID" ))) %>
                            </td>
                            <td>
                                <%# Eval( "InsertTime" ).ToString()%>
                            </td>
                        </tr>
                    </ItemTemplate>
                    <AlternatingItemTemplate>
                        <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                            onmouseout="this.style.backgroundColor=currentcolor">
                            <td><input name='cid' type='checkbox' value='<%# Eval("UserID")%>'/></td>      
                            <td>
                                <a href="GroupWhiteInfo.aspx?param=<%# Eval("UserID").ToString()%>" class="l">编辑</a>
                            </td>
                            <td>
                                <%# GetAccountsInfo( Convert.ToInt32(Eval( "UserID" ))) %>
                            </td>
                            <td>
                                <%# Eval( "InsertTime" ).ToString()%>
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
                    <gsp:AspNetPager ID="anpNews" runat="server" OnPageChanged="anpNews_PageChanged" AlwaysShow="true" FirstPageText="首页" LastPageText="末页" PageSize="20"
                        NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table" NumericButtonCount="5"
                        CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%">
                    </gsp:AspNetPager>
                </td>
            </tr>
        </table>
    </form>
</body>
</html>
