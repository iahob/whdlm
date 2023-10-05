using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data;
using System.Text;

using Game.Web.UI;
using Game.Utils;
using Game.Entity.Platform;
using Game.Kernel;
using Game.Facade;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AppManager
{
    public partial class SystemMessageInfo : AdminPage
    {
        private DataSet ds = FacadeManage.aidePlatformFacade.GetKindAndRoomList();
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 302;
            if(!Page.IsPostBack)
            {
                SystemMessageDataBind();
                KindsDataBind();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            SystemMessage systemMessage = new SystemMessage();
            if(IntParam > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改系统消息";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Edit);
                systemMessage = FacadeManage.aidePlatformFacade.GetSystemMessageInfo(IntParam);
                systemMessage.UpdateMasterID = userExt.UserID;
                systemMessage.UpdateDate = DateTime.Now;
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "新增系统消息";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Add);
                systemMessage.CreateMasterID = userExt.UserID;
                systemMessage.CreateDate = DateTime.Now;
            }

            systemMessage.MessageString = CtrlHelper.GetTextAndFilter(txtMessageString);
            systemMessage.MessageType = Convert.ToInt32(ddlMessageType.SelectedValue.Trim());
            systemMessage.StartTime = Convert.ToDateTime(CtrlHelper.GetText(txtStartTime));
            systemMessage.ConcludeTime = Convert.ToDateTime(CtrlHelper.GetText(txtConcludeTime));
            systemMessage.TimeRate = CtrlHelper.GetInt(txtTimeRate, 0);
            systemMessage.CollectNote = CtrlHelper.GetTextAndFilter(txtCollectNote);

            StringBuilder sb = new StringBuilder();
            if(!tvKinds.Nodes[0].Checked)
            {
                foreach(TreeNode node in tvKinds.CheckedNodes)
                {
                    if(node.Value != "")
                    {
                        sb.Append(node.Value.ToString() + ",");
                    }
                }
            }
            else
            {
                sb.Length = 0;
                sb.Append("0");
            }

            if(sb.ToString() == "" || sb.ToString() == null)
            {
                ShowError("请选择要控制的房间！");
                return;
            }
            systemMessage.ServerRange = sb.ToString();
            int result = IntParam > 0 ? FacadeManage.aidePlatformFacade.UpdateSystemMessage(systemMessage) : FacadeManage.aidePlatformFacade.InsertSystemMessage(systemMessage);
            if(result > 0)
            {
                ShowInfo("系统消息操作成功", "SystemMessageList.aspx", 1200);
            }
            else
            {
                ShowError("系统消息操作失败");
            }
        }
        /// <summary>
        /// 游戏信息列表
        /// </summary>
        private void KindsDataBind()
        {
            tvKinds.Nodes.Clear();

            TreeNode rootNode = new TreeNode();
            rootNode.Text = "游戏列表";
            rootNode.Value = "-1";
            rootNode.SelectAction = TreeNodeSelectAction.None;
            rootNode.Expand();
            rootNode.ShowCheckBox = true;
            tvKinds.Nodes.Add(rootNode);

            if(Servers != "0")
            {
                string[] servers = Servers.Split(',');
                DataTable gametable = ds!=null&&ds.Tables.Count>0 ? ds.Tables[0] : null;
                DataTable roomtable = ds != null && ds.Tables.Count > 0 ? ds.Tables[1] : null;
                if (gametable!=null && gametable.Rows.Count > 0)
                {
                    foreach (DataRow dr in gametable.Rows)
                    {
                        TreeNode nodeGame = new TreeNode();
                        nodeGame.Text = dr["KindName"].ToString();
                        nodeGame.Value = "";
                        nodeGame.ShowCheckBox = true;
                        nodeGame.SelectAction = TreeNodeSelectAction.None;
                        rootNode.ChildNodes.Add(nodeGame);

                        if(roomtable!=null&& roomtable.Rows.Count > 0)
                        {
                            DataRow[] rows = roomtable.Select("KindID=" + dr["KindID"]);
                            if (rows == null || rows.Length <= 0)
                                continue;

                            int j = 0;
                            foreach (DataRow drServer in rows)
                            {
                                TreeNode nodeServer = new TreeNode();
                                nodeServer.Text = drServer["ServerName"].ToString();
                                nodeServer.Value = drServer["ServerID"].ToString().Trim();
                                nodeServer.ShowCheckBox = true;
                                nodeServer.SelectAction = TreeNodeSelectAction.None;

                                for (int i = 0; i < servers.Length; i++)
                                {
                                    if (servers[i].Trim() == "") continue;
                                    if (servers[i].Trim() == drServer["ServerID"].ToString().Trim())
                                    {
                                        nodeServer.Checked = true;
                                        j++;
                                        break;
                                    }
                                }
                                nodeGame.ChildNodes.Add(nodeServer);
                            }
                            if (j == rows.Length && j != 0) nodeGame.Checked = true;
                        }
                    }
                }
            }
            else
            {
                BindKindAndServer(rootNode);
                tvKinds.Nodes[0].Checked = true;
            }
        }

        /// <summary>
        /// 绑定游戏和房间数据
        /// </summary>
        private void BindKindAndServer(TreeNode rootNode)
        {
            DataTable gametable = ds != null && ds.Tables.Count > 0 ? ds.Tables[0] : null;
            DataTable roomtable = ds != null && ds.Tables.Count > 0 ? ds.Tables[1] : null;
            if (gametable != null && gametable.Rows.Count > 0)
            {
                foreach (DataRow dr in gametable.Rows)
                {
                    TreeNode nodeGame = new TreeNode();
                    nodeGame.Text = dr["KindName"].ToString();
                    nodeGame.Value = "";
                    nodeGame.ShowCheckBox = true;
                    nodeGame.SelectAction = TreeNodeSelectAction.None;
                    rootNode.ChildNodes.Add(nodeGame);

                    if (roomtable != null && roomtable.Rows.Count > 0)
                    {
                        DataRow[] rows = roomtable.Select("KindID=" + dr["KindID"]);
                        if (rows == null || rows.Length <= 0)
                            continue;

                        foreach (DataRow drServer in rows)
                        {
                            TreeNode nodeServer = new TreeNode();
                            nodeServer.Text = drServer["ServerName"].ToString();
                            nodeServer.Value = drServer["ServerID"].ToString().Trim();
                            nodeServer.ShowCheckBox = true;
                            nodeServer.SelectAction = TreeNodeSelectAction.None;

                            nodeGame.ChildNodes.Add(nodeServer);
                        }
                    }
                }
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void SystemMessageDataBind()
        {
            if(IntParam > 0)
            {
                SystemMessage systemMessage = FacadeManage.aidePlatformFacade.GetSystemMessageInfo(IntParam);
                if(systemMessage != null)
                {
                    CtrlHelper.SetText(txtMessageString, systemMessage.MessageString.Trim());
                    ddlMessageType.SelectedValue = systemMessage.MessageType.ToString().Trim();
                    CtrlHelper.SetText(txtStartTime, systemMessage.StartTime.ToString("yyyy-MM-dd HH:mm:ss").Trim());
                    CtrlHelper.SetText(txtConcludeTime, systemMessage.ConcludeTime.ToString("yyyy-MM-dd HH:mm:ss").Trim());
                    CtrlHelper.SetText(txtTimeRate, systemMessage.TimeRate.ToString().Trim());
                    CtrlHelper.SetText(txtCollectNote, systemMessage.CollectNote.Trim());
                    Servers = systemMessage.ServerRange;
                }
            }
            else
            {
                CtrlHelper.SetText(txtStartTime, DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));
                CtrlHelper.SetText(txtConcludeTime, DateTime.Now.AddDays(10).ToString("yyyy-MM-dd HH:mm:ss"));
                CtrlHelper.SetText(txtTimeRate, "60");
            }
        }
        /// <summary>
        /// 房间范围记录
        /// </summary>
        public string Servers
        {
            get
            {
                if(ViewState["Server"] == null)
                    ViewState["Server"] = "";
                return ViewState["Server"].ToString();
            }
            set
            {
                ViewState["Server"] = value;
            }
        }
    }
}
