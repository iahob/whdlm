use WHQJAccountsDB

-- ϵͳ����
TRUNCATE TABLE [dbo].[SystemStatusInfo]
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'EnjoinRegister',0,N'����ϵͳά������ʱֹͣ��Ϸϵͳ��ע�������������վ������Ϣ��',N'ע�����',N'��ֵ��0-������1-�ر�',1)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'EnjoinLogon',0,N'����ϵͳά������ʱֹͣ��Ϸϵͳ�ĵ�¼������������վ������Ϣ��',N'��¼����',N'��ֵ��0-������1-�ر�',2)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJBindSpreadPresent',10,N'���ƹ���������ʯ',N'���ƹ�����',N'��ֵ��������ʯ����',3)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJEffectiveFriendGame',1,N'��Ч���Ѿ���',N'��Ч���Ѿ���',N'��ֵ���ƹ��������پ���Ϸ��Ϊ��Ч����',5)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJOpenMobileMall',0,N'�Ƿ����ֻ��̳�',N'�ֻ��̳�',N'��ֵ��0-������1-�ر�',6)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJPayBindSpread',0,N'�Ƿ��ֵ����ƹ���',N'��ֵ���ƹ�',N'��ֵ��0-������1-�ر�',7)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJPayChannel',3,N'��ֵ��������',N'��ֵ��������',N'��ֵ��0-�ޣ�1-΢�Źٷ���2-֧�����ٷ���3-΢�ź�֧�����ٷ�',8)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJRankingListType',7,N'��ʾ���а�����',N'���а�����',N'��ֵ��0-�ޣ�1-�Ƹ���2-���ģ�3-�Ƹ������ģ�4-ս����5-�Ƹ���ս��,6-���ĺ�ս��,7-�Ƹ������ĺ�ս��',9)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJRealNameAuthentPresent',0,N'��������֤����',N'��������֤����',N'��ֵ����������֤������ʯ��,0 ��ʾ��������������֤',10)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES(N'JJRegisterDiamondCount',10,N'ע��������ʯ',N'ע��������ʯ',N'��ֵ��ע������ֵ��0��ʾ������',11)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankPrerequisite'	,	10000	,	N'��ȡ��������ȡ��Ϸ�ұ�����ڴ����ſɲ�����'	,	N'��ȡ����'	,	N'��ֵ����ʾ��ȡ�����������ڴ����ſɴ�ȡ'	,	106	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankGameMax'	,	1000000000	,	N'��ȡ��������ȡ��Ϸ�ұ���С�ڴ����ſɲ�����'	,	N'��ȡ����'	,	N'��ֵ����ʾ��ȡ���������С�ڴ����ſɴ�ȡ'	,	107	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'EnjoinInsure'	,	0	,	N'����ϵͳά������ʱֹͣ��Ϸϵͳ�ı��չ������������վ������Ϣ��'	,	N'���з���'	,	N'��ֵ��0-������1-�ر�'	,	98	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'GrantScoreCount'	,	8000000	,	N'���û�ע��ϵͳ����Ϸ�ҵ���Ŀ��'	,	N'ע������'	,	N'��ֵ����ʾ���͵���Ϸ������'	,	4	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'RevenueRateTake'	,	10	,	N'ȡ���������������ʣ�ǧ�ֱȣ���'	,	N'ȡ��˰��'	,	N'��ֵ����ʾ����ȡ����������������ֵ��ǧ�ֱȣ���'	,	108	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'RevenueRateTransfer'	,	10	,	N'ת�˲�������������ʣ�ǧ�ֱȣ���'	,	N'ת��˰��'	,	N'��ֵ����ʾ��ͨ�������ת�˲��������������ֵ��ǧ�ֱȣ���'	,	109	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferMaxTax'	,	1000	,	N'ת�˷ⶥ���������'	,	N'ת�˷�������ⶥ'	,	N'��ֵ������ת�˷ⶥ���������0-���ⶥ'	,	100	)
--insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferPower'	,	1	,	N'�Ƿ����������ת��Ȩ��'	,	N'ת������Ȩ��'	,	N'��ֵ��0-������1-�ر�'	,	101	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferPrerequisite'	,	10000	,	N'ת��������ת����Ϸ����������ڴ����ſ�ת�ˣ�'	,	N'ת������'	,	N'��ֵ����ʾת�˽����������ڴ����ſ�ת��'	,	102	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferRetention'	,	0	,	N'���ٱ���'	,	N'ת�˱���'	,	N'��ֵ����ʾת�˽������ͱ�������'	,	104	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'TransferStauts'	,	1	,	N'ת�˹��ܱ��رգ���������վ����'	,	N'ת�˷���'	,	N'��ֵ��ת���Ƿ�������ֵ��0-������1-�ر�'	,	99	)
INSERT INTO SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) VALUES (N'MobileBattleRecordMask',7,N'�ֻ�����ս����ʾ����',N'����ս������',N'��ֵ��
	 1����ͨ����ս��
	 2����ҷ���ս��
	 3����ͨ�����ͽ�ҷ���ս��
	 4�������Ϸ��¼
	 5����ͨ�����ͽ����Ϸ��¼
	 6����ҷ����ͽ����Ϸ��¼��
	 7��ȫ��',350)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'CheckCodeValidTime'	,	2	,	N'������֤����Чʱ�䣬��λ����'	,	N'��֤����Чʱ��'	,	N'��ֵ��������֤����Чʱ��'	,	201	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankOPButton1'	,	100	,	N'��ȡ���1'	,	N'���д�ȡ'	,	N'��ֵ��ǰ�����д�ȡ���1'	,	110	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankOPButton2'	,	1000	,	N'��ȡ���2'	,	N'���д�ȡ'	,	N'��ֵ��ǰ�����д�ȡ���2'	,	111	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'BankOPButton3'	,	10000	,	N'��ȡ���3'	,	N'���д�ȡ'	,	N'��ֵ��ǰ�����д�ȡ���3'	,	112	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'DayWinLostScoreLimit'	,	0	,	N'���ÿ����Ӯ����'	,	N'��Ӯ����'	,	N'��ֵ����ʾ���ÿ����Ӯ�������ܳ����������������ܼ�����Ϸ��0��ʾ������'	,	202	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(	N'DayPlayTimeLimit'	,	0	,	N'���ÿ����Ϸʱ������(Сʱ)'	,	N'ʱ������'	,	N'��ֵ����ʾ���ÿ����Ϸʱ�䲻�ܳ����������������ܼ�����Ϸ��0��ʾ������'	,	203	)
insert SystemStatusInfo(StatusName,StatusValue,StatusString,StatusTip,StatusDescription,SortID) values(N'PayMoneyMax',1000,'��ֵ�������','��ֵ�������','ÿ�ճ�ֵ������ޣ�0:������',567)