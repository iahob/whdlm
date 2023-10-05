
USE master
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJAccountsDB')
DROP DATABASE [WHQJAccountsDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJAgentDB')
DROP DATABASE [WHQJAgentDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJGameMatchDB')
DROP DATABASE [WHQJGameMatchDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJGameScoreDB')
DROP DATABASE [WHQJGameScoreDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJPlatformDB')
DROP DATABASE [WHQJPlatformDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJRecordDB')
DROP DATABASE [WHQJRecordDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJTreasureDB')
DROP DATABASE [WHQJTreasureDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJEducateDB')
DROP DATABASE [WHQJEducateDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHQJGroupDB')
DROP DATABASE [WHQJGroupDB]
GO