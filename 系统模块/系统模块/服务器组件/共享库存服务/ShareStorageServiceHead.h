//�ر���IT������28xin.com��
#ifndef SHARE_STORAGE_SERVICE_HEAD_FILE
#define SHARE_STORAGE_SERVICE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"

//ƽ̨�ļ�
#include "..\..\�������\�������\ServiceCoreHead.h"
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

#ifdef _DEBUG		
#define		MAX_SHARED_BUFFER				10240000
#else
#define		MAX_SHARED_BUFFER				102400000
#endif

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef SHARE_STORAGE_SERVICE_CLASS
	#ifdef  SHARE_STORAGE_SERVICE_DLL
		#define SHARE_STORAGE_SERVICE_CLASS _declspec(dllexport)
	#else
		#define SHARE_STORAGE_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define SHARE_STORAGE_SERVICE_DLL_NAME		TEXT("ShareStorageService.dll")			//�������
#else
	#define SHARE_STORAGE_SERVICE_DLL_NAME		TEXT("ShareStorageServiceD.dll")		//�������
#endif

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//Լս������
struct tagShareStorageManagerParameter
{
	tagGameServiceOption *			pGameServiceOption;					//��������
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IShareStorageServiceManager	INTERFACE_VERSION(1,1)
static const GUID IID_IShareStorageServiceManager = { 0x55f92262, 0x191e, 0x461e, 0x96, 0x2a, 0xe7, 0x14, 0xaf, 0x1e, 0x85, 0x92 };
#else
#define VER_IShareStorageServiceManager	INTERFACE_VERSION(1,1)
static const GUID IID_IShareStorageServiceManager={0x35053879, 0x45ed, 0x4c8c, 0xbe, 0x1, 0x1f, 0xa6, 0x29, 0x17, 0x25, 0x5c};
#endif

//���������ӿ�
interface IShareStorageServiceManager : public IUnknownEx
{
	//���ƽӿ�
public:
	//ֹͣ����
	virtual bool StopService(WORD wServerID, WORD wKindID, BYTE cbPlayMode) = NULL;
	//��������
	virtual bool StartService(WORD wServerID, WORD wKindID, BYTE cbPlayMode) = NULL;
	//��ȡ״̬
	virtual ENUM_SHARESTORAGE_STATUS GetShareConfigStatus() = NULL;
	//����״̬
	virtual void SetShareConfigStatus(ENUM_SHARESTORAGE_STATUS emShareConfigStatus) = NULL;

	//����ӿ�
public:
	//ͬ������
	virtual bool SyncShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem) = NULL;
	//�������
	virtual bool AddSyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem) = NULL;
	//�޸�����
	virtual bool ModifySyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem) = NULL;
	//�Ƴ�����
	virtual bool RemoveSyncShareStorageItem(ULONGLONG ulItemID) = NULL;
	//��������
	virtual bool LocalShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem) = NULL;
	//�������
	virtual bool AddShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem) = NULL;
	//�޸�����
	virtual bool ModifyShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem) = NULL;
	//�Ƴ�����
	virtual bool RemoveShareStorageArrayItem(ULONGLONG ulItemID) = NULL;
	//��ȡ����
	virtual tagShareStorageItem* GetShareStorageItem(ULONGLONG ulItemID) = NULL;
	//У��ͬ��
	virtual bool EfficacySyncShareStorageItem(WORD wServerID, WORD wKindID, BYTE cbPlayMode) = NULL;
};

///////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef SHARE_STORAGE_SERVICE_DLL
	#include "ShareStorageServiceManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif