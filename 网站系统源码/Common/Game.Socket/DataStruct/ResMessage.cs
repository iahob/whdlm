using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Game.Socket.DataStruct
{
    public class ResMessage
    {
        /// <summary>
        /// 返回结果
        /// </summary>
        public int Code { get; set; }

        /// <summary>
        /// 返回信息
        /// </summary>
        public string Message { get; set; }
    }
}
