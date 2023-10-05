using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Web.UI;
using Game.Utils;
using Game.Entity.Treasure;
using Game.Facade;
using System.Data;
using Game.Entity.Accounts;
using Game.Kernel;
using Game.Entity.Platform;
using Game.Socket;
using System.Threading;
using Game.Entity.Group;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.DebugManager
{
    public partial class AddAIAccounts : AdminPage
    {

        protected static byte Status = 1;
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 1002;
            if (Page.IsPostBack) return;
            

            BindGame();

        }

        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "新增调试账号";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);


            int groupId = GameRequest.GetInt("groupid", 0);
            if (groupId <= 0)
            {


                MessageBox("参数错误");
                return;
            }

            short kindId = Convert.ToInt16(ddlGame.SelectedValue);
            if (kindId <= 0)
            {


                MessageBox("请选择游戏");
                return;
            }

            int AIRateMin = Convert.ToInt32(txtAIRateMin.Text);
            int AIRateMax = Convert.ToInt32(txtAIRateMax.Text);
            int AICount = Convert.ToInt32(txtAICount.Text);
            int DistirbuteScoreMinRatio = Convert.ToInt32(txtDistirbuteScoreMinRatio.Text);
            int DistirbuteScoreMaxRatio = Convert.ToInt32(txtDistirbuteScoreMaxRatio.Text);
            long DistributeTotalScore = Convert.ToInt64(txtDistributeTotalScore.Text);
            long DistirbuteSingleScore = Convert.ToInt64(txtDistirbuteSingleScore.Text);



            if (AIRateMin > 100 || AIRateMax > 100)
            {

                MessageBox("参与比例不能大于100%");
                return;
            }

            if (AIRateMin > AIRateMax)
            {

                MessageBox("参与比例的上限不能小于参与比例的下限");
                return;
            }

            if (DistirbuteScoreMinRatio > 100 || DistirbuteScoreMaxRatio > 100)
            {

                MessageBox("浮动范围不能大于100%");
                return;
            }

            if (DistirbuteScoreMinRatio > DistirbuteScoreMaxRatio)
            {

                MessageBox("浮动范围下限不能大于浮动范围上限");
                return;
            }

            IMGroupProperty group = FacadeManage.aideGroupFacade.GetGroupInfo(groupId);
            int LastMemberCount = group.MaxMemberCount - group.MemberCount;
            if (LastMemberCount < AICount)
            {

                MessageBox($"调试账号数量不能大于{LastMemberCount}");
                return;
            }

            IMGroupWealth groupWealth = FacadeManage.aideGroupFacade.GetGroupWealth(groupId);

            if (DistributeTotalScore > groupWealth.Score)
            {

                MessageBox($"调试账号积分上限不能大于{groupWealth.Score}");
                return;
            }

            if (DistributeTotalScore / AICount < DistirbuteSingleScore)
            {
                MessageBox($"调试账号携带积分不能大于{DistributeTotalScore / AICount}");
                return;
            }



            GroupAIParameter model =new GroupAIParameter();
            model.GroupID = groupId;
            model.KindID = kindId;
            model.AICount = AICount;
            model.DistributeTotalScore= DistributeTotalScore;
            model.PlayingAIRateMin= AIRateMin;
            model.PlayingAIRateMax = AIRateMax;
            model.DistirbuteSingleScore = DistirbuteSingleScore;
            model.DistirbuteScoreMinRatio = DistirbuteScoreMinRatio;
            model.DistirbuteScoreMaxRatio = DistirbuteScoreMaxRatio;

            //向服务器发送消息
            SocketManage.aideWebSocketFacade.OperateResultARE = new AutoResetEvent(false);
            SocketManage.aideWebSocketFacade.UpdateAiParamter(Status, model, isSent =>{});
            SocketManage.aideWebSocketFacade.OperateResultARE.WaitOne();

            if (SocketManage.aideWebSocketFacade.Code == 0)
            {

                MessageBoxCloseRef("操作成功");
            }
            else
            {

                MessageBox(SocketManage.aideWebSocketFacade.Message);
            }
        }




        /// <summary>
        /// 绑定游戏列表
        /// </summary>
        protected void BindGame()
        {
            PagerSet ps = FacadeManage.aidePlatformFacade.GetList(GameGameItem.Tablename, "", "ORDER BY GameID DESC", 1, 1000);
            DataSet ds = FacadeManage.aidePlatformFacade.GetGroupKindList();
            ddlGame.DataSource = ds.Tables[0];
            ddlGame.DataTextField = "KindName";
            ddlGame.DataValueField = "KindID";
            ddlGame.DataBind();
            ddlGame.Items.Insert(0, new ListItem("选择游戏", "0"));
        }


        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void BindData()
        {
            int groupId = GameRequest.GetInt("groupid", 0);
            if (groupId <= 0)
            {
                MessageBox("参数错误");
                return;
            }

            short kindId = Convert.ToInt16(ddlGame.SelectedValue);

            GroupAIParameter aip = FacadeManage.aideAccountsFacade.GetAIParameter(groupId, kindId);
            if (aip != null)
            {
                CtrlHelper.SetText(txtAICount, aip.AICount.ToString());
                CtrlHelper.SetText(txtDistributeTotalScore, aip.DistributeTotalScore.ToString());
                CtrlHelper.SetText(txtAIRateMin, aip.PlayingAIRateMin.ToString());
                CtrlHelper.SetText(txtAIRateMax, aip.PlayingAIRateMax.ToString());
                CtrlHelper.SetText(txtDistirbuteSingleScore, aip.DistirbuteSingleScore.ToString());
                CtrlHelper.SetText(txtDistirbuteScoreMinRatio, aip.DistirbuteScoreMinRatio.ToString());
                CtrlHelper.SetText(txtDistirbuteScoreMaxRatio, aip.DistirbuteScoreMaxRatio.ToString());

                txtDistributeTotalScore.Enabled = false;
                Status = 2;
                spAICount.InnerText = "";
                sptDistributeTotalScore.InnerText = "";



            }
            else
            {
                //默认提示
                IMGroupProperty group = FacadeManage.aideGroupFacade.GetGroupInfo(groupId);
                int LastMemberCount = group.MaxMemberCount - group.MemberCount;
                spAICount.InnerText = $"当前大联盟只有{LastMemberCount}空位，配置不可超过{LastMemberCount}";

                //long AIDistributionScore = FacadeManage.aideAccountsFacade.GetAIDistributionScore(groupId);
                IMGroupWealth groupWealth = FacadeManage.aideGroupFacade.GetGroupWealth(groupId);
                sptDistributeTotalScore.InnerText = $"当前大联盟可分配勋章为{groupWealth.Score}，配置不可超过{groupWealth.Score}";
                

                CtrlHelper.SetText(txtAICount, "");
                CtrlHelper.SetText(txtDistributeTotalScore, "");
                CtrlHelper.SetText(txtAIRateMin, "");
                CtrlHelper.SetText(txtAIRateMax, "");
                CtrlHelper.SetText(txtDistirbuteSingleScore, "");
                CtrlHelper.SetText(txtDistirbuteScoreMinRatio, "");
                CtrlHelper.SetText(txtDistirbuteScoreMaxRatio, "");

                txtDistributeTotalScore.Enabled = true;
                Status = 1;
            }

        }

        protected void ddlGame_SelectedIndexChanged(object sender, EventArgs e)
        {
            BindData();


        }
    }
}