using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Game.Data.Factory;
using Game.IData;
using Game.Kernel;
using Game.Utils;
using System.Data;
using Game.Entity.Accounts;
using Game.Entity.Agent;
using Game.Entity.NativeWeb;

namespace Game.Facade
{
    /// <summary>
    /// 网站外观
    /// </summary>
    public class NativeWebFacade
    {
        #region Fields

        private INativeWebDataProvider webData;

        #endregion

        #region 构造函数

        /// <summary>
        /// 构造函数
        /// </summary>
        public NativeWebFacade()
        {
            webData = ClassFactory.GetINativeWebDataProvider();
        }
        #endregion

        #region 数据分页
        /// <summary>
        /// 游戏规则数据分页
        /// </summary>
        /// <param name="pageIndex">分页下标</param>
        /// <param name="pageSize">分页大小</param>
        /// <returns></returns>
        public PagerSet GetGamePageList(int pageIndex, int pageSize)
        {
            return webData.GetGamePageList(pageIndex, pageSize);
        }
        /// <summary>
        /// 新闻公告数据分页
        /// </summary>
        /// <param name="pageIndex">分页下标</param>
        /// <param name="pageSize">分页大小</param>
        /// <returns></returns>
        public PagerSet GetNewPageList(int pageIndex, int pageSize)
        {
            return webData.GetNewPageList(pageIndex, pageSize);
        }
        #endregion

        #region 获取配置信息
        /// <summary>
        /// 获取配置信息
        /// </summary>
        /// <param name="configKey">配置键值</param>
        /// <returns></returns>
        public ConfigInfo GetConfigInfo(string configKey)
        {
            return webData.GetConfigInfo(configKey);
        }
        /// <summary>
        /// 获取配置信息集合
        /// </summary>
        /// <returns></returns>
        public IList<ConfigInfo> GetConfigInfoList()
        {
            return webData.GetConfigInfoList();
        }
        #endregion

        #region 手机登录信息
        /// <summary>
        /// 获取手机登录数据
        /// </summary>
        /// <returns></returns>
        public DataSet GetMobileLoginInfo()
        {
            return webData.GetMobileLoginInfo();
        }
        #endregion

        #region 获取排行榜信息
        /// <summary>
        /// 获取排行榜数据
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="typeid">排行榜类型</param>
        /// <returns></returns>
        public DataSet GetDayRankingData(int userid, int typeid)
        {
            return webData.GetDayRankingData(userid, typeid);
        }

        /// <summary>
        /// 领取排行榜奖励
        /// </summary>
        /// <param name="UserID">用户标识</param>
        /// <param name="DateID">时间标识</param>
        /// <param name="TypeID">排行榜类型</param>
        /// <param name="IP">领取地址</param>
        /// <returns></returns>
        public Message ReceiveRankingAward(int UserID, int DateID, int TypeID, string IP)
        {
            return webData.ReceiveRankingAward(UserID, DateID, TypeID, IP);
        }
       
        #endregion

        #region 新闻信息
        /// <summary>
        /// 获取首页新闻公告
        /// </summary>
        /// <returns></returns>
        public IList<SystemNotice> GetHomePageNews()
        {
            return webData.GetHomePageNews();
        }
        /// <summary>
        /// 获取新闻详情
        /// </summary>
        /// <param name="id">新闻标识</param>
        /// <returns></returns>
        public SystemNotice GetWebNewsInfo(int id)
        {
            return webData.GetWebNewsInfo(id);
        }
        #endregion

        #region 游戏规则
        /// <summary>
        /// 获取游戏规则列表
        /// </summary>
        /// <returns></returns>
        public IList<GameRule> GetGameRuleList()
        {
            return webData.GetGameRuleList();
        }
        /// <summary>
        /// 获取游戏规则
        /// </summary>
        /// <param name="kindid">游戏规则</param>
        /// <returns></returns>
        public GameRule GetGameRuleInfo(int kindid)
        {
            return webData.GetGameRuleInfo(kindid);
        }
        #endregion

        #region 广告位图片
        /// <summary>
        /// 获取广告图列表
        /// </summary>
        /// <returns></returns>
        public IList<Ads> GetAdsList()
        {
            return webData.GetAdsList();
        }
        #endregion

        #region 常见问题

        /// <summary>
        /// 获取常见问题列表
        /// </summary>
        /// <returns></returns>
        public IList<Question> GetQAList(int top = 0)
        {
            return webData.GetQAList(top);
        }

        #endregion

        #region 商城信息
        /// <summary>
        /// 获取商品首页信息
        /// </summary>
        /// <returns></returns>
        public DataSet GetAwardHomeList(int typeid)
        {
            return webData.GetAwardHomeList(typeid);
        }
        /// <summary>
        /// 获取商品信息列表
        /// </summary>
        /// <param name="typeid">商品类型</param>
        /// <returns></returns>
        public DataSet GetAwardInfoByType(int typeid)
        {
            return webData.GetAwardInfoByType(typeid);
        }
        /// <summary>
        /// 获取下单信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="awardid">商品标识</param>
        /// <returns></returns>
        public DataSet GetOrderAwardInfo(int userid, int awardid)
        {
            return webData.GetOrderAwardInfo(userid, awardid);
        }
        /// <summary>
        /// 获取兑换商品记录
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        public DataSet GetAwardOrderList(int userid)
        {
            return webData.GetAwardOrderList(userid);
        }

        /// <summary>
        /// 获取商品实体
        /// </summary>
        /// <param name="awradID"></param>
        /// <returns></returns>
        public AwardInfo GetAwardInfo(int AwardID)
        {
            return webData.GetAwardInfo(AwardID);
        }

        /// <summary>
        /// 获取玩家兑换地址信息
        /// </summary>
        /// <param name="awradID"></param>
        /// <returns></returns>
        public AwardUser GetAwardUser(int userid)
        {
            return webData.GetAwardUser(userid);
        }
        /// <summary>
        /// 购买商品
        /// </summary>
        /// <param name="awardOrder"></param>
        /// <returns></returns>
        public Message BuyAward(AwardOrder awardOrder)
        {
            return webData.BuyAward(awardOrder);
        }

        /// <summary>
        /// 添加兑换联系信息
        /// </summary>
        /// <param name="User"></param>
        /// <returns></returns>
        public int insertAwardUser(AwardUser User)
        {
            return webData.insertAwardUser(User);
        }

        /// <summary>
        /// 修改兑换联系信息
        /// </summary>
        /// <param name="User"></param>
        /// <returns></returns>
        public int updateAwardUser(AwardUser User)
        {
            return webData.updateAwardUser(User);
        }
        #endregion

        /// <summary>
        /// 获取排行榜数据
        /// </summary>
        /// <param name="typeid">排行榜类型</param>
        /// <returns></returns>
        public DataSet GetDayRankingData(int typeid)
        {
            return webData.GetDayRankingData(typeid);
        }

        /// <summary>
        /// 每日分享奖励
        /// </summary>
        /// <param name="userid">登录用户标识</param>
        /// <returns></returns>
        public DataSet GetTimesReward(int userid,string ClientIP)
        {
            return webData.GetTimesReward(userid, ClientIP);
        }

        /// <summary>
        ///获取每日奖励配置 
        /// </summary>
        /// <returns></returns>
        public DataSet GetShareReward()
        {
            return webData.GetShareReward();
        }

        #region 微信扫码登录

        /// <summary>
        /// 获取微信扫码登录记录By Key
        /// </summary>
        /// <param name="key">验证值</param>
        /// <returns><see cref="WxScanLoginRecord"/></returns>
        public WxScanLoginRecord GetWxScanLoginRecordByKey(string key)
        {
            return webData.GetWxScanLoginRecordByKey(key);
        }

        /// <summary>
        /// 保存微信扫码登录记录（新增、更新（只更新状态和获取ip））
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int SaveWxScanLoginRecord(WxScanLoginRecord model)
        {
            return webData.SaveWxScanLoginRecord(model);
        }

        /// <summary>
        /// 更新为已获取状态
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int PickWxScanLoginRecord(WxScanLoginRecord model)
        {
            return webData.PickWxScanLoginRecord(model);
        }
        #endregion
    }
}
