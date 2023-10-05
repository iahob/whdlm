var deviceWidth = document.documentElement.clientWidth;
if (deviceWidth > 750) {
    deviceWidth = 750;
}
else if (deviceWidth == 0) {
    deviceWidth = 750
}
document.documentElement.style.fontSize = deviceWidth / 7.5 + 'px';