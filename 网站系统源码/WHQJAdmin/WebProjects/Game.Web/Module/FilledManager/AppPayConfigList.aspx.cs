﻿using System;
using Game.Kernel;
using Game.Web.UI;
using Game.Facade;
using Game.Entity.Enum;
using Game.Entity.Treasure;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.FilledManager
{
    public partial class AppPayConfigList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 200;
            if(!IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpNews_PageChanged(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 批量删除配置
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {

            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量删除充值配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);
            //判断权限
            AuthUserOperationPermission(Permission.Delete);
            int result = FacadeManage.aideTreasureFacade.DeleteAppPayConfig(StrCIdList);
            if(result > 0)
            {
                ShowInfo("删除成功");
                BindData();
            }
            else
            {
                ShowInfo("删除失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            PagerSet pagerSet = FacadeManage.aideTreasureFacade.GetList(AppPayConfig.Tablename, SearchItems, Orderby, anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            litNoData.Visible = pagerSet.PageSet.Tables[0].Rows.Count <= 0;
            rptDataList.DataSource = pagerSet.PageSet;
            rptDataList.DataBind();
        }
        /// <summary>
        /// 获取类型
        /// </summary>
        protected string GetTypeName(int typeid)
        {
            return typeid == 1 ? "钻石" : "游戏币";
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
            set
            {
                ViewState["SearchItems"] = value;
            }
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
                    ViewState["Orderby"] = "ORDER BY ConfigID DESC";
                }
                return (string)ViewState["Orderby"];
            }
            set
            {
                ViewState["Orderby"] = value;
            }
        }
    }
}