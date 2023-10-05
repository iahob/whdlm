<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GameSignInInfo.aspx.cs" Inherits="Game.Web.Module.AppManager.GameSignInInfo" %>

<%@ Register Src="/Tools/ImageUploader.ascx" TagName="ImageUploader" TagPrefix="GameImg" %>
<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>
    <script type="text/javascript" src="/scripts/jquery.js"></script>
    <title></title>
    <style type="text/css">
        .auto-style1 {
            height: 21px;
            white-space: nowrap;
            width: 150px;
            text-align: right;
            text-overflow: ellipsis;
            word-break: keep-all;
            overflow: hidden;
            padding: 0px 5px;
        }
        .auto-style2 {
            height: 21px;
        }
    </style>
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
                你当前位置：系统维护 - 签到物品配置
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input id="btnReturn" type="button" value="返回" class="btn wd1" onclick="Redirect('GameSignInList.aspx')" />
                <asp:Button ID="btnCreate" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    签到配置</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                礼包名称：
            </td>
            <td>
                <asp:DropDownList ID="ddlPackageID" runat="server" Width="155" Height="24" CssClass="text">
                  
                </asp:DropDownList>
            </td>
        </tr> 
       
        <tr>
            <td class="listTdLeft">
                签到类型：
            </td>
            <td>
                <asp:DropDownList ID="ddlTypeID" runat="server" Width="155" Height="24" CssClass="text">
                    <asp:ListItem Text="每日签到" Value="0"></asp:ListItem>
                    <asp:ListItem Text="累计签到" Value="1"></asp:ListItem>
                   
                </asp:DropDownList>
            </td>
        </tr>
        
       
        <tr id="Probability">
            <td class="listTdLeft">
                签到抽奖获得礼包的概率：
            </td>
            <td>
                <asp:TextBox ID="txtProbability" runat="server" CssClass="text" validate="{number:true}"></asp:TextBox>%
            </td>
        </tr> 
        <tr id="NeedDay" runat="server">
            <td class="listTdLeft">
                累计签到所需天数：
            </td>
            <td>
                <asp:TextBox ID="txtNeedDay" runat="server" CssClass="text" validate="{number:true}"  Text="1"></asp:TextBox>
            </td>
        </tr> 
         <tr>
            <td class="listTdLeft">
                排序标识：
            </td>
            <td>
                <asp:TextBox ID="txtSortID" runat="server" CssClass="text" validate="{number:true}"></asp:TextBox>
            </td>
        </tr> 
        <%-- <tr>
            <td class="listTdLeft">
                礼包图片：
            </td>
            <td style="line-height:35px;">
                <GameImg:ImageUploader ID="upImage" MaxSize="2097152" runat="server" DeleteButtonClass="l2" DeleteButtonText="删除" Folder="/Upload/Initialize" ViewButtonClass="l2" ViewButtonText="查看" TextBoxClass="text"/> <span>[体积：不大于2M]</span>
            </td>
        </tr>--%>
        <tr>
             <td class="listTdLeft">
                启用禁用：
            </td>
            <td>
                <asp:RadioButtonList ID="rblNullity" runat="server" RepeatDirection="Horizontal">
                    <asp:ListItem Text="启用" Value="0" Selected="True"></asp:ListItem>
                    <asp:ListItem Text="禁用" Value="1"></asp:ListItem>
                </asp:RadioButtonList>
            </td>
        </tr> 
    </table>

    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input id="btnReturn2" type="button" value="返回" class="btn wd1" onclick="Redirect('GamePackageConfigList.aspx')" />
                <asp:Button ID="btnSave2" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
    <script type="text/javascript">
        $(function () {
            if ($('#ddlTypeID').val() == "0") {
                $('#NeedDay').css('display', 'none');
            }

            $('#ddlTypeID').on('change', function () {
                if ($(this).val() == '0') {
                    $('#NeedDay').css('display', 'none');
                    $('#Probability').css('display', 'table-row');
                }
              else  if ($(this).val() == '1') {
                  $('#NeedDay').css('display', 'table-row');
                  $('#Probability').css('display', 'none');
                }
            })
        })

    </script>

</body>
</html>
