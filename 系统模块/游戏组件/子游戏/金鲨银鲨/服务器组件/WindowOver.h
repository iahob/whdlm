#pragma once

class CWindowOver : public CVirtualWindow
{
	//������Ϣ
public:
	LONGLONG							m_lPlayScore;			//��ҵ÷�

	//����ͼƬ
public:
	CD3DTexture					m_ImageBack;			//����ͼƬ
	CD3DTexture					m_ImageNumber;			//����ͼƬ

	//���캯��
public:
	CWindowOver(void);
	~CWindowOver(void);

	//ϵͳ�¼�
protected:
	//������Ϣ
	virtual VOID OnWindowMovie();
	//������Ϣ
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);

	//���غ���
protected:
	//����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos);
	//��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos);
	//�滭����
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos);

	// ���溯��
private:
	// �滭����
	void DrawNumber(CD3DDevice * pD3DDevice , CD3DTexture* pImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT, BYTE cbAlpha = 255);	
	//�滭�ַ�
	VOID DrawTextString(CD3DDevice* pD3DDevice, CD3DFont * pD3DFont,  LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame);

	//���ú���
public:
	//���û���
	void SetPlayScore( LONGLONG lPlayScore ) { m_lPlayScore = lPlayScore; }
};
