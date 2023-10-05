using System.Collections.Generic;
using Game.Kernel;
using Game.Entity.Treasure;
using System.Data;
using Game.Entity.Record;

namespace Game.IData
{
    /// <summary>
    /// 金币库数据层接口
    /// </summary>
    public interface ITreasureDataProvider //: IProvider
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

        #region 充值配置
        /// <summary>
        /// 获取充值产品
        /// </summary>
        /// <param name="configId">充值产品标识</param>
        /// <returns></returns>
        AppPayConfig GetAppPayConfig(int configId);
        /// <summary>
        /// 删除充值产品
        /// </summary>
        /// <param name="idlist">标识列表</param>
        int DeleteAppPayConfig(string idlist);
        /// <summary>
        /// 判断充值产品是否存在
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        bool IsExistAppPayConfig(string where);
        /// <summary>
        /// 新增、修改充值产品
        /// </summary>
        /// <param name="config">充值产品</param>
        /// <returns></returns>
        int SaveAppPayConfig(AppPayConfig config);
        /// <summary>
        /// 获取在线充值配置
        /// </summary>
        /// <param name="configId">充值产品标识</param>
        /// <returns></returns>
        OnLineWeChat GetOnLineWeChat(int configId);
        /// <summary>
        /// 删除在线充值配置
        /// </summary>
        /// <param name="idlist">标识列表</param>
        int DeleteOnLineWeChat(string idlist);
        /// <summary>
        /// 新增在线充值配置
        /// </summary>
        /// <param name="config">充值产品</param>
        /// <returns></returns>
        int InsertOnLineWeChat(OnLineWeChat config);
        /// <summary>
        /// 修改在线充值配置
        /// </summary>
        /// <param name="config">充值产品</param>
        /// <returns></returns>
        int UpdateOnLineWeChat(OnLineWeChat config);

        /// <summary>
        /// 获取订单信息
        /// </summary>
        /// <param name="orderId"></param>
        /// <returns></returns>
        OnLinePayOrder GetOnLinePayOrder(string orderId);

        /// <summary>
        /// 充值处理订单(仅在订单状态为1的时候使用)
        /// </summary>
        /// <param name="ordersId"></param>
        /// <returns></returns>
        Message OperationOnLineOrder(string ordersId);
        #endregion

        #region 推广配置
        /// <summary>
        /// 获取推广配置
        /// </summary>
        /// <param name="configId">推广配置id</param>
        /// <returns></returns>
        SpreadConfig GetSpreadConfig(int configId);
        /// <summary>
        /// 推广配置数量
        /// </summary>
        /// <returns></returns>
        int SpreadConfigCount();
        /// <summary>
        /// 删除推广配置
        /// </summary>
        /// <param name="configId">推广配置id</param>
        /// <returns></returns>
        int DeleteSpreadConfig(int configId);
        /// <summary>
        /// 新增推广信息
        /// </summary>
        /// <param name="config">推广配置</param>
        /// <returns></returns>
        int InsertSpreadConfig(SpreadConfig config);
        /// <summary>
        /// 修改推广信息
        /// </summary>
        /// <param name="config">推广配置</param>
        /// <returns></returns>
        int UpdateSpreadConfig(SpreadConfig config);

        /// <summary>
        /// 获取推广返利配置
        /// </summary>
        /// <param name="configId"></param>
        /// <returns></returns>
        SpreadReturnConfig GetSpreadReturnConfig(int configId);

        /// <summary>
        /// 删除推广返利配置
        /// </summary>
        /// <param name="configId"></param>
        /// <returns></returns>
        int DeleteSpreadReturnConfig(int configId);

        /// <summary>
        /// 保存推广返利配置（新增、更新）
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        int SaveSpreadReturnConfig(SpreadReturnConfig config);
        #endregion

        #region 钻石信息
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageDiamondList(string where, string order, int pageIndex, int pageSize);
        /// <summary>
        /// 赠送钻石
        /// </summary>
        /// <param name="diamond">赠送钻石信息</param>
        /// <returns></returns>
        Message GrantDiamond(RecordGrantDiamond diamond);

        /// <summary>
        /// 赠送奖券
        /// </summary>
        /// <param name="AwardTicket">赠送奖券信息</param>
        /// <returns></returns>
        Message GrantAwardTicket(RecordGrantAwardTicket AwardTicket);
        /// <summary>
        /// 获取钻石前50名排行
        /// </summary>
        /// <returns></returns>
        DataSet GetDiamondRank();
        /// <summary>
        /// 获取奖券前50名排行
        /// </summary>
        /// <returns></returns>
        DataSet GetAwardTicketRank();
        #endregion

        #region 统计总数
        /// <summary>
        /// 系统统计
        /// </summary>
        /// <returns></returns>
        DataSet GetStatInfo();
        /// <summary>
        /// 按条件获取已支付总数
        /// </summary>
        /// <param name="where"></param>
        /// <returns></returns>
        decimal GetTotalPayAmount(string where);
        /// <summary>
        /// 按条件获取已支付订单数
        /// </summary>
        /// <param name="where"></param>
        /// <returns></returns>
        long GetTotalPayOrderCount(string where);
        /// <summary>
        /// 获取钻石统计
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        long GetTotalUserDiamond(string where);
        /// <summary>
        /// 获取金币分布统计
        /// </summary>
        /// <returns></returns>
        DataSet GetGoldDistribute();
        /// <summary>
        /// 获取钻石分布统计
        /// </summary>
        /// <returns></returns>
        DataSet GetDiamondDistribute();
        #endregion

        #region 金币信息
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageScoreList(string where, string order, int pageIndex, int pageSize);
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageInoutList(string where, string order, int pageIndex, int pageSize);
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageDrawList(string where, string order, int pageIndex, int pageSize);
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageInsureList(string where, string order, int pageIndex, int pageSize);
        /// <summary>
        /// 赠送金币
        /// </summary>
        /// <param name="rgt">赠送金币信息</param>
        /// <returns></returns>
        Message GrantTreasure(RecordGrantTreasure rgt);
        /// <summary>
        /// 获取兑换金币配置
        /// </summary>
        /// <param name="configId">配置标识</param>
        /// <returns></returns>
        CurrencyExchConfig GetCurrencyExch(int configId);
        /// <summary>
        /// 删除兑换金币配置
        /// </summary>
        /// <param name="idlist">标识列表</param>
        int DeleteCurrencyExch(string idlist);
        /// <summary>
        /// 判断兑换金币配置是否存在相同钻石
        /// </summary>
        /// <param name="diamond">兑换钻石数</param>
        /// <returns></returns>
        bool IsExistCurrencyExch(int diamond);
        /// <summary>
        /// 新增兑换金币配置
        /// </summary>
        /// <param name="config">兑换金币配置</param>
        /// <returns></returns>
        int InsertCurrencyExch(CurrencyExchConfig config);
        /// <summary>
        /// 修改兑换金币配置
        /// </summary>
        /// <param name="config">兑换金币配置</param>
        /// <returns></returns>
        int UpdateCurrencyExch(CurrencyExchConfig config);
        #endregion

        #region 卡线管理

        /// <summary>
        /// 批量清除玩家卡线
        /// </summary>
        /// <param name="userlist"></param>
        /// <returns></returns>
        int CleanGameScoreLocker(string userlist);

        /// <summary>
        /// 获取在线玩家
        /// </summary>
        /// <param name="userid"></param>
        /// <returns></returns>
        GameScoreLocker GetOnlineUser(int userid);

        #endregion
    }
}
