#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

//�����ļ�
#include "Macro.h"
#include "Define.h"

//�ṹ�ļ�
#include "Struct.h"
#include "Packet.h"
#include "Property.h"

//ģ���ļ�
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//////////////////////////////////////////////////////////////////////////////////

//����汾
#define VERSION_FRAME				PROCESS_VERSION(7,0,1)				//��ܰ汾
#define VERSION_PLAZA				PROCESS_VERSION(7,0,1)				//�����汾
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(7,0,1)				//�ֻ��汾
#define VERSION_MOBILE_IOS			PROCESS_VERSION(7,0,1)				//�ֻ��汾

//�汾����
#define VERSION_EFFICACY			0									//Ч��汾
#define VERSION_FRAME_SDK			INTERFACE_VERSION(7,1)				//��ܰ汾

//////////////////////////////////////////////////////////////////////////////////
//�����汾

#ifndef _DEBUG

//ƽ̨����
const TCHAR szProduct[]=TEXT("��A����");							//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("WHHEBWHQPGamePlaza");						//�㳡����
const TCHAR szProductKey[]=TEXT("WHHEBWHQPGamePlatform");					//��Ʒ����

//��ַ����
const TCHAR szCookieUrl[]=TEXT("http://jh.hm998.cn");					//��¼��ַ
const TCHAR szLogonServer[]=TEXT("jh.hm998.cn");						//��¼��ַ
const TCHAR szPlatformLink[]=TEXT("http://jh.hm998.cn");				//ƽ̨��վ
const TCHAR szValidateKey[]=TEXT("RYSyncLoginKey");						//��֤��Կ
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //��֤��ַ 

#else

//////////////////////////////////////////////////////////////////////////////////
//�ڲ�汾

//ƽ̨����
const TCHAR szProduct[]=TEXT("���������콢ƽ̨");						//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("WHQJGamePlaza");						//�㳡����
const TCHAR szProductKey[]=TEXT("WHQJGamePlatform");					//��Ʒ����

//��ַ����
const TCHAR szCookieUrl[]=TEXT("http://ry.foxuc.net");					//��¼��ַ
const TCHAR szLogonServer[]=TEXT("ry.foxuc.net");						//��¼��ַ
const TCHAR szPlatformLink[]=TEXT("http://ry.foxuc.net");				//ƽ̨��վ
const TCHAR szValidateKey[]=TEXT("RYSyncLoginKey");						//��֤��Կ
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //��֤��ַ 

#endif

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
const TCHAR szPlatformDB[]=TEXT("WHQJPlatformDB");						//ƽ̨���ݿ�
const TCHAR szAccountsDB[]=TEXT("WHQJAccountsDB");						//�û����ݿ�
const TCHAR szTreasureDB[]=TEXT("WHQJTreasureDB");						//�Ƹ����ݿ�
const TCHAR szGameMatchDB[]=TEXT("WHQJGameMatchDB");					//�������ݿ�
const TCHAR szExerciseDB[]=TEXT("WHQJEducateDB"); 						//��ϰ���ݿ�
const TCHAR szGameScoreDB[]=TEXT("WHQJGameScoreDB");					//��ϰ���ݿ�
const TCHAR szGroupDB[] = TEXT("WHQJGroupDB");							//Ⱥ�����ݿ�
//////////////////////////////////////////////////////////////////////////////////

//��Ȩ��Ϣ
const TCHAR szCompilation[]=TEXT("E4954CD9-7089-4F7D-85F6-7B6EA082312E");

//////////////////////////////////////////////////////////////////////////////////

#endif