USE WHQJTreasureDB
GO

TRUNCATE TABLE TransferReturnConfig

insert TransferReturnConfig(ReturnType,ReturnPercent,ConfigDescribe) values(	0	,	20	,	N'��ͨ���ת����ͨ���'	)
insert TransferReturnConfig(ReturnType,ReturnPercent,ConfigDescribe) values(	0	,	30	,	N'��ͨ���ת��VIP���'	)
insert TransferReturnConfig(ReturnType,ReturnPercent,ConfigDescribe) values(	0	,	40	,	N'VIP���ת����ͨ���'	)
insert TransferReturnConfig(ReturnType,ReturnPercent,ConfigDescribe) values(	0	,	50	,	N'VIP���ת��VIP���'	)