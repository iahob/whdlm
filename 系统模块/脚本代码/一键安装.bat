@echo off
echo *************************************************************************
echo                              网狐棋牌旗舰版一键安装
echo     执行网狐棋牌旗舰版数据库一键安装脚本，自动备份"D:\数据库\旗舰平台"目录下的数据库，
echo 自动创建网狐棋牌旗舰版初始数据库。请按任意键继续...
echo *************************************************************************
pause
echo.
echo 正在清理数据库
Rem 数据库重置
net stop mssqlserver
set Ymd=%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2% 
md D:\数据库\旗舰平台备份\%ymd%
XCOPY D:\数据库\旗舰平台\* D:\数据库\旗舰平台备份\%ymd% /s  /e
if exist D:\数据库\旗舰平台 (echo y|cacls D:\数据库\旗舰平台 /p everyone:f >nul 2>nul &&rd /s /q D:\数据库\旗舰平台) 
md D:\数据库\旗舰平台
net start mssqlserver
set rootPath=1_1创建数据库\
osql -E -i "%rootPath%数据库删除.sql"

echo.
echo 建立数据库
set rootPath=1_1创建数据库\
osql -E -i "%rootPath%1_1_用户库脚本.sql"
osql -E -i "%rootPath%1_2_平台库脚本.sql"
osql -E -i "%rootPath%1_3_金币库脚本.sql"
osql -E -i "%rootPath%1_4_记录库脚本.sql"
osql -E -i "%rootPath%1_5_积分库脚本.sql"
osql -E -i "%rootPath%1_6_比赛库脚本.sql"
osql -E -i "%rootPath%1_7_群组库脚本.sql"
osql -E -i "%rootPath%1-8_代理库脚本.sql"

osql -E -i "%rootPath%2_1_用户库脚本.sql"
osql -E -i "%rootPath%2_2_平台库脚本.sql"
osql -E -i "%rootPath%2_3_金币库脚本.sql"
osql -E -i "%rootPath%2_4_记录库脚本.sql"
osql -E -i "%rootPath%2_5_积分库脚本.sql"
osql -E -i "%rootPath%2_6_比赛库脚本.sql"
osql -E -i "%rootPath%2_7_群组库脚本.sql"
osql -E -i "%rootPath%2-8-代理库脚本.sql"

echo.
echo 建立链接服务器
set rootPath=1_2创建链接服务器\
osql -E -i "%rootPath%1_1用户链接.sql"
osql -E -i "%rootPath%1_2平台链接.sql"
osql -E -i "%rootPath%1_3金币链接.sql"
osql -E -i "%rootPath%1_4记录链接.sql"
osql -E -i "%rootPath%1_5积分链接.sql"
osql -E -i "%rootPath%1_6比赛链接.sql"
osql -E -i "%rootPath%1_7群组链接.sql"
echo.
echo 建立初始数据
set rootPath=1_3创建初始数据\1_1用户初始数据\
osql -E -i "%rootPath%系统配置.sql"
osql -E -i "%rootPath%会员配置.sql"

set rootPath=1_3创建初始数据\1_2平台初始数据\
osql -E -i "%rootPath%道具关系.sql"
osql -E -i "%rootPath%道具类型.sql"
osql -E -i "%rootPath%道具配置.sql"
osql -E -i "%rootPath%房间等级配置.sql"
osql -E -i "%rootPath%类型配置.sql"
osql -E -i "%rootPath%签到配置.sql"
osql -E -i "%rootPath%子道具配置.sql"
osql -E -i "%rootPath%游戏聊天短语配置.sql"
osql -E -i "%rootPath%AI 离开房间权重配置.sql"
osql -E -i "%rootPath%AI进入房间权重配置.sql"
osql -E -i "%rootPath%AI时间变换.sql"


set rootPath=1_3创建初始数据\1_3金币初始数据\
osql -E -i "%rootPath%返利配置.sql"
osql -E -i "%rootPath%列表配置.sql"

set rootPath=1_3创建初始数据\1_5积分初始数据\
set rootPath=1_3创建初始数据\1_6比赛初始数据\
osql -E -i "%rootPath%列表配置.sql"
osql -E -i "%rootPath%列表配置.sql"

set rootPath=1_3创建初始数据\1_7俱乐部初始数据\
osql -E -i "%rootPath%俱乐部配置.sql"

echo.
echo 建立存储过程
set rootPath=1_4创建存储过程\1_1用户数据库\
osql -E  -i "%rootPath%绑定机器.sql"
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%代理列表.sql"
osql -E  -i "%rootPath%好友操作.sql"
osql -E  -i "%rootPath%好友查找.sql"
osql -E  -i "%rootPath%好友登录.sql"
osql -E  -i "%rootPath%好友消息.sql"
osql -E  -i "%rootPath%机器管理.sql"
osql -E  -i "%rootPath%加载机器.sql"
osql -E  -i "%rootPath%设置权限.sql"
osql -E  -i "%rootPath%实名验证.sql"
osql -E  -i "%rootPath%校验权限.sql"
osql -E  -i "%rootPath%校验资料.sql"
osql -E  -i "%rootPath%修改密码.sql"
osql -E  -i "%rootPath%修改签名.sql"
osql -E  -i "%rootPath%用户资料.sql"
osql -E  -i "%rootPath%帐号绑定.sql"
osql -E  -i "%rootPath%帐号登录.sql"
osql -E  -i "%rootPath%注册帐号.sql"
osql -E  -i "%rootPath%自定头像.sql"
osql -E  -i "%rootPath%生成流水号.sql"
osql -E  -i "%rootPath%大联盟AI.sql"

set rootPath=1_4创建存储过程\1_2平台数据库\
osql -E  -i "%rootPath%背包管理.sql"
osql -E  -i "%rootPath%道具管理.sql"
osql -E  -i "%rootPath%房间等级描述.sql"
osql -E  -i "%rootPath%房间管理.sql"
osql -E  -i "%rootPath%会员管理.sql"
osql -E  -i "%rootPath%加载节点.sql"
osql -E  -i "%rootPath%加载类型.sql"
osql -E  -i "%rootPath%加载敏感词.sql"
osql -E  -i "%rootPath%加载页面.sql"
osql -E  -i "%rootPath%加载种类.sql"
osql -E  -i "%rootPath%喇叭使用.sql"
osql -E  -i "%rootPath%连接信息.sql"
osql -E  -i "%rootPath%加载消息.sql"
osql -E  -i "%rootPath%模块管理.sql"
osql -E  -i "%rootPath%平台配置.sql"
osql -E  -i "%rootPath%签到管理.sql"
osql -E  -i "%rootPath%实名配置.sql"
osql -E  -i "%rootPath%视频管理.sql"
osql -E  -i "%rootPath%在线信息.sql"
osql -E  -i "%rootPath%任务管理.sql"
osql -E  -i "%rootPath%生成流水号.sql"
osql -E  -i "%rootPath%加载AI配置参数.sql"
osql -E  -i "%rootPath%游戏聊天短语.sql"


set rootPath=1_4创建存储过程\1_3金币数据库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%查询用户.sql"
osql -E  -i "%rootPath%兑换管理.sql"
osql -E  -i "%rootPath%加载机器.sql"
osql -E  -i "%rootPath%机器配置.sql"
osql -E  -i "%rootPath%加载配置.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%列表描述.sql"
osql -E  -i "%rootPath%设置权限.sql"
osql -E  -i "%rootPath%视频服务.sql"
osql -E  -i "%rootPath%推广管理.sql"
osql -E  -i "%rootPath%写入费用.sql"
osql -E  -i "%rootPath%银行服务.sql"
osql -E  -i "%rootPath%游戏记录.sql"
osql -E  -i "%rootPath%游戏数据.sql"
osql -E  -i "%rootPath%游戏写分.sql"
osql -E  -i "%rootPath%游戏任务.sql"
osql -E  -i "%rootPath%生成流水号.sql"

set rootPath=1_4创建存储过程\1_5积分数据库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%加载配置.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%列表描述.sql"
osql -E  -i "%rootPath%设置权限.sql"
osql -E  -i "%rootPath%游戏记录.sql"
osql -E  -i "%rootPath%游戏写分.sql"
osql -E  -i "%rootPath%生成流水号.sql"

set rootPath=1_4创建存储过程\1_6比赛数据库\
osql -E  -i "%rootPath%比赛管理.sql"
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%加载配置.sql"
osql -E  -i "%rootPath%开始结束.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%列表描述.sql"
osql -E  -i "%rootPath%设置权限.sql"
osql -E  -i "%rootPath%写入费用.sql"
osql -E  -i "%rootPath%写入奖励.sql"
osql -E  -i "%rootPath%游戏记录.sql"
osql -E  -i "%rootPath%游戏写分.sql"
osql -E  -i "%rootPath%生成流水号.sql"

set rootPath=1_6私人房间\
osql -E  -i "%rootPath%房间费用.sql"
osql -E  -i "%rootPath%底分配置.sql"
osql -E  -i "%rootPath%房间管理.sql"
osql -E  -i "%rootPath%房间配置.sql"
osql -E  -i "%rootPath%房卡管理.sql"
osql -E  -i "%rootPath%房卡信息.sql"
osql -E  -i "%rootPath%参与信息.sql"
osql -E  -i "%rootPath%积分写分.sql"
osql -E  -i "%rootPath%回放信息.sql"
osql -E  -i "%rootPath%金币写分.sql"

set rootPath=1_7俱乐部\
osql -E  -i "%rootPath%游戏_群组成员.sql"
osql -E  -i "%rootPath%游戏_群组管理.sql"
osql -E  -i "%rootPath%游戏_群组记录.sql"
osql -E  -i "%rootPath%游戏_群组结算.sql"
osql -E  -i "%rootPath%游戏_群组列表.sql"
osql -E  -i "%rootPath%游戏_申请管理.sql"
osql -E  -i "%rootPath%游戏_玩法管理.sql"
osql -E  -i "%rootPath%游戏_系统配置.sql"
osql -E  -i "%rootPath%游戏_用户登录.sql"
osql -E  -i "%rootPath%游戏_小队管理.sql"
osql -E  -i "%rootPath%游戏_更新状态.sql"
osql -E  -i "%rootPath%生成流水号.sql"
echo.
echo *************************************************************************
echo 网狐棋牌旗舰版一键安装已经建立网狐棋牌旗舰版数据库
echo 无需生成游戏标识，请直接关闭
echo 需要生成游戏标识，请按任意键继续
echo *************************************************************************
pause

CLS
echo.
@echo 建立游戏标识
set rootPath=1_5创建游戏标识\
osql -E  -i "%rootPath%标识生成.sql"

CLS
@echo off
echo *************************************************************************
echo.
echo 网狐棋牌旗舰版一键安装脚本建立完成 
echo.
echo.
echo 版权所有： 深圳市网狐科技有限公司
echo *************************************************************************

pause


pause


