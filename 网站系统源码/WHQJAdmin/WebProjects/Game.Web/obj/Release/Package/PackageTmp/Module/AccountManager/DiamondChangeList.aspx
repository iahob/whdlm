﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="DiamondChangeList.aspx.cs" Inherits="Game.Web.Module.AccountManager.DiamondChangeList" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>用户信息</title>
    <link href="/styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="/scripts/common.js"></script>
    <script type="text/javascript" src="/scripts/comm.js"></script>
    <script type="text/javascript" src="/scripts/jquery.js"></script>
    <script type="text/javascript" src="../../scripts/My97DatePicker/WdatePicker.js"></script>

</head>
<body>
    <!-- 头部菜单 Start -->
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top" class="Lpd10">
                <div class="arr">
                </div>
            </td>
            <td width="1232" height="25" valign="top" align="left" style="width: 1232px; height: 25px; vertical-align: top; text-align: left;">目前操作功能：钻石流水
            </td>
        </tr>
    </table>
    <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0" class="Tmg7">
        <tr>
            <td height="28">
                <ul>
                    <li class="tab2" onclick="Redirect('AccountsBaseInfo.aspx?param='+GetRequest('param',0))">用户信息</li>
                    <li class="tab1">钻石流水</li>
                    <li class="tab2" onclick="Redirect('TreasureChangeList.aspx?param='+GetRequest('param',0))">金币流水</li>
                </ul>
            </td>
        </tr>
    </table>
    <form runat="server" id="form1">
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg">
            <tr>
                <td class="listTdLeft" style="width: 80px">日期查询：
                </td>
                <td>
                    <asp:TextBox ID="txtStartDate" runat="server" CssClass="text wd2" onfocus="WdatePicker({dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"></asp:TextBox><img
                        src="../../Images/btn_calendar.gif" onclick="WdatePicker({el:'txtStartDate',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"
                        style="cursor: pointer; vertical-align: middle" />
                    至
                <asp:TextBox ID="txtEndDate" runat="server" CssClass="text wd2" onfocus="WdatePicker({dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"></asp:TextBox><img
                    src="../../Images/btn_calendar.gif" onclick="WdatePicker({el:'txtEndDate',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"
                    style="cursor: pointer; vertical-align: middle" /><asp:DropDownList ID="ddlSerialType" runat="server"></asp:DropDownList>
                    <asp:Button ID="btnQuery" runat="server" Text="查询" CssClass="btn wd1" OnClick="btnQuery_Click" />
                    <asp:Button ID="btnQueryTD" runat="server" Text="今天" CssClass="btn wd1" OnClick="btnQueryTD_Click" />
                    <asp:Button ID="btnQueryYD" runat="server" Text="昨天" CssClass="btn wd1" OnClick="btnQueryYD_Click" />
                    <asp:Button ID="btnQueryTW" runat="server" Text="本周" CssClass="btn wd1" OnClick="btnQueryTW_Click" />
                    <asp:Button ID="btnQueryYW" runat="server" Text="上周" CssClass="btn wd1" OnClick="btnQueryYW_Click" />
                </td>
            </tr>
        </table>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="titleQueBg Tmg7">
            <tr>
                <td class="listTdLeft" style="width: 80px">查询统计：
                </td>
                <td>
                    <span style="color: red; font-weight: bold; line-height: 26px;">钻石变化：
                    <asp:Label ID="lbChange" runat="server" Text="0 （消耗：0  增加：0）"></asp:Label>
                    </span>
                   <%-- <input type="button" value="消耗记录" class="btn wd2" onclick="GetWindowOwn('DiamondConsume.aspx?param=' + GetRequest('param', 0), 'consume', 850, 700);" />
                       <input type="button" value="增加记录" class="btn wd2" onclick="GetWindowOwn('DiamondIncome.aspx?param=' + GetRequest('param', 0), 'income', 850, 700);" />
                       --%>
                      <asp:Button ID="btnOut" runat="server" Text="消耗记录" CssClass="btn wd2" OnClick="btnOut_Click" />
                     <asp:Button ID="btnIn" runat="server" Text="增加记录" CssClass="btn wd2" OnClick="btnIn_Click" />
                </td>
            </tr>
        </table>
        <div id="content">
            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="box Tmg7" id="list">
                <tr align="center" class="bold">
                    <td class="listTitle2">流水号
                    </td>
                    <td class="listTitle">流水时间
                    </td>
                    <td class="listTitle2">
                        <a style="color:red;" href="javascript:;" id="stype">流水类型<img src="/images/down.gif" style="margin-left:5px;" alt="选择类型" /></a>
                    </td>
                    <td class="listTitle2">操作前钻石
                    </td>
                    <td class="listTitle2">钻石变化
                    </td>
                    <td class="listTitle2">操作后钻石
                    </td>
                    <td class="listTitle2">操作地址
                    </td>
                </tr>
                <tr>
                    <td colspan="2"></td>
                    <td>
                        <div class="s-type hide" id="stype-list">
                            <asp:CheckBoxList ID="cbType" runat="server" RepeatLayout="Flow">
                            </asp:CheckBoxList>
                            <span class="s-btn">
                                <asp:Button ID="btnType" runat="server" CssClass="btn wd2" Text="确认" OnClick="btnType_Click" /></span>
                        </div>
                    </td>
                    <td colspan="4"></td>
                </tr>
                <asp:Repeater ID="rptDataList" runat="server">
                    <ItemTemplate>
                        <tr align="center" class="list" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                            onmouseout="this.style.backgroundColor=currentcolor">
                            <td>
                                <%# Eval( "SerialNumber" )%>
                            </td>
                            <td>
                                <%# Eval( "CollectDate", "{0:yyyy-MM-dd HH:mm:ss}" )%>
                            </td>
                            <td>
                                <%# GetDiamondType(Convert.ToInt32(Eval( "TypeID" ))) %>
                            </td>
                            <td>
                                <%# Eval( "CurDiamond" )%>
                            </td>
                            <td>
                                <%# Convert.ToInt32(Eval("ChangeDiamond"))<0?(Eval("ChangeDiamond").ToString()) : ("+" + Eval("ChangeDiamond").ToString()) %>
                            </td>
                            <td>
                                <%# Convert.ToInt32(Eval( "CurDiamond" )) + Convert.ToInt32(Eval( "ChangeDiamond" )) %>
                            </td>
                            <td>
                                <%# Eval( "ClientIP" ) %>
                            </td>
                        </tr>
                    </ItemTemplate>
                    <AlternatingItemTemplate>
                        <tr align="center" class="listBg" onmouseover="currentcolor=this.style.backgroundColor;this.style.backgroundColor='#caebfc';this.style.cursor='default';"
                            onmouseout="this.style.backgroundColor=currentcolor">
                            <td>
                                <%# Eval( "SerialNumber" )%>
                            </td>
                            <td>
                                <%# Eval( "CollectDate", "{0:yyyy-MM-dd HH:mm:ss}" )%>
                            </td>
                            <td>
                                <%# GetDiamondType(Convert.ToInt32(Eval( "TypeID" ))) %>
                            </td>
                            <td>
                                <%# Eval( "CurDiamond" )%>
                            </td>
                            <td>
                                <%# Convert.ToInt32(Eval("ChangeDiamond"))<0?(Eval("ChangeDiamond").ToString()) : ("+" + Eval("ChangeDiamond").ToString()) %>
                            </td>
                            <td>
                                <%# Convert.ToInt32(Eval( "CurDiamond" )) + Convert.ToInt32(Eval( "ChangeDiamond" )) %>
                            </td>
                            <td>
                                <%# Eval( "ClientIP" ) %>
                            </td>
                        </tr>
                    </AlternatingItemTemplate>
                </asp:Repeater>
                <asp:Literal runat="server" ID="litNoData" Visible="false" Text="<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>"></asp:Literal>
            </table>
        </div>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
                <td align="right" class="page">
                    <gsp:AspNetPager ID="anpNews" OnPageChanged="anpPage_PageChanged" runat="server" AlwaysShow="true" FirstPageText="首页" LastPageText="末页"
                        PageSize="20" NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table"
                        NumericButtonCount="5" CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%" UrlPaging="false">
                    </gsp:AspNetPager>
                </td>
            </tr>
        </table>

    </form>
    <script type="text/javascript">
        function GetWindowOwn(url, name, width, height) {
            var left = (window.screen.availWidth - width) / 2;
            var top = (window.screen.availHeight - height) / 2;
            var start = document.getElementById('txtStartDate').value;
            var end = document.getElementById('txtEndDate').value;
            url = url + '&stime=' + start + '&etime=' + end;
            var openthiswin = window.open(url, name, 'toolbar=no,location=no,directories=no,status=yes,menubar=no,scrollbars=yes,resizable=yes,left=' + left + ',top=' + top + ',width=' + width + ',height=' + height);
            if (openthiswin != null) openthiswin.focus();
        }

        $(function () {
            var list = $('#stype-list');
            var trigger = $('#stype');

            trigger.on('click', function () {
                if (list.hasClass('hide')) {
                    list.removeClass('hide');
                } else {
                    list.addClass('hide');
                }
            });
            $(document).on('click', function (e) {
                var target = e.target;
                if (target !== trigger[0] && target !== list[0] && !list[0].contains(target)) {
                    list.addClass('hide');
                }
            });
        })
    </script>
</body>
</html>
