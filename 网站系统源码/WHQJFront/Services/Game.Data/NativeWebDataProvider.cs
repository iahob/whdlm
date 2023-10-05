using System.Collections.Generic;
using Game.Kernel;
using Game.IData;
using System.Data.Common;
using System.Data;
using Game.Entity.Accounts;
using Game.Entity.Agent;
using Game.Entity.NativeWeb;
using Game.Entity.Treasure;
using System.Text;

namespace Game.Data
{
    /// <summary>
    /// 网站数据访问层
    /// </summary>
    public class NativeWebDataProvider : BaseDataProvider, INativeWebDataProvider
    {
        #region 构造方法

        public NativeWebDataProvider(string connString)
            : base(connString)
        {
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
            PagerParameters pager = new PagerParameters(GameRule.Tablename, "ORDER BY KindID DESC", "WHERE Nullity=0",
                pageIndex, pageSize);
            string[] returnField = {"KindID", "KindName", "KindIcon", "KindIntro"};
            pager.Fields = returnField;
            pager.CacherSize = 2;

            return GetPagerSet2(pager);
        }

        /// <summary>
        /// 新闻公告数据分页
        /// </summary>
        /// <param name="pageIndex">分页下标</param>
        /// <param name="pageSize">分页大小</param>
        /// <returns></returns>
        public PagerSet GetNewPageList(int pageIndex, int pageSize)
        {
            PagerParameters pager = new PagerParameters(SystemNotice.Tablename,
                "ORDER BY IsTop DESC,IsHot DESC,SortID ASC,NoticeID DESC", "WHERE Nullity=0", pageIndex, pageSize);
            string[] returnField = {"NoticeID", "NoticeTitle", "MoblieContent", "PublisherTime"};
            pager.Fields = returnField;
            pager.CacherSize = 2;

            return GetPagerSet2(pager);
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
            const string sqlQuery =
                @"SELECT ConfigKey,Field1,Field2,Field3,Field4,Field5,Field6,Field7,Field8,Field9,Field10,Field11,Field12,Field13 FROM ConfigInfo WITH(NOLOCK) WHERE ConfigKey = @ConfigKey";
            List<DbParameter> parms = new List<DbParameter> {Database.MakeInParam("ConfigKey", configKey)};
            return Database.ExecuteObject<ConfigInfo>(sqlQuery, parms);
        }

        /// <summary>
        /// 获取配置信息集合
        /// </summary>
        /// <returns></returns>
        public IList<ConfigInfo> GetConfigInfoList()
        {
            string sqlQuery =
                @"SELECT ConfigKey,Field1,Field2,Field3,Field4,Field5,Field6,Field7,Field8,Field9,Field10,Field11,Field12,Field13 FROM ConfigInfo WITH(NOLOCK)";
            return Database.ExecuteObjectList<ConfigInfo>(sqlQuery);
        }

        #endregion

        #region 手机登录信息

        /// <summary>
        /// 获取手机登录数据
        /// </summary>
        /// <returns></returns>
        public DataSet GetMobileLoginInfo()
        {
            return Database.ExecuteDataset(CommandType.StoredProcedure, "NET_PW_GetMobileLoginData");
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
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("UserID", userid),
                Database.MakeInParam("TypeID", typeid)
            };
            return Database.ExecuteDataset(CommandType.StoredProcedure, "NET_PW_GetDayRankingData", prams.ToArray());
        }

        /// <summary>
        /// 领取排行榜奖励
        /// </summary>
        /// <param name="userId">用户标识</param>
        /// <param name="dateId">时间标识</param>
        /// <param name="typeId">排行榜类型</param>
        /// <param name="ip">领取地址</param>
        /// <returns></returns>
        public Message ReceiveRankingAward(int userId, int dateId, int typeId, string ip)
        {
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", userId),
                Database.MakeInParam("dwDateID", dateId),
                Database.MakeInParam("dwTypeID", typeId),
                Database.MakeInParam("strClientIP", ip),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };

            return MessageHelper.GetMessageForObject<ReceiveRankAward>(Database, "NET_PJ_RecevieRankingAward", prams);
        }

        #endregion

        #region 新闻信息

        /// <summary>
        /// 获取首页新闻公告
        /// </summary>
        /// <returns></returns>
        public IList<SystemNotice> GetHomePageNews()
        {
            string sqlQuery =
                @"SELECT TOP 13 NoticeID,NoticeTitle,MoblieContent,PublisherTime,IsHot,IsTop FROM SystemNotice WITH(NOLOCK) WHERE Nullity=0 ORDER BY IsTop DESC,IsHot DESC,SortID ASC,NoticeID DESC";
            return Database.ExecuteObjectList<SystemNotice>(sqlQuery);
        }

        /// <summary>
        /// 获取新闻详情
        /// </summary>
        /// <param name="id">新闻标识</param>
        /// <returns></returns>
        public SystemNotice GetWebNewsInfo(int id)
        {
            const string sqlQuery =
                @"SELECT NoticeTitle,WebContent,Publisher,PublisherTime FROM SystemNotice WITH(NOLOCK) WHERE NoticeID = @NoticeID";
            List<DbParameter> parms = new List<DbParameter> {Database.MakeInParam("NoticeID", id)};
            return Database.ExecuteObject<SystemNotice>(sqlQuery, parms);
        }

        #endregion

        #region 游戏规则

        /// <summary>
        /// 获取游戏规则列表
        /// </summary>
        /// <returns></returns>
        public IList<GameRule> GetGameRuleList()
        {
            string sqlQuery =
                @"SELECT KindID,KindName,KindIcon,KindIntro,RuleImg,Type,LogID FROM GameRule WITH(NOLOCK) WHERE Nullity=0 ORDER BY SortID ASC,KindID DESC";
            return Database.ExecuteObjectList<GameRule>(sqlQuery);
        }

        /// <summary>
        /// 获取游戏规则
        /// </summary>
        /// <param name="kindid">游戏规则</param>
        /// <returns></returns>
        public GameRule GetGameRuleInfo(int kindid)
        {
            const string sqlQuery =
                @"SELECT KindName,KindIcon,KindIntro,KindRule FROM GameRule WITH(NOLOCK) WHERE KindID = @KindID";
            List<DbParameter> parms = new List<DbParameter> {Database.MakeInParam("KindID", kindid)};
            return Database.ExecuteObject<GameRule>(sqlQuery, parms);
        }

        #endregion

        #region 广告位图片

        /// <summary>
        /// 获取广告图列表
        /// </summary>
        /// <returns></returns>
        public IList<Ads> GetAdsList()
        {
            string sqlQuery =
                @"SELECT ID,Title,ResourceURL,LinkURL,[Type],SortID FROM Ads WITH(NOLOCK) WHERE [Type]!=3 AND [Type]!=4 ORDER BY [Type] DESC,SortID ASC";
            return Database.ExecuteObjectList<Ads>(sqlQuery);
        }

        #endregion

        #region 常见问题

        /// <summary>
        /// 获取常见问题列表
        /// </summary>
        /// <returns></returns>
        public IList<Question> GetQAList(int top = 0)
        {
            string topSql = top > 0 ? $" TOP {top} " : "";
            IList<Question> list =
                Database.ExecuteObjectList<Question>(
                    $"SELECT {topSql} * FROM DBO.[Question] ORDER BY SortID ASC,ID DESC ");
            return list;
        }

        #endregion

        #region 商城信息

        /// <summary>
        /// 获取商品首页信息
        /// </summary>
        /// <returns></returns>
        public DataSet GetAwardHomeList(int typeid)
        {
            string where = "";
            if (typeid > 0)
            {
                where = " and TypeID=" + typeid;
            }

            string sqlQuery =
                @"SELECT TypeID,TypeName,'' as ClassName FROM AwardType(NOLOCK) WHERE Nullity=0 ORDER BY SortID DESC;
                                SELECT AwardID,AwardType,AwardName,AwardImg,UnitPrice FROM AwardInfo(NOLOCK) 
                                WHERE AwardType IN(SELECT TOP 1 TypeID FROM AwardType(NOLOCK) WHERE Nullity=0 " +
                where + @" 
                                ORDER BY SortID DESC) AND Nullity=0 ORDER BY SortID DESC";
            return Database.ExecuteDataset(sqlQuery);
        }

        /// <summary>
        /// 获取商品信息列表
        /// </summary>
        /// <param name="typeid">商品类型</param>
        /// <returns></returns>
        public DataSet GetAwardInfoByType(int typeid)
        {
            string sqlQuery = @"SELECT AwardID,AwardName,AwardImg,UnitPrice,AwardType FROM AwardInfo(NOLOCK) 
                                WHERE AwardType=@AwardType AND Nullity=0 ORDER BY SortID DESC";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("AwardType", typeid)
            };
            return Database.ExecuteDataset(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 获取下单信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="awardid">商品标识</param>
        /// <returns></returns>
        public DataSet GetOrderAwardInfo(int userid, int awardid)
        {
            string sqlQuery =
                @"SELECT AwardID,AwardType,AwardName,AwardImg,UnitPrice FROM AwardInfo(NOLOCK) WHERE AwardID=@AwardID AND Nullity=0;
                                SELECT Compellation,MobilePhone,Province,City,Area,DetailAddress FROM AwardUser(NOLOCK) WHERE UserID=@UserID";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("AwardID", awardid),
                Database.MakeInParam("UserID", userid)
            };
            return Database.ExecuteDataset(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 获取兑换商品记录
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        public DataSet GetAwardOrderList(int userid)
        {
            string sqlQuery =
                @"SELECT OrderID,AwardName,BuyNum,PayTicket,OrderStatus,CollectDate FROM AwardOrder(NOLOCK) 
                                WHERE UserID=@UserID ORDER BY CollectDate DESC";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("UserID", userid)
            };
            return Database.ExecuteDataset(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 获取商品实体
        /// </summary>
        /// <param name="awradID"></param>
        /// <returns></returns>
        public AwardInfo GetAwardInfo(int awardID)
        {
            string sql = "SELECT * FROM AwardInfo WHERE AwardID=@AwardID";
            List<DbParameter> parms = new List<DbParameter>();
            parms.Add(Database.MakeInParam("AwardID", awardID));
            return Database.ExecuteObject<AwardInfo>(sql, parms);
        }


        /// <summary>
        /// 获取玩家兑换地址信息
        /// </summary>
        /// <param name="awradID"></param>
        /// <returns></returns>
        public AwardUser GetAwardUser(int userid)
        {
            string sql = "SELECT * FROM AwardUser WHERE userid=@userid";
            List<DbParameter> parms = new List<DbParameter>();
            parms.Add(Database.MakeInParam("userid", userid));
            return Database.ExecuteObject<AwardUser>(sql, parms);
        }

        /// <summary>
        /// 购买商品
        /// </summary>
        /// <param name="awardOrder"></param>
        /// <returns></returns>
        public Message BuyAward(AwardOrder awardOrder)
        {
            List<DbParameter> parms = new List<DbParameter>();
            parms.Add(Database.MakeInParam("UserID", awardOrder.UserID));
            parms.Add(Database.MakeInParam("GameID", awardOrder.GameID));
            parms.Add(Database.MakeInParam("AwardID", awardOrder.AwardID));
            parms.Add(Database.MakeInParam("AwardType", awardOrder.AwardType));
            parms.Add(Database.MakeInParam("AwardName", awardOrder.AwardName));
            parms.Add(Database.MakeInParam("AwardPrice", awardOrder.UnitPrice));
            parms.Add(Database.MakeInParam("AwardCount", awardOrder.BuyNum));
            parms.Add(Database.MakeInParam("TotalAmount", awardOrder.PayTicket));
            parms.Add(Database.MakeInParam("Gold", awardOrder.Gold));
            parms.Add(Database.MakeInParam("Diamond", awardOrder.Diamond));
            parms.Add(Database.MakeInParam("Compellation", awardOrder.Compellation));
            parms.Add(Database.MakeInParam("MobilePhone", awardOrder.MobilePhone));
            parms.Add(Database.MakeInParam("Province", awardOrder.Province));
            parms.Add(Database.MakeInParam("City", awardOrder.City));
            parms.Add(Database.MakeInParam("Area", awardOrder.Area));
            parms.Add(Database.MakeInParam("DwellingPlace", awardOrder.DetailAddress));
            parms.Add(Database.MakeInParam("BuyIP", awardOrder.ClinetIP));
            parms.Add(Database.MakeInParam("OrderID", awardOrder.OrderID));
            parms.Add(Database.MakeOutParam("strErrorDescribe", typeof(string), 127));
            Message msg = MessageHelper.GetMessageForObject<AwardOrder>(Database, "WSP_PW_BuyAward", parms);
            return msg;
        }

        /// <summary>
        /// 添加兑换联系信息
        /// </summary>
        /// <param name="User"></param>
        /// <returns></returns>
        public int insertAwardUser(AwardUser User)
        {
            string sqlQuery =
                @"INSERT INTO AwardUser(UserID,Compellation,MobilePhone,Province,City,Area,DetailAddress,CollectDate) 
                            VALUES(@UserID,@Compellation,@MobilePhone,@Province,@City,@Area,@DetailAddress,getdate())";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("UserID", User.UserID),
                Database.MakeInParam("Compellation", User.Compellation),
                Database.MakeInParam("MobilePhone", User.MobilePhone),
                Database.MakeInParam("Province", User.Province),
                Database.MakeInParam("City", User.City),
                Database.MakeInParam("Area", User.Area),
                Database.MakeInParam("DetailAddress", User.DetailAddress)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        /// <summary>
        /// 修改兑换联系信息
        /// </summary>
        /// <param name="User"></param>
        /// <returns></returns>
        public int updateAwardUser(AwardUser User)
        {
            StringBuilder sqlQuery = new StringBuilder();
            sqlQuery.Append("UPDATE AwardUser SET ")
                .Append("Compellation=@Compellation, ")
                .Append("MobilePhone=@MobilePhone, ")
                .Append("Province=@Province, ")
                .Append("City=@City, ")
                .Append("Area=@Area, ")
                .Append("DetailAddress=@DetailAddress ")
                .Append("WHERE UserID=@UserID");

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("UserID", User.UserID),
                Database.MakeInParam("Compellation", User.Compellation),
                Database.MakeInParam("MobilePhone", User.MobilePhone),
                Database.MakeInParam("Province", User.Province),
                Database.MakeInParam("City", User.City),
                Database.MakeInParam("Area", User.Area),
                Database.MakeInParam("DetailAddress", User.DetailAddress)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery.ToString(), prams.ToArray());
        }

        #endregion


        /// <summary>
        /// 获取排行榜数据
        /// </summary>
        /// <param name="typeid">排行榜类型</param>
        /// <returns></returns>
        public DataSet GetDayRankingData(int typeid)
        {
            var prams = new List<DbParameter> {Database.MakeInParam("TypeID", typeid)};
            return Database.ExecuteDataset(CommandType.StoredProcedure, "NET_PW_GetDayRankingData", prams.ToArray());
        }

        #region 每日分享

        /// <summary>
        /// 每日分享奖励
        /// </summary>
        /// <param name="userid">登录用户标识</param>
        /// <returns></returns>
        public DataSet GetTimesReward(int userid, string ClientIP)
        {
            List<DbParameter> prams = new List<DbParameter>
                {Database.MakeInParam("userid", userid), Database.MakeInParam("strClientIP", ClientIP)};

            return Database.ExecuteDataset(CommandType.StoredProcedure, "PW_TimesReward", prams.ToArray());
        }

        /// <summary>
        /// 获取每日奖励配置
        /// </summary>
        /// <returns></returns>
        public DataSet GetShareReward()
        {
            string sql = "SELECT * FROM dbo.ShareConfig(NOLOCK) WHERE ID=1";
            return Database.ExecuteDataset(sql);
        }

        #endregion

        #region 微信扫码登录

        /// <summary>
        /// 获取微信扫码登录记录By Key
        /// </summary>
        /// <param name="key">验证值</param>
        /// <returns><see cref="WxScanLoginRecord"/></returns>
        public WxScanLoginRecord GetWxScanLoginRecordByKey(string key)
        {
            return Database.ExecuteObject<WxScanLoginRecord>(
                $"SELECT * FROM {WxScanLoginRecord.Tablename}(NOLOCK) WHERE [Key]=N'{key}'");
        }

        /// <summary>
        /// 保存微信扫码登录记录（新增、更新（只更新状态和获取ip））
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int SaveWxScanLoginRecord(WxScanLoginRecord model)
        {
            var sql = model.RecordID > 0
                ? "UPDATE [dbo].[WxScanLoginRecord] SET " +
                  $" [State]={model.State},[OpenID]=N'{model.OpenID}',[UnionID]=N'{model.OpenID}'," +
                  $" [AccessToken]=N'{model.AccessToken}',[RefreshToken]=N'{model.RefreshToken}', WHERE Key={model.Key}"
                : "INSERT INTO [dbo].[WxScanLoginRecord] ([Key] ,[OpenID] ,[UnionID] ,[AccessToken] ,[RefreshToken] ,[State] ,[CollectClientIP]) " +
                  $" VALUES(N'{model.Key}',N'{model.OpenID}',N'{model.UnionID}',N'{model.AccessToken}',N'{model.RefreshToken}',{model.State},N'{model.CollectClientIP}')";

            return Database.ExecuteNonQuery(sql);
        }

        /// <summary>
        /// 更新为已获取状态
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int PickWxScanLoginRecord(WxScanLoginRecord model)
        {
            var sql = "UPDATE[dbo].[WxScanLoginRecord] SET " +
                      $" [State]={model.State},PickDate=GETDATE(),PickClientIP=N'{model.PickClientIP}' WHERE RecordID={model.RecordID}";
            return Database.ExecuteNonQuery(sql);
        }

        #endregion
    }
}