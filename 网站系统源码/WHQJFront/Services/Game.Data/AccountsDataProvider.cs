using System;
using System.Collections.Generic;
using Game.Kernel;
using Game.IData;
using System.Data.Common;
using System.Data;
using Game.Entity.Accounts;

namespace Game.Data
{
    /// <summary>
    /// 用户数据访问层
    /// </summary>
    public sealed class AccountsDataProvider : BaseDataProvider, IAccountsDataProvider
    {
        #region Fields

        #endregion

        #region 构造方法

        public AccountsDataProvider(string connString)
            : base(connString)
        {
        }

        #endregion

        #region 系统配置

        /// <summary>
        /// 获取系统配置信息
        /// </summary>
        /// <param name="key">配置键值</param>
        /// <returns></returns>
        public SystemStatusInfo GetSystemStatusInfo(string key)
        {
            const string sqlQuery =
                @"SELECT StatusValue FROM SystemStatusInfo WITH(NOLOCK) WHERE StatusName = @StatusName";
            List<DbParameter> parms = new List<DbParameter> {Database.MakeInParam("StatusName", key)};
            return Database.ExecuteObject<SystemStatusInfo>(sqlQuery, parms);
        }

        #endregion

        #region 用户信息

        /// <summary>
        /// 用户注册
        /// </summary>
        /// <param name="user"></param>
        /// <returns></returns>
        public Message Register(UserInfo user, string parentAccount)
        {
            List<DbParameter> prams = new List<DbParameter>();
            prams.Add(Database.MakeInParam("strSpreader", parentAccount));
            prams.Add(Database.MakeInParam("strAccounts", user.Accounts));
            prams.Add(Database.MakeInParam("strNickname", user.NickName));

            prams.Add(Database.MakeInParam("strLogonPass", user.LogonPass));
            prams.Add(Database.MakeInParam("strInsurePass", user.InsurePass));
            prams.Add(Database.MakeInParam("strDynamicPass", user.DynamicPass));

            prams.Add(Database.MakeInParam("strCompellation", user.Compellation));
            prams.Add(Database.MakeInParam("strPassPortID", user.PassPortID));

            prams.Add(Database.MakeInParam("dwFaceID", user.FaceID));
            prams.Add(Database.MakeInParam("dwGender", user.Gender));

            prams.Add(Database.MakeInParam("strClientIP", user.RegisterIP));
            prams.Add(Database.MakeOutParam("strErrorDescribe", typeof(string), 127));

            return MessageHelper.GetMessageForObject<UserInfo>(Database, "NET_PW_RegisterAccounts", prams);
        }

        /// <summary>
        /// 代理用户登录（微信）
        /// </summary>
        /// <param name="unionid">微信标识</param>
        /// <param name="ip">登录ip</param>
        /// <returns></returns>
        public Message WXLogin(string unionid, string ip)
        {
            List<DbParameter> prams =
                new List<DbParameter>
                {
                    Database.MakeInParam("strUserUin", unionid),
                    Database.MakeInParam("strClientIP", ip),
                    Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
                };

            return MessageHelper.GetMessageForObject<UserInfo>(Database, "NET_PW_AgentAccountsLogin", prams);
        }

        /// <summary>
        /// 代理用户安全密码登录
        /// </summary>
        /// <param name="mobile"></param>
        /// <param name="pass"></param>
        /// <param name="ip"></param>
        /// <returns></returns>
        public Message AgentMobileLogin(string mobile, string pass, string ip)
        {
            List<DbParameter> prams =
                new List<DbParameter>
                {
                    Database.MakeInParam("strMobile", mobile),
                    Database.MakeInParam("strPassword", pass),
                    Database.MakeInParam("strClientIP", ip),
                    Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
                };

            return MessageHelper.GetMessageForObject<UserInfo>(Database, "NET_PW_AgentAccountsLogin_MP", prams);
        }

        /// <summary>
        /// 根据用户id获取用户信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        public AccountsInfo GetAccountsInfoByUserID(int userid)
        {
            const string sqlQuery =
                @"SELECT UserID,GameID,SpreaderID,NickName,PassPortID,Compellation,FaceID,CustomID,RegisterOrigin,AgentID,RegisterIP,LastLogonIP,UnderWrite,PlaceName,UserUin,AgentID,RegisterMobile FROM AccountsInfo WITH(NOLOCK) WHERE UserID = @UserID";
            List<DbParameter> parms = new List<DbParameter> {Database.MakeInParam("UserID", userid)};
            return Database.ExecuteObject<AccountsInfo>(sqlQuery, parms);
        }

        /// <summary>
        /// 根据游戏id获取用户信息
        /// </summary>
        /// <param name="gameid">游戏id</param>
        /// <returns></returns>
        public AccountsInfo GetAccountsInfoByGameID(int gameid)
        {
            const string sqlQuery =
                @"SELECT UserID FROM AccountsInfo WITH(NOLOCK) WHERE GameID = @GameID";
            List<DbParameter> parms = new List<DbParameter> {Database.MakeInParam("GameID", gameid)};
            object result = Database.ExecuteScalar(CommandType.Text, sqlQuery, parms.ToArray());
            return result != null ? GetAccountsInfoByUserID(Convert.ToInt32(result)) : null;
        }

        /// <summary>
        /// 根据账号获取用户信息
        /// </summary>
        /// <param name="Accounts">游戏账号</param>
        /// <returns></returns>
        public AccountsInfo GetAccountsInfoByAccounts(string Accounts)
        {
            const string sqlQuery =
                @"SELECT UserID FROM AccountsInfo WITH(NOLOCK) WHERE Accounts = @Accounts";
            List<DbParameter> parms = new List<DbParameter> { Database.MakeInParam("Accounts", Accounts) };
            object result = Database.ExecuteScalar(CommandType.Text, sqlQuery, parms.ToArray());
            return result != null ? GetAccountsInfoByUserID(Convert.ToInt32(result)) : null;
        }

        /// <summary>
        /// 根据微信标识获取用户信息
        /// </summary>
        /// <param name="useruin">微信标识</param>
        /// <returns></returns>
        public AccountsInfo GetAccountsInfoByUserUin(string useruin)
        {
            const string sqlQuery =
                @"SELECT UserID FROM AccountsInfo WITH(NOLOCK) WHERE UserUin = @UserUin";
            List<DbParameter> parms = new List<DbParameter> {Database.MakeInParam("UserUin", useruin)};
            object result = Database.ExecuteScalar(CommandType.Text, sqlQuery, parms.ToArray());
            return result != null ? GetAccountsInfoByUserID(Convert.ToInt32(result)) : null;
        }

        /// <summary>
        /// 用户注册（微信）
        /// </summary>
        /// <param name="user">用户信息</param>
        /// <param name="registerType">注册类型</param>
        /// <param name="faceUrl"></param>
        /// <returns></returns>
        public Message RegisterWX(UserInfo user, int registerType, string faceUrl)
        {
            List<DbParameter> prams = new List<DbParameter>
            {
                Database.MakeInParam("strUserUin", user.UserUin),
                Database.MakeInParam("strNickName", user.NickName),
                Database.MakeInParam("cbGender", user.Gender),
                Database.MakeInParam("strFaceUrl", faceUrl),
                Database.MakeInParam("strSpreader", user.GameID),
                Database.MakeInParam("strClientIP", user.RegisterIP),
                Database.MakeInParam("dwRegisterOrigin", registerType),
                Database.MakeOutParam("strErrorDescribe", typeof(string), 127)
            };

            return MessageHelper.GetMessage(Database, "NET_PW_RegisterAccountsWX", prams);
        }

        /// <summary>
        /// 获取用户最后登录地址
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns>LastLogonIP</returns>
        public string GetUserIP(int userid)
        {
            AccountsInfo userInfo = GetAccountsInfoByUserID(userid);
            return userInfo?.LastLogonIP ?? "";
        }

        /// <summary>
        /// 用户IP和游戏ID绑定
        /// </summary>
        /// <param name="userIp"></param>
        /// <param name="gameId"></param>
        /// <returns></returns>
        public int UserIPBind(string userIp, int gameId)
        {
            string sqlQuery = "DELETE FROM dbo.BindIP WHERE UserIP=@UserIP;INSERT INTO dbo.BindIP( UserIP, GameID, CollectDate )VALUES ( @UserIP,@GameID,GETDATE())";
            List<DbParameter> prams = new List<DbParameter>
            {
                Database.MakeInParam("UserIP", userIp),
                Database.MakeInParam("GameID",gameId)
            };
            return Database.ExecuteNonQuery(CommandType.Text, sqlQuery, prams.ToArray());
        }

        #endregion

        #region 推广中心

        /// <summary>
        /// 推广中心首页数据
        /// </summary>
        /// <param name="userId"></param>
        /// <returns></returns>
        public DataSet GetUserSpreadHomeDataSet(int userId)
        {
            var param = new List<DbParameter>()
            {
                Database.MakeInParam("dwUserID", userId)
            };
            return Database.ExecuteDataset(CommandType.StoredProcedure, "NET_PW_UserSpreadHome", param.ToArray());
        }

        #endregion

        #region 手机登录信息

        /// <summary>
        /// 获取手机登录信息
        /// </summary>
        /// <param name="userid">登录用户标识</param>
        /// <returns></returns>
        public DataSet GetMobileLoginLaterData(int userid)
        {
            List<DbParameter> prams = new List<DbParameter> {Database.MakeInParam("dwUserID", userid)};

            return Database.ExecuteDataset(CommandType.StoredProcedure, "NET_PW_GetMobileLoginLater", prams.ToArray());
        }

        #endregion

        #region 自定义头像

        /// <summary>
        /// 获取自定义头像
        /// </summary>
        /// <param name="customId">自定义头像标识</param>
        /// <returns></returns>
        public AccountsFace GetAccountsFace(int customId)
        {
            const string sqlQuery = "SELECT UserID,FaceUrl FROM AccountsFace WITH(NOLOCK) WHERE ID=@ID";

            List<DbParameter> prams = new List<DbParameter> {Database.MakeInParam("ID", customId)};

            return Database.ExecuteObject<AccountsFace>(sqlQuery, prams);
        }

        #endregion

        #region 插入短信验证码数据
        /// <summary>
        /// 插入短信验证
        /// </summary>
        /// <param name="userid"></param>
        /// <param name="Mobile"></param>
        /// <param name="Code"></param>
        /// <returns></returns>
        public int InsertSMSInfo(int userid, string Mobile, string Code)
        {
            string sql = string.Format("delete CheckCode where PhoneNum='{0}'; insert into CheckCode(PhoneNum,CheckCode,CollectTime) values('{0}','{1}',getdate());insert into SMSLog(Mobile,date) values('{0}',getdate());", Mobile, Code);
            return Database.ExecuteNonQuery(sql);
        }
        /// <summary>
        /// 获取验证码记录次数
        /// </summary>
        /// <param name="Mobile"></param>
        /// <returns></returns>
        public string GetSMSLogCount(string Mobile)
        {
            string sql = string.Format(" select  isnull(count(id),0) from SMSLog where Mobile='{0}' and date>='{1} 00:00:00' and date<'{1} 23:59:59'", Mobile,DateTime.Now.ToShortDateString());
            return Database.ExecuteScalarToStr(CommandType.Text,sql);
        }



        /// <summary>
        /// 绑定手机
        /// </summary>
        /// <param name="userid"></param>
        /// <param name="Mobile"></param>
        /// <param name="Code"></param>
        /// <param name="time"></param>
        /// <returns></returns>
        public Message BindMobile(int userid, string Mobile, string Code, int time)
        {

            List<DbParameter> prams = new List<DbParameter>();
            prams.Add(Database.MakeInParam("Userid", userid));
            prams.Add(Database.MakeInParam("Molible", Mobile));
            prams.Add(Database.MakeInParam("Code", Code));
            prams.Add(Database.MakeInParam("time", time));
            prams.Add(Database.MakeOutParam("strErrorDescribe", typeof(string), 127));
            return MessageHelper.GetMessage(Database, "NET_PW_BindMolible", prams);
            //return Database.RunProcObject<DataSet>("NET_PW_BindMolible", prams);
        }

        /// <summary>
        /// 验证是否在规定时间内重复发送
        /// </summary>
        /// <param name="mobile"></param>
        /// <param name="minute"></param>
        /// <returns></returns>
        public bool VaildSendOnTime(string mobile, int minute)
        {
            string sql = "SELECT TOP 1 * FROM dbo.CheckCode WHERE PhoneNum=@Mobile AND GETDATE()<DATEADD(MINUTE,@Minute,CollectTime) ORDER BY CollectTime DESC ";

            DbParameter[] prams =new DbParameter[]
            {
                Database.MakeInParam("Mobile", mobile),
                Database.MakeInParam("Minute",minute)
            };
            DataSet ds= Database.ExecuteDataset(CommandType.Text, sql, prams);
            return ds.Tables[0].Rows.Count <= 0;
        }

        #endregion
    }
}
