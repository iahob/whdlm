﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Utils;
using Game.Web.UI;
using Game.Kernel;
using Game.Facade;
using System.Data;
using Game.Entity.Enum;
using Game.Entity.NativeWeb;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.WebManager
{
    public partial class AdsList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 402;
            if(!IsPostBack)
            {
                BindAds();
            }
        }
        /// <summary>
        /// 批量删除广告
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "批量删除广告";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission(Permission.Delete);
            int result = FacadeManage.aideNativeWebFacade.DeleteAds(StrCIdList);
            if(result > 0)
            {
                ShowInfo("删除成功");
                BindAds();
            }
            else
            {
                ShowError("删除失败");
            }
        }
        /// <summary>
        /// 获取广告类型
        /// </summary>
        /// <param name="typeid"></param>
        /// <returns></returns>
        protected string GetAdsType(int typeid)
        {
            switch(typeid)
            {
                case 0:
                    return "网站首页轮换图";
                case 1:
                    return "新闻公告广告图";
                case 2:
                    return "联系我们广告图";
                case 3:
                    return "H5竖版弹出广告";
                case 4:
                    return "手机横版弹出广告";
                case 5:
                    return "游戏下载广告图";
                case 6:
                    return "活动广告";
                default:
                    return "";
            }
        }
        /// <summary>
        /// 绑定数据
        /// </summary>
        private void BindAds()
        {
            //控件绑定
            PagerSet pagerSet = FacadeManage.aideNativeWebFacade.GetList(Ads.Tablename, "", "ORDER BY [Type] DESC,SortID ASC", 1, 100);
            litNoData.Visible = pagerSet.PageSet.Tables[0].Rows.Count > 0 ? false : true;
            rpData.DataSource = pagerSet.PageSet;
            rpData.DataBind();
        }
    }
}