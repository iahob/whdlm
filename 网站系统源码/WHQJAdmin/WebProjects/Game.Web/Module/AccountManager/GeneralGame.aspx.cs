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
    public partial class GeneralGame : AdminPage
    {
        private DataTable gametable;

        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 删除游戏
        /// </summary>
        protected void btnDelete_Click(object sender, EventArgs e)
        {
            //插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "删除通用游戏";
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
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            DataSet ds = FacadeManage.aideAccountsFacade.GetUserLoadGameList(0, 0);
            if (ds != null && ds.Tables.Count > 0)
            {
                gametable = ds.Tables[0];

                rptDataList.DataSource = ds.Tables[1];
                rptDataList.DataBind();
            }
            litNoData.Visible = ds.Tables[1].Rows.Count <= 0;
            rptDataList.Visible = ds.Tables[1].Rows.Count > 0;
        }
    }
}