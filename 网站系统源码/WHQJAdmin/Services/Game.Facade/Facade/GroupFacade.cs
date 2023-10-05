using System.Collections;
using System.Collections.Generic;
using System.Data;
using Game.Data.Factory;
using Game.Entity.Accounts;
using Game.Entity.Group;
using Game.IData;
using Game.Kernel;

// ReSharper disable once CheckNamespace
namespace Game.Facade
{
    /// <summary>
    /// 代理外观
    /// </summary>
    public class GroupFacade
    {
        #region Fields

        // ReSharper disable once FieldCanBeMadeReadOnly.Local
        // ReSharper disable once InconsistentNaming
        private IGroupDataProvider groupData;

        #endregion Fields

        #region 构造函数

        /// <summary>
        /// 构造函数
        /// </summary>
        public GroupFacade()
        {
            groupData = ClassFactory.GetIGroupDataProvider();
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
            return groupData.GetList(tableName, where, order, pageIndex, pageSize, fields);
        }
        /// <summary>
        /// 获取汇总统计
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <param name="field">汇总字段</param>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        public long GetTotal(string tableName, string field, string where) {
            return groupData.GetTotal(tableName, field, where);
        }
        #endregion 公用分页

        #region 系统配置
        /// <summary>
        /// 获取配置信息
        /// </summary>
        /// <param name="optionName">配置键值</param>
        /// <returns></returns>
        public IMGroupOption GetGroupOption(string optionName)
        {
            return groupData.GetGroupOption(optionName);
        }
        /// <summary>
        /// 修改配置信息
        /// </summary>
        /// <param name="optionInfo">配置信息</param>
        /// <returns></returns>
        public int UpdateGroupOption(IMGroupOption optionInfo)
        {
            return groupData.UpdateGroupOption(optionInfo);
        }
        #endregion

        #region 大联盟信息
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupList(string where, string order, int pageIndex, int pageSize)
        {
            return groupData.GetPageGroupList(where, order, pageIndex, pageSize);
        }

        

        /// <summary>
        /// 获取大联盟玩家勋章流水记录
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        public PagerSet GetPageGroupUserScoreChangeList(string where, string order, int pageIndex, int pageSize)
        {
            return groupData.GetPageGroupUserScoreChangeList(where, order, pageIndex, pageSize);
        }

        /// <summary>
        /// 获取大联盟玩家勋章提取记录
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        public PagerSet GetPageGroupBoxScoreList(string where, string order, int pageIndex, int pageSize)
        {
            return groupData.GetPageGroupBoxScoreList(where, order, pageIndex, pageSize);
        }

        /// <summary>
        /// 大联盟战绩分页
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        public PagerSet GetPageGroupDrawList(string where, string order, int pageIndex, int pageSize)
        {
            return groupData.GetPageGroupDrawList(where, order, pageIndex, pageSize);
        }
        /// <summary>
        /// 获取大联盟亲友列表
        /// </summary>
        /// <param name="groupid">大联盟标识</param>
        /// <returns></returns>
        public DataSet GetGroupMemberList(long groupid)
        {
            return groupData.GetGroupMemberList(groupid);
        }
        /// <summary>
        /// 获取大联盟信息
        /// </summary>
        /// <param name="groupId">群组编号</param>
        /// <returns></returns>
        public IMGroupProperty GetGroupInfo(long groupId)
        {
            return groupData.GetGroupInfo(groupId);
        }

        /// <summary>
        /// 批量冻结、解冻大联盟
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：解冻 1：冻结</param>
        /// <returns></returns>
        public int NullityGroup(string ids, byte nullity)
        {
            return groupData.NullityGroup(ids, nullity);
        }

        /// <summary>
        /// 批量开启、关闭大联盟模式
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：关闭 1：开始</param>
        /// <returns></returns>
        public int CanUseMedalGroup(string ids, byte nullity)
        {
            return groupData.CanUseMedalGroup(ids, nullity);
        }

        /// <summary>
        /// 批量强制解散
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <returns></returns>
        public int DeleteGroup(string ids)
        {
            return groupData.DeleteGroup(ids);
        }

        /// <summary>
        /// 移交会长
        /// </summary>
        /// <param name="info">群组信息</param>
        /// <returns></returns>
        public int UpdateGroup(IMGroupProperty info)
        {
            return groupData.UpdateGroup(info);
        }

        /// <summary>
        /// 获取大联盟在线人数（金币房间与比赛房间）
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        public int GetGroupMemberOnline(long groupId)
        {
            return groupData.GetGroupMemberOnline(groupId);
        }

        /// <summary>
        /// 获取大联盟钻石数量
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        public long GetGroupWealth(long groupId)
        {
            return groupData.GetGroupWealth(groupId);
        }

        /// <summary>
        /// 获取大联盟约战统计 
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="userId">用户标识（两个维度：userId为0是为大联盟统计，大于0时为该用户在该大联盟的统计）</param>
        /// <returns></returns>
        public StreamGroupBalance GetGroupBattleRoomStream(long groupId, int userId = 0)
        {
            return groupData.GetGroupBattleRoomStream(groupId, userId);
        }

        /// <summary>
        /// 获取大联盟约战当前房间数
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="roomStatus">房间状态</param>
        /// <returns></returns>
        public int GetGroupBattleRoomCurrent(long groupId, int roomStatus = -1)
        {
            return groupData.GetGroupBattleRoomCurrent(groupId, roomStatus);
        }

        /// <summary>
        /// 后台赠送勋章
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="medal">勋章数</param>
        /// <param name="clientIp">操作地址</param>
        /// <param name="masterId">操作管理员</param>
        /// <returns></returns>
        public Message AddMedal(int groupId, int medal, string clientIp, int masterId)
        {
            return groupData.AddMedal(groupId, medal, clientIp, masterId);
        }

        /// <summary>
        /// 检查是否可以批量修改
        /// </summary>
        /// <param name="ids"></param>
        /// <param name="point"></param>
        /// <returns></returns>
        public bool CheckAwrdPoint(string ids, int point)
        {
            return groupData.CheckAwrdPoint(ids, point);
        }

        /// <summary>
        /// 批量修改盟主点位
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：解冻 1：冻结</param>
        /// <returns></returns>
        public int ModifyAwardPoint(string ids, int point)
        {
            return groupData.ModifyAwardPoint(ids, point);
        }

        /// <summary>
        /// 检查是否可以批量修改最大人数
        /// </summary>
        /// <param name="ids"></param>
        /// <param name="point"></param>
        /// <returns></returns>
        public bool CheckMaxMemberCount(string ids, int count)
        {
            return groupData.CheckMaxMemberCount(ids, count);
        }

        /// <summary>
        /// 批量修改盟主点位
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="count">人数</param>
        /// <returns></returns>
        public int ModifyMaxMemberCount(string ids, int count)
        {
            return groupData.ModifyMaxMemberCount(ids, count);
        }

        #endregion

        #region 白名单管理
        /// <summary>
        /// 获取白名单实体 by userId
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        public IMGroupwhiteList GetIMGroupwhiteInfo(int userId)
        {
            return groupData.GetIMGroupwhiteInfo(userId);
        }

        /// <summary>
        /// 新增白名单
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public int InsertIMGroupwhiteInfo(IMGroupwhiteList model)
        {
            return groupData.InsertIMGroupwhiteInfo(model);
        }

        /// <summary>
        /// 批量删白名单
        /// </summary>
        /// <param name="ids"></param>
        /// <returns></returns>
        public int DeleteIMGroupwhiteInfo(string ids)
        {
            return groupData.DeleteIMGroupwhiteInfo(ids);
        }
        #endregion

        #region 总控系统

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupDebugList(string where, string order, int pageIndex, int pageSize)
        {
            return groupData.GetPageGroupDebugList(where, order, pageIndex, pageSize);
        }

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupDebugUserList(string where, string order, int pageIndex, int pageSize)
        {
            return groupData.GetPageGroupDebugUserList(where, order, pageIndex, pageSize);
        }

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupGameControlList(string where, string order, int pageIndex, int pageSize)
        {
            return groupData.GetPageGroupGameControlList(where, order, pageIndex, pageSize);
        }

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        public PagerSet GetPageGroupGameControlSubList(string where, string order, int pageIndex, int pageSize)
        {
            return groupData.GetPageGroupGameControlSubList(where, order, pageIndex, pageSize);
        }

        /// <summary>
        /// 获取总控增减账号联盟信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        public DataTable GetGroupDebugInfo(int groupId)
        {
            return groupData.GetGroupDebugInfo(groupId);
        }

        /// <summary>
        /// 获取总控游戏控制联盟信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        public DataTable GetGroupGameControlInfo(int groupId)
        {
            return groupData.GetGroupGameControlInfo(groupId);
        }

        /// <summary>
        /// 获取当前共享库存配置
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="kindId">游戏ID</param>
        /// <param name="playmode">模式 0.积分 1.金币 </param>
        /// <returns></returns>
        public IMGroupShareStorageProperty GetShareStorage(int groupId, int kindId, int playmode)
        {
            return groupData.GetShareStorage(groupId, kindId, playmode);
        }

        /// <summary>
        /// 获取大联盟财富
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        public IMGroupWealth GetGroupWealth(int groupId)
        {
            return groupData.GetGroupWealth(groupId);
        }

        #endregion
    }
}