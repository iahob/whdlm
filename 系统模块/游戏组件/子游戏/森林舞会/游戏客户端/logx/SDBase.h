/*!
!module SDBase�������
!moduletype common
������ṩ������������Ļ���ISDBase��
�����ֻ�о�̬����ģʽ���Ҳ�֧��ֱ�����á�
!inc SDBase.h
*/ 
#pragma once


namespace SGDP {

// �������÷�ʽ
#define SDAPI __stdcall


/*!
!macro SAFE_
һϵ�а�ȫ�ݻ�ָ������ĺ궨�塣
!inc SDBase.h
*/
#define SAFE_RELEASE(p)				{ if(p) { (p)->Release(); (p)=NULL; } }				//! ��ȫ���ͷŶ���
#define SAFE_DELETE(p)				{ if(p) { delete (p); (p)=NULL; } }					//! ��ȫ��ɾ��new�����ı���
#define SAFE_DELETE_ARRAY(p)		{ if(p) { delete [] (p); (p)=NULL; } }				//! ��ȫ��ɾ��new[]����������
//#define SAFE_PURGE(p)				{ while ((p)->Release()) {} (p)=NULL; }				// ! ��ȫ�ĳ����ͷŶ���


/*!struct
SGDP����汾�Žṹ��
�˽ṹ��ʶһ��SGDP����İ汾�š�
!inc SDBase.h
*/
struct SSGDPVersion
{
	unsigned short	wMajorVersion;		//! ���汾��
	unsigned short	wMinorVersion;		//! ���汾��
	unsigned short	wMinimumVersion;	//! ��С�汾��
	unsigned short	wBuildNumber;		//! Build���
};


/*!cls
���������Ļ��ࡣ
SGDPƽ̨�������������Ľӿ��඼�̳���ISDBase�ࡣ
!inc SDBase.h
*/
class ISDBase
{
public:
	ISDBase();


	//!group ���ü���
	/*!func
	���ӱ�ʵ�������ü�����
	*/
	virtual void SDAPI AddRef();

	/*!func
	��ѯ��ʵ����ǰ�����ü�����
	!return ��ʵ����ǰ�����ü�����
	*/
	virtual int  SDAPI QueryRef();

	/*!func
	�ͷ���ʵ����
	ÿ��������ʵ�ֶ��������ش˺����������������ͷš�
	Ϊ��ʵ�ֵ�ʵ�����˺������������ü���������ֱ���ͷ��ͷ���ʵ����
	ֻ�����ü���Ϊ0��ʱ�򣬴����ʵ���Ż������ı��ͷŵ���
	*/
	virtual void SDAPI Release();


	//!group �����Ϣ
	/*!func
	��ȡ����汾��Ϣ��
	ÿ������������ش˺��������ṩ������İ汾�š�
	!return ����汾�š���SSGDPVersion��
	*/
	virtual SSGDPVersion SDAPI GetVersion() = 0;

	/*!func
	��ȡ�������
	ÿ������������ش˺��������ṩ����������ơ�
	!return �������
	*/
	virtual const char * SDAPI GetModuleName() = 0;


protected:
	int m_iRef;
};

}