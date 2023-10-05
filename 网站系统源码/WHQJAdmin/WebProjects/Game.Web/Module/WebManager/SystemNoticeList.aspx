<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="SystemNoticeList.aspx.cs" Inherits="Game.Web.Module.WebManager.SystemNoticeList" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="../../scripts/common.js"></script>
    <title></title>
    <style type="text/css">
        .auto-style1 {
            background-color: #c2cfd5;
            border-bottom: 1px solid #99a7ae;
            height: 30px;
            white-space: nowrap;
            padding: 0px 5px;
            width: 100px;
        }
    </style>
</head>
<body>
    <form id="form1" runat="server">
    <!-- 头部菜单 Start -->
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top"  class="Lpd10"><div class="arr"></div></td>
            <td width="1232" height="25" valign="top" align="left">你当前位置：网站系统 - 新闻公告</td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td height="39" class="titleOpBg">
                <input type="button" value="新增" class="btn wd1" onclick="Redirect('SystemNoticeInfo.aspx?cmd=add')" />
                <input class="btnLine" type="button" /> 
                <asp:Button ID="btnDelete" runat="server" Text="删除" CssClass="btn wd1" 
                    onclick="btnDelete_Click" OnClientClick="return deleteop()" />                
            </td>
        </tr>
    </table>  
    <div id="content">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
            <tr align="center" class="bold">
                <td style="width:30px; height: 30px;" class="listTitle"><input type="checkbox" name="chkAll" onclick="SelectAll(this.checked);" /></td>
                <td class="auto-style1">管理</td>
                <td class="listTitleBg">新闻标题</td>
                <td class="listTitleBg">新闻来源</td>
               <%-- <td class="listTitleBg">是否热门</td>--%>
                 <td class="listTitleBg">平台类型</td>
                <td class="listTitleBg">发布时间</td>  
                <td class="listTitleBg">新闻排序</td>
                <td class="listTitleBg">禁用状态</td>           
            </tr>
            <asp:Repeater ID="rptIssue" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                    onmouseout="this.style.backgroundColor=currentcolor">
                        <td><input name='cid' type='checkbox' value='<%# Eval("NoticeID")%>'/></td>      
                        <td>                             
                            <a class="l" href="SystemNoticeInfo.aspx?cmd=edit&param=<%#Eval("NoticeID") %>">编辑</a>              
                        </td>                
                        <td><%# Eval("NoticeTitle").ToString().Length>20?Eval("NoticeTitle").ToString().Substring(0,20):Eval("NoticeTitle").ToString() %></td>
                        <td><%# Eval("Publisher") %></td>
                        <%--<td><%# Eval("IsHot").ToString()=="True"?"是":"否" %></td>--%>
                          <td><%# Convert.ToInt32(Eval("PlatformType"))==1?"三端":Convert.ToInt32(Eval("PlatformType"))==2?"LUA":Convert.ToInt32(Eval("PlatformType"))==3?"U3D":"H5" %></td>
                        <td><%# Eval("PublisherTime")%></td>
                        <td><%# Eval("SortID")%></td>
                        <td><%# GetNullityStatus((byte)(Eval("Nullity").ToString()=="True"?1:0))%></td>                        
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="listBg" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                    onmouseout="this.style.backgroundColor=currentcolor">                        
                        <td><input name='cid' type='checkbox' value='<%# Eval("NoticeID")%>'/></td>      
                        <td>                             
                            <a class="l" href="SystemNoticeInfo.aspx?cmd=edit&param=<%#Eval("NoticeID") %>">编辑</a>              
                        </td>                
                        <td><%# Eval("NoticeTitle").ToString().Length>20?Eval("NoticeTitle").ToString().Substring(0,20):Eval("NoticeTitle").ToString() %></td>
                        <td><%# Eval("Publisher") %></td>
                       <%-- <td><%# Eval("IsHot").ToString()=="True"?"是":"否" %></td>--%>
                          <td><%# Convert.ToInt32(Eval("PlatformType"))==1?"三端":Convert.ToInt32(Eval("PlatformType"))==2?"LUA":Convert.ToInt32(Eval("PlatformType"))==3?"U3D":"H5" %></td>
                        <td><%# Eval("PublisherTime")%></td>
                        <td><%# Eval("SortID")%></td>
                        <td><%# GetNullityStatus((byte)(Eval("Nullity").ToString()=="True"?1:0))%></td>                    
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
