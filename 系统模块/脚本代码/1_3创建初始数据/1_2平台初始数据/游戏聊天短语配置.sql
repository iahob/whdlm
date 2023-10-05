USE WHQJPlatformDB
GO

-----------------------------------------------------------------------------------------------------------
TRUNCATE TABLE [dbo].[GameChatPhrase]

INSERT [GameChatPhrase] ([SortID],[ChatPhraseString],[ChatTypeIndex],[ChatChance]) VALUES ( 1,'快点吧，等到花儿都谢了',1, 18)
INSERT [GameChatPhrase] ([SortID],[ChatPhraseString],[ChatTypeIndex],[ChatChance]) VALUES ( 2,'大家好，很高兴见到各位',2, 8)
INSERT [GameChatPhrase] ([SortID],[ChatPhraseString],[ChatTypeIndex],[ChatChance]) VALUES ( 3,'再见了，我会想念大家的',4, 12)
INSERT [GameChatPhrase] ([SortID],[ChatPhraseString],[ChatTypeIndex],[ChatChance]) VALUES ( 4,'各位真是不好意思，我要走了',4, 12)
INSERT [GameChatPhrase] ([SortID],[ChatPhraseString],[ChatTypeIndex],[ChatChance]) VALUES ( 5,'不要吵了，不要吵了，专心玩游戏吧',8, 10)
INSERT [GameChatPhrase] ([SortID],[ChatPhraseString],[ChatTypeIndex],[ChatChance]) VALUES ( 6,'不要走，决战到天亮',16, 20)
INSERT [GameChatPhrase] ([SortID],[ChatPhraseString],[ChatTypeIndex],[ChatChance]) VALUES ( 7,'又断线了，怎么网络这么差呀',32, 3)
INSERT [GameChatPhrase] ([SortID],[ChatPhraseString],[ChatTypeIndex],[ChatChance]) VALUES ( 8,'和你合作真是太愉快了',64, 3)

