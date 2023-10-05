using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using Game.Entity.Accounts;
using Game.Entity.Agent;
using Game.Entity.Group;
using Game.Kernel;

namespace Game.IData
{
    /// <summary>
    /// 代理库数据层接口
    /// </summary>
    public interface IGroupDataProvider
    {
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
        PagerSet GetList(string tableName, string where, string order, int pageIndex, int pageSize, string[] fields = null);
        /// <summary>
        /// 获取汇总统计
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <param name="field">汇总字段</param>
        /// <param name="where">查询条件</param>
        /// <returns></returns>
        long GetTotal(string tableName, string field, string where);
        #endregion 公用分页

        #region 系统配置
        /// <summary>
        /// 获取配置信息
        /// </summary>
        /// <param name="optionName">配置键值</param>
        /// <returns></returns>
        IMGroupOption GetGroupOption(string optionName);

        /// <summary>
        /// 修改配置信息
        /// </summary>
        /// <param name="optionInfo">配置信息</param>
        /// <returns></returns>
        int UpdateGroupOption(IMGroupOption optionInfo);

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
        PagerSet GetPageGroupList(string where, string order, int pageIndex, int pageSize);

        

        /// <summary>
        /// 获取大联盟玩家勋章流水记录
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        PagerSet GetPageGroupUserScoreChangeList(string where, string order, int pageIndex, int pageSize);

        /// <summary>
        /// 获取大联盟玩家勋章提取记录
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        PagerSet GetPageGroupBoxScoreList(string where, string order, int pageIndex, int pageSize);

        /// <summary>
        /// 大联盟战绩分页
        /// </summary>
        /// <param name="where"></param>
        /// <param name="order"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        PagerSet GetPageGroupDrawList(string where, string order, int pageIndex, int pageSize);

        /// <summary>
        /// 获取大联盟亲友列表
        /// </summary>
        /// <param name="groupid">大联盟标识</param>
        /// <returns></returns>
        DataSet GetGroupMemberList(long groupid);
        /// <summary>
        /// 获取大联盟信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        IMGroupProperty GetGroupInfo(long groupId);

        /// <summary>
        /// 批量冻结、解冻大联盟
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：解冻 1：冻结</param>
        /// <returns></returns>
        int NullityGroup(string ids, byte nullity);

        /// <summary>
        /// 批量开启、关闭大联盟模式
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：关闭 1：开始</param>
        /// <returns></returns>
        int CanUseMedalGroup(string ids, byte nullity);


        /// <summary>
        /// 批量强制解散
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <returns></returns>
        int DeleteGroup(string ids);

        /// <summary>
        /// 移交会长
        /// </summary>
        /// <param name="info">群组信息</param>
        /// <returns></returns>
        int UpdateGroup(IMGroupProperty info);

        /// <summary>
        /// 获取大联盟在线人数（金币房间与比赛房间）
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        int GetGroupMemberOnline(long groupId);

        /// <summary>
        /// 获取大联盟钻石数量
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        long GetGroupWealth(long groupId);

        /// <summary>
        /// 获取大联盟约战统计 
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="userId">用户标识（两个维度：userId为0是为大联盟统计，大于0时为该用户在该大联盟的统计）</param>
        /// <returns></returns>
        StreamGroupBalance GetGroupBattleRoomStream(long groupId, int userId = 0);

        /// <summary>
        /// 获取大联盟约战当前房间数
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="roomStatus">房间状态</param>
        /// <returns></returns>
        int GetGroupBattleRoomCurrent(long groupId, int roomStatus = -1);

        /// <summary>
        /// 后台赠送勋章
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="medal">勋章数</param>
        /// <param name="clientIp">操作地址</param>
        /// <param name="masterId">操作管理员</param>
        /// <returns></returns>
        Message AddMedal(int groupId, int medal, string clientIp, int masterId);


        /// <summary>
        /// 检查是否可以批量修改
        /// </summary>
        /// <param name="ids"></param>
        /// <param name="point"></param>
        /// <returns></returns>
        bool CheckAwrdPoint(string ids, int point);

        /// <summary>
        /// 批量修改盟主点位
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="nullity">0：解冻 1：冻结</param>
        /// <returns></returns>
        int ModifyAwardPoint(string ids, int point);

        /// <summary>
        /// 检查是否可以批量修改最大人数
        /// </summary>
        /// <param name="ids"></param>
        /// <param name="point"></param>
        /// <returns></returns>
        bool CheckMaxMemberCount(string ids, int count);

        /// <summary>
        /// 批量修改盟主点位
        /// </summary>
        /// <param name="ids">大联盟列表 （分隔符：,）</param>
        /// <param name="count">人数</param>
        /// <returns></returns>
        int ModifyMaxMemberCount(string ids, int count);


        #endregion

        #region 白名单管理

        /// <summary>
        /// 获取白名单实体 by userId
        /// </summary>
        /// <param name="Id"></param>
        /// <returns></returns>
        IMGroupwhiteList GetIMGroupwhiteInfo(int userId);

        /// <summary>
        /// 新增白名单
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        int InsertIMGroupwhiteInfo(IMGroupwhiteList model);



        /// <summary>
        /// 批量删白名单
        /// </summary>
        /// <param name="ids"></param>
        /// <returns></returns>
        int DeleteIMGroupwhiteInfo(string ids);

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
        PagerSet GetPageGroupDebugList(string where, string order, int pageIndex, int pageSize);

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageGroupDebugUserList(string where, string order, int pageIndex, int pageSize);

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageGroupGameControlList(string where, string order, int pageIndex, int pageSize);

        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="where">查询条件</param>
        /// <param name="order">排序</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <returns></returns>
        PagerSet GetPageGroupGameControlSubList(string where, string order, int pageIndex, int pageSize);

        /// <summary>
        /// 获取总控增减账号联盟信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        DataTable GetGroupDebugInfo(int groupId);

        /// <summary>
        /// 获取总控游戏控制联盟信息
        /// </summary>
        /// <param name="groupId"></param>
        /// <returns></returns>
        DataTable GetGroupGameControlInfo(int groupId);


        /// <summary>
        /// 获取当前共享库存配置
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <param name="kindId">游戏ID</param>
        /// <param name="playmode">模式 0.积分 1.金币 </param>
        /// <returns></returns>
        IMGroupShareStorageProperty GetShareStorage(int groupId, int kindId, int playmode);

        /// <summary>
        /// 获取大联盟财富
        /// </summary>
        /// <param name="groupId">大联盟ID</param>
        /// <returns></returns>
        IMGroupWealth GetGroupWealth(int groupId);

        #endregion
    }
}