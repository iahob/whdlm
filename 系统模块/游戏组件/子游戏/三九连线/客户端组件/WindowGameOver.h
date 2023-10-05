#pragma once

class CWindowGameOver : public CVirtualWindow
{
	// ������Ϣ
public:
	LONGLONG						m_lPlayScore;							// ��һ���
	LONGLONG						m_lPlayWinLose;							// �����Ӯ��

	// ��Դ��Ϣ
public:
	CD3DTexture						m_ImageBack;							// ����
	CD3DTexture						m_ImageNumer;							// ����
	CVirtualButton					m_btClosed;								// �رհ�ť

	// ���캯��
public:
	CWindowGameOver(void);
	~CWindowGameOver(void);

	// ϵͳ�¼�
protected:
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

	// ���溯��
private:
	// �滭����
	void DrawNumber(CD3DDevice * pGLDevice , CD3DTexture* ImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT, BYTE cbAlpha = 255);	

	// ���ú���
public:
	// ���û���
	void SetOverInfo( LONGLONG lPlayScore, LONGLONG lPlayWinLose ) { m_lPlayScore = lPlayScore; m_lPlayWinLose = lPlayWinLose; }

};
