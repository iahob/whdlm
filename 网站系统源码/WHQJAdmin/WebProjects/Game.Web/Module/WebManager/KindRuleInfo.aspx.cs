using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Game.Web.UI;
using Game.Entity.NativeWeb;
using Game.Facade;
using Game.Utils;
using Game.Entity.Enum;
using Game.Entity.Platform;
using Game.Kernel;
using System.IO;
using Game.Entity.PlatformManager;

namespace Game.Web.Module.WebManager
{
    public partial class KindRuleInfo : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 401;
            if(!IsPostBack)
            {
                BindGame();
                BindData();
            }
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            GameRule rule = null;
            int kindid = Convert.ToInt32(ddlGame.SelectedValue);
            int Type= Convert.ToInt32(ddlType.SelectedValue);
            if (IntParam > 0)
            {

                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "修改游戏规则";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Edit);
                rule = FacadeManage.aideNativeWebFacade.GetGameRuleInfo(IntParam);
                rule.LogID = rule.LogID + 1;
            }
            else
            {
                // 插入日志
                SystemSecurity ss = new SystemSecurity();
                ss.OperatingAccounts = userExt.UserName;
                ss.OperatingName = "新增游戏规则";
                ss.OperatingIP = userExt.PreLoginIP;
                FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

                AuthUserOperationPermission(Permission.Add);
                rule = FacadeManage.aideNativeWebFacade.GetGameRuleInfoByKindid(kindid, Type);
                if (rule != null)
                {
                    ShowError("游戏规则已存在");
                    return;
                }
                rule = new GameRule();
                rule.CollectDate = DateTime.Now;
            }
            //string filepath = upImage.FilePath;
            string imgname = "Rule" + DateTime.Now.ToString("yyyyMMddhhmmss") + ".png";
            Message msg = new Message();
            msg = SaveImage(fuFrontRuleImg, "/Upload/RuleIoc", imgname);
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            //if (string.IsNullOrEmpty(filepath))
            //{
            //    ShowError("游戏图标未上传！");
            //    return;
            //}
            //rule.KindIcon = filepath.Substring(7, filepath.Length - 7);
            rule.KindID = kindid;
            rule.KindName = ddlGame.SelectedItem.Text;
            rule.RuleImg = "/Upload/RuleIoc/" + imgname;
            rule.Type = Type;

           
            //rule.KindIntro = CtrlHelper.GetTextAndFilter(txtIntro);
            //rule.KindRule = txtRule.Text;
            rule.SortID = CtrlHelper.GetInt(txtSortID, 0);

            int result = IntParam > 0 ? FacadeManage.aideNativeWebFacade.UpdateGameRule(rule) : FacadeManage.aideNativeWebFacade.InsertGameRule(rule);
            if(result > 0)
            {
                ShowInfo("游戏规则操作成功", "KindRuleList.aspx", 1000);
            }
            else
            {
                ShowError("游戏规则操作失败");
            }
        }
        /// <summary>
        /// 绑定游戏列表
        /// </summary>
        protected void BindGame()
        {
            PagerSet ps = FacadeManage.aidePlatformFacade.GetList(GameGameItem.Tablename, "", "ORDER BY GameID DESC", 1, 1000);
            ddlGame.DataSource = ps.PageSet;
            ddlGame.DataTextField = "GameName";
            ddlGame.DataValueField = "GameID";
            ddlGame.DataBind();
        }
        /// <summary>
        /// 数据绑定
        /// </summary>
        protected void BindData()
        {
            if(IntParam > 0)
            {
                GameRule rule = FacadeManage.aideNativeWebFacade.GetGameRuleInfo(IntParam);
                if(rule != null)
                {
                    ddlGame.SelectedValue = rule.KindID.ToString();
                    ddlType.SelectedValue = rule.Type.ToString();

                    //CtrlHelper.SetText(txtIntro, rule.KindIntro);
                    //txtRule.Text = rule.KindRule;
                    //if (!string.IsNullOrEmpty(rule.KindIcon))
                    //{
                    //    upImage.FilePath = "/Upload" + rule.KindIcon;
                    //}
                    ddlGame.Enabled = false;
                    CtrlHelper.SetText(txtSortID, rule.SortID.ToString());

                }
            }
        }

        /// <summary>
        /// 保存图片
        /// </summary>
        /// <param name="fileControl"></param>
        /// <param name="path"></param>
        /// <param name="fileName"></param>
        /// <returns></returns>
        protected Message SaveImage(FileUpload fileControl, string path, string fileName)
        {
            Message msg = new Message();

            //验证图片
            HttpPostedFile file = fileControl.PostedFile;
            if (file.ContentLength != 0)
            {
                if (fileControl.FileName.Substring(fileControl.FileName.LastIndexOf(".") + 1).ToLower() != "png")
                {
                    msg.Content = "LOGO必须为PNG格式";
                    msg.Success = false;
                    return msg;
                }
                try
                {
                    // 转化图片
                    System.Drawing.Image img = System.Drawing.Image.FromStream(file.InputStream);
                }
                catch
                {
                    msg.Content = "不是合法的图片";
                    msg.Success = false;
                    return msg;
                }
            }
            else
            {
                return msg;
            }

            //检查目录
            string serverPath = Server.MapPath(path);
            if (!Directory.Exists(serverPath))
            {
                Directory.CreateDirectory(serverPath);
            }
            //检查文件
            string filePath = Server.MapPath(path + "/" + fileName);
            if (File.Exists(filePath))
            {
                File.Delete(filePath);
            }

            //保存图片
            file.SaveAs(filePath);
            return msg;
        }
    }
}