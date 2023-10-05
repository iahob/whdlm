﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="KindRuleList.aspx.cs" Inherits="Game.Web.Module.WebManager.KindRuleList" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="../../scripts/common.js"></script>
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <!-- 头部菜单 Start -->
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top"  class="Lpd10"><div class="arr"></div></td>
            <td width="1232" height="25" valign="top" align="left">你当前位置：网站系统 - 游戏规则</td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td height="39" class="titleOpBg">
                <input type="button" value="新增" class="btn wd1" onclick="Redirect('KindRuleInfo.aspx')" />
                <input class="btnLine" type="button" /> 
                <asp:Button ID="btnDelete" runat="server" Text="删除" CssClass="btn wd1" 
                    onclick="btnDelete_Click" OnClientClick="return deleteop()" />     
                <input class="btnLine" type="button" /> 
                <asp:Button ID="btnDongjie" runat="server" Text="禁用" CssClass="btn wd1" 
                    onclick="btnDongjie_Click" OnClientClick="return deleteop()" />    
                <asp:Button ID="btnJieDong" runat="server" Text="启用" CssClass="btn wd1" 
                    onclick="btnJieDong_Click" OnClientClick="return deleteop()" />     
                 <input class="btnLine" type="button" /> 
                <asp:DropDownList ID="ddlGame" runat="server">
                   
                </asp:DropDownList>
                 <input class="btnLine" type="button" /> 
                <asp:DropDownList ID="ddlType" runat="server" Width="100px">
                    <asp:ListItem Text="全部" Value="0"></asp:ListItem>
                    <asp:ListItem Text="横屏" Value="1"></asp:ListItem>
                    <asp:ListItem Text="竖屏" Value="2"></asp:ListItem>
                </asp:DropDownList>   
                <asp:Button ID="btnQuery" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery_Click" />
            </td>
        </tr>
    </table>  
    <div id="content">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
            <tr align="center" class="bold">
                <td style="width:30px;" class="listTitle"><input type="checkbox" name="chkAll" onclick="SelectAll(this.checked);" /></td>
                <td style="width:100px;" class="listTitle2">管理</td>
                <td class="listTitle2">游戏标识</td>
                <td class="listTitle2">玩法排序</td>
                <td class="listTitle2">游戏名称</td>
              <%--  <td class="listTitle2">游戏图标</td>--%>
                <td class="listTitle2">游戏规则图片</td> 
                 <td class="listTitle2">图片类型</td> 
                <td class="listTitle2">禁用状态</td>
                <td class="listTitle1">插入时间</td>           
            </tr>
            <asp:Repeater ID="rptIssue" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                    onmouseout="this.style.backgroundColor=currentcolor">
                        <td><input name='cid' type='checkbox' value='<%# Eval("ID")%>'/></td>      
                        <td>                             
                            <a class="l" href="KindRuleInfo.aspx?param=<%#Eval("ID") %>">编辑</a>              
                        </td>                
                        <td><%# Eval("KindID") %></td>
                        <td><%# Eval("SortID") %></td>
                        <td><%# Eval("KindName") %></td>
                    <%--    <td style="padding:2px;"><img src="/Upload<%# Eval( "KindIcon" )%>" width="100" height="50"/></td>--%>
                        <td style="padding:2px;"><img src="<%# Eval( "RuleImg" )%>"  height="64"/></td>
                         <td><%# Eval("Type").ToString()=="1"?"横屏":"竖屏" %></td>
                        <td><%# GetNullityStatus((byte)(Eval("Nullity")))%></td>                      
                        <td><%# Eval("CollectDate")%></td>
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="listBg" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                    onmouseout="this.style.backgroundColor=currentcolor">                        
                        <td><input name='cid' type='checkbox' value='<%# Eval("ID")%>'/></td>      
                        <td>                             
                            <a class="l" href="KindRuleInfo.aspx?param=<%#Eval("ID") %>">编辑</a>              
                        </td>                
                        <td><%# Eval("KindID") %></td>
                        <td><%# Eval("SortID") %></td>
                        <td><%# Eval("KindName") %></td>
                     <%--   <td style="padding:2px;"><img src="/Upload<%# Eval( "KindIcon" )%>" width="100" height="50"/></td>--%>
                        <td style="padding:2px;"><img src="<%# Eval( "RuleImg" )%>" height="64"/></td>
                         <td><%# Eval("Type").ToString()=="1"?"横屏":"竖屏" %></td>
                        <td><%# GetNullityStatus((byte)(Eval("Nullity")))%></td>                      
                        <td><%# Eval("CollectDate")%></td>                    
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
