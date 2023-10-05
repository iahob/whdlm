using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Utils;
using Game.Web.UI;
using Game.Kernel;
using Game.Facade;
using Game.Entity.NativeWeb;
using System.Data;
using Game.Entity.Enum;
using System.Text;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.MallManager
{
    public partial class MallInfoList : AdminPage
    {
        #region 窗口事件
        protected void Page_Load( object sender, EventArgs e )
        {
            base.moduleID = 1101;
            if( !IsPostBack )
            {
                //DataMallType();
                DataMallInfo();
            }
        }

        /// <summary>
        /// 数据分页
        /// </summary>
        protected void anpPage_PageChanged( object sender, EventArgs e )
        {
            DataMallInfo();
        }

        /// <summary>
        /// 启用
        /// </summary>
        protected void btnNulityTrue_Click( object sender, EventArgs e )
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "商品上架";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission( Permission.IsNulity );
            if(StrCIdList != "")
            {
                int num = FacadeManage.aideNativeWebFacade.UpdateAwardInfoNulity(StrCIdList, 0);
                if( num > 0 )
                {
                    ShowInfo( "商品上架成功" );
                }
                else
                {
                    ShowInfo( "商品上架失败" );
                }
            }
            DataMallInfo();
        }
        /// <summary>
        /// 禁用
        /// </summary>
        protected void btnNulityFalse_Click( object sender, EventArgs e )
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "商品下架";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            AuthUserOperationPermission( Permission.IsNulity );
            if(StrCIdList != "")
            {
                int num = FacadeManage.aideNativeWebFacade.UpdateAwardInfoNulity(StrCIdList, 1);
                if( num > 0 )
                {
                    ShowInfo( "商品下架成功" );
                }
                else
                {
                    ShowInfo( "商品下架失败" );
                }
            }
            DataMallInfo();
        }

        /// <summary>
        /// 点击查询
        /// </summary>
        protected void btnSearch_Click( object sender, EventArgs e )
        {
            DataMallInfo();
        }

        /// <summary>
        /// 刷新
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnRefresh_Click( object sender, EventArgs e )
        {
            Response.Redirect( "MallInfoList.aspx" );
        }
        #endregion

        #region 窗口方法
        /// <summary>
        /// 绑定数据
        /// </summary>
        private void DataMallInfo()
        {
            //控件绑定
            PagerSet pagerSet = FacadeManage.aideNativeWebFacade.GetList( AwardInfo.Tablename, GetWhere(), "ORDER BY SortID DESC", anpPage.CurrentPageIndex, anpPage.PageSize );
            litNoData.Visible = pagerSet.PageSet.Tables[0].Rows.Count > 0 ? false : true;
            rpData.DataSource = pagerSet.PageSet;
            rpData.DataBind();
            anpPage.RecordCount = pagerSet.RecordCount;
        }

        /// <summary>
        /// 绑定商品类型
        /// </summary>
        //private void DataMallType()
        //{
        //    DataSet ds = FacadeManage.aideNativeWebFacade.GetAllAwardType();
        //    ddlType.DataSource = ds.Tables[0];
        //    ddlType.DataTextField = "TypeName";
        //    ddlType.DataValueField = "TypeID";
        //    ddlType.DataBind();
        //    ddlType.Items.Insert( 0, new ListItem( "全部", "0" ) );
        //}

        /// <summary>
        /// 获取查询条件
        /// </summary>
        private string GetWhere()
        {
            StringBuilder where = new StringBuilder( " WHERE 1=1" );
            string name = CtrlHelper.GetTextAndFilter( txtName );
            if( !string.IsNullOrEmpty( name ) )
            {
                where.AppendFormat( " AND AwardName Like '%{0}%'", name );
            }
            string type = CtrlHelper.GetSelectValue( ddlType );
            if( type != "0" )
            {
                where.AppendFormat(" AND AwardType={0}", type );
            }
            string start = CtrlHelper.GetTextAndFilter( txtPriceStart );
            string end = CtrlHelper.GetTextAndFilter( txtPriceEnd );
            if( !string.IsNullOrEmpty( start ) && !string.IsNullOrEmpty( end ) )
            {
                where.AppendFormat(" AND UnitPrice>={0} AND UnitPrice<={1}", start, end );
            }
            return where.ToString();
        }

        /// <summary>
        /// 获取需要填写信息
        /// </summary>
        //public string GetNeedInfo( int NeedInfo )
        //{
        //    string str = "";
        //    IList<EnumDescription> list = EnumDescription.GetFieldTexts( typeof( EnumerationList.MallNeedInfo ) );
        //    for( int i = 0; i < list.Count; i++ )
        //    {
        //        EnumDescription em = list[i];
        //        int num = NeedInfo & ( Convert.ToInt32( em.EnumValue ) );
        //        if( num == ( Convert.ToInt32( em.EnumValue ) ) )
        //        {
        //            str = str + em.Description + "/";
        //        }
        //    }
        //    return str == "" ? "不需要" : str.Substring( 0, str.Length - 1 );
        //}
        #endregion
    }
}