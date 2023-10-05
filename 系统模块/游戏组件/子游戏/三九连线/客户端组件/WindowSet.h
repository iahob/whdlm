#pragma once
#include "ObjectMove.h"
#include "D3DTextureIndex.h"
#include "EditKey.h"

// ����ѡ��
enum enumSetType
{
	ST_Music,
	ST_Sound,
	ST_MAX,
};

// ��Ϸ����
class CWindowSet : public CVirtualWindow
{
	// ���ñ���
protected:
	INT						m_nCurrentVolume[ST_MAX];		// ������С
	INT						m_nLastVolume[ST_MAX];			// �ϴ�������С
	BOOL					m_bCurrentVoice[ST_MAX];		// ����
	BOOL					m_bLastVoice[ST_MAX];			// �ϴ�����

	// λ����Ϣ
protected:
	BOOL					m_bMouseDown[ST_MAX];			// �����
	BOOL					m_bMouseMove[ST_MAX];			// �����ͣ
	CPoint					m_ptLastSite[ST_MAX];			// �ϴα���λ��
	CPoint					m_ptLbuttonDown[ST_MAX];		// �����λ��
	CRect					m_rcMoveSelect[ST_MAX];			// ѡ��λ��

	// �ƶ���Ϣ
protected:	
	CObjectMove				m_stuMoveFish[ST_MAX];			// �����ƶ�

	// ��Դ����
protected:	
	CD3DTexture				m_ImageBack;					// ����
	CD3DTexture				m_ImageSetBars;					// ����
	CD3DTextureIndex		m_ImageSetMove;					// �ƶ�
	CVirtualButton			m_btNoVoice[ST_MAX];			// ������ť
	CVirtualButton			m_btVoice[ST_MAX];				// �Ǿ�����ť
	CVirtualButton			m_btClose;						// �رհ�ť
	CVirtualButton			m_btOk;							// �رհ�ť
	CVirtualButton			m_btDefaultKey;					// Ĭ�ϰ�ť

	// �Զ��尴ť����
public:
	UINT					m_nLastStartKey;				// �ϴα���
	UINT					m_nLastLineCountUpKey;			// �ϴα���
	UINT					m_nLastLineCountDownKey;		// �ϴα���
	UINT					m_nLastLineNumberUpKey;			// �ϴα���
	UINT					m_nLastLineNumberDownKey;		// �ϴα���

	CEditKey				m_EditStart;
	CEditKey				m_EditLineCountUp;
	CEditKey				m_EditLineCountDown;
	CEditKey				m_EditLineNumberUp;
	CEditKey				m_EditLineNumberDown;

public:
	CWindowSet(void);
	~CWindowSet(void);


	// ϵͳ�¼�
protected:
	// ������Ϣ
	virtual VOID OnWindowMovie();
	// ������Ϣ
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);
	// λ����Ϣ
	virtual VOID OnWindowPosition(CD3DDevice * pD3DDevice);

	// ���غ���
protected:
	// ����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);
	// ��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos);
	// �滭����
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos);

	// ���ڿ���
public:
	// ��ʾ����
	virtual VOID ShowWindow(bool bVisible);

	// ���ܺ���
public:
	// ��������
	int GetVolume( enumSetType emST ) { return m_nCurrentVolume[emST]; }
	// ��������
	BOOL GetVoice( enumSetType emST ) { return m_bCurrentVoice[emST]; }
	// ��������
	VOID SetVolume( int nVolume, enumSetType emST ) { m_nCurrentVolume[emST] = nVolume; }
	// ��������
	VOID SetVoice( BOOL bMute, enumSetType emST ) { m_bCurrentVoice[emST] = bMute; }
};
