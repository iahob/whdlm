﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GameSignInList.aspx.cs" Inherits="Game.Web.Module.AppManager.GameSignInList" %>

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
                你当前位置：系统维护 - 签到配置
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
        <tr>
            <td height="28">
                <ul>
                    <li class="tab2" onclick="Redirect('GamePackageConfigList.aspx')">签到礼包配置</li>
                    <li class="tab2" onclick="Redirect('GamePackageGoodsList.aspx')">签到物品配置</li>
                    <li class="tab1" >签到配置</li>
                </ul>
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <td height="39" class="titleOpBg">
                <input type="button" value="新增" class="btn wd1" onclick="Redirect('GameSignInInfo.aspx?cmd=add')" />
                <input class="btnLine" type="button" /> 
                <asp:Button ID="btnDelete" runat="server" Text="删除" CssClass="btn wd1" 
                    onclick="btnDelete_Click" OnClientClick="return deleteop()" />              
            <input class="btnLine" type="button"/>  
             <input class="btn wd3" type="button" value="签到记录" onclick="openWindowOwn('RecordGameSignInList.aspx?r=' + Math.random(), '签到礼包记录', 1200, 700)"/>
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
                    签到类型
                </td>                
                <td class="listTitle2">
                    签到获得礼包
                </td>                
                <td class="listTitle2">
                    签到抽奖获得礼包的概率
                </td>  
                  <td class="listTitle2">
                    累计签到所需天数
                </td>   
                <td class="listTitle2">
                    排序标识
                </td>  
                 <td class="listTitle2">
                    状态
                </td>    
                <td class="listTitle2">
                    配置时间
                </td>                  
            </tr>
            <asp:Repeater ID="rptDataList" runat="server">
                <ItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                        <td><input name='cid' type='checkbox' value='<%# Eval("SignID")%>'/></td>      
                        <td>
                            <a href="GameSignInInfo.aspx?param=<%# Eval("SignID")%>" class="l">编辑</a>
                        </td>
                        <td>
                            <%# Convert.ToInt32( Eval( "TypeID" ))==0?"每日签到":"累计签到"%>
                        </td>                        
                        <td>
                            <%#GetPackageName(Convert.ToInt32( Eval( "PackageID" )))%>
                        </td>                           
                        <td>
                            <%# Eval("Probability")%>%
                        </td>  
                        <td>
                            <%#Convert.ToInt32( Eval( "NeedDay" ))%>
                        </td>      
                        <td>
                            <%# Eval( "SortID" )%>
                        </td> 
                         <td>
                            <%#Convert.ToInt32( Eval( "Nullity" ))==0?"正常":"禁用"%>
                        </td> 
                         <td>
                            <%# Eval( "CollectDate" )%>
                        </td>                          
                    </tr>
                </ItemTemplate>
                <AlternatingItemTemplate>
                    <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                        onmouseout="this.style.backgroundColor=currentcolor">
                       <td><input name='cid' type='checkbox' value='<%# Eval("SignID")%>'/></td>      
                        <td>
                            <a href="GameSignInInfo.aspx?param=<%# Eval("SignID")%>" class="l">编辑</a>
                        </td>
                        <td>
                            <%# Convert.ToInt32( Eval( "TypeID" ))==0?"每日签到":"累计签到"%>
                        </td>                        
                        <td>
                            <%#GetPackageName(Convert.ToInt32( Eval( "PackageID" )))%>
                        </td>                           
                        <td>
                            <%# Eval("Probability")%>%
                        </td>  
                        <td>
                            <%#Convert.ToInt32( Eval( "NeedDay" ))%>
                        </td>      
                        <td>
                            <%# Eval( "SortID" )%>
                        </td> 
                         <td>
                            <%#Convert.ToInt32( Eval( "Nullity" ))==0?"正常":"禁用"%>
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
