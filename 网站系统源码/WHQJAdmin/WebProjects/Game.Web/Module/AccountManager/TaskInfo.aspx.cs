using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Web.UI;
using System.Text.RegularExpressions;

using Game.Utils;
using Game.Entity.Accounts;
using Game.Kernel;
using Game.Entity.Enum;
using Game.Facade;
using Game.Entity.Platform;
using System.Data;

namespace Game.Web.Module.AccountManager
{
    public partial class TaskInfo : AdminPage
    {
        private DataSet ds = FacadeManage.aidePlatformFacade.GetKindAndRoomList();
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 104;
            if (!IsPostBack)
            {
                BindDataList();
                BindData();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            Game.Entity.Platform.TaskInfo info = new Game.Entity.Platform.TaskInfo();
            if (IntParam>0)
            {
                AuthUserOperationPermission(Permission.Edit);
                info = FacadeManage.aidePlatformFacade.GetTaskInfo(IntParam);
            }
            else
            {
                AuthUserOperationPermission(Permission.Add);
            }

            info.CollectDate = DateTime.Now;
            info.Innings = CtrlHelper.GetInt(txtInnings,0);
            info.KindID = Convert.ToInt32(ddlKindID.SelectedValue);
            //info.MatchID = Convert.ToInt32(ddlMatchID.SelectedValue);
            info.Nullity = Convert.ToByte(ddlState.SelectedValue);
            info.ServerID = Convert.ToInt32(ddlServerID.SelectedValue);
            info.TaskDescription = CtrlHelper.GetTextAndFilter(txtDescription);
            info.TaskName = CtrlHelper.GetTextAndFilter(txtTaskName);
            info.TaskType = Convert.ToInt32(ddlTaskType.SelectedValue);
            info.TimeType = Convert.ToByte(ddlTimeType.SelectedValue);
            info.UserType = Convert.ToByte(ddlUserType.SelectedValue);
            //info.SortID = CtrlHelper.GetInt(txtSortID, 0);

            int gold = CtrlHelper.GetInt(txtGold, 0);
            int diamond = CtrlHelper.GetInt(txtDiamond, 0);
            int medal = CtrlHelper.GetInt(txtMedal, 0);
            string award = "";
            if (gold > 0)
            {
                award = award + "1:" + gold.ToString() + ";";
            }
            if (diamond > 0)
            {
                award = award + "2:" + diamond.ToString() + ";";
            }
            if (medal > 0)
            {
                award = award + "3:" + medal.ToString() + ";";
            }
            info.TaskAward = award != "" ? award.Substring(0, award.Length - 1) : "";

            int result = 0;
            if (IntParam > 0)
            {
                result = FacadeManage.aidePlatformFacade.UpdateTaskInfo(info);
            }
            else
            {
                result = FacadeManage.aidePlatformFacade.InsertTaskInfo(info);
            }
            if (result > 0)
            {
                ShowInfo("操作成功", "TaskList.aspx", 1200);
            }
            else
            {
                ShowError("操作失败");
            }
        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            Game.Entity.Platform.TaskInfo info = FacadeManage.aidePlatformFacade.GetTaskInfo(IntParam);
            if (info != null)
            {
                txtTaskName.Text = info.TaskName;
                txtDescription.Text = info.TaskDescription;
                txtInnings.Text = info.Innings.ToString();
                ddlKindID.SelectedValue = info.KindID.ToString();
                BindDataRoom();
                //ddlMatchID.SelectedValue = info.MatchID.ToString();
                ddlServerID.SelectedValue = info.ServerID.ToString();
                ddlTaskType.SelectedValue = info.TaskType.ToString();
                ddlTimeType.SelectedValue = info.TimeType.ToString();
                ddlUserType.SelectedValue = info.UserType.ToString();
                ddlState.SelectedValue = info.Nullity.ToString();
                //txtSortID.Text = info.SortID.ToString();
                if (!string.IsNullOrEmpty(info.TaskAward))
                {
                    string[] awards = null;
                    if (info.TaskAward.IndexOf(';') > 0)
                    {
                        string[] arry = info.TaskAward.Split(';');
                        foreach (var item in arry)
                        {
                            awards = item.Split(':');
                            if (awards[0].ToString() == "1")
                            {
                                txtGold.Text = awards[1].ToString();
                            }
                            else if (awards[0].ToString() == "2")
                            {
                                txtDiamond.Text = awards[1].ToString();
                            }
                            else if (awards[0].ToString() == "3")
                            {
                                txtMedal.Text = awards[1].ToString();
                            }
                        }
                    }
                    else
                    {
                        awards = info.TaskAward.Split(':');
                        if (awards[0].ToString() == "1")
                        {
                            txtGold.Text = awards[1].ToString();
                        }
                        else if(awards[0].ToString() == "2")
                        {
                            txtDiamond.Text = awards[1].ToString();
                        }
                        else if(awards[0].ToString() == "3")
                        {
                            txtMedal.Text = awards[1].ToString();
                        }
                    }
                }
            }
        }


        /// <summary>
        /// 绑定房间
        /// </summary>
        protected void ddlKindID_SelectedIndexChanged(object sender, EventArgs e)
        {
            BindDataRoom();
        }

        /// <summary>
        /// 绑定房间
        /// </summary>
        public void BindDataRoom()
        {
            string kindid = ddlKindID.SelectedValue;
            ddlServerID.Items.Clear();
            if (ds != null && ds.Tables.Count > 0 && ds.Tables[1].Rows.Count > 0)
            {
                DataRow[] rows = ds.Tables[1].Select("KindID=" + kindid);
                if (rows != null && rows.Length > 0)
                {
                    foreach (DataRow item in rows)
                    {
                        ddlServerID.Items.Add(new ListItem(rows[0]["ServerName"].ToString(), rows[0]["ServerID"].ToString()));
                    }
                }
            }
            ddlServerID.Items.Insert(0, new ListItem("不限房间", "0"));
            
        }

        /// <summary>
        /// 绑定下拉框
        /// </summary>
        private void BindDataList()
        {
            if (ds != null && ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
            {
                ddlKindID.DataSource = ds.Tables[0];
                ddlKindID.DataTextField = "KindName";
                ddlKindID.DataValueField = "KindID";
                ddlKindID.DataBind();
            }
            ddlKindID.Items.Insert(0, new ListItem("不限游戏", "0"));
            ddlServerID.Items.Insert(0, new ListItem("不限房间", "0"));
        }
    }
}