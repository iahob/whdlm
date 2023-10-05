using System;
using System.Collections.Generic;
using System.Data;
using System.Text;
using Game.Entity.Accounts;
using Game.Entity.Agent;
using Game.Entity.Enum;
using Game.Entity.Group;
using Game.Facade;
using Game.Kernel;
using Game.Utils;
using Game.Web.UI;
using Game.Entity.Treasure;
using Game.Socket;
using System.Threading;
using Game.Socket.DataStruct;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.DebugManager
{
    public partial class GroupDebugUserList : AdminPage
    {
        private DataTable GroupMaster;
        private DataSet KindRoomData = Fetch.GetKindAndRoom();


        #region 窗体事件
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {

            base.moduleID = 902;
            if (!IsPostBack)
            {
                DBDataBind();
            }
            
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged(object sender, EventArgs e)
        {
            DBDataBind();
        }
        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            DBDataBind();
        }

        #endregion

        #region 数据绑定
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void DBDataBind()
        {
            
            int GroupID = GameRequest.GetInt("groupId", 0);
            if (GroupID <= 0)
            {
                MessageBox("参数错误");
                return;
            }

            string where = $" WHERE igm.GroupID={GroupID}";

            DataTable grouptb = FacadeManage.aideGroupFacade.GetGroupDebugInfo(GroupID);
            if (grouptb.Rows.Count > 0)
            {
                rptGroupData.DataSource = grouptb;
                rptGroupData.DataBind();
            }

            PagerSet pagerSet = FacadeManage.aideGroupFacade.GetPageGroupDebugUserList(where,
                Orderby, anpPage.CurrentPageIndex, anpPage.PageSize);
            anpPage.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                rptDataList.DataSource = pagerSet.PageSet.Tables[0];
                rptDataList.DataBind();

            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptDataList.Visible = pagerSet.RecordCount > 0;
        }


        #endregion

        #region 公共属性
        /// <summary>
        /// 排序条件
        /// </summary>
        public string Orderby
        {
            get
            {
                if (ViewState["Orderby"] == null)
                {
                    ViewState["Orderby"] = "ORDER BY GroupID DESC";
                }
                return (string)ViewState["Orderby"];
            }
            set
            {
                ViewState["Orderby"] = value;
            }
        }

        #endregion

        #region 公开方法
        /// <summary>
        /// 通过游戏房间获取Table td
        /// </summary>
        /// <param name="serverId"></param>
        /// <returns></returns>
        public string GetOnlineGameRoomInfo(int groupId,int userStatus,int userId)
        {
            GameScoreLocker onlieUser = FacadeManage.aideTreasureFacade.GetOnlineUser(userId);

            int RoomID = 0;
            if (KindRoomData != null && KindRoomData.Tables.Count > 0 && onlieUser !=null)
            {
                DataTable kind = KindRoomData.Tables[0];
                DataTable room = KindRoomData.Tables[1];

                string info = string.Empty;
                DataRow[] rows = kind.Select("KindID=" + onlieUser.KindID);
                info = rows != null && rows.Length > 0 ? string.Format("<td>{0}</td>", rows[0]["KindName"]) : "<td></td>";

                RoomID = FacadeManage.aidePlatformFacade.GetOnlineUeserRoomID(groupId, userId);
                info = info + (RoomID > 0 ? string.Format("<td>{0}</td>", RoomID) : "<td></td>");
                return info;
            }
            else if (userStatus == 1)
            {
                return "<td>大厅</td><td></td>";
            }

            return "<td></td><td></td>";
        }

        /// <summary>
        /// 获取当前时间在线权重
        /// </summary>
        /// <param name="TimeCount"></param>
        /// <returns></returns>
        public int GetOnlineWeightOut(double TimeCount)
        {
            if (TimeCount > 0)
            {
                return 0;
            }
            return FacadeManage.aidePlatformFacade.GetAiWeightOutRatioByTime(TimeCount);
        }


        /// <summary>
        /// 获取当前玩家状态
        /// </summary>
        /// <param name="userStatus"></param>
        /// <returns></returns>
        public string GetUserStatus(int userStatus,int userId)
        {
            string str = "未知";
            if (userStatus == 0)
            {
                str = "离线";
            }
            else if (userStatus == 1)
            {
                str = "在线";
                GameScoreLocker onlieUser = FacadeManage.aideTreasureFacade.GetOnlineUser(userId);
                if (onlieUser != null)
                {
                    str = "游戏中";
                }
            }
            return str;
        }

        /// <summary>
        /// 获取在线时长
        /// </summary>
        /// <param name="StartTime"></param>
        /// <param name="EndTime"></param>
        /// <returns></returns>
        public string GetUserOnlineTime(int userid,DateTime StartTime, DateTime EndTime)
        {

            double LastMinutes = Fetch.GetTimeIntervalMinutes(StartTime, EndTime);
            if (LastMinutes > 0)
            {
                return "0秒";
            }

            return Fetch.GetTimeSpan(StartTime, DateTime.Now);
        }


        #endregion

        /// <summary>
        /// 强制下线
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnForceDown_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "强制下线";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            int GroupID = GameRequest.GetInt("groupId", 0);
            if (GroupID <= 0)
            {
                MessageBox("参数错误");
                return;
            }

            if (!string.IsNullOrEmpty(StrCIdList))
            {
                string[] userlist = StrCIdList.Split(',');
                if (userlist.Length > 0)
                {

                    //向服务器发送消息
                    SocketManage.aideWebSocketFacade.OperateResultARE = new AutoResetEvent(false);
                    SocketManage.aideWebSocketFacade.DelteAI((byte) DelteAIType.强制下线, GroupID, userlist, isSent => { });
                    SocketManage.aideWebSocketFacade.OperateResultARE.WaitOne();

                    if (SocketManage.aideWebSocketFacade.Code == 0)
                    {
                        ShowInfo("操作成功");
                    }
                    else
                    {
                        ShowError(SocketManage.aideWebSocketFacade.Message);
                    }
                }
            }
        }

        /// <summary>
        /// 强制退出
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnForceOut_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "强制退出";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            int GroupID = GameRequest.GetInt("groupId", 0);
            if (GroupID <= 0)
            {
                MessageBox("参数错误");
                return;
            }

            if (!string.IsNullOrEmpty(StrCIdList))
            {
                string[] userlist = StrCIdList.Split(',');
                if (userlist.Length > 0)
                {

                    //向服务器发送消息
                    SocketManage.aideWebSocketFacade.OperateResultARE = new AutoResetEvent(false);
                    SocketManage.aideWebSocketFacade.DelteAI((byte)DelteAIType.强制退出, GroupID, userlist, isSent => { });
                    SocketManage.aideWebSocketFacade.OperateResultARE.WaitOne();

                    if (SocketManage.aideWebSocketFacade.Code == 0)
                    {
                        ShowInfo("操作成功");
                    }
                    else
                    {
                        ShowError(SocketManage.aideWebSocketFacade.Message);
                    }
                }
            }
        }

        protected void btnRefresh_Click(object sender, EventArgs e)
        {
            DBDataBind();
        }
    }
}