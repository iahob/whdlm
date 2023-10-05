﻿using System;
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
using Game.Entity.Record;

namespace Game.Web.Module.DataStatistics
{
    public partial class DailyDiamond : AdminPage
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 700;
            if (!IsPostBack)
            {
                DateTime time = DateTime.Now;
                txtStartDate.Text = time.AddDays(-15).ToString("yyyy-MM-dd");
                txtEndDate.Text = time.AddDays(-1).ToString("yyyy-MM-dd");
            }
        }
    }
}