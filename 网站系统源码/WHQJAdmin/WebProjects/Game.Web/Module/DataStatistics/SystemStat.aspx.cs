using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Utils;
using Game.Web.UI;
using Game.Kernel;
using System.Data;
using Game.Facade;

namespace Game.Web.Module.DataStatistics
{
    public partial class SystemStat : AdminPage
    {
        #region 窗口事件

        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 706;
            if (!Page.IsPostBack)
            {
                BindDataStat();
            }
        }

        #endregion

        #region 数据加载
        private void BindDataStat()
        {
            DataSet ds = FacadeManage.aideTreasureFacade.GetStatInfo();
            if (ds.Tables[0]!=null && ds.Tables[0].Rows.Count > 0)
            {
                DataRow row = ds.Tables[0].Rows[0];
                ltAllRevenue.Text = (Convert.ToInt64(row["TotalRevenue"]) + Convert.ToInt64(row["TotalInsureRevenue"])).ToString("N");
                ltAllScore.Text = (Convert.ToInt64(row["Score"])+ Convert.ToInt64(row["InsureScore"])).ToString("N");
                ltAmount.Text = row["Amount"].ToString();
                ltDiamond.Text = Convert.ToInt64(row["Diamond"]).ToString("N");
                ltGameRegisterUser.Text = row["GameRegisterUser"].ToString();
                ltGoldRoomCount.Text = row["GoldRoomCount"].ToString();
                ltH5RegisterUser.Text = row["H5RegisterUser"].ToString();
                ltInsureRevenue.Text = Convert.ToInt64(row["InsureRevenue"]).ToString("N");
                ltInsureScore.Text = Convert.ToInt64(row["InsureScore"]).ToString("N");
                ltPayDiamond.Text = row["PayDiamond"].ToString();
                ltPayDiamondAmount.Text = row["PayDiamondAmount"].ToString();
                ltPayScore.Text = row["PayScore"].ToString();
                ltPayScoreAmount.Text = row["PayScoreAmount"].ToString();
                ltRevenue.Text = row["Revenue"].ToString();
                ltScore.Text = Convert.ToInt64(row["Score"]).ToString("N");
                ltScoreRoomCount.Text = row["ScoreRoomCount"].ToString();
                ltTotalAmount.Text = row["TotalAmount"].ToString();
                ltTotalInsureRevenue.Text = Convert.ToInt64(row["TotalInsureRevenue"]).ToString("N");
                ltTotalRevenue.Text = Convert.ToInt64(row["TotalRevenue"]).ToString("N");
                ltTotalUser.Text = row["TotalUser"].ToString();
                ltTotalWaste.Text = Convert.ToInt64(row["TotalWaste"]).ToString("N");
                ltWaste.Text = Convert.ToInt64(row["Waste"]).ToString("N");
                ltWebRegisterUser.Text = row["WebRegisterUser"].ToString();
            }
        }

        #endregion
    }
}