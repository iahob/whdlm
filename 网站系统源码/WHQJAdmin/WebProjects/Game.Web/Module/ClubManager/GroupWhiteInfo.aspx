<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GroupWhiteInfo.aspx.cs" Inherits="Game.Web.Module.ClubManager.GroupWhiteInfo" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <link href="/styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="/scripts/common.js"></script>
    <script type="text/javascript" src="/scripts/comm.js"></script>
    <script type="text/javascript" src="/scripts/jquery.js"></script>

    <title></title>
    <script type="text/javascript">
        function getnickname() {
            var gameID = $("#txtGameID").val();
            console.info(gameID);
            if (gameID == "") {
                $("#useridTips").html("请输入GameID").addClass("hong");
                return;
            }
            $.ajax({
                url: '/Tools/Operating.ashx?action=getuserinfo',
                type: 'post',
                data: { GameID: gameID },
                dataType: 'json',
                success: function(result) {
                    if (result.data.valid) {
                        $("#hidUserID").val(result.data.UserID);
                        $("#useridTips").html("成功").removeClass("hong");
                    } else {
                        $("#hidUserID").val("");
                        $("#useridTips").html(result.msg).addClass("hong");
                    }
                },
                complete: function() {

                }
            });
        }
    </script>
</head>
<body>
    <form id="form1" runat="server">
        <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
            <tr>
                <td width="19" height="25" valign="top" class="Lpd10">
                    <div class="arr"></div>
                </td>
                <td width="1232" height="25" valign="top" align="left">
                    你当前位置：大联盟系统 - 白名单管理
                </td>
            </tr>
        </table>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
                <td class="titleOpBg Lpd10">
                    <input id="btnReturn" type="button" value="返回" class="btn wd1" onclick="Redirect('GroupWhiteList.aspx')" />
                    <asp:Button ID="btnCreate" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
                </td>
            </tr>
        </table>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2"> 
            <tr>
                <td class="listTdLeft">
                    赠送ID：
                </td>
                <td>
                    <asp:TextBox ID="txtGameID" runat="server" CssClass="text wd4" MaxLength="7"></asp:TextBox>
                    <span class="hong">*</span>
                    <input type="button" value="检测" class="btn wd1" onclick="getnickname()"/>
                    <asp:HiddenField runat="server" ID="hidUserID"/>
                    <span id="useridTips"></span>
                </td>
            </tr>
        </table>
        <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
                <td class="titleOpBg Lpd10">
                    <input id="btnReturn2" type="button" value="返回" class="btn wd1" onclick="Redirect('GroupWhiteList.aspx')" />
                    <asp:Button ID="btnSave2" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
                </td>
            </tr>
        </table>
    </form>
</body>
</html>

