using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Web.UI;
using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Utils;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.WebManager
{
    public partial class SystemNoticeInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 403;
            if(!IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            SystemNotice notice = new SystemNotice();
            if(IntParam > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改新闻公告";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Edit);
                notice = FacadeManage.aideNativeWebFacade.GetSystemNoticeInfo(IntParam);
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "新增新闻公告";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Add);
                notice.PublisherTime = DateTime.Now;
            }

            notice.MoblieContent = txtMobile.Text;
            //notice.WebContent = txtWeb.Text;
            notice.IsHot = cbHot.Checked;
            notice.IsTop = cbTop.Checked;
            notice.Nullity = cbNullity.Checked;
            notice.Publisher = CtrlHelper.GetTextAndFilter(txtPublisher);
            notice.SortID = CtrlHelper.GetInt(txtSort,1);
            notice.NoticeTitle = CtrlHelper.GetTextAndFilter(txtTitle);
            notice.PlatformType = Convert.ToInt32(Dropplatform.SelectedValue);
            int result = IntParam > 0 ? FacadeManage.aideNativeWebFacade.UpdateSystemNotice(notice) : FacadeManage.aideNativeWebFacade.InsertSystemNotice(notice);
            if(result > 0)
            {
                ShowInfo("新闻公告操作成功", "SystemNoticeList.aspx", 1000);
            }
            else
            {
                ShowError("新闻公告操作失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void BindData()
        {
            if(IntParam > 0)
            {
                SystemNotice notice = FacadeManage.aideNativeWebFacade.GetSystemNoticeInfo(IntParam);
                if(notice != null)
                {
                    CtrlHelper.SetText(txtPublisher, notice.Publisher);
                    CtrlHelper.SetText(txtTitle, notice.NoticeTitle);
                    CtrlHelper.SetText(txtSort, notice.SortID.ToString());
                    txtMobile.Text = notice.MoblieContent;
                    Dropplatform.SelectedValue = notice.PlatformType.ToString();
                    //txtWeb.Text = notice.WebContent;
                    cbHot.Checked = notice.IsHot;
                    cbNullity.Checked = notice.Nullity;
                    cbTop.Checked = notice.IsTop;
                }
            }
        }
    }
}