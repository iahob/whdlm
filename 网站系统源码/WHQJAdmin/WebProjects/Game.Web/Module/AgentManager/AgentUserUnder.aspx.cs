using System;
using System.Collections.Generic;
using System.Data;
using Game.Web.UI;
using Game.Entity.Agent;
using Game.Facade;

namespace Game.Web.Module.AgentManager
{
    public partial class AgentUserUnder : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1001;
            if(!IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            DataSet ds = FacadeManage.aideAgentFacade.GetAgentBelowRegisterList(IntParam);
            if (ds==null) return;
            lbTotal.Text = ds.Tables[0].Rows.Count.ToString();
            litNoData.Visible = ds.Tables[0].Rows.Count <= 0;
            rptDataList.DataSource = ds;
            rptDataList.DataBind();
        }
    }
}