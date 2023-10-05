<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="RoomLink.aspx.cs" Inherits="Game.Web.Mobile.RoomLink" %>

    <!DOCTYPE html>

    <html xmlns="http://www.w3.org/1999/xhtml">

    <head runat="server">
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <title><%=Title %></title>
        <meta content="text/html; charset=utf-8" http-equiv="content-type" />
        <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no"/>
        <meta name="apple-mobile-web-app-capable" content="yes" />
        <meta name="apple-touch-fullscreen" content="yes" />
        <meta name="format-detection" content="telephone=no" />
        <link href="/css/roomshare.css" rel="stylesheet" />
        <link href="/css/weixin.css" rel="stylesheet" />
        <script type="text/javascript" src="/js/zepto/1.1.6/zepto.min.js"></script>
        <script type="text/javascript" src="/js/share.js"></script>
        <script type="text/javascript" src="/js/weixin.js"></script>
    </head>

    <body>
    <img src="/image/qj_bg.jpg" alt=""/>
    <main>
      <div class="ui-face">
        <img src="<%=KindIco %>" class="ui-head" alt="" /><!-- 
      --><span><%=KindName %></span>
      </div>
      <div class="ui-room">
        房号：<span><%=Roomid %></span>
      </div>
      <div class="ui-detail">
        <p>您的好友<span><%=Nickname %></span>喊你一起打牌啦！！！</p>
      </div>
      <a href="javascript:;" class="ui-btn" id="join" data-f="<%=Finish %>" data-u="<%=PlatformDownloadUrl %>" data-r="<%=Roomid %>" data-k="<%=Kindid %>"
                data-a="<%=Action %>" data-p="<%=Password %>" data-y="<%=Type %>">
        <img src="/image/qj_btn.png" alt=""/>
      </a>
      <div class="ui-item">
        <img src="/image/item1.png" class="ui-icon1"/><!--
        --><img src="/image/item2.png" class="ui-icon2"/><!--
        --><img src="/image/item3.png" class="ui-icon3"/>
      </div>
    </main>
        <div id="weixin-tip" class="ui-weixin-tip fn-hide">
            <p>
                <img src="/image/live_weixin.png" alt="微信打开" />
                <span title="关闭" class="close">×</span>
            </p>
        </div>
        <script type="text/javascript">
            $(function () {
                var button = $('#join');
                var url = button.attr('data-u');
                button.on('click', function () {
                  // 判断useragent，当前设备为Android设备
                  var loadDateTime = new Date();

                  // 设置时间阈值，在规定时间里面没有打开对应App的话，直接去下载地址。
                  window.setTimeout(function () {
                      var timeOutDateTime = new Date();
                      if (timeOutDateTime - loadDateTime < 5000) {

                        window.location = url; // Android端URL Schema 
                      } else {
                        window.close();
                      }
                    },
                    2000);
                  var cmd = "newqjclient";
                  if (button.attr('data-y') == 'u3d') {
                    cmd = "newqjunityclient";
                  }


                  window.location = "" + cmd + "://?roomid=" + $(this).attr('data-r') + "&kindid=" +
                    $(this).attr('data-k') + "&action=" + $(this).attr('data-a') + "&password=" + $(
                      this).attr('data-p') + "";　　 // Android端URL Schema
                });
            });
         </script>
    </body>

    </html>