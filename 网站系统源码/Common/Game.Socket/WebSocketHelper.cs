using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using Game.Socket.DataStruct;
using Game.Utils;
using WebSocketSharp;

namespace Game.Socket
{
    public class WebSocketHelper
    {
        public static WebSocketState SocketState { get; set; }

        public static WebSocket Socket;


        private static string SocketUrl { get; set; }

        private static string SocketVerifyCode { get; set; }

        //创建socket链接
        public static WebSocket CreateSocket(string socketUrl = "", string socketVerifyCode = "")
        {
            if (!string.IsNullOrEmpty(socketUrl)) SocketUrl = socketUrl;
            if (!string.IsNullOrEmpty(socketUrl)) SocketVerifyCode = socketVerifyCode;
            //已存在连接
            if (Socket != null && Socket.ReadyState == WebSocketState.Open)
            {
                return Socket;
            }

            TextLogger.Write($"ws Connect To {SocketUrl}", "WebSocket");
            //创建连接
            Socket = new WebSocket(SocketUrl);
            Socket.OnOpen += Socket_OnOpen;
            Socket.OnMessage += Socket_OnMessage;
            Socket.OnError += Socket_OnError;
            Socket.OnClose += Socket_OnClose;

            Socket.Connect();
            return Socket;
        }

        private static void Socket_OnMessage(object sender, MessageEventArgs e)
        {
            var data = ResolveMessage(e.RawData);
        }

        private static void Socket_OnClose(object sender, CloseEventArgs e)
        {
            TextLogger.Write("ws OnClose " + e.Reason, "WebSocket");
        }

        private static void Socket_OnError(object sender, ErrorEventArgs e)
        {
            TextLogger.Write("ws OnError msg:" + e.Message, "WebSocket");
        }

        /// <summary>
        /// Socket链接成功时触发事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private static void Socket_OnOpen(object sender, EventArgs e)
        {
            TextLogger.Write("ws OnOpen", "WebSocket");
            SocketState = Socket.ReadyState;

            //发送校验消息
            var validBuff = StrToByte(SocketVerifyCode, 128);
            SendData(CMD_Main.Main_Tcp, CMD_Sub.Sub_Req_Valid, validBuff, true);
        }


        /// <summary>
        /// 发送消息
        /// </summary>
        /// <param name="wMainCmdMainId">主命令</param>
        /// <param name="wSubCmdId">子命令</param>
        /// <param name="byteArray">消息体</param>
        /// <param name="isFirst">是否第一次发送</param>
        /// <param name="cbAction"></param>
        public static void SendData(CMD_Main wMainCmdMainId, CMD_Sub wSubCmdId, byte[] byteArray = null,
            bool isFirst = false,
            Action<bool> cbAction = null)
        {
            var data = new WebSocketData
            {
                wMainCmdID = (short) wMainCmdMainId,
                wSubCmdID = (short) wSubCmdId,
                wDataType = 2,
                cbCheckCode = 0,
                IsFirst = isFirst
            };

            data.SetPushData(byteArray);
            TextLogger.Write(
                $"ws SendData MainCmd:{GetCMDMainDesc(data.wMainCmdID)}({data.wMainCmdID}) SubCmd:{GetCMDSubDesc(data.wSubCmdID)}({data.wSubCmdID}) dataSize:{data.wDataSize}",
                "WebSocket");
            Socket.SendAsync(data.GetSendData(), cbAction);
        }

        /// <summary>
        /// 解析和处理消息
        /// </summary>
        /// <param name="returnData"></param>
        public static WebSocketData ResolveMessage(byte[] returnData)
        {
            var data = ResolveHead(returnData);
            TextLogger.Write(
                $"ws OnMessage MainCmd:{GetCMDMainDesc(data.wMainCmdID)}({data.wMainCmdID}) SubCmd:{GetCMDSubDesc(data.wSubCmdID)}({data.wSubCmdID})  dataSize:{data.wDataSize}",
                "WebSocket");
            //判断是否读取错误信息 先判断主命令 再判断副命令
            switch ((CMD_Main) data.wMainCmdID)
            {
                case CMD_Main.Main_WebService:
                    switch ((CMD_Sub) data.wSubCmdID)
                    {
                        default:
                            break;
                    }

                    break;
                default:
                    break;
            }

            //消息分发
            WebSocketEvent.PushMessage(data);
            return data;
        }

        /// <summary>
        /// 解析消息头
        /// </summary>
        /// <param name="dataBuffer"></param>
        /// <returns></returns>
        public static WebSocketData ResolveHead(byte[] dataBuffer)
        {
            var data = new WebSocketData
            {
                cbCheckCode = (byte) GetShort(dataBuffer),
                wDataSize = GetShort(dataBuffer, 2),
                wMainCmdID = GetShort(dataBuffer, 4),
                wSubCmdID = GetShort(dataBuffer, 6),
                RData = dataBuffer.Skip(8).ToArray()
            };

            return data;
        }


        /// <summary>
        /// 处理错误消息
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static WebSocketData DealErrMessage(WebSocketData data)
        {
            if (data.RData.Length <= 0) return data;
            data.ErrCode =GetInt(data.RData);
            data.ErrMessage = GetString(data.RData,4);
            data.IsError = true;

            return data;
        }


        #region 数据处理

        public static byte[] GetBytes(byte[] rData, int skip, int take)
        {
            var temp = rData.Skip(skip);
            if (take>0) temp = temp.Take(take);
            return temp.ToArray();
        }

        public static byte GetByte(byte[] rData, int skip = 0, int length = 1)
        {
            return GetBytes(rData, skip, length)[0];
        }

        public static bool GetBool(byte[] rData, int skip = 0, int length = 1)
        {
            return BitConverter.ToBoolean(GetBytes(rData, skip, length),0);
        }

        public static short GetShort(byte[] rData, int skip = 0, int length = 2)
        {
            return BitConverter.ToInt16(GetBytes(rData, skip, length), 0);
        }

        public static int GetInt(byte[] rData, int skip = 0, int length = 4)
        {
            return BitConverter.ToInt32(GetBytes(rData, skip, length), 0);
        }

        public static long GetLong(byte[] rData, int skip = 0, int length = 8)
        {
            return BitConverter.ToInt64(GetBytes(rData, skip, length), 0);
        }

        public static string GetString(byte[] rData, int skip = 0, int length=0)
        {
            return ByteToStr(GetBytes(rData, skip, length));
        }


        /// <summary>
        /// 把字符串转换为双字节byte[]
        /// </summary>
        /// <param name="str"></param>
        /// <param name="dataSize"></param>
        /// <returns></returns>
        public static byte[] StrToByte(string str, int dataSize)
        {
            var dataBuffer = new byte[dataSize];
            //List<byte> vb = new List<byte>();
            //foreach (char c in str)
            //{
            //    byte[] vdb = BitConverter.GetBytes(c);
            //    vb.AddRange(vdb);
            //}
            var buffer = Encoding.Unicode.GetBytes(str);
            Array.Copy(buffer, dataBuffer, buffer.Length);
            return dataBuffer;
        }

        public static string ByteToStr(byte[] dataBuffer)
        {
//            var dataBuff = new byte[dataBuffer.Length / 2];
            var strValue = "";
            for (var i = 0; i < dataBuffer.Length; i += 2)
            {
                var item = dataBuffer.Skip(i).Take(2).ToArray();
                if (item[0] == '\0' && item[1] == '\0')
                {
                    break;
                }

                //char cValue = byteToChar(item);
                strValue += Encoding.Unicode.GetString(item);
            }

            //byte[] ActDataBuff = getCopyByte(DataBuff);
            //string str = new string(temp);
            return strValue;
        }


        /// <summary>
        /// 将<c>Decimal</c>对象转换为字节数组。
        /// </summary>
        /// <param name="value">要转换的对象。</param>
        /// <returns>表示<c>Decimal</c>对象的字节数组。</returns>
        public static byte[] ToBytes(decimal value)
        {
            var bits = decimal.GetBits(value);
            var bytes = new byte[bits.Length * 2];
            for (var i = 0; i < bits.Length; i++)
            {
                for (var j = 0; j < 4; j++)
                {
                    bytes[i * 4 + j] = (byte) (bits[i] >> (j * 8));
                }
            }

            return bytes;
        }


        /// <summary>  
        /// 由byte数组转换为结构体  
        /// </summary>  
        public static T ByteToStructure<T>(byte[] dataBuffer)
        {
            object structure;
            var size = Marshal.SizeOf(typeof(T));
            var allocIntPtr = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.Copy(dataBuffer, 0, allocIntPtr, size);
                structure = Marshal.PtrToStructure(allocIntPtr, typeof(T));
            }
            finally
            {
                Marshal.FreeHGlobal(allocIntPtr);
            }

            return (T) structure;
        }


        /// <summary>  
        /// 由结构体转换为byte数组  
        /// </summary>  
        public static byte[] StructureToByte<T>(T structure)
        {
            var size = Marshal.SizeOf(typeof(T));
            var buffer = new byte[size];
            var bufferIntPtr = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.StructureToPtr(structure, bufferIntPtr, true);
                Marshal.Copy(bufferIntPtr, buffer, 0, size);
            }
            finally
            {
                Marshal.FreeHGlobal(bufferIntPtr);
            }

            return buffer;
        }


        public static int GetValidLength(byte[] bytes)
        {
            var i = 0;
            if (null == bytes || 0 == bytes.Length)
                return i;
            for (; i < bytes.Length; i++)
            {
                if (bytes[i] == '\0')
                    break;
            }

            return i;
        }

        public static byte[] GetCopyByte(byte[] bytes)
        {
            if (null == bytes || 0 == bytes.Length)
                return new byte[1];
            var length = GetValidLength(bytes);
            var bb = new byte[length];
            Array.Copy(bytes, 0, bb, 0, length);
            return bb;
        }

        public static char ByteToChar(byte[] b)
        {
            if (b[0] == 0 && b[1] == 0) return new char();
            if (b[1] == 0) return BitConverter.ToChar(b, 0);
            var c = (char) (((b[0] & 0xFF) << 8) | (b[1] & 0xFF));
            return c;
        }

        /// <summary>
        /// 16进制转byte[]
        /// </summary>
        /// <param name="hexString"></param>
        /// <returns></returns>
        public static byte[] HexStrTobyte(string hexString)
        {
            hexString = hexString.Replace(" ",
                "");
            if ((hexString.Length % 2) != 0)
                hexString += " ";
            byte[] returnBytes = new byte[hexString.Length / 2];
            for (int i = 0; i < returnBytes.Length; i++)
                returnBytes[i] = Convert.ToByte(hexString.Substring(i * 2, 2).Trim(), 16);
            return returnBytes;
        }

        #endregion

        #region 枚举匹配

        public static string GetCMDMainDesc(short main)
        {
            return EnumDescription.GetFieldText((CMD_Main) main);
        }

        public static string GetCMDSubDesc(short sub)
        {
            return EnumDescription.GetFieldText((CMD_Sub) sub);
        }

        public static string GetParamTypeDesc(short type)
        {
            return EnumDescription.GetFieldText((UpdateParamterType) type);
        }

        public static string GetUserScoreTypeDesc(short type)
        {
            return EnumDescription.GetFieldText((UpdateUserScoreType) type);
        }

        #endregion
    }
}