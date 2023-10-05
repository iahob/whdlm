using System;
using System.Threading;
using Game.Entity.Record;
using Game.Facade;
using Game.Web.UI;
using Game.Entity.Enum;
using Game.Utils;
using Game.Entity.PlatformManager;
using System.Threading.Tasks;
using Game.Socket;
using Game.Socket.DataStruct;

namespace Game.Web.Module.Diamond
{
    public partial class GrantDiamond : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            moduleID = 600;
            if (!IsPostBack)
            {
                AuthUserOperationPermission(Permission.GrantDiamond);
                trGameID.Visible = IntParam <= 0;
            }
        }

        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            var strReason = CtrlHelper.GetTextAndFilter(txtReason);
            int diamond;
            if (!int.TryParse(txtDiamond.Text, out diamond))
            {
                MessageBox("赠送钻石格式不正确");
                return;
            }

            //bool flag = cbPull.Checked;
            var userid = IntParam <= 0 ? CtrlHelper.GetInt(hidUserID, 0) : IntParam;
            //if(diamond <= 0)
            //{
            //    MessageBox("赠送钻石不能小于或等于零");
            //    return;
            //}
            if (string.IsNullOrEmpty(strReason))
            {
                MessageBox("赠送备注不能为空");
                return;
            }

            var ip = GameRequest.GetUserIP();
            var rgd = new RecordGrantDiamond
            {
                MasterID = userExt.UserID,
                UserID = userid,
                TypeID = 0,
                AddDiamond = diamond,
                ClientIP = ip,
                CollectNote = strReason
            };

            var msg = FacadeManage.aideTreasureFacade.GrantDiamond(rgd);
            if (msg.Success)
            {
                // 插入日志
                var ss = new SystemSecurity
                {
                    OperatingAccounts = userExt.UserName,
                    OperatingName = $"后台赠送钻石，用户标识：【{userid}】 钻石：{diamond}",
                    OperatingIP = userExt.PreLoginIP
                };
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                // if (flag)
                // {
                //     AccountsUmeng umeng = FacadeManage.aideAccountsFacade.GetAccountsUmeng(IntParam);
                //     if(!string.IsNullOrEmpty(umeng?.DeviceToken))
                //     {
                //         string content = "系统管理员" + userExt.UserName + "已" + (diamond < 0 ? "扣除" : "赠送") + "您" + diamond.ToString() + "钻石";
                //         DateTime start = DateTime.Now.AddMinutes(1);
                //         DateTime end = start.AddHours(5);
                //         bool result = Umeng.SendMessage(umeng.DeviceType, content, "unicast", start.ToString("yyyy-MM-dd HH:mm:ss"), end.ToString("yyyy-MM-dd HH:mm:ss"), umeng.DeviceToken);
                //         if(!result)
                //         {
                //             MessageBox("赠送成功，但推送消息失败，请前往友盟后台绑定系统后台ip");
                //             return;
                //         }
                //         RecordAccountsUmeng record = new RecordAccountsUmeng
                //         {
                //             MasterID = rgd.MasterID,
                //             UserID = rgd.UserID,
                //             PushType = umeng.DeviceType,
                //             PushTime = DateTime.Now,
                //             PushIP = ip,
                //             PushContent = content
                //         };
                //         int rows = FacadeManage.aideRecordFacade.AddRecordAccountsUmeng(record);
                //         MessageBox(rows > 0 ? "赠送成功" : "赠送成功，但推送记录写入失败");
                //     }
                //     else
                //     {
                //         MessageBox("赠送成功，但推送用户未绑定设备，无法推送");
                //     }
                // }
                // else
                // {
                //     MessageBox("赠送成功");
                // }

                //由任务线程实现通知服务端
                Task.Factory.StartNew(() =>
                {
                    SocketManage.aideWebSocketFacade.UpdateSuccessARE = new AutoResetEvent(false);
                    SocketManage.aideWebSocketFacade.UpdateUserScore(userid, (short) UpdateUserScoreType.Normal,
                        isSent => { });
                    SocketManage.aideWebSocketFacade.UpdateSuccessARE.WaitOne();
                });
                MessageBox("赠送成功");
            }
            else
            {
                MessageBox("赠送失败");
            }
        }
    }
}
