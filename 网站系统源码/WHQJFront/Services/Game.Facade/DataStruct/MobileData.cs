﻿using System;
using System.Collections.Generic;
using System.Web.UI.WebControls;

// ReSharper disable InconsistentNaming

namespace Game.Facade.DataStruct
{
    public class MobileSystemConfig
    {
        public int IsOpenMall { get; set; }
        public int IsPayBindSpread { get; set; }
        public int BindSpreadPresent { get; set; }
        public int RankingListType { get; set; }
        public int PayChannel { get; set; }
        public int DiamondBuyPropCount { get; set; }
        public int RealNameAuthentPresent { get; set; }
        public int EffectiveFriendGame { get; set; }
        public int IOSNotStorePaySwitch { get; set; }
        public int GoldBuyPropCount { get; set; }
        public int EnjoinInsure { get; set; }
        public int TransferStauts { get; set; }
        public int MobileBattleRecordMask { get; set; }
        public int IsOpenGameSignIn { get; set; }
        public int IsOpenRedemptionCenter { get; set; }

    }
    public class GroupSystemConfig
    {
        public int MaxMemberCount { get; set; }
        public int MaxCreateGroupCount { get; set; }
        public int CreateGroupTakeIngot { get; set; }
        public int CreateGroupDeductIngot { get; set; }
        public int MaxJoinGroupCount { get; set; }
        public int GroupPayType { get; set; }
        public int GroupPayTypeChange { get; set; }
        public int GroupRoomType { get; set; }
    }



    public class MobileCustomerService
    {
        public string Phone { get; set; }
        public string WeiXin { get; set; }
        public string QQ { get; set; }
        public string Link { get; set; }
    }

    public class AdsMobile
    {
        public string Title { get; set; }
        public string ResourceURL { get; set; }
        public string LinkURL { get; set; }
        public int SortID { get; set; }
        public int PlatformType { get; set; }
    }

    public class GameSignInMobile
    {
        public int GoodsID { get; set; }
        public int PackageID { get; set; }
        public string Name { get; set; }
        public int TypeID { get; set; }
        public int PackageTypeID { get; set; }
        public string ResourceURL { get; set; }
        public int SortID { get; set; }

    }

    public class NoticeMobile
    {
        public int NoticeID { get; set; }
        public string NoticeTitle { get; set; }
        public string MoblieContent { get; set; }
        public DateTime PublisherTime { get; set; }
        public int PlatformType { get; set; }
    }

    public class SpreadConfigMobile
    {
        public int ConfigID { get; set; }
        public int SpreadNum { get; set; }
        public int PresentDiamond { get; set; }
        public int PresentPropID { get; set; }
        public string PresentPropName { get; set; }
        public int PresentPropNum { get; set; }
        public bool Flag { get; set; }
    }

    public class CacheRankAward
    {
        public int RankNum { get; set; }
        public long RankValue { get; set; }
        public int UserID { get; set; }
        public int GameID { get; set; }
        public string NickName { get; set; }
        public string FaceUrl { get; set; }
        public int FaceID { get; set; }
        public long Gold { get; set; }
        public int Diamond { get; set; }
        public int AwardTicket { get; set; }
        public int DateID { get; set; }
        public int TypeID { get; set; }
    }

    public class RankingRecevieMobile
    {
        public int DateID { get; set; }
        public int UserID { get; set; }
        public int GameID { get; set; }
        public string NickName { get; set; }
        public int SystemFaceID { get; set; }
        public string FaceUrl { get; set; }
        public int TypeID { get; set; }
        public int RankID { get; set; }
        public long RankValue { get; set; }
        public int Diamond { get; set; }
    }

    public class ReceiveRankAward
    {
        public int UserID { get; set; }
        public int Diamond { get; set; }
        public int Score { get; set; }
    }

    public class AppPayConfigMoile
    {
        public int ConfigID { get; set; }
        public string AppleID { get; set; }
        public string PayName { get; set; }
        public int PayType { get; set; }
        public decimal PayPrice { get; set; }
        public int PayIdentity { get; set; }
        public int ImageType { get; set; }
        public int SortID { get; set; }
        public byte ScoreType { get; set; }
        public int Score { get; set; }
        public int PresentScore { get; set; }
        public bool IsFristPay { get; set; }
        public bool IsDayFristPay { get; set; }
    }

    public class TreasureStream
    {
        public string SerialNumber { get; set; }
        public string SerialTime { get; set; }
        public long BeforeGold { get; set; }
        public int ChangeGold { get; set; }
        public long AfterGold { get; set; }
        public string TypeName { get; set; }
    }

    public class DiamondStream
    {
        public string SerialNumber { get; set; }
        public string SerialTime { get; set; }
        public long BeforeDiamond { get; set; }
        public int ChangeDiamond { get; set; }
        public long AfterDiamond { get; set; }
        public string TypeName { get; set; }
    }

    public class DiamondExchRecord
    {
        /// <summary>
        /// 兑换后钻石
        /// </summary>
        public long AfterDiamond { get; set; }

        /// <summary>
        /// 兑换后银行金币
        /// </summary>
        public long AfterInsureScore { get; set; }

        /// <summary>
        /// 兑换后身上金币
        /// </summary>
        public long AfterScore { get; set; }

        /// <summary>
        /// 消耗钻石
        /// </summary>
        public long ExchDiamond { get; set; }

        /// <summary>
        /// 兑换金币
        /// </summary>
        public long PresentGold { get; set; }
    }

    /// <summary>
    /// 手机游戏玩法结构体
    /// </summary>
    public class MobileGameRule
    {
        /// <summary>
        /// 游戏标识
        /// </summary>
        public int KindID { get; set; }

        /// <summary>
        /// 游戏名称
        /// </summary>
        public string KindName { get; set; }

        /// <summary>
        /// 游戏简介
        /// </summary>
        public string Content { get; set; }
        /// <summary>
        /// 规则图片
        /// </summary>
        public string RuleImg { get; set; }
        /// <summary>
        /// 图片类型
        /// </summary>
        public int Type { get; set; }
        /// <summary>
        /// 图片标识
        /// </summary>
        public int LogID { get; set; }
    }

    public class MobileBattleRecord
    {
        public long GroupID { get; set; }
        public string KindName { get; set; }
        public int RoomID { get; set; }
        public int PlayMode { get; set; }
        public int OwnerID { get; set; }
        public string OwnerNickName { get; set; }
        public long CellScore { get; set; }
        public long Score { get; set; }
        public int GamesNum { get; set; }
        public IList<MobileBattleDetails> Details { get; set; }
        public IList<MobileBattleDetails> Totals { get; set; }
        public int PlayBackCode { get; set; }
        public DateTime PlayTime { get; set; }
    }

    public class MobileBattleDetails
    {
        public bool IsOwner { get; set; }
        public int RoomID { get; set; }
        public int UserID { get; set; }
        public string NickName { get; set; }
        public int GamesNum { get; set; }
        public long Score { get; set; }
        public long SumScore { get; set; }
        public int LoopCount { get; set; }
        public DateTime PlayTime { get; set; }
    }
}
