using Game.Utils;
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Threading;
using Game.Socket.DataStruct;
using WebSocketSharp;
using Game.Entity.Platform;
using Game.Entity.Accounts;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.InteropServices;
using Game.Facade;

// ReSharper disable InconsistentNaming

namespace Game.Socket
{
    /// <summary>
    /// 代理外观
    /// </summary>
    public class WebSocketFacade
    {
        #region Fields

        private WebSocket _ws;
        public string SocketUrl { get; set; } = SocketConfig.Get("SocketUrl");
        public string SocketVerifyCode { get; set; } = SocketConfig.Get("SocketVerifyCode");

        public int SocketTimeOut { get; set; } = string.IsNullOrEmpty(SocketConfig.Get("SocketTimeOut"))
            ? 15000
            : Convert.ToInt32(SocketConfig.Get("SocketTimeOut"));

        public AutoResetEvent ServerTestARE { get; set; } = new AutoResetEvent(false);
        public AutoResetEvent GetLinkServerARE { get; set; } = new AutoResetEvent(false);
        public AutoResetEvent UpdateSuccessARE { get; set; } = new AutoResetEvent(false);
        public AutoResetEvent OperateResultARE { get; set; } = new AutoResetEvent(false);

        public AutoResetEvent GetShareStorageARE { get; set; } = new AutoResetEvent(false);

        public bool UpdateSuccess { get; set; } = false;
        public List<ServerInfo> ServerList = new List<ServerInfo>();

        public ShareStorage storage = null;

        /// <summary>
        /// 返回结果
        /// </summary>
        public int Code { get; set; } = -1;

        /// <summary>
        /// 返回信息
        /// </summary>
        public string Message { get; set; } = "消息获取失败";


        #endregion Fields

        #region 构造函数

        /// <summary>
        /// 构造函数
        /// </summary>
        public WebSocketFacade()
        {
            if (string.IsNullOrEmpty(SocketUrl))
            {
                TextLogger.Write("WebSocketFacade Init Failure because no config!", "WebSocket");
                return;
            }

            _ws = WebSocketHelper.CreateSocket(SocketUrl, SocketVerifyCode);

            WebSocketEvent.EventGetLinkServer += data =>
            {
                TextLogger.Write("EventGetLinkServer Event Start", "WebSocket");
                ServerList.Clear();
                var count = WebSocketHelper.GetShort(data.RData);
                for (var i = 0; i < count; i++)
                {
                    var startIndex = i * 66 + 2;
                    ServerList.Add(
                        new ServerInfo
                        {
                            Port = WebSocketHelper.GetShort(data.RData, 0 + startIndex),
                            ServerIp = WebSocketHelper.GetString(data.RData, 2 + startIndex, 64)
                        });
                }

                TextLogger.Write($"GetLinkServer AutoResetLinkServerEvent Set {ServerList.Count}", "WebSocket");
                GetLinkServerARE.Set();
                TextLogger.Write("EventGetLinkServer Event End", "WebSocket");
            };

            WebSocketEvent.EventUpdateSuccess += (data, isSuccess) =>
            {
                TextLogger.Write("EventUpdateSuccess Event Start", "WebSocket");
                UpdateSuccess = isSuccess;
                UpdateSuccessARE.Set();
                TextLogger.Write($"EventUpdateSuccess Event End With isSuccess:{isSuccess}", "WebSocket");
            };

            WebSocketEvent.EventHttpHelper += data =>
            {
                TextLogger.Write("EventHttpHelper Event Start", "WebSocket");

                TextLogger.Write("EventHttpHelper Event End", "WebSocket");
            };

            WebSocketEvent.EventOperateResult += WebSocketEvent_EventOperateResult;

            WebSocketEvent.EventGetShareStorage += WebSocketEvent_EventGetShareStorage;
        }

        /// <summary>
        /// 处理接收共享库存实体
        /// </summary>
        /// <param name="data"></param>
        private void WebSocketEvent_EventGetShareStorage(WebSocketData data)
        {
            TextLogger.Write("GetShareStorage Event Start", "WebSocket");

            storage = new ShareStorage();
            var Rdata = data.RData;
            storage.GameServerRegister = WebSocketHelper.GetBool(Rdata, 0);
            storage.Status = WebSocketHelper.GetInt(Rdata, 1, 4);
            storage.ShareStorageItem.ItemID= WebSocketHelper.GetInt(Rdata, 5, 8);
            storage.ShareStorageItem.GroupID = WebSocketHelper.GetInt(Rdata, 13, 4);
            storage.ShareStorageItem.KindID = WebSocketHelper.GetShort(Rdata, 17, 2);
            storage.ShareStorageItem.PlayMode = WebSocketHelper.GetByte(Rdata, 19, 1);
            storage.ShareStorageItem.ModifySubItemType = WebSocketHelper.GetInt(Rdata, 20, 4);

            storage.ShareStorageItem.ConfigSubStorageItem.ConfigSysStorage = WebSocketHelper.GetInt(Rdata, 24, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigPlayerStorage = WebSocketHelper.GetInt(Rdata, 32, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigParameterK = WebSocketHelper.GetInt(Rdata, 40, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigSysStorResetRate = WebSocketHelper.GetInt(Rdata, 48, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigAnChouRate = WebSocketHelper.GetInt(Rdata, 56, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldRate = WebSocketHelper.GetInt(Rdata, 64, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldDispatchRate = WebSocketHelper.GetInt(Rdata, 72, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldStorageRate = WebSocketHelper.GetInt(Rdata, 80, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigTempSysStorage = WebSocketHelper.GetInt(Rdata, 88, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigTempPlayerStorage = WebSocketHelper.GetInt(Rdata, 96, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigTempParameterK = WebSocketHelper.GetInt(Rdata, 104, 8);
            storage.ShareStorageItem.ConfigSubStorageItem.ConfigTempSysStorResetRate = WebSocketHelper.GetInt(Rdata, 112, 8);

            storage.ShareStorageItem.CurSubStorageItem.CurStatisticSysWin= WebSocketHelper.GetInt(Rdata, 120, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurStatisticService = WebSocketHelper.GetInt(Rdata, 128, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurSysStorage = WebSocketHelper.GetInt(Rdata, 136, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurPlayerStorage = WebSocketHelper.GetInt(Rdata, 144, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurParameterK = WebSocketHelper.GetInt(Rdata, 152, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurSysStorResetRate = WebSocketHelper.GetInt(Rdata, 160, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurWinChance = WebSocketHelper.GetInt(Rdata, 168, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurResetCount = WebSocketHelper.GetInt(Rdata, 176, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurSysWin = WebSocketHelper.GetInt(Rdata, 184, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurAnChouRate = WebSocketHelper.GetInt(Rdata, 192, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurAnChouTotal = WebSocketHelper.GetInt(Rdata, 200, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurMosgoldRate = WebSocketHelper.GetInt(Rdata, 208, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurMosgoldDispatchRate = WebSocketHelper.GetInt(Rdata, 216, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurMosgoldStorageRate = WebSocketHelper.GetInt(Rdata, 224, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurTempSysStorage = WebSocketHelper.GetInt(Rdata, 232, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurTempPlayerStorage = WebSocketHelper.GetInt(Rdata, 240, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurTempParameterK = WebSocketHelper.GetInt(Rdata, 248, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurTempSysStorResetRate = WebSocketHelper.GetInt(Rdata, 256, 8);
            storage.ShareStorageItem.CurSubStorageItem.CurTempWinChance = WebSocketHelper.GetInt(Rdata, 264, 8);
            storage.ResultMessage = WebSocketHelper.GetString(Rdata, 272, 512);

            GetShareStorageARE.Set();
            TextLogger.Write("GetShareStorage Event End", "WebSocket");
        }



        /// <summary>
        /// 操作返回结果
        /// </summary>
        /// <param name="data"></param>
        private void WebSocketEvent_EventOperateResult(WebSocketData data)
        {
            TextLogger.Write("OperateResult Event Start", "WebSocket");

            Code = WebSocketHelper.GetByte(data.RData, 6, 1);
            Message = WebSocketHelper.GetString(data.RData, 7);
            OperateResultARE.Set();

            TextLogger.Write("OperateResult Event End", "WebSocket");
        }

        public bool Init()
        {
            if (_ws != null && _ws.ReadyState == WebSocketState.Open) return true;
            TextLogger.Write($"ws ReConnect To {SocketUrl}", "WebSocket");
            _ws = WebSocketHelper.CreateSocket(SocketUrl, SocketVerifyCode);
            if (_ws.ReadyState == WebSocketState.Open)
            {
                return true;
            }
            return false;
        }

        #endregion 构造函数

        public void ServerTest(Action<bool> cbAction)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            SendData(CMD_Main.Main_WebService, CMD_Sub.ServerTest, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                ServerTestARE.Set();
            });
        }


        /// <summary>
        /// 更新配置
        /// </summary>
        /// <param name="type"></param>
        /// <param name="cbAction">委托（回调）</param>
        public void UpdateParamType(short type, Action<bool> cbAction = null)
        {
            if(!Init()) return;
            var bufferList = new List<byte>();
            bufferList.AddRange(BitConverter.GetBytes(type));
            TextLogger.Write(
                $"UpdateParamType {WebSocketHelper.GetParamTypeDesc(type)}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateParamter, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                UpdateSuccessARE.Set();
            });
        }

        /// <summary>
        /// 更新玩家财富
        /// </summary>
        /// <param name="userId"></param>
        /// <param name="type"></param>
        /// <param name="cbAction">委托（回调）</param>
        public void UpdateUserScore(int userId, short type, Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            bufferList.AddRange(BitConverter.GetBytes(userId));
//            bufferList.AddRange(BitConverter.GetBytes(type));
            TextLogger.Write($"UpdateUserScore UserID:{userId} Type:{WebSocketHelper.GetUserScoreTypeDesc(type)}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateUserScore, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                UpdateSuccessARE.Set();
            });
        }

        public void UpdateMailMessage(List<int> userIds, Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            bufferList.AddRange(BitConverter.GetBytes(Convert.ToInt16(userIds.Count)));
            foreach (var userId in userIds)
            {
                bufferList.AddRange(BitConverter.GetBytes(userId));
            }

            //            bufferList.AddRange(BitConverter.GetBytes(type));
            TextLogger.Write($"UpdateMailMessage userIdsCount:{userIds.Count}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateMailMessage, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                UpdateSuccessARE.Set();
            });
        }

        /// <summary>
        /// 更新大联盟最大人数
        /// </summary>
        /// <param name="groupId">大联盟ID 0：所有</param>
        /// <param name="count">人数</param>
        /// <param name="cbAction">委托（回调）</param>
        public void UpdateGroupMaxMember(int groupId, short count, Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            bufferList.AddRange(BitConverter.GetBytes(groupId));
            bufferList.AddRange(BitConverter.GetBytes(count));
            TextLogger.Write($"UpdateGroupMaxMember groupId:{groupId} count:{count}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateGroupMaxMember, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                UpdateSuccessARE.Set();
            });
        }




        /// <summary>
        /// 更新或者获取共享库存
        /// </summary>
        /// <param name="shareStorge">共享库存</param>
        /// <param name="cbAction">委托（回调）</param>
        public void UpdateOrGetShareStorage(ShareStorage shareStorge, Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.Status));

            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ItemID));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.GroupID));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.KindID));
            bufferList.Add((byte)1);
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ModifySubItemType));

            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigSysStorage));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigPlayerStorage));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigParameterK));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigSysStorResetRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigAnChouRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldDispatchRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigMosgoldStorageRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigTempSysStorage));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigTempPlayerStorage));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigTempParameterK));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.ConfigSubStorageItem.ConfigTempSysStorResetRate));

            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurStatisticSysWin));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurStatisticService));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurSysStorage));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurPlayerStorage));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurParameterK));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurSysStorResetRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurWinChance));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurResetCount));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurSysWin));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurAnChouRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurAnChouTotal));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurMosgoldRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurMosgoldDispatchRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurMosgoldStorageRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurTempSysStorage));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurTempPlayerStorage));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurTempParameterK));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurTempSysStorResetRate));
            bufferList.AddRange(BitConverter.GetBytes(shareStorge.ShareStorageItem.CurSubStorageItem.CurTempWinChance));

            TextLogger.Write($"UpdateOrGetShareStorage groupId:{shareStorge.ShareStorageItem.GroupID} count:{shareStorge.ShareStorageItem.KindID}," +
                             $"Status:{shareStorge.Status},ModifySubItemType:{shareStorge.ShareStorageItem.ModifySubItemType} ",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateShareStorage, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                GetShareStorageARE.Set();
            });
        }


        /// <summary>
        /// 更新AI人数权重
        /// </summary>
        /// <param name="cbAction">委托（回调）</param>
        public void UpdateAiParamter(byte Status,GroupAIParameter aiPara,Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>() { Status };
            bufferList.AddRange(BitConverter.GetBytes(aiPara.GroupID));
            bufferList.AddRange(BitConverter.GetBytes(aiPara.DistributeTotalScore));
            bufferList.AddRange(BitConverter.GetBytes(aiPara.AICount));
            bufferList.AddRange(BitConverter.GetBytes(aiPara.KindID));
            bufferList.AddRange(BitConverter.GetBytes(aiPara.PlayingAIRateMin));
            bufferList.AddRange(BitConverter.GetBytes(aiPara.PlayingAIRateMax));
            bufferList.AddRange(BitConverter.GetBytes(aiPara.DistirbuteSingleScore));
            bufferList.AddRange(BitConverter.GetBytes(aiPara.DistirbuteScoreMinRatio));
            bufferList.AddRange(BitConverter.GetBytes(aiPara.DistirbuteScoreMaxRatio));


            TextLogger.Write($"UpdateAiParamter Status:{Status}, GroupID:{aiPara.GroupID}, DistributeTotalScore:{aiPara.DistributeTotalScore}," +
                             $"AICount:{aiPara.AICount},PlayingAIRateMin:{aiPara.PlayingAIRateMin},PlayingAIRateMax:{aiPara.PlayingAIRateMax}," +
                             $"DistirbuteSingleScore:{aiPara.DistirbuteSingleScore},DistirbuteScoreMinRatio:{aiPara.DistirbuteScoreMinRatio}," +
                             $"DistirbuteScoreMaxRatio:{aiPara.DistirbuteScoreMaxRatio}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateAIPARAMTER, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                OperateResultARE.Set();
            });
        }

        /// <summary>
        /// 更新AI离开权重配置
        /// </summary>
        /// <param name="cbAction">委托（回调）</param>
        public void UpdateGroupAIWeightOut(AIWeightOutRatio AIWO, Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            bufferList.AddRange(BitConverter.GetBytes(AIWO.ID));
            bufferList.AddRange(BitConverter.GetBytes(AIWO.TimeCount));
            bufferList.AddRange(BitConverter.GetBytes(AIWO.WeightOut));
            TextLogger.Write($"UpdateGroupAIWeightOut ID:{AIWO.ID},TimeCount:{AIWO.TimeCount},WeightOut:{AIWO.WeightOut}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateAIWEIGHTOUT, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                UpdateSuccessARE.Set();
            });
        }

        /// <summary>
        /// 更新AI进入房间配置
        /// </summary>
        /// <param name="cbAction">委托（回调）</param>
        public void UpdateGroupAIWeightIn(AIWeightInRatio AIWI, Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            bufferList.AddRange(BitConverter.GetBytes(AIWI.ID));
            bufferList.AddRange(BitConverter.GetBytes(AIWI.TimeCount));
            bufferList.AddRange(BitConverter.GetBytes(AIWI.WeightIn));
            TextLogger.Write($"UpdateGroupAIWeightIn ID:{AIWI.ID},TimeCount:{AIWI.TimeCount},WeightIn:{AIWI.WeightIn}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateAIWEIGHTIN, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                UpdateSuccessARE.Set();
            });
        }

        /// <summary>
        /// 更新AI人数权重
        /// </summary>
        /// <param name="cbAction">委托（回调）</param>
        public void UpdateAiWeightTotal(AITimeChange AITC, Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            bufferList.AddRange(BitConverter.GetBytes(AITC.ID));
            bufferList.AddRange(BitConverter.GetBytes(AITC.AITime));
            bufferList.AddRange(BitConverter.GetBytes(AITC.AICount));


            TextLogger.Write($"UpdateAiWeightTotal ID:{AITC.ID},AITime:{AITC.AITime},AICount:{AITC.AICount}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_UpdateAIWEIGHTTOTAL, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                UpdateSuccessARE.Set();
            });
        }

        /// <summary>
        /// 强制下线AI，强制退出AI
        /// </summary>
        /// <param name="cbAction">委托（回调）</param>
        public void DelteAI(byte OperateType,int GroupId,string[] userList, Action<bool> cbAction = null)
        {
            if (!Init()) return;
            var bufferList = new List<byte>();
            bufferList.Add(OperateType);
            bufferList.AddRange(BitConverter.GetBytes(GroupId));
            for (int i = 0; i < userList.Length; i++)
            {
                short kindId = (short)FacadeManage.aideAccountsFacade.GetAIParameterKindId(GroupId, Convert.ToInt32(userList[i]));
                bufferList.AddRange(BitConverter.GetBytes(kindId));
                bufferList.AddRange(BitConverter.GetBytes(Convert.ToInt32(userList[i])));
                bufferList.Add((byte)(kindId > 0 ? 1 : 0));

            }
            //补充数据
            if (20 - userList.Length > 0)
            {
                int count = 20 - userList.Length;
                for (int i = 0; i < count; i++)
                {
                    bufferList.AddRange(BitConverter.GetBytes((short)0));
                    bufferList.AddRange(BitConverter.GetBytes(0));
                    bufferList.Add(0);
                }
            }

            TextLogger.Write($"DelteAI OperateType:{OperateType},GroupId:{GroupId}",
                "WebSocket");
            SendData(CMD_Main.Main_WebService, CMD_Sub.Sub_Req_DELETEAI, bufferList, isSent =>
            {
                cbAction?.Invoke(isSent);
                Thread.Sleep(SocketTimeOut);
                OperateResultARE.Set();
            });
        }


        /// <summary>
        /// ReWrite SendData
        /// </summary>
        /// <param name="main">主命令</param>
        /// <param name="sub">子命令</param>
        /// <param name="byteArr">参数</param>
        /// <param name="cbAction">委托（回调）</param>
        private void SendData(CMD_Main main, CMD_Sub sub, List<byte> byteArr, Action<bool> cbAction)
        {
            WebSocketHelper.SendData(main, sub, byteArr.ToArray(), false,
                cbAction);
        }


    }
}