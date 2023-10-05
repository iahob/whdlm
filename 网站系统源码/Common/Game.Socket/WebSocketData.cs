using System;
using System.Collections.Generic;

// ReSharper disable InconsistentNaming

namespace Game.Socket
{
    public class WebSocketData
    {
        /// <summary>
        /// 主命令码	 2字节
        /// </summary>
        public short wMainCmdID { get; set; }

        /// <summary>
        /// 子命令码	 2字节
        /// </summary>
        public short wSubCmdID { get; set; }

        /// <summary>
        /// 数据类型  1字节
        /// </summary>
        public byte wDataType { get; set; }

        /// <summary>
        /// 效验字段  1字节
        /// </summary>
        public byte cbCheckCode { get; set; }

        /// <summary>
        /// 数据大小  2字节
        /// </summary>
        public short wDataSize { get; set; }

        /// <summary>
        /// 是否第一次发送
        /// </summary>
        public bool IsFirst { get; set; }


        /// <summary>
        /// 要发送的实际数据包
        /// </summary>
        public byte[] PData { get; set; }


        /// <summary>
        /// 获取到的实际数据包
        /// </summary>
        public byte[] RData { get; set; }


        /// <summary>
        /// 错误码
        /// </summary>
        public int ErrCode { get; set; }

        /// <summary>
        /// 错误信息
        /// </summary>
        public string ErrMessage { get; set; }

        /// <summary>
        /// 是否是错误消息
        /// </summary>
        public bool IsError { get; set; }


        /// <summary>
        /// 计算数据包的长度
        /// </summary>
        /// <param name="pData"></param>
        public void SetPushData(byte[] pData)
        {
            PData = pData;

            //if (pData == null)
            //{
            //    wDataSize = (short)(8 + (IsFirst ? 2 : 0));
            //}
            //else
            //{
            //    wDataSize = (short)(pData.Length + 8 + (IsFirst ? 2 : 0));
            //}
            if (pData == null)
            {
                wDataSize = (short)(8);
            }
            else
            {
                wDataSize = (short)(pData.Length + 8);
            }
        }

        /// <summary>
        /// 获取完整发送包
        /// </summary>
        /// <returns></returns>
        public byte[] GetSendData()
        {
            var byteSource = new List<byte> {cbCheckCode, wDataType};
            byteSource.AddRange(BitConverter.GetBytes(wDataSize));
            byteSource.AddRange(BitConverter.GetBytes(wMainCmdID));
            byteSource.AddRange(BitConverter.GetBytes(wSubCmdID));

            //if (IsFirst)
            //{
            //    byteSource.AddRange(BitConverter.GetBytes((short) 4));
            //}

            if (PData != null)
            {
                byteSource.AddRange(PData);
            }

            return byteSource.ToArray();
        }
    }
}