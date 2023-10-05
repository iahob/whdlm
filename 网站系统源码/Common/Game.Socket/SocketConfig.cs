// ReSharper disable once CheckNamespace

using Game.Utils;

// ReSharper disable InconsistentNaming

namespace Game.Socket
{
    public class SocketConfig
    {
        #region 方法

        /// <summary>
        /// 获取配置
        /// </summary>
        /// <param name="configKey"></param>
        /// <returns></returns>
        public static string Get(string configKey)
        {
            try
            {
                return ApplicationSettings.Get(configKey);
            }
            catch
            {
                return "";
            }
        }

        #endregion
    }
}
