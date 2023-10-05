<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RecordGoldBattleInfo.aspx.cs" Inherits="Game.Web.Module.GoldManager.RecordGoldBattleInfo" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="../../scripts/common.js"></script>
    <title>金币约战战绩</title>
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
                你当前位置：金币系统 - 约战战绩
            </td>
        </tr>
    </table>
    <!-- 头部菜单 End -->
    <div id="content">
        <%=TableData %>
    </div>
    </form>
</body>
</html>
