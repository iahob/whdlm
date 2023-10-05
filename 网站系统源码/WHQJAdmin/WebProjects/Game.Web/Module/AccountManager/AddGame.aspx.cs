using Game.Entity.Accounts;
using Game.Entity.Enum;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using Game.Web.UI;
using System;
using Game.Entity.Platform;
using System.Collections.Generic;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AccountManager
{
    public partial class AddGame : AdminPage
    {
        protected int TypeID = GameRequest.GetQueryInt("typeid", 0);

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 103;
            if (!IsPostBack)
            {
                AuthUserOperationPermission(Permission.Add);

                IList<GameKindItem> list = FacadeManage.aidePlatformFacade.GetGameList();
                if (list != null && list.Count > 0)
                {
                    //ddlKind.DataSource = list;
                    //ddlKind.DataTextField = "KindName";
                    //ddlKind.DataValueField = "KindID";
                    //ddlKind.DataBind();

                    CheckBoxList1.DataSource = list;
                    CheckBoxList1.DataTextField = "KindName";
                    CheckBoxList1.DataValueField = "KindID";
                    CheckBoxList1.DataBind();
                }

                if (TypeID <= 0)
                {
                    GID.Visible = false;
                    GNAME.Visible = false;
                }
                else
                {
                    GID.Visible = true;
                    GNAME.Visible = true;
                }
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {

            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "添加游戏";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission(Permission.Add);
            string gameid = CtrlHelper.GetTextAndFilter(txtGameID);
            string nickname = CtrlHelper.GetTextAndFilter(txtNickName);
            int sortid = CtrlHelper.GetInt(txtSort, 0);
            //int kindid = Convert.ToInt32(ddlKind.SelectedValue);
            int userid = Convert.ToInt32(hdfUserID.Value);
            int agentid = Convert.ToInt32(hdfAgentID.Value);

            if (TypeID > 0)
            {
                if (!string.IsNullOrEmpty(gameid))
                {
                    if (!Utils.Validate.IsPositiveInt(gameid))
                    {
                        MessageBox("输入查询格式不正确");
                        return;
                    }
                    if (userid<=0&& agentid<=0)
                    {
                        MessageBox("输入ID后请检测账号");
                        return;
                    }
                    if(TypeID==1&& userid <= 0)
                    {
                        MessageBox("输入的账号信息不存在");
                        return;
                    }
                    if(TypeID == 2 && userid>0 && agentid <= 0)
                    {
                        MessageBox("输入的账号为非代理账号");
                        return;
                    }
                }
                else
                {
                    MessageBox("请输入要添加的账号信息");
                    return;
                }
            }
            Message msg = new Message();
            for (int i = 0; i < CheckBoxList1.Items.Count; i++)
            {
                if (CheckBoxList1.Items[i].Selected)
                {
                    AccountsGame game = new AccountsGame();
                    game.TypeID = (byte)TypeID;
                    game.KindID =Convert.ToInt32(CheckBoxList1.Items[i].Value);
                    game.SortID = sortid;
                    game.UniteID = TypeID == 1 ? userid : TypeID == 2 ? agentid : 0;

                    msg = FacadeManage.aideAccountsFacade.AddUserLoadGame(game);
                }
               
            }
           

            if (msg.Success)
            {
                MessageBox("添加配置成功");
            }
            else
            {
                MessageBox("添加配置信息已存在");
            }
        }
    }
}