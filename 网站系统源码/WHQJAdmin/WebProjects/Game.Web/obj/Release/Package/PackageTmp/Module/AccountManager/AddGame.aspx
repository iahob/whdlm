<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AddGame.aspx.cs" Inherits="Game.Web.Module.AccountManager.AddGame" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
  <link href="../../styles/layout.css" rel="stylesheet" type="text/css"/>
  <script type="text/javascript" src="../../scripts/common.js"></script>
  <script type="text/javascript" src="/scripts/jquery.js"></script>
  <title>添加用户游戏</title>
</head>
<body>
<form id="form1" runat="server">
  <!-- 头部菜单 Start -->
  <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
    <tr>
      <td width="19" height="25" valign="top" class="Lpd10">
        <div class="arr"></div>
      </td>
      <td width="1232" height="25" valign="top" align="left">你当前位置：用户系统 - 添加游戏</td>
    </tr>
  </table>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
      <td class="titleOpBg Lpd10">
        <input type="button" value="关闭" class="btn wd1" onclick="window.close();"/>
        <input class="btnLine" type="button"/>
        <asp:Button ID="btnSave" runat="server" Text="保存" CssClass="btn wd1"
                    onclick="btnSave_Click"/>
      </td>
    </tr>
  </table>
      <table id="table" width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
          <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
            <div class="hg3  pd7">
              游戏配置信息
            </div>
          </td>
        </tr>
        <tr id="GID" runat="server">
          <td class="listTdLeft">游戏ID：</td>
          <td>
            <asp:TextBox runat="server" ID="txtGameID" CssClass="text"></asp:TextBox>
            <input type="button" value="检测" class="btn wd1" id="check" data-type="<%=TypeID %>"/>   
            <span class="hong">*</span>
          </td>
        </tr>
        <tr id="GNAME" runat="server">
            <td class="listTdLeft">用户昵称：</td>
            <td>        
                <asp:TextBox ID="txtNickName" runat="server" CssClass="text" Enabled="false"></asp:TextBox>   
                <asp:HiddenField ID="hdfUserID" runat="server" Value="0" />  
                <asp:HiddenField ID="hdfAgentID" runat="server" Value="0" />
            </td>
        </tr>
        <tr>
          <td class="listTdLeft">选择游戏：</td>
          <td>
              <input type="checkbox" id="cbAll"  name="cbAll"/>全选
             <asp:CheckBoxList ID="CheckBoxList1"  runat="server" Width="100%">
             </asp:CheckBoxList>
          <asp:DropDownList ID="ddlKind" runat="server" Width="155px">
            </asp:DropDownList>
          </td>
        </tr>
        <tr>
          <td class="listTdLeft">排序标识：</td>
          <td>
            <asp:TextBox runat="server" ID="txtSort" Text="0" CssClass="text"></asp:TextBox>
            <span class="hong">*</span>
          </td>
        </tr>
      </table>
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
      <td class="titleOpBg Lpd10">
        <input type="button" value="关闭" class="btn wd1" onclick="window.close();"/>
        <input class="btnLine" type="button"/>
        <asp:Button ID="btnSave1" runat="server" Text="保存" CssClass="btn wd1" onclick="btnSave_Click"/>
      </td>
    </tr>
  </table>
</form>
<script type="text/javascript">
    $(function () {
        $('#check').on('click', function () {
            var typeid = $(this).attr('data-type');
            var gameID = $("#txtGameID").val();
            $.ajax({
                url: '/Tools/Operating.ashx?action=getuserinfo',
                type: 'post',
                data: { GameID: gameID },
                dataType: 'json',
                success: function (result) {
                    if (result.data.valid) {
                        $("#txtNickName").val(result.data.NickName);
                        $("#hdfUserID").val(result.data.UserID);
                        $("#hdfAgentID").val(result.data.AgentID);
                        if (typeid == 2 && result.data.AgentID <= 0) {
                            $("#txtNickName").val("");
                            $("#hdfUserID").val(0);
                            $("#hdfAgentID").val(0);
                            $("#txtGameID").val('');
                            alert('选择的用户为非代理账号');
                        }
                    } else {
                        $("#txtNickName").val("");
                        $("#hdfUserID").val(0);
                        alert(result.msg);
                    }
                }
            });
        });
        
        //全选
        $('#cbAll').on('click', function () {
            for (var i = 0; i < document.getElementById("CheckBoxList1").getElementsByTagName("input").length; i++) {
                var objCheck = document.getElementById("CheckBoxList1_" + i);
                if (objCheck.checked)
                    objCheck.checked = false;
                else
                    objCheck.checked = true;
            }
        });
        
    })
</script>
</body>
</html>
