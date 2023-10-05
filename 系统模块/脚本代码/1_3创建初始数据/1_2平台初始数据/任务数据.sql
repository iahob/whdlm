truncate table TaskInfo
go
SET IDENTITY_INSERT [dbo].[TaskInfo] ON 

INSERT [dbo].[TaskInfo] ([TaskID], [TaskName], [TaskDescription], [TaskType], [UserType], [TimeType], [KindID], [ServerID], [MatchID], [TaskAward], [Innings], [Nullity], [CollectDate]) VALUES (2, N'广东麻将', N'每日连胜任务', 8, 0, 1, 200, 0, 0, N'1:50;2:10;3:50', 3, 0, CAST(0x0000A93600A30838 AS DateTime))
INSERT [dbo].[TaskInfo] ([TaskID], [TaskName], [TaskDescription], [TaskType], [UserType], [TimeType], [KindID], [ServerID], [MatchID], [TaskAward], [Innings], [Nullity], [CollectDate]) VALUES (3, N'完成5局', N'完成5局游戏', 1, 0, 2, 0, 0, 0, N'1:2000;2:10;3:2', 5, 0, CAST(0x0000A9360118D643 AS DateTime))
INSERT [dbo].[TaskInfo] ([TaskID], [TaskName], [TaskDescription], [TaskType], [UserType], [TimeType], [KindID], [ServerID], [MatchID], [TaskAward], [Innings], [Nullity], [CollectDate]) VALUES (4, N'每日胜局任务', N'胜局任务', 2, 0, 2, 0, 0, 0, N'1:2000;2:10', 3, 0, CAST(0x0000A93601209C32 AS DateTime))
INSERT [dbo].[TaskInfo] ([TaskID], [TaskName], [TaskDescription], [TaskType], [UserType], [TimeType], [KindID], [ServerID], [MatchID], [TaskAward], [Innings], [Nullity], [CollectDate]) VALUES (5, N'斗地主首胜任务', N'首胜任务', 4, 0, 2, 200, 0, 0, N'1:2000;2:10', 1, 0, CAST(0x0000A9360120BF72 AS DateTime))
INSERT [dbo].[TaskInfo] ([TaskID], [TaskName], [TaskDescription], [TaskType], [UserType], [TimeType], [KindID], [ServerID], [MatchID], [TaskAward], [Innings], [Nullity], [CollectDate]) VALUES (6, N'局数任务', N'局数任务', 1, 0, 2, 0, 0, 0, N'1:2000;2:20', 3, 0, CAST(0x0000A93601213487 AS DateTime))
INSERT [dbo].[TaskInfo] ([TaskID], [TaskName], [TaskDescription], [TaskType], [UserType], [TimeType], [KindID], [ServerID], [MatchID], [TaskAward], [Innings], [Nullity], [CollectDate]) VALUES (7, N'连胜任务', N'连胜任务111', 8, 0, 2, 200, 0, 0, N'1:2000;2:10', 3, 0, CAST(0x0000A9360132C89D AS DateTime))
SET IDENTITY_INSERT [dbo].[TaskInfo] OFF
go