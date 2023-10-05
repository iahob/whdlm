#pragma once

// ���д���
class CWindowBank : public dex::CDirectUIWindow
{

	// ��Ϣ����
public:
	bool									m_bPlaySave;				// ������
	longlong								m_lUserScore;				// �û�����
	longlong								m_lUserInsure;				// ���з���

	// ���ڱ���
public:
	dex::CD3DTexture						m_TextureBack;				// ���б���
	dex::CD3DTexture						m_TextureNumber;			// ��������
	dex::CD3DTexture						m_TextureMask;				// �����ɰ�
	dex::CDirectUIEdit						m_EditOperating;			// ��������
	dex::CDirectUIEdit						m_EditPassword;				// ���봰��
	dex::CDirectUICheck						m_CheckSave;				// ��ť
	dex::CDirectUICheck						m_CheckTake;				// ȡ�ť
	dex::CDirectUIButton					m_ButtonOK;					// ȷ����ť
	dex::CDirectUIButton					m_ButtonClose;				// �رհ�ť


	// �ຯ��
public:
	// ���캯��
	CWindowBank();
	// ��������
	virtual ~CWindowBank();

	// ��Ϣ����
public:
	// ���ڴ��� 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ���ڶ���
	void OnWindowMovie();
	// ���ڻ滭 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ������ʾ 
	void OnWindowShow( dex::CD3DDevice * pD3DDevice, bool bVisible );

	// ��Ϣ����
public:
	// ȷ�� 
	void OnClickedOK();
	// �ر�
	void OnClickedClose();
	// ��ѡ��ť
	void OnClickedCheck();
	// �����޸�
	void OnEditChangeOperating();

	// ���ܺ���
public:
	// �������з���
	void SetBankScore( longlong lUserScore, longlong lUserInsure );

	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};
