using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using Game.Entity.NativeWeb;
using Game.IData;
using Game.Kernel;
using System;
using System.Text;

namespace Game.Data
{
    /// <summary>
    /// 网站数据层
    /// </summary>
    public class NativeWebDataProvider : BaseDataProvider, INativeWebDataProvider
    {
        #region 构造方法

        /// <summary>
        /// 构造函数
        /// </summary>
        public NativeWebDataProvider(string connString)
            : base(connString)
        {
        }

        #endregion 构造方法

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
            PagerParameters pagerPrams = new PagerParameters(tableName, where, order, pageIndex, pageSize, fields);
            return GetPagerSet(pagerPrams);
        }
        #endregion 公用分页

        #region 站点配置

        /// <summary>
        /// 获取站点配置
        /// </summary>
        /// <param name="id">配置标识</param>
        /// <returns></returns>
        public ConfigInfo GetConfigInfo(int id)
        {
            string sql = $"SELECT * FROM ConfigInfo WITH(NOLOCK) WHERE ConfigID={id}";
            return Database.ExecuteObject<ConfigInfo>(sql);
        }

        /// <summary>
        /// 获取站点配置
        /// </summary>
        /// <param name="configKey">配置Key</param>
        /// <returns></returns>
        public ConfigInfo GetConfigInfo(string configKey)
        {
            string sql = "SELECT * FROM ConfigInfo WITH(NOLOCK) WHERE ConfigKey=@ConfigKey";
            var prams = new List<DbParameter> {Database.MakeInParam("ConfigKey", configKey)};
            return Database.ExecuteObject<ConfigInfo>(sql, prams);
        }

        /// <summary>
        /// 修改站点配置
        /// </summary>
        /// <param name="config">配置信息</param>
        public int UpdateConfigInfo(ConfigInfo config)
        {
            string sqlQuery =
                @"UPDATE ConfigInfo SET Field1=@Field1,Field2=@Field2,Field3=@Field3,Field4=@Field4,Field5=@Field5,
                                Field6=@Field6,Field7=@Field7,Field8=@Field8,Field9=@Field9,Field10=@Field10,Field11=@Field11,Field12=@Field12,Field13=@Field13,ConfigString=@ConfigString WHERE ConfigID=@ConfigID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("Field1", config.Field1),
                Database.MakeInParam("Field2", config.Field2),
                Database.MakeInParam("Field3", config.Field3),
                Database.MakeInParam("Field4", config.Field4),
                Database.MakeInParam("Field5", config.Field5),
                Database.MakeInParam("Field6", config.Field6),
                Database.MakeInParam("Field7", config.Field7),
                Database.MakeInParam("Field8", config.Field8),
                Database.MakeInParam("Field9", config.Field9),
                Database.MakeInParam("Field10", config.Field10),
                Database.MakeInParam("Field11", config.Field11),
                Database.MakeInParam("Field12", config.Field12),
                Database.MakeInParam("Field13", config.Field13),
                Database.MakeInParam("ConfigString", config.ConfigString),
                Database.MakeInParam("ConfigID", config.ConfigID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        #endregion 站点配置

        #region 广告管理

        /// <summary>
        /// 获取广告实体
        /// </summary>
        /// <param name="id">广告标识</param>
        /// <returns>广告实体</returns>
        public Ads GetAds(int id)
        {
            string sqlQuery = $"SELECT * FROM Ads WITH(NOLOCK) WHERE ID={id}";
            return Database.ExecuteObject<Ads>(sqlQuery);
        }

        /// <summary>
        /// 删除广告
        /// </summary>
        /// <param name="idlist">标识列表</param>
        public int DeleteAds(string idlist)
        {
            string sqlQuery = $"DELETE Ads WHERE ID IN({idlist})";
            return Database.ExecuteNonQuery(sqlQuery);
        }

        /// <summary>
        /// 新增广告
        /// </summary>
        /// <param name="ads">广告信息</param>
        public int InsertAds(Ads ads)
        {
            string sqlQuery =
                @"INSERT Ads(Title,ResourceURL,LinkURL,Type,SortID,Remark,PlatformType) VALUES(@Title,@ResourceURL,@LinkURL,@Type,@SortID,@Remark,@PlatformType)";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("Title", ads.Title),
                Database.MakeInParam("ResourceURL", ads.ResourceURL),
                Database.MakeInParam("LinkURL", ads.LinkURL),
                Database.MakeInParam("Type", ads.Type),
                Database.MakeInParam("SortID", ads.SortID),
                Database.MakeInParam("Remark", ads.Remark),
                 Database.MakeInParam("PlatformType", ads.PlatformType)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 修改广告
        /// </summary>
        /// <param name="ads">广告实体</param>
        public int UpdateAds(Ads ads)
        {
            string sqlQuery = @"UPDATE Ads SET Title=@Title,ResourceURL=@ResourceURL,LinkUrl= @LinkUrl,
                        Type=@Type,SortID=@SortID,Remark=@Remark,PlatformType=@PlatformType WHERE ID= @ID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("ID", ads.ID),
                Database.MakeInParam("Title", ads.Title),
                Database.MakeInParam("ResourceURL", ads.ResourceURL),
                Database.MakeInParam("LinkURL", ads.LinkURL),
                Database.MakeInParam("Type", ads.Type),
                Database.MakeInParam("SortID", ads.SortID),
                Database.MakeInParam("Remark", ads.Remark),
                 Database.MakeInParam("PlatformType", ads.PlatformType)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        #endregion 广告管理

        #region 新闻公告

        /// <summary>
        /// 获取新闻公告信息
        /// </summary>
        /// <param name="id">公告标识</param>
        /// <returns></returns>
        public SystemNotice GetSystemNoticeInfo(int id)
        {
            string sql = $"SELECT * FROM SystemNotice WITH(NOLOCK) WHERE NoticeID = {id}";
            return Database.ExecuteObject<SystemNotice>(sql);
        }

        /// <summary>
        /// 新增新闻公告
        /// </summary>
        /// <param name="notice">公告信息</param>
        /// <returns></returns>
        public int InsertSystemNotice(SystemNotice notice)
        {
            string sql =
                @"INSERT INTO SystemNotice(NoticeTitle,MoblieContent,WebContent,SortID,Publisher,PublisherTime,IsHot,IsTop,Nullity,PlatformType)
                            VALUES(@NoticeTitle,@MoblieContent,@WebContent,@SortID,@Publisher,@PublisherTime,@IsHot,@IsTop,@Nullity,@PlatformType)";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("NoticeTitle", notice.NoticeTitle),
                Database.MakeInParam("MoblieContent", notice.MoblieContent),
                Database.MakeInParam("WebContent", notice.WebContent),
                Database.MakeInParam("SortID", notice.SortID),
                Database.MakeInParam("Publisher", notice.Publisher),
                Database.MakeInParam("PublisherTime", notice.PublisherTime),
                Database.MakeInParam("IsHot", notice.IsHot),
                Database.MakeInParam("IsTop", notice.IsTop),
                Database.MakeInParam("Nullity", notice.Nullity),
                Database.MakeInParam("PlatformType", notice.PlatformType)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 修改公告信息
        /// </summary>
        /// <param name="notice">公告信息</param>
        /// <returns></returns>
        public int UpdateSystemNotice(SystemNotice notice)
        {
            string sqlQuery =
                @"UPDATE SystemNotice SET NoticeTitle=@NoticeTitle,MoblieContent=@MoblieContent,WebContent=@WebContent,SortID= @SortID,
                    Publisher= @Publisher,PublisherTime= @PublisherTime,IsHot=@IsHot,IsTop=@IsTop,Nullity=@Nullity,PlatformType=@PlatformType WHERE NoticeID= @NoticeID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("NoticeTitle", notice.NoticeTitle),
                Database.MakeInParam("MoblieContent", notice.MoblieContent),
                Database.MakeInParam("WebContent", notice.WebContent),
                Database.MakeInParam("SortID", notice.SortID),
                Database.MakeInParam("Publisher", notice.Publisher),
                Database.MakeInParam("PublisherTime", notice.PublisherTime),
                Database.MakeInParam("IsHot", notice.IsHot),
                Database.MakeInParam("IsTop", notice.IsTop),
                Database.MakeInParam("Nullity", notice.Nullity),
                Database.MakeInParam("NoticeID", notice.NoticeID),
                 Database.MakeInParam("PlatformType", notice.PlatformType)
            };
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 删除公告
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <returns></returns>
        public int DeleteSystemNotice(string idlist)
        {
            string sqlQuery = $"DELETE SystemNotice WHERE NoticeID IN({idlist})";
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery);
        }

        #endregion

        #region 排行榜
        /// <summary>
        /// 获取排行榜奖励配置
        /// </summary>
        /// <returns></returns>
        public DataSet GetRankingConfigList()
        {
            string sql = $"SELECT * FROM RankingConfig WITH(NOLOCK) ORDER BY TypeID ASC,RankType ASC";
            return Database.ExecuteDataset(sql);
        }

        /// <summary>
        /// 根据标识获取排行榜配置
        /// </summary>
        /// <param name="configid">配置标识</param>
        /// <returns></returns>
        public RankingConfig GetRankingConfigById(int configid)
        {
            string sql = $"SELECT * FROM RankingConfig WITH(NOLOCK) WHERE ConfigID ={configid}";
            return Database.ExecuteObject<RankingConfig>(sql);
        }

        /// <summary>
        /// 判断排行榜配置是否存在
        /// </summary>
        /// <param name="typeid">排行榜类型</param>
        /// <param name="ranktype">排行榜排名</param>
        /// <returns></returns>
        public bool ExistRankingConfig(int typeid, int ranktype)
        {
            string sql = $"SELECT ConfigID FROM RankingConfig WITH(NOLOCK) WHERE TypeID ={typeid} AND RankType ={ranktype}";
            object obj = Database.ExecuteScalar(CommandType.Text, sql);
            return obj != null;
        }

        /// <summary>
        /// 新增排行榜配置
        /// </summary>
        /// <param name="config">配置信息</param>
        /// <returns></returns>
        public int InsertRankingConfig(RankingConfig config)
        {
            string sql = @"INSERT INTO RankingConfig(TypeID,RankType,Gold,Diamond,AwardTicket,CollectDate)
                            VALUES(@TypeID,@RankType,@Gold,@Diamond,@AwardTicket,@CollectDate)";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("TypeID", config.TypeID),
                Database.MakeInParam("RankType", config.RankType),
                Database.MakeInParam("Gold", config.Gold),
                Database.MakeInParam("Diamond", config.Diamond),
                Database.MakeInParam("AwardTicket", config.AwardTicket),
                Database.MakeInParam("CollectDate", config.CollectDate)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 修改排行榜配置
        /// </summary>
        /// <param name="config">配置信息</param>
        /// <returns></returns>
        public int UpdateRankingConfig(RankingConfig config)
        {
            string sql = @"UPDATE RankingConfig SET TypeID=@TypeID,RankType=@RankType,Gold=@Gold,
                    Diamond=@Diamond,AwardTicket=@AwardTicket,CollectDate=@CollectDate WHERE ConfigID=@ConfigID";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("TypeID", config.TypeID),
                Database.MakeInParam("RankType", config.RankType),
                Database.MakeInParam("Gold", config.Gold),
                Database.MakeInParam("Diamond", config.Diamond),
                  Database.MakeInParam("AwardTicket", config.AwardTicket),
                Database.MakeInParam("CollectDate", config.CollectDate),
                Database.MakeInParam("ConfigID", config.ConfigID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 删除奖励配置
        /// </summary>
        /// <param name="configid">配置标识</param>
        /// <returns></returns>
        public int DeleteRankingConfig(int configid)
        {
            string sql = $"DELETE RankingConfig WHERE ConfigID={configid}";
            return Database.ExecuteNonQuery(sql);
        }
        /// <summary>
        /// 获取排行榜奖励列表
        /// </summary>
        /// <param name="typeid">1、财富排行 2、胜局排行</param>
        /// <param name="userid">用户标识</param>
        /// <param name="stime">发奖起始时间</param>
        /// <param name="etime">发奖结束时间</param>
        /// <returns></returns>
        public DataSet GetRankAwardList(int typeid, int userid, int sdate, int edate)
        {
            string sqlQuery = "";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("SDateID", sdate),
                Database.MakeInParam("EDateID", edate),
                Database.MakeInParam("TypeID", typeid)
            };
            if (userid > 0)
            {
                sqlQuery = "SELECT * FROM CacheRankAward(NOLOCK) WHERE DateID>=@SDateID AND DateID<=@EDateID AND UserID=@UserID AND TypeID=@TypeID ORDER BY RankNum ASC";
                prams.Add(Database.MakeInParam("UserID", userid));
            }
            else
            {
                sqlQuery = "SELECT * FROM CacheRankAward(NOLOCK) WHERE DateID>=@SDateID AND DateID<=@EDateID AND TypeID=@TypeID ORDER BY RankNum ASC";
            }
            return Database.ExecuteDataset(CommandType.Text, sqlQuery, prams.ToArray());
        }
        #endregion

        #region 游戏规则

        /// <summary>
        /// 获取游戏规则
        /// </summary>
        /// <param name="ID">ID</param>
        /// <returns></returns>
        public GameRule GetGameRuleInfo(int ID)
        {
            string sql = $"SELECT * FROM GameRule WITH(NOLOCK) WHERE ID = {ID} ORDER BY SortID ASC,KindID DESC";
            return Database.ExecuteObject<GameRule>(sql);
        }
        /// <summary>
        /// 获取游戏规则
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        public GameRule GetGameRuleInfoByKindid(int KindID,int Type)
        {
            string sql = $"SELECT * FROM GameRule WITH(NOLOCK) WHERE KindID = {KindID} and Type={Type} ORDER BY SortID ASC,KindID DESC";
            return Database.ExecuteObject<GameRule>(sql);
        }

        /// <summary>
        /// 新增游戏规则
        /// </summary>
        /// <param name="rule">游戏规则</param>
        /// <returns></returns>
        public int InsertGameRule(GameRule rule)
        {
            string sql =
                @"INSERT INTO GameRule(KindID,KindName,KindIcon,RuleImg,Type,Nullity,CollectDate,SortID) VALUES(@KindID,@KindName,@KindIcon,@RuleImg,@Type,@Nullity,@CollectDate,@SortID)";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("KindID", rule.KindID),
                Database.MakeInParam("KindName", rule.KindName),
                Database.MakeInParam("KindIcon", rule.KindIcon),
                Database.MakeInParam("RuleImg", rule.RuleImg),
                Database.MakeInParam("Type", rule.Type), 
                Database.MakeInParam("Nullity", rule.Nullity),
                Database.MakeInParam("SortID", rule.SortID),
                Database.MakeInParam("CollectDate", rule.CollectDate)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());
        }

        /// <summary>
        /// 修改游戏规则
        /// </summary>
        /// <param name="rule">游戏规则</param>
        /// <returns></returns>
        public int UpdateGameRule(GameRule rule)
        {
            string sqlQuery =
                @"UPDATE GameRule SET KindName=@KindName,KindIcon=@KindIcon,RuleImg=@RuleImg,Type=@Type,LogID=@LogID,Nullity= @Nullity,SortID=@SortID WHERE ID= @ID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("KindName", rule.KindName),
                Database.MakeInParam("KindIcon", rule.KindIcon),
                Database.MakeInParam("RuleImg", rule.RuleImg),
                Database.MakeInParam("Type", rule.Type),
                Database.MakeInParam("LogID", rule.LogID),
                Database.MakeInParam("Nullity", rule.Nullity),
                Database.MakeInParam("ID", rule.ID),
                Database.MakeInParam("SortID", rule.SortID)
            };
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 删除游戏规则
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <returns></returns>
        public int DeleteGameRule(string idlist)
        {
            string sqlQuery = $"DELETE GameRule WHERE ID IN({idlist})";
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery);
        }

        /// <summary>
        /// 禁用启用游戏规则
        /// </summary>
        /// <param name="idlist">标识列表</param>
        /// <param name="nullity">禁用启用标识</param>
        /// <returns></returns>
        public int NullityGameRule(string idlist, int nullity)
        {
            string sqlQuery = $"UPDATE GameRule SET Nullity={nullity} WHERE ID IN({idlist})";
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery);
        }

        #endregion

        #region 常见问题

        /// <summary>
        /// 获取常见问题实体 by id
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        public Question GetQuestionInfo(int id)
        {
            return Database.ExecuteObject<Question>($"SELECT * FROM DBO.[Question] WHERE ID={id}");
        }

        /// <summary>
        /// 保存常见问题（新增、修改）通用
        /// </summary>
        /// <param name="quest"></param>
        /// <returns></returns>
        public int SaveQuestionInfo(Question quest)
        {
            var parm = new List<DbParameter>()
            {
                Database.MakeInParam("QuestionTitle", quest.QuestionTitle),
                Database.MakeInParam("Answer", quest.Answer),
                Database.MakeInParam("SortID", quest.SortID)
            };
            if (quest.ID > 0) parm.Add(Database.MakeInParam("ID", quest.ID));

            const string sqlInsert =
                " INSERT DBO.[Question] (QuestionTitle,Answer,SortID) VALUES (@QuestionTitle,@Answer,@SortID) ";
            const string sqlUpdate =
                " UPDATE DBO.[Question] SET QuestionTitle=@QuestionTitle,Answer=@Answer,SortID=@SortID WHERE ID=@ID ";

            return Database.ExecuteNonQuery(CommandType.Text, quest.ID > 0 ? sqlUpdate : sqlInsert, parm.ToArray());
        }

        /// <summary>
        /// 批量删除常见问题
        /// </summary>
        /// <param name="ids"></param>
        /// <returns></returns>
        public int DeleteQuestionInfo(string ids)
        {
            return Database.ExecuteNonQuery($"DELETE DBO.[Question] WHERE ID IN ({ids})");
        }

        #endregion

        #region 数据统计
        /// <summary>
        /// 获取数据统计
        /// </summary>
        /// <param name="sDate">起始时间标识</param>
        /// <param name="eDate">结束时间标识</param>
        /// <returns></returns>
        public DataSet GetDataStatistics(string sDate, string eDate)
        {
            string sqlQuery = "SELECT * FROM StreamEveryDayData(NOLOCK) WHERE DateID>=@SDateID AND DateID<=@EDateID";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("SDateID", sDate),
                Database.MakeInParam("EDateID", eDate)
            };
            return Database.ExecuteDataset(CommandType.Text, sqlQuery, prams.ToArray());
        }
        #endregion

        #region 商品信息

        /// <summary>
        /// 根据商品id获取商品实体
        /// </summary>
        /// <param name="id">商品id</param>
        public AwardInfo GetAwardInfoByID(int id)
        {

            string sql = $"SELECT * FROM AwardInfo WITH(NOLOCK) WHERE AwardID={id}";
            return Database.ExecuteObject<AwardInfo>(sql);
        }

        /// <summary>
        /// 修改商品状态
        /// </summary>
        /// <param name="idList">商品id</param>
        /// <param name="state">商品状态</param>
        public int UpdateAwardInfoNulity(string idList, int state)
        {
            string sql = string.Format("UPDATE dbo.AwardInfo SET Nullity={0} WHERE AwardID IN ({1})", state, idList);
            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 保存商品信息
        /// </summary>
        /// <param name="info">商品实体</param>
        public void InsertAwardInfo(AwardInfo info)
        {


            string sql =
               @"INSERT INTO AwardInfo(AwardName,AwardImg,AwardType,Inventory,CollectDate,Nullity,UnitPrice,Gold,Diamond,SortID)
                            VALUES(@AwardName,@AwardImg,@AwardType,@Inventory,@CollectDate,@Nullity,@UnitPrice,@Gold,@Diamond,@SortID)";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("AwardName", info.AwardName),
                Database.MakeInParam("AwardImg",info.AwardImg),
                Database.MakeInParam("AwardType", info.AwardType),
                Database.MakeInParam("Inventory", info.Inventory),
                Database.MakeInParam("CollectDate", info.CollectDate),
                Database.MakeInParam("Nullity", info.Nullity),
                Database.MakeInParam("UnitPrice", info.UnitPrice),
                Database.MakeInParam("Gold", info.Gold),
                 Database.MakeInParam("Diamond", info.Diamond),
                 Database.MakeInParam("SortID", info.SortID),

            };

            Database.ExecuteNonQuery(CommandType.Text, sql, prams.ToArray());

        }

        /// <summary>
        /// 更新商品信息
        /// </summary>
        /// <param name="info">商品实体</param>
        public int UpdateAwardInfo(AwardInfo info)
        {
            StringBuilder sqlQuery = new StringBuilder();
            sqlQuery.Append("UPDATE AwardInfo SET ")
                    .Append("AwardName=@AwardName,")
                    .Append("AwardImg=@AwardImg,")
                    .Append("Inventory=@Inventory,")
                    .Append("Nullity=@Nullity,")
                    .Append("UnitPrice=@UnitPrice,")
                    .Append("Gold=@Gold,")
                     .Append("SortID=@SortID,")
                    .Append("Diamond=@Diamond,")
                    .Append("AwardType=@AwardType ")
                    .Append(" WHERE AwardID= @AwardID");

            var prams = new List<DbParameter>();
            prams.Add(Database.MakeInParam("AwardName", info.AwardName));
            prams.Add(Database.MakeInParam("AwardImg", info.AwardImg));
            prams.Add(Database.MakeInParam("Inventory", info.Inventory));
            prams.Add(Database.MakeInParam("Nullity", info.Nullity));
            prams.Add(Database.MakeInParam("UnitPrice", info.UnitPrice));
            prams.Add(Database.MakeInParam("Gold", info.Gold));
            prams.Add(Database.MakeInParam("Diamond", info.Diamond));
            prams.Add(Database.MakeInParam("SortID", info.SortID));
            prams.Add(Database.MakeInParam("AwardType", info.AwardType));
            prams.Add(Database.MakeInParam("AwardID", info.AwardID));

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery.ToString(), prams.ToArray());
        }

        /// <summary>
        /// 获取最大键值
        /// </summary>
        public int GetMaxAwardInfoID()
        {
            string sql = string.Format("SELECT MAX(AwardID) FROM dbo.AwardInfo");
            return Convert.ToInt32(Database.ExecuteScalar(CommandType.Text, sql));
        }

        /// <summary>
        /// 判断该类型下是否存在商品
        /// </summary>
        /// <param name="typeId">类型id</param>
        public bool IsHaveGoods(int typeID)
        {
            string sqlQuery = "SELECT TOP 1 * FROM AwardInfo WHERE AwardType=" + typeID;
            int result = Database.ExecuteNonQuery(CommandType.Text, sqlQuery);
            return result > 0 ? true : false;
        }

        #endregion 商品信息

        #region 商品订单

        /// <summary>
        /// 根据订单号获取订单
        /// </summary>
        /// <param name="orderID">订单号</param>
        public AwardOrder GetAwardOrderByID(string orderID)
        {
            string sqlQuery = $"SELECT * FROM AwardOrder WHERE OrderID='{orderID}'";
            return Database.ExecuteObject<AwardOrder>(sqlQuery);
        }

        /// <summary>
        /// 修改订单状态
        /// </summary>
        /// <param name="state">状态值</param>
        /// <param name="OrderID">订单号</param>
        /// <param name="note">处理备注</param>
        public int UpdateOrderState(int state, string orderID)
        {                                    
            string sql = string.Format("UPDATE dbo.AwardOrder SET OrderStatus={0} WHERE OrderID='{1}'",
                state, orderID);
            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 获取新的订单总数
        /// </summary>
        /// <returns></returns>
        public int GetNewOrderCounts()
        {
            string sqlQuery = "SELECT COUNT(OrderID) FROM AwardOrder WHERE OrderStatus=0";
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery);
            if (obj != null)
            {
                return Convert.ToInt32(obj);
            }
            return 0;
        }

        #endregion 商品订单

        #region 每日分享

        /// <summary>
        /// 分享配置
        /// </summary>
        /// <param name="id">配置标识</param>
        /// <returns></returns>
        public ShareConfig GetShareConfig(int id)
        {
            return Database.ExecuteObject<ShareConfig>($"SELECT * FROM ShareConfig(NOLOCK) WHERE ID = {id}");
        }
        /// <summary>
        /// 修改每日分享
        /// </summary>
        /// <param name="config">分享配置</param>

        public int UpdateShareConfig(ShareConfig config)
        {
            string sqlQuery = "UPDATE ShareConfig SET DayShareLmt=@DayShareLmt,TimeShareGold=@TimeShareGold,TimeShareDiamond=@TimeShareDiamond,Nullity=@Nullity WHERE ID=@ID";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("DayShareLmt", config.DayShareLmt),
                Database.MakeInParam("TimeShareGold", config.TimeShareGold),
                Database.MakeInParam("TimeShareDiamond", config.TimeShareDiamond),
                Database.MakeInParam("Nullity", config.Nullity),
                Database.MakeInParam("ID", config.ID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        #endregion
    }
}
