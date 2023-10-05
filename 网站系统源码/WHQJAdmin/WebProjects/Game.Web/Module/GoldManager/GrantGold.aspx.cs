using System;
using System.Threading;
using System.Threading.Tasks;
using Game.Entity.Record;
using Game.Facade;
using Game.Web.UI;
using Game.Entity.Enum;
using Game.Utils;
using Game.Socket;
using Game.Socket.DataStruct;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.GoldManager
{
    public partial class GrantGold : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            moduleID = 500;
            if (IsPostBack) return;
            AuthUserOperationPermission(Permission.GrantTreasure);
            trGameID.Visible = IntParam <= 0;
        }

        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            var strReason = CtrlHelper.GetTextAndFilter(txtReason);
            int gold;
            if (!int.TryParse(txtGold.Text, out gold))
            {
                MessageBox("赠送金币格式不正确");
                return;
            }
            var userid = IntParam <= 0 ? CtrlHelper.GetInt(hidUserID, 0) : IntParam;
            if (string.IsNullOrEmpty(strReason))
            {
                MessageBox("赠送备注不能为空");
                return;
            }

            var ip = GameRequest.GetUserIP();
            var rgt = new RecordGrantTreasure
            {
                MasterID = userExt.UserID,
                UserID = userid,
                AddGold = gold,
                ClientIP = ip,
                Reason = strReason
            };

            var msg = FacadeManage.aideTreasureFacade.GrantTreasure(rgt);
            if (msg.Success)
            {
                // 插入日志
                var ss = new SystemSecurity
                {
                    OperatingAccounts = userExt.UserName,
                    OperatingName = $"后台赠送金币，用户标识：【{userid}】金币数量：{gold}",
                    OperatingIP = userExt.PreLoginIP
                };
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                //由任务线程实现通知服务端
                Task.Factory.StartNew(() => {
                    SocketManage.aideWebSocketFacade.UpdateSuccessARE = new AutoResetEvent(false);
                    SocketManage.aideWebSocketFacade.UpdateUserScore(userid, (short)UpdateUserScoreType.Normal,
                        isSent => { });
                    SocketManage.aideWebSocketFacade.UpdateSuccessARE.WaitOne();
                });

                MessageBox("赠送成功"); 
            }
            else
            {
                MessageBox("赠送失败," + msg.MessageID);
            }
        }
    }
}
