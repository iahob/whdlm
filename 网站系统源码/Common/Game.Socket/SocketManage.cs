// ReSharper disable InconsistentNaming

using Game.Utils;

namespace Game.Socket
{
    /// <summary>
    /// 逻辑层管理类
    /// </summary>
    public class SocketManage
    {
        private static readonly object lockObj = new object();

        /// <summary>
        /// WebSocket逻辑
        /// </summary>
        private static volatile WebSocketFacade _aideWebSocketFacade;
        public static WebSocketFacade aideWebSocketFacade
        {
            get
            {
                TextLogger.Write("进来了");
                if (_aideWebSocketFacade != null) return _aideWebSocketFacade;

                lock (lockObj)
                {
                    TextLogger.Write("对象不存在");
                    if (_aideWebSocketFacade == null)
                        _aideWebSocketFacade = new WebSocketFacade();
                }
                return _aideWebSocketFacade;
            }
        }
    }
}
