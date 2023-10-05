USE [WHQJPlatformDB]
GO
-----Ç©µ½ÅäÖÃ
truncate table GamePackage
go
SET IDENTITY_INSERT [dbo].[GamePackage] ON 

INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (1, N'³é½±Ñ¡Ïî1', 0, 1, 0, N'', CAST(0x0000A92100975BFC AS DateTime), 1)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (3, N'³é½±Ñ¡Ïî2', 0, 2, 0, N'', CAST(0x0000A926011CFE13 AS DateTime), 1)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (5, N'³é½±Ñ¡Ïî3', 0, 3, 0, N'', CAST(0x0000A92701456BA2 AS DateTime), 1)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (6, N'³é½±Ñ¡Ïî4', 0, 4, 0, N'', CAST(0x0000A92800EE1833 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (7, N'³é½±Ñ¡Ïî5', 0, 5, 0, N'', CAST(0x0000A92800EE34F1 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (8, N'³é½±Ñ¡Ïî6', 0, 6, 0, N'', CAST(0x0000A92800EE3BE5 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (9, N'³é½±Ñ¡Ïî7', 0, 7, 0, N'', CAST(0x0000A92800EE42E9 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (10, N'³é½±Ñ¡Ïî8', 0, 8, 0, N'', CAST(0x0000A92800EE4C62 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (11, N'ÀÛ»ý½±Àø1', 1, 9, 0, N'', CAST(0x0000A92800EE69CF AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (12, N'ÀÛ»ý½±Àø2', 1, 10, 0, N'', CAST(0x0000A92800EE7384 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (13, N'ÀÛ»ý½±Àø3', 1, 11, 0, N'', CAST(0x0000A92800EE8064 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (14, N'ÀÛ»ý½±Àø4', 1, 12, 0, N'', CAST(0x0000A92800EE8877 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (15, N'ÀÛ»ý½±Àø5', 1, 13, 0, N'', CAST(0x0000A92800EE9078 AS DateTime), 0)
INSERT [dbo].[GamePackage] ([PackageID], [Name], [TypeID], [SortID], [Nullity], [Describe], [CollectDate], [PlatformKind]) VALUES (16, N'ÀÛ»ý½±Àø6', 1, 14, 0, N'', CAST(0x0000A92800EE9785 AS DateTime), 0)
SET IDENTITY_INSERT [dbo].[GamePackage] OFF
-----Ç©µ½ÅäÖÃ
go
truncate table GamePackageGoods
go
SET IDENTITY_INSERT [dbo].[GamePackageGoods] ON 

INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (7, 1, 0, 0, 5000, N'/Initialize/32.png', CAST(0x0000A928012CF457 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (9, 5, 3, 0, 1, N'/Initialize/9.png', CAST(0x0000A928012D35F6 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (10, 3, 0, 0, 100, N'/Initialize/8.png', CAST(0x0000A928012D561D AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (11, 7, 0, 0, 1500, N'/Initialize/19.png', CAST(0x0000A928012E819D AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (12, 6, 2, 306, 5, N'/Initialize/35.png', CAST(0x0000A928012EA058 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (13, 8, 0, 0, 5000, N'/Initialize/32.png', CAST(0x0000A928012ED5AD AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (14, 9, 3, 0, 10, N'/Initialize/14.png', CAST(0x0000A928012EEE14 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (16, 10, 0, 0, 23000, N'/Initialize/33.png', CAST(0x0000A928012F2866 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (17, 11, 0, 0, 1000, N'/Initialize/17.png', CAST(0x0000A928012F3AB0 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (18, 12, 0, 0, 10000, N'/Initialize/34.png', CAST(0x0000A928012F4E64 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (19, 13, 0, 0, 1000, N'/Initialize/19.png', CAST(0x0000A92801300593 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (20, 13, 0, 0, 10000, N'/Initialize/36(1).png', CAST(0x0000A92801301A53 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (21, 14, 3, 0, 10, N'/Initialize/21.png', CAST(0x0000A92801303134 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (24, 15, 0, 0, 15000, N'/Initialize/33.png', CAST(0x0000A928013082B9 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (25, 15, 2, 306, 20, N'/Initialize/25.png', CAST(0x0000A9280130946E AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (26, 16, 0, 0, 10000, N'/Initialize/41.png', CAST(0x0000A9280130A564 AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (27, 16, 0, 0, 110000, N'/Initialize/38(1).png', CAST(0x0000A9280130B60D AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (29, 16, 3, 0, 20, N'/Initialize/29.png', CAST(0x0000A9280130DA9A AS DateTime))
INSERT [dbo].[GamePackageGoods] ([GoodsID], [PackageID], [TypeID], [PropertyID], [GoodsNum], [ResourceURL], [CollectDate]) VALUES (30, 16, 2, 306, 20, N'/Initialize/30.png', CAST(0x0000A9280130EC7A AS DateTime))
SET IDENTITY_INSERT [dbo].[GamePackageGoods] OFF
go
-----Ç©µ½ÅäÖÃ
truncate table GameSignIn
go
SET IDENTITY_INSERT [dbo].[GameSignIn] ON 

INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (3, 0, 1, 12, 1, 1, 0, CAST(0x0000A92801310F9D AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (4, 0, 3, 12, 1, 2, 0, CAST(0x0000A92801311D70 AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (5, 0, 5, 12, 1, 3, 0, CAST(0x0000A92801312B4F AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (6, 0, 6, 16, 1, 4, 0, CAST(0x0000A928013143B1 AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (8, 0, 8, 12, 1, 6, 0, CAST(0x0000A9280131612C AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (9, 0, 7, 12, 1, 5, 0, CAST(0x0000A92A00953545 AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (10, 0, 9, 12, 1, 7, 0, CAST(0x0000A92A00955E37 AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (11, 0, 10, 12, 1, 8, 0, CAST(0x0000A92A009576EB AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (12, 1, 11, 0, 3, 1, 0, CAST(0x0000A92A0095A378 AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (13, 1, 12, 0, 5, 2, 0, CAST(0x0000A92A0095B3EC AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (14, 1, 13, 0, 7, 3, 0, CAST(0x0000A92A0095C0FE AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (15, 1, 14, 0, 14, 4, 0, CAST(0x0000A92A0095CCF7 AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (16, 1, 15, 0, 21, 5, 0, CAST(0x0000A92A0095E8C2 AS DateTime))
INSERT [dbo].[GameSignIn] ([SignID], [TypeID], [PackageID], [Probability], [NeedDay], [SortID], [Nullity], [CollectDate]) VALUES (17, 1, 16, 0, 28, 6, 0, CAST(0x0000A92A0095F2A5 AS DateTime))
-----Ç©µ½ÅäÖÃ
SET IDENTITY_INSERT [dbo].[GameSignIn] OFF
go
truncate table SigninConfig
go

INSERT [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (1, 1888)
INSERT [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (2, 2999)
INSERT [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (3, 4000)
INSERT [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (4, 8006)
INSERT [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (5, 16000)
INSERT [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (6, 32000)
INSERT [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (7, 64000)