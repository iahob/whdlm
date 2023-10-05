using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using Game.Entity.Accounts;
using Game.IData;
using Game.Kernel;

namespace Game.Data
{
    /// <summary>
    /// 帐号库数据层
    /// </summary>
    public class AccountsDataProvider : BaseDataProvider, IAccountsDataProvider
    {
        #region 构造方法

        /// <summary>
        /// 构造函数
        /// </summary>
        public AccountsDataProvider(string connString)
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

        #region 用户信息
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetUserPageList(string where, string order, int pageIndex, int pageSize)
        {
            PagerParameters pagerPrams = new PagerParameters("AccountsInfo", where, order, pageIndex, pageSize, null, "WEB_PageView_UserList");
            return GetPagerSet(pagerPrams);
        }
        /// <summary>
        /// 高效获取用户简洁信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="gameid">游戏标识</param>
        /// <param name="agentid">代理标识</param>
        /// <param name="account">用户账号</param>
        /// <param name="nickname">用户昵称</param>
        /// <returns></returns>
        public UserInfo GetUserInfo(int userid=0, int gameid=0, int agentid=0, string account="", string nickname="")
        {
            List<DbParameter> prams = new List<DbParameter>
            {
                Database.MakeInParam("dwUserID", userid),
                Database.MakeInParam("dwGameID", gameid),
                Database.MakeInParam("dwAgentID", agentid),
                Database.MakeInParam("dwAccounts", account),
                Database.MakeInParam("dwNickName", nickname)
            };
            DataSet ds = Database.ExecuteDataset(CommandType.StoredProcedure, "NET_Batch_GetUserInfo", prams.ToArray());
            UserInfo info = new UserInfo();
            if (ds != null && ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
            {
                DataRow row = ds.Tables[0].Rows[0];
                info.UserID = Convert.ToInt32(row["UserID"]);
                info.SpreaderID = Convert.ToInt32(row["SpreaderID"]);
                info.FaceID = Convert.ToInt32(row["FaceID"]);
                info.GameID = Convert.ToInt32(row["GameID"]);
                info.AgentID = Convert.ToInt32(row["AgentID"]);
                info.Accounts = row["Accounts"].ToString();
                info.NickName = row["NickName"].ToString();
            }
            return info;
        }
        /// <summary>
        /// 根据用户标识获取用户信息
        /// </summary>
        /// <param name="userId">用户标识</param>
        /// <returns></returns>
        public AccountsInfo GetAccountInfoByUserId(int userId)
        {
            string sqlQuery = $"SELECT * FROM AccountsInfo WITH(NOLOCK) WHERE UserID={userId}";
            return Database.ExecuteObject<AccountsInfo>(sqlQuery);
        }
        /// <summary>
        /// 根据游戏ID获取用户信息
        /// </summary>
        /// <param name="gameId">游戏ID</param>
        /// <returns></returns>
        public AccountsInfo GetAccountInfoByGameId(int gameId)
        {
            string sqlQuery = $"SELECT * FROM AccountsInfo WITH(NOLOCK) WHERE GameID={gameId}";
            return Database.ExecuteObject<AccountsInfo>(sqlQuery);
        }
        /// <summary>
        /// 冻结解冻账号
        /// </summary>
        /// <param name="userlist">用户列表</param>
        /// <param name="nullity">状态值（0 正常 1 冻结）</param>
        /// <returns></returns>
        public int NullityAccountInfo(string userlist, int nullity)
        {
            string sqlQuery = $"UPDATE AccountsInfo SET Nullity={nullity} WHERE UserID IN({userlist})";
            return Database.ExecuteNonQuery(sqlQuery);
        }
        /// <summary>
        /// 设置取消机器人
        /// </summary>
        /// <param name="userlist">用户列表</param>
        /// <param name="isAndroid">状态值（0 正常 1 机器人）</param>
        /// <returns></returns>
        public int AndroidAccountInfo(string userlist, int isAndroid)
        {
            string sqlQuery = $"UPDATE AccountsInfo SET IsAndroid={isAndroid} WHERE UserID IN({userlist})";
            return Database.ExecuteNonQuery(sqlQuery);
        }
        /// <summary>
        /// 获取用户基本信息通过帐号
        /// </summary>
        /// <param name="account">用户帐号</param>
        /// <returns></returns>
        public AccountsInfo GetAccountInfoByAccount(string account)
        {
            string sqlQuery = string.Format("SELECT * FROM AccountsInfo(NOLOCK) WHERE Accounts=@Accounts");

            var prams = new List<DbParameter>();
            prams.Add(Database.MakeInParam("Accounts", account));

            AccountsInfo model = Database.ExecuteObject<AccountsInfo>(sqlQuery, prams);
            if (model != null)
                return model;
            else
                return new AccountsInfo();
        }

        /// <summary>
        /// 修改用户信息
        /// </summary>
        /// <param name="accountsInfo">用户信息</param>
        /// <returns></returns>
        public int UpdateAccountsBaseInfo(AccountsInfo accountsInfo)
        {
            string sqlQuery = @"UPDATE AccountsInfo SET UnderWrite=@UnderWrite,PassPortID=@PassPortID,
                Compellation=@Compellation,Nullity=@Nullity,StunDown=@StunDown,MoorMachine=@MoorMachine,
                IsAndroid=@IsAndroid,InsurePass=@InsurePass,LogonPass=@LogonPass,RegisterMobile=@RegisterMobile WHERE UserID=@UserID";

            List<DbParameter> prams = new List<DbParameter>
            {
                Database.MakeInParam("UnderWrite", accountsInfo.UnderWrite),
                Database.MakeInParam("PassPortID", accountsInfo.PassPortID),
                Database.MakeInParam("Compellation", accountsInfo.Compellation),
                Database.MakeInParam("Nullity", accountsInfo.Nullity),
                Database.MakeInParam("StunDown", accountsInfo.StunDown),
                Database.MakeInParam("MoorMachine", accountsInfo.MoorMachine),
                Database.MakeInParam("IsAndroid", accountsInfo.IsAndroid),
                Database.MakeInParam("RegisterMobile", accountsInfo.RegisterMobile),
                Database.MakeInParam("LogonPass", accountsInfo.LogonPass),
                Database.MakeInParam("InsurePass", accountsInfo.InsurePass),
                Database.MakeInParam("UserID", accountsInfo.UserID)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 随机获取10个靓号
        /// </summary>
        /// <returns></returns>
        public DataSet GetReserveIdentifierList()
        {
            string sqlQuery = @"SELECT TOP 1 GameID FROM ReserveIdentifier WITH(NOLOCK) WHERE Distribute=0 ORDER BY NEWID()";
            return Database.ExecuteDataset(CommandType.Text, sqlQuery);
        }
        /// <summary>
        /// 获取用户头像地址
        /// </summary>
        /// <param name="customid">头像标识</param>
        /// <returns></returns>
        public string GetAccountsFaceById(int customid)
        {
            string sqlQuery = $"SELECT FaceUrl FROM AccountsFace WITH(NOLOCK) WHERE ID={customid}";
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery);
            return obj?.ToString() ?? "";
        }
        /// <summary>
        /// 批量设置取消转账权限
        /// </summary>
        /// <param name="userlist">用户列表</param>
        /// <param name="userRight">权限值</param>
        /// <param name="calc"></param>
        /// <returns></returns>
        public int TransferPowerAccounts(string userlist, int userRight, string calc = "|")
        {
            string sqlQuery = $"UPDATE AccountsInfo SET UserRight= UserRight {calc} {userRight} WHERE UserID IN({userlist})";
            return Database.ExecuteNonQuery(sqlQuery);
        }

        /// <summary>
        /// 全部设置取消转账权限
        /// </summary>
        /// <param name="userRight">权限值</param>
        /// <param name="calc"></param>
        /// <returns></returns>
        public int TransferPowerAccounts(int userRight,string calc = "|")
        {
            string sqlQuery = $"UPDATE AccountsInfo SET UserRight=UserRight {calc} {userRight}";
            return Database.ExecuteNonQuery(sqlQuery);
        }

        /// <summary>
        /// 添加超端管理员
        /// </summary>
        /// <param name="user">用户信息（用户名、登录密码）</param>
        /// <returns></returns>
        public Message InsertSuperUser(AccountsInfo user)
        {
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("strAccounts", user.Accounts),
                Database.MakeInParam("strLogonPass", user.LogonPass),
                Database.MakeInParam("dwGrantGold",user.UserRight),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };

            return MessageHelper.GetMessage(Database, "NET_PM_AddSuperUser", prams);
        }
        #endregion

        #region 限制管理
        /// <summary>
        /// 获取限制地址信息
        /// </summary>
        /// <param name="strAddress">限制地址</param>
        /// <returns></returns>
        public ConfineAddress GetConfineAddressByAddress(string strAddress)
        {
            string sqlQuery = "SELECT * FROM ConfineAddress WITH(NOLOCK) WHERE AddrString=@AddrString";
            var prams = new List<DbParameter> {Database.MakeInParam("AddrString", strAddress)};
            return Database.ExecuteObject<ConfineAddress>(sqlQuery, prams);
        }
        /// <summary>
        /// 新增限制地址信息
        /// </summary>
        /// <param name="address">地址信息</param>
        /// <returns></returns>
        public int InsertConfineAddress(ConfineAddress address)
        {
            string sqlQuery = "INSERT INTO ConfineAddress VALUES(@AddrString,@EnjoinLogon,@EnjoinRegister,@EnjoinOverDate,@CollectDate,@CollectNote)";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("AddrString", address.AddrString),
                Database.MakeInParam("EnjoinLogon", address.EnjoinLogon),
                Database.MakeInParam("EnjoinRegister", address.EnjoinRegister),
                Database.MakeInParam("EnjoinOverDate",
                    address.EnjoinOverDate.Equals(Convert.ToDateTime("1900-01-01"))
                        ? null
                        : address.EnjoinOverDate.ToString()),
                Database.MakeInParam("CollectDate", DateTime.Now),
                Database.MakeInParam("CollectNote", address.CollectNote)
            };
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        ///  更新限制地址信息
        /// </summary>
        /// <param name="address">地址信息</param>
        /// <returns></returns>
        public int UpdateConfineAddress(ConfineAddress address)
        {
            string sqlQuery = @"UPDATE ConfineAddress SET EnjoinLogon=@EnjoinLogon,EnjoinRegister=@EnjoinRegister,
                    EnjoinOverDate=@EnjoinOverDate,CollectNote=@CollectNote WHERE AddrString=@AddrString";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("EnjoinLogon", address.EnjoinLogon),
                Database.MakeInParam("EnjoinRegister", address.EnjoinRegister),
                Database.MakeInParam("EnjoinOverDate",
                    address.EnjoinOverDate.Equals(Convert.ToDateTime("1900-01-01"))
                        ? null
                        : address.EnjoinOverDate.ToString()),
                Database.MakeInParam("CollectNote", address.CollectNote),
                Database.MakeInParam("AddrString", address.AddrString)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 删除限制地址信息
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public int DeleteConfineAddress(string where)
        {
            string sqlQuery = $"DELETE ConfineAddress {where}";
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery);
        }
        /// <summary>
        /// 获取IP注册前100名
        /// </summary>
        /// <returns></returns>
        public DataSet GetIpRegisterTop100()
        {
            return Database.ExecuteDataset(CommandType.StoredProcedure, "WSP_PM_GetIPRegisterTop100");
        }
        /// <summary>
        /// 批量插入限制IP
        /// </summary>
        /// <param name="ipList">ip列表</param>
        /// <returns></returns>
        public void BatchInsertConfineAddress(string ipList)
        {
            var prams = new List<DbParameter> {Database.MakeInParam("strIpList", ipList)};
            Database.ExecuteNonQuery(CommandType.StoredProcedure, "NET_PM_BatchInsertConfineAddress", prams.ToArray());
        }
        /// <summary>
        /// 获取限制机器码信息
        /// </summary>
        /// <param name="strSerial">限制机器码</param>
        /// <returns></returns>
        public ConfineMachine GetConfineMachineBySerial(string strSerial)
        {
            string sqlQuery = "SELECT * FROM ConfineMachine WITH(NOLOCK) WHERE MachineSerial=@MachineSerial";
            var prams = new List<DbParameter> {Database.MakeInParam("MachineSerial", strSerial)};
            return Database.ExecuteObject<ConfineMachine>(sqlQuery, prams);
        }
        /// <summary>
        /// 新增限制机器码信息
        /// </summary>
        /// <param name="machine">机器码信息</param>
        /// <returns></returns>
        public int InsertConfineMachine(ConfineMachine machine)
        {
            string sqlQuery = "INSERT INTO ConfineMachine VALUES(@MachineSerial,@EnjoinLogon,@EnjoinRegister,@EnjoinOverDate,@CollectDate,@CollectNote)";
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("MachineSerial", machine.MachineSerial),
                Database.MakeInParam("EnjoinLogon", machine.EnjoinLogon),
                Database.MakeInParam("EnjoinRegister", machine.EnjoinRegister),
                Database.MakeInParam("EnjoinOverDate",
                    machine.EnjoinOverDate.Equals(Convert.ToDateTime("1900-01-01"))
                        ? null
                        : machine.EnjoinOverDate.ToString()),
                Database.MakeInParam("CollectDate", DateTime.Now),
                Database.MakeInParam("CollectNote", machine.CollectNote)
            };
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 修改限制机器码信息
        /// </summary>
        /// <param name="machine">机器码信息</param>
        /// <returns></returns>
        public int UpdateConfineMachine(ConfineMachine machine)
        {
            string sqlQuery = @"UPDATE ConfineMachine SET EnjoinLogon=@EnjoinLogon,EnjoinRegister=@EnjoinRegister,
                    EnjoinOverDate=@EnjoinOverDate,CollectNote=@CollectNote WHERE MachineSerial=@MachineSerial";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("EnjoinLogon", machine.EnjoinLogon),
                Database.MakeInParam("EnjoinRegister", machine.EnjoinRegister),
                Database.MakeInParam("EnjoinOverDate",
                    machine.EnjoinOverDate.Equals(Convert.ToDateTime("1900-01-01"))
                        ? null
                        : machine.EnjoinOverDate.ToString()),
                Database.MakeInParam("CollectNote", machine.CollectNote),
                Database.MakeInParam("MachineSerial", machine.MachineSerial)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        /// <summary>
        /// 删除限制机器码信息
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public int DeleteConfineMachine(string where)
        {
            string sqlQuery = $"DELETE ConfineMachine {where}";
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery);
        }
        /// <summary>
        /// 机器码注册前100名
        /// </summary>
        /// <returns></returns>
        public DataSet GetMachineRegisterTop100()
        {
            return Database.ExecuteDataset(CommandType.StoredProcedure, "WSP_PM_GetMachineRegisterTop100");
        }
        /// <summary>
        /// 批量插入限制机器码
        /// </summary>
        /// <param name="machineList">机器码列表</param>
        /// <returns></returns>
        public void BatchInsertConfineMachine(string machineList)
        {
            var prams = new List<DbParameter> {Database.MakeInParam("strMachineList", machineList)};
            Database.ExecuteNonQuery(CommandType.StoredProcedure, "NET_PM_BatchInsertConfineMachine", prams.ToArray());
        }
        #endregion

        #region 系统配置
        /// <summary>
        /// 获取系统配置列表
        /// </summary>
        /// <returns></returns>
        public DataSet GetSystemStatusList()
        {
            string sqlQuery = "SELECT * FROM SystemStatusInfo(NOLOCK) where StatusName<>'JJPayBindSpread' ORDER BY SortID ASC";
            return Database.ExecuteDataset(sqlQuery);
        }
        /// <summary>
        /// 获取配置信息
        /// </summary>
        /// <param name="statusName">配置键值</param>
        /// <returns></returns>
        public SystemStatusInfo GetSystemStatusInfo(string statusName)
        {
            string sqlQuery = @"SELECT * FROM SystemStatusInfo WITH(NOLOCK) WHERE StatusName=@StatusName";
            var prams = new List<DbParameter> {Database.MakeInParam("StatusName", statusName)};
            return Database.ExecuteObject<SystemStatusInfo>(sqlQuery, prams);
        }
        /// <summary>
        /// 修改配置信息
        /// </summary>
        /// <param name="statusinfo">配置信息</param>
        /// <returns></returns>
        public int UpdateSystemStatusInfo(SystemStatusInfo statusinfo)
        {
            string sqlQuery = @"UPDATE SystemStatusInfo SET StatusValue=@StatusValue,StatusString=@StatusString,
                    StatusTip=@StatusTip,StatusDescription=@StatusDescription WHERE StatusName=@StatusName";

            var prams = new List<DbParameter>
            {
                Database.MakeInParam("StatusValue", statusinfo.StatusValue),
                Database.MakeInParam("StatusString", statusinfo.StatusString),
                Database.MakeInParam("StatusTip", statusinfo.StatusTip),
                Database.MakeInParam("StatusDescription", statusinfo.StatusDescription),
                Database.MakeInParam("StatusName", statusinfo.StatusName)
            };

            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }
        #endregion

        #region 消息推送
        /// <summary>
        /// 获取推送消息用户列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public IList<AccountsUmeng> GetAccountsUmengList(string where)
        {
            string sql = $"SELECT * FROM AccountsUmeng WITH(NOLOCK) {where}";
            return Database.ExecuteObjectList<AccountsUmeng>(sql);
        }
        /// <summary>
        /// 获取推送消息对象
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        public AccountsUmeng GetAccountsUmeng(int userid)
        {
            string sql = $"SELECT * FROM AccountsUmeng WITH(NOLOCK) WHERE UserID={userid}";
            return Database.ExecuteObject<AccountsUmeng>(sql);
        }
        #endregion

        #region 游戏配置
        /// <summary>
        /// 获取游戏配置
        /// </summary>
        /// <param name="uid">用户或代理标识</param>
        /// <param name="typeid">配置类型（0 通用  1 玩家  2 代理）</param>
        /// <returns></returns>
        public DataSet GetUserLoadGameList(int uid, int typeid)
        {
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("dwUID", uid),
                Database.MakeInParam("dwTypeID", typeid)
            };

            return Database.ExecuteDataset(CommandType.StoredProcedure, "NET_PW_GetUserLoadGameList", prams.ToArray());
        }
        /// <summary>
        /// 新增游戏配置
        /// </summary>
        /// <param name="game">配置信息</param>
        /// <returns></returns>
        public Message AddUserLoadGame(AccountsGame game)
        {
            var prams = new List<DbParameter>
            {
                Database.MakeInParam("dwUniteID", game.UniteID),
                Database.MakeInParam("dwTypeID", game.TypeID),
                Database.MakeInParam("dwKindID", game.KindID),
                Database.MakeInParam("dwSortID", game.SortID)
            };

            return MessageHelper.GetMessage(Database, "NET_PW_AddUserLoadGame", prams);
        }
        /// <summary>
        /// 删除游戏
        /// </summary>
        /// <param name="idList">id列表</param>
        /// <returns></returns>
        public int DeleteUserLoadGame(string idList)
        {
            string sqlQuery = string.Format("DELETE AccountsGame WHERE ID IN({0})", idList);
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery);
        }
        #endregion

        #region 注册赠送配置
        /// <summary>
        /// 获取注册赠送实体 by id
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        public RegisterGive GetRegisterGiveInfo(int id)
        {
            return Database.ExecuteObject<RegisterGive>($"SELECT * FROM DBO.[RegisterGive] WHERE ConfigID={id}");
        }
        /// <summary>
        /// 获取注册赠送实体 by id
        /// </summary>
        /// <param name="Type"></param>
        /// <returns></returns>
        public RegisterGive GetRegisterGiveInfoByPlatformType(int Type)
        {
            return Database.ExecuteObject<RegisterGive>($"SELECT * FROM DBO.[RegisterGive] WHERE PlatformType={Type}");
        }
        /// <summary>
        /// 保存注册赠送（新增、修改）通用
        /// </summary>
        /// <param name="quest"></param>
        /// <returns></returns>
        public int SaveRegisterGiveInfo(RegisterGive quest)
        {
            var parm = new List<DbParameter>()
            {
                Database.MakeInParam("ScoreCount", quest.ScoreCount),
                Database.MakeInParam("DiamondCount", quest.DiamondCount),
                Database.MakeInParam("PlatformType", quest.PlatformType)
            };
            if (quest.ConfigID > 0) parm.Add(Database.MakeInParam("ConfigID", quest.ConfigID));

            const string sqlInsert =
                " INSERT DBO.[RegisterGive] (ScoreCount,DiamondCount,PlatformType) VALUES (@ScoreCount,@DiamondCount,@PlatformType) ";
            const string sqlUpdate =
                " UPDATE DBO.[RegisterGive] SET ScoreCount=@ScoreCount,DiamondCount=@DiamondCount,PlatformType=@PlatformType WHERE ConfigID=@ConfigID ";

            return Database.ExecuteNonQuery(CommandType.Text, quest.ConfigID > 0 ? sqlUpdate : sqlInsert, parm.ToArray());
        }

        /// <summary>
        /// 批量删注册赠送
        /// </summary>
        /// <param name="ids"></param>
        /// <returns></returns>
        public int DeleteRegisterGiveInfo(string ids)
        {
            return Database.ExecuteNonQuery($"DELETE DBO.[RegisterGive] WHERE ConfigID IN ({ids})");
        }
        #endregion

        #region 调试系统

        /// <summary>
        /// 保存大联盟调试账号配置（新增、修改）通用
        /// </summary>
        /// <param name="quest"></param>
        /// <returns></returns>
        public int SaveGroupAIParameter(GroupAIParameter model)
        {
            var parm = new List<DbParameter>()
            {
                Database.MakeInParam("GroupID", model.GroupID),
                Database.MakeInParam("KindID", model.KindID),
                Database.MakeInParam("AICount", model.AICount),
                Database.MakeInParam("DistributeTotalScore", model.DistributeTotalScore),
                Database.MakeInParam("PlayingAIRateMin", model.PlayingAIRateMin),
                Database.MakeInParam("PlayingAIRateMax", model.PlayingAIRateMax),
                Database.MakeInParam("DistirbuteSingleScore", model.DistirbuteSingleScore),
                Database.MakeInParam("DistirbuteScoreMinRatio", model.DistirbuteScoreMinRatio),
                Database.MakeInParam("DistirbuteScoreMaxRatio", model.DistirbuteScoreMaxRatio)
            };
            if (model.RecordID > 0) parm.Add(Database.MakeInParam("RecordID", model.RecordID));

            const string sqlInsert =
                " INSERT DBO.[GroupAIParameter] (GroupID,KindID,AICount,DistributeTotalScore,PlayingAIRateMin,PlayingAIRateMax,DistirbuteSingleScore,DistirbuteScoreMinRatio,DistirbuteScoreMaxRatio)" +
                " VALUES (@GroupID,@KindID,@AICount,@DistributeTotalScore,@PlayingAIRateMin,@PlayingAIRateMax,@DistirbuteSingleScore,@DistirbuteScoreMinRatio,@DistirbuteScoreMaxRatio); " +
                " SELECT SCOPE_IDENTITY(); ";

            const string sqlUpdate =
                " UPDATE DBO.[GroupAIParameter] SET GroupID=@GroupID,KindID=@KindID,AICount=@AICount,DistributeTotalScore=@DistributeTotalScore," +
                " PlayingAIRateMin=@PlayingAIRateMin,PlayingAIRateMax=@PlayingAIRateMax,DistirbuteSingleScore=@DistirbuteSingleScore,DistirbuteSingleScore=@DistirbuteSingleScore," +
                " DistirbuteScoreMaxRatio=@DistirbuteScoreMaxRatio WHERE RecordID=@RecordID ";
            if (model.RecordID > 0)
            {
                return Database.ExecuteNonQuery(CommandType.Text, model.RecordID > 0 ? sqlUpdate : sqlInsert, parm.ToArray());
            }
            else
            {
                object Result= Database.ExecuteScalar(CommandType.Text, sqlInsert, parm.ToArray());
                return Convert.ToInt32(Result);
            }
        }

        /// <summary>
        /// 获取大联盟调试账号配置
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="kindId">游戏ID</param>
        /// <returns></returns>
        public GroupAIParameter GetAIParameter(int groupId, int kindId)
        {
            return Database.ExecuteObject<GroupAIParameter>(
                $"SELECT * FROM DBO.[GroupAIParameter] WHERE GroupID={groupId} AND KindID={kindId}");
        }

        /// <summary>
        ///获取大联盟调试账号KindId
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="userId">玩家ID</param>
        /// <returns></returns>
        public int GetAIParameterKindId(int groupId, int userId)
        {
            string sqlQuery =
                $"SELECT KindID FROM DBO.[GroupAIMember] WHERE GroupID={groupId} AND UserID={userId}";
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery);
            return obj != null ? Convert.ToInt32(obj) : 0;
        }


        /// <summary>
        ///获取大联盟调试账号可分配积分
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        public long GetAIDistributionScore(int groupId)
        {
            string sqlQuery =
                $"SELECT ISNULL(SUM(DistributeTotalScore),0) FROM DBO.[GroupAIParameter] WHERE GroupID={groupId}";
            object obj = Database.ExecuteScalar(CommandType.Text, sqlQuery);
            return obj != null ? Convert.ToInt64(obj) : 0;
        }
        #endregion
    }
}