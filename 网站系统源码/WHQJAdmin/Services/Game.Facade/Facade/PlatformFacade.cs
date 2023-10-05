using System.Collections.Generic;
using System.Data;

using Game.IData;
using Game.Data.Factory;
using Game.Entity.Platform;
using Game.Kernel;
using Game.Entity.Record;

// ReSharper disable once CheckNamespace
namespace Game.Facade
{
    /// <summary>
    /// 平台库外观
    /// </summary>
    public class PlatformFacade
    {
        #region Fields

        // ReSharper disable once FieldCanBeMadeReadOnly.Local
        private IPlatformDataProvider _aidePlatformData;

        #endregion

        #region 构造函数

        /// <summary>
        /// 构造函数
        /// </summary>
        public PlatformFacade()
        {
            _aidePlatformData = ClassFactory.GetIPlatformDataProvider();
        }
        #endregion

        #region 数据库连接字符
        /// <summary>
        /// 获取积分库的连接串
        /// </summary>
        /// <param name="kindId">游戏标识</param>
        /// <returns></returns>
        public string GetConn(int kindId)
        {
            return _aidePlatformData.GetConn(kindId);
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
            return _aidePlatformData.GetList(tableName, where, order, pageIndex, pageSize, fields);
        }
        #endregion 公用分页

        #region 游戏模块
        /// <summary>
        /// 获取游戏模块
        /// </summary>
        /// <param name="gameId">游戏模块标识</param>
        /// <returns></returns>
        public GameGameItem GetGameGameItemInfo(int gameId)
        {
            return _aidePlatformData.GetGameGameItemInfo(gameId);
        }
        /// <summary>
        /// 新增游戏模块
        /// </summary>
        /// <param name="gameGameItem">游戏模块</param>
        public int InsertGameGameItem(GameGameItem gameGameItem)
        {
            return _aidePlatformData.InsertGameGameItem(gameGameItem);
        }
        /// <summary>
        /// 修改游戏模块
        /// </summary>
        /// <param name="gameGameItem">游戏模块</param>
        public int UpdateGameGameItem(GameGameItem gameGameItem)
        {
            return _aidePlatformData.UpdateGameGameItem(gameGameItem);
        }
        /// <summary>
        /// 删除游戏模块
        /// </summary>
        /// <param name="gamelist">游戏标识列表</param>
        public int DeleteGameGameItem(string gamelist)
        {
            return _aidePlatformData.DeleteGameGameItem(gamelist);
        }

        /// <summary>
        /// 删除游戏信息
        /// </summary>
        /// <param name="kindList">游戏标识列表</param>
        public int DeleteMobileKindItem(string kindList)
        {
            return _aidePlatformData.DeleteMobileKindItem(kindList);
        }

        /// <summary>
        /// 删除U3D游戏信息
        /// </summary>
        /// <param name="kindList">游戏标识列表</param>
        public int DeleteU3DGameKindItem(string kindList)
        {
            return _aidePlatformData.DeleteU3DGameKindItem(kindList);
        }

        /// <summary>
        /// 删除H5游戏信息
        /// </summary>
        /// <param name="kindList">游戏标识列表</param>
        public int DeleteH5GameKindItem(string kindList)
        {
            return _aidePlatformData.DeleteH5GameKindItem(kindList);
        }

        /// <summary>
        /// 获取游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        public MobileKindItem GetMobileKindItemInfo(int kindid)
        {
            return _aidePlatformData.GetMobileKindItemInfo(kindid);
        }

        /// <summary>
        /// 获取U3D游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        public U3DGameKindItem GetU3DGameKindItemInfo(int kindid)
        {
            return _aidePlatformData.GetU3DGameKindItemInfo(kindid);
        }

        /// <summary>
        /// 获取H5游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        public H5GameKindItem GetH5GameKindItemInfo(int kindid)
        {
            return _aidePlatformData.GetH5GameKindItemInfo(kindid);
        }

        /// <summary>
        /// 获取启用游戏列表
        /// </summary>
        /// <returns></returns>
        public IList<MobileKindItem> GetMobileKindItemList()
        {
            return _aidePlatformData.GetMobileKindItemList();
        }
        /// <summary>
        /// 新增游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int InsertMobileKindItem(MobileKindItem item)
        {
            return _aidePlatformData.InsertMobileKindItem(item);
        }

        /// <summary>
        /// 新增U3D游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int InsertU3DGameKindItem(U3DGameKindItem item)
        {
            return _aidePlatformData.InsertU3DGameKindItem(item);
        }

        /// <summary>
        /// 新增H5游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int InsertH5GameKindItem(H5GameKindItem item)
        {
            return _aidePlatformData.InsertH5GameKindItem(item);
        }


        /// <summary>
        /// 修改游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int UpdateMobileKindItem(MobileKindItem item)
        {
            return _aidePlatformData.UpdateMobileKindItem(item);
        }

        /// <summary>
        /// 修改U3D游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int UpdateU3DGameKindItem(U3DGameKindItem item)
        {
            return _aidePlatformData.UpdateU3DGameKindItem(item);
        }

        /// <summary>
        /// 修改H5游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int UpdateH5GameKindItem(H5GameKindItem item)
        {
            return _aidePlatformData.UpdateH5GameKindItem(item);
        }


        /// <summary>
        /// 批量修改时游戏可用性
        /// </summary>
        /// <param name="idList">idlist（0,x,x,x）</param>
        /// <param name="nullity">0：启用 1：禁用</param>
        /// <returns></returns>
        public int ChangeMobileKindNullity(string idList, int nullity)
        {
            return _aidePlatformData.ChangeMobileKindNullity(idList, nullity);
        }

        /// <summary>
        /// 批量修改U3D游戏可用性
        /// </summary>
        /// <param name="idList">idlist（0,x,x,x）</param>
        /// <param name="nullity">0：启用 1：禁用</param>
        /// <returns></returns>
        public int ChangeU3DGameKindNullity(string idList, int nullity)
        {
            return _aidePlatformData.ChangeU3DGameKindNullity(idList, nullity);
        }

        /// <summary>
        /// 批量修改H5游戏可用性
        /// </summary>
        /// <param name="idList">idlist（0,x,x,x）</param>
        /// <param name="nullity">0：启用 1：禁用</param>
        /// <returns></returns>
        public int ChangeH5GameKindNullity(string idList, int nullity)
        {
            return _aidePlatformData.ChangeH5GameKindNullity(idList, nullity);
        }

        /// <summary>
        /// 获取游戏列表
        /// </summary>
        /// <returns></returns>
        public IList<GameKindItem> GetGameList()
        {
            return _aidePlatformData.GetGameList();
        }
        #endregion

        #region 机器管理
        /// <summary>
        /// 获取机器信息
        /// </summary>
        /// <param name="dBInfoId">标识</param>
        /// <returns></returns>
        public DataBaseInfo GetDataBaseInfo(int dBInfoId)
        {
            return _aidePlatformData.GetDataBaseInfo(dBInfoId);
        }
        /// <summary>
        /// 获取机器信息
        /// </summary>
        /// <param name="dBAddr">机器地址</param>
        /// <returns></returns>
        public DataBaseInfo GetDataBaseInfo(string dBAddr)
        {
            return _aidePlatformData.GetDataBaseInfo(dBAddr);
        }
        /// <summary>
        /// 新增机器信息
        /// </summary>
        /// <param name="dataBase">机器信息</param>
        /// <returns></returns>
        public int InsertDataBase(DataBaseInfo dataBase)
        {
            return _aidePlatformData.InsertDataBase(dataBase);
        }
        /// <summary>
        ///  修改机器信息
        /// </summary>
        /// <param name="dataBase">机器信息</param>
        /// <returns></returns>
        public int UpdateDataBase(DataBaseInfo dataBase)
        {
            return _aidePlatformData.UpdateDataBase(dataBase);
        }
        /// <summary>
        /// 删除机器信息
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <returns></returns>
        public int DeleteDataBase(string idlist)
        {
            return _aidePlatformData.DeleteDataBase(idlist);
        }
        #endregion

        #region 游戏房间
        /// <summary>
        /// 获取游戏房间
        /// </summary>
        /// <param name="serverId">房间标识</param>
        /// <returns></returns>
        public GameRoomInfo GetGameRoomInfoInfo(int serverId)
        {
            return _aidePlatformData.GetGameRoomInfoInfo(serverId);
        }
        /// <summary>
        /// 获取游戏和房间集合
        /// </summary>
        /// <returns></returns>
        public DataSet GetKindAndRoomList()
        {
            return _aidePlatformData.GetKindAndRoomList();
        }
        #endregion

        #region 系统消息
        /// <summary>
        /// 获取系统消息
        /// </summary>
        /// <param name="id">消息标识</param>
        /// <returns></returns>
        public SystemMessage GetSystemMessageInfo(int id)
        {
            return _aidePlatformData.GetSystemMessageInfo(id);
        }
        /// <summary>
        /// 新增系统消息
        /// </summary>
        /// <param name="systemMessage">系统消息</param>
        /// <returns></returns>
        public int InsertSystemMessage(SystemMessage systemMessage)
        {
            return _aidePlatformData.InsertSystemMessage(systemMessage);
        }
        /// <summary>
        /// 修改系统消息
        /// </summary>
        /// <param name="systemMessage">系统消息</param>
        /// <returns></returns>
        public int UpdateSystemMessage(SystemMessage systemMessage)
        {
            return _aidePlatformData.UpdateSystemMessage(systemMessage);
        }
        /// <summary>
        /// 删除系统消息
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public int DeleteSystemMessage(string where)
        {
            return _aidePlatformData.DeleteSystemMessage(where);
        }
        /// <summary>
        /// 冻结解冻系统消息
        /// </summary>
        /// <param name="idList">标识字符集</param>
        /// <param name="nullity">状态值</param>
        /// <returns></returns>
        public int NullitySystemMessage(string idList, int nullity)
        {
            return _aidePlatformData.NullitySystemMessage(idList, nullity);
        }
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
        public PagerSet GetPageGoldBattleList(string where, string order, int pageIndex, int pageSize)
        {
            return _aidePlatformData.GetPageGoldBattleList(where, order, pageIndex, pageSize);
        }
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageScoreBattleList(string where, string order, int pageIndex, int pageSize)
        {
            return _aidePlatformData.GetPageScoreBattleList(where, order, pageIndex, pageSize);
        }
        /// <summary>
        /// 获取约战房间信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        public DataSet GetCreateRoomInfo(int userid)
        {
            return _aidePlatformData.GetCreateRoomInfo(userid);
        }
        /// <summary>
        /// 获取约战房间开房前50名
        /// </summary>
        /// <returns></returns>
        public DataSet GetCreateRoomRank()
        {
            return _aidePlatformData.GetCreateRoomRank();
        }
        /// <summary>
        /// 获取约战详情
        /// </summary>
        /// <param name="roomid">房间号</param>
        /// <param name="stime">记录起始时间</param>
        /// <param name="etime">记录结束时间</param>
        /// <param name="playmode">约战类型</param>
        /// <param name="hostid">房主标识</param>
        /// <returns></returns>
        public DataSet GetBattleInfoByRoom(int roomid, string stime, string etime, int playmode, int hostid)
        {
            return _aidePlatformData.GetBattleInfoByRoom(roomid, stime, etime, playmode, hostid);
        }
        #endregion

        #region 统计总数
        /// <summary>
        /// 获取创建房间钻石统计
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public long GetTotalCreateRoomDiamond(string where)
        {
            return _aidePlatformData.GetTotalCreateRoomDiamond(where);
        }
        /// <summary>
        /// 获取创建房间数量
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public long GetTotalCreateRoomTable(string where)
        {
            return _aidePlatformData.GetTotalCreateRoomTable(where);
        }
        /// <summary>
        /// 获取AA创建房间钻石统计
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public long GetAaTotalCreateRoomDiamond(string where)
        {
            return _aidePlatformData.GetAaTotalCreateRoomDiamond(where);
        }
        /// <summary>
        /// 获取在线人数统计
        /// </summary>
        /// <param name="sTime">起始时间</param>
        /// <param name="eTime">结束时间</param>
        /// <returns></returns>
        public IList<StatisticsOnline> GetUserOnlineStatistics(string sTime, string eTime)
        {
            return _aidePlatformData.GetUserOnlineStatistics(sTime, eTime);
        }
        #endregion

        #region 道具管理

        /// <summary>
        /// 获取道具信息
        /// </summary>
        /// <param name="id">道具标识</param>
        /// <param name="typeid">购买类型</param>
        /// <returns></returns>
        public GameProperty GetPropertyInfo(int id, int typeid)
        {
           return _aidePlatformData.GetPropertyInfo(id, typeid);
        }

        /// <summary>
        /// 修改道具信息
        /// </summary>
        /// <param name="property">道具信息</param>
        /// <returns></returns>
        public int UpdatePropertyInfo(GameProperty property)
        {
            return _aidePlatformData.UpdatePropertyInfo(property);
        }

        #endregion
        #region 签到管理

        /// <summary>
        /// 获取签到礼包配置列表
        /// </summary>
       
        /// <returns></returns>
        public DataSet GetGamePackageList()
        {
            return _aidePlatformData.GetGamePackageList();
        }
        /// <summary>
        /// 获取签到礼包配置
        /// </summary>
        /// <param name="PackageID">礼包标识id</param>
        /// <returns></returns>
        public GamePackage GetGamePackage(int PackageID)
        {
            return _aidePlatformData.GetGamePackage(PackageID);
        }
        /// <summary>
        /// 新增签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int InsertGamePackage(GamePackage config)
        {

            return _aidePlatformData.InsertGamePackage(config);
        }
        /// <summary>
        /// 修改签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int UpdateGamePackage(GamePackage config)
        {
            return _aidePlatformData.UpdateGamePackage(config);
        }
        /// <summary>
        /// 删除签到礼包配置
        /// </summary>
        /// <param name="PackageID">礼包标识id</param>
        /// <returns></returns>
        public int DeleteGamePackage(string PackageID)
        {
            return _aidePlatformData.DeleteGamePackage(PackageID);
        }


        /// <summary>
        /// 获取签到礼包物品配置
        /// </summary>
        /// <param name="GoodsID">物品标识id</param>
        /// <returns></returns>
        public GamePackageGoods GetGamePackageGoods(int GoodsID)
        {
            return _aidePlatformData.GetGamePackageGoods(GoodsID);
        }

        /// <summary>
        /// 新增签到礼包物品配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int InsertGamePackageGoods(GamePackageGoods config)
        {
            return _aidePlatformData.InsertGamePackageGoods(config);
        }
        /// <summary>
        /// 修改签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int UpdateGamePackageGoods(GamePackageGoods config)
        {
            return _aidePlatformData.UpdateGamePackageGoods(config);
        }
        /// <summary>
        /// 删除签到礼包物品配置
        /// </summary>
        /// <param name="GoodsID">物品标识id</param>
        /// <returns></returns>
        public int DeleteGamePackageGoods(string GoodsID)
        {
            return _aidePlatformData.DeleteGamePackageGoods(GoodsID);
        }

        /// <summary>
        /// 获取签到配置
        /// </summary>
        /// <param name="SignID">签到配置id</param>
        /// <returns></returns>
        public GameSignIn GetGameSignIn(int SignID)
        {
            return _aidePlatformData.GetGameSignIn(SignID);
        }
        /// <summary>
        /// 新增签到配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int InsertGameSignIn(GameSignIn config)
        {
            return _aidePlatformData.InsertGameSignIn(config);
        }
        /// <summary>
        /// 修改签到配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int UpdateGameSignIn(GameSignIn config)
        {
            return _aidePlatformData.UpdateGameSignIn(config);
        }
        /// <summary>
        /// 删除签到配置
        /// </summary>
        /// <param name="SignID">签到配置id</param>
        /// <returns></returns>
        public int DeleteGameSignIn(string SignID)
        {
            return _aidePlatformData.DeleteGameSignIn(SignID);

        }
        #endregion
        #region 任务管理
        /// <summary>
        /// 获取任务信息
        /// </summary>
        /// <param name="taskid">任务标识</param>
        /// <returns></returns>
        public TaskInfo GetTaskInfo(int taskid)
        {
            return _aidePlatformData.GetTaskInfo(taskid);
        }
        /// <summary>
        /// 新增任务
        /// </summary>
        /// <param name="info">任务信息</param>
        /// <returns></returns>
        public int InsertTaskInfo(TaskInfo info)
        {
            return _aidePlatformData.InsertTaskInfo(info);
        }
        /// <summary>
        /// 修改任务
        /// </summary>
        /// <param name="info">任务信息</param>
        /// <returns></returns>
        public int UpdateTaskInfo(TaskInfo info)
        {
            return _aidePlatformData.UpdateTaskInfo(info);
        }
        #endregion

        #region 录入敏感字符

        /// <summary>
        /// 录入敏感字符
        /// </summary>
        /// <param name="word"></param>
        /// <returns></returns>
        public int AddSensitiveWords(string word)
        {
            return _aidePlatformData.AddSensitiveWords(word);
        }

        /// <summary>
        /// 是否存在相同敏感字符
        /// </summary>
        /// <param name="word"></param>
        /// <returns></returns>
        public bool IsExsitSensitiveWords(string word)
        {
            return _aidePlatformData.IsExsitSensitiveWords(word);
        }

        #endregion

        #region 调试系统

        #region AI人数调整
        /// <summary>
        /// 获取AI人数调整实体 by Id
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        public AITimeChange GetAITimeChangeInfo(int aiTime)
        {
            return _aidePlatformData.GetAITimeChangeInfo(aiTime);
        }

        /// <summary>
        /// 保存AI人数调整
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int SaveAITimeChangeInfo(AITimeChange model)
        {
            return _aidePlatformData.SaveAITimeChangeInfo(model);
        }
        #endregion

        #region AI进入房间权重配置
        /// <summary>
        /// 获取AI进入房间权重配置实体
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        public AIWeightInRatio GetAIWeightInRatioInfo(int Id)
        {
            return _aidePlatformData.GetAIWeightInRatioInfo(Id);
        }

        /// <summary>
        /// 保存AI进入房间权重配置
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int SaveAIWeightInRatioInfo(AIWeightInRatio model)
        {
            return _aidePlatformData.SaveAIWeightInRatioInfo(model);
        }
        #endregion

        #region AI离开俱乐部权重配置
        /// <summary>
        /// 获取AI离开俱乐部权重配置实体
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        public AIWeightOutRatio GetAIWeightOutRatioInfo(int Id)
        {
            return _aidePlatformData.GetAIWeightOutRatioInfo(Id);
        }

        /// <summary>
        /// 保存AI离开俱乐部权重配置
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int SaveAIWeightOutRatioInfo(AIWeightOutRatio model)
        {
            return _aidePlatformData.SaveAIWeightOutRatioInfo(model);
        }

        /// <summary>
        /// 获取当前时间的下线权重
        /// </summary>
        /// <param name="TimeCount"></param>
        /// <returns></returns>
        public int GetAiWeightOutRatioByTime(double TimeCount)
        {
            return _aidePlatformData.GetAiWeightOutRatioByTime(TimeCount);
        }

        /// <summary>
        /// 获取当前在线玩家房间号
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="userId">玩家ID</param>
        /// <returns></returns>
        public int GetOnlineUeserRoomID(int groupId, int userId)
        {
            return _aidePlatformData.GetOnlineUeserRoomID(groupId, userId);
        }

        /// <summary>
        /// 获取支持联盟的游戏列表
        /// </summary>
        /// <returns></returns>
        public DataSet GetGroupKindList()
        {
            return _aidePlatformData.GetGroupKindList();
        }

        #endregion


        #endregion
    }
}
