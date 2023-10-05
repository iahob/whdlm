#ifndef GUID_8E861A52_58CD_4048_8978_187B0BE17989
#define GUID_8E861A52_58CD_4048_8978_187B0BE17989

#pragma once

#include "DirectUIWindow.h"

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_begin

//////////////////////////////////////////////////////////////////////////////////

// ���ֽṹ
struct EditText
{
	TCHAR				Text[2];
	CSize				Size;
};

// ���������
class DEXTER_D3D_CLASS CDirectUIEdit : public CDirectUIWindow
{

	// ��ʾ����
protected:
	LOGFONT											m_LogFont;					// ������Ϣ
	CD3DFont										m_EditFont;					// ��������
	CArray<EditText*>								m_ArrayText;				// ��������
	CArray<EditText*>								m_ArrayBuffer;				// ���ֻ���
	int												m_nBeginShow;				// ��ʼ��ʾλ��
	int												m_nEndShow;					// ������ʾλ��
	int												m_nBeginSelect;				// ��ʼѡ��λ��
	int												m_nEndSelect;				// ����ѡ��λ��
	int												m_nStaySite;				// ͣ��ְλ

	// ���ñ���
protected:
	bool											m_bOnlyNumber;				// ֻ����������
	bool											m_bNumberPoint;				// ��������С���
	bool											m_bPassword;				// ����
	int												m_nPasswordOffset;			// ����ƫ����

	// ��궯��
protected:
	bool											m_bMouseDown;				// �����

	// ���ͣ��
protected:
	int												m_nStayInterval;			// ͣ��ʱ��
	int												m_nStayTime;				// ͣ��ʱ��
	CSize											m_SizeStay;					// ͣ����С

	// ѡ���ƶ�
protected:
	bool											m_bLeftSelect;				// ��ѡ��
	bool											m_bRightSelect;				// ��ѡ��
	int												m_nMoveSelectTime;			// �ƶ�ѡ��ʱ��
	CLapseCount										m_SelectLapseCount;			// ѡ���ʱ

	// ��ɫ����
protected:
	D3DCOLOR										m_dwBackColor;				// ������ɫ
	D3DCOLOR										m_dwFontColor;				// ������ɫ
	D3DCOLOR										m_dwSelectBackColor;		// ѡ�б�����ɫ
	D3DCOLOR										m_dwSelectFontColor;		// ѡ��������ɫ
	D3DCOLOR										m_dwStayColor;				// ͣ����ɫ

	// ��������
public:
	// ���캯��
	CDirectUIEdit(void);
	// ��������
	virtual ~CDirectUIEdit(void);

	// ϵͳ�¼�
protected:
	// ������Ϣ
	void OnWindowMovie();
	// ������Ϣ
	void OnWindowCreate(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// ������Ϣ
	void OnWindowDestory(CD3DDevice * pD3DDevice);
	// λ����Ϣ
	void OnWindowPosition(CD3DDevice * pD3DDevice, int nPosX, int nPosY);
	// �滭����
	void OnWindowRender(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// ��Ϣ���غ���
protected:
	// ��Ϣ����
	virtual bool DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	// ��Ϣ����
	virtual bool PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);

	// �����Ϣ
protected:
	// ����ƶ� 
	void OnMouseMove(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ���̧�� 
	void OnLButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ������� 
	void OnLButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ���˫�� 
	void OnLButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);

	// ������Ϣ
protected:
	// �������� 
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
	// �������� 
	void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags);
	// �ַ���Ϣ 
	void OnChar(uint nChar, uint nRepCnt, uint nFlags);	

	// ��������
public:
	// ��������
	void SetFont( CD3DFont& font );
	// ����ֻ����������
	void SetOnlyNumber( bool bOnlyNumber, bool bNumberPoint = false ) { m_bOnlyNumber = bOnlyNumber; m_bNumberPoint = bNumberPoint; }
	// ��������״̬
	void SetPassword( bool bPassword ) { m_bPassword = bPassword; }
	// ���ñ�����ɫ
	void SetBackColor( D3DCOLOR dwBackColor ) { m_dwBackColor = dwBackColor; }
	// ����������ɫ
	void SetFontColor( D3DCOLOR dwFontColor ) { m_dwFontColor = dwFontColor; }
	// ����ѡ�б�����ɫ
	void SetSelectBackColor( D3DCOLOR dwSelectBackColor ) { m_dwSelectBackColor = dwSelectBackColor; }
	// ����ѡ��������ɫ
	void SetSelectFontColor( D3DCOLOR dwSelectFontColor ) { m_dwSelectFontColor = dwSelectFontColor; }
	// ����ͣ����ɫ
	void SetStayColor( D3DCOLOR dwStayColor ) { m_dwStayColor = dwStayColor; }
	// �����ı�
	void SetEditText( CString StrText );
	// ��ȡ�ı�
	void GetEditText( CString& StrText );

	// �ڲ�����
protected:
	// ��ȡ�»���
	EditText* AccessBuffer( TCHAR szText );
	// ɾ������
	void DeleteBuffer( EditText* pEditText );
	// ��ȡѡȡ����
	int	GetSelectIndex( CPoint ptMouse, bool bSeparationOrSo );
	// ������������
	void CopyToClipboard();
	// ��������������
	int ClipboardCount();
	// ճ��������
	void PasteFromClipboard();
	// ɾ��ѡ���ַ�
	void DeleteSelectText();
	// ������ʾλ��
	void UpdateShowSite();
	// ������ʾλ��
	void ResetShowSite();

	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif