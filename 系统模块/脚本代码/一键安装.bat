@echo off
echo *************************************************************************
echo                              ���������콢��һ����װ
echo     ִ�����������콢�����ݿ�һ����װ�ű����Զ�����"D:\���ݿ�\�콢ƽ̨"Ŀ¼�µ����ݿ⣬
echo �Զ��������������콢���ʼ���ݿ⡣�밴���������...
echo *************************************************************************
pause
echo.
echo �����������ݿ�
Rem ���ݿ�����
net stop mssqlserver
set Ymd=%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2% 
md D:\���ݿ�\�콢ƽ̨����\%ymd%
XCOPY D:\���ݿ�\�콢ƽ̨\* D:\���ݿ�\�콢ƽ̨����\%ymd% /s  /e
if exist D:\���ݿ�\�콢ƽ̨ (echo y|cacls D:\���ݿ�\�콢ƽ̨ /p everyone:f >nul 2>nul &&rd /s /q D:\���ݿ�\�콢ƽ̨) 
md D:\���ݿ�\�콢ƽ̨
net start mssqlserver
set rootPath=1_1�������ݿ�\
osql -E -i "%rootPath%���ݿ�ɾ��.sql"

echo.
echo �������ݿ�
set rootPath=1_1�������ݿ�\
osql -E -i "%rootPath%1_1_�û���ű�.sql"
osql -E -i "%rootPath%1_2_ƽ̨��ű�.sql"
osql -E -i "%rootPath%1_3_��ҿ�ű�.sql"
osql -E -i "%rootPath%1_4_��¼��ű�.sql"
osql -E -i "%rootPath%1_5_���ֿ�ű�.sql"
osql -E -i "%rootPath%1_6_������ű�.sql"
osql -E -i "%rootPath%1_7_Ⱥ���ű�.sql"
osql -E -i "%rootPath%1-8_�����ű�.sql"

osql -E -i "%rootPath%2_1_�û���ű�.sql"
osql -E -i "%rootPath%2_2_ƽ̨��ű�.sql"
osql -E -i "%rootPath%2_3_��ҿ�ű�.sql"
osql -E -i "%rootPath%2_4_��¼��ű�.sql"
osql -E -i "%rootPath%2_5_���ֿ�ű�.sql"
osql -E -i "%rootPath%2_6_������ű�.sql"
osql -E -i "%rootPath%2_7_Ⱥ���ű�.sql"
osql -E -i "%rootPath%2-8-�����ű�.sql"

echo.
echo �������ӷ�����
set rootPath=1_2�������ӷ�����\
osql -E -i "%rootPath%1_1�û�����.sql"
osql -E -i "%rootPath%1_2ƽ̨����.sql"
osql -E -i "%rootPath%1_3�������.sql"
osql -E -i "%rootPath%1_4��¼����.sql"
osql -E -i "%rootPath%1_5��������.sql"
osql -E -i "%rootPath%1_6��������.sql"
osql -E -i "%rootPath%1_7Ⱥ������.sql"
echo.
echo ������ʼ����
set rootPath=1_3������ʼ����\1_1�û���ʼ����\
osql -E -i "%rootPath%ϵͳ����.sql"
osql -E -i "%rootPath%��Ա����.sql"

set rootPath=1_3������ʼ����\1_2ƽ̨��ʼ����\
osql -E -i "%rootPath%���߹�ϵ.sql"
osql -E -i "%rootPath%��������.sql"
osql -E -i "%rootPath%��������.sql"
osql -E -i "%rootPath%����ȼ�����.sql"
osql -E -i "%rootPath%��������.sql"
osql -E -i "%rootPath%ǩ������.sql"
osql -E -i "%rootPath%�ӵ�������.sql"
osql -E -i "%rootPath%��Ϸ�����������.sql"
osql -E -i "%rootPath%AI �뿪����Ȩ������.sql"
osql -E -i "%rootPath%AI���뷿��Ȩ������.sql"
osql -E -i "%rootPath%AIʱ��任.sql"


set rootPath=1_3������ʼ����\1_3��ҳ�ʼ����\
osql -E -i "%rootPath%��������.sql"
osql -E -i "%rootPath%�б�����.sql"

set rootPath=1_3������ʼ����\1_5���ֳ�ʼ����\
set rootPath=1_3������ʼ����\1_6������ʼ����\
osql -E -i "%rootPath%�б�����.sql"
osql -E -i "%rootPath%�б�����.sql"

set rootPath=1_3������ʼ����\1_7���ֲ���ʼ����\
osql -E -i "%rootPath%���ֲ�����.sql"

echo.
echo �����洢����
set rootPath=1_4�����洢����\1_1�û����ݿ�\
osql -E  -i "%rootPath%�󶨻���.sql"
osql -E  -i "%rootPath%��ʶ��¼.sql"
osql -E  -i "%rootPath%�����б�.sql"
osql -E  -i "%rootPath%���Ѳ���.sql"
osql -E  -i "%rootPath%���Ѳ���.sql"
osql -E  -i "%rootPath%���ѵ�¼.sql"
osql -E  -i "%rootPath%������Ϣ.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%���ػ���.sql"
osql -E  -i "%rootPath%����Ȩ��.sql"
osql -E  -i "%rootPath%ʵ����֤.sql"
osql -E  -i "%rootPath%У��Ȩ��.sql"
osql -E  -i "%rootPath%У������.sql"
osql -E  -i "%rootPath%�޸�����.sql"
osql -E  -i "%rootPath%�޸�ǩ��.sql"
osql -E  -i "%rootPath%�û�����.sql"
osql -E  -i "%rootPath%�ʺŰ�.sql"
osql -E  -i "%rootPath%�ʺŵ�¼.sql"
osql -E  -i "%rootPath%ע���ʺ�.sql"
osql -E  -i "%rootPath%�Զ�ͷ��.sql"
osql -E  -i "%rootPath%������ˮ��.sql"
osql -E  -i "%rootPath%������AI.sql"

set rootPath=1_4�����洢����\1_2ƽ̨���ݿ�\
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%���߹���.sql"
osql -E  -i "%rootPath%����ȼ�����.sql"
osql -E  -i "%rootPath%�������.sql"
osql -E  -i "%rootPath%��Ա����.sql"
osql -E  -i "%rootPath%���ؽڵ�.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%�������д�.sql"
osql -E  -i "%rootPath%����ҳ��.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%����ʹ��.sql"
osql -E  -i "%rootPath%������Ϣ.sql"
osql -E  -i "%rootPath%������Ϣ.sql"
osql -E  -i "%rootPath%ģ�����.sql"
osql -E  -i "%rootPath%ƽ̨����.sql"
osql -E  -i "%rootPath%ǩ������.sql"
osql -E  -i "%rootPath%ʵ������.sql"
osql -E  -i "%rootPath%��Ƶ����.sql"
osql -E  -i "%rootPath%������Ϣ.sql"
osql -E  -i "%rootPath%�������.sql"
osql -E  -i "%rootPath%������ˮ��.sql"
osql -E  -i "%rootPath%����AI���ò���.sql"
osql -E  -i "%rootPath%��Ϸ�������.sql"


set rootPath=1_4�����洢����\1_3������ݿ�\
osql -E  -i "%rootPath%��ʶ��¼.sql"
osql -E  -i "%rootPath%��ѯ�û�.sql"
osql -E  -i "%rootPath%�һ�����.sql"
osql -E  -i "%rootPath%���ػ���.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%�뿪����.sql"
osql -E  -i "%rootPath%�б�����.sql"
osql -E  -i "%rootPath%����Ȩ��.sql"
osql -E  -i "%rootPath%��Ƶ����.sql"
osql -E  -i "%rootPath%�ƹ����.sql"
osql -E  -i "%rootPath%д�����.sql"
osql -E  -i "%rootPath%���з���.sql"
osql -E  -i "%rootPath%��Ϸ��¼.sql"
osql -E  -i "%rootPath%��Ϸ����.sql"
osql -E  -i "%rootPath%��Ϸд��.sql"
osql -E  -i "%rootPath%��Ϸ����.sql"
osql -E  -i "%rootPath%������ˮ��.sql"

set rootPath=1_4�����洢����\1_5�������ݿ�\
osql -E  -i "%rootPath%��ʶ��¼.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%�뿪����.sql"
osql -E  -i "%rootPath%�б�����.sql"
osql -E  -i "%rootPath%����Ȩ��.sql"
osql -E  -i "%rootPath%��Ϸ��¼.sql"
osql -E  -i "%rootPath%��Ϸд��.sql"
osql -E  -i "%rootPath%������ˮ��.sql"

set rootPath=1_4�����洢����\1_6�������ݿ�\
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%��ʶ��¼.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%��ʼ����.sql"
osql -E  -i "%rootPath%�뿪����.sql"
osql -E  -i "%rootPath%�б�����.sql"
osql -E  -i "%rootPath%����Ȩ��.sql"
osql -E  -i "%rootPath%д�����.sql"
osql -E  -i "%rootPath%д�뽱��.sql"
osql -E  -i "%rootPath%��Ϸ��¼.sql"
osql -E  -i "%rootPath%��Ϸд��.sql"
osql -E  -i "%rootPath%������ˮ��.sql"

set rootPath=1_6˽�˷���\
osql -E  -i "%rootPath%�������.sql"
osql -E  -i "%rootPath%�׷�����.sql"
osql -E  -i "%rootPath%�������.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%��������.sql"
osql -E  -i "%rootPath%������Ϣ.sql"
osql -E  -i "%rootPath%������Ϣ.sql"
osql -E  -i "%rootPath%����д��.sql"
osql -E  -i "%rootPath%�ط���Ϣ.sql"
osql -E  -i "%rootPath%���д��.sql"

set rootPath=1_7���ֲ�\
osql -E  -i "%rootPath%��Ϸ_Ⱥ���Ա.sql"
osql -E  -i "%rootPath%��Ϸ_Ⱥ�����.sql"
osql -E  -i "%rootPath%��Ϸ_Ⱥ���¼.sql"
osql -E  -i "%rootPath%��Ϸ_Ⱥ�����.sql"
osql -E  -i "%rootPath%��Ϸ_Ⱥ���б�.sql"
osql -E  -i "%rootPath%��Ϸ_�������.sql"
osql -E  -i "%rootPath%��Ϸ_�淨����.sql"
osql -E  -i "%rootPath%��Ϸ_ϵͳ����.sql"
osql -E  -i "%rootPath%��Ϸ_�û���¼.sql"
osql -E  -i "%rootPath%��Ϸ_С�ӹ���.sql"
osql -E  -i "%rootPath%��Ϸ_����״̬.sql"
osql -E  -i "%rootPath%������ˮ��.sql"
echo.
echo *************************************************************************
echo ���������콢��һ����װ�Ѿ��������������콢�����ݿ�
echo ����������Ϸ��ʶ����ֱ�ӹر�
echo ��Ҫ������Ϸ��ʶ���밴���������
echo *************************************************************************
pause

CLS
echo.
@echo ������Ϸ��ʶ
set rootPath=1_5������Ϸ��ʶ\
osql -E  -i "%rootPath%��ʶ����.sql"

CLS
@echo off
echo *************************************************************************
echo.
echo ���������콢��һ����װ�ű�������� 
echo.
echo.
echo ��Ȩ���У� �����������Ƽ����޹�˾
echo *************************************************************************

pause


pause


