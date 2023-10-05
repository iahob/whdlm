using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Game.Data.Factory;
using Game.IData;
using Game.Kernel;
using Game.Utils;
using System.Data;
using Game.Entity.Platform;

namespace Game.Facade
{
    /// <summary>
    /// 平台外观
    /// </summary>
    public class PlatformFacade
    {
        #region Fields

        private IPlatformDataProvider platformData;

        #endregion

        #region 构造函数

        /// <summary>
        /// 构造函数
        /// </summary>
        public PlatformFacade()
        {
            platformData = ClassFactory.GetIPlatformDataProvider();
        }
        #endregion

        #region 开房信息
        /// <summary>
        /// 钻石消耗记录
        /// </summary>
        /// <param name="whereQuery"></param>
        /// <param name="pageIndex"></param>
        /// <param name="pageSize"></param>
        /// <returns></returns>
        public PagerSet GetCreateRoomCost(string whereQuery, int pageIndex, int pageSize)
        {
            return platformData.GetCreateRoomCost(whereQuery, pageIndex, pageSize);
        }

        /// <summary>
        /// 获取创建房间
        /// </summary>
        /// <param name="roomid">房间编号</param>
        /// <param name="groupId">群组编号</param>
        /// <returns></returns>
        public StreamCreateTableFeeInfo GetStreamCreateTableFeeInfo(int roomid,long groupId)
        {
            return platformData.GetStreamCreateTableFeeInfo(roomid,groupId);
        }

        /// <summary>
        /// 获取房间总记录
        /// </summary>
        /// <param name="groupId">群组编号</param>
        /// <param name="roomId">房间编号</param>
        /// <param name="userId">房主编号</param>
        /// <returns></returns>
        public PersonalRoomScoreInfo GetPersonalRoomScoreInfo(long groupId, int roomId, int userId)
        {
            return platformData.GetPersonalRoomScoreInfo(groupId, roomId, userId);
        }
        #endregion

        #region 游戏信息
        /// <summary>
        /// 根据游戏标识获取游戏
        /// </summary>
        /// <param name="kindid">游戏标识</param>
        /// <returns></returns>
        public MobileKindItem GetGameKindItemByID(int kindid)
        {
            return platformData.GetGameKindItemByID(kindid);
        }
        /// <summary>
        /// 获取游戏列表
        /// </summary>
        /// <returns></returns>
        public IList<MobileKindItem> GetMobileKindItemList()
        {
            return platformData.GetMobileKindItemList();
        }
        /// <summary>
        /// 获取游戏列表和版本配置
        /// </summary>
        /// <returns></returns>
        public DataSet GetMobileGameAndVersion()
        {
            return platformData.GetMobileGameAndVersion();
        }
        #endregion

        #region 道具管理

        /// <summary>
        /// 获取道具信息by ID
        /// </summary>
        /// <returns></returns>
        public GameProperty GetGameProperty(int id, int ExchangeType)
        {
            return platformData.GetGameProperty(id, ExchangeType);
        }

        #endregion

        #region 公共分页
        /// <summary>
        /// 分页获取数据列表
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <param name="pageIndex">页下标</param>
        /// <param name="pageSize">页显示数</param>
        /// <param name="condition">查询条件</param>
        /// <param name="orderby">排序</param>
        /// <returns></returns>
        public PagerSet GetList(string tableName, int pageIndex, int pageSize, string condition, string orderby)
        {
            return platformData.GetList(tableName, pageIndex, pageSize, condition, orderby);
        }
        #endregion

        #region 约战战绩与统计
        /// <summary>
        /// 获取大厅战绩
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="typeid">战绩类型</param>
        /// <returns></returns>
        public DataSet GetHallBattleRecord(int userid, int typeid)
        {
            return platformData.GetHallBattleRecord(userid, typeid);
        }
        /// <summary>
        /// 获取俱乐部战绩
        /// </summary>
        /// <param name="userid">用户标识</param>
        /// <param name="groupid">俱乐部战绩</param>
        /// <param name="start">起始时间</param>
        /// <param name="end">结束时间</param>
        /// <returns></returns>
        public DataSet GetClubBattleRecord(int userid, int groupid, DateTime start, DateTime end)
        {
            return platformData.GetClubBattleRecord(userid, groupid, start, end);
        }
        #endregion
    }
}
