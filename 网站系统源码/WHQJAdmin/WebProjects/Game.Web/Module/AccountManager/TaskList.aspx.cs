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
using Game.Entity.Enum;
using Game.Facade;
using Game.Entity.Accounts;

namespace Game.Web.Module.AccountManager
{
    public partial class TaskList : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 104;
            if (!IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 数据查询
        /// </summary>
        protected void btnQuery_Click(object sender, EventArgs e)
        {
            BindData();
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            string query = CtrlHelper.GetTextAndFilter(txtSearch);
            string where = " WHERE 1=1";
            if (!string.IsNullOrEmpty(query))
            {
                where = string.Format(" WHERE TaskName LIKE '%{0}%' ", query);
            }

            PagerSet pagerSet = FacadeManage.aidePlatformFacade.GetList(Entity.Platform.TaskInfo.Tablename, where, Orderby,
                anpNews.CurrentPageIndex, anpNews.PageSize);
            anpNews.RecordCount = pagerSet.RecordCount;
            if (pagerSet.RecordCount > 0)
            {
                rptDataList.DataSource = pagerSet.PageSet;
                rptDataList.DataBind();
            }
            rptDataList.Visible = pagerSet.RecordCount > 0;
            litNoData.Visible = pagerSet.RecordCount <= 0;
        }
        /// <summary>
        /// 任务类型
        /// </summary>
        protected string TaskTypeName(int typeid)
        {
            return typeid == 1 ? "总局任务" : typeid == 2 ? "胜局任务" : typeid == 4 ? "首胜任务" : typeid == 8 ? "连胜任务" : "其他任务";
        }
        /// <summary>
        /// 用户类型
        /// </summary>
        protected string UserTypeName(int typeid)
        {
            return typeid == 1 ? "普通用户" : typeid == 2 ? "会员用户" : "所有用户";
        }
        /// <summary>
        /// 时间模式
        /// </summary>
        protected string TimeTypeName(int typeid)
        {
            return typeid == 1 ? "一次任务" : typeid == 2 ? "每日任务" : typeid == 4 ? "每周任务" : "每月任务";
        }
        /// <summary>
        /// 任务奖励
        /// </summary>
        /// <returns></returns>
        protected string GetTaskAward(string award)
        {
            string value = "";
            if (!string.IsNullOrEmpty(award))
            {
                string[] awards = null;
                if (award.IndexOf(';') > 0)
                {
                    string[] arry = award.Split(';');
                    foreach (var item in arry)
                    {
                        awards = item.Split(':');
                        value = value + AwardName(Convert.ToInt32(awards[0])) + "：" + awards[1].ToString() + ",";
                    }
                    value = value.Substring(0,value.Length-1);
                }
                else
                {
                    awards = award.Split(':');
                    value = AwardName(Convert.ToInt32(awards[0])) + "：" + awards[1].ToString();
                }
                return value;
            }
            return "";
        }

        /// <summary>
        /// 奖励的类型
        /// </summary>
        private string AwardName(int typeid)
        {
            return typeid == 1 ? "金币" : typeid == 2 ? "钻石" : typeid == 3 ? "奖券" : "其他";
        }

        /// <summary>
        /// 查询条件
        /// </summary>
        public string SearchItems
        {
            get
            {
                if (ViewState["SearchItems"] == null)
                {
                    ViewState["SearchItems"] = " WHERE 1=1";
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
                if (ViewState["Orderby"] == null)
                {
                    ViewState["Orderby"] = "ORDER BY TaskID DESC";
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