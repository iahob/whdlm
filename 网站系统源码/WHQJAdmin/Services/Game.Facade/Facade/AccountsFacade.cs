using System.Data;
using Game.Data.Factory;
using Game.Entity.Accounts;
using Game.IData;
using Game.Kernel;
using System.Collections.Generic;

// ReSharper disable once CheckNamespace
namespace Game.Facade
{
    /// <summary>
    /// 帐号库外观
    /// </summary>
    public class AccountsFacade
    {
        #region Fields

        // ReSharper disable once FieldCanBeMadeReadOnly.Local
        // ReSharper disable once InconsistentNaming
        private IAccountsDataProvider aideAccountsData;

        #endregion Fields

        #region 构造函数

        /// <summary>
        /// 构造函数
        /// </summary>
        public AccountsFacade()
        {
            aideAccountsData = ClassFactory.GetAccountsDataProvider();
        }

        #endregion 构造函数

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
            return aideAccountsData.GetList(tableName, where, order, pageIndex, pageSize, fields);
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
            return aideAccountsData.GetUserPageList(where, order, pageIndex, pageSize);
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
        public UserInfo GetUserInfo(int userid = 0, int gameid = 0, int agentid = 0, string account = "", string nickname = "")
        {
            return aideAccountsData.GetUserInfo(userid, gameid, agentid, account, nickname);
        }
        /// <summary>
        /// 根据用户标识获取用户信息
        /// </summary>
        /// <param name="userId">用户标识</param>
        /// <returns></returns>
        public AccountsInfo GetAccountInfoByUserId(int userId)
        {
            return aideAccountsData.GetAccountInfoByUserId(userId);
        }
        /// <summary>
        /// 根据游戏ID获取用户信息
        /// </summary>
        /// <param name="gameId">游戏ID</param>
        /// <returns></returns>
        public AccountsInfo GetAccountInfoByGameId(int gameId)
        {
            return aideAccountsData.GetAccountInfoByGameId(gameId);
        }
        /// <summary>
        /// 冻结解冻账号
        /// </summary>
        /// <param name="userlist">用户列表</param>
        /// <param name="nullity">状态值（0 正常 1 冻结）</param>
        /// <returns></returns>
        public int NullityAccountInfo(string userlist, int nullity)
        {
            return aideAccountsData.NullityAccountInfo(userlist, nullity);
        }
        /// <summary>
        /// 设置取消机器人
        /// </summary>
        /// <param name="userlist">用户列表</param>
        /// <param name="isAndroid">状态值（0 正常 1 机器人）</param>
        /// <returns></returns>
        public int AndroidAccountInfo(string userlist, int isAndroid)
        {
            return aideAccountsData.AndroidAccountInfo(userlist, isAndroid);
        }
        /// <summary>
        /// 获取用户基本信息通过用户帐号
        /// </summary>
        /// <param name="account">用户帐号</param>
        /// <returns>用户实体</returns>
        public AccountsInfo GetAccountInfoByAccount(string account)
        {
            return aideAccountsData.GetAccountInfoByAccount(account);
        }
        /// <summary>
        /// 修改用户信息
        /// </summary>
        /// <param name="accountsInfo">用户信息</param>
        /// <returns></returns>
        public int UpdateAccountsBaseInfo(AccountsInfo accountsInfo)
        {
            return aideAccountsData.UpdateAccountsBaseInfo(accountsInfo);
        }
        /// <summary>
        /// 随机获取10个靓号
        /// </summary>
        /// <returns></returns>
        public DataSet GetReserveIdentifierList()
        {
            return aideAccountsData.GetReserveIdentifierList();
        }

        /// <summary>
        /// 批量设置取消转账权限
        /// </summary>
        /// <param name="userlist">用户列表</param>
        /// <param name="userRight">权限值</param>
        /// <param name="calc"></param>
        /// <returns></returns>
        public int TransferPowerAccounts(string userlist, int userRight,string calc="|")
        {
            return aideAccountsData.TransferPowerAccounts(userlist, userRight, calc);
        }

        /// <summary>
        /// 全部设置取消转账权限
        /// </summary>
        /// <param name="userRight">权限值</param>
        /// <param name="calc"></param>
        /// <returns></returns>
        public int TransferPowerAccounts(int userRight, string calc = "|")
        {
            return aideAccountsData.TransferPowerAccounts(userRight, calc);
        }

        /// <summary>
        /// 添加超端管理员
        /// </summary>
        /// <param name="user">用户信息（用户名、登录密码）</param>
        /// <returns></returns>
        public Message InsertSuperUser(AccountsInfo user)
        {
            return aideAccountsData.InsertSuperUser(user);
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
            return aideAccountsData.GetConfineAddressByAddress(strAddress);
        }
        /// <summary>
        /// 新增限制地址信息
        /// </summary>
        /// <param name="address">地址信息</param>
        /// <returns></returns>
        public int InsertConfineAddress(ConfineAddress address)
        {
            return aideAccountsData.InsertConfineAddress(address);
        }
        /// <summary>
        ///  更新限制地址信息
        /// </summary>
        /// <param name="address">地址信息</param>
        /// <returns></returns>
        public int UpdateConfineAddress(ConfineAddress address)
        {
            return aideAccountsData.UpdateConfineAddress(address);
        }
        /// <summary>
        /// 删除限制地址信息
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public int DeleteConfineAddress(string where)
        {
            return aideAccountsData.DeleteConfineAddress(where);
        }
        /// <summary>
        /// 获取IP注册前100名
        /// </summary>
        /// <returns></returns>
        public DataSet GetIpRegisterTop100()
        {
            return aideAccountsData.GetIpRegisterTop100();
        }
        /// <summary>
        /// 批量插入限制IP
        /// </summary>
        /// <param name="ipList">ip列表</param>
        /// <returns></returns>
        public void BatchInsertConfineAddress(string ipList)
        {
            aideAccountsData.BatchInsertConfineAddress(ipList);
        }
        /// <summary>
        /// 获取限制机器码信息
        /// </summary>
        /// <param name="strSerial">限制机器码</param>
        /// <returns></returns>
        public ConfineMachine GetConfineMachineBySerial(string strSerial)
        {
            return aideAccountsData.GetConfineMachineBySerial(strSerial);
        }
        /// <summary>
        /// 新增限制机器码信息
        /// </summary>
        /// <param name="machine">机器码信息</param>
        /// <returns></returns>
        public int InsertConfineMachine(ConfineMachine machine)
        {
            return aideAccountsData.InsertConfineMachine(machine);
        }
        /// <summary>
        /// 修改限制机器码信息
        /// </summary>
        /// <param name="machine">机器码信息</param>
        /// <returns></returns>
        public int UpdateConfineMachine(ConfineMachine machine)
        {
            return aideAccountsData.UpdateConfineMachine(machine);
        }
        /// <summary>
        /// 删除限制机器码信息
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public int DeleteConfineMachine(string where)
        {
            return aideAccountsData.DeleteConfineMachine(where);
        }
        /// <summary>
        /// 机器码注册前100名
        /// </summary>
        /// <returns></returns>
        public DataSet GetMachineRegisterTop100()
        {
            return aideAccountsData.GetMachineRegisterTop100();
        }
        /// <summary>
        /// 批量插入限制机器码
        /// </summary>
        /// <param name="machineList">机器码列表</param>
        /// <returns></returns>
        public void BatchInsertConfineMachine(string machineList)
        {
            aideAccountsData.BatchInsertConfineMachine(machineList);
        }
        /// <summary>
        /// 获取用户头像地址
        /// </summary>
        /// <param name="customid">头像标识</param>
        /// <returns></returns>
        public string GetAccountsFaceById(int customid)
        {
            return aideAccountsData.GetAccountsFaceById(customid);
        }
        #endregion

        #region 系统配置
        /// <summary>
        /// 获取系统配置列表
        /// </summary>
        /// <returns></returns>
        public DataSet GetSystemStatusList()
        {
            return aideAccountsData.GetSystemStatusList();
        }
        /// <summary>
        /// 获取配置信息
        /// </summary>
        /// <param name="statusName">配置键值</param>
        /// <returns></returns>
        public SystemStatusInfo GetSystemStatusInfo(string statusName)
        {
            return aideAccountsData.GetSystemStatusInfo(statusName);
        }
        /// <summary>
        /// 修改配置信息
        /// </summary>
        /// <param name="statusinfo">配置信息</param>
        /// <returns></returns>
        public int UpdateSystemStatusInfo(SystemStatusInfo statusinfo)
        {
            return aideAccountsData.UpdateSystemStatusInfo(statusinfo);
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
            return aideAccountsData.GetAccountsUmengList(where);
        }
        /// <summary>
        /// 获取推送消息对象
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <returns></returns>
        public AccountsUmeng GetAccountsUmeng(int userid)
        {
            return aideAccountsData.GetAccountsUmeng(userid);
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
            return aideAccountsData.GetUserLoadGameList(uid, typeid);
        }
        /// <summary>
        /// 新增游戏配置
        /// </summary>
        /// <param name="game">配置信息</param>
        /// <returns></returns>
        public Message AddUserLoadGame(AccountsGame game)
        {
            return aideAccountsData.AddUserLoadGame(game); 
        }
        /// <summary>
        /// 删除游戏
        /// </summary>
        /// <param name="idList">id列表</param>
        /// <returns></returns>
        public int DeleteUserLoadGame(string idList)
        {
            return aideAccountsData.DeleteUserLoadGame(idList);
        }
        #endregion

        #region 注册赠送

        /// <summary>
        /// 获取注册赠送实体 by id
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        public RegisterGive GetRegisterGiveInfo(int id)
        {
            return aideAccountsData.GetRegisterGiveInfo(id);
        }

        /// <summary>
        /// 获取注册赠送实体 by id
        /// </summary>
        /// <param name="Type"></param>
        /// <returns></returns>
        public RegisterGive GetRegisterGiveInfoByPlatformType(int Type)
        {
            return aideAccountsData.GetRegisterGiveInfoByPlatformType(Type);
        }

        /// <summary>
        /// 保存注册赠送（新增、修改）通用
        /// </summary>
        /// <param name="quest"></param>
        /// <returns></returns>
        public int SaveRegisterGiveInfo(RegisterGive quest)
        {
            return aideAccountsData.SaveRegisterGiveInfo(quest);
        }

        /// <summary>
        /// 批量删注册赠送
        /// </summary>
        /// <param name="ids"></param>
        /// <returns></returns>
        public int DeleteRegisterGiveInfo(string ids)
        {
            return aideAccountsData.DeleteRegisterGiveInfo(ids);
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
            return aideAccountsData.SaveGroupAIParameter(model);
        }


        /// <summary>
        /// 获取大联盟调试账号配置
        /// </summary>
        /// <param name="groupId"></param>
        /// <param name="kind"></param>
        /// <returns></returns>
        public GroupAIParameter GetAIParameter(int groupId, int kind)
        {
            return aideAccountsData.GetAIParameter(groupId, kind);
        }

        // <summary>
        ///获取大联盟调试账号KindId
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="userId">玩家ID</param>
        /// <returns></returns>
        public int GetAIParameterKindId(int groupId, int userId)
        {
            return aideAccountsData.GetAIParameterKindId(groupId, userId);
        }

        /// <summary>
        ///获取大联盟调试账号可分配积分
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        public long GetAIDistributionScore(int groupId)
        {
            return aideAccountsData.GetAIDistributionScore(groupId);
        }

        #endregion
    }
}