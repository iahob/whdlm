use WHQJGroupDB
go


-- ���ֲ����� 
TRUNCATE TABLE [dbo].[IMGroupOption]
INSERT DBO.IMGroupOption(OptionName,OptionValue,OptionTip,OptionDescribe,SortID) 
VALUES(N'MaxMemberCount'	,	100, N'Ⱥ���������', N'����Ⱥ��������� ',100)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'MaxCreateGroupCount',10, N'��������',N'��������ܴ���Ⱥ�����������÷�Χ1-10',101)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateGroupTakeIngot',0,N'��������',N'����Ⱥ���������0��û������ 1�����Ľ�� 2��������ʯ 3������û�������� 4������û���ʯ����',102)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateGroupDeductIngot',0, N'��������',N'�봴���������ʹ�ã����������������ò�Ϊ0ʱ�����Ļ��߼������ʯ�����������������ͬû����������ֵ��������',103)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'MaxJoinGroupCount',10, N'��������',N'����������Ⱥ�����������÷�Χ1-10',104)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'GroupPayTypeChange',1, N'֧������',N'֧�������Ƿ�֧���޸ģ�0���������޸� 1�������޸�',106)
GO	
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'GroupRoomType',3, N'������������',N'�趨��ҿ��Դ����ķ������ͣ�1��������ҷ��� 2���������ַ��� 3��ͬʱ������ҷ����ͻ��ַ���',107)
GO	
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateConfigDeductIngot',0,N'�����淨����',N'�����淨��ʯ����',108)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'RollMsgTime',90,N'���ʱ��',N'����ƹ������ż��ʱ�䵥λ��',109)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'FixedMin',0,N'�̶���������',N'�̶���������',110)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'FixedMax',1000,N'�̶���������',N'�̶���������',111)
GO
INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'PercentMin',0,N'������������',N'������������',112)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'PercentMax',100,N'������������',N'������������',113)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreatorPoint',90,N'Ȧ����λ',N'��Ҵ���������Ĭ�ϵ�λ,��Χ0-100',114)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateSwitch',0,N'���������˰�����',N'0���رհ������������˿ɴ�����1��������������ָ����ҿɴ���',115)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreateGroup',3,N'��������ȦȨ��',N'����������Ȧ���������1����ͨ�û� 2�������û� 3��ȫ��',116)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'OpenMedalMode',3,N'����ѫ��ģʽ',N'����������ʱĬ�Ͽ���ѫ��ģʽ�����1����ͨ�û� 2�������û� 3��ȫ��',117)
GO

INSERT DBO.IMGroupOption (OptionName,OptionValue,OptionTip,OptionDescribe,SortID)
VALUES (N'CreatePresentMedal',100000,N'����ѫ��',N'����������Ĭ�����͸������˵�ѫ������',118)
GO
