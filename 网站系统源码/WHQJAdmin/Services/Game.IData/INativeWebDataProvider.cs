using Game.Kernel;
using Game.Entity.NativeWeb;
using System.Data;

namespace Game.IData
{
    /// <summary>
    /// 后台数据层接口
    /// </summary>
    public interface INativeWebDataProvider //: IProvider
    {
        #region 公用分页
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <param name="fields">查询字段</param>
        /// <returns></returns>
        PagerSet GetList(string tableName, string where, string order, int pageIndex, int pageSize, string[] fields = null);
        #endregion 公用分页

        #region 站点配置
        /// <summary>
        /// 获取站点配置
        /// </summary>
        /// <param name="id">配置标识</param>
        /// <returns></returns>
        ConfigInfo GetConfigInfo(int id);
        /// <summary>
        /// 获取站点配置
        /// </summary>
        /// <param name="configKey">配置Key</param>
        /// <returns></returns>
        ConfigInfo GetConfigInfo(string configKey);
        /// <summary>
        /// 修改站点配置
        /// </summary>
        /// <param name="config">配置信息</param>
        int UpdateConfigInfo(ConfigInfo config);
        #endregion 站点配置

        #region 广告管理
        /// <summary>
        /// 获取广告实体
        /// </summary>
        /// <param name="id">广告标识</param>
        /// <returns>广告实体</returns>
        Ads GetAds(int id);
        /// <summary>
        /// 删除广告
        /// </summary>
        /// <param name="idlist">标识列表</param>
        int DeleteAds(string idlist);
        /// <summary>
        /// 新增广告
        /// </summary>
        /// <param name="ads">广告信息</param>
        int InsertAds(Ads ads);
        /// <summary>
        /// 修改广告
        /// </summary>
        /// <param name="ads">广告实体</param>
        int UpdateAds(Ads ads);
        #endregion 广告管理

        #region 新闻公告
        /// <summary>
        /// 获取新闻公告信息
        /// </summary>
        /// <param name="id">公告标识</param>
        /// <returns></returns>
        SystemNotice GetSystemNoticeInfo(int id);
        /// <summary>
        /// 新增新闻公告
        /// </summary>
        /// <param name="notice">公告信息</param>
        /// <returns></returns>
        int InsertSystemNotice(SystemNotice notice);
        /// <summary>
        /// 修改公告信息
        /// </summary>
        /// <param name="notice">公告信息</param>
        /// <returns></returns>
        int UpdateSystemNotice(SystemNotice notice);
        /// <summary>
        /// 删除公告
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <returns></returns>
        int DeleteSystemNotice(string idlist);
        #endregion

        #region 排行榜
        /// <summary>
        /// 获取排行榜奖励配置
        /// </summary>
        /// <returns></returns>
        DataSet GetRankingConfigList();
        /// <summary>
        /// 根据标识获取排行榜配置
        /// </summary>
        /// <param name="configid">配置标识</param>
        /// <returns></returns>
        RankingConfig GetRankingConfigById(int configid);
        /// <summary>
        /// 判断排行榜配置是否存在
        /// </summary>
        /// <param name="typeid">排行榜类型</param>
        /// <param name="ranktype">排行榜排名</param>
        /// <returns></returns>
        bool ExistRankingConfig(int typeid, int ranktype);
        /// <summary>
        /// 新增排行榜配置
        /// </summary>
        /// <param name="config">配置信息</param>
        /// <returns></returns>
        int InsertRankingConfig(RankingConfig config);
        /// <summary>
        /// 修改排行榜配置
        /// </summary>
        /// <param name="config">配置信息</param>
        /// <returns></returns>
        int UpdateRankingConfig(RankingConfig config);
        /// <summary>
        /// 删除奖励配置
        /// </summary>
        /// <param name="configid">配置标识</param>
        /// <returns></returns>
        int DeleteRankingConfig(int configid);
        /// <summary>
        /// 获取排行榜奖励列表
        /// </summary>
        /// <param name="typeid">1、财富排行 2、胜局排行</param>
        /// <param name="userid">用户标识</param>
        /// <param name="stime">发奖起始时间</param>
        /// <param name="etime">发奖结束时间</param>
        /// <returns></returns>
        DataSet GetRankAwardList(int typeid, int userid, int sdate, int edate);
        #endregion

        #region 游戏规则
        /// <summary>
        /// 获取游戏规则
        /// </summary>
        /// <param name="ID">ID</param>
        /// <returns></returns>
        GameRule GetGameRuleInfo(int ID);


        /// <summary>
        /// 获取游戏规则
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        GameRule GetGameRuleInfoByKindid(int kindid, int Type);
        /// <summary>
        /// 新增游戏规则
        /// </summary>
        /// <param name="rule">游戏规则</param>
        /// <returns></returns>
        int InsertGameRule(GameRule rule);
        /// <summary>
        /// 修改游戏规则
        /// </summary>
        /// <param name="rule">游戏规则</param>
        /// <returns></returns>
        int UpdateGameRule(GameRule rule);
        /// <summary>
        /// 删除游戏规则
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <returns></returns>
        int DeleteGameRule(string idlist);
        /// <summary>
        /// 禁用启用游戏规则
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <param name="nullity">禁用启用标识</param>
        /// <returns></returns>
        int NullityGameRule(string idlist, int nullity);
        #endregion

        #region 常见问题

        /// <summary>
        /// 获取常见问题实体 by id
        /// </summary>
        /// <param name="id"></param>
        /// <returns><see cref="Question"/></returns>
        Question GetQuestionInfo(int id);

        /// <summary>
        /// 保存常见问题（新增、修改）通用
        /// </summary>
        /// <param name="quest"></param>
        /// <returns></returns>
        int SaveQuestionInfo(Question quest);

        /// <summary>
        /// 批量删除常见问题
        /// </summary>
        /// <param name="ids"></param>
        /// <returns></returns>
        int DeleteQuestionInfo(string ids);

        #endregion

        #region 数据统计
        /// <summary>
        /// 获取数据统计
        /// </summary>
        /// <param name="sDate">起始时间标识</param>
        /// <param name="eDate">结束时间标识</param>
        /// <returns></returns>
        DataSet GetDataStatistics(string sDate, string eDate);
        #endregion


        #region 商品信息
        /// <summary>
        /// 根据商品id获取商品实体
        /// </summary>
        /// <param name="id">商品id</param>
        AwardInfo GetAwardInfoByID(int id);

        /// <summary>
        /// 修改商品状态
        /// </summary>
        /// <param name="idList">商品id</param>
        /// <param name="state">商品状态</param>
        int UpdateAwardInfoNulity(string idList, int state);

        /// <summary>
        /// 保存商品信息
        /// </summary>
        /// <param name="info">商品实体</param>
        void InsertAwardInfo(AwardInfo info);

        /// <summary>
        /// 更新商品信息
        /// </summary>
        /// <param name="info">商品实体</param>
        int UpdateAwardInfo(AwardInfo info);

        /// <summary>
        /// 获取最大键值
        /// </summary>
        int GetMaxAwardInfoID();

        /// <summary>
        /// 判断该类型下是否存在商品
        /// </summary>
        /// <param name="typeId">类型id</param>
        bool IsHaveGoods(int typeID);

        #endregion

        #region 商品订单
        /// <summary>
        /// 根据订单号获取订单
        /// </summary>
        /// <param name="orderID">订单号</param>
        AwardOrder GetAwardOrderByID(string orderID);

        /// <summary>
        /// 修改订单状态
        /// </summary>
        /// <param name="state">状态值</param>
        /// <param name="OrderID">订单号</param>
        /// <param name="note">处理备注</param>
        int UpdateOrderState(int state, string orderID);

        /// <summary>
        /// 获取新的订单总数
        /// </summary>
        /// <returns></returns>
        int GetNewOrderCounts();
        #endregion
        #region 每日分享
        /// <summary>
        /// 分享配置
        /// </summary>
        /// <param name="id">配置标识</param>
        /// <returns></returns>
        ShareConfig GetShareConfig(int id);
        /// <summary>
        /// 修改每日分享
        /// </summary>
        /// <param name="config">分享配置</param>

        int UpdateShareConfig(ShareConfig config);
        #endregion

    }
}
