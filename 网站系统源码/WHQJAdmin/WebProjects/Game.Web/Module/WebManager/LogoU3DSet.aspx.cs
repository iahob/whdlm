﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using Game.Utils;
using Game.Web.UI;
using Game.Kernel;
using System.Text;
using Game.Entity.NativeWeb;
using Game.Entity.Accounts;
using Game.Entity.Enum;
using System.IO;
using Game.Entity.PlatformManager;
using Game.Facade;

namespace Game.Web.Module.WebManager
{
    public partial class LogoU3DSet : AdminPage
    {
        /// <summary>
        /// 页面加载
        /// </summary>
        protected void Page_Load(object sender, EventArgs e)
        {
            base.moduleID = 400;
            Random rd = new Random();
            igAdminLogo.ImageUrl = "/Upload/Site/u3dadminlogo.png?" + rd.Next(100, 1000000).ToString();
            igFrontLogo.ImageUrl = "/Upload/Site/u3dfrontlogo.png?" + rd.Next(100, 1000000).ToString();
            igQrSmall.ImageUrl = "/Upload/Site/u3dqrsmall.png?" + rd.Next(100, 1000000).ToString();
            igDownload.ImageUrl = "/Upload/Site/u3ddownLoad.png?" + rd.Next(100, 1000000).ToString();
            igMobileBack.ImageUrl = "/Upload/Site/u3dmobilebg.png?" + rd.Next(100, 1000000).ToString();
            igPcBack.ImageUrl = "/Upload/Site/u3dpcbg.png?" + rd.Next(100, 1000000).ToString();
            igSpreadCase.ImageUrl = "/Upload/Site/u3dspreadcase.png?" + rd.Next(100, 1000000).ToString();
            igCustomer.ImageUrl = "/Upload/Site/u3dqrcustomer.png?" + rd.Next(100, 1000000).ToString();
        }
        /// <summary>
        /// 数据保存
        /// </summary>
        protected void btnSave_Click(object sender, EventArgs e)
        {
            // 插入日志
            SystemSecurity ss = new SystemSecurity();
            ss.OperatingAccounts = userExt.UserName;
            ss.OperatingName = "修改U3D网站图片";
            ss.OperatingIP = userExt.PreLoginIP;
            FacadeManage.aidePlatformManagerFacade.InsertSystemSecurity(ss);

            //判断权限
            AuthUserOperationPermission(Permission.Edit);

            Message msg = new Message();
            msg = SaveImage(fuAdminLogo, "/Upload/Site", "u3dadminlogo.png");
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            msg = SaveImage(fuFrontLogo, "/Upload/Site", "u3dfrontlogo.png");
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            msg = SaveImage(fuMobileBack, "/Upload/Site", "u3dmobilebg.png");
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            msg = SaveImage(fuDownload, "/Upload/Site", "u3ddownLoad.png");
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            msg = SaveImage(fuPcBack, "/Upload/Site", "u3dpcbg.png");
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            msg = SaveImage(fuSpreadCase, "/Upload/Site", "u3dspreadcase.png");
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            msg = SaveImage(fuQrSmall, "/Upload/Site", "u3dqrsmall.png");
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            msg = SaveImage(fuCustomer, "/Upload/Site", "u3dqrcustomer.png");
            if (!msg.Success)
            {
                ShowError(msg.Content);
                return;
            }
            ShowInfo("操作成功");
            Response.Redirect("LogoU3DSet.aspx?");
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