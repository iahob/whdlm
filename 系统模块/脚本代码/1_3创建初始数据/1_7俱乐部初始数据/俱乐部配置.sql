use WHQJGroupDB
go


-- 俱乐部配置 
TRUNCATE TABLE [dbo].[IMGroupOption]
INSERT DBO.IMGroupOption(OptionName,OptionValue,OptionTip,OptionDescribe,SortID) 
VALUES(N'MaxMemberCount'	,	100, N'群组最大人数', N'单个群组最大人数 ',100)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'MaxCreateGroupCount',10, N'创建上限',N'单人最多能创建群组数量，配置范围1-10',101)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateGroupTakeIngot',0,N'创建条件',N'创建群组的条件：0、没有条件 1、消耗金币 2、消耗钻石 3、检测用户金币数量 4、检测用户钻石数量',102)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateGroupDeductIngot',0, N'创建消耗',N'与创建条件配合使用，当创建条件的配置不为0时，消耗或者检测金币钻石的数量，不配置则等同没有条件，键值等于数量',103)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'MaxJoinGroupCount',10, N'加入上限',N'单人最多加入群组数量，配置范围1-10',104)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'GroupPayTypeChange',1, N'支付服务',N'支付服务是否支持修改：0、不可以修改 1、可以修改',106)
GO	
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'GroupRoomType',3, N'创建房间类型',N'设定玩家可以创建的房间类型：1、开启金币房卡 2、开启积分房卡 3、同时开启金币房卡和积分房卡',107)
GO	
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateConfigDeductIngot',0,N'创建玩法条件',N'创建玩法钻石消耗',108)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'RollMsgTime',90,N'间隔时间',N'跑马灯滚动播放间隔时间单位秒',109)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'FixedMin',0,N'固定贡献下限',N'固定贡献下限',110)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'FixedMax',1000,N'固定贡献上限',N'固定贡献上限',111)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'PercentMin',0,N'比例贡献下限',N'比例贡献下限',112)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'PercentMax',100,N'比例贡献上限',N'比例贡献上限',113)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreatorPoint',90,N'圈主点位',N'玩家创建大联盟默认点位,范围0-100',114)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateSwitch',0,N'创建大联盟白名单',N'0、关闭白名单，所有人可创建，1、开启白名单，指定玩家可创建',115)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateGroup',3,N'创建亲友圈权限',N'允许创建亲友圈的玩家类型1、普通用户 2、代理用户 3、全部',116)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'OpenMedalMode',3,N'开启勋章模式',N'创建大联盟时默认开启勋章模式的玩家1、普通用户 2、代理用户 3、全部',117)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreatePresentMedal',100000,N'赠送勋章',N'创建大联盟默认赠送给该联盟的勋章数量',118)
GO
