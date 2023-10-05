using System;
using Game.Web.UI;
using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Utils;
using Game.Entity.Enum;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.WebManager
{
    public partial class QuestionInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 405;
            if(!IsPostBack)
            {
                BindData();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            Question quest = new Question()
            {
                QuestionTitle = txtQuestion.Text.Trim(),
                Answer = txtAnswer.Text.Trim(),
                SortID = Convert.ToInt32(txtSortID.Text)
            };
           
            if(IntParam > 0)
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改常见问题";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Edit);
                quest.ID = IntParam;
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "新增常见问题";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Add);
            }
            quest.QuestionTitle = CtrlHelper.GetTextAndFilter(txtQuestion);
            quest.Answer = CtrlHelper.GetTextAndFilter(txtAnswer);
            quest.SortID = CtrlHelper.GetInt(txtSortID,0);

            int result = FacadeManage.aideNativeWebFacade.SaveQuestionInfo(quest);
            if(result > 0)
            {
                ShowInfo("常见问题操作成功", "QuestionList.aspx", 1000);
            }
            else
            {
                ShowError("常见问题操作失败");
            }
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void BindData()
        {
            if(IntParam > 0)
            {
                Question quest = FacadeManage.aideNativeWebFacade.GetQuestionInfo(IntParam);
                if(quest != null)
                {
                    CtrlHelper.SetText(txtAnswer,quest.Answer);
                    CtrlHelper.SetText(txtQuestion,quest.QuestionTitle);
                    CtrlHelper.SetText(txtSortID,quest.SortID.ToString());
                }
            }
        }
    }
}