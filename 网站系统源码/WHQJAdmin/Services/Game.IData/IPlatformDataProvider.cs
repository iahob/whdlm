using System.Collections.Generic;
using System.Data;

using Game.Kernel;
using Game.Entity.Platform;
using Game.Entity.Record;

namespace Game.IData
{
    /// <summary>
    /// 平台库数据层接口
    /// </summary>
    public interface IPlatformDataProvider //: IProvider
    {
        #region 数据库连接字符
        /// <summary>
        /// 获取积分库的连接串
        /// </summary>
        /// <param name="kindId">游戏标识</param>
        /// <returns></returns>
        string GetConn(int kindId);
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
        PagerSet GetList(string tableName, string where, string order, int pageIndex, int pageSize, string[] fields = null);
        #endregion 公用分页

        #region 游戏模块
        /// <summary>
        /// 获取游戏模块
        /// </summary>
        /// <param name="gameId">游戏模块标识</param>
        /// <returns></returns>
        GameGameItem GetGameGameItemInfo(int gameId);
        /// <summary>
        /// 新增游戏模块
        /// </summary>
        /// <param name="gameGameItem">游戏模块</param>
        int InsertGameGameItem(GameGameItem gameGameItem);
        /// <summary>
        /// 修改游戏模块
        /// </summary>
        /// <param name="gameGameItem">游戏模块</param>
        int UpdateGameGameItem(GameGameItem gameGameItem);
        /// <summary>
        /// 删除游戏模块
        /// </summary>
        /// <param name="gamelist">游戏标识列表</param>
        int DeleteGameGameItem(string gamelist);
        /// <summary>
        /// 删除游戏信息
        /// </summary>
        /// <param name="kindlist">游戏标识列表</param>
        int DeleteMobileKindItem(string kindlist);

        /// <summary>
        /// 删除U3D游戏信息
        /// </summary>
        /// <param name="kindlist">游戏标识列表</param>
        int DeleteU3DGameKindItem(string kindlist);

        /// <summary>
        /// 删除H5游戏信息
        /// </summary>
        /// <param name="kindlist">游戏标识列表</param>
        int DeleteH5GameKindItem(string kindlist);


        /// <summary>
        /// 获取游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        MobileKindItem GetMobileKindItemInfo(int kindid);

        /// <summary>
        /// 获取U3D游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        U3DGameKindItem GetU3DGameKindItemInfo(int kindid);

        /// <summary>
        /// 获取H5游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        H5GameKindItem GetH5GameKindItemInfo(int kindid);

        /// <summary>
        /// 获取启用游戏列表
        /// </summary>
        /// <returns></returns>
        IList<MobileKindItem> GetMobileKindItemList();
        /// <summary>
        /// 新增游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        int InsertMobileKindItem(MobileKindItem item);
        /// <summary>
        /// 新增U3D游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        int InsertU3DGameKindItem(U3DGameKindItem item);

        /// <summary>
        /// 新增U3D游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        int InsertH5GameKindItem(H5GameKindItem item);

        /// <summary>
        /// 修改游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        int UpdateMobileKindItem(MobileKindItem item);

        /// <summary>
        /// 修改U3D游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        int UpdateU3DGameKindItem(U3DGameKindItem item);

        /// <summary>
        /// 修改H5游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        int UpdateH5GameKindItem(H5GameKindItem item);

        /// <summary>
        /// 批量修改时游戏可用性
        /// </summary>
        /// <param name="idList">idlist（0,x,x,x）</param>
        /// <param name="nullity">0：启用 1：禁用</param>
        /// <returns></returns>
        int ChangeMobileKindNullity(string idList, int nullity);

         /// <summary>
         /// 批量修改U3D游戏可用性
         /// </summary>
         /// <param name="idList">idlist（0,x,x,x）</param>
         /// <param name="nullity">0：启用 1：禁用</param>
         /// <returns></returns>
        int ChangeU3DGameKindNullity(string idList, int nullity);

        /// <summary>
        /// 批量修改H5游戏可用性
        /// </summary>
        /// <param name="idList">idlist（0,x,x,x）</param>
        /// <param name="nullity">0：启用 1：禁用</param>
        /// <returns></returns>
        int ChangeH5GameKindNullity(string idList, int nullity);

        /// <summary>
        /// 获取游戏列表
        /// </summary>
        /// <returns></returns>
        IList<GameKindItem> GetGameList();
        #endregion

        #region 机器管理
        /// <summary>
        /// 获取机器信息
        /// </summary>
        /// <param name="dBInfoId">标识</param>
        /// <returns></returns>
        DataBaseInfo GetDataBaseInfo(int dBInfoId);
        /// <summary>
        /// 获取机器信息
        /// </summary>
        /// <param name="dBAddr">机器地址</param>
        /// <returns></returns>
        DataBaseInfo GetDataBaseInfo(string dBAddr);
        /// <summary>
        /// 新增机器信息
        /// </summary>
        /// <param name="dataBase">机器信息</param>
        /// <returns></returns>
        int InsertDataBase(DataBaseInfo dataBase);
        /// <summary>
        ///  修改机器信息
        /// </summary>
        /// <param name="dataBase">机器信息</param>
        /// <returns></returns>
        int UpdateDataBase(DataBaseInfo dataBase);
        /// <summary>
        /// 删除机器信息
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <returns></returns>
        int DeleteDataBase(string idlist);
        #endregion

        #region 游戏房间
        /// <summary>
        /// 获取游戏房间
        /// </summary>
        /// <param name="serverId">房间标识</param>
        /// <returns></returns>
        GameRoomInfo GetGameRoomInfoInfo(int serverId);
        /// <summary>
        /// 获取游戏和房间集合
        /// </summary>
        /// <returns></returns>
        DataSet GetKindAndRoomList();
        #endregion

        #region 系统消息
        /// <summary>
        /// 获取系统消息
        /// </summary>
        /// <param name="id">消息标识</param>
        /// <returns></returns>
        SystemMessage GetSystemMessageInfo(int id);
        /// <summary>
        /// 新增系统消息
        /// </summary>
        /// <param name="systemMessage">系统消息</param>
        /// <returns></returns>
        int InsertSystemMessage(SystemMessage systemMessage);
        /// <summary>
        /// 修改系统消息
        /// </summary>
        /// <param name="systemMessage">系统消息</param>
        /// <returns></returns>
        int UpdateSystemMessage(SystemMessage systemMessage);
        /// <summary>
        /// 删除系统消息
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        int DeleteSystemMessage(string where);
        /// <summary>
        /// 冻结解冻系统消息
        /// </summary>
        /// <param name="idList">标识字符集</param>
        /// <param name="nullity">状态值</param>
        /// <returns></returns>
        int NullitySystemMessage(string idList, int nullity);
        #endregion

        #region 约战信息
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageGoldBattleList(string where, string order, int pageIndex, int pageSize);
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageScoreBattleList(string where, string order, int pageIndex, int pageSize);
        /// <summary>
        /// 获取约战房间信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        DataSet GetCreateRoomInfo(int userid);
        /// <summary>
        /// 获取约战房间开房前50名
        /// </summary>
        /// <returns></returns>
        DataSet GetCreateRoomRank();
        /// <summary>
        /// 获取约战详情
        /// </summary>
        /// <param name="roomid">房间号</param>
        /// <param name="stime">记录起始时间</param>
        /// <param name="etime">记录结束时间</param>
        /// <param name="playmode">约战类型</param>
        /// <param name="hostid">房主标识</param>
        /// <returns></returns>
        DataSet GetBattleInfoByRoom(int roomid, string stime, string etime, int playmode, int hostid);
        #endregion

        #region 统计总数
        /// <summary>
        /// 获取创建房间钻石统计
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        long GetTotalCreateRoomDiamond(string where);
        /// <summary>
        /// 获取创建房间数量
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        long GetTotalCreateRoomTable(string where);
        /// <summary>
        /// 获取AA创建房间钻石统计
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        long GetAaTotalCreateRoomDiamond(string where);
        /// <summary>
        /// 获取在线人数统计
        /// </summary>
        /// <param name="sTime">起始时间</param>
        /// <param name="eTime">结束时间</param>
        /// <returns></returns>
        IList<StatisticsOnline> GetUserOnlineStatistics(string sTime, string eTime);
        #endregion

        #region 道具管理
        /// <summary>
        /// 获取道具信息
        /// </summary>
        /// <param name="id">道具标识</param>
        /// <param name="typeid">购买类型</param>
        /// <returns></returns>
        GameProperty GetPropertyInfo(int id, int typeid);

        /// <summary>
        /// 修改道具信息
        /// </summary>
        /// <param name="property">道具信息</param>
        /// <returns></returns>
        int UpdatePropertyInfo(GameProperty property);

        #endregion

        #region 签到管理

        /// <summary>
        /// 获取签到礼包配置列表
        /// </summary>
     
        /// <returns></returns>
        DataSet GetGamePackageList();
        /// <summary>
        /// 获取签到礼包配置
        /// </summary>
        /// <param name="PackageID">礼包标识id</param>
        /// <returns></returns>
        GamePackage GetGamePackage(int PackageID);

        /// <summary>
        /// 新增签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        int InsertGamePackage(GamePackage config);
        /// <summary>
        /// 修改签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        int UpdateGamePackage(GamePackage config);
        /// <summary>
        /// 删除签到礼包配置
        /// </summary>
        /// <param name="PackageID">礼包标识id</param>
        /// <returns></returns>
        int DeleteGamePackage(string PackageID);




        /// <summary>
        /// 获取签到礼包物品配置
        /// </summary>
        /// <param name="GoodsID">物品标识id</param>
        /// <returns></returns>
        GamePackageGoods GetGamePackageGoods(int GoodsID);
        /// <summary>
        /// 新增签到礼包物品配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        int InsertGamePackageGoods(GamePackageGoods config);

        /// <summary>
        /// 修改签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        int UpdateGamePackageGoods(GamePackageGoods config);
        /// <summary>
        /// 删除签到礼包物品配置
        /// </summary>
        /// <param name="GoodsID">物品标识id</param>
        /// <returns></returns>
        int DeleteGamePackageGoods(string GoodsID);





        /// <summary>
        /// 获取签到配置
        /// </summary>
        /// <param name="SignID">签到配置id</param>
        /// <returns></returns>
        GameSignIn GetGameSignIn(int SignID);
        /// <summary>
        /// 新增签到配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        int InsertGameSignIn(GameSignIn config);
        /// <summary>
        /// 修改签到配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        int UpdateGameSignIn(GameSignIn config);
        /// 删除签到配置
        /// </summary>
        /// <param name="SignID">签到配置id</param>
        /// <returns></returns>
        int DeleteGameSignIn(string SignID);

        #endregion

        #region 任务管理
        /// <summary>
        /// 获取任务信息
        /// </summary>
        /// <param name="taskid">任务标识</param>
        /// <returns></returns>
        TaskInfo GetTaskInfo(int taskid);
        /// <summary>
        /// 新增任务
        /// </summary>
        /// <param name="info">任务信息</param>
        /// <returns></returns>
        int InsertTaskInfo(TaskInfo info);
        /// <summary>
        /// 修改任务
        /// </summary>
        /// <param name="info">任务信息</param>
        /// <returns></returns>
        int UpdateTaskInfo(TaskInfo info);
        #endregion

        #region 录入敏感字符

        /// <summary>
        /// 录入敏感字符
        /// </summary>
        /// <param name="word"></param>
        /// <returns></returns>
        int AddSensitiveWords(string word);


        /// <summary>
        /// 是否存在相同敏感字符
        /// </summary>
        /// <param name="word"></param>
        /// <returns></returns>
        bool IsExsitSensitiveWords(string word);

        #endregion

        #region 调试系统

        #region AI人数调整

        /// <summary>
        /// 获取AI人数调整实体 by aiTime
        /// </summary>
        /// <param name="aiTime"></param>
        /// <returns></returns>
        AITimeChange GetAITimeChangeInfo(int aiTime);

        /// <summary>
        /// 保存AI人数调整
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        int SaveAITimeChangeInfo(AITimeChange model);

        #endregion

        #region AI进入房间权重配置

        /// <summary>
        /// 获取AI进入房间权重配置实体
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        AIWeightInRatio GetAIWeightInRatioInfo(int Id);

        /// <summary>
        /// 保存AI进入房间权重配置
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        int SaveAIWeightInRatioInfo(AIWeightInRatio model);
        #endregion

        #region AI离开俱乐部权重配置

        /// <summary>
        /// 获取AI离开俱乐部权重配置实体
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        AIWeightOutRatio GetAIWeightOutRatioInfo(int Id);

        /// <summary>
        /// 保存AI离开俱乐部权重配置
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        int SaveAIWeightOutRatioInfo(AIWeightOutRatio model);

        /// <summary>
        /// 获取当前时间的下线权重
        /// </summary>
        /// <param name="TimeCount"></param>
        /// <returns></returns>
        int GetAiWeightOutRatioByTime(double TimeCount);

        /// <summary>
        /// 获取当前在线玩家房间号
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="userId">玩家ID</param>
        /// <returns></returns>
        int GetOnlineUeserRoomID(int groupId, int userId);

        /// <summary>
        /// 获取支持联盟的游戏列表
        /// </summary>
        /// <returns></returns>
        DataSet GetGroupKindList();

        #endregion


        #endregion
    }
}
