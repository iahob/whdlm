using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Game.Web.Helper
{
    public class JFTYCModel
    {
        public class ParameterModel
        {
            /// <summary>
            /// 商户号
            /// </summary>
            public string p1_merchant { get; set; }

            /// <summary>
            /// 订单号
            /// </summary>
            public string p2_order { get; set; }


            /// <summary>
            /// 金额
            /// </summary>
            public string p3_money { get; set; }


            /// <summary>
            /// 同步跳转地址
            /// </summary>
            public string p4_returnurl { get; set; }


            /// <summary>
            /// 异步通知地址
            /// </summary>
            public string p5_noticeurl { get; set; }


            /// <summary>
            /// 终端账号
            /// </summary>
            public string p6_custom { get; set; }


            /// <summary>
            /// 签名类型
            /// </summary>
            public string p7_signtype { get; set; }


            /// <summary>
            /// 签名
            /// </summary>
            public string p8_sign { get; set; }


            /// <summary>
            /// 支付类型
            /// </summary>
            public string p10_paytype { get; set; }


            /// <summary>
            /// 支付场景
            /// </summary>
            public string p11_payscene { get; set; }


            /// <summary>
            /// 表现形式
            /// </summary>
            public string p12_showtype { get; set; }

        }

        public class NoticeModel
        {
            /// <summary>
            /// 商户号
            /// </summary>
            public string p1_usercode { get; set; }

            /// <summary>
            /// 订单号
            /// </summary>
            public string p2_order { get; set; }

            /// <summary>
            /// 订单金额
            /// </summary>
            public string p3_ordermoney { get; set; }

            /// <summary>
            /// 状态
            /// </summary>
            public string p4_status { get; set; }


            /// <summary>
            /// 业务单号
            /// </summary>
            public string p5_tradeorder { get; set; }


            /// <summary>
            /// 支付类型
            /// </summary>
            public string p6_paymethod { get; set; }

            /// <summary>
            /// 交易金额
            /// </summary>
            public string p7_transamount { get; set; }

            /// <summary>
            /// 编码
            /// </summary>
            public string p8_charset { get; set; }

            /// <summary>
            /// 加密方式
            /// </summary>
            public string p9_signtype { get; set; }


            /// <summary>
            /// 签名
            /// </summary>
            public string p10_sign { get; set; }


            /// <summary>
            /// 备注
            /// </summary>
            public string p11_remark { get; set; }


            /// <summary>
            /// 支付时间
            /// </summary>
            public string p12_paymenttime { get; set; }

            /// <summary>
            /// 应付金额
            /// </summary>
            public string p13_amountpayable { get; set; }
        }
    }
}