USE [WHQJAgentDB]
GO
/****** Object:  Table [dbo].[AgentBelowInfo]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AgentBelowInfo](
	[AgentID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AgentBelowInfo] PRIMARY KEY CLUSTERED 
(
	[AgentID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[AgentInfo]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AgentInfo](
	[AgentID] [int] IDENTITY(1,1) NOT NULL,
	[ParentAgent] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[Password] [nvarchar](32) NOT NULL,
	[Compellation] [nvarchar](16) NOT NULL,
	[QQAccount] [nvarchar](32) NOT NULL,
	[WCNickName] [nvarchar](32) NOT NULL,
	[ContactPhone] [nvarchar](11) NOT NULL,
	[ContactAddress] [nvarchar](50) NOT NULL,
	[AgentDomain] [nvarchar](50) NOT NULL,
	[AgentLevel] [tinyint] NOT NULL,
	[AgentNote] [nvarchar](100) NOT NULL,
	[DiamondAward] [bigint] NOT NULL,
	[GoldAward] [bigint] NOT NULL,
	[BelowUser] [int] NOT NULL,
	[BelowAgent] [int] NOT NULL,
	[Nullity] [tinyint] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_AgentInfo] PRIMARY KEY CLUSTERED 
(
	[AgentID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[AgentTokenInfo]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[AgentTokenInfo](
	[UserID] [int] NOT NULL,
	[AgentID] [int] NOT NULL,
	[Token] [nvarchar](64) NOT NULL,
	[ExpirtAt] [datetime] NOT NULL,
 CONSTRAINT [PK_AgentTokenInfo] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC,
	[AgentID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[ReturnAwardConfig]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReturnAwardConfig](
	[ConfigID] [int] IDENTITY(1,1) NOT NULL,
	[AwardType] [tinyint] NOT NULL,
	[AwardLevel] [int] NOT NULL,
	[AwardScale] [decimal](18, 6) NOT NULL,
	[Nullity] [bit] NOT NULL,
	[UpdateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReturnAwardConfig] PRIMARY KEY CLUSTERED 
(
	[ConfigID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[ReturnAwardGrant]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReturnAwardGrant](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[SourceUserID] [int] NOT NULL,
	[TargetUserID] [int] NOT NULL,
	[TradeType] [tinyint] NOT NULL,
	[SourceBefore] [bigint] NOT NULL,
	[TargetBefore] [bigint] NOT NULL,
	[Amount] [bigint] NOT NULL,
	[ClientIP] [nvarchar](15) NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ReturnAwardGrant] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[ReturnAwardReceive]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReturnAwardReceive](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[UserID] [int] NOT NULL,
	[AwardType] [tinyint] NOT NULL,
	[ReceiveAward] [int] NOT NULL,
	[ReceiveBefore] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
 CONSTRAINT [PK_ReturnAwardReceive] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[ReturnAwardRecord]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReturnAwardRecord](
	[RecordID] [int] IDENTITY(1,1) NOT NULL,
	[SourceUserID] [int] NOT NULL,
	[TargetUserID] [int] NOT NULL,
	[ReturnBase] [bigint] NOT NULL,
	[AwardLevel] [int] NOT NULL,
	[AwardType] [tinyint] NOT NULL,
	[AwardScale] [decimal](18, 2) NOT NULL,
	[Award] [int] NOT NULL,
	[CollectDate] [datetime] NOT NULL,
	[ExtraField] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_ReturnAwardRecord] PRIMARY KEY CLUSTERED 
(
	[RecordID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[ReturnAwardSteam]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ReturnAwardSteam](
	[DateID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[AwardType] [int] NOT NULL,
	[AwardLevel] [int] NOT NULL,
	[Award] [bigint] NOT NULL,
	[InsertTime] [datetime] NOT NULL,
	[UpdateTime] [datetime] NOT NULL,
 CONSTRAINT [PK_ReturnAwardSteam] PRIMARY KEY CLUSTERED 
(
	[DateID] ASC,
	[UserID] ASC,
	[AwardType] ASC,
	[AwardLevel] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[SystemStatusInfo]    Script Date: 2018/6/11 19:38:11 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SystemStatusInfo](
	[StatusName] [nvarchar](32) NOT NULL,
	[StatusValue] [int] NOT NULL,
	[StatusString] [nvarchar](512) NOT NULL,
	[StatusTip] [nvarchar](50) NOT NULL,
	[StatusDescription] [nvarchar](100) NOT NULL,
	[SortID] [int] NOT NULL,
 CONSTRAINT [PK_SystemStatusInfo] PRIMARY KEY CLUSTERED 
(
	[StatusName] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
ALTER TABLE [dbo].[AgentBelowInfo] ADD  CONSTRAINT [DF_AgentBelowInfo_AgentID]  DEFAULT ((0)) FOR [AgentID]
GO
ALTER TABLE [dbo].[AgentBelowInfo] ADD  CONSTRAINT [DF_AgentBelowInfo_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[AgentBelowInfo] ADD  CONSTRAINT [DF_AgentBelowInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_ParentAgent]  DEFAULT ((0)) FOR [ParentAgent]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_Password]  DEFAULT ('') FOR [Password]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_Compellation]  DEFAULT ('') FOR [Compellation]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_QQAccount]  DEFAULT ('') FOR [QQAccount]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_WCNickName]  DEFAULT ('') FOR [WCNickName]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_ContactPhone]  DEFAULT ('') FOR [ContactPhone]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_ContactAddress]  DEFAULT ('') FOR [ContactAddress]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_AgentDomain]  DEFAULT ('') FOR [AgentDomain]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_AgentLevel]  DEFAULT ((0)) FOR [AgentLevel]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_AgentNote]  DEFAULT ('') FOR [AgentNote]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_DiamondAward]  DEFAULT ((0)) FOR [DiamondAward]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_GoldAward]  DEFAULT ((0)) FOR [GoldAward]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_BelowUser]  DEFAULT ((0)) FOR [BelowUser]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_BelowAgent]  DEFAULT ((0)) FOR [BelowAgent]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[AgentInfo] ADD  CONSTRAINT [DF_AgentInfo_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[AgentTokenInfo] ADD  CONSTRAINT [DF_AgentTokenInfo_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[AgentTokenInfo] ADD  CONSTRAINT [DF_AgentTokenInfo_AgentID]  DEFAULT ((0)) FOR [AgentID]
GO
ALTER TABLE [dbo].[AgentTokenInfo] ADD  CONSTRAINT [DF_AgentTokenInfo_Token]  DEFAULT (N'') FOR [Token]
GO
ALTER TABLE [dbo].[AgentTokenInfo] ADD  CONSTRAINT [DF_AgentTokenInfo_ExpirtAt]  DEFAULT (getdate()+(1)) FOR [ExpirtAt]
GO
ALTER TABLE [dbo].[ReturnAwardConfig] ADD  CONSTRAINT [DF_ReturnAwardConfig_AwardType]  DEFAULT ((0)) FOR [AwardType]
GO
ALTER TABLE [dbo].[ReturnAwardConfig] ADD  CONSTRAINT [DF_ReturnAwardConfig_AwardLevel]  DEFAULT ((0)) FOR [AwardLevel]
GO
ALTER TABLE [dbo].[ReturnAwardConfig] ADD  CONSTRAINT [DF_ReturnAwardConfig_AwardScale]  DEFAULT ((0)) FOR [AwardScale]
GO
ALTER TABLE [dbo].[ReturnAwardConfig] ADD  CONSTRAINT [DF_ReturnAwardConfig_Nullity]  DEFAULT ((0)) FOR [Nullity]
GO
ALTER TABLE [dbo].[ReturnAwardConfig] ADD  CONSTRAINT [DF_ReturnAwardConfig_UpdateTime]  DEFAULT (getdate()) FOR [UpdateTime]
GO
ALTER TABLE [dbo].[ReturnAwardGrant] ADD  CONSTRAINT [DF_ReturnAwardGrant_SourceUserID]  DEFAULT ((0)) FOR [SourceUserID]
GO
ALTER TABLE [dbo].[ReturnAwardGrant] ADD  CONSTRAINT [DF_ReturnAwardGrant_TargetUserID]  DEFAULT ((0)) FOR [TargetUserID]
GO
ALTER TABLE [dbo].[ReturnAwardGrant] ADD  CONSTRAINT [DF_ReturnAwardGrant_TradeType]  DEFAULT ((0)) FOR [TradeType]
GO
ALTER TABLE [dbo].[ReturnAwardGrant] ADD  CONSTRAINT [DF_ReturnAwardGrant_SourceBefore]  DEFAULT ((0)) FOR [SourceBefore]
GO
ALTER TABLE [dbo].[ReturnAwardGrant] ADD  CONSTRAINT [DF_ReturnAwardGrant_TargetBefore]  DEFAULT ((0)) FOR [TargetBefore]
GO
ALTER TABLE [dbo].[ReturnAwardGrant] ADD  CONSTRAINT [DF_ReturnAwardGrant_Amount]  DEFAULT ((0)) FOR [Amount]
GO
ALTER TABLE [dbo].[ReturnAwardGrant] ADD  CONSTRAINT [DF_ReturnAwardGrant_ClientIP]  DEFAULT (N'') FOR [ClientIP]
GO
ALTER TABLE [dbo].[ReturnAwardGrant] ADD  CONSTRAINT [DF_ReturnAwardGrant_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ReturnAwardReceive] ADD  CONSTRAINT [DF_ReturnAwardReceive_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[ReturnAwardReceive] ADD  CONSTRAINT [DF_ReturnAwardReceive_AwardType]  DEFAULT ((0)) FOR [AwardType]
GO
ALTER TABLE [dbo].[ReturnAwardReceive] ADD  CONSTRAINT [DF_ReturnAwardReceive_ReceiveAward]  DEFAULT ((0)) FOR [ReceiveAward]
GO
ALTER TABLE [dbo].[ReturnAwardReceive] ADD  CONSTRAINT [DF_ReturnAwardReceive_ReceiveBefore]  DEFAULT ((0)) FOR [ReceiveBefore]
GO
ALTER TABLE [dbo].[ReturnAwardReceive] ADD  CONSTRAINT [DF_ReturnAwardReceive_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_SourceUserID]  DEFAULT ((0)) FOR [SourceUserID]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_TargetUserID]  DEFAULT ((0)) FOR [TargetUserID]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_ReturnBase]  DEFAULT ((0)) FOR [ReturnBase]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_AwardLevel]  DEFAULT ((0)) FOR [AwardLevel]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_AwardType]  DEFAULT ((0)) FOR [AwardType]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_AwardScale]  DEFAULT ((0)) FOR [AwardScale]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_Award]  DEFAULT ((0)) FOR [Award]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_CollectDate]  DEFAULT (getdate()) FOR [CollectDate]
GO
ALTER TABLE [dbo].[ReturnAwardRecord] ADD  CONSTRAINT [DF_ReturnAwardRecord_ExtraField]  DEFAULT (N'') FOR [ExtraField]
GO
ALTER TABLE [dbo].[ReturnAwardSteam] ADD  CONSTRAINT [DF_ReturnAwardSteam_DateID]  DEFAULT ((0)) FOR [DateID]
GO
ALTER TABLE [dbo].[ReturnAwardSteam] ADD  CONSTRAINT [DF_ReturnAwardSteam_UserID]  DEFAULT ((0)) FOR [UserID]
GO
ALTER TABLE [dbo].[ReturnAwardSteam] ADD  CONSTRAINT [DF_ReturnAwardSteam_AwardType]  DEFAULT ((0)) FOR [AwardType]
GO
ALTER TABLE [dbo].[ReturnAwardSteam] ADD  CONSTRAINT [DF_ReturnAwardSteam_AwardLevel]  DEFAULT ((0)) FOR [AwardLevel]
GO
ALTER TABLE [dbo].[ReturnAwardSteam] ADD  CONSTRAINT [DF_ReturnAwardSteam_Award]  DEFAULT ((0)) FOR [Award]
GO
ALTER TABLE [dbo].[ReturnAwardSteam] ADD  CONSTRAINT [DF_ReturnAwardSteam_InsertTime]  DEFAULT (getdate()) FOR [InsertTime]
GO
ALTER TABLE [dbo].[ReturnAwardSteam] ADD  CONSTRAINT [DF_ReturnAwardSteam_UpdateTime]  DEFAULT (getdate()) FOR [UpdateTime]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusValue]  DEFAULT ((0)) FOR [StatusValue]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusString]  DEFAULT ('') FOR [StatusString]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusTip]  DEFAULT ('') FOR [StatusTip]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_StatusDescription]  DEFAULT ('') FOR [StatusDescription]
GO
ALTER TABLE [dbo].[SystemStatusInfo] ADD  CONSTRAINT [DF_SystemStatusInfo_SortID]  DEFAULT ((0)) FOR [SortID]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�����ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentBelowInfo', @level2type=N'COLUMN',@level2name=N'AgentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���������û���ʶ��������������һ����¼�����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentBelowInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�ռ�ʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentBelowInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�����ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'AgentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���������ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'ParentAgent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�û���ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ȫ����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'Password'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ʵ����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'Compellation'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'QQ�˺�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'QQAccount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'΢���ǳ�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'WCNickName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ϵ�绰' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'ContactPhone'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ϵ��ַ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'ContactAddress'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��������' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'AgentDomain'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ȼ�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'AgentLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ע' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'AgentNote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ʯ����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'DiamondAward'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ҷ���' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'GoldAward'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���������' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'BelowUser'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���ߴ�����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'BelowAgent'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���ñ�ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�޸�ʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentInfo', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�û���ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentTokenInfo', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�����ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentTokenInfo', @level2type=N'COLUMN',@level2name=N'AgentID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��֤����SHA256��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentTokenInfo', @level2type=N'COLUMN',@level2name=N'Token'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'AgentTokenInfo', @level2type=N'COLUMN',@level2name=N'ExpirtAt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�������ñ�ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardConfig', @level2type=N'COLUMN',@level2name=N'ConfigID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�������ͣ�1����ֵ��������ʯ����2����Ϸ���������������ҡ���' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardConfig', @level2type=N'COLUMN',@level2name=N'AwardType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��������Ŀǰ��֧��3����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardConfig', @level2type=N'COLUMN',@level2name=N'AwardLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��������' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardConfig', @level2type=N'COLUMN',@level2name=N'AwardScale'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�Ƿ����ã�0�����á�1�����ã�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardConfig', @level2type=N'COLUMN',@level2name=N'Nullity'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardConfig', @level2type=N'COLUMN',@level2name=N'UpdateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ת����¼��ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ת�������û���ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'SourceUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ת�뷽�û���ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'TargetUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ת�����ͣ� ͬ�������� 1����ֵ��������ʯ�� 2����Ϸ���������������ҡ�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'TradeType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ת����ԭֵ �������Ͳ�ͬ 1������ת��ǰ��ʯ 2������ת��ǰЯ�����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'SourceBefore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ת�뷽ԭֵ �������Ͳ�ͬ 1������ת��ǰ��ʯ 2������ת��ǰЯ�����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'TargetBefore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ת����ֵ�� �������� 1��������ʯ 2��������' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'Amount'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'������ַ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'ClientIP'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardGrant', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��¼��ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardReceive', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�û���ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardReceive', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�������ͣ�1����ֵ��������ʯ����2����Ϸ���������������ҡ���' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardReceive', @level2type=N'COLUMN',@level2name=N'AwardType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ȡ����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardReceive', @level2type=N'COLUMN',@level2name=N'ReceiveAward'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ȡǰ��Ӧ�ֶ�ֵ ���ݷ������ͣ�1����ֵ��������ʯ����2����Ϸ���������������ҡ���' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardReceive', @level2type=N'COLUMN',@level2name=N'ReceiveBefore'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��ȡʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardReceive', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��¼��ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'RecordID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ԭ�û�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'SourceUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�����û�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'TargetUserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'������׼' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'ReturnBase'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�����ƹ㼶��Ŀǰ��֧��3����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'AwardLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�������ͣ�1����ֵ��������ʯ����2����Ϸ���������������ҡ���' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'AwardType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��������' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'AwardScale'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ֵ �����ݷ������ͣ�1����ֵ��������ʯ����2����Ϸ���������������ҡ�����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'Award'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��¼ʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'CollectDate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��չ�ֶΣ���ֵ������¼��ֵ����ֵ�����š�����Ϸ�������������¼����Ϸ����-�������ơ�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardRecord', @level2type=N'COLUMN',@level2name=N'ExtraField'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ʱ���ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardSteam', @level2type=N'COLUMN',@level2name=N'DateID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�û���ʶ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardSteam', @level2type=N'COLUMN',@level2name=N'UserID'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�������ͣ�1����ֵ��������ʯ����2����Ϸ���������������ҡ���' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardSteam', @level2type=N'COLUMN',@level2name=N'AwardType'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��������' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardSteam', @level2type=N'COLUMN',@level2name=N'AwardLevel'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ֵ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardSteam', @level2type=N'COLUMN',@level2name=N'Award'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardSteam', @level2type=N'COLUMN',@level2name=N'InsertTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'����ʱ��' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'ReturnAwardSteam', @level2type=N'COLUMN',@level2name=N'UpdateTime'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'״̬����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusName'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'״̬��ֵ' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusValue'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'״̬�ַ�' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusString'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'״̬��ʾ����' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusTip'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'״̬������' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'SystemStatusInfo', @level2type=N'COLUMN',@level2name=N'StatusDescription'
GO