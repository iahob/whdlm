using System.Collections.Generic;
using Game.Data.Factory;
using Game.IData;
using Game.Kernel;
using System.Data;
using System.Linq;
using Game.Entity.Accounts;
// ReSharper disable InconsistentNaming

// ReSharper disable once CheckNamespace
namespace Game.Facade
{
    /// <summary>
    /// 用户外观
    /// </summary>
    public class AccountsFacade
    {
        #region Fields

        private readonly IAccountsDataProvider accountsData;

        #endregion

        #region 构造函数

        /// <summary>
        /// 构造函数
        /// </summary>
        public AccountsFacade()
        {
            accountsData = ClassFactory.GetIAccountsDataProvider();
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
            return accountsData.GetSystemStatusInfo(key);
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
            return accountsData.Register(user, parentAccount);
        }
        /// <summary>
        /// 代理用户登录（微信）
        /// </summary>
        /// <param name="unionid">微信标识</param>
        /// <param name="ip">登录ip</param>
        /// <returns></returns>
        public Message WXLogin(string unionid, string ip)
        {
            return accountsData.WXLogin(unionid, ip);
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
            return accountsData.AgentMobileLogin(mobile, pass, ip);
        }
        /// <summary>
        /// 根据用户id获取用户信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        public AccountsInfo GetAccountsInfoByUserID(int userid)
        {
            return accountsData.GetAccountsInfoByUserID(userid);
        }
        /// <summary>
        /// 根据用户id获取游戏id
        /// </summary>
        /// <param name="userid"></param>
        /// <returns></returns>
        public int GetGameIDByUserID(int userid)
        {
            return GetAccountsInfoByUserID(userid)?.GameID??0;
        }
        /// <summary>
        /// 根据账号获取用户信息
        /// </summary>
        /// <param name="Accounts">游戏账号</param>
        /// <returns></returns>
        public AccountsInfo GetAccountsInfoByAccounts(string Accounts)
        {
            return accountsData.GetAccountsInfoByAccounts(Accounts);
        }
        /// <summary>
        /// 根据用户id获取用户昵称
        /// </summary>
        /// <param name="userid"></param>
        /// <returns></returns>
        public string GetNickNameByUserID(int userid)
        {
            return GetAccountsInfoByUserID(userid)?.NickName ?? "";
        }
        /// <summary>
        /// 根据游戏id获取用户信息
        /// </summary>
        /// <param name="gameid">游戏id</param>
        /// <returns></returns>
        public AccountsInfo GetAccountsInfoByGameID(int gameid)
        {
            return accountsData.GetAccountsInfoByGameID(gameid);
        }
        /// <summary>
        /// 根据微信标识获取用户信息
        /// </summary>
        /// <param name="useruin">微信标识</param>
        /// <returns></returns>
        public AccountsInfo GetAccountsInfoByUserUin(string useruin)
        {
            return accountsData.GetAccountsInfoByUserUin(useruin);
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
            return accountsData.RegisterWX(user, registerType, faceUrl);
        }

        /// <summary>
        /// 获取用户最后登录地址
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns>LastLogonIP</returns>
        public string GetUserIP(int userid)
        {
            return accountsData.GetUserIP(userid);
        }
        /// <summary>
        /// 用户IP和游戏ID绑定
        /// </summary>
        /// <param name="userIp"></param>
        /// <param name="gameId"></param>
        /// <returns></returns>
        public int UserIPBind(string userIp, int gameId)
        {
            return accountsData.UserIPBind(userIp, gameId);
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
            return accountsData.GetUserSpreadHomeDataSet(userId);
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
            return accountsData.GetMobileLoginLaterData(userid);
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
            return accountsData.GetAccountsFace(customId);
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

            return accountsData.InsertSMSInfo(userid, Mobile, Code);
        }
        /// <summary>
        /// 获取验证码记录次数
        /// </summary>
        /// <param name="Mobile"></param>
        /// <returns></returns>
        public string GetSMSLogCount(string Mobile)
        {
            return accountsData.GetSMSLogCount(Mobile);
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

            return accountsData.BindMobile(userid, Mobile, Code, time);
        }

        /// <summary>
        /// 验证是否在规定时间内重复发送
        /// </summary>
        /// <param name="mobile"></param>
        /// <param name="minute"></param>
        /// <returns></returns>
        public bool VaildSendOnTime(string mobile, int minute)
        {
            return accountsData.VaildSendOnTime(mobile, minute);
        }
        #endregion
    }
}
