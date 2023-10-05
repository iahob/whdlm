using Game.Entity.Accounts;
using Game.Facade;
using Game.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Game.Web.Card
{
    public partial class setPassword : System.Web.UI.Page
    {
        protected string Gameid = GameRequest.GetQueryString("gameid");
        protected void Page_Load(object sender, EventArgs e)
        {

        }
        protected void btnAuth_OnClick(object sender, EventArgs e)
        {
            int spResult = 0;
            if (string.IsNullOrEmpty(txtPassword.Text))
            {
                Response.Write("<script language='javascript'> alert('抱歉，密码不能为空!') </script> ");
                return;
            }
            else if(txtPassword.Text!=txtrepeatPassword.Text)
            {
                Response.Write("<script language='javascript'> alert('抱歉，两次密码输入不一致!') </script> ");
                return;
            }
            else
            {
                if (!string.IsNullOrEmpty(Gameid))
                {
                    AccountsInfo user = FacadeManage.aideAccountsFacade.GetAccountsInfoByGameID(Convert.ToInt32(Gameid));
                    if (user.AgentID > 0)
                    {
                        spResult = FacadeManage.aideAgentFacade.SetAgentSafePassword(user.AgentID, Facade.Fetch.GetMD5(txtPassword.Text));
                    }
                }
                if (spResult > 0)
                {
                    Response.Write("<script language='javascript'> alert('设置安全密码成功!');location='index.aspx?g=" + Gameid + "' </script> ");
                  
                    return;
                }
                else
                {
                    Response.Write("<script language='javascript'> alert('设置安全密码失败!') </script> ");
                  
                    return;
                }
            }
           

        }
        /// <summary>
        /// 错误显示
        /// </summary>
        /// <param name="msg"></param>
        protected void ShowError(string msg)
        {
            Page.ClientScript.RegisterStartupScript(typeof(Page), "", string.Format("showError(\"{0}\");", msg), true);
        }
    }
}