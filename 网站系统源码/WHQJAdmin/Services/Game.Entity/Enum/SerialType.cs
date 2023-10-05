using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Game.Utils;

namespace Game.Entity.Enum
{
    /// <summary>
    /// 钻石流水场景
    /// </summary>
    [Serializable]
    [EnumDescription("钻石流水场景")]
    public enum DiamondSerialType
    {
        /// <summary>
        /// 后台赠送
        /// </summary>
        [EnumDescription("后台赠送")]
        后台赠送 = 0,

        /// <summary>
        /// 注册赠送
        /// </summary>
        [EnumDescription("注册赠送")]
        注册赠送 = 1,

        ///// <summary>
        ///// 推广奖励
        ///// </summary>
        //[EnumDescription("推广奖励")]
        //推广奖励 = 2,

        ///// <summary>
        ///// 充值推广
        ///// </summary>
        //[EnumDescription("充值推广")]
        //充值推广 = 3,

        ///// <summary>
        ///// 绑定推广赠送
        ///// </summary>
        //[EnumDescription("绑定推广赠送")]
        //绑定推广赠送 = 4,

        /// <summary>
        /// 排行榜奖励
        /// </summary>
        [EnumDescription("排行榜奖励")]
        排行榜奖励 = 5,

        /// <summary>
        /// 实名认证奖励
        /// </summary>
        [EnumDescription("实名认证奖励")]
        实名认证奖励 = 6,

        /// <summary>
        /// 代理赠送
        /// </summary>
        [EnumDescription("代理赠送")]
        代理赠送 = 7,

        /// <summary>
        /// 被代理赠送
        /// </summary>
        [EnumDescription("被代理赠送")]
        被代理赠送 = 8,

        /// <summary>
        /// 购买道具
        /// </summary>
        [EnumDescription("购买道具")]
        购买道具 = 9,

        /// <summary>
        /// 创建房间
        /// </summary>
        [EnumDescription("创建房间")]
        创建房间 = 10,

        /// <summary>
        /// AA制游戏
        /// </summary>
        [EnumDescription("AA制游戏")]
        AA制游戏 = 11,

        /// <summary>
        /// 充值赠送
        /// </summary>
        [EnumDescription("充值赠送")]
        钻石兑换 = 12,

        /// <summary>
        /// 领取返利
        /// </summary>
        [EnumDescription("领取返利")]
        领取返利 = 13,

        /// <summary>
        /// 充值额外赠送
        /// </summary>
        [EnumDescription("充值额外赠送")]
        充值额外赠送 = 14,
        /// <summary>
        /// 每日分享
        /// </summary>
        [EnumDescription("每日分享")]
        每日分享 = 15,
        /// <summary>
        /// 签到
        /// </summary>
        [EnumDescription("签到")]
        签到 = 16,
        /// <summary>
        /// 比赛奖励
        /// </summary>
        [EnumDescription("比赛奖励")]
        比赛奖励 = 17,
    }

    /// <summary>
    /// 金币流水场景
    /// </summary>
    [Serializable]
    [EnumDescription("金币流水场景")]
    public enum GoldSerialType
    {
        /// <summary>
        /// 后台赠送
        /// </summary>
        [EnumDescription("后台赠送")]
        后台赠送 = 0,

        /// <summary>
        /// 注册赠送
        /// </summary>
        [EnumDescription("注册赠送")]
        注册赠送 = 1,

        /// <summary>
        /// 主动转账
        /// </summary>
        [EnumDescription("主动转账")]
        主动转账 = 2,

        /// <summary>
        /// 接收转账
        /// </summary>
        [EnumDescription("接收转账")]
        接收转账 = 3,

        /// <summary>
        /// 购买道具
        /// </summary>
        [EnumDescription("购买道具")]
        购买道具 = 4,

        /// <summary>
        /// 兑换金币
        /// </summary>
        [EnumDescription("兑换金币")]
        兑换金币 = 5,

        /// <summary>
        /// 存入银行
        /// </summary>
        [EnumDescription("存入银行")]
        存入银行 = 6,

        /// <summary>
        /// 银行取出
        /// </summary>
        [EnumDescription("银行取出")]
        银行取出 = 7,

        /// <summary>
        /// 银行服务费
        /// </summary>
        [EnumDescription("银行服务费")]
        银行服务费 = 8,

        /// <summary>
        /// 领取返利
        /// </summary>
        [EnumDescription("领取返利")]
        领取返利 = 9,

        /// <summary>
        /// 代理赠送
        /// </summary>
        [EnumDescription("代理赠送")]
        代理赠送 = 10,

        /// <summary>
        /// 被代理赠送
        /// </summary>
        [EnumDescription("被代理赠送")]
        被代理赠送 = 11,

        /// <summary>
        /// 充值额外赠送
        /// </summary>
        [EnumDescription("充值额外赠送")]
        充值额外赠送 = 12,
        /// <summary>
        /// 每日分享
        /// </summary>
        [EnumDescription("每日分享")]
        每日分享 = 13,
        /// <summary>
        /// 签到
        /// </summary>
        [EnumDescription("签到")]
        签到 = 14,
        /// <summary>
        /// 比赛奖励
        /// </summary>
        [EnumDescription("比赛奖励")]
        比赛奖励 = 15,
        /// <summary>
        /// 绑定手机
        /// </summary>
        [EnumDescription("绑定手机")]
        绑定手机 = 16,
        /// <summary>
        /// 排行榜奖励
        /// </summary>
        [EnumDescription("排行榜奖励")]
        排行榜奖励 = 17,
    }




    /// <summary>
    /// 签到物品配置场景
    /// </summary>
    [Serializable]
    [EnumDescription("签到物品配置场景")]
    public enum GamePackageGoodsType
    {
        /// <summary>
        /// 游戏币
        /// </summary>
        [EnumDescription("游戏币")]
        游戏币 = 0,

        /// <summary>
        /// 钻石
        /// </summary>
        [EnumDescription("钻石")]
        钻石 = 1,

        /// <summary>
        /// 道具
        /// </summary>
        [EnumDescription("道具")]
        道具 = 2,

        /// <summary>
        /// 奖券
        /// </summary>
        [EnumDescription("奖券")]
        奖券 = 3,

    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    [EnumDescription("签到物品为道具场景")]
    public enum GamePackageGoodsProperty
    {
        /// <summary>
        /// 无
        /// </summary>
        [EnumDescription("无")]
        无 = 0,
        /// <summary>
        /// 大喇叭
        /// </summary>
        [EnumDescription("大喇叭")]
        大喇叭 = 306,
    }

    /// <summary>
    /// 奖券流水场景
    /// </summary>
    [Serializable]
    [EnumDescription("奖券流水场景")]
    public enum AwardTicketSerialType
    {
        /// <summary>
        /// 后台赠送
        /// </summary>
        [EnumDescription("后台赠送")]
        后台赠送 = 0,
        /// <summary>
        /// 签到
        /// </summary>
        [EnumDescription("签到")]
        签到 = 1,
        /// <summary>
        /// 任务
        /// </summary>
        [EnumDescription("任务")]
        任务 = 2,
        /// <summary>
        /// 排行榜奖励
        /// </summary>
        [EnumDescription("排行榜奖励")]
        排行榜奖励 = 3,
        /// <summary>
        /// 游戏获得
        /// </summary>
        [EnumDescription("游戏获得")]
        游戏获得 = 4,
        /// <summary>
        /// 比赛奖励
        /// </summary>
        [EnumDescription("比赛奖励")]
        比赛奖励 = 5,
        /// <summary>
        /// 兑换花费
        /// </summary>
        [EnumDescription("兑换花费")]
        兑换花费 = 10,
    }

    /// <summary>
    /// 大联盟勋章流水类型
    /// </summary>
    [Serializable]
    [EnumDescription("大联盟勋章流水类型")]
    public enum GroupScoreType
    {
        /// <summary>
        /// 发放
        /// </summary>
        [EnumDescription("发放")]
        发放 = 0,
        /// <summary>
        /// 扣除
        /// </summary>
        [EnumDescription("扣除")]
        扣除 = 1,
        /// <summary>
        /// 运营商
        /// </summary>
        [EnumDescription("运营商")]
        运营商 = 2,
        /// <summary>
        /// 成员贡献
        /// </summary>
        [EnumDescription("成员贡献")]
        成员贡献 = 3
    }

    /// <summary>
    /// 大联盟玩家勋章操作类型
    /// </summary>
    [Serializable]
    [EnumDescription("大联盟玩家勋章操作类型")]
    public enum GroupScorePresentType
    {
        /// <summary>
        /// 赠送
        /// </summary>
        [EnumDescription("赠送")]
        赠送 = 0,
        /// <summary>
        /// 消耗
        /// </summary>
        [EnumDescription("消耗")]
        消耗 = 1,
    }

    /// <summary>
    /// 大联盟玩家勋章流水类型
    /// </summary>
    [Serializable]
    [EnumDescription("大联盟玩家勋章流水类型")]
    public enum GroupUserScoreType
    {
        /// <summary>
        /// 赠送
        /// </summary>
        [EnumDescription("赠送")]
        赠送 = 0,
        /// <summary>
        /// 扣除
        /// </summary>
        [EnumDescription("扣除")]
        扣除 = 1,
        /// <summary>
        /// 游戏输赢
        /// </summary>
        [EnumDescription("游戏输赢")]
        游戏输赢 = 2,
        /// <summary>
        /// 贡献
        /// </summary>
        [EnumDescription("贡献")]
        贡献 = 3,
        /// <summary>
        /// 清零
        /// </summary>
        [EnumDescription("清零")]
        清零 = 4,
    }

}