using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;

using Game.Kernel;
using Game.IData;
using Game.Entity.Platform;
using Game.Entity.Record;
using System.Text;

namespace Game.Data
{
    /// <summary>
    /// 平台库数据层
    /// </summary>
    public class PlatformDataProvider : BaseDataProvider, IPlatformDataProvider
    {
        #region 构造方法
        public PlatformDataProvider(string connString)
            : base(connString)
        {
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
            GameGameItem game = GetGameGameItemInfo(kindId);
            if (game != null)
            {
                DataBaseInfo database = GetDataBaseInfo(game.DataBaseAddr);
                if (database != null)
                {
                    string userId = Utils.CWHEncryptNet.XorCrevasse(database.DBUser);
                    string password = Utils.CWHEncryptNet.XorCrevasse(database.DBPassword);
                    return
                        $"Data Source={game.DataBaseAddr + (string.IsNullOrEmpty(database.DBPort.ToString()) ? "" : ("," + database.DBPort.ToString()))}; Initial Catalog={game.DataBaseName}; User ID={userId}; Password={password}; Pooling=true";
                }
            }
            return "";
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
        /// <param name="tableKey">表唯一主键</param>
        /// <param name="fields">查询字段</param>
        /// <returns></returns>
        public PagerSet GetList(string tableName, string where, string order, int pageIndex, int pageSize, string[] fields = null)
        {
            PagerParameters pagerPrams = new PagerParameters(tableName, where, order, pageIndex, pageSize, fields);
            return GetPagerSet(pagerPrams);
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
            string sqlQuery = $"SELECT * FROM GameGameItem WITH(NOLOCK) WHERE GameID={gameId}";
            return Database.ExecuteObject<GameGameItem>(sqlQuery);
        }
        /// <summary>
        /// 新增游戏模块
        /// </summary>
        /// <param name="gameGameItem">游戏模块</param>
        public int InsertGameGameItem(GameGameItem gameGameItem)
        {
            string sqlQuery = @"INSERT INTO GameGameItem VALUES(@GameID,@GameName,@SuportType,@DataBaseAddr,
                            @DataBaseName,@ServerVersion,@ClientVersion,@ServerDLLName,@ClientExeName)";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("GameID", gameGameItem.GameID),
                Database.MakeInParam("GameName", gameGameItem.GameName),
                Database.MakeInParam("SuportType", gameGameItem.SuportType),
                Database.MakeInParam("DataBaseAddr", gameGameItem.DataBaseAddr),
                Database.MakeInParam("DataBaseName", gameGameItem.DataBaseName),
                Database.MakeInParam("ServerVersion", gameGameItem.ServerVersion),
                Database.MakeInParam("ClientVersion", gameGameItem.ClientVersion),
                Database.MakeInParam("ServerDLLName", gameGameItem.ServerDLLName),
                Database.MakeInParam("ClientExeName", gameGameItem.ClientExeName)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 修改游戏模块
        /// </summary>
        /// <param name="gameGameItem">游戏模块</param>
        public int UpdateGameGameItem(GameGameItem gameGameItem)
        {
            string sqlQuery = @"UPDATE GameGameItem SET GameName=@GameName,SuportType=@SuporType,DataBaseAddr=@DataBaseAddr,
                        DataBaseName=@DataBaseName,ServerVersion=@ServerVersion,ClientVersion=@ClientVersion,
                        ServerDLLName=@ServerDLLName,ClientExeName=@ClientExeName WHERE GameID=@GameID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("GameName", gameGameItem.GameName),
                Database.MakeInParam("SuporType", gameGameItem.SuportType),
                Database.MakeInParam("DataBaseAddr", gameGameItem.DataBaseAddr),
                Database.MakeInParam("DataBaseName", gameGameItem.DataBaseName),
                Database.MakeInParam("ServerVersion", gameGameItem.ServerVersion),
                Database.MakeInParam("ClientVersion", gameGameItem.ClientVersion),
                Database.MakeInParam("ServerDLLName", gameGameItem.ServerDLLName),
                Database.MakeInParam("ClientExeName", gameGameItem.ClientExeName),
                Database.MakeInParam("GameID", gameGameItem.GameID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 删除游戏模块
        /// </summary>
        /// <param name="gamelist">游戏标识列表</param>
        public int DeleteGameGameItem(string gamelist)
        {
            string sqlQuery = $"DELETE GameGameItem WHERE GameID IN({gamelist})";
            return Database.ExecuteNonQuery(sqlQuery);
        }
        /// <summary>
        /// 删除游戏信息
        /// </summary>
        /// <param name="kindlist">游戏标识列表</param>
        public int DeleteMobileKindItem(string kindlist)
        {
            string sql = $"DELETE MobileKindItem WHERE KindID IN ({kindlist})";
            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 删除U3D游戏信息
        /// </summary>
        /// <param name="kindlist">游戏标识列表</param>
        public int DeleteU3DGameKindItem(string kindlist)
        {
            string sql = $"DELETE U3DGameKindItem WHERE KindID IN ({kindlist})";
            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 删除H5游戏信息
        /// </summary>
        /// <param name="kindlist">游戏标识列表</param>
        public int DeleteH5GameKindItem(string kindlist)
        {
            string sql = $"DELETE H5GameKindItem WHERE KindID IN ({kindlist})";
            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 获取游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        public MobileKindItem GetMobileKindItemInfo(int kindid)
        {
            string sql = $"SELECT * FROM MobileKindItem WITH(NOLOCK) WHERE KindID={kindid}";
            return Database.ExecuteObject<MobileKindItem>(sql);
        }

        /// <summary>
        /// 获取U3D游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        public U3DGameKindItem GetU3DGameKindItemInfo(int kindid)
        {
            string sql = $"SELECT * FROM U3DGameKindItem WITH(NOLOCK) WHERE KindID={kindid}";
            return Database.ExecuteObject<U3DGameKindItem>(sql);
        }

        /// <summary>
        /// 获取H5游戏信息
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        public H5GameKindItem GetH5GameKindItemInfo(int kindid)
        {
            string sql = $"SELECT * FROM H5GameKindItem WITH(NOLOCK) WHERE KindID={kindid}";
            return Database.ExecuteObject<H5GameKindItem>(sql);
        }

        /// <summary>
        /// 获取启用游戏列表
        /// </summary>
        /// <returns></returns>
        public IList<MobileKindItem> GetMobileKindItemList()
        {
            const string sql = "SELECT * FROM MobileKindItem WITH(NOLOCK) WHERE Nullity=0 ";
            return Database.ExecuteObjectList<MobileKindItem>(sql);
        }
        /// <summary>
        /// 新增游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int InsertMobileKindItem(MobileKindItem item)
        {
            string sql = @"INSERT INTO MobileKindItem(KindID,KindName,TypeID,ModuleName,ClientVersion,ResVersion,SortID,KindMark,Nullity) 
                        VALUES(@KindID,@KindName,@TypeID,@ModuleName,@ClientVersion,@ResVersion,@SortID,@KindMark,@Nullity)";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("KindID", item.KindID),
                Database.MakeInParam("KindName", item.KindName),
                Database.MakeInParam("TypeID", item.TypeID),
                Database.MakeInParam("ModuleName", item.ModuleName),
                Database.MakeInParam("ClientVersion", item.ClientVersion),
                Database.MakeInParam("ResVersion", item.ResVersion),
                Database.MakeInParam("SortID", item.SortID),
                Database.MakeInParam("KindMark", item.KindMark),
                Database.MakeInParam("Nullity", item.Nullity)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 新增U3D游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int InsertU3DGameKindItem(U3DGameKindItem item)
        {
            string sql = @"INSERT INTO U3DGameKindItem(KindID,KindName,TypeID,ModuleName,ClientVersion,ResVersion,SortID,KindMark,Nullity) 
                        VALUES(@KindID,@KindName,@TypeID,@ModuleName,@ClientVersion,@ResVersion,@SortID,@KindMark,@Nullity)";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("KindID", item.KindID),
                Database.MakeInParam("KindName", item.KindName),
                Database.MakeInParam("TypeID", item.TypeID),
                Database.MakeInParam("ModuleName", item.ModuleName),
                Database.MakeInParam("ClientVersion", item.ClientVersion),
                Database.MakeInParam("ResVersion", item.ResVersion),
                Database.MakeInParam("SortID", item.SortID),
                Database.MakeInParam("KindMark", item.KindMark),
                Database.MakeInParam("Nullity", item.Nullity)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 新增H5游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int InsertH5GameKindItem(H5GameKindItem item)
        {
            string sql = @"INSERT INTO H5GameKindItem(KindID,KindName,TypeID,ModuleName,ClientVersion,ResVersion,SortID,KindMark,Nullity) 
                        VALUES(@KindID,@KindName,@TypeID,@ModuleName,@ClientVersion,@ResVersion,@SortID,@KindMark,@Nullity)";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("KindID", item.KindID),
                Database.MakeInParam("KindName", item.KindName),
                Database.MakeInParam("TypeID", item.TypeID),
                Database.MakeInParam("ModuleName", item.ModuleName),
                Database.MakeInParam("ClientVersion", item.ClientVersion),
                Database.MakeInParam("ResVersion", item.ResVersion),
                Database.MakeInParam("SortID", item.SortID),
                Database.MakeInParam("KindMark", item.KindMark),
                Database.MakeInParam("Nullity", item.Nullity)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 修改游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int UpdateMobileKindItem(MobileKindItem item)
        {
            string sql = @"UPDATE MobileKindItem SET TypeID=@TypeID,ModuleName=@ModuleName,ClientVersion=@ClientVersion,KindName=@KindName,
                            ResVersion=@ResVersion,SortID=@SortID,KindMark=@KindMark,Nullity=@Nullity WHERE KindID=@KindID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("KindID", item.KindID),
                Database.MakeInParam("TypeID", item.TypeID),
                Database.MakeInParam("ModuleName", item.ModuleName),
                Database.MakeInParam("ClientVersion", item.ClientVersion),
                Database.MakeInParam("KindName", item.KindName),
                Database.MakeInParam("ResVersion", item.ResVersion),
                Database.MakeInParam("SortID", item.SortID),
                Database.MakeInParam("KindMark", item.KindMark),
                Database.MakeInParam("Nullity", item.Nullity)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 修改U3D游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int UpdateU3DGameKindItem(U3DGameKindItem item)
        {
            string sql = @"UPDATE U3DGameKindItem SET TypeID=@TypeID,ModuleName=@ModuleName,ClientVersion=@ClientVersion,KindName=@KindName,
                            ResVersion=@ResVersion,SortID=@SortID,KindMark=@KindMark,Nullity=@Nullity WHERE KindID=@KindID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("KindID", item.KindID),
                Database.MakeInParam("TypeID", item.TypeID),
                Database.MakeInParam("ModuleName", item.ModuleName),
                Database.MakeInParam("ClientVersion", item.ClientVersion),
                Database.MakeInParam("KindName", item.KindName),
                Database.MakeInParam("ResVersion", item.ResVersion),
                Database.MakeInParam("SortID", item.SortID),
                Database.MakeInParam("KindMark", item.KindMark),
                Database.MakeInParam("Nullity", item.Nullity)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 修改H5游戏信息
        /// </summary>
        /// <param name="item">游戏信息</param>
        /// <returns></returns>
        public int UpdateH5GameKindItem(H5GameKindItem item)
        {
            string sql = @"UPDATE H5GameKindItem SET TypeID=@TypeID,ModuleName=@ModuleName,ClientVersion=@ClientVersion,KindName=@KindName,
                            ResVersion=@ResVersion,SortID=@SortID,KindMark=@KindMark,Nullity=@Nullity WHERE KindID=@KindID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("KindID", item.KindID),
                Database.MakeInParam("TypeID", item.TypeID),
                Database.MakeInParam("ModuleName", item.ModuleName),
                Database.MakeInParam("ClientVersion", item.ClientVersion),
                Database.MakeInParam("KindName", item.KindName),
                Database.MakeInParam("ResVersion", item.ResVersion),
                Database.MakeInParam("SortID", item.SortID),
                Database.MakeInParam("KindMark", item.KindMark),
                Database.MakeInParam("Nullity", item.Nullity)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 批量修改时游戏可用性
        /// </summary>
        /// <param name="idList">idlist（0,x,x,x）</param>
        /// <param name="nullity">0：启用 1：禁用</param>
        /// <returns></returns>
        public int ChangeMobileKindNullity(string idList, int nullity)
        {
            string sql = $"UPDATE MobileKindItem SET Nullity = {nullity} WHERE KindID IN ({idList}) ";
            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 批量修改U3D游戏可用性
        /// </summary>
        /// <param name="idList">idlist（0,x,x,x）</param>
        /// <param name="nullity">0：启用 1：禁用</param>
        /// <returns></returns>
        public int ChangeU3DGameKindNullity(string idList, int nullity)
        {
            string sql = $"UPDATE U3DGameKindItem SET Nullity = {nullity} WHERE KindID IN ({idList}) ";
            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 批量修改H5游戏可用性
        /// </summary>
        /// <param name="idList">idlist（0,x,x,x）</param>
        /// <param name="nullity">0：启用 1：禁用</param>
        /// <returns></returns>
        public int ChangeH5GameKindNullity(string idList, int nullity)
        {
            string sql = $"UPDATE H5GameKindItem SET Nullity = {nullity} WHERE KindID IN ({idList}) ";
            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 获取游戏列表
        /// </summary>
        /// <returns></returns>
        public IList<GameKindItem> GetGameList()
        {
            string sqlQuery = "SELECT * FROM GameKindItem WHERE Nullity=0";
            return Database.ExecuteObjectList<GameKindItem>(sqlQuery);
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
            string sqlQuery = $"SELECT * FROM DataBaseInfo WITH(NOLOCK) WHERE DBInfoID= {dBInfoId}";
            return Database.ExecuteObject<DataBaseInfo>(sqlQuery);
        }
        /// <summary>
        /// 获取机器信息
        /// </summary>
        /// <param name="dBAddr">机器地址</param>
        /// <returns></returns>
        public DataBaseInfo GetDataBaseInfo(string dBAddr)
        {
            string sqlQuery = "SELECT * FROM DataBaseInfo WITH(NOLOCK) WHERE DBAddr= @DBAddr";

            var prams = new List<DbParameter> { Database.MakeInParam("DBAddr", dBAddr) };

            return Database.ExecuteObject<DataBaseInfo>(sqlQuery, prams);
        }
        /// <summary>
        /// 新增机器信息
        /// </summary>
        /// <param name="dataBase">机器信息</param>
        /// <returns></returns>
        public int InsertDataBase(DataBaseInfo dataBase)
        {
            string sqlQuery = @"INSERT INTO DataBaseInfo(DBAddr,DBPort,DBUser,DBPassword,MachineID,Information) VALUES(@DBAddr,
                    @DBPort,@DBUser,@DBPassword,@MachineID,@Information)";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("DBAddr", dataBase.DBAddr),
                Database.MakeInParam("DBPort", dataBase.DBPort),
                Database.MakeInParam("DBUser", dataBase.DBUser),
                Database.MakeInParam("DBPassword", dataBase.DBPassword),
                Database.MakeInParam("MachineID", dataBase.MachineID),
                Database.MakeInParam("Information", dataBase.Information)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        ///  修改机器信息
        /// </summary>
        /// <param name="dataBase">机器信息</param>
        /// <returns></returns>
        public int UpdateDataBase(DataBaseInfo dataBase)
        {
            string sqlQuery = @"UPDATE DataBaseInfo SET DBAddr=@DBAddr,DBPort=@DBPort,DBUser=@DBUser,
            DBPassword=@DBPassword,MachineID=@MachineID,Information=@Information WHERE DBInfoID=@DBInfoID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("DBAddr", dataBase.DBAddr),
                Database.MakeInParam("DBPort", dataBase.DBPort),
                Database.MakeInParam("DBUser", dataBase.DBUser),
                Database.MakeInParam("DBPassword", dataBase.DBPassword),
                Database.MakeInParam("MachineID", dataBase.MachineID),
                Database.MakeInParam("Information", dataBase.Information),
                Database.MakeInParam("DBInfoID", dataBase.DBInfoID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 删除机器信息
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <returns></returns>
        public int DeleteDataBase(string idlist)
        {
            string sqlQuery = $"DELETE DataBaseInfo WHERE DBInfoID IN({idlist})";
            return Database.ExecuteNonQuery(sqlQuery);
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
            string sqlQuery = $"SELECT * FROM GameRoomInfo WITH(NOLOCK) WHERE ServerID= {serverId}";
            return Database.ExecuteObject<GameRoomInfo>(sqlQuery);
        }
        /// <summary>
        /// 获取游戏和房间集合
        /// </summary>
        /// <returns></returns>
        public DataSet GetKindAndRoomList()
        {
            string sqlQuery = "SELECT KindID,KindName,Nullity FROM GameKindItem(NOLOCK);SELECT KindID,ServerID,ServerName,Nullity FROM GameRoomInfo(NOLOCK)";
            return Database.ExecuteDataset(sqlQuery);
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
            string sqlQuery = $"SELECT * FROM SystemMessage WITH(NOLOCK) WHERE ID= {id}";
            return Database.ExecuteObject<SystemMessage>(sqlQuery);
        }
        /// <summary>
        /// 新增系统消息
        /// </summary>
        /// <param name="systemMessage">系统消息</param>
        /// <returns></returns>
        public int InsertSystemMessage(SystemMessage systemMessage)
        {
            string sqlQuery = @"INSERT INTO SystemMessage(MessageType,ServerRange,MessageString,StartTime,
            ConcludeTime,TimeRate,Nullity,CreateDate,CreateMasterID,UpdateDate,UpdateMasterID,UpdateCount,CollectNote) 
            VALUES(@MessageType,@ServerRange,@MessageString,@StartTime,@ConcludeTime,@TimeRate,@Nullity,@CreateDate,
                @CreateMasterID,@UpdateDate,@UpdateMasterID,@UpdateCount,@CollectNote)";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("MessageType", systemMessage.MessageType),
                Database.MakeInParam("ServerRange", systemMessage.ServerRange),
                Database.MakeInParam("MessageString", systemMessage.MessageString),
                Database.MakeInParam("StartTime", systemMessage.StartTime),
                Database.MakeInParam("ConcludeTime", systemMessage.ConcludeTime),
                Database.MakeInParam("TimeRate", systemMessage.TimeRate),
                Database.MakeInParam("Nullity", systemMessage.Nullity),
                Database.MakeInParam("CreateDate", systemMessage.CreateDate),
                Database.MakeInParam("CreateMasterID", systemMessage.CreateMasterID),
                Database.MakeInParam("UpdateDate", systemMessage.UpdateDate),
                Database.MakeInParam("UpdateMasterID", systemMessage.UpdateMasterID),
                Database.MakeInParam("UpdateCount", systemMessage.UpdateCount),
                Database.MakeInParam("CollectNote", systemMessage.CollectNote)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 修改系统消息
        /// </summary>
        /// <param name="systemMessage">系统消息</param>
        /// <returns></returns>
        public int UpdateSystemMessage(SystemMessage systemMessage)
        {
            string sqlQuery = @"UPDATE SystemMessage SET MessageType=@MessageType,ServerRange=@ServerRange,
                    MessageString=@MessageString,StartTime=@StartTime,ConcludeTime=@ConcludeTime,TimeRate=@TimeRate,
                    Nullity=@Nullity,UpdateDate=@UpdateDate,UpdateMasterID=@UpdateMasterID,UpdateCount=@UpdateCount,
                    CollectNote=@CollectNote WHERE ID=@ID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("MessageType", systemMessage.MessageType),
                Database.MakeInParam("ServerRange", systemMessage.ServerRange),
                Database.MakeInParam("MessageString", systemMessage.MessageString),
                Database.MakeInParam("StartTime", systemMessage.StartTime),
                Database.MakeInParam("ConcludeTime", systemMessage.ConcludeTime),
                Database.MakeInParam("TimeRate", systemMessage.TimeRate),
                Database.MakeInParam("Nullity", systemMessage.Nullity),
                Database.MakeInParam("UpdateDate", systemMessage.UpdateDate),
                Database.MakeInParam("UpdateMasterID", systemMessage.UpdateMasterID),
                Database.MakeInParam("UpdateCount", systemMessage.UpdateCount),
                Database.MakeInParam("CollectNote", systemMessage.CollectNote),
                Database.MakeInParam("ID", systemMessage.ID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 删除系统消息
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public int DeleteSystemMessage(string where)
        {
            string sqlQuery = $"DELETE SystemMessage {where}";
            return Database.ExecuteNonQuery(sqlQuery);
        }
        /// <summary>
        /// 冻结解冻系统消息
        /// </summary>
        /// <param name="idList">标识字符集</param>
        /// <param name="nullity">状态值</param>
        /// <returns></returns>
        public int NullitySystemMessage(string idList, int nullity)
        {
            string sql = $"UPDATE SystemMessage SET Nullity ={nullity} WHERE ID IN({idList})";
            return Database.ExecuteNonQuery(sql);
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
            PagerParameters pagerPrams = new PagerParameters("StreamCreateTableFeeInfo", where, order, pageIndex, pageSize, null, "WEB_PageView_GoldBattle");
            return GetPagerSet(pagerPrams);
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
            PagerParameters pagerPrams = new PagerParameters("StreamCreateTableFeeInfo", where, order, pageIndex, pageSize, null, "WEB_PageView_ScoreBattle");
            return GetPagerSet(pagerPrams);
        }
        /// <summary>
        /// 获取约战房间信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        public DataSet GetCreateRoomInfo(int userid)
        {
            string sql =
                $"SELECT COUNT(UserID) AS UCont,SUM(CreateTableFee) AS CTotal FROM StreamCreateTableFeeInfo WITH(NOLOCK) WHERE PayMode=0 AND UserID={userid}";
            return Database.ExecuteDataset(CommandType.Text, sql);
        }
        /// <summary>
        /// 获取约战房间开房前50名
        /// </summary>
        /// <returns></returns>
        public DataSet GetCreateRoomRank()
        {
            string sql = "SELECT TOP 50 UserID,COUNT(UserID) AS UCont FROM StreamCreateTableFeeInfo WITH(NOLOCK) WHERE PayMode=0 AND RoomScoreInfo IS NOT NULL GROUP BY UserID ORDER BY COUNT(UserID) DESC";
            return Database.ExecuteDataset(CommandType.Text, sql);
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
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("dwRoomID", roomid),
                Database.MakeInParam("dwSTime", stime),
                Database.MakeInParam("dwETime", etime),
                Database.MakeInParam("dwUserID", hostid),
                Database.MakeInParam("dwModeType", playmode)
            };
            return Database.ExecuteDataset(CommandType.StoredProcedure, "NET_PM_QueryBackBattleInfo", prams.ToArray());
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
            string sqlQuery =
                $"SELECT ISNULL(SUM(CreateTableFee),0) AS Diamond FROM StreamCreateTableFeeInfo WITH(NOLOCK) {where}";
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery);
            return obj != null ? Convert.ToInt64(obj) : 0;
        }
        /// <summary>
        /// 获取创建房间数量
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public long GetTotalCreateRoomTable(string where)
        {
            string sqlQuery = $"SELECT COUNT(RecordID) AS [Table] FROM StreamCreateTableFeeInfo WITH(NOLOCK) {where}";
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery);
            return obj != null ? Convert.ToInt64(obj) : 0;
        }
        /// <summary>
        /// 获取AA创建房间钻石统计
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public long GetAaTotalCreateRoomDiamond(string where)
        {
            string sqlQuery =
                $"SELECT ISNULL(SUM(Diamond),0) FROM WHQJRecordDB.dbo.RecordGameDiamond WITH(NOLOCK) WHERE RoomID IN(SELECT RoomID FROM StreamCreateTableFeeInfo WITH(NOLOCK) {where})";
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery);
            return obj != null ? Convert.ToInt64(obj) : 0;
        }
        /// <summary>
        /// 获取在线人数统计
        /// </summary>
        /// <param name="sTime">起始时间</param>
        /// <param name="eTime">结束时间</param>
        /// <returns></returns>
        public IList<StatisticsOnline> GetUserOnlineStatistics(string sTime, string eTime)
        {
            string sqlQuery = @"SELECT InsertDateTime AS DTime,OnLineCountSum AS RUser,AndroidCountSum AS AUser FROM OnLineStreamInfo WITH(NOLOCK) WHERE InsertDateTime>=@sTime AND InsertDateTime<=@eTime ORDER BY InsertDateTime ASC";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("sTime", sTime),
                Database.MakeInParam("eTime", eTime)
            };

            return Database.ExecuteObjectList<StatisticsOnline>(sqlQuery, prams);
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
            return Database.ExecuteObject<GameProperty>($"SELECT * FROM GameProperty(NOLOCK) WHERE ID = {id} AND ExchangeType={typeid}");
        }

        /// <summary>
        /// 修改道具信息
        /// </summary>
        /// <param name="property">道具信息</param>
        /// <returns></returns>
        public int UpdatePropertyInfo(GameProperty property)
        {
            string sqlQuery = "UPDATE GameProperty SET ExchangeRatio=@ExchangeRatio,Name=@Name,BuyResultsGold=@BuyResultsGold,UseResultsGold=@UseResultsGold,RegulationsInfo=@RegulationsInfo,SortID=@SortID,Nullity=@Nullity WHERE ID=@ID AND ExchangeType=@ExchangeType";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("ExchangeType", property.ExchangeType),
                Database.MakeInParam("ExchangeRatio", property.ExchangeRatio),
                Database.MakeInParam("Name", property.Name),
                Database.MakeInParam("BuyResultsGold", property.BuyResultsGold),
                Database.MakeInParam("UseResultsGold", property.UseResultsGold),
                Database.MakeInParam("RegulationsInfo", property.RegulationsInfo),
                Database.MakeInParam("SortID", property.SortID),
                Database.MakeInParam("Nullity", property.Nullity),
                Database.MakeInParam("ID", property.ID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        #endregion

        #region 签到管理


        /// <summary>
        /// 获取签到礼包配置列表
        /// </summary>

        /// <returns></returns>
        public DataSet GetGamePackageList()
        {
            string sqlQuery = $"SELECT * FROM GamePackage WITH(NOLOCK) ";
            return Database.ExecuteDataset(sqlQuery);
        }
        /// <summary>
        /// 获取签到礼包配置
        /// </summary>
        /// <param name="PackageID">礼包标识id</param>
        /// <returns></returns>
        public GamePackage GetGamePackage(int PackageID)
        {
            string sqlQuery = $"SELECT * FROM GamePackage WITH(NOLOCK) WHERE PackageID = {PackageID}";
            return Database.ExecuteObject<GamePackage>(sqlQuery);
        }
        /// <summary>
        /// 新增签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int InsertGamePackage(GamePackage config)
        {
            string sqlQuery =
                @"INSERT INTO GamePackage(Name,TypeID,SortID,Nullity,Describe,PlatformKind,CollectDate) 
                            VALUES(@Name,@TypeID,@SortID,@Nullity,@Describe,@PlatformKind,getdate())";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("Name", config.Name),
                Database.MakeInParam("TypeID", config.TypeID),
                Database.MakeInParam("SortID", config.SortID),
                Database.MakeInParam("Nullity", config.Nullity),
                Database.MakeInParam("Describe", config.Describe),
                Database.MakeInParam("PlatformKind", config.PlatformKind)

            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 修改签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int UpdateGamePackage(GamePackage config)
        {
            StringBuilder sqlQuery = new StringBuilder();
            sqlQuery.Append("UPDATE GamePackage SET ")
                .Append("Name=@Name, ")
                .Append("TypeID=@TypeID, ")
                .Append("SortID=@SortID, ")
                .Append("Nullity=@Nullity, ")
                .Append("Describe=@Describe, ")
                .Append("PlatformKind=@PlatformKind ")
                .Append("WHERE PackageID=@PackageID");

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("Name", config.Name),
                Database.MakeInParam("TypeID", config.TypeID),
                Database.MakeInParam("SortID", config.SortID),
                Database.MakeInParam("Nullity", config.Nullity),
                Database.MakeInParam("Describe", config.Describe),
                Database.MakeInParam("PlatformKind", config.PlatformKind),
                Database.MakeInParam("PackageID", config.PackageID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery.ToString(), prams.ToArray());
        }


        /// <summary>
        /// 删除签到礼包配置
        /// </summary>
        /// <param name="PackageID">礼包标识id</param>
        /// <returns></returns>
        public int DeleteGamePackage(string PackageID)
        {
            string sqlQuery = $"DELETE GamePackage WHERE PackageID in({PackageID})";
            return Database.ExecuteNonQuery(sqlQuery);
        }








        /// <summary>
        /// 获取签到礼包物品配置
        /// </summary>
        /// <param name="GoodsID">物品标识id</param>
        /// <returns></returns>
        public GamePackageGoods GetGamePackageGoods(int GoodsID)
        {
            string sqlQuery = $"SELECT * FROM GamePackageGoods WITH(NOLOCK) WHERE GoodsID = {GoodsID}";
            return Database.ExecuteObject<GamePackageGoods>(sqlQuery);
        }
        /// <summary>
        /// 新增签到礼包物品配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int InsertGamePackageGoods(GamePackageGoods config)
        {
            string sqlQuery =
                @"INSERT INTO GamePackageGoods(PackageID,TypeID,PropertyID,GoodsNum,ResourceURL,CollectDate) 
                            VALUES(@PackageID,@TypeID,@PropertyID,@GoodsNum,@ResourceURL,getdate())";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("PackageID", config.PackageID),
                Database.MakeInParam("TypeID", config.TypeID),
                Database.MakeInParam("PropertyID", config.PropertyID),
                Database.MakeInParam("GoodsNum", config.GoodsNum),
                Database.MakeInParam("ResourceURL", config.ResourceURL)

            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 修改签到礼包配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int UpdateGamePackageGoods(GamePackageGoods config)
        {
            StringBuilder sqlQuery = new StringBuilder();
            sqlQuery.Append("UPDATE GamePackageGoods SET ")
                .Append("PackageID=@PackageID, ")
                .Append("TypeID=@TypeID, ")
                .Append("PropertyID=@PropertyID, ")
                .Append("GoodsNum=@GoodsNum, ")
                .Append("ResourceURL=@ResourceURL ")
                .Append("WHERE GoodsID=@GoodsID");

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("PackageID", config.PackageID),
                Database.MakeInParam("TypeID", config.TypeID),
                Database.MakeInParam("PropertyID", config.PropertyID),
                Database.MakeInParam("GoodsNum", config.GoodsNum),
                Database.MakeInParam("ResourceURL", config.ResourceURL),
                Database.MakeInParam("GoodsID", config.GoodsID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery.ToString(), prams.ToArray());
        }
        /// <summary>
        /// 删除签到礼包物品配置
        /// </summary>
        /// <param name="GoodsID">物品标识id</param>
        /// <returns></returns>
        public int DeleteGamePackageGoods(string GoodsID)
        {
            string sqlQuery = $"DELETE GamePackageGoods WHERE GoodsID in( {GoodsID})";
            return Database.ExecuteNonQuery(sqlQuery);
        }









        /// <summary>
        /// 获取签到配置
        /// </summary>
        /// <param name="SignID">签到配置id</param>
        /// <returns></returns>
        public GameSignIn GetGameSignIn(int SignID)
        {
            string sqlQuery = $"SELECT * FROM GameSignIn WITH(NOLOCK) WHERE SignID = {SignID}";
            return Database.ExecuteObject<GameSignIn>(sqlQuery);
        }
        /// <summary>
        /// 新增签到配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int InsertGameSignIn(GameSignIn config)
        {
            string sqlQuery =
                @"INSERT INTO GameSignIn(PackageID,TypeID,Probability,NeedDay,SortID,Nullity,CollectDate) 
                            VALUES(@PackageID,@TypeID,@Probability,@NeedDay,@SortID,@Nullity,getdate())";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("PackageID", config.PackageID),
                Database.MakeInParam("TypeID", config.TypeID),
                Database.MakeInParam("Probability", config.Probability),
                Database.MakeInParam("NeedDay", config.NeedDay),
                Database.MakeInParam("SortID", config.SortID),
                Database.MakeInParam("Nullity", config.Nullity),


            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 修改签到配置
        /// </summary>
        /// <param name="config"></param>
        /// <returns></returns>
        public int UpdateGameSignIn(GameSignIn config)
        {
            StringBuilder sqlQuery = new StringBuilder();
            sqlQuery.Append("UPDATE GameSignIn SET ")
                .Append("PackageID=@PackageID, ")
                .Append("TypeID=@TypeID, ")
                .Append("Probability=@Probability, ")
                .Append("NeedDay=@NeedDay, ")
                .Append("SortID=@SortID, ")
                .Append("Nullity=@Nullity ")
                .Append("WHERE SignID=@SignID");

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("PackageID", config.PackageID),
                Database.MakeInParam("TypeID", config.TypeID),
                Database.MakeInParam("Probability", config.Probability),
                Database.MakeInParam("NeedDay", config.NeedDay),
                Database.MakeInParam("SortID", config.SortID),
                Database.MakeInParam("Nullity", config.Nullity),
                Database.MakeInParam("SignID", config.SignID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery.ToString(), prams.ToArray());
        }
        /// <summary>
        /// 删除签到配置
        /// </summary>
        /// <param name="SignID">签到配置id</param>
        /// <returns></returns>
        public int DeleteGameSignIn(string SignID)
        {
            string sqlQuery = $"DELETE GameSignIn WHERE SignID in ({SignID})";
            return Database.ExecuteNonQuery(sqlQuery);
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
            string sqlQuery = $"SELECT * FROM TaskInfo WITH(NOLOCK) WHERE TaskID= {taskid}";
            return Database.ExecuteObject<TaskInfo>(sqlQuery);
        }
        /// <summary>
        /// 新增任务
        /// </summary>
        /// <param name="info">任务信息</param>
        /// <returns></returns>
        public int InsertTaskInfo(TaskInfo info)
        {
            string sqlQuery = @"INSERT INTO TaskInfo(TaskName,TaskDescription,TaskType,UserType,TimeType,KindID,ServerID,MatchID,TaskAward,Innings,Nullity) 
                        VALUES(@TaskName,@TaskDescription,@TaskType,@UserType, @TimeType,@KindID,@ServerID,@MatchID,@TaskAward,@Innings,@Nullity)";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("TaskName", info.TaskName),
                Database.MakeInParam("TaskDescription", info.TaskDescription),
                Database.MakeInParam("TaskType", info.TaskType),
                Database.MakeInParam("UserType", info.UserType),
                Database.MakeInParam("TimeType", info.TimeType),
                Database.MakeInParam("KindID", info.KindID),
                Database.MakeInParam("ServerID", info.ServerID),
                Database.MakeInParam("MatchID", info.MatchID),
                Database.MakeInParam("TaskAward", info.TaskAward),
                Database.MakeInParam("Innings", info.Innings),
                Database.MakeInParam("Nullity", info.Nullity)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 修改任务
        /// </summary>
        /// <param name="info">任务信息</param>
        /// <returns></returns>
        public int UpdateTaskInfo(TaskInfo info)
        {
            string sql = @"UPDATE TaskInfo SET TaskName=@TaskName,TaskDescription=@TaskDescription,TaskType=@TaskType,UserType=@UserType,
                TimeType=@TimeType,KindID=@KindID,ServerID=@ServerID,MatchID=@MatchID,Innings=@Innings,TaskAward=@TaskAward,Nullity=@Nullity WHERE TaskID=@TaskID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("TaskID", info.TaskID),
                Database.MakeInParam("TaskName", info.TaskName),
                Database.MakeInParam("TaskDescription", info.TaskDescription),
                Database.MakeInParam("TaskType", info.TaskType),
                Database.MakeInParam("UserType", info.UserType),
                Database.MakeInParam("TimeType", info.TimeType),
                Database.MakeInParam("KindID", info.KindID),
                Database.MakeInParam("ServerID", info.ServerID),
                Database.MakeInParam("MatchID", info.MatchID),
                Database.MakeInParam("TaskAward", info.TaskAward),
                Database.MakeInParam("Innings", info.Innings),
                Database.MakeInParam("Nullity", info.Nullity)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
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
            string sqlQuery = "INSERT INTO dbo.SensitiveWords( ForbidWords ) VALUES(@word)";
            var prams = new List<DbParameter> { Database.MakeInParam("word", word) };
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }


        /// <summary>
        /// 是否存在相同敏感字符
        /// </summary>
        /// <param name="word"></param>
        /// <returns></returns>
        public bool IsExsitSensitiveWords(string word)
        {
            string sqlQuery = "SELECT COUNT(*) FROM dbo.SensitiveWords WHERE ForbidWords=@word";
            var prams = new List<DbParameter> { Database.MakeInParam("word", word) };
            return (int)Database.ExecuteScalar(CommandType.Text, sqlQuery, prams.ToArray()) > 0;
        }

        #endregion

        #region 调试系统

        #region AI人数调整
        /// <summary>
        /// 获取AI人数调整实体 by aiTime
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        public AITimeChange GetAITimeChangeInfo(int aiTime)
        {
            return Database.ExecuteObject<AITimeChange>($"SELECT * FROM DBO.[AITimeChange] WHERE AITime={aiTime}");
        }

        /// <summary>
        /// 保存AI人数调整
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int SaveAITimeChangeInfo(AITimeChange model)
        {
            var parm = new List<DbParameter>()
                {
                    Database.MakeInParam("AICount", model.AICount),
                    Database.MakeInParam("AITime", model.AITime)
                };

            const string sqlUpdate =
                " UPDATE DBO.[AITimeChange] SET AICount=@AICount,AITime=@AITime WHERE AITime=@AITime ";

            return Database.ExecuteNonQuery(CommandType.Text, sqlUpdate, parm.ToArray());
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
            return Database.ExecuteObject<AIWeightInRatio>($"SELECT * FROM DBO.[AIWeightInRatio] WHERE ID={Id}");
        }

        /// <summary>
        /// 保存AI进入房间权重配置
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int SaveAIWeightInRatioInfo(AIWeightInRatio model)
        {
            var parm = new List<DbParameter>()
            {
                Database.MakeInParam("TimeCount", model.TimeCount),
                Database.MakeInParam("WeightIn", model.WeightIn),
                Database.MakeInParam("ID", model.ID),
            };

            const string sqlUpdate =
                " UPDATE DBO.[AIWeightInRatio] SET WeightIn=@WeightIn,TimeCount=@TimeCount WHERE ID=@ID ";

            return Database.ExecuteNonQuery(CommandType.Text, sqlUpdate, parm.ToArray());
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
            return Database.ExecuteObject<AIWeightOutRatio>($"SELECT * FROM DBO.[AIWeightOutRatio] WHERE ID={Id}");
        }

        /// <summary>
        /// 保存AI离开俱乐部权重配置
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int SaveAIWeightOutRatioInfo(AIWeightOutRatio model)
        {
            var parm = new List<DbParameter>()
            {
                Database.MakeInParam("TimeCount", model.TimeCount),
                Database.MakeInParam("WeightOut", model.WeightOut),
                Database.MakeInParam("ID", model.ID),
            };

            const string sqlUpdate =
                " UPDATE DBO.[AIWeightOutRatio] SET WeightOut=@WeightOut,TimeCount=@TimeCount WHERE ID=@ID ";

            return Database.ExecuteNonQuery(CommandType.Text, sqlUpdate, parm.ToArray());
        }

        /// <summary>
        /// 获取当前时间的下线权重
        /// </summary>
        /// <param name="TimeCount"></param>
        /// <returns></returns>
        public int GetAiWeightOutRatioByTime(double TimeCount)
        {
            string sqlQuery =
                $@"DECLARE @AIWeightOut INT
			        DECLARE @MaxTimeCount INT=0
			        SELECT TOP 1  @MaxTimeCount=TimeCount FROM dbo.AIWeightOutRatio ORDER BY timeCount DESC
			        IF @MaxTimeCount<={TimeCount}
			        BEGIN
				        SELECT TOP 1  @AIWeightOut=WeightOut FROM dbo.AIWeightOutRatio ORDER BY timeCount DESC
			        END
			        ELSE
			        BEGIN
				        SELECT TOP 1 @AIWeightOut=WeightOut FROM WHQJPlatformDB.dbo.AIWeightOutRatio WHERE timeCount>={TimeCount} 
			        END
                    SELECT @AIWeightOut";
            return Convert.ToInt32(Database.ExecuteScalar(CommandType.Text, sqlQuery));
        }
        #endregion


        /// <summary>
        /// 获取当前在线玩家房间号
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="userId">玩家ID</param>
        /// <returns></returns>
        public int GetOnlineUeserRoomID(int groupId,int userId)
        {
            string sqlQuery =
                $"SELECT TOP 1  RoomID FROM WHQJPlatformDB.dbo.PersonalRoomScoreInfo WHERE GroupID={groupId} AND UserID={userId} ORDER BY WriteTime DESC";
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery);
            return obj != null ? Convert.ToInt32(obj) : 0;
        }

        /// <summary>
        /// 获取支持联盟的游戏列表
        /// </summary>
        /// <returns></returns>
        public DataSet GetGroupKindList()
        {
            string sqlQuery = @"SELECT MAX(ggi.GameName)AS KindName, MAX(pri.KindID) AS KindID FROM dbo.PersonalRoomInfo pri INNER JOIN dbo.GameGameItem ggi ON ggi.GameID = pri.KindID GROUP BY pri.KindID";
            return Database.ExecuteDataset(sqlQuery);
        }

        #endregion


    }
}
