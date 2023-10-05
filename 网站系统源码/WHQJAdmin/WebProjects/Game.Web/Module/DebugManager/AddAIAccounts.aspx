<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AddAIAccounts.aspx.cs" Inherits="Game.Web.Module.DebugManager.AddAIAccounts" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>
  <script type="text/javascript" src="/scripts/jquery.js"></script>
    <title></title>
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
                你当前位置：调试系统 - 增减账号 - 联盟列表 - 用户列表 - 增加账号配置
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <asp:Button ID="btnCreate" runat="server" Text="保存" CssClass="btn wd1"  OnClick="btnSave_Click"  />
            </td>
        </tr>
    </table>
    
      <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
          <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
            <div class="hg3  pd7">
              增加账号配置</div>
          </td>
        </tr>
        <tr>
          <td class="listTdLeft">
            对应游戏：
          </td>
          <td>
            <asp:DropDownList ID="ddlGame" runat="server" OnSelectedIndexChanged="ddlGame_SelectedIndexChanged" AutoPostBack="True">
            </asp:DropDownList>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator8" runat="server" ErrorMessage="请选择游戏" ControlToValidate="ddlGame" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
           
          </td>
        </tr> 
        <tr>
          <td class="listTdLeft">
            调试账号数：
          </td>
          <td>
            <asp:TextBox ID="txtAICount" runat="server" CssClass="text"></asp:TextBox>
            <span class="hong">*</span>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" ErrorMessage="请输入调试账号数" ControlToValidate="txtAICount" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server" ErrorMessage="调试账号数格式不正确" Display="Dynamic" ControlToValidate="txtAICount" ValidationExpression="^[1-9]\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
            <span id="spAICount" runat="server"  class="hong"></span>
            </td>
        </tr> 
        <tr>
          <td class="listTdLeft">
            参与比例：
          </td>
          <td>
            <asp:TextBox ID="txtAIRateMin" runat="server" CssClass="text"></asp:TextBox> % - <asp:TextBox ID="txtAIRateMax" runat="server" CssClass="text"></asp:TextBox> %
            <span class="hong">*</span>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" ErrorMessage="请输入参与比例下限" ControlToValidate="txtAIRateMin" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator ID="RegularExpressionValidator2" runat="server" ErrorMessage="参与比例下限格式不正确" Display="Dynamic" ControlToValidate="txtAIRateMin" ValidationExpression="^[1-9]\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" ErrorMessage="请输入参与比例上限" ControlToValidate="txtAIRateMax" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator ID="RegularExpressionValidator3" runat="server" ErrorMessage="参与比例上限格式不正确" Display="Dynamic" ControlToValidate="txtAIRateMax" ValidationExpression="^[1-9]\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
          </td>
        </tr> 
        <tr>
          <td class="listTdLeft">
            调试账号积分上限：
          </td>
          <td>
            <asp:TextBox ID="txtDistributeTotalScore" runat="server" CssClass="text"></asp:TextBox>
            <span class="hong">*</span>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" ErrorMessage="请输入调试账号积分上限" ControlToValidate="txtDistributeTotalScore" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator ID="RegularExpressionValidator4" runat="server" ErrorMessage="调试账号积分上限格式不正确" Display="Dynamic" ControlToValidate="txtDistributeTotalScore" ValidationExpression="^[1-9]\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
            <span id="sptDistributeTotalScore" runat="server"  class="hong"></span>
            </td>
        </tr> 
        <tr>
          <td class="listTdLeft">
            携带积分：
          </td>
          <td>
            <asp:TextBox ID="txtDistirbuteSingleScore" runat="server" CssClass="text"></asp:TextBox>
            <span class="hong">*</span>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server" ErrorMessage="请输入携带积分" ControlToValidate="txtDistirbuteSingleScore" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator ID="RegularExpressionValidator5" runat="server" ErrorMessage="携带积分格式不正确" Display="Dynamic" ControlToValidate="txtDistirbuteSingleScore" ValidationExpression="^[1-9]\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
            <span id="spDistirbuteSingleScore" runat="server"  class="hong"></span>
            </td>
        </tr> 
        <tr>
          <td class="listTdLeft">
            浮动范围：
          </td>
          <td>
            <asp:TextBox ID="txtDistirbuteScoreMinRatio" runat="server" CssClass="text"></asp:TextBox> % - <asp:TextBox ID="txtDistirbuteScoreMaxRatio" runat="server" CssClass="text"></asp:TextBox> %
            <span class="hong">*</span>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator6" runat="server" ErrorMessage="请输入浮动范围下限" ControlToValidate="txtDistirbuteScoreMinRatio" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator ID="RegularExpressionValidator6" runat="server" ErrorMessage="浮动范围下限格式不正确" Display="Dynamic" ControlToValidate="txtDistirbuteScoreMinRatio" ValidationExpression="^[1-9]\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator7" runat="server" ErrorMessage="请输入浮动范围上限" ControlToValidate="txtDistirbuteScoreMaxRatio" Display="Dynamic" ForeColor="Red"></asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator ID="RegularExpressionValidator7" runat="server" ErrorMessage="浮动范围上限格式不正确" Display="Dynamic" ControlToValidate="txtDistirbuteScoreMaxRatio" ValidationExpression="^[1-9]\d*$" ForeColor="Red"></asp:RegularExpressionValidator>
          </td>
        </tr> 
      </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <asp:Button ID="btnSave2" runat="server" Text="保存" CssClass="btn wd1"  OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>