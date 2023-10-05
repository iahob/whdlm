using System;
using System.Web.UI.WebControls;
using Game.Entity.Accounts;
using Game.Entity.Enum;
using Game.Web.UI;
using Game.Facade;
using Game.Utils;
using Game.Kernel;
using Game.Entity.Treasure;
using Game.Entity.Platform;
using System.Data;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AccountManager
{
    public partial class UserPlaying : AdminPage
    {
        private DataSet KindRoomData = Fetch.GetKindAndRoom();
        private DataTable UserData;

        #region 事件

        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 101;
            AuthUserOperationPermission(Permission.Read);
            if (IsPostBack) return;
            BindServerList();
            BindingData();
        }

        protected void btnQueryOrder_Click(object sender, EventArgs e)
        {
            BindingData();
        }

        protected void btnCleanLocker_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "清除卡线";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission(Permission.Add);
            if (StrCIdList.Length == 0)
            {
                ShowError("请选择需要清除卡线的玩家");
                return;
            }

            int result = FacadeManage.aideTreasureFacade.CleanGameScoreLocker(StrCIdList);
            if (result > 0)
            {
                ShowInfo("清除卡线成功");
                BindingData();
            }
            else
            {
                ShowError("清除卡线失败");
            }
        }

        protected void anpPage_PageChanged(object sender, EventArgs e)
        {
            BindingData();
        }

        #endregion

        #region 数据方法

        protected void BindingData()
        {
            string type = ddlSearchType.SelectedValue;
            string search = CtrlHelper.GetTextAndFilter(txtSearch);
            string where = string.Empty;

            if (!string.IsNullOrEmpty(search))
            {
                if (!Utils.Validate.IsPositiveInt(search))
                {
                    ShowError("请输入正确的查询格式");
                    return;
                }
                UserInfo info = FacadeManage.aideAccountsFacade.GetUserInfo(0, Convert.ToInt32(search));
                if (info == null)
                {
                    ShowError("输入的查询信息不存在");
                    return;
                }
                where = " WHERE UserID=" + info.UserID.ToString();
            }
            int serverId = Convert.ToInt32(ddlServerID.SelectedValue);
            if (serverId > 0)
            {
                where = string.IsNullOrEmpty(where) ? $" WHERE ServerID={serverId}" : (where + $" AND ServerID={serverId}");
            }

            PagerSet pagerSet = FacadeManage.aideTreasureFacade.GetPageScoreList(where, " ORDER BY UserID ASC", anpPage.CurrentPageIndex, anpPage.PageSize);
            anpPage.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                UserData = pagerSet.PageSet.Tables[1];
                rptData.DataSource = pagerSet.PageSet.Tables[0];
                rptData.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptData.Visible = pagerSet.RecordCount > 0;
        }

        //绑定房间
        private void BindServerList()
        {
            if (KindRoomData != null && KindRoomData.Tables.Count > 0)
            {
                DataTable table = KindRoomData.Tables[1];
                if (table != null && table.Rows.Count > 0)
                {
                    for (int i = 0; i < table.Rows.Count; i++)
                    {
                        if (table.Rows[i]["Nullity"].ToString() == "0")
                        {
                            ddlServerID.Items.Add(new ListItem(table.Rows[i]["ServerName"].ToString(), table.Rows[i]["ServerID"].ToString()));
                        }
                    }
                }
            }
            ddlServerID.Items.Insert(0, new ListItem("全部房间", "0"));
        }

        #endregion

        #region 公开方法

        /// <summary>
        /// 获取用户信息转为Table td
        /// </summary>
        /// <param name="userId"></param>
        /// <returns></returns>
        public string GetAccountsInfo(int userId)
        {
            if (UserData != null)
            {
                DataRow[] row = UserData.Select("UserID=" + userId);
                if (row != null && row.Length > 0)
                {
                    return string.Format("<td>{0}</td><td>{1}</td>",
                         row[0]["NickName"], row[0]["GameID"]);
                }
            }
            return "<td></td><td></td>";
        }

        /// <summary>
        /// 通过游戏房间获取Table td
        /// </summary>
        /// <param name="serverId"></param>
        /// <returns></returns>
        public string GetGameRoomInfo(int kindid,int serverId)
        {
            if(KindRoomData!=null&& KindRoomData.Tables.Count > 0)
            {
                DataTable kind = KindRoomData.Tables[0];
                DataTable room = KindRoomData.Tables[1];

                string info = string.Empty;
                DataRow[] rows = kind.Select("KindID=" + kindid);
                info = rows != null && rows.Length > 0 ? string.Format("<td>{0}</td>", rows[0]["KindName"]) : "<td></td>";
                rows = room.Select("ServerID=" + serverId);
                info = info + (rows != null && rows.Length > 0 ? string.Format("<td>{0}</td>", rows[0]["ServerName"]) : "<td></td>");
                return info;
            }
            return "<td></td><td></td>";
        }

        #endregion
    }
}
