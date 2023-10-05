<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GroupGameControlSubInfo.aspx.cs" Inherits="Game.Web.Module.DebugManager.GroupGameControlSubInfo" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>用户信息</title>
    <link href="/styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="/scripts/common.js"></script>
    <script type="text/javascript" src="/scripts/comm.js"></script>
    <script type="text/javascript" src="/scripts/jquery.js"></script>
    <style type="text/css">
        .listTdLeft {
            width: 82px;
        }

        .listTdRight1 {
            width: 200px;
        }

        .listTdBtn {
            text-align: center;
        }

            .listTdBtn .btn1 {
                height: 41px;
                font-size: 15px;
                margin: 10px auto;
            }
    </style>
</head>
<body>
    <!-- 头部菜单 Start -->
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top" class="Lpd10">
                <div class="arr">
                </div>
            </td>
            <td width="1232" height="25" valign="top" align="left" style="width: 1232px; height: 25px; vertical-align: top; text-align: left;">你当前位置：调试系统 - 游戏控制 - 联盟列表 - 游戏列表 - 配置修改
            </td>
        </tr>
    </table>
    <form runat="server" id="form1">
        <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
                <td class="titleOpBg Lpd10">
                    <input type="button" value="关闭" class="btn wd1" onclick="window.close();" />
                  <asp:Button ID="btnRefresh" runat="server" Text="刷新" CssClass="btn wd1" OnClick="btnRefresh_Click"   />
                </td>
            </tr>
        </table>
        <table style="width: 99%;  border: 1px solid #615a5a; " border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
            <tr>
                <td height="35" colspan="4" class="f14 bold Lpd8 Rpd8">
                    <div class="hg3  pd7">
                        抽水配置
                    </div>
                </td>
            </tr>
            <tr>
                <td class="listTdLeft">当前抽水比例：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurAnChouRate" runat="server"></asp:Literal>%
                </td>
                <td class="listTdLeft">当前抽水总数：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurAnChouTotal" runat="server"></asp:Literal>
                </td>

            </tr>
            <tr>
                <td class="listTdLeft">配置抽水比例：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigAnChouRate" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>%
                </td>
                <td class="listTdLeft"></td>
                <td class="listTdRight1"></td>

            </tr>
            <tr>
                <td height="35" colspan="4" class="listTdBtn">
                    <asp:Button ID="btnAnChou" runat="server" Text="执行" CssClass="btn wd8 btn1" OnClick="btnAnChou_Click" />
                </td>
            </tr>

            <tr>
                <td height="35" colspan="4" class="f14 bold Lpd8 Rpd8">
                    <div class="hg3  pd7">
                        当前库存数据
                    </div>
                </td>
            </tr>
            <tr>
                <td class="listTdLeft">当前系统库存：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurSysStorage" runat="server"></asp:Literal>
                </td>
                <td class="listTdLeft">当前玩家库存：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurPlayerStorage" runat="server"></asp:Literal>
                </td>

            </tr>
            <tr>
                <td class="listTdLeft">当前调节系数：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurParameterK" runat="server"></asp:Literal>%
                </td>
                <td class="listTdLeft">当前强制重置区间：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurSysStorResetRate" runat="server"></asp:Literal>%
                </td>

            </tr>
            <tr>
                <td class="listTdLeft">当前中奖率：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurWinChance" runat="server"></asp:Literal>%
                </td>
                <td class="listTdLeft">当前重置次数：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurResetCount" runat="server"></asp:Literal>
                </td>


            </tr>
            <tr>
                <td class="listTdLeft">当前累计库存统计：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurSysWin" runat="server"></asp:Literal>
                </td>
                <td class="listTdLeft"></td>
                <td class="listTdRight1"></td>
            </tr>

            <tr>
                <td height="35" colspan="4" class="f14 bold Lpd8 Rpd8">
                    <div class="hg3  pd7">
                        库存配置
                    </div>
                </td>
            </tr>
            <tr>
                <td class="listTdLeft">配置系统库存：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigSysStorage" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>
                </td>
                <td class="listTdLeft">配置玩家库存：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigPlayerStorage" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>
                </td>

            </tr>
            <tr>
                <td class="listTdLeft">配置系数：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigParameterK" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>%
                </td>
                <td class="listTdLeft">配置强制重置区间：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigSysStorResetRate" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>%
                </td>

            </tr>
            <tr>
                <td height="35" colspan="4" class="listTdBtn">
                    <asp:Button ID="btnStorge" runat="server" Text="执行" CssClass="btn wd8 btn1" OnClick="btnStorge_Click" />
                </td>
            </tr>
            <tr runat="server" id="trAnchou1">
                <td height="35" colspan="4" class="f14 bold Lpd8 Rpd8">
                    <div class="hg3  pd7">
                        大奖配置
                    </div>
                </td>
            </tr>
            <tr runat="server" id="trAnchou2">
                <td class="listTdLeft">当前大奖库存比例：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurMosgoldStorageRate" runat="server"></asp:Literal>%
                </td>


                <td class="listTdLeft">配置大奖库存比例：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigMosgoldStorageRate" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>%
                </td>

            </tr>
            <tr runat="server" id="trAnchou3">
                <td class="listTdLeft">当前大奖抽取比例：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurMosgoldRate" runat="server"></asp:Literal>%
                </td>
                <td class="listTdLeft">配置大奖抽取比例：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigMosgoldRate" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>%
                </td>

            </tr>
            <tr runat="server" id="trAnchou4">
                <td class="listTdLeft">当前派奖概率：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurMosgoldDispatchRate" runat="server"></asp:Literal>%
                </td>
                <td class="listTdLeft">配置派奖概率：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigMosgoldDispatchRate" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>%
                （建议配置8% - 15%）
                </td>
            </tr>
            <tr runat="server" id="trAnchou5">
                <td height="35" colspan="4" class="listTdBtn">
                    <asp:Button ID="btnMosgold" runat="server" Text="执行" CssClass="btn wd8 btn1" OnClick="btnMosgold_Click" />
                </td>
            </tr>

            <tr>
                <td height="35" colspan="4" class="f14 bold Lpd8 Rpd8">
                    <div class="hg3  pd7">
                        当前临时调试库存数据
                    </div>
                </td>
            </tr>
            <tr>
                <td class="listTdLeft">当前系统库存：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurTempSysStorage" runat="server"></asp:Literal>
                </td>
                <td class="listTdLeft">当前玩家库存：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurTempPlayerStorage" runat="server"></asp:Literal>
                </td>

            </tr>
            <tr>
                <td class="listTdLeft">当前调节系数：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurTempParameterK" runat="server"></asp:Literal>%
                </td>
                <td class="listTdLeft">当前强制重置区间：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurTempSysStorResetRate" runat="server"></asp:Literal>%
                </td>

            </tr>
            <tr>
                <td class="listTdLeft">当前中奖率：
                </td>
                <td class="listTdRight1">
                    <asp:Literal ID="ltCurTempWinChance" runat="server"></asp:Literal>%
                </td>
                <td class="listTdLeft"></td>
                <td class="listTdRight1"></td>

            </tr>
            <tr>
                <td height="35" colspan="4" class="f14 bold Lpd8 Rpd8">
                    <div class="hg3  pd7">
                        临时库存配置
                    </div>
                </td>
            </tr>
            <tr>
                <td class="listTdLeft">配置系统库存：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigTempSysStorage" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>
                </td>
                <td class="listTdLeft">配置玩家库存：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigTempPlayerStorage" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>
                </td>

            </tr>
            <tr>
                <td class="listTdLeft">配置系数：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigTempParameterK" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>%
                </td>
                <td class="listTdLeft">配置强制重置区间：
                </td>
                <td class="listTdRight1">
                    <asp:TextBox ID="txtConfigTempSysStorResetRate" CssClass="text" MaxLength="16" runat="server"></asp:TextBox>%
                </td>

            </tr>
            <tr>
                <td height="35" colspan="4" class="listTdBtn">
                    <asp:Button ID="btnTempStorge" runat="server" Text="执行" CssClass="btn wd8 btn1" OnClick="btnTempStorge_Click" />
                </td>
            </tr>
        </table>


        <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
                <td class="titleOpBg Lpd10">
                    <input type="button" value="关闭" class="btn wd1" onclick="window.close();" />
                  <asp:Button ID="Button1" runat="server" Text="刷新" CssClass="btn wd1" OnClick="btnRefresh_Click"   />
                </td>
            </tr>
        </table>
    </form>
</body>
</html>
