using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Kernel;
using Game.Facade;
using Game.Entity.NativeWeb;
using Game.Utils;

namespace Game.Web.Themes
{
    public partial class TabSiteConfig : System.Web.UI.UserControl
    {
        protected int IntParam = GameRequest.GetQueryInt( "param", 0 );

        protected void Page_Load( object sender, EventArgs e )
        {
            if(!IsPostBack)
            {
                NativeWebFacade aideNativeWebFacade = new NativeWebFacade();
                PagerSet pagerSet = aideNativeWebFacade.GetList(ConfigInfo.Tablename, "", " ORDER BY ConfigID ASC", 1, 10000);
                rptDataList.DataSource = pagerSet.PageSet;
                rptDataList.DataBind();
            }
        }
    }
}