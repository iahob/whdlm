using System;
using Game.Utils;
using Game.Web.UI;
using Game.Kernel;
using Game.Entity.Group;
using Game.Entity.Enum;
using Game.Facade;
using Game.Socket;
using System.Threading;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.ClubManager
{
    public partial class SystemSet : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 902;
            if(!Page.IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            AuthUserOperationPermission(Permission.Edit);

            string OptionName= CtrlHelper.GetText(txtStatusName);
            
            IMGroupOption config = new IMGroupOption();
            config.OptionName = OptionName;
            config.OptionValue = CtrlHelper.GetInt(txtStatusValue, 0);
            config.OptionTip = CtrlHelper.GetTextAndFilter(txtStatusTip);
            config.OptionDescribe = CtrlHelper.GetTextAndFilter(txtStatusDescription);

            int result = FacadeManage.aideGroupFacade.UpdateGroupOption(config);
            if(result > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = $"修改大联盟配置，操作项：【{OptionName}】";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                if (OptionName == "MaxMemberCount")
                {
                    //int count = CtrlHelper.GetInt(txtStatusValue, 0);
                    //int res = FacadeManage.aideGroupFacade.ModifyMaxMemberCount("", count);
                    //SocketManage.aideWebSocketFacade.UpdateSuccessARE = new AutoResetEvent(false);
                    //SocketManage.aideWebSocketFacade.UpdateGroupMaxMember(0, Convert.ToInt16(count), isSent => { });
                    //SocketManage.aideWebSocketFacade.UpdateSuccessARE.WaitOne();

                }
                ShowInfo("修改成功");
            }
            else
            {
                ShowError("修改失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            PagerSet pagerSet = FacadeManage.aideGroupFacade.GetList(IMGroupOption.Tablename , SearchItems, Orderby,1, 100);
            rptDataList.DataSource = pagerSet.PageSet;
            rptDataList.DataBind();

            IMGroupOption config = FacadeManage.aideGroupFacade.GetGroupOption(string.IsNullOrEmpty(StrParam) ? "CreateConfigDeductIngot" : StrParam);
            if (config == null) return;
            CtrlHelper.SetText(txtStatusName, config.OptionName);
            CtrlHelper.SetText(txtStatusValue, config.OptionValue.ToString());
            CtrlHelper.SetText(txtStatusTip, config.OptionTip);
            CtrlHelper.SetText(txtStatusDescription, config.OptionDescribe);
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
                    ViewState["SearchItems"] = "WHERE 1=1 and OptionName<>'GroupRoomType'";
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
                    ViewState["Orderby"] = "ORDER BY SortID ASC";
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
