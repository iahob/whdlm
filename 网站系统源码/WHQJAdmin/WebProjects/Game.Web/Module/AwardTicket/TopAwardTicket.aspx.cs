using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Kernel;
using System.Text;
using System.Data;
using Game.Utils;
using Game.Entity.Accounts;
using Game.Facade;
using Game.Entity.Treasure;

namespace Game.Web.Module.AwardTicket
{
    public partial class TopAwardTicket : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1200;
            if (!IsPostBack)
            {
                rptDataList.DataSource = FacadeManage.aideTreasureFacade.GetAwardTicketRank();
                rptDataList.DataBind();
            }
        }
    }
}