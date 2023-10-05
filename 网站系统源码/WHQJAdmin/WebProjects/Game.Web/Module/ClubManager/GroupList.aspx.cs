using System;
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
using Game.Entity.PlatformManager;

namespace Game.Web.Module.ClubManager
{
    public partial class GroupList : AdminPage
    {
        private DataTable wealth;
        private DataTable GroupMaster;

        #region 窗体事件
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 902;
            if(!IsPostBack)
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
       
        /// <summary>
        /// 批量冻结群组
        /// </summary>
        protected void btnDongjie_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量冻结大联盟";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Add);
            int result = FacadeManage.aideGroupFacade.NullityGroup(StrCIdList, 2);
            if(result > 0)
            {
                ShowInfo("冻结成功");
                DBDataBind();
            }
            else
            {
                ShowError("冻结失败");
            }
        }
        /// <summary>
        /// 批量解冻群组
        /// </summary>
        protected void btnJiedong_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量解冻大联盟";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Add);
            int result = FacadeManage.aideGroupFacade.NullityGroup(StrCIdList, 1);
            if(result > 0)
            {
                ShowInfo("解冻成功");
                DBDataBind();
            }
            else
            {
                ShowError("解冻失败");
            }
        }

        /// <summary>
        /// 批量关闭勋章模式
        /// </summary>
        protected void btnCloseMedal_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量关闭勋章模式";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Add);
            int result = FacadeManage.aideGroupFacade.CanUseMedalGroup(StrCIdList, 0);
            if (result > 0)
            {
                ShowInfo("关闭成功");
                DBDataBind();
            }
            else
            {
                ShowError("关闭失败");
            }
        }
        /// <summary>
        /// 批量开启勋章模式
        /// </summary>
        protected void btnOpenMedal_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量开启勋章模式";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Add);
            int result = FacadeManage.aideGroupFacade.CanUseMedalGroup(StrCIdList, 1);
            if (result > 0)
            {
                ShowInfo("开启成功");
                DBDataBind();
            }
            else
            {
                ShowError("开启失败");
            }
        }
        #endregion

        #region 数据绑定
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void DBDataBind()
        {
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            int type = Convert.ToInt32(ddlSearchType.SelectedValue);

            string where = " WHERE 1=1";
            if (!string.IsNullOrEmpty(query))
            {
                where = where + string.Format(" AND {0}={1}", type==1? "CreaterGameID": "GroupID", query);
            }

            PagerSet pagerSet = FacadeManage.aideGroupFacade.GetPageGroupList(where, 
                Orderby, anpPage.CurrentPageIndex, anpPage.PageSize);
            anpPage.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                wealth = pagerSet.PageSet.Tables[1];
                GroupMaster= pagerSet.PageSet.Tables[2];
                rptDataList.DataSource = pagerSet.PageSet.Tables[0];
                rptDataList.DataBind();
            }
            litNoData.Visible = pagerSet.RecordCount <= 0;
            rptDataList.Visible = pagerSet.RecordCount > 0;
        }
        /// <summary>
        /// 获取大联盟信息
        /// </summary>
        protected string GetGroupWealth(int groupid)
        {
            if(wealth!=null&& wealth.Rows.Count > 0)
            {
                DataRow[] rows = wealth.Select("GroupID="+ groupid);
                if (rows != null && rows.Length > 0)
                {
                    //<td>{rows[0]["Ingot"]}</td><td>{rows[0]["ConsumeIngot"]}</td><td>{rows[0]["BattleCreate"]}</td>
                    return $"<td>{rows[0]["Score"]}</td>";
                }
            }
            //<td></td><td></td><td></td>
            return "<td></td>";
        }

        /// <summary>
        /// 获取大联盟主配置
        /// </summary>
        /// <param name="groupid"></param>
        /// <returns></returns>
        protected string GetGroupMasterMember(int groupid)
        {
            if (GroupMaster != null && GroupMaster.Rows.Count > 0)
            {
                DataRow[] rows = GroupMaster.Select($"GroupID={groupid}");
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("<td>{0}%</td>", rows[0]["AwardPoint"]);
                }
               
            }
            return "<td></td>";
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





    }
}