﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RecordGrantGameID.aspx.cs" Inherits="Game.Web.Module.AccountManager.RecordGrantGameID" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

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
                你当前位置：用户系统 - 赠送靓号记录
            </td>
        </tr>
    </table>
    <!-- 头部菜单 End -->
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="关闭" class="btn wd1" onclick="window.close();" />
            </td>
        </tr>
    </table>
    <div id="content">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
            <tr align="center" class="bold">
                <td class="listTitle2">
                    赠送时间
                </td>
                <td class="listTitle2">
                    赠送靓号
                </td>
                <td class="listTitle2">
                    赠送前靓号
                </td>
                <td class="listTitle2">
                    赠送管理员
                </td>
                <td class="listTitle2">
                    赠送地址
                </td>
            </tr>
            <asp:Repeater ID="rptDataList" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td>
                            <%# Eval( "CollectDate") %>
                        </td>
                        <td>
                            <%# Eval( "ReGameID") %>
                        </td>
                        <td>
                            <%# Eval( "CurGameID") %>
                        </td>
                        <td>
                            <%# GetMasterName(Convert.ToInt32(Eval( "MasterID"))) %>
                        </td>
                        <td>
                            <%# Eval( "ClientIP") %>
                        </td>
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="listBg" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td>
                            <%# Eval( "CollectDate") %>
                        </td>
                        <td>
                            <%# Eval( "ReGameID") %>
                        </td>
                        <td>
                            <%# Eval( "CurGameID") %>
                        </td>
                        <td>
                            <%# GetMasterName(Convert.ToInt32(Eval( "MasterID"))) %>
                        </td>
                        <td>
                            <%# Eval( "ClientIP") %>
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
