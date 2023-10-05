<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RankingConfigList.aspx.cs" Inherits="Game.Web.Module.AppManager.RankingConfigList" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
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
            <td width="1232" height="25" valign="top" align="left">
                你当前位置：系统维护 - 排行管理
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
        <tr>
            <td height="28">
                <ul>
                    <li class="tab1">排行配置</li>
                    <li class="tab2" onclick="Redirect('RankingRecordList.aspx')">奖励记录</li>
                </ul>
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <td height="39" class="titleOpBg">
                <input type="button" value="新增" class="btn wd1" onclick="Redirect('RankingConfigInfo.aspx?cmd=add')" />
                <input class="btnLine" type="button" /> 
                <asp:Button ID="btnDelete" runat="server" Text="删除" CssClass="btn wd1" 
                    onclick="btnDelete_Click" OnClientClick="return deleteop()" />    
            </td>
    </table>
    <div id="content">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box" id="list">
            <tr align="center" class="bold">
                <td style="width:30px;" class="listTitle"><input type="checkbox" name="chkAll" onclick="SelectAll(this.checked);" /></td>
                <td class="listTitle2">
                    管理
                </td>
                <td class="listTitle2">
                    排行榜类型
                </td>                
                <td class="listTitle2">
                    排行榜排名
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
                <td class="listTitle2">
                    配置时间
                </td>                  
            </tr>
            <asp:Repeater ID="rptDataList" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td><input name='cid' type='checkbox' value='<%# Eval("ConfigID")%>'/></td>      
                        <td>
                            <a href="RankingConfigInfo.aspx?param=<%# Eval("ConfigID")%>" class="l">编辑</a>
                        </td>
                        <td>
                            <%# Eval( "TypeID" ).ToString()=="1"?"财富排行榜":"胜局排行榜" %>
                        </td>                        
                        <td>
                            <%# GetRankType(Convert.ToInt32(Eval( "RankType" ))) %>
                        </td>                           
                        <td>
                            <%# Eval( "Diamond" )%>
                        </td>  
                        <td>
                            <%# Eval( "Gold" ) %>
                        </td> 
                        <td>
                            <%# Eval( "AwardTicket" ) %>
                        </td>      
                        <td>
                            <%# Eval( "CollectDate" )%>
                        </td>                           
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td><input name='cid' type='checkbox' value='<%# Eval("ConfigID")%>'/></td>      
                        <td>
                            <a href="RankingConfigInfo.aspx?param=<%# Eval("ConfigID")%>" class="l">编辑</a>
                        </td>
                        <td>
                            <%# Eval( "TypeID" ).ToString()=="1"?"财富排行榜":"胜局排行榜" %>
                        </td>                        
                        <td>
                            <%# GetRankType(Convert.ToInt32(Eval( "RankType" ))) %>
                        </td>                           
                        <td>
                            <%# Eval( "Diamond" )%>
                        </td>  
                        <td>
                            <%# Eval( "Gold" ) %>
                        </td> 
                         <td>
                            <%# Eval( "AwardTicket" ) %>
                        </td>       
                        <td>
                            <%# Eval( "CollectDate" )%>
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
