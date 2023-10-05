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
using System.Data;
using Game.Entity.NativeWeb;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.MallManager
{
    public partial class MallInfo : AdminPage
    {
        #region 窗口事件
        protected void Page_Load( object sender, EventArgs e )
        {
            base.moduleID = 1101;
            if( !IsPostBack )
            {
                //DataMallType();
                Inition();
            }
        }
        /// <summary>
        /// 点击保存
        /// </summary>
        protected void btnSave_Click( object sender, EventArgs e )
        {
            if( !IsValid )
            {
                return;
            }

            if( string.IsNullOrEmpty( CtrlHelper.GetSelectValue( ddlTypeID ) ) )
            {
                ShowError( "请为商品选择一个商品类型" );
                return;
            }

            AwardInfo info = new AwardInfo();
            if( IntParam > 0 )
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改商品配置";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission( Permission.Edit );
                info = FacadeManage.aideNativeWebFacade.GetAwardInfoByID( IntParam );
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "新增商品配置";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission( Permission.Add );
                info.CollectDate = DateTime.Now;
            }

            #region 图片验证


            //展示图上传
            try
            {
                info.AwardImg = upBigImage.FilePath;
            }
            catch( Exception ex )
            {
                ShowError( "展示图上传失败：" + ex.Message );
                return;
            }

            if (string.IsNullOrEmpty(info.AwardImg))
            {
                ShowError("请上传商品大图");
                return;
            }

            #endregion

            info.AwardName = CtrlHelper.GetTextAndFilter( txtAwardName );
            if (info.AwardName.Length > 32)
            {
                ShowError("商品名称过长！");
                return;
            }
            info.Inventory = CtrlHelper.GetInt( txtInventory, 0 );
          
            info.Nullity = Convert.ToByte( rbNullity.SelectedValue );
            info.UnitPrice = CtrlHelper.GetInt( txtPrice, 0 );
            info.Gold = CtrlHelper.GetInt(txtGold, 0);
            info.Diamond = CtrlHelper.GetInt(txtDiamond, 0);
            info.SortID = CtrlHelper.GetInt( txtSortID, 0 );
            info.AwardType = Convert.ToInt32( CtrlHelper.GetSelectValue( ddlTypeID ) );
            bool result;
            if( IntParam > 0 )
            {
                result = FacadeManage.aideNativeWebFacade.UpdateAwardInfo( info );
            }
            else
            {
                result = FacadeManage.aideNativeWebFacade.InsertAwardInfo( info );
            }

            if( result )
            {
                ShowInfo( "商品信息保存成功", "MallInfoList.aspx", 1000 );
            }
            else
            {
                ShowError( "商品信息保存失败" );
            }
        }
        #endregion

        #region 窗口方法
        /// <summary>
        /// 绑定商品类型
        /// </summary>
        //private void DataMallType()
        //{
        //    ///绑定商品类型
        //    DataSet ds = FacadeManage.aideNativeWebFacade.GetAllAwardType();
        //    ddlTypeID.DataSource = ds.Tables[0];
        //    ddlTypeID.DataTextField = "TypeName";
        //    ddlTypeID.DataValueField = "TypeID";
        //    ddlTypeID.DataBind();

        //    ///绑定需要填写信息
        //    IList<EnumDescription> NeedInfo = EnumDescription.GetFieldTexts( typeof( EnumerationList.MallNeedInfo ) );
        //    cbNeedInfo.DataSource = NeedInfo;
        //    cbNeedInfo.DataValueField = "EnumValue";
        //    cbNeedInfo.DataTextField = "Description";
        //    cbNeedInfo.DataBind();
        //    for( int i = 0; i < cbNeedInfo.Items.Count; i++ )
        //    {
        //        cbNeedInfo.Items[i].Selected = true;
        //    }
        //}
        /// <summary>
        /// 数据初始化
        /// </summary>
        private void Inition()
        {
            if( IntParam > 0 )
            {
                AwardInfo info = FacadeManage.aideNativeWebFacade.GetAwardInfoByID( IntParam );
                CtrlHelper.SetText( txtAwardName, info.AwardName );
               
                CtrlHelper.SetText( txtInventory, info.Inventory.ToString() );
                CtrlHelper.SetText( txtPrice, info.UnitPrice.ToString() );

                CtrlHelper.SetText(txtGold, info.Gold.ToString());
                CtrlHelper.SetText(txtDiamond, info.Diamond.ToString());
                CtrlHelper.SetText( txtSortID, info.SortID.ToString() );
                //rbIsReturn.SelectedValue = info.IsReturn == true ? "1" : "0";
                rbNullity.SelectedValue = info.Nullity.ToString();
                ddlTypeID.SelectedValue = info.AwardType.ToString();

                //for( int i = 0; i < cbNeedInfo.Items.Count; i++ )
                //{
                //    int valu = Convert.ToInt32( cbNeedInfo.Items[i].Value );
                //    int num = info.NeedInfo & valu;
                //    if( num == valu )
                //    {
                //        cbNeedInfo.Items[i].Selected = true;
                //    }
                //    else
                //    {
                //        cbNeedInfo.Items[i].Selected = false;
                //    }
                //}

               
                if( !string.IsNullOrEmpty( info.AwardImg) )
                {
                    upBigImage.FilePath = info.AwardImg;
                }
            }
        }

        #endregion
    }
}