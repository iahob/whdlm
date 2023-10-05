using System.Collections.Generic;

using Game.Kernel;
using System.Data;
using Game.Entity.Accounts;
// ReSharper disable InconsistentNaming

namespace Game.IData
{
    /// <summary>
    ///  用户库数据层接口
    /// </summary>
    public interface IAccountsDataProvider//:IProvider
    {
        #region 系统配置
        /// <summary>
        /// 获取系统配置信息
        /// </summary>
        /// <param name="key">配置键值</param>
        /// <returns></returns>
        SystemStatusInfo GetSystemStatusInfo(string key);
        #endregion

        #region 用户信息
        /// <summary>
        /// 用户注册
        /// </summary>
        /// <param name="user"></param>
        /// <returns></returns>
        Message Register(UserInfo user, string parentAccount);
        /// <summary>
        /// 代理用户登录（微信）
        /// </summary>
        /// <param name="unionid">微信标识</param>
        /// <param name="ip">登录ip</param>
        /// <returns></returns>
        Message WXLogin(string unionid, string ip);
        /// <summary>
        /// 代理用户安全密码登录
        /// </summary>
        /// <param name="mobile"></param>
        /// <param name="pass"></param>
        /// <param name="ip"></param>
        /// <returns></returns>
        Message AgentMobileLogin(string mobile, string pass, string ip);
        /// <summary>
        /// 根据用户id获取用户信息
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        AccountsInfo GetAccountsInfoByUserID(int userid);
        /// <summary>
        /// 根据游戏id获取用户信息
        /// </summary>
        /// <param name="gameid">游戏id</param>
        /// <returns></returns>
        AccountsInfo GetAccountsInfoByGameID(int gameid);
        /// 根据账号获取用户信息
        /// </summary>
        /// <param name="Accounts">游戏账号</param>
        /// <returns></returns>
        AccountsInfo GetAccountsInfoByAccounts(string Accounts);
        
            /// <summary>
            /// 根据微信标识获取用户信息
            /// </summary>
            /// <param name="useruin">微信标识</param>
            /// <returns></returns>
            AccountsInfo GetAccountsInfoByUserUin(string useruin);

        /// <summary>
        /// 用户注册（微信）
        /// </summary>
        /// <param name="user">用户信息</param>
        /// <param name="registerType">注册类型</param>
        /// <param name="faceUrl"></param>
        /// <returns></returns>
        Message RegisterWX(UserInfo user, int registerType, string faceUrl);

        /// <summary>
        /// 获取用户最后登录地址
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns>LastLogonIP</returns>
        string GetUserIP(int userid);

        /// <summary>
        /// 用户IP和游戏ID绑定
        /// </summary>
        /// <param name="userIp"></param>
        /// <param name="gameId"></param>
        /// <returns></returns>
        int UserIPBind(string userIp, int gameId);
        #endregion

        #region 推广中心

        /// <summary>
        /// 推广中心首页数据
        /// </summary>
        /// <param name="userId"></param>
        /// <returns></returns>
        DataSet GetUserSpreadHomeDataSet(int userId);

        #endregion

        #region 手机登录信息
        /// <summary>
        /// 获取手机登录信息
        /// </summary>
        /// <param name="userid">登录用户标识</param>
        /// <returns></returns>
        DataSet GetMobileLoginLaterData(int userid);
        #endregion

        #region 自定义头像
        /// <summary>
        /// 获取自定义头像
        /// </summary>
        /// <param name="customId">自定义头像标识</param>
        /// <returns></returns>
        AccountsFace GetAccountsFace(int customId);
        #endregion

        #region 插入短信验证码数据
        /// <summary>
        /// 插入短信验证
        /// </summary>
        /// <param name="userid"></param>
        /// <param name="Mobile"></param>
        /// <param name="Code"></param>
        /// <returns></returns>
        int InsertSMSInfo(int userid, string Mobile, string Code);
        /// <summary>
        /// 获取验证码记录次数
        /// </summary>
        /// <param name="Mobile"></param>
        /// <returns></returns>
        string GetSMSLogCount(string Mobile);
        /// <summary>
        /// 绑定手机
        /// </summary>
        /// <param name="userid"></param>
        /// <param name="Mobile"></param>
        /// <param name="Code"></param>
        /// <param name="time"></param>
        /// <returns></returns>
        Message BindMobile(int userid, string Mobile, string Code, int time);

        /// <summary>
        /// 验证是否在规定时间内重复发送
        /// </summary>
        /// <param name="mobile"></param>
        /// <param name="minute"></param>
        /// <returns></returns>
        bool VaildSendOnTime(string mobile, int minute);

        #endregion
    }
}
