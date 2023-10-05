<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ShareLink.aspx.cs" Inherits="Game.Web.Mobile.ShareLink" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">

<head runat="server">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <title>
    <%=Title %>
  </title>
  <meta content="text/html; charset=utf-8" http-equiv="content-type" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no"
  />
  <meta name="apple-mobile-web-app-capable" content="yes" />
  <meta name="apple-touch-fullscreen" content="yes" />
  <meta name="format-detection" content="telephone=no" />
  <link href="/css/friendshare.css" rel="stylesheet" />
  <link href="/css/weixin.css" rel="stylesheet" />
  <script type="text/javascript" src="/js/zepto/1.1.6/zepto.min.js"></script>
  <script type="text/javascript" src="/js/share.js"></script>
  <script type="text/javascript" src="/js/weixin.js"></script>
</head>

<body>
  <img src="/image/qj_bg.jpg" alt=""/>
  <main>
    <div class="ui-face">
      <img src="<%=FaceUrl %>" class="ui-player" alt="" />
      <span><%=Nickname %></span>
    </div>
    <div class="ui-room">
      推荐ID：<span><%=Spread %></span>
    </div>
    <div class="ui-detail">
      <p>您的好友向您推荐游戏<br/>填写推荐ID，可获得 <img src="/image/diamond.png" alt="" /> <span><%=Diamond %></span> 奖励</p>
    </div>
    <a href="javascript:;" target="_blank" class="ui-btn" id="recevice">
      <img src="/image/recieve.png" alt=""/>
    </a>
    <div class="ui-item">
      <img src="/image/item1.png" class="ui-icon1"/><!--
      --><img src="/image/item2.png" class="ui-icon2"/><!--
      --><img src="/image/item3.png" class="ui-icon3"/>
    </div>
  </main>

  <div id="weixin-tip" class="ui-weixin-tip fn-hide">
    <img src="/image/live_weixin.png" alt="微信打开" />
    <span title="关闭" class="close">×</span>
  </div>
  <script type="text/javascript">
    var link = '<%=PlatformDownloadUrl %>';
    $(function () {
      var button = $('#recevice');
      button.on('click', function () {
        // 判断useragent，当前设备为Android设备
        var loadDateTime = new Date();

        // 设置时间阈值，在规定时间里面没有打开对应App的话，直接去下载地址。
        window.setTimeout(function () {
          var timeOutDateTime = new Date();
          if (timeOutDateTime - loadDateTime < 5000) {

            window.location = link; // Android端URL Schema
          } else {
            window.close();
          }
        },
          2000);
        window.location = "newqjclient://?1=1";　　 // Android端URL Schema
      });
    });
  </script>
</body>
</html>
