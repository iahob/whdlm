using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Web.UI;
using Game.Socket.DataStruct;
using Game.Socket;
using System.Threading;
using Game.Utils;
using Game.Facade;
using Game.Entity.Group;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.DebugManager
{
    public partial class GroupGameControlSubInfo : AdminPage
    {

        public static ShareStorage storage = new ShareStorage();

        public static int groupId = 0;
        public static int kindId = 0;
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                groupId = GameRequest.GetInt("groupid", 0);
                kindId = GameRequest.GetInt("kindid", 0);

                if (groupId <= 0 || kindId <= 0)
                {
                    MessageBox("参数错误");
                    return;
                }


                storage=new ShareStorage();
                storage.Status =(int)OperStorgeType.客户端查询子项;
                storage.ShareStorageItem.GroupID = groupId;
                storage.ShareStorageItem.KindID =(short)kindId;
                SendMessageToServer();

                int kindType = 1;
                if (kindType == 1)
                {
                    trAnchou1.Visible = false;
                    trAnchou2.Visible = false;
                    trAnchou3.Visible = false;
                    trAnchou4.Visible = false;
                    trAnchou5.Visible = false;
                }
                else
                {
                    trAnchou1.Visible = true;
                    trAnchou2.Visible = true;
                    trAnchou3.Visible = true;
                    trAnchou4.Visible = true;
                    trAnchou5.Visible = true;
                }



            }


        }

        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void BindData()
        {

            if (storage.GameServerRegister && storage.Status== (int)OperStorgeType.客户端查询子项失败)
            {
                MessageBox(storage.ResultMessage);
                return;
            }

            if (!storage.GameServerRegister)
            {
                IMGroupShareStorageProperty gssp = FacadeManage.aideGroupFacade.GetShareStorage(groupId, kindId, 1);
                if (gssp != null)
                {
                    byte[] buffer = WebSocketHelper.HexStrTobyte(gssp.CurSubStorageItem);
                    var Rdata = buffer;
                    if (buffer.Length>0)
                    {
                        storage.ShareStorageItem.CurSubStorageItem.CurStatisticSysWin = WebSocketHelper.GetLong(Rdata, 0, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurStatisticService = WebSocketHelper.GetLong(Rdata, 8, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurSysStorage = WebSocketHelper.GetLong(Rdata, 16, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurPlayerStorage = WebSocketHelper.GetLong(Rdata, 24, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurParameterK = WebSocketHelper.GetLong(Rdata, 32, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurSysStorResetRate = WebSocketHelper.GetLong(Rdata, 40, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurWinChance = WebSocketHelper.GetLong(Rdata, 48, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurResetCount = WebSocketHelper.GetLong(Rdata, 56, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurSysWin = WebSocketHelper.GetLong(Rdata, 64, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurAnChouRate = WebSocketHelper.GetLong(Rdata, 72, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurAnChouTotal = WebSocketHelper.GetLong(Rdata, 80, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurMosgoldRate = WebSocketHelper.GetLong(Rdata, 88, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurMosgoldDispatchRate = WebSocketHelper.GetLong(Rdata, 96, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurMosgoldStorageRate = WebSocketHelper.GetLong(Rdata, 104, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurTempSysStorage = WebSocketHelper.GetLong(Rdata, 112, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurTempPlayerStorage = WebSocketHelper.GetLong(Rdata, 120, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurTempParameterK = WebSocketHelper.GetLong(Rdata, 128, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurTempSysStorResetRate = WebSocketHelper.GetLong(Rdata, 136, 8);
                        storage.ShareStorageItem.CurSubStorageItem.CurTempWinChance = WebSocketHelper.GetLong(Rdata, 144, 8);
                    }

                  
                }
            }

            ltCurAnChouRate.Text = storage.ShareStorageItem.CurSubStorageItem.CurAnChouRate.ToString();
            ltCurAnChouTotal.Text = storage.ShareStorageItem.CurSubStorageItem.CurAnChouTotal.ToString();
            ltCurSysStorage.Text = storage.ShareStorageItem.CurSubStorageItem.CurSysStorage.ToString();
            ltCurPlayerStorage.Text = storage.ShareStorageItem.CurSubStorageItem.CurPlayerStorage.ToString();
            ltCurParameterK.Text = storage.ShareStorageItem.CurSubStorageItem.CurParameterK.ToString();
            ltCurSysStorResetRate.Text = storage.ShareStorageItem.CurSubStorageItem.CurSysStorResetRate.ToString();
            ltCurWinChance.Text = storage.ShareStorageItem.CurSubStorageItem.CurWinChance.ToString();
            ltCurResetCount.Text = storage.ShareStorageItem.CurSubStorageItem.CurResetCount.ToString();
            ltCurSysWin.Text = storage.ShareStorageItem.CurSubStorageItem.CurSysWin.ToString();
            ltCurMosgoldStorageRate.Text = storage.ShareStorageItem.CurSubStorageItem.CurMosgoldStorageRate.ToString();
            ltCurMosgoldRate.Text = storage.ShareStorageItem.CurSubStorageItem.CurMosgoldRate.ToString();
            ltCurMosgoldDispatchRate.Text = storage.ShareStorageItem.CurSubStorageItem.CurMosgoldDispatchRate.ToString();
            ltCurTempSysStorage.Text = storage.ShareStorageItem.CurSubStorageItem.CurTempSysStorage.ToString();
            ltCurTempPlayerStorage.Text = storage.ShareStorageItem.CurSubStorageItem.CurTempPlayerStorage.ToString();
            ltCurTempParameterK.Text = storage.ShareStorageItem.CurSubStorageItem.CurTempParameterK.ToString();
            ltCurTempSysStorResetRate.Text = storage.ShareStorageItem.CurSubStorageItem.CurTempSysStorResetRate.ToString();
            ltCurTempWinChance.Text = storage.ShareStorageItem.CurSubStorageItem.CurTempWinChance.ToString();

        }





        /// <summary>
        /// 修改抽水配置
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnAnChou_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "修改抽水配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            string ConfigAnChouRate = txtConfigAnChouRate.Text;
            if (string.IsNullOrEmpty(ConfigAnChouRate))
            {
                MessageBox("配置抽水比例不能为空");
                return;
            }

            if (!Utils.Validate.IsPositiveInt(ConfigAnChouRate) )
            {
                MessageBox("配置抽水比例格式错误");
                return;
            }
            if (Convert.ToInt64(ConfigAnChouRate) > 100)
            {
                MessageBox("配置抽水比例不能大于100");
                return;
            }

            storage.ShareStorageItem.ConfigSubStorageItem.ConfigAnChouRate = Convert.ToInt64(ConfigAnChouRate);

            storage.Status = (int)OperStorgeType.修改子项;
            storage.ShareStorageItem.ModifySubItemType = (int)UpateStorgeSubType.暗抽子项;

            SendMessageToServer();
            if (storage.Status==(int)OperStorgeType.修改子项结果失败)
            {
                MessageBox(storage.ResultMessage);
            }
            else
            {
                MessageBox("操作成功");
            }
        }

        /// <summary>
        /// 修改大奖配置
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnMosgold_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "修改大奖配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            string ConfigMosgoldStorageRate=txtConfigMosgoldStorageRate.Text;
            if (string.IsNullOrEmpty(ConfigMosgoldStorageRate))
            {
                MessageBox("大奖库存比例不能为空");
                return;
            }

            if (!Utils.Validate.IsPositiveInt(ConfigMosgoldStorageRate))
            {
                MessageBox("大奖库存比例格式错误");
                return;
            }


            string ConfigMosgoldRate = txtConfigMosgoldRate.Text;
            if (string.IsNullOrEmpty(ConfigMosgoldRate))
            {
                MessageBox("大奖抽取比例不能为空");
                return;
            }

            if (!Utils.Validate.IsPositiveInt(ConfigMosgoldRate))
            {
                MessageBox("大奖抽取比例格式错误");
                return;
            }

            if (Convert.ToInt64(ConfigMosgoldRate) > 100)
            {
                MessageBox("大奖抽取比例不能大于100");
                return;
            }


            string ConfigMosgoldDispatchRate = txtConfigMosgoldDispatchRate.Text;
            if (string.IsNullOrEmpty(ConfigMosgoldDispatchRate))
            {
                MessageBox("派奖概率不能为空");
                return;
            }

            if (!Utils.Validate.IsPositiveInt(ConfigMosgoldDispatchRate))
            {
                MessageBox("派奖概率格式错误");
                return;
            }

            if (Convert.ToInt64(ConfigMosgoldDispatchRate) > 100)
            {
                MessageBox("派奖概率不能大于100");
                return;
            }

            storage.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldStorageRate = Convert.ToInt64(ConfigMosgoldStorageRate);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldRate = Convert.ToInt64(ConfigMosgoldRate);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldDispatchRate = Convert.ToInt64(ConfigMosgoldDispatchRate);

            storage.Status = (int)OperStorgeType.修改子项;
            storage.ShareStorageItem.ModifySubItemType = (int)UpateStorgeSubType.大奖子项;

            SendMessageToServer();
            if (storage.Status == (int)OperStorgeType.修改子项结果失败)
            {
                MessageBox(storage.ResultMessage);
            }
            else
            {
                MessageBox("操作成功");
            }

        }

        /// <summary>
        /// 修改库存配置
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnStorge_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "修改库存配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            string ConfigSysStorage = txtConfigSysStorage.Text;
            if (string.IsNullOrEmpty(ConfigSysStorage))
            {
                MessageBox("系统库存不能为空");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(ConfigSysStorage))
            {
                MessageBox("系统库存格式错误");
                return;
            }


            string ConfigPlayerStorage = txtConfigPlayerStorage.Text;
            if (string.IsNullOrEmpty(ConfigPlayerStorage))
            {
                MessageBox("玩家库存不能为空");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(ConfigPlayerStorage))
            {
                MessageBox("玩家库存格式错误");
                return;
            }

            string ConfigParameterK = txtConfigParameterK.Text;
            if (string.IsNullOrEmpty(ConfigParameterK))
            {
                MessageBox("配置系数不能为空");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(ConfigParameterK))
            {
                MessageBox("配置系数格式错误");
                return;
            }
            if (Convert.ToInt64(ConfigParameterK) > 100)
            {
                MessageBox("配置系数不能大于100");
                return;
            }

            string ConfigSysStorResetRate = txtConfigSysStorResetRate.Text;
            if (string.IsNullOrEmpty(ConfigSysStorResetRate))
            {
                MessageBox("强制重置区间不能为空");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(ConfigSysStorResetRate))
            {
                MessageBox("强制重置区间格式错误");
                return;
            }

            if (Convert.ToInt64(ConfigSysStorResetRate) > 100)
            {
                MessageBox("强制重置区间不能大于100");
                return;
            }

            storage.ShareStorageItem.ConfigSubStorageItem.ConfigSysStorage = Convert.ToInt64(ConfigSysStorage);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigPlayerStorage = Convert.ToInt64(ConfigPlayerStorage);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigParameterK = Convert.ToInt64(ConfigParameterK);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigSysStorResetRate = Convert.ToInt64(ConfigSysStorResetRate);

            storage.Status = (int)OperStorgeType.修改子项;
            storage.ShareStorageItem.ModifySubItemType = (int)UpateStorgeSubType.库存子项;

            SendMessageToServer();
            if (storage.Status == (int)OperStorgeType.修改子项结果失败)
            {
                MessageBox(storage.ResultMessage);
            }
            else
            {
                MessageBox("操作成功");
            }
        }

        /// <summary>
        /// 修改临时库存配置
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>

        protected void btnTempStorge_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "修改临时库存配置";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            string ConfigTempSysStorage = txtConfigTempSysStorage.Text;
            if (string.IsNullOrEmpty(ConfigTempSysStorage))
            {
                MessageBox("临时系统库存不能为空");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(ConfigTempSysStorage))
            {
                MessageBox("临时系统库存格式错误");
                return;
            }


            string ConfigTempPlayerStorage = txtConfigTempPlayerStorage.Text;
            if (string.IsNullOrEmpty(ConfigTempPlayerStorage))
            {
                MessageBox("临时玩家库存不能为空");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(ConfigTempPlayerStorage))
            {
                MessageBox("临时玩家库存比例格式错误");
                return;
            }

            string ConfigTempParameterK = txtConfigTempParameterK.Text;
            if (string.IsNullOrEmpty(ConfigTempParameterK))
            {
                MessageBox("临时配置系数不能为空");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(ConfigTempParameterK))
            {
                MessageBox("临时配置系数格式错误");
                return;
            }
            if (Convert.ToInt64(ConfigTempParameterK) > 100)
            {
                MessageBox("临时配置系数不能大于100");
                return;
            }

            string ConfigTempSysStorResetRate = txtConfigTempSysStorResetRate.Text;
            if (string.IsNullOrEmpty(ConfigTempSysStorResetRate))
            {
                MessageBox("临时强制重置区间不能为空");
                return;
            }
            if (!Utils.Validate.IsPositiveInt(ConfigTempSysStorResetRate))
            {
                MessageBox("临时强制重置区间格式错误");
                return;
            }

            if (Convert.ToInt64(ConfigTempSysStorResetRate) > 100)
            {
                MessageBox("临时强制重置区间不能大于100");
                return;
            }

            storage.ShareStorageItem.ConfigSubStorageItem.ConfigTempSysStorage = Convert.ToInt64(ConfigTempSysStorage);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigTempPlayerStorage = Convert.ToInt64(ConfigTempPlayerStorage);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigTempParameterK = Convert.ToInt64(ConfigTempParameterK);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigTempSysStorResetRate = Convert.ToInt64(ConfigTempSysStorResetRate);

            storage.Status = (int)OperStorgeType.修改子项;
            storage.ShareStorageItem.ModifySubItemType = (int)UpateStorgeSubType.临时库存子项;

            SendMessageToServer();
            if (storage.Status == (int)OperStorgeType.修改子项结果失败)
            {
                MessageBox(storage.ResultMessage);
            }
            else
            {
                MessageBox("操作成功");
            }
        }


        protected void SendMessageToServer()
        {

            
            //向服务器发送消息
            SocketManage.aideWebSocketFacade.GetShareStorageARE = new AutoResetEvent(false);
            SocketManage.aideWebSocketFacade.UpdateOrGetShareStorage(storage, isSent => { });
            SocketManage.aideWebSocketFacade.GetShareStorageARE.WaitOne();

            storage = new ShareStorage();

            if (SocketManage.aideWebSocketFacade.storage != null)
            {
                storage = SocketManage.aideWebSocketFacade.storage;
            }

            BindData();
        }

        protected void btnRefresh_Click(object sender, EventArgs e)
        {
            storage.Status = (int)OperStorgeType.客户端查询子项;
            storage.ShareStorageItem.GroupID = groupId;
            storage.ShareStorageItem.KindID = (short)kindId;
            SendMessageToServer();
        }
    }
}