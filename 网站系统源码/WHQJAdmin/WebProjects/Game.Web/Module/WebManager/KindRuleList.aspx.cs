using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Text;

using Game.Web.UI;
using Game.Kernel;
using Game.Utils;
using Game.Entity.Enum;
using Game.Facade;
using Game.Entity.NativeWeb;
using Game.Entity.Platform;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.WebManager
{
    public partial class KindRuleList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 401;
            if(!IsPostBack)
            {
                RPDataBind();
                BindGame();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            RPDataBind();
        }
        /// <summary>
        /// 批量删除规则
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量删除游戏规则";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Delete);
            int result = FacadeManage.aideNativeWebFacade.DeleteGameRule(StrCIdList);
            if(result > 0)
            {
                ShowInfo("删除成功");
                RPDataBind();
            }
            else
            {
                ShowError("删除失败");
            }
        }
        /// <summary>
        /// 批量禁用规则
        /// </summary>
        protected void btnDongjie_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量禁用游戏规则";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Enable);
            int result = FacadeManage.aideNativeWebFacade.NullityGameRule(StrCIdList, 1);
            if(result > 0)
            {
                ShowInfo("禁用成功");
                RPDataBind();
            }
            else
            {
                ShowError("禁用失败");
            }
        }
        /// <summary>
        /// 批量启用规则
        /// </summary>
        protected void btnJieDong_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量启用游戏规则";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Enable);
            int result = FacadeManage.aideNativeWebFacade.NullityGameRule(StrCIdList, 0);
            if(result > 0)
            {
                ShowInfo("启用成功");
                RPDataBind();
            }
            else
            {
                ShowError("启用失败");
            }
        }
        /// <summary>
        /// 设置查询条件
        /// </summary>
        private void SetCondition()
        {
            int kindid = Convert.ToInt32(ddlGame.SelectedValue);
            int Type = Convert.ToInt32(ddlType.SelectedValue);
            StringBuilder condition = new StringBuilder("WHERE 1=1");
            if (kindid > 0&& Type>0)
            {
                condition.AppendFormat(" AND KindID={0} and Type={1}", kindid, Type);
            }
            else if(Type > 0)
            {
                condition.AppendFormat(" AND  Type={0}",Type);
            }
            else if (kindid > 0)
            {
                condition.AppendFormat(" AND  KindID={0}", kindid);
            }
            ViewState["SearchItems"] = condition.ToString();
        }

        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            SetCondition();
            RPDataBind();
        }
        /// <summary>
        /// 绑定游戏列表
        /// </summary>
        protected void BindGame()
        {
            PagerSet ps = FacadeManage.aidePlatformFacade.GetList(GameGameItem.Tablename, "", "ORDER BY GameID DESC", 1, 1000);
            ddlGame.DataSource = ps.PageSet;
            ddlGame.DataTextField = "GameName";
            ddlGame.DataValueField = "GameID";
            ddlGame.DataBind();
            ddlGame.Items.Insert(0, new ListItem("选择游戏", "0"));
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void RPDataBind()
        {
            PagerSet pagerSet = FacadeManage.aideNativeWebFacade.GetList(GameRule.Tablename, SearchItems, Orderby, anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            litNoData.Visible = pagerSet.PageSet.Tables[0].Rows.Count <= 0;
            rptIssue.DataSource = pagerSet.PageSet;
            rptIssue.DataBind();

        }
        /// <summary>
        /// 查询条件
        /// </summary>
        public string SearchItems
        {
            get
            {
                if(ViewState["SearchItems"] == null)
                {
                    ViewState["SearchItems"] = "WHERE 1=1";
                }
                return (string)ViewState["SearchItems"];
            }
            set { ViewState["SearchItems"] = value; }
        }
        /// <summary>
        /// 排序条件
        /// </summary>
        public string Orderby
        {
            get
            {
                if(ViewState["Orderby"] == null)
                {
                    ViewState["Orderby"] = "ORDER BY SortID ASC,KindID DESC";
                }
                return (string)ViewState["Orderby"];
            }
            set { ViewState["Orderby"] = value; }
        }
    }
}