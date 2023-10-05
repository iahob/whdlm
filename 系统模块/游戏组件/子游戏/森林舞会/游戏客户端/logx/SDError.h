/*!
!module SDError���������
!moduletype common
SGDPƽ̨�Ļ��������������
��Ҫ�Ĺ����У�
.���������Ϣ�ʹ�����Ϣ��
.�����쳣�������û��趨�ķ�ʽ�����쳣��
.�ṩȫ�Զ����쳣�ύ���ơ�
 
�����ȫ������Ч�����̰߳�ȫ��
����Ӧ�ó����ʼ��SDError������������ڴ˽����ڷ����������쳣��������ʹ�õ���������������쳣�����������񲢰����û����趨����
SGDPƽ̨�������Ҳʹ��SDError�����������Ϣ���Դ�����ģʽ���޸Ľ�Ӱ��˽���ʹ�õ���������Ĵ������ģʽ��

�ڴ�����У����ǰ������Ϣ�ֳ�7�֣�
.Debug ������Ϣ��ȱʡ����ģʽ����DebugView�������Ϣ��
.Warn ������Ϣ��ȱʡ����ģʽ����DebugView�������Ϣ��
.Error ����δԤ�ڵ��쳣��һ����ָ������û��Ԥ�ϵ����쳣����û���ṩ��Ӧ���쳣������롣ȱʡ����ģʽ����DebugView�������Ϣ���׳��쳣����SDError���쳣������ƽ��д���
.FatalError ���ش���һ��ָ������Ԥ�ϵ��Ŀ��ܳ��ֵĵ��³����ܼ������еĴ��󣬱����û�û�а�װDirectX��ȱʡ����ģʽ��ʹ��MessageBox�����Ϣ��ֱ���˳����򣬲��׳��쳣��
.Log ��¼��Ϣ����Ҫ������¼����Ϣ��
.RemoteLog Զ�̼�¼��Ϣ����Ҫ��¼��Զ�̷������ϵ���Ϣ��
.Dump ����ִ��״̬��Ϣ����������ִ��״̬��ִ�л�������Ϣ��
 
������ľ���ʹ��ʾ����!!!link \ʾ��\���ʾ��\SDError@Demo1@�쳣����.htm SDError Demo1 �쳣����!!!��

!inc SDError.h
*/ 

#pragma once
#include "SDBase.h"
#include <Winnt.h>


namespace SGDP {

// ģ������
const char SDERROR_MODULENAME[] = "SDError";

// �汾��
const SSGDPVersion	SDERROR_VERSION = {0,3,0,1};


//=============================================================================================================================
/*! 
!macro ST_
�쳣����ꡣ
һ������£��û�����ʹ����Щ�ꡣֻ�е��û�ϣ�����Լ��ķ�ʽ�����쳣ʱ����Ҫ�õ���Щ�쳣����ꡣ
����ʹ��ʾ����!!!link \ʾ��\���ʾ��\SDError@Demo1@�쳣����.htm SDError Demo1 �쳣����!!!��
!inc SDError.h
*/
#define STB						try {									//! �쳣����ʼ
#define STE						} catch(...) { }						//! �쳣�������������ڴ��ڼ䷢���쳣�����̵����쳣�������κδ���ͱ��档
#define STER(s)					} catch(...) { return s; }				//! �쳣�������������ڴ��ڼ䷢���쳣�����̵����쳣�������κδ���ͱ��档Ȼ��return s��
#define STEI(s)					} catch(...) { SDErrorCatch(1,s); 		//! �쳣�������������ڴ��ڼ䷢���쳣��������һ��Error�¼���
#define STEF(s)					} catch(...) { SDErrorCatch(2,s); }		//! �쳣�������������ڴ��ڼ䷢���쳣��������һ��FatalError�¼���


#define SDERROR_ADDRFLAG				"[@P#]"					// ���������ʶһ����ַ���˵�ַ������ת�����Զ�ת��ΪԴ�����ļ������к�


/*! 
!macro SDERROR_TYPE_
����������͡�
!inc SDError.h
*/
#define SDERROR_TYPE_DEBUG					0				//! ����
#define SDERROR_TYPE_WARN					1				//! ����
#define SDERROR_TYPE_FATALERROR				2				//! ���ش���
#define SDERROR_TYPE_ERROR					3				//! �쳣
#define SDERROR_TYPE_LOG					4				//! LOG
#define SDERROR_TYPE_REMOTELOG				5				//! Զ��LOG
#define SDERROR_TYPE_DUMP					6				//! DUMP
#define SDERROR_TYPE_MAX					7
#define SDERROR_TYPE_UNKNOW					0xFFFF


/*! 
!macro SDERROR_OUTPUT_
������ģʽ��
�趨Debug��Warn��Error��FatalErrorģʽ�£���Ϣ����ͳ������еķ�ʽ��
��Щ�������OR�ķ�ʽ�������ʹ�á�
����ʹ��ʾ����!!!link \ʾ��\���ʾ��\SDError@Demo1@�쳣����.htm SDError Demo1 �쳣����!!!��
!inc SDError.h
*/
#define SDERROR_OUTPUT_NOTHROW				0x00000001		//! ����������������ļ��б�ʶΪN
#define SDERROR_OUTPUT_THROW				0x00000002		//! ������׳��쳣�������ļ��б�ʶΪT
#define SDERROR_OUTPUT_EXIT					0x00000004		//! ������˳��������ļ��б�ʶΪE
#define SDERROR_OUTPUT_MESSAGEBOX			0x00000100		//! �ԶԻ�����ʽ������������ļ��б�ʶΪM
#define SDERROR_OUTPUT_DEBUGWND				0x00000200		//! ��DebugView�����������Ϣ�������ļ��б�ʶΪD
#define SDERROR_OUTPUT_ERRORDUMP			0x00000400		//! �Ѵ�����Ϣ���͸��������������ļ��б�ʶΪO
#define SDERROR_OUTPUT_USERDUMP				0x00000800		//! ���û���Ϣ���͸��������������ļ��б�ʶΪU
#define SDERROR_OUTPUT_CRASHDUMP			0x00001000		//! �ѱ�����Ϣ���͸�������������ģʽ�����ڲ�ʹ�ã������ļ��б�ʶΪC
#define SDERROR_OUTPUT_LOGFILE				0x00010000		//! ��LOG��ʽ������������ļ��б�ʶΪL
#define SDERROR_OUTPUT_REMOTELOG			0x00020000		//! ��LOG��ʽ���������Ϣ��Զ�̷������������ļ��б�ʶΪR


/*! 
!macro SDERROR_DUMPFORMAT_
���͸���������DUMP������������ݡ�
!inc SDError.h
*/
#define SDERROR_DUMPFORMAT_SMALL			0x00000001		//! ��С�ı������棬ֻ�������ö�ջ��Ϣ�������ļ��б�ʶΪS
#define SDERROR_DUMPFORMAT_NORMAL			0x00000002		//! �еȳߴ�ı������棬���������Ķ�ջ��Ϣ�������ļ��б�ʶΪN
#define SDERROR_DUMPFORMAT_FULL				0x00000004		//! �����ı������棬���������û��ڴ棬�����ļ��б�ʶΪF
#define SDERROR_DUMPFORMAT_SYSTEMINFO1		0x00000100		//! �����ͻ��˻�����Ϣ1��Ϊ�ܹ����ٵõ�����Ϣ�������ļ��б�ʶΪA
#define SDERROR_DUMPFORMAT_SYSTEMINFO2		0x00000200		//! �����ͻ��˻�����Ϣ2��Ϊ��Ҫһ��ʱ��õ�����Ϣ�������ļ��б�ʶΪB
#define SDERROR_DUMPFORMAT_SCREENSHOT_HIGH	0x00001000		//! �����ͻ��������ͼ���߾��ȣ������ļ��б�ʶΪH
#define SDERROR_DUMPFORMAT_SCREENSHOT_MED	0x00002000		//! �����ͻ��������ͼ���еȾ��ȣ������ļ��б�ʶΪM
#define SDERROR_DUMPFORMAT_SCREENSHOT_LOW	0x00004000		//! �����ͻ��������ͼ���;��ȣ������ļ��б�ʶΪL
#define SDERROR_DUMPFORMAT_EXTRAINFO		0x00010000		//! �������Ϣ�������ļ��б�ʶΪE
#define SDERROR_DUMPFORMAT_USERINFO1		0x00020000		//! �����û��������Ϣ1�������ļ��б�ʶΪ1
#define SDERROR_DUMPFORMAT_USERINFO2		0x00040000		//! �����û��������Ϣ2�������ļ��б�ʶΪ2
#define SDERROR_DUMPFORMAT_USERINFO3		0x00080000		//! �����û��������Ϣ3�������ļ��б�ʶΪ3


/*! 
!macro SDERROR_LOGFORMAT_
Log�ļ���֯��ʽ��
!inc SDError.h
*/
#define SDERROR_LOGFORMAT_SINGLE			0x00000001		//! ���м�¼�����һ���ļ�������ļ��б�ʶΪS
#define SDERROR_LOGFORMAT_DIVIDEBYMODULE	0x00000002		//! ����ģ�������ļ���ţ������ļ��б�ʶΪM
#define SDERROR_LOGFORMAT_DIVIDEBYTIME		0x00000004		//! ����ʱ����ļ���ţ������ļ��б�ʶΪT����ģʽ�����SDERROR_LOGFORMAT_DIVIDEBYMODULEģʽͬʱʹ��


/*! 
!macro SDERROR_VERSION_
�汾�ȶԽ����
!inc SDError.h
*/
#define SDERROR_VERSION_OLD					-2				//! ���������汾�뵱ǰʵ������汾�����ݣ���ǰ�汾����
#define SDERROR_VERSION_NEW					-1				//! ���������汾�뵱ǰʵ������汾�����ݣ���ǰ�汾����
#define SDERROR_VERSION_COMPATIBLE			1				//! ���������汾�뵱ǰʵ������汾����
#define SDERROR_VERSION_OK					2				//! ���������汾�뵱ǰʵ������汾��ȫ��ͬ


//=============================================================================================================================
/*!typedef
�˳��ص��������Ͷ��塣
�������쳣�󣬰����û��趨������������ر�Ӧ�ó���ǰ�������ô˻ص�������
�û������ڴ˻ص�����������Դ�ͷŵȹ�����
!inc SDError.h
*/
typedef void (*SDERROR_EXIT_CB)();		

/*!typedef
��Ϣ���ͻص��������Ͷ��塣
����Ҫ����Ϣ�ύ��������ʱ������ô˻ص�������
����Ϊһ��1024�ֽڳ��ȵ��ַ������Ѿ�����˶�Ҫ�ύ��Ϣ��������
�û����԰�Ҫ���ӵ���Ϣ���ڴ��ַ����ĺ��档
!inc SDError.h
*/
typedef void (*SDERROR_SENDERROR_CB)(char *);



//=============================================================================================================================
/*!cls
����쳣����׷�١�����ȹ��ܵ��ࡣ
!inc SDError.h
*/
class ISDError : public ISDBase
{
public:
	/************************************************************************************/
	//!group ��ʼ��
	/*!func
	��ʼ�������������
	!return	��ʼ���Ƿ�ɹ���
	*/
	virtual bool 			SDAPI	InitErrorModule		() = 0;


	/************************************************************************************/
	//!group ���������Ϣ�����ģʽ
	/*!func
	���ô��������Ϣ�����ģʽ��
	!param [in] ������������Ϣ��������ͣ���SDERROR_TYPE_��
	!param [in] ������������Ϣ�����ģʽ����SDERROR_OUTPUT_��
	!return	�����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	SetOutputMode		(unsigned long dwType, unsigned long dwMode) = 0;

	/*!func
	��ȡ��ǰ���������Ϣ�����ģʽ��
	!param [in] ������������Ϣ��������ͣ���SDERROR_TYPE_��
	!return	�������ʧ�ܣ�����0�����򷵻ش�����������Ϣ�����ģʽ����SDERROR_OUTPUT_��
	*/
	virtual unsigned long	SDAPI	GetOutputMode		(unsigned long dwType) = 0;


	/*!func
	������Ϣ����Ի�������ģʽ��
	!param [in] ��Ϣ����Ի���ı��⡣
	*/
	virtual void			SDAPI	SetMessageBoxFormat	(const char * strTitle) = 0;

	/*!func
	��ȡ��ǰ��Ϣ����Ի�������ģʽ��
	!return	�������ʧ�ܣ�����NULL�����򷵻���Ϣ����Ի���ı��⡣
	*/
	virtual const char *	SDAPI	GetMessageBoxFormat	() = 0;


	/*!func
	�����˳������ģʽ��
	!param [in] �˳�ǰ���õĻص���������SDERROR_EXIT_CB��
	*/
	virtual void			SDAPI	SetExitFormat		(SDERROR_EXIT_CB pfnExitCB) = 0;

	/*!func
	��ȡ��ǰ�˳������ģʽ.
	!return	�������ʧ�ܣ�����NULL�����򷵻��˳�ǰ���õĻص���������SDERROR_EXIT_CB��
	*/
	virtual SDERROR_EXIT_CB	SDAPI	GetExitFormat		() = 0;


	/*!func
	����LOG��ģʽ��
	!param [in] Logģʽ����SDERROR_LOGFORMAT_��
	!param [in] Log�ļ����ļ�����
	*/
	virtual void			SDAPI	SetLogFormat		(unsigned long dwFormat, const char * strLogFile=0 ) = 0;

	/*!func
	��ȡ��ǰLOG��ģʽ��
	!param [out] Log�ļ����ļ�����
	!return Logģʽ����SDERROR_LOGFORMAT_��
	*/
	virtual unsigned long	SDAPI	GetLogFormat		(const char ** pstrLogFile) = 0;


	/*!func
	����Զ��LOG��ģʽ��
	!param [in] LOG������������ʱip��ַ����������ַ�Ͷ˿ںţ������÷ֺŷָ�����������ַ�����磺www.snda.com/log.asp;202.100.96.233:1234/remotelog.php
	*/
	virtual void			SDAPI	SetRemoteLogFormat	(const char * strServerAddr) = 0;

	/*!func
	��ȡ��ǰԶ��LOG��ģʽ��
	!return LOG��������
	*/
	virtual const char *	SDAPI	GetRemoteLogFormat	() = 0;


	/*!func
	���ñ���DUMP�����ģʽ��
	!param [in] ����DUMP�����ģʽ����SDERROR_DUMPFORMAT_��
	!param [in] �������������������ʱip��ַ����������ַ�Ͷ˿ںţ������÷ֺŷָ�����������ַ�����磺www.snda.com/bugreport.asp;202.100.96.233:1234/crash.php
	!param [in] ����û���Ϣǰ�Ļص���������SDERROR_SENDERROR_CB��
	!param [in] �û���Ϣ����Ի��������˵������󳤶�256���֡�
	!param [in] �û���Ϣ����Ի���ĵ�1��������˵������󳤶�32���֡�
	!param [in] �û���Ϣ����Ի���ĵ�2��������˵������󳤶�32���֡�
	!param [in] �û���Ϣ����Ի���ĵ�3��������˵������󳤶�64���֡�
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	SetCrashDumpFormat	(unsigned long dwFormat, const char * strServerAddr, SDERROR_SENDERROR_CB pfnSendErrorCB=0, const char * strTitle=0, const char * strInfo1=0, const char * strInfo2=0, const char * strInfo3=0) = 0;

	/*!func
	��ȡ����DUMP�����ģʽ��
	!param [out] ����DUMP�����ģʽ����SDERROR_DUMPFORMAT_��
	!param [out] ���������������
	!param [out] ����û���Ϣǰ�Ļص���������SDERROR_SENDERROR_CB��
	!param [out] �û���Ϣ����Ի��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�1��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�2��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�3��������˵����
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	GetCrashDumpFormat	(unsigned long * pdwFormat, const char ** pstrServerAddr, SDERROR_SENDERROR_CB * pfnSendErrorCB=0, const char ** pstrTitle=0, const char ** pstrInfo1=0, const char ** pstrInfo2=0, const char ** pstrInfo3=0) = 0;


	/*!func
	���ô���DUMP�����ģʽ��
	!param [in] ����DUMP�����ģʽ����SDERROR_DUMPFORMAT_��
	!param [in] �������������������ʱip��ַ����������ַ�Ͷ˿ںţ������÷ֺŷָ�����������ַ�����磺www.snda.com/bugreport.asp;202.100.96.233:1234/crash.php
	!param [in] ����û���Ϣǰ�Ļص���������SDERROR_SENDERROR_CB��
	!param [in] �û���Ϣ����Ի��������˵������󳤶�256���֡�
	!param [in] �û���Ϣ����Ի���ĵ�1��������˵������󳤶�32���֡�
	!param [in] �û���Ϣ����Ի���ĵ�2��������˵������󳤶�32���֡�
	!param [in] �û���Ϣ����Ի���ĵ�3��������˵������󳤶�64���֡�
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	SetErrorDumpFormat	(unsigned long dwFormat, const char * strServerAddr, SDERROR_SENDERROR_CB pfnSendErrorCB=0, const char * strTitle=0, const char * strInfo1=0, const char * strInfo2=0, const char * strInfo3=0) = 0;

	/*!func
	��ȡ����DUMP�����ģʽ��
	!param [out] ����DUMP�����ģʽ����SDERROR_DUMPFORMAT_��
	!param [out] ���������������
	!param [out] ����û���Ϣǰ�Ļص���������SDERROR_SENDERROR_CB��
	!param [out] �û���Ϣ����Ի��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�1��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�2��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�3��������˵����
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	GetErrorDumpFormat	(unsigned long * pdwFormat, const char ** pstrServerAddr, SDERROR_SENDERROR_CB * pfnSendErrorCB=0, const char ** pstrTitle=0, const char ** pstrInfo1=0, const char ** pstrInfo2=0, const char ** pstrInfo3=0) = 0;

	
	/*!func
	�û�DUMP�����ģʽ��
	!param [in] ����DUMP�����ģʽ����SDERROR_DUMPFORMAT_��
	!param [in] �������������������ʱip��ַ����������ַ�Ͷ˿ںţ������÷ֺŷָ�����������ַ�����磺www.snda.com/bugreport.asp;202.100.96.233:1234/crash.php
	!param [in] ����û���Ϣǰ�Ļص���������SDERROR_SENDERROR_CB��
	!param [in] �û���Ϣ����Ի��������˵������󳤶�256���֡�
	!param [in] �û���Ϣ����Ի���ĵ�1��������˵������󳤶�32���֡�
	!param [in] �û���Ϣ����Ի���ĵ�2��������˵������󳤶�32���֡�
	!param [in] �û���Ϣ����Ի���ĵ�3��������˵������󳤶�64���֡�
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	SetUserDumpFormat	(unsigned long dwFormat, const char * strServerAddr, SDERROR_SENDERROR_CB pfnSendErrorCB=0, const char * strTitle=0, const char * strInfo1=0, const char * strInfo2=0, const char * strInfo3=0) = 0;

	/*!func
	��ȡ�û�DUMP�����ģʽ��
	!param [out] ����DUMP�����ģʽ����SDERROR_DUMPFORMAT_��
	!param [out] ���������������
	!param [out] ����û���Ϣǰ�Ļص���������SDERROR_SENDERROR_CB��
	!param [out] �û���Ϣ����Ի��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�1��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�2��������˵����
	!param [out] �û���Ϣ����Ի���ĵ�3��������˵����
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	GetUserDumpFormat	(unsigned long * pdwFormat, const char ** pstrServerAddr, SDERROR_SENDERROR_CB * pfnSendErrorCB=0, const char ** pstrTitle=0, const char ** pstrInfo1=0, const char ** pstrInfo2=0, const char ** pstrInfo3=0) = 0;


	/*!func
	���ò�Ʒ��Ϣ��
	���õ�ǰ��Ʒ�Ĳ�Ʒ���ƣ����кţ��汾�š�
	!param [in] ��Ʒ���ơ�
	!param [in] ��Ʒ���кš�
	!param [in] ��Ʒ�汾�š�
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	SetProductInfo		(const char * strProduct, const char * strSeries, const char * strVersion) = 0;

	/*!func
	���ò�Ʒ��Ϣ��
	ʹ�������ļ��е���Ϣ���õ�ǰ��Ʒ�Ĳ�Ʒ���ƣ����кţ��汾�š�
	!param [in] ��Common���а���Product��Series��Ϣ�������ļ���
	!param [in] ��Common���а���Version��Ϣ�������ļ���
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	SetProductInfo		(const char * strConfigFile = 0, const char * strVersionFile = 0) = 0;

	/*!func
	�������ļ���ȡ������Ϣ��
	!param [in] �����ļ��ļ�����
	!return �����Ƿ�ɹ���
	*/
	virtual bool			SDAPI	LoadConfigFromIni	(const char * strIniFile = 0) = 0;


	/************************************************************************************/
	//!group ��Ϣ���
	/*!func
	���档
	!param [in] Ҫ�������Ϣ��
	!param [in] �����Ϣ���������
	*/
	virtual void			SDAPI	Warn				(const char * strInfo, const char * strModuleName = 0) = 0;

	/*!func
	δԤ�ڵĴ���
	!param [in] Ҫ�������Ϣ��
	!param [in] �����Ϣ���������
	*/
	virtual void			SDAPI	Error				(const char * strInfo, const char * strModuleName = 0) = 0;

	/*!func
	Ԥ�ڵĴ���
	!param [in] Ҫ�������Ϣ��
	!param [in] �����Ϣ���������
	*/
	virtual void			SDAPI	FatalError			(const char * strInfo, const char * strModuleName = 0) = 0;

	/*!func
	���ԡ�
	!param [in] Ҫ�������Ϣ��
	!param [in] �����Ϣ���������
	*/
	virtual void			SDAPI	Debug				(const char * strInfo, const char * strModuleName = 0) = 0;	

	/*!func
	��¼��
	!param [in] Ҫ�������Ϣ��
	!param [in] �����Ϣ���������
	*/
	virtual void			SDAPI	Log					(const char * strInfo, const char * strModuleName = 0) = 0;

	/*!func
	Զ�̼�¼��
	!param [in] Ҫ�������Ϣ��
	!param [in] �����Ϣ���������
	*/
	virtual void			SDAPI	RemoteLog			(const char * strInfo, const char * strModuleName = 0) = 0;

	/*!func
	�û�DUMP��
	!param [in] Ҫ�������Ϣ��
	!param [in] �����Ϣ���������
	*/
	virtual void			SDAPI	Dump				(const char * strInfo, const char * strModuleName = 0) = 0;


	/************************************************************************************/
	//!group �汾�Ƚ�
	/*!func
	�汾�Ƚϣ��ж�����汾���Ƿ������û�Ҫ��
	!param [in] �û�Ҫ�������汾�š�
	!param [in] ��ǰʵ������汾�š�
	!return	�汾���Ƿ�ƥ�䣬��SDERROR_VERSION_��
	*/
	virtual int				SDAPI	VerCheck			(const SSGDPVersion * pRequestVer, const SSGDPVersion * pRealVer) = 0;


	/************************************************************************************/
	// DUMP��ǰ��������״̬���˺��������ڲ�ʹ��
	virtual bool			SDAPI	DumpProgram			(int iDumpType, _EXCEPTION_POINTERS * pExceptionInfo=0, const char * strInfo=0) = 0;
};


//=============================================================================================================================
// �ṩ��STEI,STEFʹ�õĴ�������
void SDAPI SDErrorCatch(int iMode,const char * strInfo);	


//=============================================================================================================================
/*!func
�˺������ش��������SDError��ʵ����
!param [in] �û�Ҫ��������Ͱ汾�ţ���SSGDPVersion��
!return ����ɹ������ش������ʵ�������ʧ�ܷ��ؿ�ָ�롣
!inc SDError.h
*/
ISDError * SDAPI SDErrorGetModule(const SSGDPVersion *pstVersion);

/*!typedef
SDErrorGetModule ���������Ͷ��塣
!inc SDError.h
*/
typedef ISDError * (SDAPI *PFN_SDErrorGetModule)(const SSGDPVersion *pstVersion);


/*!func
�˺������ش��������SDError��ʵ��������֤�汾�š�
!return ����ɹ������ش������ʵ�������ʧ�ܷ��ؿ�ָ�롣
!inc SDError.h
*/
ISDError * SDAPI SDErrorInstance();

/*!typedef
SDErrorInstance ���������Ͷ��塣
!inc SDError.h
*/
typedef ISDError * (SDAPI *PFN_SDErrorInstance)();

}
