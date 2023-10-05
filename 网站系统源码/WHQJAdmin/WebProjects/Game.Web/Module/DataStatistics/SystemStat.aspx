<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="SystemStat.aspx.cs" Inherits="Game.Web.Module.DataStatistics.SystemStat" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css"/>
    <script type="text/javascript" src="../../scripts/common.js"></script>
    <title></title>
    <style type="text/css">
        .gamelist { width: 650px; }

        .gamelist span {
            float: left;
            width: 200px;
            height: 23px;
            text-align: left;
            margin-top: 0px;
            margin-right: 0;
            margin-bottom: 2px;
            margin-left: 0;
        }
    </style>
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
            你当前位置：数据分析 - 系统统计
        </td>
    </tr>
</table>
<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
        <td class="titleOpBg Lpd10">
            <input type="button" id="btnRefresh" class="btn wd1" value="刷新" onclick="javascript:location.href = location.href;"/>
        </td>
    </tr>
</table>
<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
<tr>
    <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
        <div class="hg3  pd7">
            用户统计
        </div>
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        平台总用户：
    </td>
    <td>
        <asp:Literal ID="ltTotalUser" runat="server"></asp:Literal> 人
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日网站注册：
    </td>
    <td>
        <asp:Literal ID="ltWebRegisterUser" runat="server"></asp:Literal> 人
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日游戏注册：
    </td>
    <td>
        <asp:Literal ID="ltGameRegisterUser" runat="server"></asp:Literal> 人
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日H5注册：
    </td>
    <td>
        <asp:Literal ID="ltH5RegisterUser" runat="server"></asp:Literal> 人
    </td>
</tr>
<tr>
    <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
        <div class="hg3  pd7">
            金币统计
        </div>
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        身上金币总量：
    </td>
    <td>
        <asp:Literal ID="ltScore" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        保险柜总量：
    </td>
    <td>
        <asp:Literal ID="ltInsureScore" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        保险柜+身上总量：
    </td>
    <td>
        <asp:Literal ID="ltAllScore" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日充值金币：
    </td>
    <td>
        <asp:Literal ID="ltPayScore" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日充值金币金额：
    </td>
    <td>
        <asp:Literal ID="ltPayScoreAmount" runat="server"></asp:Literal> RMB
    </td>
</tr>
<tr>
    <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
        <div class="hg3  pd7">
            钻石统计
        </div>
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        钻石总量：
    </td>
    <td>
        <asp:Literal ID="ltDiamond" runat="server"></asp:Literal> 钻石
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日充值钻石：
    </td>
    <td>
        <asp:Literal ID="ltPayDiamond" runat="server"></asp:Literal> 钻石
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日充值钻石金额：
    </td>
    <td>
        <asp:Literal ID="ltPayDiamondAmount" runat="server"></asp:Literal> 钻石
    </td>
</tr>
<tr>
    <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
        <div class="hg3  pd7">
            充值统计
        </div>
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        总充值金额：
    </td>
    <td>
        <asp:Literal ID="ltTotalAmount" runat="server"></asp:Literal> RMB
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日充值金额：
    </td>
    <td>
        <asp:Literal ID="ltAmount" runat="server"></asp:Literal> RMB
    </td>
</tr>
<tr>
    <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
        <div class="hg3  pd7">
            服务费统计
        </div>
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        银行+游戏总服务费：
    </td>
    <td>
        <asp:Literal ID="ltAllRevenue" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        游戏总服务费：
    </td>
    <td>
        <asp:Literal ID="ltTotalRevenue" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日游戏服务费：
    </td>
    <td>
        <asp:Literal ID="ltRevenue" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        银行总服务费：
    </td>
    <td>
        <asp:Literal ID="ltTotalInsureRevenue" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日银行服务费：
    </td>
    <td>
        <asp:Literal ID="ltInsureRevenue" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
        <div class="hg3  pd7">
            损耗统计
        </div>
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        损耗总量：
    </td>
    <td>
        <asp:Literal ID="ltTotalWaste" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        今日损耗：
    </td>
    <td>
        <asp:Literal ID="ltWaste" runat="server"></asp:Literal> 金币
    </td>
</tr>
<tr>
    <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
        <div class="hg3  pd7">
            开房统计
        </div>
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        积分约战次数：
    </td>
    <td>
        <asp:Literal ID="ltScoreRoomCount" runat="server"></asp:Literal> 次
    </td>
</tr>
<tr>
    <td class="listTdLeft">
        金币约战次数：
    </td>
    <td>
        <asp:Literal ID="ltGoldRoomCount" runat="server"></asp:Literal> 次
    </td>
</tr>
</table>
<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
        <td class="titleOpBg Lpd10">
            <input type="button" id="btnRefresh1" class="btn wd1" value="刷新" onclick="javascript:location.href = location.href;"/>
        </td>
    </tr>
</table>
</form>
</body>
</html>