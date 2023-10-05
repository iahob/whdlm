using System.Collections.Generic;

using Game.Kernel;
using System.Data;
using Game.Entity.Accounts;
using Game.Entity.Agent;
using Game.Entity.NativeWeb;

namespace Game.IData
{
    /// <summary>
    /// 网站库数据层接口
    /// </summary>
    public interface INativeWebDataProvider //: IProvider
    {
        #region 数据分页
        /// <summary>
        /// 游戏规则数据分页
        /// </summary>
        /// <param name="pageIndex">分页下标</param>
        /// <param name="pageSize">分页大小</param>
        /// <returns></returns>
        PagerSet GetGamePageList(int pageIndex, int pageSize);
        /// <summary>
        /// 新闻公告数据分页
        /// </summary>
        /// <param name="pageIndex">分页下标</param>
        /// <param name="pageSize">分页大小</param>
        /// <returns></returns>
        PagerSet GetNewPageList(int pageIndex, int pageSize);
        #endregion

        #region 获取配置信息
        /// <summary>
        /// 获取配置信息
        /// </summary>
        /// <param name="configKey">配置键值</param>
        /// <returns></returns>
        ConfigInfo GetConfigInfo(string configKey);
        /// <summary>
        /// 获取配置信息集合
        /// </summary>
        /// <returns></returns>
        IList<ConfigInfo> GetConfigInfoList();
        #endregion

        #region 手机登录信息
        /// <summary>
        /// 获取手机登录数据
        /// </summary>
        /// <returns></returns>
        DataSet GetMobileLoginInfo();
        #endregion

        #region 获取排行榜信息
        /// <summary>
        /// 获取排行榜数据
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="typeid">排行榜类型</param>
        /// <returns></returns>
        DataSet GetDayRankingData(int userid, int typeid);
        /// <summary>
        /// 领取排行榜奖励
        /// </summary>
        /// <param name="userId">用户标识</param>
        /// <param name="dateId">时间标识</param>
        /// <param name="typeId">排行榜类型</param>
        /// <param name="ip">领取地址</param>
        /// <returns></returns>
        Message ReceiveRankingAward(int userId, int dateId, int typeId, string ip);

        #endregion

        #region 新闻信息
        /// <summary>
        /// 获取首页新闻公告
        /// </summary>
        /// <returns></returns>
        IList<SystemNotice> GetHomePageNews();
        /// <summary>
        /// 获取新闻详情
        /// </summary>
        /// <param name="id">新闻标识</param>
        /// <returns></returns>
        SystemNotice GetWebNewsInfo(int id);
        #endregion

        #region 游戏规则
        /// <summary>
        /// 获取游戏规则列表
        /// </summary>
        /// <returns></returns>
        IList<GameRule> GetGameRuleList();
        /// <summary>
        /// 获取游戏规则
        /// </summary>
        /// <param name="kindid">游戏规则</param>
        /// <returns></returns>
        GameRule GetGameRuleInfo(int kindid);
        #endregion

        #region 广告位图片
        /// <summary>
        /// 获取广告图列表
        /// </summary>
        /// <returns></returns>
        IList<Ads> GetAdsList();
        #endregion

        #region 常见问题

        /// <summary>
        /// 获取常见问题列表
        /// </summary>
        /// <returns></returns>
        IList<Question> GetQAList(int top = 0);

        #endregion

        #region 商城信息
        /// <summary>
        /// 获取商品首页信息
        /// </summary>
        /// <returns></returns>
        DataSet GetAwardHomeList(int typeid);
        /// <summary>
        /// 获取商品信息列表
        /// </summary>
        /// <param name="typeid">商品类型</param>
        /// <returns></returns>
        DataSet GetAwardInfoByType(int typeid);
        /// <summary>
        /// 获取下单信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="awardid">商品标识</param>
        /// <returns></returns>
        DataSet GetOrderAwardInfo(int userid, int awardid);
        /// <summary>
        /// 获取兑换商品记录
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        DataSet GetAwardOrderList(int userid);

        /// <summary>
        /// 获取商品实体
        /// </summary>
        /// <param name="awradID"></param>
        /// <returns></returns>
        AwardInfo GetAwardInfo(int AwardID);
        /// <summary>
        /// 获取玩家兑换地址信息
        /// </summary>
        /// <param name="awradID"></param>
        /// <returns></returns>
        AwardUser GetAwardUser(int userid);

        /// <summary>
        /// 购买商品
        /// </summary>
        /// <param name="awardOrder"></param>
        /// <returns></returns>
        Message BuyAward(AwardOrder awardOrder);

        /// <summary>
        /// 添加兑换联系信息
        /// </summary>
        /// <param name="User"></param>
        /// <returns></returns>
        int insertAwardUser(AwardUser User);
        /// <summary>
        /// 修改兑换联系信息
        /// </summary>
        /// <param name="User"></param>
        /// <returns></returns>
        int updateAwardUser(AwardUser User);


        /// <summary>
        /// 获取排行榜数据
        /// </summary>
        /// <param name="typeid">排行榜类型</param>
        /// <returns></returns>
        DataSet GetDayRankingData(int typeid);
        #endregion

        /// <summary>
        /// 每日分享奖励
        /// </summary>
        /// <param name="userid">登录用户标识</param>
        /// <returns></returns>
        DataSet GetTimesReward(int userid, string ClientIP);

        /// <summary>
        /// 获取每日奖励配置
        /// </summary>
        /// <returns></returns>
        DataSet GetShareReward();

        #region 微信扫码登录

        /// <summary>
        /// 获取微信扫码登录记录By Key
        /// </summary>
        /// <param name="key">验证值</param>
        /// <returns><see cref="WxScanLoginRecord"/></returns>
        WxScanLoginRecord GetWxScanLoginRecordByKey(string key);

        /// <summary>
        /// 保存微信扫码登录记录（新增、更新（只更新状态和获取ip））
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        int SaveWxScanLoginRecord(WxScanLoginRecord model);

        /// <summary>
        /// 更新为已获取状态
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        int PickWxScanLoginRecord(WxScanLoginRecord model);
        #endregion
    }
}
