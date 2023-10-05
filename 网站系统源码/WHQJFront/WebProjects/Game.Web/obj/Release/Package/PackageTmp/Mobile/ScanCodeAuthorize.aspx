<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ScanCodeAuthorize.aspx.cs" Inherits="Game.Web.Mobile.ScanCodeAuthorize" %>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>微信扫码登录</title>
    <style>
        #msg {
            width: 300px;
            margin: 0 auto;
            text-align: center;
            color: #f00;
            font-weight: bold;
        }
    </style>
</head>
<body>
<div id="msg"><%= ErrorMsg %></div>
</body>
<script>
    var msg = document.getElementById("msg");
    if (!msg.innerHTML) {
        window.close();
    }
</script>
</html>