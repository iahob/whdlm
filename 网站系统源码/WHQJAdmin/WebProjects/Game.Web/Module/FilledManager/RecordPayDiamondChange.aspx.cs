using System;
using Game.Web.UI;
using Game.Utils;
using Game.Entity.Treasure;
using Game.Facade;
using Game.Entity.Enum;

namespace Game.Web.Module.FilledManager
{
    public partial class RecordPayDiamondChange : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 203;
            if (!Page.IsPostBack)
            {
                if (IntParam == 0)
                {
                    MessageBoxCloseRef("参数错误");
                    return;
                }

                BindData();
            }
        }

        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(StrParam))
            {
                MessageBox("参数错误，请重试");
                return;
            }

            var msg = FacadeManage.aideTreasureFacade.OperationOnLineOrder(StrParam);
            if (msg.Success)
            {
                MessageBoxCloseRef("处理成功");
            }
            else
            {
                MessageBox("操作失败：" + msg.Content);
            }
        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        private void BindData()
        {
            var info = FacadeManage.aideTreasureFacade.GetOnLinePayOrder(StrParam);
            CtrlHelper.SetText(lblOrderID, info.OrderID);
            CtrlHelper.SetText(lblAmount, info.Amount.ToString("F2"));
            CtrlHelper.SetText(lblStatus, info.OrderStatus == 0 ? "未处理" : info.OrderStatus == 1 ? "已充值，未处理" : "已完成");
        }
    }
}