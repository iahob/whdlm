USE WHQJTreasureDB
GO

-----------------------------------------------------------------------------------------------------------
TRUNCATE TABLE [dbo].[GameColumnItem]

INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 1,'昵称',100,3)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 2,'ID',60,1)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 3,'游戏币',90,30)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 4,'银行',90,4)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 5,'输赢情况',90,5)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 6,'类别',65,6)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 7,'桌子号',65,20)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 8,'椅子号',65,21)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 9,'在线时长',65,52)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 10,'携带等级',65,53)
INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 11,'离开等级',65,54)

--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 7,'状态',60,7)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 4,'桌号',40,20)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 5,'级别',85,44)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 6,'经验值',85,33)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 7,'魅力值',85,34)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 8,'元宝',60,32)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 9,'胜率',60,40)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 10,'逃率',60,43)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 11,'总局',60,39)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 12,'赢局',60,35)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 13,'输局',60,36)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 14,'和局',60,37)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 15,'逃局',60,38)
--INSERT [GameColumnItem] ([SortID],[ColumnName],[ColumnWidth],[DataDescribe]) VALUES ( 16,'签名',150,12)