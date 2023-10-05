using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Kernel;
using System.Text;
using Game.Utils;
using Game.Entity.Accounts;
using Game.Facade;
using Game.Entity.Treasure;
using System.Data;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.AccountManager
{
    public partial class AgentGame : AdminPage
    {
        private DataTable gametable;

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 103;
            if (!IsPostBack)
            {
                litNoData.Visible = true;
                litNoData.Text = "<tr class='tdbg'><td colspan='100' align='center'><br>默认不显示配置信息<br><br></td></tr>";
            }
        }

        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery1_Click(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 删除游戏
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "删除代理游戏";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Delete);
            int result = FacadeManage.aideAccountsFacade.DeleteUserLoadGame(StrCIdList);
            if (result > 0)
            {
                ShowInfo("删除成功");
                BindData();
            }
            else
            {
                ShowError("删除失败");
            }
        }
        /// <summary>
        /// 获取游戏名称
        /// </summary>
        public string GetKindName(int kindid)
        {
            if (gametable != null && gametable.Rows.Count > 0)
            {
                DataRow[] rows = gametable.Select("KindID=" + kindid);
                if (rows != null && rows.Length > 0)
                {
                    return rows[0]["KindName"].ToString();
                }
            }
            return "";
        }
        protected void btnRefush_Click(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            if (string.IsNullOrEmpty(query))
            {
                ShowError("请输入查询代理信息");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(query))
            {
                ShowError("输入查询格式不正确");
                return;
            }
            AccountsInfo info = FacadeManage.aideAccountsFacade.GetAccountInfoByGameId(Convert.ToInt32(query));
            if (info == null)
            {
                ShowError("查询信息不存在");
                return;
            }
            if (info.AgentID <= 0)
            {
                ShowError("查询用户不是代理商");
                return;
            }

            DataSet ds = FacadeManage.aideAccountsFacade.GetUserLoadGameList(info.AgentID, 2);
            if (ds != null && ds.Tables.Count > 0)
            {
                gametable = ds.Tables[0];

                rptDataList.DataSource = ds.Tables[1];
                rptDataList.DataBind();
            }
            litNoData.Visible = ds.Tables[1].Rows.Count <= 0;
            rptDataList.Visible = ds.Tables[1].Rows.Count > 0;
            if (ds.Tables[1].Rows.Count <= 0)
            {
                litNoData.Visible = true;
                litNoData.Text = "<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>";
            }
        }
    }
}