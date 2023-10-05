using System;
using System.Collections.Generic;
using System.Data;
using Game.Web.UI;
using Game.Facade;
using Game.Utils;

namespace Game.Web.Module.ClubManager
{
    public partial class GroupMember : AdminPage
    {
        private DataTable usertable;
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 902;
            if (!IsPostBack)
            {
                string gid = GameRequest.GetQueryString("gid");
                DataSet ds = FacadeManage.aideGroupFacade.GetGroupMemberList(Convert.ToInt64(gid));
                if (ds != null && ds.Tables[0].Rows.Count > 0)
                {
                    usertable = ds.Tables[1];
                    rptDataList.DataSource = ds.Tables[0];
                    rptDataList.DataBind();
                }
            }
        }

        /// <summary>
        /// 获取用户信息
        /// </summary>
        protected string GetUserInfo(int userid)
        {
            if(usertable!=null&& usertable.Rows.Count > 0)
            {
                DataRow[] rows = usertable.Select("UserID=" + userid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("<td>{0}</td><td>{1}</td>",
                        rows[0]["GameID"], rows[0]["NickName"]);
                }
            }
            return "<td></td><td></td>";
        }
    }
}