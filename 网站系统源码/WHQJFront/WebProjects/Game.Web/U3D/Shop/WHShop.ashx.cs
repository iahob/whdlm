using Game.Utils;
using Game.Facade;
using System;
using System.Collections.Generic;
using System.Web;
using Game.Facade.Enum;
using Game.Facade.DataStruct;
using Game.Entity.NativeWeb;
using Game.Entity.Treasure;
using Game.Kernel;
using Game.Entity.Platform;
using Game.Web.Helper;
using System.Data;
using System.Linq;
using Game.Entity.Accounts;
using Game.Entity.Group;
using System.Text.RegularExpressions;

namespace Game.Web.U3D.Shop
{
  /// <summary>
  /// WHShop 的摘要说明
  /// </summary>
  public class WHShop : IHttpHandler
  {

    public void ProcessRequest(HttpContext context)
    {
      //允许跨站请求域名
      context.Response.AddHeader("Access-Control-Allow-Origin", AppConfig.MoblieInterfaceDomain);
      //接口返回数据格式
      context.Response.ContentType = "application/json";
      //接口请求类型
      string action = GameRequest.GetQueryString("action").ToLower();
      //逻辑处理入口
      switch (action)
      {
        case "getshophome":
          GetShopHome(context);
          break;
        case "getawardinfolist":
          GetAwardInfoList(context);
          break;
        case "getorderawardinfo":
          GetOrderAwardInfo(context);
          break;
        case "getawardrecord":
          GetAwardRecord(context);
          break;
        case "setawarduser":
             SetAwardUser(context);
          break;
         case "buyaward":
              BuyAward(context);
           break;
           default:
          break;
      }
    }
      /// <summary>
      /// 获取商品类型
      /// </summary>
      /// <param name="context"></param>
    private void GetShopHome(HttpContext context)
    {
      AjaxJsonValid _ajv = new AjaxJsonValid();
      int typeid = GameRequest.GetQueryInt("typeid", 0);
      int userid= GameRequest.GetQueryInt("userid", 0);

            _ajv.msg = "获取数据成功";
      _ajv.SetValidDataValue(true);

      DataSet ds = FacadeManage.aideNativeWebFacade.GetAwardHomeList(typeid);
            UserCurrency Currency = FacadeManage.aideTreasureFacade.GetUserCurrency(userid);
            IList<AwardInfo> list= DataHelper.ConvertDataTableToObjects<AwardInfo>(ds.Tables[1]);
            foreach (AwardInfo Info in list)
            {
                Info.AwardImg = Info.AwardImg.IndexOf("http://", StringComparison.Ordinal) < 0
                    ? Fetch.GetUploadFileUrl(Info.AwardImg)
                    : Info.AwardImg;
            }

      _ajv.SetDataItem("type", ds.Tables[0]);
      _ajv.SetDataItem("list", list);
      _ajv.SetDataItem("AwardTicket", Currency==null?0: Currency.AwardTicket);
            context.Response.Write(_ajv.SerializeToJson());
    }
    /// <summary>
    /// 获取商品列表
    /// </summary>
    /// <param name="context"></param>
    private void GetAwardInfoList(HttpContext context)
    {
      int typeid = GameRequest.GetQueryInt("typeid", 0);

      AjaxJsonValid _ajv = new AjaxJsonValid();
      _ajv.msg = "获取数据成功";
      _ajv.SetValidDataValue(true);

      DataSet ds = FacadeManage.aideNativeWebFacade.GetAwardInfoByType(typeid);
            IList<AwardInfo> list = DataHelper.ConvertDataTableToObjects<AwardInfo>(ds.Tables[0]);
            foreach (AwardInfo Info in list)
            {
                Info.AwardImg = Info.AwardImg.IndexOf("http://", StringComparison.Ordinal) < 0
                    ? Fetch.GetUploadFileUrl(Info.AwardImg)
                    : Info.AwardImg;
            }
            _ajv.SetDataItem("list", list);
      context.Response.Write(_ajv.SerializeToJson());
    }
    /// <summary>
    /// 获取下单信息
    /// </summary>
    /// <param name="context"></param>
    private void GetOrderAwardInfo(HttpContext context)
    {
      int id = GameRequest.GetQueryInt("id", 0);
      int userid = GameRequest.GetQueryInt("uid", 0);


      AjaxJsonValid _ajv = new AjaxJsonValid();
            if (id == 0)
            {
                _ajv.msg = "缺少参数奖品ID";
                context.Response.Write(_ajv.SerializeToJson());
                return;
            }

     UserCurrency Currency = FacadeManage.aideTreasureFacade.GetUserCurrency(userid);
            _ajv.msg = "获取数据成功";
      _ajv.SetValidDataValue(true);
     

      DataSet ds = FacadeManage.aideNativeWebFacade.GetOrderAwardInfo(userid, id);

      _ajv.SetDataItem("award", ds.Tables[0]);
      _ajv.SetDataItem("uinfo", ds.Tables[1]);
      _ajv.SetDataItem("AwardTicket", Currency.AwardTicket);
            context.Response.Write(_ajv.SerializeToJson());
    }
    /// <summary>
    /// 获取兑换记录信息
    /// </summary>
    /// <param name="context"></param>
    private void GetAwardRecord(HttpContext context)
    {
       int userid = GameRequest.GetQueryInt("uid", 0);
       
      AjaxJsonValid _ajv = new AjaxJsonValid();
        
      _ajv.msg = "获取数据成功";
      _ajv.SetValidDataValue(true);

      DataSet ds = FacadeManage.aideNativeWebFacade.GetAwardOrderList(userid);

      _ajv.SetDataItem("list", ds.Tables[0]);
      context.Response.Write(_ajv.SerializeToJson());
    }


        private void SetAwardUser(HttpContext context)
        {
            AjaxJsonValid ajaxJson = new AjaxJsonValid();
            Message msg = new Message();
            int userid = GameRequest.GetQueryInt("userid", 0);
            string settype= TextFilter.FilterScript(GameRequest.GetQueryString("settype"));  //设置类型update修改insert添加
            string compellation = TextFilter.FilterScript(GameRequest.GetQueryString("name"));      //真实姓名
            string mobilePhone = TextFilter.FilterScript(GameRequest.GetQueryString("phone"));      //移动电话
            string province = GameRequest.GetQueryString("province");        //省份
            string city = GameRequest.GetQueryString("city");                //城市
            string area = GameRequest.GetQueryString("area");                //地区
            string dwellingPlace = TextFilter.FilterScript(GameRequest.GetQueryString("address"));  //详细地址
                                                                                                   //验证真实姓名

            msg = CheckingRealNameFormat(compellation, false);
            if (!msg.Success)
            {
                ajaxJson.msg = "请输入正确的收件人";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }


            //验证手机号

            msg = CheckingMobilePhoneNumFormat(mobilePhone, false);
            if (!msg.Success)
            {
                ajaxJson.msg = "请输入正确的手机号码";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }
            //验证地址邮编

            if (province.Length <1)
            {
                ajaxJson.msg = "请选择省份";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }
            if (city.Length <1)
            {
                ajaxJson.msg = "请选择城市";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }
            if (area.Length< 1)
            {
                ajaxJson.msg = "请选择地区";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }
            if (string.IsNullOrEmpty(dwellingPlace))
            {
                ajaxJson.msg = "请输入详细地址";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }
            AwardUser awardUser = new AwardUser();
            awardUser.UserID = userid;
            awardUser.Compellation = compellation;
            awardUser.MobilePhone = mobilePhone;
            awardUser.Province = province;
            awardUser.City = city;
            awardUser.Area = area;
            awardUser.DetailAddress = dwellingPlace;
            if(settype== "update")
            {
                int rs= FacadeManage.aideNativeWebFacade.updateAwardUser(awardUser);
                if (rs > 0)
                {
                    ajaxJson.msg = "修改成功";
                    ajaxJson.SetValidDataValue(true);
                    ajaxJson.SetDataItem("rs", 1);
                    context.Response.Write(ajaxJson.SerializeToJson());
                    return;
                }
                else
                {
                    ajaxJson.msg = "修改失败";
                    ajaxJson.SetDataItem("rs", -1);
                    context.Response.Write(ajaxJson.SerializeToJson());
                    return;
                }
            }
            if(settype == "insert")
            {
                int rs = FacadeManage.aideNativeWebFacade.insertAwardUser(awardUser);
                if (rs > 0)
                {
                    ajaxJson.msg = "提交成功";
                    ajaxJson.SetValidDataValue(true);
                    ajaxJson.SetDataItem("rs", 1);
                    context.Response.Write(ajaxJson.SerializeToJson());
                    return;
                }
                else
                {
                    ajaxJson.msg = "提交失败";
                    ajaxJson.SetDataItem("rs", -1);
                    context.Response.Write(ajaxJson.SerializeToJson());
                    return;
                }
            }


        }



        /// <summary>
        /// 购买商品
        /// </summary>
        /// <param name="context"></param>
        private void BuyAward(HttpContext context)
        {
            Message msg = new Message();
            AjaxJsonValid ajaxJson = new AjaxJsonValid();
            int userid = GameRequest.GetQueryInt("userid", 0);
            //判断登录
            //if (!Fetch.IsUserOnline())
            //{
            //    ajaxJson.code = 1;
            //    ajaxJson.msg = "请先登录";
            //    context.Response.Write(ajaxJson.SerializeToJson());
            //    return;
            //}

            //获取参数
           // int typeID = GameRequest.GetQueryInt("TypeID", 0);
            int awardID = GameRequest.GetQueryInt("awardID", 0);           //商品ID
            int counts = GameRequest.GetQueryInt("counts", 0);             //购买数量
            AccountsInfo userinfo = FacadeManage.aideAccountsFacade.GetAccountsInfoByUserID(userid);

            //验证奖品
            if (awardID == 0)
            {
                ajaxJson.msg = "非常抱歉，你所选购的商品不存在！";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }

            //验证数量
            if (counts <= 0)
            {
                ajaxJson.msg = "请输入正确的兑换数量！";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }
            if (counts > 100)
            {
                ajaxJson.msg = "兑换数量不能超过100！";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }

            AwardInfo awardInfo = FacadeManage.aideNativeWebFacade.GetAwardInfo(awardID);
         
            AwardUser awardUser= FacadeManage.aideNativeWebFacade.GetAwardUser(userid);


            //验证用户
            UserCurrency Currency = FacadeManage.aideTreasureFacade.GetUserCurrency(userid);


            //验证余额
            int totalAmount = awardInfo.UnitPrice * counts;     //总金额
            if (totalAmount <= 0)
            {
                ajaxJson.msg = "很抱歉！兑换的奖品配置额度太大或为零";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }
            if (totalAmount >(Currency==null?0:Currency.AwardTicket))
            {
                ajaxJson.msg = "很抱歉！您的奖券数不足，不能兑换该奖品";
                ajaxJson.code = -1;
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }

            //验证库存
            if (awardInfo.Inventory <= 0 && awardInfo.AwardType!=1)
            {
                ajaxJson.msg = "很抱歉！奖品的库存数不足，请更新其他奖品或者等待补充库存";
                context.Response.Write(ajaxJson.SerializeToJson());
                return;
            }

            //扣除奖牌
            Currency.AwardTicket = Currency.AwardTicket - totalAmount;

            //更新奖牌
            AwardOrder awardOrder = new AwardOrder();
            awardOrder.OrderID= Fetch.GetOrderIDByPrefix("XT");
            awardOrder.UserID = Currency.UserID;
            awardOrder.GameID = userinfo.GameID;
            awardOrder.AwardID = awardID;
            awardOrder.AwardType = awardInfo.AwardType;
            awardOrder.AwardName = awardInfo.AwardName;
            awardOrder.UnitPrice = awardInfo.UnitPrice;
            awardOrder.BuyNum = counts;
            awardOrder.PayTicket = totalAmount;
            awardOrder.Gold = awardInfo.Gold;
            awardOrder.Diamond = awardInfo.Diamond;
            awardOrder.Compellation = awardUser==null?"": awardUser.Compellation;
            awardOrder.MobilePhone = awardUser == null ? "" : awardUser.MobilePhone ;
            awardOrder.Province = awardUser == null ? "" : awardUser.Province ;
            awardOrder.City = awardUser == null ? "" : awardUser.City;
            awardOrder.Area = awardUser == null ? "" : awardUser.Area;
            awardOrder.DetailAddress = awardUser == null ? "" : awardUser.DetailAddress;
            awardOrder.ClinetIP = Utility.UserIP;

            msg = FacadeManage.aideNativeWebFacade.BuyAward(awardOrder);
            if (msg.Success)
            {

                ajaxJson.SetValidDataValue(true);
                 ajaxJson.msg = "兑换成功!实物商品请注意查收";
                if (awardInfo.AwardType == 1)
                {
                    ajaxJson.msg = "兑换成功！将在5分钟内发放至游戏中";
                }
               
                awardOrder = msg.EntityList[0] as AwardOrder;
              
                ajaxJson.SetDataItem("rs", 1);
                context.Response.Write(ajaxJson.SerializeToJson());
            }
            else
            {
                ajaxJson.msg = msg.Content;
                ajaxJson.SetDataItem("rs", -1);
                context.Response.Write(ajaxJson.SerializeToJson());
            }
        }
        /// <summary>
        /// 验证真实姓名
        /// </summary>
        /// <param name="realName"></param>
        /// <returns></returns>
        public static Message CheckingRealNameFormat(string realName, bool isAllowNull)
        {
            Message msg = new Message();
            if (isAllowNull && string.IsNullOrEmpty(realName))
            {
                return msg;
            }
            if (!isAllowNull && string.IsNullOrEmpty(realName))
            {
                msg.Success = false;
                msg.Content = "真实姓名不能为空";
                return msg;
            }
            if (realName.Length > 16)
            {
                msg.Success = false;
                msg.Content = "真实姓名太长";
                return msg;
            }
            return msg;
        }

        /// <summary>
        /// 验证QQ号码
        /// </summary>
        /// <param name="qq"></param>
        /// <param name="isAllowNull"></param>
        /// <returns></returns>
        public static Message CheckingQQFormat(string qq, bool isAllowNull)
        {
            Message msg = new Message();
            if (isAllowNull && string.IsNullOrEmpty(qq))
            {
                return msg;
            }
            if (!isAllowNull && string.IsNullOrEmpty(qq))
            {
                msg.Success = false;
                msg.Content = "QQ号码不能为空";
                return msg;
            }
            Regex reg = new Regex(@"^\d{4,20}$");
            if (!string.IsNullOrEmpty(qq) && !reg.IsMatch(qq))
            {
                msg.Success = false;
                msg.Content = "QQ号码格式不对";
                return msg;
            }
            return msg;
        }

        /// <summary>
        /// 验证移动电话号码
        /// </summary>
        /// <param name="mobilePhoneNum"></param>
        /// <returns></returns>
        public static Message CheckingMobilePhoneNumFormat(string mobilePhoneNum, bool isAllowNull)
        {
            Message msg = new Message();
            if (isAllowNull && string.IsNullOrEmpty(mobilePhoneNum))
            {
                return msg;
            }
            if (!isAllowNull && string.IsNullOrEmpty(mobilePhoneNum))
            {
                msg.Success = false;
                msg.Content = "电话号码不能为空";
                return msg;
            }

            Regex isMobile = new Regex(@"^\d{11}$", RegexOptions.Compiled);
            if (!isMobile.IsMatch(mobilePhoneNum))
            {
                msg.Success = false;
                msg.Content = "移动电话格式不正确";
                return msg;
            }
            return msg;
        }

        public bool IsReusable
    {
      get
      {
        return false;
      }
    }
  }
}