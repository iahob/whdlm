using System.Collections.Generic;
using Game.IData;
using Game.Data.Factory;
using Game.Kernel;
using Game.Entity.Treasure;
using System.Data;
using Game.Entity.Record;

// ReSharper disable once CheckNamespace
namespace Game.Facade
{
    /// <summary>
    /// 金币库外观
    /// </summary>
    public class TreasureFacade
    {
        #region Fields

        // ReSharper disable once InconsistentNaming
        // ReSharper disable once FieldCanBeMadeReadOnly.Local
        private ITreasureDataProvider aideTreasureData;

        #endregion

        #region 构造函数

        /// <summary>
        /// 构造函数
        /// </summary>
        public TreasureFacade()
        {
            aideTreasureData = ClassFactory.GetITreasureDataProvider();
        }
        #endregion

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
        public PagerSet GetList(string tableName, string where, string order, int pageIndex, int pageSize, string[] fields = null)
        {
            return aideTreasureData.GetList(tableName, where, order, pageIndex, pageSize, fields);
        }
        #endregion 公用分页

        #region 充值配置
        /// <summary>
        /// 获取充值产品
        /// </summary>
        /// <param name="configId">充值产品标识</param>
        /// <returns></returns>
        public AppPayConfig GetAppPayConfig(int configId)
        {
            return aideTreasureData.GetAppPayConfig(configId);
        }
        /// <summary>
        /// 删除充值产品
        /// </summary>
        /// <param name="idlist">标识列表</param>
        public int DeleteAppPayConfig(string idlist)
        {
            return aideTreasureData.DeleteAppPayConfig(idlist);
        }
        /// <summary>
        /// 判断充值产品是否存在
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public bool IsExistAppPayConfig(string where)
        {
            return aideTreasureData.IsExistAppPayConfig(where);
        }
        /// <summary>
        /// 新增、修改充值产品
        /// </summary>
        /// <param name="config">充值产品</param>
        /// <returns></returns>
        public int SaveAppPayConfig(AppPayConfig config)
        {
            return aideTreasureData.SaveAppPayConfig(config);
        }
        
        /// <summary>
        /// 获取在线充值配置
        /// </summary>
        /// <param name="configId">充值产品标识</param>
        /// <returns></returns>
        public OnLineWeChat GetOnLineWeChat(int configId)
        {
            return aideTreasureData.GetOnLineWeChat(configId);
        }

        /// <summary>
        /// 删除在线充值配置
        /// </summary>
        /// <param name="idlist">标识列表</param>
        public int DeleteOnLineWeChat(string idlist)
        {
            return aideTreasureData.DeleteOnLineWeChat(idlist);
        }
        /// <summary>
        /// 新增在线充值配置
        /// </summary>
        /// <param name="config">充值产品</param>
        /// <returns></returns>
        public int InsertOnLineWeChat(OnLineWeChat config)
        {
            return aideTreasureData.InsertOnLineWeChat(config);
        }
        /// <summary>
        /// 修改在线充值配置
        /// </summary>
        /// <param name="config">充值产品</param>
        /// <returns></returns>
        public int UpdateOnLineWeChat(OnLineWeChat config)
        {
            return aideTreasureData.UpdateOnLineWeChat(config);
        }

        /// <summary>
        /// 获取订单信息
        /// </summary>
        /// <param name="orderId"></param>
        /// <returns></returns>
        public OnLinePayOrder GetOnLinePayOrder(string orderId)
        {
            return aideTreasureData.GetOnLinePayOrder(orderId);
        }

        /// <summary>
        /// 充值处理订单(仅在订单状态为1的时候使用)
        /// </summary>
        /// <param name="ordersId"></param>
        /// <returns></returns>
        public Message OperationOnLineOrder(string ordersId)
        {
            return aideTreasureData.OperationOnLineOrder(ordersId);
        }
        #endregion

        #region 推广配置
        /// <summary>
        /// 获取推广配置
        /// </summary>
        /// <param name="configId">推广配置id</param>
        /// <returns></returns>
        public SpreadConfig GetSpreadConfig(int configId)
        {
            return aideTreasureData.GetSpreadConfig(configId);
        }
        /// <summary>
        /// 推广配置数量
        /// </summary>
        /// <returns></returns>
        public int SpreadConfigCount()
        {
            return aideTreasureData.SpreadConfigCount();
        }
        /// <summary>
        /// 删除推广配置
        /// </summary>
        /// <param name="configId">推广配置id</param>
        /// <returns></returns>
        public int DeleteSpreadConfig(int configId)
        {
            return aideTreasureData.DeleteSpreadConfig(configId);
        }
        /// <summary>
        /// 新增推广信息
        /// </summary>
        /// <param name="config">推广配置</param>
        /// <returns></returns>
        public int InsertSpreadConfig(SpreadConfig config)
        {
            return aideTreasureData.InsertSpreadConfig(config);
        }
        /// <summary>
        /// 修改推广信息
        /// </summary>
        /// <param name="config">推广配置</param>
        /// <returns></returns>
        public int UpdateSpreadConfig(SpreadConfig config)
        {
            return aideTreasureData.UpdateSpreadConfig(config);
        }

        /// <summary>
        /// 获取推广返利配置
        /// </summary>
        /// <param name="configId"></param>
        /// <returns></returns>
        public SpreadReturnConfig GetSpreadReturnConfig(int configId)
        {
            return aideTreasureData.GetSpreadReturnConfig(configId);
        }

        /// <summary>
        /// 删除推广返利配置
        /// </summary>
        /// <param name="configId"></param>
        /// <returns></returns>
        public int DeleteSpreadReturnConfig(int configId)
        {
            return aideTreasureData.DeleteSpreadReturnConfig(configId);
        }

        /// <summary>
        /// 保存推广返利配置（新增、更新）
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int SaveSpreadReturnConfig(SpreadReturnConfig config)
        {
            return aideTreasureData.SaveSpreadReturnConfig(config);
        }
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
        public PagerSet GetPageDiamondList(string where, string order, int pageIndex, int pageSize)
        {
            return aideTreasureData.GetPageDiamondList(where, order, pageIndex, pageSize);
        }
        /// <summary>
        /// 赠送钻石
        /// </summary>
        /// <param name="diamond">赠送钻石信息</param>
        /// <returns></returns>
        public Message GrantDiamond(RecordGrantDiamond diamond)
        {
            return aideTreasureData.GrantDiamond(diamond);
        }

        /// <summary>
        /// 赠送奖券
        /// </summary>
        /// <param name="AwardTicket">赠送奖券信息</param>
        /// <returns></returns>
        public Message GrantAwardTicket(RecordGrantAwardTicket AwardTicket)
        {
            return aideTreasureData.GrantAwardTicket(AwardTicket);
        }
        /// <summary>
        /// 获取钻石前50名排行
        /// </summary>
        /// <returns></returns>
        public DataSet GetDiamondRank()
        {
            return aideTreasureData.GetDiamondRank();
        }
        /// <summary>
        /// 获取奖券前50名排行
        /// </summary>
        /// <returns></returns>
        public DataSet GetAwardTicketRank()
        {
            return aideTreasureData.GetAwardTicketRank();
        }
        #endregion

        #region 统计总数

        /// <summary>
        /// 系统统计
        /// </summary>
        /// <returns></returns>
        public DataSet GetStatInfo()
        {
            return aideTreasureData.GetStatInfo();
        }
        /// <summary>
        /// 按条件获取已支付总数
        /// </summary>
        /// <param name="where"></param>
        /// <returns></returns>
        public decimal GetTotalPayAmount(string where)
        {
            return aideTreasureData.GetTotalPayAmount(where);
        }
        /// <summary>
        /// 按条件获取已支付订单数
        /// </summary>
        /// <param name="where"></param>
        /// <returns></returns>
        public long GetTotalPayOrderCount(string where)
        {
            return aideTreasureData.GetTotalPayOrderCount(where);
        }
        /// <summary>
        /// 获取钻石统计
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public long GetTotalUserDiamond(string where)
        {
            return aideTreasureData.GetTotalUserDiamond(where);
        }
        /// <summary>
        /// 获取金币分布统计
        /// </summary>
        /// <returns></returns>
        public DataSet GetGoldDistribute()
        {
            return aideTreasureData.GetGoldDistribute();
        }
        /// <summary>
        /// 获取钻石分布统计
        /// </summary>
        /// <returns></returns>
        public DataSet GetDiamondDistribute()
        {
            return aideTreasureData.GetDiamondDistribute();
        }
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
        public PagerSet GetPageScoreList(string where, string order, int pageIndex, int pageSize)
        {
            return aideTreasureData.GetPageScoreList(where, order, pageIndex, pageSize);
        }
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageInoutList(string where, string order, int pageIndex, int pageSize)
        {
            return aideTreasureData.GetPageInoutList(where, order, pageIndex, pageSize);
        }
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageDrawList(string where, string order, int pageIndex, int pageSize)
        {
            return aideTreasureData.GetPageDrawList(where, order, pageIndex, pageSize);
        }
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageInsureList(string where, string order, int pageIndex, int pageSize)
        {
            return aideTreasureData.GetPageInsureList(where, order, pageIndex, pageSize);
        }
        /// <summary>
        /// 赠送金币
        /// </summary>
        /// <param name="rgt">赠送金币信息</param>
        /// <returns></returns>
        public Message GrantTreasure(RecordGrantTreasure rgt)
        {
            return aideTreasureData.GrantTreasure(rgt);
        }
        /// <summary>
        /// 获取兑换金币配置
        /// </summary>
        /// <param name="configId">配置标识</param>
        /// <returns></returns>
        public CurrencyExchConfig GetCurrencyExch(int configId)
        {
            return aideTreasureData.GetCurrencyExch(configId);
        }
        /// <summary>
        /// 删除兑换金币配置
        /// </summary>
        /// <param name="idlist">标识列表</param>
        public int DeleteCurrencyExch(string idlist)
        {
            return aideTreasureData.DeleteCurrencyExch(idlist);
        }
        /// <summary>
        /// 判断兑换金币配置是否存在相同钻石
        /// </summary>
        /// <param name="diamond">兑换钻石数</param>
        /// <returns></returns>
        public bool IsExistCurrencyExch(int diamond)
        {
            return aideTreasureData.IsExistCurrencyExch(diamond);
        }
        /// <summary>
        /// 新增兑换金币配置
        /// </summary>
        /// <param name="config">兑换金币配置</param>
        /// <returns></returns>
        public int InsertCurrencyExch(CurrencyExchConfig config)
        {
            return aideTreasureData.InsertCurrencyExch(config);
        }
        /// <summary>
        /// 修改兑换金币配置
        /// </summary>
        /// <param name="config">兑换金币配置</param>
        /// <returns></returns>
        public int UpdateCurrencyExch(CurrencyExchConfig config)
        {
            return aideTreasureData.UpdateCurrencyExch(config);
        }
        #endregion

        #region 卡线管理

        /// <summary>
        /// 批量清除玩家卡线
        /// </summary>
        /// <param name="userlist"></param>
        /// <returns></returns>
        public int CleanGameScoreLocker(string userlist)
        {
            return aideTreasureData.CleanGameScoreLocker(userlist);
        }

        /// <summary>
        /// 获取在线玩家
        /// </summary>
        /// <param name="userid"></param>
        /// <returns></returns>
        public GameScoreLocker GetOnlineUser(int userid)
        {
            return aideTreasureData.GetOnlineUser(userid);
        }

        #endregion
    }
}
