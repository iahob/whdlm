use WHQJAccountsDB

-- 系统设置
TRUNCATE TABLE [dbo].[SystemStatusInfo]
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'EnjoinRegister',0,N'由于系统维护，暂时停止游戏系统的注册服务，请留意网站公告信息！',N'注册服务',N'键值：0-开启，1-关闭',1)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'EnjoinLogon',0,N'由于系统维护，暂时停止游戏系统的登录服务，请留意网站公告信息！',N'登录服务',N'键值：0-开启，1-关闭',2)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJBindSpreadPresent',10,N'绑定推广人赠送钻石',N'绑定推广赠送',N'键值：赠送钻石数量',3)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJEffectiveFriendGame',1,N'有效好友局数',N'有效好友局数',N'键值：推广玩家玩多少局游戏成为有效好友',5)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJOpenMobileMall',0,N'是否开启手机商城',N'手机商城',N'键值：0-开启，1-关闭',6)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJPayBindSpread',0,N'是否充值需绑定推广人',N'充值绑定推广',N'键值：0-开启，1-关闭',7)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJPayChannel',3,N'充值渠道类型',N'充值渠道类型',N'键值：0-无，1-微信官方，2-支付宝官方，3-微信和支付宝官方',8)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJRankingListType',7,N'显示排行榜类型',N'排行榜类型',N'键值：0-无，1-财富，2-消耗，3-财富和消耗，4-战绩，5-财富和战绩,6-消耗和战绩,7-财富和消耗和战绩',9)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJRealNameAuthentPresent',0,N'防沉迷认证奖励',N'防沉迷认证奖励',N'键值：防沉迷认证奖励钻石数,0 表示不开启防沉迷认证',10)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJRegisterDiamondCount',10,N'注册赠送钻石',N'注册赠送钻石',N'键值：注册赠送值，0表示不赠送',11)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankPrerequisite'	,	10000	,	N'存取条件，存取游戏币必须大于此数才可操作！'	,	N'存取条件'	,	N'键值：表示存取金币数必须大于此数才可存取'	,	106	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankGameMax'	,	1000000000	,	N'存取条件，存取游戏币必须小于此数才可操作！'	,	N'存取条件'	,	N'键值：表示存取金币数必须小于此数才可存取'	,	107	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'EnjoinInsure'	,	0	,	N'由于系统维护，暂时停止游戏系统的保险柜服务，请留意网站公告信息！'	,	N'银行服务'	,	N'键值：0-开启，1-关闭'	,	98	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'GrantScoreCount'	,	8000000	,	N'新用户注册系统送游戏币的数目！'	,	N'注册赠送'	,	N'键值：表示赠送的游戏币数量'	,	4	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'RevenueRateTake'	,	10	,	N'取款操作服务比例比率（千分比）！'	,	N'取款税率'	,	N'键值：表示银行取款操作服务比例比率值（千分比）！'	,	108	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'RevenueRateTransfer'	,	10	,	N'转账操作服务比例比率（千分比）！'	,	N'转账税率'	,	N'键值：表示普通玩家银行转账操作服务比例比率值（千分比）！'	,	109	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferMaxTax'	,	1000	,	N'转账封顶服务比例！'	,	N'转账服务比例封顶'	,	N'键值：银行转账封顶服务比例，0-不封顶'	,	100	)
--insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferPower'	,	1	,	N'是否开启所有玩家转账权限'	,	N'转账所有权限'	,	N'键值：0-开启，1-关闭'	,	101	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferPrerequisite'	,	10000	,	N'转账条件，转账游戏币数必须大于此数才可转账！'	,	N'转账条件'	,	N'键值：表示转账金币数必须大于此数才可转账'	,	102	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferRetention'	,	0	,	N'至少保留'	,	N'转账保留'	,	N'键值：表示转账金币数最低保留多少'	,	104	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferStauts'	,	1	,	N'转账功能被关闭，请留意网站公告'	,	N'转账服务'	,	N'键值：转账是否开启，键值：0-开启，1-关闭'	,	99	)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES (N'MobileBattleRecordMask',7,N'手机大厅战绩显示类型',N'大厅战绩类型',N'键值：
	 1：普通房卡战绩
	 2：金币房卡战绩
	 3：普通房卡和金币房卡战绩
	 4：金币游戏记录
	 5：普通房卡和金币游戏记录
	 6：金币房卡和金币游戏记录；
	 7：全部',350)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'CheckCodeValidTime'	,	2	,	N'短信验证码有效时间，单位分钟'	,	N'验证码有效时间'	,	N'键值：短信验证码有效时间'	,	201	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankOPButton1'	,	100	,	N'存取额度1'	,	N'银行存取'	,	N'键值：前端银行存取额度1'	,	110	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankOPButton2'	,	1000	,	N'存取额度2'	,	N'银行存取'	,	N'键值：前端银行存取额度2'	,	111	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankOPButton3'	,	10000	,	N'存取额度3'	,	N'银行存取'	,	N'键值：前端银行存取额度3'	,	112	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'DayWinLostScoreLimit'	,	0	,	N'玩家每天输赢限制'	,	N'输赢上限'	,	N'键值：表示玩家每日输赢分数不能超过此数，超过不能继续游戏，0表示不限制'	,	202	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'DayPlayTimeLimit'	,	0	,	N'玩家每天游戏时间上限(小时)'	,	N'时长上限'	,	N'键值：表示玩家每日游戏时间不能超过此数，超过不能继续游戏，0表示不限制'	,	203	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(N'PayMoneyMax',1000,'充值额度上限','充值额度上限','每日充值额度上限，0:不限制',567)