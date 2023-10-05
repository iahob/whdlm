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
using Game.Entity.Platform;
using System.Data;

namespace Game.Web.Module.Diamond
{
    public partial class RecordScoreBattleInfo : AdminPage
    {
        protected string TableData;
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 604;
            if (!IsPostBack)
            {
                int roomid = GameRequest.GetQueryInt("rid", 0);
                int mode = GameRequest.GetQueryInt("mode", 0);
                int id = GameRequest.GetQueryInt("uid", 0);
                string time = GameRequest.GetQueryString("time");

                DateTime stime = Convert.ToDateTime(time).AddMinutes(-10);
                DateTime etime = stime.AddHours(3);

                DataSet ds = FacadeManage.aidePlatformFacade.GetBattleInfoByRoom(roomid,
                    stime.ToString("yyyy-MM-dd HH:mm:ss"), etime.ToString("yyyy-MM-dd HH:mm:ss"), mode, id);

                StringBuilder sb = new StringBuilder();
                sb.Append("<table width=\"100%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" class=\"box\" id=\"list\">");
                if (ds != null && ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
                {
                    DataTable battle = ds.Tables[0];
                    DataTable user = ds.Tables[1];
                    DataTable gamenum = ds.Tables[2];
                    DataTable battleinfo = ds.Tables[3];
                    //表格头信息
                    sb.Append("<tr align=\"center\" class=\"bold\">");
                    sb.Append("<td class=\"listTitle2\">局数</td>");
                    foreach (DataRow uinfo in user.Rows)
                    {
                        sb.AppendFormat("<td class=\"listTitle2\">{0}</td>", GetUserInfo(Convert.ToInt32(uinfo["UserID"]), user));
                    }
                    sb.Append("</tr>");
                    //表格详情
                    foreach (DataRow num in gamenum.Rows)
                    {
                        sb.Append("<tr align=\"center\" class=\"list\">");
                        sb.AppendFormat("<td>第{0}局</td>", num["GamesNum"]);
                        DataRow[] rows = battleinfo.Select("GamesNum=" + num["GamesNum"]);
                        foreach (DataRow uinfo in user.Rows)
                        {
                            foreach (DataRow binfo in rows)
                            {
                                if (uinfo["UserID"].ToString().Equals(binfo["UserID"].ToString()))
                                {
                                    sb.AppendFormat("<td>{0}</td>", binfo["Score"]);
                                    break;
                                }
                            }
                        }
                        sb.Append("</tr>");
                    }
                    //表格合计
                    sb.Append("<tr align=\"center\" class=\"list\">");
                    sb.Append("<td>合计：</td>");
                    foreach (DataRow uinfo in user.Rows)
                    {
                        foreach (DataRow binfo in battle.Rows)
                        {
                            if (uinfo["UserID"].ToString().Equals(binfo["UserID"].ToString()))
                            {
                                sb.AppendFormat("<td>{0}</td>", binfo["Score"]);
                                break;
                            }
                        }
                    }
                    sb.Append("</tr>");
                }
                else
                {
                    sb.Append("<tr class='tdbg'><td colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>");
                }
                sb.Append("</table>");
                TableData = sb.ToString();
            }
        }
        /// <summary>
        /// 获取用户信息
        /// </summary>
        protected string GetUserInfo(int userid, DataTable usertable)
        {
            if (usertable != null && usertable.Rows.Count > 0)
            {
                DataRow[] rows = usertable.Select("UserID=" + userid);
                if (rows != null && rows.Length > 0)
                {
                    return string.Format("{0}<br/>{1}", rows[0]["NickName"], rows[0]["GameID"]);
                }
            }
            return "";
        }
    }
}