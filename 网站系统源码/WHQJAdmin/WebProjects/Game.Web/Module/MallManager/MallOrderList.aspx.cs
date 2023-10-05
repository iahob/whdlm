using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Web.UI;
using Game.Kernel;
using Game.Utils;
using Game.Facade;
using Game.Entity.NativeWeb;
using Game.Entity.Enum;
using System.Text;

namespace Game.Web.Module.MallManager
{
    public partial class MallOrderList : AdminPage
    {
        #region 属性
        /// <summary>
        /// 排序条件
        /// </summary>
        public string OrderBy
        {
            get
            {
                if( ViewState["OrderBy"] == null )
                {
                    ViewState["OrderBy"] = "Order By CollectDate DESC";
                }
                return (string)ViewState["OrderBy"];
            }
            set
            {
                ViewState["OrderBy"] = value;
            }
        }

        /// <summary>
        /// 查询条件
        /// </summary>
        public string SearchItems
        {
            get
            {
                if( ViewState["SearchItems"] == null )
                {
                    ViewState["SearchItems"] = "";
                }
                return (string)ViewState["SearchItems"];
            }
            set
            {
                ViewState["SearchItems"] = value;
            }
        }
        #endregion

        #region 窗口事件
        protected void Page_Load( object sender, EventArgs e )
        {
            base.moduleID = 1102;
            if( !IsPostBack )
            {
                DataMallOrder();
            }
        }

        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged( object sender, EventArgs e )
        {
            DataMallOrder();
        }

        /// <summary>
        /// 订单查询
        /// </summary>
        protected void btnSearch_Click( object sender, EventArgs e )
        {
            StringBuilder where = new StringBuilder( "WHERE 1=1" );
            string orderNum = CtrlHelper.GetTextAndFilter( txtOrderID );
            if( !string.IsNullOrEmpty( orderNum ) )
            {
                where.AppendFormat( " AND OrderID='{0}'", orderNum );
            }
            string start = CtrlHelper.GetTextAndFilter( txtStartDate );
            string end = CtrlHelper.GetTextAndFilter( txtEndDate );
            if( !string.IsNullOrEmpty( start ) && !string.IsNullOrEmpty( end ) )
            {
                where.AppendFormat(" AND CollectDate>='{0}' AND CollectDate<='{1}'", Fetch.GetStartTime( Convert.ToDateTime( start ) ), Fetch.GetEndTime( Convert.ToDateTime( end ) ) );
            }
            SearchItems = where.ToString();
            anpPage.CurrentPageIndex = 1;
            DataMallOrder();
        }

        /// <summary>
        /// 用户查询
        /// </summary>
        protected void btnUseSearch_Click( object sender, EventArgs e )
        {
            StringBuilder where = new StringBuilder( "WHERE 1=1" );
            string text = CtrlHelper.GetTextAndFilter( txtUser );
            int a = 0;
            if( !string.IsNullOrEmpty( text ) )
            {
                int userID;
                if(ddlType.SelectedValue == "0" )
                {
                    userID = GetUserIDByAccount( text );
                }
                else
                {
                    if( !int.TryParse(text, out a) )
                    {
                        ShowError( "游戏ID必须为整数" );
                        return;
                    }
                    userID = GetUserIDByGameID( Convert.ToInt32( text ) );
                }
                where.AppendFormat( " AND UserID={0}", userID );
            }
            SearchItems = where.ToString();
            DataMallOrder();
        }

        /// <summary>
        /// 订单状态查询
        /// </summary>
        protected void btnNewOrder_Command( object sender, CommandEventArgs e )
        {
            StringBuilder where = new StringBuilder( "WHERE 1=1" );
            string orderNum = CtrlHelper.GetTextAndFilter( txtOrderID );
            if( orderNum != "" )
            {
                where.AppendFormat( " AND OrderID={0}", orderNum );
            }
            string start = CtrlHelper.GetTextAndFilter( txtStartDate );
            string end = CtrlHelper.GetTextAndFilter( txtEndDate );
            if( start != "" && end != "" )
            {
                where.AppendFormat(" AND CollectDate>='{0}' AND CollectDate<='{1}'", Fetch.GetStartTime( Convert.ToDateTime( start ) ), Fetch.GetEndTime( Convert.ToDateTime( end ) ) );
            }
            string text = CtrlHelper.GetTextAndFilter( txtUser );
            if( !string.IsNullOrEmpty( text ) )
            {
                int userID;
                if( CtrlHelper.GetSelectValue( ddlType ) == "0" )
                {
                    userID = GetUserIDByAccount( text );
                }
                else
                {
                    if( !Utils.Validate.IsNumeric( text ) )
                    {
                        ShowError( "游戏ID必须为整数" );
                        return;
                    }
                    userID = GetUserIDByGameID( Convert.ToInt32( text ) );
                }
                where.AppendFormat( " AND UserID={0}", userID );
            }
            where.AppendFormat( " AND OrderStatus={0}", e.CommandArgument );
            SearchItems = where.ToString();
            DataMallOrder();
        }

        /// <summary>
        /// 刷新
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnRefresh_Click( object sender, EventArgs e )
        {
            Response.Redirect( "MallOrderList.aspx" );
        }
        #endregion

        #region 窗口方法

        /// <summary>
        /// 绑定数据
        /// </summary>
        private void DataMallOrder()
        {
            //控件绑定
            PagerSet pagerSet = FacadeManage.aideNativeWebFacade.GetList( AwardOrder.Tablename, SearchItems, " ORDER BY OrderID DESC", anpPage.CurrentPageIndex, anpPage.PageSize );
            if( pagerSet.PageSet.Tables[0].Rows.Count > 0 )
            {
                litNoData.Visible = false;
                rpData.Visible = true;
                rpData.DataSource = pagerSet.PageSet;
                rpData.DataBind();
                anpPage.RecordCount = pagerSet.RecordCount;
            }
            else
            {
                litNoData.Visible = true;
                rpData.Visible = false;
            }
        }

        /// <summary>
        /// 获取订单状态
        /// </summary>
        public string GetOrderState( int state )
        {
            switch( state )
            {
                case 0:
                    return "<font class=\"hong\">新订单</font>";
                case 1:
                    return "<font class=\"lanse\">已发货</font>";
                case 2:
                    return "已收货";
                case 3:
                    return "<font class=\"chengse\">申请退货</font>";
                case 4:
                    return "<font class=\"chengse\">同意退货</font>";
                case 5:
                    return "拒绝退货";
                case 6:
                    return "退货成功并且已退款";
            }
            return "";
        }

        /// <summary>
        /// 获取商品名称
        /// </summary>
        public string GetAwardInfoName( int id )
        {
            AwardInfo info = FacadeManage.aideNativeWebFacade.GetAwardInfoByID( id );
            if( info != null )
            {
                return info.AwardName;
            }
            return "";
        }
        #endregion
    }
}