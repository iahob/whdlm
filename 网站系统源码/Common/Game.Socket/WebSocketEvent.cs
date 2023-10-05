using Game.Socket.DataStruct;

namespace Game.Socket
{
    public class WebSocketEvent
    {
        //获取最优链接服务器
        public delegate void dgGetLinkServer(WebSocketData data);
        public static event dgGetLinkServer EventGetLinkServer;

        //更新成功
        public delegate void dgUpdateSuccess(WebSocketData data,bool isSuccess);
        public static event dgUpdateSuccess EventUpdateSuccess;

        public delegate void dgHttpHelper(WebSocketData data);
        public static event dgHttpHelper EventHttpHelper;

        public delegate void dgOperateResult(WebSocketData data);
        public static event dgOperateResult EventOperateResult;

        public delegate void dgGetShareStorage(WebSocketData data);
        public static event dgGetShareStorage EventGetShareStorage;

        public static void PushMessage(WebSocketData data)
        {
            if (data == null) return;

            switch ((CMD_Main)data.wMainCmdID)
            {
                case CMD_Main.Main_WebService:
                    switch ((CMD_Sub)data.wSubCmdID)
                    {
                        case CMD_Sub.Sub_Req_HttpHelper:
                            EventHttpHelper?.Invoke(data);
                            break;
                        case CMD_Sub.Sub_Resp_UpdateSuccess:
                            EventUpdateSuccess?.Invoke(data,true);
                            break;
                        case CMD_Sub.Sub_Resp_UpdateFailure:
                            EventUpdateSuccess?.Invoke(data, false);
                            break;
                        case CMD_Sub.Sub_Resp_OperateResult:
                            EventOperateResult?.Invoke(data);
                            break;
                        case CMD_Sub.Sub_Resq_GetShareStorage:
                            EventGetShareStorage?.Invoke(data);
                            break;
                    }
                    break;
            }
        }

    }
}